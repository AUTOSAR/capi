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
/// @file       stub.h
/// @brief      time base proxy Stub class
/// @details
/// @date       2023-02-01
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// export_level=/TimeSync/TimeBase
/// module_path=/TimeSync/TimeBase
/// @endcode
///
/// ================================================================

#ifndef ARA_TSYNC_INTERNAL_TIMEBASE_PROXY_STUB_H_
#define ARA_TSYNC_INTERNAL_TIMEBASE_PROXY_STUB_H_

#include <ara/core/map.h>
#include <ara/core/string_view.h>
#include <isoft/ipccpp/buffer.h>
#include <isoft/ipccpp/client.h>
#include <isoft/ipccpp/packet.h>
#include <isoft/ipccpp/utility.h>
#include <isoft/naicpp/global_evloop.h>

#include <cstdint>
#include <memory>

#include "ara/tsync/internal/timebase/proxy/config.h"
#include "ara/tsync/internal/timebase/proxy/message.h"
#include "ara/tsync/internal/timebase/proxy/proxyeventtype.h"
#include "ara/tsync/internal/timebase/resource/tbresmanager.h"

namespace ara {
namespace tsync {
namespace internal {
namespace timebase {
namespace proxy {

/// @brief time base proxy Stub class
class Stub final
{
public:
    /// @brief create proxy stub (client)
    /// @param tbName - time base name
    /// @return proxy stub pointer
    static std::unique_ptr< Stub > CreateStub(ara::core::String const &tbName) noexcept;

    /// @brief constructor
    /// @exception exception specification
    explicit Stub() = default;

    /// @brief destructor
    ~Stub() noexcept;

    /// @brief copy constructor is prohibited
    /// @param other - other object
    Stub(Stub const &other) = delete;

    /// @brief copy is prohibited
    /// @param other - other object
    /// @return reference to class object
    Stub &operator=(Stub const &other) = delete;

    /// @brief move constructor
    /// @param other an existing rvalue reference.
    Stub(Stub &&other) noexcept = default;

    /// @brief '=' operator overload
    /// @param other an existing rvalue reference.
    /// @return reference to class object
    Stub &operator=(Stub &&other) &noexcept = default;

    /// @brief open time base proxy client
    /// @param tbName - time base name
    /// @return 0 - success
    /// @return <0 - failure
    std::int32_t Open(ara::core::String const &tbName) noexcept;

    /// @brief close time base proxy client
    /// @return 0 - success
    /// @return <0 - failure
    std::int32_t Close() noexcept;

    /// @brief get current time base context
    /// @return time base context pointer
    timebase::resource::TBContext *GetTimeBaseResourceContext() noexcept
    {
        if (nullptr == resourceMan_) {
            return nullptr;
        }
        return resourceMan_->GetContext(timeBaseId_);
    }

    /// @brief trigger event
    /// @param event - event type
    /// @return 0 - success
    /// @return <0 - failure
    std::int32_t Notify(ProxyEventType const &event) noexcept;

    /// @brief notification callback function
    /// @param event - event
    /// @param option - operation corresponding to the event
    using NotificationHandler = std::function< void(ProxyEventType const event, std::uint8_t const option) >;

    /// @brief register event notification
    /// @param event - event
    /// @param cb - callback function
    /// @return 0 - success
    /// @return <0 - failure
    std::int32_t RegisterNotification(ProxyEventType const &event, NotificationHandler const &cb) noexcept;

    /// @brief unregister event notification
    /// @param event - event
    /// @return 0 - success
    /// @return <0 - failure
    std::int32_t UnregisterNotification(ProxyEventType const &event) noexcept;

private:
    /// @brief IPC general request operation
    /// @param type - event type
    /// @param option - event operation
    /// @return 0 - success
    /// @return <0 - failure
    std::int32_t _doIpcRequest(ProxyEventType const &type, std::uint8_t const &option) noexcept;

    /// @brief IPC asynchronous send callback function
    /// @param context - user parameter
    /// @param status - IPC communication status
    /// @param responsePacket - the packet responsed from peer
    /// @exception exception specification
    static void IPCClientSendAsyncHandler(void *const context,
                                          isoft::ipc::IPCClientHandlerStatus const status,
                                          isoft::ipc::IPCPacket *const responsePacket);

private:
    /// @name mainLoop_ - main event loop handle
    std::shared_ptr< isoft::naicpp::EvLoop > mainLoop_{nullptr};

    /// @name ipcClient_ - IPCClient handle (not used by internal library)
    std::shared_ptr< isoft::ipc::IPCClient > ipcClient_{nullptr};

    /// @name timeBaseName_ - time base name
    ara::core::String timeBaseName_{};

    /// @name timeBaseId_ - time base ID
    timebase::resource::TimeBaseId timeBaseId_{};

    /// @name resourceMan_ - time base resource manager handle
    std::shared_ptr< resource::TBResManager > resourceMan_{nullptr};

    /// @name notificationHandlers_ - notification callback functions
    ara::core::Map< ProxyEventType, NotificationHandler > notificationHandlers_{};
};

}  // namespace proxy
}  // namespace timebase
}  // namespace internal
}  // namespace tsync
}  // namespace ara

#endif  /// ARA_TSYNC_INTERNAL_TIMEBASE_PROXY_STUB_H_