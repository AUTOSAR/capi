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
/// @file       isoft_ctx_kdf_aes.cpp
/// @brief      AutoSar-Crypto encryption/decryption module
/// @details
/// @date       2022-03-15
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/DefaultEncryptionDecryption/KeyDerivation
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_01014
/// @unit_name=PCtxKdf_Aes
/// @unit_description=AES-based key derivation context
/// @endcode
///
/// ================================================================

#include "ara/crypto/cryp/kdf/isoft_ctx_kdf_aes.h"

#include "ara/crypto/common/isoft_data_type.h"
#include "ara/crypto/cryp/cryobj/isoft_key_symmetric_aes.h"
#include "ara/crypto/cryp/isoft_crypto_provider.h"
#include "ara/crypto/cryp/isoft_service_extension.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
//********************************/
/// @brief Key derivation function interface: OpenSSL encapsulation.
/// @param cryptoProvider crypto provider
/// @param nKeyBitLength key length: in bits
/// @returns
/// @throws
PCtxKdf_Aes::PCtxKdf_Aes(PCryptoProvider &cryptoProvider, uint32_t const nKeyBitLength) noexcept
    : PCtxKdf_Base{cryptoProvider}, nKeyBitLength_{nKeyBitLength}
{
}
//***************/  //CryptoContext interface
/// @brief Returns a CryptoPrimitivId instance containing the instance identifier.
/// @returns CryptoPrimitiveId instance
CryptoPrimitiveId::Uptr PCtxKdf_Aes::GetCryptoPrimitiveId() const noexcept
{
    CryptoPrimitiveId::Uptr pReturn{nullptr};
    switch (nKeyBitLength_) {
        case kInt_128: {
            pReturn = std::make_unique< isoft_def::PAlgId_Kdf_Aes128 >();
        } break;
        case kInt_192: {
            pReturn = std::make_unique< isoft_def::PAlgId_Kdf_Aes192 >();
        } break;
        case kInt_256: {
            pReturn = std::make_unique< isoft_def::PAlgId_Kdf_Aes256 >();
        } break;
        default: {
            pReturn = std::make_unique< isoft_def::PAlgId_Kdf_Aes >();
        } break;
    }
    return pReturn;
}
/// @brief Get ExtensionService instance.
/// @brief Get ExtensionService instance.
/// @returns ExtensionService instance.
/// @trace_id_sws={SWS_CRYPT_21517}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02006}
ExtensionService::Uptr PCtxKdf_Aes::GetExtensionService() const noexcept
{
    return {std::make_unique< PServiceExtension< PCtxKdf_Aes > >(*this)};
}
/// @brief Get the fixed size of the application-specific "filler" required for this context instance. If this instance of the key derivation context does not support filler values, 0 should be returned.
///			Get the fixed size of the target key ID required for the diversification algorithm. The return value is constant for each interface instance, i.e., independent of configuration.
/// @brief Get the fixed size of an application specific "filler" required by this context instance.
///			If this instance of the key derivation context does not support filler values, 0 shall be
/// returned. 			Get the fixed size of the target key ID required by diversification algorithm. Returned
/// value is constant for each instance of the interface, i.e. independent from configuration by
/// @returns size of the application specific filler in bytes Returned value is constant for this instance of the key
/// derivation context, i.e. independent from configuration by the @c Init() call. size of the key ID in bytes the @c
/// Init() call.
///         For this instance of the key derivation context, the return value is constant, i.e., independent of the configuration of @c Init() call. @c
///         The byte size of the key ID when @c Init() is called.
/// @trace_id_sws={SWS_CRYPT_21518}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02103}
/// @threadsafety={Thread-safe}
std::size_t PCtxKdf_Aes::GetKeyIdSize() const noexcept { return static_cast< size_t >(nKeyBitLength_) / kInt_8U; }
//***************/
/// @brief Check whether the given key material meets requirements
/// @param sourceKM source key material
/// @returns  true if stafiy false otherwise
bool PCtxKdf_Aes::ChecRestrictedUseObject(RestrictedUseObject const &sourceKM) const noexcept
{
    PAlgId_Symmetric_AesKey const cryptoKey;
    // 2022-01-21 Accept key types: PAlgId_Symmetric_DesKey
    return cryptoKey.IsMinePrimitiveId(sourceKM.GetCryptoPrimitiveId()->GetPrimitiveId());
}
/// @brief Check whether the length of the input raw key meets requirements
/// @name  CheckSourceKeyLen
/// @param nKeyLen key length
/// @returns  true if keylen is support false otherwise
bool PCtxKdf_Aes::CheckSourceKeyLen(uint32_t nKeyLen) const noexcept { return nKeyLen >= kInt_16U; }
/// @brief Get the maximum supported key length (in bits).
/// @name  GetMaxKeyBitLength
/// @returns  maximal supported length of the key in bits
std::size_t PCtxKdf_Aes::GetMaxKeyBitLength() const noexcept
{
    return static_cast< size_t >(PAlgId_Symmetric_AesKey::EKeyLen::kKeyBitLength_256);
}
/// @brief Get the minimum supported key length (in bits).
/// @name  GetMinKeyBitLength
/// @returns  minimal supported length of the key in bits
std::size_t PCtxKdf_Aes::GetMinKeyBitLength() const noexcept
{
    return static_cast< size_t >(PAlgId_Symmetric_AesKey::EKeyLen::kKeyBitLength_128);
}
//********************************/
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara
