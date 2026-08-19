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
/// @file       threadpool.h
/// @brief      Thread pool header file
/// @details
/// @date       2023-10-07
/// @author     mazelin
/// @version    1.2.0
///
/// ================================================================

#pragma once

#include "ara/core/promise.h"

namespace ara {
namespace com {
namespace internal {
/// @brief Thread pool type
/// @code{.isoft}
/// export_level=/COM
/// @endcode
class ThreadPool
{
public:
    /// @brief Task type signature
    using Task = std::function< void() >;
    /// @brief Constructor
    /// @code{.isoft}
    /// export_level=/COM
    /// @endcode
    ThreadPool() noexcept;
    /// @brief Constructor
    /// @code{.isoft}
    /// export_level=/COM
    /// @endcode
    /// @param[in] size Number of threads
    explicit ThreadPool(size_t size) noexcept;
    /// @brief Constructor
    /// @code{.isoft}
    /// export_level=/COM
    /// @endcode
    /// @param[in] name Thread pool name
    /// @param[in] size Number of threads
    explicit ThreadPool(ara::core::StringView name, size_t size) noexcept;
    /// @brief Destructor
    ~ThreadPool() noexcept;
    /// @brief Copy constructor
    /// @param other
    ThreadPool(ThreadPool const& other) noexcept = delete;
    /// @brief Move constructor
    /// @param other
    ThreadPool(ThreadPool&& other) noexcept = default;
    /// @brief Copy assignment operator
    /// @param other
    /// @return ThreadPool
    ThreadPool& operator=(ThreadPool const& other) noexcept = delete;
    /// @brief Move assignment operator
    /// @param other
    /// @return ThreadPool
    ThreadPool& operator=(ThreadPool&& other) noexcept = default;
    /// @brief Check if thread pool is available
    /// @code{.isoft}
    /// export_level=/COM
    /// @endcode
    /// @return Is available
    explicit operator bool() const noexcept;
    /// @brief Get number of threads
    /// @code{.isoft}
    /// export_level=/COM
    /// @endcode
    /// @return Number of threads
    size_t Size() const noexcept;
    /// @brief Get number of tasks
    /// @code{.isoft}
    /// export_level=/COM
    /// @endcode
    /// @return Number of tasks
    size_t Count() const noexcept;
    /// @brief Dispatch task (no return)
    /// @code{.isoft}
    /// export_level=/COM
    /// @endcode
    /// @param[in] task Task to dispatch
    void Dispatch(Task&& task) noexcept;
    /// @brief Dispatch task (no return)
    /// @code{.isoft}
    /// export_level=/COM
    /// @endcode
    /// @param[in] task Task to dispatch
    void operator()(Task&& task) noexcept;
    /// @brief Dispatch task (asynchronous return)
    /// @code{.isoft}
    /// export_level=/COM
    /// @endcode
    /// @tparam Func Callable object type: T/ara::core::Result<T, E>/ara::core::Future<T, E>(Args...)
    /// @tparam Args Callable object parameter list types
    /// @param[in] func Callable object
    /// @param[in] args Callable object parameter list
    /// @return ara::core::Future<T, E> Consumer's future return
    template < typename Func, typename... Args >
    inline auto Async(Func&& func, Args&&... args) noexcept
    {
        using Promise = PromiseWrapper< std::result_of_t< Func(Args...) > >;
        auto promise{std::make_shared< typename Promise::PromiseType >()};
        auto future{promise->get_future()};
        auto task{std::bind(std::forward< Func >(func), std::forward< Args >(args)...)};
        Dispatch([promise{std::move(promise)}, task{std::move(task)}] {
            try {
                Promise::SetValue(*promise, task());
            } catch (...) {
                promise->set_exception(std::current_exception());
            }
        });
        return future;
    }
    /// @brief Provider promise class (encapsulates promise; non-specialized)
    /// @tparam T Default type
    template < typename T >
    struct PromiseWrapper
    {
        /// @brief Type alias -- Promise type
        using PromiseType = ara::core::Promise< T >;
        /// @brief Set value
        /// @param[in] promise Promise
        /// @param[in] value Value
        static void SetValue(PromiseType& promise, T&& value) noexcept { promise.set_value(std::move(value)); }
    };
    /// @brief Provider promise class (encapsulates promise; specialized for result)
    /// @tparam T Possible data type of the result (including void)
    /// @tparam E Possible error type of the result
    template < typename T, typename E >
    struct PromiseWrapper< ara::core::Result< T, E > >
    {
        /// @brief Type alias -- Promise type
        using PromiseType = ara::core::Promise< T, E >;
        /// @brief Set value
        /// @tparam c Specialization condition -- Is it a void type? (No)
        /// @param[in] promise Promise
        /// @param[in] result Result
        template < bool c = std::is_same< T, void >::value >
        static void SetValue(PromiseType& promise,
                             ara::core::Result< T, E >&& result,
                             std::enable_if_t< !c >* = nullptr) noexcept
        {
            if (result) {
                PromiseWrapper< T >::SetValue(promise, std::move(result).Value());
            } else {
                promise.SetError(std::move(result).Error());
            }
        }
        /// @brief Set value
        /// @tparam c Specialization condition -- Is it a void type? (Yes)
        /// @param[in] promise Promise
        /// @param[in] result Result
        template < bool c = std::is_same< T, void >::value >
        static void SetValue(PromiseType& promise,
                             ara::core::Result< T, E >&& result,
                             std::enable_if_t< c >* = nullptr) noexcept
        {
            if (result) {
                promise.set_value();
            } else {
                promise.SetError(std::move(result).Error());
            }
        }
    };
    /// @brief Provider promise class (encapsulates promise; specialized for future)
    /// @tparam T Possible data type of the result (including void)
    /// @tparam E Possible error type of the result
    template < typename T, typename E >
    struct PromiseWrapper< ara::core::Future< T, E > >
    {
        /// @brief Type alias -- Promise type
        using PromiseType = ara::core::Promise< T, E >;
        /// @brief Set value
        /// @param[in] promise Promise
        /// @param[in] future Future
        static void SetValue(PromiseType& promise, ara::core::Future< T, E >&& future) noexcept
        {
            PromiseWrapper< ara::core::Result< T, E > >::SetValue(promise, std::move(future).GetResult());
        }
    };

    /// @brief Type alias -- Mark as enumerable
    using IsEnumerableTag = void;
    /// @brief Enumeration operation -- enumerate members of this type via a passed function object
    /// @tparam F Passed function object type
    /// @param[in] fun Passed function object
    template < typename F >
    void enumerate(F& fun)  // NOLINT -- Template interface name > naming convention
    {
        fun(impl_);
    }

private:
    /// @brief Thread pool implementation type -- Declaration
    class Impl;
    /// @brief Thread pool implementation
    std::unique_ptr< Impl > impl_;
};
}  // namespace internal
}  // namespace com
}  // namespace ara