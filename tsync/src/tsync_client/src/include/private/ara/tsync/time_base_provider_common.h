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
/// @file       time_base_provider_common.h
/// @brief      time base provider common implementation class
/// @details
/// @date       2023-02-09
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

#ifndef ARA_TSYNC_TIME_BASE_PROVIDER_COMMON_H_
#define ARA_TSYNC_TIME_BASE_PROVIDER_COMMON_H_

#include <chrono>
#include <cstdint>

#include "ara/tsync/clock.h"
#include "ara/tsync/common.h"
#include "ara/tsync/internal/timebase/proxy/stub.h"
#include "ara/tsync/provider_time_base_validation_notification.h"
#include "ara/tsync/synchronized_time_base_provider.h"
#include "ara/tsync/time_validation_measurement_types.h"
#include "ara/tsync/timestamp.h"

namespace ara {
namespace tsync {

/// @brief time base provider common implementation class
class TimeBaseProviderCommon
{
protected:
    /// @brief constructor
    /// @param inst - instance descriptor
    explicit TimeBaseProviderCommon(ara::core::String inst) noexcept
        : kInstanceSpecifier(std::move(inst)), name{}, proxyStub{nullptr}
    {
    }
    /// @brief destructor
    virtual ~TimeBaseProviderCommon() noexcept { static_cast< void >(_Destroy()); }

public:
    /// @brief '=' operator overload
    /// @param other an existing rvalue reference.
    /// @return reference to private class object
    /// TODO(zhoubo): move assignment operator is explicitly defaulted but implicitly deleted
    /// TODO(zhoubo): deleted member function should be public
    TimeBaseProviderCommon &operator=(TimeBaseProviderCommon &&) &noexcept = delete;

    /// @brief copy constructor is prohibited
    /// @param other - other object
    TimeBaseProviderCommon(TimeBaseProviderCommon const &other) = delete;

    /// @brief copy is prohibited
    /// @param other - other object
    /// @return reference to private class object
    TimeBaseProviderCommon &operator=(TimeBaseProviderCommon const &other) = delete;

    /// @brief move constructor
    /// @param other an existing rvalue reference.
    TimeBaseProviderCommon(TimeBaseProviderCommon &&) noexcept = default;

public:
    /// @brief get current time
    /// @return timestamp
    virtual ara::tsync::Timestamp GetCurrentTime() const noexcept = 0;

    /// @brief used to set a new time value for the time domain, which will immediately trigger bus transmission. Does not modify the system clock, only records the difference.
    /// @param timePoint - time value
    /// @param userData - user data
    /// @return 0 - success
    /// @return <0 - failure
    virtual std::int32_t SetTime(ara::tsync::Timestamp const &timePoint,
                                 ara::core::Span< ara::core::Byte const > const &userData) noexcept = 0;

    /// @brief get rate deviation
    /// @return rate deviation
    double GetRateDeviation() const noexcept;

    /// @brief set rate deviation
    /// @param rateCorrection - rate deviation value
    /// @return rate deviation
    std::int32_t SetRateCorrection(const double &rateCorrection) noexcept;

    /// @brief set user data
    /// @param userData - user data
    /// @return 0 - success
    /// @return <0 - failure
    std::int32_t SetUserData(ara::core::Span< ara::core::Byte const > const &userData) noexcept;

    /// @brief get user data
    /// @return user data
    ara::core::Span< ara::core::Byte const > GetUserData(void) const noexcept;

    /// @brief register time validation notification. Can be used by time base provider applications to receive time synchronization parameters. At most one notifier can be registered, and each registration will overwrite the current one.
    /// @param notifier - notifier
    /// @return 0 - success
    /// @return <0 - failure
    std::int32_t RegisterTimeValidationNotification(ProviderTimeBaseValidationNotification &notifier) noexcept;

    /// @brief unregister time validation notification.
    /// @return 0 - success
    /// @return <0 - failure
    std::int32_t UnregisterTimeValidationNotification() const noexcept;

protected:
    /// @brief initialize
    /// @return 0 - success
    /// @return <0 - failure
    std::int32_t _Init() noexcept;

    /// @brief destroy
    /// @return 0 - success
    /// @return <0 - failure
    std::int32_t _Destroy() noexcept;

    /// @brief check the validity of the current time base
    /// @return 0 - no exception
    /// @return <0 - problematic
    virtual std::int32_t Validate() const noexcept = 0;

protected:
    /// @name instanceSpecifier - instance identifier of the current time base
    ara::core::String const kInstanceSpecifier{};  //NOLINT

    /// @name name - time base name
    ara::core::String name{};  //NOLINT

    /// @name proxyStub - time base proxy stub
    std::unique_ptr< internal::timebase::proxy::Stub > proxyStub{nullptr};  //NOLINT

    /// @name resourceCtx - time base content
    internal::timebase::resource::TBContext *resourceCtx{nullptr};  //NOLINT
};                                                                  /// class TimeBaseProviderCommon

}  // namespace tsync
}  // namespace ara

#endif  /// ARA_TSYNC_TIME_BASE_PROVIDER_COMMON_H_