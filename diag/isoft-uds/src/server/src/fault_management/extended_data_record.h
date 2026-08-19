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
/// @file       extended_data_record.h
/// @brief      This file provides extended_data_record functionality
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
/// @trace_id_sr=dcff2b4e-9576-4b50-a132-2fcfc607cad8
/// @unit_name=ExtendedDataRecord
/// @unit_description=This file provides extended_data_record functionality
/// @endcode
///
/// ================================================================

#ifndef __ISOFT_UDS_FAULT_EXTENDED_DATA_RECORD_H__
#define __ISOFT_UDS_FAULT_EXTENDED_DATA_RECORD_H__

#include <memory>
#include <vector>

#include "data_management/diagnostic_data_manager.h"
#include "fault_management/public.h"
#include "isoft/uds/fault_management/fault_management.h"
namespace isoft {
namespace uds {
namespace server {

/// @brief Dtc forward declaration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02474
/// @needwork = dd
/// @endcode
class Dtc;

/// @brief DemRuntimeContext forward declaration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02475
/// @needwork = dd
/// @endcode
class FaultManagementRuntimeContext;

/// @brief MemoryEntryExtendedRecord forward declaration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02476
/// @needwork = dd
/// @endcode
class MemoryEntryExtendedRecord;

/// @brief MemoryDestination forward declaration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02477
/// @needwork = dd
/// @endcode
class MemoryDestination;

/// @brief EDR wrapper class, encapsulates various configuration parameters and behaviors such as configuration parsing and data capture
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02478
/// @needwork = dd
/// @endcode
class ExtendedDataRecord final
{
public:
    /// @brief DemExtendedDataRecord constructor
    /// @param[in] context
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01595
    /// @needwork = dd
    /// @endcode
    ExtendedDataRecord(FaultManagementRuntimeContext& context,
                       std::shared_ptr< DiagnosticDataManager >& dataManage,
                       ExtendedDataRecordConfig& config) noexcept
        : context_{context}, config_{config}, dataManage_{dataManage} {};

    /// @brief Get configured attribute, the number of this EDR
    /// @return EDR number
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01596
    /// @needwork = dd
    /// @endcode
    uint8_t GetRecordNumber() const noexcept { return config_.recordNumber; }

    /// @brief Get configured attribute, whether this EDR allows data update
    /// @return Returns true if allowed, false if not allowed
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01597
    /// @needwork = dd
    /// @endcode
    bool GetUpdate() const noexcept { return config_.update; }

    /// @brief Get configured attribute, the trigger of this EDR is used to identify under what circumstances data can be captured and updated
    /// @return The trigger of this EDR
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01598
    /// @needwork = dd
    /// @endcode
    DiagnosticRecordTriggerEnum GetTrigger() const noexcept { return config_.trigger; }

    /// @brief Get configured attribute, custom trigger
    /// @return Custom trigger
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01599
    /// @needwork = dd
    /// @endcode
    std::string GetCustomTrigger() const noexcept { return config_.customTrigger; }

    /// @brief Capture data of all DataElements configured for this EDR
    /// @param[out] edrt Output parameter, EDR wrapper class convenient for storage in destination, captured data will be stored in this structure
    /// @param[in] dtc The dtc value to which this EDR belongs
    /// @return Returns true on success, false on failure
    /// @code{.isoft}
    /// @throw unknown
    /// @needwork = dd
    /// @endcode
    bool CaptureRecord(std::vector< MemoryEntryExtendedRecord >& edrt, std::uint32_t const dtc);

    /// @brief Whether a certain dataelement of edr belongs to an external element
    /// @return true if it belongs to external element
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01600
    /// @needwork = dd
    /// @endcode
    EdrDataElementCategory GetDataElementCategory(std::uint32_t const& dataElementId);

    /// @brief Since the configured DEM related DataElements need to be obtained in real time, this function is introduced to implement it
    /// @param[in] provider The data type of the DataElement to be obtained
    /// @param[out] data Obtained data, output parameter
    /// @param[in] dtc The dtc value to which the data belongs
    /// @param[in] memId The id of the destination where the data is to be stored, some data acquisition requires this parameter
    /// @code{.isoft}
    /// @throw unknown
    /// @needwork = dd
    /// @endcode
    void GetInternalData(EdrDataElementCategory const provider,
                         std::vector< std::uint8_t >& data,
                         std::uint32_t const dtc,
                         std::uint32_t const memId) const;

private:
    /// @brief Specific implementation of capturing DEM related DataElement data
    /// @param[out] datas Obtained data, output parameter
    /// @param[in] dtc The dtc to which the data belongs
    /// @param[in] dest The destination where the data is to be stored
    /// @param[in] category The data type of the DataElement
    /// @code{.isoft}
    /// @throw unknown
    /// @needwork = dd
    /// @endcode
    static void CaptureInternalData(std::vector< std::uint8_t >& datas,
                                    std::shared_ptr< Dtc > const& dtc,
                                    std::shared_ptr< MemoryDestination > const& dest,
                                    EdrDataElementCategory const category);

    /// @name context_
    /// Save dem runtime context, many logics need to query data from here
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01601
    /// @needwork = dd
    /// @endcode
    FaultManagementRuntimeContext& context_;

    /// @brief Configuration
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01602
    /// @needwork = dd
    /// @endcode
    ExtendedDataRecordConfig config_;

    /// @brief Data manager
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01603
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< DiagnosticDataManager > dataManage_;
};

}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif  // __ISOFT_UDS_FAULT_EXTENDED_DATA_RECORD_H__