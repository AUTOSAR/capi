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
/// @file       isoft_ctx_dsv_verifier_public_rsa_pss.cpp
/// @brief      AutoSar-Crypto Encryption/Decryption Module
/// @details
/// @date       2023-10-30
/// @author     Che Jinzhao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default Encryption/Signature Storage
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_01009
/// @unit_name=PCtxDsv_VerifierPublic_Rsa_Pss
/// @unit_description=Signature Verification Public Key RSA-PSS Context Interface
/// @endcode
///
/// ================================================================

#include "ara/crypto/cryp/dsv/isoft_ctx_dsv_verifier_public_rsa_pss.h"

#include "ara/crypto/common/security_error_domain.h"
#include "ara/crypto/cryp/cryobj/isoft_key_public_ipc.h"
#include "ara/crypto/cryp/cryobj/isoft_key_public_rsa.h"
#include "ara/crypto/cryp/hash/isoft_ctx_hash_function.h"
#include "ara/crypto/cryp/isoft_crypto_provider.h"
#include "ara/crypto/cryp/isoft_service_signature.h"
#include "ara/crypto/cryp/select/isoft_select_rsa.h"
#include "ara/crypto/cryp/verifier_public_ctx.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
//********************************/
/// @brief Signature verification public key context interface.
//********************************/
/// @brief CryptoContext interface
/// @param cryptoProvider Encryption provider
PCtxDsv_VerifierPublic_Rsa_Pss::PCtxDsv_VerifierPublic_Rsa_Pss(PCryptoProvider &cryptoProvider) noexcept
    : PCtxDsv_VerifierPublic{cryptoProvider}
{
}
//***************/  //PServiceSignature interface
/// @brief Get the maximum supported key length in bits.
/// @returns  maximal supported length of the key in bits
std::size_t PCtxDsv_VerifierPublic_Rsa_Pss::GetMaxKeyBitLength() const noexcept
{
    return static_cast< std::size_t >(PAlgId_Asymmetric_RsaKey::EKeyLen::kRsaKeyMaxBitLength);
}
/// @brief Get the minimum supported key length in bits.
/// @returns  minimal supported key length in bits.
std::size_t PCtxDsv_VerifierPublic_Rsa_Pss::GetMinKeyBitLength() const noexcept
{
    return static_cast< std::size_t >(PAlgId_Asymmetric_RsaKey::EKeyLen::kRsaKeyMinBitLength);
}
/// @brief Verify support for a specific key length based on the context.
/// @param keyBitLength Key length: in bits
/// @returns  @c true if provided value of the key length is supported by the context
bool PCtxDsv_VerifierPublic_Rsa_Pss::IsKeyBitLengthSupported(std::size_t keyBitLength) const noexcept
{
    if ((keyBitLength < GetMinKeyBitLength()) || (keyBitLength > GetMaxKeyBitLength())) {
        return false;
    }
    return 0U == (keyBitLength % static_cast< std::size_t >(PAlgId_Asymmetric_RsaKey::EKeyLen::kRsaBlockStep));
}
/// @brief Get the size of the signature value produced and required by the current algorithm.
/// @returns Size of the produced and required signature value
std::size_t PCtxDsv_VerifierPublic_Rsa_Pss::GetSignatureSize() const noexcept
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
bool PCtxDsv_VerifierPublic_Rsa_Pss::CheckKey(PublicKey const &key) const noexcept
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
bool PCtxDsv_VerifierPublic_Rsa_Pss::DoDecrypto(uint8_t const *pInputData,
                                                uint32_t nDataLen,
                                                bool suppressPadding,
                                                uint8_t const *const pDigestData,
                                                uint32_t nDigestDataLen) const noexcept
{
    std::ignore = suppressPadding;
    ara::crypto::cryp::isoft_def::PSelectRsa::BoolResult resBool{
        ara::crypto::cryp::isoft_def::PSelectRsa::BoolResult::FromError(SecurityErrorDomain::Errc::kLogicFault)};
    PKeyPublic_Ipc_Rsa const *const pKeyPublicIpc{dynamic_cast< PKeyPublic_Ipc_Rsa const * >(GetPublicKey())};
    if (pKeyPublicIpc != nullptr) {
        // IPC
        PSelectRsa selectRsa{pKeyPublicIpc->GetSlotId()};
        resBool = selectRsa.PublicDecryptoPssIpc(pInputData, nDataLen, static_cast< uint32_t >(GetRequiredHashAlgId()),
                                                 static_cast< uint32_t >(nSaltLen_), pDigestData, nDigestDataLen);
    } else {
        // Local
        PKeyPublic_Rsa const *const pRsaKey{
            static_cast< PKeyPublic_Rsa const * >(static_cast< void const * >(GetPublicKey()))};
        PSelectRsa selectRsa{pRsaKey->GetRsa()};
        resBool
            = selectRsa.PublicDecryptoPssLocal(pInputData, nDataLen, static_cast< uint32_t >(GetRequiredHashAlgId()),
                                               static_cast< uint32_t >(nSaltLen_), pDigestData, nDigestDataLen);
    }

    if (!resBool.HasValue()) {
        return false;
    }
    return resBool.Value();
}
/// @brief Set salt length
/// @param slen Salt length
void PCtxDsv_VerifierPublic_Rsa_Pss::_SetSaltLen(int32_t const slen) noexcept { nSaltLen_ = slen; }

//********************************/
/// @brief Parameterized constructor
/// @param cryptoProvider Encryption provider
PCtxDsv_VerifierPublic_Rsa_Pss_Md5::PCtxDsv_VerifierPublic_Rsa_Pss_Md5(PCryptoProvider &cryptoProvider) noexcept
    : PCtxDsv_VerifierPublic_Rsa_Pss{cryptoProvider}
{
    _SetContextAglId(static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kHashMd5));
}
/// @brief Return a CryptoPrimitivId instance containing the instance identifier.
/// @returns  CryptoPrimitiveId::Uptr
CryptoPrimitiveId::Uptr PCtxDsv_VerifierPublic_Rsa_Pss_Md5::GetCryptoPrimitiveId() const noexcept
{
    return {std::make_unique< PAlgId_Dsv_VerifierPublicRsa_Pss_Md5 >()};
}
/// @brief Extended service member class.
/// @returns  SignatureService::Uptr
SignatureService::Uptr PCtxDsv_VerifierPublic_Rsa_Pss_Md5::GetSignatureService() const noexcept
{
    return {std::make_unique< PServiceSignature< PCtxDsv_VerifierPublic_Rsa_Pss_Md5 > >(*this)};
}
/// @brief Check if equal
/// @param nSigCryId Crypto primitive ID of the encryption algorithm used in the signature algorithm
/// @returns Crypto primitive ID
bool PCtxDsv_VerifierPublic_Rsa_Pss_Md5::IsCryptoAlgIdMacthing(CryptoPrimitiveId::AlgId const nSigCryId) const noexcept
{
    if (nSigCryId == static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kDsv_SignerPrivateRsa_PssMd5)) {
        return true;
    }
    return false;
}
//-----------------------------------------------------------------------------------------------------------------//
/// @brief Parameterized constructor
/// @param cryptoProvider Encryption provider
PCtxDsv_VerifierPublic_Rsa_Pss_Sha1::PCtxDsv_VerifierPublic_Rsa_Pss_Sha1(PCryptoProvider &cryptoProvider) noexcept
    : PCtxDsv_VerifierPublic_Rsa_Pss{cryptoProvider}
{
    _SetContextAglId(static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kHashSha1));
}
/// @brief Return a CryptoPrimitivId instance containing the instance identifier.
/// @returns  CryptoPrimitiveId instance
CryptoPrimitiveId::Uptr PCtxDsv_VerifierPublic_Rsa_Pss_Sha1::GetCryptoPrimitiveId() const noexcept
{
    return {std::make_unique< PAlgId_Dsv_VerifierPublicRsa_Pss_Sha1 >()};
}
/// @brief Extended service member class.
/// @returns  SignatureService instance
SignatureService::Uptr PCtxDsv_VerifierPublic_Rsa_Pss_Sha1::GetSignatureService() const noexcept
{
    return {std::make_unique< PServiceSignature< PCtxDsv_VerifierPublic_Rsa_Pss_Sha1 > >(*this)};
}
/// @brief Check if equal
/// @param nSigCryId Crypto primitive ID of the encryption algorithm used in the signature algorithm
/// @returns true if Macth sucess false otherwise
bool PCtxDsv_VerifierPublic_Rsa_Pss_Sha1::IsCryptoAlgIdMacthing(CryptoPrimitiveId::AlgId const nSigCryId) const noexcept
{
    if (nSigCryId == static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kDsv_SignerPrivateRsa_PssMd5)) {
        return true;
    }
    return false;
}
//-----------------------------------------------------------------------------------------------------------------//
/// @brief Parameterized constructor
/// @param cryptoProvider Encryption provider
PCtxDsv_VerifierPublic_Rsa_Pss_Sha2_224::PCtxDsv_VerifierPublic_Rsa_Pss_Sha2_224(
    PCryptoProvider &cryptoProvider) noexcept
    : PCtxDsv_VerifierPublic_Rsa_Pss{cryptoProvider}
{
    _SetContextAglId(static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kHashSha2_224));
}
/// @brief Return a CryptoPrimitivId instance containing the instance identifier.
/// @returns  CryptoPrimitiveIdd instance
CryptoPrimitiveId::Uptr PCtxDsv_VerifierPublic_Rsa_Pss_Sha2_224::GetCryptoPrimitiveId() const noexcept
{
    return {std::make_unique< PAlgId_Dsv_VerifierPublicRsa_Pss_Sha2_224 >()};
}
/// @brief Extended service member class.
/// @returns  SignatureServiced instance
SignatureService::Uptr PCtxDsv_VerifierPublic_Rsa_Pss_Sha2_224::GetSignatureService() const noexcept
{
    return {std::make_unique< PServiceSignature< PCtxDsv_VerifierPublic_Rsa_Pss_Sha2_224 > >(*this)};
}
/// @brief Check if equal
/// @param nSigCryId Crypto primitive ID of the encryption algorithm used in the signature algorithm
/// @returns true if Macth sucess false otherwise
bool PCtxDsv_VerifierPublic_Rsa_Pss_Sha2_224::IsCryptoAlgIdMacthing(
    CryptoPrimitiveId::AlgId const nSigCryId) const noexcept
{
    if (nSigCryId == static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kDsv_SignerPrivateRsa_PssSha2_224)) {
        return true;
    }
    return false;
}
//-----------------------------------------------------------------------------------------------------------------//
/// @brief Parameterized constructor
/// @param cryptoProvider Encryption provider
PCtxDsv_VerifierPublic_Rsa_Pss_Sha2_256::PCtxDsv_VerifierPublic_Rsa_Pss_Sha2_256(
    PCryptoProvider &cryptoProvider) noexcept
    : PCtxDsv_VerifierPublic_Rsa_Pss{cryptoProvider}
{
    _SetContextAglId(static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kHashSha2_256));
}
/// @brief Return a CryptoPrimitivId instance containing the instance identifier.
/// @returns  CryptoPrimitiveId instance
CryptoPrimitiveId::Uptr PCtxDsv_VerifierPublic_Rsa_Pss_Sha2_256::GetCryptoPrimitiveId() const noexcept
{
    return {std::make_unique< PAlgId_Dsv_VerifierPublicRsa_Pss_Sha2_256 >()};
}
/// @brief Extended service member class.
/// @returns  SignatureService instance
SignatureService::Uptr PCtxDsv_VerifierPublic_Rsa_Pss_Sha2_256::GetSignatureService() const noexcept
{
    return {std::make_unique< PServiceSignature< PCtxDsv_VerifierPublic_Rsa_Pss_Sha2_256 > >(*this)};
}
/// @brief Check if equal
/// @param nSigCryId Crypto primitive ID of the encryption algorithm used in the signature algorithm
/// @returns true if Macth sucess false otherwise
bool PCtxDsv_VerifierPublic_Rsa_Pss_Sha2_256::IsCryptoAlgIdMacthing(
    CryptoPrimitiveId::AlgId const nSigCryId) const noexcept
{
    if (nSigCryId == static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kDsv_SignerPrivateRsa_PssSha2_256)) {
        return true;
    }
    return false;
}
//-----------------------------------------------------------------------------------------------------------------//
/// @brief Parameterized constructor
/// @param cryptoProvider Encryption provider
PCtxDsv_VerifierPublic_Rsa_Pss_Sha2_384::PCtxDsv_VerifierPublic_Rsa_Pss_Sha2_384(
    PCryptoProvider &cryptoProvider) noexcept
    : PCtxDsv_VerifierPublic_Rsa_Pss{cryptoProvider}
{
    _SetContextAglId(static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kHashSha2_384));
}
/// @brief Return a CryptoPrimitivId instance containing the instance identifier.
/// @returns  CryptoPrimitiveId instance
CryptoPrimitiveId::Uptr PCtxDsv_VerifierPublic_Rsa_Pss_Sha2_384::GetCryptoPrimitiveId() const noexcept
{
    return {std::make_unique< PAlgId_Dsv_VerifierPublicRsa_Pss_Sha2_384 >()};
}
/// @brief Extended service member class.
/// @returns  SignatureService instance
SignatureService::Uptr PCtxDsv_VerifierPublic_Rsa_Pss_Sha2_384::GetSignatureService() const noexcept
{
    return {std::make_unique< PServiceSignature< PCtxDsv_VerifierPublic_Rsa_Pss_Sha2_384 > >(*this)};
}
/// @brief Check if equal
/// @param nSigCryId Crypto primitive ID of the encryption algorithm used in the signature algorithm
/// @returns true if Macth sucess false otherwise
bool PCtxDsv_VerifierPublic_Rsa_Pss_Sha2_384::IsCryptoAlgIdMacthing(
    CryptoPrimitiveId::AlgId const nSigCryId) const noexcept
{
    if (nSigCryId == static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kDsv_SignerPrivateRsa_PssSha2_384)) {
        return true;
    }
    return false;
}
//-----------------------------------------------------------------------------------------------------------------//
/// @brief Parameterized constructor
/// @param cryptoProvider Encryption provider
PCtxDsv_VerifierPublic_Rsa_Pss_Sha2_512::PCtxDsv_VerifierPublic_Rsa_Pss_Sha2_512(
    PCryptoProvider &cryptoProvider) noexcept
    : PCtxDsv_VerifierPublic_Rsa_Pss{cryptoProvider}
{
    _SetContextAglId(static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kHashSha2_512));
}
/// @brief Return a CryptoPrimitivId instance containing the instance identifier.
/// @returns  CryptoPrimitiveId instance
CryptoPrimitiveId::Uptr PCtxDsv_VerifierPublic_Rsa_Pss_Sha2_512::GetCryptoPrimitiveId() const noexcept
{
    return {std::make_unique< PAlgId_Dsv_VerifierPublicRsa_Pss_Sha2_512 >()};
}
/// @brief Extended service member class.
/// @returns  SignatureService instance
SignatureService::Uptr PCtxDsv_VerifierPublic_Rsa_Pss_Sha2_512::GetSignatureService() const noexcept
{
    return {std::make_unique< PServiceSignature< PCtxDsv_VerifierPublic_Rsa_Pss_Sha2_512 > >(*this)};
}
/// @brief Check if equal
/// @param nSigCryId Crypto primitive ID of the encryption algorithm used in the signature algorithm
/// @returns true if Macth sucess false otherwise
bool PCtxDsv_VerifierPublic_Rsa_Pss_Sha2_512::IsCryptoAlgIdMacthing(
    CryptoPrimitiveId::AlgId const nSigCryId) const noexcept
{
    if (nSigCryId == static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kDsv_SignerPrivateRsa_PssSha2_512)) {
        return true;
    }
    return false;
}
//********************************/

}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara
