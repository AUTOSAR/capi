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
/// @file       skeleton.h
/// @brief      time base proxy Skeleton class
/// @details
/// @date       2023-02-01
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_TSYNC_INTERNAL_TIMEBASE_PROXY_SKELETON_H_
#define ARA_TSYNC_INTERNAL_TIMEBASE_PROXY_SKELETON_H_

#include <ara/core/string_view.h>
#include <isoft/ipccpp/buffer.h>
#include <isoft/ipccpp/packet.h>
#include <isoft/ipccpp/server.h>
#include <isoft/ipccpp/utility.h>
#include <isoft/naicpp/global_evloop.h>

#include <chrono>
#include <cstdint>
#include <memory>

#include "ara/tsync/internal/timebase/proxy/config.h"
#include "ara/tsync/internal/timebase/proxy/message.h"
#include "ara/tsync/internal/timebase/proxy/proxyeventtype.h"
#include "ara/tsync/internal/timebase/proxy/sessionmanager.h"
#include "ara/tsync/internal/timebase/resource/tbresmanager.h"
#include "ara/tsync/internal/timedomain/manager.h"

namespace ara {
namespace tsync {
namespace internal {
namespace timebase {
namespace proxy {

/// @brief time base proxy skeleton side
class Skeleton final
{
public:
    /// @brief create proxy skeleton
    /// @param rs - time base resource manager handle
    /// @param pconfig - configuration manager handle
    /// @param timeDomainManager - time domain resource manager handle
    /// @return proxy skeleton pointer
    static std::unique_ptr< Skeleton > CreateSkeleton(
        std::shared_ptr< timebase::resource::TBResManager > const& rs,
        std::shared_ptr< config::ConfigManager > const& pconfig,
        std::shared_ptr< timedomain::TDManager > const& timeDomainManager) noexcept;

    /// @brief constructor
    /// @param rs - time base resource manager handle
    /// @param pconfig - configuration manager handle
    /// @param timeDomainManager - time domain resource manager handle
    Skeleton() = default;

    /// @brief destructor
    ~Skeleton() noexcept;

    /// @brief copy constructor is prohibited
    /// @param other - other object
    Skeleton(Skeleton const& other) = delete;

    /// @brief copy is prohibited
    /// @param other - other object
    /// @return reference to class object
    Skeleton& operator=(Skeleton const& other) = delete;

    /// @brief move constructor
    /// @param other an existing rvalue reference.
    Skeleton(Skeleton&&) noexcept = default;

    /// @brief '=' operator overload
    /// @param other an existing rvalue reference.
    /// @return reference to class object
    Skeleton& operator=(Skeleton&&) & noexcept = default;

    /// @brief register event callback function, called when an event from the Stub side arrives.
    /// @param cb - callback function.
    void OnEvent(ProxyEventHandler const& cb) noexcept { eventCb_ = cb; }

    /// @brief notify status change
    /// @param timeBaseId - time base ID
    /// @return 0 - success
    /// @return <0 - failure
    std::int32_t NotifyStatusChanged(timebase::resource::TimeBaseId const& timeBaseId) noexcept
    {
        return _doNotify(timeBaseId, ProxyEventType::kStatusChanged,
                         static_cast< std::uint8_t >(Message::Option::kNotify));
    }

    /// @brief notify synchronization status change
    /// @param timeBaseId - time base ID
    /// @return 0 - success
    /// @return <0 - failure
    std::int32_t NotifySynchronizationStateChanged(timebase::resource::TimeBaseId const& timeBaseId) noexcept
    {
        return _doNotify(timeBaseId, ProxyEventType::kSynchronizationStateChanged,
                         static_cast< std::uint8_t >(Message::Option::kNotify));
    }

    /// @brief notify time jump
    /// @param timeBaseId - time base ID
    /// @return 0 - success
    /// @return <0 - failure
    std::int32_t NotifyLeapJump(timebase::resource::TimeBaseId const& timeBaseId) noexcept
    {
        return _doNotify(timeBaseId, ProxyEventType::kLeapJump, static_cast< std::uint8_t >(Message::Option::kNotify));
    }

    /// @brief notify precision measurement
    /// @param timeBaseId - time base ID
    /// @return 0 - success
    /// @return <0 - failure
    std::int32_t NotifyPrecisionMeasurement(timebase::resource::TimeBaseId const& timeBaseId) noexcept
    {
        return _doNotify(timeBaseId, ProxyEventType::kPrecisionMeasurement,
                         static_cast< std::uint8_t >(Message::Option::kNotify));
    }

    /// @brief time validation notification
    /// @param timeBaseId - time base ID
    /// @return 0 - success
    /// @return <0 - failure
    std::int32_t NotifyValidationMeasurementSetPdelayInitiatorData(
        timebase::resource::TimeBaseId const& timeBaseId) noexcept
    {
        return _doNotify(timeBaseId, ProxyEventType::kValidationMeasurement,
                         static_cast< std::uint8_t >(Message::ValidationMeasurementOption::kSetPdelayInitiatorData));
    }

    /// @brief time validation notification
    /// @param timeBaseId - time base ID
    /// @return 0 - success
    /// @return <0 - failure
    std::int32_t NotifyValidationMeasurementSetPdelayResponderData(
        timebase::resource::TimeBaseId const& timeBaseId) noexcept
    {
        return _doNotify(timeBaseId, ProxyEventType::kValidationMeasurement,
                         static_cast< std::uint8_t >(Message::ValidationMeasurementOption::kSetPdelayResponderData));
    }

    /// @brief time validation notification
    /// @param timeBaseId - time base ID
    /// @return 0 - success
    /// @return <0 - failure
    std::int32_t NotifyValidationMeasurementSetSlaveTimingData(
        timebase::resource::TimeBaseId const& timeBaseId) noexcept
    {
        return _doNotify(timeBaseId, ProxyEventType::kValidationMeasurement,
                         static_cast< std::uint8_t >(Message::ValidationMeasurementOption::kSetSlaveTimingData));
    }

    /// @brief time validation notification
    /// @param timeBaseId - time base ID
    /// @return 0 - success
    /// @return <0 - failure
    std::int32_t NotifyValidationMeasurementSetMasterTimingData(
        timebase::resource::TimeBaseId const& timeBaseId) noexcept
    {
        return _doNotify(timeBaseId, ProxyEventType::kValidationMeasurement,
                         static_cast< std::uint8_t >(Message::ValidationMeasurementOption::kSetMasterTimingData));
    }

private:
    /// @brief open service
    /// @param rs - time base resource manager handle
    /// @param pconfig - configuration manager handle
    /// @param timeDomainManager - time domain resource manager handle
    /// @return 0 - success
    /// @return <0 - failure
    std::int32_t _open(std::shared_ptr< timebase::resource::TBResManager > const& rs,
                       std::shared_ptr< config::ConfigManager > const& pconfig,
                       std::shared_ptr< timedomain::TDManager > const& timeDomainManager) noexcept;

    /// @brief close service
    /// @return 0 - success
    /// @return <0 - failure
    std::int32_t _close() noexcept;

    /// @brief IPC request callback function, called when a request from the client is received.
    /// @param context - user parameter
    /// @param htype - handle type
    /// @param reqPacket - data packet
    static void IPCServerRequestHandler(void* const context,
                                        isoft::ipc::IPCServerHandleType const htype,
                                        isoft::ipc::IPCPacket* const reqPacket) noexcept;

    /// @brief IPC service connection callback function, called when a client establishes or disconnects a connection.
    /// @param context - user parameter
    /// @param type - type
    /// @param packet - data packet
    /// @exception exception specification
    static void IPCServerConnectionHandler(void* const context,
                                           isoft::ipc::IPCServerHandleType const type,
                                           isoft::ipc::IPCPacket* const packet);
    /// @brief general response function
    /// @param serviceId - service ID (time base ID)
    /// @param kSid - session ID
    /// @param type - message type
    /// @param option - message operation
    /// @return 0 - success
    /// @return <0 - failure
    std::int32_t _doIpcResponse(SessionManager::ServiceId const& serviceId,
                                SessionManager::SessionId const& kSid,
                                SessionManager::SessionType const& type,
                                std::uint8_t const& option) const noexcept;

    /// @brief general event notification. Notify the specified event on the specified time base.
    /// @param timeBaseId - time base ID
    /// @param eventType - event type
    /// @param option - operation
    /// @return notification result
    std::int32_t _doNotify(timebase::resource::TimeBaseId const& timeBaseId,
                           ProxyEventType const& eventType,
                           std::uint8_t const& option) noexcept;

    /// @traceid  {SWS_TS_00803}
    /// @brief check whether time precision measurement registration is allowed, only non-global domain masters are allowed to register
    /// @param timeBaseId - time base ID
    /// @return true allowed, false not allowed
    bool _checkAllowMeasurementRegistration(timebase::resource::TimeBaseId const& timeBaseId) noexcept;

private:
    /// @name ipcServer_ - IPC server handle
    std::unique_ptr< isoft::ipc::IPCServer > ipcServer_{nullptr};

    /// @name resourceMan_ - time base resource manager handle
    std::shared_ptr< resource::TBResManager > resourceMan_{nullptr};

    /// @name configManager_ - configuration manager handle
    std::shared_ptr< config::ConfigManager > configManager_{nullptr};

    /// @name timeDomainManager_ - time domain resource manager handle
    std::shared_ptr< timedomain::TDManager > timeDomainManager_{nullptr};

    /// @name sessionMan_ session manager handle
    std::unique_ptr< SessionManager > sessionMan_{nullptr};

    /// @name eventCb_ - event callback function
    ProxyEventHandler eventCb_{};
};

}  // namespace proxy
}  // namespace timebase
}  // namespace internal
}  // namespace tsync
}  // namespace ara

#endif  /// ARA_TSYNC_INTERNAL_TIMEBASE_PROXY_SKELETON_H_