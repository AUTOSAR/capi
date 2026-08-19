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
/// @file       nm_comm_manager.cpp
/// @brief      Management class for communication with Network Management, implementing network state switching and network state notification
/// @details
/// @date       2024-05-03
/// @author     lianglongxiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/StateManagement/NMComm
/// @interface_level=module
/// @trace_id_sr=SR_SM_05003, SR_SM_05004
/// @unit_name=NMCommManager
/// @unit_description=Management class for communication with Network Management, implementing network state switching and network state notification
/// @endcode
///
/// ================================================================

#include "nm_comm_manager.h"

#include <ara/sm/error_domain_sm.h>
namespace ara {
namespace sm {
namespace nm_comm {

/// @brief Constructor function
/// @param name CommManager name
NMCommManager::NMCommManager(core::String const &name) noexcept
    : ICommunicationManager{name}, globalConfigInstance_{nullptr}, nmServers_{}
{
    log_.LogInfo() << "NMCommManager::NMCommManager()";
}

/// @brief Destructor function
NMCommManager::~NMCommManager() noexcept { log_.LogInfo() << "NMCommManager::~NMCommManager()"; }

/// @brief Initialize
/// @return true - success
/// @return false - failure
bool NMCommManager::Init() noexcept
{
    log_.LogInfo() << "NMCommManager::Init()";
    if (globalConfigInstance_ == nullptr) {
        log_.LogWarn() << "NMCommManager::Init(), globalConfigInstance_ is nullptr";
        return false;
    }
    ara::core::Vector< ara::sm::config::NMServiceInfo > nmServiceInfoList{
        globalConfigInstance_->GetAllNMServiceInfoList()};
    if (nmServiceInfoList.empty()) {
        log_.LogWarn() << "NMCommManager::Init(), nmServiceInfoList is empty";
        return true;
    }
    nmServers_.clear();
    log_.LogInfo() << "NMCommManager::Init(), try to create nmServers_";
    for (const auto &nmServiceInfo : nmServiceInfoList) {
        std::unique_ptr< NMNetworkStateControlServer > server;
        server.reset(new (std::nothrow)
                         NMNetworkStateControlServer(nmServiceInfo.nmNetworkHandle, nmServiceInfo.nmNetworkInstanceID));
        if (server == nullptr) {
            log_.LogWarn() << "NMCommManager::Init(), NMNetworkStateControlServer is nullptr for nmNetworkHandle:"
                           << nmServiceInfo.nmNetworkHandle.c_str();
            return false;
        }
        nmServers_[nmServiceInfo.nmNetworkHandle] = std::move(server);
    }
    return true;
}

/// @brief Start accepting requests
/// @return true - success
/// @return false - failure
bool NMCommManager::Start() noexcept
{
    log_.LogInfo() << "NMCommManager::Start()";
    if (!started_) {
        if (nmServers_.empty()) {
            log_.LogWarn() << "NMCommManager::Start(), nmServers_ is empty";
            started_ = true;
            return true;
        }
        for (const auto &pair : nmServers_) {
            if (!pair.second->Start()) {
                log_.LogWarn() << "NMCommManager::Start(), server Start err for nmNetworkHandle:" << pair.first.c_str();
                return false;
            }
        }
        started_ = true;
    }
    return true;
}

/// @brief Stop accepting requests
void NMCommManager::Stop() noexcept
{
    log_.LogInfo() << "NMCommManager::Stop()";
    for (const auto &pair : nmServers_) {
        pair.second->Stop();
    }
}

/// @brief Register the callback function for appending events
/// @param appendEventHandler Callback function for appending events
void NMCommManager::RegisterAppendEventHandler(std::function< void(common::Event &&) > appendEventHandler) noexcept
{
    log_.LogInfo() << "NMCommManager::RegisterAppendEventHandler()";
    for (auto &pair : nmServers_) {
        pair.second->RegisterAppendEventHandler(appendEventHandler);
    }
}

/// @brief Asynchronously enable or disable the network
/// @param nmNetworkHandle Network name
/// @param requestedState Network state
core::Future< void > NMCommManager::AsyncSetNetworkState(
    core::String const &nmNetworkHandle, common::NetworkStateInternalType const &requestedState) const noexcept
{
    log_.LogInfo() << "NMCommManager::AsyncSetNetworkState(), nmNetworkHandle:" << nmNetworkHandle.c_str()
                   << "requestedState:" << static_cast< int32_t >(requestedState);
    core::Map< core::String, std::unique_ptr< NMNetworkStateControlServer > >::const_iterator const serverIt{
        nmServers_.find(nmNetworkHandle)};
    if (nmServers_.end() != serverIt) {
        return serverIt->second->AsyncSetNetworkRequestedState(requestedState);
    }
    log_.LogWarn() << "NMCommManager::AsyncSetNetworkState(), there is no server for nmNetworkHandle:"
                   << nmNetworkHandle.c_str();
    core::Promise< void > promise;
    promise.SetError(SMErrc::kRejected);
    return promise.get_future();
}

/// @brief Set the global configuration instance
/// @param globalConfigInstance Global configuration instance
void NMCommManager::SetGlobalConfigInstance(
    std::shared_ptr< config::GlobalConfig > const &globalConfigInstance) noexcept
{
    log_.LogInfo() << "NMCommManager::SetGlobalConfigInstance()";
    globalConfigInstance_ = globalConfigInstance;
}

}  // namespace nm_comm
}  // namespace sm
}  // namespace ara