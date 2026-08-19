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
/// @file       fg_state_internal_type.h
/// @brief      Define function group information and function types used for communication between Client and Server
/// @details
/// @date       2024-06-25
/// @author     lianglongxiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/StateManagement/Utils
/// @unit_name=FGStateInternalType
/// @interface_level=module
/// @unit_description=Define function group information and function types used for communication between Client and Server
/// @trace_id_sr=SR_SM_02001, SR_SM_02002
/// @endcode
///
/// ================================================================

#ifndef ARA_SM_INTERNAL_FG_STATE_TYPE_INTERNAL_H_
#define ARA_SM_INTERNAL_FG_STATE_TYPE_INTERNAL_H_

#include <ara/core/error_domain.h>
#include <ara/core/result.h>
#include <ara/core/string.h>
#include <ara/core/vector.h>

#include <cstdint>

namespace ara {
namespace sm {
namespace fg_state_ipc {
/// @brief Simplification
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
using String = core::String;

/// @brief FGStateNotifyResult An enumeration with errors that can occur within this Functional Cluster
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_02002
/// @trace_id_ad=AD_SM_08095
/// @trace_id_dd=DD_SM_08354
/// @needwork = ad
/// @endcode
enum class FGStateNotifyResult : int16_t
{
    /// @brief no error
    kSuccess               = 0,
    kConnectionRefused     = -1,    ///< connection refused
    kTimeout               = -2,    ///< time out
    kNotInited             = -3,    ///< not inited
    kCanNotCreateMSG       = -4,    ///< can not create msg
    kCanNotAppendBuffer    = -5,    ///< can not append buffer
    kAlreadyInited         = -11,   ///< already inited
    kCanNotGetEVLoop       = -12,   ///< can not get ev loop
    kCanNotInitNai         = -13,   ///< can not init nai socket
    kCanNotCreateIPCClient = -14,   ///< can not create IPC client
    kCanNotStartIPCClient  = -15,   ///< can not start IPC client
    kAlreadySubscribed     = -21,   ///< already subscribed
    kNotAlreadySubscribed  = -31,   ///< not already subscribed
    kCanNotFindServer      = -32,   ///< can not find server
    kCanNotMalloc          = -33,   ///< can not malloc
    kOthers                = -100,  ///< other errors
};

/// @brief Function group state information
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_02001, SR_SM_02002
/// @trace_id_ad=AD_SM_08096
/// @trace_id_dd=DD_SM_08355
/// @needwork = ad
/// @endcode
struct FGStateInternalType
{
    /// @brief metaModelIdentifier of the function group
    String fgName;

    /// @brief metaModelIdentifier of the function group state. If it is empty, it means the current function group is in an undefined state.
    String fgState;
};

/// @brief Asynchronous RequestAllFGState IPC response message handler
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
using RequestRespMsgHandlerInternalType
    = void(FGStateNotifyResult const, core::Vector< FGStateInternalType > const &allFGState);

/// @brief Asynchronous SubscribeFGState IPC response message handler
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
using SubscribeRespMsgHandlerInternalType = void(FGStateNotifyResult);

/// @brief Asynchronous UnsubscribeFGState IPC response message handler
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
using UnsubscribeRespMsgHandlerInternalType = void(FGStateNotifyResult);

}  // namespace fg_state_ipc
}  // namespace sm
}  // namespace ara

#endif  // ARA_SM_INTERNAL_FG_STATE_TYPE_INTERNAL_H_