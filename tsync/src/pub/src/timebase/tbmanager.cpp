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
/// @file       tbmanager.cpp
/// @brief      time base management class
/// @details
/// @date       2023-02-01
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#include "ara/tsync/internal/timebase/tbmanager.h"

#include "ara/tsync/internal/common.h"
#include "ara/tsync/internal/log/logger.h"
#include "isoft/util/mix.h"

namespace ara {
namespace tsync {
namespace internal {
namespace timebase {

/// @brief log output
/// @return Logger object reference
static inline ara::tsync::internal::tslog::Logger &LOG() noexcept { return ara::tsync::internal::tslog::Log(); }

/// @brief update time base resource status based on time domain configuration
/// @param tbId - time base ID
/// @param FromDomainId - time domain that triggered this update
/// @param isSync - whether triggered by a synchronization domain; only synchronization domain triggers rrc calculation
/// @return check result
StatusChecker TBManager::_updateTimeBaseResource(timebase::resource::TimeBaseId const tbId,
                                                 internal::TimeDomainId const &fromDomainId,
                                                 bool const isSync) noexcept
{
    StatusChecker sc;
    ara::core::String tbName;
    timebase::resource::TBContext *timeBaseCtx{nullptr};
    config::TimeBase const *timeBaseCfg{nullptr};
    internal::TimeDomainId domainId;
    std::shared_ptr< timedomain::TDContext > timeDomainCtx;

    if (nullptr == resourceMan_) {
        return sc;
    }

    if (nullptr == timeDomainManager_) {
        return sc;
    }

    if (nullptr == configManager_) {
        return sc;
    }

    timeBaseCtx = resourceMan_->GetContext(tbId);
    if (nullptr == timeBaseCtx) {
        return sc;
    }
    /// If it is a time base provider, no need to check time base and notify status changes, nor time precision measurement notification
    if (timeBaseCtx->IsProvider()) {
        return sc;
    }

    domainId      = timeBaseCtx->GetTimeData().DomainId();
    timeDomainCtx = timeDomainManager_->GetContext(domainId);
    if (nullptr == timeDomainCtx) {
        return sc;
    }
    config::TimeDomainSet::Domain const *const timeDomainCfg{timeDomainManager_->GetConfig(domainId)};
    if (nullptr == timeDomainCfg) {
        return sc;
    }

    if (fromDomainId == domainId) {
        /// Simulate goto
        do {
            tbName = timeBaseCtx->GetName().ToString();
            if (nullptr == configManager_->GetTimeBaseSet()) {
                break;
            }
            timeBaseCfg = configManager_->GetTimeBaseSet()->GetTimeBase(tbName);
            if (nullptr == timeBaseCfg) {
                break;
            }

            /// Check and update user data
            /// Only check user data of the current time base
            if (timeDomainCtx->GetUserData() != timeBaseCtx->GetTimeData().GetUserData()) {
                sc.SetUserDataChanged(true);
            }

            // First copy the time base context status, otherwise if the synchronization status has not changed, the sc.status synchronization status is initial state, meaningless
            sc.status = timeBaseCtx->GetTimeData().GetTimeStatus();
            /// Check and update synchronization status
            /// First check the status of the current time base; if unchanged, check the status of the underlying bound synchronization time domain
            if (!timeBaseCtx->GetTimeData().GetTimeStatus().IsSameSyncStatus(timeDomainCtx->GetStatus())) {
                sc.status.SetSameSyncStatus(timeDomainCtx->GetStatus());
                sc.SetSynchronizationStatusChanged(true);
            } else if (timeDomainCtx->IsOffsetDomain()) {
                /// If the status of the current time base (domain) has not changed, and the current time base is an offset time base, further check the status of the underlying referenced synchronization time domain
                std::shared_ptr< timedomain::TDContext > const refTimeDomainCtx{
                    timeDomainManager_->GetContext(timeBaseCtx->GetRefTimeData().DomainId())};
                if (nullptr != refTimeDomainCtx) {
                    if (!timeBaseCtx->GetRefTimeData().GetTimeStatus().IsSameSyncStatus(
                            refTimeDomainCtx->GetStatus())) {
                        timeBaseCtx->GetRefTimeData().SetTimeStatus(refTimeDomainCtx->GetStatus());
                        sc.status.SetSameSyncStatus(refTimeDomainCtx->GetStatus());
                        sc.SetSynchronizationStatusChanged(true);
                    }
                }
            } else {
            }

            /// Check time jump
            /// @traceid{SWS_TS_00141}
            /// Time jump check must be performed after each synchronization. But it must be after at least one successful synchronization, i.e., ara::tsync::SynchronizationStatus
            /// is not equal to kNotSynchronizedUntilStartup (meaning, do not check on first synchronization)
            bool const syncToGW{timeBaseCtx->GetTimeData().GetTimeStatus().IsSyncToGateway()};
            if (!timeBaseCtx->GetTimeData().GetTimeStatus().IsGlobalTimeBase() && !syncToGW) {
                break;
            }

            if (nullptr == configManager_->GetTimeDomainSet()) {
                break;
            }
            ara::core::StringView const slaveName{
                std::move(internal::config::BaseName(timeBaseCfg->GetNetworkTimeConsumer()))};
            config::TimeDomainSet::Slave const *const timeDomainSlaveCfg{
                configManager_->GetTimeDomainSet()->GetSlave(domainId, slaveName)};
            if (nullptr == timeDomainSlaveCfg) {
                break;
            }

            /// Retrieve whether TimeLeap threshold is reached
            double leapJumpTs{timeDomainCtx->GetLeapJumpValue()};
            if (timeDomainCtx->IsOffsetDomain()) {
                /// If current is offset time base, add the jump value of the underlying referenced synchronization time domain
                std::shared_ptr< timedomain::TDContext > const refTimeDomainCtx{
                    timeDomainManager_->GetContext(timeBaseCtx->GetRefTimeData().DomainId())};
                if (nullptr != refTimeDomainCtx) {
                    if (!refTimeDomainCtx->HaveSyncOverTwice()) {
                        break;
                    }
                    leapJumpTs += refTimeDomainCtx->GetLeapJumpValue();
                }
            }
            /// @traceid{SWS_TS_00027}
            if (leapJumpTs > 0) {
                /// @traceid{SWS_TS_00139}
                if (!isoft::util::IsEq0(timeDomainSlaveCfg->GetTimeLeapFutureThreshold())) {
                    if (leapJumpTs > timeDomainSlaveCfg->GetTimeLeapFutureThreshold()) {
                        sc.status.SetTimeLeapFuture();
                        timeBaseCtx->GetTimeData().SetTimeLeapHealingRealCounter(0U);
                        LOG().Info() << timeBaseCtx->GetName().ToString() << " SetTimeLeapFuture,";
                    } else {
                        /// @traceid{SWS_TS_00028}
                        std::uint32_t counter{timeBaseCtx->GetTimeData().TimeLeapHealingRealCounter()};
                        counter = counter + 1U;
                        timeBaseCtx->GetTimeData().SetTimeLeapHealingRealCounter(counter);
                        std::uint32_t const configCounter{timeBaseCtx->GetTimeData().TimeLeapHealingCounter()};

                        if (counter >= configCounter) {
                            sc.status.SetTimeLeapNone();
                            timeBaseCtx->GetTimeData().SetTimeLeapHealingRealCounter(0U);
                            LOG().Verbose()
                                << timeBaseCtx->GetName().ToString() << ", sc.status.SetLeapJumpChanged true"
                                << ", IsTimeLeapNone=" << sc.status.IsTimeLeapNone()
                                << ", TimeLeapHealingCounter()=" << configCounter;
                        }
                    }
                }
            } else {
                /// @traceid{SWS_TS_00140}
                if (!isoft::util::IsEq0(timeDomainSlaveCfg->GetTimeLeapPastThreshold())) {
                    if (std::abs(leapJumpTs) > timeDomainSlaveCfg->GetTimeLeapPastThreshold()) {
                        sc.status.SetTimeLeapPast();
                        timeBaseCtx->GetTimeData().SetTimeLeapHealingRealCounter(0U);
                        LOG().Info() << timeBaseCtx->GetName().ToString() << " SetTimeLeapPast,";
                    } else {
                        /// @traceid{SWS_TS_00028}
                        std::uint32_t counter{timeBaseCtx->GetTimeData().TimeLeapHealingRealCounter()};
                        counter = counter + 1U;
                        timeBaseCtx->GetTimeData().SetTimeLeapHealingRealCounter(counter);
                        std::uint32_t const configCounter{timeBaseCtx->GetTimeData().TimeLeapHealingCounter()};

                        if (counter >= configCounter) {
                            sc.status.SetTimeLeapNone();
                            timeBaseCtx->GetTimeData().SetTimeLeapHealingRealCounter(0U);
                            LOG().Verbose()
                                << timeBaseCtx->GetName().ToString() << ", sc.status.SetLeapJumpChanged true"
                                << ", IsTimeLeapNone=" << sc.status.IsTimeLeapNone()
                                << ", TimeLeapHealingCounter()=" << configCounter;
                        }
                    }
                }
            }
            if ((true != sc.status.IsSameLeapJump(timeBaseCtx->GetTimeData().GetTimeStatus()))
                && (0U == timeBaseCtx->GetTimeData().TimeLeapHealingRealCounter())) {
                sc.SetLeapJumpChanged(true);
            }

            /// Simulate goto
        } while (false);

        /// Write the checked status to the time base
        timeBaseCtx->GetTimeData().SetOffset(timeDomainCtx->GetOffsetTS());
        timeBaseCtx->GetTimeData().SetUserData(timeDomainCtx->GetUserData());
        timeBaseCtx->GetTimeData().SetTimeStatus(sc.status);
        timeBaseCtx->SetTLSyncTime(
            Clock::time_point{std::chrono::nanoseconds{timeDomainCtx->GetTlSyncTime().ToChrono().count()}});
        timeBaseCtx->SetGlobalTime(
            Clock::time_point{std::chrono::nanoseconds{timeDomainCtx->GetGlobalTimeStamp().ToChrono().count()}});
        timeBaseCtx->SetVirtualLocalTime(
            static_cast< uint32_t >(timeDomainCtx->GetSteadyTimeStamp().ToChrono().count()));

        timeBaseCtx->SetPathDelay(static_cast< std::uint32_t >(timeDomainCtx->GetDelayTS().ToChrono().count()));

        if (timeDomainCtx->IsOffsetDomain()) {
            /// If current is offset time base, add the underlying referenced synchronization time domain data
            std::shared_ptr< timedomain::TDContext > const refTimeDomainCtx{
                timeDomainManager_->GetContext(timeBaseCtx->GetRefTimeData().DomainId())};
            if (nullptr != refTimeDomainCtx) {
                timeBaseCtx->GetRefTimeData().SetOffset(refTimeDomainCtx->GetOffsetTS());
                timeBaseCtx->GetRefTimeData().SetUserData(refTimeDomainCtx->GetUserData());
                timeBaseCtx->GetRefTimeData().SetTimeStatus(refTimeDomainCtx->GetStatus());
                Clock::time_point gTime{
                    std::chrono::nanoseconds{timeDomainCtx->GetOffsetTS().ToChrono().count()
                                             + refTimeDomainCtx->GetGlobalTime().time_since_epoch().count()}};
                timeBaseCtx->SetGlobalTime(std::move(gTime));
            }
        }
    }

    // Synchronization time base calculates rrc, saved in synchronization domain
    // Whether the jump exceeds the past/future threshold should only affect whether to notify the user, not as a condition for calculating Rrc
    if (timeDomainCtx->IsSyncDomain() && isSync) {
        /// If the ECU is configured only as a domain slave, take the received sync timestamp; slave's SyncEnTS is global time
        Clock::time_point synGlobalTime{std::chrono::nanoseconds{timeDomainCtx->GetSyncEnTS().ToChrono().count()
                                                                 + timeDomainCtx->GetCorrectionField()
                                                                 + timeDomainCtx->GetDelayTS().ToChrono().count()}};
        Clock::time_point syncLocalTime{std::chrono::nanoseconds{timeDomainCtx->GetSyncInTS().ToChrono().count()}};
        /// If the ECU is configured as master & slave, this ECU's time base consumer
        /// When calculating, take the sync transmission timestamp; master's SyncEnTS is local time, needs conversion
        if (timeDomainCfg->GetIsMasterOnThisMachine()) {
            std::chrono::nanoseconds const tp{timeDomainCtx->GetSyncEnTS().ToChrono().count()};
            synGlobalTime = Clock::time_point{
                std::chrono::nanoseconds{timeDomainCtx->Steady2GlobalTime(tp).time_since_epoch().count()}};
            syncLocalTime = Clock::time_point{tp};
        }
        if (0
            == timeBaseCtx->DoParaRCMeasurement(
                synGlobalTime, syncLocalTime,
                static_cast< std::size_t >(
                    timeBaseCfg->GetTimeSyncCorrection().GetRateCorrectionPerMeasurementDuration()),
                timeBaseCfg->GetTimeSyncCorrection().GetRateDeviationMeasurementDuration())) {
            Clock::time_point tmTVsync;
            std::chrono::nanoseconds syncJumpTs;
            Clock::time_point tmTLsync;
            Clock::time_point tmTG;
            double dbRrc{0.0F};
            timeBaseCtx->GetTimeCorrection().GetMeasurePara(tmTVsync, syncJumpTs, tmTLsync, tmTG, dbRrc);
            timeDomainCtx->SetMeasurePara(tmTVsync, syncJumpTs, tmTLsync, tmTG, dbRrc);
            timeBaseCtx->GetTimeData().SetRateDeviation(dbRrc - 1.0);
        }
    }
    // Offset time base takes relevant parameters from synchronization domain
    // Whether the jump exceeds the past/future threshold should only affect whether to notify the user, not as a condition for calculating Rrc
    if (timeDomainCtx->IsOffsetDomain() && isSync) {
        std::shared_ptr< timedomain::TDContext > const refTimeDomainCtx{
            timeDomainManager_->GetContext(timeBaseCtx->GetRefTimeData().DomainId())};
        if (nullptr != refTimeDomainCtx) {
            Clock::time_point tmTVsync;
            std::chrono::nanoseconds syncJumpTs;
            Clock::time_point tmTLsync;
            Clock::time_point tmTG;
            double dbRrc{0.0};
            refTimeDomainCtx->GetMeasurePara(tmTVsync, syncJumpTs, tmTLsync, tmTG, dbRrc);
            timeBaseCtx->SetMeasurePara(tmTVsync, syncJumpTs, tmTLsync, tmTG, dbRrc);
            timeBaseCtx->GetTimeData().SetRateDeviation(dbRrc - 1.0);
            timeBaseCtx->GetRefTimeData().SetRateDeviation(dbRrc - 1.0);
        }
    }

    return sc;
}

/// @brief notify time base manager that an event has occurred on a time domain
/// @param domainId - time domain ID
/// @param event - event type
void TBManager::Notify(internal::TimeDomainId const &domainId, EventType const &event) noexcept
{
    /// 1. Find all time bases based on domainId
    /// 2. Check status changes of all relevant time bases, notify proxy
    std::shared_ptr< timedomain::TDContext > const domainCtx{timeDomainManager_->GetContext(domainId)};
    if (nullptr == domainCtx) {
        LOG().Error() << "Notify domainCtx nullptr";
        return;
    }
    if (nullptr == proxySkeleton_) {
        LOG().Error() << "Notify proxySkeleton_ nullptr";
        return;
    }
    std::function< void(timebase::resource::TimeBaseId const &tbId) > const updateMethod{
        [this, domainCtx, event, domainId](timebase::resource::TimeBaseId const &tbId) noexcept -> void {
            timebase::resource::TBContext *const ctx{this->resourceMan_->GetContext(tbId)};
            if (nullptr == ctx) {
                return;
            }
            StatusChecker sc;
            switch (event) {
                case EventType::kTimeSyncFinished:
                case EventType::kTimeSyncTimeout: {
                    /// If it is a time base provider, no need to check time base and notify status changes, nor time precision measurement notification
                    if (ctx->IsProvider()) {
                        return;
                    }
                    sc = _updateTimeBaseResource(tbId, domainId, domainCtx->IsSyncDomain());
                    if (domainCtx->GetDomainId().ToUint8() == tbId.GetDomainId().ToUint8()) {
                        if (sc.IsLeapJumpChanged()) {
                            static_cast< void >(this->proxySkeleton_->NotifyLeapJump(tbId));
                        }
                        if (sc.IsSynchronizationStatusChanged()) {
                            static_cast< void >(this->proxySkeleton_->NotifySynchronizationStateChanged(tbId));
                        }
                        if (sc.IsStatusChanged()) {
                            static_cast< void >(this->proxySkeleton_->NotifyStatusChanged(tbId));
                        }
                        static_cast< void >(this->proxySkeleton_->NotifyPrecisionMeasurement(tbId));
                    }
                    break;
                }
                case EventType::kTimeValidationSetPdelayInitiatorData: {
                    /// Copy required data
                    tsync::PdelayInitiatorMeasurementType data;
                    data.requestOriginTimestamp
                        = static_cast< std::uint64_t >(domainCtx->GetPdelayReqEnTS().ToChrono().count());
                    data.responseReceiptTimestamp
                        = static_cast< std::uint64_t >(domainCtx->GetPdelayRespInTS().ToChrono().count());
                    // in GlobalTime
                    data.requestReceiptTimestamp = Timestamp(domainCtx->GetPdelayReqInTS().ToChrono());
                    // in GlobalTime
                    data.responseOriginTimestamp = Timestamp(domainCtx->GetPdelayRespEnTS().ToChrono());
                    std::chrono::nanoseconds const tp{domainCtx->GetGlobalTime().time_since_epoch().count()};

                    data.referenceGlobalTimestamp = Timestamp(tp);
                    data.referenceLocalTimestamp
                        = static_cast< std::uint64_t >(Clock::now().time_since_epoch().count());
                    data.pDelay     = static_cast< std::uint32_t >(domainCtx->GetDelayTS().ToChrono().count());
                    data.sequenceId = domainCtx->GetPdelaySeqId();
                    ctx->SetPdelayMeasurementData(data);
                    static_cast< void >(this->proxySkeleton_->NotifyValidationMeasurementSetPdelayInitiatorData(tbId));
                    break;
                }
                case EventType::kTimeValidationSetPdelayResponderData: {
                    tsync::PdelayResponderMeasurementType data;
                    data.requestReceiptTimestamp
                        = static_cast< std::uint64_t >(domainCtx->GetPdelayReqInTS().ToChrono().count());
                    data.responseOriginTimestamp
                        = static_cast< std::uint64_t >(domainCtx->GetPdelayRespEnTS().ToChrono().count());
                    data.referenceLocalTimestamp
                        = static_cast< std::uint64_t >(domainCtx->GetGlobalTime().time_since_epoch().count());
                    std::chrono::nanoseconds const tp{domainCtx->GetGlobalTime().time_since_epoch().count()};
                    data.referenceGlobalTimestamp = Timestamp(tp);
                    data.sequenceId               = domainCtx->GetPdelaySeqId();
                    ctx->SetPdelayResponderData(data);
                    static_cast< void >(this->proxySkeleton_->NotifyValidationMeasurementSetPdelayResponderData(tbId));
                    break;
                }
                case EventType::kTimeValidationSetSlaveTimingData: {
                    tsync::TimeSlaveMeasurementType data;
                    data.preciseOriginTimestamp = Timestamp((domainCtx->GetSyncEnTS().ToChrono()));
                    /// TODO(person in charge): Study what timestamp this marks.
                    std::chrono::nanoseconds const tp{domainCtx->GetGlobalTime().time_since_epoch().count()};
                    data.referenceGlobalTimestamp = Timestamp(tp);
                    data.referenceLocalTimestamp
                        = static_cast< std::uint64_t >(Clock::now().time_since_epoch().count());
                    data.syncIngressTimestamp
                        = static_cast< std::uint64_t >(domainCtx->GetSyncInTS().ToChrono().count());
                    data.correctionField = static_cast< std::int64_t >(domainCtx->GetCorrectionField());
                    data.pDelay          = static_cast< std::uint32_t >(domainCtx->GetDelayTS().ToChrono().count());
                    data.sequenceId      = domainCtx->GetSyncSeqId();
                    ctx->SetTimeSlaveMeasurementData(data);
                    static_cast< void >(this->proxySkeleton_->NotifyValidationMeasurementSetSlaveTimingData(tbId));
                    break;
                }
                case EventType::kTimeValidationSetMasterTimingData: {
                    tsync::TimeMasterMeasurementType data;
                    std::chrono::nanoseconds syncEnSteady{std::move(domainCtx->GetSyncEnTS().ToChrono())};
                    data.preciseOriginTimestamp
                        = Timestamp(domainCtx->Steady2GlobalTime(std::move(syncEnSteady)).time_since_epoch());
                    data.syncEgressTimestamp
                        = static_cast< std::uint64_t >(domainCtx->GetSyncEnTS().ToChrono().count());
                    data.sequenceId = domainCtx->GetSyncSeqId();
                    ctx->SetTimeMasterMeasurementData(data);
                    static_cast< void >(this->proxySkeleton_->NotifyValidationMeasurementSetMasterTimingData(tbId));
                    break;
                }
                case EventType::kSetTime: {
                    break;
                }
            }
        }};

    /// First update synchronization time base, then update offset time base,
    /// Offset time base cannot perform rrc calculation; it takes related data from synchronization time base
    ara::core::Vector< timebase::resource::TimeBaseId > const tbIds{resourceMan_->GetTimeBaseIds(domainId)};
    for (ara::core::Vector< timebase::resource::TimeBaseId >::const_iterator itm{tbIds.cbegin()}; itm != tbIds.cend();
         ++itm) {
        updateMethod(*itm);
    }
    if ((EventType::kTimeSyncFinished == event) || (EventType::kTimeSyncTimeout == event)) {
        /// Synchronization corresponding offset time base does not need kTimeValidationSetPdelayInitiatorData
        /// kTimeValidationSetPdelayResponderData
        ara::core::Vector< timebase::resource::TimeBaseId > const offsetTbIds{
            resourceMan_->GetOffsetTimeBaseIds(domainId)};
        for (ara::core::Vector< timebase::resource::TimeBaseId >::const_iterator itm{offsetTbIds.cbegin()};
             itm != offsetTbIds.cend(); ++itm) {
            updateMethod(*itm);
        }
    }
}

/// @brief initialize storage related, get storage time, periodic storage
/// @param getStorageTimeCb - get time callback
/// @param setStorageTimeCb - store time callback
/// @return 0 - success
/// @return <0 - failure
void TBManager::InitStorage(GetStorageTimeHandler const &getStorageTimeCb,
                            SetStorageTimeHandler const &setStorageTimeCb) noexcept
{
    if ((nullptr == getStorageTimeCb) || (nullptr == getStorageTimeCb)) {
        LOG().Error() << "TBManager::InitStorage nullptr";
        return;
    }
    getStorageTimeCb_ = getStorageTimeCb;
    setStorageTimeCb_ = setStorageTimeCb;
    std::ignore       = _loadTimeFromStorage();
    std::ignore       = _createCyclicBackupTimer();
}
/// @brief initialize
/// @param pConfig configuration manager
/// @param timeDomain time domain manager
/// @return 0 - success
/// @return <0 - failure
std::int32_t TBManager::_init(std::shared_ptr< config::ConfigManager > const &pConfig,
                              std::shared_ptr< timedomain::TDManager > const &timeDomain) noexcept
{
    configManager_     = pConfig;
    timeDomainManager_ = timeDomain;

    /// Create time base resource
    resourceMan_ = resource::TBResManager::CreateManager(configManager_);
    if (nullptr == resourceMan_) {
        return kRET_E1;
    }

    /// Create time base proxy skeleton
    proxySkeleton_ = proxy::Skeleton::CreateSkeleton(resourceMan_, configManager_, timeDomainManager_);
    if (nullptr == proxySkeleton_) {
        return kRET_E1;
    }
    proxySkeleton_->OnEvent(
        [this](timebase::resource::TimeBaseId const tbId, proxy::ProxyEventType const event) noexcept -> void {
            _proxySkeletonEventHandler(tbId, event);
        });
    return 0;
}

/// @brief time base proxy skeleton event callback function
/// @param tbId - time base resource context
/// @param event - event
void TBManager::_proxySkeletonEventHandler(timebase::resource::TimeBaseId const tbId,
                                           proxy::ProxyEventType const event) noexcept
{
    timebase::resource::TBContext *timeBaseCtx{nullptr};

    if (nullptr == resourceMan_) {
        return;
    }
    if (nullptr == timeDomainManager_) {
        return;
    }
    timeBaseCtx = resourceMan_->GetContext(tbId);
    if (nullptr == timeBaseCtx) {
        return;
    }
    std::shared_ptr< timedomain::TDContext > const timeDomainCtx{
        timeDomainManager_->GetContext(timeBaseCtx->GetTimeData().DomainId())};

    if (nullptr == timeDomainCtx) {
        return;
    }

    if (!timeBaseCtx->IsProvider()) {
        return;
    }

    if ((proxy::ProxyEventType::kSetTime == event) || (proxy::ProxyEventType::kUpdateTime == event)
        || (proxy::ProxyEventType::kSetUserData == event) || (proxy::ProxyEventType::kSetRateDeviation == event)) {
        /// Update data from time base to time domain
        timeDomainCtx->SetOffsetTS(timeBaseCtx->GetTimeData().Offset());
        timeDomainCtx->SetUserData(timeBaseCtx->GetTimeData().GetUserData());
        timeDomainCtx->SetRateDeviation(timeBaseCtx->GetTimeData().RateDeviation());
        if (proxy::ProxyEventType::kSetTime == event) {
            if (timeBaseCtx->GetId().IsSyncTimeBase()) {
                timeDomainCtx->SetGlobalTimeStamp(timeBaseCtx->LastGlobalTimestamp(),
                                                  timeBaseCtx->LastSteadyTimestamp());
                _updateOffset(tbId);
            }
            if (nullptr != setTimeCb_) {
                setTimeCb_(timeBaseCtx->GetTimeData().DomainId());
            }
        }
        /// Setting rate requires ensuring that the synchronization and offset time base time domain rates are consistent
        bool const isSync{timeBaseCtx->GetId().IsSyncTimeBase()};
        if ((proxy::ProxyEventType::kSetRateDeviation == event) && (isSync)) {
            /// Configure stable clock; currently do not consider related offset time bases
            if (timeBaseCtx->IsSteadyClock()) {
                timeDomainCtx->SetGlobalTimeStamp(timeBaseCtx->LastGlobalTimestamp(),
                                                  timeBaseCtx->LastSteadyTimestamp());
            }
            _updateOffset(tbId);
        } else if (proxy::ProxyEventType::kSetRateDeviation == event) {
            if (timeBaseCtx->GetId().IsOffsetTimeBase()) {
                _updateSync(tbId);
            }
        } else {
        }
    }
}

/// @brief recover time from persistent storage during initialization
/// @return 0 - success
/// @return <0 - failure
std::int32_t TBManager::_loadTimeFromStorage() noexcept
{
    /// @traceid{SWS_TS_00212}
    /// Time persistence initialization, open storage, read time, set domain and time base
    if (nullptr != getStorageTimeCb_) {
        ara::core::Vector< internal::config::TimeBase > const &timeBaseCfgs{configManager_->GetTimeBaseSet()->GetAll()};
        std::size_t const tbNumber{timeBaseCfgs.size()};
        for (std::size_t tbIndex{0U}; tbIndex < tbNumber; tbIndex++) {
            internal::config::TimeBase const &cfg{timeBaseCfgs[tbIndex]};
            internal::TimeDomainId const timeDomainId{cfg.GetTimeDomainId()};
            config::TimeDomainSet::Domain const *const timeDomainCfg{
                configManager_->GetTimeDomainSet()->GetDomain(timeDomainId)};
            if (timeDomainCfg->GetIsMasterOnThisMachine() && cfg.IsProvider()) {
                timebase::resource::TBContext *const timeBaseCtx{resourceMan_->GetContext(cfg.GetName())};
                std::shared_ptr< timedomain::TDContext > const timeDomainCtx{
                    timeDomainManager_->GetContext(timeDomainId)};
                if (!cfg.GetStorageInstanceSpecifier().empty()) {
                    ara::core::String userData;
                    userData.clear();  // just for qac
                    bool bGet{false};
                    std::uint64_t lastTimeVal{0U};
                    double rateDeviation{0};
                    ara::core::String const &timebaseName{cfg.GetName()};
                    if (cfg.GetStorageFile().empty()) {
                        /// kv storage
                        ara::core::StringView const speciStrView{cfg.GetStorageInstanceSpecifier().c_str()};
                        ara::core::StringView const keyStrView{cfg.GetStorageKVKey().c_str()};
                        bGet = getStorageTimeCb_(timebaseName, speciStrView, keyStrView, "", userData, rateDeviation,
                                                 lastTimeVal);
                        kvVector_.push_back(cfg.GetName());
                    } else {
                        /// file storage
                        ara::core::StringView const speciStrView{cfg.GetStorageInstanceSpecifier().c_str()};
                        ara::core::StringView const fileStrView{cfg.GetStorageFile().c_str()};
                        bGet = getStorageTimeCb_(timebaseName, speciStrView, "", fileStrView, userData, rateDeviation,
                                                 lastTimeVal);
                        fileVector_.push_back(cfg.GetName());
                    }
                    if (bGet) {
                        timeBaseCtx->GetTimeData().SetRateDeviation(rateDeviation);
                        internal::UserData data;
                        data.FromString(userData);
                        timeBaseCtx->GetTimeData().SetUserData(data);
                        timeDomainCtx->SetAllowDispatch(true);
                        /// After restart, set stable time to current time
                        std::uint64_t const lastSteadyNano{
                            static_cast< std::uint64_t >(Clock::now().time_since_epoch().count())};
                        LOG().Verbose() << "_loadTimeFromStorage timeDomainId=" << timeDomainId.ToUint8()
                                        << ", lastTimeVal=" << lastTimeVal;
                        // ara::tsync::Timestamp
                        // ts{std::chrono::system_clock::now().time_since_epoch()};.
                        if (timeDomainId.IsSyncDomain()) {
                            internal::TimeValue timePoint;
                            timePoint.FromChrono(std::chrono::nanoseconds(lastTimeVal));
                            timeBaseCtx->SetLastGlobalTimestamp(timePoint);
                            timePoint.FromChrono(std::chrono::nanoseconds(lastSteadyNano));
                            timeBaseCtx->SetLastSteadyTimestamp(timePoint);
                            timeBaseCtx->GetTimeData().SetOffset(
                                internal::TimeValue(std::chrono::nanoseconds{lastTimeVal - lastSteadyNano}));
                            resourceMan_->UpdateOffsetTimeBase(timeBaseCtx->GetId());
                            timeDomainCtx->SetGlobalTimeStamp(timeBaseCtx->LastGlobalTimestamp(),
                                                              timeBaseCtx->LastSteadyTimestamp());
                            _updateOffset(timeBaseCtx->GetId());
                        } else if (timeDomainId.IsOffsetDomain()) {
                            timeBaseCtx->GetTimeData().SetOffset(
                                internal::TimeValue(std::chrono::nanoseconds{lastTimeVal}));
                        } else {
                        }
                        /// Update data from time base to time domain
                        timeDomainCtx->SetOffsetTS(timeBaseCtx->GetTimeData().Offset());
                        timeDomainCtx->SetUserData(timeBaseCtx->GetTimeData().GetUserData());
                        timeDomainCtx->SetRateDeviation(timeBaseCtx->GetTimeData().RateDeviation());
                        /// @traceid{SWS_TS_00213}
                        /// R2311 time persistence: after initialization read, persist immediately old + interval, only supports KV
                        if ((cfg.GetStorageFile().empty()) && (cfg.GetCyclicBackupInterval() > 0)) {
                            std::uint64_t storeTime{
                                lastTimeVal
                                + static_cast< std::uint64_t >(isoft::util::TimeS2NS(cfg.GetCyclicBackupInterval()))};
                            LOG().Info() << "_loadTimeFromStorage StoreTime timebaseName=" << cfg.GetName()
                                         << ", storeTime=" << storeTime;
                            ara::core::StringView keyStrView{cfg.GetStorageKVKey().c_str()};
                            setStorageTimeCb_(StorageType::kStorageKv, timebaseName, keyStrView, userData,
                                              rateDeviation, storeTime);
                        }
                    }
                } else {
                    /// No persistence configured, initialize synchronization time base to system time
                    if (timeDomainId.IsSyncDomain()) {
                        if (timeDomainCfg->IsSteadyClock()) {
                            internal::TimeValue timePoint;
                            internal::TimeValue timePoint1;
                            timePoint.FromChrono(Clock::now().time_since_epoch());
                            timePoint1.FromChrono(std::chrono::nanoseconds{0});  /// test AAAAA
                            timeBaseCtx->SetLastGlobalTimestamp(timePoint1);
                            timeBaseCtx->SetLastSteadyTimestamp(timePoint);
                            std::chrono::nanoseconds timeStamep{timeBaseCtx->LastGlobalTimestamp().ToChrono().count()};
                            timeStamep = timeStamep - timeBaseCtx->LastSteadyTimestamp().ToChrono();
                            timeBaseCtx->GetTimeData().SetOffset(internal::TimeValue(timeStamep));
                            LOG().Info() << "_loadTimeFromStorage init, SteadyClock, SyncDomain="
                                         << timeDomainId.ToUint8()
                                         << ", offset=" << timeBaseCtx->GetTimeData().Offset().GetSecond();
                            timeDomainCtx->SetOffsetTS(timeBaseCtx->GetTimeData().Offset());
                            timeDomainCtx->SetGlobalTimeStamp(timeBaseCtx->LastGlobalTimestamp(),
                                                              timeBaseCtx->LastSteadyTimestamp());
                            resourceMan_->UpdateOffsetTimeBase(timeBaseCtx->GetId());
                        } else {
                            internal::TimeValue timePoint;
                            timePoint.FromChrono(std::chrono::system_clock::now().time_since_epoch());
                            timeBaseCtx->SetLastGlobalTimestamp(timePoint);
                            timePoint.FromChrono(Clock::now().time_since_epoch());
                            timeBaseCtx->SetLastSteadyTimestamp(timePoint);
                            std::chrono::nanoseconds timeStamep{timeBaseCtx->LastGlobalTimestamp().ToChrono().count()};
                            timeStamep = timeStamep - timeBaseCtx->LastSteadyTimestamp().ToChrono();
                            timeBaseCtx->GetTimeData().SetOffset(internal::TimeValue(timeStamep));
                            LOG().Info() << "_loadTimeFromStorage init, SyncDomain=" << timeDomainId.ToUint8()
                                         << ", offset=" << timeBaseCtx->GetTimeData().Offset().GetSecond();
                            timeDomainCtx->SetOffsetTS(timeBaseCtx->GetTimeData().Offset());
                            timeDomainCtx->SetGlobalTimeStamp(timeBaseCtx->LastGlobalTimestamp(),
                                                              timeBaseCtx->LastSteadyTimestamp());
                            resourceMan_->UpdateOffsetTimeBase(timeBaseCtx->GetId());
                        }
                    }
                }
            }
        }
    }
    return 0;
}

/// @brief store time value to persistent storage when process exits
/// @return 0 - success
/// @return <0 - failure
std::int32_t TBManager::StoreTimeToStorage() noexcept
{
    /// @traceid{SWS_TS_00212}
    /// Time persistence: when process exits, store time value to persistent storage
    if (nullptr != setStorageTimeCb_) {
        for (Storageiterator it{kvVector_.begin()}; kvVector_.end() != it; ++it) {
            internal::config::TimeBase const *const pCfg{configManager_->GetTimeBaseSet()->GetTimeBase(*it)};
            internal::TimeDomainId const timeDomainId{pCfg->GetTimeDomainId()};
            std::shared_ptr< timedomain::TDContext > timeDomainCtx{timeDomainManager_->GetContext(timeDomainId)};
            timebase::resource::TBContext *const timeBaseCtx{resourceMan_->GetContext(pCfg->GetName())};
            /// When persisting, synchronization time domain stores global-time, offset time domain stores offset
            ara::core::StringView timebaseStrView{pCfg->GetName().c_str()};
            std::uint64_t lastTimeVal{0U};
            if (timeDomainCtx->IsSyncDomain()) {
                lastTimeVal = static_cast< std::uint64_t >(timeDomainCtx->GetGlobalTime().time_since_epoch().count());
            } else {
                lastTimeVal = static_cast< std::uint64_t >(timeBaseCtx->GetTimeData().Offset().ToChrono().count());
            }
            ara::core::StringView keyStrView{pCfg->GetStorageKVKey().c_str()};
            ara::core::String const userData{timeBaseCtx->GetTimeData().GetUserData().ToString()};
            double const rateDeviation{timeBaseCtx->GetTimeData().RateDeviation()};
            setStorageTimeCb_(StorageType::kStorageKv, pCfg->GetName(), std::move(keyStrView), userData, rateDeviation,
                              lastTimeVal);
        }
        for (Storageiterator it{fileVector_.begin()}; fileVector_.end() != it; ++it) {
            internal::config::TimeBase const *const pCfg{configManager_->GetTimeBaseSet()->GetTimeBase(*it)};
            internal::TimeDomainId const timeDomainId{pCfg->GetTimeDomainId()};
            std::shared_ptr< timedomain::TDContext > timeDomainCtx{timeDomainManager_->GetContext(timeDomainId)};
            timebase::resource::TBContext *const timeBaseCtx{resourceMan_->GetContext(pCfg->GetName())};
            /// When persisting, synchronization time domain stores global-time, offset time domain stores offset
            std::uint64_t lastTimeVal{0U};
            if (timeDomainCtx->IsSyncDomain()) {
                lastTimeVal = static_cast< std::uint64_t >(timeDomainCtx->GetGlobalTime().time_since_epoch().count());
            } else {
                lastTimeVal = static_cast< std::uint64_t >(timeBaseCtx->GetTimeData().Offset().ToChrono().count());
            }
            ara::core::String const userData{timeBaseCtx->GetTimeData().GetUserData().ToString()};
            double const rateDeviation{timeBaseCtx->GetTimeData().RateDeviation()};
            setStorageTimeCb_(StorageType::kStorageFile, pCfg->GetName(), "", userData, rateDeviation, lastTimeVal);
        }
    }

    return 0;
}

/// @brief when setting rate offset and time for synchronization time base, need to update the corresponding offset time base and time domain
/// @param tbId - time base resource context
void TBManager::_updateOffset(timebase::resource::TimeBaseId const &tbId) noexcept
{
    timebase::resource::TBContext *timeBaseCtx{nullptr};

    if (nullptr == resourceMan_) {
        return;
    }
    if (nullptr == timeDomainManager_) {
        return;
    }
    timeBaseCtx = resourceMan_->GetContext(tbId);
    if (nullptr == timeBaseCtx) {
        return;
    }

    if (!timeBaseCtx->IsProvider()) {
        return;
    }
    resourceMan_->UpdateOffsetTimeBase(tbId);
    std::shared_ptr< internal::config::TimeDomainSet const > timeDomainSetCfg{configManager_->GetTimeDomainSet()};
    for (internal::TimeDomainId domainId{0U}; true == domainId.IsValid(); ++domainId) {
        config::TimeDomainSet::Domain const *const offsetTimeDomainCfg{timeDomainManager_->GetConfig(domainId)};
        std::shared_ptr< timedomain::TDContext > const offsetTimeDomainCtx{timeDomainManager_->GetContext(domainId)};
        if (nullptr == offsetTimeDomainCfg) {
            continue;
        }
        if (nullptr == offsetTimeDomainCtx) {
            continue;
        }
        bool const matchDomainId{timeDomainSetCfg->GetSyncDomainId(domainId) == timeBaseCtx->GetTimeData().DomainId()};
        if (domainId.IsOffsetDomain() && offsetTimeDomainCfg->GetIsMasterOnThisMachine() && (matchDomainId)) {
            offsetTimeDomainCtx->SetRateDeviation(timeBaseCtx->GetTimeData().RateDeviation());
            offsetTimeDomainCtx->SetGlobalTimeStamp(timeBaseCtx->LastGlobalTimestamp(),
                                                    timeBaseCtx->LastSteadyTimestamp());
            break;
        }
    }
}

/// @brief when setting rate offset for offset time base, need to update the corresponding synchronization time base and time domain
/// @param tbId - time base resource context
void TBManager::_updateSync(timebase::resource::TimeBaseId const &tbId) noexcept
{
    timebase::resource::TBContext *timeBaseCtx{nullptr};

    if (nullptr == resourceMan_) {
        return;
    }
    if (nullptr == timeDomainManager_) {
        return;
    }
    timeBaseCtx = resourceMan_->GetContext(tbId);
    if (nullptr == timeBaseCtx) {
        return;
    }

    if (!timeBaseCtx->IsProvider()) {
        return;
    }
    resourceMan_->UpdateSyncTimeBase(tbId);
    std::shared_ptr< timedomain::TDContext > syncTimeDomainCtx{
        timeDomainManager_->GetContext(timeBaseCtx->GetRefTimeData().DomainId())};
    if (nullptr != syncTimeDomainCtx) {
        syncTimeDomainCtx->SetRateDeviation(timeBaseCtx->GetTimeData().RateDeviation());
    }
}

/// @brief create periodic persistence timer
/// @return 0 - success
/// @return <0 - failure
std::int32_t TBManager::_createCyclicBackupTimer() noexcept
{
    /// @traceid{SWS_TS_00212}
    /// Time persistence initialization, open storage, read time, set domain and time base
    if (nullptr != setStorageTimeCb_) {
        ara::core::Vector< internal::config::TimeBase > const &timeBaseCfgs{configManager_->GetTimeBaseSet()->GetAll()};
        std::size_t const tbNumber{timeBaseCfgs.size()};
        for (std::size_t tbIndex{0U}; tbIndex < tbNumber; tbIndex++) {
            internal::config::TimeBase const &cfg{timeBaseCfgs[tbIndex]};
            internal::TimeDomainId const timeDomainId{cfg.GetTimeDomainId()};
            config::TimeDomainSet::Domain const *const timeDomainCfg{
                configManager_->GetTimeDomainSet()->GetDomain(timeDomainId)};
            if (timeDomainCfg->GetIsMasterOnThisMachine() && cfg.IsProvider()) {
                std::shared_ptr< timedomain::TDContext > const timeDomainCtx{
                    timeDomainManager_->GetContext(timeDomainId)};
                if (!cfg.GetStorageInstanceSpecifier().empty()) {
                    double interval{cfg.GetCyclicBackupInterval()};
                    if (interval > 0) {
                        isoft::naicpp::EvLoop::TimerPtr timebaseTimer{};
                        static_cast< void >(isoft::naicpp::GlobalGeneralEvLoop::Get()->MakeTimer(
                            timebaseTimer, (static_cast< std::int32_t >(isoft::util::TimeS2MS(interval))),
                            [this, cfg, interval]() -> void {
                                this->_doCyclicBackup(cfg.GetName());
                                this->storageKVTimerMap_[cfg.GetName()]->UpdateTime(
                                    (static_cast< std::int32_t >(isoft::util::TimeS2MS(interval))));
                            }));
                        storageKVTimerMap_[cfg.GetName()] = timebaseTimer;
                    }
                }
            }
        }
    }
    return 0;
}
/// @brief perform periodic persistence
/// @return 0 - success
/// @return <0 - failure
std::int32_t TBManager::_doCyclicBackup(ara::core::String const &timebaseName) noexcept
{
    /// @traceid{SWS_TS_00212}
    /// Time persistence periodic backup

    internal::config::TimeBase const *const pCfg{configManager_->GetTimeBaseSet()->GetTimeBase(timebaseName)};
    internal::TimeDomainId const timeDomainId{pCfg->GetTimeDomainId()};
    std::shared_ptr< timedomain::TDContext > timeDomainCtx{timeDomainManager_->GetContext(timeDomainId)};
    timebase::resource::TBContext *const timeBaseCtx{resourceMan_->GetContext(pCfg->GetName())};
    /// When persisting, synchronization time domain stores global-time, offset time domain stores offset
    std::uint64_t storeTime{static_cast< std::uint64_t >(isoft::util::TimeS2NS(pCfg->GetCyclicBackupInterval()))};
    if (timeDomainCtx->IsSyncDomain()) {
        storeTime += static_cast< std::uint64_t >(timeDomainCtx->GetGlobalTime().time_since_epoch().count());
    } else {
        storeTime += static_cast< std::uint64_t >(timeBaseCtx->GetTimeData().Offset().ToChrono().count());
    }
    ara::core::StringView keyStrView{pCfg->GetStorageKVKey().c_str()};
    ara::core::String const userData{timeBaseCtx->GetTimeData().GetUserData().ToString()};
    double const rateDeviation{timeBaseCtx->GetTimeData().RateDeviation()};
    LOG().Info() << "_doCyclicBackup timebaseName=" << timebaseName << ", storeTime=" << storeTime;
    setStorageTimeCb_(StorageType::kStorageKv, timebaseName, std::move(keyStrView), userData, rateDeviation, storeTime);
    return 0;
}

}  // namespace timebase
}  // namespace internal
}  // namespace tsync
}  // namespace ara