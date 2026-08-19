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
/// @file       signer_private_ctx.h
/// @brief      AutoSar-Crypto Encryption/Decryption module
/// @details    Signature private key context interface.
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
/// @module_path=/CRYPTO/Default Encryption/Decryption/Signature and Verification
/// @interface_level=software
/// @trace_id_sr=SR_CRYPTO_01009
/// @unit_name=SignerPrivateCtx
/// @unit_description=Signature Private Key Context Base Class
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_SIGNER_PRIVATE_CTX_H_
#define ARA_CRYPTO_CRYP_SIGNER_PRIVATE_CTX_H_

#include "ara/core/result.h"
#include "ara/crypto/common/mem_region.h"
#include "ara/crypto/cryp/cryobj/private_key.h"
#include "ara/crypto/cryp/cryobj/signature.h"
#include "ara/crypto/cryp/crypto_context.h"
#include "ara/crypto/cryp/signature_service.h"

namespace ara {
namespace crypto {
namespace cryp {
//********************************/
/// @brief Signature private key context interface.
/// @brief Signature Private key Context interface.
/// @interface SignerPrivateCtx
/// @AUTOSAR_SWS {SWS_CRYPT_23500}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02204}
//********************************/
/// @code{.isoft}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02317
/// @trace_id_dd=DD_CRYPTO_05093
/// @needwork = ad
/// @endcode
class SignerPrivateCtx : public CryptoContext
{
public:
    /// @brief Unique smart pointer for the interface.
    /// @brief Unique smart pointer of the interface.
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_23501}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02204}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_03197
    /// @trace_id_dd=DD_CRYPTO_06409
    /// @needwork = ad
    /// @endcode
    using Uptr = std::unique_ptr< SignerPrivateCtx >;
    /// @brief Unique smart pointer for the constant interface.
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_03198
    /// @trace_id_dd=DD_CRYPTO_06410
    /// @needwork = ad
    /// @endcode
    using Uptrc = std::unique_ptr< SignerPrivateCtx const >;

    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02318
    /// @trace_id_dd=DD_CRYPTO_05094
    /// @needwork = ad
    /// @endcode
    SignerPrivateCtx() = default;
    /// @brief Default virtual destructor
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02319
    /// @trace_id_dd=DD_CRYPTO_05095
    /// @needwork = ad
    /// @endcode
    ~SignerPrivateCtx() noexcept override = default;
    /// @brief Default copy assignment operator
    /// @brief Copy-assign another SignerPrivateCtx to this instance.
    /// @param other Another instance of this class
    /// @return *this, containing the contents of @a other
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_30214}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02004}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02320
    /// @trace_id_dd=DD_CRYPTO_05096
    /// @needwork = ad
    /// @endcode
    SignerPrivateCtx &operator=(SignerPrivateCtx const &other) = delete;
    /// @brief Default move assignment operator
    /// @brief Move-assign another SignerPrivateCtx to this instance.
    /// @param other Another instance of this class
    /// @return *this, containing the contents of @a other
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_30215}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02004}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02321
    /// @trace_id_dd=DD_CRYPTO_05097
    /// @needwork = ad
    /// @endcode
    SignerPrivateCtx &operator=(SignerPrivateCtx &&other) = delete;
    /// @brief Default copy constructor
    /// @param other Another instance of this class
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02322
    /// @trace_id_dd=DD_CRYPTO_05098
    /// @needwork = ad
    /// @endcode
    SignerPrivateCtx(SignerPrivateCtx const &other) noexcept = delete;
    /// @brief Default move constructor
    /// @param other Another instance of this class
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02323
    /// @trace_id_dd=DD_CRYPTO_05099
    /// @needwork = ad
    /// @endcode
    SignerPrivateCtx(SignerPrivateCtx &&other) noexcept = delete;

public:
    /// @brief Get the SignatureService instance.
    /// @brief Get SignatureService instance.
    /// @return
    /// @code{.isoft}
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_23510}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02006}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02324
    /// @trace_id_dd=DD_CRYPTO_05100
    /// @needwork = ad
    /// @endcode
    virtual SignatureService::Uptr GetSignatureService() const noexcept = 0;
    /// @brief Clear the encryption context.
    /// @brief Clear the crypto context.
    /// @return has value if Reset sucess false otherwise
    /// @code{.isoft}
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_23516}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02108}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02325
    /// @trace_id_dd=DD_CRYPTO_05101
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< void > Reset() noexcept = 0;
    /// @brief Set (deploy) a key for the signer private algorithm context.
    /// @brief Set (deploy) a key to the signer private algorithm context.
    /// @param key  the source key object
    /// @return has value if SetKey sucess false otherwise
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kIncompatibleObject     if the provided key object is incompatible with this
    /// symmetric key context
    /// @error: SecurityErrorDomain::kUsageViolation         if the transformation type associated with this context is
    /// prohibited by    /// the "allowed usage" restrictions of provided key object
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_23515}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02001}
    /// @uptrace={RS_CRYPTO_02003}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02326
    /// @trace_id_dd=DD_CRYPTO_05102
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< void > SetKey(PrivateKey const &key) noexcept = 0;
    /// @brief Sign the provided digest value stored in the hash function context.
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kInvalidArgument        if hash-function algorithm does not comply with the
    /// signature algorithm specification of this context
    /// @error: SecurityErrorDomain::kInvalidInputSize       if the user supplied @c context has incorrect (or
    /// unsupported) size
    /// @error: SecurityErrorDomain::kProcessingNotFinished  if the method @c hash.Finish() was not called before the
    /// call of this method
    /// @error: SecurityErrorDomain::kUninitializedContext   this context was not initialized by a key value
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_23511}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02204}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02327
    /// @trace_id_dd=DD_CRYPTO_05103
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< Signature::Uptrc >
    /// @brief Sign a provided digest value stored in the hash-function context.
    ///       This method must put the hash-function algorithm ID and a @a COUID of the used key-pair to the resulting
    ///       signature object! The user supplied @c context may be used for such algorithms as: Ed25519ctx, Ed25519ph,
    ///       Ed448ph. If the target algorithm doesn't support the @c context argument then the empty (default) value
    ///       must be supplied!
    /// @param hashFn  a finalized hash-function context that contains a digest value ready for sign
    /// @param context  an optional user supplied "context" (its support depends from concrete algorithm)
    /// @return unique smart pointer to serialized signature
    SignPreHashed(HashFunctionCtx const &hashFn,
                  ReadOnlyMemRegion const &context = ReadOnlyMemRegion()) const noexcept = 0;
    /// @brief Sign the directly provided hash value or message value.
    ///         This method can be used to implement "multi-pass" signature algorithms that process messages directly, i.e., without the need for "pre-hashing" (such as Ed25519ctx).
    ///         This method is also suitable for implementing traditional pre-hashed signature schemes (such as Ed25519ph, Ed448ph, ECDSA). If the target algorithm does not support the context parameter, an empty value (default) must be provided!
    /// @brief Sign a directly provided hash or message value.
    ///         This method can be used for implementation of the "multiple passes" signature algorithms that process a
    ///         message directly, i.e. without "pre-hashing" (like Ed25519ctx). But also this method is suitable for
    ///         implementation of the traditional signature schemes with pre-hashing (like Ed25519ph, Ed448ph, ECDSA).
    ///         If the target algorithm doesn’t support the @c context argument then the empty (default) value must be
    ///         supplied!
    /// @param value  the (pre-)hashed or direct message value that should be signed
    /// @param context  an optional user supplied "context" (its support depends from concrete algorithm)
    /// @return actual size of the signature value stored to the output buffer
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kInvalidInputSize  if size of the input @c value or @c context arguments are
    /// incorrect / unsupported
    /// @error: SecurityErrorDomain::kUninitializedContext  if the context was not initialized by a key value
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_23512}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02204}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02328
    /// @trace_id_dd=DD_CRYPTO_05104
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< ara::core::Vector< ara::core::Byte > > Sign(ReadOnlyMemRegion const &value,
                                                                           ReadOnlyMemRegion const &context
                                                                           = ReadOnlyMemRegion()) const noexcept = 0;
    /// @brief Sign a directly provided digest value and create a signature object.
    /// @brief Sign a directly provided digest value and create the @c Signature object.
    ///       This method must put the hash-function algorithm ID and a @a COUID of the used key-pair to the resulting
    ///       signature object! The user supplied @c context may be used for such algorithms as: Ed25519ctx, Ed25519ph,
    ///       Ed448ph. If the target algorithm doesn't support the @c context argument then the empty (default) value
    ///       must be supplied!
    /// @param hashAlgId  hash function algorithm ID
    /// @param hashValue  hash function value (resulting digest without any truncations)
    /// @param context  an optional user supplied "context" (its support depends from concrete algorithm)
    /// @return unique smart pointer to serialized signature
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kInvalidArgument        if hash-function algorithm does not comply with the
    /// signature algorithm specification of this context
    /// @error: SecurityErrorDomain::kInvalidInputSize       if the user supplied @c context has incorrect (or
    /// unsupported) size
    /// @error: SecurityErrorDomain::kUninitializedContext   this context was not initialized by a key value
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_23513}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02204}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02329
    /// @trace_id_dd=DD_CRYPTO_05105
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< Signature::Uptrc > SignPreHashed(AlgId hashAlgId,
                                                                ReadOnlyMemRegion const &hashValue,
                                                                ReadOnlyMemRegion const &context
                                                                = ReadOnlyMemRegion()) const noexcept = 0;
    /// @brief Sign the directly provided hash value or message value.
    ///         This method can be used to implement "multi-pass" signature algorithms that process messages directly, i.e., without the need for "pre-hashing" (such as Ed25519ctx).
    ///         This method is also suitable for implementing traditional pre-hashed signature schemes (such as Ed25519ph, Ed448ph, ECDSA).
    ///         This method sets the size of the output container based on the actually saved values! If the target algorithm does not support the context parameter, an empty value (default) must be provided!
    /// @brief Sign a directly provided hash or message value.
    ///         This method can be used for implementation of the "multiple passes" signature algorithms that process a
    ///         message directly, i.e. without "pre-hashing" (like Ed25519ctx). But also this method is suitable for
    ///         implementation of the traditional signature schemes with pre-hashing (like Ed25519ph, Ed448ph, ECDSA).
    ///         This method sets the size of the output container according to actually saved value!
    ///         If the target algorithm doesn't support the @c context argument then the empty (default) value must be
    ///         supplied!
    /// @param value  the (pre-)hashed or direct message value that should be signed
    /// @param context  an optional user supplied "context" (its support depends from concrete algorithm)
    /// @return actual size of the signature value stored to the output buffer
    /// @code{.isoft}
    /// @tparam Alloc  a custom allocator type of the output container
    /// @error: SecurityErrorDomain::kInvalidInputSize       if size of the input @c value or @c context arguments are
    /// incorrect / unsupported
    /// @error: SecurityErrorDomain::kInsufficientCapacity   if capacity of the output @c signature container is not
    /// enough
    /// @error: SecurityErrorDomain::kUninitializedContext   if the context was not initialized by a key value
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_23514}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02204}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02330
    /// @trace_id_dd=DD_CRYPTO_05106
    /// @needwork = ad
    /// @endcode
    template < typename Alloc = VENDOR_IMPLEMENTATION_DEFINED >
    ara::core::Result< ByteVector< Alloc > > Sign(ReadOnlyMemRegion value,
                                                  ReadOnlyMemRegion context = ReadOnlyMemRegion()) const noexcept
    {
        ara::core::Result< ara::core::Vector< ara::core::Byte > > result = Sign(value, context);
        if (false == result.HasValue()) {
            return ara::core::Result< ByteVector< Alloc > >::FromError(result.Error());
        }
        ByteVector< Alloc > signature;
        signature.resize(result.Value().size());
        memcpy(core::data(signature), result.Value().data(), result.Value().size());
        return ara::core::Result< ByteVector< Alloc > >::FromValue(signature);
    }

public:
};
//********************************/
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYP_SIGNER_PRIVATE_CTX_H_
