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
/// @file       sessionmanager.h
/// @brief      time base proxy session management class
/// @details
/// @date       2023-02-01
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_TSYNC_INTERNAL_TIMEBASE_PROXY_SESSION_MANAGER_H_
#define ARA_TSYNC_INTERNAL_TIMEBASE_PROXY_SESSION_MANAGER_H_

#include <ara/core/map.h>
#include <ara/core/set.h>
#include <ara/core/string_view.h>
#include <isoft/ipccpp/client.h>

#include <cstdint>
#include <memory>

#include "ara/tsync/internal/common.h"
#include "ara/tsync/internal/timebase/proxy/message.h"
#include "ara/tsync/internal/timebase/proxy/proxyeventtype.h"
#include "ara/tsync/internal/timebase/proxy/session.h"
#include "ara/tsync/internal/timebase/proxy/sessioncompare.h"

namespace ara {
namespace tsync {
namespace internal {
namespace timebase {
namespace proxy {

/// @brief SessionManager
class SessionManager final
{
public:
    /// @brief constructor
    SessionManager() = default;

    /// @brief destructor
    ~SessionManager() = default;

    /// @brief copy constructor is prohibited
    /// @param other - other object
    SessionManager(SessionManager const &other) = delete;

    /// @brief copy is prohibited
    /// @param other - other object
    /// @return reference to class object
    SessionManager &operator=(SessionManager const &other) = delete;

    /// @brief move constructor
    /// @param other an existing rvalue reference.
    SessionManager(SessionManager &&) noexcept = default;

    /// @brief '=' operator overload
    /// @param other an existing rvalue reference.
    /// @return reference to class object
    SessionManager &operator=(SessionManager &&) &noexcept = default;

    /// @brief service ID
    using ServiceId = std::uint16_t;

    /// @brief IPC session ID
    using SessionId = Session::Id;
    /// @brief Session type
    using SessionType = Session::Type;
    /// @brief SessionList type
    using SessionList = ara::core::Set< std::unique_ptr< Session >, SessionCompare >;

    /// @brief service and session map type
    using SessionMap = ara::core::Map< ServiceId, SessionList >;

    /// @brief callback function type
    using OnlineHandler = std::function< void(ServiceId serviceId) >;

    /// @brief traverse all SessionIds that meet the criteria based on ServiceId and session type, calling the user's callback function for each found.
    /// @note The user callback function must return 0, otherwise traversal will be terminated.
    using SessionTraverseHandler
        = std::function< std::int32_t(ServiceId serviceId, SessionType const type, SessionId kSid) >;

    /// @brief service online notification function, called when a service comes online for the first time (a session joins).
    /// @param cb - callback function
    void OnServiceOnline(OnlineHandler const &cb) noexcept { onlineCb_ = cb; }

    /// @brief service offline notification function, called when a service goes offline (all sessions end).
    /// @param cb - callback function
    void OnServiceOffline(OnlineHandler const &cb) noexcept { offlineCb_ = cb; }

    /// @brief add session
    /// @param serviceId - service id
    /// @param type - type
    /// @param sessionId - IPC session ID
    /// @code{.isoft}
    /// interface_level=component
    /// @endcode
    void AddSession(ServiceId serviceId, Message::Type const type, SessionId const sessionId) noexcept
    {
        SessionMap::iterator const sessionMapIt{sessionMaps_.find(serviceId)};
        /// time base found
        std::unique_ptr< Session > s{std::make_unique< Session >(sessionId)};
        if (sessionMaps_.end() != sessionMapIt) {
            /// find sessionId
            SessionList &sessionList{sessionMapIt->second};
            SessionList::iterator const slIt{sessionList.find(s)};
            /// if found, set type flag
            if (sessionList.end() != slIt) {
                (*slIt)->SetTypeFlag(type, true);
                /// if not found, create session
            } else {
                s->SetTypeFlag(type, true);
                static_cast< void >(sessionList.emplace(std::move(s)));
            }
            /// if not found time base, create it
        } else {
            SessionList sl;
            s->SetTypeFlag(type, true);
            static_cast< void >(sl.emplace(std::move(s)));
            ServiceId const serviceIdTmp{serviceId};
            static_cast< void >(sessionMaps_.emplace(std::make_pair(serviceId, std::move(sl))));
            /// The first session established for this timebase, call the online notification function
            if (nullptr != onlineCb_) {
                onlineCb_(serviceIdTmp);
            }
        }
        return;
    }

    /// @brief delete a session under a certain service/message type, equivalent to unregistering notification of that type
    /// @param serviceId - service id
    /// @param type - type
    /// @param sessionId - IPC session ID
    void DelSession(ServiceId const serviceId, Message::Type const type, SessionId const sessionId) noexcept
    {
        SessionMap::iterator const sessionMapIt{sessionMaps_.find(serviceId)};
        /// time base not found
        if (sessionMaps_.end() == sessionMapIt) {
            return;
        }

        /// find sessionId
        SessionList &sessionList{sessionMapIt->second};
        SessionList const &sessionListConst{sessionList};
        std::unique_ptr< Session > const s{std::make_unique< Session >(sessionId)};
        SessionList::iterator const slIt{sessionListConst.find(s)};
        /// if found, set type flag to false
        if (sessionList.end() != slIt) {
            (*slIt)->SetTypeFlag(type, false);
        }
        /// if not found, return
        return;
    }

    /// @brief clear sessions of all types when the client exits
    /// @param sessionId - IPC session ID
    void ClearSession(SessionId const sessionId) noexcept
    {
        ara::core::Vector< ServiceId > rmList{};
        SessionMap::iterator itm{sessionMaps_.begin()};
        for (; itm != sessionMaps_.end(); itm++) {
            SessionList &sl{itm->second};
            std::unique_ptr< Session > const s{std::make_unique< Session >(sessionId)};
            SessionList const &sl2{sl};
            SessionList::iterator const itl{sl2.find(s)};
            if (itl != sl.cend()) {
                static_cast< void >(sl.erase(itl));
            }

            // If all sessions under the current serviceId are empty, then call the Offline callback function
            if (sl.empty()) {
                if (offlineCb_ != nullptr) {
                    offlineCb_(itm->first);
                }
                rmList.emplace_back(itm->first);
            }
        }

        /// Delete unnecessary time bases
        for (ara::core::Vector< ServiceId >::const_iterator itr{rmList.cbegin()}; itr != rmList.cend(); ++itr) {
            static_cast< void >(sessionMaps_.erase(*itr));
        }

        return;
    }

    /// @brief get all session IDs
    /// @param sidList - session ID list
    void GetAllSessionId(ara::core::Set< SessionId > &sidList) noexcept
    {
        for (SessionMap::iterator itm{sessionMaps_.begin()}; itm != sessionMaps_.end(); itm++) {
            SessionList &sl{itm->second};
            for (SessionList::const_iterator itl{sl.cbegin()}; itl != sl.cend(); itl++) {
                static_cast< void >(sidList.emplace((*itl)->kSid));
            }
        }
    }

    /// @brief traverse all session handles that meet the criteria based on serviceId and message type, calling the user's callback function for each found.
    /// @note The user callback function must return 0, otherwise traversal will be terminated.
    /// @param serviceId - service ID
    /// @param type - type
    /// @param cb - callback function
    /// @return callback result
    std::int32_t TraverseSession(ServiceId const serviceId,
                                 Message::Type const type,
                                 SessionTraverseHandler const &cb) noexcept
    {
        std::int32_t ret{0};
        if (nullptr == cb) {
            return 0;
        }

        SessionMap::iterator const sessionMapIt{sessionMaps_.find(serviceId)};
        /// time base not found
        if (sessionMaps_.end() == sessionMapIt) {
            return 0;
        }

        /// find sessionId
        SessionList const &sessionList{sessionMapIt->second};
        for (SessionList::const_iterator it{sessionList.cbegin()}; it != sessionList.cend(); it++) {
            Session *const s{(*it).get()};
            if (nullptr == s) {
                continue;
            }
            if (true != s->GetTypeFlag(type)) {
                continue;
            }
            if (0 != cb(serviceId, type, s->kSid)) {
                ret = kRET_E1;
                break;
            }
        }

        return ret;
    }

private:
    /// @name sessionMaps_ - the map of serviceId and SessionList
    SessionMap sessionMaps_;

    /// @name onlineCb_
    OnlineHandler onlineCb_{nullptr};
    /// @name offlineCb_
    OnlineHandler offlineCb_{nullptr};
};

}  // namespace proxy
}  // namespace timebase
}  // namespace internal
}  // namespace tsync
}  // namespace ara

#endif  /// ARA_TSYNC_INTERNAL_TIMEBASE_PROXY_SESSION_MANAGER_H_
