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
/// @file       isoft_cert_sign_request.cpp
/// @brief      AutoSar-Crypto Certificate management module
/// @details
/// @date       2021-12-21
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Certificate component/Certificate request
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_03005
/// @unit_name=PCertSignRequest
/// @unit_description=Certificate signing request (CSR) object interface
/// @endcode
///
/// ================================================================

#include "ara/crypto/x509/isoft_cert_sign_request.h"

#include <utility>

#include "ara/crypto/common/entry_point.h"
#include "ara/crypto/cryp/cryobj/isoft_key_private_rsa.h"
#include "ara/crypto/cryp/cryobj/isoft_key_public.h"
#include "ara/crypto/cryp/cryobj/isoft_signature.h"
#include "ara/crypto/openssl/isoft_openssl_encrypt.h"
#include "ara/crypto/x509/isoft_x509_provider.h"

namespace ara {
namespace crypto {
namespace x509 {
namespace isoft_def {
//********************************/
/// @brief Certificate signing request (CSR) object interface. This interface is specifically used for complete parsing of the request content.
//********************************/    //PCertSignRequest interface
/// @brief Verify the self-signed signature of the certificate request.
/// @returns @c true if the signature is correct
bool PCertSignRequest::Verify() const noexcept
{
    if (nullptr == pReqX509_) {
        return false;
    }
    EVP_PKEY *const pkey{X509_REQ_get_pubkey(pReqX509_)};
    if (nullptr == pkey) {
        return false;
    }
    int32_t const nReturn{X509_REQ_verify(pReqX509_, pkey)};
    EVP_PKEY_free(pkey);
    return nReturn > 0;
}
/// @brief Export this certificate signing request in DER-encoded ASN1 format. Note: This is the CSR that can be sent to a CA to obtain a certificate. // The 2011 document does not have the "const" modifier
/// @return
/// @error: SecurityErrorDomain::kInvalidUsageOrder  this error will be returned in case not all required information
/// has been provided
ara::core::Result< ara::core::Vector< ara::core::Byte > > PCertSignRequest::ExportASN1CertSignRequest() const noexcept
{
    BIO *const pNewBio{BIO_new(BIO_s_mem())};
    ara::core::Vector< ara::core::Byte > vecData;
    int32_t const nNeedLen{i2d_X509_REQ_bio(pNewBio, pReqX509_)};  // Output DER format encoding
    if (nNeedLen <= 0) {
        std::ignore = BIO_free(pNewBio);
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kInvalidUsageOrder);
    }
    size_t const len{BIO_ctrl_pending(pNewBio)};
    if (len == 0U) {
        std::ignore = BIO_free(pNewBio);
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kInvalidUsageOrder);
    }
    vecData.resize(static_cast< std::size_t >(len));
    std::ignore = BIO_read(pNewBio, static_cast< void * >(vecData.data()), static_cast< int32_t >(len));
    std::ignore = BIO_free(pNewBio);

    return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromValue(vecData);
}
/// @brief Return the format version of the certificate request.
/// @return format version of the certificate request
uint32_t PCertSignRequest::Version() const noexcept
{
    int32_t const nVersion{static_cast< int32_t >(X509_REQ_get_version(pReqX509_))};
    return static_cast< uint32_t >(nVersion) + 1U;
}
/// @brief Return the signature object of the request.
/// @brief Return signature object of the request.
/// @returns signature object of the request
/// @trace_id_sws={SWS_CRYPT_40315}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02306}
/// @threadsafety={Thread-safe}
ara::crypto::cryp::Signature const &PCertSignRequest::GetSignature() const noexcept
{
    if (nullptr == pSignature_) {
        ara::crypto::cryp::isoft_def::PSignature::Uptr pSignature{
            std::make_unique< ara::crypto::cryp::isoft_def::PSignature >(nSignHashAlgId_)};

        ara::core::Vector< ara::core::Byte > vecData;
        std::ignore = _ExportCert_Der(vecData);
        ara::core::Byte *const pData{vecData.data()};
        pSignature->SetSignatureData(nSignHashAlgId_, pData, static_cast< uint32_t >(vecData.size()));
        pSignature_ = std::move(pSignature);
    }
    return *pSignature_;
}
//********************************/ //Serializable interface
/// @brief Publicly serialize itself.
/// @param formatId Data format: Raw, DER, PEM, etc.
/// @return a buffer with the serialized object
ara::core::Result< ara::core::Vector< ara::core::Byte > > PCertSignRequest::ExportPublicly(
    FormatId formatId) const noexcept
{
    /// @error: SecurityErrorDomain::kInsufficientCapacity   if (output.empty() == false), but it's capacity is less
    /// than required
    /// @error: SecurityErrorDomain::kUnknownIdentifier      if an unknown format ID was specified
    if (false == PCertificate::IsValidFormatID(formatId, false)) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kUnknownIdentifier);
    }
    /// @error: SecurityErrorDomain::kUnsupportedFormat      if the specified format ID is not supported for this object
    /// type
    if (false == PCertificate::IsValidFormatID(formatId, true)) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kUnsupportedFormat);
    }
    std::function< int32_t(BIO *, FunctionId) > const func{[this](BIO *const pbio, FunctionId const funcId) -> int32_t {
        if (funcId == kFuncDer) {
            int32_t const ret{i2d_X509_REQ_bio(pbio, pReqX509_)};
            return ret;
        }
        if (funcId == kFuncPem) {
            int32_t const ret{PEM_write_bio_X509_REQ(pbio, pReqX509_)};
            return ret;
        }
        return 0;
    }};
    return ExportPublic_Fun(formatId, func);
}
//********************************/ //BasicCertInfo interface
/// @brief Get the key constraints of the key associated with this PKCS#10 object.
/// @brief Get the key constraints for the key associated with this PKCS#10 object.
/// @returns key constraints
/// @trace_id_sws={SWS_CRYPT_40115}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02306}
/// @threadsafety={Thread-safe}
BasicCertInfo::KeyConstraints PCertSignRequest::GetConstraints() const noexcept
{
    KeyConstraints const nReturn{GetkConstrNone()};
    return nReturn;
}
/// @brief Get the path length constraint defined in the basic constraints extension.
/// @brief Get the constraint on the path length defined in the Basic Constraints extension.
/// @returns certification path length limit
/// @trace_id_sws={SWS_CRYPT_40114}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02306}
/// @threadsafety={Thread-safe}
std::uint32_t PCertSignRequest::GetPathLimit() const noexcept  // basicConstraints=critical,CA:true,pathlen:1
{
    return 0U;
}
/// @brief Check whether the CA attribute of the X509v3 Basic Constraints extension is true (i.e., pathlen=0).
/// @brief Check whether the CA attribute of X509v3 Basic Constraints is true (i.e. pathlen=0).
/// @returns @c true if it is a CA request and @c false otherwise
/// @trace_id_sws={SWS_CRYPT_40113}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02306}
/// @threadsafety={Thread-safe}
bool PCertSignRequest::IsCa() const noexcept  // basicConstraints=critical,CA:true,pathlen:1
{
    return false;
}
/// @brief Get the subject DN.
/// @return subject DN
X509DN const &PCertSignRequest::SubjectDn() const noexcept
{
    if (dnSubject_.GetAttrCount() <= 0) {
        std::ignore = _InitSubjectDn(pReqX509_);
    }

    return dnSubject_;
}
/// @brief Load the subject public key information object into the realm of the specified crypto provider.
///         If (cryptoProvider == nullptr), the X509PublicKeyInfo object will be loaded into the realm of the stack-default crypto provider.
/// @brief Load the subject public key information object to realm of specified crypto provider.
///         If (cryptoProvider == nullptr) then @c X509PublicKeyInfo object will be loaded in realm of the Stack-default
///         Crypto Provider.
/// @param cryptoProvider Crypto provider, unique pointer of a target crypto provider, where the public key will be used
/// @returns constant reference of the subject public key interface
/// @trace_id_sws={SWS_CRYPT_40111}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02306}
/// @threadsafety={Thread-safe}
X509PublicKeyInfo const &PCertSignRequest::SubjectPubKey(cryp::CryptoProvider *cryptoProvider) const noexcept
{
    std::ignore = cryptoProvider;
    X509_PUBKEY *const pKeyPublic{X509_REQ_get_X509_PUBKEY(pReqX509_)};
    if (pKeyPublic == nullptr) {
        return keyPublic_;
    }
    std::ignore = keyPublic_.UpdatePubkeyData(pKeyPublic, nSignHashAlgId_, nHashSize_, nSignatureSize_);

    ara::core::Result< ara::crypto::cryp::PublicKey::Uptrc > const resPublicKey{keyPublic_.GetPublicKey()};
    if (!resPublicKey.HasValue()) {
        return keyPublic_;
    }
    ara::crypto::cryp::isoft_def::PKeyPublic_Base const *const pPublicKey{
        dynamic_cast< ara::crypto::cryp::isoft_def::PKeyPublic_Base const * >(resPublicKey.Value().get())};
    if (nullptr == pPublicKey) {
        return keyPublic_;
    }
    ara::core::Result< ara::core::Vector< ara::core::Byte > > resVec{
        pPublicKey->ExportPublicly(Serializable::kFormatPemEncoded)};
    if (!resVec.HasValue()) {
        return keyPublic_;
    }
    ara::core::Vector< ara::core::Byte > vecData{std::move(resVec).Value()};
    if (vecData.empty()) {
        return keyPublic_;
    }
    ara::core::String const pubData{T_TransChar(&vecData[0U]), vecData.size()};
    /// The certificate signing request does not have a unique identifier, so the UUID of the public key is used as the map key.
    uint64_t mQwordMs{pPublicKey->GetObjectId().mCouid.mGeneratorUid.mQwordMs};
    ara::core::String serialNumber;
    while (true) {
        if (mQwordMs <= 0U) {
            break;
        }
        serialNumber += static_cast< char8_t >(mQwordMs % kInt_10U + kInt_48U);
        mQwordMs /= kInt_10U;
    }
    if (cryptoProvider == nullptr) {
        ara::core::InstanceSpecifier const iSpecify{ara::core::StringView("isoft")};
        LoadCryptoProvider(iSpecify)->InsertSubjectPubKey(serialNumber, pubData);
    } else {
        cryptoProvider->InsertSubjectPubKey(serialNumber, pubData);
    }
    return keyPublic_;
}
//********************************/
/// @brief Constructor
/// @param x509Provider Certificate provider
PCertSignRequest::PCertSignRequest(X509Provider &x509Provider) noexcept
    : PCertSignRequest{
        x509Provider, nullptr, static_cast< CryptoAlgId >(ara::crypto::cryp::isoft_def::EPhCtxTypeID::kHashSha1),
        0U,           0U,      X509_REQ_new(),
        nullptr,      nullptr, {}}
{
}
/// @brief Full parameter constructor qac
/// @param x509Provider Certificate provider
/// @param pSignature   Signature object
/// @param nSignHashAlgId Hash algorithm ID
/// @param nHashSize Hash length
/// @param nSignatureSize Signature length
/// @param pReqX509            Certificate signing request object
/// @param dnSubject        DN subject
/// @param keyPublic        Public key
/// @param stCertFileName   Certificate file path
PCertSignRequest::PCertSignRequest(X509Provider &x509Provider,
                                   ara::crypto::cryp::Signature::Uptr pSignature,
                                   CryptoAlgId const nSignHashAlgId,
                                   uint32_t const nHashSize,
                                   uint32_t const nSignatureSize,
                                   X509_REQ *const pReqX509,
                                   X509DN const *const dnSubject,
                                   X509PublicKeyInfo const *const keyPublic,
                                   ara::core::String stCertFileName) noexcept
    : CertSignRequest{x509Provider}
    , pSignature_{std::move(pSignature)}
    , nSignHashAlgId_{nSignHashAlgId}
    , nHashSize_{nHashSize}
    , nSignatureSize_{nSignatureSize}
    , pReqX509_{pReqX509}
    , dnSubject_{x509Provider}
    , keyPublic_{}  // NOLINT
    , stCertFileName_{std::move(stCertFileName)}
{
    std::ignore = dnSubject;
    std::ignore = keyPublic;
}
/// @brief Destructor
PCertSignRequest::~PCertSignRequest() noexcept { _DeleteX509Req(); }
/// @brief Load a certificate from memory
/// @param memCert Certificate memory data
/// @param formatID Certificate encoding format
/// @returns true load cert sucess false otherwise
bool PCertSignRequest::LoadCertFile(ReadOnlyMemRegion const &memCert, FormatId const formatID) noexcept
{
    X509_REQ *pX509Req{nullptr};
    if ((nullptr == pX509Req) && ((kFormatDefault == formatID) || (kFormatDerEncoded == formatID))) {
        pX509Req = _LoadCertFile_Der(memCert);
    }
    if ((nullptr == pX509Req) && ((kFormatDefault == formatID) || (kFormatPemEncoded == formatID))) {
        pX509Req = _LoadCertFile_Pem(memCert);
    }
    return _InitCertData(pX509Req);
}
/// @brief Set the version number: the actual value will be decremented by 1
/// @param nVersion Version number
/// @returns true if set version sucess false otherwise
bool PCertSignRequest::SetVersion(int32_t const nVersion) noexcept
{
    if (nullptr == pReqX509_) {
        return false;
    }
    int32_t const nReturn{X509_REQ_set_version(pReqX509_, static_cast< int64_t >(nVersion) - 1)};
    return nReturn > 0;
}
/// @brief Set SubjectDn
/// @param memDn    Certificate DN information
/// @param formatID DN encoding format
/// @returns true if SetSubjectDn sucess false otherwise
bool PCertSignRequest::SetSubjectDn(ReadOnlyMemRegion const &memDn, FormatId const formatID) noexcept
{
    if (nullptr == pReqX509_) {
        return false;
    }
    int32_t nReturn{0};
    if (formatID == Serializable::kFormatDerEncoded) {
        uint8_t const *pData{memDn.data()};
        X509_NAME *const pX509Name{d2i_X509_NAME(nullptr, &pData, static_cast< int64_t >(memDn.size()))};
        nReturn = X509_REQ_set_subject_name(pReqX509_, pX509Name);
    }
    return nReturn > 0;
}
/// @brief Set extended attributes
/// @param memExt   Extension data information
/// @param formatID DN encoding format
/// @returns true if SetExtension sucess false otherwise
bool PCertSignRequest::SetExtension(ReadOnlyMemRegion const &memExt, FormatId const formatID) noexcept
{
    if (nullptr == pReqX509_) {
        return false;
    }
    if (memExt.empty()) {
        return true;
    }
    STACK_OF(X509_EXTENSION) * exts{nullptr};
    if (formatID == Serializable::kFormatDerEncoded) {
        uint8_t const *pData{memExt.data()};
        exts = d2i_X509_EXTENSIONS(nullptr, &pData, static_cast< int64_t >(memExt.size()));
    } else {
        return false;
    }
    int32_t const ret{X509_REQ_add_extensions(pReqX509_, exts)};
    sk_X509_EXTENSION_pop_free(exts, &X509_EXTENSION_free);
    return ret > 0;
}
/// @brief Set public key information, including public key, signature algorithm, signature value, etc.
/// @returns  true if set PublicKey sucess false otherwise
/// @param pPrivateKey Private key object pointer
bool PCertSignRequest::SetPublicKey(cryp::PrivateKey const *const pPrivateKey) noexcept
{
    cryp::isoft_def::PKeyPrivate_Rsa const *const pKeyPrivateRsa{
        dynamic_cast< cryp::isoft_def::PKeyPrivate_Rsa const *const >(pPrivateKey)};
    if (nullptr == pKeyPrivateRsa) {
        return false;
    }

    EVP_PKEY *const pEvpKey{pKeyPrivateRsa->GetRsa()};
    int32_t nReturn = X509_REQ_set_pubkey(pReqX509_, pEvpKey);
    if (nReturn <= 0) {
        return false;
    }
    return true;
}

/// @brief Set the signature, including hash algorithm, hash length, signature length, etc.
/// @param nHashAlgId Hash algorithm cryptographic primitive ID
/// @param nHashSize  Hash length
/// @param nSignatureSize Signature length
/// @returns true if set Signatur sucess false otherwise
bool PCertSignRequest::SetSignatur(CryptoAlgId const nHashAlgId,
                                   uint32_t const nHashSize,
                                   uint32_t const nSignatureSize) noexcept
{
    nSignHashAlgId_ = nHashAlgId;
    nHashSize_      = nHashSize;
    nSignatureSize_ = nSignatureSize;
    // Signature
    EVP_PKEY *const pEvpKey{X509_REQ_get0_pubkey(pReqX509_)};
    if (nullptr == pEvpKey) {
        return false;
    }
    int32_t const nNid{static_cast< int32_t >(Trans2Nid(nHashAlgId))};
    EVP_MD const *const pDigest{EVP_get_digestbynid(nNid)};

    int32_t const nReturn{X509_REQ_sign(pReqX509_, pEvpKey, pDigest)};
    return nReturn > 1;
}
/// @brief Associate an X509_REQ
/// @param pX509Req Certificate signing request object
/// @return true if Attach X509Req sucess false otherwise
bool PCertSignRequest::AttachX509Req(X509_REQ *const pX509Req) noexcept
{
    _DeleteX509Req();
    pReqX509_ = pX509Req;
    return true;
}

/// @brief Return a clone of itself
/// @return self Instance
PCertSignRequest::Uptr PCertSignRequest::CloneSelf() const noexcept
{
    PCertSignRequest::Uptr pReturn{std::make_unique< PCertSignRequest >(MyProvider())};
    X509_REQ *pNewX509{nullptr};
    if (pReqX509_ != nullptr) {
        pNewX509 = X509_REQ_dup(pReqX509_);
    }
    std::ignore              = pReturn->AttachX509Req(pNewX509);
    pReturn->stCertFileName_ = stCertFileName_;
    return pReturn;
}
//********************************/
/// @brief Load a certificate request in DER format
/// @param memCert Certificate memory data
/// @return X509_REQ structure pointer
X509_REQ *PCertSignRequest::_LoadCertFile_Der(ReadOnlyMemRegion const &memCert) noexcept
{
    X509_REQ *pX509Req{nullptr};
    uint8_t const *pDataIn{memCert.data()};
    int32_t const nLenIn{static_cast< int32_t >(memCert.size())};
    pX509Req = d2i_X509_REQ(&pReqX509_, &pDataIn, static_cast< int64_t >(nLenIn));

    return pX509Req;
}
/// @brief Load a certificate request in PEM format
/// @param memCert Certificate memory data
/// @return X509_REQ structure pointer
X509_REQ *PCertSignRequest::_LoadCertFile_Pem(ReadOnlyMemRegion const &memCert) noexcept
{
    X509_REQ *pX509Req{nullptr};
    BIO *const pBio{BIO_new_mem_buf(memCert.data(), static_cast< int32_t >(memCert.size()))};
    pX509Req    = PEM_read_bio_X509_REQ(pBio, &pReqX509_, nullptr, nullptr);
    std::ignore = BIO_free(pBio);
    return pX509Req;
}
/// @brief Export a certificate request in DER format
/// @returns ture if export cert sucess false otherwise
/// @param vecData Buffer to receive the exported certificate file
bool PCertSignRequest::_ExportCert_Der(ara::core::Vector< ara::core::Byte > &vecData) const noexcept
{
    BIO *const pNewBio{BIO_new(BIO_s_mem())};
    if (i2d_X509_REQ_bio(pNewBio, pReqX509_) <= 0) {
        return false;
    }
    int32_t const nNeedLen{PH_BIO_pending(pNewBio)};
    vecData.clear();
    if (nNeedLen > 0) {
        vecData.resize(static_cast< std::size_t >(nNeedLen));
        std::ignore = BIO_read(pNewBio, static_cast< void * >(vecData.data()), nNeedLen);
    }
    std::ignore = BIO_free(pNewBio);
    return nNeedLen > 0;
}
/// @brief Export a certificate request in PEM format
/// @returns ture if export cert sucess false otherwise
/// @param vecData Buffer to receive the exported certificate file
bool PCertSignRequest::_ExportCert_Pem(ara::core::Vector< ara::core::Byte > &vecData) const noexcept
{
    BIO *const pNewBio{BIO_new(BIO_s_mem())};
    int32_t const nNeedLen{PEM_write_bio_X509_REQ(pNewBio, pReqX509_)};
    vecData.clear();
    if (nNeedLen > 0) {
        vecData.resize(static_cast< std::size_t >(nNeedLen));
        std::ignore = BIO_read(pNewBio, static_cast< void * >(vecData.data()), nNeedLen);
    }
    std::ignore = BIO_free(pNewBio);
    return nNeedLen > 0;
}
/// @brief Initialize some certificate attribute information
/// @param pX509Req Certificate signing request
/// @returns ture if init cert sucess false otherwise
bool PCertSignRequest::_InitCertData(X509_REQ const *const pX509Req) const noexcept
{
    std::ignore = pX509Req;
    if (pReqX509_ == nullptr) {
        return false;
    }
    return _InitSubjectDn(pReqX509_);
}
/// @brief Initialize some certificate attribute information
/// @param pX509Req Certificate signing request
/// @returns ture if init SubjectDn sucess false otherwise
bool PCertSignRequest::_InitSubjectDn(X509_REQ const *const pX509Req) const noexcept
{
    X509_NAME *const pCommonName{X509_REQ_get_subject_name(pX509Req)};
    if (nullptr == pCommonName) {
        return false;
    }
    std::ignore = dnSubject_.SetX509Name(pCommonName);
    return true;
}
/// @brief Process the obtained extension data via NID
/// @param nID NID
/// @param pfunc Callback function
/// @return ture if DealAttr By ID sucess false otherwise
bool PCertSignRequest::_DealAttrByID(
    int32_t const nID, std::function< bool(uint8_t const *pData, int32_t nLen) > const &pfunc) const noexcept
{
    if (nullptr == pReqX509_) {
        return false;
    }
    int32_t const nIndex{X509_REQ_get_attr_by_NID(pReqX509_, nID, -1)};
    if (-1 == nIndex) {
        return false;
    }
    X509_ATTRIBUTE *const attr{X509_REQ_get_attr(pReqX509_, nIndex)};
    ASN1_TYPE *const ext{X509_ATTRIBUTE_get0_type(attr, 0)};
    if (ext == nullptr) {
        return false;
    }
    if (ext->type != V_ASN1_SEQUENCE) {
        return false;
    }
    uint8_t *const pData{ext->value.sequence->data};  // NOLINT
    int32_t const nLen{ext->value.sequence->length};  // NOLINT
    return pfunc(pData, nLen);
}
/// @brief Get the digest information in the certificate request
/// @returns  ture if get digest sucess false otherwise
/// @param vecData Buffer to receive the exported certificate file
bool PCertSignRequest::_GetDigest(ara::core::Vector< ara::core::Byte > &vecData) const noexcept
{
    int32_t const nNid{static_cast< int32_t >(Trans2Nid(nSignHashAlgId_))};
    if (NID_undef == nNid) {
        return false;
    }
    EVP_MD const *const pDigest{EVP_get_digestbynid(nNid)};
    ara::core::Vector< uint8_t > vecOutData;
    vecOutData.resize(kInt_256U);
    uint8_t *const pOutData{vecOutData.data()};

    uint32_t nDataLen{0U};
    int32_t const nReturn{X509_REQ_digest(pReqX509_, pDigest, static_cast< uint8_t * >(pOutData), &nDataLen)};
    vecData.reserve(static_cast< std::size_t >(nDataLen));
    vecData.clear();
    for (uint32_t i{0U}; i < nDataLen; ++i) {
        vecData.push_back(ara::core::Byte(*(pOutData + i)));
    }
    return nReturn != 0;
}
/// @brief Delete the X509_REQ structure
void PCertSignRequest::_DeleteX509Req() noexcept
{
    if (pReqX509_ != nullptr) {
        X509_REQ_free(pReqX509_);
        pReqX509_ = nullptr;
    }
}
//********************************/
}  // namespace  isoft_def
}  // namespace x509
}  // namespace crypto
}  // namespace ara
