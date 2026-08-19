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
/// @file       isoft_ctx_key_encapsulator_public.h
/// @brief      AutoSar-Crypto encryption/decryption module
/// @details    Asymmetric key encapsulator.
/// @date       2022-04-24
/// @author     Chang Zheng
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// </td><td>2022-04-24  </td>1.0.0    </td>Chang Zheng     </td>Create initial version</td>
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default encryption/Key encapsulation
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_01015
/// @unit_name=PCtxKeyEncapsulatorPublic
/// @unit_description=Public key encapsulation context base class
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_PUHUA_CTX_KEY_ENCAPSULATOR_PUBLIC_H_
#define ARA_CRYPTO_CRYP_PUHUA_CTX_KEY_ENCAPSULATOR_PUBLIC_H_

#include "ara/crypto/cryp/key_encapsulator_public_ctx.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
/// @brief Crypto provider
class PCryptoProvider;
/// @brief Asymmetric key encapsulator.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00712
/// @trace_id_dd=DD_CRYPTO_02624
/// @needwork = ad
/// @endcode
class PCtxKeyEncapsulatorPublic : public KeyEncapsulatorPublicCtx
{
public:
    /// @brief Parameterized constructor
    /// @param cryptoProvider Crypto provider
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00713
    /// @trace_id_dd=DD_CRYPTO_02625
    /// @needwork = ad
    /// @endcode
    explicit PCtxKeyEncapsulatorPublic(PCryptoProvider &cryptoProvider) noexcept;
    /// @brief default constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00714
    /// @trace_id_dd=DD_CRYPTO_02626
    /// @needwork = ad
    /// @endcode
    PCtxKeyEncapsulatorPublic() = delete;
    /// @brief default virtual destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00715
    /// @trace_id_dd=DD_CRYPTO_02627
    /// @needwork = ad
    /// @endcode
    ~PCtxKeyEncapsulatorPublic() noexcept override = default;
    /// @brief default move constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00716
    /// @trace_id_dd=DD_CRYPTO_02628
    /// @needwork = ad
    /// @endcode
    PCtxKeyEncapsulatorPublic(PCtxKeyEncapsulatorPublic &&other) = delete;
    /// @brief default move assignment operator
    /// @param other another object instance of this class
    /// @return  *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00717
    /// @trace_id_dd=DD_CRYPTO_02629
    /// @needwork = ad
    /// @endcode
    PCtxKeyEncapsulatorPublic &operator=(PCtxKeyEncapsulatorPublic &&other) = delete;
    /// @brief default copy assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00718
    /// @trace_id_dd=DD_CRYPTO_02630
    /// @needwork = ad
    /// @endcode
    PCtxKeyEncapsulatorPublic &operator=(PCtxKeyEncapsulatorPublic const &other) = delete;
    /// @brief default copy constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00719
    /// @trace_id_dd=DD_CRYPTO_02631
    /// @needwork = ad
    /// @endcode
    PCtxKeyEncapsulatorPublic(PCtxKeyEncapsulatorPublic const &other) = delete;

private:
    /// @brief Crypto provider
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02632
    /// @needwork = dda
    /// @endcode
    PCryptoProvider &cryptoProvider_;

public:
    /// @brief Get the provider.
    /// @return Crypto provider reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00720
    /// @trace_id_dd=DD_CRYPTO_02633
    /// @needwork = ad
    /// @endcode
    CryptoProvider &MyProvider() const noexcept override;

public:
    /// @brief Get the fixed size of the encapsulated data block.
    /// @brief Get fixed size of the encapsulated data block.
    /// @returns size of the encapsulated data block in bytes
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_21818}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00721
    /// @trace_id_dd=DD_CRYPTO_02634
    /// @needwork = ad
    /// @endcode
    std::size_t GetEncapsulatedSize() const noexcept override = 0;
    /// @brief Get the ExtensionService instance.
    /// @brief Get ExtensionService instance.
    /// @return ExtensionService instance
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_21802}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02006}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00722
    /// @trace_id_dd=DD_CRYPTO_02635
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
    /// @trace_id_sws={SWS_CRYPT_21817}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00723
    /// @trace_id_dd=DD_CRYPTO_02636
    /// @needwork = ad
    /// @endcode
    std::size_t GetKekEntropy() const noexcept override = 0;
    /// @brief Add the content (payload) to be encapsulated according to RFC 5990 ("keying data"). Currently, only SymmetricKey and SecretSeed objects are supported.
    /// @brief Add the content to be encapsulated (payload) according to RFC 5990 ("keying data").
    ///         At the moment only SymmetricKey and SecretSeed objects are supported.
    /// @param keyingData  the payload to be protected
    /// @returns has vlaue if AddKeyingData sucess false otherwise
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_21810}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02007}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00724
    /// @trace_id_dd=DD_CRYPTO_02637
    /// @needwork = ad
    /// @endcode
    ara::core::Result< void > AddKeyingData(RestrictedUseObject &keyingData) noexcept override = 0;
    /// @brief Encapsulate the key encryption key (KEK).
    /// @brief Encapsulate Key Encryption Key (KEK).
    /// @param kdf  a context of a key derivation function, which should be used for the target KEK production
    /// @param kekAlgId  an algorithm ID of the target KEK
    /// @param salt  an optional salt value (if used, it should be unique for each instance of the target key)
    /// @param ctxLabel  an optional application specific "context label" (it can identify purpose of the target key
    /// and/or communication parties)
    /// @returns    unique smart pointer to a symmetric key object derived from a randomly generated material
    /// encapsulated to the output buffer
    ///       Only first @c GetEncapsulatedSize() bytes of the output buffer should be updated by this method.
    ///       Produced @c SymmetricKey object has following attributes: session, non-exportable, Allowed Key Usage: @c
    ///       kAllowKeyExporting. This method can be used for direct production of the target key, without creation of
    ///       the intermediate @c SecretSeed object.
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_21813}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02102}
    /// @uptrace={RS_CRYPTO_02108}
    /// @uptrace={RS_CRYPTO_02115}
    /// @error: SecurityErrorDomain::kUninitializedContext   if the context was not initialized by a public key value
    /// @error: SecurityErrorDomain::kInvalidArgument        if @c kekAlgId specifies incorrect algorithm
    /// @error: SecurityErrorDomain::kInsufficientCapacity   if the @c output.size() is not enough to save the
    /// encapsulation result
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00725
    /// @trace_id_dd=DD_CRYPTO_02638
    /// @needwork = ad
    /// @endcode
    ara::core::Result< ara::core::Vector< ara::core::Byte > > Encapsulate(
        KeyDerivationFunctionCtx &kdf,
        AlgId kekAlgId,
        ReadOnlyMemRegion const &salt     = ReadOnlyMemRegion(),
        ReadOnlyMemRegion const &ctxLabel = ReadOnlyMemRegion()) const noexcept override = 0;

public:
    /// @brief Clear the encryption context.
    /// @brief Clear the crypto context.
    /// @return has vlaue if Reset sucess false otherwise
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_21816}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02108}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00726
    /// @trace_id_dd=DD_CRYPTO_02639
    /// @needwork = ad
    /// @endcode
    ara::core::Result< void > Reset() noexcept override = 0;
    /// @brief Set (deploy) a key to the key encapsulator public algorithm context.
    /// @brief Set (deploy) a key to the key encapsulator public algorithm context.
    /// @param key  the source key object
    /// @returns has vlaue if SetKey sucess false otherwise
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_21815}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02001}
    /// @uptrace={RS_CRYPTO_02003}
    /// @error: SecurityErrorDomain::kIncompatibleObject     if the provided key object is incompatible with this
    /// symmetric key context
    /// @error: SecurityErrorDomain::kUsageViolation         if the transformation type associated with this context is
    /// prohibited by the "allowed usage" restrictions of provided key object
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00727
    /// @trace_id_dd=DD_CRYPTO_02640
    /// @needwork = ad
    /// @endcode
    ara::core::Result< void > SetKey(PublicKey const &key) noexcept override = 0;

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
    /// @trace_id_ad=AD_CRYPTO_00728
    /// @trace_id_dd=DD_CRYPTO_02641
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
    /// @trace_id_ad=AD_CRYPTO_00729
    /// @trace_id_dd=DD_CRYPTO_02642
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
    /// @trace_id_ad=AD_CRYPTO_00730
    /// @trace_id_dd=DD_CRYPTO_02643
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
    /// @trace_id_ad=AD_CRYPTO_00731
    /// @trace_id_dd=DD_CRYPTO_02644
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
    /// @trace_id_ad=AD_CRYPTO_00732
    /// @trace_id_dd=DD_CRYPTO_02645
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
    /// @trace_id_ad=AD_CRYPTO_00733
    /// @trace_id_dd=DD_CRYPTO_02646
    /// @needwork = ad
    /// @endcode
    virtual bool IsKeyBitLengthSupported(std::size_t keyBitLength) const noexcept = 0;
    /// @brief Check whether a key is set for this context.
    /// @brief Check if a key has been set to this context.
    /// @returns true if a key has been set to this context false otherwise
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_29049}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00734
    /// @trace_id_dd=DD_CRYPTO_02647
    /// @needwork = ad
    /// @endcode
    virtual bool IsKeyAvailable() const noexcept = 0;
    /// @brief Determine whether the context is initialized
    /// @return true if already init false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00735
    /// @trace_id_dd=DD_CRYPTO_02648
    /// @needwork = ad
    /// @endcode
    bool IsInitialized() const noexcept override = 0;
    /// @brief Get the crypto primitive ID.
    /// @return CryptoPrimitivId instance.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00736
    /// @trace_id_dd=DD_CRYPTO_02649
    /// @needwork = ad
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override = 0;
};
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYP_PUHUA_CTX_KEY_ENCAPSULATOR_PUBLIC_H_