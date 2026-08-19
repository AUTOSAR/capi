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
/// @file       strtype.h
/// @brief      type convert to/from str
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
/// @unit_name=StrType
/// @unit_description=type convert to/from str
/// @endcode
///
/// ================================================================

#ifndef ARA_UCM_PKGMGR_COMMON_STR_TYPE_H_
#define ARA_UCM_PKGMGR_COMMON_STR_TYPE_H_

#include "alias.h"
#include "ara/ucm/internal/types/impl_type_transferidtype.h"
#include "ara/ucm/pkgmgr/impl_type_actiontype.h"
#include "ara/ucm/pkgmgr/impl_type_activateoptiontype.h"
#include "ara/ucm/pkgmgr/impl_type_packagemanagerstatustype.h"
#include "ara/ucm/pkgmgr/impl_type_resulttype.h"
#include "ara/ucm/pkgmgr/impl_type_swpackagestatetype.h"
#include "executors/help_executor.h"
#include "types/impl_type_finalactiontype.h"
#include "types/software_cluster_dependency.h"

namespace ara {
namespace ucm {
namespace pkgmgr {
namespace strtype {

/// @brief ActionTypeToStr
/// @param type
/// @throws no
/// @return str
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10409
/// @trace_id_dd=DD_UCM_11128
/// @needwork = dd
/// @endcode
AraString ActionTypeToStr(ActionType const type);
/// @brief ActionTypeFromStr
/// @param stype
/// @throws no
/// @return ActionType
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10409
/// @trace_id_dd=DD_UCM_11129
/// @needwork = dd
/// @endcode
ActionType ActionTypeFromStr(AraString const& stype);

/// @brief ResultTypeToStr
/// @param type
/// @throws no
/// @return str
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10410
/// @trace_id_dd=DD_UCM_11130
/// @needwork = dd
/// @endcode
AraString ResultTypeToStr(ResultType const type);
/// @brief ResultTypeFromStr
/// @param stype
/// @throws no
/// @return ResultType
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10410
/// @trace_id_dd=DD_UCM_11131
/// @needwork = dd
/// @endcode
ResultType ResultTypeFromStr(AraString const& stype);

/// @brief ActivateOptionTypeToStr
/// @param type
/// @throws no
/// @return str
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10411
/// @trace_id_dd=DD_UCM_11132
/// @needwork = dd
/// @endcode
AraString ActivateOptionTypeToStr(ActivateOptionType const type);

/// @brief FinalActionTypeToStr
/// @param type
/// @throws no
/// @return str
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10412
/// @trace_id_dd=DD_UCM_11133
/// @needwork = dd
/// @endcode
AraString FinalActionTypeToStr(FinalActionType const type);

/// @brief PackageManagerStatusTypeToStr
/// @param type
/// @throws no
/// @return str
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10413
/// @trace_id_dd=DD_UCM_11134
/// @needwork = dd
/// @endcode
AraString PackageManagerStatusTypeToStr(PackageManagerStatusType const type);

/// @brief SwPackageStateTypeToStr
/// @param type
/// @throws no
/// @return str
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10414
/// @trace_id_dd=DD_UCM_11135
/// @needwork = dd
/// @endcode
AraString SwPackageStateTypeToStr(SwPackageStateType const type);

/// @brief SoftwareClusterDependencyCategoryEnumToStr
/// @param type
/// @throws no
/// @return str
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10415
/// @trace_id_dd=DD_UCM_11136
/// @needwork = dd
/// @endcode
AraString SoftwareClusterDependencyCategoryEnumToStr(SoftwareClusterDependencyCategoryEnum const type);

/// @brief SoftwareClusterDependencyLogicalOperatorEnumToStr
/// @param type
/// @throws no
/// @return str
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10415
/// @trace_id_dd=DD_UCM_11137
/// @needwork = dd
/// @endcode
AraString SoftwareClusterDependencyLogicalOperatorEnumToStr(SoftwareClusterDependencyLogicalOperatorEnum const type);

/// @brief SoftwareClusterDependencyOperatorEnumToStr
/// @param type
/// @throws no
/// @return str
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10415
/// @trace_id_dd=DD_UCM_11138
/// @needwork = dd
/// @endcode
AraString SoftwareClusterDependencyOperatorEnumToStr(SoftwareClusterDependencyOperatorEnum const type);

/// @brief HelpExecutorOSUpdateTypeToStr
/// @param type
/// @throws no
/// @return str
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10416
/// @trace_id_dd=DD_UCM_11139
/// @needwork = dd
/// @endcode
AraString HelpExecutorOSUpdateTypeToStr(HelpExecutor::OSUpdateType const type);

/// @brief HelpExecutorSwclUpdateTypeToStr
/// @param type
/// @throws no
/// @return str
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10416
/// @trace_id_dd=DD_UCM_11140
/// @needwork = dd
/// @endcode
AraString HelpExecutorSwclUpdateTypeToStr(HelpExecutor::SwclUpdateType const type);

/// @brief convert TransferId to hex str
/// @param id TransferIdType
/// @throws no
/// @return str
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10417
/// @trace_id_dd=DD_UCM_11141
/// @needwork = dd
/// @endcode
AraString TransferIdTypeToHexStr(TransferIdType const& id);
/// @brief get TransferId from hex str
/// @param hex
/// @throws no
/// @return TransferIdType
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10417
/// @trace_id_dd=DD_UCM_11142
/// @needwork = dd
/// @endcode
TransferIdType TransferIdTypeFromHexStr(AraString const& hex);

}  // namespace strtype
}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // ARA_UCM_PKGMGR_COMMON_STR_TYPE_H_
