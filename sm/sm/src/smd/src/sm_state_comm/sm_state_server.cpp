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
/// @file       sm_state_server.cpp
/// @brief      The class for providing state machine state service for SMControlApplication and AA.
/// @details
/// @date       2024-06-25
/// @author     lianglongxiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/StateManagement/SMStateComm
/// @interface_level=uint
/// @trace_id_sr=SR_SM_09001, SR_SM_09008
/// @unit_name=SMStateServer
/// @unit_description=The class for providing state machine state service for SMControlApplication and AA.
/// @endcode
///
/// ================================================================

#include "sm_state_server.h"

#include <memory>
#include <mutex>
#include <thread>

#include "helper.h"

namespace ara {
namespace sm {
namespace sm_state_comm {
/// @brief Constructor function
/// @param info State machine service information
SMStateServer::SMStateServer(config::SMServiceInfo const &info) noexcept
    : smFQN_(info.smFQN)
    , smStateRequestInstanceId_(info.instanceId)
    , triggerOutSmStateInstanceId_(info.notificationPort)
{
    log_.LogInfo() << "SMStateServer::SMStateServer(), smFQN_:" << smFQN_.c_str()
                   << "smStateRequestInstanceId_:" << smStateRequestInstanceId_.c_str()
                   << "triggerOutSmStateInstanceId_:" << triggerOutSmStateInstanceId_.c_str();
}

/// @brief Destructor function
SMStateServer::~SMStateServer() noexcept
{
    log_.LogInfo() << "SMStateServer::~SMStateServer(), smFQN_:" << smFQN_.c_str();
    Stop();
}

/// @brief Initialize
/// @return
bool SMStateServer::Init() noexcept
{
    log_.LogInfo() << "SMStateServer::Init(), smStateRequestInstanceId_:" << smStateRequestInstanceId_.c_str()
                   << ", triggerOutSmStateInstanceId_:" << triggerOutSmStateInstanceId_.c_str();
    if (!smStateRequestInstanceId_.empty()) {
        ara::core::Result< ara::sm::sm_state_comm::SMStateRequest, ara::core::ErrorCode > skeletonStateRequestRes{
            SMStateRequest::Create< SMStateRequest >(
                com::InstanceIdentifier{(core::StringView{smStateRequestInstanceId_})},
                com::MethodCallProcessingMode::kEvent)};
        if (!skeletonStateRequestRes.HasValue()) {
            log_.LogWarn() << "SMStateServer::Init(), SMStateRequest Create err";
            return false;
        }
        smStateRequestInstance_ = std::make_unique< SMStateRequest >(std::move(skeletonStateRequestRes).Value());
        smStateRequestInstance_->SetSmFQN(smFQN_);
    } else {
        log_.LogWarn() << "SMStateServer::Init(), smStateRequestInstanceId_ is nullptr";
    }

    if (!triggerOutSmStateInstanceId_.empty()) {
        ara::core::Result< ara::sm::sm_state_comm::TriggerOutSMState, ara::core::ErrorCode > skeletonTriggerOutRes{
            TriggerOutSMState::Create< TriggerOutSMState >(
                com::InstanceIdentifier{(core::StringView{triggerOutSmStateInstanceId_.c_str()})},
                com::MethodCallProcessingMode::kEvent)};
        if (!skeletonTriggerOutRes.HasValue()) {
            log_.LogWarn() << "SMStateServer::Init(), TriggerOutSMState Create err using InstanceIdentifier, try "
                              "InstanceSpecifier";
            ara::core::Result< ara::sm::sm_state_comm::TriggerOutSMState, ara::core::ErrorCode >
                skeletonTriggerOutResFinal{TriggerOutSMState::Create< TriggerOutSMState >(
                    ara::core::InstanceSpecifier{(core::StringView{triggerOutSmStateInstanceId_.c_str()})},
                    com::MethodCallProcessingMode::kEvent)};
            if (!skeletonTriggerOutResFinal.HasValue()) {
                log_.LogWarn() << "SMStateServer::Init(), SMStateRequest Create err finally";
            } else {
                triggerOutSmStateInstance_
                    = std::make_unique< TriggerOutSMState >(std::move(skeletonTriggerOutResFinal).Value());
            }
        } else {
            triggerOutSmStateInstance_
                = std::make_unique< TriggerOutSMState >(std::move(skeletonTriggerOutRes).Value());
        }

        if (!triggerOutSmStateInstance_) {
            return false;
        }
        std::ignore = triggerOutSmStateInstance_->Notifier.RegisterGetHandler(
            [this]() noexcept { return _getSMStateHandler(smFQN_); });
        std::ignore = triggerOutSmStateInstance_->Notifier.Update(ara::core::String{""});
    } else {
        log_.LogWarn() << "SMStateServer::Init(), triggerOutSmStateInstanceId_ is nullptr";
    }
    return true;
}

/// @brief  Start
/// @return
bool SMStateServer::Start() noexcept
{
    log_.LogInfo() << "SMStateServer::Start(), for smFQN_:" << smFQN_.c_str();
    if (!started_) {
        log_.LogDebug() << "SMStateServer::Start(), try to OfferService.";
        if (smStateRequestInstance_) {
            ara::core::Result< void > const result{smStateRequestInstance_->OfferService()};
            if (result.HasValue()) {
                log_.LogDebug() << "SMStateServer::Start(), smStateRequestInstance_ offer service succeed.";
            } else {
                log_.LogWarn() << "SMStateServer::Start(), smStateRequestInstance_ failed to offer service with error:"
                               << result.Error();
                return false;
            }
        } else {
            log_.LogWarn() << "SMStateServer::Start(), smStateRequestInstance_ is nullptr";
            return true;
        }

        if (triggerOutSmStateInstance_) {
            ara::core::Result< void > const result{triggerOutSmStateInstance_->OfferService()};
            if (result.HasValue()) {
                log_.LogDebug() << "SMStateServer::Start(), triggerOutSmStateInstance_ offer service succeed.";
            } else {
                log_.LogWarn()
                    << "SMStateServer::Start(), triggerOutSmStateInstance_ failed to offer service with error:"
                    << result.Error();
                return false;
            }
        } else {
            log_.LogWarn() << "SMStateServer::Start(), triggerOutSmStateInstance_ is nullptr";
            return true;
        }
        started_ = true;
    }
    return true;
}

/// @brief Stop
void SMStateServer::Stop() noexcept
{
    log_.LogInfo() << "SMStateServer::Stop()";
    if (started_) {
        log_.LogDebug() << "SMStateServer::Stop(), all instances stop offer service";
        if (smStateRequestInstance_) {
            smStateRequestInstance_->StopOfferService();
        }
        if (triggerOutSmStateInstance_) {
            triggerOutSmStateInstance_->StopOfferService();
        }

        started_ = false;
    }
}

/// @brief Update notification
/// @param smState State machine state
void SMStateServer::NotifySMStateUpdate(core::String const &smState) const noexcept
{
    log_.LogInfo() << "SMStateServer::NotifySMStateUpdate(), smState:" << smState.c_str();
    if (triggerOutSmStateInstance_) {
        ara::core::Result< void > const result{triggerOutSmStateInstance_->Notifier.Update(smState)};
        if (!result.HasValue()) {
            log_.LogWarn()
                << "SMStateServer::NotifySMStateUpdate(), triggerOutSmStateInstance_ failed to update Fg state,"
                << result.Error();
        }
    } else {
        log_.LogWarn() << "SMStateServer::NotifySMStateUpdate(), triggerOutSmStateInstance_ is nullptr";
    }
}

/// @brief The handler for getting Fg state in TriggerIOFGState and TriggerOutFGState
/// @param smFQN State machine FQN
/// @return The Future object containing Fg state
ara::core::Future< core::String > SMStateServer::_getSMStateHandler(core::String const &smFQN) const noexcept
{
    log_.LogInfo() << "SMStateServer::_getSMStateHandler()";

    ara::core::Promise< core::String > promise{};
    ara::core::Future< core::String > future{promise.get_future()};
    if (appendEventHandler_) {
        common::Event request;
        request.type                  = common::EventType::kInGetSMState;
        request.data                  = new core::String(smFQN);
        request.requestPromiseWithStr = std::move(promise);
        appendEventHandler_(std::move(request));
    } else {
        promise.SetError(SMErrc::kRejected);
        log_.LogWarn() << "SMStateServer::_getSMStateHandler(), appendEventHandler_ is nullptr";
    }
    return future;
}

/// @brief Register the callback function for appending events
/// @param appendEventHandler Callback function for appending events
void SMStateServer::RegisterAppendEventHandler(
    std::function< void(common::Event &&) > const &appendEventHandler) noexcept
{
    appendEventHandler_ = appendEventHandler;
    if (smStateRequestInstance_) {
        smStateRequestInstance_->RegisterAppendEventHandler(appendEventHandler);
    } else {
        log_.LogWarn() << "SMStateServer::Start(), smStateRequestInstance_ is nullptr";
    }
}

}  // namespace sm_state_comm
}  // namespace sm
}  // namespace ara
