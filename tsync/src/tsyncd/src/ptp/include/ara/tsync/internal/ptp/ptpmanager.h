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
/// @file       ptpmanager.h
/// @brief      PTP management class
/// @details
/// @date       2023-01-11
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// export_level=/TimeSync/Ptp
/// module_path=/TimeSync/Ptp
/// @endcode
///
/// ================================================================

#ifndef ARA_TSYNC_INTERNAL_PTP_MANAGER_H_
#define ARA_TSYNC_INTERNAL_PTP_MANAGER_H_

#include <ara/core/map.h>
#include <isoft/naicpp/global_evloop.h>

#include <chrono>
#include <cstdint>
#include <memory>

#include "ara/tsync/internal/config/configmanager.h"
#include "ara/tsync/internal/ptp/event.h"
#include "ara/tsync/internal/ptp/message/followup.h"
#include "ara/tsync/internal/ptp/message/header.h"
#include "ara/tsync/internal/ptp/message/pdelayreq.h"
#include "ara/tsync/internal/ptp/message/pdelayresp.h"
#include "ara/tsync/internal/ptp/message/pdelayrespfollowup.h"
#include "ara/tsync/internal/ptp/message/sync.h"
#include "ara/tsync/internal/ptp/networkmanager.h"
#include "ara/tsync/internal/ptp/ptpcontext.h"
#include "ara/tsync/internal/timedomain/manager.h"

namespace ara {
namespace tsync {
namespace internal {
namespace ptp {

/// @brief time precision measurement protocol module management class
class PtpManager final
{
    /// @brief kClockIdentByte4 constant declaration
    constexpr static std::uint8_t kClockIdentByte3{0xFFU};

    /// @brief kClockIdentByte4 constant declaration
    constexpr static std::uint8_t kClockIdentByte4{0xFEU};

public:
    /// @brief constructor
    PtpManager() = default;

    /// @brief destructor
    ~PtpManager() noexcept;

    /// @brief copy constructor is prohibited
    /// @param other - other object
    PtpManager(PtpManager const &other) = delete;

    /// @brief copy is prohibited
    /// @param other - other object
    /// @return reference to class object
    PtpManager &operator=(PtpManager const &other) = delete;

    /// @brief move constructor
    /// @param other an existing rvalue reference.
    PtpManager(PtpManager &&other) noexcept = default;

    /// @brief '=' operator overload
    /// @param other an existing rvalue reference.
    /// @return reference to class object
    PtpManager &operator=(PtpManager &&other) &noexcept = default;

    /// @brief create Ptp manager
    /// @param timeDomain - time domain management handle
    /// @param networkmgr - network management handle
    /// @param configManager - configuration manager handle
    /// @param bAllowDisPach - whether to allow time domain master to distribute time by default
    /// @return handle, or nullptr.
    static std::unique_ptr< PtpManager > CreateManager(std::shared_ptr< timedomain::TDManager > const &timeDomain,
                                                       std::shared_ptr< ptp::NetworkManager > const &networkmgr,
                                                       std::shared_ptr< config::ConfigManager > const &configManager,
                                                       bool const bAllowDisPach) noexcept;

    /// @brief register message event callback function, called when the message is received.
    ///         Multiple calls will overwrite the old callback function.
    /// @param type - message type
    /// @param cb - message event callback function, nullptr means cancel.
    void OnMessageRecv(MessageEventType const &type, MessageEventHandler const &cb) noexcept;

    /// @brief register message event callback function, called when the message is sent.
    ///         Multiple calls will overwrite the old callback function.
    /// @param type - message type
    /// @param cb - message event callback function, nullptr means cancel.
    void OnMessageSend(MessageEventType const &type, MessageEventHandler const &cb) noexcept;

    /// @brief register time event callback function, called when the event occurs
    /// @param type - time event type
    /// @param cb - time event callback function
    void OnTimeEvent(TimeEventType const &type, TimeEventHandler const &cb) noexcept;

    /// @brief set time
    ///         This function will immediately send Sync message and FollowUp message.
    /// @param domainId - time domain ID
    /// @return 0 - success
    /// @return <0 - failure
    std::int32_t SetTime(internal::TimeDomainId const &domainId) noexcept;

private:
    /// @brief distribute time synchronization information
    ///         This function will immediately send Sync message and FollowUp message.
    /// @param domainId - time domain ID
    /// @return 0 - success
    /// @return <0 - failure
    std::int32_t _dispatchTime(internal::TimeDomainId const &domainId) noexcept;

    /// @brief get PTP context under the specified time domain
    /// @param domainId - time domain ID
    /// @returns PTP context address, or nullptr.
    std::shared_ptr< PtpContext > _getContext(internal::TimeDomainId const &domainId) noexcept
    {
        ara::core::Map< std::uint8_t, std::shared_ptr< PtpContext > >::iterator const domainCtx{
            contextSet_.find(domainId.ToUint8())};
        if (contextSet_.end() == domainCtx) {
            return std::shared_ptr< PtpContext >{nullptr};
        }
        return contextSet_[domainId.ToUint8()];
    }

    /// @brief get PTP configuration under the specified time domain
    /// @param domainId - time domain ID
    /// @returns PTP configuration address, or nullptr.
    Configure const *_getConfig(internal::TimeDomainId const &domainId) noexcept
    {
        std::shared_ptr< PtpContext > ctx{_getContext(domainId)};
        if (nullptr == ctx) {
            return nullptr;
        }
        return &ctx->PtpConfig();
    }

    /// @brief initialize
    /// @param timeDomain - time domain management handle
    /// @param networkmgr - network management handle
    /// @param configManager - configuration manager handle
    /// @param bAllowDisPach - whether to allow time domain master to distribute time by default
    /// @return 0 - success
    /// @return <0 - failure
    std::int32_t _init(std::shared_ptr< timedomain::TDManager > const &timeDomain,
                       std::shared_ptr< ptp::NetworkManager > const &networkmgr,
                       std::shared_ptr< config::ConfigManager > const &configManager,
                       bool const bAllowDisPach) noexcept;

    /// @brief initialize context
    /// @return 0 - success
    /// @return <0 - failure
    std::int32_t _initContext() noexcept;

    /// @brief initialize timer
    /// @return 0 - success
    /// @return <0 - failure
    std::int32_t _initTimer() noexcept;

    /// @brief send message
    /// @param destAddr - destination address
    /// @param data - data
    /// @param size - data length
    /// @param ptpCfg - ptpCfg configuration
    /// @return send timestamp
    std::chrono::nanoseconds _sendMessage(Network::Address const &destAddr,
                                          void const *const data,
                                          std::uint16_t const size,
                                          Configure const *const ptpCfg) const noexcept;

    /// @brief send Sync message. Data is obtained from the corresponding time domain context.
    /// @param domainId time domain ID. In compatibility mode, domainId can only be 0; in AUTOSAR mode, domainId ranges 0 - 15;
    /// @return 0 - success
    /// @return <0 - failure
    /// @traceid{PRS_TS_00104} Assembly and verification of time synchronization packets.
    std::int32_t _sendSyncMessage(internal::TimeDomainId const &domainId) noexcept;

    /// @brief send Followup message. Data is obtained from the corresponding time domain context.
    /// @param domainId time domain ID;
    /// @return 0 - success
    /// @return <0 - failure
    std::int32_t _sendFollowupMessage(internal::TimeDomainId const &domainId) noexcept;

    /// AUTOSAR does not specify a special protocol format for messages other than SyncFollowup, so all subsequent packet types follow the standard IEEE802.1as.
    /// AUTOSAR will use all P2P messages for path delay calculation.

    /// @brief send PdelayRequest message. Data is obtained from the corresponding time domain context.
    ///     PTP protocol specification requires that currently only the path delay from the local machine to time domain 0 needs to be measured.
    /// @param domainId time domain ID. In compatibility mode, domainId can only be 0; in AUTOSAR mode, domainId ranges 0 - 15;
    /// @return 0 - success
    /// @return <0 - failure
    std::int32_t _sendPdelayReqMessage(internal::TimeDomainId const &domainId) noexcept;

    /// @brief send PdelayResponse message. Data is obtained from the corresponding time domain context.
    /// @param preqmsg delay request request message
    /// @param domainId time domain ID;
    /// @return 0 - success
    /// @return <0 - failure
    std::int32_t _sendPdelayRespMessage(message::PdelayReq const *const preqmsg,
                                        internal::TimeDomainId const &domainId) noexcept;

    /// @brief send PdelayResponseFollowup message. Data is obtained from the corresponding time domain context.
    /// @param preqmsg delay request request message
    /// @param domainId time domain ID;
    /// @return 0 - success
    /// @return <0 - failure
    std::int32_t _sendPdelayRespFollowupMessage(message::PdelayReq const *const preqmsg,
                                                internal::TimeDomainId const &domainId) noexcept;

    /// @brief parse Sync message.
    /// @param mesg message to parse;
    /// @param inTs - message reception arrival timestamp.
    /// @return 0 - success
    /// @return <0 - failure
    std::int32_t _disassembleSyncMesg(message::Sync const *const mesg, std::chrono::nanoseconds const &inTs) noexcept;

    /// @brief parse Followup message.
    /// @param mesg message to parse;
    /// @param inTs - message reception arrival timestamp.
    /// @return 0 - success
    /// @return <0 - failure
    std::int32_t _disassembleFollowupMesg(message::Followup const *const mesg,
                                          std::chrono::nanoseconds const &inTs) noexcept;

    /// @brief parse PdelayRespFollowup message.
    /// @param mesg message to parse;
    /// @param inTs - message reception arrival timestamp.
    /// @return 0 - success
    /// @return <0 - failure
    std::int32_t _disassemblePdelayRespFollowupMesg(message::PdelayRespFollowup const *const mesg,
                                                    std::chrono::nanoseconds const &inTs) noexcept;
    /// @brief parse PdelayResp message.
    /// @param mesg message to parse;
    /// @param inTs - message reception arrival timestamp.
    /// @return 0 - success
    /// @return <0 - failure
    std::int32_t _disassemblePdelayRespMesg(message::PdelayResp const *const mesg,
                                            std::chrono::nanoseconds const &inTs) noexcept;

    /// @brief parse PdelayReq message.
    /// @param mesg message to parse
    /// @param inTs - message reception arrival timestamp.
    /// @return 0 - success
    /// @return <0 - failure
    std::int32_t _disassemblePdelayReqMesg(message::PdelayReq const *const mesg,
                                           std::chrono::nanoseconds const &inTs) noexcept;

    /// @brief network message asynchronous receive callback function.
    /// @param mesg - received message buffer.
    /// @param mesgSize - length of received message.
    /// @param sourceAddr - message source address.
    /// @param destAddr - message destination address.
    /// @param recvTimeStamp - system timestamp at reception, returned to user.
    void _networkRecvCb(void const *const mesg,
                        std::uint16_t const mesgSize,
                        Network::Address const &sourceAddr,
                        Network::Address const &destAddr,
                        std::chrono::nanoseconds const &recvTimeStamp) noexcept;

    /// @brief calculate offset, result stored in time synchronization context offset
    /// @param domainId - time domain ID
    /// @returns none
    /// @traceid{SWS_TS_00055}
    void _calculateOffset(internal::TimeDomainId const &domainId) noexcept;

    /// @brief calculate path delay, result stored in time synchronization context delay
    /// @param domainId - time domain ID
    /// @returns none
    void _calculateDelay(internal::TimeDomainId const &domainId) noexcept;

    /// @brief according to configuration, synchronize ptp external system clock and phc clock
    /// @param domainId - time domain ID
    /// @returns none
    void _syncExternalClock(internal::TimeDomainId const &domainId) noexcept;

private:
    /// @name mainLoop_ - main event loop handle
    std::shared_ptr< isoft::naicpp::EvLoop > mainLoop_{nullptr};

    /// @name timeDomainManager_ - time domain manager handle
    std::shared_ptr< timedomain::TDManager > timeDomainManager_{nullptr};

    /// @name nmManager_ - network manager handle
    std::shared_ptr< ptp::NetworkManager > nmManager_{nullptr};

    /// @name configManager_ - configuration manager handle
    std::shared_ptr< config::ConfigManager > configManager_{nullptr};

    ara::core::Map< std::uint8_t, std::shared_ptr< PtpContext > >
        /// @name contextSet_ - PTP context set
        contextSet_{};

    /// @name syncMesgSendCb_ message event callback function
    MessageEventHandler syncMesgSendCb_{};
    /// @name syncMesgRecvCb_ message event callback function
    MessageEventHandler syncMesgRecvCb_{};

    /// @name followUpMesgSendCb_ message event callback function
    MessageEventHandler followUpMesgSendCb_{};
    /// @name followUpMesgRecvCb_ message event callback function
    MessageEventHandler followUpMesgRecvCb_{};

    /// @name pdelayReqMesgSendCb_ message event callback function
    MessageEventHandler pdelayReqMesgSendCb_{};
    /// @name pdelayReqMesgRecvCb_ message event callback function
    MessageEventHandler pdelayReqMesgRecvCb_{};

    /// @name pdelayRespMesgSendCb_ message event callback function
    MessageEventHandler pdelayRespMesgSendCb_{};
    /// @name pdelayRespMesgRecvCb_ message event callback function
    MessageEventHandler pdelayRespMesgRecvCb_{};

    /// @name pdelayRespFlpMesgSendCb_ message event callback function
    MessageEventHandler pdelayRespFlpMesgSendCb_{};
    /// @name pdelayRespFlpMesgRecvCb_ message event callback function
    MessageEventHandler pdelayRespFlpMesgRecvCb_{};

    /// @name timeSyncFinishedCb_ time event callback function
    TimeEventHandler timeSyncFinishedCb_{};
    /// @name timeSyncTimeoutCb_ message event callback function
    TimeEventHandler timeSyncTimeoutCb_{};
    /// @name pdelayFinishedCb_ message event callback function
    TimeEventHandler pdelayFinishedCb_{};

    /// @name bAllowDisPach_ - whether to allow provider to send sync/followup messages without setting time
    bool bAllowDisPach_{false};

    /// @name domainOnConnectors_ - set of slaves configured for each network card's domain, used for measurement and application of pdelay
    /// If the domain is configured as master, it should not be added
    /// Start traversing the added ones, [0] is the smallest
    ara::core::Map< ara::core::String, ara::core::Vector< internal::TimeDomainId > > domainOnConnectors_{};
};  /// class Manager

}  // namespace ptp
}  // namespace internal
}  // namespace tsync
}  // namespace ara

#endif  /// ARA_TSYNC_INTERNAL_PTP_MANAGER_H_
