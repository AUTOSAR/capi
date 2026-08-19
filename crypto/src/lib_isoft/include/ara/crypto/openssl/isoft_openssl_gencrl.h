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
// https://github.com/openssl/openssl/blob/openssl-3.5.1/apps/include/apps.h
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
/// @file       isoft_openssl_gencrl.h
/// @brief      AutoSar-Crypto configuration
/// @details    Functions related to certificate revocation list
/// @date       2023-11-28
/// @author     Che Jinzhao
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author       <th>Description
/// <tr>}<2023-11-28  </td>1.0.0    </td>Che Jinzhao      <td>Create initial version</td>
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Certificate component/Certificate revocation
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_03006
/// @unit_name=CRL
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_OPENSSL_PUHUA_OPENSSL_GENCRL_H_
#define ARA_CRYPTO_OPENSSL_PUHUA_OPENSSL_GENCRL_H_

#include <openssl/ocsp.h>
#include <openssl/txt_db.h>
#include <sys/stat.h>

#include "ara/crypto/common/isoft_data_type.h"

namespace ara {
namespace crypto {
namespace openssl {
namespace isoft_def {

/// @brief Some macro definitions used
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_06581
/// @needwork = dd
/// @endcode
#define B_FORMAT_TEXT 0x8000
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_06582
/// @needwork = dd
/// @endcode
#define FORMAT_TEXT (1 | B_FORMAT_TEXT) /* Generic text */
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_06583
/// @needwork = dd
/// @endcode
#define FORMAT_ASN1 4 /* ASN.1/DER */
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_06584
/// @needwork = dd
/// @endcode
#define FORMAT_PEM (5 | B_FORMAT_TEXT)
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_06585
/// @needwork = dd
/// @endcode
#define FORMAT_PKCS12 6
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_06586
/// @needwork = dd
/// @endcode
#define FORMAT_ENGINE 8 /* Not really a file format */
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_06587
/// @needwork = dd
/// @endcode
#define FORMAT_MSBLOB 11 /* MS Key blob format */
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_06588
/// @needwork = dd
/// @endcode
#define FORMAT_PVK 12 /* MS PVK file format */
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_06589
/// @needwork = dd
/// @endcode
#define FORMAT_HTTP 13 /* Download using HTTP */
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_06590
/// @needwork = dd
/// @endcode
#define DB_type 0
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_06591
/// @needwork = dd
/// @endcode
#define DB_exp_date 1
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_06592
/// @needwork = dd
/// @endcode
#define DB_rev_date 2
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_06593
/// @needwork = dd
/// @endcode
#define DB_serial 3
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_06594
/// @needwork = dd
/// @endcode
#define DB_file 4
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_06595
/// @needwork = dd
/// @endcode
#define DB_name 5 /* index - unique when active and not */
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_06596
/// @needwork = dd
/// @endcode
#define DB_NUMBER 6
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_06597
/// @needwork = dd
/// @endcode
#define DB_TYPE_REV 'R' /* Revoked  */
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_06598
/// @needwork = dd
/// @endcode
#define DB_TYPE_EXP 'E' /* Expired  */
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_06599
/// @needwork = dd
/// @endcode
#define DB_TYPE_VAL 'V' /* Valid ; inserted with: ca ... -valid */
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_06600
/// @needwork = dd
/// @endcode
#define DB_TYPE_SUSP 'S' /* Suspended  */
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_06601
/// @needwork = dd
/// @endcode
#define BASE_SECTION "ca"
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_06602
/// @needwork = dd
/// @endcode
#define ENV_DEFAULT_CA "default_ca"
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_06603
/// @needwork = dd
/// @endcode
#define ENV_DATABASE "database"
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_06604
/// @needwork = dd
/// @endcode
#define ENV_CRLEXT "crl_extensions"
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_06605
/// @needwork = dd
/// @endcode
#define ENV_UNIQUE_SUBJECT "unique_subject"
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_06606
/// @needwork = dd
/// @endcode
#define ENV_PRIVATE_KEY "private_key"
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_06607
/// @needwork = dd
/// @endcode
#define ENV_CERTIFICATE "certificate"
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_06608
/// @needwork = dd
/// @endcode
#define ENV_DEFAULT_MD "default_md"
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_06609
/// @needwork = dd
/// @endcode
#define ENV_CRLNUMBER "crlnumber"
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_06610
/// @needwork = dd
/// @endcode
#define ENV_DEFAULT_CRL_DAYS "default_crl_days"
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_06611
/// @needwork = dd
/// @endcode
#define ENV_DEFAULT_CRL_HOURS "default_crl_hours"
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_06612
/// @needwork = dd
/// @endcode
#define PW_MIN_LENGTH 4
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_06613
/// @needwork = dd
/// @endcode
#define SERIAL_RAND_BITS 159 /* index - unique */
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_06614
/// @needwork = dd
/// @endcode
#define BSIZE 256
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_06615
/// @needwork = dd
/// @endcode
#define APP_PASS_LEN 1024
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_06616
/// @needwork = dd
/// @endcode
#define _UC(c) (static_cast< uint8_t >(c))
//--------------------------------------------------------------------------------------------------------------//
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01572
/// @trace_id_dd=DD_CRYPTO_04038
/// @trace_id_sr=SR_CRYPTO_03006
/// @unit_name=Pw_cb_data
/// @needwork = ad
/// @endcode
struct Pw_cb_data
{
    /// @brief Password
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06650
    /// @needwork = dda
    /// @endcode
    void const *password;
    /// @brief Prompt message
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06651
    /// @needwork = dda
    /// @endcode
    char8_t const *promptInfo;
};
/// @brief PW_CB_DATA structure
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_03156
/// @trace_id_dd=DD_CRYPTO_06359
/// @trace_id_sr=SR_CRYPTO_03006
/// @unit_name=PW_CB_DATA
/// @needwork = ad
/// @endcode
using PW_CB_DATA = Pw_cb_data;
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_06635
/// @trace_id_sr=SR_CRYPTO_03006
/// @unit_name=OSSL_NELEM
/// @needwork = dd
/// @endcode
#define OSSL_NELEM(x) (sizeof(x) / sizeof((x)[0]))
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_06636
/// @trace_id_sr=SR_CRYPTO_03006
/// @unit_name=NUM_REASONS
/// @needwork = dd
/// @endcode
#define NUM_REASONS OSSL_NELEM(g_CrlReasons)

/// @brief Unpack revocation list information
/// @param prevtm A constant pointer to an ASN1_TIME type pointer. ASN1_TIME is commonly used to represent time-related data structures.
/// @param preason A constant pointer to an int32_t integer. May be used to receive or store an integer value representing a reason or status code.
/// @param phold Constant pointer to ASN1_OBJECT type pointer
/// @param pinvtm Constant pointer to ASN1_GENERALIZEDTIME type pointer. ASN1_GENERALIZEDTIME is commonly used to represent general time format data structures.
/// @param str A constant pointer to constant char8_t (possibly a character type)
/// @return 1 sucess 0 failed
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
                       char8_t const *const str) noexcept;
/// @brief Parse revocation reason code and related parameters
/// @param reasonStr [in] Reason code string
/// @param argStr [in] Additional parameter string (may be nullptr)
/// @param hold [out] Temporarily stores the hold instruction object
/// @param phold [out] Output parameter, returns the hold instruction object
/// @param compTime [out] Output parameter, returns the invalidity time object
/// @return Reason code on successful parsing, -1 on failure
/// @note For hold instructions and invalidity times, corresponding ASN.1 objects will be created based on the parameters
int32_t Get_ReasonCode(char8_t *reasonStr,
                       char8_t *argStr,
                       ASN1_OBJECT **hold,
                       ASN1_OBJECT **const phold,
                       ASN1_GENERALIZEDTIME **compTime) noexcept;
/// @brief Release ASN1 structure resources
/// @param tmp Copy of the input string (needs to be freed)
/// @param compTime ASN1 generalized time structure object pointer (needs to be freed)
/// @param ret Function return status code
/// @return The passed return status code
int32_t Free_ASN1_Source(char8_t *tmp, ASN1_GENERALIZEDTIME *compTime, int32_t ret) noexcept;
/*-
 * Convert revocation field to X509_REVOKED entry
 * return code:
 * 0 error
 * 1 OK
 * 2 OK and some extensions added (i.e. V2 CRL)
 */
/// @brief Revoke
/// @param rev Pointer to X509_REVOKED
/// @param str Pointer to char
/// @return 0 error 1 OK 2 OK and some extensions added
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_04041
/// @trace_id_sr=SR_CRYPTO_03006
/// @unit_name=Make_revoked
/// @needwork = dd
/// @endcode
int32_t Make_revoked(X509_REVOKED *const rev, char8_t const *const str) noexcept;
/// @brief Release OpenSSL ASN.1 object resources to avoid memory leaks
/// @param hold Pointer to ASN1_OBJECT, representing the hold instruction code (e.g., NID_hold_instruction_code). Generated by Unpack_revinfo() parsing, may be nullptr.
/// @param compTime Pointer to ASN1_GENERALIZEDTIME, representing the certificate invalidity time (e.g., NID_invalidity_date). Generated by Unpack_revinfo() parsing, may be nullptr.
/// @param rtmp Pointer to ASN1_ENUMERATED, representing the revocation reason code (e.g., NID_crl_reason). Dynamically allocated by ASN1_ENUMERATED_new(), stores the ASN.1 encoding of reasonCode.
/// @param revDate Pointer to ASN1_TIME, representing the certificate revocation time. Generated by Unpack_revinfo() parsing, must be non-nullptr (otherwise i == 0 returns early). Set into the CRL entry via X509_REVOKED_set_revocationDate().
void Free_ObjectSource(ASN1_OBJECT *hold,
                       ASN1_GENERALIZEDTIME *compTime,
                       ASN1_ENUMERATED *rtmp,
                       ASN1_TIME *revDate) noexcept;
/// @brief Next
/// @param format Format
/// @return 0 sucess 1 failed
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_04042
/// @trace_id_sr=SR_CRYPTO_03006
/// @unit_name=Istext
/// @needwork = dd
/// @endcode
int32_t Istext(int32_t const format) noexcept;
/// @brief Mode
/// @param mode Mode
/// @param format Format
/// @return Mode string
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_04043
/// @trace_id_sr=SR_CRYPTO_03006
/// @unit_name=Modestr
/// @needwork = dd
/// @endcode
char8_t const *Modestr(char8_t const mode, int32_t const format) noexcept;

/// @brief Action mode, returns the corresponding action based on the parameter mode
/// @param mode Mode
/// @return Mode string
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_04044
/// @trace_id_sr=SR_CRYPTO_03006
/// @unit_name=Modeverb
/// @needwork = dd
/// @endcode
const char8_t *Modeverb(char8_t const mode) noexcept;

/// @brief Default open input BIO
/// @param filename File name
/// @param mode Mode
/// @param format Format
/// @param quiet Whether quiet
/// @return Input file data
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
                      int32_t const quiet) noexcept;

/// @brief Allocate memory
/// @param sz Size of memory to allocate (in bytes)
/// @param what A constant pointer to a constant character, typically used to describe the purpose or use of the memory being allocated
/// @return Starting address of allocated memory
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_04046
/// @trace_id_sr=SR_CRYPTO_03006
/// @unit_name=App_malloc
/// @needwork = dd
/// @endcode
void *App_malloc(int32_t const sz, char8_t const *const what) noexcept;

/// @brief Buffer
/// @param fp File pointer
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_04047
/// @trace_id_sr=SR_CRYPTO_03006
/// @unit_name=Unbuffer
/// @needwork = dd
/// @endcode
void Unbuffer(FILE *const fp) noexcept;

//--------------------------------------------------------------------------------------------------------------//
//                                                                                                              //
//                                                                                                              //
//                                                                                                              //
//                                               Database related modules                                                   //
//                                                                                                              //
//                                                                                                              //
//                                                                                                              //
//--------------------------------------------------------------------------------------------------------------//
/// @brief CA_DB attribute structure
///        unique_subject is a boolean indicating whether subject uniqueness is required in the certificate database.
///       If unique_subject is set to 1, then certificates added to the certificate database must have unique subject names. Adding a new certificate fails if a certificate with the same subject name already exists.
///       If unique_subject is set to 0, the certificate database allows multiple certificates with the same subject name. This is typically allowed in a CA certificate database.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_03157
/// @trace_id_dd=DD_CRYPTO_06360
/// @trace_id_sr=SR_CRYPTO_03006
/// @unit_name=DB_ATTR
/// @needwork = ad
/// @endcode
using DB_ATTR =
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01395
    /// @trace_id_dd=DD_CRYPTO_04048
    /// @needwork = dd
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01395
    /// @trace_id_dd=DD_CRYPTO_04049
    /// @trace_id_sr=SR_CRYPTO_03006
    /// @unit_name=Db_attr_st
    /// @needwork = dd
    /// @endcode
    struct Db_attr_st
{
    int32_t uniqueSubject;
};

/// @brief Database structure
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_03158
/// @trace_id_dd=DD_CRYPTO_06361
/// @trace_id_sr=SR_CRYPTO_03006
/// @unit_name=CA_DB
/// @needwork = ad
/// @endcode
using CA_DB =
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01395
    /// @trace_id_dd=DD_CRYPTO_04050
    /// @needwork = dd
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01395
    /// @trace_id_dd=DD_CRYPTO_04051
    /// @trace_id_sr=SR_CRYPTO_03006
    /// @unit_name=Ca_db_st
    /// @needwork = dd
    /// @endcode
    struct Ca_db_st
{
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01395
    /// @trace_id_dd=DD_CRYPTO_06637
    /// @needwork = dd
    /// @endcode
    DB_ATTR attributes;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01395
    /// @trace_id_dd=DD_CRYPTO_06638
    /// @needwork = dd
    /// @endcode
    TXT_DB *db;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01395
    /// @trace_id_dd=DD_CRYPTO_06639
    /// @needwork = dd
    /// @endcode
    char8_t *dbfname;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_01395
    /// @trace_id_dd=DD_CRYPTO_06640
    /// @needwork = dd
    /// @endcode
    struct stat dbst;
};

/// @brief Load database file
/// @param dbData DB data extracted from persistent file
/// @param dblen Length of DB data extracted from persistent file
/// @param uniqueSubject Unique identifier for issued certificate
/// @return Starting address of database file
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_04052
/// @trace_id_sr=SR_CRYPTO_03006
/// @unit_name=Load_index
/// @needwork = dd
/// @endcode
CA_DB *Load_index(char8_t const *const dbData, int32_t const dblen, bool const uniqueSubject) noexcept;
/// @brief Load index
/// @param dbfile Database file pointer
/// @param dbAttr Pointer to database attribute structure
/// @return Starting address of database file
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_04053
/// @trace_id_sr=SR_CRYPTO_03006
/// @unit_name=Load_index2
/// @needwork = dd
/// @endcode
CA_DB *Load_index2(char8_t const *const dbfile, DB_ATTR *const dbAttr) noexcept;

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
void Clear_db(CA_DB *const db) noexcept;
/// @brief Check time format
/// @param str String
/// @return 0 sucess false otherwise
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_04055
/// @trace_id_sr=SR_CRYPTO_03006
/// @unit_name=Check_time_format
/// @needwork = dd
/// @endcode
int32_t Check_time_format(char8_t const *const str) noexcept;

/// @brief Index name
/// @param a Input string A
/// @return 1 or 0
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_04056
/// @trace_id_sr=SR_CRYPTO_03006
/// @unit_name=Index_name_qual
/// @needwork = dd
/// @endcode
int32_t Index_name_qual(char8_t **const a) noexcept;
/// @brief Index serialization hash operation
/// @param a Input string A
/// @return Hash result
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_04057
/// @trace_id_sr=SR_CRYPTO_03006
/// @unit_name=Index_serial_hash
/// @needwork = dd
/// @endcode
uint64_t Index_serial_hash(const OPENSSL_CSTRING *const a) noexcept;

/// @brief Index serialization comparison
/// @param a Input string A
/// @param b Input string B
/// @return 0 equal
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_04058
/// @trace_id_sr=SR_CRYPTO_03006
/// @unit_name=Index_serial_cmp
/// @needwork = dd
/// @endcode
int32_t Index_serial_cmp(const OPENSSL_CSTRING *const a, const OPENSSL_CSTRING *const b) noexcept;

/// @brief Index name comparison
/// @param a Input string A
/// @param b Input string B
/// @return 0 equal
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_04059
/// @trace_id_sr=SR_CRYPTO_03006
/// @unit_name=Index_name_cmp
/// @needwork = dd
/// @endcode
int32_t Index_name_cmp(const OPENSSL_CSTRING *const a, const OPENSSL_CSTRING *const b) noexcept;
/// @brief Index name hash operation
/// @param a Input string A
/// @return Hash result
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_04060
/// @trace_id_sr=SR_CRYPTO_03006
/// @unit_name=Index_name_hash
/// @needwork = dd
/// @endcode
uint64_t Index_name_hash(const OPENSSL_CSTRING *const a) noexcept;

/// @brief Create database index
/// @param db Database
/// @param field Field
/// @param qual Function pointer to define a "quality" or "qualification" function
/// @param hash Function pointer to define hash function
/// @param cmp Function pointer to define comparison function
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
                               OPENSSL_LH_COMPFUNC const cmp) noexcept;
/// @brief Create index for certificate database db
/// @param db Database, pointer to CA_DB type
/// @return  Returns > 0 on success, <= 0 on error
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_04066
/// @trace_id_sr=SR_CRYPTO_03006
/// @unit_name=Index_index
/// @needwork = dd
/// @endcode
int32_t Index_index(CA_DB *const db) noexcept;
/// @brief Check database
/// @param db Database, pointer to CA_DB type
/// @return true check sucess false otherwise
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_04067
/// @trace_id_sr=SR_CRYPTO_03006
/// @unit_name=CheckDatabase
/// @needwork = dd
/// @endcode
bool CheckDatabase(CA_DB *const db) noexcept;

/// @brief /* Update the db file for expired certificates */
/// @brief Revoking a certificate means revoking a certificate that is still within its validity period, for security reasons, as it is potentially revocable.
///         While making a certificate become expired through updatedb results in the certificate expiring and being permanently invalid.
/// @param db Database, pointer of type CA_DB
/// @return Number of revoked certificates
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_04068
/// @trace_id_sr=SR_CRYPTO_03006
/// @unit_name=Do_updatedb
/// @needwork = dd
/// @endcode
int32_t Do_updatedb(CA_DB *const db) noexcept;

/// @brief Save db to bio
/// @param dbfile Database file
/// @param db Database, pointer of type CA_DB
/// @return 1 save sucess 0 failed
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_04069
/// @trace_id_sr=SR_CRYPTO_03006
/// @unit_name=Save_index
/// @needwork = dd
/// @endcode
int32_t Save_index(BIO **const out, CA_DB *const db) noexcept;

/// @brief Update database
/// @param db   Database pointer
/// @param out  Updated database content; if null, indicates no update occurred
/// @return true update db sucess false otherwise
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_04070
/// @trace_id_sr=SR_CRYPTO_03006
/// @unit_name=UpdateDb
/// @needwork = dd
/// @endcode
bool UpdateDb(CA_DB *const db, BIO **const out) noexcept;

/// @brief Load db data from persistent file; if there are updates, synchronize data back to the persistent file
/// @param uniqueSubject Unique identifier for issuing certificates. Default value is zero, which allows multiple certificates with the same subject to exist simultaneously in the CA index file; otherwise, only one is allowed.
/// @param dbData Starting address of db file content
/// @param dblen  Length of db file content
/// @param bioOut Memory data for updating persistent file; if null, indicates no update is needed
/// @return Starting address of the database
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
                       BIO **const bioOut) noexcept;
//--------------------------------------------------------------------------------------------------------------//
//                                                                                                              //
//                                                                                                              //
//                                                                                                              //
//                                              Prepare for CRL Signing                                                 //
//                                                                                                              //
//                                                                                                              //
//                                                                                                              //
//--------------------------------------------------------------------------------------------------------------//
/*
 * Parse message digest name, put it in *EVP_MD; return 0 on failure, else 1.
 */
/// @brief Get the digest algorithm structure pointer corresponding to the specified digest name
/// @param name Digest name
/// @param mdp  Pointer to the digest algorithm structure (output)
/// @return 1 sucess 0 failed
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_04072
/// @trace_id_sr=SR_CRYPTO_03006
/// @unit_name=Opt_md
/// @needwork = dd
/// @endcode
int32_t Opt_md(char8_t const *const name, EVP_MD const **const mdp) noexcept;
/// @brief Digest used for signing the CRL to ensure security
/// @param pkey EVP_PKEY key resource
/// @return Digest information
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_04073
/// @trace_id_sr=SR_CRYPTO_03006
/// @unit_name=ReadyDgest
/// @needwork = dd
/// @endcode
EVP_MD const *ReadyDgest(EVP_PKEY *const pkey) noexcept;
//--------------------------------------------------------------------------------------------------------------//
//                                                                                                              //
//                                                                                                              //
//                                                                                                              //
//                                              CRL Generation Module                                                   //
//                                                                                                              //
//                                                                                                              //
//                                                                                                              //
//--------------------------------------------------------------------------------------------------------------//
/// @brief If GenerateCRLists() fails, call this function to release memory.
///         Do not call if successful, as these variables are still needed later.
/// @param db Database pointer
/// @param crl Certificate Revocation List pointer
/// @param returnFalse Flag identifier
/// @param x509 Certificate pointer
/// @return true scuess false failed
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_04074
/// @trace_id_sr=SR_CRYPTO_03006
/// @unit_name=ClearCacheInGenCRL
/// @needwork = dd
/// @endcode
bool ClearCacheInGenCRL(CA_DB *const db, X509_CRL *const crl, bool const returnFalse, X509 *const x509) noexcept;
/// @brief Serialize random number
/// @param b Pointer to a BIGNUM object
/// @param ai Pointer to an ASN1_INTEGER object
/// @return Random number sequence
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_04075
/// @trace_id_sr=SR_CRYPTO_03006
/// @unit_name=Rand_serial
/// @needwork = dd
/// @endcode
int32_t Rand_serial(BIGNUM *const b, ASN1_INTEGER *const ai) noexcept;
/// @brief Local serialization
/// @param serialfile Serialized file
/// @param create Flag identifier
/// @param retai Double pointer to ASN1_INTEGER
/// @return Serial number
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_04076
/// @trace_id_sr=SR_CRYPTO_03006
/// @unit_name=Load_serial
/// @needwork = dd
/// @endcode
BIGNUM *Load_serial(char8_t const *const serialfile, int32_t const create, ASN1_INTEGER **const retai) noexcept;
/// @brief Save serialization result
/// @param serialfile Serialization file pointer
/// @param suffix Pointer
/// @param serial Serialization data
/// @param retai Pointer to ASN1_INTEGER
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
                    ASN1_INTEGER **const retai) noexcept;

/// @brief Key string
/// @param ctx Context pointer
/// @param value Data participating in algorithm operation
/// @return 0 sucess -1 failed
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_04078
/// @trace_id_sr=SR_CRYPTO_03006
/// @unit_name=Pkey_ctrl_string
/// @needwork = dd
/// @endcode
int32_t Pkey_ctrl_string(EVP_PKEY_CTX *const ctx, char8_t const *const value) noexcept;
/// @brief Signature initialization
/// @param ctx Context pointer
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
int32_t Do_sign_init(EVP_MD_CTX *const ctx, EVP_PKEY *const pkey, EVP_MD const *md) noexcept;

/// @brief Perform signature on X509 Certificate Revocation List
/// @param x Pointer to X509_CRL
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
int32_t Do_X509_CRL_sign(X509_CRL *const x, EVP_PKEY *const pkey, EVP_MD const *const md) noexcept;
/// @brief Rotate serialization
/// @param serialfile This is the name of the serial number file to be processed (including path if necessary)
/// @param newSuffix Used to create a temporary or new version of the serial number file
/// @param oldSuffix This is the old suffix identifying the currently used version of the serial number file; the function attempts to rename the file containing old_suffix to this filename
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
                      char8_t const *const oldSuffix) noexcept;
/// @brief Entry function for generating CRL
/// @param db Database file, loaded from index.txt, storing revoked certificate information
/// @param x509Root X509 root certificate
/// @param dgst Used for signing
/// @param pkey EVP_PKEY key resource
/// @return Pointer to Certificate Revocation List structure
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_04082
/// @trace_id_sr=SR_CRYPTO_03006
/// @unit_name=GenerateCRLs
/// @needwork = dd
/// @endcode
X509_CRL *GenerateCRLs(CA_DB *const db, X509 *const x509Root, EVP_MD const *const dgst, EVP_PKEY *const pkey) noexcept;
/// @brief Execute revoke logic
/// @param db  Database
/// @param crlV2 Records revocation status; if not OCSP_REVOKED_STATUS_NOSTATUS, crlV2 is set to 1
/// @param crl Certificate Revocation List
/// @return Success status
int32_t Do_Revoke_ByCRL(CA_DB *const db, int32_t &crlV2, X509_CRL *crl) noexcept;

//--------------------------------------------------------------------------------------------------------------//
//                                                                                                              //
//                                                                                                              //
//                                                                                                              //
//                                              Certificate Revocation Module                                              //
//                                                                                                              //
//                                                                                                              //
//                                                                                                              //
//--------------------------------------------------------------------------------------------------------------//
/* Additional revocation information types */
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_03159
/// @trace_id_dd=DD_CRYPTO_06362
/// @trace_id_sr=SR_CRYPTO_03006
/// @unit_name=REVINFO_TYPE
/// @needwork = ad
/// @endcode
using REVINFO_TYPE = enum {
    kREV_VALID          = -1, /* Valid (not-revoked) status */
    kREV_NONE           = 0,  /* No additional information */
    kREV_CRL_REASON     = 1,  /* Value is CRL reason code */
    kREV_HOLD           = 2,  /* Value is hold instruction */
    kREV_KEY_COMPROMISE = 3,  /* Value is cert key compromise time */
    kREV_CA_COMPROMISE  = 4   /* Value is CA key compromise time */
};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_06652
/// @trace_id_sr=SR_CRYPTO_03006
/// @unit_name=index_name_cmp_noconst
/// @needwork = dd
/// @endcode
#define index_name_cmp_noconst(a, b)                                                                                   \
    Index_name_cmp((const OPENSSL_CSTRING *)CHECKED_PTR_OF(OPENSSL_STRING, a),                                         \
                   (const OPENSSL_CSTRING *)CHECKED_PTR_OF(OPENSSL_STRING, b))

/*****************************************************************/

/*
 * Given revocation information convert to a DB string. The format of the
 * string is: revtime[,reason,extra]. Where 'revtime' is the revocation time
 * (the current time). 'reason' is the optional CRL reason and 'extra' is any
 * additional argument
 */
/// @brief Revoke
/// @param revType Revocation type
/// @param revArg  Revocation argument
/// @return String
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_04083
/// @trace_id_sr=SR_CRYPTO_03006
/// @unit_name=Make_revocation_str
/// @needwork = dd
/// @endcode
char8_t *Make_revocation_str(REVINFO_TYPE const revType, char8_t const *const revArg) noexcept;

/// @brief Get database entry by index
/// @param db Database
/// @param idx Index
/// @param value Data participating in algorithm operation
/// @return Database index string
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_04084
/// @trace_id_sr=SR_CRYPTO_03006
/// @unit_name=TXT_DB_get_by_index_ph
/// @needwork = dd
/// @endcode
OPENSSL_STRING *TXT_DB_get_by_index_ph(TXT_DB *const db, int32_t const idx, OPENSSL_STRING *const value) noexcept;
/// @brief Revoke
/// @param x509 Certificate
/// @param db  Database
/// @param revType Revocation type
/// @param value Data participating in algorithm operation
/// @return 1 sucess
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_04085
/// @trace_id_sr=SR_CRYPTO_03006
/// @unit_name=Do_revoke
/// @needwork = dd
/// @endcode
int32_t Do_revoke(X509 *const x509, CA_DB *const db, REVINFO_TYPE const revType, const char8_t *const value) noexcept;
/// @brief FunDoWhile encapsulates the code at the location calling this function to reduce the complexity of Do_revoke
/// @param bn Big number extracted from x509
/// @param row This is an array of m rows, m=DB_NUMBER, each m represents a char-type string
/// @param rrow This is an mxn two-dimensional matrix obtained by TXT_DB_get_by_index_ph
/// @param tm Pointer to ASN1_TIME structure, representing the "notAfter" field value of the X.509 certificate. Indicates the expiration time of the certificate.
/// @param x509 X509 certificate
/// @param irow Used to store TXT_DB field values from db
/// @param revType Constant identifier indicating revocation status
/// @param ok Indicates revocation result; 1 means revocation successful
/// @param value Revocation argument
/// @param revStr Revocation type string
/// @param db Database
/// @return Whether this function executed successfully; true indicates success, false will break the loop calling this function
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
                CA_DB *const db) noexcept;
/// @brief Entry function for revoking certificates
/// @param db Database
/// @param revcert Certificate to revoke
/// @param out Output buffer
/// @return true revoke cert sucess
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_04086
/// @trace_id_sr=SR_CRYPTO_03006
/// @unit_name=RevokeCertificate
/// @needwork = dd
/// @endcode
bool RevokeCertificate(CA_DB *const db, X509 *const revcert, BIO **const out) noexcept;
/// @brief Find sequence
/// @param db Database
/// @param ser Serialization data
/// @return Serial number
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_04087
/// @trace_id_sr=SR_CRYPTO_03006
/// @unit_name=Lookup_serial
/// @needwork = dd
/// @endcode
char8_t **Lookup_serial(CA_DB *const db, ASN1_INTEGER *const ser) noexcept;
/// @brief Generate OCSP response
/// @param resp Pointer of type OCSP_RESPONSE
/// @param req  Pointer of type OCSP_REQUEST
/// @param db Database
/// @param ca CA certificate
/// @param rcert Revoked certificate
/// @param rkey  Key used for revoking the certificate
/// @param rmd Pointer to EVP_MD
/// @param flags Flags
/// @param nmin Minutes
/// @param ndays Days
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
                        int32_t const badsig) noexcept;
/// @brief Release memory pointed to by various parameters in the parameter list
/// @param mctx Pointer to EVP_MD_CTX
/// @param thisupd Pointer to thisupd
/// @param nextupd Pointer to nextupd
/// @param bs Pointer to bs
void Free_Time_Ocsp(EVP_MD_CTX *mctx, ASN1_TIME *thisupd, ASN1_TIME *nextupd, OCSP_BASICRESP *bs) noexcept;
/// @brief Load a certificate
/// @return Pointer to certificate structure
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_04089
/// @trace_id_sr=SR_CRYPTO_03006
/// @unit_name=LoadIssureCert
/// @needwork = dd
/// @endcode
X509 *LoadIssureCert() noexcept;
/// @brief Load a key
/// @return Pointer to key structure
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_04090
/// @trace_id_sr=SR_CRYPTO_03006
/// @unit_name=LoadIssureKey
/// @needwork = dd
/// @endcode
EVP_PKEY *LoadIssureKey() noexcept;
/// @brief Release index
/// @param db Database
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_01395
/// @trace_id_dd=DD_CRYPTO_04091
/// @trace_id_sr=SR_CRYPTO_03006
/// @unit_name=Free_index
/// @needwork = dd
/// @endcode
void Free_index(CA_DB *const db) noexcept;

}  // namespace  isoft_def
}  // namespace openssl
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_OPENSSL_PUHUA_OPENSSL_GENCRL_H_
