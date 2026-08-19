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
/// @file       isoft_ctx_hash_function.cpp
/// @brief      AutoSar-Crypto Encryption/Decryption Module
/// @details
/// @date       2021-12-21
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default Encryption/Hash Function
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_01020
/// @unit_name=PCtxHashFunction
/// @unit_description=Hash Context Base Class
/// @endcode
///
/// ================================================================

#include "ara/crypto/cryp/hash/isoft_ctx_hash_function.h"

#include <openssl/evp.h>

#include <memory>

#include "ara/crypto/common/isoft_data_type.h"
#include "ara/crypto/cryp/cryobj/isoft_crypto_primitive_id.h"
#include "ara/crypto/cryp/cryobj/isoft_key_symmetric.h"
#include "ara/crypto/cryp/cryobj/isoft_secret_seed.h"
#include "ara/crypto/cryp/isoft_crypto_provider.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
//********************************/
/// @brief (GetHashAlgId()) returns the EVP_MD object. By passing specified parameters via GetHashAlgId(), virtual functions are avoided.
/// @param algId Crypto primitive ID
/// @return Pointer to EVP_MD structure
EVP_MD const *PCtxHashFunction::GetEVP_MD(AlgId const algId) noexcept
{
    EVP_MD const *evpMd{nullptr};
    switch (static_cast< EPhCtxTypeID >(algId)) {
        case EPhCtxTypeID::kHashMd5: {
            evpMd = EVP_md5();
        } break;
        case EPhCtxTypeID::kHashSha1: {
            evpMd = EVP_sha1();
        } break;
        case EPhCtxTypeID::kHashSha2_224: {
            evpMd = EVP_sha224();
        } break;
        case EPhCtxTypeID::kHashSha2_256: {
            evpMd = EVP_sha256();
        } break;
        case EPhCtxTypeID::kHashSha2_384: {
            evpMd = EVP_sha384();
        } break;
        case EPhCtxTypeID::kHashSha2_512: {
            evpMd = EVP_sha512();
        } break;
        default: {
            break;
        }
    }
    return evpMd;
}
//********************************/
/// @brief PCtxHashFunction constructor.
/// @param cryptoProvider Encryption provider
PCtxHashFunction::PCtxHashFunction(PCryptoProvider &cryptoProvider) noexcept
    : PCtxHashFunction{cryptoProvider, EHashWorkState::kHashNotStarted, {}}
{
}
/// @brief Parameterized constructor
/// @param cryptoProvider Encryption provider
/// @param eHashState Hash working state
/// @param secretSeedCouId Secret seed crypto material ID
PCtxHashFunction::PCtxHashFunction(PCryptoProvider &cryptoProvider,
                                   EHashWorkState const eHashState,
                                   CryptoObjectUid const &secretSeedCouId) noexcept
    : HashFunctionCtx{}  // NOLINT
    , cryptoProvider_{cryptoProvider}
    , eHashState_{eHashState}
    , secretSeedCouId_{secretSeedCouId}
    , evpMdCtx_{EVP_MD_CTX_new()}
{
}
/// @brief Destructor
PCtxHashFunction::~PCtxHashFunction()
{
    if (evpMdCtx_ != nullptr) {
        EVP_MD_CTX_free(evpMdCtx_);
        evpMdCtx_ = nullptr;
    }
}
/// @brief Check if the encryption context has been initialized and is ready for use. It checks all required values, including: key values, IV/seed, etc.
/// @return @c true if the crypto context is completely initialized and ready to use, and @c false otherwise
bool PCtxHashFunction::IsInitialized() const noexcept { return eHashState_ == EHashWorkState::kHashStart; }
/// @brief Get a reference to the Crypto Provider associated with this context.
/// @return a reference to Crypto Provider instance that provides this context
// PRQA S 4623 QAC /// @qac: AUTOSAR standard interface
inline CryptoProvider &PCtxHashFunction::MyProvider() const noexcept { return cryptoProvider_; }
// PRQA L:QAC
//***************/
/// @brief SWS_CRYPT_21116: Obtain a part of the computed digest request.
///         The entire digest value is kept in the context until the next call to Start(), so any part of it can be extracted or verified again.
///         If (full_digest_size <= offset) then return_size = 0 bytes; Else return_size = min(output.size()，
///         (full_digest_size - offset)) bytes. This method can be implemented "inline" after the ara::core::memcpy() function is standardized.
/// @param offset Offset
/// @return number of digest bytes really stored to the output buffer (they are always <= @c output.size() and
///     denoted below as @a return_size)
ara::core::Result< ara::core::Vector< ara::core::Byte > > PCtxHashFunction::GetDigest(std::size_t offset) const noexcept
{
    /// SWS_CRYPT_21116、SWS_CRYPT_00919
    /// If the digest calculation has not been completed by calling the Finish() method, this function should return a SecurityErrorDomain::kProcessingNotFinished error.
    if (eHashState_ != EHashWorkState::kHashFinish) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kProcessingNotFinished);
    }

    ara::core::Vector< ara::core::Byte > resultByte;
    //
    uint8_t const *const pData{GetHashResult()};
    uint32_t const nDataLen{GetHashLength()};
    // SWS_CRYPT_00907 Retrieving the hash value
    for (uint32_t i{static_cast< uint32_t >(offset)}; i < nDataLen; ++i) {
        ara::core::Byte const byData{*(pData + i)};
        resultByte.push_back(byData);
    }

    return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromValue(resultByte);
}
//***************/
/// @brief SWS_CRYPT_21118: Initialize a context for new data stream processing or generation (depending on the primitive) without an IV.
/// @brief SWS_CRYPT_00908: Clear the current hash value
/// @return has vlaue if has started false otherwise
ara::core::Result< void > PCtxHashFunction::Start() noexcept
{
    // SWS_CRYPT_00908: If the configured context AlgId expects an IV but none is provided, Start will return a SecurityErrorDomain::kMissingArgument error.
    // error.
    if (SupportIv() == true) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kMissingArgument);
    }
    // Reset return result
    DoInit();
    secretSeedCouId_ = CryptoObjectUid();
    eHashState_      = EHashWorkState::kHashStart;
    return ara::core::Result< void >::FromValue();
}
/// @brief SWS_CRYPT_21110: Initialize a context for new data stream processing or generation (depending on the primitive).
///         If the IV size is larger than the maximum supported size by the algorithm, the implementation may use only the leading bytes of the sequence.
/// @param iv Initialization vector
/// @return has vlaue if has started false otherwise
ara::core::Result< void > PCtxHashFunction::Start(ReadOnlyMemRegion const &iv) noexcept
{
    if (SupportIv() == true) {
        /// If the configured context AlgId supports IV but no iv is provided,
        /// Start will return a SecurityErrorDomain::kMissingArgument error.
        if (iv.empty() == true) {
            return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kMissingArgument);
        }
        /// If the configured context AlgId supports IV but does not support the provided IV size (i.e., if it is insufficient for initialization),
        /// return a SecurityErrorDomain::kInvalidInputSize error.
        if (iv.size() < GetIvMaxLength()) {
            return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kInvalidInputSize);
        }
    }

    if (SupportIv() == false) {     // Not supported
        if (iv.empty() == false) {  // Provided iv even though not supported
            return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUnsupported);
        }
    }

    // Trim iv
    uint8_t const *piv{nullptr};
    if (iv.empty() == false) {  // Or _SupportIv() == true is fine
        // SWS_CRYPT_22110: If iv size exceeds max length, only use the first max length bytes
        ReadOnlyMemRegion const tempIv{iv.first(static_cast< size_t >(GetIvMaxLength()))};
        // Consider case where iv size exceeds max length
        piv = static_cast< uint8_t const * >(tempIv.data());
    }

    // Reset return result
    if (SupportIv() == false) {  // No iv needed
        DoInit();
    } else {  // IV required
        DoInitByIV(piv);
    }
    secretSeedCouId_ = CryptoObjectUid();
    eHashState_      = EHashWorkState::kHashStart;
    return ara::core::Result< void >::FromValue();
}
/// @brief SWS_CRYPT_21111: Initialize a context for new data stream processing or generation (depending on the primitive).
///         If the IV size is larger than the maximum supported size by the algorithm, the implementation may use only the leading bytes of the sequence.
/// @param iv Secret seed for initialization
/// @return has vlaue if has started false otherwise
ara::core::Result< void > PCtxHashFunction::Start(SecretSeed const &iv) noexcept
{
    uint32_t const ivMaxLength{GetIvMaxLength()};
    if (SupportIv() == true) {
        /// If the configured context AlgId supports IV but no iv is provided,
        /// Start will return a SecurityErrorDomain::kMissingArgument error.
        if (iv.GetPayloadSize() == kInt_0U) {
            return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kMissingArgument);
        }
        /// If the configured context AlgId supports IV, but does not support the size of the provided IV (i.e., if it is insufficient to initialize),
        /// then return SecurityErrorDomain::kInvalidInputSize error
        if (iv.GetPayloadSize() < ivMaxLength) {
            return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kInvalidInputSize);
        }
    }

    /// If the underlying algorithm (or its current implementation) basically does not support IV variables, return SecurityErrorDomain::kUnsupported error
    if (SupportIv() == false) {  // not supported
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUnsupported);
    }

    // trim iv
    uint8_t *const pTempIv{new uint8_t[static_cast< size_t >(ivMaxLength)]};

    ClipIV(iv, ivMaxLength, pTempIv);

    uint8_t const *const piv{pTempIv};

    /// reset return result
    if (SupportIv() == false) {  // iv not needed
        DoInit();
    } else {  // iv needed
        DoInitByIV(piv);
    }

    delete[](pTempIv);
    secretSeedCouId_ = iv.GetObjectId().mCouid;
    eHashState_      = EHashWorkState::kHashStart;
    return ara::core::Result< void >::FromValue();
}
/// @brief Trim iv to meet algorithm support size; if exceeded, keep only the first l bytes
/// @name   ClipIV
/// @param iv initialization secret seed
/// @param ivMaxLength maximum length of initialization seed
/// @param pTempIv memory address to store the result
void PCtxHashFunction::ClipIV(SecretSeed const &iv, uint32_t const ivMaxLength, uint8_t *const pTempIv) noexcept
{
    PSecretSeed const *const pSecretSeed{dynamic_cast< PSecretSeed const * >(&iv)};
    if (pSecretSeed == nullptr) {
        return;
    }

    // SWS_CRYPT_01202: If the iv size exceeds max length, only use the first max length bytes
    uint8_t const *const pSecretSeedData{pSecretSeed->GetSecretSeedData()};
    std::ignore = memcpy(pTempIv, pSecretSeedData, static_cast< size_t >(ivMaxLength));
}
/// @brief Update the digest calculation context with a new part of the message. This method is specifically for cases where the RestrictedUseObject is part of the "message".
/// @param in input data buffer
/// @return has vlaue if Update sucess false otherwise
ara::core::Result< void > PCtxHashFunction::Update(RestrictedUseObject const &in) noexcept
{
    // SWS_CRYPT_00909: If Start has not been called before, Update will return a SecurityErrorDomain::kProcessingNotStarted error.
    if ((eHashState_ != EHashWorkState::kHashStart) && (eHashState_ != EHashWorkState::kHashUpdate)) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kProcessingNotStarted);
    }
    uint8_t const *pData{nullptr};
    PKeySymmetric_Base const *const pKeySymmetricBase{dynamic_cast< PKeySymmetric_Base const * >(&in)};
    if (pKeySymmetricBase != nullptr) {
        pData = pKeySymmetricBase->GetKeySymmetric();
    } else {
        PSecretSeed const *const pSecretSeed{dynamic_cast< PSecretSeed const * >(&in)};
        if (pSecretSeed == nullptr) {
            return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kProcessingNotStarted);
        }
        pData = pSecretSeed->GetSecretSeedData();
    }
    if (pData == nullptr) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kProcessingNotStarted);
    }
    // SWS_CRYPT_00905
    DoUpdate(pData, static_cast< uint32_t >(in.GetPayloadSize()));
    eHashState_ = EHashWorkState::kHashUpdate;
    return ara::core::Result< void >::FromValue();
}
/// @brief Update the digest calculation context with a new part of the message.
/// @param in input data buffer
/// @return has vlaue if Update sucess false otherwise
ara::core::Result< void > PCtxHashFunction::Update(ReadOnlyMemRegion const &in) noexcept
{
    // SWS_CRYPT_00909: If Start has not been called before, Update will return a SecurityErrorDomain::kProcessingNotStarted error.
    if ((eHashState_ != EHashWorkState::kHashStart) && (eHashState_ != EHashWorkState::kHashUpdate)) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kProcessingNotStarted);
    }
    DoUpdate(in.data(), static_cast< uint32_t >(in.size()));
    eHashState_ = EHashWorkState::kHashUpdate;
    return ara::core::Result< void >::FromValue();
}
/// @brief Update the digest calculation context with a new part of the message. This method facilitates handling of constant tags.
/// @param in input data buffer
/// @return has vlaue if Update sucess false otherwise
ara::core::Result< void > PCtxHashFunction::Update(std::uint8_t const &in) noexcept
{
    // SWS_CRYPT_00909: If Start has not been called before, Update will return a SecurityErrorDomain::kProcessingNotStarted error.
    if ((eHashState_ != EHashWorkState::kHashStart) && (eHashState_ != EHashWorkState::kHashUpdate)) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kProcessingNotStarted);
    }
    DoUpdate(&in, 1U);
    eHashState_ = EHashWorkState::kHashUpdate;
    return ara::core::Result< void >::FromValue();
}
/// @brief Finish digest calculation and optionally generate a "signature" object. Only after calling this method can the digest be signed, verified, extracted, or compared.
/// @brief SWS_CRYPT_21115
/// @return has vlaue if Finish sucess false otherwise
ara::core::Result< ara::core::Vector< ara::core::Byte > > PCtxHashFunction::Finish() noexcept
{
    // SWS_CRYPT_00910:
    // If Finish is called multiple times for the same hash calculation, only the first call should apply the finalization step; that is, all subsequent calls can only return the hash value.
    if (eHashState_ == EHashWorkState::kHashFinish) {
        return GetDigest(0U);
    }
    // SWS_CRYPT_00906: If Start has not been successfully called before, Finish will return a SecurityErrorDomain::kProcessingNotStarted error.
    if (eHashState_ == EHashWorkState::kHashNotStarted) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kProcessingNotStarted);
    }
    // SWS_CRYPT_00906:
    // If Update has not been successfully called after the last Start, Finish will return a SecurityErrorDomain::kInvalidUsageOrder error
    if (eHashState_ == EHashWorkState::kHashStart) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kInvalidUsageOrder);
    }
    // SWS_CRYPT_00906 Finish hash value calculation and obtain hash value
    DoFinish();
    eHashState_ = EHashWorkState::kHashFinish;
    return GetDigest(0U);
}
//***************/
/// @brief Check the current status of stream processing: whether it has started.
/// @return true if has started false otherwise
bool PCtxHashFunction::IsStarted() const noexcept
{
    return (EHashWorkState::kHashStart == eHashState_) || (EHashWorkState::kHashUpdate == eHashState_);
}
/// @brief Check the current status of stream processing: whether it is finished.
/// @return true if has finished false otherwise
bool PCtxHashFunction::IsFinished() const noexcept { return eHashState_ == EHashWorkState::kHashFinish; }
/// @brief Compare the computed digest with the expected value.
/// @param expected expected value
/// @param offset offset
/// @return true if equal false otherwise
bool PCtxHashFunction::Compare(ReadOnlyMemRegion const &expected, std::size_t offset) const noexcept
{
    ara::core::Result< ara::core::Vector< ara::core::Byte > > const autoResult{GetDigest(offset)};
    if (false == autoResult.HasValue()) {
        return false;
    }
    ara::core::Vector< ara::core::Byte > const &vecCrcData{autoResult.Value()};
    if (vecCrcData.size() != expected.size()) {
        return false;
    }
    for (uint32_t i{0U}; i < vecCrcData.size(); ++i) {
        ara::core::Byte const byExpected{expected[static_cast< std::size_t >(i)]};
        if (vecCrcData[static_cast< std::size_t >(i)] != byExpected) {
            return false;
        }
    }
    return true;
}
//********************************/
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara