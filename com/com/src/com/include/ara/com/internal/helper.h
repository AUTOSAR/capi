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
/// @file       helper.h
/// @brief      Communication helper header file
/// @details
/// @date       2022-07-19
/// @author     mazelin
/// @version    1.2.0
///
/// ================================================================

#pragma once

#include <thread>

#include "ara/com/internal/proxy/field.h"
#include "ara/com/internal/proxy/method.h"
#include "ara/com/internal/proxy/proxy.h"
#include "ara/com/internal/skeleton/event.h"
#include "ara/com/internal/skeleton/field.h"
#include "ara/com/internal/skeleton/skeleton.h"

namespace ara {
namespace com {
namespace internal {
namespace proxy {
/// @brief Get data cache -- Field implementation type -- Proxy side
/// @return Value cache
template < typename Field >
std::enable_if_t< IsField< Field >::value, typename Field::FieldType& > Value() noexcept
{
    static typename Field::FieldType s_Instance;
    return s_Instance;
}
/// @brief Handle notification -- Event/notifiable field
/// @tparam Subscriber Subscriber type
/// @param[in] subscriber Subscriber
template < typename Subscriber >
void HandleNotice(Subscriber& subscriber,
                  std::enable_if_t< IsEvent< Subscriber >::value || EnableNField< Subscriber >() >* = nullptr) noexcept
{
    ara::core::Vector< typename Subscriber::SampleType > samples;
    auto passedNumberRes{subscriber.GetNewSamples([&](auto sample) { samples.emplace_back(std::move(*sample)); }, 1)};
    if (!passedNumberRes) {
        auto passedNumberError{std::move(passedNumberRes).Error()};
        ComLogWarning("get notice failed", GenArg(passedNumberError));
        return;
    }
    auto e2eResultRes{subscriber.GetResult()};
    if (!e2eResultRes) {
        auto e2eError{std::move(e2eResultRes).Error()};
        ComLogWarning("check notice failed", GenArg(e2eError));
        return;
    }
    auto e2eResult{std::move(e2eResultRes).Value()};
    auto e2eSMState{e2eResult.GetSMState()};
    auto e2eProfileCheckStatus{e2eResult.GetProfileCheckStatus()};
    ComLogDebug("check notice done", GenArg(e2eSMState), GenArg(e2eProfileCheckStatus));
    auto passedNumber{std::move(passedNumberRes).Value()};
    ComLogInfo("handle notice done", GenK2V("passed", static_cast< int32_t >(passedNumber)), GenArg(samples));
}
/// @brief Handle subscription state change -- Event/notifiable field
/// @tparam Subscriber Subscriber type
/// @param[in] subscriber Subscriber
template < typename Subscriber >
void HandleSubscriptionStateChange(
    Subscriber& subscriber,
    SubscriptionState state,
    std::enable_if_t< IsEvent< Subscriber >::value || EnableNField< Subscriber >() >* = nullptr) noexcept
{
    std::ignore = subscriber;
    ComLogInfo("handle notice subscription state change", GenArg(state));
}
/// @brief Subscribe -- Event/notifiable field
/// @tparam Subscriber Subscriber type
/// @param[in] subscriber Subscriber
/// @return Result object -- empty/value or error
template < typename Subscriber >
ara::core::Result< void > Subscribe(
    Subscriber& subscriber,
    std::enable_if_t< IsEvent< Subscriber >::value || EnableNField< Subscriber >() >* = nullptr) noexcept
{
    using Result = ara::core::Result< void >;
    if (subscriber.IsSubscribed()) {
        return Result{};
    }
    {
        auto voidRes{subscriber.SetReceiveHandler([&]() { HandleNotice(subscriber); })};
        if (!voidRes) {
            return voidRes;
        }
    }
    {
        auto voidRes{subscriber.SetSubscriptionStateChangeHandler(
            [&](auto state) { HandleSubscriptionStateChange(subscriber, state); })};
        if (!voidRes) {
            return voidRes;
        }
    }
    return subscriber.Subscribe(1);
}
/// @brief Unsubscribe -- Event/notifiable field
/// @tparam Subscriber Subscriber type
/// @param[in] subscriber Subscriber
template < typename Subscriber >
void Unsubscribe(Subscriber& subscriber,
                 std::enable_if_t< IsEvent< Subscriber >::value || EnableNField< Subscriber >() >* = nullptr) noexcept
{
    if (!subscriber.IsSubscribed()) {
        return;
    }
    subscriber.Unsubscribe();
}
/// @brief Call method -- One-way method
/// @tparam Args Method parameter list types
/// @tparam Vals Value list types (compatible with implicit type conversion of parameter list)
/// @param[in] method Method
/// @param[in] vals Value list
/// @return Result object -- empty/value or error
template < typename... Args, typename... Vals >
ara::core::Result< void > Call(FMethod< void(Args...) >& method, Vals&&... vals) noexcept
{
    using Result = ara::core::Result< void >;
    ComLogInfo("call method", GenK2V("input(s)", std::forward< Vals >(vals)...));
    method(std::forward< Vals >(vals)...);
    ComLogInfo("call method done", GenK2V("input(s)", std::forward< Vals >(vals)...));
    return Result{};
}
/// @brief Call method -- Two-way method (returns void)
/// @tparam Vals Value list types (compatible with implicit type conversion of parameter list)
/// @param[in] method Method
/// @param[in] vals Value list
/// @return Result object -- empty/value or error
template < typename... Args, typename... Vals >
ara::core::Result< void > Call(VMethod< void(Args...) >& method, Vals&&... vals) noexcept
{
    using Result = ara::core::Result< void >;
    ComLogInfo("call method...", GenK2V("input(s)", std::forward< Vals >(vals)...));
    auto future{method(std::forward< Vals >(vals)...)};
    constexpr auto kWaitTimeout{std::chrono::milliseconds(1000)};
    constexpr auto kCheckInterval{std::chrono::milliseconds(50)};
    for (auto begin{std::chrono::system_clock::now()}, end{begin + kWaitTimeout};
         future.wait_until(begin += kCheckInterval) != ara::core::future_status::ready;) {
        if (begin < end) {
            ComLogDebug("call method wait...");
            continue;
        }
        ComLogWarning("call method timeout", GenK2V("input(s)", std::forward< Vals >(vals)...));
        return Result{MakeErrorCode(ComErrc::kUnknownError, -__LINE__)};
    }
    auto outputRes{future.GetResult()};
    if (!outputRes) {
        auto error{std::move(outputRes).Error()};
        ComLogWarning("call method failed", GenK2V("input(s)", std::forward< Vals >(vals)...), GenArg(error));
        return Result{error};
    }
    ComLogInfo("call method done", GenK2V("input(s)", std::forward< Vals >(vals)...));
    return Result{};
}
/// @brief Call method -- Two-way method (returns value)
/// @tparam R Method return type
/// @tparam Args Method parameter list types
/// @tparam Vals Value list types (compatible with implicit type conversion of parameter list)
/// @param[in] method Method
/// @param[in] vals Value list
/// @return Result object -- empty/value or error
template < typename R, typename... Args, typename... Vals >
ara::core::Result< void > Call(RMethod< R(Args...) >& method, Vals&&... vals) noexcept
{
    using Result = ara::core::Result< void >;
    ComLogInfo("call method...", GenK2V("input(s)", std::forward< Vals >(vals)...));
    auto future{method(std::forward< Vals >(vals)...)};
    constexpr auto kWaitTimeout{std::chrono::milliseconds(1000)};
    constexpr auto kCheckInterval{std::chrono::milliseconds(50)};
    for (auto begin{std::chrono::system_clock::now()}, end{begin + kWaitTimeout};
         future.wait_until(begin += kCheckInterval) != ara::core::future_status::ready;) {
        if (begin < end) {
            ComLogDebug("call method wait...");
            continue;
        }
        ComLogWarning("call method timeout", GenK2V("input(s)", std::forward< Vals >(vals)...));
        return Result{MakeErrorCode(ComErrc::kUnknownError, -__LINE__)};
    }
    auto outputRes{future.GetResult()};
    if (!outputRes) {
        auto error{std::move(outputRes).Error()};
        ComLogWarning("call method failed", GenK2V("input(s)", std::forward< Vals >(vals)...), GenArg(error));
        return Result{error};
    }
    auto output{std::move(outputRes).Value()};
    ComLogInfo("call method done", GenK2V("input(s)", std::forward< Vals >(vals)...), GenArg(output));
    return Result{};
}
/// @brief Call method -- One-way method
/// @tparam Args Method parameter list types
/// @tparam kI Method parameter index list
/// @param[in] method Method
/// @return Result object -- empty/value or error
template < typename... Args, size_t... kI >
ara::core::Result< void > Call(FMethod< void(Args...) >& method,
                               std::index_sequence< kI... >,
                               std::enable_if_t< sizeof...(Args) >* = nullptr) noexcept
{
    std::tuple< std::remove_cv_t< std::remove_reference_t< Args > >... > args;
    return Call(method, std::forward< Args >(std::get< kI >(args))...);
}
/// @brief Call method -- One-way method
/// @tparam Args Method parameter list types
/// @param[in] method Method
/// @return Result object -- empty/value or error
template < typename... Args >
ara::core::Result< void > Call(FMethod< void(Args...) >& method,
                               std::enable_if_t< sizeof...(Args) >* = nullptr) noexcept
{
    return Call(method, std::index_sequence_for< Args... >{});
}
/// @brief Call method -- Two-way method (returns void)
/// @tparam Args Method parameter list types
/// @tparam kI Method parameter index list
/// @param[in] method Method
/// @return Result object -- empty/value or error
template < typename... Args, size_t... kI >
ara::core::Result< void > Call(VMethod< void(Args...) >& method,
                               std::index_sequence< kI... >,
                               std::enable_if_t< sizeof...(Args) >* = nullptr) noexcept
{
    std::tuple< std::remove_cv_t< std::remove_reference_t< Args > >... > args;
    return Call(method, std::forward< Args >(std::get< kI >(args))...);
}
/// @brief Call method -- Two-way method (returns void)
/// @tparam Args Method parameter list types
/// @param[in] method Method
/// @return Result object -- empty/value or error
template < typename... Args >
ara::core::Result< void > Call(VMethod< void(Args...) >& method,
                               std::enable_if_t< sizeof...(Args) >* = nullptr) noexcept
{
    return Call(method, std::index_sequence_for< Args... >{});
}
/// @brief Call method -- Two-way method (returns value)
/// @tparam R Method return type
/// @tparam Args Method parameter list types
/// @tparam kI Method parameter index list
/// @param[in] method Method
/// @return Result object -- empty/value or error
template < typename R, typename... Args, size_t... kI >
ara::core::Result< void > Call(RMethod< R(Args...) >& method,
                               std::index_sequence< kI... >,
                               std::enable_if_t< sizeof...(Args) >* = nullptr) noexcept
{
    std::tuple< std::remove_cv_t< std::remove_reference_t< Args > >... > args;
    return Call(method, std::forward< Args >(std::get< kI >(args))...);
}
/// @brief Call method -- Two-way method (returns value)
/// @tparam R Method return type
/// @tparam Args Method parameter list types
/// @param[in] method Method
/// @return Result object -- empty/value or error
template < typename R, typename... Args >
ara::core::Result< void > Call(RMethod< R(Args...) >& method, std::enable_if_t< sizeof...(Args) >* = nullptr) noexcept
{
    return Call(method, std::index_sequence_for< Args... >{});
}
/// @brief Set field -- Settable field
/// @tparam Field Field type
/// @tparam Val Value type (compatible with implicit type conversion of parameter)
/// @param[in] field Field
/// @param[in] val Value
/// @return Result object -- empty/value or error
template < typename Field, typename Val >
ara::core::Result< void > Set(Field& field, Val&& val, std::enable_if_t< EnableSField< Field >() >* = nullptr) noexcept
{
    using Result = ara::core::Result< void >;
    ComLogInfo("set field...", GenK2V("value", val));
    auto future{field.Set(std::forward< Val >(val))};
    constexpr auto kWaitTimeout{std::chrono::milliseconds(1000)};
    constexpr auto kCheckInterval{std::chrono::milliseconds(50)};
    for (auto begin{std::chrono::system_clock::now()}, end{begin + kWaitTimeout};
         future.wait_until(begin += kCheckInterval) != ara::core::future_status::ready;) {
        if (begin < end) {
            ComLogDebug("set field wait...");
            continue;
        }
        ComLogWarning("set field timeout", GenK2V("value", val));
        return Result{MakeErrorCode(ComErrc::kUnknownError, -__LINE__)};
    }
    auto valueRes{future.GetResult()};
    if (!valueRes) {
        auto error{std::move(valueRes).Error()};
        ComLogWarning("set field failed", GenK2V("value", val), GenArg(error));
        return Result{error};
    }
    Value< Field >() = std::move(valueRes).Value();
    ComLogInfo("set field done", GenK2V("value", Value< Field >()));
    return Result{};
}
/// @brief Set field -- Settable field
/// @tparam Field Field type
/// @param[in] field Field
/// @return Result object -- empty/value or error
template < typename Field >
ara::core::Result< void > Set(Field& field, std::enable_if_t< EnableSField< Field >() >* = nullptr) noexcept
{
    return Set(field, Value< Field >());
}
/// @brief Get field -- Gettable field
/// @tparam Field Field type
/// @param[in] field Field
/// @return Result object -- empty/value or error
template < typename Field >
ara::core::Result< void > Get(Field& field, std::enable_if_t< EnableGField< Field >() >* = nullptr) noexcept
{
    using Result = ara::core::Result< void >;
    ComLogInfo("get field...");
    auto future{field.Get()};
    constexpr auto kWaitTimeout{std::chrono::milliseconds(1000)};
    constexpr auto kCheckInterval{std::chrono::milliseconds(50)};
    for (auto begin{std::chrono::system_clock::now()}, end{begin + kWaitTimeout};
         future.wait_until(begin += kCheckInterval) != ara::core::future_status::ready;) {
        if (begin < end) {
            ComLogDebug("get field wait...");
            continue;
        }
        ComLogWarning("get field timeout");
        return Result{MakeErrorCode(ComErrc::kUnknownError, -__LINE__)};
    }
    auto valueRes{future.GetResult()};
    if (!valueRes) {
        auto error{std::move(valueRes).Error()};
        ComLogWarning("get field failed", GenArg(error));
        return Result{error};
    }
    Value< Field >() = std::move(valueRes).Value();
    ComLogInfo("get field done", GenK2V("value", Value< Field >()));
    return Result{};
}
}  // namespace proxy
namespace skeleton {
/// @brief Get data cache -- Field implementation type -- Skeleton side
/// @return Data cache
template < typename Field >
std::enable_if_t< IsField< Field >::value, typename Field::FieldType& > Value() noexcept
{
    static typename Field::FieldType s_Instance;
    return s_Instance;
}
/// @brief Notify -- Event
/// @tparam Notifier Notifier type
/// @tparam Val Value type (compatible with implicit type conversion of parameter)
/// @param[in] notifier Notifier
/// @param[in] val Value
/// @return Result object -- empty/value or error
template < typename Notifier, typename Val >
ara::core::Result< void > Notify(Notifier& notifier,
                                 Val&& val,
                                 std::enable_if_t< IsEvent< Notifier >::value >* = nullptr) noexcept
{
    using Result = ara::core::Result< void >;
    ComLogInfo("notify event", GenK2V("sample", val));
    auto voidRes{notifier.Send(std::forward< Val >(val))};
    if (!voidRes) {
        ComLogWarning("notify event failed", GenK2V("sample", val), GenK2V("error", voidRes));
        return voidRes;
    }
    return Result{};
}
/// @brief Notify -- Event
/// @tparam Notifier Notifier type
/// @param[in] notifier Notifier
/// @return Result object -- empty/value or error
template < typename Notifier >
ara::core::Result< void > Notify(Notifier& notifier, std::enable_if_t< IsEvent< Notifier >::value >* = nullptr) noexcept
{
    using Result = ara::core::Result< void >;
    auto sampleRes{notifier.Allocate()};
    if (!sampleRes) {
        auto error{std::move(sampleRes).Error()};
        ComLogWarning("notify event failed", GenArg(error));
        return Result{error};
    }
    auto sample{std::move(sampleRes).Value()};
    return Notify(notifier, sample);
}
/// @brief Notify -- Notifiable field
/// @tparam Notifier Notifier type
/// @tparam Val Value type (compatible with implicit type conversion of parameter)
/// @param[in] notifier Notifier
/// @param[in] val Value
/// @return Result object -- empty/value or error
template < typename Notifier, typename Val >
ara::core::Result< void > Notify(Notifier& notifier,
                                 Val&& val,
                                 std::enable_if_t< EnableNField< Notifier >() >* = nullptr) noexcept
{
    using Result = ara::core::Result< void >;
    ComLogInfo("notify field", GenK2V("value", val));
    auto voidRes{notifier.Update(std::forward< Val >(val))};
    if (!voidRes) {
        ComLogWarning("notify field failed", GenK2V("value", val), GenK2V("error", voidRes));
        return voidRes;
    }
    return Result{};
}
/// @brief Notify -- Notifiable field
/// @tparam Notifier Notifier type
/// @param[in] notifier Notifier
/// @return Result object -- empty/value or error
template < typename Notifier >
ara::core::Result< void > Notify(Notifier& notifier, std::enable_if_t< EnableNField< Notifier >() >* = nullptr) noexcept
{
    return Notify(notifier, Value< Notifier >());
}
/// @brief Handle set -- Settable field
/// @tparam Field Field type
/// @param[in] val Value
/// @return Future object -- asynchronous/synchronous wait for result object
template < typename Field >
std::enable_if_t< IsField< Field >::value, ara::core::Future< typename Field::FieldType > > HandleSet(
    typename Field::FieldType const& val) noexcept
{
    ara::core::Promise< typename Field::FieldType > promise;
    ComLogInfo("handle field set", GenK2V("last", Value< Field >()), GenArg(val));
    Value< Field >() = val;
    promise.set_value(Value< Field >());
    return promise.get_future();
}
/// @brief Handle get -- Gettable field
/// @tparam Field Field type
/// @param[in] val Value
/// @return Future object -- asynchronous/synchronous wait for result object
template < typename Field >
std::enable_if_t< IsField< Field >::value, ara::core::Future< typename Field::FieldType > > HandleGet() noexcept
{
    ara::core::Promise< typename Field::FieldType > promise;
    ComLogInfo("handle field get", GenK2V("last", Value< Field >()));
    promise.set_value(Value< Field >());
    return promise.get_future();
}
/// @brief Preset field -- Notifiable field
/// @tparam Field Field type
/// @param[in] field Field
/// @return Result object -- empty/value or error
template < typename Field >
ara::core::Result< void > Preset(Field& field, std::enable_if_t< IsNField< Field >::value >* = nullptr) noexcept
{
    using Result = ara::core::Result< void >;
    {
        auto voidRes{field.Update(Value< Field >())};
        if (!voidRes) {
            ComLogWarning("init field value failed", GenK2V("error", voidRes));
            return voidRes;
        }
    }
    return Result{};
}
/// @brief Preset field -- Settable field
/// @tparam Field Field type
/// @param[in] field Field
/// @return Result object -- empty/value or error
template < typename Field >
ara::core::Result< void > Preset(Field& field, std::enable_if_t< IsSField< Field >::value >* = nullptr) noexcept
{
    using Result = ara::core::Result< void >;
    {
        auto voidRes{field.RegisterSetHandler(&HandleSet< Field >)};
        if (!voidRes) {
            ComLogWarning("register field set handler failed", GenK2V("error", voidRes));
            return voidRes;
        }
    }
    return Result{};
}
/// @brief Preset field -- Gettable field
/// @tparam Field Field type
/// @param[in] field Field
/// @return Result object -- empty/value or error
template < typename Field >
ara::core::Result< void > Preset(Field& field, std::enable_if_t< IsGField< Field >::value >* = nullptr) noexcept
{
    using Result = ara::core::Result< void >;
    bool custom{true};
    if (custom) {
        auto voidRes{field.RegisterGetHandler(&HandleGet< Field >)};
        if (!voidRes) {
            ComLogWarning("register field get handler failed", GenK2V("error", voidRes));
            return voidRes;
        }
    } else {
        auto voidRes{field.Update(Value< Field >())};
        if (!voidRes) {
            ComLogWarning("init field value failed", GenK2V("error", voidRes));
            return voidRes;
        }
    }
    return Result{};
}
/// @brief Preset field -- Notifiable/Settable field
/// @tparam Field Field type
/// @param[in] field Field
/// @return Result object -- empty/value or error
template < typename Field >
ara::core::Result< void > Preset(Field& field, std::enable_if_t< IsNSField< Field >::value >* = nullptr) noexcept
{
    using Result = ara::core::Result< void >;
    {
        auto voidRes{field.RegisterSetHandler(&HandleSet< Field >)};
        if (!voidRes) {
            ComLogWarning("register field set handler failed", GenK2V("error", voidRes));
            return voidRes;
        }
    }
    {
        auto voidRes{field.Update(Value< Field >())};
        if (!voidRes) {
            ComLogWarning("init field value failed", GenK2V("error", voidRes));
            return voidRes;
        }
    }
    return Result{};
}
/// @brief Preset field -- Notifiable/Gettable field
/// @tparam Field Field type
/// @param[in] field Field
/// @return Result object -- empty/value or error
template < typename Field >
ara::core::Result< void > Preset(Field& field, std::enable_if_t< IsNGField< Field >::value >* = nullptr) noexcept
{
    using Result = ara::core::Result< void >;
    bool custom{true};
    if (custom) {
        auto voidRes{field.RegisterGetHandler(&HandleGet< Field >)};
        if (!voidRes) {
            ComLogWarning("register field get handler failed", GenK2V("error", voidRes));
            return voidRes;
        }
    }
    {
        auto voidRes{field.Update(Value< Field >())};
        if (!voidRes) {
            ComLogWarning("init field value failed", GenK2V("error", voidRes));
            return voidRes;
        }
    }
    return Result{};
}
/// @brief Preset field -- Settable/Gettable field
/// @tparam Field Field type
/// @param[in] field Field
/// @return Result object -- empty/value or error
template < typename Field >
ara::core::Result< void > Preset(Field& field, std::enable_if_t< IsSGField< Field >::value >* = nullptr) noexcept
{
    using Result = ara::core::Result< void >;
    {
        auto voidRes{field.RegisterSetHandler(&HandleSet< Field >)};
        if (!voidRes) {
            ComLogWarning("register field set handler failed", GenK2V("error", voidRes));
            return voidRes;
        }
    }
    bool custom{true};
    if (custom) {
        auto voidRes{field.RegisterGetHandler(&HandleGet< Field >)};
        if (!voidRes) {
            ComLogWarning("register field get handler failed", GenK2V("error", voidRes));
            return voidRes;
        }
    } else {
        auto voidRes{field.Update(Value< Field >())};
        if (!voidRes) {
            ComLogWarning("init field value failed", GenK2V("error", voidRes));
            return voidRes;
        }
    }
    return Result{};
}
/// @brief Preset field -- Notifiable/Settable/Gettable field
/// @tparam Field Field type
/// @param[in] field Field
/// @return Result object -- empty/value or error
template < typename Field >
ara::core::Result< void > Preset(Field& field, std::enable_if_t< IsNSGField< Field >::value >* = nullptr) noexcept
{
    using Result = ara::core::Result< void >;
    {
        auto voidRes{field.RegisterSetHandler(std::bind(&HandleSet< Field >, std::placeholders::_1))};
        if (!voidRes) {
            ComLogWarning("register field set handler failed", GenK2V("error", voidRes));
            return voidRes;
        }
    }
    bool custom{true};
    if (custom) {
        auto voidRes{field.RegisterGetHandler(&HandleGet< Field >)};
        if (!voidRes) {
            ComLogWarning("register field get handler failed", GenK2V("error", voidRes));
            return voidRes;
        }
    }
    {
        auto voidRes{field.Update(Value< Field >())};
        if (!voidRes) {
            ComLogWarning("init field value failed", GenK2V("error", voidRes));
            return voidRes;
        }
    }
    return Result{};
}
}  // namespace skeleton
}  // namespace internal
}  // namespace com
}  // namespace ara