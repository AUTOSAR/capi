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
/// @file       dm_comm_manager.cpp
/// @brief      Management class for communication with Diagnostic Management
/// @details
/// @date       2024-04-30
/// @author     lianglongxiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/StateManagement/DMComm
/// @interface_level=module
/// @trace_id_sr=SR_SM_03002
/// @unit_name=DMCommManager
/// @unit_description=Management class for communication with Diagnostic Management
/// @endcode
///
/// ================================================================

#include "dm_comm_manager.h"

#include "define.h"
namespace ara {
namespace sm {
namespace dm_comm {

/// @brief Constructor function
/// @param name CommManager name
DMCommManager::DMCommManager(core::String const &name) noexcept
    : ICommunicationManager{name}
    , log_{log::CreateLogger((core::StringView{"#CMG"}), (core::StringView{"Communication Manager context"}))}
    , started_{false}
    , ecuResetRequestInstance_{nullptr}
{
    log_.LogInfo() << "DMCommManager::DMCommManager()";
}

/// @brief Destructor function
DMCommManager::~DMCommManager() noexcept
{
    log_.LogInfo() << "DMCommManager::~DMCommManager()";
    DMCommManager::Stop();
}

/// @brief Initialize
/// @return true - success
/// @return false - failure
bool DMCommManager::Init() noexcept
{
    log_.LogInfo() << "DMCommManager::Init()";
    if (ecuResetRequestInstance_ == nullptr) {
        core::StringView kEcuResetRequestIdentifier{common::GetkEcuResetRequestIdentifier()};
        ecuResetRequestInstance_.reset(
            new (std::nothrow) EcuResetRequestImpl(core::InstanceSpecifier(std::move(kEcuResetRequestIdentifier))));
    }
    return ecuResetRequestInstance_ != nullptr;
}

/// @brief Start accepting requests
/// @return true - success
/// @return false - failure
bool DMCommManager::Start() noexcept
{
    log_.LogInfo() << "DMCommManager::Start()";

    bool ret{true};
    if (!started_) {
        log_.LogDebug() << "DMCommManager::Start(), try to OfferService.";
        if (ecuResetRequestInstance_) {
            ara::core::Result< void > const result{ecuResetRequestInstance_->Offer()};
            // Check result
            if (result.HasValue()) {
                log_.LogDebug() << "DMCommManager::Start(), ecuResetRequestInstance_ offer service succeed.";
                started_ = true;
            } else {
                log_.LogWarn() << "DMCommManager::Start(), ecuResetRequestInstance_ failed to offer service with error:"
                               << result.Error();
                ret = false;
            }
        } else {
            log_.LogWarn() << "DMCommManager::Start(), ecuResetRequestInstance_ is nullptr";
            ret = false;
        }
    }
    return ret;
}

/// @brief Stop accepting requests
void DMCommManager::Stop() noexcept
{
    log_.LogInfo() << "DMCommManager::Stop()";

    if (started_) {
        if (ecuResetRequestInstance_) {
            log_.LogDebug() << "DMCommManager::Stop(), ecuResetRequestInstance_ stop offer service";
            ecuResetRequestInstance_->StopOffer();
        }
        started_ = false;
    }
}

/// @brief Register the callback function for appending events
/// @param appendEventHandler Callback function for appending events
void DMCommManager::RegisterAppendEventHandler(std::function< void(common::Event &&) > appendEventHandler) noexcept
{
    log_.LogInfo() << "DMCommManager::RegisterAppendEventHandler()";
    if (ecuResetRequestInstance_) {
        ecuResetRequestInstance_->RegisterAppendEventHandler(appendEventHandler);
    } else {
        log_.LogDebug() << "DMCommManager::RegisterAppendEventHandler(), ecuResetRequestInstance_ is nullptr";
    }
}

}  // namespace dm_comm
}  // namespace sm
}  // namespace ara