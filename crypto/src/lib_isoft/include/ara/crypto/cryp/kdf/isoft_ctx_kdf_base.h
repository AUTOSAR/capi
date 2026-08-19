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
/// @file       isoft_ctx_kdf_base.h
/// @brief      AutoSar-Crypto encryption and decryption module
/// @details    Key derivation function interface.
/// @date       2022-03-15
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default Encryption and Decryption/Key Derivation
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_01014
/// @unit_name=PCtxKdf_Base
/// @unit_description=Key derivation context base class
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_PUHUA_CTX_KDF_BASE_H_
#define ARA_CRYPTO_CRYP_PUHUA_CTX_KDF_BASE_H_

#include "ara/crypto/common/isoft_data_type.h"
#include "ara/crypto/cryp/cryobj/restricted_use_object.h"
#include "ara/crypto/cryp/isoft_auto_buff.h"
#include "ara/crypto/cryp/key_derivation_function_ctx.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
/// @brief Encryption provider
class PCryptoProvider;
//********************************/
/// @brief Key derivation function interface: OpenSSL wrapper.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00192
/// @trace_id_dd=DD_CRYPTO_01058
/// @needwork = ad
/// @endcode
class PCtxKdf_Base : public KeyDerivationFunctionCtx
{
private:
    /// @brief Encryption provider object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01059
    /// @needwork = dda
    /// @endcode
    PCryptoProvider& cryptoProvider_;
    /// @brief Key material
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01060
    /// @needwork = dda
    /// @endcode
    RestrictedUseObject const* pSrcKeyMaterial_;
    /// @brief Cryptographic primitive ID of the target
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01061
    /// @needwork = dda
    /// @endcode
    AlgId nTargetAlgId_;
    /// @brief Allowed usage
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01062
    /// @needwork = dda
    /// @endcode
    AllowedUsageFlags nTargetAllowedUsage_;
    /// @brief Tag //
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01063
    /// @needwork = dda
    /// @endcode
    ReadOnlyMemRegion ctxLabel_;
    /// @brief Whether initialized
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01064
    /// @needwork = dda
    /// @endcode
    bool bInit_;
    /// @brief Number of iterations
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01065
    /// @needwork = dda
    /// @endcode
    uint32_t nIterations_;
    /// @brief Key slot cache
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01066
    /// @needwork = dda
    /// @endcode
    internal::PAutoBuff buffSalt_;
    /// @brief Target key ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01067
    /// @needwork = dda
    /// @endcode
    AlgId targetKeyId_;
    /// @brief Parameterized constructor
    /// @param cryptoProvider encryption provider
    /// @param pSrcKeyMaterial source key material data
    /// @param nTargetAlgId target algorithm cryptographic primitive ID
    /// @param nTarGetAllowedUsage target allowed usage
    /// @param ctxLabel context label
    /// @param bInit whether initialized
    /// @param nIterations number of iterations
    /// @param buffSalt salt buffer data
    /// @param targetKeyId target key ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01068
    /// @needwork = dda
    /// @endcode
    PCtxKdf_Base(PCryptoProvider& cryptoProvider,
                 RestrictedUseObject const* const pSrcKeyMaterial,
                 AlgId const nTargetAlgId,
                 AllowedUsageFlags const nTarGetAllowedUsage,
                 ReadOnlyMemRegion const& ctxLabel,
                 bool const bInit,
                 uint32_t const nIterations,
                 internal::PAutoBuff const* const buffSalt,
                 AlgId const targetKeyId) noexcept;

public:
    /// @brief Parameterized constructor
    /// @param cryptoProvider PCryptoProvider
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00193
    /// @trace_id_dd=DD_CRYPTO_01069
    /// @needwork = ad
    /// @endcode
    explicit PCtxKdf_Base(PCryptoProvider& cryptoProvider) noexcept;
    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00194
    /// @trace_id_dd=DD_CRYPTO_01070
    /// @needwork = ad
    /// @endcode
    PCtxKdf_Base() = delete;
    /// @brief Default virtual destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00195
    /// @trace_id_dd=DD_CRYPTO_01071
    /// @needwork = ad
    /// @endcode
    ~PCtxKdf_Base() noexcept override = default;
    /// @brief Default move constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00196
    /// @trace_id_dd=DD_CRYPTO_01072
    /// @needwork = ad
    /// @endcode
    PCtxKdf_Base(PCtxKdf_Base&& other) = delete;
    /// @brief Default move assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00197
    /// @trace_id_dd=DD_CRYPTO_01073
    /// @needwork = ad
    /// @endcode
    PCtxKdf_Base& operator=(PCtxKdf_Base&& other) = delete;
    /// @brief Default copy assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00198
    /// @trace_id_dd=DD_CRYPTO_01074
    /// @needwork = ad
    /// @endcode
    PCtxKdf_Base& operator=(PCtxKdf_Base const& other) = delete;
    /// @brief Default copy constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00199
    /// @trace_id_dd=DD_CRYPTO_01075
    /// @needwork = ad
    /// @endcode
    PCtxKdf_Base(PCtxKdf_Base const& other) = delete;

public:  // CryptoContext interface
    /// @brief Return the CryptoPrimitivId instance containing the instance identifier.
    /// @name   GetCryptoPrimitiveId
    /// @returns CryptoPrimitivId instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00200
    /// @trace_id_dd=DD_CRYPTO_01076
    /// @needwork = ad
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override = 0;
    /// @brief Check whether the encryption context has been initialized and is usable. It checks all required values, including: key value, IV/seed, etc.
    /// @name   IsInitialized
    /// @returns true if already init false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00201
    /// @trace_id_dd=DD_CRYPTO_01077
    /// @needwork = ad
    /// @endcode
    inline bool IsInitialized() const noexcept override { return bInit_; }
    /// @brief Get a reference to the Crypto Provider for this context.
    /// @name   MyProvider
    /// @returns reference to the encryption provider
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00202
    /// @trace_id_dd=DD_CRYPTO_01078
    /// @needwork = ad
    /// @endcode
    CryptoProvider& MyProvider() const noexcept override;

public:  // KeyDerivationFunctionCtx interface
    /// @brief Add an application-specific filler value stored in a (non-secret) ReadOnlyMemRegion.
    /// @brief Add an application filler value stored in a (non-secret) ReadOnlyMemRegion. If (GetFillerSize() == 0),
    ///     then this method call will be ignored.
    ///     Add a secret application filler value stored in a SecretSeed object. If (GetFillerSize() == 0), then this
    ///     method call will be ignored. Add a salt value stored in a (non-secret) ReadOnlyMemRegion.
    /// @name AddSalt
    /// @param salt  a salt value (if used, it should be unique for each instance of the target key)
    /// @returns has vlaue if AddSalt sucess false otherwise
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kInvalidInputSize  if size of the appFiller is incorrect, i.e. if (appFiller.size()
    ///     < GetFillerSize());
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_21510}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02102}
    /// @uptrace={RS_CRYPTO_02107}
    /// @uptrace={RS_CRYPTO_02108}
    /// @uptrace={RS_CRYPTO_02111}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00203
    /// @trace_id_dd=DD_CRYPTO_01079
    /// @needwork = ad
    /// @endcode
    ara::core::Result< void > AddSalt(ReadOnlyMemRegion const& salt) noexcept override;
    /// @brief Add a secret salt value stored in a SecretSeed object.
    /// @brief Add a secret salt value stored in a SecretSeed object.
    /// @name  AddSecretSalt
    /// @param salt  a salt value (if used, it should be unique for each instance of the target key)
    /// @return
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_21513}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02102}
    /// @uptrace={RS_CRYPTO_02107}
    /// @uptrace={RS_CRYPTO_02108}
    /// @uptrace={RS_CRYPTO_02111}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00204
    /// @trace_id_dd=DD_CRYPTO_01080
    /// @needwork = ad
    /// @endcode
    ara::core::Result< void > AddSecretSalt(SecretSeed const& salt) noexcept override;
    /// @brief Configure the number of iterations applied by default. The implementation may limit the minimum and/or maximum number of iterations.
    /// @brief Configure the number of iterations that will be applied by default.
    ///     Implementation can restrict minimal and/or maximal value of the iterations number.
    /// @name  ConfigIterations
    /// @param iterations  the required number of iterations of the base function
    /// @returns actual number of the iterations configured in the context now (after this method call)
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_21514}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00205
    /// @trace_id_dd=DD_CRYPTO_01081
    /// @needwork = ad
    /// @endcode
    std::uint32_t ConfigIterations(std::uint32_t iterations = 0U) noexcept override;
    /// @brief Derive a symmetric key from the provided key material and the provided context configuration.
    /// @brief Derive a symmetric key from the provided key material and provided context configuration.
    /// @param isSession  the "session" (or "temporary") attribute for the target key (if @c true)
    /// @param isExportable  the exportability attribute for the target key (if @c true)
    /// @returns unique smart pointer to the created instance of derived symmetric key
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kUninitializedContext	if the context was not sufficiently initialized
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_21515}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02102}
    /// @uptrace={RS_CRYPTO_02107}
    /// @uptrace={RS_CRYPTO_02108}
    /// @uptrace={RS_CRYPTO_02111}
    /// @uptrace={RS_CRYPTO_02115}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00206
    /// @trace_id_dd=DD_CRYPTO_01082
    /// @needwork = ad
    /// @endcode
    ara::core::Result< SymmetricKey::Uptrc > DeriveKey(bool isSession    = true,
                                                       bool isExportable = false) const noexcept override;
    /// @brief Derive a "slave" key material (secret seed) from the provided "master" key material and the provided context configuration.
    /// @brief Derive a "slave" key material (secret seed) from the provided "master" key material and provided context
    /// configuration.
    /// @param isSession  the "session" (or "temporary") attribute for the target key (if @c true)
    /// @param isExportable  the exportability attribute for the target key (if @c true)
    /// @returns unique smart pointer to the created @c SecretSeed object
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kUninitializedContext	if the context was not sufficiently initialized
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_21516}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02007}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00207
    /// @trace_id_dd=DD_CRYPTO_01083
    /// @needwork = ad
    /// @endcode
    ara::core::Result< SecretSeed::Uptrc > DeriveSeed(bool isSession    = true,
                                                      bool isExportable = false) const noexcept override;
    /// @brief Clear the encryption context.
    /// @brief Clear the crypto context.
    /// @name  Reset
    /// @returns
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_21524}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02108}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00208
    /// @trace_id_dd=DD_CRYPTO_01084
    /// @needwork = ad
    /// @endcode
    ara::core::Result< void > Reset() noexcept override;
    /// @brief Get the ExtensionService instance.
    /// @brief Get ExtensionService instance.
    /// @name  GetExtensionService
    /// @returns
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_21517}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02006}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00209
    /// @trace_id_dd=DD_CRYPTO_01085
    /// @needwork = ad
    /// @endcode
    ExtensionService::Uptr GetExtensionService() const noexcept override = 0;
    /// @brief Get the fixed size of the application-specific "filler" required by this context instance.
    /// @brief Get the fixed size of an application specific "filler" required by this context instance.
    /// @returns fixed size of the "filler"
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_21518}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02103}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00210
    /// @trace_id_dd=DD_CRYPTO_01086
    /// @needwork = ad
    /// @endcode
    std::size_t GetKeyIdSize() const noexcept override = 0;
    /// @brief Get the symmetric algorithm ID of the target (slave) key. If the context has not been configured by the Init() method, kAlgIdUndefined should be returned.
    /// @brief Get the symmetric algorithm ID of target (slave) key.
    /// @returns the symmetric algorithm ID of the target key, configured by the last call of the @c Init() method
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_21520}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02103}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00211
    /// @trace_id_dd=DD_CRYPTO_01087
    /// @needwork = ad
    /// @endcode
    AlgId GetTargetAlgId() const noexcept override;
    /// @brief Get the allowed usage key of the target (slave) key.
    /// @returns allowed key usage bit-flags of target keys
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_21521}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02008}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00212
    /// @trace_id_dd=DD_CRYPTO_01088
    /// @needwork = ad
    /// @endcode
    AllowedUsageFlags GetTarGetAllowedUsage() const noexcept override;
    /// @brief Get the bit length of the target (diversified) key. The return value is configured by the context factory method, i.e., independent of configuration.
    /// @brief Get the bit-length of target (diversified) keys.
    ///     Returned value is configured by the context factory method, i.e. independent from configuration by
    /// @returns the length of target (diversified) key in bits the @c Init() calls.
    /// @name GetTargetKeyBitLength
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_21522}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02103}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00213
    /// @trace_id_dd=DD_CRYPTO_01089
    /// @needwork = ad
    /// @endcode
    std::size_t GetTargetKeyBitLength() const noexcept override = 0;
    /// @brief Initialize this context by at least setting the target key ID.
    /// @name  Init
    /// @param targetKeyId  ID of the target key
    /// @param targetAlgId  the identifier of the target symmetric crypto algorithm
    /// @param allowedUsage bit-flags that define a list of allowed transformations' types in which the target key
    /// may be used
    /// @param ctxLabel  an optional application specific "context label" (this can identify the purpose of the
    /// target key and/or communication parties)
    /// @returns has vlaue if Init sucess false otherwise
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kIncompatibleArguments	if @c targetAlgId specifies a cryptographic algorithm
    ///     different from a symmetric one with key length equal to @c GetTargetKeyBitLength();
    /// @error: SecurityErrorDomain::kUsageViolation        if @c allowedUsage specifies more usages of the derived
    ///     key-material than the source key-material, i.e. usage of the derived key-material may not be expanded beyond
    ///     what the source key-material allows
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_21523}
    /// @uptrace={RS_CRYPTO_02102}
    /// @uptrace={RS_CRYPTO_02107}
    /// @uptrace={RS_CRYPTO_02108}
    /// @uptrace={RS_CRYPTO_02111}
    /// @uptrace={RS_CRYPTO_02115}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00214
    /// @trace_id_dd=DD_CRYPTO_01090
    /// @needwork = ad
    /// @endcode
    ara::core::Result< void > Init(ReadOnlyMemRegion const& targetKeyId,
                                   AlgId targetAlgId                 = kAlgIdAny,
                                   AllowedUsageFlags allowedUsage    = kAllowKdfMaterialAnyUsage,
                                   ReadOnlyMemRegion const& ctxLabel = ReadOnlyMemRegion()) noexcept override;
    /// @brief Set (deploy) key material to the key derivation algorithm context.
    /// @brief Set (deploy) key-material to the key derivation algorithm context.
    /// @param sourceKM  the source key-material
    /// @returns has vlaue if SetSourceKeyMaterial sucess false otherwise
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kIncompatibleObject    if the provided key object is incompatible with this
    ///     symmetric key context
    /// @error: SecurityErrorDomain::kUsageViolation        if deriving a key is prohibited by the "allowed
    ///     usage" restrictions of the provided source key-material
    /// @error: SecurityErrorDomain::kBruteForceRisk        if key length of the @c sourceKm is below of an
    ///     internally defined limitation
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_21525}
    /// @uptrace={RS_CRYPTO_02001}
    /// @uptrace={RS_CRYPTO_02003}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00215
    /// @trace_id_dd=DD_CRYPTO_01091
    /// @needwork = ad
    /// @endcode
    ara::core::Result< void > SetSourceKeyMaterial(RestrictedUseObject const& sourceKM) noexcept override;

protected:  // PCtxKdf_Base interface
    /// @brief: Check whether the given key material meets requirements.
    /// @name   ChecRestrictedUseObject
    /// @param sourceKM the source key-material
    /// @returns  true if stafiy false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01092
    /// @needwork = dda
    /// @endcode
    virtual bool ChecRestrictedUseObject(RestrictedUseObject const& sourceKM) const noexcept = 0;
    /// @brief Check whether the length of the input raw key is satisfied.
    /// @name   CheckSourceKeyLen
    /// @param nKeyLen Key length
    /// @returns true if keylen is support false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01093
    /// @needwork = dda
    /// @endcode
    virtual bool CheckSourceKeyLen(uint32_t nKeyLen) const noexcept = 0;

public:  // PServiceExtension interface
    /// @brief Get the actual bit length of the key loaded into the context. If no key is set for the context, returns 0.
    /// @brief Get actual bit-length of a key loaded to the context. If no key was set to the context yet then 0 is
    ///     returned.
    /// @name GetActualKeyBitLength
    /// @returns actual length of a key (now set to the algorithm context) in bits
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_29045}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00216
    /// @trace_id_dd=DD_CRYPTO_01094
    /// @needwork = ad
    /// @endcode
    virtual std::size_t GetActualKeyBitLength() const noexcept;
    /// @brief Get the COUID of the key deployed to the context attached to this extended service. If no key is set for the context, returns an empty COUID (Nil).
    /// @brief Get the COUID of the key deployed to the context this extension service is attached to.
    ///     If no key was set to the context yet then an empty COUID (Nil) is returned.
    /// @name GetActualKeyCOUID
    /// @returns the COUID of the CryptoObject
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_29047}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00217
    /// @trace_id_dd=DD_CRYPTO_01095
    /// @needwork = ad
    /// @endcode
    virtual CryptoObjectUid GetActualKeyCOUID() const noexcept;
    /// @brief Get the allowed usage of this context (according to the key object properties loaded into this context).
    /// If the context has not been initialized with a key object, must return 0 (all flags reset).
    /// @brief Get allowed usages of this context (according to the key object attributes loaded to this context).
    ///     If the context is not initialized by a key object yet then zero (all flags are reset) must be returned.
    /// @name GetAllowedUsage
    /// @returns a combination of bit-flags that specifies allowed usages of the context
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_29046}
    /// @uptrace={RS_CRYPTO_02008}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00218
    /// @trace_id_dd=DD_CRYPTO_01096
    /// @needwork = ad
    /// @endcode
    virtual AllowedUsageFlags GetAllowedUsage() const noexcept;
    /// @brief Get the maximum supported key length (in bits).
    /// @brief Get maximal supported key length in bits.
    /// @name  GetMaxKeyBitLength
    /// @returns maximal supported length of the key in bits
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_29044}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00219
    /// @trace_id_dd=DD_CRYPTO_01097
    /// @needwork = ad
    /// @endcode
    virtual std::size_t GetMaxKeyBitLength() const noexcept;
    /// @brief Get the minimum supported key length (in bits).
    /// @brief Get minimal supported key length in bits.
    /// @name  GetMinKeyBitLength
    /// @returns minimal supported length of the key in bits
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_29043}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00220
    /// @trace_id_dd=DD_CRYPTO_01098
    /// @needwork = ad
    /// @endcode
    virtual std::size_t GetMinKeyBitLength() const noexcept;
    /// @brief Validate support for a specific key length according to the context.
    /// @brief Verify supportness of specific key length by the context.
    /// @name  IsKeyBitLengthSupported
    /// @param keyBitLength Key length in bits
    /// @returns @c true if provided value of the key length is supported by the context
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_29048}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00221
    /// @trace_id_dd=DD_CRYPTO_01099
    /// @needwork = ad
    /// @endcode
    virtual bool IsKeyBitLengthSupported(std::size_t keyBitLength) const noexcept;
    /// @brief Check whether a key is set for this context.
    /// @brief Check if a key has been set to this context.
    /// @name  IsKeyAvailable
    /// @returns true if a key has been set to this context false otherwise
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_29049}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00222
    /// @trace_id_dd=DD_CRYPTO_01100
    /// @needwork = ad
    /// @endcode
    virtual bool IsKeyAvailable() const noexcept;

protected:
    /// @brief Function to export the secret key.
    /// @param buffReturn Result buffer
    /// @param pSrcData Source data
    /// @param nSrcLen Source data length
    /// @param pSalt Salt data
    /// @param nSaltLen Salt length
    /// @param nHashId Hash algorithm crypto primitive ID
    /// @param nKeyLen Key length
    /// @param nIterations Number of iterations
    /// @return ture if derive key sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01101
    /// @needwork = dda
    /// @endcode
    bool _Derive_Key(internal::PAutoBuff& buffReturn,
                     uint8_t const* const pSrcData,
                     uint32_t const nSrcLen,
                     uint8_t const* const pSalt,
                     uint32_t nSaltLen,
                     AlgId const nHashId,
                     uint32_t const nKeyLen,
                     uint32_t nIterations) const noexcept;

private:
    /// @brief To reduce the complexity of the _Derive_Key function, encapsulate part of the internal code into a function.
    /// @param nIterationIndex Number of iterations for key derivation
    /// @param buffKeyData Key data buffer
    /// @param pSrcData Source data for derivation
    /// @param nSrcLen Length of source data for derivation
    /// @param nSaltLen Length of salt
    /// @param pSalt Salt value
    /// @param nKeyLen Key length
    /// @param nHashId Hash algorithm ID
    /// @return Whether buffKeyData was successfully obtained
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_
    /// @trace_id_dd=DD_CRYPTO_
    /// @trace_id_sr=SR_CRYPTO_
    /// @unit_name=
    /// @needwork = dd
    /// @endcode
    bool _getBuffKeyData(uint32_t const nIterationIndex,
                         internal::PAutoBuff& buffKeyData,
                         uint8_t const* const pSrcData,
                         uint32_t const nSrcLen,
                         uint32_t const nSaltLen,
                         uint8_t const* const pSalt,
                         uint32_t const nKeyLen,
                         AlgId const nHashId) const noexcept;

public:
};
//********************************/
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYP_PUHUA_CTX_KDF_BASE_H_
