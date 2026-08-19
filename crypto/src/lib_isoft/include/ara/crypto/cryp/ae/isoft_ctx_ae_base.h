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
/// @file       isoft_ctx_ae_base.h
/// @brief      AutoSar-Crypto encryption and decryption module
/// @details    Authenticated encryption base class.
/// @date       2022-05-10
/// @author     Zheng Chang
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default Encryption and Decryption/Authenticated Encryption
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_01011
/// @unit_name=PCtxAe_Base
/// @unit_description=Ae authenticated encryption template base class
/// @endcode
///
/// ================================================================
///
/// AE AuthCipherCtx abbreviation
///
/// ================================================================

#ifndef ARA_CRYPTO_CRYP_PUHUA_CTX_AE_BASE_H_
#define ARA_CRYPTO_CRYP_PUHUA_CTX_AE_BASE_H_

#include "ara/crypto/cryp/auth_cipher_ctx.h"
#include "ara/crypto/cryp/mac/isoft_ctx_mac_aes_cbc.h"
#include "ara/crypto/cryp/mac/isoft_ctx_mac_base.h"
#include "ara/crypto/cryp/mac/isoft_ctx_mac_des_cbc.h"
#include "ara/crypto/cryp/symmetric/isoft_ctx_symmetric_stream.h"
#include "ara/crypto/cryp/symmetric/isoft_ctx_symmetric_stream_aes.h"
#include "ara/crypto/cryp/symmetric/isoft_ctx_symmetric_stream_des.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
/// @brief Encryption and decryption provider
class PCryptoProvider;
/// @brief Ae authenticated encryption template base class
/// @code{.isoft}
/// @tparam T_Mac
/// @tparam T_SymmetricStream
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00487
/// @trace_id_dd=DD_CRYPTO_01913
/// @needwork = ad
/// @endcode
template < typename T_Mac, typename T_SymmetricStream >
// PRQA S 2025 QAC /// @qac: false positive
class PCtxAe_Base : public AuthCipherCtx
// PRQA L:QAC
{
public:
    /// @brief Constructor: pass CryptoProvider
    /// @param cryptoProvider encryption provider
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00488
    /// @trace_id_dd=DD_CRYPTO_01914
    /// @needwork = ad
    /// @endcode
    explicit PCtxAe_Base(PCryptoProvider& cryptoProvider) noexcept(noexcept(
        std::make_unique< T_Mac >(cryptoProvider)) && noexcept(std::make_unique< T_SymmetricStream >(cryptoProvider)));
    /// @brief Default virtual destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00489
    /// @trace_id_dd=DD_CRYPTO_01915
    /// @needwork = ad
    /// @endcode
    ~PCtxAe_Base() override = default;
    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00490
    /// @trace_id_dd=DD_CRYPTO_01916
    /// @needwork = ad
    /// @endcode
    PCtxAe_Base() noexcept = delete;
    /// @brief Default move constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00491
    /// @trace_id_dd=DD_CRYPTO_01917
    /// @needwork = ad
    /// @endcode
    PCtxAe_Base(PCtxAe_Base&& other) noexcept = delete;
    /// @brief Default copy constructor
    /// @param other another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00492
    /// @trace_id_dd=DD_CRYPTO_01918
    /// @needwork = ad
    /// @endcode
    PCtxAe_Base(PCtxAe_Base const& other) noexcept = delete;
    /// @brief Default copy assignment operator
    /// @param other another object instance of this class
    /// @returns *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00493
    /// @trace_id_dd=DD_CRYPTO_01919
    /// @needwork = ad
    /// @endcode
    PCtxAe_Base& operator=(PCtxAe_Base const& other) noexcept = delete;
    /// @brief Default move assignment operator
    /// @param other another object instance of this class
    /// @returns *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00494
    /// @trace_id_dd=DD_CRYPTO_01920
    /// @needwork = ad
    /// @endcode
    PCtxAe_Base& operator=(PCtxAe_Base&& other) noexcept = delete;

public:
    /// @brief Authenticated encryption state
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00495
    /// @trace_id_dd=DD_CRYPTO_01921
    /// @needwork = ad
    /// @endcode
    // PRQA S 2025 QAC /// @qac: false positive
    enum class EAeWorkState : std::uint8_t
    // PRQA L:QAC
    {
        /// @brief Not started
        kAeNotStarted = 0,
        /// @brief Started
        kAeStart = 0x01,
        /// @brief Working
        kAeUpdate = 0x02,
        /// @brief Completed
        kAeFinish = 0x0F,
    };

private:
    /// @brief Encryption and decryption manager
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01922
    /// @needwork = dda
    /// @endcode
    PCryptoProvider& cryptoProvider_;
    /// @brief MAC context pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01923
    /// @needwork = dda
    /// @endcode
    PCtxMac_Base::Uptr pCtxMacBase_;
    /// @brief AE working state enumeration
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01924
    /// @needwork = dda
    /// @endcode
    EAeWorkState eAeState_;
    /// @brief Symmetric stream encryption context pointer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01925
    /// @needwork = dda
    /// @endcode
    PCtxSymmetricStream::Uptr pCtxSymmetricStream_;
    /// @brief Temporary storage array for return results
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01926
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< ara::core::Byte > vecResult_;
    /// @brief Store transformation information
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01927
    /// @needwork = dda
    /// @endcode
    CryptoTransform eCryptoTransform_;
    /// @brief Parameterized constructor
    /// @param cryptoProvider encryption provider
    /// @param pCtxMacBase pointer to MAC algorithm context object
    /// @param eAeState AE state
    /// @param pCtxSymmetricStream pointer to symmetric stream encryption context object
    /// @param eCryptoTransform encryption direction: encryption or decryption, etc.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01928
    /// @needwork = dda
    /// @endcode
    PCtxAe_Base(PCryptoProvider& cryptoProvider,
                PCtxMac_Base::Uptr pCtxMacBase,
                EAeWorkState const eAeState,
                PCtxSymmetricStream::Uptr pCtxSymmetricStream,
                CryptoTransform const eCryptoTransform) noexcept;

public:
    /// @brief Get a reference to Crypto Provider of this context.
    /// @return encryption provider instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00496
    /// @trace_id_dd=DD_CRYPTO_01929
    /// @needwork = ad
    /// @endcode
    CryptoProvider& MyProvider() const noexcept override;
    /// @brief Return CryptoPrimitivId instance containing instance identification.
    /// @return encryption algorithm instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00497
    /// @trace_id_dd=DD_CRYPTO_01930
    /// @needwork = ad
    /// @endcode
    CryptoPrimitiveId::Uptr GetCryptoPrimitiveId() const noexcept override = 0;
    /// @brief Check if the crypto context is already initialized and ready to use.
    /// @return true if has already initialized false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00498
    /// @trace_id_dd=DD_CRYPTO_01931
    /// @needwork = ad
    /// @endcode
    bool IsInitialized() const noexcept override;

private:
    /// @brief Process plaintext
    /// @param stSrcData data to be processed
    /// @param nDataLen data length
    /// @param expectedTag expected value
    /// @return encrypted data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00000
    /// @trace_id_dd=DD_CRYPTO_01932
    /// @needwork = dda
    /// @endcode
    virtual ara::core::Result< ara::core::Vector< ara::core::Byte > > ProcessData(
        uint8_t const* const stSrcData, uint32_t const nDataLen, ReadOnlyMemRegion const& expectedTag) const noexcept;

public:  // AuthCipherCtx interface
    /// @brief Check the computed digest against the expected "signature" object.
    ///     The entire digest value is kept in the context until the next call to Start(), so it can be verified or extracted again.
    ///     This method can be implemented "inline" after the function ara::core::memcmp() is standardized.
    /// @brief Check the calculated digest against an expected "signature" object.
    ///       Entire digest value is kept in the context up to next call @c Start(), therefore it can be
    ///       verified again or extracted.
    ///       This method can be implemented as "inline" after standartization of function @c ara::core::memcmp().
    /// @param expected  the signature object containing an expected digest value
    /// @returns @c true if value and meta-information of the provided "signature" object is identical to calculated
    ///          digest and current configuration of the context respectively; but @c false otherwise
    /// @return ture if check sucess false otherwise
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_20319}
    /// @uptrace={RS_CRYPTO_02203}
    /// @uptrace={RS_CRYPTO_02204}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @error: SecurityErrorDomain::kProcessingNotFinished
    ///     if the digest calculation was not finished by a call of the @c Finish() method
    /// @error: SecurityErrorDomain::kIncompatibleObject
    ///     if the provided "signature" object was produced by another crypto primitive type
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00499
    /// @trace_id_dd=DD_CRYPTO_01933
    /// @needwork = ad
    /// @endcode
    ara::core::Result< bool > Check(Signature const& expected) const noexcept override;
    /// @brief Get the BlockService instance.
    /// @brief Get BlockService instance.
    /// @return BlockService instance
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_20102}
    /// @uptrace={RS_CRYPTO_02006}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00500
    /// @trace_id_dd=DD_CRYPTO_01934
    /// @needwork = ad
    /// @endcode
    BlockService::Uptr GetBlockService() const noexcept override = 0;
    /// @brief Get digest
    /// @param offset position of the first byte of digest that should be placed to the output buffer
    /// @return digest information
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00501
    /// @trace_id_dd=DD_CRYPTO_01935
    /// @needwork = ad
    /// @endcode
    ara::core::Result< ara::core::Vector< ara::core::Byte > > GetDigest(std::size_t offset
                                                                        = 0U) const noexcept override;
    /// @brief Get the transformation type configured for this context: kEncrypt or kDecrypt
    /// @brief Get the kind of transformation configured for this context: kEncrypt or kDecrypt
    /// @returns @c CryptoTransform
    /// @return
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_21715}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @error: SecurityErrorDomain::kUninitializedContext  if the transformation direction of this context
    ///         is configurable during an initialization, but the context was not initialized yet
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00502
    /// @trace_id_dd=DD_CRYPTO_01936
    /// @needwork = ad
    /// @endcode
    ara::core::Result< CryptoTransform > GetTransformation() const noexcept override;
    /// @brief Get the maximum size supported for associated public data.
    /// @brief Get maximal supported size of associated public data.
    /// @returns maximal supported size of associated public data in bytes
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_20103}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00503
    /// @trace_id_dd=DD_CRYPTO_01937
    /// @needwork = ad
    /// @endcode
    std::uint64_t GetMaxAssociatedDataSize() const noexcept override;
    /// @brief Process confidential data.
    ///         The input buffer will be overwritten by the processed message. This function is the final call, i.e., all relevant data must have been provided.
    ///         Therefore, this function will check the authentication tag and return the processed data only if the tag is valid.
    /// @brief Process confidential data.
    ///         The input buffer will be overwritten by the processed message.
    ///         This function is the final call, i.e. all associated data must have been already provided.
    ///         Hence, the function will check the authentication tag and only return the processed data, if the tag is
    ///         valid.
    /// @param in the input buffer containing the full message
    /// @param expectedTag  pointer to read only mem region
    /// @returns encrypted data
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_23634}
    /// @uptrace={RS_CRYPTO_02302}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @error: SecurityErrorDomain::kInvalidInputSize
    ///     if size of the input buffer is not divisible by the block size (see @c GetBlockSize())
    /// @error: SecurityErrorDomain::kProcessingNotStarted
    ///     if the data processing was not started by a call of the @c Start() method
    /// @error: SecurityErrorDomain::kAuthTagNotValid
    ///     if the processed data cannot be authenticated
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00504
    /// @trace_id_dd=DD_CRYPTO_01938
    /// @needwork = ad
    /// @endcode
    ara::core::Result< ara::core::Vector< ara::core::Byte > > ProcessConfidentialData(
        ReadOnlyMemRegion const& in, ReadOnlyMemRegion const& expectedTag = ReadOnlyMemRegion()) noexcept override;
    /// @brief Process confidential data. The input buffer will be overwritten by the processed message. After calling this method, no other associated data will be updated.
    /// @brief Process confidential data.
    ///         The input buffer will be overwritten by the processed message
    ///         After this method is called no additional associated data may be updated.
    /// @param inOut  the input buffer containing the full message
    /// @param expectedTag  pointer to read only mem region
    /// @return has value if ProcessConfidentialData sucess false otherwise
    /// @code{.isoft}
    /// @error: SecurityErrorDomain::kInvalidInputSize
    ///     if size of the input buffer is not divisible by the block size (see @c GetBlockSize())
    /// @error: SecurityErrorDomain::kProcessingNotStarted
    ///     if the data processing was not started by a call of the @c Start() method
    /// @error: SecurityErrorDomain::kAuthTagNotValid
    ///     if the processed data cannot be authenticated
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_23635}
    /// @uptrace={RS_CRYPTO_02302}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00505
    /// @trace_id_dd=DD_CRYPTO_01939
    /// @needwork = ad
    /// @endcode
    ara::core::Result< void > ProcessConfidentialData(ReadWriteMemRegion& inOut,
                                                      ReadOnlyMemRegion const& expectedTag
                                                      = ReadOnlyMemRegion()) noexcept override;
    /// @brief Clear the encryption context.
    /// @brief Clear the crypto context.
    /// @return has value if reset sucess false otherwise
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_20414}
    /// @uptrace={RS_CRYPTO_02108}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00506
    /// @trace_id_dd=DD_CRYPTO_01940
    /// @needwork = ad
    /// @endcode
    ara::core::Result< void > Reset() noexcept override;
    /// @brief Set (deploy) a key for the authenticated cipher symmetric algorithm context.
    /// @brief Set (deploy) a key to the authenticated cipher symmetric algorithm context.
    /// @param key  the source key object
    /// @param transform  the "direction" indicator:
    ///     deploy the key for encryption (if @c true) or for decryption (if @cfalse)
    /// @return has value if SetKey sucess false otherwise
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_23911}
    /// @uptrace={RS_CRYPTO_02001}
    /// @uptrace={RS_CRYPTO_02003}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @error: SecurityErrorDomain::kIncompatibleObject
    ///     if the provided key object is incompatible with this symmetric key context
    /// @error: SecurityErrorDomain::kUsageViolation
    ///     if the transformation type associated with this context (taking into account the direction specified by @c
    ///     transform) is prohibited by the "allowed usage" restrictions of provided key object
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00507
    /// @trace_id_dd=DD_CRYPTO_01941
    /// @needwork = ad
    /// @endcode
    ara::core::Result< void > SetKey(SymmetricKey const& key,
                                     CryptoTransform transform = CryptoTransform::kEncrypt) noexcept override;
    /// @brief Initialize the context for new data processing or generation (depending on the primitive).
    ///     If the IV size is larger than the maximum supported by the algorithm, the implementation may only use the leading bytes of the sequence.
    /// @brief Initialize the context for a new data processing or generation (depending from the primitive).
    ///         If IV size is greater than maximally supported by the algorithm then an implementation may use the
    ///         leading bytes only from the sequence.
    /// @param iv  an optional Initialization Vector (IV) or "nonce" value
    /// @return has value if Start sucess false otherwise
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_24714}
    /// @uptrace={RS_CRYPTO_02302}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @error: SecurityErrorDomain::kUninitializedContext   if the context was not initialized
    /// @error: SecurityErrorDomain::kInvalidInputSize
    ///     if the size of provided IV is not supported (i.e. if it is not enough for the initialization)
    /// @error: SecurityErrorDomain::kUnsupported
    ///     if the base algorithm (or its current implementation) principally doesn't support the IV variation, but
    ///     provided IV value is not empty, i.e. if <tt>(iv.empty() == false)
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00508
    /// @trace_id_dd=DD_CRYPTO_01942
    /// @needwork = ad
    /// @endcode
    ara::core::Result< void > Start(ReadOnlyMemRegion const& iv = ReadOnlyMemRegion()) noexcept(true) override;
    /// @brief Initialize the context for new data processing or generation (depending on the primitive).
    /// If the IV size is larger than the maximum supported by the algorithm, the implementation may only use the leading bytes of the sequence.
    /// @brief Initialize the context for a new data processing or generation (depending from the primitive).
    ///         If IV size is greater than maximally supported by the algorithm then an implementation may use the
    ///         leading bytes only from the sequence.
    /// @param iv  the Initialization Vector (IV) or "nonce" object
    /// @return has value if Start sucess false otherwise
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_24715}
    /// @uptrace={RS_CRYPTO_02302}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @error: SecurityErrorDomain::kUninitializedContext   if the context was not initialized
    /// @error: SecurityErrorDomain::kInvalidInputSize
    ///     if the size of provided IV is not supported (i.e. if it is not enough for the initialization)
    /// @error: SecurityErrorDomain::kUnsupported
    ///     if the base algorithm (or its current implementation) principally doesn't support the IV variation
    /// @error: SecurityErrorDomain::kUsageViolation
    ///     if this transformation type is prohibited by the "allowed usage" restrictions of the provided @c SecretSeed
    ///     object
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00509
    /// @trace_id_dd=DD_CRYPTO_01943
    /// @needwork = ad
    /// @endcode
    ara::core::Result< void > Start(SecretSeed const& iv) noexcept override;
    /// @brief Update digest calculation with the specified RestrictedUseObject. This method is specifically for cases where the RestrictedUseObject is part of the "message".
    /// @brief Update the digest calculation by the specified RestrictedUseObject.
    ///         This method is dedicated for cases then the @c RestrictedUseObject is a part of the "message".
    /// @param in  a part of input message that should be processed
    /// @return has value if UpdateAssociatedData sucess false otherwise
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_20312}
    /// @uptrace={RS_CRYPTO_02302}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @error: SecurityErrorDomain::kProcessingNotStarted
    ///     if the digest calculation was not initiated by a call of the @c Start() method
    /// @error: SecurityErrorDomain::kInvalidUsageOrder      if ProcessConfidentialData has already been called
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00510
    /// @trace_id_dd=DD_CRYPTO_01944
    /// @needwork = ad
    /// @endcode
    ara::core::Result< void > UpdateAssociatedData(RestrictedUseObject const& in) noexcept override;
    /// @brief Update digest calculation with a new block of associated data.
    /// @brief Update the digest calculation by a new chunk of associated data.
    /// @param in  a part of the input message that should be processed
    /// @return has value if UpdateAssociatedData sucess false otherwise
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_20313}
    /// @uptrace={RS_CRYPTO_02302}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @error: SecurityErrorDomain::kProcessingNotStarted
    ///     if the digest calculation was not initiated by a call of the @c Start() method
    /// @error: SecurityErrorDomain::kInvalidUsageOrder      if ProcessConfidentialData has already been called
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00511
    /// @trace_id_dd=DD_CRYPTO_01945
    /// @needwork = ad
    /// @endcode
    ara::core::Result< void > UpdateAssociatedData(ReadOnlyMemRegion const& in) noexcept override;
    /// @brief Update digest calculation by the specified byte. This method facilitates handling of constant tags.
    /// @brief Update the digest calculation by the specified Byte. This method is convenient for processing of
    /// constant tags.
    /// @param in  a byte value that is a part of input message
    /// @return has value if UpdateAssociatedData sucess false otherwise
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_20314}
    /// @uptrace={RS_CRYPTO_02302}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @error: SecurityErrorDomain::kProcessingNotStarted
    ///     if the digest calculation was not initiated by a call of the @c Start() method
    /// @error: SecurityErrorDomain::kInvalidUsageOrder      if ProcessConfidentialData has already been called
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00512
    /// @trace_id_dd=DD_CRYPTO_01946
    /// @needwork = ad
    /// @endcode
    ara::core::Result< void > UpdateAssociatedData(std::uint8_t const& in) noexcept override;

public:  // PServiceStream interface
    /// @brief Get the actual bit length of the key loaded into the context. If no key is set for the context, return 0.
    /// @brief Get actual bit-length of a key loaded to the context. If no key was set to the context yet then 0 is
    /// returned.
    /// @returns actual length of a key (now set to the algorithm context) in bits
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_29045}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00513
    /// @trace_id_dd=DD_CRYPTO_01947
    /// @needwork = ad
    /// @endcode
    virtual std::size_t GetActualKeyBitLength() const noexcept;
    /// @brief Get the COUID of the key deployed to the context attached to this extended service. If no key is set for the context, return an empty COUID (Nil).
    /// @brief Get the COUID of the key deployed to the context this extension service is attached to.
    /// If no key was set to the context yet then an empty COUID (Nil) is returned.
    /// @returns the COUID of the CryptoObject
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_29047}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00514
    /// @trace_id_dd=DD_CRYPTO_01948
    /// @needwork = ad
    /// @endcode
    virtual CryptoObjectUid GetActualKeyCOUID() const noexcept;
    /// @brief Get the allowed usage of this context (according to the key object attributes loaded into this context).
    ///  If the context has not been initialized with a key object, 0 must be returned (all flags reset).
    /// @brief Get allowed usages of this context (according to the key object attributes loaded to this context).
    /// If the context is not initialized by a key object yet then zero (all flags are reset) must be returned.
    /// @returns a combination of bit-flags that specifies allowed usages of the context
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_29046}
    /// @uptrace={RS_CRYPTO_02008}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00515
    /// @trace_id_dd=DD_CRYPTO_01949
    /// @needwork = ad
    /// @endcode
    virtual AllowedUsageFlags GetAllowedUsage() const noexcept;
    /// @brief Get the maximum supported key length (in bits).
    /// @brief Get maximal supported key length in bits.
    /// @returns maximal supported length of the key in bits
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_29044}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00516
    /// @trace_id_dd=DD_CRYPTO_01950
    /// @needwork = ad
    /// @endcode
    virtual std::size_t GetMaxKeyBitLength() const noexcept;
    /// @brief Get the minimum supported key length (in bits).
    /// @brief Get minimal supported key length in bits.
    /// @returns minimal supported length of the key in bits
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_29043}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00517
    /// @trace_id_dd=DD_CRYPTO_01951
    /// @needwork = ad
    /// @endcode
    virtual std::size_t GetMinKeyBitLength() const noexcept;
    /// @brief Verify support for a specific key length according to the context.
    /// @brief Verify supportness of specific key length by the context.
    /// @param keyBitLength key length in bits
    /// @returns @c true if provided value of the key length is supported by the context
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_29048}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00518
    /// @trace_id_dd=DD_CRYPTO_01952
    /// @needwork = ad
    /// @endcode
    virtual bool IsKeyBitLengthSupported(std::size_t keyBitLength) const noexcept;
    /// @brief Check if a key is set for this context.
    /// @brief Check if a key has been set to this context.
    /// @return true if a key has been set to this context false otherwise
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_29049}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00519
    /// @trace_id_dd=DD_CRYPTO_01953
    /// @needwork = ad
    /// @endcode
    virtual bool IsKeyAvailable() const noexcept;
    /// @brief Get the block (or internal buffer) size of the underlying algorithm.
    /// @brief Get block (or internal buffer) size of the base algorithm.
    /// @returns size of the block in bytes
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_29033}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00520
    /// @trace_id_dd=DD_CRYPTO_01954
    /// @needwork = ad
    /// @endcode
    virtual std::size_t GetBlockSize() const noexcept;
    /// @brief Get the actual bit length of the IV loaded into the context.
    /// @brief Get actual bit-length of an IV loaded to the context.
    /// @param ivUid  optional pointer to a buffer for saving an @a COUID of a IV object now loaded to the context.
    /// If the context was initialized by a @c SecretSeed object then the output buffer @c *ivUid must be filled
    ///       by @a COUID of this loaded IV object, in other cases @c *ivUid must be filled by all zeros.
    /// @returns actual length of the IV (now set to the algorithm context) in bits
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_29035}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00521
    /// @trace_id_dd=DD_CRYPTO_01955
    /// @needwork = ad
    /// @endcode
    virtual std::size_t GetActualIvBitLength(ara::core::Optional< CryptoObjectUid >& ivUid) const noexcept;
    /// @brief Get the expected default size of the initialization vector (IV) or nonce.
    /// @brief Get default expected size of the Initialization Vector (IV) or nonce.
    /// @returns default expected size of IV in bytes
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_29032}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00522
    /// @trace_id_dd=DD_CRYPTO_01956
    /// @needwork = ad
    /// @endcode
    virtual std::size_t GetIvSize() const noexcept;
    /// @brief Verify the validity of a specific initialization vector (IV) length.
    /// @brief Verify validity of specific Initialization Vector (IV) length.
    /// @param ivSize  the length of the IV in bytes
    /// @returns @c true if provided IV length is supported by the algorithm and @c false otherwise
    /// @code{.isoft}
    /// @export_level=/Crypto
    /// @trace_id_sws={SWS_CRYPT_29034}
    /// @uptrace={RS_CRYPTO_02309}
    /// @threadsafety={Thread-safe}
    /// @tracestatus={draft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_CRYPTO_00523
    /// @trace_id_dd=DD_CRYPTO_01957
    /// @needwork = ad
    /// @endcode
    virtual bool IsValidIvSize(std::size_t ivSize) const noexcept;
};
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara
#endif  // ARA_CRYPTO_CRYP_PUHUA_CTX_AE_BASE_H_