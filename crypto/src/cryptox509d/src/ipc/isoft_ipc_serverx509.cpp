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
/// @file       isoft_ipc_serverx509.cpp
/// @brief      AutoSar-Crypto Key Storage Module
/// @details
/// @date       2022-08-01
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Reusable Features/IPC Server
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_06003
/// @unit_name=PIpcServerX509
/// @unit_description=Certificate IPC Server
/// @endcode
///
/// ================================================================

#include "ara/crypto/ipc/isoft_ipc_serverx509.h"

#include <isoft/ipccpp/buffer.h>
#include <isoft/ipccpp/utility.h>

#include "ara/crypto/common/isoft_assert.h"
#include "ara/crypto/common/isoft_log_api.h"
#include "ara/crypto/x509/isoft_x509_manager.h"

namespace ara {
namespace crypto {
namespace keys {
namespace isoft_def {
//********************************/
/// @brief Extended use of isoft's logging system
using ara::crypto::isoft_def::LogError;
/// @brief Extended use of isoft's logging system
using ara::crypto::isoft_def::LogInfo;
/// @brief Extended use of nai library's event loop
using ::isoft::naicpp::EvLoop;
/// @brief Extended use of nai library's event loop
using ::isoft::naicpp::GlobalGeneralEvLoop;
//********************************/
/// @brief Handles IPC messages: Events
/// @param context IPC message context information
/// @param type IPC service handle type (IPC event type)
/// @param pReqPacket IPC data packet
void PIpcServerX509::CallBack_DealIpcEvent(void const* const context,
                                           isoft::ipc::IPCServerHandleType const type,
                                           isoft::ipc::IPCPacket const* const pReqPacket) noexcept
{
    std::ignore = (context);
    std::ignore = type;
    std::ignore = pReqPacket;
}
/// @brief Handles notification messages
/// @param context IPC message context information
/// @param type IPC service handle type (IPC event type)
/// @param pReqPacket IPC data packet
void PIpcServerX509::CallBack_DealMsgPost(void* const context,
                                          isoft::ipc::IPCServerHandleType const type,
                                          isoft::ipc::IPCPacket* const pReqPacket) noexcept
{
    std::ignore = context;
    std::ignore = type;
    std::ignore = pReqPacket;
    if (type != IPC_SERVER_HANDLER_POST) {
        return;
    }

    PIpcServerX509* const pIpcServer{static_cast< PIpcServerX509* >(context)};
    int32_t const nDealLen{pIpcServer->ProcessIpcPacket(pReqPacket, type)};
    // After processing the message
    isoft::ipc::IPCPacketBufferReference* const pIpcBuff{pReqPacket->GetBuffer()};
    if (static_cast< uint32_t >(nDealLen)
        == pIpcBuff->GetSize())  // nDealLen being 0 means the logic deletes the request message itself
    {
    }
    std::ignore = nDealLen;
}
/// @brief Handles request messages
/// @param context IPC message context information
/// @param type IPC service handle type (IPC event type)
/// @param pReqPacket IPC data packet
void PIpcServerX509::CallBack_DealMsgReq(void* const context,
                                         isoft::ipc::IPCServerHandleType const type,
                                         isoft::ipc::IPCPacket* const pReqPacket) noexcept
{
    std::ignore = context;
    std::ignore = type;
    std::ignore = pReqPacket;
    if ((type != IPC_SERVER_HANDLER_ONCE) && (type != IPC_SERVER_HANDLER_MULTI)) {
        return;
    }

    PIpcServerX509* const pIpcServer{static_cast< PIpcServerX509* >(context)};
    int32_t const nDealLen{pIpcServer->ProcessIpcPacket(pReqPacket, type)};
    // After processing the message
    isoft::ipc::IPCPacketBufferReference* const pIpcBuff{pReqPacket->GetBuffer()};
    if (static_cast< uint32_t >(nDealLen)
        == pIpcBuff->GetSize())  // nDealLen being 0 means the logic deletes the request message itself
    {
    }
    std::ignore = nDealLen;
}
//********************************/
/// @brief Parameterized constructor
/// @param ipcProcessManager KeyProvider IPC server-side key slot manager.
/// @throws
PIpcServerX509::PIpcServerX509(PX509_Manager& ipcProcessManager) noexcept
    : pEvLoop_{}  // NOLINT
    , pIpcServer_{}
    , ipcProcessManager_{ipcProcessManager}
{
}
/// @brief Preparation work before starting the service
/// @return true or false
bool PIpcServerX509::Begin() noexcept
{
    // Initialize event loop
    // 2022-10-27
    // According to GlobalGeneralEvLoop::Initialize internal logic, this function can be called multiple times (only initialized on the first call). So, to be safe, call it again.
    // //2022-10-22 Initialization in ara::core::Initialize depends on whether the precompile macro is defined
    pEvLoop_ = GlobalGeneralEvLoop::Get();
    if (nullptr == pEvLoop_) {
        LogError() << "PIpcServer::Begin(): nullptr == GlobalGeneralEvLoop";
        return false;
    }
    int32_t const r{isoft::ipc::IPCInitNaiUDS(pEvLoop_)};
    if (r == -1) {
        LogError() << "PIpcServer::Begin()->IPCInitNaiUDS(): " << r;
        return false;
    }
    // Initialize IPCServer
    ara::core::String const stIpcName{GetIpcName_Server_X509()};
    pIpcServer_ = isoft::ipc::IPCServer::Create(stIpcName.data());
    if (nullptr == pIpcServer_) {
        LogError() << "PIpcServer::Begin()->isoft::ipc::IPCServer::Create(" << stIpcName.data() << ")";
        return false;
    }
    pIpcServer_->SetHandler(IPC_SERVER_HANDLER_CONNECT, &CallBack_DealIpcEvent, this);
    pIpcServer_->SetHandler(IPC_SERVER_HANDLER_DISCONNECT, &CallBack_DealIpcEvent, this);
    pIpcServer_->SetHandler(IPC_SERVER_HANDLER_POST, &CallBack_DealMsgPost, this);
    pIpcServer_->SetHandler(IPC_SERVER_HANDLER_ONCE, &CallBack_DealMsgReq, this);
    pIpcServer_->SetHandler(IPC_SERVER_HANDLER_MULTI, &CallBack_DealMsgReq, this);
    LogInfo() << "PIpcServer::Begin";

    return true;
}
/// @brief Start the service
/// @return true or false
bool PIpcServerX509::Working() noexcept
{
    if (pEvLoop_ == nullptr) {
        return false;
    }
    int32_t ret{pIpcServer_->Start()};
    if (ret < 0) {
        LogInfo() << "PIpcServer::start() error ";
        return false;
    }

    LogInfo() << "PIpcServer::Working";
    ret = pEvLoop_->Run(true);
    if (ret < 0) {
        LogInfo() << "PIpcServer::Working evloop run error ";
        return false;
    }
    LogInfo() << "PIpcServer::Working Exit.";
    return true;
}
/// @brief Cleanup work before ending the service
/// @return true or false
bool PIpcServerX509::End() noexcept
{
    if (pIpcServer_ != nullptr) {
        std::ignore = pIpcServer_->Stop();
        pIpcServer_->Release();
        pIpcServer_.reset();
    }
    /// Do not stop Evloop; other modules may still be using it. Even if not in use, no need to close it.
    if (pEvLoop_ != nullptr) {
        pEvLoop_.reset();
    }

    if (-1 == isoft::ipc::IPCDeInitNaiUDS()) {
        LogError() << "eccp::Server::Close()->IPCDeInitNaiUDS()";
        return false;
    }
    LogInfo() << "PIpcServer::End";
    return true;
}
/// @brief Stop the service
/// @return 0 sucess fail otherwise
int32_t PIpcServerX509::Stop() noexcept
{
    int32_t const ret{pIpcServer_->Stop()};
    if (0 != ret) {
    }

    pEvLoop_.reset();
    return ret;
}

/// @brief Processes IPC data packets
/// @param pReqPacket IPC data packet
/// @param type IPC service handle type (IPC event type)
/// @return
/// @throws
int32_t PIpcServerX509::ProcessIpcPacket(isoft::ipc::IPCPacket* const pReqPacket,
                                         isoft::ipc::IPCServerHandleType const type) noexcept
{
    // IPCPacket::GetBuffer can only get the first Buff; in actual transmission, large-volume data Buff may be a linked list
    PIpcAutoPacket reqMsg;
    reqMsg.AttachPacket(pReqPacket, false);
    PIpcAutoPacket aswMsg;
    aswMsg.AttachPacket(pIpcServer_->MakeResponse(pReqPacket->GetSessionId()), false);
    int32_t nReturn{0};
    if (reqMsg.GetExtSize() > 0U) {
        // 2023-01-16 hanjingjing temporarily uses a whole block of cache to receive all input data
        uint32_t const nRecvDataLen{reqMsg.DealBuffData(nullptr)};
        ara::crypto::internal::PAutoBuff recvBuff{nRecvDataLen};
        uint32_t const nDealLen{reqMsg.DealBuffData(
            [&recvBuff](uint32_t nIndex, uint8_t const* const pData, uint32_t const nLen) noexcept -> uint32_t {
                std::ignore = nIndex;
                bool const addDataSuccess{recvBuff.AddData(pData, nLen)};
                if (addDataSuccess == true) {
                    return nLen;
                }
                return 0U;
            })};
        PH_ASSERT(nRecvDataLen == nDealLen);

        nReturn
            = ipcProcessManager_.ProcessIpcMsg(recvBuff.Data(), static_cast< std::uint16_t >(recvBuff.size()), aswMsg);
    } else {
        isoft::ipc::IPCPacketBufferReference* const pIpcBuff{pReqPacket->GetBuffer()};
        uint8_t* const pReqData{pIpcBuff->GetPtr()};
        uint16_t const nReqLen{static_cast< uint16_t >(pIpcBuff->GetLen())};
        nReturn = ipcProcessManager_.ProcessIpcMsg(pReqData, nReqLen, aswMsg);
    }
    if (nReturn > 0) {
        switch (type) {
            case IPC_SERVER_HANDLER_POST:  // NOLINT
            case IPC_SERVER_HANDLER_ONCE: {
                std::ignore = pIpcServer_->Send(aswMsg.GetIpcPacket(), true);
            } break;
            case IPC_SERVER_HANDLER_MULTI: {
                std::ignore = pIpcServer_->Send(aswMsg.GetIpcPacket(), false);
            } break;
            default: {
                nReturn = 0;
            } break;
        }
    }
    return nReturn;
}
/// @brief Sends IPC data packet
/// @param pReqPacket IPC data packet
/// @param bComplete Whether complete
/// @return 0 sucess fail otherwise
int32_t PIpcServerX509::SendIpcPacket(isoft::ipc::IPCPacket* const pReqPacket, bool const bComplete) const noexcept
{
    if ((nullptr == pIpcServer_) || (nullptr == pReqPacket)) {
        return 0;
    }
    std::ignore = pIpcServer_->Send(pReqPacket, bComplete);
    return static_cast< int32_t >(pReqPacket->GetBuffer()->GetLen());
}
/// @brief Creates a new IPC data packet
/// @param nSessionID Temporary session ID
/// @return
/// @throws
isoft::ipc::IPCPacket* PIpcServerX509::NewIpcPacket(
    uint64_t const nSessionID) const noexcept  // Create a new IPC data packet
{
    if (nullptr == pIpcServer_) {
        return nullptr;
    }
    return pIpcServer_->MakeResponse(nSessionID);
}
//********************************/
}  // namespace  isoft_def
}  // namespace keys
}  // namespace crypto
}  // namespace ara
