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
/// @file       isoft_ctx_dsv_signer_private_rsa.cpp
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
/// @unit_name=PCtxDsv_SignerPrivate_Rsa
/// @unit_description=Signature RSA private key context interface
/// @endcode
///
/// ================================================================

#include "ara/crypto/cryp/dsv/isoft_ctx_dsv_signer_private_rsa.h"

#include "ara/crypto/cryp/cryobj/isoft_key_private_ipc.h"
#include "ara/crypto/cryp/cryobj/isoft_key_private_rsa.h"
#include "ara/crypto/cryp/isoft_crypto_provider.h"
#include "ara/crypto/cryp/isoft_service_signature.h"
#include "ara/crypto/cryp/select/isoft_select_rsa.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
/// //AUTOSAR-AP Interface
//*/ //PServiceCrypto Interface
/// @brief Gets the maximum supported key length (in bits).
/// @returns maximal supported length of the key in bits
std::size_t PCtxDsv_SignerPrivate_Rsa::GetMaxKeyBitLength() const noexcept
{
    return static_cast< std::size_t >(PAlgId_Asymmetric_RsaKey::EKeyLen::kRsaKeyMaxBitLength);
}
/// @brief Gets the minimum supported key length (in bits).
/// @returns  minimal supported length of the key in bits
std::size_t PCtxDsv_SignerPrivate_Rsa::GetMinKeyBitLength() const noexcept
{
    return static_cast< std::size_t >(PAlgId_Asymmetric_RsaKey::EKeyLen::kRsaKeyMinBitLength);
}
/// @brief Validates support for a specific key length based on the context.
/// @param keyBitLength Key length: in bits.
/// @returns  @c true if provided value of the key length is supported by the context
bool PCtxDsv_SignerPrivate_Rsa::IsKeyBitLengthSupported(std::size_t keyBitLength) const noexcept
{
    if ((keyBitLength < GetMinKeyBitLength()) || (keyBitLength > GetMaxKeyBitLength())) {
        return false;
    }
    return 0U == (keyBitLength % static_cast< std::size_t >(PAlgId_Asymmetric_RsaKey::EKeyLen::kRsaBlockStep));
}
/// @brief Gets the size of the signature value produced and required by the current algorithm.
/// @returns  std::size_t
std::size_t PCtxDsv_SignerPrivate_Rsa::GetSignatureSize() const noexcept
{
    PKeyPrivate_Rsa const *const pRsaKey{
        static_cast< PKeyPrivate_Rsa const * >(static_cast< void const * >(GetPrivateKey()))};
    if (pRsaKey != nullptr) {
        return pRsaKey->GetPayloadSize();
    }
    return 0U;
}
//***************/
/// @brief Here cryptoKey is the default value kAsymmetricRsaKey.
/// However, the value of the passed-in key is dynamically determined based on the length, which could be kAsymmetricRsaKey512 or kAsymmetricRsaKey1024, etc.
/// @param key Private key.
/// @returns true if check key sucess false otherwise
bool PCtxDsv_SignerPrivate_Rsa::CheckKey(PrivateKey const &key) const noexcept
{
    PAlgId_Asymmetric_RsaKey const cryptoKey;
    return cryptoKey.IsMinePrimitiveId(key.GetCryptoPrimitiveId()->GetPrimitiveId());
}
/// @brief Performs signature encryption logic.
ara::core::Result< ara::core::Vector< ara::core::Byte > >
/// @brief Performs signature encryption logic.
/// @param pInputData Starting address of the input data for the algorithm operation.
/// @param nDataLen Data length.
/// @param suppressPadding Whether to suppress padding.
/// @return Encrypted data.
PCtxDsv_SignerPrivate_Rsa::DoEncrypto(uint8_t const *pInputData, uint32_t nDataLen, bool suppressPadding) const noexcept
{
    PKeyPrivate_Ipc_Rsa const *const pKeyPrivateIpc{dynamic_cast< PKeyPrivate_Ipc_Rsa const * >(GetPrivateKey())};
    if (pKeyPrivateIpc != nullptr) {
        PSelectRsa selectRsa{pKeyPrivateIpc->GetSlotId()};
        int32_t const nPadding{suppressPadding ? RSA_NO_PADDING : RSA_PKCS1_PADDING};
        return selectRsa.PrivateEncrypto(pInputData, nDataLen, nPadding);
    }
    PKeyPrivate_Rsa const *const pRsaKey{
        static_cast< PKeyPrivate_Rsa const * >(static_cast< void const * >(GetPrivateKey()))};
    PSelectRsa selectRsa{pRsaKey->GetRsa()};
    int32_t const nPadding{suppressPadding ? RSA_NO_PADDING : RSA_PKCS1_PADDING};
    return selectRsa.PrivateEncrypto(pInputData, nDataLen, nPadding);
}
//********************************/
/// @brief Constructor with parameters.
/// @param cryptoProvider Crypto provider.
PCtxDsv_SignerPrivate_Rsa_Md5::PCtxDsv_SignerPrivate_Rsa_Md5(PCryptoProvider &cryptoProvider) noexcept
    : PCtxDsv_SignerPrivate_Rsa{cryptoProvider}
{
    SetHashAlgID(EPhCtxTypeID::kHashMd5);
}
//*/ //CryptoContext Interface
/// @brief Returns the CryptoPrimitiveId instance containing the instance identifier.
/// @returns CryptoPrimitiveId instance.
CryptoPrimitiveId::Uptr PCtxDsv_SignerPrivate_Rsa_Md5::GetCryptoPrimitiveId() const noexcept
{
    return {std::make_unique< PAlgId_Dsv_SignerPrivateRsa_Md5 >()};
}
/// @brief Extension service member class.
/// @returns SignatureService instance.
SignatureService::Uptr PCtxDsv_SignerPrivate_Rsa_Md5::GetSignatureService() const noexcept
{
    return {std::make_unique< PServiceSignature< PCtxDsv_SignerPrivate_Rsa_Md5 > >(*this)};
}
/// @brief Returns the crypto primitive ID.
/// @return Crypto primitive ID.
CryptoPrimitiveId::AlgId PCtxDsv_SignerPrivate_Rsa_Md5::GetCryptoAlgId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kDsv_SignerPrivateRsaMd5);
}
//-----------------------------------------------------------------------------------------------------------------//
/// @brief Constructor with parameters.
/// @param cryptoProvider Crypto provider.
PCtxDsv_SignerPrivate_Rsa_Sha1::PCtxDsv_SignerPrivate_Rsa_Sha1(PCryptoProvider &cryptoProvider) noexcept
    : PCtxDsv_SignerPrivate_Rsa{cryptoProvider}
{
    SetHashAlgID(EPhCtxTypeID::kHashSha1);
}
//*/ //CryptoContext Interface
/// @brief Returns the CryptoPrimitiveId instance containing the instance identifier.
/// @returns CryptoPrimitiveId instance.
CryptoPrimitiveId::Uptr PCtxDsv_SignerPrivate_Rsa_Sha1::GetCryptoPrimitiveId() const noexcept
{
    return {std::make_unique< PAlgId_Dsv_SignerPrivateRsa_Sha1 >()};
}
/// @brief Extension service member class.
/// @returns SignatureService instance.
SignatureService::Uptr PCtxDsv_SignerPrivate_Rsa_Sha1::GetSignatureService() const noexcept
{
    return {std::make_unique< PServiceSignature< PCtxDsv_SignerPrivate_Rsa_Sha1 > >(*this)};
}
/// @brief Returns the crypto primitive ID.
/// @return Crypto primitive ID.
CryptoPrimitiveId::AlgId PCtxDsv_SignerPrivate_Rsa_Sha1::GetCryptoAlgId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kDsv_SignerPrivateRsaSha1);
}
//-----------------------------------------------------------------------------------------------------------------//
/// @brief Constructor with parameters.
/// @param cryptoProvider Crypto provider.
PCtxDsv_SignerPrivate_Rsa_Sha2_224::PCtxDsv_SignerPrivate_Rsa_Sha2_224(PCryptoProvider &cryptoProvider) noexcept
    : PCtxDsv_SignerPrivate_Rsa{cryptoProvider}
{
    SetHashAlgID(EPhCtxTypeID::kHashSha2_224);
}
//*/ //CryptoContext Interface
/// @brief Returns the CryptoPrimitiveId instance containing the instance identifier.
/// @returns CryptoPrimitiveId instance.
CryptoPrimitiveId::Uptr PCtxDsv_SignerPrivate_Rsa_Sha2_224::GetCryptoPrimitiveId() const noexcept
{
    return {std::make_unique< PAlgId_Dsv_SignerPrivateRsa_Sha2_224 >()};
}
/// @brief Extension service member class.
/// @returns SignatureService instance.
SignatureService::Uptr PCtxDsv_SignerPrivate_Rsa_Sha2_224::GetSignatureService() const noexcept
{
    return {std::make_unique< PServiceSignature< PCtxDsv_SignerPrivate_Rsa_Sha2_224 > >(*this)};
}
/// @brief Returns the crypto primitive ID.
/// @return Crypto primitive ID.
CryptoPrimitiveId::AlgId PCtxDsv_SignerPrivate_Rsa_Sha2_224::GetCryptoAlgId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kDsv_SignerPrivateRsaSha2_224);
}
//-----------------------------------------------------------------------------------------------------------------//
/// @brief Constructor with parameters.
/// @param cryptoProvider Crypto provider.
PCtxDsv_SignerPrivate_Rsa_Sha2_256::PCtxDsv_SignerPrivate_Rsa_Sha2_256(PCryptoProvider &cryptoProvider) noexcept
    : PCtxDsv_SignerPrivate_Rsa{cryptoProvider}
{
    SetHashAlgID(EPhCtxTypeID::kHashSha2_256);
}
//*********/ //CryptoContext Interface
/// @brief Returns the CryptoPrimitiveId instance containing the instance identifier.
/// @returns CryptoPrimitiveId instance.
CryptoPrimitiveId::Uptr PCtxDsv_SignerPrivate_Rsa_Sha2_256::GetCryptoPrimitiveId() const noexcept
{
    return {std::make_unique< PAlgId_Dsv_SignerPrivateRsa_Sha2_256 >()};
}
/// @brief Extension service member class.
/// @returns SignatureService instance.
SignatureService::Uptr PCtxDsv_SignerPrivate_Rsa_Sha2_256::GetSignatureService() const noexcept
{
    return {std::make_unique< PServiceSignature< PCtxDsv_SignerPrivate_Rsa_Sha2_256 > >(*this)};
}
/// @brief Return crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PCtxDsv_SignerPrivate_Rsa_Sha2_256::GetCryptoAlgId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kDsv_SignerPrivateRsaSha2_256);
}
//-----------------------------------------------------------------------------------------------------------------//
/// @brief Parameterized constructor
/// @param cryptoProvider Encryption provider
PCtxDsv_SignerPrivate_Rsa_Sha2_384::PCtxDsv_SignerPrivate_Rsa_Sha2_384(PCryptoProvider &cryptoProvider) noexcept
    : PCtxDsv_SignerPrivate_Rsa{cryptoProvider}
{
    SetHashAlgID(EPhCtxTypeID::kHashSha2_384);
}
//***************/  //CryptoContext interface
/// @brief Return a CryptoPrimitivId instance containing the instance identifier.
/// @returns  CryptoPrimitiveId instance
CryptoPrimitiveId::Uptr PCtxDsv_SignerPrivate_Rsa_Sha2_384::GetCryptoPrimitiveId() const noexcept
{
    return {std::make_unique< PAlgId_Dsv_SignerPrivateRsa_Sha2_384 >()};
}
/// @brief Extended service member class.
/// @returns  SignatureService instance
SignatureService::Uptr PCtxDsv_SignerPrivate_Rsa_Sha2_384::GetSignatureService() const noexcept
{
    return {std::make_unique< PServiceSignature< PCtxDsv_SignerPrivate_Rsa_Sha2_384 > >(*this)};
}
/// @brief Return crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PCtxDsv_SignerPrivate_Rsa_Sha2_384::GetCryptoAlgId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kDsv_SignerPrivateRsaSha2_384);
}
//-----------------------------------------------------------------------------------------------------------------//
/// @brief Parameterized constructor
/// @param cryptoProvider Encryption provider
PCtxDsv_SignerPrivate_Rsa_Sha2_512::PCtxDsv_SignerPrivate_Rsa_Sha2_512(PCryptoProvider &cryptoProvider) noexcept
    : PCtxDsv_SignerPrivate_Rsa{cryptoProvider}
{
    SetHashAlgID(EPhCtxTypeID::kHashSha2_512);
}
//***************/  //CryptoContext interface
/// @brief Return a CryptoPrimitivId instance containing the instance identifier.
/// @returns  CryptoPrimitiveId instance
CryptoPrimitiveId::Uptr PCtxDsv_SignerPrivate_Rsa_Sha2_512::GetCryptoPrimitiveId() const noexcept
{
    return {std::make_unique< PAlgId_Dsv_SignerPrivateRsa_Sha2_512 >()};
}
/// @brief Extended service member class.
/// @returns  SignatureService instance
SignatureService::Uptr PCtxDsv_SignerPrivate_Rsa_Sha2_512::GetSignatureService() const noexcept
{
    return {std::make_unique< PServiceSignature< PCtxDsv_SignerPrivate_Rsa_Sha2_512 > >(*this)};
}
/// @brief Return crypto primitive ID
/// @return Crypto primitive ID
CryptoPrimitiveId::AlgId PCtxDsv_SignerPrivate_Rsa_Sha2_512::GetCryptoAlgId() const noexcept
{
    return static_cast< CryptoPrimitiveId::AlgId >(EPhCtxTypeID::kDsv_SignerPrivateRsaSha2_512);
}
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara
