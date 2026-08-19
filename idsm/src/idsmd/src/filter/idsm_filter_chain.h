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
/// @file       idsm_filter_chain.h
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
/// @unit_description=Filter chain implementation
/// @endcode
///
/// ================================================================

#ifndef ARA_IDSM_FILTER_CHAIN_H_
#define ARA_IDSM_FILTER_CHAIN_H_
#include "ara/idsm/internal/event.h"
#include "idsm_aggre_filter.h"
#include "idsm_base_filter.h"
namespace ara {
namespace idsm {
/// @brief Filter chain
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00232
/// @trace_id_dd=DD_IDSM_00672
/// @needwork = ad
/// @endcode
class FilterChain final
{
public:
    /// @brief Use state, sample filter filters events. Aggregation is done in aggregation filter
    /// @param event event to filter
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00233
    /// @trace_id_dd=DD_IDSM_00673
    /// @needwork = ad
    /// @endcode
    void Filter(EventPtr const& event);
    /// @brief Register filter into filter chain
    /// @param filter filter to register
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00234
    /// @trace_id_dd=DD_IDSM_00674
    /// @needwork = ad
    /// @endcode
    void AddFilter(FilterBasePtr const& filter);
    /// @brief Register threshold filter into filter chain
    /// @param filter filter to register
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00235
    /// @trace_id_dd=DD_IDSM_00675
    /// @needwork = ad
    /// @endcode
    void SetThresholdFilter(FilterBasePtr const& filter);
    /// @brief Register aggregation filter into filter chain
    /// @param filter filter to register
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00236
    /// @trace_id_dd=DD_IDSM_00676
    /// @needwork = ad
    /// @endcode
    void SetAggreFilter(std::shared_ptr< AggregationFilter > const& filter) noexcept;
    /// @brief Timer processing
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00237
    /// @trace_id_dd=DD_IDSM_00677
    /// @needwork = ad
    /// @endcode
    void Timerhandler();

public:
    /// @brief parameterless constructor
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00679
    /// @needwork = dda
    /// @endcode
    FilterChain() = default;

private:
    /// @brief Filters after aggregation filter filter security events
    /// @param event security event to filter
    /// @param isAgg whether it is an aggregated event
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00680
    /// @needwork = dda
    /// @endcode
    void _filterSecondHalf(EventPtr const& event, bool const isAgg = false);

private:
    /// @brief aggregation filter
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00681
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< AggregationFilter > aggreFilter_{nullptr};
    /// @brief second half filters
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00682
    /// @needwork = dda
    /// @endcode
    std::list< FilterBasePtr > afterFilter_;
    /// @brief first half filters
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00683
    /// @needwork = dda
    /// @endcode
    std::list< FilterBasePtr > beforeFilter_;
};
/// @brief Type redefinition: define smart pointer type for data structure
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00239
/// @trace_id_dd=DD_IDSM_00684
/// @needwork = ad
/// @endcode
using FilterChainPtr = std::shared_ptr< FilterChain >;

}  // namespace idsm
}  // namespace ara
#endif