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
/// @brief      This file provides the implementation of DTCInformation.
/// @details
/// @date       2022-03-28
/// @author     gaohuiming
/// @version    1.2.0
///
/// ================================================================

#include "ara/diag/dtc_information.h"

#include "ara/diag/diag_error_domain.h"
#include "gen_code/dtc_information/clientAgent/dtc_information_agent.h"
#include "resolve.h"
#include "utility.h"
namespace ara {
namespace diag {

/// @brief Constructor for a DTCInformation instance which allows for DTC
/// related operation per DiagnosticMemoryDestination.
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
/// @param[in] specifier InstanceSpecifier to an PortPrototype of an
/// DiagnosticDTCInformationInterface
/// @throws on overflow
///
/// @traceid{SWS_DM_00664}@tracestatus{draft}
DTCInformation::DTCInformation(ara::core::InstanceSpecifier const &specifier)
{
    ara::core::Result< internal::InstanceInfo > const retrieveResult{internal::Resolve(specifier)};
    if (retrieveResult.HasValue()) {
        proxy_ = std::make_shared< isoft::dm::dic::DtcInformationAgent >(retrieveResult.Value().instanceId,
                                                                         retrieveResult.Value().serviceInstanceId);
    } else {
        internal::LogError() << "DTCInformation::DTCInformation|not found specifier="
                             << std::move(specifier.ToString());
    }
}

/// @brief Retrieves the current UDS DTC status byte of the given DTC
/// identifier.
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
/// @param[in] dtc DTC indentifier for which the status should be retrieved.
/// @return the current UDS DTC status byte of the given DTC identifier.
/// error code: kNoSuchDtc
/// @throws on overflow
///
/// @traceid{SWS_DM_00666}@tracestatus{draft}
ara::core::Result< DTCInformation::UdsDtcStatusByteType > DTCInformation::GetCurrentStatus(std::uint32_t dtc)
{
    if (proxy_.get() == nullptr) {
        internal::LogError() << "DTCInformation::GetCurrentStatus|proxy is null.";
        return ara::core::Result< DTCInformation::UdsDtcStatusByteType >::FromError(ara::diag::DiagErrc::kFailed);
    }
    return proxy_->GetCurrentStatus(dtc);
}

/// @brief Register a notifier function which is called if a UDS DTC status is
/// changed.
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
/// @param[in] notifier The function to be called if a DTC status has changed.
/// @return ara::core::Result<void> error code: kNoSuchDtc, kInvalidArgument
/// @throws on overflow
///
/// @traceid{SWS_DM_00667}@tracestatus{draft}
ara::core::Result< void > DTCInformation::SetDTCStatusChangedNotifier(
    std::function< void(std::uint32_t dtc,
                        UdsDtcStatusByteType udsStatusByteOld,
                        UdsDtcStatusByteType udsStatusByteNew) > notifier)  /// NOLINT
{
    if (proxy_.get() == nullptr) {
        internal::LogError() << "DTCInformation::SetDTCStatusChangedNotifier|proxy is null.";
        return ara::core::Result< void >::FromError(ara::diag::DiagErrc::kFailed);
    }

    if (!proxy_->IsServiceReady()) {
        internal::LogError() << "DTCInformation::SetDTCStatusChangedNotifier|service is not ready";
        return ara::core::Result< void >::FromError(ara::diag::DiagErrc::kNotOffered);
    }

    proxy_->RegisterDTCStatusChangedNotifier(notifier);
    return ara::core::Result< void >::FromValue();
}

/// @brief Register a notifier function which is called if the SnapshotRecord is
/// changed.
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
/// @param[in] notifier The function to be called if the SnapshotRecord is
/// changed.
/// @return ara::core::Result<void>  error code: kInvalidArgument
/// @throws on overflow
///
/// @traceid{SWS_DM_00668}@tracestatus{draft}
ara::core::Result< void > DTCInformation::SetSnapshotRecordUpdatedNotifier(
    std::function< void(SnapshotRecordUpdatedType) > notifier)  /// NOLINT
{
    if (proxy_.get() == nullptr) {
        internal::LogError() << "DTCInformation::SetSnapshotRecordUpdatedNotifier|proxy is null.";
        return ara::core::Result< void >::FromError(ara::diag::DiagErrc::kFailed);
    }

    if (!proxy_->IsServiceReady()) {
        internal::LogError() << "DTCInformation::SetSnapshotRecordUpdatedNotifier|"
                                "service is not ready";
        return ara::core::Result< void >::FromError(ara::diag::DiagErrc::kNotOffered);
    }

    proxy_->RegisterSnapshotRecordUpdatedNotifier(notifier);
    return ara::core::Result< void >::FromValue();
}

/// @brief Contains the number of currently stored fault memory entries.
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
/// @return Number of currently stored fault memory entries.
/// @throws on overflow
///
/// @traceid{SWS_DM_00669}@tracestatus{draft}
ara::core::Result< std::uint32_t > DTCInformation::GetNumberOfStoredEntries()
{
    if (proxy_.get() == nullptr) {
        internal::LogError() << "DTCInformation::GetNumberOfStoredEntries|proxy is null.";
        return ara::core::Result< std::uint32_t >::FromError(ara::diag::DiagErrc::kFailed);
    }
    return proxy_->GetNumberOfStoredEntries();
}

/// @brief Register a notifier function which is called if the number of
/// currently stored fault memory entries changed.
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
/// @param[in] notifier The function to be called if the number of entries for
/// this diagnostic event memory instance has changed.
/// @return ara::core::Result<void> error code: kInvalidArgument
/// @throws on overflow
///
/// @traceid{SWS_DM_00670}@tracestatus{draft}
ara::core::Result< void > DTCInformation::SetNumberOfStoredEntriesNotifier(
    std::function< void(std::uint32_t) > notifier)  /// NOLINT
{
    if (proxy_.get() == nullptr) {
        internal::LogError() << "DTCInformation::EnableControlDtc|"
                                "SetNumberOfStoredEntriesNotifier is null.";
        return ara::core::Result< void >::FromError(ara::diag::DiagErrc::kFailed);
    }

    if (!proxy_->IsServiceReady()) {
        internal::LogError() << "DTCInformation::SetNumberOfStoredEntriesNotifier|"
                                "service is not ready";
        return ara::core::Result< void >::FromError(ara::diag::DiagErrc::kNotOffered);
    }

    proxy_->RegisterNumberOfStoredEntriesNotifier(notifier);
    return ara::core::Result< void >::FromValue();
}

/// @brief Contains the current event memory overflow status.
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
/// @return Current status of event memory overflow.
/// @throws on overflow
///
/// @traceid{SWS_DM_00919}@tracestatus{draft}
ara::core::Result< bool > DTCInformation::GetEventMemoryOverflow()
{
    if (proxy_.get() == nullptr) {
        internal::LogError() << "DTCInformation::GetEventMemoryOverflow|proxy is null.";
        return ara::core::Result< bool >::FromError(ara::diag::DiagErrc::kFailed);
    }
    return proxy_->GetEventMemoryOverflow();
}

/// @brief Register a notifier function which is called if the current event
/// memory overflow status changed.
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
/// @param[in] notifier The function to be called if the overflow status for
/// this diagnostic event memory instance has changed.
/// @return ara::core::Result<void> error code: kInvalidArgument
/// @throws on overflow
///
/// @traceid{SWS_DM_00918}@tracestatus{draft}
ara::core::Result< void > DTCInformation::SetEventMemoryOverflowNotifier(
    std::function< void(bool) > notifier)  // NOLINT
{
    if (proxy_.get() == nullptr) {
        internal::LogError() << "DTCInformation::SetEventMemoryOverflowNotifier|proxy is null.";
        return ara::core::Result< void >::FromError(ara::diag::DiagErrc::kFailed);
    }

    if (!proxy_->IsServiceReady()) {
        internal::LogError() << "DTCInformation::SetEventMemoryOverflowNotifier|"
                                "service is not ready";
        return ara::core::Result< void >::FromError(ara::diag::DiagErrc::kNotOffered);
    }

    proxy_->RegisterEventMemoryOverflowNotifier(notifier);
    return ara::core::Result< void >::FromValue();
}

/// @brief Method for Clearing a DTC or a group of DTCs.
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
/// @param[in] dtcGroup DTC group to be cleared.
/// @return void or errors : kBusy, kFailed, kMemoryError, kWrongDtc
/// @throws on overflow
///
/// @traceid{SWS_DM_00671}@tracestatus{draft}
ara::core::Result< void > DTCInformation::Clear(std::uint32_t dtcGroup)
{
    if (proxy_.get() == nullptr) {
        internal::LogError() << "DTCInformation::Clear|proxy is null.";
        return ara::core::Result< void >::FromError(ara::diag::DiagErrc::kFailed);
    }
    return proxy_->Clear(dtcGroup);
}

/// @brief Contains the current status of the ControlDTCStatus
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
/// @return The current status of ControlDtcStatus (related to UDS service 0x85)
/// @throws on overflow
///
/// @traceid{SWS_DM_00672}@tracestatus{draft}
ara::core::Result< ara::diag::ControlDtcStatusType > DTCInformation::GetControlDTCStatus()
{
    if (proxy_.get() == nullptr) {
        internal::LogError() << "DTCInformation::GetControlDTCStatus|proxy is null.";
        return ara::core::Result< ara::diag::ControlDtcStatusType >::FromError(ara::diag::DiagErrc::kFailed);
    }

    return proxy_->GetControlDTCStatus();
}

/// @brief Registers a notifier function which is called if the control DTC
/// setting is changed.
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
/// @param[in] notifier The function to be called if the ControlDTCStatus
/// (related to UDS service 0x85) for this diagnostic memory instance has
/// changed.
/// @return error code: kInvalidArgument
/// @throws on overflow
///
/// @traceid{SWS_DM_00673}@tracestatus{draft}
ara::core::Result< void > DTCInformation::SetControlDtcStatusNotifier(
    std::function< void(ara::diag::ControlDtcStatusType) > notifier)  // NOLINT
{
    if (proxy_.get() == nullptr) {
        internal::LogError() << "DTCInformation::SetControlDtcStatusNotifier|proxy is null.";
        return ara::core::Result< void >::FromError(ara::diag::DiagErrc::kFailed);
    }

    if (!proxy_->IsServiceReady()) {
        internal::LogError() << "DTCInformation::SetControlDtcStatusNotifier|service is not ready";
        return ara::core::Result< void >::FromError(ara::diag::DiagErrc::kNotOffered);
    }

    proxy_->RegisterControlDtcStatusNotifier(notifier);
    return ara::core::Result< void >::FromValue();
}

/// @brief Enforce restoring ControlDTCStatus setting to enabled in case the
/// monitor has some conditions or states demands to do so.
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
/// @return ara::core::Result<void>
/// @throws on overflow
///
/// @traceid{SWS_DM_00674}@tracestatus{draft}
ara::core::Result< void > DTCInformation::EnableControlDtc()
{
    if (proxy_.get() == nullptr) {
        internal::LogError() << "DTCInformation::EnableControlDtc|proxy is null.";
        return ara::core::Result< void >::FromError(ara::diag::DiagErrc::kFailed);
    }
    return proxy_->EnableControlDtc();
}

/// @brief Contains the current status of DTC Suppression.
/// @param[in] dtc  DTC indentifier for which the suppression status should be
/// retrieved.
/// @returns the current status of DTC Suppression.
/// @arerror DiagErrorDomain::DiagErrc::kWrongDtc wrong DTC number requested.
/// @arerror ara::diag::DiagErrc::kServiceNotAvailable
///
/// @artraceid{SWS_DM_01293}@artracestatus{draft}
/// @aruptrace{RS_AP_00139}
/// @aruptrace{RS_Diag_04220}
///
ara::core::Result< ara::diag::DTCInformation::DtcSuppressionType > DTCInformation::GetDtcSuppression(std::uint32_t dtc)
{
    if (proxy_.get() == nullptr) {
        internal::LogError() << "DTCInformation::GetDtcSuppression|proxy is null.";
        return ara::core::Result< ara::diag::DTCInformation::DtcSuppressionType >::FromError(
            ara::diag::DiagErrc::kServiceNotAvailable);
    }
    return proxy_->GetDtcSuppression(dtc);
}

/// @brief Set the status of DTC Suppression.
/// @param[in] dtc  DTC indentifier for which the DTC Suppression status should be
/// set.
/// @param[in] suppressionStatus  DTC Suppression status that should be set.
/// @arerror DiagErrorDomain::DiagErrc::kWrongDtc wrong DTC number requested.
/// @arerror DiagErrorDomain::DiagErrc::kSuppressionIgnored Suppression request
/// ignored.
///
/// @artraceid{SWS_DM_01292}@artracestatus{draft}
/// @aruptrace{RS_AP_00139}
/// @aruptrace{RS_Diag_04220}
///
ara::core::Result< void > DTCInformation::SetDtcSuppression(std::uint32_t dtc, DtcSuppressionType suppressionStatus)
{
    if (proxy_.get() == nullptr) {
        internal::LogError() << "DTCInformation::SetDtcSuppression|proxy is null.";
        return ara::core::Result< void >::FromError(ara::diag::DiagErrc::kServiceNotAvailable);
    }
    return proxy_->SetDtcSuppression(dtc, suppressionStatus);
}

}  // namespace diag
}  // namespace ara