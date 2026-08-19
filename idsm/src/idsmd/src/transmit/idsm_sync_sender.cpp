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
/// @file       idsm_sync_sender.cpp
/// @brief      Synchronous data sender: data sending will block security event processing
/// @details
/// @date       2023-02-16
/// @author     niuliming
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/idsm/qualified security event storage
/// @interface_level=unit
/// @trace_id_sr=SR_IDSM_0011
/// @unit_name=SyncSender
/// @unit_description=
/// @endcode
///
/// ================================================================

#include "idsm_sync_sender.h"

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "ara/idsm/internal/common.h"
#include "config/idsm_struct_init.h"
#include "log/idsm_log.h"
#include "nai/os/nai_socket.h"

namespace ara {
namespace idsm {
/// @brief Send data to peer
/// @param data Data to transmit
/// @param highPriority Callback function called upon transmission completion
/// @return Number of bytes sent, returns -1 on error
/// @exception Stack overflow exception
/// @code{.isoft}
/// @threadsafety={unsafe}
/// @endcode
int64_t SyncSender::Send(Message const& data, bool highPriority)
{
    std::ignore = highPriority;

    if (fd_ == -1) {
        static uint64_t s_Counter{0U};
        /// @details Attempt to establish network connection to IDSR
        if (_createSocket() != 0) {
            s_Counter += 1U;
            LOG_WARN << "idsm connect idsr fail. retry times:" << s_Counter;
            return -1;
        }
        s_Counter = 0U;
    }
    TransDataPtr transData{data.GetData()};
    size_t const size{transData->Size()};
    int64_t const bytes{_write(transData->Data(), size)};
    if (bytes == -1) {
        return -1;
    }
    if (static_cast< size_t >(bytes) != size) {
        return -1;
    }
    return bytes;
}
/// @brief Establish connection to peer
/// @return 0 on success, -1 on failure
/// @exception Stack overflow exception
int32_t SyncSender::_createSocket()
{
    fd_ = nai_sock_open(AF_INET, SOCK_STREAM, 0);
    if (fd_ == -1) {
        LOG_ERROR << "idsm qualified security event sync transmit: nai create socket fail errno:" << SysErr();
        return -1;
    }
    struct sockaddr_in srvAddr
    {
    };
    std::ignore        = memset(&srvAddr, 0, sizeof(srvAddr));
    srvAddr.sin_family = static_cast< sa_family_t >(AF_INET);
    ara::core::String const pIpAddr{GetIpAddr()};
    int32_t res{nai_inet_pton(AF_INET, pIpAddr.c_str(), pIpAddr.size(), &srvAddr.sin_addr.s_addr)};
    if (res != 1) {
        LOG_ERROR << "idsm qualified security event sync transmit: convert socket address fail errno:" << SysErr();
        return -1;
    }
    if (srvAddr.sin_addr.s_addr == 0U) {
        LOG_WARN << "idsm qualified security event sync transmit: ip address is 0.";
    }
    uint16_t const hostPort{GetPort()};
    srvAddr.sin_port = nai_htons_ua(&hostPort);
    nai_int_t const srvAddrSize{static_cast< nai_int_t >(sizeof(srvAddr))};
    res = nai_sock_connect(fd_, static_cast< nai_sockaddr_t* >(static_cast< void* >(&srvAddr)), srvAddrSize);
    if (res == -1) {
        ara::core::String const address{GetIpAddr() + ":" + std::to_string(static_cast< int32_t >(GetPort()))};
        LOG_WARN << "idsm qualified security event sync transmit: connect func fail. ip: " << address.c_str()
                 << ", errno:" << SysErr();
        std::ignore = nai_sock_close(fd_);
        fd_         = -1;
        return -1;
    }
    return 0;
}
/// @brief Send data
/// @param data Data to send
/// @param size Length of data to send
/// @return Length of data sent, returns -1 on failure
/// @exception Stack overflow exception
int64_t SyncSender::_write(uint8_t const* const data, size_t const size)
{
    size_t writePos{0U};
    size_t remainBytes{size};
    while (true) {
        if (remainBytes <= 0U) {
            break;
        }
        int64_t const bytes{nai_sock_send(fd_, data + writePos, remainBytes, MSG_NOSIGNAL)};
        if (bytes == -1) {
            std::ignore = nai_sock_close(fd_);
            fd_         = -1;
            break;
        }
        remainBytes -= static_cast< size_t >(bytes);
        writePos += static_cast< size_t >(bytes);
    }
    if (remainBytes != 0U) {
        LOG_ERROR << "idsm qualified security event sync transmit: synchronize write fail. errno:" << SysErr();
        return -1;
    }
    return static_cast< int64_t >(writePos);
}

}  // namespace idsm
}  // namespace ara