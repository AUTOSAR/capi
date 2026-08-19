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
/// @file       timer.cpp
/// @brief
/// @details
/// @date       2024-06-20
/// @author     wangyanlong
/// @version    1.2.0
///
/// ================================================================

#include "ara/phm/internal/timer.h"

#include <isoft/naicpp/global_evloop.h>

#include "ara/phm/internal/phm_log.h"
#include "ara/phm/internal/types.h"

namespace ara {
namespace phm {
namespace internal {

/// @brief Creation of a timer.
/// @param name name of timer.
/// @param durationMs period of timer.
/// @param cb callback of timer.
/// @param periodic true, periodic timer; false, one shot timer.
Timer::Timer(ara::core::String const& name,
             int32_t const& durationMs,
             std::function< void() > const& cb,
             bool const periodic) noexcept
    : kName{name}, evLoop_{isoft::naicpp::GlobalGeneralEvLoop::Get()}, timerTask_{nullptr}
{
    LOG_INFO << "timer, name " << kName << ", duration ms " << durationMs << ", periodic " << periodic;
    timerTask_             = std::make_shared< TimerTask >();
    timerTask_->durationMs = durationMs;
    timerTask_->cb         = cb;
    timerTask_->periodic   = periodic;
    timerTask_->isRun      = false;

    std::weak_ptr< TimerTask > const taskWeakPtr{timerTask_};
    int32_t const ret{evLoop_->MakeTimer(timerTask_->timer, -1, [taskWeakPtr, name]() {
        std::shared_ptr< TimerTask > tmpShared{taskWeakPtr.lock()};
        if (tmpShared) {
            if (!tmpShared->isRun.load()) {
                LOG_DEBUG << "timer " << name << " is stopped.";
                return;
            }

            tmpShared->cb();
            if (tmpShared->periodic) {
                int32_t const r{static_cast< int32_t >(tmpShared->timer->UpdateTime(tmpShared->durationMs))};
                if (r < 0) {
                    LOG_ERROR << "update timer " << name << " error";
                }
            } else {
                // Timer that runs once
                tmpShared->isRun.store(false);
            }
        }
    })};

    if (0 != ret) {
        LOG_ERROR << "make timer error, ret " << ret;
        return;
    }
}

/// @brief Default deconstructor.
Timer::~Timer() noexcept
{
    Stop();
    timerTask_.reset();
    evLoop_.reset();
}

/// @brief Start timer.
/// @return 0, start timer success; < 0, failed.
int32_t Timer::Start() noexcept
{
    if (timerTask_->timer->UpdateTime(timerTask_->durationMs) < 0) {
        LOG_ERROR << "start timer " << kName << " error.";
    }
    timerTask_->isRun.store(true);
    LOG_DEBUG << "timer " << kName << " started";
    return 0;
}

/// @brief Stop timer.
void Timer::Stop() noexcept
{
    if (timerTask_->timer->UpdateTime(-1) < 0) {
        LOG_ERROR << "stop timer " << kName << " error.";
    }
    timerTask_->isRun.store(false);
    LOG_DEBUG << "timer " << kName << " stopped.";
}

/// @brief Returns whether the timer is running.
/// @return true, timer is running; false, timer is not running.
bool Timer::IsRun() const noexcept { return timerTask_->isRun.load(); }

}  // namespace internal
}  // namespace phm
}  // namespace ara