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
/// @file       event.cpp
/// @brief      Event class implementation
/// @details
/// @date       2024-07-20
/// @author     hanzhibo
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/UCM Master/Fsm
/// @interface_level=unit
/// @trace_id_sr=SR_UCM_00035
/// @unit_name=Event
/// @unit_description=Event class implementation
/// @endcode
///
/// ================================================================

#include "fsm/approvals/event.h"

#include "iostream"
#include "utils/ucmm_log.h"

namespace ara {
namespace ucm {
namespace vpkgmgr {

/// @brief Wait for signal trigger
/// @param timeoutMs Maximum wait time in milliseconds
/// @return
/// @throws no
Event::ResultType Event::Wait(int32_t const timeoutMs)
{
    if (signals_.load() <= 0) {
        return Event::ResultType::kSuccess;
    }

    std::unique_lock< std::mutex > lock{mtx_};
    if (timeoutMs >= 0) {
        bool const result{cv_.wait_for(lock, std::chrono::milliseconds(timeoutMs),
                                       [this]() noexcept { return signals_.load() <= 0; })};
        if (result) {
            return Event::ResultType::kSuccess;
        }
        return Event::ResultType::kTimeout;
    }

    cv_.wait(lock, [this]() noexcept { return signals_.load() <= 0; });
    return Event::ResultType::kSuccess;
}

/// @brief Trigger signal
/// @param count Number of signals to trigger, default 1
/// @throws no
void Event::Signal(int32_t const count) noexcept
{
    if (signals_.load() > 0) {
        int32_t const ret{signals_.fetch_sub(count)};
        LOG_DEBUG << "ret" << ret;
        cv_.notify_one();
        LOG_DEBUG << "events signal=" << signals_.load();
    }
}

/// @brief Increase the number of signals to wait for
/// @param count Number of signals to increase by
/// @throws no
void Event::Add(int32_t const count) noexcept { std::ignore = signals_.fetch_add(count); }

/// @brief Reset
/// @throws no
void Event::Reset() noexcept
{
    if (signals_.load() > 0) {
        signals_.store(0);
        cv_.notify_one();
    }
}

}  // namespace vpkgmgr
}  // namespace ucm
}  // namespace ara