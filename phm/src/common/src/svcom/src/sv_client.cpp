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
/// @file       sv_client.cpp
/// @brief
/// @details
/// @date       2024-06-20
/// @author     wangyanlong
/// @version    1.2.0
///
/// ================================================================

#include "ara/phm/internal/com/sv_client.h"

#include <isoft/ipccpp/buffer.h>
#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <unistd.h>

#include <fstream>

#include "ara/phm/internal/phm_log.h"
#include "ara/phm/internal/types.h"

namespace ara {
namespace phm {
namespace internal {
namespace svcom {
/// @brief Report checkpoint to SupervisionManager.
/// @param specifierId instance specifier of supervised entity.
/// @param checkpointId checkpoint id.
/// @return empty sucess, otherwise error.
int32_t Client::ReportCheckpoint(Specifier const &specifierId, uint32_t const checkpointId) noexcept
{
    Message const msg{EventType::kReportCheckpoint, specifierId,       checkpointId, GetPid(),
                      processClusterAffiliation_,   TimeStamp::GetMs()};
    messageSerializer_.SetMsg(msg);
    Chunk const chunk{messageSerializer_.Serialize()};
    if (chunk.size() == static_cast< size_t >(0)) {
        LOG_ERROR << "serialize error.";
        return -1;
    }

    isoft::ipc::IPCPacket *const request{_MakeAndFillRequest(chunk)};
    if (request == nullptr) {
        LOG_ERROR << "_MakeAndFillRequest error.";
        return -1;
    }
    std::shared_ptr< isoft::ipc::IPCClient > ipcClient{GetIpcClient()};
    int32_t const ret{ipcClient->Post(request)};
    if (0 != ret) {
        LOG_ERROR << "Post error.";
        return -1;
    }
    return 0;
}

/// @brief Returns status of LocalSupervision.
/// @param specifierId instance specifier of supervised entity.
/// @return The local supervision status.
SupervisionStatus Client::GetLocalSupervisionStatus(Specifier const &specifierId) noexcept
{
    Message const msg{EventType::kGetLocalSupervisionStatus,
                      specifierId,
                      0U,
                      GetPid(),
                      processClusterAffiliation_,
                      TimeStamp::GetMs()};
    messageSerializer_.SetMsg(msg);
    Chunk const chunk{messageSerializer_.Serialize()};
    if (chunk.size() == static_cast< size_t >(0)) {
        LOG_ERROR << "serialize error.";
        return kInvalidSupervisionStatus;
    }

    isoft::ipc::IPCPacket *const request{_MakeAndFillRequest(chunk)};
    if (request == nullptr) {
        LOG_ERROR << "_MakeAndFillRequest error.";
        return kInvalidSupervisionStatus;
    }
    isoft::ipc::IPCPacket *response{nullptr};
    std::shared_ptr< isoft::ipc::IPCClient > ipcClient{GetIpcClient()};
    int32_t const ret{ipcClient->SendSync(request, &response, 2000)};
    if (ret < 0) {
        LOG_ERROR << "send sync error " << ret;
        return kInvalidSupervisionStatus;
    }

    isoft::ipc::IPCPacketBufferReference *const ipcBuffer{response->GetBuffer()};
    if (ipcBuffer == nullptr) {
        LOG_ERROR << "ipcBuffer is null.";
        return kInvalidSupervisionStatus;
    }
    SupervisionStatus const status{*reinterpret_cast< SupervisionStatus * >(ipcBuffer->GetPtr())};
    response    = isoft::ipc::IPCPacket::Release(response);
    std::ignore = response;
    return status;
}

/// @brief Returns status of GlobalSupervision.
/// @param specifierId instance specifier of supervised entity.
/// @return The global supervision status.
SupervisionStatus Client::GetGlobalSupervisionStatus(Specifier const &specifierId) noexcept
{
    Message const msg{EventType::kGetGlobalSupervisionStatus,
                      specifierId,
                      0U,
                      GetPid(),
                      processClusterAffiliation_,
                      TimeStamp::GetMs()};
    messageSerializer_.SetMsg(msg);
    Chunk const chunk{messageSerializer_.Serialize()};
    if (chunk.size() == static_cast< size_t >(0)) {
        LOG_ERROR << "serialize error.";
        return kInvalidSupervisionStatus;
    }

    isoft::ipc::IPCPacket *const request{_MakeAndFillRequest(chunk)};
    if (request == nullptr) {
        LOG_ERROR << "_MakeAndFillRequest error.";
        return kInvalidSupervisionStatus;
    }

    isoft::ipc::IPCPacket *response{nullptr};
    std::shared_ptr< isoft::ipc::IPCClient > ipcClient{GetIpcClient()};
    int32_t const ret{ipcClient->SendSync(request, &response, 2000)};
    if (ret < 0) {
        LOG_ERROR << "send sync error " << ret;
        return kInvalidSupervisionStatus;
    }

    isoft::ipc::IPCPacketBufferReference *const ipcBuffer{response->GetBuffer()};
    if (ipcBuffer == nullptr) {
        LOG_ERROR << "ipcBuffer is null.";
        return kInvalidSupervisionStatus;
    }
    SupervisionStatus const status{*reinterpret_cast< SupervisionStatus * >(ipcBuffer->GetPtr())};
    response    = isoft::ipc::IPCPacket::Release(response);
    std::ignore = response;
    return status;
}

/// @brief get process cluster affiliation.
/// @return process cluster affiliation.
ProcessClusterAffiliation Client::GetProcessClusterAffiliation() noexcept
{
    isoft::ara_fsh::Process const fsh{};
    ara::core::String const execManifestPath{fsh.GetExecutionManifest()};
    bool const manifestNotExist{access(const_cast< char * >(execManifestPath.c_str()), F_OK) != 0};
    if ((true == execManifestPath.empty()) || manifestNotExist) {
        if (IsExecutionManagement()) {
            std::cout << "process cluster affiliation is EM" << std::endl;
            return ProcessClusterAffiliation::kExecutionManagement;
        }
        std::cout << "process cluster affiliation is not EM nor SM" << std::endl;
        return ProcessClusterAffiliation::kOther;
    }
    if (IsStateManagement(execManifestPath)) {
        std::cout << "process cluster affiliation is SM" << std::endl;
        return ProcessClusterAffiliation::kStateManagement;
    }
    std::cout << "process cluster affiliation is not EM nor SM" << std::endl;
    return ProcessClusterAffiliation::kOther;
}

/// @brief if process is SM.
/// @param manifest execution manifest.
/// @return true, process is SM; false not SM.
bool Client::IsStateManagement(ara::core::String const &manifest) noexcept
{
    std::ifstream fileStream{manifest.c_str()};
    rapidjson::IStreamWrapper jsonStream{fileStream};
    rapidjson::Document handler;
    std::ignore = handler.ParseStream(jsonStream);
    if (handler.HasParseError()) {
        std::cout << "parse manifest " << manifest << " error" << std::endl;
        return false;
    }

    if (handler.HasMember(GetProcessClusterAffiliationConfigField().c_str())) {
        ara::core::String const affiliation{handler[GetProcessClusterAffiliationConfigField().c_str()].GetString()};
        std::cout << "process cluster affiliation is " << affiliation << std::endl;
        if (affiliation == GetSmClusterAffiliation()) {
            return true;
        }
    }
    return false;
}

/// @brief if process is EM.
/// @return true, process is EM; false, not EM.
bool Client::IsExecutionManagement() noexcept
{
    isoft::ara_fsh::Process const fsh{};
    std::cout << "process exec name is " << fsh.GetBinName() << std::endl;
    return fsh.GetBinName() == fsh.GetEmBinName();
}

}  // namespace svcom
}  // namespace internal
}  // namespace phm
}  // namespace ara
