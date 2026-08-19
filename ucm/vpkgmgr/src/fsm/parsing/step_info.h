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
/// @file       step_info.h
/// @brief      StepInfo header
/// @details
/// @date       2024-07-20
/// @author     hanzhibo
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// export_level=/
/// @module_path=/UCM Master/Fsm
/// @interface_level=unit
/// @trace_id_sr=SR_UCM_00035
/// @unit_name=StepInfo
/// @unit_description=StepInfo header
/// @endcode
///
/// ================================================================

#ifndef ARA_UCM_VPKGMGR_PARSERING_CORE_STRUCT_H_
#define ARA_UCM_VPKGMGR_PARSERING_CORE_STRUCT_H_

#include <ara/core/string.h>
#include <ara/core/vector.h>
#include <isoft/manifestreader/tps_enumeration.h>

#include <cstdint>
#include <memory>

#include "utils/alias.h"

namespace ara {
namespace ucm {
namespace vpkgmgr {

/// @brief SoftwarePackageStoringEnum
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
using SoftwarePackageStoringEnum = isoft::manifestreader::tps::SoftwarePackageStoringEnum;

/// @brief TransferStepInfo
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00482
/// @trace_id_dd=DD_UCM_Master_00311
/// @needwork = ad
/// @endcode
struct TransferStepInfo
{
    /// @brief softwarePackageRef
    ara::core::String softwarePackageRef;
};
/// @brief TransferStepInfoPtr
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
using TransferStepInfoPtr = std::shared_ptr< TransferStepInfo >;

/// @brief TransferInfo
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00482
/// @trace_id_dd=DD_UCM_Master_00312
/// @needwork = dd
/// @endcode
struct TransferInfo
{
    /// @brief storing
    SoftwarePackageStoringEnum storing;
    /// @brief transferInfos
    ara::core::Vector< TransferStepInfoPtr > transferInfos;
};
/// @brief TransferInfoPtr
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
using TransferInfoPtr = std::shared_ptr< TransferInfo >;

/// @brief ProcessStepInfo
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00482
/// @trace_id_dd=DD_UCM_Master_00313
/// @needwork = dd
/// @endcode
struct ProcessStepInfo
{
    /// @brief softwarePackageRef
    ara::core::String softwarePackageRef;
    /// @brief ProcessStepInfo
};
/// @brief ProcessStepInfoPtr
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
using ProcessStepInfoPtr = std::shared_ptr< ProcessStepInfo >;

/// @brief SwpStepInfo
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00482
/// @trace_id_dd=DD_UCM_Master_00314
/// @needwork = dd
/// @endcode
struct SwpStepInfo
{
    /// @brief shortName
    ara::core::String shortName;
    /// @brief safetyPolicy
    ///ara::core::String safetyPolicy;.
    /// @brief ucmId
    ara::core::String ucmId;

    /// @brief transferStepPtr
    AraList< TransferInfoPtr > transferStepPtrList;
    /// @brief processStepPtr
    ProcessStepInfoPtr processStepPtr;
    /// @brief activateStep
    bool activateStep{false};
    /// @brief SwpStepInfo
};
/// @brief SwpStepInfoPtr
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
using SwpStepInfoPtr = std::shared_ptr< SwpStepInfo >;

/// @brief UcmStepInfo
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00482
/// @trace_id_dd=DD_UCM_Master_00315
/// @needwork = dd
/// @endcode
struct UcmStepInfo
{
    /// @brief shortName
    ara::core::String shortName;
    /// @brief ucmId
    ara::core::String ucmId;
    /// @brief safetyPolicy
    ///ara::core::String safetyPolicy;.
    /// @brief swpInfos
    ara::core::Vector< SwpStepInfoPtr > swpInfos;
    /// @brief UcmStepInfo
};
/// @brief UcmStepInfoPtr
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
using UcmStepInfoPtr = std::shared_ptr< UcmStepInfo >;

/// @brief RolloutStepInfo
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00482
/// @trace_id_dd=DD_UCM_Master_00316
/// @needwork = dd
/// @endcode
struct RolloutStepInfo
{
    /// @brief shortName
    ara::core::String shortName;
    /// @brief safetyPolicy
    ara::core::String safetyPolicy;
    /// @brief ucmInfos
    ara::core::Vector< UcmStepInfoPtr > ucmInfos;
    /// @brief RolloutStepInfo
};
/// @brief RolloutStepInfoPtr
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
using RolloutStepInfoPtr = std::shared_ptr< RolloutStepInfo >;

}  // namespace vpkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // ARA_UCM_VPKGMGR_PARSERING_CORE_STRUCT_H_