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

/// ================================================================
///
/// File description:
/// ----------------
/// @file       x509_extensions.cpp
/// @brief      AutoSar-Crypto Certificate Management Module
/// @details
/// @date       2021-12-21
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Certificate Component/X.509 Extension Data
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_03003
/// @unit_name=X509Extensions
/// @unit_description=Certificate Extension Information
/// @endcode
///
/// ================================================================

#include "ara/crypto/x509/x509_extensions.h"

#include <openssl/asn1.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>

#include "ara/crypto/common/isoft_common_api.h"
#include "ara/crypto/openssl/isoft_openssl_encrypt.h"
#include "ara/crypto/x509/isoft_certificate.h"
#include "ara/crypto/x509/x509_provider.h"

namespace ara {
namespace crypto {
namespace x509 {
//********************************/
/// @brief X.509 Extensions interface.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_03318
/// @trace_id_dd=DD_CRYPTO_06565
/// @needwork = ad
/// @endcode
class PX509Extension
{
private:
    /// @brief Standard x509 extensions
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_03318
    /// @trace_id_dd=DD_CRYPTO_06566
    /// @needwork = dd
    /// @endcode
    X509_EXTENSION *pExtension_{nullptr};

public:
    /// @brief Constructor
    /// @param pExtension x509 extension
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_03318
    /// @trace_id_dd=DD_CRYPTO_06567
    /// @needwork = dd
    /// @endcode
    explicit PX509Extension(X509_EXTENSION *const pExtension) noexcept;
    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_03318
    /// @trace_id_dd=DD_CRYPTO_06568
    /// @needwork = dd
    /// @endcode
    virtual ~PX509Extension() noexcept = default;
    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_03318
    /// @trace_id_dd=DD_CRYPTO_06569
    /// @needwork = dd
    /// @endcode
    PX509Extension() noexcept = default;
    /// @brief Default copy constructor
    /// @param a Another object instance of this class other PX509Extension
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_03318
    /// @trace_id_dd=DD_CRYPTO_06570
    /// @needwork = dd
    /// @endcode
    PX509Extension(PX509Extension const &a) noexcept = delete;
    /// @brief Default move constructor
    /// @param a Another object instance of this class other PX509Extension
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_03318
    /// @trace_id_dd=DD_CRYPTO_06571
    /// @needwork = dd
    /// @endcode
    PX509Extension(PX509Extension &&a) noexcept = delete;
    /// @brief Default copy assignment operator
    /// @param a Another object instance of this class other PX509Extension
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_03318
    /// @trace_id_dd=DD_CRYPTO_06572
    /// @needwork = dd
    /// @endcode
    PX509Extension &operator=(PX509Extension const &a) = delete;
    /// @brief Default move assignment operator
    /// @param a Another object instance of this class other PX509Extension
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_03318
    /// @trace_id_dd=DD_CRYPTO_06573
    /// @needwork = dd
    /// @endcode
    PX509Extension &operator=(PX509Extension &&a) = delete;
    /// @brief Get extension ID
    /// @return nID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_03318
    /// @trace_id_dd=DD_CRYPTO_06574
    /// @needwork = dd
    /// @endcode
    int32_t GetNID() const noexcept;
    /// @brief Get critical
    /// @return Number of Crits
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_03318
    /// @trace_id_dd=DD_CRYPTO_06575
    /// @needwork = dd
    /// @endcode
    int32_t GetCritical() const noexcept;
    /// @brief Get extension name
    /// @return Extension name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_03318
    /// @trace_id_dd=DD_CRYPTO_06576
    /// @needwork = dd
    /// @endcode
    ara::core::String GetName() noexcept;
    /// @brief Get extension content
    /// @return Extension content
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_03318
    /// @trace_id_dd=DD_CRYPTO_06577
    /// @needwork = dd
    /// @endcode
    ara::core::String GetValue() noexcept;
    /// @brief Get extension data
    /// @return Extension data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_03318
    /// @trace_id_dd=DD_CRYPTO_06578
    /// @needwork = dd
    /// @endcode
    ara::core::Vector< ara::core::Byte > GetData() noexcept;
    /// @brief String conversion
    /// @param pOctetString ASN.1 encoded string
    /// @return String
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_03318
    /// @trace_id_dd=DD_CRYPTO_06579
    /// @needwork = dd
    /// @endcode
    static ara::core::Vector< ara::core::Byte > Asn1Octet2Vector(ASN1_OCTET_STRING *const pOctetString) noexcept;

public:
    /// @brief Convert NID to name
    /// @param nID Extension ID
    /// @return Name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_03318
    /// @trace_id_dd=DD_CRYPTO_06580
    /// @needwork = dd
    /// @endcode
    static ara::core::String TransID2Name(int32_t const nID) noexcept;
};
/// @brief Constructor
/// @param pExtension X509_EXTENSION pointer object
PX509Extension::PX509Extension(X509_EXTENSION *const pExtension) noexcept : PX509Extension{}
{
    pExtension_ = pExtension;
}
/// @brief Get extension ID
/// @return Extension ID
int32_t PX509Extension::GetNID() const noexcept
{
    ASN1_OBJECT *const pObject{X509_EXTENSION_get_object(pExtension_)};
    if (nullptr == pObject) {
        return NID_undef;
    }
    int32_t const nID{OBJ_obj2nid(pObject)};
    return nID;
}
/// @brief Get critical
/// @return Number of Crits
int32_t PX509Extension::GetCritical() const noexcept
{
    if (nullptr == pExtension_) {
        return 0;
    }
    int32_t const nID{X509_EXTENSION_get_critical(pExtension_)};
    return nID;
}
/// @brief Get extension name
/// @return Extension name
ara::core::String PX509Extension::GetName() noexcept
{
    ASN1_OBJECT *const pObject{X509_EXTENSION_get_object(pExtension_)};
    if (nullptr == pObject) {
        return ara::core::String();
    }
    int32_t const nID{OBJ_obj2nid(pObject)};
    if (NID_undef == nID) {
        int32_t const extNameLen{1024};

        ara::core::Vector< ara::crypto::char8_t > vecChBuff;
        vecChBuff.resize(static_cast< size_t >(extNameLen));

        std::ignore = OBJ_obj2txt(static_cast< char8_t * >(vecChBuff.data()), extNameLen, pObject, 1);
        return ara::core::String{static_cast< char8_t * >(vecChBuff.data())};
    }

    void const *const pchExtName{static_cast< void const * >(OBJ_nid2ln(nID))};
    if (nullptr == pchExtName) {
        return ara::core::String{};
    }

    return ara::core::String{static_cast< ara::crypto::char8_t const * >(pchExtName)};
}
/// @brief Get extension content
/// @return Extension content
ara::core::String PX509Extension::GetValue() noexcept
{
    BIO *const extBio{BIO_new(BIO_s_mem())};
    if (nullptr == extBio) {
        return ara::core::String();
    }
    ara::core::String stReturn;

    int32_t const nLen{X509V3_EXT_print(extBio, pExtension_, 0U, 0)};
    if (0 != nLen) {
        BUF_MEM *bptr{nullptr};
        std::ignore = PH_BIO_get_mem_ptr(extBio, &bptr);
        std::ignore = PH_BIO_set_close(extBio, BIO_NOCLOSE);
        ara::crypto::char8_t const nCharAlter{'\n'};
        ara::crypto::char8_t const nCharEnter{'\r'};
        // remove newlines
        int32_t const lastchar{static_cast< int32_t >(bptr->length)};
        char8_t &chLastData{*(bptr->data + lastchar - 1)};
        if ((lastchar > 1) && ((chLastData == nCharAlter) || (chLastData == nCharEnter))) {
            chLastData = static_cast< ara::crypto::char8_t >(0);
        }

        stReturn = ara::core::String(bptr->data, bptr->length);
        BUF_MEM_free(bptr);
    }
    std::ignore = BIO_free(extBio);
    return stReturn;
}
/// @brief Get extension data
/// @return Extension data
ara::core::Vector< ara::core::Byte > PX509Extension::GetData() noexcept
{
    ASN1_OCTET_STRING *const pAsn1Octet{X509_EXTENSION_get_data(pExtension_)};
    if (nullptr == pAsn1Octet) {
        return ara::core::Vector< ara::core::Byte >();
    }
    // ASN1_STRING_data
    return Asn1Octet2Vector(pAsn1Octet);
}
/// @brief String conversion
/// @param pOctetString ASN.1 encoded string
/// @return String
ara::core::Vector< ara::core::Byte > PX509Extension::Asn1Octet2Vector(ASN1_OCTET_STRING *const pOctetString) noexcept
{
    ara::core::Vector< ara::core::Byte > vecData;
    uint8_t *pData{nullptr};
    int32_t const nLen{i2d_ASN1_OCTET_STRING(pOctetString, &pData)};
    if (nLen > 0) {
        vecData.reserve(static_cast< std::size_t >(nLen));
        for (int32_t i{0}; i < nLen; ++i) {
            vecData.push_back(ara::core::Byte(*(pData + i)));
        }
    }
    return vecData;
}
/// @brief Convert NID to name
/// @param nID Extension ID
/// @return Name
ara::core::String PX509Extension::TransID2Name(int32_t const nID) noexcept
{
    const void *const pchExtName{T_TransVoid(OBJ_nid2ln(nID))};
    if (nullptr == pchExtName) {
        return ara::core::String{};
    }

    return ara::core::String{static_cast< ara::crypto::char8_t const * >(pchExtName)};
}
//********************************/
/// @brief Publicly serialize itself.
/// @brief Serialize itself publicly.
/// @param formatId  the Crypto Provider specific identifier of the output format
/// @returns a buffer with the serialized object
/// @trace_id_sws={SWS_CRYPT_10711}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02112}
/// @error: SecurityErrorDomain::kInsufficientCapacity   if (output.empty() == false), but it's capacity is less than
/// required
/// @error: SecurityErrorDomain::kUnknownIdentifier      if an unknown format ID was specified
/// @error: SecurityErrorDomain::kUnsupportedFormat      if the specified format ID is not supported for this object
/// type
/// @threadsafety={Thread-safe}
ara::core::Result< ara::core::Vector< ara::core::Byte > > X509Extensions::ExportPublicly(
    FormatId formatId) const noexcept
{
    /// SecurityErrorDomain::kUnknownIdentifier      if an unknown format ID was specified
    if (false == isoft_def::PCertificate::IsValidFormatID(formatId, false)) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kUnknownIdentifier);
    }
    /// SecurityErrorDomain::kUnsupportedFormat      if the specified format ID is not supported for this object
    /// type
    if (false == isoft_def::PCertificate::IsValidFormatID(formatId, true)) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kUnsupportedFormat);
    }
    std::function< int32_t(BIO *, FunctionId) > const func{[this](BIO *const pbio, FunctionId const funcId) -> int32_t {
        if (funcId == kFuncDer) {
            int32_t const ret{ASN1_item_i2d_bio(ASN1_ITEM_rptr(X509_EXTENSIONS), pbio, pExtStack_)};
            return ret;
        }
        if (funcId == kFuncPem) {
            BIO *const pPublic{BIO_new(BIO_s_mem())};
            int32_t ret{ASN1_item_i2d_bio(ASN1_ITEM_rptr(X509_EXTENSIONS), pPublic, pExtStack_)};
            if (ret > 0) {
                int32_t const nNeedLen{static_cast< int32_t >(PH_BIO_pending(pPublic))};
                int32_t const nBase64Len{static_cast< int32_t >(nNeedLen * 8 / 6 + 3)};
                ara::core::Vector< ara::core::Byte > vecDatatemp;
                vecDatatemp.resize(static_cast< std::size_t >(nNeedLen));
                std::ignore = BIO_read(pPublic, vecDatatemp.data(), nNeedLen);
                uint8_t *const pBuffBase64{new uint8_t[static_cast< u_long >(nBase64Len)]};
                ret         = EVP_EncodeBlock(pBuffBase64, T_TransBytes(vecDatatemp.data()), nNeedLen);
                std::ignore = BIO_write(pbio, pBuffBase64, ret);
                delete[] pBuffBase64;
            }
            std::ignore = BIO_free(pPublic);
            return ret;
        }
        return 0;
    }};
    return ExportPublic_Fun(formatId, func);
}
//********************************/
/// @brief Calculate the number of elements in the sequence.
/// @brief Count number of elements in the sequence.
/// @return number of elements in the sequence
/// @trace_id_sws={SWS_CRYPT_40511}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02306}
/// @threadsafety={Thread-safe}
std::size_t X509Extensions::Count() const noexcept { return sk_X509_EXTENSION_num(pExtStack_); }
//********************************/
/// @brief Constructor
/// @param x509Provider Certificate provider
X509Extensions::X509Extensions(X509Provider &x509Provider) noexcept : X509Object{x509Provider}
{
    pExtStack_ = sk_X509_EXTENSION_new_null();  //NOLINT
}
/// @brief Destructor
X509Extensions::~X509Extensions() noexcept { sk_X509_EXTENSION_pop_free(pExtStack_, &X509_EXTENSION_free); }
//***************/
/// @brief Set X.509 extensions
/// @param pExtStack X.509 extensions
/// @return ture if update sucess false otherwise
bool X509Extensions::UpdateExtensions(X509_EXTENSIONS const *const pExtStack) noexcept
{
    if (nullptr == pExtStack) {
        return false;
    }
    if (nullptr == pExtStack_) {
        return false;
    }
    _clearExtensions();
    int32_t const nTotal{X509v3_get_ext_count(pExtStack)};

    for (int32_t i{0}; i < nTotal; ++i) {
        X509_EXTENSION *const pExtension{X509v3_get_ext(pExtStack, i)};
        if (nullptr == pExtension) {
            return false;
        }
        if (sk_X509_EXTENSION_push(pExtStack_, pExtension) < 0) {
            return false;
        }
    }
    return true;
}
/// @brief Set X.509 extensions
/// @param pX509 Standard format certificate
/// @return ture if update sucess false otherwise
bool X509Extensions::UpdateExtensions(X509 const *const pX509) noexcept
{
    if (nullptr == pX509) {
        return false;
    }
    if (nullptr == pExtStack_) {
        return false;
    }
    _clearExtensions();
    int32_t const nTotal{X509_get_ext_count(pX509)};
    for (int32_t i{0}; i < nTotal; ++i) {
        X509_EXTENSION *const pExtension{X509_get_ext(pX509, i)};
        if (nullptr == pExtension) {
            return false;
        }

        if (sk_X509_EXTENSION_push(pExtStack_, pExtension) < 0) {
            return false;
        }
    }
    return true;
}
/// @brief Add extension item
/// @param nID Extension item ID
/// @param nCrit Criticality flag
/// @param stData Extension item content
/// @return ture if AddExtension sucess false otherwise
bool X509Extensions::AddExtension(int32_t const nID, bool const nCrit, ara::core::String const &stData) noexcept
{
    if (nullptr == pExtStack_) {
        return false;
    }
    X509_EXTENSION *pX509Extension = _createExtension(nID, nCrit, stData);
    if (pX509Extension == nullptr) {
        return false;
    }
    if (sk_X509_EXTENSION_push(pExtStack_, pX509Extension) < 0) {
        return false;
    }

    return true;
}
/// @brief Remove extension item
/// @param nID Extension item ID
/// @return ture if DelExtension sucess false otherwise
bool X509Extensions::DelExtension(int32_t const nID) noexcept
{
    if (nullptr == pExtStack_) {
        return false;
    }
    int const delNum{_deleteAllExtensionsByNid(nID)};
    return delNum > 0;
}
/// @brief Find extension data by ID
/// @param nID Extension item ID
/// @return ture if FindExtension sucess false otherwise
bool X509Extensions::FindExtension(int32_t const nID) noexcept
{
    if (nullptr == pExtStack_) {
        return false;
    }
    ara::core::Vector< X509_EXTENSION * > results;
    _findAllExtensionsByNid(nID, results);
    return results.empty() == false;
}
//********************************/

}  // namespace x509
}  // namespace crypto
}  // namespace ara
