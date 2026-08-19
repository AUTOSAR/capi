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
/// @file       package_manager.h
/// @brief      PackageManagement service interface
/// @details
/// @date       2024-08-15
/// @author     cuiyinli
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @export_level=/UCM/PackageManager
/// @module_path=/UCM/PackageManager
/// @interface_level=module
/// @trace_id_sr=SR_UCM_00001,SR_UCM_00024
/// @unit_name=PackageManager
/// @unit_description=PackageManagement service interface
/// @endcode
///
/// ================================================================

#ifndef ARA_UCM_PKGMGR_PACKAGE_MANAGER_PACKAGE_MANAGER_H_
#define ARA_UCM_PKGMGR_PACKAGE_MANAGER_PACKAGE_MANAGER_H_

#include "ara/ucm/pkgmgr/impl_type_activateoptiontype.h"
#include "common/alias.h"
#include "package_manager_impl.h"
#include "types/impl_type_finalactiontype.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief This is the implementation of the PackageManagement service interface
/// which can be used by clients to install, update or uninstall applications,
/// configuration data, calibration data or manifests.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10198
/// @trace_id_dd=DD_UCM_10283
/// @needwork = ad
/// @endcode
class PackageManager : public ara::ucm::pkgmgr::skeleton::PackageManagementSkeleton
{
    /// @brief Skeleton
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_
    /// @trace_id_dd=DD_UCM_
    /// @needwork = no
    /// @endcode
    using Skeleton = ara::ucm::pkgmgr::skeleton::PackageManagementSkeleton;
    /// @brief Skeleton
    using Skeleton::Skeleton;

public:
    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10199
    /// @trace_id_dd=DD_UCM_10284
    /// @needwork = ad
    /// @endcode
    ~PackageManager() noexcept override = default;

    /// @brief Copy constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10200
    /// @trace_id_dd=DD_UCM_10285
    /// @needwork = ad
    /// @endcode
    PackageManager(const PackageManager& other) = delete;
    /// @brief Copy assignment operator
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10201
    /// @trace_id_dd=DD_UCM_10286
    /// @needwork = ad
    /// @endcode
    PackageManager& operator=(const PackageManager& other) = delete;

    /// @brief Move constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10202
    /// @trace_id_dd=DD_UCM_10287
    /// @needwork = ad
    /// @endcode
    PackageManager(PackageManager&& other) = default;
    /// @brief Move assignment operator
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10203
    /// @trace_id_dd=DD_UCM_10288
    /// @needwork = ad
    /// @endcode
    PackageManager& operator=(PackageManager&& other) = default;

    /// @brief Initializes a new instance of this service with the given instance id
    /// @param id Instance identifier required by ara com
    /// @return unique_ptr of PackageManager
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10204
    /// @trace_id_dd=DD_UCM_10289
    /// @needwork = ad
    /// @endcode
    static std::unique_ptr< PackageManager > Create(ara::core::InstanceSpecifier const& id);

    /// @brief Set an implementation for this service
    ///
    /// The sequence to initialize UCM is as follows: first create this class, then create PackageManagerImpl, then
    /// connect them by using this function. In this way the dependency between the instances is broken. Please note
    /// that by calling this function the current class takes an ownership over an instance of PackageManagerImpl.
    ///
    /// @param impl to be set
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10205
    /// @trace_id_dd=DD_UCM_10290
    /// @needwork = ad
    /// @endcode
    void SetImpl(std::unique_ptr< PackageManagerImpl > impl) noexcept;

    /// @brief Update the current status
    /// The function is used mostly for testing purposes
    /// to make possible to catch the status update easily.
    ///
    /// @param newState to be set
    ///
    /// @return nothing
    /// @throws no
    /// @code{.isoft}
    /// @uptrace={SWS_UCM_00019, c1193fc064a89e66cc70442317a4e93789cb6089}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10206
    /// @trace_id_dd=DD_UCM_10291
    /// @needwork = ad
    /// @endcode
    virtual void UpdateCurrentStatus(PackageManagerStatusType const& newState);

    /// @brief Activate the processed components.
    ///
    /// @return The result of the activate operation, which specifies if the
    /// operation was successful.
    /// @throws no
    ///
    /// @code{.isoft}
    /// @uptrace={SWS_UCM_00025, c861f009a45d863eac7b7b8a9dd757e98ad66788}
    /// @uptrace={SWS_UCM_00099, 70b231c56978c822310b81eacf09f6b37d8bb27b}
    /// TODO: UCM cannot update itself currently
    /// @uptrace={SWS_UCM_00100}
    /// TODO: UCM cannot update host currently
    /// @uptrace={SWS_UCM_00101}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10207
    /// @trace_id_dd=DD_UCM_10292
    /// @needwork = ad
    /// @endcode
    FutureVoid Activate() noexcept override;

    /// @brief Abort an ongoing processing of a Software Package.
    ///
    /// @param id Transfer ID of Software Package.
    ///
    /// @return The result of cancelling operation, which specifies if the
    /// it was successful.
    /// @throws no
    ///
    /// TODO: This is not actually working AR-102527
    /// @code{.isoft}
    /// @uptrace={SWS_UCM_00003}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10208
    /// @trace_id_dd=DD_UCM_10293
    /// @needwork = ad
    /// @endcode
    FutureVoid Cancel(TransferIdType const& id) noexcept override;

    /// @brief Delete a transferred Software Package.
    ///
    /// @param id Transfer ID of Software Package.
    ///
    /// @return The result of operation, which specifies if the
    /// request was successful.
    /// @throws no
    ///
    /// @code{.isoft}
    /// @uptrace={SWS_UCM_00021, 42fb0196fc3e30aaebd956a796f3d0c9087747ff}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10209
    /// @trace_id_dd=DD_UCM_10294
    /// @needwork = ad
    /// @endcode
    FutureVoid DeleteTransfer(TransferIdType const& id) noexcept override;

    /// @brief Finish the processing for the current set of
    /// processed Software Packages. Cleanup all data of the processing
    /// including the sources of the Software Packages.
    ///
    /// @return The result of the finish operation, which specifies if the
    /// operation was successful.
    /// @throws no
    ///
    /// @code{.isoft}
    /// @uptrace={SWS_UCM_00020, c304f705561f30589f9a5c477025efd999d874ec}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10210
    /// @trace_id_dd=DD_UCM_10295
    /// @needwork = ad
    /// @endcode
    FutureVoid Finish() noexcept override;

    /// @brief Retrieve all actions performed by UCM in provided time range.
    ///
    /// @param timestampGE  begin (left side) of the time window, inclusive (Greater-or-Equal)
    /// @param timestampLT  end (right side) of the time window, exclusive (Less-Than)
    ///
    /// @returns the list of all actions performed by UCM in provided time range
    /// @throws no
    ///
    /// @code{.isoft}
    /// @uptrace={SWS_UCM_00115, 245afb4f017b33c4f234c4c6971f8909056dfe8e}
    /// @uptrace={SWS_UCM_00160, 86ad8df5a74be8af738fbee74671cd671a3def16}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10211
    /// @trace_id_dd=DD_UCM_10296
    /// @needwork = ad
    /// @endcode
    FutureGetHistory GetHistory(std::uint64_t const& timestampGE, std::uint64_t const& timestampLT) noexcept override;

    /// @brief Get the UCM Instance Identifier.
    ///
    /// @return The Identifier of UCM Instance.
    /// @throws no
    ///
    /// @code{.isoft}
    /// @uptrace={SWS_UCM_00009, eebe8d18318d58b2b4ad454a0a9c4259a7d62a7f}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10212
    /// @trace_id_dd=DD_UCM_10297
    /// @needwork = ad
    /// @endcode
    FutureGetId GetId() noexcept override;

    /// @brief Retrieve a list of SoftwareClusters that have pending changes.
    /// The returned list includes all SoftwareClusters that are to be added,
    /// updated or removed. The list of changes is extended
    /// in the course of processing software Packages.
    ///
    /// @return List of SoftwareClusters that are in state kAdded,
    /// kUpdated or kRemoved.
    /// @throws no
    ///
    /// @code{.isoft}
    /// @uptrace={SWS_UCM_00030, fc6b6673c961e2808696fd9132dab3bd51475689}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10213
    /// @trace_id_dd=DD_UCM_10298
    /// @needwork = ad
    /// @endcode
    FutureGetSwClusterChangeInfo GetSwClusterChangeInfo() noexcept override;

    /// @brief Retrieve a list of SoftwareClusters.
    ///
    /// @return List of installed SoftwareClusters that are in state kPresent.
    /// @throws no
    ///
    /// @code{.isoft}
    /// @uptrace={SWS_UCM_00004, da85e8207663f7e18aa4ba7d507bdf5a7fe7fae3}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10214
    /// @trace_id_dd=DD_UCM_10299
    /// @needwork = ad
    /// @endcode
    FutureGetSwClusterInfo GetSwClusterInfo() noexcept override;

    /// @brief Retrieve a Software Package list.
    ///
    /// @return List of all Software Packages that have been successfully
    /// transferred and are ready to be installed.
    /// @throws no
    ///
    /// TODO: implement consecutiveBytesReceived and consecutiveBlocksReceived
    /// existing implementation still uses size only. AR-102456
    /// @code{.isoft}
    /// @uptrace={SWS_UCM_00069}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10215
    /// @trace_id_dd=DD_UCM_10300
    /// @needwork = ad
    /// @endcode
    FutureGetSwPackages GetSwPackages() noexcept override;

    /// @brief Retrieve a Software Clusters information list.
    ///
    /// @return List of the general information of the Software Clusters present
    /// in the platform.
    /// @throws no
    ///
    /// @code{.isoft}
    /// @uptrace={SWS_UCM_00185, 81712418206ee56bab4db464aed7161fd394816f}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10216
    /// @trace_id_dd=DD_UCM_10301
    /// @needwork = ad
    /// @endcode
    FutureGetSwClusterDescription GetSwClusterDescription() noexcept override;

    /// @brief Get the progress of the currently processed Software Package.
    ///
    /// @param id Transfer ID of Software Package.
    ///
    /// @return The progress of the current package processing (0x00 - 0x64).
    /// @throws no
    ///
    /// @code{.isoft}
    /// @uptrace={SWS_UCM_00018, 879a4e06bb574751c77e63447157577da323e458}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10217
    /// @trace_id_dd=DD_UCM_10302
    /// @needwork = ad
    /// @endcode
    FutureGetSwProcessProgress GetSwProcessProgress(TransferIdType const& id) noexcept override;

    /// @brief Process transferred Software Package.
    ///
    /// @param id Transfer ID of Software Package.
    ///
    /// @return The result of processing, which specifies if the
    /// operation was successful.
    /// @throws no
    ///
    /// @code{.isoft}
    /// @uptrace={SWS_UCM_00001, a665954b7a62b2349db109a9408efe084b8a7136}
    /// @uptrace={SWS_UCM_00137, 2052ed9640ab051f6212c8c03aa7eeafc653b726}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10218
    /// @trace_id_dd=DD_UCM_10303
    /// @needwork = ad
    /// @endcode
    FutureVoid ProcessSwPackage(TransferIdType const& id) noexcept override;

    /// @brief Revert the changes done by processing (ProcessSwPackage) of one
    /// or several software packages.
    ///
    /// @return The result of the revert operation, which specifies if the
    /// operation was successful.
    /// @throws no
    ///
    /// @code{.isoft}
    /// @uptrace={SWS_UCM_00024, 172b762f2b191a7eb048ce985f423291856573bf}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10219
    /// @trace_id_dd=DD_UCM_10304
    /// @needwork = ad
    /// @endcode
    FutureVoid RevertProcessedSwPackages() noexcept override;

    /// @brief Rollback the system to the state
    /// before the packages were processed.
    ///
    /// @return The result of the rollback operation, which specifies if the
    /// operation was successful.
    /// @throws no
    ///
    /// @code{.isoft}
    /// @uptrace={SWS_UCM_00005, ee3e6bb15b678695524b76c5f32d908d094f0974}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10220
    /// @trace_id_dd=DD_UCM_10305
    /// @needwork = ad
    /// @endcode
    FutureVoid Rollback() noexcept override;

    /// @brief Block-wise transfer of a Software Package.
    ///
    /// @param id Transfer ID of the currently running request.
    /// @param data Data block of the Software Package.
    /// @param blockCounter Block counter of the current block.
    ///
    /// @return The result of transferring current data block, which specifies if the
    /// sw package has been successfully transferred.
    /// @throws no
    ///
    /// @code{.isoft}
    /// @uptrace={SWS_UCM_00007, c9e41bf68059e0421604c79e27feced11f961119}
    /// @uptrace={SWS_UCM_00008, d4f76d1688bffafd81411bb5ba6b3e9cb6ce2985}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10221
    /// @trace_id_dd=DD_UCM_10306
    /// @needwork = ad
    /// @endcode
    FutureVoid TransferData(TransferIdType const& id,
                            ByteVectorType const& data,
                            std::uint64_t const& blockCounter) noexcept override;

    /// @brief Finish the transfer of a Software Package.
    ///
    /// @param id Transfer ID of the currently running request.
    ///
    /// @return The result of finishing this sw package transfer, which specifies if the
    /// sw package has been successfully transferred.
    /// @throws no
    ///
    /// @code{.isoft}
    /// @uptrace={SWS_UCM_00007, c9e41bf68059e0421604c79e27feced11f961119}
    /// @uptrace={SWS_UCM_00010, a1bea3f583c3ef00e3c12792db807ec836d80f78}
    /// @uptrace={SWS_UCM_00103, 72d6a4ff65df294f6ab37f5eee80221335635046}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10222
    /// @trace_id_dd=DD_UCM_10307
    /// @needwork = ad
    /// @endcode
    FutureVoid TransferExit(TransferIdType const& id) noexcept override;

    /// @brief Start the transfer of a Software Package.
    /// Transfer Id for subsequent calls to TransferData will be generated
    /// and returned as a part of TransferStartOutput.
    ///
    /// @param size Size (in bytes) of the Software Package to be transferred.
    ///
    /// @return The struct which contains the result of the transfer start operation
    /// and transfer id generated for this operation.
    /// @throws no
    ///
    /// @code{.isoft}
    /// @uptrace={SWS_UCM_00007, c9e41bf68059e0421604c79e27feced11f961119}
    /// @uptrace={SWS_UCM_00088, 6fde704d91b8e40ced3cb4f3f78b3659d274fee3}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10223
    /// @trace_id_dd=DD_UCM_10308
    /// @needwork = ad
    /// @endcode
    FutureTransferStart TransferStart(std::uint64_t const& size) noexcept override;

    /// @brief Resets the software clusters change info. vector
    ///
    /// @returns nothing
    /// @throws no
    ///void ResetSwclChangeInfo();////////TODO////mytodo//////////////////mydel////

    /// @brief getter returning a reference to impl_
    /// @return a reference to impl_
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10224
    /// @trace_id_dd=DD_UCM_10309
    /// @needwork = ad
    /// @endcode
    PackageManagerImpl& GetImpl() const { return *impl_; }
    /// @brief getter returning a pointer to impl_
    /// @return a pointer to impl_
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10225
    /// @trace_id_dd=DD_UCM_10310
    /// @needwork = ad
    /// @endcode
    PackageManagerImpl* GetImplPtr() const noexcept { return impl_.get(); }

    // Currently, the field does not have a default Getter. Remove it after it is available.
    /// @brief CurrentStatus_Getter
    /// @return future
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10226
    /// @trace_id_dd=DD_UCM_10311
    /// @trace_id_sr=SR_UCM_00024
    /// @needwork = ad
    /// @endcode
    AraFuture< PackageManagerStatusType > CurrentStatus_Getter();

private:
    /// @brief statusType_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10312
    /// @needwork = dda
    /// @endcode
    PackageManagerStatusType statusType_{PackageManagerStatusType::kIdle};
    /// @brief Smart pointer to the implementation class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10313
    /// @needwork = dda
    /// @endcode
    std::unique_ptr< PackageManagerImpl > impl_;
};

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // ARA_UCM_PKGMGR_PACKAGE_MANAGER_PACKAGE_MANAGER_H_
