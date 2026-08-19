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
/// @file       evnode.h
/// @brief
/// @details
/// @date       2021-11-10
/// @author     wei.jia
/// @version    1.2.0
///
/// ================================================================

#ifndef ISOFT_NAICPP_INCLUDE_PUBLIC_ISOFT_NAICPP_INTERNAL_EVNODE_H_
#define ISOFT_NAICPP_INCLUDE_PUBLIC_ISOFT_NAICPP_INTERNAL_EVNODE_H_

#include <nai/io/nai_event.h>

#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <utility>
namespace isoft {
namespace naicpp {
namespace internal {

int const kExecFunction{0};
int const kCloseSelf{1};

/// @brief EvNode encapsulation
class EvNode final
{
public:
    /// @brief callback function type
    using CallBack = std::function< void() >;

    ///
    /// @brief copy constructor is forbidden.
    ///
    /// @traceid
    /// @uptrace
    /// @param other class instance.
    EvNode(EvNode const& other) noexcept = delete;

    /// @brief Copy Assignment Operator is forbidden.
    ///
    /// @traceid
    /// @uptrace
    /// @param other class instance.
    /// @returns   instance reference
    EvNode& operator=(EvNode const& other) noexcept = delete;

    /// @brief Move Constructor is forbidden.
    ///
    /// @traceid
    /// @uptrace
    /// @param obj class instance.
    /// @returns   instance object
    EvNode(EvNode&& obj) noexcept = delete;

    /// @brief Move Assignment Operator is forbidden.
    ///
    /// @traceid
    /// @uptrace
    /// @param other class instance.
    /// @returns   instance reference
    EvNode& operator=(EvNode&& other) & noexcept = delete;

    /// @brief default constructor
    ///
    /// @traceid
    /// @uptrace
    EvNode() noexcept { static_cast< void >(nai_evnode_init(&eventNode_)); }

    /// @brief destructor.
    ///
    /// @traceid
    /// @uptrace
    ~EvNode() noexcept
    {
        Close();
        assert(IsClosed());
    }

    ///
    /// @brief Start evnode,
    ///
    /// @param evloop evnode host thread
    /// @param timer_msec  Timer interval (0 means non-timer)
    /// @param callback  evnode event callback
    /// @return int
    ///
    int Open(nai_evloop_t* const evloop, int const timer_msec, CallBack&& callback) noexcept
    {
        if (!IsClosed()) {
            return -1;
        }
        callback_ = std::move(callback);
        int r{nai_evnode_set_cb(&eventNode_, [](nai_evnode_t* const p, nai_int_t const events) noexcept -> int {
            return reinterpret_cast< EvNode* >(
                       reinterpret_cast< char* >(p)
                       - reinterpret_cast< char* >(&reinterpret_cast< EvNode* >(0)->eventNode_))  // NOLINT
                ->OnEvent(static_cast< unsigned int >(events));
        })};
        if (r < 0) {
            return r;
        }

        if (timer_msec != 0) {
            r = nai_evnode_set_timeout(&eventNode_, 0, timer_msec);
        }
        if (r < 0) {
            return r;
        }

        return nai_evnode_open(&eventNode_, evloop);
    }

    ///
    /// @brief Send close request
    /// If in the loop thread, close directly; otherwise, send a signal to notify the loop to close the node
    /// @return int
    int PostClose() noexcept
    {
        if (IsClosed()) {
            return -1;
        }
        int const ret{nai_evnode_in_dispatch(&eventNode_)};
        if (1 == ret) {
            return nai_evnode_close(&eventNode_);
        } else {
            return nai_evnode_post(&eventNode_, kCloseSelf);
        }
    }

    ///
    /// @brief Send close request, call notify after closing
    ///
    /// @param notify
    /// @return int
    int CloseWithNotify(std::function< void() > const& notify) noexcept
    {
        if (IsClosed()) {
            return -1;
        }
        int r;
        if (1 == nai_evnode_in_dispatch(&eventNode_)) {
            r = nai_evnode_close(&eventNode_);
            if (r >= 0) {
                notify();
            }
        } else {
            r = nai_evnode_post(&eventNode_, kCloseSelf);
            if (r >= 0) {
                closeNotify_ = notify;
            }
        }
        return r;
    }

    ///
    /// @brief Blocking close
    ///
    /// @return int
    int Close() noexcept
    {
        if (IsClosed()) {
            return -1;
        }
        int r;
        if (1 == nai_evnode_in_dispatch(&eventNode_)) {
            r = nai_evnode_close(&eventNode_);
            if (r < 0) {
                return r;
            }
        } else {
            r = nai_evloop_exec(
                nai_evnode_get_loop(&eventNode_),
                [](void* args) -> int {
                    nai_evnode_t* evnode = reinterpret_cast< nai_evnode_t* >(args);
                    return nai_evnode_close(evnode);
                },
                &eventNode_);
            if (r < 0) {
                return r;
            }
            closeNotify_ = []() noexcept {};
        }
        return r;
    }

    ///
    /// @brief Notify the loop to run the callback
    ///
    /// @return int
    int Signal() noexcept
    {
        if (IsClosed()) {
            return -1;
        }
        return nai_evnode_post(&eventNode_, kExecFunction);
    }

    ///
    /// @brief Check if eventNode is closed
    ///
    /// @return int
    bool IsClosed() const noexcept { return nai_evnode_get_loop(&eventNode_) == nullptr; }

    ///
    /// @brief Delay timer timeout duration
    /// @param msecond   Delay in milliseconds
    ///
    /// @return  >=0     on success
    int UpdateTime(int const msecond) noexcept { return nai_evnode_set_timeout(&eventNode_, 0, msecond); }

private:
    ///
    /// @brief Event callback
    /// @param events   Events
    ///
    /// @return  0  success
    int OnEvent(nai_int_t const events) noexcept
    {
        if (IsClosed()) {
            return -1;
        }
        int r{0};
        unsigned int const NAICPP_EV_NOTIFY{static_cast< unsigned int >(NAI_EV_NOTIFY)};
        unsigned int const NAICPP_EV_TIMEOUT{static_cast< unsigned int >(NAI_EV_TIMEOUT)};
        if (NAICPP_EV_NOTIFY == (events & NAICPP_EV_NOTIFY)) {
            unsigned int const NAICPP_EV_NOTIFY_CODE{static_cast< unsigned int >(NAI_EV_NOTIFY_CODE(events))};
            switch (NAICPP_EV_NOTIFY_CODE) {
                case kExecFunction: {
                    /* async execution */
                    callback_();
                    break;
                }
                case kCloseSelf: {
                    /* close self */
                    r = nai_evnode_close(&eventNode_);
                    assert(r >= 0);
                    closeNotify_();
                    break;
                }
                default: {
                    break;
                }
            }
        } else if (NAICPP_EV_TIMEOUT == (events & NAICPP_EV_TIMEOUT)) {
            callback_();
        } else {
        }
        return r;
    }

private:
    /// @name eventNode_
    /// nai event node
    nai_evnode_t eventNode_;

    /// @name callback_
    /// call back
    CallBack callback_;

    /// @name closeNotify_
    // Consider std::shared_ptr<CallBack> cb_ or Callback raw point; effect of sharing a variable (atomic_exchange)
    CallBack closeNotify_;
};

}  // namespace internal
}  // namespace naicpp
}  // namespace isoft

#endif  // ISOFT_NAICPP_INCLUDE_PUBLIC_ISOFT_NAICPP_INTERNAL_EVNODE_H_
