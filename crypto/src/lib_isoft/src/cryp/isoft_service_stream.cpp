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
/// @file       isoft_service_stream.cpp
/// @brief      AutoSar-Crypto Encryption/Decryption Module
/// @details
/// @date       2022-02-16
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default Encryption/Service Module
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_06005
/// @unit_name=PServiceStream
/// @unit_description=Extended Meta-Information Service for Symmetric Encryption Stream Cipher Contexts
/// @endcode
///
/// ================================================================

#include "ara/crypto/cryp/isoft_service_stream.h"

#include "ara/crypto/cryp/ae/isoft_ctx_ae_stream_aes.h"
#include "ara/crypto/cryp/ae/isoft_ctx_ae_stream_des.h"
#include "ara/crypto/cryp/symmetric/isoft_ctx_symmetric_stream_aes.h"
#include "ara/crypto/cryp/symmetric/isoft_ctx_symmetric_stream_des.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
//********************************/
// /// @brief Parameterized constructor
// /// @param ctxCrypto Crypto context template class object
// template < typename T_CtxCrypto >
// inline PServiceStream< T_CtxCrypto >::PServiceStream(T_CtxCrypto const& ctxCrypto) noexcept
//     : BlockService{}, ctxCrypto_{ctxCrypto}
// {
// }
// //********************************/ //ExtensionService interface
// /// @brief Get the actual bit length of the key loaded into the context. Returns 0 if no key is set for the context.
// /// @returns  actual length of a key (now set to the algorithm context) in bits
// template < typename T_CtxCrypto >
// inline std::size_t PServiceStream< T_CtxCrypto >::GetActualKeyBitLength() const noexcept
// {
//     return ctxCrypto_.GetActualKeyBitLength();
// }
// /// @brief Get the COUID of the key deployed to the context attached to this extended service. Returns an empty COUID (Nil) if no key is set for the context.
// /// @returns  the COUID of the CryptoObject
// template < typename T_CtxCrypto >
// inline CryptoObjectUid PServiceStream< T_CtxCrypto >::GetActualKeyCOUID() const noexcept
// {
//     return std::move(ctxCrypto_.GetActualKeyCOUID());
// }
// /// @brief
// /// Get the allowed usage for this context (based on the properties of the key object loaded into this context). If the context has not yet been initialized with a key object, it must return 0 (all flags reset).
// /// @returns  a combination of bit-flags that specifies allowed usages of the context
// template < typename T_CtxCrypto >
// inline AllowedUsageFlags PServiceStream< T_CtxCrypto >::GetAllowedUsage() const noexcept
// {
//     return ctxCrypto_.GetAllowedUsage();
// }
// /// @brief Get the maximum supported key length in bits.
// /// @returns  maximal supported length of the key in bits
// template < typename T_CtxCrypto >
// inline std::size_t PServiceStream< T_CtxCrypto >::GetMaxKeyBitLength() const noexcept
// {
//     return ctxCrypto_.GetMaxKeyBitLength();
// }
// /// @brief Get the minimum supported key length in bits.
// /// @returns  minimal supported length of the key in bits
// template < typename T_CtxCrypto >
// inline std::size_t PServiceStream< T_CtxCrypto >::GetMinKeyBitLength() const noexcept
// {
//     return ctxCrypto_.GetMinKeyBitLength();
// }
// /// @brief Verify support for a specific key length based on the context.
// /// @param keyBitLength Key length: in bits
// /// @returns @c true if provided value of the key length is supported by the context
// template < typename T_CtxCrypto >
// inline bool PServiceStream< T_CtxCrypto >::IsKeyBitLengthSupported(std::size_t keyBitLength) const noexcept
// {
//     return ctxCrypto_.IsKeyBitLengthSupported(keyBitLength);
// }
// /// @brief Check if a key is set for this context.
// /// @returns true if a key has been set to this context false otherwise
// template < typename T_CtxCrypto >
// inline bool PServiceStream< T_CtxCrypto >::IsKeyAvailable() const noexcept
// {
//     return ctxCrypto_.IsKeyAvailable();
// }
// //********************************/ //BlockService interface
// /// @brief Get the actual bit length of the IV loaded into the context.
// /// @tparam T_CtxCrypto
// /// @param ivUid UID of the Initialization Vector (IV)
// /// @return  actual length of the IV (now set to the algorithm context) in bits
// template < typename T_CtxCrypto >
// inline std::size_t PServiceStream< T_CtxCrypto >::GetActualIvBitLength(
//     ara::core::Optional< CryptoObjectUid >& ivUid) const noexcept
// {
//     return ctxCrypto_.GetActualIvBitLength(ivUid);
// }
// /// @brief Get the block (or internal buffer) size of the underlying algorithm.
// /// @returns  size of the block in bytes
// template < typename T_CtxCrypto >
// inline std::size_t PServiceStream< T_CtxCrypto >::GetBlockSize() const noexcept
// {
//     return ctxCrypto_.GetBlockSize();
// }
// /// @brief Get the default expected size of the Initialization Vector (IV) or nonce.
// /// @returns  default expected size of IV in bytes
// template < typename T_CtxCrypto >
// inline std::size_t PServiceStream< T_CtxCrypto >::GetIvSize() const noexcept
// {
//     return ctxCrypto_.GetIvSize();
// }
// /// @brief Verify the validity of a specific Initialization Vector (IV) length.
// /// @param ivSize Initialization vector length
// /// @returns @c true if provided IV length is supported by the algorithm and @c false otherwise
// template < typename T_CtxCrypto >
// inline bool PServiceStream< T_CtxCrypto >::IsValidIvSize(std::size_t ivSize) const noexcept
// {
//     /// @returns @c true if provided IV length is supported by the algorithm and @c false otherwise
//     return ctxCrypto_.IsValidIvSize(ivSize);
// }
//********************************/ //Explicitly declare template class
// 2022-02-16 hanjingjing: PServiceStream can also be specifically designed as a pointer to a specific type family
template class PServiceStream< PCtxSymmetricStream_Des_Cfb >;
template class PServiceStream< PCtxSymmetricStream_Des_Cfb64 >;
template class PServiceStream< PCtxSymmetricStream_Des_Ofb >;
template class PServiceStream< PCtxSymmetricStream_Des_Ofb64 >;
template class PServiceStream< PCtxSymmetricStream_3Des_Cfb1 >;
template class PServiceStream< PCtxSymmetricStream_3Des_Cfb64 >;
template class PServiceStream< PCtxSymmetricStream_3Des_Ofb64 >;
template class PServiceStream< PCtxSymmetricStream_Aes_Cfb1 >;
template class PServiceStream< PCtxSymmetricStream_Aes_Cfb8 >;
template class PServiceStream< PCtxSymmetricStream_Aes_Cfb128 >;
template class PServiceStream< PCtxSymmetricStream_Aes_Ofb128 >;
template class PServiceStream< PCtxAeStreamAes_Cbc >;
template class PServiceStream< PCtxAeStreamDes_Cbc >;
template class PServiceStream< PCtxSymmetricStream_Aes_Ctr >;
//********************************/
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara
