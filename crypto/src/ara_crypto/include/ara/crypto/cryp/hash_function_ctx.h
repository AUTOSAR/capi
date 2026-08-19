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
/// @file       hash_function_ctx.h
/// @brief      AutoSar-Crypto Encryption/Decryption module
/// @details    Hash function interface.
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
/// @module_path=/CRYPTO/Default Encryption/Decryption/Hash Function
/// @interface_level=software
/// @trace_id_sr=SR_CRYPTO_01007
/// @unit_name=HashFunctionCtx
/// @unit_description=Hash Function Base Class
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_HASH_FUNCTION_CTX_H_
#define ARA_CRYPTO_CRYP_HASH_FUNCTION_CTX_H_

#include "ara/core/result.h"
#include "ara/crypto/common/base_id_types.h"
#include "ara/crypto/cryp/cryobj/secret_seed.h"
#include "ara/crypto/cryp/cryobj/signature.h"
#include "ara/crypto/cryp/crypto_context.h"
#include "ara/crypto/cryp/digest_service.h"

namespace ara {
namespace crypto {
namespace cryp {
//********************************/
/// @brief Hash function interface.
/// @brief Hash function interface.
/// @interface HashFunctionCtx
/// @AUTOSAR_SWS {SWS_CRYPT_21100}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02205}
//********************************/
/// @code{.isoft}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02468
/// @trace_id_dd=DD_CRYPTO_05256
/// @needwork = ad
/// @endcode
class HashFunctionCtx : public CryptoContext
{
public:
    /// @brief Unique smart pointer for the interface.
    /// @brief Unique smart pointer of the interface.
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_21101}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02205}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_03317
    /// @trace_id_dd=DD_CRYPTO_06564
    /// @needwork = ad
    /// @endcode
    using Uptr = std::unique_ptr< HashFunctionCtx >;

    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=software
    /// @needwork = no
    /// @endcode
    HashFunctionCtx() noexcept = default;

    /// @brief Default destructor
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02469
    /// @trace_id_dd=DD_CRYPTO_05257
    /// @needwork = ad
    /// @endcode
    ~HashFunctionCtx() override = default;
    /// @brief Default copy assignment operator
    /// @brief Copy-assign another HashFunctionCtx to this instance.
    /// @param other Another instance of this class
    /// @return *this, containing the contents of @a other
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_30214}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02004}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02470
    /// @trace_id_dd=DD_CRYPTO_05258
    /// @needwork = ad
    /// @endcode
    HashFunctionCtx &operator=(HashFunctionCtx const &other) = delete;
    /// @brief Default move assignment operator
    /// @brief Move-assign another HashFunctionCtx to this instance.
    /// @param other Another instance of this class
    /// @return *this, containing the contents of @a other
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_30215}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02004}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02471
    /// @trace_id_dd=DD_CRYPTO_05259
    /// @needwork = ad
    /// @endcode
    HashFunctionCtx &operator=(HashFunctionCtx &&other) = delete;
    /// @brief Default copy constructor
    /// @param other Another instance of this class
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02472
    /// @trace_id_dd=DD_CRYPTO_05260
    /// @needwork = ad
    /// @endcode
    HashFunctionCtx(HashFunctionCtx const &other) noexcept = delete;
    /// @brief Default move constructor
    /// @param other Another instance of this class
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02473
    /// @trace_id_dd=DD_CRYPTO_05261
    /// @needwork = ad
    /// @endcode
    HashFunctionCtx(HashFunctionCtx &&other) noexcept = delete;

public:
    /// @brief Complete the digest calculation and optionally generate a "signature" object. Only after calling this method can the digest be signed, verified, extracted, or compared.
    /// @brief Finish the digest calculation and optionally produce the "signature" object.
    ///       Only after call of this method the digest can be signed, verified, extracted or compared.
    /// @return unique smart pointer to created signature object, if <tt>(makeSignatureObject == true)</tt> or an empty
    /// Signature object if <tt>(makeSignatureObject == false)</tt>
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kProcessingNotStarted  if the digest calculation was not initiated by a call of the
    /// @c Start() method
    /// @error: SecurityErrorDomain::kInvalidUsageOrder  if the digest calculation has not started yet or not been
    /// updated at least once
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_21115}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02302}
    /// @uptrace={RS_CRYPTO_02205}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02474
    /// @trace_id_dd=DD_CRYPTO_05262
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< ara::core::Vector< ara::core::Byte > > Finish() noexcept = 0;
    /// @brief Get the DigestService instance.
    /// @brief Get DigestService instance.
    /// @return
    /// @code{.isoft}
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_21102}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02006}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02475
    /// @trace_id_dd=DD_CRYPTO_05263
    /// @needwork = ad
    /// @endcode
    virtual DigestService::Uptr GetDigestService() const noexcept = 0;
    /// @brief Get the requested part of the calculated digest.
    ///         The entire digest value is kept in the context until the next call to Start(), so any part of it can be extracted or verified again.
    ///         If (full_digest_size <= offset) then return_size = 0 bytes; Else return_size = min(output.size()，
    ///         (full_digest_size - offset)) bytes. This method can be implemented "inline" after the ara::core::memcpy() function is standardized.
    /// @brief Get requested part of calculated digest.
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
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_21116}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02205}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02476
    /// @trace_id_dd=DD_CRYPTO_05264
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< ara::core::Vector< ara::core::Byte > > GetDigest(std::size_t offset
                                                                                = 0U) const noexcept = 0;
    /// @brief Get the requested part of the calculated digest into a pre-allocated managed container. This method sets the size of the output container based on the actually saved values.
    ///         The entire digest value is kept in the context until the next call to Start(), so any part of it can be extracted or verified again.
    ///          If (full_digest_size <= offset) then return_size = 0 bytes; else return_size = min(output.capacity(),
    ///          (full_digest_size - offset)) bytes.
    /// @brief Get requested part of calculated digest to pre-reserved managed container.
    ///         This method sets the size of the output container according to actually saved value.
    ///         Entire digest value is kept in the context up to next call Start(), therefore any its part can be
    ///         extracted again or verified. If (full_digest_size <= offset) then return_size = 0 bytes; else
    ///         return_size = min(output.capacity(), (full_digest_size - offset)) bytes.
    /// @param offset  position of first byte of digest that should be placed to the output buffer
    /// @return number of digest bytes really stored to the output buffer (they are always <= @c output.size() and
    /// denoted below as @a return_size)
    /// @code{.isoft}
    /// @tparam Alloc  a custom allocator type of the output container
    /// @error: SecurityErrorDomain::kProcessingNotFinished  if the digest calculation was not finished by a call of the
    /// @c Finish() method
    /// @error: SecurityErrorDomain::kUsageViolation  if the buffered digest belongs to a MAC/HMAC/AE/AEAD context
    /// initialized by a key without @c kAllowSignature permission
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_21117}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02205}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_02477
    /// @trace_id_dd=DD_CRYPTO_05265
    /// @needwork = ad
    /// @endcode
    template < typename Alloc = VENDOR_IMPLEMENTATION_DEFINED >
    // PRQA S 2135 QAC /// @qac: AUTOSAR standard interface
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
    /// @brief Initialize the context for new data stream processing or generation (depending on the primitive) without an IV.
    /// @brief Initialize the context for a new data stream processing or generation (depending on the primitive)
    /// without IV.
    /// @return has value if start sucess false otherwise
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kMissingArgument    the configured hash function expected an IV
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_21118}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02302}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02478
    /// @trace_id_dd=DD_CRYPTO_05266
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< void > Start() noexcept = 0;
    /// @brief Initialize the context for new data stream processing or generation (depending on the primitive).
    /// If the IV size is larger than the maximum size supported by the algorithm, the implementation may use only the leading bytes of the sequence.
    /// @brief Initialize the context for a new data stream processing or generation (depending on the primitive).
    ///         If IV size is greater than maximally supported by the algorithm then an implementation may use the
    ///         leading bytes only from the sequence.
    /// @param iv  an optional Initialization Vector (IV) or "nonce" value
    /// @return has value if start sucess false otherwise
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kInvalidInputSize   if the size of provided IV is not supported (i.e. if it is not
    /// enough for the initialization)
    /// @error: SecurityErrorDomain::kUnsupported        if the base algorithm (or its current implementation)
    /// principally doesn't support the IV variation, but provided IV value is not empty, i.e. if <tt>(iv.empty() ==
    /// false)</tt>
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_21110}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02302}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02479
    /// @trace_id_dd=DD_CRYPTO_05267
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< void > Start(ReadOnlyMemRegion const &iv) noexcept = 0;
    /// @brief Initialize the context for new data stream processing or generation (depending on the primitive).
    /// If the IV size is larger than the maximum size supported by the algorithm, the implementation may use only the leading bytes of the sequence.
    /// @brief Initialize the context for a new data stream processing or generation (depending on the primitive).
    ///         If IV size is greater than maximally supported by the algorithm then an implementation may use the
    ///         leading bytes only from the sequence.
    /// @param iv  the Initialization Vector (IV) or "nonce" object
    /// @return has value if start sucess false otherwise
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kInvalidInputSize   if the size of provided IV is not supported (i.e. if it is not
    /// enough for the initialization)
    /// @error: SecurityErrorDomain::kUnsupported        if the base algorithm (or its current implementation)
    /// principally doesn't support the IV variation
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_21111}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02302}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02480
    /// @trace_id_dd=DD_CRYPTO_05268
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< void > Start(SecretSeed const &iv) noexcept = 0;
    /// @brief Update the digest calculation context with a new part of the message. This method is specialized for cases where the RestrictedUseObject is part of the "message".
    /// @brief Update the digest calculation context by a new part of the message.
    ///         This method is dedicated for cases then the @c RestrictedUseObject is a part of the "message".
    /// @param in  a part of input message that should be processed
    /// @return  has value if Update sucess false otherwise
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kProcessingNotStarted   if the digest calculation was not initiated by a call of the @c Start() method
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_21112}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02302}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02481
    /// @trace_id_dd=DD_CRYPTO_05269
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< void > Update(RestrictedUseObject const &in) noexcept = 0;
    /// @brief Update the digest calculation context with a new part of the message.
    /// @brief Update the digest calculation context by a new part of the message.
    /// @param in  a part of the input message that should be processed
    /// @return  has value if Update sucess false otherwise
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kProcessingNotStarted   if the digest calculation was not initiated by a call of the @c Start() method
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_21113}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02302}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02482
    /// @trace_id_dd=DD_CRYPTO_05270
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< void > Update(ReadOnlyMemRegion const &in) noexcept = 0;
    /// @brief Update the digest calculation context with a new part of the message. This method facilitates handling of constant tags.
    /// @brief Update the digest calculation context by a new part of the message.
    ///         This method is convenient for processing of constant tags.
    /// @param in  a byte value that is a part of input message
    /// @return  has value if Update sucess false otherwise
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kProcessingNotStarted  if the digest calculation was not initiated by a call of the @c Start() method
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_21114}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02302}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02483
    /// @trace_id_dd=DD_CRYPTO_05271
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< void > Update(std::uint8_t const &in) noexcept = 0;

public:
};
//********************************/
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYP_HASH_FUNCTION_CTX_H_
