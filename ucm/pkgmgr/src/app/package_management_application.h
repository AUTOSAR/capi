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
/// @file       package_management_application.h
/// @brief      Package management application
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
/// @interface_level=unit
/// @trace_id_sr=SR_UCM_00001
/// @unit_name=PackageManagementApplication
/// @unit_description=Package management application
/// @endcode
///
/// ================================================================

#ifndef ARA_UCM_PKGMGR_APP_PACKAGE_MANAGEMENT_APPLICATION_H_
#define ARA_UCM_PKGMGR_APP_PACKAGE_MANAGEMENT_APPLICATION_H_

#include <memory>

#include "adaptive_application.h"
#include "ara/ucm/pkgmgr/packagemanagement_skeleton.h"
#include "common/alias.h"
#include "sm/sm_service.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief This class represents
/// Package Management Application
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10002
/// @trace_id_dd=DD_UCM_10018
/// @needwork = ad
/// @endcode
class PackageManagementApplication : public AdaptiveApplication
{
public:
    /// @brief constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10019
    /// @needwork = dda
    /// @endcode
    PackageManagementApplication() noexcept {};  // NOLINT
    /// @brief destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10020
    /// @needwork = dda
    /// @endcode
    ~PackageManagementApplication() override = default;

    /// @brief delete copy construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10021
    /// @needwork = dda
    /// @endcode
    PackageManagementApplication(PackageManagementApplication const& other) = delete;
    /// @brief delete copy asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10022
    /// @needwork = dda
    /// @endcode
    PackageManagementApplication& operator=(PackageManagementApplication const& other) = delete;
    /// @brief delete move construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10023
    /// @needwork = dda
    /// @endcode
    PackageManagementApplication(PackageManagementApplication&& other) = delete;
    /// @brief delete move asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10024
    /// @needwork = dda
    /// @endcode
    PackageManagementApplication& operator=(PackageManagementApplication&& other) = delete;

protected:
    /// @brief init something here
    /// @return bool
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10025
    /// @needwork = dda
    /// @endcode
    bool OnInitialize() noexcept override;
    /// @brief run the app
    /// @return no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10026
    /// @needwork = dda
    /// @endcode
    void Run() noexcept override;
    /// @brief cleanup something here
    /// @return no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10027
    /// @needwork = dda
    /// @endcode
    void OnTerminate() noexcept override;

private:
    /// @brief parse the given manifest file
    /// @param machineManifestPath machine manifest path
    /// @throws no
    /// @return the value of the ucm ID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10028
    /// @needwork = dda
    /// @endcode
    static AraString GetUcmId(AraString const& machineManifestPath);

    /// @brief parse the value of maxWaitTimeWhenCallUpdateMethod
    /// @param executionManifestPath execution manifest path
    /// @throws no
    /// @return int
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10029
    /// @needwork = dda
    /// @endcode
    static std::int32_t GetMaxWaitTimeWhenCallUpdateMethod(AraString const& executionManifestPath);

    /// @brief open the kv storage
    /// @throws no
    /// @return bool
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10030
    /// @needwork = dda
    /// @endcode
    static bool OpenKVStorage();

    /// @brief callback after find sm service
    /// @throws no
    /// @return no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10031
    /// @needwork = dda
    /// @endcode
    void _onUpdateRequestAvailabeFromSM();

private:
    /// @brief Pointer to Package Management service skeleton
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10032
    /// @needwork = dda
    /// @endcode
    std::unique_ptr< ara::ucm::pkgmgr::skeleton::PackageManagementSkeleton > service_{nullptr};
    /// @brief Pointer to SM
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10033
    /// @needwork = dda
    /// @endcode
    std::unique_ptr< UpdateRequestService > smService_{nullptr};
};

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // ARA_UCM_PKGMGR_APP_PACKAGE_MANAGEMENT_APPLICATION_H_
