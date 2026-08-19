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
/// @date       2023-08-29
/// @author     zhaoyunfei
/// @version    1.2.0
///
/// ================================================================

#include "thread_pool.h"

#include <chrono>
#include <iostream>

#include "utils/ucmm_log.h"

namespace ara {
namespace ucm {
namespace vpkgmgr {

ThreadPool::ThreadPool(const char *name) : isRun_{false}, faileNum_{0}, totalNum_{0} { threadPoolName_ = name; }

/**
 * @brief
 *
 * @param threadNum
 * @return int32_t
 */
int32_t ThreadPool::Start(int32_t threadNum)
{
    threadNum_ = threadNum;
    if (threadNum_ <= 0) {
        threadNum_ = std::thread::hardware_concurrency();
    }
    workers_.clear();
    isRun_ = true;
    for (uint32_t i = 0; i < threadNum_; i++) {
        std::thread *t = new std::thread(std::bind(&ThreadPool::_task_func, this));
        // t->SetSetafftinityNp();
        // t->native_handle();
        // t->get_id();
        workers_.push_back(t);
    }
    return 0;
}

bool ThreadPool::IsStart() const { return isRun_; }

int32_t ThreadPool::GetThreadNum() const { return static_cast< int32_t >(threadNum_); }

int32_t ThreadPool::GetTaskNums()
{
    std::unique_lock< std::mutex > lock(queueMutex_);
    return static_cast< int32_t >(tasks_.size());
}

void ThreadPool::SetMaxTaskNums(int32_t maxTaskNum)
{
    if (maxTaskNum <= 0) {
        return;
    }
    maxTaskNum_ = maxTaskNum;
    return;
}

void ThreadPool::_task_func()
{
    std::thread::id const tid{std::this_thread::get_id()};
    std::stringstream tidstr;
    tidstr << tid;
    while (isRun_) {
        std::chrono::steady_clock::time_point nextPointTime = std::chrono::steady_clock::now();
        ProcFuncNode_t pTaskNode;
        {
            std::unique_lock< std::mutex > lock(queueMutex_);

            while (tasks_.empty()) {
                condition_.wait(lock);
                if (!isRun_) {
                    return;
                }
            }
            pTaskNode = tasks_.front();
            tasks_.pop_front();
        }

        if (!pTaskNode) {
            continue;
        }

        std::chrono::steady_clock::time_point getTaskTime = std::chrono::steady_clock::now();
        if (0 != pTaskNode(tid)) {
            faileNum_++;
        }
        totalNum_++;
        std::chrono::steady_clock::time_point endTime       = std::chrono::steady_clock::now();
        std::chrono::steady_clock::duration poolOneTaskTime = endTime - nextPointTime;
        std::chrono::steady_clock::duration taskExecTime    = endTime - getTaskTime;
        std::stringstream os;
        os << tid;
        constexpr int32_t kInt_1000_temp{1000};
        LOG_DEBUG << threadPoolName_ << " thread pool, tid=" << tidstr.str()
                  << " the time it takes to perform one task(competition) is "
                  << double(poolOneTaskTime.count()) / kInt_1000_temp << " us";
        LOG_DEBUG << threadPoolName_ << " thread pool, tid=" << tidstr.str()
                  << " the time it takes to perform one task is " << double(taskExecTime.count()) / kInt_1000_temp
                  << " us";
    }
    return;
}

int32_t ThreadPool::Stop()
{
    if (!isRun_) {
        return 0;
    }

    isRun_.store(false);
    condition_.notify_all();
    for (uint32_t i = 0; i < threadNum_; i++) {
        workers_[i]->join();
    }
    workers_.clear();
    LOG_DEBUG << __FUNCTION__;
    return 0;
}

int32_t ThreadPool::Commit(ProcFuncNode_t const &tasknode)
{
    if (!tasknode) {
        return 1;  // todo
    }

    if (!isRun_) {
        LOG_ERROR << "thread pool isn't run: " << threadPoolName_;
    }

    std::unique_lock< std::mutex > lock(queueMutex_);
    if (maxTaskNum_ > 0 && (tasks_.size() > maxTaskNum_)) {
        std::list< ProcFuncNode_t >::iterator iter;
        LOG_ERROR << "thread name=" << threadPoolName_ << " TaskNum=" << tasks_.size() << " limit num=" << maxTaskNum_
                  << " commit task failed!";
        return 1;
    }

    tasks_.push_back(tasknode);
    LOG_DEBUG << "thread pool name=" << threadPoolName_ << " queue size=" << tasks_.size();
    condition_.notify_one();
    return 0;
}

}  // namespace vpkgmgr
}  // namespace ucm
}  // namespace ara
