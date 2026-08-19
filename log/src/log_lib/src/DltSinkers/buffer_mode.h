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
/// @file       buffer_mode.h
/// @brief      Buffer mode management class, managing the buffer flush strategy of FileSinkerNoLocker
/// @details
/// @date       2026-01-30
/// @author     Copilot
/// @version    1.2.0
///
/// ================================================================

#ifndef __BUFFER_MODE_H__
#define __BUFFER_MODE_H__

#include <isoft/naicpp/evloop.h>
#include <isoft/naicpp/global_evloop.h>

#include <cstdint>
#include <functional>
#include <memory>
#include <tuple>

#include "Utils/src/log_buffer.h"
#include "Utils/src/private_log.h"
#include "common.h"

namespace ara {
namespace log {
namespace internal {

/// @brief Buffer mode enumeration
enum class BufferMode : std::uint8_t
{
    ON_MSG,            ///< Real-time flush, each log is flushed immediately
    ON_SPECIFIC_SIZE,  ///< Buffered flush, triggered when a specified size or timeout is reached
};

/// @brief Flush callback function: called when a flush is needed
using FlushCallback = std::function< bool() >;

/// @brief Buffer mode manager
class BufferModeManager final
{
public:
    /// @brief Constructor
    /// @param logBuffer Pointer to the log buffer
    /// @param flushCallback Flush callback function
    BufferModeManager(FlushCallback flushCallback) noexcept
        : flushCallback_(std::move(flushCallback)), mode_(BufferMode::ON_SPECIFIC_SIZE)
    {
    }

    ~BufferModeManager() noexcept { Stop(); }

    BufferModeManager(BufferModeManager const&) = delete;
    BufferModeManager& operator=(BufferModeManager const&) = delete;
    BufferModeManager(BufferModeManager&&)                 = delete;
    BufferModeManager& operator=(BufferModeManager&&) = delete;

    /// @brief Set the buffer mode
    /// @param mode Buffer mode
    void SetMode(BufferMode mode) noexcept
    {
        mode_ = mode;
        if (mode_ == BufferMode::ON_MSG) {
            Stop();  // No timer needed in real-time mode
        }
    }

    /// @brief Get the current buffer mode
    /// @return Current buffer mode
    BufferMode GetMode() const noexcept { return mode_; }

    /// @brief Set the timeout (in milliseconds)
    /// @param timeoutMs Timeout in milliseconds, default 3000ms
    void SetTimeout(std::int32_t timeoutMs) noexcept { timeoutMs_ = timeoutMs; }

    /// @brief Start the buffer mode manager (effective only in ON_SPECIFIC_SIZE mode)
    /// @return true success, false failure
    bool Start() noexcept
    {
        if (mode_ == BufferMode::ON_MSG) {
            return true;  // No need to start timer in real-time mode
        }

        eventLoop_ = isoft::naicpp::GlobalGeneralEvLoop::Get();
        if (!eventLoop_) {
            LOGERROR("BufferModeManager::Start") << " eventLoop is null";
            return false;
        }

        std::function< void() > onTimer = [this]() { this->_onTimer(); };
        std::int32_t ret                = eventLoop_->MakeTimer(timer_, timeoutMs_, std::move(onTimer));
        if (ret != 0) {
            LOGERROR("BufferModeManager::Start") << " MakeTimer failed, ret: " << ret;
            return false;
        }

        return true;
    }

    /// @brief Stop the timer
    void Stop() noexcept
    {
        if (timer_) {
            timer_.reset();
            timer_ = nullptr;
        }
    }

    /// @brief Record the last write time (used for timer judgment)
    void RecordWrite() noexcept { lastWriteTime_ = std::chrono::steady_clock::now(); }

private:
    /// @brief Timer callback function
    void _onTimer() noexcept
    {
        // Check if there is unflushed data

        auto now     = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast< std::chrono::milliseconds >(now - lastWriteTime_).count();

        // Exceeded the set timeout, perform flush
        if (elapsed >= timeoutMs_) {
            if (flushCallback_) {
                std::ignore = flushCallback_();
            }
        }

        // Proceed to the next check
        if (timer_) {
            timer_->UpdateTime(timeoutMs_);
        }
    }

private:
    FlushCallback flushCallback_{nullptr};           ///< Flush callback function
    BufferMode mode_{BufferMode::ON_SPECIFIC_SIZE};  ///< Buffer mode

    std::int32_t timeoutMs_{3000};  ///< Timeout in milliseconds, default 3 seconds

    isoft::naicpp::EvLoop::TimerPtr timer_{nullptr};               ///< Timer
    std::shared_ptr< isoft::naicpp::EvLoop > eventLoop_{nullptr};  ///< Event loop

    std::chrono::steady_clock::time_point lastWriteTime_{};  ///< Last write time
};

}  // namespace internal
}  // namespace log
}  // namespace ara

#endif  // __BUFFER_MODE_H__
