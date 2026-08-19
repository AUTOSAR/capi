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
/// @file       isoft_ctx_symmetric_key_wrapper_aes.cpp
/// @brief      AutoSar-Crypto Encryption/Decryption Module
/// @details
/// @date       2022-04-18
/// @author     Chang Zheng
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Default Encryption/Key Wrapping
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_01018
/// @unit_name=PCtxSymmetricKeyWrapperAes
/// @unit_description=Base class for AES-based Key Wrapper Context
/// @endcode
///
/// ================================================================

#include "ara/crypto/cryp/wrap/isoft_ctx_symmetric_key_wrapper_aes.h"

#include "ara/crypto/common/isoft_common_api.h"
#include "ara/crypto/common/isoft_data_type.h"
#include "ara/crypto/cryp/isoft_service_extension.h"

namespace {
/// @brief Check key
/// @param key Key
/// @return
bool L_CheckKey(ara::crypto::cryp::SymmetricKey const &key) noexcept
{
    ara::crypto::cryp::isoft_def::PAlgId_Symmetric_AesKey const cryptoKey;
    return cryptoKey.IsMinePrimitiveId(key.GetCryptoPrimitiveId()->GetPrimitiveId());
}
}  // namespace

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
/// @brief Parameterized constructor
/// @param cryptoProvider Encryption provider
/// @qac Suppress warning [4268]: Initialization helper can be replaced with a delegator.
PCtxSymmetricKeyWrapperAes::PCtxSymmetricKeyWrapperAes(PCryptoProvider &cryptoProvider) noexcept
    : PCtxSymmetricKeyWrapper_Base{cryptoProvider}
    , pSymmetricBlockAes_{std::make_unique< PCtxSymmetricBlock_Aes_Ecb >(cryptoProvider)}
{
}

/// @brief Calculate the size (in bytes) of the wrapped key from the raw key length (bits). This method is useful for implementations differing from RFC3394 / RFC5649.
/// @brief Calculate size of the wrapped key in bytes from original key length in bits.
///       This method can be useful for some implementations different from RFC3394 / RFC5649.
/// @param keyLength  original key length in bits
/// @returns size of the wrapped key in bytes
/// @trace_id_sws={SWS_CRYPT_24013}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02201}
/// @threadsafety={Thread-safe}
std::size_t PCtxSymmetricKeyWrapperAes::CalculateWrappedKeySize(std::size_t keyLength) const noexcept
{
    std::size_t blocks{keyLength / kInt_64U};
    std::size_t const remain{keyLength % kInt_64U};

    if (remain > 0U) {
        blocks += 1U;
    }
    return (blocks + 1U) * kInt_64U;
}

/// @brief Get the maximum length of the target key supported by the implementation. This method is useful for implementations differing from RFC3394 / RFC5649.
/// @brief Get maximum length of the target key supported by the implementation.
///       This method can be useful for some implementations different from RFC3394 / RFC5649.
/// @returns maximum length of the target key in bits
/// @trace_id_sws={SWS_CRYPT_24012}
/// @tracestatus={draft}
/// @uptrace={RS_CRYPTO_02201}
/// @threadsafety={Thread-safe}
std::size_t PCtxSymmetricKeyWrapperAes::GetMaxTarGetKeyLength() const noexcept { return GetMaxKeyBitLength(); }
/// @brief Set (deploy) a key for the symmetric algorithm context.
/// @param key Symmetric key
/// @param transform Encryption direction: Encrypt or Decrypt
/// @returns  has vlaue if setkey sucess false otherwise
ara::core::Result< void > PCtxSymmetricKeyWrapperAes::SetKey(SymmetricKey const &key,
                                                             CryptoTransform transform) noexcept
{
    // SetKey shall return a kIncompatibleObject error, if the provided key object is incompatible with this symmetric key context
    // Legacy issue, needs testing
    if (false == L_CheckKey(key)) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kIncompatibleObject);
    }

    /// @error: SetKey shall return a kInvalidArgument error, if the provided transformation
    /// direction is not allowed in Symmetric Key wrapper algorithm context.
    if ((transform != CryptoTransform::kWrap) && (transform != CryptoTransform::kUnwrap)) {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kInvalidArgument);
    }

    if (pSymmetricBlockAes_ == nullptr) {
        return ara::core::Result< void >::FromValue();
    }
    uint32_t const nKeyAllowedUsage{key.GetAllowedUsage()};

    if (transform == CryptoTransform::kWrap) {
        transform = CryptoTransform::kEncrypt;
        if ((kAllowKeyExporting & nKeyAllowedUsage) != kAllowKeyExporting) {
            return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUsageViolation);
        }
    } else if (transform == CryptoTransform::kUnwrap) {
        transform = CryptoTransform::kDecrypt;
        if ((kAllowKeyImporting & nKeyAllowedUsage) != kAllowKeyImporting) {
            return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUsageViolation);
        }
    } else {
        return ara::core::Result< void >::FromError(SecurityErrorDomain::Errc::kUsageViolation);
    }

    return pSymmetricBlockAes_->SetKey(key, transform);
}

/// @brief Clear encryption context.
/// @returns  has vlaue if Reset sucess false otherwise
ara::core::Result< void > PCtxSymmetricKeyWrapperAes::Reset() noexcept
{
    if (pSymmetricBlockAes_ == nullptr) {
        return ara::core::Result< void >::FromValue();
    }
    return pSymmetricBlockAes_->Reset();
}

/// @brief Get the allowed usage for this context (based on the properties of the key object loaded into this context).
/// If the context has not yet been initialized with a key object, it must return 0 (all flags reset).
/// @returns  a combination of bit-flags that specifies allowed usages of the context
AllowedUsageFlags PCtxSymmetricKeyWrapperAes::GetAllowedUsage() const noexcept
{
    if (false == IsKeyAvailable()) {
        return 0U;
    }
    return pSymmetricBlockAes_->GetAllowedUsage();
}

/// @brief Get the COUID of the key deployed to the context attached to this extended service. Returns an empty COUID (Nil) if no key is set for the context.
/// @returns  the COUID of the CryptoObject
CryptoObjectUid PCtxSymmetricKeyWrapperAes::GetActualKeyCOUID() const noexcept
{
    if (false == IsKeyAvailable()) {
        return CryptoObjectUid{};
    }
    return pSymmetricBlockAes_->GetActualKeyCOUID();
}
/// @brief Get the maximum supported key length in bits.
/// @returns  maximal supported length of the key in bits
std::size_t PCtxSymmetricKeyWrapperAes::GetMaxKeyBitLength() const noexcept
{
    if (false == IsKeyAvailable()) {
        return 0U;
    }
    return pSymmetricBlockAes_->GetMaxKeyBitLength();
}
/// @brief Get the minimum supported key length in bits.
/// @returns minimal supported length of the key in bits
std::size_t PCtxSymmetricKeyWrapperAes::GetMinKeyBitLength() const noexcept
{
    if (false == IsKeyAvailable()) {
        return 0U;
    }
    return pSymmetricBlockAes_->GetMinKeyBitLength();
}
/// @brief Verify support for a specific key length based on the context.
/// @param keyBitLength Key length: in bits
/// @returns  @c true if provided value of the key length is supported by the context
bool PCtxSymmetricKeyWrapperAes::IsKeyBitLengthSupported(std::size_t keyBitLength) const noexcept
{
    if (false == IsKeyAvailable()) {
        return false;
    }
    return pSymmetricBlockAes_->IsKeyBitLengthSupported(keyBitLength);
}
/// @brief Check if a key is set for this context.
/// @returns  true if a key has been set to this context false otherwise
bool PCtxSymmetricKeyWrapperAes::IsKeyAvailable() const noexcept
{
    if (pSymmetricBlockAes_ == nullptr) {
        return false;
    }
    return pSymmetricBlockAes_->IsKeyAvailable();
}

//***************/
/// @brief Get the actual bit length of the key loaded into the context. Returns 0 if no key is set for the context.
/// @returns  actual length of a key (now set to the algorithm context) in bits
std::size_t PCtxSymmetricKeyWrapperAes::GetActualKeyBitLength() const noexcept
{
    if (false == IsKeyAvailable()) {
        return 0U;
    }
    return pSymmetricBlockAes_->GetActualKeyBitLength();
}
/// @brief Execute the "key unwrap" operation on the provided BLOB and generate a SecretSeed object.
/// @param stFuncName Function name used for IPC call
/// @param pInputData Starting address of input data in algorithm operation
/// @param nDataLen Data length
/// @param nKeySlotId Key slot ID
/// @return Wrapping or unwrapping result
ara::core::Result< ara::core::Vector< ara::core::Byte > > PCtxSymmetricKeyWrapperAes::_DoWrapAndUnWrapIpc(
    ara::core::StringView const &stFuncName,
    uint8_t const *const pInputData,
    uint32_t const nDataLen,
    uint32_t const nKeySlotId) const noexcept
{
    keys::isoft_def::PIpcAutoPacket aswMsg;  // Receive response packet
    keys::isoft_def::PIpcClient *const ipcClient{keys::isoft_def::PIpcClient::GetInstance_Keys()};
    if (ipcClient == nullptr) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kLogicFault);
    }
    bool const bDealIpc{ipcClient->DealIpcRequest(
        stFuncName, aswMsg,
        [this, pInputData, nDataLen, nKeySlotId](keys::isoft_def::PIpcAutoPacket const *const pReqMsg) -> uint16_t {
            if (pReqMsg != nullptr) {
                keys::isoft_def::cryptoctx::PIpcReq_Wrap ipcReq;
                ipcReq.SetIpcSlotID(pSymmetricBlockAes_->GetSlotId());
                if (pSymmetricBlockAes_->GetTransformation().HasValue()) {
                    ara::crypto::CryptoTransform const nTransform{pSymmetricBlockAes_->GetTransformation().Value()};
                    uint32_t intTransform{0U};
                    if (nTransform == ara::crypto::CryptoTransform::kEncrypt) {
                        intTransform = static_cast< uint32_t >(ara::crypto::CryptoTransform::kWrap);
                    } else if (nTransform == ara::crypto::CryptoTransform::kDecrypt) {
                        intTransform = static_cast< uint32_t >(ara::crypto::CryptoTransform::kUnwrap);
                    } else {
                    }
                    ipcReq.SetTransform(intTransform);
                }
                ipcReq.SetKeySlotId(nKeySlotId);
                ipcReq.SetDataLen(nDataLen);
                std::ignore = pReqMsg->AddDataToIpc(&ipcReq, sizeof(ipcReq));
                if (ipcReq.GetDataLen() > 0U) {
                    std::ignore = pReqMsg->AddDataToIpc(pInputData, static_cast< uint16_t >(nDataLen));
                }
            }
            return static_cast< uint16_t >(sizeof(keys::isoft_def::cryptoctx::PIpcReq_Wrap) + nDataLen);
        })};
    if (false == bDealIpc) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kLogicFault);
    }
    keys::isoft_def::cryptoctx::PIpcAsw_common *const pIpcAsw{
        static_cast< keys::isoft_def::cryptoctx::PIpcAsw_common * >(static_cast< void * >(aswMsg.GetIpcBody()))};
    if (0 != pIpcAsw->GetErrorID()) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kLogicFault);
    }
    uint8_t const *const pdata{pIpcAsw->GetData()};
    uint32_t const len{pIpcAsw->GetDataLen()};
    ara::core::Vector< ara::core::Byte > resultByte;
    try {
        resultByte.reserve(static_cast< std::size_t >(len));
    } catch (std::bad_alloc &) {
        return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(
            SecurityErrorDomain::Errc::kInsufficientCapacity);
    }
    for (uint32_t i{0U}; i < len; ++i) {
        ara::core::Byte const byData{*(pdata + i)};
        resultByte.push_back(byData);
    }
    return ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromValue(resultByte);
}

}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara
