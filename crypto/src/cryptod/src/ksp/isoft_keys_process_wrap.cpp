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
/// @file       isoft_keys_process_wrap.cpp
/// @brief      AutoSar-Crypto Key Storage Module
/// @details
/// @date       2022-08-17
/// @author     Chang Zheng
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Key Manager/Key Wrap IPC Service
/// @interface_level=module
/// @trace_id_sr=SR_CRYPTO_01018
/// @unit_name=PKeysProcess_Wrap
/// @unit_description=Key wrap IPC service
/// @endcode
///
/// ================================================================

#include "ara/crypto/ksp/isoft_keys_process_wrap.h"

#include "ara/crypto/cryp/isoft_crypto_provider.h"
#include "ara/crypto/cryp/wrap/isoft_ctx_symmetric_key_wrapper_aes_pad.h"
#include "ara/crypto/cryp/wrap/isoft_ctx_symmetric_key_wrapper_aes_unpad.h"
#include "ara/crypto/cryp/wrap/isoft_ctx_symmetric_key_wrapper_des.h"
#include "ara/crypto/ksp/isoft_keys_manager.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
//********************************/
/// @brief Key wrap logic processing
/// @param lpcProcessManager LCP command processing manager
PKeysProcess_Wrap::PKeysProcess_Wrap(keys::isoft_def::PKeys_Manager &lpcProcessManager) noexcept
    : PKeysProcess_T_Base< PKeysProcess_Wrap >{this, lpcProcessManager}
{
    _InsertMapCmd(FUNC_NAME_Wrap(DoWrapKeyMaterial_Pad), &PKeysProcess_Wrap::DoWrapKeyMaterial_Pad);
    _InsertMapCmd(FUNC_NAME_Wrap(DoWrapKeyMaterial_Unpad), &PKeysProcess_Wrap::DoWrapKeyMaterial_Unpad);
    _InsertMapCmd(FUNC_NAME_Wrap(DoUnwrapKey_Pad), &PKeysProcess_Wrap::DoUnwrapKey_Pad);
    _InsertMapCmd(FUNC_NAME_Wrap(DoUnwrapKey_Unpad), &PKeysProcess_Wrap::DoUnwrapKey_Unpad);
    _InsertMapCmd(FUNC_NAME_Wrap(DoWrapDesKeyMaterial), &PKeysProcess_Wrap::DoWrapDesKeyMaterial);
    _InsertMapCmd(FUNC_NAME_Wrap(DoUnwrapDesKey), &PKeysProcess_Wrap::DoUnwrapDesKey);
}

//********************************/
/// @brief Unwrap: Performs "key unwrap" operation on the provided BLOB and generates a key object. pad is padding
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @returns has value if  do aes pad unwrap sucess
keys::isoft_def::PKeysProcess_Base::PResultLen PKeysProcess_Wrap::DoUnwrapKey_Pad(
    keys::isoft_def::PIpcPac_Head const *pReqHead, keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept
{
    return _DoWrapOperator< PCtxSymmetricKeyWrapperAesPad >(pReqHead, aswMsg, DoOperateWrap::kDoUnwrap_Pad);
}
//********************************/
/// @brief Performs "key wrap" operation on the provided key material.
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @returns  has value if  do aes pad wrap sucess
keys::isoft_def::PKeysProcess_Base::PResultLen PKeysProcess_Wrap::DoWrapKeyMaterial_Pad(
    keys::isoft_def::PIpcPac_Head const *pReqHead, keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept
{
    return _DoWrapOperator< PCtxSymmetricKeyWrapperAesPad >(pReqHead, aswMsg, DoOperateWrap::kDoWrap_Pad);
}
//********************************/
/// @brief Unwrap: Performs "key unwrap" operation on the provided BLOB and generates a key object. unPad is unpadding
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @returns  has value if  do aes unpad unwrap sucess
keys::isoft_def::PKeysProcess_Base::PResultLen PKeysProcess_Wrap::DoUnwrapKey_Unpad(
    keys::isoft_def::PIpcPac_Head const *pReqHead, keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept
{
    return _DoWrapOperator< PCtxSymmetricKeyWrapperAesUnPad >(pReqHead, aswMsg, DoOperateWrap::kDoUnwrap_UnPad);
}
//********************************/
/// @brief Performs "key wrap" operation on the provided key material.
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @returns  has value if  do aes unpad wrap sucess
keys::isoft_def::PKeysProcess_Base::PResultLen PKeysProcess_Wrap::DoWrapKeyMaterial_Unpad(
    keys::isoft_def::PIpcPac_Head const *pReqHead, keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept
{
    return _DoWrapOperator< PCtxSymmetricKeyWrapperAesUnPad >(pReqHead, aswMsg, DoOperateWrap::kDoWrap_UnPad);
}
//********************************/
/// @brief Performs "key unwrap" operation on the provided BLOB and generates a key object
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @return has value if do des unwrap sucess
keys::isoft_def::PKeysProcess_Base::PResultLen PKeysProcess_Wrap::DoUnwrapDesKey(
    keys::isoft_def::PIpcPac_Head const *pReqHead, keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept
{
    return _DoWrapOperator< PCtxSymmetricKeyWrapperDes >(pReqHead, aswMsg, DoOperateWrap::kDoUnwrapDes);
}
/// @brief Performs "key wrap" operation on the provided key material.
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @return has value if  do des wrap sucess
keys::isoft_def::PKeysProcess_Base::PResultLen PKeysProcess_Wrap::DoWrapDesKeyMaterial(
    keys::isoft_def::PIpcPac_Head const *pReqHead, keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept
{
    return _DoWrapOperator< PCtxSymmetricKeyWrapperDes >(pReqHead, aswMsg, DoOperateWrap::kDoWrapDes);
}
/// @brief Common logic processing for wrap operations
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @param doOperator Wrapping operation type
/// @return has value if do Operator sucess
/// @code{.isoft}
/// @tparam T_ClassType
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00000
/// @trace_id_dd=DD_CRYPTO_05933
/// @needwork = dda
/// @endcode
template < typename T_ClassType >
keys::isoft_def::PKeysProcess_Base::PResultLen PKeysProcess_Wrap::_DoWrapOperator(
    keys::isoft_def::PIpcPac_Head const *const pReqHead,
    keys::isoft_def::PIpcAutoPacket &aswMsg,
    DoOperateWrap const doOperator) const noexcept
{
    if (nullptr == pReqHead) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);  // IPC parameter error;
    }

    keys::isoft_def::cryptoctx::PIpcReq_Wrap const *const pReqMsg{
        pReqHead->GetBody< keys::isoft_def::cryptoctx::PIpcReq_Wrap >()};
    uint32_t const nIpcSlotID{pReqMsg->GetIpcSlotID()};
    uint32_t nDataLen{0U};
    uint8_t const *pData{nullptr};
    uint32_t const nKeySlotId{pReqMsg->GetKeySlotId()};
    uint32_t const nTransform{pReqMsg->GetTransform()};
    ara::core::Result< ara::core::Vector< uint8_t > > result{
        ara::core::Result< ara::core::Vector< uint8_t > >::FromError(SecurityErrorDomain::Errc::kLogicFault)};

    if (nKeySlotId != 0U) {  // Data to be wrapped is an ipc slot
        result = {std::move(_LoadKeyData(nKeySlotId))};
        if (!result.HasValue()) {
            return ProcessCmd_Error(pReqHead, aswMsg, result.Error().Value());
        }
        pData    = result.Value().data();
        nDataLen = static_cast< uint32_t >(result.Value().size());
    } else {  // Data to be wrapped is not an ipc slot
        pData    = pReqMsg->GetData();
        nDataLen = pReqMsg->GetDataLen();
    }
    if ((pData == nullptr) || (nDataLen == 0U)) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcSlotNotExist);  // IPC parameter error;
    }

    cryp::isoft_def::PCryptoProvider::Uptr const pPCryptoProvider{
        std::make_unique< cryp::isoft_def::PCryptoProvider >()};
    SymmetricKey::Uptrc pSymmetricKey{nullptr};
    ara::core::Result< SymmetricKey::Uptrc > resSymmetricKey{
        ara::core::Result< SymmetricKey::Uptrc >::FromError(SecurityErrorDomain::Errc::kLogicFault)};
    uint32_t nKeyDataLen{0U};
    uint8_t const *pKeyData{nullptr};
    ara::core::Result< ara::core::Vector< uint8_t > > resultKeyData{
        ara::core::Result< ara::core::Vector< uint8_t > >::FromError(SecurityErrorDomain::Errc::kLogicFault)};

    if (nIpcSlotID != 0U) {  /// Key used for wrapping is ipc
        resultKeyData = {std::move(_LoadKeyData(nIpcSlotID))};
        if (!resultKeyData.HasValue()) {
            return ProcessCmd_Error(pReqHead, aswMsg, resultKeyData.Error().Value());
        }
        pKeyData    = resultKeyData.Value().data();
        nKeyDataLen = static_cast< uint32_t >(resultKeyData.Value().size());
        if ((pKeyData == nullptr) || (nKeyDataLen == 0U)) {
            return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcSlotNotExist);  // IPC parameter error;
        }

        ara::core::Result< ara::crypto::CryptoAlgId > const resCryptoAlgId{_GetPrimitiveId(nIpcSlotID)};
        if (!resCryptoAlgId.HasValue()) {
            return ProcessCmd_Error(pReqHead, aswMsg, resCryptoAlgId.Error().Value());
        }
        ara::crypto::CryptoAlgId const algId{resCryptoAlgId.Value()};

        ara::core::Result< AllowedUsageFlags > const resAllowedUsage{_GetAllowedUsage(nIpcSlotID)};
        if (!resAllowedUsage.HasValue()) {
            return ProcessCmd_Error(pReqHead, aswMsg, resAllowedUsage.Error().Value());
        }
        AllowedUsageFlags const allowedUsageFlags{resAllowedUsage.Value()};
        ReadOnlyMemRegion const memKeyData{pKeyData, static_cast< size_t >(nKeyDataLen)};
        /// Key in slot: algorithm ID and usage restrictions are readily available, can create directly; otherwise, can only create corresponding key based on key length
        resSymmetricKey = pPCryptoProvider->GenerateSymmetricKeyEx(algId, memKeyData, allowedUsageFlags);
    } else {
        /// Key used for wrapping is non-ipc
        pKeyData    = pReqMsg->GetData();
        nKeyDataLen = pReqMsg->GetDataLen();
        if ((pKeyData == nullptr) || (nKeyDataLen == 0U)) {
            return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcSlotNotExist);  // IPC parameter error;
        }
        ReadOnlyMemRegion const memKeyData{pKeyData, static_cast< size_t >(nKeyDataLen)};
        AllowedUsageFlags const allowedUsageFlags{kAllowDataEncryption | kAllowDataDecryption | kAllowKeyExporting
                                                  | kAllowKeyImporting};
        ara::crypto::CryptoAlgId targetAlgId{static_cast< ara::crypto::CryptoAlgId >(EPhCtxTypeID::kUnDefine)};
        if ((doOperator == DoOperateWrap::kDoWrapDes) || (doOperator == DoOperateWrap::kDoUnwrapDes)) {
            /// Create 3des key
            switch (nKeyDataLen) {
                case kInt_8U: {
                    targetAlgId = static_cast< ara::crypto::CryptoAlgId >(EPhCtxTypeID::kSymmetricDesKey);
                    break;
                }
                case kInt_24U: {
                    targetAlgId = static_cast< ara::crypto::CryptoAlgId >(EPhCtxTypeID::kSymmetric3DesKey);
                    break;
                }
                default: {
                    break;
                }
            }
        } else {
            switch (nKeyDataLen) {
                case kInt_16U: {
                    targetAlgId = static_cast< ara::crypto::CryptoAlgId >(EPhCtxTypeID::kSymmetricAesKey128);
                    break;
                }
                case kInt_24U: {
                    targetAlgId = static_cast< ara::crypto::CryptoAlgId >(EPhCtxTypeID::kSymmetricAesKey192);
                    break;
                }
                case kInt_32U: {
                    targetAlgId = static_cast< ara::crypto::CryptoAlgId >(EPhCtxTypeID::kSymmetricAesKey256);
                    break;
                }
                default: {
                    break;
                }
            }
        }
        if (targetAlgId == static_cast< ara::crypto::CryptoAlgId >(EPhCtxTypeID::kUnDefine)) {
            return PResultLen::FromError(SecurityErrorDomain::Errc::kLogicFault);  // IPC parameter error;
        }
        resSymmetricKey = pPCryptoProvider->GenerateSymmetricKeyEx(targetAlgId, memKeyData, allowedUsageFlags);
    }
    if (!resSymmetricKey.HasValue()) {
        return ProcessCmd_Error(pReqHead, aswMsg, resSymmetricKey.Error().Value());
    }
    /// Generate key
    pSymmetricKey = std::move(resSymmetricKey).Value();
    std::unique_ptr< T_ClassType > const pReturn{std::move(std::make_unique< T_ClassType >(*pPCryptoProvider))};
    /// Set key
    ara::core::Result< void > const resVoid{
        std::move(pReturn->SetKey(*pSymmetricKey, static_cast< ara::crypto::CryptoTransform >(nTransform)))};
    if (!resVoid.HasValue()) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kLogicFault);  // IPC parameter error;
    }
    /// Execute specific logic
    ara::core::Result< ara::core::Vector< ara::core::Byte > > resRetByte{
        ara::core::Result< ara::core::Vector< ara::core::Byte > >::FromError(SecurityErrorDomain::Errc::kLogicFault)};
    if ((doOperator == DoOperateWrap::kDoWrap_Pad) || (doOperator == DoOperateWrap::kDoWrap_UnPad)
        || (doOperator == DoOperateWrap::kDoWrapDes)) {
        resRetByte = pReturn->DoWrap(pData, nDataLen);
    }
    /// Execute specific logic
    if ((doOperator == DoOperateWrap::kDoUnwrap_Pad) || (doOperator == DoOperateWrap::kDoUnwrap_UnPad)
        || (doOperator == DoOperateWrap::kDoUnwrapDes)) {
        resRetByte = pReturn->DoUnWrap(pData, nDataLen);
    }
    if (!resRetByte.HasValue()) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kLogicFault);  // IPC parameter error;
    }
    ara::core::Vector< ara::core::Byte > vecData{std::move(resRetByte).Value()};
    std::size_t const cipherLen{vecData.size()};
    uint8_t *const cipherData{T_TransBytes(vecData.data())};

    // Build response packet
    uint16_t const nAswLen{static_cast< uint16_t >(
        pReqHead->GetHeadLen() + static_cast< uint16_t >(sizeof(keys::isoft_def::cryptoctx::PIpcAsw_common))
        + static_cast< uint16_t >(cipherLen))};
    aswMsg.CreatePacket(nAswLen);
    std::ignore = aswMsg.InitIpcHead(pReqHead, nAswLen);
    keys::isoft_def::cryptoctx::PIpcAsw_common *const pPacAsw{
        aswMsg.GetIpcBody< keys::isoft_def::cryptoctx::PIpcAsw_common >()};
    pPacAsw->SetErrorID(0);
    pPacAsw->SetDataLen(static_cast< uint32_t >(cipherLen));
    std::ignore = memcpy(static_cast< uint8_t * >(static_cast< void * >(pPacAsw + 1)),
                         static_cast< void const * >(cipherData), static_cast< size_t >(cipherLen));
    return PResultLen::FromValue(pReqHead->nPacSize);
}
//********************************/
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara