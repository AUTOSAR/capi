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
/// @file       fault_management_runtime_context.h
/// @brief      This file provides fault_management_runtime_context functionality
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
/// @trace_id_sr=3769719b-0140-4437-b341-afd828d203d0
/// @unit_name=FaultManagementRuntimeContext
/// @unit_description=This file provides fault_management_runtime_context functionality
/// @endcode
///
/// ================================================================

#ifndef __ISOFT_UDS_FAULT_RUNTIME_CONTEXT_H__
#define __ISOFT_UDS_FAULT_RUNTIME_CONTEXT_H__

#include <map>
#include <string>
#include <vector>

#include "data_management/diagnostic_data_manager.h"
#include "isoft/uds/fault_management/fault_management.h"

namespace isoft {
namespace uds {
namespace server {

/// @brief ControlDtcSetting class forward declaration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02506
/// @needwork = dd
/// @endcode
class ControlDtcSetting;

/// @brief DemDiagnosticCommonProps class forward declaration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02507
/// @needwork = dd
/// @endcode
class DemDiagnosticCommonProps;

/// @brief Dtc class forward declaration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02508
/// @needwork = dd
/// @endcode
class Dtc;

/// @brief DtcProps class forward declaration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02509
/// @needwork = dd
/// @endcode
class DtcProps;

/// @brief DtcGroup class forward declaration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02510
/// @needwork = dd
/// @endcode
class DtcGroup;

/// @brief Event class forward declaration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02511
/// @needwork = dd
/// @endcode
class Event;

/// @brief Monitor class forward declaration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02512
/// @needwork = dd
/// @endcode
class Monitor;

/// @brief ControlDtcSetting class forward declaration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02513
/// @needwork = dd
/// @endcode
class ControlDtcSetting;

/// @brief MemoryDestination class forward declaration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02514
/// @needwork = dd
/// @endcode
class MemoryDestination;

/// @brief FreezeFrame class forward declaration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02515
/// @needwork = dd
/// @endcode
class FreezeFrame;

/// @brief DataIdentifierSet class forward declaration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02516
/// @needwork = dd
/// @endcode
class DataIdentifierSet;

/// @brief ExtendedDataRecord class forward declaration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02517
/// @needwork = dd
/// @endcode
class ExtendedDataRecord;

/// @brief Indicator class forward declaration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02518
/// @needwork = dd
/// @endcode
class Indicator;

/// @brief Condition class forward declaration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02519
/// @needwork = dd
/// @endcode
class Condition;

/// @brief ConditionGroup class forward declaration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02520
/// @needwork = dd
/// @endcode
class ConditionGroup;

/// @brief OperationCycle class forward declaration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02521
/// @needwork = dd
/// @endcode
class OperationCycle;

/// @brief Aging class forward declaration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02522
/// @needwork = dd
/// @endcode
class Aging;

/// @brief runtime context class
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02523
/// @needwork = dd
/// @endcode
class FaultManagementRuntimeContext final : public std::enable_shared_from_this< FaultManagementRuntimeContext >
{
public:
    /// @brief FaultManagementRuntimeContext copy constructor
    /// @param[in] other FaultManagementRuntimeContext reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01700
    /// @needwork = dd
    /// @endcode
    FaultManagementRuntimeContext(FaultManagementRuntimeContext const& other) = default;
    /// @brief FaultManagementRuntimeContext move constructor
    /// @param[in] other FaultManagementRuntimeContext rvalue reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01701
    /// @needwork = dd
    /// @endcode
    FaultManagementRuntimeContext(FaultManagementRuntimeContext&& other) = default;
    /// @brief DemRuntimeContext assignment operator
    /// @param[in] right FaultManagementRuntimeContext reference
    /// @return FaultManagementRuntimeContext reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01702
    /// @needwork = dd
    /// @endcode
    FaultManagementRuntimeContext& operator=(FaultManagementRuntimeContext const& right) = default;
    /// @brief FaultManagementRuntimeContext move assignment operator
    /// @param[in] right FaultManagementRuntimeContext rvalue reference
    /// @return FaultManagementRuntimeContext reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01703
    /// @needwork = dd
    /// @endcode
    FaultManagementRuntimeContext& operator=(FaultManagementRuntimeContext&& right) = default;

public:
    /// @brief delete FaultManagementRuntimeContext default constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01704
    /// @needwork = dd
    /// @endcode
    FaultManagementRuntimeContext() = default;
    /// @brief FaultManagementRuntimeContext destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01705
    /// @needwork = dd
    /// @endcode
    ~FaultManagementRuntimeContext() = default;

public:
    /// @brief Initialization function
    /// @param[in] dmc Configuration object
    /// @param[in] storage Persistent storage
    /// @return Initialization result
    /// @code{.isoft}
    /// @throw
    /// @needwork = dd
    /// @endcode
    std::int32_t Initialize(FaultManagement& config,
                            std::shared_ptr< PersistenceInterface >& per,
                            std::shared_ptr< DiagnosticDataManager >& dataManager);

    /// @brief Start service
    /// @return Result
    /// @code{.isoft}
    /// @throw
    /// @needwork = dd
    /// @endcode
    int32_t Start();

    /// @brief Stop service
    /// @code{.isoft}
    /// @throw
    /// @needwork = dd
    /// @endcode
    void Stop();

    /// @brief Get common properties object
    /// @return Common properties object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01706
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< DemDiagnosticCommonProps > GetCommonProps() const noexcept { return commonProps_; }

    /// @brief Get DTC (diagnostic trouble code) object
    /// @param[in] id DTC number
    /// @return DTC object
    /// @code{.isoft}
    /// @throw
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< Dtc > const GetDtc(uint32_t const id) const;

    /// @brief Get DTC (diagnostic trouble code) object
    /// @param[in] dtcValue DTC value
    /// @return DTC object
    /// @code{.isoft}
    /// @throw
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< Dtc > GetDtcByValue(uint32_t const dtcValue) const;

    /// @brief Get DTC (diagnostic trouble code) group object
    /// @param[in] groupNumber Group ID
    /// @return DTC group object
    /// @code{.isoft}
    /// @throw
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< DtcGroup > GetDtcGroup(uint32_t const groupNumber) const;

    /// @brief Get all DTC objects
    /// @return DTC object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01707
    /// @needwork = dd
    /// @endcode
    std::vector< std::shared_ptr< Dtc > > GetAllDtc() noexcept;

    /// @brief Get control fault code setting object
    /// @return Control fault code setting object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01708
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< ControlDtcSetting > GetDtcSetting() const noexcept { return dtcSetting_; }

    /// @brief Get cluster name
    /// @return Cluster name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01709
    /// @needwork = dd
    /// @endcode
    std::string GetClusterName() const noexcept { return clusterName_; }

    /// @brief Get memory object corresponding to specified ID
    /// @param[in] id Memory ID
    /// @return Memory object
    /// @code{.isoft}
    /// @throw
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< MemoryDestination > GetDestination(uint32_t const id) const;

    /// @brief Get primary memory
    /// @return Primary memory object
    /// @code{.isoft}
    /// @throw
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< MemoryDestination > GetPrimaryDestination() const;

    /// @brief Get freeze frame object corresponding to specified freeze frame number
    /// @param[in] number Freeze frame number
    /// @return Freeze frame object
    /// @code{.isoft}
    /// @throw
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< FreezeFrame > GetFreezeFrame(std::uint32_t const number) const;

    /// @brief Get extended data record object corresponding to specified extended data record number
    /// @param[in] number Extended data record number
    /// @return Extended data record object
    /// @code{.isoft}
    /// @throw
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< ExtendedDataRecord > GetExtendDataRecord(std::uint32_t const number);

    /// @brief Get all dtcs in primary storage
    /// @return dtc list
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01710
    /// @needwork = dd
    /// @endcode
    std::set< uint32_t >& GetPrimaryDtcList() { return inPrimaryDtcList_; }

private:
    /// @brief Read common properties
    /// @param[in] dmc Configuration object
    /// @return Success/Failure
    /// @code{.isoft}
    /// @throw
    /// @needwork = dd
    /// @endcode
    bool _createCommonProperties(FaultManagement& config);

    /// @brief Build configured DID dataset
    /// @param[in] dmc Configuration object
    /// @return Success/Failure
    /// @code{.isoft}
    /// @throw
    /// @needwork = dd
    /// @endcode
    bool _createConfiguredDids(FaultManagement& config, std::shared_ptr< DiagnosticDataManager >& dataManager);

    /// @brief Create DTC group
    /// @param[in] dmc Configuration object
    /// @return Success/Failure
    /// @code{.isoft}
    /// @throw
    /// @needwork = dd
    /// @endcode
    bool _createDTCGroups(FaultManagement& config, std::shared_ptr< PersistenceInterface >& per);

    /// @brief Create condition enable group
    /// @param[in] dmc Configuration object
    /// @return Success/Failure
    /// @code{.isoft}
    /// @throw
    /// @needwork = dd
    /// @endcode
    bool _createConditionGroups(FaultManagement& config);

    /// @brief Create operation cycle
    /// @param[in] dmc Configuration object
    /// @return Success/Failure
    /// @code{.isoft}
    /// @throw
    /// @needwork = dd
    /// @endcode
    bool _createOperationCycles(FaultManagement& config);

    /// @brief Create diagnostic event object and complete related item mapping
    /// @param[in] dmc Configuration object
    /// @return Success/Failure
    /// @code{.isoft}
    /// @throw
    /// @needwork = dd
    /// @endcode
    bool _createAndMapEvents(FaultManagement& config);

    /// @brief Define default group
    /// @code{.isoft}
    /// @throw
    /// @needwork = dd
    /// @endcode
    void _defineDefaultGroups();

private:
    /// @name events_
    /// Event map
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01711
    /// @needwork = dd
    /// @endcode
    std::map< uint32_t, std::shared_ptr< Event > > events_{};

    /// @name operationCycles_
    /// OperationCycle map
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01712
    /// @needwork = dd
    /// @endcode
    std::map< uint32_t, std::shared_ptr< OperationCycle > > operationCycles_{};

    /// @name agings_
    /// Aging map
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01713
    /// @needwork = dd
    /// @endcode
    std::map< uint32_t, std::shared_ptr< Aging > > agings_{};

    /// @name dtcProps_
    /// DtcProps map
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01714
    /// @needwork = dd
    /// @endcode
    std::map< uint32_t, std::shared_ptr< DtcProps > > dtcProps_{};

    /// @name dtcs_
    /// Dtc map
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01715
    /// @needwork = dd
    /// @endcode
    std::map< uint32_t, std::shared_ptr< Dtc > > dtcs_{};

    /// @name dtcValueMapping_
    /// Dtc map
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01716
    /// @needwork = dd
    /// @endcode
    std::map< uint32_t, std::shared_ptr< Dtc > > dtcValueMapping_{};

    /// @name dtcGroups_
    /// DtcGroup map
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01717
    /// @needwork = dd
    /// @endcode
    std::map< uint32_t, std::shared_ptr< DtcGroup > > dtcGroups_{};

    /// @name clearConditions_
    /// Clear Condition map
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01718
    /// @needwork = dd
    /// @endcode
    std::map< uint32_t, std::shared_ptr< Condition > > clearConditions_{};

    /// @name enableConditions_
    /// Enable Condition map
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01719
    /// @needwork = dd
    /// @endcode
    std::map< uint32_t, std::shared_ptr< Condition > > enableConditions_{};

    /// @name enableConditionGroups_
    /// Enable Condition Group map
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01720
    /// @needwork = dd
    /// @endcode
    std::map< uint32_t, std::shared_ptr< ConditionGroup > > enableConditionGroups_{};

    /// @name clearConditionGroups_
    /// clear Condition Group map
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01721
    /// @needwork = dd
    /// @endcode
    std::map< uint32_t, std::shared_ptr< ConditionGroup > > clearConditionGroups_{};

    /// @name indicators_
    /// Indicator map
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01722
    /// @needwork = dd
    /// @endcode
    std::map< uint32_t, std::shared_ptr< Indicator > > indicators_{};

    /// @name ffs_
    /// FreezeFrame map
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01723
    /// @needwork = dd
    /// @endcode
    std::map< uint32_t, std::shared_ptr< FreezeFrame > > ffs_{};

    /// @name edrs_
    /// ExtendedDataRecord map
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01724
    /// @needwork = dd
    /// @endcode
    std::map< uint32_t, std::shared_ptr< ExtendedDataRecord > > edrs_{};

    /// @name memDestinations_
    /// MemoryDestination map
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01725
    /// @needwork = dd
    /// @endcode
    std::map< uint32_t, std::shared_ptr< MemoryDestination > > memDestinations_{};

    /// @name didSets_
    /// DataIdentifierSet map
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01726
    /// @needwork = dd
    /// @endcode
    std::map< uint32_t, std::shared_ptr< DataIdentifierSet > > didSets_{};

    /// @name monitors_
    /// Monitor vector
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01727
    /// @needwork = dd
    /// @endcode
    std::vector< std::shared_ptr< Monitor > > monitors_{};

    /// @name commonProps_
    /// pointer of DemDiagnosticCommonProps
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01728
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< DemDiagnosticCommonProps > commonProps_{nullptr};

    /// @name dtcSetting_
    /// pointer of ControlDtcSetting
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01729
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< ControlDtcSetting > dtcSetting_{nullptr};

    /// @name inPrimaryDtcList_
    /// pointer of ControlDtcSetting
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01730
    /// @needwork = dd
    /// @endcode
    std::set< uint32_t > inPrimaryDtcList_{};

    /// @name clusterName_
    /// cluster name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01731
    /// @needwork = dd
    /// @endcode
    std::string clusterName_{};
};

}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif  // __ISOFT_UDS_FAULT_RUNTIME_CONTEXT_H__