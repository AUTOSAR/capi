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
/// @file       control_dtc_setting.h
/// @brief      This file provides control_dtc_setting functionality
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
/// @trace_id_sr=e67ae07e-cf42-48e2-9cfe-1b92dfea4173
/// @unit_name=ControlDtcSetting
/// @unit_description=This file provides control_dtc_setting functionality
/// @endcode
///
/// ================================================================

#ifndef __ISOFT_UDS_FAULT_CONTROL_DTC_SETTING_H__
#define __ISOFT_UDS_FAULT_CONTROL_DTC_SETTING_H__

#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <vector>

namespace isoft {
namespace uds {
namespace server {

/// @brief control dtc setting wrapper class
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02497
/// @needwork = dd
/// @endcode
class ControlDtcSetting final
{
public:
    /// @brief ControlDtcSetting copy constructor
    /// @param[in] other ControlDtcSetting reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00794
    /// @needwork = dda
    /// @endcode
    ControlDtcSetting(ControlDtcSetting const& other) = delete;

    /// @brief ControlDtcSetting move constructor
    /// @param[in] other ControlDtcSetting rvalue reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00795
    /// @needwork = dda
    /// @endcode
    ControlDtcSetting(ControlDtcSetting&& other) = delete;

    /// @brief DemControlDtcSetting assignment operator
    /// @param[in] right ControlDtcSetting reference
    /// @return ControlDtcSetting reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00796
    /// @needwork = dda
    /// @endcode
    ControlDtcSetting& operator=(ControlDtcSetting const& right) = delete;

    /// @brief ControlDtcSetting move assignment operator
    /// @param[in] right ControlDtcSetting rvalue reference
    /// @return ControlDtcSetting reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00797
    /// @needwork = dda
    /// @endcode
    ControlDtcSetting& operator=(ControlDtcSetting&& right) = delete;

public:
    /// @brief ControlDtcSetting default constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01636
    /// @needwork = dd
    /// @endcode
    ControlDtcSetting() = default;

    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01637
    /// @needwork = dd
    /// @endcode
    ~ControlDtcSetting() noexcept;

    /// @brief Set status
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01638
    /// @needwork = dd
    /// @endcode
    int32_t Set(bool const on);

    /// @brief Get status
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01639
    /// @needwork = dd
    /// @endcode
    bool Get() const noexcept;

    /// @brief De-initialize, release related resources
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01640
    /// @needwork = dd
    /// @endcode
    void UnInit();

    /// @brief Callback function std::function<void(bool const)> alias
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01641
    /// @needwork = dd
    /// @endcode
    using StatusChangedCallback = std::function< void(bool const) >;

    /// @brief Set status change callback function
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01642
    /// @needwork = dd
    /// @endcode
    void SetNotifier(StatusChangedCallback const& callback);

private:
    /// @brief Current status
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01643
    /// @needwork = dd
    /// @endcode
    std::atomic_bool state_{true};

    /// @brief Whether to change state for the first time, the callback notification logic should also be triggered when the state does not change during the first state setting, so this parameter is introduced
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01644
    /// @needwork = dd
    /// @endcode
    std::atomic_bool first_{true};
    /// @brief Status change callback function
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01645
    /// @needwork = dd
    /// @endcode
    std::vector< StatusChangedCallback > callbacks_;

    /// @brief This parameter is introduced to protect data when reading and writing data in multiple threads
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01646
    /// @needwork = dd
    /// @endcode
    std::mutex lock_;
};

}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif  // __ISOFT_UDS_FAULT_CONTROL_DTC_SETTING_H__