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
/// @file       isoft_ctx_key_decapsulator_private.h
/// @brief      AutoSar-Crypto encryption/decryption module
/// @details    Asymmetric key encapsulator.
/// @date       2022-04-14
/// @author     Chang Zheng
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr><td>2022-04-24  </td>1.0.0    </td>Chang Zheng     </td>Create initial version</td>
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default encryption/Key encapsulation
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_01015
/// @unit_name=PCtxKeyDecapsulatorPrivate
/// @unit_description=Private key decapsulation context base class
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_PUHUA_CTX_KEY_DESCAPSULATOR_PEIVATE_H_
#define ARA_CRYPTO_CRYP_PUHUA_CTX_KEY_DESCAPSULATOR_PEIVATE_H_

#include "ara/crypto/cryp/key_decapsulator_private_ctx.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
/// @brief Crypto provider
class PCryptoProvider;
/// @brief Asymmetric key encapsulator.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00737
/// @trace_id_dd=DD_CRYPTO_02650
/// @needwork = ad
/// @endcode
class PCtxKeyDecapsulatorPrivate : public KeyDecapsulatorPrivateCtx
{
public:
    /// @brief Parameterized constructor
    /// @param cryptoProvider Crypto provider
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00738
    /// @trace_id_dd=DD_CRYPTO_02651
    /// @needwork = ad
    /// @endcode
    explicit PCtxKeyDecapsulatorPrivate(PCryptoProvider &cryptoProvider) noexcept;
    /// @brief default constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00739
    /// @trace_id_dd=DD_CRYPTO_02652
    /// @needwork = ad
    /// @endcode
    PCtxKeyDecapsulatorPrivate() = delete;
    /// @brief default virtual destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00740
    /// @trace_id_dd=DD_CRYPTO_02653
    /// @needwork = ad
    /// @endcode
    ~PCtxKeyDecapsulatorPrivate() noexcept override = default;
    /// @brief default move constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00741
    /// @trace_id_dd=DD_CRYPTO_02654
    /// @needwork = ad
    /// @endcode
    PCtxKeyDecapsulatorPrivate(PCtxKeyDecapsulatorPrivate &&other) = delete;
    /// @brief default move assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00742
    /// @trace_id_dd=DD_CRYPTO_02655
    /// @needwork = ad
    /// @endcode
    PCtxKeyDecapsulatorPrivate &operator=(PCtxKeyDecapsulatorPrivate &&other) = delete;
    /// @brief default copy assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00743
    /// @trace_id_dd=DD_CRYPTO_02656
    /// @needwork = ad
    /// @endcode
    PCtxKeyDecapsulatorPrivate &operator=(PCtxKeyDecapsulatorPrivate const &other) = delete;
    /// @brief default copy constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00744
    /// @trace_id_dd=DD_CRYPTO_02657
    /// @needwork = ad
    /// @endcode
    PCtxKeyDecapsulatorPrivate(PCtxKeyDecapsulatorPrivate const &other) = delete;

private:
    /// @brief Crypto provider object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02658
    /// @needwork = dda
    /// @endcode
    PCryptoProvider &cryptoProvider_;

public:
    /// @brief Get the provider.
    /// @return Crypto provider reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00745
    /// @trace_id_dd=DD_CRYPTO_02659
    /// @needwork = ad
    /// @endcode
    CryptoProvider &MyProvider() const noexcept override;

public:
    /// @brief Decapsulate the key encryption key (KEK). The generated SymmetricKey object has the following properties: session, non-exportable, Key.
    /// Usage:kAllowKeyImporting。
    ///         This method can be used to directly generate a target key without creating an intermediate SecretSeed object.
    /// @brief Decapsulate Key Encryption Key (KEK).
    ///         Produced @c SymmetricKey object has following attributes: session, non-exportable, Key Usage: @c
    ///         kAllowKeyImporting. This method can be used for direct production of the target key, without creation of
    ///         the intermediate @c SecretSeed object.
    /// @param input  an input buffer (its size should be equal @c GetEncapsulatedSize() bytes)
    /// @param kdf  a context of a key derivation function, which should be used for the target KEK production
    /// @param kekAlgId  an algorithm ID of the target KEK
    /// @param salt  an optional salt value (if used, it should be unique for each instance of the target key)
    /// @param ctxLabel  an pptional application specific "context label" (it can identify purpose of the target key
    /// and/or communication parties)
    /// @returns unique smart pointer to a symmetric key object derived from a key material decapsulated from the input
    /// block
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kUninitializedContext   if the context was not initialized by a private key value
    /// @error: SecurityErrorDomain::kUnknownIdentifier      if @c kekAlgId specifies incorrect algorithm
    /// @error: SecurityErrorDomain::kInvalidInputSize       if (input.size() <> this->GetEncapsulatedSize())
    /// @trace_id_sws={SWS_CRYPT_21412}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02102}
    /// @uptrace={RS_CRYPTO_02108}
    /// @uptrace={RS_CRYPTO_02115}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00746
    /// @trace_id_dd=DD_CRYPTO_02660
    /// @needwork = ad
    /// @endcode
    ara::core::Result< SymmetricKey::Uptrc > DecapsulateKey(ReadOnlyMemRegion const &input,
                                                            KeyDerivationFunctionCtx &kdf,
                                                            AlgId kekAlgId,
                                                            ReadOnlyMemRegion const &salt = ReadOnlyMemRegion(),
                                                            ReadOnlyMemRegion const &ctxLabel
                                                            = ReadOnlyMemRegion()) const noexcept override = 0;
    /// @brief Decapsulate key material.
    /// The returned key material object should be used to derive a symmetric key. The generated SecretSeed object has the following properties: session, non-exportable, AlgID = this KEM AlgID.
    /// @brief Decapsulate key material.
    ///         Returned Key Material object should be used for derivation of a symmetric key.
    ///         Produced @c SecretSeed object has following attributes: session, non-exportable, AlgID = this KEM AlgID.
    /// @param input  a buffer with the encapsulated seed (its size should be equal @c GetEncapsulatedSize() bytes)
    /// @param allowedUsage  the allowed usage scope of the target seed
    /// @returns unique smart pointer to @c SecretSeed object, which keeps the key material decapsulated from the input
    /// buffer
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kUninitializedContext   if the context was not initialized by a private key value
    /// @error: SecurityErrorDomain::kInsufficientCapacity   if the @c output.size() is not enough to save the
    /// decapsulation result
    /// @trace_id_sws={SWS_CRYPT_21411}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02007}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00747
    /// @trace_id_dd=DD_CRYPTO_02661
    /// @needwork = ad
    /// @endcode
    ara::core::Result< SecretSeed::Uptrc > DecapsulateSeed(ReadOnlyMemRegion const &input,
                                                           SecretSeed::Usage allowedUsage
                                                           = kAllowKdfMaterialAnyUsage) const noexcept override = 0;
    /// @brief Get the fixed size of the encapsulated data block.
    /// @brief Get fixed size of the encapsulated data block.
    /// @returns size of the encapsulated data block in bytes
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_21416}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00748
    /// @trace_id_dd=DD_CRYPTO_02662
    /// @needwork = ad
    /// @endcode
    std::size_t GetEncapsulatedSize() const noexcept override = 0;
    /// @brief Get the ExtensionService instance.
    /// @brief Get ExtensionService instance.
    /// @return ExtensionService instance
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_21402}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02006}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00749
    /// @trace_id_dd=DD_CRYPTO_02663
    /// @needwork = ad
    /// @endcode
    ExtensionService::Uptr GetExtensionService() const noexcept override = 0;
    /// @brief Get the key entropy (bit length) of the key (KEK) material.
    /// For RSA systems, the return value corresponds to the module length N (-1); for DH-like systems, the return value corresponds to the module length q (-1).
    /// @brief Get entropy (bit-length) of the key encryption key (KEK) material.
    ///         For RSA system the returned value corresponds to the length of module N (minus 1).
    ///         For DH-like system the returned value corresponds to the length of module q (minus 1).
    /// @returns entropy of the KEK material in bits
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_21415}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00750
    /// @trace_id_dd=DD_CRYPTO_02664
    /// @needwork = ad
    /// @endcode
    std::size_t GetKekEntropy() const noexcept override = 0;
    /// @brief Clear the encryption context.
    /// @brief Clear the crypto context.
    /// @return has vlaue if Reset sucess false otherwise
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_21414}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02108}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00751
    /// @trace_id_dd=DD_CRYPTO_02665
    /// @needwork = ad
    /// @endcode
    ara::core::Result< void > Reset() noexcept override = 0;
    /// @brief Set (deploy) a key to the key decapsulator private algorithm context.
    /// @brief Set (deploy) a key to the key decapsulator private algorithm context.
    /// @param key  the source key object
    /// @return has vlaue if SetKey sucess false otherwise
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kIncompatibleObject     if the provided key object is incompatible with this
    /// private key context
    /// @error: SecurityErrorDomain::kUsageViolation         if the transformation type associated with this context is
    /// prohibited by the "allowed usage" restrictions of provided key object
    /// @trace_id_sws={SWS_CRYPT_21413}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02001}
    /// @uptrace={RS_CRYPTO_02003}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00752
    /// @trace_id_dd=DD_CRYPTO_02666
    /// @needwork = ad
    /// @endcode
    ara::core::Result< void > SetKey(PrivateKey const &key) noexcept override = 0;

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
    /// @trace_id_ad=AD_CRYPTO_00753
    /// @trace_id_dd=DD_CRYPTO_02667
    /// @needwork = ad
    /// @endcode
    virtual std::size_t GetActualKeyBitLength() const noexcept = 0;
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
    /// @trace_id_ad=AD_CRYPTO_00754
    /// @trace_id_dd=DD_CRYPTO_02668
    /// @needwork = ad
    /// @endcode
    virtual CryptoObjectUid GetActualKeyCOUID() const noexcept = 0;
    /// @brief Get the allowed usage of this context (according to the key object properties loaded into this context). If the context has not been initialized with a key object, must return 0 (all flags reset).
    /// @brief Get allowed usages of this context (according to the key object attributes loaded to this context).
    /// If the context is not initialized by a key object yet then zero (all flags are reset) must be returned.
    /// @returns a combination of bit-flags that specifies allowed usages of the context
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_29046}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02008}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00755
    /// @trace_id_dd=DD_CRYPTO_02669
    /// @needwork = ad
    /// @endcode
    virtual AllowedUsageFlags GetAllowedUsage() const noexcept = 0;
    /// @brief Get the maximum supported key length (in bits).
    /// @brief Get maximal supported key length in bits.
    /// @returns maximal supported length of the key in bits
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_29044}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00756
    /// @trace_id_dd=DD_CRYPTO_02670
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
    /// @trace_id_ad=AD_CRYPTO_00757
    /// @trace_id_dd=DD_CRYPTO_02671
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
    /// @trace_id_ad=AD_CRYPTO_00758
    /// @trace_id_dd=DD_CRYPTO_02672
    /// @needwork = ad
    /// @endcode
    virtual bool IsKeyBitLengthSupported(std::size_t keyBitLength) const noexcept = 0;
    /// @brief Check whether a key is set for this context.
    /// @brief Check if a key has been set to this context.
    /// @return true if a key has been set to this context false otherwise
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_29049}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00759
    /// @trace_id_dd=DD_CRYPTO_02673
    /// @needwork = ad
    /// @endcode
    virtual bool IsKeyAvailable() const noexcept = 0;
    /// @brief Determine whether the context is initialized
    /// @return true if already init false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00760
    /// @trace_id_dd=DD_CRYPTO_02674
    /// @needwork = ad
    /// @endcode
    bool IsInitialized() const noexcept override = 0;
    /// @brief Get the crypto primitive ID.
    /// @return CryptoPrimitivId instance.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00761
    /// @trace_id_dd=DD_CRYPTO_02675
    /// @needwork = ad
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override = 0;
};
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYP_PUHUA_CTX_KEY_DESCAPSULATOR_PEIVATE_H_
