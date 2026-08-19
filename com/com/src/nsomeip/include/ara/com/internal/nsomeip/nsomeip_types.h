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
/// @file       nsomeip_types.h
/// @brief      Binding layer type header file
/// @details
/// @date       2021-12-31
/// @author     mazelin
/// @version    1.2.0
///
/// ================================================================

#ifndef __COM_NSOMEIP_TYPES_H
#define __COM_NSOMEIP_TYPES_H

#include "ara/com/types.h"
#include "nsomeip/core/nsi_app.h"
#include "nsomeip/core/nsi_types.h"
#include "nsomeip/net/nsi_message.h"

/// @brief Namespace -- internal binding layer type
namespace ara {
namespace com {
namespace internal {
namespace nsomeip {
namespace types {
/// @brief Type alias -- service identifier
using ServiceId = nsi_serv_t;
/// @brief Type alias -- instance identifier
using InstanceId = nsi_inst_t;
/// @brief Type alias -- service major version
using ServiceVersionMajor = nsi_version_t;
/// @brief Type alias -- service minor version
using ServiceVersionMinor = nsi_versmin_t;
/// @brief Service version blacklist type
struct BlacklistedVersion
{
    /// @brief Service major version
    ServiceVersionMajor major;
    /// @brief Service minor version
    ServiceVersionMinor minor;
};
/// @brief Type alias -- event group identifier
using EventGroupId = nsi_gid_t;
/// @brief Type alias -- event identifier
using EventId = nsi_eid_t;
/// @brief Type alias -- method identifier
using MethodId = nsi_method_t;
/// @brief Convert to generic fixed-length array
/// @tparam T Array element type
/// @tparam kN Number of array elements
/// @tparam kI List of array element indices
/// @param[in] a Original array reference
/// @return Generic fixed-length array
template < class T, size_t kN, size_t... kI >
constexpr ara::core::Array< std::remove_cv_t< T >, kN > ToArray(T (&a)[kN], std::index_sequence< kI... >) noexcept
{
    return decltype(ToArray(a)){{a[kI]...}};
}
/// @brief Convert to generic fixed-length array
/// @tparam T Array element type
/// @tparam kN Number of array elements
/// @param[in] a Original array reference
/// @return Generic fixed-length array
template < class T, size_t kN >
constexpr ara::core::Array< std::remove_cv_t< T >, kN > ToArray(T (&a)[kN]) noexcept
{
    return ToArray(a, std::make_index_sequence< kN >{});
}
/// @brief Convert to generic fixed-length array
/// @tparam T Array element type
/// @tparam kN Number of array elements
/// @tparam kI List of array element indices
/// @param[in] a Original array rvalue reference
/// @return Generic fixed-length array
template < class T, size_t kN, size_t... kI >
constexpr ara::core::Array< std::remove_cv_t< T >, kN > ToArray(T(&&a)[kN], std::index_sequence< kI... >) noexcept
{
    return decltype(ToArray(a)){{std::move(a[kI])...}};
}
/// @brief Convert to generic fixed-length array
/// @tparam T Array element type
/// @tparam kN Number of array elements
/// @param[in] a Original array rvalue reference
/// @return Generic fixed-length array
template < class T, size_t kN >
constexpr ara::core::Array< std::remove_cv_t< T >, kN > ToArray(T(&&a)[kN]) noexcept
{
    return ToArray(std::move(a), std::make_index_sequence< kN >{});
}
ara::core::String& ToString(ara::core::String& r, nsi_message_t const& m) noexcept;
}  // namespace types

/// @brief Message type
struct Message
{
    /// @brief Type alias -- application layer application
    using App = nsi_app_t;
    /// @brief Type alias -- application layer message
    using Msg = nsi_message_t;
    /// @brief Type alias -- service identifier
    using ServiceId = types::ServiceId;
    /// @brief Type alias -- instance identifier
    using InstanceId = types::InstanceId;
    /// @brief Type alias -- service major version
    using MajorVersion = types::ServiceVersionMajor;
    /// @brief Type alias -- service minor version
    using MinorVersion = types::ServiceVersionMinor;
    /// @brief Type alias -- event group identifier
    using EventGroupId = types::EventGroupId;
    /// @brief Type alias -- method/event identifier
    using MethodId = types::MethodId;
    /// @brief Type alias -- message length
    using Length = decltype(Msg::hdr.len);
    /// @brief Type alias -- client identifier
    using ClientId = nsi_cid_t;
    /// @brief Type alias -- session identifier
    using SessionId = nsi_session_t;
    /// @brief Type alias -- protocol version
    using ProtocolVersion = decltype(Msg::hdr.protocol);
    /// @brief Type alias -- interface version
    using InterfaceVersion = decltype(Msg::hdr.interface);
    /// @brief Type alias -- message type
    using MessageType = decltype(Msg::hdr.type);
    /// @brief Type alias -- message return code
    using ReturnCode = decltype(Msg::hdr.code);
    /// @brief Type alias -- transport protocol
    using TransportProtocol = uint8_t;
    /// @brief Type alias -- session handling mode
    using SessionHandling = uint8_t;

    /// @ref [PRS_SOMEIPSD_00515] -- Reserved and special Service-IDs
    /// @brief Constant -- service identifier: 0x0000 Reserved
    static constexpr ServiceId kServiceIdReserved{0x0000};
    /// @brief Constant -- service identifier: 0xFF00 - 0xFF1F Reserved for Testing at OEM
    static constexpr ServiceId kServiceIdReservedMin4OEMTest{0xFF00};
    static constexpr ServiceId kServiceIdReservedMax4OEMTest{0xFF1F};
    /// @brief Constant -- service identifier: 0xFF20 - 0xFF3F Reserved for Testing at Tier-1
    static constexpr ServiceId kServiceIdReservedMin4Tier1Test{0xFF20};
    static constexpr ServiceId kServiceIdReservedMax4Tier1Test{0xFF3F};
    /// @brief Constant -- service identifier: 0xFF40 - 0xFF5F Reserved for ECU Internal Communication (Tier-1 proprietary)
    static constexpr ServiceId kServiceIdReservedMin4Tier1ECU{0xFF40};
    static constexpr ServiceId kServiceIdReservedMax4Tier1ECU{0xFF5F};
    /// @brief Constant -- service identifier: 0xFFFE Reserved for announcing non-SOME/IP service instances.
    static constexpr ServiceId kServiceIdReservedNonService{0xFFFE};
    /// @brief Constant -- service identifier: 0xFFFF SOME/IP and SOME/IP-SD special service (Magic Cookie, SOME/IPSD, ...).
    static constexpr ServiceId kServiceIdReservedSpecialService{0xFFFF};
    /// @brief Constant -- service identifier: any service identifier
    static constexpr ServiceId kServiceIdAny{NSI_SERVICE_ANY};

    /// @ref [PRS_SOMEIPSD_00516] -- Reserved and Special Instance-IDs
    /// @brief Constant -- instance identifier: 0x0000 Reserved
    static constexpr InstanceId kInstanceIdReserved{0x0000};
    /// @brief Constant -- instance identifier: 0xFFFF All Instances
    static constexpr InstanceId kInstanceIdReservedAll{0xFFFF};
    /// @brief Constant -- instance identifier: any instance identifier
    static constexpr InstanceId kInstanceIdAny{NSI_INSTANCE_ANY};

    /// @brief Constant -- service major version: default
    static constexpr MajorVersion kMajorVersionDefault{NSI_DEFAULT_MAJOR};
    /// @brief Constant -- service major version: any
    static constexpr MajorVersion kMajorVersionAny{NSI_MAJOR_ANY};
    /// @brief Constant -- service major version: undefined blacklist
    static constexpr MajorVersion kMajorVersionUndefinedBlacklisted{kMajorVersionAny - 1};

    /// @brief Constant -- service minor version: default
    static constexpr MinorVersion kMinorVersionDefault{0x00000000};
    /// @brief Constant -- service minor version: any
    static constexpr MinorVersion kMinorVersionAny{NSI_MINOR_ANY};

    /// @ref [PRS_SOMEIPSD_00531] -- Reserved Eventgroup-IDs
    /// @brief Constant -- event group identifier: 0x0000 Reserved
    static constexpr EventGroupId kEventGroupIdReserved{0x0000};
    /// @brief Constant -- event group identifier: 0xFFFF All Eventgroups
    static constexpr EventGroupId kEventGroupIdReservedAll{0xFFFF};

    /// @ref [PRS_SOMEIPSD_00517] -- Reserved and Special Method/Event-IDs
    /// @ref [PRS_SOMEIPSD_00519] -- Method-IDs and Event-IDs of Service 0xFFFF
    /// @brief Constant -- method/event identifier: 0x0000 Reserved
    static constexpr MethodId kMethodIdReservedMin4Method{0x0000};
    /// @brief Constant -- method/event identifier: 0x7FFF Reserved
    static constexpr MethodId kMethodIdReservedMax4Method{0x7FFF};
    /// @brief Constant -- method/event identifier: 0x8000 Reserved
    static constexpr MethodId kMethodIdReservedMin4Event{0x8000};
    /// @brief Constant -- method/event identifier: 0xFFFF Reserved
    static constexpr MethodId kMethodIdReservedMax4Event{0xFFFF};

    /// @brief Constant -- method/event identifier: 0x0000 SOME/IP Magic Cookie Messages
    static constexpr MethodId kMethodIdReservedMCMethod{0x0000};
    /// @brief Constant -- method/event identifier: 0x8000 SOME/IP Magic Cookie Messages
    static constexpr MethodId kMethodIdReservedMCEvent{0x8000};
    /// @brief Constant -- method/event identifier: 0x8100 SOME/IP-SD messages (events)
    static constexpr MethodId kMethodIdReservedSDEvents{0x8100};

    /// @brief Constant -- method/event identifier: any
    static constexpr MethodId kMethodIdAny{NSI_METHOD_ANY};

    /// @ref [PRS_SOMEIP_00042]
    /// @brief Constant -- length: 8 lower bound client + session + protocol + interface + type + code
    static constexpr Length kLengthMin{8};

    /// @ref [PRS_SOMEIP_00051]
    /// @brief Constant -- protocol version: default
    static constexpr ProtocolVersion kProtocolVersionDefault{NSI_PROTOCOL_VERSION};

    /// @ref [PRS_SOMEIP_00055] -- Message Types
    /// @brief Constant -- message type: 0XFF Unknown
    static constexpr MessageType kMessageTypeUnknown{0XFF};
    /// @brief Constant -- message type: 0X00 REQUEST A request expecting a response (even void)
    static constexpr MessageType kMessageTypeRequest{NSI_MT_REQUEST};
    /// @brief Constant -- message type: 0X01 REQUEST_NO_RETURN A fire&forget request
    static constexpr MessageType kMessageTypeRequestNoReturn{NSI_MT_REQUEST_NO_RETURN};
    /// @brief Constant -- message type: 0X02 NOTIFICATION A request of a notification/event callback expecting no response
    static constexpr MessageType kMessageTypeNotification{NSI_MT_NOTIFICATION};
    /// @brief Constant -- message type: 0X80 RESPONSE The response message
    static constexpr MessageType kMessageTypeResponse{NSI_MT_RESPONSE};
    /// @brief Constant -- message type: 0X81 ERROR The response containing an error
    static constexpr MessageType kMessageTypeError{NSI_MT_ERROR};
    /// @brief Constant -- message type: 0X20 TP_REQUEST A TP request expecting a response (even void)
    static constexpr MessageType kMessageTypeTpRequest{0X20};
    /// @brief Constant -- message type: 0X21 TP_REQUEST_NO_RETURN A TP fire&forget request
    static constexpr MessageType kMessageTypeTpRequestNoReturn{0X21};
    /// @brief Constant -- message type: 0X22 TP_NOTIFICATION A TP request of a notification/event callback expecting no response
    static constexpr MessageType kMessageTypeTpNotification{0X22};
    /// @brief Constant -- message type: 0XA0 TP_RESPONSE The TP response message
    static constexpr MessageType kMessageTypeTpResponse{0XA0};
    /// @brief Constant -- message type: 0XA1 TP_ERROR The TP response containing an error
    static constexpr MessageType kMessageTypeTpError{0XA1};

    /// @ref [PRS_SOMEIP_00187] -- Return Codes
    /// @brief Constant -- message return code: 0x00 E_OK No error occurred
    static constexpr ReturnCode kReturnCodeOk{0x00};
    /// @brief Constant -- message return code: 0x01 E_NOT_OK An unspecified error occurred
    static constexpr ReturnCode kReturnCodeNotOk{0x01};
    /// @brief Constant -- message return code: 0x02 E_UNKNOWN_SERVICE The requested Service ID is unknown.
    static constexpr ReturnCode kReturnCodeUnknownService{0x02};
    /// @brief Constant -- message return code: 0x03 E_UNKNOWN_METHOD The requested Method ID is unknown. Service ID is known.
    static constexpr ReturnCode kReturnCodeUnknownMethod{0x03};
    /// @brief Constant -- message return code: 0x04 E_NOT_READY Service ID and Method ID are known. Application not running.
    static constexpr ReturnCode kReturnCodeNotReady{0x04};
    /// @brief Constant -- message return code: 0x05 E_NOT_REACHABLE System running the service is not reachable (internal error code only).
    static constexpr ReturnCode kReturnCodeNotReachable{0x05};
    /// @brief Constant -- message return code: 0x06 E_TIMEOUT A timeout occurred (internal error code only).
    static constexpr ReturnCode kReturnCodeTimeout{0x06};
    /// @brief Constant -- message return code: 0x07 E_WRONG_PROTOCOL_VERSION Version of SOME/IP protocol not supported
    static constexpr ReturnCode kReturnCodeWrongProtocolVersion{0x07};
    /// @brief Constant -- message return code: 0x08 E_WRONG_INTERFACE_VERSION Interface version mismatch
    static constexpr ReturnCode kReturnCodeWrongInterfaceVersion{0x08};
    /// @brief Constant -- message return code: 0x09 E_MALFORMED_MESSAGE Deserialization error, so that payload cannot be deserialized.
    static constexpr ReturnCode kReturnCodeMalformedMessage{0x09};
    /// @brief Constant -- message return code: 0x0A E_WRONG_MESSAGE_TYPE An unexpected message type was received (e.g. REQUEST_NO_RETURN for a method defined as REQUEST).
    static constexpr ReturnCode kReturnCodeWrongMessageType{0x0A};
    /// @brief Constant -- message return code: 0x0B E_E2E_REPEATED Repeated E2E calculation error
    static constexpr ReturnCode kReturnCodeE2ERepeated{0x0B};
    /// @brief Constant -- message return code: 0x0C E_E2E_WRONG_SEQUENCE Wrong E2E sequence error
    static constexpr ReturnCode kReturnCodeE2EWrongSequence{0x0C};
    /// @brief Constant -- message return code: 0x0D E_E2E Not further specified E2E error
    static constexpr ReturnCode kReturnCodeE2E{0x0D};
    /// @brief Constant -- message return code: 0x0E E_E2E_NOT_AVAILABLE E2E not available
    static constexpr ReturnCode kReturnCodeE2ENotAvailable{0x0E};
    /// @brief Constant -- message return code: 0x0F E_E2E_NO_NEW_DATA No new data for E2E calculation present.
    static constexpr ReturnCode kReturnCodeE2ENoNewData{0x0F};
    /// @brief Constant -- message return code: 0x10 - 0x1F RESERVED Reserved for generic SOME/IP errors. These errors will be specified in future versions of this document.
    static constexpr ReturnCode kReturnCodeReservedMin4Generic{0x10};
    static constexpr ReturnCode kReturnCodeReservedMax4Generic{0x1F};
    /// @brief Constant -- message return code: 0x20 - 0x5E RESERVED Reserved for specific errors of services and methods. These errors are specified by the interface specification.
    static constexpr ReturnCode kReturnCodeReservedMin4Specific{0x20};
    static constexpr ReturnCode kReturnCodeReservedMax4Specific{0x5E};

    /// [TR_SOMEIP_00187] When the return code reserves the first 2 bits, verify the last 6 bits
    /// @brief Constant -- message return code reserved bit mask: 0xC0
    static constexpr ReturnCode kReturnCodeReservedMask{0xC0};
    /// @brief Constant -- message return code valid bit mask: 0x3F
    static constexpr ReturnCode kReturnCodeValidMask{0x3F};

    /// @brief Constant -- transport protocol: unknown
    static constexpr TransportProtocol kTransportProtocolUnknown{NSI_RT_UNKNOWN};
    /// @brief Constant -- transport protocol: TCP
    static constexpr TransportProtocol kTransportProtocolTCP{NSI_RT_RELIABLE};
    /// @brief Constant -- transport protocol: UDP
    static constexpr TransportProtocol kTransportProtocolUDP{NSI_RT_UNRELIABLE};
    /// @brief Constant -- transport protocol: TCP/UDP
    static constexpr TransportProtocol kTransportProtocolBoth{NSI_RT_BOTH};

    /// @brief Constant -- session handling mode: unknown
    static constexpr SessionHandling kSessionHandlingUnknown{0x00};
    /// @brief Constant -- session handling mode: active
    static constexpr SessionHandling kSessionHandlingActive{0x01};
    /// @brief Constant -- session handling mode: inactive
    static constexpr SessionHandling kSessionHandlingInactive{0x02};

    /// @brief Application layer application
    App& app;
    /// @brief Application layer message
    std::shared_ptr< Msg > msg{};

    /// @brief Determine whether it is an event identifier
    static constexpr bool IsEventId(MethodId eventId) noexcept
    {
        return eventId > kMethodIdReservedMin4Event && eventId < kMethodIdReservedMax4Event;
    }
    /// @brief Determine whether it is a request identifier
    static constexpr bool IsMethodId(MethodId methodId) noexcept
    {
        return methodId > kMethodIdReservedMin4Method && methodId < kMethodIdReservedMax4Method;
    }

    /// @brief Determine whether it is an event message
    static constexpr bool IsEvent(MessageType type) noexcept { return type == kMessageTypeNotification; }
    /// @brief Determine whether it is a request message
    static constexpr bool IsRequest(MessageType type) noexcept { return type == kMessageTypeRequest; }
    /// @brief Determine whether it is a no-return request message
    static constexpr bool IsRequestNoReturn(MessageType type) noexcept { return type == kMessageTypeRequestNoReturn; }
    /// @brief Determine whether it is a response message
    static constexpr bool IsResponse(MessageType type) noexcept { return type == kMessageTypeResponse; }
    /// @brief Determine whether it is an error message
    static constexpr bool IsError(MessageType type) noexcept { return type == kMessageTypeError; }

    /// @brief Determine whether it is an event message
    bool IsEvent() const noexcept { return IsEvent(msg->hdr.type); }
    /// @brief Determine whether it is a request message
    bool IsRequest() const noexcept { return IsRequest(msg->hdr.type); }
    /// @brief Determine whether it is a no-return request message
    bool IsRequestNoReturn() const noexcept { return IsRequestNoReturn(msg->hdr.type); }
    /// @brief Determine whether it is a response message
    bool IsResponse() const noexcept { return IsResponse(msg->hdr.type); }
    /// @brief Determine whether it is an error message
    bool IsError() const noexcept { return IsError(msg->hdr.type); }

    /// @brief Type alias -- mark enumerable
    using IsEnumerableTag = void;
    /// @brief Enumeration operation -- enumerate members of this type by passing in a function body
    /// @tparam F Incoming function body type
    /// @param[in] fun Incoming function body
    template < typename F >
    void enumerate(F& fun)  // NOLINT -- Template interface name > naming convention
    {
        fun(msg->hdr.serv);
        fun(msg->inst);
        fun(msg->hdr.method);
        fun(msg->hdr.len);
        fun(msg->hdr.client);
        fun(msg->hdr.session);
        fun(msg->hdr.interface);
        fun(msg->hdr.type);
        fun(msg->hdr.code);
        fun(msg->reliable ? true : false);
    }
    /// @brief Operator -- equality comparison
    /// @param[in] other
    /// @return bool
    bool operator==(Message const& other) const noexcept
    {
        if (!(msg.get() == other.msg.get())) {
            return false;
        }
        return true;
    }
    /// @brief Operator -- less than comparison
    /// @param[in] other
    /// @return bool
    bool operator<(Message const& other) const noexcept
    {
        if (!(msg.get() == other.msg.get())) {
            return msg.get() < other.msg.get();
        }
        return false;
    }
};

/// @brief Service instance information type -- declaration
struct ServiceInfo;
/// @brief Event group information type -- declaration
struct GroupInfo;
/// @brief Event information type -- declaration
struct EventInfo;
/// @brief Method information type -- declaration
struct MethodInfo;
/// @brief Field information type -- declaration
struct FieldInfo;

/// @brief Service instance information type
struct ServiceInfo
{
    /// @brief Type alias -- service identifier
    using ServiceId = Message::ServiceId;
    /// @brief Type alias -- instance identifier
    using InstanceId = Message::InstanceId;
    /// @brief Type alias -- service major version
    using MajorVersion = Message::MajorVersion;
    /// @brief Type alias -- service minor version
    using MinorVersion = Message::MinorVersion;
    /// @brief Type alias -- event group identifier
    using EventGroupId = Message::EventGroupId;
    /// @brief Type alias -- event identifier
    using EventId = Message::MethodId;
    /// @brief Type alias -- method identifier
    using MethodId = Message::MethodId;
    /// @brief Type alias -- service instance identifier
    using InstanceID = std::pair< ServiceId, InstanceId >;
    /// @brief Type alias -- service instance unique identifier
    using InstanceUID = std::tuple< ServiceId, InstanceId, MajorVersion, MinorVersion >;

    /// @brief Constructor
    /// @param[in] instanceIdentifierIn Instance identifier
    explicit ServiceInfo(InstanceIdentifier&& instanceIdentifierIn) noexcept
        : instanceIdentifier{std::move(instanceIdentifierIn)}
    {
    }
    /// @brief Instance identifier
    InstanceIdentifier instanceIdentifier;
    /// @brief Enumeration -- service type
    enum class Type : uint8_t
    {
        kUnknown,   ///< Unknown
        kProvided,  ///< Provided
        kRequired,  ///< Required
    } type{Type::kUnknown};
    /// @brief Instance FQN name
    ara::core::String name{};
    /// @brief Service identifier
    ServiceId serviceId{};
    /// @brief Instance identifier
    InstanceId instanceId{};
    /// @brief Version type
    struct Version
    {
        /// @brief Major version
        MajorVersion major{Message::kMajorVersionDefault};
        /// @brief Minor version
        MinorVersion minor{Message::kMinorVersionDefault};
    } version{};
    /// @brief Event group information set
    ara::core::Map< EventGroupId, std::unique_ptr< GroupInfo > > groupInfos{};
    /// @brief Event information set
    ara::core::Map< EventId, std::unique_ptr< EventInfo > > eventInfos{};
    /// @brief Method information set
    ara::core::Map< MethodId, std::unique_ptr< MethodInfo > > methodInfos{};
    /// @brief Field information set
    std::set< std::unique_ptr< FieldInfo > > fieldInfos{};

    /// @brief Get service instance identifier
    auto ID() const noexcept { return InstanceID{serviceId, instanceId}; }
    /// @brief Determine whether it is P-side service information
    auto IsProvided() const noexcept { return type == Type::kProvided; }
    /// @brief Determine whether it is R-side service information
    auto IsRequired() const noexcept { return type == Type::kRequired; }

    /// @brief Type alias -- mark enumerable
    using IsEnumerableTag = void;
    /// @brief Enumeration operation -- enumerate members of this type by passing in a function body
    /// @tparam F Incoming function body type
    /// @param[in] fun Incoming function body
    template < typename F >
    void enumerate(F& fun)  // NOLINT -- Template interface name > naming convention
    {
        fun(instanceIdentifier);
        fun(serviceId);
        fun(instanceId);
        fun(version.major);
        fun(version.minor);
    }
    /// @brief Operator -- equality comparison
    /// @param[in] other
    /// @return bool
    bool operator==(ServiceInfo const& other) const noexcept
    {
        if (!(instanceIdentifier == other.instanceIdentifier)) {
            return false;
        }
        if (!(serviceId == other.serviceId)) {
            return false;
        }
        if (!(instanceId == other.instanceId)) {
            return false;
        }
        if (!(version.major == other.version.major)) {
            return false;
        }
        if (!(version.minor == other.version.minor)) {
            return false;
        }
        return true;
    }
    /// @brief Operator -- less than comparison
    /// @param[in] other
    /// @return bool
    bool operator<(ServiceInfo const& other) const noexcept
    {
        if (!(instanceIdentifier == other.instanceIdentifier)) {
            return instanceIdentifier < other.instanceIdentifier;
        }
        if (!(serviceId == other.serviceId)) {
            return serviceId < other.serviceId;
        }
        if (!(instanceId == other.instanceId)) {
            return instanceId < other.instanceId;
        }
        if (!(version.major == other.version.major)) {
            return version.major < other.version.major;
        }
        if (!(version.minor == other.version.minor)) {
            return version.minor < other.version.minor;
        }
        return false;
    }
};
/// @brief Service instance information type -- skeleton side
struct PServiceInfo : ServiceInfo
{
    /// @brief Reference base class constructor
    using ServiceInfo::ServiceInfo;
    /// @brief Load balancing priority
    uint32_t loadBalancingPriority{};
    /// @brief Load balancing weight
    uint32_t loadBalancingWeight{};
    /// @brief Service discovery server configuration
    struct SdServerConfig
    {
        /// @brief Service offer initial behavior
        struct InitialOfferBehavior
        {
            /// @brief Service discovery initial maximum delay time (s)
            double initialDelayMaxValue{3.0};  // NOLINT -- Default value > naming convention
            /// @brief Service discovery initial minimum delay time (s)
            double initialDelayMinValue{0.0};  // NOLINT -- Default value > naming convention
            /// @brief Service discovery repetition delay time (s)
            double initialRepetitionsBaseDelay{0.01};  // NOLINT -- Default value > naming convention
            /// @brief Service discovery repetition maximum count
            uint32_t initialRepetitionsMax{3};  // NOLINT -- Default value > naming convention
        } initialOfferBehavior{};
        /// @brief Service offer message interval time (s)
        double offerCyclicDelay{1.0};  // NOLINT -- Default value > naming convention
        /// @brief Response allowed delay for received multicast request
        struct RequestResponseDelay
        {
            /// @brief Maximum delay (s)
            double maxValue{10.0};  // NOLINT -- Default value > naming convention
            /// @brief Minimum delay (s)
            double minValue{0.0};  // NOLINT -- Default value > naming convention
        } requestResponseDelay{};
        /// @brief Service offer message time-to-live (s)
        uint32_t serviceOfferTimeToLive{0xFFFFFFFF};  // NOLINT -- Default value > naming convention
    } sdServerConfig{};
    /// @brief Service data accumulation size upper limit (UDP, bytes)
    uint32_t udpCollectionBufferSizeThreshold{0x00010000};  // NOLINT -- Default value > naming convention
    /// @brief Thread pool size
    size_t threadPoolSize{};
};
/// @brief Service instance information type -- proxy side
struct RServiceInfo : ServiceInfo
{
    /// @brief Reference base class constructor
    using ServiceInfo::ServiceInfo;
    /// @brief Blacklist version set
    ara::core::Vector< Version > blacklistedVersions{};
    /// @brief Required minimum minor version
    MinorVersion requiredMinorVersion{Message::kMinorVersionAny};
    /// @brief Service discovery client configuration
    struct SdClientConfig
    {
        /// @brief Service find initial behavior
        struct InitialFindBehavior
        {
            /// @brief Service discovery initial maximum delay time (s)
            double initialDelayMaxValue{3.0};  // NOLINT -- Default value > naming convention
            /// @brief Service discovery initial minimum delay time (s)
            double initialDelayMinValue{0.0};  // NOLINT -- Default value > naming convention
            /// @brief Service discovery repetition delay time (s)
            double initialRepetitionsBaseDelay{0.01};  // NOLINT -- Default value > naming convention
            /// @brief Service discovery repetition maximum count
            uint32_t initialRepetitionsMax{3};  // NOLINT -- Default value > naming convention
        } initialFindBehavior{};
        /// @brief Service find message time-to-live (s)
        uint32_t serviceFindTimeToLive{0xFFFFFFFF};  // NOLINT -- Default value > naming convention
    } sdClientConfig{};
    /// @brief Enumeration -- service discovery version driven behavior
    enum class VersionDrivenFindBehavior : uint8_t
    {
        kExactOrAnyMinorVersion,  ///< Exact or any minor version
        kMinimumMinorVersion,     ///< Minimum minor version
    } versionDrivenFindBehavior{VersionDrivenFindBehavior::kExactOrAnyMinorVersion};
    /// @brief Service data accumulation size upper limit (UDP, bytes)
    uint32_t udpCollectionBufferSizeThreshold{0x00000100};  // NOLINT -- Default value > naming convention
    /// @brief Thread pool size
    size_t threadPoolSize{};
};
/// @brief Event group information type
struct GroupInfo
{
    /// @brief Type alias -- event group identifier
    using EventGroupId = Message::EventGroupId;
    /// @brief Type alias -- event identifier
    using EventId = Message::MethodId;

    /// @brief Constructor
    /// @param[in] info Service information
    explicit GroupInfo(ServiceInfo& info) noexcept : serviceInfo{info} {}
    /// @brief Service information
    ServiceInfo& serviceInfo;
    /// @brief Event group identifier
    EventGroupId groupId{};
    /// @brief Event information set
    ara::core::Map< EventId, EventInfo* > eventInfos{};

    /// @brief Type alias -- mark enumerable
    using IsEnumerableTag = void;
    /// @brief Enumeration operation -- enumerate members of this type by passing in a function body
    /// @tparam F Incoming function body type
    /// @param[in] fun Incoming function body
    template < typename F >
    void enumerate(F& fun)  // NOLINT -- Template interface name > naming convention
    {
        fun(serviceInfo);
        fun(groupId);
    }
    /// @brief Operator -- equality comparison
    /// @param[in] other
    /// @return bool
    bool operator==(GroupInfo const& other) const noexcept
    {
        if (!(serviceInfo == other.serviceInfo)) {
            return false;
        }
        if (!(groupId == other.groupId)) {
            return false;
        }
        return true;
    }
    /// @brief Operator -- less than comparison
    /// @param[in] other
    /// @return bool
    bool operator<(GroupInfo const& other) const noexcept
    {
        if (!(serviceInfo == other.serviceInfo)) {
            return serviceInfo < other.serviceInfo;
        }
        if (!(groupId == other.groupId)) {
            return groupId < other.groupId;
        }
        return false;
    }
};
/// @brief Event group information type -- skeleton side
struct PGroupInfo : GroupInfo
{
    /// @brief Reference base class constructor
    using GroupInfo::GroupInfo;
    /// @brief Event group multicast threshold (same as unicast without multicast address)
    uint32_t multicastThreshold{};
    /// @brief Service discovery server event group configuration
    struct SdServerEventGroupTimingConfig
    {
        /// @brief Response allowed delay for received multicast request
        struct RequestResponseDelay
        {
            /// @brief Maximum delay (s)
            double maxValue{10.0};  // NOLINT -- Default value > naming convention
            /// @brief Minimum delay (s)
            double minValue{0.0};  // NOLINT -- Default value > naming convention
        } requestResponseDelay{};
    } sdServerEventGroupTimingConfig{};
};
/// @brief Event group information type -- proxy side
struct RGroupInfo : GroupInfo
{
    /// @brief Reference base class constructor
    using GroupInfo::GroupInfo;
    /// @brief Service discovery client event group configuration
    struct SdClientEventGroupTimingConfig
    {
        /// @brief Event group subscription timeout (s)
        double subscribeEventGroupRetryDelay{5.0};  // NOLINT -- Default value > naming convention
        /// @brief Event group subscription retry count
        uint32_t subscribeEventGroupRetryMax{3};  // NOLINT -- Default value > naming convention
        /// @brief Event group subscription time-to-live (s)
        uint32_t timeToLive{0xFFFFFFFF};  // NOLINT -- Default value > naming convention
        /// @brief Response allowed delay for received multicast request
        struct RequestResponseDelay
        {
            /// @brief Maximum delay (s)
            double maxValue{10.0};  // NOLINT -- Default value > naming convention
            /// @brief Minimum delay (s)
            double minValue{0.0};  // NOLINT -- Default value > naming convention
        } requestResponseDelay{};
    } sdClientEventGroupTimingConfig{};
};
/// @brief Event information type
struct EventInfo
{
    /// @brief Type alias -- event group identifier
    using EventGroupId = Message::EventGroupId;
    /// @brief Type alias -- event identifier
    using EventId = Message::MethodId;
    /// @brief Type alias -- transport protocol
    using TransportProtocol = Message::TransportProtocol;
    /// @brief Type alias -- session handling mode
    using SessionHandling = Message::SessionHandling;

    /// @brief Constructor
    /// @param[in] info Service information
    explicit EventInfo(ServiceInfo& info) noexcept : serviceInfo{info} {}
    /// @brief Service information
    ServiceInfo& serviceInfo;
    /// @brief Event identifier
    EventId eventId{};
    /// @brief Event maximum segment length (UDP)
    uint32_t maximumSegmentLength{10};  // NOLINT -- Default value > naming convention
    /// @brief Event separation time (UDP, s)
    double separationTime{0.01};  // NOLINT -- Default value > naming convention
    /// @brief Enumeration -- serialization mode
    enum class Serializer
    {
        kSomeip,       ///< SOME/IP serialization mode
        kSignalBased,  ///< Signal-based serialization mode
    } serializer{Serializer::kSomeip};
    /// @brief Event transport protocol
    TransportProtocol transportProtocol{Message::kTransportProtocolUnknown};
    /// @brief Event group information set
    ara::core::Map< EventGroupId, GroupInfo* > groupInfos{};
    /// @brief Field information (notifier)
    FieldInfo* fieldInfo{};
    /// @brief Session handling mode
    SessionHandling sessionHandling{Message::kSessionHandlingUnknown};

    /// @brief Type alias -- mark enumerable
    using IsEnumerableTag = void;
    /// @brief Enumeration operation -- enumerate members of this type by passing in a function body
    /// @tparam F Incoming function body type
    /// @param[in] fun Incoming function body
    template < typename F >
    void enumerate(F& fun)  // NOLINT -- Template interface name > naming convention
    {
        fun(serviceInfo);
        fun(eventId);
    }
    /// @brief Operator -- equality comparison
    /// @param[in] other
    /// @return bool
    bool operator==(EventInfo const& other) const noexcept
    {
        if (!(serviceInfo == other.serviceInfo)) {
            return false;
        }
        if (!(eventId == other.eventId)) {
            return false;
        }
        return true;
    }
    /// @brief Operator -- less than comparison
    /// @param[in] other
    /// @return bool
    bool operator<(EventInfo const& other) const noexcept
    {
        if (!(serviceInfo == other.serviceInfo)) {
            return serviceInfo < other.serviceInfo;
        }
        if (!(eventId == other.eventId)) {
            return eventId < other.eventId;
        }
        return false;
    }
};
/// @brief Event information type -- skeleton side
struct PEventInfo : EventInfo
{
    /// @brief Reference base class constructor
    using EventInfo::EventInfo;
    /// @brief Time data accumulation property
    struct CollectionProps
    {
        /// @brief Event data accumulation time upper limit (UDP, s)
        double udpCollectionBufferTimeout{0.01};  // NOLINT -- Default value > naming convention
        /// @brief Enumeration -- event data accumulation trigger mode (UDP)
        enum class UdpCollectionTrigger : uint8_t
        {
            kAlways,  ///< Accumulation enabled
            kNever,   ///< Accumulation disabled
        } udpCollectionTrigger{UdpCollectionTrigger::kNever};
    } collectionProps{};
};
/// @brief Event information type -- proxy side
struct REventInfo : EventInfo
{
    /// @brief Reference base class constructor
    using EventInfo::EventInfo;
};
/// @brief Method information type
struct MethodInfo
{
    /// @brief Type alias -- method identifier
    using MethodId = Message::MethodId;
    /// @brief Type alias -- method type
    using MessageType = Message::MessageType;
    /// @brief Type alias -- transport protocol
    using TransportProtocol = Message::TransportProtocol;
    /// @brief Type alias -- session handling mode
    using SessionHandling = Message::SessionHandling;

    /// @brief Constructor
    /// @param[in] info Service information
    explicit MethodInfo(ServiceInfo& info) noexcept : serviceInfo{info} {}
    /// @brief Service information
    ServiceInfo& serviceInfo;
    /// @brief Method type
    MessageType type{Message::kMessageTypeUnknown};
    /// @brief Method identifier
    MethodId methodId{};
    /// @brief Method request maximum segment length (UDP)
    uint32_t maximumSegmentLengthRequest{1};  // NOLINT -- Default value > naming convention
    /// @brief Method response maximum segment length (UDP)
    uint32_t maximumSegmentLengthResponse{1};  // NOLINT -- Default value > naming convention
    /// @brief Method request separation time (UDP, s)
    double separationTimeRequest{1.0};  // NOLINT -- Default value > naming convention
    /// @brief Method response separation time (UDP, s)
    double separationTimeResponse{1.0};  // NOLINT -- Default value > naming convention
    /// @brief Method transport protocol
    TransportProtocol transportProtocol{Message::kTransportProtocolUnknown};
    /// @brief Field information (setter/getter)
    FieldInfo* fieldInfo{};

    /// @brief Type alias -- mark enumerable
    using IsEnumerableTag = void;
    /// @brief Enumeration operation -- enumerate members of this type by passing in a function body
    /// @tparam F Incoming function body type
    /// @param[in] fun Incoming function body
    template < typename F >
    void enumerate(F& fun)  // NOLINT -- Template interface name > naming convention
    {
        fun(serviceInfo);
        fun(methodId);
    }
    /// @brief Operator -- equality comparison
    /// @param[in] other
    /// @return bool
    bool operator==(MethodInfo const& other) const noexcept
    {
        if (!(serviceInfo == other.serviceInfo)) {
            return false;
        }
        if (!(methodId == other.methodId)) {
            return false;
        }
        return true;
    }
    /// @brief Operator -- less than comparison
    /// @param[in] other
    /// @return bool
    bool operator<(MethodInfo const& other) const noexcept
    {
        if (!(serviceInfo == other.serviceInfo)) {
            return serviceInfo < other.serviceInfo;
        }
        if (!(methodId == other.methodId)) {
            return methodId < other.methodId;
        }
        return false;
    }
};
/// @brief Method information type -- skeleton side
struct PMethodInfo : MethodInfo
{
    /// @brief Reference base class constructor
    using MethodInfo::MethodInfo;
    /// @brief Method data accumulation property
    struct CollectionProps
    {
        /// @brief Method data accumulation time upper limit (UDP, s)
        double udpCollectionBufferTimeout{0.01};  // NOLINT -- Default value > naming convention
        /// @brief Enumeration -- method data accumulation trigger mode (UDP)
        enum class UdpCollectionTrigger : uint8_t
        {
            kAlways,  ///< Accumulation enabled
            kNever,   ///< Accumulation disabled
        } udpCollectionTrigger{UdpCollectionTrigger::kNever};
    } collectionProps{};
};
/// @brief Method information type -- proxy side
struct RMethodInfo : MethodInfo
{
    /// @brief Reference base class constructor
    using MethodInfo::MethodInfo;
    /// @brief Method data accumulation property
    struct CollectionProps
    {
        /// @brief Method data accumulation time upper limit (UDP, s)
        double udpCollectionBufferTimeout{0.01};  // NOLINT -- Default value > naming convention
        /// @brief Enumeration -- method data accumulation trigger mode (UDP)
        enum class UdpCollectionTrigger : uint8_t
        {
            kAlways,  ///< Accumulation enabled
            kNever,   ///< Accumulation disabled
        } udpCollectionTrigger{UdpCollectionTrigger::kNever};
    } collectionProps{};
};
/// @brief Field information type
struct FieldInfo
{
    /// @brief Constructor
    /// @param[in] info Service information
    explicit FieldInfo(ServiceInfo& info) noexcept : serviceInfo{info} {}
    /// @brief Service information
    ServiceInfo& serviceInfo;
    /// @brief Notifier information
    EventInfo* notifierInfo{};
    /// @brief Setter information
    MethodInfo* setterInfo{};
    /// @brief Getter information
    MethodInfo* getterInfo{};
};
/// @brief Field information type -- skeleton side
struct PFieldInfo : FieldInfo
{
    /// @brief Reference base class constructor
    using FieldInfo::FieldInfo;
};
/// @brief Field information type -- proxy side
struct RFieldInfo : FieldInfo
{
    /// @brief Reference base class constructor
    using FieldInfo::FieldInfo;
};
}  // namespace nsomeip
}  // namespace internal
}  // namespace com
}  // namespace ara

namespace ara {
namespace com {
namespace internal {
namespace format {
/// @brief Format message type
template <>
struct Formatter< ara::core::String, nsi_message_t >
{
    /// @brief Convert to string
    /// @param[in, out] r String reference
    /// @param[in] m Message
    /// @return String reference
    static ara::core::String& ToString(ara::core::String& r, nsi_message_t const& m) noexcept
    {
        return ara::com::internal::nsomeip::types::ToString(r, m);
    }
};
}  // namespace format
}  // namespace internal
}  // namespace com
}  // namespace ara
#endif
