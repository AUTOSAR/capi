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
/// @file       server_impl.cpp
/// @brief      This file provides DOIP server implementation related
/// @details
/// @date       2023-10-08
/// @author     zhanglipeng
/// @version    1.2.0
///
/// ================================================================

#include "server_impl.h"

#include <arpa/inet.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <sys/types.h>

#include <bitset>
#include <random>

#include "isoft/doip/doip_error_domain.h"
#include "isoft/doip/public_message.h"
#include "nai/runtime/nai_errno.h"
#include "public_log.h"
#include "public_tcp_stream.h"
#include "public_udp_dgram.h"
#include "server_listen.h"
#include "server_netlink_monitor.h"
#include "server_user_manager.h"
#include "server_user_tcp.h"
#include "server_user_udp.h"
#include "thread/therad_safe.h"

namespace isoft {
namespace doip {

/// @brief Global unique network interface monitoring instance
std::unique_ptr< ServerNetlinkMonitor > ServerImpl::s_GNetlinkMonitor_{nullptr};  // NOLINT

/// @brief Constructor
/// @param[in] config Configuration
/// @param[in] requestHandler DOIP message callback
ServerImpl::ServerImpl(ServerConfig config, ClientRequestCb requestHandler)
    : evloop_{isoft::naicpp::GlobalGeneralEvLoop::Get()}
    , requestHandler_{std::move(requestHandler)}
    , diagConncetionHandler_{}
    , config_{std::move(config)}
    , tcp_{std::make_unique< ServerListen >(
          evloop_, config_.isUseTls, config_.serverCrt, config_.serverKey, config_.ca)}
    , udp_{std::make_shared< PublicUdpDgram >()}
    , sendUdp_{std::make_shared< PublicUdpDgram >()}
    , vehicleIdTimer_{}
    , vehicleAnnouncementUdpUser_{}
    , announcementTimes_{0U}
    , vehicleAnnouncementData_{}
    , isCanSendVehicleAnnouncement_{false}
    , isActivate_{false}
    , setAllIpv4_{}
{
}

/// @brief Destructor
ServerImpl::~ServerImpl() noexcept
{
    evloop_.reset();
    if (tcp_.get() != nullptr) {
        tcp_.reset();
    }
    if (udp_.get() != nullptr) {
        udp_.reset();
    }
    if (sendUdp_.get() != nullptr) {
        sendUdp_.reset();
    }
    if (vehicleIdTimer_.get() != nullptr) {
        vehicleIdTimer_.reset();
    }

    if (vehicleAnnouncementUdpUser_.get() != nullptr) {
        vehicleAnnouncementUdpUser_.reset();
    }
}

/// @brief Activate DOIP network
/// @param[in] cb Callback for activating a diagnostic connection
/// @return Activation result
ara::core::Result< void > ServerImpl::Start(DiagConnectionActivateCb const& cb)
{
    if (isActivate_) {
        LogError() << "ServerImpl::Start|server has activate";
        return ara::core::Result< void >::FromError(DoipErrorCode::kDoipActivateErr);
    }

    /// Activate TCP
    if (_activateTcp() == -1) {
        LogError() << "ServerImpl::Start|activate tcp err";
        return ara::core::Result< void >::FromError(DoipErrorCode::kDoipActivateTcpErr);
    }

    /// Activate UDP
    if (_activateUdp() == -1) {
        LogError() << "ServerImpl::Start|activate udp err";
        return ara::core::Result< void >::FromError(DoipErrorCode::kDoipActivateUdpErr);
    }

    // if (GenAllIpv4() != 0) {
    //     LogWarn() << "ServerImpl::Start|GenAllIpv4 not success";
    // }

    isActivate_            = true;
    diagConncetionHandler_ = cb;
    _createVehicleAnnouncementHandler();
    LogInfo() << "ServerImpl::Start|tcp&udp success";
    return ara::core::Result< void >::FromValue();
}

/// @brief Stop DOIP network
/// @return Operation result
ara::core::Result< void > ServerImpl::Stop()
{
    if (!isActivate_) {
        LogWarn() << "ServerImpl::Stop|server no activate";
        return ara::core::Result< void >::FromError(DoipErrorCode::kDoipActivateErr);
    }

    std::ignore = evloop_->Exec([this]() {
        if (tcp_.get() == nullptr) {
            LogError() << "ServerImpl::Stop|server tcp_ is nullptr";
            return;
        }

        if (udp_.get() == nullptr) {
            LogError() << "ServerImpl::Stop|server udp_ is nullptr";
            return;
        }

        if (sendUdp_.get() == nullptr) {
            LogError() << "ServerImpl::Stop|server sendUdp_ is nullptr";
            return;
        }

        ServerUserManager::GetInstance().Release();

        /// Deactivate UDP
        if (udp_->Stop() == -1) {
            LogWarn() << "ServerImpl::Stop|server udp  stop errno =" << isoft::threadsafe::Safe_Strerror(errno);
        }

        /// Deactivate sending UDP
        if (sendUdp_->Stop() == -1) {
            LogWarn() << "ServerImpl::Stop|server serndUdp stop errno =" << isoft::threadsafe::Safe_Strerror(errno);
        }

        /// Deactivate TCP
        if (tcp_->Stop() == -1) {
            LogWarn() << "ServerImpl::Stop|server tcp stop errno =" << isoft::threadsafe::Safe_Strerror(errno);
        }
    });

    isActivate_ = false;
    return ara::core::Result< void >::FromValue();
}

/// @brief Reply DOIP message
/// @param[in] fd sock fd
/// @param[in] protocloType Protocol type
/// @param[in] vbody Data packet
/// @param[in] isSend Whether to send this packet to peer
/// @return Operation result
bool ServerImpl::SendDoIPMessage(int32_t const fd,
                                 PayloadType const protocloType,
                                 ara::core::Vector< std::uint8_t > const& vbody,
                                 bool const isSend)
{
    bool result{false};
    switch (protocloType) {
        case PayloadType::kVehicleAnnoucementIdResponse: {
            result = _responseVehicleAnnoucementId(fd, protocloType, vbody, isSend);
        } break;
        case PayloadType::kDiagPowerModeInfoResponse: {
            result = _responseUdp(fd, protocloType, vbody);
        } break;
        case PayloadType::kDiagMessage:
        case PayloadType::kDiagMessagePositiveAcknowledgement:
        case PayloadType::kDiagMessageNegativeAcknowledgement: {
            result = _responseTcp(fd, protocloType, vbody);
        } break;
        default:
            break;
    }

    return result;
}

/// @brief Get MAC
/// @return mac
MacAddress ServerImpl::GetMacAddress()
{
    MacAddress macAddress{};

    nai_socknbuf_t localAddress;
    localAddress.len = static_cast< int32_t >(sizeof(localAddress.storage));
    std::int32_t const sock_result{nai_sockaddr_pton(config_.serverIP_.data(), config_.serverIP_.length(),  // NOLINT
                                                     &localAddress.addr, &localAddress.len)};               // NOLINT
    if (sock_result != -1) {
        macAddress = sendUdp_->GetMacAddress(&localAddress.addr);  // NOLINT
    } else {
        LogError() << "ServerImpl::GetMacAddress|nai_sockaddr_pton fails, error:"
                   << isoft::threadsafe::Safe_Strerror(errno);
    }

    return macAddress;
}

/// @brief Unified handling of received DOIP messages from peer
/// @param[in] userBase Socket user
/// @param[in] type Protocol type
/// @param[in] vbody Message body
void ServerImpl::DoipMessageProcessCenter(ServerUserBase* userBase,
                                          PayloadType const type,
                                          ara::core::Vector< uint8_t > const& vbody)
{
    if (userBase == nullptr) {
        LogError() << "ServerImpl::DoipMessageProcessCenter|userBase is nullptr";
        return;
    }

    switch (type) {
        case PayloadType::kVehicleIdRequest:            /// Pass to upper layer for processing page 29
        case PayloadType::kVehicleIdRequestWithEid:     /// Pass to upper layer for processing
        case PayloadType::kVehicleIdRequestWithVin:     /// Pass to upper layer for processing
        case PayloadType::kDiagPowerModeInfoRequest: {  /// Pass to upper layer for processing
            int32_t fd = userBase->GetFd();
            requestHandler_(fd, type, vbody);
        } break;
        case PayloadType::kRoutingActivationRequest: {  /// page 74
            _processRoutingActivation(userBase, vbody);
        } break;
        case PayloadType::kAliveCheckResponse: {
            _processAliveCheckResponse(userBase, vbody);
        } break;
        case PayloadType::kDoipEntityStatusRequest: {
            _processDoipEntityStatus(userBase);
        } break;
        case PayloadType::kDiagMessage: {
            _processDiagMessage(userBase, type, vbody);
        } break;
        default:
            break;
    }
}

/// @brief Handle diagnostic connection
/// @param[in] userTcp tcp user
/// @param[in] state Diagnostic connection state
void ServerImpl::ProcessDiagConnection(ServerUserTcp* userTcp, bool const state)
{
    if (userTcp == nullptr) {
        LogError() << "ServerImpl::ProcessDiagConnection|userTcp is nullptr";
        return;
    }
    ara::core::String localIp;
    uint16_t localPort{};
    ara::core::String peerIp;
    uint16_t peerPort{};
    userTcp->GetSocketAddressInfo(localIp, localPort, peerIp, peerPort);

    diagConncetionHandler_(userTcp->GetFd(), userTcp->GetSa(), localIp, localPort, peerIp, peerPort, state);
}

/// @brief Activate TCP
/// @return Activation result
int32_t ServerImpl::_activateTcp()
{
    if (tcp_.get() == nullptr) {
        LogError() << "ServerImpl::ActivateTcp|tcp_ is nullptr";
        return -1;
    }

    /// New connection callback notification
    NewConnectionCallback newConnectonCb
        = [this](std::unique_ptr< PublicTcpStream > stream, nai_socknbuf_t const& naiSocknbuf, ServerListen* listen) {
              std::unique_ptr< ServerUserTcp > tcpUser
                  = std::make_unique< ServerUserTcp >(std::move(stream), this, config_.requestMaxBytes, listen);
              if (tcpUser.get() == nullptr) {
                  LogError() << "ServerImpl::Start|create tcp user err tcpUser is nullptr";
                  return;
              }

              /// Initialize new TCP user
              int32_t initRes = tcpUser->Initialize();
              if (initRes != 0) {
                  LogError() << "ServerImpl::Start|init tcp user err";
                  return;
              }

              tcpUser->SetSockBuf(naiSocknbuf);

              /// Add to user manager for unified management
              ServerUserManager::GetInstance().AddTcpUser(std::move(tcpUser));
          };

    /// Bind port conversion
    ara::core::String strTcpListenPort{std::to_string(static_cast< int32_t >(config_.tcpPort_)).c_str()};  // NOLINT
    if (config_.isUseTls) {
        strTcpListenPort = std::to_string(static_cast< int32_t >(config_.tlsPort_)).c_str();  // NOLINT
    }
    return tcp_->Start(config_.serverIP_, strTcpListenPort, newConnectonCb);  // NOLINT
}

/// @brief Activate UDP
/// @return Activation result
int32_t ServerImpl::_activateUdp()
{
    if (udp_.get() == nullptr) {
        LogError() << "ServerImpl::_activateUdp|udp_ is nullptr";
        return -1;
    }

    if (sendUdp_.get() == nullptr) {
        LogError() << "ServerImpl::_activateUdp|sendUdp_ is nullptr";
        return -1;
    }

    /// New message callback notification, one UDP datagram per DOIP packet, process one at a time
    PublicUdpDgram::UdpNewMsgCallback newMsgCb = [this](PublicUdpDgram* const dgram, int32_t const events) -> int32_t {
        if (dgram != udp_.get()) {
            LogError() << "ServerImpl::_activateUdp|udp recv data, but is not this dgram";
            return -1;
        }

        if ((events & NAI_EV_ERROR) == NAI_EV_ERROR) {
            LogError() << "ServerImpl::_activateUdp|recv data NAI_EV_ERROR";
            return -1;
        }

        LogDebug() << "ServerImpl::_activateUdp|recv data events =" << events;

        /// Has readable data
        if ((events & NAI_EV_READ) != 0) {
            // uint32_t udpFd = _allocateUdpFd();
            // std::unique_ptr< ServerUserUdp > udpUser
            //     = std::make_unique< ServerUserUdp >(udpFd, sendUdp_, this, config_.requestMaxBytes);
            // if (udpUser.get() == nullptr) {
            //     LogError() << "ServerImpl::_activateUdp|create usp user err ucpUser is nullptr";
            //     return -2;
            // }

            // /// Add to user manager for unified management and process data once
            // ServerUserManager::GetInstance().AddUdpUser(std::move(udpUser));

            // int32_t ec;
            // while (true) {
            //     std::vector< uint8_t > buffer(config_.requestMaxBytes);
            //     nai_socknbuf_t nai_socknbuf_t_;
            //     nai_socknbuf_t_.len = static_cast< int32_t >(sizeof(nai_socknbuf_t_.storage));
            //     int32_t recvRes
            //         = dgram->Recv(buffer.data(), config_.requestMaxBytes, &nai_socknbuf_t_.addr, &nai_socknbuf_t_.len);
            //     if (recvRes <= 0) {
            //         ec = nai_errno;
            //         if (ec == NAI_EAGAIN || ec == NAI_EINPROGRESS) {
            //             LogDebug() << "ServerImpl::ActivateUdp|please try later";
            //             break;
            //         }
            //         LogError() << "ServerImpl::ActivateUdp|detect error, no = " << strerror(errno);
            //         break;
            //     }
            //     uint32_t udpFd = _allocateUdpFd();
            //     std::unique_ptr< ServerUserUdp > udpUser
            //         = std::make_unique< ServerUserUdp >(udpFd, sendUdp_, this, config_.requestMaxBytes);
            //     if (udpUser.get() == nullptr) {
            //         LogError() << "ServerImpl::ActivateUdp|create usp user err ucpUser is nullptr";
            //         return -2;
            //     }
            //     udpUser->SetSockBuf(nai_socknbuf_t_);
            //     /// 添加到user管理器统一管理并处理一次数据
            //     ServerUserManager::GetInstance().AddUdpUser(std::move(udpUser), buffer.data(), recvRes);
            // }
            _handleUdpData(dgram);
        }

        // static bool isSetTag{false};  /// Whether the flag for sending vehicle announcements has been set
        // if ((events & NAI_EV_WRITE) != 0 && !isSetTag) {
        //     LogDebug() << "ServerImpl::_activateUdp| send announcement size =" << vehicleAnnouncementData_.size();
        //     isSetTag                      = true;
        //     isCanSendVehicleAnnouncement_ = true;
        //     if (!vehicleAnnouncementData_.empty()) {
        //         BroadcastAnnouncement();
        //     } else {
        //         LogDebug() << "ServerImpl::_activateUdp|send announcement, vehicleAnnouncementData_ is empty";
        //     }
        // }

        return 0;
    };

    ara::core::String const strUdpListenPort{
        std::to_string(static_cast< int32_t >(config_.udpPort_)).c_str()};  // NOLINT
    int32_t res = udp_->Start(evloop_, "0.0.0.0", strUdpListenPort, newMsgCb);
    if (res == -1) {
        LogError() << "ServerImpl::_activateUdp|start udp fail";
        return res;
    }

    res = sendUdp_->Start(evloop_, config_.serverIP_, strUdpListenPort,
                          [this](PublicUdpDgram* const dgram, int32_t const events) -> int32_t {
                              if (dgram != sendUdp_.get()) {
                                  LogError() << "ServerImpl::_activateUdp|sendUdp_ recv data, but is not this dgram";
                                  return -1;
                              }

                              if ((events & NAI_EV_ERROR) == NAI_EV_ERROR) {
                                  LogError() << "ServerImpl::_activateUdp|sendUdp_ recv data NAI_EV_ERROR";
                                  return -1;
                              }

                              LogDebug() << "ServerImpl::_activateUdp|sendUdp recv data events =" << events;

                              /// 有可读数据，
                              if ((events & NAI_EV_READ) != 0) {
                                  _handleUdpData(dgram);
                              }
                              return 0;
                          });
    if (res == -1) {
        LogError() << "ServerImpl::_activateUdp|start sendUdp fail";
        return res;
    }

    return res;
}

/// @brief Start sending vehicle announcements
/// @param[in] body Vehicle announcement message packet
void ServerImpl::BroadcastAnnouncement(ara::core::Vector< uint8_t > const& body)
{
    if (body.empty()) {
        LogError() << "ServerImpl::BroadcastAnnouncement|announcement size is empty";
        return;
    }

    vehicleAnnouncementData_ = body;
    // if (!isCanSendVehicleAnnouncement_.load()) {
    //     LogInfo() << "ServerImpl::BroadcastAnnouncement|udp cannot be written";
    //     return;
    // }

    _broadcastAnnouncement();
    LogInfo() << "ServerImpl::_broadcastAnnouncement|success";
}

/// @brief Start network interface status monitoring (up, down, IP change)
/// @return Operation result
ara::core::Result< void > ServerImpl::StartNetlinkMonitor(NetlinkMonitorCb const& cb)
{
    if (s_GNetlinkMonitor_.get() != nullptr) {
        LogError() << "ServerImpl::StartNetlinkMonitor|s_GNetlinkMonitor_ has been monitored";
        return ara::core::Result< void >::FromError(DoipErrorCode::kDoipInternalErr);
    }

    s_GNetlinkMonitor_ = std::make_unique< ServerNetlinkMonitor >();
    if (s_GNetlinkMonitor_.get() == nullptr) {
        LogError() << "ServerImpl::StartNetlinkMonitor|create s_GNetlinkMonitor_ fail";
        return ara::core::Result< void >::FromError(DoipErrorCode::kDoipInternalErr);
    }
    int32_t const res = s_GNetlinkMonitor_->Start(cb);
    if (res != 0) {
        LogError() << "ServerImpl::StartNetlinkMonitor|Start fail res =" << res;
        return ara::core::Result< void >::FromError(DoipErrorCode::kDoipActivateErr);
    }
    return ara::core::Result< void >::FromValue();
}

/// @brief Stop network interface status monitoring
/// @return Operation result
ara::core::Result< void > ServerImpl::StopNetlinkMonitor()
{
    if (s_GNetlinkMonitor_.get() == nullptr) {
        LogError() << "ServerImpl::StartNetlinkMonitor|s_GNetlinkMonitor_ is nullptr";
        return ara::core::Result< void >::FromError(DoipErrorCode::kDoipInternalErr);
    }

    int32_t const res = s_GNetlinkMonitor_->Stop();
    if (res != 0) {
        LogError() << "ServerImpl::StartNetlinkMonitor|Stop fail res =" << res;
        return ara::core::Result< void >::FromError(DoipErrorCode::kDoipActivateErr);
    }
    return ara::core::Result< void >::FromValue();
}

/// @brief Allocate UDP user ID
/// @return id
int32_t ServerImpl::_allocateUdpFd()
{
    /// Start from 2, 1 is used for broadcast
    static int32_t s_BeginUdpFd{2};
    if (s_BeginUdpFd >= INT32_MAX) {
        s_BeginUdpFd = 2;
    }
    return s_BeginUdpFd++;
}

void ServerImpl::_createVehicleAnnouncementHandler()
{
    vehicleAnnouncementUdpUser_ = std::make_unique< ServerUserUdp >(1, sendUdp_, this, config_.requestMaxBytes);
    if (vehicleAnnouncementUdpUser_.get() == nullptr) {
        LogError() << "ServerImpl::_createVehicleAnnouncementHandler|vehicleAnnouncementUdpUser_ is nullptr";
        return;
    }
    nai_socknbuf_t addr;
    _getBroadcast(addr);
    vehicleAnnouncementUdpUser_->SetSockBuf(addr);
}

/// @brief Start sending vehicle announcements
void ServerImpl::_broadcastAnnouncement()
{
    if (vehicleAnnouncementUdpUser_.get() == nullptr) {
        LogError() << "ServerImpl::_broadcastAnnouncement|vehicleAnnouncementUdpUser_ is nullptr";
        return;
    }

    /// Whether announcement sending has started
    static bool s_IsRuning{false};
    if (s_IsRuning) {
        LogDebug() << "ServerImpl::_broadcastAnnouncement|announcement is sending s_IsRuning =" << s_IsRuning;
        return;
    }

    s_IsRuning = true;

    std::function< void() > cb = [this]() {
        std::ignore = vehicleAnnouncementUdpUser_->SendToClient(PayloadType::kVehicleAnnoucementIdResponse,
                                                                vehicleAnnouncementData_);
        announcementTimes_++;
        if (announcementTimes_ == config_.vehicleAnnouncementCount) {
            vehicleIdTimer_.reset();
            vehicleAnnouncementData_.clear();
            s_IsRuning         = false;
            announcementTimes_ = 0U;
        } else {
            std::ignore = vehicleIdTimer_->UpdateTime(static_cast< int >(config_.vehicleAnnouncementInterval));
        }
    };

    /// According to iso-13400 table12 A_DoIP_Announce_Wait
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1U, static_cast< int >(config_.vehicleAnnouncementWait));
    uint32_t genResult = dis(gen);
    if (genResult == 0U) {
        genResult = 1U;
    }
    int32_t const makeTimerRes = evloop_->MakeTimer(vehicleIdTimer_, static_cast< int >(genResult), std::move(cb));
    LogDebug() << "ServerImpl::_broadcastAnnouncement| begin send announcement genResult =" << genResult
               << "makeTimerRes =" << makeTimerRes << "waitTime =" << config_.vehicleAnnouncementWait;
}

/// @brief Handle route activation
/// @param[in] userBase tcp user
/// @param[in] vbody Route activation data packet
void ServerImpl::_processRoutingActivation(ServerUserBase* userBase, ara::core::Vector< uint8_t > const& vbody)
{
    if (userBase == nullptr) {
        LogError() << "ServerImpl::_processRoutingActivation|userBase is nullptr";
        return;
    }
    RoutingActivationRequest req;
    if (!req.Deserialize(vbody)) {
        LogError() << "ServerImpl::_processRoutingActivation|Deserialize RoutingActivationRequest err";
        return;
    }

    uint16_t const sa = req.sourceAddress_;  // NOLINT
    std::function< void(RoutingActivationResponseType const) > response
        = [this, userBase, sa](RoutingActivationResponseType const codeType) {
              RoutingActivationResponse resp;
              resp.clientLogicalAddress_ = sa;                    // NOLINT
              resp.serverLogicalAddress_ = config_.logicAdress_;  // NOLINT
              resp.responseCode_         = codeType;              // NOLINT
              ara::core::Vector< uint8_t > body;
              resp.Serialize(body);
              std::ignore = userBase->SendToClient(PayloadType::kRoutingActivationResponse, body);
          };

    if (config_.testerMaxConnections == 0U) {
        response(RoutingActivationResponseType::kNoSocketAvailable);
        /// Close connection
        userBase->ChangeConnectionState(DoipConnectionState::kFinalize);
        return;
    }

    /// Check if source address is valid, logical address range is in table-13
    if (sa < config_.routingActivationSourceAdressCheckBegin || sa > config_.routingActivationSourceAdressCheckEnd) {
        LogError() << "ServerImpl::_processRoutingActivation|req.sourceAddress_ err sourceAddress_ = " << sa;  // NOLINT
        response(RoutingActivationResponseType::kInvalidSourceAddress);
        /// Close connection
        userBase->ChangeConnectionState(DoipConnectionState::kFinalize);
        return;
    }

    /// Check if activation type is supported
    if (req.activationType_ != RoutingActivationType::kRoutingActivationType_Default         // NOLINT
        && req.activationType_ != RoutingActivationType::kRoutingActivationType_Required) {  // NOLINT
        LogError() << "ServerImpl::_processRoutingActivation|req.activationType err activationType = "
                   << static_cast< uint16_t >(req.activationType_);  // NOLINT
        response(RoutingActivationResponseType::kUnsupportedActivationType);
        /// Close connection
        userBase->ChangeConnectionState(DoipConnectionState::kFinalize);
        return;
    }

    /// After the above checks pass, perform socket check
    int32_t fd{userBase->GetFd()};
    std::function< void(RoutingActivationResponseType) > socketHandlerCb
        = [this, fd, sa](RoutingActivationResponseType resCode) {
              ServerUserTcp* userTcp = ServerUserManager::GetInstance().GetTcpUser(fd);
              if (userTcp == nullptr) {
                  LogError() << "ServerImpl::_processRoutingActivation|socketHandlerCb userTcp id nullptr fd =" << fd;
                  return;
              }

              LogInfo() << "ServerImpl::_processRoutingActivation|socketHandlerCb result fd =" << fd << "sa =" << sa
                        << "code =" << ara::log::LogHex16{static_cast< uint8_t >(resCode)};

              RoutingActivationResponse resp;
              resp.clientLogicalAddress_ = sa;                    // NOLINT
              resp.serverLogicalAddress_ = config_.logicAdress_;  // NOLINT
              resp.responseCode_         = resCode;               // NOLINT
              ara::core::Vector< uint8_t > body;
              resp.Serialize(body);
              std::ignore = userTcp->SendToClient(PayloadType::kRoutingActivationResponse, body);

              if (resCode != RoutingActivationResponseType::kSuccessful) {
                  userTcp->ChangeConnectionState(DoipConnectionState::kFinalize);
                  return;
              }

              /// Whether authentication is required, not considered yet

              /// Whether confirmation is required, not considered yet

              /// Activate a diagnostic connection
              userTcp->ChangeConnectionState(DoipConnectionState::kActive);
              ProcessDiagConnection(userTcp, true);
          };

    /// (DoIP-002)config_.testerMaxConnections
    ServerUserManager::GetInstance().RoutingActivationHander(fd, req.sourceAddress_,
                                                             config_.testerMaxConnections,  // NOLINT
                                                             socketHandlerCb);
}

/// @brief Handle logic for obtaining DOIP entity information
/// @param[in] userBase udp user
void ServerImpl::_processDoipEntityStatus(ServerUserBase* userBase)
{
    if (userBase == nullptr) {
        LogError() << "ServerImpl::_processDoipEntityStatus|userBase is nullptr";
        return;
    }
    EntityStatusResponse resp;
    resp.entityType_     = config_.isGateWay ? NodeType::kDoipGateway : NodeType::kDoipNode;                 // NOLINT
    resp.maxOpenSockets_ = static_cast< uint8_t >(config_.testerMaxConnections);                             // NOLINT
    uint8_t openSockets  = static_cast< uint8_t >(ServerUserManager::GetInstance().GetActivateTcpNumber());  // NOLINT
    resp.currentlyOpenSocket_ = openSockets;                                                                 // NOLINT
    resp.maxDataSize_         = config_.requestMaxBytes;                                                     // NOLINT
    ara::core::Vector< uint8_t > body;
    resp.Serialize(body);
    std::ignore = userBase->SendToClient(PayloadType::kDoipEntityStatusResponse, body);
    DelayRemoveUdpUser(userBase->GetFd());
}

/// @brief Handle alive check logic
/// @param[in] userBase tcp user
/// @param[in] vbody Alive check data packet
void ServerImpl::_processAliveCheckResponse(ServerUserBase* userBase, ara::core::Vector< uint8_t > const& vbody)
{
    ServerUserTcp* pUserTcp = dynamic_cast< ServerUserTcp* >(userBase);
    if (pUserTcp == nullptr) {
        LogError() << "ServerImpl::_processAliveCheckResponse|pUserTcp is nullptr";
        return;
    }

    AliveCheckResponse req;
    if (!req.Deserialize(vbody)) {
        LogError() << "ServerImpl::_processAliveCheckResponse|Deserialize AliveCheckResponse err";
        return;
    }

    /// Heartbeat response
    pUserTcp->DoAliveCheckResponse(req.sourceAddress_);  // NOLINT
}

/// @brief Handle diagnostic message
/// @param[in] userBase tcp user
/// @param[in] type Protocol type
/// @param[in] vbody Diagnostic data packet
void ServerImpl::_processDiagMessage(ServerUserBase* userBase,
                                     PayloadType const type,
                                     ara::core::Vector< uint8_t > const& vbody)
{
    ServerUserTcp* pUserTcp = dynamic_cast< ServerUserTcp* >(userBase);
    if (pUserTcp == nullptr) {
        LogError() << "ServerImpl::_processDiagMessage|pUserTcp is nullptr";
        return;
    }

    DiagMessage req;
    if (!req.Deserialize(vbody)) {
        LogError() << "ServerImpl::_processDiagMessage|Deserialize DiagMessage err";
        return;
    }

    std::function< void(DiagNackType const) > response = [&pUserTcp, &req](DiagNackType const codeType) {
        DiagMessageNack nck;
        nck.sourceAddress_ = req.targetAddress_;  // NOLINT
        nck.targetAddress_ = req.sourceAddress_;  // NOLINT
        nck.nck_           = codeType;            // NOLINT
        ara::core::Vector< uint8_t > body;
        nck.Serialize(body);
        std::ignore = pUserTcp->SendToClient(PayloadType::kDiagMessageNegativeAcknowledgement, body);
    };

    /// Check SA
    if (req.sourceAddress_ != pUserTcp->GetSa()) {  // NOLINT
        response(DiagNackType::kInvalidSourceAddress);
        pUserTcp->ChangeConnectionState(DoipConnectionState::kFinalize);
        LogError() << "ServerImpl::_processDiagMessage|check req sa =" << req.sourceAddress_  // NOLINT
                   << "sa =" << pUserTcp->GetSa() << "fd =" << pUserTcp->GetFd();
        return;
    }

    if (!config_.diagAddressRadius_.empty()) {
        /// Check TA
        bool result{false};
        for (size_t i = 0U; i < config_.diagAddressRadius_.size(); ++i) {
            DiagAddressRadius const& diagAddress = config_.diagAddressRadius_[i];
            if ((diagAddress.startAddress <= req.targetAddress_) && (diagAddress.endAddress >= req.targetAddress_)) {
                result = true;
                break;
            }
        }
        if (!result) {
            response(DiagNackType::kInvalidTargetAddress);
            return;
        }
    }

    /// Pass diagnostic message to upper layer
    requestHandler_(pUserTcp->GetFd(), type, vbody);
}

/// @brief Reply vehicle announcement information
/// @param[in] fd sock fd
/// @param[in] protocloType Protocol number
/// @param[in] vbody Message body
/// @param[in] isSend Whether to send this message
/// @return Send result
bool ServerImpl::_responseVehicleAnnoucementId(int32_t const fd,
                                               PayloadType const protocloType,
                                               ara::core::Vector< std::uint8_t > const& vbody,
                                               bool const isSend)
{
    std::function< void() > cb = [this, fd, protocloType, vbody, isSend]() {
        ServerUserUdp* userUdp = ServerUserManager::GetInstance().GetUdpUser(fd);
        if (userUdp == nullptr) {
            LogError() << "ServerImpl::_responseVehicleAnnoucementId|userUdp is nullptr fd =" << fd;
            return;
        }

        if (isSend) {
            std::ignore = userUdp->SendToClient(protocloType, vbody);
        }
        DelayRemoveUdpUser(fd);
        vehicleIdTimer_.reset();
    };

    /// According to iso-13400 table12 A_DoIP_Announce_Wait
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1U, static_cast< int >(config_.vehicleAnnouncementWait));
    uint32_t genResult = dis(gen);
    if (genResult == 0U) {
        genResult = 1U;
    }
    int32_t const makeTimerRes = evloop_->MakeTimer(vehicleIdTimer_, static_cast< int >(genResult), std::move(cb));
    LogDebug() << "ServerImpl::_responseVehicleAnnoucementId|gen_random_num =" << genResult
               << "makeTimerRes =" << makeTimerRes << "config_wait_time =" << config_.vehicleAnnouncementWait;
    return true;
}

/// @brief Reply UDP type message
/// @param[in] fd sock fd
/// @param[in] protocloType Protocol number
/// @param[in] vbody Message body
/// @return Reply result
bool ServerImpl::_responseUdp(int32_t const fd,
                              PayloadType const protocloType,
                              ara::core::Vector< std::uint8_t > const& vbody)
{
    ServerUserUdp* userUdp = ServerUserManager::GetInstance().GetUdpUser(fd);
    if (userUdp == nullptr) {
        LogError() << "ServerImpl::_responseUdp|userUdp is nullptr fd =" << fd;
        return false;
    }

    int32_t sendLen{userUdp->SendToClient(protocloType, vbody)};
    DelayRemoveUdpUser(fd);
    return sendLen > 0;
}

/// @brief Reply TCP type message
/// @param[in] fd sock fd
/// @param[in] protocloType Protocol number
/// @param[in] vbody Message body
/// @return Reply result
bool ServerImpl::_responseTcp(int32_t const fd,
                              PayloadType const protocloType,
                              ara::core::Vector< std::uint8_t > const& vbody)
{
    ServerUserTcp* userTcp = ServerUserManager::GetInstance().GetTcpUser(fd);
    if (userTcp == nullptr) {
        LogError() << "ServerImpl::_responseTcp|userTcp is nullptr fd =" << fd;
        return false;
    }

    int32_t sendLen{userTcp->SendToClient(protocloType, vbody)};
    return sendLen > 0;
}

void ServerImpl::DelayRemoveUdpUser(int32_t const fd)
{
    if (evloop_.get() == nullptr) {
        return;
    }

    std::ignore = evloop_->Exec([fd]() { ServerUserManager::GetInstance().RemoveUdpUser(fd); });
}

/// @brief Get broadcast address
/// @param[out] buf Broadcast address
void ServerImpl::_getBroadcast(nai_socknbuf_t& buf) const
{
    ara::core::String ip   = config_.serverIP_;  // NOLINT
    ara::core::String mask = config_.networkMask;
    ara::core::String broadcastIp{"255.255.255.255"};
    // std::ignore = ip.append(".");
    // std::ignore = mask.append(".");
    // std::size_t ip_find_pos{ip.find_first_of(".")};
    // std::size_t mask_find_pos{mask.find_first_of(".")};
    // std::size_t ip_start{0U};
    // std::size_t mask_start{0U};
    // while ((ip_find_pos != std::string::npos) && (mask_find_pos != std::string::npos)) {
    //     ara::core::String const sub_ip{std::move(ip.substr(ip_start, ip_find_pos))};
    //     uint8_t const ip_value{static_cast<uint8_t>(std::stoi(sub_ip.c_str()))};
    //     ara::core::String const sub_mask{std::move(mask.substr(mask_start, mask_find_pos))};
    //     uint8_t const mask_value{static_cast<uint8_t>(std::stoi(sub_mask.c_str()))};
    //     uint8_t broadcast_value{static_cast<uint8_t>(mask_value & ip_value)};
    //     broadcast_value |= ~mask_value;
    //     std::ignore = broadcastIp.append(std::to_string(static_cast<uint32_t>(broadcast_value)));
    //     std::ignore = broadcastIp.append(".");
    //     ip_start = ip_find_pos + 1U;
    //     mask_start = mask_find_pos + 1U;
    //     ip_find_pos = ip.find_first_of(".", ip_start);
    //     mask_find_pos = mask.find_first_of(".", mask_start);
    // }
    // broadcastIp.pop_back();

    std::ignore = broadcastIp.append(":");
    std::int32_t const port{static_cast< int32_t >(config_.udpPort_)};  // NOLINT
    std::ignore = broadcastIp.append(std::to_string(port));
    buf.len     = static_cast< int32_t >(sizeof(buf.storage));
    std::int32_t const res{nai_sockaddr_pton(broadcastIp.data(), broadcastIp.length(), &buf.addr, &buf.len)};
    if (res == -1) {
        LogError() << "ServerImpl::_getBroadcast|nai_sockaddr_pton err:" << isoft::threadsafe::Safe_Strerror(errno);
    }
    LogInfo() << "ServerImpl::_getBroadcast|ip =" << ip.c_str() << "mask =" << mask.c_str()
              << "broadcastIp =" << broadcastIp.c_str();
}

/// @brief 处理收到单播或广播消息
/// @param dgram udp句柄
void ServerImpl::_handleUdpData(PublicUdpDgram* const dgram)
{
    if (dgram == nullptr) {
        LogError() << "ServerImpl::_handleUdpData|dgram is nullptr";
        return;
    }

    int32_t ec;
    while (true) {
        std::vector< uint8_t > buffer(config_.requestMaxBytes);
        nai_socknbuf_t nai_socknbuf_t_;
        nai_socknbuf_t_.len = static_cast< int32_t >(sizeof(nai_socknbuf_t_.storage));
        int32_t recvRes
            = dgram->Recv(buffer.data(), config_.requestMaxBytes, &nai_socknbuf_t_.addr, &nai_socknbuf_t_.len);
        if (recvRes <= 0) {
            ec = nai_errno;
            if (ec == NAI_EAGAIN || ec == NAI_EINPROGRESS) {
                LogDebug() << "ServerImpl::_handleUdpData|please try later";
                break;
            }
            LogError() << "ServerImpl::_handleUdpData|detect error, no = " << strerror(errno);
            break;
        }
        uint32_t udpFd = _allocateUdpFd();
        std::unique_ptr< ServerUserUdp > udpUser
            = std::make_unique< ServerUserUdp >(udpFd, sendUdp_, this, config_.requestMaxBytes);
        if (udpUser.get() == nullptr) {
            LogError() << "ServerImpl::_handleUdpData|create usp user err ucpUser is nullptr";
            return;
        }

        LogDebug() << "ServerImpl::_handleUdpData|recieve data size =" << recvRes;
        udpUser->SetSockBuf(nai_socknbuf_t_);
        /// 添加到user管理器统一管理并处理一次数据
        ServerUserManager::GetInstance().AddUdpUser(std::move(udpUser), buffer.data(), recvRes);
    }
}

/// @brief 获取本机所有的ipv4
/// @return 所有的地址
int32_t ServerImpl::GenAllIpv4()
{
    int32_t retVal = 0;

    struct ifaddrs* ifAddrStruct = nullptr;
    void* tmpAddrPtr             = nullptr;

    retVal = getifaddrs(&ifAddrStruct);
    if (0 != retVal) {
        retVal = errno;

        return retVal;
    }

    ara::core::String strIpvX;

    char addressBuffer[INET_ADDRSTRLEN] = {'0'};

    while (nullptr != ifAddrStruct) {
        if (ifAddrStruct->ifa_addr != nullptr && AF_INET == ifAddrStruct->ifa_addr->sa_family) {
            tmpAddrPtr = &(reinterpret_cast< struct sockaddr_in* >(ifAddrStruct->ifa_addr))->sin_addr;

            std::ignore = inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
            strIpvX     = ara::core::String{addressBuffer};

            std::ignore = setAllIpv4_.insert(strIpvX);

            std::ignore = memset(addressBuffer, 0, INET_ADDRSTRLEN);
        }

        ifAddrStruct = ifAddrStruct->ifa_next;
    }

    return retVal;
}

}  // namespace doip
}  // namespace isoft