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
/// @file       network.h
/// @brief      PTP module network class
/// @details
/// @date       2023-01-10
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_TSYNC_INTERNAL_PTP_NETWORK_H_
#define ARA_TSYNC_INTERNAL_PTP_NETWORK_H_

#include <chrono>
#include <cstdint>
#include <functional>
#include <memory>

#include "ara/core/string.h"
#include "ara/core/vector.h"
#include "ara/tsync/internal/log/logger.h"
#include "isoft/naicpp/evloop.h"
#include "isoft/osi/network/socketraw.h"
#include "nai/io/nai_io.h"

namespace ara {
namespace tsync {
namespace internal {
namespace ptp {
static constexpr std::size_t kMAX_MESSAGE_BUFF_SIZE{2048U};

/// @brief Network
class Network final
{
public:
    /// @brief Address
    using Address = isoft::osi::network::Ether::Address;

    /// @brief default constructor
    Network() = default;

    /// @brief default destructor
    ~Network() noexcept { static_cast< void >(Close()); };

    /// @brief copy constructor is prohibited
    /// @param other - other object
    Network(Network const &other) = delete;

    /// @brief copy is prohibited
    /// @param other - other object
    /// @return reference to class object
    Network &operator=(Network const &other) = delete;

    /// @brief move constructor
    /// @param other an existing rvalue reference.
    Network(Network &&) noexcept = default;

    /// @brief '=' operator overload
    /// @param other an existing rvalue reference.
    /// @return reference to class object
    Network &operator=(Network &&) &noexcept = default;

    /// @brief open network
    /// @param devName - network device name
    /// @param stampType - timestamp type
    /// @param multMacVec - set of multicast MAC addresses for all time domains on this network card
    /// @param syncGlobalToPhc - whether to synchronize the global clock to the phc clock
    /// @return 0 - success
    /// @return <0 - failure
    std::int32_t Open(ara::core::String const &devName,
                      config::TSTimeStampTypeEnum const stampType,
                      ara::core::Vector< Network::Address > const &multMacVec,
                      bool syncGlobalToPhc) noexcept;

    /// @brief close network
    /// @return 0 - success
    /// @return <0 - failure
    std::int32_t Close() noexcept;

    /// @brief synchronously send network data packet.
    /// @param mesg - message buffer to send.
    /// @param mesgSize - length of message to send.
    /// @param destAddr - destination address.
    /// @param vlanPri - VLAN priority, if -1, no VLAN tag is set.
    /// @param vlanId - VLAN id。
    /// @param sendTimeStamp - system timestamp at sending completion, returned to user.
    /// @return 0 - success
    /// @return <0 - failure, and set errno
    std::int32_t Send(void const *const mesg,
                      std::uint16_t const mesgSize,
                      Address const &destAddr,
                      std::int8_t const vlanPri,
                      std::int16_t const vlanId,
                      std::chrono::nanoseconds &sendTimeStamp) noexcept;

    /// @brief expected phc clock value when Sync is received, used to adjust phc clock
    /// @param type - clock type
    /// @param timeExpect - expected phc clock value
    /// @return 0 - success
    /// @return <0 - failure, and set errno
    std::int32_t AdjustClockTime(ClockAdjust type, std::int64_t const timeExpect) noexcept;

    /// @brief asynchronous receive callback function.
    /// @param mesg - received message buffer.
    /// @param mesgSize - length of received message.
    /// @param sourceAddr - message source address.
    /// @param destAddr - message destination address.
    /// @param recvTimeStamp - system timestamp at reception, returned to user.
    using MessageRecvedHandler = std::function< void(const void *mesg,
                                                     std::uint16_t mesgSize,
                                                     Address const &sourceAddr,
                                                     Address const &destAddr,
                                                     std::chrono::nanoseconds const &recvTimeStamp) >;

    /// @brief asynchronously receive network data packet.
    /// @param cb receive callback function.
    /// @return 0 - success
    /// @return <0 - failure, and set errno
    void OnRecv(MessageRecvedHandler const &cb) noexcept { mesgRecvCb_ = cb; }

private:
    /// @brief synchronous receive function.
    /// @param mesg received message buffer.
    /// @param mesgSize - message buffer length, will be modified to actual length after reception.
    /// @param sourceAddr - message source address.
    /// @param destAddr - message destination address.
    /// @param recvTimeStamp - system timestamp at reception, returned to user.
    /// @return 0 - success
    /// @return <0 - failure
    std::int32_t _recv(std::uint8_t *const mesg,
                       std::uint16_t &mesgSize,
                       Address &sourceAddr,
                       Address &destAddr,
                       std::chrono::nanoseconds &recvTimeStamp) noexcept;

    /// @brief NAI message callback function
    /// @param stream - NAI stream handle
    /// @param events - events
    /// @return status
    static std::int32_t NAIMessageEventHandler(nai_stream_t *const stream, std::int32_t const events) noexcept;

private:
    /// @name sockStream_ - NAI socket handle
    nai_stream_t sockStream_{};

    /// @name sockRaw_ - raw socket handle
    isoft::osi::network::SocketRaw sockRaw_{};

    /// @name sockfd_ - socket file descriptor
    std::int32_t sockfd_{-1};

    /// @name multiCastMacAddr_ - multicast MAC address
    Address multiCastMacAddr_{};

    /// @name localMacAddr_ - local MAC address
    Address localMacAddr_{};

    /// @name mesgRecvCb_ - message user callback function
    MessageRecvedHandler mesgRecvCb_{};

    /// @name macVec_ - set of ptp message MAC addresses
    ara::core::Vector< Address > macVec_{};
};  /// class Network

}  // namespace ptp
}  // namespace internal
}  // namespace tsync
}  // namespace ara

#endif  /// ARA_TSYNC_INTERNAL_PTP_NETWORK_H_
