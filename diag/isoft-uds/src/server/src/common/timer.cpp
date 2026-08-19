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
/// @brief      This file implements timer functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=common
/// @trace_id_sr=6093955b-6ee7-4430-a7c8-8a28597be63b
/// @unit_name=timer
/// @unit_description=This file provides timer functionality
/// @interface_level=unit
/// @endcode
///
/// ================================================================

#include "timer.h"

#include <isoft/naicpp/global_evloop.h>

#include "common/log.h"
namespace isoft {
namespace uds {
namespace server {


Timer::~Timer() noexcept
{
    Stop();
    LogVerbose() << "Timer::~Timer| start remove timer:" << reinterpret_cast< uintptr_t >(timer_.get());
    timer_.reset();
    LogVerbose() << "Timer::~Timer| stop remove timer:" << reinterpret_cast< uintptr_t >(timer_.get());

}

std::int32_t Timer::Initialize(std::function< void() >&& handler, std::uint32_t const duration) noexcept
{
    duration_ = static_cast< int32_t >(duration);
    if (lifePtr_.get() == nullptr) {
        lifePtr_ = std::make_shared< uint32_t >(1U);
    }
    std::weak_ptr< uint32_t > weakPtr{lifePtr_};
    std::int32_t result = isoft::naicpp::GlobalGeneralEvLoop::Get()->MakeTimer(
        timer_, kInvalidDuration, [this, weakPtr, timeout = std::move(handler)]() {
            if (weakPtr.expired()) {
                return;
            }
            running_ = false;
            timeout();
        });
    LogVerbose() << "Timer::Initialize| make timer:" << reinterpret_cast< uintptr_t >(timer_.get());
    return result;
}


std::int32_t Timer::Initialize(std::function< void() >&& handler) noexcept
{
    if (lifePtr_.get() == nullptr) {
        lifePtr_ = std::make_shared< uint32_t >(1U);
    }
    std::weak_ptr< uint32_t > weakPtr{lifePtr_};
    std::int32_t result = isoft::naicpp::GlobalGeneralEvLoop::Get()->MakeTimer(
        timer_, kInvalidDuration, [this, weakPtr, timeout = std::move(handler)]() {
            if (weakPtr.expired()) {
                return;
            }
            running_ = false;
            timeout();
        });
    LogVerbose() << "Timer::Initialize| make timer:" << reinterpret_cast< uintptr_t >(timer_.get());
    return result;
}

std::int32_t Timer::Start(std::uint32_t const duration) noexcept
{
    std::int32_t const status{timer_->UpdateTime(static_cast< int32_t >(duration))};
    if (status == 0) {
        running_ = true;
        LogInfo() << "Timer start success, duration = " << duration << " s";
    } else {
        LogWarn() << "Timer start fails, duration = " << duration << " s";
    }
    return status;
}


std::int32_t Timer::Start() noexcept
{
    std::int32_t const status{timer_->UpdateTime(duration_)};
    if (status == 0) {
        running_ = true;
        LogInfo() << "Timer start success, duration = " << duration_ << " s";
    } else {
        LogWarn() << "Timer start fails, duration = " << duration_ << " s";
    }
    return status;
}


std::int32_t Timer::Stop() noexcept
{
    if (timer_.get() == nullptr) {
        return 0;
    }
    std::int32_t const status{timer_->UpdateTime(kInvalidDuration)};
    if (status == 0) {
        running_ = false;
    }
    return status;
}

}  // namespace server
}  // namespace uds
}  // namespace isoft
