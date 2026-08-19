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
/// @file       function_group.h
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
/// @unit_description=Used to manage the function group.
/// @interface_level=software
/// @endcode
///
/// ================================================================

#ifndef ARA_EXEC_FUNCTION_GROUP_H_
#define ARA_EXEC_FUNCTION_GROUP_H_

#include <ara/core/result.h>
#include <ara/core/string.h>
#include <ara/core/string_view.h>

namespace ara {
namespace exec {

/// @brief Class representing Function Group defined in meta-model (ARXML).
/// @code{.isoft}
/// @interface_level=software
/// @trace_id_sr=SR_EM_10003
/// @trace_id_ad=AD_EM_00036
/// @trace_id_dd=DD_EM_00722
/// @needwork = ad
/// @endcode
class FunctionGroup // PRQA S 5215 #Suppress issue of class name and file name inconsistency
{
public:
    /// @brief Preconstruction token class for FunctionGroup construction.
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_sr=SR_EM_10003
    /// @trace_id_ad=AD_EM_00036
    /// @trace_id_dd=DD_EM_00723
    /// @needwork = dda
    /// @endcode
    class CtorToken final
    {
    public:
        /// @brief Constructor that creates FunctionGroup instance.
        /// @param pathToFunctionGroup shortName of the represented ModeDeclarationGroupPrototype.
        /// @code{.isoft}
        /// @interface_level=software
        /// @trace_id_ad=AD_EM_00036
        /// @trace_id_dd=DD_EM_00724
        /// @needwork = dda
        /// @endcode
        explicit CtorToken(ara::core::StringView const pathToFunctionGroup) noexcept;

        /// @brief Returns shortName of the ModeDeclarationGroupPrototype.
        /// @return ARXML shortName.
        /// @code{.isoft}
        /// @interface_level=software
        /// @trace_id_ad=AD_EM_00036
        /// @trace_id_dd=DD_EM_00725
        /// @needwork = dda
        /// @endcode
        ara::core::StringView GetPathToFunctionGroup() const noexcept;

    private:
 /// @brief Function group FQN
        /// @code{.isoft}
        /// @interface_level=none
        /// @trace_id_ad=AD_EM_00036
        /// @trace_id_dd=DD_EM_00727
        /// @needwork = dda
        /// @endcode
        ara::core::String pathToFunctionGroup_;

        /// @brief EQ operator to compare with other CtorToken.
        /// @param l CtorToken instance to compare
        /// @param r CtorToken instance to compare
        /// @return true if objects are equal.
        /// @code{.isoft}
        /// @interface_level=software
        /// @trace_id_ad=AD_EM_00036
        /// @trace_id_dd=DD_EM_00726
        /// @needwork = dda
        /// @endcode
        friend bool operator==(FunctionGroup::CtorToken const& l, FunctionGroup::CtorToken const& r) noexcept;
    };

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
    static ara::core::Result< FunctionGroup::CtorToken > Preconstruct(  // PRQA S 2024
        ara::core::StringView const metaModelIdentifier) noexcept;

    /// @brief Constructor that creates FunctionGroup instance.
    /// @param token representing pre-constructed object.
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_sws=SWS_EM_02265, 45c52a0e0f8eee90eac6fc743e4e9f63d3e81b39
    /// @trace_id_ad=AD_EM_00036
    /// @trace_id_dd=DD_EM_00729
    /// @needwork = dda
    /// @endcode
    FunctionGroup(FunctionGroup::CtorToken&& token) noexcept;  // NOLINT

    /// @brief Destructor of the FunctionGroup instance.
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_sws=SWS_EM_02266, d8f48e4f6d21fcdaf571d12f0ef157b817616ba1
    /// @trace_id_ad=AD_EM_00036
    /// @trace_id_dd=DD_EM_00730
    /// @needwork = dda
    /// @endcode
    ~FunctionGroup() noexcept = default;

    /// @brief Copy Constructor of the FunctionGroup instance.
    /// @param fg The FunctionGroup instance to be copied.
 /// @exception std::bad_alloc Thrown when memory allocation fails
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_EM_00036
    /// @trace_id_dd=DD_EM_00731
    /// @needwork = dda
    /// @endcode
    FunctionGroup(FunctionGroup const& fg) = default;
    /// @brief Move Constructor of the FunctionGroup Instance
    /// @param fg The FunctionGroup instance to be moved.
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_EM_00036
    /// @trace_id_dd=DD_EM_00732
    /// @needwork = dda
    /// @endcode
    FunctionGroup(FunctionGroup&& fg) = default;

    /// @brief Default copy assignment operator.
    /// @param fg FunctionGroup instance to copy.
    /// @return new FunctionGroup instance
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_EM_00036
    /// @trace_id_dd=DD_EM_00733
    /// @needwork = dda
    /// @endcode
    FunctionGroup& operator=(FunctionGroup const& fg) = default;

    /// @brief Default move assignment operator.
    /// @param fg FunctionGroup instance to move.
    /// @return new FunctionGroup instance
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_EM_00036
    /// @trace_id_dd=DD_EM_00734
    /// @needwork = dda
    /// @endcode
    FunctionGroup& operator=(FunctionGroup&& fg) = default;

    /// @brief Getter for the stored meta-model identifier
    /// @return StringView value for the stored meta-model identifier
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_EM_00036
    /// @trace_id_dd=DD_EM_00737
    /// @needwork = dda
    /// @endcode
    ara::core::StringView GetMetaModelIdentifier() const noexcept;

private:
 /// @brief String object
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00036
    /// @trace_id_dd=DD_EM_00738
    /// @needwork = dda
    /// @endcode
    CtorToken token_;

    /// @brief EQ operator to compare with other FunctionGroup instance.
    /// @param l FunctionGroup instance to compare.
    /// @param r FunctionGroup instance to compare.
    /// @return true in case both FunctionGroups are representing exactly
    ///         the same meta-model element, false otherwise.
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_sws=SWS_EM_02267
    /// @trace_id_ad=AD_EM_00036
    /// @trace_id_dd=DD_EM_00735
    /// @needwork = dda
    /// @endcode
    friend bool operator==(FunctionGroup const& l, FunctionGroup const& r) noexcept;

    /// @brief UNEQ operator to compare with other FunctionGroup instance.
    /// @param l FunctionGroup instance to compare.
    /// @param r FunctionGroup instance to compare.
    /// @return false in case both FunctionGroups are representing
    ///          exactly the same meta-model element, true otherwise.
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_sws=SWS_EM_02268
    /// @trace_id_ad=AD_EM_00036
    /// @trace_id_dd=DD_EM_00736
    /// @needwork = dda
    /// @endcode
    friend bool operator!=(FunctionGroup const& l, FunctionGroup const& r) noexcept;
};

/// @brief EQ operator to compare with other CtorToken.
/// @param l CtorToken instance to compare
/// @param r CtorToken instance to compare
/// @return true if objects are equal.
/// @code{.isoft}
/// @interface_level=software
/// @trace_id_ad=AD_EM_00036
/// @trace_id_dd=DD_EM_00726
/// @needwork = dda
/// @endcode
inline bool operator==(FunctionGroup::CtorToken const& l, FunctionGroup::CtorToken const& r) noexcept
{
    return l.pathToFunctionGroup_ == r.pathToFunctionGroup_;
}

/// @brief EQ operator to compare with other FunctionGroup instance.
/// @param l FunctionGroup instance to compare.
/// @param r FunctionGroup instance to compare.
/// @return true in case both FunctionGroups are representing exactly
///         the same meta-model element, false otherwise.
/// @code{.isoft}
/// @interface_level=software
/// @trace_id_sws=SWS_EM_02267
/// @trace_id_ad=AD_EM_00036
/// @trace_id_dd=DD_EM_00735
/// @needwork = dda
/// @endcode
inline bool operator==(FunctionGroup const& l, FunctionGroup const& r) noexcept { return l.token_ == r.token_; }

/// @brief UNEQ operator to compare with other FunctionGroup instance.
/// @param l FunctionGroup instance to compare.
/// @param r FunctionGroup instance to compare.
/// @return false in case both FunctionGroups are representing
///          exactly the same meta-model element, true otherwise.
/// @code{.isoft}
/// @interface_level=software
/// @trace_id_sws=SWS_EM_02268
/// @trace_id_ad=AD_EM_00036
/// @trace_id_dd=DD_EM_00736
/// @needwork = dda
/// @endcode
inline bool operator!=(FunctionGroup const& l, FunctionGroup const& r) noexcept { return !(l == r); }

}  // namespace exec
}  // namespace ara

#endif  ///< ARA_EXEC_FUNCTION_GROUP_H_
