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
/// @file       shell_comm_manager.cpp
/// @brief      The class for managing communication with ShellApp for changing/getting function group state and state machine state
/// @details
/// @date       2024-05-07
/// @author     lianglongxiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/StateManagement/ShellComm
/// @interface_level=module
/// @trace_id_sr=SR_SM_10001
/// @unit_name=ShellCommManager
/// @unit_description=The class for managing communication with ShellApp for changing/getting function group state and state machine state
/// @endcode
///
/// ================================================================

#include "shell_comm_manager.h"

namespace ara {
namespace sm {
namespace shell_comm {

/// @brief Constructor function
/// @param name CommManager name
ShellCommManager::ShellCommManager(core::String const &name) noexcept : ICommunicationManager{name}
{
    log_.LogInfo() << "ShellCommManager::ShellCommManager()";
}

/// @brief Destructor function
ShellCommManager::~ShellCommManager() noexcept
{
    log_.LogInfo() << "ShellCommManager::~ShellCommManager()";
    ShellCommManager::Stop();
}

/// @brief Initialize
/// @return true - success
/// @return false - failure
bool ShellCommManager::Init() noexcept
{
    log_.LogInfo() << "ShellCommManager::Init()";
    ara::core::Result< ShellRequestImpl > skeletonShellRequestRes{ShellRequestImpl::Create< ShellRequestImpl >(
        core::InstanceSpecifier{(core::StringView{common::GetkShellRequestIdentifier()})},
        ara::com::MethodCallProcessingMode::kEvent)};
    if (!skeletonShellRequestRes.HasValue()) {
        log_.LogError() << "ShellCommManager::Init(), ShellRequestImpl Create err";
        return false;
    }
    shellRequestInstance_ = std::make_unique< ShellRequestImpl >(std::move(skeletonShellRequestRes).Value());
    return true;
}

/// @brief Start accepting requests
/// @return true - success
/// @return false - failure
bool ShellCommManager::Start() noexcept
{
    log_.LogInfo() << "ShellCommManager::Start()";
    if (!started_) {
        log_.LogDebug() << "ShellCommManager::Start(), try to OfferService.";
        if (shellRequestInstance_) {
            ara::core::Result< void > const result{shellRequestInstance_->OfferService()};
            if (result.HasValue()) {
                log_.LogDebug() << "ShellCommManager::Start(), shellRequestInstance_ offer service succeed.";
            } else {
                log_.LogError()
                    << "ShellCommManager::Start(), shellRequestInstance_ failed to offer service with error:"
                    << result.Error();
                return false;
            }
        } else {
            log_.LogError() << "ShellCommManager::Start(), shellRequestInstance_ is nullptr";
            return false;
        }
        started_ = true;
    }
    return true;
}

/// @brief Stop accepting requests
void ShellCommManager::Stop() noexcept
{
    log_.LogInfo() << "ShellCommManager::Stop()";
    if (started_) {
        log_.LogDebug() << "ShellCommManager::Stop(), all instances stop offer service";
        shellRequestInstance_->StopOfferService();
        started_ = false;
    }
}

/// @brief Register the callback function for appending events
/// @param appendEventHandler Callback function for appending events
void ShellCommManager::RegisterAppendEventHandler(std::function< void(common::Event &&) > appendEventHandler) noexcept
{
    log_.LogInfo() << "SMStateCommManager::RegisterAppendEventHandler()";
    if (shellRequestInstance_) {
        shellRequestInstance_->RegisterAppendEventHandler(appendEventHandler);
    } else {
        log_.LogDebug() << "ShellCommManager::RegisterAppendEventHandler(), shellRequestInstance_ is nullptr";
    }
}

void ShellCommManager::NotifySMStateUpdate(core::String const &smFQN, core::String const &smState) const noexcept
{
    log_.LogInfo() << "ShellCommManager::NotifySMStateUpdate(), smFQN:" << smFQN << "smState:" << smState;
    if (started_) {
        shellRequestInstance_->NotifySMStateUpdate(smFQN, smState);
    } else {
        log_.LogDebug() << "ShellCommManager::NotifySMStateUpdate(), shellRequestInstance_ hasn't been started";
    }
}
}  // namespace shell_comm
}  // namespace sm
}  // namespace ara