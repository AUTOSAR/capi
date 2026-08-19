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
/// @file       synchronized_time_base_provider.cpp
/// @brief      synchronization time base provider implementation class
/// @details
/// @date       2023-02-09
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#include "ara/tsync/synchronized_time_base_provider.h"

#include "ara/tsync/internal/log/logger.h"
#include "ara/tsync/synchronized_time_base_provider_private_impl.h"
#include "ara/tsync/tsync_error_domain.h"

namespace ara {
namespace tsync {

/// @traceid  {SWS_TS_01101}
/// @brief    constructor
/// @param    instanceSpecifier
SynchronizedTimeBaseProvider::SynchronizedTimeBaseProvider(
    ara::core::InstanceSpecifier const &instanceSpecifier) noexcept
    : privateImpl_{}
{
    ara::core::String const instanceStr{std::move(instanceSpecifier.ToString())};
    privateImpl_ = SynchronizedTimeBaseProviderPrivateImpl::CreatePrivateImpl(instanceStr);
    if (nullptr == privateImpl_) {
        LOG().Fatal() << "nullptr == privateImpl_";
    }
}

/// @traceid  {SWS_TS_01102}
/// @brief    move constructor
/// @param    stb object reference
SynchronizedTimeBaseProvider::SynchronizedTimeBaseProvider(SynchronizedTimeBaseProvider &&stb) noexcept
    : privateImpl_{std::move(stb.privateImpl_)}
{
}

/// @traceid  {SWS_TS_01103}
/// @traceid  {SWS_TS_01107}
/// @brief    operator overload
/// @param    stb object reference
/// @returns object reference
SynchronizedTimeBaseProvider &SynchronizedTimeBaseProvider::operator=(SynchronizedTimeBaseProvider &&stb) &noexcept
{
    if (this == &stb) {
        return *this;
    }

    this->privateImpl_ = std::move(stb.privateImpl_);
    return *this;
}

/// @traceid  {SWS_TS_01106}
/// @brief    destructor
SynchronizedTimeBaseProvider::~SynchronizedTimeBaseProvider() noexcept
{
    /// TODO(person in charge): persistent storage
    /// @traceid  {SWS_TS_00212}
    this->privateImpl_ = nullptr;
}

/// @brief    used to set a new time value for the clock, which will trigger bus transmission.
/// @param    timePoint time value.
/// @param    userData user data.
/// @returns  setting result
/// @traceid  {SWS_TS_01107}
ara::core::Result< void > SynchronizedTimeBaseProvider::SetTime(
    ara::tsync::Timestamp const timePoint, ara::core::Span< ara::core::Byte const > const userData) noexcept
{
    std::int32_t r{0};

    if (nullptr == privateImpl_) {
        LOG().Error() << "nullptr == privateImpl_";
        return ara::core::Result< void >::FromError(TsyncErrc::kTimeCannotSet);
    }

    r = privateImpl_->SetTime(timePoint, userData);
    if (0 != r) {
        LOG().Error() << "SynchronizedTimeBaseProvider.SetTime(): " << r;
        return ara::core::Result< void >::FromError(TsyncErrc::kTimeCannotSet);
    }

    return ara::core::Result< void >::FromValue();
}

/// @traceid  {SWS_TS_01108}
/// @brief    used to set a new time value for the clock, only updates the clock value; bus transmission will be triggered in the next cycle.
/// @param    timePoint time value
/// @param    userData user data
/// @returns  update result

ara::core::Result< void > SynchronizedTimeBaseProvider::UpdateTime(
    ara::tsync::Timestamp const timePoint, ara::core::Span< ara::core::Byte const > const userData) noexcept
{
    std::int32_t r{0};

    if (nullptr == privateImpl_) {
        LOG().Error() << "nullptr == privateImpl_";
        return ara::core::Result< void >::FromError(TsyncErrc::kTimeCannotSet);
    }

    r = privateImpl_->UpdateTime(timePoint, userData);
    if (0 != r) {
        LOG().Error() << "SynchronizedTimeBaseProvider.UpdateTime(): " << r;
        return ara::core::Result< void >::FromError(TsyncErrc::kDaemonConnectionLost);
    }

    return ara::core::Result< void >::FromValue();
}

/// @traceid  {SWS_TS_01109}
/// @brief    used to get the current time (regardless of synchronization status).
/// @returns current time
ara::tsync::Timestamp SynchronizedTimeBaseProvider::GetCurrentTime() const noexcept
{
    // If connecting to the time domain fails, return 0.
    // NOTE: SWS does not specify the behavior on failure; failure to connect to the domain is a serious error, and returning time is meaningless.

    if (nullptr == privateImpl_) {
        LOG().Error() << "nullptr == privateImpl_";
        return ara::tsync::Timestamp(std::chrono::nanoseconds(-1));
    }

    return privateImpl_->GetCurrentTime();
}

/// @traceid  {SWS_TS_01110}
/// @brief    set rate correction.
/// @param    rateCorrection rate correction value. 0.5 means the rate slows down to half, 2.0 means it speeds up to twice.
/// @returns  setting result
ara::core::Result< void > SynchronizedTimeBaseProvider::SetRateCorrection(double const rateCorrection) noexcept
{
    std::int32_t r{0};

    if (nullptr == privateImpl_) {
        LOG().Error() << "nullptr == privateImpl_";
        return ara::core::Result< void >::FromError(TsyncErrc::kTimeCannotSet);
    }

    r = privateImpl_->SetRateCorrection(rateCorrection);
    if (0 != r) {
        LOG().Error() << "SynchronizedTimeBaseProvider.SetRateCorrection(): " << r;
        return ara::core::Result< void >::FromError(TsyncErrc::kLimitsExceeded);
    }

    return ara::core::Result< void >::FromValue();
}

/// @brief    get rate correction.
/// @returns rate correction
/// @traceid  {SWS_TS_01111}
double SynchronizedTimeBaseProvider::GetRateDeviation() noexcept
{
    if (nullptr == privateImpl_) {
        LOG().Error() << "nullptr == privateImpl_";
        return 0.0;
    }

    return privateImpl_->GetRateDeviation();
}

/// @traceid  {SWS_TS_01112}
/// @brief    set user data.
/// @param    userData user data.
/// @returns  setting result
ara::core::Result< void > SynchronizedTimeBaseProvider::SetUserData(
    ara::core::Span< ara::core::Byte const > const userData) noexcept
{
    std::int32_t r{0};

    if (nullptr == privateImpl_) {
        LOG().Error() << "nullptr == privateImpl_";
        return ara::core::Result< void >::FromError(TsyncErrc::kDaemonConnectionLost);
    }
    r = privateImpl_->SetUserData(userData);
    if (0 != r) {
        return ara::core::Result< void >::FromError(TsyncErrc::kTimeCannotSet);
    }
    return ara::core::Result< void >::FromValue();
}

/// @traceid  {SWS_TS_01113}
/// @brief    get user data.
/// @returns  the set user data.
ara::core::Span< ara::core::Byte const > SynchronizedTimeBaseProvider::GetUserData() const noexcept
{
    if (nullptr == privateImpl_) {
        LOG().Error() << "nullptr == privateImpl_";
        return ara::core::Span< ara::core::Byte const >();
    }
    return privateImpl_->GetUserData();
}

/// @brief
// Register time validation notification. Can be used by time base provider applications to receive time synchronization parameters. At most one notifier can be registered, and each registration will overwrite the current one.
/// @param    timeBaseValidationNotification notifier;
/// @traceid  {SWS_TS_01114}
void SynchronizedTimeBaseProvider::RegisterTimeValidationNotification(
    ProviderTimeBaseValidationNotification &timeBaseValidationNotification) noexcept
{
    std::int32_t r{0};

    if (nullptr == privateImpl_) {
        LOG().Error() << "nullptr == privateImpl_";
    }
    r = privateImpl_->RegisterTimeValidationNotification(timeBaseValidationNotification);
    if (0 != r) {
        LOG().Error() << "SynchronizedTimeBaseProvider RegisterTimeValidationNotification, error:" << r;
    }
}

/// @brief    unregister time validation notification.
/// @traceid  {SWS_TS_01115}
void SynchronizedTimeBaseProvider::UnregisterTimeValidationNotification() noexcept
{
    std::int32_t r{0};

    if (nullptr == privateImpl_) {
        LOG().Error() << "nullptr == privateImpl_";
    }
    r = privateImpl_->UnregisterTimeValidationNotification();
    if (0 != r) {
        LOG().Error() << "SynchronizedTimeBaseProvider UnregisterTimeValidationNotification, error:" << r;
    }
}

}  // namespace tsync
}  // namespace ara
