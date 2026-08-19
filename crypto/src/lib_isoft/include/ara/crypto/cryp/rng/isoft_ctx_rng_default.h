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
/// @file       isoft_ctx_rng_default.h
/// @brief      AutoSar-Crypto encryption/decryption module
/// @details    Random number generator context interface.
/// @date       2022-03-11
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr>}<2022-03-11  </td>1.0.0    <td>hanjingjing      <td>Create initial version</td>
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default encryption/Random number generation
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_01017
/// @unit_name=PCtxRng_Default
/// @unit_description=Default random number context
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_PUHUA_CTX_RNG_DEAULT_H_
#define ARA_CRYPTO_CRYP_PUHUA_CTX_RNG_DEAULT_H_

#include "ara/crypto/cryp/rng/isoft_ctx_rng_base.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
//********************************/
/// @brief Default random number generator context interface.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00160
/// @trace_id_dd=DD_CRYPTO_00961
/// @needwork = ad
/// @endcode
class PCtxRng_Default : public PCtxRng_Base
{
public:
    /// @brief Unique smart pointer for the interface.
    /// @brief Shared smart pointer of the interface.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00160
    /// @trace_id_dd=DD_CRYPTO_06258
    /// @needwork = dd
    /// @endcode
    using Uptr = std::unique_ptr< PCtxRng_Default >;

public:
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_06259
    /// @needwork = dda
    /// @endcode
    using PCtxRng_Base::PCtxRng_Base;
    /// @brief default virtual destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00962
    /// @needwork = dda
    /// @endcode
    ~PCtxRng_Default() noexcept override = default;
    /// @brief default move constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00963
    /// @needwork = dda
    /// @endcode
    PCtxRng_Default(PCtxRng_Default&& other) = delete;
    /// @brief default move assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00964
    /// @needwork = dda
    /// @endcode
    PCtxRng_Default& operator=(PCtxRng_Default&& other) = delete;
    /// @brief default copy assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00965
    /// @needwork = dda
    /// @endcode
    PCtxRng_Default& operator=(PCtxRng_Default const& other) = delete;
    /// @brief default copy constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00966
    /// @needwork = dda
    /// @endcode
    PCtxRng_Default(PCtxRng_Default const& other) = delete;

public:  // AP-AUTOSAR interface
    /// @brief Returns the CryptoPrimitivId instance containing the instance identifier.
    /// @name   GetCryptoPrimitiveId
    /// @returns CryptoPrimitivId instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00967
    /// @needwork = dda
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override;
    /// @brief Returns an allocated buffer containing a generated random sequence of the requested size.
    /// @name   Generate
    /// @param count Number of random bytes to generate
    /// @returns  @c a buffer filled with the generated random sequence
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00969
    /// @needwork = dda
    /// @endcode
    ara::core::Result< ara::core::Vector< ara::core::Byte > > Generate(std::uint32_t const& count) noexcept override;
    /// @brief Get the ExtensionService instance.
    /// @name   GetExtensionService
    /// @returns ExtensionService instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00970
    /// @needwork = dda
    /// @endcode
    ExtensionService::Uptr GetExtensionService() const noexcept override;

#if AP_VERSION_PUHUA == 20
    /// @brief Update the internal state of the RNG by mixing it with the provided additional entropy.
    /// This method is optional for the implementation. Implementations of this method can "accumulate" entropy for future use.
    /// @name   AddEntropy
    /// @param entropy Provided additional entropy to mix
    /// @returns @c has value or true if the method is supported and the entropy has been updated successfully
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00968
    /// @needwork = dda
    /// @endcode
    bool AddEntropy(ReadOnlyMemRegion const& entropy) noexcept override;
    /// @brief Set the internal state of the RNG using the provided seed.
    /// @param seed Secret seed
    /// @return @c has value or true if the method is supported and the state has been set successfully
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00971
    /// @needwork = dda
    /// @endcode
    bool Seed(SecretSeed const& seed) noexcept override;
    /// @brief Set the internal state of the RNG using memory.
    /// @name   Seed
    /// @param seed Secret seed
    /// @return @c has value or true if the method is supported and the state has been set successfully
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00972
    /// @needwork = dda
    /// @endcode
    bool Seed(ReadOnlyMemRegion const& seed) noexcept override;
    /// @brief Set the internal state of the RNG using the provided seed.   //???
    /// @brief Set the internal state of the RNG using the provided seed
    /// @param key  a SymmetricKey with the key used as seed value
    /// @return @c has value or true if the method is supported and the key has been set successfully
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00973
    /// @needwork = dda
    /// @endcode
    bool SetKey(SymmetricKey const& key) noexcept override;
#elif AP_VERSION_PUHUA == 30
    /// @brief Update the internal state of the RNG by mixing it with the provided additional entropy.
    /// This method is optional for the implementation. Implementations of this method can "accumulate" entropy for future use.
    /// @name   AddEntropy
    /// @param entropy Provided additional entropy to mix
    /// @returns @c has value or true if the method is supported and the entropy has been updated successfully
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00968
    /// @needwork = dda
    /// @endcode
    ara::core::Result< void > AddEntropy(ReadOnlyMemRegion const& entropy) noexcept override;
    /// @brief Set the internal state of the RNG using the provided seed.
    /// @param seed Secret seed
    /// @return @c has value or true if the method is supported and the state has been set successfully
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00971
    /// @needwork = dda
    /// @endcode
    ara::core::Result< void > Seed(SecretSeed const& seed) noexcept override;
    /// @brief Set the internal state of the RNG using memory.
    /// @name   Seed
    /// @param seed Secret seed
    /// @return @c has value or true if the method is supported and the state has been set successfully
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00972
    /// @needwork = dda
    /// @endcode
    ara::core::Result< void > Seed(ReadOnlyMemRegion const& seed) noexcept override;
    /// @brief Set the internal state of the RNG using the provided seed.   //???
    /// @brief Set the internal state of the RNG using the provided seed
    /// @param key  a SymmetricKey with the key used as seed value
    /// @return @c has value or true if the method is supported and the key has been set successfully
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00973
    /// @needwork = dda
    /// @endcode
    ara::core::Result< void > SetKey(SymmetricKey const& key) noexcept override;
#endif

public:  // PServiceExtension interface
    /// @brief Get the allowed usage of this context (according to the key object properties loaded into this context).
    /// If the context has not been initialized with a key object, must return 0 (all flags reset).
    /// @name   GetAllowedUsage
    /// @returns a combination of bit-flags that specifies allowed usages of the context
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00974
    /// @needwork = dda
    /// @endcode
    AllowedUsageFlags GetAllowedUsage() const noexcept override;
    /// @brief Get the maximum supported key length (in bits).
    /// @name   GetMaxKeyBitLength
    /// @returns maximal supported length of the key in bits
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00975
    /// @needwork = dda
    /// @endcode
    std::size_t GetMaxKeyBitLength() const noexcept override;
    /// @brief Get the minimum supported key length (in bits).
    /// @name   GetMinKeyBitLength
    /// @returns minimal supported length of the key in bits
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00976
    /// @needwork = dda
    /// @endcode
    std::size_t GetMinKeyBitLength() const noexcept override;
    /// @brief Validate support for a specific key length according to the context.
    /// @name   IsKeyBitLengthSupported
    /// @param keyBitLength Key length: in bits
    /// @returns @c true if provided value of the key length is supported by the context
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00977
    /// @needwork = dda
    /// @endcode
    bool IsKeyBitLengthSupported(std::size_t keyBitLength) const noexcept override;

public:  // PCtxRandomGenerator interface
    /// @brief Initialize RNG
    /// The return value indicates whether seed initialization is allowed (some global random number contexts may not initialize seeds due to occupied shared resources).
    /// @name   InitlizeRng
    /// @returns true if has already init false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00978
    /// @needwork = dda
    /// @endcode
    bool InitlizeRng() noexcept;

protected:
    /// @brief Set key via IPC
    /// @name   _SetKeyIpc
    /// @param stFuncName Function name used for IPC call
    /// @param key Symmetric key
    /// @returns true if set ipc key sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00979
    /// @needwork = dda
    /// @endcode
    ara::core::Result< void > _SetKeyIpc(ara::core::StringView const& stFuncName, SymmetricKey const& key) noexcept;
    /// @brief Generate random numbers via IPC
    /// @param stFuncName Function name used for IPC call
    /// @param count Number of random bytes to generate
    /// @returns Generated random number
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00980
    /// @needwork = dda
    /// @endcode
    ara::core::Result< ara::core::Vector< ara::core::Byte > > _GenerateIpc(ara::core::StringView const& stFuncName,
                                                                           std::uint32_t const& count) const noexcept;

private:
    /// @brief Encapsulates common code from AddEntropy (SWS_CRYPT_22914) interfaces for AP2.0 and AP3.0 versions due to return value changes.
    /// @param entropy Seed, ReadOnlyMemRegion type
    /// @return Has value: success; No value: failure
    ara::core::Result< void > _addEntropy(ReadOnlyMemRegion const& entropy) noexcept;
    /// @brief Encapsulates common code from seed (SWS_CRYPT_22911) interfaces for AP2.0 and AP3.0 versions due to return value changes.
    /// @param seed Seed, ReadOnlyMemRegion type
    /// @return Has value: success; No value: failure
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    ara::core::Result< void > _seedRom(ReadOnlyMemRegion const& seed) noexcept;
    /// @brief Encapsulates common code from seed (SWS_CRYPT_22912) interfaces for AP2.0 and AP3.0 versions due to return value changes.
    /// @param seed Seed, SecretSeed type
    /// @return Has value: success; No value: failure
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    ara::core::Result< void > _seedSecretSeed(SecretSeed const& seed) noexcept;
    /// @brief Encapsulates common code from SetKey (SWS_CRYPT_22913) interfaces for AP2.0 and AP3.0 versions due to return value changes.
    /// @param key Key
    /// @return Has value: success; No value: failure
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    ara::core::Result< void > _setSymmetricKey(SymmetricKey const& key) noexcept;

public:
};
//********************************/
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYP_PUHUA_CTX_RNG_DEAULT_H_
