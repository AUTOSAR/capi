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
/// @file       public.h
/// @brief      This file provides public functionality
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
/// @trace_id_sr=0711f598-b180-4d87-b725-f5394814b30c
/// @unit_name=public
/// @unit_description=This file provides public functionality
/// @endcode
///
/// ================================================================

#ifndef __ISOFT_UDS_FAULT_PUBLIC_H__
#define __ISOFT_UDS_FAULT_PUBLIC_H__

#include <cstdint>
#include <memory>

#include "common/log.h"
#include "isoft/uds/common.h"
#include "isoft/uds/fault_management/fault_management.h"
#include "isoft/uds/fault_management/monitor_interface.h"

namespace isoft {
namespace uds {
namespace server {

/// @brief Valid data mask
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02482
/// @needwork = dd
/// @endcode
constexpr std::uint8_t kPandingData{0xFFU};

/// @brief FDC maximum value
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02483
/// @needwork = dd
/// @endcode
constexpr std::int8_t kFdcMax{127};

/// @brief FDC maximum value minus one
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02484
/// @needwork = dd
/// @endcode
constexpr std::int8_t kFdcMaxLess{126};

/// @brief FDC minimum value
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02485
/// @needwork = dd
/// @endcode
constexpr std::int8_t kFdcMin{-128};

/// @brief FDC minimum value plus one
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02486
/// @needwork = dd
/// @endcode
constexpr std::int8_t kFdcMinMore{-127};

/// @brief Invalid DebounceNodeValues ID
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02487
/// @needwork = dd
/// @endcode
constexpr std::uint32_t kInvalidDebounceID{0xFFFFFFFFU};
/// @brief Invalid Event ID
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02488
/// @needwork = dd
/// @endcode
constexpr std::uint32_t kInvalidEventID{0xFFFFFFFFU};
/// @brief Invalid DTC ID
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02489
/// @needwork = dd
/// @endcode
constexpr std::uint32_t kInvalidDtcID{0xFFFFFFFFU};
/// @brief Invalid ExtendedDataRecordNumber
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02490
/// @needwork = dd
/// @endcode
constexpr std::uint8_t kInvalidExtendedDataRecordNumber{0x00U};
/// @brief Invalid FreezeFrameRecordNumber
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02491
/// @needwork = dd
/// @endcode
constexpr std::uint32_t kInvalidFreezeFrameRecordNumber{0x00U};
/// @brief Invalid Indicator ID
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02492
/// @needwork = dd
/// @endcode
constexpr std::uint32_t kInvalidIndicatorID{0xFFFFFFFFU};
/// @brief Invalid Monitor ID
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02493
/// @needwork = dd
/// @endcode
constexpr std::uint32_t kInvalidMonitorID{0xFFFFFFFFU};
/// @brief Invalid OperationCycle ID
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02494
/// @needwork = dd
/// @endcode
constexpr std::uint32_t kInvalidOperationCycleID{0xFFFFFFFFU};

/// @brief DiagnosticCommonProps configuration parameter wrapper class
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02495
/// @needwork = dd
/// @endcode
class DemDiagnosticCommonProps final
{
public:
    /// @brief DebounceNodeValues default constructor
    /// @code{.isoft}
    /// @throw unknown
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00793
    /// @needwork = dda
    /// @endcode
    DemDiagnosticCommonProps() = delete;

    /// @brief DemDiagnosticCommonProps function
    /// @param const Parameter
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01622
    /// @needwork = dd
    /// @endcode
    explicit DemDiagnosticCommonProps(CommonProps const& commonProps) : commonProps_{commonProps} {}

    /// @brief DebounceNodeValues copy constructor
    /// @param[in] other DebounceNodeValues reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01623
    /// @needwork = dd
    /// @endcode
    DemDiagnosticCommonProps(DemDiagnosticCommonProps const& other) = default;

    /// @brief DebounceNodeValues move constructor
    /// @param[in] other DebounceNodeValues rvalue reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01624
    /// @needwork = dd
    /// @endcode
    DemDiagnosticCommonProps(DemDiagnosticCommonProps&& other) = default;

    /// @brief DebounceNodeValues assignment operator
    /// @param[in] right DebounceNodeValues reference
    /// @return DebounceNodeValues reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01625
    /// @needwork = dd
    /// @endcode
    DemDiagnosticCommonProps& operator=(DemDiagnosticCommonProps const& right) = default;

    /// @brief DebounceNodeValues move assignment operator
    /// @param[in] right DebounceNodeValues rvalue reference
    /// @return DebounceNodeValues reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01626
    /// @needwork = dd
    /// @endcode
    DemDiagnosticCommonProps& operator=(DemDiagnosticCommonProps&& right) = default;

    /// @brief DebounceNodeValues destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01627
    /// @needwork = dd
    /// @endcode
    ~DemDiagnosticCommonProps() = default;

    /// @brief Get whether to process aging cycle counter for every AgingCycle
    /// @return true or false
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01628
    /// @needwork = dd
    /// @endcode
    bool IsAgingRequiresTestedCycle() const noexcept { return commonProps_.agingRequiresTestedCycle; }

    /// @brief Get configured DiagnosticClearDtcLimitationEnum
    /// @return DiagnosticClearDtcLimitationEnum
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01629
    /// @needwork = dd
    /// @endcode
    DiagnosticClearDtcLimitationEnum GetClearDtcLimitation() const noexcept { return commonProps_.clearDtcLimitation; }

    /// @brief Get configured DiagnosticOccurrenceCounterProcessingEnum
    /// @return DiagnosticOccurrenceCounterProcessingEnum
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01630
    /// @needwork = dd
    /// @endcode
    DiagnosticOccurrenceCounterProcessingEnum GetOccurrenceCounterProcessing() const noexcept
    {
        return commonProps_.occurrenceCounterProcessing;
    }

    /// @brief Check whether to persist the "TestFailed" status bit
    /// @return true or false
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01631
    /// @needwork = dd
    /// @endcode
    bool IsStatusBitStorageTestFailed() const noexcept { return commonProps_.statusBitStorageTestFailed; }

    /// @brief Get whether to reset the "Confirm" status bit when Event Memory Entry is displaced
    /// @return true or false
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01632
    /// @needwork = dd
    /// @endcode
    bool IsResetConfirmedBitOnOverflow() const noexcept { return commonProps_.resetConfirmedBitOnOverflow; }

    /// @brief Get configured DiagnosticStatusBitHandlingTestFailedSinceLastClearEnum
    /// @return DiagnosticStatusBitHandlingTestFailedSinceLastClearEnum
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01633
    /// @needwork = dd
    /// @endcode
    DiagnosticStatusBitHandlingTestFailedSinceLastClearEnum GetStatusBitHandlingTestFailedSinceLastClear()
        const noexcept
    {
        return commonProps_.statusBitHandlingTestFailedSinceLastClear;
    }

private:
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01634
    /// @needwork = dd
    /// @endcode
    CommonProps commonProps_;
};

/// @brief Helper class in DEM module
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02496
/// @needwork = dd
/// @endcode
class Helper final
{
public:
    /// @brief Fill MetaInfoMap with dtc
    /// @param[in] dtc dtc value
    /// @param[in] metaInfo The MetaInfoMap to be filled
    /// @code{.isoft}
    /// @throw unknown
    /// @needwork = dd
    /// @endcode
    static void MakeMetaInfo(std::uint32_t const dtc, MetaInfoMap& metaInfo);
    /// @brief Get current timestamp, unit: millisecond
    /// @return Time
    /// @code{.isoft}
    /// @throw unknown
    /// @needwork = dd
    /// @endcode
    static std::uint64_t GetCurTime();

    /// @brief State MonitorAction as StringView, convenient for log output use
    /// @param[in] action The parameter to be converted
    /// @return StringView
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01635
    /// @needwork = dd
    /// @endcode
    static std::string ToString(MonitorAction const action) noexcept;
};

}  // namespace server
}  // namespace uds
}  // namespace isoft
#endif  // __ISOFT_UDS_FAULT_COMMON_H__