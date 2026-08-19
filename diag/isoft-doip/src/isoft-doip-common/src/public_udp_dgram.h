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
/// @file       public_udp_dgram.h
/// @brief      This file provides DOIP common NAI low-level encapsulation dgram definition related
/// @details
/// @date       2023-10-08
/// @author     zhanglipeng
/// @version    1.2.0
///
/// ================================================================

#ifndef ISOFT_DOIP_PUBLIC_UDP_DGRAM_H_
#define ISOFT_DOIP_PUBLIC_UDP_DGRAM_H_
#include <ara/core/string.h>
#include <isoft/naicpp/evloop.h>
#include <isoft/naicpp/global_evloop.h>
#include <nai/io/nai_io.h>
#include <nai/os/nai_socket.h>

#include <memory>

#include "isoft/doip/public_def.h"

namespace isoft {
namespace doip {

class PublicUdpDgram final
{
public:
    /// @brief Constructor
    PublicUdpDgram();

    /// @brief Destructor
    ~PublicUdpDgram() noexcept;

    /// @brief Copy constructor
    /// @param[in] other
    PublicUdpDgram(PublicUdpDgram const& other) = delete;

    /// @brief Assignment operator
    /// @param[in] other
    /// @return PublicUdpDgram reference
    PublicUdpDgram& operator=(PublicUdpDgram const& other) = delete;

    /// @brief Move constructor
    /// @param[in] other
    PublicUdpDgram(PublicUdpDgram&& other) = delete;

    /// @brief Move assignment operator
    /// @param[in] other
    /// @return PublicUdpDgram reference
    PublicUdpDgram& operator=(PublicUdpDgram&& other) = delete;

    /// @brief Notify new message arrival callback function
    using UdpNewMsgCallback = std::function< int32_t(PublicUdpDgram*, int32_t) >;

public:
    /// @brief UDP initialization
    /// @return Initialization result
    int32_t Initialize();

    /// @brief Set new message callback
    /// @param[in] callback Callback function
    /// @return Set result
    int32_t SetNewMsgCallback(UdpNewMsgCallback const& callback);

    /// @brief Bind address and port
    /// @param[in] evLoop Event loop
    /// @param[in] address Bind address
    /// @param[in] port Bind port
    /// @return Bind result
    int32_t Bind(std::shared_ptr< naicpp::EvLoop > const& evLoop,
                 ara::core::String const& address,
                 ara::core::String const& port);

    /// @brief Connect to peer
    /// @param[in] evLoop Event loop
    /// @param[in] address Peer address
    /// @return Connection result
    int32_t Connect(std::shared_ptr< naicpp::EvLoop > const& evLoop, ara::core::String const& address);

    /// @brief Close socket
    /// @return Close result
    int32_t Close();

    /// @brief Close socket
    /// @return Close result
    int32_t Shutdown()
    {
        std::ignore = nai_dgram_set_mode(&dgram_, 0);
        std::ignore = nai_dgram_shutdown(&dgram_, NAI_SOCK_RW);
        return Close();
    }

    /// @brief Receive UDP data
    /// @param[in] buf Receive buffer
    /// @param[in] len Receive buffer size
    /// @param[in] name Peer address
    /// @param[in] namelen Address size
    /// @return Receive result
    int64_t Recv(void* const buf, size_t const len, nai_sockaddr_t* const name, std::int32_t* const namelen);

    /// @brief Send UDP data
    /// @param[in] buf Send buffer
    /// @param[in] len Send buffer size
    /// @param[in] name Peer address
    /// @param[in] namelen Address size
    /// @return Send result
    int64_t Send(void const* const buf, size_t const len, nai_sockaddr_t const* const name, std::int32_t const namelen);

    /// @brief Send UDP data
    /// @param[in] buf Send buffer
    /// @param[in] len Send buffer size
    /// @param[in] address Peer address
    /// @return Send result
    int64_t Send(void const* const buf, size_t const len, ara::core::String const& address);

    /// @brief Set broadcast attribute
    /// @return Set result
    int32_t EnableBroadcast() const;

    /// @brief Get MAC
    /// @param[in] localAddress Local address
    /// @return mac
    MacAddress GetMacAddress(nai_sockaddr_t* const localAddress) const;

    /// @brief Start UDP listening
    /// @param[in] evloop Event loop
    /// @param[in] address IP address
    /// @param[in] port port
    /// @param[in] cb New message callback
    /// @return Start result
    int32_t Start(std::shared_ptr< naicpp::EvLoop > const& evloop,
                  ara::core::String const& address,
                  ara::core::String const& port,
                  UdpNewMsgCallback const& cb);

    /// @brief Release UDP resources
    /// @return Result
    int32_t Stop() { return Close(); }

private:
    /// @brief udp sock
    nai_dgram_t dgram_{};

    /// @brief New message callback
    UdpNewMsgCallback newMsgCallback_;
};

}  // namespace doip
}  // namespace isoft

#endif  // ISOFT_DOIP_PUBLIC_UDP_DGRAM_H_