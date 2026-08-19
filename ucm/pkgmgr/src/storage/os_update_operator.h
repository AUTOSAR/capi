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
/// @file       os_update_operator.h
/// @brief      OS update operator
/// @details
/// @date       2024-08-15
/// @author     cuiyinli
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @export_level=/UCM/SoftwareClusterManager
/// @module_path=/UCM/SoftwareClusterManager
/// @interface_level=unit
/// @trace_id_sr=SR_UCM_00001
/// @unit_name=OSUpdateOperator
/// @unit_description=OS update operator
/// @endcode
///
/// ================================================================

#ifndef ARA_UCM_PKGMGR_STORAGE_OS_UPDATE_OPERATOR_H_
#define ARA_UCM_PKGMGR_STORAGE_OS_UPDATE_OPERATOR_H_

#include "common/alias.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief The actual operator for OS update
///
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10270
/// @trace_id_dd=DD_UCM_10584
/// @needwork = ad
/// @endcode
class OSUpdateOperator
{
public:
    /// @brief constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10585
    /// @needwork = dda
    /// @endcode
    OSUpdateOperator() = delete;

    /// @brief constructor
    /// @param oSSwclDestinationPath
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10586
    /// @needwork = dda
    /// @endcode
    explicit OSUpdateOperator(AraString oSSwclDestinationPath);

    // Install to OS partition
    /// @brief PlaceToOSPatition
    /// @param deltaUpdate
    /// @return result
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10587
    /// @needwork = dda
    /// @endcode
    AraResultVoid PlaceToOSPatition(bool const deltaUpdate = false) const;

    // Invalidate the OS partition of the specified version
    /// @brief InvalidOSPatition
    /// @param oSVersion
    /// @return result
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10588
    /// @needwork = dda
    /// @endcode
    AraResultVoid InvalidOSPatition(AraString const& oSVersion) const;

    // Activate the OS partition of the specified version
    /// @brief ActiveOSPatition
    /// @param oSVersion
    /// @return result
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10589
    /// @needwork = dda
    /// @endcode
    AraResultVoid ActiveOSPatition(AraString const& oSVersion) const;

    // Verify that the current OS is the specified version
    /// @brief VerifyOSPatition
    /// @param oSVersion
    /// @return result
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10590
    /// @needwork = dda
    /// @endcode
    AraResultVoid VerifyOSPatition(AraString const& oSVersion) const;

private:
    /// @brief DoUpdateOSScript
    /// @param scriptPath
    /// @param scriptArg
    /// @return bool
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10591
    /// @needwork = dda
    /// @endcode
    static bool DoUpdateOSScript(AraString const& scriptPath, AraString const& scriptArg);

private:
    /// @brief updated OS swcl destination path
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10592
    /// @needwork = dda
    /// @endcode
    AraString dstPath_;
    /// @brief updated OS swcl script path
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10593
    /// @needwork = dda
    /// @endcode
    AraString scriptPath_;
};

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // ARA_UCM_PKGMGR_STORAGE_OS_UPDATE_OPERATOR_H_
