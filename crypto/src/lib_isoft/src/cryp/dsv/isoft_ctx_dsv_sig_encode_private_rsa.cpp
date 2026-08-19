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
/// @file       isoft_ctx_dsv_sig_encode_private_rsa.cpp
/// @brief      AutoSar-Crypto Encryption/Decryption Module
/// @details
/// @date       2022-03-24
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default Encryption/Signature Storage
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_01009
/// @unit_name=PCtxDsv_SigEncodePrivate_Rsa
/// @unit_description=Private key context for asymmetric signature computation and short message encoding (like RSA)
/// @endcode
///
/// ================================================================

#include "ara/crypto/cryp/dsv/isoft_ctx_dsv_sig_encode_private_rsa.h"

#include "ara/crypto/cryp/cryobj/isoft_key_private_ipc.h"
#include "ara/crypto/cryp/cryobj/isoft_key_private_rsa.h"
#include "ara/crypto/cryp/isoft_crypto_provider.h"
#include "ara/crypto/cryp/isoft_service_extension.h"
#include "ara/crypto/cryp/select/isoft_select_rsa.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
//********************************/
/// @brief Private key context for asymmetric signature computation and short message encoding (like RSA).
/// A restricted group of trusted subscribers can use this primitive to simultaneously provide confidentiality, authenticity, and non-repudiation of short messages, provided that the public keys are appropriately generated and kept confidential.
//****/ //CryptoContext Interface
/// @brief Returns the CryptoPrimitiveId instance containing the instance identifier.
/// @returns CryptoPrimitiveId instance.
CryptoPrimitiveId::Uptr PCtxDsv_SigEncodePrivate_Rsa::GetCryptoPrimitiveId() const noexcept
{
    return {std::make_unique< PAlgId_Dsv_SigEncodePrivateRsa >()};
}
//*/ //SigEncodePrivateCtx Interface
/// @brief Extension service member class.
/// @returns ExtensionService instance.
ExtensionService::Uptr PCtxDsv_SigEncodePrivate_Rsa::GetExtensionService() const noexcept
{
    return {std::make_unique< PServiceExtension< PCtxDsv_SigEncodePrivate_Rsa > >(*this)};
}
//*/ //ExtensionService Interface
/// @brief Gets the maximum supported key length (in bits).
/// @returns  maximal supported length of the key in bits
std::size_t PCtxDsv_SigEncodePrivate_Rsa::GetMaxKeyBitLength() const noexcept
{
    return static_cast< std::size_t >(PAlgId_Asymmetric_RsaKey::EKeyLen::kRsaKeyMaxBitLength);
}
/// @brief Gets the minimum supported key length (in bits).
/// @returns minimal supported length of the key in bits
std::size_t PCtxDsv_SigEncodePrivate_Rsa::GetMinKeyBitLength() const noexcept
{
    return static_cast< std::size_t >(PAlgId_Asymmetric_RsaKey::EKeyLen::kRsaKeyMinBitLength);
}
/// @brief Validates support for a specific key length based on the context.
/// @param keyBitLength Key length: in bits.
/// @returns  @c true if provided value of the key length is supported by the context
bool PCtxDsv_SigEncodePrivate_Rsa::IsKeyBitLengthSupported(std::size_t keyBitLength) const noexcept
{
    if ((keyBitLength < GetMinKeyBitLength()) || (keyBitLength > GetMaxKeyBitLength())) {
        return false;
    }
    return 0U == (keyBitLength % static_cast< std::size_t >(PAlgId_Asymmetric_RsaKey::EKeyLen::kRsaBlockStep));
}
//***************/
/// @brief Gets the length of the encrypted block.
/// @returns Length of the encrypted block.
uint32_t PCtxDsv_SigEncodePrivate_Rsa::GetBlockSize() const noexcept
{
    PKeyPrivate_Rsa const *const pRsaKey{
        static_cast< PKeyPrivate_Rsa const * >(static_cast< void const * >(GetPrivateKey()))};
    if (pRsaKey != nullptr) {
        return static_cast< uint32_t >(pRsaKey->GetPayloadSize());
    }
    return 0U;
}
/// @brief Checks whether the key meets the requirements.
/// @param key Private key.
/// @returns  true if check key sucess false otherwise
bool PCtxDsv_SigEncodePrivate_Rsa::CheckKey(PrivateKey const &key) const noexcept
{
    PAlgId_Asymmetric_RsaKey const cryptoKey{};
    return cryptoKey.IsMinePrimitiveId(key.GetCryptoPrimitiveId()->GetPrimitiveId());
}
/// @brief Performs signature encryption logic.
/// @returns  auto
/// @param pInputData Starting address of the input data for the algorithm operation.
/// @param nDataLen Data length.
/// @param suppressPadding Whether to suppress padding.
/// @return Encrypted data.
ara::core::Result< ara::core::Vector< ara::core::Byte > > PCtxDsv_SigEncodePrivate_Rsa::DoEncrypto(
    uint8_t const *pInputData, uint32_t nDataLen, bool suppressPadding) const noexcept
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
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara
