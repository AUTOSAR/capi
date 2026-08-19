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
/// @file       idsm_event_proper.h
/// @brief      Security event static property pool
/// @details
/// @date       2023-02-15
/// @author     niuliming
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/idsm/Security event static property pool
/// @interface_level=module
/// @trace_id_sr=SR_IDSM_0004
/// @unit_name=EventProperPool
/// @unit_description=Security event static property pool
/// @endcode
///
/// ================================================================

#ifndef IDSM_EVENT_PROPER_H_
#define IDSM_EVENT_PROPER_H_
#include <ara/core/map.h>
#include <ara/core/string.h>

#include <condition_variable>
#include <memory>
#include <mutex>

#include "ara/idsm/internal/event.h"

namespace ara {
namespace idsm {
/// @brief Type redefinition: define smart pointer type for data structure
using EventProperPtr = std::shared_ptr< EventProper >;
/// @brief Security event property pool
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00166
/// @trace_id_dd=DD_IDSM_00391
/// @needwork = ad
/// @endcode
class EventProperPool final
{
public:
    /// @brief Get the unique instance of EventProper
    /// @return EventProper object pointer
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @threadsafety={reentrant}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00167
    /// @trace_id_dd=DD_IDSM_00392
    /// @needwork = ad
    /// @endcode
    static std::shared_ptr< EventProperPool > GetInstance();
    /// @brief Set properties of security event
    /// @param eventId security event
    /// @param proper properties of security event
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00168
    /// @trace_id_dd=DD_IDSM_00393
    /// @needwork = ad
    /// @endcode
    void SetEventProper(uint16_t const eventId, EventProperPtr const& proper);
    /// @brief Set reporting mode of security event
    /// @param eventId security event
    /// @param mode reporting mode of security event
    /// @return -1 failure, 0 success
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00169
    /// @trace_id_dd=DD_IDSM_00394
    /// @needwork = ad
    /// @endcode
    int32_t SetEventMode(uint16_t const eventId, ReportingMode const mode);
    /// @brief Set reporting mode of security event
    /// @param eventId security event
    /// @param mode reporting mode of security event
    /// @return -1 failure, 0 success
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00170
    /// @trace_id_dd=DD_IDSM_00395
    /// @needwork = ad
    /// @endcode
    int32_t SetEventMode(uint16_t const eventId, ara::core::String const& mode);
    /// @brief Get reporting mode of security event
    /// @param eventId security event
    /// @return reporting mode of security event
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00171
    /// @trace_id_dd=DD_IDSM_00396
    /// @needwork = ad
    /// @endcode
    ReportingMode GetEventMode(uint16_t const eventId);
    /// @brief Get properties of security event
    /// @param eventId security event
    /// @return smart pointer to security event properties
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00172
    /// @trace_id_dd=DD_IDSM_00397
    /// @needwork = ad
    /// @endcode
    EventProperPtr GetEventProper(uint16_t const eventId);
    /// @brief Reset security event property pool
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00173
    /// @trace_id_dd=DD_IDSM_00398
    /// @needwork = ad
    /// @endcode
    void Reset() noexcept { properMap_.clear(); }

public:
    /// @brief destructor
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00399
    /// @needwork = dda
    /// @endcode
    ~EventProperPool() = default;

public:
    /// @brief copy constructor
    /// @param pool object to copy
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00400
    /// @needwork = dda
    /// @endcode
    EventProperPool(EventProperPool const& pool) = delete;
    /// @brief move constructor
    /// @param pool object to move
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00401
    /// @needwork = dda
    /// @endcode
    EventProperPool(EventProperPool&& pool) = delete;
    /// @brief copy assignment operator
    /// @param pool object to copy in assignment operator
    /// @return left operand of assignment operator
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00402
    /// @needwork = dda
    /// @endcode
    EventProperPool& operator=(EventProperPool const& pool) = delete;
    /// @brief move assignment operator
    /// @param pool object to move in assignment operator
    /// @return left operand of assignment operator
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00403
    /// @needwork = dda
    /// @endcode
    EventProperPool& operator=(EventProperPool&& pool) = delete;

private:
    /// @brief constructor
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00404
    /// @needwork = dda
    /// @endcode
    EventProperPool();

private:
    /// @brief Unique instance of event property pool
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00405
    /// @needwork = dda
    /// @endcode
    static std::shared_ptr< EventProperPool > s_SingleInstance_;
    /// @brief Singleton thread-safe mutex lock
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00406
    /// @needwork = dda
    /// @endcode
    static std::mutex s_SigleMutex_;
    /// @brief Mapping between event Id and event context property. key is event type Id
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00407
    /// @needwork = dda
    /// @endcode
    ara::core::Map< uint16_t, EventProperPtr > properMap_{};
    /// @brief Reporting mode format conversion string-->ReportingMode
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00408
    /// @needwork = dda
    /// @endcode
    ara::core::Map< ara::core::String, ReportingMode > modeMap_{};
};

}  // namespace idsm
}  // namespace ara

#endif