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
/// @file       server_listen.h
/// @brief      This file provides DOIP server listening implementation related
/// @details
/// @date       2023-10-08
/// @author     zhanglipeng
/// @version    1.2.0
///
/// ================================================================

#ifndef ISOFT_DOIP_SERVER_LISTEN_H_
#define ISOFT_DOIP_SERVER_LISTEN_H_
#include <ara/core/string.h>
#include <isoft/naicpp/evloop.h>
#include <isoft/naicpp/global_evloop.h>
#include <nai/io/nai_io.h>

#ifdef NAI_ENABLE_SSL
    #include <nai/io/nai_ssl.h>
    #include <openssl/err.h>
    #include <openssl/ssl.h>
#endif

#include <nai/os/nai_socket.h>

#include <functional>
#include <memory>

namespace isoft {
namespace doip {

/// @brief Forward declaration of PublicTcpStream
class PublicTcpStream;

class ServerListen;

/// @brief Constructor
/// @param[in] evloop Event loop
using NewConnectionCallback = std::function< void(
    std::unique_ptr< PublicTcpStream > stream, nai_socknbuf_t const& naiSocknbu, ServerListen* listen) >;

class ServerListen
{
public:
    /// @brief Constructor
    explicit ServerListen(std::shared_ptr< naicpp::EvLoop > evloop,
                          bool isUseTls               = false,
                          ara::core::String serverCrt = "",
                          ara::core::String serverKey = "",
                          ara::core::String ca        = "");

    /// @brief Destructor
    ~ServerListen() noexcept;

    /// @brief Copy constructor
    /// @param[in] other
    ServerListen(ServerListen const& other) = delete;

    /// @brief Assignment operator
    /// @param{in]} other
    /// @return ServerListen reference
    ServerListen& operator=(ServerListen const& other) = delete;

    /// @brief Move constructor
    /// @param[in] other
    ServerListen(ServerListen&& other) = delete;

    /// @brief Move assignment operator
    /// @param[in] other
    /// @return ServerListen reference
    ServerListen& operator=(ServerListen&& other) = delete;

public:
    /// @brief Start server listening
    /// @param[in] address ip
    /// @param[in] port port
    /// @param[in] cb New connection callback
    /// @return Listening result
    int32_t Start(ara::core::String const& address, ara::core::String const& port, NewConnectionCallback const& cb);

    /// @brief Stop listening service
    /// @return Operation result
    int32_t Stop();

#ifdef NAI_ENABLE_SSL
    SSL_CTX* GetCtx() noexcept { return openSSLCtx_; }
#endif

private:
    /// @brief Accept a TCP connection
    /// @return Accept result
    int32_t _accept();

#ifdef NAI_ENABLE_SSL
    bool _initTLS() noexcept;
#endif

private:
    /// @brief listen server
    nai_server_t listen_{};

    /// @brief Event loop
    std::shared_ptr< naicpp::EvLoop > evloop_{};

    bool isUseTls_{false};

    ara::core::String serverCrt_{""};

    ara::core::String serverKey_{""};

    ara::core::String ca_{""};

    /// @brief New connection arrival callback
    NewConnectionCallback newConnectionCallback_{};

#ifdef NAI_ENABLE_SSL
    /// @brief ctx
    nai_ssl_ctx_t ctx_;

    /// @brief TLS object
    nai_ssl_t ssl_{};

    SSL_CTX* openSSLCtx_{nullptr};
#endif
};

}  // namespace doip
}  // namespace isoft

#endif  // ISOFT_DOIP_SERVER_LISTEN_H_