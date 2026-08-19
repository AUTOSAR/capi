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
/// @file       synchronized_time_base_consumer.h
/// @brief      synchronization time base consumer
/// @details
/// @date       2023-02-09
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

#ifndef _ARA_TSYNC_SYNCHRONIZED_TIME_BASE_CONSUMER_H_
#define _ARA_TSYNC_SYNCHRONIZED_TIME_BASE_CONSUMER_H_

#include <cstdint>
#include <functional>

#include "ara/core/instance_specifier.h"
#include "ara/tsync/consumer_time_base_validation_notification.h"
#include "ara/tsync/synchronized_time_base_status.h"
#include "ara/tsync/time_precision_measurement_type.h"

namespace ara {
namespace tsync {

/// @brief
/// SynchronizedTimeBaseConsumer class is the access point for time base consumers, which finds the time base bound to the current process through an InstanceSpecifier.
/// Through this class interface, you can get the current time point, rate deviation, current status, and received user data.
///
/// @traceid  {SWS_TS_01000, 20-11}
class SynchronizedTimeBaseConsumer final
{
public:
    /// @traceid  {SWS_TS_01001, 20-11}
    /// @brief    constructor
    /// @param specifier instance reference descriptor.
    explicit SynchronizedTimeBaseConsumer(ara::core::InstanceSpecifier const &specifier) noexcept;
    /// @traceid
    /// @brief    constructor for internal ara use
    /// @param name time base name.
    explicit SynchronizedTimeBaseConsumer(ara::core::String const &name) noexcept;

    /// @traceid  {SWS_TS_01002, 20-11}
    /// @brief    destructor
    ~SynchronizedTimeBaseConsumer() noexcept;

    /// @traceid  {SWS_TS_01003, 20-11}
    /// @brief    move constructor
    /// @param stbc an existing rvalue reference to time base.
    SynchronizedTimeBaseConsumer(SynchronizedTimeBaseConsumer &&stbc) noexcept;

    /// @traceid  {SWS_TS_01004, 20-11}
    /// @brief    '=' operator overload
    /// @param stbc an existing rvalue reference to time base.
    /// @returns  object reference
    SynchronizedTimeBaseConsumer &operator=(SynchronizedTimeBaseConsumer &&stbc) &noexcept;

    /// @traceid  {SWS_TS_01005, 20-11}
    /// @brief    copy constructor is prohibited
    /// @param stbc an existing time base.
    SynchronizedTimeBaseConsumer(SynchronizedTimeBaseConsumer const &stbc) = delete;

    /// @traceid  {SWS_TS_01006, 20-11}
    /// @brief    copy is prohibited
    /// @param stdc an existing time base.
    /// @returns  object reference
    SynchronizedTimeBaseConsumer &operator=(SynchronizedTimeBaseConsumer &stdc) = delete;

    /// @traceid  {SWS_TS_01007, 20-11}
    /// @brief    used to get the current time (regardless of synchronization status).
    /// @returns  obtained timestamp
    /// @note Note that this time may not be synchronized; you need to check the status via GetTimeWithStatus().
    ara::tsync::Timestamp GetCurrentTime() const noexcept;

    /// @traceid  {SWS_TS_01008, 20-11}
    /// @brief    get rate deviation.
    /// @returns  rate deviation value.
    double GetRateDeviation() const noexcept;

    /// @traceid  {SWS_TS_01009, 20-11}
    /// @brief
    /// Get a snapshot of the current clock status. Includes status flags, clock configuration, and the time value at the snapshot.
    /// @returns  synchronization time base status.
    SynchronizedTimeBaseStatus GetTimeWithStatus() const noexcept;

    /// @traceid  {SWS_TS_01010, 20-11}
    /// @brief
    /// Register a notification function that will be called when status flags change. (e.g., synchronization status, time jump, or user data).
    /// At most one notifier can be registered, and each registration will overwrite the current one.
    /// @param    notifier user-defined notification function.
    void RegisterStatusChangeNotifier(
        std::function< void(SynchronizedTimeBaseStatus const &) > const &notifier) noexcept;

    /// @traceid  {SWS_TS_01011, 20-11}
    /// @brief    unregister the notification function; after unregistration, no further notifications will be sent.
    void UnregisterStatusChangeNotifier() noexcept;

    /// @traceid  {SWS_TS_01012, 20-11}
    /// @brief
    /// Register a notification function that will be called when the synchronization status changes. At most one notifier can be registered, and each registration will overwrite the current one.
    /// @param    notifier user-defined notification function.
    void RegisterSynchronizationStateChangeNotifier(
        std::function< void(SynchronizationStatus const &) > const &notifier) noexcept;

    /// @traceid  {SWS_TS_01013, 20-11}
    /// @brief    unregister the notification function; after unregistration, no further notifications will be sent.
    void UnregisterSynchronizationStateChangeNotifier() noexcept;

    /// @traceid  {SWS_TS_01014, 20-11}
    /// @brief    register a notification function that will be called when a time jump occurs.
    ///           At most one notifier can be registered, and each registration will overwrite the current one.
    /// @param    notifier notification function.
    void RegisterTimeLeapNotifier(std::function< void(SynchronizedTimeBaseStatus const &) > const &notifier) noexcept;

    /// @traceid  {SWS_TS_01015, 20-11}
    /// @brief    unregister the notification function; after unregistration, no further notifications will be sent.
    void UnregisterTimeLeapNotifier() noexcept;

    /// @traceid  {SWS_TS_01016, 20-11}
    /// @brief    register time validation notification. Can be used by time base provider applications to receive time synchronization parameters.
    ///           At most one notifier can be registered, and each registration will overwrite the current one.
    /// @param    timeBaseValidationNotification notifier.
    void RegisterTimeValidationNotification(
        ConsumerTimeBaseValidationNotification &timeBaseValidationNotification) noexcept;

    /// @traceid  {SWS_TS_01017, 20-11}
    /// @brief    unregister time validation notification; after unregistration, no further notifications will be sent.
    void UnregisterTimeValidationNotification() noexcept;

    /// @traceid  {SWS_TS_01018,20-11}
    /// @brief Register time precision measurement notification. The registered function is called when a new time precision snapshot is available.
    ///           Tsync does not perform any queuing; if needed, it must be done within the callback function.
    /// @param    notifier notification callback function.
    void RegisterTimePrecisionMeasurementNotifier(
        std::function< void(TimePrecisionMeasurement const &) > const &notifier) noexcept;

    /// @traceid  {SWS_TS_01019,20-11}
    /// @brief    unregister time precision measurement notification; after unregistration, no further notifications will be sent.
    void UnregisterTimePrecisionMeasurementNotifier() noexcept;

private:
    /// @name SynchronizedTimeBaseConsumerPrivateImpl - forward declaration
    class SynchronizedTimeBaseConsumerPrivateImpl;

    std::unique_ptr< SynchronizedTimeBaseConsumerPrivateImpl >
        /// @name privateImpl_ pointer to private implementation class
        privateImpl_;

};  ///  class SynchronizedTimeBaseConsumer

}  // namespace tsync
}  // namespace ara

#endif  /// _ARA_TSYNC_SYNCHRONIZED_TIME_BASE_CONSUMER_H_
