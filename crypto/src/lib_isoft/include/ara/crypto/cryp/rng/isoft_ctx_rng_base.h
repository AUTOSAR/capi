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
/// @file       isoft_ctx_rng_base.h
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
/// <tr>}<2022-03-11  </td>1.0.0    </td>hanjingjing      <td>Create initial version</td>
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default encryption/Random number generation
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_01017
/// @unit_name=PCtxRng_Base
/// @unit_description=Random number generation base class
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_PUHUA_CTX_RNG_BASE_H_
#define ARA_CRYPTO_CRYP_PUHUA_CTX_RNG_BASE_H_

#include "ara/crypto/cryp/random_generator_ctx.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
/// @brief Crypto provider
class PCryptoProvider;
//********************************/
/// @brief Random number generator context interface.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00161
/// @trace_id_dd=DD_CRYPTO_00981
/// @needwork = ad
/// @endcode
class PCtxRng_Base : public RandomGeneratorCtx
{
public:
    /// @brief the stream work state
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00162
    /// @trace_id_dd=DD_CRYPTO_00982
    /// @needwork = ad
    /// @endcode
    enum class EStreamWorkState : std::uint8_t
    {
        /// @brief Not started
        kNotStarted = 0,
        /// @brief Started
        kStart = 0x01,
        /// @brief Working
        kUpdate = 0x02,
        /// @brief Finished
        kFinish = 0x0F,
    };
    /// @brief the buff data length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00163
    /// @trace_id_dd=DD_CRYPTO_00983
    /// @needwork = ad
    /// @endcode
    enum class EBuffLen : std::int32_t
    {
        /// @brief the max input data length
        kMaxInputBuffLen = 1024 * 4,
        /// @brief the max output data length
        kMaxOutputBuffLen = 1024,
    };

private:
    /// @brief Crypto provider object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00984
    /// @needwork = dda
    /// @endcode
    PCryptoProvider& cryptoProvider_;
    /// @brief Symmetric key pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00985
    /// @needwork = dda
    /// @endcode
    SymmetricKey const* pSymmetricKey_;
    /// @brief Working state //
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00986
    /// @needwork = dda
    /// @endcode
    EStreamWorkState eWorkState_;
    /// @brief Parameterized constructor
    /// @param cryptoProvider Crypto provider
    /// @param pSymmetricKey Symmetric key pointer
    /// @param eWorkState Working state
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_00987
    /// @needwork = dda
    /// @endcode
    PCtxRng_Base(PCryptoProvider& cryptoProvider,
                 SymmetricKey const* const pSymmetricKey,
                 EStreamWorkState const eWorkState) noexcept;

protected:
    /// @brief Set the working state.
    /// @param otherState Working state
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00164
    /// @trace_id_dd=DD_CRYPTO_00988
    /// @needwork = ad
    /// @endcode
    void _SetWorkState(EStreamWorkState const otherState) noexcept { eWorkState_ = otherState; }

public:
    /// @brief Unique smart pointer for the interface.
    /// @brief Shared smart pointer of the interface.
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_22901}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02206}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00161
    /// @trace_id_dd=DD_CRYPTO_06260
    /// @needwork = dd
    /// @endcode
    using Uptr = std::unique_ptr< PCtxRng_Base >;

public:
    /// @brief Parameterized constructor
    /// @param cryptoProvider PCryptoProvider
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00165
    /// @trace_id_dd=DD_CRYPTO_00989
    /// @needwork = ad
    /// @endcode
    explicit PCtxRng_Base(PCryptoProvider& cryptoProvider) noexcept;
    /// @brief default constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00166
    /// @trace_id_dd=DD_CRYPTO_00990
    /// @needwork = ad
    /// @endcode
    PCtxRng_Base() = delete;
    /// @brief default virtual destructor
    /// @brief Destructor.
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_20401}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02008}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00167
    /// @trace_id_dd=DD_CRYPTO_00991
    /// @needwork = ad
    /// @endcode
    ~PCtxRng_Base() noexcept override = default;
    /// @brief default move constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00168
    /// @trace_id_dd=DD_CRYPTO_00992
    /// @needwork = ad
    /// @endcode
    PCtxRng_Base(PCtxRng_Base&& other) = delete;
    /// @brief default move assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00169
    /// @trace_id_dd=DD_CRYPTO_00993
    /// @needwork = ad
    /// @endcode
    PCtxRng_Base& operator=(PCtxRng_Base&& other) = delete;
    /// @brief default copy assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00170
    /// @trace_id_dd=DD_CRYPTO_00994
    /// @needwork = ad
    /// @endcode
    PCtxRng_Base& operator=(PCtxRng_Base const& other) = delete;
    /// @brief default copy constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00171
    /// @trace_id_dd=DD_CRYPTO_00995
    /// @needwork = ad
    /// @endcode
    PCtxRng_Base(PCtxRng_Base const& other) = delete;

public:  // CryptoContext interface
    /// @brief Returns the CryptoPrimitivId instance containing the instance identifier.
    /// @brief Return CryptoPrimitivId instance containing instance identification.
    /// @return CryptoPrimitivId instance
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_20411}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02008}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00172
    /// @trace_id_dd=DD_CRYPTO_00996
    /// @needwork = ad
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override = 0;
    /// @brief Check whether the encryption context is initialized and usable. It checks all required values, including: key value, IV/seed, etc.
    /// @brief Check if the crypto context is already initialized and ready to use.
    ///           It checks all required values, including: key value, IV/seed, etc.
    /// @returns @c true if the crypto context is completely initialized and ready to use, and @c false otherwise
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_20412}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00173
    /// @trace_id_dd=DD_CRYPTO_00997
    /// @needwork = ad
    /// @endcode
    bool IsInitialized() const noexcept override;
    /// @brief Get a reference to the Crypto Provider for this context.
    /// @brief Get a reference to Crypto Provider of this context.
    /// @returns a reference to Crypto Provider instance that provides this context
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_20654}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02401}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00174
    /// @trace_id_dd=DD_CRYPTO_00998
    /// @needwork = ad
    /// @endcode
    CryptoProvider& MyProvider() const noexcept override;

public:  // RandomGeneratorCtx interface
    /// @brief Returns an allocated buffer containing a generated random sequence of the requested size.
    /// @brief Return an allocated buffer with a generated random sequence of the requested size.
    /// @param count number of random bytes to generate
    /// @returns @c a buffer filled with the generated random sequence
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kUninitializedContext   if this context implements a local RNG  (i.e., the RNG
    /// state is controlled by the application),
    ///             and has to be seeded by the application because it either has not already been seeded or ran out of
    ///             entropy.
    /// @error: SecurityErrorDomain::kBusyResource           if this context implements a global RNG (i.e., the RNG
    /// state is controlled by the stack and not the application)
    ///             that is currently out-of-entropy and therefore cannot provide the requested number of random bytes.
    /// @trace_id_sws={SWS_CRYPT_22915}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02206}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00176
    /// @trace_id_dd=DD_CRYPTO_01000
    /// @needwork = ad
    /// @endcode
    ara::core::Result< ara::core::Vector< ara::core::Byte > > Generate(
        std::uint32_t const& count) noexcept override = 0;
    /// @brief Get the ExtensionService instance.
    /// @brief Get ExtensionService instance.
    /// @return ExtensionService instance
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_22902}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02006}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00177
    /// @trace_id_dd=DD_CRYPTO_01001
    /// @needwork = ad
    /// @endcode
    ExtensionService::Uptr GetExtensionService() const noexcept override = 0;
#if AP_VERSION_PUHUA == 20
    /// @brief Update the internal state of the RNG by mixing it with the provided additional entropy.
    /// This method is optional for the implementation. Implementations of this method can "accumulate" entropy for future use.
    /// @brief Update the internal state of the RNG by mixing it with the provided additional entropy.
    ///         This method is optional for implementation. An implementation of this method may "accumulate" provided
    ///         entropy for future use.
    /// @param entropy  a memory region with the additional entropy value
    /// @returns @c has value or true if the method is supported and the entropy has been updated successfully
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_22914}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02206}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00175
    /// @trace_id_dd=DD_CRYPTO_00999
    /// @needwork = ad
    /// @endcode
    bool AddEntropy(ReadOnlyMemRegion const& entropy) noexcept override = 0;
#elif AP_VERSION_PUHUA == 30
    /// @brief Update the internal state of the RNG by mixing it with the provided additional entropy.
    /// This method is optional for the implementation. Implementations of this method can "accumulate" entropy for future use.
    /// @brief Update the internal state of the RNG by mixing it with the provided additional entropy.
    ///         This method is optional for implementation. An implementation of this method may "accumulate" provided
    ///         entropy for future use.
    /// @param entropy  a memory region with the additional entropy value
    /// @returns @c has value or true if the method is supported and the entropy has been updated successfully
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_22914}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02206}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00175
    /// @trace_id_dd=DD_CRYPTO_00999
    /// @needwork = ad
    /// @endcode
    ara::core::Result< void > AddEntropy(ReadOnlyMemRegion const& entropy) noexcept override = 0;
#endif
#if AP_VERSION_PUHUA == 20
    /// @brief Set the internal state of the RNG using the provided seed.
    /// @brief Set the internal state of the RNG using the provided seed
    /// @param seed  a memory region with the seed value
    /// @return @c has value or true if the method is supported and the state has been set successfully
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_22911}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02206}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00178
    /// @trace_id_dd=DD_CRYPTO_01002
    /// @needwork = ad
    /// @endcode
    bool Seed(ReadOnlyMemRegion const& seed) noexcept override = 0;
#elif AP_VERSION_PUHUA == 30
    /// @brief Set the internal state of the RNG using the provided seed.
    /// @brief Set the internal state of the RNG using the provided seed
    /// @param seed  a memory region with the seed value
    /// @return @c has value or true if the method is supported and the state has been set successfully
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_22911}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02206}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00178
    /// @trace_id_dd=DD_CRYPTO_01002
    /// @needwork = ad
    /// @endcode
    ara::core::Result< void > Seed(ReadOnlyMemRegion const& seed) noexcept override = 0;
#endif
#if AP_VERSION_PUHUA == 20
    /// @brief Set the internal state of the RNG using the provided seed.
    /// @brief Set the internal state of the RNG using the provided seed
    /// @param seed  a memory region with the seed value
    /// @return @c has value or true if the method is supported and the state has been set successfully
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_22912}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02206}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00179
    /// @trace_id_dd=DD_CRYPTO_01003
    /// @needwork = ad
    /// @endcode
    bool Seed(SecretSeed const& seed) noexcept override = 0;
#elif AP_VERSION_PUHUA == 30
    /// @brief Set the internal state of the RNG using the provided seed.
    /// @brief Set the internal state of the RNG using the provided seed
    /// @param seed  a memory region with the seed value
    /// @return @c has value or true if the method is supported and the state has been set successfully
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_22912}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02206}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00179
    /// @trace_id_dd=DD_CRYPTO_01003
    /// @needwork = ad
    /// @endcode
    ara::core::Result< void > Seed(SecretSeed const& seed) noexcept override = 0;
#endif
#if AP_VERSION_PUHUA == 20
    /// @brief Set the internal state of the RNG using the provided seed.   //???
    /// @brief Set the internal state of the RNG using the provided seed
    /// @param key  a SymmetricKey with the key used as seed value
    /// @return @c has value or true if the method is supported and the key has been set successfully
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_22913}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02001}
    /// @uptrace={RS_CRYPTO_02003}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00180
    /// @trace_id_dd=DD_CRYPTO_01004
    /// @needwork = ad
    /// @endcode
    bool SetKey(SymmetricKey const& key) noexcept override;
#elif AP_VERSION_PUHUA == 30
    /// @brief Set the internal state of the RNG using the provided seed.   //???
    /// @brief Set the internal state of the RNG using the provided seed
    /// @param key  a SymmetricKey with the key used as seed value
    /// @return @c has value or true if the method is supported and the key has been set successfully
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_22913}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02001}
    /// @uptrace={RS_CRYPTO_02003}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00180
    /// @trace_id_dd=DD_CRYPTO_01004
    /// @needwork = ad
    /// @endcode
    ara::core::Result< void > SetKey(SymmetricKey const& key) noexcept override;
#endif

public:  // PServiceExtension interface
    /// @brief Get the actual bit length of the key loaded into the context. If no key is set for the context, returns 0.
    /// @brief Get actual bit-length of a key loaded to the context. If no key was set to the context yet then 0 is
    /// returned.
    /// @returns actual length of a key (now set to the algorithm context) in bits
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_29045}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00181
    /// @trace_id_dd=DD_CRYPTO_01005
    /// @needwork = ad
    /// @endcode
    virtual std::size_t GetActualKeyBitLength() const noexcept;
    /// @brief Get the COUID of the key deployed to the context attached to this extended service. If no key is set for the context, returns an empty COUID (Nil).
    /// @brief Get the COUID of the key deployed to the context this extension service is attached to.
    /// If no key was set to the context yet then an empty COUID (Nil) is returned.
    /// @returns the COUID of the CryptoObject
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_29047}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00182
    /// @trace_id_dd=DD_CRYPTO_01006
    /// @needwork = ad
    /// @endcode
    virtual CryptoObjectUid GetActualKeyCOUID() const noexcept;
    /// @brief Get the allowed usage of this context (according to the key object properties loaded into this context).
    /// If the context has not been initialized with a key object, must return 0 (all flags reset).
    /// @brief Get allowed usages of this context (according to the key object attributes loaded to this context).
    /// If the context is not initialized by a key object yet then zero (all flags are reset) must be returned.
    /// @returns a combination of bit-flags that specifies allowed usages of the context
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_29046}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02008}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00183
    /// @trace_id_dd=DD_CRYPTO_01007
    /// @needwork = ad
    /// @endcode
    virtual AllowedUsageFlags GetAllowedUsage() const noexcept;
    /// @brief Get the maximum supported key length (in bits).
    /// @brief Get maximal supported key length in bits.
    /// @returns maximal supported length of the key in bits
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_29044}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00184
    /// @trace_id_dd=DD_CRYPTO_01008
    /// @needwork = ad
    /// @endcode
    virtual std::size_t GetMaxKeyBitLength() const noexcept = 0;
    /// @brief Get the minimum supported key length (in bits).
    /// @brief Get minimal supported key length in bits.
    /// @returns minimal supported length of the key in bits
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_29043}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00185
    /// @trace_id_dd=DD_CRYPTO_01009
    /// @needwork = ad
    /// @endcode
    virtual std::size_t GetMinKeyBitLength() const noexcept = 0;
    /// @brief Validate support for a specific key length according to the context.
    /// @brief Verify supportness of specific key length by the context.
    /// @param keyBitLength Key length in bits
    /// @returns @c true if provided value of the key length is supported by the context
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_29048}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00186
    /// @trace_id_dd=DD_CRYPTO_01010
    /// @needwork = ad
    /// @endcode
    virtual bool IsKeyBitLengthSupported(std::size_t keyBitLength) const noexcept;
    /// @brief Check whether a key is set for this context.
    /// @brief Check if a key has been set to this context.
    /// @return true if a key has been set to this context false otherwise
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_29049}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00187
    /// @trace_id_dd=DD_CRYPTO_01011
    /// @needwork = ad
    /// @endcode
    virtual bool IsKeyAvailable() const noexcept;

public:  // PCtxRng_Base interface
protected:
    /// @brief Whether RNG initialization is allowed.
    /// @param nAllowFlags A container type and constant bit-flags of allowed usages of a key or a secret seed object.
    /// @return true if can use init rng false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01012
    /// @needwork = dda
    /// @endcode
    static bool IsAllowedRngInit(AllowedUsageFlags const nAllowFlags) noexcept;
    /// @brief Determine whether the key is IPC.
    /// @return true if is ipc key false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01013
    /// @needwork = dda
    /// @endcode
    bool _IsIpcKey() const noexcept;
    /// @brief Get the IPC slot ID of the key.
    /// @return Slot ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01014
    /// @needwork = dda
    /// @endcode
    uint32_t _GetSlotID() const noexcept;
};
//********************************/
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYP_PUHUA_CTX_RNG_BASE_H_
