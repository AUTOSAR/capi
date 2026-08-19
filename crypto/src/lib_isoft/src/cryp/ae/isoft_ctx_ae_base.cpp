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
/// @file       isoft_ctx_ae_base.cpp
/// @brief      AutoSar-Crypto encryption/decryption module
/// @details
/// @date       2022-05-10
/// @author     Chang Zheng
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/DefaultEncryptionDecryption/AuthenticatedEncryption
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_01011
/// @unit_name=PCtxAe_Base
/// @unit_description=Ae authenticated encryption template base class
/// @endcode
///
/// ================================================================

#include "ara/crypto/cryp/ae/isoft_ctx_ae_base.h"

#include "ara/crypto/cryp/isoft_crypto_provider.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
//********************************/
/// @brief Constructor: pass CryptoProvider
/// @param cryptoProvider crypto provider
template < typename T_Mac, typename T_SymmetricStream >  // NOLINT
inline PCtxAe_Base< T_Mac, T_SymmetricStream >::PCtxAe_Base(PCryptoProvider &cryptoProvider) noexcept(noexcept(
    std::make_unique< T_Mac >(cryptoProvider)) &&noexcept(std::make_unique< T_SymmetricStream >(cryptoProvider)))
    : PCtxAe_Base{cryptoProvider, std::make_unique< T_Mac >(cryptoProvider), EAeWorkState::kAeNotStarted,
                  std::make_unique< T_SymmetricStream >(cryptoProvider), CryptoTransform::kUnknown}
{
}
/// @brief Parameterized constructor
/// @param cryptoProvider crypto provider
/// @param pCtxMacBase pointer to MAC algorithm context object
/// @param eAeState AE state
/// @param pCtxSymmetricStream pointer to symmetric stream cipher context object
/// @param eCryptoTransform encryption direction: encrypt or decrypt, etc.
template < typename T_Mac, typename T_SymmetricStream >
inline PCtxAe_Base< T_Mac, T_SymmetricStream >::PCtxAe_Base(PCryptoProvider &cryptoProvider,
                                                            PCtxMac_Base::Uptr pCtxMacBase,
                                                            EAeWorkState const eAeState,
                                                            PCtxSymmetricStream::Uptr pCtxSymmetricStream,
                                                            CryptoTransform const eCryptoTransform) noexcept
    : AuthCipherCtx{}
    , cryptoProvider_{cryptoProvider}
    , pCtxMacBase_{std::move(pCtxMacBase)}
    , eAeState_{eAeState}
    , pCtxSymmetricStream_{std::move(pCtxSymmetricStream)}
    , vecResult_{}  // NOLINT
    , eCryptoTransform_{eCryptoTransform}
{
}
/// @brief Get the reference to the Crypto Provider for this context.
/// @returns  crypto provider instance
template < typename T_Mac, typename T_SymmetricStream >
inline CryptoProvider &PCtxAe_Base< T_Mac, T_SymmetricStream >::MyProvider() const noexcept
{
    // PRQA S 4623 QAC /// @qac: AUTOSAR standard interface
    return cryptoProvider_;
    // PRQA L:QAC
}
/// @brief Check whether the encryption context has been initialized and is usable. It checks all required values, including: key value, IV/seed, etc.
/// @returns true if has already initialized false otherwise
template < typename T_Mac, typename T_SymmetricStream >
inline bool PCtxAe_Base< T_Mac, T_SymmetricStream >::IsInitialized() const noexcept
{
    return IsKeyAvailable();
}
/// @brief Check the computed digest against the expected "signature" object.
///     The entire digest value is kept in the context until the next call to Start() and can therefore be verified or extracted again.
///     This method can be implemented "inline" after the function ara::core::memcmp() is standardized.
/// @param expected expected value
/// @returns @c true if value and meta-information of the provided "signature" object is identical to calculated
///          digest and current configuration of the context respectively; but @c false otherwise
template < typename T_Mac, typename T_SymmetricStream >
inline ara::core::Result< bool > PCtxAe_Base< T_Mac, T_SymmetricStream >::Check(
    Signature const &expected) const noexcept
{
    /// @error: SecurityErrorDomain::kProcessingNotFinished  if the digest calculation was not finished by a call of the @c Finish() method
    if (eAeState_ != EAeWorkState::kAeFinish) {
        return ara::core::Result< bool >::FromError(SecurityErrorDomain::Errc::kProcessingNotFinished);
    }
    /// @error: SecurityErrorDomain::kIncompatibleObject  if the provided "signature" object was produced by another crypto primitive type
    // Here, if the digest is generated by a non-hash function, hashID is kAlgIdUndefined; if generated by a hash function, it is the corresponding hashID
    if (expected.GetHashAlgId() != pCtxMacBase_->GetHashAlgId()) {
        return ara::core::Result< bool >::FromError(SecurityErrorDomain::Errc::kIncompatibleObject);
    }

    return pCtxMacBase_->Check(expected);
}
/// @brief Get digest
/// @param offset offset
/// @return digest information
template < typename T_Mac, typename T_SymmetricStream >
inline ara::core::Result< ara::core::Vector< ara::core::Byte > > PCtxAe_Base< T_Mac, T_SymmetricStream >::GetDigest(
    std::size_t offset) const noexcept
{
    if (EAeWorkState::kAeFinish != eAeState_) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kProcessingNotFinished);
    }
    return pCtxMacBase_->GetDigest(offset);
}
/// @brief Get the transformation type configured for this context: kEncrypt or kDecrypt
/// @returns  @c CryptoTransform
template < typename T_Mac, typename T_SymmetricStream >
inline ara::core::Result< CryptoTransform > PCtxAe_Base< T_Mac, T_SymmetricStream >::GetTransformation() const noexcept
{
    /// @error: SecurityErrorDomain::kUninitializedContext  if the transformation direction of this context
    ///                is configurable during an initialization, but the context was not initialized yet
    if (eAeState_ == EAeWorkState::kAeNotStarted) {
        return ara::core::Result< CryptoTransform >::FromError(SecurityErrorDomain::Errc::kUninitializedContext);
    }
    return pCtxSymmetricStream_->GetTransformation();
}
/// @brief Clear the encryption context.
/// @returns  has value if reset sucess false otherwise
template < typename T_Mac, typename T_SymmetricStream >
inline ara::core::Result< void > PCtxAe_Base< T_Mac, T_SymmetricStream >::Reset() noexcept
{
    std::ignore = pCtxMacBase_->Reset();
    return pCtxSymmetricStream_->Reset();
}
/// @brief Set (deploy) a key for the authenticated cipher symmetric algorithm context.
/// @param key symmetric key
/// @param transform encryption direction: encrypt or decrypt
/// @returns   has value if SetKey sucess false otherwise
template < typename T_Mac, typename T_SymmetricStream >
inline ara::core::Result< void > PCtxAe_Base< T_Mac, T_SymmetricStream >::SetKey(SymmetricKey const &key,
                                                                                 CryptoTransform transform) noexcept
{
    PKeySymmetric_Base const *const pKeySymmetricBase{dynamic_cast< PKeySymmetric_Base const * >(&key)};
    if (pKeySymmetricBase == nullptr) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUsageViolation);
    }

    /// @error:    a kInvalidArgument error, if the provided transformation direction is not al
    /// lowed in authenticated cipher symmetric algorithm context.
    if ((transform != CryptoTransform::kEncrypt) && (transform != CryptoTransform::kDecrypt)) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kInvalidArgument);
    }

    uint32_t const nKeyAllowedUsage{key.GetAllowedUsage()};
    if (transform == CryptoTransform::kEncrypt) {
        if ((kAllowDataEncryption & nKeyAllowedUsage) != kAllowDataEncryption) {
            return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUsageViolation);
        }
        /// mac requires kAllowSignature usage
        ara::core::Result< void > const resVoid{pCtxMacBase_->SetKey(key, CryptoTransform::kMacGenerate)};
        if (!resVoid.HasValue()) {
            return ara::core::Result< void >::FromError(
                static_cast< SecurityErrorDomain::Errc >(resVoid.Error().Value()));
        }
    } else if (transform == CryptoTransform::kDecrypt) {
        if ((kAllowDataDecryption & nKeyAllowedUsage) != kAllowDataDecryption) {
            return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUsageViolation);
        }
        /// mac requires kAllowSignature usage
        ara::core::Result< void > const resVoid{pCtxMacBase_->SetKey(key, CryptoTransform::kMacVerify)};
        if (!resVoid.HasValue()) {
            return ara::core::Result< void >::FromError(
                static_cast< SecurityErrorDomain::Errc >(resVoid.Error().Value()));
        }
    } else {
        /// @error: SecurityErrorDomain::kUsageViolation         if the transformation type associated with this context
        ///         (taking into account the direction specified by @c transform) is prohibited by the "allowed usage"
        ///         restrictions of provided key object
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUsageViolation);
    }

    eCryptoTransform_ = transform;
    return pCtxSymmetricStream_->SetKey(key, transform);
}
/// @brief Initialize the context for new data processing or generation (depending on the primitive).
///     If the IV size is larger than the maximum size supported by the algorithm, the implementation may only use the leading bytes of the sequence.
/// @param iv initialization vector
/// @returns  has value if Start sucess false otherwise
template < typename T_Mac, typename T_SymmetricStream >
inline ara::core::Result< void > PCtxAe_Base< T_Mac, T_SymmetricStream >::Start(ReadOnlyMemRegion const &iv) noexcept
{
    // You can determine whether setkey has been called by checking an identifier, or by checking whether the key inside pCtxMacBase_ is empty
    if (pCtxMacBase_->GetKey() == nullptr) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUninitializedContext);
    }

    // Start shall return a SecurityErrorDomain::kInvalidInputSize error, if the provided data is insufficient.
    // Check whether its length meets requirements
    uint32_t const ivMaxLength{pCtxMacBase_->GetIvMaxLength()};
    if (pCtxMacBase_->SupportIv()
        == true) {  // If using iv.empty == false to judge, this case would be missed: IV is supported but iv is empty
        if (iv.size() < ivMaxLength) {
            return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kInvalidInputSize);
        }
    }

    // If the configured context AlgId does not support IV, Start will return a SecurityErrorDomain::kUnsupported error.
    if (pCtxMacBase_->SupportIv() == false) {
        if (iv.empty() == false) {
            return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUnsupported);
        }
    }

    // Reset return result
    ara::core::Result< void > resVoid{pCtxMacBase_->Start(iv)};
    if (!resVoid.HasValue()) {
        return ara::core::Result< void >::FromError(static_cast< ara::crypto::SecurityErrc >(resVoid.Error().Value()));
    }
    resVoid = pCtxSymmetricStream_->Start(iv);
    if (!resVoid.HasValue()) {
        return ara::core::Result< void >::FromError(static_cast< ara::crypto::SecurityErrc >(resVoid.Error().Value()));
    }
    eAeState_ = EAeWorkState::kAeStart;
    return ara::core::Result< void >::FromValue();
}
/// @brief Initialize the context for new data processing or generation (depending on the primitive).
///     If the IV size is larger than the maximum size supported by the algorithm, the implementation may only use the leading bytes of the sequence.
/// @param iv initialization secret seed
/// @returns  has value if Start sucess false otherwise
template < typename T_Mac, typename T_SymmetricStream >
inline ara::core::Result< void > PCtxAe_Base< T_Mac, T_SymmetricStream >::Start(SecretSeed const &iv) noexcept
{
    // You can determine whether setkey has been called by checking an identifier, or by checking whether the key inside pCtxMacBase_ is empty
    if (pCtxMacBase_->GetKey() == nullptr) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUninitializedContext);
    }

    // If the configured context AlgId does not support IV, Start will return a SecurityErrorDomain::kUnsupported error.
    if (pCtxMacBase_->SupportIv() == false) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUnsupported);
    }

    // If the size of the provided IV is not supported by the configured context AlgId, Start will return a SecurityErrorDomain::kInvalidInputSize error.
    uint32_t const ivMaxLength{pCtxMacBase_->GetIvMaxLength()};
    if (iv.GetPayloadSize() < ivMaxLength) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kInvalidInputSize);
    }

    AllowedUsageFlags const nKeyAllowedUsage{iv.GetAllowedUsage()};
    if (eCryptoTransform_ == CryptoTransform::kEncrypt) {
        if ((kAllowDataEncryption & nKeyAllowedUsage) != kAllowDataEncryption) {
            return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUsageViolation);
        }
    } else {
        if ((kAllowDataDecryption & nKeyAllowedUsage) != kAllowDataDecryption) {
            return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUsageViolation);
        }
    }

    // Reset return result
    ara::core::Result< void > resVoid{pCtxMacBase_->Start(iv)};
    if (!resVoid.HasValue()) {
        return ara::core::Result< void >::FromError(static_cast< ara::crypto::SecurityErrc >(resVoid.Error().Value()));
    }
    resVoid = pCtxSymmetricStream_->Start(iv);
    if (!resVoid.HasValue()) {
        return ara::core::Result< void >::FromError(static_cast< ara::crypto::SecurityErrc >(resVoid.Error().Value()));
    }
    eAeState_ = EAeWorkState::kAeStart;
    return ara::core::Result< void >::FromValue();
}
/// @brief Update the digest calculation with the specified RestrictedUseObject. This method is specifically for cases where the RestrictedUseObject is part of the "message".
template < typename T_Mac, typename T_SymmetricStream >
inline ara::core::Result< void >
/// @param in input data buffer
/// @returns has value if UpdateAssociatedData sucess false otherwise
PCtxAe_Base< T_Mac, T_SymmetricStream >::UpdateAssociatedData(RestrictedUseObject const &in) noexcept
{
    if (EAeWorkState::kAeFinish == eAeState_) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kInvalidUsageOrder);
    }
    // SWS_CRYPT_00909: If Start has not been called before, Update will return a SecurityErrorDomain::kProcessingNotStarted error.
    if ((eAeState_ != EAeWorkState::kAeStart) && (eAeState_ != EAeWorkState::kAeUpdate)) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kProcessingNotStarted);
    }

    ara::core::Result< void > const resVoid{pCtxMacBase_->Update(in)};
    if (!resVoid.HasValue()) {
        return ara::core::Result< void >::FromError(static_cast< ara::crypto::SecurityErrc >(resVoid.Error().Value()));
    }
    eAeState_ = EAeWorkState::kAeUpdate;
    return ara::core::Result< void >::FromValue();
}
/// @brief Update the digest calculation with a new block of associated data.
template < typename T_Mac, typename T_SymmetricStream >
inline ara::core::Result< void >
/// @param in input data buffer
/// @returns  has value if UpdateAssociatedData sucess false otherwise
PCtxAe_Base< T_Mac, T_SymmetricStream >::UpdateAssociatedData(ReadOnlyMemRegion const &in) noexcept
{
    // if ProcessConfidentialData has already been called
    if (EAeWorkState::kAeFinish == eAeState_) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kInvalidUsageOrder);
    }
    // if the digest calculation was not initiated by a call of the Start() method
    if ((eAeState_ != EAeWorkState::kAeStart) && (eAeState_ != EAeWorkState::kAeUpdate)) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kProcessingNotStarted);
    }

    // SWS_CRYPT_009098: If the configured context AlgId does not support IV, Start will return a SecurityErrorDomain::kUnsupported error.
    // Reset return result
    ara::core::Result< void > const resVoid{pCtxMacBase_->Update(in)};
    if (!resVoid.HasValue()) {
        return ara::core::Result< void >::FromError(static_cast< ara::crypto::SecurityErrc >(resVoid.Error().Value()));
    }
    eAeState_ = EAeWorkState::kAeUpdate;
    return ara::core::Result< void >::FromValue();
}
/// @brief Update the digest calculation with the specified bytes. This method facilitates handling of constant tags.
template < typename T_Mac, typename T_SymmetricStream >
inline ara::core::Result< void >
/// @param in input data buffer
/// @returns  has value if UpdateAssociatedData sucess false otherwise
PCtxAe_Base< T_Mac, T_SymmetricStream >::UpdateAssociatedData(std::uint8_t const &in) noexcept
{
    // if ProcessConfidentialData has already been called
    if (EAeWorkState::kAeFinish == eAeState_) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kInvalidUsageOrder);
    }
    // SWS_CRYPT_00909: If Start has not been called before, Update will return a SecurityErrorDomain::kProcessingNotStarted error.
    if ((eAeState_ != EAeWorkState::kAeStart) && (eAeState_ != EAeWorkState::kAeUpdate)) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kProcessingNotStarted);
    }

    ara::core::Result< void > const resVoid{pCtxMacBase_->Update(in)};
    if (!resVoid.HasValue()) {
        return ara::core::Result< void >::FromError(static_cast< ara::crypto::SecurityErrc >(resVoid.Error().Value()));
    }
    eAeState_ = EAeWorkState::kAeUpdate;
    return ara::core::Result< void >::FromValue();
}
/// @brief Get the maximum supported size for associated public data.
/// @brief Get maximal supported size of associated public data.
/// @returns maximal supported size of associated public data in bytes
/// @trace_id_sws={SWS_CRYPT_20103}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02309}
/// @threadsafety={Thread-safe}
template < typename T_Mac, typename T_SymmetricStream >
inline std::uint64_t PCtxAe_Base< T_Mac, T_SymmetricStream >::GetMaxAssociatedDataSize() const noexcept
{
    return pCtxSymmetricStream_->GetMaxKeyBitLength();
}
/// @brief Process data
/// @param stSrcData data to be processed
/// @param nDataLen data length
/// @param expectedTag expected value
/// @returns encrypted data
template < typename T_Mac, typename T_SymmetricStream >
inline ara::core::Result< ara::core::Vector< ara::core::Byte > > PCtxAe_Base< T_Mac, T_SymmetricStream >::ProcessData(
    uint8_t const *const stSrcData, uint32_t const nDataLen, ReadOnlyMemRegion const &expectedTag) const noexcept
{
    // Encrypt/decrypt data
    if ((stSrcData == nullptr) || (nDataLen == 0U)) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kLogicFault);
    }
    // Start
    ara::core::Vector< ara::core::Byte > vecEncrypto;
    uint32_t const nBlockSize{static_cast< uint32_t >(GetBlockSize())};
    int32_t const alignedBlock{static_cast< int32_t >(nDataLen) / static_cast< int32_t >(nBlockSize)};
    int32_t const nonAlignedData{static_cast< int32_t >(nDataLen) % static_cast< int32_t >(nBlockSize)};
    int32_t i{0};
    // Encrypt stream
    for (i = 0; i < alignedBlock; i++) {
        ReadOnlyMemRegion const memRegionSrc{T_TransBytes(stSrcData + static_cast< uint32_t >(i) * nBlockSize),
                                             static_cast< size_t >(nBlockSize)};
        ara::core::Result< ara::core::Vector< ara::core::Byte > > resultVecEncrypto{
            pCtxSymmetricStream_->ProcessBlocks(memRegionSrc)};
        if (!resultVecEncrypto.HasValue()) {
            return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
                SecurityErrorDomain::Errc::kLogicFault);
        }
        ara::core::Vector< ara::core::Byte > const vecTemp{std::move(resultVecEncrypto).Value()};
        std::ignore = vecEncrypto.insert(vecEncrypto.cend(), vecTemp.cbegin(), vecTemp.cend());
    }
    // Encrypt tail
    if (nonAlignedData > 0) {
        ReadOnlyMemRegion const memRegionSrcEnd{T_TransBytes(stSrcData + static_cast< uint32_t >(i) * nBlockSize),
                                                static_cast< size_t >(nonAlignedData)};
        ara::core::Result< ara::core::Vector< ara::core::Byte > > resultVecEncrypto{
            pCtxSymmetricStream_->FinishBytes(memRegionSrcEnd)};
        if (!resultVecEncrypto.HasValue()) {
            return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
                SecurityErrorDomain::Errc::kLogicFault);
        }
        ara::core::Vector< ara::core::Byte > const vecTemp{std::move(resultVecEncrypto).Value()};
        std::ignore = vecEncrypto.insert(vecEncrypto.cend(), vecTemp.cbegin(), vecTemp.cend());
    }

    ara::core::Vector< ara::core::Byte > returnResult;
    /// Encryption: Encrypt-and-MAC (E&M) // SWS_CRYPT_01804
    if (GetTransformation() == CryptoTransform::kEncrypt) {
        // Generate mac data
        std::ignore = pCtxMacBase_->Finish();
        ara::core::Result< ara::core::Vector< ara::core::Byte > > const resultVecMac{pCtxMacBase_->GetDigest(0U)};
        if (!resultVecMac.HasValue()) {
            return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
                SecurityErrorDomain::Errc::kLogicFault);
        }
        ara::core::Vector< ara::core::Byte > vecMac{resultVecMac.Value()};

        std::ignore = returnResult.insert(returnResult.cbegin(), vecEncrypto.cbegin(), vecEncrypto.cend());
        std::ignore = returnResult.insert(returnResult.cend(), vecMac.begin(), vecMac.end());
    } else {
        // Decrypt data // SWS_CRYPT_01805
        ara::core::Byte *const pData{vecEncrypto.data()};
        ReadOnlyMemRegion const memKeyData{T_TransBytes< ara::core::Byte >(pData), vecEncrypto.size()};
        ara::core::Result< void > const resVoid{pCtxMacBase_->Update(memKeyData)};
        if (!resVoid.HasValue()) {
            return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
                SecurityErrorDomain::Errc::kLogicFault);
        }

        ara::core::Result< ara::crypto::cryp::Signature::Uptrc > const resSignature{pCtxMacBase_->Finish(false)};
        if (!resSignature.HasValue()) {
            return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
                SecurityErrorDomain::Errc::kLogicFault);
        }
        // Verify data
        if (pCtxMacBase_->Compare(expectedTag, 0U)) {
            std::ignore = returnResult.insert(returnResult.cbegin(), vecEncrypto.cbegin(), vecEncrypto.cend());
        }
    }

    return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromValue(returnResult);
}
/// @brief Process confidential data.
///         The input buffer will be overwritten with the processed message. This function is the final call, i.e., all relevant data must have been provided.
///         Therefore, this function will check the authentication tag and only return the processed data if the tag is valid.
/// @brief Process confidential data.
///         The input buffer will be overwritten by the processed message.
///         This function is the final call, i.e. all associated data must have been already provided.
///         Hence, the function will check the authentication tag and only return the processed data, if the tag is
///         valid.
/// @param in the input buffer containing the full message
/// @param expectedTag  pointer to read only mem region
/// @returns  encrypted data
/// @trace_id_sws={SWS_CRYPT_23634}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02302}
/// @error: SecurityErrorDomain::kInvalidInputSize  if size of the input buffer is not divisible by the block size (see
/// @c GetBlockSize())
/// @error: SecurityErrorDomain::kProcessingNotStarted  if the data processing was not started by a call of the @c
/// Start() method
/// @error: SecurityErrorDomain::kAuthTagNotValid  if the processed data cannot be authenticated
/// @threadsafety={Thread-safe}
/// virtual ara::core::Result<ara::core::Vector<ara::core::Byte> > (ReadOnlyMemRegion in,
/// ReadOnlyMemRegion expectedTag = nullptr ) noexcept = 0;
template < typename T_Mac, typename T_SymmetricStream >
inline ara::core::Result< ara::core::Vector< ara::core::Byte > >
PCtxAe_Base< T_Mac, T_SymmetricStream >::ProcessConfidentialData(ReadOnlyMemRegion const &in,
                                                                 ReadOnlyMemRegion const &expectedTag) noexcept
{
    // SWS_CRYPT_00909: If Start has not been called before, Update will return a SecurityErrorDomain::kProcessingNotStarted error.
    // SWS_CRYPT_01802 (when decrypting) UpdateAssociatedData is optional for the user. In this case, the MAC should be calculated only based on the confidential data
    // SWS_CRYPT_01800 states that if ProcessConfidentialData is called, there is no need to call the UpdateAssociatedData function
    if ((eAeState_ != EAeWorkState::kAeStart) && (eAeState_ != EAeWorkState::kAeUpdate)) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kProcessingNotStarted);
    }

    /// @error: SecurityErrorDomain::kInvalidInputSize       if size of the input buffer is not divisible by the block
    /// size (see @c GetBlockSize())
    if ((in.size() % GetBlockSize()) != 0U) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kInvalidInputSize);
    }

    ara::core::Result< ara::core::Vector< ara::core::Byte > > resResult{
        std::move(ProcessData(in.data(), static_cast< uint32_t >(in.size()), expectedTag))};
    if (!resResult.HasValue()) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kAuthTagNotValid);
    }
    ara::core::Vector< ara::core::Byte > const returnResult{std::move(resResult).Value()};
    //@error SecurityErrorDomain::kAuthTagNotValid  if the processed data cannot be authenticated
    if (returnResult.empty()) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kAuthTagNotValid);
    }

    eAeState_ = EAeWorkState::kAeFinish;
    return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromValue(returnResult);
}
/// @brief Process confidential data.
///         The input buffer will be overwritten with the processed message. After calling this method, no other related data will be updated.
/// @brief Process confidential data.
///         The input buffer will be overwritten by the processed message
///         After this method is called no additional associated data may be updated.
/// @param inOut  the input buffer containing the full message
/// @param expectedTag  pointer to read only mem region
/// @returns  ara::core::Result<void>
/// @error: SecurityErrorDomain::kInvalidInputSize       if size of the input buffer is not divisible by the block size
/// (see @c GetBlockSize())
/// @error: SecurityErrorDomain::kProcessingNotStarted   if the data processing was not started by a call of the @c
/// Start() method
/// @error: SecurityErrorDomain::kAuthTagNotValid        if the processed data cannot be authenticated
/// virtual ara::core::Result<void> ProcessConfidentialData(ReadWriteMemRegion inOut, ReadOnlyMemRegion expectedTag =
/// nullptr) noexcept = 0;
template < typename T_Mac, typename T_SymmetricStream >
inline ara::core::Result< void > PCtxAe_Base< T_Mac, T_SymmetricStream >::ProcessConfidentialData(
    ReadWriteMemRegion &inOut, ReadOnlyMemRegion const &expectedTag) noexcept
{
    // SWS_CRYPT_00909: If Start has not been called before, Update will return a SecurityErrorDomain::kProcessingNotStarted error.
    if ((eAeState_ != EAeWorkState::kAeStart) && (eAeState_ != EAeWorkState::kAeUpdate)) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kProcessingNotStarted);
    }

    /// @error: SecurityErrorDomain::kInvalidInputSize       if size of the input buffer is not divisible by the block
    /// size (see @c GetBlockSize())
    if ((inOut.size() % GetBlockSize()) != 0U) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kInvalidInputSize);
    }

    ara::core::Result< ara::core::Vector< ara::core::Byte > > resResult{
        std::move(ProcessData(inOut.data(), static_cast< uint32_t >(inOut.size()), expectedTag))};
    if (!resResult.HasValue()) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kAuthTagNotValid);
    }

    vecResult_ = std::move(resResult).Value();

    //@error SecurityErrorDomain::kAuthTagNotValid  if the processed data cannot be authenticated
    if (vecResult_.empty()) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kAuthTagNotValid);
    }
    ara::core::Byte *const pData{vecResult_.data()};
    ReadWriteMemRegion out{T_TransBytes< ara::core::Byte >(pData), vecResult_.size()};
    // inOut has no method to increase length, temporarily use member variable swap
    std::swap(out, inOut);

    eAeState_ = EAeWorkState::kAeFinish;
    return ara::core::Result< void >::FromValue();
}
/// @brief Get the actual bit length of the key loaded into the context. If no key is set for the context, returns 0.
/// @brief Get actual bit-length of a key loaded to the context. If no key was set to the context yet then 0 is
/// returned.
/// @returns actual length of a key (now set to the algorithm context) in bits
template < typename T_Mac, typename T_SymmetricStream >
inline std::size_t PCtxAe_Base< T_Mac, T_SymmetricStream >::GetActualKeyBitLength() const noexcept
{
    return pCtxSymmetricStream_->GetActualKeyBitLength();
}
/// @brief Get the COUID of the key deployed to the context attached to this extension service. If no key is set for the context, returns an empty COUID (Nil).
/// @brief Get the COUID of the key deployed to the context this extension service is attached to.
/// If no key was set to the context yet then an empty COUID (Nil) is returned.
/// @returns the COUID of the CryptoObject
template < typename T_Mac, typename T_SymmetricStream >
inline CryptoObjectUid PCtxAe_Base< T_Mac, T_SymmetricStream >::GetActualKeyCOUID() const noexcept
{
    return pCtxSymmetricStream_->GetActualKeyCOUID();
}
/// @brief Get the allowed usage of this context (according to the key object properties loaded into this context).
///     If the context has not been initialized with a key object, it must return 0 (all flags reset).
/// @brief Get allowed usages of this context (according to the key object attributes loaded to this context).
/// If the context is not initialized by a key object yet then zero (all flags are reset) must be returned.
/// @returns a combination of bit-flags that specifies allowed usages of the context
template < typename T_Mac, typename T_SymmetricStream >
inline AllowedUsageFlags PCtxAe_Base< T_Mac, T_SymmetricStream >::GetAllowedUsage() const noexcept
{
    return pCtxSymmetricStream_->GetAllowedUsage();
}
/// @brief Get the maximum supported key length (in bits).
/// @brief Get maximal supported key length in bits.
/// @returns maximal supported length of the key in bits
template < typename T_Mac, typename T_SymmetricStream >
inline std::size_t PCtxAe_Base< T_Mac, T_SymmetricStream >::GetMaxKeyBitLength() const noexcept
{
    return pCtxSymmetricStream_->GetMaxKeyBitLength();
}
/// @brief Get the minimum supported key length (in bits).
/// @brief Get minimal supported key length in bits.
/// @returns minimal supported length of the key in bits
template < typename T_Mac, typename T_SymmetricStream >
inline std::size_t PCtxAe_Base< T_Mac, T_SymmetricStream >::GetMinKeyBitLength() const noexcept
{
    return pCtxSymmetricStream_->GetMinKeyBitLength();
}
/// @brief Verify support for a specific key length according to the context.
/// @brief Verify supportness of specific key length by the context.
/// @param keyBitLength key length: in bits
/// @returns @c true if provided value of the key length is supported by the context
template < typename T_Mac, typename T_SymmetricStream >
inline bool PCtxAe_Base< T_Mac, T_SymmetricStream >::IsKeyBitLengthSupported(std::size_t keyBitLength) const noexcept
{
    return pCtxSymmetricStream_->IsKeyBitLengthSupported(keyBitLength);
}
/// @brief Check whether a key is set for this context.
/// @brief Check if a key has been set to this context.
/// @return true if a key has been set to this context false otherwise
template < typename T_Mac, typename T_SymmetricStream >
inline bool PCtxAe_Base< T_Mac, T_SymmetricStream >::IsKeyAvailable() const noexcept
{
    return pCtxSymmetricStream_->IsKeyAvailable();
}
/// @brief Get the block (or internal buffer) size of the underlying algorithm.
/// @brief Get block (or internal buffer) size of the base algorithm.
/// @returns size of the block in bytes
template < typename T_Mac, typename T_SymmetricStream >
inline std::size_t PCtxAe_Base< T_Mac, T_SymmetricStream >::GetBlockSize() const noexcept
{
    return pCtxSymmetricStream_->GetBlockSize();
}
/// @brief Get the actual bit length of the IV loaded into the context.
/// @brief Get actual bit-length of an IV loaded to the context.
/// @param ivUid  optional pointer to a buffer for saving an @a COUID of a IV object now loaded to the context.
/// If the context was initialized by a @c SecretSeed object then the output buffer @c *ivUid must be filled
///       by @a COUID of this loaded IV object, in other cases @c *ivUid must be filled by all zeros.
/// @returns actual length of the IV (now set to the algorithm context) in bits
template < typename T_Mac, typename T_SymmetricStream >
inline std::size_t PCtxAe_Base< T_Mac, T_SymmetricStream >::GetActualIvBitLength(
    ara::core::Optional< CryptoObjectUid > &ivUid) const noexcept
{
    return pCtxSymmetricStream_->GetActualIvBitLength(ivUid);
}
/// @brief Get the default expected size of the initialization vector (IV) or nonce.
/// @brief Get default expected size of the Initialization Vector (IV) or nonce.
/// @returns default expected size of IV in bytes
template < typename T_Mac, typename T_SymmetricStream >
inline std::size_t PCtxAe_Base< T_Mac, T_SymmetricStream >::GetIvSize() const noexcept
{
    return pCtxSymmetricStream_->GetIvSize();
}
/// @brief Verify the validity of a specific initialization vector (IV) length.
/// @brief Verify validity of specific Initialization Vector (IV) length.
/// @param ivSize  the length of the IV in bytes
/// @returns @c true if provided IV length is supported by the algorithm and @c false otherwise
template < typename T_Mac, typename T_SymmetricStream >
inline bool PCtxAe_Base< T_Mac, T_SymmetricStream >::IsValidIvSize(std::size_t ivSize) const noexcept
{
    return pCtxSymmetricStream_->IsValidIvSize(ivSize);
}
//********************************/
template class PCtxAe_Base< PCtxMacAesCbc_128, PCtxSymmetricStream_Aes_Cfb128 >;
template class PCtxAe_Base< PCtxMacDesCbc, PCtxSymmetricStream_Des_Cfb >;
//********************************/
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara