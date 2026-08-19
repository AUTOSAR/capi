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
/// @file       status_handler.h
/// @brief      This file provides status_handler functionality
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
/// @trace_id_sr=45a5d7ba-8189-4675-bb74-d450c6d3f2f5
/// @unit_name=StatusHandler
/// @unit_description=This file provides status_handler functionality
/// @endcode
///
/// ================================================================

#ifndef __ISOFT_UDS_FAULT_STATUS_HANDLER_H__
#define __ISOFT_UDS_FAULT_STATUS_HANDLER_H__

#include "fault_management/public.h"
#include "fault_management/status.h"
#include "isoft/uds/fault_management/fault_management.h"
#include "isoft/uds/fault_management/monitor_interface.h"

namespace isoft {
namespace uds {
namespace server {

/// @brief status handler interface
/// @code{.isoft}
/// @tparam Status of event or dtc
/// @interface_level=unit
/// @needwork=dd
/// @endcode
template < class Status >
class StatusHandlerInterface
{
public:
    /// @brief StatusHandlerInterface default constructor
    /// @code{.isoft}
    /// @throw
    /// @needwork = dd
    /// @endcode
    StatusHandlerInterface() = default;

    /// @brief StatusHandlerInterface copy constructor
    /// @param[in] other StatusHandlerInterface reference
    /// @code{.isoft}
    /// @throw
    /// @needwork = dd
    /// @endcode
    StatusHandlerInterface(StatusHandlerInterface const& other) = default;

    /// @brief StatusHandlerInterface move constructor
    /// @param[in] other StatusHandlerInterface rvalue reference
    /// @code{.isoft}
    /// @throw
    /// @needwork = dd
    /// @endcode
    StatusHandlerInterface(StatusHandlerInterface&& other) noexcept = default;

    /// @brief StatusHandlerInterface copy assignment operator
    /// @param[in] right StatusHandlerInterface reference
    /// @return StatusHandlerInterface reference
    /// @code{.isoft}
    /// @throw
    /// @needwork = dd
    /// @endcode
    StatusHandlerInterface& operator=(StatusHandlerInterface const& right) = default;

    /// @brief StatusHandlerInterface move assignment operator
    /// @param[in] right StatusHandlerInterface rvalue reference
    /// @return StatusHandlerInterface reference
    /// @code{.isoft}
    /// @throw
    /// @needwork = dd
    /// @endcode
    StatusHandlerInterface& operator=(StatusHandlerInterface&& right) noexcept = default;

    /// @brief StatusHandlerInterface destructor
    virtual ~StatusHandlerInterface() = default;

    /// @brief Reset interface
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01771
    /// @needwork = dd
    /// @endcode
    virtual void Reset() noexcept = 0;

    /// @brief Operation interface executed by DM when receiving a clear request when clear operation is prohibited
    /// @param[in] clearBehavior The operation to perform when clear operation is prohibited
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01772
    /// @needwork = dd
    /// @endcode
    virtual void ClearForbidden(DiagnosticClearEventAllowedBehaviorEnum const clearBehavior) noexcept = 0;

    /// @brief State change interface at operation cycle start
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01773
    /// @needwork = dd
    /// @endcode
    virtual void OpCycleStartedStatusChange() noexcept = 0;

    /// @brief State change interface at operation cycle end
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01774
    /// @needwork = dd
    /// @endcode
    virtual void OpCycleStoppedStatusChange() noexcept = 0;

    /// @brief Set callback function
    /// @param[in] notifier Notification callback function
    /// @code{.isoft}
    /// @throw
    /// @needwork = dd
    /// @endcode
    void SetNotifier(std::function< void(Status, Status) > const& notifier) { notifier_ = notifier; }

    /// @brief Get status
    /// @return Return status value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01775
    /// @needwork = dd
    /// @endcode
    Status const& GetStatus() const noexcept { return status_; }

protected:
    /// @brief Get status
    /// @return Return status value reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01776
    /// @needwork = dd
    /// @endcode
    Status& _GetStatus() noexcept { return status_; }

    /// @brief Status change notification
    /// @param[in] oldStatus
    /// @param[in] newStatus
    /// @code{.isoft}
    /// @throw
    /// @needwork = dd
    /// @endcode
    void _OnNotify(Status const oldStatus, Status const newStatus) const
    {
        if (notifier_ == nullptr) {
            return;
        }

        notifier_(oldStatus, newStatus);
    }

private:
    /// @name status_
    /// status of dtc or event
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01777
    /// @needwork = dd
    /// @endcode
    Status status_;
    /// @name notifier_
    /// notifier when status changed
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01778
    /// @needwork = dd
    /// @endcode
    std::function< void(Status, Status) > notifier_;
};

/// @brief event status handler class
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02529
/// @needwork = dd
/// @endcode
class EventStatusHandler : public StatusHandlerInterface< EventStatus >
{
public:
    /// @brief Set diagnostic event status based on monitor reported content
    /// @param[in] actionType The behavior reported by monitor in AA
    /// @param[in] recoverableInSameOperationCycle Whether fault recovery is performed in the same operation cycle
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01779
    /// @needwork = dd
    /// @endcode
    void SetStatus(MonitorAction const actionType, bool const recoverableInSameOperationCycle = true) noexcept;

    /// @brief Reset diagnostic event status, reset behavior based on [SWS_DM_01026]
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01780
    /// @needwork = dd
    /// @endcode
    void Reset() noexcept override;

    /// @brief Operation executed by DM when receiving a clear request when clear operation is prohibited
    /// @param[in] clearBehavior The operation to perform when clear operation is prohibited
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01781
    /// @needwork = dd
    /// @endcode
    void ClearForbidden(DiagnosticClearEventAllowedBehaviorEnum const clearBehavior) noexcept override;

    /// @brief Diagnostic event status change at operation cycle start
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01782
    /// @needwork = dd
    /// @endcode
    void OpCycleStartedStatusChange() noexcept override;

    /// @brief Diagnostic event status change at operation cycle end
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01783
    /// @needwork = dd
    /// @endcode
    void OpCycleStoppedStatusChange() noexcept override;

    /// @brief Clear diagnostic event status
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01784
    /// @needwork = dd
    /// @endcode
    void Clear() noexcept;
};

/// @brief dtc status handler class
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02530
/// @needwork = dd
/// @endcode
class DTCStatusHandler : public StatusHandlerInterface< DTCStatus >
{
public:
    /// @brief Set diagnostic DTC status based on monitor reported content
    /// @param[in] actionType The behavior reported by monitor in AA
    /// @param[in] failureCycleCountReached Whether the failure count has reached the threshold
    /// @param[in] indicatorLock Whether the WIR status bit is locked
    /// @param[in] hasIndicator Whether fault indicator is bound
    /// @param[in] recoverableInSameOperationCycle Whether fault recovery is performed in the same operation cycle
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01785
    /// @needwork = dd
    /// @endcode
    void SetStatus(MonitorAction const actionType,
                   bool const failureCycleCountReached,
                   bool const indicatorLock,
                   bool const hasIndicator,
                   bool const recoverableInSameOperationCycle) noexcept;

    /// @brief Status reset
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01786
    /// @needwork = dd
    /// @endcode
    void Reset() noexcept override;

    /// @brief Operation executed by DM when receiving a clear request when clear operation is prohibited
    /// @param[in] clearBehavior The operation to perform when clear operation is prohibited
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01787
    /// @needwork = dd
    /// @endcode
    void ClearForbidden(DiagnosticClearEventAllowedBehaviorEnum const clearBehavior) noexcept override;

    /// @brief State change at operation cycle start
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01788
    /// @needwork = dd
    /// @endcode
    void OpCycleStartedStatusChange() noexcept override;

    /// @brief State change at operation cycle end
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01789
    /// @needwork = dd
    /// @endcode
    void OpCycleStoppedStatusChange() noexcept override;

    /// @brief Status bit processing after operation cycle end
    /// @param[in] thresholdReached
    /// @param[in] testFailedSinceLastClear
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01790
    /// @needwork = dd
    /// @endcode
    void ProcessEndOfOperationCycle(bool const thresholdReached, bool const testFailedSinceLastClear) noexcept;

    /// @brief Set kWarningIndicatorRequested
    /// @param[in] b
    /// @code{.isoft}
    /// @throw
    /// @needwork = dd
    /// @endcode
    void SetStatusWIR(bool const b);

    /// @brief Clear status
    /// @param[in] isWIRLock Whether the WIR status bit is locked
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01791
    /// @needwork = dd
    /// @endcode
    void Clear(bool const isWIRLock) noexcept;

    /// @brief Reset status to status
    /// @param[in] status DTC status
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01792
    /// @needwork = dd
    /// @endcode
    void ResetTo(DTCStatus const& status) noexcept;
};

}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif  // __ISOFT_UDS_FAULT_STATUS_HANDLER_H__