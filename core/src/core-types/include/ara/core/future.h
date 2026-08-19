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
/// @file       future.h
/// @brief      The definition and implementation of ara::core::Future type that is a composition of basic features of std::future and methods borrowed from the C++ proposal N3857.
/// @details
/// @date       2022-06-21
/// @author     hanzhibo
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_CORE_FUTURE_H_
#define ARA_CORE_FUTURE_H_

#include <cassert>
#include <chrono>
#include <future>
#include <system_error>

#include "ara/core/core_error_domain.h"
#include "ara/core/error_code.h"
#include "ara/core/exception.h"
#include "ara/core/future_error_domain.h"
#include "ara/core/internal/callback_agent.h"
#include "ara/core/internal/callback_agent_arg.h"
#include "ara/core/internal/state.h"
#include "ara/core/result.h"

#if !defined(ATTR_NODISCARD)
    #if __cplusplus >= 201703L
        #define ar_attribute_nodiscard [[nodiscard]]
    #else
        #if defined(__GNUC__) || defined(__clang__)
            #define ar_attribute_nodiscard __attribute__((warn_unused_result))
        #else
            #define ar_attribute_nodiscard /* nothing */
        #endif
    #endif
    #define ATTR_NODISCARD ar_attribute_nodiscard
#endif

namespace ara {
namespace core {

/* Forward declaration */
template < typename, typename >
class Promise;

/**
 * @brief Specifies the state of a Future as returned by wait_for() and wait_until().
 *
 * These definitions are equivalent to the ones from std::future_status. However, the
 * item std::future_status::deferred is not supported here.
 *
 * @uptrace{SWS_CORE_00361, 23094eab1b9d3c0176a11f646673531fa5f77655}
 */
enum class future_status : uint8_t  // NOLINT
{
    ///< the shared state is ready
    ready = 1,  // NOLINT
    ///< the shared state did not become ready before the specified timeout has passed
    timeout,  // NOLINT
};

// deleted by hanzhibo
// inline std::ostream& operator<<(std::ostream& out, FutureException const& ex)
// {
//     return (out << "FutureException: " << ex.Error() << " (" << ex.what() << ")");
// }

/**
 * @brief Provides ara::core specific Future operations to collect the results of an asynchronous call.
 *
 * Much of its functionality is delegated to std::future and all methods that resemble std::future are guaranteed to
 * behave the same.
 *
 * If the valid() member function of an instance returns true, all other methods are guaranteed to work on that
 * instance. Otherwise, they may fail with or without an exception. A thrown exception will be of type
 * std::future_error.
 *
 * Having an invalid future will usually happen when the future was moved from using the move constructor or move
 * assignment.
 *
 * @uptrace{SWS_CORE_00321, 302fa88ba42809fab8275eb420dc0c7e53cee288}
 */
template < typename T, typename E = ErrorCode >
class Future final
{
public:
    using R = Result< T, E >;

    using Lock = std::unique_lock< std::mutex >;
    /// @brief Data type
    using value_type = T;  // NOLINT
    /// @brief Error type
    using error_type = E;  // NOLINT

public:
    // /// Alias type for T
    // using ValueType = T;
    // /// Alias type for the Promise type collaborating with this Future type
    // using PromiseType = Promise<T, E>;

    /**
     * @brief Default constructor
     *
     * @uptrace{SWS_CORE_00322, 3d249073b46054c2846591aa23ab024ce78e9794}
     */
    Future() noexcept = default;

    /// @brief Destructor for Future objects.
    /// This will also disable any callback that has been set.
    /// @uptrace{SWS_CORE_00333, 439c3151912126d380803d6047f41ac47a55855c}
    ~Future()
    {
        Lock lock(mutex_);
        if (pState_) {
            pState_->SetCallbackClearFlag();
        }
    }
    /**
     * @brief Constructs a Future from a given std::future and a pointer to the extra state.
     *
     * @param delegateFuture std::future instance
     * @param extraState state that is shared with the Promise
     */
    Future(std::future< R >&& delegateFuture, internal::State::Ptr extraState)
        : delegateFuture_(std::move(delegateFuture)), pState_(std::move(extraState))
    {
    }

    /// @uptrace{SWS_CORE_00334, 279bb26a670f4e5d3a13647ffc4d69e445c29684}
    Future(Future const&) = delete;

    /// @uptrace{SWS_CORE_00335, bb82f085f34d8e25f30b1d581b7c7d17ecd73d3c}
    Future& operator=(Future const&) = delete;

    /**
     * @uptrace{SWS_CORE_00323, 5ac90a7364eea61ede00996c35342bb46fb985ee}
     */
    Future(Future&& other) noexcept
        : lock_(other.mutex_), delegateFuture_(std::move(other.delegateFuture_)), pState_(std::move(other.pState_))
    {
        lock_.unlock();
    }

    /**
     * @uptrace{SWS_CORE_00325, 166f3555c0b24118fac73c7c6becdd44076c39c3}
     */
    Future& operator=(Future&& other) noexcept
    {
        if (this != &other) {
            Lock lhsLock(mutex_, std::defer_lock);
            Lock rhsLock(other.mutex_, std::defer_lock);
            std::lock(lhsLock, rhsLock);

            delegateFuture_ = std::move(other.delegateFuture_);
            pState_         = std::move(other.pState_);
        }
        return *this;
    }

    // Stop defining noexcept in order to let other exception be propageted to upper levels and fix some crashes
    /// @uptrace{SWS_CORE_00336, 0000000000000000000000000000000000000000}
    ATTR_NODISCARD R GetResult() noexcept
    {
#ifndef ARA_NO_EXCEPTIONS
        try {
            return delegateFuture_.get();
        } catch (std::future_error const& ex) {
            std::error_code const& ec = ex.code();
            future_errc err;
            if (ec == std::future_errc::broken_promise) {
                err = future_errc::broken_promise;
            } else if (ec == std::future_errc::future_already_retrieved) {
                err = future_errc::future_already_retrieved;
            } else if (ec == std::future_errc::promise_already_satisfied) {
                err = future_errc::promise_already_satisfied;
            } else if (ec == std::future_errc::no_state) {
                err = future_errc::no_state;
            } else {
                // Should rather use a vendor/demonstrator-specific ErrorDomain here?
                return R::FromError(CoreErrc::kInvalidArgument);
            }
            R r = R::FromError(err);
            return r;
        }
#else
        // TODO:: Somehow query the future object whether it contains an exception,
        // and if so, translate it into a Result<...>
        // This does not seem possible with C++14's std::future, because it lacks boost::future's
        // has_exception() method. Unfortunately, boost::future cannot be compiled with
        // -fno-exceptions, so that one is out as well.
        return delegateFuture_.get();
#endif
    }

#ifndef ARA_NO_EXCEPTIONS
    /**
     * @brief Get the value.
     *
     * This call blocks until the result or an exception is available.
     *
     * @returns value of type T
     *
     * @uptrace{SWS_CORE_00326, 6a0899ec7586256ce94decda64b39a86586e4bb8}
     */
    T get()  // NOLINT
    {
        return GetResult().ValueOrThrow();
    }
#else // 2023-10-08 Ensure compilation passes
    T get() { return GetResult().Value(); }
#endif

    /**
     * @brief Checks if the future is valid, i.e. if it has a shared state.
     *
     * @returns true if the future is usable, false otherwise
     *
     * @uptrace{SWS_CORE_00327, 5f47867d1cd49c58310a4123780dbafdb7a852b8}
     */
    bool valid() const noexcept  // NOLINT
    {
        return delegateFuture_.valid();
    }

    /**
     * @brief Waits for a value or an exception to be available.
     *
     * After this method returns, get() is guaranteed to not block and is_ready() will return true.
     *
     * @uptrace{SWS_CORE_00328, 5990cffd918c5a8b2dd05825122549afb31fd413}
     */
    void wait() const  // NOLINT
    {
        delegateFuture_.wait();
    }

    /**
     * @brief Wait for the given period.
     *
     * If the Future becomes ready or the timeout is reached, the method returns.
     *
     * @param timeout_duration maximal duration to wait for
     * @returns status that indicates whether the timeout hit or if a value is available
     *
     * @uptrace{SWS_CORE_00329, a53e699d54ca435e095295e5e9ac33037888b299}
     */
    template < typename Rep, typename Period >
    future_status wait_for(std::chrono::duration< Rep, Period > const& timeout_duration) const  // NOLINT
    {
        switch (delegateFuture_.wait_for(timeout_duration)) {
            case std::future_status::ready:
                return future_status::ready;
            case std::future_status::timeout:
                return future_status::timeout;
            default:
                assert(!"this std::future_status should not occur in our setup");
        }
    }

    /**
     * @brief Wait until the given time.
     *
     * If the Future becomes ready or the time is reached, the method returns.
     *
     * @param deadline latest point in time to wait
     * @returns status that indicates whether the time was reached or if a value is available
     *
     * @uptrace{SWS_CORE_00330, 79bdbf2579feea15c752e0ba695758fc2d1f3369}
     */
    template < typename Clock, typename Duration >
    future_status wait_until(std::chrono::time_point< Clock, Duration > const& deadline) const  // NOLINT
    {
        switch (delegateFuture_.wait_until(deadline)) {
            case std::future_status::ready:
                return future_status::ready;
            case std::future_status::timeout:
                return future_status::timeout;
            default:
                assert(!"this std::future_status should not occur in our setup");
        }
    }
    /**
     * True when the future contains either a result or an exception.
     *
     * If is_ready() returns true, get() and the wait calls are guaranteed to not block.
     *
     * @returns true if the Future contains data, false otherwise
     *
     * @uptrace{SWS_CORE_00332, 51670bbcd1b591382707dcbf61f0af5a4ccc6a62}
     */
    bool is_ready() const  // NOLINT
    {
        return std::future_status::ready == delegateFuture_.wait_for(std::chrono::seconds::zero());
    }

public:
    /// @brief Register a callable to be invoked when Future is ready. When func is called, get() and GetResult() are guaranteed not to block.
    /// @code{.isoft}
    /// export_level=/Core
    /// @traceid {SWS_CORE_00331}
    /// @threadsafety {re-entrant}
    /// @endcode
    /// @brief Register a callable to be invoked when Future is ready. When func is called, get() and GetResult() are guaranteed not to block.
    /// func can be called in the context of this call, or in Promise::set_value() or Promise::SetError() or elsewhere.
    /// The return type of then() depends on the return type of func (also known as continuation).
    /// Let U be the return type of the continuation (i.e., a type equivalent to std::result_of<std::decay<F>::type(Future<T,E>)>::type).
    /// If U is Future<T2,E2> for some types T2, E2, the return type of then() is Future<T2,E2>. This is called implicit future unwrapping.
    /// If U is Result<T2,E2> for some types T2, E2, the return type of then() is Future<T2,E2>. This is called implicit result unwrapping.
    /// Otherwise it is Future<U,E>.
    /// @tparam F
    /// @param func
    /// @return
    /// @tparam F
    template < typename F >
    auto then(F&& func)  // NOLINT
    {
        Lock lock(mutex_);
        using T_Func         = F;
        using PAgentCallback = typename internal::PAgentCallback< T_Func, E, std::result_of_t< T_Func() > >;
        typename PAgentCallback::Ptr pAgentCallback{std::make_shared< PAgentCallback >(std::forward< T_Func >(func))};
        if (is_ready()) {
            pAgentCallback->FireCallback();
        }
        pState_->AttachCallback(pAgentCallback);
        return pAgentCallback->GetFuture();
    }
    /// @brief Register a callable to be invoked when Future is ready. When func is called, get() and GetResult() are guaranteed not to block.
    /// @code{.isoft}
    /// export_level=/Core
    /// @traceid {SWS_CORE_00331}
    /// @threadsafety {re-entrant}
    /// @endcode
    /// @brief Register a callable to be invoked when Future is ready. When func is called, get() and GetResult() are guaranteed not to block.
    /// func is called in the context of the provided execution context executor.
    /// The return type of then() depends on the return type of func (also known as continuation).
    /// Let U be the return type of the continuation (i.e., a type equivalent to std::result_of<std::decay<F>::type(Future<T,E>)>::type).
    /// If U is Future<T2,E2> for some types T2, E2, the return type of then() is Future<T2,E2>. This is called implicit future unwrapping.
    /// If U is Result<T2,E2> for some types T2, E2, the return type of then() is Future<T2,E2>. This is called implicit result unwrapping.
    /// Otherwise it is Future<U,E>.
    /// @tparam F
    /// @tparam ExecutorT
    /// @param func
    /// @param executor
    /// @return
    template < typename F, typename ExecutorT >
    auto then(F&& func, ExecutorT&& executor)  // NOLINT
    {
        Lock lock(mutex_);
        using T_Func = F;
        using PAgentCallback
            = internal::PAgentCallback_Arg< T_Func, ExecutorT, E, std::result_of_t< T_Func(ExecutorT) > >;
        typename PAgentCallback::Ptr pAgentCallback{
            std::make_shared< PAgentCallback >(std::forward< T_Func >(func), std::forward< ExecutorT >(executor))};
        if (is_ready()) {
            pAgentCallback->FireCallback();
        }
        pState_->AttachCallback(pAgentCallback);
        return pAgentCallback->GetFuture();
    }
    // template <typename F>
    // auto then_test1(F&& func) {
    //     using T_Func = F;
    //     Lock lock(mutex_);
    //     auto pAgentCallback = internal::PCallbackAgent::Make<T_Func, E>(std::forward<T_Func>(func));
    //     if (is_ready()) {
    //         pAgentCallback->FireCallback();
    //     }
    //     pState_->AttachCallback(pAgentCallback);
    //     return pAgentCallback->GetFuture();
    // }
    //****************/
private:
    std::mutex mutex_;
    Lock lock_;  // this Lock is only necessary for the move constructor

    std::future< R > delegateFuture_;
    internal::State::Ptr pState_{nullptr};

    template < typename, typename >
    friend class Promise;
};

/// @brief Specialization of class Future for "void" values
/// @tparam E  the type of error
///
/// @uptrace{SWS_CORE_06221, 128eede48e71c8d2ce9bb678949737414623b806}
template < typename E >
class Future< void, E > final
{
public:
    using R = Result< void, E >;

    using Lock = std::unique_lock< std::mutex >;

    /// @brief Data type
    using value_type = void;  // NOLINT
    /// @brief Error type
    using error_type = E;  // NOLINT

public:
    // /// Alias type for T
    // using ValueType = void;
    // /// Alias type for the Promise type collaborating with this Future type
    // using PromiseType = Promise<void, E>;
    // /// Alias type for the future_status type
    // using Status = future_status;

    /// @uptrace{SWS_CORE_06222, aa826ebda60d2763737938555ba6abfd210893e6}
    Future() noexcept = default;

    /// @uptrace{SWS_CORE_06233, ba964109c5eb8c0911a82bbc663c5812e24933f2}
    ~Future()
    {
        Lock lock(mutex_);

        if (pState_) {
            pState_->SetCallbackClearFlag();
        }
    }

    /// @uptrace{SWS_CORE_06234, faf349a3effd2634597af5a06cb7e0566109e82b}
    Future(Future const&) = delete;

    /// @uptrace{SWS_CORE_06235, 1e32312de6bc1318430bcfa04b6e1236d3126a80}
    Future& operator=(Future const&) = delete;

    /// @uptrace{SWS_CORE_06223, 57c6bfe2305a8d80e3a48b7c48b9fcf8de637ec4}
    Future(Future&& other) noexcept
        : lock_(other.mutex_), delegateFuture_(std::move(other.delegateFuture_)), pState_(std::move(other.pState_))
    {
        lock_.unlock();
    }

    /// @uptrace{SWS_CORE_06225, 44a6018f6b417573ea0ed0699c10a6ec009b9efc}
    Future& operator=(Future&& other) noexcept
    {
        if (this != &other) {
            Lock lhsLock(mutex_, std::defer_lock);
            Lock rhsLock(other.mutex_, std::defer_lock);
            std::lock(lhsLock, rhsLock);

            delegateFuture_ = std::move(other.delegateFuture_);
            pState_         = std::move(other.pState_);
        }
        return *this;
    }

#ifndef ARA_NO_EXCEPTIONS
    /// @uptrace{SWS_CORE_06226, d553ca888e0bba3a52db9e09955591a2cb285dc0}
    void get()  // NOLINT
    {
        return GetResult().ValueOrThrow();
    }
#else // 2023-10-08 Ensure compilation passes
    void get() { return GetResult().Value(); }
#endif

    /// @uptrace{SWS_CORE_06236, d8c662fa5811ffd375b67f7880256eb6db2b15b0}
    ATTR_NODISCARD Result< void, E > GetResult() noexcept
    {
#ifndef ARA_NO_EXCEPTIONS
        try {
            return delegateFuture_.get();
        } catch (std::future_error const& ex) {
            std::error_code const& ec = ex.code();
            future_errc err;
            if (ec == std::future_errc::broken_promise) {
                err = future_errc::broken_promise;
            } else if (ec == std::future_errc::future_already_retrieved) {
                err = future_errc::future_already_retrieved;
            } else if (ec == std::future_errc::promise_already_satisfied) {
                err = future_errc::promise_already_satisfied;
            } else if (ec == std::future_errc::no_state) {
                err = future_errc::no_state;
            } else {
                // Should rather use a vendor/demonstrator-specific ErrorDomain here?
                return R::FromError(CoreErrc::kInvalidArgument);
            }
            R r = R::FromError(err);
            return r;
        }
#else
        return delegateFuture_.get();
#endif
    }

    /// @uptrace{SWS_CORE_06227, cd20bb8f96ba8edd8cebeadbf51c70f3475e288b}
    bool valid() const noexcept  // NOLINT
    {
        return delegateFuture_.valid();
    }

    /// @uptrace{SWS_CORE_06228, 2c2d8a498cbc7081c61e8b5784b2852cf42495d3}
    void wait() const  // NOLINT
    {
        delegateFuture_.wait();
    }

    /// @uptrace{SWS_CORE_06229, 08610f7ac3b7c35264401c867e502238bf2a1002}
    template < typename Rep, typename Period >
    future_status wait_for(std::chrono::duration< Rep, Period > const& timeoutDuration) const  // NOLINT
    {
        switch (delegateFuture_.wait_for(timeoutDuration)) {
            case std::future_status::ready:
                return future_status::ready;
            case std::future_status::timeout:
                return future_status::timeout;
            default:
                assert(!"this std::future_status should not occur in our setup");
        }
    }

    /// @uptrace{SWS_CORE_06230, 5386019b35fbf221ce3c0f4557149b7221f36667}
    template < typename Clock, typename Duration >
    future_status wait_until(std::chrono::time_point< Clock, Duration > const& deadline) const  // NOLINT
    {
        switch (delegateFuture_.wait_until(deadline)) {
            case std::future_status::ready:
                return future_status::ready;
            case std::future_status::timeout:
                return future_status::timeout;
            default:
                assert(!"this std::future_status should not occur in our setup");
        }
    }

    /// @uptrace{SWS_CORE_06232, b9b25381d9b48cc35a4e694000f1d4c6ae96dd6b}
    bool is_ready() const  // NOLINT
    {
        return std::future_status::ready == delegateFuture_.wait_for(std::chrono::seconds::zero());
    }

    /// @brief Register a callable to be invoked when Future is ready. When func is called, get() and GetResult() are guaranteed not to block.
    /// @code{.isoft}
    /// export_level=/Core
    /// @traceid {SWS_CORE_06231}
    /// @threadsafety {re-entrant}
    /// @endcode
    /// @brief Register a callable to be invoked when Future is ready. When func is called, get() and GetResult() are guaranteed not to block.
    /// func can be called in the context of this call, or in Promise::set_value() or Promise::SetError() or elsewhere.
    /// The return type of then() depends on the return type of func (also known as continuation).
    /// Let U be the return type of the continuation (i.e., a type equivalent to std::result_of<std::decay<F>::type(Future<T,E>)>::type).
    /// If U is Future<T2,E2> for some types T2, E2, the return type of then() is Future<T2,E2>. This is called implicit future unwrapping.
    /// If U is Result<T2,E2> for some types T2, E2, the return type of then() is Future<T2,E2>. This is called implicit result unwrapping.
    /// Otherwise it is Future<U,E>.
    /// @tparam F
    /// @param func
    /// @return
    /// @tparam F
    template < typename F >
    auto then(F&& func)  // NOLINT
    {
        Lock lock(mutex_);
        using T_Func         = F;
        using PAgentCallback = typename internal::PAgentCallback< T_Func, E, std::result_of_t< T_Func() > >;
        typename PAgentCallback::Ptr pAgentCallback{std::make_shared< PAgentCallback >(std::forward< T_Func >(func))};
        if (is_ready()) {
            pAgentCallback->FireCallback();
        }
        pState_->AttachCallback(pAgentCallback);
        return pAgentCallback->GetFuture();
    }
    /// @brief Register a callable to be invoked when Future is ready. When func is called, get() and GetResult() are guaranteed not to block.
    /// @code{.isoft}
    /// export_level=/Core
    /// @traceid {SWS_CORE_06237}
    /// @threadsafety {re-entrant}
    /// @endcode
    /// @brief Register a callable to be invoked when Future is ready. When func is called, get() and GetResult() are guaranteed not to block.
    /// func is called in the context of the provided execution context executor.
    /// The return type of then() depends on the return type of func (also known as continuation).
    /// Let U be the return type of the continuation (i.e., a type equivalent to std::result_of<std::decay<F>::type(Future<T,E>)>::type).
    /// If U is Future<T2,E2> for some types T2, E2, the return type of then() is Future<T2,E2>. This is called implicit future unwrapping.
    /// If U is Result<T2,E2> for some types T2, E2, the return type of then() is Future<T2,E2>. This is called implicit result unwrapping.
    /// Otherwise it is Future<U,E>.
    /// @tparam F
    /// @tparam ExecutorT
    /// @param func
    /// @param executor
    /// @return
    template < typename F, typename ExecutorT >
    auto then(F&& func, ExecutorT&& executor)  // NOLINT
    {
        Lock lock(mutex_);
        using T_Func = F;
        using PAgentCallback
            = internal::PAgentCallback_Arg< T_Func, ExecutorT, E, std::result_of_t< T_Func(ExecutorT) > >;
        typename PAgentCallback::Ptr pAgentCallback{
            std::make_shared< PAgentCallback >(std::forward< T_Func >(func), std::forward< ExecutorT >(executor))};
        if (is_ready()) {
            pAgentCallback->FireCallback();
        }
        pState_->AttachCallback(pAgentCallback);
        return pAgentCallback->GetFuture();
    }

private:
    Future(std::future< R >&& delegateFuture, internal::State::Ptr extraState)
        : delegateFuture_(std::move(delegateFuture)), pState_(std::move(extraState))
    {
    }

    std::mutex mutex_;
    Lock lock_;  // this Lock is only necessary for the move constructor

    std::future< R > delegateFuture_;
    internal::State::Ptr pState_;

    template < typename, typename >
    friend class Promise;
};

}  // namespace core
}  // namespace ara

#endif  // ARA_CORE_FUTURE_H_
