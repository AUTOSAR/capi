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
/// @file       timer.h
/// @brief      Timer, periodic task.
/// @details
/// @date       2024-06-05
/// @author     wangyanlong
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/PlatformHealthManagement/Util
/// @unit_description=Timer, periodic task.
/// @trace_id_sr=SR_PHM_01002,SR_PHM_01003,SR_PHM_01004,SR_PHM_01005,SR_PHM_01012,SR_PHM_01013
/// @unit_name=Timer
/// @interface_level=module
/// @endcode
///
/// ================================================================

#ifndef ARA_PHM_INTERNAL_UTIL_TIMER_H_
#define ARA_PHM_INTERNAL_UTIL_TIMER_H_

#include <ara/core/string.h>
#include <isoft/naicpp/global_evloop.h>

#include <atomic>
#include <chrono>
#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>

namespace ara {
namespace phm {
namespace internal {

/// @brief timer task.
/// @trace_id_sr=SR_PHM_01002,SR_PHM_01003,SR_PHM_01004,SR_PHM_01005,SR_PHM_01012,SR_PHM_01013
/// @needwork = ad
struct TimerTask
{
    /// @brief inner timer from isoft eventloop.
    isoft::naicpp::EvLoop::TimerPtr timer;

    /// @brief period of timer.
    int32_t durationMs{0};

    /// @brief callback of timer.
    std::function< void() > cb;

    /// @brief true one shot timer; false periodic timer.
    bool periodic{false};

    /// @brief whether timer is running.
    std::atomic_bool isRun{};
};

/// @brief Timer, periodic task.
/// @trace_id_sr=SR_PHM_01002,SR_PHM_01003,SR_PHM_01004,SR_PHM_01005,SR_PHM_01012,SR_PHM_01013
/// @needwork = ad
class Timer
{
public:
    /// @brief Creation of a timer.
    /// @param name name of timer.
    /// @param durationMs period of timer.
    /// @param cb callback of timer.
    /// @param periodic true, periodic timer; false, one shot timer.
    /// @trace_id_sr=SR_PHM_01002,SR_PHM_01003,SR_PHM_01004,SR_PHM_01005,SR_PHM_01012,SR_PHM_01013
    /// @needwork = ad
    Timer(ara::core::String const& name,
          int32_t const& durationMs,
          std::function< void() > const& cb,
          bool const periodic) noexcept;

    /// @brief Default deconstructor.
    /// @trace_id_sr=SR_PHM_01002,SR_PHM_01003,SR_PHM_01004,SR_PHM_01005,SR_PHM_01012,SR_PHM_01013
    /// @needwork = ad
    ~Timer() noexcept;

    /// @brief Start timer.
    /// @return 0, start timer success; < 0, failed.
    /// @trace_id_sr=SR_PHM_01002,SR_PHM_01003,SR_PHM_01004,SR_PHM_01005,SR_PHM_01012,SR_PHM_01013
    /// @needwork = ad
    int32_t Start() noexcept;

    /// @brief Stop timer.
    /// @trace_id_sr=SR_PHM_01002,SR_PHM_01003,SR_PHM_01004,SR_PHM_01005,SR_PHM_01012,SR_PHM_01013
    /// @needwork = ad
    void Stop() noexcept;

    /// @brief Returns whether the timer is running.
    /// @return true, timer is running; false, timer is not running.
    /// @trace_id_sr=SR_PHM_01002,SR_PHM_01003,SR_PHM_01004,SR_PHM_01005,SR_PHM_01012,SR_PHM_01013
    /// @needwork = ad
    bool IsRun() const noexcept;

    /// @brief The copy constructor for Timer shall not be used.
    /// @param obj The object to be copied.
    /// @trace_id_sr=SR_PHM_01002,SR_PHM_01003,SR_PHM_01004,SR_PHM_01005,SR_PHM_01012,SR_PHM_01013
    /// @needwork = ad
    Timer(Timer& obj) = delete;

    /// @brief The copy assignment for Timer shall not be used.
    /// @param obj The object to be copied.
    /// @return The copied object.
    /// @trace_id_sr=SR_PHM_01002,SR_PHM_01003,SR_PHM_01004,SR_PHM_01005,SR_PHM_01012,SR_PHM_01013
    /// @needwork = ad
    Timer& operator=(Timer const& obj) = delete;

    /// @brief The move constructor for Timer shall not be used.
    /// @param obj The object to be moved.
    /// @trace_id_sr=SR_PHM_01002,SR_PHM_01003,SR_PHM_01004,SR_PHM_01005,SR_PHM_01012,SR_PHM_01013
    /// @needwork = ad
    Timer(Timer&& obj) = delete;

    /// @brief The move assignment for Timer shall not be used.
    /// @param obj The object to be moved.
    /// @return The moved object.
    /// @trace_id_sr=SR_PHM_01002,SR_PHM_01003,SR_PHM_01004,SR_PHM_01005,SR_PHM_01012,SR_PHM_01013
    /// @needwork = ad
    Timer& operator=(Timer const&& obj) = delete;

private:
    /// @brief name of timer.
    /// @trace_id_sr=SR_PHM_01002,SR_PHM_01003,SR_PHM_01004,SR_PHM_01005,SR_PHM_01012,SR_PHM_01013
    /// @needwork = dda
    ara::core::String const kName;

    /// @brief event loop.
    /// @trace_id_sr=SR_PHM_01002,SR_PHM_01003,SR_PHM_01004,SR_PHM_01005,SR_PHM_01012,SR_PHM_01013
    /// @needwork = dda
    std::shared_ptr< isoft::naicpp::EvLoop > evLoop_;

    /// @brief timer task.
    /// @trace_id_sr=SR_PHM_01002,SR_PHM_01003,SR_PHM_01004,SR_PHM_01005,SR_PHM_01012,SR_PHM_01013
    /// @needwork = dda
    std::shared_ptr< TimerTask > timerTask_;
};

}  // namespace internal
}  // namespace phm
}  // namespace ara

#endif  // ARA_PHM_INTERNAL_UTIL_TIMER_H_
