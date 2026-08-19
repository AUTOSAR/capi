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
/// @file       socket_stream.h
/// @brief      OSI network module stream socket class
/// @details
/// @date       2023-01-09
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#ifndef ISOFT_OSI_NETWORK_SOCKET_STREAM_H_
#define ISOFT_OSI_NETWORK_SOCKET_STREAM_H_

#include <chrono>
#include <cstdint>
#include <functional>
#include <memory>
#include <string>

namespace isoft {
namespace osi {
namespace network {
namespace socket {

/// @brief Stream socket service class
class StreamServer final  // PRQA S 5215
{
public:
 /// @brief Constructor
    StreamServer() = default;

 /// @brief Destructor
    ~StreamServer() noexcept { std::ignore = Close(); }
 /// @brief Disable move construction
    /// @param other the other StreamServer
    StreamServer(StreamServer &&other) noexcept = delete;

 /// @brief Disable copy construction
    /// @param other the other StreamServer
    StreamServer(StreamServer const &other) noexcept = delete;

 /// @brief Disable move assignment
    /// @param other the other StreamServer
 /// @return New StreamServer
    StreamServer &operator=(StreamServer &&other) noexcept = delete;

 /// @brief Disable copy assignment
    /// @param other the other StreamServer
 /// @return New StreamServer
    StreamServer &operator=(StreamServer const &other) noexcept = delete;

 /// @brief Open socket and listen for service
 /// @param serverAddr Server address
 /// @param serverPort Server port
 /// @return 0 success; <0 failure
    int32_t Open(std::string const &serverAddr, uint16_t const serverPort) noexcept;

 /// @brief Open socket and listen for service
 /// @param serverPort Server port
 /// @return 0 success; <0 failure
    int32_t Open(uint16_t const serverPort) noexcept { return Open(std::string{}, serverPort); }

 /// @brief Close socket
 /// @return 0 success; <0 failure
    int32_t Close() const noexcept;

 /// @brief Wait for client connection
 /// @return >=0 newly connected client; <0 failure
    int32_t WaitConnect() const noexcept;

 /// @brief Send network data packet
 /// @param fd Network socket descriptor
 /// @param mesg Message buffer to send
 /// @param mesgSize Length of message to send
 /// @return 0 success; <0 failure
    static int32_t Send(int32_t const fd, const void *const mesg, uint32_t const mesgSize) noexcept;

 /// @brief Receive network data packet
 /// @param fd Socket file descriptor
 /// @param mesg Received message buffer
 /// @param mesgSize Message buffer length
 /// @return <0 no valid packet; >0 valid packet
    static int32_t Recv(int32_t const fd, void *const mesg, uint32_t const mesgSize) noexcept;

 /// @brief Get file descriptor
 /// @return File descriptor
    int32_t GetFd() const noexcept { return sockFd_; }

private:
 /// @brief sockfd_ socket file descriptor
    int32_t sockFd_{-1};

 /// @brief Server address
    std::string serverAddr_;

 /// @brief Service port number
    uint16_t serverPort_{0U};

};  ///< class StreamServer

/// @brief Stream socket client class
class StreamClient final
{
public:
 /// @brief Constructor
    StreamClient() = default;

 /// @brief Destructor
    ~StreamClient() noexcept { std::ignore = Close(); }

 /// @brief Disable move construction
    /// @param other the other StreamClient
    StreamClient(StreamClient &&other) noexcept = delete;

 /// @brief Disable copy construction
    /// @param other the other StreamClient
    StreamClient(StreamClient const &other) noexcept = delete;

 /// @brief Disable move assignment
    /// @param other the other StreamClient
 /// @return New StreamClient
    StreamClient &operator=(StreamClient &&other) noexcept = delete;

 /// @brief Disable copy assignment
    /// @param other the other StreamClient
 /// @return New StreamClient
    StreamClient &operator=(StreamClient const &other) noexcept = delete;

 /// @brief Open socket and connect to service
 /// @param serverAddr Server address
 /// @param serverPort Server port
 /// @return 0 success; <0 failure
    int32_t Open(std::string const &serverAddr, uint16_t const serverPort) noexcept;

 /// @brief Close original socket
 /// @return 0 success; <0 failure
    int32_t Close() const noexcept;

 /// @brief Send network data packet
 /// @param mesg Message buffer to send
 /// @param mesgSize Length of message to send
 /// @return 0 success; <0 failure
    int32_t Send(void const *const mesg, uint32_t const mesgSize) const noexcept;

 /// @brief Receive network data packet
 /// @param mesg, Received message buffer
 /// @param mesgSize Message buffer length
 /// @return >0 valid packet; <0 invalid packet
    int32_t Recv(void *const mesg, uint32_t const mesgSize) const noexcept;

private:
 /// @brief sockfd_ socket file descriptor
    int32_t sockFd_{-1};

 /// @brief Server address
    std::string serverAddr_;

 /// @brief Service port number
    uint16_t serverPort_{0U};

};  ///< class StreamServer

}  // namespace socket
}  // namespace network
}  // namespace osi
}  // namespace isoft

#endif  ///< ISOFT_OSI_NETWORK_SOCKET_STREAM_H_
