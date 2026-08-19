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
/// @file       auth_cipher_ctx.h
/// @brief      AutoSar-Crypto Encryption/Decryption module
/// @details    Generic authenticated cipher context
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
/// @module_path=/CRYPTO/Default Encryption/Decryption/Authenticated Encryption
/// @interface_level=software
/// @trace_id_sr=SR_CRYPTO_01011
/// @unit_name=AuthCipherCtx
/// @unit_description=Authenticated Encryption Base Class
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_AUTH_CIPHER_CTX_H_
#define ARA_CRYPTO_CRYP_AUTH_CIPHER_CTX_H_

#include "ara/core/result.h"
#include "ara/crypto/cryp/block_service.h"
#include "ara/crypto/cryp/cryobj/secret_seed.h"
#include "ara/crypto/cryp/cryobj/signature.h"
#include "ara/crypto/cryp/cryobj/symmetric_key.h"
#include "ara/crypto/cryp/crypto_context.h"

namespace ara {
namespace crypto {
namespace cryp {
//********************************/
/// @brief Generic authenticated cipher context interface.
///     Methods from the BufferedDigest interface are used to verify associated public data.
///     Methods from the StreamCipherCtx interface are used to encrypt/decrypt and verify the confidential part of the message. Data processing must be performed in the following order:
///     Call one of the Start() methods.
///     Process all relevant public data via the Update() method.
///     Process the confidential part of the message by calling ProcessBlocks(), ProcessBytes() (and optionally FinishBytes()).
///     Call the Finish() method to complete the authentication code calculation (and optionally obtain it).
///     A copy of the calculated MAC can be extracted (via GetDigest()) or compared internally (via Compare()).
///     The receiver must not use the decrypted data until the entire decryption and authentication process is complete! i.e., decrypted data should only be used after successful MAC verification!
/// @interface AuthCipherCtx
/// @brief Generalized Authenticated Cipher Context interface.
///     Methods of the derived interface @c BufferedDigest are used for authentication of associated public data.
///     Methods of the derived interface @c StreamCipherCtx are used for encryption/decryption and authentication of
///     onfidential part of message. The data processing must be executed in following order:
///     1. Call one of the @c Start() methods.
///     2. Process all associated public data via calls of @c Update() methods.
///     3. Process the confidential part of the message via calls of @c ProcessBlocks(), @c ProcessBytes() (and
///     optionally @c FinishBytes()) methods.
///     4. Call the @c Finish() method due to finalize the authentication code calculation (and get it optionally).
///     5. Copy of the calculated MAC may be extracted (by @c GetDigest()) or compared internally (by @c Compare()).
///     Receiver side should not use decrypted data before finishing of the whole decryption and authentication
///     process! I.e. decrypted data can be used only after successful MAC verification!
///     @AUTOSAR_SWS {SWS_CRYPT_20100}
///     @tracestatus={draft}
///     @uptrace{RS_CRYPTO_02207}
//********************************/
/// @code{.isoft}
/// @interface_level=software
/// @trace_id_ad=AD_CRYPTO_02145
/// @trace_id_dd=DD_CRYPTO_04916
/// @needwork = ad
/// @endcode
class AuthCipherCtx : public CryptoContext
{
public:
    /// @brief Unique smart pointer for the interface.
    /// @brief Unique smart pointer of the interface.
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_20101}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02207}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02145
    /// @trace_id_dd=DD_CRYPTO_06399
    /// @needwork = dd
    /// @endcode
    using Uptr = std::unique_ptr< AuthCipherCtx >;

    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02146
    /// @trace_id_dd=DD_CRYPTO_04917
    /// @needwork = ad
    /// @endcode
    AuthCipherCtx() noexcept = default;
    /// @brief Default virtual destructor
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02147
    /// @trace_id_dd=DD_CRYPTO_04918
    /// @needwork = ad
    /// @endcode
    ~AuthCipherCtx() noexcept override = default;
    /// @brief Default copy assignment operator
    /// @brief Copy-assign another AuthCipherCtx to this instance.
    /// @param other Another instance of this class
    /// @return *this, containing the contents of @a other
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_30214}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02004}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02148
    /// @trace_id_dd=DD_CRYPTO_04919
    /// @needwork = ad
    /// @endcode
    AuthCipherCtx &operator=(AuthCipherCtx const &other) = delete;
    /// @brief Default move assignment operator
    /// @brief Move-assign another AuthCipherCtx to this instance.
    /// @param other Another instance of this class
    /// @return *this, containing the contents of @a other
    /// @code{.isoft}
    /// @trace_id_sws={SWS_CRYPT_30215}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02004}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02149
    /// @trace_id_dd=DD_CRYPTO_04920
    /// @needwork = ad
    /// @endcode
    AuthCipherCtx &operator=(AuthCipherCtx &&other) = delete;
    /// @brief Default copy constructor
    /// @param other Another instance of this class
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02150
    /// @trace_id_dd=DD_CRYPTO_04921
    /// @needwork = ad
    /// @endcode
    AuthCipherCtx(AuthCipherCtx const &other) noexcept = delete;
    /// @brief Default move constructor
    /// @param other Another instance of this class
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02151
    /// @trace_id_dd=DD_CRYPTO_04922
    /// @needwork = ad
    /// @endcode
    AuthCipherCtx(AuthCipherCtx &&other) noexcept = delete;

public:
    /// @brief
    /// Check the calculated digest against the expected "signature" object. The entire digest value is kept in the context until the next call to Start(), so it can be verified or extracted again.
    ///         This method can be implemented "inline" after the ara::core::memcmp() function is standardized.
    ///          digest and current configuration of the context respectively; but @c false otherwise
    ///       @error: SecurityErrorDomain::kProcessingNotFinished  if the digest calculation was not finished by
    ///       a call of the @c Finish() method
    ///       @error: SecurityErrorDomain::kIncompatibleObject  if the provided "signature" object was produced by
    ///       another crypto primitive type
    /// @code{.isoft}
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_20319}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02203}
    /// @uptrace={RS_CRYPTO_02204}
    ///       @threadsafety{Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02152
    /// @trace_id_dd=DD_CRYPTO_04923
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< bool >
    /// @brief Check the calculated digest against an expected "signature" object.
    ///       Entire digest value is kept in the context up to next call @c Start(), therefore it can be
    ///       verified again or extracted.
    ///       This method can be implemented as "inline" after standartization of function @c ara::core::memcmp().
    /// @param expected  the signature object containing an expected digest value
    /// @name  Check
    /// @return @c true if value and meta-information of the provided "signature" object is identical to calculated
    Check(Signature const &expected) const noexcept = 0;
    /// @brief Get the BlockService instance.
    /// @return BlockService instance
    /// @brief Get BlockService instance.
    /// @code{.isoft}
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_20102}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02006}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02153
    /// @trace_id_dd=DD_CRYPTO_04924
    /// @needwork = ad
    /// @endcode
    virtual BlockService::Uptr GetBlockService() const noexcept = 0;
    /// @brief Get the digest
    /// @param offset position of the first byte of digest that should be placed to the output buffer
    /// @return Digest information
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02154
    /// @trace_id_dd=DD_CRYPTO_04925
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< ara::core::Vector< ara::core::Byte > > GetDigest(std::size_t offset
                                                                                = 0U) const noexcept = 0;
    /// @brief Get the transformation type configured for this context: kEncrypt or kDecrypt
    /// @brief Get the kind of transformation configured for this context: kEncrypt or kDecrypt
    /// @return ara::core::Result @c CryptoTransform
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kUninitializedContext  if the transformation direction of this context
    ///                is configurable during an initialization, but the context was not initialized yet
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_21715}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02156
    /// @trace_id_dd=DD_CRYPTO_04927
    /// @needwork = ad
    /// @endcode
    // PRQA S 2024 QAC /// @qac: False positive
    virtual ara::core::Result< CryptoTransform > GetTransformation() const noexcept = 0;
    // PRQA L:QAC
    /// @brief Get the maximum supported size of associated public data.
    /// @brief Get maximal supported size of associated public data.
    /// @return maximal supported size of associated public data in bytes
    /// @code{.isoft}
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_20103}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02157
    /// @trace_id_dd=DD_CRYPTO_04928
    /// @needwork = ad
    /// @endcode
    virtual std::uint64_t GetMaxAssociatedDataSize() const noexcept = 0;
    /// @brief Process confidential data.
    ///         The input buffer will be overwritten with the processed message. This function is the final call, i.e., all relevant data must have been provided.
    ///         Therefore, this function will check the authentication tag and only return processed data if the tag is valid.
    /// @brief Process confidential data.
    ///         The input buffer will be overwritten by the processed message.
    ///         This function is the final call, i.e. all associated data must have been already provided.
    ///         Hence, the function will check the authentication tag and only return the processed data, if the tag is
    ///         valid.
    /// @param in the input buffer containing the full message
    /// @param expectedTag  pointer to read only mem region
    /// @return @c ara::core::Vector<ara::core::Byte>
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kInvalidInputSize  if size of the input buffer is not divisible by the block size
    /// (see @c GetBlockSize())
    /// @error: SecurityErrorDomain::kProcessingNotStarted  if the data processing was not started by a call of the @c
    /// Start() method
    /// @error: SecurityErrorDomain::kAuthTagNotValid  if the processed data cannot be authenticated
    /// virtual ara::core::Result<ara::core::Vector<ara::core::Byte> > ProcessConfidentialData(ReadOnlyMemRegion in,
    /// ReadOnlyMemRegion expectedTag = nullptr ) noexcept = 0;
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_23634}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02302}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02158
    /// @trace_id_dd=DD_CRYPTO_04929
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< ara::core::Vector< ara::core::Byte > > ProcessConfidentialData(
        ReadOnlyMemRegion const &in, ReadOnlyMemRegion const &expectedTag = ReadOnlyMemRegion()) noexcept = 0;
    /// @brief Process confidential data.
    ///         The input buffer will be overwritten with the processed message. After calling this method, no more associated data will be updated.
    /// @brief Process confidential data.
    ///         The input buffer will be overwritten by the processed message
    ///         After this method is called no additional associated data may be updated.
    /// @param inOut  the input buffer containing the full message
    /// @param expectedTag  pointer to read only mem region
    /// @return @c void
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kInvalidInputSize       if size of the input buffer is not divisible by the block
    /// size (see @c GetBlockSize())
    /// @error: SecurityErrorDomain::kProcessingNotStarted   if the data processing was not started by a call of the @c
    /// Start() method
    /// @error: SecurityErrorDomain::kAuthTagNotValid        if the processed data cannot be authenticated
    /// virtual ara::core::Result<void> ProcessConfidentialData(ReadWriteMemRegion inOut, ReadOnlyMemRegion expectedTag
    /// = nullptr) noexcept = 0;
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_23635}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02302}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02159
    /// @trace_id_dd=DD_CRYPTO_04930
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< void > ProcessConfidentialData(ReadWriteMemRegion &inOut,
                                                              ReadOnlyMemRegion const &expectedTag
                                                              = ReadOnlyMemRegion()) noexcept = 0;
    /// @brief Clear the encryption context.
    /// @brief Clear the crypto context.
    /// @return
    /// @code{.isoft}
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_20414}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02108}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02160
    /// @trace_id_dd=DD_CRYPTO_04931
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< void > Reset() noexcept = 0;
    /// @brief Set (deploy) a key for the authenticated cipher symmetric algorithm context.
    /// @brief Set (deploy) a key to the authenticated cipher symmetric algorithm context.
    /// @param key  the source key object
    /// @param transform  the "direction" indicator: deploy the key for encryption (if @c true) or for
    /// decryption (if @c false)
    /// @return @c void has value if SetKey sucess ,flase otherwise
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kIncompatibleObject     if the provided key object is incompatible with this
    /// symmetric key context
    /// @error: SecurityErrorDomain::kUsageViolation         if the transformation type associated with this context
    ///         (taking into account the direction specified by @c transform) is prohibited by the "allowed usage"
    ///         restrictions of provided key object
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_23911}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02001}
    /// @uptrace={RS_CRYPTO_02003}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02161
    /// @trace_id_dd=DD_CRYPTO_04932
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< void > SetKey(SymmetricKey const &key,
                                             CryptoTransform transform = CryptoTransform::kEncrypt) noexcept = 0;
    /// @brief Initialize the context for new data processing or generation (depending on the primitive).
    /// If the IV size is larger than the maximum size supported by the algorithm, the implementation may use only the leading bytes of the sequence.
    /// @brief Initialize the context for a new data processing or generation (depending from the primitive).
    ///         If IV size is greater than maximally supported by the algorithm then an implementation may use the
    ///         leading bytes only from the sequence.
    /// @param iv  an optional Initialization Vector (IV) or "nonce" value
    /// @return @c void has value if Start sucess ,flase otherwise
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kUninitializedContext   if the context was not initialized
    /// @error: SecurityErrorDomain::kInvalidInputSize       if the size of provided IV is not supported (i.e. if it is
    /// not enough for the initialization)
    /// @error: SecurityErrorDomain::kUnsupported            if the base algorithm (or its current implementation)
    /// principally doesn't support the IV variation,
    ///                 but provided IV value is not empty, i.e. if <tt>(iv.empty() == false)
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_24714}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02302}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02162
    /// @trace_id_dd=DD_CRYPTO_04933
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< void > Start(ReadOnlyMemRegion const &iv = ReadOnlyMemRegion()) noexcept = 0;
    /// @brief Initialize the context for new data processing or generation (depending on the primitive).
    /// If the IV size is larger than the maximum size supported by the algorithm, the implementation may use only the leading bytes of the sequence.
    /// @brief Initialize the context for a new data processing or generation (depending from the primitive).
    ///         If IV size is greater than maximally supported by the algorithm then an implementation may use the
    ///         leading bytes only from the sequence.
    /// @param iv  the Initialization Vector (IV) or "nonce" object
    /// @return @c  void has value if Start sucess ,flase otherwise
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kUninitializedContext   if the context was not initialized
    /// @error: SecurityErrorDomain::kInvalidInputSize       if the size of provided IV is not supported (i.e. if it is
    /// not enough for the initialization)
    /// @error: SecurityErrorDomain::kUnsupported            if the base algorithm (or its current implementation)
    /// principally doesn't support the IV variation
    /// @error: SecurityErrorDomain::kUsageViolation         if this transformation type is prohibited by the "allowed
    /// usage" restrictions of the provided @c SecretSeed object
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_24715}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02302}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02163
    /// @trace_id_dd=DD_CRYPTO_04934
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< void > Start(SecretSeed const &iv) noexcept = 0;
    /// @brief Update the digest calculation with the specified RestrictedUseObject. This method is specialized for cases where the RestrictedUseObject is part of the "message".
    /// @brief Update the digest calculation by the specified RestrictedUseObject.
    ///         This method is dedicated for cases then the @c RestrictedUseObject is a part of the "message".
    /// @param in  a part of input message that should be processed
    /// @return @c void has value if UpdateAssociatedData sucess ,flase otherwise
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kProcessingNotStarted   if the digest calculation was not initiated by a call of
    /// the
    /// @c Start() method
    /// @error: SecurityErrorDomain::kInvalidUsageOrder      if ProcessConfidentialData has already been called
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_20312}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02302}
    /// @threadsafety={Thread-safe}。
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02164
    /// @trace_id_dd=DD_CRYPTO_04935
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< void > UpdateAssociatedData(RestrictedUseObject const &in) noexcept = 0;
    /// @brief Update the digest calculation with a new block of associated data.
    /// @brief Update the digest calculation by a new chunk of associated data.
    /// @param in  a part of the input message that should be processed
    /// @return @c void  has value if UpdateAssociatedData sucess ,flase otherwise
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kProcessingNotStarted   if the digest calculation was not initiated by a call of
    /// the
    /// @c Start() method
    /// @error: SecurityErrorDomain::kInvalidUsageOrder      if ProcessConfidentialData has already been called
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_20313}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02302}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02165
    /// @trace_id_dd=DD_CRYPTO_04936
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< void > UpdateAssociatedData(ReadOnlyMemRegion const &in) noexcept = 0;
    /// @brief Update the digest calculation with the specified bytes. This method facilitates handling of constant tags.
    /// @brief Update the digest calculation by the specified Byte. This method is convenient for processing of
    /// constant tags.
    /// @param in  a byte value that is a part of input message
    /// @return @c void  has value if UpdateAssociatedData sucess ,flase otherwise
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kProcessingNotStarted   if the digest calculation was not initiated by a call of the @c Start() method
    /// @error: SecurityErrorDomain::kInvalidUsageOrder      if ProcessConfidentialData has already been called
    /// @export_level=/crypto
    /// @trace_id_sws={SWS_CRYPT_20314}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02302}
    /// @threadsafety={Thread-safe}
    /// @interface_level=software
    /// @trace_id_ad=AD_CRYPTO_02166
    /// @trace_id_dd=DD_CRYPTO_04937
    /// @needwork = ad
    /// @endcode
    virtual ara::core::Result< void > UpdateAssociatedData(std::uint8_t const &in) noexcept = 0;
    //***************/
public:
};
//********************************/

}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYP_AUTH_CIPHER_CTX_H_
