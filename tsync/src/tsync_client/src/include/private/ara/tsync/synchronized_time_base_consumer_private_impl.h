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
/// @file       synchronized_time_base_consumer_private_impl.h
/// @brief      time base consumer private implementation class
/// @details
/// @date       2023-02-08
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// export_level=/TimeSync/SyncTimeBase
/// module_path=/TimeSync/SyncTimeBase
/// @endcode
///
/// ================================================================

#ifndef ARA_TSYNC_SYNCHRONIZED_TIME_BASE_CONSUMER_PRIVATE_IMPL_H_
#define ARA_TSYNC_SYNCHRONIZED_TIME_BASE_CONSUMER_PRIVATE_IMPL_H_

#include <chrono>

#include "ara/tsync/clock.h"
#include "ara/tsync/consumer_time_base_validation_notification.h"
#include "ara/tsync/internal/timebase/proxy/stub.h"
#include "ara/tsync/internal/timestatus.h"
#include "ara/tsync/synchronized_time_base_consumer.h"
#include "ara/tsync/synchronized_time_base_status.h"
#include "ara/tsync/synchronized_time_base_status_private_impl.h"
#include "ara/tsync/time_validation_measurement_types.h"
#include "ara/tsync/timestamp.h"
#include "common.h"

namespace ara {
namespace tsync {

/// @brief time base consumer private class
class SynchronizedTimeBaseConsumer::SynchronizedTimeBaseConsumerPrivateImpl
{
public:
    /// @brief create time base consumer private class
    /// @param inst - time base instance descriptor
    /// @return pointer to time base consumer private class
    static std::unique_ptr< SynchronizedTimeBaseConsumer::SynchronizedTimeBaseConsumerPrivateImpl > CreatePrivateImpl(
        ara::core::String const &inst) noexcept;

    /// @brief constructor
    /// @param inst - instance descriptor
    explicit SynchronizedTimeBaseConsumerPrivateImpl(ara::core::String inst) noexcept
        : kInstanceSpecifier{std::move(inst)}, name_{}, status_{}, proxyStub_{nullptr}
    {
    }

    /// @brief copy constructor is prohibited
    /// @param other - other object
    SynchronizedTimeBaseConsumerPrivateImpl(SynchronizedTimeBaseConsumerPrivateImpl const &other) = delete;

    /// @brief copy is prohibited
    /// @param other - other object
    /// @return reference to private class object
    SynchronizedTimeBaseConsumerPrivateImpl &operator=(SynchronizedTimeBaseConsumerPrivateImpl const &other) = delete;

    /// @brief move constructor
    /// @param other an existing rvalue reference to time base consumer private class.
    SynchronizedTimeBaseConsumerPrivateImpl(SynchronizedTimeBaseConsumerPrivateImpl &&) noexcept = default;

    /// @brief '=' operator overload
    /// @param other an existing rvalue reference to time base consumer private class.
    /// @return reference to private class object
    /// TODO(niuliming): move assignment operator is explicitly defaulted but implicitly deleted
    SynchronizedTimeBaseConsumerPrivateImpl &operator=(SynchronizedTimeBaseConsumerPrivateImpl &&) &noexcept = delete;

    /// @brief destructor
    virtual ~SynchronizedTimeBaseConsumerPrivateImpl() noexcept { static_cast< void >(_destroy()); }

    /// @brief get current time
    /// @return timestamp
    ara::tsync::Timestamp GetCurrentTime() const noexcept;

    /// @brief get time and status
    /// @return time status
    SynchronizedTimeBaseStatus const &GetTimeWithStatus() const noexcept;

    /// @brief get rate deviation
    /// @return rate deviation
    double GetRateDeviation() const noexcept;

    /// @brief register status change notification
    /// @param notifier - notifier
    void RegisterStatusChangeNotifier(
        std::function< void(SynchronizedTimeBaseStatus const &) > const &notifier) noexcept;

    /// @brief unregister status change notification
    void UnregisterStatusChangeNotifier() const noexcept;

    /// @brief register synchronization status change notification
    /// @param notifier - notifier
    void RegisterSynchronizationStateChangeNotifier(
        std::function< void(SynchronizationStatus const &) > const &notifier) noexcept;

    /// @brief unregister synchronization status change notification
    void UnregisterSynchronizationStateChangeNotifier() const noexcept;

    /// @brief register time jump notification
    /// @param notifier - notifier
    void RegisterTimeLeapNotifier(std::function< void(SynchronizedTimeBaseStatus const &) > const &notifier) noexcept;

    /// @brief unregister time jump notification
    void UnregisterTimeLeapNotifier() const noexcept;

    /// @brief register time validation notification
    /// @param notifier - notifier
    void RegisterTimeValidationNotification(ConsumerTimeBaseValidationNotification &notifier) noexcept;

    /// @brief unregister time validation notification
    void UnregisterTimeValidationNotification() const noexcept;

    /// @brief register time precision measurement notification
    /// @param notifier - notifier
    void RegisterTimePrecisionMeasurementNotifier(
        std::function< void(TimePrecisionMeasurement const &) > const &notifier) noexcept;

    /// @brief unregister time precision measurement notification
    void UnregisterTimePrecisionMeasurementNotifier() const noexcept;

private:
    /// @brief initialize
    /// @return 0 - success
    /// @return <0 - failure
    std::int32_t _init() noexcept;

    /// @brief destroy
    /// @return 0 - success
    /// @return <0 - failure
    std::int32_t _destroy() noexcept;

    /// @brief update time base status
    /// @return 0 - success
    /// @return <0 - failure
    std::int32_t _updateStatus(void) const noexcept;

private:
    /// @name instanceSpecifier - instance identifier of the current time base
    ara::core::String const kInstanceSpecifier{};

    /// @name name - time base name
    ara::core::String name_{};

    /// @name status - time status
    mutable SynchronizedTimeBaseStatus status_{};

    /// @name proxyStub - time base proxy stub
    std::unique_ptr< internal::timebase::proxy::Stub > proxyStub_{nullptr};

    /// @name resourceCtx - time base content
    internal::timebase::resource::TBContext *resourceCtx_{nullptr};
};

}  // namespace tsync
}  // namespace ara

#endif  // _ARA_TSYNC_SYNCHRONIZED_TIME_BASE_CONSUMER_PRIVATE_IMPL_H_
