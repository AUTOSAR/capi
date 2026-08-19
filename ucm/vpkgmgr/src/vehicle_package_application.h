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
/// @file       vehicle_package_application.h
/// @brief      Completes program initialization and shutdown, creates other modules
/// @details
/// @date       2024-07-20
/// @author     hanzhibo
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// export_level=/
/// @module_path=/UCM Master/Main
/// @interface_level=unit
/// @trace_id_sr=SR_UCM_00035
/// @unit_name=VehiclePackageManagementApplication
/// @unit_description=Completes program initialization and shutdown, creates other modules
/// @endcode
///
/// ================================================================

#ifndef ARA_UCM_VPKGMGR_VEHICLE_PACKAGE_MANAGER_APPLICATION_H_
#define ARA_UCM_VPKGMGR_VEHICLE_PACKAGE_MANAGER_APPLICATION_H_

#include <ara/exec/execution_client.h>
#include <ara/log/log_stream.h>
#include <ara/log/logger.h>

#include <atomic>
#include <memory>

#include "services/vehicle_driver_application/vehicle_driver_interface_service.h"
#include "services/vehicle_package_management/vehicle_package_manager_service.h"
#include "services/vehicle_state_manager/vehicle_state_manager_service.h"

namespace ara {
namespace ucm {
namespace vpkgmgr {

/// @brief This class represents
/// Vehicle Package Management Application
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00001
/// @trace_id_dd=DD_UCM_Master_00001
/// @needwork = ad
/// @endcode
class VehiclePackageManagementApplication
{
public:
    /// @brief constructor
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00002
    /// @trace_id_dd=DD_UCM_Master_00002
    /// @needwork = ad
    /// @endcode
    VehiclePackageManagementApplication() = default;
    ///~VehiclePackageManagementApplication();

    /// @brief Performs at initialize step actions.
    /// @return Initialize success status (Was it successful or not)
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00003
    /// @trace_id_dd=DD_UCM_Master_00003
    /// @needwork = ad
    /// @endcode
    bool Initialize();
    /// @brief Uninitialize
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00004
    /// @trace_id_dd=DD_UCM_Master_00004
    /// @needwork = ad
    /// @endcode
    void Uninitialize();

    /// @brief A method to be executed at runtime.
    /// Shall be reimplemented for the specific application
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00005
    /// @trace_id_dd=DD_UCM_Master_00005
    /// @needwork = ad
    /// @endcode
    void Run() const;
    /// @brief Stop
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00006
    /// @trace_id_dd=DD_UCM_Master_00006
    /// @needwork = ad
    /// @endcode
    void Stop() const noexcept;

    /// @brief InitServices
    /// @param state
    /// @return bool
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00007
    /// @trace_id_dd=DD_UCM_Master_00007
    /// @needwork = ad
    /// @endcode
    bool InitServices(pkgmgr::CampaignStateType const& state);

private:
    /// @brief Launch the update campaign based on Vehicle Package Manifest content
    ///bool IsNeedContinueLastUpdate();

    /// @brief begin update campaign
    ///bool StartUpdateCampaign();

    /// @brief Report to Execution Manager that the App reached its running state
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00008
    /// @needwork = dda
    /// @endcode
    static void ReportRunningState() noexcept;

    /// @brief _toTransferState
    /// @param campaignState
    /// @return TransferStateType
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00009
    /// @needwork = dda
    /// @endcode
    pkgmgr::TransferStateType _toTransferState(pkgmgr::CampaignStateType const campaignState) const noexcept;

private:
    /// @brief _vehiclePackageManagerInit
    /// @param state
    /// @return bool
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00010
    /// @needwork = dda
    /// @endcode
    bool _vehiclePackageManagerInit(pkgmgr::TransferStateType const& state);
    /// @brief _vehicleDriverInterfaceServiceInit
    /// @param state
    /// @return bool
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00011
    /// @needwork = dda
    /// @endcode
    bool _vehicleDriverInterfaceServiceInit(pkgmgr::CampaignStateType const& state);
    /// @brief _vehicleStateManagerServiceInit
    /// @return bool
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00012
    /// @needwork = dda
    /// @endcode
    bool _vehicleStateManagerServiceInit();

private:
    /// @brief Pointer to Vehicle Package Management service skeleton
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00013
    /// @needwork = dda
    /// @endcode
    VehiclePackageManagerService::Ptr vpmService_;
    /// @brief Poin
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00014
    /// @needwork = dda
    /// @endcode
    VehicleDriverInterfaceService::Ptr driverInterfaceService_;
    /// @brief Poin
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00015
    /// @needwork = dda
    /// @endcode
    VehicleStateManagerService::Ptr stateManagerService_;

    /// @brief Logger for logging errors
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00016
    /// @needwork = dda
    /// @endcode
    ara::log::Logger& log_{
        ara::log::CreateLogger(std::move(ara::core::StringView("#UCM")),
                               std::move(ara::core::StringView("VehiclePackageManagementApplication context")),
                               ara::log::LogLevel::kVerbose)};
};

}  // namespace vpkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // ARA_UCM_VPKGMGR_VEHICLE_PACKAGE_MANAGER_APPLICATION_H_
