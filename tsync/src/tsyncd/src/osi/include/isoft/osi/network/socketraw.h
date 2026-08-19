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
/// @file       socketraw.h
/// @brief      OSI network module raw socket class
/// @details
/// @date       2023-01-09
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#ifndef ISOFT_OSI_NETWORK_SOCKET_RAW_H_
#define ISOFT_OSI_NETWORK_SOCKET_RAW_H_

#include <ara/core/vector.h>
#include <isoft/naicpp/global_evloop.h>

#include <chrono>
#include <cstdint>
#include <functional>
#include <memory>

#include "adaptivekalman.h"
#include "ara/tsync/internal/common.h"
#include "ara/tsync/internal/config/common.h"
#include "isoft/osi/network/ether.h"
#include "isoft/osi/network/phcclock.h"
#include "isoft/osi/network/sysclock.h"
namespace isoft {
namespace osi {
namespace network {
using ara::tsync::internal::ClockAdjust;
/// @brief raw socket class
class SocketRaw final
{
    /// @brief kPacketBuffSize constant declaration
    static constexpr std::size_t kPacketBuffSize{1600U};

    /// @brief kControlBuffSize constant declaration
    static constexpr std::size_t kControlBuffSize{256U};

    /// @brief kIovlen constant declaration
    static constexpr std::size_t kIovlen{60U};

    /// @brief kMaxTryCnt constant declaration
    static constexpr std::uint8_t kMaxTryCnt{40U};

    /// @brief kSleepStep constant declaration
    static constexpr std::uint32_t kSleepStep{30U};

    /// @brief kRecvSleep constant declaration
    static constexpr std::uint32_t kRecvSleep{1000U};

    /// @brief kEtherHeaderLen constant declaration
    static constexpr std::size_t kEtherHeaderLen{44U};

    /// @brief kMSgTypeOffset constant declaration
    static constexpr std::size_t kMSgTypeOffset{14U};

    /// @brief kRecvTryCnt constant declaration
    static constexpr std::uint8_t kRecvTryCnt{10U};

    /// @brief kMsgTypeBits constant declaration
    static constexpr std::uint8_t kMsgTypeBits{0x0FU};

public:
    /// @brief time domain type definition
    using TSTimeStampTypeEnum = ara::tsync::internal::config::TSTimeStampTypeEnum;

public:
    /// @brief constructor
    SocketRaw() = default;

    /// @brief destructor
    ~SocketRaw() = default;

    /// @brief copy constructor is prohibited
    /// @param other - other object
    SocketRaw(SocketRaw const &other) = delete;

    /// @brief copy is prohibited
    /// @param other - other object
    /// @return reference to class object
    SocketRaw &operator=(SocketRaw const &other) = delete;

    /// @brief move constructor
    /// @param other an existing rvalue reference.
    SocketRaw(SocketRaw &&) noexcept = default;

    /// @brief '=' operator overload
    /// @param other an existing rvalue reference.
    /// @return reference to class object
    SocketRaw &operator=(SocketRaw &&) &noexcept = default;

    /// @brief open raw socket
    /// @param deviceName - network device name
    /// @param stampType - timestamp type
    /// @param multMacVec - set of multicast MAC addresses for all time domains on this network card
    /// @param syncGlobalToPhc - whether to synchronize the global clock to the phc clock
    /// @return opened socket file descriptor; <0 indicates failure.
    std::int32_t Open(std::string const &deviceName,
                      TSTimeStampTypeEnum const stampType,
                      ara::core::Vector< Ether::Address > const &multMacVec,
                      bool syncGlobalToPhc) noexcept;

    /// @brief close raw socket
    /// @return 0 - success
    /// @return <0 - failure
    std::int32_t Close() noexcept;

    /// @brief send network data packet.
    /// @param mesg - message buffer to send.
    /// @param mesgSize - length of message to send.
    /// @param sendTimeStamp - system timestamp at sending completion, returned to user.
    /// @param bEventMsg - whether it is an event message
    /// @return 0 - success
    /// @return <0 - failure, and set errno
    std::int32_t Send(const void *const mesg,
                      std::uint16_t const mesgSize,
                      std::chrono::nanoseconds &sendTimeStamp) noexcept;

    /// @brief receive network data packet
    /// @param mesg received message buffer.
    /// @param mesgSize - message buffer length, will be modified to actual length after reception.
    /// @param recvTimeStamp - system timestamp at reception, returned to user.
    /// @return 0 - no valid message, >0 valid message
    /// @return <0 - failure
    std::int32_t Recv(std::uint8_t *const mesg,
                      std::uint16_t &mesgSize,
                      std::chrono::nanoseconds &recvTimeStamp) noexcept;

    /// @brief expected phc clock value when Sync is received, used to adjust phc clock
    /// @param type - clock type
    /// @param timeExpect - expected phc clock value
    /// @return 0 - success
    /// @return <0 - failure, and set errno
    std::int32_t AdjustClockTime(ClockAdjust type, std::int64_t const timeExpect) noexcept;

private:
    /// @brief get the most recent network send/receive timestamp
    /// @param timeStamp - timestamp.
    /// @param savePhcOriginStamp - whether to record the original phc timestamp
    /// @return 0 - success
    /// @return <0 - failure
    std::int32_t _getIoTimeStamp(std::chrono::nanoseconds &timeStamp, bool savePhcOriginStamp = false) noexcept;

    /// @brief initialize network configuration, software timestamp
    /// @return 0 - success
    /// @return <0 - failure
    std::int32_t _initConfigSoftStamping() const noexcept;

    /// @brief initialize network configuration, hardware timestamp
    /// @return 0 - success
    /// @return <0 - failure
    std::int32_t _initConfigHardwareStamping() noexcept;

    /// @brief determine whether the most recently sent message is an event type message
    /// @return true for event type message
    bool _isEventMSg() const noexcept;

    /// @brief measure the deviation between phc clock and system clock
    void _measureSysOffset() noexcept;

private:
    /// @name ifName_ - network device name
    std::string ifName_;

    /// @name ifIndex_ - network device index number
    std::int32_t ifIndex_{0};

    /// @name sockfd_ - socket file descriptor
    std::int32_t sockfd_{-1};

    /// @name multiCastAddrVec_ - set of multicast MAC addresses used for communication
    ara::core::Vector< Ether::Address > multiCastAddrVec_;

    /// @name stampType_ - whether to use software timestamp or hardware timestamp
    TSTimeStampTypeEnum stampType_{TSTimeStampTypeEnum::kSoftware};

    /// @name phcClock_
    std::shared_ptr< PhcClock > phcClock_{nullptr};

    /// @name sysClock_
    std::shared_ptr< SysClock > sysClock_{nullptr};
    /// @name updatehwClockTimer_ phc measurement timer
    isoft::naicpp::EvLoop::TimerPtr updatehwClockTimer_{nullptr};

    /// @name offset_ deviation between phc clock and system clock in ns
    int64_t offset_{0};

    /// @name lastSyncSys_ system clock at last synchronization
    int64_t lastSyncSystem_{0};

    /// @name sysToSteadyoffset_ deviation between system utc clock and steady clock in ns
    int64_t sysToSteadyoffset_{0};

    /// @name sysToSteadyoffset2_ deviation between system utc clock and steady clock in ns
    int64_t sysToSteadyoffset2_{0};

    /// @name lastMsgSequenceId_ sequence number of the most recently sent ptp message
    std::uint16_t lastMsgSequenceId_{0};

    /// @name lastEventMsgType_  type of the most recently sent message
    std::uint16_t lastEventMsgType_{0};

    /// @name lastRecvEventMsgType_  type of the most recently received message
    std::uint16_t lastRecvEventMsgType_{0};

    /// @name syncGlobalToPhc - whether to synchronize the global clock to the phc clock
    bool syncGlobalToPhc_{false};

    /// @name syncPhcTimeStamp_  phc timestamp of the most recently received sync message

    int64_t phcTimeStamp_{0};
    AdaptiveClockKalman offsetFilter_{};
};  /// class SocketRaw

}  // namespace network
}  // namespace osi
}  // namespace isoft

#endif  /// ISOFT_OSI_NETWORK_SOCKET_RAW_H_
