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
/// @file       synchronized_time_base_provider.h
/// @brief      synchronization time base provider
/// @details
/// @date       2022-01-01
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// export_level=/TimeSync
/// module_path=/TimeSync/SyncTimeBase
/// @endcode
///
/// ================================================================

#ifndef _ARA_TSYNC_SYNCHRONIZED_TIME_BASE_PROVIDER_H_
#define _ARA_TSYNC_SYNCHRONIZED_TIME_BASE_PROVIDER_H_

#include "ara/core/instance_specifier.h"
#include "ara/core/span.h"
#include "ara/tsync/provider_time_base_validation_notification.h"
#include "ara/tsync/timestamp.h"

namespace ara {
namespace tsync {

/// @brief synchronization time base provider
/// @details
/// SynchronizedTimeBaseProvider class is the access point for synchronization time base providers, which finds the time base bound to the current process through an InstanceSpecifier.
/// @details Through this class interface, you can set, update, get global time points, and set/get rate deviation and user data.
/// @details
/// @traceid  {SWS_TS_01100}

/// @section API example
/// @par SetTime()
/// @brief
/// After this function is called, the specified time value will be immediately distributed through the underlying communication bus, and thereafter the time value will automatically evolve according to a fixed period based on user configuration. The subsequent distributed time is not a fixed value, but evolves according to the current machine's clock. The underlying principle is that at the moment this function is called, the difference between the user-set time value and the current system time value is recorded. In subsequent periodic distributions, the distributed time is system time + difference. Therefore, generally users only need to distribute once at startup and do not need to call SetTime() periodically, unless the time value needs to be changed.
/// @include SetTime.cpp

/// @par UpdateTime()
/// @brief
/// Updating time is very similar to setting time; the difference is that it does not distribute the time immediately, but waits until the next cycle (according to user configuration) to distribute the time. In other words, SetTime() distributes time additionally in real time on top of the synchronization cycle, whereas UpdateTime() does not distribute time additionally.
/// @brief The code flow is the same as SetTime(), not repeated here.
//
/// @par Time validation notification
/// @brief The time base consumer provides a time validation mechanism. Together with the time base provider, it completely obtains the four important timestamps in the PTP protocol synchronization process.
/// @include TimeBaseValidationNotification.cpp

class SynchronizedTimeBaseProvider final
{
public:
    /// @traceid  {SWS_TS_01101}
    /// @brief    constructor
    /// @param    instanceSpecifier descriptor
    explicit SynchronizedTimeBaseProvider(ara::core::InstanceSpecifier const& instanceSpecifier) noexcept;

    /// @traceid  {SWS_TS_01102}
    /// @brief    move constructor
    /// @param    stb object reference
    SynchronizedTimeBaseProvider(SynchronizedTimeBaseProvider&& stb) noexcept;

    /// @traceid  {SWS_TS_01103}
    /// @traceid  {SWS_TS_01107}
    /// @brief    operator overload
    /// @param    stb object reference
    /// @returns  object reference
    SynchronizedTimeBaseProvider& operator=(SynchronizedTimeBaseProvider&& stb) & noexcept;

    /// @traceid  {SWS_TS_01104}
    /// @brief    copy constructor is prohibited
    /// @param    stb object reference
    SynchronizedTimeBaseProvider(SynchronizedTimeBaseProvider const& stb) = delete;

    /// @traceid  {SWS_TS_01105}
    /// @brief    copy is prohibited
    /// @param    stb object reference
    /// @returns  object reference
    SynchronizedTimeBaseProvider& operator=(SynchronizedTimeBaseProvider const& stb) = delete;

    /// @traceid  {SWS_TS_01106}
    /// @brief    destructor
    ~SynchronizedTimeBaseProvider() noexcept;

    /// @traceid  {SWS_TS_01107}
    /// @brief    used to set a new time value for the clock, which will trigger bus transmission.
    /// @param    timePoint time value.
    /// @param    userData user data.
    /// @returns  setting result
    ara::core::Result< void > SetTime(ara::tsync::Timestamp const timePoint,
                                      ara::core::Span< ara::core::Byte const > const userData = {}) noexcept;

    /// @traceid  {SWS_TS_01108}
    /// @brief    used to set a new time value for the clock, only updates the clock value; bus transmission will be triggered in the next cycle.
    /// @param    timePoint time value
    /// @param    userData user data
    /// @returns  update result
    ara::core::Result< void > UpdateTime(ara::tsync::Timestamp const timePoint,
                                         ara::core::Span< ara::core::Byte const > const userData = {}) noexcept;

    /// @traceid  {SWS_TS_01109}
    /// @brief    used to get the current time (regardless of synchronization status).
    /// @returns  timestamp
    ara::tsync::Timestamp GetCurrentTime() const noexcept;

    /// @traceid  {SWS_TS_01110}
    /// @brief    set rate correction.
    /// @param    rateCorrection rate correction value. 0.5 means the rate slows down to half, 2.0 means it speeds up to twice.
    /// @returns  setting result
    ara::core::Result< void > SetRateCorrection(double const rateCorrection) noexcept;

    /// @traceid  {SWS_TS_01111}
    /// @brief    get rate deviation.
    /// @returns  set rate correction value
    double GetRateDeviation() noexcept;

    /// @traceid  {SWS_TS_01112}
    /// @brief    set user data.
    /// @param    userData user data.
    /// @returns  setting result
    ara::core::Result< void > SetUserData(ara::core::Span< ara::core::Byte const > const userData) noexcept;

    /// @traceid  {SWS_TS_01113}
    /// @brief    get user data.
    /// @returns  the set user data.
    ara::core::Span< ara::core::Byte const > GetUserData() const noexcept;

    /// @traceid  {SWS_TS_01114}
    /// @brief
    /// Register time validation notification. Can be used by time base provider applications to receive time synchronization parameters. At most one notifier can be registered, and each registration will overwrite the current one.
    /// @param    timeBaseValidationNotification notifier
    void RegisterTimeValidationNotification(
        ProviderTimeBaseValidationNotification& timeBaseValidationNotification) noexcept;

    /// @traceid  {SWS_TS_01115}
    /// @brief    unregister time validation notification.
    void UnregisterTimeValidationNotification() noexcept;

private:
    /// @brief SynchronizedTimeBaseProviderPrivateImpl
    class SynchronizedTimeBaseProviderPrivateImpl;
    std::unique_ptr< SynchronizedTimeBaseProviderPrivateImpl >
        /// @name privateImpl_ - pointer to private implementation class
        privateImpl_;

};  ///  class SynchronizedTimeBaseProvider

}  // namespace tsync
}  // namespace ara

#endif  //_ARA_TSYNC_SYNCHRONIZED_TIME_BASE_PROVIDER_H_
