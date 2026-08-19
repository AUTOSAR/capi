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
/// @file       isoft_ctx_kdf_des.cpp
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
/// @unit_name=PCtxKdf_Des
/// @unit_description=DES-based key derivation context
/// @endcode
///
/// ================================================================

#include "ara/crypto/cryp/kdf/isoft_ctx_kdf_des.h"

#include "ara/crypto/common/isoft_data_type.h"
#include "ara/crypto/cryp/cryobj/isoft_crypto_primitive_id.h"
#include "ara/crypto/cryp/cryobj/isoft_key_symmetric_des.h"
#include "ara/crypto/cryp/isoft_crypto_provider.h"
#include "ara/crypto/cryp/isoft_service_extension.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
//********************************/
/// @brief Key derivation function interface: OpenSSL encapsulation.
/// @param cryptoProvider crypto provider
PCtxKdf_Des::PCtxKdf_Des(PCryptoProvider& cryptoProvider) noexcept : PCtxKdf_Base{cryptoProvider}
{
    nKeyBitLength_ = static_cast< uint32_t >(PAlgId_Symmetric_DesKey::EKeyLen::kDesKeyMinBitLength);
}
//***************/  //CryptoContext interface
/// @brief Returns a CryptoPrimitivId instance containing the instance identifier.
/// @name  GetCryptoPrimitiveId
/// @returns CryptoPrimitivId instance
CryptoPrimitiveId::Uptr PCtxKdf_Des::GetCryptoPrimitiveId() const noexcept
{
    return {std::make_unique< PAlgId_Kdf_Des >()};
    CryptoPrimitiveId::Uptr pReturn{nullptr};
    switch (nKeyBitLength_) {
        case kInt_64: {
            pReturn = std::make_unique< isoft_def::PAlgId_Kdf_Des >();
        } break;
        case kInt_192: {
            pReturn = std::make_unique< isoft_def::PAlgId_Kdf_3Des >();
        } break;
        default: {
            pReturn = std::make_unique< isoft_def::PAlgId_Kdf_Des >();
        } break;
    }
    return pReturn;
}
/// @brief Get ExtensionService instance.
/// @name  GetExtensionService
/// @returns ExtensionService instance
ExtensionService::Uptr PCtxKdf_Des::GetExtensionService() const noexcept
{
    return {std::make_unique< PServiceExtension< PCtxKdf_Des > >(*this)};
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
/// @name  GetKeyIdSize
std::size_t PCtxKdf_Des::GetKeyIdSize() const noexcept { return static_cast< size_t >(nKeyBitLength_) / kInt_8U; }
//***************/
/// @brief Check whether the given key material meets requirements
/// @name  ChecRestrictedUseObject
/// @param sourceKM source key material
/// @returns  true if stafiy false otherwise
bool PCtxKdf_Des::ChecRestrictedUseObject(RestrictedUseObject const& sourceKM) const noexcept
{
    PAlgId_Symmetric_DesKey cryptoKey;
    // 2022-01-21 Accept key types: PAlgId_Symmetric_DesKey
    return cryptoKey.IsMinePrimitiveId(sourceKM.GetCryptoPrimitiveId()->GetPrimitiveId());
}
/// @brief Check whether the length of the input raw key meets requirements
/// @name  CheckSourceKeyLen
/// @param nKeyLen key length
/// @returns  true if keylen is support false otherwise
bool PCtxKdf_Des::CheckSourceKeyLen(uint32_t nKeyLen) const noexcept
{
    //
    return nKeyLen >= kInt_8U;
}
//********************************/
/// @brief Parameterized constructor
/// @param cryptoProvider crypto provider
PCtxKdf_3Des::PCtxKdf_3Des(PCryptoProvider& cryptoProvider) noexcept : PCtxKdf_Des{cryptoProvider}
{
    _SetKeyBitLen(static_cast< uint32_t >(PAlgId_Symmetric_DesKey::EKeyLen::kDesKeyMaxBitLength));
}
//***************/  //CryptoContext interface
/// @brief Returns a CryptoPrimitivId instance containing the instance identifier.
/// @return CryptoPrimitivId instance
CryptoPrimitiveId::Uptr PCtxKdf_3Des::GetCryptoPrimitiveId() const noexcept
{
    return {std::make_unique< PAlgId_Kdf_3Des >()};
}
//***************/  //KeyDerivationFunctionCtx interface
/// @brief Get ExtensionService instance.
/// @return ExtensionService instance
ExtensionService::Uptr PCtxKdf_3Des::GetExtensionService() const noexcept
{
    return {std::make_unique< PServiceExtension< PCtxKdf_3Des > >(*this)};
}
//***************/
/// @brief Check whether the given key material meets requirements. This key can be either deskey or 3deskey.
/// @param sourceKM source key material
/// @return true if stafiy false otherwise
bool PCtxKdf_3Des::ChecRestrictedUseObject(RestrictedUseObject const& sourceKM) const noexcept
{
    PAlgId_Symmetric_3DesKey cryptoKey;
    // 2022-01-21 Accept key types: PAlgId_Symmetric_DesKey
    return cryptoKey.IsMinePrimitiveId(sourceKM.GetCryptoPrimitiveId()->GetPrimitiveId());
}
//********************************/
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara
