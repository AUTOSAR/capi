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
/// @file       doip_config.cpp
/// @brief      This file provides the implementation of the DoIP configuration management class
/// @details
/// @date       2022-07-24
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================

#include "doip_config.h"

#include <isoft/ara_fsh/process.h>
#include <isoft/manifestreader/manifest_reader.h>
#ifdef NMODEL
    #include <isoft/nml/definition/doip_port.h>
    #include <isoft/nml/definition/ethernet_communication_connector.h>
    #include <isoft/nml/definition/ipv4_configuration.h>
    #include <isoft/nml/definition/ipv6_configuration.h>
    #include <isoft/nml/definition/network_endpoint.h>

    #include "hash/hash.h"
#endif
#include "ara/diag/diag_error_domain.h"
#include "log/log.h"
#include "serialization/serialization.h"
namespace ara {
namespace diag {
namespace doip {
#ifdef NMODEL
std::string Trim(const std::string& s)
{
    auto start = s.begin();
    while (start != s.end() && std::isspace(static_cast< unsigned char >(*start)))
        ++start;
    auto end = s.end();
    do {
        --end;
    } while (start < end && std::isspace(static_cast< unsigned char >(*end)));
    return (start < end + 1) ? std::string(start, end + 1) : std::string();
}

std::vector< std::string > SplitString(const std::string& s)
{
    std::vector< std::string > tokens;
    std::string token;
    std::istringstream tokenStream(s);
    char delimiter = ',';
    while (std::getline(tokenStream, token, delimiter)) {
        std::string trimmedToken = Trim(token);
        if (!trimmedToken.empty())
            tokens.push_back(trimmedToken);
    }
    return tokens;
}
std::uint16_t ToUint16(std::string& value)
{
    std::istringstream iss(value);
    std::uint16_t val;
    if (iss >> val && iss.eof()) {
        return val;
    }
    return {};
}
ara::core::Optional< ara::core::Array< std::uint8_t, 6U > > ToRawData(const std::string& s)
{
    std::vector< std::string > sList = SplitString(s);
    ara::core::Array< std::uint8_t, 6U > result{};

    std::size_t idx{0U};
    for (auto&& raw : sList) {
        std::istringstream iss(raw);
        std::uint8_t val;
        if (iss >> val && iss.eof()) {
            result[idx] = val;
            idx++;
        }
    }

    return {result};
}

/// @brief Get Port unique identifier
/// @param tag
/// @param key
/// @return
std::vector< std::string > GetPortFqn(
    std::set< std::shared_ptr< isoft::nml::definition::AdaptiveSoftwareComponent > > adaptiveSoftwareComponentTable,
    isoft::nml::definition::DoipInterfaceEnum type) noexcept
{
    std::vector< std::string > fqnTable{};
    for (auto&& adaptiveSoftwareComponentPtr : adaptiveSoftwareComponentTable) {
        if (adaptiveSoftwareComponentPtr.get() == nullptr) {
            continue;
        }
        for (auto&& portTypePtr : adaptiveSoftwareComponentPtr->port) {
            if (portTypePtr.get() == nullptr) {
                continue;
            }
            if (portTypePtr->tag == isoft::nml::ModelTag::kDoipPort) {
                if (portTypePtr->doipPort.get() == nullptr) {
                    continue;
                }
                if (portTypePtr->doipPort->interface.get() == nullptr) {
                    continue;
                }
                if (type == *portTypePtr->doipPort->interface) {
                    fqnTable.emplace_back(portTypePtr->doipPort->isoftNmlObjectFqn);
                }
            }
        }
        if (!adaptiveSoftwareComponentPtr->softwareComponent.empty()) {
            std::vector< std::string > subFqnTable{};
            std::set< std::shared_ptr< isoft::nml::definition::AdaptiveSoftwareComponent > >
                childSoftwareComponmentList{adaptiveSoftwareComponentPtr->softwareComponent.begin(),
                                            adaptiveSoftwareComponentPtr->softwareComponent.end()};
            subFqnTable = GetPortFqn(childSoftwareComponmentList, type);
            if (subFqnTable.empty()) {
                continue;
            }
            fqnTable.insert(fqnTable.end(), subFqnTable.begin(), subFqnTable.end());
        }
    }
    return fqnTable;
}
#endif
/// @brief Singleton instantiation
/// @return DoIPConfig&
/// @throw unknown
DoIPConfig& DoIPConfig::GetConfig()
{
    static DoIPConfig s_Config;
    return s_Config;
}

#ifdef NMODEL
bool DoIPConfig::Initialize(
    std::set< std::shared_ptr< isoft::nml::definition::AdaptiveSoftwareComponent > > adaptiveSoftwareComponentTable,
    std::shared_ptr< isoft::nml::definition::DoipModule > doipModulePtr)
{
    if (doipModulePtr.get() == nullptr) {
        common::LogWarn() << "DoIPConfig::Initialize| not doip config!";
        return false;
    }
    useNmodel_ = true;
    if (doipModulePtr->priority.get() != nullptr) {
        priority_     = *doipModulePtr->priority;
        protocolKind_ = "doip";
    }
    if (doipModulePtr->entityStatMaxByteFieldUse.get() != nullptr) {
        entityStatusMaxByteFieldUse_ = *doipModulePtr->entityStatMaxByteFieldUse;
    }
    if (doipModulePtr->eid.get() != nullptr) {
        optEid_ = ToRawData(*doipModulePtr->eid);
    }
    if (doipModulePtr->gid.get() != nullptr) {
        optGid_ = ToRawData(*doipModulePtr->gid);
    }
    if (doipModulePtr->gidInvalidityPattern.get() != nullptr) {
        gidInvalidityPattern_ = ToUint16(*doipModulePtr->gidInvalidityPattern);
    }
    if (doipModulePtr->logicalAddress.get() != nullptr) {
        logicalAddress_ = *doipModulePtr->logicalAddress;
    }
    if (doipModulePtr->maxRequestBytes.get() != nullptr) {
        maxRequestBytes_ = *doipModulePtr->maxRequestBytes;
    }
    if (doipModulePtr->vinInvalidityPattern.get() != nullptr) {
        vinInvalidityPattern_ = ToUint16(*doipModulePtr->vinInvalidityPattern);
    }
    std::ignore = _initializeDoipNetworkConfiguration(doipModulePtr->networkInterface);
    std::ignore = _initializeDoipRequestConfiguration(doipModulePtr->requestConfiguration);
    std::ignore = _initializeActivationInterface(adaptiveSoftwareComponentTable, doipModulePtr->isoftNmlObjectFqn);

    std::ignore
        = _initializeGroupIdentificationInterface(adaptiveSoftwareComponentTable, doipModulePtr->isoftNmlObjectFqn);

    std::ignore = _initializePowerModeInterface(adaptiveSoftwareComponentTable, doipModulePtr->isoftNmlObjectFqn);

    std::ignore = _initializeTriggerVehicleAnnouncementInterface(adaptiveSoftwareComponentTable,
                                                                 doipModulePtr->isoftNmlObjectFqn);
    return true;
}
bool DoIPConfig::_initializeDoipNetworkConfiguration(
    std::vector< std::shared_ptr< isoft::nml::definition::DoipNetworkConfiguration > > doipNetworkConfigurationTable)
{
    for (auto&& doipNetworkConfigurationPtr : doipNetworkConfigurationTable) {
        if (doipNetworkConfigurationPtr.get() == nullptr) {
            continue;
        }
        DoIpNetworkConfiguration config{};
        if (doipNetworkConfigurationPtr->networkInterfaceId.get() != nullptr) {
            config.networkInterfaceId = *doipNetworkConfigurationPtr->networkInterfaceId;
        }
        if (doipNetworkConfigurationPtr->eidRetrieval.get() != nullptr) {
            if (isoft::nml::definition::DoipEidRetrievalEnum::kEidUseMac
                == *doipNetworkConfigurationPtr->eidRetrieval) {
                config.eidUseMac = true;
            }
        }
        if (doipNetworkConfigurationPtr->isActivationLineDependent.get() != nullptr) {
            config.isActivationLineDependent = *doipNetworkConfigurationPtr->isActivationLineDependent;
        }
        if (doipNetworkConfigurationPtr->maxAnnounceWaitTime.get() != nullptr) {
            config.maxInitialVehicleAnnouncementTime = *doipNetworkConfigurationPtr->maxAnnounceWaitTime;
        }
        if (doipNetworkConfigurationPtr->maxTesterConnections.get() != nullptr) {
            config.maxTesterConnections = *doipNetworkConfigurationPtr->maxTesterConnections;
        }
        if (doipNetworkConfigurationPtr->tcpAliveCheckTimeout.get() != nullptr) {
            config.tcpAliveCheckResponseTimeout = *doipNetworkConfigurationPtr->tcpAliveCheckTimeout;
        }
        if (doipNetworkConfigurationPtr->tcpGeneralInactivityTime.get() != nullptr) {
            config.tcpGeneralInactivityTime = *doipNetworkConfigurationPtr->tcpGeneralInactivityTime;
        }
        if (doipNetworkConfigurationPtr->tcpInitialInactivityTime.get() != nullptr) {
            config.tcpInitialInactivityTime = *doipNetworkConfigurationPtr->tcpInitialInactivityTime;
        }
        if (doipNetworkConfigurationPtr->vehicleAnnouncementCount.get() != nullptr) {
            config.vehicleAnnouncementCount = *doipNetworkConfigurationPtr->vehicleAnnouncementCount;
        }
        if (doipNetworkConfigurationPtr->vehicleAnnounceInterval.get() != nullptr) {
            config.vehicleAnnouncementInterval = *doipNetworkConfigurationPtr->vehicleAnnounceInterval;
        }
        if (doipNetworkConfigurationPtr->vinSyncStatus.get() != nullptr) {
            config.vehicleIdentificationSyncStatus = *doipNetworkConfigurationPtr->vinSyncStatus;
        }
        if (doipNetworkConfigurationPtr->udpPort.get() != nullptr) {
            config.udpPort_ = *doipNetworkConfigurationPtr->udpPort;
        }
        if (doipNetworkConfigurationPtr->tcpPort.get() != nullptr) {
            config.tcpPort = *doipNetworkConfigurationPtr->tcpPort;
        }
        if (!doipNetworkConfigurationPtr->connectorRef.expired()) {
            std::shared_ptr< isoft::nml::definition::NetworkEndpoint > unicastNetworkEndpointPtr{
                doipNetworkConfigurationPtr->connectorRef.lock()};
            if (unicastNetworkEndpointPtr.get() != nullptr) {
                if (unicastNetworkEndpointPtr->ipv4.get() != nullptr) {
                    std::shared_ptr< isoft::nml::definition::Ipv4Configuration > Ipv4Ptr{
                        unicastNetworkEndpointPtr->ipv4};
                    if (Ipv4Ptr->address.get() != nullptr) {
                        config.ipv4Address = *Ipv4Ptr->address;
                    }
                    if (Ipv4Ptr->mask.get() != nullptr) {
                        config.networkMask = *Ipv4Ptr->mask;
                    }
                }
            }
        }
        mapDoipNetworkConfig_.insert({config.networkInterfaceId, config});
    }
    return true;
}
bool DoIPConfig::_initializeDoipRequestConfiguration(
    std::vector< std::shared_ptr< isoft::nml::definition::DoipRequestConfiguration > > doipRequestConfigurationTable)
{
    requestConfigPtr_ = std::make_shared< RequestConfigVector >();
    for (auto&& doipRequestConfiguration : doipRequestConfigurationTable) {
        RequestConfiguration config{};
        if (doipRequestConfiguration->startAddress.get() != nullptr) {
            config.startAddress = *doipRequestConfiguration->startAddress;
        }
        if (doipRequestConfiguration->endAddress.get() != nullptr) {
            config.endAddress = *doipRequestConfiguration->endAddress;
        }
        if (doipRequestConfiguration->requestType.get() != nullptr) {
            switch (*doipRequestConfiguration->requestType) {
                case isoft::nml::definition::RequestTypeEnum::kFunctional:
                    config.requestType = RequestTypeEnum::kFunctional;
                    break;
                case isoft::nml::definition::RequestTypeEnum::kPhysical:
                    config.requestType = RequestTypeEnum::kPhysical;
                    break;
                default:
                    break;
            }
        }
        requestConfigPtr_->emplace_back(config);
    }
    return true;
}

bool DoIPConfig::_initializeActivationInterface(
    std::set< std::shared_ptr< isoft::nml::definition::AdaptiveSoftwareComponent > > adaptiveSoftwareComponentTable,
    std::string doipModuleFqn) noexcept
{
    std::vector< std::string > portTable
        = GetPortFqn(adaptiveSoftwareComponentTable,
                     isoft::nml::definition::DoipInterfaceEnum::kDiagnosticDoIpActivationLineInterface);
    if (portTable.empty()) {
        common::LogInfo() << "DoIPConfig::_initializeActivationInterface| not config activation interface!";
        return true;
    }
    ActivationLineInfo info{};
    info.serviceInstanceId = Hash32(portTable.front());
    info.instanceId        = Hash16(doipModuleFqn);
    vecActivationLine_.emplace_back(info);
    return true;
}

bool DoIPConfig::_initializeGroupIdentificationInterface(
    std::set< std::shared_ptr< isoft::nml::definition::AdaptiveSoftwareComponent > > adaptiveSoftwareComponentTable,
    std::string doipModuleFqn) noexcept
{
    std::vector< std::string > portTable
        = GetPortFqn(adaptiveSoftwareComponentTable,
                     isoft::nml::definition::DoipInterfaceEnum::kDiagnosticDoIpGroupIdentificationInterface);
    if (portTable.empty()) {
        common::LogInfo() << "DoIPConfig::_initializeActivationInterface| not config group id interface!";
        return true;
    }
    groupInfo_.serviceInstanceId = Hash32(portTable.front());
    groupInfo_.instanceId        = Hash16(doipModuleFqn);
    return true;
}

bool DoIPConfig::_initializePowerModeInterface(
    std::set< std::shared_ptr< isoft::nml::definition::AdaptiveSoftwareComponent > > adaptiveSoftwareComponentTable,
    std::string doipModuleFqn) noexcept
{
    std::vector< std::string > portTable = GetPortFqn(
        adaptiveSoftwareComponentTable, isoft::nml::definition::DoipInterfaceEnum::kDiagnosticDoIpPowerModeInterface);
    if (portTable.empty()) {
        common::LogInfo() << "DoIPConfig::_initializeActivationInterface| not config power mode interface!";
        return true;
    }
    powerInfo_.serviceInstanceId = Hash32(portTable.front());
    powerInfo_.instanceId        = Hash16(doipModuleFqn);
    return true;
}

bool DoIPConfig::_initializeTriggerVehicleAnnouncementInterface(
    std::set< std::shared_ptr< isoft::nml::definition::AdaptiveSoftwareComponent > > adaptiveSoftwareComponentTable,
    std::string doipModuleFqn) noexcept
{
    std::vector< std::string > portTable
        = GetPortFqn(adaptiveSoftwareComponentTable,
                     isoft::nml::definition::DoipInterfaceEnum::kDiagnosticDoIpTriggerVehicleAnnouncementInterface);
    if (portTable.empty()) {
        common::LogInfo()
            << "DoIPConfig::_initializeActivationInterface| not config trigger vehicle announcement interface!";
        return true;
    }
    std::string const defaultServiceName{".TriggerVehicleAnnouncement"};
    std::string serviceInstance{doipModuleFqn};
    serviceInstance += defaultServiceName;

    triggerVehicleAnnouncementServiceInstanceId_ = Hash32(serviceInstance);
    return true;
}
#endif
/// @brief Initialize and parse the DoIP.json file
/// @return true: Success false: Failure
/// @throw unknown
bool DoIPConfig::Init()
{
    isoft::ara_fsh::Process const process;
    ara::core::String const etcPath{process.GetEtcDir()};
    ara::core::String const path{etcPath + "/doip.json"};
    ara::core::Result< std::unique_ptr< isoft::manifestreader::Manifest > > const result{
        isoft::manifestreader::OpenManifest(std::move(ara::core::StringView(path.c_str())))};

    if (!result.HasValue()) {
        common::LogError() << "DoIPConfig::Init|open manifest no value error:" << std::move(result.Error().Message());
        return false;
    }

    isoft::manifestreader::Manifest* const readerPtr{result.Value().get()};
    if (readerPtr == nullptr) {
        common::LogError() << "DoIPConfig::Init|readerPtr is nullptr";
        return false;
    }
    std::ignore
        = readerPtr->Load(std::move(ara::core::StringView(
                              "interface.DIAGNOSTIC-DO-IP-TRIGGER-VEHICLE-ANNOUNCEMENT-INTERFACE.serviceInstanceId")),
                          triggerVehicleAnnouncementServiceInstanceId_);

    std::ignore = readerPtr->Load(
        std::move(ara::core::StringView("interface.DIAGNOSTIC-DO-IP-GROUP-IDENTIFICATION-INTERFACE.serviceInstanceId")),
        groupInfo_.serviceInstanceId);
    std::ignore = readerPtr->Load(
        std::move(ara::core::StringView("interface.DIAGNOSTIC-DO-IP-GROUP-IDENTIFICATION-INTERFACE.clientInstanceId")),
        groupInfo_.instanceId);

    std::ignore = readerPtr->Load(
        std::move(ara::core::StringView("interface.DIAGNOSTIC-DO-IP-POWER-MODE-INTERFACE.serviceInstanceId")),
        powerInfo_.serviceInstanceId);
    std::ignore = readerPtr->Load(
        std::move(ara::core::StringView("interface.DIAGNOSTIC-DO-IP-POWER-MODE-INTERFACE.serviceInstanceId")),
        powerInfo_.instanceId);

    std::ignore = readerPtr->Load(std::move(ara::core::StringView("vin.dataIdentifier")), vinInfo_.dataIdentifier);
    std::ignore = readerPtr->Load(std::move(ara::core::StringView("vin.diagnosticInstanceIdentifier")),
                                  vinInfo_.diagnosticInstanceIdentifier);
    std::ignore = readerPtr->Load(std::move(ara::core::StringView("vin.diagnosticInterfaceClass")),
                                  vinInfo_.diagnosticInterfaceClass);
    std::ignore
        = readerPtr->Load(std::move(ara::core::StringView("vin.serviceInstanceId")), vinInfo_.serviceInstanceId);
    std::ignore = readerPtr->Load(std::move(ara::core::StringView("vin.clientInstanceId")), vinInfo_.instanceId);

    std::ignore = readerPtr->IterateArray(
        std::move(ara::core::StringView("interface.DIAGNOSTIC-DO-IP-ACTIVATION-LINE-INTERFACE")),
        [this](std::size_t const idx, isoft::manifestreader::ManifestNode const& node) {
            std::ignore = idx;
            ActivationLineInfo config;
            std::ignore = node.Load(std::move(ara::core::StringView("serviceInstanceId")), config.serviceInstanceId);
            std::ignore = node.Load(std::move(ara::core::StringView("clientInstanceId")), config.instanceId);
            vecActivationLine_.emplace_back(config);
        });

    std::ignore = readerPtr->Load(std::move(ara::core::StringView("protocol.priority")), priority_);
    std::ignore = readerPtr->Load(std::move(ara::core::StringView("protocol.protocolKind")), protocolKind_);
    _initDoIpInstantiation(readerPtr);

    std::ignore = readerPtr->IterateArray(
        std::move(ara::core::StringView("networkConfiguration")),
        [this](std::size_t const idx, isoft::manifestreader::ManifestNode const& node) {
            std::ignore = idx;
            PlatformModuleEthernetEndpointConfiguration config{};
            std::ignore = node.Load(std::move(ara::core::StringView("id")), config.id);
            std::ignore
                = node.Load(std::move(ara::core::StringView("communicationConnector")), config.communicationConnector);
            std::ignore = node.Load(std::move(ara::core::StringView("tcpPort")), config.tcpPort);
            std::ignore = node.Load(std::move(ara::core::StringView("udpPort")), config.udpPort_);  // NOLINT
            mapNetworkConfig_[config.id] = config;
        });
    std::ignore = readerPtr->IterateArray(
        std::move(ara::core::StringView("communicationConnector")),
        [this](std::size_t const idx, isoft::manifestreader::ManifestNode const& node) {
            std::ignore = idx;
            EthernetCommunicationConnector connector{};
            std::ignore = node.Load(std::move(ara::core::StringView("id")), connector.id);
            std::ignore = node.Load(std::move(ara::core::StringView("unicastNetworkEndpoint")),
                                    connector.unicastNetworkEndpoint);
            mapCommunicationConnector_[connector.id] = connector;
        });
    std::ignore = readerPtr->IterateArray(
        std::move(ara::core::StringView("unicastNetworkEndpoint")),
        [this](std::size_t const idx, isoft::manifestreader::ManifestNode const& node) {
            std::ignore = idx;
            NetworkEndpoint endpoint;
            std::ignore = node.Load(std::move(ara::core::StringView("id")), endpoint.id);
            std::ignore = node.IterateArray(
                std::move(ara::core::StringView("networkEndpointAddress")),
                [this, &endpoint](std::size_t const networkIndex,
                                  isoft::manifestreader::ManifestNode const& networkNode) {
                    std::ignore = networkIndex;
                    NetworkEndpointAddress address;
                    std::int32_t networkResult{networkNode.Load(
                        std::move(ara::core::StringView("Ipv4Configuration.ipv4Address")), address.ipv4.ipv4Address)};
                    if (networkResult == 0) {
                        address.type = IpType::kIpv4;
                        core::String strSource;
                        std::ignore = networkNode.Load(
                            std::move(ara::core::StringView("Ipv4Configuration.ipv4AddressSource")), strSource);
                        ara::diag::doip::Ipv4AddressSource source{ara::diag::doip::Ipv4AddressSource::kAutoIp};
                        if (strSource == "FIXED") {
                            source = ara::diag::doip::Ipv4AddressSource::kFixed;
                        }
                        address.ipv4.ipv4AddressSource = source;
                        std::ignore
                            = networkNode.Load(std::move(ara::core::StringView("Ipv4Configuration.networkMask")),
                                               address.ipv4.networkMask);
                    } else {
                        networkResult
                            = networkNode.Load(std::move(ara::core::StringView("Ipv6Configuration.ipv6Address")),
                                               address.ipv6.ipv6Address);
                        if (networkResult == 0) {
                            address.type = IpType::kIpv6;
                        }
                        std::ignore = networkNode.Load(
                            std::move(ara::core::StringView("Ipv6Configuration.ipAddressPrefixLength")),
                            address.ipv6.ipAddressPrefixLength);
                        std::int32_t source{};
                        std::ignore = networkNode.Load(
                            std::move(ara::core::StringView("Ipv6Configuration.ipv6AddressSource")), source);
                        std::ignore = source;
                        address.ipv6.ipv6AddressSource
                            = static_cast< Ipv6AddressSource >(address.ipv6.ipv6AddressSource);
                    }
                    endpoint.address                        = address;
                    mapUnicastNetworkEndpoint_[endpoint.id] = endpoint;
                });
            mapUnicastNetworkEndpoint_[endpoint.id] = endpoint;
        });

    return true;
}

/// @brief Get TCP port number according to the identifier of DoIPInterface
/// @param[in] networkInterfaceId Identifier of DoIPInterface
/// @return TCP port number
/// @throw unknown
uint16_t DoIPConfig::TcpPort(uint8_t const networkInterfaceId)
{
#ifdef NMODEL
    if (useNmodel_) {
        return mapDoipNetworkConfig_[networkInterfaceId].tcpPort;
    } else {
#endif
        uint32_t const no{mapDoipNetworkConfig_[networkInterfaceId].networkConfiguration};
        return mapNetworkConfig_[no].tcpPort;
#ifdef NMODEL
    }
#endif
}

/// @brief Get maximum connections according to the identifier of DoIPInterface
/// @param[in] networkInterfaceId Identifier of DoIPInterface
/// @return Maximum connections
/// @throw unknown
uint32_t DoIPConfig::MaxTesterConnections(uint8_t const networkInterfaceId)
{
    return mapDoipNetworkConfig_[networkInterfaceId].maxTesterConnections;
}

/// @brief Get DoIP server configuration
/// @param[in] networkInterfaceId Network port ID
/// @param[out] config Configuration
bool DoIPConfig::GetDoipServerConfig(uint8_t const networkInterfaceId, isoft::doip::ServerConfig& config)
{
    auto itetNetworkConfig = mapDoipNetworkConfig_.find(networkInterfaceId);
    if (itetNetworkConfig == mapDoipNetworkConfig_.end()) {
        return false;
    }
    config.logicAdress_ = LogicalAddress();  // NOLINT
#ifdef NMODEL
    if (useNmodel_) {
        config.serverIP_   = itetNetworkConfig->second.ipv4Address;  // NOLINT
        config.networkMask = itetNetworkConfig->second.networkMask;
    } else {
#endif
        config.serverIP_   = IpAddress(networkInterfaceId).ipv4.ipv4Address;  // NOLINT
        config.networkMask = IpAddress(networkInterfaceId).ipv4.networkMask;
#ifdef NMODEL
    }
#endif
    config.tcpPort_             = TcpPort(networkInterfaceId);  // NOLINT
    config.udpPort_             = UdpPort(networkInterfaceId);  // NOLINT
    config.testerMaxConnections = MaxTesterConnections(networkInterfaceId);
    config.requestMaxBytes      = MaxRequestBytes();
    config.aliveCheckTimer = static_cast< uint32_t >(TcpAliveCheckResponseTimeout(networkInterfaceId) * MSTOSCONVERT);
    config.generalInactivityTimer   = static_cast< uint32_t >(TcpGeneralInactivityTime(networkInterfaceId));
    config.initialInactivityTimer   = static_cast< uint32_t >(TcpInitialInactivityTime(networkInterfaceId));
    config.vehicleAnnouncementCount = VehicleAnnouncementCount(networkInterfaceId);
    config.vehicleAnnouncementInterval
        = static_cast< uint32_t >(VehicleAnnouncementInterval(networkInterfaceId) * MSTOSCONVERT);
    config.vehicleAnnouncementWait
        = static_cast< uint32_t >(MaxInitialVehicleAnnouncementTime(networkInterfaceId) * MSTOSCONVERT);

    config.doipProtocolVersion_ = doipVersion_;
    config.isUseTls             = !serverCrt_.empty();
    if (config.isUseTls) {
        config.serverCrt = serverCrt_;
        config.serverKey = serverKey_;
        config.ca        = ca_;
    }

    // for (ara::core::Vector< RequestConfiguration >::const_iterator iter{std::move(requestConfigPtr_->cbegin())};
    //      iter != requestConfigPtr_->cend(); ++iter) {
    //     RequestConfiguration const request_config{*iter};
    //     isoft::doip::DiagAddressRadius diagAddress;
    //     diagAddress.startAddress = request_config.startAddress;
    //     diagAddress.endAddress   = request_config.endAddress;
    //     config.diagAddressRadius_.emplace_back(diagAddress);
    // }
    return true;
}

/// @brief Parse information related to DoIpInstantiation in the configuration file
/// @param[in] manifest File parser
/// @throw unknown
void DoIPConfig::_initDoIpInstantiation(isoft::manifestreader::Manifest* const manifest)
{
    core::Vector< uint16_t > vecValue;
    std::int32_t loadResult{manifest->Load(std::move(ara::core::StringView("DoIpInstantiation.eid")), vecValue)};
    if ((isoft::kSuccess == loadResult) && (vecValue.size() == isoft::doip::kEidValueLength)) {
        isoft::doip::EidValue eid{};
        size_t const eSize{eid.size()};
        for (size_t i{0U}; i < eSize; i++) {
            eid[i] = static_cast< uint8_t >(vecValue[i]);
        }
        optEid_ = eid;
    }

    vecValue.clear();
    std::ignore = manifest->Load(std::move(ara::core::StringView("DoIpInstantiation.entityStatusMaxByteFieldUse")),
                                 entityStatusMaxByteFieldUse_);
    loadResult  = manifest->Load(std::move(ara::core::StringView("DoIpInstantiation.gid")), vecValue);
    if ((isoft::kSuccess == loadResult) && (vecValue.size() == isoft::doip::kGidValueLength)) {
        isoft::doip::GidValue gid{};
        size_t const gSize{gid.size()};
        for (size_t i{0U}; i < gSize; i++) {
            gid[i] = static_cast< uint8_t >(vecValue[i]);
        }
        optGid_ = gid;
    }

    vecValue.clear();
    std::ignore = manifest->Load(std::move(ara::core::StringView("DoIpInstantiation.gidInvalidityPattern")),
                                 gidInvalidityPattern_);
    std::ignore = manifest->Load(std::move(ara::core::StringView("DoIpInstantiation.logicalAddress")), logicalAddress_);
    std::ignore
        = manifest->Load(std::move(ara::core::StringView("DoIpInstantiation.maxRequestBytes")), maxRequestBytes_);
    std::ignore = manifest->IterateArray(
        std::move(ara::core::StringView("DoIpInstantiation.networkInterface")),
        [this](std::size_t const idx, isoft::manifestreader::ManifestNode const& node) {
            std::ignore = idx;
            uint8_t id{};
            if (node.Load(std::move(ara::core::StringView("networkInterfaceId")), id) != 0) {
                common::LogError() << "Server::Initialize|" << __LINE__;
                return;
            }
            DoIpNetworkConfiguration networkConfig;
            std::ignore = node.Load(std::move(ara::core::StringView("eidUseMac")), networkConfig.eidUseMac);
            std::ignore = node.Load(std::move(ara::core::StringView("isActivationLineDependent")),
                                    networkConfig.isActivationLineDependent);
            std::ignore = node.Load(std::move(ara::core::StringView("maxInitialVehicleAnnouncementTime")),
                                    networkConfig.maxInitialVehicleAnnouncementTime);
            std::ignore = node.Load(std::move(ara::core::StringView("maxTesterConnections")),
                                    networkConfig.maxTesterConnections);
            std::ignore = node.Load(std::move(ara::core::StringView("networkConfiguration")),
                                    networkConfig.networkConfiguration);
            std::ignore = node.Load(std::move(ara::core::StringView("tcpAliveCheckResponseTimeout")),
                                    networkConfig.tcpAliveCheckResponseTimeout);
            std::ignore = node.Load(std::move(ara::core::StringView("tcpGeneralInactivityTime")),
                                    networkConfig.tcpGeneralInactivityTime);
            std::ignore = node.Load(std::move(ara::core::StringView("tcpInitialInactivityTime")),
                                    networkConfig.tcpInitialInactivityTime);
            std::ignore = node.Load(std::move(ara::core::StringView("vehicleAnnouncementCount")),
                                    networkConfig.vehicleAnnouncementCount);
            std::ignore = node.Load(std::move(ara::core::StringView("vehicleAnnouncementInterval")),
                                    networkConfig.vehicleAnnouncementInterval);
            std::ignore = node.Load(std::move(ara::core::StringView("vehicleIdentificationSyncStatus")),
                                    networkConfig.vehicleIdentificationSyncStatus);
            mapDoipNetworkConfig_[id] = networkConfig;
        });
    requestConfigPtr_ = std::make_shared< RequestConfigVector >();
    std::ignore       = manifest->IterateArray(
        std::move(ara::core::StringView("DoIpInstantiation.requestConfiguration")),
        [this](std::size_t const idx, isoft::manifestreader::ManifestNode const& node) {
            std::ignore = idx;
            RequestConfiguration config;
            std::ignore = node.Load(std::move(ara::core::StringView("endAddress")), config.endAddress);
            core::String type;
            std::ignore = node.Load(std::move(ara::core::StringView("requestType")), type);
            if (type == "PHYSICAL") {
                config.requestType = RequestTypeEnum::kPhysical;
            } else if (type == "FUNCTIONAL") {
                config.requestType = RequestTypeEnum::kFunctional;
            } else {
                ;
            }
            std::ignore = node.Load(std::move(ara::core::StringView("startAddress")), config.startAddress);
            requestConfigPtr_->push_back(config);
        });

    std::ignore = manifest->Load(std::move(ara::core::StringView("DoIpInstantiation.vinInvalidityPattern")),
                                 vinInvalidityPattern_);

    std::ignore = manifest->Load(std::move(ara::core::StringView("DoIpInstantiation.doipVersion")), doipVersion_);
    std::ignore
        = manifest->Load(std::move(ara::core::StringView("DoIpInstantiation.noParamVehicleIdentificationSwitch")),
                         noParamVehicleIdentificationSwitch_);
    std::ignore = manifest->Load(std::move(ara::core::StringView("DoIpInstantiation.serverCrt")), serverCrt_);
    std::ignore = manifest->Load(std::move(ara::core::StringView("DoIpInstantiation.serverKey")), serverKey_);
    std::ignore = manifest->Load(std::move(ara::core::StringView("DoIpInstantiation.ca")), ca_);
}

/// @brief Get timeout (unit: second) according to the identifier of DoIPInterface
///        This timeout specifies the maximum direct inactivity time after the TCP_DATA socket is established.
///        After the specified time, without routing activation, the TCP_DATA socket will be closed by the DoIP entity.
/// @param[in] networkInterfaceId Identifier of DoIPInterface
/// @return Maximum duration (unit: second)
/// @throw unknown
float DoIPConfig::TcpInitialInactivityTime(uint8_t const networkInterfaceId)
{
    return mapDoipNetworkConfig_[networkInterfaceId].tcpInitialInactivityTime;
}

/// @brief Get timeout (unit: second) according to the identifier of DoIPInterface
///        This timeout specifies the maximum inactivity time (no data received or sent) of the TCP_DATA socket before being closed by the DoIP entity.
/// @param[in] networkInterfaceId Identifier of DoIPInterface
/// @return Maximum duration (unit: second)
/// @throw unknown
float DoIPConfig::TcpGeneralInactivityTime(uint8_t const networkInterfaceId)
{
    return mapDoipNetworkConfig_[networkInterfaceId].tcpGeneralInactivityTime;
}

/// @brief Get timeout (unit: second) according to the identifier of DoIPInterface
///        This timeout specifies the maximum time the DoIP entity waits for an alive check response after writing an alive check request on the TCP_DATA socket.
///        Therefore, the timer expires if the underlying TCP stack fails to deliver the alive check request message.
/// @param[in] networkInterfaceId Identifier of DoIPInterface
/// @return Maximum duration (unit: second)
/// @throw unknown
float DoIPConfig::TcpAliveCheckResponseTimeout(uint8_t const networkInterfaceId)
{
    return mapDoipNetworkConfig_[networkInterfaceId].tcpAliveCheckResponseTimeout;
}

/// @brief Get whether to depend on the activation line according to the identifier of DoIPInterface
/// @param[in] networkInterfaceId Identifier of DoIPInterface
/// @return true: Depend false: Not depend
/// @throw unknown
bool DoIPConfig::IsActivationLineDependent(uint8_t const networkInterfaceId)
{
    return mapDoipNetworkConfig_[networkInterfaceId].isActivationLineDependent;
}

/// @brief Get network endpoint address according to the identifier of DoIPInterface
/// @param[in] networkInterfaceId Identifier of DoIPInterface
/// @return Network endpoint address collection
/// @throw unknown
NetworkEndpointAddress DoIPConfig::IpAddress(uint8_t const networkInterfaceId)
{
    uint32_t no{mapDoipNetworkConfig_[networkInterfaceId].networkConfiguration};
    no = mapNetworkConfig_[no].communicationConnector;
    no = mapCommunicationConnector_[no].unicastNetworkEndpoint;
    return mapUnicastNetworkEndpoint_[no].address;
}

/// @brief Get UDP port number according to the identifier of DoIPInterface
/// @param[in] networkInterfaceId Identifier of DoIPInterface
/// @return UDP port number
/// @throw unknown
uint16_t DoIPConfig::UdpPort(uint8_t const networkInterfaceId)
{
#ifdef NMODEL
    if (useNmodel_) {
        return mapDoipNetworkConfig_[networkInterfaceId].udpPort_;
    } else {
#endif
        uint32_t const no{mapDoipNetworkConfig_[networkInterfaceId].networkConfiguration};
        return mapNetworkConfig_[no].udpPort_;
#ifdef NMODEL
    }
#endif
}

/// @brief Get the number of vehicle announcements according to the identifier of DoIPInterface. Default: three times
/// @param[in] networkInterfaceId
/// @return Number of external announcements
/// @throw unknown
uint32_t DoIPConfig::VehicleAnnouncementCount(uint8_t const networkInterfaceId)
{
    return mapDoipNetworkConfig_[networkInterfaceId].vehicleAnnouncementCount;
}

/// @brief Get the interval for the DoIP entity to send vehicle announcement messages after configuring a valid IP address according to the identifier of DoIPInterface.
/// @param[in] networkInterfaceId Identifier of DoIPInterface
/// @return Time interval
/// @throw unknown
float DoIPConfig::VehicleAnnouncementInterval(uint8_t const networkInterfaceId)
{
    return mapDoipNetworkConfig_[networkInterfaceId].vehicleAnnouncementInterval;
}

/// @brief Get maximum waiting time (unit: second) according to the identifier of DoIPInterface
///        This timing parameter specifies the initial time the DoIP entity waits to respond to a vehicle identification request after configuring a valid IP address,
///        and the time the DoIP entity waits to send vehicle announcement messages.
///        The value of this timing parameter shall be randomly determined between the minimum and maximum values.
/// @param[in] networkInterfaceId Identifier of DoIPInterface
/// @return Maximum duration (unit: second)
/// @throw unknown
float DoIPConfig::MaxInitialVehicleAnnouncementTime(uint8_t const networkInterfaceId)
{
    return mapDoipNetworkConfig_[networkInterfaceId].maxInitialVehicleAnnouncementTime;
}

/// @brief Get whether the MAC of the network interface is used as EID according to the identifier of DoIPInterface.
/// @param[in] networkInterfaceId Identifier of DoIPInterface
/// @return True: Use MAC False: EID needs to be manually configured via dolplnstatition.eid.
/// @throw unknown
bool DoIPConfig::EidUseMac(uint8_t const networkInterfaceId)
{
    return mapDoipNetworkConfig_[networkInterfaceId].eidUseMac;
}

/// @brief Get whether to additionally use the optional VIN/GID synchronization status in vehicle identification/announcement according to the identifier of DoIPInterface.
/// @param[in] networkInterfaceId Identifier of DoIPInterface
/// @return True: Use VIN/GID synchronization status False: Do not use VIN/GID synchronization status
/// @throw unknown
bool DoIPConfig::VehicleIdentificationSyncStatus(uint8_t const networkInterfaceId)
{
    return mapDoipNetworkConfig_[networkInterfaceId].vehicleIdentificationSyncStatus;
}

}  // namespace doip
}  // namespace diag
}  // namespace ara