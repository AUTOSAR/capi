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
/// @file       function_group_set.h
/// @brief      ara configuration series function group set manifest class
/// @details
/// @date       2024-03-28
/// @author     xueliang.bao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/ExecutionManagement/Config
/// @unit_name=FunctionGroupManifest
/// @unit_description=Used to read and save the function group configuration.
/// @interface_level=module
/// @endcode
///
/// ================================================================

#ifndef ARA_EXEC_INTERNAL_CONFIG_FUNCTION_GROUP_SET_H_
#define ARA_EXEC_INTERNAL_CONFIG_FUNCTION_GROUP_SET_H_

#include <ara/core/string.h>
#include <ara/core/vector.h>
#include <isoft/manifestreader/manifest_node.h>

namespace ara {
namespace exec {
namespace internal {
namespace config {

/// @brief Function group configuration
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_EM_20002
/// @trace_id_ad=AD_EM_00110
/// @trace_id_dd=DD_EM_00228
/// @needwork = ad
/// @endcode
class FunctionGroup  // PRQA S 2502,5215 # avoid name hiding
{
public:
    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00110
    /// @trace_id_dd=DD_EM_00847
    /// @needwork = dda
    /// @endcode
    FunctionGroup() = default;

    /// @brief Get the function group FQN
    /// @return Function group FQN
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00110
    /// @trace_id_dd=DD_EM_00229
    /// @needwork = dda
    /// @endcode
    ara::core::String const& GetName() const noexcept { return name_; }

    /// @brief Get the function group state
    /// @return Function group state
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00110
    /// @trace_id_dd=DD_EM_00230
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< ara::core::String > const& GetStates() const noexcept { return states_; }

    /// @brief Load the specified function group information from Node
    /// @param node Node
    /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00110
    /// @trace_id_dd=DD_EM_00231
    /// @needwork = dda
    /// @endcode
    int32_t ManifestLoader(isoft::manifestreader::ManifestNode const& node) noexcept;

    /// @brief Whether the configuration is correct
    /// @return true configuration error; false configuration correct
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00110
    /// @trace_id_dd=DD_EM_00232
    /// @needwork = dda
    /// @endcode
    bool HasConfigError() const noexcept;

    /// @brief Print FunctionGroup information
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    void Debug() const noexcept;

private:
    // PRQA S 2428,4151 ++ # Use char* to define json keys to be compatible with manifestreader

    /// @brief Key value of the function group name (FQN)
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    static constexpr char const* const kName{"name"};

    /// @brief Key value of the function group state
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    static constexpr char const* const kStates{"states"};

    // PRQA S 2428,4151 -- # Use char* to define json keys to be compatible with manifestreader

    /// @brief Function group name
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00110
    /// @trace_id_dd=DD_EM_00233
    /// @needwork = dda
    /// @endcode
    ara::core::String name_{};

    /// @brief Function group state
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00110
    /// @trace_id_dd=DD_EM_00234
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< ara::core::String > states_{};

    /// @brief Whether the configuration is incorrect
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00110
    /// @trace_id_dd=DD_EM_00235
    /// @needwork = dda
    /// @endcode
    bool hasConfigError_{true};
};

/// @brief Function group set class
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_EM_20002
/// @trace_id_ad=AD_EM_00110
/// @trace_id_dd=DD_EM_00236
/// @needwork = ad
/// @endcode
class FunctionGroupSet
{
public:
    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00110
    /// @trace_id_dd=DD_EM_00237
    /// @needwork = dda
    /// @endcode
    FunctionGroupSet() noexcept = default;

    /// @brief Copy constructor
    /// @param other Another function group set instance
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00110
    /// @trace_id_dd=DD_EM_00840
    /// @needwork = dda
    /// @endcode
    FunctionGroupSet(FunctionGroupSet const& other) = default;

    /// @brief Move constructor
    /// @param other Another function group set instance
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    FunctionGroupSet(FunctionGroupSet&& other) noexcept = delete;

    /// @brief Copy assignment operator
    /// @param other Another function group set instance
    /// @return New function group set
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00110
    /// @trace_id_dd=DD_EM_00841
    /// @needwork = dda
    /// @endcode
    FunctionGroupSet& operator=(FunctionGroupSet const& other) = default;

    /// @brief Move assignment operator
    /// @param other Another function group set instance
    /// @return New function group set
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    FunctionGroupSet& operator=(FunctionGroupSet&& other) noexcept = delete;

    /// @brief Default destructor
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00110
    /// @trace_id_dd=DD_EM_00238
    /// @needwork = dda
    /// @endcode
    ~FunctionGroupSet() noexcept { functionGroups_.clear(); }

    /// @brief Load all function group sets and create a function group set instance
    /// @return Function group set instance
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00110
    /// @trace_id_dd=DD_EM_00239
    /// @needwork = dda
    /// @endcode
    static std::shared_ptr< FunctionGroupSet > CreateInstance() noexcept;  // PRQA S 2024

    /// @brief Load the function group set according to the function group set path, and create a function group set instance
    /// @param fgSetPath Function group set manifest file path
    /// @return Function group set instance
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00110
    /// @trace_id_dd=DD_EM_00240
    /// @needwork = dda
    /// @endcode
    static std::shared_ptr< FunctionGroupSet > CreateInstance(ara::core::String const& fgSetPath) noexcept;

    /// @brief Get all function group information of this function group set
    /// @return All function group information of this function group set
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00110
    /// @trace_id_dd=DD_EM_00241
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< FunctionGroup > const& GetFunctionGroups() const noexcept { return functionGroups_; }

private:
    // PRQA S 2428,4151 ++ # Use char* to define json keys to be compatible with manifestreader

    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    static constexpr char const* const kFunctionGroups{"functionGroups"};

    // PRQA S 2428,4151 -- # Use char* to define json keys to be compatible with manifestreader

    /// @brief Load the function group set according to the function group set path
    /// @param fgsPath Function group set manifest file path
    /// @return Loading result, <0 loading failed; =0 loading successful
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00110
    /// @trace_id_dd=DD_EM_00242
    /// @needwork = dda
    /// @endcode
    int32_t _Load(ara::core::String const& fgsPath) noexcept;

    /// @brief Load function group information of the platform or user software cluster
    /// @param swclPath Software cluster list path
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00110
    /// @trace_id_dd=DD_EM_00243
    /// @needwork = dda
    /// @endcode
    void _LoadSwclFunctionGroups(ara::core::String const& swclPath) noexcept;

private:
    /// @brief Function group set
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00110
    /// @trace_id_dd=DD_EM_00244
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< FunctionGroup > functionGroups_;
};

}  // namespace config
}  // namespace internal
}  // namespace exec
}  // namespace ara

#endif  ///< ARA_EXEC_INTERNAL_CONFIG_FUNCTION_GROUP_SET_H_
