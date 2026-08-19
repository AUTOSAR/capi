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
/// @file       evnode_timer.h
/// @brief
/// @details
/// @date       2021-11-10
/// @author     wei.jia
/// @version    1.2.0
///
/// ================================================================

#ifndef ISOFT_NAICPP_INCLUDE_PUBLIC_ISOFT_NAICPP_EVNODE_TIMER_H_
#define ISOFT_NAICPP_INCLUDE_PUBLIC_ISOFT_NAICPP_EVNODE_TIMER_H_

#include <nai/io/nai_event.h>

#include <cstdint>
#include <forward_list>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <utility>

#include "isoft/define.h"
#include "isoft/naicpp/internal/evnode.h"
namespace isoft {
namespace naicpp {

/// @brief EvNodeTimer encapsulation
class EvNodeTimer final
{
public:
    ///
    /// @brief Update the new timing interval
    ///
    /// @param msecond
    /// @return  >=0     success
    int UpdateTime(int const msecond) noexcept { return evnode_.UpdateTime(msecond); }
    ///
    /// @brief Blocking close
    ///
    /// @return int
    int Close() noexcept { return evnode_.Close(); }

    /// @brief destructor.
    ///
    /// @traceid
    /// @uptrace
    ~EvNodeTimer() { evnode_.Close(); }

protected:
    friend class EvLoop;

    /// @brief default constructor
    ///
    /// @traceid
    /// @uptrace
    EvNodeTimer() = default;

    /// @brief copy constructor is forbidden.
    ///
    /// @traceid
    /// @uptrace
    /// @param other class instance.
    EvNodeTimer(EvNodeTimer const& other) noexcept = default;

    /// @brief Copy Assignment Operator is forbidden.
    ///
    /// @traceid
    /// @uptrace
    /// @param other class instance.
    /// @returns   instance reference
    EvNodeTimer& operator=(EvNodeTimer const& other) noexcept = default;

    /// @brief Move Constructor is forbidden.
    ///
    /// @traceid
    /// @uptrace
    /// @param obj class instance.
    /// @returns   instance object
    EvNodeTimer(EvNodeTimer&& obj) noexcept = default;

    /// @brief Move Assignment Operator is forbidden.
    ///
    /// @traceid
    /// @uptrace
    /// @param other class instance.
    /// @returns   instance reference
    EvNodeTimer& operator=(EvNodeTimer&& other) & noexcept = default;
    ///
    /// @brief Start evnode
    ///
    /// @param evloop
    /// @param timer_msec Timer interval (0 means non-timer)
    /// @param callback evnode event callback
    /// @return int
    int Open(nai_evloop_t* const evloop, int const timer_msec, internal::EvNode::CallBack&& callback) noexcept
    {
        int const r{evnode_.Open(evloop, timer_msec, std::move(callback))};
        if (r >= 0) {
            return isoft::kSuccess;
        }
        return r;
    }

private:
    /// @name evnode_
    /// event node
    internal::EvNode evnode_;
};

}  // namespace naicpp
}  // namespace isoft

#endif  // ISOFT_NAICPP_INCLUDE_PUBLIC_ISOFT_NAICPP_EVNODE_TIMER_H_
