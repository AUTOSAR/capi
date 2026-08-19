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
/// @file       public_tcp_stream.cpp
/// @brief      This file provides DOIP common NAI low-level encapsulation stream definition related
/// @details
/// @date       2023-10-08
/// @author     zhanglipeng
/// @version    1.2.0
///
/// ================================================================

#include "public_tcp_stream.h"

#include "public.h"
#include "public_log.h"
#include "public_tls_opt.h"
#include "thread/therad_safe.h"
namespace isoft {
namespace doip {

/// @brief Constructor
PublicTcpStream::PublicTcpStream() = default;

/// @brief Destructor
PublicTcpStream::~PublicTcpStream() noexcept { std::ignore = Shutdown(); }

/// @brief Initialize
/// @return Initialization result
int32_t PublicTcpStream::Initialize() { return nai_stream_init(&stream_); }

#ifdef NAI_ENABLE_SSL
int32_t PublicTcpStream::InitializeTLS(nai_ssl_t const& ssl) noexcept
{
    nai_ssl_init(&ssl_);
    nai_ssl_set_ops(&ssl_, &tls_ops, &stream_);

    int32_t r = nai_ssl_open(&ssl_, ssl.ctx, 0);
    if (r == -1) {
        LogError() << "PublicTcpStream::InitializeTLS|nai_ssl_open fails:" << isoft::threadsafe::Safe_Strerror(errno);
        return r;
    }

    r = nai_ssl_enable_load(&ssl_, 1);
    if (r == -1) {
        LogError() << "PublicTcpStream::InitializeTLS|nai_ssl_enable_load fails:"
                   << isoft::threadsafe::Safe_Strerror(errno);
        return r;
    }

    r = nai_ssl_enable_ktls(&ssl_, 1);
    if (r == -1) {
        LogError() << "PublicTcpStream::InitializeTLS|nai_ssl_enable_ktls fails:"
                   << isoft::threadsafe::Safe_Strerror(errno);
        return r;
    }

    return r;
}
#endif

/// @brief Connect to peer
/// @param[in] evLoop Event loop
/// @param[in] addressPort IP and port
/// @return Result
int32_t PublicTcpStream::Connect(std::shared_ptr< naicpp::EvLoop > evLoop, ara::core::String addressPort)  // NOLINT
{
    nai_socknbuf_t list;
    list.len    = static_cast< int32_t >(sizeof(list.storage));
    int32_t res = nai_sockaddr_pton(addressPort.data(), addressPort.size(), &list.addr, &list.len);
    if (res == -1) {
        LogError() << "PublicTcpStream::Connect|Stream init fails:" << isoft::threadsafe::Safe_Strerror(errno);
        return res;
    }
    return nai_stream_connect(&stream_, evLoop->GetRawEvLoop(), &list.addr, sizeof(list.addr));
}

/// @brief Read data from socket stream
/// @param[in] buf Buffer address to read
/// @param[in] len Buffer length to read
/// @return Read result
int64_t PublicTcpStream::Read(void* const buf, size_t const len, bool isUseTls)
{
    if (isUseTls) {
        // #ifdef NAI_ENABLE_SSL
        //         return nai_ssl_read(&ssl_, buf, len);
        // #endif
        //         LogError() << "PublicTcpStream::Read|no nai tls lib";
        //         return -1;
    }

    return nai_stream_read(&stream_, buf, len);
}

/// @brief Write data to socket stream
/// @param[in] buf Buffer address to write
/// @param[in] len Buffer length to write
/// @return Write result
int64_t PublicTcpStream::Write(const void* const buf, size_t const len, bool isUseTls)
{
    if (isUseTls) {
        // #ifdef NAI_ENABLE_SSL
        //         return nai_ssl_write(&ssl_, buf, len);
        // #endif
        //         LogError() << "PublicTcpStream::Write|no nai tls lib";
        //         return -1;
    }

    return nai_stream_write(&stream_, buf, len);
}

/// @brief Get peer IP and port
/// @param[out] ip
/// @param[out] port
void PublicTcpStream::GetPeerIpAndPort(ara::core::String& ip, std::uint16_t& port) const
{
    std::ignore = ip;
    std::ignore = port;

    nai_socknbuf_t nbuf;
    uint32_t const addrLen{128U};
    char addr[addrLen]{'0'};
    nbuf.len = static_cast< int32_t >(sizeof(nbuf.addr));
    int64_t res{nai_sock_get_peername(nai_stream_get_fd(&stream_), &nbuf.addr, &nbuf.len)};
    if (res == -1) {
        LogError() << "PublicTcpStream::GetPeerIpAndPort|get sock name fails! error:"
                   << isoft::threadsafe::Safe_Strerror(errno);
        return;
    }
    res = nai_sockaddr_ntop(&nbuf.addr, nbuf.len, addr, sizeof(addr), 0);
    if (res == -1) {
        LogError() << "PublicTcpStream::GetPeerIpAndPort|ntop fails! error:" << isoft::threadsafe::Safe_Strerror(errno);
        return;
    }
    ip = ara::core::String(addr);
    /* get port */
    nai_sockaddr_info_t const* const i{nai_sockaddr_info(static_cast< int32_t >(nbuf.addr.sa_family))};
    if (i != nullptr) {
        port = static_cast< uint16_t >(nai_sockaddr_get_port(i, &nbuf.addr));  // NOLINT
    }
}

/// @brief Get local IP and port
/// @param[out] ip
/// @param[out] port
void PublicTcpStream::GetLocalIpAndPort(ara::core::String& ip, std::uint16_t& port) const
{
    std::ignore = ip;
    std::ignore = port;

    nai_socknbuf_t nbuf;
    uint32_t const addrLen{128U};
    char addr[addrLen]{'0'};
    nbuf.len = static_cast< int32_t >(sizeof(nbuf.addr));
    int64_t res{nai_sock_get_sockname(nai_stream_get_fd(&stream_), &nbuf.addr, &nbuf.len)};
    if (res == -1) {
        LogError() << "PublicTcpStream::GetLocalIpAndPort|get sock name fails! error:" << res;
        return;
    }
    res = nai_sockaddr_ntop(&nbuf.addr, nbuf.len, addr, sizeof(addr), 0);
    if (res == -1) {
        LogError() << "PublicTcpStream::GetLocalIpAndPort|get sock name fails! error:" << res;
        return;
    }
    ip = ara::core::String(addr);
    /* get port */
    nai_sockaddr_info_t const* const i{nai_sockaddr_info(static_cast< int32_t >(nbuf.addr.sa_family))};
    if (i != nullptr) {
        port = static_cast< uint16_t >(nai_sockaddr_get_port(i, &nbuf.addr));  // NOLINT
    }
}

/// @brief Set new message callback
/// @param[in] cb Callback
/// @return Set result
int32_t PublicTcpStream::SetNewMsgCallback(TcpNewMsgCallback cb)
{
    newMsgCallback_ = std::move(cb);
    return nai_stream_set_cb(&stream_, [](nai_stream_t* p, int events) {
        PublicTcpStream* stream = reinterpret_cast< PublicTcpStream* >(
            reinterpret_cast< char* >(p)
            - reinterpret_cast< uint64_t >(&reinterpret_cast< PublicTcpStream* >(0)->stream_));
        return stream->newMsgCallback_(stream, events);
    });
}

}  // namespace doip
}  // namespace isoft