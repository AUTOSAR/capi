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
/// @file       isoft_ctx_symmetric_key_wrapper_base.h
/// @brief      AutoSar-Crypto encryption/decryption module
/// @details    Symmetric key wrapping.
/// @date       2022-04-18
/// @author     Chang Zheng
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr>}<2022-04-18  </td>1.0.0    <td>Chang Zheng     </td>Create initial version</td>
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default encryption/Key wrapping
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_01018
/// @unit_name=PCtxSymmetricKeyWrapper_Base
/// @unit_description=Key wrapping context base class
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_PUHUA_CTX_SYMMETRIC_KEY_WRAPPER_BASE_H_
#define ARA_CRYPTO_CRYP_PUHUA_CTX_SYMMETRIC_KEY_WRAPPER_BASE_H_

#include "ara/crypto/cryp/symmetric_key_wrapper_ctx.h"
#include "ara/crypto/ipc/isoft_ipc_client.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
/// @brief Crypto provider
class PCryptoProvider;
/// @brief Symmetric key wrapping: base class
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00530
/// @trace_id_dd=DD_CRYPTO_01989
/// @needwork = ad
/// @endcode
class PCtxSymmetricKeyWrapper_Base : public SymmetricKeyWrapperCtx
{
public:
    /// @brief Parameterized constructor
    /// @param cryptoProvider PCryptoProvider
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00531
    /// @trace_id_dd=DD_CRYPTO_01990
    /// @needwork = ad
    /// @endcode
    explicit PCtxSymmetricKeyWrapper_Base(PCryptoProvider &cryptoProvider) noexcept;
    /// @brief default constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00532
    /// @trace_id_dd=DD_CRYPTO_01991
    /// @needwork = ad
    /// @endcode
    PCtxSymmetricKeyWrapper_Base() = delete;
    /// @brief default virtual destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00533
    /// @trace_id_dd=DD_CRYPTO_01992
    /// @needwork = ad
    /// @endcode
    ~PCtxSymmetricKeyWrapper_Base() override = default;
    /// @brief default move constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00534
    /// @trace_id_dd=DD_CRYPTO_01993
    /// @needwork = ad
    /// @endcode
    PCtxSymmetricKeyWrapper_Base(PCtxSymmetricKeyWrapper_Base &&other) = delete;
    /// @brief default move assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00535
    /// @trace_id_dd=DD_CRYPTO_01994
    /// @needwork = ad
    /// @endcode
    PCtxSymmetricKeyWrapper_Base &operator=(PCtxSymmetricKeyWrapper_Base &&other) = delete;
    /// @brief default copy assignment operator
    /// @param other another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00536
    /// @trace_id_dd=DD_CRYPTO_01995
    /// @needwork = ad
    /// @endcode
    PCtxSymmetricKeyWrapper_Base &operator=(PCtxSymmetricKeyWrapper_Base const &other) = delete;
    /// @brief default copy constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00537
    /// @trace_id_dd=DD_CRYPTO_01996
    /// @needwork = ad
    /// @endcode
    PCtxSymmetricKeyWrapper_Base(PCtxSymmetricKeyWrapper_Base const &other) = delete;

private:
    /// @brief Crypto provider object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01997
    /// @needwork = dda
    /// @endcode
    PCryptoProvider &cryptoProvider_;

public:
    /// @brief Calculate the size (in bytes) of the wrapped key from the original key length (in bits). This method is useful for implementations different from RFC3394 / RFC5649.
    /// @brief Calculate size of the wrapped key in bytes from original key length in bits.
    ///       This method can be useful for some implementations different from RFC3394 / RFC5649.
    /// @param keyLength  original key length in bits
    /// @returns size of the wrapped key in bytes
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_24013}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02201}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00538
    /// @trace_id_dd=DD_CRYPTO_01998
    /// @needwork = ad
    /// @endcode
    std::size_t CalculateWrappedKeySize(std::size_t keyLength) const noexcept override = 0;
    /// @brief Get the ExtensionService instance.
    /// @brief Get ExtensionService instance.
    /// @return
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_24002}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02006}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00539
    /// @trace_id_dd=DD_CRYPTO_01999
    /// @needwork = ad
    /// @endcode
    ExtensionService::Uptr GetExtensionService() const noexcept override = 0;
    /// @brief Get the maximum length of the target key supported by the implementation. This method is useful for implementations different from RFC3394 / RFC5649.
    /// @brief Get maximum length of the target key supported by the implementation.
    ///       This method can be useful for some implementations different from RFC3394 / RFC5649.
    /// @returns maximum length of the target key in bits
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_24012}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02201}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00540
    /// @trace_id_dd=DD_CRYPTO_02000
    /// @needwork = ad
    /// @endcode
    std::size_t GetMaxTarGetKeyLength() const noexcept override = 0;
    /// @brief Get the expected granularity (block size) of the target key.
    ///         If the class implements RFC3394 (KW without padding), this method should return 8 (i.e., 8 bytes = 64 bits).
    ///         If the class implements RFC5649 (KW with padding), this method should return 1 (i.e., 1 octet = 8 bits).
    /// @brief Get expected granularity of the target key (block size).
    ///       If the class implements RFC3394 (KW without padding) then this method should return 8 (i.e. 8 octets = 64
    ///       bits). If the class implements RFC5649 (KW with padding) then this method should return 1 (i.e. 1 octet =
    ///       8 bits).
    /// @returns size of the block in bytes
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_24011}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02201}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00541
    /// @trace_id_dd=DD_CRYPTO_02001
    /// @needwork = ad
    /// @endcode
    std::size_t GetTargetKeyGranularity() const noexcept override = 0;
    /// @brief Clear the encryption context.
    /// @brief Clear the crypto context.
    /// @return
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_24019}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02108}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00542
    /// @trace_id_dd=DD_CRYPTO_02002
    /// @needwork = ad
    /// @endcode
    ara::core::Result< void > Reset() noexcept override = 0;
    /// @brief Set (deploy) a key to the symmetric key wrapper algorithm context.
    /// @brief Set (deploy) a key to the symmetric key wrapper algorithm context.
    /// @param key  the source key object
    /// @param transform  the "direction" indicator: deploy the key for direct transformation (if @c true) or
    /// for reverse one (if @c false)
    /// @return has value if SetKey sucess false otherwise
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kIncompatibleObject     if the provided key object is incompatible with this
    /// symmetric key context
    /// @error: SecurityErrorDomain::kUsageViolation         if the transformation type associated with this context
    ///                (taking into account the direction specified by @c transform) is prohibited by the "allowed
    ///                usage" restrictions of provided key object
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_24018}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02001}
    /// @uptrace={RS_CRYPTO_02003}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00543
    /// @trace_id_dd=DD_CRYPTO_02003
    /// @needwork = ad
    /// @endcode
    ara::core::Result< void > SetKey(SymmetricKey const &key, CryptoTransform transform) noexcept override = 0;
    /// @brief Perform a "key
    /// unwrap" operation on the provided BLOB and generate a key object. If the implementation is based on an AES block cipher and applied to an AES key, this method should conform to RFC3394 or RFC5649.
    ///         The created Key object has the following properties: session and non-exportable (because it is imported without meta-information)!
    /// @brief Execute the "key unwrap" operation for provided BLOB and produce @c Key object.
    ///         This method should be compliant to RFC3394 or RFC5649, if implementation is based on the AES block
    ///         cipher and applied to an AES key. The created @c Key object has following attributes: session and
    ///         non-exportable (because it was imported without meta-information)!
    ///         @c SymmetricKey may be unwrapped in following way: SymmetricKey::Uptrc key = SymmetricKey::Cast(UnwrapKey(wrappedKey, ...));
    ///         @c PrivateKey may be unwrapped in following way: PrivateKey::Uptrc key = PrivateKey::Cast(UnwrapKey(wrappedKey, ...));
    ///         In both examples the @c Cast() method may additionally @b throw the @c BadObjectTypeException if an actual type of the unwrapped key differs from the target one!
    /// @param wrappedKey  a memory region that contains wrapped key
    /// @param algId  an identifier of the target symmetric crypto algorithm
    /// @param allowedUsage  bit-flags that define a list of allowed transformations' types in which the target key
    /// can be used
    /// @returns unique smart pointer to @c Key object, which keeps unwrapped key material
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kInvalidInputSize       if the size of provided wrapped key is unsupported
    /// @error: SecurityErrorDomain::kUninitializedContext   if the context was not initialized by a key value
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_24016}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02115}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00544
    /// @trace_id_dd=DD_CRYPTO_02004
    /// @needwork = ad
    /// @endcode
    ara::core::Result< RestrictedUseObject::Uptrc > UnwrapKey(ReadOnlyMemRegion const &wrappedKey,
                                                              AlgId algId,
                                                              AllowedUsageFlags allowedUsage) const noexcept override;
    /// @brief Perform a "key unwrap" operation on the provided BLOB and generate a SecretSeed object.
    ///         If the implementation is based on an AES block cipher and applied to AES key material, this method should conform to RFC3394 or RFC5649. The created SecretSeed object has the following properties: session and non-exportable (because it is imported without meta-information).
    /// @brief Execute the "key unwrap" operation for provided BLOB and produce @c SecretSeed object.
    ///       This method should be compliant to RFC3394 or RFC5649, if implementation is based on the AES block cipher
    ///       and applied to an AES key material. The created @c SecretSeed object has following attributes: session and
    ///       non-exportable (because it was imported without meta-information).
    /// @param wrappedSeed  a memory region that contains wrapped seed
    /// @param targetAlgId  the target symmetric algorithm identifier (also defines a target seed-length)
    /// @param allowedUsage  allowed usage scope of the target seed
    /// @returns unique smart pointer to @c SecretSeed object, which keeps unwrapped key material
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kInvalidInputSize       if the size of provided wrapped seed is unsupported
    /// @error: SecurityErrorDomain::kUninitializedContext   if the context was not initialized by a key value
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_24015}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02007}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00545
    /// @trace_id_dd=DD_CRYPTO_02005
    /// @needwork = ad
    /// @endcode
    ara::core::Result< SecretSeed::Uptrc > UnwrapSeed(ReadOnlyMemRegion const &wrappedSeed,
                                                      AlgId targetAlgId,
                                                      SecretSeed::Usage allowedUsage) const noexcept override;
    /// @brief Perform a "key wrapping" operation on the provided key material.
    ///         If an implementation is based on an AES block cipher and applied to an AES key, this method should conform to RFC3394 or RFC5649. The method CalculateWrappedKeySize() can be used to calculate the required output buffer size.
    /// @brief Execute the "key wrap" operation for the provided key material.
    ///       This method should be compliant to RFC3394 or RFC5649, if an implementation is based on the AES block
    ///       cipher and applied to an AES key. Method @c CalculateWrappedKeySize() can be used for size calculation of
    ///       the required output buffer.
    /// @param key  a key that should be wrapped
    /// @return Wrapping result
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kInsufficientCapacity   if the size of the @c wrapped buffer is not enough for
    /// storing the result
    /// @error: SecurityErrorDomain::kInvalidInputSize       if the @c key object has an unsupported length
    /// @error: SecurityErrorDomain::kUninitializedContext   if the context was not initialized by a key value
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_24014}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02201}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00546
    /// @trace_id_dd=DD_CRYPTO_02006
    /// @needwork = ad
    /// @endcode
    ara::core::Result< ara::core::Vector< ara::core::Byte > > WrapKeyMaterial(
        RestrictedUseObject const &key) const noexcept override;

public:
    /// @brief Get the bound crypto provider.
    /// @return Crypto provider reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00547
    /// @trace_id_dd=DD_CRYPTO_02007
    /// @needwork = ad
    /// @endcode
    CryptoProvider &MyProvider() const noexcept override;

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
    /// @trace_id_ad=AD_CRYPTO_00548
    /// @trace_id_dd=DD_CRYPTO_02008
    /// @needwork = ad
    /// @endcode
    virtual std::size_t GetActualKeyBitLength() const noexcept = 0;
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
    /// @trace_id_ad=AD_CRYPTO_00549
    /// @trace_id_dd=DD_CRYPTO_02009
    /// @needwork = ad
    /// @endcode
    virtual CryptoObjectUid GetActualKeyCOUID() const noexcept = 0;
    /// @brief
    /// Get the allowed usage of this context (according to the key object properties loaded into this context). If the context has not been initialized with a key object, must return 0 (all flags reset).
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
    /// @trace_id_ad=AD_CRYPTO_00550
    /// @trace_id_dd=DD_CRYPTO_02010
    /// @needwork = ad
    /// @endcode
    virtual AllowedUsageFlags GetAllowedUsage() const noexcept = 0;
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
    /// @trace_id_ad=AD_CRYPTO_00551
    /// @trace_id_dd=DD_CRYPTO_02011
    /// @needwork = ad
    /// @endcode
    virtual std::size_t GetMaxKeyBitLength() const noexcept = 0;
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
    /// @trace_id_ad=AD_CRYPTO_00552
    /// @trace_id_dd=DD_CRYPTO_02012
    /// @needwork = ad
    /// @endcode
    virtual std::size_t GetMinKeyBitLength() const noexcept = 0;
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
    /// @trace_id_ad=AD_CRYPTO_00553
    /// @trace_id_dd=DD_CRYPTO_02013
    /// @needwork = ad
    /// @endcode
    virtual bool IsKeyBitLengthSupported(std::size_t keyBitLength) const noexcept = 0;
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
    /// @trace_id_ad=AD_CRYPTO_00554
    /// @trace_id_dd=DD_CRYPTO_02014
    /// @needwork = ad
    /// @endcode
    virtual bool IsKeyAvailable() const noexcept = 0;

public:
    /// @brief Check whether the encryption context is initialized and usable. It checks all required values, including: key value, IV/seed, etc.
    /// @return  true if already init false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00555
    /// @trace_id_dd=DD_CRYPTO_02015
    /// @needwork = ad
    /// @endcode
    bool IsInitialized() const noexcept override;
    /// @brief Perform wrapping logic.
    /// @param pInputData Starting address of input data for algorithm operation
    /// @param nDataLen Data length
    /// @return Wrapping result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00556
    /// @trace_id_dd=DD_CRYPTO_02016
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< ara::core::Vector< ara::core::Byte > > DoWrap(uint8_t const *pInputData,
                                                                             uint32_t nDataLen) const noexcept = 0;
    /// @brief Perform local unwrapping logic.
    /// @param pInputData Starting address of input data for algorithm operation
    /// @param nDataLen Data length
    /// @return Unwrapping result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00557
    /// @trace_id_dd=DD_CRYPTO_02017
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< ara::core::Vector< ara::core::Byte > > DoUnWrap(uint8_t const *pInputData,
                                                                               uint32_t nDataLen) const noexcept = 0;

protected:
    /// @brief doUnwrapSeed
    /// @param wrappedSeed Wrapped seed
    /// @return Wrapping result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02018
    /// @needwork = dda
    /// @endcode
    virtual ara::core::Result< ara::core::Vector< ara::core::Byte > > DoUnwrap(
        ReadOnlyMemRegion const &wrappedSeed) const noexcept = 0;
    /// @brief Key wrapping.
    /// @param key the key need to wrapped
    /// @return Unwrapping result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_02019
    /// @needwork = dda
    /// @endcode
    virtual ara::core::Result< ara::core::Vector< ara::core::Byte > > DoWrapKeyMaterial(
        RestrictedUseObject const &key) const noexcept = 0;
};

}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYP_PUHUA_CTX_SYMMETRIC_KEY_WRAPPER_BASE_H_