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
/// @file       fg_state_notify_async_client.h
/// @brief      Function group state information asynchronous notification client
/// @details
/// @date       2024-06-25
/// @author     lianglongxiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/StateManagement/FGStateNotifyClient
/// @unit_name=FGStateNotifyAsyncClient
/// @interface_level=software
/// @unit_description=Function group state information asynchronous notification client
/// @trace_id_sr=SR_SM_02001, SR_SM_02002
/// @endcode
///
/// ================================================================

#ifndef ARA_SM_INTERNAL_FG_STATE_NOTIFY_ASYNC_CLIENT_H_
#define ARA_SM_INTERNAL_FG_STATE_NOTIFY_ASYNC_CLIENT_H_

#include <ara/core/result.h>
#include <ara/core/vector.h>
#include <ara/sm/internal/fg_state_notify_error_domain.h>
#include <ara/sm/internal/fg_state_type.h>

#include <memory>

namespace ara {
namespace sm {
namespace fg_state_ipc {

/// @code{.isoft}
/// @interface_level=none
/// @trace_id_sr=SR_SM_02001, SR_SM_02002
/// @needwork = no
/// @endcode
class Client;
}  // namespace fg_state_ipc
}  // namespace sm
}  // namespace ara

namespace ara {
namespace sm {
namespace fg_state_notify_client {

/// @brief Function group state information asynchronous notification client RequestAllFGState accesses all function group state information SubscribeFGState subscribes to function group state information
/// @code{.isoft}
/// @interface_level=software
/// @trace_id_sr=SR_SM_02001, SR_SM_02002
/// @trace_id_ad=AD_SM_08025
/// @trace_id_dd=DD_SM_08026
/// @needwork = ad
/// @endcode
class FGStateNotifyAsyncClient final
{
public:
    /// @brief Destructor function
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_sr=SR_SM_02001, SR_SM_02002
    /// @trace_id_ad=AD_SM_08014
    /// @trace_id_dd=DD_SM_08014
    /// @needwork = ad
    /// @endcode
    ~FGStateNotifyAsyncClient() noexcept = default;

    /// @brief Get instance
    /// @return
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_sr=SR_SM_02001, SR_SM_02002
    /// @trace_id_ad=AD_SM_08015
    /// @trace_id_dd=DD_SM_08015
    /// @needwork = ad
    /// @endcode
    static FGStateNotifyAsyncClient *GetInstance() noexcept;

    /// @brief Destroy
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_sr=SR_SM_02001, SR_SM_02002
    /// @trace_id_ad=AD_SM_08016
    /// @trace_id_dd=DD_SM_08016
    /// @needwork = ad
    /// @endcode
    void Destroy() const noexcept;

    /// @brief Initialize
    /// @param[in] clientID Client ID, identifies this Client when subscribing to function group state information
    /// @return core:Result<void>
    /// @return kAlreadyInited, already initialized;
    /// @return kCanNotGetEVLoop, failed to get evloop;
    /// @return kCanNotCreateIPCClient, failed to create IPC client;
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_sr=SR_SM_02001, SR_SM_02002
    /// @trace_id_ad=AD_SM_08017
    /// @trace_id_dd=DD_SM_08017
    /// @needwork = ad
    /// @endcode
    core::Result< void > Init(core::String const &clientID) const noexcept;

    /// @brief Asynchronously get all function group state information
    /// @param[in] requestFGStateRespMsgHandler IPC response message handler for RequestAllFGState
    /// @param[in] timeout Maximum time from sending request message to receiving response message, in milliseconds
    /// @return core:Result<void>
    /// @return kNotInited, not initialized yet
    /// @return kCanNotCreateMSG, failed to create message
    /// @return kConnectionRefused, connection refused;
    /// @return kTimeout, timeout;
    /// @return kOthers, other errors;
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_sr=SR_SM_02001, SR_SM_02002
    /// @trace_id_ad=AD_SM_08018
    /// @trace_id_dd=DD_SM_08018
    /// @needwork = ad
    /// @endcode
    core::Result< void > RequestAllFGState(
        std::function< RequestFGStateRespMsgHandlerType > const &requestFGStateRespMsgHandler,
        int32_t const &timeout = -1) const noexcept;

    /// @brief Asynchronously subscribe to function group state change information
    /// @param[in] subscribeRespMsgHandler IPC response message handler for SubscribeFGState
    /// @param[in] fgStateChangeHandler Callback function when function group state changes;
    /// @param[in] timeout Maximum time from sending request message to receiving response message, in milliseconds
    /// @return core:Result<void>
    /// @return kNotInited, not initialized yet
    /// @return kCanNotCreateMSG, failed to create message
    /// @return kAlreadySubscribed, already subscribed
    /// @return kOthers, other errors;
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_sr=SR_SM_02001, SR_SM_02002
    /// @trace_id_ad=AD_SM_08019
    /// @trace_id_dd=DD_SM_08019
    /// @needwork = ad
    /// @endcode
    core::Result< void > SubscribeFGState(std::function< SubscribeRespMsgHandlerType > const &subscribeRespMsgHandler,
                                          std::function< void(FGStateType &fgState) > const &fgStateChangeHandler,
                                          int32_t const &timeout = -1) const noexcept;

    /// @brief Unsubscribe
    /// @return core:Result<void>
    /// @return kNotInited, not initialized yet
    /// @return kCanNotCreateMSG, failed to create message
    /// @return kConnectionRefused, connection refused;
    /// @return kTimeout, timeout;
    /// @return kNotAlreadySubscribed, not subscribed yet
    /// @return kOthers, other errors;
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_sr=SR_SM_02001, SR_SM_02002
    /// @trace_id_ad=AD_SM_08020
    /// @trace_id_dd=DD_SM_08020
    /// @needwork = ad
    /// @endcode
    core::Result< void > UnsubscribeFGState(
        std::function< UnsubscribeRespMsgHandlerType > const &unsubscribeRespMsgHandler,
        int32_t const &timeout = -1) const noexcept;

public:
    /// @brief Constructor
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_02001, SR_SM_02002
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08027
    /// @needwork = dda
    /// @endcode
    FGStateNotifyAsyncClient() noexcept;

    /// @brief Copy constructor
    /// @param other The FGStateNotifyAsyncClient instance to be copyed
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_02001, SR_SM_02002
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08028
    /// @needwork = dda
    /// @endcode
    FGStateNotifyAsyncClient(FGStateNotifyAsyncClient const &other) = delete;

    /// @brief Assignment constructor
    /// @param other The FGStateNotifyAsyncClient instance to be copyed
    /// @return the assigned FGStateNotifyAsyncClient instance itself
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_02001, SR_SM_02002
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08029
    /// @needwork = dda
    /// @endcode
    FGStateNotifyAsyncClient &operator=(FGStateNotifyAsyncClient const &other) = delete;

    /// @brief Move constructor
    /// @param other The FGStateNotifyAsyncClient instance to be moved
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_02001, SR_SM_02002
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08030
    /// @needwork = dda
    /// @endcode
    FGStateNotifyAsyncClient(FGStateNotifyAsyncClient &&other) = delete;

    /// @brief Move assignment function
    /// @param other The FGStateNotifyAsyncClient instance to be moved
    /// @return the assigned FGStateNotifyAsyncClient instance itself
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_02001, SR_SM_02002
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08031
    /// @needwork = dda
    /// @endcode
    FGStateNotifyAsyncClient &operator=(FGStateNotifyAsyncClient &&other) = delete;

private:
    /// @brief Private implementation
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_02001, SR_SM_02002
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08032
    /// @needwork = dda
    /// @endcode
    std::unique_ptr< ara::sm::fg_state_ipc::Client > clientImpl_;

    /// @brief sInstance_
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_02001, SR_SM_02002
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08033
    /// @needwork = dda
    /// @endcode
    static std::unique_ptr< FGStateNotifyAsyncClient > s_Instance_;
};

}  // namespace fg_state_notify_client
}  // namespace sm
}  // namespace ara

#endif  // ARA_SM_INTERNAL_FG_STATE_NOTIFY_ASYNC_CLIENT_H_
