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
/// @file       socketraw.cpp
/// @brief      OSI network module raw socket class
/// @details
/// @date       2023-01-09
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#include <chrono>
#include <iostream>
#include <string>
// clang-format off
#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <linux/net_tstamp.h>
#include <linux/sockios.h>
#include <net/ethernet.h>
#include <net/if.h>
//#include <poll.h>.
#include <sys/ioctl.h>  //NOLINT
#include <sys/socket.h>
#include <sys/types.h>
#include <ctime>
#include <unistd.h>

#include <ara/core/steady_clock.h>  //NOLINT
#include "ara/tsync/internal/common.h"
#include "isoft/osi/network/device.h"
#include "isoft/osi/network/ether.h"
#include "isoft/util/time.h"
#include "ara/tsync/internal/log/logger.h"
#include "isoft/osi/network/socketraw.h"

// clang-format on
/// FIXME: struct cmsghdr and related structures have domain name inclusion issues, so wrap them in a layer.
namespace {

/// @brief log output
/// @return Logger object reference
static inline ara::tsync::internal::tslog::Logger &LOG() noexcept { return ara::tsync::internal::tslog::Log(); }

/// @brief read send/receive timestamps from kernel message
/// @param msg - received kernel message
/// @param stampType - timestamp type
/// @return raw timestamp
std::chrono::nanoseconds GetTimeStampFromMesg(
    struct msghdr *const msg, ara::tsync::internal::config::TSTimeStampTypeEnum const stampType) noexcept
{
    std::int32_t level{0};
    std::int32_t type{0};
    struct timespec *ts{nullptr};

    /// TODO(zhoubo): clang-tidy do not use C-style cast to convert between
    /// unrelated types
    for (struct cmsghdr *cmsg{CMSG_FIRSTHDR(msg)}; nullptr != cmsg; cmsg = CMSG_NXTHDR(msg, cmsg))  // NOLINT
    {
        type  = cmsg->cmsg_type;
        level = cmsg->cmsg_level;
        // std::cout << "LEVEL:" << std::dec << (int)level << "-" << std::dec <<
        // (int)SOL_SOCKET << std::endl std::cout << "TYPE: " << std::dec <<
        // (int)type << "-" << std::dec << (int)SO_TIMESTAMPING << std::endl
        if ((SOL_SOCKET == level) && (SO_TIMESTAMPING == type)) {
            if (cmsg->cmsg_len < ((sizeof(*ts)) * ara::tsync::internal::kTS_NUM_3)) {
                LOG().Error() << "short SO_TIMESTAMPING message, cmsg->cmsg_len=" << cmsg->cmsg_len
                              << ", shosizeof(*ts)" << sizeof(*ts);
                // return std::chrono::nanoseconds{0}.
            } else {
                ts = reinterpret_cast< struct timespec * >(CMSG_DATA(cmsg));
                break;
            }
        }
    }

    struct timespec *ret{nullptr};
    if ((nullptr != ts) && (ara::tsync::internal::config::TSTimeStampTypeEnum::kHardware == stampType)) {
        // Only event messages have hardware timestamps,
        ret = (ts + ara::tsync::internal::kTS_NUM_2);
        std::int64_t const t1{isoft::util::TimeSpec2Int64(ret)};
        // std::int64_t const t2{t1 - offset};
        return std::move(std::chrono::nanoseconds{t1});
    }
    // if ((nullptr != ts) && (ara::tsync::internal::config::TSTimeStampTypeEnum::kHardware == stampType)) {
    //     ret = &ts[0];
    //     std::int64_t const nanocounts{isoft::util::TimeSpec2Int64(ret)};
    //     std::chrono::nanoseconds const tp{nanocounts};

    //     return isoft::util::TimeSystem2Steady(tp);
    // }
    return std::chrono::nanoseconds(-1);
}

}  // namespace

namespace isoft {
namespace osi {
namespace network {

/// @brief open raw socket
/// @param deviceName - network device name
/// @param stampType - timestamp type
/// @param multMacVec - set of multicast MAC addresses for all time domains on this network card
/// @param syncGlobalToPhc - whether to synchronize the global clock to the phc clock
/// @return opened socket file descriptor; <0 indicates failure.
std::int32_t SocketRaw::Open(std::string const &deviceName,
                             TSTimeStampTypeEnum const stampType,
                             ara::core::Vector< Ether::Address > const &multMacVec,
                             bool syncGlobalToPhc) noexcept
{
    sockaddr_ll addr{};

    if (deviceName.empty()) {
        return ara::tsync::internal::kRET_E1;
    }

    ifName_           = deviceName;
    stampType_        = stampType;
    multiCastAddrVec_ = multMacVec;
    syncGlobalToPhc_  = syncGlobalToPhc;

    /// Get network card index number via network card name
    ifIndex_ = static_cast< std::int32_t >(::if_nametoindex(ifName_.c_str()));
    if (0 == ifIndex_) {
        return ara::tsync::internal::kRET_E2;
    }

    uint16_t const protolId{static_cast< uint16_t >(ETH_P_ALL)};
    sockfd_ = ::socket(AF_PACKET, SOCK_RAW, static_cast< std::int32_t >(htons(protolId)));
    if (-1 == sockfd_) {
        return ara::tsync::internal::kRET_E3;
    }

    static_cast< void >(::memset(&addr, 0, sizeof(addr)));
    // For raw sockets, bind only needs to specify sll_family
    addr.sll_family = static_cast< std::uint16_t >(AF_PACKET);
    // If sll_ifindex is not specified during bind, sendto must be used to specify it when sending.
    addr.sll_ifindex = ifIndex_;

    // bind will combine with the protocol type specified by the third argument of socket
    addr.sll_protocol = htons(static_cast< std::uint16_t >(ETH_P_1588));
    // addr.sll_protocol = htons(ETH_P_ALL).
    addr.sll_pkttype = static_cast< std::uint8_t >(PACKET_MULTICAST);

    if (-1 == ::bind(sockfd_, reinterpret_cast< struct sockaddr * >(&addr), sizeof(addr))) {
        std::ignore = ::close(sockfd_);
        return ara::tsync::internal::kRET_E4;
    }

    // Set multicast address
    // Can also be set using ioctl
    // struct ifr_hwaddr mcAddr
    // ioctl(sockfd_, SIOCADDMULTI, )

    packet_mreq mcReq{};
    mcReq.mr_ifindex = ifIndex_;
    // Receive packets from the specified multicast group
    for (auto const multiCastAddr : multiCastAddrVec_) {
        mcReq.mr_type = static_cast< std::uint16_t >(PACKET_MR_MULTICAST);
        mcReq.mr_alen = Ether::Address::kLength;
        multiCastAddr.CopyTo(static_cast< std::uint8_t * >(&mcReq.mr_address[0]));
        if (0 > ::setsockopt(sockfd_, SOL_PACKET, PACKET_ADD_MEMBERSHIP, &mcReq, sizeof(mcReq))) {
            std::string addrStr;
            multiCastAddr.ToStr(addrStr);
            std::ignore = ::close(sockfd_);
            LOG().Error() << "Open setsockopt err1, " << addrStr;
            return ara::tsync::internal::kRET_E1;
        }
    }

    // Receive specified unicast packets
    Ether::Address myEtherAddr;
    if (0 == GetMacAddress(ifName_, myEtherAddr)) {
        mcReq.mr_type = static_cast< std::uint16_t >(PACKET_MR_UNICAST);
        mcReq.mr_alen = Ether::Address::kLength;
        myEtherAddr.CopyTo(static_cast< std::uint8_t * >(&mcReq.mr_address[0]));
        std::ignore = mcReq.mr_address[0];
        if (0 > ::setsockopt(sockfd_, SOL_PACKET, PACKET_ADD_MEMBERSHIP, &mcReq, sizeof(mcReq))) {
            LOG().Error() << "Open setsockopt err " << ara::tsync::internal::GetErrString();
            std::ignore = ::close(sockfd_);
            return ara::tsync::internal::kRET_E1;
        }
    } else {
    }

    std::int32_t errCode{0};
    //   if (TSTimeStampTypeEnum::kSoftware == stampType_) {
    //     errCode = _initConfigSoftStamping();
    //   } else
    if (TSTimeStampTypeEnum::kHardware == stampType_) {
        errCode = _initConfigHardwareStamping();
    } else {
    }
    if (0 != errCode) {
        std::ignore = ::close(sockfd_);
        return errCode;
    }
    return sockfd_;
}

///
// Timestamp acquisition explanation
// url: https://www.kernel.org/doc/Documentation/networking/timestamping.txt
// The timestamp of data packet sending or receiving can be set using setsockopt.
//
// 1. Timestamp generation:
//      Through the third parameter of setsockopt, options are as follows:
//      SO_TIMESTAMP
//      SO_TIMESTAMPNS
//                      The two are almost the same, the only difference is precision. SO_TIMESTAMP obtains the time using struct
//                      timeval structure, maximum precision to microseconds; SO_TIMESTAMPNS uses struct
//                      timespec structure, precision to nanoseconds.
//                      Can only be used to get the timestamp of received packets, not for sending.
//      SO_TIMESTAMPING
//                      More powerful, can be used for both sending and receiving. Supports multiple time sources.
//  NOTE:   All TIMESTAMP support both OLD and NEW versions.
//  OLD supports 32-bit, NEW supports 64-bit. The system will choose the default version based on the current architecture. If you need to specify, you can explicitly write,
//  such as SO_TIMESTAMPNS_NEW. The 32-bit version can support up to 2038.
//
// 2. Time source:
//      Software and hardware timestamps can be obtained. Software timestamps are applied by the kernel, hardware timestamps by the network card (requires driver support).
//      SOF_TIMESTAMPING_SOFTWARE
//          SOF_TIMESTAMPING_RX_SOFTWARE    Get receive software timestamp
//          SOF_TIMESTAMPING_TX_SOFTWARE    Get send software timestamp
//      SOF_TIMESTAMPING_HARDWARE
//          SOF_TIMESTAMPING_RX_HARDWARE    Get receive hardware timestamp
//          SOF_TIMESTAMPING_TX_HARDWARE    Get send hardware timestamp
//  NOTE:   Enabling hardware timestamps requires setting via ioctl.

/// @brief initialize network configuration, software timestamp
/// @return 0 - success
/// @return <0 - failure
std::int32_t SocketRaw::_initConfigSoftStamping() const noexcept
{
    /// FIXME: Cannot use SO_TIMESTAMPING for both reception and sending simultaneously
    /// Basis: SO_TIMESTAMP can only be used for reception, SO_TIMESTAMPING can be used for both reception and sending.
    /// Phenomenon:
    ///       When SO_TIMESTAMPING is set for both sending and receiving, the later set will cause the earlier set to fail to bring back timestamps,
    ///       type is ING
    ///       When SO_TIMESTAMPNS is set for both sending and receiving, the sending will fail to bring back timestamps,
    ///       type is NS. Set SO_TIMESTAMPING for sending,
    ///       SO_TIMESTAMPNS for reception; regardless of order, both can bring back, but the type becomes NS.
    ///       Set SO_TIMESTAMPNS for sending,
    ///       SO_TIMESTAMPING for reception; regardless of order, sending fails to bring back timestamps, reception can bring back, but the type becomes NS.
    /// Conclusion: Use SO_TIMESTAMPING for sending, SO_TIMESTAMPNS for reception.
    /// BUG: On a single machine, an independent network card cannot obtain send timestamps,
    /// order irrelevant, regardless of whether receive timestamp is enabled.
    std::uint32_t flag{0U};
    std::int32_t opt{0};
    std::int32_t ret{0};

    /// Set to bring back send and receive timestamps
    opt  = SO_TIMESTAMPING;
    flag = (static_cast< std::uint32_t >(SOF_TIMESTAMPING_SOFTWARE))
           | (static_cast< std::uint32_t >(SOF_TIMESTAMPING_TX_SOFTWARE))
           | (static_cast< std::uint32_t >(SOF_TIMESTAMPING_RX_SOFTWARE));
    ret = ::setsockopt(sockfd_, SOL_SOCKET, opt, static_cast< const void * >(&flag), sizeof(flag));
    if (-1 == ret) {
        LOG().Error() << "_initConfigSoftStamping setsockopt software stamping err: "
                      << ara::tsync::internal::GetErrString();
        return ara::tsync::internal::kRET_E3;
    }

    return 0;
}

/// @brief initialize network configuration, hardware timestamp
/// @return 0 - success
/// @return <0 - failure
std::int32_t SocketRaw::_initConfigHardwareStamping() noexcept
{
    ifreq ifContent{};
    hwtstamp_config cfg{};
    std::uint32_t flag{0U};
    std::int32_t opt{0};
    std::int32_t ret{0};

    std::ignore = memset(&ifContent, 0, sizeof(ifContent));
    std::ignore = memset(&cfg, 0, sizeof(cfg));

    std::ignore = strncpy(static_cast< ara::tsync::internal::char8_t * >(&ifContent.ifr_name[0]), ifName_.c_str(),
                          sizeof(ifContent.ifr_name) - 1U);
    std::ignore = ifContent.ifr_name[0];
    ifContent.ifr_data = reinterpret_cast< ara::tsync::internal::char8_t * >(&cfg);
    cfg.tx_type        = HWTSTAMP_TX_ON;
    cfg.rx_filter      = HWTSTAMP_FILTER_PTP_V2_EVENT;
    ret                = ioctl(sockfd_, static_cast< std::uint64_t >(SIOCSHWTSTAMP), &ifContent);
    std::ignore        = cfg;
    if (0 > ret) {
        LOG().Error() << "ioctl HWTSTAMP_TX_ON failed:" << ara::tsync::internal::GetErrString();
        return ara::tsync::internal::kRET_E2;
    }

    /// FIXME: Cannot use SO_TIMESTAMPING for both reception and sending simultaneously
    /// Basis: SO_TIMESTAMP can only be used for reception, SO_TIMESTAMPING can be used for both reception and sending.
    /// Phenomenon:
    ///       When SO_TIMESTAMPING is set for both sending and receiving, the later set will cause the earlier set to fail to bring back timestamps,
    ///       type is ING
    ///       When SO_TIMESTAMPNS is set for both sending and receiving, the sending will fail to bring back timestamps,
    ///       type is NS. Set SO_TIMESTAMPING for sending,
    ///       SO_TIMESTAMPNS for reception; regardless of order, both can bring back, but the type becomes NS.
    ///       Set SO_TIMESTAMPNS for sending,
    ///       SO_TIMESTAMPING for reception; regardless of order, sending fails to bring back timestamps, reception can bring back, but the type becomes NS.
    /// Conclusion: Use SO_TIMESTAMPING for sending, SO_TIMESTAMPNS for reception.
    /// BUG: On a single machine, an independent network card cannot obtain send timestamps,
    /// order irrelevant, regardless of whether receive timestamp is enabled.

    /// Set to bring back receive timestamp & set to bring back send timestamp
    flag = (static_cast< std::uint32_t >(SOF_TIMESTAMPING_RAW_HARDWARE))
           | (static_cast< std::uint32_t >(SOF_TIMESTAMPING_RX_HARDWARE))
           | (static_cast< std::uint32_t >(SOF_TIMESTAMPING_TX_HARDWARE));
    opt = SO_TIMESTAMPING;

    ret = ::setsockopt(sockfd_, SOL_SOCKET, opt, static_cast< const void * >(&flag), sizeof(flag));
    if (-1 == ret) {
        return ara::tsync::internal::kRET_E3;
    }
    std::int32_t const flags{1};
    if (setsockopt(sockfd_, SOL_SOCKET, SO_SELECT_ERR_QUEUE, &flags, sizeof(flags)) < 0) {
        LOG().Error() << "_initConfigHardwareStamping setsockopt err_queue err: "
                      << ara::tsync::internal::GetErrString();
        return ara::tsync::internal::kRET_E4;
    }

    phcClock_ = std::make_shared< PhcClock >();
    if (nullptr == phcClock_) {
        LOG().Error() << "_initConfigHardwareStamping phcClock_ nullptr";
        return ara::tsync::internal::kRET_E5;
    }
    ret = phcClock_->Open(ifName_);
    if (0 != ret) {
        LOG().Error() << "_initConfigHardwareStamping phcClock_ Open failed";
        return ara::tsync::internal::kRET_E6;
    }
    _measureSysOffset();

    timespec tsReal1{};
    std::ignore = clock_gettime(CLOCK_REALTIME, &tsReal1);
    phcClock_->InitPhcClockValue(tsReal1.tv_sec * kTS_NSPERSEC + tsReal1.tv_nsec);
    timespec nowMonic{};
    timespec tsReal{};
    std::ignore = clock_gettime(CLOCK_MONOTONIC, &nowMonic);
    std::ignore = clock_gettime(CLOCK_REALTIME, &tsReal);
    std::int64_t const offsetNs2{tsReal.tv_sec * kTS_NSPERSEC + tsReal.tv_nsec - nowMonic.tv_sec * kTS_NSPERSEC
                                 - nowMonic.tv_nsec};
    if (0 == sysToSteadyoffset2_) {
        sysToSteadyoffset2_ = offsetNs2;
    }

    static_cast< void >(isoft::naicpp::GlobalGeneralEvLoop::Get()->MakeTimer(
        updatehwClockTimer_, (static_cast< std::int32_t >(isoft::util::TimeS2MS(kPHC_SYNC_OFFSET))),
        [this]() -> void { _measureSysOffset(); }));

    return ret;
}

/// @brief close raw socket
/// @return 0 - success
/// @return <0 - failure
std::int32_t SocketRaw::Close() noexcept
{
    phcClock_           = nullptr;
    updatehwClockTimer_ = nullptr;
    if (0 != ::close(sockfd_)) {
        return ara::tsync::internal::kRET_E1;
    }
    return 0;
}

/// @brief get the most recent network send/receive timestamp
/// @param timeStamp - timestamp.
/// @param savePhcOriginStamp - whether to record the original phc timestamp
/// @return 0 - success
/// @return <0 - failure
std::int32_t SocketRaw::_getIoTimeStamp(std::chrono::nanoseconds &timeStamp, bool savePhcOriginStamp) noexcept
{
    std::ignore = timeStamp;

    std::array< ara::tsync::internal::char8_t, kPacketBuffSize > packetBuff{};
    msghdr mesgHeader{};
    iovec msgIov{};

    std::array< ara::tsync::internal::char8_t, kControlBuffSize > contrlBuff{};
    std::int32_t recvFlags{0};
    ssize_t ret{0U};

    msgIov.iov_base = packetBuff.data();
    msgIov.iov_len  = kIovlen;

    static_cast< void >(::memset(&mesgHeader, 0, sizeof(mesgHeader)));
    mesgHeader.msg_iov        = &msgIov;
    mesgHeader.msg_iovlen     = 1U;
    mesgHeader.msg_name       = nullptr;
    mesgHeader.msg_namelen    = 0U;
    mesgHeader.msg_control    = contrlBuff.data();
    mesgHeader.msg_controllen = kControlBuffSize;

    recvFlags = MSG_ERRQUEUE;
    /// Alternative to poll. Because send timestamps may not be available immediately, need to poll.
    std::uint32_t tryCnt{0U};
    bool bMatch{false};
    std::uint16_t sequenceId{0U};
    std::uint8_t messageType{0U};
    /// During testing, maxTryCnt was 20, occasionally not returned. Increase it.
    std::uint8_t const maxTryCnt{kMaxTryCnt};
    while (tryCnt < maxTryCnt) {
        std::uint32_t i{0U};
        tryCnt++;
        static_cast< void >(::usleep(tryCnt * kSleepStep));
        while (true) {
            ret = ::recvmsg(sockfd_, &mesgHeader, recvFlags);
            /// Data received
            if (ret > 0) {
                break;
            }

            /// No data received, poll to read
            if (i > maxTryCnt) {
                return ara::tsync::internal::kRET_E1;
            }
            i++;
            /// Resource temporarily unavailable, read again
            if (ara::tsync::internal::GetErrNo() == EAGAIN) {
                static_cast< void >(::usleep(kSleepStep * i));  // sleep 1ms to avoid repeated reads
                continue;
                /// Other errors
            }
            LOG().Error() << "_getIoTimeStamp recvmsg ,err:" << ara::tsync::internal::GetErrString();
            return ara::tsync::internal::kRET_E2;
        }
        // test   send timestamp detection,

        sequenceId  = ntohs(*(reinterpret_cast< uint16_t * >(packetBuff.data() + kEtherHeaderLen)));
        messageType = *(packetBuff.data() + kMSgTypeOffset) & kMsgTypeBits;
        if ((sequenceId == lastMsgSequenceId_) && (lastEventMsgType_ == messageType)) {
            bMatch = true;
            break;
        }
    }

    if (!bMatch) {
        LOG().Error() << "_getIoTimeStamp wrong message ,messageType=" << messageType << ",sequenceId=" << sequenceId
                      << ",lastMsgSequenceId_=" << lastMsgSequenceId_ << ",lastEventMsgType_=" << lastEventMsgType_;
        return ara::tsync::internal::kRET_E8;
    }

    /// Parse time
    timeStamp = GetTimeStampFromMesg(&mesgHeader, stampType_);
    if (savePhcOriginStamp) {
        phcTimeStamp_ = timeStamp.count();
    }
    timeStamp   = timeStamp - std::chrono::nanoseconds{offset_ + sysToSteadyoffset2_ + kUTC_TAI_OFFSET_NS};
    std::ignore = mesgHeader;
    if (0 > timeStamp.count()) {
        return ara::tsync::internal::kRET_E4;
    }
    return 0;
}

/// @brief send network data packet.
/// @param mesg - message buffer to send.
/// @param mesgSize - length of message to send.
/// @param sendTimeStamp - system timestamp at sending completion, returned to user.
/// @return 0 - success
/// @return <0 - failure, and set errno
std::int32_t SocketRaw::Send(const void *const mesg,
                             std::uint16_t const mesgSize,
                             std::chrono::nanoseconds &sendTimeStamp) noexcept
{
    ssize_t ret{0};
    struct sockaddr_ll llAddr
    {
    };
    std::ignore = sendTimeStamp;
    /// For raw sockets, sendto only needs to specify ifindex,
    /// but if ifindex was specified during bind, then sendto does not need to specify it.
    static_cast< void >(::memset(&llAddr, 0, sizeof(llAddr)));
    llAddr.sll_family   = static_cast< std::uint16_t >(AF_PACKET);
    llAddr.sll_protocol = htons(static_cast< std::uint16_t >(ETH_P_1588));
    llAddr.sll_ifindex  = ifIndex_;
    ara::tsync::internal::char8_t const *const packetBuff{static_cast< ara::tsync::internal::char8_t const * >(mesg)};
    lastMsgSequenceId_ = ntohs(*(reinterpret_cast< uint16_t const * >(packetBuff + kEtherHeaderLen)));
    lastEventMsgType_  = *(packetBuff + kMSgTypeOffset) & kMsgTypeBits;
    std::array< struct iovec, 1 > msgIov{};
    msgIov[0U].iov_base = const_cast< void * >(mesg);
    msgIov[0U].iov_len  = mesgSize;

    struct ::msghdr msgHeader
    {
    };
    msgHeader.msg_name       = &llAddr;
    msgHeader.msg_namelen    = sizeof(llAddr);
    msgHeader.msg_iov        = msgIov.data();
    msgHeader.msg_iovlen     = 1U;
    msgHeader.msg_control    = nullptr;
    msgHeader.msg_controllen = 0U;

    ret = ::sendmsg(sockfd_, &msgHeader, 0);
    if (-1 == ret) {
        return ara::tsync::internal::kRET_E1;
    }
    std::chrono::nanoseconds const ts{ara::core::SteadyClock::now().time_since_epoch().count()};
    if (TSTimeStampTypeEnum::kSoftware == stampType_) {
        sendTimeStamp = ts;
    } else {
        if (_isEventMSg()) {
            bool const bSaveStamp{
                (static_cast< std::uint8_t >(ara::tsync::internal::GptpType::kSync) == lastEventMsgType_)};
            std::int32_t const err{_getIoTimeStamp(sendTimeStamp, bSaveStamp)};
            if (0 != err) {
                LOG().Error() << "Cant read Send timestamp from OS, using the "
                                 "application timestamp instead, this may "
                                 "not be expact.err="
                              << err;
                sendTimeStamp = ts;
                return ara::tsync::internal::kRET_E2;
            }
        } else {
            sendTimeStamp = ts;
        }
    }
    return 0;
}

/// @brief receive network data packet
/// @param mesg received message buffer.
/// @param mesgSize - message buffer length, will be modified to actual length after reception.
/// @param recvTimeStamp - system timestamp at reception, returned to user.
/// @return 0 - no valid message, >0 valid message
/// @return <0 - failure
std::int32_t SocketRaw::Recv(std::uint8_t *const mesg,
                             std::uint16_t &mesgSize,
                             std::chrono::nanoseconds &recvTimeStamp) noexcept
{
    struct msghdr mesgHeader
    {
    };
    std::array< struct iovec, 1 > msgIov{};
    std::array< ara::tsync::internal::char8_t, kControlBuffSize > contrlBuff{};
    ssize_t ret{0U};
    std::ignore         = recvTimeStamp;
    msgIov[0U].iov_base = mesg;
    msgIov[0U].iov_len  = mesgSize;

    static_cast< void >(::memset(&mesgHeader, 0, sizeof(mesgHeader)));
    mesgHeader.msg_iov        = msgIov.data();
    mesgHeader.msg_iovlen     = 1U;
    mesgHeader.msg_name       = nullptr;
    mesgHeader.msg_namelen    = 0U;
    mesgHeader.msg_control    = contrlBuff.data();
    mesgHeader.msg_controllen = kControlBuffSize;

    /// Because send timestamps may not be available immediately, need to poll.
    std::uint8_t i{0U};
    while (true) {
        ret = ::recvmsg(sockfd_, &mesgHeader, MSG_DONTWAIT);
        /// Data received
        if (ret > 0) {
            break;
        }

        /// No data received, poll to read
        if (i > kRecvTryCnt) {
            if (ara::tsync::internal::GetErrNo() == EAGAIN) {
                return 0;
            }
            return ara::tsync::internal::kRET_E1;
        }
        i++;
        /// Resource temporarily unavailable, read again
        if (ara::tsync::internal::GetErrNo() == EAGAIN) {
            // LOGDEBUG() << "recvmsg() again: " << i
            static_cast< void >(::usleep(kRecvSleep));  // sleep 1ms to avoid repeated reads
            continue;
            /// Other errors
        }
        /// :LOG().Error() << "recvmsg(): " << strerror(GetErrNo());
        return ara::tsync::internal::kRET_E2;
    }

    /// Write back the actual length of received data
    mesgSize = static_cast< std::uint16_t >(msgIov[0U].iov_len);

    // Parse time
    if (TSTimeStampTypeEnum::kSoftware == stampType_) {
        // PTP event messages need hardware timestamps; others still use software timestamps
        recvTimeStamp = std::chrono::nanoseconds(isoft::util::GetNSTimeofDay());
    } else {
        recvTimeStamp            = GetTimeStampFromMesg(&mesgHeader, stampType_);
        std::uint8_t messageType = *(mesg + kMSgTypeOffset) & kMsgTypeBits;
        if ((static_cast< std::uint8_t >(ara::tsync::internal::GptpType::kSync)) == messageType) {
            phcTimeStamp_ = recvTimeStamp.count();
        }

        recvTimeStamp = recvTimeStamp - std::chrono::nanoseconds{offset_ + sysToSteadyoffset2_ + kUTC_TAI_OFFSET_NS};
        std::ignore   = mesgHeader;
    }

    return static_cast< std::int32_t >(ret);
}
/// @brief determine whether the most recently sent message is an event type message
/// @return true for event type message
bool SocketRaw::_isEventMSg() const noexcept
{
    return (static_cast< std::uint8_t >(ara::tsync::internal::GptpType::kSync) == lastEventMsgType_)
           || (static_cast< std::uint8_t >(ara::tsync::internal::GptpType::kPdelayReq) == lastEventMsgType_)
           || (static_cast< std::uint8_t >(ara::tsync::internal::GptpType::kPdelayResp) == lastEventMsgType_);
}
/// @brief expected phc clock value when Sync is received, used to adjust phc clock
/// @param type - clock type
/// @param timeExpect - expected phc clock value
/// @return 0 - success
/// @return <0 - failure, and set errno
std::int32_t SocketRaw::AdjustClockTime(ClockAdjust type, std::int64_t const timeExpect) noexcept
{
    LOG().Verbose() << "AdjustClockTime,timeExpect = " << timeExpect << ", phcTimeStamp_=" << phcTimeStamp_
                    << ", offset=" << (phcTimeStamp_ - timeExpect) << ", type=" << static_cast< std::uint8_t >(type)
                    << ", sys_global_offset=" << timeExpect - (phcTimeStamp_ - kUTC_TAI_OFFSET_NS - offset_);
    if (labs(timeExpect + kUTC_TAI_OFFSET_NS - phcTimeStamp_) >= (kMaxOffsetSeconds * kTS_NSPERSEC)) {
        this->phcClock_ = nullptr;
        this->phcClock_ = std::make_shared< PhcClock >();
        if (nullptr == this->phcClock_) {
            LOG().Error() << "AdjustClockTime timer pocess this->phcClock_ nullptr";
        } else {
            std::int32_t const ret{this->phcClock_->Open(this->ifName_)};
            if (0 != ret) {
                LOG().Error() << "AdjustClockTime timer pocess  phcClock_ Open failed";
                this->phcClock_ = nullptr;
            }
            std::ignore = this->phcClock_->InitPhcClockValue((timeExpect));
        }
    } else {
        std::ignore
            = phcClock_->AdjustClock(phcTimeStamp_ - (timeExpect + kUTC_TAI_OFFSET_NS), lastSyncSystem_ + offset_);
    }
    if (ClockAdjust::kAdjustPhcSystem == type) {
        if (nullptr == sysClock_) {
            sysClock_ = std::make_shared< SysClock >();
            sysClock_->Open();
        }
        if (labs(timeExpect - (phcTimeStamp_ - kUTC_TAI_OFFSET_NS - offset_)) >= (kMaxOffsetSeconds * kTS_NSPERSEC)) {
            /// Reopen, need to reset sysToSteadyoffset; system time adjustment, previously calculated deviation not applicable
            sysClock_ = std::make_shared< SysClock >();
            sysClock_->Open();
            std::ignore         = sysClock_->InitSysClockValue(timeExpect);
            sysToSteadyoffset_  = 0;
            sysToSteadyoffset2_ = 0;
        } else {
            //AdjustTimeOffset -offset_
            // std::ignore
            //     = sysClock_->AdjustClock(-offset_ + phcTimeStamp_ - timeExpect - kUTC_TAI_OFFSET_NS, lastSyncSystem_);
            std::ignore
                = sysClock_->AdjustClock((phcTimeStamp_ - kUTC_TAI_OFFSET_NS - offset_) - timeExpect, lastSyncSystem_);
        }
    }
    _measureSysOffset();
    return 0;
}

/// @brief measure the deviation between phc clock and system clock
void SocketRaw::_measureSysOffset() noexcept
{
    /// First calculate the deviation; the more idle the system, the more accurate this deviation
    std::int64_t const offsetNs{std::chrono::system_clock::now().time_since_epoch().count()
                                - ara::core::SteadyClock::now().time_since_epoch().count()};
    if (0 == sysToSteadyoffset_) {
        sysToSteadyoffset_ = offsetNs;
    } else {
        if (std::abs(offsetNs) < std::abs(sysToSteadyoffset_)) {
            sysToSteadyoffset_ = offsetNs;
        }
    }
    timespec nowMonic{};
    timespec tsReal{};
    std::ignore = clock_gettime(CLOCK_MONOTONIC, &nowMonic);
    std::ignore = clock_gettime(CLOCK_REALTIME, &tsReal);
    std::int64_t const offsetNs2{tsReal.tv_sec * kTS_NSPERSEC + tsReal.tv_nsec - nowMonic.tv_sec * kTS_NSPERSEC
                                 - nowMonic.tv_nsec};
    if (0 == sysToSteadyoffset2_) {
        sysToSteadyoffset2_ = offsetNs2;
    } else {
        if (std::abs(offsetNs2) < std::abs(sysToSteadyoffset2_)) {
            sysToSteadyoffset2_ = offsetNs2;
        }
    };
    int64_t offset{0};
    int64_t delay{0};
    int64_t ts{0};
    if (nullptr != this->phcClock_) {
        std::int32_t const err{this->phcClock_->GetClockSysOffset(offset, delay, ts)};
        std::ignore = offset;
        std::ignore = delay;
        if (0 == err) {
            if (labs(offset) >= (kMaxOffsetSeconds * kTS_NSPERSEC)) {
                /// Deviation too large, reset,
                this->phcClock_ = nullptr;
                offsetFilter_   = AdaptiveClockKalman();
            } else {
                if (offsetFilter_.Update(static_cast< double >(labs(offset)))) {
                    offset_         = offset;
                    lastSyncSystem_ = ts - offset_;
                    if (false == syncGlobalToPhc_) {
                        this->phcClock_->AdjustClock(offset_, ts);
                    }
                }
            }
        }
    }
    if (nullptr == this->phcClock_) {
        this->phcClock_ = std::make_shared< PhcClock >();
        if (nullptr == this->phcClock_) {
            LOG().Error() << "_measureSysOffset timer pocess this->phcClock_ nullptr";
        } else {
            std::int32_t errCode{this->phcClock_->Open(this->ifName_)};
            if (0 != errCode) {
                LOG().Error() << "_measureSysOffset timer pocess  phcClock_ Open failed";
                this->phcClock_ = nullptr;
                offsetFilter_   = AdaptiveClockKalman();
            }
            std::ignore = clock_gettime(CLOCK_REALTIME, &tsReal);
            phcClock_->InitPhcClockValue(tsReal.tv_sec * kTS_NSPERSEC + tsReal.tv_nsec);
            std::ignore = this->phcClock_->GetClockSysOffset(offset, delay, ts);
            if (offsetFilter_.Update(static_cast< double >(labs(offset)))) {
                std::ignore     = offset;
                std::ignore     = delay;
                offset_         = offset;
                lastSyncSystem_ = ts - offset_;
            }
        }
    }
    if (nullptr != this->updatehwClockTimer_) {
        std::ignore = this->updatehwClockTimer_->UpdateTime(
            static_cast< std::int32_t >(isoft::util::TimeS2MS(kPHC_SYNC_INTERNVAL)));
    }
}

}  // namespace network
}  // namespace osi
}  // namespace isoft
