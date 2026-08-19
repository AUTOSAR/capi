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
/// @file       event.h
/// @brief      Communication service proxy event header file
/// @details
/// @date       2021-12-31
/// @author     mazelin
/// @version    1.2.0
///
/// ================================================================

#pragma once

#include <deque>
#include <mutex>
#include <numeric>

#include "ara/com/types.h"

/// @brief Namespace -- Internal proxy
namespace ara {
namespace com {
namespace internal {
namespace proxy {
/// @brief Template type -- Service proxy event
/// @tparam T Event data type
template < typename T >
class Event;

/// @brief Template type -- Check if it is an event type (No)
/// @tparam T
template < typename T >
struct IsEvent : std::false_type
{
};
/// @brief Template type -- Check if it is an event type (Yes)
/// @tparam T
template < typename T >
struct IsEvent< Event< T > > : std::true_type
{
};

/// @brief Template type -- Event base type
/// @code{.isoft}
/// export_level=/COM/SoC/SOA
/// @endcode
/// @tparam T Event data type
template < typename T >
class EventBase
{
public:
    /// @brief Type alias -- Sample data type
    using SampleType = std::remove_cv_t< std::remove_reference_t< T > >;
    /// @brief Type alias -- New sample callback signature
    /// @ref [SWS_CM_00702] -- Signature of Callable f
    using NewSamplesCallback = std::function< void(SamplePtr< SampleType const >) >;
    /// @brief Constructor
    EventBase() noexcept { ComLogTrace(""); }
    /// @brief Destructor
    virtual ~EventBase() noexcept { ComLogTrace(""); }
    /// @brief Copy constructor
    /// @param other
    EventBase(EventBase const& other) noexcept = default;
    /// @brief Move constructor
    /// @param other
    EventBase(EventBase&& other) noexcept = default;
    /// @brief Copy assignment operator
    /// @param other
    /// @return EventBase
    EventBase& operator=(EventBase const& other) noexcept = default;
    /// @brief Move assignment operator
    /// @param other
    /// @return EventBase
    EventBase& operator=(EventBase&& other) noexcept = default;
    /// @brief Subscribe to event
    /// @param[in] maxSampleCount Maximum number of samples to receive
    /// @return Result object -- empty/value or error
    virtual ara::core::Result< void > Subscribe(size_t maxSampleCount) noexcept = 0;
    /// @brief Unsubscribe from event
    virtual void Unsubscribe() noexcept = 0;
    /// @brief Get event subscription state
    /// @return Event subscription state @ref SubscriptionStateChangeHandler
    virtual SubscriptionState GetSubscriptionState() const noexcept = 0;
    /// @brief Set event receive handler
    /// @param[in] handler Receive handler
    /// @return Result object -- empty/value or error
    virtual ara::core::Result< void > SetReceiveHandler(EventReceiveHandler handler) noexcept = 0;
    /// @brief Reset event receive handler
    /// @return Result object -- empty/value or error
    virtual ara::core::Result< void > UnsetReceiveHandler() noexcept = 0;
    /// @brief Set event subscription state change handler
    /// @param[in] handler Subscription state change handler
    /// @return Result object -- empty/value or error
    virtual ara::core::Result< void > SetSubscriptionStateChangeHandler(
        SubscriptionStateChangeHandler handler) noexcept = 0;
    /// @brief Reset event subscription state change handler
    virtual void UnsetSubscriptionStateChangeHandler() noexcept = 0;
#ifdef HAS_COM_EVENT_LOST_CHECKER
    /// @brief Set sample loss handler
    /// @param[in] handler Loss handler
    /// @return Result object -- empty/value or error
    virtual ara::core::Result< void > SetSampleLostHandler(SampleLostHandler handler) noexcept = 0;
    /// @brief Reset sample loss handler
    virtual void UnsetSampleLostHandler() noexcept = 0;
#endif  // HAS_COM_EVENT_LOST_CHECKER
    /// @brief Get new samples
    /// @param[in] callback Callback function to process new samples
    /// @param[in] maxNumberOfSamples Maximum number of samples to get
    /// @return Result object -- empty/value or error
    virtual ara::core::Result< size_t > GetNewSamples(NewSamplesCallback&& callback,
                                                      size_t maxNumberOfSamples) noexcept = 0;
    /// @brief Get the number of remaining samples
    /// @return Number of remaining samples
    virtual size_t GetFreeSampleCount() noexcept = 0;
    /// @brief Get the last received E2E result
    /// @return Result object -- E2E value or error
    virtual e2exf::E2EResult GetResult() const noexcept = 0;
    /// @brief Get the global E2E SMState
    /// @return Result of the last called E2E_check function
    virtual e2e::SMState GetSMState() const noexcept { return e2e::SMState{}; }

protected:  ///< @note Used for fields
    /// @brief Check if it is a field
    /// @return bool
    virtual bool IsField() const noexcept  // NOLINT -- Virtual function naming convention > protected member function naming convention
    {
        return false;
    }
};

/// @brief Template type -- Event type
/// @code{.isoft}
/// export_level=/COM/SoC/SOA
/// @endcode
/// @tparam T Event data type
template < typename T >
class Event
{
public:
    /// @brief Type alias -- Event implementation type
    using EventImpl = EventBase< T >;
    /// @brief Type alias -- Sample data type
    using SampleType = typename EventImpl::SampleType;
    /// @brief Type alias -- New sample callback signature
    /// @ref [SWS_CM_00702] -- Signature of Callable f
    using NewSamplesCallback = typename EventImpl::NewSamplesCallback;
    /// @brief Constructor
    Event() noexcept { ComLogTrace("create service proxy event"); }
    /// @brief Destructor
    ~Event() noexcept { ComLogTrace("destroy service proxy event"); }
    /// @brief Copy constructor
    /// @param[in] other
    Event(Event const& other) noexcept = delete;
    /// @brief Move constructor
    /// @param[in] other
    Event(Event&& other) noexcept = default;
    /// @brief Copy assignment operator
    /// @param[in] other
    /// @return Event&
    Event& operator=(Event const& other) noexcept = delete;
    /// @brief Move assignment operator
    /// @param[in] other
    /// @return Event&
    Event& operator=(Event&& other) noexcept = default;
    /// @brief Overloaded assignment operator
    /// @param[in] impl
    /// @return Event&
    Event& operator=(std::shared_ptr< EventImpl >&& impl) noexcept
    {
        ComLogTrace("assign bind proxy event");
        impl_ = std::move(impl);
        return *this;
    }
    /// @brief Subscribe to event
    /// @code{.isoft}
    /// export_level=/COM/SoC/SOA
    /// @endcode
    /// @param[in] maxSampleCount Maximum number of samples to receive
    /// @return Result object -- empty/value or error
    /// @retval ComErrc::kMaxSampleCountNotRealizable Already subscribed, and the maximum sample count differs
    /// @ref [SWS_CM_00141] -- Method to subscribe to a service event
    /// @ref [SWS_CM_00700] -- Ensure memory allocation of maxSampleCount samples
    /// @ref [SWS_CM_00022] -- Re-entrancy – Subscribe
    ara::core::Result< void > Subscribe(size_t maxSampleCount) noexcept
    {
        ComLogTrace("subscribe service event", GenArg(maxSampleCount));
        assert(impl_);
        return impl_->Subscribe(maxSampleCount);
    }
    /// @brief Unsubscribe from event
    /// @code{.isoft}
    /// export_level=/COM/SoC/SOA
    /// @endcode
    /// @ref [SWS_CM_00151] -- Method to unsubscribe from a service event
    /// @ref [SWS_CM_00023] -- Re-entrancy – Unsubscribe
    void Unsubscribe() noexcept
    {
        ComLogTrace("unsubscribe service event");
        assert(impl_);
        impl_->Unsubscribe();
    }
    /// @brief Get event subscription state
    /// @code{.isoft}
    /// export_level=/COM/SoC/SOA
    /// @endcode
    /// @return Event subscription state @ref SubscriptionStateChangeHandler
    /// @ref [SWS_CM_00316] -- Query Subscription State
    /// @ref [SWS_CM_00024] -- Re-entrancy – GetSubscriptionState
    SubscriptionState GetSubscriptionState() const noexcept
    {
        assert(impl_);
        return impl_->GetSubscriptionState();
    }
    /// @brief Check if subscribed
    /// @code{.isoft}
    /// export_level=/COM/SoC/SOA
    /// @endcode
    /// @return bool Is subscribed
    bool IsSubscribed() const noexcept { return GetSubscriptionState() == SubscriptionState::kSubscribed; }
    /// @brief Set event receive handler
    /// @code{.isoft}
    /// export_level=/COM/SoC/SOA
    /// @endcode
    /// @param[in] handler Receive handler
    /// @return Result object -- empty/value or error
    /// @retval ComErrc::kSetHandlerNotSet Field set handler not set
    /// @ref [SWS_CM_00181] -- Enable service event trigger
    /// @ref [SWS_CM_00028] -- Re-entrancy – SetReceiveHandler
    ara::core::Result< void > SetReceiveHandler(EventReceiveHandler handler) noexcept
    {
        return SetReceiveHandler(handler, [](std::function< void() > const& f) { f(); });
    }
    /// @brief Set event receive handler
    /// @code{.isoft}
    /// export_level=/COM/SoC/SOA
    /// @endcode
    /// @param[in] handler Receive handler
    /// @param[in] executor Executor -- the processing will take place in its context
    /// @return Result object -- empty/value or error
    /// @retval ComErrc::kSetHandlerNotSet Field set handler not set
    /// @retval ComErrc::kCouldNotExecute Cannot execute (e.g., due to resource issues)
    /// @ref [SWS_CM_11357] -- Error behaviour of provided Execution Context for enabling service event trigger
    /// @ref [SWS_CM_11356] -- Execution Context for enabling service event trigger
    /// @ref [SWS_CM_00028] -- Re-entrancy – SetReceiveHandler
    template < typename Executor >
    ara::core::Result< void > SetReceiveHandler(EventReceiveHandler handler, Executor&& executor) noexcept
    {
        using Result = ara::core::Result< void >;
        ComLogTrace("set service event receive handler");
        if (!handler) {
            ComLogError("set service event receive handler rejected: handler invalid");
            return Result{MakeErrorCode(ComErrc::kSetHandlerNotSet, -__LINE__)};
        }
        if (!executor) {
            ComLogError("set service event receive handler rejected: executor invalid");
            return Result{MakeErrorCode(ComErrc::kCouldNotExecute, -__LINE__)};
        }
        assert(impl_);
        return impl_->SetReceiveHandler([handler{std::move(handler)}, executor{std::move(executor)}]() {
            executor([handler{std::move(handler)}]() { handler(); });
        });
    }
    /// @brief Reset event receive handler
    /// @code{.isoft}
    /// export_level=/COM/SoC/SOA
    /// @endcode
    /// @return Result object -- empty/value or error
    /// @retval ComErrc::kUnsetFailure Resetting the receive handler failed
    /// @ref [SWS_CM_00183] -- Disable service event trigger
    /// @ref [SWS_CM_00029] -- Re-entrancy – UnsetReceiveHandler
    ara::core::Result< void > UnsetReceiveHandler() noexcept
    {
        ComLogTrace("unset service event receive handler");
        assert(impl_);
        return impl_->UnsetReceiveHandler();
    }
    /// @brief Set event subscription state change handler
    /// @code{.isoft}
    /// export_level=/COM/SoC/SOA
    /// @endcode
    /// @param[in] handler Subscription state change handler
    /// @return Result object -- empty/value or error
    /// @ref [SWS_CM_00333] -- Set Subscription State change handler
    /// @ref [SWS_CM_00025] -- Re-entrancy – SetSubscriptionStateChangeHandler
    ara::core::Result< void > SetSubscriptionStateChangeHandler(SubscriptionStateChangeHandler handler) noexcept
    {
        return SetSubscriptionStateChangeHandler(handler, [](std::function< void() > const& f) { f(); });
    }
    /// @brief Set event subscription state change handler
    /// @code{.isoft}
    /// export_level=/COM/SoC/SOA
    /// @endcode
    /// @param[in] handler Subscription state change handler
    /// @param[in] executor Executor -- the processing will take place in its context
    /// @return Result object -- empty/value or error
    /// @retval ComErrc::kCouldNotExecute Cannot execute (e.g., due to resource issues)
    /// @ref [SWS_CM_11355] -- Error behaviour of provided Execution Context for setting Subscription State change handler
    /// @ref [SWS_CM_11354] -- Execution Context for setting Subscription State change handler
    /// @ref [SWS_CM_00025] -- Re-entrancy – SetSubscriptionStateChangeHandler
    template < typename Executor >
    ara::core::Result< void > SetSubscriptionStateChangeHandler(SubscriptionStateChangeHandler handler,
                                                                Executor&& executor) noexcept
    {
        using Result = ara::core::Result< void >;
        ComLogTrace("set service event subscription state change handler");
        if (!handler) {
            ComLogError("set service event subscription state change handler rejected: handler invalid");
            return Result{MakeErrorCode(ComErrc::kSetHandlerNotSet, -__LINE__)};
        }
        if (!executor) {
            ComLogError("set service event subscription state change handler rejected: executor invalid");
            return Result{MakeErrorCode(ComErrc::kCouldNotExecute, -__LINE__)};
        }
        assert(impl_);
        return impl_->SetSubscriptionStateChangeHandler(
            [handler{std::move(handler)}, executor{std::move(executor)}](SubscriptionState state) {
                executor([handler{std::move(handler)}, state{std::move(state)}]() { handler(state); });
            });
    }
    /// @brief Reset event subscription state change handler
    /// @code{.isoft}
    /// export_level=/COM/SoC/SOA
    /// @endcode
    /// @ref [SWS_CM_00334] -- Unset Subscription State change handler
    /// @ref [SWS_CM_00026] -- Re-entrancy – UnsetSubscriptionStateChangeHandler
    void UnsetSubscriptionStateChangeHandler() noexcept
    {
        ComLogTrace("unset service event subscription state change handler");
        assert(impl_);
        impl_->UnsetSubscriptionStateChangeHandler();
    }
#ifdef HAS_COM_EVENT_LOST_CHECKER
    /// @brief Set event sample loss handler
    /// @code{.isoft}
    /// export_level=/COM/SoC/SOA
    /// @endcode
    /// @param[in] handler Sample loss handler
    /// @return Result object -- empty/value or error
    ara::core::Result< void > SetSampleLostHandler(SampleLostHandler handler) noexcept
    {
        return SetSampleLostHandler(handler, [](std::function< void() > const& f) { f(); });
    }
    /// @brief Set event sample loss handler
    /// @code{.isoft}
    /// export_level=/COM/SoC/SOA
    /// @endcode
    /// @param[in] handler Sample loss handler
    /// @param[in] executor Executor -- the processing will take place in its context
    /// @return Result object -- empty/value or error
    /// @retval ComErrc::kCouldNotExecute Cannot execute (e.g., due to resource issues)
    template < typename Executor >
    ara::core::Result< void > SetSampleLostHandler(SampleLostHandler handler, Executor&& executor) noexcept
    {
        using Result = ara::core::Result< void >;
        ComLogTrace("set service event sample lost handler");
        if (!handler) {
            ComLogError("set service event sample lost handler rejected: handler invalid");
            return Result{MakeErrorCode(ComErrc::kCouldNotExecute, -__LINE__)};
        }
        if (!executor) {
            ComLogError("set service event sample lost handler rejected: executor invalid");
            return Result{MakeErrorCode(ComErrc::kCouldNotExecute, -__LINE__)};
        }
        assert(impl_);
        return impl_->SetSampleLostHandler(
            [handler{std::move(handler)}, executor{std::move(executor)}](SampleLostStatus const& state) {
                executor([handler{std::move(handler)}, state]() { handler(state); });
            });
    }
    /// @brief Reset event sample loss handler
    /// @code{.isoft}
    /// export_level=/COM/SoC/SOA
    /// @endcode
    void UnsetSampleLostHandler() noexcept
    {
        ComLogTrace("unset service event sample lost handler");
        assert(impl_);
        impl_->UnsetSampleLostHandler();
    }
#endif  // HAS_COM_EVENT_LOST_CHECKER
    /// @brief Get new samples
    /// @code{.isoft}
    /// export_level=/COM/SoC/SOA
    /// @endcode
    /// @param[in] callback Callback function to process new samples
    /// @param[in] maxNumberOfSamples Maximum number of samples to get
    /// @return Result object -- empty/value or error
    /// @retval ComErrc::kMaxSamplesReached Event cache has reached the maximum number of samples
    /// @ref [SWS_CM_00701] -- Method to update the event cache
    /// @ref [SWS_CM_00703] -- Sequence of actions in GetNewSamples
    /// @ref [SWS_CM_00704] -- Return Value
    /// @ref [SWS_CM_00714] -- Re-entrancy – GetNewSamples
    ara::core::Result< size_t > GetNewSamples(NewSamplesCallback&& callback,
                                              size_t maxNumberOfSamples = std::numeric_limits< size_t >::max()) noexcept
    {
        return GetNewSamples(std::move(callback), maxNumberOfSamples, [](std::function< void() > const& f) { f(); });
    }
    /// @brief Get new samples
    /// @code{.isoft}
    /// export_level=/COM/SoC/SOA
    /// @endcode
    /// @param[in] callback Callback function to process new samples
    /// @param[in] maxNumberOfSamples Maximum number of samples to get
    /// @param[in] executor Executor -- the processing will take place in its context
    /// @return Result object -- empty/value or error
    /// @retval ComErrc::kMaxSamplesReached Event cache has reached the maximum number of samples
    /// @retval ComErrc::kCouldNotExecute Cannot execute (e.g., due to resource issues)
    /// @ref [SWS_CM_11359] -- Error behaviour of provided Execution Context to update the event cache
    /// @ref [SWS_CM_11358] -- Execution Context to update the event cache
    /// @ref [SWS_CM_00703] -- Sequence of actions in GetNewSamples
    /// @ref [SWS_CM_00704] -- Return Value
    /// @ref [SWS_CM_00714] -- Re-entrancy – GetNewSamples
    template < typename Executor >
    ara::core::Result< size_t > GetNewSamples(NewSamplesCallback&& callback,
                                              size_t maxNumberOfSamples,
                                              Executor&& executor) noexcept
    {
        TimeRecorder(TimeNM::Timer, "APINotify::GetNewSamples");
        using Result = ara::core::Result< size_t >;
        ComLogTrace("get service event new samples", GenArg(maxNumberOfSamples));
        if (!executor) {
            ComLogError("get service event new samples rejected: executor invalid");
            return Result{MakeErrorCode(ComErrc::kCouldNotExecute, -__LINE__)};
        }
        assert(impl_);
        return impl_->GetNewSamples(
            [callback{std::move(callback)}, executor{std::move(executor)}](SamplePtr< SampleType const > sample) {
                executor([callback{std::move(callback)}, sample{std::move(sample)}]() { callback(std::move(sample)); });
            },
            maxNumberOfSamples);
    }
    /// @brief Get the number of remaining samples
    /// @code{.isoft}
    /// export_level=/COM/SoC/SOA
    /// @endcode
    /// @return Number of remaining samples
    /// @ref [SWS_CM_00705] -- Query Free Sample Slots
    /// @ref [SWS_CM_00706] -- Return Value of GetFreeSampleCount
    /// @ref [SWS_CM_11024] -- Mapping of GetFreeSampleCount method
    /// @ref [SWS_CM_00027] -- Re-entrancy – GetFreeSampleCount
    size_t GetFreeSampleCount() noexcept
    {
        ComLogTrace("");
        assert(impl_);
        return impl_->GetFreeSampleCount();
    }
    /// @brief Get the last received E2E result
    /// @code{.isoft}
    /// export_level=/COM/SoC/SOA
    /// @endcode
    /// @return Result object -- E2E value or error
    /// @ref [SWS_CM_90424] -- Provide E2E Result
    e2exf::E2EResult GetResult() const noexcept
    {
        ComLogTrace("");
        assert(impl_);
        return impl_->GetResult();
    }
    /// @brief Get the global E2E SMState
    /// @code{.isoft}
    /// export_level=/COM/SoC/SOA
    /// @endcode
    /// @return Result of the last called E2E_check function
    e2e::SMState GetSMState() const noexcept
    {
        ComLogTrace("");
        assert(impl_);
        return impl_->GetSMState();
    }

private:
    std::shared_ptr< EventImpl > impl_;
};
}  // namespace proxy
}  // namespace internal
}  // namespace com
}  // namespace ara