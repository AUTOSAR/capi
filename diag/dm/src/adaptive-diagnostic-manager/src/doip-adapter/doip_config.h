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
/// @file       doip_config.h
/// @brief      This file provides the definition of the DoIP configuration management class
/// @details
/// @date       2022-07-08
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_DIAG_DOPI_DOIP_CONFIG_H_
#define ARA_DIAG_DOPI_DOIP_CONFIG_H_
#include <ara/core/map.h>
#include <ara/core/optional.h>
#include <ara/core/span.h>
#include <ara/core/string.h>
#include <ara/core/vector.h>
#include <isoft/doip/config.h>
#include <isoft/doip/public_def.h>
#ifdef NMODEL
    #include <isoft/nml/definition/adaptive_software_component.h>
    #include <isoft/nml/definition/doip_module.h>
    #include <isoft/nml/definition/doip_network_configuration.h>
    #include <isoft/nml/definition/doip_request_configuration.h>

    #include <set>
    #include <vector>
#endif
#include <string>
#include <utility>

namespace isoft {
namespace manifestreader {

/// @brief declaration of Manifest
class Manifest;
}  // namespace manifestreader
}  // namespace isoft
namespace ara {
namespace diag {
namespace doip {

#define MSTOSCONVERT 1000U

/// @brief Enum for Doip request type
/// @name RequestTypeEnum
enum class RequestTypeEnum : uint8_t
{
    kPhysical   = 0,
    kFunctional = 1
};

/// @brief Request configuration, used to determine whether an incoming diagnostics
///        message request should be interpreted as PHYSICAL or FUNCTIONAL.
///        Any request whose target address is not within the configured target address range will be rejected.
/// @name RequestConfiguration
class RequestConfiguration final
{
public:
    /// @name endAddress
    /// End address
    uint16_t endAddress{0};
    /// @name requestType
    /// Address type
    RequestTypeEnum requestType{RequestTypeEnum::kPhysical};
    /// @name startAddress
    /// Start address
    uint16_t startAddress{0};
};

/// @brief Network interface attributes of DoIP.
/// @name DoIpNetworkConfiguration
class DoIpNetworkConfiguration final
{
public:
    /// @name eidUseMac
    /// Whether MAC is used as EID
    bool eidUseMac{false};
    /// @name isActivationLineDependent
    /// Whether to depend on the activation line
    bool isActivationLineDependent{false};
    /// @name maxInitialVehicleAnnouncementTime
    /// This timing parameter specifies the initial time that the DoIP entity waits to respond to a vehicle identification request after configuring a valid IP address,
    /// as well as the time that the DoIP entity waits to send a vehicle announcement message.
    /// The value of this timing parameter should be randomly determined between the minimum and maximum values.
    float maxInitialVehicleAnnouncementTime{0.0F};
    /// @name maxTesterConnections
    /// Maximum number of tester connections before performing an alive check.
    uint32_t maxTesterConnections{0};
#ifdef NMODEL
    /// @name ipv4Address
    /// IPv4 address. Format: 255.255.255.255.
    /// In case ipv4AddressSource is FIXED, the IP address should be declared, so no automatic configuration mechanism is used.
    core::String ipv4Address{};

    /// @name networkMask
    /// Network mask. Format 255.255.255.255
    core::String networkMask;

    /// @name tcpPort
    /// Configure TCP port number
    uint16_t tcpPort;

    /// @name udpPort_
    /// Configure UDP port number
    uint16_t udpPort_;  // NOLINT

#endif
    /// @name networkConfiguration
    /// Network configuration (protocol, port, IP address) for transmitting DoIP messages on the specified VLAN.
    uint32_t networkConfiguration{0};

    /// @name networkInterfaceId
    /// This attribute defines the identifier of the DoIPInterface.
    uint32_t networkInterfaceId{0};
    /// @name tcpAliveCheckResponseTimeout
    /// Timeout (in seconds) that an Alive Check request waits for a response before the connection is considered disconnected.
    float tcpAliveCheckResponseTimeout{0.0F};
    /// @name tcpGeneralInactivityTime
    /// Maximum inactivity timeout for a TCP socket connection before the DoIP module closes the corresponding socket connection.
    /// (in [s])
    /// 13400-2:2012。
    float tcpGeneralInactivityTime{0.0F};
    /// @name tcpInitialInactivityTime
    /// Initial inactivity timeout for a TCP socket connection directly after the socket connection is made.
    /// (in [s])
    float tcpInitialInactivityTime{0.0F};
    /// @name vehicleAnnouncementCount
    /// Number of vehicle announcement messages when assigning an IP address.
    uint32_t vehicleAnnouncementCount{0};
    /// @name vehicleAnnouncementInterval
    /// Waiting time for sending subsequent vehicle notification messages.
    float vehicleAnnouncementInterval{0.0F};
    /// @name vehicleIdentificationSyncStatus
    /// Define whether to additionally use the optional VIN/GID synchronization status in vehicle identification/announcement
    bool vehicleIdentificationSyncStatus{false};
};

/// @brief Configuration attributes defining port, protocol type and IP address for communication on VLAN.
struct PlatformModuleEthernetEndpointConfiguration final
{
    /// @name id
    /// PlatformModuleEthernetEndpointConfiguration object identifier
    uint32_t id;
    /// @name communicationConnector
    /// CommunicationConnector (VLAN) for network configuration.
    uint32_t communicationConnector;

    /// @name tcpPort
    /// Configure TCP port number
    uint16_t tcpPort;
    /// @name udpPort_
    /// Configure UDP port number
    uint16_t udpPort_;  // NOLINT
};

/// @brief Ethernet-specific connection attributes.
struct EthernetCommunicationConnector final
{
    /// @name id
    /// EthernetCommunicationConnector object identifier
    uint32_t id;
    /// @name unicastNetworkEndpoint
    /// Network endpoint defining the machine's IPAddress.
    uint32_t unicastNetworkEndpoint;
};

/// @brief Define how the node obtains its IPv4-Address.
enum class Ipv4AddressSource : std::uint8_t
{
    kAutoIp      = 0,
    kAutoIp_doip = 2,
    kDhcpv4      = 3,
    kFixed       = 4
};

/// @brief Configure IPv4 protocol
class Ipv4Configuration final
{
public:
    /// @name ipv4Address
    /// IPv4 address. Format: 255.255.255.255.
    /// In case ipv4AddressSource is FIXED, the IP address should be declared, so no automatic configuration mechanism is used.
    core::String ipv4Address{};
    /// @name ipv4AddressSource
    /// Define how the node obtains its IP address.
    Ipv4AddressSource ipv4AddressSource{};
    /// @name networkMask
    /// Network mask. Format 255.255.255.255
    core::String networkMask;
};

/// @brief Define how the node obtains its IPv6-Address.
enum class Ipv6AddressSource : std::uint8_t
{
    kDhcpv6              = 0,
    kFixed               = 1,
    kLinkLocal           = 2,
    kLinkLocal_doip      = 3,
    kRouterAdvertisement = 4
};

/// @brief IPv6 (Internet Protocol version 6) configuration.
class Ipv6Configuration final
{
public:
    /// @name ipv6Address
    /// IPv6 address. Format: FFFF:……:FFFF.
    /// In case ipv6AddressSource is FIXED, the IP address should be declared, so no automatic configuration mechanism is used.
    core::String ipv6Address{};
    /// @name ipv6AddressSource
    /// Define how the node obtains its IPv6-Address.
    Ipv6AddressSource ipv6AddressSource{};
    /// @name ipAddressPrefixLength Length of ipAddressPrefix
    /// IPv6 prefix length defines the part of the network prefix in the IPv6 address.
    uint32_t ipAddressPrefixLength{};
};

/// @brief Define IP protocol type
enum class IpType : std::uint8_t
{
    kIpv4 = 0,
    kIpv6 = 1
};

/// @brief To establish a valid network endpoint address, there must be a MAC multicast group reference or ipv4 configuration or ipv6 configuration.
class NetworkEndpointAddress final
{
public:
    /// @name id
    /// NetworkEndpointAddress object identifier
    uint32_t id{};
    /// @name type
    /// Define IP protocol type
    IpType type{};
    /// @name ipv4
    Ipv4Configuration ipv4;
    /// @name ipv6
    Ipv6Configuration ipv6;
};

/// @brief Configure network node information
class NetworkEndpoint final

{
public:
    /// @name id
    /// NetworkEndpoint object identifier
    uint32_t id{};
    /// @name vecAddress vector of NetworkEndpointAddress
    /// Collection of network endpoint addresses
    NetworkEndpointAddress address;
};

/// @brief VinInfo
class VinInfo final
{
public:
    /// @name dataIdentifier
    uint16_t dataIdentifier{0U};
    /// @name diagnosticInterfaceClass
    core::String diagnosticInterfaceClass;
    /// @name diagnosticInstanceIdentifier
    core::String diagnosticInstanceIdentifier;

    std::uint16_t instanceId{0U};
    std::uint32_t serviceInstanceId{0U};
};

class PowerModeInfo final
{
public:
    std::uint16_t instanceId{0U};
    std::uint32_t serviceInstanceId{0U};
};

class GroupIdentificationInfo final
{
public:
    std::uint16_t instanceId{0U};
    std::uint32_t serviceInstanceId{0U};
};

class ActivationLineInfo final
{
public:
    std::uint16_t instanceId{0U};
    std::uint32_t serviceInstanceId{0U};
};

/// @brief Collection of request configurations
using RequestConfigVector = core::Vector< RequestConfiguration >;

/// @brief Shared pointer to collection of request configurations
using RequestConfigVectorPtr = std::shared_ptr< RequestConfigVector >;

/// @brief DoIPConfig, get DoIP-related configuration parameters
class DoIPConfig final
{
private:
    /* data */
public:
    /// @brief Singleton instantiation
    /// @return DoIPConfig&
    /// @throw unknown
    static DoIPConfig& GetConfig();

    /// @brief structure of DoIPConfig
    /// @throw unknown
    DoIPConfig() = default;

    /// @brief deconstruction of DoIPConfig
    ~DoIPConfig() = default;

    /// @brief Copy constructor
    /// @param[in]  other
    /// @throw unknown
    DoIPConfig(DoIPConfig const& other) = default;

    /// @brief Copy assignment operator
    /// @param[in]  other
    /// @return Reference
    /// @throw unknown
    DoIPConfig& operator=(DoIPConfig const& other) = default;

    /// @brief Move constructor
    /// @param[in] other
    /// @throw unknown
    DoIPConfig(DoIPConfig&& other) = default;

    /// @brief Move assignment operator
    /// @param[in] other
    /// @return Reference
    /// @throw unknown
    DoIPConfig& operator=(DoIPConfig&& other) = default;

#ifdef NMODEL
    bool Initialize(
        std::set< std::shared_ptr< isoft::nml::definition::AdaptiveSoftwareComponent > > adaptiveSoftwareComponentTable,
        std::shared_ptr< isoft::nml::definition::DoipModule > doipModulePtr);
#endif
    /// @brief Initialization, parse DoIP.json file
    /// @return true: Success  false: Failure
    /// @throw unknown
    bool Init();

    /// @brief Get TCP port number according to the identifier of DoIPInterface
    /// @param[in] networkInterfaceId Identifier of DoIPInterface
    /// @return TCP port number
    /// @throw unknown
    uint16_t TcpPort(uint8_t const networkInterfaceId);

    /// @brief Get maximum byte length of request
    /// @return Maximum length
    /// @throw unknown
    uint32_t MaxRequestBytes() const noexcept { return maxRequestBytes_; }

    /// @brief Get logical address
    /// @return Current logical address
    /// @throw unknown
    uint16_t LogicalAddress() const noexcept { return logicalAddress_; }

    /// @brief Get maximum number of connections according to the identifier of DoIPInterface
    /// @param[in] networkInterfaceId Identifier of DoIPInterface
    /// @return Maximum number of connections
    /// @throw unknown
    uint32_t MaxTesterConnections(uint8_t const networkInterfaceId);

    /// @brief Get timeout time (in seconds) according to the identifier of DoIPInterface
    ///        This timeout specifies the maximum time of direct inactivity after the TCP_DATA socket is established.
    ///        After the specified time, if there is no activation route, the TCP_DATA socket will be closed by the DoIP entity.
    /// @param[in] networkInterfaceId Identifier of DoIPInterface
    /// @return Maximum duration (in seconds)
    /// @throw unknown
    float TcpInitialInactivityTime(uint8_t const networkInterfaceId);

    /// @brief Get timeout time (in seconds) according to the identifier of DoIPInterface
    ///        This timeout specifies the maximum inactivity time (no data received or sent) of the TCP_DATA socket before it is closed by the DoIP entity.
    /// @param[in] networkInterfaceId Identifier of DoIPInterface
    /// @return Maximum duration (in seconds)
    /// @throw unknown
    float TcpGeneralInactivityTime(uint8_t const networkInterfaceId);

    /// @brief Get timeout time (in seconds) according to the identifier of DoIPInterface
    ///        This timeout specifies the maximum time that the DoIP entity waits for an alive check response after "writing an alive check request on the TCP_DATA socket".
    ///        Therefore, if the underlying TCP stack fails to deliver the alive check request message, the timer will expire.
    /// @param[in] networkInterfaceId Identifier of DoIPInterface
    /// @return Maximum duration (in seconds)
    /// @throw unknown
    float TcpAliveCheckResponseTimeout(uint8_t const networkInterfaceId);

    /// @brief Get whether to depend on the activation line according to the identifier of DoIPInterface
    /// @param[in] networkInterfaceId Identifier of DoIPInterface
    /// @return true:  Dependent  false: Independent
    /// @throw unknown
    bool IsActivationLineDependent(uint8_t const networkInterfaceId);

    /// @brief Get maximum waiting time (in seconds) according to the identifier of DoIPInterface
    ///        This timing parameter specifies the initial time that the DoIP entity waits to respond to a vehicle identification request after configuring a valid IP address,
    ///        as well as the time that the DoIP entity waits to send a vehicle announcement message.
    ///        The value of this timing parameter should be randomly determined between the minimum and maximum values.
    /// @param[in] networkInterfaceId Identifier of DoIPInterface
    /// @return Maximum duration (in seconds)
    /// @throw unknown
    float MaxInitialVehicleAnnouncementTime(uint8_t const networkInterfaceId);

    /// @brief Get all request configurations
    /// @return Request configurations
    RequestConfigVectorPtr RequestConfig() const noexcept { return requestConfigPtr_; }

    /// @brief Get network endpoint address according to the identifier of DoIPInterface
    /// @param[in] networkInterfaceId Identifier of DoIPInterface
    /// @return Network endpoint address
    /// @throw unknown
    NetworkEndpointAddress IpAddress(uint8_t const networkInterfaceId);

    /// @brief Get UDP port number according to the identifier of DoIPInterface
    /// @param[in] networkInterfaceId Identifier of DoIPInterface
    /// @return UDP port number
    /// @throw unknown
    uint16_t UdpPort(uint8_t const networkInterfaceId);

    /// @brief Get vehicle identifier information
    /// @return Vehicle identifier information
    /// @throw unknown
    VinInfo Vin() const { return vinInfo_; }

    /// @brief Get GID invalid value matching value, which can be used to represent GID when GID acquisition fails. This value can only be defined as 0 or 255
    /// @return Invalid matching value
    /// @throw unknown
    uint8_t GidInvalidityPattern() const noexcept { return static_cast< uint8_t >(gidInvalidityPattern_); }

    /// @brief Get VIN invalid value matching value, which can be used to represent VIN when VIN acquisition fails. This value can only be defined as 0 or 255
    /// @return Invalid matching value
    /// @throw unknown
    uint8_t VinInvalidityPattern() const noexcept { return static_cast< uint8_t >(vinInvalidityPattern_); }

    /// @brief Get the value of EID
    /// @param[out] eid Used to get the value of EID
    /// @return True: Success  False: Failure
    bool Eid(isoft::doip::EidValue& eid) noexcept
    {
        std::ignore = eid;
        if (optEid_.has_value()) {
            eid = *optEid_;
            return true;
        }
        return false;
    }

    /// @brief Get the value of GID
    /// @param[out] gid Used to get the value of GID
    /// @return True: Success  False: Failure
    bool Gid(isoft::doip::GidValue& gid) noexcept
    {
        std::ignore = gid;
        if (optGid_.has_value()) {
            gid = *optGid_;
            return true;
        }
        return false;
    }

    /// @brief Get whether the MAC of the network interface is used as EID according to the identifier of DoIPInterface.
    /// @param[in] networkInterfaceId Identifier of DoIPInterface
    /// @return True: Use MAC  False: EID needs to be manually configured through dolplnstatition.eid.
    /// @throw unknown
    bool EidUseMac(uint8_t const networkInterfaceId);

    /// @brief Get whether to additionally use the optional VIN/GID synchronization status in vehicle identification/announcement according to the identifier of DoIPInterface.
    /// @param[in] networkInterfaceId Identifier of DoIPInterface
    /// @return True: Use VIN/GID synchronization status  False: Do not use VIN/GID synchronization status
    /// @throw unknown
    bool VehicleIdentificationSyncStatus(uint8_t const networkInterfaceId);

    /// @brief Get the number of vehicle announcements according to the identifier of DoIPInterface. Default: three times
    /// @param[in] networkInterfaceId
    /// @return Number of external announcements
    /// @throw unknown
    uint32_t VehicleAnnouncementCount(uint8_t const networkInterfaceId);

    /// @brief Get the time interval for the DoIP entity to send vehicle announcement messages after configuring a valid IP address according to the identifier of DoIPInterface.
    /// @param[in] networkInterfaceId Identifier of DoIPInterface
    /// @return Time interval
    /// @throw unknown
    float VehicleAnnouncementInterval(uint8_t const networkInterfaceId);

    /// @brief Get the instance descriptor of the DoipTriggerVehicleAnnouncement service
    /// @return Instance descriptor of the DoipTriggerVehicleAnnouncement service
    /// @throw unknown

    std::uint32_t

    TriggerVehicleAnnouncement() const
    {
        return triggerVehicleAnnouncementServiceInstanceId_;
    }

    PowerModeInfo& PowerMode() { return powerInfo_; }

    GroupIdentificationInfo& GroupGroupIdentification() { return groupInfo_; }

    ara::core::Vector< ActivationLineInfo >& ActivationLine() { return vecActivationLine_; }

    /// @brief Get persistent instance descriptor
    /// @return "dmd/root/DoIpStorage"
    /// @throw unknown
    static core::String StorageInstanceSpecifier();

    /// @brief Get the priority of diagnostic requests
    /// @return Priority
    std::int32_t Priority() const noexcept { return priority_; }

    /// @brief Get protocol type
    /// @return Protocol type
    /// @throw unknown
    core::String ProtocolKind() const { return protocolKind_; }

    /// @brief Get DoIP service configuration
    /// @param[in] networkInterfaceId Network port ID
    /// @param[out] config Configuration
    bool GetDoipServerConfig(uint8_t const networkInterfaceId, isoft::doip::ServerConfig& config);

    /// @brief Get network port list
    /// @param[out] list Network port array
    void GetNetworkInterfaceIdList(core::Vector< uint8_t >& list)
    {
        for (core::Map< uint8_t, DoIpNetworkConfiguration >::iterator it = mapDoipNetworkConfig_.begin();
             it != mapDoipNetworkConfig_.end(); ++it) {
            list.emplace_back(it->first);
        }
    }

    /// @brief Get all DoIP network port configurations
    /// @return All network port configurations
    core::Map< uint8_t, DoIpNetworkConfiguration > const& GetDoIPNetworkConfiguration()
    {
        return mapDoipNetworkConfig_;
    };

    /// @brief Get DoIP version number
    /// @return Version number
    uint8_t GetDoIPVersion() const { return doipVersion_; }

    /// @brief Get the switch for vehicle identification reply without parameters
    /// @return Switch
    bool GetNoParamVehicleIdentificationSwitch() const { return noParamVehicleIdentificationSwitch_; }

    /// @brief Get the server entity certificate for TLS communication
    /// @return Certificate
    core::String GetServerCa() const { return serverCrt_; }

    /// @brief Get the server private key for TLS communication
    /// @return Private key
    core::String GetServerKey() const { return serverKey_; }

private:
#ifdef NMODEL
    bool _initializeDoipNetworkConfiguration(
        std::vector< std::shared_ptr< isoft::nml::definition::DoipNetworkConfiguration > >
            doipNetworkConfigurationTable);
    bool _initializeDoipRequestConfiguration(
        std::vector< std::shared_ptr< isoft::nml::definition::DoipRequestConfiguration > >
            doipRequestConfigurationTable);

    bool _initializeActivationInterface(
        std::set< std::shared_ptr< isoft::nml::definition::AdaptiveSoftwareComponent > > adaptiveSoftwareComponentTable,
        std::string doipModuleFqn) noexcept;

    bool _initializeGroupIdentificationInterface(
        std::set< std::shared_ptr< isoft::nml::definition::AdaptiveSoftwareComponent > > adaptiveSoftwareComponentTable,
        std::string doipModuleFqn) noexcept;

    bool _initializePowerModeInterface(
        std::set< std::shared_ptr< isoft::nml::definition::AdaptiveSoftwareComponent > > adaptiveSoftwareComponentTable,
        std::string doipModuleFqn) noexcept;

    bool _initializeTriggerVehicleAnnouncementInterface(
        std::set< std::shared_ptr< isoft::nml::definition::AdaptiveSoftwareComponent > > adaptiveSoftwareComponentTable,
        std::string doipModuleFqn) noexcept;
#endif

    /// @brief Parse information related to DoIpInstantiation in the configuration file
    /// @param[in] manifest File parser
    /// @throw unknown
    void _initDoIpInstantiation(isoft::manifestreader::Manifest* const manifest);

    /// @name triggerVehicleAnnouncement_
    /// DoipTriggerVehicleAnnouncement service instantiation identifier
    core::String triggerVehicleAnnouncement_;

    PowerModeInfo powerInfo_;

    GroupIdentificationInfo groupInfo_;

    ara::core::Vector< ActivationLineInfo > vecActivationLine_;

    std::uint32_t triggerVehicleAnnouncementServiceInstanceId_{0U};
    /// @name optEid_
    /// EID value
    core::Optional< isoft::doip::EidValue > optEid_;
    /// @name entityStatusMaxByteFieldUse_
    /// Used to distinguish the optional support of the diagnostic entity status response for the "maximum data size" element.
    bool entityStatusMaxByteFieldUse_{};
    /// @name optGid_
    /// GID value
    core::Optional< isoft::doip::GidValue > optGid_;
    /// @name gidInvalidityPattern_
    /// GID invalid value
    uint16_t gidInvalidityPattern_{};
    /// @name logicalAddress_
    /// Describes the logical address of the DoIP entity, which is used for VehicleAnnouncement and RoutingActivation responses.
    uint16_t logicalAddress_{};
    /// @name maxRequestBytes_
    /// Specifies the maximum number of bytes allowed for a DoIP message request without a DoIP header.
    uint32_t maxRequestBytes_{};
    /// @name requestConfigPtr_
    /// Collection of request configurations
    RequestConfigVectorPtr requestConfigPtr_;
    /// @name mapDoipNetworkConfig_
    /// Network interface configuration
    core::Map< uint8_t, DoIpNetworkConfiguration > mapDoipNetworkConfig_;

    /// @name mapNetworkConfig_
    /// Platform module Ethernet configuration
    core::Map< uint32_t, PlatformModuleEthernetEndpointConfiguration > mapNetworkConfig_;
    /// @name mapCommunicationConnector_
    /// Ethernet communication connector
    core::Map< uint32_t, EthernetCommunicationConnector > mapCommunicationConnector_;
    /// @name mapUnicastNetworkEndpoint_
    /// Network address information
    core::Map< uint32_t, NetworkEndpoint > mapUnicastNetworkEndpoint_;

    /// @name vinInvalidityPattern_
    /// VIN invalid value
    uint16_t vinInvalidityPattern_{};
    /// @name vinInfo_
    /// VIN information
    struct VinInfo vinInfo_;
    /// @name priority_
    /// Priority
    std::int32_t priority_{};
    /// @name protocolKind_
    /// Protocol type
    core::String protocolKind_;

    /// @brief  DoIP version number
    uint8_t doipVersion_{3U};

    /// @brief Switch for vehicle identification reply without parameters,  false: Do not reply to messages when receiving a vehicle identification request without parameters
    bool noParamVehicleIdentificationSwitch_{false};

    /// @brief Server entity certificate for TLS communication
    core::String serverCrt_{""};

    /// @brief  Server private key for TLS communication
    core::String serverKey_{""};

    core::String ca_{""};
#ifdef NMODEL
    bool useNmodel_{false};
#endif
};

}  // namespace doip
}  // namespace diag
}  // namespace ara
#endif  // ARA_DIAG_DOPI_DOIP_CONFIG_H_