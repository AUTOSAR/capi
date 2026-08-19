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
/// @file       memory_destination.h
/// @brief      This file provides memory_destination functionality
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
/// @trace_id_sr=5ceb3942-513a-4499-a6cd-7e3392ad0950
/// @unit_name=MemoryDestination
/// @unit_description=This file provides memory_destination functionality
/// @endcode
///
/// ================================================================

#ifndef __ISOFT_UDS_FAULT_MEMORY_DESTINATION_H__
#define __ISOFT_UDS_FAULT_MEMORY_DESTINATION_H__
#include <map>
#include <memory>
#include <mutex>
#include <string>

#include "fault_management/public.h"
#include "isoft/uds/fault_management/dtc_information_interface.h"
#include "isoft/uds/fault_management/fault_management.h"
namespace isoft {
namespace uds {
namespace server {

/// @brief Dtc forward declaration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02555
/// @needwork = dd
/// @endcode
class Dtc;

/// @brief DtcGroup forward declaration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02556
/// @needwork = dd
/// @endcode
class DtcGroup;

/// @brief MemoryEntry forward declaration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02557
/// @needwork = dd
/// @endcode
class MemoryEntry;

/// @brief FaultManagementRuntimeContext forward declaration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02558
/// @needwork = dd
/// @endcode
class FaultManagementRuntimeContext;

/// @brief PersistenceInterface forward declaration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02559
/// @needwork = dd
/// @endcode
class PersistenceInterface;

/// @brief DTCAndStatusRecord forward declaration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02560
/// @needwork = dd
/// @endcode
class DTCAndStatusRecord;

/// @brief DTCExtDataRecord forward declaration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02561
/// @needwork = dd
/// @endcode
class DTCExtDataRecord;

/// @brief Destination implementation base class, encapsulates common attributes and external interaction functions of Primary Destination and UserDefined Destination configured in configuration, such as: obtaining various data, allocating and saving data, cleaning, etc.
/// 
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02562
/// @needwork = dd
/// @endcode
class MemoryDestination : public std::enable_shared_from_this< MemoryDestination >
{
public:
    /// @brief MemoryDestination copy constructor
    /// @param[in] other MemoryDestination reference
    MemoryDestination(MemoryDestination const& other) = delete;

    /// @brief MemoryDestination move constructor
    /// @param[in] other MemoryDestination rvalue reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00816
    /// @needwork = dda
    /// @endcode
    MemoryDestination(MemoryDestination&& other) = delete;

    /// @brief MemoryDestination copy assignment operator
    /// @param[in] right MemoryDestination reference
    /// @return MemoryDestination reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00817
    /// @needwork = dda
    /// @endcode
    MemoryDestination& operator=(MemoryDestination const& right) = delete;

    /// @brief MemoryDestination move assignment operator
    /// @param[in] right MemoryDestination rvalue reference
    /// @return MemoryDestination reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00818
    /// @needwork = dda
    /// @endcode
    MemoryDestination& operator=(MemoryDestination&& right) = delete;

public:
    /// @brief delete MemoryDestination default constructor
    MemoryDestination() = delete;

    /// @brief MemoryDestination constructor with DemRuntimeContext parameter
    /// @param[in] context FaultManagementRuntimeContext reference
    /// @param[in] config MemoryDestinationConfig reference
    /// @param[in] clusterName std::string reference
    /// @param[in] per std::shared_ptr< PersistenceInterface > reference
    /// @return MemoryDestination reference
    MemoryDestination(FaultManagementRuntimeContext& context,
                      MemoryDestinationConfig& config,
                      std::string& clusterName,
                      std::shared_ptr< PersistenceInterface >& per) noexcept
        : context_{context}, config_{config}, clusterName_{clusterName}, per_{per}
    {
    }

    /// @brief MemoryDestination destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01904
    /// @needwork = dd
    /// @endcode
    virtual ~MemoryDestination() = default;

    /// @brief MemoryDestination initialization function
    /// @param[in] dtcList Diagnostic trouble code list
    /// @param[in] node Configuration node
    /// @param[in] skeleton Externally generated dtc skeleton bound to this destination
    /// @param[in] name Software set name, passed in here, convenient for use when using local storage in multiple places
    /// @param[in] storage Storage bound to this destination, used to store data
    /// @return Returns true on success, false on failure
    /// @code{.isoft}
    /// @throw unknown
    /// @needwork = dd
    /// @endcode
    virtual bool Init(std::vector< uint32_t > const& dtcList, std::vector< uint32_t > const& operationList);

    /// @brief interface Get the number of diagnostic trouble code data in storage that meet the filter criteria
    /// @param[in] dtcStatusMask Diagnostic trouble code status mask
    /// @param[out] dtcStatusAvailabilityMask Diagnostic module supported diagnostic trouble code status mask
    /// @param[out] dtcFormatIdentifier Diagnostic module supported diagnostic trouble code format standard
    /// @param[out] dtcCount Number of diagnostic trouble codes meeting the filter criteria
    /// @return Result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01905
    /// @needwork = dd
    /// @endcode
    virtual bool GetNumberOfDTCByStatusMask(std::uint8_t dtcStatusMask,
                                            std::uint8_t& dtcStatusAvailabilityMask,
                                            std::uint8_t& dtcFormatIdentifier,
                                            std::uint16_t& dtcCount) noexcept;

    /// @brief interface Get diagnostic trouble code information meeting the filter criteria
    /// @param[in] dtcStatusMask Diagnostic trouble code status mask
    /// @param[out] dtcStatusAvailabilityMask Diagnostic module supported diagnostic trouble code status mask
    /// @param[out] dtcAndStatusRecords Diagnostic trouble code dataset
    /// @return Result
    /// @code{.isoft}
    /// @throw unknown
    /// @needwork = dd
    /// @endcode
    virtual bool GetDTCByStatusMask(std::uint8_t dtcStatusMask,
                                    std::uint8_t& dtcStatusAvailabilityMask,
                                    std::vector< DTCAndStatusRecord >& dtcAndStatusRecords);

    /// @brief interface Get snapshot record of specified diagnostic trouble code
    /// @param[in] dtcMaskRecord Diagnostic trouble code value
    /// @param[in] ssrMumber Snapshot number
    /// @param[out] dtcAndStatusRecord Diagnostic trouble code data (value + status)
    /// @param[out] records Snapshot dataset
    /// @return Result
    /// @code{.isoft}
    /// @throw unknown
    /// @needwork = dd
    /// @endcode
    virtual bool GetDTCSnapshotRecordByDTCNumber(std::uint32_t dtcMaskRecord,
                                                 std::uint8_t ssrMumber,
                                                 DTCAndStatusRecord& dtcAndStatusRecord,
                                                 std::vector< SnapshotDataRecordType >& records);

    /// @brief interface Get extended data record of specified diagnostic trouble code
    /// @param[in] dtcMaskRecord Diagnostic trouble code value
    /// @param[in] edrNumber Extended data number
    /// @param[out] dtcAndStatusRecord Diagnostic trouble code data (value + status)
    /// @param[out] records Extended dataset
    /// @return Result
    /// @code{.isoft}
    /// @throw unknown
    /// @needwork = dd
    /// @endcode
    virtual bool GetDTCExtDataRecordByDTCNumber(std::uint32_t dtcMaskRecord,
                                                std::uint8_t edrNumber,
                                                DTCAndStatusRecord& dtcAndStatusRecord,
                                                std::vector< DTCExtDataRecord >& records);

    /// @brief interface Get the number of diagnostic trouble codes meeting the given filter criteria
    /// @param[in] dtcSeverityMask DTCSeverityMask diagnostic trouble code severity mask
    /// @param[in] dtcStatusMask DTCStatusMask diagnostic trouble code status mask
    /// @param[out] dtcStatusAvailabilityMask DTCStatusAvailabilityMask diagnostic trouble code status available bit mask
    /// @param[out] dtcFormatIdentifier DTCFormatIdentifier diagnostic trouble code format and encoding identifier
    /// @param[out] dtcCount Number of diagnostic trouble codes matching the filter criteria
    /// @return Result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01906
    /// @needwork = dd
    /// @endcode
    virtual bool GetNumberOfDTCBySeverityMaskRecord(std::uint8_t dtcSeverityMask,
                                                    std::uint8_t dtcStatusMask,
                                                    std::uint8_t& dtcStatusAvailabilityMask,
                                                    std::uint8_t& dtcFormatIdentifier,
                                                    std::uint16_t& dtcCount) noexcept;

    /// @brief interface Get the set of currently supported diagnostic trouble codes
    /// @param[out] dtcStatusAvailabilityMask DTCStatusAvailabilityMask diagnostic trouble code status available bit mask
    /// @param[out] dtcAndStatusRecords Diagnostic trouble code dataset
    /// @return Result
    /// @code{.isoft}
    /// @throw unknown
    /// @needwork = dd
    /// @endcode
    virtual bool GetSupportedDTC(std::uint8_t& dtcStatusAvailabilityMask,
                                 std::vector< DTCAndStatusRecord >& dtcAndStatusRecords);

    /// @brief Check whether this destination is primary
    /// @return Returns true if primary, otherwise false
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01907
    /// @needwork = dd
    /// @endcode
    bool IsPrimary() const noexcept;

    /// @brief When dtc status changes, call here to notify the proxy through the corresponding skeleton
    /// @param[in] dtc The dtc value whose status changed
    /// @param[in] oldStatus Old status
    /// @param[in] newStatus New status
    /// @code{.isoft}
    /// @throw unknown
    /// @needwork = dd
    /// @endcode
    void NotifyDtcStatus(std::uint32_t const dtc, std::uint8_t const oldStatus, std::uint8_t const newStatus);

    /// @brief When the snapshot record of dtc is updated, call here to notify the proxy through the corresponding skeleton
    /// @param[out] ssrs The updated snapshot records
    /// @param[in] dtc The dtc value to which the snapshot record belongs
    /// @code{.isoft}
    /// @throw unknown
    /// @needwork = dd
    /// @endcode
    void NotifySnapshotRecordUpdated(std::vector< SnapshotDataRecordType > const& ssrs, uint32_t const dtc);

    /// @brief Trigger save operation, called when dem module exits
    /// @code{.isoft}
    /// @throw unknown
    /// @needwork = dd
    /// @endcode
    void Save();

    /// @brief Get Entry by dtc value
    /// @param[in] dtc dtc value
    /// @return Returns the corresponding pointer if Entry is found, otherwise returns nullptr
    /// @code{.isoft}
    /// @throw unknown
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< MemoryEntry > GetEntry(uint32_t const dtc);

    /// @brief Attempt to allocate Entry to this destination, if already allocated, return the previously allocated one, if not allocated, trigger displacement strategy
    /// @param[in] dtc dtc value
    /// @param[in] status dtc status
    /// @param[in] severity dtc severity
    /// @param[out] isDisplacement Output parameter, whether displacement was triggered during allocation
    /// @param[in] priority dtc priority, displacement strategy requires this value
    /// @return Returns Entry if allocation is successful, otherwise returns nullptr
    /// @code{.isoft}
    /// @throw unknown
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< MemoryEntry > TryAllocEntry(uint32_t const dtc,
                                                 uint8_t const status,
                                                 bool& isDisplacement,
                                                 uint8_t const priority);

    /// @brief Persistently save Entry
    /// @param[in] entry The Entry to be saved
    /// @return Returns true on success, false on failure
    /// @code{.isoft}
    /// @throw unknown
    /// @needwork = dd
    /// @endcode
    bool SaveEntry(std::shared_ptr< MemoryEntry > const& entry, bool const isFellToDb = false);

    /// @brief Clear triggered by aging, modify dtc status from Entry
    /// @param[in] entry The Entry whose data is to be processed
    /// @param[in] isResetSinceLastClear Corresponds to the configuration in common props, when this value is true, the dtc status bit
    /// kTestFailedSinceLastClear will be reset
    /// @return Returns kSuccessful on success, corresponding error on failure
    /// @code{.isoft}
    /// @throw unknown
    /// @needwork = dd
    /// @endcode
    void ClearFromAging(std::shared_ptr< MemoryEntry > const& entry, bool const isResetSinceLastClear);

    /// @brief Implementation of clearing dtc group
    /// @param[in] group The dtc group to be cleared
    /// @param[in] isFromUds Whether this clear action is initiated from a UDS service
    /// @return ClearDtcErrorCodeEnum
    /// @code{.isoft}
    /// @throw unknown
    /// @needwork = dd
    /// @endcode
    int32_t Clear(std::shared_ptr< DtcGroup > const& group, bool const isFromUds);

    /// @brief Implementation of clearing a single dtc
    /// @param[in] dtc The dtc to be cleared
    /// @param[in] isFromUds Whether this clear action is initiated from a UDS service
    /// @return ClearDtcErrorCodeEnum
    /// @code{.isoft}
    /// @throw unknown
    /// @needwork = dd
    /// @endcode
    int32_t Clear(std::shared_ptr< Dtc > const& dtc, bool const isFromUds);

    /// @brief Get the configured parameter, displacement of this destination
    /// @return Displacement strategy
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01908
    /// @needwork = dd
    /// @endcode
    DiagnosticEventDisplacementStrategyEnum GetDisplacementStrategy() const noexcept
    {
        return config_.displacementStrategy;
    }

    /// @brief Get the configured parameter, mask of dtc status
    /// @return dtc status mask
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01909
    /// @needwork = dd
    /// @endcode
    uint8_t GetDtcStatusAvailabilityMask() const noexcept { return config_.dtcStatusAvailabilityMask; }

    /// @brief Get the configured parameter, the maximum number of Entries that can be stored in this destination
    /// @return Configured maximum value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01910
    /// @needwork = dd
    /// @endcode
    uint32_t GetMaxNumberOfEventEntries() const noexcept { return config_.maxNumberOfEventEntries; }

    /// @brief Get the configured parameter, trigger for Entry storage
    /// @return Entry storage trigger
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01911
    /// @needwork = dd
    /// @endcode
    DiagnosticMemoryEntryStorageTriggerEnum GetMemoryEntryStorageTrigger() const noexcept
    {
        return config_.memoryEntryStorageTrigger;
    }

    /// @brief Get the configured parameter, encoding type of freeze frame record
    /// @return Freeze frame record encoding type
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01912
    /// @needwork = dd
    /// @endcode
    DiagnosticTypeOfFreezeFrameRecordNumerationEnum GetTypeOfFreezeFrameRecordNumeration() const noexcept
    {
        return config_.typeOfFreezeFrameRecordNumeration;
    }

    /// @brief Whether it is full, determine overflow status
    /// @return Returns true if full, false if not full
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01913
    /// @needwork = dd
    /// @endcode
    bool IsOverflow() noexcept { return entries_.size() == config_.maxNumberOfEventEntries; }

    /// @brief Add operation cycle status to destination
    /// @param[in] idOc Operation cycle id
    /// @param[in] state Operation cycle status
    /// @code{.isoft}
    /// @throw unknown
    /// @needwork = dd
    /// @endcode
    void AddOperationCycleState(uint32_t const idOc, uint8_t const state);

    /// @brief Get the number of Entries in this destination
    /// @return The number of Entries in this destination
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01914
    /// @needwork = dd
    /// @endcode
    std::uint32_t GetCountForNumberOfEntries() noexcept;

    /// @brief Get the number of Entries in this destination, excluding suppressed DTCs
    /// @return The number of Entries in this destination
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01915
    /// @needwork = dd
    /// @endcode
    std::uint32_t GetCountForNumberOfEntriesFilterSuppression() noexcept;

    /// @brief Get the number of storage entries
    /// @return Number of storage entries
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01916
    /// @needwork = dd
    /// @endcode
    uint32_t GetEntrySize() noexcept;

    /// @brief Get the number of storage entries that match the specified status mask and status availability mask
    /// @param[in] statusMask dtc status mask
    /// @param[in] dtcStatusAvailabilityMask dtc status valid bit mask
    /// @return Number of storage entries
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01917
    /// @needwork = dd
    /// @endcode
    uint32_t GetEntrySize(std::uint8_t const statusMask, std::uint8_t const dtcStatusAvailabilityMask) noexcept;

    /// @brief Get the number of storage entries that match the specified status mask, severity mask and status availability mask
    /// @param[in] statusMask dtc status mask
    /// @param[in] dtcSeverityMask dtc severity mask
    /// @param[in] dtcStatusAvailabilityMask dtc status valid bit mask
    /// @return Number of storage entries
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01918
    /// @needwork = dd
    /// @endcode
    uint32_t GetEntrySize(std::uint8_t const statusMask,
                          std::uint8_t const dtcSeverityMask,
                          std::uint8_t const dtcStatusAvailabilityMask) noexcept;

    /// @brief Get DTC information that matches the specified status mask and status availability mask
    /// @param[out] dtcAndStatusRecords Diagnostic trouble code data (value + status)
    /// @code{.isoft}
    /// @throw
    /// @needwork = dd
    /// @endcode
    void GetEntriesByMask(std::vector< DTCAndStatusRecord >& dtcAndStatusRecords);

    /// @brief Get storage cluster name
    /// @return Storage cluster name
    /// @code{.isoft}
    /// @throw unknown
    /// @needwork = dd
    /// @endcode
    std::string GetClusterName() const { return clusterName_; }

    /// @brief Get database ID
    /// @return ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01919
    /// @needwork = dd
    /// @endcode
    std::uint32_t GetMemoryId() const { return config_.destinationID; }

    /// @brief Get supported DTC format type
    /// @return Type
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01920
    /// @needwork = dd
    /// @endcode
    DiagnosticTypeOfDtcSupportedEnum GetTypeOfDtcSupported() const { return config_.typeOfDtcSupported; }

    /// @brief Get dem runtime context
    /// @return Runtime context
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01921
    /// @needwork = dd
    /// @endcode
    FaultManagementRuntimeContext const& GetContext() const noexcept { return context_; }

protected:
    /// @brief Allocate storage entry space
    /// @param[in] dtc Diagnostic trouble code
    /// @param[in] status Diagnostic trouble code status
    /// @return Storage entry object
    /// @code{.isoft}
    /// @throw
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< MemoryEntry > _AllocEntry(uint32_t const dtc, uint8_t const status);

    /// @brief Clear storage entry for specified DTC
    /// @param[in] dtc Diagnostic trouble code
    /// @param[in] isResetStatus Whether to reset dtc status
    /// @return Clear result enumeration value
    /// @code{.isoft}
    /// @throw unknown
    /// @needwork = dd
    /// @endcode
    bool _ClearEntry(uint32_t const dtc, bool const isResetStatus);

    /// @brief Clear all storage entities
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01922
    /// @needwork = dd
    /// @endcode
    void _DeleteAllEntry();

    /// @brief Delete a single storage entity
    /// @param entry Entity
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01923
    /// @needwork = dd
    /// @endcode
    void _DeleteEntry(std::shared_ptr< MemoryEntry > const& entry);

    /// @brief Clear operation performed by DM when receiving a clear request when clear operation is prohibited
    /// @param[in] entry Storage entry object
    /// @param[in] clearBehavior The clear behavior to be performed
    /// @code{.isoft}
    /// @throw unknown
    /// @needwork = dd
    /// @endcode
    void _ClearEntryForbiddenStatus(std::shared_ptr< MemoryEntry > const& entry,
                                    DiagnosticClearEventAllowedBehaviorEnum const clearBehavior) const;

    /// @brief Get replacement entry, priority and occurrence time filter
    /// @param[in] priorityNew Diagnostic trouble code priority
    /// @param[in] context DEM runtime context
    /// @param[in] storage Persistent storage
    /// @param[in] clusterName Cluster name
    /// @param[in] dtc Diagnostic trouble code
    /// @param[in] status Diagnostic trouble code status
    /// @param[in] dtcSeverity Diagnostic trouble severity
    /// @return Storage entry object
    /// @code{.isoft}
    /// @throw
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< MemoryEntry > _DisplacementPrioOcc(uint8_t const priorityNew,
                                                        uint32_t const dtc,
                                                        uint8_t const status);

    /// @brief Get replacement entry, full strategy (priority, diagnostic event status, fault occurrence time)
    /// @param[in] priorityNew Diagnostic trouble code priority
    /// @param[in] context DEM runtime context
    /// @param[in] storage Persistent storage
    /// @param[in] clusterName Cluster name
    /// @param[in] dtc Diagnostic trouble code
    /// @param[in] status Diagnostic trouble code status
    /// @param[in] dtcSeverity Diagnostic trouble severity
    /// @return Storage entry object
    /// @code{.isoft}
    /// @throw
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< MemoryEntry > _DisplacementFull(uint8_t const priorityNew,
                                                     uint32_t const dtc,
                                                     uint8_t const status);

    /// @brief Store operation cycle
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01924
    /// @needwork = dd
    /// @endcode
    void _SaveOperation();

    /// @brief Build storage key for DTC
    /// @param[in] dtcValue Diagnostic trouble code
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01925
    /// @needwork = dd
    /// @endcode
    std::string _MakeDTCStroedKey(std::uint32_t const dtcValue) const noexcept;

    /// @brief Build key for operation cycle
    /// @param[in] operation Operation cycle ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01926
    /// @needwork = dd
    /// @endcode
    std::string _MakeOperationStroedKey(std::uint32_t const operation) const noexcept;

private:
    /// @brief Get the storage entry with the lowest priority
    /// @param[in] priorityNew Diagnostic trouble code priority
    /// @param[out] entries Storage entry vector
    /// @param[in] context DEM runtime context
    /// @code{.isoft}
    /// @throw
    /// @needwork = dd
    /// @endcode
    void _getLowestPriorityEntries(uint8_t priorityNew, std::vector< std::shared_ptr< MemoryEntry > >& entries);

    /// @brief Get storage entries with the same priority
    /// @param[in] priorityNew Diagnostic trouble code priority
    /// @param[out] entries Storage entry vector
    /// @param[in] context DEM runtime context
    /// @code{.isoft}
    /// @throw
    /// @needwork = dd
    /// @endcode
    void _getEqualPriorityEntries(uint8_t const priorityNew, std::vector< std::shared_ptr< MemoryEntry > >& entries);

    /// @brief Filter out diagnostic entries with diagnostic event status Passive
    /// @param[in] entries Storage entry vector
    /// @param[out] entriesResult Filtered storage entry vector
    /// @param[in] context DEM runtime context
    /// @code{.isoft}
    /// @throw
    /// @needwork = dd
    /// @endcode
    static void FilterPassiveEntries(std::vector< std::shared_ptr< MemoryEntry > > const& entries,
                                     std::vector< std::shared_ptr< MemoryEntry > >& entriesResult);

    /// @brief Filter out the set of entries that have not completed testing in the current operation cycle
    /// @param[in] entries Storage entry vector
    /// @param[out] entriesResult Filtered storage entry vector
    /// @code{.isoft}
    /// @throw
    /// @needwork = dd
    /// @endcode
    static void FilterTestNotComplateEntries(std::vector< std::shared_ptr< MemoryEntry > > const& entries,
                                             std::vector< std::shared_ptr< MemoryEntry > >& entriesResult);

    /// @brief Get the oldest storage entry
    /// @param[in] entries Storage entry vector
    /// @return Storage entry
    /// @code{.isoft}
    /// @throw
    /// @needwork = dd
    /// @endcode
    static std::shared_ptr< MemoryEntry > GetOldestEntry(std::vector< std::shared_ptr< MemoryEntry > >& entries);

    /// @brief DTC status handling during storage entry replacement
    /// @param[in] context DEM runtime context
    /// @param[in] status Diagnostic trouble code status
    /// @return DTC status after processing
    /// @code{.isoft}
    /// @throw
    /// @needwork = dd
    /// @endcode
    static uint8_t StatusDuringDisplacement(uint8_t const status, FaultManagementRuntimeContext const& context);

    /// @brief Comparison function, ascending by time
    /// @param[in] item1
    /// @param[in] item2
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01927
    /// @needwork = dd
    /// @endcode
    static bool CompareEntryByTimeUpOrder(std::shared_ptr< MemoryEntry > const& item1,
                                          std::shared_ptr< MemoryEntry > const& item2) noexcept;

    /// @brief Comparison function, descending by time
    /// @param[in] item1
    /// @param[in] item2
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01928
    /// @needwork = dd
    /// @endcode
    static bool CompareEntryByTimeDownOrder(std::shared_ptr< MemoryEntry > const& item1,
                                            std::shared_ptr< MemoryEntry > const& item2) noexcept;

private:
    /// @brief _initRegisterDtcSetting function
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01929
    /// @needwork = dd
    /// @endcode
    void _initRegisterDtcSetting();
    /// @brief _initRegisterClear function
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01930
    /// @needwork = dd
    /// @endcode
    int32_t _initRegisterClear();
    /// @brief _initRegisterGetNumberOfStoredEntries function
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01931
    /// @needwork = dd
    /// @endcode
    int32_t _initRegisterGetNumberOfStoredEntries();
    /// @brief _initRegisterGetControlDTCStatus function
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01932
    /// @needwork = dd
    /// @endcode
    int32_t _initRegisterGetControlDTCStatus();
    /// @brief _initRegisterEnableControlDtc function
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01933
    /// @needwork = dd
    /// @endcode
    int32_t _initRegisterEnableControlDtc();
    /// @brief _initRegisterGetCurrentStatus function
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01934
    /// @needwork = dd
    /// @endcode
    int32_t _initRegisterGetCurrentStatus();
    /// @brief _initRegisterGetEventMemoryOverflow function
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01935
    /// @needwork = dd
    /// @endcode
    int32_t _initRegisterGetEventMemoryOverflow();
    /// @brief _initRegisterGetDtcSuppression function
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01936
    /// @needwork = dd
    /// @endcode
    int32_t _initRegisterGetDtcSuppression();
    /// @brief _initRegisterSetDtcSuppression function
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01937
    /// @needwork = dd
    /// @endcode
    int32_t _initRegisterSetDtcSuppression();
    /// @brief _initLoadAllEntry function
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01938
    /// @needwork = dd
    /// @endcode
    uint32_t _initLoadAllEntry(std::vector< uint32_t > const& dtcList);
    /// @brief _initLoadAllOpenationStatus function
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01939
    /// @needwork = dd
    /// @endcode
    uint32_t _initLoadAllOpenationStatus(std::vector< uint32_t > const& operationList);

private:
    /// @name clearMutex_
    /// for thread safe
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01940
    /// @needwork = dd
    /// @endcode
    std::mutex clearMutex_;

    /// @name entries_
    /// MemoryEntry map
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01941
    /// @needwork = dd
    /// @endcode
    std::map< uint32_t, std::shared_ptr< MemoryEntry > > entries_;  // save
    /// @name ocAndStates_
    /// map of operation cycle and state
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01942
    /// @needwork = dd
    /// @endcode
    std::map< uint32_t, uint8_t > ocAndStates_;  // save

    /// @name context_
    /// context of runtime in dem
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01943
    /// @needwork = dd
    /// @endcode
    FaultManagementRuntimeContext& context_;

    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01944
    /// @needwork = dd
    /// @endcode
    MemoryDestinationConfig config_;

    /// @name clusterName_
    /// name of this cluster
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01945
    /// @needwork = dd
    /// @endcode
    std::string clusterName_;

    /// @name Memory
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01946
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< PersistenceInterface > per_;

    /// @name mutex_
    /// for thread safe
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01947
    /// @needwork = dd
    /// @endcode
    std::recursive_mutex entryMutex_;
    /// @name mutex_
    /// for thread safe
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01948
    /// @needwork = dd
    /// @endcode
    std::recursive_mutex operationMutex_;
};

}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif  // __ISOFT_UDS_FAULT_MEMORY_DESTINATION_H__