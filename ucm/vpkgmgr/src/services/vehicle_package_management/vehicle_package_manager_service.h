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
/// @file       vehicle_package_manager_service.h
/// @brief      Service interface provided by VUCM to the OTA client adaptive application
/// @details
/// @date       2024-07-20
/// @author     hanzhibo
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// export_level=/UCM Master/Package Management Service
/// @module_path=/UCM Master/Services
/// @interface_level=module
/// @trace_id_sr=SR_UCM_00035,SR_UCM_00033,SR_UCM_00034,SR_UCM_00042,SR_UCM_00039
/// @unit_name=VehiclePackageManagerService
/// @unit_description=Service interface provided by VUCM to the OTA client adaptive application
/// @endcode
///
/// ================================================================

#ifndef ARA_UCM_VPKGMGR_VEHICLE_PACKAGE_MANAGER_IMPL_H_
#define ARA_UCM_VPKGMGR_VEHICLE_PACKAGE_MANAGER_IMPL_H_

#include <memory>
#include <utility>
#include <vector>

#include "ara/core/promise.h"
#include "ara/core/vector.h"
#include "ara/log/logger.h"
#include "ara/ucm/pkgmgr/packagemanagement_proxy.h"
#include "ara/ucm/pkgmgr/vehiclepackagemanagement_skeleton.h"
#include "fsm/fsm_manager.h"

namespace ara {
namespace ucm {
namespace vpkgmgr {

/// @brief Service interface provided by VUCM to the OTA client adaptive application
///
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00450
/// @trace_id_dd=DD_UCM_Master_00904
/// @needwork = ad
/// @endcode
class VehiclePackageManagerService : public pkgmgr::skeleton::VehiclePackageManagementSkeleton
{
public:
    /// @brief alias Ptr
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_
    /// @trace_id_dd=DD_UCM_
    /// @needwork = no
    /// @endcode
    using Ptr = std::shared_ptr< VehiclePackageManagerService >;

    /// @brief constructor
    /// @param instanceSpec
    /// @param mode
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00451
    /// @trace_id_dd=DD_UCM_Master_00905
    /// @needwork = ad
    /// @endcode
    VehiclePackageManagerService(ara::core::InstanceSpecifier instanceSpec,
                                 ara::com::MethodCallProcessingMode const mode);
    /// @brief constructor
    /// @param instanceId
    /// @param mode
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00452
    /// @trace_id_dd=DD_UCM_Master_00906
    /// @needwork = ad
    /// @endcode
    VehiclePackageManagerService(ara::com::InstanceIdentifier instanceId,
                                 ara::com::MethodCallProcessingMode const mode);

    /// @brief FieldInit
    /// @param state
    /// @return bool
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00453
    /// @trace_id_dd=DD_UCM_Master_00907
    /// @trace_id_sr=SR_UCM_00035,SR_UCM_00042
    /// @needwork = ad
    /// @endcode
    bool FieldInit(pkgmgr::TransferStateType const& state);

    ///void ContinueVerificationIfNeeded();

    /// @brief Let a new campaign begin
    /// @return Returns a void future or a specific error code (please refer to ara::core::Future usage instructions)
    /// @throws no
    /// @code{.isoft}
    /// @uptrace={SWS_UCM_01271}
    ///
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00454
    /// @trace_id_dd=DD_UCM_Master_00908
    /// @needwork = ad
    /// @endcode
    ara::core::Future< void > AllowCampaign() override;

    /// @brief This method aborts the ongoing vehicle package processing activity.
    /// @param disableCampaign Whether to disable new update activities. false: only disable this update activity. true: also disable subsequent update activities
    /// @return Returns a void future or a specific error code (please refer to ara::core::Future usage instructions)
    /// @throws no
    /// @code{.isoft}
    /// @uptrace={SWS_UCM_01227, SWS_UCM_01236, SWS_UCM_01244, SWS_UCM_01245, SWS_UCM_01270, SWS_UCM_01271,
    /// SWS_UCM_01269}
    ///
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00455
    /// @trace_id_dd=DD_UCM_Master_00909
    /// @needwork = ad
    /// @endcode
    ara::core::Future< void > CancelCampaign(bool const& disableCampaign) override;

    /// @brief Retrieve all operations executed by the UCM host
    /// @param timestampGE Earliest timestamp (inclusive)
    /// @param timestampLT Latest timestamp (exclusive)
    /// @return Returns a future of all operation records executed by the UCM host or a specific error code (please refer to ara::core::Future usage instructions)
    /// @throws no
    /// @code{.isoft}
    /// @uptrace={SWS_UCM_01247}
    ///
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00456
    /// @trace_id_dd=DD_UCM_Master_00910
    /// @trace_id_sr=SR_UCM_00035,SR_UCM_00034
    /// @needwork = ad
    /// @endcode
    ara::core::Future< GetCampaignHistoryOutput > GetCampaignHistory(std::uint64_t const& timestampGE,
                                                                     std::uint64_t const& timestampLT) override;

    /// @brief Compares the provided list of available software clusters from the vehicle backend with the internal information of software clusters currently in the vehicle, and returns the list of software clusters selected for update
    /// @param availableSoftwarePackages List of available software packages from the backend
    /// @return Returns a future of the list of available update software packages from the backend or a specific error code (please refer to ara::core::Future usage instructions)
    /// @throws no
    /// @code{.isoft}
    /// @uptrace={SWS_UCM_01102}
    /// @uptrace={SWS_UCM_01103}
    ///
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00457
    /// @trace_id_dd=DD_UCM_Master_00911
    /// @needwork = ad
    /// @endcode
    ara::core::Future< SwPackageInventoryOutput > SwPackageInventory(
        pkgmgr::SwNameVersionVectorType const& availableSoftwarePackages) override;

    /// @brief Get the list of software clusters in the kPresent state on the platform.
    /// @return Returns a future of a software cluster list or a specific error code (please refer to ara::core::Future usage instructions)
    /// @throws no
    /// @code{.isoft}
    /// @uptrace={SWS_UCM_01101}
    ///
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00458
    /// @trace_id_dd=DD_UCM_Master_00912
    /// @trace_id_sr=SR_UCM_00035,SR_UCM_00033
    /// @needwork = ad
    /// @endcode
    ara::core::Future< GetSwClusterInfoOutput > GetSwClusterInfo() override;

    /// @brief Get software package information that is part of the current active activity processed by VUCM
    /// @return Returns a future of a software package information list or a specific error code (please refer to ara::core::Future usage instructions)
    /// @throws no
    /// @code{.isoft}
    /// @uptrace={SWS_UCM_01306}
    ///
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00459
    /// @trace_id_dd=DD_UCM_Master_00913
    /// @needwork = ad
    /// @endcode
    ara::core::Future< GetSwPackagesOutput > GetSwPackages() override;

    /// @brief Start transporting the vehicle package. The size of the vehicle package to be transferred to VUCM must be provided. VUCM will generate a transfer ID for subsequent calls
    /// TransferData、TransferXit、ProcessSwPackage and DeleteTransfer.
    /// @param size The size of the vehicle package to be transferred (bytes).
    /// @return   Returns a future of a vehicle package transfer information handle or a specific error code (please refer to ara::core::Future usage instructions)
    /// @throws no
    /// @code{.isoft}
    /// @uptrace={SWS_UCM_01006}
    ///
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00460
    /// @trace_id_dd=DD_UCM_Master_00914
    /// @trace_id_sr=SR_UCM_00035,SR_UCM_00039
    /// @needwork = ad
    /// @endcode
    ara::core::Future< TransferVehiclePackageOutput > TransferVehiclePackage(std::uint64_t const& size) override;

    /// @brief Start the transfer of a software package. The name of the software package to be transferred to VUCM must be provided. VUCM
    /// will generate a Transfer ID for subsequent calls to TransferData, TransferXit, and DeleteTransfer.
    /// The size of the software package for transfer to UCM subordinates is provided in the vehicle package and its contained software package manifest.
    /// @param softwarePackageName The short name of the software package to be transferred.
    /// @return   Returns a future of a software package transfer information handle or a specific error code (please refer to ara::core::Future usage instructions)
    /// @throws no
    /// @code{.isoft}
    /// @uptrace={SWS_UCM_01007}
    ///
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00461
    /// @trace_id_dd=DD_UCM_Master_00915
    /// @needwork = ad
    /// @endcode
    ara::core::Future< TransferStartOutput > TransferStart(pkgmgr::SwNameType const& softwarePackageName) override;

    /// @brief Block transfer interface for software package or vehicle package to VUCM
    /// @param id Transfer handle
    /// @param data Data block of the software package or vehicle package
    /// @param blockCounter Block counter value of the current block
    /// @return ara::core::Future<void> Returns a future or a specific error code (please refer to ara::core::Future usage instructions)
    /// @throws no
    /// @code{.isoft}
    /// @uptrace={SWS_UCM_01008}
    ///
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00462
    /// @trace_id_dd=DD_UCM_Master_00916
    /// @needwork = ad
    /// @endcode
    ara::core::Future< void > TransferData(pkgmgr::TransferIdType const& id,
                                           pkgmgr::ByteVectorType const& data,
                                           std::uint64_t const& blockCounter) override;

    /// @brief Transfer of software package or vehicle package to VUCM is complete.
    /// @param id Transfer handle
    /// @return Returns a void future or a specific error code (please refer to ara::core::Future usage instructions)
    /// @throws no
    /// @code{.isoft}
    /// @uptrace={SWS_UCM_01009}
    ///
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00463
    /// @trace_id_dd=DD_UCM_Master_00917
    /// @needwork = ad
    /// @endcode
    ara::core::Future< void > TransferExit(pkgmgr::TransferIdType const& id) override;

    /// @brief Delete the transferred software package or vehicle package
    /// @param id Transfer handle id of the currently running request.
    /// @return Returns a void future or a specific error code (please refer to ara::core::Future usage instructions)
    /// @throws no
    /// @code{.isoft}
    /// @uptrace={SWS_UCM_01010}
    ///
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00464
    /// @trace_id_dd=DD_UCM_Master_00918
    /// @needwork = ad
    /// @endcode
    ara::core::Future< void > DeleteTransfer(pkgmgr::TransferIdType const& id) override;

    /// @brief UpdateState
    /// @param state
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00465
    /// @trace_id_dd=DD_UCM_Master_00919
    /// @trace_id_sr=SR_UCM_00035,SR_UCM_00042
    /// @needwork = ad
    /// @endcode
    void UpdateState(pkgmgr::TransferStateType const state);

private:
    /// @brief _getNotifier
    /// @return future
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00920
    /// @trace_id_sr=SR_UCM_00035,SR_UCM_00042
    /// @needwork = dda
    /// @endcode
    ara::core::Future< pkgmgr::TransferStateType > _getNotifier() const noexcept;

private:
    /// @brief state_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00921
    /// @trace_id_sr=SR_UCM_00035,SR_UCM_00042
    /// @needwork = dda
    /// @endcode
    pkgmgr::TransferStateType state_;
};

}  // namespace vpkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // ARA_UCM_VPKGMGR_VEHICLE_PACKAGE_MANAGER_IMPL_H_
