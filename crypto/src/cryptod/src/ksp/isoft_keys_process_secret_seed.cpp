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
/// @file       isoft_keys_process_secret_seed.cpp
/// @brief      AutoSar-Crypto Secret Seed Module
/// @details
/// @date       2023-09-06
/// @author     Chang Zheng
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Key Manager/Secret Seed IPC Service
/// @interface_level=module
/// @trace_id_sr=SR_CRYPTO_06005
/// @unit_name=PKeysProcess_SecretSeed
/// @unit_description=Secret seed IPC service
/// @endcode
///
/// ================================================================

#include "ara/crypto/ksp/isoft_keys_process_secret_seed.h"

#include "ara/crypto/cryp/cryobj/isoft_secret_seed.h"
#include "ara/crypto/ksp/isoft_keys_manager.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
//********************************/
/// @brief Secret seed logic
/// @name  PKeysProcess_SecretSeed
/// @param lpcProcessManager LCP command processing manager
PKeysProcess_SecretSeed::PKeysProcess_SecretSeed(keys::isoft_def::PKeys_Manager &lpcProcessManager) noexcept
    : PKeysProcess_T_Base< PKeysProcess_SecretSeed >{this, lpcProcessManager}
{
    _InsertMapCmd(FUNC_NAME_KeyProvider(Clone), &PKeysProcess_SecretSeed::Clone);
    _InsertMapCmd(FUNC_NAME_KeyProvider(JumpFrom), &PKeysProcess_SecretSeed::JumpFrom);
    _InsertMapCmd(FUNC_NAME_KeyProvider(Jump), &PKeysProcess_SecretSeed::Jump);
    _InsertMapCmd(FUNC_NAME_KeyProvider(Next), &PKeysProcess_SecretSeed::Next);
    _InsertMapCmd(FUNC_NAME_KeyProvider(OperatorXor), &PKeysProcess_SecretSeed::OperatorXor);
    _InsertMapCmd(FUNC_NAME_KeyProvider(GetPayloadSize), &PKeysProcess_SecretSeed::GetPayloadSize);
}
//********************************/
/// @brief Clones this Secret Seed object to a new session object
/// @name   Clone
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @returns  reference to this updated object
keys::isoft_def::PKeysProcess_Base::PResultLen PKeysProcess_SecretSeed::Clone(
    keys::isoft_def::PIpcPac_Head const *pReqHead, keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept
{
    return SecretSeedOperator(pReqHead, aswMsg, DoOperateSecretSeed::kDoClone);
}
/// @brief Sets the value of this seed object to "jump" from its initial state to the specified number of steps according to the "count" expression defined by the associated cryptographic algorithm.
/// @name   JumpFrom
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @returns reference to this updated object
keys::isoft_def::PKeysProcess_Base::PResultLen PKeysProcess_SecretSeed::JumpFrom(
    keys::isoft_def::PIpcPac_Head const *pReqHead, keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept
{
    return SecretSeedOperator(pReqHead, aswMsg, DoOperateSecretSeed::kDoJumpFrom);
}
/// @brief Sets the value of this seed object to "jump" from its current state to the specified number of steps according to the "count" expression defined by the associated cryptographic algorithm.
///         The object specified by the from parameter is the source object that retains the initial value, which will be used for subsequent jumps.
/// @name   Jump
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @returns reference to this updated object
keys::isoft_def::PKeysProcess_Base::PResultLen PKeysProcess_SecretSeed::Jump(
    keys::isoft_def::PIpcPac_Head const *pReqHead, keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept
{
    return SecretSeedOperator(pReqHead, aswMsg, DoOperateSecretSeed::kDoJump);
}
/// @brief Sets the next value of the secret seed according to the "count" expression defined by the cryptographic algorithm associated with this object.
/// @name   Next
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @returns reference to this updated object
keys::isoft_def::PKeysProcess_Base::PResultLen PKeysProcess_SecretSeed::Next(
    keys::isoft_def::PIpcPac_Head const *pReqHead, keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept
{
    return SecretSeedOperator(pReqHead, aswMsg, DoOperateSecretSeed::kDoNext);
}
/// @brief Xor operation
/// @name   OperatorXor
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @returns reference to this updated object
keys::isoft_def::PKeysProcess_Base::PResultLen PKeysProcess_SecretSeed::OperatorXor(
    keys::isoft_def::PIpcPac_Head const *pReqHead, keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept
{
    return SecretSeedOperator(pReqHead, aswMsg, DoOperateSecretSeed::kDoOperatorXor);
}
/// @brief Gets size
/// @name   GetPayloadSize
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @returns reference to this updated object
keys::isoft_def::PKeysProcess_Base::PResultLen PKeysProcess_SecretSeed::GetPayloadSize(
    keys::isoft_def::PIpcPac_Head const *pReqHead, keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept
{
    if (nullptr == pReqHead) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);  // IPC parameter error
    }
    keys::isoft_def::cryptoctx::PIpcReq_Seed const *const pReqMsg{
        pReqHead->GetBody< keys::isoft_def::cryptoctx::PIpcReq_Seed >()};
    uint32_t const nSlotId{pReqMsg->GetIpcSlotID()};

    keys::isoft_def::PSvrKeySlotLoader *const pFindSlotLoader{_FindSlotLoader(nSlotId)};
    if (nullptr == pFindSlotLoader) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcSlotNotExist);
    }

    ara::core::Result< ara::core::Vector< uint8_t > > const result{pFindSlotLoader->LoadKeyData()};
    if (!result.HasValue()) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kAccessViolation);
    }

    // Build response packet
    uint16_t const nAswLen{
        static_cast< uint16_t >(pReqHead->GetHeadLen() + sizeof(keys::isoft_def::cryptoctx::PIpcReq_Seed))};
    aswMsg.CreatePacket(nAswLen);
    std::ignore = aswMsg.InitIpcHead(pReqHead, nAswLen);
    keys::isoft_def::cryptoctx::PIpcAsw_Seed *const pPacAsw{
        static_cast< keys::isoft_def::cryptoctx::PIpcAsw_Seed * >(static_cast< void * >(aswMsg.GetIpcBody()))};
    pPacAsw->SetErrorID(0);
    pPacAsw->SetDataLen(static_cast< uint32_t >(result.Value().size()));

    return PResultLen::FromValue(pReqHead->nPacSize);
}
/// @brief Abstract common logic processing function
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @param doOperator Wrapping operation type
/// @return reference to this updated object
keys::isoft_def::PKeysProcess_Base::PResultLen PKeysProcess_SecretSeed::SecretSeedOperator(
    keys::isoft_def::PIpcPac_Head const *pReqHead,
    keys::isoft_def::PIpcAutoPacket &aswMsg,
    DoOperateSecretSeed doOperator) const noexcept
{
    if (nullptr == pReqHead) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);  // IPC parameter error
    }
    keys::isoft_def::cryptoctx::PIpcReq_Seed const *const pReqMsg{
        pReqHead->GetBody< keys::isoft_def::cryptoctx::PIpcReq_Seed >()};
    uint8_t const *const pData{pReqMsg->GetData()};
    uint32_t const cloneLen{pReqMsg->GetDataLen()};
    uint32_t const nSlotId{pReqMsg->GetIpcSlotID()};
    int64_t const nSteps{pReqMsg->GetSteps()};

    keys::isoft_def::PSvrKeySlotLoader *const pFindSlotLoader{_FindSlotLoader(nSlotId)};
    if (nullptr == pFindSlotLoader) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcSlotNotExist);
    }

    ara::core::Result< ara::core::Vector< uint8_t > > const result{pFindSlotLoader->LoadKeyData()};
    if (!result.HasValue()) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kAccessViolation);
    }
    ara::core::Vector< uint8_t > const &vecData{result.Value()};
    std::size_t const keyLen{vecData.size()};

    PSecretSeed::Uptr const pSecretSeed{std::make_unique< PSecretSeed >(vecData.data(), keyLen)};
    if (pSecretSeed == nullptr) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);  // IPC parameter error
    }

    ReadOnlyMemRegion const memData{pData, static_cast< size_t >(cloneLen)};

    uint8_t const *pDataAfterOperatr{nullptr};
    std::size_t dataLen{0U};
    bool needReturn{false};
    std::size_t returnDataLen{0U};

    if (doOperator == DoOperateSecretSeed::kDoClone) {
        ara::core::Result< ara::crypto::cryp::SecretSeed::Uptr > resSecretSeed{pSecretSeed->Clone(memData)};
        if (!resSecretSeed.HasValue()) {
            return PResultLen::FromError(SecurityErrorDomain::Errc::kLogicFault);
        }
        ara::crypto::cryp::SecretSeed::Uptr cloneSecretSeed{std::move(resSecretSeed).Value()};
        ara::core::Result< PSecretSeed::Uptrc > pResultSecretSeed{
            SecretSeed::Downcast< PSecretSeed >(std::move(cloneSecretSeed))};
        if (!pResultSecretSeed.HasValue()) {
        }
        PSecretSeed::Uptrc const clonepSecretSeed{std::move(pResultSecretSeed).Value()};
        needReturn        = true;
        pDataAfterOperatr = clonepSecretSeed->GetSecretSeedData();
        dataLen           = clonepSecretSeed->GetDataLen();
        returnDataLen     = dataLen;
    } else if (doOperator == DoOperateSecretSeed::kDoJumpFrom) {
        PSecretSeed::Uptr const pSecretSeedOther{std::make_unique< PSecretSeed >(pData, cloneLen)};
        ara::core::Result< void > const resVoid{pSecretSeed->JumpFrom(*pSecretSeedOther, nSteps)};
        if (!resVoid.HasValue()) {
            return PResultLen::FromError(SecurityErrorDomain::Errc::kLogicFault);
        }
        pDataAfterOperatr = pSecretSeedOther->GetSecretSeedData();
        dataLen           = pSecretSeedOther->GetDataLen();
    } else if (doOperator == DoOperateSecretSeed::kDoNext) {
        std::ignore       = pSecretSeed->Next();
        pDataAfterOperatr = pSecretSeed->GetSecretSeedData();
        dataLen           = pSecretSeed->GetDataLen();
    } else if (doOperator == DoOperateSecretSeed::kDoOperatorXor) {
        pSecretSeed->operator^=(memData);
        pDataAfterOperatr = pSecretSeed->GetSecretSeedData();
        dataLen           = pSecretSeed->GetDataLen();
    } else {
        /// Unknown operation, return error
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);  // IPC parameter error
    }
    /// No need to return, just store it
    if (!needReturn) {
        ara::core::Result< bool > const resBool{
            pFindSlotLoader->SaveKeyData(nSlotId, pDataAfterOperatr, static_cast< uint16_t >(dataLen))};
        if (!resBool.HasValue()) {
            if (resBool.Value() == false) {
                return PResultLen::FromError(SecurityErrorDomain::Errc::kAccessViolation);
            }
        }
    }
    // Build response packet
    uint16_t const nAswLen{static_cast< uint16_t >(static_cast< size_t >(pReqHead->GetHeadLen())
                                                   + sizeof(keys::isoft_def::cryptoctx::PIpcReq_Seed) + returnDataLen)};
    aswMsg.CreatePacket(nAswLen);
    std::ignore = aswMsg.InitIpcHead(pReqHead, nAswLen);
    keys::isoft_def::cryptoctx::PIpcAsw_Seed *const pPacAsw{
        static_cast< keys::isoft_def::cryptoctx::PIpcAsw_Seed * >(static_cast< void * >(aswMsg.GetIpcBody()))};
    pPacAsw->SetErrorID(0);
    pPacAsw->SetDataLen(static_cast< uint32_t >(keyLen));
    /// Need to return the data
    if (needReturn) {
        std::ignore = memcpy(static_cast< uint8_t * >(static_cast< void * >(pPacAsw + 1)), pDataAfterOperatr, dataLen);
    }
    return PResultLen::FromValue(pReqHead->nPacSize);
}
//********************************/
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara