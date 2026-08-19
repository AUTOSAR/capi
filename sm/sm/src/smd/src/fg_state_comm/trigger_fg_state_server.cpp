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
/// @file       trigger_fg_state_server.cpp
/// @brief      The class for providing function group state service for AA
/// @details
/// @date       2024-06-25
/// @author     lianglongxiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/StateManagement/FGStateComm
/// @interface_level=uint
/// @trace_id_sr=SR_SM_01001, SR_SM_01002, SR_SM_01003
/// @unit_name=TriggerFGStateServer
/// @unit_description=The class for providing function group state service for AA
/// @endcode
///
/// ================================================================

#include "trigger_fg_state_server.h"

#include <memory>
#include <mutex>
#include <thread>

#include "helper.h"

namespace ara {
namespace sm {
namespace fg_state_comm {
/// @brief Constructor function
/// @param info Function group service information
TriggerFGStateServer::TriggerFGStateServer(config::FGServiceInfo const &info) noexcept
    : started_{false}
    , fgFQN_{info.fgFQN}
    , triggerInFgInstanceId_{}
    , triggerIOFgInstanceId_{}
    , triggerOutFgInstanceId_{}
    , triggerInFgStateInstance_{nullptr}
    , triggerIOFgStateInstance_{nullptr}
    , triggerOutFgStateInstance_{nullptr}
    , log_{log::CreateLogger((core::StringView{"#CMG"}), (core::StringView{"Communication Manager context"}))}
    , appendEventHandler_{nullptr}
{
    log_.LogInfo() << "TriggerFGStateServer::TriggerFGStateServer(), fgFQN:" << info.fgFQN;
    core::String const keyTriggerInFgInstanceId{common::GetkTriggerInFGServiceInstances()};
    core::Map< core::String, core::String >::const_iterator const triggerInFgInstanceIdIt{
        info.fgInstanceID.find(keyTriggerInFgInstanceId)};
    if (info.fgInstanceID.end() != triggerInFgInstanceIdIt) {
        triggerInFgInstanceId_ = triggerInFgInstanceIdIt->second;
    } else {
        log_.LogDebug() << "TriggerFGStateServer::TriggerFGStateServer(), triggerInFgInstanceId_ is empty";
    }

    core::String const keyTriggerIOFgInstanceId{common::GetkTriggerIOFGServiceInstances()};
    core::Map< ara::core::String, ara::core::String >::const_iterator const triggerIOFgInstanceIdIt{
        info.fgInstanceID.find(keyTriggerIOFgInstanceId)};
    if (info.fgInstanceID.end() != triggerIOFgInstanceIdIt) {
        triggerIOFgInstanceId_ = triggerIOFgInstanceIdIt->second;
    } else {
        log_.LogDebug() << "TriggerFGStateServer::TriggerFGStateServer(), triggerIOFgInstanceId_ is empty";
    }

    core::String const keyTriggerOutFgInstanceId{common::GetkTriggerOutFGServiceInstances()};
    core::Map< core::String, core::String >::const_iterator const triggerOutFgInstanceIdIt{
        info.fgInstanceID.find(keyTriggerOutFgInstanceId)};
    if (info.fgInstanceID.end() != triggerOutFgInstanceIdIt) {
        triggerOutFgInstanceId_ = triggerOutFgInstanceIdIt->second;
    } else {
        log_.LogDebug() << "TriggerFGStateServer::TriggerFGStateServer(), triggerOutFgInstanceId_ is empty";
    }

    log_.LogDebug() << "TriggerFGStateServer::TriggerFGStateServer(), fgFQN_:" << fgFQN_.c_str()
                    << "triggerInFgInstanceId_:" << triggerInFgInstanceId_.c_str()
                    << "triggerIOFgInstanceId_:" << triggerIOFgInstanceId_.c_str()
                    << "triggerOutFgInstanceId_:" << triggerOutFgInstanceId_.c_str();
}

/// @brief Destructor function
TriggerFGStateServer::~TriggerFGStateServer() noexcept
{
    log_.LogInfo() << "TriggerFGStateServer::~TriggerFGStateServer(), fgFQN_:" << fgFQN_.c_str();
}

/// @brief Initialize
/// @return
bool TriggerFGStateServer::Init() noexcept
{
    log_.LogInfo() << "TriggerFGStateServer::Init()";
    ara::core::Result< ara::sm::fg_state_comm::TriggerInFGState, ara::core::ErrorCode > skeletonTriggerInRes{
        TriggerInFGState::Create< TriggerInFGState >(
            com::InstanceIdentifier{(core::StringView{triggerInFgInstanceId_})},
            com::MethodCallProcessingMode::kEvent)};
    if (!skeletonTriggerInRes.HasValue()) {
        log_.LogWarn() << "TriggerFGStateServer::Init(), TriggerInFGState Create err";
        return false;
    }
    triggerInFgStateInstance_ = std::make_unique< TriggerInFGState >(std::move(skeletonTriggerInRes).Value());
    std::ignore               = triggerInFgStateInstance_->Trigger.RegisterSetHandler(
        [this](core::String const &newState) noexcept { return _setFGStateHandler(newState); });

    ara::core::Result< ara::sm::fg_state_comm::TriggerIOFGState, ara::core::ErrorCode > skeletonTriggerIORes{
        TriggerIOFGState::Create< TriggerIOFGState >(
            com::InstanceIdentifier{(core::StringView{triggerIOFgInstanceId_})},
            com::MethodCallProcessingMode::kEvent)};
    if (!skeletonTriggerIORes.HasValue()) {
        log_.LogWarn() << "TriggerFGStateServer::Init(), TriggerIOFGState Create err";
        return false;
    }
    triggerIOFgStateInstance_ = std::make_unique< TriggerIOFGState >(std::move(skeletonTriggerIORes).Value());
    std::ignore               = triggerIOFgStateInstance_->Trigger.RegisterSetHandler(
        [this](core::String const &newState) noexcept { return _setFGStateHandler(newState); });
    std::ignore = triggerIOFgStateInstance_->Notifier.RegisterGetHandler(
        [this]() noexcept { return _getFGStateHandler(fgFQN_); });
    std::ignore = triggerIOFgStateInstance_->Notifier.Update(core::String{""});

    ara::core::Result< ara::sm::fg_state_comm::TriggerOutFGState, ara::core::ErrorCode > skeletonTriggerOutRes{
        TriggerOutFGState::Create< TriggerOutFGState >(
            com::InstanceIdentifier{(core::StringView{triggerOutFgInstanceId_})},
            com::MethodCallProcessingMode::kEvent)};
    if (!skeletonTriggerOutRes.HasValue()) {
        log_.LogWarn() << "TriggerFGStateServer::Init(), TriggerOutFGState Create err";
        return false;
    }
    triggerOutFgStateInstance_ = std::make_unique< TriggerOutFGState >(std::move(skeletonTriggerOutRes).Value());
    std::ignore                = triggerOutFgStateInstance_->Notifier.RegisterGetHandler(
        [this]() noexcept { return _getFGStateHandler(fgFQN_); });
    std::ignore = triggerOutFgStateInstance_->Notifier.Update(core::String{""});
    return true;
}

/// @brief  Start
/// @return
bool TriggerFGStateServer::Start() noexcept
{
    log_.LogInfo() << "TriggerFGStateServer::Start()";
    if (!started_) {
        log_.LogDebug() << "TriggerFGStateServer::Start(), try to OfferService.";
        if (triggerInFgStateInstance_) {
            ara::core::Result< void > const result{triggerInFgStateInstance_->OfferService()};
            if (result.HasValue()) {
                log_.LogDebug() << "TriggerFGStateServer::Start(), triggerInFgStateInstance_ offer service succeed.";
            } else {
                log_.LogWarn()
                    << "TriggerFGStateServer::Start(), triggerInFgStateInstance_ failed to offer service with error:"
                    << result.Error();
                return false;
            }
        } else {
            log_.LogWarn() << "TriggerFGStateServer::Start(), triggerInFgStateInstance_ is nullptr";
            return false;
        }

        if (triggerIOFgStateInstance_) {
            ara::core::Result< void > const result{triggerIOFgStateInstance_->OfferService()};
            if (result.HasValue()) {
                log_.LogDebug() << "TriggerFGStateServer::Start(), triggerIOFgStateInstance_ offer service succeed.";
            } else {
                log_.LogWarn()
                    << "TriggerFGStateServer::Start(), triggerIOFgStateInstance_ failed to offer service with error:"
                    << result.Error();
                return false;
            }
        } else {
            log_.LogWarn() << "TriggerFGStateServer::Start(), triggerIOFgStateInstance_ is nullptr";
            return false;
        }

        if (triggerOutFgStateInstance_) {
            ara::core::Result< void > const result{triggerOutFgStateInstance_->OfferService()};
            if (result.HasValue()) {
                log_.LogDebug() << "TriggerFGStateServer::Start(), triggerOutFgStateInstance_ offer service succeed.";
            } else {
                log_.LogWarn()
                    << "TriggerFGStateServer::Start(), triggerOutFgStateInstance_ failed to offer service with error:"
                    << result.Error();
                return false;
            }
        } else {
            log_.LogWarn() << "TriggerFGStateServer::Start(), triggerOutFgStateInstance_ is nullptr";
            return false;
        }
        started_ = true;
    }
    return true;
}

/// @brief Stop
void TriggerFGStateServer::Stop() noexcept
{
    log_.LogInfo() << "TriggerFGStateServer::Stop()";
    if (started_) {
        log_.LogDebug() << "TriggerFGStateServer::Stop(), all instances stop offer service";
        triggerInFgStateInstance_->StopOfferService();
        triggerIOFgStateInstance_->StopOfferService();
        triggerOutFgStateInstance_->StopOfferService();
        started_ = false;
    }
}

/// @brief Register the callback function for appending events
/// @param appendEventHandler Callback function for appending events
void TriggerFGStateServer::RegisterAppendEventHandler(
    std::function< void(common::Event &&) > const &appendEventHandler) noexcept
{
    appendEventHandler_ = appendEventHandler;
}

/// @brief Update notification
/// @param newFGState
void TriggerFGStateServer::NotifyFGStateUpdate(core::String const &newFGState) const noexcept
{
    log_.LogInfo() << "TriggerFGStateServer::NotifyFGStateUpdate(), newFGState:" << newFGState.c_str();
    if (triggerIOFgStateInstance_) {
        ara::core::Result< void > const result{triggerIOFgStateInstance_->Notifier.Update(newFGState)};
        if (!result.HasValue()) {
            log_.LogWarn()
                << "TriggerFGStateServer::NotifyFGStateUpdate(), triggerIOFgStateInstance_ failed to update Fg state,"
                << result.Error();
        }
    } else {
        log_.LogWarn() << "TriggerFGStateServer::NotifyFGStateUpdate(), triggerIOFgStateInstance_ is nullptr";
    }

    if (triggerOutFgStateInstance_) {
        ara::core::Result< void > const result{triggerOutFgStateInstance_->Notifier.Update(newFGState)};
        if (!result.HasValue()) {
            log_.LogWarn()
                << "TriggerFGStateServer::NotifyFGStateUpdate(), triggerOutFgStateInstance_ failed to update Fg state,"
                << result.Error();
        }
    } else {
        log_.LogWarn() << "TriggerFGStateServer::NotifyFGStateUpdate(), triggerOutFgStateInstance_ is nullptr";
    }
}

/// @brief The handler for TriggerInFGState
/// @param newState The new fg state
/// @return The trigger in result
core::Future< core::String > TriggerFGStateServer::_setFGStateHandler(core::String const &newState) const noexcept
{
    log_.LogInfo() << "TriggerFGStateServer::_setFGStateHandler(), newState:" << newState.c_str();
    core::Promise< core::String > promise{};
    core::Future< core::String > future{promise.get_future()};

    if (appendEventHandler_) {
        common::Event request;
        request.type                  = common::EventType::kInAASetFGState;
        request.data                  = new common::FGStateInfo{fgFQN_, newState};
        request.requestPromiseWithStr = std::move(promise);
        appendEventHandler_(std::move(request));
    } else {
        promise.SetError(SMErrc::kRejected);
        log_.LogWarn() << "TriggerFGStateServer::_setFGStateHandler(), appendEventHandler_ is nullptr";
    }
    return future;
}

/// @brief The handler for getting Fg state in TriggerIOFGState and TriggerOutFGState
/// @param  fgFQN Function group FQN
/// @return The Future object containing Fg state
core::Future< core::String > TriggerFGStateServer::_getFGStateHandler(core::String const &fgFQN) const noexcept
{
    log_.LogInfo() << "TriggerFGStateServer::_getFGStateHandler()";

    core::Promise< core::String > promise{};
    core::Future< core::String > future{promise.get_future()};

    if (appendEventHandler_) {
        common::Event request;
        request.type                  = common::EventType::kInAAGetFGState;
        request.data                  = new core::String(fgFQN);
        request.requestPromiseWithStr = std::move(promise);
        appendEventHandler_(std::move(request));
    } else {
        promise.SetError(SMErrc::kRejected);
        log_.LogWarn() << "TriggerFGStateServer::_getFGStateHandler(), appendEventHandler_ is nullptr";
    }
    return future;
}

}  // namespace fg_state_comm
}  // namespace sm
}  // namespace ara
