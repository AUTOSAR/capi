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
/// @brief
/// @details
/// @date       2023-08-29
/// @author     zhaoyunfei
/// @version    1.2.0
///
/// ================================================================

#ifndef __THREAD_POOL_H__
#define __THREAD_POOL_H__

#include <atomic>
#include <condition_variable>
#include <functional>
#include <future>
#include <iostream>
#include <list>
#include <memory>
#include <mutex>
#include <queue>
#include <vector>

namespace ara {
namespace ucm {
namespace vpkgmgr {

using ProcFuncNode_t = std::function< int32_t(std::thread::id const &) >;

class ThreadPool final
{
public:
    explicit ThreadPool(const char *name);
    ThreadPool()                   = default;
    ThreadPool(ThreadPool const &) = delete;
    ThreadPool(ThreadPool &&)      = delete;
    ThreadPool &operator=(const ThreadPool &) = delete;
    ThreadPool &operator=(ThreadPool &&) = delete;
    virtual ~ThreadPool()                = default;

    int32_t Start(int32_t threadNum);

    bool IsStart() const;

    int32_t GetThreadNum() const;

    int32_t GetTaskNums();

    void SetMaxTaskNums(int32_t maxTaskNum);

    int32_t Stop();

    int32_t Commit(ProcFuncNode_t const &tasknode);

private:
    void _task_func();

private:
    std::string threadPoolName_{""};
    std::atomic_bool isRun_{false};
    std::atomic_int_fast32_t faileNum_{0};
    std::atomic_int_fast32_t totalNum_{0};

    uint32_t threadNum_  = 0;
    uint32_t maxTaskNum_ = 0;
    std::mutex queueMutex_;
    std::condition_variable condition_;
    std::vector< std::thread * > workers_;
    std::list< ProcFuncNode_t > tasks_;
};

}  // namespace vpkgmgr
}  // namespace ucm
}  // namespace ara
#endif  //__THREAD_POOL_H__
