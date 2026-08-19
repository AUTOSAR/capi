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
/// @file       isoft_ctx_mac_base.h
/// @brief      AutoSar-Crypto Encryption Module
/// @details    Message authentication code interface.
/// @date       2022-04-15
/// @author     Chang Zheng
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author        <th>Description
/// <tr><td>2021-12-21  <td>1.0.0    <td>hanjingjing        <td>Initial version creation
/// <tr><td>2023-8-10   <td>1.0.0    <td>Che Jinzhao  <td>Improve functional requirements
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default Encryption/Message Authentication Code
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_01005
/// @unit_name=PCtxMac_Base
/// @unit_description=Message Authentication Code Base Class
/// @endcode
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_PUHUA_CTX_MAC_BASE_H_
#define ARA_CRYPTO_CRYP_PUHUA_CTX_MAC_BASE_H_

#include <openssl/evp.h>

#include "ara/crypto/cryp/cryobj/isoft_crypto_primitive_id.h"
#include "ara/crypto/cryp/isoft_auto_buff.h"
#include "ara/crypto/cryp/message_authn_code_ctx.h"
#include "ara/crypto/openssl/isoft_openssl_cmac.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
/// @brief Encryption provider
class PCryptoProvider;
/// @brief MAC algorithm context base class
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00263
/// @trace_id_dd=DD_CRYPTO_01350
/// @needwork = ad
/// @endcode
class PCtxMac_Base : public MessageAuthnCodeCtx
{
public:
    /// @brief the mac work state
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00264
    /// @trace_id_dd=DD_CRYPTO_01351
    /// @needwork = ad
    /// @endcode
    enum class EMacWorkState : std::uint8_t
    {
        /// @brief Not started
        kMacNotStarted = 0,
        /// @brief Started
        kMacStart = 0x01,
        /// @brief Working
        kMacUpdate = 0x02,
        /// @brief Completed
        kMacFinish = 0x0F,
    };

protected:
    /// @brief Encryption provider object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01352
    /// @needwork = dda
    /// @endcode
    PCryptoProvider& cryptoProvider_;  // NOLINT Subclasses still use this
    /// @brief CMAC_CTX structure
    openssl::isoft_def::CMAC_CTX* cmacCtx_{};  // NOLINT Subclasses still use this

private:
    /// @brief MAC working state
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01353
    /// @needwork = dda
    /// @endcode
    EMacWorkState eMacState_;
    /// @brief Symmetric key pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01354
    /// @needwork = dda
    /// @endcode
    SymmetricKey const* pSymmetricKey_;
    /// @brief Encryption direction
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01355
    /// @needwork = dda
    /// @endcode
    CryptoTransform eCryptoTransform_;
    /// @brief If secretSeed is used for IV initialization, record its COUID for GetActualIvBitLength calls to retrieve the COUID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01356
    /// @needwork = dda
    /// @endcode
    CryptoObjectUid secretSeedCouId_;
    /// @brief Parameterized constructor
    /// @param cryptoProvider Encryption provider
    /// @param eMacState MAC working state
    /// @param pSymmetricKey Symmetric key pointer
    /// @param eCryptoTransform Encryption direction: encrypt or decrypt, etc.
    /// @param secretSeedCouId Secret seed crypto material ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01357
    /// @needwork = dda
    /// @endcode
    PCtxMac_Base(PCryptoProvider& cryptoProvider,
                 EMacWorkState const eMacState,
                 SymmetricKey const* const pSymmetricKey,
                 CryptoTransform const eCryptoTransform,
                 CryptoObjectUid const& secretSeedCouId) noexcept;

public:
    /// @brief Unique smart pointer of the interface.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00263
    /// @trace_id_dd=DD_CRYPTO_06278
    /// @needwork = dd
    /// @endcode
    using Uptr = std::unique_ptr< PCtxMac_Base >;

public:
    /// @brief Parameterized constructor
    /// @name PCtxMac_Base
    /// @param cryptoProvider Encryption provider PCryptoProvider
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00266
    /// @trace_id_dd=DD_CRYPTO_01359
    /// @needwork = ad
    /// @endcode
    explicit PCtxMac_Base(PCryptoProvider& cryptoProvider) noexcept;
    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00267
    /// @trace_id_dd=DD_CRYPTO_01360
    /// @needwork = ad
    /// @endcode
    PCtxMac_Base() = delete;
    /// @brief Default virtual destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00268
    /// @trace_id_dd=DD_CRYPTO_01361
    /// @needwork = ad
    /// @endcode
    ~PCtxMac_Base() noexcept override;
    /// @brief Default move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00269
    /// @trace_id_dd=DD_CRYPTO_01362
    /// @needwork = ad
    /// @endcode
    PCtxMac_Base(PCtxMac_Base&& other) = delete;
    /// @brief Default move assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00270
    /// @trace_id_dd=DD_CRYPTO_01363
    /// @needwork = ad
    /// @endcode
    PCtxMac_Base& operator=(PCtxMac_Base&& other) = delete;
    /// @brief Default copy assignment function
    /// @param other Another object instance of this class
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00271
    /// @trace_id_dd=DD_CRYPTO_01364
    /// @needwork = ad
    /// @endcode
    PCtxMac_Base& operator=(PCtxMac_Base const& other) = delete;
    /// @brief Default copy constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00272
    /// @trace_id_dd=DD_CRYPTO_01365
    /// @needwork = ad
    /// @endcode
    PCtxMac_Base(PCtxMac_Base const& other) = delete;

public:
    /// @brief Checks the calculated digest against the expected "signature" object.
    ///         The entire digest value is kept in the context until the next call to Start(), so it can be verified or extracted again. This method can be implemented "inline" after the function ara::core::memcmp() is standardized.
    /// @brief Check the calculated digest against an expected "signature" object.
    ///       Entire digest value is kept in the context up to next call @c Start(), therefore it can be verified again
    ///       or extracted. This method can be implemented as "inline" after standartization of function @c
    ///       ara::core::memcmp().
    /// @name  Check
    /// @param expected  the signature object containing an expected digest value
    /// @returns @c true if value and meta-information of the provided "signature" object is identical to calculated
    ///          digest and current configuration of the context respectively; but @c false otherwise
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kProcessingNotFinished  if the digest calculation was not finished by a call of the
    /// @c Finish() method
    /// @error: SecurityErrorDomain::kIncompatibleObject     if the provided "signature" object was produced by another
    /// crypto primitive type
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_22119}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02203}
    /// @uptrace={RS_CRYPTO_02204}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00273
    /// @trace_id_dd=DD_CRYPTO_01366
    /// @needwork = ad
    /// @endcode
    ara::core::Result< bool > Check(Signature const& expected) const noexcept override;
    /// @brief 2011 standard interface
    /// @brief Completes digest calculation and optionally generates a "signature" object. Only after calling this method can the digest be signed, verified, extracted, or compared!
    ///         If the signature object is produced by a keyed MAC/HMAC/AE/AEAD algorithm, the dependency COUID of the "signature" shall be set to the COUID of the symmetric key used.
    /// @brief Finish the digest calculation and optionally produce the "signature" object.
    ///         Only after call of this method the digest can be signed, verified, extracted or compared!
    ///         If the signature object produced by a keyed MAC/HMAC/AE/AEAD algorithm then the dependence COUID of the
    ///         "signature" should be set to COUID of used symmetric key.
    /// @param makeSignatureObject  if this argument is @c true then the method will also produce the signature object
    /// @return    unique smart pointer to created signature object, if(makeSignatureObject == true) or nullptr
    /// if(makeSignatureObject == false)
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kProcessingNotStarted  if the digest calculation was not initiated by a call of the @c Start() method
    /// @error: SecurityErrorDomain::kUsageViolation  if the buffered digest belongs to a MAC/HMAC/AE/AEAD context
    /// initialized by a key without @c kAllowSignature permission, but (makeSignatureObject == true)
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_22115}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02302}
    /// @uptrace={RS_CRYPTO_02203}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00274
    /// @trace_id_dd=DD_CRYPTO_01367
    /// @needwork = ad
    /// @endcode
    ara::core::Result< Signature::Uptrc > Finish(bool makeSignatureObject) noexcept override;
    /// @brief 2311 standard interface
    /// @brief Completes digest calculation and optionally generates a "signature" object. Only after calling this method can the digest be signed, verified, extracted, or compared!
    ///         If the signature object is produced by a keyed MAC/HMAC/AE/AEAD algorithm, the dependency COUID of the "signature" shall be set to the COUID of the symmetric key used.
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
    ara::core::Result< void > Finish() noexcept override;
    /// @brief Gets the DigestService instance.
    /// @brief Get DigestService instance.
    /// @name  GetDigestService
    /// @returns
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_22102}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02006}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00275
    /// @trace_id_dd=DD_CRYPTO_01368
    /// @needwork = ad
    /// @endcode
    DigestService::Uptr GetDigestService() const noexcept override = 0;
    /// @brief Gets the requested part of the calculated digest into an existing memory buffer.
    ///         The entire digest value is kept in the context until the next call to Start(), so any part of it can be extracted or verified again.
    ///         If (full_digest_size <= offset) then return_size = 0 bytes; else return_size = min(output.size(),
    ///         (full_digest_size - offset)) bytes. This method can be implemented "inline" after the function ara::core::memcpy() is standardized.
    /// @brief Get requested part of calculated digest to existing memory buffer.
    ///         Entire digest value is kept in the context up to next call @c Start(), therefore any its part can be
    ///         extracted again or verified. If (full_digest_size <= offset) then return_size = 0 bytes; else
    ///         return_size = min(output.size(), (full_digest_size - offset)) bytes. This method can be implemented as
    ///         "inline" after standartization of function @c ara::core::memcpy().
    /// @name GetDigest
    /// @param offset  position of the first byte of digest that should be placed to the output buffer
    /// @returns number of digest bytes really stored to the output buffer (they are always <= @c output.size() and
    /// denoted below as @a return_size)
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kProcessingNotFinished  if the digest calculation was not finished by a call of the
    /// @c Finish() method
    /// @error: SecurityErrorDomain::kUsageViolation         if the buffered digest belongs to a MAC/HMAC/AE/AEAD
    /// context initialized by a key without @c kAllowSignature permission
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_22116}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02203}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00276
    /// @trace_id_dd=DD_CRYPTO_01369
    /// @needwork = ad
    /// @endcode
    ara::core::Result< ara::core::Vector< ara::core::Byte > > GetDigest(std::size_t offset
                                                                        = 0U) const noexcept override;
    /// @brief Uses base class template functions
    using MessageAuthnCodeCtx::GetDigest;
    /// @brief Clears the crypto context.
    /// @brief Clear the crypto context.
    /// @name  Reset
    /// @returns
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_22120}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02108}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00277
    /// @trace_id_dd=DD_CRYPTO_01370
    /// @needwork = ad
    /// @endcode
    ara::core::Result< void > Reset() noexcept override;
    /// @brief Sets (deploys) a key for the message authentication code algorithm context.
    /// @brief Set (deploy) a key to the message authn code algorithm context.
    /// @param key  the source key object
    /// @param transform  the "direction" indicator: deploy the key for direct transformation (if @c true) or
    /// for reverse one (if @c false)
    /// @returns has vlaue if SetKey sucess false otherwise
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kIncompatibleObject     if the provided key object is incompatible with this
    /// symmetric key context
    /// @error: SecurityErrorDomain::kUsageViolation         if the transformation type associated with this context
    /// (taking into account the direction specified by @c transform) is prohibited by the "allowed usage" restrictions
    /// of provided key object
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_22118}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02001}
    /// @uptrace={RS_CRYPTO_02003}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00278
    /// @trace_id_dd=DD_CRYPTO_01371
    /// @needwork = ad
    /// @endcode
    ara::core::Result< void > SetKey(SymmetricKey const& key,
                                     CryptoTransform transform = CryptoTransform::kMacGenerate) noexcept override;
    /// @brief Initializes the context for a new data stream processing or generation (depending on the primitive).
    ///         If the IV size exceeds the maximum supported size of the algorithm, the implementation may use only the leading bytes of the sequence.
    /// @brief Initialize the context for a new data stream processing or generation (depending from the primitive).
    ///       If IV size is greater than maximally supported by the algorithm then an implementation may use the
    ///       leading bytes only from the sequence.
    /// @name  Start
    /// @param iv  an optional Initialization Vector (IV) or "nonce" value
    /// @returns has vlaue if Start sucess false otherwise
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kUninitializedContext  if the context was not initialized by deploying a key
    /// @error: SecurityErrorDomain::kInvalidInputSize  if the size of provided IV is not supported (i.e. if it is not
    /// enough for the initialization)
    /// @error: SecurityErrorDomain::kUnsupported  if the base algorithm (or its current implementation) principally
    /// doesn't support the IV variation, but provided IV value is not empty, i.e. if <tt>(iv.empty() == false)</tt>
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_22110}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02302}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00279
    /// @trace_id_dd=DD_CRYPTO_01372
    /// @needwork = ad
    /// @endcode
    ara::core::Result< void > Start(ReadOnlyMemRegion const& iv = ReadOnlyMemRegion()) noexcept override;
    /// @brief Initializes the context for a new data stream processing or generation (depending on the primitive).
    ///         If the IV size exceeds the maximum supported size of the algorithm, the implementation may use only the leading bytes of the sequence.
    /// @brief Initialize the context for a new data stream processing or generation (depending from the primitive).
    ///       If IV size is greater than maximally supported by the algorithm then an implementation may use the
    ///       leading bytes only from the sequence.
    /// @name  Start
    /// @param iv  the Initialization Vector (IV) or "nonce" object
    /// @returns has value if Start sucess false  otherwise
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kUninitializedContext   if the context was not initialized by deploying a key
    /// @error: SecurityErrorDomain::kInvalidInputSize       if the size of provided IV is not supported (i.e. if it is
    /// not enough for the initialization)
    /// @error: SecurityErrorDomain::kUnsupported            if the base algorithm (or its current implementation)
    /// principally doesn't support the IV variation
    /// @error: SecurityErrorDomain::kUsageViolation         if this transformation type is prohibited by the "allowed
    /// usage" restrictions of the provided @c SecretSeed object
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_22111}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02302}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00280
    /// @trace_id_dd=DD_CRYPTO_01373
    /// @needwork = ad
    /// @endcode
    ara::core::Result< void > Start(SecretSeed const& iv) noexcept override;
    /// @brief Updates the digest calculation context with a new part of the message. This method is dedicated to cases where RestrictedUseObject is part of the "message".
    /// @brief Update the digest calculation context by a new part of the message.
    ///       This method is dedicated for cases then the @c RestrictedUseObject is a part of the "message".
    /// @name  Update
    /// @param in  a part of input message that should be processed
    /// @returns has vlaue if Update sucess false otherwise
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kProcessingNotStarted  if the digest calculation was not initiated by a call of the
    /// @c Start() method
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_22112}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02302}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00281
    /// @trace_id_dd=DD_CRYPTO_01374
    /// @needwork = ad
    /// @endcode
    ara::core::Result< void > Update(RestrictedUseObject const& in) noexcept override;
    /// @brief Updates the digest calculation context with a new part of the message.
    /// @brief Update the digest calculation context by a new part of the message.
    /// @name Update
    /// @param in  a part of the input message that should be processed
    /// @returns  has vlaue if Update sucess false otherwise
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kProcessingNotStarted   if the digest calculation was not initiated by a call of
    /// the
    /// @c Start() method
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_22113}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02302}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00282
    /// @trace_id_dd=DD_CRYPTO_01375
    /// @needwork = ad
    /// @endcode
    ara::core::Result< void > Update(ReadOnlyMemRegion const& in) noexcept override;
    /// @brief Updates the digest calculation context with a new part of the message. This method facilitates handling constant tags.
    /// @brief Update the digest calculation context by a new part of the message.
    ///       This method is convenient for processing of constant tags.
    /// @name  Update
    /// @param in  a byte value that is a part of input message
    /// @returns  has vlaue if Update sucess false otherwise
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kProcessingNotStarted  if the digest calculation was not initiated by a call of the
    /// @c Start() method
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_22114}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02302}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00283
    /// @trace_id_dd=DD_CRYPTO_01376
    /// @needwork = ad
    /// @endcode
    ara::core::Result< void > Update(std::uint8_t const& in) noexcept override;
    /// @brief Creates a signature object for this MAC. The dependency COUID of this object shall be set to the symmetric key used to generate the MAC.
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
    /// @trace_id_ad=AD_CRYPTO_04353
    /// @trace_id_dd=DD_CRYPTO_08788
    /// @needwork = ad
    /// @endcode
    ara::core::Result< Signature::Uptrc > MakeSignature() noexcept override;

protected:
    /// @brief Init operation
    /// @name   _DoInit
    /// @param piv Pointer to initialization vector
    /// @returns true if _DoInit sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01377
    /// @needwork = dda
    /// @endcode
    bool _DoInit(uint8_t const* const piv) noexcept;
    /// @brief Finish operation
    /// @name   DoFinish
    /// @returns true if DoFinish sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01378
    /// @needwork = dda
    /// @endcode
    virtual bool DoFinish() noexcept;
    /// @brief Reset operation
    /// @name   DoReset
    /// @returns true if DoReset sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01379
    /// @needwork = dda
    /// @endcode
    virtual bool DoReset() noexcept;
    /// @brief Update operation
    /// @name   DoUpdate
    /// @param pVoidData Starting address of data
    /// @param nDataLen Data length
    /// @returns true if DoUpdate sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01380
    /// @needwork = dda
    /// @endcode
    virtual bool DoUpdate(void const* pVoidData, uint32_t nDataLen) noexcept;

public:
    /// @brief SWS_CRYPT_01218 Checks if IV operation is supported
    /// @name   SupportIv
    /// @returns true if support iv false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00284
    /// @trace_id_dd=DD_CRYPTO_01381
    /// @needwork = ad
    /// @endcode
    virtual bool SupportIv() const noexcept = 0;

public:  // Interface of PCtxHashFunction: Provided for DigestService calls
    /// @brief Gets the maximum IV length corresponding to the specific algorithm
    /// @name   GetIvMaxLength
    /// @returns Corresponding maximum IV length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00285
    /// @trace_id_dd=DD_CRYPTO_01382
    /// @needwork = ad
    /// @endcode
    virtual uint32_t GetIvMaxLength() const noexcept = 0;
    /// @brief Get Mac value length
    /// @name   GetMacLength
    /// @returns MAC value length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00286
    /// @trace_id_dd=DD_CRYPTO_01383
    /// @needwork = ad
    /// @endcode
    virtual uint32_t GetMacLength() const noexcept = 0;
    /// @brief Get MAC result
    /// @name   GetMacResult
    /// @returns Pointer to MAC result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00287
    /// @trace_id_dd=DD_CRYPTO_01384
    /// @needwork = ad
    /// @endcode
    virtual uint8_t* GetMacResult() const noexcept = 0;
    /// @brief Check if initialization is complete
    /// @name   IsInitialized
    /// @returns true if already init false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00288
    /// @trace_id_dd=DD_CRYPTO_01385
    /// @needwork = ad
    /// @endcode
    bool IsInitialized() const noexcept override;
    /// @brief View the current state of stream processing: whether it has started.
    /// @name   IsStarted
    /// @returns true if already started false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00289
    /// @trace_id_dd=DD_CRYPTO_01386
    /// @needwork = ad
    /// @endcode
    virtual bool IsStarted() const noexcept;
    /// @brief Check the current state of stream processing: whether it is completed.
    /// @name   IsFinished
    /// @returns true if already Finished false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00290
    /// @trace_id_dd=DD_CRYPTO_01387
    /// @needwork = ad
    /// @endcode
    virtual bool IsFinished() const noexcept;
    /// @brief Compares the calculated digest with the expected value.
    /// @name   Compare
    /// @param expected Expected value
    /// @param offset Offset
    /// @returns true if equal false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00291
    /// @trace_id_dd=DD_CRYPTO_01388
    /// @needwork = ad
    /// @endcode
    virtual bool Compare(ReadOnlyMemRegion const& expected, std::size_t offset) const noexcept;
    /// @brief Get a reference to Crypto Provider of this context.
    /// @name MyProvider
    /// @returns Reference to encryption provider
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00292
    /// @trace_id_dd=DD_CRYPTO_01389
    /// @needwork = ad
    /// @endcode
    CryptoProvider& MyProvider() const noexcept override;
    /// @brief Get key
    /// @name   GetKey
    /// @returns Pointer to key object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00293
    /// @trace_id_dd=DD_CRYPTO_01390
    /// @needwork = ad
    /// @endcode
    inline virtual SymmetricKey const* GetKey() const noexcept { return pSymmetricKey_; }
    /// @brief Check key
    /// @name   CheckKey
    /// @param key the Symmetric Key
    /// @returns true if check key sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00294
    /// @trace_id_dd=DD_CRYPTO_01391
    /// @needwork = ad
    /// @endcode
    virtual bool CheckKey(SymmetricKey const& key) const noexcept = 0;
    /// @brief Get hashid of the generated digest
    /// @return Hashid of the digest
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00295
    /// @trace_id_dd=DD_CRYPTO_01392
    /// @needwork = ad
    /// @endcode
    virtual CryptoPrimitiveId::AlgId GetHashAlgId() const noexcept = 0;
    /// @brief Get the seed COUID filling the context IV
    /// @name   GetSecretSeedCryptoObjectUid
    /// @returns Seed COUID filling the context IV
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00296
    /// @trace_id_dd=DD_CRYPTO_01393
    /// @needwork = ad
    /// @endcode
    inline CryptoObjectUid GetSecretSeedCryptoObjectUid() const noexcept { return secretSeedCouId_; }

protected:  // Interface of PCtxHashFunction
    /// @brief Initialize member variables
    /// @name  InitMacResult
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01394
    /// @needwork = dda
    /// @endcode
    virtual void InitMacResult() noexcept = 0;

protected:
    /// @brief IPC mode setkey
    /// @name   _SetKeyIpc
    /// @param key Symmetric key
    /// @param transform Encryption direction: encrypt or decrypt
    /// @returns has value if setkey sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01395
    /// @needwork = dda
    /// @endcode
    ara::core::Result< void > _SetKeyIpc(SymmetricKey const& key, CryptoTransform const transform) noexcept;
    /// @brief IPC mode init
    /// @name   _DoInitIpc
    /// @param piv Pointer to initialization vector
    /// @returns true if _DoInitIpc sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01396
    /// @needwork = dda
    /// @endcode
    bool _DoInitIpc(uint8_t const* const piv) const noexcept;
    /// @brief IPC mode finish
    /// @return true if _DoFinishIpc sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01397
    /// @needwork = dda
    /// @endcode
    bool _DoFinishIpc() const noexcept;
    /// @brief IPC mode update
    /// @param pVoidData Starting address of data
    /// @param nDataLen Data length
    /// @return true if _DoUpdateIpc sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01398
    /// @needwork = dda
    /// @endcode
    bool _DoUpdateIpc(void const* const pVoidData, uint32_t const nDataLen) const noexcept;
    /// @brief IPC mode Reset
    /// @return true if DoResetIpc sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01399
    /// @needwork = dda
    /// @endcode
    static bool DoResetIpc() noexcept;
    /// @brief IPC mode execution function
    /// @param funName Function name within IPC packet
    /// @param pVoidData Starting address of data
    /// @param nDataLen Data length
    /// @return MAC execution result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01400
    /// @needwork = dda
    /// @endcode
    ara::core::Result< ara::core::Vector< ara::core::Byte > > _DoIpcFun(ara::core::StringView const& funName,
                                                                        void const* const pVoidData = nullptr,
                                                                        uint32_t const nDataLen = 0U) const noexcept;

public:  // ExtensionService interface
    /// @brief Gets the actual bit length of the key loaded into the context. Returns 0 if no key is set for the context.
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
    /// @trace_id_ad=AD_CRYPTO_00297
    /// @trace_id_dd=DD_CRYPTO_01401
    /// @needwork = ad
    /// @endcode
    std::size_t GetActualKeyBitLength() const noexcept
    {
        if (false == IsKeyAvailable()) {
            return 0U;
        }
        return pSymmetricKey_->GetPayloadSize() * kInt_8U;
    };
    /// @brief Gets the COUID of the key deployed to the context attached to this extension service. Returns an empty COUID (Nil) if no key is set for the context.
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
    /// @trace_id_ad=AD_CRYPTO_00298
    /// @trace_id_dd=DD_CRYPTO_01402
    /// @needwork = ad
    /// @endcode
    CryptoObjectUid GetActualKeyCOUID() const noexcept
    {
        if (false == IsKeyAvailable()) {
            return CryptoObjectUid{};
        }
        return pSymmetricKey_->GetObjectId().mCouid;
    }
    /// @brief Gets the allowed usage of this context (according to the properties of the key object loaded into this context).
    ///         If the context has not been initialized with a key object, it must return 0 (all flags reset).
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
    /// @trace_id_ad=AD_CRYPTO_00299
    /// @trace_id_dd=DD_CRYPTO_01403
    /// @needwork = ad
    /// @endcode
    AllowedUsageFlags GetAllowedUsage() const noexcept
    {
        if (pSymmetricKey_ == nullptr) {
            return kAllowPrototypedOnly;
        }
        return pSymmetricKey_->GetAllowedUsage();
    }
    /// @brief Gets the maximum supported key length in bits.
    /// @brief Get maximal supported key length in bits.
    /// @returns maximal supported length of the key in bits
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_29044}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00300
    /// @trace_id_dd=DD_CRYPTO_01404
    /// @needwork = ad
    /// @endcode
    std::size_t GetMaxKeyBitLength() const noexcept
    {
        if (pSymmetricKey_ == nullptr) {
            return 0U;
        }
        return static_cast< std::size_t >(PAlgId_Symmetric_DesKey::EKeyLen::kDesKeyMaxBitLength);
    }
    /// @brief Gets the minimum supported key length in bits.
    /// @brief Get minimal supported key length in bits.
    /// @returns minimal supported length of the key in bits
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_29043}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00301
    /// @trace_id_dd=DD_CRYPTO_01405
    /// @needwork = ad
    /// @endcode
    std::size_t GetMinKeyBitLength() const noexcept
    {
        if (pSymmetricKey_ == nullptr) {
            return 0U;
        }
        return static_cast< std::size_t >(PAlgId_Symmetric_DesKey::EKeyLen::kDesKeyMinBitLength);
    }
    /// @brief Verifies support for a specific key length based on the context.
    /// @brief Verify supportness of specific key length by the context.
    /// @param keyBitLength Key length: in bits
    /// @returns @c true if provided value of the key length is supported by the context
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_29048}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00302
    /// @trace_id_dd=DD_CRYPTO_01406
    /// @needwork = ad
    /// @endcode
    bool IsKeyBitLengthSupported(std::size_t const keyBitLength) const noexcept
    {
        // Default implementation: check if Key.BitLength is between min and max; subclasses should implement this virtual function if Ctx has special requirements.
        if (keyBitLength < GetMinKeyBitLength()) {
            return false;
        }
        if (keyBitLength > GetMaxKeyBitLength()) {
            return false;
        }
        return true;
    }
    /// @brief Checks if a key is set for this context.
    /// @brief Check if a key has been set to this context.
    /// @return
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_29049}
    /// @tracestatus={draft}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00303
    /// @trace_id_dd=DD_CRYPTO_01407
    /// @needwork = ad
    /// @endcode
    bool IsKeyAvailable() const noexcept
    {
        if (nullptr == pSymmetricKey_) {
            return false;
        }
        return true;
    }
    /// @brief MAC init local operation, callable externally
    /// @param piv Pointer to initialization vector
    /// @return true if DoInitLocal sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00304
    /// @trace_id_dd=DD_CRYPTO_01408
    /// @needwork = ad
    /// @endcode
    virtual bool DoInitLocal(uint8_t const* piv) noexcept = 0;
    /// @brief MAC finish local operation, callable externally
    /// @return true if DoFinishLocal sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00305
    /// @trace_id_dd=DD_CRYPTO_01409
    /// @needwork = ad
    /// @endcode
    virtual bool DoFinishLocal() noexcept = 0;
    /// @brief MAC update local operation, callable externally
    /// @param pVoidData Starting address of data
    /// @param nDataLen Data length
    /// @return true if DoUpdateLocal sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00306
    /// @trace_id_dd=DD_CRYPTO_01410
    /// @needwork = ad
    /// @endcode
    virtual bool DoUpdateLocal(void const* pVoidData, uint32_t nDataLen) noexcept = 0;
    /// @brief MAC reset local operation, callable externally
    /// @return true if DoResetLocal sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00307
    /// @trace_id_dd=DD_CRYPTO_01411
    /// @needwork = ad
    /// @endcode
    virtual bool DoResetLocal() noexcept = 0;
};

}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara

#endif  // ARA_CRYPTO_CRYP_PUHUA_CTX_MAC_BASE_H_