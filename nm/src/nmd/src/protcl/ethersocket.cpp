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
/// @file       ethersocket.cpp
/// @brief      Socket management class
/// @details
/// @date       2022-06-10
/// @author     hejunwei
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @trace_id_sr=SRS_NM_00001,SRS_NM_00002
/// @unit_name=EtherSocket
/// @unit_description=Socket management class
/// @module_path=/NetworkManager/protcl
/// @interface_level=/NetworkManager/protcl
/// @endcode
///
/// ================================================================

#include "include/ethersocket.h"

#include <nai/os/nai_file.h>
#include <nai/os/nai_socket.h>

#include <memory>

#include "common/common.h"
#include "config/include/configure.h"
#include "utils/include/utils.h"

namespace ara {
namespace nm {
namespace internal {

/// @brief create multicast socket.
/// @param localIpAddress   unicast ip address of ethernet communicator.
/// @param mulcastIpAddress multicast ip address of udp cluster.
/// @param mulcastPort      multicast port of udp cluster.
/// @param pduLength        pdu length.
/// @returns                0 ok.
std::int32_t EtherSocket::OpenSocket(ara::core::String const &localIpAddress,
                                     ara::core::String const &mulcastIpAddress,
                                     std::uint32_t const mulcastPort,
                                     std::uint32_t const pduLength) noexcept
{
    nai_fd_t const fd{nai_sock_open(AF_INET, SOCK_DGRAM, 0)};
    if (fd == -1) {
        NmLogger().LogError() << "EtherSocket::OpenSocket, create socket failed localIpAddress="
                              << localIpAddress.c_str();
        return -1;
    }
    socketFd_ = fd;
    std::uint8_t const loopVal{0U};
    struct ip_mreq mulcastSer
    {
    };

    nai_sockaddr_in4_t serv;
    static_cast< void >(memset(&serv, 0, sizeof(serv)));
    serv.sin_family = static_cast< std::uint16_t >(AF_INET);
    serv.sin_port   = htons(static_cast< std::uint16_t >(mulcastPort));
    static_cast< void >(inet_pton(AF_INET, mulcastIpAddress.data(), &serv.sin_addr.s_addr));
    std::ignore = serv;  // just for qac
    std::int32_t retCode{
        nai_sock_bind(fd, reinterpret_cast< struct sockaddr * >(&serv), static_cast< std::int32_t >(sizeof(serv)))};
    if (retCode == -1) {
        NmLogger().LogError() << "EtherSocket::OpenSocket, socket bind failed localIpAddress="
                              << localIpAddress.c_str();
        static_cast< void >(nai_sock_close(socketFd_));
        return retCode;
    }

    mulcastSer.imr_multiaddr.s_addr = inet_addr(mulcastIpAddress.data());
    mulcastSer.imr_interface.s_addr = inet_addr(localIpAddress.data());
    retCode = nai_sock_set_opt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, reinterpret_cast< NmChar const * >(&mulcastSer),
                               static_cast< std::int32_t >(sizeof(mulcastSer)));
    if (0 > retCode) {
        NmLogger().LogError() << "EtherSocket::OpenSocket, setsockopt "
                                 "IP_ADD_MEMBERSHIP error localIpAddress="
                              << localIpAddress.c_str();
        static_cast< void >(nai_sock_close(socketFd_));
        return retCode;
    }
    // Set the default network interface for multicast; otherwise, sending will fail if there is no gateway
    struct in_addr interfaceAddr
    {
    };
    interfaceAddr.s_addr = inet_addr(localIpAddress.c_str());
    retCode = nai_sock_set_opt(fd, IPPROTO_IP, IP_MULTICAST_IF, reinterpret_cast< NmChar const * >(&interfaceAddr),
                               static_cast< std::int32_t >(sizeof(interfaceAddr)));
    if (0 > retCode) {
        NmLogger().LogError() << "EtherSocket::OpenSocket, setsockopt "
                                 "IP_MULTICAST_IF error localIpAddress="
                              << localIpAddress.c_str();
        static_cast< void >(nai_sock_close(socketFd_));
        return retCode;
    }
    if (0 != nai_stream_init(&netStream_)) {
        NmLogger().LogError() << "EtherSocket::OpenSocket, nai_stream_init error localIpAddress="
                              << localIpAddress.c_str();
        static_cast< void >(nai_sock_close(socketFd_));
        return -1;
    }
    if (0 > nai_stream_set_fd(&netStream_, socketFd_, NAI_FD_TYPE_SOCK)) {
        NmLogger().LogError() << "EtherSocket::OpenSocket, nai_stream_set_fd error localIpAddress="
                              << localIpAddress.c_str();
        static_cast< void >(nai_sock_close(socketFd_));
        return -1;
    }

    if (0 > nai_stream_set_cb(&netStream_, &RecvNmMessageHandle)) {
        NmLogger().LogError() << "EtherSocket::OpenSocket, nai_stream_set_cb error localIpAddress="
                              << localIpAddress.c_str();
        static_cast< void >(nai_sock_close(socketFd_));
        return -1;
    }

    if (0 > nai_stream_open(&netStream_, isoft::naicpp::GlobalGeneralEvLoop::Get()->GetRawEvLoop())) {
        NmLogger().LogError() << "EtherSocket::OpenSocket, nai_stream_open error localIpAddress="
                              << localIpAddress.c_str();
        static_cast< void >(nai_sock_close(socketFd_));
        return -1;
    }
    std::size_t bufLen{pduLength};
    bufLen   = bufLen + kSocketExtBuffer;
    pBuffer_ = std::make_unique< std::uint8_t[] >(bufLen);
    static_cast< void >(memset(pBuffer_.get(), 0, bufLen));
    // forbid multicast loopback,
    retCode           = setsockopt(fd, IPPROTO_IP, IP_MULTICAST_LOOP, &loopVal, sizeof(loopVal));
    errCode_          = 0;
    localIpAddress_   = localIpAddress;
    mulcastIpAddress_ = mulcastIpAddress;
    mulcastPort_      = mulcastPort;
    pduLength_        = pduLength;
    return retCode;
}

/// @brief destructor of EtherSocket.
EtherSocket::~EtherSocket() noexcept
{
    if (0 == errCode_) {
        if (0 > nai_stream_close(&netStream_)) {
        }
    }
    if (-1 != socketFd_) {
        static_cast< void >(nai_sock_close(socketFd_));
    }
    pBuffer_ = nullptr;
}

/// @brief socket message reciver call back.
/// @param stream           stream handle pointer.
/// @param events           socket events specific.
/// @returns                0 ok.
std::int32_t EtherSocket::RecvNmMessageHandle(nai_stream_t *const stream, std::int32_t const events) noexcept
{
    std::uint32_t const evtError{static_cast< std::uint32_t >(NAI_EV_ERROR)};
    std::uint32_t const evtRead{static_cast< std::uint32_t >(NAI_EV_READ)};
    std::uint32_t const newEvt{static_cast< std::uint32_t >(events)};

    if (evtError == (newEvt & evtError)) {
        return -1;
    }
    EtherSocket *const pEtherSocket{
        nai_containof(reinterpret_cast< NmChar * >(stream), EtherSocket, netStream_)};  // NOLINT

    if (evtRead == (newEvt & evtRead)) {
        static_cast< void >(pEtherSocket->_recvNmMessage());
    }
    return 0;
}

/// @brief process of receiving protocol message.
/// @returns                kNmOperOK ok.
NmOperCode EtherSocket::_recvNmMessage() noexcept
{
    std::int32_t length{0};
    static_cast< void >(
        memset(pBuffer_.get(), 0, (static_cast< size_t >(pduLength_) + static_cast< size_t >(kSocketExtBuffer))));
    while (true) {
        std::size_t const recvLen{pduLength_};
        std::int32_t const sktFd{socketFd_};
        length = static_cast< std::int32_t >(
            recvfrom(sktFd, pBuffer_.get() + lastBufferLen_, recvLen - lastBufferLen_, MSG_DONTWAIT, nullptr, nullptr));
        if (length > 0) {
            lastBufferLen_ += static_cast< std::uint32_t >(length);
            if (pduLength_ == lastBufferLen_) {
                (msgCallBack_(pBuffer_.get(), lastBufferLen_));
                static_cast< void >(memset(pBuffer_.get(), 0, recvLen + kSocketExtBuffer));
                lastBufferLen_ = 0U;
            }
        } else {
            break;
        }
    }
    return NmOperCode::kNmOperOK;
}

/// @brief send nm message.
/// @param pktBuffer           message buffer.
/// @param bufferLenth         message length.
/// @returns                   On success, return the number of bytes sent.  On
/// error, -1 is returned
std::int32_t EtherSocket::SendNmMessage(std::uint8_t const pktBuffer[], std::size_t const bufferLenth) const noexcept
{
    struct sockaddr_in mulcastDesAddr
    {
    };
    static_cast< void >(memset(&mulcastDesAddr, 0, sizeof(mulcastDesAddr)));
    mulcastDesAddr.sin_family = static_cast< std::uint16_t >(AF_INET);
    mulcastDesAddr.sin_port   = htons(static_cast< std::uint16_t >(mulcastPort_));
    static_cast< void >(inet_pton(AF_INET, mulcastIpAddress_.data(), &mulcastDesAddr.sin_addr.s_addr));
    std::ignore = mulcastDesAddr;  /// qac4127
    return static_cast< std::int32_t >(sendto(socketFd_, pktBuffer, bufferLenth, 0,
                                              reinterpret_cast< struct sockaddr * >(&mulcastDesAddr),
                                              sizeof(mulcastDesAddr)));
}

}  // namespace internal
}  // namespace nm
}  // namespace ara
