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
/// @file       isoft_ipc_auto_message.cpp
/// @brief      AutoSar-Crypto Key Storage Module
/// @details
/// @date       2022-08-17
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Reusable Functions/Reusable Function Module
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_06005
/// @unit_name=PIpcAutoPacket
/// @unit_description=IPC message encapsulation class with automatic memory allocation/deallocation
/// @endcode
///
/// ================================================================

#include "ara/crypto/ipc/isoft_ipc_auto_message.h"

#include <memory.h>
#include <nai/os/nai_thread.h>

#include "ara/crypto/ipc/isoft_ipc_protocol.h"

namespace ara {
namespace crypto {
namespace keys {
namespace isoft_def {
//********************************/
/// @brief Reinitialize
void PIpcAutoPacket::ReInit() noexcept
{
    _DelPacket();
    bAutoDelPacket_ = false;
}
/// @brief Constructor with parameters
/// @param pIpcPacket IPC packet
/// @param nNewLen Length of the new IPC packet
/// @param bAutoDelPacket Whether to automatically release
PIpcAutoPacket::PIpcAutoPacket(isoft::ipc::IPCPacket *const pIpcPacket,
                               uint16_t const nNewLen,
                               bool const bAutoDelPacket) noexcept
    : PIpcAutoPacket{}
{
    pIpcPacket_     = pIpcPacket;
    bAutoDelPacket_ = bAutoDelPacket;
    _NewPacketBuff(nNewLen);
}
/// @brief Destructor
/// @name  ~PIpcAutoPacket
PIpcAutoPacket::~PIpcAutoPacket() noexcept { _DelPacket(); }
/// @brief Return the data in the first buffer of the IPCPacket (there may be many, so this function is only suitable for processing small data packets)
/// @name  data
/// @returns Starting address of data
uint8_t *PIpcAutoPacket::data() const noexcept
{
    if (nullptr == pIpcPacket_) {
        return nullptr;
    }
    isoft::ipc::IPCPacketBufferReference *const pBuffer{pIpcPacket_->GetBuffer()};
    return pBuffer->GetPtr();
}
/// @brief Return the length of the first node of the buffer chain
/// @name  SetFirstSize
/// @param nSize Data length
void PIpcAutoPacket::SetFirstSize(int32_t const nSize) const noexcept
{
    isoft::ipc::IPCPacketBufferReference *pBuffer{pIpcPacket_->GetBuffer()};
    if (nullptr == pBuffer) {
        pBuffer = pIpcPacket_->AppendBuffer(static_cast< std::size_t >(nSize));
    }
    std::ignore = pBuffer->SetLen(static_cast< std::size_t >(nSize));
}
/// @brief Get the extra data length
/// @return Extra data length
uint32_t PIpcAutoPacket::GetExtSize() const noexcept
{
    if (nullptr == pIpcPacket_) {
        return 0U;
    }
    isoft::ipc::IPCPacketBufferReference *pBuffer{pIpcPacket_->GetBuffer()};
    if (nullptr == pBuffer) {
        return 0U;
    }
    pBuffer = pIpcPacket_->GetNextBuffer(pBuffer);
    uint32_t nBuffTotal{0U};
    while (true) {
        if (pBuffer == nullptr) {
            break;
        }
        nBuffTotal += static_cast< uint32_t >(pBuffer->GetSize());
        pBuffer = pIpcPacket_->GetNextBuffer(pBuffer);
    }
    return nBuffTotal;
}
/// @brief Get the total data length
/// @return Data length
uint32_t PIpcAutoPacket::GetTotalSize() const noexcept
{
    if (nullptr == pIpcPacket_) {
        return 0U;
    }
    uint32_t nBuffTotal{0U};
    isoft::ipc::IPCPacketBufferReference *pBuffer{pIpcPacket_->GetBuffer()};
    while (true) {
        if (pBuffer == nullptr) {
            break;
        }
        nBuffTotal += static_cast< uint32_t >(pBuffer->GetSize());
        pBuffer = pIpcPacket_->GetNextBuffer(pBuffer);
    }
    return nBuffTotal;
}
/// @brief Add data to pIpcPacket_
/// @param pVoidData Starting address of data
/// @param nDataLen Data length
/// @return Length of data added
uint32_t PIpcAutoPacket::AddDataToIpc(void const *const pVoidData, uint16_t const nDataLen) const noexcept
{
    uint32_t nCopyTotal{0U};
    isoft::ipc::IPCPacketBufferReference *pIpcBuff{pIpcPacket_->GetBuffer()};
    if (nullptr == pIpcBuff) {
        pIpcBuff = pIpcPacket_->AppendBuffer(static_cast< size_t >(nDataLen) - static_cast< size_t >(nCopyTotal));
    }
    uint8_t const *const pBData{static_cast< uint8_t const * >(pVoidData)};

    while (true) {
        if ((pIpcBuff == nullptr) || (nCopyTotal >= nDataLen)) {
            break;
        }
        uint32_t const nExistLen{static_cast< uint32_t >(pIpcBuff->GetLen())};
        uint32_t const nEmptyIpcBuffLen{static_cast< uint32_t >(pIpcBuff->GetSize() - nExistLen)};
        if (nEmptyIpcBuffLen > 0U) {
            uint8_t *const pEmptyIpcBuff{pIpcBuff->GetPtr() + nExistLen};
            int32_t const nCopyLen{
                std::min< int32_t >(static_cast< int32_t >(nEmptyIpcBuffLen),
                                    static_cast< int32_t >(nDataLen) - static_cast< int32_t >(nCopyTotal))};
            std::ignore = memcpy(pEmptyIpcBuff, pBData + nCopyTotal, static_cast< std::size_t >(nCopyLen));
            nCopyTotal += static_cast< uint32_t >(nCopyLen);
            std::ignore = pIpcBuff->SetLen(static_cast< size_t >(nExistLen) + static_cast< size_t >(nCopyLen));
        } else {
            pIpcBuff = pIpcPacket_->GetNextBuffer(pIpcBuff);
            if (nullptr == pIpcBuff) {
                pIpcBuff
                    = pIpcPacket_->AppendBuffer(static_cast< size_t >(nDataLen) - static_cast< size_t >(nCopyTotal));
            }
        }
    }
    return nCopyTotal;
}
/// @brief Process multi-packet data using a callback function
/// @param pfunc Callback function
/// @return Packet length
uint32_t PIpcAutoPacket::DealBuffData(
    std::function< uint32_t(uint32_t nIndex, uint8_t const *pData, uint32_t nLen) > const &pfunc) const noexcept
{
    if (nullptr == pIpcPacket_) {
        return 0U;
    }
    uint32_t nBuffTotal{0U};
    uint32_t nIndex{0U};
    isoft::ipc::IPCPacketBufferReference *pBuffer{pIpcPacket_->GetBuffer()};
    while (true) {
        if (pBuffer == nullptr) {
            break;
        }
        if (pfunc != nullptr) {
            uint8_t *const pData{pBuffer->GetPtr()};
            nBuffTotal += pfunc(nIndex, pData, static_cast< uint32_t >(pBuffer->GetSize()));
        } else {
            nBuffTotal += static_cast< uint32_t >(pBuffer->GetSize());
        }
        nIndex += 1U;
        pBuffer = pIpcPacket_->GetNextBuffer(pBuffer);
    }
    return nBuffTotal;
}
/// @brief Get the packet body content within the IPC packet
/// @name  GetIpcBody
/// @returns Starting address of the packet content
uint8_t *PIpcAutoPacket::GetIpcBody() const noexcept
{
    if (nullptr == pIpcPacket_) {
        return nullptr;
    }
    PIpcPac_Head *const pIpcHead{static_cast< PIpcPac_Head * >(static_cast< void * >(data()))};
    return T_TransBytes(pIpcHead + 1) + pIpcHead->nFuncNameLen;
}
/// @brief Bind an externally allocated IPC packet
/// @name  AttachPacket
/// @param pIpcPacket IPC packet
/// @param bAutoDel Whether to automatically release
void PIpcAutoPacket::AttachPacket(isoft::ipc::IPCPacket *const pIpcPacket, bool const bAutoDel) noexcept
{
    _DelPacket();
    pIpcPacket_     = pIpcPacket;
    bAutoDelPacket_ = bAutoDel;
}
/// @brief Create an empty IPC packet
/// @name  CreatePacket
/// @param nNewLen Length of the new IPC packet
void PIpcAutoPacket::CreatePacket(uint16_t const nNewLen) noexcept { _NewPacketBuff(nNewLen); }
/// @brief Get the function name within the IPC packet
/// @name  GetFuncName
/// @returns Function name within the IPC packet
ara::core::StringView PIpcAutoPacket::GetFuncName() const noexcept
{
    if (nullptr == pIpcPacket_) {
        return {""};
    }
    PIpcPac_Head *const pIpcHead{static_cast< PIpcPac_Head * >(static_cast< void * >(data()))};
    return pIpcHead->GetFuncName();
}
/// @brief Initialize the IPC packet header
/// @name  InitIpcHead
/// @param stFuncName Function name used for IPC call
/// @param nPacSize Length of the IPC packet
/// @returns true if init sucess false otherwise
bool PIpcAutoPacket::InitIpcHead(ara::core::StringView const &stFuncName, uint16_t const nPacSize) noexcept
{
    if (nullptr == pIpcPacket_) {
        return false;
    }
    isoft::ipc::IPCPacketBufferReference *const pBuffer{pIpcPacket_->GetBuffer()};
    uint8_t *const pBuffData{data()};
    if (nullptr == pBuffData) {
        return false;
    }
    PIpcPac_Head *const pPacHead{static_cast< PIpcPac_Head * >(static_cast< void * >(pBuffer->GetPtr()))};
    pPacHead->nProcessID   = static_cast< uint64_t >(nai_thread_id());
    pPacHead->nCrc         = 0;
    pPacHead->nPacSize     = nPacSize;
    pPacHead->nFuncNameLen = static_cast< uint16_t >(stFuncName.size());
    std::ignore
        = memcpy(static_cast< uint8_t * >(static_cast< void * >(pPacHead + 1)), stFuncName.data(), stFuncName.size());
    // Set the length of the Packet
    std::ignore = pBuffer->SetLen(static_cast< std::size_t >(nPacSize));
    return nullptr != pPacHead;
}
/// @brief Initialize the IPC packet header
/// @name  InitIpcHead
/// @param pIpcMsg IPC message
/// @param nPacSize Length of the IPC packet
/// @returns  true if init sucess false otherwise
bool PIpcAutoPacket::InitIpcHead(void const *const pIpcMsg, uint16_t const nPacSize) noexcept
{
    PIpcPac_Head const *const pIpcMsgHead{static_cast< PIpcPac_Head const * >(static_cast< void const * >(pIpcMsg))};
    return InitIpcHead(pIpcMsgHead->GetFuncName(), nPacSize);
}
/// @brief Get the process ID
/// @name  GetProcessId
/// @returns Process ID
uint64_t PIpcAutoPacket::GetProcessId() const noexcept
{
    if (nullptr == pIpcPacket_) {
        return 0U;
    }
    return pIpcPacket_->GetPeerPid();
}
/// @brief Get the session ID
/// @name  GetSessionId
/// @returns Session ID
uint64_t PIpcAutoPacket::GetSessionId() const noexcept
{
    if (nullptr == pIpcPacket_) {
        return 0U;
    }
    return pIpcPacket_->GetSessionId();
}
//***************/
/// @brief Create a new Packet
/// @name  _NewPacketBuff
/// @param nNewLen Length of the new IPC packet
void PIpcAutoPacket::_NewPacketBuff(uint16_t const nNewLen) noexcept
{
    std::ignore = pIpcPacket_->AppendBuffer(static_cast< std::size_t >(nNewLen));
}
/// @brief Release the Packet
/// @name  _DelPacket
void PIpcAutoPacket::_DelPacket() noexcept
{
    if (bAutoDelPacket_) {
        pIpcPacket_ = isoft::ipc::IPCPacket::Release(pIpcPacket_);
    } else {
        pIpcPacket_ = nullptr;
    }
}
//********************************/
}  // namespace  isoft_def
}  // namespace keys
}  // namespace crypto
}  // namespace ara
