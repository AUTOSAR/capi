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
/// @file       key_derivation_function_ctx.h
/// @brief      AutoSar-Crypto Encryption/Decryption module
/// @details    Key derivation function interface.
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
/// @module_path=/CRYPTO/Default Encryption/Decryption/Key Derivation
/// @interface_level=software
/// @trace_id_sr=SR_CRYPTO_01014
/// @unit_name=KeyDerivationFunctionCtx
/// @unit_description=Key Derivation Function Interface Base Class
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_KEY_DERIVATION_FUNCTION_CTX_H_
#define ARA_CRYPTO_CRYP_KEY_DERIVATION_FUNCTION_CTX_H_

#include "ara/core/result.h"
#include "ara/crypto/common/mem_region.h"
#include "ara/crypto/cryp/cryobj/secret_seed.h"
#include "ara/crypto/cryp/cryobj/symmetric_key.h"
#include "ara/crypto/cryp/crypto_context.h"
#include "ara/crypto/cryp/extension_service.h"

namespace ara {
namespace crypto {
namespace cryp {
//********************************/
/// @brief Key derivation function interface.
/// @brief Key Derivation Function interface.
/// @interface KeyDerivationFunctionCtx
/// @AUTOSAR_SWS {SWS_CRYPT_21500}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02103}
//********************************/
/// @code{.isoft}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02399
/// @trace_id_dd=DD_CRYPTO_05183
/// @needwork = ad
/// @endcode
class KeyDerivationFunctionCtx : public CryptoContext
{
public:
    /// @brief Unique smart pointer for the interface.
    /// @brief Unique smart pointer of the interface.
    /// @code{.isoft}
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_21501}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02103}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_03206
    /// @trace_id_dd=DD_CRYPTO_06418
    /// @needwork = ad
    /// @endcode
    using Uptr = std::unique_ptr< KeyDerivationFunctionCtx >;
    /// @brief Constructor
    /// @code{.isoft}
    /// @interface_level=software
    /// @needwork = no
    /// @endcode
    KeyDerivationFunctionCtx() noexcept = default;
    /// @brief Default destructor
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02400
    /// @trace_id_dd=DD_CRYPTO_05184
    /// @needwork = ad
    /// @endcode
    ~KeyDerivationFunctionCtx() override = default;
    /// @brief Default copy assignment operator
    /// @brief Copy-assign another KeyDerivationFunctionCtx to this instance.
    /// @param other Another instance of this class
    /// @return *this, containing the contents of @a other
    /// @code{.isoft}
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_30214}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02004}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02401
    /// @trace_id_dd=DD_CRYPTO_05185
    /// @needwork = ad
    /// @endcode
    KeyDerivationFunctionCtx &operator=(KeyDerivationFunctionCtx const &other) = delete;
    /// @brief Default move assignment operator
    /// @brief Move-assign another KeyDerivationFunctionCtx to this instance.
    /// @param other Another instance of this class
    /// @return *this, containing the contents of @a other
    /// @code{.isoft}
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_30215}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02004}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02402
    /// @trace_id_dd=DD_CRYPTO_05186
    /// @needwork = ad
    /// @endcode
    KeyDerivationFunctionCtx &operator=(KeyDerivationFunctionCtx &&other) = delete;
    /// @brief Default copy constructor
    /// @param other Another instance of this class
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02403
    /// @trace_id_dd=DD_CRYPTO_05187
    /// @needwork = ad
    /// @endcode
    KeyDerivationFunctionCtx(KeyDerivationFunctionCtx const &other) noexcept = delete;
    /// @brief Default move constructor
    /// @param other Another instance of this class
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02404
    /// @trace_id_dd=DD_CRYPTO_05188
    /// @needwork = ad
    /// @endcode
    KeyDerivationFunctionCtx(KeyDerivationFunctionCtx &&other) noexcept = delete;

public:
    /// @brief Add an application padding value stored in a (non-secret) ReadOnlyMemRegion.
    /// @brief Add an application filler value stored in a (non-secret) ReadOnlyMemRegion. If (GetFillerSize() == 0),
    /// then this method call will be ignored.
    ///			Add a secret application filler value stored in a SecretSeed object. If (GetFillerSize() == 0),
    /// then this method call will be ignored. 			Add a salt value stored in a (non-secret)
    /// ReadOnlyMemRegion.
    /// @param salt  a salt value (if used, it should be unique for each instance of the target key)
    /// @return has value if AddSalt sucess false otherwise
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kInvalidInputSize  if size of the appFiller is incorrect, i.e. if (appFiller.size()
    /// < GetFillerSize());
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_21510}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02102}
    /// @uptrace={RS_CRYPTO_02107}
    /// @uptrace={RS_CRYPTO_02108}
    /// @uptrace={RS_CRYPTO_02111}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02405
    /// @trace_id_dd=DD_CRYPTO_05189
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< void > AddSalt(ReadOnlyMemRegion const &salt) noexcept = 0;
    /// @brief Add a secret salt value stored in a SecretSeed object.
    /// @brief Add a secret salt value stored in a SecretSeed object.
    /// @param salt  a salt value (if used, it should be unique for each instance of the target key)
    /// @return
    /// @code{.isoft}
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_21513}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02102}
    /// @uptrace={RS_CRYPTO_02107}
    /// @uptrace={RS_CRYPTO_02108}
    /// @uptrace={RS_CRYPTO_02111}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02406
    /// @trace_id_dd=DD_CRYPTO_05190
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< void > AddSecretSalt(SecretSeed const &salt) noexcept = 0;
    /// @brief Configure the number of iterations applied by default. The implementation may limit the minimum and/or maximum number of iterations.
    /// @brief Configure the number of iterations that will be applied by default.
    ///			Implementation can restrict minimal and/or maximal value of the iterations number.
    /// @param iterations  the required number of iterations of the base function (0 means implementation default number)
    /// @return actual number of the iterations configured in the context now (after this method call)
    /// @code{.isoft}
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_21514}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02407
    /// @trace_id_dd=DD_CRYPTO_05191
    /// @needwork = ad
    /// @endcode
    virtual std::uint32_t ConfigIterations(std::uint32_t iterations = 0U) noexcept = 0;
    /// @brief Derive a symmetric key from the provided key material and the provided context configuration.
    /// @brief Derive a symmetric key from the provided key material and provided context configuration.
    /// @param isSession  the "session" (or "temporary") attribute for the target key (if @c true)
    /// @param isExportable  the exportability attribute for the target key (if @c true)
    /// @return unique smart pointer to the created instance of derived symmetric key
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kUninitializedContext	if the context was not sufficiently initialized
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_21515}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02102}
    /// @uptrace={RS_CRYPTO_02107}
    /// @uptrace={RS_CRYPTO_02108}
    /// @uptrace={RS_CRYPTO_02111}
    /// @uptrace={RS_CRYPTO_02115}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02408
    /// @trace_id_dd=DD_CRYPTO_05192
    /// @needwork = ad
    /// @endcode
    // PRQA S 2024 QAC /// @qac: False positive
    virtual ara::core::Result< SymmetricKey::Uptrc > DeriveKey(bool isSession    = true,
                                                               bool isExportable = false) const noexcept = 0;
    // PRQA L:QAC
    /// @brief Derive a "sub" key material (secret seed) from the provided "master" key material and the provided context configuration.
    /// @brief Derive a "slave" key material (secret seed) from the provided "master" key material and provided context
    /// configuration.
    /// @param isSession  the "session" (or "temporary") attribute for the target key (if @c true)
    /// @param isExportable  the exportability attribute for the target key (if @c true)
    /// @return unique smart pointer to the created @c SecretSeed object
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kUninitializedContext	if the context was not sufficiently initialized
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_21516}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02007}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02409
    /// @trace_id_dd=DD_CRYPTO_05193
    /// @needwork = ad
    /// @endcode
    // PRQA S 2024 QAC /// @qac: False positive
    virtual ara::core::Result< SecretSeed::Uptrc > DeriveSeed(bool isSession    = true,
                                                              bool isExportable = false) const noexcept = 0;
    // PRQA L:QAC
    /// @brief Clear the encryption context.
    /// @brief Clear the crypto context.
    /// @return
    /// @code{.isoft}
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_21524}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02108}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02410
    /// @trace_id_dd=DD_CRYPTO_05194
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< void > Reset() noexcept = 0;
    /// @brief Get the ExtensionService instance.
    /// @brief Get ExtensionService instance.
    /// @return
    /// @code{.isoft}
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_21517}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02006}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02411
    /// @trace_id_dd=DD_CRYPTO_05195
    /// @needwork = ad
    /// @endcode
    virtual ExtensionService::Uptr GetExtensionService() const noexcept = 0;
    /// @brief Get the fixed size of the application-specific "filler" required for this context instance.
    /// If this instance of the key derivation context does not support padding values, 0 should be returned.
    /// Get the fixed size of the target key ID required by the diversification algorithm. The return value is constant for each interface instance, i.e., independent of configuration.
    /// @brief Get the fixed size of an application specific "filler" required by this context instance.  If this
    /// instance of the key derivation context does not support filler values, 0 shall be returned.
    ///	Get the fixed size of the target key ID required by diversification algorithm. Returned value is
    /// constant for each instance of the interface, i.e. independent from configuration by
    /// @return size of the application specific filler in bytes Returned value is constant for this instance of the
    /// key derivation context, i.e. independent from configuration by the @c Init() call. size of the key ID in bytes
    /// @code{.isoft}
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_21518}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02103}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02412
    /// @trace_id_dd=DD_CRYPTO_05196
    /// @needwork = ad
    /// @endcode
    virtual std::size_t GetKeyIdSize() const noexcept = 0;
    /// @brief Get the symmetric algorithm ID of the target (sub) key. If the context has not been configured by calling the Init() method, kAlgIdUndefined should be returned.
    /// @brief Get the symmetric algorithm ID of target (slave) key.
    ///	If the context was not configured yet by a call of the @c Init() method then @c kAlgIdUndefined
    /// should be.
    /// @return the symmetric algorithm ID of the target key, configured by the last call of the @c Init() method
    /// returned.
    /// @code{.isoft}
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_21520}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02103}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02413
    /// @trace_id_dd=DD_CRYPTO_05197
    /// @needwork = ad
    /// @endcode
    virtual AlgId GetTargetAlgId() const noexcept = 0;
    /// @brief Get the allowed usage key of the target (sub) key.
    /// @return allowed key usage bit-flags of target keys
    /// @code{.isoft}
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_21521}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02008}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02414
    /// @trace_id_dd=DD_CRYPTO_05198
    /// @needwork = ad
    /// @endcode
    virtual AllowedUsageFlags GetTarGetAllowedUsage() const noexcept = 0;
    /// @brief Get the bit length of the target (diversified) key. The return value is configured by the context factory method, i.e., independent of configuration.
    /// @brief Get the bit-length of target (diversified) keys.
    ///	Returned value is configured by the context factory method, i.e. independent from configuration by
    /// @return the length of target (diversified) key in bits the @c Init() calls.
    /// @code{.isoft}
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_21522}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02103}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02415
    /// @trace_id_dd=DD_CRYPTO_05199
    /// @needwork = ad
    /// @endcode
    virtual std::size_t GetTargetKeyBitLength() const noexcept = 0;
    /// @brief Initialize this context by at least setting the target key ID.
    /// @param targetKeyId  ID of the target key
    /// @param targetAlgId  the identifier of the target symmetric crypto algorithm
    /// // Identifier of the target symmetric encryption algorithm
    /// @param allowedUsage bit-flags that define a list of allowed transformations' types in which the target key
    /// may be used
    /// @param ctxLabel	an optional application specific "context label" (this can identify the purpose
    /// of the target key and/or communication parties)
    /// Optional application-specific "context tag" (can identify the purpose of the target key and/or communication parties)
    /// @return has value if Init sucess false otherwise
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kIncompatibleArguments	if @c targetAlgId specifies a cryptographic algorithm
    /// different from a symmetric one with key length equal to @c GetTargetKeyBitLength();
    /// @error: SecurityErrorDomain::kUsageViolation			if @c allowedUsage specifies more usages of the
    /// derived key-material than the source key-material,
    ///	i.e. usage of the derived key-material may not be expanded beyond what
    /// the source key-material allows
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_21523}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02102}
    /// @uptrace={RS_CRYPTO_02107}
    /// @uptrace={RS_CRYPTO_02108}
    /// @uptrace={RS_CRYPTO_02111}
    /// @uptrace={RS_CRYPTO_02115}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02416
    /// @trace_id_dd=DD_CRYPTO_05200
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< void > Init(ReadOnlyMemRegion const &targetKeyId,
                                           AlgId targetAlgId                 = kAlgIdAny,
                                           AllowedUsageFlags allowedUsage    = kAllowKdfMaterialAnyUsage,
                                           ReadOnlyMemRegion const &ctxLabel = ReadOnlyMemRegion()) noexcept = 0;
    /// @brief Set (deploy) key-material to the key-derivation algorithm context.
    /// @brief Set (deploy) key-material to the key derivation algorithm context.
    /// @param sourceKM  the source key-material
    /// @return  has value if SetSourceKeyMaterial sucess false otherwise
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kIncompatibleObject		if the provided key object is incompatible with
    /// this symmetric key context
    /// @error: SecurityErrorDomain::kUsageViolation			if deriving a key is prohibited by the "allowed
    /// usage" restrictions of the provided source key-material
    /// @error: SecurityErrorDomain::kBruteForceRisk			if key length of the @c sourceKm is below of an
    /// internally defined limitation
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_21525}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02001}
    /// @uptrace={RS_CRYPTO_02003}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02417
    /// @trace_id_dd=DD_CRYPTO_05201
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< void > SetSourceKeyMaterial(RestrictedUseObject const &sourceKM) noexcept = 0;

public:
};
//********************************/
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYP_KEY_DERIVATION_FUNCTION_CTX_H_
