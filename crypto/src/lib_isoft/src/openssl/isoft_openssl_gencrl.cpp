// Disclaimer
//
// This work (specification and/or software implementation) and the material
// contained in it, as released by AUTOSAR, is for the purpose of information
// only. AUTOSAR and the companies that have contributed to it shall not be
// liable for any use of the work.
//
// The material contained in this work is protected by copyright and other
// types of intellectual property rights. The commercial exploitation of the
// material contained in this work requires a license to such intellectual
// property rights.
//
// This work may be utilized or reproduced without any modification, in any
// form or by any means, for informational purposes only. For any other
// purpose, no part of the work may be utilized or reproduced, in any form
// or by any means, without permission in writing from the publisher.
//
// The work has been developed for automotive applications only. It has
// neither been developed, nor tested for non-automotive applications.
//
// The word AUTOSAR and the AUTOSAR logo are registered trademarks.
// --------------------------------------------------------------------------
//
// OpenSSL source attribution
//
// Portions of this file are derived from OpenSSL source code.
// OpenSSL 3.5.1 is used as the reference version for source attribution.
//
// Reference upstream source:
// https://github.com/openssl/openssl/blob/openssl-3.5.1/apps/ca.c
//
// Applicable copyright notice for the OpenSSL-derived portions:
// Copyright 1995-2024 The OpenSSL Project Authors. All Rights Reserved.
//
// The OpenSSL-derived portions are licensed under the Apache License 2.0.
// A copy of the license is provided in:
// LICENSES/LICENSE-Apache-2.0.txt
//
// The OpenSSL-derived portions have been modified for integration into the
// CAPI C++ and AUTOSAR-specific codebase.
//
// Nothing in the AUTOSAR notice above is intended to restrict the rights
// granted under the Apache License 2.0 for the OpenSSL-derived portions.
//
// --------------------------------------------------------------------------


/// ================================================================
///
/// File description:
/// ----------------
/// @file       isoft_openssl_gencrl.cpp
/// @brief      AutoSar-Crypto configuration
/// @details    Functions related to certificate revocation
/// @date       2023-11-28
/// @author     Che Jinzhao
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author       <th>Description
/// <tr><td>2023-11-28  <td>1.0.0    <td>Che Jinzhao        <td>Initial version creation
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Certificate Component/Certificate Revocation
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_03006
/// @unit_name=CRL
/// @endcode
///
/// ================================================================

#include "ara/crypto/openssl/isoft_openssl_gencrl.h"

#include <fcntl.h>
#include <openssl/asn1.h>
#include <openssl/bio.h>
#include <openssl/conf.h>
#include <openssl/conf_api.h>
#include <openssl/crypto.h>
#include <openssl/engine.h>
#include <openssl/err.h>
#include <openssl/lhash.h>
#include <openssl/pem.h>
#include <openssl/pkcs12.h>
#include <openssl/ui.h>
#include <openssl/x509v3.h>
#include <sys/stat.h>

#include <cctype>
#include <cstring>
#include <iostream>

#include "ara/core/string.h"
#include "ara/crypto/common/isoft_common_api.h"
#include "ara/crypto/common/isoft_data_type.h"
#include "ara/crypto/common/isoft_log_api.h"
#include "openssl/x509.h"

namespace ara {
namespace crypto {
namespace openssl {
namespace isoft_def {
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_04039
/// @needwork = dd
/// @endcode
char8_t const *g_CrlReasons[]{/* CRL reason strings */
                              "unspecified", "keyCompromise", "CACompromise", "affiliationChanged", "superseded",
                              "cessationOfOperation", "certificateHold", "removeFromCRL",
                              /* Additional pseudo reasons */
                              "holdInstruction", "keyTime", "CAkeyTime"};
/// @brief Parse revocation info string, extract and build ASN.1 data structures
/// @param prevtm [out] output parameter, returns parsed revocation time (ASN1_TIME format)
/// @param preason [out] output parameter, returns parsed revocation reason code
/// @param phold [out] output parameter, returns hold instruction ASN1_OBJECT object
/// @param pinvtm [out] output parameter, returns certificate invalidity time ASN1_GENERALIZEDTIME object
/// @param str [in] input string, format "time[,reason[,additional parameters]]"
/// @return success returns 1, failure returns 0
/// @note Caller is responsible for freeing returned ASN.1 object memory
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_04040
/// @trace_id_sr=SR_CRYPTO_03006
/// @unit_name=Unpack_revinfo
/// @needwork = dd
/// @endcode
int32_t Unpack_revinfo(ASN1_TIME **const prevtm,
                       int32_t *const preason,
                       ASN1_OBJECT **const phold,
                       ASN1_GENERALIZEDTIME **const pinvtm,
                       char8_t const *const str) noexcept
{
    char8_t *tmp{nullptr};                    // copy of input string
    char8_t *rTimeStr{nullptr};               // revocation time string part
    char8_t *reasonStr{nullptr};              // revocation reason string part
    char8_t *argStr{nullptr};                 // additional parameter string part
    char8_t *p{nullptr};                      // temporary pointer for string splitting
    int32_t reasonCode{-1};                   // parsed reason code
    int32_t ret{0};                           // return value
    ASN1_GENERALIZEDTIME *compTime{nullptr};  // temporary invalidity time object

    // Copy input string for modification
    tmp = OPENSSL_strdup(str);

    if (tmp == nullptr) {
        return Free_ASN1_Source(tmp, compTime, ret);
    }

    // Split string into parts
    p = strchr(tmp, ',');

    rTimeStr = tmp;

    if (nullptr != p) {
        *p = '\0';
        p++;
        reasonStr = p;
        p         = strchr(p, ',');
        if (nullptr != p) {
            *p     = '\0';
            argStr = p + 1;
        }
    }

    // Parse revocation time
    if (prevtm != nullptr) {
        *prevtm = ASN1_UTCTIME_new();
        if (*prevtm == nullptr) {
            return Free_ASN1_Source(tmp, compTime, ret);
        }
        if (ASN1_UTCTIME_set_string(*prevtm, rTimeStr) == 0) {
            return Free_ASN1_Source(tmp, compTime, ret);
        }
    }

    // Parse revocation reason and additional parameters
    if (reasonStr != nullptr) {
        ASN1_OBJECT *hold{nullptr};  // temporary hold instruction object
        reasonCode = Get_ReasonCode(reasonStr, argStr, &hold, phold, &compTime);
        if (-1 == reasonCode) {
            return Free_ASN1_Source(tmp, compTime, ret);
        }
    }

    // Set output parameters
    if (preason != nullptr) {
        *preason = reasonCode;
    }
    if (pinvtm != nullptr) {
        *pinvtm  = compTime;  // transfer ownership
        compTime = nullptr;   // prevent double free
    }

    ret = 1;
    return Free_ASN1_Source(tmp, compTime, ret);
}

/// @brief Parse revocation reason code and related parameters
/// @param reasonStr [in] reason code string
/// @param argStr [in] additional parameter string (may be nullptr)
/// @param hold [out] temporarily store hold instruction object
/// @param phold [out] output parameter, returns hold instruction object
/// @param compTime [out] output parameter, returns invalidity time object
/// @return returns reason code on success, -1 on failure
/// @note For hold instructions and invalidity time, corresponding ASN.1 objects are created based on parameters
int32_t Get_ReasonCode(char8_t *reasonStr,
                       char8_t *argStr,
                       ASN1_OBJECT **hold,
                       ASN1_OBJECT **const phold,
                       ASN1_GENERALIZEDTIME **compTime) noexcept
{
    int32_t reasonCode{-1};

    // Match predefined reason strings
    for (uint32_t i = 0U; i < NUM_REASONS; i++) {
        if (0 == strcasecmp(reasonStr, g_CrlReasons[i])) {  // NOLINT
            reasonCode = static_cast< int32_t >(i);
            break;
        }
    }
    if (reasonCode == OCSP_REVOKED_STATUS_NOSTATUS) {
        return -1;
    }

    // Special reason code handling
    if (reasonCode == kInt_7) {
        reasonCode = OCSP_REVOKED_STATUS_REMOVEFROMCRL;
    } else if (reasonCode == kInt_8) {  // Hold instruction
        if (argStr == nullptr) {
            return -1;
        }
        reasonCode = OCSP_REVOKED_STATUS_CERTIFICATEHOLD;
        *hold      = OBJ_txt2obj(argStr, 0);

        if (*hold == nullptr) {
            return -1;
        }
        // Decide whether to return object or release immediately based on phold parameter
        if (phold != nullptr) {
            *phold = *hold;
        } else {
            ASN1_OBJECT_free(*hold);
        }
    } else if ((reasonCode == kInt_9) || (reasonCode == kInt_10)) {
        if (argStr == nullptr) {
            return -1;
        }
        *compTime = ASN1_GENERALIZEDTIME_new();
        if (*compTime == nullptr) {
            return -1;
        }
        if (ASN1_GENERALIZEDTIME_set_string(*compTime, argStr) == 0) {
            return -1;
        }
        if (reasonCode == kInt_9) {
            reasonCode = OCSP_REVOKED_STATUS_KEYCOMPROMISE;
        } else {
            reasonCode = OCSP_REVOKED_STATUS_CACOMPROMISE;
        }
    }

    return reasonCode;
}
/// @brief Release ASN1 structure resources
/// @param tmp copy of input string (needs to be freed)
/// @param compTime ASN1 generalized time structure pointer (needs to be freed)
/// @param ret function return status code
/// @return passed return status code
int32_t Free_ASN1_Source(char8_t *tmp, ASN1_GENERALIZEDTIME *compTime, int32_t ret) noexcept
{
    OPENSSL_free(tmp);
    ASN1_GENERALIZEDTIME_free(compTime);

    return ret;
}
/*-
* Convert revocation field to X509_REVOKED entry
* return code:
* 0 error
* 1 OK
* 2 OK and some extensions added (i.e. V2 CRL)
*/
/// @brief Revoke
/// @param rev pointer to X509_REVOKED
/// @param str pointer to char
/// @return 0 error 1 OK 2 OK and some extensions added
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_04041
/// @trace_id_sr=SR_CRYPTO_03006
/// @unit_name=Make_revoked
/// @needwork = dd
/// @endcode
int32_t Make_revoked(X509_REVOKED *const rev, char8_t const *const str) noexcept
{
    int32_t reasonCode{-1};
    int32_t i{0};
    int32_t ret{0};
    ASN1_OBJECT *hold{nullptr};
    ASN1_GENERALIZEDTIME *compTime{nullptr};
    ASN1_ENUMERATED *rtmp{nullptr};

    ASN1_TIME *revDate{nullptr};

    i = Unpack_revinfo(&revDate, &reasonCode, &hold, &compTime, str);

    if (i == 0) {
        Free_ObjectSource(hold, compTime, rtmp, revDate);
        return ret;
    }

    if (rev != nullptr) {
        if (X509_REVOKED_set_revocationDate(rev, revDate) == 0) {
            Free_ObjectSource(hold, compTime, rtmp, revDate);
            return ret;
        }
    }

    if (rev != nullptr) {
        if (reasonCode != OCSP_REVOKED_STATUS_NOSTATUS) {
            rtmp = ASN1_ENUMERATED_new();
            if (rtmp == nullptr) {
                Free_ObjectSource(hold, compTime, rtmp, revDate);
                return ret;
            }
            if (0 == ASN1_ENUMERATED_set(rtmp, static_cast< int64_t >(reasonCode))) {
                Free_ObjectSource(hold, compTime, rtmp, revDate);
                return ret;
            }
            if (X509_REVOKED_add1_ext_i2d(rev, NID_crl_reason, rtmp, 0, 0U) == 0) {
                Free_ObjectSource(hold, compTime, rtmp, revDate);
                return ret;
            }
        }
    }

    if ((rev != nullptr) && (compTime == nullptr)) {
        if (X509_REVOKED_add1_ext_i2d(rev, NID_invalidity_date, compTime, 0, 0U) == 0) {
            Free_ObjectSource(hold, compTime, rtmp, revDate);
            return ret;
        }
    }
    if ((rev != nullptr) && (hold != nullptr)) {
        if (X509_REVOKED_add1_ext_i2d(rev, NID_hold_instruction_code, hold, 0, 0U) == 0) {
            Free_ObjectSource(hold, compTime, rtmp, revDate);
            return ret;
        }
    }

    if (reasonCode != OCSP_REVOKED_STATUS_NOSTATUS) {
        ret = 2;
    } else {
        ret = 1;
    }

    Free_ObjectSource(hold, compTime, rtmp, revDate);
    return ret;
}
/// @brief Release OpenSSL ASN.1 object resources to avoid memory leaks
/// @param hold pointer to ASN1_OBJECT, represents revocation instruction code (e.g., NID_hold_instruction_code), generated by Unpack_revinfo(), may be nullptr
/// @param compTime pointer to ASN1_GENERALIZEDTIME, represents certificate invalidity time (e.g., NID_invalidity_date), generated by Unpack_revinfo(), may be nullptr
/// @param rtmp pointer to ASN1_ENUMERATED, represents revocation reason code (e.g., NID_crl_reason), dynamically allocated by ASN1_ENUMERATED_new(), stores ASN.1 encoding of reasonCode
/// @param revDate pointer to ASN1_TIME, represents certificate revocation time, generated by Unpack_revinfo(), must be non-nullptr (otherwise returns early if i == 0), set to CRL entry via X509_REVOKED_set_revocationDate()
void Free_ObjectSource(ASN1_OBJECT *hold,
                       ASN1_GENERALIZEDTIME *compTime,
                       ASN1_ENUMERATED *rtmp,
                       ASN1_TIME *revDate) noexcept
{
    ASN1_OBJECT_free(hold);
    ASN1_GENERALIZEDTIME_free(compTime);
    ASN1_ENUMERATED_free(rtmp);
    ASN1_TIME_free(revDate);
}
/// @brief Next
/// @param format format
/// @return 0 sucess 1 failed
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_04042
/// @trace_id_sr=SR_CRYPTO_03006
/// @unit_name=Istext
/// @needwork = dd
/// @endcode
int32_t Istext(int32_t const format) noexcept
{
    if ((static_cast< uint32_t >(format) & static_cast< uint32_t >(B_FORMAT_TEXT))
        == static_cast< uint32_t >(B_FORMAT_TEXT)) {
        return 1;
    }
    return 0;
}

/// @brief Mode
/// @param mode mode
/// @param format format
/// @return mode string
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_04043
/// @trace_id_sr=SR_CRYPTO_03006
/// @unit_name=Modestr
/// @needwork = dd
/// @endcode
char8_t const *Modestr(char8_t const mode, int32_t const format) noexcept
{
    OPENSSL_assert(mode == 'a' || mode == 'r' || mode == 'w');

    switch (mode) {
        case 'a': {
            return (Istext(format) != 0) ? "a" : "ab";
        } break;
        case 'r': {
            return (Istext(format) != 0) ? "r" : "rb";
        } break;
        case 'w': {
            return (Istext(format) != 0) ? "w" : "wb";
        } break;
        default: {
            return "a";
        }
    }
    /* The assert above should make sure we never reach this point */
    return nullptr;
}

/// @brief Action mode, specifies what action this function returns based on parameter mode
/// @param mode mode
/// @return mode string
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_04044
/// @trace_id_sr=SR_CRYPTO_03006
/// @unit_name=Modeverb
/// @needwork = dd
/// @endcode
const char8_t *Modeverb(char8_t const mode) noexcept
{
    switch (mode) {
        case 'a': {
            return "appending";
        } break;
        case 'r': {
            return "reading";
        } break;
        case 'w': {
            return "writing";
        } break;
        default: {
            return "appending";
        }
    }
    return "(doing something)";
}

/// @brief Default open incoming BIO
/// @param filename filename
/// @param mode mode
/// @param format format
/// @param quiet whether silent
/// @return incoming file data
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_04045
/// @trace_id_sr=SR_CRYPTO_03006
/// @unit_name=Bio_open_default
/// @needwork = dd
/// @endcode
BIO *Bio_open_default(char8_t const *const filename,
                      char8_t const mode,
                      int32_t const format,
                      int32_t const quiet) noexcept
{
    BIO *const ret{BIO_new_file(filename, Modestr(mode, format))};
    if (0 != quiet) {
        return ret;
    }
    if (ret != nullptr) {
        return ret;
    }
    return nullptr;
}

/// @brief Allocate memory
/// @param sz size of memory to allocate (in bytes)
/// @param what pointer to constant char describing allocation purpose
/// @return allocated memory address
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_04046
/// @trace_id_sr=SR_CRYPTO_03006
/// @unit_name=App_malloc
/// @needwork = dd
/// @endcode
void *App_malloc(int32_t const sz, char8_t const *const what) noexcept
{
    std::ignore = what;
    void *const vp{OPENSSL_malloc(static_cast< size_t >(sz))};
    return vp;
}

/// @brief Buffer
/// @param fp file pointer
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_04047
/// @trace_id_sr=SR_CRYPTO_03006
/// @unit_name=Unbuffer
/// @needwork = dd
/// @endcode
void Unbuffer(FILE *const fp) noexcept
{
/*
* On VMS, setbuf() will only take 32-bit pointers, and a compilation
* with /POINTER_SIZE=64 will give off a MAYLOSEDATA2 warning here.
* However, we trust that the C RTL will never give us a FILE pointer
* above the first 4 GB of memory, so we simply turn off the warning
* temporarily.
*/
#if defined(OPENSSL_SYS_VMS) && defined(__DECC)
    #pragma environment save
    #pragma message disable maylosedata2
#endif
    setbuf(fp, nullptr);
#if defined(OPENSSL_SYS_VMS) && defined(__DECC)
    #pragma environment restore
#endif
}

//--------------------------------------------------------------------------------------------------------------//
//                                                                                                              //
//                                                                                                              //
//                                                                                                              //
//                                              Database Related Modules                                                   //
//                                                                                                              //
//                                                                                                              //
//                                                                                                              //
//--------------------------------------------------------------------------------------------------------------//
/// @brief Load database file
/// @param dbData          db data retrieved from persistent file
/// @param dblen           length of db data retrieved from persistent file
/// @param uniqueSubject   unique identifier for issued certificates
/// @return database file address
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_04052
/// @trace_id_sr=SR_CRYPTO_03006
/// @unit_name=Load_index
/// @needwork = dd
/// @endcode
CA_DB *Load_index(char8_t const *const dbData, int32_t const dblen, bool const uniqueSubject) noexcept
{
    CA_DB *retdb{nullptr};
    TXT_DB *tmpdb{nullptr};
    BIO *const in{BIO_new_mem_buf(dbData, dblen)};

    do {
        if ((tmpdb = TXT_DB_read(in, DB_NUMBER)) == nullptr) {
            break;
        }
        retdb = static_cast< CA_DB * >(OPENSSL_malloc(sizeof(*retdb)));
        if (retdb == nullptr) {
            break;
        }
        retdb->db                       = tmpdb;
        tmpdb                           = nullptr;
        retdb->attributes.uniqueSubject = static_cast< int32_t >(uniqueSubject);
        retdb->dbfname                  = OPENSSL_strdup("index");
    } while (false);

    TXT_DB_free(tmpdb);
    BIO_free_all(in);
    return retdb;
}

/// @brief Load factor
/// @param dbfile  database file pointer
/// @param dbAttr pointer to database attribute structure
/// @return database file address
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_04053
/// @trace_id_sr=SR_CRYPTO_03006
/// @unit_name=Load_index2
/// @needwork = dd
/// @endcode
CA_DB *Load_index2(char8_t const *const dbfile, DB_ATTR *const dbAttr) noexcept
{
    CA_DB *retdb{nullptr};
    TXT_DB *tmpdb{nullptr};
    BIO *in{nullptr};
    CONF *const dbAttrConf{nullptr};
    char buf[BSIZE];
#ifndef OPENSSL_NO_POSIX_IO
    FILE *dbfp{nullptr};
    struct stat dbst;  // NOLINT
#endif

    in = BIO_new_file(dbfile, "r");
    do {
        if (in == nullptr) {
            break;
        }

#ifndef OPENSSL_NO_POSIX_IO
        std::ignore = BIO_get_fp(in, &dbfp);  // NOLINT
        if (fstat(fileno(dbfp), &dbst) == -1) {
            SYSerr(SYS_F_FSTAT, errno);
            ERR_add_error_data(3, "fstat('", dbfile, "')");
            break;
        }
#endif

        if ((tmpdb = TXT_DB_read(in, DB_NUMBER)) == nullptr) {
            break;
        }

        std::ignore = BIO_snprintf(buf, sizeof(buf), "%s.attr", dbfile);

        retdb     = static_cast< CA_DB * >(App_malloc(static_cast< int32_t >(sizeof(*retdb)), "new DB"));
        retdb->db = tmpdb;
        tmpdb     = nullptr;
        if (nullptr != dbAttr) {
            retdb->attributes = *dbAttr;
        } else {
            retdb->attributes.uniqueSubject = 1;
        }

        retdb->dbfname = OPENSSL_strdup(dbfile);
#ifndef OPENSSL_NO_POSIX_IO
        retdb->dbst = dbst;
#endif
    } while (false);

    NCONF_free(dbAttrConf);
    TXT_DB_free(tmpdb);
    BIO_free_all(in);
    return retdb;
}

/// @brief Clear database
/// @param db CA_DB type database
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_04054
/// @trace_id_sr=SR_CRYPTO_03006
/// @unit_name=Clear_db
/// @needwork = dd
/// @endcode
void Clear_db(CA_DB *const db) noexcept
{
    if (nullptr != db) {
        TXT_DB_free(db->db);
        OPENSSL_free(db->dbfname);
        OPENSSL_free(db);
    }
}

/// @brief Check time format
/// @param str string
/// @return 0 sucess false otherwise
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_04055
/// @trace_id_sr=SR_CRYPTO_03006
/// @unit_name=Check_time_format
/// @needwork = dd
/// @endcode
int32_t Check_time_format(char8_t const *const str) noexcept { return ASN1_TIME_set_string(nullptr, str); }

/// @brief Index name
/// @param a input string
/// @return 1 or 0
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_04056
/// @trace_id_sr=SR_CRYPTO_03006
/// @unit_name=Index_name_qual
/// @needwork = dd
/// @endcode
int32_t Index_name_qual(char8_t **const a) noexcept
{
    if (a[0U][0U] == 'V') {
        return 1;
    }
    return 0;
}

/// @brief Index serialization hash operation
/// @param a input string
/// @return hash result
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_04057
/// @trace_id_sr=SR_CRYPTO_03006
/// @unit_name=Index_serial_hash
/// @needwork = dd
/// @endcode
uint64_t Index_serial_hash(const OPENSSL_CSTRING *const a) noexcept
{
    char8_t const *n{nullptr};

    n = a[DB_serial];
    while (*n == '0') {
        n++;
    }
    return OPENSSL_LH_strhash(n);
}

/// @brief Index serialization comparison
/// @param a input string A
/// @param b input string B
/// @return 0 equal
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_04058
/// @trace_id_sr=SR_CRYPTO_03006
/// @unit_name=Index_serial_cmp
/// @needwork = dd
/// @endcode
int32_t Index_serial_cmp(const OPENSSL_CSTRING *const a, const OPENSSL_CSTRING *const b) noexcept
{
    char8_t const *aa{nullptr};
    char8_t const *bb{nullptr};

    for (aa = a[DB_serial]; *aa == '0'; aa++) {
    }
    for (bb = b[DB_serial]; *bb == '0'; bb++) {
    }

    size_t const alen{strnlen(aa, SSIZE_MAX)};
    size_t const blen{strnlen(bb, SSIZE_MAX)};
    size_t const len{alen > blen ? alen : blen};
    return strncmp(aa, bb, len);
}

/// @brief Index name comparison
/// @param a input string A
/// @param b input string B
/// @return 0 equal
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_04059
/// @trace_id_sr=SR_CRYPTO_03006
/// @unit_name=Index_name_cmp
/// @needwork = dd
/// @endcode
int32_t Index_name_cmp(const OPENSSL_CSTRING *const a, const OPENSSL_CSTRING *const b) noexcept
{
    size_t const alen{strnlen(static_cast< char8_t const * >(a[DB_name]), SSIZE_MAX)};
    size_t const blen{strnlen(static_cast< char8_t const * >(b[DB_name]), SSIZE_MAX)};
    size_t const len{alen > blen ? alen : blen};
    return strncmp(a[DB_name], b[DB_name], len);
}
/// @brief Index name hash operation
/// @param a input string A
/// @return hash result
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_04060
/// @trace_id_sr=SR_CRYPTO_03006
/// @unit_name=Index_name_hash
/// @needwork = dd
/// @endcode
uint64_t Index_name_hash(const OPENSSL_CSTRING *const a) noexcept { return OPENSSL_LH_strhash(a[DB_name]); }
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_06641
/// @trace_id_sr=SR_CRYPTO_03006
/// @unit_name=IMPLEMENT_LHASH_HASH_FN_OVERRIDE
/// @needwork = dd
/// @endcode
#define IMPLEMENT_LHASH_HASH_FN_OVERRIDE(name, o_type)                                                                 \
    uint64_t name##_LHASH_HASH(void const *const arg)                                                                  \
    {                                                                                                                  \
        const o_type *a{static_cast< const o_type * >(arg)};                                                           \
        return name##_hash(a);                                                                                         \
    }
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_06642
/// @trace_id_sr=SR_CRYPTO_03006
/// @unit_name=IMPLEMENT_LHASH_COMP_FN_OVERRIDE
/// @needwork = dd
/// @endcode
#define IMPLEMENT_LHASH_COMP_FN_OVERRIDE(name, o_type)                                                                 \
    int32_t name##_LHASH_COMP(const void *const arg1, const void *const arg2) noexcept                                 \
    {                                                                                                                  \
        const o_type *a{static_cast< const o_type * >(arg1)};                                                          \
        const o_type *b{static_cast< const o_type * >(arg2)};                                                          \
        return name##_cmp(a, b);                                                                                       \
    }
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_04061
/// @trace_id_sr=SR_CRYPTO_03006
/// @unit_name=IMPLEMENT_LHASH_HASH_FN_OVERRIDE
/// @needwork = dd
/// @endcode
IMPLEMENT_LHASH_HASH_FN_OVERRIDE(Index_serial, OPENSSL_CSTRING)
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_04062
/// @trace_id_sr=SR_CRYPTO_03006
/// @unit_name=IMPLEMENT_LHASH_COMP_FN_OVERRIDE
/// @needwork = dd
/// @endcode
IMPLEMENT_LHASH_COMP_FN_OVERRIDE(Index_serial, OPENSSL_CSTRING)
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_04063
/// @trace_id_sr=SR_CRYPTO_03006
/// @unit_name=IMPLEMENT_LHASH_HASH_FN_OVERRIDE
/// @needwork = dd
/// @endcode
IMPLEMENT_LHASH_HASH_FN_OVERRIDE(Index_name, OPENSSL_CSTRING)
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_04064
/// @trace_id_sr=SR_CRYPTO_03006
/// @unit_name=IMPLEMENT_LHASH_COMP_FN_OVERRIDE
/// @needwork = dd
/// @endcode
IMPLEMENT_LHASH_COMP_FN_OVERRIDE(Index_name, OPENSSL_CSTRING)
/// @brief Create database index
/// @param db database
/// @param field field
/// @param qual function pointer defining a "quality" or "qualification" function
/// @param hash defines hash function
/// @param cmp defines comparison function
/// @return 1 create sucess 0 failed
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_04065
/// @trace_id_sr=SR_CRYPTO_03006
/// @unit_name=TXT_DB_create_index_ph
/// @needwork = dd
/// @endcode
int32_t TXT_DB_create_index_ph(TXT_DB *const db,
                               int32_t const field,
                               int32_t (*const qual)(OPENSSL_STRING *),
                               OPENSSL_LH_HASHFUNC const hash,
                               OPENSSL_LH_COMPFUNC const cmp) noexcept
{
    LHASH_OF(OPENSSL_STRING) * idx{nullptr};
    OPENSSL_STRING *r{nullptr};
    OPENSSL_STRING *k{nullptr};
    int32_t i{0};
    int32_t n{0};

    if (field >= db->num_fields) {
        db->error = DB_ERROR_INDEX_OUT_OF_RANGE;
        return 0;
    }
    /* FIXME: we lose type checking at this point */
    idx = reinterpret_cast< LHASH_OF(OPENSSL_STRING) * >(OPENSSL_LH_new(hash, cmp));
    if (nullptr == idx) {
        db->error = DB_ERROR_MALLOC;
        return 0;
    }
    n = sk_OPENSSL_PSTRING_num(db->data);
    for (i = 0; i < n; i++) {
        r = sk_OPENSSL_PSTRING_value(db->data, i);
        if (qual != nullptr) {
            if (qual(r) == 0) {
                continue;
            }
        }

        if ((k = lh_OPENSSL_STRING_insert(idx, r)) != nullptr) {  // NOLINT
            db->error = DB_ERROR_INDEX_CLASH;
            db->arg1  = sk_OPENSSL_PSTRING_find(db->data, k);
            db->arg2  = i;
            lh_OPENSSL_STRING_free(idx);
            return 0;
        }
        if (lh_OPENSSL_STRING_retrieve(idx, r) == nullptr) {  // NOLINT
            db->error = DB_ERROR_MALLOC;
            lh_OPENSSL_STRING_free(idx);
            return 0;
        }
    }
    lh_OPENSSL_STRING_free(db->index[field]);
    db->index[field] = idx;
    db->qual[field]  = qual;
    return 1;
}

/// @brief Create indexes for certificate database db
/// @param db database, pointer of type CA_DB
/// @return  Returns > 0 on success, <= 0 on error
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_04066
/// @trace_id_sr=SR_CRYPTO_03006
/// @unit_name=Index_index
/// @needwork = dd
/// @endcode
int32_t Index_index(CA_DB *const db) noexcept
{
    if (TXT_DB_create_index_ph(db->db, DB_serial, nullptr, LHASH_HASH_FN(Index_serial), LHASH_COMP_FN(Index_serial))
        == 0) {
        return 0;
    }

    if ((db->attributes.uniqueSubject != 0)
        && (TXT_DB_create_index_ph(db->db, DB_name, Index_name_qual, LHASH_HASH_FN(Index_name),
                                   LHASH_COMP_FN(Index_name))
            == 0)) {
        return 0;
    }
    return 1;
}

/// @brief Check database
/// @param db database, pointer of type CA_DB
/// @return true check sucess false otherwise
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_04067
/// @trace_id_sr=SR_CRYPTO_03006
/// @unit_name=CheckDatabase
/// @needwork = dd
/// @endcode
bool CheckDatabase(CA_DB *const db) noexcept
{
    /* Lets check some fields */
    char8_t *const *pDatai{nullptr};
    char8_t const *pSerial{nullptr};
    int32_t serialLen{0};
    for (uint32_t i{0U}; i < static_cast< uint32_t >(sk_OPENSSL_PSTRING_num(db->db->data)); i++) {
        pDatai = sk_OPENSSL_PSTRING_value(db->db->data, static_cast< int32_t >(i));
        if ((pDatai[DB_type][0] != DB_TYPE_REV) && (pDatai[DB_rev_date][0] != '\0')) {
            Clear_db(db);
            return false;
        }
        if ((pDatai[DB_type][0] == DB_TYPE_REV) && (Make_revoked(nullptr, pDatai[DB_rev_date]) == 0)) {
            Clear_db(db);
            return false;
        }
        if (0 == Check_time_format(static_cast< char8_t * >(pDatai[DB_exp_date]))) {
            Clear_db(db);
            return false;
        }
        pSerial   = pDatai[DB_serial];
        serialLen = static_cast< int32_t >(strnlen(pSerial, SSIZE_MAX));
        if (*pSerial == '-') {
            pSerial++;
            serialLen--;
        }
        if (((static_cast< uint32_t >(serialLen) & 1U) != 0U) || (serialLen < 2)) {
            Clear_db(db);
            return false;
        }
        for (; '\0' != *pSerial; pSerial++) {
            if (0 == isxdigit(static_cast< int32_t >(_UC(*pSerial)))) {
                Clear_db(db);
                return false;
            }
        }
    }

    return true;
}

/// @brief /* Update the db file for expired certificates */
/// @brief Revoking a certificate means revoking a valid certificate for security reasons, which can be undone
///         whereas updating the db to make a certificate expired makes it permanently invalid
/// @param db database, pointer of type CA_DB
/// @return number of revoked certificates
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_04068
/// @trace_id_sr=SR_CRYPTO_03006
/// @unit_name=Do_updatedb
/// @needwork = dd
/// @endcode
int32_t Do_updatedb(CA_DB *const db) noexcept
{
    ASN1_UTCTIME *aTm{nullptr};
    int32_t i{0};
    int32_t cnt{0};
    int32_t dbY2k{0};
    int32_t aY2k{0}; /* flags = 1 if y >= 2000 */
    char8_t **rrow{nullptr};
    char8_t *aTmS{nullptr};

    aTm = ASN1_UTCTIME_new();
    if (aTm == nullptr) {
        return -1;
    }

    /* get actual time and make a string */
    if (X509_gmtime_adj(aTm, 0) == nullptr) {
        ASN1_UTCTIME_free(aTm);
        return -1;
    }
    aTmS = static_cast< char8_t * >(App_malloc(aTm->length + 1, "time string"));
    if (aTmS == nullptr) {
        return -1;
    }

    std::ignore       = memcpy(aTmS, aTm->data, static_cast< size_t >(aTm->length));
    aTmS[aTm->length] = '\0';

    if (strncmp(aTmS, "49", 2U) <= 0) {
        aY2k = 1;
    } else {
        aY2k = 0;
    }

    for (i = 0; i < sk_OPENSSL_PSTRING_num(db->db->data); i++) {
        rrow = sk_OPENSSL_PSTRING_value(db->db->data, i);

        if (rrow[DB_type][0] == DB_TYPE_VAL) {
            /* ignore entries that are not valid */
            if (strncmp(rrow[DB_exp_date], "49", 2U) <= 0) {
                dbY2k = 1;
            } else {
                dbY2k = 0;
            }

            if (dbY2k == aY2k) {
                size_t const alen{strnlen(static_cast< char8_t const * >(rrow[DB_exp_date]), SSIZE_MAX)};
                size_t const blen{strnlen(static_cast< char8_t const * >(aTmS), SSIZE_MAX)};
                size_t const len{alen > blen ? alen : blen};
                /* all on the same y2k side */
                if (strncmp(rrow[DB_exp_date], aTmS, len) <= 0) {
                    rrow[DB_type][0] = DB_TYPE_EXP;
                    rrow[DB_type][1] = '\0';
                    cnt++;
                }
            } else if (dbY2k < aY2k) {
                rrow[DB_type][0] = DB_TYPE_EXP;
                rrow[DB_type][1] = '\0';
                cnt++;
            }
        }
    }

    ASN1_UTCTIME_free(aTm);
    OPENSSL_free(aTmS);
    return cnt;
}

/// @brief Save db to bio
/// @param dbfile database file
/// @param db database, pointer of type CA_DB
/// @return 1 save sucess 0 failed
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_04069
/// @trace_id_sr=SR_CRYPTO_03006
/// @unit_name=Save_index
/// @needwork = dd
/// @endcode
int32_t Save_index(BIO **const out, CA_DB *const db) noexcept
{
    int32_t j{0};
    j = static_cast< int32_t >(TXT_DB_write(*out, db->db));
    if (j <= 0) {
        return 0;
    }

    return 1;
}

/// @brief Update database
/// @param db   database pointer
/// @param out  updated database content, null indicates no update
/// @return true update db sucess false otherwise
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_04070
/// @trace_id_sr=SR_CRYPTO_03006
/// @unit_name=UpdateDb
/// @needwork = dd
/// @endcode
bool UpdateDb(CA_DB *const db, BIO **const out) noexcept
{
    int32_t const res{Do_updatedb(db)};
    if (res == -1) {
        Clear_db(db);
        return false;
    }
    if (res == 0) {
        ara::crypto::isoft_def::LogInfo() << "No entries found to mark expired";
        return true;
    }
    if (Save_index(out, db) == 0) {
        Clear_db(db);
        return false;
    }
    return true;
}

/// @brief Load db data from persistent file, update data to persistent file if there are changes
/// @param uniqueSubject unique identifier for issued certificates, default is zero, allows multiple certificates with the same subject to exist simultaneously in the CA index file, otherwise only one is allowed
/// @param dbData       pointer to db file content
/// @param dblen        length of db file content
/// @param bioOut       updated persistent file memory data, null indicates no update needed
/// @return database address
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_04071
/// @trace_id_sr=SR_CRYPTO_03006
/// @unit_name=LoadAndUpdateDb
/// @needwork = dd
/// @endcode
CA_DB *LoadAndUpdateDb(int32_t const uniqueSubject,
                       char8_t const *const dbData,
                       int32_t const dblen,
                       BIO **const bioOut) noexcept
{
    CA_DB *db{nullptr};
    /*****************************************************************/
    /* we need to load the database file */

    db = Load_index(dbData, dblen, static_cast< bool >(uniqueSubject));

    do {
        if (db == nullptr) {
            break;
        }

        // Check database
        // e.g., check dates, serial numbers, etc.
        if (CheckDatabase(db) == false) {
            break;
        }

        // Create indexes -- index_serial, index_name
        if (Index_index(db) <= 0) {
            break;
        }

        // Update the db file for expired certificates (update index.txt)
        // Theoretically, every time the database is loaded, it should check if certificates have expired
        if (UpdateDb(db, bioOut) == false) {
            break;
        }

        return db;

    } while (false);

    Clear_db(db);
    return nullptr;
}
//--------------------------------------------------------------------------------------------------------------//
//                                                                                                              //
//                                                                                                              //
//                                                                                                              //
//                                              Preparation for CRL Signing                                                    //
//                                                                                                              //
//                                                                                                              //
//                                                                                                              //
//--------------------------------------------------------------------------------------------------------------//
/*
* Parse message digest name, put it in *EVP_MD; return 0 on failure, else 1.
*/
/// @brief Get digest algorithm structure pointer based on specified digest name
/// @param name digest name
/// @param mdp  digest algorithm structure pointer
/// @return 1 sucess 0 failed
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_04072
/// @trace_id_sr=SR_CRYPTO_03006
/// @unit_name=Opt_md
/// @needwork = dd
/// @endcode
int32_t Opt_md(char8_t const *const name, EVP_MD const **const mdp) noexcept
{
    *mdp = EVP_get_digestbyname(name);
    if (*mdp != nullptr) {
        return 1;
    }
    return 0;
}

/// @brief dgest used for CRL signing to ensure security
/// @param pkey EVP_PKEY key resource
/// @return digest information
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_04073
/// @trace_id_sr=SR_CRYPTO_03006
/// @unit_name=ReadyDgest
/// @needwork = dd
/// @endcode
EVP_MD const *ReadyDgest(EVP_PKEY *const pkey) noexcept
{
    EVP_MD const *dgst{nullptr};
    int32_t defNid{0};
    int32_t const defRet{EVP_PKEY_get_default_digest_nid(pkey, &defNid)};
    char8_t const *md{"default"};
    /*
* EVP_PKEY_get_default_digest_nid() returns 2 if the digest is
* mandatory for this algorithm.
*/
    if ((defRet == kInt_2) && (defNid == static_cast< int32_t >(NID_undef))) {
        /* The signing algorithm requires there to be no digest */
        dgst = EVP_md_null();
    } else {
        if (strncmp(md, "default", kInt_7U) == 0) {
            if (defRet <= 0) {
                dgst = nullptr;
            }
            md = static_cast< char8_t const * >(OBJ_nid2sn(defNid));
        }

        if (Opt_md(md, &dgst) == 0) {
            dgst = nullptr;
        }
    }

    return dgst;
}

//--------------------------------------------------------------------------------------------------------------//
//                                                                                                              //
//                                                                                                              //
//                                                                                                              //
//                                              Generate CRL Module                                                      //
//                                                                                                              //
//                                                                                                              //
//                                                                                                              //
//--------------------------------------------------------------------------------------------------------------//
/// @brief If GenerateCRLists() fails, call this function to release memory
///         Do not call if successful, as these variables are still needed later
/// @param db database pointer
/// @param crl certificate revocation list pointer
/// @param returnFalse identifier
/// @param x509 certificate pointer
/// @return true scuess false failed
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_04074
/// @trace_id_sr=SR_CRYPTO_03006
/// @unit_name=ClearCacheInGenCRL
/// @needwork = dd
/// @endcode
bool ClearCacheInGenCRL(CA_DB *const db, X509_CRL *const crl, bool const returnFalse, X509 *const x509) noexcept
{
    Clear_db(db);
    if (x509 != nullptr) {
        X509_free(x509);
    }
    if (crl != nullptr) {
        X509_CRL_free(crl);
    }
    if (returnFalse) {
        return false;
    }
    return true;
}

/// @brief Random number serialization
/// @param b pointer to BIGNUM object
/// @param ai pointer to ASN1_INTEGER object
/// @return random number sequence
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_04075
/// @trace_id_sr=SR_CRYPTO_03006
/// @unit_name=Rand_serial
/// @needwork = dd
/// @endcode
int32_t Rand_serial(BIGNUM *const b, ASN1_INTEGER *const ai) noexcept
{
    BIGNUM *btmp{nullptr};
    int32_t ret{0};

    btmp = b == nullptr ? BN_new() : b;
    if (btmp == nullptr) {
        return 0;
    }
    do {
        if (BN_rand(btmp, SERIAL_RAND_BITS, BN_RAND_TOP_ANY, BN_RAND_BOTTOM_ANY) == 0) {
            break;
        }
        if (ai != nullptr) {
            if (nullptr == BN_to_ASN1_INTEGER(btmp, ai)) {
                break;
            }
        }

        ret = 1;
    } while (false);

    if (btmp != b) {
        BN_free(btmp);
    }

    return ret;
}

/// @brief Local serialization
/// @param serialfile serialized file
/// @param create identifier
/// @param retai double pointer to ASN1_INTEGER
/// @return serial number
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_04076
/// @trace_id_sr=SR_CRYPTO_03006
/// @unit_name=Load_serial
/// @needwork = dd
/// @endcode
BIGNUM *Load_serial(char8_t const *const serialfile, int32_t const create, ASN1_INTEGER **const retai) noexcept
{
    BIO *in{nullptr};
    BIGNUM *ret{nullptr};
    char8_t buf[kInt_1024U]{'\0'};
    ASN1_INTEGER *ai{nullptr};

    do {
        ai = ASN1_INTEGER_new();
        if (ai == nullptr) {
            break;
        }

        in = BIO_new_file(serialfile, "r");
        if (in == nullptr) {
            if (create == 0) {
                perror(serialfile);
                break;
            }
            ERR_clear_error();
            ret = BN_new();
            if ((ret == nullptr) || (Rand_serial(ret, ai) == 0)) {
                ara::crypto::isoft_def::LogInfo() << "Out of memory";
            }
        } else {
            if (a2i_ASN1_INTEGER(in, ai, buf, kInt_1024) == 0) {
                break;
            }
            ret = ASN1_INTEGER_to_BN(ai, nullptr);
            if (ret == nullptr) {
                break;
            }
        }

        if ((ret != nullptr) && (retai != nullptr)) {
            *retai = ai;
            ai     = nullptr;
        }
    } while (false);

    std::ignore = BIO_free(in);
    ASN1_INTEGER_free(ai);
    return ret;
}

/// @brief Save serialization result
/// @param serialfile serialized file pointer
/// @param suffix pointer
/// @param serial serialization
/// @param retai pointer to ASN1_INTEGER
/// @return 0 save sucess
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_04077
/// @trace_id_sr=SR_CRYPTO_03006
/// @unit_name=Save_serial
/// @needwork = dd
/// @endcode
int32_t Save_serial(char8_t const *const serialfile,
                    char8_t const *const suffix,
                    BIGNUM const *const serial,
                    ASN1_INTEGER **const retai) noexcept
{
    char8_t buf[1][BSIZE];
    BIO *out{nullptr};
    int32_t ret{0};
    ASN1_INTEGER *ai{nullptr};
    int32_t j{0};

    if (suffix == nullptr) {
        j = static_cast< int32_t >(strnlen(serialfile, SSIZE_MAX));
    } else {
        j = static_cast< int32_t >((strnlen(serialfile, SSIZE_MAX) + strnlen(suffix, SSIZE_MAX) + 1U));
    }
    do {
        if (j >= BSIZE) {
            break;
        }

        if (suffix == nullptr) {
            std::ignore = OPENSSL_strlcpy(buf[0], serialfile, static_cast< size_t >(BSIZE));
        } else {
#ifndef OPENSSL_SYS_VMS
            j = BIO_snprintf(buf[0], sizeof(buf[0]), "%s.%s", serialfile, suffix);
#else
            j = BIO_snprintf(buf[0], sizeof(buf[0]), "%s-%s", serialfile, suffix);
#endif
        }
        std::ignore = j;
        out         = BIO_new_file(buf[0], "w");
        if (out == nullptr) {
            break;
        }

        if ((ai = BN_to_ASN1_INTEGER(serial, nullptr)) == nullptr) {
            break;
        }
        std::ignore = i2a_ASN1_INTEGER(out, ai);
        std::ignore = BIO_puts(out, "\n");
        ret         = 1;
        if (nullptr != retai) {
            *retai = ai;
            ai     = nullptr;
        }

    } while (false);

    BIO_free_all(out);
    ASN1_INTEGER_free(ai);
    return ret;
}

/// @brief Key string
/// @param ctx context pointer
/// @param value data participating in algorithm operation
/// @return 0 sucess -1 failed
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_04078
/// @trace_id_sr=SR_CRYPTO_03006
/// @unit_name=Pkey_ctrl_string
/// @needwork = dd
/// @endcode
int32_t Pkey_ctrl_string(EVP_PKEY_CTX *const ctx, char8_t const *const value) noexcept
{
    int32_t rv{0};
    char8_t *stmp{nullptr};
    char8_t *vtmp{nullptr};
    stmp = OPENSSL_strdup(value);
    if (stmp == nullptr) {
        return -1;
    }

    vtmp = strchr(stmp, ':');
    if (nullptr != vtmp) {
        *vtmp = static_cast< char8_t >(0U);
        vtmp++;
    }
    rv = EVP_PKEY_CTX_ctrl_str(ctx, stmp, vtmp);
    OPENSSL_free(stmp);
    return rv;
}

/// @brief Signature initialization
/// @param ctx context pointer
/// @param pkey EVP_PKEY key resource
/// @param md EVP_MD pointer
/// @return 0 sign init failed 1 sucess
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_04079
/// @trace_id_sr=SR_CRYPTO_03006
/// @unit_name=Do_sign_init
/// @needwork = dd
/// @endcode
int32_t Do_sign_init(EVP_MD_CTX *const ctx, EVP_PKEY *const pkey, EVP_MD const *md) noexcept
{
    EVP_PKEY_CTX *pkctx{nullptr};
    // int32_t i,
    int32_t defNid{0};

    if (ctx == nullptr) {
        return 0;
    }
    /*
* EVP_PKEY_get_default_digest_nid() returns 2 if the digest is mandatory
* for this algorithm.
*/
    if ((EVP_PKEY_get_default_digest_nid(pkey, &defNid) == 2) && (defNid == NID_undef)) {
        /* The signing algorithm requires there to be no digest */
        md = nullptr;
    }
    if (EVP_DigestSignInit(ctx, &pkctx, md, nullptr, pkey) == 0) {
        return 0;
    }
    return 1;
}

/// @brief Perform X509 certificate revocation list signing
/// @param x pointer to X509_CRL
/// @param pkey EVP_PKEY key resource
/// @param md EVP_MD pointer
/// @return 0 sucess 1 failed
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_04080
/// @trace_id_sr=SR_CRYPTO_03006
/// @unit_name=Do_X509_CRL_sign
/// @needwork = dd
/// @endcode
int32_t Do_X509_CRL_sign(X509_CRL *const x, EVP_PKEY *const pkey, EVP_MD const *const md) noexcept
{
    int32_t rv{0};
    EVP_MD_CTX *const mctx{EVP_MD_CTX_new()};
    rv = Do_sign_init(mctx, pkey, md);
    if (rv > 0) {
        rv = X509_CRL_sign_ctx(x, mctx);
    }
    EVP_MD_CTX_free(mctx);
    if (rv > 0) {
        return 1;
    }
    return 0;
}

/// @brief Rotate serialization
/// @param serialfile   name of the serial number file to process (including path if necessary)
/// @param newSuffix   suffix used to create temporary or new version of serial number file
/// @param oldSuffix   old suffix identifying current serial number file version; function attempts to rename file containing old_suffix to filename with old_suffix
/// @return 0 sucess
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_04081
/// @trace_id_sr=SR_CRYPTO_03006
/// @unit_name=Rotate_serial
/// @needwork = dd
/// @endcode
int32_t Rotate_serial(char8_t const *const serialfile,
                      char8_t const *const newSuffix,
                      char8_t const *const oldSuffix) noexcept
{
    char8_t buf[2][BSIZE];
    int32_t i{0};
    int32_t j{0};

    i = static_cast< int32_t >((strnlen(serialfile, SSIZE_MAX) + strnlen(oldSuffix, SSIZE_MAX)));
    j = static_cast< int32_t >((strnlen(serialfile, SSIZE_MAX) + strnlen(newSuffix, SSIZE_MAX)));
    if (i > j) {
        j = i;
    }

    do {
        if (j + 1 >= BSIZE) {
            break;
        }
#ifndef OPENSSL_SYS_VMS
        j           = BIO_snprintf(buf[0], sizeof(buf[0]), "%s.%s", serialfile, newSuffix);
        std::ignore = j;
        j           = BIO_snprintf(buf[1], sizeof(buf[1]), "%s.%s", serialfile, oldSuffix);
        std::ignore = j;
#else
        j = BIO_snprintf(buf[0], sizeof(buf[0]), "%s-%s", serialfile, newSuffix);
        j = BIO_snprintf(buf[1], sizeof(buf[1]), "%s-%s", serialfile, oldSuffix);
#endif
        if (rename(serialfile, buf[1]) < 0) {
            if (errno != ENOENT) {
#ifdef ENOTDIR
                if (errno != ENOTDIR)
#endif
                {
                    break;
                }
            }
        }
        if (rename(buf[0], serialfile) < 0) {
            std::ignore = rename(buf[1], serialfile);
            break;
        }
        return 1;

    } while (false);

    return 0;
}

/// @brief Entry function for generating CRL
/// @param db database file, loaded from index.txt, contains revoked certificate information
/// @param x509Root X509 root certificate
/// @param dgst used for signing
/// @param pkey EVP_PKEY key resource
/// @return pointer to certificate revocation list structure
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_04082
/// @trace_id_sr=SR_CRYPTO_03006
/// @unit_name=GenerateCRLs
/// @needwork = dd
/// @endcode
X509_CRL *GenerateCRLs(CA_DB *const db, X509 *const x509Root, EVP_MD const *const dgst, EVP_PKEY *const pkey) noexcept
{
    // crl extensions here refer to X509v3 extensions supported by CRL, allowing additional information to be added to the CRL.
    // Common CRL extensions include:
    // CRLNumber: CRL number
    // AuthorityKeyIdentifier: issuer key identifier
    // IssuingDistributionPoint: CRL distribution point
    int32_t crlV2{0};
    char8_t *const crlExt{nullptr};
    BIGNUM *crlnumber{nullptr};

    // Attempt to load CRL validity period related configurations from configuration file
    // and save to crldays, crlhours, and crlsec variables.
    int64_t const crldays{30};
    int64_t const crlhours{2};
    int64_t const crlsec{1};
    // ----------------------------------------------------------------
    //
    //                        making CRL
    //
    // ----------------------------------------------------------------
    X509_CRL *crl{nullptr};
    // new an x509_crl
    if ((crl = X509_CRL_new()) == nullptr) {
        return nullptr;
    }
    // Set issuer name field of X509_CRL structure.
    // According to PKI rules, a CRL's issuer name must match the subject name of the issuing CA of the revoked certificates.
    // Internal checks seem to exist here; revoked certificates in CRL and X509 certificates are different matters.
    if (X509_CRL_set_issuer_name(crl, X509_get_subject_name(x509Root)) == 0) {
        return nullptr;
    }

    // N1_TIME and ASN1_TIME_new() relate to time representation in X509 certificates.
    // Allocate an empty ASN.1 time structure for storing start/end times of certificates or CRLs later.
    ASN1_TIME *tmptm{nullptr};
    tmptm = ASN1_TIME_new();

    // Set last update and next update times sequentially
    if (X509_gmtime_adj(tmptm, 0) == nullptr) {
        ASN1_TIME_free(tmptm);
        return nullptr;
    }
    if ((X509_CRL_set1_lastUpdate(crl, tmptm) == 0)) {
        ASN1_TIME_free(tmptm);
        return nullptr;
    }
    if (nullptr
        == X509_time_adj_ex(tmptm, static_cast< int32_t >(crldays), crlhours * kInt_60 * kInt_60 + crlsec, nullptr)) {
        ASN1_TIME_free(tmptm);
        return nullptr;
    }
    std::ignore = X509_CRL_set1_nextUpdate(crl, tmptm);

    ASN1_TIME_free(tmptm);

    int32_t ret{Do_Revoke_ByCRL(db, crlV2, crl)};
    if (-1 == ret) {
        return nullptr;
    }
    /*
* sort the data so it will be written in serial number order
*/
    std::ignore = X509_CRL_sort(crl);

    /* we now have a CRL */

    // Set version number
    if ((crlExt != nullptr) || (crlV2 != 0)) {
        if (X509_CRL_set_version(crl, 1) == 0) {
            return nullptr; /* version 2 CRL */
        }
    }

    /* we have a CRL number that need updating */

    BN_free(crlnumber);
    crlnumber = nullptr;

    // Sign
    if (Do_X509_CRL_sign(crl, pkey, dgst) == 0) {
        return nullptr;
    }
    return crl;
}
/// @brief Execute revoke logic
/// @param db  database
/// @param crlV2 record revocation status, if not OCSP_REVOKED_STATUS_NOSTATUS, set crlV2 to 1
/// @param crl  certificate revocation list
/// @return success status
int32_t Do_Revoke_ByCRL(CA_DB *const db, int32_t &crlV2, X509_CRL *crl) noexcept
{
    // Traverse OpenSSL CA certificate database
    char8_t *const *pp2{nullptr};
    X509_REVOKED *r{nullptr};
    ASN1_INTEGER *tmpser{nullptr};
    uint32_t j{0U};
    BIGNUM *serial{nullptr};  // Serial number used to store serial number extracted from database certificates
    for (uint32_t i{0U}; i < static_cast< uint32_t >(sk_OPENSSL_PSTRING_num(db->db->data)); i++) {
        pp2 = sk_OPENSSL_PSTRING_value(db->db->data, static_cast< int32_t >(i));
        if (pp2[DB_type][0] == DB_TYPE_REV) {
            if ((r = X509_REVOKED_new()) == nullptr) {
                return -1;
            }
            j = static_cast< uint32_t >(Make_revoked(r, pp2[DB_rev_date]));
            if (j == 0U) {
                return -1;
            }
            if (j == 2U) {
                crlV2 = 1;
            }
            if (BN_hex2bn(&serial, pp2[DB_serial]) == 0) {
                return -1;
            }
            tmpser = BN_to_ASN1_INTEGER(serial, nullptr);  // Extract serial number from certificate
            BN_free(serial);
            serial = nullptr;
            if (tmpser == nullptr) {
                return -1;
            }
            std::ignore = X509_REVOKED_set_serialNumber(r, tmpser);
            ASN1_INTEGER_free(tmpser);
            std::ignore = X509_CRL_add0_revoked(crl, r);
        }
    }
    return 1;
}
//--------------------------------------------------------------------------------------------------------------//
//                                                                                                              //
//                                                                                                              //
//                                                                                                              //
//                                              Certificate Revocation Module                                                     //
//                                                                                                              //
//                                                                                                              //
//                                                                                                              //
//--------------------------------------------------------------------------------------------------------------//

/*
* Given revocation information convert to a DB string. The format of the
* string is: revtime[,reason,extra]. Where 'revtime' is the revocation time
* (the current time). 'reason' is the optional CRL reason and 'extra' is any
* additional argument
*/
/// @brief Revoke
/// @param revType revocation type
/// @param revArg  revocation argument
/// @return string
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_04083
/// @trace_id_sr=SR_CRYPTO_03006
/// @unit_name=Make_revocation_str
/// @needwork = dd
/// @endcode
char8_t *Make_revocation_str(REVINFO_TYPE const revType, char8_t const *const revArg) noexcept
{
    char8_t *str{nullptr};
    char8_t const *reason{nullptr};
    char8_t const *other{nullptr};
    ASN1_OBJECT *otmp{nullptr};
    ASN1_UTCTIME *revtm{nullptr};
    int32_t i{0};

    switch (revType) {
        case kREV_NONE:
        case kREV_VALID:
            break;

        case kREV_CRL_REASON: {
            for (i = 0; i < kInt_8; i++) {
                if (strcasecmp(revArg, g_CrlReasons[i]) == 0) {  // NOLINT
                    reason = g_CrlReasons[i];                    // NOLINT
                    break;
                }
            }
            if (reason == nullptr) {
                return nullptr;
            }
        } break;
        case kREV_HOLD: {
            /* Argument is an OID */
            otmp = OBJ_txt2obj(revArg, 0);
            ASN1_OBJECT_free(otmp);

            if (otmp == nullptr) {
                return nullptr;
            }

            reason = "holdInstruction";
            other  = revArg;
        } break;

        case kREV_KEY_COMPROMISE:
        case kREV_CA_COMPROMISE: {
            /* Argument is the key compromise time  */
            if (ASN1_GENERALIZEDTIME_set_string(nullptr, revArg) == 0) {
                return nullptr;
            }
            other = revArg;
            if (revType == kREV_KEY_COMPROMISE) {
                reason = "keyTime";
            } else {
                reason = "CAkeyTime";
            }

            break;
        }
    }

    revtm = X509_gmtime_adj(nullptr, 0);

    if (revtm == nullptr) {
        return nullptr;
    }

    i = revtm->length + 1;

    if (nullptr != reason) {
        i += static_cast< int32_t >((strnlen(reason, SIZE_MAX) + 1U));
    }
    if (nullptr != other) {
        i += static_cast< int32_t >((strnlen(other, SIZE_MAX) + 1U));
    }

    str         = static_cast< char8_t * >(App_malloc(i, "revocation reason"));
    std::ignore = OPENSSL_strlcpy(str, T_TransChar(revtm->data), static_cast< size_t >(i));
    if (nullptr != reason) {
        std::ignore = OPENSSL_strlcat(str, ",", static_cast< size_t >(i));
        std::ignore = OPENSSL_strlcat(str, reason, static_cast< size_t >(i));
    }
    if (nullptr != other) {
        std::ignore = OPENSSL_strlcat(str, ",", static_cast< size_t >(i));
        std::ignore = OPENSSL_strlcat(str, other, static_cast< size_t >(i));
    }
    ASN1_UTCTIME_free(revtm);
    return str;
}

/// @brief Get database by index
/// @param db database
/// @param idx index
/// @param value data participating in algorithm operation
/// @return database index string
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_04084
/// @trace_id_sr=SR_CRYPTO_03006
/// @unit_name=TXT_DB_get_by_index_ph
/// @needwork = dd
/// @endcode
OPENSSL_STRING *TXT_DB_get_by_index_ph(TXT_DB *const db, int32_t const idx, OPENSSL_STRING *const value) noexcept
{
    OPENSSL_STRING *ret{nullptr};
    LHASH_OF(OPENSSL_STRING) * lh{nullptr};

    if (idx >= db->num_fields) {
        db->error = DB_ERROR_INDEX_OUT_OF_RANGE;
        return nullptr;
    }
    lh = db->index[idx];
    if (lh == nullptr) {
        db->error = DB_ERROR_NO_INDEX;
        return nullptr;
    }
    ret       = lh_OPENSSL_STRING_retrieve(lh, value);  //NOLINT
    db->error = DB_ERROR_OK;
    return ret;
}
/// @brief Revoke
/// @param x509 certificate
/// @param db  database
/// @param revType revocation type
/// @param value data participating in algorithm operation
/// @return 1 sucess
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_04085
/// @trace_id_sr=SR_CRYPTO_03006
/// @unit_name=Do_revoke
/// @needwork = dd
/// @endcode
int32_t Do_revoke(X509 *const x509, CA_DB *const db, REVINFO_TYPE const revType, const char8_t *const value) noexcept
{
    const ASN1_TIME *tm{nullptr};
    char8_t *row[DB_NUMBER];
    char8_t **rrow{nullptr};
    char8_t **irow{nullptr};
    char8_t *revStr{nullptr};
    BIGNUM *bn{nullptr};
    int32_t ok{-1};
    int32_t i{0};

    for (i = 0; i < DB_NUMBER; i++) {
        row[i] = nullptr;  // NOLINT
    }
    row[DB_name] = X509_NAME_oneline(X509_get_subject_name(x509), nullptr, 0);
    bn           = ASN1_INTEGER_to_BN(X509_get_serialNumber(x509), nullptr);

    do {
        bool isSuccess{FunDoWhile(bn, row, rrow, tm, x509, irow, revType, ok, value, revStr, db)};
        if (isSuccess == false) {
            break;
        }
    } while (false);

    for (i = 0; i < DB_NUMBER; i++) {
        OPENSSL_free(row[i]);  // NOLINT
    }

    return ok;
}
/// @brief FunDoWhile function encapsulates code at the calling location to reduce complexity of Do_revoke function
/// @param bn big num extracted from x509
/// @param row this is an array of m rows, m=DB_NUMBER, each m represents a char string
/// @param rrow this is an mxn two-dimensional matrix obtained by TXT_DB_get_by_index_ph
/// @param tm pointer to ASN1_TIME structure, value of X.509 certificate "notAfter" field. Represents certificate expiration time (point in time when certificate is no longer valid).
/// @param x509 x509 certificate
/// @param irow used to store TXT_DB field values in db
/// @param revType constant identifier representing revocation status
/// @param ok indicates revocation result, 1 means success
/// @param value revocation argument
/// @param revStr revocation type
/// @param db database
/// @return whether this function executed successfully, true means success, false will break the loop calling this function
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_
/// @trace_id_dd=DD_CRYPTO_
/// @trace_id_sr=SR_CRYPTO_
/// @unit_name=Do_revoke
/// @needwork = dd
/// @endcode
bool FunDoWhile(BIGNUM *bn,
                char8_t **row,
                char8_t **rrow,
                ASN1_TIME const *tm,
                X509 *const x509,
                char8_t **irow,
                REVINFO_TYPE const revType,
                int32_t &ok,
                const char8_t *const value,
                char8_t *revStr,
                CA_DB *const db) noexcept
{
    if (bn == nullptr) {
        return false;
    }
    if (0 != BN_is_zero(bn)) {
        row[DB_serial] = OPENSSL_strdup("00");
    } else {
        row[DB_serial] = BN_bn2hex(bn);
    }
    BN_free(bn);
    if ((row[DB_name] != nullptr) && (row[DB_name][0] == '\0')) {
        /* Entries with empty Subjects actually use the serial number instead */
        OPENSSL_free(row[DB_name]);
        row[DB_name] = OPENSSL_strdup(row[DB_serial]);
    }
    if ((row[DB_name] == nullptr) || (row[DB_serial] == nullptr)) {
        return false;
    }
    /*
* We have to lookup by serial number because name lookup skips revoked
* certs
*/
    rrow = TXT_DB_get_by_index_ph(db->db, DB_serial, row);
    if (rrow == nullptr) {
        /* We now just add it to the database as DB_TYPE_REV('V') */
        row[DB_type]                 = OPENSSL_strdup("V");
        tm                           = X509_get0_notAfter(x509);
        row[DB_exp_date]             = static_cast< char8_t * >(App_malloc(tm->length + 1, "row exp_data"));
        std::ignore                  = memcpy(row[DB_exp_date], tm->data, static_cast< size_t >(tm->length));
        row[DB_exp_date][tm->length] = '\0';
        row[DB_rev_date]             = nullptr;
        row[DB_file]                 = OPENSSL_strdup("unknown");

        if ((row[DB_type] == nullptr) || (row[DB_file] == nullptr)) {
            return false;
        }

        irow
            = static_cast< char8_t ** >(App_malloc(static_cast< int32_t >(sizeof(*irow)) * (DB_NUMBER + 1), "row ptr"));
        for (int32_t i = 0; i < DB_NUMBER; i++) {
            irow[i] = row[i];  // NOLINT
        }
        irow[DB_NUMBER] = nullptr;

        if (TXT_DB_insert(db->db, irow) == 0) {
            OPENSSL_free(irow);
            return false;
        }

        for (int32_t i = 0; i < DB_NUMBER; i++) {
            row[i] = nullptr;  // NOLINT
        }

        /* Revoke Certificate */
        if (revType == kREV_VALID) {
            ok = 1;
        } else {
            /* Retry revocation after DB insertion */
            ok = ara::crypto::openssl::isoft_def::Do_revoke(x509, db, revType, value);
        }

        return false;
    }
    if (index_name_cmp_noconst(row, rrow) != 0) {  // NOLINT
        return false;
    }
    if (revType == kREV_VALID) {
        return false;
    }
    if (rrow[DB_type][0] == DB_TYPE_REV) {
        return false;
    }
    revStr = Make_revocation_str(revType, value);
    if (revStr == nullptr) {
        return false;
    }
    rrow[DB_type][0]  = DB_TYPE_REV;
    rrow[DB_type][1]  = '\0';
    rrow[DB_rev_date] = revStr;
    ok                = 1;
    return true;
}

/// @brief Entry function for revoking certificates
/// @param db database
/// @param revcert certificate to revoke
/// @param out output buffer
/// @return true revoke cert sucess
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_04086
/// @trace_id_sr=SR_CRYPTO_03006
/// @unit_name=RevokeCertificate
/// @needwork = dd
/// @endcode
bool RevokeCertificate(CA_DB *const db, X509 *const revcert, BIO **const out) noexcept
{
    REVINFO_TYPE const revType{kREV_NONE};
    char8_t *const revArg{nullptr};
    uint32_t j{0U};
    bool isSuccess{true};

    do {
        if (revcert == nullptr) {
            isSuccess = false;
            break;
        }

        j = static_cast< uint32_t >(Do_revoke(revcert, db, revType, revArg));
        if (*out == nullptr) {
            isSuccess = false;
        }

        if (j <= 0U) {
            isSuccess = false;
            break;
        }
        X509_free(revcert);

        if (Save_index(out, db) == 0) {
            isSuccess = false;
            break;
        }

        return isSuccess;
    } while (false);

    Clear_db(db);

    return isSuccess;
}
/// @brief Find sequence
/// @param db database
/// @param ser serialization
/// @return serial number
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_04087
/// @trace_id_sr=SR_CRYPTO_03006
/// @unit_name=Lookup_serial
/// @needwork = dd
/// @endcode
char8_t **Lookup_serial(CA_DB *const db, ASN1_INTEGER *const ser) noexcept
{
    int32_t i{0};
    BIGNUM *bn{nullptr};
    char8_t *itmp{nullptr};
    char8_t *row[DB_NUMBER]{nullptr};
    char8_t **rrow{nullptr};
    for (i = 0; i < DB_NUMBER; i++) {
        row[i] = nullptr;  // NOLINT
    }
    bn = ASN1_INTEGER_to_BN(ser, nullptr);
    OPENSSL_assert(bn != nullptr); /* FIXME: should report an error at this
              * point and abort */
    if (0 != BN_is_zero(bn)) {
        itmp = OPENSSL_strdup("00");
    } else {
        itmp = BN_bn2hex(bn);
    }

    row[DB_serial] = itmp;
    BN_free(bn);
    rrow = TXT_DB_get_by_index_ph(db->db, DB_serial, row);
    OPENSSL_free(itmp);
    return rrow;
}
/// @brief Generate OCSP response
/// @param resp pointer to OCSP_RESPONSE
/// @param req  pointer to OCSP_REQUEST
/// @param db database
/// @param ca ca certificate
/// @param rcert revoked certificate
/// @param rkey   key used to revoke certificate
/// @param rmd pointer to EVP_MD
/// @param flags indication
/// @param nmin minutes
/// @param ndays days
/// @param badsig bad sig
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_04088
/// @trace_id_sr=SR_CRYPTO_03006
/// @unit_name=Make_ocsp_response
/// @needwork = dd
/// @endcode
void Make_ocsp_response(OCSP_RESPONSE **const resp,
                        OCSP_REQUEST *const req,
                        CA_DB *const db,
                        STACK_OF(X509) *const ca,
                        X509 *const rcert,
                        EVP_PKEY *const rkey,
                        EVP_MD const *const rmd,
                        uint64_t const flags,
                        int32_t const nmin,
                        int32_t const ndays,
                        int32_t const badsig) noexcept
{
    ASN1_TIME *thisupd{nullptr};
    ASN1_TIME *nextupd{nullptr};
    OCSP_CERTID *cid{nullptr};
    OCSP_BASICRESP *bs{nullptr};
    int32_t i{0};
    int32_t idCount{0};
    EVP_MD_CTX *mctx{nullptr};
    EVP_PKEY_CTX *pkctx{nullptr};

    idCount = OCSP_request_onereq_count(req);

    if (idCount <= 0) {
        *resp = OCSP_response_create(OCSP_RESPONSE_STATUS_MALFORMEDREQUEST, nullptr);
        Free_Time_Ocsp(mctx, thisupd, nextupd, bs);
        return;
    }

    bs      = OCSP_BASICRESP_new();
    thisupd = X509_gmtime_adj(nullptr, 0);
    if (ndays != -1) {
        nextupd = X509_time_adj_ex(nullptr, ndays, static_cast< long >(nmin) * kInt_60, nullptr);  // NOLINT
    }

    /* Examine each certificate id in the request */
    for (i = 0; i < idCount; i++) {
        OCSP_ONEREQ *one{nullptr};
        ASN1_INTEGER *serial{nullptr};
        char8_t **inf{nullptr};
        int32_t jj{0};
        int32_t found{0};
        ASN1_OBJECT *certIdMdOid{nullptr};
        EVP_MD const *certIdMd{nullptr};
        one = OCSP_request_onereq_get0(req, i);
        cid = OCSP_onereq_get0_id(one);

        std::ignore = OCSP_id_get0_info(nullptr, &certIdMdOid, nullptr, nullptr, cid);

        certIdMd = EVP_get_digestbyobj(certIdMdOid);
        if (certIdMd == nullptr) {
            *resp = OCSP_response_create(OCSP_RESPONSE_STATUS_INTERNALERROR, nullptr);
            Free_Time_Ocsp(mctx, thisupd, nextupd, bs);
            return;
        }
        for (jj = 0; (jj < sk_X509_num(ca)) && (found == 0); jj++) {
            X509 *const caCert{sk_X509_value(ca, jj)};  //NOLINT
            OCSP_CERTID *const caId{OCSP_cert_to_id(certIdMd, nullptr, caCert)};

            if (OCSP_id_issuer_cmp(caId, cid) == 0) {
                found = 1;
            }

            OCSP_CERTID_free(caId);
        }

        if (found == 0) {
            std::ignore = OCSP_basic_add1_status(bs, cid, V_OCSP_CERTSTATUS_UNKNOWN, 0, nullptr, thisupd, nextupd);
            continue;
        }
        std::ignore = OCSP_id_get0_info(nullptr, nullptr, nullptr, &serial, cid);
        inf         = Lookup_serial(db, serial);
        if (inf == nullptr) {
            std::ignore = OCSP_basic_add1_status(bs, cid, V_OCSP_CERTSTATUS_UNKNOWN, 0, nullptr, thisupd, nextupd);
        } else if (inf[DB_type][0] == DB_TYPE_VAL) {
            std::ignore = OCSP_basic_add1_status(bs, cid, V_OCSP_CERTSTATUS_GOOD, 0, nullptr, thisupd, nextupd);
        } else if (inf[DB_type][0] == DB_TYPE_REV) {
            ASN1_OBJECT *inst{nullptr};
            ASN1_TIME *revtm{nullptr};
            ASN1_GENERALIZEDTIME *invtm{nullptr};
            OCSP_SINGLERESP *single{nullptr};
            int32_t reason{-1};
            std::ignore = Unpack_revinfo(&revtm, &reason, &inst, &invtm, inf[DB_rev_date]);
            single      = OCSP_basic_add1_status(bs, cid, V_OCSP_CERTSTATUS_REVOKED, reason, revtm, thisupd, nextupd);
            if (invtm != nullptr) {
                std::ignore = OCSP_SINGLERESP_add1_ext_i2d(single, NID_invalidity_date, invtm, 0, 0U);
            } else if (inst != nullptr) {
                std::ignore = OCSP_SINGLERESP_add1_ext_i2d(single, NID_hold_instruction_code, inst, 0, 0U);
            }
            ASN1_OBJECT_free(inst);
            ASN1_TIME_free(revtm);
            ASN1_GENERALIZEDTIME_free(invtm);
        }
    }

    std::ignore = OCSP_copy_nonce(bs, req);

    mctx = EVP_MD_CTX_new();
    if (mctx == nullptr) {
        if (EVP_DigestSignInit(mctx, &pkctx, rmd, nullptr, rkey) == 0) {
            *resp = OCSP_response_create(OCSP_RESPONSE_STATUS_INTERNALERROR, nullptr);
            Free_Time_Ocsp(mctx, thisupd, nextupd, bs);
            return;
        }
    }
    if (OCSP_basic_sign_ctx(bs, rcert, mctx, nullptr, flags) == 0) {
        *resp = OCSP_response_create(OCSP_RESPONSE_STATUS_INTERNALERROR, bs);
        Free_Time_Ocsp(mctx, thisupd, nextupd, bs);
        return;
    }

    (void)badsig;
    *resp = OCSP_response_create(OCSP_RESPONSE_STATUS_SUCCESSFUL, bs);

    Free_Time_Ocsp(mctx, thisupd, nextupd, bs);
}
/// @brief Release memory pointed to by parameters in the parameter list
/// @param mctx pointer to EVP_MD_CTX
/// @param thisupd pointer to thisupd
/// @param nextupd pointer to nextupd
/// @param bs pointer to bs
void Free_Time_Ocsp(EVP_MD_CTX *mctx, ASN1_TIME *thisupd, ASN1_TIME *nextupd, OCSP_BASICRESP *bs) noexcept
{
    EVP_MD_CTX_free(mctx);
    ASN1_TIME_free(thisupd);
    ASN1_TIME_free(nextupd);
    OCSP_BASICRESP_free(bs);
}
/// @brief Load a certificate
/// @return pointer to certificate structure
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_04089
/// @trace_id_sr=SR_CRYPTO_03006
/// @unit_name=LoadIssureCert
/// @needwork = dd
/// @endcode
X509 *LoadIssureCert() noexcept
{
    X509 *pIssureCert{nullptr};
    /// Certificates and private keys used for OCSP responses are hardcoded
    char8_t const *const registerCert{
        "-----BEGIN CERTIFICATE-----\n"
        "MIIDezCCAmMCFDyIWetR11iR871EAT3JGo8pECltMA0GCSqGSIb3DQEBCwUAMHcx"
        "CzAJBgNVBAYTAkNOMRAwDgYDVQQIDAdCZWlKaW5nMRAwDgYDVQQHDAdCZWlqaW5n"
        "MQswCQYDVQQKDAJQSDEMMAoGA1UECwwDREVWMQswCQYDVQQDDAJDWjEcMBoGCSqG"
        "SIb3DQEJARYNMTIzNDU2QHFxLmNvbTAeFw0yMzA3MjUwNzE5MTlaFw0zMzA3MjIw"
        "NzE5MTlaMH0xCzAJBgNVBAYTAkNOMRAwDgYDVQQIDAdCZWlKaW5nMRAwDgYDVQQH"
        "DAdCZWlKaW5nMQwwCgYDVQQKDANQSDExDTALBgNVBAsMBERFVjExDDAKBgNVBAMM"
        "A0NaMTEfMB0GCSqGSIb3DQEJARYQMTIzNDU2Nzg5QHFxLmNvbTCCASIwDQYJKoZI"
        "hvcNAQEBBQADggEPADCCAQoCggEBANv5N1U6Kl+B4dkViYJxNO94wAcchkqTJt+F"
        "POaRLiuexM7FYjqHx05MZq5xiPMQvnE4uxARTegDqvE+g6W3N2C+o3oGLcY7XTEa"
        "H0ZyDDd0lt/+7zJrUDkXNoKzDkEdacmOmJ6t67Q+QO2ZUQYD2GtN2jHbllEpJC7X"
        "mGAQQbsdeOUIyUIghqf/crDL9fdVTLANkznQDXjBbn8NpBnct8aeyx57ryiAuph2"
        "aMU1K3cfgM9Apfi1YQeBZTTHInYFH12js+3gtXsxXWdMMpCsgOT9KEp9JN6nGVBf"
        "ILz37x/RZGxHwdlh/M3gp80aJItPNhyvenohlZ8ecRZXM/McUisCAwEAATANBgkq"
        "hkiG9w0BAQsFAAOCAQEAaVn9cGSTbiYFfSdBvqjlLkeVe4XT6Y2QyUU0AfNVWnI2"
        "Y4RGRdTUvzakD2K/C1DSpr+TJzMU2emGhK+4LoG95MVtlaskrDuXcBv4hp7qLEzT"
        "2hisf3xIjdPJocqqDxlzqTWcEL2MSi8pms8rLgirBZQr4cFAC3OuVVCxF3xMfVep"
        "GA7bW1Z0AoR/41e3aJNRbW25BnzOEDLBq43zkbVkrF5OpxtqgVlZXwTsl0XJTdtb"
        "/b2MRfUlwClw7v9Mh944QHyF7a/GqrZ3lvtcs59vuwibXRRjkzlkvfv3CIhyauH6"
        "Ck4RDIDrGjunxM6iWyZhfqEOCvrhY3J1rcK85FJt7w==\n"
        "-----END CERTIFICATE-----\n"};

    BIO *const pCertBio{BIO_new_mem_buf(registerCert, static_cast< int32_t >(strnlen(registerCert, SIZE_MAX)))};
    pIssureCert = PEM_read_bio_X509(pCertBio, nullptr, nullptr, nullptr);

    std::ignore = BIO_free(pCertBio);

    return pIssureCert;
}
/// @brief Load a key
/// @return pointer to key structure
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_04090
/// @trace_id_sr=SR_CRYPTO_03006
/// @unit_name=LoadIssureKey
/// @needwork = dd
/// @endcode
EVP_PKEY *LoadIssureKey() noexcept
{
    EVP_PKEY *pIssureKey{nullptr};
    char8_t const *const registerKey{
        "-----BEGIN RSA PRIVATE KEY-----\n"
        "MIIEpAIBAAKCAQEA2/k3VToqX4Hh2RWJgnE073jABxyGSpMm34U85pEuK57EzsVi"
        "OofHTkxmrnGI8xC+cTi7EBFN6AOq8T6Dpbc3YL6jegYtxjtdMRofRnIMN3SW3/7v"
        "MmtQORc2grMOQR1pyY6Ynq3rtD5A7ZlRBgPYa03aMduWUSkkLteYYBBBux145QjJ"
        "QiCGp/9ysMv191VMsA2TOdANeMFufw2kGdy3xp7LHnuvKIC6mHZoxTUrdx+Az0Cl"
        "+LVhB4FlNMcidgUfXaOz7eC1ezFdZ0wykKyA5P0oSn0k3qcZUF8gvPfvH9FkbEfB"
        "2WH8zeCnzRoki082HK96eiGVnx5xFlcz8xxSKwIDAQABAoIBAQDagpRZn/7pgq6j"
        "owZGwkfQPgWaytYHjGdIECVv7/WKNRvRPjeWZrwv5ir1zCg7M33KII2Jp3bZK5OK"
        "Mww4+GgVPSpNE4gy61NmbEl3A+Fo0PLyrV5Umk9Fdu6aJF3bhulH49NthNyn3lNG"
        "8k6PCBDafqppbnTCSHj94wIlIuD4ibW1AZa3w+fOULPoAN/TvmedW289y64ECQQd"
        "duuPKvYiy5kwiny2cdYYbyE4BaZccAK2Lo9xycMukASkMjI9v6yCEE8j28lOZXCC"
        "BuLIhf9whdlQJ8bcAPhLEeiaEoJsBHY6g1H38S0mxYUk8Tl3oiKyK1KpCDIWRV/C"
        "Vjef0Y/pAoGBAPKIpeDaWaOlZEQ4NUdYHUcNBOo0Zcxt1EAbAWT2wSsU4dtdtGeU"
        "F+1g1StorTheIS/Df3w6UqHv60idVrLhEb1AvGPhSZiupdZiZZ9lhXvT9wrgKNb0"
        "lH0a/Ra1ats3HkVcW6haSmY1L9ykNMsCEfTtBqHMPBfFOHcsSv2YaqlXAoGBAOgv"
        "5gR3Gk4vf/T9NUA8Bz5ktrcuPrY+XIx6cUPD4jj6uzvf/KH8GHAfK3OPeaVo5bgZ"
        "rIsUbsCFsQEVe8ZpjLhozXbsep84+wmYT6ntalV8Jtw4i57k5PthDw1YNAMSkF/F"
        "58eszBs+I0C8OFEbDm1JSmFUviTfPJYNprv1rtVNAoGAUGMOZ/q3X1BAbYQb1KvV"
        "+GjOSjpXy2xTv3O0AJzw5bCkM6tZUkN5sso3nY2ybE52vXgU1z4ViWsWFxlfVSbY"
        "VFwSlSBk3mB0wxwWBsEFf0eASHCH2Md0OAsr+8zhwAsC04aLMr/KjRnv5Tff77Gd"
        "9gph+lrRSCesUYOyz18naz0CgYBzJG25ASd2Zm/aGkdyF1KwSlmh9bj7v2k98OqC"
        "ZH2nLA/13Vn3KoEj+REfOaNlt+NTrMe4IOXba3EfzHuZJXa3bKGXvCYDF8rGM1Yu"
        "OqJkyjCrPQ9gOLqlPrisMXV5oJcgJ5CNoM+HIwVwcOassfIxmArewZuZxWrSLurk"
        "gy20cQKBgQC7kvniNXaUIVy05KREhbMqFfDTT4Y/dU6Cmr13kEBrDDlf545FLkIg"
        "9L7ZDqgnueZ1u7LmWWYpi/tsGhc8c3wQoLx/2AEK68+ldPhClbfj5Udh0rz26D8S"
        "YgparYgt4KiSJ46Zq1ykBiWX9XojeLxe+8ZPiE/MrXADs5OlhjR3Mw==\n"
        "-----END RSA PRIVATE KEY-----\n"};

    BIO *const pIssureKeyBio{BIO_new_mem_buf(registerKey, static_cast< int32_t >(strnlen(registerKey, SIZE_MAX)))};
    pIssureKey  = PEM_read_bio_PrivateKey(pIssureKeyBio, nullptr, nullptr, nullptr);
    std::ignore = BIO_free(pIssureKeyBio);
    return pIssureKey;
}
/// @brief Release index
/// @param db database
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_04091
/// @trace_id_sr=SR_CRYPTO_03006
/// @unit_name=Free_index
/// @needwork = dd
/// @endcode
void Free_index(CA_DB *const db) noexcept
{
    if (nullptr != db) {
        TXT_DB_free(db->db);
        OPENSSL_free(db->dbfname);
        OPENSSL_free(db);
    }
}

}  // namespace  isoft_def
}  // namespace openssl
}  // namespace crypto
}  // namespace ara
