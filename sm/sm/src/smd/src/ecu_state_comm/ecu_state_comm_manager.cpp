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
/// @file       ecu_state_comm_manager.cpp
/// @brief      Management class for ECU state communication
/// @details
/// @date       2024-05-02
/// @author     lianglongxiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/StateManagement/EcuStateComm
/// @interface_level=module
/// @trace_id_sr=SR_SM_07001, SR_SM_07002
/// @unit_name=EcuStateCommManager
/// @unit_description=Management class for ECU state communication
/// @endcode
///
/// ================================================================

#include "ecu_state_comm_manager.h"

namespace ara {
namespace sm {
namespace ecu_state_comm {

/// @brief Constructor function
/// @param name CommManager name
EcuStateCommManager::EcuStateCommManager(core::String const &name) noexcept
    : ICommunicationManager{name}
    , log_{log::CreateLogger((core::StringView{"#CMG"}), (core::StringView{"Communication Manager context"}))}
    , started_{false}
    , triggerInEcuStateInstance_{}
    , triggerIOEcuStateInstance_{}
    , triggerOutEcuStateInstance_{}
    , appendEventHandler_{}
{
    log_.LogInfo() << "EcuStateCommManager::EcuStateCommManager()";
}

/// @brief Destructor function
EcuStateCommManager::~EcuStateCommManager() noexcept
{
    log_.LogInfo() << "EcuStateCommManager::~EcuStateCommManager()";
    EcuStateCommManager::Stop();
}

/// @brief Initialize
/// @return true - success
/// @return false - failure
bool EcuStateCommManager::Init() noexcept
{
    log_.LogInfo() << "EcuStateCommManager::Init()";
    ara::core::Result< ara::sm::ecu_state_comm::TriggerInEcuState, ara::core::ErrorCode > skeletonTriggerInRes{
        TriggerInEcuState::Create< TriggerInEcuState >(
            core::InstanceSpecifier{(core::StringView{common::GetkTriggerInEcuStateIdentifier()})},
            ara::com::MethodCallProcessingMode::kEvent)};
    if (!skeletonTriggerInRes.HasValue()) {
        log_.LogWarn() << "EcuStateCommManager::Init(), TriggerInEcuState Create err";
    } else {
        triggerInEcuStateInstance_ = std::make_unique< TriggerInEcuState >(std::move(skeletonTriggerInRes).Value());
        std::ignore                = triggerInEcuStateInstance_->Trigger.RegisterSetHandler(
            [this](core::String const &newState) noexcept { return _setEcuStateHandler(newState); });
    }

    ara::core::Result< ara::sm::ecu_state_comm::TriggerIOEcuState, ara::core::ErrorCode > skeletonTriggerIORes{
        TriggerIOEcuState::Create< TriggerIOEcuState >(
            core::InstanceSpecifier{(core::StringView{common::GetkTriggerIOEcuStateIdentifier()})},
            ara::com::MethodCallProcessingMode::kEvent)};
    if (!skeletonTriggerIORes.HasValue()) {
        log_.LogWarn() << "EcuStateCommManager::Init(), TriggerIOEcuState Create err";
    } else {
        triggerIOEcuStateInstance_ = std::make_unique< TriggerIOEcuState >(std::move(skeletonTriggerIORes).Value());
        std::ignore                = triggerIOEcuStateInstance_->Trigger.RegisterSetHandler(
            [this](core::String const &newState) noexcept { return _setEcuStateHandler(newState); });
        std::ignore = triggerIOEcuStateInstance_->Notifier.RegisterGetHandler(
            [this]() noexcept { return _getEcuStateHandler(); });
        std::ignore = triggerIOEcuStateInstance_->Notifier.Update(ara::core::String{""});
    }
    ara::core::Result< ara::sm::ecu_state_comm::TriggerOutEcuState, ara::core::ErrorCode > skeletonTriggerOutRes{
        TriggerOutEcuState::Create< TriggerOutEcuState >(
            core::InstanceSpecifier{(core::StringView{common::GetkTriggerOutEcuStateIdentifier()})},
            ara::com::MethodCallProcessingMode::kEvent)};
    if (!skeletonTriggerOutRes.HasValue()) {
        log_.LogWarn() << "EcuStateCommManager::Init(), TriggerOutEcuState Create err";
    } else {
        triggerOutEcuStateInstance_ = std::make_unique< TriggerOutEcuState >(std::move(skeletonTriggerOutRes).Value());
        std::ignore                 = triggerOutEcuStateInstance_->Notifier.RegisterGetHandler(
            [this]() noexcept { return _getEcuStateHandler(); });
        std::ignore = triggerOutEcuStateInstance_->Notifier.Update(ara::core::String{""});
    }
    return true;
}

/// @brief Start accepting requests
/// @return true - success
/// @return false - failure
bool EcuStateCommManager::Start() noexcept
{
    log_.LogInfo() << "EcuStateCommManager::Start()";
    if (!started_) {
        log_.LogDebug() << "EcuStateCommManager::Start(), try to OfferService.";
        if (triggerInEcuStateInstance_) {
            ara::core::Result< void > const result{triggerInEcuStateInstance_->OfferService()};
            if (result.HasValue()) {
                log_.LogDebug() << "EcuStateCommManager::Start(), triggerInEcuStateInstance_ offer service succeed.";
            } else {
                log_.LogWarn()
                    << "EcuStateCommManager::Start(), triggerInEcuStateInstance_ failed to offer service with error:"
                    << result.Error();
                return false;
            }
        } else {
            log_.LogWarn() << "EcuStateCommManager::Start(), triggerInEcuStateInstance_ is nullptr";
        }

        if (triggerIOEcuStateInstance_) {
            ara::core::Result< void > const result{triggerIOEcuStateInstance_->OfferService()};
            if (result.HasValue()) {
                log_.LogDebug() << "EcuStateCommManager::Start(), triggerIOEcuStateInstance_ offer service succeed.";
            } else {
                log_.LogWarn()
                    << "EcuStateCommManager::Start(), triggerIOEcuStateInstance_ failed to offer service with error:"
                    << result.Error();
                return false;
            }
        } else {
            log_.LogWarn() << "EcuStateCommManager::Start(), triggerIOEcuStateInstance_ is nullptr";
        }

        if (triggerOutEcuStateInstance_) {
            ara::core::Result< void > const result{triggerOutEcuStateInstance_->OfferService()};
            if (result.HasValue()) {
                log_.LogDebug() << "EcuStateCommManager::Start(), triggerOutEcuStateInstance_ offer service succeed.";
            } else {
                log_.LogWarn()
                    << "EcuStateCommManager::Start(), triggerOutEcuStateInstance_ failed to offer service with error:"
                    << result.Error();
                return false;
            }
        } else {
            log_.LogWarn() << "EcuStateCommManager::Start(), triggerOutEcuStateInstance_ is nullptr";
        }
        started_ = true;
    }
    return true;
}

/// @brief Stop accepting requests
void EcuStateCommManager::Stop() noexcept
{
    log_.LogInfo() << "EcuStateCommManager::Stop()";
    if (started_) {
        log_.LogDebug() << "EcuStateCommManager::Stop(), all instances stop offer service";
        if (triggerInEcuStateInstance_) {
            triggerInEcuStateInstance_->StopOfferService();
        }
        if (triggerIOEcuStateInstance_) {
            triggerIOEcuStateInstance_->StopOfferService();
        }
        if (triggerOutEcuStateInstance_) {
            triggerOutEcuStateInstance_->StopOfferService();
        }
        started_ = false;
    }
}

/// @brief Register the callback function for appending events
/// @param appendEventHandler Callback function for appending events
void EcuStateCommManager::RegisterAppendEventHandler(
    std::function< void(common::Event &&) > appendEventHandler) noexcept
{
    log_.LogInfo() << "EcuStateCommManager::RegisterAppendEventHandler()";
    appendEventHandler_ = appendEventHandler;
}

/// @brief Notify the monitored state machine that the ECU state has been updated
/// @param ecuState ECU state
void EcuStateCommManager::NotifyEcuStateUpdate(core::String const &ecuState) const noexcept
{
    log_.LogInfo() << "EcuStateCommManager::NotifyEcuStateUpdate(), ecuState:" << ecuState.c_str();
    if (triggerIOEcuStateInstance_) {
        ara::core::Result< void > const result{triggerIOEcuStateInstance_->Notifier.Update(ecuState)};
        if (!result.HasValue()) {
            log_.LogWarn()
                << "EcuStateCommManager::NotifyEcuStateUpdate(), triggerIOEcuStateInstance_ failed to update ecu state,"
                << result.Error();
        }
    } else {
        log_.LogWarn() << "EcuStateCommManager::NotifyEcuStateUpdate(), triggerIOEcuStateInstance_ is nullptr";
    }

    if (triggerOutEcuStateInstance_) {
        ara::core::Result< void > const result{triggerOutEcuStateInstance_->Notifier.Update(ecuState)};
        if (!result.HasValue()) {
            log_.LogWarn() << "EcuStateCommManager::NotifyEcuStateUpdate(), triggerOutEcuStateInstance_ failed to "
                              "update ecu state,"
                           << result.Error();
        }
    } else {
        log_.LogWarn() << "EcuStateCommManager::NotifyEcuStateUpdate(), triggerOutEcuStateInstance_ is nullptr";
    }
}

/// @brief The handler for TriggerInEcuState
/// @param newState The new ecu state
/// @return The trigger in result
ara::core::Future< core::String > EcuStateCommManager::_setEcuStateHandler(core::String const &newState) const noexcept
{
    log_.LogInfo() << "EcuStateCommManager::_setEcuStateHandler(), newState:" << newState.c_str();
    ara::core::Promise< core::String > promise{};
    ara::core::Future< core::String > future{promise.get_future()};

    if (appendEventHandler_) {
        common::Event request;
        request.type                  = common::EventType::kInAASetEcuState;
        request.data                  = new core::String(newState);
        request.requestPromiseWithStr = std::move(promise);
        appendEventHandler_(std::move(request));
    } else {
        promise.SetError(SMErrc::kRejected);
        log_.LogWarn() << "EcuStateCommManager::_setEcuStateHandler(), appendEventHandler_ is nullptr";
    }
    return future;
}

/// @brief The handler for getting ecu state in TriggerIOEcuState and TriggerOutEcuState
/// @return The Future object containing ecu state
ara::core::Future< core::String > EcuStateCommManager::_getEcuStateHandler() const noexcept
{
    log_.LogInfo() << "EcuStateCommManager::_getEcuStateHandler()";

    ara::core::Promise< core::String > promise{};
    ara::core::Future< core::String > future{promise.get_future()};

    if (appendEventHandler_) {
        common::Event request;
        request.type                  = common::EventType::kInGetEcuState;
        request.data                  = nullptr;
        request.requestPromiseWithStr = std::move(promise);
        appendEventHandler_(std::move(request));
    } else {
        promise.SetError(SMErrc::kRejected);
        log_.LogWarn() << "EcuStateCommManager::_getEcuStateHandler(), appendEventHandler_ is nullptr";
    }
    return future;
}

}  // namespace ecu_state_comm
}  // namespace sm
}  // namespace ara