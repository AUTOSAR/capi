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
/// @file       operation_cycle.h
/// @brief      This file provides the definitions of OperationCycle and related types.
/// @details
/// @date       2021-11-23
/// @author     jiawei
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_DIAG_OPERATION_CYCLE_H_
#define ARA_DIAG_OPERATION_CYCLE_H_

#include <ara/core/instance_specifier.h>
#include <ara/core/result.h>

#include <functional>
#include <memory>

namespace isoft {
namespace dm {
namespace dic {
class OperationCycleAgent;
}  // namespace dic
}  // namespace dm
}  // namespace isoft

namespace ara {
namespace diag {

/// @brief Represents the state information of operation cycles.
///
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
///
/// @traceid{SWS_DM_00750}@tracestatus{draft}
enum class OperationCycleType
{
    kOperationCycleStart = 0x00,  ///< Start/restart the operation cycle.
    kOperationCycleEnd   = 0x01,  ///< End the operation cycle
};

namespace api {
/// @brief Declare OperationCycleProxyWrapper
class OperationCycleProxyWrapper;
}  // namespace api

/// @brief DiagnosticOperationCycleInterface provides functionality for handling of operation cycles.
///
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
///
/// @traceid{SWS_DM_00751}@tracestatus{draft}
class OperationCycle
{
public:
    /// @brief Constructor for DiagnosticOperationCycleInterface
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] specifier InstanceSpecifier to an PortPrototype of an DiagnosticOperationCycleInterface
    /// @throws on overflow
    ///
    /// @traceid{SWS_DM_00752}@tracestatus{draft}
    explicit OperationCycle(ara::core::InstanceSpecifier const& specifier);

    /// @brief Destructor of DiagnosticOperationCycleInterface
    ///
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    ///
    /// @traceid{SWS_DM_00753}@tracestatus{draft}
    ~OperationCycle() noexcept = default;

    /// @brief copy constructor
    /// @param other
    OperationCycle(OperationCycle const& other) noexcept = default;
    /// @brief copy assignment operator
    /// @param other
    /// @return OperationCycle&
    OperationCycle& operator=(OperationCycle const& other) noexcept = default;
    /// @brief move constructor
    /// @param other
    OperationCycle(OperationCycle&& other) noexcept = default;
    /// @brief move assignment operator
    /// @param other
    /// @return OperationCycle&
    OperationCycle& operator=(OperationCycle&& other) noexcept = default;

    /// @brief Get current OperationCycle
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @returns the current OperationCycle
    ///
    ///
    /// @traceid{SWS_DM_00754}@tracestatus{draft}
    ara::core::Result< OperationCycleType > GetOperationCycle();

    /// @brief Registering a notifier function which is called if the operation cycle is changed.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] notifier operation cycle changed callBack
    /// @return ara::core::Result<void>
    /// @throws on overflow
    ///
    /// @traceid{SWS_DM_00755}@tracestatus{draft}
    ara::core::Result< void > SetNotifier(std::function< void(OperationCycleType) > notifier);

    /// @brief Set OperationCycle
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] operationCycle current OperationCycle
    /// @return ara::core::Result<void>
    /// @throws on overflow
    ///
    /// @traceid{SWS_DM_00756}@tracestatus{draft}
    ara::core::Result< void > SetOperationCycle(OperationCycleType operationCycle);

private:
    std::shared_ptr< isoft::dm::dic::OperationCycleAgent >
        /// @name proxy_
        proxy_;
};
}  // namespace diag
}  // namespace ara

#endif  // ARA_DIAG_OPERATION_CYCLE_H_