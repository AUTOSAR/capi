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
/// @file       isoft_ctx_symmetric_block.cpp
/// @brief      AutoSar-Crypto Encryption and Decryption Module
/// @details
/// @date       2022-01-20
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default Encryption and Decryption/Symmetric Encryption
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_01001
/// @unit_name=PCtxSymmetricBlock
/// @unit_description=Symmetric Block Cipher Context Base Class
/// @endcode
///
/// ================================================================

#include "ara/crypto/cryp/symmetric/isoft_ctx_symmetric_block.h"

#include "ara/crypto/common/isoft_assert.h"
#include "ara/crypto/common/isoft_common_api.h"
#include "ara/crypto/cryp/cryobj/isoft_key_symmetric.h"
#include "ara/crypto/cryp/cryobj/isoft_key_symmetric_ipc.h"
#include "ara/crypto/cryp/isoft_crypto_provider.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
//********************************/
/// @brief Interface for symmetric block cipher context with padding.
/// @param cryptoProvider Crypto provider
/// @returns
/// @throws
PCtxSymmetricBlock::PCtxSymmetricBlock(PCryptoProvider &cryptoProvider) noexcept
    : PCtxSymmetricBlock{cryptoProvider, CryptoTransform::kUnknown, nullptr}
{
}
/// @brief Constructor with parameters
/// @param cryptoProvider Crypto provider
/// @param eCryptoTransform Encryption direction: encrypt or decrypt, etc.
/// @param pSymmetricKey Pointer to symmetric key
PCtxSymmetricBlock::PCtxSymmetricBlock(PCryptoProvider &cryptoProvider,
                                       CryptoTransform const eCryptoTransform,
                                       SymmetricKey const *const pSymmetricKey) noexcept
    : SymmetricBlockCipherCtx{}  // NOLINT
    , cryptoProvider_{cryptoProvider}
    , eCryptoTransform_{eCryptoTransform}
    , pSymmetricKey_{pSymmetricKey}
{
}
//***************/  // CryptoContext interface
/// @brief Check whether the encryption context has been initialized and is ready for use. It checks all required values, including: key value, IV/seed, etc.
/// @returns  true if already init false otherwise
bool PCtxSymmetricBlock::IsInitialized() const noexcept
{
    return ((CryptoTransform::kEncrypt == eCryptoTransform_) || (CryptoTransform::kDecrypt == eCryptoTransform_));
}
/// @brief Get a reference to the Crypto Provider for this context.
/// @returns Reference to the crypto provider
// PRQA S 4623 QAC /// @qac: AUTOSAR standard interface
inline CryptoProvider &PCtxSymmetricBlock::MyProvider() const noexcept { return cryptoProvider_; }
// PRQA L:QAC
//***************/ // SymmetricBlockCipherCtx interface
/// @brief Get the transformation type configured for this context: kEncrypt or kDecrypt.
/// @returns  ara::core::Result<CryptoTransform>
ara::core::Result< CryptoTransform > PCtxSymmetricBlock::GetTransformation() const noexcept
{
    /// @error: SecurityErrorDomain::kUninitializedContext   if the transformation direction of this context
    ///                is configurable during an initialization, but the context was not initialized yet
    if (false == IsInitialized()) {
        return ara::core::Result< CryptoTransform >::FromError(SecurityErrorDomain::Errc::kUninitializedContext);
    }
    return ara::core::Result< CryptoTransform >::FromValue(eCryptoTransform_);
}
/// @brief Indicates whether the currently configured transformation accepts only full input data blocks.
/// @returns   @c true if the transformation requires the maximum size of input data and @c false otherwise
ara::core::Result< bool > PCtxSymmetricBlock::IsMaxInputOnly() const noexcept
{
    /// @error: SecurityErrorDomain::kUninitializedContext   if the transformation direction of this context
    ///                is configurable during an initialization, but the context was not initialized yet
    ///         If the transformation direction of this context is configurable during initialization but the context has not been initialized yet
    ara::core::Result< CryptoTransform > const result{GetTransformation()};
    if (!result.HasValue()) {
        return ara::core::Result< bool >::FromError(result.Error());
    }
    return ara::core::Result< bool >::FromValue(result.Value() == CryptoTransform::kDecrypt);
}
/// @brief Indicates whether the currently configured transformation can only produce full output data blocks.
/// @returns   @c true if the transformation can produce only the maximum size of output data and @c false otherwise
ara::core::Result< bool > PCtxSymmetricBlock::IsMaxOutputOnly() const noexcept
{
    /// @error: SecurityErrorDomain::kUninitializedContext   if the transformation direction of this context
    ///                is configurable during an initialization, but the context was not initialized yet
    ///         If the transformation direction of this context is configurable during initialization but the context has not been initialized yet
    ara::core::Result< CryptoTransform > const result{GetTransformation()};
    if (!result.HasValue()) {
        return ara::core::Result< bool >::FromError(result.Error());
    }
    return ara::core::Result< bool >::FromValue(result.Value() == CryptoTransform::kEncrypt);
}
/// @brief Process (encrypt/decrypt) an input block according to the cipher configuration.
/// @brief Process (encrypt / decrypt) an input block according to the cryptor configuration.
/// @param in Input data buffer
/// @param suppressPadding  if @c true then the method doesn't apply the padding, but the payload should fill the
/// whole block of the plain data
/// @returns    the output buffer
/// @trace_id_sws={SWS_CRYPT_23716}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02201}
/// @error: SecurityErrorDomain::kIncorrectInputSize    if the mentioned above rules about the input size is violated
/// @error:  SecurityErrorDomain::kInsufficientCapacity  if the @c out.size() is not enough to store the transformation
/// result
/// @error:  SecurityErrorDomain::kUninitializedContext  if the context was not initialized by a key value
/// @threadsafety={Thread-safe}
ara::core::Result< ara::core::Vector< ara::core::Byte > > PCtxSymmetricBlock::ProcessBlock(
    ReadOnlyMemRegion const &in, bool suppressPadding) const noexcept
{
    /// @error: SecurityErrorDomain::kUninitializedContext   if the context was not initialized by a key value
    if ((false == IsKeyAvailable()) || (false == IsInitialized())) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kUninitializedContext);
    }

    /// Legacy issue: 25-04-27 Write like this for now, check if it's correct after testing
    /// @error:  kInvalidInputSize error, if kDecrypt is configured and the size of the provided input buffer is not equal to the block-size.
    if (CryptoTransform::kDecrypt == eCryptoTransform_) {
        if (0U != (in.size() % GetBlockSize())) {
            return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
                SecurityErrorDomain::Errc::kInvalidInputSize);
        }
    }

    // Check whether the input and output data sizes meet expectations
    uint32_t const nNeedOutputLen{_CheckSize(in, suppressPadding)};
    /// @error: SecurityErrorDomain::kIncorrectInputSize    if the mentioned above rules about the input size is
    /// violated
    if (0U == nNeedOutputLen) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kInvalidInputSize);
    }
    /// @error:  SecurityErrorDomain::kInsufficientCapacity  if the @c out.size() is not enough to store the
    /// transformation result
    if (nNeedOutputLen > static_cast< uint32_t >(EBuffLen::kMaxOutputBuffLen)) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kInsufficientCapacity);
    }
    // Encryption/decryption calculation
    ara::core::Vector< ara::core::Byte > const vecResult;
    if ((false == suppressPadding) && ((CryptoTransform::kEncrypt == eCryptoTransform_))) {
        internal::PAutoBuff buffInput{static_cast< uint32_t >(in.size() + 2U * GetBlockSize())};  // kMaxInputBuffLen_
        std::size_t const nNewLen{_PaddingInput(buffInput, in)};
        if (0U == nNewLen) {
            return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
                SecurityErrorDomain::Errc::kInsufficientCapacity);
        }
        /// @error:  SecurityErrorDomain::kInsufficientCapacity  if the @c out.size() is not enough to store the
        /// transformation result
        return _ProcessBlocks(buffInput.Data(), static_cast< uint32_t >(nNewLen), false);
    }
    /// @error:  SecurityErrorDomain::kInsufficientCapacity  if the @c out.size() is not enough to store the
    /// transformation result
    return _ProcessBlocks(in.data(), static_cast< uint32_t >(in.size()),
                          (false == suppressPadding) && (CryptoTransform::kDecrypt == eCryptoTransform_));
}
/// @brief Process blocks without padding.
///         The input and output buffers must have the same size, and that size must be divisible by the block size (see GetBlockSize()). Pointers to input and output buffers must be aligned to the block size boundary!
/// @returns  ara::core::Result<ara::core::Vector<ara::core::Byte> >
/// @throws
/// @param in Input data buffer
/// @return an output data buffer
ara::core::Result< ara::core::Vector< ara::core::Byte > > PCtxSymmetricBlock::ProcessBlocks(
    ReadOnlyMemRegion const &in) const noexcept
{
    /// @error: SecurityErrorDomain::kUninitializedContext   if the context was not initialized by a key value
    if (false == IsKeyAvailable()) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kUninitializedContext);
    }
    /// @error: SecurityErrorDomain::kInvalidInputSize       if size of the input buffer is not divisible by the block
    /// size (see @c GetBlockSize())
    std::size_t const nInputLen{in.size()};
    std::size_t const nBlockSize{GetBlockSize()};
    if ((nInputLen % nBlockSize) != 0U) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kInvalidInputSize);
    }
    std::size_t const nOutputLen{(in.size() + (nBlockSize - 1U)) & ~(nBlockSize - 1U)};
    /// @error: SecurityErrorDomain::kIncompatibleArguments  if sizes of the input and output buffer are not equal
    if (nInputLen != nOutputLen) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kIncompatibleArguments);
    }
    // /// @error: SecurityErrorDomain::kInOutBuffersIntersect  if the input and output buffers partially intersect
    uint8_t const *const pInputData{in.data()};
    // Encryption/decryption calculation
    return {_ProcessBlocks(pInputData, static_cast< uint32_t >(nInputLen), false)};
}
/// @brief Clear the encryption context.
/// @returns has value if reset sucess false otherwise
ara::core::Result< void > PCtxSymmetricBlock::Reset() noexcept
{
    eCryptoTransform_ = CryptoTransform::kUnknown;
    pSymmetricKey_    = nullptr;
    return ara::core::Result< void >::FromValue();
}
/// @brief Set (deploy) a key for the symmetric algorithm context.
/// @param key Symmetric key
/// @param transform Encryption direction: encrypt or decrypt
/// @returns  has value if SetKey sucess false otherwise
ara::core::Result< void > PCtxSymmetricBlock::SetKey(SymmetricKey const &key, CryptoTransform transform) noexcept
{
    /// @error: SecurityErrorDomain::kIncompatibleObject     if the provided key object is incompatible with this
    /// symmetric key context
    ///         If the provided key object is incompatible with this symmetric key context
    if (false == CheckKey(key)) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kIncompatibleObject);
    }

    /// @error: SecurityErrorDomain::kInvalidArgument  if the provided transformation
    ///  direction is not allowed in Symmetric BlockCipher algorithm context.
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
//********************************/
/// @brief Get the actual bit length of the key loaded into the context. If no key is set for the context, return 0.
/// @returns Length of the key set for the context
std::size_t PCtxSymmetricBlock::GetActualKeyBitLength() const noexcept
{
    if (false == IsKeyAvailable()) {
        return 0U;
    }
    return pSymmetricKey_->GetPayloadSize() * kInt_8U;  // Convert to bit length
}
/// @brief Get the COUID of the key deployed to the context attached to this extended service. If no key is set for the context, return an empty COUID (Nil).
/// @returns COUID of the key set for the context
CryptoObjectUid PCtxSymmetricBlock::GetActualKeyCOUID() const noexcept
{
    if (false == IsKeyAvailable()) {
        return CryptoObjectUid{};
    }
    return pSymmetricKey_->GetObjectId().mCouid;
}
/// @brief Get the allowed usage of this context (according to the key object properties loaded into this context).
///         If the context has not been initialized with a key object, it must return 0 (all flags reset).
/// @returns Allowed usage of the key object
AllowedUsageFlags PCtxSymmetricBlock::GetAllowedUsage() const noexcept
{
    if (false == IsKeyAvailable()) {
        return kAllowPrototypedOnly;
    }
    return pSymmetricKey_->GetAllowedUsage();
}
/// @brief Verify support for a specific key length according to the context.
/// @param keyBitLength Key length: in bits
/// @returns  @c true if provided value of the key length is supported by the context
bool PCtxSymmetricBlock::IsKeyBitLengthSupported(std::size_t keyBitLength) const noexcept
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
/// @returns  true if a key has been set to this context false otherwise
bool PCtxSymmetricBlock::IsKeyAvailable() const noexcept
{
    if (nullptr == pSymmetricKey_) {
        return false;
    }
    return true;
}
/// @brief Get the maximum expected size of the input data block. The suppressPadding parameter will make it equal to the block size.
/// @param suppressPadding Whether to suppress padding
/// @returns  maximum size of the input data block in bytes
std::size_t PCtxSymmetricBlock::GetMaxInputSize(bool suppressPadding) const noexcept
{
    if (suppressPadding) {
        return GetBlockSize();
    }
    return static_cast< std::size_t >(EBuffLen::kMaxInputBuffLen);
}
/// @brief Get the maximum possible size of the output data block. If (IsEncryption() == true), the value returned by this method is independent of the suppressPadding parameter and will equal the block size.
/// true), the value returned by this method is independent of the suppressPadding parameter and will be equal to the block size.
/// @param suppressPadding Whether to suppress padding
/// @returns  maximum size of the output data block in bytes
std::size_t PCtxSymmetricBlock::GetMaxOutputSize(bool suppressPadding) const noexcept
{
    if (suppressPadding) {
        return GetBlockSize();
    }
    return static_cast< std::size_t >(EBuffLen::kMaxOutputBuffLen);
}
//********************************/
/// @brief Check input parameters
/// @param in Input data buffer
/// @param suppressPadding Whether to suppress padding
/// @returns Actual space required for output
uint32_t PCtxSymmetricBlock::_CheckSize(ReadOnlyMemRegion const &in, bool const suppressPadding) const noexcept
{
    if (static_cast< std::size_t >(EBuffLen::kMaxOutputBuffLen) < GetMaxOutputSize(suppressPadding)) {
        return 0U;
    }
    bool bCheckSize{false};  // Check whether the input and output data sizes meet expectations
    uint32_t nNeedOutputLen{0U};
    if ((CryptoTransform::kEncrypt == eCryptoTransform_)) {
        if (suppressPadding) {
            bCheckSize     = (0U == (in.size() % GetBlockSize()));
            nNeedOutputLen = static_cast< uint32_t >(in.size());
        } else  // Encryption and padding required
        {
            std::size_t const nNeedInputLen{_CalPaddingLen(static_cast< uint32_t >(in.size()), suppressPadding)};
            if (nNeedInputLen > GetMaxInputSize(suppressPadding)) {
                bCheckSize = false;
            } else {
                if (in.empty()) {
                    bCheckSize = false;
                } else {
                    bCheckSize = true;
                }
            }
            nNeedOutputLen = static_cast< uint32_t >(nNeedInputLen);
        }
    } else if (CryptoTransform::kDecrypt == eCryptoTransform_) {
        bCheckSize     = (0U == (in.size() % GetBlockSize()));
        nNeedOutputLen = static_cast< uint32_t >(in.size());
    } else {
    }
    if (bCheckSize) {
        return nNeedOutputLen;
    }
    return 0U;
}
/// @brief Perform encryption/decryption calculation
/// @param pInputData Starting address of input data in algorithm operation
/// @param nDataLen Data length
/// @param bUsePadding Whether to enable padding
/// @returns Encryption/decryption result
ara::core::Result< ara::core::Vector< ara::core::Byte > > PCtxSymmetricBlock::_ProcessBlocks(
    uint8_t const *const pInputData, uint32_t const nDataLen, bool const bUsePadding) const noexcept
{
    uint32_t nBufDataLen{0U};
    ara::core::Vector< ara::core::Byte > vecOut;
    vecOut.resize(GetBlockSize());
    if (static_cast< std::size_t >(std::abs(T_TransBytes(vecOut.data()) - pInputData)) < GetBlockSize()) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kInOutBuffersIntersect);
    }
    PKeySymmetric_Ipc const *const pPKeySymmetricIpc{dynamic_cast< PKeySymmetric_Ipc const * >(pSymmetricKey_)};
    // ipc
    if (pPKeySymmetricIpc != nullptr) {
        ara::core::Result< uint32_t > const res{_DoCipherIpc(vecOut, pInputData, nDataLen)};
        if (!res.HasValue()) {
            return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
                static_cast< SecurityErrorDomain::Errc >(res.Error().Value()));
        }
        nBufDataLen = res.Value();
    } else {
        ara::core::Result< uint32_t > const res{DoCipher(vecOut, pInputData, nDataLen)};
        if (!res.HasValue()) {
            return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
                static_cast< SecurityErrorDomain::Errc >(res.Error().Value()));
        }
        nBufDataLen = res.Value();
    }
    if (nBufDataLen == 0U) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kLogicFault);
    }
    if (bUsePadding) {
        uint32_t const dataLen{
            static_cast< uint32_t >(static_cast< uint8_t >(vecOut.at(static_cast< size_t >(nBufDataLen) - 1U)))};
        if (nBufDataLen < dataLen) {
            return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
                SecurityErrorDomain::Errc::kLogicFault);
        }
        nBufDataLen -= dataLen;

        vecOut.resize(static_cast< std::size_t >(nBufDataLen));
    }
    return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromValue(vecOut);
}
/// @brief Local encryption/decryption calculation, result stored in buffOutput_, return value is ciphertext length
/// @param vecOut Result output vector
/// @param pInputData Starting address of input data in algorithm operation
/// @param nDataLen Data length
/// @param alignedData Whether to align data
/// @returns Ciphertext length
ara::core::Result< uint32_t > PCtxSymmetricBlock::DoCipher(ara::core::Vector< ara::core::Byte > &vecOut,
                                                           uint8_t const *const pInputData,
                                                           uint32_t const nDataLen,
                                                           bool const alignedData) const noexcept
{
    std::ignore = alignedData;
    PKeySymmetric_Ipc const *const pPKeySymmetricIpc{dynamic_cast< PKeySymmetric_Ipc const * >(pSymmetricKey_)};
    if (pPKeySymmetricIpc != nullptr) {
        return ara::core::Result< uint32_t >::FromError(SecurityErrorDomain::Errc::kLogicFault);
    }
    std::size_t const nBlockLength{GetBlockSize()};
    int32_t const nBlockCount{static_cast< int32_t >(nDataLen) / static_cast< int32_t >(nBlockLength)};
    uint32_t const nVecOutLen{T_AlignNumberUp(nDataLen, static_cast< uint32_t >(GetBlockSize()))};
    try {
        vecOut.resize(static_cast< std::size_t >(nVecOutLen));
    } catch (std::bad_alloc &) {
        return ara::core::Result< uint32_t >::FromError(SecurityErrorDomain::Errc::kInsufficientCapacity);
    }
    for (int32_t i{0}; i < nBlockCount; ++i) {
        int32_t const nOffset{i * static_cast< int32_t >(nBlockLength)};
        uint8_t const *const pInput{pInputData + nOffset};
        uint8_t *const pOutput{T_TransBytes(vecOut.data()) + nOffset};
        DoSymmetric(pOutput, pInput, static_cast< uint32_t >(nBlockLength));
    }
    vecOut.resize(nBlockLength * static_cast< std::size_t >(nBlockCount));  // Need to confirm if this usage is correct
    return ara::core::Result< uint32_t >::FromValue(static_cast< uint32_t >(vecOut.size()));
}
/// @brief IPC encryption/decryption calculation, result stored in buffOutput_, return value is ciphertext length
/// @param vecOut Result output vector
/// @param pInputData Starting address of input data in algorithm operation
/// @param nDataLen Data length
/// @returns Ciphertext length
ara::core::Result< uint32_t > PCtxSymmetricBlock::_DoCipherIpc(ara::core::Vector< ara::core::Byte > &vecOut,
                                                               uint8_t const *const pInputData,
                                                               uint32_t const nDataLen) const noexcept
{
    PKeySymmetric_Ipc const *const pIpcKey{dynamic_cast< PKeySymmetric_Ipc const * >(pSymmetricKey_)};
    if (pIpcKey == nullptr) {
        return ara::core::Result< uint32_t >::FromError(SecurityErrorDomain::Errc::kLogicFault);
    }
    keys::isoft_def::PIpcClient *const ipcClient{keys::isoft_def::PIpcClient::GetInstance_Keys()};
    if (ipcClient == nullptr) {
        return ara::core::Result< uint32_t >::FromError(SecurityErrorDomain::Errc::kLogicFault);
    }
    if ((eCryptoTransform_ != CryptoTransform::kEncrypt) && (eCryptoTransform_ != CryptoTransform::kDecrypt)) {
        return ara::core::Result< uint32_t >::FromError(SecurityErrorDomain::Errc::kLogicFault);
    }
    size_t const nBlockCount{GetBlockSize()};
    uint32_t const nSlotId{pIpcKey->GetSlotId()};

    ara::core::StringView const stFuncName{GetIpcFuncNameDoCipher()};
    keys::isoft_def::PIpcAutoPacket aswMsg;  // Receive response packet
    bool const bDealIpc{ipcClient->DealIpcRequest(
        stFuncName, aswMsg,
        [this, nSlotId, nBlockCount, pInputData,
         nDataLen](keys::isoft_def::PIpcAutoPacket const *const pReqMsg) -> uint16_t {
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
                std::ignore = pReqMsg->AddDataToIpc(&ipcReq, sizeof(ipcReq));
                std::ignore = pReqMsg->AddDataToIpc(pInputData, static_cast< uint16_t >(nDataLen));
            }
            return static_cast< uint16_t >(sizeof(keys::isoft_def::cryptoctx::PIpcReq_docipher) + nDataLen);
        })};
    if (false == bDealIpc) {
        return ara::core::Result< uint32_t >::FromError(SecurityErrorDomain::Errc::kLogicFault);
    }
    keys::isoft_def::cryptoctx::PIpcAsw_common *const pIpcAsw{
        aswMsg.GetIpcBody< keys::isoft_def::cryptoctx::PIpcAsw_common >()};
    if (0 != pIpcAsw->GetErrorID()) {
        return ara::core::Result< uint32_t >::FromError(SecurityErrorDomain::Errc::kLogicFault);
    }
    uint32_t nCopyBodyLen{0U};
    try {
        vecOut.resize(static_cast< std::size_t >(pIpcAsw->GetDataLen()));
    } catch (std::bad_alloc &) {
        return ara::core::Result< uint32_t >::FromError(SecurityErrorDomain::Errc::kInsufficientCapacity);
    }
    uint8_t *const pOutData{T_TransBytes(vecOut.data())};
    uint32_t const nDealLen{aswMsg.DealBuffData(
        [pOutData, &nCopyBodyLen](uint32_t const nIndex, uint8_t const *const pData, uint32_t const nLen) -> uint32_t {
            if (0U == nIndex) {
                // For the first packet, the header needs to be removed
                keys::isoft_def::PIpcPac_Head const *const pIpcHead{
                    static_cast< keys::isoft_def::PIpcPac_Head const * >(static_cast< void const * >(pData))};
                keys::isoft_def::cryptoctx::PIpcAsw_docipher const *const pIpcBody{
                    pIpcHead->GetBody< keys::isoft_def::cryptoctx::PIpcAsw_docipher >()};
                int32_t const nCopyLen{std::min< int32_t >(
                    static_cast< int32_t >(pIpcBody->GetDataLen()),
                    static_cast< int32_t >(nLen) - static_cast< int32_t >(pIpcHead->GetHeadLen())
                        - static_cast< int32_t >(sizeof(keys::isoft_def::cryptoctx::PIpcAsw_docipher)))};

                std::ignore
                    = memcpy(pOutData + nCopyBodyLen, pIpcBody->GetData(), static_cast< std::size_t >(nCopyLen));
                nCopyBodyLen += static_cast< uint32_t >(nCopyLen);
            } else {
                std::ignore = memcpy(pOutData + nCopyBodyLen, pData, static_cast< std::size_t >(nLen));
                nCopyBodyLen += nLen;
            }
            return nLen;
        })};
    PH_ASSERT(nDealLen >= nCopyBodyLen);
    return ara::core::Result< uint32_t >::FromValue(static_cast< uint32_t >(nCopyBodyLen));
}
/// @brief Pad the input data using the PKCS7Padding scheme
/// @param buffInput Input buffer
/// @param in Input data buffer
/// @returns Padded data length
std::size_t PCtxSymmetricBlock::_PaddingInput(internal::PAutoBuff &buffInput,
                                              ReadOnlyMemRegion const &in) const noexcept
{
    uint32_t const nNeedLen{_CalPaddingLen(static_cast< uint32_t >(in.size()), false)};
    if (buffInput.GetBuffLen() < nNeedLen) {
        return 0U;
    }

    std::ignore = buffInput.SetData(in.data(), static_cast< uint32_t >(in.size()), false);
    // PKCS7Padding
    uint8_t const nPaddingData{static_cast< uint8_t >(nNeedLen - in.size())};
    std::ignore = buffInput.AddData(nPaddingData, static_cast< uint32_t >(nNeedLen - in.size()));
    return static_cast< std::size_t >(nNeedLen);
}
/// @brief Calculate the length after padding
/// @param nInputLen Data length before padding
/// @param suppressPadding Whether to suppress padding
/// @returns Length after padding
uint32_t PCtxSymmetricBlock::_CalPaddingLen(uint32_t const nInputLen, bool const suppressPadding) const noexcept
{
    if (suppressPadding) {
        return nInputLen;
    }
    std::size_t const nBlockSize{GetBlockSize()};
    if (0U == (nInputLen % nBlockSize)) {
        return static_cast< uint32_t >(nInputLen + nBlockSize);
    }
    return static_cast< uint32_t >((nInputLen + (nBlockSize - 1U)) & ~(nBlockSize - 1U));
}
//***************/
/// @brief Set (deploy) a key for the symmetric algorithm context. for ipc
/// @param stFuncName Function name used for IPC call
/// @param key Symmetric key
/// @param transform Encryption direction: encrypt or decrypt
/// @return has value if set key sucess false otherwise
ara::core::Result< void > PCtxSymmetricBlock::_SetKeyIpc(ara::core::StringView const &stFuncName,
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
        stFuncName, aswMsg, [nSlotId](keys::isoft_def::PIpcAutoPacket const *const pReqMsg) -> uint16_t {
            if (pReqMsg != nullptr) {
                keys::isoft_def::cryptoctx::PIpcReq_Setkey ipcReq;
                ipcReq.nIpcSlotID = nSlotId;
                std::ignore       = pReqMsg->AddDataToIpc(&ipcReq, sizeof(ipcReq));
            }
            return sizeof(keys::isoft_def::cryptoctx::PIpcReq_Setkey);
        })};
    if (false == bDealIpc) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUsageViolation);
    }
    keys::isoft_def::cryptoctx::PIpcAsw_Setkey *const pIpcAsw{
        aswMsg.GetIpcBody< keys::isoft_def::cryptoctx::PIpcAsw_Setkey >()};
    if (0 != pIpcAsw->GetErrorID()) {
        return ara::core::Result< void >::FromError(
            TransformErrorID(static_cast< ara::core::ErrorDomain::CodeType >(pIpcAsw->GetErrorID())));
    }
    return ara::core::Result< void >::FromValue();
}
/// @brief Return the IPC interface function name: SetKey
/// @returns  ara::core::StringView const  &
ara::core::StringView PCtxSymmetricBlock::GetIpcFuncNameSetKey() noexcept { return FUNC_NAME_Symmetric(SetKey); }
/// @brief Get the key slot ID
/// @returns Key slot ID
uint32_t PCtxSymmetricBlock::GetSlotId() const noexcept
{
    PKeySymmetric_Ipc const *const pIpcKey{dynamic_cast< PKeySymmetric_Ipc const * >(pSymmetricKey_)};
    if (pIpcKey == nullptr) {
        return 0U;
    }
    return pIpcKey->GetSlotId();
}
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara
