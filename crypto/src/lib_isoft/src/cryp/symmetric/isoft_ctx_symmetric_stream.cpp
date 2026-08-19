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
/// @file       isoft_ctx_symmetric_stream.cpp
/// @brief      AutoSar-Crypto Encryption and Decryption Module
/// @details
/// @date       2022-02-16
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default Encryption and Decryption/Symmetric Encryption
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_01002
/// @unit_name=PCtxSymmetricStream
/// @unit_description=Stream Symmetric Encryption Base Class
/// @endcode
///
/// ================================================================

#include "ara/crypto/cryp/symmetric/isoft_ctx_symmetric_stream.h"

#include "ara/crypto/common/isoft_common_api.h"
#include "ara/crypto/cryp/cryobj/isoft_key_symmetric.h"
#include "ara/crypto/cryp/cryobj/isoft_key_symmetric_ipc.h"
#include "ara/crypto/cryp/isoft_crypto_provider.h"
#include "ara/crypto/cryp/isoft_service_crypto.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
//********************************/
/// @brief Constructor with parameters
/// @param cryptoProvider Crypto provider
/// @returns
/// @throws
PCtxSymmetricStream::PCtxSymmetricStream(PCryptoProvider &cryptoProvider) noexcept
    : PCtxSymmetricStream{cryptoProvider, {}, {}, nullptr, {}, nullptr, 0U, nullptr, false}
{
}
/// @brief Constructor with parameters
/// @param cryptoProvider Crypto provider
/// @param eWorkState Working state
/// @param eCryptoTransform Encryption direction: encrypt or decrypt, etc.
/// @param pSymmetricKey Pointer to symmetric key
/// @param secretSeedCouId Secret seed encryption material ID
/// @param buffOutput Output buffer
/// @param nCountInCache Number of items in cache
/// @param buffCache Cache buffer
/// @param isAfterPreocessNonAligned Whether ProcessBytes has been called
PCtxSymmetricStream::PCtxSymmetricStream(PCryptoProvider &cryptoProvider,
                                         EStreamWorkState const eWorkState,
                                         CryptoTransform const eCryptoTransform,
                                         SymmetricKey const *const pSymmetricKey,
                                         CryptoObjectUid const &secretSeedCouId,
                                         internal::PAutoBuff const *const buffOutput,
                                         uint32_t const nCountInCache,
                                         internal::PAutoBuff const *const buffCache,
                                         bool const isAfterPreocessNonAligned) noexcept
    : StreamCipherCtx{}  // NOLINT
    , cryptoProvider_{cryptoProvider}
    , eWorkState_{eWorkState}
    , eCryptoTransform_{eCryptoTransform}
    , pSymmetricKey_{nullptr}
    , secretSeedCouId_{secretSeedCouId}
    , buffOutput_{static_cast< uint32_t >(EBuffLen::kMaxOutputBuffLen)}
    , nCountInCache_{nCountInCache}
    , buffCache_{static_cast< uint32_t >(EBuffLen::kMaxCacheBuffLen)}
    , isAfterPreocessNonAligned_{isAfterPreocessNonAligned}
{
    std::ignore = pSymmetricKey;
    std::ignore = buffOutput;
    std::ignore = buffCache;
}
/// @brief Check whether the encryption context has been initialized and is ready for use. It checks all required values, including: key value, IV/seed, etc.
/// @name  IsInitialized
/// @returns  true if already init false otherwise
bool PCtxSymmetricStream::IsInitialized() const noexcept
{
    return (CryptoTransform::kEncrypt == eCryptoTransform_) || (CryptoTransform::kDecrypt == eCryptoTransform_);
}
/// @brief Get a reference to the Crypto Provider for this context.
/// @name  MyProvider
/// @returns Reference to the crypto provider
// PRQA S 4623 QAC /// @qac: AUTOSAR standard interface
inline CryptoProvider &PCtxSymmetricStream::MyProvider() const noexcept { return cryptoProvider_; }
// PRQA L:QAC
/// @brief Process the last part of the message (possibly not aligned to the block size boundary).
/// @name  FinishBytes
/// @param in Input data buffer
/// @returns Ciphertext data
ara::core::Result< ara::core::Vector< ara::core::Byte > > PCtxSymmetricStream::FinishBytes(
    ReadOnlyMemRegion const &in) noexcept
{
    /// @error: SecurityErrorDomain::kInsufficientCapacity   if capacity of the output buffer is not enough
    uint32_t const nBlockSize{static_cast< uint32_t >(GetBlockSize())};
    uint32_t const nOutBuffLen{buffOutput_.GetBuffLen()};
    size_t const nInLen{in.size()};

    /// @error: SecurityErrorDomain::kProcessingNotStarted   if data processing was not started by a call of the @c
    if (false == IsStarted()) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kProcessingNotStarted);
    }

    /// @error: SecurityErrorDomain::kInOutBuffersIntersect  if the input and output buffers partially intersect
    uint8_t const *const pOutputData{buffOutput_.Data(0U)};
    uint8_t const *const pInputData{in.data()};
    if (static_cast< std::size_t >(std::abs(pOutputData - pInputData)) < nBlockSize) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kInOutBuffersIntersect);
    }

    ara::core::Vector< ara::core::Byte > vecResult;
    // Encryption/decryption calculation bytewise
    if (IsBytewiseMode()) {
        ///  If (IsBytewiseMode() == true) then it must be: out.size() >= in.size()
        if (nOutBuffLen < nInLen) {
            return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
                SecurityErrorDomain::Errc::kInsufficientCapacity);
        }
        vecResult = _ProcessBlocks(in.data(), static_cast< uint32_t >(in.size()));
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromValue(std::move(vecResult));
    }

    // Encryption/decryption calculation blockwise
    /// Because we need to process both in and cache
    uint32_t const nNeedOutBuffLen{static_cast< uint32_t >((nInLen + static_cast< size_t >(nBlockSize) * 2U - 1U)
                                                           & (~(static_cast< size_t >(nBlockSize) - 1U)))};
    if ((nNeedOutBuffLen > nOutBuffLen)) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kInsufficientCapacity);
    }

    size_t const inLen{nCountInCache_ + in.size()};
    internal::PAutoBuff buffTotal{static_cast< uint32_t >(inLen)};
    std::ignore = buffTotal.AddData(buffCache_.Data(0U), nCountInCache_);
    std::ignore = buffTotal.AddData(in.data(), static_cast< uint32_t >(in.size()));
    /// Process the last part
    /// This method may be used for processing of a whole message in a single call (in any mode)!
    size_t const alignedNum{inLen / nBlockSize};
    size_t const totalLen{alignedNum * nBlockSize};
    if (totalLen > 0U) {
        internal::PAutoBuff buffOutput{static_cast< uint32_t >(totalLen)};
        uint32_t const nOutLen{DoCipherLocal(buffOutput, in.data(), static_cast< uint32_t >(totalLen), true)};
        if (nOutLen > 0U) {
            uint8_t const *const pData{buffOutput.Data(0U)};
            for (uint32_t j{0U}; j < nOutLen; j++) {
                vecResult.push_back(ara::core::Byte(*(pData + j)));
            }
        }
    }

    size_t const nonAlignedSize{inLen % nBlockSize};
    if (nonAlignedSize == 0U) {
        /// Indicates that non-aligned data has been processed
        eWorkState_                = EStreamWorkState::kFinish;
        isAfterPreocessNonAligned_ = false;
        _ClearCache();
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromValue(std::move(vecResult));
    }

    internal::PAutoBuff buffInput{nBlockSize};
    std::ignore = buffInput.AddData(buffTotal.Data(0U) + totalLen, static_cast< uint32_t >(inLen - totalLen));
    size_t const paddingLen{_PaddingInput(buffInput)};
    ara::core::Vector< ara::core::Byte > vecResultPadding;
    vecResultPadding = _ProcessBlocks(buffInput.Data(0U), static_cast< uint32_t >(paddingLen));
    if (vecResultPadding.empty()) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kLogicFault);
    }
    std::ignore = vecResult.insert(vecResult.cend(), vecResultPadding.begin(),
                                   vecResultPadding.begin() + static_cast< std::ptrdiff_t >(inLen));
    /// Indicates that non-aligned data has been processed, data stream processing completed
    eWorkState_                = EStreamWorkState::kFinish;
    isAfterPreocessNonAligned_ = false;
    _ClearCache();

    return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromValue(std::move(vecResult));
}
/// @brief Check the operation mode of byte attributes.
/// @name  IsBytewiseMode
/// @returns  true if byte mode false otherwise
bool PCtxSymmetricStream::IsBytewiseMode() const noexcept
{
    /// @returns @c true  if the mode can process messages the byte-by-byte (without padding up to the block boundary)
    ///      and @c false if only the block-by-block (only full blocks can be processed, the padding is mandatory)
    return true;  // 2022-02-17 hanjingjing: Default uses stream in "byte" units
}
/// @brief Get the transformation type configured for this context: kEncrypt or kDecrypt.
/// @name  GetTransformation
/// @returns kEncrypt or kDecrypt
ara::core::Result< CryptoTransform > PCtxSymmetricStream::GetTransformation() const noexcept
{
    /// @error: SecurityErrorDomain::kUninitializedContext  if the transformation direction of this context
    ///                is configurable during an initialization, but the context was not initialized yet
    if (false == IsInitialized()) {
        return ara::core::Result< CryptoTransform >::FromError(SecurityErrorDomain::Errc::kUninitializedContext);
    }
    return ara::core::Result< CryptoTransform >::FromValue(eCryptoTransform_);
}
/// @brief Check whether seek operations are supported in the current mode.
/// @name  IsSeekableMode
/// @returns true if seekable false otherwise
bool PCtxSymmetricStream::IsSeekableMode() const noexcept
{
    /// @returns @c true the seek operation is supported in the current mode and @c false otherwise
    return false;  // 2022-02-17 hanjingjing: Seek operation not supported by default  //2023-08-16 Currently, stream encryption is implemented based on CFB and OFB modes, seekmode is not supported
}
/// @brief Process the initial part of the message aligned to the block size boundary.
///         This is a copy-optimized method that does not use an internal cache buffer! It can only be used before processing any non-block-aligned boundary data.
///         Pointers to input and output buffers must be aligned to the block size boundary! Input and output buffers can completely overlap, but they must not partially intersect!
/// @name  ProcessBlocks
/// @returns  ara::core::Result<ara::core::Vector<ara::core::Byte> >
/// @param in Input data buffer
/// @return Ciphertext data
ara::core::Result< ara::core::Vector< ara::core::Byte > > PCtxSymmetricStream::ProcessBlocks(
    ReadOnlyMemRegion const &in) noexcept
{
    size_t const inLen{in.size()};
    size_t const blockSize{GetBlockSize()};
    /// @error SecurityErrorDomain::kIncompatibleArguments  if sizes of the input and output buffers are not equal
    if ((buffOutput_.GetBuffLen() < inLen) || (inLen == 0U)) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kIncompatibleArguments);
    }
    /// @error: SecurityErrorDomain::kInvalidInputSize       if size of the input buffer is not divisible by the block
    /// size (see @c GetBlockSize())
    if ((inLen % blockSize) != 0U) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kInvalidInputSize);
    }
    /// @error: SecurityErrorDomain::kInOutBuffersIntersect  if the input and output buffers partially intersect
    uint8_t const *const pOutputData{buffOutput_.Data(0U)};
    uint8_t const *const pInputData{in.data()};
    if (static_cast< std::size_t >(std::abs(pOutputData - pInputData)) < blockSize) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kInOutBuffersIntersect);
    }
    /// @error: SecurityErrorDomain::kInvalidUsageOrder      if this method is called after processing of non-aligned
    /// data (to the block-size boundary)
    if (isAfterPreocessNonAligned_) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kInvalidUsageOrder);
    }
    /// @error: SecurityErrorDomain::kProcessingNotStarted   if the data processing was not started by a call of the @c
    /// Start() method
    if (false == IsStarted()) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kProcessingNotStarted);
    }
    ara::core::Vector< ara::core::Byte > vecResult;
    internal::PAutoBuff &buffOutput{buffOutput_};
    uint32_t const nOutLen{DoCipherLocal(buffOutput, in.data(), static_cast< uint32_t >(inLen), true)};
    if (nOutLen <= 0U) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kLogicFault);
    }
    uint8_t *const pOutBuff{buffOutput.Data(0U)};
    /// input output len equal
    for (uint32_t i{0U}; i < nOutLen; ++i) {
        vecResult.push_back(ara::core::Byte(*(pOutBuff + i)));
    }
    return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromValue(std::move(vecResult));
}
/// @brief Process the initial part of the message aligned to the block size boundary.
///         This is a copy-optimized method that does not use an internal cache buffer! It can be used for the first block of non-block-aligned data processing. Pointers to input and output buffers must be aligned to the block size boundary!
/// @name  ProcessBlocks
/// @param inOut Pointer to the input-output buffer
/// @returns  has value if ProcessBlocks sucess false otherwise
ara::core::Result< void > PCtxSymmetricStream::ProcessBlocks(ReadWriteMemRegion &inOut) noexcept
{
    size_t const nBlockSize{GetBlockSize()};
    size_t const inLen{inOut.size()};

    if (inLen == 0U) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kInvalidInputSize);
    }

    /// @error: SecurityErrorDomain::kInvalidUsageOrder      if this method is called after processing of non-aligned
    /// data (to the block-size boundary)
    if (isAfterPreocessNonAligned_) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kInvalidUsageOrder);
    }
    /// @error: SecurityErrorDomain::kProcessingNotStarted   if the data processing was not started by a call of the @c
    /// Start() method
    if (false == IsStarted()) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kProcessingNotStarted);
    }

    /// This function can handle the first block of non-block-aligned data processing
    if (inLen < nBlockSize) {
        internal::PAutoBuff buffInput{static_cast< uint32_t >(nBlockSize)};
        std::ignore = buffInput.AddData(inOut.data(), static_cast< uint32_t >(inLen));
        size_t const paddingLen{_PaddingInput(buffInput)};
        ara::core::Vector< ara::core::Byte > vecResultPadding;
        vecResultPadding = _ProcessBlocks(buffInput.Data(0U), static_cast< uint32_t >(paddingLen));
        for (size_t i{0U}; i < inLen; i++) {
            inOut[i] = static_cast< uint8_t >(vecResultPadding[i]);
        }
        return ara::core::Result< void >::FromValue();
    }

    /// @error: SecurityErrorDomain::kInvalidInputSize       if size of the @c inOut buffer is not divisible by the
    /// block size (see @c GetBlockSize())
    if ((inLen % nBlockSize) != 0U) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kInvalidInputSize);
    }
    internal::PAutoBuff &buffOutput{buffOutput_};
    uint32_t const nOutLen{DoCipherLocal(buffOutput, inOut.data(), static_cast< uint32_t >(inLen), true)};
    if (nOutLen <= 0U) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kLogicFault);
    }
    uint8_t *const pOutBuff{buffOutput.Data(0U)};
    /// input output len equal
    for (uint32_t i{0U}; i < nOutLen; ++i) {
        inOut[static_cast< size_t >(i)] = static_cast< uint8_t >(*(pOutBuff + i));
    }
    return ara::core::Result< void >::FromValue();
}
/// @brief Process a non-final part of the message (not aligned to the block size boundary).
///         If (IsBytewiseMode() == false) then it must be: bs = GetBlockSize(), out.size() >= (((in.size() + bs - 1) /
///         bs) * bs) If (IsBytewiseMode() == true) then it must be: out.size() >= in.size()
///         Input and output buffers must not intersect! This method is "copy-ineffective", so it should only be used when the application cannot control the chunking of the original message!
/// @name  ProcessBytes
/// @param in Input data buffer
/// @returns Ciphertext data
ara::core::Result< ara::core::Vector< ara::core::Byte > > PCtxSymmetricStream::ProcessBytes(
    ReadOnlyMemRegion const &in) noexcept
{
    size_t const inLen{in.size()};
    uint32_t const outBufLen{buffOutput_.GetBuffLen()};
    size_t const blockSize{GetBlockSize()};
    /// @error: SecurityErrorDomain::kInsufficientCapacity   if the output buffer has capacity insufficient for placing
    /// of the transformation result
    if ((outBufLen < inLen) || (inLen == 0U)) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kInsufficientCapacity);
    }
    /// @error: SecurityErrorDomain::kInOutBuffersIntersect  if the input and output buffers intersect
    uint8_t const *const pOutputData{buffOutput_.Data(0U)};
    uint8_t const *const pInputData{in.data()};
    if (static_cast< std::size_t >(std::abs(pOutputData - pInputData)) < blockSize) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kInOutBuffersIntersect);
    }
    /// @error: SecurityErrorDomain::kProcessingNotStarted   if data processing was not started by a call of the @c
    /// Start() method
    if (false == IsStarted()) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kProcessingNotStarted);
    }

    ara::core::Vector< ara::core::Byte > vecResult;
    // bytewise mode
    if (IsBytewiseMode()) {
        vecResult = _ProcessBlocks(in.data(), static_cast< uint32_t >(inLen));
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromValue(std::move(vecResult));
    }

    // blockwise mode
    if (outBufLen < ((inLen + blockSize - 1U) & (~(blockSize - 1U)))) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kInsufficientCapacity);
    }
    size_t const alignedNum{inLen / blockSize};
    size_t const totalLen{alignedNum * blockSize};
    if (totalLen > 0U) {
        internal::PAutoBuff buffOutput{static_cast< uint32_t >(totalLen)};
        uint32_t const nOutLen{DoCipherLocal(buffOutput, in.data(), static_cast< uint32_t >(totalLen), true)};
        if (nOutLen > 0U) {
            uint8_t const *const pData{buffOutput.Data(0U)};
            for (uint32_t j{0U}; j < nOutLen; j++) {
                vecResult.push_back(ara::core::Byte(*(pData + j)));
            }
        }
    }
    /// Cache the part that is not aligned
    size_t const nonAlignedSize{inLen % blockSize};
    if (nonAlignedSize != 0U) {
        uint8_t const *const inData{in.data()};
        std::ignore = _SaveToCache(inData + totalLen, static_cast< int32_t >(nonAlignedSize));
        /// Indicates that non-aligned data is being processed; at this time, ProcessBlocks cannot be called, only finishBytes can process non-aligned data
        isAfterPreocessNonAligned_ = true;
    }
    return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromValue(std::move(vecResult));
}
/// @brief Clear the encryption context.
/// @name  Reset
/// @returns  has value if reset sucess false otherwise
ara::core::Result< void > PCtxSymmetricStream::Reset() noexcept
{
    eCryptoTransform_ = CryptoTransform::kUnknown;
    pSymmetricKey_    = nullptr;
    eWorkState_       = EStreamWorkState::kNotStarted;
    buffCache_.ResetData();
    nCountInCache_             = 0U;
    isAfterPreocessNonAligned_ = false;
    return ara::core::Result< void >::FromValue();
}
/// @brief Set the position of the next byte in the encryption/decryption gamma stream.
/// @name  Seek
/// @param offset Offset
/// @param fromBegin Whether to start from the Begin position
/// @returns   has value if Seek sucess false otherwise
ara::core::Result< void > PCtxSymmetricStream::Seek(std::int64_t offset, bool fromBegin) noexcept
{
    /// @error: SecurityErrorDomain::kUnsupported            if the seek operation is not supported by the current mode
    if (false == IsSeekableMode()) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUnsupported);
    }
    /// @error: SecurityErrorDomain::kProcessingNotStarted   if the data processing was not started by a call of the @c
    /// Start() method
    if (false == IsStarted()) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kProcessingNotStarted);
    }
    /// @error: SecurityErrorDomain::kBelowBoundary          if the @c offset value is incorrect (in context of the the
    /// @c fromBegin argument),
    ///         i.e. it points before begin of the stream (note: it is an optional error condition)
    if (fromBegin && (offset < 0)) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kBelowBoundary);
    }
    /// @error: SecurityErrorDomain::kInvalidArgument        if the offset is not aligned to the required boundary (see
    /// @c IsBytewiseMode())
    if (false == IsBytewiseMode()) {
        if ((static_cast< std::size_t >(std::abs(offset)) % GetBlockSize()) != 0U) {
            return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kInvalidArgument);
        }
    }
    return DoSeek(offset, fromBegin);
}
/// @brief Set (deploy) a key to the stream cipher algorithm context.
/// @name  SetKey
/// @param key Symmetric key
/// @param transform Encryption direction: encrypt or decrypt
/// @returns   has value if SetKey sucess false otherwise
ara::core::Result< void > PCtxSymmetricStream::SetKey(SymmetricKey const &key, CryptoTransform transform) noexcept
{
    //     SetKey shall return a kIncompatibleObject error, if the provided Symmet
    // ric Key belongs to a different ara::crypto::cryp::CryptoProvider in
    // stance. Legacy issue

    /// @error: SecurityErrorDomain::kIncompatibleObject     if the provided key object is incompatible with this
    /// symmetric key context
    ///         If the provided key object is incompatible with this symmetric key context
    if (false == CheckKey(key)) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kIncompatibleObject);
    }
    /// @error: SecurityErrorDomain::kUsageViolation         if the transformation type associated with this context
    ///                (taking into account the direction specified by @c transform) is prohibited by the "allowed
    ///                usage" restrictions of provided key object
    ///         If the "allowed usage" restrictions of the provided key object prohibit the transformation type associated with this context (considering the direction specified by @c transform)
    if ((transform != CryptoTransform::kEncrypt) && (transform != CryptoTransform::kDecrypt)) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kInvalidArgument);
    }

    AllowedUsageFlags const keyUsage{key.GetAllowedUsage()};
    if (((CryptoTransform::kEncrypt == transform) && (kAllowDataEncryption != (keyUsage & kAllowDataEncryption)))
        || ((CryptoTransform::kDecrypt == transform) && (kAllowDataDecryption != (keyUsage & kAllowDataDecryption)))) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUsageViolation);
    }
    pSymmetricKey_    = &key;
    eCryptoTransform_ = transform;
    return ara::core::Result< void >::FromValue();
}

/// @brief Initialize the context for a new data stream processing or generation (depending on the primitive).
///         If the IV size is larger than the maximum supported size of the algorithm, the implementation may only use the leading bytes of the sequence.
/// @name  Start
/// @param iv Initialization vector
/// @returns  has value if Start sucess false otherwise
ara::core::Result< void > PCtxSymmetricStream::Start(ReadOnlyMemRegion const &iv) noexcept
{
    /// @error: SecurityErrorDomain::kUninitializedContext   if the context was not initialized by deploying a key
    if ((false == IsInitialized()) || (false == IsKeyAvailable())) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUninitializedContext);
    }
    /// @error: SecurityErrorDomain::kInvalidInputSize       if the size of provided IV is not supported (i.e. if it is
    /// not enough for the initialization)
    uint32_t const nIvSize{static_cast< uint32_t >(GetIvSize())};
    if (nIvSize > 0U) {
        if (false == IsValidIvSize(iv.size())) {
            return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kInvalidInputSize);
        }
    }
    /// @error: SecurityErrorDomain::kUnsupported            if the base algorithm (or its current implementation)
    ///                principally doesn't support the IV variation, but provided IV value is not empty, i.e. if
    ///                <tt>(iv.empty() == false)</tt>
    // If the underlying algorithm (or its current implementation) does not fundamentally support IV variants, but the provided IV value is not empty
    if (0U == nIvSize) {
        if (false == iv.empty()) {
            return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUnsupported);
        }
    }
    if (nIvSize > 0U) {
        DoInit(iv);
    }
    eWorkState_ = EStreamWorkState::kStart;
    return ara::core::Result< void >::FromValue();
}
/// @brief Initialize the context for a new data stream processing or generation (depending on the primitive).
///         If the IV size is larger than the maximum supported size of the algorithm, the implementation may only use the leading bytes of the sequence.
/// @name  Start
/// @param iv Initialization secret seed
/// @returns  has value if Start sucess false otherwise
ara::core::Result< void > PCtxSymmetricStream::Start(SecretSeed const &iv) noexcept
{
    /// @error: SecurityErrorDomain::kUninitializedContext   if the context was not initialized by deploying a key
    if ((false == IsInitialized()) || (false == IsKeyAvailable())) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUninitializedContext);
    }
    /// @error: SecurityErrorDomain::kUsageViolation         if this transformation type is prohibited by the "allowed
    /// usage" restrictions of the provided @c SecretSeed object
    SecretSeed::Usage const ivUsage{iv.GetAllowedUsage()};
    if (((CryptoTransform::kEncrypt == eCryptoTransform_) && (kAllowDataEncryption != (ivUsage & kAllowDataEncryption)))
        || ((CryptoTransform::kDecrypt == eCryptoTransform_)
            && (kAllowDataDecryption != (ivUsage & kAllowDataDecryption)))) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUsageViolation);
    }
    /// @error: SecurityErrorDomain::kInvalidInputSize       if the size of provided IV is not supported (i.e. if it is
    /// not enough for the initialization)
    uint32_t const nIvSize{static_cast< uint32_t >(GetIvSize())};
    if (nIvSize > 0U) {
        if (false == IsValidIvSize(iv.GetPayloadSize())) {
            return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kInvalidInputSize);
        }
    }
    /// @error: SecurityErrorDomain::kUnsupported            if the base algorithm (or its current implementation)
    /// principally doesn't support the IV variation
    if (0U == nIvSize) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUnsupported);
    }

    DoInit(iv);
    /// Indicates
    eWorkState_ = EStreamWorkState::kStart;
    return ara::core::Result< void >::FromValue();
}
//********************************/
/// @brief Get the actual bit length of the key loaded into the context. If no key is set for the context, return 0.
/// @name  GetActualKeyBitLength
/// @returns Length of the key set for the context
std::size_t PCtxSymmetricStream::GetActualKeyBitLength() const noexcept
{
    if (false == IsKeyAvailable()) {
        return 0U;
    }
    return pSymmetricKey_->GetPayloadSize() * kInt_8U;
}
/// @brief Get the COUID of the key deployed to the context attached to this extended service. If no key is set for the context, return an empty COUID (Nil).
/// @name  GetActualKeyCOUID
/// @returns COUID of the key set for the context
CryptoObjectUid PCtxSymmetricStream::GetActualKeyCOUID() const noexcept
{
    if (false == IsKeyAvailable()) {
        return CryptoObjectUid{};
    }
    return pSymmetricKey_->GetObjectId().mCouid;
}
/// @brief Get the allowed usage of this context (according to the key object properties loaded into this context).
///         If the context has not been initialized with a key object, it must return 0 (all flags reset).
/// @name  GetAllowedUsage
/// @returns Allowed usage of the key object
AllowedUsageFlags PCtxSymmetricStream::GetAllowedUsage() const noexcept
{
    if (false == IsKeyAvailable()) {
        return kAllowPrototypedOnly;
    }
    return pSymmetricKey_->GetAllowedUsage();
}
/// @brief Verify support for a specific key length according to the context.
/// @name  IsKeyBitLengthSupported
/// @param keyBitLength Key length: in bits
/// @returns   @c true if provided value of the key length is supported by the context
bool PCtxSymmetricStream::IsKeyBitLengthSupported(std::size_t keyBitLength) const noexcept
{
    // By default, check that the Key.BitLength is between the minimum and maximum values: if the Ctx has special requirements, please implement this virtual function in the subclass
    if (keyBitLength < GetMinKeyBitLength()) {
        return false;
    }
    if (keyBitLength > GetMaxKeyBitLength()) {
        return false;
    }
    return true;
}
/// @brief Check whether a key is set for this context.
/// @name  IsKeyAvailable
/// @returns  true if a key has been set to this context false otherwise
bool PCtxSymmetricStream::IsKeyAvailable() const noexcept
{
    if (nullptr == pSymmetricKey_) {
        return false;
    }
    return true;
}
/// @brief Get the block (or internal buffer) size of the underlying algorithm.
/// @name  GetBlockSize
/// @returns Block (or internal buffer) size of the underlying algorithm
std::size_t PCtxSymmetricStream::GetBlockSize() const noexcept
{
    // 2022-02-17 hanjingjing: Default uses byte-oriented stream
    return 1U;
}
/// @brief Get the actual bit length of the IV loaded into the context.
/// @name  GetActualIvBitLength
/// @param ivUid UID of the initialization vector (IV)
/// @returns Actual bit length of the IV in the context
std::size_t PCtxSymmetricStream::GetActualIvBitLength(ara::core::Optional< CryptoObjectUid > &ivUid) const noexcept
{
    // 2022-02-17 hanjingjing: IV not supported by default
    std::ignore = ivUid;
    return 0U;
}
/// @brief Get the default expected size of the Initialization Vector (IV) or nonce.
/// @name  GetIvSize
/// @returns  default expected size of IV in bytes
std::size_t PCtxSymmetricStream::GetIvSize() const noexcept
{
    // 2022-02-17 hanjingjing: IV not supported by default
    return 0U;
}
/// @brief Verify the validity of a specific Initialization Vector (IV) length.
/// @name  IsValidIvSize
/// @param ivSize Initialization vector length
/// @returns @c true if provided IV length is supported by the algorithm and @c false otherwise
bool PCtxSymmetricStream::IsValidIvSize(std::size_t ivSize) const noexcept
{
    // 2022-02-17 hanjingjing: IV supported by default
    std::ignore = ivSize;
    return true;
}
//********************************/
/// @brief View current state of stream processing: whether started.
/// @name  IsStarted
/// @returns  true if has already started false otherwise
bool PCtxSymmetricStream::IsStarted() const noexcept
{
    return (EStreamWorkState::kStart == eWorkState_) || (EStreamWorkState::kUpdate == eWorkState_)
           || (EStreamWorkState::kFinish == eWorkState_);
}
/// @brief Check current state of stream processing: whether finished.
/// @name  IsFinished
/// @returns true if has already finished false otherwise
bool PCtxSymmetricStream::IsFinished() const noexcept { return EStreamWorkState::kFinish == eWorkState_; }
/// @brief Execute Seek operation
/// @name  DoSeek
/// @param offset Offset
/// @param fromBegin Whether starting from the Begin position
/// @returns  has value if DoSeek sucess false otherwise
ara::core::Result< void > PCtxSymmetricStream::DoSeek(std::int64_t offset, bool fromBegin) noexcept
{
    // 2022-02-17 hanjingjing: Seek not supported by default
    std::ignore = offset;
    std::ignore = fromBegin;
    return ara::core::Result< void >::FromValue();
}
//***************/
/// @brief Save to cache, return the actual amount saved
/// @name  _SaveToCache
/// @param pData Data: memory start address
/// @param nLen Data length: in bytes
/// @return Actual amount saved
int32_t PCtxSymmetricStream::_SaveToCache(uint8_t const *const pData, int32_t const nLen) noexcept
{
    uint32_t const nHaveCache{nCountInCache_};
    uint8_t *const pCachBuff{buffCache_.Data(0U)};
    for (int32_t i{0}; i < nLen; i++) {
        *(pCachBuff + nCountInCache_) = *(pData + i);
        nCountInCache_ += kInt_1U;
    }
    return static_cast< int32_t >(nCountInCache_) - static_cast< int32_t >(nHaveCache);
}
/// @brief Clear cache
/// @name  _ClearCache
/// @returns  void
void PCtxSymmetricStream::_ClearCache() noexcept
{
    buffCache_.ResetData();
    nCountInCache_ = 0U;
}
/// @brief Calculate encryption/decryption
/// @name  _ProcessBlocks
/// @returns  ara::core::Vector<ara::core::Byte>
/// @throws
/// @param pInputData Start address of input data in algorithm operation
/// @param nDataLen Data length
/// @return Ciphertext data
ara::core::Vector< ara::core::Byte > PCtxSymmetricStream::_ProcessBlocks(uint8_t const *const pInputData,
                                                                         uint32_t const nDataLen) noexcept
{
    ara::core::Vector< ara::core::Byte > vecResult;
    if (DoCipherLocal(buffOutput_, pInputData, nDataLen, false) > 0U) {
        uint8_t const *const pData{buffOutput_.Data(0U)};
        for (uint32_t i{0U}; i < nDataLen; ++i) {
            vecResult.push_back(ara::core::Byte(*(pData + i)));
        }
    }
    return vecResult;
}
/// @brief Set (deploy) a key for the symmetric algorithm context. For IPC
/// @name  _SetKeyIpc
/// @throws
/// @param stFuncName Function name used for IPC call
/// @param key Symmetric key
/// @param transform Encryption direction: Encrypt or Decrypt
/// @return has value if set key sucess false otherwise
ara::core::Result< void > PCtxSymmetricStream::_SetKeyIpc(ara::core::StringView const &stFuncName,
                                                          SymmetricKey const &key,
                                                          CryptoTransform const transform) noexcept
{
    PKeySymmetric_Ipc const *const pIpcKey{dynamic_cast< PKeySymmetric_Ipc const * >(&key)};
    if (pIpcKey == nullptr) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kIncompatibleObject);
    }

    if ((transform != CryptoTransform::kEncrypt) && (transform != CryptoTransform::kDecrypt)) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUsageViolation);
    }

    AllowedUsageFlags const keyUsage{key.GetAllowedUsage()};
    if (((CryptoTransform::kEncrypt == transform) && (kAllowDataEncryption != (keyUsage & kAllowDataEncryption)))
        || ((CryptoTransform::kDecrypt == transform) && (kAllowDataDecryption != (keyUsage & kAllowDataDecryption)))) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUsageViolation);
    }

    eCryptoTransform_ = transform;
    pSymmetricKey_    = &key;
    uint32_t const nSlotId{pIpcKey->GetSlotId()};

    keys::isoft_def::PIpcClient *const ipcClient{keys::isoft_def::PIpcClient::GetInstance_Keys()};
    if (ipcClient == nullptr) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kIncompatibleObject);
    }

    keys::isoft_def::PIpcAutoPacket aswMsg;  // Receive response packet
    bool const bDealIpc{ipcClient->DealIpcRequest(
        stFuncName, aswMsg,
        [nSlotId](keys::isoft_def::PIpcAutoPacket const *const pReqMsg) -> uint16_t {
            if (pReqMsg != nullptr) {
                keys::isoft_def::cryptoctx::PIpcReq_Setkey ipcReq;
                ipcReq.nIpcSlotID = nSlotId;
                std::ignore       = pReqMsg->AddDataToIpc(&ipcReq, sizeof(ipcReq));
            }
            return sizeof(keys::isoft_def::cryptoctx::PIpcReq_Setkey);
        },
        -1)};
    if (false == bDealIpc) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUsageViolation);
    }
    keys::isoft_def::cryptoctx::PIpcAsw_Setkey *const pIpcAsw{
        static_cast< keys::isoft_def::cryptoctx::PIpcAsw_Setkey * >(static_cast< void * >(aswMsg.GetIpcBody()))};
    if (0 != pIpcAsw->GetErrorID()) {
        return ara::core::Result< void >::FromError(
            TransformErrorID(static_cast< ara::core::ErrorDomain::CodeType >(pIpcAsw->GetErrorID())));
    }
    return ara::core::Result< void >::FromValue();
}
/// @brief Set (deploy) a key for the symmetric algorithm context. For IPC
/// @name  _DoCipherIpc
/// @returns  uint32_t
/// @throws
/// @param stFuncName Function name used for IPC call
/// @param buffOutput Output buffer
/// @param pInputData Start address of input data in algorithm operation
/// @param nDataLen Data length
/// @param pIvData Initialization data
/// @param nIvLen Initialization data length
/// @param alignedData Whether data is aligned
/// @return Ciphertext data length
uint32_t PCtxSymmetricStream::_DoCipherIpc(ara::core::StringView const &stFuncName,
                                           internal::PAutoBuff &buffOutput,
                                           uint8_t const *const pInputData,
                                           uint32_t const nDataLen,
                                           uint8_t const *const pIvData,
                                           uint32_t const nIvLen,
                                           bool const alignedData) noexcept
{
    PKeySymmetric_Ipc const *const pIpcKey{dynamic_cast< PKeySymmetric_Ipc const * >(pSymmetricKey_)};
    if (pIpcKey == nullptr) {
        return 0U;
    }
    keys::isoft_def::PIpcClient *const ipcClient{keys::isoft_def::PIpcClient::GetInstance_Keys()};
    if (ipcClient == nullptr) {
        return 0U;
    }
    if ((eCryptoTransform_ != CryptoTransform::kEncrypt) && (eCryptoTransform_ != CryptoTransform::kDecrypt)) {
        return 0U;
    }
    size_t const nBlockCount{GetBlockSize()};
    uint32_t const nSlotId{pIpcKey->GetSlotId()};

    keys::isoft_def::PIpcAutoPacket aswMsg;  // Receive response packet
    bool const bDealIpc{ipcClient->DealIpcRequest(
        stFuncName, aswMsg,
        [this, nSlotId, nBlockCount, pInputData, nDataLen, pIvData, nIvLen,
         alignedData](keys::isoft_def::PIpcAutoPacket const *const pReqMsg) -> uint16_t {
            if (pReqMsg != nullptr) {
                keys::isoft_def::cryptoctx::PIpcReq_docipher ipcReq;
                ipcReq.SetIpcSlotID(nSlotId);
                if (eCryptoTransform_ == CryptoTransform::kEncrypt) {
                    ipcReq.SetTransform(1U);
                } else if (eCryptoTransform_ == CryptoTransform::kDecrypt) {
                    ipcReq.SetTransform(kInt_2U);
                } else {
                    ipcReq.SetTransform(0U);
                }
                ipcReq.SetBlocksize(static_cast< uint32_t >(nBlockCount));
                ipcReq.SetDataLen(nDataLen);
                ipcReq.SetIvLen(nIvLen);
                ipcReq.SetOffset(GetOffset());
                ipcReq.SetCurrentPos(GetCurrentPos());
                ipcReq.SetFromBegin(GetFromBegin());
                ipcReq.SetFinishBytes(GetFinishBytes());
                ipcReq.SetAlgedData(alignedData);
                std::ignore = pReqMsg->AddDataToIpc(&ipcReq, sizeof(ipcReq));
                std::ignore = pReqMsg->AddDataToIpc(pInputData, static_cast< uint16_t >(nDataLen));
                if (ipcReq.GetIvLen() > 0U) {
                    std::ignore = pReqMsg->AddDataToIpc(pIvData, static_cast< uint16_t >(nIvLen));
                }
            }
            return static_cast< uint16_t >(sizeof(keys::isoft_def::cryptoctx::PIpcReq_docipher) + nDataLen + nIvLen);
        })};
    if (false == bDealIpc) {
        return 0U;
    }
    keys::isoft_def::cryptoctx::PIpcAsw_docipher *const pIpcAsw{
        static_cast< keys::isoft_def::cryptoctx::PIpcAsw_docipher * >(static_cast< void * >(aswMsg.GetIpcBody()))};
    if (0 != pIpcAsw->GetErrorID()) {
        return 0U;
    }
    uint8_t *const pData{pIpcAsw->GetData()};
    bool const ret{buffOutput.SetData(pData, pIpcAsw->GetDataLen(), false)};
    std::ignore = ret;
    SetCurrentPos(pIpcAsw->GetCurrentPos());
    SetFinishBytes(pIpcAsw->GetFinishBytes());
    SetOffset(0);
    SetFromBegin(true);
    return pIpcAsw->GetDataLen();
}
/// @brief Get function name from IPC packet: SetKey
/// @name  GetIpcFuncNameSetKey
/// @returns  ara::core::StringView const  &
ara::core::StringView PCtxSymmetricStream::GetIpcFuncNameSetKey() noexcept { return FUNC_NAME_Symmetric(SetKey); }
///
/// @brief Get key slot ID
/// @name  GetSlotId
/// @returns Key slot ID
uint32_t PCtxSymmetricStream::GetSlotId() noexcept
{
    PKeySymmetric_Ipc const *const pIpcKey{dynamic_cast< PKeySymmetric_Ipc const * >(pSymmetricKey_)};
    if (pIpcKey == nullptr) {
        return 0U;
    }
    return pIpcKey->GetSlotId();
}
/// @brief Fill input data using PKCS7Padding scheme
/// @name  _PaddingInput
/// @param buffInput Input buffer
/// @returns Filled data length
std::size_t PCtxSymmetricStream::_PaddingInput(internal::PAutoBuff &buffInput) const noexcept
{
    uint32_t const nNeedLen{_CalPaddingLen(static_cast< uint32_t >(buffInput.size()))};

    // PKCS7Padding fill
    uint8_t const nPaddingData{static_cast< uint8_t >(nNeedLen - buffInput.size())};
    std::ignore = buffInput.AddData(nPaddingData, static_cast< uint32_t >(nNeedLen - buffInput.size()));
    return static_cast< std::size_t >(nNeedLen);
}
/// @brief Calculate length after padding
/// @name  _CalPaddingLen
/// @param nInputLen Data length before padding
/// @returns Length after padding
uint32_t PCtxSymmetricStream::_CalPaddingLen(uint32_t const nInputLen) const noexcept
{
    std::size_t const nBlockSize{GetBlockSize()};
    if (0U == (nInputLen % nBlockSize)) {
        return static_cast< uint32_t >(nInputLen + nBlockSize);
    }
    return static_cast< uint32_t >((nInputLen + (nBlockSize - 1U)) & ~(nBlockSize - 1U));
}
//********************************/
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara