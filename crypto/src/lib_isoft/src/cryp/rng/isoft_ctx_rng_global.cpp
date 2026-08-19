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
/// @file       isoft_ctx_rng_global.cpp
/// @brief      AutoSar-Crypto Encryption and Decryption Module
/// @details
/// @date       2023-07-17
/// @author     Zheng Chang
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default Encryption and Decryption/Random Number Generation
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_01016
/// @unit_name=PCtxRng_Global
/// @unit_description=Global Random Number Context
/// @endcode
///
/// ================================================================

#include "ara/crypto/cryp/rng/isoft_ctx_rng_global.h"

#include <openssl/rand.h>

#include "ara/crypto/common/isoft_data_type.h"
#include "ara/crypto/cryp/isoft_crypto_provider.h"
#include "ara/crypto/cryp/isoft_service_extension.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
//********************************/
/// @brief Interface of the random number generator context. //CryptoContext interface
/// @returns CryptoPrimitivId instance
CryptoPrimitiveId::Uptr PCtxRng_Global::GetCryptoPrimitiveId() const noexcept
{
    return {std::make_unique< PAlgId_RandomGenerator_Global >()};
}
/// @brief Return an allocated buffer containing a generated random sequence of the requested size.
/// @brief Return an allocated buffer with a generated random sequence of the requested size.
/// @param count number of random bytes to generate
/// @returns @c a buffer filled with the generated random sequence
/// @trace_id_sws={SWS_CRYPT_22915}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02206}
/// @error: SecurityErrorDomain::kUninitializedContext   if this context implements a local RNG  (i.e., the RNG state is
/// controlled by the application),
///             and has to be seeded by the application because it either has not already been seeded or ran out of
///             entropy.
/// @error: SecurityErrorDomain::kBusyResource           if this context implements a global RNG (i.e., the RNG state is
/// controlled by the stack and not the application)
///             that is currently out-of-entropy and therefore cannot provide the requested number of random bytes.
/// @threadsafety={Thread-safe}
ara::core::Result< ara::core::Vector< ara::core::Byte > > PCtxRng_Global::Generate(std::uint32_t const& count) noexcept
{
    /// @error: SecurityErrorDomain::kBusyResource           if this context implements a global RNG (i.e., the RNG
    /// state is controlled by the stack and not the application)
    ///             that is currently out-of-entropy and therefore cannot provide the requested number of random bytes.
    if (0 == RAND_status()) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kBusyResource);
    }

    ara::core::Vector< ara::core::Byte > vecRandom;
    vecRandom.resize(static_cast< std::size_t >(count));
    int32_t const ret{RAND_priv_bytes(T_TransBytes(vecRandom.data()), static_cast< int32_t >(count))};
    if (ret == 0) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kBusyResource);
    }

    return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromValue(vecRandom);
}
/// @brief Get an ExtensionService instance.
/// @brief Get ExtensionService instance.
/// @returns ExtensionService instance
/// @trace_id_sws={SWS_CRYPT_22902}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02006}
ExtensionService::Uptr PCtxRng_Global::GetExtensionService() const noexcept
{
    return {std::make_unique< PServiceExtension< PCtxRng_Global > >(*this)};
}
#if AP_VERSION_PUHUA == 20
/// @brief Update the internal state of the RNG by mixing it with the provided additional entropy. This method is optional for implementation. Implementations of this method can "accumulate" entropy for future use.
/// @brief Update the internal state of the RNG by mixing it with the provided additional entropy.
///         This method is optional for implementation. An implementation of this method may "accumulate" provided
///         entropy for future use.
/// @param entropy  a memory region with the additional entropy value
/// @returns @c has value or true if the method is supported and the entropy has been updated successfully
/// @trace_id_sws={SWS_CRYPT_22914}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02206}
/// @threadsafety={Thread-safe}
bool PCtxRng_Global::AddEntropy(ReadOnlyMemRegion const& entropy) noexcept
{
    // SWS_CRYPT_00502 :
    // If RandomGeneratorCtx uses global state, calling its Seed(), SetKey(), and AddEntropy() methods will return false without modifying the global state.
    std::ignore = entropy;
    return false;
}
#elif AP_VERSION_PUHUA == 30
/// @brief Update the internal state of the RNG by mixing it with the provided additional entropy. This method is optional for implementation. Implementations of this method can "accumulate" entropy for future use.
/// @brief Update the internal state of the RNG by mixing it with the provided additional entropy.
///         This method is optional for implementation. An implementation of this method may "accumulate" provided
///         entropy for future use.
/// @param entropy  a memory region with the additional entropy value
/// @returns @c has value or true if the method is supported and the entropy has been updated successfully
/// @trace_id_sws={SWS_CRYPT_22914}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02206}
/// @threadsafety={Thread-safe}
ara::core::Result< void > PCtxRng_Global::AddEntropy(ReadOnlyMemRegion const& entropy) noexcept
{
    // SWS_CRYPT_00502 :
    // If RandomGeneratorCtx uses global state, calling its Seed(), SetKey(), and AddEntropy() methods will return false without modifying the global state.
    std::ignore = entropy;
    return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUnsupported);
}
#endif
#if AP_VERSION_PUHUA == 20
/// @brief Set the internal state of the RNG using the provided seed.
/// @param seed Secret seed
/// @returns  @c has value or true if the method is supported and the state has been set successfully
bool PCtxRng_Global::Seed(ReadOnlyMemRegion const& seed) noexcept
{
    // SWS_CRYPT_00502 :
    // If RandomGeneratorCtx uses global state, calling its Seed(), SetKey(), and AddEntropy() methods will return false without modifying the global state.
    std::ignore = seed;
    return false;
}
#elif AP_VERSION_PUHUA == 30
/// @brief Set the internal state of the RNG using the provided seed.
/// @param seed Secret seed
/// @returns  @c has value or true if the method is supported and the state has been set successfully
ara::core::Result< void > PCtxRng_Global::Seed(ReadOnlyMemRegion const& seed) noexcept
{
    // SWS_CRYPT_00502 :
    // If RandomGeneratorCtx uses global state, calling its Seed(), SetKey(), and AddEntropy() methods will return false without modifying the global state.
    std::ignore = seed;
    return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUnsupported);
}
#endif
#if AP_VERSION_PUHUA == 20
/// @brief Set the internal state of the RNG using the provided seed.
/// @brief Set the internal state of the RNG using the provided seed
/// @param seed  a memory region with the seed value
/// @returns @c has value or true if the method is supported and the state has been set successfully
bool PCtxRng_Global::Seed(SecretSeed const& seed) noexcept
{
    // SWS_CRYPT_00502 :
    // If RandomGeneratorCtx uses global state, calling its Seed(), SetKey(), and AddEntropy() methods will return false without modifying the global state.
    std::ignore = seed;
    return false;
}
#elif AP_VERSION_PUHUA == 30
/// @brief Set the internal state of the RNG using the provided seed.
/// @brief Set the internal state of the RNG using the provided seed
/// @param seed  a memory region with the seed value
/// @returns @c has value or true if the method is supported and the state has been set successfully
ara::core::Result< void > PCtxRng_Global::Seed(SecretSeed const& seed) noexcept
{
    std::ignore = seed;
    return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUnsupported);
}
#endif
#if AP_VERSION_PUHUA == 20
/// @brief Set the internal state of the RNG using the provided seed.   //
/// @brief Set the internal state of the RNG using the provided seed
/// @param key  a SymmetricKey with the key used as seed value
/// @returns @c has value or true if the method is supported and the key has been set successfully
bool PCtxRng_Global::SetKey(SymmetricKey const& key) noexcept
{
    // SWS_CRYPT_00502 :
    // If RandomGeneratorCtx uses global state, calling its Seed(), SetKey(), and AddEntropy() methods will return false without modifying the global state.
    std::ignore = key;
    return false;
}
#elif AP_VERSION_PUHUA == 30
/// @brief Set the internal state of the RNG using the provided seed.   //
/// @brief Set the internal state of the RNG using the provided seed
/// @param key  a SymmetricKey with the key used as seed value
/// @returns @c has value or true if the method is supported and the key has been set successfully
ara::core::Result< void > PCtxRng_Global::SetKey(SymmetricKey const& key) noexcept
{
    // SWS_CRYPT_00502 :
    // If RandomGeneratorCtx uses global state, calling its Seed(), SetKey(), and AddEntropy() methods will return false without modifying the global state.
    std::ignore = key;
    return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUnsupported);
}
#endif
/// @brief Check whether the encryption context has been initialized and is ready for use. It checks all required values, including: key value, IV/seed, etc.
/// @name  IsInitialized
/// @returns  bool
bool PCtxRng_Global::IsInitialized() const noexcept { return true; }
//***************/
/// @brief Get the allowed usage of this context (according to the key object properties loaded into this context).
/// If the context has not been initialized with a key object, it must return 0 (all flags reset).
/// @returns  a combination of bit-flags that specifies allowed usages of the context
AllowedUsageFlags PCtxRng_Global::GetAllowedUsage() const noexcept { return kAlgIdNone; }
/// @brief Get the maximum supported key length (in bits).
/// @brief Get maximal supported key length in bits.
/// @returns maximal supported length of the key in bits
/// @trace_id_sws={SWS_CRYPT_29044}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02309}
/// @threadsafety={Thread-safe}
/// @returns  std::size_t
std::size_t PCtxRng_Global::GetMaxKeyBitLength() const noexcept { return kInt_0U; }
/// @brief Get the minimum supported key length (in bits).
/// @brief Get minimal supported key length in bits.
/// @returns minimal supported length of the key in bits
/// @trace_id_sws={SWS_CRYPT_29043}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02309}
/// @threadsafety={Thread-safe}
std::size_t PCtxRng_Global::GetMinKeyBitLength() const noexcept { return kInt_0U; }
/// @brief Verify support for a specific key length according to the context.
/// @brief Verify supportness of specific key length by the context.
/// @param keyBitLength Key length: in bits
/// @returns @c true if provided value of the key length is supported by the context
/// @trace_id_sws={SWS_CRYPT_29048}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02309}
/// @threadsafety={Thread-safe}
bool PCtxRng_Global::IsKeyBitLengthSupported(std::size_t keyBitLength) const noexcept
{
    // By default, check that the Key.BitLength is between the minimum and maximum values: if the Ctx has special requirements, please implement this virtual function in the subclass
    std::ignore = keyBitLength;
    return false;
}
//********************************/
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara
