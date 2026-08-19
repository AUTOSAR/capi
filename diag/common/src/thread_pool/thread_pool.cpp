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
/// @file       thread_pool.cpp
/// @brief
/// @details
/// @date       2024-11-13
/// @author     xuhua
/// @version    1.2.0
///
/// ================================================================

#include "thread_pool/thread_pool.h"

#include <cassert>
#include <cstring>

namespace isoft {

u_int32_t const kTaskWaitTime{1000U};

extern "C" nai_thread_api thread_proc(void *const arg)
{
    ThreadPool *const pThreadPool{static_cast< ThreadPool * >(arg)};
    if (nullptr == pThreadPool) {
        return nullptr;
    }
    pThreadPool->Execute();
    return static_cast< nai_thread_api >(pThreadPool);
}

void ThreadPool::_ActivateWorker()
{
    // Check thread nums and activate threads to take task
    if (mCreatedThreadNum_ >= workerMax_) {
        taskQueueCondition_.notify_all();
        return;
    }

    while (mCreatedThreadNum_ < workerMax_) {
        {
            std::unique_lock< std::mutex > const lock{taskQueueLock_};
            // assert(mCreatedThreadNum_ > mCurrentRunningThread_);
            if (static_cast< int16_t >(mCreatedThreadNum_) - static_cast< int16_t >(mCurrentRunningThread_)
                >= static_cast< int16_t >(taskQueue_.size())) {
                break;
            }
        }
        _CreateNewThread();
        taskQueueCondition_.notify_one();
    }
    taskQueueCondition_.notify_all();
}

void ThreadPool::_CreateNewThread()
{
    nai_int_t r{};
    nai_thread_t t{};
    std::ignore = nai_thread_init(&t);
    r           = nai_thread_create(&t, &a_, &thread_proc, this);
    if (r < 0) {
        return;
    }
    r = nai_thread_detach(&t);
    if (r < 0) {
        return;
    }
    std::ignore = t;
    ++mCreatedThreadNum_;
}

void ThreadPool::Execute()
{
    while (running_.load()) {
        do {
            std::function< void() > task;
            {
                std::unique_lock< std::mutex > lock{taskQueueLock_};
                if (taskQueue_.empty()) {
                    break;
                }
                task = std::move(taskQueue_.front());
                taskQueue_.pop();
            }
            if (nullptr != task) {
                ++mCurrentRunningThread_;
                task();
                --mCurrentRunningThread_;
            }
        } while (true);

        {
            std::unique_lock< std::mutex > lock{taskQueueLock_};
            std::cv_status const cvS{taskQueueCondition_.wait_for(lock, std::chrono::milliseconds{kTaskWaitTime})};
            if (std::cv_status::timeout == cvS) {
                if (taskQueue_.empty()) {
                    if (mCreatedThreadNum_.load() > workerMin_) {
                        break;  // Thread pool thread count reduction, thread exits when no tasks for one second
                    }
                }
            }
        }
    }
    --mCreatedThreadNum_;
}

ThreadPool::~ThreadPool() noexcept { Stop(); }

ThreadPool &ThreadPool::GetInstance() noexcept
{
    static ThreadPool s_ThreadPool{};
    return s_ThreadPool;
}

void ThreadPool::Start() noexcept { running_ = true; }

void ThreadPool::Stop() noexcept
{
    running_ = false;
    taskQueueCondition_.notify_all();
}

std::uint32_t ThreadPool::GetTaskNum()
{
    std::unique_lock< std::mutex > const lockTaskList{taskQueueLock_};
    return static_cast< std::uint32_t >(taskQueue_.size());
}

}  // namespace isoft