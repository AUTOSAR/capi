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
/// @file       synchronized_time_base_consumer_private_impl.cpp
/// @brief      time base consumer private implementation class
/// @details
/// @date       2023-02-08
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#include "ara/tsync/synchronized_time_base_consumer_private_impl.h"

#include <isoft/ara_fsh/filesystem_hierarchy.h>

#include <chrono>

#include "ara/tsync/internal/config/instancespecifierset.h"
#include "ara/tsync/internal/log/logger.h"
#include "ara/tsync/time_base_provider_common.h"

namespace ara {
namespace tsync {

/// @brief create time base consumer private class
/// @param inst - time base instance descriptor
/// @return pointer to time base consumer private class
std::unique_ptr< SynchronizedTimeBaseConsumer::SynchronizedTimeBaseConsumerPrivateImpl >
SynchronizedTimeBaseConsumer::SynchronizedTimeBaseConsumerPrivateImpl::CreatePrivateImpl(
    ara::core::String const &inst) noexcept
{
    std::unique_ptr< SynchronizedTimeBaseConsumer::SynchronizedTimeBaseConsumerPrivateImpl > pri;
    pri = std::make_unique< SynchronizedTimeBaseConsumer::SynchronizedTimeBaseConsumerPrivateImpl >(inst);
    if (nullptr != pri) {
        if (0 != pri->_init()) {
            pri = nullptr;
        }
    }
    return pri;
}

/// @brief initialize
/// @return 0 - success
/// @return <0 - failure
std::int32_t SynchronizedTimeBaseConsumer::SynchronizedTimeBaseConsumerPrivateImpl::_init() noexcept
{
    std::int32_t retCode{0};
#ifdef ARA_TSYNC_DEBUG_WITHOUT_EM
    ara::core::String mapFile = "./time_base_map.json";
#else
    ara::core::String const mapFile{isoft::ara_fsh::Process().GetTimeBaseMap()};
#endif

    /// instead of goto
    do {
        /// Open configuration, get time base name based on instance descriptor
        internal::config::InstanceSpecifierSet config;
        std::int32_t const r{config.Load(mapFile)};
        if (0 != r) {
            // retCode = kRET_E1;
        } else {
            this->name_ = std::move(config.GetTimeBaseNameByInstanceSpecifier(this->kInstanceSpecifier));
        }
        /// If no mapping is found, it is considered internal library usage; the passed value is the name
        if (this->name_.empty()) {
            this->name_ = kInstanceSpecifier;
        }

        /// Open time base proxy
        this->proxyStub_ = internal::timebase::proxy::Stub::CreateStub(this->name_);
        if (nullptr == this->proxyStub_) {
            retCode = kRET_E2;
            break;
        }
        this->resourceCtx_ = this->proxyStub_->GetTimeBaseResourceContext();
        if (nullptr == this->resourceCtx_) {
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
std::int32_t SynchronizedTimeBaseConsumer::SynchronizedTimeBaseConsumerPrivateImpl::_destroy() noexcept
{
    proxyStub_   = nullptr;
    resourceCtx_ = nullptr;
    return 0;
}

/// @brief get current time
/// @return timestamp
ara::tsync::Timestamp SynchronizedTimeBaseConsumer::SynchronizedTimeBaseConsumerPrivateImpl::GetCurrentTime()
    const noexcept
{
    if (nullptr == resourceCtx_) {
        return ara::tsync::Timestamp(std::chrono::nanoseconds(-1));
    }
    return resourceCtx_->GetConsumerCurrentTime();
}

/// @brief get time and status
/// @return time status
SynchronizedTimeBaseStatus const &
SynchronizedTimeBaseConsumer::SynchronizedTimeBaseConsumerPrivateImpl::GetTimeWithStatus() const noexcept
{
    if (nullptr == resourceCtx_) {
        return status_;
    }
    // if (0 !=
    // const_cast<SynchronizedTimeBaseConsumer::SynchronizedTimeBaseConsumerPrivateImpl*>(this)->_updateStatus())
    // {.
    std::int32_t const r{this->_updateStatus()};
    if (0 != r) {
        LOG().Error() << "GetTimeWithStatus, _updateStatus error:" << r;
    }
    return status_;
}

/// @brief get rate deviation
/// @return rate deviation
double SynchronizedTimeBaseConsumer::SynchronizedTimeBaseConsumerPrivateImpl::GetRateDeviation() const noexcept
{
    if (nullptr == resourceCtx_) {
        return 0.0;
    }
    return resourceCtx_->GetConsumerRateDeviation();
}

/// @brief update time base status
/// @return 0 - success
/// @return <0 - failure
std::int32_t SynchronizedTimeBaseConsumer::SynchronizedTimeBaseConsumerPrivateImpl::_updateStatus(void) const noexcept
{
    SynchronizedTimeBaseStatus::SynchronizedTimeBaseStatusPrivateImpl *statusPri{nullptr};

    if (nullptr == this->resourceCtx_) {
        return kRET_E1;
    }
    if (nullptr == this->status_.privateImpl) {
        return kRET_E2;
    }
    statusPri = this->status_.privateImpl.get();
    if (nullptr == statusPri) {
        return kRET_E3;
    }

    internal::TimeStatus const &timeStatus{resourceCtx_->GetTimeStatus()};
    if (timeStatus.IsTimeLeapFuture()) {
        statusPri->timeLeapJump = LeapJump::kTimeLeapFuture;
    } else if (timeStatus.IsTimeLeapPast()) {
        statusPri->timeLeapJump = LeapJump::kTimeLeapPast;
    } else if (timeStatus.IsTimeLeapNone()) {
        statusPri->timeLeapJump = LeapJump::kTimeLeapNone;
    } else {
        ;
    }

    if (timeStatus.IsGlobalTimeBase()) {
        statusPri->status = SynchronizationStatus::kSynchronized;
    } else if (timeStatus.IsSyncToGateway()) {
        statusPri->status = SynchronizationStatus::kSynchToGateway;
    } else if (timeStatus.IsTimeout()) {
        statusPri->status = SynchronizationStatus::kTimeOut;
    } else {
        statusPri->status = SynchronizationStatus::kNotSynchronizedUntilStartup;
    }

    statusPri->userData     = resourceCtx_->GetUserData();
    statusPri->creationTime = this->GetCurrentTime();

    return 0;
}

/// @brief register status change notification
/// @param notifier - notifier
void SynchronizedTimeBaseConsumer::SynchronizedTimeBaseConsumerPrivateImpl::RegisterStatusChangeNotifier(
    std::function< void(SynchronizedTimeBaseStatus const &) > const &notifier) noexcept
{
    if (nullptr == proxyStub_) {
        return;
    }
    internal::timebase::proxy::Stub::NotificationHandler const cb{
        [this, notifier](internal::timebase::proxy::ProxyEventType const event, std::uint8_t const option) -> void {
            std::ignore = event;
            std::ignore = option;
            std::int32_t const r{_updateStatus()};
            if (0 != r) {
                LOG().Error() << "RegisterStatusChangeNotifier _updateStatus error:" << r;
                return;
            }
            if (nullptr != notifier) {
                notifier(this->status_);
            }
        }};

    std::int32_t const ret{
        proxyStub_->RegisterNotification(ara::tsync::internal::timebase::proxy::ProxyEventType::kStatusChanged, cb)};
    if (0 != ret) {
        LOG().Error() << "RegisterStatusChangeNotifier RegisterNotification error:" << ret;
    }
}

/// @brief unregister status change notification
void SynchronizedTimeBaseConsumer::SynchronizedTimeBaseConsumerPrivateImpl::UnregisterStatusChangeNotifier()
    const noexcept
{
    if (nullptr == proxyStub_) {
        return;
    }
    std::int32_t const ret{
        proxyStub_->UnregisterNotification(ara::tsync::internal::timebase::proxy::ProxyEventType::kStatusChanged)};
    if (0 != ret) {
        LOG().Error() << "UnregisterStatusChangeNotifier UnregisterNotification error:" << ret;
    }
}

/// @brief register synchronization status change notification
/// @param notifier - notifier
void SynchronizedTimeBaseConsumer::SynchronizedTimeBaseConsumerPrivateImpl::RegisterSynchronizationStateChangeNotifier(
    std::function< void(SynchronizationStatus const &) > const &notifier) noexcept
{
    if (nullptr == proxyStub_) {
        return;
    }
    internal::timebase::proxy::Stub::NotificationHandler const cb{
        [this, notifier](internal::timebase::proxy::ProxyEventType const event, std::uint8_t const option) -> void {
            std::ignore = event;
            std::ignore = option;
            if (0 != _updateStatus()) {
                LOG().Error() << "RegisterSynchronizationStateChangeNotifier "
                                 "_updateStatus error";
                return;
            }
            if (nullptr != notifier) {
                notifier(this->status_.privateImpl->status);
            }
        }};
    std::int32_t const ret{proxyStub_->RegisterNotification(
        ara::tsync::internal::timebase::proxy::ProxyEventType::kSynchronizationStateChanged, cb)};
    if (0 != ret) {
        LOG().Error() << "RegisterSynchronizationStateChangeNotifier "
                         "RegisterNotification error:"
                      << ret;
    }
}

/// @brief unregister synchronization status change notification
void SynchronizedTimeBaseConsumer::SynchronizedTimeBaseConsumerPrivateImpl::
    UnregisterSynchronizationStateChangeNotifier() const noexcept
{
    if (nullptr == proxyStub_) {
        return;
    }
    std::int32_t const ret{proxyStub_->UnregisterNotification(
        ara::tsync::internal::timebase::proxy::ProxyEventType::kSynchronizationStateChanged)};
    if (0 != ret) {
        LOG().Error() << "UnregisterSynchronizationStateChangeNotifier "
                         "UnregisterNotification error:"
                      << ret;
    }
}

/// @brief register time jump notification
/// @param notifier - notifier
void SynchronizedTimeBaseConsumer::SynchronizedTimeBaseConsumerPrivateImpl::RegisterTimeLeapNotifier(
    std::function< void(SynchronizedTimeBaseStatus const &) > const &notifier) noexcept
{
    if (nullptr == proxyStub_) {
        return;
    }
    internal::timebase::proxy::Stub::NotificationHandler const cb{
        [this, notifier](internal::timebase::proxy::ProxyEventType const event, std::uint8_t const option) -> void {
            std::ignore = event;
            std::ignore = option;
            if (0 != _updateStatus()) {
                LOG().Error() << "RegisterTimeLeapNotifier _updateStatus error";
                return;
            }
            if (nullptr != notifier) {
                notifier(this->status_);
            }
        }};
    std::int32_t const ret{
        proxyStub_->RegisterNotification(ara::tsync::internal::timebase::proxy::ProxyEventType::kLeapJump, cb)};
    if (0 != ret) {
        LOG().Error() << "RegisterTimeLeapNotifier RegisterNotification error:" << ret;
    }
}

/// @brief unregister time jump notification
void SynchronizedTimeBaseConsumer::SynchronizedTimeBaseConsumerPrivateImpl::UnregisterTimeLeapNotifier() const noexcept
{
    if (nullptr == proxyStub_) {
        return;
    }
    std::int32_t const ret{
        proxyStub_->UnregisterNotification(ara::tsync::internal::timebase::proxy::ProxyEventType::kLeapJump)};
    if (0 != ret) {
        LOG().Error() << "UnregisterTimeLeapNotifier UnregisterNotification error:" << ret;
    }
}

/// @brief register time validation notification
/// @param notifier - notifier
void SynchronizedTimeBaseConsumer::SynchronizedTimeBaseConsumerPrivateImpl::RegisterTimeValidationNotification(
    ConsumerTimeBaseValidationNotification &notifier) noexcept
{
    if (nullptr == proxyStub_) {
        return;
    }
    internal::timebase::proxy::Stub::NotificationHandler const cb{
        [this, &notifier](internal::timebase::proxy::ProxyEventType const event, std::uint8_t const option) -> void {
            std::ignore = event;
            std::uint8_t const setSlaveTimingData{static_cast< std::uint8_t >(
                internal::timebase::proxy::Message::ValidationMeasurementOption::kSetSlaveTimingData)};
            std::uint8_t const setPdelayInitiatorData{static_cast< std::uint8_t >(
                internal::timebase::proxy::Message::ValidationMeasurementOption::kSetPdelayInitiatorData)};
            switch (static_cast< std::uint32_t >(option)) {
                /// @traceid{SWS_TS_00424}
                case setSlaveTimingData: {
                    notifier.SetSlaveTimingData(this->resourceCtx_->GetSlaveTimingData());
                    break;
                }

                /// @traceid{SWS_TS_00426}
                case setPdelayInitiatorData: {
                    notifier.SetPdelayInitiatorData(this->resourceCtx_->GetPdelayInitiatorData());
                    break;
                }

                default: {
                    break;
                }
            }
        }};
    std::int32_t const ret{proxyStub_->RegisterNotification(
        ara::tsync::internal::timebase::proxy::ProxyEventType::kValidationMeasurement, cb)};
    if (0 != ret) {
        LOG().Error() << "RegisterTimeValidationNotification RegisterNotification error:" << ret;
    }
}

/// @brief unregister time validation notification
void SynchronizedTimeBaseConsumer::SynchronizedTimeBaseConsumerPrivateImpl::UnregisterTimeValidationNotification()
    const noexcept
{
    if (nullptr == proxyStub_) {
        return;
    }
    std::int32_t const ret{proxyStub_->UnregisterNotification(
        ara::tsync::internal::timebase::proxy::ProxyEventType::kValidationMeasurement)};
    if (0 != ret) {
        LOG().Error() << "UnregisterTimeValidationNotification UnregisterNotification error:" << ret;
    }
}

/// @brief register time precision measurement notification
/// @param notifier - notifier
void SynchronizedTimeBaseConsumer::SynchronizedTimeBaseConsumerPrivateImpl::RegisterTimePrecisionMeasurementNotifier(
    std::function< void(TimePrecisionMeasurement const &) > const &notifier) noexcept
{
    if (nullptr == proxyStub_) {
        return;
    }
    if (nullptr == notifier) {
        return;
    }
    /// @traceid {SWS_TS_00800}
    internal::timebase::proxy::Stub::NotificationHandler const cb{[this, notifier](
                                                                      internal::timebase::proxy::ProxyEventType const
                                                                          event,
                                                                      std::uint8_t const option) -> void {
        std::ignore = event;
        std::ignore = option;
        // hjw
        if (nullptr == this->resourceCtx_) {
            return;
        }
        if (nullptr == this->status_.privateImpl) {
            return;
        }
        if (0 != _updateStatus()) {
            LOG().Error() << "RegisterTimePrecisionMeasurementNotifier _updateStatus error";
            return;
        }
        SynchronizedTimeBaseStatus::SynchronizedTimeBaseStatusPrivateImpl const *const statusPri{
            this->status_.privateImpl.get()};
        if (nullptr == statusPri) {
            return;
        }
        /// TODO(person in charge): specific meaning of timestamp
        internal::TimeValue glbTime;
        internal::TimeValue tlSyncTime;
        glbTime.FromChrono(this->resourceCtx_->GlobalTime().time_since_epoch());
        tlSyncTime.FromChrono(this->resourceCtx_->GetTLSyncTime().time_since_epoch());
        // TODO(person in charge): statusPri->status has set already?
        internal::TimeStatus tmStatus;
        switch (statusPri->status) {
            case SynchronizationStatus::kTimeOut: {
                tmStatus.SetTimeout(true);
                break;
            }

            case SynchronizationStatus::kSynchronized: {
                tmStatus.SetGlobalTimeBase(true);
                break;
            }

            case SynchronizationStatus::kSynchToGateway: {
                tmStatus.SetSyncToGateway(true);
                break;
            }

            case SynchronizationStatus::kNotSynchronizedUntilStartup: {
                break;
            }
        }
        switch (statusPri->timeLeapJump) {
            case LeapJump::kTimeLeapFuture: {
                tmStatus.SetTimeLeapFuture();
                break;
            }

            case LeapJump::kTimeLeapPast: {
                tmStatus.SetTimeLeapPast();
                break;
            }

            case LeapJump::kTimeLeapNone: {
                break;
            }
        }
        TimePrecisionMeasurement tpm{0};
        tpm.timeBaseStatus = tmStatus.ToUint8();

        tpm.glbSeconds     = static_cast< std::uint32_t >(glbTime.GetSecond());
        tpm.glbNanoSeconds = static_cast< std::uint32_t >(glbTime.GetNanoSecond());
        /// @traceid {SWS_TS_00801} If it is an offset time domain, only the above three are passed.
        if (resourceCtx_->IsValidSynchronizedTimeBaseConsumer()) {
            tpm.virtualLocalTimeLow = this->resourceCtx_->GetVirtualLocalTime();
            // NOTE:
            // GetRateDeviation() returns double, but TimePrecisionMeasurement uses std::int16_t, so multiply by 1000 to convert to integer.
            //      To meet the numerical requirements, it cannot be guaranteed, because SWS specifies that TimePrecisionMeasurement.rateDeviation is in the range of +-32000
            //      PPM, but the value of rateDeviation is not specified. If it is extremely large, say 9x10^300, it cannot be satisfied anyway.
            //      Therefore, we do not consider meeting the numerical requirements; we only consider general actual situations. That is, rateDeviation is usually very small,
            //      temporarily assume a maximum of 0.01, in which case magnifying by 10000 times can meet the requirement.
            int32_t const mNum{1000000};
            tpm.rateDeviation  = static_cast< std::int16_t >(this->resourceCtx_->RateDeviation() * mNum);
            tpm.locSeconds     = static_cast< std::uint32_t >(tlSyncTime.GetSecond());
            tpm.locNanoSeconds = static_cast< std::uint32_t >(tlSyncTime.GetNanoSecond());
            tpm.pathDelay      = this->resourceCtx_->PathDelay();
        }
        notifier(tpm);
    }};
    std::int32_t const ret{proxyStub_->RegisterNotification(
        ara::tsync::internal::timebase::proxy::ProxyEventType::kPrecisionMeasurement, cb)};
    if (0 != ret) {
        LOG().Error() << "RegisterTimePrecisionMeasurementNotifier "
                         "RegisterNotification error:"
                      << ret;
    }
}

/// @brief unregister time precision measurement notification
void SynchronizedTimeBaseConsumer::SynchronizedTimeBaseConsumerPrivateImpl::UnregisterTimePrecisionMeasurementNotifier()
    const noexcept
{
    if (nullptr == proxyStub_) {
        return;
    }
    std::int32_t const ret{proxyStub_->UnregisterNotification(
        ara::tsync::internal::timebase::proxy::ProxyEventType::kPrecisionMeasurement)};
    if (0 != ret) {
        LOG().Error() << "UnregisterTimePrecisionMeasurementNotifier "
                         "UnregisterNotification error:"
                      << ret;
    }
}

}  // namespace tsync
}  // namespace ara
