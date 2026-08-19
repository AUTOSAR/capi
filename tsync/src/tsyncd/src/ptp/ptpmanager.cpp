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
/// @file       ptpmanager.cpp
/// @brief      PTP management class
/// @details
/// @date       2023-01-17
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#include "ara/tsync/internal/ptp/ptpmanager.h"

#include "ara/tsync/internal/common.h"
#include "ara/tsync/internal/log/logger.h"

namespace ara {
namespace tsync {
namespace internal {
namespace ptp {

/// @brief log output
/// @return Logger object reference
static inline ara::tsync::internal::tslog::Logger &LOG() noexcept { return ara::tsync::internal::tslog::Log(); }

/// @brief create Ptp manager
/// @param timeDomain - time domain management handle
/// @param networkmgr - network management handle
/// @param configManager - configuration manager handle
/// @param bAllowDisPach - whether to allow time domain master to distribute time by default
/// @return handle, or nullptr.
std::unique_ptr< PtpManager > PtpManager::CreateManager(std::shared_ptr< timedomain::TDManager > const &timeDomain,
                                                        std::shared_ptr< ptp::NetworkManager > const &networkmgr,
                                                        std::shared_ptr< config::ConfigManager > const &configManager,
                                                        bool const bAllowDisPach) noexcept
{
    std::unique_ptr< PtpManager > man;
    man = std::make_unique< PtpManager >();
    if (nullptr != man) {
        if (0 != man->_init(timeDomain, networkmgr, configManager, bAllowDisPach)) {
            man = nullptr;
        }
    }
    return man;
}

/// @brief destructor
PtpManager::~PtpManager() noexcept
{
    mainLoop_          = nullptr;
    timeDomainManager_ = nullptr;
    nmManager_         = nullptr;
    configManager_     = nullptr;
    for (internal::TimeDomainId domainId{0U}; true == domainId.IsValid(); ++domainId) {
        ara::core::Map< std::uint8_t, std::shared_ptr< PtpContext > >::iterator const domainCtx{
            contextSet_.find(domainId.ToUint8())};
        if (contextSet_.end() != domainCtx) {
            contextSet_[domainId.ToUint8()]->ResetTimer();
        }
    }
    contextSet_.clear();
}

/// @brief initialize context
/// @return 0 - success
/// @return <0 - failure
std::int32_t PtpManager::_initContext() noexcept
{
    if (nullptr == timeDomainManager_) {
        return kRET_E1;
    }
    ara::core::Map< uint64_t, uint16_t > portNumMap;

    /// Scan all configured time domains, fill context information into ptpContext
    for (internal::TimeDomainId domainId{0U}; domainId.IsValid(); ++domainId) {
        config::TimeDomainSet::Domain const *const timeDomainCfg{timeDomainManager_->GetConfig(domainId)};
        std::shared_ptr< timedomain::TDContext > const timeDomainCtx{timeDomainManager_->GetContext(domainId)};
        if (nullptr == timeDomainCfg) {
            continue;
        }
        if (nullptr == timeDomainCtx) {
            continue;
        }
        ara::core::Map< std::uint8_t, std::shared_ptr< PtpContext > >::iterator const domainCtx{
            contextSet_.find(domainId.ToUint8())};
        if (contextSet_.end() == domainCtx) {
            contextSet_[domainId.ToUint8()] = std::make_shared< ptp::PtpContext >();
        }
        std::shared_ptr< ptp::PtpContext > ptpCtx{contextSet_[domainId.ToUint8()]};
        Configure &ptpCfg{ptpCtx->PtpConfig()};

        ptpCtx->SetTimeDomainContext(timeDomainCtx);
        ptpCfg.domainId = domainId;

        /// Whether the current time domain is Master on this machine
        if (timeDomainCfg->GetIsMasterOnThisMachine()) {
            ptpCfg.isMaster = true;
        } else {
            ptpCfg.isMaster = false;
        }
        timeDomainCtx->SetMasterFlag(ptpCfg.isMaster);

        ptpCfg.debounceTime = timeDomainCfg->GetDebounceTime();
        /// TODO(person in charge): // gateway
        // globalTimeCorreciontProps: ptp dont need this

        // globalTimeDomainProperty
        {
            config::TimeDomainSet::DomainProperty const &property{timeDomainCfg->GetGlobalTimeDomainProperty()};
            // crcFlags
            {
                config::CrcFlags const &crcFlags{property.GetCrcFlags()};
                ptpCfg.domainPtp.crcCorrectionField        = crcFlags.GetCrcCorrectionField();
                ptpCfg.domainPtp.crcDomainNumber           = crcFlags.GetCrcDomainNumber();
                ptpCfg.domainPtp.crcMessageLength          = crcFlags.GetCrcMessageLength();
                ptpCfg.domainPtp.crcPreciseOriginTimestamp = crcFlags.GetCrcPreciseOriginTimestamp();
                ptpCfg.domainPtp.crcSequenceId             = crcFlags.GetCrcSequenceId();
                ptpCfg.domainPtp.crcSourcePortIdentity     = crcFlags.GetCrcSourcePortIdentity();
            }
            // destinationPhysicalAddress
            static_cast< void >(ptpCfg.destAddr.FromStr(property.GetDestinationPhysicalAddress().c_str()));
            ptpCfg.vlanPri = property.GetVlanPriority();
            if (ptpCfg.isMaster) {
                ptpCfg.etherconnector = timeDomainCfg->GetGlobalTimeMaster().GetEthernetConnector();
                ptpCfg.vlanId         = timeDomainCfg->GetGlobalTimeMaster().GetVlanId();
            }
            // fupDataIDList;
            for (std::uint8_t const &data : property.GetFupDataIDList()) {
                ptpCfg.fupDataIDList.push_back(data);
            }
            // managedCouplingPort
            {
                config::ManagedCouplingPort const &port{property.GetManagedCouplingPort()};
                ptpCfg.domainPtp.pdelayLatencyThreshold           = port.GetPdelayLatencyThreshold();
                ptpCfg.domainPtp.globalTimeTxPdelayReqPeriod      = port.GetPdelayRequestPeriod();
                ptpCfg.domainPtp.pdelayRespAndRespFollowupTimeout = port.GetPdelayRespAndRespFollowupTimeout();
                ptpCfg.domainPtp.globalTimePdelayRespEnable       = port.GetPdelayResponseEnabled();
            }
            // messageCompliance
            if (config::GlobalTimeMessageFormat::kIEEE802_1AS == property.GetMessageCompliance()) {
                ptpCfg.domainPtp.messageCompliance = true;
            } else {
                ptpCfg.domainPtp.messageCompliance = false;
            }
            // vlanPriority
            //ptpCfg.framePrio = property.vlanPriority.
        }
        // globalTimeMaster
        if (true != timeDomainCfg->GetGlobalTimeMaster().GetName().empty()) {
            config::GlobalTimeMaster const &master{timeDomainCfg->GetGlobalTimeMaster()};
            ptpCfg.immediateResumeTime          = master.GetImmediateResumeTime();
            ptpCfg.isSystemWideGlobalTimeMaster = master.IsSystemWideGlobalTimeMaster();
            // syncPeriod
            ptpCfg.domainPtp.globalTimeTxPeriod = master.GetSyncPeriod();
            // crcSecured
            if (config::GlobalTimeCrcSupport::kCrcSupported == master.GetCrcSecured()) {
                ptpCfg.domainPtp.globalTimeTxCrcSecured = true;
            } else {
                ptpCfg.domainPtp.globalTimeTxCrcSecured = false;
            }
            // subTlvConfig
            {
                config::SubTlvConfig const &cfg{master.GetSubTlvConfig()};
                ptpCfg.domainPtp.tsynTLVFollowupOFSSubTLV  = cfg.OfsSubTlv();
                ptpCfg.domainPtp.tlvFollowupStatusSubTLV   = cfg.StatusSubTlv();
                ptpCfg.domainPtp.tlvFollowupTimeSubTLV     = cfg.TimeSubTlv();
                ptpCfg.domainPtp.tlvFollowupUserDataSubTLV = cfg.UserDataSubTlv();
            }

            // Set local synchronization status; machines configured as Master are initially synchronized
            if (true == ptpCfg.isMaster) {
                if (true == master.IsSystemWideGlobalTimeMaster()) {
                    timeDomainCtx->SetStatusSynchronized();
                } else {
                    timeDomainCtx->SetStatusSyncToGateway();
                }
            } else {
                timeDomainCtx->SetStatusNotSynchronized();
            }
        }

        // globalTimeSubDomain: PTP dont need this
        // networkSegmentId: PTP dont need this

        // slave
        if (false == timeDomainCfg->GetSlave().empty()) {
            // FIXME: There are multiple TimeSlaves, need to specify which configuration to use
            // Solution 1: Restrict that the same Machine can have only one Slave configuration for the same Domain, otherwise it conflicts with SWS.
            // Solution 2: ptp layer does not perform timeout and other slave-side detection; that detection is done at the timebase layer, so the slave configuration in timedomain needs to be copied to timebase to take effect.
            //             Since timebase side also has the same configuration, the timebase side configuration is preferred.
            if (!ptpCfg.isMaster) {
                for (std::size_t i{0U}; i < timeDomainCfg->GetSlave().size(); ++i) {
                    if (!timeDomainCfg->GetSlave()[i].GetEthernetConnector().empty()) {
                        ptpCfg.etherconnector = timeDomainCfg->GetSlave()[i].GetEthernetConnector();
                        ptpCfg.vlanId         = timeDomainCfg->GetSlave()[i].GetVlanId();
                        ptpCfg.domainPtp.globalTimeFollowupTimeout
                            = timeDomainCfg->GetSlave()[i].GetFollowUpTimeoutValue();
                        ptpCfg.domainPtp.rxCrcValidated = timeDomainCfg->GetSlave()[i].GetCrcValidated();
                        domainOnConnectors_[ptpCfg.etherconnector].emplace_back(domainId);
                        break;
                    }
                }
            }
        }
        // 2.2 tps not configured, add sdg
        ptpCfg.domainPtp.globalTimePropagationDelay         = timeDomainCfg->GetGlobalTimePropagationDelay();
        ptpCfg.domainPtp.masterSlaveConflictDetection       = timeDomainCfg->GetMasterSlaveConflictDetection();
        ptpCfg.domainPtp.crcTimeFlagsTxSecured              = timeDomainCfg->GetCrcTimeFlagsTxSecured();
        ptpCfg.domainPtp.globalTimeSequenceCounterJumpWidth = timeDomainCfg->GetGlobalTimeSequenceCounterJumpWidth();

        // syncLossTimeout
        ptpCfg.syncLossTimeout = timeDomainCfg->GetSyncLossTimeout();
        if (!ptpCfg.etherconnector.empty()) {
            ara::core::String const netname{isoft::osi::network::GetDeviceName(ptpCfg.etherconnector)};
            Network::Address addr;
            if (0 == isoft::osi::network::GetMacAddress(netname.c_str(), addr)) {
                struct message::PortIdentity srcPortIdentity;
                std::uint8_t *const pByte{reinterpret_cast< std::uint8_t * >(&srcPortIdentity.clockIdentity)};
                // just for qac. *(pByte + internal::kTS_NUM_2) same as pByte[internal::kTS_NUM_2]
                *pByte                         = addr.addrBytes[0];
                *(pByte + 1)                   = addr.addrBytes[1];
                *(pByte + internal::kTS_NUM_2) = addr.addrBytes[internal::kTS_NUM_2];
                *(pByte + internal::kTS_NUM_3) = kClockIdentByte3;
                *(pByte + internal::kTS_NUM_4) = kClockIdentByte4;
                *(pByte + internal::kTS_NUM_5) = addr.addrBytes[internal::kTS_NUM_3];
                *(pByte + internal::kTS_NUM_6) = addr.addrBytes[internal::kTS_NUM_4];
                *(pByte + internal::kTS_NUM_7) = addr.addrBytes[internal::kTS_NUM_5];
                ara::core::Map< uint64_t, uint16_t >::iterator const portNumiterator{
                    portNumMap.find(srcPortIdentity.clockIdentity)};
                if (portNumMap.end() == portNumiterator) {
                    portNumMap[srcPortIdentity.clockIdentity] = 0U;
                }
                srcPortIdentity.clockIdentity = isoft::util::NtoH64(srcPortIdentity.clockIdentity);
                std::uint16_t clockId{1U};  // for qac
                clockId                                   = clockId + portNumMap[srcPortIdentity.clockIdentity];
                portNumMap[srcPortIdentity.clockIdentity] = clockId;
                srcPortIdentity.portNumber                = portNumMap[srcPortIdentity.clockIdentity];
                ptpCtx->SetSrcPortIdentity(srcPortIdentity);
                ptpCtx->PtpConfig().localAddr = addr;
            }
        }
        LOG().Info() << "domain " << domainId.ToUint8() << " Initialized";
    }

    return 0;
}

/// @brief initialize timer
/// @return 0 - success
/// @return <0 - failure
std::int32_t PtpManager::_initTimer() noexcept
{
    Configure const *ptpCfg{nullptr};
    std::shared_ptr< PtpContext > ptpCtx{nullptr};
    std::shared_ptr< timedomain::TDContext > timeDomainCtx{nullptr};

    /// PTP configuration is for specific time domains, each time domain has its own configuration. Therefore, timers are also one per time domain.

    // Only need to set domains that can distribute
    for (internal::TimeDomainId domainId{0U}; domainId.IsValid(); ++domainId) {
        ptpCtx = _getContext(domainId);
        ptpCfg = _getConfig(domainId);
        if ((nullptr == ptpCtx) || (nullptr == ptpCfg)) {
            continue;
        }
        if (true != ptpCtx->IsValid()) {
            continue;
        }
        timeDomainCtx = ptpCtx->GetTimeDomainContext();

        /// Master
        if (ptpCfg->isMaster) {
            /// @traceid{PRS_TS_00016}
            /// If GlobalTimeTxPeriod is not 0 and the GLOBAL_TIME_BASE bit in TimeBaseStatus is set, TimeMaster should periodically send Sync.
            ///     note: Once synchronized at least once, the GLOBAL_TIME_BASE bit will be set, i.e., the state is not NotSynchronized.
            /// 2023-12-5 As long as it is master, isSystemWideGlobalTimeMaster true or false, send periodically first
            bool const statusSynchronized{timeDomainCtx->IsStatusSynchronized()};
            bool const syncToGW{timeDomainCtx->IsStatusSyncToGateway()};
            if ((ptpCfg->domainPtp.globalTimeTxPeriod > 0) && (statusSynchronized || syncToGW)) {
                double const to{ptpCfg->domainPtp.globalTimeTxPeriod};
                // First start timer does not set time, send immediately
                /// note: Timer time of 0 cannot trigger immediately
                timeDomainCtx->SetAllowDispatch(bAllowDisPach_);
                static_cast< void >(mainLoop_->MakeTimer(
                    ptpCtx->SyncPeriodTimer(), 1, [ptpCtx, domainId, to, timeDomainCtx, this]() noexcept -> void {
                        // Continuously set timeout
                        // NOTE:
                        // Continuously set timeout is not precise; tests show millisecond-level errors. For precise timing, need to manually calculate time deviation based on timestamps and correct time.
                        // MakeTimer() parameter is in ms, globalTimeTxPeriod unit is seconds
                        static_cast< void >(ptpCtx->SyncPeriodTimer()->UpdateTime(
                            static_cast< std::int32_t >(isoft::util::TimeS2MS(to))));

                        /// The purpose here is to reset states like time jump
                        timeDomainCtx->SetOffsetTS(timeDomainCtx->GetOffsetTS());

                        if (0 != this->_dispatchTime(domainId)) {
                            LOG().Error() << "_dispatchTime(" << domainId.ToUint8() << ")";
                        }
                    }));
            }

            /// Slave
        } else {
            /// Start SyncLoss timer
            if (ptpCfg->syncLossTimeout > 0) {
                double const to{ptpCfg->syncLossTimeout};
                static_cast< void >(mainLoop_->MakeTimer(
                    ptpCtx->SyncPeriodTimer(), (static_cast< std::int32_t >(isoft::util::TimeS2MS(to))),
                    [this, timeDomainCtx, domainId]() -> void {
                        LOG().Warn() << "time domain " << domainId.ToUint8() << " synchronization time out !";
                        {
                            timeDomainCtx->SetStatusTimeout();
                        }
                        /// Call time event callback function
                        TimeEventHandler const timeCb{timeSyncTimeoutCb_};
                        if (nullptr != timeCb) {
                            timeCb(domainId);
                        }
                    }));
            }
        }

    }  // for domain

    ///////////////////////// Delay measurement timer /////////////////////////
    /// Delay measurement should be performed on a network card's synchronization domain, slave, smallest domainid, for measurement
    /// @upstrace{PRS_TS_00003}  Periodically perform delay measurement or use fixed values.
    /// @upstrace{PRS_TS_00140}
    /// If globalTimeTxPdelayReqPeriod equals 0, disable propagation delay measurement and use fixed value globalTimePropagationDelay.
    /// @upstrace{PRS_TS_00141}   If globalTimeTxPdelayReqPeriod is greater than 0, perform periodic measurement.
    /// @upstrace{PRS_TS_00142}   If globalTimeTxPdelayReqPeriod is greater than 0, use globalTimePropagationDelay as default value.
    /// @upstrace{PRS_TS_00149}   If globalTimeTxPdelayReqPeriod is greater than 0, the smallest domain should be used for delay measurement, and the measurement result should be applied to all time resources. Only one periodic Pdelay is needed on an ECU, and the set TimeDomain is 0 to be compatible with the 802.1as standard.

    for (auto const &item : domainOnConnectors_) {
        /// Traverse addition; the first element of the vector is the smallest domainid
        internal::TimeDomainId const minDomainId{*(item.second.begin())};
        ptpCfg = _getConfig(minDomainId);
        ptpCtx = _getContext(minDomainId);

        if (nullptr == ptpCfg) {
            LOG().Warn() << "the TimeDomain 0 is not configured, Pdelay Mesurement will disabled.";
            return 0;
        }
        if (nullptr == ptpCtx) {
            LOG().Warn() << "the TimeDomain 0 is not configured, Pdelay Mesurement will disabled.";
            return 0;
        }

        timeDomainCtx = ptpCtx->GetTimeDomainContext();

        /// If this machine is the master of time domain 0, no further delay measurement is needed.
        if (true == ptpCfg->isMaster) {
            timeDomainCtx->SetDelayTS(std::move(internal::TimeValue{std::chrono::nanoseconds{0}}));
            return 0;
        }

        /// Initialize with fixed value
        std::uint64_t const dts{
            static_cast< std::uint64_t >(isoft::util::TimeS2NS(ptpCfg->domainPtp.globalTimePropagationDelay))};
        timeDomainCtx->SetDelayTS(std::move(internal::TimeValue(std::chrono::nanoseconds{dts})));

        if (ptpCfg->domainPtp.globalTimeTxPdelayReqPeriod > 0) {
            double const to{ptpCfg->domainPtp.globalTimeTxPdelayReqPeriod};
            /// First start timer does not set time, send immediately
            static_cast<
                void >(mainLoop_->MakeTimer(ptpCtx->PdelayTimer(), 1, [this, ptpCtx, to, minDomainId]() -> void {
                /// Continuously set timeout
                /// NOTE.
                /// Continuously set timeout is not precise; tests show millisecond-level errors. For precise timing, need to manually calculate time deviation based on timestamps and correct time.
                /// MakeTimer() parameter is in ms, globalTimeTxPeriod unit is seconds
                static_cast< void >(
                    ptpCtx->PdelayTimer()->UpdateTime((static_cast< std::int32_t >(isoft::util::TimeS2MS(to)))));
                /// Send Pdelay message
                if (0 != this->_sendPdelayReqMessage(minDomainId)) {
                    LOG().Error() << "_sendPdelayReqMessage(0)";
                    return;
                }
                /// Call message event callback function
                MessageEventHandler const mesgCb{pdelayReqMesgSendCb_};
                if (nullptr != mesgCb) {
                    mesgCb(minDomainId);
                }
            }));
        }
    }

    return 0;
}

/// @brief initialize
/// @param timeDomain - time domain management handle
/// @param networkmgr - network management handle
/// @param configManager - configuration manager handle
/// @param bAllowDisPach - whether to allow time domain master to distribute time by default
/// @return 0 - success
/// @return <0 - failure
std::int32_t PtpManager::_init(std::shared_ptr< timedomain::TDManager > const &timeDomain,
                               std::shared_ptr< ptp::NetworkManager > const &networkmgr,
                               std::shared_ptr< config::ConfigManager > const &configManager,
                               bool const bAllowDisPach) noexcept
{
    mainLoop_          = isoft::naicpp::GlobalGeneralEvLoop::Get();
    timeDomainManager_ = timeDomain;
    nmManager_         = networkmgr;
    configManager_     = configManager;
    bAllowDisPach_     = bAllowDisPach;
    if (0 != _initContext()) {
        return kRET_E2;
    }
    Configure const *ptpCfg{nullptr};
    std::shared_ptr< PtpContext > ptpCtx{nullptr};
    for (internal::TimeDomainId domainId{0U}; domainId.IsValid(); ++domainId) {
        ptpCtx = _getContext(domainId);
        ptpCfg = _getConfig(domainId);
        if ((nullptr == ptpCtx) || (nullptr == ptpCfg)) {
            continue;
        }
        if (true != ptpCtx->IsValid()) {
            continue;
        }
        nmManager_->OnRecv(
            ptpCfg->etherconnector,
            [this](void const *const mesg, std::uint16_t const mesgSize, Network::Address const &sourceAddr,
                   Network::Address const &destAddr, std::chrono::nanoseconds const &recvTimeStamp) noexcept -> void {
                _networkRecvCb(mesg, mesgSize, sourceAddr, destAddr, recvTimeStamp);
            });
    }

    if (0 != _initTimer()) {
        return kRET_E3;
    }

    return 0;
}

/// @brief register message event callback function, called when the message is received.
///         Multiple calls will overwrite the old callback function.
/// @param type - message type
/// @param cb - message event callback function, nullptr means cancel.
void PtpManager::OnMessageRecv(MessageEventType const &type, MessageEventHandler const &cb) noexcept
{
    switch (type) {
        case MessageEventType::kSync: {
            syncMesgRecvCb_ = cb;
            break;
        }
        case MessageEventType::kFollowup: {
            followUpMesgRecvCb_ = cb;
            break;
        }
        case MessageEventType::kPdelayReq: {
            pdelayReqMesgRecvCb_ = cb;
            break;
        }
        case MessageEventType::kPdelayResp: {
            pdelayRespMesgRecvCb_ = cb;
            break;
        }
        case MessageEventType::kPdelayRespFollowup: {
            pdelayRespFlpMesgRecvCb_ = cb;
            break;
        }
        default: {
            break;
        }
    }
}

/// @brief register message event callback function, called when the message is sent.
///         Multiple calls will overwrite the old callback function.
/// @param type - message type
/// @param cb - message event callback function, nullptr means cancel.
void PtpManager::OnMessageSend(MessageEventType const &type, MessageEventHandler const &cb) noexcept
{
    switch (type) {
        case MessageEventType::kSync: {
            syncMesgSendCb_ = cb;
            break;
        }
        case MessageEventType::kFollowup: {
            followUpMesgSendCb_ = cb;
            break;
        }
        case MessageEventType::kPdelayReq: {
            pdelayReqMesgSendCb_ = cb;
            break;
        }
        case MessageEventType::kPdelayResp: {
            pdelayRespMesgSendCb_ = cb;
            break;
        }
        case MessageEventType::kPdelayRespFollowup: {
            pdelayRespFlpMesgSendCb_ = cb;
            break;
        }
        default: {
            break;
        }
    }
}

/// @brief register time event callback function, called when the event occurs
/// @param type - time event type
/// @param cb - time event callback function
void PtpManager::OnTimeEvent(TimeEventType const &type, TimeEventHandler const &cb) noexcept
{
    switch (type) {
        case TimeEventType::kTimeSyncFinished: {
            timeSyncFinishedCb_ = cb;
            break;
        }
        case TimeEventType::kTimeSyncTimeout: {
            timeSyncTimeoutCb_ = cb;
            break;
        }
        case TimeEventType::kPdelayFinished: {
            pdelayFinishedCb_ = cb;
            break;
        }
    }
}

/// @brief network message asynchronous receive callback function.
/// @param mesg - received message buffer.
/// @param mesgSize - length of received message.
/// @param sourceAddr - message source address.
/// @param destAddr - message destination address.
/// @param recvTimeStamp - system timestamp at reception, returned to user.
void PtpManager::_networkRecvCb(void const *const mesg,
                                std::uint16_t const mesgSize,
                                Network::Address const &sourceAddr,
                                Network::Address const &destAddr,
                                std::chrono::nanoseconds const &recvTimeStamp) noexcept
{
    std::ignore = mesgSize;
    message::Header const *const header{reinterpret_cast< message::Header const * >(mesg)};
    internal::TimeDomainId const domainId{header->GetDomainId()};
    std::shared_ptr< PtpContext > const ptpCtx{this->_getContext(domainId)};
    Configure const *const ptpCfg{this->_getConfig(domainId)};
    if ((nullptr == ptpCtx) || (nullptr == ptpCfg)) {
        LOG().Error() << "nullptr == ptpCtx || nullptr == ptpCfg)";
        return;
    }
    if ((true != (destAddr == ptpCfg->destAddr)) && (true != (destAddr == ptpCfg->localAddr))) {
        return;
    }

    ptpCtx->SetPeerAddr(sourceAddr);
    std::chrono::nanoseconds const &ts{recvTimeStamp};

    /// @traceid{PRS_TS_00050} TimeMaster should support transmission and response of Sync/Followup/PdelayReq/PdelayResp/PdelayRespFollowup etc. packets.
    /// @traceid{PRS_TS_00023} TimeSlave should support response to SyncFollowup, just like PdelayReq/PdelayResp.
    /// @traceid{PRS_TS_00055} Path delay measurement should use P2P method.
    /// @traceid{PRS_TS_00120} After message verification passes, it should be parsed.

    switch (header->GetType()) {
        case message::Type::kSync: {
            LOG().Verbose() << "Recved SyncMessage from domain " << domainId.ToUint8();
            // E2E T2
            // Master conflict detection
            if (ptpCfg->isMaster && ptpCfg->domainPtp.masterSlaveConflictDetection) {
                LOG().Error() << "Master Conflicted ! this Machine Have been Configured as Master ! this "
                                 "SyncMessage will be ignored.";
            } else {
                static_cast< void >(this->_disassembleSyncMesg(reinterpret_cast< message::Sync const * >(mesg), ts));
            }
            break;
        }
        case message::Type::kFollowup: {
            LOG().Verbose() << "Recved FolupMessage from domain " << domainId.ToUint8();
            // Master conflict detection
            if (ptpCfg->isMaster && ptpCfg->domainPtp.masterSlaveConflictDetection) {
                LOG().Error() << "Master Conflicted ! this Machine Have been Configured as Master ! this "
                                 "FollowupMessage will be ignored.";
            } else {
                static_cast< void >(
                    this->_disassembleFollowupMesg(reinterpret_cast< message::Followup const * >(mesg), ts));
            }
            break;
        }

        case message::Type::kPdelayReq: {
            static_cast< void >(
                this->_disassemblePdelayReqMesg(reinterpret_cast< message::PdelayReq const * >(mesg), ts));
            LOG().Verbose() << "Recved PdelayReqMessage from domain " << domainId.ToUint8();
            break;
        }

        case message::Type::kPdelayResp: {
            static_cast< void >(
                this->_disassemblePdelayRespMesg(reinterpret_cast< message::PdelayResp const * >(mesg), ts));
            LOG().Verbose() << "Recved PdelayRespMessage from domain " << domainId.ToUint8();
            break;
        }

        case message::Type::kPdelayRespFollowup: {
            static_cast< void >(this->_disassemblePdelayRespFollowupMesg(
                reinterpret_cast< message::PdelayRespFollowup const * >(mesg), ts));
            LOG().Verbose() << "Recved PdelayReqFupMessage from domain " << domainId.ToUint8();
            break;
        }

        default: {
            /// AUTOSAR ignores packet types other than SyncFollowup in E2E
            /// @upstrace PRS_TS_00005 The protocol state machine should not process Announce messages.
            /// @upstrace PRS_TS_00206 All IEEE rapid spanning tree protocols should be ignored at the receiver.
            /// Ignore all other types of messages
            break;
        }
    }

    return;
}

/// @brief send message
/// @param destAddr - destination address
/// @param data - data
/// @param size - data length
/// @param ptpCfg - ptpCfg configuration
/// @return send timestamp
std::chrono::nanoseconds PtpManager::_sendMessage(Network::Address const &destAddr,
                                                  void const *const data,
                                                  std::uint16_t const size,
                                                  Configure const *const ptpCfg) const noexcept
{
    if ((nullptr == data) || (size <= 0U)) {
        LOG().Error() << "data(" << data << "), size(" << size << ")";
        return std::move(std::chrono::nanoseconds(-1));
    }
    if (nullptr == ptpCfg) {
        LOG().Error() << "ptpCfg is nullptr";
        return std::move(std::chrono::nanoseconds(-1));
    }

    std::chrono::nanoseconds ts{0};
    if (0 != nmManager_->Send(data, size, destAddr, ptpCfg->vlanPri, ptpCfg->vlanId, ptpCfg->etherconnector, ts)) {
        LOG().Error() << "network_.Send())";
        return std::move(std::chrono::nanoseconds(-1));
    }

    return ts;
}

/// @brief send Sync message. Data is obtained from the corresponding time domain context.
/// @param domainId time domain ID. In compatibility mode, domainId can only be 0; in AUTOSAR mode, domainId ranges 0 - 15;
/// @return 0 - success
/// @return <0 - failure
/// @traceid{PRS_TS_00104} Assembly and verification of time synchronization packets.
std::int32_t PtpManager::_sendSyncMessage(internal::TimeDomainId const &domainId) noexcept
{
    message::Sync mesg;
    std::shared_ptr< PtpContext > const context{_getContext(domainId)};
    Configure const *const pDomainConf{_getConfig(domainId)};
    std::shared_ptr< timedomain::TDContext > const timeDomainCtx{timeDomainManager_->GetContext(domainId)};

    if (nullptr == pDomainConf) {
        return kRET_E1;
    }
    if (nullptr == context) {
        return kRET_E1;
    }
    if (nullptr == timeDomainCtx) {
        return kRET_E1;
    }

    /// @traceid{PRS_TS_00189} SyncSeqId increments by 1 each time after sending, and after reaching 65535 continues from 0.
    /// The first sent SeqID is 1, not 0
    timeDomainCtx->SetSyncSeqId(static_cast< std::uint16_t >(timeDomainCtx->GetSyncSeqId() + 1U));

    mesg.Init();
    /// @traceid{PRS_TS_00061} In compatibility mode, only IEEE802.1AS packets are sent.
    /// @traceid{PRS_TS_00062} In non-compatibility mode, AUTOSAR packets are sent.
    mesg.SetDomainId(domainId);
    mesg.SetClockIdentity(context->GetSrcPortIdentity().clockIdentity);
    mesg.SetSourcePortNumber(context->GetSrcPortIdentity().portNumber);
    mesg.SetSequenceID(timeDomainCtx->GetSyncSeqId());
    mesg.SetLogMessageInterval(static_cast< std::int8_t >(log2(pDomainConf->domainPtp.globalTimeTxPeriod)));
    std::chrono::nanoseconds tp{timeDomainCtx->GetGlobalTime().time_since_epoch().count()};
    if (!pDomainConf->domainPtp.messageCompliance) {
        mesg.SetOriginTimeStamp(tp);
    } else {
        std::chrono::nanoseconds const ts2{0};
        mesg.SetOriginTimeStamp(ts2);
    }

    // E2E T1
    tp = _sendMessage(pDomainConf->destAddr, reinterpret_cast< const void * >(&mesg), sizeof(mesg), pDomainConf);
    if (tp.count() < 0) {
        return kRET_E2;
    }
    timeDomainCtx->SetSyncEnTS(std::move(internal::TimeValue(tp)));

    return 0;
}

/// @brief send Followup message. Data is obtained from the corresponding time domain context.
/// @param domainId time domain ID. In compatibility mode, domainId can only be 0; in AUTOSAR mode, domainId ranges 0 - 15;
/// @return 0 - success
/// @return <0 - failure
std::int32_t PtpManager::_sendFollowupMessage(internal::TimeDomainId const &domainId) noexcept
{
    message::Followup mesg;
    std::shared_ptr< PtpContext > const ptpCtx{_getContext(domainId)};
    Configure const *const ptpCfg{_getConfig(domainId)};
    std::shared_ptr< timedomain::TDContext > const timeDomainCtx{timeDomainManager_->GetContext(domainId)};

    if (nullptr == ptpCfg) {
        return kRET_E1;
    }
    if (nullptr == ptpCtx) {
        return kRET_E1;
    }
    if (nullptr == timeDomainCtx) {
        return kRET_E1;
    }

    mesg.Init();
    /// @traceid{PRS_TS_00061} In compatibility mode, only IEEE802.1AS packets are sent.
    /// @traceid{PRS_TS_00062} In non-compatibility mode, AUTOSAR packets are sent.
    mesg.SetDomainId(domainId);

    /// @traceid{PRS_TS_00190}  Followup message uses the same sequenceID value as the Sync message.
    mesg.SetSequenceID(timeDomainCtx->GetSyncSeqId());
    mesg.SetLogMessageInterval(static_cast< std::int8_t >(log2(ptpCfg->domainPtp.globalTimeTxPeriod)));
    std::chrono::nanoseconds tp{timeDomainCtx->GetSyncEnTS().ToChrono().count()};
    /// @traceid{PRS_TS_00018} The Followup message should carry the calculated PreciseOriginTimeStamp.
    // E2E T1
    // SyncEnTS stores steady_clock time value, convert to GlobalTime time value (add Offset).
    // mesg.SetPreciseOriginTimeStamp(tp + timeDomainCtx->GetOffsetTS()).
    mesg.SetPreciseOriginTimeStamp(
        static_cast< internal::TimeValue >(timeDomainCtx->Steady2GlobalTime(tp).time_since_epoch()));
    mesg.SetClockIdentity(ptpCtx->GetSrcPortIdentity().clockIdentity);
    mesg.SetSourcePortNumber(ptpCtx->GetSrcPortIdentity().portNumber);

    mesg.AssembleTLV(ptpCtx);

    // The size of the Followup message is not fixed; it needs to be calculated based on configuration.
    tp = _sendMessage(ptpCfg->destAddr, reinterpret_cast< const void * >(&mesg), mesg.GetMessageLength(), ptpCfg);
    if (tp.count() < 0) {
        return kRET_E2;
    }
    // Followup does not need to record send timestamp

    return 0;
}

/// @brief send PdelayReqMessage message. Data is obtained from the corresponding time domain context.
///     PTP protocol specification requires that currently only the path delay from the local machine to time domain 0 needs to be measured.
/// @param domainId time domain ID. In compatibility mode, domainId can only be 0; in AUTOSAR mode, domainId ranges 0 - 15;
/// @return 0 - success
/// @return <0 - failure
std::int32_t PtpManager::_sendPdelayReqMessage(internal::TimeDomainId const &domainId) noexcept
{
    message::PdelayReq mesg;
    std::shared_ptr< PtpContext > const ptpCtx{_getContext(domainId)};
    Configure const *const ptpCfg{_getConfig(domainId)};
    std::shared_ptr< timedomain::TDContext > const timeDomainCtx{timeDomainManager_->GetContext(domainId)};

    if (nullptr == ptpCfg) {
        return kRET_E1;
    }
    if (nullptr == ptpCtx) {
        return kRET_E1;
    }
    if (nullptr == timeDomainCtx) {
        return kRET_E1;
    }

    std::uint16_t seqId{timeDomainCtx->GetPdelaySeqId()};
    /// @traceid{PRS_TS_00188} PdelaySeqId increments by 1 each time.
    // In actual implementation, for convenience, increase seqId count before sending PdelayReqMessage.
    ++seqId;
    timeDomainCtx->SetPdelaySeqId(seqId);
    mesg.Init();
    mesg.SetDomainId(domainId);
    mesg.SetSequenceID(seqId);
    mesg.SetLogMessageInterval(static_cast< std::int8_t >(log2(ptpCfg->domainPtp.globalTimeTxPdelayReqPeriod)));

    std::chrono::nanoseconds tp{Clock::now().time_since_epoch().count()};
    mesg.SetOriginTimeStamp(std::move(internal::TimeValue(tp)));
    mesg.SetClockIdentity(ptpCtx->GetSrcPortIdentity().clockIdentity);
    mesg.SetSourcePortNumber(ptpCtx->GetSrcPortIdentity().portNumber);

    tp = _sendMessage(ptpCfg->destAddr, reinterpret_cast< const void * >(&mesg), sizeof(mesg), ptpCfg);
    if (tp.count() < 0) {
        return kRET_E2;
    }
    // P2P T1
    timeDomainCtx->SetPdelayReqEnTS(std::move(internal::TimeValue(tp)));
    // Reset timeout detection
    timeDomainCtx->SetPdelayRespTimeout(false);
    timeDomainCtx->SetPdelayRespFlpTimeout(false);

    return 0;
}

/// @brief send PdelayResponse message. Data is obtained from the corresponding time domain context.
/// @param preqmsg delay request request message
/// @param domainId time domain ID;
/// @return 0 - success
/// @return <0 - failure
std::int32_t PtpManager::_sendPdelayRespMessage(message::PdelayReq const *const preqmsg,
                                                internal::TimeDomainId const &domainId) noexcept
{
    message::PdelayResp mesg;
    std::shared_ptr< PtpContext > const ptpCtx{_getContext(domainId)};
    Configure const *const ptpCfg{_getConfig(domainId)};
    std::shared_ptr< timedomain::TDContext > const timeDomainCtx{timeDomainManager_->GetContext(domainId)};

    if (nullptr == ptpCfg) {
        return kRET_E1;
    }
    if (nullptr == ptpCtx) {
        return kRET_E1;
    }
    if (nullptr == timeDomainCtx) {
        return kRET_E1;
    }

    /// @traceid{PRS_TS_00191}  Pdelay_Resp message uses the same sequenceID value as the Pdelay_Req message.
    std::uint16_t const seqId{timeDomainCtx->GetPdelaySeqId()};
    mesg.Init();
    mesg.SetDomainId(domainId);
    mesg.SetSequenceID(seqId);
    // P2P T2
    mesg.SetRequestReceiptTimeStamp(timeDomainCtx->GetPdelayReqInTS());
    if (nullptr != preqmsg) {
        struct message::PortIdentity portIdentity;
        portIdentity.clockIdentity = isoft::util::HtoN64(preqmsg->GetClockIdentity());
        portIdentity.portNumber    = isoft::util::HtoN16(preqmsg->GetSourcePortNumber());
        mesg.SetRequestingPortIdentity(portIdentity);
    }
    mesg.SetClockIdentity(ptpCtx->GetSrcPortIdentity().clockIdentity);
    mesg.SetSourcePortNumber(ptpCtx->GetSrcPortIdentity().portNumber);
    // P2P T3
    std::chrono::nanoseconds const ts{
        std::move(_sendMessage(ptpCtx->GetPeerAddr(), reinterpret_cast< const void * >(&mesg), sizeof(mesg), ptpCfg))};
    if (ts.count() < 0) {
        return kRET_E2;
    }
    timeDomainCtx->SetPdelayRespEnTS(std::move(internal::TimeValue(ts)));

    return 0;
}

/// @brief send PdelayResponseFollowup message. Data is obtained from the corresponding time domain context.
/// @param preqmsg delay request request message
/// @param domainId time domain ID;
/// @return 0 - success
/// @return <0 - failure
std::int32_t PtpManager::_sendPdelayRespFollowupMessage(message::PdelayReq const *const preqmsg,
                                                        internal::TimeDomainId const &domainId) noexcept
{
    message::PdelayRespFollowup mesg;
    mesg.Init();
    std::shared_ptr< PtpContext > const ptpCtx{_getContext(domainId)};
    Configure const *const ptpCfg{_getConfig(domainId)};
    std::shared_ptr< timedomain::TDContext > const timeDomainCtx{timeDomainManager_->GetContext(domainId)};

    if (nullptr == ptpCfg) {
        return kRET_E1;
    }
    if (nullptr == ptpCtx) {
        return kRET_E1;
    }
    if (nullptr == timeDomainCtx) {
        return kRET_E1;
    }

    /// @traceid{PRS_TS_00191}  Pdelay_Resp_fup message uses the same sequenceID value as the Pdelay_Req message.
    std::uint16_t const seqId{timeDomainCtx->GetPdelaySeqId()};

    mesg.SetDomainId(domainId);
    mesg.SetSequenceID(seqId);
    // P2P T3
    mesg.SetResponseOriginTimeStamp(timeDomainCtx->GetPdelayRespEnTS());
    if (nullptr != preqmsg) {
        struct message::PortIdentity portIdentity;
        portIdentity.clockIdentity = isoft::util::HtoN64(preqmsg->GetClockIdentity());
        portIdentity.portNumber    = isoft::util::HtoN16(preqmsg->GetSourcePortNumber());
        mesg.SetRequestingPortIdentity(portIdentity);
    }
    mesg.SetClockIdentity(ptpCtx->GetSrcPortIdentity().clockIdentity);
    mesg.SetSourcePortNumber(ptpCtx->GetSrcPortIdentity().portNumber);
    std::chrono::nanoseconds const ts{
        std::move(_sendMessage(ptpCtx->GetPeerAddr(), reinterpret_cast< const void * >(&mesg), sizeof(mesg), ptpCfg))};
    if (ts.count() < 0) {
        return kRET_E2;
    }

    return 0;
}
/// @brief set time
///         This function will immediately send Sync message and Followup message.
/// @param domainId - time domain ID
/// @return 0 - success
/// @return <0 - failure
std::int32_t PtpManager::SetTime(internal::TimeDomainId const &domainId) noexcept
{
    std::shared_ptr< PtpContext > ptpCtx{nullptr};
    std::shared_ptr< timedomain::TDContext > timeDomainCtx{nullptr};
    Configure const *ptpCfg{nullptr};
    /// PTP configuration is for specific time domains, each time domain has its own configuration. Therefore, timers are also one per time domain.
    ptpCtx = _getContext(domainId);
    ptpCfg = _getConfig(domainId);
    if ((nullptr == ptpCtx) || (nullptr == ptpCfg)) {
        LOG().Error() << "SetTime nullptr == ptpCtx (" << domainId.ToUint8() << ")";
        return kRET_E1;
    }
    if (true != ptpCtx->IsValid()) {
        LOG().Error() << "SetTime ptpCtx->IsValid (" << domainId.ToUint8() << ")";
        return kRET_E1;
    }
    timeDomainCtx = ptpCtx->GetTimeDomainContext();
    double const to{ptpCfg->domainPtp.globalTimeTxPeriod};
    static_cast< void >(ptpCtx->SyncPeriodTimer()->UpdateTime(static_cast< std::int32_t >(isoft::util::TimeS2MS(to))));
    timeDomainCtx->SetAllowDispatch(true);
    /// The purpose here is to reset states like time jump
    timeDomainCtx->SetOffsetTS(timeDomainCtx->GetOffsetTS());

    if (0 != this->_dispatchTime(domainId)) {
        LOG().Error() << "SetTime error _dispatchTime(" << domainId.ToUint8() << ")";
    }
    return 0;
}
/// @brief distribute time synchronization information
///         This function will immediately send Sync message and Followup message.
/// @param domainId - time domain ID
/// @return 0 - success
/// @return <0 - failure
std::int32_t PtpManager::_dispatchTime(internal::TimeDomainId const &domainId) noexcept
{
    std::shared_ptr< PtpContext > const ptpCtx{_getContext(domainId)};
    Configure const *const ptpCfg{_getConfig(domainId)};
    std::shared_ptr< timedomain::TDContext > const timeDomainCtx{timeDomainManager_->GetContext(domainId)};

    if (nullptr == ptpCtx) {
        return kRET_E1;
    }
    if (nullptr == ptpCfg) {
        return kRET_E1;
    }
    if (nullptr == timeDomainCtx) {
        return kRET_E1;
    }

    if (ptpCfg->isMaster != true) {
        LOG().Warn() << "someone wants to _dispatchTime on domain " << domainId.ToUint8()
                     << ", but this machine is not a master.";
        return kRET_E2;
    }
    if (timeDomainCtx->AllowDispatch() != true) {
        return 0;
    }

    LOG().Verbose() << "_dispatchTime to TimeDomain " << domainId.ToUint8();

    // Send Sync message
    if (0 != _sendSyncMessage(domainId)) {
        LOG().Error() << "_sendSyncMessage(" << domainId.ToUint8() << ")";
        return -GetErrNo();
    }

    // Call message event callback function
    MessageEventHandler const mesgCb{syncMesgSendCb_};
    if (nullptr != mesgCb) {
        mesgCb(domainId);
    }

    // Send Followup
    if (0 != _sendFollowupMessage(domainId)) {
        LOG().Error() << "_sendFollowupMessage(" << static_cast< std::int32_t >(domainId.ToUint8()) << ")";
        return -GetErrNo();
    }
    // Followup message is not an event message, so there is no callback function when sending.

    // For the sender, the time synchronization completion callback function also needs to be called because the STB on the same Machine also needs to do ROC based on this event.
    // Call time event callback function

    TimeEventHandler const timeCb{timeSyncFinishedCb_};
    if (nullptr != timeCb) {
        timeCb(domainId);
    }
    if (timeDomainCtx->IsSyncDomain()) {
        _syncExternalClock(domainId);
    }
    return 0;
}

// In the P2P communication model, offset and delay calculation formulas are as follows:
// offset = ((t4 - t3) - (t2 - t1) ) / 2
// delay = ((t2 - t1) + (t4 - t3)) / 2
// AUTOSAR clock synchronization calculation steps:
// 1. P2P calculates delay
// 2. Sync calculates offset
// Therefore the calculation method is:
// t2 - t1 = offset + delay
// offset = t2 - t1 - delay

/// @brief calculate path delay, result stored in time synchronization context delay
/// @param domainId - time domain ID
/// @returns none
void PtpManager::_calculateDelay(internal::TimeDomainId const &domainId) noexcept
{
    std::shared_ptr< PtpContext > const ptpCtx{_getContext(domainId)};
    Configure const *const ptpCfg{_getConfig(domainId)};
    std::shared_ptr< timedomain::TDContext > const timeDomainCtx{timeDomainManager_->GetContext(domainId)};

    if (nullptr == ptpCtx) {
        return;
    }
    if (nullptr == ptpCfg) {
        return;
    }
    if (nullptr == timeDomainCtx) {
        return;
    }

    std::chrono::nanoseconds const delay{
        std::move(((timeDomainCtx->GetPdelayReqInTS() - timeDomainCtx->GetPdelayReqEnTS()).ToChrono()
                   + (timeDomainCtx->GetPdelayRespInTS() - timeDomainCtx->GetPdelayRespEnTS()).ToChrono())
                  / 2)};
    std::chrono::nanoseconds const d{std::abs(delay.count())};

#if 0 & ARA_TSYNC_DEBUG
    std::cout << "T1\tT2\tT3\tT4" << std::endl;
    std::cout << isoft::util::CutoutSecond(timeDomainCtx->GetPdelayReqEnTS().ToChrono()) << "-"
              << isoft::util::CutoutNanoSecond(timeDomainCtx->GetPdelayReqEnTS().ToChrono()) << std::endl;
    std::cout << isoft::util::CutoutSecond(timeDomainCtx->GetPdelayReqInTS().ToChrono()) << "-"
              << isoft::util::CutoutNanoSecond(timeDomainCtx->GetPdelayReqInTS().ToChrono()) << std::endl;
    std::cout << isoft::util::CutoutSecond(timeDomainCtx->GetPdelayRespEnTS().ToChrono()) << "-"
              << isoft::util::CutoutNanoSecond(timeDomainCtx->GetPdelayRespEnTS().ToChrono()) << std::endl;
    std::cout << isoft::util::CutoutSecond(timeDomainCtx->GetPdelayRespInTS().ToChrono()) << "-"
              << isoft::util::CutoutNanoSecond(timeDomainCtx->GetPdelayRespInTS().ToChrono()) << std::endl;
    std::cout << "DelayTsInNS: " << d.count() << std::endl;
#endif
    /// @upstrace PRS_TS_00154 If the calculated delay value exceeds PdelayLatencyThreshold, keep the previous delay value.
    if (d.count() > static_cast< std::int64_t >(isoft::util::TimeS2NS(ptpCfg->domainPtp.pdelayLatencyThreshold))) {
        return;
    }

    /// @upstrace{PRS_TS_00149}   If globalTimeTxPdelayReqPeriod is greater than 0, the smallest time domain should be used for delay measurement,
    /// and the measurement result should be applied to all time resources.

    for (auto const id : domainOnConnectors_[ptpCfg->etherconnector]) {
        std::shared_ptr< timedomain::TDContext > timeDomainCtxtmp{timeDomainManager_->GetContext(id)};
        if (nullptr != timeDomainCtxtmp) {
            timeDomainCtxtmp->SetDelayTS(std::move(internal::TimeValue(d)));
        }
    }
}

/// @brief calculate offset, result stored in time synchronization context offset
/// @param domainId - time domain ID
/// @returns none
/// @traceid{SWS_TS_00055}
void PtpManager::_calculateOffset(internal::TimeDomainId const &domainId) noexcept
{
    std::shared_ptr< PtpContext > const ptpCtx{_getContext(domainId)};
    std::shared_ptr< timedomain::TDContext > const timeDomainCtx{timeDomainManager_->GetContext(domainId)};

    if (nullptr == ptpCtx) {
        return;
    }
    if (nullptr == timeDomainCtx) {
        return;
    }

    std::chrono::nanoseconds const offset{
        timeDomainCtx->GetSyncInTS().ToChrono().count() - timeDomainCtx->GetSyncEnTS().ToChrono().count()
        - timeDomainCtx->GetCorrectionField() - timeDomainCtx->GetDelayTS().ToChrono().count()};
    // After calculation using the E2E formula, Offset is Local - Global, convert to Global - Local
    timeDomainCtx->SetOffsetTS(internal::TimeValue{std::chrono::nanoseconds{-(offset.count())}});
#if 0
    std::cout << "SyncInTS: " << isoft::util::CutoutSecond(timeDomainCtx->GetSyncInTS().ToChrono()) << "-" << isoft::util::CutoutNanoSecond(timeDomainCtx->GetSyncInTS().ToChrono())<< std::endl;
    std::cout << "SyncEnTS: " << isoft::util::CutoutSecond(timeDomainCtx->GetSyncEnTS().ToChrono()) << "-" << isoft::util::CutoutNanoSecond(timeDomainCtx->GetSyncEnTS().ToChrono())<< std::endl;
    std::cout << "CorrectionEnTs: " << isoft::util::CutoutSecond((timeDomainCtx->GetCorrectionField())) << "-" << isoft::util::CutoutNanoSecond((timeDomainCtx->GetCorrectionField()))<< std::endl;
    std::cout << "DelayTS : " << timeDomainCtx->GetDelayTS().GetSecond()<<"-"<<timeDomainCtx->GetDelayTS().GetNanoSecond() << std::endl;
    std::cout << "OffSetTS: " << isoft::util::CutoutSecond(timeDomainCtx->GetOffsetTS().ToChrono()) << "-" << isoft::util::CutoutNanoSecond(timeDomainCtx->GetOffsetTS().ToChrono())<< std::endl;
    std::cout << "PreOffSetTS: " << isoft::util::CutoutSecond(timeDomainCtx->GetPreOffsetTS().ToChrono()) << "-" << isoft::util::CutoutNanoSecond(timeDomainCtx->GetPreOffsetTS().ToChrono())<< std::endl;
    std::cout << "OffSetTS - PreOffSetTS: " << isoft::util::CutoutSecond(timeDomainCtx->GetOffsetTS().ToChrono() - timeDomainCtx->GetPreOffsetTS().ToChrono()) << "-"  << isoft::util::CutoutNanoSecond(timeDomainCtx->GetOffsetTS().ToChrono() - timeDomainCtx->GetPreOffsetTS().ToChrono())<< std::endl;
#endif
}

/// @brief parse Sync message.
/// @param mesg message to parse;
/// @param inTs - message reception arrival timestamp.
/// @return 0 - success
/// @return <0 - failure
std::int32_t PtpManager::_disassembleSyncMesg(message::Sync const *const mesg,
                                              std::chrono::nanoseconds const &inTs) noexcept
{
    internal::TimeDomainId const domainId{mesg->GetDomainId()};
    std::uint16_t const seqId{mesg->GetSequenceID()};
    std::shared_ptr< PtpContext > const ptpCtx{_getContext(domainId)};
    Configure const *const ptpCfg{_getConfig(domainId)};
    std::shared_ptr< timedomain::TDContext > const timeDomainCtx{timeDomainManager_->GetContext(domainId)};

    if (nullptr == ptpCtx) {
        return kRET_E1;
    }
    if (nullptr == ptpCfg) {
        return kRET_E1;
    }
    if (nullptr == timeDomainCtx) {
        return kRET_E1;
    }

    // Master conflict detection
    if (ptpCfg->isMaster) {
        return 0;
    }
    if (!ptpCtx->IsValid()) {
        return kRET_E4;
    }

    // Each time a Sync message is received, update the SyncLossTimer timeout and restart the timer.
    // If it is updated before timeout, timeout will not be triggered.
    double const to{ptpCfg->syncLossTimeout};
    static_cast< void >(ptpCtx->SyncPeriodTimer()->UpdateTime(static_cast< std::int32_t >(isoft::util::TimeS2MS(to))));

    /// @traceid{PRS_TS_00198} If the SeqID of the received Sync differs too much from the previous Sync, or is the same, ignore this message.
    /// @traceid{PRS_TS_00199} The first Sync message or messages after timeout do not need to check JumpWide
    ///         After the first sync or timeout reset, syncid will be set to 0.
    if (timeDomainCtx->GetSyncSeqId() != 0U) {
        if (mesg->GetSequenceID() == timeDomainCtx->GetSyncSeqId()) {
            return kRET_E2;
        }
        std::uint16_t const v{static_cast< std::uint16_t >(1U + ptpCfg->domainPtp.globalTimeSequenceCounterJumpWidth)};
        if (0U != v) {
            if ((mesg->GetSequenceID() - timeDomainCtx->GetSyncSeqId())
                >= ptpCfg->domainPtp.globalTimeSequenceCounterJumpWidth) {
                return kRET_E3;
            }
        }
    }

    timeDomainCtx->SetSyncInTS(std::move(internal::TimeValue(inTs)));
    timeDomainCtx->SetSyncSeqId(seqId);
    // Reset timeout detection
    timeDomainCtx->SetStatusSynchronized();

    // Call message event callback function
    MessageEventHandler const mesgCb{syncMesgRecvCb_};
    if (nullptr != mesgCb) {
        mesgCb(domainId);
    }

    return 0;
}

/// @brief parse Followup message.
/// @param mesg message to parse;
/// @param inTs - message reception arrival timestamp.
/// @return 0 - success
/// @return <0 - failure
std::int32_t PtpManager::_disassembleFollowupMesg(message::Followup const *const mesg,
                                                  std::chrono::nanoseconds const &inTs) noexcept
{
    internal::TimeDomainId const domainId{mesg->GetDomainId()};
    // std::uint16_t seqId = mesg->GetSequenceID().
    std::shared_ptr< PtpContext > const ptpCtx{_getContext(domainId)};
    Configure const *const ptpCfg{_getConfig(domainId)};
    std::shared_ptr< timedomain::TDContext > const timeDomainCtx{timeDomainManager_->GetContext(domainId)};

    if (nullptr == ptpCtx) {
        LOG().Error() << "_disassembleFollowupMesg null pointer";
        return kRET_E1;
    }
    if (nullptr == ptpCfg) {
        LOG().Error() << "_disassembleFollowupMesg null pointer";
        return kRET_E1;
    }
    if (nullptr == timeDomainCtx) {
        LOG().Error() << "_disassembleFollowupMesg null pointer";
        return kRET_E1;
    }

    if (!ptpCtx->IsValid()) {
        LOG().Error() << "_disassembleFollowupMesg  ptpCtx->IsValid false";
        return kRET_E4;
    }
    // Master conflict detection
    if (ptpCfg->isMaster) {
        LOG().Warn() << "_disassembleFollowupMesg Master receive FollowupMesg";
        return 0;
    }

    // Verify the message; if not passed, return error.
    // FIXME: Verification has issues, needs correction
    if (true != mesg->Validate(ptpCtx)) {
        LOG().Error() << "_disassembleFollowupMesg Validate false";
        return 0;
    }

    /// @traceid{PRS_TS_00197} Timeout detection; after Followup timeout, discard Sync content
    /// @traceid{PRS_TS_00025} Use globalTimeFollowupTimeout to detect Followup timeout; if timeout occurs, seqid should be reset.
    if (ptpCfg->domainPtp.globalTimeFollowupTimeout > 0) {
        if ((inTs - timeDomainCtx->GetSyncInTS().ToChrono()).count()
            > static_cast< std::int64_t >(isoft::util::TimeS2NS(ptpCfg->domainPtp.globalTimeFollowupTimeout))) {
            timeDomainCtx->SetSyncSeqId(0U);
            timeDomainCtx->SetStatusTimeout();
            return kRET_E2;
        }
    }

    /// @traceid{PRS_TS_00196} If the SeqID of Followup does not match that of Sync, ignore this message.
    if (mesg->GetSequenceID() != timeDomainCtx->GetSyncSeqId()) {
        return kRET_E3;
    }

    // If the Followup message comes from an offset time domain, ignore T1 and record the offset value.
    if (mesg->GetDomainId().IsOffsetDomain()) {
        message::SubTlvOFS const *const tlvOfs{mesg->GetSubTlvOFS()};
        timeDomainCtx->SetDomainId(domainId);
        timeDomainCtx->SetUserData(tlvOfs->GetUserData());
        /// @traceid{PRS_TS_00110} offset time value
        timeDomainCtx->SetOffsetTS(tlvOfs->GetOffsetTS());
        /// @traceid{PRS_TS_00213} If MessageCompliance is FALSE and there is an OFS SubTLV in StatusSubTLV,
        /// then the status should be synchronized to TimeBase.
        // 0 SyncToGlobalTimeMaster, 1 SyncTo SubDomain
        if (0U == tlvOfs->GetStatus()) {
            timeDomainCtx->SetStatusSynchronized();
        } else {
            timeDomainCtx->SetStatusSyncToGateway();
        }
    } else {
        // E2E T1
        timeDomainCtx->SetSyncEnTS(mesg->GetPreciseOriginTimeStamp());
        // The correctionfield in the message is in ScaledNs; when using, divide by 2^16.  11.2.15.2.3(802.1as-2011)
        timeDomainCtx->SetCorrectionField(mesg->GetCorrectionField() >> kTS_NUM_16);
        _calculateOffset(domainId);
        internal::TimeValue globaltime;
        std::chrono::nanoseconds const correctionNs{mesg->GetCorrectionField() >> kTS_NUM_16};
        globaltime.FromChrono(mesg->GetPreciseOriginTimeStamp().ToChrono() + correctionNs
                              + timeDomainCtx->GetDelayTS().ToChrono());
        internal::TimeValue localGlobal{
            timeDomainCtx->Steady2GlobalTime(timeDomainCtx->GetSyncInTS().ToChrono()).time_since_epoch()};

        if (timeDomainManager_->GetConfig(domainId)->IsTestPrecisionMeasure()) {
            LOG().Info() << "_disassembleFollowupMesg, globaltime = " << globaltime.ToChrono().count()
                         << ", localGlobal=" << localGlobal.ToChrono().count()
                         << ", offset=" << (localGlobal.ToChrono().count() - globaltime.ToChrono().count())
                         << ", master_originTimeStamp=" << timeDomainCtx->GetSyncEnTS().ToChrono().count();
        }
        timeDomainCtx->SetGlobalTimeStamp(globaltime, timeDomainCtx->GetSyncInTS());
        timeDomainCtx->SetDomainId(domainId);
        timeDomainCtx->SetTlSyncTime(localGlobal);

        ///////////////////// Parse TLV ///////////////////////////
        /// @traceid{PRS_TS_00106} If MessageCompliance is FALSE, the Slave should check whether AUTOSAR TLV exists.
        /// @traceid{PRS_TS_00156} If MessageCompliance is FALSE, the status should be checked from the Status
        /// SubTLV of AUTOSAR TLV and synchronized to TimeBase.
        if (false == ptpCfg->domainPtp.messageCompliance) {
            message::SubTlvUserData const *const subTlvUserData{mesg->GetSubTlvUserData()};
            if (nullptr != subTlvUserData) {
                timeDomainCtx->SetUserData(subTlvUserData->GetUserData());
            }
            message::SubTlvStatus const *const subTlvStatus{mesg->GetSubTlvStatus()};
            if (nullptr != subTlvStatus) {
                /// @traceid{PRS_TS_00156} If MessageCompliance is FALSE, the status should be checked from the Status
                /// SubTLV of AUTOSAR TLV and synchronized to TimeBase.
                if (0U == subTlvStatus->GetStatus()) {
                    timeDomainCtx->SetStatusSynchronized();
                } else {
                    timeDomainCtx->SetStatusSyncToGateway();
                }
            } else {
                /// @traceid{PRS_TS_00212} If MessageCompliance is FALSE,
                /// and StatusSubTLV does not exist, then the SYNC_TO_GATEWAY bit of TimeBase status should be 0. (Actually the whole byte is 0)
                timeDomainCtx->SetStatusSynchronized();
            }
        } else {
            /// @traceid{PRS_TS_00211}
            /// If MessageCompliance is TRUE, then the SYNC_TO_GATEWAY bit of TimeBase status should be 0. (Actually the whole byte is 0)
            timeDomainCtx->SetStatusSynchronized();
        }
    }

    // Call message event callback function
    // Although the Followup message is not an event message, it brings back the precise sending time of the Sync message, so the callback function is meaningful.
    MessageEventHandler const mesgCb{followUpMesgRecvCb_};
    if (nullptr != mesgCb) {
        mesgCb(domainId);
    }

    // Call time event callback function
    TimeEventHandler const timeCb{timeSyncFinishedCb_};
    if (nullptr != timeCb) {
        timeCb(domainId);
    }
    if (mesg->GetDomainId().IsSyncDomain()) {
        _syncExternalClock(domainId);
    }
    return 0;
}

/// @brief parse PdelayRespFollowup message.
/// @param mesg message to parse;
/// @param inTs - message reception arrival timestamp.
/// @return 0 - success
/// @return <0 - failure
std::int32_t PtpManager::_disassemblePdelayRespFollowupMesg(message::PdelayRespFollowup const *const mesg,
                                                            std::chrono::nanoseconds const &inTs) noexcept
{
    internal::TimeDomainId const domainId{mesg->GetDomainId()};
    // std::uint16_t seqId = mesg->GetSequenceID().
    std::shared_ptr< PtpContext > const ptpCtx{_getContext(domainId)};
    Configure const *const ptpCfg{_getConfig(domainId)};
    std::shared_ptr< timedomain::TDContext > const timeDomainCtx{timeDomainManager_->GetContext(domainId)};

    if (nullptr == ptpCtx) {
        return kRET_E1;
    }
    if (nullptr == ptpCfg) {
        return kRET_E1;
    }
    if (nullptr == timeDomainCtx) {
        return kRET_E1;
    }

    if (!ptpCtx->IsValid()) {
        return kRET_E4;
    }
    /// @traceid{PRS_TS_00195} Timeout detection; after RespFollowup timeout, discard Resp content
    /// @traceid{PRS_TS_00164} Use pdelayRespAndRespFollowupTimeout to detect Resp/RespFlp timeout.
    /// @traceid{PRS_TS_00210} After timeout occurs, any Resp/RespFlp should be ignored until a new Req is sent.
    if (ptpCfg->domainPtp.pdelayRespAndRespFollowupTimeout > 0) {
        // If Resp has already timed out, ignore this message
        if (timeDomainCtx->IsPdelayRespTimeout()) {
            return kRET_E2;
        }
        // RespFollowup timeout
        if ((inTs - timeDomainCtx->GetPdelayRespInTS().ToChrono()).count()
            > static_cast< std::int64_t >(isoft::util::TimeS2NS(ptpCfg->domainPtp.pdelayRespAndRespFollowupTimeout))) {
            timeDomainCtx->SetPdelayRespFlpTimeout(true);
            return kRET_E3;
        }
    }

    ////////// Verification /////////////
    /// @traceid{PRS_TS_00194} If the SeqID of respFollowup does not match that of req, ignore this message.
    if (mesg->GetSequenceID() != timeDomainCtx->GetPdelaySeqId()) {
        return kRET_E4;
    }

    // P2P T3
    timeDomainCtx->SetPdelayRespEnTS(mesg->GetResponseOriginTimeStamp());
    /// @traceid{PRS_TS_00004} If timeout, use the previous delay value.
    _calculateDelay(domainId);

    // Call message event callback function
    MessageEventHandler const mesgCb{pdelayRespFlpMesgRecvCb_};
    if (nullptr != mesgCb) {
        mesgCb(domainId);
    }

    // Call time event callback function
    TimeEventHandler const timeCb{pdelayFinishedCb_};
    if (nullptr != timeCb) {
        timeCb(domainId);
    }

    return 0;
}

/// @brief parse PdelayResp message.
/// @param mesg message to parse;
/// @param inTs - message reception arrival timestamp.
/// @return 0 - success
/// @return <0 - failure
std::int32_t PtpManager::_disassemblePdelayRespMesg(message::PdelayResp const *const mesg,
                                                    std::chrono::nanoseconds const &inTs) noexcept
{
    internal::TimeDomainId const domainId{mesg->GetDomainId()};
    // std::uint16_t seqId = mesg->GetSequenceID().
    std::shared_ptr< PtpContext > const ptpCtx{_getContext(domainId)};
    Configure const *const ptpCfg{_getConfig(domainId)};
    std::shared_ptr< timedomain::TDContext > const timeDomainCtx{timeDomainManager_->GetContext(domainId)};

    if (nullptr == ptpCtx) {
        return kRET_E1;
    }
    if (nullptr == ptpCfg) {
        return kRET_E1;
    }
    if (nullptr == timeDomainCtx) {
        return kRET_E1;
    }
    if (!ptpCtx->IsValid()) {
        return kRET_E4;
    }
    ////////// Verification /////////////
    /// @traceid{PRS_TS_00192} If the SeqID of resp does not match that of req, ignore this message.
    if (mesg->GetSequenceID() != timeDomainCtx->GetPdelaySeqId()) {
        return kRET_E2;
    }

    /// @traceid{PRS_TS_00193} Timeout detection; after timeout, ignore Resp
    /// @traceid{PRS_TS_00164} Use pdelayRespAndRespFollowupTimeout to detect Resp/RespFlp timeout.
    /// @traceid{PRS_TS_00210} After timeout occurs, any Resp/RespFlp should be ignored until a new Req is sent.
    if (ptpCfg->domainPtp.pdelayRespAndRespFollowupTimeout > 0) {
        // Resp timeout
        if ((inTs - timeDomainCtx->GetPdelayReqEnTS().ToChrono()).count()
            > static_cast< std::int64_t >(isoft::util::TimeS2NS(ptpCfg->domainPtp.pdelayRespAndRespFollowupTimeout))) {
            timeDomainCtx->SetPdelayRespTimeout(true);
            return kRET_E3;
        }
    }

    // P2P T4
    timeDomainCtx->SetPdelayRespInTS(std::move(internal::TimeValue(inTs)));
    // P2P T2
    timeDomainCtx->SetPdelayReqInTS(mesg->GetRequestReceiptTimeStamp());

    // Call message event callback function
    MessageEventHandler const mesgCb{pdelayRespMesgRecvCb_};
    if (nullptr != mesgCb) {
        mesgCb(domainId);
    }

    return 0;
}

/// @brief parse PdelayReq message.
/// @param mesg message to parse
/// @param inTs - message reception arrival timestamp.
/// @return 0 - success
/// @return <0 - failure
std::int32_t PtpManager::_disassemblePdelayReqMesg(message::PdelayReq const *const mesg,
                                                   std::chrono::nanoseconds const &inTs) noexcept
{
    internal::TimeDomainId const domainId{mesg->GetDomainId()};
    std::uint16_t const seqId{mesg->GetSequenceID()};
    std::shared_ptr< PtpContext > const ptpCtx{_getContext(domainId)};
    Configure const *const ptpCfg{_getConfig(domainId)};
    std::shared_ptr< timedomain::TDContext > const timeDomainCtx{timeDomainManager_->GetContext(domainId)};
    MessageEventHandler mesgCb;

    if (nullptr == ptpCtx) {
        return kRET_E1;
    }
    if (nullptr == ptpCfg) {
        return kRET_E1;
    }
    if (nullptr == timeDomainCtx) {
        return kRET_E1;
    }

    if (!ptpCtx->IsValid()) {
        return kRET_E4;
    }
    // If the current ECU is not the Master of this time domain, ignore any PdelayReq packet.
    if (true != ptpCfg->isMaster) {
        return kRET_E2;
    }

    /// @traceid{PRS_TS_00012} If GlobalTImePdelayRespEnable is TRUE, then upon receiving PdelayReq, respond with PdelayResp.
    /// @traceid{PRS_TS_00143} If GlobalTImePdelayRespEnable is FALSE, then ignore PdelayReq.
    // If responding to PdelayReq is disabled, ignore PdelayReq packets.
    if (true != ptpCfg->domainPtp.globalTimePdelayRespEnable) {
        return kRET_E3;
    }

    // P2P T2
    timeDomainCtx->SetPdelayReqInTS(std::move(internal::TimeValue(inTs)));
    timeDomainCtx->SetPdelaySeqId(seqId);

    // Call message event callback function
    mesgCb = pdelayReqMesgRecvCb_;
    if (nullptr != mesgCb) {
        mesgCb(domainId);
    }

    // Send PdelayResp
    if (0 != _sendPdelayRespMessage(mesg, domainId)) {
        LOG().Error() << "_sendPdelayRespMessage(" << domainId.ToUint8() << ")";
        return kRET_E4;
    }

    // Call message event callback function
    mesgCb = pdelayRespMesgSendCb_;
    if (nullptr != mesgCb) {
        mesgCb(domainId);
    }

    // Send PdelayRespFollowup
    if (0 != _sendPdelayRespFollowupMessage(mesg, domainId)) {
        LOG().Error() << "_sendPdelayRespFollowupMessage(" << domainId.ToUint8() << ")";
        return kRET_E5;
    }

    // Although the PdelayRespFollowup message is not an event message, SWS requires a callback function after sending RespFollowup.
    mesgCb = pdelayRespFlpMesgSendCb_;
    if (nullptr != mesgCb) {
        mesgCb(domainId);
    }

    return 0;
}

/// @brief according to configuration, synchronize ptp external system clock and phc clock
/// @param domainId - time domain ID
/// @returns none
void PtpManager::_syncExternalClock(internal::TimeDomainId const &domainId) noexcept
{
    Configure const *const ptpCfg{_getConfig(domainId)};
    std::shared_ptr< timedomain::TDContext > const timeDomainCtx{timeDomainManager_->GetContext(domainId)};
    /// slave slave did not call system's adjusttime, need to observe the effect
    if ((nullptr != ptpCfg) && (false == ptpCfg->isMaster) && (nullptr != timeDomainCtx)) {
        if (timeDomainManager_->GetConfig(domainId)->IsSyncGlobalToPhc()) {
            ClockAdjust type{ClockAdjust::kAdjustPhc};
            if ((timeDomainManager_->GetConfig(domainId)->IsSyncGlobalToSystem())) {
                type = ClockAdjust::kAdjustPhcSystem;
            }
            std::chrono::nanoseconds tp{timeDomainCtx->GetSyncInTS().ToChrono().count()};
            nmManager_->AdjustClockTime(type, timeDomainCtx->Steady2GlobalTime(tp).time_since_epoch().count(),
                                        ptpCfg->etherconnector);
        }
    } else if ((nullptr != ptpCfg) && (true == ptpCfg->isMaster) && (nullptr != timeDomainCtx)) {
        if (timeDomainManager_->GetConfig(domainId)->IsSyncGlobalToPhc()) {
            std::chrono::nanoseconds tp{timeDomainCtx->GetSyncEnTS().ToChrono().count()};
            nmManager_->AdjustClockTime(ClockAdjust::kAdjustPhc,
                                        timeDomainCtx->Steady2GlobalTime(tp).time_since_epoch().count(),
                                        ptpCfg->etherconnector);
        }
    }
}
}  // namespace ptp
}  // namespace internal
}  // namespace tsync
}  // namespace ara