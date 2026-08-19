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
/// @file       status_handler.cpp
/// @brief      This file implements status_handler functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=fault_management
/// @trace_id_sr=45a5d7ba-8189-4675-bb74-d450c6d3f2f5
/// @unit_name=StatusHandler
/// @unit_description=This file provides status_handler functionality
/// @interface_level=unit
/// @endcode
///
/// ================================================================

#include "fault_management/status_handler.h"

namespace isoft {
namespace uds {
namespace server {


void EventStatusHandler::SetStatus(MonitorAction const actionType, bool const recoverableInSameOperationCycle) noexcept
{
    EventStatus const statusOld{_GetStatus()};
    // [SWS_DM_01025].
    if (actionType == MonitorAction::kFailed) {
        _GetStatus().Set(EventStatusBit::kTestFailed);
        _GetStatus().Set(EventStatusBit::kTestFailedThisOperationCycle);
        _GetStatus().Reset(EventStatusBit::kTestNotCompletedThisOperationCycle);
    } else {
        if (actionType == MonitorAction::kPassed) {
            if (recoverableInSameOperationCycle) {
                if (_GetStatus().Get(EventStatusBit::kTestFailedThisOperationCycle)) {
                    _GetStatus().Reset(EventStatusBit::kTestFailed);
                }
            }
            _GetStatus().Reset(EventStatusBit::kTestFailedThisOperationCycle);
            _GetStatus().Reset(EventStatusBit::kTestNotCompletedThisOperationCycle);
        }
    }

    if (statusOld != _GetStatus()) {
        _OnNotify(statusOld, _GetStatus());
    }
}

void EventStatusHandler::Reset() noexcept
{
    // [SWS_DM_01026].
    EventStatus const statusOld{_GetStatus()};
    _GetStatus().Reset(EventStatusBit::kTestFailed);

    if (statusOld != _GetStatus()) {
        _OnNotify(statusOld, _GetStatus());
    }
}

void EventStatusHandler::Clear() noexcept
{
    EventStatus const statusOld{_GetStatus()};
    _GetStatus().Clear();

    if (statusOld != _GetStatus()) {
        _OnNotify(statusOld, _GetStatus());
    }
}


void EventStatusHandler::ClearForbidden(DiagnosticClearEventAllowedBehaviorEnum const clearBehavior) noexcept
{
    EventStatus const statusOld{_GetStatus()};
    switch (clearBehavior) {
        case DiagnosticClearEventAllowedBehaviorEnum::kNoStatusByteChange: {
            break;
        }
        case DiagnosticClearEventAllowedBehaviorEnum::kOnlyThisCycleAndReadiness: {
            _GetStatus().ClearOnThisCycleAndReadiness();
            break;
        }
        default: {
        } break;
    }

    if (statusOld != _GetStatus()) {
        _OnNotify(statusOld, _GetStatus());
    }
}

void EventStatusHandler::OpCycleStartedStatusChange() noexcept
{
    // [SWS_DM_01027].
    EventStatus const statusOld{_GetStatus()};
    _GetStatus().Reset(EventStatusBit::kTestFailedThisOperationCycle);
    // ISO14229-1 Figure D.7 — DTC status bit 6 testNotCompletedThisOperationCycle logic
    _GetStatus().Set(EventStatusBit::kTestNotCompletedThisOperationCycle);

    if (statusOld != _GetStatus()) {
        _OnNotify(statusOld, _GetStatus());
    }
}

void EventStatusHandler::OpCycleStoppedStatusChange() noexcept
{
    // _OnNotify(_GetStatus());.
}






void DTCStatusHandler::SetStatus(MonitorAction const actionType,
                                 bool const failureCycleCountReached,
                                 bool const indicatorLock,
                                 bool const hasIndicator,
                                 bool const recoverableInSameOperationCycle) noexcept
{
    DTCStatus const statusOld{_GetStatus()};
    std::ignore = indicatorLock;
    // [SWS_DM_00883].
    if (actionType == MonitorAction::kFailed) {
        _GetStatus().Set(DTCStatusBit::kTestFailed);
        _GetStatus().Set(DTCStatusBit::kTestFailedThisOperationCycle);
        _GetStatus().Set(DTCStatusBit::kPendingDTC);

        if (failureCycleCountReached) {
            _GetStatus().Set(DTCStatusBit::kConfirmedDTC);
        }

        _GetStatus().Set(DTCStatusBit::kTestFailedSinceLastClear);
        _GetStatus().Reset(DTCStatusBit::kTestNotCompletedSinceLastClear);
        _GetStatus().Reset(DTCStatusBit::kTestNotCompletedThisOperationCycle);
        if (hasIndicator) {
            if (_GetStatus().Get(DTCStatusBit::kConfirmedDTC)) {
                _GetStatus().Set(DTCStatusBit::kWarningIndicatorRequested);
            }
        }
    } else {
        if (actionType == MonitorAction::kPassed) {
            /* recoverableInSameOperationCycle
            If the attribute is set to true then reporting PASSED will
            reset the indication of a failed test in the current operation
            cycle. If the attribute is set to false then reporting
            PASSED will be ignored and not lead to a reset of the
            indication of a failed test.*/
            if (recoverableInSameOperationCycle || !_GetStatus().Get(DTCStatusBit::kTestFailedThisOperationCycle)) {
                _GetStatus().Reset(DTCStatusBit::kTestFailed);
            }

            _GetStatus().Reset(DTCStatusBit::kTestNotCompletedSinceLastClear);
            _GetStatus().Reset(DTCStatusBit::kTestNotCompletedThisOperationCycle);
        }
    }

    if (statusOld != _GetStatus()) {
        _OnNotify(statusOld, _GetStatus());
    }
}

void DTCStatusHandler::Reset() noexcept
{
    DTCStatus const statusOld{_GetStatus()};
    _GetStatus().Reset(DTCStatusBit::kTestFailed);

    if (statusOld != _GetStatus()) {
        _OnNotify(statusOld, _GetStatus());
    }
}


void DTCStatusHandler::Clear(bool const isWIRLock) noexcept
{
    DTCStatus const statusOld{_GetStatus()};
    // [SWS_DM_00217].
    _GetStatus().Clear(isWIRLock);

    if (statusOld != _GetStatus()) {
        _OnNotify(statusOld, _GetStatus());
    }
}


void DTCStatusHandler::ResetTo(DTCStatus const& status) noexcept
{
    DTCStatus const statusOld{_GetStatus()};
    _GetStatus() = status;

    if (statusOld != _GetStatus()) {
        _OnNotify(statusOld, _GetStatus());
    }
}


void DTCStatusHandler::ClearForbidden(DiagnosticClearEventAllowedBehaviorEnum const clearBehavior) noexcept
{
    DTCStatus const statusOld{_GetStatus()};
    switch (clearBehavior) {
        case DiagnosticClearEventAllowedBehaviorEnum::kNoStatusByteChange: {
            break;
        }
        case DiagnosticClearEventAllowedBehaviorEnum::kOnlyThisCycleAndReadiness: {
            _GetStatus().ClearOnThisCycleAndReadiness();
            break;
        }
        default: {
        } break;
    }
    if (statusOld != _GetStatus()) {
        _OnNotify(statusOld, _GetStatus());
    }
}

void DTCStatusHandler::OpCycleStartedStatusChange() noexcept
{
    DTCStatus const statusOld{_GetStatus()};
    _GetStatus().Reset(DTCStatusBit::kTestFailedThisOperationCycle);
    _GetStatus().Set(DTCStatusBit::kTestNotCompletedThisOperationCycle);

    if (statusOld != _GetStatus()) {
        _OnNotify(statusOld, _GetStatus());
    }
}

void DTCStatusHandler::OpCycleStoppedStatusChange() noexcept
{
    DTCStatus const statusOld{_GetStatus()};
    if (!_GetStatus().Get(DTCStatusBit::kTestFailedThisOperationCycle)) {
        if (!_GetStatus().Get(DTCStatusBit::kTestNotCompletedThisOperationCycle)) {
            _GetStatus().Reset(DTCStatusBit::kPendingDTC);
        }
    }

    if (statusOld != _GetStatus()) {
        _OnNotify(statusOld, _GetStatus());
    }
}



void DTCStatusHandler::ProcessEndOfOperationCycle(bool const thresholdReached,
                                                  bool const testFailedSinceLastClear) noexcept
{
    DTCStatus const statusOld{_GetStatus()};
    if (thresholdReached) {
        _GetStatus().Reset(DTCStatusBit::kConfirmedDTC);
    }
    if (testFailedSinceLastClear) {
        _GetStatus().Reset(DTCStatusBit::kTestFailedSinceLastClear);
    }

    // _GetStatus().Reset(DTCStatusBit::kPendingDTC);.

    if (statusOld != _GetStatus()) {
        _OnNotify(statusOld, _GetStatus());
    }
}



void DTCStatusHandler::SetStatusWIR(bool const b)
{
    if (b) {
        _GetStatus().Set(DTCStatusBit::kWarningIndicatorRequested);
    } else {
        _GetStatus().Reset(DTCStatusBit::kWarningIndicatorRequested);
    }
}
}  // namespace server
}  // namespace uds
}  // namespace isoft
