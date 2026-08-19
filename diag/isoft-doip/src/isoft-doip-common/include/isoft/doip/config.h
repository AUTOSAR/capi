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
/// @file       config.h
/// @brief      This document provides DOIP configuration related information
/// @details
/// @date       2023-10-08
/// @author     zhanglipeng
/// @version    1.2.0
///
/// ================================================================

#ifndef ISOFT_DOIP_CONFIG_H_
#define ISOFT_DOIP_CONFIG_H_
#include <ara/core/string.h>
#include <ara/core/vector.h>

namespace isoft {
namespace doip {
uint16_t const kDefaultPort{13400U};  /// NOLINT

uint16_t const kDefaultTlsPort{3496U};  /// NOLINT

uint32_t const kDefaultReceiveMaxBytes{65535U};  /// NOLINT

uint32_t const kDefaultVehicleAnnouncementInterval{500U};  /// NOLINT

uint32_t const kDefaultVehicleAnnouncementWait{500U};  /// NOLINT

uint32_t const kDefaultGeneralInactivityTimer{300U};  /// NOLINT

uint32_t const kDefaultAliveCheckTimer{500U};  /// NOLINT

uint32_t const kDefaultTesterMaxConnections{10U};  /// NOLINT

uint32_t const kDefaultRequestMaxBytes{65535U};  /// NOLINT

uint16_t const kRoutingActivationSourceAdressCheckBegin{0x0E00U};  /// NOLINT

uint16_t const kRoutingActivationSourceAdressCheckEnd{0x0FFFU};  /// NOLINT
/// @brief Doip client configuration
class ClientConfig final
{
public:
    /// @brief tcp port
    uint16_t tcpPort_{kDefaultPort};  // NOLINT
    /// @brief udp port
    uint16_t udpPort_{kDefaultPort};  // NOLINT
    /// @brief To connect to the IP address of the ECU
    ara::core::String serverIP_{};  // NOLINT
    /// @brief The default broadcast address is the first broadcast address
    ara::core::String broadcastIP_{"255.255.255.255"};  // NOLINT
    /// @brief Source address (doip client logical address)
    uint16_t sourceAddress_{0U};  // NOLINT
    /// @brief doip version
    uint8_t doipProtocolVersion_{0x02U};  // NOLINT
    /// @brief Message sending interval, unit: seconds
    uint32_t processingTime_{2U};  // NOLINT
    /// @brief Route activation type
    uint8_t routingActivationType_{0x00U};  // NOLINT
    /// @brief The maximum allowed number of bytes for receiving DoIP messages without DoIP headers
    uint32_t receiveMaxBytes{kDefaultReceiveMaxBytes};

    /// @brief is use tls
    bool isUseTls{false};
    /// @brief CA certificate
    ara::core::String ca{""};

    /// @brief Client entity certificate
    ara::core::String clientCrt{""};
    /// @brief Client entity key
    ara::core::String clientKey{""};

    /// @brief tls port
    uint16_t tlsPort_{kDefaultTlsPort};  // NOLINT
};

/// @brief DoIP server configuration
class DiagAddressRadius final
{
public:
    /// @brief Diagnostic starting address
    uint16_t startAddress{0U};
    /// @brief Diagnostic End Address
    uint16_t endAddress{0U};
};

/// @brief DoIP Server Config
class ServerConfig final
{
public:
    /// @brief Server logical address (used for vehicle announcement response and routing activation response)
    uint16_t logicAdress_{};  /// NOLINT
    /// @brief tcp port
    uint16_t tcpPort_{kDefaultPort};  /// NOLINT
    /// @brief udp port
    uint16_t udpPort_{kDefaultPort};  /// NOLINT
    /// @brief The IP address to be monitored defaults to all network ports
    ara::core::String serverIP_{"0.0.0.0"};  /// NOLINT
    /// @brief Subnet mask
    ara::core::String networkMask{};
    /// @brief doip protocol version
    uint8_t doipProtocolVersion_{0x03U};  // NOLINT
    /// @brief Number of vehicle announcements after configuring a valid IP address
    uint32_t vehicleAnnouncementCount{3U};
    /// @brief Trigger 3 vehicle announcements, the interval between each message, unit: milliseconds
    uint32_t vehicleAnnouncementInterval{kDefaultVehicleAnnouncementInterval};
    /// @brief The maximum time before replying to vehicle identification request and actively sending vehicle announcement, random between [0, this value], unit: milliseconds
    uint32_t vehicleAnnouncementWait{kDefaultVehicleAnnouncementWait};
    /// @brief Socket general inactivity timer duration, unit: seconds
    uint32_t generalInactivityTimer{kDefaultGeneralInactivityTimer};
    /// @brief Socket initial inactivity timer duration, unit: seconds
    uint32_t initialInactivityTimer{2U};
    /// @brief Wait for alive check response timer, unit: milliseconds
    uint32_t aliveCheckTimer{kDefaultAliveCheckTimer};
    /// @brief Maximum number of concurrent socket connections, i.e., the maximum number of testers that can be connected
    uint32_t testerMaxConnections{kDefaultTesterMaxConnections};
    /// @brief Maximum allowed bytes for DoIP message requests without DoIP headers
    uint32_t requestMaxBytes{kDefaultRequestMaxBytes};
    /// @brief The range of SA for routing activation check, start range, closed interval
    uint16_t routingActivationSourceAdressCheckBegin{kRoutingActivationSourceAdressCheckBegin};
    /// @brief The range of SA for routing activation check, end range, closed interval
    uint16_t routingActivationSourceAdressCheckEnd{kRoutingActivationSourceAdressCheckEnd};
    /// Diagnostic target address range
    ara::core::Vector< DiagAddressRadius > diagAddressRadius_{};  /// NOLINT
    /// @brief Gateway flag
    bool isGateWay{false};

    /// @brief Whether to use tls
    bool isUseTls{false};
    /// @brief Server entity certificate
    ara::core::String serverCrt{""};
    /// @brief Server entity key
    ara::core::String serverKey{""};
    /// @brief CA certificate
    ara::core::String ca{""};
    /// @brief tls port
    uint16_t tlsPort_{kDefaultTlsPort};  /// NOLINT
};

}  // namespace doip
}  // namespace isoft

#endif  // ISOFT_DOIP_CONFIG_H_