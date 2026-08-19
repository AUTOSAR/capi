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
/// @file       condition.h
/// @brief      This file provides condition functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/isoft/uds/fault_management
/// @interface_level=software
/// @trace_id_sr=5c3ea362-aee6-4de8-871f-fc1332e90b6c
/// @unit_name=condition
/// @unit_description=This file provides condition functionality
/// @endcode
///
/// ================================================================

#ifndef __ISOFT_UDS_FAULT_CONDITION_H__
#define __ISOFT_UDS_FAULT_CONDITION_H__

#include <memory>

#include "isoft/uds/fault_management/fault_management.h"

namespace isoft {
namespace uds {
namespace server {

/// @brief condition wrapper class
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02566
/// @needwork = dd
/// @endcode
class Condition final
{
public:
    /// @brief Condition copy constructor
    /// @param[in] other Condition reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00819
    /// @needwork = dda
    /// @endcode
    Condition(Condition const& other) = delete;
    /// @brief Condition copy constructor
    /// @param[in] other Condition rvalue reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00820
    /// @needwork = dda
    /// @endcode
    Condition(Condition&& other) = delete;
    /// @brief Condition assignment operator
    /// @param[in] right Condition reference
    /// @return Condition reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00821
    /// @needwork = dda
    /// @endcode
    Condition& operator=(Condition const& right) = delete;
    /// @brief Condition assignment operator
    /// @param[in] right Condition rvalue reference
    /// @return Condition reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00822
    /// @needwork = dda
    /// @endcode
    Condition& operator=(Condition&& right) = delete;

public:
    /// @brief Condition default constructor
    Condition() = delete;

    /// @brief Condition constructor
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01958
    /// @needwork = dd
    /// @endcode
    explicit Condition(ConditionConfig& config) : config_{config} {}

    /// @brief Condition default destructor
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01959
    /// @needwork = dd
    /// @endcode
    ~Condition() = default;

    /// @brief Start service
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01960
    /// @needwork = dd
    /// @endcode
    bool Init();

    /// @brief Check whether Condition is satisfied
    /// @return Returns true if satisfied, false if not satisfied
    /// @code{.isoft}
    /// @throw unknown
    /// @needwork = dd
    /// @endcode
    bool CheckCondition() const noexcept;

    /// @brief Alias for callback function std::function<void()>
    /// @brief State change callback function
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01961
    /// @needwork = dd
    /// @endcode
    using StatusChangedCallback = std::function< void() >;

    /// @brief Set the callback function for condition state change
    /// @param[in] callback Callback function
    /// @code{.isoft}
    /// @throw unknown
    /// @needwork = dd
    /// @endcode
    void OnConditionStatusChanged(StatusChangedCallback const& callback);

private:
    /// @brief Whether this condition is satisfied
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01962
    /// @needwork = dd
    /// @endcode
    std::atomic_bool state_{true};

    /// @brief Set state change callback function
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01963
    /// @needwork = dd
    /// @endcode
    std::vector< StatusChangedCallback > callbacks_{};

    /// @brief Condition configuration
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01964
    /// @needwork = dd
    /// @endcode
    ConditionConfig config_{};
};

}  // namespace server
}  // namespace uds
}  // namespace isoft
#endif  // __ISOFT_UDS_FAULT_CONDITION_H__