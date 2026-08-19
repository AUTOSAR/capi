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
/// @file       package_manager_impl.h
/// @brief      PackageManagement service impl
/// @details
/// @date       2024-08-15
/// @author     cuiyinli
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// export_level=/UCM/PackageManager
/// @module_path=/UCM/PackageManager
/// @interface_level=unit
/// @trace_id_sr=SR_UCM_00001,SR_UCM_00002,SR_UCM_00011,SR_UCM_00014,SR_UCM_00015,SR_UCM_00019,SR_UCM_00028,SR_UCM_00029,SR_UCM_00003,SR_UCM_00020,SR_UCM_00026,SR_UCM_00004,SR_UCM_00021,SR_UCM_00031,SR_UCM_00023,SR_UCM_00032,SR_UCM_00007,SR_UCM_00030
/// @unit_name=PackageManagerImpl
/// @unit_description=PackageManagement service impl
/// @endcode
///
/// ================================================================

#ifndef ARA_UCM_PKGMGR_PACKAGE_MANAGER_PACKAGE_MANAGER_IMPL_H_
#define ARA_UCM_PKGMGR_PACKAGE_MANAGER_PACKAGE_MANAGER_IMPL_H_

#include <memory>

#include "common/alias.h"
#include "data/software_package_manager.h"
#include "fsm/fsm_manager.h"
#include "sm/sm_adapter.h"
#include "storage/filesystem_swcl_manager.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief This class is the entry point for package management tasks
/// such as installing, uninstalling and update of software clusters
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10227
/// @trace_id_dd=DD_UCM_10314
/// @needwork = ad
/// @endcode
class PackageManagerImpl
{
public:
    /// @brief Initializes the PackageManagerImpl
    /// @param ucmId
    /// @param swpkgManager Manager to deal with software packages
    /// @param swclManager
    /// @param fsmManager
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10228
    /// @trace_id_dd=DD_UCM_10315
    /// @needwork = ad
    /// @endcode
    PackageManagerImpl(AraString const& ucmId,
                       std::unique_ptr< SoftwarePackageManager > swpkgManager,
                       std::unique_ptr< FileSystemSWCLManager > swclManager,
                       std::unique_ptr< FsmManager > fsmManager) noexcept;

    /// @brief destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10229
    /// @trace_id_dd=DD_UCM_10316
    /// @needwork = ad
    /// @endcode
    ~PackageManagerImpl() noexcept = default;

    /// @brief delete copy construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10230
    /// @trace_id_dd=DD_UCM_10317
    /// @needwork = ad
    /// @endcode
    PackageManagerImpl(PackageManagerImpl const& other) = delete;
    /// @brief delete copy asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10231
    /// @trace_id_dd=DD_UCM_10318
    /// @needwork = ad
    /// @endcode
    PackageManagerImpl& operator=(PackageManagerImpl const& other) = delete;
    /// @brief delete move construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10232
    /// @trace_id_dd=DD_UCM_10319
    /// @needwork = ad
    /// @endcode
    PackageManagerImpl(PackageManagerImpl&& other) = delete;
    /// @brief delete move asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10233
    /// @trace_id_dd=DD_UCM_10320
    /// @needwork = ad
    /// @endcode
    PackageManagerImpl& operator=(PackageManagerImpl&& other) = delete;

    // Transfer and Software Package
    /// @brief Block-wise transfer of a Software Package.
    ///
    /// @param id Transfer ID of the currently running request.
    /// @param data Data block of the Software Package.
    /// @param blockCounter Block counter of the current block.
    ///
    /// @return The result of transferring current data block, which specifies if the
    /// sw package has been successfully transferred.
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10234
    /// @trace_id_dd=DD_UCM_10321
    /// @trace_id_sr=SR_UCM_00014,SR_UCM_00019
    /// @needwork = ad
    /// @endcode
    FutureVoid TransferData(TransferIdType const& id,
                            ByteVectorType const& data,
                            std::uint64_t const& blockCounter) const noexcept;
    /// @brief Finish the transfer of a Software Package.
    ///
    /// @param id Transfer ID of the currently running request.
    ///
    /// @return The result of finishing this sw package transfer, which specifies if the
    /// sw package has been successfully transferred.
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10235
    /// @trace_id_dd=DD_UCM_10322
    /// @trace_id_sr=SR_UCM_00014,SR_UCM_00019,SR_UCM_00031
    /// @needwork = ad
    /// @endcode
    FutureVoid TransferExit(TransferIdType const& id) const noexcept;
    /// @brief Start the transfer of a Software Package.
    /// Transfer Id for subsequent calls to TransferData will be generated
    /// and returned as a part of TransferStartOutput.
    ///
    /// @param size Size (in bytes) of the Software Package to be transferred.
    ///
    /// @return The struct which contains the result of the transfer start operation
    /// and transfer id generated for this operation.
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10236
    /// @trace_id_dd=DD_UCM_10323
    /// @trace_id_sr=SR_UCM_00014,SR_UCM_00019
    /// @needwork = ad
    /// @endcode
    FutureTransferStart TransferStart(std::uint64_t const size) const noexcept;

    /// @brief Delete a transferred Software Package.
    ///
    /// @param id Transfer ID of Software Package.
    ///
    /// @return The result of operation, which specifies if the
    /// request was successful.
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10237
    /// @trace_id_dd=DD_UCM_10324
    /// @needwork = ad
    /// @endcode
    FutureVoid DeleteTransfer(TransferIdType const& id) const noexcept;

    /// @brief Retrieve a Software Package list.
    ///
    /// @return List of all Software Packages that have been successfully
    /// transferred and are ready to be installed.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10238
    /// @trace_id_dd=DD_UCM_10325
    /// @needwork = ad
    /// @endcode
    FutureGetSwPackages GetSwPackages() const noexcept;
    /// @brief Get the progress of the currently processed Software Package.
    ///
    /// @param id Transfer ID of Software Package.
    ///
    /// @return The progress of the current package processing (0x00 - 0x64).
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10239
    /// @trace_id_dd=DD_UCM_10326
    /// @trace_id_sr=SR_UCM_00023
    /// @needwork = ad
    /// @endcode
    FutureGetSwProcessProgress GetSwProcessProgress(TransferIdType const& id) const noexcept;

    // Operations on Software Clusters (FileSystemSWCLManager)
    /// @brief Retrieve a list of SoftwareClusters that have pending changes.
    /// The returned list includes all SoftwareClusters that are to be added,
    /// updated or removed. The list of changes is extended
    /// in the course of processing software Packages.
    ///
    /// @return List of SoftwareClusters that are in state kAdded,
    /// kUpdated or kRemoved.
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10240
    /// @trace_id_dd=DD_UCM_10327
    /// @trace_id_sr=SR_UCM_00011
    /// @needwork = ad
    /// @endcode
    FutureGetSwClusterChangeInfo GetSwClusterChangeInfo() const noexcept;
    /// @brief Retrieve a Software Clusters information list.
    ///
    /// @return List of the general information of the Software Clusters present
    /// in the platform.
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10241
    /// @trace_id_dd=DD_UCM_10328
    /// @trace_id_sr=SR_UCM_00002
    /// @needwork = ad
    /// @endcode
    FutureGetSwClusterDescription GetSwClusterDescription() const noexcept;
    /// @brief Retrieve a list of SoftwareClusters.
    ///
    /// @return List of installed SoftwareClusters that are in state kPresent.
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10242
    /// @trace_id_dd=DD_UCM_10329
    /// @trace_id_sr=SR_UCM_00002
    /// @needwork = ad
    /// @endcode
    FutureGetSwClusterInfo GetSwClusterInfo() const noexcept;

    /// @brief Retrieve all actions performed by UCM in provided time range.
    ///
    /// @param timestampGE  begin (left side) of the time window, inclusive (Greater-or-Equal)
    /// @param timestampLT  end (right side) of the time window, exclusive (Less-Than)
    ///
    /// @returns the list of all actions performed by UCM in provided time range
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10243
    /// @trace_id_dd=DD_UCM_10330
    /// @trace_id_sr=SR_UCM_00032
    /// @needwork = ad
    /// @endcode
    FutureGetHistory GetHistory(std::uint64_t const& timestampGE, std::uint64_t const& timestampLT) const noexcept;

    // Functions to be called by FsmManager
    /// @brief Activate the processed components.
    /// @return The result of the activate operation, which specifies if the
    /// operation was successful.
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10244
    /// @trace_id_dd=DD_UCM_10331
    /// @trace_id_sr=SR_UCM_00028,SR_UCM_00029,SR_UCM_00003,SR_UCM_00021,SR_UCM_00007,SR_UCM_00030
    /// @needwork = ad
    /// @endcode
    FutureVoid Activate() const noexcept;

    /// @brief Finish the processing for the current set of
    /// processed Software Packages. Cleanup all data of the processing
    /// including the sources of the Software Packages.
    ///
    /// @return The result of the finish operation, which specifies if the
    /// operation was successful.
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10245
    /// @trace_id_dd=DD_UCM_10332
    /// @trace_id_sr=SR_UCM_00015
    /// @needwork = ad
    /// @endcode
    FutureVoid Finish() const noexcept;
    /// @brief Process transferred Software Package.
    ///
    /// @param id Transfer ID of Software Package.
    ///
    /// @return The result of processing, which specifies if the
    /// operation was successful.
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10246
    /// @trace_id_dd=DD_UCM_10333
    /// @trace_id_sr=SR_UCM_00028,SR_UCM_00029,SR_UCM_00003,SR_UCM_00026
    /// @needwork = ad
    /// @endcode
    FutureVoid ProcessSwPackage(TransferIdType const& id) const noexcept;

    /// @brief Abort an ongoing processing of a Software Package.
    /// @param id Transfer ID of Software Package.
    /// @return The result of cancelling operation, which specifies if the
    /// it was successful.
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10247
    /// @trace_id_dd=DD_UCM_10334
    /// @trace_id_sr=SR_UCM_00020
    /// @needwork = ad
    /// @endcode
    FutureVoid Cancel(TransferIdType const& id) const noexcept;
    /// @brief Revert the changes done by processing (ProcessSwPackage) of one
    /// or several software packages.
    ///
    /// @return The result of the revert operation, which specifies if the
    /// operation was successful.
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10248
    /// @trace_id_dd=DD_UCM_10335
    /// @trace_id_sr=SR_UCM_00020
    /// @needwork = ad
    /// @endcode
    FutureVoid RevertProcessedSwPackages() const noexcept;

    /// @brief Rollback the system to the state
    /// before the packages were processed.
    ///
    /// @return The result of the rollback operation, which specifies if the
    /// operation was successful.
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10249
    /// @trace_id_dd=DD_UCM_10336
    /// @needwork = ad
    /// @endcode
    FutureVoid Rollback() const noexcept;

    /// @brief Get the UCM Instance Identifier.
    ///
    /// @return The Identifier of UCM Instance.
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10250
    /// @trace_id_dd=DD_UCM_10337
    /// @needwork = ad
    /// @endcode
    FutureGetId GetId() const noexcept;

    /// @brief Continues the rollingback if needed in separate thread
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10251
    /// @trace_id_dd=DD_UCM_10338
    /// @needwork = ad
    /// @endcode
    void ContinueIfNecessary() const noexcept;

private:
    /// @brief ucmid here
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10339
    /// @needwork = dda
    /// @endcode
    AraString ucmId_;

    /// @brief Implements actions with software packages
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10340
    /// @needwork = dda
    /// @endcode
    std::unique_ptr< SoftwarePackageManager > swpkgManager_;

    /// @brief Implements actions with software clusters
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10341
    /// @needwork = dda
    /// @endcode
    std::unique_ptr< FileSystemSWCLManager > swclManager_;

    /// @brief Manager which controls everything about UCM FSM (state transitions etc.)
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10342
    /// @needwork = dda
    /// @endcode
    std::unique_ptr< FsmManager > fsmManager_;
};

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // ARA_UCM_PKGMGR_PACKAGE_MANAGER_PACKAGE_MANAGER_IMPL_H_
