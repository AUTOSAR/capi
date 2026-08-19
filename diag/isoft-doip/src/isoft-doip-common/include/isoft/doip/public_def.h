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
/// @file       public_def.h
/// @brief      This file provides DOIP common type definition related
/// @details
/// @date       2023-10-08
/// @author     zhanglipeng
/// @version    1.2.0
///
/// ================================================================

#ifndef ISOFT_DOIP_PUBLIC_DEF_H_
#define ISOFT_DOIP_PUBLIC_DEF_H_
#include <ara/core/array.h>
#include <ara/core/string.h>
#include <ara/core/vector.h>

#include <cstdint>
namespace isoft {
namespace doip {
/// @name kVinValueLength
/// VIN value length
static uint32_t const kVinValueLength{17U};
/// @name kMacAddressLength
/// MAC address length
static uint32_t const kMacAddressLength{6U};
/// @name kGidValueLength
/// GID value length
static uint32_t const kGidValueLength{6U};
/// @name kEidValueLength
/// EID length
static uint32_t const kEidValueLength{6U};
/// @name kSecondToMsTransform
/// Conversion unit between seconds and milliseconds
static uint32_t const kSecondToMsTransform{1000U};
/// @brief DOIP packet header size
static size_t const kDoipHeadSize{8U};
/// @name kDoip_PayloadLength_offset
/// PayloadLength field offset in DOIP packet header
static uint8_t const kDoip_PayloadLength_offset{4U};

/// @name VinValue
/// VIN data type
using VinValue = ara::core::Array< uint8_t, kVinValueLength >;
/// @name MacAddress
/// MacAddress data type
using MacAddress = ara::core::Array< uint8_t, kMacAddressLength >;
/// @name GidValue
/// GID data type
using GidValue = ara::core::Array< uint8_t, kGidValueLength >;
/// @name EidValue
/// EID data type
using EidValue = ara::core::Array< uint8_t, kEidValueLength >;

/// @brief GidStatus
/// Gid Status data type
class GidStatus final
{
public:
    /// @name groupIdentification
    /// Stores GID value
    GidValue groupIdentification{};
    /// @name furtherActionRequired
    /// This is additional information used to notify the client that a DOIP entity exists without an initial connection or uses TLS security methods
    std::uint8_t furtherActionRequired{0U};

    /// @brief syncStatus
    /// This is additional information notifying the client that all DOIP entities have synchronized their VIN information
    std::uint8_t syncStatus{0U};
};

/// @brief DOIP protocol version
enum class DoipProtocolVersion : uint8_t
{
    kDoipIso13400_2_2010 = 0x01,
    kDoipIso13400_2_2012 = 0x02
};

/// @brief DOIP message type Table 17
/// c->s Client to server messages
/// s->c Server to client messages
enum class PayloadType : uint16_t
{
    kGenericNegativeAcknowledgement     = 0x0000,  /// Generic DOIP header negative acknowledgment (s->c)
    kVehicleIdRequest                   = 0x0001,  /// Vehicle announcement request (c->s)
    kVehicleIdRequestWithEid            = 0x0002,  /// Vehicle announcement request with EID (c->s)
    kVehicleIdRequestWithVin            = 0x0003,  /// Vehicle announcement request with VIN (c->s)
    kVehicleAnnoucementIdResponse       = 0x0004,  /// Send vehicle announcement (s->c)
    kRoutingActivationRequest           = 0x0005,  /// Route activation request (c->s)
    kRoutingActivationResponse          = 0x0006,  /// Route activation response (s->c)
    kAliveCheckRequest                  = 0x0007,  /// Socket connection alive check request (s->c) (c->s)
    kAliveCheckResponse                 = 0x0008,  /// Socket connection alive check response (c->s) (s->c)
    kDoipEntityStatusRequest            = 0x4001,  /// Entity status request (c->s)
    kDoipEntityStatusResponse           = 0x4002,  /// Entity status response (s->c)
    kDiagPowerModeInfoRequest           = 0x4003,  /// Power mode request (c->s)
    kDiagPowerModeInfoResponse          = 0x4004,  /// Power mode response (s->c)
    kDiagMessage                        = 0x8001,  /// Diagnostic message (c->s) (s->c)
    kDiagMessagePositiveAcknowledgement = 0x8002,  /// Diagnostic message positive response (s->c)
    kDiagMessageNegativeAcknowledgement = 0x8003   /// Diagnostic message negative response (s->c)
};

/// @brief DOIP entity type (page 35)
enum class NodeType : uint8_t
{
    kDoipGateway = 0x00,  /// Gateway
    kDoipNode    = 0x01   /// Normal node
};

/// @brief Power mode
enum class PowerMode : uint8_t
{
    kNotReady     = 0x00,  /// Not ready
    kReady        = 0x01,  /// Ready
    kNotSupported = 0x02   /// Not supported
};

/// @brief Diagnostic negative response error code
enum class DiagNackType : uint8_t
{
    kInvalidSourceAddress   = 0x02,  /// Invalid source address
    kInvalidTargetAddress   = 0x03,  /// Invalid target address
    kTooLargeMessage        = 0x04,  /// Message too large
    kReceiveBufferOverflow  = 0x05,  /// Buffer overflow
    kUnreachableTarget      = 0x06,  /// Message target unreachable due to missing route activation
    kUnknownNetwork         = 0x07,  /// Message rejected due to unknown network error
    kTransportProtocolError = 0x08   /// Message rejected due to transport protocol error
};

/// @brief Generic header negative acknowledgment error code
enum class GenericNackType : uint8_t
{
    kInvalidProtocolVersion = 0x00,  /// DOIP protocol version not supported (close connection)
    kUnsupportedPayloadType = 0x01,  /// Message type not supported (discard packet)
    kToolLargeMessage       = 0x02,  /// Message too large (discard packet)
    kReceiveBufferOverflow  = 0x03,  /// Buffer overflow (discard packet)
    kInvalidPayloadLength   = 0x04,  /// Corresponding message length invalid (close connection)
    kGenericNackTypeEnd,             /// This value is not a standard value, added for convenience
};

/// @brief Route activation type
enum class RoutingActivationType : uint8_t
{
    kRoutingActivationType_Default  = 0x00U,  /// Default type
    kRoutingActivationType_Required = 0x01U,  /// Legally required diagnostic communication
    kRoutingActivationType_Security = 0xE0U,  /// Central security
};

/// @brief Route activation response code page 75
enum class RoutingActivationResponseType : uint8_t
{
    kInvalidSourceAddress = 0x00,  /// Invalid source address. Close connection
    kNoSocketAvailable    = 0x01,  /// Close connection because concurrent TCP socket is unavailable
    kBusy                 = 0x02,  /// Route activation rejected because received SA differs from table connection entry on already activated TCP_DATA socket, close connection
    kAlreadyRegisteredTester = 0x03,  /// Route activation rejected because SA is already registered and activated on another TCP_DATA socket, close connection
    kFailedAuthentication      = 0x04,  /// Missing authentication, route activation rejected
    kRejectedConfirmation      = 0x05,  /// Route activation rejected due to confirmation rejection, close connection
    kUnsupportedActivationType = 0x06,  /// Route activation rejected due to unsupported route activation type, close connection
    kNoSecureSocket = 0x07,  /// Route activation rejected because the specified activation type requires a secure TLS TCP_DATA socket, close connection
    kSuccessful = 0x10,  /// Route activation successful
    kPending    = 0x11   /// Route is activated, requires confirmation, route can only be activated after in-vehicle confirmation
};

/// @brief DOIP connection status definition
/// Reference: ISO 13400-2 p.43
enum class DoipConnectionState : uint8_t
{
    kListen         = 0x00,
    kInitialized    = 0x01,
    kAuthentication = 0x02,
    kConfirmation   = 0x03,
    kActive         = 0x04,
    kFinalize       = 0x05
};

/// @brief Packet header parsing operation code
enum class ParsPackageHeadCode : int8_t
{
    kParsPackageHeadCode_Success                   = 0,   /// Parse successful
    kParsPackageHeadCode_HeadNotComplete           = -1,  /// Packet header incomplete, continue waiting for packet arrival
    kParsPackageHeadCode_ParsErr                   = -2,  /// Deserialization error
    kParsPackageHeadCode_protocolVersionErr        = -3,  /// Protocol version error, reply NACK and close network connection
    kParsPackageHeadCode_inverseprotocolVersionErr = -4,  /// Inverse protocol version error, reply NACK and close network connection
    kParsPackageHeadCode_PayloadTypeErr            = -5,  /// Message type not supported, discard data packet
    kParsPackageHeadCode_MsgTooLarge               = -6,  /// Message length too long, reply NACK and discard data packet
    kParsPackageHeadCode_PayloadLengthErr          = -7,  /// Payload message length error, reply NACK and close network connection
    kParsPackageHeadCode_BodyNotComplete           = -8,  /// Packet body incomplete, continue waiting for packet arrival
};

}  // namespace doip
}  // namespace isoft

#endif  // ISOFT_DOIP_PUBLIC_DEF_H_