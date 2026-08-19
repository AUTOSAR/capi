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
/// @file       dtc_information_agent.cpp
/// @brief
/// @details
/// @date       2024-12-17
/// @author     zhanglipeng
/// @version    1.2.0
///
/// ================================================================

#include "dtc_information_agent.h"

namespace isoft {
namespace dm {
namespace dis {

DtcInformationAgent::DtcInformationAgent(uint32_t const& serviceInstanceId)
{
    proxy_ = std::make_unique< DtcInformationProxy >(serviceInstanceId);
}

/// @brief Notify DTC status change
/// @param dtcStatus DTC status
/// @return
void DtcInformationAgent::NotifyDTCStatusChanged(std::uint32_t dtc,
                                                 std::uint8_t udsStatusByteOld,
                                                 std::uint8_t udsStatusByteNew)
{
    proxy_->NotifyDTCStatusChanged(dtc, udsStatusByteOld, udsStatusByteNew);
}

/// @brief Notify snapshot data change
/// @param dtcSnapshot Snapshot
/// @return
void DtcInformationAgent::NotifySnapshotRecordUpdated(SnapshotRecordUpdatedType const& dtcSnapshot)
{
    proxy_->NotifySnapshotRecordUpdated(dtcSnapshot);
}

/// @brief Notify the change of the number of entities stored in the database
/// @param numberOfStoredEntries Number of entities
/// @return
void DtcInformationAgent::NotifyNumberOfStoredEntries(std::uint32_t const numberOfStoredEntries)
{
    proxy_->NotifyNumberOfStoredEntries(numberOfStoredEntries);
}

/// @brief Notify whether the database overflows
/// @param isOverflow Overflow flag
/// @return
void DtcInformationAgent::NotifyEventMemoryOverflow(bool const isOverflow)
{
    proxy_->NotifyEventMemoryOverflow(isOverflow);
}

/// @brief Notify the change of the DTC control flag
/// @param dtcStatusType Flag
/// @return
void DtcInformationAgent::NotifyControlDtcStatus(uint8_t const dtcStatusType)
{
    proxy_->NotifyControlDtcStatus(dtcStatusType);
}

/// @brief Register callback for AA side to get the number of stored entities in the database
/// @param callBack Callback function to be registered
/// @return
void DtcInformationAgent::RegisterGetNumberOfStoredEntries(std::function< std::uint32_t() > const& callBack)
{
    proxy_->RegisterGetNumberOfStoredEntries(callBack);
}

/// @brief Register callback for AA side to clear DTC
/// @param callBack Callback function to be registered
/// @return
void DtcInformationAgent::RegisterClear(std::function< int32_t(std::uint32_t const) > const& callBack)
{
    proxy_->RegisterClear(callBack);
}

/// @brief Register callback for AA side to get DTC control status
/// @param callBack Callback function to be registered
/// @return
void DtcInformationAgent::RegisterGetControlDTCStatus(std::function< std::uint8_t() > const& callBack)
{
    proxy_->RegisterGetControlDTCStatus(callBack);
}

/// @brief Register callback for AA side to restore DTC status change
/// @param callBack Callback function to be registered
/// @return
void DtcInformationAgent::RegisterEnableControlDtc(std::function< std::int32_t() > const& callBack)
{
    proxy_->RegisterEnableControlDtc(callBack);
}

/// @brief Register callback for AA side to get current DTC status code
/// @param callBack Callback function to be registered
/// @return
void DtcInformationAgent::RegisterGetCurrentStatus(
    std::function< std::int32_t(std::uint32_t const, uint8_t&) > const& callBack)
{
    proxy_->RegisterGetCurrentStatus(callBack);
}

/// @brief Register callback for AA side to get database overflow flag
/// @param callBack Callback function to be registered
/// @return
void DtcInformationAgent::RegisterGetEventMemoryOverflow(std::function< bool() > const& callBack)
{
    proxy_->RegisterGetEventMemoryOverflow(callBack);
}

/// @brief Register callback to get DTC suppression status
/// @param callback Callback function to be registered
void DtcInformationAgent::RegisterGetDtcSuppression(
    std::function< std::int32_t(std::uint32_t, std::uint8_t&) > const& callback) noexcept
{
    proxy_->RegisterGetDtcSuppression(callback);
}

/// @brief Register callback to set DTC suppression status
/// @param callback Callback function to be registered
void DtcInformationAgent::RegisterSetDtcSuppression(
    std::function< std::int32_t(std::uint32_t, std::uint8_t) > const& callback) noexcept
{
    proxy_->RegisterSetDtcSuppression(callback);
}
}  // namespace dis
}  // namespace dm
}  // namespace isoft