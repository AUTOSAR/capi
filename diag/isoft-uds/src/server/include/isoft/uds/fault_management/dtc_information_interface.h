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
/// @file       dtc_information_interface.h
/// @brief      This file provides dtc_information_interface functionality
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
/// @trace_id_sr=59c2c792-0e56-49e7-9edf-6d420110d62a
/// @unit_name=DtcInformationInterface
/// @unit_description=This file provides dtc_information_interface functionality
/// @endcode
///
/// ================================================================

#ifndef __ISOFT_UDS_FAULT_DTC_INFORMATION_INTERFACE_H__
#define __ISOFT_UDS_FAULT_DTC_INFORMATION_INTERFACE_H__

#include <cstdint>
#include <functional>
#include <memory>
#include <vector>

namespace isoft {
namespace uds {
namespace server {

/// @brief DTC status change type
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01356
/// @trace_id_dd=DD_UDS_02648
/// @needwork = ad
/// @endcode
class DTCStatusChangedType
{
public:
    /// @name dtc
    /// @brief DTC
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01098
    /// @trace_id_dd=DD_UDS_02212
    /// @needwork = ad
    /// @endcode
    std::uint32_t dtc{0U};
    /// @name oldValue
    /// @brief Old value
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01099
    /// @trace_id_dd=DD_UDS_02213
    /// @needwork = ad
    /// @endcode
    std::uint8_t oldValue{0U};
    /// @name newValue
    /// @brief New value
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01100
    /// @trace_id_dd=DD_UDS_02214
    /// @needwork = ad
    /// @endcode
    std::uint8_t newValue{0U};
};

/// @brief Snapshot data identifier type
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01357
/// @trace_id_dd=DD_UDS_02649
/// @needwork = ad
/// @endcode
class SnapshotDataIdentifierType
{
public:
    /// @name dataIdentifier
    /// @brief Data identifier
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01101
    /// @trace_id_dd=DD_UDS_02215
    /// @needwork = ad
    /// @endcode
    std::uint16_t dataIdentifier{0U};
    /// @name data
    /// @brief Data
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01102
    /// @trace_id_dd=DD_UDS_02216
    /// @needwork = ad
    /// @endcode
    std::vector< std::uint8_t > data{};
};

/// @brief Snapshot data record type
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01358
/// @trace_id_dd=DD_UDS_02650
/// @needwork = ad
/// @endcode
class SnapshotDataRecordType
{
public:
    /// @brief Snapshot data record number
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01103
    /// @trace_id_dd=DD_UDS_02217
    /// @needwork = ad
    /// @endcode
    std::uint8_t snapshotRecordNumber{0U};
    /// @brief Snapshot data identifier
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01104
    /// @trace_id_dd=DD_UDS_02218
    /// @needwork = ad
    /// @endcode
    std::vector< SnapshotDataIdentifierType > snapshotDataIdentifiers{};
};

/// @brief Snapshot data record type
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01359
/// @trace_id_dd=DD_UDS_02651
/// @needwork = ad
/// @endcode
using SnapshotDataRecordsType = std::vector< SnapshotDataRecordType >;

/// @brief Snapshot data record update type
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01360
/// @trace_id_dd=DD_UDS_02652
/// @needwork = ad
/// @endcode
class SnapshotRecordUpdatedType
{
public:
    /// @brief DTC
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01105
    /// @trace_id_dd=DD_UDS_02219
    /// @needwork = ad
    /// @endcode
    std::uint32_t nDTC{0U};
    /// @brief Snapshot data record to be replaced
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01106
    /// @trace_id_dd=DD_UDS_02220
    /// @needwork = ad
    /// @endcode
    SnapshotDataRecordsType vecToBeReplaced{};
};

/// @brief Control DTC status type
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01361
/// @trace_id_dd=DD_UDS_02653
/// @needwork = ad
/// @endcode
enum class ControlDtcStatusType : uint8_t
{
    kDTCSettingOn  = 0x00,  ///<  Updating of diagnostic trouble code status bits is under normal operating conditions
    kDTCSettingOff = 0x01   ///<  Updating of diagnostic trouble code status bits is stopped
};

/// @brief DTC suppression type
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01362
/// @trace_id_dd=DD_UDS_02654
/// @needwork = ad
/// @endcode
enum class DtcSuppressionType
{
    kDTCSuppressionOff = 0x00,  ///<  suppression status off
    kDTCSuppressionOn  = 0x01,  ///<  suppression status on
};
/// @brief Clear callback
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01363
/// @trace_id_dd=DD_UDS_02655
/// @needwork = ad
/// @endcode
using ClearCallBack = std::function< int32_t(std::uint32_t const) >;
/// @brief Get control DTC status callback
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01364
/// @trace_id_dd=DD_UDS_02656
/// @needwork = ad
/// @endcode
using GetControlDTCStatusBack = std::function< ControlDtcStatusType() >;
/// @brief Get entity count callback
/// @brief Get storage entity count callback
/// @code{.isoft}
/// @interface_level=unit
/// @needwork=ad
/// @interface_level=unit
/// @needwork=ad
/// @endcode
using GetNumberOfStoredEntriesCallBack = std::function< std::uint32_t() >;
/// @brief Enable control DTC callback
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01365
/// @trace_id_dd=DD_UDS_02657
/// @needwork = ad
/// @endcode
using EnableControlDtcCallBack = std::function< std::int32_t() >;
/// @brief Get current status callback
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01366
/// @trace_id_dd=DD_UDS_02658
/// @needwork = ad
/// @endcode
using GetCurrentStatusCallBack = std::function< std::int32_t(std::uint32_t const, std::uint8_t&) >;
/// @brief Get event memory overflow callback
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01367
/// @trace_id_dd=DD_UDS_02659
/// @needwork = ad
/// @endcode
using GetEventMemoryOverflowCallBack = std::function< bool() >;

using SetDtcSuppressionCallback = std::function< std::int32_t(std::uint32_t const, DtcSuppressionType const) >;
using GetDtcSuppressionCallback = std::function< std::int32_t(std::uint32_t const, DtcSuppressionType&) >;
/// @brief DTC information interface
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01368
/// @trace_id_dd=DD_UDS_02660
/// @needwork = ad
/// @endcode
class DtcInformationInterface
{
public:
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_01011
    /// @needwork = dda
    /// @endcode
    DtcInformationInterface(DtcInformationInterface const&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_01012
    /// @needwork = dda
    /// @endcode
    DtcInformationInterface(DtcInformationInterface&&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_01013
    /// @needwork = dda
    /// @endcode
    DtcInformationInterface& operator=(DtcInformationInterface const&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_01014
    /// @needwork = dda
    /// @endcode
    DtcInformationInterface& operator=(DtcInformationInterface&&) = delete;
    /// @brief Constructor
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01107
    /// @trace_id_dd=DD_UDS_02221
    /// @needwork = ad
    /// @endcode
    DtcInformationInterface() noexcept = default;
    /// @brief Destructor
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01108
    /// @trace_id_dd=DD_UDS_02222
    /// @needwork = ad
    /// @endcode
    virtual ~DtcInformationInterface() noexcept = default;

public:
    /// @brief Notify DTC status change
    /// @param dtcStatus dtc status
    /// @return Returns 0 on success
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01109
    /// @trace_id_dd=DD_UDS_02223
    /// @needwork = ad
    /// @endcode
    virtual std::int32_t NotifyDTCStatusChanged(DTCStatusChangedType const& dtcStatus) = 0;

    /// @brief Notify snapshot data record update
    /// @param dtcSnapshot Snapshot
    /// @return Returns 0 on success
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01110
    /// @trace_id_dd=DD_UDS_02224
    /// @needwork = ad
    /// @endcode
    virtual std::int32_t NotifySnapshotRecordUpdated(SnapshotRecordUpdatedType const& dtcSnapshot) = 0;
    /// @brief Notify storage entity count
    /// @param numberOfStoredEntries Entity count
    /// @return Returns 0 on success
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01111
    /// @trace_id_dd=DD_UDS_02225
    /// @needwork = ad
    /// @endcode
    virtual std::int32_t NotifyNumberOfStoredEntries(std::uint32_t const numberOfStoredEntries) = 0;
    /// @brief Notify event memory overflow
    /// @param isOverflow Overflow flag
    /// @return Returns 0 on success
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01112
    /// @trace_id_dd=DD_UDS_02226
    /// @needwork = ad
    /// @endcode
    virtual std::int32_t NotifyEventMemoryOverflow(bool const isOverflow) = 0;
    /// @brief Notify control DTC status
    /// @param dtcStatusType Flag
    /// @return Returns 0 on success
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01113
    /// @trace_id_dd=DD_UDS_02227
    /// @needwork = ad
    /// @endcode
    virtual std::int32_t NotifyControlDtcStatus(ControlDtcStatusType const dtcStatusType) = 0;
    /// @brief Register get storage entity count callback
    /// @param callBack Callback
    /// @return Returns 0 on success
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01114
    /// @trace_id_dd=DD_UDS_02228
    /// @needwork = ad
    /// @endcode
    virtual std::int32_t RegisterGetNumberOfStoredEntries(GetNumberOfStoredEntriesCallBack const& callBack) = 0;
    /// @brief Register clear callback
    /// @param callBack Callback
    /// @return Returns 0 on success
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01115
    /// @trace_id_dd=DD_UDS_02229
    /// @needwork = ad
    /// @endcode
    virtual std::int32_t RegisterClear(ClearCallBack const& callBack) = 0;

    /// @brief Register get control DTC status callback
    /// @param callBack Callback
    /// @return Returns 0 on success
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01116
    /// @trace_id_dd=DD_UDS_02230
    /// @needwork = ad
    /// @endcode
    virtual std::int32_t RegisterGetControlDTCStatus(GetControlDTCStatusBack const& callBack) = 0;
    /// @brief Register enable control DTC status callback
    /// @param callBack Callback
    /// @return Returns 0 on success
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01117
    /// @trace_id_dd=DD_UDS_02231
    /// @needwork = ad
    /// @endcode
    virtual std::int32_t RegisterEnableControlDtc(EnableControlDtcCallBack const& callBack) = 0;
    /// @brief Register get current status callback
    /// @param callBack Callback
    /// @return Returns 0 on success
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01118
    /// @trace_id_dd=DD_UDS_02232
    /// @needwork = ad
    /// @endcode
    virtual std::int32_t RegisterGetCurrentStatus(GetCurrentStatusCallBack const& callBack) = 0;
    /// @brief Register get event memory overflow callback
    /// @param callBack Callback
    /// @return Returns 0 on success
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01119
    /// @trace_id_dd=DD_UDS_02233
    /// @needwork = ad
    /// @endcode
    virtual std::int32_t RegisterGetEventMemoryOverflow(GetEventMemoryOverflowCallBack const& callBack) = 0;
    /// @brief Register set DTC suppression callback
    /// @param callBack Callback
    /// @return Returns 0 on success
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01120
    /// @trace_id_dd=DD_UDS_02234
    /// @needwork = ad
    /// @endcode
    virtual std::int32_t RegisterSetDtcSuppression(SetDtcSuppressionCallback const& callBack) = 0;
    /// @brief Register get DTC suppression callback
    /// @param callBack Callback
    /// @return Returns 0 on success
    /// @code{.isoft}
    /// @note None
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_01121
    /// @trace_id_dd=DD_UDS_02235
    /// @needwork = ad
    /// @endcode
    virtual std::int32_t RegisterGetDtcSuppression(GetDtcSuppressionCallback const& callBack) = 0;
};

}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif  // DM_UDS_GENERIC_UDS_SERVICE_H_