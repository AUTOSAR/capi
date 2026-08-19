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
/// @file       isoft_ctx_encryptor_public_rsa.cpp
/// @brief      AutoSar-Crypto Encryption/Decryption Module
/// @details
/// @date       2022-03-02
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default Encryption/Asymmetric Encryption
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_01008
/// @unit_name=PCtxEncryptorPublic_Rsa
/// @unit_description=Asymmetric Encryption Public Key Context Interface
/// @endcode
///
/// ================================================================

#include "ara/crypto/cryp/asymmetric/isoft_ctx_encryptor_public_rsa.h"

#include "ara/crypto/common/isoft_assert.h"
#include "ara/crypto/common/isoft_data_type.h"
#include "ara/crypto/cryp/cryobj/isoft_crypto_primitive_id.h"
#include "ara/crypto/cryp/cryobj/isoft_key_public.h"
#include "ara/crypto/cryp/isoft_service_crypto.h"
#include "ara/crypto/cryp/select/isoft_select_rsa.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
//********************************/
/// @brief Asymmetric encryption public key context interface: RSA.
//********************************/     //AUTOSAR-AP interface
/// @brief Return a CryptoPrimitivId instance containing the instance identifier.
/// @name  GetCryptoPrimitiveId
/// @returns  CryptoPrimitivId instance
CryptoPrimitiveId::Uptr PCtxEncryptorPublic_Rsa::GetCryptoPrimitiveId() const noexcept
{
    return {std::make_unique< PAlgId_Asymmetric_Rsa >()};
}
/// @brief Get CryptoService instance.
/// @name  GetCryptoService
/// @returns  CryptoService instance
CryptoService::Uptr PCtxEncryptorPublic_Rsa::GetCryptoService() const noexcept
{
    return {std::make_unique< PServiceCrypto< PCtxEncryptorPublic_Rsa > >(*this)};
}
//***************/  //PServiceCrypto interface
/// @brief Get the maximum supported key length in bits.
/// @name  GetMaxKeyBitLength
/// @returns  Maximum key length
std::size_t PCtxEncryptorPublic_Rsa::GetMaxKeyBitLength() const noexcept
{
    return static_cast< std::size_t >(PAlgId_Asymmetric_RsaKey::EKeyLen::kRsaKeyMaxBitLength);
}
/// @brief Get the minimum supported key length in bits.
/// @name  GetMinKeyBitLength
/// @returns Minimum key length
std::size_t PCtxEncryptorPublic_Rsa::GetMinKeyBitLength() const noexcept
{
    return static_cast< std::size_t >(PAlgId_Asymmetric_RsaKey::EKeyLen::kRsaKeyMinBitLength);
}
/// @brief Verify support for a specific key length based on the context.
/// @name  IsKeyBitLengthSupported
/// @param keyBitLength Key length: in bits
/// @returns  true if support false otherwise
bool PCtxEncryptorPublic_Rsa::IsKeyBitLengthSupported(std::size_t keyBitLength) const noexcept
{
    if ((keyBitLength < GetMinKeyBitLength()) || (keyBitLength > GetMaxKeyBitLength())) {
        return false;
    }
    return 0U == (keyBitLength % static_cast< std::size_t >(PAlgId_Asymmetric_RsaKey::EKeyLen::kRsaBlockStep));
}
/// @brief Get the block (or internal buffer) size of the underlying algorithm.
///         For digest, byte-stream cipher, and RNG contexts, this is an informational method used only for interface optimization.
/// @name  GetBlockSize
/// @returns  Block size
std::size_t PCtxEncryptorPublic_Rsa::GetBlockSize() const noexcept
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
/// @name  CheckKey
/// @param key Public key
/// @returns true if check sucess false otherwise
bool PCtxEncryptorPublic_Rsa::CheckKey(PublicKey const &key) const noexcept
{
    PAlgId_Asymmetric_RsaKey const cryptoKey;

    CryptoPrimitiveId::Uptr const rsaPrimitiveId{key.GetCryptoPrimitiveId()};
    CryptoPrimitiveId::AlgId const algId{rsaPrimitiveId->GetPrimitiveId()};
    if (false == cryptoKey.IsMinePrimitiveId(algId)) {
        return false;
    }
    return key.CheckKey();
}
/// @brief Perform encryption/decryption calculation
/// @name  ExecuteEncryptoLogic
/// @param pInputData Start address of input data in algorithm operation
/// @param nDataLen Data length
/// @param suppressPadding Whether padding is suppressed
/// @return Encrypted data
ara::core::Result< ara::core::Vector< ara::core::Byte > > PCtxEncryptorPublic_Rsa::ExecuteEncryptoLogic(
    uint8_t const *pInputData, uint32_t nDataLen, bool suppressPadding) const noexcept
{
    PKeyPublic_Ipc_Rsa const *const pKeyPublicIpc{dynamic_cast< PKeyPublic_Ipc_Rsa const * >(GetPublicKey())};
    if (pKeyPublicIpc != nullptr) {
        PSelectRsa selectRsa{pKeyPublicIpc->GetSlotId()};
        int32_t const nPadding{suppressPadding ? RSA_NO_PADDING : RSA_PKCS1_PADDING};
        return selectRsa.PublicEncrypto(pInputData, nDataLen, nPadding);
    }
    PKeyPublic_Rsa const *const pRsaKey{
        static_cast< PKeyPublic_Rsa const * >(static_cast< void const * >(GetPublicKey()))};
    PSelectRsa selectRsa{pRsaKey->GetRsa()};
    int32_t const nPadding{suppressPadding ? RSA_NO_PADDING : RSA_PKCS1_PADDING};
    return selectRsa.PublicEncrypto(pInputData, nDataLen, nPadding);
}
/// @brief Get key modulus length: different Padding schemes correspond to different values in RSA
/// @name  GetModulusSize
/// @returns  uint32_t
/// @throws
/// @param suppressPadding Whether padding is suppressed
/// @return
uint32_t PCtxEncryptorPublic_Rsa::GetModulusSize(bool suppressPadding)
    const noexcept  // Get key modulus length: different Padding schemes correspond to different values in RSA
{
    uint32_t const nBlockLen{static_cast< uint32_t >(GetBlockSize())};
    if (suppressPadding) {
        return nBlockLen;
    }
    // RSA_PKCS1_PADDING - 11   //RSA_PKCS1_OAEP_PADDING - 43   //RSA_NO_PADDING Length unchanged
    PH_ASSERT(nBlockLen > kInt_11U);
    return nBlockLen - kInt_11U;
}
//********************************/
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara
