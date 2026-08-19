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
/// @file       isoft_service_extension.cpp
/// @brief      AutoSar-Crypto Encryption/Decryption Module
/// @details
/// @date       2022-03-11
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default Encryption/Service Module
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_06005
/// @unit_name=PServiceExtension
/// @unit_description=Base class for basic meta-information services for all contexts
/// @endcode
///
/// ================================================================

#include "ara/crypto/cryp/isoft_service_extension.h"

#include "ara/crypto/cryp/dh/isoft_ctx_key_agreement_private.h"
#include "ara/crypto/cryp/dsv/isoft_ctx_dsv_msg_recovery_public_rsa.h"
#include "ara/crypto/cryp/dsv/isoft_ctx_dsv_sig_encode_private_rsa.h"
#include "ara/crypto/cryp/kdf/isoft_ctx_kdf_aes.h"
#include "ara/crypto/cryp/kdf/isoft_ctx_kdf_des.h"
#include "ara/crypto/cryp/kem/isoft_ctx_key_decapsulator_private_rsa.h"
#include "ara/crypto/cryp/kem/isoft_ctx_key_encapsulator_public_rsa.h"
#include "ara/crypto/cryp/rng/isoft_ctx_rng_default.h"
#include "ara/crypto/cryp/rng/isoft_ctx_rng_global.h"
#include "ara/crypto/cryp/wrap/isoft_ctx_symmetric_key_wrapper_aes_pad.h"
#include "ara/crypto/cryp/wrap/isoft_ctx_symmetric_key_wrapper_aes_unpad.h"
#include "ara/crypto/cryp/wrap/isoft_ctx_symmetric_key_wrapper_des.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
//********************************/
// /// @brief Base class for basic meta-information services for all contexts.
// /// @param ctxCrypto Crypto context template class object
// template < typename T_CtxCrypto >
// inline PServiceExtension< T_CtxCrypto >::PServiceExtension(T_CtxCrypto const& ctxCrypto) noexcept
//     : ExtensionService{}, ctxCrypto_{ctxCrypto}
// {
// }
// //***************/
// /// @brief Get the actual bit length of the key loaded into the context. Returns 0 if no key is set for the context.
// /// @returns  actual length of a key (now set to the algorithm context) in bits
// template < typename T_CtxCrypto >
// inline std::size_t PServiceExtension< T_CtxCrypto >::GetActualKeyBitLength() const noexcept
// {
//     return ctxCrypto_.GetActualKeyBitLength();
// }
// /// @brief Get the COUID of the key deployed to the context attached to this extended service. Returns an empty COUID (Nil) if no key is set for the context.
// /// @returns  the COUID of the CryptoObject
// template < typename T_CtxCrypto >
// inline CryptoObjectUid PServiceExtension< T_CtxCrypto >::GetActualKeyCOUID() const noexcept
// {
//     return std::move(ctxCrypto_.GetActualKeyCOUID());
// }
// /// @brief Get the allowed usage for this context (based on the properties of the key object loaded into this context).
// ///         If the context has not yet been initialized with a key object, it must return 0 (all flags reset).
// /// @returns  a combination of bit-flags that specifies allowed usages of the context
// template < typename T_CtxCrypto >
// inline AllowedUsageFlags PServiceExtension< T_CtxCrypto >::GetAllowedUsage() const noexcept
// {
//     return ctxCrypto_.GetAllowedUsage();
// }
// /// @brief Get the maximum supported key length in bits.
// /// @returns  maximal supported length of the key in bits
// template < typename T_CtxCrypto >
// inline std::size_t PServiceExtension< T_CtxCrypto >::GetMaxKeyBitLength() const noexcept
// {
//     return ctxCrypto_.GetMaxKeyBitLength();
// }
// /// @brief Get the minimum supported key length in bits.
// template < typename T_CtxCrypto >
// /// @brief Get the minimum supported key length in bits.
// /// @returns  minimal supported length of the key in bits
// inline std::size_t PServiceExtension< T_CtxCrypto >::GetMinKeyBitLength() const noexcept
// {
//     return ctxCrypto_.GetMinKeyBitLength();
// }
// /// @brief Verify support for a specific key length based on the context.
// /// @param keyBitLength Key length: in bits
// /// @returns   @c true if provided value of the key length is supported by the context
// template < typename T_CtxCrypto >
// inline bool PServiceExtension< T_CtxCrypto >::IsKeyBitLengthSupported(std::size_t keyBitLength) const noexcept
// {
//     return ctxCrypto_.IsKeyBitLengthSupported(keyBitLength);
// }
// /// @brief Check if a key is set for this context.
// /// @returns true if a key has been set to this context false otherwise
// template < typename T_CtxCrypto >
// inline bool PServiceExtension< T_CtxCrypto >::IsKeyAvailable() const noexcept
// {
//     return ctxCrypto_.IsKeyAvailable();
// }
//********************************/
template class PServiceExtension< PCtxRng_Default >;
template class PServiceExtension< PCtxRng_Global >;
template class PServiceExtension< PCtxKdf_Des >;
template class PServiceExtension< PCtxKdf_3Des >;
template class PServiceExtension< PCtxKdf_Aes >;
template class PServiceExtension< PCtxDsv_SigEncodePrivate_Rsa >;
template class PServiceExtension< PCtxDsv_MsgRecoveryPublic_Rsa >;
template class PServiceExtension< PCtxSymmetricKeyWrapperAesPad >;
template class PServiceExtension< PCtxSymmetricKeyWrapperAesUnPad >;
template class PServiceExtension< PCtxSymmetricKeyWrapperDes >;
template class PServiceExtension< PCtxKeyDecapsulatorPrivateRsa >;
template class PServiceExtension< PCtxKeyEncapsulatorPublicRsa >;
template class PServiceExtension< PCtxKeyAgreementPrivate >;
//********************************/
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara
