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
/// @file       fw_client.cpp
/// @brief      fw client side processing
/// @details    fw client side processing
/// @date       2025-03-20
/// @author     jzy
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/fw/lib client
/// @interface_level=module
/// @trace_id_sr=SR_FW_0011
/// @unit_name=Firewall_Libclient
/// @unit_description=Firewall external interface IPC communication processing
/// class
/// @endcode
///
/// ================================================================
///
///
/// ================================================================

#include "ara/fw/client/fw_client.h"

#include <isoft/ara_fsh/filesystem_hierarchy.h>
#include <isoft/ipccpp/buffer.h>
#include <isoft/ipccpp/client.h>
#include <isoft/ipccpp/packet.h>
#include <isoft/ipccpp/utility.h>
#include <isoft/naicpp/global_evloop.h>
#include <nai/os/nai_proc.h>

#include <cstring>

#include "ara/fw/common/common.h"
#include "ara/fw/internal/ipc_name.h"
#include "ara/fw/internal/log_api.h"
#include "ara/fw/internal/msg_process.h"
#include "ara/fw/internal/serialization_error_domain.h"

namespace ara {
namespace fw {
namespace internal {

using ::ara::core::InstanceSpecifier;
using ::isoft::naicpp::EvLoop;
using ::isoft::naicpp::GlobalGeneralEvLoop;
/// @brief
std::unique_ptr< FWClient > FWClient::sS_Instance_{nullptr};  // NOLINT
/// @brief
std::mutex FWClient::sS_Mtx_{};
/// @brief
/// @return
/// @throw
FWClient *FWClient::GetInstance()
{
    if (sS_Instance_.get() == nullptr) {
        std::lock_guard< std::mutex > const lock{sS_Mtx_};
        if (sS_Instance_.get() == nullptr) {
            sS_Instance_.reset(new FWClient);
            if (sS_Instance_->Init() != 0) {
                LogError() << "FWClient::sS_Instance_ init fail.";
                sS_Instance_.reset();
            }
        }
    }
    return sS_Instance_.get();
}
/// @brief
/// @throw
void FWClient::Destroy()
{
    // int64_t const res{clientPtr_->Stop()};
    // if (res != 0) {
    //     LogError() << "idsc ipc client stop err.";
    //     return;
    // }
    clientPtr_->Release();
    std::ignore = isoft::ipc::IPCDeInitNaiUDS();
    sS_Instance_.reset(nullptr);
}

// FWClient::~FWClient() { Destroy(); }
/// @brief
/// @return
/// @throw
int32_t FWClient::Init()
{
    /// Initialize IPC client
    evLoopPtr_ = GlobalGeneralEvLoop::Get();
    if (evLoopPtr_.get() == nullptr) {
        LogError() << "fw client: Get EvLoop fail. errno: " << SysErr();
        return -1;
    }

    int32_t const retCode{::isoft::ipc::IPCInitNaiUDS(evLoopPtr_)};
    if (retCode == -1) {
        LogError() << "fw client: call IPCInitNaiUDS func fail.";
        std::ignore = isoft::ipc::IPCDeInitNaiUDS();
        return -1;
    }

    clientPtr_ = isoft::ipc::IPCClient::Create(GetkFWIpcFile_Client());
    if (nullptr == clientPtr_.get()) {
        LogError() << "fw client: create ipc client fail. errno: " << SysErr();
        return -1;
    }
    isoft::ipc::IPCClientConnectionHandler const handler{
        [](void *const context, ::isoft::ipc::IPCClientConnectionStatus const status) noexcept {
            FWClient *const client{static_cast< FWClient * >(context)};
            switch (status) {
                case IPC_CLIENT_HANDLER_STATUS_CONNECTED: {
                    client->SetConnect(true);
                    break;
                }
                case IPC_CLIENT_HANDLER_STATUS_DISCONNECTED: {
                    client->SetConnect(false);
                    break;
                }
                default: {
                    break;
                }
            }
            return;
        }};
    clientPtr_->SetConnectionHandler(handler, this);
    LogInfo() << "fw client create ipc client=" << GetkFWIpcFile_Client() << " successful";
    return 0;
}
/// @brief
/// @param bConnect
void FWClient::SetConnect(bool const bConnect) noexcept
{
    bConnect_ = bConnect;
    ara::core::String errMsg{""};
    if (bConnect_ == true) {
        errMsg = "connect";
    } else {
        errMsg = "disconnect";
    }
    LogInfo() << "fw client is " << errMsg.c_str();
    return;
}

/// @brief client side switch firewall state
/// @param specifierId
/// @param fwStatus
/// @return
///@exception
ara::core::Result< void > FWClient::SwitchFirewallState(ara::core::InstanceSpecifier const &specifierId,
                                                        FWState fwStatus)
{
    Chunk const evMsg{ClientEventParse::Serialize(specifierId.ToString(), fwStatus)};
    return SendEventMsg(evMsg);
}

/// @brief
/// @param evMsg
/// @return
///@exception
ara::core::Result< void > FWClient::SendEventMsg(Chunk const &evMsg)
{
    size_t const len{evMsg.size()};
    ara::core::Result< void > result{};

    ::isoft::ipc::IPCPacket *const request{clientPtr_->MakeRequest()};
    ::isoft::ipc::IPCPacketBufferReference *const ipcBuffer{request->AppendBuffer(len)};
    uint8_t *const buf{ipcBuffer->GetPtr()};

    std::ignore = std::memcpy(buf, evMsg.data(), len);
    std::ignore = ipcBuffer->SetLen(len);

    ::isoft::ipc::IPCPacket *response{nullptr};
    // TODO(jzy) Optimize throughput based on business requirements later
    int32_t const ret{clientPtr_->SendSync(request, &response, 1000)};
    if (0 != ret) {
        LogError() << "SendEventMsg: fw client notify post errno:" << SysErr();
        result.EmplaceError(FwErrc::kServiceNotAvailable);
        std::ignore = ::isoft::ipc::IPCPacket::Release(response);
        return result;
    }
    if (nullptr == response) {
        LogError() << "SendEventMsg: get response failed!";
        result.EmplaceError(FwErrc::kServiceNotAvailable);
        std::ignore = ::isoft::ipc::IPCPacket::Release(response);
        return result;
    }
    ::isoft::ipc::IPCPacketBufferReference *const buffer{response->GetBuffer()};
    if (nullptr == buffer) {
        LogError() << "SendEventMsg: get response  buffer failed!";
        result.EmplaceError(FwErrc::kServiceNotAvailable);
        std::ignore = ::isoft::ipc::IPCPacket::Release(response);
        return result;
    }
    uint8_t *const respBuf{buffer->GetPtr()};
    FwErrc fwCode{FwErrc::kOk};
    std::ignore = nai_memcpy(&fwCode, respBuf, sizeof(FwErrc));

    // get  fw engine error code.
    if (fwCode != FwErrc::kOk) {
        LogError() << "SendEventMsg:get fwErrorCode from server! ";
        result.EmplaceError(fwCode);
        return result;
    }
    return ara::core::Result< void >::FromValue();
}

}  // namespace internal
}  // namespace fw
}  // namespace ara