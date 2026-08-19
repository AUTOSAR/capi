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
/// @file       sm_state_request.cpp
/// @brief      A implementation of ara::sm::skeleton::StateMachineServiceSkeleton.
/// @details
/// @date       2024-05-07
/// @author     lianglongxiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/StateManagement/SMStateComm
/// @interface_level=uint
/// @trace_id_sr=SR_SM_09001
/// @unit_name=SMStateRequest
/// @unit_description=A implementation of ara::sm::skeleton::StateMachineServiceSkeleton.
/// @endcode
///
/// ================================================================

#include "sm_state_comm/sm_state_request.h"

namespace ara {
namespace sm {
namespace sm_state_comm {
/// @brief Create notification
/// @param appendEventHandler Callback function for appending events
void SMStateRequest::RegisterAppendEventHandler(
    std::function< void(common::Event &&) > const &appendEventHandler) noexcept
{
    appendEventHandler_ = appendEventHandler;
}

/// @brief Request state transition
/// @param transitionRequest Transition request
/// @return Request result
core::Future< void > SMStateRequest::RequestState(::ara::sm::TransitionRequestType const &transitionRequest) noexcept
{
    log_.LogInfo() << "SMStateRequest::RequestState(), smFQN_:" << smFQN_.c_str()
                   << "transitionRequest:" << transitionRequest;

    core::Promise< void > promise{};
    core::Future< void > future{promise.get_future()};

    if (appendEventHandler_ != nullptr) {
        if (smFQN_.empty() == false) {
            common::Event request;
            request.type           = common::EventType::kInSMCSetSMState;
            request.data           = new common::SMStateRequestInfo{smFQN_, transitionRequest};
            request.requestPromise = std::move(promise);
            appendEventHandler_(std::move(request));
        }
    } else {
        promise.SetError(SMErrc::kRejected);
        log_.LogWarn() << "TriggerFGStateServer::RequestState(), appendEventHandler_ is nullptr or smFQN_ is empty";
    }
    return future;
}

/// @brief Set state machine FQN
/// @param smFQN State machine FQN
void SMStateRequest::SetSmFQN(core::String const &smFQN) noexcept { smFQN_ = smFQN; }
}  // namespace sm_state_comm
}  // namespace sm
}  // namespace ara
