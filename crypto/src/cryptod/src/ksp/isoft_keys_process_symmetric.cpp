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
/// @file       isoft_keys_process_symmetric.cpp
/// @brief      AutoSar-Crypto Key Storage Module
/// @details
/// @date       2022-08-17
/// @author     Chang Zheng
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Key Manager/Symmetric Encryption IPC Service
/// @interface_level=module
/// @trace_id_sr=SR_CRYPTO_01001
/// @unit_name=PKeysProcess_Symmetric
/// @unit_description=Symmetric encryption IPC service
/// @endcode
///
/// ================================================================

#include "ara/crypto/ksp/isoft_keys_process_symmetric.h"

#include <openssl/aes.h>
#include <openssl/des.h>

#include "ara/crypto/cryp/isoft_crypto_provider.h"
#include "ara/crypto/cryp/symmetric/isoft_ctx_symmetric_block_aes.h"
#include "ara/crypto/cryp/symmetric/isoft_ctx_symmetric_block_des.h"
#include "ara/crypto/cryp/symmetric/isoft_ctx_symmetric_stream_aes.h"
#include "ara/crypto/cryp/symmetric/isoft_ctx_symmetric_stream_des.h"
#include "ara/crypto/ksp/isoft_keys_manager.h"

namespace ara {
namespace crypto {
namespace cryp {
namespace isoft_def {
//********************************/
/// @brief Symmetric encryption logic
/// @name  PKeysProcess_Symmetric
/// @param lpcProcessManager LCP command processing manager
PKeysProcess_Symmetric::PKeysProcess_Symmetric(keys::isoft_def::PKeys_Manager &lpcProcessManager) noexcept
    : PKeysProcess_T_Base< PKeysProcess_Symmetric >{this, lpcProcessManager}
{
    _InsertMapCmd(FUNC_NAME_Symmetric(SetKey), &PKeysProcess_Symmetric::SetKey);
    _InsertMapCmd(FUNC_NAME_Symmetric(GetPayloadSize), &PKeysProcess_Symmetric::GetPayloadSize);
    /// Block cipher
    _InsertMapCmd(FUNC_NAME_Symmetric(DoCipher_Aes_Ecb), &PKeysProcess_Symmetric::DoCipher_Aes_Ecb);
    _InsertMapCmd(FUNC_NAME_Symmetric(DoCipher_Aes_Cbc), &PKeysProcess_Symmetric::DoCipher_Aes_Cbc);
    _InsertMapCmd(FUNC_NAME_Symmetric(DoCipher_Des_Ecb), &PKeysProcess_Symmetric::DoCipher_Des_Ecb);
    _InsertMapCmd(FUNC_NAME_Symmetric(DoCipher_Des_Cbc), &PKeysProcess_Symmetric::DoCipher_Des_Cbc);
    _InsertMapCmd(FUNC_NAME_Symmetric(DoCipher_3Des_Ecb), &PKeysProcess_Symmetric::DoCipher_3Des_Ecb);
    _InsertMapCmd(FUNC_NAME_Symmetric(DoCipher_3Des_Cbc), &PKeysProcess_Symmetric::DoCipher_3Des_Cbc);
    /// Stream cipher
    _InsertMapCmd(FUNC_NAME_Symmetric(DoCipher_Aes_Ctr_Stream), &PKeysProcess_Symmetric::DoCipher_Aes_Ctr_Stream);
    _InsertMapCmd(FUNC_NAME_Symmetric(DoCipher_Aes_Cfb1_Stream), &PKeysProcess_Symmetric::DoCipher_Aes_Cfb1_Stream);
    _InsertMapCmd(FUNC_NAME_Symmetric(DoCipher_Aes_Cfb8_Stream), &PKeysProcess_Symmetric::DoCipher_Aes_Cfb8_Stream);
    _InsertMapCmd(FUNC_NAME_Symmetric(DoCipher_Aes_Cfb128_Stream), &PKeysProcess_Symmetric::DoCipher_Aes_Cfb128_Stream);
    _InsertMapCmd(FUNC_NAME_Symmetric(DoCipher_Aes_Ofb128_Stream), &PKeysProcess_Symmetric::DoCipher_Aes_Ofb128_Stream);

    _InsertMapCmd(FUNC_NAME_Symmetric(DoCipher_3Des_Cfb1_Stream), &PKeysProcess_Symmetric::DoCipher_3Des_Cfb1_Stream);
    _InsertMapCmd(FUNC_NAME_Symmetric(DoCipher_3Des_Cfb64_Stream), &PKeysProcess_Symmetric::DoCipher_3Des_Cfb64_Stream);
    _InsertMapCmd(FUNC_NAME_Symmetric(DoCipher_3Des_Ofb64_Stream), &PKeysProcess_Symmetric::DoCipher_3Des_Ofb64_Stream);
    _InsertMapCmd(FUNC_NAME_Symmetric(DoCipher_Des_Ofb64_Stream), &PKeysProcess_Symmetric::DoCipher_Des_Ofb64_Stream);
    _InsertMapCmd(FUNC_NAME_Symmetric(DoCipher_Des_Ofb_Stream), &PKeysProcess_Symmetric::DoCipher_Des_Ofb_Stream);
    _InsertMapCmd(FUNC_NAME_Symmetric(DoCipher_Des_Cfb64_Stream), &PKeysProcess_Symmetric::DoCipher_Des_Cfb64_Stream);
    _InsertMapCmd(FUNC_NAME_Symmetric(DoCipher_Des_Cfb_Stream), &PKeysProcess_Symmetric::DoCipher_Des_Cfb_Stream);
}

//********************************/
/// @brief Executes encryption logic
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @param block  Whether it is block encryption
/// @return Common logic processing return value if DoCipher succeeds
/// @code{.isoft}
/// @tparam T_ClassType
/// @interface_level=unit
/// @trace_id_ad=AD_CRYPTO_00000
/// @trace_id_dd=DD_CRYPTO_06107
/// @needwork = dda
/// @endcode
template < typename T_ClassType >
keys::isoft_def::PKeysProcess_Base::PResultLen PKeysProcess_Symmetric::_DoCipher_Common(
    keys::isoft_def::PIpcPac_Head const *const pReqHead,
    keys::isoft_def::PIpcAutoPacket &aswMsg,
    bool const block) const noexcept
{
    if (nullptr == pReqHead) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);  // IPC parameter error
    }
    keys::isoft_def::cryptoctx::PIpcReq_docipher const *const pReqMsg{
        pReqHead->GetBody< keys::isoft_def::cryptoctx::PIpcReq_docipher >()};

    uint32_t const nTransform{pReqMsg->GetTransform()};
    uint32_t const nIpcSlotID{pReqMsg->GetIpcSlotID()};

    int64_t const nOffset{pReqMsg->GetOffset()};
    uint32_t const nCurrentPos{pReqMsg->GetCurrentPos()};
    uint32_t const nFinshBytes{pReqMsg->GetFinishBytes()};
    bool const nFromBegin{pReqMsg->GetFromBegin()};
    bool const nAlgedData{pReqMsg->GetAlgedData()};

    ara::core::Result< ara::core::Vector< uint8_t > > const result{_LoadKeyData(nIpcSlotID)};
    if (!result.HasValue()) {
        return ProcessCmd_Error(pReqHead, aswMsg, result.Error().Value());
    }
    ara::core::Vector< uint8_t > const &vecData{result.Value()};
    std::size_t const keyLen{vecData.size()};

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

    /// Generate key
    cryp::isoft_def::PCryptoProvider::Uptr const pPCryptoProvider{
        std::make_unique< cryp::isoft_def::PCryptoProvider >()};
    ReadOnlyMemRegion const memKeyData{vecData.data(), keyLen};
    ara::core::Result< SymmetricKey::Uptrc > resSymmetricKey{
        pPCryptoProvider->GenerateSymmetricKeyEx(algId, memKeyData, allowedUsageFlags)};
    if (!resSymmetricKey.HasValue()) {
        return ProcessCmd_Error(pReqHead, aswMsg, resSymmetricKey.Error().Value());
    }
    SymmetricKey::Uptrc const pSymmetricKey{std::move(resSymmetricKey).Value()};

    std::unique_ptr< T_ClassType > const pCipherContext{std::move(std::make_unique< T_ClassType >(*pPCryptoProvider))};
    if (pCipherContext == nullptr) {
        return ProcessCmd_Error(pReqHead, aswMsg, SecurityErrorDomain::Errc::kIpcFault);  // IPC parameter error;
    }
    ara::core::Result< void > const resVoid{
        std::move(pCipherContext->SetKey(*pSymmetricKey, static_cast< CryptoTransform >(nTransform)))};
    if (!resVoid.HasValue()) {
        return ProcessCmd_Error(pReqHead, aswMsg, resVoid.Error().Value());
    }

    uint8_t const *const pInputIvData{pReqMsg->GetIvData()};
    uint32_t const inputIvDataLen{pReqMsg->GetIvLen()};
    ReadOnlyMemRegion const memInputIvData{pInputIvData, static_cast< size_t >(inputIvDataLen)};

    /// These are optional calls, so function return values are not checked
    std::ignore = pCipherContext->Start(memInputIvData);
    pCipherContext->SetCurrentPos(nCurrentPos);
    pCipherContext->SetFinishBytes(nFinshBytes);
    std::ignore = pCipherContext->Seek(nOffset, nFromBegin);

    uint8_t const *const pInputData{pReqMsg->GetData()};
    uint32_t const inputDataLen{pReqMsg->GetDataLen()};
    uint8_t *pData{nullptr};
    uint32_t nDataLen{0U};

    ara::core::Vector< ara::core::Byte > vecOut;
    if (!block) {
        vecOut.resize(static_cast< size_t >(inputDataLen));
    }
    ara::core::Result< uint32_t > const res{
        std::move(pCipherContext->DoCipher(vecOut, pInputData, inputDataLen, nAlgedData))};
    if (!res.HasValue()) {
        return ProcessCmd_Error(pReqHead, aswMsg, res.Error().Value());
    }
    pData    = T_TransBytes< ara::core::Byte >(vecOut.data());
    nDataLen = static_cast< uint32_t >(vecOut.size());

    if ((pData == nullptr) || (nDataLen == 0U)) {
        return ProcessCmd_Error(pReqHead, aswMsg, SecurityErrorDomain::Errc::kIpcFault);
    }
    // Build response packet
    uint16_t const nAswLen{static_cast< uint16_t >(
        pReqHead->GetHeadLen() + static_cast< uint32_t >(sizeof(keys::isoft_def::cryptoctx::PIpcAsw_docipher))
        + nDataLen)};
    aswMsg.CreatePacket(nAswLen);
    std::ignore = aswMsg.InitIpcHead(pReqHead, nAswLen);
    aswMsg.SetFirstSize(static_cast< int32_t >(pReqHead->GetHeadLen()));
    keys::isoft_def::cryptoctx::PIpcAsw_docipher pacAsw;
    pacAsw.SetErrorID(0);
    pacAsw.SetDataLen(nDataLen);
    pacAsw.SetCurrentPos(pCipherContext->GetCurrentPos());
    pacAsw.SetFinishBytes(pCipherContext->GetFinishBytes());
    std::ignore = aswMsg.AddDataToIpc(&pacAsw, sizeof(pacAsw));
    std::ignore = aswMsg.AddDataToIpc(pData, static_cast< uint16_t >(nDataLen));

    return PResultLen::FromValue(pReqHead->nPacSize);
}
//********************************/
/// @brief Sets key
/// @name  SetKey
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @returns  has vlaue if SetKey sucess
keys::isoft_def::PKeysProcess_Base::PResultLen PKeysProcess_Symmetric::SetKey(
    keys::isoft_def::PIpcPac_Head const *pReqHead, keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept
{
    return _SetKey(pReqHead, aswMsg);
}
//********************************/
/// @brief Gets symmetric key size
/// @name  GetPayloadSize
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @returns  Key size
keys::isoft_def::PKeysProcess_Base::PResultLen PKeysProcess_Symmetric::GetPayloadSize(
    keys::isoft_def::PIpcPac_Head const *pReqHead, keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept
{
    if (nullptr == pReqHead) {
        return PResultLen::FromError(SecurityErrorDomain::Errc::kIpcFault);  // IPC parameter error;
    }
    keys::isoft_def::cryptoctx::PIpcReq_Common const *const pReqMsg{
        pReqHead->GetBody< keys::isoft_def::cryptoctx::PIpcReq_Common >()};

    ara::core::Result< ara::core::Vector< uint8_t > > const result{_LoadKeyData(pReqMsg->GetIpcSlotID())};
    if (!result.HasValue()) {
        return ProcessCmd_Error(pReqHead, aswMsg, result.Error().Value());
    }

    size_t const nPayloadSize{result.Value().size()};

    // Build response packet
    uint16_t const nAswLen{
        static_cast< uint16_t >(pReqHead->GetHeadLen() + sizeof(keys::isoft_def::cryptoctx::PIpcAsw_common))};
    aswMsg.CreatePacket(nAswLen);
    std::ignore = aswMsg.InitIpcHead(pReqHead, nAswLen);
    keys::isoft_def::cryptoctx::PIpcAsw_common *const pPacAsw{
        static_cast< keys::isoft_def::cryptoctx::PIpcAsw_common * >(static_cast< void * >(aswMsg.GetIpcBody()))};
    pPacAsw->SetErrorID(0);
    pPacAsw->SetDataLen(static_cast< uint32_t >(nPayloadSize));

    return PResultLen::FromValue(pReqHead->nPacSize);
}
//********************************/
/// @brief aesecb logic processing
/// @name  DoCipher_Aes_Ecb
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @returns  has vlaue if DoCipher sucess
keys::isoft_def::PKeysProcess_Base::PResultLen PKeysProcess_Symmetric::DoCipher_Aes_Ecb(
    keys::isoft_def::PIpcPac_Head const *pReqHead, keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept
{
    return _DoCipher_Common< PCtxSymmetricBlock_Aes_Ecb >(pReqHead, aswMsg);
}
//********************************/

/// @brief aescbc logic processing
/// @name  DoCipher_Aes_Cbc
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @returns  has vlaue if DoCipher sucess
keys::isoft_def::PKeysProcess_Base::PResultLen PKeysProcess_Symmetric::DoCipher_Aes_Cbc(
    keys::isoft_def::PIpcPac_Head const *pReqHead, keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept
{
    return _DoCipher_Common< PCtxSymmetricBlock_Aes_Cbc >(pReqHead, aswMsg);
}
//********************************/

/// @brief Des_Ecb logic processing
/// @name  DoCipher_Des_Ecb
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @returns  has vlaue if DoCipher sucess
keys::isoft_def::PKeysProcess_Base::PResultLen PKeysProcess_Symmetric::DoCipher_Des_Ecb(
    keys::isoft_def::PIpcPac_Head const *pReqHead, keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept
{
    return _DoCipher_Common< PCtxSymmetricBlock_Des_Ecb >(pReqHead, aswMsg);
}
/// @brief Des_Cbc logic processing
/// @name  DoCipher_Des_Cbc
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @returns  has vlaue if DoCipher sucess
keys::isoft_def::PKeysProcess_Base::PResultLen PKeysProcess_Symmetric::DoCipher_Des_Cbc(
    keys::isoft_def::PIpcPac_Head const *pReqHead, keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept
{
    return _DoCipher_Common< PCtxSymmetricBlock_Des_Cbc >(pReqHead, aswMsg);
}
/// @brief 3Des_Ecb logic processing
/// @name  DoCipher_3Des_Ecb
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @returns  has vlaue if DoCipher sucess
keys::isoft_def::PKeysProcess_Base::PResultLen PKeysProcess_Symmetric::DoCipher_3Des_Ecb(
    keys::isoft_def::PIpcPac_Head const *pReqHead, keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept
{
    return _DoCipher_Common< PCtxSymmetricBlock_3Des_Ecb >(pReqHead, aswMsg);
}
/// @brief 3Des_Cbc logic processing
/// @name  DoCipher_3Des_Cbc
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @returns has vlaue if DoCipher sucess
keys::isoft_def::PKeysProcess_Base::PResultLen PKeysProcess_Symmetric::DoCipher_3Des_Cbc(
    keys::isoft_def::PIpcPac_Head const *pReqHead, keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept
{
    return _DoCipher_Common< PCtxSymmetricBlock_3Des_Cbc >(pReqHead, aswMsg);
}

/// @brief aesctr stream encryption logic
/// @name  DoCipher_Aes_Ctr_Stream
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @returns  has vlaue if DoCipher sucess
keys::isoft_def::PKeysProcess_Base::PResultLen PKeysProcess_Symmetric::DoCipher_Aes_Ctr_Stream(
    keys::isoft_def::PIpcPac_Head const *pReqHead, keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept
{
    return _DoCipher_Common< PCtxSymmetricStream_Aes_Ctr >(pReqHead, aswMsg, false);
}
/// @brief Aes_Cfb1 stream encryption logic
/// @name  DoCipher_Aes_Cfb1_Stream
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @returns  has vlaue if DoCipher sucess
keys::isoft_def::PKeysProcess_Base::PResultLen PKeysProcess_Symmetric::DoCipher_Aes_Cfb1_Stream(
    keys::isoft_def::PIpcPac_Head const *pReqHead, keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept
{
    return _DoCipher_Common< PCtxSymmetricStream_Aes_Cfb1 >(pReqHead, aswMsg, false);
}
/// @brief Aes_Cfb8 execute encryption operation
/// @name  DoCipher_Aes_Cfb8_Stream
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @returns  has vlaue if DoCipher sucess
keys::isoft_def::PKeysProcess_Base::PResultLen PKeysProcess_Symmetric::DoCipher_Aes_Cfb8_Stream(
    keys::isoft_def::PIpcPac_Head const *pReqHead, keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept
{
    return _DoCipher_Common< PCtxSymmetricStream_Aes_Cfb8 >(pReqHead, aswMsg, false);
}
/// @brief Aes_Cfb128 execute encryption operation
/// @name  DoCipher_Aes_Cfb128_Stream
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @returns  has vlaue if DoCipher sucess
keys::isoft_def::PKeysProcess_Base::PResultLen PKeysProcess_Symmetric::DoCipher_Aes_Cfb128_Stream(
    keys::isoft_def::PIpcPac_Head const *pReqHead, keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept
{
    return _DoCipher_Common< PCtxSymmetricStream_Aes_Cfb128 >(pReqHead, aswMsg, false);
}
/// @brief Aes_Ofb128 execute encryption operation
/// @name  DoCipher_Aes_Ofb128_Stream
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @returns  has vlaue if DoCipher sucess
keys::isoft_def::PKeysProcess_Base::PResultLen PKeysProcess_Symmetric::DoCipher_Aes_Ofb128_Stream(
    keys::isoft_def::PIpcPac_Head const *pReqHead, keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept
{
    return _DoCipher_Common< PCtxSymmetricStream_Aes_Ofb128 >(pReqHead, aswMsg, false);
}

/// @brief Des_Cfb64 logic processing
/// @name  DoCipher_Des_Cfb64_Stream
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @returns  has vlaue if DoCipher sucess
keys::isoft_def::PKeysProcess_Base::PResultLen PKeysProcess_Symmetric::DoCipher_Des_Cfb64_Stream(
    keys::isoft_def::PIpcPac_Head const *pReqHead, keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept
{
    return _DoCipher_Common< PCtxSymmetricStream_Des_Cfb64 >(pReqHead, aswMsg, false);
}

/// @brief Des_Cfb execute encryption operation
/// @name  DoCipher_Des_Cfb_Stream
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @returns  has vlaue if DoCipher sucess
keys::isoft_def::PKeysProcess_Base::PResultLen PKeysProcess_Symmetric::DoCipher_Des_Cfb_Stream(
    keys::isoft_def::PIpcPac_Head const *pReqHead, keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept
{
    return _DoCipher_Common< PCtxSymmetricStream_Des_Cfb >(pReqHead, aswMsg, false);
}

/// @brief Des_Ofb64 logic processing
/// @name  DoCipher_Des_Ofb64_Stream
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @returns  has vlaue if DoCipher sucess
keys::isoft_def::PKeysProcess_Base::PResultLen PKeysProcess_Symmetric::DoCipher_Des_Ofb64_Stream(
    keys::isoft_def::PIpcPac_Head const *pReqHead, keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept
{
    return _DoCipher_Common< PCtxSymmetricStream_Des_Ofb64 >(pReqHead, aswMsg, false);
}
/// @brief Des_Ofb logic processing
/// @name  DoCipher_Des_Ofb_Stream
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @returns  has vlaue if DoCipher sucess
keys::isoft_def::PKeysProcess_Base::PResultLen PKeysProcess_Symmetric::DoCipher_Des_Ofb_Stream(
    keys::isoft_def::PIpcPac_Head const *pReqHead, keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept
{
    return _DoCipher_Common< PCtxSymmetricStream_Des_Ofb >(pReqHead, aswMsg, false);
}
/// @brief 3Des_Cfb1 execute encryption operation
/// @name  DoCipher_3Des_Cfb1_Stream
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @returns  has vlaue if DoCipher sucess
keys::isoft_def::PKeysProcess_Base::PResultLen PKeysProcess_Symmetric::DoCipher_3Des_Cfb1_Stream(
    keys::isoft_def::PIpcPac_Head const *pReqHead, keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept
{
    return _DoCipher_Common< PCtxSymmetricStream_3Des_Cfb1 >(pReqHead, aswMsg, false);
}
/// @brief 3Des_Cfb64 execute encryption operation
/// @name  DoCipher_3Des_Cfb64_Stream
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @returns  has vlaue if DoCipher sucess
keys::isoft_def::PKeysProcess_Base::PResultLen PKeysProcess_Symmetric::DoCipher_3Des_Cfb64_Stream(
    keys::isoft_def::PIpcPac_Head const *pReqHead, keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept
{
    return _DoCipher_Common< PCtxSymmetricStream_3Des_Cfb64 >(pReqHead, aswMsg, false);
}
/// @brief 3Des_Ofb64 execute encryption operation
/// @name  DoCipher_3Des_Ofb64_Stream
/// @param pReqHead IPC request packet header
/// @param aswMsg IPC managed response message
/// @returns  has vlaue if DoCipher sucess
keys::isoft_def::PKeysProcess_Base::PResultLen PKeysProcess_Symmetric::DoCipher_3Des_Ofb64_Stream(
    keys::isoft_def::PIpcPac_Head const *pReqHead, keys::isoft_def::PIpcAutoPacket &aswMsg) const noexcept
{
    return _DoCipher_Common< PCtxSymmetricStream_3Des_Ofb64 >(pReqHead, aswMsg, false);
}
//********************************/
}  // namespace  isoft_def
}  // namespace cryp
}  // namespace crypto
}  // namespace ara