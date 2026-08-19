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
/// @file       status.h
/// @brief      This file provides status functionality
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
/// @trace_id_sr=c43bac66-834d-441b-b809-1e90618a990b
/// @unit_name=status
/// @unit_description=This file provides status functionality
/// @endcode
///
/// ================================================================

#ifndef __ISOFT_UDS_FAULT_STATUS_H__
#define __ISOFT_UDS_FAULT_STATUS_H__

#include <bitset>
#include <cstdint>

#include "isoft/uds/type_int.h"

namespace isoft {
namespace uds {
namespace server {

/// @brief Diagnostic event status bits
/// @code{.isoft}
/// @needwork = dd
/// @endcode
enum class EventStatusBit : std::uint8_t
{
    kTestFailed,                         ///< bit 0: TestFailed
    kTestFailedThisOperationCycle,       ///< bit 1: TestFailedThisOperationCycle
    kTestNotCompletedThisOperationCycle  ///< bit 6: TestNotCompletedThisOperationCycle
};

/// @brief event status class
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02479
/// @needwork = dd
/// @endcode
class EventStatus final
{
public:
    /// @brief Constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01604
    /// @needwork = dd
    /// @endcode
    EventStatus() noexcept;

    /// @brief Get diagnostic event status
    /// @return Returns diagnostic event status value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01605
    /// @needwork = dd
    /// @endcode
    uint8_t Get() const noexcept;

    /// @brief Get the set information of the specified bit
    /// @param[in] at Diagnostic event status bit
    /// @return true/false Whether the specified bit is set
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01606
    /// @needwork = dd
    /// @endcode
    bool Get(EventStatusBit const at) const noexcept;

    /// @brief Set the specified bit of diagnostic event status
    /// @param[in] to Diagnostic event status bit
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01607
    /// @needwork = dd
    /// @endcode
    void Set(EventStatusBit const to) noexcept;

    /// @brief Clear the specified bit of diagnostic event status
    /// @param[in] to Diagnostic event status bit
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01608
    /// @needwork = dd
    /// @endcode
    void Reset(EventStatusBit const to) noexcept;

    /// @brief Clear diagnostic event status
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01609
    /// @needwork = dd
    /// @endcode
    void Clear() noexcept;

    /// @brief Implement the clear behavior specified by kOnlyThisCycleAndReadiness
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01610
    /// @needwork = dd
    /// @endcode
    void ClearOnThisCycleAndReadiness() noexcept;

    /// @brief Equality comparison
    /// @param[in] other Diagnostic event status
    /// @return true/false
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01611
    /// @needwork = dd
    /// @endcode
    bool operator==(EventStatus const& other) const noexcept { return eventStatus_ == other.eventStatus_; }

    /// @brief Inequality comparison
    /// @param[in] other Diagnostic event status
    /// @return true/false
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01612
    /// @needwork = dd
    /// @endcode
    bool operator!=(EventStatus const& other) const noexcept { return !(*this == other); }

private:
    /// @brief Get the position corresponding to the diagnostic event status bit
    /// @param[in] esb Diagnostic event status bit
    /// @return Returns the position corresponding to the diagnostic event status bit
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01613
    /// @needwork = dd
    /// @endcode
    static std::size_t BitPositionFrom(EventStatusBit const esb) noexcept;

    /// @brief Diagnostic event status size
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01614
    /// @needwork = dd
    /// @endcode
    static uint8_t const kDemEventStatusSize{7U};

    /// @name eventStatus_
    /// event status
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01615
    /// @needwork = dd
    /// @endcode
    std::bitset< kDemEventStatusSize > eventStatus_;
};

/// @brief Add a layer of processing to DTC status to facilitate future support for non-UDS
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02480
/// @needwork = dd
/// @endcode
enum class DTCStatusBit : int8_t
{
    kTestFailed                         = 0,
    kTestFailedThisOperationCycle       = 1,
    kPendingDTC                         = 2,
    kConfirmedDTC                       = 3,
    kTestNotCompletedSinceLastClear     = 4,
    kTestFailedSinceLastClear           = 5,
    kTestNotCompletedThisOperationCycle = 6,
    kWarningIndicatorRequested          = 7
};

/// @brief dtc status class
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02481
/// @needwork = dd
/// @endcode
class DTCStatus final
{
public:
    /// @brief Constructor
    /// @code{.isoft}
    /// @throw unknown
    /// @needwork = dd
    /// @endcode
    DTCStatus();

    /// @brief Constructor
    /// @param[in] val
    /// @code{.isoft}
    /// @throw unknown
    /// @needwork = dd
    /// @endcode
    explicit DTCStatus(uint8_t const val);

    /// @brief Get the current dtc status value
    /// @return Returns the current dtc status value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01616
    /// @needwork = dd
    /// @endcode
    uint8_t Get() const noexcept;

    /// @brief Return the set status of the specified bit of dtc
    /// @param[in] bitPosition DTC status bit
    /// @return true/false Whether set
    /// @code{.isoft}
    /// @throw
    /// @needwork = dd
    /// @endcode
    bool Get(DTCStatusBit const bitPosition) const;

    /// @brief Set the specified bit
    /// @param[in] bitPosition DTC status bit
    /// @code{.isoft}
    /// @throw
    /// @needwork = dd
    /// @endcode
    void Set(DTCStatusBit const bitPosition);

    /// @brief Clear the specified bit
    /// @param[in] bitPosition DTC status bit
    /// @code{.isoft}
    /// @throw
    /// @needwork = dd
    /// @endcode
    void Reset(DTCStatusBit const bitPosition) noexcept;

    /// @brief Clear dtc status
    /// @param[in] isWIRLock kWarningIndicatorRequested bit lock
    /// @code{.isoft}
    /// @throw unknown
    /// @needwork = dd
    /// @endcode
    void Clear(bool const isWIRLock);

    /// @brief Implement the clear behavior specified by kOnlyThisCycleAndReadiness
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01617
    /// @needwork = dd
    /// @endcode
    void ClearOnThisCycleAndReadiness() noexcept;

    /// @brief Determine whether any bit other than the given bits is set
    /// @param[in] bitPosition1 DTC status bit 1
    /// @param[in] bitPosition2 DTC status bit 2
    /// @return true/false
    /// @code{.isoft}
    /// @throw
    /// @needwork = dd
    /// @endcode
    bool IsSetOther(DTCStatusBit const bitPosition1, DTCStatusBit const bitPosition2) const noexcept;

    /// @brief MemoryOverflow function
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01618
    /// @needwork = dd
    /// @endcode
    void MemoryOverflow() noexcept;

    /// @brief dtc status size
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01619
    /// @needwork = dd
    /// @endcode
    static uint8_t const kStatusByteSize{kInt32_8U};

    /// @brief Define dtc status byte type
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01620
    /// @needwork = dd
    /// @endcode
    using StatusByteType = std::bitset< kStatusByteSize >;

    /// @brief Equality comparison
    /// @param[in] other
    /// @return true/false
    /// @code{.isoft}
    /// @throw
    /// @needwork = dd
    /// @endcode
    bool operator==(DTCStatus const& other) const noexcept { return currentStatus_ == other.currentStatus_; }

    /// @brief Inequality comparison
    /// @param[in] other
    /// @return true/false
    /// @code{.isoft}
    /// @throw
    /// @needwork = dd
    /// @endcode
    bool operator!=(DTCStatus const& other) const noexcept { return !(*this == other); }

private:
    /// @name currentStatus_
    /// dtc status
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01621
    /// @needwork = dd
    /// @endcode
    StatusByteType currentStatus_;
};

}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif  // __ISOFT_UDS_FAULT_STATUS_H__
