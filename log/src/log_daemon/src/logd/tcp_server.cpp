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
/// @file       tcp_server.cpp
/// @brief      Template class for encapsulating log parameters
/// @details
/// @date       2024-06-28
/// @author     yangjinbiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/LOG/DltLogd
/// @interface_level = unit
/// @trace_id_sr=LOG_SR_00001,LOG_SR_00003,LOG_SR_00009
/// @unit_name = tcp_server
/// @unit_description=Backend of Dlt module, used to support log and command forwarding.
/// @endcode
///
/// ================================================================

#include "tcp_server.h"

#include <isoft/naicpp/evloop.h>
#include <isoft/naicpp/global_evloop.h>
#include <limits.h>
#include <nai/io/nai_io.h>
#include <nai/os/nai_socket.h>

#include <cstdlib>
#include <cstring>
#include <utility>

#include "Utils/src/private_log.h"
namespace ara {
namespace log {
namespace internal {

/// @brief
/// @param[in]  ip
/// @param[in]  port
/// @param[in]  lsner
TcpServer::TcpServer(std::string ip, std::uint16_t const &port, TcpServer::Listener *const lsner) noexcept
    : serverIp_{std::move(ip)}, serverPort_{port}, mListener_{lsner}
{
    LOGVERBOSE(__func__) << " enter ";
}

/// @brief
TcpServer::~TcpServer() noexcept
{
    LOGVERBOSE(__func__) << " enter ";

    // Set mainLoop_ to null here to ensure no further reference to the event loop during destruction
    mainLoop_ = nullptr;

    LOGVERBOSE(__func__) << " leave  ";
}
/// @brief
/// @return
std::int32_t TcpServer::Open() noexcept
{
    LOGVERBOSE(__func__) << " enter ";
    std::int32_t ret{0};

    mainLoop_ = isoft::naicpp::GlobalGeneralEvLoop::Get();
    if (mainLoop_ == nullptr) {
        LOGERROR("Get Global EvLoop failed") << __func__;
        return -1;
    }
    ret = this->_AddLisentPort(serverPort_);
    if (listenfd_ > 0) {
        LOGVERBOSE(__func__) << " listenfd_: " << listenfd_;
        AddStream(listenfd_, this);
    }
    LOGVERBOSE(__func__) << " leave  ";
    return ret;
}
/// @brief
/// @param[in]  dataBuffer
/// @param[in]  messageSize
/// @return
std::int32_t TcpServer::Send(uint8_t *const dataBuffer, std::size_t const &messageSize) noexcept
{
    LOGVERBOSE(__func__) << " enter ";
    std::uint8_t *const pSend{dataBuffer};
    for (const std::shared_ptr< StreamInfo > &s : streams_) {
        if (s->socketfd == listenfd_) {
            continue;
        }
        ssize_t dataSent{0};
        ssize_t const ssmessageSize{static_cast< ssize_t >(messageSize)};
        while (dataSent < ssmessageSize) {
            ssize_t const ret{nai_sock_send(s->socketfd, pSend + dataSent, messageSize - dataSent, 0)};
            if (ret < 0) {
                return -1;
            }
            dataSent += ret;
        }
    }
    LOGVERBOSE(__func__) << " leave  ";
    return static_cast< std::int32_t >(streams_.size() - 1);
}
/// @brief
/// @return
std::int32_t TcpServer::ClientCount() const noexcept
{
    LOGVERBOSE(__func__) << clientSet_.size();
    return static_cast< std::int32_t >(clientSet_.size());
}
/// @brief
/// @param[in]  socketfd
void TcpServer::_OnSocketRead(std::int32_t const &socketfd) noexcept
{
    LOGVERBOSE(__func__) << " enter socketfd: " << socketfd << " listenfd_: " << listenfd_
                         << " clientCount: " << clientSet_.size();
    if (socketfd == listenfd_) {
        nai_sockaddr_t cliaddr{};
        nai_int_t socklen{static_cast< nai_int_t >(sizeof(cliaddr))};
        nai_fd_t cliFd = nai_sock_accept(listenfd_, &cliaddr, &socklen);
        if (cliFd < 0) {
            LOGERROR("accept failed: ") << __func__;
            return;
        }
        LOGVERBOSE("client fd conncted : ") << cliFd << " count: " << clientSet_.size();

        SetNonBlocking(cliFd);
        AddStream(cliFd, this);
        std::ignore = clientSet_.insert(cliFd);
        if (mListener_ != nullptr) {
            mListener_->OnSocketStatus(true);
        }
    } else {
        if (mListener_ != nullptr) {
            std::int32_t ret{mListener_->OnSocketRead(socketfd)};
            LOGVERBOSE(__func__) << " OnSocketRead ret: " << ret << " fd: " << socketfd;
            if (ret != 0) {
                LOGVERBOSE(" socket close : ") << socketfd;
                LOGVERBOSE(__func__) << " trigger DeleteStream fd: " << socketfd;
                DeleteStream(socketfd);
                std::ignore = clientSet_.erase(socketfd);

                if (mListener_ != nullptr) {
                    mListener_->OnSocketStatus(false);
                }
                LOGVERBOSE(__func__) << " client erase done fd: " << socketfd
                                     << " remain clientCount: " << clientSet_.size();
            }
        }
    }
    LOGVERBOSE(__func__) << " leave  ";
}
/// @brief
/// @param[in]  socketfd
void TcpServer::_OnSocketClose(std::int32_t const &socketfd) noexcept
{
    LOGVERBOSE(__func__) << " enter socketfd: " << socketfd << " clientCount(before): " << clientSet_.size();

    std::ignore = clientSet_.erase(socketfd);
    LOGVERBOSE(__func__) << " client erase done socketfd: " << socketfd << " clientCount(after): " << clientSet_.size();

    // Note: The socket is automatically closed by nai_stream_close (if fdown=1 is set)
    // No need to manually call nai_sock_close here, otherwise it would cause double closing

    if (mListener_ != nullptr) {
        mListener_->OnSocketStatus(false);
    }
    LOGVERBOSE(__func__) << " leave  ";
}
/// @brief
/// @param[in]  port
std::int32_t TcpServer::_AddLisentPort(std::uint16_t const &port) noexcept
{
    LOGVERBOSE(__func__) << " enter ";
    if (port <= 0U) {
        return -1;
    }
    std::int32_t ret{0};
    listenfd_ = nai_sock_open(AF_INET, SOCK_STREAM, 0);
    if (listenfd_ == NAI_FD_INVALID) {
        LOGERROR("nai_sock_open failed") << __func__;
        return -1;
    }

    // Allow address reuse
    if (nai_sock_reuse(listenfd_, 1, NAI_SOCK_REUSEADDR) < 0) {
        LOGERROR("nai_sock_reuse failed, continue anyway") << __func__;
    }

    // Construct listen address and bind
    nai_sockaddr_t serverAddress{};
    nai_int_t addrLen = static_cast< nai_int_t >(sizeof(serverAddress));
    if (nai_sockaddr_mk_inet(AF_INET, nullptr, port, &serverAddress, &addrLen) < 0) {
        LOGERROR("nai_sockaddr_mk_inet failed") << __func__;
        nai_sock_close(listenfd_);
        listenfd_ = -1;
        return -1;
    }
    if (nai_sock_bind(listenfd_, &serverAddress, addrLen) < 0) {
        LOGERROR("nai_sock_bind failed") << __func__ << " port: " << port;
        nai_sock_close(listenfd_);
        listenfd_ = -1;
        return -1;
    }
    if (nai_sock_listen(listenfd_, SOMAXCONN) < 0) {
        LOGERROR("nai_sock_listen failed") << __func__;
        nai_sock_close(listenfd_);
        listenfd_ = -1;
        return -1;
    }
    LOGVERBOSE(__func__) << " leave  port: " << port << " listenfd: " << listenfd_;
    return ret;
}

/// @brief
/// @param[in]  fd
void TcpServer::SetNonBlocking(std::int32_t const &fd) noexcept
{
    LOGVERBOSE(__func__) << " enter ";
    if (nai_sock_set_blocking(fd, 0) < 0) {
        LOGERROR("set nonblock fail") << __func__;
    }
    LOGVERBOSE(__func__) << " leave  ";
}

/// @brief
void TcpServer::CloseStream() noexcept
{
    LOGVERBOSE(__func__) << " enter ";
    for (const std::shared_ptr< StreamInfo > &stinf : streams_) {
        LOGVERBOSE(__func__) << " socketfd: " << stinf->socketfd;
        if (stinf->socketfd < 0) {
            continue;
        }
        // Remove from clientSet_
        std::ignore = clientSet_.erase(stinf->socketfd);

        std::int32_t const ret{nai_stream_close(&stinf->sockStream)};
        if (0 > ret) {
            LOGERROR("nai_stream_close failed : ") << __func__ << " socketfd: " << stinf->socketfd
                                                   << " errno: " << errno << " " << strerror(errno) << " ret: " << ret;
            continue;
        }
    }
    streams_.clear();

    // Explicitly close listening socket
    if (listenfd_ >= 0) {
        LOGVERBOSE(__func__) << " closing listenfd_: " << listenfd_;
        if (0 > nai_sock_close(listenfd_)) {
            LOGERROR("nai_sock_close listenfd_ failed: ")
                << __func__ << " fd: " << listenfd_ << " errno: " << errno << " " << strerror(errno);
        }
        listenfd_ = -1;
    }

    LOGVERBOSE(__func__) << " leave  ";
}

/// @brief
/// @param[in]  socketfd
/// @param[in]  handler
void TcpServer::AddStream(std::int32_t const &socketfd, TcpServer *const handler) noexcept
{
    LOGVERBOSE(__func__) << " enter ";
    std::shared_ptr< StreamInfo > si{std::make_shared< StreamInfo >()};
    si->data     = handler;
    si->socketfd = socketfd;

    if (0 != nai_stream_init(&si->sockStream)) {
        LOGERROR("nai_stream_init failed") << __func__;
        return;
    }
    if (0 > nai_stream_set_fd(&si->sockStream, si->socketfd, NAI_FD_TYPE_SOCK)) {
        LOGERROR("nai_stream_set_fd failed") << __func__;
        return;
    }
    // Set file descriptor ownership: stream owns the fd, close will automatically close the underlying socket
    if (0 > nai_stream_set_fdown(&si->sockStream, 1)) {
        LOGERROR("nai_stream_set_fdown failed") << __func__;
        return;
    }
    if (0 > nai_stream_set_cb(&si->sockStream, NAIMessageEventHandler)) {
        LOGERROR("nai_stream_set_cb failed") << __func__;
        return;
    }
    if (0 > nai_stream_open(&si->sockStream, mainLoop_->GetRawEvLoop())) {
        LOGERROR("nai_stream_open failed") << __func__;
        // Open failed, need to clean up the already initialized stream
        nai_stream_close(&si->sockStream);
        return;
    }

    streams_.push_back(si);
    LOGVERBOSE(__func__) << " leave  socketfd: " << socketfd;
}
/// @brief
/// @param[in]  socketid
void TcpServer::DeleteStream(std::int32_t const &socketid) noexcept
{
    LOGVERBOSE(__func__) << " enter socketid: " << socketid;
    streams_.remove_if([&](std::shared_ptr< StreamInfo > const &value) {
        if (socketid == value->socketfd) {
            LOGVERBOSE(__func__) << " close stream fd: " << value->socketfd;
            if (0 > nai_stream_close(&(value->sockStream))) {
                LOGERROR("nai_stream_close failed") << __func__;
            } else {
                LOGVERBOSE(__func__) << " nai_stream_close success fd: " << value->socketfd;
            }
        }
        return socketid == value->socketfd;
    });
    LOGVERBOSE(__func__) << " leave socketid: " << socketid;
}
/// @brief
/// @param[in]  stream
/// @param[in]  events
/// @return
std::int32_t TcpServer::NAIMessageEventHandler(nai_stream_t *const stream, std::int32_t const events) noexcept
{
    LOGVERBOSE(__func__) << " enter events: " << events;

    // Add safety comment to explain the necessity of the cast
    // Safe conversion: known to be the address of sockStream_ member in StreamInfo structure
    StreamInfo *const net{reinterpret_cast< StreamInfo * >(nai_containof(stream, StreamInfo, sockStream))};  // NOLINT
    if ((events & NAI_EV_ERROR) > 0) {
        if ((net != nullptr) && (net->data != nullptr)) {
            LOGVERBOSE(__func__) << "error : " << net->socketfd;
            LOGERROR(__func__) << " NAI_EV_ERROR close   fd : " << net->socketfd;
            net->data->DeleteStream(net->socketfd);
            net->data->_OnSocketClose(net->socketfd);
        }
        return nai_errno;
    }
    // Read event
    if ((events & NAI_EV_READ) > 0) {
        LOGVERBOSE(__func__) << " reading fd: " << ((net != nullptr) ? net->socketfd : -1);
        if ((net != nullptr) && (net->data != nullptr)) {
            std::ignore = LOGVERBOSE(__func__);
            net->data->_OnSocketRead(net->socketfd);
        } else {
            LOGERROR("get NAIMessageEventHandler but StreamInfo*  net is nullptr ") << __func__;
        }
    }
    LOGVERBOSE(__func__) << " leave  ";

    return 0;
}
}  // namespace internal
}  // namespace log
}  // namespace ara
