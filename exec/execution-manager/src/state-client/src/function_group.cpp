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
/// @file       function_group.cpp
/// @brief      Function group class implementation
/// @details
/// @date       2022-01-01
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/ExecutionManagement/StateClient
/// @unit_name=FunctionGroup
/// @unit_description=Used to manage a group of processes.
/// @interface_level=software
/// @endcode
///
/// ================================================================

#include "ara/exec/function_group.h"

#include <isoft/ara_fsh/filesystem_hierarchy.h>

#include "ara/exec/exec_error_domain.h"
#include "ara/exec/internal/config/function_group_set.h"
#include "ara/exec/internal/config/software_cluster_manifest.h"

namespace ara {
namespace exec {

/// @brief Pre construction method for FunctionGroup.
/// This method shall validate/verify meta-model path passed and perform any operation that could
/// fail and are expected to be performed in constructor.
/// @param metaModelIdentifier stringified meta model identifier (short name path) where path separator is '/'.
/// @return a construction token from which an instance of FunctionGroup can be constructed,
///          or ExecErrorDomain error.
///         ExecErrc::kMetaModelError
///             if metaModelIdentifier passed is incorrect (e.g. FunctionGroup identifier has been passed).
///         ExecErrc::kGeneralError if any other error occurs.
/// @code{.isoft}
/// @interface_level=software
/// @trace_id_sws=SWS_EM_02264, 254ccea456581590f28bfb8e8d5d05f068f85462
/// @trace_id_ad=AD_EM_00036
/// @trace_id_dd=DD_EM_00728
/// @needwork = dda
/// @endcode
ara::core::Result< FunctionGroup::CtorToken > FunctionGroup::Preconstruct(
    ara::core::StringView const metaModelIdentifier) noexcept
{
    static std::shared_ptr< internal::config::FunctionGroupSet > s_FgSet{
        internal::config::FunctionGroupSet::CreateInstance()};
    int32_t r{1};
    if (s_FgSet) {
        for (auto const& fg : s_FgSet->GetFunctionGroups()) {  // PRQA S 2961
            if (metaModelIdentifier == fg.GetName()) {
                r = 0;
            }
        }
    } else {
        r = -1;
    }

    if (r == 0) {
        return ara::core::Result< FunctionGroup::CtorToken >::FromValue(CtorToken(metaModelIdentifier));
    }
    if (r == 1) {
        return ara::core::Result< FunctionGroup::CtorToken >::FromError(
            ara::exec::MakeErrorCode(ara::exec::ExecErrc::kMetaModelError, ara::core::ErrorDomain::SupportDataType{0}));
    }
    return ara::core::Result< FunctionGroup::CtorToken >::FromError(
        ara::exec::MakeErrorCode(ara::exec::ExecErrc::kGeneralError, ara::core::ErrorDomain::SupportDataType{0}));
}

/// @brief Constructor that creates FunctionGroup instance.
/// @param token representing pre-constructed object.
/// @code{.isoft}
/// @interface_level=software
/// @trace_id_sws=SWS_EM_02265, 45c52a0e0f8eee90eac6fc743e4e9f63d3e81b39
/// @trace_id_ad=AD_EM_00036
/// @trace_id_dd=DD_EM_00729
/// @needwork = dda
/// @endcode
FunctionGroup::FunctionGroup(FunctionGroup::CtorToken&& token) noexcept : token_{std::move(token)} {}

/// @brief Constructor that creates FunctionGroup instance.
/// @param pathToFunctionGroup shortName of the represented ModeDeclarationGroupPrototype.
/// @code{.isoft}
/// @interface_level=software
/// @trace_id_ad=AD_EM_00036
/// @trace_id_dd=DD_EM_00724
/// @needwork = dda
/// @endcode
FunctionGroup::CtorToken::CtorToken(core::StringView const pathToFunctionGroup) noexcept
    : pathToFunctionGroup_{pathToFunctionGroup}
{
}

/// @brief Returns shortName of the ModeDeclarationGroupPrototype.
/// @return ARXML shortName.
/// @code{.isoft}
/// @interface_level=software
/// @trace_id_ad=AD_EM_00036
/// @trace_id_dd=DD_EM_00725
/// @needwork = dda
/// @endcode
ara::core::StringView FunctionGroup::CtorToken::GetPathToFunctionGroup() const noexcept { return pathToFunctionGroup_; }

/// @brief Getter for the stored meta-model identifier
/// @return StringView value for the stored meta-model identifier
/// @code{.isoft}
/// @interface_level=software
/// @trace_id_ad=AD_EM_00036
/// @trace_id_dd=DD_EM_00737
/// @needwork = dda
/// @endcode
core::StringView FunctionGroup::GetMetaModelIdentifier() const noexcept { return token_.GetPathToFunctionGroup(); }

}  // namespace exec
}  // namespace ara
