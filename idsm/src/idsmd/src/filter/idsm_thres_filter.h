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
/// @file       idsm_thres_filter.h
/// @brief      Threshold filter implementation
/// @details
/// @date       2023-02-15
/// @author     niuliming
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/idsm/Filter chain
/// @interface_level=unit
/// @trace_id_sr=SR_IDSM_0010
/// @unit_name=ThresholdFilter
/// @unit_description=Threshold filter implementation
/// @endcode
///
/// ================================================================

#ifndef ARA_IDSM_THRES_FILTER_H_
#define ARA_IDSM_THRES_FILTER_H_
#include "ara/idsm/internal/event.h"
#include "idsm_base_filter.h"
namespace ara {
namespace idsm {

/// @brief Default threshold of threshold filter
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00228
/// @trace_id_dd=DD_IDSM_00656
/// @needwork = dd
/// @endcode
static uint64_t const kThresholdConst{65536U};
/// @brief Threshold period time interval
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00228
/// @trace_id_dd=DD_IDSM_00657
/// @needwork = dd
/// @endcode
static time_t const kThresInterval{1024};
/// @brief Threshold filter
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_IDSM_00228
/// @trace_id_dd=DD_IDSM_00658
/// @needwork = ad
/// @endcode
class ThresholdFilter : public FilterBase
{
public:
    /// @brief Filter interface
    /// @param event event to filter
    /// @return filter result true: passed, false: not passed
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00659
    /// @needwork = dda
    /// @endcode
    bool Filter(EventPtr const& event) override;

public:
    /// @brief parameterless constructor
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// export_level=/idsm/Filter chain
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00229
    /// @trace_id_dd=DD_IDSM_00660
    /// @needwork = ad
    /// @endcode
    ThresholdFilter() noexcept : ThresholdFilter{0U, kThresholdConst, 0, kThresInterval} {}
    /// @brief parameterized constructor
    /// @param intervals time interval
    /// @param threshold threshold
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @threadsafety={unsafe}
    /// export_level=/idsm/Filter chain
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00230
    /// @trace_id_dd=DD_IDSM_00661
    /// @needwork = ad
    /// @endcode
    ThresholdFilter(time_t const intervals, uint64_t const threshold) noexcept
        : ThresholdFilter{0U, threshold, 0, intervals}
    {
    }

public:
    /// @brief copy constructor
    /// @param filter object to copy
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00662
    /// @needwork = dda
    /// @endcode
    ThresholdFilter(ThresholdFilter const& filter) noexcept = default;
    /// @brief move constructor
    /// @param filter object to move
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00663
    /// @needwork = dda
    /// @endcode
    ThresholdFilter(ThresholdFilter&& filter) noexcept = default;
    /// @brief copy assignment operator
    /// @param filter object to copy in assignment operator
    /// @return left operand of assignment operator
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00664
    /// @needwork = dda
    /// @endcode
    ThresholdFilter& operator=(ThresholdFilter const& filter) noexcept = default;
    /// @brief move assignment operator
    /// @param filter object to move in assignment operator
    /// @return left operand of assignment operator
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00665
    /// @needwork = dda
    /// @endcode
    ThresholdFilter& operator=(ThresholdFilter&& filter) noexcept = default;
    /// @brief destructor
    /// @exception stack overflow exception
    /// @code{.isoft}
    /// export_level=/idsm/Filter chain
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00231
    /// @trace_id_dd=DD_IDSM_00666
    /// @needwork = ad
    /// @endcode
    ~ThresholdFilter() override = default;

private:
    /// @brief parameterized constructor
    /// @param sum initial value of counter
    /// @param threshold threshold
    /// @param timeStamp start timestamp of threshold period
    /// @param intervals threshold period
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00667
    /// @needwork = dda
    /// @endcode
    ThresholdFilter(uint64_t const sum,
                    uint64_t const threshold,
                    time_t const timeStamp,
                    time_t const intervals) noexcept
        : counterSum_{sum}, thresholdNum_{threshold}, startTimestamp_{timeStamp}, timeInterval_{intervals}
    {
    }

private:
    /// @brief accumulated sum of counter for all events within time interval
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00668
    /// @needwork = dda
    /// @endcode
    uint64_t counterSum_;
    /// @brief threshold
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00669
    /// @needwork = dda
    /// @endcode
    uint64_t thresholdNum_;
    /// @brief start timestamp of current time interval
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00670
    /// @needwork = dda
    /// @endcode
    time_t startTimestamp_;
    /// @brief time interval
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IDSM_00000
    /// @trace_id_dd=DD_IDSM_00671
    /// @needwork = dda
    /// @endcode
    time_t timeInterval_;
};

}  // namespace idsm
}  // namespace ara
#endif
