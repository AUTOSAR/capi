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
/// @brief      This file provides the definition of Monitor and related types.
/// @details
/// @date       2024-12-09
/// @author     zhanglipeng
/// @version    1.2.0
///
/// ================================================================

#ifndef __ARA_DIAG_FAULT_MONITOR_H__
#define __ARA_DIAG_FAULT_MONITOR_H__
#include <isoft/uds/fault_management/monitor_interface.h>

#include "gen_code/monitor/serviceAgent/monitor_agent.h"

namespace ara {
namespace diag {
namespace dmd {

/// @brief monitor interface
class Monitor : public isoft::uds::server::MonitorInterface
{
public:
    Monitor(Monitor const&) = delete;
    Monitor(Monitor&&)      = delete;
    Monitor& operator=(Monitor const&) = delete;
    Monitor& operator=(Monitor&&) = delete;
    explicit Monitor(uint32_t const& serviceInstanceId) noexcept;
    ~Monitor() noexcept override = default;

public:
    /// @brief Notify AA side that monitor needs to be reinitialized
    /// @param reasonType Initialization reason
    /// @return Returns 0 for successful call
    std::int32_t NotifyInitMonitor(isoft::uds::server::InitMonitorReason const reasonType) override;

    /// @brief Notify AA side to get FDC
    /// @param iFdc fdc
    /// @return Returns 0 for successful call
    std::int32_t NotifyFaultDetectionCounter() override;

    /// @brief Register callback for AA side to report events
    /// @param callBack Callback function
    /// @return Returns 0 for successful registration
    std::int32_t RegisterSetMonitorAction(isoft::uds::server::SetMonitorActionCallBack const& callBack) override;

    /// @brief Register callback for AA side to set FDC
    /// @param callBack Callback function
    /// @return Returns 0 for successful registration
    std::int32_t RegisterSetFaultDetectionCounter(
        isoft::uds::server::SetFaultDetectionCounterCallBack const& callBack) override;

    /// @brief Register callback for AA side to set counter-based debounce parameters
    /// @param callBack Callback function
    /// @return Returns 0 for successful registration
    std::int32_t RegisterSetCounterBased(isoft::uds::server::SetCounterBasedCallBack const& callBack) override;

    /// @brief Register callback for AA side to set timer-based debounce parameters
    /// @param callBack Callback function
    /// @return Returns 0 for successful registration
    std::int32_t RegisterSetTimeBased(isoft::uds::server::SetTimeBasedCallBack const& callBack) override;

private:
    std::unique_ptr< isoft::dm::dis::MonitorAgent > monitorAgent_;

    isoft::uds::server::SetMonitorActionCallBack monitorActionCb_{nullptr};
    isoft::uds::server::SetFaultDetectionCounterCallBack fdcCb_{nullptr};
    isoft::uds::server::SetCounterBasedCallBack counterCb_{nullptr};
    isoft::uds::server::SetTimeBasedCallBack timeCb_{nullptr};
};

}  // namespace dmd
}  // namespace diag
}  // namespace ara

#endif  // DM_UDS_GENERIC_UDS_SERVICE_H_