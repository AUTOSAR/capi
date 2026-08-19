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
/// @file       action_item_sync.h
/// @brief      Synchronization action
/// @details
/// @date       2024-08-20
/// @author     lianglongxiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/StateManagement/StateMachineManagement
/// @unit_name=ActionItemSync
/// @interface_level=uint
/// @unit_description=Synchronization action
/// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
/// @endcode
///
/// ================================================================

#ifndef ACTION_ITEM_SYNC_H_
#define ACTION_ITEM_SYNC_H_

#include "define.h"
#include "helper.h"
#include "state_machine_management/action_item_base.h"

namespace ara {
namespace sm {
namespace state_machine_management {

/// @brief Synchronization action
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
/// @trace_id_ad=AD_SM_08059
/// @trace_id_dd=DD_SM_08165
/// @needwork = ad
/// @endcode
class ActionItemSync : public ActionItemBase
{
public:
    /// @brief Constructor function
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_ad=AD_SM_08060
    /// @trace_id_dd=DD_SM_08166
    /// @needwork = ad
    /// @endcode
    ActionItemSync() noexcept;

    /// @brief Destructor function
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_ad=AD_SM_08061
    /// @trace_id_dd=DD_SM_08167
    /// @needwork = ad
    /// @endcode
    ~ActionItemSync() noexcept override = default;

    /// @brief deleted copy constructor function
    /// @param other The ActionItemSync instance to be copyed
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_ad=AD_SM_08062
    /// @trace_id_dd=DD_SM_08168
    /// @needwork = ad
    /// @endcode
    ActionItemSync(ActionItemSync const &other) = delete;

    /// @brief deleted copy assignment function
    /// @param other The ActionItemSync instance to be copyed
    /// @return the assigned ActionItemSync instance itself
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_ad=AD_SM_08063
    /// @trace_id_dd=DD_SM_08169
    /// @needwork = ad
    /// @endcode
    ActionItemSync &operator=(ActionItemSync const &other) = delete;

    /// @brief Move constructor function
    /// @param other The ActionItemSync instance to be moved
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_ad=AD_SM_08064
    /// @trace_id_dd=DD_SM_08170
    /// @needwork = ad
    /// @endcode
    ActionItemSync(ActionItemSync &&other) = default;

    /// @brief Move assignment function
    /// @param other The ActionItemSync instance to be moved
    /// @return the assigned ActionItemSync instance itself
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_ad=AD_SM_08065
    /// @trace_id_dd=DD_SM_08171
    /// @needwork = ad
    /// @endcode
    ActionItemSync &operator=(ActionItemSync &&other) = delete;

    /// @brief Start executing the action
    /// @param processID The ID when this action starts execution (also known as processing sequence number)
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_sr=SR_SM_09001, SR_SM_09002
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_ad=AD_SM_08066
    /// @trace_id_dd=DD_SM_08172
    /// @needwork = ad
    /// @endcode
    void StartExecute(uint32_t const processID) noexcept override;

    /// @brief Get String describing ActionItemSync
    /// @return String describing ActionItemSync
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_ad=AD_SM_08067
    /// @trace_id_dd=DD_SM_08173
    /// @needwork = ad
    /// @endcode
    inline core::String ActionItemToString() const noexcept override
    {
        ActionType const actionType{GetType()};
        return "type: " + state_machine_management::ToString(actionType);
    }

protected:
    /// @brief Handling after action execution fails
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08174
    /// @needwork = dda
    /// @endcode
    void OnFailure() noexcept override;

    /// @brief Handling after action execution succeeds
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08175
    /// @needwork = dda
    /// @endcode
    void OnSuccess() noexcept override;

    /// @brief Callback when the Future corresponding to Action is ready
    /// @param processID Processing sequence number
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_09002, SR_SM_09006, SR_SM_09007
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08176
    /// @needwork = dda
    /// @endcode
    void OnActionFutureReady(uint32_t const processID) noexcept override;
};

}  // namespace state_machine_management
}  // namespace sm
}  // namespace ara
#endif  // ACTION_ITEM_SYNC_H_