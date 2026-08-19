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
/// @file       socket_stream.cpp
/// @brief      OSI network module stream socket class
/// @details
/// @date       2023-11-19
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#include "isoft/osi/network/socket_stream.h"

#include <cstring>
#include <iostream>

/// @brief Define operating system interface as Linux
#define __OSI_LINUX__ 1

#ifdef __OSI_LINUX__

// clang-format off
    #include <arpa/inet.h>
    #include <sys/socket.h>
    #include <sys/types.h>
    #include <unistd.h>
// clang-format on

namespace isoft {
namespace osi {
namespace network {
namespace socket {

/// @brief Open socket and listen for service
/// @param serverAddr Server address
/// @param serverPort Server port
/// @return 0 success; <0 failure
int32_t StreamServer::Open(std::string const &serverAddr, uint16_t const serverPort) noexcept
{
    struct sockaddr_in addr
    {
    };

    if (0U == serverPort) {
        return -1;
    }

    serverPort_ = serverPort;
    serverAddr_ = serverAddr;

    sockFd_ = ::socket(AF_INET, SOCK_STREAM | SOCK_CLOEXEC, 0);
    if (-1 == sockFd_) {
        return -1;
    }

    static_cast< void >(::memset(&addr, 0, sizeof(addr)));
    addr.sin_family = static_cast< sa_family_t >(AF_INET);
    addr.sin_port   = htons(serverPort_);
    if (serverAddr.empty()) {
        addr.sin_addr.s_addr = INADDR_ANY;  // PRQA MS "INADDR_ANY" 3080
    } else {
        addr.sin_addr.s_addr = inet_addr(serverAddr.c_str());
    }

    int32_t const optval{1};
    std::ignore = ::setsockopt(sockFd_, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    if (0 != ::bind(sockFd_, reinterpret_cast< struct sockaddr * >(&addr), sizeof(addr))) {  // PRQA S 3049
        return -1;
    }

    if (0 != ::listen(sockFd_, 1)) {
        return -1;
    }

    return 0;
}

/// @brief Close socket
/// @return 0 success; <0 failure
int32_t StreamServer::Close() const noexcept { return ::close(sockFd_); }

/// @brief Wait for client connection
/// @return >=0 newly connected client; <0 failure
int32_t StreamServer::WaitConnect() const noexcept { return accept(sockFd_, nullptr, nullptr); }

/// @brief Send network data packet
/// @param fd Network socket descriptor
/// @param mesg Message buffer to send
/// @param mesgSize Length of message to send
/// @return 0 success; <0 failure
int32_t StreamServer::Send(int32_t const fd, const void *const mesg, uint32_t const mesgSize) noexcept
{
    return static_cast< int32_t >(::send(fd, mesg, static_cast< size_t >(mesgSize), 0));
}

/// @brief Receive network data packet
/// @param fd Socket file descriptor
/// @param mesg Received message buffer
/// @param mesgSize Message buffer length
/// @return <0 no valid packet; >0 valid packet
int32_t StreamServer::Recv(int32_t const fd, void *const mesg, uint32_t const mesgSize) noexcept
{
    return static_cast< int32_t >(::recv(fd, mesg, static_cast< size_t >(mesgSize), 0));
}

/// @brief Open socket and connect to service
/// @param serverAddr Server address
/// @param serverPort Server port
/// @return 0 success; <0 failure
int32_t StreamClient::Open(std::string const &serverAddr, uint16_t const serverPort) noexcept
{
    struct sockaddr_in addr
    {
    };

    if (0U == serverPort) {
        return -1;
    }
    if (serverAddr.empty()) {
        return -1;
    }

    serverPort_ = serverPort;
    serverAddr_ = serverAddr;

    sockFd_ = ::socket(AF_INET, SOCK_STREAM | SOCK_CLOEXEC, 0);
    if (-1 == sockFd_) {
        return -1;
    }

    static_cast< void >(::memset(&addr, 0, sizeof(addr)));
    addr.sin_family      = static_cast< sa_family_t >(AF_INET);
    addr.sin_port        = htons(serverPort_);
    addr.sin_addr.s_addr = inet_addr(serverAddr.c_str());

    if (0 != ::connect(sockFd_, reinterpret_cast< struct sockaddr * >(&addr), sizeof(addr))) {  // PRQA S 3049
        return -1;
    }

    return 0;
}

/// @brief Close original socket
/// @return 0 success; <0 failure
int32_t StreamClient::Close() const noexcept { return ::close(sockFd_); }

/// @brief Send network data packet
/// @param mesg Message buffer to send
/// @param mesgSize Length of message to send
/// @return 0 success; <0 failure
int32_t StreamClient::Send(void const *const mesg, uint32_t const mesgSize) const noexcept
{
    return static_cast< int32_t >(::send(sockFd_, mesg, static_cast< size_t >(mesgSize), 0));
}

/// @brief Receive network data packet
/// @param mesg, Received message buffer
/// @param mesgSize Message buffer length
/// @return >0 valid packet; <0 invalid packet
int32_t StreamClient::Recv(void *const mesg, uint32_t const mesgSize) const noexcept
{
    return static_cast< int32_t >(::recv(sockFd_, mesg, static_cast< size_t >(mesgSize), 0));
}

}  // namespace socket
}  // namespace network
}  // namespace osi
}  // namespace isoft

#else  /// __OSI_LINUX__
static_assert(false, "Only port for LINUX");
#endif