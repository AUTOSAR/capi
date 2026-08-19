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
/// @file       fg_state_notify_comm_manager.cpp
/// @brief      Management class for communication with PHM and IDSM, implementing function group state notification
/// @details
/// @date       2024-05-03
/// @author     lianglongxiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/StateManagement/FGStateNotifyComm
/// @interface_level=module
/// @trace_id_sr=SR_SM_02001, SR_SM_02002
/// @unit_name=FGStateNotifyCommManager
/// @unit_description=Management class for communication with PHM and IDSM, implementing function group state notification
/// @endcode
///
/// ================================================================

#include "fg_state_notify_comm_manager.h"

#include "define.h"

namespace ara {
namespace sm {
namespace fg_state_notify_comm {

/// @brief Constructor function
/// @param name CommManager name
FGStateNotifyCommManager::FGStateNotifyCommManager(core::String const &name) noexcept : ICommunicationManager{name}
{
    log_.LogInfo() << "FGStateNotifyCommManager::FGStateNotifyCommManager()";
}

/// @brief Destructor function
FGStateNotifyCommManager::~FGStateNotifyCommManager() noexcept
{
    log_.LogInfo() << "FGStateNotifyCommManager::~FGStateNotifyCommManager()";
}

/// @brief Initialize
/// @return true - success
/// @return false - failure
bool FGStateNotifyCommManager::Init() noexcept
{
    log_.LogInfo() << "FGStateNotifyCommManager::Init()";
    if (serverInstance_ == nullptr) {
        core::StringView kFGStateNotifyServerIdentifier{common::GetkFGStateNotifyServerIdentifier()};
        serverInstance_ = std::make_unique< fg_state_ipc::Server >(
            core::InstanceSpecifier(std::move(kFGStateNotifyServerIdentifier)));
        std::ignore = serverInstance_->Init();
    }
    return true;
}

/// @brief Start accepting requests
/// @return true - success
/// @return false - failure
bool FGStateNotifyCommManager::Start() noexcept
{
    log_.LogInfo() << "FGStateNotifyCommManager::Start()";
    if (!started_) {
        log_.LogDebug() << "FGStateNotifyCommManager::Start(), try to OfferService.";
        if (serverInstance_) {
            bool const result{serverInstance_->Start()};
            if (!result) {
                log_.LogError() << "FGStateNotifyCommManager::Start(), serverInstance_ failed to Start";
                return false;
            }
        } else {
            log_.LogError() << "FGStateNotifyCommManager::Start(), serverInstance_ is nullptr";
            return false;
        }
        started_ = true;
    }
    return true;
}

/// @brief Stop accepting requests
void FGStateNotifyCommManager::Stop() noexcept
{
    log_.LogInfo() << "FGStateNotifyCommManager::Stop()";
    if (serverInstance_) {
        serverInstance_->Stop();
        std::ignore = serverInstance_->Destroy();
    } else {
        log_.LogError() << "FGStateNotifyCommManager::Stop(), serverInstance_ is nullptr";
    }
}

/// @brief Register the callback function for appending events
/// @param appendEventHandler Callback function for appending events
void FGStateNotifyCommManager::RegisterAppendEventHandler(
    std::function< void(common::Event &&) > appendEventHandler) noexcept
{
    log_.LogInfo() << "FGStateNotifyCommManager::RegisterAppendEventHandler()";
    if (serverInstance_) {
        serverInstance_->RegisterAppendEventHandler(appendEventHandler);
    } else {
        log_.LogError() << "FGStateNotifyCommManager::RegisterAppendEventHandler(), serverInstance_ is nullptr";
    }
}

/// @brief Notify function group state update
/// @param fgFQN Function group name
/// @param fgState Function group state
void FGStateNotifyCommManager::NotifyFGStateUpdate(core::String const &fgFQN,
                                                   core::String const &fgState) const noexcept
{
    log_.LogInfo() << "FGStateNotifyCommManager::NotifyFGStateUpdate()";
    if (serverInstance_) {
        fg_state_ipc::FGStateInternalType fgStateInternal;
        fgStateInternal.fgName  = fgFQN;
        fgStateInternal.fgState = fgState;
        serverInstance_->PublishFGState(std::move(fgStateInternal));
    } else {
        log_.LogError() << "FGStateNotifyCommManager::NotifyFGStateUpdate(), serverInstance_ is nullptr";
    }
}

}  // namespace fg_state_notify_comm
}  // namespace sm
}  // namespace ara