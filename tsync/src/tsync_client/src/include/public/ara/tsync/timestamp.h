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
/// @file       timestamp.h
/// @brief      timestamp type definition
/// @details
/// @date       2022-01-01
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// export_level=/TimeSync
/// module_path=/TimeSync/SyncTimeBase
/// @endcode
///
/// ================================================================

#ifndef _ARA_TSYNC_TIMESTAMEP_H_
#define _ARA_TSYNC_TIMESTAMEP_H_

#include <ara/core/steady_clock.h>

#include <chrono>

namespace ara {
namespace tsync {

/// @traceid  {SWS_TS_01260, 23-11}
/// @brief    Pseudo-clock definition.
struct TimeBase
{
    /// @traceid  {SWS_TS_01261, 23-11}
    using rep = std::int64_t;
    /// @traceid  {SWS_TS_01262, 23-11}
    using period = std::nano;
    /// @traceid  {SWS_TS_01263, 23-11}
    using duration = std::chrono::duration< rep, period >;
    /// @traceid  {SWS_TS_01264, 23-11}
    using time_point = std::chrono::time_point< TimeBase >;
    /// @traceid  {SWS_TS_01265, 23-11}
    static constexpr bool is_steady = false;
};

/// @traceid  {SWS_TS_01251, 20-11}
/// @brief    Standard timestamp type is an alias for a generic time_point.
using Timestamp = std::chrono::time_point< TimeBase, std::chrono::nanoseconds >;

}  // namespace tsync
}  // namespace ara

#endif  //__ARA_TSYNC_TIMESTAMEP_H__
