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
/// @file       symmetric_key_wrapper_ctx.h
/// @brief      AutoSar-Crypto Encryption/Decryption module
/// @details    Context for symmetric key wrapping algorithms.
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
/// @module_path=/CRYPTO/Default Encryption/Decryption/Key Wrapping
/// @interface_level=software
/// @trace_id_sr=SR_CRYPTO_01018
/// @unit_name=SymmetricKeyWrapperCtx
/// @unit_description=Symmetric Key Wrapping Algorithm Context Base Class
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_SYMMETRIC_KEY_WRAPPER_CTX_H_
#define ARA_CRYPTO_CRYP_SYMMETRIC_KEY_WRAPPER_CTX_H_

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
/// @brief Context for symmetric key wrapping algorithms (for AES, it should be compatible with RFC3394 or RFC5649).
///     The public interface of this context is specifically for wrapping/unwrapping raw key material, i.e., without allocating any meta-information to the key material in the source crypto object.
///     However, beyond this, this context type should support some "hidden" low-level methods suitable for exporting/importing entire crypto objects.
///     Key wrapping of entire crypto objects (including associated meta-information) can be done via ExportSecuredObject() and ImportSecuredObject() methods, but does not comply with RFC3394 or RFC5649.
/// @brief Context of a symmetric key wrap algorithm (for AES it should be compatible with RFC3394 or RFC5649).
///     The public interface of this context is dedicated for raw key material wrapping/ unwrapping,
///     i.e. without any meta-information assigned to the key material in source crypto object.
///     But additionally this context type should support some "hidden" low-level methods suitable for whole crypto
///     object exporting/importing. Key Wrapping of a whole crypto object (including associated meta-information) can be
///     done by methods: ExportSecuredObject() and ImportSecuredObject(), but without compliance to RFC3394 or RFC5649.
/// @interface SymmetricKeyWrapperCtx
/// @AUTOSAR_SWS {SWS_CRYPT_24000}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02104}
/// @uptrace={RS_CRYPTO_02208}
//********************************/
/// @code{.isoft}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02484
/// @trace_id_dd=DD_CRYPTO_05272
/// @needwork = ad
/// @endcode
class SymmetricKeyWrapperCtx : public CryptoContext
{
public:
    /// @brief Unique smart pointer for the interface.
    /// @brief Unique smart pointer of the interface.
    /// @code{.isoft}
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_24001}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02201}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_03211
    /// @trace_id_dd=DD_CRYPTO_06423
    /// @needwork = ad
    /// @endcode
    using Uptr = std::unique_ptr< SymmetricKeyWrapperCtx >;

public:
    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=software
    /// @needwork = no
    /// @endcode
    SymmetricKeyWrapperCtx() noexcept = default;
    /// @brief Default destructor
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02485
    /// @trace_id_dd=DD_CRYPTO_05273
    /// @needwork = ad
    /// @endcode
    ~SymmetricKeyWrapperCtx() override = default;

    /// @brief Default copy assignment operator
    /// @brief Copy-assign another SymmetricKeyWrapperCtx to this instance.
    /// @param other Another instance of this class
    /// @return *this, containing the contents of @a other
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_30214}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02004}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02486
    /// @trace_id_dd=DD_CRYPTO_05274
    /// @needwork = ad
    /// @endcode
    SymmetricKeyWrapperCtx &operator=(SymmetricKeyWrapperCtx const &other) = delete;
    /// @brief Default move assignment operator
    /// @brief Move-assign another SymmetricKeyWrapperCtx to this instance.
    /// @param other Another instance of this class
    /// @return *this, containing the contents of @a other
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_30215}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02004}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02487
    /// @trace_id_dd=DD_CRYPTO_05275
    /// @needwork = ad
    /// @endcode
    SymmetricKeyWrapperCtx &operator=(SymmetricKeyWrapperCtx &&other) = delete;
    /// @brief Default copy constructor
    /// @param other Another instance of this class
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02488
    /// @trace_id_dd=DD_CRYPTO_05276
    /// @needwork = ad
    /// @endcode
    SymmetricKeyWrapperCtx(SymmetricKeyWrapperCtx const &other) noexcept = delete;
    /// @brief Default move constructor
    /// @param other Another instance of this class
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02489
    /// @trace_id_dd=DD_CRYPTO_05277
    /// @needwork = ad
    /// @endcode
    SymmetricKeyWrapperCtx(SymmetricKeyWrapperCtx &&other) noexcept = delete;

public:
    /// @brief Calculate the size of the wrapped key (in bytes) from the raw key length (in bits). This method is useful for implementations that differ from RFC3394 / RFC5649.
    /// @brief Calculate size of the wrapped key in bytes from original key length in bits.
    ///       This method can be useful for some implementations different from RFC3394 / RFC5649.
    /// @param keyLength Key length: byte unit
    /// @return size of the wrapped key in bytes
    /// @code{.isoft}
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_24013}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02201}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02490
    /// @trace_id_dd=DD_CRYPTO_05278
    /// @needwork = ad
    /// @endcode
    virtual std::size_t CalculateWrappedKeySize(std::size_t keyLength) const noexcept = 0;
    /// @brief Get the ExtensionService instance.
    /// @brief Get ExtensionService instance.
    /// @return
    /// @code{.isoft}
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_24002}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02006}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02491
    /// @trace_id_dd=DD_CRYPTO_05279
    /// @needwork = ad
    /// @endcode
    virtual ExtensionService::Uptr GetExtensionService() const noexcept = 0;
    /// @brief Get the maximum length of the target key supported by the implementation. This method is useful for implementations that differ from RFC3394 / RFC5649.
    /// @brief Get maximum length of the target key supported by the implementation.
    ///       This method can be useful for some implementations different from RFC3394 / RFC5649.
    /// @return maximum length of the target key in bits
    /// @code{.isoft}
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_24012}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02201}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02492
    /// @trace_id_dd=DD_CRYPTO_05280
    /// @needwork = ad
    /// @endcode
    virtual std::size_t GetMaxTarGetKeyLength() const noexcept = 0;
    /// @brief Get the expected granularity (block size) of the target key.
    ///         If the class implements RFC3394 (KW without padding), this method should return 8 (i.e., 8 bytes = 64 bits).
    ///         If the class implements RFC5649 (KW with padding), this method should return 1 (i.e., 1 octet = 8 bits).
    /// @brief Get expected granularity of the target key (block size).
    ///       If the class implements RFC3394 (KW without padding) then this method should return 8 (i.e. 8 octets = 64
    ///       bits). If the class implements RFC5649 (KW with padding) then this method should return 1 (i.e. 1 octet =
    ///       8 bits).
    /// @return size of the block in bytes
    /// @code{.isoft}
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_24011}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02201}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02493
    /// @trace_id_dd=DD_CRYPTO_05281
    /// @needwork = ad
    /// @endcode
    virtual std::size_t GetTargetKeyGranularity() const noexcept = 0;
    /// @brief Clear the encryption context.
    /// @brief Clear the crypto context.
    /// @return has value if Reset sucess false otherwise
    /// @code{.isoft}
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_24019}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02108}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02494
    /// @trace_id_dd=DD_CRYPTO_05282
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< void > Reset() noexcept = 0;
    /// @brief Set (deploy) a key to the symmetric key wrapper algorithm context.
    /// @brief Set (deploy) a key to the symmetric key wrapper algorithm context.
    /// @param key  the source key object
    /// @param transform  the "direction" indicator: deploy the key for direct transformation (if @c true) or
    /// for reverse one (if @c false)
    /// @return
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kIncompatibleObject     if the provided key object is incompatible with this
    /// symmetric key context
    /// @error: SecurityErrorDomain::kUsageViolation         if the transformation type associated with this context
    ///                (taking into account the direction specified by @c transform) is prohibited by the "allowed
    ///                usage" restrictions of provided key object
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_24018}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02001}
    /// @uptrace={RS_CRYPTO_02003}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02495
    /// @trace_id_dd=DD_CRYPTO_05283
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< void > SetKey(SymmetricKey const &key, CryptoTransform transform) noexcept = 0;
    /// @brief Perform a "key unwrap" operation on the provided BLOB and generate a key object of the expected type. See UnwrapKey() for more details.
    /// @brief Execute the "key unwrap" operation for provided BLOB and produce a %Key object of expected type. For
    /// additional details see @c UnwrapKey() .
    /// @param wrappedKey  a memory region that contains wrapped key
    /// @param algId  an identifier of the target symmetric crypto algorithm
    /// @param allowedUsage  bit-flags that define a list of allowed transformations' types in which the target key
    /// can be used
    /// @return unique smart pointer to @c ExpectedKey object, which keeps unwrapped key material
    /// @code{.isoft}
    /// @tparam ExpectedKey  the expected type of concrete key
    /// @error: SecurityErrorDomain::kInvalidInputSize       if the size of provided wrapped key is unsupported
    /// @error: SecurityErrorDomain::kUninitializedContext   if the context was not initialized by a key value
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_24017}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02115}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02496
    /// @trace_id_dd=DD_CRYPTO_05284
    /// @needwork = ad
    /// @endcode
    template < typename ExpectedKey >
    ara::core::Result< typename ExpectedKey::Uptrc > UnwrapConcreteKey(ReadOnlyMemRegion wrappedKey,
                                                                       AlgId algId,
                                                                       AllowedUsageFlags allowedUsage) noexcept
    {
        ara::core::Result< RestrictedUseObject::Uptrc > pRestrictedUseObjectResult{
            UnwrapKey(wrappedKey, algId, allowedUsage)};
        if (!pRestrictedUseObjectResult.HasValue()) {
            return ara::core::Result< typename ExpectedKey::Uptrc >::FromValue(nullptr);
        }
        RestrictedUseObject::Uptrc pRestrictedUseObject{std::move(pRestrictedUseObjectResult).Value()};
        ara::core::Result< typename ExpectedKey::Uptrc > pResultExpectedKey{
            std::move(RestrictedUseObject::Downcast< ExpectedKey >(std::move(pRestrictedUseObject)))};
        if (!pResultExpectedKey.HasValue()) {
            return ara::core::Result< typename ExpectedKey::Uptrc >::FromValue(nullptr);
        }
        typename ExpectedKey::Uptrc pExpectedKey{std::move(pResultExpectedKey).Value()};
        return ara::core::Result< typename ExpectedKey::Uptrc >::FromValue(std::move(pExpectedKey));
    }
    /// @brief Perform a "key unwrap" operation on the provided BLOB and generate a key object. If the implementation is based on AES block ciphers and applied to AES keys, this method should comply with RFC3394 or RFC5649.
    /// @brief Execute the "key unwrap" operation for provided BLOB and produce @c Key object.
    ///         This method should be compliant to RFC3394 or RFC5649, if implementation is based on the AES block
    ///         cipher and applied to an AES key. The created @c Key object has following attributes: session and
    ///         non-exportable (because it was imported without meta-information)!
    ///         @c SymmetricKey may be unwrapped in following way: SymmetricKey::Uptrc key = SymmetricKey::Cast(UnwrapKey(wrappedKey, ...));
    ///         @c PrivateKey may be unwrapped in following way: PrivateKey::Uptrc key =
    ///         PrivateKey::Cast(UnwrapKey(wrappedKey, ...)); In both examples the @c Cast() method may additionally @b
    ///         throw the @c BadObjectTypeException if an actual type of the unwrapped key differs from the target one!
    /// @param wrappedKey  a memory region that contains wrapped key
    /// @param algId  an identifier of the target symmetric crypto algorithm
    /// @param allowedUsage  bit-flags that define a list of allowed transformations' types in which the target key
    /// can be used
    /// @return unique smart pointer to @c Key object, which keeps unwrapped key material
    /// @param wrappedKey  a memory region that contains wrapped key
    /// @param algId  an identifier of the target symmetric crypto algorithm
    /// @param allowedUsage  bit-flags that define a list of allowed transformations' types in which the target key
    /// can be used
    /// @return unique smart pointer to @c Key object, which keeps unwrapped key material
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kInvalidInputSize       if the size of provided wrapped key is unsupported
    /// @error: SecurityErrorDomain::kUninitializedContext   if the context was not initialized by a key value
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_24016}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02115}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02497
    /// @trace_id_dd=DD_CRYPTO_05285
    /// @needwork = ad
    /// @endcode
    // PRQA S 2024 QAC /// @qac: False positive
    virtual ara::core::Result< RestrictedUseObject::Uptrc > UnwrapKey(
        ReadOnlyMemRegion const &wrappedKey, AlgId algId, AllowedUsageFlags allowedUsage) const noexcept = 0;
    // PRQA L:QAC

    /// @brief Perform a "key unwrap" operation on the provided BLOB and generate a SecretSeed object.
    ///         If the implementation is based on AES block ciphers and applied to AES key material, this method should comply with RFC3394 or RFC5649. The created SecretSeed object has the following properties: session and non-exportable (because it is imported without meta-information).
    /// @brief Execute the "key unwrap" operation for provided BLOB and produce @c SecretSeed object.
    ///       This method should be compliant to RFC3394 or RFC5649, if implementation is based on the AES block cipher
    ///       and applied to an AES key material. The created @c SecretSeed object has following attributes: session and
    ///       non-exportable (because it was imported without meta-information).
    /// @name  UnwrapSeed
    /// @param wrappedSeed  a memory region that contains wrapped seed
    /// @param targetAlgId  the target symmetric algorithm identifier also defines a target seed-length
    /// @param allowedUsage  allowed usage scope of the target seed
    /// @return unique smart pointer to @c SecretSeed object, which keeps unwrapped key material
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kInvalidInputSize       if the size of provided wrapped seed is unsupported
    /// @error: SecurityErrorDomain::kUninitializedContext   if the context was not initialized by a key value
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_24015}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02007}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02498
    /// @trace_id_dd=DD_CRYPTO_05286
    /// @needwork = ad
    /// @endcode
    // PRQA S 2024 QAC /// @qac: False positive
    virtual ara::core::Result< SecretSeed::Uptrc > UnwrapSeed(ReadOnlyMemRegion const &wrappedSeed,
                                                              AlgId targetAlgId,
                                                              SecretSeed::Usage allowedUsage) const noexcept = 0;
    // PRQA L:QAC
    /// @brief Perform a "key wrap" operation on the provided key material.
    ///         If an implementation is based on AES block ciphers and applied to AES keys, this method should comply with RFC3394 or RFC5649. The method CalculateWrappedKeySize() can be used to calculate the required output buffer size.
    /// @brief Execute the "key wrap" operation for the provided key material.
    ///       This method should be compliant to RFC3394 or RFC5649, if an implementation is based on the AES block
    ///       cipher and applied to an AES key. Method @c CalculateWrappedKeySize() can be used for size calculation of
    ///       the required output buffer.
    /// @param key  a key that should be wrapped
    /// @return
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kInsufficientCapacity   if the size of the @c wrapped buffer is not enough for
    /// storing the result
    /// @error: SecurityErrorDomain::kInvalidInputSize       if the @c key object has an unsupported length
    /// @error: SecurityErrorDomain::kUninitializedContext   if the context was not initialized by a key value
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_24014}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02201}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02499
    /// @trace_id_dd=DD_CRYPTO_05287
    /// @needwork = ad
    /// @endcode
    // PRQA S 2024 QAC /// @qac: False positive
    virtual ara::core::Result< ara::core::Vector< ara::core::Byte > > WrapKeyMaterial(
        RestrictedUseObject const &key) const noexcept = 0;
    // PRQA L:QAC

public:
};
//********************************/
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYP_SYMMETRIC_KEY_WRAPPER_CTX_H_
