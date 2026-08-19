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
/// @file       public.cpp
/// @brief      This file implements public functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=fault_management
/// @trace_id_sr=0711f598-b180-4d87-b725-f5394814b30c
/// @unit_name=public
/// @unit_description=This file provides public functionality
/// @interface_level=unit
/// @endcode
///
/// ================================================================

#include "fault_management/public.h"

#include <nai/os/nai_system.h>

#include <array>
#include <string>

#include "isoft/uds/type_int.h"

namespace isoft {
namespace uds {
namespace server {



std::uint64_t Helper::GetCurTime() { return nai_time() / kInt32_1000U; }



std::string Helper::ToString(MonitorAction const action) noexcept
{
    std::string ret{""};
    switch (action) {
        case MonitorAction::kPassed: {
            ret = std::string("kPassed");
            break;
        }
        case MonitorAction::kFailed: {
            ret = std::string("kFailed");
            break;
        }
        case MonitorAction::kPrepassed: {
            ret = std::string("kPrepassed");
            break;
        }
        case MonitorAction::kPrefailed: {
            ret = std::string("kPrefailed");
            break;
        }
        case MonitorAction::kFdcThresholdReached: {
            ret = std::string("kFdcThresholdReached");
            break;
        }
        case MonitorAction::kResetTestFailed: {
            ret = std::string("kResetTestFailed");
            break;
        }
        case MonitorAction::kFreezeDebouncing: {
            ret = std::string("kFreezeDebouncing");
            break;
        }
        case MonitorAction::kResetDebouncing: {
            ret = std::string("kResetDebouncing");
            break;
        }
        default: {
            ret = std::string("");
            break;
        }
    }
    return ret;
}




void Helper::MakeMetaInfo(std::uint32_t const dtc, MetaInfoMap& metaInfo)
{
    std::uint8_t const kDtcSize{7U};
    std::array< char, kDtcSize > strDtc{};
    std::int32_t const retPrintf{std::snprintf(strDtc.data(), kDtcSize, "%02X%02X%02X",
                                               (dtc >> kInt32_16U) & kInt8_0xFFU, (dtc >> kInt32_8U) & kInt8_0xFFU,
                                               dtc & kInt8_0xFFU)};
    if (retPrintf < 0) {
        LogInfo() << "Helper::MakeMetaInfo -> std::snprintf failed. ret=" << retPrintf;
    }
    metaInfo[std::string("kDtc")]     = strDtc.data();      // [0-9A-F]{6}
    metaInfo[std::string("kContext")] = std::to_string(1);  // MetaInfo::Contenxt::kFaultMemory
}

}  // namespace server
}  // namespace uds
}  // namespace isoft
