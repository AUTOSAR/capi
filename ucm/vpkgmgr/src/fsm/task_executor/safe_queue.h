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
/// @file       safe_queue.h
/// @brief      SafeQueue impl
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
/// @unit_name=SafeQueue
/// @unit_description=SafeQueue impl
/// @endcode
///
/// ================================================================

#ifndef SAFEQUEUE_HPP_
#define SAFEQUEUE_HPP_

#include <ara/core/optional.h>
#include <ara/log/logger.h>

#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <list>
#include <mutex>
#include <queue>
#include <thread>

namespace ara {
namespace ucm {
namespace vpkgmgr {

/// @brief Thread-safe asynchronous queue
/// @code{.isoft}
/// @tparam T
/// @tparam Container
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00335
/// @trace_id_dd=DD_UCM_Master_00673
/// @needwork = ad
/// @endcode
template < class T, class Container = std::list< T > >
/// @brief SafeQueue
class SafeQueue
{
    /// @brief value_type
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_
    /// @trace_id_dd=DD_UCM_
    /// @needwork = no
    /// @endcode
    typedef typename Container::value_type value_type;  // NOLINT
    /// @brief size_type
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_
    /// @trace_id_dd=DD_UCM_
    /// @needwork = no
    /// @endcode
    typedef typename Container::size_type size_type;  // NOLINT
    /// @brief container_type
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_
    /// @trace_id_dd=DD_UCM_
    /// @needwork = no
    /// @endcode
    typedef Container container_type;  // NOLINT

public:
    /// @brief constructor, Create safe queue
    /// @code{.isoft}
    /// @throw
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00674
    /// @needwork = dda
    /// @endcode
    SafeQueue() = default;
    /// @brief destructor, Destroy safe queue
    /// @code{.isoft}
    /// @throw
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00675
    /// @needwork = dda
    /// @endcode
    ~SafeQueue() noexcept { std::lock_guard< std::mutex > lock(mutex_); }

    /// @brief delete copy construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00676
    /// @needwork = dda
    /// @endcode
    SafeQueue(SafeQueue const& other) = delete;
    /// @brief delete copy asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00677
    /// @needwork = dda
    /// @endcode
    SafeQueue& operator=(SafeQueue const& other) = delete;
    /// @brief delete move construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00678
    /// @needwork = dda
    /// @endcode
    SafeQueue(SafeQueue&& other) = delete;
    /// @brief delete move asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00679
    /// @needwork = dda
    /// @endcode
    SafeQueue& operator=(SafeQueue&& other) = delete;

    /// @brief Sets the maximum number of items in the queue. Defaults is 0: No limit
    /// @param maxNumItems
    /// @return
    /// @code{.isoft}
    /// @throw
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00680
    /// @needwork = dda
    /// @endcode
    void SetMaxNumItems(int32_t const maxNumItems) { maxNumItems_ = maxNumItems; }

    /// @brief Pushes the item into the queue
    /// @param item An item
    /// @return true if an item was pushed into the queue
    /// @code{.isoft}
    /// @throw
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00681
    /// @needwork = dda
    /// @endcode
    bool Push(value_type const& item)
    {
        log_.LogVerbose() << "SafeQueuePushStop::Push(value_type const& item), begin.";
        std::lock_guard< std::mutex > const localLock{mutex_};

        if ((maxNumItems_ > 0U) && (queue_.size() > maxNumItems_)) {
            return false;
        }

        queue_.push(item);
        condition_.notify_one();
        log_.LogVerbose() << "SafeQueuePushStop::Push(value_type const& item), end.";
        return true;
    }

    /// @brief Pushes the item into the queue
    /// @param item An item
    /// @return true if an item was pushed into the queue
    /// @code{.isoft}
    /// @throw
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00682
    /// @needwork = dda
    /// @endcode
    bool Push(value_type&& item)
    {
        log_.LogVerbose() << "SafeQueuePushStop::Push(value_type&& item), begin.";
        std::lock_guard< std::mutex > const localLock{mutex_};

        if ((maxNumItems_ > 0U) && (queue_.size() > maxNumItems_)) {
            return false;
        }

        queue_.push(std::move(item));
        condition_.notify_one();
        log_.LogVerbose() << "SafeQueuePushStop::Push(value_type&& item), end.";
        return true;
    }

    /// @brief Pops item from the queue. If queue is empty, this function blocks until item becomes available
    /// @param item The item
    /// @code{.isoft}
    /// @throw
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00683
    /// @needwork = dda
    /// @endcode
    void Pop(value_type& item)
    {
        std::unique_lock< std::mutex > localLock{mutex_};
        condition_.wait(localLock, [this]() { return !queue_.empty(); });
        item = queue_.front();
        queue_.pop();
    }

    /// @brief Pops item from the queue using the contained type's move assignment operator, if it has one..
    ///  This method is identical to the pop() method if that type has no move assignment operator.
    ///  If queue is empty, this function blocks until item becomes available.
    /// @param item An item
    /// @code{.isoft}
    /// @throw
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00684
    /// @needwork = dda
    /// @endcode
    void MovePop(value_type& item)
    {
        std::unique_lock< std::mutex > localLock{mutex_};
        condition_.wait(localLock, [this]() { return !queue_.empty(); });
        item = std::move(queue_.front());
        queue_.pop();
    }

    /// @brief Tries to pop item from the queue
    /// @param item An item
    /// @return False is returned if no item is available
    /// @code{.isoft}
    /// @throw
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00685
    /// @needwork = dda
    /// @endcode
    bool TryPop(value_type& item)
    {
        std::unique_lock< std::mutex > const localLock{mutex_};

        if (queue_.empty()) {
            return false;
        }

        item = queue_.front();
        queue_.pop();
        return true;
    }

    /// @brief Tries to pop item from the queue using the contained type's move assignment operator, if it has one..
    ///  This method is identical to the try_pop() method if that type has no move assignment operator.
    /// @param item An item
    /// @return False is returned if no item is available
    /// @code{.isoft}
    /// @throw
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00686
    /// @needwork = dda
    /// @endcode
    bool TryMovePop(value_type& item)
    {
        std::unique_lock< std::mutex > const localLock{mutex_};

        if (queue_.empty()) {
            return false;
        }

        item = std::move(queue_.front());
        queue_.pop();
        return true;
    }

    /// @brief Stop
    /// @code{.isoft}
    /// @throw
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00687
    /// @needwork = dda
    /// @endcode
    void Stop()
    {
        log_.LogDebug() << "SafeQueue::Stop(), begin.";
        std::unique_lock< std::mutex > const localLock{mutex_};
        terminate_ = true;
        condition_.notify_all();
        log_.LogDebug() << "SafeQueue::Stop(), end.";
    }

    /// @brief Pops item from the queue. If the queue is empty, blocks for timeout microseconds, or until item becomes available
    /// @param item An item
    /// @param timeout The number of microseconds to wait
    /// @return true if get an item from the queue, false if no item is received before the timeout
    /// @code{.isoft}
    /// @throw
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00688
    /// @needwork = dda
    /// @endcode
    bool TimeoutPop(value_type& item, std::uint64_t const timeout)
    {
        std::unique_lock< std::mutex > localLock{mutex_};
        std::ignore = condition_.wait_for(localLock, std::chrono::milliseconds(timeout), [this]() {
            bool const term = terminate_.load();
            return (!queue_.empty()) || term;
        });
        if (queue_.empty()) {
            return false;
        }
        std::ignore = item;
        item        = queue_.front();
        queue_.pop();
        return true;
    }

    /// @brief Pops item from the queue using the contained type's move assignment operator, if it has one..
    ///  If the queue is empty, blocks for timeout milliseconds, or until item becomes available.
    ///  This method is identical to the try_pop() method if that type has no move assignment operator.
    /// @param item An item
    /// @param timeout The number of milliseconds to wait
    /// @return true if get an item from the queue, false if no item is received before the timeout
    /// @code{.isoft}
    /// @throw
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00689
    /// @needwork = dda
    /// @endcode
    bool TimeoutMovePop(value_type& item, std::uint64_t const timeout = 0U)
    {
        std::unique_lock< std::mutex > localLock{mutex_};
        std::ignore = condition_.wait_for(localLock, std::chrono::milliseconds(timeout), [this]() {
            bool const term = terminate_.load();
            return (!queue_.empty()) || term;
        });
        if (queue_.empty()) {
            return false;
        }
        std::ignore = item;
        item        = std::move(queue_.front());
        queue_.pop();
        return true;
    }

    /// @brief Gets the number of items in the queue
    /// @return number of items in the queue
    /// @code{.isoft}
    /// @throw
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00690
    /// @needwork = dda
    /// @endcode
    size_type Size() const
    {
        std::lock_guard< std::mutex > const localLock{mutex_};
        return queue_.size();
    }

    /// @brief Check if the queue is empty
    /// @return bool
    /// @code{.isoft}
    /// @throw
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00691
    /// @needwork = dda
    /// @endcode
    bool Empty() const
    {
        std::lock_guard< std::mutex > const localLock{mutex_};
        return queue_.empty();
    }

    /// @brief Swaps the contents
    /// @param sq The SafeQueue to swap with 'this'
    /// @code{.isoft}
    /// @throw
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00692
    /// @needwork = dda
    /// @endcode
    void Swap(SafeQueue& sq)
    {
        if (this != &sq) {
            std::lock_guard< std::mutex > lock1(mutex_);
            std::lock_guard< std::mutex > lock2(sq.mutex_);
            queue_.swap(sq.queue_);

            if (!queue_.empty()) {
                condition_.notify_all();
            }

            if (!sq.queue_.empty()) {
                sq.condition_.notify_all();
            }
        }
    }

private:
    /// @brief queue_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00693
    /// @needwork = dda
    /// @endcode
    std::queue< T, Container > queue_;
    /// @brief mutex_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00694
    /// @needwork = dda
    /// @endcode
    mutable std::mutex mutex_;
    /// @brief condition_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00695
    /// @needwork = dda
    /// @endcode
    std::condition_variable condition_;
    /// @brief maxNumItems_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00696
    /// @needwork = dda
    /// @endcode
    uint32_t maxNumItems_{0U};

    /// @brief terminate_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00697
    /// @needwork = dda
    /// @endcode
    std::atomic< bool > terminate_{false};

    /// @brief Logger for logging errors
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00698
    /// @needwork = dda
    /// @endcode
    ara::log::Logger& log_{ara::log::CreateLogger(std::move(ara::core::StringView("#UCM")),
                                                  std::move(ara::core::StringView("SafeQueue context")),
                                                  ara::log::LogLevel::kVerbose)};
};

/// @brief Swaps the contents of two SafeQueue objects
/// @param q1
/// @param q2
/// @throw
// template <class T, class Container>.
// void swap(SafeQueue<T, Container>& q1, SafeQueue<T, Container>& q2) {.
//     q1.swap(q2);.
// }.

}  // namespace vpkgmgr
}  // namespace ucm
}  // namespace ara

#endif /* SAFEQUEUE_HPP_ */
