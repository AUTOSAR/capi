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
/// @file       server.h
/// @brief      Communication server side
/// @details
/// @date       2024-06-25
/// @author     lianglongxiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/StateManagement/Utils
/// @unit_name=Server
/// @interface_level=module
/// @unit_description=Communication server side
/// @trace_id_sr=SR_SM_02001, SR_SM_02002
/// @endcode
///
/// ================================================================

#ifndef FG_STATE_NOTIFY_SERVER_H_
#define FG_STATE_NOTIFY_SERVER_H_

#include <ara/core/instance_specifier.h>
#include <ara/core/map.h>
#include <ara/core/string_view.h>
#include <ara/log/logger.h>
#include <isoft/ipccpp/buffer.h>
#include <isoft/ipccpp/packet.h>
#include <isoft/ipccpp/server.h>
#include <isoft/ipccpp/utility.h>
#include <isoft/naicpp/evloop.h>
#include <isoft/naicpp/global_evloop.h>

#include <memory>
#include <set>

#include "event.h"
#include "msg_type.h"

// non-generated code
namespace ara {
namespace sm {
namespace fg_state_ipc {

/// @brief Communication server side
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_SM_02001, SR_SM_02002
/// @trace_id_ad=AD_SM_08094
/// @trace_id_dd=DD_SM_08340
/// @needwork = ad
/// @endcode
class Server final
{
public:
    /// @brief Constructor function
    /// @param instance
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_02001, SR_SM_02002
    /// @trace_id_ad=AD_SM_00359
    /// @trace_id_dd=DD_SM_00375
    /// @needwork = ad
    /// @endcode
    explicit Server(core::InstanceSpecifier const &instance) noexcept;

    /// @brief Disable default construction
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_02001, SR_SM_02002
    /// @trace_id_ad=AD_SM_00360
    /// @trace_id_dd=DD_SM_00376
    /// @needwork = ad
    /// @endcode
    Server() = delete;

    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_02001, SR_SM_02002
    /// @trace_id_ad=AD_SM_00361
    /// @trace_id_dd=DD_SM_00377
    /// @needwork = ad
    /// @endcode
    ~Server() = default;

    /// @brief deleted copy constructor function
    /// @param other The Server instance to be copyed
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_02001, SR_SM_02002
    /// @trace_id_ad=AD_SM_00362
    /// @trace_id_dd=DD_SM_00378
    /// @needwork = ad
    /// @endcode
    Server(Server const &other) = delete;

    /// @brief deleted copy assignment function
    /// @param other The Server instance to be copyed
    /// @return the assigned Server instance itself
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_02001, SR_SM_02002
    /// @trace_id_ad=AD_SM_00363
    /// @trace_id_dd=DD_SM_00379
    /// @needwork = ad
    /// @endcode
    Server &operator=(Server const &other) = delete;

    /// @brief Move constructor function
    /// @param other The Server instance to be moved
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_02001, SR_SM_02002
    /// @trace_id_ad=AD_SM_00364
    /// @trace_id_dd=DD_SM_00380
    /// @needwork = ad
    /// @endcode
    Server(Server &&other) = default;

    /// @brief Move assignment function
    /// @param other The Server instance to be moved
    /// @return the assigned Server instance itself
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_02001, SR_SM_02002
    /// @trace_id_ad=AD_SM_00365
    /// @trace_id_dd=DD_SM_00381
    /// @needwork = ad
    /// @endcode
    Server &operator=(Server &&other) = delete;

    /// @brief Initialize
    /// @return true - success
    /// @return false - failure
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_02001, SR_SM_02002
    /// @trace_id_ad=AD_SM_00366
    /// @trace_id_dd=DD_SM_00382
    /// @needwork = ad
    /// @endcode
    bool Init() noexcept;

    /// @brief Start accepting requests
    /// @return true - success
    /// @return false - failure
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_02001, SR_SM_02002
    /// @trace_id_ad=AD_SM_00367
    /// @trace_id_dd=DD_SM_00383
    /// @needwork = ad
    /// @endcode
    bool Start() const noexcept;

    /// @brief Stop accepting requests
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_02001, SR_SM_02002
    /// @trace_id_ad=AD_SM_00368
    /// @trace_id_dd=DD_SM_00384
    /// @needwork = ad
    /// @endcode
    void Stop() const noexcept;

    /// @brief Publish state
    /// @param state
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_02002
    /// @trace_id_ad=AD_SM_00369
    /// @trace_id_dd=DD_SM_00385
    /// @needwork = ad
    /// @endcode
    void PublishFGState(FGStateInternalType const &state) noexcept;

    /// @brief Register the callback function for appending events
    /// @param appendEventHandler Callback function for appending events
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_02001, SR_SM_02002
    /// @trace_id_ad=AD_SM_00370
    /// @trace_id_dd=DD_SM_00386
    /// @needwork = ad
    /// @endcode
    void RegisterAppendEventHandler(std::function< void(common::Event &&) > const &appendEventHandler) noexcept;

    /// @brief Destroy
    /// @return 0 - success
    /// @return non-zero - failure
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_sr=SR_SM_02001, SR_SM_02002
    /// @trace_id_ad=AD_SM_00371
    /// @trace_id_dd=DD_SM_00387
    /// @needwork = ad
    /// @endcode
    int32_t Destroy() const noexcept;

protected:
    /// @brief Message processing
    /// @param context Context
    /// @param type IPC type
    /// @param packet IPC packet
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_02001, SR_SM_02002
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08341
    /// @needwork = dda
    /// @endcode
    static void SMsgHandler(void *const context,
                            isoft::ipc::IPCServerHandleType const type,
                            isoft::ipc::IPCPacket *const packet);

    /// @brief Message processing
    /// @param type IPC type
    /// @param reqPacket IPC packet
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_02001, SR_SM_02002
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08342
    /// @needwork = dda
    /// @endcode
    void _MsgHandler(isoft::ipc::IPCServerHandleType const type, isoft::ipc::IPCPacket *const reqPacket) noexcept;

    /// @brief Send all states
    /// @param sessionId Session ID
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_02001
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08343
    /// @needwork = dda
    /// @endcode
    void _SendAllFGState(isoft::ipc::IPCSessionId const &sessionId) noexcept;

    /// @brief Send subscription state reply
    /// @param sessionId Session ID
    /// @param sendMsg Message to be sent
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_02002
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08344
    /// @needwork = dda
    /// @endcode
    void _SendSubscribeFGStateResp(isoft::ipc::IPCSessionId const &sessionId,
                                   SubscribeFGStateRespMsg &sendMsg) const noexcept;

    /// @brief Send unsubscription state reply
    /// @param sessionId Session ID
    /// @param sendMsg Message to be sent
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_02002
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08345
    /// @needwork = dda
    /// @endcode
    void _SendUnsubscribeFGStateResp(isoft::ipc::IPCSessionId const &sessionId,
                                     SubscribeFGStateRespMsg &sendMsg) const noexcept;

    /// @brief Asynchronously get the current states of all function groups
    /// @param sessionId Session ID
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_02001
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08346
    /// @needwork = dda
    /// @endcode
    void _GetAndSendAllFgStates(isoft::ipc::IPCSessionId const &sessionId) noexcept;

    /// @brief Convert a string in the format FG1.state;FG2.state to core::Vector<FGStateInternalType>
    /// @param str String format of function group states
    /// @return List of function group information
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_02001
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08347
    /// @needwork = dda
    /// @endcode
    core::Vector< FGStateInternalType > _Str2FGStateInternalVec(core::String const &str) const noexcept;

private:
    /// @brief Response to get all function group states
    /// @param sessionId Session ID
    /// @param fgStates List of function group states
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_02001
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08348
    /// @needwork = dda
    /// @endcode
    void _responseToGetAllFgStates(isoft::ipc::IPCSessionId const &sessionId,
                                   core::Vector< FGStateInternalType > const &fgStates) const noexcept;

    /// @brief Logger instance
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_02001, SR_SM_02002
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08349
    /// @needwork = dda
    /// @endcode
    log::Logger &log_;

    /// @brief server instance
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_02001, SR_SM_02002
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08350
    /// @needwork = dda
    /// @endcode
    std::unique_ptr< ::isoft::ipc::IPCServer > ipcServerPtr_;

    /// @brief Mapping of subscriber names to their subscribers
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_02002
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08351
    /// @needwork = dda
    /// @endcode
    std::map< core::String, isoft::ipc::IPCSessionId > subscriberMap_;

    /// @brief Loop instance
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_02001, SR_SM_02002
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08352
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< isoft::naicpp::EvLoop > evLoopPtr_;

    /// @brief Function handle for publishing events to EventManager
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_sr=SR_SM_02001, SR_SM_02002
    /// @trace_id_ad=AD_SM_00000
    /// @trace_id_dd=DD_SM_08353
    /// @needwork = dda
    /// @endcode
    std::function< void(common::Event &&) > appendEventHandler_;
};

}  // namespace fg_state_ipc
}  // namespace sm
}  // namespace ara

#endif  // FG_STATE_NOTIFY_SERVER_H_
