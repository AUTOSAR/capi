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
/// @file       memory_primary.h
/// @brief      This file provides memory_primary functionality
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
/// @trace_id_sr=52255098-0011-41d2-8639-9dd99a6e87c5
/// @unit_name=MemoryPrimary
/// @unit_description=This file provides memory_primary functionality
/// @endcode
///
/// ================================================================

#ifndef __ISOFT_UDS_FAULT_MEMORY_PRIMARY_H__
#define __ISOFT_UDS_FAULT_MEMORY_PRIMARY_H__

#include <memory>

#include "fault_management/memory_destination.h"
namespace isoft {
namespace uds {
namespace server {

/// @brief primary memory class
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02563
/// @needwork = dd
/// @endcode
class MemoryPrimary : public MemoryDestination
{
public:
    /// @brief Default constructor
    MemoryPrimary() = delete;

    /// @brief Constructor
    /// @param[in] context
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01949
    /// @needwork = dd
    /// @endcode
    MemoryPrimary(FaultManagementRuntimeContext& context,
                  MemoryDestinationConfig& config,
                  std::string& clusterName,
                  std::shared_ptr< PersistenceInterface > per) noexcept
        : MemoryDestination{context, config, clusterName, per}
    {
    }

    /// @brief Get the number of diagnostic trouble code data in storage that meet the filter criteria
    /// @param[in] dtcStatusMask Diagnostic trouble code status mask
    /// @param[out] dtcStatusAvailabilityMask Diagnostic module supported diagnostic trouble code status mask
    /// @param[out] dtcFormatIdentifier Diagnostic module supported diagnostic trouble code format standard
    /// @param[out] dtcCount Number of diagnostic trouble codes meeting the filter criteria
    /// @return Result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01950
    /// @needwork = dd
    /// @endcode
    bool GetNumberOfDTCByStatusMask(std::uint8_t dtcStatusMask,
                                    std::uint8_t& dtcStatusAvailabilityMask,
                                    std::uint8_t& dtcFormatIdentifier,
                                    std::uint16_t& dtcCount) noexcept override;

    /// @brief Get the number of diagnostic trouble codes meeting the given filter criteria
    /// @param[in] dtcSeverityMask DTCSeverityMask diagnostic trouble code severity mask
    /// @param[in] dtcStatusMask DTCStatusMask diagnostic trouble code status mask
    /// @param[out] dtcStatusAvailabilityMask DTCStatusAvailabilityMask diagnostic trouble code status available bit mask
    /// @param[out] dtcFormatIdentifier DTCFormatIdentifier diagnostic trouble code format and encoding identifier
    /// @param[out] dtcCount Number of diagnostic trouble codes matching the filter criteria
    /// @return Result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01951
    /// @needwork = dd
    /// @endcode
    bool GetNumberOfDTCBySeverityMaskRecord(std::uint8_t dtcSeverityMask,
                                            std::uint8_t dtcStatusMask,
                                            std::uint8_t& dtcStatusAvailabilityMask,
                                            std::uint8_t& dtcFormatIdentifier,
                                            std::uint16_t& dtcCount) noexcept override;

    /// @brief Get the set of currently supported diagnostic trouble codes
    /// @param[out] dtcStatusAvailabilityMask DTCStatusAvailabilityMask diagnostic trouble code status available bit mask
    /// @param[out] dtcAndStatusRecords Diagnostic trouble code dataset
    /// @return Result
    /// @code{.isoft}
    /// @throw unknown
    /// According to iso-14229 Table 269 0x0A description
    /// And according to autosar document [SWS_DM_00968]
    /// All supported dtc data should be returned
    /// @needwork = dd
    /// @endcode
    bool GetSupportedDTC(std::uint8_t& dtcStatusAvailabilityMask,
                         std::vector< DTCAndStatusRecord >& dtcAndStatusRecords) override;

private:
    /// @brief Return the DTC format standard currently supported by the diagnostic module
    /// @return DTC format standard identification value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01952
    /// @needwork = dd
    /// @endcode
    std::uint8_t _dtcFormatIdFromTypeOfDtcSupported() const noexcept;
};

}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif  // __ISOFT_UDS_FAULT_MEMORY_PRIMARY_H__