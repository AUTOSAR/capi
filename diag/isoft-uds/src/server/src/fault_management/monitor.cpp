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
/// @file       monitor.cpp
/// @brief      This file implements monitor functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=fault_management
/// @trace_id_sr=03389dbf-d8bc-4764-8e38-6c7dfcd33028
/// @unit_name=monitor
/// @unit_description=This file provides monitor functionality
/// @interface_level=unit
/// @endcode
///
/// ================================================================

#include "fault_management/monitor.h"

#include <string>

#include "common/log.h"
#include "fault_management/public.h"

namespace isoft {
namespace uds {
namespace server {





bool Monitor::Init(std::shared_ptr< MonitorInterface >& monitorInterface,
                   std::function< void(std::int8_t const) > const& setFDC)
{
    if (monitorInterface.get() == nullptr) {
        LogWarn() << "Monitor::Init|monitorInterface is nullptr";
        return false;
    }
    monitorInterface_ = monitorInterface;

    int32_t res = monitorInterface_->RegisterSetFaultDetectionCounter([this, setFDC](int8_t const fdc) {
        LogDebug() << "Monitor::Init|AA set fdc value =" << fdc;
        setFDC(fdc);
    });
    if (res != 0) {
        LogWarn() << "Monitor::Init|RegisterSetFaultDetectionCounter fail res =" << res;
        return false;
    }

    res = monitorInterface_->RegisterSetMonitorAction([this](MonitorAction const action) -> int32_t {
        LogDebug() << "Monitor::Init|AA set monitor action value =" << static_cast< uint32_t >(action);
        /// @brief monitorCallback_ function
        
        return monitorCallback_(action);
    });
    if (res != 0) {
        LogWarn() << "Monitor::Init|RegisterSetMonitorAction fail res =" << res;
        return false;
    }

    res = monitorInterface_->RegisterSetCounterBased([this](CounterBased const conterBased) {
        LogDebug() << "Monitor::Init|AA set conter based counterFailedThreshold =" << conterBased.counterFailedThreshold
                   << "counterPassedThreshold =" << conterBased.counterPassedThreshold
                   << "counterIncrementStepSize =" << conterBased.counterIncrementStepSize
                   << "counterDecrementStepSize =" << conterBased.counterDecrementStepSize
                   << "counterJumpUpValue =" << conterBased.counterJumpUpValue
                   << "counterJumpDownValue =" << conterBased.counterJumpDownValue
                   << "counterJumpUp =" << conterBased.counterJumpUp
                   << "counterJumpDown =" << conterBased.counterJumpDown;
        counterCallback_(conterBased);
    });
    if (res != 0) {
        LogWarn() << "Monitor::Init|RegisterSetCounterBased fail res =" << res;
        return false;
    }

    res = monitorInterface_->RegisterSetTimeBased([this](TimeBased const timeBased) {
        LogDebug() << "Monitor::Init|AA set time based timeFailedThreshold =" << timeBased.timeFailedThreshold
                   << "timePassedThreshold =" << timeBased.timePassedThreshold;
        timerCallback_(timeBased);
    });
    if (res != 0) {
        LogWarn() << "Monitor::Init|RegisterSetTimeBased fail res =" << res;
        return false;
    }

    return true;
}



void Monitor::InitMonitor(InitMonitorReason const reasonType) const
{
    LogVerbose() << "Monitor::InitMonitor -> enter. reason=" << static_cast< std::uint32_t >(reasonType);
    if (monitorInterface_.get() == nullptr) {
        LogWarn() << "Monitor::InitMonitor -> monitorInterface_ is null.";
        return;
    }

    int const res = monitorInterface_->NotifyInitMonitor(reasonType);
    LogDebug() << "Monitor::InitMonitor -> NotifyInitMonitor reason=" << static_cast< std::uint32_t >(reasonType)
               << "res =" << res;
}


void Monitor::GetFaultDetectionCounter() const
{
    if (monitorInterface_.get() == nullptr) {
        LogWarn() << "Monitor::GetFaultDetectionCounter -> monitorInterface_ is null.";
        return;
    }

    int32_t const res = monitorInterface_->NotifyFaultDetectionCounter();
    LogDebug() << "Monitor::GetFaultDetectionCounter -> NotifyFaultDetectionCounter res =" << res;
}



void Monitor::OnRecieveAction(std::function< int32_t(MonitorAction const&) > const& func)
{
    if (monitorInterface_.get() == nullptr) {
        LogWarn() << "Monitor::OnRecieveAction -> monitorInterface_ is nullptr";
        return;
    }

    if (nullptr == func) {
        LogWarn() << "Monitor::OnRecieveAction -> func is nullptr";
        return;
    }
    monitorCallback_ = func;
}

void Monitor::OnRecieveCounter(std::function< void(CounterBased const&) > const& func)
{
    if (nullptr == func) {
        LogWarn() << "Monitor::OnRecieveCounter -> func is nullptr";
        return;
    }

    counterCallback_ = func;
}

void Monitor::OnRecieveTimer(std::function< void(TimeBased const&) > const& func)
{
    if (nullptr == func) {
        LogWarn() << "Monitor::OnRecieveTimer -> func is nullptr";
        return;
    }

    timerCallback_ = func;
}

}  // namespace server
}  // namespace uds
}  // namespace isoft
