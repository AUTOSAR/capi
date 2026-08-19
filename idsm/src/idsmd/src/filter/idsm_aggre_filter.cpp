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
/// @file       idsm_aggre_filter.cpp
/// @brief      Aggregation filter implementation
/// @details
/// @date       2024-09-18
/// @author     niuliming
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/idsm/Filter chain
/// @interface_level=unit
/// @trace_id_sr=SR_IDSM_0009
/// @unit_name=AggregationFilter
/// @unit_description=
/// @endcode
///
/// ================================================================

#include "idsm_aggre_filter.h"

#include "event/idsm_event_memory.h"
#include "log/idsm_log.h"

namespace ara {
namespace idsm {
/// @brief Filter interface. When not in aggregation period, set the start point of aggregation period
/// @param event event to filter
/// @return true indicates passed filter check, otherwise not passed
/// @code{.isoft}
/// @threadsafety={unsafe}
/// export_level=/
/// @endcode
bool AggregationFilter::Filter(EventPtr const& event) noexcept
{
    /// @details Start a new aggregation period
    if (startTimestamp_ == 0) {
        std::chrono::steady_clock::duration const drNanSecond{std::chrono::steady_clock::now().time_since_epoch()};
        startTimestamp_ = (std::chrono::duration_cast< std::chrono::seconds >(drNanSecond)).count();
    }

    uint64_t const ctxNewSize{event->GetContextDataSize()};
    /// @details First security event within aggregation period
    if (aggEvent_.use_count() == 0) {
        aggEvent_ = event;
        uint16_t const eventSize{EVENT_MEM_SIZE};
        EventMemPool::GetInstance()->AllocMem(eventSize, ctxNewSize);
        return false;
    }

    /// @details Accumulate event counter of aggregated events
    uint32_t counterSum{event->GetCounter()};
    counterSum += aggEvent_->GetCounter();
    aggEvent_->SetCounter(static_cast< uint16_t >(counterSum));

    ///@details Update context data of aggregated events according to policy
    if (event->GetContextDataSize() != 0U) {
        /// @details Use the first context data within aggregation period
        if (dataSource_ == ContextDataSource::kUseFirstContextData) {
            if (aggEvent_->GetContextDataSize() == 0U) {
                ContextDataType ctxData;
                event->GetContextData(ctxData);
                aggEvent_->SetContextData(ctxData);
                AdjustCtxMem(0U, ctxNewSize);
            }
        }
        /// @details Use the last context data within aggregation period
        if (dataSource_ == ContextDataSource::kUseLastContextData) {
            uint64_t const ctxOldSize{aggEvent_->GetContextDataSize()};
            ContextDataType ctxData;
            event->GetContextData(ctxData);
            aggEvent_->SetContextData(ctxData);
            AdjustCtxMem(ctxOldSize, ctxNewSize);
        }
    }
    return false;
}
/// @brief Get aggregated event
/// @return aggregated event
/// @code{.isoft}
/// @threadsafety={unsafe}
/// export_level=/
/// @endcode
EventPtr AggregationFilter::GetEvent() noexcept
{
    /// @details Return aggregated event and reset aggregated event to empty
    EventPtr ret{aggEvent_};
    aggEvent_.reset();
    return ret;
}
/// @brief Determine if aggregation period ends. Set end flag when ended
/// @return true if aggregation period ends, otherwise false
/// @exception stack overflow exception
/// @code{.isoft}
/// @threadsafety={unsafe}
/// export_level=/
/// @endcode
bool AggregationFilter::IsArrivedTime()
{
    /// @details This time point does not belong to any aggregation period
    if (startTimestamp_ == 0) {
        return false;
    }

    bool ret{false};
    std::chrono::steady_clock::duration const drNanSecond{std::chrono::steady_clock::now().time_since_epoch()};
    time_t const currentTimestamp{(std::chrono::duration_cast< std::chrono::seconds >(drNanSecond)).count()};
    /// @details After aggregation period ends, do not proactively start the next aggregation period
    if ((currentTimestamp - startTimestamp_) >= timeInterval_) {
        ret             = true;
        startTimestamp_ = 0;
    }
    return ret;
}
/// @brief Adjust memory used by context of aggregated events
/// @param oldMem current context size of aggregated event
/// @param newMem new context size of aggregated event to update
/// @exception stack overflow exception
void AggregationFilter::AdjustCtxMem(uint64_t const oldMem, uint64_t const newMem)
{
    if (oldMem == newMem) {
        return;
    }
    if (oldMem < newMem) {
        EventMemPool::GetInstance()->AllocMem(0U, newMem - oldMem);
        return;
    }
    EventMemPool::GetInstance()->ReleaseMem(0U, oldMem - newMem);
}

}  // namespace idsm
}  // namespace ara
