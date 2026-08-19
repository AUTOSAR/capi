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
/// @file       public_udp_dgram.cpp
/// @brief      This file provides DOIP common NAI low-level encapsulation dgram definition related
/// @details
/// @date       2023-10-08
/// @author     zhanglipeng
/// @version    1.2.0
///
/// ================================================================

#include "public_udp_dgram.h"

#include <ifaddrs.h>    // COMPATIBLE_WARN: for getting MAC Address
#include <net/if.h>     // COMPATIBLE_WARN: for getting MAC Address
#include <sys/ioctl.h>  // COMPATIBLE_WARN: for getting MAC Address

#include "public_log.h"
#include "thread/therad_safe.h"

namespace isoft {
namespace doip {

/// @brief Constructor
PublicUdpDgram::PublicUdpDgram() = default;

/// @brief Destructor
PublicUdpDgram::~PublicUdpDgram() noexcept { std::ignore = Shutdown(); }

/// @brief UDP initialization
/// @return Initialization result
int32_t PublicUdpDgram::Initialize() { return nai_dgram_init(&dgram_); }

/// @brief Set new message callback
/// @param[in] callback Callback function
/// @return Set result
int32_t PublicUdpDgram::SetNewMsgCallback(UdpNewMsgCallback const& callback)
{
    newMsgCallback_ = callback;
    return nai_dgram_set_cb(&dgram_, [](nai_dgram_t* const p, int32_t const events) -> int32_t {
        LogInfo() << "PublicUdpDgram::SetNewMsgCallback|read data, events:" << events;
        PublicUdpDgram* const dgram{reinterpret_cast< PublicUdpDgram* >(
            reinterpret_cast< char* >(p)
            - reinterpret_cast< uint64_t >(&reinterpret_cast< PublicUdpDgram* >(0)->dgram_))};
        if (dgram == nullptr) {
            LogError() << "PublicUdpDgram::SetNewMsgCallback|dgram cb dgram is nullptr";
            return -1;
        }
        return dgram->newMsgCallback_(dgram, events);
    });
}

/// @brief Bind address and port
/// @param[in] evLoop Event loop
/// @param[in] address Bind address
/// @param[in] port Bind port
/// @return Bind result
int32_t PublicUdpDgram::Bind(std::shared_ptr< naicpp::EvLoop > const& evLoop,
                             ara::core::String const& address,
                             ara::core::String const& port)
{
    nai_socknbuf_t list;
    list.len = static_cast< int32_t >(sizeof(list.storage));
    ara::core::String const addressPort{address + ":" + port};
    std::int32_t const res{nai_sockaddr_pton(addressPort.data(), addressPort.size(), &list.addr, &list.len)};
    if (res == -1) {
        LogError() << "PublicUdpDgram::Bind|dgram init fails:" << isoft::threadsafe::Safe_Strerror(errno);
        return res;
    }

    if (evLoop.get() == nullptr) {
        LogError() << "PublicUdpDgram::Bind|dgram evLoop is nullptr";
        return -1;
    }

    return nai_dgram_bind(&dgram_, evLoop->GetRawEvLoop(), &list.addr, static_cast< int32_t >(sizeof(list.addr)));
}

/// @brief Connect to peer
/// @param[in] evLoop Event loop
/// @param[in] address Peer address
/// @return Connection result
int32_t PublicUdpDgram::Connect(std::shared_ptr< naicpp::EvLoop > const& evLoop, ara::core::String const& address)
{
    nai_socknbuf_t list;
    list.len = static_cast< int32_t >(sizeof(list.storage));
    std::int32_t const res{nai_sockaddr_pton(address.data(), address.size(), &list.addr, &list.len)};
    std::ignore = list.len;
    if (res == -1) {
        LogInfo() << "PublicUdpDgram::Connect|dgram init fails" << isoft::threadsafe::Safe_Strerror(errno);
        return res;
    }

    if (evLoop.get() == nullptr) {
        LogError() << "PublicUdpDgram::Connect|dgram evLoop is nullptr";
        return -1;
    }

    return nai_dgram_connect(&dgram_, evLoop->GetRawEvLoop(), &list.addr, static_cast< int32_t >(sizeof(list.addr)));
}

/// @brief Close socket
/// @return Close result
int32_t PublicUdpDgram::Close() { return nai_dgram_close(&dgram_); }

/// @brief Receive UDP data
/// @param[in] buf Receive buffer
/// @param[in] len Receive buffer size
/// @param[in] name Peer address
/// @param[in] namelen Address size
/// @return Receive result
int64_t PublicUdpDgram::Recv(void* const buf, size_t const len, nai_sockaddr_t* const name, std::int32_t* const namelen)
{
    return nai_dgram_recvfrom(&dgram_, buf, len, 0, name, namelen);
}

/// @brief Send UDP data
/// @param[in] buf Send buffer
/// @param[in] len Send buffer size
/// @param[in] name Peer address
/// @param[in] namelen Address size
/// @return Send result
int64_t PublicUdpDgram::Send(void const* const buf,
                             size_t const len,
                             nai_sockaddr_t const* const name,
                             std::int32_t const namelen)
{
    return nai_dgram_sendto(&dgram_, buf, len, 0, name, namelen);
}

/// @brief Send UDP data
/// @param[in] buf Send buffer
/// @param[in] len Send buffer size
/// @param[in] address Peer address
/// @return Send result
int64_t PublicUdpDgram::Send(void const* const buf, size_t const len, ara::core::String const& address)
{
    nai_socknbuf_t list;
    list.len = static_cast< int32_t >(sizeof(list.storage));
    std::int64_t res{nai_sockaddr_pton(address.data(), address.size(), &list.addr, &list.len)};
    if (res == -1) {
        LogError() << "PublicUdpDgram::Send|dgram nai_sockaddr_pton err:" << isoft::threadsafe::Safe_Strerror(errno);
        return res;
    }

    res = Send(buf, len, &list.addr, list.len);
    if (res == -1) {
        LogError() << "PublicUdpDgram::Send|err:" << isoft::threadsafe::Safe_Strerror(errno);
    }
    return res;
}

/// @brief Set broadcast attribute
/// @return Set result
int32_t PublicUdpDgram::EnableBroadcast() const
{
    std::int32_t broadcastEnable{1};
    return nai_sock_set_opt(nai_dgram_get_fd(&dgram_), SOL_SOCKET, SO_BROADCAST,
                            reinterpret_cast< const char* >(&broadcastEnable),
                            static_cast< int32_t >(sizeof(broadcastEnable)));
}

/// @brief Get MAC
/// @param[in] localAddress Local address
/// @return mac
MacAddress PublicUdpDgram::GetMacAddress(nai_sockaddr_t* const localAddress) const
{
    MacAddress macAddress{};
    bool success{false};
    struct sockaddr_in* const localInt4Address{reinterpret_cast< struct sockaddr_in* >(localAddress)};
    try {
        struct ifaddrs* firstInterface{};
        if ((getifaddrs(&firstInterface) == 0)) {
            struct ifaddrs* currentInterface{};
            for (currentInterface = firstInterface; currentInterface != nullptr;
                 currentInterface = currentInterface->ifa_next) {
                if (currentInterface->ifa_addr == nullptr) {
                    continue;
                }

                if (currentInterface->ifa_addr->sa_family == static_cast< uint16_t >(AF_INET)) {
                    bool const isNameEqual{(strcmp("lo", currentInterface->ifa_name) == 0)};
                    if (((currentInterface->ifa_flags & static_cast< uint32_t >(IFF_LOOPBACK)) != 0U) || isNameEqual) {
                        continue;
                    }

                    struct sockaddr_in* const currentSocketAddress{
                        reinterpret_cast< struct sockaddr_in* >(currentInterface->ifa_addr)};
                    if (localInt4Address->sin_addr.s_addr == currentSocketAddress->sin_addr.s_addr) {
                        struct ifreq macBuffer
                        {
                        };
                        std::ignore = strcpy(macBuffer.ifr_name, currentInterface->ifa_name);  // NOLINT

                        std::int32_t const ioctlResult{
                            ioctl(nai_dgram_get_fd(&dgram_), static_cast< uint64_t >(SIOCGIFHWADDR), &macBuffer)};
                        if (ioctlResult == 0) {
                            constexpr uint32_t kMacLength{6U};
                            for (uint32_t i{0U}; i < kMacLength; i++) {
                                macAddress[i] = (macBuffer.ifr_hwaddr.sa_data[i]);  // NOLINT
                            }
                            success = true;
                            break;
                        }  // else continue
                    }
                }
            }

            freeifaddrs(firstInterface);
        }
    } catch (...) {
        success = false;
    }

    if (!success) {
        // We did not find an interface with givenIp, or other errors occured.
        //   -> Return some kind of error
        // default value should be returned
        macAddress.fill(0U);
    }
    return macAddress;
}

/// @brief Start UDP listening
/// @param[in] evloop Event loop
/// @param[in] address IP address
/// @param[in] port port
/// @param[in] cb New message callback
/// @return Start result
int32_t PublicUdpDgram::Start(std::shared_ptr< naicpp::EvLoop > const& evloop,
                              ara::core::String const& address,
                              ara::core::String const& port,
                              UdpNewMsgCallback const& cb)
{
    int32_t res = Initialize();
    if (res == -1) {
        LogError() << "PublicUdpDgram::Start|Initialize err:" << isoft::threadsafe::Safe_Strerror(errno);
        return res;
    }

    res = nai_dgram_set_opt(&dgram_, NAI_IO_REUSEADDR, 1);
    if (res == -1) {
        LogError() << "PublicUdpDgram::Start|NAI_IO_REUSEADDR err:" << isoft::threadsafe::Safe_Strerror(errno)
                   << "fd =" << nai_dgram_get_fd(&dgram_);
        return res;
    }

    res = nai_dgram_set_opt(&dgram_, NAI_IO_REUSEPORT, 1);
    if (res == -1) {
        LogError() << "PublicUdpDgram::Start|NAI_IO_REUSEPORT err:" << isoft::threadsafe::Safe_Strerror(errno);
        return res;
    }

    res = Bind(evloop, address, port);
    if (res == -1) {
        LogError() << "PublicUdpDgram::Start|Bind err:" << isoft::threadsafe::Safe_Strerror(errno);
        return res;
    }

    res = EnableBroadcast();
    if (res == -1) {
        LogError() << "PublicUdpDgram::Start|EnableBroadcast err:" << isoft::threadsafe::Safe_Strerror(errno);
        return res;
    }

    res = SetNewMsgCallback(cb);
    if (res == -1) {
        LogError() << "PublicUdpDgram::Start|SetNewMsgCallback err:" << isoft::threadsafe::Safe_Strerror(errno);
        return res;
    }

    return res;
}

}  // namespace doip
}  // namespace isoft