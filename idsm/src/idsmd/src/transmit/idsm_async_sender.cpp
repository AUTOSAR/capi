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
/// @file       idsm_async_sender.cpp
/// @brief      Asynchronous sender: This sender uses a separate thread to interact with the event generation thread via a security event pool
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
/// @unit_name=AsyncSender
/// @unit_description=
/// @endcode
///
/// ================================================================

#include "idsm_async_sender.h"

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <chrono>
#include <thread>

#include "ara/idsm/internal/common.h"
#include "log/idsm_log.h"
#include "nai/os/nai_socket.h"
namespace ara {
namespace idsm {
/// @brief Send data to peer
/// @param eventTrans Data to transmit
/// @param highPriority Whether the transmitted data is urgent
/// @return Number of bytes sent, returns -1 on error
/// @exception Stack overflow exception
/// @code{.isoft}
/// @threadsafety={unsafe}
/// @endcode
int64_t AsyncSender::Send(Message const& eventTrans, bool highPriority)
{
    if (highPriority) {
        errMsg_ = eventTrans;
        return 0;
    }

    {
        std::unique_lock< std::mutex > const lck{condiMutex_};
        records_.push_back(eventTrans);
        cond_.notify_one();
    }

    return 0;
}
/// @brief Register callback function
/// @param callback Callback function for data transmission status
/// @exception Stack overflow exception
void AsyncSender::RegisterCallback(SendCallBack const& callback) { handler_ = callback; }
/// @brief Thread entry function
/// @param thrArg Thread entry parameter, AsyncSender object
/// @exception Stack overflow exception
/// @code{.isoft}
/// @threadsafety={unsafe}
/// @endcode
void AsyncSender::Run(void* const thrArg)
{
    AsyncSender* const sender{static_cast< AsyncSender* >(thrArg)};
    while (true) {
        std::list< Message > events;
        sender->_get(events);
        while (true) {
            if (events.empty()) {
                break;
            }
            Message const& eventTrans{events.front()};
            int64_t bytes{sender->_syncWrite(eventTrans)};
            TransDataPtr transData{eventTrans.GetData()};
            int64_t eventSize{static_cast< int64_t >(transData->Size())};
            if (bytes == eventSize) {
                events.pop_front();
                continue;
            }
            /// @details Data write failed, trigger data write failure callback
            sender->handler_(transData, false);
            TransDataPtr transErrData{sender->errMsg_.GetData()};
            eventSize = static_cast< int64_t >(transErrData->Size());
            while (true) {
                /// @details Asynchronous data send failed: repeatedly probe network, every 2 seconds
                /// @details When network recovers, send communication failure security event
                bytes = sender->_syncWrite(sender->errMsg_);
                if (bytes == eventSize) {
                    break;
                }
                uint8_t const timeSleep{2U};
                std::this_thread::sleep_for(std::chrono::seconds(timeSleep));
            }
        }
    }
}
/// @brief Start asynchronous write thread
/// @exception Stack overflow exception
/// @code{.isoft}
/// @threadsafety={unsafe}
/// @endcode
void AsyncSender::_init()
{
    records_.clear();
    std::thread t{AsyncSender::Run, this};
    t.detach();
}
/// @brief Establish connection to peer
/// @return 0 on success, -1 on failure
/// @exception Stack overflow exception
/// @code{.isoft}
/// @threadsafety={unsafe}
/// @endcode
int32_t AsyncSender::_createSocket()
{
    /// @details Create new socket object
    fd_ = nai_sock_open(AF_INET, SOCK_STREAM, 0);
    if (fd_ == static_cast< nai_fd_t >(NAI_FD_INVALID)) {
        LOG_WARN << "idsm qualified security event async transmit: socket func fail. errno: " << SysErr();
        return -1;
    }
    /// @details Construct remote address object
    struct sockaddr_in srvAddr
    {
    };
    std::ignore        = memset(&srvAddr, 0, sizeof(srvAddr));
    srvAddr.sin_family = static_cast< sa_family_t >(AF_INET);
    ara::core::String const pIpAddr{GetIpAddr()};
    int32_t const res{nai_inet_pton(AF_INET, pIpAddr.c_str(), pIpAddr.size(), &srvAddr.sin_addr.s_addr)};
    if (res != 1) {
        LOG_ERROR << "idsm qualified security event sync transmit: convert socket address fail errno:" << SysErr();
        return -1;
    }
    if (srvAddr.sin_addr.s_addr == 0U) {
        LOG_WARN << "idsm qualified security event sync transmit: ip address is 0.";
    }

    uint16_t const hostPort{GetPort()};
    srvAddr.sin_port = nai_htons_ua(&hostPort);
    /// @details Initiate network connection to remote address
    nai_int_t const srvAddrSize{static_cast< nai_int_t >(sizeof(srvAddr))};
    int32_t const ret{
        nai_sock_connect(fd_, static_cast< nai_sockaddr_t* >(static_cast< void* >(&srvAddr)), srvAddrSize)};
    if (ret == -1) {
        ara::core::String const address{GetIpAddr() + ":" + std::to_string(static_cast< int32_t >(GetPort()))};
        LOG_WARN << "idsm qualified security event async transmit: connect func fail. ip: " << address.c_str()
                 << ", errno:" << SysErr();
        std::ignore = nai_sock_close(fd_);
        fd_         = -1;
        return -1;
    }
    return 0;
}
/// @brief Get set of data to be sent
/// @param eventsTrans Set of data to be sent
/// @exception Stack overflow exception
void AsyncSender::_get(std::list< Message >& eventsTrans)
{
    std::unique_lock< std::mutex > lck{condiMutex_};
    while (records_.empty()) {
        cond_.wait(lck);
    }
    eventsTrans.swap(records_);
}
/// @brief Send data
/// @param eventTrans Data to send
/// @return Length of data sent, returns -1 on failure
/// @exception Stack overflow exception
int64_t AsyncSender::_syncWrite(Message const& eventTrans)
{
    if (fd_ == -1) {
        static uint64_t s_Counter{0U};
        if (_createSocket() != 0) {
            s_Counter += 1U;
            LOG_WARN << "idsm connect idsr fail. retry times:" << s_Counter;
            return -1;
        }
        s_Counter = 0U;
    }
    TransDataPtr transData{eventTrans.GetData()};
    size_t remainBytes{transData->Size()};
    int64_t writePos{0};
    while (true) {
        if (remainBytes <= 0U) {
            break;
        }
        int64_t const bytes{nai_sock_send(fd_, transData->Data() + writePos, remainBytes, MSG_NOSIGNAL)};
        if (bytes == -1) {
            std::ignore = nai_sock_close(fd_);
            fd_         = -1;
            break;
        }
        remainBytes -= static_cast< size_t >(bytes);
        writePos += bytes;
    }
    if (remainBytes != 0U) {
        LOG_ERROR << "idsm qualified security event sync transmit: synchronize write fail. errno: " << SysErr();
        return writePos;
    }
    return writePos;
}

}  // namespace idsm
}  // namespace ara