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
/// @file       file_stat_checker.h
/// @brief      Independent class: encapsulates stat/inode file checking and timer callback
/// @details
/// @date       2024-06-28
/// @author     Copilot
/// @version    1.2.0
///
/// ================================================================

#ifndef __FILE_STAT_CHECKER_H__
#define __FILE_STAT_CHECKER_H__

#include <isoft/naicpp/evloop.h>
#include <isoft/naicpp/global_evloop.h>
#include <isoft/naicpp/terminating_handler.h>
#include <nai/io/nai_event.h>
#include <sys/stat.h>
#include <unistd.h>

#include <cstdint>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "Utils/src/private_log.h"
#include "common.h"

namespace ara {
namespace log {
namespace internal {

/// @brief Aggregate result callback: returns true when any target needs to be reopened
using FileCheckCallback = std::function< void(bool needReopen) >;

/// @brief Per-target result callback: provides target filepath, fd, and needReopen flag for that target
using FileCheckPerTargetCallback = std::function< void(bool needReopen) >;

/// @brief Encapsulates stat/inode file checking and a timer based on EvLoop
class FileStatChecker final
{
public:
    /// @brief Default constructor, used for multi-target scenarios
    FileStatChecker() noexcept = default;
    ~FileStatChecker() noexcept
    {
        Stop();
        ClearTargets();
    }

    /// @brief Perform one check immediately
    /// @return Whether the file needs to be reopened
    bool CheckNow() noexcept
    {
        bool anyNeedReopen{false};
        for (Target const &t : targets_) {
            bool needReopen{false};
            if (!t.filepath.empty()) {
                struct stat st;
                if (stat(t.filepath.c_str(), &st) == -1) {
                    needReopen = true;
                } else {
                    struct stat fst;
                    if (t.fd != -1 && fstat(t.fd, &fst) == 0) {
                        if (fst.st_ino != st.st_ino) {
                            needReopen = true;
                        }
                    }
                }
            }
            if (t.callback) {
                t.callback(needReopen);
            }
            anyNeedReopen = anyNeedReopen || needReopen;
        }
        return anyNeedReopen;
    }

    /// @brief Start a timer to check periodically
    /// @param[in] milliseconds The period in milliseconds
    /// @return true if the timer was created successfully
    bool Start(std::int32_t milliseconds = 5000) noexcept
    {
        eventLoop_ = isoft::naicpp::GlobalGeneralEvLoop::Get();
        if (!eventLoop_) {
            LOGERROR("FileStatChecker::Start") << " eventLoop is null";
            return false;
        }

        std::function< void() > onTimer = [this, milliseconds]() { this->_onTimer(milliseconds); };
        std::int32_t ret                = eventLoop_->MakeTimer(timer_, milliseconds, std::move(onTimer));
        if (ret != 0) {
            LOGERROR("FileStatChecker::Start") << " MakeTimer failed, ret : " << ret;
        }
        return (ret == 0);
    }

    /// @brief Stop the timer
    void Stop() noexcept
    {
        if (timer_) {
            timer_.reset();
            timer_ = nullptr;
        }
    }

    /// @brief Add a target to check
    /// @return Target index (for subsequent updates/removals)
    std::size_t AddTarget(std::string const &filepath, std::int32_t fd, FileCheckPerTargetCallback cb) noexcept
    {
        targets_.push_back(Target{filepath, fd, std::move(cb)});
        return targets_.size() - 1U;
    }

    /// @brief Update a specified target
    void UpdateTarget(std::size_t idx, std::string const &filepath, std::int32_t fd) noexcept
    {
        if (idx < targets_.size()) {
            targets_[idx].filepath = filepath;
            targets_[idx].fd       = fd;
        }
    }

    /// @brief Remove a specified target
    void RemoveTarget(std::size_t idx) noexcept
    {
        if (idx < targets_.size()) {
            targets_.erase(targets_.begin() + static_cast< long >(idx));
        }
    }

    /// @brief Clear all targets
    void ClearTargets() noexcept { targets_.clear(); }

private:
    void _onTimer(std::int32_t milliseconds)
    {
        std::ignore = CheckNow();
        // Proceed to the next check
        if (timer_) {
            timer_->UpdateTime(milliseconds);
        }
    }

private:
    struct Target
    {
        std::string filepath{};
        std::int32_t fd{-1};
        FileCheckPerTargetCallback callback{nullptr};
    };
    std::vector< Target > targets_{};

    isoft::naicpp::EvLoop::TimerPtr timer_{nullptr};
    std::shared_ptr< isoft::naicpp::EvLoop > eventLoop_{nullptr};
};

}  // namespace internal
}  // namespace log
}  // namespace ara
#endif  // __FILE_STAT_CHECKER_H__