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
/// @file       steady_clock.h
/// @brief
/// @details
/// @date       2022-06-24
/// @author     hanzhibo
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_CORE_STEADY_CLOCK_H_
#define ARA_CORE_STEADY_CLOCK_H_

#include <chrono>
#include <cstdint>

namespace ara {
namespace core {

/// @brief This clock represents a monotonic clock.
///
/// The time points of this clock cannot decrease as physical time moves forward and the time between ticks of this
/// clock is constant.
///
/// @uptrace{SWS_CORE_06401, b32f19634a7f8edabcf830730e6a636853421980}
/// @uptrace{SWS_CORE_11800, f2f4affcb039bedbe6eae03480c0aa6d7bf10a5c}
class SteadyClock final
{
public:
    /// @uptrace{SWS_CORE_06412, 5985997451e43a9937fdb8333537c21718472c3a}
    using rep = std::int64_t;  // NOLINT

    /// @uptrace{SWS_CORE_06413, 0063cd31ed18628c4622c0d6ee1427d9fdef9db0}
    using period = std::nano;  // NOLINT

    /// @uptrace{SWS_CORE_06411, f9f603dc8808b08a2917139b060d592d3991d6b3}
    using duration = std::chrono::duration< rep, period >;  // NOLINT

    /// @uptrace{SWS_CORE_06414, f084082cde6e44ac26cd64ac0b7e3ece9766c4bd}
    using time_point = std::chrono::time_point< SteadyClock, duration >;  // NOLINT

    /// @uptrace{SWS_CORE_06431, d1da14cb2ed9999c75b0c17769a579341d4e08b9}
    static constexpr bool is_steady = true;  // NOLINT

    /// @uptrace{SWS_CORE_06432, 769a9b16ae307f8226779891027cb1656bc03e2e}
    /// @uptrace{SWS_CORE_11801, 2d944e8eea378fd70a8cf35fa97e36074eabcb26}
    static time_point now() noexcept  // NOLINT
    {
        duration dur(std::chrono::steady_clock::now().time_since_epoch());
        time_point tp(dur);
        return tp;
    }
};

}  // namespace core
}  // namespace ara

#endif  // ARA_CORE_STEADY_CLOCK_H_
