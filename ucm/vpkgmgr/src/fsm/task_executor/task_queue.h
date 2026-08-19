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
/// @file       task_queue.h
/// @brief      TaskQueue impl
/// @details
/// @date       2024-07-20
/// @author     hanzhibo
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// export_level=/
/// @module_path=/UCM Master/Fsm
/// @interface_level=unit
/// @trace_id_sr=SR_UCM_00035
/// @unit_name=TaskQueue
/// @unit_description=TaskQueue impl
/// @endcode
///
/// ================================================================

#ifndef _TASK_QUEUE_H__
#define _TASK_QUEUE_H__

#include <functional>
#include <thread>
#include <utility>

#include "fsm/task_executor/safe_queue.h"
#include "utils/single_ton.h"
#include "utils/ucmm_log.h"

namespace ara {
namespace ucm {
namespace vpkgmgr {

/// @brief Used to bind task execution functions
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
using TaskFuncType = std::function< int32_t() >;

/// @brief Used for
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00336
/// @trace_id_dd=DD_UCM_Master_00699
/// @needwork = ad
/// @endcode
class TaskQueue : public Singleton< TaskQueue >
{
public:
    /// @brief constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00337
    /// @trace_id_dd=DD_UCM_Master_00700
    /// @needwork = ad
    /// @endcode
    TaskQueue() = default;
    /// @brief destructor
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00338
    /// @trace_id_dd=DD_UCM_Master_00701
    /// @needwork = ad
    /// @endcode
    ~TaskQueue() noexcept
    {
        LOG_DEBUG << "TaskQueue::~TaskQueue begin.";
        if (nullptr != t_.get()) {
            if (t_->joinable()) {
                t_->join();
                t_.reset(nullptr);
            }
        }
        LOG_DEBUG << "TaskQueue::~TaskQueue end.";
    }

    /// @brief delete copy construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00339
    /// @trace_id_dd=DD_UCM_Master_00702
    /// @needwork = ad
    /// @endcode
    TaskQueue(TaskQueue const& other) = delete;
    /// @brief delete copy asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00340
    /// @trace_id_dd=DD_UCM_Master_00703
    /// @needwork = ad
    /// @endcode
    TaskQueue& operator=(TaskQueue const& other) = delete;
    /// @brief delete move construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00341
    /// @trace_id_dd=DD_UCM_Master_00704
    /// @needwork = ad
    /// @endcode
    TaskQueue(TaskQueue&& other) = delete;
    /// @brief delete move asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00342
    /// @trace_id_dd=DD_UCM_Master_00705
    /// @needwork = ad
    /// @endcode
    TaskQueue& operator=(TaskQueue&& other) = delete;

    /// @brief PushTask
    /// @param task
    /// @return bool
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00343
    /// @trace_id_dd=DD_UCM_Master_00706
    /// @needwork = ad
    /// @endcode
    bool PushTask(TaskFuncType&& task) { return tasksQueue_.Push(std::move(task)); }

    /// @brief SyncRun
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00344
    /// @trace_id_dd=DD_UCM_Master_00707
    /// @needwork = ad
    /// @endcode
    void SyncRun()
    {
        isRun_      = true;
        std::ignore = _run();
    }

    /// @brief AsyncRun
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00345
    /// @trace_id_dd=DD_UCM_Master_00708
    /// @needwork = ad
    /// @endcode
    void AsyncRun()
    {
        if (nullptr != t_.get()) {
            LOG_WARN << "TaskQueue is already async run!!!";
            return;
        }
        t_ = std::make_unique< std::thread >([this]() { std::ignore = this->_run(); });
    }

    /// @brief Stop
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00346
    /// @trace_id_dd=DD_UCM_Master_00709
    /// @needwork = ad
    /// @endcode
    void Stop()
    {
        isRun_ = false;
        tasksQueue_.Stop();
    };

private:
    /// @brief Run
    /// @return reuslt of run
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00710
    /// @needwork = dda
    /// @endcode
    int32_t _run()
    {
        LOG_INFO << "";
        isRun_ = true;
        while (isRun_) {
            TaskFuncType taskFunc;
            std::uint64_t const timeout{100U};
            if (!tasksQueue_.TimeoutPop(taskFunc, timeout)) {
                continue;
            }

            if (nullptr == taskFunc) {
                continue;
            }

            LOG_DEBUG << "TaskQueue: one new task to run";
            if (0 != taskFunc()) {
                ///LOG_WARN << "TaskQueue: taskFunc run error!";
            }
        }
        return 0;
    }

private:
    /// @brief isRun_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00711
    /// @needwork = dda
    /// @endcode
    bool isRun_{false};
    /// @brief t_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00712
    /// @needwork = dda
    /// @endcode
    std::unique_ptr< std::thread > t_;
    /// @brief tasksQueue_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00713
    /// @needwork = dda
    /// @endcode
    SafeQueue< TaskFuncType > tasksQueue_;
};

}  // namespace vpkgmgr
}  // namespace ucm
}  // namespace ara

#endif