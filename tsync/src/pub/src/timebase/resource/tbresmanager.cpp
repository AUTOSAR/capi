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
/// @file       tbresmanager.cpp
/// @brief      time base resource management class
/// @details
/// @date       2023-02-01
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#include "ara/tsync/internal/timebase/resource/tbresmanager.h"

#include <cstdint>

#include "ara/tsync/internal/common.h"
#include "ara/tsync/internal/log/logger.h"

namespace ara {
namespace tsync {
namespace internal {
namespace timebase {
namespace resource {

/// @brief log output
/// @return Logger object reference
static inline ara::tsync::internal::tslog::Logger &LOG() noexcept { return ara::tsync::internal::tslog::Log(); }

/// @brief create time base resource manager based on configuration
/// @param configMan - time base configuration set
/// @return time base resource manager handle
std::shared_ptr< TBResManager > TBResManager::CreateManager(
    std::shared_ptr< config::ConfigManager const > const &configMan) noexcept
{
    std::shared_ptr< TBResManager > man{nullptr};
    if (nullptr == configMan) {
        return man;
    }
    man = std::make_shared< TBResManager >();
    if (nullptr == man) {
        return man;
    }
    if (0 != man->_create(configMan)) {
        man = nullptr;
    }
    return man;
}

/// @brief open an existing time base resource manager
/// @return time base resource manager handle
std::shared_ptr< TBResManager > TBResManager::OpenManager() noexcept
{
    std::shared_ptr< TBResManager > man{std::make_shared< TBResManager >()};
    if (nullptr == man) {
        return man;
    }
    if (0 != man->_open()) {
        man = nullptr;
    }
    return man;
}

/// @brief destructor
TBResManager::~TBResManager() noexcept
{
    if (nullptr == timeBaseConfigSet_) {
        static_cast< void >(_close());
    } else {
        static_cast< void >(_destroy());
    }
    configMan_         = nullptr;
    timeBaseConfigSet_ = nullptr;
}

/// @brief get time base resource context based on time base name
/// @param tbName - time base name
/// @return time base resource context
TBContext *TBResManager::GetContext(ara::core::String const &tbName) const noexcept
{
    std::size_t const tbNumber{static_cast< std::size_t >(shm_.Size()) / sizeof(TBContext)};
    if (nullptr == contextSet_) {
        return nullptr;
    }
    resource::Name name{};
    name.FromString(tbName);
    for (std::size_t tbIndex{0U}; tbIndex < tbNumber; tbIndex++) {
        if (contextSet_[tbIndex].GetName() == name) {
            return contextSet_ + tbIndex;
        }
    }

    return nullptr;
}

/// @brief get the list of offset time base IDs associated with the specified time domain ID
/// @param domainId - time domain ID
/// @return list of time base IDs
ara::core::Vector< TimeBaseId > TBResManager::GetOffsetTimeBaseIds(
    internal::TimeDomainId const &domainId) const noexcept
{
    ara::core::Vector< TimeBaseId > tbIds;
    std::size_t const tbNumber{static_cast< std::size_t >(shm_.Size()) / sizeof(TBContext)};
    if (nullptr == contextSet_) {
        return tbIds;
    }

    for (std::size_t tbIndex{0U}; tbIndex < tbNumber; tbIndex++) {
        if (domainId == contextSet_[tbIndex].GetRefTimeData().DomainId()) {
            tbIds.emplace_back(contextSet_[tbIndex].GetId());
        }
    }
    return tbIds;
}

/// @brief get the list of time base IDs associated with the specified time domain ID
/// @param domainId - time domain ID
/// @return list of time base IDs
ara::core::Vector< TimeBaseId > TBResManager::GetTimeBaseIds(internal::TimeDomainId const &domainId) const noexcept
{
    ara::core::Vector< TimeBaseId > tbIds;
    std::size_t const tbNumber{static_cast< std::size_t >(shm_.Size()) / sizeof(TBContext)};
    if (nullptr == contextSet_) {
        return tbIds;
    }

    for (std::size_t tbIndex{0U}; tbIndex < tbNumber; tbIndex++) {
        if (domainId == contextSet_[tbIndex].GetTimeData().DomainId()) {
            tbIds.emplace_back(contextSet_[tbIndex].GetId());
        }
    }
    return tbIds;
}
/// @brief create and open time base resource manager, then other processes can connect and use it
/// @param configMan - time base configuration set
/// @return 0 - success
/// @return <0 - failure
std::int32_t TBResManager::_create(std::shared_ptr< config::ConfigManager const > const &configMan) noexcept
{
    if (nullptr == configMan) {
        return kRET_E1;
    }
    configMan_ = configMan;
    _makeSharedMemPath();
    timeBaseConfigSet_ = configMan->GetTimeBaseSet();

    if (nullptr == timeBaseConfigSet_) {
        return kRET_E2;
    }
    ara::core::Vector< internal::config::TimeBase > const &timeBaseCfgs{timeBaseConfigSet_->GetAll()};
    std::size_t const tbNumber{timeBaseCfgs.size()};
    std::size_t const shmSize{tbNumber * sizeof(TBContext)};
    std::int32_t retCode{0};
    std::shared_ptr< internal::config::TimeDomainSet const > timeDomainSetCfg{configMan_->GetTimeDomainSet()};

    /// instead of goto
    while (true) {
        if (0 != shm_.Create(shareMemPath_.c_str(), shmSize)) {
            LOG().Fatal() << "shm_.Create(" << shareMemPath_ << "," << shmSize << "): " << GetErrString();
            retCode = kRET_E2;
            break;
        }
        contextSet_ = reinterpret_cast< TBContext * >(shm_.Get());

        /// Initialize Context
        for (std::size_t tbIndex{0U}; tbIndex < tbNumber; tbIndex++) {
            TBContext *const ctx{contextSet_ + tbIndex};
            internal::config::TimeBase const &cfg{timeBaseCfgs[tbIndex]};
            internal::TimeDomainId const timeDomainId{cfg.GetTimeDomainId()};

            /// Under default configuration, if machine2 has no association with machine1, consumer does not generate a "timeSyncCorrection" configuration.
            /// Adaptive configuration: if time base does not have one, use the time domain's.
            double adaptionInterval{timeDomainSetCfg->GetDomain(timeDomainId)
                                        ->GetGlobalTimeCorrectionProps()
                                        .GetOffsetCorrectionAdaptionInterval()};
            double jumpThreshold{timeDomainSetCfg->GetDomain(timeDomainId)
                                     ->GetGlobalTimeCorrectionProps()
                                     .GetOffsetCorrectionJumpThreshold()};
            if (cfg.GetTimeSyncCorrection().GetInitConfig()) {
                adaptionInterval = cfg.GetTimeSyncCorrection().GetOffsetCorrectionAdaptionInterval();
                jumpThreshold    = cfg.GetTimeSyncCorrection().GetOffsetCorrectionJumpThreshold();
            }
            static_cast< void >(new (ctx) resource::TBContext());
            ctx->InitContext(cfg.GetName(), _makeId(static_cast< std::uint8_t >(tbIndex), timeDomainId),
                             cfg.IsProvider(), cfg.GetTimeSyncCorrection().GetAllowProviderRateCorrection(),
                             static_cast< std::uint64_t >(adaptionInterval),
                             static_cast< std::uint64_t >(jumpThreshold),
                             timeDomainSetCfg->GetDomain(timeDomainId)->IsSteadyClock());
            static_cast< void >(new (&(ctx->GetTimeData())) resource::TimeData());
            ctx->GetTimeData().Init(timeDomainId);
            if (timeDomainId.IsOffsetDomain()) {
                TimeDomainId const id{timeDomainSetCfg->GetSyncDomainId(timeDomainId)};
                if (id.IsValid()) {
                    static_cast< void >(new (&(contextSet_[tbIndex].GetRefTimeData())) resource::TimeData());
                    contextSet_[tbIndex].GetRefTimeData().Init(id);
                }
            }

            config::TimeDomainSet::Domain const *const timeDomainCfg{
                configMan_->GetTimeDomainSet()->GetDomain(timeDomainId)};
            if (false == timeDomainCfg->GetSlave().empty()) {
                for (std::size_t i{0U}; i < timeDomainCfg->GetSlave().size(); i++) {
                    if (!timeDomainCfg->GetSlave()[i].GetEthernetConnector().empty()) {
                        ctx->GetTimeData().SetTimeLeapHealingCounter(
                            static_cast< std::uint32_t >(timeDomainCfg->GetSlave()[i].GetTimeLeapHealingCounter()));
                        break;
                    }
                }
            }
        }

        /// instead of goto
        break;
    }  /// while(true)

    return retCode;
}

/// @brief close and destroy time base resource manager, after which no process can use it
/// @return 0 - success
/// @return <0 - failure
std::int32_t TBResManager::_destroy() noexcept
{
    std::int32_t retCode{0};

    if (0 != shm_.Destroy()) {
        LOG().Error() << "shm_.Destroy(): " << GetErrString();
        retCode = kRET_E1;
    }

    contextSet_ = nullptr;

    return retCode;
}

/// @brief open time base resource manager for current process to use
/// @return 0 - success
/// @return <0 - failure
std::int32_t TBResManager::_open() noexcept
{
    std::int32_t retCode{0};
    _makeSharedMemPath();
    /// instead of goto
    while (true) {
        if (0 != shm_.Map(shareMemPath_.c_str())) {
            // LOG().Warn() << "shm_.Map(" << shareMemPath_ << "): " // remove many internal library errors
            retCode = kRET_E1;
            break;
        }
        contextSet_ = reinterpret_cast< TBContext * >(shm_.Get());

        /// instead of goto
        break;
    }  /// while(true)

    return retCode;
}

/// @brief close time base resource manager, after which current process cannot use it
/// @return 0 - success
/// @return <0 - failure
std::int32_t TBResManager::_close() noexcept
{
    std::int32_t retCode{0};

    if (0 != shm_.Unmap()) {
        LOG().Error() << "shm_.Unmap(): " << GetErrString();
        retCode = kRET_E1;
    }
    contextSet_ = nullptr;

    return retCode;
}

/// @brief when the synchronization time base sets the time, update the corresponding offset time base time lastGlobalTimestamp_ lastSteadyTimestamp_
/// @param tbId - time base ID
void TBResManager::UpdateOffsetTimeBase(timebase::resource::TimeBaseId const &tbId) noexcept
{
    timebase::resource::TBContext *const timeBaseCtx{GetContext(tbId)};
    if (nullptr == timeBaseCtx) {
        return;
    }
    if (!timeBaseCtx->GetTimeData().DomainId().IsSyncDomain()) {
        return;
    }
    std::shared_ptr< internal::config::TimeDomainSet const > timeDomainSetCfg{configMan_->GetTimeDomainSet()};

    ara::core::Vector< internal::config::TimeBase > const &timeBaseCfgs{timeBaseConfigSet_->GetAll()};
    std::size_t const tbNumber{timeBaseCfgs.size()};
    for (std::size_t tbIndex{0U}; tbIndex < tbNumber; tbIndex++) {
        TBContext *const ctx{contextSet_ + tbIndex};
        if (ctx == timeBaseCtx) {
            continue;
        }
        internal::config::TimeBase const &cfg{timeBaseCfgs[tbIndex]};
        internal::TimeDomainId const timeDomainId{cfg.GetTimeDomainId()};
        if (timeDomainId.IsOffsetDomain()) {
            TimeDomainId const id{timeDomainSetCfg->GetSyncDomainId(timeDomainId)};
            if (id == timeBaseCtx->GetTimeData().DomainId()) {
                ctx->GetRefTimeData().SetOffset(timeBaseCtx->GetTimeData().Offset());
                ctx->SetLastGlobalTimestamp(std::move(timeBaseCtx->LastGlobalTimestamp()));
                ctx->SetLastSteadyTimestamp(std::move(timeBaseCtx->LastSteadyTimestamp()));
                ctx->GetTimeData().SetRateDeviation(timeBaseCtx->GetTimeData().RateDeviation());
            }
        }
    }
}

/// @brief when the offset time base sets the rate offset, update the corresponding synchronization time base
/// @param tbId - time base ID
void TBResManager::UpdateSyncTimeBase(timebase::resource::TimeBaseId const &tbId) noexcept
{
    timebase::resource::TBContext *const timeBaseCtx{GetContext(tbId)};
    if (nullptr == timeBaseCtx) {
        return;
    }
    if (!timeBaseCtx->GetTimeData().DomainId().IsOffsetDomain()) {
        return;
    }
    std::shared_ptr< internal::config::TimeDomainSet const > timeDomainSetCfg{configMan_->GetTimeDomainSet()};

    ara::core::Vector< internal::config::TimeBase > const &timeBaseCfgs{timeBaseConfigSet_->GetAll()};
    std::size_t const tbNumber{timeBaseCfgs.size()};
    for (std::size_t tbIndex{0U}; tbIndex < tbNumber; tbIndex++) {
        TBContext *const ctx{contextSet_ + tbIndex};
        if (ctx == timeBaseCtx) {
            continue;
        }
        internal::config::TimeBase const &cfg{timeBaseCfgs[tbIndex]};
        internal::TimeDomainId const timeDomainId{cfg.GetTimeDomainId()};
        if (timeDomainId.IsSyncDomain()) {
            TimeDomainId const id{timeDomainSetCfg->GetSyncDomainId(timeDomainId)};
            if (id == timeBaseCtx->GetTimeData().DomainId()) {
                ctx->GetTimeData().SetRateDeviation(timeBaseCtx->GetTimeData().RateDeviation());
            }
        }
    }
}

}  // namespace resource
}  // namespace timebase
}  // namespace internal
}  // namespace tsync
}  // namespace ara