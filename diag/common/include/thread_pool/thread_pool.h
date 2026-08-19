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
/// @file       thread_pool.h
/// @brief      This file provides thread pool related definitions.
/// @details    Expansion and contraction strategy: 1. Expansion: dynamically expand threads to maximum based on task count; 2. Contraction: threads exit after waiting 1 second if there are no tasks in the task queue, maintaining at least workerMin_ working threads;
/// @date       2022-04-22
/// @author     gaohuiming
/// @version    1.2.0
///
/// ================================================================
///
/// TODO:
/// 1. Add interface or modify existing interface, set variable nai_thread_attr_t* a
/// Set thread attributes, such as thread stack;
/// 2. Optimize thread pool contraction strategy;
///
/// ================================================================

#ifndef _ISOFT_COMMON_THREAD_POOL_H_
#define _ISOFT_COMMON_THREAD_POOL_H_

#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>

#include "nai/os/nai_thread.h"

namespace isoft {

extern "C" nai_thread_api thread_proc(void *const arg);  //NOLINT

/// @brief Thread pool
/// @code{.isoft}
/// export_level=/Diagnostics/Diagnostic Manager/Main
/// @endcode
class ThreadPool
{
private:
    /// @brief Whether running
    std::atomic_bool running_{true};

    /// @brief Current number of running threads
    std::atomic_uint16_t mCurrentRunningThread_{0};

    /// @brief Current number of created threads
    std::atomic_uint16_t mCreatedThreadNum_{0};

    /// @brief Maximum number of threads
    std::uint16_t workerMax_{0U};

    /// @brief Minimum number of threads, thread pool contracts to this value
    std::uint16_t workerMin_{2};

    /// @brief Task queue
    std::queue< std::function< void() > > taskQueue_{};

    /// @brief Task queue lock
    std::mutex taskQueueLock_{};

    /// @brief Task queue condition variable
    std::condition_variable taskQueueCondition_{};

    /// @brief Thread attributes
    nai_thread_attr_t a_{};

public:
    /// @brief Constructor
    ThreadPool() = default;

    /// @brief Destructor
    virtual ~ThreadPool() noexcept;

    /// @brief Copy constructor
    /// @param[in] object Reference
    ThreadPool(ThreadPool const &object) = delete;

    /// @brief Copy assignment operator
    /// @param[in] object Reference
    /// @return Reference
    ThreadPool &operator=(ThreadPool const &object) = delete;

    /// @brief Move constructor
    /// @param[in, out] object Rvalue reference
    ThreadPool(ThreadPool &&object) = delete;

    /// @brief Move assignment operator
    /// @param[in, out] object Rvalue reference
    /// @return Reference
    ThreadPool &operator=(ThreadPool &&object) = delete;

private:
    void _ActivateWorker();

    void _CreateNewThread();

public:
    /// @brief Get thread pool singleton
    /// @code{.isoft}
    /// export_level=/Diagnostics/Diagnostic Manager/Main
    /// @endcode
    /// @return Singleton
    static ThreadPool &GetInstance() noexcept;

    /// @brief Initialize thread pool
    /// @code{.isoft}
    /// export_level=/Diagnostics/Diagnostic Manager/Main
    /// @endcode
    /// @param[in] workerMax Maximum number of worker threads
    inline void Initialize(std::uint32_t const workerMax) noexcept
    {
        std::uint32_t const concurrency{std::thread::hardware_concurrency()};
        if (concurrency > workerMax) {
            workerMax_ = static_cast< std::uint16_t >(concurrency);
        } else {
            workerMax_ = static_cast< std::uint16_t >(workerMax);
        }
    }

    /// @brief Execute thread task
    /// @throw
    void Execute();

    /// @brief Start thread pool
    /// @code{.isoft}
    /// export_level=/Diagnostics/Diagnostic Manager/Main
    /// @endcode
    void Start() noexcept;

    /// @brief Stop thread pool
    /// @code{.isoft}
    /// export_level=/Diagnostics/Diagnostic Manager/Main
    /// @endcode
    void Stop() noexcept;

    /// @brief Submit task
    /// @code{.isoft}
    /// export_level=/Diagnostics/Diagnostic Manager/Main
    /// @endcode
    /// @param[in] task Task to execute
    /// @return Whether thread pool is running, if not running, submission fails
    template < class F, class... Args >
    bool Submit(F &&f, Args &&...args)
    {
        if (running_.load()) {
            {
                std::unique_lock< std::mutex > const lock{taskQueueLock_};
                taskQueue_.emplace([f, args...]() noexcept { f(args...); });
            }
            _ActivateWorker();
            return true;
        }
        return false;
    }

    /// @brief Get current number of tasks
    std::uint32_t GetTaskNum();
};

}  // namespace isoft

#endif  // _ISOFT_COMMON_THREAD_POOL_H_