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
/// @file       fw_manager.cpp
/// @brief      fw main processing class
/// @details    fw main processing class
/// @date       2024-12-16
/// @author     jzy
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/fw/Firewall Manager
/// @interface_level=module
/// @trace_id_sr=SR_FW_0001,SR_FW_0003
/// @unit_name=FW_Manager
/// @unit_description=Firewall management class
/// @endcode
///
/// ================================================================
///
///
/// ================================================================

#include "fw_manager.h"

#include <isoft/ipccpp/buffer.h>
#include <isoft/ipccpp/utility.h>

#include "ara/fw/common/common.h"
#include "ara/fw/filterengine/delete.h"
#include "ara/fw/internal/log_api.h"
#include "ara/fw/internal/msg_process.h"
#include "ara/fw/internal/serialization_error_domain.h"
#include "filterchain/fw_filter_chain.h"
namespace ara {
namespace fw {
namespace internal {

/// @brief FWManager constructor
FWManager::FWManager() noexcept
    : upFWManifestReader_(std::make_unique< ConfigManager >())
    , upExeClient_(std::make_unique< ara::exec::ExecutionClient >())
    , upFWFilterChain_()
    , upIpcServer_{std::make_unique< PIpcServer >(*this)}
{
}

/// @brief
bool FWManager::Init() noexcept
{
    // ipc init.
    if (!upIpcServer_->Init()) {
        LogError() << "FWManager:ipc init failed!";
        return false;
    }

    // manifest parse. If there is no firewall configuration, there is no need to
    // shut down fwdaemon.
    std::ignore = upFWManifestReader_->InitManifest();

    // set filterchain unique_ptr.
    upFWFilterChain_ = std::make_unique< FWFilterChain >(upFWManifestReader_);

    // notify exec fw daemon starting.
    ara::core::Result< void > exec = upExeClient_->ReportExecutionState(ara::exec::ExecutionState::kRunning);
    if (!exec.HasValue()) {
        LogError() << "FWManager::Init exec client report errmsg=" << exec.Error().Message();
        return false;
    }
    LogInfo() << "ReportExecutionState success!";

    return true;
}

bool FWManager::Run() noexcept
{
    if (upIpcServer_->Start()) {
        LogError() << "FWManager:ipc Run failed!";
        return false;
    }
    return true;
}

void FWManager::Stop() noexcept
{
    // IPC STOP
    upIpcServer_->Stop();
    upIpcServer_.reset();
    // EXEC STOP
    upExeClient_.reset();
    // MANIFEST STOP
    upFWManifestReader_.reset();
    // Business processing pointer stop.
    upFWFilterChain_.reset();

    return;
}

/// @brief Server-side processing of received data.
/// @param pReqPacket
/// @return
FwErrc FWManager::ReceiveDataProcess(isoft::ipc::IPCPacket *const pReqPacket) noexcept
{
    FwErrc fwErrorCodeNum{FwErrc::kOk};
    ::isoft::ipc::IPCPacketBufferReference *const ipcBuffer{pReqPacket->GetBuffer()};
    if (nullptr == ipcBuffer) {
        LogError() << "ReceiveDataProcess: event from AA. err: ipc read data fail";
        fwErrorCodeNum = FwErrc::kServiceNotAvailable;
        return fwErrorCodeNum;
    }
    // get data.
    ClientEventMsg eventMsg;
    size_t const msgSize{ipcBuffer->GetLen()};
    ClientEventParse::Deserialize(ipcBuffer->GetPtr(), msgSize, &eventMsg);
    LogDebug() << "Idsm receive event from AA. event: " << eventMsg.GetInstanceId().c_str()
               << " fwStatus:" << eventMsg.GetFwStatus();

    // fwmanager process  data。
    // return error code.
    fwErrorCodeNum = ProcessIpcMsg(eventMsg);
    return fwErrorCodeNum;
}

/// @brief
/// @param readIds
/// @param packet
/// @return
FwErrc FWManager::ProcessIpcMsg(ClientEventMsg const &eventMsg) noexcept
{
    FwErrc errorCode{FwErrc::kOk};
    /// @brief Descriptor obtained via IPC.
    ara::core::String instaceSpec{eventMsg.GetInstanceId()};
    /// @brief Firewall state obtained via IPC
    uint32_t fsState{eventMsg.GetFwStatus()};
    LogInfo() << "get InstanceName: " << instaceSpec.c_str();
    LogInfo() << "get fwStatus: " << fsState;

    /// Check if the instance's corresponding state is in the list.
    if (!upFWManifestReader_->QueryFWStateExist(instaceSpec, fsState)) {
        LogError() << "ProcessIpcMsg:query fw state  failed!";
        errorCode = FwErrc::kServiceNotAvailable;
        // ResponseProcessResult(errorCode, packet);
        return errorCode;
    }

    // Filter chain sets the queried instance and firewall state
    upFWFilterChain_->SetFWStateAndInstance(instaceSpec, fsState);

    // Filter chain gets RuleProps and defaultaction.
    if (!upFWFilterChain_->GetFWRulePropsAndDefaultAction()) {
        LogError() << "ProcessIpcMsg:GetFWRulePropsAndDefaultAction process failed!";
        errorCode = FwErrc::kServiceNotAvailable;
        // ResponseProcessResult(errorCode, packet);
        return errorCode;
    }

    // fw creates filter nft, filter tables and Input/Output chains.
    if (!upFWFilterChain_->CreateTableAndChain()) {
        LogError() << "ProcessIpcMsg:CreateTableAndChain process failed!";
        errorCode = FwErrc::kServiceNotAvailable;
        // Delete created table.
        FilterTableDelete();
        return errorCode;
    }

    // Perform actual filtering
    if (!upFWFilterChain_->AllRulesCheck()) {
        LogError() << "ProcessIpcMsg:AllRulesCheck process failed!";
        errorCode = FwErrc::kServiceNotAvailable;
        // Delete created table.
        FilterTableDelete();
        return errorCode;
    }

    // Processing completed, return normal processing
    // ResponseProcessResult(errorCode, packet);
    return errorCode;
}

/// @brief
/// @param fwErrorCode
void FWManager::ResponseProcessResult(FwErrc const &fwErrorCode, ::isoft::ipc::IPCPacket *packet) noexcept
{
    ::isoft::ipc::IPCSessionId const sessionId{packet->GetSessionId()};
    ::isoft::ipc::IPCPacket *const responsePacket{upIpcServer_->NewIpcPacket(sessionId)};
    uint16_t const responseSize{5U};
    ::isoft::ipc::IPCPacketBufferReference *const buffer{responsePacket->AppendBuffer(responseSize)};
    uint8_t *const buf{buffer->GetPtr()};
    uint32_t const fwRetValue{static_cast< uint32_t >(fwErrorCode)};
    LogError() << "ResponseProcessResult:send error code is " << fwRetValue;
    std::ignore = nai_memcpy(buf, &fwRetValue, sizeof(fwRetValue));
    std::ignore = buffer->SetLen(sizeof(fwRetValue));
    std::ignore = upIpcServer_->SendIpcPacket(responsePacket, true);
    return;
}

}  // namespace internal
}  // namespace fw
}  // namespace ara
