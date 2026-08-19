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
/// @file       message_authn_code_ctx.h
/// @brief      AutoSar-Crypto Encryption/Decryption module
/// @details    Keyed message authentication code context interface (MAC/HMAC).
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
/// @module_path=/CRYPTO/Default Encryption/Decryption/Message Authentication Code
/// @interface_level=software
/// @trace_id_sr=SR_CRYPTO_01005
/// @unit_name=MessageAuthnCodeCtx
/// @unit_description=Keyed Message Authentication Code Context Base Class
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_MESSAGE_AUTHN_CODE_CTX_H_
#define ARA_CRYPTO_CRYP_MESSAGE_AUTHN_CODE_CTX_H_

#include "ara/core/result.h"
#include "ara/crypto/cryp/cryobj/secret_seed.h"
#include "ara/crypto/cryp/cryobj/signature.h"
#include "ara/crypto/cryp/cryobj/symmetric_key.h"
#include "ara/crypto/cryp/crypto_context.h"
#include "ara/crypto/cryp/digest_service.h"

namespace ara {
namespace crypto {
namespace cryp {
//********************************/
/// @brief Keyed message authentication code context interface definition (MAC/HMAC).
/// @brief Keyed Message Authentication Code Context interface definition (MAC/HMAC).
/// @interface MessageAuthnCodeCtx
/// @AUTOSAR_SWS {SWS_CRYPT_22100}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02203}
//********************************/
/// @code{.isoft}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02450
/// @trace_id_dd=DD_CRYPTO_05238
/// @needwork = ad
/// @endcode
class MessageAuthnCodeCtx : public CryptoContext
{
public:
    /// @brief Unique smart pointer for the interface.
    /// @brief Unique smart pointer of the interface.
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_22101}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02203}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_03210
    /// @trace_id_dd=DD_CRYPTO_06422
    /// @needwork = ad
    /// @endcode
    using Uptr = std::unique_ptr< MessageAuthnCodeCtx >;

    /// @brief Constructor
    /// @code{.isoft}
    /// @interface_level=software
    /// @needwork = no
    /// @endcode
    MessageAuthnCodeCtx() noexcept = default;
    /// @brief Default virtual destructor
    /// @brief Destructor.
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_20401}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02008}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02451
    /// @trace_id_dd=DD_CRYPTO_05239
    /// @needwork = ad
    /// @endcode
    ~MessageAuthnCodeCtx() noexcept override = default;
    /// @brief Default copy constructor
    /// @param other Another instance of this class
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02452
    /// @trace_id_dd=DD_CRYPTO_05240
    /// @needwork = ad
    /// @endcode
    MessageAuthnCodeCtx(MessageAuthnCodeCtx const &other) noexcept = delete;
    /// @brief Default move constructor
    /// @param other Another instance of this class
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02453
    /// @trace_id_dd=DD_CRYPTO_05241
    /// @needwork = ad
    /// @endcode
    MessageAuthnCodeCtx(MessageAuthnCodeCtx &&other) noexcept = delete;
    /// @brief Default copy assignment operator
    /// @brief Copy-assign another MessageAuthnCodeCtx to this instance.
    /// @param other Another instance of this class
    /// @return *this, containing the contents of @a other
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_30214}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02004}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02454
    /// @trace_id_dd=DD_CRYPTO_05242
    /// @needwork = ad
    /// @endcode
    MessageAuthnCodeCtx &operator=(MessageAuthnCodeCtx const &other) = delete;
    /// @brief Default move assignment operator
    /// @brief Move-assign another MessageAuthnCodeCtx to this instance.
    /// @param other Another instance of this class
    /// @return *this, containing the contents of @a other
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_30215}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02004}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02455
    /// @trace_id_dd=DD_CRYPTO_05243
    /// @needwork = ad
    /// @endcode
    MessageAuthnCodeCtx &operator=(MessageAuthnCodeCtx &&other) = delete;

public:
    /// @brief Check the calculated digest against the expected "signature" object.
    /// The entire digest value is kept in the context until the next call to Start(), so it can be verified or extracted again. This method can be implemented "inline" after the ara::core::memcmp() function is standardized.
    /// @brief Check the calculated digest against an expected "signature" object.
    ///       Entire digest value is kept in the context up to next call @c Start(), therefore it can be verified again
    ///       or extracted. This method can be implemented as "inline" after standartization of function @c
    ///       ara::core::memcmp().
    /// @param expected  the signature object containing an expected digest value
    /// @return @c true if value and meta-information of the provided "signature" object is identical to calculated
    ///          digest and current configuration of the context respectively; but @c false otherwise
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kProcessingNotFinished  if the digest calculation was not finished by a call of the
    ///         @c Finish() method
    /// @error: SecurityErrorDomain::kIncompatibleObject     if the provided "signature" object was produced by another
    ///         crypto primitive type
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_22119}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02203}
    /// @uptrace={RS_CRYPTO_02204}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02456
    /// @trace_id_dd=DD_CRYPTO_05244
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< bool > Check(Signature const &expected) const noexcept = 0;
    /// @brief 2011 standard interface
    /// @brief Complete the digest calculation and optionally generate a "signature" object. Only after calling this method can the digest be signed, verified, extracted, or compared!
    ///         If the signature object is produced by a keyed MAC/HMAC/AE/AEAD algorithm, the dependent COUID of the "signature" should be set to the COUID of the symmetric key used.
    /// @brief Finish the digest calculation and optionally produce the "signature" object.
    ///         Only after call of this method the digest can be signed, verified, extracted or compared!
    ///         If the signature object produced by a keyed MAC/HMAC/AE/AEAD algorithm then the dependence COUID of the
    ///         "signature" should be set to COUID of used symmetric key.
    /// @param makeSignatureObject  if this argument is @c true then the method will also produce the signature object
    /// @return    unique smart pointer to created signature object, if(makeSignatureObject == true) or nullptr
    /// if(makeSignatureObject == false)
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kProcessingNotStarted  if the digest calculation was not initiated by a call of the
    /// @c Start() method
    /// @error: SecurityErrorDomain::kUsageViolation  if the buffered digest belongs to a MAC/HMAC/AE/AEAD context
    /// initialized by a key without @c kAllowSignature permission, but (makeSignatureObject == true)
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_22115}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02302}
    /// @uptrace={RS_CRYPTO_02203}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02457
    /// @trace_id_dd=DD_CRYPTO_05245
    /// @needwork = ad
    /// @endcode
    // PRQA S 2024 QAC /// @qac: False positive
    virtual ara::core::Result< Signature::Uptrc > Finish(bool makeSignatureObject) noexcept = 0;
    // PRQA L:QAC

    /// @brief 2311 standard interface
    /// @brief Complete the digest calculation and optionally generate a "signature" object. Only after calling this method can the digest be signed, verified, extracted, or compared!
    ///         If the signature object is produced by a keyed MAC/HMAC/AE/AEAD algorithm, the dependent COUID of the "signature" should be set to the COUID of the symmetric key used.
    /// @brief Finish the digest calculation and optionally produce the "signature" object.
    ///         Only after call of this method the digest can be signed, verified, extracted or compared!
    ///         If the signature object produced by a keyed MAC/HMAC/AE/AEAD algorithm then the dependence COUID of the
    ///         "signature" should be set to COUID of used symmetric key.
    /// @return ara::core::Result< void >
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kProcessingNotStarted  if the digest calculation was not initiated by a call of the @c Start() method
    /// @error: SecurityErrorDomain::kUsageViolation  if the buffered digest belongs to a MAC/HMAC/AE/AEAD context
    /// initialized by a key without @c kAllowSignature permission, but (makeSignatureObject == true)
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_22115}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02302}
    /// @uptrace={RS_CRYPTO_02203}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_
    /// @trace_id_dd=DD_CRYPTO_
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< void > Finish() noexcept = 0;
    /// @brief Get the DigestService instance.
    /// @brief Get DigestService instance.
    /// @return
    /// @code{.isoft}
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_22102}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02006}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02458
    /// @trace_id_dd=DD_CRYPTO_05246
    /// @needwork = ad
    /// @endcode
    virtual DigestService::Uptr GetDigestService() const noexcept = 0;
    /// @brief Get the requested part of the calculated digest into an existing memory buffer.
    /// The entire digest value is kept in the context until the next call to Start(), so any part of it can be extracted or verified again.
    ///         If (full_digest_size <= offset) then return_size = 0 bytes; else return_size = min(output.size(),
    ///         (full_digest_size - offset)) bytes. This method can be implemented "inline" after the ara::core::memcpy() function is standardized.
    /// @brief Get requested part of calculated digest to existing memory buffer.
    ///         Entire digest value is kept in the context up to next call @c Start(), therefore any its part can be
    ///         extracted again or verified. If (full_digest_size <= offset) then return_size = 0 bytes; else
    ///         return_size = min(output.size(), (full_digest_size - offset)) bytes. This method can be implemented as
    ///         "inline" after standartization of function @c ara::core::memcpy().
    /// @param offset  position of the first byte of digest that should be placed to the output buffer
    /// @return number of digest bytes really stored to the output buffer (they are always <= @c output.size() and
    /// denoted below as @a return_size)
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kProcessingNotFinished  if the digest calculation was not finished by a call of the
    /// @c Finish() method
    /// @error: SecurityErrorDomain::kUsageViolation         if the buffered digest belongs to a MAC/HMAC/AE/AEAD
    /// context initialized by a key without @c kAllowSignature permission
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_22116}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02203}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02459
    /// @trace_id_dd=DD_CRYPTO_05247
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< ara::core::Vector< ara::core::Byte > > GetDigest(std::size_t offset
                                                                                = 0U) const noexcept = 0;
    /// @brief Get the requested part of the calculated digest into a pre-allocated managed container.
    /// This method sets the size of the output container based on the actually saved values. The entire digest value is kept in the context until the next call to Start(), so any part of it can be extracted or verified again.
    ///         If (full_digest_size <= offset) then return_size = 0 bytes; else return_size = min(output.capacity(),
    ///         (full_digest_size - offset)) bytes.
    /// @brief Get requested part of calculated digest to pre-reserved managed container.
    ///       This method sets the size of the output container according to actually saved value.
    ///       Entire digest value is kept in the context up to next call @c Start(), therefore any its part can be
    ///       extracted again or verified. If (full_digest_size <= offset) then return_size = 0 bytes; else return_size
    ///       = min(output.capacity(), (full_digest_size - offset)) bytes.
    /// @param offset  position of first byte of digest that should be placed to the output buffer
    /// @return number of digest bytes really stored to the output buffer (they are always <= @c output.size() and
    /// denoted below as @a return_size)
    /// @code{.isoft}
    /// @tparam Alloc  a custom allocator type of the output container
    /// @error: SecurityErrorDomain::kProcessingNotFinished  if the digest calculation was not finished by a call of the
    /// @c Finish() method
    /// @error: SecurityErrorDomain::kUsageViolation         if the buffered digest belongs to a MAC/HMAC/AE/AEAD
    /// context initialized by a key without @c kAllowSignature permission
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_22117}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02203}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02460
    /// @trace_id_dd=DD_CRYPTO_05248
    /// @needwork = ad
    /// @endcode
    template < typename Alloc = VENDOR_IMPLEMENTATION_DEFINED >
    // PRQA S 2135 QAC  /// @qac: AUTOSAR standard interface
    ara::core::Result< ByteVector< Alloc > > GetDigest(std::size_t offset = 0U) const noexcept
    // PRQA L:QAC
    {
        ara::core::Result< ara::core::Vector< ara::core::Byte > > result = GetDigest(offset);
        if (false == result.HasValue()) {
            return ara::core::Result< ByteVector< Alloc > >::FromError(result.Error());
        }
        ByteVector< Alloc > output;
        output.resize(result.Value().size());
        memcpy(core::data(output), result.Value().data(), result.Value().size());
        return ara::core::Result< ByteVector< Alloc > >::FromValue(output);
    }
    /// @brief Clear the encryption context.
    /// @brief Clear the crypto context.
    /// @return
    /// @code{.isoft}
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_22120}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02108}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02461
    /// @trace_id_dd=DD_CRYPTO_05249
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< void > Reset() noexcept = 0;
    /// @brief Set (deploy) a key for the message authentication code algorithm context.
    /// @brief Set (deploy) a key to the message authn code algorithm context.
    /// @param key  the source key object
    /// @param transform  the "direction" indicator: deploy the key for direct transformation (if @c true) or
    /// for reverse one (if @c false)
    /// @return  has value if SetKey sucess false otherwise
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kIncompatibleObject     if the provided key object is incompatible with this
    /// symmetric key context
    /// @error: SecurityErrorDomain::kUsageViolation         if the transformation type associated with this context
    /// (taking into account the direction specified by @c transform) is prohibited by the "allowed usage" restrictions
    /// of provided key object
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_22118}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02001}
    /// @uptrace={RS_CRYPTO_02003}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02462
    /// @trace_id_dd=DD_CRYPTO_05250
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< void > SetKey(SymmetricKey const &key,
                                             CryptoTransform transform = CryptoTransform::kMacGenerate) noexcept = 0;
    /// @brief Initialize the context for new data stream processing or generation (depending on the primitive).
    /// If the IV size is larger than the maximum size supported by the algorithm, the implementation may use only the leading bytes of the sequence.
    /// @brief Initialize the context for a new data stream processing or generation (depending from the primitive).
    ///       If IV size is greater than maximally supported by the algorithm then an implementation may use the
    ///       leading bytes only from the sequence.
    /// @param iv  an optional Initialization Vector (IV) or "nonce" value
    /// @return  has value if Start sucess false otherwise
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kUninitializedContext  if the context was not initialized by deploying a key
    /// @error: SecurityErrorDomain::kInvalidInputSize  if the size of provided IV is not supported (i.e. if it is not
    /// enough for the initialization)
    /// @error: SecurityErrorDomain::kUnsupported  if the base algorithm (or its current implementation) principally
    /// doesn't support the IV variation, but provided IV value is not empty, i.e. if <tt>(iv.empty() == false)</tt>
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_22110}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02302}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02463
    /// @trace_id_dd=DD_CRYPTO_05251
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< void > Start(ReadOnlyMemRegion const &iv = ReadOnlyMemRegion()) noexcept = 0;
    /// @brief Initialize the context for new data stream processing or generation (depending on the primitive).
    /// If the IV size is larger than the maximum size supported by the algorithm, the implementation may use only the leading bytes of the sequence.
    /// @brief Initialize the context for a new data stream processing or generation (depending from the primitive).
    ///       If IV size is greater than maximally supported by the algorithm then an implementation may use the
    ///       leading bytes only from the sequence.
    /// @param iv  the Initialization Vector (IV) or "nonce" object
    /// @return  has value if Start sucess false otherwise
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kUninitializedContext   if the context was not initialized by deploying a key
    /// @error: SecurityErrorDomain::kInvalidInputSize       if the size of provided IV is not supported (i.e. if it is
    /// not enough for the initialization)
    /// @error: SecurityErrorDomain::kUnsupported            if the base algorithm (or its current implementation)
    /// principally doesn't support the IV variation
    /// @error: SecurityErrorDomain::kUsageViolation         if this transformation type is prohibited by the "allowed
    /// usage" restrictions of the provided @c SecretSeed object
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_22111}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02302}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02464
    /// @trace_id_dd=DD_CRYPTO_05252
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< void > Start(SecretSeed const &iv) noexcept = 0;
    /// @brief Update the digest calculation context with a new part of the message. This method is specialized for cases where the RestrictedUseObject is part of the "message".
    /// @brief Update the digest calculation context by a new part of the message.
    ///       This method is dedicated for cases then the @c RestrictedUseObject is a part of the "message".
    /// @param in  a part of input message that should be processed
    /// @return
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kProcessingNotStarted  if the digest calculation was not initiated by a call of the
    /// @c Start() method
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_22112}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02302}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02465
    /// @trace_id_dd=DD_CRYPTO_05253
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< void > Update(RestrictedUseObject const &in) noexcept = 0;
    /// @brief Update the digest calculation context with a new part of the message.
    /// @brief Update the digest calculation context by a new part of the message.
    /// @param in  a part of the input message that should be processed
    /// @return  has value if Update sucess false otherwise
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kProcessingNotStarted   if the digest calculation was not initiated by a call of
    /// the
    /// @c Start() method
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_22113}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02302}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02466
    /// @trace_id_dd=DD_CRYPTO_05254
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< void > Update(ReadOnlyMemRegion const &in) noexcept = 0;
    /// @brief Update the digest calculation context with a new part of the message. This method facilitates handling of constant tags.
    /// @brief Update the digest calculation context by a new part of the message.
    ///       This method is convenient for processing of constant tags.
    /// @param in  a byte value that is a part of input message
    /// @return  has value if Update sucess false otherwise
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kProcessingNotStarted  if the digest calculation was not initiated by a call of the
    /// @c Start() method
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_22114}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02302}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02467
    /// @trace_id_dd=DD_CRYPTO_05255
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< void > Update(std::uint8_t const &in) noexcept = 0;
    /// @brief Create a signature object for this MAC. The dependent COUID of this object should be set to the symmetric key used to generate the MAC.
    /// @brief Create a Signature object for this MAC.The dependence COUID of this object shall be set to the
    ///        symmetric key used to generate the MAC.
    /// @return   unique smart pointer to serialized signature
    /// @code{.isoft}
    /// @error:  ara::crypto::CryptoErrc::kProcessingNotFinished  if the MAC calculation was not completed by a call of the Finish() method
    /// @c MakeSignature() method
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_40987}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02302,RS_CRYPTO_02203}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_04354
    /// @trace_id_dd=DD_CRYPTO_08789
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< Signature::Uptrc > MakeSignature() noexcept = 0;

public:
};
//********************************/
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYP_MESSAGE_AUTHN_CODE_CTX_H_
