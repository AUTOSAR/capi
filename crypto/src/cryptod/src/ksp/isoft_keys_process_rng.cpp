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
/// @file       isoft_keys_process_rng.cpp
/// @brief      AutoSar-Crypto Key Storage Module
/// @details
/// @date       2022-08-17
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Key Manager/Random Number Generation IPC Service
/// @interface_level=module
/// @trace_id_sr=SR_CRYPTO_01017
/// @unit_name=PKeysProcess_Rng
/// @unit_description=Random number generation IPC service
/// @endcode
///
/// ================================================================

#include "ara/crypto/ksp/isoft_keys_process_rng.h"

#include <openssl/rand.h>

#include "ara/crypto/ksp/isoft_keys_manager.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {

//********************************/
/// @brief Random number processing logic
/// @param lpcProcessManager LCP command processing manager
PKeysProcess_Rng::PKeysProcess_Rng(keys::isoft_def::PKeys_Manager &lpcProcessManager) noexcept
    : PKeysProcess_T_Base< PKeysProcess_Rng >{this, lpcProcessManager}
{
    _InsertMapCmd(FUNC_NAME_RNG(SetKey), &PKeysProcess_Rng::SetKey);
    _InsertMapCmd(FUNC_NAME_RNG(Generate), &PKeysProcess_Rng::Generate);
}

/// @brief Sets key
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @returns  has value if setkey sucess
keys::isoft_def::PKeysProcess_Base::PResultLen PKeysProcess_Rng::SetKey(
    keys::isoft_def::PIpcPac_Head const *const pReqHead, keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept
{
    return _SetKey(pReqHead, aswMsg);
}

/// @brief Generates random numbers
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @returns  has value if generate sucess
keys::isoft_def::PKeysProcess_Base::PResultLen PKeysProcess_Rng::Generate(
    keys::isoft_def::PIpcPac_Head const *const pReqHead, keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept
{
    if (nullptr == pReqHead) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);  // IPC parameter error;
    }

    keys::isoft_def::cryptoctx::PIpcReq_Rng const *const pReqMsg{
        pReqHead->GetBody< keys::isoft_def::cryptoctx::PIpcReq_Rng >()};
    uint32_t const nIpcSlotID{pReqMsg->GetIpcSlotID()};
    uint32_t const nGenerateLen{pReqMsg->GetGenerateLen()};
    uint32_t nDataLen{0U};
    uint8_t const *pData{nullptr};

    ara::core::Result< ara::core::Vector< uint8_t > > const result{_LoadKeyData(nIpcSlotID)};
    if (!result.HasValue()) {
        return ProcessCmd_Error(pReqHead, aswMsg, result.Error().Value());
    }
    pData    = result.Value().data();
    nDataLen = static_cast< uint32_t >(result.Value().size());

    RAND_seed(pData, static_cast< int32_t >(nDataLen));

    if (0 == RAND_status()) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kUninitializedContext);  // IPC parameter error;
    }

    ara::core::Vector< ara::core::Byte > vecRandom;
    vecRandom.resize(static_cast< std::size_t >(nGenerateLen));
    int32_t const ret{
        RAND_bytes(T_TransBytes< ara::core::Byte >(vecRandom.data()), static_cast< int32_t >(nGenerateLen))};
    if (ret == 0) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kUninitializedContext);  // IPC parameter error;
    }

    // Build response packet
    uint16_t const nAswLen{static_cast< uint16_t >(
        pReqHead->GetHeadLen() + static_cast< uint16_t >(sizeof(keys::isoft_def::cryptoctx::PIpcAsw_Rng))
        + static_cast< uint16_t >(nGenerateLen))};
    aswMsg.CreatePacket(nAswLen);
    std::ignore = aswMsg.InitIpcHead(pReqHead, nAswLen);
    keys::isoft_def::cryptoctx::PIpcAsw_Rng *const pPacAsw{
        static_cast< keys::isoft_def::cryptoctx::PIpcAsw_Rng * >(static_cast< void * >(aswMsg.GetIpcBody()))};
    if (pPacAsw == nullptr) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);  // IPC parameter error;
    }
    pPacAsw->SetErrorID(0);
    std::ignore = memcpy(static_cast< uint8_t * >(static_cast< void * >(pPacAsw + 1)), vecRandom.data(),
                         static_cast< std::size_t >(nGenerateLen));
    return PResultLen::FromValue(pReqHead->nPacSize);
}

}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara