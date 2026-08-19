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
/// @file       public_tcp_stream.h
/// @brief      This file provides DOIP common NAI low-level encapsulation stream definition related
/// @details
/// @date       2023-10-08
/// @author     zhanglipeng
/// @version    1.2.0
///
/// ================================================================

#ifndef ISOFT_DOIP_PUBLIC_TCP_STREAM_H_
#define ISOFT_DOIP_PUBLIC_TCP_STREAM_H_
#include <ara/core/string.h>
#include <isoft/naicpp/evloop.h>
#include <isoft/naicpp/global_evloop.h>
#include <nai/io/nai_io.h>

#ifdef NAI_ENABLE_SSL
    #include <nai/io/nai_ssl.h>
#endif

#include <nai/os/nai_socket.h>

#include <memory>

namespace isoft {
namespace doip {
class PublicTcpStream final
{
public:
    /// @brief Constructor
    PublicTcpStream();

    /// @brief Destructor
    ~PublicTcpStream() noexcept;

    /// @brief Copy constructor
    /// @param[in] other
    PublicTcpStream(PublicTcpStream const& other) = delete;

    /// @brief Assignment operator
    /// @param[in] other
    /// @return PublicTcpStream reference
    PublicTcpStream& operator=(PublicTcpStream const& other) = delete;

    /// @brief Move constructor
    /// @param[in] other
    PublicTcpStream(PublicTcpStream&& other) = delete;

    /// @brief Move assignment operator
    /// @param[in] other
    /// @return PublicTcpStream reference
    PublicTcpStream& operator=(PublicTcpStream&& other) = delete;

    /// @brief New message callback alias
    using TcpNewMsgCallback = std::function< int32_t(PublicTcpStream*, int32_t) >;

public:
    /// @brief Initialize
    /// @return Initialization result
    int32_t Initialize();

#ifdef NAI_ENABLE_SSL
    int32_t InitializeTLS(nai_ssl_t const& ssl) noexcept;
#endif

    /// @brief Connect to peer
    /// @param[in] evLoop Event loop
    /// @param[in] addressPort IP and port
    /// @return Result
    int32_t Connect(std::shared_ptr< naicpp::EvLoop > evLoop, ara::core::String addressPort);

    /// @brief Set FD
    /// @param fd fd
    /// @param type Descriptor type
    /// @return Set result
    int32_t SetFd(nai_fd_t const fd, std::int32_t const type) { return nai_stream_set_fd(&stream_, fd, type); }

    /// @brief Get FD
    /// @return fd
    int32_t GetFd() { return nai_stream_get_fd(&stream_); }

    /// @brief Inherit listening socket attributes
    /// @param[in] listen Listening socket
    /// @return Inherit result
    int32_t Inherit(nai_server_t* const listen) { return nai_stream_inherit(&stream_, listen); }

    /// @brief Read data from socket stream
    /// @param[in] buf Buffer address to read
    /// @param[in] len Buffer length to read
    /// @return Read result
    int64_t Read(void* const buf, size_t const len, bool isUseTls = false);

    /// @brief Write data to socket stream
    /// @param[in] buf Buffer address to write
    /// @param[in] len Buffer length to write
    /// @return Write result
    int64_t Write(const void* const buf, size_t const len, bool isUseTls = false);

    /// @brief Get peer IP and port
    /// @param[out] ip
    /// @param[out] port
    void GetPeerIpAndPort(ara::core::String& ip, std::uint16_t& port) const;

    /// @brief Get local IP and port
    /// @param[out] ip
    /// @param[out] port
    void GetLocalIpAndPort(ara::core::String& ip, std::uint16_t& port) const;

    /// @brief Open a socket stream
    /// @param[in] evLoop Event loop
    /// @return
    int32_t Open(std::shared_ptr< naicpp::EvLoop > const& evLoop)
    {
        return nai_stream_open(&stream_, evLoop->GetRawEvLoop());
    }

    /// @brief Close socket
    /// @return Close result
    int32_t Close() { return nai_stream_close(&stream_); }

    /// @brief Close socket
    /// @return Close result
    int32_t Shutdown()
    {
        std::ignore = nai_stream_set_mode(&stream_, 0);
        std::ignore = nai_stream_shutdown(&stream_, NAI_SOCK_RW);
        return Close();
    }

#ifdef NAI_ENABLE_SSL
    int32_t SSLHandshake() { return nai_ssl_handshake(&ssl_); }

    int32_t SSLVerify(int32_t const opt) { return nai_ssl_verify(&ssl_, opt); }

    /// @brief Close socket
    /// @return Close result
    int32_t ShutdownTLS() { return nai_ssl_shutdown(&ssl_, NAI_SOCK_RW); }
#endif

    /// @brief Set socket option
    /// @param[in] opt Option
    /// @param[in] value Option value
    /// @return Set result
    int32_t Setopt(std::int32_t const opt, intptr_t const value) { return nai_stream_set_opt(&stream_, opt, value); }

    /// @brief Set new message callback
    /// @param[in] cb Callback
    /// @return Set result
    int32_t SetNewMsgCallback(TcpNewMsgCallback cb);

private:
    /// @brief tcp socket
    nai_stream_t stream_{};

    /// @brief New message callback notification
    TcpNewMsgCallback newMsgCallback_{};

#ifdef NAI_ENABLE_SSL
    /// @brief TLS object
    nai_ssl_t ssl_{};
#endif
};

}  // namespace doip
}  // namespace isoft

#endif  // ISOFT_DOIP_PUBLIC_TCP_STREAM_H_