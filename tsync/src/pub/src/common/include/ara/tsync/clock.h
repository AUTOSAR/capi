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
/// @file       clock.h
/// @brief      clock type definition
/// @details
/// @date       2023-02-09
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// export_level=/TimeSync/Utils
/// module_path=/TimeSync/Utils
/// @endcode
///
/// ================================================================

#ifndef ARA_TSYNC_CLOCK_H_
#define ARA_TSYNC_CLOCK_H_

#include <ara/core/steady_clock.h>

#include <chrono>

namespace ara {
namespace tsync {

/// @brief Class Clock provides an abstraction of the reference clock used by
///     SynchronizedTimeBaseProvider and SynchronizedTimeBaseConsumer classes.
class Clock final
{
public:
    // autosar requires using ara::core::SteadyClock  SWS_TS_00043
    /// @brief ReferenceClock - define alias
    using ReferenceClock = ara::core::SteadyClock;

    /// temporarily unused/// @brief rep - define alias
    /// temporarily unusedusing rep = ReferenceClock::rep

    /// temporarily unused/// @brief duration - define alias
    /// temporarily unusedusing duration = ReferenceClock::duration

    /// @brief time_point - define alias
    using time_point = std::chrono::time_point< ReferenceClock >;  // NOLINT

    /// @brief Method to obtain the current time_point.
    /// @return The current time as a time point based on the ReferenceClock.
    static time_point now() noexcept;  // NOLINT
};
}  // namespace tsync
}  // namespace ara

#endif  /// ARA_TSYNC_CLOCK_H_