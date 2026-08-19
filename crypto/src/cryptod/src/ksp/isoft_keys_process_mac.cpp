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
/// @file       isoft_keys_process_mac.cpp
/// @brief      AutoSar-Crypto Key Storage Module
/// @details
/// @date       2022-08-17
/// @author     Chang Zheng
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Key Manager/Message Authentication Code IPC Service
/// @interface_level=module
/// @trace_id_sr=SR_CRYPTO_02005
/// @unit_name=PKeysProcess_Mac
/// @unit_description=Message authentication code IPC service
/// @endcode
///
/// ================================================================

#include "ara/crypto/ksp/isoft_keys_process_mac.h"

#include "ara/crypto/cryp/isoft_crypto_provider.h"
#include "ara/crypto/ksp/isoft_keys_manager.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {

//********************************/
/// @brief Constructor
/// @param lpcProcessManager LCP command processing manager
PKeysProcess_Mac::PKeysProcess_Mac(keys::isoft_def::PKeys_Manager &lpcProcessManager) noexcept
    : PKeysProcess_T_Base< PKeysProcess_Mac >{this, lpcProcessManager}, mapContextManger_{}  // NOLINT
{
    _InsertMapCmd(FUNC_NAME_Mac(SetKey), &PKeysProcess_Mac::SetKey);
    _InsertMapCmd(FUNC_NAME_Mac(DoInit), &PKeysProcess_Mac::DoInit);
    _InsertMapCmd(FUNC_NAME_Mac(DoUpdate), &PKeysProcess_Mac::DoUpdate);
    _InsertMapCmd(FUNC_NAME_Mac(DoFinish), &PKeysProcess_Mac::DoFinish);
    _InsertMapCmd(FUNC_NAME_Mac(DoReset), &PKeysProcess_Mac::DoReset);
}
/// @brief mac processing logic
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @param doOperater MAC operation type
/// @return
keys::isoft_def::PKeysProcess_Base::PResultLen PKeysProcess_Mac::_doOperaterMac(
    keys::isoft_def::PIpcPac_Head const *const pReqHead,
    keys::isoft_def::PIpcAutoPacket &aswMsg,
    DoOperateMac const doOperater) const noexcept
{
    if (nullptr == pReqHead) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);  // IPC parameter error;
    }
    keys::isoft_def::cryptoctx::PIpcReq_DoMac const *const pReqMsg{
        pReqHead->GetBody< keys::isoft_def::cryptoctx::PIpcReq_DoMac >()};
    uint64_t nPid{pReqHead->nProcessID};
    ara::crypto::CryptoAlgId const algId{static_cast< ara::crypto::CryptoAlgId >(pReqMsg->nAlgID)};
    ara::crypto::CryptoTransform const nTransform{static_cast< ara::crypto::CryptoTransform >(pReqMsg->nTransform)};
    uint32_t const nDataLnen{pReqMsg->nDataLen};
    uint8_t const *const pData{pReqMsg->GetData()};

    ara::crypto::cryp::isoft_def::MacContextManager *pMacContextManager{_findMacContextByID(nPid)};
    if (pMacContextManager == nullptr) {
        if ((doOperater != DoOperateMac::kDoSetKey) && (doOperater != DoOperateMac::kDoInit)) {
            return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);  // IPC parameter error;
        }
        cryp::isoft_def::PCryptoProvider::Uptr const pPCryptoProvider{
            std::make_unique< cryp::isoft_def::PCryptoProvider >()};
        ara::core::Result< ara::crypto::cryp::MessageAuthnCodeCtx::Uptr > resMacCtx{
            pPCryptoProvider->CreateMessageAuthCodeCtx(algId)};
        if (!resMacCtx.HasValue()) {
            return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);  // IPC parameter error;
        }
        ara::crypto::cryp::MessageAuthnCodeCtx::Uptr pMacCtxUptr{std::move(resMacCtx).Value()};

        ara::core::Result< ara::core::Vector< uint8_t > > const result{_LoadKeyData(pReqMsg->nIpcSlotID)};
        if (!result.HasValue()) {
            return ProcessCmd_Error(pReqHead, aswMsg, result.Error().Value());
        }
        ara::core::Vector< uint8_t > const &vecData{result.Value()};
        std::size_t const keyLen{vecData.size()};

        ara::core::Result< ara::crypto::CryptoAlgId > const resCryptoAlgId{_GetPrimitiveId(pReqMsg->nIpcSlotID)};
        if (!resCryptoAlgId.HasValue()) {
            return ProcessCmd_Error(pReqHead, aswMsg, resCryptoAlgId.Error().Value());
        }
        ara::crypto::CryptoAlgId const algIdKey{resCryptoAlgId.Value()};

        ara::core::Result< AllowedUsageFlags > const resAllowedUsage{_GetAllowedUsage(pReqMsg->nIpcSlotID)};
        if (!resAllowedUsage.HasValue()) {
            return ProcessCmd_Error(pReqHead, aswMsg, resAllowedUsage.Error().Value());
        }
        AllowedUsageFlags const allowedUsageFlags{resAllowedUsage.Value()};
        ReadOnlyMemRegion const memKeyData{vecData.data(), keyLen};
        ara::core::Result< SymmetricKey::Uptrc > resSymmetricKey{
            pPCryptoProvider->GenerateSymmetricKeyEx(algIdKey, memKeyData, allowedUsageFlags)};
        if (!resSymmetricKey.HasValue()) {
            return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);  // IPC parameter error;
        }
        SymmetricKey::Uptrc pSymmetricKey{std::move(resSymmetricKey).Value()};
        ara::core::Result< void > const resVoid{pMacCtxUptr->SetKey(*pSymmetricKey, nTransform)};
        if (!resVoid.HasValue()) {
            return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);  // IPC parameter error;
        }
        std::unique_ptr< MacContextManager > stMacContextPtr{std::make_unique< MacContextManager >()};
        std::time_t const t0{std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())};
        stMacContextPtr->timeSecond = t0;
        stMacContextPtr->macContext = std::make_pair(std::move(pMacCtxUptr), std::move(pSymmetricKey));
        std::ignore                 = mapContextManger_.insert(std::make_pair(nPid, std::move(stMacContextPtr)));
        pMacContextManager          = _findMacContextByID(nPid);
    }

    MessageAuthnCodeCtx *const pMessageAuthnCodeCtx{pMacContextManager->macContext.first.get()};
    PCtxMac_Base *const pMacBase{dynamic_cast< PCtxMac_Base * >(pMessageAuthnCodeCtx)};
    if (pMacBase == nullptr) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);  // IPC parameter error;
    }

    bool ret{false};
    uint8_t *pDegestData{nullptr};
    uint32_t pDegestDataLen{0U};
    ara::core::Vector< uint8_t > vecDegestData;
    ara::core::String const strIv{static_cast< char8_t const * >(static_cast< void const * >(pData)),
                                  static_cast< size_t >(nDataLnen)};
    switch (doOperater) {
        case DoOperateMac::kDoInit: {
            ret = pMacBase->DoInitLocal(static_cast< uint8_t const * >(static_cast< void const * >(strIv.data())));
        } break;
        case DoOperateMac::kDoReset: {
            ret = true;
        } break;
        case DoOperateMac::kDoUpdate: {
            ret = pMacBase->DoUpdateLocal(pData, nDataLnen);
        } break;
        case DoOperateMac::kDoFinish: {
            ret = pMacBase->DoFinishLocal();
            if (ret) {
                /// finish: copy digest data to client, context can be removed from map
                pDegestDataLen = pMacBase->GetMacLength();
                vecDegestData.resize(static_cast< size_t >(pDegestDataLen));
                pDegestData = vecDegestData.data();
                std::ignore = std::memcpy(pDegestData, static_cast< void const * >(pMacBase->GetMacResult()),
                                          static_cast< size_t >(pDegestDataLen));
                /// Delete specified context
                _removeMacContextFromPid(nPid);
                /// Delete timed-out contexts
                _clearMacContextOfTimeOut();
            }
        }
        /// Start cleanup
        break;
        default: {
            break;
        }
    }
    if ((!ret) && (doOperater != DoOperateMac::kDoSetKey)) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);  // IPC parameter error;
    }

    // Build response packet
    uint16_t const nAswLen{static_cast< uint16_t >(static_cast< size_t >(pReqHead->GetHeadLen())
                                                   + sizeof(keys::isoft_def::cryptoctx::PIpcAsw_DoMac)
                                                   + static_cast< size_t >(pDegestDataLen))};
    aswMsg.CreatePacket(nAswLen);
    std::ignore = aswMsg.InitIpcHead(pReqHead, nAswLen);
    aswMsg.SetFirstSize(static_cast< int32_t >(pReqHead->GetHeadLen()));
    keys::isoft_def::cryptoctx::PIpcAsw_DoMac pacAsw;
    pacAsw.SetErrorID(0);
    pacAsw.SetDataLen(pDegestDataLen);
    std::ignore = aswMsg.AddDataToIpc(&pacAsw, sizeof(pacAsw));
    if (pDegestData != nullptr) {
        std::ignore = aswMsg.AddDataToIpc(pDegestData, static_cast< uint16_t >(pDegestDataLen));
    }

    return PResultLen::FromValue(pReqHead->nPacSize);
}
/// @brief Sets key
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @returns  has value if setkey sucess false otherwise
keys::isoft_def::PKeysProcess_Base::PResultLen PKeysProcess_Mac::SetKey(
    keys::isoft_def::PIpcPac_Head const *const pReqHead, keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept
{
    return _doOperaterMac(pReqHead, aswMsg, DoOperateMac::kDoSetKey);
}
/// @brief Initialization operation
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @returns  has value if DoInit sucess false otherwise
keys::isoft_def::PKeysProcess_Base::PResultLen PKeysProcess_Mac::DoInit(
    keys::isoft_def::PIpcPac_Head const *const pReqHead, keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept
{
    return _doOperaterMac(pReqHead, aswMsg, DoOperateMac::kDoInit);
}
/// @brief Update operation
/// @name  DoUpdate
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @returns  has value if setkey sucess false otherwise
keys::isoft_def::PKeysProcess_Base::PResultLen PKeysProcess_Mac::DoUpdate(
    keys::isoft_def::PIpcPac_Head const *const pReqHead, keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept
{
    return _doOperaterMac(pReqHead, aswMsg, DoOperateMac::kDoUpdate);
}
/// @brief Finish operation
/// @name  DoFinish
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @returns  has value if setkey sucess false otherwise
keys::isoft_def::PKeysProcess_Base::PResultLen PKeysProcess_Mac::DoFinish(
    keys::isoft_def::PIpcPac_Head const *const pReqHead, keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept
{
    return _doOperaterMac(pReqHead, aswMsg, DoOperateMac::kDoFinish);
}
/// @brief Reset operation
/// @name  DoReset
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @returns  has value if setkey sucess false otherwise
keys::isoft_def::PKeysProcess_Base::PResultLen PKeysProcess_Mac::DoReset(
    keys::isoft_def::PIpcPac_Head const *const pReqHead, keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept
{
    return _doOperaterMac(pReqHead, aswMsg, DoOperateMac::kDoReset);
}

}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara