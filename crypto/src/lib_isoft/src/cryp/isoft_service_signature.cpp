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
/// @file       isoft_service_signature.cpp
/// @brief      AutoSar-Crypto Encryption/Decryption Module
/// @details
/// @date       2022-03-23
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default Encryption/Service Module
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_06005
/// @unit_name=PServiceSignature
/// @unit_description=Extended Meta-Information Service for Signature Contexts
/// @endcode
///
/// ================================================================

#include "ara/crypto/cryp/isoft_service_signature.h"

#include "ara/crypto/cryp/cryobj/crypto_primitive_id.h"
#include "ara/crypto/cryp/dsv/isoft_ctx_dsv_signer_private_rsa.h"
#include "ara/crypto/cryp/dsv/isoft_ctx_dsv_signer_private_rsa_pss.h"
#include "ara/crypto/cryp/dsv/isoft_ctx_dsv_verifier_public_rsa.h"
#include "ara/crypto/cryp/dsv/isoft_ctx_dsv_verifier_public_rsa_pss.h"

// isoft rsa
#include "ara/crypto/cryp/dsv/isoft_ctx_dsv_signer_private_ecdsa.h"
#include "ara/crypto/cryp/dsv/isoft_ctx_dsv_verifier_public_ecdsa.h"
namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
//********************************/
/// @brief Extended Meta-Information Service for Signature Contexts.
//********************************/
// template < typename T_CtxCrypto >
// /// @brief Parameterized constructor
// /// @param ctxCrypto Crypto context template class object
// inline PServiceSignature< T_CtxCrypto >::PServiceSignature(T_CtxCrypto const& ctxCrypto) noexcept
//     : SignatureService{}, ctxCrypto_{ctxCrypto}
// {
// }
// //***************/  //ExtensionService interface
// /// @brief Get the actual bit length of the key loaded into the context. Returns 0 if no key is set for the context.
// /// @brief Get actual bit-length of a key loaded to the context. If no key was set to the context yet then 0 is
// /// returned.
// /// @returns actual length of a key (now set to the algorithm context) in bits
// /// @trace_id_sws={SWS_CRYPT_29045}
// /// @tracestatus={draft}
// /// @uptrace={RS_CRYPTO_02309}
// /// @threadsafety={Thread-safe}
// template < typename T_CtxCrypto >
// inline std::size_t PServiceSignature< T_CtxCrypto >::GetActualKeyBitLength() const noexcept
// {
//     return ctxCrypto_.GetActualKeyBitLength();
// }
// /// @brief Get the COUID of the key deployed to the context attached to this extended service. Returns an empty COUID (Nil) if no key is set for the context.
// /// @brief Get the COUID of the key deployed to the context this extension service is attached to.
// /// If no key was set to the context yet then an empty COUID (Nil) is returned.
// /// @returns the COUID of the CryptoObject
// /// @trace_id_sws={SWS_CRYPT_29047}
// /// @tracestatus={draft}
// /// @uptrace={RS_CRYPTO_02309}
// /// @threadsafety={Thread-safe}
// template < typename T_CtxCrypto >
// inline CryptoObjectUid PServiceSignature< T_CtxCrypto >::GetActualKeyCOUID() const noexcept
// {
//     return std::move(ctxCrypto_.GetActualKeyCOUID());
// }
// /// @brief
// /// Get the allowed usage for this context (based on the properties of the key object loaded into this context). If the context has not yet been initialized with a key object, it must return 0 (all flags reset).
// /// @brief Get allowed usages of this context (according to the key object attributes loaded to this context).
// /// If the context is not initialized by a key object yet then zero (all flags are reset) must be returned.
// /// @tparam T_CtxCrypto
// /// @return a combination of bit-flags that specifies allowed usages of the context
// /// @trace_id_sws={SWS_CRYPT_29046}
// /// @tracestatus={draft}
// /// @uptrace={RS_CRYPTO_02008}
// /// @threadsafety={Thread-safe}
// template < typename T_CtxCrypto >
// inline AllowedUsageFlags PServiceSignature< T_CtxCrypto >::GetAllowedUsage() const noexcept
// {
//     return ctxCrypto_.GetAllowedUsage();
// }
// /// @brief Get the maximum supported key length in bits.
// /// @brief Get maximal supported key length in bits.
// /// @tparam T_CtxCrypto
// /// @return maximal supported length of the key in bits
// /// @trace_id_sws={SWS_CRYPT_29044}
// /// @tracestatus={draft}
// /// @uptrace={RS_CRYPTO_02309}
// /// @threadsafety={Thread-safe}
// template < typename T_CtxCrypto >
// inline std::size_t PServiceSignature< T_CtxCrypto >::GetMaxKeyBitLength() const noexcept
// {
//     return ctxCrypto_.GetMaxKeyBitLength();
// }
// /// @brief Get the minimum supported key length in bits.
// /// @brief Get minimal supported key length in bits.
// /// @tparam T_CtxCrypto
// /// @return minimal supported length of the key in bits
// /// @trace_id_sws={SWS_CRYPT_29043}
// /// @tracestatus={draft}
// /// @uptrace={RS_CRYPTO_02309}
// /// @threadsafety={Thread-safe}
// template < typename T_CtxCrypto >
// inline std::size_t PServiceSignature< T_CtxCrypto >::GetMinKeyBitLength() const noexcept
// {
//     return ctxCrypto_.GetMinKeyBitLength();
// }
// /// @brief Verify support for a specific key length based on the context.
// /// @brief Verify supportness of specific key length by the context.
// /// @tparam T_CtxCrypto
// /// @param keyBitLength Key length: in bits
// /// @return @c true if provided value of the key length is supported by the context
// /// @trace_id_sws={SWS_CRYPT_29048}
// /// @tracestatus={draft}
// /// @uptrace={RS_CRYPTO_02309}
// /// @threadsafety={Thread-safe}
// template < typename T_CtxCrypto >
// inline bool PServiceSignature< T_CtxCrypto >::IsKeyBitLengthSupported(std::size_t keyBitLength) const noexcept
// {
//     return ctxCrypto_.IsKeyBitLengthSupported(keyBitLength);
// }
// /// @brief Check if a key is set for this context.
// /// @brief Check if a key has been set to this context.
// /// @tparam T_CtxCrypto
// /// @return  true if a key has been set to this context false otherwise
// /// @trace_id_sws={SWS_CRYPT_29049}
// /// @tracestatus={draft}
// /// @uptrace={RS_CRYPTO_02309}
// /// @threadsafety={Thread-safe}
// template < typename T_CtxCrypto >
// inline bool PServiceSignature< T_CtxCrypto >::IsKeyAvailable() const noexcept
// {
//     return ctxCrypto_.IsKeyAvailable();
// }
// //***************/  //SignatureService interface
// /// @brief Get the ID of the hash algorithm required by the current signature algorithm.
// /// @brief Get an ID of hash algorithm required by current signature algorithm.
// /// @tparam T_CtxCrypto
// /// @return required hash algorithm ID or @c kAlgIdAny if the signature algorithm specification does not include a
// /// concrete hash function
// /// @trace_id_sws={SWS_CRYPT_29003}
// /// @tracestatus={draft}
// /// @uptrace={RS_CRYPTO_02309}
// /// @threadsafety={Thread-safe}
// template < typename T_CtxCrypto >
// inline CryptoPrimitiveId::AlgId PServiceSignature< T_CtxCrypto >::GetRequiredHashAlgId() const noexcept
// {
//     return ctxCrypto_.GetRequiredHashAlgId();
// }
// /// @brief Get the hash size required by the current signature algorithm.
// /// @brief Get the hash size required by current signature algorithm.
// /// @returns required hash size in bytes
// /// @tparam T_CtxCrypto
// /// @trace_id_sws={SWS_CRYPT_29002}
// /// @tracestatus={draft}
// /// @uptrace={RS_CRYPTO_02309}
// template < typename T_CtxCrypto >
// inline std::size_t PServiceSignature< T_CtxCrypto >::GetRequiredHashSize() const noexcept
// {
//     return ctxCrypto_.GetRequiredHashSize();
// }
// /// @brief Get the size of the signature value produced and required by the current algorithm.
// /// @brief Get size of the signature value produced and required by the current algorithm.
// /// @returns size of the signature value in bytes
// /// @tparam T_CtxCrypto
// /// @trace_id_sws={SWS_CRYPT_29004}
// /// @tracestatus={draft}
// /// @uptrace={RS_CRYPTO_02309}
// /// @threadsafety={Thread-safe}
// template < typename T_CtxCrypto >
// inline std::size_t PServiceSignature< T_CtxCrypto >::GetSignatureSize() const noexcept
// {
//     return ctxCrypto_.GetSignatureSize();
// }
//********************************/
/// @brief sign ecdsa
template class PServiceSignature< PCtxDsv_SignerPrivate_Ecdsa >;
template class PServiceSignature< PCtxDsv_SignerPrivate_Rsa_Md5 >;
template class PServiceSignature< PCtxDsv_SignerPrivate_Rsa_Sha1 >;
template class PServiceSignature< PCtxDsv_SignerPrivate_Rsa_Sha2_224 >;
template class PServiceSignature< PCtxDsv_SignerPrivate_Rsa_Sha2_256 >;
template class PServiceSignature< PCtxDsv_SignerPrivate_Rsa_Sha2_384 >;
template class PServiceSignature< PCtxDsv_SignerPrivate_Rsa_Sha2_512 >;
/// @brief sign pss
template class PServiceSignature< PCtxDsv_SignerPrivate_Rsa_Pss_Md5 >;
template class PServiceSignature< PCtxDsv_SignerPrivate_Rsa_Pss_Sha1 >;
template class PServiceSignature< PCtxDsv_SignerPrivate_Rsa_Pss_Sha2_224 >;
template class PServiceSignature< PCtxDsv_SignerPrivate_Rsa_Pss_Sha2_256 >;
template class PServiceSignature< PCtxDsv_SignerPrivate_Rsa_Pss_Sha2_384 >;
template class PServiceSignature< PCtxDsv_SignerPrivate_Rsa_Pss_Sha2_512 >;
/// @brief verify no-pss
template class PServiceSignature< PCtxDsv_VerifierPublic_Rsa_Md5 >;
template class PServiceSignature< PCtxDsv_VerifierPublic_Rsa_Sha1 >;
template class PServiceSignature< PCtxDsv_VerifierPublic_Rsa_Sha2_224 >;
template class PServiceSignature< PCtxDsv_VerifierPublic_Rsa_Sha2_256 >;
template class PServiceSignature< PCtxDsv_VerifierPublic_Rsa_Sha2_384 >;
template class PServiceSignature< PCtxDsv_VerifierPublic_Rsa_Sha2_512 >;
/// @brief verify ecdsa
template class PServiceSignature< PCtxDsv_VerifierPublic_Ecdsa >;
/// @brief verify pss
template class PServiceSignature< PCtxDsv_VerifierPublic_Rsa_Pss_Md5 >;
template class PServiceSignature< PCtxDsv_VerifierPublic_Rsa_Pss_Sha1 >;
template class PServiceSignature< PCtxDsv_VerifierPublic_Rsa_Pss_Sha2_224 >;
template class PServiceSignature< PCtxDsv_VerifierPublic_Rsa_Pss_Sha2_256 >;
template class PServiceSignature< PCtxDsv_VerifierPublic_Rsa_Pss_Sha2_384 >;
template class PServiceSignature< PCtxDsv_VerifierPublic_Rsa_Pss_Sha2_512 >;
/********************************/
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara
