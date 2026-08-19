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
/// @file       function_group_state.cpp
/// @brief      Function group state class implementation
/// @details
/// @date       2022-01-01
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/ExecutionManagement/StateClient
/// @unit_name=FunctionGroupState
/// @unit_description=Used to manage the state of a function group.
/// @interface_level=software
/// @endcode
///
/// ================================================================

#include "ara/exec/function_group_state.h"

#include <isoft/ara_fsh/filesystem_hierarchy.h>

#include "ara/exec/exec_error_domain.h"
#include "ara/exec/internal/config/function_group_set.h"
#include "ara/exec/internal/config/software_cluster_manifest.h"

namespace ara {
namespace exec {

/// @brief Pre construction method for FunctionGroupState.
/// This method shall validate/verify meta-model path passed and perform any operation that could
/// fail and are expected to be performed in constructor.
/// @param functionGroup the Function Group instance the state shall be connected with.
/// @param metaModelIdentifier stringified meta model identifier (short name path) where path separator is '/'.
/// @return a construction token from which an instance of FunctionGroupState can be constructed,
///         or ExecErrorDomain error.
///         ExecErrc::kMetaModelError
///             if metaModelIdentifier passed is incorrect (e.g. FunctionGroup identifier has been passed).
///         ExecErrc::kGeneralError if any other error occurs.
/// @code{.isoft}
/// @interface_level=software
/// @trace_id_sws=SWS_EM_02270, 0cfd69c041c6a69c38a0e056bac71b61ce56adbc
/// @trace_id_ad=AD_EM_00037
/// @trace_id_dd=DD_EM_00747
/// @needwork = dda
/// @endcode
ara::core::Result< FunctionGroupState::CtorToken > FunctionGroupState::Preconstruct(
    FunctionGroup const& functionGroup, ara::core::StringView const metaModelIdentifier) noexcept
{
    int32_t r{1};
    static std::shared_ptr< ara::exec::internal::config::FunctionGroupSet > s_FgSet{
        internal::config::FunctionGroupSet::CreateInstance()};
    if (s_FgSet) {
        for (auto const& fg : s_FgSet->GetFunctionGroups()) {  // PRQA S 2961
            if (functionGroup.GetMetaModelIdentifier() != fg.GetName()) {
                continue;
            }
            for (auto const& state : fg.GetStates()) {  // PRQA S 2961
                if (state == metaModelIdentifier) {
                    r = 0;
                    break;
                }
            }
        }
    } else {
        r = -1;
    }

    if (r == 0) {
        return ara::core::Result< FunctionGroupState::CtorToken >::FromValue(
            CtorToken(functionGroup.GetMetaModelIdentifier(), metaModelIdentifier));
    }
    if (r == 1) {
        return ara::core::Result< FunctionGroupState::CtorToken >::FromError(
            ara::exec::MakeErrorCode(ara::exec::ExecErrc::kMetaModelError, ara::core::ErrorDomain::SupportDataType{0}));
    }
    return ara::core::Result< FunctionGroupState::CtorToken >::FromError(
        ara::exec::MakeErrorCode(ara::exec::ExecErrc::kGeneralError, ara::core::ErrorDomain::SupportDataType{0}));
}

/// @brief Constructor that creates FunctionGroupState instance.
/// @param token representing pre-constructed object.
/// @code{.isoft}
/// @interface_level=software
/// @trace_id_sws=SWS_EM_02271, d973595e55a8642968c7b0b0f4555137146ca929
/// @trace_id_ad=AD_EM_00037
/// @trace_id_dd=DD_EM_00748
/// @needwork = dda
/// @endcode
FunctionGroupState::FunctionGroupState(FunctionGroupState::CtorToken&& token) noexcept : token_{std::move(token)} {}

/// @brief Constructor that creates a new instance.
/// @param pathToFunctionGroup ARXML short-name path to a Machine.functionGroup of ModeDeclarationGroupPrototype.
/// @param pathToModeDeclaration ARXML short-name path to a ModeDeclarationGroup.modeDeclaration of ModeDeclaration.
/// @code{.isoft}
/// @interface_level=software
/// @trace_id_ad=AD_EM_00037
/// @trace_id_dd=DD_EM_00741
/// @needwork = dda
/// @endcode
FunctionGroupState::CtorToken::CtorToken(core::StringView const pathToFunctionGroup,
                                         core::StringView const pathToModeDeclaration) noexcept
    : pathToFunctionGroup_{pathToFunctionGroup}, pathToModeDeclaration_{pathToModeDeclaration}
{
}

/// @brief Returns the path to a  Machine.functionGroup of ModeDeclarationGroupPrototype.
/// @return ARXML short-name path.
/// @code{.isoft}
/// @interface_level=software
/// @trace_id_ad=AD_EM_00037
/// @trace_id_dd=DD_EM_00742
/// @needwork = dda
/// @endcode
core::StringView FunctionGroupState::CtorToken::GetPathToFunctionGroup() const noexcept { return pathToFunctionGroup_; }

/// @brief Returns the path to a ModeDeclarationGroup.modeDeclaration of ModeDeclaration.
/// @return ARXML short-name path.
/// @code{.isoft}
/// @interface_level=software
/// @trace_id_ad=AD_EM_00037
/// @trace_id_dd=DD_EM_00743
/// @needwork = dda
/// @endcode
core::StringView FunctionGroupState::CtorToken::GetPathToModeDeclaration() const noexcept
{
    return pathToModeDeclaration_;
}

}  // namespace exec
}  // namespace ara
