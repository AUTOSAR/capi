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
/// @file       memory_primary.cpp
/// @brief      This file implements memory_primary functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=fault_management
/// @trace_id_sr=52255098-0011-41d2-8639-9dd99a6e87c5
/// @unit_name=MemoryPrimary
/// @unit_description=This file provides memory_primary functionality
/// @interface_level=unit
/// @endcode
///
/// ================================================================

#include "fault_management/memory_primary.h"

#include "common/log.h"
#include "fault_management/fault_management_runtime_context.h"
#include "isoft/uds/type_int.h"

namespace isoft {
namespace uds {
namespace server {






bool MemoryPrimary::GetNumberOfDTCByStatusMask(std::uint8_t dtcStatusMask,
                                               std::uint8_t& dtcStatusAvailabilityMask,
                                               std::uint8_t& dtcFormatIdentifier,
                                               std::uint16_t& dtcCount) noexcept
{
    std::ignore = dtcStatusAvailabilityMask;
    std::ignore = dtcFormatIdentifier;
    std::ignore = dtcCount;
    LogVerbose() << "MemoryPrimary::GetNumberOfDTCByStatusMask from uds enter. dtcStatusMask=" << dtcStatusMask
                 << "memId =" << GetMemoryId() << "clusterName" << GetClusterName().c_str();
    dtcStatusAvailabilityMask = GetDtcStatusAvailabilityMask();
    dtcFormatIdentifier       = _dtcFormatIdFromTypeOfDtcSupported();
    dtcCount                  = static_cast< std::uint16_t >(GetEntrySize(dtcStatusMask, dtcStatusAvailabilityMask));
    return true;
}







bool MemoryPrimary::GetNumberOfDTCBySeverityMaskRecord(std::uint8_t dtcSeverityMask,
                                                       std::uint8_t dtcStatusMask,
                                                       std::uint8_t& dtcStatusAvailabilityMask,
                                                       std::uint8_t& dtcFormatIdentifier,
                                                       std::uint16_t& dtcCount) noexcept
{
    std::ignore = dtcStatusAvailabilityMask;
    std::ignore = dtcFormatIdentifier;
    std::ignore = dtcCount;
    LogVerbose() << "MemoryPrimary::GetNumberOfDTCBySeverityMaskRecord from uds enter. dtcSeverityMask="
                 << dtcSeverityMask << ", dtcStatusMask=" << dtcStatusMask << "memId =" << GetMemoryId()
                 << "clusterName" << GetClusterName().c_str();
    dtcStatusAvailabilityMask = GetDtcStatusAvailabilityMask();
    dtcFormatIdentifier       = _dtcFormatIdFromTypeOfDtcSupported();
    dtcCount = static_cast< std::uint16_t >(GetEntrySize(dtcStatusMask, dtcSeverityMask, dtcStatusAvailabilityMask));
    return true;
}





bool MemoryPrimary::GetSupportedDTC(std::uint8_t& dtcStatusAvailabilityMask,
                                    std::vector< DTCAndStatusRecord >& dtcAndStatusRecords)
{
    std::ignore = dtcStatusAvailabilityMask;
    LogVerbose() << "MemoryPrimary::GetSupportedDTC from uds enter dtcStatusAvailabilityMask ="
                 << dtcStatusAvailabilityMask << "memId =" << GetMemoryId()
                 << "clusterName =" << GetClusterName().c_str();
    dtcStatusAvailabilityMask = GetDtcStatusAvailabilityMask();
    GetEntriesByMask(dtcAndStatusRecords);
    return true;
}


std::uint8_t MemoryPrimary::_dtcFormatIdFromTypeOfDtcSupported() const noexcept
{
    std::uint8_t val{kInt8_0xFFU};
    decltype(auto) typeOfDtcSupported = GetTypeOfDtcSupported();
    switch (typeOfDtcSupported) {
        case DiagnosticTypeOfDtcSupportedEnum::kIso119924: {
            val = kInt8_3U;
            break;
        }
        case DiagnosticTypeOfDtcSupportedEnum::kIso142291: {
            val = kInt8_1U;
            break;
        }
        case DiagnosticTypeOfDtcSupportedEnum::kSaeJ2012Da: {
            val = 0U;
            break;
        }
        default: {
            LogWarn() << "MemoryPrimary::DtcFormatIdFromTypeOfDtcSupported -> invalid type of dtc supported."
                      << "type=" << static_cast< std::uint8_t >(typeOfDtcSupported);
            break;
        }
    }
    return val;
}
}  // namespace server
}  // namespace uds
}  // namespace isoft
