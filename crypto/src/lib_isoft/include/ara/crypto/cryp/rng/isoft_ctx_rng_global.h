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
/// @file       isoft_ctx_rng_global.h
/// @brief      AutoSar-Crypto Encryption Module
/// @details    Interface for random number generator context.
/// @date       2023-07-17
/// @author     Chang Zheng
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr><td>2023-07-17  <td>1.0.0    <td>Chang Zheng  <td>Initial version creation
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default Encryption/Random Number Generation
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_01016
/// @unit_name=PCtxRng_Global
/// @unit_description=Global Random Number Context
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_PUHUA_CTX_RNG_GLOBAL_H_
#define ARA_CRYPTO_CRYP_PUHUA_CTX_RNG_GLOBAL_H_

#include "ara/crypto/cryp/rng/isoft_ctx_rng_base.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
//********************************/
/// @brief Interface for random number generator context, default interface.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00188
/// @trace_id_dd=DD_CRYPTO_01015
/// @needwork = ad
/// @endcode
class PCtxRng_Global : public PCtxRng_Base
{
public:
    /// @brief Unique smart pointer of the interface.
    /// @brief Shared smart pointer of the interface.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00188
    /// @trace_id_dd=DD_CRYPTO_06261
    /// @needwork = dd
    /// @endcode
    using Uptr = std::unique_ptr< PCtxRng_Global >;

public:
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06262
    /// @needwork = dda
    /// @endcode
    using PCtxRng_Base::PCtxRng_Base;
    /// @brief Default virtual destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01016
    /// @needwork = dda
    /// @endcode
    ~PCtxRng_Global() noexcept override = default;
    /// @brief Default move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01017
    /// @needwork = dda
    /// @endcode
    PCtxRng_Global(PCtxRng_Global&& other) = delete;
    /// @brief Default move assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01018
    /// @needwork = dda
    /// @endcode
    PCtxRng_Global& operator=(PCtxRng_Global&& other) = delete;
    /// @brief Default copy assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01019
    /// @needwork = dda
    /// @endcode
    PCtxRng_Global& operator=(PCtxRng_Global const& other) = delete;
    /// @brief Default copy constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01020
    /// @needwork = dda
    /// @endcode
    PCtxRng_Global(PCtxRng_Global const& other) = delete;

public:  // AP-AUTOSAR interface
    /// @brief Returns the CryptoPrimitivId instance containing the instance identifier.
    /// @name   GetCryptoPrimitiveId
    /// @returns CryptoPrimitivId instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01021
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override;
    /// @brief Checks if the crypto context has been initialized and is ready for use. It checks all required values, including: key values, IV/seed, etc.
    /// @brief Check if the crypto context is already initialized and ready to use.
    ///           It checks all required values, including: key value, IV/seed, etc.
    /// @returns @c true if the crypto context is completely initialized and ready to use, and @c false otherwise
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_20412}@tracestatus{draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_08693
    /// @needwork = dda
    /// @endcode
    bool IsInitialized() const noexcept override;
    /// @brief Returns an allocated buffer containing a generated random sequence of the requested size.
    /// @name   Generate
    /// @param count Number of random bytes to generate
    /// @returns @c a buffer filled with the generated random sequence
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01023
    /// @needwork = dda
    /// @endcode
    ara::core::Result< ara::core::Vector< ara::core::Byte > > Generate(std::uint32_t const& count) noexcept override;
    /// @brief Gets the ExtensionService instance.
    /// @name   GetExtensionService
    /// @returns ExtensionService instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01024
    /// @needwork = dda
    /// @endcode
    ExtensionService::Uptr GetExtensionService() const noexcept override;
#if AP_VERSION_PUHUA == 20
    /// @brief Updates the internal state of the RNG by mixing it with the provided additional entropy.
    /// This method is optional for implementation. Implementations of this method may "accumulate" entropy for future use.
    /// @name   AddEntropy
    /// @param entropy Provided additional entropy mix
    /// @returns @c has value or true if the method is supported and the entropy has been updated successfully
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01022
    /// @needwork = dda
    /// @endcode
    bool AddEntropy(ReadOnlyMemRegion const& entropy) noexcept override;
    /// @brief Sets the internal state of the RNG using the provided seed.
    /// @name   Seed
    /// @param seed Secret seed
    /// @return @c has value or true if the method is supported and the state has been set successfully
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01025
    /// @needwork = dda
    /// @endcode
    bool Seed(ReadOnlyMemRegion const& seed) noexcept override;
    /// @brief Sets the internal state of the RNG using the provided seed.
    /// @param seed Secret seed
    /// @return @c has value or true if the method is supported and the state has been set successfully
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01026
    /// @needwork = dda
    /// @endcode
    bool Seed(SecretSeed const& seed) noexcept override;
    /// @brief Sets the internal state of the RNG using the provided seed.   //???
    /// @brief Set the internal state of the RNG using the provided seed
    /// @param key  a SymmetricKey with the key used as seed value
    /// @return @c has value or true if the method is supported and the key has been set successfully
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01027
    /// @needwork = dda
    /// @endcode
    bool SetKey(SymmetricKey const& key) noexcept override;
#elif AP_VERSION_PUHUA == 30
    /// @brief Updates the internal state of the RNG by mixing it with the provided additional entropy.
    /// This method is optional for implementation. Implementations of this method may "accumulate" entropy for future use.
    /// @name   AddEntropy
    /// @param entropy Provided additional entropy mix
    /// @returns @c has value or true if the method is supported and the entropy has been updated successfully
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01022
    /// @needwork = dda
    /// @endcode
    ara::core::Result< void > AddEntropy(ReadOnlyMemRegion const& entropy) noexcept override;
    /// @brief Sets the internal state of the RNG using the provided seed.
    /// @name   Seed
    /// @param seed Secret seed
    /// @return @c has value or true if the method is supported and the state has been set successfully
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01025
    /// @needwork = dda
    /// @endcode
    ara::core::Result< void > Seed(ReadOnlyMemRegion const& seed) noexcept override;
    /// @brief Sets the internal state of the RNG using the provided seed.
    /// @param seed Secret seed
    /// @return @c has value or true if the method is supported and the state has been set successfully
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01026
    /// @needwork = dda
    /// @endcode
    ara::core::Result< void > Seed(SecretSeed const& seed) noexcept override;
    /// @brief Sets the internal state of the RNG using the provided seed.   //???
    /// @brief Set the internal state of the RNG using the provided seed
    /// @param key  a SymmetricKey with the key used as seed value
    /// @return @c has value or true if the method is supported and the key has been set successfully
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01027
    /// @needwork = dda
    /// @endcode
    ara::core::Result< void > SetKey(SymmetricKey const& key) noexcept override;
#endif

public:  // PServiceExtension interface
    /// @brief
    /// Gets the allowed usage of this context (based on the properties of the key object loaded into this context). If the context has not been initialized with a key object, it must return 0 (all flags reset).
    /// @name   GetAllowedUsage
    /// @returns a combination of bit-flags that specifies allowed usages of the context
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01028
    /// @needwork = dda
    /// @endcode
    AllowedUsageFlags GetAllowedUsage() const noexcept override;
    /// @brief Gets the maximum supported key length in bits.
    /// @name   GetMaxKeyBitLength
    /// @returns maximal supported length of the key in bits
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01029
    /// @needwork = dda
    /// @endcode
    std::size_t GetMaxKeyBitLength() const noexcept override;
    /// @brief Gets the minimum supported key length in bits.
    /// @name   GetMinKeyBitLength
    /// @returns minimal supported length of the key in bits
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01030
    /// @needwork = dda
    /// @endcode
    std::size_t GetMinKeyBitLength() const noexcept override;
    /// @brief Verifies support for a specific key length based on the context.
    /// @name   IsKeyBitLengthSupported
    /// @param keyBitLength Key length: in bits
    /// @returns @c true if provided value of the key length is supported by the context
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01031
    /// @needwork = dda
    /// @endcode
    bool IsKeyBitLengthSupported(std::size_t keyBitLength) const noexcept override;

public:
};
//********************************/
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYP_PUHUA_CTX_RNG_GLOBAL_H_
