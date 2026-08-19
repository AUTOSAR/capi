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
/// @file       ucm_comm_manager.cpp
/// @brief      =Management class for communication with UCM
/// @details
/// @date       2024-06-26
/// @author     lianglongxiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/StateManagement/UCMComm
/// @interface_level=module
/// @trace_id_sr=SR_SM_04001, SR_SM_04005, SR_SM_04006, SR_SM_04007, SR_SM_04008, SR_SM_04009
/// @unit_name=UCMCommManager
/// @unit_description=Management class for communication with UCM
/// @endcode
///
/// ================================================================

#include "ucm_comm_manager.h"

namespace ara {
namespace sm {
namespace ucm_comm {

/// @brief Constructor function
/// @param name CommManager name
UCMCommManager::UCMCommManager(core::String const &name) noexcept : ICommunicationManager{name}
{
    log_.LogInfo() << "UCMCommManager::UCMCommManager()";
}

/// @brief Destructor function
UCMCommManager::~UCMCommManager() noexcept
{
    log_.LogInfo() << "UCMCommManager::~UCMCommManager()";
    UCMCommManager::Stop();
}

/// @brief Initialize
/// @return true - success
/// @return false - failure
bool UCMCommManager::Init() noexcept
{
    log_.LogInfo() << "UCMCommManager::Init()";
    ara::core::Result< UpdateRequestImpl > skeletonUpdateRequestRes{UpdateRequestImpl::Create< UpdateRequestImpl >(
        core::InstanceSpecifier{(core::StringView{common::GetkUpdateRequestIdentifier()})},
        ara::com::MethodCallProcessingMode::kEvent)};
    if (!skeletonUpdateRequestRes.HasValue()) {
        log_.LogWarn() << "UCMCommManager::Init(), UpdateRequestImpl Create err";
        return false;
    }
    updateRequestInstance_ = std::make_unique< UpdateRequestImpl >(std::move(skeletonUpdateRequestRes).Value());
    return true;
}

/// @brief Start accepting requests
/// @return true - success
/// @return false - failure
bool UCMCommManager::Start() noexcept
{
    log_.LogInfo() << "UCMCommManager::Start()";
    if (!started_) {
        log_.LogDebug() << "UCMCommManager::Start(), try to OfferService.";
        if (updateRequestInstance_) {
            ara::core::Result< void > const result{updateRequestInstance_->OfferService()};
            if (result.HasValue()) {
                log_.LogDebug() << "UCMCommManager::Start(), updateRequestInstance_ offer service succeed.";
            } else {
                log_.LogWarn() << "UCMCommManager::Start(), updateRequestInstance_ failed to offer service with error:"
                               << result.Error();
                return false;
            }
        } else {
            log_.LogWarn() << "UCMCommManager::Start(), updateRequestInstance_ is nullptr";
            return false;
        }
        started_ = true;
    }
    return true;
}

/// @brief Stop accepting requests
void UCMCommManager::Stop() noexcept
{
    log_.LogInfo() << "UCMCommManager::Stop()";
    if (started_) {
        log_.LogDebug() << "UCMCommManager::Stop(), all instances stop offer service";
        if (updateRequestInstance_) {
            updateRequestInstance_->StopOfferService();
        }
        started_ = false;
    }
}

/// @brief Register the callback function for appending events
/// @param appendEventHandler Callback function for appending events
void UCMCommManager::RegisterAppendEventHandler(std::function< void(common::Event &&) > appendEventHandler) noexcept
{
    log_.LogInfo() << "SMStateCommManager::RegisterAppendEventHandler()";
    if (updateRequestInstance_) {
        updateRequestInstance_->RegisterAppendEventHandler(appendEventHandler);
    } else {
        log_.LogDebug() << "UCMCommManager::RegisterAppendEventHandler(), updateRequestInstance_ is nullptr";
    }
}

}  // namespace ucm_comm
}  // namespace sm
}  // namespace ara