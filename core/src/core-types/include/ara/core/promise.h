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
/// @file       promise.h
/// @brief      The definition and implementation of ara::core::Promise that is the corresponding promise that returns an ara::core::Future.
/// @details
/// @date       2022-06-22
/// @author     hanzhibo
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_CORE_PROMISE_H_
#define ARA_CORE_PROMISE_H_

#include <exception>
#include <mutex>
#include <system_error>

#include "ara/core/error_code.h"
#include "ara/core/future.h"
#include "ara/core/internal/state.h"
#include "ara/core/result.h"

namespace ara {
namespace core {
/* Forward declaration */
template < typename, typename >
class Future;

/// @brief ara::core specific variant of std::promise class
///
/// @tparam T the type of value.
/// @tparam T the type of error.
///
/// @uptrace{SWS_CORE_00340, 94daa4ecb246e411577093039dcba25fcf8d60b9}
template < typename T, typename E = ErrorCode >
class Promise  // final
{
    using R = Result< T, E >;

    using Lock = std::unique_lock< std::mutex >;

public:
    /// @uptrace{SWS_CORE_00341, d5beba4370ae18617184f13c73e6c62e7d21e9c9}
    Promise() : extraState_(std::make_shared< internal::State >()) {}

    /// @uptrace{SWS_CORE_00349, 896ac39076b92b19edabd10aff5cf5f64623285c}
    ~Promise() = default;

    /// @uptrace{SWS_CORE_00350, 13db6d0b4ac477e520457694c813e694232953c1}
    Promise(Promise const&) = delete;

    /// @uptrace{SWS_CORE_00351, 0eb964da88aff0363839816388e253ae6da6fe40}
    Promise& operator=(Promise const&) = delete;

    /// @uptrace{SWS_CORE_00342, c89a640e17853814fd8850c6d60bb6041febddb5}
    Promise(Promise&& other) noexcept
        : lock_(other.mutex_)
        , delegatePromise_(std::move(other.delegatePromise_))
        , extraState_(std::move(other.extraState_))
    {
        lock_.unlock();
    }

    /// @uptrace{SWS_CORE_00343, 2bde0c1ec87c49bdd29049b46ba16f09ca84c205}
    Promise& operator=(Promise&& other) noexcept
    {
        if (this != &other) {
            Lock lhsLock(mutex_, std::defer_lock);
            Lock rhsLock(other.mutex_, std::defer_lock);
            std::lock(lhsLock, rhsLock);

            delegatePromise_ = std::move(other.delegatePromise_);
            extraState_      = std::move(other.extraState_);
        }
        return *this;
    }

    /// @uptrace{SWS_CORE_00352, 016b11d63f9d854a1ebf26ed2ef0bfa996f851ba}
    void swap(Promise& other) noexcept  // NOLINT
    {
        Lock lhsLock(mutex_, std::defer_lock);
        Lock rhsLock(other.mutex_, std::defer_lock);
        std::lock(lhsLock, rhsLock);

        using std::swap;
        swap(delegatePromise_, other.delegatePromise_);
        swap(extraState_, other.extraState_);
    }

    /// @brief Returns an associated Future for type T.
    ///
    /// The returned Future is set as soon as this Promise receives the result or an exception. This method must only be
    /// called once as it is not allowed to have multiple Futures per Promise.
    ///
    /// @returns a Future for type T
    ///
    /// @uptrace{SWS_CORE_00344, fc70051d35d710f795441926d40e7d9b56d4d830}
    Future< T, E > get_future()  // NOLINT
    {
        return Future< T, E >(delegatePromise_.get_future(), extraState_);
    }

    /// @uptrace{SWS_CORE_00353, b99573bc7767552116a1b6642d30c4ceeac94e52}
    void SetError(E&& err)
    {
        R r = R::FromError(std::move(err));
        SetResult(std::move(r));
    }

    /// @uptrace{SWS_CORE_00354, 2b9ae910d67681bc297837fa8948a1e94f293fbc}
    void SetError(E const& err)
    {
        R r = R::FromError(err);
        SetResult(r);
    }

    /// @uptrace{SWS_CORE_00355, 3ad07a576e0a8d4c8547b0b04f6f49f4b1fe64ed}
    void SetResult(const Result< T, E >& result)
    {
        Lock lock(mutex_);

        delegatePromise_.set_value(result);
        extraState_->FireCallback_NoLock();
    }

    /// @uptrace{SWS_CORE_00356, efff3b0ff3ed803f826d0e178d1a4e7c38776691}
    void SetResult(Result< T, E >&& result)
    {
        Lock lock(mutex_);

        delegatePromise_.set_value(std::move(result));
        extraState_->FireCallback_NoLock();
    }

    /// @brief Sets an exception.
    ///
    /// Calling Get() on the associated Future will rethrow the exception in the context the Future's method was
    /// called
    /// in.
    ///
    /// @param p exception_ptr to set
    ///
    /// @note This method is DEPRECATED. The exception is defined by the error code.
    void set_exception(std::exception_ptr p)  // NOLINT
    {
        Lock lock(mutex_);

        delegatePromise_.set_exception(p);
        extraState_->FireCallback_NoLock();
    }

    /// @brief Move the result into the future.
    ///
    /// @param value the value to store
    ///
    /// @uptrace{SWS_CORE_00346, d940a1efcef64365dd51a785f98d097add5ca9a6}
    void set_value(T&& value)  // NOLINT
    {
        R r{std::move(value)};
        SetResult(std::move(r));
    }

    /// @brief Copy result into the future.
    ///
    /// @param value the value to store
    ///
    /// @uptrace{SWS_CORE_00345, c3e193cad95341602494b3d70d351473d6bb6f94}
    void set_value(T const& value)  // NOLINT
    {
        R r{value};
        SetResult(r);
    }

public:
    /// @brief
    /// @return
    ara::core::internal::State::Ptr GetStatePtr() { return extraState_; }

private:
    std::mutex mutex_;
    Lock lock_;  // this Lock is only necessary for the move constructor

    std::promise< R > delegatePromise_;
    typename ara::core::internal::State::Ptr extraState_;
};
/// @brief Specialization of class Promise for "void" values
/// @tparam E  the type of error
///
/// @uptrace{SWS_CORE_06340, 5b731718c19658a04fb1a1c4922ebf4a4f2fdf79}
template < typename E >
class Promise< void, E > final
{
    using R = Result< void, E >;

    using Lock = std::unique_lock< std::mutex >;

public:
    /// @uptrace{SWS_CORE_06341, 0200c9cd275a0cdf026783c47f66eba2ed19952e}
    Promise() : extraState_(std::make_shared< internal::State >()) {}

    /// @uptrace{SWS_CORE_06350, b96bf9013e86d94762a816688907ea0ef05914ee}
    Promise(Promise const&) = delete;

    /// @uptrace{SWS_CORE_06342, a6b6c24206a4d16979b0510a8028d34be246e8ff}
    Promise(Promise&& other) noexcept
        : lock_(other.mutex_)
        , delegatePromise_(std::move(other.delegatePromise_))
        , extraState_(std::move(other.extraState_))
    {
        lock_.unlock();
    }

    /// @uptrace{SWS_CORE_06349, 8a1ae18c1dcdfd781c95339e57af8f5dacf318d0}
    ~Promise() = default;

    /// @uptrace{SWS_CORE_06351, 774a284519019cdb38017a2863b8da1a425104f2}
    Promise& operator=(Promise const&) = delete;

    /// @uptrace{SWS_CORE_06343, 45fbf22056ffa37e95c8b86dbe30f29c8f6f5f7f}
    Promise& operator=(Promise&& other) noexcept
    {
        if (this != &other) {
            Lock lhsLock(mutex_, std::defer_lock);
            Lock rhsLock(other.mutex_, std::defer_lock);
            std::lock(lhsLock, rhsLock);

            delegatePromise_ = std::move(other.delegatePromise_);
            extraState_      = std::move(other.extraState_);
        }
        return *this;
    }

    /// @uptrace{SWS_CORE_06352, 685eb74736a675991231ed17fdd02eafe1ba48b9}
    void swap(Promise& other) noexcept  // NOLINT
    {
        Lock lhsLock(mutex_, std::defer_lock);
        Lock rhsLock(other.mutex_, std::defer_lock);
        std::lock(lhsLock, rhsLock);

        using std::swap;
        swap(delegatePromise_, other.delegatePromise_);
        swap(extraState_, other.extraState_);
    }

    /// @uptrace{SWS_CORE_06344, 94831d6e068edfd1c566856591897d2e842e4f87}
    Future< void, E > get_future()  // NOLINT
    {
        return Future< void, E >(delegatePromise_.get_future(), extraState_);
    }

    /// @uptrace{SWS_CORE_06345, 10ae9a70011bfdcf44f9ce05c08eaa5ac49d2802}
    void set_value()  // NOLINT
    {
        R r = R::FromValue();
        SetResult(std::move(r));
    }

    /// @uptrace{SWS_CORE_06353, 7f5eca52bf5e1793275d7d55b6f1203e12a763af}
    void SetError(E&& err)
    {
        R r = R::FromError(std::move(err));
        SetResult(std::move(r));
    }

    /// @uptrace{SWS_CORE_06354, 2771afc0e275d5229643d5126098db4a320339f2}
    void SetError(E const& err)
    {
        R r = R::FromError(err);
        SetResult(r);
    }

    /// @uptrace{SWS_CORE_06355, d4051dd480d2dc5f01af97c28f8127399d892096}
    void SetResult(const Result< void, E >& result)
    {
        Lock lock(mutex_);

        delegatePromise_.set_value(result);
        extraState_->FireCallback_NoLock();
    }

    /// @uptrace{SWS_CORE_06356, ddd1d2888913bf01f1c28c376d355d8206620a54}
    void SetResult(Result< void, E >&& result)
    {
        Lock lock(mutex_);

        delegatePromise_.set_value(std::move(result));
        extraState_->FireCallback_NoLock();
    }

    // unspecified, remove this!
    void set_exception(std::exception_ptr p)  // NOLINT
    {
        Lock lock(mutex_);

        delegatePromise_.set_exception(p);
        extraState_->FireCallback_NoLock();
    }

public:
    /// @brief
    /// @return
    ara::core::internal::State::Ptr GetStatePtr() { return extraState_; }

private:
    std::mutex mutex_;
    Lock lock_;  // this Lock is only necessary for the move constructor

    std::promise< R > delegatePromise_;
    typename ara::core::internal::State::Ptr extraState_;
    template < typename, typename, typename >
    friend class PAgentCallback;
    template < typename, typename, typename >
    friend class PAgentCallback_Arg;
};

}  // namespace core
}  // namespace ara

#endif  // ARA_CORE_PROMISE_H_
