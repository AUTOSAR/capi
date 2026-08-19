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
/// @unit_name = thread_pool
/// @unit_description=Lib interface side of Dlt, internal support function
/// @endcode
///
/// ================================================================

#ifndef __LOG_INTERNAL_THREAD_POOL__
#define __LOG_INTERNAL_THREAD_POOL__

#include <atomic>
#include <ctime>
#include <iomanip>
#include <memory>
#include <thread>
#include <tuple>
#include <vector>

#include "DltLogDefines/logger_scope_data.h"
#include "DltSinkers/remote_sinker.h"
#include "Utils/src/private_log.h"
#include "determ_log_meta_data.h"
#include "memory_pool.h"
#include "priority_queue.h"
namespace ara {
namespace log {
namespace internal {

/// @brief Console cache length
constexpr std::size_t kConsoleStrBufferLen{2048000U};

/// @brief Encode length
constexpr std::size_t kConsoleStrBufferPad{2000U};

/// @brief Encode length
constexpr std::int32_t kLogPriority{50};
/// @brief Implementation of the task thread pool
class ThreadPool
{
public:
    /// @brief Default constructor
    ThreadPool() = default;

    // Destructor, cleans up the thread pool
    ~ThreadPool() = default;

    /// @brief  Copy constructor
    /// @param other
    ThreadPool(ThreadPool const &other) = delete;
    /// @brief Copy assignment
    /// @param other
    /// @return Reference
    ThreadPool &operator=(ThreadPool const &other) = delete;
    /// @brief  Move constructor
    /// @param other
    ThreadPool(ThreadPool &&other) noexcept = delete;
    /// @brief Move assignment
    /// @param other
    /// @return Reference
    ThreadPool &operator=(ThreadPool &&other) noexcept = delete;

    /// @brief  If you start 1 thread, it handles everything; with 2 threads, it handles files
    void CreateBasicThread(std::size_t queuelength, std::shared_ptr< GlobalMemoryPool > &globalPool)
    {
        taskQueue_ = new PriorityQueue(queuelength);
        consoleString_.reserve(kConsoleStrBufferLen + kConsoleStrBufferPad);
        globalPool_ = globalPool;
        workers_.emplace_back([this] {
            struct sched_param param
            {
            };
            param.sched_priority    = kLogPriority;
            pthread_t currentThread = pthread_self();
            std::ignore             = pthread_setschedparam(currentThread, SCHED_FIFO, &param);

            bool run{true};
            while (run) {
                while (taskQueue_->Empty() && !stop_.load(std::memory_order_relaxed)) {
                    if ((false == IsNetworkThreadWorking()) && (consoleString_.length() > 0)) {
                        std::cout << consoleString_;
#ifdef __android__
                        __android_log_print(ANDROID_LOG_INFO, "LOGTAG", consoleString_.c_str());
#endif
                        consoleString_.clear();
                    }
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                }
                if (taskQueue_->Empty() && stop_.load(std::memory_order_relaxed)) {
                    run = false;
                    break;
                }
                bool bHasTask{true};
                while (bHasTask) {
                    LogMetaData *dataPtr(nullptr);
                    bool gotTask{taskQueue_->PopTask(dataPtr)};

                    if (gotTask) {
                        if (nullptr != dataPtr) {
                            if (true == dataPtr->toConsle) {
                                globalPool_->CheckAndExpand();
                                _FlushToConsole(dataPtr);
                            }
                            // Handles network when single-threaded
                            if (false == IsNetworkThreadWorking()) {
                                if (true == dataPtr->toRemote) {
                                    globalPool_->CheckAndExpand();
                                    if ((dataPtr->message.headersize > 0U)
                                        && (dataPtr->message.standardheader != nullptr)) {
                                        std::uint8_t *headerPtr
                                            = reinterpret_cast< std::uint8_t * >(dataPtr->message.standardheader);
                                        std::uint32_t const headerSize{dataPtr->message.headersize};
                                        std::uint8_t *bodyPtr{dataPtr->buffer.data()};
                                        std::uint32_t const bodySize{static_cast< std::uint32_t >(dataPtr->size)};
                                        if (dataPtr->toRemote && (RemoteSinker::Instance() != nullptr)) {
                                            std::ignore = RemoteSinker::Instance()->SaveLog(headerPtr, headerSize,
                                                                                            bodyPtr, bodySize);
                                        }
                                    }
                                }
                            }
                            if (true == dataPtr->toFile) {
                                globalPool_->CheckAndExpand();
                                if ((dataPtr->message.headersize > 0U)
                                    && (dataPtr->message.standardheader != nullptr)) {
                                    std::uint8_t *headerPtr
                                        = reinterpret_cast< std::uint8_t * >(dataPtr->message.standardheader);
                                    std::uint32_t const headerSize{dataPtr->message.headersize};
                                    std::uint8_t *bodyPtr{dataPtr->buffer.data()};
                                    std::uint32_t const bodySize{static_cast< std::uint32_t >(dataPtr->size)};
                                    constexpr std::uint64_t kMICROSECONDS_PER_SECOND = 1000000ULL;
                                    DltTimeStamp dltTimestamp;
                                    dltTimestamp.seconds  = dataPtr->timeStamp / kMICROSECONDS_PER_SECOND;
                                    dltTimestamp.uSeconds = dataPtr->timeStamp % kMICROSECONDS_PER_SECOND;
                                    if (nullptr != dataPtr->rawFileSinker) {
                                        std::ignore = dataPtr->rawFileSinker->SaveLogWithTimeStamp(
                                            headerPtr, headerSize, bodyPtr, bodySize, dltTimestamp);
                                    }
                                }
                            }
                            // dataPtr->encoder = nullptr;
                            globalPool_->FreeBlock(reinterpret_cast< std::uint8_t * >(dataPtr));
                        }
                    } else {
                        // Queue is empty and closed, exit thread
                        if (taskQueue_->Empty()) {
                            if (stop_.load(std::memory_order_relaxed)) {
                                run = false;
                            }
                            bHasTask = false;
                        }
                        break;
                    }
                }
            }
        });
    }
    /// @brief  Create a thread to handle network and console. Only needed when both file & console exist.
    void CreateConsoleThread(std::size_t queuelength)
    {
        networktaskQueue_ = new PriorityQueue(queuelength);
        workers_.emplace_back([this] {
            struct sched_param param
            {
            };
            param.sched_priority    = kLogPriority;
            pthread_t currentThread = pthread_self();
            std::ignore             = pthread_setschedparam(currentThread, SCHED_FIFO, &param);

            bool run{true};
            while (run) {
                while (networktaskQueue_->Empty() && !stop_.load(std::memory_order_relaxed)) {
                    if (consoleString_.length() > 0) {
                        std::cout << consoleString_;
#ifdef __android__
                        __android_log_print(ANDROID_LOG_INFO, "LOGTAG", consoleString_.c_str());
#endif
                        consoleString_.clear();
                    }
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                }
                if (networktaskQueue_->Empty() && stop_.load(std::memory_order_relaxed)) {
                    run = false;
                    break;
                }
                bool bHasTask{true};
                while (bHasTask) {
                    LogMetaData *dataPtr(nullptr);
                    bool gotTask{networktaskQueue_->PopTask(dataPtr)};

                    if (gotTask) {
                        if (nullptr != dataPtr) {
                            globalPool_->CheckAndExpand();
                            if (true == dataPtr->toRemote) {
                                if ((dataPtr->message.headersize > 0U)
                                    && (dataPtr->message.standardheader != nullptr)) {
                                    std::uint8_t *headerPtr
                                        = reinterpret_cast< std::uint8_t * >(dataPtr->message.standardheader);
                                    std::uint32_t const headerSize{dataPtr->message.headersize};
                                    std::uint8_t *bodyPtr{dataPtr->buffer.data()};
                                    std::uint32_t const bodySize{static_cast< std::uint32_t >(dataPtr->size)};
                                    if (dataPtr->toRemote && RemoteSinker::Instance() != nullptr) {
                                        std::ignore = RemoteSinker::Instance()->SaveLog(headerPtr, headerSize, bodyPtr,
                                                                                        bodySize);
                                    }
                                }
                            }
                            if (true == dataPtr->toConsle) {
                                globalPool_->CheckAndExpand();
                                _FlushToConsole(dataPtr);
                            }
                            if (true == dataPtr->toFile) {
                                this->AddTask(dataPtr);
                            } else {
                                globalPool_->FreeBlock(reinterpret_cast< std::uint8_t * >(dataPtr));
                            }
                        }
                    } else {
                        // Queue is empty and closed, exit thread
                        if (networktaskQueue_->Empty()) {
                            if (stop_.load(std::memory_order_relaxed)) {
                                run = false;
                            }
                            bHasTask = false;
                        }
                        break;
                    }
                }
            }
        });
    }
    /// @brief  Clean up the thread pool
    void DestroyThreads() noexcept
    {
        stop_.store(true, std::memory_order_relaxed);
        for (std::thread &worker : workers_) {
            if (worker.joinable()) {
                worker.join();
            }
        }
        workers_.clear();
        if (nullptr != taskQueue_) {
            delete taskQueue_;
            taskQueue_ = nullptr;
        }
        if (nullptr != networktaskQueue_) {
            delete networktaskQueue_;
            networktaskQueue_ = nullptr;
        }
        if (consoleString_.length() > 0) {
            std::cout << consoleString_;
#ifdef __android__
            __android_log_print(ANDROID_LOG_INFO, "LOGTAG", consoleString_.c_str());
#endif
            consoleString_.clear();
        }
    }

    /// @brief  Add a task to the thread pool
    void AddTask(LogMetaData *dataPtr) noexcept { taskQueue_->PushTask(dataPtr); }

    /// @brief  Add a task to the thread pool
    void AddNetworkTask(LogMetaData *dataPtr) noexcept { networktaskQueue_->PushNetworkTask(dataPtr); }

    /// @brief  Whether a dedicated thread handles the network
    bool IsNetworkThreadWorking() const { return workers_.size() > 1U; }
    void PrintTaskCount() const
    {
        std::size_t taskQueueCount = 0;
        if (nullptr != taskQueue_) {
            taskQueueCount = taskQueue_->Size();
        }
        std::size_t networktaskQueueCount = 0;
        if (nullptr != networktaskQueue_) {
            networktaskQueueCount = networktaskQueue_->Size();
        }
        LOGERROR(__func__) << ", pid=" << getpid() << ",taskQueue_ =" << taskQueueCount
                           << ", networktaskQueue_=" << networktaskQueueCount;
    }

private:
    void _FlushToConsole(LogMetaData *dataPtr)
    {
        std::ostringstream ossConsole;
        ossConsole << dataPtr->buffer.data() << std::endl;
        consoleString_ += ossConsole.str();
        if (consoleString_.length() > kConsoleStrBufferLen) {
            std::cout << consoleString_;
#ifdef __android__
            __android_log_print(ANDROID_LOG_INFO, "LOGTAG", consoleString_.c_str());
#endif
            consoleString_.clear();
        }
    }

private:
    /// @brief  If you start 1 thread, it handles everything; with 2 threads, it only handles the console
    PriorityQueue *taskQueue_{nullptr};
    /// @brief  With 2 threads, this queue only handles files and network
    PriorityQueue *networktaskQueue_{nullptr};
    /// @brief  Collection of threads
    std::vector< std::thread > workers_{};
    /// @brief  Process end flag
    std::atomic< bool > stop_{false};
    /// @brief  Global memory pool
    std::shared_ptr< GlobalMemoryPool > globalPool_{nullptr};
    /// @brief  Local buffer
    std::string consoleString_{};

};  // namespace internal

}  // namespace internal
}  // namespace log
}  // namespace ara
#endif
