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
/// @file       debounce_interface.cpp
/// @brief      This file implements debounce_interface functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=fault_management
/// @trace_id_sr=bfc788d1-ad11-4b84-b950-538ece9d76eb
/// @unit_name=DebounceInterface
/// @unit_description=This file provides debounce_interface functionality
/// @interface_level=unit
/// @endcode
///
/// ================================================================

#include "fault_management/debounce_interface.h"

#include "common/log.h"
#include "fault_management/event.h"
#include "fault_management/public.h"

namespace isoft {
namespace uds {
namespace server {


DebounceInterface::DebounceInterface(std::shared_ptr< Event > const& event, DebounceAlgorithmConfig& config) noexcept
{
    event_                  = std::weak_ptr< Event >(event);
    debounceCounterStorage_ = config.debounceCounterStorage;
    debounceBehavior_       = config.debounceBehavior;
}


std::int8_t DebounceInterface::GetFaultDetectionCounter() noexcept
{
    if (debounceCounter_.load() > kFdcMax) {
        debounceCounter_ = kFdcMax;
    }
    if (debounceCounter_.load() < kFdcMin) {
        debounceCounter_ = kFdcMin;
    }
    return static_cast< std::int8_t >(debounceCounter_.load());
}


DebouncingState DebounceInterface::GetDouncingState() noexcept
{
    std::int32_t const fdc{debounceCounter_.load()};
    if ((0 < fdc) && (fdc < kFdcMax)) {
        return DebouncingState::kTemporarilyDefective;
    }
    if (fdc >= kFdcMax) {
        return DebouncingState::kFinallyDefective;
    }
    if ((kFdcMin < fdc) && (fdc < 0)) {
        return DebouncingState::kTemporarilyHealed;
    }
    if (fdc <= kFdcMin) {
        return DebouncingState::kFinallyHealed;
    }
    // fdc == 0
    return DebouncingState::kNeutral;
}


void DebounceInterface::_NotifyEvent(MonitorAction const actionType)
{
    std::unique_lock< std::recursive_mutex > const lock{mutex_};
    std::shared_ptr< Event > event{event_.lock()};
    if (event != nullptr) {
        event->SetDebouncedStatus(actionType);
    }
}


void DebounceInterface::_NotifierFDCChanged()
{
    if (fdcChangeNotifier_ != nullptr) {
        fdcChangeNotifier_(GetFaultDetectionCounter());
    }
}


void DebounceInterface::_FireEventGetFDCFromMonitor()
{
    std::unique_lock< std::recursive_mutex > const lock{mutex_};
    std::shared_ptr< Event > event{event_.lock()};
    if (event != nullptr) {
        event->GetFaultDetectionCounterFromMonitor();
    }
}


void DebounceInterface::_SetFDC(std::int32_t const fdc, bool const notify)
{
    debounceCounter_ = fdc;
    if (notify) {
        _NotifierFDCChanged();
    }
}

void DebounceInterface::SetFDC(int8_t const fdc, bool const notify /* = true*/)
{
    debounceCounter_.store(static_cast< int32_t >(fdc));
    if (!notify) {
        return;
    }
    if (kFdcMax == debounceCounter_.load()) {
        _NotifyEvent(MonitorAction::kFailed);

    } else if (kFdcMin == debounceCounter_.load()) {
        _NotifyEvent(MonitorAction::kPassed);
    }
    _NotifierFDCChanged();
}
}  // namespace server
}  // namespace uds
}  // namespace isoft
