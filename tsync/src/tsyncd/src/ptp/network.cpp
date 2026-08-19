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
/// @file       network.cpp
/// @brief      PTP module network class
/// @details
/// @date       2023-01-10
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#include "ara/tsync/internal/ptp/network.h"

#include <isoft/naicpp/global_evloop.h>

#include <chrono>
#include <string>

#include "ara/tsync/internal/common.h"
#include "ara/tsync/internal/log/logger.h"
#include "isoft/osi/network/device.h"
#include "isoft/osi/network/ether.h"
#include "isoft/osi/network/socketraw.h"
#include "nai/io/nai_event.h"
#include "nai/io/nai_io.h"
#include "nai/os/nai_file.h"
#include "nai/runtime/nai_errno.h"

namespace ara {
namespace tsync {
namespace internal {
namespace ptp {

/// @brief log output
/// @return Logger object reference
static inline ara::tsync::internal::tslog::Logger &LOG() noexcept { return ara::tsync::internal::tslog::Log(); }

/// @brief NAI message callback function
/// @param stream - NAI stream handle
/// @param events - events
/// @return status
std::int32_t Network::NAIMessageEventHandler(nai_stream_t *const stream, std::int32_t const events) noexcept
{
    // get object from stream
    /// TODO(zhoubo): clang-tidy do not use C-style cast to convert between
    /// unrelated types
    Network *const net{nai_containof(stream, Network, sockStream_)};  // NOLINT
    std::uint32_t const newEvt{static_cast< std::uint32_t >(events)};
    std::uint32_t const evtRead{static_cast< std::uint32_t >(NAI_EV_READ)};
    std::uint32_t const evtWrite{static_cast< std::uint32_t >(NAI_EV_WRITE)};
    // Error event

    // Read event NAI_EV_IOE  NAI_EV_READ
    if (evtRead == (newEvt & evtRead)) {
        // Keep reading until empty, because one event may receive many packets
        // TODO(person in charge):
        // If there are multiple packets in the buffer, need special handling; mesgRecvCb_ handles one message at a time, message length varies per message
        while (true) {
            Address sourceAddr;
            Address destAddr;
            std::chrono::nanoseconds recvTimeStamp;
            std::array< std::uint8_t, kMAX_MESSAGE_BUFF_SIZE > mesgBuff{};
            std::uint16_t mesgSize{kMAX_MESSAGE_BUFF_SIZE};
            if (0 != net->_recv(mesgBuff.data(), mesgSize, sourceAddr, destAddr, recvTimeStamp)) {
                if (EAGAIN != GetErrNo()) {
                    return -GetErrNo();
                }
                return 0;
            }

            if (nullptr != net->mesgRecvCb_) {
                net->mesgRecvCb_(mesgBuff.data(), mesgSize, sourceAddr, destAddr, recvTimeStamp);
            }

        }  // while(true).
    }      // if (events & NAI_EV_READ).

    // Write event
    if (evtWrite == (newEvt & evtWrite)) {
        ;
    }

    return 0;
}

/// @brief open network
/// @param devName - network device name
/// @param stampType - timestamp type
/// @param multMacVec - set of multicast MAC addresses for all time domains on this network card
/// @param syncGlobalToPhc - whether to synchronize the global clock to the phc clock
/// @return 0 - success
/// @return <0 - failure
std::int32_t Network::Open(ara::core::String const &devName,
                           config::TSTimeStampTypeEnum const stampType,
                           ara::core::Vector< Network::Address > const &multMacVec,
                           bool syncGlobalToPhc) noexcept
{
    macVec_ = multMacVec;
    /// Get the MAC address of the current network card
    if (0 != isoft::osi::network::GetMacAddress(devName.c_str(), localMacAddr_)) {
        LOG().Error() << "GetMacAddress(" << devName << ")";
        return kRET_E2;
    }

    /// Open raw socket
    sockfd_ = sockRaw_.Open(devName.c_str(), stampType, multMacVec, syncGlobalToPhc);
    if (0 > sockfd_) {
        if (config::TSTimeStampTypeEnum::kSoftware != stampType) {
            LOG().Error() << "sockRaw_.Open(" << devName << ") failed, change to use system-clock stamp";
            sockfd_
                = sockRaw_.Open(devName.c_str(), config::TSTimeStampTypeEnum::kSoftware, multMacVec, syncGlobalToPhc);
        }

        if (0 > sockfd_) {
            LOG().Error() << "sockRaw_.Open(" << devName << ")";
            return kRET_E3;
        }
    }

    /// Initialize NAI receiving stream
    if (0 != nai_stream_init(&sockStream_)) {
        LOG().Error() << "nai_stream_init(&sockStream_)";
        return kRET_E4;
    }

    /// Set file descriptor for stream object
    if (0 > nai_stream_set_fd(&sockStream_, sockfd_, NAI_FD_TYPE_SOCK)) {
        LOG().Error() << "nai_stream_set_fd(&sockStream_)";
        return kRET_E5;
    }

    /// Set event callback function
    if (0 > nai_stream_set_cb(&sockStream_, NAIMessageEventHandler)) {
        LOG().Error() << "nai_stream_set_cb(&sockStream_)";
        return kRET_E6;
    }

    /// Open receive stream, start receive event loop
    if (0 > nai_stream_open(&sockStream_, isoft::naicpp::GlobalGeneralEvLoop::Get()->GetRawEvLoop())) {
        LOG().Error() << "nai_stream_open(stream_): " << GetErrString();
        return kRET_E7;
    }

    return 0;
}
/// @brief close network
/// @return 0 - success
/// @return <0 - failure
std::int32_t Network::Close() noexcept
{
    std::int32_t retCode{0};

    if (sockfd_ < 0) {
        return 0;
    }

    if (0 > nai_stream_close(&sockStream_)) {
        LOG().Error() << "nai_stream_close(): " << GetErrString();
        retCode = kRET_E1;
    }

    if (0 != sockRaw_.Close()) {
        LOG().Error() << "sockRaw_.Close(): " << GetErrString();
        retCode = kRET_E2;
    }

    return retCode;
}

/// @brief synchronously send network data packet.
/// @param mesg - message buffer to send.
/// @param mesgSize - length of message to send.
/// @param destAddr - destination address.
/// @param vlanPri - VLAN priority, if -1, no VLAN tag is set.
/// @param vlanId - VLAN id。
/// @param sendTimeStamp - system timestamp at sending completion, returned to user.
/// @return 0 - success
/// @return <0 - failure, and set errno
std::int32_t Network::Send(const void *const mesg,
                           std::uint16_t const mesgSize,
                           Address const &destAddr,
                           std::int8_t const vlanPri,
                           std::int16_t const vlanId,
                           std::chrono::nanoseconds &sendTimeStamp) noexcept
{
    std::array< std::uint8_t, kMAX_MESSAGE_BUFF_SIZE > packetBuff{};
    std::uint16_t packetSize{0U};
    if ((vlanPri >= 0) && (vlanId >= 0)) {
        isoft::osi::network::Ether *const ethTmp{reinterpret_cast< isoft::osi::network::Ether * >(packetBuff.data())};
        isoft::osi::network::Ether *const ethH{new (ethTmp) isoft::osi::network::Ether(true)};
        ethH->SetVlan(static_cast< std::uint8_t >(vlanPri), static_cast< std::uint16_t >(vlanId));
    } else {
        isoft::osi::network::Ether *const ethTmp{reinterpret_cast< isoft::osi::network::Ether * >(packetBuff.data())};
        static_cast< void >(new (ethTmp) isoft::osi::network::Ether(false));
    }

    isoft::osi::network::Ether *const etherHeader{reinterpret_cast< isoft::osi::network::Ether * >(packetBuff.data())};
    etherHeader->SetSourceAddress(localMacAddr_);
    etherHeader->SetDestAddress(destAddr);
    etherHeader->SetType(isoft::osi::network::Ether::Type::kPtp);
    std::uint32_t const headSize{etherHeader->GetHeadSize()};
    etherHeader->ToNetworkFormat();

    /// TODO(person in charge): Later optimize the interface to send multiple data blocks to avoid copying.
    packetSize = static_cast< std::uint16_t >(headSize + mesgSize);
    static_cast< void >(::memmove(packetBuff.data() + headSize, mesg, static_cast< std::size_t >(mesgSize)));

    if (0 != sockRaw_.Send(packetBuff.data(), packetSize, sendTimeStamp)) {
        LOG().Error() << "sockRaw_.Send()";
        return kRET_E1;
    }

    return 0;
}

/// @brief synchronous receive function.
/// @param mesg received message buffer.
/// @param mesgSize - message buffer length, will be modified to actual length after reception.
/// @param sourceAddr - message source address.
/// @param destAddr - message destination address.
/// @param recvTimeStamp - system timestamp at reception, returned to user.
/// @return 0 - success
/// @return <0 - failure
std::int32_t Network::_recv(std::uint8_t *const mesg,
                            std::uint16_t &mesgSize,
                            Address &sourceAddr,
                            Address &destAddr,
                            std::chrono::nanoseconds &recvTimeStamp) noexcept
{
    std::ignore = mesgSize;
    std::array< std::uint8_t, kMAX_MESSAGE_BUFF_SIZE > packetBuff{};
    std::uint16_t packetSize{kMAX_MESSAGE_BUFF_SIZE};
    std::int32_t const ret{sockRaw_.Recv(packetBuff.data(), packetSize, recvTimeStamp)};
    if (0 > ret) {
        LOG().Error() << "sockRaw_.Recv()"
                      << ",errno=" << GetErrNo() << "," << GetErrString();
        return kRET_E1;
    }
    if (0 == ret) {
        return kRET_E1;
    }

    isoft::osi::network::Ether *const etherHeader{reinterpret_cast< isoft::osi::network::Ether * >(packetBuff.data())};
    mesgSize = static_cast< std::uint16_t >(packetSize - etherHeader->GetHeadSize());
    etherHeader->GetSourceAddress(sourceAddr);
    etherHeader->GetDestAddress(destAddr);

    /// TODO(person in charge): Later optimize the interface to send multiple data blocks.
    static_cast< void >(
        ::memmove(mesg, packetBuff.data() + etherHeader->GetHeadSize(), static_cast< std::size_t >(mesgSize)));

    return 0;
}

/// @brief expected phc clock value when Sync is received, used to adjust phc clock
/// @param type - clock type
/// @param timeExpect - expected phc clock value
/// @return 0 - success
/// @return <0 - failure
std::int32_t Network::AdjustClockTime(ClockAdjust type, std::int64_t const timeExpect) noexcept
{
    std::int32_t const ret{sockRaw_.AdjustClockTime(type, timeExpect)};
    if (0 > ret) {
        LOG().Error() << "sockRaw_.AdjustClockTime()"
                      << ",errno=" << GetErrNo() << "," << GetErrString();
        return kRET_E1;
    }
    if (0 == ret) {
        return kRET_E1;
    }
    return 0;
}
}  // namespace ptp
}  // namespace internal
}  // namespace tsync
}  // namespace ara
