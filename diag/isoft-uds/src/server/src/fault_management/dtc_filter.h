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
/// @file       dtc_filter.h
/// @brief      This file provides dtc_filter functionality
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
/// @trace_id_sr=fb2562bf-0b4e-451b-aefb-b329096652a8
/// @unit_name=DtcFilter
/// @unit_description=This file provides dtc_filter functionality
/// @endcode
///
/// ================================================================

#ifndef __ISOFT_UDS_FAULT_DTC_FILTER_H__
#define __ISOFT_UDS_FAULT_DTC_FILTER_H__

#include <cstdint>
#include <memory>

#include "fault_management/types.h"
#include "isoft/uds/fault_management/dtc_information_interface.h"
namespace isoft {
namespace uds {
namespace server {

/// @brief FaultManagementRuntimeContext class declaration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02531
/// @needwork = dd
/// @endcode
class FaultManagementRuntimeContext;

/// @brief DtcFilter class
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02532
/// @needwork = dd
/// @endcode
class DtcFilter final
{
public:
    /// @brief Copy constructor
    /// @param[in] other DtcFilter reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01793
    /// @needwork = dd
    /// @endcode
    DtcFilter(DtcFilter const& other) = default;

    /// @brief Move constructor
    /// @param[in] other DtcFilter rvalue reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01794
    /// @needwork = dd
    /// @endcode
    DtcFilter(DtcFilter&& other) = default;
    /// @param[in] right DtcFilter reference
    /// @return DtcFilter reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00803
    /// @needwork = dda
    /// @endcode
    DtcFilter& operator=(DtcFilter const& right) = delete;
    /// @param[in] right DtcFilter rvalue reference
    /// @return DtcFilter reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00804
    /// @needwork = dda
    /// @endcode
    DtcFilter& operator=(DtcFilter&& right) = delete;

public:
    /// @brief Constructor
    /// @param[in] context This parameter is recorded in the class to facilitate obtaining various data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01795
    /// @needwork = dd
    /// @endcode
    explicit DtcFilter(FaultManagementRuntimeContext& context) noexcept;

    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01796
    /// @needwork = dd
    /// @endcode
    ~DtcFilter() noexcept;

    /// @brief Get the number of DTCs
    /// @param[in] dtcStatusMask This byte is used in the request message to allow the client to request DTC information whose status matches the DTCStatusMask. When any one of the actual status bits of the DTC is set to '1', and the corresponding status bit in the DTCStatusMask is also set to '1', the status of the DTC matches the DTCStatusMask.
    /// @param[out] dtcStatusAvailabilityMask A byte whose bit definitions are the same as statusOfDTC, indicating the status bits supported by the server. Bits not supported by the server should be set to '0'. Each supported bit (indicated by the value '1') should be implemented for each DTC supported by the server.
    /// @param[out] dtcFormatIdentifier This 1-byte parameter value defines the format of the DTC reported by the server.
    /// @param[out] dtcCount This 2-byte parameter collectively refers to the DTCCountHighByte and DTCCountLowByte parameters sent in response to the reportNumberOfDTCByStatusMask request. DTCCount provides a count of the number of DTCs that match the DTCStatusMask defined in the client request.
    /// @return result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01797
    /// @needwork = dd
    /// @endcode
    int32_t GetNumberOfDTCByStatusMask(std::uint8_t const dtcStatusMask,
                                       std::uint8_t& dtcStatusAvailabilityMask,
                                       std::uint8_t& dtcFormatIdentifier,
                                       std::uint16_t& dtcCount);

    /// @brief Get DTC information
    /// @param[in] dtcStatusMask This byte is used in the request message to allow the client to request DTC information whose status matches the DTCStatusMask. When any one of the actual status bits of the DTC is set to '1', and the corresponding status bit in the DTCStatusMask is also set to '1', the status of the DTC matches the DTCStatusMask.
    /// @param[out] dtcStatusAvailabilityMask A byte whose bit definitions are the same as statusOfDTC, indicating the status bits supported by the server. Bits not supported by the server should be set to '0'. Each supported bit (indicated by the value '1') should be implemented for each DTC supported by the server.
    /// @param[out] dtcAndStatusRecords This parameter record contains one or more groups of DTCHighByte, DTCMiddleByte, DTCLowByte and statusOfDTC.
    /// @return result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01798
    /// @needwork = dd
    /// @endcode
    int32_t GetDTCByStatusMask(std::uint8_t const dtcStatusMask,
                               std::uint8_t& dtcStatusAvailabilityMask,
                               std::vector< DTCAndStatusRecord >& dtcAndStatusRecords);

    /// @brief Get DTCSnapshotNumber
    /// @param[out] dtcAndSnapshotNumberTable SnapshotDataRecordType vector reference
    /// @return result
    /// @code{.isoft}
    /// @throw unknown
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00805
    /// @needwork = dda
    /// @needwork = dd
    /// @endcode
    int32_t GetDTCSnapshotNumber(std::vector< DTCAndSnapshotNumber >& dtcAndSnapshotNumberTable) noexcept;

    /// @brief Get DTCSnapshotRecordByDTCNumber
    /// @param[in] dtcMaskRecord A 3-byte value containing DTCHighByte, DTCMiddleByte, and DTCLowByte, which together represent a unique identification number for a specific diagnostic trouble code supported by the server.
    /// @param[in] ssrNumber A 1-byte value representing the specific DTCSnapshot data record number requested by the client for the DTCMaskRecord defined by the subfunction.
    ///                      0x00 is reserved for regulatory purposes (e.g., WWH-OBD), 0x01-0xFE range is for vehicle manufacturer specific use, 0xFF requests the server to report all stored DTCSnapshot data records at once.
    /// @param[out] dtcAndStatusRecord Contains one or more groups of DTCHighByte, DTCMiddleByte, DTCLowByte and statusOfDTC.
    /// @param[out] records Reference to SnapshotDataRecordType vector
    /// @return result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01799
    /// @needwork = dd
    /// @endcode
    int32_t GetDTCSnapshotRecordByDTCNumber(std::uint32_t const dtcMaskRecord,
                                            std::uint8_t const ssrNumber,
                                            DTCAndStatusRecord& dtcAndStatusRecord,
                                            std::vector< SnapshotDataRecordType >& records);

    /// @brief Get DTCExtDataRecordByDTCNumber
    /// @brief Get DTC extended data record
    /// @param[in] dtcMaskRecord A 3-byte value containing DTCHighByte, DTCMiddleByte, and DTCLowByte, which together represent a unique identification number for a specific diagnostic trouble code supported by the server.
    /// @param[in] edrNumber Echo of the DTCExtDataRecordNumber parameter specified by the client in the reportDTCExtDataRecordByDTCNumber request, or the actual DTCExtDataRecordNumber of the stored DTCExtendedData record.
    /// @param[out] dtcAndStatusRecord This parameter record contains one or more groups of DTCHighByte, DTCMiddleByte, DTCLowByte and statusOfDTC.
    /// @param[out] records Reference to DTCExtDataRecord vector
    /// @return result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01800
    /// @needwork = dd
    /// @endcode
    int32_t GetDTCExtDataRecordByDTCNumber(std::uint32_t const dtcMaskRecord,
                                           std::uint8_t const edrNumber,
                                           DTCAndStatusRecord& dtcAndStatusRecord,
                                           std::vector< DTCExtDataRecord >& records);

    /// @brief Get the number of DTCs
    /// @param[in] dtcSeverityMask A 3-byte value containing 3 DTC severity bits. This byte is used in the request message to allow the client to request information about DTCs defined by the DTCSeverityMask.
    /// @param[in] dtcStatusMask A 1-byte value used in the request message to allow the client to request information about DTCs whose status matches the DTCStatusMask.
    /// @param[out] dtcStatusAvailabilityMask A 1-byte value indicating the status bits supported by the server. Bits not supported should be set to '0'.
    /// @param[out] dtcFormatIdentifier A 1-byte value defining the format of the DTCs reported by the server.
    /// @param[out] dtcCount A 2-byte value indicating the number of DTCs defined by the DTCStatusMask.
    /// @return result
    /// @code{.isoft}
    /// @throw unknown
    /// @needwork = dd
    /// @endcode
    int32_t GetNumberOfDTCBySeverityMaskRecord(std::uint8_t const dtcSeverityMask,
                                               std::uint8_t const dtcStatusMask,
                                               std::uint8_t& dtcStatusAvailabilityMask,
                                               std::uint8_t& dtcFormatIdentifier,
                                               std::uint16_t& dtcCount);

    /// @brief Get all supported DTCs
    /// @param[out] dtcStatusAvailabilityMask A 1-byte value indicating the status bits supported by the server. Bits not supported should be set to '0'.
    /// @param[out] dtcAndStatusRecords Reference to DTCAndStatusRecord vector
    /// @return result
    /// @code{.isoft}
    /// @throw unknown
    /// @needwork = dd
    /// @endcode
    int32_t GetSupportedDTC(std::uint8_t& dtcStatusAvailabilityMask,
                            std::vector< DTCAndStatusRecord >& dtcAndStatusRecords);

    /// @brief Get fault detection counts for all DTCs
    /// @param[out] dtcFdcRecords Reference to DTCFaultDetectionCounterRecord vector
    /// @return result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01801
    /// @needwork = dd
    /// @endcode
    int32_t GetDTCFaultDetectionCounter(std::vector< DTCFaultDetectionCounterRecord >& dtcFdcRecords);

    /// @brief Get user-defined DTCs
    /// @param[in] dtcStatusMask A 1-byte value used in the request message to allow the client to request information about DTCs whose status matches the DTCStatusMask.
    /// @param[in] memorySelection memory id
    /// @param[out] dtcStatusAvailabilityMask A 1-byte value indicating the status bits supported by the server. Bits not supported should be set to '0'.
    /// @param[out] dtcAndStatusRecords Reference to DTCAndStatusRecord vector
    /// @return result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01802
    /// @needwork = dd
    /// @endcode
    int32_t GetUserDefMemoryDTCByStatusMask(std::uint8_t const dtcStatusMask,
                                            std::uint8_t const memorySelection,
                                            std::uint8_t& dtcStatusAvailabilityMask,
                                            std::vector< DTCAndStatusRecord >& dtcAndStatusRecords);

    /// @brief Get user-defined DTCSnapshot records
    /// @param[in] dtcMaskRecord A 3-byte value containing 3 DTC severity bits. This byte is used in the request message to allow the client to request information about DTCs defined by the DTCSeverityMask.
    /// @param[in] ssrNumber A 1-byte value used in the request message to allow the client to request DTCSnapshot records defined by the DTCSnapshotRecordNumber.
    /// @param[in] memorySelection memory id
    /// @param[out] dtcAndStatusRecord Reference to DTCAndStatusRecord
    /// @param[out] records Reference to SnapshotDataRecordType vector
    /// @return result
    /// @code{.isoft}
    /// @throw unknown
    /// @needwork = dd
    /// @endcode
    int32_t GetUserDefMemoryDTCSnapshotRecordByDTCNumber(std::uint32_t const dtcMaskRecord,
                                                         std::uint8_t const ssrNumber,
                                                         std::uint8_t const memorySelection,
                                                         DTCAndStatusRecord& dtcAndStatusRecord,
                                                         std::vector< SnapshotDataRecordType >& records);

    /// @brief Get user-defined DTCExtendedData records
    /// @param[in] dtcMaskRecord A 3-byte value containing 3 DTC severity bits. This byte is used in the request message to allow the client to request information about DTCs defined by the DTCSeverityMask.
    /// @param[in] edrNumber A 1-byte value used in the request message to allow the client to request DTCExtendedData records defined by the DTCExtDataRecordNumber.
    /// @param[in] memorySelection memory id
    /// @param[out] dtcAndStatusRecord Reference to DTCAndStatusRecord
    /// @param[out] records Reference to DTCExtDataRecord vector
    /// @return result
    /// @code{.isoft}
    /// @throw unknown
    /// @needwork = dd
    /// @endcode
    int32_t GetUserDefMemoryDTCExtDataRecordByDTCNumber(std::uint32_t const dtcMaskRecord,
                                                        std::uint8_t const edrNumber,
                                                        std::uint8_t const memorySelection,
                                                        DTCAndStatusRecord& dtcAndStatusRecord,
                                                        std::vector< DTCExtDataRecord >& records);

private:
    /// @name context_
    /// context of runtime in dem
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01803
    /// @needwork = dd
    /// @endcode
    FaultManagementRuntimeContext& context_;
};

}  // namespace server
}  // namespace uds
}  // namespace isoft
#endif  // __ISOFT_UDS_FAULT_DTC_FILTER_H__