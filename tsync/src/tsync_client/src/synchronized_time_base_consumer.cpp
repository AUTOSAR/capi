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
/// @file       synchronized_time_base_consumer.cpp
/// @brief      synchronization time base consumer
/// @details
/// @date       2023-02-09
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#include "ara/tsync/synchronized_time_base_consumer.h"

#include <chrono>

#include "ara/tsync/synchronized_time_base_consumer_private_impl.h"
//#include "debug.h".

namespace ara {
namespace tsync {

/// @traceid  {SWS_TS_01001, 20-11}
/// @brief    constructor
/// @param    specifier
SynchronizedTimeBaseConsumer::SynchronizedTimeBaseConsumer(ara::core::InstanceSpecifier const &specifier) noexcept
    : privateImpl_{}
{
    ara::core::String const instanceStr{std::move(specifier.ToString())};
    privateImpl_ = SynchronizedTimeBaseConsumerPrivateImpl::CreatePrivateImpl(instanceStr);
    if (nullptr == privateImpl_) {
        return;
    }
}

/// @traceid  {SWS_TS_01001, 20-11}
/// @brief    constructor
/// @param    specifier
SynchronizedTimeBaseConsumer::SynchronizedTimeBaseConsumer(ara::core::String const &name) noexcept : privateImpl_{}
{
    privateImpl_ = SynchronizedTimeBaseConsumerPrivateImpl::CreatePrivateImpl(name);
    if (nullptr == privateImpl_) {
        return;
    }
}

/// @brief    destructor
/// @traceid  {SWS_TS_01002, 20-11}
SynchronizedTimeBaseConsumer::~SynchronizedTimeBaseConsumer() noexcept { privateImpl_ = nullptr; }

/// @traceid  {SWS_TS_01003, 20-11}
/// @brief    move constructor
/// @param    stbc object reference
SynchronizedTimeBaseConsumer::SynchronizedTimeBaseConsumer(SynchronizedTimeBaseConsumer &&stbc) noexcept
    : privateImpl_{std::move(stbc.privateImpl_)}
{
}

/// @brief    operator overload

/// @traceid  {SWS_TS_01004, 20-11}
/// @brief    operator overload
/// @param    stbc object reference
/// @return   object reference
SynchronizedTimeBaseConsumer &SynchronizedTimeBaseConsumer::operator=(SynchronizedTimeBaseConsumer &&stbc) &noexcept
{
    if (this == &stbc) {
        return *this;
    }

    this->privateImpl_ = std::move(stbc.privateImpl_);
    return *this;
}

/// @brief    used to get the current time (regardless of synchronization status).
/// @returns  timestamp
/// @traceid  {SWS_TS_01007, 20-11}
ara::tsync::Timestamp SynchronizedTimeBaseConsumer::GetCurrentTime() const noexcept
{
    /// note: If the time domain is not connected, return 0, because the obtained time is meaningless at that point.
    if (nullptr == privateImpl_) {
        return ara::tsync::Timestamp(std::chrono::nanoseconds(-1));
    }

    return privateImpl_->GetCurrentTime();
}

/// @traceid  {SWS_TS_01008, 20-11}
/// @traceid {SWS_TS_00202}
/// GetRateDeviation should return the calculated rate deviation; if not yet calculated, should return the initial value 1.
/// @traceid {SWS_TS_00070} If not yet calculated, should return 0.0.
/// WTF???
/// @brief    get rate deviation.
/// @returns  rate deviation value
double SynchronizedTimeBaseConsumer::GetRateDeviation() const noexcept
{
    if (nullptr == privateImpl_) {
        return 0.0;
    }

    return privateImpl_->GetRateDeviation();
}

/// @brief
/// Get a snapshot of the current clock status. Includes status flags, clock configuration, and the time value at the snapshot.
/// @returns  synchronization time base status
/// @traceid  {SWS_TS_01009, 20-11}
SynchronizedTimeBaseStatus SynchronizedTimeBaseConsumer::GetTimeWithStatus() const noexcept
{
    if (nullptr == privateImpl_) {
        return SynchronizedTimeBaseStatus{};
    }
    return privateImpl_->GetTimeWithStatus();
}

/// @brief
// Register a notification function that will be called when status flags change. (e.g., synchronization status, time jump, or user data). At most one notifier can be registered, and each registration will overwrite the current one.
/// @param    notifier notification function;
/// @traceid  {SWS_TS_01010, 20-11}
void SynchronizedTimeBaseConsumer::RegisterStatusChangeNotifier(
    std::function< void(SynchronizedTimeBaseStatus const &) > const &notifier) noexcept
{
    if (nullptr == privateImpl_) {
        return;
    }
    return privateImpl_->RegisterStatusChangeNotifier(notifier);
}

/// @brief    unregister the notification function.
/// @traceid  {SWS_TS_01011, 20-11}
void SynchronizedTimeBaseConsumer::UnregisterStatusChangeNotifier() noexcept
{
    if (nullptr == privateImpl_) {
        return;
    }

    return privateImpl_->UnregisterStatusChangeNotifier();
}

/// @brief
/// Register a notification function that will be called when the synchronization status changes. At most one notifier can be registered, and each registration will overwrite the current one.
/// @param    notifier notification function;
/// @traceid  {SWS_TS_01012, 20-11}
void SynchronizedTimeBaseConsumer::RegisterSynchronizationStateChangeNotifier(
    std::function< void(SynchronizationStatus const &) > const &notifier) noexcept
{
    if (nullptr == privateImpl_) {
        return;
    }

    privateImpl_->RegisterSynchronizationStateChangeNotifier(notifier);
}

/// @brief    unregister the notification function.
/// @traceid  {SWS_TS_01013, 20-11}
void SynchronizedTimeBaseConsumer::UnregisterSynchronizationStateChangeNotifier() noexcept
{
    if (nullptr == privateImpl_) {
        return;
    }

    privateImpl_->UnregisterSynchronizationStateChangeNotifier();
}

/// @brief
/// Register a notification function that will be called when a time jump occurs. At most one notifier can be registered, and each registration will overwrite the current one.
/// @param    notifier notification function;
/// @traceid  {SWS_TS_01014, 20-11}
void SynchronizedTimeBaseConsumer::RegisterTimeLeapNotifier(
    std::function< void(SynchronizedTimeBaseStatus const &) > const &notifier) noexcept
{
    if (nullptr == privateImpl_) {
        return;
    }

    privateImpl_->RegisterTimeLeapNotifier(notifier);
}

/// @brief    unregister the notification function.
/// @traceid  {SWS_TS_01015, 20-11}
void SynchronizedTimeBaseConsumer::UnregisterTimeLeapNotifier() noexcept
{
    if (nullptr == privateImpl_) {
        return;
    }

    privateImpl_->UnregisterTimeLeapNotifier();
}

/// @brief
// Register time validation notification. Can be used by time base provider applications to receive time synchronization parameters. At most one notifier can be registered, and each registration will overwrite the current one.
/// @param    timeBaseValidationNotification notifier;
/// @traceid  {SWS_TS_01016, 20-11}
void SynchronizedTimeBaseConsumer::RegisterTimeValidationNotification(
    ConsumerTimeBaseValidationNotification &timeBaseValidationNotification) noexcept
{
    if (nullptr == privateImpl_) {
        return;
    }

    privateImpl_->RegisterTimeValidationNotification(timeBaseValidationNotification);
}

/// @brief    unregister time validation notification.
/// @traceid  {SWS_TS_01017, 20-11}
void SynchronizedTimeBaseConsumer::UnregisterTimeValidationNotification() noexcept
{
    if (nullptr == privateImpl_) {
        return;
    }

    privateImpl_->UnregisterTimeValidationNotification();
}

/// @brief    register time precision measurement notification. The registered function is called when a new time precision snapshot is available.
//          Tsync does not perform any queuing; if needed, it must be done within the callback function.
/// @param    notifier notification callback function
/// @traceid  {SWS_TS_01018,20-11}
void SynchronizedTimeBaseConsumer::RegisterTimePrecisionMeasurementNotifier(
    std::function< void(TimePrecisionMeasurement const &) > const &notifier) noexcept
{
    if (nullptr == privateImpl_) {
        return;
    }
    privateImpl_->RegisterTimePrecisionMeasurementNotifier(notifier);
}

/// @brief    unregister time precision measurement notification. The registered function is called when a new time precision snapshot is available.
//          Tsync does not perform any queuing; if needed, it must be done within the callback function.
/// @traceid  {SWS_TS_01019,20-11}
void SynchronizedTimeBaseConsumer::UnregisterTimePrecisionMeasurementNotifier() noexcept
{
    if (nullptr == privateImpl_) {
        return;
    }
    privateImpl_->UnregisterTimePrecisionMeasurementNotifier();
}

}  // namespace tsync
}  // namespace ara
