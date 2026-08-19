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
/// @file       dtc_information_proxy.cpp
/// @brief
/// @details
/// @date       2024-12-17
/// @author     zhanglipeng
/// @version    1.2.0
///
/// ================================================================

#include "dtc_information_proxy.h"

#include "ara/diag/diag_error_domain.h"
#include "log/log.h"
#include "serialization/serialization.h"

namespace isoft {
namespace dm {
namespace dic {

constexpr uint8_t kFuncIDGetCurrentStatus{1U};
constexpr uint8_t kFuncIDSetDTCStatusChangedNotifier{2U};
constexpr uint8_t kFuncIDSetSnapshotRecordUpdatedNotifier{3U};
constexpr uint8_t kFuncIDGetNumberOfStoredEntries{4U};
constexpr uint8_t kFuncIDSetNumberOfStoredEntriesNotifier{5U};
constexpr uint8_t kFuncIDGetEventMemoryOverflow{6U};
constexpr uint8_t kFuncIDSetEventMemoryOverflowNotifier{7U};
constexpr uint8_t kFuncIDClear{8U};
constexpr uint8_t kFuncIDGetControlDTCStatus{9U};
constexpr uint8_t kFuncIDSetControlDtcStatusNotifier{10U};
constexpr uint8_t kFuncIDEnableControlDtc{11U};
constexpr uint8_t kFuncIDGetDtcSuppression{12U};
constexpr uint8_t kFuncIDSetDtcSuppression{13U};

using ara::diag::common::LogError;

DtcInformationProxy::DtcInformationProxy(uint16_t const& instanceId, uint32_t const& serviceInstanceId)
    : BussinessClientProxy{serviceInstanceId, instanceId}
{
    RegisterNotificationCallBack([this](FuncData const& funcData) { _notify(funcData); });
}

/// @brief Register callback function for DTC status change notification
/// @param notifier Callback function to be registered
/// @return
void DtcInformationProxy::RegisterDTCStatusChangedNotifier(
    std::function< void(std::uint32_t dtc,
                        ara::diag::DTCInformation::UdsDtcStatusByteType,
                        ara::diag::DTCInformation::UdsDtcStatusByteType) > const& notifier)
{
    static uint8_t s_CallId{0U};

    ara::core::Result< void > result{RegisterCallBack(kFuncIDSetDTCStatusChangedNotifier, s_CallId)};
    if (!result.HasValue()) {
        LogError() << "DtcInformationProxy::RegisterDTCStatusChangedNotifier|register error";
        return;
    }

    dtcStatusChangedNotifierCb_ = notifier;
}

/// @brief Register callback function for snapshot data updated notification
/// @param notifier Callback function to be registered
/// @return
void DtcInformationProxy::RegisterSnapshotRecordUpdatedNotifier(
    std::function< void(ara::diag::DTCInformation::SnapshotRecordUpdatedType) > const& notifier)
{
    static uint8_t s_CallId{0U};

    ara::core::Result< void > result{RegisterCallBack(kFuncIDSetSnapshotRecordUpdatedNotifier, s_CallId)};
    if (!result.HasValue()) {
        LogError() << "DtcInformationProxy::RegisterSnapshotRecordUpdatedNotifier|register error";
        return;
    }

    snapshotRecordUpdatedNotifierCb_ = notifier;
}

/// @brief Register callback function for the change of the number of entities stored in the database notification
/// @param notifier Callback function to be registered
/// @return
void DtcInformationProxy::RegisterNumberOfStoredEntriesNotifier(std::function< void(std::uint32_t) > const& notifier)
{
    static uint8_t s_CallId{0U};

    ara::core::Result< void > result{RegisterCallBack(kFuncIDSetNumberOfStoredEntriesNotifier, s_CallId)};
    if (!result.HasValue()) {
        LogError() << "DtcInformationProxy::RegisterNumberOfStoredEntriesNotifier|register error";
        return;
    }

    numberOfStoredEntriesNotifierCb_ = notifier;
}

/// @brief Register callback function for whether the database has overflowed notification
/// @param notifier Callback function to be registered
/// @return
void DtcInformationProxy::RegisterEventMemoryOverflowNotifier(std::function< void(bool) > const& notifier)
{
    static uint8_t s_CallId{0U};

    ara::core::Result< void > result{RegisterCallBack(kFuncIDSetEventMemoryOverflowNotifier, s_CallId)};
    if (!result.HasValue()) {
        LogError() << "DtcInformationProxy::RegisterEventMemoryOverflowNotifier|register error";
        return;
    }

    eventMemoryOverflowNotifierCb_ = notifier;
}

/// @brief Register callback function for the change of the flag controlling DTC notification
/// @param notifier Callback function to be registered
/// @return
void DtcInformationProxy::RegisterControlDtcStatusNotifier(
    std::function< void(ara::diag::ControlDtcStatusType) > const& notifier)
{
    static uint8_t s_CallId{0U};

    ara::core::Result< void > result{RegisterCallBack(kFuncIDSetControlDtcStatusNotifier, s_CallId)};
    if (!result.HasValue()) {
        LogError() << "DtcInformationProxy::RegisterControlDtcStatusNotifier|register error";
        return;
    }

    controlDtcStatusNotifierCb_ = notifier;
}

/// @brief Get the number of entities stored in the database
/// @param
/// @return Return 0 for success
ara::core::Result< std::uint32_t > DtcInformationProxy::GetNumberOfStoredEntries()
{
    static uint8_t s_CallId{0U};
    ara::core::Result< uint32_t > const result{SyncCallFunc< uint32_t >(kFuncIDGetNumberOfStoredEntries, s_CallId)};
    if (result.HasValue()) {
        uint32_t const ret = result.Value();
        return ara::core::Result< std::uint32_t >::FromValue(ret);
    }
    return ara::core::Result< std::uint32_t >::FromError(result.Error());
}

/// @brief Clear a group of DTCs
/// @param dtcGroup the group id to be clear
/// @return Return 0 for success
ara::core::Result< void > DtcInformationProxy::Clear(std::uint32_t const& dtcGroup)
{
    static uint8_t s_CallId{0U};
    ara::core::Result< void > const result{SyncCallFunc< void >(kFuncIDClear, s_CallId, dtcGroup)};
    if (result.HasValue()) {
        return ara::core::Result< void >::FromValue();
    }
    return ara::core::Result< void >::FromError(result.Error());
}

/// @brief Get the control status of DTC
/// @param
/// @return Return 0 for success
ara::core::Result< ara::diag::ControlDtcStatusType > DtcInformationProxy::GetControlDTCStatus()
{
    static uint8_t s_CallId{0U};
    ara::core::Result< uint8_t > const result{SyncCallFunc< uint8_t >(kFuncIDGetControlDTCStatus, s_CallId)};
    if (result.HasValue()) {
        ara::diag::ControlDtcStatusType const ret = static_cast< ara::diag::ControlDtcStatusType >(result.Value());
        return ara::core::Result< ara::diag::ControlDtcStatusType >::FromValue(ret);
    }
    return ara::core::Result< ara::diag::ControlDtcStatusType >::FromError(result.Error());
}

/// @brief Restore DTC status change
/// @param
/// @return Return 0 for success
ara::core::Result< void > DtcInformationProxy::EnableControlDtc()
{
    static uint8_t s_CallId{0U};
    ara::core::Result< void > const result{SyncCallFunc< void >(kFuncIDEnableControlDtc, s_CallId)};
    if (result.HasValue()) {
        return ara::core::Result< void >::FromValue();
    }
    return ara::core::Result< void >::FromError(result.Error());
}

/// @brief Get the current status code of DTC
/// @param
/// @return Return 0 for success
ara::core::Result< ara::diag::DTCInformation::UdsDtcStatusByteType > DtcInformationProxy::GetCurrentStatus(
    std::uint32_t const& dtc)
{
    static uint8_t s_CallId{0U};
    ara::core::Result< uint8_t > const result{SyncCallFunc< uint8_t >(kFuncIDGetCurrentStatus, s_CallId, dtc)};
    if (result.HasValue()) {
        ara::diag::DTCInformation::UdsDtcStatusByteType const ret{result.Value()};
        return ara::core::Result< ara::diag::DTCInformation::UdsDtcStatusByteType >::FromValue(ret);
    }
    return ara::core::Result< ara::diag::DTCInformation::UdsDtcStatusByteType >::FromError(result.Error());
}

/// @brief Get the database overflow flag
/// @param
/// @return Return 0 for success
ara::core::Result< bool > DtcInformationProxy::GetEventMemoryOverflow()
{
    static uint8_t s_CallId{0U};
    ara::core::Result< bool > const result{SyncCallFunc< bool >(kFuncIDGetEventMemoryOverflow, s_CallId)};
    if (result.HasValue()) {
        bool const ret{result.Value()};
        return ara::core::Result< bool >::FromValue(ret);
    }
    return ara::core::Result< bool >::FromError(result.Error());
}

/// @brief Get the suppression status of DTC
/// @param dtc dtcValue to be obtained
/// @return Return specific status
ara::core::Result< ara::diag::DTCInformation::DtcSuppressionType > DtcInformationProxy::GetDtcSuppression(
    std::uint32_t const& dtc) noexcept
{
    static uint8_t s_CallId{0U};
    ara::core::Result< std::uint8_t > const result{
        SyncCallFunc< std::uint8_t >(kFuncIDGetDtcSuppression, s_CallId, dtc)};
    if (result.HasValue()) {
        ara::diag::DTCInformation::DtcSuppressionType const ret{
            static_cast< ara::diag::DTCInformation::DtcSuppressionType >(result.Value())};
        return ara::core::Result< ara::diag::DTCInformation::DtcSuppressionType >::FromValue(ret);
    }
    return ara::core::Result< ara::diag::DTCInformation::DtcSuppressionType >::FromError(result.Error());
}

/// @brief Set the suppression status of DTC
/// @param dtc dtcValue to be set
/// @param suppressionStatus Suppression status
/// @return Execution result
ara::core::Result< void > DtcInformationProxy::SetDtcSuppression(
    std::uint32_t dtc, ara::diag::DTCInformation::DtcSuppressionType suppressionStatus) noexcept
{
    static uint8_t s_CallId{0U};
    std::uint8_t const suppressionStatus8{static_cast< std::uint8_t >(suppressionStatus)};
    ara::core::Result< void > const result{
        SyncCallFunc< void >(kFuncIDSetDtcSuppression, s_CallId, dtc, suppressionStatus8)};
    if (result.HasValue()) {
        return ara::core::Result< void >::FromValue();
    }
    return ara::core::Result< void >::FromError(result.Error());
}

void DtcInformationProxy::_notify(FuncData const& funcData) const
{
    switch (funcData.funcId) {
        case kFuncIDSetDTCStatusChangedNotifier: {
            if (nullptr != dtcStatusChangedNotifierCb_) {
                std::uint32_t dtc{};
                std::uint8_t udsStatusByteOld{};
                std::uint8_t udsStatusByteNew{};
                if (serialize::Deserialize(funcData.data, dtc, udsStatusByteOld, udsStatusByteNew) > 0) {
                    ara::diag::DTCInformation::UdsDtcStatusByteType udsStatusByteOldCopy;
                    udsStatusByteOldCopy.udsDtcStatusByteType_ = udsStatusByteOld;  /// NOLINT
                    ara::diag::DTCInformation::UdsDtcStatusByteType udsStatusByteNewCopy;
                    udsStatusByteNewCopy.udsDtcStatusByteType_ = udsStatusByteNew;  /// NOLINT
                    dtcStatusChangedNotifierCb_(dtc, udsStatusByteOldCopy, udsStatusByteNewCopy);
                }
            } else {
                LogError() << "DtcInformationProxy::_notify|dtcStatusChangedNotifierCb_ is nullptr";
            }
            break;
        }
        case kFuncIDSetSnapshotRecordUpdatedNotifier: {
            if (nullptr != snapshotRecordUpdatedNotifierCb_) {
                SnapshotRecordUpdatedType ssr;
                if (serialize::Deserialize(funcData.data, ssr) > 0) {
                    ara::diag::DTCInformation::SnapshotRecordUpdatedType ssrCopy;
                    ssrCopy.DTC = ssr.dtc;
                    for (auto& sr : ssr.toBeReplaced) {
                        ara::diag::DTCInformation::SnapshotDataRecordType srd;
                        srd.snapshotRecordNumber = sr.snapshotRecordNumber;
                        for (auto& ssrd : sr.snapshotDataIdentifiers) {
                            ara::diag::DTCInformation::SnapshotDataIdentifierType ssrdCopy;
                            ssrdCopy.dataIdentifier = ssrd.dataIdentifier;
                            ssrdCopy.data           = ssrd.data;
                            srd.snapshotDataIdentifiers.emplace_back(ssrdCopy);
                        }

                        ssrCopy.ToBeReplaced.emplace_back(srd);
                    }
                    snapshotRecordUpdatedNotifierCb_(ssrCopy);
                }
            } else {
                LogError() << "DtcInformationProxy::_notify|snapshotRecordUpdatedNotifierCb_ is nullptr";
            }
            break;
        }
        case kFuncIDSetNumberOfStoredEntriesNotifier: {
            if (nullptr != numberOfStoredEntriesNotifierCb_) {
                std::uint32_t count{};
                if (serialize::Deserialize(funcData.data, count) > 0) {
                    numberOfStoredEntriesNotifierCb_(count);
                }
            } else {
                LogError() << "DtcInformationProxy::_notify|numberOfStoredEntriesNotifierCb_ is nullptr";
            }
            break;
        }
        case kFuncIDSetEventMemoryOverflowNotifier: {
            if (nullptr != eventMemoryOverflowNotifierCb_) {
                bool flow{};
                if (serialize::Deserialize(funcData.data, flow) > 0) {
                    eventMemoryOverflowNotifierCb_(flow);
                }
            } else {
                LogError() << "DtcInformationProxy::_notify|eventMemoryOverflowNotifierCb_ is nullptr";
            }
            break;
        }
        case kFuncIDSetControlDtcStatusNotifier: {
            if (nullptr != controlDtcStatusNotifierCb_) {
                std::uint8_t type{};
                if (serialize::Deserialize(funcData.data, type) > 0) {
                    controlDtcStatusNotifierCb_(static_cast< ara::diag::ControlDtcStatusType >(type));
                }
            } else {
                LogError() << "DtcInformationProxy::_notify|controlDtcStatusNotifierCb_ is nullptr";
            }
            break;
        }
        default: {
        } break;
    }
}

}  // namespace dic
}  // namespace dm
}  // namespace isoft