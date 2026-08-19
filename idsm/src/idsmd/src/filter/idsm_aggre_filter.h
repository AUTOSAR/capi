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
/// @file       idsm_aggre_filter.h
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
/// @unit_description=Aggregation filter implementation
/// @endcode
///
/// ================================================================

#ifndef ARA_IDSM_AGGRE_FILTER_H_
#define ARA_IDSM_AGGRE_FILTER_H_
#include "ara/idsm/internal/event.h"
#include "idsm_base_filter.h"
namespace ara {
namespace idsm {
/// @brief Aggregation period time interval
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00225
/// @trace_id_dd=DD_IDSM_00637
/// @needwork = dd
/// @endcode
static time_t const kAggInterval{1024};
/// @brief Enumeration class for context data source
/// @code{.isoft}
/// export_level=/idsm/Filter chain
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00224
/// @trace_id_dd=DD_IDSM_00638
/// @needwork = ad
/// @endcode
enum class ContextDataSource : uint8_t
{
    /// @brief Aggregation filter uses the first context data within aggregation period
    kUseFirstContextData = 1,
    /// @brief Aggregation filter uses the last context data within aggregation period
    kUseLastContextData
};

/// @brief Aggregation filter
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00225
/// @trace_id_dd=DD_IDSM_00639
/// @needwork = ad
/// @endcode
class AggregationFilter : public FilterBase
{
public:
    /// @brief Filter interface. When not in aggregation period, set the start point of aggregation period
    /// @param event event to filter
    /// @return true indicates passed filter check, otherwise not passed
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00640
    /// @needwork = dda
    /// @endcode
    bool Filter(EventPtr const& event) noexcept override;
    /// @brief Get aggregated event
    /// @return aggregated event
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00641
    /// @needwork = dda
    /// @endcode
    EventPtr GetEvent() noexcept;
    /// @brief Determine if aggregation period ends. Set end flag when ended
    /// @return true if aggregation period ends, otherwise false
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00642
    /// @needwork = dda
    /// @endcode
    bool IsArrivedTime();

public:
    /// @brief parameterized constructor
    /// @param intervals aggregation period
    /// @param source context data source of aggregated event
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// export_level=/idsm/Filter chain
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00643
    /// @needwork = dda
    /// @endcode
    AggregationFilter(time_t const intervals, ContextDataSource const source) noexcept
        : AggregationFilter{intervals, 0, source, nullptr}
    {
    }
    /// @brief parameterless constructor
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// export_level=/idsm/Filter chain
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00226
    /// @trace_id_dd=DD_IDSM_00644
    /// @needwork = ad
    /// @endcode
    AggregationFilter() noexcept : AggregationFilter{kAggInterval, 0, ContextDataSource::kUseFirstContextData, nullptr}
    {
    }

public:
    /// @brief copy constructor
    /// @param filter object to copy
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00645
    /// @needwork = dda
    /// @endcode
    AggregationFilter(AggregationFilter const& filter) noexcept = default;
    /// @brief move constructor
    /// @param filter object to move
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00646
    /// @needwork = dda
    /// @endcode
    AggregationFilter(AggregationFilter&& filter) noexcept = default;
    /// @brief copy assignment operator
    /// @param filter object to copy in assignment operator
    /// @return left operand of assignment operator
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00647
    /// @needwork = dda
    /// @endcode
    AggregationFilter& operator=(AggregationFilter const& filter) noexcept = default;
    /// @brief move assignment operator
    /// @param filter object to move in assignment operator
    /// @return left operand of assignment operator
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00648
    /// @needwork = dda
    /// @endcode
    AggregationFilter& operator=(AggregationFilter&& filter) noexcept = default;
    /// @brief destructor
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// export_level=/idsm/Filter chain
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00227
    /// @trace_id_dd=DD_IDSM_00649
    /// @needwork = ad
    /// @endcode
    ~AggregationFilter() override = default;

private:
    /// @brief Adjust memory used by context of aggregated events
    /// @param oldMem current context size of aggregated event
    /// @param newMem new context size of aggregated event to update
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00650
    /// @needwork = dda
    /// @endcode
    static void AdjustCtxMem(uint64_t const oldMem, uint64_t const newMem);

private:
    /// @brief parameterized constructor
    /// @param intervals aggregation period
    /// @param timeStamp start timestamp of aggregation period
    /// @param source context policy of aggregated event
    /// @param event aggregated event
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00651
    /// @needwork = dda
    /// @endcode
    AggregationFilter(time_t const intervals,
                      time_t const timeStamp,
                      ContextDataSource const source,
                      EventPtr event) noexcept
        : timeInterval_{intervals}, startTimestamp_{timeStamp}, dataSource_{source}, aggEvent_{std::move(event)}
    {
    }

private:
    /// @name timeInterval_
    /// @brief time interval
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00652
    /// @needwork = dda
    /// @endcode
    time_t timeInterval_;
    /// @name startTimestamp_
    /// @brief start timestamp of aggregation period, 0 means aggregation period not started
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00653
    /// @needwork = dda
    /// @endcode
    time_t startTimestamp_;
    /// @name dataSource_
    /// @brief policy for using context data when aggregating events
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00654
    /// @needwork = dda
    /// @endcode
    ContextDataSource dataSource_;
    /// @name aggEvent_
    /// @brief aggregated event
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00655
    /// @needwork = dda
    /// @endcode
    EventPtr aggEvent_;
};

}  // namespace idsm
}  // namespace ara
#endif