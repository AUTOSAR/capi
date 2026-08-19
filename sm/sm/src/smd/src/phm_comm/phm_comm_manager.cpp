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
/// @file       phm_comm_manager.cpp
/// @brief      Management class for communication with Platform Health Management, implementing error recovery functionality
/// @details
/// @date       2024-05-07
/// @author     lianglongxiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/StateManagement/PHMComm
/// @interface_level=module
/// @trace_id_sr=SR_SM_08001
/// @unit_name=PHMCommManager
/// @unit_description=Management class for communication with Platform Health Management, implementing error recovery functionality
/// @endcode
///
/// ================================================================

#include "phm_comm/phm_comm_manager.h"

namespace ara {
namespace sm {
namespace phm_comm {

/// @brief Constructor function
/// @param name CommManager name
PHMCommManager::PHMCommManager(core::String const &name) noexcept
    : ICommunicationManager{name}
    , log_{log::CreateLogger((core::StringView{"#CMG"}), (core::StringView{"PHMCommManager context"}))}
{
    log_.LogInfo() << "PHMCommManager::PHMCommManager()";
}

/// @brief Move constructor function
/// @param other The PHMCommManager instance to be moved
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_SM_00099
/// @trace_id_dd=DD_SM_00099
/// @needwork = ad
/// @endcode
PHMCommManager::PHMCommManager(PHMCommManager &&other) noexcept
    : ICommunicationManager{std::move(other)}
    , started_{other.started_}
    , volHealthChannelAction_{std::move(other.volHealthChannelAction_)}
    , hcTestUnitHealthChannelAction_{std::move(other.hcTestUnitHealthChannelAction_)}
    , tyrePressureHealthChannelAction_{std::move(other.tyrePressureHealthChannelAction_)}
    , engineRecoveryAction_{std::move(other.engineRecoveryAction_)}
    , svTestUnitRecoveryAction_{std::move(other.svTestUnitRecoveryAction_)}
    , wheelRecoveryAction_{std::move(other.wheelRecoveryAction_)}
    , log_{other.log_}
{
}

/// @brief Destructor function
PHMCommManager::~PHMCommManager() noexcept { log_.LogInfo() << "PHMCommManager::~PHMCommManager()"; }

/// @brief Initialize
/// @return true - success
/// @return false - failure
bool PHMCommManager::Init() noexcept
{
    log_.LogInfo() << "PHMCommManager::Init()";
    core::StringView kVolHealthChannelActionIdentifier{common::GetkVolHealthChannelActionIdentifier()};
    core::StringView kTyrePressureHealthChannelActionIdentifier{
        common::GetkTyrePressureHealthChannelActionIdentifier()};
    core::StringView kHcTestUnitHealthChannelActionIdentifier{common::GetkHcTestUnitHealthChannelActionIdentifier()};
    core::StringView kWheelRecoveryActionIdentifier{common::GetkWheelRecoveryActionIdentifier()};
    core::StringView kSvTestUnitRecoveryActionIdentifier{common::GetkSvTestUnitRecoveryActionIdentifier()};
    core::StringView kEngineRecoveryActionIdentifier{common::GetkEngineRecoveryActionIdentifier()};
    volHealthChannelAction_ = std::make_unique< VolHealthChannelAction >(
        core::InstanceSpecifier(std::move(kVolHealthChannelActionIdentifier)));
    tyrePressureHealthChannelAction_ = std::make_unique< TyrePressureHealthChannelAction >(
        core::InstanceSpecifier(std::move(kTyrePressureHealthChannelActionIdentifier)));
    hcTestUnitHealthChannelAction_ = std::make_unique< HcTestUnitHealthChannelAction >(
        core::InstanceSpecifier(std::move(kHcTestUnitHealthChannelActionIdentifier)));
    wheelRecoveryAction_
        = std::make_unique< WheelRecoveryAction >(core::InstanceSpecifier(std::move(kWheelRecoveryActionIdentifier)));
    svTestUnitRecoveryAction_ = std::make_unique< SvTestUnitRecoveryAction >(
        core::InstanceSpecifier(std::move(kSvTestUnitRecoveryActionIdentifier)));
    engineRecoveryAction_
        = std::make_unique< EngineRecoveryAction >(core::InstanceSpecifier(std::move(kEngineRecoveryActionIdentifier)));
    return true;
}

/// @brief Start accepting requests
/// @return true - success
/// @return false - failure
bool PHMCommManager::Start() noexcept
{
    log_.LogInfo() << "PHMCommManager::Start()";
    if (!started_) {
        if (volHealthChannelAction_) {
            std::ignore = volHealthChannelAction_->Offer();
        } else {
            log_.LogWarn() << "PHMCommManager::Start(), volHealthChannelAction_ is nullptr";
        }

        if (tyrePressureHealthChannelAction_) {
            std::ignore = tyrePressureHealthChannelAction_->Offer();
        } else {
            log_.LogWarn() << "PHMCommManager::Start(), tyrePressureHealthChannelAction_ is nullptr";
        }

        if (hcTestUnitHealthChannelAction_) {
            std::ignore = hcTestUnitHealthChannelAction_->Offer();
        } else {
            log_.LogWarn() << "PHMCommManager::Start(), hcTestUnitHealthChannelAction_ is nullptr";
        }

        if (wheelRecoveryAction_) {
            std::ignore = wheelRecoveryAction_->Offer();
        } else {
            log_.LogWarn() << "PHMCommManager::Start(), wheelRecoveryAction_ is nullptr";
        }

        if (svTestUnitRecoveryAction_) {
            std::ignore = svTestUnitRecoveryAction_->Offer();
        } else {
            log_.LogWarn() << "PHMCommManager::Start(), svTestUnitRecoveryAction_ is nullptr";
        }

        if (engineRecoveryAction_) {
            std::ignore = engineRecoveryAction_->Offer();
        } else {
            log_.LogWarn() << "PHMCommManager::Start(), engineRecoveryAction_ is nullptr";
        }
        started_ = true;
    }
    return true;
}

/// @brief Stop accepting requests
void PHMCommManager::Stop() noexcept
{
    log_.LogInfo() << "PHMCommManager::Stop()";
    if (started_) {
        if (volHealthChannelAction_) {
            volHealthChannelAction_->StopOffer();
        } else {
            log_.LogWarn() << "PHMCommManager::Stop(), volHealthChannelAction_ is nullptr";
        }

        if (tyrePressureHealthChannelAction_) {
            tyrePressureHealthChannelAction_->StopOffer();
        } else {
            log_.LogWarn() << "PHMCommManager::Stop(), tyrePressureHealthChannelAction_ is nullptr";
        }

        if (hcTestUnitHealthChannelAction_) {
            hcTestUnitHealthChannelAction_->StopOffer();
        } else {
            log_.LogWarn() << "PHMCommManager::Stop(), hcTestUnitHealthChannelAction_ is nullptr";
        }

        if (wheelRecoveryAction_) {
            wheelRecoveryAction_->StopOffer();
        } else {
            log_.LogWarn() << "PHMCommManager::Stop(), wheelRecoveryAction_ is nullptr";
        }

        if (svTestUnitRecoveryAction_) {
            svTestUnitRecoveryAction_->StopOffer();
        } else {
            log_.LogWarn() << "PHMCommManager::Stop(), svTestUnitRecoveryAction_ is nullptr";
        }

        if (engineRecoveryAction_) {
            engineRecoveryAction_->StopOffer();
        } else {
            log_.LogWarn() << "PHMCommManager::Stop(), engineRecoveryAction_ is nullptr";
        }
        started_ = false;
    }
}

/// @brief Register the callback function for appending events
/// @param appendEventHandler Callback function for appending events
void PHMCommManager::RegisterAppendEventHandler(std::function< void(common::Event &&) > appendEventHandler) noexcept
{
    log_.LogInfo() << "PHMCommManager::RegisterAppendEventHandler()";
    if (volHealthChannelAction_) {
        volHealthChannelAction_->RegisterAppendEventHandler(appendEventHandler);
    } else {
        log_.LogWarn() << "PHMCommManager::RegisterAppendEventHandler(), volHealthChannelAction_ is nullptr";
    }

    if (tyrePressureHealthChannelAction_) {
        tyrePressureHealthChannelAction_->RegisterAppendEventHandler(appendEventHandler);
    } else {
        log_.LogWarn() << "PHMCommManager::RegisterAppendEventHandler(), tyrePressureHealthChannelAction_ is nullptr";
    }

    if (hcTestUnitHealthChannelAction_) {
        hcTestUnitHealthChannelAction_->RegisterAppendEventHandler(appendEventHandler);
    } else {
        log_.LogWarn() << "PHMCommManager::RegisterAppendEventHandler(), hcTestUnitHealthChannelAction_ is nullptr";
    }

    if (wheelRecoveryAction_) {
        wheelRecoveryAction_->RegisterAppendEventHandler(appendEventHandler);
    } else {
        log_.LogWarn() << "PHMCommManager::RegisterAppendEventHandler(), wheelRecoveryAction_ is nullptr";
    }

    if (svTestUnitRecoveryAction_) {
        svTestUnitRecoveryAction_->RegisterAppendEventHandler(appendEventHandler);
    } else {
        log_.LogWarn() << "PHMCommManager::RegisterAppendEventHandler(), svTestUnitRecoveryAction_ is nullptr";
    }

    if (engineRecoveryAction_) {
        engineRecoveryAction_->RegisterAppendEventHandler(appendEventHandler);
    } else {
        log_.LogWarn() << "PHMCommManager::RegisterAppendEventHandler(), engineRecoveryAction_ is nullptr";
    }
}

}  // namespace phm_comm
}  // namespace sm
}  // namespace ara