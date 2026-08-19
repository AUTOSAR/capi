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
/// @file       status.cpp
/// @brief      This file implements status functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=fault_management
/// @trace_id_sr=c43bac66-834d-441b-b809-1e90618a990b
/// @unit_name=status
/// @unit_description=This file provides status functionality
/// @interface_level=unit
/// @endcode
///
/// ================================================================

#include "fault_management/status.h"

#include "fault_management/public.h"
#include "isoft/uds/type_int.h"

namespace isoft {
namespace uds {
namespace server {
EventStatus::EventStatus() noexcept
{
    std::ignore = eventStatus_.reset();
    Set(EventStatusBit::kTestNotCompletedThisOperationCycle);
}


uint8_t EventStatus::Get() const noexcept { return static_cast< uint8_t >(eventStatus_.to_ulong()); }



bool EventStatus::Get(EventStatusBit const at) const noexcept { return eventStatus_[BitPositionFrom(at)]; }


void EventStatus::Set(EventStatusBit const to) noexcept
{
    std::ignore = eventStatus_.set(static_cast< size_t >(BitPositionFrom(to)));
}


void EventStatus::Reset(EventStatusBit const to) noexcept
{
    std::ignore = eventStatus_.reset(static_cast< size_t >(BitPositionFrom(to)));
}

void EventStatus::Clear() noexcept
{
    Reset(EventStatusBit::kTestFailed);
    Reset(EventStatusBit::kTestFailedThisOperationCycle);
    Set(EventStatusBit::kTestNotCompletedThisOperationCycle);
}

void EventStatus::ClearOnThisCycleAndReadiness() noexcept
{
    Reset(EventStatusBit::kTestFailedThisOperationCycle);
    Set(EventStatusBit::kTestNotCompletedThisOperationCycle);
}



std::size_t EventStatus::BitPositionFrom(EventStatusBit const esb) noexcept
{
    std::size_t bp{0U};
    switch (esb) {
        case EventStatusBit::kTestFailedThisOperationCycle: {
            bp = kInt8_1U;
            break;
        }
        case EventStatusBit::kTestNotCompletedThisOperationCycle: {
            bp = kInt8_6U;
            break;
        }
        default: {
            bp = 0U;
            break;
        }
    }
    return bp;
}


DTCStatus::DTCStatus()
{
    std::ignore = currentStatus_.reset();
    Set(DTCStatusBit::kTestNotCompletedSinceLastClear);
    Set(DTCStatusBit::kTestNotCompletedThisOperationCycle);
}



DTCStatus::DTCStatus(uint8_t const val)
{
    for (uint32_t pos{0U}; pos < kInt32_8U; ++pos) {
        std::ignore = currentStatus_.set(static_cast< size_t >(pos),
                                         static_cast< bool >(val & static_cast< uint8_t >(1U << pos)));
    }
}


uint8_t DTCStatus::Get() const noexcept { return static_cast< uint8_t >(currentStatus_.to_ulong()); }



bool DTCStatus::Get(DTCStatusBit const bitPosition) const { return currentStatus_[static_cast< size_t >(bitPosition)]; }



void DTCStatus::Set(DTCStatusBit const bitPosition)
{
    std::ignore = currentStatus_.set(static_cast< size_t >(bitPosition));
}


void DTCStatus::Reset(DTCStatusBit const bitPosition) noexcept
{
    std::ignore = currentStatus_.reset(static_cast< size_t >(bitPosition));
}



void DTCStatus::Clear(bool const isWIRLock)
{
    Reset(DTCStatusBit::kTestFailed);
    Reset(DTCStatusBit::kTestFailedThisOperationCycle);
    Reset(DTCStatusBit::kPendingDTC);
    Reset(DTCStatusBit::kConfirmedDTC);
    Set(DTCStatusBit::kTestNotCompletedSinceLastClear);
    Reset(DTCStatusBit::kTestFailedSinceLastClear);
    Set(DTCStatusBit::kTestNotCompletedThisOperationCycle);
    if (!isWIRLock) {
        Reset(DTCStatusBit::kWarningIndicatorRequested);
    }
}

void DTCStatus::ClearOnThisCycleAndReadiness() noexcept
{
    Reset(DTCStatusBit::kTestFailedThisOperationCycle);
    Set(DTCStatusBit::kTestNotCompletedSinceLastClear);
    Reset(DTCStatusBit::kTestFailedSinceLastClear);
    Set(DTCStatusBit::kTestNotCompletedThisOperationCycle);
}


bool DTCStatus::IsSetOther(DTCStatusBit const bitPosition1, DTCStatusBit const bitPosition2) const noexcept
{
    StatusByteType statusBT{currentStatus_};
    std::ignore = statusBT.reset(static_cast< size_t >(bitPosition1));
    std::ignore = statusBT.reset(static_cast< size_t >(bitPosition2));
    return statusBT.any();
}

void DTCStatus::MemoryOverflow() noexcept { Reset(DTCStatusBit::kTestFailedSinceLastClear); }
}  // namespace server
}  // namespace uds
}  // namespace isoft
