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
/// @file       log_command_ipc_client.cpp
/// @brief      IPC client implementation
/// @details
/// @date       2024-06-28
/// @author     yangjinbiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/LOG/DltProtocol
/// @interface_level = unit
/// @unit_name = log_command_ipc_client
/// @unit_description=Implementation of the IPC client communication module on the Dlt lib side
/// @endcode
///
/// ================================================================

#include "log_command_ipc_client.h"

#include <cstring>

#include "isoft/ipccpp/buffer.h"
#include "isoft/ipccpp/utility.h"
#include "isoft/naicpp/evloop.h"

#ifdef ARA_LOG_INTERNAL_IPC_DEBUG
    #include "isoft/ipccpp/debug.h"
#endif

#include "Utils/src/private_log.h"

namespace ara {
namespace log {
namespace internal {

/// @brief Constructor
LogCommandIpcClient::LogCommandIpcClient() noexcept { LOGVERBOSE("enter : ") << __func__; }

/// @brief Destructor
LogCommandIpcClient::~LogCommandIpcClient() noexcept { LOGVERBOSE("enter : ") << __func__; }

/// @brief Initialize the IPC client, establish a connection with the daemon process
/// @return LogCommandIpcClientStatus initialization status
LogCommandIpcClientStatus LogCommandIpcClient::Init() noexcept
{
    LOGVERBOSE("enter : ") << __func__;

    // === Get the event loop ===
    mainLoop_ = isoft::naicpp::GlobalGeneralEvLoop::Get();
    if (nullptr == mainLoop_) {
        LOGVERBOSE(__func__) << "mainloop nullptr";
        return LogCommandIpcClientStatus::kInitFailed;
    }

#ifdef ARA_LOG_INTERNAL_IPC_DEBUG
    // Create the IPC debugging environment
    if (0 > isoft::ipc::debug::CreateIPC("log-demo-id33")) {
        std::ignore = LOGVERBOSE("isoft::ipc::debug::CreateIPC() : ");
    }
    std::ignore = LOGVERBOSE("create ipc ");
#endif

    // === Initialize the IPC client ===
    std::int32_t r = isoft::ipc::IPCInitNaiUDS(mainLoop_);
    if (r == -1) {
        LOGVERBOSE(__func__) << "IPCInitNaiUDS failed ";
        std::ignore = isoft::ipc::IPCDeInitNaiUDS();
        return LogCommandIpcClientStatus::kInitFailed;
    }

    // === Create the IPC client ===
    std::string const ipcAddr{"logd/CMDC"};
    ipcClient_ = isoft::ipc::IPCClient::Create(ipcAddr.c_str());
    if (nullptr == ipcClient_) {
        std::ignore = isoft::ipc::IPCDeInitNaiUDS();
        LOGVERBOSE(__func__) << "IPCClient::Create failed ";
        return LogCommandIpcClientStatus::kClientCreateFailed;
    }

    // === Start the IPC client ===
    if (0 != ipcClient_->Start()) {
        ipcClient_->Release();
        std::ignore = isoft::ipc::IPCDeInitNaiUDS();
        LOGVERBOSE(__func__) << "IPCClient::Start failed ";
        return LogCommandIpcClientStatus::kClientStartFailed;
    }

    status_ = true;
    LOGVERBOSE(__func__) << "leave - Init success";
    return LogCommandIpcClientStatus::kSuccess;
}

/// @brief Destroy the IPC client
/// @return LogCommandIpcClientStatus destruction status
LogCommandIpcClientStatus LogCommandIpcClient::Destroy() noexcept
{
    LOGVERBOSE(__func__) << " enter ";

    // === Stop the IPC client ===
    if (nullptr != ipcClient_) {
        if (0 != ipcClient_->Stop()) {
            LOGVERBOSE("ipcClient_->Stop() failed in : ") << __func__;
            // Continue cleanup, do not return error
        }
        ipcClient_->Release();
        ipcClient_ = nullptr;
    }

    // === Deinitialize IPC ===
    std::ignore = isoft::ipc::IPCDeInitNaiUDS();

#ifdef ARA_LOG_INTERNAL_IPC_DEBUG
    if (0 > isoft::ipc::debug::DestroyIPC("araloglibdebuge")) {
        LOGVERBOSE("isoft::ipc::debug::DestroyIPC()") << __func__;
        // Continue cleanup, do not return error
    }
#endif

    // === Clean up the event loop ===
    if (mainLoop_ != nullptr) {
        mainLoop_ = nullptr;
    }

    status_ = false;
    LOGVERBOSE(__func__) << " leave - Destroy success";
    return LogCommandIpcClientStatus::kSuccess;
}

/// @brief Send an IPC request
/// @param[in] cmd IPC command string (JSON format)
/// @return std::int32_t sending result
///   0 indicates sending succeeded
///   < 0 indicates sending failed
std::int32_t LogCommandIpcClient::SendRequest(std::string const &cmd) noexcept
{
    if (nullptr == ipcClient_) {
        LOGVERBOSE(__func__) << "ipcClient_ is nullptr";
        return -1;
    }

    isoft::ipc::IPCPacket *const ipcRequest{ipcClient_->MakeRequest()};
    if (nullptr == ipcRequest) {
        LOGVERBOSE(__func__) << "MakeRequest failed";
        return -2;
    }

    isoft::ipc::IPCPacketBufferReference *const reqBuffer{ipcRequest->AppendBuffer(cmd.length())};
    if (nullptr == reqBuffer) {
        LOGVERBOSE(__func__) << "AppendBuffer failed";
        return -3;
    }

    std::ignore = memcpy(reqBuffer->GetPtr(), cmd.c_str(), cmd.length());
    std::ignore = reqBuffer->SetLen(cmd.length());

    /// true indicates subscription mode, requiring multiple replies; -1 means no timeout
    std::int32_t const sendResult = ipcClient_->SendAsync(ipcRequest, true, IPCClientSendAsyncHandler, -1, this);

    if (sendResult < 0) {
        LOGERROR(__func__) << "ipc send error: " << sendResult;
        return sendResult;
    }

    return 0;
}

/// @brief IPC asynchronous send callback function
/// @param[in] context User parameter (points to the LogCommandIpcClient object)
/// @param[in] status IPC communication status
/// @param[in] responsePacket Response data packet
void LogCommandIpcClient::IPCClientSendAsyncHandler(void *const context,
                                                    isoft::ipc::IPCClientHandlerStatus const status,
                                                    isoft::ipc::IPCPacket *const responsePacket)
{
    LOGVERBOSE(__func__) << " :enter :";
    isoft::ipc::IPCPacketBufferReference *buffer = nullptr;

    LogCommandIpcClient *const client{static_cast< LogCommandIpcClient * >(context)};
    if (nullptr == client) {
        LOGVERBOSE(__func__) << " nullptr == client";
        return;
    }
    if (status == IPC_CLIENT_HANDLER_STATUS_ERR) {
        LOGVERBOSE(__func__) << " status == IPC_CLIENT_HANDLER_STATUS_ERR";
        return;
    }
    if (nullptr == responsePacket) {
        LOGVERBOSE(__func__) << "nullptr == responsePacket";
        return;
    }
    buffer = responsePacket->GetBuffer();
    if (nullptr == buffer) {
        LOGVERBOSE(__func__) << "nullptr == buffer";
        return;
    }

    // Call the response callback function
    if (client->responseCallback_) {
        std::string const jsonPacket(reinterpret_cast< char const * >(buffer->GetPtr()), buffer->GetLen());
        client->responseCallback_(jsonPacket);
    }
}

}  // namespace internal
}  // namespace log
}  // namespace ara
