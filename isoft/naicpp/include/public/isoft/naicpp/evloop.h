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
/// @file       evloop.h
/// @brief      EvLoop class
/// @details
/// @date       2021-11-10
/// @author     wei.jia
/// @version    1.2.0
///
/// ================================================================
///
/// Example usage:
/// A: User's own thread such as the main thread:
/// int main() {
/// EvLoop loop;
/// loop.Open();
/// TimerPtr tm1 ; loop.MakeTimer(tm1,2, [] {
/// // dosomesing
/// tm1.rest(); // Optionally release the timer here
/// });
/// TimerPtr tm2 ; loop.MakeTimer(tm2,2, [&] {
/// loop.Stop();
/// });
/// loop.Exec([&] {
/// // dosomesing
/// });
/// loop.Run(true); // Will block until loop.Stop() is called;
/// tm2.rest();
/// // dosomesing
/// exec.reset();
/// loop.Close();
/// }
/// B: Use EvLoop's own thread
/// Any module
/// EvLoop loop;
/// loop.Open();
/// TimerPtr tm1 ; loop.MakeTimer(tm1,2, [] {
/// // dosomesing
/// });
/// loop.Run(false); // Returns immediately
/// TimerPtr tm2 ; loop.MakeTimer(tm2,2, [&] {
/// tm1.rest();
/// loop.Stop();
/// });
/// loop.Exec([&] {
/// // dosomesing
/// });
/// tm2.rest();
/// loop.Stop();
/// loop.Close();
///
/// ================================================================

#ifndef ISOFT_NAICPP_INCLUDE_PUBLIC_ISOFT_NAICPP_EVLOOP_H_
#define ISOFT_NAICPP_INCLUDE_PUBLIC_ISOFT_NAICPP_EVLOOP_H_

#include <nai/io/nai_event.h>

#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <utility>

#include "isoft/define.h"
#include "isoft/naicpp/evnode_exec.h"
#include "isoft/naicpp/evnode_timer.h"
namespace isoft {
namespace naicpp {

///
/// @brief Event loop encapsulation
/// Open/Run/Stop/Close are not thread-safe
class EvLoop final
{
public:
    /// @brief EvNodeExec share pointer
    using ExecutorPtr = std::shared_ptr< EvNodeExec >;

    /// @brief EvNodeTimer share pointer
    using TimerPtr = std::shared_ptr< EvNodeTimer >;

    ///
    /// @brief Delete Copy Construct & Copy Assignment Operator
    /// & Move Assignment Operator & Move Constructor
    ///
    /// @brief copy constructor is forbidden.
    ///
    /// @traceid
    /// @uptrace
    /// @param other class instance.
    EvLoop(EvLoop const& other) noexcept = delete;

    /// @brief Copy Assignment Operator is forbidden.
    ///
    /// @traceid
    /// @uptrace
    /// @param other class instance.
    /// @returns   instance reference
    EvLoop& operator=(EvLoop const& other) noexcept = delete;

    /// @brief Move Constructor is forbidden.
    ///
    /// @traceid
    /// @uptrace
    /// @param obj class instance.
    /// @returns   instance object
    EvLoop(EvLoop&& obj) noexcept = delete;

    /// @brief Move Assignment Operator is forbidden.
    ///
    /// @traceid
    /// @uptrace
    /// @param other class instance.
    /// @returns   instance reference
    EvLoop& operator=(EvLoop&& other) & noexcept = delete;

    /// @brief default constructor
    ///
    /// @traceid
    /// @uptrace
    EvLoop() noexcept = default;

    /// @brief destructor.
    ///
    /// @traceid
    /// @uptrace
    ~EvLoop() noexcept { static_cast< void >(Close()); }

    ///
    /// @brief Get the Raw  Nai Ev Loop object
    /// Try not to call this function unless writing C functionality
    /// @return nai_evloop_t*
    nai_evloop_t* GetRawEvLoop() const noexcept { return naiLoop_; }

    ///
    /// @brief Initialize the evloop thread
    ///
    /// @param enableExecutor Whether to enable the executor, whether Exec() is available
    /// @returns  isoft::kSuccess Success
    /// @returns   -1 Failure
    int Open(bool const enableExecutor) noexcept;

    ///
    /// @brief Start the evloop thread
    ///
    /// @param useUserThread
    ///             true: Run blocks, processing asynchronous events in the current thread
    ///             false: Run creates a new thread internally and returns
    /// @note If useUserThread==true, waitCloseFinish needs to be set to false when calling Close;
    /// @note If it is @ref @see GlobalGeneralEvLoop, ensure consistency with the GlobalGeneralEvLoop::Initialize mode: i.e., ensure
    /// When GlobalGeneralEvLoop::Mode::kUserThread mode, useUserThread=true, and vice versa
    /// @return int
    int Run(bool const useUserThread) noexcept;

    ///
    /// @brief stop evloop
    ///
    /// @note This method only notifies the evloop to stop working, meaning the evloop may not have actually stopped working when the method returns.
    /// @note The behavior of restarting Run after Stop is undefined, Close is required before Open again
    void Stop() noexcept
    {
        if (nullptr != naiLoop_) {
            static_cast< void >(nai_evloop_break(naiLoop_, 0));
        }
    }

    ///
    /// @brief Close the evloop
    ///
    /// @note Pay attention to different Run modes, the timing of calling Stop/Close differs
    ///       When Run with useUserThread=true, after calling Stop, wait for Run to return to safely close the EvLoop
    ///       When Run with useUserThread=false, you should call Stop first, then ensure that Close is not called within the Evloop (Evnode callback)
    /// @return int
    int Close() noexcept;

    /// @brief Execute function in evloop (Executor)
    ///
    /// @note This method is thread-safe
    /// @param fun  function reference
    /// @return int
    int Exec(std::function< void() >&& fun) noexcept
    {
        if (nodeExec_ == nullptr) {
            return -1;
        }
        return nodeExec_->Exec(std::move(fun));
    }

    ///
    /// @brief Create a Timer
    /// If trigger needs to happen again (e.g., cyclically), call UpdateTime(xx) of the corresponding TimerPtr in onTimerCallback
    /// @param timer[output] The created Timer
    /// @param msecond Timer trigger time, in milliseconds
    /// @param onTimerCallback Timer callback
    /// @note The held Timer should be released before loop Close
    /// @note This method is thread-safe
    /// @return int
    int MakeTimer(TimerPtr& timer, int const msecond, std::function< void() >&& onTimerCallback) noexcept
    {  // NOLINT
        if (naiLoop_ == nullptr) {
            return -1;
        }

        TimerPtr t{new EvNodeTimer()};
        if (!t) {
            return -1;
        }

        timer = t;
        int const r{t->Open(naiLoop_, msecond, std::move(onTimerCallback))};
        if (r != isoft::kSuccess) {
            timer.reset();
        }

        return r;
    }

private:
    ///
    /// @brief Initialize
    ///
    /// @returns  isoft::kSuccess Success
    /// @returns   -1 Failure
    int InitExecutor() noexcept
    {
        assert(this->nodeExec_ == nullptr);
        this->nodeExec_ = new EvNodeExec();
        /* // if (nodeExec_ == nullptr) {
        //     return -1;
        // } */
        int const r{nodeExec_->Open(naiLoop_)};
        if (r != isoft::kSuccess) {
            delete this->nodeExec_;
            this->nodeExec_ = nullptr;
            return r;
        }

        return isoft::kSuccess;
    }

private:
    /// @name naiLoop_
    /// nai run loop
    nai_evloop_t* naiLoop_{nullptr};

    /// @name naiLoopThread_
    /// nai loop thread
    std::thread naiLoopThread_;

    /// @name nodeExec_
    /// post call Executor
    EvNodeExec* nodeExec_{nullptr};
};

}  // namespace naicpp
}  // namespace isoft

#endif  // ISOFT_NAICPP_INCLUDE_PUBLIC_ISOFT_NAICPP_EVLOOP_H_
