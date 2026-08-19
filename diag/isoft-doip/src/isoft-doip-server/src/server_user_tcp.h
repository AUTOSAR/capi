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
/// @file       server_user_tcp.h
/// @brief      This file provides DOIP server TCP related definitions
/// @details
/// @date       2023-10-08
/// @author     zhanglipeng
/// @version    1.2.0
///
/// ================================================================

#ifndef ISOFT_DOIP_SERVER_USER_TCP_H_
#define ISOFT_DOIP_SERVER_USER_TCP_H_
#include <ara/core/map.h>
#include <ara/core/string.h>
#include <isoft/naicpp/evnode_timer.h>

#include <mutex>

#include "isoft/doip/public_def.h"
#include "public_tcp_stream.h"
#include "server_user_base.h"

#ifdef NAI_ENABLE_SSL
    #include <openssl/err.h>
    #include <openssl/ssl.h>
#endif

namespace isoft {
namespace doip {

/// @brief Forward declaration of ServerImpl
class ServerImpl;
/// @brief Forward declaration of PublicTcpStream
class PublicTcpStream;

class ServerListen;

/// @brief DOIP TCP user implementation
class ServerUserTcp : public ServerUserBase
{
public:
    /// @brief Constructor
    /// @param[in] stream tcp stream
    /// @param[in] server DOIP service object
    /// @param[in] bufferSize Read buffer size
    ServerUserTcp(std::unique_ptr< PublicTcpStream > stream,
                  ServerImpl* server,
                  size_t bufferSize,
                  ServerListen* listen);

    /// @brief Destructor
    ~ServerUserTcp() noexcept override;

    /// @brief Copy constructor
    /// @param[in] other
    ServerUserTcp(ServerUserTcp const& other) = delete;

    /// @brief Assignment operator
    /// @param[in] other
    /// @return ServerUserTcp&
    ServerUserTcp& operator=(ServerUserTcp const& other) = delete;

    /// @brief Move constructor
    /// @param[in] other
    ServerUserTcp(ServerUserTcp&& other) = delete;

    /// @brief Move assignment operator
    /// @param[in] other
    /// @return ServerUserTcp&
    ServerUserTcp& operator=(ServerUserTcp&& other) = delete;

public:
    /// @brief Initialize
    /// @return Initialization result
    int32_t Initialize();

    /// @brief Get identifier
    /// @return Identifier
    int32_t GetFd() override
    {
        if (stream_.get() == nullptr) {
            return -1;
        }

        return stream_->GetFd();
    }

    /// @brief Set source address
    /// @param[in] sa Source address
    void SetSa(uint16_t const sa) { sa_ = sa; }

    /// @brief Get source address
    /// @return Source address
    uint16_t GetSa() const { return sa_; }

    /// @brief Get connection state
    /// @return State
    DoipConnectionState GetState() const { return connectionState_; }

    /// @brief Set alive check count
    /// @param[in] num Count
    void SetAliveCheckNum(uint16_t const num) { aliveCheckNum_ = num; }

    /// @brief Get alive check count
    /// @return Count
    uint16_t GetAliveCheckNum() const { return aliveCheckNum_; }

    /// @brief Get 4-tuple information
    /// @param[out] localIp Local IP
    /// @param[out] localPort Local port
    /// @param[out] peerIp Peer IP
    /// @param[out] peerPort Peer port
    void GetSocketAddressInfo(ara::core::String& localIp,
                              uint16_t& localPort,
                              ara::core::String& peerIp,
                              uint16_t& peerPort);

    /// @brief Request alive check
    /// @param[in] cb Check result callback
    void DoAliveCheckRequest(std::function< void(bool const) > const& cb, int32_t const targetFd);

    /// @brief Reply alive check
    /// @param[in] sa Source logical address
    void DoAliveCheckResponse(uint16_t const sa);

    /// @brief Update general inactivity timer
    void UpdateGeneralInactivityTimer();

    /// @brief Change socket state
    /// @param[in] state State
    void ChangeConnectionState(DoipConnectionState const state) override;

    /// @brief Send message to peer
    /// @param[in] type Protocol number
    /// @param[in] body Message body
    /// @return Send result
    int32_t SendToClient(PayloadType const type, ara::core::Vector< uint8_t > const& body) override;

private:
    /// @brief Handle read/write errors and other events
    /// @return Result
    int32_t _handleEvent(int32_t const events);

    /// @brief Release corresponding heartbeat timer
    /// @param isAlive Whether the target connection is still online
    /// @param targetFd Target FD
    void _releaseAliveCheckTimer(bool const isAlive, int32_t const targetFd = 0);

private:
    /// @brief sa_
    /// Sending end logical address
    uint16_t sa_;

    /// @brief tcp socket
    std::unique_ptr< PublicTcpStream > stream_;

    /// @brief  connectionState_
    /// Connection state
    DoipConnectionState connectionState_;

    /// @brief  initialInactivityTimer_
    /// Initial timer, close socket if route activation not received within 2 seconds
    std::shared_ptr< naicpp::EvNodeTimer > initialInactivityTimer_;

    /// @brief  generalInactivityTimer_
    /// General timer, close connection if no data sent or received within a certain range
    std::shared_ptr< naicpp::EvNodeTimer > generalInactivityTimer_;

    class AliveCheckInfo
    {
    public:
        std::shared_ptr< naicpp::EvNodeTimer > aliveCheckTimer{};
        std::function< void(bool const) > aliveCheckCb{};
    };

    /// @brief Alive check callback notification
    ara::core::Map< int32_t, AliveCheckInfo > mapAliveCheckInfo_;

    /// @brief Local IP
    ara::core::String localIp_;

    /// @brief Local port
    uint16_t localPort_;

    /// @brief Peer IP
    ara::core::String peerIp_;

    /// @brief Peer port
    uint16_t peerPort_;

    /// @brief Number of alive checks performed
    uint16_t aliveCheckNum_;

    /// @brief Thread safety issue
    std::recursive_mutex streamLock_;

    /// @brief Whether TLS handshake is complete
    bool isHandshakeFinished_;

    ServerListen* listen_;

#ifdef NAI_ENABLE_SSL
    SSL* ssl_{nullptr};
#endif
};

}  // namespace doip
}  // namespace isoft

#endif  // ISOFT_DOIP_SERVER_USER_TCP_H_