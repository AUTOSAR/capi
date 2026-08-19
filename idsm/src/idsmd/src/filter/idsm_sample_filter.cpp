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
/// @file       idsm_sample_filter.cpp
/// @brief      Sampling filter implementation
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
/// @trace_id_sr=SR_IDSM_0008
/// @unit_name=SampleFilter
/// @unit_description=
/// @endcode
///
/// ================================================================

#include "idsm_sample_filter.h"

#include "event/idsm_event_memory.h"
#include "log/idsm_log.h"

namespace ara {
namespace idsm {
/// @brief Filter interface
/// @param event event to filter
/// @return filter result true: passed, false: not passed
/// @exception stack overflow exception
/// @code{.isoft}
/// @threadsafety={unsafe}
/// export_level=/
/// @endcode
bool SampleFilter::Filter(EventPtr const& event) noexcept
{
    static_cast< void >(event);
    if (circleNum_ == 0U) {
        return false;
    }

    if (eventSum_ == circleNum_) {
        eventSum_ = 0U;
    }
    if (eventSum_ == 0U) {
        eventSum_ += 1U;
        return true;
    }
    eventSum_ += 1U;
    LOG_WARN << "filter chain check fail. filter: sample filter, event id: " << event->GetEventId()
             << ", counter:" << event->GetCounter();
    return false;
}
}  // namespace idsm
}  // namespace ara
