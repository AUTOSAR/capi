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
/// @file       idsm_filter_chain.cpp
/// @brief      Filter chain implementation
/// @details
/// @date       2023-02-15
/// @author     niuliming
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/idsm/Filter chain
/// @interface_level=module
/// @trace_id_sr=SR_IDSM_0006
/// @unit_name=FilterChain
/// @unit_description=
/// @endcode
///
/// ================================================================

#include "idsm_filter_chain.h"

#include <chrono>

#include "event/idsm_event_memory.h"
#include "event/idsm_event_sink.h"
#include "log/idsm_log.h"
#include "server/idsm_fg_state.h"
namespace ara {
namespace idsm {
/// @brief Use state, sample filter filters events. Aggregation is done in aggregation filter
/// @param event event to filter
/// @exception stack overflow exception
/// @code{.isoft}
/// @threadsafety={unsafe}
/// @endcode
void FilterChain::Filter(EventPtr const& event)
{
    /// @details: Filter checks before aggregation filter
    for (auto& ele : beforeFilter_) {
        if (!ele->Filter(event)) {
            return;
        }
    }

    /// @details Aggregation filter check
    if (aggreFilter_ != nullptr) {
        /// @details Check if aggregation period ends
        if (aggreFilter_->IsArrivedTime()) {
            /// @details Process aggregated event
            _filterSecondHalf(aggreFilter_->GetEvent(), true);
        }
        /// @details: 1. Aggregation period not started, this time point not in aggregation period
        /// @details: 2. This time point is within aggregation period but period not ended
        /// @details: Aggregation filter processes security event
        std::ignore = aggreFilter_->Filter(event);
        return;
    }

    /// @details Threshold filter check
    _filterSecondHalf(event);
}
/// @brief Register filter into filter chain
/// @param filter filter to register
/// @exception stack overflow exception
/// @code{.isoft}
/// @threadsafety={unsafe}
/// @endcode
void FilterChain::AddFilter(FilterBasePtr const& filter) { beforeFilter_.push_back(filter); }
/// @brief Register threshold filter into filter chain
/// @param filter filter to register
/// @exception stack overflow exception
/// @code{.isoft}
/// @threadsafety={unsafe}
/// @endcode
void FilterChain::SetThresholdFilter(FilterBasePtr const& filter) { afterFilter_.push_back(filter); }
/// @brief Register aggregation filter into filter chain
/// @param filter filter to register
/// @exception stack overflow exception
/// @code{.isoft}
/// @threadsafety={unsafe}
/// @endcode
void FilterChain::SetAggreFilter(std::shared_ptr< AggregationFilter > const& filter) noexcept { aggreFilter_ = filter; }
/// @brief Timer processing
/// @exception stack overflow exception
/// @code{.isoft}
/// @threadsafety={unsafe}
/// @endcode
void FilterChain::Timerhandler()
{
    /// @details No aggregation filter
    if (aggreFilter_ == nullptr) {
        return;
    }
    /// @details This time point not in aggregation period or aggregation period not ended
    if (!aggreFilter_->IsArrivedTime()) {
        return;
    }
    /// @details Process aggregated event
    EventPtr const event{aggreFilter_->GetEvent()};
    _filterSecondHalf(event, true);
}
/// @brief Filters after aggregation filter filter security events
/// @param event security event to filter
/// @param isAgg whether the security event to filter is an aggregated event
/// @exception stack overflow exception
void FilterChain::_filterSecondHalf(EventPtr const& event, bool const isAgg)
{
    if (event.get() == nullptr) {
        LOG_WARN << "aggregation filter have a empty event.";
        return;
    }

    /// @details: Threshold filter check
    for (auto& ele : afterFilter_) {
        if (!ele->Filter(event)) {
            /// @details Threshold filter check failed, release memory statistics of aggregated event
            if (isAgg) {
                uint32_t const eventSize{EVENT_MEM_SIZE};
                uint64_t const ctxSize{event->GetContextDataSize()};
                EventMemPool::GetInstance()->ReleaseMem(eventSize, ctxSize);
            }
            return;
        }
    }

    /// @details Filter chain check passed, becomes qualified security event
    EventPool::GetInstance()->Push(event);
    if (!isAgg) {
        /// @details When non-aggregated event, update memory statistics of security event
        uint16_t const eventSize{EVENT_MEM_SIZE};
        uint64_t const ctxSize{event->GetContextDataSize()};
        EventMemPool::GetInstance()->AllocMem(eventSize, ctxSize);
    }
}

}  // namespace idsm
}  // namespace ara