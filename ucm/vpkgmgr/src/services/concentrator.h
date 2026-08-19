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
/// @file       concentrator.h
/// @brief      Implementation for requesting OTA Client to transfer software packages and requesting driver approval.
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
/// @trace_id_sr=SR_UCM_00035,SR_UCM_00042
/// @unit_name=Concentrator
/// @unit_description=Implementation for requesting OTA Client to transfer software packages and requesting driver approval.
/// @endcode
///
/// ================================================================

#ifndef VPKGMGR_SRC_FSM_CONCENTRATOR_H_
#define VPKGMGR_SRC_FSM_CONCENTRATOR_H_

#include <ara/core/map.h>
#include <ara/core/vector.h>
#include <ara/log/log_stream.h>
#include <ara/log/logger.h>

#include "fsm/fsm_manager.h"
#include "fsm/parsing/vehicle_package_parser.h"
#include "services/vehicle_driver_application/vehicle_driver_interface_service.h"
#include "services/vehicle_package_management/vehicle_package_manager_service.h"
#include "services/vehicle_state_manager/vehicle_state_manager_service.h"
#include "utils/single_ton.h"
namespace ara {
namespace ucm {
namespace vpkgmgr {

/// @brief Concentrator
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00429
/// @trace_id_dd=DD_UCM_Master_00877
/// @needwork = ad
/// @endcode
class Concentrator : public Singleton< Concentrator >
{
public:
    /// @brief  FIXME(yunfei)
    /// @brief constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00430
    /// @trace_id_dd=DD_UCM_Master_00878
    /// @needwork = ad
    /// @endcode
    Concentrator() = default;
    /// @brief destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00431
    /// @trace_id_dd=DD_UCM_Master_00879
    /// @needwork = ad
    /// @endcode
    ~Concentrator() = default;

    /// @brief delete copy construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00432
    /// @trace_id_dd=DD_UCM_Master_00880
    /// @needwork = ad
    /// @endcode
    Concentrator(Concentrator const& other) = delete;
    /// @brief delete copy asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00433
    /// @trace_id_dd=DD_UCM_Master_00881
    /// @needwork = ad
    /// @endcode
    Concentrator& operator=(Concentrator const& other) = delete;
    /// @brief delete move construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00434
    /// @trace_id_dd=DD_UCM_Master_00882
    /// @needwork = ad
    /// @endcode
    Concentrator(Concentrator&& other) = delete;
    /// @brief delete move asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00435
    /// @trace_id_dd=DD_UCM_Master_00883
    /// @needwork = ad
    /// @endcode
    Concentrator& operator=(Concentrator&& other) = delete;

    /// @brief LinkService
    /// @param vdi
    /// @param vsms
    /// @param vpms
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00436
    /// @trace_id_dd=DD_UCM_Master_00884
    /// @needwork = ad
    /// @endcode
    void LinkService(VehicleDriverInterfaceService::Ptr const& vdi,
                     VehicleStateManagerService::Ptr const& vsms,
                     VehiclePackageManagerService::Ptr const& vpms) noexcept;

    /// @brief ReportState
    /// @param campaignState
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00437
    /// @trace_id_dd=DD_UCM_Master_00885
    /// @trace_id_sr=SR_UCM_00035,SR_UCM_00042
    /// @needwork = ad
    /// @endcode
    void ReportState(pkgmgr::CampaignStateType const campaignState);
    /// @brief NotificationSwpkg
    /// @param swNameVersion
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00438
    /// @trace_id_dd=DD_UCM_Master_00886
    /// @needwork = ad
    /// @endcode
    void NotificationSwpkg(pkgmgr::SwNameVersionType const& swNameVersion) noexcept;

    ///inline VehicleDriverInterfaceService::Ptr GetVdiService() { return vdiService_; }
    ///inline VehicleStateManagerService::Ptr GetVsmService() { return vsmService_; }
    ///inline VehiclePackageManagerService::Ptr GetVpmsService() { return vpmsService_; }

    /// @brief UpdateDriverNotifications
    /// @param driverNotifications
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00439
    /// @trace_id_dd=DD_UCM_Master_00887
    /// @needwork = ad
    /// @endcode
    void UpdateDriverNotifications(ara::core::Map< ara::core::String, bool > const& driverNotifications);
    /// @brief UpdateSafetyCondition
    /// @param safetyCondition
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00440
    /// @trace_id_dd=DD_UCM_Master_00888
    /// @needwork = ad
    /// @endcode
    void UpdateSafetyCondition(pkgmgr::SafetyPolicyType const& safetyCondition);

    /// @brief TransferApproval
    /// @return bool
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00441
    /// @trace_id_dd=DD_UCM_Master_00889
    /// @needwork = ad
    /// @endcode
    bool TransferApproval();
    /// @brief ProcessApproval
    /// @return bool
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00442
    /// @trace_id_dd=DD_UCM_Master_00890
    /// @needwork = ad
    /// @endcode
    bool ProcessApproval();
    /// @brief ActivateApproval
    /// @return bool
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00443
    /// @trace_id_dd=DD_UCM_Master_00891
    /// @needwork = ad
    /// @endcode
    bool ActivateApproval();

private:
    /// @brief vdiService_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00892
    /// @needwork = dda
    /// @endcode
    VehicleDriverInterfaceService::Ptr vdiService_;
    /// @brief vsmService_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00893
    /// @needwork = dda
    /// @endcode
    VehicleStateManagerService::Ptr vsmService_;
    /// @brief vpmsService_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00894
    /// @needwork = dda
    /// @endcode
    VehiclePackageManagerService::Ptr vpmsService_;
    /// @brief swpkgTransfer_  ///SoftwarePackageTransfer::Ptr swpkgTransfer_;

    /// @brief safetyCondition_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00895
    /// @needwork = dda
    /// @endcode
    pkgmgr::SafetyPolicyType safetyCondition_;
    /// @brief Driver notification enable status for each execution state
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00896
    /// @needwork = dda
    /// @endcode
    ara::core::Map< ara::core::String, bool > driverNotifications_;
};

}  // namespace vpkgmgr
}  // namespace ucm
}  // namespace ara

#endif