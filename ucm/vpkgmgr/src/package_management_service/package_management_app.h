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
/// @file       package_management_app.h
/// @brief      vucm acts as a client of ucm. This class mainly encapsulates software package transfer, upgrade/update, activation, rollback, and retrieval of software package/cluster information for ucm.
/// @details
/// @date       2024-07-20
/// @author     hanzhibo
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// export_level=/UCM Master/Package Management Service
/// @module_path=/UCM Master/Package Management Service
/// @interface_level=module
/// @trace_id_sr=SR_UCM_00035,SR_UCM_00033,SR_UCM_00034,SR_UCM_00036
/// @unit_name=PackageManagementApp
/// @unit_description=Encapsulates software package transfer, upgrade/update, activation, rollback, and retrieval of software package/cluster information for ucm
/// @endcode
///
/// ================================================================

#ifndef ARA_UCM_PKGMGR_SAMPLE_PACKAGE_MANAGEMENT_APP_H_
#define ARA_UCM_PKGMGR_SAMPLE_PACKAGE_MANAGEMENT_APP_H_

#include <ara/core/result.h>
#include <ara/core/string.h>
#include <ara/log/log_stream.h>
#include <ara/log/logger.h>

#include <atomic>
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <vector>

#include "ara/ucm/pkgmgr/packagemanagement_proxy.h"
#include "package_management_service/data_transfer.h"

namespace ara {
namespace ucm {
namespace vpkgmgr {

/// @brief Activation method type
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00347
/// @trace_id_dd=DD_UCM_Master_00714
/// @needwork = ad
/// @endcode
enum class ActivateOptionType : std::uint8_t
{
    kWaitForReboot      = 0x00U,
    kRestartApplication = 0x01U,
    kReboot             = 0x02U
};

/// @brief ucm master acts as a client of ucm. This class mainly encapsulates software package transfer, upgrade/update, activation, rollback, and retrieval of software package/cluster information for ucm.
/// If the connection to the UCM subordinate is lost, the com component manages retries (findservice needs to run continuously).
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00348
/// @trace_id_dd=DD_UCM_Master_00715
/// @needwork = ad
/// @endcode
class PackageManagementApp final
{
public:
    /// @brief constructor
    /// @param id
    /// @param proxy
    /// @param apiWaitTime
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00349
    /// @trace_id_dd=DD_UCM_Master_00716
    /// @needwork = ad
    /// @endcode
    PackageManagementApp(ara::core::String id,
                         std::shared_ptr< pkgmgr::proxy::PackageManagementProxy > proxy,
                         uint32_t const apiWaitTime);
    /// @brief destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00350
    /// @trace_id_dd=DD_UCM_Master_00717
    /// @needwork = ad
    /// @endcode
    ~PackageManagementApp() = default;

    /// @brief Copy constructor
    /// @param other Source object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00351
    /// @trace_id_dd=DD_UCM_Master_00718
    /// @needwork = ad
    /// @endcode
    PackageManagementApp(PackageManagementApp const& other) = delete;
    /// @brief Move constructor
    /// @param other Source object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00352
    /// @trace_id_dd=DD_UCM_Master_00719
    /// @needwork = ad
    /// @endcode
    PackageManagementApp(PackageManagementApp&& other) = delete;
    /// @brief Copy assignment operator
    /// @param other Source object
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00353
    /// @trace_id_dd=DD_UCM_Master_00720
    /// @needwork = ad
    /// @endcode
    PackageManagementApp& operator=(PackageManagementApp const& other) = delete;
    /// @brief Move assignment operator
    /// @param other Source object
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00354
    /// @trace_id_dd=DD_UCM_Master_00721
    /// @needwork = ad
    /// @endcode
    PackageManagementApp& operator=(PackageManagementApp&& other) = delete;

    /// @brief Convert the current status to a human-readable string
    /// @param status
    /// @return str
    /// @throws no
    ///
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00355
    /// @trace_id_dd=DD_UCM_Master_00722
    /// @needwork = ad
    /// @endcode
    static ara::core::StringView const CurrentStatusToString(pkgmgr::PackageManagerStatusType const status);

    /// @brief Initialize resources
    /// @return Returns true on success, false on failure
    /// @throws no
    ///
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00356
    /// @trace_id_dd=DD_UCM_Master_00723
    /// @needwork = ad
    /// @endcode
    static bool Init() noexcept;

    // /// @brief Set whether this ucm needs to restart the platform during activation
    // /// @param isReboot true: need to restart platform, false: no need to restart platform
    // /// @throws no
    // ///
    ///inline void SetReboot(bool const isReboot) noexcept { updateReboot_ = isReboot; }

    // /// @brief Get whether this ucm needs to restart the platform during activation
    // /// @return true: need to restart platform, false: no need to restart platform
    // /// @throws no
    // ///
    ///inline bool IsNeedReboot() const noexcept { return updateReboot_; } // This function is not effectively used

    // /// @brief Check if the current ucm is online
    // /// @return true: online, false: offline
    // /// @throws no
    // ///
    ///inline bool isConnect() const noexcept { return isConnected_; }

    // /// @brief Connect to ucm and update status
    // /// @return
    // /// @throws no
    // ///
    ///bool Connect();

    // /// @brief Notification of reconnection to ucm
    // /// @return
    // /// @throws no
    // ///
    ///void OnReConnect();

    /// @brief Notification of service availability
    /// @return
    /// @throws no
    ///
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00357
    /// @trace_id_dd=DD_UCM_Master_00724
    /// @needwork = ad
    /// @endcode
    void OnServiceAvailable();

    /// @brief Notification of service unavailability
    /// @return
    /// @throws no
    ///
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00358
    /// @trace_id_dd=DD_UCM_Master_00725
    /// @needwork = ad
    /// @endcode
    void OnServiceUnavailable();

    /// @brief Check if service is available
    /// @return bool
    /// @throws no
    ///
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00359
    /// @trace_id_dd=DD_UCM_Master_00726
    /// @needwork = ad
    /// @endcode
    bool IsServiceAvailable();

    /// @brief Get the ucm identifier
    /// @return Returns ucm identifier
    /// @throws no
    ///
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00360
    /// @trace_id_dd=DD_UCM_Master_00727
    /// @trace_id_sr=SR_UCM_00035,SR_UCM_00036
    /// @needwork = ad
    /// @endcode
    pkgmgr::UCMIdentifierType const& GetId() const noexcept;

    /// @brief Get historical operation records within a specified time period
    /// @param timestampGE Start time
    /// @param timestampLT End time
    /// @return Returns historical operation records within the specified time period
    /// @throws no
    ///
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00361
    /// @trace_id_dd=DD_UCM_Master_00728
    /// @trace_id_sr=SR_UCM_00035,SR_UCM_00034,SR_UCM_00036
    /// @needwork = ad
    /// @endcode
    pkgmgr::GetHistoryVectorType GetHistory(std::uint64_t const& timestampGE, std::uint64_t const& timestampLT);

    /// @brief Get ucm software cluster information
    /// @return Returns ucm software cluster information
    /// @throws no
    ///
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00362
    /// @trace_id_dd=DD_UCM_Master_00729
    /// @trace_id_sr=SR_UCM_00035,SR_UCM_00033,SR_UCM_00036
    /// @needwork = ad
    /// @endcode
    pkgmgr::SwClusterInfoVectorType GetSwClusterInfo() noexcept;  /// @throws no

    /// @brief Get the software cluster information for ucm update/upgrade
    /// @return Returns ucm update/upgrade software cluster information
    /// @throws no
    ///
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00363
    /// @trace_id_dd=DD_UCM_Master_00730
    /// @trace_id_sr=SR_UCM_00035,SR_UCM_00036
    /// @needwork = ad
    /// @endcode
    pkgmgr::SwClusterInfoVectorType GetSwClusterChangeInfo();

    /// @brief GetSwClusterDescription
    /// @return SwDescVectorType
    /// @throws no
    ///
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00364
    /// @trace_id_dd=DD_UCM_Master_00731
    /// @trace_id_sr=SR_UCM_00035,SR_UCM_00036
    /// @needwork = ad
    /// @endcode
    pkgmgr::SwDescVectorType GetSwClusterDescription();

    /// @brief GetSwPackages
    /// @return SwPackageInfoVectorType
    /// @throws no
    ///
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00365
    /// @trace_id_dd=DD_UCM_Master_00732
    /// @trace_id_sr=SR_UCM_00035,SR_UCM_00036
    /// @needwork = ad
    /// @endcode
    pkgmgr::SwPackageInfoVectorType GetSwPackages();

    /// @brief Transfer the software package at the given path
    /// @param packagePath Full file path of the software package archive
    /// @return result
    /// @throws no
    ///
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00366
    /// @trace_id_dd=DD_UCM_Master_00733
    /// @trace_id_sr=SR_UCM_00035,SR_UCM_00036
    /// @needwork = ad
    /// @endcode
    ara::core::Result< pkgmgr::TransferIdType > TransferSoftwarePackage(ara::core::String const& packagePath) const;

    /// @brief GetSwProcessProgress
    /// @param transferId
    /// @return Progress
    /// @throws no
    ///
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00367
    /// @trace_id_dd=DD_UCM_Master_00734
    /// @trace_id_sr=SR_UCM_00035,SR_UCM_00036
    /// @needwork = ad
    /// @endcode
    uint8_t GetSwProcessProgress(pkgmgr::TransferIdType const& transferId);  // No implementation yet

    /// @brief Transfer the software package at the given path using the given DataTransfer object
    /// @param packagePath Full file path of the software package archive
    /// @param dataTransfer Instance of the DataTransfer object to be used
    /// @return True if the package was successfully transferred, otherwise false
    /// @throws no
    ///
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00368
    /// @trace_id_dd=DD_UCM_Master_00735
    /// @trace_id_sr=SR_UCM_00035,SR_UCM_00036
    /// @needwork = ad
    /// @endcode
    ara::core::Result< pkgmgr::TransferIdType > TransferSoftwarePackage(
        ara::core::String const& packagePath,
        DataTransfer< pkgmgr::proxy::PackageManagementProxy >& dataTransfer) const;

    /// @brief Delete the specified software package
    /// @param transferId
    /// @return True if the package was successfully deleted, otherwise false
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00369
    /// @trace_id_dd=DD_UCM_Master_00736
    /// @trace_id_sr=SR_UCM_00035,SR_UCM_00036
    /// @needwork = ad
    /// @endcode
    bool DeleteSoftwarePackage(pkgmgr::TransferIdType const& transferId) const;

    /// @brief Delete the transferred software package
    /// @return True if the package was successfully deleted, otherwise false
    /// @throws no
    ///
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00370
    /// @trace_id_dd=DD_UCM_Master_00737
    /// @trace_id_sr=SR_UCM_00035,SR_UCM_00036
    /// @needwork = ad
    /// @endcode
    bool DeleteSoftwarePackages() const;  // Not used

    /// @brief Install the latest transferred software package. If no software package has been transferred, this method has no effect.
    /// @param transferId
    /// @return True if the installation is successful, false otherwise
    /// @throws no
    ///
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00371
    /// @trace_id_dd=DD_UCM_Master_00738
    /// @trace_id_sr=SR_UCM_00035,SR_UCM_00036
    /// @needwork = ad
    /// @endcode
    bool Install(pkgmgr::TransferIdType const transferId);

    /// @brief Activate
    /// @return Returns true if activation is successful, false otherwise
    /// @throws no
    ///
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00372
    /// @trace_id_dd=DD_UCM_Master_00739
    /// @trace_id_sr=SR_UCM_00035,SR_UCM_00036
    /// @needwork = ad
    /// @endcode
    bool Activate();

    /// @brief Complete the final installation process. Called after Activate, Rollback, or Cancel
    /// @return Returns true on success, false on failure.
    /// @throws no
    ///
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00373
    /// @trace_id_dd=DD_UCM_Master_00740
    /// @trace_id_sr=SR_UCM_00035,SR_UCM_00036
    /// @needwork = ad
    /// @endcode
    bool Finish();

    /// @brief Rollback the latest installed and "activated" software package
    /// @return True if rollback is successful, false otherwise
    /// @throws no
    ///
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00374
    /// @trace_id_dd=DD_UCM_Master_00741
    /// @trace_id_sr=SR_UCM_00035,SR_UCM_00036
    /// @needwork = ad
    /// @endcode
    bool Rollback();

    /// @brief Cancel the current processing
    /// @param processingTransferId Id of the package being processed
    /// @return Returns true on success, false on failure.
    /// @throws no
    ///
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00375
    /// @trace_id_dd=DD_UCM_Master_00742
    /// @trace_id_sr=SR_UCM_00035,SR_UCM_00036
    /// @needwork = ad
    /// @endcode
    bool Cancel(pkgmgr::TransferIdType const processingTransferId);

    /// @brief Subscribe to status field notifications.
    /// @return Returns true on success, false on failure.
    /// @throws no
    ///
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00376
    /// @trace_id_dd=DD_UCM_Master_00743
    /// @trace_id_sr=SR_UCM_00035,SR_UCM_00036
    /// @needwork = ad
    /// @endcode
    bool StatusSubscribe();

    /// @brief Update the current UCM status
    /// @return true on successful update, false on failure, possibly due to connection loss
    /// @throws no
    ///
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00377
    /// @trace_id_dd=DD_UCM_Master_00744
    /// @needwork = ad
    /// @endcode
    bool UpdateStatus();

    /// @brief Get the current UCM status
    /// @return Returns the current UCM status
    /// @throws no
    ///
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00378
    /// @trace_id_dd=DD_UCM_Master_00745
    /// @trace_id_sr=SR_UCM_00035,SR_UCM_00036
    /// @needwork = ad
    /// @endcode
    pkgmgr::PackageManagerStatusType GetCurrentStatus() noexcept;

    /// @brief Attempt rollback to restore to kIdle state
    /// @return Returns True: rollback successful, false: rollback failed
    /// @throws no
    ///
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00379
    /// @trace_id_dd=DD_UCM_Master_00746
    /// @trace_id_sr=SR_UCM_00035,SR_UCM_00036
    /// @needwork = ad
    /// @endcode
    bool AttemptStateRecovery();

private:
    /// @brief Callback for processing status change notifications.
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00747
    /// @trace_id_sr=SR_UCM_00035,SR_UCM_00036
    /// @needwork = dda
    /// @endcode
    void _onStatusChange();

    /// @brief Check if the status is the expected value
    /// @param askedStatus
    /// @return True if the current status equals the expected status, otherwise false
    /// @throws no

    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00748
    /// @needwork = dda
    /// @endcode
    bool _isCorrectStatus(pkgmgr::PackageManagerStatusType const askedStatus) const noexcept;

    /// @brief Check if there are pending changes in the software cluster set
    /// @return True if the there are changes, else false.
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00749
    /// @needwork = dda
    /// @endcode
    bool _areChangesPending();

    /// @brief Get the transfer id of the software package being transferred/executed
    /// @return TransferIdType transfer id
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00750
    /// @needwork = dda
    /// @endcode
    ara::core::Result< pkgmgr::TransferIdType > _getProcessingTransferId() const;

    /// @brief Gets the instance of the PackageManagement service that is used by this app
    /// @return std::shared_ptr<PackageManagementService>.
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00751
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< pkgmgr::proxy::PackageManagementProxy > _getService() const noexcept;

    /// @brief Revert processed software packages
    /// @returns True if RevertProcessedSwPackages is successful, false otherwise
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00752
    /// @trace_id_sr=SR_UCM_00035,SR_UCM_00036
    /// @needwork = dda
    /// @endcode
    bool _revertProcessedSwPackages();

    /// @brief _waitStatus
    /// @param initialStatus
    /// @param expectedStatus
    /// @return 0: expected status reached, which may include the initial status; 1: status other than initial and expected status reached; 2: ended waiting because cannot move forward
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00753
    /// @needwork = dda
    /// @endcode
    int32_t _waitStatus(pkgmgr::PackageManagerStatusType const initialStatus,
                        std::set< pkgmgr::PackageManagerStatusType > expectedStatus) const;

    /// @brief Wait for service to become available; returns true if available, false if timeout
    /// @param lock
    /// @return bool
    /// @throws no
    ///
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00754
    /// @needwork = dda
    /// @endcode
    bool _waitServiceAvailable(std::unique_lock< std::mutex >& lock);

    /// @brief Check whether service unavailability was notified between operationStartTime and the current time
    /// @param operationStartTime
    /// @return bool
    /// @throws no
    ///
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00755
    /// @needwork = dda
    /// @endcode
    bool _isServiceUnavailableNotified(std::uint64_t const operationStartTime) const noexcept;

private:
    /// @brief Instance id of the ucm
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00756
    /// @needwork = dda
    /// @endcode
    pkgmgr::UCMIdentifierType const kUcmId{};

    /// @brief The PackageManagement service proxy instance used by this application
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00757
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< pkgmgr::proxy::PackageManagementProxy > pkgmPtr_{};

    /// @brief Waiting time for general interface calls
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00758
    /// @needwork = dda
    /// @endcode
    uint32_t const kApiWaitTime{0U};

    /// @brief Current status of the ucm
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00759
    /// @needwork = dda
    /// @endcode
    std::atomic< pkgmgr::PackageManagerStatusType > currentStatus_{};

    /// @brief Logger for logging errors
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00760
    /// @needwork = dda
    /// @endcode
    ara::log::Logger& log_{ara::log::CreateLogger(std::move(ara::core::StringView("#UCM")),
                                                  std::move(ara::core::StringView("PackageManagementApp context")),
                                                  ara::log::LogLevel::kVerbose)};

    /// @brief Service mutex, e.g., changes in service availability and use of service-provided operations, involving e.g., serviceAvailable_ etc.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00761
    /// @needwork = dda
    /// @endcode
    std::mutex serviceMutex_{};
    /// @brief Whether the service is available
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00762
    /// @needwork = dda
    /// @endcode
    std::atomic< bool > serviceAvailable_{true};
    /// @brief Condition variable for service availability
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00763
    /// @needwork = dda
    /// @endcode
    std::condition_variable serviceAvailableCondition_{};
    /// @brief Time of last service unavailability
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00764
    /// @needwork = dda
    /// @endcode
    std::uint64_t lastServiceUnavailableTime_{0U};
};

/// @brief Define alias PackageManagementAppPtr
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
using PackageManagementAppPtr = std::shared_ptr< PackageManagementApp >;

}  // namespace vpkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // ARA_UCM_PKGMGR_SAMPLE_PACKAGE_MANAGEMENT_APP_H_
