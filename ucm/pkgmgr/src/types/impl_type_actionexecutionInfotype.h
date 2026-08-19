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
/// @file       impl_type_actionexecutionInfotype.h
/// @brief      Definition of the action execution information type
/// @details
/// @date       2024-08-15
/// @author     cuiyinli
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @export_level=/UCM/Utils
/// @module_path=/UCM/Utils
/// @interface_level=unit
/// @trace_id_sr=SR_UCM_00001
/// @unit_name=ActionExecutionInfoType
/// @unit_description=Definition of the action execution information type
/// @endcode
///
/// ================================================================

#ifndef ARA_UCM_PKGMGR_IMPL_TYPE_ACTIONEXECUTIONINFOTYPE_H_
#define ARA_UCM_PKGMGR_IMPL_TYPE_ACTIONEXECUTIONINFOTYPE_H_

#include <cstdint>

#include "ara/ucm/pkgmgr/impl_type_activateoptiontype.h"
#include "ara/ucm/pkgmgr/impl_type_resulttype.h"
#include "ara/ucm/pkgmgr/impl_type_transferidtype.h"
#include "common/alias.h"
#include "common/const.h"
#include "common/rjson_manifest.h"
#include "common/strtype.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief ActionExecutionInfoType
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10317
/// @trace_id_dd=DD_UCM_10686
/// @needwork = ad
/// @endcode
class ActionExecutionInfoType
{
public:
    /// @brief actionType
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10687
    /// @needwork = dda
    /// @endcode
    ActionType actionType;
    /// @brief activateOptionType
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10688
    /// @needwork = dda
    /// @endcode
    ActivateOptionType activateOptionType;
    /// @brief deltaPackageApplicableVersion
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10689
    /// @needwork = dda
    /// @endcode
    AraString deltaPackageApplicableVersion;
    /// @brief swclName
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10690
    /// @needwork = dda
    /// @endcode
    AraString swclName;
    /// @brief curSWCLVersion
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10691
    /// @needwork = dda
    /// @endcode
    AraString curSWCLVersion;
    /// @brief swclVersion_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10692
    /// @needwork = dda
    /// @endcode
    AraString swclVersion;
    /// @brief Time
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10693
    /// @needwork = dda
    /// @endcode
    std::uint64_t time;
    /// @brief Resolution
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10694
    /// @needwork = dda
    /// @endcode
    ResultType resolution;

    /// @brief ManifestLoader
    /// @param node
    /// @return result
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10695
    /// @needwork = dda
    /// @endcode
    std::int32_t ManifestLoader(RManifestNode const &node)
    {
        *reinterpret_cast< uint8_t * >(&(this->actionType))
            = static_cast< uint8_t >(RManifestNodeGetI(node, kUCMProcessedSoftwarePackageActionType, 0));

        *reinterpret_cast< uint8_t * >(&(this->activateOptionType))
            = static_cast< uint8_t >(RManifestNodeGetI(node, kUCMProcessedSoftwarePackageActivateOptionType, 0));

        deltaPackageApplicableVersion
            = RManifestNodeGetSS(node, kUCMProcessedSoftwarePackagedeltaPackageApplicableVersion);
        swclName       = RManifestNodeGetSS(node, kUCMProcessedSoftwarePackageSwclName);
        curSWCLVersion = RManifestNodeGetSS(node, kUCMProcessedSoftwarePackageCurSwclVersion);
        swclVersion    = RManifestNodeGetSS(node, kUCMProcessedSoftwarePackageSwclVersion);

        this->time = RManifestNodeGetUI64(node, kUCMProcessedSoftwarePackageActionTimeStamp, 0U);
        *reinterpret_cast< uint8_t * >(&(this->resolution))
            = static_cast< uint8_t >(RManifestNodeGetI(node, kUCMProcessedSoftwarePackageActionResolution, 0));

        return isoft::kSuccess;
    }

    /// @brief convert ActionExecutionInfoType to string
    /// @return string
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10696
    /// @needwork = dda
    /// @endcode
    inline AraString Str() const
    {
        return "ActionExecutionInfoType{actionType:" + strtype::ActionTypeToStr(actionType)
               + " activateOptionType:" + strtype::ActivateOptionTypeToStr(activateOptionType)
               + " deltaPackageApplicableVersion:" + deltaPackageApplicableVersion + " swclName:" + swclName
               + " curVer_:" + curSWCLVersion + " swclVer_:" + swclVersion + " time:" + std::to_string(time)
               + " resolution:" + strtype::ResultTypeToStr(resolution) + "}";
    }
};
}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // ARA_UCM_PKGMGR_IMPL_TYPE_ACTIONEXECUTIONINFOTYPE_H_
