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
/// @file       server_listen.cpp
/// @brief      This file provides DOIP server listening implementation related
/// @details
/// @date       2023-10-08
/// @author     zhanglipeng
/// @version    1.2.0
///
/// ================================================================

#include "server_listen.h"

#include <nai/runtime/nai_string.h>

#include "public.h"
#include "public_log.h"
#include "public_tcp_stream.h"
#include "public_tls_opt.h"
#include "thread/therad_safe.h"

namespace isoft {
namespace doip {

/// @brief Constructor
/// @param[in] evloop Event loop
ServerListen::ServerListen(std::shared_ptr< naicpp::EvLoop > evloop,
                           bool isUseTls,
                           ara::core::String serverCrt,
                           ara::core::String serverKey,
                           ara::core::String ca)
    : evloop_{std::move(evloop)}
    , isUseTls_{isUseTls}
    , serverCrt_{std::move(serverCrt)}
    , serverKey_{std::move(serverKey)}
    , ca_{std::move(ca)}
{
}

/// @brief Destructor
ServerListen::~ServerListen() noexcept { evloop_.reset(); }

/// @brief Start server listening
/// @param[in] address ip
/// @param[in] port port
/// @param[in] cb New connection callback
/// @return Listening result
int32_t ServerListen::Start(ara::core::String const& address,
                            ara::core::String const& port,
                            NewConnectionCallback const& cb)
{
    // #ifdef NAI_ENABLE_SSL
    //     if (isUseTls_) {
    //         if (serverCrt_.empty()) {
    //             LogError() << "ServerListen::Start|server_crt is empty";
    //             return -1;
    //         }

    //         if (serverKey_.empty()) {
    //             LogError() << "ServerListen::Start|server_key is empty";
    //             return -1;
    //         }

    //         nai_ssl_ctx_init(&ctx_);

    //         int32_t r = nai_ssl_ctx_open(&ctx_, NAI_SSL_TLS);
    //         if (r == -1) {
    //             LogError() << "ServerListen::Start|nai_ssl_ctx_open fails!"
    //                        << " error:" << isoft::threadsafe::Safe_Strerror(errno);
    //             return -1;
    //         }

    //         nai_str_t serverCrt = nai_strconst(const_cast< char* >(serverCrt_.c_str()));
    //         nai_str_t serverKey = nai_strconst(const_cast< char* >(serverKey_.c_str()));
    //         r                   = nai_ssl_ctx_certificate(&ctx_, &serverCrt, &serverKey, 0, 0);
    //         if (r == -1) {
    //             LogError() << "ServerListen::Start|nai_ssl_ctx_certificate fails!"
    //                        << " error:" << isoft::threadsafe::Safe_Strerror(errno);
    //             return -1;
    //         }

    //         r = nai_ssl_ctx_verify_depth(&ctx_, 1);
    //         if (r == -1) {
    //             LogError() << "ServerListen::Start|nai_ssl_ctx_verify_depth fails!"
    //                        << " error:" << isoft::threadsafe::Safe_Strerror(errno);
    //             return -1;
    //         }
    //     }
    // #endif

    nai_socknbuf_t nbuf{};
    int32_t res{static_cast< int32_t >(nai_sockaddr_list(address.c_str(), port.c_str(), &nbuf, sizeof(nbuf)))};
    if (res == -1) {
        LogError() << "ServerListen::Start|sockaddr fails!" << isoft::threadsafe::Safe_Strerror(errno);
        return res;
    }

    res = nai_server_init(&listen_);
    if (res == -1) {
        LogError() << "ServerListen::Start|Initialize fails!" << isoft::threadsafe::Safe_Strerror(errno);
        return res;
    }

    res = nai_server_set_opt(&listen_, NAI_IO_REUSEADDR, 1);
    if (res == -1) {
        LogError() << "ServerListen::Start|Setopt NAI_IO_REUSEADDR error:" << isoft::threadsafe::Safe_Strerror(errno);
        return res;
    }

    res = nai_server_set_opt(&listen_, NAI_IO_REUSEPORT, 1);
    if (res == -1) {
        LogError() << "ServerListen::Start|Setopt NAI_IO_REUSEPORT error:" << isoft::threadsafe::Safe_Strerror(errno);
        return res;
    }
    int const timout{30000};
    res = nai_server_set_opt(&listen_, NAI_IO_RECVTIMEO, timout);
    if (res == -1) {
        LogError() << "ServerListen::Start|Setopt NAI_IO_RECVTIMEO error:" << isoft::threadsafe::Safe_Strerror(errno);
        return res;
    }

    res = nai_server_set_opt(&listen_, NAI_IO_SENDTIMEO, timout);
    if (res == -1) {
        LogError() << "ServerListen::Start|Setopt NAI_IO_SENDTIMEO error:" << isoft::threadsafe::Safe_Strerror(errno);
        return res;
    }

    res = nai_server_set_cb(&listen_, [](nai_stream_t* const p, std::int32_t const events) -> int32_t {
        LogDebug() << "ServerListen::Callback|new events:" << events;
        ServerListen* const pListen{reinterpret_cast< ServerListen* >(
            reinterpret_cast< char* >(p)
            - reinterpret_cast< uint64_t >(&reinterpret_cast< ServerListen* >(0)->listen_))};
        return pListen->_accept();
    });

    if (res == -1) {
        LogError() << "ServerListen::Start|nai_listen_set_cb error:" << isoft::threadsafe::Safe_Strerror(errno);
        return res;
    }

    res = nai_server_bind(&listen_, evloop_->GetRawEvLoop(), &nbuf.addr, nbuf.len);
    if (res == -1) {
        LogError() << "ServerListen::Start|bind fails!" << isoft::threadsafe::Safe_Strerror(errno);
        return res;
    }

    // #ifdef NAI_ENABLE_SSL
    //     if (isUseTls_) {
    //         nai_ssl_init(&ssl_);
    //         nai_ssl_set_ops(&ssl_, &tls_ops, &listen_);

    //         res = nai_ssl_open(&ssl_, &ctx_, 0);
    //         if (res == -1) {
    //             LogError() << "ServerListen::_initTLS|nai_ssl_open fails!"
    //                        << " error:" << isoft::threadsafe::Safe_Strerror(errno);
    //             return false;
    //         }
    //     }
    // #endif
    bool isEnableTls = false;
#ifdef NAI_ENABLE_SSL
    if (isUseTls_) {
        if (serverCrt_.empty()) {
            LogWarn() << "ServerListen::Start|server_crt is empty";
        }

        if (serverKey_.empty()) {
            LogWarn() << "ServerListen::Start|server_key is empty";
        }

        if (ca_.empty()) {
            LogWarn() << "ServerListen::Start|ca is empty";
        }

        openSSLCtx_ = SSL_CTX_new(TLS_server_method());
        if (!openSSLCtx_) {
            LogError() << "ServerListen::Start|create ctx fail";
            ERR_print_errors_fp(stderr);
            return -1;
        }

        // Set minimum and maximum protocol versions to TLS 1.2
        res = SSL_CTX_set_max_proto_version(openSSLCtx_, TLS1_3_VERSION);
        if (res == 0) {
            LogError() << "ServerListen::Start|SSL_CTX_set_max_proto_version fail, res = " << res;
            ERR_print_errors_fp(stderr);
            return -1;
        }

        res = SSL_CTX_set_min_proto_version(openSSLCtx_, TLS1_2_VERSION);
        if (res == 0) {
            LogError() << "ServerListen::Start|SSL_CTX_set_min_proto_version faile, res = " << res;
            ERR_print_errors_fp(stderr);
            return -1;
        }

        if (!serverCrt_.empty() && !serverKey_.empty()) {
            // Load server certificate
            if (SSL_CTX_use_certificate_chain_file(openSSLCtx_, serverCrt_.c_str()) <= 0) {
                LogError() << "ServerListen::Start|load certificate fail, certificate = " << serverCrt_;
                ERR_print_errors_fp(stderr);
                return -1;
            }

            // Load server private key
            if (SSL_CTX_use_PrivateKey_file(openSSLCtx_, serverKey_.c_str(), SSL_FILETYPE_PEM) <= 0) {
                LogError() << "ServerListen::Start|load key fail, key = " << serverKey_.c_str()
                           << "certificate = " << serverCrt_.c_str();
                ERR_print_errors_fp(stderr);
                return -1;
            }

            // Verify private key matches certificate
            if (!SSL_CTX_check_private_key(openSSLCtx_)) {
                LogError() << "ServerListen::Start|key not match certificate, key = " << serverKey_.c_str()
                           << "certificate = " << serverCrt_.c_str();
                ERR_print_errors_fp(stderr);
                return -1;
            }
        } else {
            LogWarn() << "ServerListen::Start|serverCrt or serverKey is empty, serverKey =" << serverKey_.c_str()
                      << ",serverCrt =" << serverCrt_.c_str();
            return -1;
        }

        // Set client certificate verification
        if (!ca_.empty()) {
            // Load CA certificate
            if (SSL_CTX_load_verify_locations(openSSLCtx_, ca_.c_str(), nullptr) <= 0) {
                LogError() << "ServerListen::Start|load ca fial, ca_ = " << ca_;
                ERR_print_errors_fp(stderr);
                return -1;
            }
            SSL_CTX_set_verify(openSSLCtx_, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, nullptr);
        } else {
            LogWarn() << "ServerListen::Start|ca is empty, do not verify client";
        }
        SSL_CTX_set_verify_depth(openSSLCtx_, 4);
        isEnableTls = true;
    }
#endif

    newConnectionCallback_ = cb;
    LogInfo() << "ServerListen::Start|success address:" << address.c_str() << " port:" << port.c_str()
              << "isUseTls =" << isUseTls_ << "isEnableTls =" << isEnableTls;

    return res;
}

/// @brief Stop listening service
/// @return Operation result
int32_t ServerListen::Stop()
{
    int32_t r{0};
#ifdef NAI_ENABLE_SSL
    if (isUseTls_) {
        // r = nai_ssl_close(&ssl_);
        // if (r == -1) {
        //     LogWarn() << "ServerListen::Stop|nai_ssl_close fails!" << isoft::threadsafe::Safe_Strerror(errno);
        // }

        // r = nai_ssl_ctx_close(&ctx_);
        // if (r == -1) {
        //     LogWarn() << "ServerListen::Stop|nai_ssl_ctx_close fails!" << isoft::threadsafe::Safe_Strerror(errno);
        // }

        SSL_CTX_free(openSSLCtx_);
        openSSLCtx_ = nullptr;
    }
#endif

    r = nai_server_close(&listen_);
    if (r == -1) {
        LogWarn() << "ServerListen::Stop|nai_server_close fails!" << isoft::threadsafe::Safe_Strerror(errno);
    }
    return 0;
}

/// @brief Accept a TCP connection
/// @return Accept result
int32_t ServerListen::_accept()
{
    nai_fd_t f{};
    while (true) {
        nai_socknbuf_t naiSocknbuf{};
        naiSocknbuf.len = static_cast< int32_t >(sizeof(naiSocknbuf.storage));
        f               = nai_server_accept(&listen_, &naiSocknbuf.addr, &naiSocknbuf.len);
        if (f == NAI_FD_INVALID) {
            break;
        }
        std::unique_ptr< PublicTcpStream > stream{std::make_unique< PublicTcpStream >()};
        if (stream.get() == nullptr) {
            LogError() << "ServerListen::_accept|Stream create fails!";
            return -1;
        }
        int32_t res{stream->Initialize()};
        if (res == -1) {
            LogError() << "ServerListen::_accept|Stream init fails!"
                       << " error:" << isoft::threadsafe::Safe_Strerror(errno);
            return -1;
        }
        res = stream->SetFd(f, NAI_FD_TYPE_SOCK);
        if (res == -1) {
            LogError() << "ServerListen::_accept|Stream set fd fails!"
                       << " error:" << isoft::threadsafe::Safe_Strerror(errno);
            return -1;
        }
        res = stream->Inherit(&listen_);
        if (res == -1) {
            LogError() << "ServerListen::_accept|Stream inherit fails!"
                       << " error:" << isoft::threadsafe::Safe_Strerror(errno);
            return -1;
        }
        res = stream->Setopt(NAI_IO_NODELAY, 1);
        if (res == -1) {
            LogError() << "ServerListen::Start|Setopt NAI_IO_NODELAY error:" << isoft::threadsafe::Safe_Strerror(errno);
            return res;
        }
        res = stream->Open(evloop_);
        if (res == -1) {
            LogError() << "ServerListen::_accept|Stream open fails!"
                       << " error:" << isoft::threadsafe::Safe_Strerror(errno);
            return -1;
        }

        // #ifdef NAI_ENABLE_SSL
        //         if (isUseTls_) {
        //             res = stream->InitializeTLS(ssl_);
        //             if (res == -1) {
        //                 LogError() << "ServerListen::_accept|InitializeTLS fails!"
        //                            << " error:" << isoft::threadsafe::Safe_Strerror(errno);
        //                 return -1;
        //             }
        //         }
        // #endif
        newConnectionCallback_(std::move(stream), naiSocknbuf, this);
        LogDebug() << "ServerListen::_accept|Stream new connection be coming fd:" << f;
    }

    return 0;
}

#ifdef NAI_ENABLE_SSL
bool ServerListen::_initTLS() noexcept
{
    if (serverCrt_.empty()) {
        LogError() << "ServerListen::_initTLS|server_crt is empty";
        return false;
    }

    if (serverKey_.empty()) {
        LogError() << "ServerListen::_initTLS|server_key is empty";
        return false;
    }

    nai_ssl_ctx_init(&ctx_);

    int32_t r = nai_ssl_ctx_open(&ctx_, NAI_SSL_TLS);
    if (r == -1) {
        LogError() << "ServerListen::_initTLS|nai_ssl_ctx_open fails!"
                   << " error:" << isoft::threadsafe::Safe_Strerror(errno);
        return false;
    }

    nai_str_t serverCrt = nai_strconst(const_cast< char* >(serverCrt_.c_str()));
    nai_str_t serverKey = nai_strconst(const_cast< char* >(serverKey_.c_str()));
    r                   = nai_ssl_ctx_certificate(&ctx_, &serverCrt, &serverKey, 0, 0);
    if (r == -1) {
        LogError() << "ServerListen::_initTLS|nai_ssl_ctx_certificate fails!"
                   << " error:" << isoft::threadsafe::Safe_Strerror(errno);
        return false;
    }

    r = nai_ssl_ctx_verify_depth(&ctx_, 1);
    if (r == -1) {
        LogError() << "ServerListen::_initTLS|nai_ssl_ctx_verify_depth fails!"
                   << " error:" << isoft::threadsafe::Safe_Strerror(errno);
        return false;
    }

    nai_ssl_init(&ssl_);
    nai_ssl_set_ops(&ssl_, &tls_ops, &listen_);

    r = nai_ssl_open(&ssl_, &ctx_, 0);
    if (r == -1) {
        LogError() << "ServerListen::_initTLS|nai_ssl_open fails!"
                   << " error:" << isoft::threadsafe::Safe_Strerror(errno);
        return false;
    }

    return true;
}
#endif

}  // namespace doip
}  // namespace isoft