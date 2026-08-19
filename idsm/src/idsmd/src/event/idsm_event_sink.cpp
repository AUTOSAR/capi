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
/// @file       idsm_event_sink.cpp
/// @brief      Qualified security event buffer pool
/// @details
/// @date       2023-02-15
/// @author     niuliming
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/idsm/Qualified security event pool
/// @interface_level=module
/// @trace_id_sr=SR_IDSM_0011
/// @unit_name=EventPool
/// @unit_description=
/// @endcode
///
/// ================================================================

#include "idsm_event_sink.h"

#include <mutex>

#include "log/idsm_log.h"
namespace ara {
namespace idsm {
/// @brief Unique instance of event pool
std::shared_ptr< EventPool > EventPool::s_SingleInstance_{nullptr};  // NOLINT
/// @brief Singleton thread-safe mutex lock
std::mutex EventPool::s_SigleMutex_{};
/// @brief Get EventPool instance
/// @return EventPool instance
/// @exception stack overflow exception
/// @code{.isoft}
/// @threadsafety={reentrant}
/// @endcode
std::shared_ptr< EventPool > EventPool::GetInstance()
{
    if (s_SingleInstance_ == nullptr) {
        std::unique_lock< std::mutex > const lock{s_SigleMutex_};
        if (s_SingleInstance_ == nullptr) {
            s_SingleInstance_.reset(new EventPool);
        }
    }
    return s_SingleInstance_;
}
/// @brief Put Event into event buffer
/// @param event event
/// @exception stack overflow exception
/// @code{.isoft}
/// @threadsafety={reentrant}
/// @endcode
void EventPool::Push(EventPtr const& event)
{
    LOG_INFO << "idsm qualified security event generate. event id: " << event->GetEventId()
             << "event counter: " << event->GetCounter() << "event timestamp: " << event->GetTimeStamp()
             << "event context data size: " << event->GetContextDataSize() << "event pool size " << records_.size();
    {
        std::unique_lock< std::mutex > const lck{condiMutex_};
        records_.push_back(event);
        cond_.notify_one();
    }
}
/// @brief Get all events in the event buffer
/// @param events all events in the event buffer
/// @exception stack overflow exception
/// @code{.isoft}
/// @threadsafety={reentrant}
/// @endcode
void EventPool::Get(std::list< EventPtr >& events)
{
    if (!events.empty()) {
        LOG_ERROR << "idsm qualified security event transmit fail. events not empty";
    }
    std::unique_lock< std::mutex > lck{condiMutex_};
    while (records_.empty()) {
        /// @brief Wait for one second each time, then return an empty list
        std::cv_status status = cond_.wait_for(lck, std::chrono::seconds(1));
        if (status == std::cv_status::timeout) {
            return;
        }
    }
    events.swap(records_);
    return;
}

}  // namespace idsm
}  // namespace ara
