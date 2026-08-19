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
/// @file       client.h
/// @brief      Implementation of function group state information asynchronous notification client
/// @details
/// @date       2024-06-25
/// @author     lianglongxiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/StateManagement/Utils
/// @unit_name=Client
/// @interface_level=module
/// @unit_description=Implementation of function group state information asynchronous notification client
/// @trace_id_sr=SR_SM_02001, SR_SM_02002
/// @endcode
///
/// ================================================================

#ifndef FG_STATE_NOTIFY_ASYNC_CLIENT_IMP_H_
#define FG_STATE_NOTIFY_ASYNC_CLIENT_IMP_H_

#include <ara/core/instance_specifier.h>
#include <ara/core/map.h>
#include <ara/core/result.h>
#include <ara/core/vector.h>
#include <ara/log/logger.h>
#include <isoft/ipccpp/buffer.h>
#include <isoft/ipccpp/client.h>
#include <isoft/ipccpp/packet.h>
#include <isoft/ipccpp/utility.h>
#include <isoft/naicpp/evloop.h>

#include <condition_variable>
#include <memory>
#include <mutex>

#include "fg_state_internal_type.h"

namespace ara {
namespace sm {
namespace fg_state_ipc {

/// @brief Type naming simplification
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
using IPCClientPtr = std::shared_ptr< ::isoft::ipc::IPCClient >;

/// @brief Implementation of function group state information asynchronous notification client
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_02001, SR_SM_02002
/// @trace_id_ad=AD_SM_08103
/// @trace_id_dd=DD_SM_08375
/// @needwork = ad
/// @endcode
class Client final
{
public:
    /// @brief Constructor
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_02001, SR_SM_02002
    /// @trace_id_ad=AD_SM_00432
    /// @trace_id_dd=DD_SM_00448
    /// @needwork = ad
    /// @endcode
    Client() = default;

    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_02001, SR_SM_02002
    /// @trace_id_ad=AD_SM_00433
    /// @trace_id_dd=DD_SM_00449
    /// @needwork = ad
    /// @endcode
    ~Client() = default;

    /// @brief deleted copy constructor function
    /// @param other The Client instance to be copyed
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_02001, SR_SM_02002
    /// @trace_id_ad=AD_SM_00434
    /// @trace_id_dd=DD_SM_00450
    /// @needwork = ad
    /// @endcode
    Client(Client const &other) = delete;

    /// @brief deleted copy assignment function
    /// @param other The Client instance to be copyed
    /// @return the assigned Client instance itself
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_02001, SR_SM_02002
    /// @trace_id_ad=AD_SM_00435
    /// @trace_id_dd=DD_SM_00451
    /// @needwork = ad
    /// @endcode
    Client &operator=(Client const &other) = delete;

    /// @brief Move constructor function
    /// @param other The Client instance to be moved
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_02001, SR_SM_02002
    /// @trace_id_ad=AD_SM_00436
    /// @trace_id_dd=DD_SM_00452
    /// @needwork = ad
    /// @endcode
    Client(Client &&other) = default;

    /// @brief Move assignment function
    /// @param other The Client instance to be moved
    /// @return the assigned Client instance itself
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_02001, SR_SM_02002
    /// @trace_id_ad=AD_SM_00437
    /// @trace_id_dd=DD_SM_00453
    /// @needwork = ad
    /// @endcode
    Client &operator=(Client &&other) = delete;
    /// @brief Initialize
    /// @param[in] clientID Client ID, identifies this Client when subscribing to function group state information
    /// @return core:Result<void>
    /// @return kAlreadyInited, already initialized;
    /// @return kCanNotGetEVLoop, failed to get evloop;
    /// @return kCanNotCreateIPCClient, failed to create IPC client;
    /// @code{.isoft}
    ///
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_02001, SR_SM_02002
    /// @trace_id_ad=AD_SM_00438
    /// @trace_id_dd=DD_SM_00454
    /// @needwork = ad
    /// @endcode
    FGStateNotifyResult Init(core::String const &clientID) noexcept;

    /// @brief Deinitialize
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_02001, SR_SM_02002
    /// @trace_id_ad=AD_SM_00439
    /// @trace_id_dd=DD_SM_00455
    /// @needwork = ad
    /// @endcode
    void Destroy() const noexcept;

    /// @brief Asynchronously get all function group state information
    /// @param[in] reqRespMsgHandler IPC response message handler for RequestAllFGState
    /// @param[in] timeout Maximum time from sending request message to receiving response message, in milliseconds
    /// @return Notification result
    /// @return kNotInited, not initialized yet
    /// @return kCanNotCreateMSG, failed to create message
    /// @return kConnectionRefused, connection refused;
    /// @return kTimeout, timeout;
    /// @return kOthers, other errors;
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_02001
    /// @trace_id_ad=AD_SM_00440
    /// @trace_id_dd=DD_SM_00456
    /// @needwork = ad
    /// @endcode
    FGStateNotifyResult RequestAllFGState(std::function< RequestRespMsgHandlerInternalType > const &reqRespMsgHandler,
                                          int32_t const &timeout = -1) noexcept;

    /// @brief Asynchronously subscribe to function group state change information
    /// @param[in] subscribeRespMsgHandler IPC response message handler for SubscribeFGState
    /// @param[in] FGStateChangeHandler Callback function when function group state changes;
    /// @param[in] timeout Maximum time from sending request message to receiving response message, in milliseconds
    /// @return Notification result
    /// @return kNotInited, not initialized yet
    /// @return kCanNotCreateMSG, failed to create message
    /// @return kAlreadySubscribed, already subscribed
    /// @return kOthers, other errors;
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_02002
    /// @trace_id_ad=AD_SM_00441
    /// @trace_id_dd=DD_SM_00457
    /// @needwork = ad
    /// @endcode
    FGStateNotifyResult SubscribeFGState(
        std::function< SubscribeRespMsgHandlerInternalType > const &subscribeRespMsgHandler,
        std::function< void(FGStateInternalType const &fgState) > const &fGStateChangeHandler,
        int32_t const &timeout = -1) noexcept;
    /// @brief Unsubscribe
    /// @return Notification result
    /// @return kNotInited, not initialized yet
    /// @return kCanNotCreateMSG, failed to create message
    /// @return kConnectionRefused, connection refused;
    /// @return kTimeout, timeout;
    /// @return kNotAlreadySubscribed, not subscribed yet
    /// @return kOthers, other errors;
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_02002
    /// @trace_id_ad=AD_SM_00442
    /// @trace_id_dd=DD_SM_00458
    /// @needwork = ad
    /// @endcode
    FGStateNotifyResult UnsubscribeFGState(
        std::function< UnsubscribeRespMsgHandlerInternalType > const &unsubscribeRespMsgHandler,
        int32_t const &timeout = -1) noexcept;

private:
    /// @brief Process all states after the request
    /// @param context Processing context
    /// @param status IPC client handle status
    /// @param responsePacket IPC response packet
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_02001
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08376
    /// @needwork = dda
    /// @endcode
    void _handlerAfterRequestingAllFGState
        [[maybe_unused]] (void *const context,
                          isoft::ipc::IPCClientHandlerStatus const &status,
                          isoft::ipc::IPCPacket *const responsePacket) const noexcept;

    /// @brief Process the group state after subscription
    /// @param context Processing context
    /// @param status IPC client handle status
    /// @param responsePacket IPC response packet
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_02002
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08377
    /// @needwork = dda
    /// @endcode
    void _handlerAfterSubscribingFGState([[maybe_unused]] void *const context,
                                         isoft::ipc::IPCClientHandlerStatus const &status,
                                         isoft::ipc::IPCPacket *const responsePacket) noexcept;

    /// @brief Process the group state after unsubscription
    /// @param context Processing context
    /// @param status IPC client handle status
    /// @param responsePacket IPC response packet
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_02002
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08378
    /// @needwork = dda
    /// @endcode
    void _handlerAfterUnsubscribingFGState([[maybe_unused]] void *const context,
                                           isoft::ipc::IPCClientHandlerStatus const &status,
                                           isoft::ipc::IPCPacket *const responsePacket) noexcept;

    /// @brief  Whether initialized
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_02001, SR_SM_02002
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08379
    /// @needwork = dda
    /// @endcode
    bool bInit_{false};

    /// @brief Client ID, identifies this Client when subscribing to function group state information
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_02001, SR_SM_02002
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08380
    /// @needwork = dda
    /// @endcode
    core::String clientId_{};

    /// @brief  Logger instance
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_02001, SR_SM_02002
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08381
    /// @needwork = dda
    /// @endcode
    log::Logger &logger_{
        log::CreateLogger(core::StringView{"FGN"}, core::StringView{"FGStateNotifyComm"}, log::LogLevel::kVerbose)};

    /// @brief Use forward declaration to hide headers related to eventloop
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_02001, SR_SM_02002
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08382
    /// @needwork = dda
    /// @endcode
    IPCClientPtr clientPtr_{nullptr};

    /// @brief Asynchronous RequestAllFGState IPC response message handler callback
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_02001
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08383
    /// @needwork = dda
    /// @endcode
    std::function< RequestRespMsgHandlerInternalType > reqRespMsgHandler_{};

    /// @brief Asynchronous SubscribeFGState IPC response message handler callback
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_02002
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08384
    /// @needwork = dda
    /// @endcode
    std::function< SubscribeRespMsgHandlerInternalType > subscribeRespMsgHandler_{};

    /// @brief Whether it has subscribed
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_02002
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08385
    /// @needwork = dda
    /// @endcode
    bool subScribed_{false};

    /// @brief Callback function
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_02002
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08386
    /// @needwork = dda
    /// @endcode
    std::function< void(FGStateInternalType const &fgState) > fGStateHandler_{};

    /// @brief Asynchronous UnsubscribeFGState IPC response message handler callback
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_02002
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08387
    /// @needwork = dda
    /// @endcode
    std::function< UnsubscribeRespMsgHandlerInternalType > unsubscribeRespMsgHandler_{};

    /// @brief Event loop pointer
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_02001, SR_SM_02002
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08388
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< ::isoft::naicpp::EvLoop > evLoopPtr_{nullptr};
};

}  // namespace fg_state_ipc
}  // namespace sm
}  // namespace ara

#endif  // FG_STATE_NOTIFY_ASYNC_CLIENT_IMP_H_
