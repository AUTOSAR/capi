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
/// @file       package_manager_impl.cpp
/// @brief      PackageManagement service implementation
/// @details
/// @date       2024-08-15
/// @author     cuiyinli
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/UCM/PackageManager
/// @interface_level=unit
/// @trace_id_sr=SR_UCM_00001
/// @unit_name=PackageManagerImpl
/// @unit_description=PackageManagement service implementation
/// @endcode
///
/// ================================================================

#include "package_manager_impl.h"

#include "util/future_ext.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief Initializes the PackageManagerImpl
/// @param ucmId
/// @param swpkgManager Manager to deal with software packages
/// @param swclManager
/// @param fsmManager
PackageManagerImpl::PackageManagerImpl(AraString const& ucmId,
                                       std::unique_ptr< SoftwarePackageManager > swpkgManager,
                                       std::unique_ptr< FileSystemSWCLManager > swclManager,
                                       std::unique_ptr< FsmManager > fsmManager) noexcept
    : ucmId_{std::move(ucmId)}
    , swpkgManager_{std::move(swpkgManager)}
    , swclManager_{std::move(swclManager)}
    , fsmManager_{std::move(fsmManager)}
{
}

// Operations on software packages (SoftwarePackageManager)
/// @brief Block-wise transfer of a Software Package.
///
/// @param id Transfer ID of the currently running request.
/// @param data Data block of the Software Package.
/// @param blockCounter Block counter of the current block.
///
/// @return The result of transferring current data block, which specifies if the
/// sw package has been successfully transferred.
/// @throws no
FutureVoid PackageManagerImpl::TransferData(TransferIdType const& id,
                                            ByteVectorType const& data,
                                            std::uint64_t const& blockCounter) const noexcept
{
    return swpkgManager_->TransferData(id, data, blockCounter);
}
/// @brief Finish the transfer of a Software Package.
///
/// @param id Transfer ID of the currently running request.
///
/// @return The result of finishing this sw package transfer, which specifies if the
/// sw package has been successfully transferred.
/// @throws no
FutureVoid PackageManagerImpl::TransferExit(TransferIdType const& id) const noexcept
{
    return swpkgManager_->TransferExit(id, swclManager_.get());
}
/// @brief Start the transfer of a Software Package.
/// Transfer Id for subsequent calls to TransferData will be generated
/// and returned as a part of TransferStartOutput.
///
/// @param size Size (in bytes) of the Software Package to be transferred.
///
/// @return The struct which contains the result of the transfer start operation
/// and transfer id generated for this operation.
/// @throws no
FutureTransferStart PackageManagerImpl::TransferStart(std::uint64_t const size) const noexcept
{
    return swpkgManager_->TransferStart(size);
}

/// @brief Delete a transferred Software Package.
///
/// @param id Transfer ID of Software Package.
///
/// @return The result of operation, which specifies if the
/// request was successful.
/// @throws no
FutureVoid PackageManagerImpl::DeleteTransfer(TransferIdType const& id) const noexcept
{
    return swpkgManager_->DeleteTransfer(id);
}
/// @brief Get the progress of the currently processed Software Package.
///
/// @param id Transfer ID of Software Package.
///
/// @return The progress of the current package processing (0x00 - 0x64).
/// @throws no
FutureGetSwProcessProgress PackageManagerImpl::GetSwProcessProgress(TransferIdType const& id) const noexcept
{
    return swpkgManager_->GetSwProcessProgress(id);
}
/// @brief Retrieve a Software Package list.
///
/// @return List of all Software Packages that have been successfully
/// transferred and are ready to be installed.
/// @throws no
FutureGetSwPackages PackageManagerImpl::GetSwPackages() const noexcept { return swpkgManager_->GetSwPackages(); }

// Operations on Software Clusters (FileSystemSWCLManager)
/// @brief Retrieve a list of SoftwareClusters that have pending changes.
/// The returned list includes all SoftwareClusters that are to be added,
/// updated or removed. The list of changes is extended
/// in the course of processing software Packages.
///
/// @return List of SoftwareClusters that are in state kAdded,
/// kUpdated or kRemoved.
/// @throws no
FutureGetSwClusterChangeInfo PackageManagerImpl::GetSwClusterChangeInfo() const noexcept
{
    return swclManager_->GetSwClusterChangeInfo();
}
/// @brief Retrieve a Software Clusters information list.
///
/// @return List of the general information of the Software Clusters present
/// in the platform.
/// @throws no
FutureGetSwClusterDescription PackageManagerImpl::GetSwClusterDescription() const noexcept
{
    return swclManager_->GetSwClusterDescription();
}
/// @brief Retrieve a list of SoftwareClusters.
///
/// @return List of installed SoftwareClusters that are in state kPresent.
/// @throws no
FutureGetSwClusterInfo PackageManagerImpl::GetSwClusterInfo() const noexcept
{
    return swclManager_->GetSwClusterInfo();
}

/// @brief Retrieve all actions performed by UCM in provided time range.
///
/// @param timestampGE  begin (left side) of the time window, inclusive (Greater-or-Equal)
/// @param timestampLT  end (right side) of the time window, exclusive (Less-Than)
///
/// @returns the list of all actions performed by UCM in provided time range
/// @throws no
FutureGetHistory PackageManagerImpl::GetHistory(std::uint64_t const& timestampGE,
                                                std::uint64_t const& timestampLT) const noexcept
{
    return swclManager_->GetHistory(timestampGE, timestampLT);
}

// FSM part (FsmManager)
/// @brief Activate the processed components.
/// @return The result of the activate operation, which specifies if the
/// operation was successful.
/// @throws no
FutureVoid PackageManagerImpl::Activate() const noexcept { return fsmManager_->Activate(); }

/// @brief Finish the processing for the current set of
/// processed Software Packages. Cleanup all data of the processing
/// including the sources of the Software Packages.
///
/// @return The result of the finish operation, which specifies if the
/// operation was successful.
/// @throws no
FutureVoid PackageManagerImpl::Finish() const noexcept { return fsmManager_->Finish(); }
/// @brief Revert the changes done by processing (ProcessSwPackage) of one
/// or several software packages.
///
/// @return The result of the revert operation, which specifies if the
/// operation was successful.
/// @throws no
FutureVoid PackageManagerImpl::RevertProcessedSwPackages() const noexcept
{
    return fsmManager_->RevertProcessedSwPackages();
}

/// @brief Rollback the system to the state
/// before the packages were processed.
///
/// @return The result of the rollback operation, which specifies if the
/// operation was successful.
/// @throws no
FutureVoid PackageManagerImpl::Rollback() const noexcept { return fsmManager_->Rollback(); }

/// @brief Continues the rollingback if needed in separate thread
void PackageManagerImpl::ContinueIfNecessary() const noexcept { fsmManager_->ContinueIfNecessary(); }

/// @brief Abort an ongoing processing of a Software Package.
/// @param id Transfer ID of Software Package.
/// @return The result of cancelling operation, which specifies if the
/// it was successful.
/// @throws no
FutureVoid PackageManagerImpl::Cancel(TransferIdType const& id) const noexcept { return fsmManager_->Cancel(id); }
/// @brief Process transferred Software Package.
///
/// @param id Transfer ID of Software Package.
///
/// @return The result of processing, which specifies if the
/// operation was successful.
/// @throws no
FutureVoid PackageManagerImpl::ProcessSwPackage(TransferIdType const& id) const noexcept
{
    return fsmManager_->ProcessSwPackage(id);
}

// some simple function
/// @brief Get the UCM Instance Identifier.
///
/// @return The Identifier of UCM Instance.
/// @throws no
FutureGetId PackageManagerImpl::GetId() const noexcept
{
    // TBD Id should be read from manifest
    return SyncCall< GetIdOutput >([this]() { return AraResult< GetIdOutput >({ucmId_}); });
}

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara
