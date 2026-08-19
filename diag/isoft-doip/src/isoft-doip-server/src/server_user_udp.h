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
/// @file       server_user_udp.h
/// @brief      This file provides DOIP server UDP related definitions
/// @details
/// @date       2023-10-08
/// @author     zhanglipeng
/// @version    1.2.0
///
/// ================================================================

#ifndef ISOFT_DOIP_SERVER_USER_UDP_H_
#define ISOFT_DOIP_SERVER_USER_UDP_H_
#include <nai/os/nai_socket.h>

#include <mutex>

#include "server_user_base.h"

namespace isoft {
namespace doip {

/// @brief Forward declaration of ServerImpl
class ServerImpl;
/// @brief Forward declaration of PublicUdpDgram
class PublicUdpDgram;

/// @brief DOIP UDP user implementation
class ServerUserUdp : public ServerUserBase
{
public:
    /// @brief Constructor
    /// @param[in] fd sock fd
    /// @param[in] udp udp dgram
    /// @param[in] server DOIP service implementation
    /// @param[in] bufferSize Read buffer size
    ServerUserUdp(int32_t fd, std::shared_ptr< PublicUdpDgram > udp, ServerImpl* server, size_t bufferSize);

    /// @brief Destructor
    ~ServerUserUdp() noexcept override;

    /// @brief Copy constructor
    /// @param[in] other
    ServerUserUdp(ServerUserUdp const& other) = delete;

    /// @brief Assignment operator
    /// @param[in] other
    /// @return
    ServerUserUdp& operator=(ServerUserUdp const& other) = delete;

    /// @brief Move constructor
    /// @param[in] other
    ServerUserUdp(ServerUserUdp&& other) = delete;

    /// @brief Move assignment operator
    /// @param[in] other
    /// @return
    ServerUserUdp& operator=(ServerUserUdp&& other) = delete;

public:
    /// @brief Get descriptor
    /// @return Descriptor
    int32_t GetFd() override { return fd_; }

    /// @brief Process received UDP packet
    void Process(uint8_t* data, size_t const size);

    /// @brief Send data to peer
    /// @param[in] type Protocol number
    /// @param[in] body Message body
    /// @return Send result
    int32_t SendToClient(PayloadType const type, ara::core::Vector< uint8_t > const& body) override;

private:
    void _releaseSelf();

private:
    /// @brief
    /// Descriptor assigned to UDP
    int32_t fd_;

    /// @brief NAI datagram
    std::shared_ptr< PublicUdpDgram > udp_;

    /// @brief Thread safety issue
    std::recursive_mutex udpLock_{};
};

}  // namespace doip
}  // namespace isoft

#endif  // ISOFT_DOIP_SERVER_USER_UDP_H_