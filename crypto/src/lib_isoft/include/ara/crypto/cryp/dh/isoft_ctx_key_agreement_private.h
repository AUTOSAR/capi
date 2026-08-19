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
/// @file       isoft_ctx_key_agreement_private.h
/// @brief      AutoSar-Crypto encryption/decryption module
/// @details    Key exchange.
/// @date       2022-05-05
/// @author     Chang Zheng
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr><td>2022-04-26  </td>1.0.0    </td>Chang Zheng     <table>Create initial version</td>
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default encryption/Key exchange
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_01014
/// @unit_name=PCtxKeyAgreementPrivate
/// @unit_description=Key exchange
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_PUHUA_CTX_KEY_AGREEMENT_PEIVATE_H_
#define ARA_CRYPTO_CRYP_PUHUA_CTX_KEY_AGREEMENT_PEIVATE_H_

#include "ara/crypto/cryp/cryobj/isoft_key_private_ecc.h"
#include "ara/crypto/cryp/key_agreement_private_ctx.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {

/// @brief Crypto provider
class PCryptoProvider;
/// @brief Key exchange.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00223
/// @trace_id_dd=DD_CRYPTO_01102
/// @needwork = ad
/// @endcode
class PCtxKeyAgreementPrivate : public KeyAgreementPrivateCtx
{
public:
    /// @brief Parameterized constructor
    /// @param cryptoProvider PCryptoProvider
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00224
    /// @trace_id_dd=DD_CRYPTO_01103
    /// @needwork = ad
    /// @endcode
    explicit PCtxKeyAgreementPrivate(PCryptoProvider &cryptoProvider) noexcept;
    /// @brief default constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00225
    /// @trace_id_dd=DD_CRYPTO_01104
    /// @needwork = ad
    /// @endcode
    PCtxKeyAgreementPrivate() noexcept = delete;
    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00226
    /// @trace_id_dd=DD_CRYPTO_01105
    /// @needwork = ad
    /// @endcode
    ~PCtxKeyAgreementPrivate() noexcept override = default;
    /// @brief default copy assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00227
    /// @trace_id_dd=DD_CRYPTO_01106
    /// @needwork = ad
    /// @endcode
    PCtxKeyAgreementPrivate &operator=(PCtxKeyAgreementPrivate const &other) noexcept = delete;
    /// @brief default move assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00228
    /// @trace_id_dd=DD_CRYPTO_01107
    /// @needwork = ad
    /// @endcode
    PCtxKeyAgreementPrivate &operator=(PCtxKeyAgreementPrivate &&other) noexcept = delete;
    /// @brief default copy constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00229
    /// @trace_id_dd=DD_CRYPTO_01108
    /// @needwork = ad
    /// @endcode
    PCtxKeyAgreementPrivate(PCtxKeyAgreementPrivate const &other) noexcept = delete;
    /// @brief default move constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00230
    /// @trace_id_dd=DD_CRYPTO_01109
    /// @needwork = ad
    /// @endcode
    PCtxKeyAgreementPrivate(PCtxKeyAgreementPrivate &&other) noexcept = delete;

private:
    /// @brief Crypto provider object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01110
    /// @needwork = dda
    /// @endcode
    PCryptoProvider &cryptoProvider_;
    /// @brief Private key pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01111
    /// @needwork = dda
    /// @endcode
    PrivateKey const *pPrivateKey_;
    /// @brief Parameterized constructor
    /// @param cryptoProvider Crypto provider
    /// @param pPrivateKey Private key object pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01112
    /// @needwork = dda
    /// @endcode
    PCtxKeyAgreementPrivate(PCryptoProvider &cryptoProvider, PrivateKey const *const pPrivateKey) noexcept;

public:
    /// @brief Return the provider.
    /// @return Crypto provider reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00231
    /// @trace_id_dd=DD_CRYPTO_01113
    /// @needwork = ad
    /// @endcode
    CryptoProvider &MyProvider() const noexcept override;

private:
    /// @brief Generate keys.
    /// @param otherSideKey Another object instance of this class
    /// @param sharedKey Shared key
    /// @param len Key length
    /// @return true if compute share key sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01114
    /// @needwork = dda
    /// @endcode
    bool _computeKey(PublicKey const &otherSideKey, u_char *const sharedKey, size_t &len) const noexcept;
    /// @brief Check the key.
    /// @param key Private key
    /// @return true if check key sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01115
    /// @needwork = dda
    /// @endcode
    static bool CheckKey(PrivateKey const &key) noexcept;

private:
    /// @brief Generate keys via IPC.
    /// @param otherSideKey Another object instance of this class
    /// @param sharedKey Shared key
    /// @param len Key length
    /// @return true if compute share key sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01116
    /// @needwork = dda
    /// @endcode
    bool _computeKeyIpc(PublicKey const &otherSideKey, u_char *const sharedKey, size_t &len) const noexcept;

public:
    /// @brief Generate a common symmetric key by performing a key agreement algorithm between this private key and the other party's public key.
    /// The generated SymmetricKey object has the following attributes: session, non-exportable. This method can be used to directly generate a target key without creating an intermediate SecretSeed object.
    /// @brief Produce a common symmetric key via execution of the   between this private key and a public key of
    /// another side.
    /// Produced @c SymmetricKey object has following attributes: session, non-exportable.
    /// This method can be used for direct production of the target key, without creation of the intermediate @c
    /// SecretSeed object.
    /// @param otherSideKey The public key of the other side of the Key-Agreement
    /// @param kdf  the Context of a Key Derivation Function, which should be used for the target key production
    /// @param targetAlgId  identifier of the target symmetric algorithm (also defines a target key-length)
    /// @param allowedUsage  the allowed usage scope of the target key
    /// @param salt  an optional salt value (if used, it should be unique for each instance of the target key)
    /// @param ctxLabel  an optional application specific "context label" (it can identify purpose of the target
    /// key and/or communication parties)
    /// @returns a unique pointer to @c SecretSeed object, which contains the key material produced by the Key-Agreement
    /// algorithm
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kUninitializedContext   if the context was not initialized by a key value
    /// @error: SecurityErrorDomain::kIncompatibleObject     if the public and private keys correspond to different
    /// algorithms
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_21312}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02115}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00232
    /// @trace_id_dd=DD_CRYPTO_01117
    /// @needwork = ad
    /// @endcode
    ara::core::Result< SymmetricKey::Uptrc > AgreeKey(PublicKey const &otherSideKey,
                                                      KeyDerivationFunctionCtx &kdf,
                                                      AlgId targetAlgId,
                                                      AllowedUsageFlags allowedUsage,
                                                      ReadOnlyMemRegion const &salt = ReadOnlyMemRegion(),
                                                      ReadOnlyMemRegion const &ctxLabel
                                                      = ReadOnlyMemRegion()) const noexcept override;
    /// @brief Generate a common secret seed by performing a key agreement algorithm between this private key and the other party's public key.
    /// The generated SecretSeed object has the following attributes: session, non-exportable, AlgID.
    /// (Key-Agreement Algorithm ID)。
    /// @brief Produce a common secret seed via execution of the key-agreement algorithm between this private key and a
    /// public key of another side.
    ///         Produced @c SecretSeed object has following attributes: session, non-exportable, AlgID (this
    ///         Key-Agreement Algorithm ID).
    /// @param otherSideKey The public key of the other side of the Key-Agreement
    /// @param allowedUsage  the allowed usage scope of the target seed
    /// @returns unique pointer to @c SecretSeed object, which contains the key material produced by the Key-Agreement
    /// algorithm
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kUninitializedContext   if the context was not initialized by a key value
    /// @error: SecurityErrorDomain::kIncompatibleObject     if the public and private keys correspond to different
    /// algorithms
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_21311}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02007}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00233
    /// @trace_id_dd=DD_CRYPTO_01118
    /// @needwork = ad
    /// @endcode
    ara::core::Result< SecretSeed::Uptrc > AgreeSeed(PublicKey const &otherSideKey,
                                                     SecretSeed::Usage allowedUsage
                                                     = kAllowKdfMaterialAnyUsage) const noexcept override;
    /// @brief Get the ExtensionService instance.
    /// @brief Get ExtensionService instance.
    /// @return
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_21302}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02006}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00234
    /// @trace_id_dd=DD_CRYPTO_01119
    /// @needwork = ad
    /// @endcode
    ExtensionService::Uptr GetExtensionService() const noexcept override;
    /// @brief Clear the encryption context.
    /// @brief Clear the crypto context.
    /// @return has value if reset sucess false otherwise
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_21314}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02108}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00235
    /// @trace_id_dd=DD_CRYPTO_01120
    /// @needwork = ad
    /// @endcode
    ara::core::Result< void > Reset() noexcept override;
    /// @brief Set (deploy) a key to the key agreement private algorithm context.
    /// @brief Set (deploy) a key to the key agreement private algorithm context.
    /// @param key  the source key object
    /// @returns has value if SetKey sucess false otherwise
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kIncompatibleObject if the provided key object is incompatible with this private
    /// key context
    /// @error: SecurityErrorDomain::kUsageViolation     if the transformation type associated with this context is
    /// prohibited by the "allowed usage" restrictions of provided key object
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_21313}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02001}
    /// @uptrace={RS_CRYPTO_02003}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00236
    /// @trace_id_dd=DD_CRYPTO_01121
    /// @needwork = ad
    /// @endcode
    ara::core::Result< void > SetKey(PrivateKey const &key) noexcept override;

public:  // PServiceExtension interface
    /// @brief Get the actual bit length of the key loaded into the context. If no key is set for the context, returns 0.
    /// @brief Get actual bit-length of a key loaded to the context. If no key was set to the context yet then 0 is
    /// returned.
    /// @returns actual length of a key (now set to the algorithm context) in bits
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_29045}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00237
    /// @trace_id_dd=DD_CRYPTO_01122
    /// @needwork = ad
    /// @endcode
    virtual std::size_t GetActualKeyBitLength() const noexcept;
    /// @brief Get the COUID of the key deployed to the context attached to this extended service. If no key is set for the context, returns an empty COUID (Nil).
    /// @brief Get the COUID of the key deployed to the context this extension service is attached to.
    /// If no key was set to the context yet then an empty COUID (Nil) is returned.
    /// @returns the COUID of the CryptoObject
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_29047}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00238
    /// @trace_id_dd=DD_CRYPTO_01123
    /// @needwork = ad
    /// @endcode
    virtual CryptoObjectUid GetActualKeyCOUID() const noexcept;
    /// @brief Get the allowed usage of this context (according to the key object properties loaded into this context).
    /// If the context has not been initialized with a key object, must return 0 (all flags reset).
    /// @brief Get allowed usages of this context (according to the key object attributes loaded to this context).
    /// If the context is not initialized by a key object yet then zero (all flags are reset) must be returned.
    /// @returns a combination of bit-flags that specifies allowed usages of the context
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_29046}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02008}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00239
    /// @trace_id_dd=DD_CRYPTO_01124
    /// @needwork = ad
    /// @endcode
    virtual AllowedUsageFlags GetAllowedUsage() const noexcept;
    /// @brief Get the maximum supported key length (in bits).
    /// @brief Get maximal supported key length in bits.
    /// @returns maximal supported length of the key in bits
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_29044}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00240
    /// @trace_id_dd=DD_CRYPTO_01125
    /// @needwork = ad
    /// @endcode
    virtual std::size_t GetMaxKeyBitLength() const noexcept;
    /// @brief Get the minimum supported key length (in bits).
    /// @brief Get minimal supported key length in bits.
    /// @returns minimal supported length of the key in bits
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_29043}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00241
    /// @trace_id_dd=DD_CRYPTO_01126
    /// @needwork = ad
    /// @endcode
    virtual std::size_t GetMinKeyBitLength() const noexcept;
    /// @brief Validate support for a specific key length according to the context.
    /// @brief Verify supportness of specific key length by the context.
    /// @param keyBitLength Key length in bits
    /// @returns @c true if provided value of the key length is supported by the context
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_29048}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00242
    /// @trace_id_dd=DD_CRYPTO_01127
    /// @needwork = ad
    /// @endcode
    virtual bool IsKeyBitLengthSupported(std::size_t keyBitLength) const noexcept;
    /// @brief Check whether a key is set for this context.
    /// @brief Check if a key has been set to this context.
    /// @return
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_29049}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00243
    /// @trace_id_dd=DD_CRYPTO_01128
    /// @needwork = ad
    /// @endcode
    virtual bool IsKeyAvailable() const noexcept;
    /// @brief Whether initialized.
    /// @return true if has already init key
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00244
    /// @trace_id_dd=DD_CRYPTO_01129
    /// @needwork = ad
    /// @endcode
    bool IsInitialized() const noexcept override;
    /// @brief Get the instance identifier ID.
    /// @return CryptoPrimitiveId instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00245
    /// @trace_id_dd=DD_CRYPTO_01130
    /// @needwork = ad
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override;
};
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYP_PUHUA_CTX_KEY_AGREEMENT_PEIVATE_H_
