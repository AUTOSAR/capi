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
/// @file       field.h
/// @brief      Communication service proxy field header file
/// @details
/// @date       2021-12-31
/// @author     mazelin
/// @version    1.2.0
///
/// ================================================================

#pragma once

#include "event.h"
#include "method.h"

/// @brief Namespace -- Internal proxy
namespace ara {
namespace com {
namespace internal {
namespace proxy {
/// @brief Field mode enumeration
/// @code{.isoft}
/// export_level=/COM/SoC/SOA
/// @endcode
enum FieldMode
{
    kN   = 1 << 0,       ///< Notifiable
    kS   = 1 << 1,       ///< Settable
    kG   = 1 << 2,       ///< Gettable
    kNS  = kN | kS,      ///< Notifiable/Settable
    kNG  = kN | kG,      ///< Notifiable/Gettable
    kSG  = kS | kG,      ///< Settable/Gettable
    kNSG = kN | kS | kG  ///< Notifiable/Settable/Gettable
};

/// @brief Template type -- Service proxy field -- Declaration
/// @tparam T Field data type
/// @tparam mode Field mode -- Default Notifiable/Settable/Gettable
template < typename T, FieldMode mode = FieldMode::kNSG >
class Field;

/// @brief Type alias -- Notifiable field
template < typename T >
using NField = Field< T, FieldMode::kN >;
/// @brief Type alias -- Settable field
template < typename T >
using SField = Field< T, FieldMode::kS >;
/// @brief Type alias -- Gettable field
template < typename T >
using GField = Field< T, FieldMode::kG >;
/// @brief Type alias -- Notifiable/Settable field
template < typename T >
using NSField = Field< T, FieldMode::kNS >;
/// @brief Type alias -- Notifiable/Gettable field
template < typename T >
using NGField = Field< T, FieldMode::kNG >;
/// @brief Type alias -- Settable/Gettable field
template < typename T >
using SGField = Field< T, FieldMode::kSG >;
/// @brief Type alias -- Notifiable/Settable/Gettable field
template < typename T >
using NSGField = Field< T, FieldMode::kNSG >;

/// @brief Template type -- Check if it is a field type (No)
/// @tparam T
template < typename T >
struct IsField : std::false_type
{
};
/// @brief Template type -- Check if it is a field type (Yes)
/// @tparam T
template < typename T, FieldMode mode >
struct IsField< Field< T, mode > > : std::true_type
{
};
/// @brief Template type -- Check if it is a notifiable field type (No)
/// @tparam T
template < typename T >
struct IsNField : std::false_type
{
};
/// @brief Template type -- Check if it is a notifiable field type (Yes)
/// @tparam T
template < typename T >
struct IsNField< NField< T > > : std::true_type
{
};
/// @brief Template type -- Check if it is a settable field type (No)
/// @tparam T
template < typename T >
struct IsSField : std::false_type
{
};
/// @brief Template type -- Check if it is a settable field type (Yes)
/// @tparam T
template < typename T >
struct IsSField< SField< T > > : std::true_type
{
};
/// @brief Template type -- Check if it is a gettable field type (No)
/// @tparam T
template < typename T >
struct IsGField : std::false_type
{
};
/// @brief Template type -- Check if it is a gettable field type (Yes)
/// @tparam T
template < typename T >
struct IsGField< GField< T > > : std::true_type
{
};
/// @brief Template type -- Check if it is a notifiable/settable field type (No)
/// @tparam T
template < typename T >
struct IsNSField : std::false_type
{
};
/// @brief Template type -- Check if it is a notifiable/settable field type (Yes)
/// @tparam T
template < typename T >
struct IsNSField< NSField< T > > : std::true_type
{
};
/// @brief Template type -- Check if it is a notifiable/gettable field type (No)
/// @tparam T
template < typename T >
struct IsNGField : std::false_type
{
};
/// @brief Template type -- Check if it is a notifiable/gettable field type (Yes)
/// @tparam T
template < typename T >
struct IsNGField< NGField< T > > : std::true_type
{
};
/// @brief Template type -- Check if it is a settable/gettable field type (No)
/// @tparam T
template < typename T >
struct IsSGField : std::false_type
{
};
/// @brief Template type -- Check if it is a settable/gettable field type (Yes)
/// @tparam T
template < typename T >
struct IsSGField< SGField< T > > : std::true_type
{
};
/// @brief Template type -- Check if it is a notifiable/settable/gettable field type (No)
/// @tparam T
template < typename T >
struct IsNSGField : std::false_type
{
};
/// @brief Template type -- Check if it is a notifiable/settable/gettable field type (Yes)
/// @tparam T
template < typename T >
struct IsNSGField< NSGField< T > > : std::true_type
{
};

/// @brief Check if notifiable
/// @tparam T Field data type
/// @return bool
template < typename T >
constexpr bool EnableNField() noexcept
{
    return IsNField< T >::value || IsNSField< T >::value || IsNGField< T >::value || IsNSGField< T >::value;
}
/// @brief Check if settable
/// @tparam T Field data type
/// @return bool
template < typename T >
constexpr bool EnableSField() noexcept
{
    return IsSField< T >::value || IsNSField< T >::value || IsSGField< T >::value || IsNSGField< T >::value;
}
/// @brief Check if gettable
/// @tparam T Field data type
/// @return bool
template < typename T >
constexpr bool EnableGField() noexcept
{
    return IsGField< T >::value || IsNGField< T >::value || IsSGField< T >::value || IsNSGField< T >::value;
}

/// @brief Check if has a notifier
/// @param[in] mode Field mode
/// @return bool
constexpr bool HasNotifier(FieldMode mode) noexcept { return (mode & FieldMode::kN) != 0; }
/// @brief Check if has a setter
/// @param[in] mode Field mode
/// @return bool
constexpr bool HasSetter(FieldMode mode) noexcept { return (mode & FieldMode::kS) != 0; }
/// @brief Check if has a getter
/// @param[in] mode Field mode
/// @return bool
constexpr bool HasGetter(FieldMode mode) noexcept { return (mode & FieldMode::kG) != 0; }

/// @brief Template type -- Notifiable field base type
/// @code{.isoft}
/// export_level=/COM/SoC/SOA
/// @endcode
/// @tparam T Field data type
/// @tparam bool Notifier exists by default
template < typename T, bool = true >
class NFieldBase : public virtual EventBase< T >
{
public:
    /// @brief Type alias -- Field data type
    using FieldType = std::remove_cv_t< std::remove_reference_t< T > >;
    /// @brief Constructor
    NFieldBase() noexcept { ComLogTrace(""); }
    /// @brief Destructor
    ~NFieldBase() noexcept override { ComLogTrace(""); }
    /// @brief Copy constructor
    /// @param other
    NFieldBase(NFieldBase const& other) noexcept = default;
    /// @brief Move constructor
    /// @param other
    NFieldBase(NFieldBase&& other) noexcept = default;
    /// @brief Copy assignment operator
    /// @param other
    /// @return NFieldBase
    NFieldBase& operator=(NFieldBase const& other) noexcept = default;
    /// @brief Move assignment operator
    /// @param other
    /// @return NFieldBase
    NFieldBase& operator=(NFieldBase&& other) noexcept = default;

protected:  ///< @note Used for fields
    /// @brief Check if it is a field
    /// @return bool
    bool IsField() const noexcept override  // NOLINT -- Virtual function naming convention > protected member function naming convention
    {
        return true;
    }
};
/// @brief Template type -- Non-notifiable field base type
/// @tparam T Field data type
template < typename T >
class NFieldBase< T, false >
{
};

/// @brief Template type -- Settable field base type
/// @code{.isoft}
/// export_level=/COM/SoC/SOA
/// @endcode
/// @tparam T Field data type
/// @tparam bool Setter exists by default
template < typename T, bool = true >
class SFieldBase
{
public:
    /// @brief Type alias -- Field data type
    using FieldType = std::remove_cv_t< std::remove_reference_t< T > >;
    /// @brief Type alias -- Set method signature
    using SetMethod = RMethod< FieldType(FieldType const&) >;
    /// @brief Constructor
    SFieldBase() noexcept { ComLogTrace(""); }
    /// @brief Destructor
    virtual ~SFieldBase() noexcept { ComLogTrace(""); }
    /// @brief Copy constructor
    /// @param other
    SFieldBase(SFieldBase const& other) noexcept = default;
    /// @brief Move constructor
    /// @param other
    SFieldBase(SFieldBase&& other) noexcept = delete;
    /// @brief Copy assignment operator
    /// @param other
    /// @return SFieldBase
    SFieldBase& operator=(SFieldBase const& other) noexcept = delete;
    /// @brief Move assignment operator
    /// @param other
    /// @return SFieldBase
    SFieldBase& operator=(SFieldBase&& other) noexcept = default;
    /// @brief Set field
    /// @param[in] value Field value
    /// @return Future object -- asynchronous/synchronous wait for result object
    virtual ara::core::Future< FieldType > Set(FieldType const& value) noexcept = 0;
};
/// @brief Template type -- Non-settable field base type
/// @tparam T Field data type
template < typename T >
class SFieldBase< T, false >
{
};

/// @brief Template type -- Gettable field base type
/// @code{.isoft}
/// export_level=/COM/SoC/SOA
/// @endcode
/// @tparam T Field data type
/// @tparam bool Getter exists by default
template < typename T, bool = true >
class GFieldBase
{
public:
    /// @brief Type alias -- Field data type
    using FieldType = std::remove_cv_t< std::remove_reference_t< T > >;
    /// @brief Type alias -- Get method signature
    using GetMethod = RMethod< FieldType() >;
    /// @brief Constructor
    GFieldBase() noexcept { ComLogTrace(""); }
    /// @brief Destructor
    virtual ~GFieldBase() noexcept { ComLogTrace(""); }
    /// @brief Copy constructor
    /// @param other
    GFieldBase(GFieldBase const& other) noexcept = default;
    /// @brief Move constructor
    /// @param other
    GFieldBase(GFieldBase&& other) noexcept = default;
    /// @brief Copy assignment operator
    /// @param other
    /// @return GFieldBase
    GFieldBase& operator=(GFieldBase const& other) noexcept = default;
    /// @brief Move assignment operator
    /// @param other
    /// @return GFieldBase
    GFieldBase& operator=(GFieldBase&& other) noexcept = default;
    /// @brief Get field
    /// @return Future object -- asynchronous/synchronous wait for result object
    virtual ara::core::Future< FieldType > Get() noexcept = 0;
};
/// @brief Template type -- Non-gettable field base type
/// @tparam T Field data type
template < typename T >
class GFieldBase< T, false >
{
};

/// @brief Template type -- Field base type
/// @code{.isoft}
/// export_level=/COM/SoC/SOA
/// @endcode
/// @tparam T Field data type
/// @tparam mode Field mode
template < typename T, FieldMode mode >
class FieldBase
    : public virtual NFieldBase< T, HasNotifier(mode) >
    , public virtual SFieldBase< T, HasSetter(mode) >
    , public virtual GFieldBase< T, HasGetter(mode) >
{
public:
    /// @brief Type alias -- Field data type
    using FieldType = std::remove_cv_t< std::remove_reference_t< T > >;
};

/// @brief Template type -- Field type
/// @code{.isoft}
/// export_level=/COM/SoC/SOA
/// @endcode
/// @tparam T Field data type
/// @tparam mode Field mode
template < typename T, FieldMode mode >
class Field
{
public:
    /// @brief Type alias -- Field implementation type
    using FieldImpl = FieldBase< T, mode >;
    /// @brief Type alias -- Field data type
    using FieldType = typename FieldImpl::FieldType;
    /// @brief Type alias -- Sample data type
    using SampleType = typename NFieldBase< T >::SampleType;
    /// @brief Type alias -- New sample callback signature
    /// @ref [SWS_CM_00702] -- Signature of Callable f
    using NewSamplesCallback = typename NFieldBase< T >::NewSamplesCallback;
    /// @brief Type alias -- Set method signature
    using SetMethod = typename SFieldBase< T >::SetMethod;
    /// @brief Type alias -- Get method signature
    using GetMethod = typename GFieldBase< T >::GetMethod;
    /// @brief Constructor
    Field() noexcept { ComLogTrace("create service proxy field"); }
    /// @brief Destructor
    ~Field() noexcept { ComLogTrace("destroy service proxy field"); }
    /// @brief Copy constructor
    /// @param[in] other
    Field(Field const& other) noexcept = delete;
    /// @brief Move constructor
    /// @param[in] other
    Field(Field&& other) noexcept = default;
    /// @brief Copy assignment operator
    /// @param[in] other
    /// @return Field&
    Field& operator=(Field const& other) noexcept = delete;
    /// @brief Move assignment operator
    /// @param[in] other
    /// @return Field&
    Field& operator=(Field&& other) noexcept = default;
    /// @brief Overloaded assignment operator
    /// @param[in] impl
    /// @return Field&
    Field& operator=(std::shared_ptr< FieldImpl >&& impl) noexcept
    {
        ComLogTrace("assign bind proxy field");
        impl_ = std::move(impl);
        return *this;
    }
    /// @brief Subscribe to field -- has notifier
    /// @code{.isoft}
    /// export_level=/COM/SoC/SOA
    /// @endcode
    /// @param[in] maxSampleCount Maximum number of samples to receive
    /// @return Result object -- empty/value or error
    /// @retval ComErrc::kMaxSampleCountNotRealizable Already subscribed, and the maximum sample count differs
    /// @ref [SWS_CM_00141] -- Method to subscribe to a service event
    /// @ref [SWS_CM_00700] -- Ensure memory allocation of maxSampleCount samples
    /// @ref [SWS_CM_00022] -- Re-entrancy – Subscribe
    template < bool c = HasNotifier(mode), typename = std::enable_if_t< c > >
    ara::core::Result< void > Subscribe(size_t maxSampleCount) noexcept
    {
        ComLogTrace("subscribe service field", GenArg(maxSampleCount));
        assert(impl_);
        return impl_->Subscribe(maxSampleCount);
    }
    /// @brief Unsubscribe from field -- has notifier
    /// @code{.isoft}
    /// export_level=/COM/SoC/SOA
    /// @endcode
    /// @ref [SWS_CM_00151] -- Method to unsubscribe from a service event
    /// @ref [SWS_CM_00023] -- Re-entrancy – Unsubscribe
    template < bool c = HasNotifier(mode), typename = std::enable_if_t< c > >
    void Unsubscribe() noexcept
    {
        ComLogTrace("unsubscribe service field");
        assert(impl_);
        impl_->Unsubscribe();
    }
    /// @brief Get field subscription state -- has notifier
    /// @code{.isoft}
    /// export_level=/COM/SoC/SOA
    /// @endcode
    /// @return Field subscription state @ref SubscriptionStateChangeHandler
    /// @ref [SWS_CM_00316] -- Query Subscription State
    /// @ref [SWS_CM_00024] -- Re-entrancy – GetSubscriptionState
    template < bool c = HasNotifier(mode), typename = std::enable_if_t< c > >
    SubscriptionState GetSubscriptionState() const noexcept
    {
        assert(impl_);
        return impl_->GetSubscriptionState();
    }
    /// @brief Check if subscribed -- has notifier
    /// @code{.isoft}
    /// export_level=/COM/SoC/SOA
    /// @endcode
    /// @return bool Is subscribed
    template < bool c = HasNotifier(mode), typename = std::enable_if_t< c > >
    bool IsSubscribed() const noexcept
    {
        return GetSubscriptionState() == SubscriptionState::kSubscribed;
    }
    /// @brief Set field receive handler -- has notifier
    /// @code{.isoft}
    /// export_level=/COM/SoC/SOA
    /// @endcode
    /// @param[in] handler Receive handler
    /// @return Result object -- empty/value or error
    /// @retval ComErrc::kSetHandlerNotSet Field set handler not set
    /// @ref [SWS_CM_00181] -- Enable service event trigger
    /// @ref [SWS_CM_00028] -- Re-entrancy – SetReceiveHandler
    template < bool c = HasNotifier(mode), typename = std::enable_if_t< c > >
    ara::core::Result< void > SetReceiveHandler(EventReceiveHandler handler) noexcept
    {
        return SetReceiveHandler(handler, [](std::function< void() > const& f) { f(); });
    }
    /// @brief Set field receive handler -- has notifier
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
    template < typename Executor, bool c = HasNotifier(mode), typename = std::enable_if_t< c > >
    ara::core::Result< void > SetReceiveHandler(EventReceiveHandler handler, Executor&& executor) noexcept
    {
        using Result = ara::core::Result< void >;
        ComLogTrace("set service field receive handler");
        if (!handler) {
            ComLogError("set service field receive handler rejected: handler invalid");
            return Result{MakeErrorCode(ComErrc::kSetHandlerNotSet, -__LINE__)};
        }
        if (!executor) {
            ComLogError("set service field receive handler rejected: executor invalid");
            return Result{MakeErrorCode(ComErrc::kCouldNotExecute, -__LINE__)};
        }
        assert(impl_);
        return impl_->SetReceiveHandler([handler{std::move(handler)}, executor{std::move(executor)}]() {
            executor([handler{std::move(handler)}]() { handler(); });
        });
    }
    /// @brief Reset field receive handler -- has notifier
    /// @code{.isoft}
    /// export_level=/COM/SoC/SOA
    /// @endcode
    /// @return Result object -- empty/value or error
    /// @retval ComErrc::kUnsetFailure Resetting the receive handler failed
    /// @ref [SWS_CM_00183] -- Disable service event trigger
    /// @ref [SWS_CM_00029] -- Re-entrancy – UnsetReceiveHandler
    template < bool c = HasNotifier(mode), typename = std::enable_if_t< c > >
    ara::core::Result< void > UnsetReceiveHandler() noexcept
    {
        ComLogTrace("unset service field receive handler");
        assert(impl_);
        return impl_->UnsetReceiveHandler();
    }
    /// @brief Set field subscription state change handler -- has notifier
    /// @code{.isoft}
    /// export_level=/COM/SoC/SOA
    /// @endcode
    /// @param[in] handler Subscription state change handler
    /// @return Result object -- empty/value or error
    /// @ref [SWS_CM_00333] -- Set Subscription State change handler
    /// @ref [SWS_CM_00025] -- Re-entrancy – SetSubscriptionStateChangeHandler
    template < bool c = HasNotifier(mode), typename = std::enable_if_t< c > >
    ara::core::Result< void > SetSubscriptionStateChangeHandler(SubscriptionStateChangeHandler handler) noexcept
    {
        return SetSubscriptionStateChangeHandler(handler, [](std::function< void() > const& f) { f(); });
    }
    /// @brief Set field subscription state change handler -- has notifier
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
    template < typename Executor, bool c = HasNotifier(mode), typename = std::enable_if_t< c > >
    ara::core::Result< void > SetSubscriptionStateChangeHandler(SubscriptionStateChangeHandler handler,
                                                                Executor&& executor) noexcept
    {
        using Result = ara::core::Result< void >;
        ComLogTrace("set service field subscription state change handler");
        if (!handler) {
            ComLogError("set service field subscription state change handler rejected: handler invalid");
            return Result{MakeErrorCode(ComErrc::kSetHandlerNotSet, -__LINE__)};
        }
        if (!executor) {
            ComLogError("set service field subscription state change handler rejected: executor invalid");
            return Result{MakeErrorCode(ComErrc::kCouldNotExecute, -__LINE__)};
        }
        assert(impl_);
        return impl_->SetSubscriptionStateChangeHandler(
            [handler{std::move(handler)}, executor{std::move(executor)}](SubscriptionState state) {
                executor([handler{std::move(handler)}, state{std::move(state)}]() { handler(state); });
            });
    }
    /// @brief Reset field subscription state change handler -- has notifier
    /// @code{.isoft}
    /// export_level=/COM/SoC/SOA
    /// @endcode
    /// @ref [SWS_CM_00334] -- Unset Subscription State change handler
    /// @ref [SWS_CM_00026] -- Re-entrancy – UnsetSubscriptionStateChangeHandler
    template < bool c = HasNotifier(mode), typename = std::enable_if_t< c > >
    void UnsetSubscriptionStateChangeHandler() noexcept
    {
        ComLogTrace("unset service field subscription state change handler");
        assert(impl_);
        impl_->UnsetSubscriptionStateChangeHandler();
    }
#ifdef HAS_COM_EVENT_LOST_CHECKER
    /// @brief Set field sample loss handler -- has notifier
    /// @code{.isoft}
    /// export_level=/COM/SoC/SOA
    /// @endcode
    /// @param[in] handler Sample loss handler
    /// @return Result object -- empty/value or error
    template < bool c = HasNotifier(mode), typename = std::enable_if_t< c > >
    ara::core::Result< void > SetSampleLostHandler(SampleLostHandler handler) noexcept
    {
        return SetSampleLostHandler(handler, [](std::function< void() > const& f) { f(); });
    }
    /// @brief Set field sample loss handler -- has notifier
    /// @code{.isoft}
    /// export_level=/COM/SoC/SOA
    /// @endcode
    /// @param[in] handler Sample loss handler
    /// @param[in] executor Executor -- the processing will take place in its context
    /// @return Result object -- empty/value or error
    /// @retval ComErrc::kCouldNotExecute Cannot execute (e.g., due to resource issues)
    template < typename Executor, bool c = HasNotifier(mode), typename = std::enable_if_t< c > >
    ara::core::Result< void > SetSampleLostHandler(SampleLostHandler handler, Executor&& executor) noexcept
    {
        using Result = ara::core::Result< void >;
        ComLogTrace("set service field sample lost handler");
        if (!handler) {
            ComLogError("set service field sample lost handler rejected: handler invalid");
            return Result{MakeErrorCode(ComErrc::kCouldNotExecute, -__LINE__)};
        }
        if (!executor) {
            ComLogError("set service field sample lost handler rejected: executor invalid");
            return Result{MakeErrorCode(ComErrc::kCouldNotExecute, -__LINE__)};
        }
        assert(impl_);
        return impl_->SetSampleLostHandler(
            [handler{std::move(handler)}, executor{std::move(executor)}](SampleLostStatus const& state) {
                executor([handler{std::move(handler)}, state]() { handler(state); });
            });
    }
    /// @brief Reset field sample loss handler -- has notifier
    /// @code{.isoft}
    /// export_level=/COM/SoC/SOA
    /// @endcode
    template < bool c = HasNotifier(mode), typename = std::enable_if_t< c > >
    void UnsetSampleLostHandler() noexcept
    {
        ComLogTrace("unset service field sample lost handler");
        assert(impl_);
        impl_->UnsetSampleLostHandler();
    }
#endif  // HAS_COM_EVENT_LOST_CHECKER
    /// @brief Get new samples -- has notifier
    /// @code{.isoft}
    /// export_level=/COM/SoC/SOA
    /// @endcode
    /// @param[in] callback Callback function to process new samples
    /// @param[in] maxNumberOfSamples Maximum number of samples to get
    /// @return Result object -- empty/value or error
    /// @retval ComErrc::kMaxSamplesReached Field cache has reached the maximum number of samples
    /// @ref [SWS_CM_00701] -- Method to update the event cache
    /// @ref [SWS_CM_00703] -- Sequence of actions in GetNewSamples
    /// @ref [SWS_CM_00704] -- Return Value
    /// @ref [SWS_CM_00714] -- Re-entrancy – GetNewSamples
    template < bool c = HasNotifier(mode), typename = std::enable_if_t< c > >
    ara::core::Result< size_t > GetNewSamples(NewSamplesCallback&& callback,
                                              size_t maxNumberOfSamples = std::numeric_limits< size_t >::max()) noexcept
    {
        return GetNewSamples(std::move(callback), maxNumberOfSamples, [](std::function< void() > const& f) { f(); });
    }
    /// @brief Get new samples -- has notifier
    /// @code{.isoft}
    /// export_level=/COM/SoC/SOA
    /// @endcode
    /// @param[in] callback Callback function to process new samples
    /// @param[in] maxNumberOfSamples Maximum number of samples to get
    /// @param[in] executor Executor -- the processing will take place in its context
    /// @return Result object -- empty/value or error
    /// @retval ComErrc::kMaxSamplesReached Field cache has reached the maximum number of samples
    /// @retval ComErrc::kCouldNotExecute Cannot execute (e.g., due to resource issues)
    /// @ref [SWS_CM_11359] -- Error behaviour of provided Execution Context to update the event cache
    /// @ref [SWS_CM_11358] -- Execution Context to update the event cache
    /// @ref [SWS_CM_00703] -- Sequence of actions in GetNewSamples
    /// @ref [SWS_CM_00704] -- Return Value
    /// @ref [SWS_CM_00714] -- Re-entrancy – GetNewSamples
    template < typename Executor, bool c = HasNotifier(mode), typename = std::enable_if_t< c > >
    ara::core::Result< size_t > GetNewSamples(NewSamplesCallback&& callback,
                                              size_t maxNumberOfSamples,
                                              Executor&& executor) noexcept
    {
        using Result = ara::core::Result< size_t >;
        ComLogTrace("get service field new samples", GenArg(maxNumberOfSamples));
        if (!executor) {
            ComLogError("get service field new samples rejected: executor invalid");
            return Result{MakeErrorCode(ComErrc::kCouldNotExecute, -__LINE__)};
        }
        assert(impl_);
        return impl_->GetNewSamples(
            [callback{std::move(callback)}, executor{std::move(executor)}](SamplePtr< SampleType const > sample) {
                executor([callback{std::move(callback)}, sample{std::move(sample)}]() { callback(std::move(sample)); });
            },
            maxNumberOfSamples);
    }
    /// @brief Get the number of remaining samples -- has notifier
    /// @code{.isoft}
    /// export_level=/COM/SoC/SOA
    /// @endcode
    /// @return Number of remaining samples
    /// @ref [SWS_CM_00705] -- Query Free Sample Slots
    /// @ref [SWS_CM_00706] -- Return Value of GetFreeSampleCount
    /// @ref [SWS_CM_11024] -- Mapping of GetFreeSampleCount method
    /// @ref [SWS_CM_00027] -- Re-entrancy – GetFreeSampleCount
    template < bool c = HasNotifier(mode), typename = std::enable_if_t< c > >
    size_t GetFreeSampleCount() noexcept
    {
        ComLogTrace("");
        assert(impl_);
        return impl_->GetFreeSampleCount();
    }
    /// @brief Get the last received E2E result -- has notifier
    /// @code{.isoft}
    /// export_level=/COM/SoC/SOA
    /// @endcode
    /// @return Result object -- E2E value or error
    /// @ref [SWS_CM_90424] -- Provide E2E Result
    template < bool c = HasNotifier(mode), typename = std::enable_if_t< c > >
    e2exf::E2EResult GetResult() const noexcept
    {
        ComLogTrace("");
        assert(impl_);
        return impl_->GetResult();
    }
    /// @brief Get the global E2E SMState -- has notifier
    /// @code{.isoft}
    /// export_level=/COM/SoC/SOA
    /// @endcode
    /// @return Result of the last called E2E_check function
    template < bool c = HasNotifier(mode), typename = std::enable_if_t< c > >
    e2e::SMState GetSMState() const noexcept
    {
        ComLogTrace("");
        assert(impl_);
        return impl_->GetSMState();
    }
    /// @brief Set field -- has setter
    /// @code{.isoft}
    /// export_level=/COM/SoC/SOA
    /// @endcode
    /// @param[in] value Field value
    /// @return Future object -- asynchronous/synchronous wait for result object
    /// @ref [SWS_CM_00113] -- Method to set the value of a field
    /// @ref [SWS_CM_00133] -- Existence of the set method
    /// @ref [SWS_CM_00031] -- Re-entrancy – Set
    template < bool c = HasSetter(mode), typename = std::enable_if_t< c > >
    ara::core::Future< FieldType > Set(FieldType const& value) noexcept
    {
        ComLogTrace("set service field sample", GenArg(value));
        assert(impl_);
        return impl_->Set(value);
    }
    /// @brief Get field -- has getter
    /// @code{.isoft}
    /// export_level=/COM/SoC/SOA
    /// @endcode
    /// @return Future object -- asynchronous/synchronous wait for result object
    /// @ref [SWS_CM_00112] -- Method to get the value of a field
    /// @ref [SWS_CM_00132] -- Existence of getter method
    /// @ref [SWS_CM_00030] -- Re-entrancy – Get
    template < bool c = HasGetter(mode), typename = std::enable_if_t< c > >
    ara::core::Future< FieldType > Get() noexcept
    {
        ComLogTrace("get service field sample");
        assert(impl_);
        return impl_->Get();
    }

private:
    std::shared_ptr< FieldImpl > impl_;
};
}  // namespace proxy
}  // namespace internal
}  // namespace com
}  // namespace ara