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
/// @file       session_manager.h
/// @brief      This file provides session_manager functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/isoft/uds/session_management
/// @interface_level=software
/// @trace_id_sr=1edf8088-f170-41bb-a325-19b81f84c357
/// @unit_name=SessionManager
/// @unit_description=This file provides session_manager functionality
/// @endcode
///
/// ================================================================

#ifndef ISOFT_UDS_SERVER_SESSIONLAYER_H_
#define ISOFT_UDS_SERVER_SESSIONLAYER_H_

#include <memory>

#include "common/request_context.h"
#include "isoft/uds/session_management/session_model.h"
namespace isoft {
namespace uds {
namespace server {

/// @brief Timer class forward declaration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02409
/// @needwork = dd
/// @endcode
class Timer;

/// @brief Session change callback function
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02410
/// @needwork = dd
/// @endcode
using NotifyOfSessionChangeCallback = std::function< void(SessionId, SessionId) >;

/// @brief Session change callback function ID
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02411
/// @needwork = dd
/// @endcode
using NotifyOfSessionChangeCallbackId = std::uint32_t;
/// @brief P2 timer ID
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02412
/// @needwork = dd
/// @endcode
using P2TimerId = std::uint32_t;
/// @brief Timer pointer
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02413
/// @needwork = dd
/// @endcode
using TimerPtr = std::shared_ptr< Timer >;

/// @brief Session class
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02414
/// @needwork = dd
/// @endcode
class Session final
{
public:
    /// @param
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00602
    /// @needwork = dda
    /// @endcode
    Session(Session const&) = delete;
    /// @param
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00603
    /// @needwork = dda
    /// @endcode
    Session& operator=(Session const&) = delete;
    /// @param
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00604
    /// @needwork = dda
    /// @endcode
    Session(Session&&) = delete;
    /// @param
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00605
    /// @needwork = dda
    /// @endcode
    Session& operator=(Session&&) = delete;

public:
    /// @brief Session function
    /// @param currentSessionId Current session ID
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01064
    /// @needwork = dd
    /// @endcode
    explicit Session(SessionId& currentSessionId) noexcept;
    /// @brief Destructor
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00606
    /// @needwork = dda
    /// @endcode
    ~Session() noexcept = default;

    /// @brief Initialization function
    /// @param const Parameter
    /// @return Return code
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01065
    /// @needwork = dd
    /// @endcode
    std::int32_t Initialize(SessionModel const& config) noexcept;

    /// @brief Check
    /// @return Operation result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01066
    /// @needwork = dd
    /// @endcode
    bool Check() const noexcept;

    /// @brief Get name
    /// @return String
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01067
    /// @needwork = dd
    /// @endcode
    std::string GetShortName() const noexcept;

    /// @brief Get P2ServerMax
    /// @return Unsigned integer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01068
    /// @needwork = dd
    /// @endcode
    std::uint16_t GetP2ServerMax() const noexcept;

    /// @brief Get P2StarServerMax
    /// @return Unsigned integer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01069
    /// @needwork = dd
    /// @endcode
    std::uint16_t GetP2StarServerMax() const noexcept;

    /// @brief Get ID
    /// @return Unsigned integer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01070
    /// @needwork = dd
    /// @endcode
    std::uint8_t GetSessionId() const noexcept;

private:
    /// @brief Session model
    /// @name config_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01071
    /// @needwork = dd
    /// @endcode
    SessionModel config_;
    /// @brief Current session ID
    /// @name currentSessionId_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01072
    /// @needwork = dd
    /// @endcode
    SessionId& currentSessionId_;
};

/// @brief DiagnosticSessionControl service is used to enable different diagnostic sessions in the server.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02415
/// @needwork = dd
/// @endcode
class SessionManager final
{
public:
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00607
    /// @needwork = dda
    /// @endcode
    SessionManager(SessionManager const&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00608
    /// @needwork = dda
    /// @endcode
    SessionManager& operator=(SessionManager const&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00609
    /// @needwork = dda
    /// @endcode
    SessionManager(SessionManager&&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00610
    /// @needwork = dda
    /// @endcode
    SessionManager& operator=(SessionManager&&) = delete;

    /// @brief SessionManager function
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01073
    /// @needwork = dd
    /// @endcode
    SessionManager() noexcept;

    /// @brief Destructor
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01074
    /// @needwork = dd
    /// @endcode
    ~SessionManager() noexcept = default;

    /// @brief Initialization function
    /// @return Return code
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01075
    /// @needwork = dd
    /// @endcode
    std::int32_t Initialize(std::set< SessionModel > const& model) noexcept;

    /// @brief Get ID
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01076
    /// @needwork = dd
    /// @endcode
    SessionId GetCurrentSessionId() const noexcept;

    /// @brief Add
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01077
    /// @needwork = dd
    /// @endcode
    NotifyOfSessionChangeCallbackId AddNotifyOfSessionChange(NotifyOfSessionChangeCallback& callback) noexcept;

    /// @brief Remove
    /// @param id Identifier
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01078
    /// @needwork = dd
    /// @endcode
    void RemoveNotifyOfSessionChange(NotifyOfSessionChangeCallbackId id) noexcept;

    /// @brief Set Session
    /// @param id Identifier
    /// @param client Parameter
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01079
    /// @needwork = dd
    /// @endcode
    void SetSession(SessionId id, Address client) noexcept;

    /// @brief Get Session
    /// @param id Identifier
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01080
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< Session > GetSession(SessionId id) noexcept;

    /// @brief Get Client
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01081
    /// @needwork = dd
    /// @endcode
    Address GetClient() const noexcept { return client_; }

private:
    /// @brief Session change callback table
    /// @name notifyOfSessionChangeCallbackTable_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01082
    /// @needwork = dd
    /// @endcode
    std::map< NotifyOfSessionChangeCallbackId, NotifyOfSessionChangeCallback > notifyOfSessionChangeCallbackTable_{};

    /// @brief P2 timer table
    /// @name p2TimerTable_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01083
    /// @needwork = dd
    /// @endcode
    std::map< P2TimerId, TimerPtr > p2TimerTable_{};

    /// @brief Current session ID
    /// @name currentSessionId_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01084
    /// @needwork = dd
    /// @endcode
    SessionId currentSessionId_{0U};

    /// @brief Client address
    /// @name client_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01085
    /// @needwork = dd
    /// @endcode
    Address client_{0U};

    /// @brief Session table
    /// @name table_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01086
    /// @needwork = dd
    /// @endcode
    std::map< SessionId, std::shared_ptr< Session > > table_{};
};

}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif  // ISOFT_UDS_SERVER_SESSIONLAYER_H_