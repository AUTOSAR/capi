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
/// @file       condition.h
/// @brief      This file provides the definitions of Condition and related types.
/// @details
/// @date       2021-11-23
/// @author     jiawei
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_DIAG_CONDITION_H_
#define ARA_DIAG_CONDITION_H_

#include <ara/core/instance_specifier.h>
#include <ara/core/result.h>

#include <memory>

namespace isoft {
namespace dm {
namespace dic {
class ConditionAgent;
}  // namespace dic
}  // namespace dm
}  // namespace isoft

namespace ara {
namespace diag {

/// @brief Type for Condition status
///
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
///
/// @traceid{SWS_DM_00710}@tracestatus{draft}
enum class ConditionType
{
    kConditionFalse = 0x00,  ///< condition is set to false
    kConditionTrue  = 0x01,  ///< condition is set to true
};

namespace api {
/// @brief Declare ConditionProxyWrapper
class ConditionProxyWrapper;
}  // namespace api

/// @brief DiagnosticConditionInterface
///
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
///
/// @traceid{SWS_DM_00711}@tracestatus{draft}
class Condition
{
public:
    /// @brief Constructor of Condition Class
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] specifier InstanceSpecifier to an PortPrototype of an DiagnosticConditionInterface
    /// @throws on overflow
    ///
    /// @traceid{SWS_DM_00712}@tracestatus{draft}
    explicit Condition(ara::core::InstanceSpecifier const& specifier);

    /// @brief Destructor of class Condition
    ///
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    ///
    /// @traceid{SWS_DM_00713}@tracestatus{draft}
    ~Condition() noexcept = default;

    /// @brief copy constructor
    /// @param other
    Condition(Condition const& other) noexcept = default;
    /// @brief copy assignment operator
    /// @param other
    /// @return Condition&
    Condition& operator=(Condition const& other) noexcept = default;
    /// @brief move constructor
    /// @param other
    Condition(Condition&& other) noexcept = default;
    /// @brief move assignment operator
    /// @param other
    /// @return Condition&
    Condition& operator=(Condition&& other) noexcept = default;

    /// @brief Get current condition
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @return the current condition
    ///
    ///
    /// @traceid{SWS_DM_00714}@tracestatus{draft}
    ara::core::Result< ConditionType > GetCondition();

    /// @brief Set condition
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] condition current condition
    /// @return ara::core::Result<void>
    /// @throws on overflow
    ///
    /// @traceid{SWS_DM_00715}@tracestatus{draft}
    ara::core::Result< void > SetCondition(ConditionType condition);

private:
    std::shared_ptr< isoft::dm::dic::ConditionAgent >
        /// @brief proxy_
        proxy_;
};
}  // namespace diag
}  // namespace ara

#endif  // ARA_DIAG_CONDITION_H_