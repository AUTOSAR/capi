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
/// @file       idsm_event_sink.h
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
/// @unit_description=Qualified security event buffer pool
/// @endcode
///
/// ================================================================

#ifndef ARA_IDSM_EVENT_SINK_H_
#define ARA_IDSM_EVENT_SINK_H_
#include <condition_variable>
#include <list>
#include <memory>
#include <mutex>

#include "ara/idsm/internal/event.h"

namespace ara {
namespace idsm {
/// @brief Security event pool
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00137
/// @trace_id_dd=DD_IDSM_00301
/// @needwork = ad
/// @endcode
class EventPool final
{
public:
    /// @brief Get EventPool instance
    /// @return EventPool instance
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @threadsafety={reentrant}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00138
    /// @trace_id_dd=DD_IDSM_00302
    /// @needwork = ad
    /// @endcode
    static std::shared_ptr< EventPool > GetInstance();
    /// @brief Put Event into event buffer
    /// @param event event
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @threadsafety={reentrant}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00139
    /// @trace_id_dd=DD_IDSM_00303
    /// @needwork = ad
    /// @endcode
    void Push(EventPtr const& event);
    /// @brief Get all events in the event buffer
    /// @param events all events in the event buffer
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @threadsafety={reentrant}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00140
    /// @trace_id_dd=DD_IDSM_00304
    /// @needwork = ad
    /// @endcode
    void Get(std::list< EventPtr >& events);
    /// @brief Reset security event pool
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00141
    /// @trace_id_dd=DD_IDSM_00305
    /// @needwork = ad
    /// @endcode
    void Reset() noexcept { records_.clear(); }

public:
    /// @brief destructor
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00306
    /// @needwork = dda
    /// @endcode
    ~EventPool() = default;

public:
    /// @brief copy constructor
    /// @param pool object to copy
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00307
    /// @needwork = dda
    /// @endcode
    EventPool(EventPool const& pool) = delete;
    /// @brief move constructor
    /// @param pool object to move
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00308
    /// @needwork = dda
    /// @endcode
    EventPool(EventPool&& pool) = delete;
    /// @brief copy assignment operator
    /// @param pool object to copy in assignment operator
    /// @return left operand of assignment operator
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00309
    /// @needwork = dda
    /// @endcode
    EventPool& operator=(EventPool const& pool) = delete;
    /// @brief move assignment operator
    /// @param pool object to move in assignment operator
    /// @return left operand of assignment operator
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00310
    /// @needwork = dda
    /// @endcode
    EventPool& operator=(EventPool&& pool) = delete;

private:
    /// @brief constructor
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00311
    /// @needwork = dda
    /// @endcode
    EventPool() noexcept = default;

private:
    /// @brief Unique instance of event pool
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00312
    /// @needwork = dda
    /// @endcode
    static std::shared_ptr< EventPool > s_SingleInstance_;
    /// @brief Singleton thread-safe mutex lock
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00313
    /// @needwork = dda
    /// @endcode
    static std::mutex s_SigleMutex_;
    /// @brief Condition variable for synchronizing producer and consumer threads
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00314
    /// @needwork = dda
    /// @endcode
    std::condition_variable cond_;
    /// @brief Mutex for condition variable
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00315
    /// @needwork = dda
    /// @endcode
    std::mutex condiMutex_;
    /// @brief Cached qualified security events
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00316
    /// @needwork = dda
    /// @endcode
    std::list< EventPtr > records_;
};

}  // namespace idsm
}  // namespace ara

#endif