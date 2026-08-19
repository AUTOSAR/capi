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
/// @file       server_impl.h
/// @brief      This file provides DOIP server implementation related
/// @details
/// @date       2023-10-08
/// @author     zhanglipeng
/// @version    1.2.0
///
/// ================================================================

#ifndef ISOFT_DOIP_SERVER_IMPL_H_
#define ISOFT_DOIP_SERVER_IMPL_H_
#include <ara/core/result.h>
#include <ara/core/vector.h>
#include <isoft/naicpp/evloop.h>
#include <isoft/naicpp/global_evloop.h>
#include <nai/os/nai_socket.h>

#include <atomic>
#include <set>

#include "isoft/doip/config.h"
#include "isoft/doip/public_def.h"
#include "isoft/doip/server.h"

namespace isoft {
namespace doip {

/// @brief Forward declaration of ServerUserBase
class ServerUserBase;

/// @brief Forward declaration of ServerListen
class ServerListen;

/// @brief Forward declaration of ServerUserTcp
class ServerUserTcp;

/// @brief Forward declaration of PublicUdpDgram
class PublicUdpDgram;

/// @brief Forward declaration of ServerUserUdp
class ServerUserUdp;

/// @brief Network interface monitoring
class ServerNetlinkMonitor;

/// @brief DOIP server implementation
class ServerImpl
{
public:
    /// @brief Constructor
    /// @param[in] config Configuration
    /// @param[in] requestHandler DOIP message callback
    ServerImpl(ServerConfig config, ClientRequestCb requestHandler);

    /// @brief Destructor
    ~ServerImpl() noexcept;

    /// @brief Copy constructor
    /// @param[in] other
    ServerImpl(ServerImpl const& other) = delete;

    /// @brief Assignment operator
    /// @param[in] other
    /// @return ServerImpl reference
    ServerImpl& operator=(ServerImpl const& other) = delete;

    /// @brief Move constructor
    /// @param[in] other
    ServerImpl(ServerImpl&& other) = delete;

    /// @brief Move assignment operator
    /// @param[in] other
    /// @return ServerImpl reference
    ServerImpl& operator=(ServerImpl&& other) = delete;

public:
    /// @brief Activate DOIP network
    /// @param[in] cb Callback for activating a diagnostic connection
    /// @return Activation result
    ara::core::Result< void > Start(DiagConnectionActivateCb const& cb);

    /// @brief Deactivate network service, stop providing service externally
    /// @return Network stop result
    ara::core::Result< void > Stop();

    /// @brief Reply DOIP message
    /// @param[in] fd sock fd
    /// @param[in] protocloType Protocol type
    /// @param[in] vbody Data packet
    /// @param[in] isSend Whether to send this packet to peer
    /// @return Operation result
    bool SendDoIPMessage(int32_t const fd,
                         PayloadType const protocloType,
                         ara::core::Vector< std::uint8_t > const& vbody,
                         bool const isSend);

    /// @brief Get MAC
    /// @return mac
    MacAddress GetMacAddress();

    /// @brief Start sending vehicle announcements
    /// @param[in] body Vehicle announcement message packet
    void BroadcastAnnouncement(ara::core::Vector< uint8_t > const& body);

    /// @brief Start network interface status monitoring (up, down, IP change)
    /// @return Operation result
    static ara::core::Result< void > StartNetlinkMonitor(NetlinkMonitorCb const& cb);

    /// @brief Stop network interface status monitoring
    /// @return Operation result
    static ara::core::Result< void > StopNetlinkMonitor();

public:
    /// @brief Unified handling of received DOIP messages from peer
    /// @param[in] userBase Socket user
    /// @param[in] type Protocol type
    /// @param[in] vbody Message body
    void DoipMessageProcessCenter(ServerUserBase* userBase,
                                  PayloadType const type,
                                  ara::core::Vector< uint8_t > const& vbody);

    /// @brief Handle diagnostic connection
    /// @param[in] userTcp tcp user
    /// @param[in] state Diagnostic connection state
    void ProcessDiagConnection(ServerUserTcp* userTcp, bool const state);

    /// @brief Get DOIP server configuration
    /// @return Configuration
    ServerConfig const& GetConfig() { return config_; }

    /// @brief Get IO event loop
    /// @return evloop
    std::shared_ptr< naicpp::EvLoop > GetEvLoop() { return evloop_; }

    /// @brief Get all IP4 addresses of this machine
    std::set< ara::core::String > const& GetAllIpv4() { return setAllIpv4_; }

    /// @brief Generate local IP4 address list
    /// @return Whether generation succeeded
    int32_t GenAllIpv4();

    /// @brief Clear UDP instance
    /// @param fd Instance identifier
    void DelayRemoveUdpUser(int32_t const fd);

private:
    /// @brief Activate TCP
    /// @return Activation code
    int32_t _activateTcp();

    /// @brief Activate UDP
    /// @return Activation code
    int32_t _activateUdp();

    /// @brief Allocate UDP identifier
    /// @return Identifier
    int32_t _allocateUdpFd();

    /// @brief Create vehicle announcement sending handler
    void _createVehicleAnnouncementHandler();

    /// @brief Start sending vehicle announcements
    void _broadcastAnnouncement();

    /// @brief Handle route activation
    /// @param[in] userBase tcp user
    /// @param[in] vbody Route activation data packet
    void _processRoutingActivation(ServerUserBase* userBase, ara::core::Vector< uint8_t > const& vbody);

    /// @brief Handle logic for obtaining DOIP entity information
    /// @param[in] userBase udp user
    void _processDoipEntityStatus(ServerUserBase* userBase);

    /// @brief Handle alive check logic
    /// @param[in] userBase tcp user
    /// @param[in] vbody Alive check data packet
    void _processAliveCheckResponse(ServerUserBase* userBase, ara::core::Vector< uint8_t > const& vbody);

    /// @brief Handle diagnostic message
    /// @param[in] userBase tcp user
    /// @param[in] type Protocol type
    /// @param[in] vbody Diagnostic data packet
    void _processDiagMessage(ServerUserBase* userBase,
                             PayloadType const type,
                             ara::core::Vector< uint8_t > const& vbody);

    /// @brief Reply vehicle announcement information
    /// @param[in] fd sock fd
    /// @param[in] protocloType Protocol number
    /// @param[in] vbody Message body
    /// @param[in] isSend Whether to send this message
    /// @return Send result
    bool _responseVehicleAnnoucementId(int32_t const fd,
                                       PayloadType const protocloType,
                                       ara::core::Vector< std::uint8_t > const& vbody,
                                       bool const isSend);

    /// @brief Reply UDP type message
    /// @param[in] fd sock fd
    /// @param[in] protocloType Protocol number
    /// @param[in] vbody Message body
    /// @return Reply result
    bool _responseUdp(int32_t const fd, PayloadType const protocloType, ara::core::Vector< std::uint8_t > const& vbody);

    /// @brief Reply TCP type message
    /// @param[in] fd sock fd
    /// @param[in] protocloType Protocol number
    /// @param[in] vbody Message body
    /// @return Reply result
    bool _responseTcp(int32_t const fd, PayloadType const protocloType, ara::core::Vector< std::uint8_t > const& vbody);

    /// @brief Get broadcast address of the server's local network
    /// @param[out] buf Broadcast address
    void _getBroadcast(nai_socknbuf_t& buf) const;

    /// @brief 处理收到单播或广播消息
    /// @param dgram udp句柄
    void _handleUdpData(PublicUdpDgram* const dgram);

private:
    /// @brief Event loop
    std::shared_ptr< naicpp::EvLoop > evloop_;

    /// @brief Client request callback, notifying upper layer for processing (mainly diagnostic processing)
    ClientRequestCb requestHandler_;

    /// @brief Callback for activating a diagnostic connection
    DiagConnectionActivateCb diagConncetionHandler_;

    /// @brief Configuration
    ServerConfig config_;

    /// @brief TCP binding listening service
    std::unique_ptr< ServerListen > tcp_;

    /// @brief UDP receiving data
    std::shared_ptr< PublicUdpDgram > udp_;

    /// @brief UDP sending data
    std::shared_ptr< PublicUdpDgram > sendUdp_;

    /// @name  timer_ptr_
    /// isoft::naicpp::EvNodeTimer object
    std::shared_ptr< isoft::naicpp::EvNodeTimer > vehicleIdTimer_;

    /// @brief Vehicle announcement processing object
    std::unique_ptr< ServerUserUdp > vehicleAnnouncementUdpUser_;

    /// @brief Number of announcement sending attempts
    uint16_t announcementTimes_;

    /// @brief Vehicle announcement data
    ara::core::Vector< uint8_t > vehicleAnnouncementData_;

    /// @brief Whether vehicle announcement can be sent
    std::atomic_bool isCanSendVehicleAnnouncement_;

    /// @brief Whether network service is activated
    bool isActivate_;

    /// @brief All IP4 addresses of this machine, used to filter broadcast and not receive messages from itself
    std::set< ara::core::String > setAllIpv4_;

    /// @brief Global unique network interface monitoring instance
    static std::unique_ptr< ServerNetlinkMonitor > s_GNetlinkMonitor_;
};

}  // namespace doip
}  // namespace isoft

#endif  // ISOFT_DOIP_SERVER_IMPL_H_