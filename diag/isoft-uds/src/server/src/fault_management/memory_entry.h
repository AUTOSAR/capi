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
/// @file       memory_entry.h
/// @brief      This file provides memory_entry functionality
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
/// @trace_id_sr=41be708c-b25e-40c7-ac9f-e7bbb42e2c54
/// @unit_name=MemoryEntry
/// @unit_description=This file provides memory_entry functionality
/// @endcode
///
/// ================================================================

#ifndef __ISOFT_UDS_FAULT_MEMORY_INFO_H_
#define __ISOFT_UDS_FAULT_MEMORY_INFO_H_

#include <atomic>
#include <map>
#include <memory>
#include <mutex>
#include <vector>

#include "fault_management/public.h"
#include "fault_management/types.h"
#include "isoft/uds/fault_management/dtc_information_interface.h"
#include "isoft/uds/type_int.h"
#include "serialization/serialization.h"
namespace isoft {
namespace uds {
namespace server {

/// @brief Default dtc status
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02500
/// @needwork = dd
/// @endcode
std::uint8_t const kDefaultDtcStatus{0x80U};

/// @brief Fault management runtime context
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02501
/// @needwork = dd
/// @endcode
class FaultManagementRuntimeContext;

/// @brief Freeze frame record
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02502
/// @needwork = dd
/// @endcode
class MemoryEntryFreezeRecord final
{
public:
    /// @brief did
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01662
    /// @needwork = dd
    /// @endcode
    uint16_t did{0};  // did
    /// @brief Data corresponding to did
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01663
    /// @needwork = dd
    /// @endcode
    std::vector< uint8_t > vecData{};  // data corresponding to did

    /// Define an alias voide
    using IsEnumerableTag = void;
    /// @brief enumerate function
    /// @param[in] func The function to be called
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01664
    /// @needwork = dd
    /// @endcode
    template < typename F >
    /// @brief enumerate function
    void enumerate(F& func)  // NOLINT
    {
        func(did);
        func(vecData);
    }
};

/// @brief Extended data record
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02503
/// @needwork = dd
/// @endcode
class MemoryEntryExtendedRecord final
{
public:
    /// @brief Extended data number
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01665
    /// @needwork = dd
    /// @endcode
    std::uint32_t id{0};  // data element id
    /// @brief Data corresponding to extended data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01666
    /// @needwork = dd
    /// @endcode
    std::vector< uint8_t > vecData{};  // data corresponding to did
    /// @code{.isoft}
    /// Define an alias void
    /// @needwork = dd
    /// @endcode
    using IsEnumerableTag = void;
    /// @brief enumerate function
    /// @param[in] func The function to be called
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01667
    /// @needwork = dd
    /// @endcode
    template < typename F >
    /// @brief enumerate function
    void enumerate(F& func)  // NOLINT
    {
        func(id);
        func(vecData);
    }
};

/// @brief Data structure of dtc storage entity
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02504
/// @needwork = dd
/// @endcode
class MemoryEntryData final
{
public:
    /// @name time
    /// time whem alloc this entry
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01668
    /// @needwork = dd
    /// @endcode
    uint64_t time{0UL};  // save

    /// @name occurrenceCounter
    /// occurrence counter
    // [SWS_DM_00945]
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01669
    /// @needwork = dd
    /// @endcode
    std::uint8_t occurrenceCounter{1U};  // save

    /// @name fdc
    /// fault detected counter
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01670
    /// @needwork = dd
    /// @endcode
    int8_t fdc{0};  // save

    /// @name dtcStatus
    /// Initial value of status of dtc
    /// Binary is 01010000, decimal is 80, hexadecimal is 50
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01671
    /// @needwork = dd
    /// @endcode
    std::uint8_t dtcStatus{kDefaultDtcStatus};  // save

    /// @name agingCounter_
    /// counter of aging
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01672
    /// @needwork = dd
    /// @endcode
    uint32_t agingCounter{0U};

    /// @brief Freeze frame specific content, key: freezeRecordNumber ; value: specific freeze frame data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01673
    /// @needwork = dd
    /// @endcode
    std::map< uint8_t, std::vector< MemoryEntryFreezeRecord > > mapFreezeRecord{};

    /// @brief Extended data key: extended data number value: corresponding specific extended data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01674
    /// @needwork = dd
    /// @endcode
    std::map< uint8_t, std::vector< MemoryEntryExtendedRecord > > mapExtendedDataRecord{};

    /// @brief Clear function
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01675
    /// @needwork = dd
    /// @endcode
    void Clear()
    {
        mapFreezeRecord.clear();
        mapExtendedDataRecord.clear();
        fdc               = 0;
        occurrenceCounter = 0U;
        time              = 0UL;
        agingCounter      = 0U;
    }
    /// @brief Define an alias void
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01676
    /// @needwork = dd
    /// @endcode
    using IsEnumerableTag = void;
    /// @brief enumerate function
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01997
    /// @needwork = dd
    /// @endcode
    template < typename F >
    /// @brief enumerate function
    void enumerate(F& func)  // NOLINT
    {
        func(time);
        func(occurrenceCounter);
        func(fdc);
        func(dtcStatus);
        func(agingCounter);
        func(mapFreezeRecord);
        func(mapExtendedDataRecord);
    }
};

/// @brief Data structure of storage entity
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02505
/// @needwork = dd
/// @endcode
class MemoryEntry final
{
public:
    /// @brief Constructor
    /// @param[in] context Runtime context
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01677
    /// @needwork = dd
    /// @endcode
    explicit MemoryEntry(FaultManagementRuntimeContext& context) noexcept : context_{context} {}

    /// @brief Clear storage entry
    /// @param[in] storage Persistent storage
    /// @param[in] memId Memory ID
    /// @param[in] clusterName Cluster name
    /// @return Result
    /// @code{.isoft}
    /// @throw
    /// @needwork = dd
    /// @endcode
    void Clear();

    /// @brief Find freeze frame data record
    /// @param[in] record Record number
    /// @return Freeze frame data record object
    /// @code{.isoft}
    /// @throw
    /// @needwork = dd
    /// @endcode
    void FindFreezeFrameDataRecord(uint8_t const record, std::vector< SnapshotDataRecordType >& records);

    /// @brief Allocate freeze frame storage
    /// @param[in] storage Persistent storage
    /// @param[in] memId Memory ID
    /// @param[in] clusterName Cluster name
    /// @param[in] ssr Snapshot record
    /// @param[in] typeOfFreezeFrameRecordNumeration Freeze frame number type
    /// @return Freeze frame record object
    /// @code{.isoft}
    /// @throw
    /// @needwork = dd
    /// @endcode
    void AllocFreezeFrameFrom(std::uint8_t const recordNumber, std::vector< MemoryEntryFreezeRecord > const& ff);

    /// @brief Find extended data record
    /// @param[in] record Record number
    /// @return Extended data record object
    /// @code{.isoft}
    /// @throw
    /// @needwork = dd
    /// @endcode
    void FindExtendedDataRecord(uint8_t const record,
                                std::vector< DTCExtDataRecord >& records,
                                std::uint32_t const memId);

    /// @brief Allocate extended data record storage
    /// @param[in] storage Persistent storage
    /// @param[in] memId Memory ID
    /// @param[in] clusterName Cluster name
    /// @param[in] edrt Extended data record object
    /// @return Extended data record object
    /// @code{.isoft}
    /// @throw
    /// @needwork = dd
    /// @endcode
    void AllocExtendedDataRecordFrom(std::uint8_t const recordNumber,
                                     std::vector< MemoryEntryExtendedRecord > const& edr);

    /// @brief Get diagnostic trouble code
    /// @return Diagnostic trouble code
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01678
    /// @needwork = dd
    /// @endcode
    uint32_t GetDtcValue() const noexcept { return dtcValue_; }

    /// @brief Get fault occurrence time
    /// @return Time
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01679
    /// @needwork = dd
    /// @endcode
    uint64_t GetTime() const noexcept { return entryData_.time; }

    /// @brief Get number of freeze frames
    /// @return Number of freeze frames
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01680
    /// @needwork = dd
    /// @endcode
    uint32_t GetFFSize() noexcept;

    /// @brief Get diagnostic trouble code status
    /// @return Diagnostic trouble code status
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01681
    /// @needwork = dd
    /// @endcode
    uint8_t GetDtcStatus() const noexcept;

    /// @brief Get diagnostic trouble code severity
    /// @return Diagnostic trouble code severity
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01682
    /// @needwork = dd
    /// @endcode
    uint8_t GetDtcSeverity() const noexcept;

    /// @brief Get fdc
    /// @return fdc value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01683
    /// @needwork = dd
    /// @endcode
    int8_t GetFDC() const noexcept { return entryData_.fdc; }

    /// @brief Get DTC priority
    /// @param[in] context DEM runtime context
    /// @return Priority
    /// @code{.isoft}
    /// @throw
    /// @needwork = dd
    /// @endcode
    uint8_t GetPriority() const;

    /// @brief Determine whether it is passive state
    /// @param[in] context DEM runtime context
    /// @return
    /// @code{.isoft}
    /// @throw
    /// @needwork = dd
    /// @endcode
    bool IsPassive() const;

    /// @brief Set diagnostic trouble code
    /// @param[in] val Diagnostic trouble code
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01684
    /// @needwork = dd
    /// @endcode
    void SetDtcValue(uint32_t const val) noexcept { dtcValue_ = val; }

    /// @brief Set fault occurrence time
    /// @param[in] timeValue Time
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01685
    /// @needwork = dd
    /// @endcode
    void SetTime(uint64_t const timeValue) noexcept { entryData_.time = timeValue; }

    /// @brief Set diagnostic trouble code status
    /// @param[in] status Diagnostic trouble code status
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01686
    /// @needwork = dd
    /// @endcode
    void SetDtcStatus(uint8_t const status) noexcept { entryData_.dtcStatus = status; }

    /// @brief Set fdc value
    /// @param[in] fdc fdc value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01687
    /// @needwork = dd
    /// @endcode
    void SetFDC(int8_t const fdc) noexcept { entryData_.fdc = fdc; }

    /// @brief Get aging count value
    /// @return Aging count value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01688
    /// @needwork = dd
    /// @endcode
    uint32_t GetAgingCount() const noexcept { return entryData_.agingCounter; }

    /// @brief Increment aging count
    /// @return Current aging count value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01689
    /// @needwork = dd
    /// @endcode
    uint32_t AgingIncrement() noexcept
    {
        ++entryData_.agingCounter;
        return entryData_.agingCounter;
    }

    /// @brief Reset aging count
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01690
    /// @needwork = dd
    /// @endcode
    void AgingReset() noexcept { entryData_.agingCounter = 0U; }

    /// @brief Get number of fault occurrences
    /// @return Number of fault occurrences
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01691
    /// @needwork = dd
    /// @endcode
    uint8_t GetOccurrenceCounter() const noexcept;

    /// @brief Increment number of fault occurrences
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01692
    /// @needwork = dd
    /// @endcode
    void OccurrenceCounterIncrement() noexcept;

    /// @brief Reset number of fault occurrences
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01693
    /// @needwork = dd
    /// @endcode
    void OccurrenceCounterReset() noexcept { entryData_.occurrenceCounter = 0U; }

public:
    /// @brief Serialize
    /// @param[out] serializedMessage
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01694
    /// @needwork = dd
    /// @endcode
    void Serialize(std::vector< uint8_t >& serializedMessage);

    /// @brief Deserialize
    /// @param[in] serializedMessage
    /// @return true if successful
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01695
    /// @needwork = dd
    /// @endcode
    bool Deserialize(std::vector< uint8_t > const& serializedMessage);

private:
    /// @name dtcValue_
    /// value of dtc
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01696
    /// @needwork = dd
    /// @endcode
    uint32_t dtcValue_{0U};

    /// @brief Storage entity data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01697
    /// @needwork = dd
    /// @endcode
    MemoryEntryData entryData_;

    /// @name mutex_
    /// for thread safe
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01698
    /// @needwork = dd
    /// @endcode

    /// @endcode
    mutable std::recursive_mutex mutex_;

    /// @brief Runtime context
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01699
    /// @needwork = dd
    /// @endcode
    FaultManagementRuntimeContext& context_;
};

}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif  // ARA_DIAG_DEM_MEMORY_INFO_H_