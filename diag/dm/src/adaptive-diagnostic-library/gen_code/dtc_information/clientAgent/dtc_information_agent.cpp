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
namespace dic {

DtcInformationAgent::DtcInformationAgent(uint16_t const& instanceId, uint32_t const& serviceInstanceId)
    : proxy_{std::make_unique< DtcInformationProxy >(instanceId, serviceInstanceId)}
{
}

/// @brief Register callback function for DTC status change notification
/// @param notifier Callback function to be registered
/// @return
void DtcInformationAgent::RegisterDTCStatusChangedNotifier(
    std::function< void(std::uint32_t dtc,
                        ara::diag::DTCInformation::UdsDtcStatusByteType,
                        ara::diag::DTCInformation::UdsDtcStatusByteType) > const& notifier)
{
    proxy_->RegisterDTCStatusChangedNotifier(notifier);
}

/// @brief Register callback function for snapshot data updated notification
/// @param notifier Callback function to be registered
/// @return
void DtcInformationAgent::RegisterSnapshotRecordUpdatedNotifier(
    std::function< void(ara::diag::DTCInformation::SnapshotRecordUpdatedType) > const& notifier)
{
    proxy_->RegisterSnapshotRecordUpdatedNotifier(notifier);
}

/// @brief Register callback function for the change of the number of entities stored in the database notification
/// @param notifier Callback function to be registered
/// @return
void DtcInformationAgent::RegisterNumberOfStoredEntriesNotifier(std::function< void(std::uint32_t) > const& notifier)
{
    proxy_->RegisterNumberOfStoredEntriesNotifier(notifier);
}

/// @brief Register callback function for whether the database has overflowed notification
/// @param notifier Callback function to be registered
/// @return
void DtcInformationAgent::RegisterEventMemoryOverflowNotifier(std::function< void(bool) > const& notifier)
{
    proxy_->RegisterEventMemoryOverflowNotifier(notifier);
}

/// @brief Register callback function for the change of the flag controlling DTC notification
/// @param notifier Callback function to be registered
/// @return
void DtcInformationAgent::RegisterControlDtcStatusNotifier(
    std::function< void(ara::diag::ControlDtcStatusType) > const& notifier)
{
    proxy_->RegisterControlDtcStatusNotifier(notifier);
}

/// @brief Get the number of entities stored in the database
/// @param
/// @return Return 0 for success
ara::core::Result< std::uint32_t > DtcInformationAgent::GetNumberOfStoredEntries()
{
    return proxy_->GetNumberOfStoredEntries();
}

/// @brief Clear a group of DTCs
/// @param dtcGroup the group id to be clear
/// @return Return 0 for success
ara::core::Result< void > DtcInformationAgent::Clear(std::uint32_t const& dtcGroup) { return proxy_->Clear(dtcGroup); }

/// @brief Get the control status of DTC
/// @param
/// @return Return 0 for success
ara::core::Result< ara::diag::ControlDtcStatusType > DtcInformationAgent::GetControlDTCStatus()
{
    return proxy_->GetControlDTCStatus();
}

/// @brief restore control DTC status
/// @param
/// @return Return 0 for success
ara::core::Result< void > DtcInformationAgent::EnableControlDtc() { return proxy_->EnableControlDtc(); }

/// @brief Get the current status code of DTC
/// @param dtc dtcValue to be obtained
/// @return Return 0 for success
ara::core::Result< ara::diag::DTCInformation::UdsDtcStatusByteType > DtcInformationAgent::GetCurrentStatus(
    std::uint32_t const& dtc)
{
    return proxy_->GetCurrentStatus(dtc);
}

/// @brief Get the database overflow flag
/// @return Return 0 for success
ara::core::Result< bool > DtcInformationAgent::GetEventMemoryOverflow() { return proxy_->GetEventMemoryOverflow(); }

/// @brief Get the suppression status of DTC
/// @param dtc DTC value to be obtained
/// @return Return specific status
ara::core::Result< ara::diag::DTCInformation::DtcSuppressionType > DtcInformationAgent::GetDtcSuppression(
    std::uint32_t const& dtc) noexcept
{
    return proxy_->GetDtcSuppression(dtc);
}

/// @brief Set the suppression status of DTC
/// @param dtc dtcValue to be set
/// @param suppressionStatus Suppression status
/// @return Execution result
ara::core::Result< void > DtcInformationAgent::SetDtcSuppression(
    std::uint32_t dtc, ara::diag::DTCInformation::DtcSuppressionType suppressionStatus) noexcept
{
    return proxy_->SetDtcSuppression(dtc, suppressionStatus);
}

/// @brief Determine whether the service is ready
/// @return true: Ready
bool DtcInformationAgent::IsServiceReady() { return proxy_->IsServiceReady(); }

}  // namespace dic
}  // namespace dm
}  // namespace isoft