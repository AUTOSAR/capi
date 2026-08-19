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
/// @file       idsc_client.cpp
/// @brief      =
/// @details
/// @date       2022-12-27
/// @author     niuliming
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/idsm/lib client
/// @interface_level=unit
/// @trace_id_sr=SRS_IDSM_0005
/// @unit_name=IdsmClient
/// @unit_description=
/// @endcode
///
/// ================================================================

#include "idsc_client.h"

#include <isoft/ara_fsh/filesystem_hierarchy.h>
#include <isoft/ipccpp/buffer.h>
#include <isoft/ipccpp/client.h>
#include <isoft/ipccpp/packet.h>
#include <isoft/ipccpp/utility.h>
#include <isoft/naicpp/global_evloop.h>
#include <nai/os/nai_proc.h>

#include <cstring>

#include "ara/idsm/internal/common.h"
#include "ara/idsm/internal/idsm_error_domain.h"
#include "ara/idsm/internal/message_process.h"
#include "idsc_log.h"

namespace ara {
namespace idsm {

using ::ara::core::InstanceSpecifier;
using ::isoft::naicpp::EvLoop;
using ::isoft::naicpp::GlobalGeneralEvLoop;
/// @brief
std::unique_ptr< IdsmClient > IdsmClient::s_Instance_{nullptr};  // NOLINT
/// @brief
std::mutex IdsmClient::s_Mtx_{};
/// @brief
/// @return
/// @throw
IdsmClient* IdsmClient::GetInstance()
{
    printf("IdsmClient::instance_ pointer:%p\n", static_cast< void* >(s_Instance_.get()));
    if (s_Instance_ == nullptr) {
        std::lock_guard< std::mutex > const lock{s_Mtx_};
        if (s_Instance_ == nullptr) {
            //instance_.reset(new IdsmClient);
            s_Instance_ = std::make_unique< IdsmClient >();
            if (s_Instance_->Init() != 0) {
                printf("IdsmClient::instance_ init fail.\n");
                s_Instance_.reset();
            }
        }
    }
    return s_Instance_.get();
}
/// @brief
/// @throw
void IdsmClient::Destroy()
{
    // int64_t const res{clientPtr_->Stop()};
    // if (res != 0) {
    //     IDSC_LOG_ERROR << "idsc ipc client stop err.";
    //     return;
    // }
    clientPtr_->Release();
    std::ignore = isoft::ipc::IPCDeInitNaiUDS();
    s_Instance_.reset(nullptr);
}

// IdsmClient::~IdsmClient() { Destroy(); }
/// @brief
/// @return
/// @throw
int32_t IdsmClient::Init()
{
    /// @brief Get process ID
    processId_ = static_cast< uint32_t >(nai_get_pid());

    /// Initialize IPC client
    evLoopPtr_ = GlobalGeneralEvLoop::Get();
    if (evLoopPtr_ == nullptr) {
        IDSC_LOG_ERROR << "idsm client: Get EvLoop fail. errno: " << SysErr();
        return -1;
    }

    int32_t const retCode{::isoft::ipc::IPCInitNaiUDS(evLoopPtr_)};
    if (retCode == -1) {
        IDSC_LOG_ERROR << "idsm client: call IPCInitNaiUDS func fail.";
        std::ignore = isoft::ipc::IPCDeInitNaiUDS();
        return -1;
    }

    ara::core::StringView const ipcIdsmClient{"idsmd/qsev"};
    clientPtr_ = isoft::ipc::IPCClient::Create(ipcIdsmClient.data());
    if (nullptr == clientPtr_.get()) {
        IDSC_LOG_ERROR << "idsm client: create ipc client fail. errno: " << SysErr();
        return -1;
    }
    isoft::ipc::IPCClientConnectionHandler const handler{
        [](void* const context, ::isoft::ipc::IPCClientConnectionStatus const status) noexcept {
            IdsmClient* const client{static_cast< IdsmClient* >(context)};
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
        }};
    clientPtr_->SetConnectionHandler(handler, this);
    IDSC_LOG_INFO << "idsm client create ipc client=" << ipcIdsmClient.data() << " successful";
    return 0;
}
/// @brief
/// @param bConnect
void IdsmClient::SetConnect(bool const bConnect) noexcept
{
    bConnect_ = bConnect;
    ara::core::String errMsg{""};
    if (bConnect_ == true) {
        errMsg = "connect";
    } else {
        errMsg = "disconnect";
    }
    IDSC_LOG_INFO << "idsm client is " << errMsg.c_str();
}
/// @brief
/// @param instanceId
/// @param count
/// @return
/// @throw
bool IdsmClient::Report(ara::core::InstanceSpecifier const& instanceId, CountType const count)
{
    TimestampSource source{TimestampSource::kUnknown};
    TimestampType timestamp{0U};
    if (timeCB_ != nullptr) {
        source    = TimestampSource::kAppCallback;
        timestamp = timeCB_();
    }
    Chunk const evMsg{ClientEventParse::Serialize(instanceId.ToString(), processId_, timestamp, source, count)};

    return _sendEventMsg(evMsg);
}
/// @brief
/// @param instanceId
/// @param contextData
/// @param count
/// @return
/// @throw
bool IdsmClient::Report(ara::core::InstanceSpecifier const& instanceId,
                        ContextDataType const& contextData,
                        CountType const count)
{
    TimestampSource source{TimestampSource::kUnknown};
    TimestampType timestamp{0U};
    if (timeCB_ != nullptr) {
        timestamp = timeCB_();
        source    = TimestampSource::kAppCallback;
    }
    Chunk const evMsg{
        ClientEventParse::Serialize(instanceId.ToString(), processId_, contextData, timestamp, source, count)};
    return _sendEventMsg(evMsg);
}
/// @brief
/// @param instanceId
/// @param timestamp
/// @param count
/// @return
/// @throw
bool IdsmClient::Report(ara::core::InstanceSpecifier const& instanceId,
                        TimestampType const timestamp,
                        CountType const count)
{
    Chunk const evMsg{
        ClientEventParse::Serialize(instanceId.ToString(), processId_, timestamp, TimestampSource::kAppReport, count)};
    return _sendEventMsg(evMsg);
}
/// @brief
/// @param instanceId
/// @param contextData
/// @param timestamp
/// @param count
/// @return
/// @throw
bool IdsmClient::Report(ara::core::InstanceSpecifier const& instanceId,
                        ContextDataType const& contextData,
                        TimestampType const timestamp,
                        CountType const count)
{
    Chunk const evMsg{ClientEventParse::Serialize(instanceId.ToString(), processId_, contextData, timestamp,
                                                  TimestampSource::kAppReport, count)};
    return _sendEventMsg(evMsg);
}
/// @brief
/// @param evMsg
/// @return
/// @throw
bool IdsmClient::_sendEventMsg(Chunk const& evMsg)
{
    size_t const len{evMsg.size()};

    ::isoft::ipc::IPCPacket* const request{clientPtr_->MakeRequest()};
    ::isoft::ipc::IPCPacketBufferReference* const ipcBuffer{request->AppendBuffer(len)};
    uint8_t* const buf{ipcBuffer->GetPtr()};

    std::ignore = std::memcpy(buf, evMsg.data(), len);
    std::ignore = ipcBuffer->SetLen(len);

    ::isoft::ipc::IPCPacket* response{nullptr};
    // TODO(Niu Liming): Optimize throughput later based on business requirements
    int32_t const ret{clientPtr_->SendSync(request, &response, 1000)};
    if (0 != ret) {
        IDSC_LOG_ERROR << "idsm client notify post errno:" << SysErr();
        return false;
    }
    if (nullptr == response) {
        return false;
    }
    ::isoft::ipc::IPCPacketBufferReference* const buffer{response->GetBuffer()};
    if (nullptr == buffer) {
        return false;
    }
    uint8_t* const respBuf{buffer->GetPtr()};
    if (*(static_cast< uint32_t* >(static_cast< void* >(respBuf))) == 0U) {
        IDSC_LOG_INFO << "idsm client send event to idsm success";
    } else {
        IDSC_LOG_ERROR << "idsm client send event to idsm fail";
    }
    std::ignore = ::isoft::ipc::IPCPacket::Release(response);
    return true;
}

}  // namespace idsm
}  // namespace ara