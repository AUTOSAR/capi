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
/// @file       random_generator_ctx.h
/// @brief      AutoSar-Crypto Encryption/Decryption module
/// @details    Random number generator context interface.
/// @date       2021-12-21
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <table> <td>2021-12-21 <td>1.0.0 <td>hanjingjing <td>Created initial version
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default Encryption/Decryption/Random Number Generation
/// @interface_level=software
/// @trace_id_sr=SR_CRYPTO_01017
/// @unit_name=RandomGeneratorCtx
/// @unit_description=Random Number Generator Context Base Class
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_RANDOM_GENERATOR_CTX_H_
#define ARA_CRYPTO_CRYP_RANDOM_GENERATOR_CTX_H_

#include "ara/core/result.h"
#include "ara/crypto/common/mem_region.h"
#include "ara/crypto/cryp/cryobj/secret_seed.h"
#include "ara/crypto/cryp/cryobj/symmetric_key.h"
#include "ara/crypto/cryp/crypto_context.h"
#include "ara/crypto/cryp/extension_service.h"

#define AP_VERSION_PUHUA 20
namespace ara {
namespace crypto {
namespace cryp {
//********************************/
/// @brief Random number generator context interface.
/// @brief Interface of Random Number Generator Context.
/// @interface RandomGeneratorCtx
/// @AUTOSAR_SWS {SWS_CRYPT_22900}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02206}
//********************************/
/// @code{.isoft}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02424
/// @trace_id_dd=DD_CRYPTO_05212
/// @needwork = ad
/// @endcode
class RandomGeneratorCtx : public CryptoContext
{
public:
    /// @brief Unique smart pointer for the interface.
    /// @brief Shared smart pointer of the interface.
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_22901}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02206}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_03208
    /// @trace_id_dd=DD_CRYPTO_06420
    /// @needwork = ad
    /// @endcode
    using Uptr = std::unique_ptr< RandomGeneratorCtx >;

public:
    /// @brief Constructor
    /// @code{.isoft}
    /// @interface_level=software
    /// @needwork = no
    /// @endcode
    RandomGeneratorCtx() noexcept = default;

    /// @brief Default destructor
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02425
    /// @trace_id_dd=DD_CRYPTO_05213
    /// @needwork = ad
    /// @endcode
    ~RandomGeneratorCtx() override = default;
    /// @brief Default copy assignment operator
    /// @brief Copy-assign another RandomGeneratorCtx to this instance.
    /// @param other Another instance of this class
    /// @return *this, containing the contents of @a other
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_30214}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02004}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02426
    /// @trace_id_dd=DD_CRYPTO_05214
    /// @needwork = ad
    /// @endcode
    RandomGeneratorCtx &operator=(RandomGeneratorCtx const &other) = delete;
    /// @brief Default move assignment operator
    /// @brief Move-assign another RandomGeneratorCtx to this instance.
    /// @param other Another instance of this class
    /// @return *this, containing the contents of @a other
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_30215}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02004}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02427
    /// @trace_id_dd=DD_CRYPTO_05215
    /// @needwork = ad
    /// @endcode
    RandomGeneratorCtx &operator=(RandomGeneratorCtx &&other) = delete;
    /// @brief Default copy constructor
    /// @param other Another instance of this class
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02428
    /// @trace_id_dd=DD_CRYPTO_05216
    /// @needwork = ad
    /// @endcode
    RandomGeneratorCtx(RandomGeneratorCtx const &other) noexcept = delete;
    /// @brief Default move constructor
    /// @param other Another instance of this class
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02429
    /// @trace_id_dd=DD_CRYPTO_05217
    /// @needwork = ad
    /// @endcode
    RandomGeneratorCtx(RandomGeneratorCtx &&other) noexcept = delete;

public:
    /// @brief Returns an allocated buffer containing a generated random sequence of the requested size.
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
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02431
    /// @trace_id_dd=DD_CRYPTO_05219
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< ara::core::Vector< ara::core::Byte > >
    /// @brief Return an allocated buffer with a generated random sequence of the requested size.
    /// @param count number of random bytes to generate
    /// @return @c a buffer filled with the generated random sequence
    Generate(std::uint32_t const &count) noexcept = 0;
    /// @brief Get the ExtensionService instance.
    /// @brief Get ExtensionService instance.
    /// @return ExtensionService instance
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_22902}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02006}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02432
    /// @trace_id_dd=DD_CRYPTO_05220
    /// @needwork = ad
    /// @endcode
    virtual ExtensionService::Uptr GetExtensionService() const noexcept = 0;
#if AP_VERSION_PUHUA == 20
    /// @brief Update the internal state of the RNG by mixing it with the provided additional entropy.
    /// This method is optional for implementation. Implementations of this method can "accumulate" entropy for future use.
    /// @brief Update the internal state of the RNG by mixing it with the provided additional entropy.
    ///         This method is optional for implementation. An implementation of this method may "accumulate" provided
    ///         entropy for future use.
    /// @param entropy  a memory region with the additional entropy value
    /// @return @c has value or true if the method is supported and the entropy has been updated successfully
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_22914}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02206}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02430
    /// @trace_id_dd=DD_CRYPTO_05218
    /// @needwork = ad
    /// @endcode
    virtual bool AddEntropy(ReadOnlyMemRegion const &entropy) noexcept = 0;
    /// @brief Set the internal state of the RNG using the provided seed.
    /// @brief Set the internal state of the RNG using the provided seed
    /// @param seed  a memory region with the seed value
    /// @return @c has value or true if the method is supported and the state has been set successfully
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_22911}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02206}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02433
    /// @trace_id_dd=DD_CRYPTO_05221
    /// @needwork = ad
    /// @endcode
    virtual bool Seed(ReadOnlyMemRegion const &seed) noexcept = 0;
    /// @brief Set the internal state of the RNG using the provided seed.
    /// @brief Set the internal state of the RNG using the provided seed
    /// @param seed  a memory region with the seed value
    /// @return @c has value or true if the method is supported and the state has been set successfully
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_22912}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02206}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02434
    /// @trace_id_dd=DD_CRYPTO_05222
    /// @needwork = ad
    /// @endcode
    virtual bool Seed(SecretSeed const &seed) noexcept = 0;
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
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02435
    /// @trace_id_dd=DD_CRYPTO_05223
    /// @needwork = ad
    /// @endcode
    virtual bool SetKey(SymmetricKey const &key) noexcept = 0;
#elif AP_VERSION_PUHUA == 30
    /// @brief Update the internal state of the RNG by mixing it with the provided additional entropy.
    /// This method is optional for implementation. Implementations of this method can "accumulate" entropy for future use.
    /// @brief Update the internal state of the RNG by mixing it with the provided additional entropy.
    ///         This method is optional for implementation. An implementation of this method may "accumulate" provided
    ///         entropy for future use.
    /// @param entropy  a memory region with the additional entropy value
    /// @return @c has value or true if the method is supported and the entropy has been updated successfully
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_22914}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02206}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02430
    /// @trace_id_dd=DD_CRYPTO_05218
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< void > AddEntropy(ReadOnlyMemRegion const &entropy) noexcept = 0;
    /// @brief Set the internal state of the RNG using the provided seed.
    /// @brief Set the internal state of the RNG using the provided seed
    /// @param seed  a memory region with the seed value
    /// @return @c has value or true if the method is supported and the state has been set successfully
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_22911}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02206}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02433
    /// @trace_id_dd=DD_CRYPTO_05221
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< void > Seed(ReadOnlyMemRegion const &seed) noexcept = 0;
    /// @brief Set the internal state of the RNG using the provided seed.
    /// @brief Set the internal state of the RNG using the provided seed
    /// @param seed  a memory region with the seed value
    /// @return @c has value or true if the method is supported and the state has been set successfully
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_22912}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02206}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02434
    /// @trace_id_dd=DD_CRYPTO_05222
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< void > Seed(SecretSeed const &seed) noexcept = 0;
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
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02435
    /// @trace_id_dd=DD_CRYPTO_05223
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< void > SetKey(SymmetricKey const &key) noexcept = 0;
#endif

public:
};
//********************************/
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYP_RANDOM_GENERATOR_CTX_H_
