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
/// @file       sm_state_comm_manager.cpp
/// @brief      Management class for communication related to state machine states
/// @details
/// @date       2024-05-07
/// @author     lianglongxiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/StateManagement/SMStateComm
/// @interface_level=module
/// @trace_id_sr=SR_SM_09001, SR_SM_09008
/// @unit_name=SMStateCommManager
/// @unit_description=Management class for communication related to state machine states
/// @endcode
///
/// ================================================================

#include "sm_state_comm_manager.h"
namespace ara {
namespace sm {
namespace sm_state_comm {

/// @brief Constructor function
/// @param name CommManager name
SMStateCommManager::SMStateCommManager(core::String const &name) noexcept
    : ICommunicationManager{name}, globalConfigInstance_{}, smStateServers_{}
{
    log_.LogInfo() << "SMStateCommManager::SMStateCommManager()";
}

/// @brief Destructor function
SMStateCommManager::~SMStateCommManager() noexcept
{
    log_.LogInfo() << "SMStateCommManager::~SMStateCommManager()";
    SMStateCommManager::Stop();
}

/// @brief Initialize
/// @return true - success
/// @return false - failure
bool SMStateCommManager::Init() noexcept
{
    log_.LogInfo() << "SMStateCommManager::Init()";
    if (globalConfigInstance_ == nullptr) {
        log_.LogWarn() << "SMStateCommManager::Init(), globalConfigInstance_ is nullptr";
        return false;
    }
    ara::core::Vector< ara::sm::config::SMServiceInfo > smServiceInfoList{
        globalConfigInstance_->GetAllSMServiceInfoList()};
    if (smServiceInfoList.empty()) {
        log_.LogWarn() << "SMStateCommManager::Init(), smServiceInfoList is empty";
    }
    smStateServers_.clear();
    log_.LogDebug() << "SMStateCommManager::Init(), try to create smStateServers_";
    for (const auto &smServiceInfo : smServiceInfoList) {
        std::unique_ptr< ara::sm::sm_state_comm::SMStateServer > server{
            std::make_unique< SMStateServer >(smServiceInfo)};
        if (server == nullptr) {
            log_.LogWarn() << "SMStateCommManager::Init(), SMStateServer is nullptr for smFQN:"
                           << smServiceInfo.smFQN.c_str();
            return false;
        }
        std::ignore                          = server->Init();
        smStateServers_[smServiceInfo.smFQN] = std::move(server);
    }
    return true;
}

/// @brief Start accepting requests
/// @return true - success
/// @return false - failure
bool SMStateCommManager::Start() noexcept
{
    log_.LogInfo() << "SMStateCommManager::Start()";
    if (smStateServers_.empty()) {
        log_.LogWarn() << "SMStateCommManager::Start(), smStateServers_ is empty";
    }
    for (const auto &pair : smStateServers_) {
        if (!pair.second->Start()) {
            log_.LogWarn() << "SMStateCommManager::Start(), server Start errr for smFQN:" << pair.first.c_str();
            return false;
        }
    }
    return true;
}

/// @brief Stop accepting requests
void SMStateCommManager::Stop() noexcept
{
    log_.LogInfo() << "SMStateCommManager::Stop()";
    for (const auto &pair : smStateServers_) {
        pair.second->Stop();
    }
}

/// @brief Register the callback function for appending events
/// @param appendEventHandler Callback function for appending events
void SMStateCommManager::RegisterAppendEventHandler(std::function< void(common::Event &&) > appendEventHandler) noexcept
{
    log_.LogInfo() << "SMStateCommManager::RegisterAppendEventHandler()";
    for (const auto &pair : smStateServers_) {
        pair.second->RegisterAppendEventHandler(appendEventHandler);
    }
}

/// @brief Notify function group state update
/// @param smFQN Function group name
/// @param smState Function group state
void SMStateCommManager::NotifySMStateUpdate(core::String const &smFQN, core::String const &smState) const noexcept
{
    log_.LogInfo() << "SMStateCommManager::NotifySMStateUpdate()";
    core::Map< core::String, std::unique_ptr< SMStateServer > >::const_iterator const serverIt{
        smStateServers_.find(smFQN)};
    if (smStateServers_.end() == serverIt) {
        log_.LogWarn() << "SMStateCommManager::NotifySMStateUpdate(), invliad smFQN:" << smFQN.c_str();
    } else {
        serverIt->second->NotifySMStateUpdate(smState);
    }
}

/// @brief Set the global configuration instance
/// @param globalConfigInstance Global configuration instance
void SMStateCommManager::SetGlobalConfigInstance(
    std::shared_ptr< config::GlobalConfig > const &globalConfigInstance) noexcept
{
    log_.LogInfo() << "SMStateCommManager::SetGlobalConfigInstance()";
    globalConfigInstance_ = globalConfigInstance;
}

}  // namespace sm_state_comm
}  // namespace sm
}  // namespace ara