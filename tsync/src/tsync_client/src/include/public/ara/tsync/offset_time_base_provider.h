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
/// @file       offset_time_base_provider.h
/// @brief      offset time base provider
/// @details
/// @date       2022-01-01
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// export_level=/TimeSync
/// module_path=/TimeSync/OffsetTimeBase
/// @endcode
///
/// ================================================================

#ifndef _ARA_TSYNC_OFFSET_TIME_BASE_PROVIDER_H_
#define _ARA_TSYNC_OFFSET_TIME_BASE_PROVIDER_H_

#include "ara/core/instance_specifier.h"
#include "ara/core/span.h"
#include "ara/tsync/provider_time_base_validation_notification.h"
#include "ara/tsync/timestamp.h"

namespace ara {
namespace tsync {

/// @brief
/// OffsetTimeBaseProvider class is the access point for offset time base providers, which finds the time base bound to the current process through an InstanceSpecifier.
/// Through this class interface, you can set, update, get offset time, and set/get rate deviation and user data.
///
/// @traceid  {SWS_TS_01200, 20-11}
class OffsetTimeBaseProvider final
{
public:
    /// @traceid  {SWS_TS_01201, 20-11}
    /// @brief    constructor
    /// @param    instanceSpecifier  descriptor
    explicit OffsetTimeBaseProvider(ara::core::InstanceSpecifier const& instanceSpecifier) noexcept;

    /// @traceid  {SWS_TS_01202,20-11}
    /// @brief    move constructor
    /// @param    stb object reference
    OffsetTimeBaseProvider(OffsetTimeBaseProvider&& stb) noexcept;

    /// @traceid  {SWS_TS_01203,20-11}
    /// @brief    operator overload
    /// @param    stb object reference
    /// @returns  object reference.
    OffsetTimeBaseProvider& operator=(OffsetTimeBaseProvider&& stb) & noexcept;

    /// @traceid  {SWS_TS_01205,20-11}
    /// @brief    copy is prohibited
    /// @param    stdp object reference
    /// @returns  object reference.
    OffsetTimeBaseProvider& operator=(OffsetTimeBaseProvider const& stdp) = delete;

    /// @brief    copy constructor is prohibited
    /// @param other - other object
    OffsetTimeBaseProvider(OffsetTimeBaseProvider const& other) = delete;

    /// @traceid  {SWS_TS_01206,20-11}
    /// @brief    destructor
    ~OffsetTimeBaseProvider() noexcept;

    /// @traceid  {SWS_TS_01207,20-11}
    /// @brief    used to set a new offset time value for the clock, which will trigger bus transmission.
    /// @param    timePoint time value
    /// @param    userData user data
    /// @returns  setting result
    ara::core::Result< void > SetOffsetTime(ara::tsync::Timestamp const timePoint,
                                            ara::core::Span< ara::core::Byte const > const userData = {}) noexcept;

    /// @brief    used to get the current time (regardless of synchronization status).
    /// @returns  current time
    /// @traceid  {SWS_TS_01208,20-11}
    ara::tsync::Timestamp GetCurrentTime() const noexcept;

    /// @traceid  {SWS_TS_01209,20-11}
    /// @brief    set rate correction.
    /// @param    rateCorrection rate correction value
    /// @returns  setting result
    ara::core::Result< void > SetRateCorrection(double const rateCorrection) noexcept;

    /// @traceid  {SWS_TS_01210,20-11}
    /// @brief    get rate deviation.
    /// @returns  rate correction value
    double GetRateDeviation() noexcept;

    /// @traceid  {SWS_TS_01211,20-11}
    /// @brief    set user data.
    /// @param    userData user data
    /// @returns  setting result
    ara::core::Result< void > SetUserData(ara::core::Span< ara::core::Byte const > const userData) noexcept;

    /// @traceid  {SWS_TS_01212,20-11}
    /// @brief    get the set user data.
    /// @returns  the set user data.
    ara::core::Span< ara::core::Byte const > GetUserData() const noexcept;

    /// @traceid  {SWS_TS_01213,20-11}
    /// @brief
    /// Register time validation notification. Can be used by time base provider applications to receive time synchronization parameters. At most one notifier can be registered, and each registration will overwrite the current one.
    /// @param    timeBaseValidationNotification notifier
    void RegisterTimeValidationNotification(
        ProviderTimeBaseValidationNotification& timeBaseValidationNotification) noexcept;

    /// @traceid  {SWS_TS_01214,20-11}
    /// @brief    unregister time validation notification.
    void UnregisterTimeValidationNotification() noexcept;

private:
    /// @name OffsetTimeBaseProviderPrivateImpl - forward declaration
    class OffsetTimeBaseProviderPrivateImpl;

    std::unique_ptr< OffsetTimeBaseProviderPrivateImpl >
        /// @name privateImpl_ pointer to private implementation class
        privateImpl_;

};  ///  class OffsetTimeBaseProvider

}  // namespace tsync
}  // namespace ara

#endif  /// _ARA_TSYNC_OFFSET_TIME_BASE_PROVIDER_H_
