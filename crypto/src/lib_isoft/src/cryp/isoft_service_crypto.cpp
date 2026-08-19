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
/// @file       isoft_service_crypto.cpp
/// @brief      AutoSar-Crypto Encryption/Decryption Module
/// @details
/// @date       2021-12-21
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default Encryption/Service Module
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_06005
/// @unit_name=PServiceCrypto
/// @unit_description=Extended Meta-Information Service for Crypto Contexts
/// @endcode
///
/// ================================================================

#include "ara/crypto/cryp/isoft_service_crypto.h"

#include "ara/crypto/cryp/asymmetric/isoft_ctx_decryptor_private_rsa.h"
#include "ara/crypto/cryp/asymmetric/isoft_ctx_encryptor_public_rsa.h"
#include "ara/crypto/cryp/symmetric/isoft_ctx_symmetric_block_aes.h"
#include "ara/crypto/cryp/symmetric/isoft_ctx_symmetric_block_des.h"
namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
//********************************/
// template < typename T_CtxCrypto >
// /// @brief Parameterized constructor
// /// @param ctxCrypto Crypto context template class object
// inline PServiceCrypto< T_CtxCrypto >::PServiceCrypto(T_CtxCrypto const& ctxCrypto) noexcept
//     : CryptoService{}, ctxCrypto_{ctxCrypto}
// {
// }
// //********************************/ //ExtensionService interface
// /// @brief Get the actual bit length of the key loaded into the context. Returns 0 if no key is set for the context.
// /// @return actual length of a key (now set to the algorithm context) in bits
// template < typename T_CtxCrypto >
// inline std::size_t PServiceCrypto< T_CtxCrypto >::GetActualKeyBitLength() const noexcept
// {
//     return ctxCrypto_.GetActualKeyBitLength();
// }
// /// @brief Get the COUID of the key deployed to the context attached to this extended service. Returns an empty COUID (Nil) if no key is set for the context.
// /// @return the COUID of the CryptoObject
// template < typename T_CtxCrypto >
// inline CryptoObjectUid PServiceCrypto< T_CtxCrypto >::GetActualKeyCOUID() const noexcept
// {
//     return std::move(ctxCrypto_.GetActualKeyCOUID());
// }
// /// @brief Get the allowed usage for this context (based on the properties of the key object loaded into this context).
// /// If the context has not yet been initialized with a key object, it must return 0 (all flags reset).
// /// @return a combination of bit-flags that specifies allowed usages of the context
// template < typename T_CtxCrypto >
// inline AllowedUsageFlags PServiceCrypto< T_CtxCrypto >::GetAllowedUsage() const noexcept
// {
//     return ctxCrypto_.GetAllowedUsage();
// }
// /// @brief Get the maximum supported key length in bits.
// /// @return maximal supported length of the key in bits
// template < typename T_CtxCrypto >
// inline std::size_t PServiceCrypto< T_CtxCrypto >::GetMaxKeyBitLength() const noexcept
// {
//     return ctxCrypto_.GetMaxKeyBitLength();
// }
// /// @brief Get the minimum supported key length in bits.
// /// @return minimal supported length of the key in bits
// template < typename T_CtxCrypto >
// inline std::size_t PServiceCrypto< T_CtxCrypto >::GetMinKeyBitLength() const noexcept
// {
//     return ctxCrypto_.GetMinKeyBitLength();
// }
// /// @brief Verify support for a specific key length based on the context.
// /// @tparam T_CtxCrypto
// /// @param keyBitLength Key length: in bits
// /// @return  @c true if provided value of the key length is supported by the context
// template < typename T_CtxCrypto >
// inline bool PServiceCrypto< T_CtxCrypto >::IsKeyBitLengthSupported(std::size_t keyBitLength) const noexcept
// {
//     return ctxCrypto_.IsKeyBitLengthSupported(keyBitLength);
// }
// /// @brief Check if a key is set for this context.
// /// @return true if a key has been set to this context false otherwise
// template < typename T_CtxCrypto >
// inline bool PServiceCrypto< T_CtxCrypto >::IsKeyAvailable() const noexcept
// {
//     return ctxCrypto_.IsKeyAvailable();
// }
// //********************************/ //CryptoService interface
// /// @brief Get the block (or internal buffer) size of the underlying algorithm.
// ///         For digest, byte-stream cipher, and RNG contexts, this is an informational method used only for interface optimization.
// /// @return size of the block in bytes
// template < typename T_CtxCrypto >
// inline std::size_t PServiceCrypto< T_CtxCrypto >::GetBlockSize() const noexcept
// {
//     return ctxCrypto_.GetBlockSize();
// }
// /// @brief Get the maximum expected size of the input data block. With the suppressPadding parameter, it will be equal to the block size.
// /// @brief Get maximum expected size of the input data block. @c suppressPadding argument and it will be equal to the
// /// block size.
// /// @param suppressPadding  if @c true then the method calculates the size for the case when the whole space of the
// /// plain data block is used for the payload only
// /// @returns maximum size of the input data block in bytes
// /// @trace_id_sws={SWS_CRYPT_29021}
// /// @tracestatus={draft}
// /// @uptrace={RS_CRYPTO_02309}
// /// @threadsafety={Thread-safe}
// template < typename T_CtxCrypto >
// inline std::size_t PServiceCrypto< T_CtxCrypto >::GetMaxInputSize(bool suppressPadding) const noexcept
// {
//     return ctxCrypto_.GetMaxInputSize(suppressPadding);
// }
// /// @brief Get the maximum possible size of the output data block.
// ///         If (IsEncryption() == true), the value returned by this method is independent of the suppressPadding parameter and will be equal to the block size.
// /// @brief Get maximum possible size of the output data block.
// ///         If (IsEncryption() == true) then a value returned by this method is independent from the @c suppressPadding
// ///         argument and will be equal to the block size.
// /// @param suppressPadding  if @c true then the method calculates the size for the case when the whole space of the
// /// plain data block is used for the payload only
// /// @returns maximum size of the output data block in bytes
// /// @trace_id_sws={SWS_CRYPT_29022}
// /// @tracestatus={draft}
// /// @uptrace={RS_CRYPTO_02309}
// /// @threadsafety={Thread-safe}
// template < typename T_CtxCrypto >
// inline std::size_t PServiceCrypto< T_CtxCrypto >::GetMaxOutputSize(bool suppressPadding) const noexcept
// {
//     return ctxCrypto_.GetMaxOutputSize(suppressPadding);
// }
//********************************/ //Explicitly declare template class
template class PServiceCrypto< PCtxSymmetricBlock_Des_Ecb >;
template class PServiceCrypto< PCtxSymmetricBlock_Des_Cbc >;
template class PServiceCrypto< PCtxSymmetricBlock_3Des_Ecb >;
template class PServiceCrypto< PCtxSymmetricBlock_3Des_Cbc >;
template class PServiceCrypto< PCtxSymmetricBlock_Aes_Ecb >;
template class PServiceCrypto< PCtxSymmetricBlock_Aes_Cbc >;
template class PServiceCrypto< PCtxEncryptorPublic_Rsa >;
template class PServiceCrypto< PCtxDecryptorPrivate_Rsa >;
//********************************/
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara
