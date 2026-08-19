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
/// @file       vehicle_driver_interface_service.h
/// @brief      All service interfaces provided by the UCM host to the vehicle driver adaptive application.
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
/// @trace_id_sr=SR_UCM_00035,SR_UCM_00042,SR_UCM_00038
/// @unit_name=VehicleDriverInterfaceService
/// @unit_description=All service interfaces provided by the UCM host to the vehicle driver adaptive application.
/// @endcode
///
/// ================================================================

#ifndef ARA_UCM_VPKGMGR_VEHICLE_DRIVER_INTERFACE_H_
#define ARA_UCM_VPKGMGR_VEHICLE_DRIVER_INTERFACE_H_

#include <ara/core/future.h>
#include <ara/ucm/pkgmgr/error_domain_ucmerrordomain.h>

#include <algorithm>
#include <functional>
#include <memory>
#include <utility>

#include "ara/ucm/vdai/vehicledriverapplication_skeleton.h"
#include "fsm/fsm_manager.h"

namespace ara {
namespace ucm {
namespace vpkgmgr {

/// @brief All service interfaces provided by the UCM host to the vehicle driver adaptive application.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00466
/// @trace_id_dd=DD_UCM_Master_00922
/// @trace_id_sr=SR_UCM_00035,SR_UCM_00042
/// @needwork = ad
/// @endcode
class VehicleDriverInterfaceService : public vdai::skeleton::VehicleDriverApplicationSkeleton
{
public:
    /// @brief alias CBType
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_
    /// @trace_id_dd=DD_UCM_
    /// @needwork = no
    /// @endcode
    using CBType = std::function< void(bool const&, pkgmgr::SafetyPolicyType const&) >;
    /// @brief alias Ptr
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_
    /// @trace_id_dd=DD_UCM_
    /// @needwork = no
    /// @endcode
    using Ptr = std::shared_ptr< VehicleDriverInterfaceService >;

    /// @brief constructor
    /// @param instanceSpec
    /// @param mode
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00467
    /// @trace_id_dd=DD_UCM_Master_00923
    /// @needwork = ad
    /// @endcode
    VehicleDriverInterfaceService(ara::core::InstanceSpecifier instanceSpec,
                                  ara::com::MethodCallProcessingMode const mode);
    /// @brief constructor
    /// @param instanceId
    /// @param mode
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00468
    /// @trace_id_dd=DD_UCM_Master_00924
    /// @needwork = ad
    /// @endcode
    VehicleDriverInterfaceService(ara::com::InstanceIdentifier instanceId,
                                  ara::com::MethodCallProcessingMode const mode);

    /// @brief FieldInit
    /// @param state
    /// @return bool
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00469
    /// @trace_id_dd=DD_UCM_Master_00925
    /// @trace_id_sr=SR_UCM_00035,SR_UCM_00042
    /// @needwork = ad
    /// @endcode
    bool FieldInit(pkgmgr::CampaignStateType const& state);

    /// @brief SetApprovalCB
    /// @param cb
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00470
    /// @trace_id_dd=DD_UCM_Master_00926
    /// @needwork = ad
    /// @endcode
    void SetApprovalCB(CBType const& cb);

    /// @brief Called by the adaptive application (AA layer) to notify VUCM of the driver's notification resolution (approve or reject)
    /// @param approval Driver notification resolution
    /// @param safetyPolicy
    /// Safety policy calculated by the vehicle state manager adaptive application (AA side)
    /// @return Returns a void future, or a future of a specific error code (please refer to ara::core::Future usage instructions)
    /// @throws no
    ///
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00471
    /// @trace_id_dd=DD_UCM_Master_00927
    /// @trace_id_sr=SR_UCM_00035,SR_UCM_00038
    /// @needwork = ad
    /// @endcode
    ara::core::Future< void > DriverApproval(bool const& approval,
                                             pkgmgr::SafetyPolicyType const& safetyPolicy) override;

    /// @brief This method aborts the ongoing vehicle package processing activity.
    /// @param disableCampaign
    /// Whether to disable new update activities. false: only disable this update activity.
    /// true: also disable subsequent update activities
    /// @return Returns a void future, or a future of a specific error code (please refer to ara::core::Future usage instructions)
    /// @throws no
    ///
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00472
    /// @trace_id_dd=DD_UCM_Master_00928
    /// @trace_id_sr=SR_UCM_00035,SR_UCM_00038
    /// @needwork = ad
    /// @endcode
    ara::core::Future< void > CancelCampaign(bool const& disableCampaign) override;

    /// @brief Allow a new upgrade/update activity to start
    /// @return Returns a void future, or a future of a specific error code (please refer to ara::core::Future usage instructions)
    /// @throws no
    ///
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00473
    /// @trace_id_dd=DD_UCM_Master_00929
    /// @trace_id_sr=SR_UCM_00035,SR_UCM_00038
    /// @needwork = ad
    /// @endcode
    ara::core::Future< void > AllowCampaign() override;

    /// @brief Retrieve all operations executed by the UCM host
    /// @param timestampGE Earliest timestamp (inclusive)
    /// @param timestampLT Latest timestamp (exclusive)
    /// @return Returns a future of all operation records executed by the UCM host (please refer to ara::core::Future usage instructions) or a specific error code
    /// @throws no
    ///
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00474
    /// @trace_id_dd=DD_UCM_Master_00930
    /// @trace_id_sr=SR_UCM_00035,SR_UCM_00038
    /// @needwork = ad
    /// @endcode
    ara::core::Future< GetCampaignHistoryOutput > GetCampaignHistory(std::uint64_t const& timestampGE,
                                                                     std::uint64_t const& timestampLT) override;

    /// @brief List of general information of software clusters existing in the adaptive platform
    /// @return Returns a Future of the general software cluster information list or a specific error code (please refer to ara::core::Future usage instructions)
    /// @throws no
    ///
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00475
    /// @trace_id_dd=DD_UCM_Master_00931
    /// @trace_id_sr=SR_UCM_00035,SR_UCM_00038
    /// @needwork = ad
    /// @endcode
    ara::core::Future< GetSwClusterDescriptionOutput > GetSwClusterDescription() override;

    /// @brief List of general information of software packages that are part of the current active activity processed by VUCM
    /// @return Returns a Future of the general software package information list or a specific error code (please refer to ara::core::Future usage instructions)
    /// @throws no
    ///
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00476
    /// @trace_id_dd=DD_UCM_Master_00932
    /// @trace_id_sr=SR_UCM_00035,SR_UCM_00038
    /// @needwork = ad
    /// @endcode
    ara::core::Future< GetSwPackageDescriptionOutput > GetSwPackageDescription() override;

    /// @brief Get the current package processing progress (0-100%).
    /// @return Returns a Future of the current package upgrade/installation progress or a specific error code (please refer to ara::core::Future usage instructions)
    /// Current package processing progress (0%-100%). 0x00…0x64, 0xFF indicates no information available
    /// @throws no
    ///
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00477
    /// @trace_id_dd=DD_UCM_Master_00933
    /// @trace_id_sr=SR_UCM_00035,SR_UCM_00038
    /// @needwork = ad
    /// @endcode
    ara::core::Future< GetSwProcessProgressOutput > GetSwProcessProgress() override;

    /// @brief Get the current package transfer progress (0-100%).
    /// @return Returns a Future of the current package transfer progress or a specific error code (please refer to ara::core::Future usage instructions)
    /// @throws no
    ///
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00478
    /// @trace_id_dd=DD_UCM_Master_00934
    /// @trace_id_sr=SR_UCM_00035,SR_UCM_00038
    /// @needwork = ad
    /// @endcode
    ara::core::Future< GetSwTransferProgressOutput > GetSwTransferProgress() override;

private:
    /// @brief actionCB_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00935
    /// @needwork = dda
    /// @endcode
    CBType actionCB_;
};

}  // namespace vpkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // ARA_UCM_VPKGMGR_VEHICLE_DRIVER_INTERFACE_H_