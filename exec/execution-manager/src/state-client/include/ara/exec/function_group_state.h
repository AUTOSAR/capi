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
/// @file       function_group_state.h
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

#ifndef ARA_EXEC_FUNCTION_GROUP_STATE_H_
#define ARA_EXEC_FUNCTION_GROUP_STATE_H_

#include <ara/core/result.h>
#include <ara/core/string.h>
#include <ara/core/string_view.h>

#include "ara/exec/function_group.h"

namespace ara {
namespace exec {

/// @brief Class representing Function Group State defined in the meta-model (ARXML).
/// @code{.isoft}
/// @interface_level=software
/// @trace_id_sws=SWS_EM_02269, 2e5bfc12d598fdb1cd6cfd5383769d1721216f99
/// @trace_id_sr=SR_EM_10004
/// @trace_id_ad=AD_EM_00037
/// @trace_id_dd=DD_EM_00739
/// @needwork = ad
/// @endcode
class FunctionGroupState
{
public:
    /// @brief Preconstruction token class for FunctionGroupState construction.
    /// Implementation is vendor specific.
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_sr=SR_EM_10004
    /// @trace_id_ad=AD_EM_00037
    /// @trace_id_dd=DD_EM_00740
    /// @needwork = dda
    /// @endcode
    class CtorToken final
    {
    public:
        /// @brief Constructor that creates a new instance.
        /// @param pathToFunctionGroup ARXML short-name path to a Machine.functionGroup of ModeDeclarationGroupPrototype.
        /// @param pathToModeDeclaration ARXML short-name path to a ModeDeclarationGroup.modeDeclaration of ModeDeclaration.
        /// @code{.isoft}
        /// @interface_level=software
        /// @trace_id_ad=AD_EM_00037
        /// @trace_id_dd=DD_EM_00741
        /// @needwork = dda
        /// @endcode
        CtorToken(ara::core::StringView const pathToFunctionGroup,
                  ara::core::StringView const pathToModeDeclaration) noexcept;

        /// @brief Returns the path to a  Machine.functionGroup of ModeDeclarationGroupPrototype.
        /// @return ARXML short-name path.
        /// @code{.isoft}
        /// @interface_level=software
        /// @trace_id_ad=AD_EM_00037
        /// @trace_id_dd=DD_EM_00742
        /// @needwork = dda
        /// @endcode
        ara::core::StringView GetPathToFunctionGroup() const noexcept;

        /// @brief Returns the path to a ModeDeclarationGroup.modeDeclaration of ModeDeclaration.
        /// @return ARXML short-name path.
        /// @code{.isoft}
        /// @interface_level=software
        /// @trace_id_ad=AD_EM_00037
        /// @trace_id_dd=DD_EM_00743
        /// @needwork = dda
        /// @endcode
        ara::core::StringView GetPathToModeDeclaration() const noexcept;

    private:
        /// @code{.isoft}
        /// @interface_level=none
        /// @trace_id_ad=AD_EM_00037
        /// @trace_id_dd=DD_EM_00745
        /// @needwork = dda
        /// @endcode
        ara::core::String pathToFunctionGroup_;
        /// @code{.isoft}
        /// @interface_level=none
        /// @trace_id_ad=AD_EM_00037
        /// @trace_id_dd=DD_EM_00746
        /// @needwork = dda
        /// @endcode
        ara::core::String pathToModeDeclaration_;

        /// @brief EQ operator to compare with other CtorToken.
        /// @param l CtorToken instance to compare this one with.
        /// @param r CtorToken instance to compare this one with.
        /// @return true if objects are equal.
        /// @code{.isoft}
        /// @interface_level=software
        /// @trace_id_ad=AD_EM_00037
        /// @trace_id_dd=DD_EM_00744
        /// @needwork = dda
        /// @endcode
        friend bool operator==(FunctionGroupState::CtorToken const& l, FunctionGroupState::CtorToken const& r) noexcept;
    };

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
    static ara::core::Result< FunctionGroupState::CtorToken > Preconstruct(  // PRQA S 2024
        FunctionGroup const& functionGroup,
        ara::core::StringView const metaModelIdentifier) noexcept;

    /// @brief Constructor that creates FunctionGroupState instance.
    /// @param token representing pre-constructed object.
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_sws=SWS_EM_02271, d973595e55a8642968c7b0b0f4555137146ca929
    /// @trace_id_ad=AD_EM_00037
    /// @trace_id_dd=DD_EM_00748
    /// @needwork = dda
    /// @endcode
    explicit FunctionGroupState(FunctionGroupState::CtorToken&& token) noexcept;

    /// @brief Destructor of the FunctionGroupState instance.
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_sws=SWS_EM_02272, fbce6feb163722c2c2288da20e728d4303ea56da
    /// @trace_id_ad=AD_EM_00037
    /// @trace_id_dd=DD_EM_00749
    /// @needwork = dda
    /// @endcode
    ~FunctionGroupState() noexcept = default;

    /// @brief Default Copy Constructor.
    /// @param other the other FunctionGroupState
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_EM_00037
    /// @trace_id_dd=DD_EM_00750
    /// @needwork = dda
    /// @endcode
    FunctionGroupState(FunctionGroupState const& other) = default;

    /// @brief Default Move Constructor.
    /// @param other the other FunctionGroupState
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_EM_00037
    /// @trace_id_dd=DD_EM_00751
    /// @needwork = dda
    /// @endcode
    FunctionGroupState(FunctionGroupState&& other) = default;

    /// @brief Default Copy Assignment Operator.
 /// @param other Other function group state
 /// @return New function group state instance
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_EM_00037
    /// @trace_id_dd=DD_EM_00752
    /// @needwork = dda
    /// @endcode
    FunctionGroupState& operator=(FunctionGroupState const& other) = default;

    /// @brief Default Move Assignment Operator.
    /// @param other other FunctionGroupState
    /// @return new FunctionGroupState
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_EM_00037
    /// @trace_id_dd=DD_EM_00753
    /// @needwork = dda
    /// @endcode
    FunctionGroupState& operator=(FunctionGroupState&& other) = default;

    /// @brief Getter for the stored Function Group name
    /// @return the storef Function Group name
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_EM_00037
    /// @trace_id_dd=DD_EM_00756
    /// @needwork = dda
    /// @endcode
    ara::core::StringView GetGroupName() const noexcept { return token_.GetPathToFunctionGroup(); };

    /// @brief Getter for the stored Function Group State name
    /// @return the storef State name
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_EM_00037
    /// @trace_id_dd=DD_EM_00757
    /// @needwork = dda
    /// @endcode
    ara::core::StringView GetStateName() const noexcept { return token_.GetPathToModeDeclaration(); };

private:
 /// @brief String
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00037
    /// @trace_id_dd=DD_EM_00758
    /// @needwork = dda
    /// @endcode
    CtorToken token_;

    /// @brief UNEQ operator to compare with other FunctionGroupState instance.
    /// @param l FunctionGroupState instance to compare
    /// @param r FunctionGroupState instance to compare
    /// @return false in case both FunctionGroupStates are representing
    ///          exactly the same meta-model element, true otherwise.
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_sws=SWS_EM_02274, 28c0e978ace9d125ee55ea4c00f132a00678dc20
    /// @trace_id_ad=AD_EM_00037
    /// @trace_id_dd=DD_EM_00755
    /// @needwork = dda
    /// @endcode
    friend bool operator!=(FunctionGroupState const& l, FunctionGroupState const& r) noexcept;

    /// @brief EQ operator to compare with other FunctionGroupState.
    /// @param l FunctionGroupState instance to compare
    /// @param r FunctionGroupState instance to compare
    /// @return true in case both FunctionGroupStates are representing exactly the same meta-model element,
    ///         false otherwise.
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_sws=SWS_EM_02273, de62bd23d7bf256d422ec54dae26c4a1f5f97a0a
    /// @trace_id_ad=AD_EM_00037
    /// @trace_id_dd=DD_EM_00754
    /// @needwork = dda
    /// @endcode
    friend bool operator==(FunctionGroupState const& l, FunctionGroupState const& r) noexcept;
};

/// @brief EQ operator to compare with other CtorToken.
/// @param l CtorToken instance to compare this one with.
/// @param r CtorToken instance to compare this one with.
/// @return true if objects are equal.
/// @code{.isoft}
/// @interface_level=software
/// @trace_id_ad=AD_EM_00037
/// @trace_id_dd=DD_EM_00744
/// @needwork = dda
/// @endcode
inline bool operator==(FunctionGroupState::CtorToken const& l, FunctionGroupState::CtorToken const& r) noexcept
{
    if (l.pathToFunctionGroup_ != r.pathToFunctionGroup_) {
        return false;
    }

    if (l.pathToModeDeclaration_ != r.pathToModeDeclaration_) {
        return false;
    }

    return true;
}

/// @brief EQ operator to compare with other FunctionGroupState instance.
/// @param l FunctionGroupState instance to compare
/// @param r FunctionGroupState instance to compare
/// @return true in case both FunctionGroupStates are representing exactly the same meta-model element,
///         false otherwise.
/// @code{.isoft}
/// @interface_level=software
/// @trace_id_sws=SWS_EM_02273, de62bd23d7bf256d422ec54dae26c4a1f5f97a0a
/// @trace_id_ad=AD_EM_00037
/// @trace_id_dd=DD_EM_00754
/// @needwork = dda
/// @endcode
inline bool operator==(FunctionGroupState const& l, FunctionGroupState const& r) noexcept
{
    return l.token_ == r.token_;
}

/// @brief UNEQ operator to compare with other FunctionGroupState instance.
/// @param l FunctionGroupState instance to compare
/// @param r FunctionGroupState instance to compare
/// @return false in case both FunctionGroupStates are representing
///          exactly the same meta-model element, true otherwise.
/// @code{.isoft}
/// @interface_level=software
/// @trace_id_sws=SWS_EM_02274, 28c0e978ace9d125ee55ea4c00f132a00678dc20
/// @trace_id_ad=AD_EM_00037
/// @trace_id_dd=DD_EM_00755
/// @needwork = dda
/// @endcode
inline bool operator!=(FunctionGroupState const& l, FunctionGroupState const& r) noexcept { return !(l == r); }
}  // namespace exec
}  // namespace ara

#endif  ///< ARA_EXEC_FUNCTION_GROUP_STATE_H_
