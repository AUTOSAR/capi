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
/// @file       idsm_event_proper.cpp
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
/// @unit_description=
/// @endcode
///
/// ================================================================

#include "idsm_event_proper.h"

#include <mutex>

#include "log/idsm_log.h"
namespace ara {
namespace idsm {
/// @brief Unique instance of event property pool
std::shared_ptr< EventProperPool > EventProperPool::s_SingleInstance_{nullptr};  // NOLINT
/// @brief Singleton thread-safe mutex lock
std::mutex EventProperPool::s_SigleMutex_{};  // NOLINT
/// @brief constructor
/// @exception stack overflow exception
EventProperPool::EventProperPool()
{
    std::ignore = modeMap_.insert(std::pair< ara::core::String, ReportingMode >("BRIEF", ReportingMode::kBrief));
    std::ignore = modeMap_.insert(std::pair< ara::core::String, ReportingMode >("OFF", ReportingMode::kOff));
    std::ignore = modeMap_.insert(std::pair< ara::core::String, ReportingMode >("DETAILED", ReportingMode::kDetailed));
    std::ignore = modeMap_.insert(std::pair< ara::core::String, ReportingMode >("BRIEF_BYPASSING_FILTERS",
                                                                                ReportingMode::kBriefBypassingFilters));
    std::ignore = modeMap_.insert(std::pair< ara::core::String, ReportingMode >(
        "DETAILED_BYPASSING_FILTERS", ReportingMode::kDetailedBypassingFilters));
}
/// @brief Get the unique instance of EventProper
/// @return EventProper object pointer
/// @exception stack overflow exception
/// @code{.isoft}
/// @threadsafety={reentrant}
/// @endcode
std::shared_ptr< EventProperPool > EventProperPool::GetInstance()
{
    if (s_SingleInstance_ == nullptr) {
        std::unique_lock< std::mutex > const lock{s_SigleMutex_};
        if (s_SingleInstance_ == nullptr) {
            s_SingleInstance_.reset(new EventProperPool);
        }
    }
    return s_SingleInstance_;
}
/// @brief Set properties of security event
/// @param eventId security event
/// @param proper properties of security event
/// @exception stack overflow exception
/// @code{.isoft}
/// @threadsafety={unsafe}
/// @endcode
void EventProperPool::SetEventProper(uint16_t const eventId, EventProperPtr const& proper)
{
    properMap_[eventId] = proper;
}
/// @brief Set reporting mode of security event
/// @param eventId security event
/// @param mode reporting mode of security event
/// @return -1 failure, 0 success
/// @exception stack overflow exception
/// @code{.isoft}
/// @threadsafety={unsafe}
/// @endcode
int32_t EventProperPool::SetEventMode(uint16_t const eventId, ReportingMode const mode)
{
    if (properMap_.count(eventId) == 0U) {
        // TODO(person in charge): error handling
        return -1;
    }
    properMap_[eventId]->SetReportMode(mode);
    return 0;
}
/// @brief Set reporting mode of security event
/// @param eventId security event
/// @param mode reporting mode of security event
/// @return -1 failure, 0 success
/// @exception stack overflow exception
/// @code{.isoft}
/// @threadsafety={unsafe}
/// @endcode
int32_t EventProperPool::SetEventMode(uint16_t const eventId, ara::core::String const& mode)
{
    if (properMap_.count(eventId) == 0U) {
        // TODO(person in charge): error handling
        return -1;
    }
    ReportingMode const m{modeMap_[mode]};
    properMap_[eventId]->SetReportMode(m);
    return 0;
}
/// @brief Get reporting mode of security event
/// @param eventId security event
/// @return reporting mode of security event
/// @exception stack overflow exception
/// @code{.isoft}
/// @threadsafety={unsafe}
/// @endcode
ReportingMode EventProperPool::GetEventMode(uint16_t const eventId)
{
    if (properMap_.count(eventId) == 0U) {
        return ReportingMode::kUnknown;
    }
    return properMap_[eventId]->GetReportMode();
}
/// @brief Get properties of security event
/// @param eventId security event
/// @return smart pointer to security event properties
/// @exception stack overflow exception
/// @code{.isoft}
/// @threadsafety={unsafe}
/// @endcode
EventProperPtr EventProperPool::GetEventProper(uint16_t const eventId)
{
    if (properMap_.count(eventId) == 0U) {
        return EventProperPtr{nullptr};
    }
    return properMap_[eventId];
}

}  // namespace idsm
}  // namespace ara