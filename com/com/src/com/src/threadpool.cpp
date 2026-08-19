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
/// @file       threadpool.cpp
/// @brief      Thread pool implementation file
/// @details
/// @date       2023-10-07
/// @author     mazelin
/// @version    1.2.0
///
/// ================================================================

#include "ara/com/internal/threadpool.h"

#include <deque>
#include <list>

#include "ara/com/internal/log/log.h"

namespace ara {
namespace com {
namespace internal {
/// @brief Thread pool implementation type
class ThreadPool::Impl
{
public:
    /// @brief Constructor
    /// @param[in] size Number of threads
    explicit Impl(ara::core::StringView name, size_t size) noexcept : name_{name}, size_{size}, runable_{false}
    {
        Start();
    }
    /// @brief Destructor
    ~Impl() noexcept { Stop(); }
    /// @brief Copy constructor
    /// @param other
    Impl(Impl const& other) noexcept = delete;
    /// @brief Move constructor
    /// @param other
    Impl(Impl&& other) noexcept = delete;
    /// @brief Copy assignment operator
    /// @param other
    /// @return Impl
    Impl& operator=(Impl const& other) noexcept = delete;
    /// @brief Move assignment operator
    /// @param other
    /// @return Impl
    Impl& operator=(Impl&& other) noexcept = delete;
    /// @brief Start the thread pool
    void Start() noexcept
    {
        ComLogTrace("start thread pool", GenArg(this));
        if (runable_) {
            return;
        }
        runable_ = true;
        while (threads_.size() < size_) {
            threads_.emplace_back(&Impl::Run, this, threads_.size() + 1);
        }
        ComLogDebug("start thread pool done", GenArg(this));
    }
    /// @brief Stop the thread pool
    void Stop() noexcept
    {
        ComLogTrace("stop thread pool", GenArg(this));
        if (!runable_) {
            return;
        }
        runable_ = false;
        cv_.notify_all();
        for (auto& thread : threads_) {
            if (thread.joinable()) {
                thread.join();
            }
        }
        ComLogDebug("stop thread pool done", GenArg(this));
    }
    /// @brief Execute thread pool tasks
    void Run(size_t tid) noexcept
    {
        constexpr auto kMaxNameSize{32};
        char name[kMaxNameSize]{};
        std::ignore = snprintf(name, sizeof(name), "%016lx",
                               true ? tid : std::hash< std::thread::id >{}(std::this_thread::get_id()));
        ComLogTrace("run thread", GenArg(name), GenArg(this));
        std::ignore = pthread_getname_np(pthread_self(), name, sizeof(name));
        while (true) {
            Task task;
            {
                std::unique_lock< decltype(mutex_) > lock{mutex_};
                cv_.wait(lock, [this] { return !runable_ || !tasks_.empty(); });
                if (!runable_ && tasks_.empty()) {
                    break;
                }
                task = std::move(tasks_.front());
                tasks_.pop_front();
            }
            ComLogTrace("run thread task", GenArg(name));
            task();
            ComLogTrace("run thread task done", GenArg(name));
        }
        ComLogDebug("run thread done", GenArg(name), GenArg(this));
    }
    /// @brief Get number of threads
    size_t Size() const noexcept { return threads_.size(); }
    /// @brief Get number of tasks
    size_t Count() const noexcept { return tasks_.size(); }
    /// @brief Dispatch task (no return)
    /// @param[in] task Task to dispatch
    void Dispatch(Task&& task) noexcept
    {
        if (!runable_) {
            ComLogDebug("dispatch thread failed", GenArg(this));
            return;
        }
        if (threads_.empty()) {
            ComLogTrace("run task");
            task();
            ComLogTrace("run task done");
            return;
        }
        ComLogTrace("dispatch thread", GenArg(this));
        {
            std::unique_lock< decltype(mutex_) > lock{mutex_};
            tasks_.emplace_back(std::forward< Task >(task));
            cv_.notify_one();
        }
        ComLogTrace("dispatch thread done", GenArg(this));
    }

    /// @brief Type alias -- Mark as enumerable
    using IsEnumerableTag = void;
    /// @brief Enumeration operation -- enumerate members of this type via a passed function object
    /// @tparam F Passed function object type
    /// @param[in] fun Passed function object
    template < typename F >
    void enumerate(F& fun)  // NOLINT -- Template interface name > naming convention
    {
        fun("name", name_);
        fun("size", size_);
        fun("runable", !!runable_);
        fun("tasks", static_cast< int32_t >(tasks_.size()));
    }

private:
    /// @brief Thread pool name
    ara::core::String name_;
    /// @brief Number of threads
    size_t size_;
    /// @brief Is runnable
    std::atomic< bool > runable_;
    /// @brief Condition variable
    std::condition_variable cv_;
    /// @brief Task queue
    std::deque< Task > tasks_;
    /// @brief Thread queue
    std::list< std::thread > threads_;
    /// @brief Mutex
    std::mutex mutex_;
};
ThreadPool::ThreadPool() noexcept : ThreadPool{static_cast< size_t >(std::thread::hardware_concurrency())} {}
ThreadPool::ThreadPool(size_t size) noexcept : impl_{std::make_unique< ThreadPool::Impl >("", size)} {}
ThreadPool::ThreadPool(ara::core::StringView name, size_t size) noexcept
    : impl_{std::make_unique< ThreadPool::Impl >(name, size)}
{
}
ThreadPool::~ThreadPool() noexcept = default;
ThreadPool::operator bool() const noexcept { return impl_ != nullptr; }
size_t ThreadPool::Size() const noexcept { return impl_->Size(); }
size_t ThreadPool::Count() const noexcept { return impl_->Count(); }
void ThreadPool::Dispatch(Task&& task) noexcept { impl_->Dispatch(std::forward< Task >(task)); }
void ThreadPool::operator()(Task&& task) noexcept { impl_->Dispatch(std::forward< Task >(task)); }
}  // namespace internal
}  // namespace com
}  // namespace ara
