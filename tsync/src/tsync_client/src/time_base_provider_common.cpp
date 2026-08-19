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
/// @file       time_base_provider_common.cpp
/// @brief      time base provider common implementation class
/// @details
/// @date       2023-02-09
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#include "ara/tsync/time_base_provider_common.h"

#include <isoft/ara_fsh/filesystem_hierarchy.h>

#include "ara/tsync/internal/config/instancespecifierset.h"
#include "ara/tsync/internal/log/logger.h"

namespace ara {
namespace tsync {

/// @brief initialize
/// @return 0 - success
/// @return <0 - failure
std::int32_t TimeBaseProviderCommon::_Init() noexcept
{
    std::int32_t retCode{0};
#if ARA_TSYNC_DEBUG_WITHOUT_EM
    ara::core::String const mapFile{"./time_base_map.json"};
#else
    ara::core::String const mapFile{isoft::ara_fsh::Process().GetTimeBaseMap()};
#endif

    /// instead of goto
    do {
        /// Open configuration, get time base name based on instance descriptor
        internal::config::InstanceSpecifierSet config;
        if (0 != config.Load(mapFile)) {
            LOG().Fatal() << "TimeBaseProviderCommon failed to Load(" << mapFile;
            retCode = kRET_E1;
            break;
        }
        this->name = std::move(config.GetTimeBaseNameByInstanceSpecifier(this->kInstanceSpecifier));

        /// Open time base proxy
        this->proxyStub = internal::timebase::proxy::Stub::CreateStub(this->name);
        if (nullptr == this->proxyStub) {
            retCode = kRET_E2;
            break;
        }
        this->resourceCtx = this->proxyStub->GetTimeBaseResourceContext();
        if (nullptr == this->resourceCtx) {
            retCode = kRET_E3;
            break;
        }

        /// instead of goto
    } while (false);

    return retCode;
}

/// @brief destroy
/// @return 0 - success
/// @return <0 - failure
std::int32_t TimeBaseProviderCommon::_Destroy() noexcept
{
    LOG().Verbose() << "TimeBaseProviderCommon Destroy proxyStub is " << proxyStub.get();
    proxyStub   = nullptr;
    resourceCtx = nullptr;
    return 0;
}

/// @brief get rate deviation
/// @return rate deviation
double TimeBaseProviderCommon::GetRateDeviation() const noexcept
{
    if (0 != Validate()) {
        return 0.0;
    }
    return resourceCtx->RateDeviation();
}

/// @brief set rate deviation
/// @param rateCorrection - rate deviation value
/// @return rate deviation
std::int32_t TimeBaseProviderCommon::SetRateCorrection(const double &rateCorrection) noexcept
{
    if (0 != Validate()) {
        return kRET_E1;
    }
    if ((rateCorrection > internal::config::kUSER_RATE_MAX) || (rateCorrection < internal::config::kUSER_RATE_MIN)) {
        return kRET_E4;
    }
    std::int32_t ret{resourceCtx->SyncProviderSetRateDeviation(rateCorrection - 1)};
    if (0 != ret) {
        return ret;
    }
    if (0 != proxyStub->Notify(internal::timebase::proxy::ProxyEventType::kSetRateDeviation)) {
        return kRET_E2;
    }
    return 0;
}

/// @brief set user data
/// @param userData - user data
/// @return 0 - success
/// @return <0 - failure
std::int32_t TimeBaseProviderCommon::SetUserData(ara::core::Span< ara::core::Byte const > const &userData) noexcept
{
    if (0 != Validate()) {
        return kRET_E1;
    }
    resourceCtx->SetUserData(userData);
    if (0 != proxyStub->Notify(internal::timebase::proxy::ProxyEventType::kSetUserData)) {
        return kRET_E2;
    }
    return 0;
}

/// @brief get user data
/// @return user data
ara::core::Span< ara::core::Byte const > TimeBaseProviderCommon::GetUserData(void) const noexcept
{
    if (0 != Validate()) {
        return ara::core::Span< ara::core::Byte const >();
    }

    return resourceCtx->GetUserData().ToByteSpan();
}

/// @brief register time validation notification. Can be used by time base provider applications to receive time synchronization parameters. At most one notifier can be registered, and each registration will overwrite the current one.
/// @param notifier - notifier
/// @return 0 - success
/// @return <0 - failure
std::int32_t TimeBaseProviderCommon::RegisterTimeValidationNotification(
    ProviderTimeBaseValidationNotification &notifier) noexcept
{
    if (0 != Validate()) {
        return kRET_E1;
    }

    internal::timebase::proxy::Stub::NotificationHandler const cb{
        [this, &notifier](internal::timebase::proxy::ProxyEventType const event, std::uint8_t const option) -> void {
            std::ignore = event;
            // FIXME: global time (system_clock)
            // time count (steady_clock)
            std::uint8_t const setMasterTimingData{static_cast< std::uint8_t >(
                internal::timebase::proxy::Message::ValidationMeasurementOption::kSetMasterTimingData)};
            std::uint8_t const setPdelayResponderData{static_cast< std::uint8_t >(
                internal::timebase::proxy::Message::ValidationMeasurementOption::kSetPdelayResponderData)};

            switch (static_cast< std::uint32_t >(option)) {
                /// @traceid{SWS_TS_00425}
                case setMasterTimingData: {
                    notifier.SetMasterTimingData(this->resourceCtx->GetMasterTimingData());
                    break;
                }

                /// @traceid{SWS_TS_00427}
                case setPdelayResponderData: {
                    notifier.SetPdelayResponderData(this->resourceCtx->GetPdelayResponderData());
                    break;
                }

                default: {
                    break;
                }
            };
        }};
    if (0 != proxyStub->RegisterNotification(internal::timebase::proxy::ProxyEventType::kValidationMeasurement, cb)) {
        return kRET_E2;
    }
    return 0;
}

/// @brief    unregister time validation notification.
/// @return 0 - success
/// @return <0 - failure
std::int32_t TimeBaseProviderCommon::UnregisterTimeValidationNotification() const noexcept
{
    if (0 != Validate()) {
        return kRET_E1;
    }
    if (0 != proxyStub->UnregisterNotification(internal::timebase::proxy::ProxyEventType::kValidationMeasurement)) {
        return kRET_E2;
    }
    return 0;
}

}  // namespace tsync
}  // namespace ara
