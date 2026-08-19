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
/// @file       doip_server.cpp
/// @brief      This file provides the implementation of the Diagnostic Service class, which is used to implement the standard diagnostic transmission handler
/// @details
/// @date       2023-10-08
/// @author     zhanglipeng
/// @version    1.2.0
///
/// ================================================================

#include "doip_server.h"

#include <isoft/naicpp/global_evloop.h>

#include <atomic>

#include "cancellation/cancellation_event.h"
#include "channel_id_manager.h"
#include "diag_channel.h"
#include "diag_channel_manager.h"
#include "doip_config.h"
#include "gen_code/activation_line/serviceAgent/activation_line_agent.h"
#include "gen_code/generic_data_identifier/serviceAgent/genericDataIdentifier_agent.h"
#include "gen_code/group_identification/serviceAgent/group_identification_agent.h"
#include "gen_code/power_mode/serviceAgent/power_mode_agent.h"
#include "gen_code/raw_data_identifier/serviceAgent/raw_data_identifier_agent.h"
#include "gen_code/trigger_vehicle_announcement/serviceAgent/trigger_vehicle_announcement_agent.h"
#include "log/log.h"
#include "serialization/serialization/common_data_type.h"
#include "thread_pool/thread_pool.h"
namespace ara {
namespace diag {
namespace doip {

///@brief Constructor
///@param[in] handlerId DoIP protocol ID
///@param[in] transportProtocolMgr Transport layer manager
/// @throw
DoIPServer::DoIPServer(std::shared_ptr< isoft::uds::server::ServerManager > serverManager) noexcept
    : isoft::uds::server::Transport{std::move(serverManager)}
    , diagChannelManagerPtr_{std::make_shared< DiagChannelManager >()}
    , stop_{false}
    , mapDoipServer_{}
    , mapNetinterfaceToIp_{}
{
}

/// @brief Destructor
/// @throw unknown
DoIPServer::~DoIPServer() noexcept
{
    diagChannelManagerPtr_.reset();
    doipTriggerVehicleAnnouncementPtr_.reset();
    doipPowerModeHandlerPtr_.reset();
    doipGroupIdHandlerPtr_.reset();
    mapDoipServer_.clear();
    mapNetinterfaceToIp_.clear();

    for (auto& entry : mapActiveHandlerPtr_) {
        if (entry.second.get() != nullptr) {
            continue;
        }

        entry.second.reset();
    }
    mapActiveHandlerPtr_.clear();
}

#ifdef NMODEL
/// @brief Initialization
/// @code{.isoft}
/// export_level=/Diagnostics/Diagnostic Manager/DoIP
/// @endcode
/// @return kInitializeFailed: Failure  kInitializeOk: Success
/// @throw
bool DoIPServer::Initialize(
    std::set< std::shared_ptr< isoft::nml::definition::AdaptiveSoftwareComponent > > adaptiveSoftwareComponentList,
    std::shared_ptr< isoft::nml::definition::DoipModule > doipModulePtr) noexcept
{
    /// Load DoIP configuration
    if (!DoIPConfig::GetConfig().Initialize(adaptiveSoftwareComponentList, doipModulePtr)) {
        return false;
    }
    uint32_t triggerServiceInstanceId

        = DoIPConfig::GetConfig().TriggerVehicleAnnouncement();
    if (triggerServiceInstanceId > 0) {
        /// init trigger
        doipTriggerVehicleAnnouncementPtr_
            = std::make_shared< isoft::dm::dis::TriggerVehicleAnnouncementAgent >(triggerServiceInstanceId

            );
        doipTriggerVehicleAnnouncementPtr_->RegisterTriggerVehicleAnnouncement(
            [this](uint8_t networkInterfaceId) -> int32_t {
                // bool const bIsActivationLineDependent{
                //     DoIPConfig::GetConfig().IsActivationLineDependent(networkInterfaceId)};
                // if (bIsActivationLineDependent) {
                //     common::LogError()
                //         << "DoIPServer::Initialize|doip net need activation line not need trigger|net id ="
                //         << networkInterfaceId;
                //     return 100;
                // }

                // _startNetworkService(networkInterfaceId);

                /// send vehicle announcement
                _broadcastAnnouncement(networkInterfaceId);

                common::LogVerbose() << "DoIPServer::Initialize|Trigger is called|netword_id = " << networkInterfaceId;
                return 0;
            });

        common::LogVerbose() << "DoIPServer::Initialize|config trigger interface triggerServiceInstanceId ="
                             << triggerServiceInstanceId;

    } else {
        common::LogVerbose() << "DoIPServer::Initialize|no config trigger interface";
    }
    PowerModeInfo& power = DoIPConfig::GetConfig().PowerMode();

    if (power.serviceInstanceId > 0) {
        doipPowerModeHandlerPtr_ = std::make_shared< isoft::dm::dic::PowerModeAgent >(

            power.instanceId, power.serviceInstanceId

        );
    } else {
        common::LogVerbose() << "DoIPServer::Initialize|no config power interface";
    }

    GroupIdentificationInfo& group = DoIPConfig::GetConfig().GroupGroupIdentification();

    if (group.serviceInstanceId > 0) {
        doipGroupIdHandlerPtr_ = std::make_shared< isoft::dm::dic::GroupIdentificationAgent >(

            group.instanceId, group.serviceInstanceId

        );
    } else {
        common::LogVerbose() << "DoIPServer::Initialize|no config group identification interface";
    }

    ChannelIdManager::GetInstance().Initialize();

    /// Monitor network card status changes
    // ara::core::Result<void> startNetLinkMonitorResult =
    //     isoft::doip::Server::StartNetlinkMonitor([this](int32_t const networkId, ara::core::String const& ip) {
    //         TaskTool::PostTask(std::move([this, networkId, ip]() {
    //             _stopNetworkService(static_cast<uint8_t>(networkId));
    //             _updateNetInterfaceIp(static_cast<uint8_t>(networkId), ip);
    //             common::LogInfo() << "DoIPServer::Initialize|netlink cb networkId =" << networkId
    //                               << "ip =" << ip.c_str();
    //         }));
    //     });
    // if (!startNetLinkMonitorResult.HasValue()) {
    //     common::LogWarn() << "DoIPServer::Initialize|start netlink monitor fails:" <<
    //     startNetLinkMonitorResult.Error();
    // }

    _createReadVinInterface();
    common::LogInfo() << "DoIPServer::Initialize|success";
    return true;
    return true;
}
#endif

/// @brief Initialization
/// @return kInitializeFailed: Failure  kInitializeOk: Success
/// @throw unknown
bool DoIPServer::Initialize()
{
    /// Load DoIP configuration
    if (!DoIPConfig::GetConfig().Init()) {
        return false;
    }

    uint32_t triggerServiceInstanceId

        = DoIPConfig::GetConfig().TriggerVehicleAnnouncement();
    if (triggerServiceInstanceId > 0) {
        /// init trigger
        doipTriggerVehicleAnnouncementPtr_
            = std::make_shared< isoft::dm::dis::TriggerVehicleAnnouncementAgent >(triggerServiceInstanceId

            );
        doipTriggerVehicleAnnouncementPtr_->RegisterTriggerVehicleAnnouncement(
            [this](uint8_t networkInterfaceId) -> int32_t {
                // bool const bIsActivationLineDependent{
                //     DoIPConfig::GetConfig().IsActivationLineDependent(networkInterfaceId)};
                // if (bIsActivationLineDependent) {
                //     common::LogError()
                //         << "DoIPServer::Initialize|doip net need activation line not need trigger|net id ="
                //         << networkInterfaceId;
                //     return 100;
                // }

                // _startNetworkService(networkInterfaceId);

                /// send vehicle announcement
                _broadcastAnnouncement(networkInterfaceId);

                common::LogVerbose() << "DoIPServer::Initialize|Trigger is called|netword_id = " << networkInterfaceId;
                return 0;
            });

        common::LogVerbose() << "DoIPServer::Initialize|config trigger interface triggerServiceInstanceId ="
                             << triggerServiceInstanceId;

    } else {
        common::LogVerbose() << "DoIPServer::Initialize|no config trigger interface";
    }
    PowerModeInfo& power = DoIPConfig::GetConfig().PowerMode();

    if (power.serviceInstanceId > 0) {
        doipPowerModeHandlerPtr_ = std::make_shared< isoft::dm::dic::PowerModeAgent >(

            power.instanceId, power.serviceInstanceId

        );
    } else {
        common::LogVerbose() << "DoIPServer::Initialize|no config power interface";
    }

    GroupIdentificationInfo& group = DoIPConfig::GetConfig().GroupGroupIdentification();

    if (group.serviceInstanceId > 0) {
        doipGroupIdHandlerPtr_ = std::make_shared< isoft::dm::dic::GroupIdentificationAgent >(

            group.instanceId, group.serviceInstanceId

        );
    } else {
        common::LogVerbose() << "DoIPServer::Initialize|no config group identification interface";
    }

    ChannelIdManager::GetInstance().Initialize();

    /// Monitor network card status changes
    // ara::core::Result<void> startNetLinkMonitorResult =
    //     isoft::doip::Server::StartNetlinkMonitor([this](int32_t const networkId, ara::core::String const& ip) {
    //         TaskTool::PostTask(std::move([this, networkId, ip]() {
    //             _stopNetworkService(static_cast<uint8_t>(networkId));
    //             _updateNetInterfaceIp(static_cast<uint8_t>(networkId), ip);
    //             common::LogInfo() << "DoIPServer::Initialize|netlink cb networkId =" << networkId
    //                               << "ip =" << ip.c_str();
    //         }));
    //     });
    // if (!startNetLinkMonitorResult.HasValue()) {
    //     common::LogWarn() << "DoIPServer::Initialize|start netlink monitor fails:" <<
    //     startNetLinkMonitorResult.Error();
    // }

    _createReadVinInterface();
    common::LogInfo() << "DoIPServer::Initialize|success";
    return true;
}

/// @brief Start DoIP service
/// @throw unknown
bool DoIPServer::Start()
{
    /// Construct DoIP network
    ara::core::Map< uint8_t, DoIpNetworkConfiguration > const& doipNetConfig
        = DoIPConfig::GetConfig().GetDoIPNetworkConfiguration();

    for (auto const& config : doipNetConfig) {
        if (config.second.isActivationLineDependent) {
            common::LogVerbose() << "DoIPServer::Start|doip net need activation line net_id =" << config.first;
            continue;
        }

        bool const res = _startNetworkService(config.first);
        common::LogVerbose() << "DoIPServer::Start|doip net start, because it not need ctivation line net_id ="
                             << config.first << "res =" << res;
    }

    /// Construct activation line interface
    ara::core::Vector< ActivationLineInfo >& activation = DoIPConfig::GetConfig().ActivationLine();
    if (!activation.empty()) {
        for (auto& config : activation) {
            uint16_t const instanceID        = config.instanceId;
            uint32_t const serviceInstanceID = config.serviceInstanceId;

            std::shared_ptr< isoft::dm::dic::ActivationLineAgent > acAgent
                = std::make_shared< isoft::dm::dic::ActivationLineAgent >(

                    instanceID, serviceInstanceID

                );

            acAgent->RegisterOnReady([this,

                                      instanceID, serviceInstanceID

            ](bool ready) {
                common::LogVerbose() << "DoIPServer::Start|notify activation service ready instanceID =" << instanceID
                                     << "serviceInstanceID" << serviceInstanceID << "ready =" << ready;

                if (!ready) {
                    return;
                }

                static std::atomic_bool s_IsCall{false};
                if (s_IsCall.load()) {
                    return;
                }

                s_IsCall.store(true);

                isoft::ThreadPool::GetInstance().Submit([this,

                                                         instanceID, serviceInstanceID

                ]() {
                    ara::core::Map< uint32_t, std::shared_ptr< isoft::dm::dic::ActivationLineAgent > >::iterator iter
                        = mapActiveHandlerPtr_.find(

                            serviceInstanceID

                        );
                    if (iter == mapActiveHandlerPtr_.end()) {
                        common::LogError()
                            << "DoIPServer::Start|notify activation service ready don not have ac instance "
                               "instanceID ="

                            << instanceID << "serviceInstanceID" << serviceInstanceID;

                        return;
                    }

                    if (iter->second.get() == nullptr) {
                        common::LogError()
                            << "DoIPServer::Start|notify activation service ready ac is nullptr instance "
                               "instanceID ="

                            << instanceID << "serviceInstanceID" << serviceInstanceID;

                        return;
                    }

                    ara::core::Result< std::uint8_t > networkIDRes = iter->second->GetNetworkInterfaceId();
                    if (!networkIDRes.HasValue()) {
                        common::LogError()
                            << "DoIPServer::Start|get network error =" << networkIDRes.Error() << "instanceID ="

                            << instanceID << "serviceInstanceID" << serviceInstanceID;

                        s_IsCall.store(false);
                        return;
                    }

                    uint8_t const networkInterfaceId = networkIDRes.Value();
                    bool const bIsActivationLineDependent{
                        DoIPConfig::GetConfig().IsActivationLineDependent(networkInterfaceId)};
                    if (!bIsActivationLineDependent) {
                        common::LogWarn() << "DoIPServer::Start|doip net need trigger,not need activation line|net id ="
                                          << networkInterfaceId;
                        return;
                    }

                    ara::core::Result< bool > stateRes = iter->second->GetActivationLineState();
                    if (!stateRes.HasValue()) {
                        common::LogError() << "DoIPServer::Start|get activation line state error =" << stateRes.Error()
                                           << "instanceID ="

                                           << instanceID << "serviceInstanceID" << serviceInstanceID;

                        s_IsCall.store(false);
                        return;
                    }

                    if (stateRes.Value()) {
                        if (_startNetworkService(networkInterfaceId)) {
                            /// Send announcement
                            _broadcastAnnouncement(networkInterfaceId);
                        }
                    } else {
                        _stopNetworkService(networkInterfaceId);
                    }

                    iter->second->RegisterActivationLineStateNotifier([this, networkInterfaceId](bool state) {
                        common::LogVerbose() << "DoIPServer::Start|notify update activation state networkInterfaceId ="
                                             << networkInterfaceId << "state" << state;
                        if (state) {
                            if (_startNetworkService(networkInterfaceId)) {
                                /// Send announcement
                                _broadcastAnnouncement(networkInterfaceId);
                            }
                        } else {
                            _stopNetworkService(networkInterfaceId);
                        }
                    });

                    common::LogVerbose() << "DoIPServer::Start|notify activation service ready finished instanceID ="

                                         << instanceID << "serviceInstanceID" << serviceInstanceID;
                });
            });
            mapActiveHandlerPtr_[

                serviceInstanceID

            ] = acAgent;
        }
    } else {
        common::LogVerbose() << "DoIPServer::Start|no config activation interface";
    }

    common::LogVerbose() << "DoIPServer::Start|success net_id_size =" << doipNetConfig.size()
                         << "activation_size =" << activation.size();
    return true;
}

/// @brief Stop DoIP service
/// @throw unknown
void DoIPServer::Stop()
{
    if (stop_) {
        return;
    }

    if (diagChannelManagerPtr_.get() != nullptr) {
        diagChannelManagerPtr_->Stop();
    }

    /// Stop DoIP service
    for (ara::core::Map< uint8_t, std::shared_ptr< isoft::doip::Server > >::iterator it = mapDoipServer_.begin();
         it != mapDoipServer_.end(); ++it) {
        if (it->second.get() == nullptr) {
            continue;
        }

        std::ignore = it->second->Stop();
    }

    /// Stop network card status monitoring
    /// isoft::doip::Server::StopNetlinkMonitor();

    /// Reset stop flag
    serverManager_.reset();
    stop_ = true;
}

std::shared_ptr< isoft::uds::server::Channel > DoIPServer::GetChannel(isoft::uds::server::Address serverAddr,
                                                                      isoft::uds::server::Address clientAddr) noexcept
{
    if (diagChannelManagerPtr_.get() == nullptr) {
        common::LogError() << "DoIPServer::GetChannel|diagChannelManagerPtr_ is nullptr clientAddr =" << clientAddr;
        return nullptr;
    }
    std::ignore = serverAddr;
    return diagChannelManagerPtr_->GetDiagChannelPtr(clientAddr);
}

/// @brief Reply to DoIP message
/// @param[in] networkId Network port ID
/// @param[in] fd fd
/// @param[in] protocloType Protocol number
/// @param[in] msg Message body
/// @param[in] isSend Whether to send
/// @return Reply result
bool DoIPServer::SendDoIPMessage(uint8_t const networkId,
                                 int32_t const fd,
                                 isoft::doip::PayloadType const protocloType,
                                 ara::core::Vector< uint8_t > const& msg,
                                 bool const isSend)
{
    ara::core::Map< uint8_t, std::shared_ptr< isoft::doip::Server > >::iterator it = mapDoipServer_.find(networkId);
    if (it == mapDoipServer_.end()) {
        common::LogError() << "DoIPServer::SendDoIPMessage|no have server networkId =" << networkId;
        return false;
    }

    if (it->second.get() == nullptr) {
        common::LogError() << "DoIPServer::SendDoIPMessage|server is nullptr networkId =" << networkId;
        return false;
    }

    return it->second->SendDoIPMessage(fd, protocloType, msg, isSend);
}

/// @brief Enable network
/// @param[in] networkId Network ID
bool DoIPServer::_startNetworkService(uint8_t const networkId) noexcept
{
    ara::core::Map< uint8_t, std::shared_ptr< isoft::doip::Server > >::iterator it{mapDoipServer_.find(networkId)};
    if (it != mapDoipServer_.end()) {
        common::LogError() << "DoIPServer::_startNetworkService|service has start networkId =" << networkId;
        return false;
    }

    /// Get DoIP server configuration
    isoft::doip::ServerConfig config;
    if (!DoIPConfig::GetConfig().GetDoipServerConfig(networkId, config)) {
        common::LogError() << "DoIPServer::_startNetworkService|network not config networkId =" << networkId;
        return false;
    }
    ara::core::String const dynamicIP = _getNetInterfaceIpById(networkId);
    if (!dynamicIP.empty()) {
        config.serverIP_ = dynamicIP;  // NOLINT
        common::LogInfo() << "DoIPServer::_startNetworkService|netinterface get dynamic ip networkId =" << networkId
                          << "ip =" << dynamicIP.c_str();
    }

    /// create doip service
    std::unique_ptr< isoft::doip::Server > server{std::make_unique< isoft::doip::Server >(
        config, [this, networkId](int32_t const fd, isoft::doip::PayloadType const protocloType,
                                  ara::core::Vector< uint8_t > const& msg) noexcept {
            switch (protocloType) {
                case isoft::doip::PayloadType::kVehicleIdRequest: {
                    _dealVehicleIdRequest(networkId, fd);
                } break;
                case isoft::doip::PayloadType::kVehicleIdRequestWithEid: {
                    _dealVehicleIdRequestWithEid(networkId, fd, msg);
                } break;
                case isoft::doip::PayloadType::kVehicleIdRequestWithVin: {
                    _dealVehicleIdRequestWithVin(networkId, fd, msg);
                } break;
                case isoft::doip::PayloadType::kDiagPowerModeInfoRequest: {
                    _dealPowerModeInfoRequest(networkId, fd);
                } break;
                case isoft::doip::PayloadType::kDiagMessage: {
                    _dealDiagMessage(networkId, fd, msg);
                } break;
                default: {
                    common::LogError() << "DoIPServer::_startNetworkService|unknown msg networkId =" << networkId
                                       << "protocloType =" << ara::log::LogHex16{static_cast< uint16_t >(protocloType)};
                } break;
            }
        })};

    if (server.get() == nullptr) {
        common::LogError() << "DoIPServer::_startNetworkService|Initialize service is nullptr";
        return false;
    }

    /// start doip and register cb
    ara::core::Result< void > activateResult{
        server->Start([this, networkId](int32_t const fd, uint16_t const sa, ara::core::String const& localIp,
                                        uint16_t const localPort, ara::core::String const& peerIp,
                                        uint16_t const peerPort, bool const state) {
            if (state) {
                /// open dia connection
                _openDiagChannel(networkId, fd, sa, localIp, localPort, peerIp, peerPort);
            } else {
                /// close dia connection
                _closeDiagChannel(networkId, fd);
            }
        })};

    if (!activateResult.HasValue()) {
        common::LogError() << "DoIPServer::_startNetworkService|Start service fails:" << activateResult.Error();
        return false;
    }

    mapDoipServer_[networkId] = std::move(server);
    common::LogVerbose() << "DoIPServer::_startNetworkService|add a doip service size =" << mapDoipServer_.size()
                         << "networkId =" << networkId;

    /// send vehicle announcement
    // _getVehicleId([this, networkId](isoft::doip::VehicleIdResponse& res) {
    //     ara::core::Map< uint8_t, std::shared_ptr< isoft::doip::Server > >::iterator iter{
    //         mapDoipServer_.find(networkId)};
    //     if (iter == mapDoipServer_.end()) {
    //         common::LogError() << "DoIPServer::_startNetworkService|_getVehicleId server is nullptr networkId="
    //                            << networkId;
    //         return;
    //     }
    //     isoft::doip::EidValue eid{_getEid(networkId)};
    //     for (size_t i{0U}; i < eid.size(); i++) {
    //         res.eid_[i] = eid[i];
    //     }
    //     ara::core::Vector< uint8_t > body;
    //     res.Serialize(body);
    //     iter->second->_broadcastAnnouncement(body);
    //     common::LogVerbose() << "DoIPServer::_startNetworkService|_broadcastAnnouncement networkId =" << networkId;
    // });

    return true;
}

/// @brief Stop network service
/// @param[in] networkId Network ID
void DoIPServer::_stopNetworkService(uint8_t const networkId) noexcept
{
    ara::core::Map< uint8_t, std::shared_ptr< isoft::doip::Server > >::iterator it{mapDoipServer_.find(networkId)};
    if (it == mapDoipServer_.end()) {
        return;
    }

    std::ignore = it->second->Stop();
    std::ignore = mapDoipServer_.erase(it);
}

/// @brief Monitor network card IP changes and update the IP corresponding to the network port
/// @param networkId Network port ID
/// @param ip Updated IP
void DoIPServer::_updateNetInterfaceIp(uint8_t const networkId, ara::core::String const& ip)
{
    if (ip.empty()) {
        return;
    }

    mapNetinterfaceToIp_[networkId] = ip;
}

/// @brief Get the IP of the network port
/// @param networkId Network port ID
/// @return ip
ara::core::String DoIPServer::_getNetInterfaceIpById(uint8_t const networkId)
{
    ara::core::Map< uint8_t, ara::core::String >::iterator it = mapNetinterfaceToIp_.find(networkId);
    if (it == mapNetinterfaceToIp_.end()) {
        return "";
    }

    return it->second;
}

/// @brief Open a diagnostic connection
/// @param[in] networkId Network port ID
/// @param[in] fd fd
/// @param[in] localIp Local IP
/// @param[in] localPort Local port
/// @param[in] peerIp Peer IP
/// @param[in] peerPort Peer port
void DoIPServer::_openDiagChannel(uint8_t const networkId,
                                  int32_t const fd,
                                  uint16_t const sa,
                                  ara::core::String const& localIp,
                                  uint16_t const localPort,
                                  ara::core::String const& peerIp,
                                  uint16_t const peerPort)
{
    if (serverManager_.get() == nullptr) {
        common::LogError() << "DoIPServer::_openDiagChannel|serverManager_ is nullptr networkId =" << networkId
                           << "fd =" << fd << "sa =" << sa;
        return;
    }

    if (diagChannelManagerPtr_.get() == nullptr) {
        common::LogError() << "DoIPServer::_openDiagChannel|diagChannelManagerPtr_ is nullptr networkId =" << networkId
                           << "fd =" << fd << "sa =" << sa;
        return;
    }

    std::uint64_t udsChannelId{0U};
    int32_t const ta{ChannelIdManager::GetInstance().Generate(localIp, localPort, peerIp, peerPort, udsChannelId)};
    std::shared_ptr< DiagChannel > diagChannel{
        std::make_shared< DiagChannel >(this, sa, udsChannelId, networkId, fd, localIp, localPort, peerIp, peerPort)};
    diagChannelManagerPtr_->AddDiagConnectionPtr(diagChannel);

    common::LogInfo() << "DoIPServer::_openDiagChannel|open DiagChannel"
                      << "peerIp =" << peerIp.c_str() << "peerPort =" << peerPort << "localIp =" << localIp.c_str()
                      << "localPort =" << localPort << "udsChannelId =" << udsChannelId << "fd =" << fd
                      << "networkId =" << networkId << "sa =" << sa << "ta" << ta;

    /// Channel reconnection logic
    if (ta > 0) {
        decltype(auto) server{serverManager_->GetServerByPa(ta)};
        if (server == nullptr) {
            common::LogError() << "DoIPServer::_openDiagChannel|channel reestablishment invalid physical address ="
                               << ta;
            return;
        }

        server->NotifyReestablishment(diagChannel);
    }
}

/// @brief Close a diagnostic connection
/// @param[in] localIp Local IP
/// @param[in] localPort Local port
/// @param[in] peerIp Peer IP
/// @param[in] peerPort Peer port
void DoIPServer::_closeDiagChannel(uint8_t const networkId, int32_t const fd)
{
    common::LogInfo() << "DoIPServer::_closeDiagChannel|close DiagChannel networkId =" << networkId << " fd =" << fd;

    diagChannelManagerPtr_->RemoveDiagConnectionPtr(networkId, fd);
}

/// @brief Handle VIN acquisition logic
/// @param[in] networkId Network port ID
/// @param[in] fd sock fd
void DoIPServer::_dealVehicleIdRequest(uint8_t const networkId, int32_t const fd)
{
    if (DoIPConfig::GetConfig().GetNoParamVehicleIdentificationSwitch()) {
        common::LogWarn() << "DoIPServer::_dealVehicleIdRequest|config param not allow response vehicleid, networkId ="
                          << networkId << " fd =" << fd;
        return;
    }
    _getVehicleId([this, networkId, fd](isoft::doip::VehicleIdResponse& res) {
        isoft::doip::EidValue eid{_getEid(networkId)};
        for (size_t i{0U}; i < eid.size(); i++) {
            res.eid_[i] = eid[i];  // NOLINT
        }
        ara::core::Vector< uint8_t > body;
        res.Serialize(body);
        SendDoIPMessage(networkId, fd, isoft::doip::PayloadType::kVehicleAnnoucementIdResponse, body);
    });
}

/// @brief Handle vehicle announcement request with EID
/// @param[in] networkId Network port ID
/// @param[in] fd File descriptor
/// @param[in] msg Message body
void DoIPServer::_dealVehicleIdRequestWithEid(uint8_t const networkId,
                                              int32_t const fd,
                                              ara::core::Vector< uint8_t > const& msg) noexcept
{
    isoft::doip::EidVehicleIdRequest req;
    if (!req.Deserialize(msg)) {
        common::LogError() << "DoIPServer::_dealVehicleIdRequestWithEid|Deserialize fail";
        return;
    }

    _getVehicleId([this, networkId, fd, req](isoft::doip::VehicleIdResponse& res) {
        isoft::doip::EidValue eid{_getEid(networkId)};
        for (size_t i{0U}; i < eid.size(); i++) {
            res.eid_[i] = eid[i];  // NOLINT
        }
        ara::core::Vector< uint8_t > body;
        res.Serialize(body);
        SendDoIPMessage(networkId, fd, isoft::doip::PayloadType::kVehicleAnnoucementIdResponse, body,
                        req.eid_ == res.eid_);  // NOLINT
    });
}

/// @brief Handle vehicle announcement request with VIN
/// @param[in] networkId Network port ID
/// @param[in] fd File descriptor
/// @param[in] msg Message body
void DoIPServer::_dealVehicleIdRequestWithVin(uint8_t const networkId,
                                              int32_t const fd,
                                              ara::core::Vector< uint8_t > const& msg) noexcept
{
    isoft::doip::VinVehicleIdRequest req;
    if (!req.Deserialize(msg)) {
        common::LogError() << "DoIPServer::_dealVehicleIdRequestWithVin|Deserialize fail";
        return;
    }

    _getVehicleId([this, networkId, fd, req](isoft::doip::VehicleIdResponse& res) {
        isoft::doip::EidValue eid{_getEid(networkId)};
        for (size_t i{0U}; i < eid.size(); i++) {
            res.eid_[i] = eid[i];  // NOLINT
        }
        ara::core::Vector< uint8_t > body;
        res.Serialize(body);
        SendDoIPMessage(networkId, fd, isoft::doip::PayloadType::kVehicleAnnoucementIdResponse, body,
                        req.vin_ == res.vin_);  // NOLINT
    });
}

/// @brief Handle power mode data request
/// @param[in] networkId Network port ID
/// @param[in] fd File descriptor
void DoIPServer::_dealPowerModeInfoRequest(uint8_t const networkId, int32_t const fd) noexcept
{
    if (doipPowerModeHandlerPtr_.get() == nullptr) {
        common::LogWarn()
            << "DoIPServer::_dealPowerModeInfoRequest|no config power_mode interface, return power's vaue is not_ready";
        isoft::doip::PowerModeResponse power;
        ara::core::Vector< uint8_t > body;
        power.powerMode_ = isoft::doip::PowerMode::kNotReady;  // NOLINT
        power.Serialize(body);
        SendDoIPMessage(networkId, fd, isoft::doip::PayloadType::kDiagPowerModeInfoResponse, body);
        return;
    }

    isoft::ThreadPool::GetInstance().Submit([this, networkId, fd]() {
        ara::core::Future< uint8_t > powerModeRes = doipPowerModeHandlerPtr_->GetDoIPPowerMode();
        ara::core::future_status const futureStatus{powerModeRes.wait_for(std::chrono::milliseconds(2000))};
        isoft::doip::PowerModeResponse power;
        do {
            if (futureStatus != ara::core::future_status::ready) {
                common::LogWarn() << "DoIPServer::_dealPowerModeInfoRequest|read power_mode timeout";
                break;
            }

            ara::core::Result< uint8_t > result = powerModeRes.GetResult();
            if (!result.HasValue()) {
                common::LogError() << "DoIPServer::_dealPowerModeInfoRequest|read power_mode error"
                                   << result.Error().Message();
                break;
            }

            power.powerMode_ = static_cast< isoft::doip::PowerMode >(result.Value());  // NOLINT
        } while (false);

        ara::core::Vector< uint8_t > body;
        power.Serialize(body);
        SendDoIPMessage(networkId, fd, isoft::doip::PayloadType::kDiagPowerModeInfoResponse, body);
    });
}

/// @brief Handle diagnostic message (UDS)
/// @param[in] fd sock fd
/// @param[in] msg Diagnostic message body
void DoIPServer::_dealDiagMessage(uint8_t networkId, int32_t const fd, ara::core::Vector< uint8_t > const& msg)
{
    if (serverManager_.get() == nullptr) {
        common::LogError() << "DoIPServer::_dealDiagMessage|serverManager_ is nullptr networkId =" << networkId
                           << "fd =" << fd;
        return;
    }

    DiagChannelPtr diagChannel = diagChannelManagerPtr_->GetDiagChannelPtr(networkId, fd);
    if (diagChannel.get() == nullptr) {
        common::LogError() << "DoIPServer::_dealDiagMessage|diagChannel is nullptr networkId =" << networkId
                           << "fd =" << fd;
        return;
    }

    isoft::doip::DiagMessage req;
    if (!req.Deserialize(msg)) {
        common::LogError() << "DoIPServer::_dealDiagMessage|diagMessage deserialize fail,"
                           << "fd =" << fd;
        return;
    }

    uint8_t const priorty = _getAddressPriority(req.sourceAddress_);                              // NOLINT
    isoft::uds::server::TargetAddressType const taType{_findUdsAddressType(req.targetAddress_)};  // NOLINT

    common::LogDebug() << "DoIPServer::_dealDiagMessage|begin deal networkId =" << networkId << " fd =" << fd
                       << "sa =" << req.sourceAddress_ << "ta =" << req.targetAddress_  // NOLINT
                       << "priorty =" << priorty << "taType =" << static_cast< uint16_t >(taType);

    std::shared_ptr< isoft::uds::server::Message > uds
        = std::make_shared< isoft::uds::server::Message >(req.sourceAddress_, req.targetAddress_);  // NOLINT
    uds->SetTaType(taType);
    uds->GetBody().assign(req.uds_.begin(), req.uds_.end());  // NOLINT
    if (taType == isoft::uds::server::TargetAddressType::kPhysical) {
        decltype(auto) server{serverManager_->GetServerByPa(req.targetAddress_)};  // NOLINT
        if (server == nullptr) {
            diagChannel->SendDiagNack(isoft::doip::DiagNackType::kInvalidTargetAddress, req.targetAddress_,  // NOLINT
                                      req.sourceAddress_);                                                   // NOLINT
            common::LogError() << "DoIPServer::_dealDiagMessage|invalid physical address|"
                               << req.targetAddress_;  // NOLINT
            return;
        }

        isoft::uds::Result< void > res = server->Indicate(uds, priorty, diagChannel);
        if (!res.HasValue()) {
            diagChannel->SendDiagNack(isoft::doip::DiagNackType::kReceiveBufferOverflow, req.targetAddress_,  // NOLINT
                                      req.targetAddress_);                                                    // NOLINT
            common::LogWarn() << "DoIPServer::_dealDiagMessage|physical Indicate, dcm refuse uds, error ="
                              << res.Error();
            return;
        }

        if (!diagChannel->SendDiagAck(req.targetAddress_, req.sourceAddress_)) {  // NOLINT
            common::LogError() << "DoIPServer::_dealDiagMessage|SendDiagAck error";
            return;
        }

        server->HandleMessage(uds, priorty, diagChannel);
        common::LogVerbose() << "DoIPServer::_dealDiagMessage|physical transfer uds to dcm finished uds_size ="
                             << uds->GetBody().size();
    } else {
        decltype(auto) serverList{serverManager_->GetServerByFa(req.targetAddress_)};  // NOLINT
        if (serverList.empty()) {
            diagChannel->SendDiagNack(isoft::doip::DiagNackType::kInvalidTargetAddress, req.targetAddress_,  // NOLINT
                                      req.sourceAddress_);                                                   // NOLINT
            common::LogError() << "DoIPServer::_dealDiagMessage|invalid functional address|"
                               << req.targetAddress_;  // NOLINT
            return;
        }

        common::LogDebug() << "DoIPServer::_dealDiagMessage|functional Indicate serverlist_size =" << serverList.size();

        uint16_t assigned{0U};
        for (decltype(auto) server : serverList) {
            if (server.get() == nullptr) {
                common::LogError() << "DoIPServer::_dealDiagMessage|functional server is nullptr";
                continue;
            }
            isoft::uds::Result< void > res = server->Indicate(uds, priorty, diagChannel);
            if (res.HasValue()) {
                assigned++;
                common::LogDebug() << "DoIPServer::_dealDiagMessage|functional Indicate uds_size ="
                                   << uds->GetBody().size() << "assigned =" << assigned;
            } else {
                common::LogWarn() << "DoIPServer::_dealDiagMessage|functional Indicate, dcm refuse uds, error ="
                                  << res.Error() << "assigned = " << assigned;
            }
        }

        if (assigned == 0U) {
            diagChannel->SendDiagNack(isoft::doip::DiagNackType::kReceiveBufferOverflow, req.targetAddress_,  // NOLINT
                                      req.sourceAddress_);                                                    // NOLINT
            return;
        }

        diagChannel->SendDiagAck(req.targetAddress_, req.sourceAddress_);  // NOLINT
        for (decltype(auto) server : serverList) {
            if (server.get() == nullptr) {
                common::LogError() << "DoIPServer::_dealDiagMessage|functional server is nullptr";
                continue;
            }

            server->HandleMessage(uds, priorty, diagChannel);
            common::LogDebug() << "DoIPServer::_dealDiagMessage|functional transfer uds to dcm finished uds_size ="
                               << uds->GetBody().size();
        }
    }
}

/// @brief Get vehicle announcement
/// @param[in] cb Callback notification for getting the complete announcement
void DoIPServer::_getVehicleId(std::function< void(isoft::doip::VehicleIdResponse&) > const& cb) noexcept
{
    isoft::ThreadPool::GetInstance().Submit([this, cb]() {
        isoft::doip::VehicleIdResponse response;
        response.serverLogicalAddress_ = DoIPConfig::GetConfig().LogicalAddress();  // NOLINT
        /// Get GID
        if (doipGroupIdHandlerPtr_.get() == nullptr) {
            common::LogWarn() << "DoIPServer::_getVehicleId|doipGroupIdHandlerPtr_ is nullptr";
            if (!DoIPConfig::GetConfig().Gid(response.gid_)) {  // NOLINT
                response.gid_.fill(0U);                         // NOLINT
            }
        } else {
            ara::core::Future< isoft::dm::GidStatus > gidRes = doipGroupIdHandlerPtr_->GetGidStatus();
            ara::core::future_status const futureStatus{gidRes.wait_for(std::chrono::milliseconds(2000))};
            do {
                if (futureStatus != ara::core::future_status::ready) {
                    if (!DoIPConfig::GetConfig().Gid(response.gid_)) {  // NOLINT
                        response.gid_.fill(0U);                         // NOLINT
                    }
                    common::LogWarn() << "DoIPServer::_getVehicleId|read gid timeout";
                    break;
                }

                ara::core::Result< isoft::dm::GidStatus > result = gidRes.GetResult();
                if (!result.HasValue()) {
                    if (!DoIPConfig::GetConfig().Gid(response.gid_)) {  // NOLINT
                        response.gid_.fill(0U);                         // NOLINT
                    }
                    common::LogWarn() << "DoIPServer::_getVehicleId|read gid error" << result.Error().Message();
                    break;
                }

                response.gid_           = result.Value().groupIdentification;    // NOLINT
                response.furtherAction_ = result.Value().furtherActionRequired;  // NOLINT
                response.vinGidStatus_  = result.Value().syncStatus;             // NOLINT
            } while (false);
        }

        isoft::dm::MetaInfoMap metaInfo;
        ara::core::String const strMetaInfoContext{"kContext"};
        metaInfo[strMetaInfoContext] = "2";
        ara::diag::doip::VinInfo const vinInfo{DoIPConfig::GetConfig().Vin()};
        static std::shared_ptr< isoft::dm::CancellationEvent > s_Cancellation_Event_Ptr
            = std::make_shared< isoft::dm::CancellationEvent >();
        /// Get VIN
        if (genericVin_.get() != nullptr) {
            ara::core::Future< ara::core::Vector< std::uint8_t > > res
                = genericVin_->Read(vinInfo.dataIdentifier, metaInfo, s_Cancellation_Event_Ptr);
            ara::core::future_status const futureStatus{res.wait_for(std::chrono::milliseconds(2000))};
            do {
                if (futureStatus != ara::core::future_status::ready) {
                    response.vin_.fill(DoIPConfig::GetConfig().VinInvalidityPattern());
                    common::LogWarn() << "DoIPServer::_getVehicleId|1 read vin timeout";
                    break;
                }

                ara::core::Result< ara::core::Vector< std::uint8_t > > result = res.GetResult();
                if (!result.HasValue()) {
                    response.vin_.fill(DoIPConfig::GetConfig().VinInvalidityPattern());  // NOLINT
                    common::LogWarn() << "DoIPServer::_getVehicleId|1 read vin error" << result.Error().Message();
                    break;
                }

                for (size_t i{0U}; i < result.Value().size(); i++) {
                    response.vin_[i] = result.Value()[i];  // NOLINT
                }
            } while (false);
        } else if (rawVin_.get() != nullptr) {
            ara::core::Future< ara::core::Vector< std::uint8_t > > res
                = rawVin_->Read(metaInfo, s_Cancellation_Event_Ptr);
            ara::core::future_status const futureStatus{res.wait_for(std::chrono::milliseconds(2000))};
            do {
                if (futureStatus != ara::core::future_status::ready) {
                    response.vin_.fill(DoIPConfig::GetConfig().VinInvalidityPattern());  // NOLINT
                    common::LogWarn() << "DoIPServer::_getVehicleId|2 read vin timeout";
                    break;
                }

                ara::core::Result< ara::core::Vector< std::uint8_t > > result = res.GetResult();
                if (!result.HasValue()) {
                    response.vin_.fill(DoIPConfig::GetConfig().VinInvalidityPattern());  // NOLINT
                    common::LogWarn() << "DoIPServer::_getVehicleId|2 read vin error" << result.Error().Message();
                    break;
                }

                for (size_t i{0U}; i < result.Value().size(); i++) {
                    response.vin_[i] = result.Value()[i];  // NOLINT
                }
            } while (false);
        } else {
            response.vin_.fill(DoIPConfig::GetConfig().VinInvalidityPattern());  // NOLINT
            common::LogWarn() << "DoIPServer::_getVehicleId|no interface can read vin";
        }

        cb(response);
    });
}

/// @brief Get EID
/// @param[in] networkId Network port ID
/// @return Result
isoft::doip::EidValue DoIPServer::_getEid(uint8_t const networkId) noexcept
{
    isoft::doip::EidValue eid{};
    bool const isUseMac{DoIPConfig::GetConfig().EidUseMac(networkId)};
    {
        ara::core::Map< uint8_t, std::shared_ptr< isoft::doip::Server > >::iterator it{mapDoipServer_.find(networkId)};
        bool const isFindSuccess{it != mapDoipServer_.end()};
        if (isUseMac && isFindSuccess) {
            eid = it->second->GetMacAddress();
        } else {
            bool const isConfigEid{DoIPConfig::GetConfig().Eid(eid)};
            if (!isConfigEid && isFindSuccess) {
                eid = it->second->GetMacAddress();
            }
        }
    }
    return eid;
}

/// @brief Find request type by receiver logical address
/// @param[in] ta Receiver logical address
/// @return kPhysical , kFunctional
/// @throw unknown
isoft::uds::server::TargetAddressType DoIPServer::_findUdsAddressType(uint16_t const ta)
{
    isoft::uds::server::TargetAddressType type{isoft::uds::server::TargetAddressType::kPhysical};
    RequestConfigVectorPtr requestConfigVectorPtr{DoIPConfig::GetConfig().RequestConfig()};
    if (requestConfigVectorPtr.get() == nullptr) {
        common::LogError()
            << "DoIPServer::_findUdsAddressType|_findUdsAddressType request_config_vector_ptr is null | ta =" << ta;
        return type;
    }
    for (core::Vector< RequestConfiguration >::const_iterator iter{requestConfigVectorPtr->cbegin()};
         iter != requestConfigVectorPtr->cend(); ++iter) {
        RequestConfiguration const requestConfig{*iter};
        if ((ta < requestConfig.startAddress) || (ta > requestConfig.endAddress)) {
            continue;
        }

        switch (requestConfig.requestType) {
            case RequestTypeEnum::kPhysical: {
                type = isoft::uds::server::TargetAddressType::kPhysical;
                break;
            }
            case RequestTypeEnum::kFunctional: {
                type = isoft::uds::server::TargetAddressType::kFunctional;
                break;
            }
        }
        break;
    }
    return type;
}

/// @brief Get priority according to diagnostic SA
/// @param[in] sa
/// @return 0-3 Three levels agreed with DCM  0: Highest priority  3: Lowest priority
uint8_t DoIPServer::_getAddressPriority(uint16_t sa)
{
    if (sa >= isoft::serialize::kInt16_0x0E00U && sa <= isoft::serialize::kInt16_0x0E7FU) {
        return 0U;
    }
    if (sa >= isoft::serialize::kInt16_0x0E80U && sa <= isoft::serialize::kInt16_0x0EFFU) {
        return 1U;
    }
    if (sa >= isoft::serialize::kInt16_0x0F00U && sa <= isoft::serialize::kInt16_0x0F7FU) {
        return 2U;
    }
    return 3U;
}

/// @brief Create interface for reading VIN
void DoIPServer::_createReadVinInterface()
{
    ara::diag::doip::VinInfo const vinInfo{DoIPConfig::GetConfig().Vin()};
    if (vinInfo.dataIdentifier == 0U) {
        common::LogInfo() << "DoIPServer::_createReadVinInterface|no config vin";
        return;
    }

    // if (vinInfo.diagnosticInterfaceClass == "DIAGNOSTIC-DATA-IDENTIFIER-GENERIC-INTERFACE") {
    //     genericVin_ = std::make_shared< isoft::dm::dic::GenericDataIdentifierAgent >(vinInfo.instanceId,
    //                                                                                  vinInfo.serviceInstanceId);
    // } else if (vinInfo.diagnosticInterfaceClass == "DIAGNOSTIC-DATA-IDENTIFIER-INTERFACE") {
    //     rawVin_
    //         = std::make_shared< isoft::dm::dic::RawDataIdentifierAgent >(vinInfo.instanceId, vinInfo.serviceInstanceId);
    // } else {
    //     common::LogError() << "DoIPServer::_createReadVinInterface|invalid interface class "
    //                        << vinInfo.diagnosticInterfaceClass.c_str();
    //     return;
    // }
    common::LogVerbose() << "DoIPServer::_createReadVinInterface|create success vin =" << vinInfo.dataIdentifier
                         << "interface_class =" << vinInfo.diagnosticInterfaceClass.c_str()
                         << "instance_id =" << vinInfo.instanceId << "service_id =" << vinInfo.serviceInstanceId;
}

/// @brief Broadcast announcement encapsulation
/// @param networkId Network port
void DoIPServer::_broadcastAnnouncement(uint8_t const networkId)
{
    _getVehicleId([this, networkId](isoft::doip::VehicleIdResponse& res) {
        ara::core::Map< uint8_t, std::shared_ptr< isoft::doip::Server > >::iterator iter{
            mapDoipServer_.find(networkId)};
        if (iter == mapDoipServer_.end()) {
            common::LogError() << "DoIPServer::Initialize|trigger not find doip server networkId=" << networkId;
            return;
        }

        if (iter->second.get() == nullptr) {
            common::LogError() << "DoIPServer::Initialize|trigger doip server is nullptr networkId=" << networkId;
            return;
        }

        isoft::doip::EidValue eid{_getEid(networkId)};
        for (size_t i{0U}; i < eid.size(); i++) {
            res.eid_[i] = eid[i];  // NOLINT
        }
        ara::core::Vector< uint8_t > body;
        res.Serialize(body);
        iter->second->BroadcastAnnouncement(body);
        common::LogVerbose() << "DoIPServer::Initialize|trigger broadcast announcement networkId =" << networkId;
    });
}

}  // namespace doip
}  // namespace diag
}  // namespace ara