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
/// @file       dtc_information.cpp
/// @brief      This file provides the implementation of DtcInformation and related types.
/// @details
/// @date       2024-12-12
/// @author     zhanglipeng
/// @version    1.2.0
///
/// ================================================================

#include "dtc_information.h"

namespace ara {
namespace diag {
namespace dmd {

DtcInformation::DtcInformation(uint32_t const& serviceInstanceId) noexcept
    : dtcInformationAgent_{std::make_unique< isoft::dm::dis::DtcInformationAgent >(serviceInstanceId)}
{
}

/// @brief Notify DTC status changes
/// @param dtcStatus DTC status
/// @return Returns 0 for success
std::int32_t DtcInformation::NotifyDTCStatusChanged(isoft::uds::server::DTCStatusChangedType const& dtcStatus)
{
    if (dtcInformationAgent_.get() == nullptr) {
        return -1;
    }

    dtcInformationAgent_->NotifyDTCStatusChanged(dtcStatus.dtc, dtcStatus.oldValue, dtcStatus.newValue);
    return 0;
}

/// @brief Notify snapshot data changes
/// @param dtcSnapshot Snapshot
/// @return Returns 0 for success
std::int32_t DtcInformation::NotifySnapshotRecordUpdated(
    isoft::uds::server::SnapshotRecordUpdatedType const& dtcSnapshot)
{
    if (dtcInformationAgent_.get() == nullptr) {
        return -1;
    }

    isoft::dm::SnapshotRecordUpdatedType ssr;
    ssr.dtc = dtcSnapshot.nDTC;
    for (auto const& sr : dtcSnapshot.vecToBeReplaced) {
        isoft::dm::SnapshotDataRecordType srd;
        srd.snapshotRecordNumber = sr.snapshotRecordNumber;
        for (auto const& ssrd : sr.snapshotDataIdentifiers) {
            isoft::dm::SnapshotDataIdentifierType ssrdCopy;
            ssrdCopy.dataIdentifier = ssrd.dataIdentifier;
            ssrdCopy.data           = ssrd.data;
            srd.snapshotDataIdentifiers.emplace_back(ssrdCopy);
        }
        ssr.toBeReplaced.emplace_back(srd);
    }

    dtcInformationAgent_->NotifySnapshotRecordUpdated(ssr);
    return 0;
}

/// @brief Notify changes in the number of entities stored in the database
/// @param numberOfStoredEntries Number of entities
/// @return Returns 0 for success
std::int32_t DtcInformation::NotifyNumberOfStoredEntries(std::uint32_t const numberOfStoredEntries)
{
    if (dtcInformationAgent_.get() == nullptr) {
        return -1;
    }

    dtcInformationAgent_->NotifyNumberOfStoredEntries(numberOfStoredEntries);
    return 0;
}

/// @brief Notify whether the database has overflowed
/// @param isOverflow Overflow flag
/// @return Returns 0 for success
std::int32_t DtcInformation::NotifyEventMemoryOverflow(bool const isOverflow)
{
    if (dtcInformationAgent_.get() == nullptr) {
        return -1;
    }

    dtcInformationAgent_->NotifyEventMemoryOverflow(isOverflow);
    return 0;
}

/// @brief Notify changes in the flag controlling DTC
/// @param dtcStatusType Flag
/// @return Returns 0 for success
std::int32_t DtcInformation::NotifyControlDtcStatus(isoft::uds::server::ControlDtcStatusType const dtcStatusType)
{
    if (dtcInformationAgent_.get() == nullptr) {
        return -1;
    }

    dtcInformationAgent_->NotifyControlDtcStatus(static_cast< uint8_t >(dtcStatusType));
    return 0;
}

/// @brief Register callback for AA side to get the number of entities stored in the database
/// @param callBack Callback function
/// @return Returns 0 for successful registration
std::int32_t DtcInformation::RegisterGetNumberOfStoredEntries(
    isoft::uds::server::GetNumberOfStoredEntriesCallBack const& callBack)
{
    if (dtcInformationAgent_.get() == nullptr) {
        return -1;
    }

    getNumberOfStoredEntriesCallBack_ = callBack;
    dtcInformationAgent_->RegisterGetNumberOfStoredEntries(
        [this]() -> uint32_t { return getNumberOfStoredEntriesCallBack_(); });
    return 0;
}

/// @brief Register callback for AA side to clear DTC
/// @param callBack Callback function
/// @return Returns 0 for successful registration
std::int32_t DtcInformation::RegisterClear(isoft::uds::server::ClearCallBack const& callBack)
{
    if (dtcInformationAgent_.get() == nullptr) {
        return -1;
    }

    clearCallBack_ = callBack;
    dtcInformationAgent_->RegisterClear([this](uint32_t const dtc) -> int32_t { return clearCallBack_(dtc); });
    return 0;
}

/// @brief Register callback for AA side to get the control status of DTC
/// @param callBack Callback function
/// @return Returns 0 for successful registration
std::int32_t DtcInformation::RegisterGetControlDTCStatus(isoft::uds::server::GetControlDTCStatusBack const& callBack)
{
    if (dtcInformationAgent_.get() == nullptr) {
        return -1;
    }

    getControlDTCStatusBack_ = callBack;
    dtcInformationAgent_->RegisterGetControlDTCStatus(
        [this]() -> std::uint8_t { return static_cast< uint8_t >(getControlDTCStatusBack_()); });
    return 0;
}

/// @brief Register callback for AA side to restore DTC status changes
/// @param callBack Callback function
/// @return Returns 0 for successful registration
std::int32_t DtcInformation::RegisterEnableControlDtc(isoft::uds::server::EnableControlDtcCallBack const& callBack)
{
    if (dtcInformationAgent_.get() == nullptr) {
        return -1;
    }

    enableControlDtcCallBack_ = callBack;
    dtcInformationAgent_->RegisterEnableControlDtc([this]() -> int32_t { return enableControlDtcCallBack_(); });
    return 0;
}

/// @brief Register callback for AA side to get the current status code of DTC
/// @param callBack Callback function
/// @return Returns 0 for successful registration
std::int32_t DtcInformation::RegisterGetCurrentStatus(isoft::uds::server::GetCurrentStatusCallBack const& callBack)
{
    if (dtcInformationAgent_.get() == nullptr) {
        return -1;
    }

    getCurrentStatusCallBack_ = callBack;
    dtcInformationAgent_->RegisterGetCurrentStatus([this](std::uint32_t const dtc, uint8_t& dtcStatus) -> int32_t {
        return getCurrentStatusCallBack_(dtc, dtcStatus);
    });
    return 0;
}

/// @brief Register callback for AA side to get the database overflow flag
/// @param callBack Callback function
/// @return Returns 0 for successful registration
std::int32_t DtcInformation::RegisterGetEventMemoryOverflow(
    isoft::uds::server::GetEventMemoryOverflowCallBack const& callBack)
{
    if (dtcInformationAgent_.get() == nullptr) {
        return -1;
    }

    getEventMemoryOverflowCallBack_ = callBack;
    dtcInformationAgent_->RegisterGetEventMemoryOverflow(
        [this]() -> bool { return getEventMemoryOverflowCallBack_(); });
    return 0;
}

/// @brief Register callback for AA side to set DTC suppression status
/// @param callBack Callback function
/// @return
std::int32_t DtcInformation::RegisterSetDtcSuppression(isoft::uds::server::SetDtcSuppressionCallback const& callBack)
{
    if (dtcInformationAgent_.get() == nullptr) {
        return -1;
    }

    setDtcSuppressionCallback_ = callBack;
    dtcInformationAgent_->RegisterSetDtcSuppression([this](std::uint32_t dtc, std::uint8_t status) -> int32_t {
        isoft::uds::server::DtcSuppressionType type{static_cast< isoft::uds::server::DtcSuppressionType >(status)};
        return setDtcSuppressionCallback_(dtc, type);
    });
    return 0;
}

/// @brief Register callback for AA side to get DTC suppression status
/// @param callBack
/// @return
std::int32_t DtcInformation::RegisterGetDtcSuppression(isoft::uds::server::GetDtcSuppressionCallback const& callBack)
{
    if (dtcInformationAgent_.get() == nullptr) {
        return -1;
    }

    getDtcSuppressionCallback_ = callBack;
    dtcInformationAgent_->RegisterGetDtcSuppression([this](std::uint32_t dtc, std::uint8_t& status) -> int32_t {
        isoft::uds::server::DtcSuppressionType type{};
        std::int32_t result{0U};
        result = getDtcSuppressionCallback_(dtc, type);
        status = static_cast< std::uint8_t >(type);
        return result;
    });
    return 0;
}
}  // namespace dmd
}  // namespace diag
}  // namespace ara