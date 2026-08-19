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
/// @file       isoft_ctx_decryptor_private_rsa.cpp
/// @brief      AutoSar-Crypto encryption/decryption module
/// @details
/// @date       2022-03-02
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/DefaultEncryptionDecryption/SymmetricEncryption
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_01008
/// @unit_name=PCtxDecryptorPrivate_Rsa
/// @unit_description=Asymmetric decryption private key context interface
/// @endcode
///
/// ================================================================

#include "ara/crypto/cryp/asymmetric/isoft_ctx_decryptor_private_rsa.h"

#include "ara/crypto/cryp/cryobj/isoft_crypto_primitive_id.h"
#include "ara/crypto/cryp/isoft_service_crypto.h"
#include "ara/crypto/cryp/select/isoft_select_rsa.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
//********************************/ //AUTOSAR-AP interface
/// @brief Returns a CryptoPrimitivId instance containing the instance identifier.
/// @returns  CryptoPrimitivId instance
CryptoPrimitiveId::Uptr PCtxDecryptorPrivate_Rsa::GetCryptoPrimitiveId() const noexcept
{
    return {std::make_unique< PAlgId_Asymmetric_Rsa >()};
}
/// @brief Get the CryptoService instance.
/// @returns  CryptoService instance
CryptoService::Uptr PCtxDecryptorPrivate_Rsa::GetCryptoService() const noexcept
{
    return {std::make_unique< PServiceCrypto< PCtxDecryptorPrivate_Rsa > >(*this)};
}
//***************/  //PServiceCrypto interface
/// @brief Get the maximum supported key length (in bits).
/// @returns  maximum key length
std::size_t PCtxDecryptorPrivate_Rsa::GetMaxKeyBitLength() const noexcept
{
    return static_cast< std::size_t >(PAlgId_Asymmetric_RsaKey::EKeyLen::kRsaKeyMaxBitLength);
}
/// @brief Get the minimum supported key length (in bits).
/// @returns  minimum key length
std::size_t PCtxDecryptorPrivate_Rsa::GetMinKeyBitLength() const noexcept
{
    return static_cast< std::size_t >(PAlgId_Asymmetric_RsaKey::EKeyLen::kRsaKeyMinBitLength);
}
/// @brief Verify support for a specific key length according to the context.
/// @param keyBitLength key length: in bits
/// @returns true support length false otherwise
bool PCtxDecryptorPrivate_Rsa::IsKeyBitLengthSupported(std::size_t keyBitLength) const noexcept
{
    if ((keyBitLength < GetMinKeyBitLength()) || (keyBitLength > GetMaxKeyBitLength())) {
        return false;
    }
    return 0U == (keyBitLength % static_cast< std::size_t >(PAlgId_Asymmetric_RsaKey::EKeyLen::kRsaBlockStep));
}
/// @brief Get the block (or internal buffer) size of the underlying algorithm.
/// For digests, byte stream ciphers, and RNG contexts, it is an informational method used only to optimize interface usage.
/// @returns  algorithm block size
std::size_t PCtxDecryptorPrivate_Rsa::GetBlockSize() const noexcept
{
    PKeyPrivate_Ipc_Rsa const *const pKeyPrivateIpc{dynamic_cast< PKeyPrivate_Ipc_Rsa const * >(GetPrivateKey())};
    if (pKeyPrivateIpc != nullptr) {
        return pKeyPrivateIpc->GetPayloadSize();
    }
    PKeyPrivate_Rsa const *const pRsaKey{
        static_cast< PKeyPrivate_Rsa const * >(static_cast< void const * >(GetPrivateKey()))};
    if (pRsaKey != nullptr) {
        return pRsaKey->GetPayloadSize();
    }
    return 0U;
}
//***************/
/// @brief Check whether the key meets requirements
/// @param key private key
/// @returns true if check key sucess false otherwise
bool PCtxDecryptorPrivate_Rsa::CheckKey(PrivateKey const &key) const noexcept
{
    PAlgId_Asymmetric_RsaKey const cryptoKey;
    return cryptoKey.IsMinePrimitiveId(key.GetCryptoPrimitiveId()->GetPrimitiveId());
}
/// @brief Calculate encryption/decryption
ara::core::Result< ara::core::Vector< ara::core::Byte > >
/// @param pInputData start address of input data in algorithm operation
/// @param nDataLen data length
/// @param suppressPadding whether to suppress padding
/// @return encrypted data
PCtxDecryptorPrivate_Rsa::ExecuteDecryptoLogic(uint8_t const *pInputData,
                                               uint32_t nDataLen,
                                               bool suppressPadding) const noexcept
{
    PKeyPrivate_Ipc_Rsa const *const pKeyPrivateIpc{dynamic_cast< PKeyPrivate_Ipc_Rsa const * >(GetPrivateKey())};
    if (pKeyPrivateIpc != nullptr) {
        PSelectRsa selectRsa{pKeyPrivateIpc->GetSlotId()};
        int32_t const nPadding{suppressPadding ? RSA_NO_PADDING : RSA_PKCS1_PADDING};
        return selectRsa.PrivateDecrypto(pInputData, nDataLen, nPadding);
    }
    PKeyPrivate_Rsa const *const pRsaKey{
        static_cast< PKeyPrivate_Rsa const * >(static_cast< void const * >(GetPrivateKey()))};
    PSelectRsa selectRsa{pRsaKey->GetRsa()};
    int32_t const nPadding{suppressPadding ? RSA_NO_PADDING : RSA_PKCS1_PADDING};
    return selectRsa.PrivateDecrypto(pInputData, nDataLen, nPadding);
}
//********************************/
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara
