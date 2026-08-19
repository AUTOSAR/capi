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
/// @file       public_key.h
/// @brief      AutoSar-Crypto Encryption/Decryption module
/// @details    Generic asymmetric public key interface.
/// @date       2021-12-21
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr><td>2021-12-21  <td>1.0.0    <td>hanjingjing <td>Created initial version
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default Encryption/Decryption/Asymmetric Public Key
/// @interface_level=software
/// @trace_id_sr=SR_CRYPTO_06005
/// @unit_name=PublicKey
/// @unit_description=Public Key Base Class
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_PUBLIC_KEY_H_
#define ARA_CRYPTO_CRYP_PUBLIC_KEY_H_

#include "ara/crypto/common/base_id_types.h"
#include "ara/crypto/common/mem_region.h"
#include "ara/crypto/common/serializable.h"
#include "ara/crypto/cryp/cryobj/restricted_use_object.h"
#include "ara/crypto/cryp/hash_function_ctx.h"

namespace ara {
namespace crypto {
namespace cryp {
//********************************/
/// @brief Generic asymmetric public key interface.
/// @brief General Asymmetric Public Key interface.
/// @interface PublicKey
/// @AUTOSAR_SWS {SWS_CRYPT_22700}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02202}
//********************************/
/// @code{.isoft}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02519
/// @trace_id_dd=DD_CRYPTO_05312
/// @needwork = ad
/// @endcode
class PublicKey : public RestrictedUseObject
{
public:
    /// @brief Unique smart pointer for the interface.
    /// @brief Unique smart pointer of the interface.
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_22701}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02202}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_03216
    /// @trace_id_dd=DD_CRYPTO_06428
    /// @needwork = ad
    /// @endcode
    using Uptr = std::unique_ptr< PublicKey >;
    /// @brief Unique smart pointer for the constant interface.
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_03217
    /// @trace_id_dd=DD_CRYPTO_06429
    /// @needwork = ad
    /// @endcode
    using Uptrc = std::unique_ptr< PublicKey const >;

private:
    /// @brief Constant object type
    /// @brief const object type
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_22702}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02202}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05313
    /// @needwork = dda
    /// @endcode
    CryptoObjectType const kObjectType{CryptoObjectType::kPublicKey};

public:
    /// @brief Get the type of cryptographic material.
    /// @return Key type
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02520
    /// @trace_id_dd=DD_CRYPTO_05314
    /// @needwork = ad
    /// @endcode
    CryptoObjectType GetkObjectType() const noexcept { return kObjectType; }

public:
    /// @brief Check the correctness of the key.
    /// @brief Check the key for its correctness.
    /// @param strongCheck  the severeness flag that indicates type of the required check: strong (if @c true) or
    /// fast (if @c false)
    /// @return @c true if the key is correct
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_22711}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02202}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02521
    /// @trace_id_dd=DD_CRYPTO_05315
    /// @needwork = ad
    /// @endcode
    virtual bool CheckKey(bool strongCheck = true) const noexcept = 0;
    /// @brief Calculate the hash value of the public key value. The raw public key value BLOB can be obtained via the Serializable interface.
    /// @brief Calculate hash of the Public Key value.
    ///        The original public key value BLOB is available via the @c Serializable interface.
    /// @param hashFunc  a hash-function instance that should be used the hashing
    /// @return a buffer preallocated for the resulting hash value
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kInsufficientCapacity   if size of the hash buffer is not enough for storing of the
    /// result
    /// @error: SecurityErrorDomain::kIncompleteArgState     if the @c hashFunc context is not initialized
    /// @trace_id_sws={SWS_CRYPT_22712}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02202}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02522
    /// @trace_id_dd=DD_CRYPTO_05316
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< ara::core::Vector< ara::core::Byte > > HashPublicKey(
        HashFunctionCtx &hashFunc) const noexcept = 0;
    /// @brief Calculate the hash value of the public key value. This method sets the size of the output container based on the actually saved values!
    ///         The raw public key value BLOB can be obtained via the Serializable interface.
    /// @brief Calculate hash of the Public Key value.
    ///    This method sets the size of the output container according to actually saved value!
    ///    The original public key value BLOB is available via the Serializable interface.
    /// @param hashFunc  a hash-function instance that should be used the hashing
    /// @return  pre-reserved managed container for the resulting hash value
    /// @code{.isoft}
    /// @tparam Alloc  a custom allocator type of the output container
    /// @error: SecurityErrorDomain::kInsufficientCapacity   if capacity of the hash buffer is not enough for storing of
    /// the result
    /// @error: SecurityErrorDomain::kIncompleteArgState     if the @c hashFunc context is not initialized
    /// @trace_id_sws={SWS_CRYPT_22713}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02202}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02523
    /// @trace_id_dd=DD_CRYPTO_05317
    /// @needwork = ad
    /// @endcode
    template < typename Alloc = VENDOR_IMPLEMENTATION_DEFINED >
    // PRQA S 2135 QAC  /// @qac: AUTOSAR standard interface
    ara::core::Result< ByteVector< Alloc > > HashPublicKey(HashFunctionCtx &hashFunc) const noexcept
    // PRQA L:QAC
    {
        ara::core::Result< ara::core::Vector< ara::core::Byte > > result = HashPublicKey(hashFunc);
        if (false == result.HasValue()) {
            return ara::core::Result< ByteVector< Alloc > >::FromError(result.Error());
        }

        ByteVector< Alloc > hash;
        hash.resize(result.Value().size());
        memcpy(core::data(hash), result.Value().data(), result.Value().size());
        return ara::core::Result< ByteVector< Alloc > >::FromValue(hash);
    }

public:
};
//********************************/
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYP_PUBLIC_KEY_H_
