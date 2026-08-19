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
/// @file       condition_group.h
/// @brief      This file provides condition group type definitions
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
/// @trace_id_sr=218ebb01-f354-4639-9bc1-5f91a494023b
/// @unit_name=ConditionGroup
/// @unit_description=This file provides condition_group functionality
/// @endcode
///
/// ================================================================

#ifndef __ISOFT_UDS_FAULT_CONDITION_GROUP_H__
#define __ISOFT_UDS_FAULT_CONDITION_GROUP_H__

#include <atomic>
#include <functional>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

namespace isoft {
namespace uds {
namespace server {

/// @brief Condition class declaration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02451
/// @needwork = dd
/// @endcode
class Condition;

/// @brief ConditionGroup class
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02452
/// @needwork = dd
/// @endcode
class ConditionGroup final
{
public:
    /// @brief ConditionGroup copy constructor
    /// @param[in] other ConditionGroup reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00772
    /// @needwork = dda
    /// @endcode
    ConditionGroup(ConditionGroup const& other) = delete;
    /// @brief ConditionGroup move constructor
    /// @param[in] other ConditionGroup rvalue reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00773
    /// @needwork = dda
    /// @endcode
    ConditionGroup(ConditionGroup&& other) noexcept = delete;
    /// @brief ConditionGroup assignment operator
    /// @param[in] right ConditionGroup reference

    /// @return ConditionGroup reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00774
    /// @needwork = dda
    /// @endcode
    ConditionGroup& operator=(ConditionGroup const& right) = delete;
    /// @brief ConditionGroup move assignment operator
    /// @param[in] right ConditionGroup rvalue reference
    /// @return ConditionGroup reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00775
    /// @needwork = dda
    /// @endcode
    ConditionGroup& operator=(ConditionGroup&& right) noexcept = delete;

public:
    /// @brief Default constructor
    /// @code{.isoft}
    /// @throw unknown
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01990
    /// @needwork = dd
    /// @endcode
    ConditionGroup() = default;

    /// @brief Default destructor
    /// @code{.isoft}
    /// @throw unknown
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01991
    /// @needwork = dd
    /// @endcode
    ~ConditionGroup() = default;

    /// @brief Parse parameters from configuration
    /// @brief Initialize ConditionGroup
    /// @param[in] conditionList List of condition IDs contained in this group
    /// @param[in] conditions All conditions, conditions contained in this group will be found here
    /// @param[in] groupName Name of this group
    /// @return Returns true on success, false on failure
    /// @code{.isoft}
    /// @throw unknown
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01992
    /// @needwork = dd
    /// @endcode
    bool Init(std::set< uint32_t >& conditionList,
              std::map< uint32_t, std::shared_ptr< Condition > >& conditions,
              std::string const& groupName);

    /// @brief Check whether ConditionGroup is satisfied
    /// @return Returns true if satisfied, false if not satisfied
    /// @code{.isoft}
    /// @throw unknown
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01993
    /// @needwork = dd
    /// @endcode
    bool CheckConditionGroup() const noexcept;

    /// @brief Alias for callback function std::function<void(bool const fulfilled)>
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01484
    /// @needwork = dd
    /// @endcode
    using StatusChangedCallback = std::function< void(bool const fulfilled) >;

    /// @brief Set the callback function for this group state change
    /// @param[in] callback Callback function
    /// @code{.isoft}
    /// @throw unknown
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01994
    /// @needwork = dd
    /// @endcode
    void OnConditionGroupStatusChanged(StatusChangedCallback const& callback);

private:
    /// @brief Initialize callback function related logic
    /// @code{.isoft}
    /// @throw unknown
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01995
    /// @needwork = dd
    /// @endcode
    void _initCallbacks();

    /// @brief Current state of this group
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01485
    /// @needwork = dd
    /// @endcode
    std::atomic_bool currentState_{false};

    /// @brief All conditions in this group
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01486
    /// @needwork = dd
    /// @endcode
    std::vector< std::shared_ptr< Condition > > conditions_{};

    /// @brief Set state change callback function
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01487
    /// @needwork = dd
    /// @endcode
    std::vector< StatusChangedCallback > callbacks_{};
};

}  // namespace server
}  // namespace uds
}  // namespace isoft
#endif  // __ISOFT_UDS_FAULT_CONDITION_GROUP_H__