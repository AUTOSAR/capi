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
/// @file       secret_seed.h
/// @brief      AutoSar-Crypto Encryption/Decryption module
/// @details    Secret seed object interface.
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
/// @module_path=/CRYPTO/Default Encryption/Decryption/Secret Seed
/// @interface_level=software
/// @trace_id_sr=SR_CRYPTO_06005
/// @unit_name=SecretSeed
/// @unit_description=Secret Seed Object
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_SECRET_SEED_H_
#define ARA_CRYPTO_CRYP_SECRET_SEED_H_

#include "ara/core/result.h"
#include "ara/crypto/common/base_id_types.h"
#include "ara/crypto/common/mem_region.h"
#include "ara/crypto/cryp/cryobj/crypto_object.h"
#include "ara/crypto/cryp/cryobj/restricted_use_object.h"

namespace ara {
namespace crypto {
namespace cryp {
//********************************/
/// @brief Secret seed object interface.
///     This object contains a raw bit sequence of a specific length (without any filtering of allowed/disallowed values)!
///     Secret seed values can only be loaded into non-key inputs (like IV/salt/nonce) of cryptographic transformation contexts!
///     The bit length of the secret seed is specific to the concrete encryption algorithm, corresponding to the maximum of its input/output/salt block lengths.
/// @brief Secret Seed object interface.
///     This object contains a raw bit sequence of specific length (without any filtering of allowed/disallowed values)!
///     The secret seed value can be loaded only to a non-key input of a cryptographic transformation context (like
///     IV/salt/nonce)! Bit length of the secret seed is specific to concret crypto algorithm and corresponds to maximum
///     of its input/output/salt block-length.
/// @interface SecretSeed
/// @AUTOSAR_SWS {SWS_CRYPT_23000}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02007}
//********************************/
/// @code{.isoft}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02524
/// @trace_id_dd=DD_CRYPTO_05318
/// @needwork = ad
/// @endcode
class SecretSeed : public RestrictedUseObject
{
public:
    /// @brief Unique smart pointer for the constant interface.
    /// @brief Unique smart pointer of a constant interface instance.
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_23001}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02007}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_03218
    /// @trace_id_dd=DD_CRYPTO_06430
    /// @needwork = ad
    /// @endcode
    using Uptrc = std::unique_ptr< SecretSeed const >;
    /// @brief Unique smart pointer for the volatile interface instance.
    /// @brief Unique smart pointer of a volatile interface instance.
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_23002}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02007}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_03219
    /// @trace_id_dd=DD_CRYPTO_06431
    /// @needwork = ad
    /// @endcode
    using Uptr = std::unique_ptr< SecretSeed >;

private:
    /// @brief This interface maps to a static specific value of the CryptoObjectType enumeration.
    /// @brief Static mapping of this interface to specific value of @c CryptoObjectType enumeration.
    /// static CryptoObjectType const  kObjectType = CryptoObjectType::kSecretSeed;
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_23003}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02007}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_05319
    /// @needwork = dda
    /// @endcode
    CryptoObjectType const kObjectType{CryptoObjectType::kSecretSeed};

public:
    /// @brief Get the type of cryptographic material.
    /// @return Key object type
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02525
    /// @trace_id_dd=DD_CRYPTO_05320
    /// @needwork = ad
    /// @endcode
    CryptoObjectType GetkObjectType() const noexcept { return kObjectType; }

public:
    /// @brief Clone this Secret Seed object into a new session object.
    /// The created object instance is session and non-exportable. The AllowedUsageFlags property of the "cloned" object is the same as that of the source object!
    ///         If the size of the xorDelta parameter is smaller than the value size of this seed, only the corresponding leading bytes of the original seed should be XOR-ed, but the rest should be copied without change.
    ///         If the size of the xorDelta parameter is larger than the value size of this seed, the extra bytes of xorDelta should be ignored.
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_23011}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02007}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02526
    /// @trace_id_dd=DD_CRYPTO_05321
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< SecretSeed::Uptr >
    /// @brief Clone this Secret Seed object to new session object.
    ///         Created object instance is session and non-exportable, @c AllowedUsageFlags attribute of the "cloned"
    ///         object is identical to this attribute of the source object! If size of the @c xorDelta argument is less
    ///         than the value size of this seed then only correspondent number of leading bytes of the original seed
    ///         should be XOR-ed, but the rest should be copied without change. If size of the @c xorDelta argument is
    ///         larger than the value size of this seed then extra bytes of the @c xorDelta should be ignored.
    /// @param xorDelta  optional "delta" value that must be XOR-ed with the "cloned" copy of the original seed
    /// @returns unique smart pointer to "cloned" session @c SecretSeed object
    Clone(ReadOnlyMemRegion const &xorDelta = ReadOnlyMemRegion()) const noexcept = 0;
    /// @brief Set the value of this seed object to "jump" from the initial state to the specified number of steps, according to the "count" expression defined by the encryption algorithm associated with this object.
    ///         Steps can be positive or negative, corresponding to forward and backward "jumps", but a value of 0 means copying the value only to this seed object. The seed size of the from parameter must be greater than or equal to this seed size.
    /// @brief Set value of this seed object as a "jump" from an initial state to specified number of steps,
    ///           according to "counting" expression defined by a cryptographic algorithm associated with this object.
    ///       @c steps may have positive and negative values that correspond to forward and backward direction of the
    ///       "jump" respectively, but 0 value means only copy @c from value to this seed object.
    ///       Seed size of the @c from argument always must be greater or equal of this seed size.
    /// @param from  source object that keeps the initial value for jumping from
    /// @param steps  number of steps for the "jump"
    /// @return reference to this updated object
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kIncompatibleObject  if this object and the @c from argument are associated
    ///                with incompatible cryptographic algorithms
    /// @error: SecurityErrorDomain::kInvalidInputSize  if value size of the @c from seed is less then
    ///                value size of this one
    /// @trace_id_sws={SWS_CRYPT_23012}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02007}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02527
    /// @trace_id_dd=DD_CRYPTO_05322
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< void > JumpFrom(SecretSeed const &from, std::int64_t steps) noexcept = 0;
    /// @brief Set the value of this seed object to "jump" from its current state to the specified number of steps, according to the "count" expression defined by the encryption algorithm associated with this object.
    ///         Steps can be positive or negative, corresponding to forward and backward "jumps", but a value of 0 means the current seed value does not change.
    /// @brief Set value of this seed object as a "jump" from it's current state to specified number of steps,
    ///           according to "counting" expression defined by a cryptographic algorithm associated with this object.
    ///       @c steps may have positive and negative values that correspond to forward and backward direction of the
    ///       "jump" respectively, but 0 value means no changes of the current seed value.
    /// @param steps  number of "steps" for jumping (forward or backward) from the current state
    /// @return reference to this updated object
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_23014}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02007}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02528
    /// @trace_id_dd=DD_CRYPTO_05323
    /// @needwork = ad
    /// @endcode
    virtual SecretSeed &Jump(std::int64_t steps) noexcept = 0;
    /// @brief Set the next value of the secret seed according to the "count" expression defined by the cipher algorithm associated with this object.
    ///         If the associated cipher algorithm does not specify a "count" expression, then a generic increment operation must be implemented as the default (little-endian representation, i.e., the first byte is the least significant).
    /// @brief Set next value of the secret seed according to "counting" expression defined by a cryptographic algorithm
    /// associated with this object.
    ///       If the associated cryptographic algorithm doesn't specify a "counting" expression then generic increment
    ///       operation must be implemented as default (little-endian notation, i.e. first byte is least significant).
    /// @return reference to this updated object
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_23013}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02007}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02529
    /// @trace_id_dd=DD_CRYPTO_05324
    /// @needwork = ad
    /// @endcode
    virtual SecretSeed &Next() noexcept = 0;
    /// @brief XOR this seed object with another seed object and save the result to this object.
    ///         If the seed sizes of this object and the source parameter differ, only the corresponding leading bytes in this seed object should be updated.
    /// @brief XOR value of this seed object with another one and save result to this object.
    ///       If seed sizes in this object and in the @c source argument are different then only correspondent number
    ///       of leading bytes in this seed object should be updated.
    /// @param source  right argument for the XOR operation
    /// @return reference to this updated object
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_23015}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02007}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02530
    /// @trace_id_dd=DD_CRYPTO_05325
    /// @needwork = ad
    /// @qac [2657]possibly cannot be modified: This assignment operator is declared 'virtual'.
    /// @endcode
    // PRQA S 2657 QAC /// @qac: AUTOSAR standard interface
    virtual SecretSeed &operator^=(SecretSeed const &source) &noexcept = 0;
    // PRQA L:QAC
    /// @brief XOR this seed object with the provided memory region and save the result to this object.
    ///         If the seed sizes of this object and the source parameter differ, the leading bytes of this seed object should be updated.
    /// @brief XOR value of this seed object with provided memory region and save result to this object.
    ///       If seed sizes in this object and in the @c source argument are different then only correspondent number
    ///       of leading bytes of this seed object should be updated.
    /// @param source  right argument for the XOR operation
    /// @return reference to this updated object
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_23016}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02007}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02531
    /// @trace_id_dd=DD_CRYPTO_05326
    /// @needwork = ad
    /// @qac [2657]possibly cannot be modified: This assignment operator is declared 'virtual'.
    /// @endcode
    // PRQA S 2657 QAC /// @qac: AUTOSAR standard interface
    virtual SecretSeed &operator^=(ReadOnlyMemRegion const &source) &noexcept = 0;
    // PRQA L:QAC

public:
};
//********************************/
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYP_SECRET_SEED_H_
