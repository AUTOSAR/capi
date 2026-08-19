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
/// @file       fg_state_type.h
/// @brief      Define function group state data types and related handler function types
/// @details
/// @date       2024-06-25
/// @author     lianglongxiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/StateManagement/FGStateNotifyClient
/// @unit_name=FGStateNotifyType
/// @interface_level=software
/// @unit_description=Define function group state data types and related handler function types
/// @trace_id_sr=SR_SM_02001, SR_SM_02002
/// @endcode
///
/// ================================================================

#ifndef ARA_SM_INTERNAL_FG_STATE_TYPE_H_
#define ARA_SM_INTERNAL_FG_STATE_TYPE_H_

#include <ara/core/result.h>
#include <ara/core/string.h>
#include <ara/core/vector.h>

#include <cstdint>

namespace ara {
namespace sm {
namespace fg_state_notify_client {

/// @brief Function group state information
/// @code{.isoft}
/// @interface_level=software
/// @trace_id_sr=SR_SM_02001, SR_SM_02002
/// @trace_id_ad=AD_SM_08024
/// @trace_id_dd=DD_SM_08025
/// @needwork = ad
/// @endcode
struct FGStateType
{
    /// @brief metaModelIdentifier of the function group
    core::String fgName;
    /// @brief metaModelIdentifier of the function group state. If it is empty, it means the current function group is in an undefined state.
    core::String fgState;
};

/// @brief Asynchronous RequestAllFGState IPC response message handler
/// @param res Result
/// @return kConnectionRefused, connection refused;
/// @return kTimeout, timeout;
/// @return kOthers, other errors;
/// @param allFGState Returns all function group state information
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_sr=SR_SM_02001, SR_SM_02002
/// @needwork = no
/// @endcode
using RequestFGStateRespMsgHandlerType
    = void(core::Result< void > const res, core::Vector< FGStateType > const& allFGState);

/// @brief Asynchronous SubscribeFGState IPC response message handler
/// @param res Result
/// @return kConnectionRefused, connection refused;
/// @return kTimeout, timeout;
/// @return kOthers, other errors;
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_sr=SR_SM_02001, SR_SM_02002
/// @needwork = no
/// @endcode
using SubscribeRespMsgHandlerType = void(core::Result< void > const res);

/// @brief Asynchronous UnsubscribeFGState IPC response message handler
/// @param res Result
/// @return kConnectionRefused, connection refused;
/// @return kTimeout, timeout;
/// @return kOthers, other errors;
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_sr=SR_SM_02001, SR_SM_02002
/// @needwork = no
/// @endcode
using UnsubscribeRespMsgHandlerType = void(core::Result< void > const res);
}  // namespace fg_state_notify_client
}  // namespace sm
}  // namespace ara

#endif  // ARA_SM_INTERNAL_FG_STATE_TYPE_H_