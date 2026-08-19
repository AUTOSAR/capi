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
/// @file       priority_queue.h
/// @brief      Template class for encapsulating log parameters
/// @details
/// @date       2024-06-28
/// @author     yangjinbiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/LOG/DltLog
/// @interface_level = unit
/// @trace_id_sr=LOG_SR_00001,LOG_SR_00002,LOG_SR_00003,LOG_SR_00004,LOG_SR_00006,LOG_SR_00010,LOG_SR_00011,LOG_SR_00012,LOG_SR_00013,LOG_SR_00014
/// @unit_name = priority_queue
/// @unit_description=Lib interface side of Dlt, internal support function
/// @endcode
///
/// ================================================================

#ifndef __LOG_INTERNAL_PRIORITY_QUEUE__
#define __LOG_INTERNAL_PRIORITY_QUEUE__

#include <array>
#include <atomic>
#include <functional>
#include <memory>

#include "concurrentqueue/moodycamel/concurrentqueue.h"
#include "determ_log_meta_data.h"
namespace ara {
namespace log {
namespace internal {

// /// @brief Task structure
// struct Task
// {
//     /// @brief Executed in the user thread when enqueuing, with locking
//     std::function< void() > preFunc_{nullptr};
//     /// @brief Executed in the worker thread
//     std::function< void() > postFunc_{nullptr};
//     /// @brief Task priority
//     std::uint8_t priority_{0U};
// };

/// @brief Priority queue
class PriorityQueue
{
public:
    /// @brief Default constructor
    explicit PriorityQueue(std::size_t queuelength)
    {
        for (std::size_t i = 0; i < queues_.size(); i++) {
            queues_[i] = new moodycamel::ConcurrentQueue< LogMetaData * >(queuelength);
        }
    }
    /// @brief Destructor
    ~PriorityQueue()
    {
        for (std::size_t i = 0; i < queues_.size(); i++) {
            delete queues_[i];
            queues_[i] = nullptr;
        }
    }

    /// @brief  Copy constructor
    /// @param other
    PriorityQueue(PriorityQueue const &other) = delete;
    /// @brief Copy assignment
    /// @param other
    /// @return Reference
    PriorityQueue &operator=(PriorityQueue const &other) = delete;
    /// @brief  Move constructor
    /// @param other
    PriorityQueue(PriorityQueue &&other) noexcept = delete;
    /// @brief Move assignment
    /// @param other
    /// @return Reference
    PriorityQueue &operator=(PriorityQueue &&other) noexcept = delete;

    /// @brief Add a task to the specified priority queue
    /// @param task Task
    /// @param storagePtr Memory pointer for the lock-free queue node storage
    void PushTask(LogMetaData *dataPtr)
    {
        if (queues_[0]->enqueue(dataPtr)) {
            totalTasks_.fetch_add(1, std::memory_order_release);
        }
    }

    /// @brief Add a task to the specified priority queue
    /// @param task Task
    /// @param storagePtr Memory pointer for the lock-free queue node storage
    void PushNetworkTask(LogMetaData *dataPtr)
    {
        if (queues_[0]->enqueue(dataPtr)) {
            totalTasks_.fetch_add(1, std::memory_order_release);
        }
    }
    /// @brief Attempt to retrieve a task starting from the highest priority
    /// @param task Task
    /// @return Whether the operation succeeded
    bool PopTask(LogMetaData *&dataPtr)
    {
        // Check closed state
        if (Size() == 0) {
            return false;
        }
        for (std::size_t i = 0; i < queues_.size(); ++i) {
            if (queues_[i]->try_dequeue(dataPtr)) {
                totalTasks_.fetch_sub(1, std::memory_order_release);
                return true;
            }
        }
        return false;
    }

    /// @brief Get the total number of tasks in the queue
    size_t Size() const { return totalTasks_.load(std::memory_order_relaxed); }

    /// @brief Check if the queue is empty
    bool Empty() const { return totalTasks_.load(std::memory_order_relaxed) == 0; }

private:
    /// @brief One queue per priority, currently only one
    std::array< moodycamel::ConcurrentQueue< LogMetaData * > *, 1 > queues_{};

    /// @brief Total number of pending tasks
    std::atomic< size_t > totalTasks_{0};
};

}  // namespace internal
}  // namespace log
}  // namespace ara
#endif