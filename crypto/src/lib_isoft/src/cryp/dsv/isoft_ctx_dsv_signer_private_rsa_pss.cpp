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
/// @file       isoft_ctx_dsv_signer_private_rsa_pss.cpp
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
/// @unit_name=PCtxDsv_SignerPrivate_Rsa_Pss
/// @unit_description=Signature RSA-PSS Private Key Context Interface
/// @endcode
///
/// ================================================================

#include "ara/crypto/cryp/dsv/isoft_ctx_dsv_signer_private_rsa_pss.h"

#include "ara/crypto/common/security_error_domain.h"
#include "ara/crypto/cryp/cryobj/isoft_key_private_ipc.h"
#include "ara/crypto/cryp/cryobj/isoft_key_private_rsa.h"
#include "ara/crypto/cryp/hash/isoft_ctx_hash_function.h"
#include "ara/crypto/cryp/isoft_service_signature.h"
#include "ara/crypto/cryp/select/isoft_select_rsa.h"
#include "openssl/evp.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
//********************************/ //AUTOSAR-AP interface
//***************/
/// @brief Get the maximum supported key length in bits.
/// @returns  maximal supported key length in bits.
std::size_t PCtxDsv_SignerPrivate_Rsa_Pss::GetMaxKeyBitLength() const noexcept
{
    return static_cast< std::size_t >(PAlgId_Asymmetric_RsaKey::EKeyLen::kRsaKeyMaxBitLength);
}
/// @brief Get the minimum supported key length in bits.
/// @returns  minimal supported key length in bits.
std::size_t PCtxDsv_SignerPrivate_Rsa_Pss::GetMinKeyBitLength() const noexcept
{
    return static_cast< std::size_t >(PAlgId_Asymmetric_RsaKey::EKeyLen::kRsaKeyMinBitLength);
}
/// @brief Verify support for a specific key length based on the context.
/// @param keyBitLength Key length: in bits
/// @returns  @c true if provided value of the key length is supported by the context
bool PCtxDsv_SignerPrivate_Rsa_Pss::IsKeyBitLengthSupported(std::size_t keyBitLength) const noexcept
{
    if ((keyBitLength < GetMinKeyBitLength()) || (keyBitLength > GetMaxKeyBitLength())) {
        return false;
    }
    return 0U == (keyBitLength % static_cast< std::size_t >(PAlgId_Asymmetric_RsaKey::EKeyLen::kRsaBlockStep));
}
/// @brief Get the size of the signature value produced and required by the current algorithm.
/// @returns  Size of the signature value produced and required by the algorithm
std::size_t PCtxDsv_SignerPrivate_Rsa_Pss::GetSignatureSize() const noexcept
{
    PKeyPrivate_Rsa const *const pRsaKey{
        static_cast< PKeyPrivate_Rsa const * >(static_cast< void const * >(GetPrivateKey()))};
    if (pRsaKey != nullptr) {
        return pRsaKey->GetPayloadSize();
    }
    return 0U;
}
//***************/
/// @brief Here cryptoKey is the default value kAsymmetricRsaKey
///         However, the passed-in key value is dynamic and determined by different lengths; it could be kAsymmetricRsaKey512, kAsymmetricRsaKey1024, etc.
/// @param key Private key
/// @returns true if check key sucess false otherwise
bool PCtxDsv_SignerPrivate_Rsa_Pss::CheckKey(PrivateKey const &key) const noexcept
{
    PAlgId_Asymmetric_RsaKey const cryptoKey;
    return cryptoKey.IsMinePrimitiveId(key.GetCryptoPrimitiveId()->GetPrimitiveId());
}
/// @brief Execute signature encryption logic
ara::core::Result< ara::core::Vector< ara::core::Byte > >
/// @brief Execute signature encryption logic
/// @param pInputData Start address of input data in algorithm operation
/// @param nDataLen Data length
/// @param suppressPadding Whether padding is suppressed
/// @return Encrypted data
PCtxDsv_SignerPrivate_Rsa_Pss::DoEncrypto(uint8_t const *pInputData,
                                          uint32_t nDataLen,
                                          bool suppressPadding) const noexcept
{
    std::ignore = suppressPadding;
    PKeyPrivate_Ipc_Rsa const *const pKeyPrivateIpc{dynamic_cast< PKeyPrivate_Ipc_Rsa const * >(GetPrivateKey())};
    if (pKeyPrivateIpc != nullptr) {
        // IPC
        PSelectRsa selectRsa{pKeyPrivateIpc->GetSlotId()};
        return selectRsa.PrivateEncryptoPssIpc(pInputData, nDataLen, static_cast< uint32_t >(GetRequiredHashAlgId()),
                                               static_cast< uint32_t >(nSaltLen_));
    }
    // Local
    PKeyPrivate_Rsa const *const pRsaKey{
        static_cast< PKeyPrivate_Rsa const * >(static_cast< void const * >(GetPrivateKey()))};
    // First apply PSS to the digest
    PSelectRsa selectRsa{pRsaKey->GetRsa()};
    return selectRsa.PrivateEncryptoPssLocal(pInputData, nDataLen, static_cast< uint32_t >(GetRequiredHashAlgId()),
                                             static_cast< uint32_t >(nSaltLen_));
}
/// @brief Set salt length
/// @param slen Salt length
void PCtxDsv_SignerPrivate_Rsa_Pss::_SetSaltLen(int32_t const slen) noexcept { nSaltLen_ = slen; }

//********************************/
/// @brief Parameterized constructor
/// @param cryptoProvider Encryption provider
PCtxDsv_SignerPrivate_Rsa_Pss_Md5::PCtxDsv_SignerPrivate_Rsa_Pss_Md5(PCryptoProvider &cryptoProvider) noexcept
    : PCtxDsv_SignerPrivate_Rsa_Pss{cryptoProvider}
{
    SetHashAlgID(EPhCtxTypeID::kHashMd5);
}
/// @brief Return a CryptoPrimitivId instance containing the instance identifier.
/// @returns  CryptoPrimitivId instance
CryptoPrimitiveId::Uptr PCtxDsv_SignerPrivate_Rsa_Pss_Md5::GetCryptoPrimitiveId() const noexcept
{
    return {std::make_unique< PAlgId_Dsv_SignerPrivateRsa_Pss_Md5 >()};
}
/// @brief Extended service member class.
/// @returns  SignatureService instance
SignatureService::Uptr PCtxDsv_SignerPrivate_Rsa_Pss_Md5::GetSignatureService() const noexcept
{
    return {std::make_unique< PServiceSignature< PCtxDsv_SignerPrivate_Rsa_Pss_Md5 > >(*this)};
}
/// @brief Get crypto primitive ID
/// @return  Crypto primitive ID
CryptoPrimitiveId::AlgId PCtxDsv_SignerPrivate_Rsa_Pss_Md5::GetCryptoAlgId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kDsv_SignerPrivateRsa_PssMd5);
}
//-----------------------------------------------------------------------------------------------------------------//
/// @brief Parameterized constructor
/// @param cryptoProvider Encryption provider
PCtxDsv_SignerPrivate_Rsa_Pss_Sha1::PCtxDsv_SignerPrivate_Rsa_Pss_Sha1(PCryptoProvider &cryptoProvider) noexcept
    : PCtxDsv_SignerPrivate_Rsa_Pss{cryptoProvider}
{
    SetHashAlgID(EPhCtxTypeID::kHashSha1);
}
/// @brief Return a CryptoPrimitivId instance containing the instance identifier.
/// @returns CryptoPrimitivId instance
CryptoPrimitiveId::Uptr PCtxDsv_SignerPrivate_Rsa_Pss_Sha1::GetCryptoPrimitiveId() const noexcept
{
    return {std::make_unique< PAlgId_Dsv_SignerPrivateRsa_Pss_Sha1 >()};
}
/// @brief Extended service member class.
/// @returns  SignatureService instance
SignatureService::Uptr PCtxDsv_SignerPrivate_Rsa_Pss_Sha1::GetSignatureService() const noexcept
{
    return {std::make_unique< PServiceSignature< PCtxDsv_SignerPrivate_Rsa_Pss_Sha1 > >(*this)};
}
/// @brief Get crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PCtxDsv_SignerPrivate_Rsa_Pss_Sha1::GetCryptoAlgId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kDsv_SignerPrivateRsa_PssSha1);
}
//-----------------------------------------------------------------------------------------------------------------//
/// @brief Parameterized constructor
/// @param cryptoProvider Encryption provider
PCtxDsv_SignerPrivate_Rsa_Pss_Sha2_224::PCtxDsv_SignerPrivate_Rsa_Pss_Sha2_224(PCryptoProvider &cryptoProvider) noexcept
    : PCtxDsv_SignerPrivate_Rsa_Pss{cryptoProvider}
{
    SetHashAlgID(EPhCtxTypeID::kHashSha2_224);
}
/// @brief Return a CryptoPrimitivId instance containing the instance identifier.
/// @returns  CryptoPrimitivId instance
CryptoPrimitiveId::Uptr PCtxDsv_SignerPrivate_Rsa_Pss_Sha2_224::GetCryptoPrimitiveId() const noexcept
{
    return {std::make_unique< PAlgId_Dsv_SignerPrivateRsa_Pss_Sha2_224 >()};
}
/// @brief Extended service member class.
/// @returns  SignatureService instance
SignatureService::Uptr PCtxDsv_SignerPrivate_Rsa_Pss_Sha2_224::GetSignatureService() const noexcept
{
    return {std::make_unique< PServiceSignature< PCtxDsv_SignerPrivate_Rsa_Pss_Sha2_224 > >(*this)};
}
/// @brief Get crypto primitive ID
/// @return  Crypto primitive ID
CryptoPrimitiveId::AlgId PCtxDsv_SignerPrivate_Rsa_Pss_Sha2_224::GetCryptoAlgId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kDsv_SignerPrivateRsa_PssSha2_224);
}
//-----------------------------------------------------------------------------------------------------------------//
/// @brief Parameterized constructor
/// @param cryptoProvider Encryption provider
PCtxDsv_SignerPrivate_Rsa_Pss_Sha2_256::PCtxDsv_SignerPrivate_Rsa_Pss_Sha2_256(PCryptoProvider &cryptoProvider) noexcept
    : PCtxDsv_SignerPrivate_Rsa_Pss{cryptoProvider}
{
    SetHashAlgID(EPhCtxTypeID::kHashSha2_256);
}
/// @brief Return a CryptoPrimitivId instance containing the instance identifier.
/// @returns  CryptoPrimitivId instance.
CryptoPrimitiveId::Uptr PCtxDsv_SignerPrivate_Rsa_Pss_Sha2_256::GetCryptoPrimitiveId() const noexcept
{
    return {std::make_unique< PAlgId_Dsv_SignerPrivateRsa_Pss_Sha2_256 >()};
}
/// @brief Extended service member class.
/// @returns  SignatureService instance.
SignatureService::Uptr PCtxDsv_SignerPrivate_Rsa_Pss_Sha2_256::GetSignatureService() const noexcept
{
    return {std::make_unique< PServiceSignature< PCtxDsv_SignerPrivate_Rsa_Pss_Sha2_256 > >(*this)};
}
/// @brief Get crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PCtxDsv_SignerPrivate_Rsa_Pss_Sha2_256::GetCryptoAlgId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kDsv_SignerPrivateRsa_PssSha2_256);
}
//-----------------------------------------------------------------------------------------------------------------//
/// @brief Parameterized constructor
/// @param cryptoProvider Encryption provider
PCtxDsv_SignerPrivate_Rsa_Pss_Sha2_384::PCtxDsv_SignerPrivate_Rsa_Pss_Sha2_384(PCryptoProvider &cryptoProvider) noexcept
    : PCtxDsv_SignerPrivate_Rsa_Pss{cryptoProvider}
{
    SetHashAlgID(EPhCtxTypeID::kHashSha2_384);
}
/// @brief Return a CryptoPrimitivId instance containing the instance identifier.
/// @returns  CryptoPrimitivId instance
CryptoPrimitiveId::Uptr PCtxDsv_SignerPrivate_Rsa_Pss_Sha2_384::GetCryptoPrimitiveId() const noexcept
{
    return {std::make_unique< PAlgId_Dsv_SignerPrivateRsa_Pss_Sha2_384 >()};
}
/// @brief Extended service member class.
/// @returns  SignatureService instance
SignatureService::Uptr PCtxDsv_SignerPrivate_Rsa_Pss_Sha2_384::GetSignatureService() const noexcept
{
    return {std::make_unique< PServiceSignature< PCtxDsv_SignerPrivate_Rsa_Pss_Sha2_384 > >(*this)};
}
/// @brief Get crypto primitive ID
/// @return  Crypto primitive ID
CryptoPrimitiveId::AlgId PCtxDsv_SignerPrivate_Rsa_Pss_Sha2_384::GetCryptoAlgId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kDsv_SignerPrivateRsa_PssSha2_384);
}
//-----------------------------------------------------------------------------------------------------------------//
/// @brief Parameterized constructor
/// @param cryptoProvider Encryption provider
PCtxDsv_SignerPrivate_Rsa_Pss_Sha2_512::PCtxDsv_SignerPrivate_Rsa_Pss_Sha2_512(PCryptoProvider &cryptoProvider) noexcept
    : PCtxDsv_SignerPrivate_Rsa_Pss{cryptoProvider}
{
    SetHashAlgID(EPhCtxTypeID::kHashSha2_512);
}
/// @brief Return a CryptoPrimitivId instance containing the instance identifier.
/// @returns  CryptoPrimitivId instance.
CryptoPrimitiveId::Uptr PCtxDsv_SignerPrivate_Rsa_Pss_Sha2_512::GetCryptoPrimitiveId() const noexcept
{
    return {std::make_unique< PAlgId_Dsv_SignerPrivateRsa_Pss_Sha2_512 >()};
}
/// @brief Extended service member class.
/// @returns  SignatureService instance.
SignatureService::Uptr PCtxDsv_SignerPrivate_Rsa_Pss_Sha2_512::GetSignatureService() const noexcept
{
    return {std::make_unique< PServiceSignature< PCtxDsv_SignerPrivate_Rsa_Pss_Sha2_512 > >(*this)};
}
/// @brief Get crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PCtxDsv_SignerPrivate_Rsa_Pss_Sha2_512::GetCryptoAlgId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kDsv_SignerPrivateRsa_PssSha2_512);
}
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara
