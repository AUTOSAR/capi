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
/// @file       action_item_sleep.h
/// @brief      Sleep action class
/// @details
/// @date       2024-08-20
/// @author     lianglongxiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/StateManagement/StateMachineManagement
/// @unit_name=ActionItemSleep
/// @interface_level=uint
/// @unit_description=Sleep action item
/// @trace_id_sr=SR_SM_05005, SR_SM_09007
/// @endcode
///
/// ================================================================

#ifndef ACTION_ITEM_SLEEP_H_
#define ACTION_ITEM_SLEEP_H_

#include "define.h"
#include "helper.h"
#include "state_machine_management/action_item_base.h"

namespace ara {
namespace sm {
namespace state_machine_management {

/// @brief Sleep action item
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_sr=SR_SM_05005, SR_SM_09007
/// @trace_id_ad=AD_SM_08079
/// @trace_id_dd=DD_SM_08276
/// @needwork = ad
/// @endcode
class ActionItemSleep : public ActionItemBase
{
public:
    /// @brief Constructor function
    /// @param sleepTimeMs the sleep time(ms)
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_05005, SR_SM_09007
    /// @trace_id_ad=AD_SM_08080
    /// @trace_id_dd=DD_SM_08277
    /// @needwork = ad
    /// @endcode
    explicit ActionItemSleep(uint32_t const &sleepTimeMs) noexcept;

    /// @brief Destructor function
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_05005, SR_SM_09007
    /// @trace_id_ad=AD_SM_08081
    /// @trace_id_dd=DD_SM_08278
    /// @needwork = ad
    /// @endcode
    ~ActionItemSleep() noexcept override = default;

    /// @brief deleted copy constructor function
    /// @param other The ActionItemSleep instance to be copyed
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_05005, SR_SM_09007
    /// @trace_id_ad=AD_SM_08082
    /// @trace_id_dd=DD_SM_08279
    /// @needwork = ad
    /// @endcode
    ActionItemSleep(ActionItemSleep const &other) = delete;

    /// @brief deleted copy assignment function
    /// @param other The ActionItemSleep instance to be copyed
    /// @return the assigned ActionItemSleep instance itself
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_05005, SR_SM_09007
    /// @trace_id_ad=AD_SM_08083
    /// @trace_id_dd=DD_SM_08280
    /// @needwork = ad
    /// @endcode
    ActionItemSleep &operator=(ActionItemSleep const &other) = delete;

    /// @brief Move constructor function
    /// @param other The ActionItemSleep instance to be moved
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_05005, SR_SM_09007
    /// @trace_id_ad=AD_SM_08084
    /// @trace_id_dd=DD_SM_08281
    /// @needwork = ad
    /// @endcode
    ActionItemSleep(ActionItemSleep &&other) = default;

    /// @brief Move assignment function
    /// @param other The ActionItemSleep instance to be moved
    /// @return the assigned ActionItemSleep instance itself
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_05005, SR_SM_09007
    /// @trace_id_ad=AD_SM_08085
    /// @trace_id_dd=DD_SM_08282
    /// @needwork = ad
    /// @endcode
    ActionItemSleep &operator=(ActionItemSleep &&other) = delete;

    /// @brief Start executing the action
    /// @param processID The ID when this action starts execution (also known as processing sequence number)
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_05005, SR_SM_09007
    /// @trace_id_ad=AD_SM_08086
    /// @trace_id_dd=DD_SM_08283
    /// @needwork = ad
    /// @endcode
    void StartExecute(uint32_t const processID) noexcept override;

    /// @brief Get String describing ActionItemSleep
    /// @return String describing ActionItemSleep
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_SM_05005, SR_SM_09007
    /// @trace_id_ad=AD_SM_08087
    /// @trace_id_dd=DD_SM_08284
    /// @needwork = ad
    /// @endcode
    inline core::String ActionItemToString() const noexcept override
    {
        uint32_t const sleepTimeMs{GetSleepTime()};
        return "type: " + state_machine_management::ToString(GetType()) + " " + core::to_string(sleepTimeMs) + " ms";
    }

protected:
    /// @brief Handling after action execution fails
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_05005, SR_SM_09007
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08285
    /// @needwork = dda
    /// @endcode
    void OnFailure() noexcept override;

    /// @brief Handling after action execution succeeds
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_05005, SR_SM_09007
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08286
    /// @needwork = dda
    /// @endcode
    void OnSuccess() noexcept override;

    /// @brief Callback when the Future corresponding to Action is ready
    /// @param processID Processing sequence number
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_05005, SR_SM_09007
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08287
    /// @needwork = dda
    /// @endcode
    void OnActionFutureReady(uint32_t const processID) noexcept override;
};

}  // namespace state_machine_management
}  // namespace sm
}  // namespace ara
#endif  // ACTION_ITEM_SLEEP_H_