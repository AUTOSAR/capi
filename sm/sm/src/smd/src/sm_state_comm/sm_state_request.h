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
/// @file       sm_state_request.h
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
/// @unit_name=SMStateRequest
/// @interface_level=uint
/// @unit_description=A implementation of ara::sm::skeleton::StateMachineServiceSkeleton.
/// @trace_id_sr=SR_SM_09001
/// @endcode
///
/// ================================================================

#ifndef SM_STATE_REQUEST_H_
#define SM_STATE_REQUEST_H_

#include <ara/log/logger.h>
#include <ara/sm/statemachineservice_skeleton.h>

#include "event.h"

namespace ara {
namespace sm {
namespace sm_state_comm {

/// @brief A implementation of ara::sm::skeleton::StateMachineServiceSkeleton.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_sr=SR_SM_09001
/// @trace_id_ad=AD_SM_08089
/// @trace_id_dd=DD_SM_08316
/// @needwork = ad
/// @endcode
class SMStateRequest : public ara::sm::skeleton::StateMachineServiceSkeleton
{
public:
    /// @brief Abstract skeleton class which is implemented here.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = no
    /// @endcode
    using Skeleton = ara::sm::skeleton::StateMachineServiceSkeleton;

    /// @brief Use base class constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = no
    /// @endcode
    using Skeleton::Skeleton;

    /// @brief Destructor function
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_09001
    /// @trace_id_ad=AD_SM_00312
    /// @trace_id_dd=DD_SM_00328
    /// @needwork = ad
    /// @endcode
    ~SMStateRequest() final = default;

    /// @brief deleted copy constructor function
    /// @param other The SMStateRequest instance to be copyed
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_09001
    /// @trace_id_ad=AD_SM_00313
    /// @trace_id_dd=DD_SM_00329
    /// @needwork = ad
    /// @endcode
    SMStateRequest(SMStateRequest const &other) = delete;

    /// @brief deleted copy assignment function
    /// @param other The SMStateRequest instance to be copyed
    /// @return the assigned SMStateRequest instance itself
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_09001
    /// @trace_id_ad=AD_SM_00314
    /// @trace_id_dd=DD_SM_00330
    /// @needwork = ad
    /// @endcode
    SMStateRequest &operator=(SMStateRequest const &other) = delete;

    /// @brief Move constructor function
    /// @param other The SMStateRequest instance to be moved
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_09001
    /// @trace_id_ad=AD_SM_00315
    /// @trace_id_dd=DD_SM_00331
    /// @needwork = ad
    /// @endcode
    SMStateRequest(SMStateRequest &&other) = default;

    /// @brief Move assignment function
    /// @param other The SMStateRequest instance to be moved
    /// @return the assigned SMStateRequest instance itself
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_09001
    /// @trace_id_ad=AD_SM_00316
    /// @trace_id_dd=DD_SM_00332
    /// @needwork = ad
    /// @endcode
    SMStateRequest &operator=(SMStateRequest &&other) = delete;

    /// @brief Register the callback function for appending events
    /// @param appendEventHandler Callback function for appending events
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_09001
    /// @trace_id_ad=AD_SM_00317
    /// @trace_id_dd=DD_SM_00333
    /// @needwork = ad
    /// @endcode
    void RegisterAppendEventHandler(std::function< void(common::Event &&) > const &appendEventHandler) noexcept;

    /// @brief Set the state machine FQN to which this service instance belongs
    /// @param smFQN State machine FQN
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_09001
    /// @trace_id_ad=AD_SM_00318
    /// @trace_id_dd=DD_SM_00334
    /// @needwork = ad
    /// @endcode
    void SetSmFQN(core::String const &smFQN) noexcept;

    /// @brief Request to transition the state machine state
    /// @param transitionRequest Request to transition the state machine state
    /// @return Future object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_09001
    /// @trace_id_ad=AD_SM_00319
    /// @trace_id_dd=DD_SM_00335
    /// @needwork = ad
    /// @endcode
    core::Future< void > RequestState(::ara::sm::TransitionRequestType const &transitionRequest) noexcept override;

private:
    /// @brief Function handle for publishing events to EventManager
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_09001
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08317
    /// @needwork = dda
    /// @endcode
    std::function< void(common::Event &&) > appendEventHandler_{nullptr};

    /// @brief The state machine FQN to which this service instance belongs
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_09001
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08318
    /// @needwork = dda
    /// @endcode
    core::String smFQN_{};

    /// @brief Logger instance
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_09001
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08319
    /// @needwork = dda
    /// @endcode
    log::Logger &log_{
        log::CreateLogger((core::StringView{"#CMG"}), (core::StringView{"Communication Manager context"}))};
};
}  // namespace sm_state_comm
}  // namespace sm
}  // namespace ara

#endif  // SM_STATE_REQUEST_H_
