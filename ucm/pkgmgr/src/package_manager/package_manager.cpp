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
/// @file       package_manager.cpp
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
/// @interface_level=module
/// @trace_id_sr=SR_UCM_00001
/// @unit_name=PackageManager
/// @unit_description=PackageManagement service implementation
/// @endcode
///
/// ================================================================

#include "package_manager.h"

#include "package_manager_impl.h"
#include "util/future_ext.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief Initializes a new instance of this service with the given instance id
/// @param id Instance identifier required by ara com
/// @return unique_ptr of PackageManager
/// @throws no
std::unique_ptr< PackageManager > PackageManager::Create(ara::core::InstanceSpecifier const& id)
{
    std::unique_ptr< PackageManager > pmUPtr{nullptr};

    AraResult< PackageManager > ret{
        PMSkeleton::Create< PackageManager >(id, ara::com::MethodCallProcessingMode::kEvent)};
    if (ret.HasValue()) {
        pmUPtr = std::make_unique< PackageManager >(std::move(ret).Value());
    }

    return pmUPtr;
}

/// @brief Set an implementation for this service
/// @param impl to be set
void PackageManager::SetImpl(std::unique_ptr< PackageManagerImpl > impl) noexcept { impl_ = std::move(impl); }

/// @brief CurrentStatus_Getter
/// @return future
/// @throws no
AraFuture< PackageManagerStatusType > PackageManager::CurrentStatus_Getter()
{
    return SyncCall< PackageManagerStatusType >([this]() noexcept { return statusType_; });
}

/// @brief Update the current status
/// @param newState to be set
/// @throws no
void PackageManager::UpdateCurrentStatus(PackageManagerStatusType const& newState)
{
    statusType_ = newState;
    std::ignore = CurrentStatus.Update(newState);
}

/// @brief Activate the processed components.
/// @return The result of the activate operation, which specifies if the
/// operation was successful.
/// @throws no
FutureVoid PackageManager::Activate() noexcept { return impl_->Activate(); }

/// @brief Abort an ongoing processing of a Software Package.
/// @param id Transfer ID of Software Package.
/// @return The result of cancelling operation, which specifies if the
/// it was successful.
/// @throws no
FutureVoid PackageManager::Cancel(TransferIdType const& id) noexcept { return impl_->Cancel(id); }

/// @brief Delete a transferred Software Package.
///
/// @param id Transfer ID of Software Package.
///
/// @return The result of operation, which specifies if the
/// request was successful.
/// @throws no
FutureVoid PackageManager::DeleteTransfer(TransferIdType const& id) noexcept { return impl_->DeleteTransfer(id); }

/// @brief Finish the processing for the current set of
/// processed Software Packages. Cleanup all data of the processing
/// including the sources of the Software Packages.
///
/// @return The result of the finish operation, which specifies if the
/// operation was successful.
/// @throws no
FutureVoid PackageManager::Finish() noexcept { return impl_->Finish(); }

/// @brief Retrieve all actions performed by UCM in provided time range.
///
/// @param timestampGE  begin (left side) of the time window, inclusive (Greater-or-Equal)
/// @param timestampLT  end (right side) of the time window, exclusive (Less-Than)
///
/// @returns the list of all actions performed by UCM in provided time range
/// @throws no
FutureGetHistory PackageManager::GetHistory(std::uint64_t const& timestampGE, std::uint64_t const& timestampLT) noexcept
{
    return impl_->GetHistory(timestampGE, timestampLT);
}

/// @brief Get the UCM Instance Identifier.
///
/// @return The Identifier of UCM Instance.
/// @throws no
FutureGetId PackageManager::GetId() noexcept { return impl_->GetId(); }

/// @brief Retrieve a list of SoftwareClusters that have pending changes.
/// The returned list includes all SoftwareClusters that are to be added,
/// updated or removed. The list of changes is extended
/// in the course of processing software Packages.
///
/// @return List of SoftwareClusters that are in state kAdded,
/// kUpdated or kRemoved.
/// @throws no
FutureGetSwClusterChangeInfo PackageManager::GetSwClusterChangeInfo() noexcept
{
    return impl_->GetSwClusterChangeInfo();
}

/// @brief Retrieve a list of SoftwareClusters.
///
/// @return List of installed SoftwareClusters that are in state kPresent.
/// @throws no
FutureGetSwClusterInfo PackageManager::GetSwClusterInfo() noexcept { return impl_->GetSwClusterInfo(); }
/// @brief Retrieve a Software Package list.
///
/// @return List of all Software Packages that have been successfully
/// transferred and are ready to be installed.
/// @throws no
FutureGetSwPackages PackageManager::GetSwPackages() noexcept { return impl_->GetSwPackages(); }
/// @brief Retrieve a Software Clusters information list.
///
/// @return List of the general information of the Software Clusters present
/// in the platform.
/// @throws no
FutureGetSwClusterDescription PackageManager::GetSwClusterDescription() noexcept
{
    return impl_->GetSwClusterDescription();
}

/// @brief Get the progress of the currently processed Software Package.
///
/// @param id Transfer ID of Software Package.
///
/// @return The progress of the current package processing (0x00 - 0x64).
/// @throws no
FutureGetSwProcessProgress PackageManager::GetSwProcessProgress(TransferIdType const& id) noexcept
{
    return impl_->GetSwProcessProgress(id);
}
/// @brief Process transferred Software Package.
///
/// @param id Transfer ID of Software Package.
///
/// @return The result of processing, which specifies if the
/// operation was successful.
/// @throws no
FutureVoid PackageManager::ProcessSwPackage(TransferIdType const& id) noexcept { return impl_->ProcessSwPackage(id); }
/// @brief Revert the changes done by processing (ProcessSwPackage) of one
/// or several software packages.
///
/// @return The result of the revert operation, which specifies if the
/// operation was successful.
/// @throws no
FutureVoid PackageManager::RevertProcessedSwPackages() noexcept { return impl_->RevertProcessedSwPackages(); }

/// @brief Rollback the system to the state
/// before the packages were processed.
///
/// @return The result of the rollback operation, which specifies if the
/// operation was successful.
/// @throws no
FutureVoid PackageManager::Rollback() noexcept { return impl_->Rollback(); }

/// @brief Block-wise transfer of a Software Package.
///
/// @param id Transfer ID of the currently running request.
/// @param data Data block of the Software Package.
/// @param blockCounter Block counter of the current block.
///
/// @return The result of transferring current data block, which specifies if the
/// sw package has been successfully transferred.
/// @throws no
FutureVoid PackageManager::TransferData(TransferIdType const& id,
                                        ByteVectorType const& data,
                                        std::uint64_t const& blockCounter) noexcept
{
    return impl_->TransferData(id, data, blockCounter);
}
/// @brief Finish the transfer of a Software Package.
///
/// @param id Transfer ID of the currently running request.
///
/// @return The result of finishing this sw package transfer, which specifies if the
/// sw package has been successfully transferred.
/// @throws no
FutureVoid PackageManager::TransferExit(TransferIdType const& id) noexcept { return impl_->TransferExit(id); }
/// @brief Start the transfer of a Software Package.
/// Transfer Id for subsequent calls to TransferData will be generated
/// and returned as a part of TransferStartOutput.
///
/// @param size Size (in bytes) of the Software Package to be transferred.
///
/// @return The struct which contains the result of the transfer start operation
/// and transfer id generated for this operation.
/// @throws no
FutureTransferStart PackageManager::TransferStart(std::uint64_t const& size) noexcept
{
    return impl_->TransferStart(size);
}

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara
