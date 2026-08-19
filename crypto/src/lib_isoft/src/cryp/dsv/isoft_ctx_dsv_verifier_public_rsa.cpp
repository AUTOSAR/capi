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
/// @file       isoft_ctx_dsv_verifier_public_rsa.cpp
/// @brief      AutoSar-Crypto Encryption/Decryption Module
/// @details
/// @date       2022-03-23
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default Encryption/Signature Storage
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_01009
/// @unit_name=PCtxDsv_VerifierPublic_Rsa
/// @unit_description=Signature Verification RSA Public Key Context Interface
/// @endcode
///
/// ================================================================

#include "ara/crypto/cryp/dsv/isoft_ctx_dsv_verifier_public_rsa.h"

#include "ara/crypto/cryp/cryobj/isoft_key_public_ipc.h"
#include "ara/crypto/cryp/cryobj/isoft_key_public_rsa.h"
#include "ara/crypto/cryp/isoft_crypto_provider.h"
#include "ara/crypto/cryp/isoft_service_signature.h"
#include "ara/crypto/cryp/select/isoft_select_rsa.h"
#include "ara/crypto/openssl/isoft_openssl_encrypt.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
//********************************/
/// @brief Signature verification public key context interface.
//********************************/
/// @brief Get the maximum supported key length in bits
/// @returns  maximal supported length of the key in bits
std::size_t PCtxDsv_VerifierPublic_Rsa::GetMaxKeyBitLength() const noexcept
{
    return static_cast< std::size_t >(PAlgId_Asymmetric_RsaKey::EKeyLen::kRsaKeyMaxBitLength);
}
/// @brief Get the minimum supported key length in bits.
/// @returns minimal supported key length in bits.
std::size_t PCtxDsv_VerifierPublic_Rsa::GetMinKeyBitLength() const noexcept
{
    return static_cast< std::size_t >(PAlgId_Asymmetric_RsaKey::EKeyLen::kRsaKeyMinBitLength);
}
/// @brief Verify support for a specific key length based on the context.
/// @param keyBitLength Key length: in bits
/// @returns  @c true if provided value of the key length is supported by the context
bool PCtxDsv_VerifierPublic_Rsa::IsKeyBitLengthSupported(std::size_t keyBitLength) const noexcept
{
    if ((keyBitLength < GetMinKeyBitLength()) || (keyBitLength > GetMaxKeyBitLength())) {
        return false;
    }
    return 0U == (keyBitLength % static_cast< std::size_t >(PAlgId_Asymmetric_RsaKey::EKeyLen::kRsaBlockStep));
}
/// @brief Get the size of the signature value produced and required by the current algorithm.
/// @returns  Size of the produced and required signature value
std::size_t PCtxDsv_VerifierPublic_Rsa::GetSignatureSize() const noexcept
{
    PKeyPublic_Rsa const *const pRsaKey{
        static_cast< PKeyPublic_Rsa const * >(static_cast< void const * >(GetPublicKey()))};
    if (pRsaKey != nullptr) {
        return pRsaKey->GetPayloadSize();
    }
    return 0U;
}
//***************/
/// @brief Check if Key meets requirements
/// @param key Public key
/// @returns true if check key sucess false otherwise
bool PCtxDsv_VerifierPublic_Rsa::CheckKey(PublicKey const &key) const noexcept
{
    PAlgId_Asymmetric_RsaKey const cryptoKey;
    if (false == cryptoKey.IsMinePrimitiveId(key.GetCryptoPrimitiveId()->GetPrimitiveId())) {
        return false;
    }

    return key.CheckKey();
}
/// @brief Execute signature encryption logic
/// @param pInputData Start address of input data in algorithm operation
/// @param nDataLen Data length
/// @param suppressPadding Whether padding is suppressed
/// @param pDigestData Digest data
/// @param nDigestDataLen Digest data length
/// @return Encrypted data
bool PCtxDsv_VerifierPublic_Rsa::DoDecrypto(uint8_t const *pInputData,
                                            uint32_t nDataLen,
                                            bool suppressPadding,
                                            uint8_t const *const pDigestData,
                                            uint32_t nDigestDataLen) const noexcept
{
    if (pDigestData == nullptr) {
        return false;
    }
    if (nDigestDataLen == 0U) {
        return false;
    }
    if (pInputData == nullptr) {
        return false;
    }
    if (nDataLen == 0U) {
        return false;
    }

    PKeyPublic_Ipc_Rsa const *const pKeyPublicIpc{dynamic_cast< PKeyPublic_Ipc_Rsa const * >(GetPublicKey())};
    ara::core::Vector< ara::core::Byte > vecDecrypto;
    if (pKeyPublicIpc != nullptr) {
        PSelectRsa selectRsa{pKeyPublicIpc->GetSlotId()};
        int32_t const nPadding{suppressPadding ? RSA_NO_PADDING : RSA_PKCS1_PADDING};
        vecDecrypto = std::move(selectRsa.PublicDecrypto(pInputData, nDataLen, nPadding)).Value();
    } else {
        PKeyPublic_Rsa const *const pRsaKey{
            static_cast< PKeyPublic_Rsa const * >(static_cast< void const * >(GetPublicKey()))};
        PSelectRsa selectRsa{pRsaKey->GetRsa()};
        int32_t const nPadding{suppressPadding ? RSA_NO_PADDING : RSA_PKCS1_PADDING};
        vecDecrypto = std::move(selectRsa.PublicDecrypto(pInputData, nDataLen, nPadding)).Value();
    }

    if (vecDecrypto.empty()) {
        return false;
    }

    if (vecDecrypto.size() != nDigestDataLen) {
        return false;
    }
    // Compare signatures
    uint8_t const *const pSignatureData{
        static_cast< uint8_t const * >(static_cast< void const * >(vecDecrypto.data()))};
    for (uint32_t i{0U}; i < nDigestDataLen; ++i) {
        if (*(pSignatureData + i) != *(pDigestData + i)) {
            return false;
        }
    }
    return true;
}
//********************************/
/// @brief Parameterized constructor
/// @param cryptoProvider Encryption provider
PCtxDsv_VerifierPublic_Rsa_Md5::PCtxDsv_VerifierPublic_Rsa_Md5(PCryptoProvider &cryptoProvider) noexcept
    : PCtxDsv_VerifierPublic_Rsa{cryptoProvider}
{
    _SetContextAglId(static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kHashMd5));
}
//***************/  //CryptoContext interface
/// @brief Return a CryptoPrimitivId instance containing the instance identifier.
/// @returns  CryptoPrimitiveId instance
CryptoPrimitiveId::Uptr PCtxDsv_VerifierPublic_Rsa_Md5::GetCryptoPrimitiveId() const noexcept
{
    return {std::make_unique< PAlgId_Dsv_VerifierPublicRsa_Md5 >()};
}
/// @brief Extended service member class.
/// @returns  SignatureService instance
SignatureService::Uptr PCtxDsv_VerifierPublic_Rsa_Md5::GetSignatureService() const noexcept
{
    return {std::make_unique< PServiceSignature< PCtxDsv_VerifierPublic_Rsa_Md5 > >(*this)};
}
/// @brief Check if equal
/// @param nSigCryId Crypto primitive ID of the encryption algorithm used in the signature algorithm
/// @returns true if Macth sucess false otherwise
bool PCtxDsv_VerifierPublic_Rsa_Md5::IsCryptoAlgIdMacthing(CryptoPrimitiveId::AlgId const nSigCryId) const noexcept
{
    if (nSigCryId == static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kDsv_SignerPrivateRsaMd5)) {
        return true;
    }
    return false;
}
//-----------------------------------------------------------------------------------------------------------------//
/// @brief Parameterized constructor
/// @param cryptoProvider Encryption provider
PCtxDsv_VerifierPublic_Rsa_Sha1::PCtxDsv_VerifierPublic_Rsa_Sha1(PCryptoProvider &cryptoProvider) noexcept
    : PCtxDsv_VerifierPublic_Rsa{cryptoProvider}
{
    _SetContextAglId(static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kHashSha1));
}
//***************/  //CryptoContext interface
/// @brief Return a CryptoPrimitivId instance containing the instance identifier.
/// @returns  CryptoPrimitiveId instance
CryptoPrimitiveId::Uptr PCtxDsv_VerifierPublic_Rsa_Sha1::GetCryptoPrimitiveId() const noexcept
{
    return {std::make_unique< PAlgId_Dsv_VerifierPublicRsa_Sha1 >()};
}
/// @brief Extended service member class.
/// @returns  SignatureService instance
SignatureService::Uptr PCtxDsv_VerifierPublic_Rsa_Sha1::GetSignatureService() const noexcept
{
    return {std::make_unique< PServiceSignature< PCtxDsv_VerifierPublic_Rsa_Sha1 > >(*this)};
}
/// @brief Check if equal
/// @param nSigCryId Crypto primitive ID of the encryption algorithm used in the signature algorithm
/// @returns true if Macth sucess false otherwise
bool PCtxDsv_VerifierPublic_Rsa_Sha1::IsCryptoAlgIdMacthing(CryptoPrimitiveId::AlgId const nSigCryId) const noexcept
{
    if (nSigCryId == static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kDsv_SignerPrivateRsaSha1)) {
        return true;
    }
    return false;
}
//-----------------------------------------------------------------------------------------------------------------//
/// @brief Parameterized constructor
/// @param cryptoProvider Encryption provider
PCtxDsv_VerifierPublic_Rsa_Sha2_224::PCtxDsv_VerifierPublic_Rsa_Sha2_224(PCryptoProvider &cryptoProvider) noexcept
    : PCtxDsv_VerifierPublic_Rsa{cryptoProvider}
{
    _SetContextAglId(static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kHashSha2_224));
}
//***************/  //CryptoContext interface
/// @brief Return a CryptoPrimitivId instance containing the instance identifier.
/// @returns  CryptoPrimitiveId instance
CryptoPrimitiveId::Uptr PCtxDsv_VerifierPublic_Rsa_Sha2_224::GetCryptoPrimitiveId() const noexcept
{
    return {std::make_unique< PAlgId_Dsv_VerifierPublicRsa_Sha2_224 >()};
}
/// @brief Extended service member class.
/// @returns  SignatureService instance
SignatureService::Uptr PCtxDsv_VerifierPublic_Rsa_Sha2_224::GetSignatureService() const noexcept
{
    return {std::make_unique< PServiceSignature< PCtxDsv_VerifierPublic_Rsa_Sha2_224 > >(*this)};
}
/// @brief Check if equal
/// @param nSigCryId Crypto primitive ID of the encryption algorithm used in the signature algorithm
/// @returns true if Macth sucess false otherwise
bool PCtxDsv_VerifierPublic_Rsa_Sha2_224::IsCryptoAlgIdMacthing(CryptoPrimitiveId::AlgId const nSigCryId) const noexcept
{
    if (nSigCryId == static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kDsv_SignerPrivateRsaSha2_224)) {
        return true;
    }
    return false;
}
//-----------------------------------------------------------------------------------------------------------------//
/// @brief Parameterized constructor
/// @param cryptoProvider Encryption provider
PCtxDsv_VerifierPublic_Rsa_Sha2_256::PCtxDsv_VerifierPublic_Rsa_Sha2_256(PCryptoProvider &cryptoProvider) noexcept
    : PCtxDsv_VerifierPublic_Rsa{cryptoProvider}
{
    _SetContextAglId(static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kHashSha2_256));
}
//***************/  //CryptoContext interface
/// @brief Return a CryptoPrimitivId instance containing the instance identifier.
/// @returns  CryptoPrimitiveId instance
CryptoPrimitiveId::Uptr PCtxDsv_VerifierPublic_Rsa_Sha2_256::GetCryptoPrimitiveId() const noexcept
{
    return {std::make_unique< PAlgId_Dsv_VerifierPublicRsa_Sha2_256 >()};
}
/// @brief Extended service member class.
/// @returns  SignatureService instance
SignatureService::Uptr PCtxDsv_VerifierPublic_Rsa_Sha2_256::GetSignatureService() const noexcept
{
    return {std::make_unique< PServiceSignature< PCtxDsv_VerifierPublic_Rsa_Sha2_256 > >(*this)};
}
/// @brief Check if equal
/// @param nSigCryId Crypto primitive ID of the encryption algorithm used in the signature algorithm
/// @returns true if Macth sucess false otherwise
bool PCtxDsv_VerifierPublic_Rsa_Sha2_256::IsCryptoAlgIdMacthing(CryptoPrimitiveId::AlgId const nSigCryId) const noexcept
{
    if (nSigCryId == static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kDsv_SignerPrivateRsaSha2_256)) {
        return true;
    }
    return false;
}
//-----------------------------------------------------------------------------------------------------------------//
/// @brief Parameterized constructor
/// @param cryptoProvider Encryption provider
PCtxDsv_VerifierPublic_Rsa_Sha2_384::PCtxDsv_VerifierPublic_Rsa_Sha2_384(PCryptoProvider &cryptoProvider) noexcept
    : PCtxDsv_VerifierPublic_Rsa{cryptoProvider}
{
    _SetContextAglId(static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kHashSha2_384));
}
//***************/  //CryptoContext interface
/// @brief Return a CryptoPrimitivId instance containing the instance identifier.
/// @returns  CryptoPrimitiveId instance
CryptoPrimitiveId::Uptr PCtxDsv_VerifierPublic_Rsa_Sha2_384::GetCryptoPrimitiveId() const noexcept
{
    return {std::make_unique< PAlgId_Dsv_VerifierPublicRsa_Sha2_384 >()};
}
/// @brief Extended service member class.
/// @returns  SignatureService instance
SignatureService::Uptr PCtxDsv_VerifierPublic_Rsa_Sha2_384::GetSignatureService() const noexcept
{
    return {std::make_unique< PServiceSignature< PCtxDsv_VerifierPublic_Rsa_Sha2_384 > >(*this)};
}
/// @brief Check if equal
/// @param nSigCryId Crypto primitive ID of the encryption algorithm used in the signature algorithm
/// @returns true if Macth sucess false otherwise
bool PCtxDsv_VerifierPublic_Rsa_Sha2_384::IsCryptoAlgIdMacthing(CryptoPrimitiveId::AlgId const nSigCryId) const noexcept
{
    if (nSigCryId == static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kDsv_SignerPrivateRsaSha2_384)) {
        return true;
    }
    return false;
}
//-----------------------------------------------------------------------------------------------------------------//
/// @brief Parameterized constructor
/// @param cryptoProvider Encryption provider
PCtxDsv_VerifierPublic_Rsa_Sha2_512::PCtxDsv_VerifierPublic_Rsa_Sha2_512(PCryptoProvider &cryptoProvider) noexcept
    : PCtxDsv_VerifierPublic_Rsa{cryptoProvider}
{
    _SetContextAglId(static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kHashSha2_512));
}
//***************/  //CryptoContext interface
/// @brief Return a CryptoPrimitivId instance containing the instance identifier.
/// @returns  CryptoPrimitiveId instance
CryptoPrimitiveId::Uptr PCtxDsv_VerifierPublic_Rsa_Sha2_512::GetCryptoPrimitiveId() const noexcept
{
    return {std::make_unique< PAlgId_Dsv_VerifierPublicRsa_Sha2_512 >()};
}
/// @brief Extended service member class.
/// @returns  SignatureService instance
SignatureService::Uptr PCtxDsv_VerifierPublic_Rsa_Sha2_512::GetSignatureService() const noexcept
{
    return {std::make_unique< PServiceSignature< PCtxDsv_VerifierPublic_Rsa_Sha2_512 > >(*this)};
}
/// @brief Check if equal
/// @param nSigCryId Crypto primitive ID of the encryption algorithm used in the signature algorithm
/// @returns true if Macth sucess false otherwise
bool PCtxDsv_VerifierPublic_Rsa_Sha2_512::IsCryptoAlgIdMacthing(CryptoPrimitiveId::AlgId const nSigCryId) const noexcept
{
    if (nSigCryId == static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kDsv_SignerPrivateRsaSha2_512)) {
        return true;
    }
    return false;
}
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara
