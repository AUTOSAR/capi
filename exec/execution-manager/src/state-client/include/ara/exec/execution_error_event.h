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
/// @file       execution_error_event.h
/// @brief      Execution error event class implementation
/// @details
/// @date       2022-01-01
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/ExecutionManagement/StateClient
/// @unit_name=StateClient
/// @unit_description=Used to manage the error events when switching the function group state.
/// @interface_level=software
/// @endcode
///
/// ================================================================

#ifndef _ARA_EXEC_EXECUTION_ERROR_EVENT_H_
#define _ARA_EXEC_EXECUTION_ERROR_EVENT_H_

#include <ara/core/string.h>

#include <cstdint>

#include "ara/exec/function_group.h"

namespace ara {
namespace exec {

/// @brief Execution error type definition
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_sws=SWS_EM_02541 Definition of ExecutionError
/// @needwork = no
/// @endcode
using ExecutionError = uint32_t;

/// @brief Execution error event structure definition, used to obtain execution error information through the state client
/// @code{.isoft}
/// @interface_level=software
/// @trace_id_sws=SWS_EM_02544 Definition of ExecutionErrorEvent
/// @trace_id_sr=SR_EM_10005
/// @trace_id_ad=AD_EM_00038
/// @trace_id_dd=DD_EM_00759
/// @needwork = ad
/// @endcode
struct ExecutionErrorEvent final  // NOLINT
{
 /// @brief Execution error information
    /// @code{.isoft}
    /// @interface_level=software
 /// @trace_id_sws=SWS_EM_02545 Definition of executionError
    /// @trace_id_ad=AD_EM_00038
    /// @trace_id_dd=DD_EM_00781
    /// @needwork = dda
    /// @endcode
    ExecutionError executionError{0U};
 /// @brief Function group information that failed to switch state
    /// @code{.isoft}
    /// @interface_level=software
 /// @trace_id_sws=SWS_EM_02546 Definition of functionGroup
    /// @trace_id_ad=AD_EM_00038
    /// @trace_id_dd=DD_EM_00782
    /// @needwork = dda
    /// @endcode
    FunctionGroup functionGroup;
};
}  // namespace exec
}  // namespace ara
#endif  ///< _ARA_EXEC_EXECUTION_ERROR_EVENT_H_
