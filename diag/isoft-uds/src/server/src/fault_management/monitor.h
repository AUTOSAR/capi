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
/// @file       monitor.h
/// @brief      This file provides monitor functionality
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
/// @trace_id_sr=03389dbf-d8bc-4764-8e38-6c7dfcd33028
/// @unit_name=monitor
/// @unit_description=This file provides monitor functionality
/// @endcode
///
/// ================================================================

#ifndef __ISOFT_UDS_FAULT_MONITOR_H__
#define __ISOFT_UDS_FAULT_MONITOR_H__

#include <functional>
#include <memory>

#include "isoft/uds/fault_management/monitor_interface.h"

namespace isoft {
namespace uds {
namespace server {

/// @brief Monitor class
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02567
/// @needwork = dd
/// @endcode
class Monitor final
{
public:
    /// @brief Initialization function
    /// @param[in] instanceID Instance ID
    /// @param[in] setFDC FDC callback
    /// @return Success/Failure
    /// @code{.isoft}
    /// @throw
    /// @needwork = dd
    /// @endcode
    bool Init(std::shared_ptr< MonitorInterface >& monitorInterface,
              std::function< void(std::int8_t const) > const& setFDC);

    /// @brief Initialize Monitor
    /// @param[in] reasonType Initialization reason type
    /// @code{.isoft}
    /// @throw
    /// @needwork = dd
    /// @endcode
    void InitMonitor(InitMonitorReason const reasonType) const;

    /// @brief Get anomaly detection count
    /// @code{.isoft}
    /// @throw
    /// @needwork = dd
    /// @endcode
    void GetFaultDetectionCounter() const;

    /// @brief Register receive callback function
    /// @param[in] func
    /// @code{.isoft}
    /// @throw
    /// @needwork = dd
    /// @endcode
    void OnRecieveAction(std::function< int32_t(MonitorAction const&) > const& func);

    /// @brief Register receive callback function
    /// @param const Parameter
    /// @code{.isoft}
    /// @throw
    /// @needwork = dd
    /// @endcode
    void OnRecieveCounter(std::function< void(CounterBased const&) > const& func);

    /// @brief Register receive callback function
    /// @param const Parameter
    /// @code{.isoft}
    /// @throw
    /// @needwork = dd
    /// @endcode
    void OnRecieveTimer(std::function< void(TimeBased const&) > const& func);

private:
    /// @brief Monitor callback variable
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01965
    /// @needwork = dd
    /// @endcode
    std::function< int32_t(MonitorAction const&) > monitorCallback_{};

    /// @brief Counter callback variable
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @needwork = dd
    /// @endcode
    std::function< void(CounterBased const&) > counterCallback_{};

    /// @brief Timer callback variable
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01966
    /// @needwork = dd
    /// @endcode
    std::function< void(TimeBased const&) > timerCallback_{};

    /// @name skeleton_
    /// pointer of MonitorSkeletonImpl
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01967
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< MonitorInterface > monitorInterface_;
};

}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif  // __ISOFT_UDS_FAULT_MONITOR_H__