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
/// @file       package_management_app.cpp
/// @brief      vucm acts as a client of ucm. This class mainly encapsulates software package transfer, upgrade/update, activation, rollback, and retrieval of software package/cluster information for ucm.
/// @details
/// @date       2024-07-20
/// @author     hanzhibo
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/UCM Master/Package Management Service
/// @interface_level=module
/// @trace_id_sr=SR_UCM_00035
/// @unit_name=PackageManagementApp
/// @unit_description=Encapsulates software package transfer, upgrade/update, activation, rollback, and retrieval of software package/cluster information for ucm
/// @endcode
///
/// ================================================================

#include "package_management_app.h"

#include <ara/core/future.h>
#include <ara/core/promise.h>
#include <ara/ucm/internal/extraction/filesystem.h>
#include <ara/ucm/internal/extraction/tinyfs.h>
#include <ara/ucm/internal/transfer/helper.h>
///#include <ara/ucm/pkgmgr/error_domain_ucmerrordomain.h>
#include <ara/ucm/pkgmgr/packagemanagement_proxy.h>

#include <utility>

#include "ara/ucm/pkgmgr/error_domain_ucmerrordomain.h"
#include "fsm/fsm_manager.h"
#include "package_management_service/data_transfer.h"
#include "utils/helper.h"
#include "utils/ucmm_log.h"

namespace ara {
namespace ucm {
namespace vpkgmgr {

/// @brief Define alias PackageManagementProxy
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
using PackageManagementProxy = pkgmgr::proxy::PackageManagementProxy;
/// @brief Define alias GetSwPackagesOutput
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
using GetSwPackagesOutput = pkgmgr::PackageManagement::GetSwPackagesOutput;
/// @brief Define alias GetHistoryOutput
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
using GetHistoryOutput = pkgmgr::PackageManagement::GetHistoryOutput;
/// @brief Define alias GetSwClusterChangeInfoOutput
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
using GetSwClusterChangeInfoOutput = pkgmgr::PackageManagement::GetSwClusterChangeInfoOutput;
/// @brief Define alias GetSwClusterDescriptionOutput
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
using GetSwClusterDescriptionOutput = pkgmgr::PackageManagement::GetSwClusterDescriptionOutput;

/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00372
/// @trace_id_dd=DD_UCM_Master_00739
/// @needwork = dd
/// @endcode
constexpr int32_t kMs100{100};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00348
/// @trace_id_dd=DD_UCM_Master_00753
/// @needwork = dd
/// @endcode
constexpr int32_t kMs200{200};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00372
/// @trace_id_dd=DD_UCM_Master_00739
/// @needwork = dd
/// @endcode
constexpr int32_t kMs500{500};

/// @brief get milliseconds of current time
/// @return milliseconds of current time
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00348
/// @trace_id_dd=DD_UCM_Master_00937
/// @needwork = dd
/// @endcode
static uint64_t NowMS()
{
    const int64_t ms
        = std::chrono::duration_cast< std::chrono::milliseconds >(std::chrono::system_clock::now().time_since_epoch())
              .count();
    return static_cast< uint64_t >(ms);
}

/// @brief Convert the current status to a human-readable string
/// @param status
/// @return str
/// @throws no
ara::core::StringView const PackageManagementApp::CurrentStatusToString(pkgmgr::PackageManagerStatusType const status)
{
    ara::core::StringView str{};
    switch (status) {
        case pkgmgr::PackageManagerStatusType::kIdle: {
            str = std::move(ara::core::StringView("kIdle"));
            break;
        }
        case pkgmgr::PackageManagerStatusType::kReady: {
            str = std::move(ara::core::StringView("kReady"));
            break;
        }
        case pkgmgr::PackageManagerStatusType::kProcessing: {
            str = std::move(ara::core::StringView("kProcessing"));
            break;
        }
        case pkgmgr::PackageManagerStatusType::kActivating: {
            str = std::move(ara::core::StringView("kActivating"));
            break;
        }
        case pkgmgr::PackageManagerStatusType::kVerifying: {
            str = std::move(ara::core::StringView("kVerifying"));
            break;
        }
        case pkgmgr::PackageManagerStatusType::kActivated: {
            str = std::move(ara::core::StringView("kActivated"));
            break;
        }
        case pkgmgr::PackageManagerStatusType::kRollingBack: {
            str = std::move(ara::core::StringView("kRollingBack"));
            break;
        }
        case pkgmgr::PackageManagerStatusType::kRolledBack: {
            str = std::move(ara::core::StringView("kRolledBack"));
            break;
        }
        case pkgmgr::PackageManagerStatusType::kCleaningUp: {
            str = std::move(ara::core::StringView("kCleaningUp"));
            break;
        }
        case pkgmgr::PackageManagerStatusType::kExceptionRollingBack: {
            str = std::move(ara::core::StringView("kExceptionRollingBack"));
            break;
        }
        case pkgmgr::PackageManagerStatusType::kExceptionCleaningUp: {
            str = std::move(ara::core::StringView("kExceptionCleaningUp"));
            break;
        }
        default: {
            str = std::move(ara::core::StringView("Unknown"));
            break;
        }
    }
    return str;
}

/// @brief constructor
/// @param id
/// @param proxy
/// @param apiWaitTime
/// @throws no
PackageManagementApp::PackageManagementApp(ara::core::String id,
                                           std::shared_ptr< PackageManagementProxy > proxy,
                                           uint32_t const apiWaitTime)
    : kUcmId{std::move(id)}, pkgmPtr_{std::move(proxy)}, kApiWaitTime{apiWaitTime}
{
}

/// @brief Initialize resources
/// @return Returns true on success, false on failure
/// @throws no
bool PackageManagementApp::Init() noexcept { return true; }

/// @brief Notification of service availability
/// @return
/// @throws no
void PackageManagementApp::OnServiceAvailable()
{
    log_.LogDebug() << "PackageManagementApp::OnServiceAvailable(), begin for kUcmId:" << kUcmId.c_str();
    std::unique_lock< std::mutex > const lock{serviceMutex_};
    serviceAvailable_ = true;
    serviceAvailableCondition_.notify_all();
    log_.LogDebug() << "PackageManagementApp::OnServiceAvailable(), end for kUcmId:" << kUcmId.c_str();
}
/// @brief Notification of service unavailability
/// @return
/// @throws no
void PackageManagementApp::OnServiceUnavailable()
{
    log_.LogDebug() << "PackageManagementApp::OnServiceUnavailable(), begin for kUcmId:" << kUcmId.c_str();
    std::unique_lock< std::mutex > const lock{serviceMutex_};
    serviceAvailable_           = false;
    lastServiceUnavailableTime_ = NowMS();
    log_.LogDebug() << "PackageManagementApp::OnServiceUnavailable(), end with lastServiceUnavailableTime_:"
                    << lastServiceUnavailableTime_ << "for kUcmId:" << kUcmId.c_str();
}
/// @brief Check if service is available
/// @return bool
/// @throws no
bool PackageManagementApp::IsServiceAvailable()
{
    log_.LogDebug() << "PackageManagementApp::IsServiceAvailable(), begin for kUcmId:" << kUcmId.c_str();
    std::unique_lock< std::mutex > const lock{serviceMutex_};
    log_.LogDebug() << "PackageManagementApp::IsServiceAvailable(), end with serviceAvailable_:" << serviceAvailable_
                    << "for kUcmId:" << kUcmId.c_str();
    return serviceAvailable_;
}
/// @brief WaitServiceAvailable
/// @param lock
/// @return bool
/// @throws no
bool PackageManagementApp::_waitServiceAvailable(std::unique_lock< std::mutex >& lock)
{
    log_.LogDebug() << "PackageManagementApp::_waitServiceAvailable(), begin for kUcmId:" << kUcmId.c_str();
    while (FsmManager::GetInstance()->CanMoveForward()) {  // Check if can continue
        bool const waitRes{serviceAvailableCondition_.wait_for(
            lock, std::chrono::milliseconds(kGlobalWaitTime), [this]() noexcept -> bool { return serviceAvailable_; })};
        if (waitRes) {  // Service available
            log_.LogDebug() << "PackageManagementApp::_waitServiceAvailable(), end with true for kUcmId:"
                            << kUcmId.c_str();
            return true;
        }
    }
    log_.LogDebug() << "PackageManagementApp::_waitServiceAvailable(), end with false:"
                    << "for kUcmId:" << kUcmId.c_str();
    return false;  // Cannot continue waiting
}
/// @brief Check if service is available
/// @param operationStartTime
/// @return bool
/// @throws no
bool PackageManagementApp::_isServiceUnavailableNotified(std::uint64_t const operationStartTime) const noexcept
{  // Whether service unavailability was notified between operationStartTime and the current time
    log_.LogDebug() << "PackageManagementApp::_isServiceUnavailableNotified(), begin with operationStartTime:"
                    << operationStartTime << "lastServiceUnavailableTime_:" << lastServiceUnavailableTime_
                    << "for kUcmId:" << kUcmId.c_str();
    bool const ret{
        lastServiceUnavailableTime_
        > operationStartTime};  // The time of the last service unavailability notification is greater than the operation start time
    log_.LogDebug() << "PackageManagementApp::_isServiceUnavailableNotified(), end with ret:" << ret
                    << "for kUcmId:" << kUcmId.c_str();
    return ret;
}

/// @brief Get the ucm identifier
/// @return Returns ucm identifier
/// @throws no
pkgmgr::UCMIdentifierType const& PackageManagementApp::GetId() const noexcept { return kUcmId; }

/// @brief Get historical operation records within a specified time period
/// @param timestampGE Start time
/// @param timestampLT End time
/// @return Returns historical operation records within the specified time period
/// @throws no
pkgmgr::GetHistoryVectorType PackageManagementApp::GetHistory(std::uint64_t const& timestampGE,
                                                              std::uint64_t const& timestampLT)
{
    ara::core::Future< GetHistoryOutput > future{pkgmPtr_->GetHistory(timestampGE, timestampLT)};
    if (ara::core::future_status::ready != future.wait_for(std::chrono::milliseconds(kApiWaitTime))) {
        LOG_ERROR << "future is not timeout!!! maybe disconnected!";
        return pkgmgr::GetHistoryVectorType{};
    }

    ara::core::Result< GetHistoryOutput > const historyResult{future.GetResult()};
    if (!historyResult.HasValue()) {
        LOG_INFO << "ucmId=" << kUcmId.c_str() << " GetHistory errmsg=" << historyResult.Error().Message().data();
        return pkgmgr::GetHistoryVectorType{};
    }
    return historyResult.Value().history;
}

/// @brief Get ucm software cluster information
/// @return Returns ucm software cluster information
/// @throws no
pkgmgr::SwClusterInfoVectorType PackageManagementApp::GetSwClusterInfo() noexcept
{
    ara::core::Future< pkgmgr::PackageManagement::GetSwClusterInfoOutput > future{pkgmPtr_->GetSwClusterInfo()};
    if (ara::core::future_status::timeout == future.wait_for(std::chrono::milliseconds(kApiWaitTime))) {
        LOG_ERROR << "future is not timeout!!! maybe disconnected!";
        return pkgmgr::SwClusterInfoVectorType{};
    }
    ara::core::Result< pkgmgr::PackageManagement::GetSwClusterInfoOutput > const swInfosResult{future.GetResult()};
    if (!swInfosResult.HasValue()) {
        LOG_INFO << "ucmId=" << kUcmId.c_str() << " GetSwClusterInfo errmsg=" << swInfosResult.Error().Message().data();
        return pkgmgr::SwClusterInfoVectorType{};
    }

    // Using std::move on an empty swInfosResult.Value().SwInfo would cause a crash
    if (swInfosResult.Value().SwInfo.empty()) {
        LOG_INFO << "ucmId=" << kUcmId.c_str() << " GetSwClusterInfo size=0";
    }
    return swInfosResult.Value().SwInfo;
}

/// @brief Get the software cluster information for ucm update/upgrade
/// @return Returns ucm update/upgrade software cluster information
/// @throws no
pkgmgr::SwClusterInfoVectorType PackageManagementApp::GetSwClusterChangeInfo()
{
    ara::core::Future< GetSwClusterChangeInfoOutput > future{pkgmPtr_->GetSwClusterChangeInfo()};
    if (ara::core::future_status::timeout == future.wait_for(std::chrono::milliseconds(kApiWaitTime))) {
        LOG_ERROR << "future is not timeout!!! maybe disconnected!";
        return pkgmgr::SwClusterInfoVectorType{};
    }
    ara::core::Result< GetSwClusterChangeInfoOutput > const changeInfoResult{future.GetResult()};
    if (!changeInfoResult.HasValue()) {
        LOG_INFO << "ucmId=" << kUcmId.c_str()
                 << " GetSwClusterChangeInfo errmsg=" << changeInfoResult.Error().Message().data();
        return pkgmgr::SwClusterInfoVectorType{};
    }
    return changeInfoResult.Value().SwInfo;
}

/// @brief GetSwClusterDescription
/// @return SwDescVectorType
/// @throws no
pkgmgr::SwDescVectorType PackageManagementApp::GetSwClusterDescription()
{
    ara::core::Future< GetSwClusterDescriptionOutput > future{pkgmPtr_->GetSwClusterDescription()};
    if (ara::core::future_status::ready != future.wait_for(std::chrono::milliseconds(kApiWaitTime))) {
        LOG_ERROR << "future is not timeout!!! maybe disconnected!";
        return pkgmgr::SwDescVectorType{};
    }

    ara::core::Result< GetSwClusterDescriptionOutput > const result{future.GetResult()};
    if (!result.HasValue()) {
        LOG_INFO << "ucmId=" << kUcmId.c_str() << " GetSwClusterDescription errmsg=" << result.Error().Message().data();
        return pkgmgr::SwDescVectorType{};
    }
    return result.Value().SwCluster;
}

/// @brief GetSwPackages
/// @return SwPackageInfoVectorType
/// @throws no
pkgmgr::SwPackageInfoVectorType PackageManagementApp::GetSwPackages()
{
    ara::core::Future< GetSwPackagesOutput > future{pkgmPtr_->GetSwPackages()};
    if (ara::core::future_status::ready != future.wait_for(std::chrono::milliseconds(kApiWaitTime))) {
        LOG_ERROR << "future is not timeout!!! maybe disconnected!";
        return pkgmgr::SwPackageInfoVectorType{};
    }

    ara::core::Result< GetSwPackagesOutput > const swPackagesResult{future.GetResult()};
    if (!swPackagesResult.HasValue()) {
        LOG_INFO << "ucmId=" << kUcmId.c_str() << " GetSwPackages errmsg=" << swPackagesResult.Error().Message().data();
        return pkgmgr::SwPackageInfoVectorType{};
    }

    return swPackagesResult.Value().Packages;
}

/// @brief Check if the status is the expected value
/// @param askedStatus
/// @return True if the current status equals the expected status, otherwise false
/// @throws no
bool PackageManagementApp::_isCorrectStatus(pkgmgr::PackageManagerStatusType const askedStatus) const noexcept
{
    return (currentStatus_.load() == askedStatus);
}

/// @brief Callback for processing status change notifications.
/// @throws no
void PackageManagementApp::_onStatusChange()
{
    std::ignore = pkgmPtr_->CurrentStatus.GetNewSamples(
        [this](const auto state) {
            this->currentStatus_.store(*state);
            pkgmgr::PackageManagerStatusType const tempStatus{this->currentStatus_.load()};
            if (false == CurrentStatusToString(tempStatus).empty()) {
                LOG_INFO << "PackageManagementApp::_onStatusChange ucmId=" << kUcmId.c_str()
                         << " state transition notification :" << CurrentStatusToString(tempStatus).data();
            } else {
                LOG_ERROR << "PackageManagementApp::_onStatusChange ucmId=" << kUcmId.c_str() << " Empty status field.";
            }
            ///cond_.notify_one();.
        },
        1U);
}

/// @brief Subscribe to status field notifications.
/// @return Returns true on success, false on failure.
/// @throws no
bool PackageManagementApp::StatusSubscribe()
{
    if (pkgmPtr_->CurrentStatus.IsSubscribed()) {
        LOG_INFO << "status field callback already subscribed";
        return true;
    }

    // Register event receive callback, not mandatory to set a callback function
    std::ignore = pkgmPtr_->CurrentStatus.SetReceiveHandler([this]() { this->_onStatusChange(); });

    // Subscribe to field
    ara::core::Result< void > const r{pkgmPtr_->CurrentStatus.Subscribe(1U)};
    if (!r.HasValue()) {
        LOG_ERROR << "pkgmPtr_->CurrentStatus.Subscribe errmsg=" << r.Error().Message().data();
        return false;
    }

    // Initial update of service.
    // This is necessary in case application is being re-attached to the service
    // after a crash.
    ara::core::Result< pkgmgr::PackageManagerStatusType > const result{pkgmPtr_->CurrentStatus.Get().GetResult()};
    if (!result.HasValue()) {
        LOG_ERROR << "id=" << kUcmId.c_str() << " get CurrentStatus error=" << result.Error().Message().data();
        return false;
    }
    this->currentStatus_.store(result.Value());

    // pkgmPtr_ got initialized via callback.
    LOG_INFO << "Status field callback registered.";
    return true;
}

/// @brief
/// @note The field Get method returns quickly. If it does not return due to a network disconnection, it's a com component issue. If ucm blocks and does not return, it's a ucm issue.
/// That is, the com component must guarantee the reliability of method calls, and ucm must guarantee rapid return of method calls.
/// @return bool
/// @throws no
bool PackageManagementApp::UpdateStatus()
{
    log_.LogDebug() << "PackageManagementApp::UpdateStatus(), begin for kUcmId:" << kUcmId.c_str();

    ara::core::Future< pkgmgr::PackageManagerStatusType > future{pkgmPtr_->CurrentStatus.Get()};
    if (ara::core::future_status::timeout == future.wait_for(std::chrono::milliseconds(kApiWaitTime))) {
        log_.LogDebug() << "PackageManagementApp::UpdateStatus(), failed to wait_for for kUcmId:" << kUcmId.c_str()
                        << ",so will return false.";
        return false;
    }

    ara::core::Result< pkgmgr::PackageManagerStatusType > const result{future.GetResult()};
    if (!result.HasValue()) {
        log_.LogDebug() << "PackageManagementApp::UpdateStatus(), Get Error Message:" << result.Error().Message().data()
                        << "for kUcmId:" << kUcmId.c_str();
        return false;
    }
    currentStatus_.store(result.Value());
    log_.LogDebug() << "PackageManagementApp::UpdateStatus(), end with currentStatus_:"
                    << CurrentStatusToString(currentStatus_.load()).data() << "for kUcmId:" << kUcmId.c_str();
    return true;
}

/// @brief Get the current UCM status
/// @return Returns the current UCM status
/// @throws no
pkgmgr::PackageManagerStatusType PackageManagementApp::GetCurrentStatus() noexcept
{
    std::ignore = UpdateStatus();
    return currentStatus_.load();
}

/// @brief Transfer the software package at the given path
/// @param packagePath Full file path of the software package archive
/// @return result
/// @throws no
ara::core::Result< pkgmgr::TransferIdType > PackageManagementApp::TransferSoftwarePackage(
    ara::core::String const& packagePath) const
{
    DataTransfer< PackageManagementProxy > dataTransfer{pkgmPtr_};
    return this->TransferSoftwarePackage(packagePath, dataTransfer);
}

/// @brief Transfer the software package at the given path
/// @param packagePath Full file path of the software package archive
/// @param dataTransfer
/// @return result
/// @throws no
ara::core::Result< pkgmgr::TransferIdType > PackageManagementApp::TransferSoftwarePackage(
    ara::core::String const& packagePath, DataTransfer< PackageManagementProxy >& dataTransfer) const
{
    bool const found{pkgmgr::Filesystem::DoesFileExist(packagePath)};
    if (!found) {
        LOG_ERROR << "GetFileSize failed! maybe Software Package not found at path" << packagePath.c_str();
        return ara::core::Result< pkgmgr::TransferIdType >::FromError(pkgmgr::UCMErrorDomainErrc::kGeneralMemoryError);
    }

    uint64_t const fileSize{pkgmgr::tinyfs::GetFileSize(packagePath)};
    if (!dataTransfer.InitSession(fileSize)) {
        LOG_ERROR << "Initialization of transfer with size " << fileSize << " failed";
        return ara::core::Result< pkgmgr::TransferIdType >::FromError(pkgmgr::UCMErrorDomainErrc::kTransferInterrupted);
    }

    pkgmgr::TransferIdType const transferId{dataTransfer.GetTransferId()};

    if (!dataTransfer.Transfer(packagePath)) {
        LOG_ERROR << "Transfer of software package associated with TransferId="
                  << pkgmgr::helper::ToHexString(transferId).c_str() << " failed";
        return ara::core::Result< pkgmgr::TransferIdType >::FromError(pkgmgr::UCMErrorDomainErrc::kTransferInterrupted);
    }

    ara::core::Result< void > const exitRes{dataTransfer.ExitSession()};
    if (!exitRes.HasValue()) {
        LOG_ERROR << "Exiting transfer associated with TransferId " << pkgmgr::helper::ToHexString(transferId).c_str()
                  << " failed";
        return ara::core::Result< pkgmgr::TransferIdType >::FromError(exitRes.Error());
    }
    LOG_INFO << "ucmId=" << kUcmId.c_str() << "Transfer of Software Package was successful for session "
             << pkgmgr::helper::ToHexString(transferId).c_str();
    return ara::core::Result< pkgmgr::TransferIdType >::FromValue(transferId);
}

/// @brief Delete the specified software package
/// @param transferId
/// @return True if the package was successfully deleted, otherwise false
/// @throws no
bool PackageManagementApp::DeleteSoftwarePackage(pkgmgr::TransferIdType const& transferId) const
{
    log_.LogDebug() << "PackageManagementApp::DeleteSoftwarePackage(), begin with transferId:"
                    << pkgmgr::helper::ToHexString(transferId).c_str() << "for kUcmId:" << kUcmId.c_str();
    DataTransfer< PackageManagementProxy > dataTransfer{pkgmPtr_};
    return dataTransfer.DeleteSwPackage(transferId);
}

/// @brief Delete the transferred software package
/// @return True if the package was successfully deleted, otherwise false
/// @throws no
bool PackageManagementApp::DeleteSoftwarePackages() const
{  // Not used
    DataTransfer< PackageManagementProxy > dataTransfer{pkgmPtr_};
    GetSwPackagesOutput getSwPackageResult{dataTransfer.GetSwPackages()};

    pkgmgr::TransferIdType id;
    for (pkgmgr::SwPackageInfoVectorType::iterator iter{getSwPackageResult.Packages.begin()};
         iter != getSwPackageResult.Packages.end(); iter++) {
        pkgmgr::SwPackageInfoType const& entry{(*iter)};
        id = entry.TransferID;
        if (!dataTransfer.DeleteSwPackage(id)) {
            LOG_WARN << "ucmId=" << kUcmId.c_str() << "Delete package failed for TransferID "
                     << pkgmgr::helper::ToHexString(id).c_str();
        }
    }

    getSwPackageResult = dataTransfer.GetSwPackages();
    if (false == getSwPackageResult.Packages.empty()) {
        LOG_WARN << "ucmId=" << kUcmId.c_str() << "Package deletion failed."
                 << "Remaining packages: " << getSwPackageResult.Packages.size();
        return false;
    }
    return true;
}

/// @brief Install the latest transferred software package. If no software package has been transferred, this method has no effect.
/// @param transferId
/// @return True if the installation is successful, false otherwise
/// @throws no
bool PackageManagementApp::Install(pkgmgr::TransferIdType const transferId)
{
    log_.LogDebug() << "PackageManagementApp::Install(), begin with transferId:"
                    << pkgmgr::helper::ToHexString(transferId).c_str() << "for kUcmId:" << kUcmId.c_str();

    // need to check that UCM subordinate is in kReady or kIdle state before processing any package
    if ((!this->_isCorrectStatus(pkgmgr::PackageManagerStatusType::kIdle))
        && (!this->_isCorrectStatus(pkgmgr::PackageManagerStatusType::kReady))) {
        log_.LogError() << "PackageManagementApp::Install(), can't ProcessSwPackage in currentStatus_:"
                        << CurrentStatusToString(currentStatus_).data()
                        << ", so will TriggerCancel for kUcmId:" << kUcmId.c_str();
        // Cancel Campaign
        FsmManager::GetInstance()->TriggerCancelInTQ(pkgmgr::UCMMasterResolutionType::kUCMError);
        return false;
    }

    ara::core::Result< void > const result{pkgmPtr_->ProcessSwPackage(transferId).GetResult()};
    if (!result.HasValue()) {
        log_.LogError() << "PackageManagementApp::Install(), failed to ProcessSwPackage for transferId:"
                        << pkgmgr::helper::ToHexString(transferId).c_str()
                        << "with error:" << result.Error().Message().data()
                        << ", so will TriggerCancel for kUcmId:" << kUcmId.c_str();

        pkgmgr::UCMStepErrorType const ucmStepError{kUcmId, pkgmgr::SoftwarePackageStepType::kProcess,
                                                    static_cast< uint8_t >(result.Error().Value())};
        std::ignore = FsmManager::GetInstance()->SaveUcmStepError(ucmStepError);

        // Cancel Campaign
        FsmManager::GetInstance()->TriggerCancelInTQ(pkgmgr::UCMMasterResolutionType::kUCMError);
        return false;
    }
    log_.LogDebug() << "PackageManagementApp::Install(), end with success for transferId:"
                    << pkgmgr::helper::ToHexString(transferId).c_str() << "kUcmId:" << kUcmId.c_str();
    return true;
}

// Note: The goal of Activate is not to bring the UCM to IDLE state
/// @brief Activate
/// @return Returns true if activation is successful, false otherwise
/// @throws no
bool PackageManagementApp::Activate()
{
    log_.LogDebug() << "PackageManagementApp::Activate(), begin with currentStatus_:"
                    << CurrentStatusToString(currentStatus_) << "for kUcmId:" << kUcmId.c_str();
    bool result{true};
    bool canMoveForward{true};  // Whether can continue
    while ((canMoveForward = FsmManager::GetInstance()->CanMoveForward())) {
        log_.LogDebug() << "PackageManagementApp::Activate(), now currentStatus_:"
                        << CurrentStatusToString(currentStatus_) << "for kUcmId:" << kUcmId.c_str();
        if (pkgmgr::PackageManagerStatusType::kReady == currentStatus_) {
            log_.LogDebug() << "PackageManagementApp::Activate(), try Activate for kUcmId:" << kUcmId.c_str();
            ara::core::Future< void > future;      // Operation future
            future = ara::core::Future< void >{};  // meaningless, just for qac
            std::uint64_t operationStartTime{0U};  // Operation start time
            {
                std::unique_lock< std::mutex > lock{serviceMutex_};
                log_.LogDebug() << "PackageManagementApp::Activate(), get serviceAvailable_:" << serviceAvailable_
                                << "for kUcmId:" << kUcmId.c_str();

                // If service unavailable, wait for service to become available
                if (!serviceAvailable_.load()) {
                    if (!_waitServiceAvailable(lock)) {  // If service not available within timeout, end
                        log_.LogDebug() << "PackageManagementApp::Activate(), can't WaitServiceAvailable for kUcmId:"
                                        << kUcmId.c_str();
                        result = false;
                        break;
                    }
                }
                // Record operation start time:
                operationStartTime = NowMS();
                log_.LogDebug() << "PackageManagementApp::Activate(), get operationStartTime:" << operationStartTime
                                << "for kUcmId:" << kUcmId.c_str();

                //  Start executing operation:
                log_.LogDebug() << "PackageManagementApp::Activate(), do Activate for kUcmId:" << kUcmId.c_str();
                future = pkgmPtr_->Activate();
            }

            bool alreadyReboot{false};
            int32_t counter{0};
            int32_t const counterLimit{50};
            while ((
                canMoveForward
                = FsmManager::GetInstance()
                      ->CanMoveForward())) {  // Wait for result (Activate may trigger target UCM restart / target UCM may restart unexpectedly).
                log_.LogDebug() << "PackageManagementApp::Activate(), try wait_for for Activate for kUcmId:"
                                << kUcmId.c_str();
                counter++;
                ara::core::future_status const futureStatus{future.wait_for(std::chrono::milliseconds(
                    kGlobalWaitTime))};  // TODO(Han Zhibo): Use a condition variable to wait for cancellation or abort and future value (then function)
                if (ara::core::future_status::timeout == futureStatus) {  // Timeout
                    // When using future to wait for the operation result, if the waiting time (custom duration per operation) is too long, check whether service unavailability occurred between operationStartTime and now (compare with lastServiceUnavailableTime_),
                    //  If service unavailability occurred (regardless of how recent) AND the service is available again, it is considered that the target ucm has restarted: alreadyReboot? en
                    const bool serviceAvail                 = serviceAvailable_.load();
                    const bool isServiceUnavailableNotified = _isServiceUnavailableNotified(operationStartTime);
                    if ((counter >= counterLimit) && isServiceUnavailableNotified && serviceAvail) {
                        log_.LogDebug() << "PackageManagementApp::Activate(), waiting too long and there is a service "
                                           "unvailable notified since and now serviceAvailable_:"
                                        << serviceAvailable_ << ",so set alreadyReboot for kUcmId:" << kUcmId.c_str();
                        alreadyReboot = true;
                        break;
                    }
                    continue;  // Continue waiting
                }
                // Result available
                log_.LogDebug() << "PackageManagementApp::Activate(), have got result for Activate for kUcmId:"
                                << kUcmId.c_str();
                break;
            }

            if (!canMoveForward) {
                log_.LogDebug() << "PackageManagementApp::Activate(), can't move forward for kUcmId:" << kUcmId.c_str();
                break;
            }
            if (alreadyReboot) {  // Whether restarted
                log_.LogDebug() << "PackageManagementApp::Activate(), try to WaitStatus because of alreadyReboot:"
                                << alreadyReboot << "for kUcmId:" << kUcmId.c_str();
                std::this_thread::sleep_for(std::chrono::milliseconds(kMs500));
                int32_t const ret{_waitStatus(
                    pkgmgr::PackageManagerStatusType::kReady,
                    {pkgmgr::PackageManagerStatusType::kReady,
                     pkgmgr::PackageManagerStatusType::
                         kActivating,  // If currentStatus_ is Ready state, it will re-Activate, but this only causes an extra erroneous action.
                     pkgmgr::PackageManagerStatusType::kVerifying, pkgmgr::PackageManagerStatusType::kActivated,
                     pkgmgr::PackageManagerStatusType::kRollingBack, pkgmgr::PackageManagerStatusType::kRolledBack})};
                log_.LogDebug() << "PackageManagementApp::Activate(),  WaitStatus ret:" << ret
                                << "for kUcmId:" << kUcmId.c_str();
                if (0 == ret) {  // Reached expected status
                    continue;    // Restart
                }
                if (1 == ret) {  // Reached unexpected status
                    // Cancel Campaign
                    FsmManager::GetInstance()->TriggerCancelInTQ(pkgmgr::UCMMasterResolutionType::kUCMError);
                    result = false;
                    break;
                }
                result = false;
                break;
            }
            // Result available
            ara::core::Result< void > const res{future.GetResult()};
            if (!res.HasValue()) {                                                              // Error occurred
                if (res.CheckError(ara::ucm::pkgmgr::UCMErrorDomainErrc::kResettingMachine)) {  // Restart
                    log_.LogDebug() << "PackageManagementApp::Activate(), kUcmId:" << kUcmId.c_str()
                                    << "will be rebooted.";
                    int32_t const ret{_waitStatus(
                        pkgmgr::PackageManagerStatusType::kReady,
                        {pkgmgr::PackageManagerStatusType::
                             kActivating,  // If currentStatus_ returns Ready state, Activate will inevitably fail
                         pkgmgr::PackageManagerStatusType::kVerifying, pkgmgr::PackageManagerStatusType::kActivated,
                         pkgmgr::PackageManagerStatusType::kRollingBack,
                         pkgmgr::PackageManagerStatusType::kRolledBack})};

                    log_.LogDebug() << "PackageManagementApp::Activate(),  WaitStatus ret:" << ret
                                    << "for kUcmId:" << kUcmId.c_str();
                    if (0 == ret) {  // Reached expected status
                        continue;    // Restart
                    }
                    if (1 == ret) {  // Reached unexpected status
                        // Cancel Campaign
                        FsmManager::GetInstance()->TriggerCancelInTQ(pkgmgr::UCMMasterResolutionType::kUCMError);
                        result = false;
                        break;
                    }
                    result = false;
                    break;
                }
                if (helper::CheckResultIsError(
                        res, {ara::com::ComErrc::kGrantEnforcementError, ara::com::ComErrc::kNetworkBindingFailure,
                              ara::com::ComErrc::kServiceNotAvailable})) {
                    log_.LogDebug() << "PackageManagementApp::Activate(), get comErr:" << res.Error().Message().data()
                                    << ", so will try again later for kUcmId:" << kUcmId.c_str();
                    std::this_thread::sleep_for(std::chrono::milliseconds(kMs100));
                    continue;
                }
                log_.LogError() << "PackageManagementApp::Activate(), failed to Activate for ucmId:" << kUcmId.c_str()
                                << "with errmsg:" << res.Error().Message().data()
                                << ", so will TriggerCancel for kUcmId:" << kUcmId.c_str();
                result = false;

                pkgmgr::UCMStepErrorType const ucmStepError{kUcmId, pkgmgr::SoftwarePackageStepType::kVerify,
                                                            static_cast< uint8_t >(res.Error().Value())};
                std::ignore = FsmManager::GetInstance()->SaveUcmStepError(ucmStepError);

                // Cancel Campaign
                FsmManager::GetInstance()->TriggerCancelInTQ(pkgmgr::UCMMasterResolutionType::kUCMError);
                break;
            }
            // When Activate involves a restart, it will not reach this branch
            log_.LogDebug() << "PackageManagementApp::Activate(), succeed to Activate for ucmId:" << kUcmId.c_str();
            result = true;
            break;
        }
        if (pkgmgr::PackageManagerStatusType::kActivating == currentStatus_) {
            log_.LogDebug() << "PackageManagementApp::Activate(), try to WaitStatus because of "
                               "currentStatus_:kActivating for ucmId:"
                            << kUcmId.c_str();
            int32_t const ret{_waitStatus(
                pkgmgr::PackageManagerStatusType::kActivating,
                {pkgmgr::PackageManagerStatusType::kVerifying, pkgmgr::PackageManagerStatusType::kActivated,
                 pkgmgr::PackageManagerStatusType::kRollingBack, pkgmgr::PackageManagerStatusType::kRolledBack})};

            log_.LogDebug() << "PackageManagementApp::Activate(),  WaitStatus ret:" << ret
                            << "for ucmId:" << kUcmId.c_str();
            if (0 == ret) {  // Reached expected status
                continue;    // Restart
            }
            if (1 == ret) {  // Reached unexpected status
                // Cancel Campaign
                FsmManager::GetInstance()->TriggerCancelInTQ(pkgmgr::UCMMasterResolutionType::kUCMError);
                result = false;
                break;
            }
            result = false;
            break;
        }
        if (pkgmgr::PackageManagerStatusType::kVerifying == currentStatus_) {
            log_.LogDebug()
                << "PackageManagementApp::Activate(), try to WaitStatus because of currentStatus_:kVerifying for ucmId:"
                << kUcmId.c_str();
            int32_t const ret{_waitStatus(
                pkgmgr::PackageManagerStatusType::kVerifying,
                {pkgmgr::PackageManagerStatusType::kActivated, pkgmgr::PackageManagerStatusType::kRollingBack,
                 pkgmgr::PackageManagerStatusType::kRolledBack})};

            log_.LogDebug() << "PackageManagementApp::Activate(),  WaitStatus ret:" << ret
                            << "for kUcmId:" << kUcmId.c_str();
            if (0 == ret) {  // Reached expected status
                continue;    // Restart
            }
            if (1 == ret) {  // Reached unexpected status
                // Cancel Campaign
                FsmManager::GetInstance()->TriggerCancelInTQ(pkgmgr::UCMMasterResolutionType::kUCMError);
                result = false;
                break;
            }
            result = false;
            break;
        }
        if (pkgmgr::PackageManagerStatusType::kActivated == currentStatus_) {
            log_.LogDebug() << "PackageManagementApp::Activate(), scceed to Activate for kUcmId:" << kUcmId.c_str();
            result = true;
            break;
        }
        // else if((pkgmgr::PackageManagerStatusType::kRollingBack == currentStatus_) || (pkgmgr::PackageManagerStatusType::kRolledBack == currentStatus_)) {.
        if (0U
            != std::set< pkgmgr::PackageManagerStatusType >{pkgmgr::PackageManagerStatusType::kRollingBack,
                                                            pkgmgr::PackageManagerStatusType::kRolledBack}
                   .count(currentStatus_)) {  // just for qac
            log_.LogError() << "PackageManagementApp::Activate(), failed to Activate for kUcmId:" << kUcmId.c_str()
                            << ", so will TriggerCancel.";

            // Cancel Campaign
            FsmManager::GetInstance()->TriggerCancelInTQ(pkgmgr::UCMMasterResolutionType::kUCMError);
            result = false;
            break;
        }
        log_.LogDebug() << "PackageManagementApp::Activate(), can't Activate in currentStatus_:"
                        << CurrentStatusToString(currentStatus_) << "for kUcmId:" << kUcmId.c_str()
                        << ", so will TriggerCancel.";
        result = false;

        // Cancel Campaign
        FsmManager::GetInstance()->TriggerCancelInTQ(pkgmgr::UCMMasterResolutionType::kUCMError);
        break;
    }

    log_.LogDebug() << "PackageManagementApp::Activate(), end with result:" << result
                    << "canMoveForward:" << canMoveForward << "for kUcmId:" << kUcmId.c_str();
    return result && canMoveForward;
}

// Note: The goal of Finish is not to bring the UCM to IDLE state
/// @brief Complete the final installation process. Called after Activate, Rollback, or Cancel
/// @return Returns true on success, false on failure.
/// @throws no
bool PackageManagementApp::Finish()
{
    log_.LogDebug() << "PackageManagementApp::Finish(), begin with currentStatus_:"
                    << CurrentStatusToString(currentStatus_) << "for kUcmId:" << kUcmId.c_str();
    bool result{true};
    bool canWorkAsMaster{true};  // Whether can continue
    while ((canWorkAsMaster = FsmManager::GetInstance()->CanWorkAsMaster())) {
        log_.LogDebug() << "PackageManagementApp::Finish(), now currentStatus_:"
                        << CurrentStatusToString(currentStatus_).data() << "for kUcmId:" << kUcmId.c_str();
        if (pkgmgr::PackageManagerStatusType::kActivated == currentStatus_) {
            log_.LogDebug() << "PackageManagementApp::Finish(), try Finish for kUcmId:" << kUcmId.c_str();
            ara::core::Future< void > future;      // Operation future
            future = ara::core::Future< void >{};  // meaningless, just for qac
            std::uint64_t operationStartTime{0U};  // Operation start time
            {
                std::unique_lock< std::mutex > lock{serviceMutex_};
                log_.LogDebug() << "PackageManagementApp::Finish(), get serviceAvailable_:" << serviceAvailable_
                                << "for kUcmId:" << kUcmId.c_str();

                // If service unavailable, wait for service to become available
                if (!serviceAvailable_.load()) {
                    if (!_waitServiceAvailable(lock)) {  // If service not available within timeout, end
                        log_.LogDebug() << "PackageManagementApp::Finish(), can't WaitServiceAvailable.";
                        result = false;
                        break;
                    }
                }
                // Record operation start time:
                operationStartTime = NowMS();
                log_.LogDebug() << "PackageManagementApp::Finish(), get operationStartTime:" << operationStartTime
                                << "for kUcmId:" << kUcmId.c_str();

                //  Start executing operation:
                log_.LogDebug() << "PackageManagementApp::Finish(), do Finish for kUcmId:" << kUcmId.c_str();
                future = pkgmPtr_->Finish();
            }

            bool alreadyReboot{false};
            int32_t counter{0};
            int32_t const counterLimit{25};
            while (
                (canWorkAsMaster = FsmManager::GetInstance()
                                       ->CanWorkAsMaster())) {  // Wait for result (target UCM may restart unexpectedly)
                log_.LogDebug() << "PackageManagementApp::Finish(), try wait_for for Finish for kUcmId:"
                                << kUcmId.c_str();
                counter++;
                ara::core::future_status const futureStatus{future.wait_for(std::chrono::milliseconds(
                    kGlobalWaitTime))};  // TODO(Han Zhibo): Use a condition variable to wait for cancellation or abort and future value (then function)
                if (ara::core::future_status::timeout == futureStatus) {  // Timeout
                    // When using future to wait for the operation result, if the waiting time (custom duration per operation) is too long, check whether service unavailability occurred between operationStartTime and now (compare with lastServiceUnavailableTime_),
                    //  If service unavailability occurred (regardless of how recent) AND the service is available again, it is considered that the target ucm has restarted: alreadyReboot? en

                    const bool serviceAvail                 = serviceAvailable_.load();
                    const bool isServiceUnavailableNotified = _isServiceUnavailableNotified(operationStartTime);
                    if ((counter >= counterLimit) && isServiceUnavailableNotified && serviceAvail) {
                        log_.LogDebug() << "PackageManagementApp::Finish(), waiting too long and there is a service "
                                           "unvailable notified since and now serviceAvailable_:"
                                        << serviceAvailable_ << ",so set alreadyReboot for kUcmId:" << kUcmId.c_str();
                        alreadyReboot = true;
                        break;
                    }
                    continue;  // Continue waiting
                }
                // Result available
                log_.LogDebug() << "PackageManagementApp::Finish(), have got result for Finish for kUcmId:"
                                << kUcmId.c_str();
                break;
            }

            if (!canWorkAsMaster) {
                log_.LogDebug() << "PackageManagementApp::Finish(), can't work as master for kUcmId:" << kUcmId.c_str();
                break;
            }
            if (alreadyReboot) {  // Whether reconnected
                log_.LogDebug() << "PackageManagementApp::Finish(), try to WaitStatus because of alreadyReboot:"
                                << alreadyReboot << "for kUcmId:" << kUcmId.c_str();
                std::this_thread::sleep_for(std::chrono::milliseconds(kMs500));
                int32_t const ret{_waitStatus(
                    pkgmgr::PackageManagerStatusType::kActivated,
                    {pkgmgr::PackageManagerStatusType::kActivated, pkgmgr::PackageManagerStatusType::kCleaningUp,
                     pkgmgr::PackageManagerStatusType::kIdle})};

                log_.LogDebug() << "PackageManagementApp::Finish(),  WaitStatus ret:" << ret
                                << "for kUcmId:" << kUcmId.c_str();
                if (0 == ret) {
                    continue;  // Restart
                }
                if (1 == ret) {
                    // Exit Campaign
                    std::ignore = TaskQueue::GetInstance()->PushTask([]() -> int32_t {
                        FsmManager::GetInstance()->Abort();
                        return 0;
                    });

                    result = false;
                    break;
                }
                result = false;
                break;
            }
            // Result available
            ara::core::Result< void > const res{future.GetResult()};
            if (!res.HasValue()) {  // Error occurred
                if (helper::CheckResultIsError(
                        res, {ara::com::ComErrc::kGrantEnforcementError, ara::com::ComErrc::kNetworkBindingFailure,
                              ara::com::ComErrc::kServiceNotAvailable})) {
                    log_.LogDebug() << "PackageManagementApp::Finish(), get comErr:" << res.Error().Message().data()
                                    << ", so will try again later for kUcmId:" << kUcmId.c_str();
                    std::this_thread::sleep_for(std::chrono::milliseconds(kMs100));
                    continue;
                }

                log_.LogError() << "PackageManagementApp::Finish(), failed to Finish for ucmId:" << kUcmId.c_str()
                                << "with errmsg:" << res.Error().Message().data()
                                << ", so will abort for kUcmId:" << kUcmId.c_str();
                result = false;

                // Exit Campaign
                std::ignore = TaskQueue::GetInstance()->PushTask([]() -> int32_t {
                    FsmManager::GetInstance()->Abort();
                    return 0;
                });
                break;
            }
            // Success
            log_.LogDebug() << "PackageManagementApp::Finish(), succeed to Finish for ucmId:" << kUcmId.c_str();
            result = true;
            break;
        }
        // else if((pkgmgr::PackageManagerStatusType::kCleaningUp == currentStatus_) ||.
        //     (pkgmgr::PackageManagerStatusType::kIdle == currentStatus_)) {.
        if (0U
            != std::set< pkgmgr::PackageManagerStatusType >{pkgmgr::PackageManagerStatusType::kCleaningUp,
                                                            pkgmgr::PackageManagerStatusType::kIdle}
                   .count(currentStatus_)) {  // just for qac
            log_.LogDebug() << "PackageManagementApp::Finish(), succeed to have Finished for ucmId:" << kUcmId.c_str()
                            << "because of currentStatus_:" << CurrentStatusToString(currentStatus_).data();
            result = true;
            break;
        }

        log_.LogDebug() << "PackageManagementApp::Finish(), can't Finish in currentStatus_:"
                        << CurrentStatusToString(currentStatus_).data() << "for kUcmId:" << kUcmId.c_str()
                        << ", so will abort.";
        result = false;
        // Exit Campaign
        std::ignore = TaskQueue::GetInstance()->PushTask([]() -> int32_t {
            FsmManager::GetInstance()->Abort();
            return 0;
        });
        break;
    }

    log_.LogDebug() << "PackageManagementApp::Finish(), end with result:" << result
                    << "canWorkAsMaster:" << canWorkAsMaster << "for kUcmId:" << kUcmId.c_str();
    return result && canWorkAsMaster;
}

/// @brief Rollback the latest installed and "activated" software package
/// @return True if rollback is successful, false otherwise
/// @throws no
bool PackageManagementApp::Rollback()
{
    if (!this->_areChangesPending()) {
        // If this UCM has no software changes, calling rollback directly returns true
        return true;
    }
    ara::core::Result< void > const result{pkgmPtr_->Rollback().GetResult()};
    if (!result.HasValue()) {
        LOG_ERROR << "call failed! ucmId" << kUcmId.c_str() << " errmsg=" << result.Error().Message().data();
        return false;
    }
    LOG_INFO << "call successfully. ucmId=" << kUcmId.c_str();
    return true;
}

/// @brief Cancel the current processing
/// @param processingTransferId Id of the package being processed
/// @return Returns true on success, false on failure.
/// @throws no
bool PackageManagementApp::Cancel(pkgmgr::TransferIdType const processingTransferId)
{
    LOG_INFO << "begin";
    ara::core::Result< void > const result{pkgmPtr_->Cancel(processingTransferId).GetResult()};
    if (!result.HasValue()) {
        LOG_ERROR << "call failed! software package ID =" << pkgmgr::helper::ToHexString(processingTransferId).c_str()
                  << " ucmId=" << kUcmId.c_str() << " errmsg=" << result.Error().Message().data();
        return false;
    }
    LOG_INFO << "call successfully transferId=" << pkgmgr::helper::ToHexString(processingTransferId).c_str()
             << " ucmId=" << kUcmId.c_str();
    return true;
}

/// @brief Revert processed software packages
/// @returns True if RevertProcessedSwPackages is successful, false otherwise
/// @throws no
bool PackageManagementApp::_revertProcessedSwPackages()
{
    LOG_INFO << "begin";
    ara::core::Result< void > const result{pkgmPtr_->RevertProcessedSwPackages().GetResult()};
    if (!result.HasValue()) {
        LOG_ERROR << "call failed! ucmId=" << kUcmId.c_str() << " errmsg=" << result.Error().Message().data();
        return false;
    }
    LOG_INFO << "call successful. ucmId=" << kUcmId.c_str();
    return true;
}

/// @brief WaitStatus
/// @param initialStatus
/// @param expectedStatus
/// @return 0: expected status reached, which may include the initial status; 1: status other than initial and expected status reached; 2: ended waiting because cannot move forward
/// @throws no
int32_t PackageManagementApp::_waitStatus(pkgmgr::PackageManagerStatusType const initialStatus,
                                          std::set< pkgmgr::PackageManagerStatusType > expectedStatus) const
{
    ara::core::String strExpectedStatus;
    for (pkgmgr::PackageManagerStatusType const& st : expectedStatus) {
        std::ignore = strExpectedStatus.append(CurrentStatusToString(st).data());
        std::ignore = strExpectedStatus.append(",");
    }
    if (!strExpectedStatus.empty()) {
        strExpectedStatus.pop_back();
    }

    log_.LogDebug() << "PackageManagementApp::_waitStatus(), begin with initialStatus:"
                    << CurrentStatusToString(initialStatus).data() << "expectedStatus:" << strExpectedStatus.c_str()
                    << "for kUcmId:" << kUcmId.c_str();

    while (FsmManager::GetInstance()->CanMoveForward()) {
        std::set< pkgmgr::PackageManagerStatusType >::iterator const iter{expectedStatus.find(currentStatus_.load())};
        if (iter != expectedStatus.end()) {  // Expected status reached
            log_.LogDebug() << "PackageManagementApp::_waitStatus(), end because of expected currentStatus_:"
                            << CurrentStatusToString(currentStatus_).data() << "for kUcmId:" << kUcmId.c_str();
            return 0;
        }
        if (currentStatus_ == initialStatus) {  // Current status is initial status InitialStatus
            log_.LogDebug() << "PackageManagementApp::_waitStatus(), currentStatus_:"
                            << CurrentStatusToString(currentStatus_).data()
                            << ", so try to check again later for kUcmId:" << kUcmId.c_str();
            std::this_thread::sleep_for(std::chrono::milliseconds(kMs200));
            continue;
        }

        log_.LogDebug() << "PackageManagementApp::_waitStatus(), end because of unexpected currentStatus_:"
                        << CurrentStatusToString(currentStatus_).data() << "for kUcmId:" << kUcmId.c_str();
        return 1;
    }
    log_.LogDebug() << "PackageManagementApp::_waitStatus(), end because of Can't Move Forward for kUcmId:"
                    << kUcmId.c_str();
    return 2;
}

// The goal is to bring the UCM to IDLE state
/// @brief Attempt rollback to restore to kIdle state
/// @return Returns True: rollback successful, false: rollback failed
/// @throws no
bool PackageManagementApp::AttemptStateRecovery()
{
    log_.LogDebug() << "PackageManagementApp::AttemptStateRecovery(), begin with currentStatus_:"
                    << CurrentStatusToString(currentStatus_).data() << "for kUcmId:" << kUcmId.c_str();

    bool result{true};
    bool canWorkAsMaster{true};  // Whether can continue
    while ((canWorkAsMaster = FsmManager::GetInstance()->CanWorkAsMaster())) {
        log_.LogDebug() << "PackageManagementApp::AttemptStateRecovery(), now currentStatus_:"
                        << CurrentStatusToString(currentStatus_).data() << "for kUcmId:" << kUcmId.c_str();
        if (pkgmgr::PackageManagerStatusType::kIdle == currentStatus_) {
            // TODO(Han Zhibo): If the software cluster versions involved in the campaign do not exist on the target UCM, then: AttemptStateRecovery succeeds, otherwise fails.
            log_.LogDebug()
                << "PackageManagementApp::AttemptStateRecovery(), break with success because of currentStatus_:"
                << CurrentStatusToString(currentStatus_).data() << "for kUcmId:" << kUcmId.c_str();
            result = true;
            break;
        }
        if (pkgmgr::PackageManagerStatusType::kCleaningUp == currentStatus_) {
            log_.LogDebug()
                << "PackageManagementApp::AttemptStateRecovery(), try to WaitStatus for kCleaningUp for kUcmId:"
                << kUcmId.c_str();
            int32_t const ret{
                _waitStatus(pkgmgr::PackageManagerStatusType::kCleaningUp, {pkgmgr::PackageManagerStatusType::kIdle})};
            log_.LogDebug() << "PackageManagementApp::AttemptStateRecovery(),  WaitStatus ret:" << ret
                            << "for kUcmId:" << kUcmId.c_str();
            if (0 == ret) {
                continue;  // Restart
            }
            if (1 == ret) {
                // Exit Campaign
                std::ignore = TaskQueue::GetInstance()->PushTask([]() -> int32_t {
                    FsmManager::GetInstance()->Abort();
                    return 0;
                });
                result      = false;
                break;
            }
            result = false;
            break;
        }
        if (pkgmgr::PackageManagerStatusType::kProcessing == currentStatus_) {
            log_.LogDebug() << "PackageManagementApp::AttemptStateRecovery(), try Cancel for kUcmId:" << kUcmId.c_str();
            // TODO(Han Zhibo): Add CanWorkAsMaster inside
            ara::core::Result< pkgmgr::TransferIdType > const processIdResult{_getProcessingTransferId()};
            if (!processIdResult.HasValue()) {  // Failed to get the software package being processed
                continue;                       // Restart
            }

            ara::core::Future< void > future;      // Operation future
            future = ara::core::Future< void >{};  // meaningless, just for qac
            std::uint64_t operationStartTime{0U};  // Operation start time
            {
                std::unique_lock< std::mutex > lock{serviceMutex_};
                log_.LogDebug() << "PackageManagementApp::AttemptStateRecovery(), get serviceAvailable_:"
                                << serviceAvailable_ << "for kUcmId:" << kUcmId.c_str();

                // If service unavailable, wait for service to become available
                if (!serviceAvailable_.load()) {
                    if (!_waitServiceAvailable(lock)) {  // If service not available within timeout, end
                        log_.LogDebug()
                            << "PackageManagementApp::AttemptStateRecovery(), can't WaitServiceAvailable for kUcmId:"
                            << kUcmId.c_str();
                        result = false;
                        break;
                    }
                }
                // Record operation start time:
                operationStartTime = NowMS();
                log_.LogDebug() << "PackageManagementApp::AttemptStateRecovery(), get operationStartTime:"
                                << operationStartTime << "for kUcmId:" << kUcmId.c_str();

                //  Start executing operation:
                log_.LogDebug() << "PackageManagementApp::AttemptStateRecovery(), do Cancel for kUcmId:"
                                << kUcmId.c_str();
                future = pkgmPtr_->Cancel(processIdResult.Value());
            }

            bool alreadyReboot{false};
            int32_t counter{0};
            int32_t const counterLimit{25};
            while ((canWorkAsMaster = FsmManager::GetInstance()->CanWorkAsMaster())) {
                // Wait for result (target UCM may restart unexpectedly)
                log_.LogDebug() << "PackageManagementApp::AttemptStateRecovery(), try wait_for for Cancel for kUcmId:"
                                << kUcmId.c_str();
                counter++;
                ara::core::future_status const futureStatus{future.wait_for(std::chrono::milliseconds(
                    kGlobalWaitTime))};  // TODO(Han Zhibo): Use a condition variable to wait for cancellation or abort and future value (then function)
                if (ara::core::future_status::timeout == futureStatus) {  // Timeout
                    // When using future to wait for the operation result, if the waiting time (custom duration per operation) is too long, check whether service unavailability occurred between operationStartTime and now (compare with lastServiceUnavailableTime_),
                    //  If service unavailability occurred (regardless of how recent) AND the service is available again, it is considered that the target ucm has restarted: alreadyReboot? en
                    const bool serviceAvail                 = serviceAvailable_.load();
                    const bool isServiceUnavailableNotified = _isServiceUnavailableNotified(operationStartTime);
                    if ((counter >= counterLimit) && isServiceUnavailableNotified && serviceAvail) {
                        log_.LogDebug() << "PackageManagementApp::AttemptStateRecovery(), waiting too long and there "
                                           "is a service unvailable notified since and now serviceAvailable_:"
                                        << serviceAvailable_ << ",so set alreadyReboot for kUcmId:" << kUcmId.c_str();
                        alreadyReboot = true;
                        break;
                    }
                    continue;  // Continue waiting
                }
                // Result available
                log_.LogDebug()
                    << "PackageManagementApp::AttemptStateRecovery(), have got result for Cancel for kUcmId:"
                    << kUcmId.c_str();
                break;
            }

            if (!canWorkAsMaster) {
                log_.LogDebug() << "PackageManagementApp::AttemptStateRecovery(), can't work as master for kUcmId:"
                                << kUcmId.c_str();
                break;
            }
            if (alreadyReboot) {  // Whether restarted
                log_.LogDebug()
                    << "PackageManagementApp::AttemptStateRecovery(), try to WaitStatus because of alreadyReboot:"
                    << alreadyReboot << "for kUcmId:" << kUcmId.c_str();
                std::this_thread::sleep_for(std::chrono::milliseconds(kMs500));
                int32_t const ret{
                    _waitStatus(pkgmgr::PackageManagerStatusType::kProcessing,
                                {pkgmgr::PackageManagerStatusType::kProcessing, pkgmgr::PackageManagerStatusType::kIdle,
                                 pkgmgr::PackageManagerStatusType::kReady})};
                log_.LogDebug() << "PackageManagementApp::AttemptStateRecovery(),  WaitStatus ret:" << ret
                                << "for kUcmId:" << kUcmId.c_str();
                if (0 == ret) {
                    continue;  // Restart
                }
                if (1 == ret) {
                    // Exit Campaign
                    std::ignore = TaskQueue::GetInstance()->PushTask([]() -> int32_t {
                        FsmManager::GetInstance()->Abort();
                        return 0;
                    });
                    result      = false;
                    break;
                }
                result = false;
                break;
            }
            // Result available
            ara::core::Result< void > const res{future.GetResult()};
            if (!res.HasValue()) {  // Error occurred
                // if (res.CheckError(ara::ucm::pkgmgr::UCMErrorDomainErrc::kCancelFailed) ||.
                //     res.CheckError(ara::ucm::pkgmgr::UCMErrorDomainErrc::kOperationNotPermitted)) {.
                if (helper::CheckResultIsError(res, {ara::ucm::pkgmgr::UCMErrorDomainErrc::kCancelFailed,
                                                     ara::ucm::pkgmgr::UCMErrorDomainErrc::kOperationNotPermitted})) {
                    log_.LogDebug() << "PackageManagementApp::AttemptStateRecovery(), Cancel get Error:"
                                    << res.Error().Message().data()
                                    << ", so will WaitStatus for kUcmId:" << kUcmId.c_str();
                    int32_t const ret{
                        _waitStatus(pkgmgr::PackageManagerStatusType::kProcessing,
                                    {pkgmgr::PackageManagerStatusType::kReady})};  // Can still cancel in kReady state
                    log_.LogDebug() << "PackageManagementApp::AttemptStateRecovery(), WaitStatus ret:" << ret
                                    << "for kUcmId:" << kUcmId.c_str();
                    if (0 == ret) {
                        continue;  // Restart
                    }
                    if (1 == ret) {
                        // Exit Campaign
                        std::ignore = TaskQueue::GetInstance()->PushTask([]() -> int32_t {
                            FsmManager::GetInstance()->Abort();
                            return 0;
                        });
                        result      = false;
                        break;
                    }
                    result = false;
                    break;
                }
                // else if (res.CheckError(ara::ucm::pkgmgr::UCMErrorDomainErrc::kInvalidTransferId) || res.CheckError(ara::com::ComErrc::kGrantEnforcementError) || res.CheckError(ara::com::ComErrc::kNetworkBindingFailure) || res.CheckError(ara::com::ComErrc::kServiceNotAvailable)) {.
                if (helper::CheckResultIsError(
                        res, {ara::ucm::pkgmgr::UCMErrorDomainErrc::kInvalidTransferId,
                              ara::com::ComErrc::kGrantEnforcementError, ara::com::ComErrc::kNetworkBindingFailure,
                              ara::com::ComErrc::kServiceNotAvailable})) {
                    log_.LogDebug() << "PackageManagementApp::AttemptStateRecovery(), Cancel get Error:"
                                    << res.Error().Message().data()
                                    << ", so will try again later for kUcmId:" << kUcmId.c_str();
                    std::this_thread::sleep_for(std::chrono::milliseconds(kMs100));
                    continue;
                }
                log_.LogError() << "PackageManagementApp::AttemptStateRecovery(), failed to Cancel for ucmId:"
                                << kUcmId.c_str() << "with errmsg:" << res.Error().Message().data()
                                << ", so will abort.";
                result = false;

                // Exit Campaign
                std::ignore = TaskQueue::GetInstance()->PushTask([]() -> int32_t {
                    FsmManager::GetInstance()->Abort();
                    return 0;
                });
                break;
            }
            // Success
            log_.LogDebug() << "PackageManagementApp::AttemptStateRecovery(), succeed to Cancel for ucmId:"
                            << kUcmId.c_str();
            int32_t const ret{
                _waitStatus(pkgmgr::PackageManagerStatusType::kProcessing,
                            {pkgmgr::PackageManagerStatusType::kIdle, pkgmgr::PackageManagerStatusType::kReady})};
            log_.LogDebug() << "PackageManagementApp::AttemptStateRecovery(), WaitStatus ret:" << ret
                            << "for kUcmId:" << kUcmId.c_str();
            if (0 == ret) {
                continue;  // Restart
            }
            if (1 == ret) {
                // Exit Campaign
                std::ignore = TaskQueue::GetInstance()->PushTask([]() -> int32_t {
                    FsmManager::GetInstance()->Abort();
                    return 0;
                });
                result      = false;
                break;
            }
            result = false;
            break;
        }
        if (pkgmgr::PackageManagerStatusType::kReady == currentStatus_) {
            log_.LogDebug() << "PackageManagementApp::AttemptStateRecovery(), try RevertProcessedSwPackages for kUcmId:"
                            << kUcmId.c_str();
            ara::core::Future< void > future;      // Operation future
            future = ara::core::Future< void >{};  // meaningless, just for qac
            std::uint64_t operationStartTime{0U};  // Operation start time
            {
                std::unique_lock< std::mutex > lock{serviceMutex_};
                log_.LogDebug() << "PackageManagementApp::AttemptStateRecovery(), get serviceAvailable_:"
                                << serviceAvailable_ << "for kUcmId:" << kUcmId.c_str();

                // If service unavailable, wait for service to become available
                if (!serviceAvailable_.load()) {
                    if (!_waitServiceAvailable(lock)) {  // If service not available within timeout, end
                        log_.LogDebug()
                            << "PackageManagementApp::AttemptStateRecovery(), can't WaitServiceAvailable for kUcmId:"
                            << kUcmId.c_str();
                        result = false;
                        break;
                    }
                }
                // Record operation start time:
                operationStartTime = NowMS();
                log_.LogDebug() << "PackageManagementApp::AttemptStateRecovery(), get operationStartTime:"
                                << operationStartTime << "for kUcmId:" << kUcmId.c_str();

                //  Start executing operation:
                log_.LogDebug()
                    << "PackageManagementApp::AttemptStateRecovery(), do RevertProcessedSwPackages for kUcmId:"
                    << kUcmId.c_str();
                future = pkgmPtr_->RevertProcessedSwPackages();
            }

            bool alreadyReboot{false};
            int32_t counter{0};
            int32_t const counterLimit{25};
            while (
                (canWorkAsMaster = FsmManager::GetInstance()
                                       ->CanWorkAsMaster())) {  // Wait for result (target UCM may restart unexpectedly)
                log_.LogDebug() << "PackageManagementApp::AttemptStateRecovery(), try wait_for for "
                                   "RevertProcessedSwPackages for kUcmId:"
                                << kUcmId.c_str();
                counter++;
                ara::core::future_status const futureStatus{future.wait_for(std::chrono::milliseconds(
                    kGlobalWaitTime))};  // TODO(Han Zhibo): Use a condition variable to wait for cancellation or abort and future value (then function)
                if (ara::core::future_status::timeout == futureStatus) {  // Timeout
                    // When using future to wait for the operation result, if the waiting time (custom duration per operation) is too long, check whether service unavailability occurred between operationStartTime and now (compare with lastServiceUnavailableTime_),
                    //  If service unavailability occurred (regardless of how recent) AND the service is available again, it is considered that the target ucm has restarted: alreadyReboot? en
                    const bool serviceAvail                 = serviceAvailable_.load();
                    const bool isServiceUnavailableNotified = _isServiceUnavailableNotified(operationStartTime);
                    if ((counter >= counterLimit) && isServiceUnavailableNotified && serviceAvail) {
                        log_.LogDebug() << "PackageManagementApp::AttemptStateRecovery(), waiting too long and there "
                                           "is a service unvailable notified since and now serviceAvailable_:"
                                        << serviceAvailable_ << ",so set alreadyReboot for kUcmId:" << kUcmId.c_str();
                        alreadyReboot = true;
                        break;
                    }
                    continue;  // Continue waiting
                }
                // Result available
                log_.LogDebug() << "PackageManagementApp::AttemptStateRecovery(), have got result for "
                                   "RevertProcessedSwPackages for kUcmId:"
                                << kUcmId.c_str();
                break;
            }

            if (!canWorkAsMaster) {
                log_.LogDebug() << "PackageManagementApp::AttemptStateRecovery(), can't work as master for kUcmId:"
                                << kUcmId.c_str();
                break;
            }
            if (alreadyReboot) {  // Whether reconnected
                log_.LogDebug()
                    << "PackageManagementApp::AttemptStateRecovery(), try to WaitStatus because of alreadyReboot:"
                    << alreadyReboot << "for kUcmId:" << kUcmId.c_str();
                std::this_thread::sleep_for(std::chrono::milliseconds(kMs500));
                int32_t const ret{_waitStatus(
                    pkgmgr::PackageManagerStatusType::kReady,
                    {pkgmgr::PackageManagerStatusType::kReady, pkgmgr::PackageManagerStatusType::kCleaningUp,
                     pkgmgr::PackageManagerStatusType::kIdle})};
                log_.LogDebug() << "PackageManagementApp::AttemptStateRecovery(),  WaitStatus ret:" << ret
                                << "for kUcmId:" << kUcmId.c_str();
                if (0 == ret) {
                    continue;  // Restart
                }
                if (1 == ret) {
                    // Exit Campaign
                    std::ignore = TaskQueue::GetInstance()->PushTask([]() -> int32_t {
                        FsmManager::GetInstance()->Abort();
                        return 0;
                    });
                    result      = false;
                    break;
                }
                result = false;
                break;
            }
            // Result available
            ara::core::Result< void > const res{future.GetResult()};
            if (!res.HasValue()) {  // Error occurred
                // if (res.CheckError(ara::com::ComErrc::kGrantEnforcementError) ||.
                //     res.CheckError(ara::com::ComErrc::kNetworkBindingFailure) ||.
                //     res.CheckError(ara::com::ComErrc::kServiceNotAvailable)) {.
                if (helper::CheckResultIsError(
                        res, {ara::com::ComErrc::kGrantEnforcementError, ara::com::ComErrc::kNetworkBindingFailure,
                              ara::com::ComErrc::kServiceNotAvailable})) {
                    log_.LogDebug()
                        << "PackageManagementApp::AttemptStateRecovery(), RevertProcessedSwPackages get Error:"
                        << res.Error().Message().data() << ", so will try again later for kUcmId:" << kUcmId.c_str();
                    std::this_thread::sleep_for(std::chrono::milliseconds(kMs100));
                    continue;
                }
                log_.LogError() << "PackageManagementApp::AttemptStateRecovery(), failed to "
                                   "RevertProcessedSwPackages for ucmId:"
                                << kUcmId.c_str() << "with errmsg:" << res.Error().Message().data()
                                << ", so will abort.";
                result = false;

                // Exit Campaign
                std::ignore = TaskQueue::GetInstance()->PushTask([]() -> int32_t {
                    FsmManager::GetInstance()->Abort();
                    return 0;
                });
                break;
            }
            // Success
            log_.LogDebug() << "PackageManagementApp::AttemptStateRecovery(), succeed to "
                               "RevertProcessedSwPackages for ucmId:"
                            << kUcmId.c_str();
            int32_t const ret{
                _waitStatus(pkgmgr::PackageManagerStatusType::kReady,
                            {pkgmgr::PackageManagerStatusType::kCleaningUp, pkgmgr::PackageManagerStatusType::kIdle})};
            log_.LogDebug() << "PackageManagementApp::AttemptStateRecovery(), WaitStatus ret:" << ret
                            << "for kUcmId:" << kUcmId.c_str();
            if (0 == ret) {
                continue;  // Restart
            }
            if (1 == ret) {
                // Exit Campaign
                std::ignore = TaskQueue::GetInstance()->PushTask([]() -> int32_t {
                    FsmManager::GetInstance()->Abort();
                    return 0;
                });
                result      = false;
                break;
            }
            result = false;
            break;
        }
        if (pkgmgr::PackageManagerStatusType::kActivating == currentStatus_) {
            log_.LogDebug()
                << "PackageManagementApp::AttemptStateRecovery(), try to WaitStatus for kActivating for kUcmId:"
                << kUcmId.c_str();
            int32_t const ret{_waitStatus(
                pkgmgr::PackageManagerStatusType::kActivating,
                {pkgmgr::PackageManagerStatusType::kReady, pkgmgr::PackageManagerStatusType::kVerifying,
                 pkgmgr::PackageManagerStatusType::kActivated, pkgmgr::PackageManagerStatusType::kRollingBack,
                 pkgmgr::PackageManagerStatusType::kRolledBack})};
            log_.LogDebug() << "PackageManagementApp::AttemptStateRecovery(),  WaitStatus ret:" << ret
                            << "for kUcmId:" << kUcmId.c_str();
            if (0 == ret) {
                continue;  // Restart
            }
            if (1 == ret) {
                // Exit Campaign
                std::ignore = TaskQueue::GetInstance()->PushTask([]() -> int32_t {
                    FsmManager::GetInstance()->Abort();
                    return 0;
                });
                result      = false;
                break;
            }
            result = false;
            break;
        }
        if (pkgmgr::PackageManagerStatusType::kVerifying == currentStatus_) {
            log_.LogDebug()
                << "PackageManagementApp::AttemptStateRecovery(), try to WaitStatus for kVerifying for kUcmId:"
                << kUcmId.c_str();
            int32_t const ret{_waitStatus(
                pkgmgr::PackageManagerStatusType::kVerifying,
                {pkgmgr::PackageManagerStatusType::kActivated, pkgmgr::PackageManagerStatusType::kRollingBack,
                 pkgmgr::PackageManagerStatusType::kRolledBack})};
            log_.LogDebug() << "PackageManagementApp::AttemptStateRecovery(),  WaitStatus ret:" << ret
                            << "for kUcmId:" << kUcmId.c_str();
            if (0 == ret) {
                continue;  // Restart
            }
            if (1 == ret) {
                // Exit Campaign
                std::ignore = TaskQueue::GetInstance()->PushTask([]() -> int32_t {
                    FsmManager::GetInstance()->Abort();
                    return 0;
                });
                result      = false;
                break;
            }
            result = false;
            break;
        }
        if (pkgmgr::PackageManagerStatusType::kActivated == currentStatus_) {
            log_.LogDebug() << "PackageManagementApp::AttemptStateRecovery(), try Rollback for ucmId:"
                            << kUcmId.c_str();

            ara::core::Future< void > future;      // Operation future
            future = ara::core::Future< void >{};  // meaningless, just for qac
            std::uint64_t operationStartTime{0U};  // Operation start time
            {
                std::unique_lock< std::mutex > lock{serviceMutex_};
                log_.LogDebug() << "PackageManagementApp::AttemptStateRecovery(), get serviceAvailable_:"
                                << serviceAvailable_ << "for kUcmId:" << kUcmId.c_str();

                // If service unavailable, wait for service to become available
                if (!serviceAvailable_.load()) {
                    if (!_waitServiceAvailable(lock)) {  // If service not available within timeout, end
                        log_.LogDebug()
                            << "PackageManagementApp::AttemptStateRecovery(), can't WaitServiceAvailable for kUcmId:"
                            << kUcmId.c_str();
                        result = false;
                        break;
                    }
                }
                // Record operation start time:
                operationStartTime = NowMS();
                log_.LogDebug() << "PackageManagementApp::AttemptStateRecovery(), get operationStartTime:"
                                << operationStartTime << "for kUcmId:" << kUcmId.c_str();

                //  Start executing operation:
                log_.LogDebug() << "PackageManagementApp::AttemptStateRecovery(), do Rollback for kUcmId:"
                                << kUcmId.c_str();
                future = pkgmPtr_->Rollback();
            }

            bool alreadyReboot{false};
            int32_t counter{0};
            int32_t const counterLimit{50};
            while (
                (canWorkAsMaster = FsmManager::GetInstance()
                                       ->CanWorkAsMaster())) {  // Wait for result (target UCM may restart unexpectedly)
                log_.LogDebug() << "PackageManagementApp::AttemptStateRecovery(), try wait_for for Rollback for kUcmId:"
                                << kUcmId.c_str();
                counter++;
                ara::core::future_status const futureStatus{future.wait_for(std::chrono::milliseconds(
                    kGlobalWaitTime))};  // TODO(Han Zhibo): Use a condition variable to wait for cancellation or abort and future value (then function)
                if (ara::core::future_status::timeout == futureStatus) {  // Timeout
                    // When using future to wait for the operation result, if the waiting time (custom duration per operation) is too long, check whether service unavailability occurred between operationStartTime and now (compare with lastServiceUnavailableTime_),
                    //  If service unavailability occurred (regardless of how recent) AND the service is available again, it is considered that the target ucm has restarted: alreadyReboot? en
                    const bool serviceAvail                 = serviceAvailable_.load();
                    const bool isServiceUnavailableNotified = _isServiceUnavailableNotified(operationStartTime);
                    if ((counter >= counterLimit) && isServiceUnavailableNotified && serviceAvail) {
                        log_.LogDebug() << "PackageManagementApp::AttemptStateRecovery(), waiting too long and there "
                                           "is a service unvailable notified since and now serviceAvailable_:"
                                        << serviceAvailable_ << ",so set alreadyReboot for kUcmId:" << kUcmId.c_str();
                        alreadyReboot = true;
                        break;
                    }
                    continue;  // Continue waiting
                }
                // Result available
                log_.LogDebug() << "PackageManagementApp::AttemptStateRecovery(), have got result for "
                                   "RevertProcessedSwPackages for kUcmId:"
                                << kUcmId.c_str();
                break;
            }

            if (!canWorkAsMaster) {
                log_.LogDebug() << "PackageManagementApp::AttemptStateRecovery(), can't work as master for kUcmId:"
                                << kUcmId.c_str();
                break;
            }
            if (alreadyReboot) {  // Whether reconnected
                log_.LogDebug()
                    << "PackageManagementApp::AttemptStateRecovery(), try to WaitStatus because of alreadyReboot:"
                    << alreadyReboot << "for kUcmId:" << kUcmId.c_str();
                std::this_thread::sleep_for(std::chrono::milliseconds(kMs500));
                int32_t const ret{_waitStatus(
                    pkgmgr::PackageManagerStatusType::kActivated,
                    {pkgmgr::PackageManagerStatusType::kActivated, pkgmgr::PackageManagerStatusType::kRollingBack,
                     pkgmgr::PackageManagerStatusType::kRolledBack})};
                log_.LogDebug() << "PackageManagementApp::AttemptStateRecovery(),  WaitStatus ret:" << ret
                                << "for kUcmId:" << kUcmId.c_str();
                if (0 == ret) {
                    continue;  // Restart
                }
                if (1 == ret) {
                    // Exit Campaign
                    std::ignore = TaskQueue::GetInstance()->PushTask([]() -> int32_t {
                        FsmManager::GetInstance()->Abort();
                        return 0;
                    });
                    result      = false;
                    break;
                }
                result = false;
                break;
            }
            // Result available
            ara::core::Result< void > const res{future.GetResult()};
            if (!res.HasValue()) {  // Error occurred
                // if (res.CheckError(ara::com::ComErrc::kGrantEnforcementError) ||.
                //     res.CheckError(ara::com::ComErrc::kNetworkBindingFailure) ||.
                //     res.CheckError(ara::com::ComErrc::kServiceNotAvailable)) {.
                if (helper::CheckResultIsError(
                        res, {ara::com::ComErrc::kGrantEnforcementError, ara::com::ComErrc::kNetworkBindingFailure,
                              ara::com::ComErrc::kServiceNotAvailable})) {
                    log_.LogDebug() << "PackageManagementApp::AttemptStateRecovery(), Rollback get Error:"
                                    << res.Error().Message().data()
                                    << ", so will try again later for kUcmId:" << kUcmId.c_str();
                    std::this_thread::sleep_for(std::chrono::milliseconds(kMs100));
                    continue;
                }
                log_.LogError() << "PackageManagementApp::AttemptStateRecovery(), failed to Rollback for ucmId:"
                                << kUcmId.c_str() << "with errmsg:" << res.Error().Message().data()
                                << ", so will abort.";
                result = false;

                // Exit Campaign
                std::ignore = TaskQueue::GetInstance()->PushTask([]() -> int32_t {
                    FsmManager::GetInstance()->Abort();
                    return 0;
                });
                break;
            }
            log_.LogDebug() << "PackageManagementApp::AttemptStateRecovery(), succeed to Rollback for ucmId:"
                            << kUcmId.c_str();
            int32_t const ret{_waitStatus(
                pkgmgr::PackageManagerStatusType::kActivated,
                {pkgmgr::PackageManagerStatusType::kRollingBack, pkgmgr::PackageManagerStatusType::kRolledBack})};
            log_.LogDebug() << "PackageManagementApp::AttemptStateRecovery(), WaitStatus ret:" << ret
                            << "for kUcmId:" << kUcmId.c_str();
            if (0 == ret) {
                continue;  // Restart
            }
            if (1 == ret) {
                // Exit Campaign
                std::ignore = TaskQueue::GetInstance()->PushTask([]() -> int32_t {
                    FsmManager::GetInstance()->Abort();
                    return 0;
                });
                result      = false;
                break;
            }
            result = false;
            break;
        }
        if (pkgmgr::PackageManagerStatusType::kRollingBack == currentStatus_) {
            log_.LogDebug()
                << "PackageManagementApp::AttemptStateRecovery(), try to WaitStatus for kRollingBack for kUcmId:"
                << kUcmId.c_str();
            int32_t const ret{_waitStatus(pkgmgr::PackageManagerStatusType::kRollingBack,
                                          {pkgmgr::PackageManagerStatusType::kRolledBack})};
            log_.LogDebug() << "PackageManagementApp::AttemptStateRecovery(),  WaitStatus ret:" << ret
                            << "for kUcmId:" << kUcmId.c_str();
            if (0 == ret) {
                continue;  // Restart
            }
            if (1 == ret) {
                // Exit Campaign
                std::ignore = TaskQueue::GetInstance()->PushTask([]() -> int32_t {
                    FsmManager::GetInstance()->Abort();
                    return 0;
                });
                result      = false;
                break;
            }
            result = false;
            break;
        }
        if (pkgmgr::PackageManagerStatusType::kRolledBack == currentStatus_) {
            log_.LogDebug() << "PackageManagementApp::AttemptStateRecovery(), try Finish for ucmId:" << kUcmId.c_str();

            ara::core::Future< void > future{};    // Operation future
            future = ara::core::Future< void >{};  // meaningless, just for qac
            std::uint64_t operationStartTime{0U};  // Operation start time
            {
                std::unique_lock< std::mutex > lock{serviceMutex_};
                log_.LogDebug() << "PackageManagementApp::AttemptStateRecovery(), get serviceAvailable_:"
                                << serviceAvailable_ << "for kUcmId:" << kUcmId.c_str();

                // If service unavailable, wait for service to become available
                if (!serviceAvailable_.load()) {
                    if (!_waitServiceAvailable(lock)) {  // If service not available within timeout, end
                        log_.LogDebug()
                            << "PackageManagementApp::AttemptStateRecovery(), can't WaitServiceAvailable for kUcmId:"
                            << kUcmId.c_str();
                        result = false;
                        break;
                    }
                }
                // Record operation start time:
                operationStartTime = NowMS();
                log_.LogDebug() << "PackageManagementApp::AttemptStateRecovery(), get operationStartTime:"
                                << operationStartTime << "for kUcmId:" << kUcmId.c_str();

                //  Start executing operation:
                log_.LogDebug() << "PackageManagementApp::AttemptStateRecovery(), do Finish for kUcmId:"
                                << kUcmId.c_str();
                future = pkgmPtr_->Finish();
            }

            bool alreadyReboot{false};
            int32_t counter{0};
            int32_t const counterLimit{25};
            while (
                (canWorkAsMaster = FsmManager::GetInstance()
                                       ->CanWorkAsMaster())) {  // Wait for result (target UCM may restart unexpectedly)
                log_.LogDebug() << "PackageManagementApp::AttemptStateRecovery(), try wait_for for Finish for kUcmId:"
                                << kUcmId.c_str();
                counter++;
                ara::core::future_status const futureStatus{future.wait_for(std::chrono::milliseconds(
                    kGlobalWaitTime))};  // TODO(Han Zhibo): Use a condition variable to wait for cancellation or abort and future value (then function)
                if (ara::core::future_status::timeout == futureStatus) {  // Timeout
                    // When using future to wait for the operation result, if the waiting time (custom duration per operation) is too long, check whether service unavailability occurred between operationStartTime and now (compare with lastServiceUnavailableTime_),
                    //  If service unavailability occurred (regardless of how recent) AND the service is available again, it is considered that the target ucm has restarted: alreadyReboot? en
                    const bool serviceAvail                 = serviceAvailable_.load();
                    const bool isServiceUnavailableNotified = _isServiceUnavailableNotified(operationStartTime);
                    if ((counter >= counterLimit) && isServiceUnavailableNotified && serviceAvail) {
                        log_.LogDebug() << "PackageManagementApp::AttemptStateRecovery(), waiting too long and there "
                                           "is a service unvailable notified since and now serviceAvailable_:"
                                        << serviceAvailable_ << ",so set alreadyReboot for kUcmId:" << kUcmId.c_str();
                        alreadyReboot = true;
                        break;
                    }
                    continue;  // Continue waiting
                }
                // Result available
                log_.LogDebug()
                    << "PackageManagementApp::AttemptStateRecovery(), have got result for Finish for kUcmId:"
                    << kUcmId.c_str();
                break;
            }

            if (!canWorkAsMaster) {
                log_.LogDebug() << "PackageManagementApp::AttemptStateRecovery(), can't work as master for kUcmId:"
                                << kUcmId.c_str();
                break;
            }
            if (alreadyReboot) {  // Whether reconnected
                log_.LogDebug()
                    << "PackageManagementApp::AttemptStateRecovery(), try to WaitStatus because of alreadyReboot:"
                    << alreadyReboot << "for kUcmId:" << kUcmId.c_str();
                std::this_thread::sleep_for(std::chrono::milliseconds(kMs500));
                int32_t const ret{_waitStatus(
                    pkgmgr::PackageManagerStatusType::kRolledBack,
                    {pkgmgr::PackageManagerStatusType::kRolledBack, pkgmgr::PackageManagerStatusType::kCleaningUp,
                     pkgmgr::PackageManagerStatusType::kIdle})};
                log_.LogDebug() << "PackageManagementApp::AttemptStateRecovery(),  WaitStatus ret:" << ret
                                << "for kUcmId:" << kUcmId.c_str();
                if (0 == ret) {
                    continue;  // Restart
                }
                if (1 == ret) {
                    // Exit Campaign
                    std::ignore = TaskQueue::GetInstance()->PushTask([]() -> int32_t {
                        FsmManager::GetInstance()->Abort();
                        return 0;
                    });
                    result      = false;
                    break;
                }
                result = false;
                break;
            }
            // Result available
            ara::core::Result< void > const res{future.GetResult()};
            if (!res.HasValue()) {  // Error occurred
                // if (res.CheckError(ara::com::ComErrc::kGrantEnforcementError) ||.
                //     res.CheckError(ara::com::ComErrc::kNetworkBindingFailure) ||.
                //     res.CheckError(ara::com::ComErrc::kServiceNotAvailable)) {.
                if (helper::CheckResultIsError(
                        res, {ara::com::ComErrc::kGrantEnforcementError, ara::com::ComErrc::kNetworkBindingFailure,
                              ara::com::ComErrc::kServiceNotAvailable})) {
                    log_.LogDebug() << "PackageManagementApp::AttemptStateRecovery(), Finish get Error:"
                                    << res.Error().Message().data()
                                    << ", so will try again later for kUcmId:" << kUcmId.c_str();
                    std::this_thread::sleep_for(std::chrono::milliseconds(kMs100));
                    continue;
                }

                log_.LogError() << "PackageManagementApp::AttemptStateRecovery(), failed to Finish for ucmId:"
                                << kUcmId.c_str() << " with errmsg:" << res.Error().Message().data()
                                << ", so will abort.";
                result = false;

                // Exit Campaign
                std::ignore = TaskQueue::GetInstance()->PushTask([]() -> int32_t {
                    FsmManager::GetInstance()->Abort();
                    return 0;
                });
                break;
            }
            log_.LogDebug() << "PackageManagementApp::AttemptStateRecovery(), succeed to Finish for ucmId:"
                            << kUcmId.c_str();
            int32_t const ret{
                _waitStatus(pkgmgr::PackageManagerStatusType::kRolledBack,
                            {pkgmgr::PackageManagerStatusType::kCleaningUp, pkgmgr::PackageManagerStatusType::kIdle})};
            log_.LogDebug() << "PackageManagementApp::AttemptStateRecovery(), WaitStatus ret:" << ret
                            << "for kUcmId:" << kUcmId.c_str();
            if (0 == ret) {
                continue;  // Restart
            }
            if (1 == ret) {
                // Exit Campaign
                std::ignore = TaskQueue::GetInstance()->PushTask([]() -> int32_t {
                    FsmManager::GetInstance()->Abort();
                    return 0;
                });
                result      = false;
                break;
            }
            result = false;
            break;
        }

        log_.LogError() << "PackageManagementApp::AttemptStateRecovery(), can't do nothing for currentStatus_:"
                        << CurrentStatusToString(currentStatus_).data() << " for ucmId:" << kUcmId.c_str();
        // Exit Campaign
        std::ignore = TaskQueue::GetInstance()->PushTask([]() -> int32_t {
            FsmManager::GetInstance()->Abort();
            return 0;
        });
        result      = false;
        break;
    }

    log_.LogDebug() << "PackageManagementApp::AttemptStateRecovery(), end with result:" << result
                    << " canWorkAsMaster:" << canWorkAsMaster << " for kUcmId:" << kUcmId.c_str();
    return result && canWorkAsMaster;
}

/// @brief Check if there are pending changes in the software cluster set
/// @return True if the there are changes, else false.
/// @throws no
bool PackageManagementApp::_areChangesPending()
{
    ara::core::Future< GetSwClusterChangeInfoOutput > future{pkgmPtr_->GetSwClusterChangeInfo()};
    GetSwClusterChangeInfoOutput const changeInfoOutput{future.get()};
    return !(changeInfoOutput.SwInfo.empty());
}

/// @brief Get the transfer id of the software package being transferred/executed
/// @return TransferIdType transfer id
/// @throws no
ara::core::Result< pkgmgr::TransferIdType > PackageManagementApp::_getProcessingTransferId() const
{
    ara::core::Future< GetSwPackagesOutput > futureGetPackagesOutput{pkgmPtr_->GetSwPackages()};
    ara::core::Result< GetSwPackagesOutput > result{futureGetPackagesOutput.GetResult()};
    if (!result.HasValue()) {
        LOG_WARN << "call faild! ucmId=" << kUcmId.c_str() << "errmsg=" << result.Error().Message().data();
        return ara::core::Result< pkgmgr::TransferIdType >::FromError(result.Error());
    }

    GetSwPackagesOutput vGetPackagesOutput{result.Value()};
    pkgmgr::TransferIdType activeTransferId{{0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U}};
    bool isHave{false};
    for (pkgmgr::SwPackageInfoVectorType::iterator iter{vGetPackagesOutput.Packages.begin()};
         iter != vGetPackagesOutput.Packages.end(); iter++) {
        pkgmgr::SwPackageInfoType const& package{(*iter)};
        if (package.State == pkgmgr::SwPackageStateType::kProcessing) {
            activeTransferId = package.TransferID;
            isHave           = true;
            break;
        }
    }
    if (isHave) {
        return ara::core::Result< pkgmgr::TransferIdType >::FromValue(activeTransferId);
    }
    /// TODO(yunfei) pkgmgr::UCMErrorDomainErrc::kGeneralReject is used inappropriately here
    return ara::core::Result< pkgmgr::TransferIdType >::FromError(pkgmgr::UCMErrorDomainErrc::kGeneralReject);
}

/// @brief Gets the instance of the PackageManagement service that is used by this app
/// @return std::shared_ptr<PackageManagementService>.
/// @throws no
std::shared_ptr< PackageManagementProxy > PackageManagementApp::_getService() const noexcept { return pkgmPtr_; }

}  // namespace vpkgmgr
}  // namespace ucm
}  // namespace ara
