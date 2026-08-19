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
/// @file       fg_state_comm_manager.cpp
/// @brief      Management class for function group state communication
/// @details
/// @date       2024-05-03
/// @author     lianglongxiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/StateManagement/FGStateComm
/// @interface_level=module
/// @trace_id_sr=SR_SM_01001, SR_SM_01002, SR_SM_01003
/// @unit_name=FGStateCommManager
/// @unit_description=Management class for function group state communication
/// @endcode
///
/// ================================================================

#include "fg_state_comm_manager.h"
namespace ara {
namespace sm {
namespace fg_state_comm {

/// @brief Constructor function
/// @param name CommManager name
FGStateCommManager::FGStateCommManager(core::String const &name) noexcept
    : ICommunicationManager{name}
    , globalConfigInstance_{}
    , triggerFGStateServers_{}
    , started_{false}
    , log_{log::CreateLogger((core::StringView{"#CMG"}), (core::StringView{"Communication Manager context"}))}
{
    log_.LogInfo() << "FGStateCommManager::FGStateCommManager()";
}

/// @brief Destructor function
FGStateCommManager::~FGStateCommManager() noexcept
{
    log_.LogInfo() << "FGStateCommManager::~FGStateCommManager()";
    FGStateCommManager::Stop();
}

/// @brief Initialize
/// @return true - success
/// @return false - failure
bool FGStateCommManager::Init() noexcept
{
    log_.LogInfo() << "FGStateCommManager::Init()";
    if (globalConfigInstance_ == nullptr) {
        log_.LogWarn() << "FGStateCommManager::Init(), globalConfigInstance_ is nullptr";
        return false;
    }
    ara::core::Vector< ara::sm::config::FGServiceInfo > fgServiceInfoList{
        globalConfigInstance_->GetAllFGServiceInfoList()};
    if (fgServiceInfoList.empty()) {
        log_.LogWarn() << "FGStateCommManager::Init(), fgServiceInfoList is empty";
        return true;
    }
    triggerFGStateServers_.clear();
    log_.LogDebug() << "FGStateCommManager::Init(), try to create triggerFGStateServers_";
    for (const auto &fgServiceInfo : fgServiceInfoList) {
        std::unique_ptr< TriggerFGStateServer > server;
        server.reset(new (std::nothrow) TriggerFGStateServer(fgServiceInfo));
        if (server == nullptr) {
            log_.LogWarn() << "FGStateCommManager::Init(), TriggerFGStateServer is nullptr for fgFQN:"
                           << fgServiceInfo.fgFQN.c_str();
            return false;
        }
        std::ignore                                 = server->Init();
        triggerFGStateServers_[fgServiceInfo.fgFQN] = std::move(server);
    }
    return true;
}

/// @brief Start accepting requests
/// @return true - success
/// @return false - failure
bool FGStateCommManager::Start() noexcept
{
    log_.LogInfo() << "FGStateCommManager::Start()";
    if (!started_) {
        if (triggerFGStateServers_.empty()) {
            log_.LogWarn() << "FGStateCommManager::Start(), triggerFGStateServers_ is empty";
            started_ = true;
            return true;
        }
        for (const auto &pair : triggerFGStateServers_) {
            if (!pair.second->Start()) {
                log_.LogWarn() << "FGStateCommManager::Start(), server Start err for fgFQN:" << pair.first.c_str();
                return false;
            }
        }
        started_ = true;
    }
    return true;
}

/// @brief Stop accepting requests
void FGStateCommManager::Stop() noexcept
{
    log_.LogInfo() << "FGStateCommManager::Stop()";
    for (const auto &pair : triggerFGStateServers_) {
        pair.second->Stop();
    }
}

/// @brief Register the callback function for appending events
/// @param appendEventHandler Callback function for appending events
void FGStateCommManager::RegisterAppendEventHandler(std::function< void(common::Event &&) > appendEventHandler) noexcept
{
    log_.LogInfo() << "FGStateCommManager::RegisterAppendEventHandler()";
    for (const auto &pair : triggerFGStateServers_) {
        pair.second->RegisterAppendEventHandler(appendEventHandler);
    }
}

/// @brief Notify function group state update
/// @param fgFQN Function group name
/// @param fgState Function group state
void FGStateCommManager::NotifyFGStateUpdate(core::String const &fgFQN, core::String const &fgState) const noexcept
{
    log_.LogInfo() << "FGStateCommManager::NotifyFGStateUpdate()";
    core::Map< core::String, std::unique_ptr< TriggerFGStateServer > >::const_iterator const serverIt{
        triggerFGStateServers_.find(fgFQN)};
    if (triggerFGStateServers_.end() == serverIt) {
        log_.LogWarn() << "FGStateCommManager::NotifyFGStateUpdate(), invliad fgFQN:" << fgFQN.c_str();
    } else {
        serverIt->second->NotifyFGStateUpdate(fgState);
    }
}

/// @brief Set the global configuration instance
/// @param globalConfigInstance Global configuration instance
void FGStateCommManager::SetGlobalConfigInstance(
    std::shared_ptr< config::GlobalConfig > const &globalConfigInstance) noexcept
{
    log_.LogInfo() << "FGStateCommManager::SetGlobalConfigInstance()";
    globalConfigInstance_ = globalConfigInstance;
}

}  // namespace fg_state_comm
}  // namespace sm
}  // namespace ara