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
/// @file       public_message.cpp
/// @brief      This file provides DOIP message packet definition related
/// @details
/// @date       2023-10-08
/// @author     zhanglipeng
/// @version    1.2.0
///
/// ================================================================

#include "isoft/doip/public_message.h"

namespace isoft {
namespace doip {

/// @brief Serialize DOIP packet header
/// @param[out] serializedMessage Serialized byte array
void Header::Serialize(ara::core::Vector< uint8_t > &serializedMessage) const
{
    ToByteVector< uint32_t >(payloadLength_, serializedMessage);  // NOLINT

    uint16_t usPayloadTypeInt{static_cast< uint16_t >(payloadType_)};  // NOLINT
    ToByteVector< uint16_t >(usPayloadTypeInt, serializedMessage);

    std::ignore = serializedMessage.insert(serializedMessage.cbegin(), inverseprotocolVersion_);  // NOLINT

    std::ignore = serializedMessage.insert(serializedMessage.cbegin(), protocolVersion_);  // NOLINT
}

/// @brief DOIP packet header deserialization
/// @param[in] serializedMessage Packet header byte array
/// @return Deserialization result
bool Header::Deserialize(ara::core::Vector< uint8_t > const &serializedMessage)
{
    if (serializedMessage.size() < kDoipHeadSize) {
        return false;
    }

    std::size_t ulOffset{0U};
    protocolVersion_ = serializedMessage[ulOffset];  // NOLINT
    ++ulOffset;

    inverseprotocolVersion_ = serializedMessage[ulOffset];  // NOLINT
    ++ulOffset;

    payloadType_ = ToEnum< PayloadType >(serializedMessage, ulOffset);  // NOLINT

    payloadLength_ = ToUnsignedInteger< uint32_t >(serializedMessage, ulOffset);  // NOLINT

    return true;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Alive check request packet serialization
/// @code{.isoft}
/// export_level=/DoIP
/// @endcode
/// @param[out] serializedMessage Serialized byte array
void AliveCheckRequest::Serialize(ara::core::Vector< uint8_t > &serializedMessage) const { serializedMessage.clear(); }

/// @brief Alive check request packet deserialization
/// @code{.isoft}
/// export_level=/DoIP
/// @endcode
/// @param[in] serializedMessage Alive check byte array
/// @return Deserialization result
bool AliveCheckRequest::Deserialize(ara::core::Vector< uint8_t > const &serializedMessage)
{
    std::ignore = serializedMessage;
    return true;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
/// @brief Alive check response packet serialization
/// @code{.isoft}
/// export_level=/DoIP
/// @endcode
/// @param[out] serializedMessage Serialized byte array
void AliveCheckResponse::Serialize(ara::core::Vector< uint8_t > &serializedMessage) const
{
    ToByteVector< uint16_t >(sourceAddress_, serializedMessage);  // NOLINT
}

/// @brief Alive check response packet deserialization
/// @code{.isoft}
/// export_level=/DoIP
/// @endcode
/// @param[in] serializedMessage Alive check byte array
/// @return Deserialization result
bool AliveCheckResponse::Deserialize(ara::core::Vector< uint8_t > const &serializedMessage)
{
    static uint8_t const kAliveCheckPackageSize{2U};
    if (serializedMessage.size() < kAliveCheckPackageSize) {
        return false;
    }

    std::size_t ulOffset{0U};
    sourceAddress_ = ToUnsignedInteger< uint16_t >(serializedMessage, ulOffset);  // NOLINT

    return true;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////

/// @brief Diagnostic positive response serialization
/// @code{.isoft}
/// export_level=/DoIP
/// @endcode
/// @param serializedMessage Serialized byte array
void DiagMessageAck::Serialize(ara::core::Vector< uint8_t > &serializedMessage) const
{
    if (!uds_.empty()) {                                                                               // NOLINT
        std::ignore = serializedMessage.insert(serializedMessage.cbegin(), uds_.begin(), uds_.end());  // NOLINT
    }

    ToByteVector< uint8_t >(ack_, serializedMessage);  // NOLINT

    ToByteVector< uint16_t >(targetAddress_, serializedMessage);  // NOLINT

    ToByteVector< uint16_t >(sourceAddress_, serializedMessage);  // NOLINT
}

/// @brief Diagnostic positive response deserialization
/// @code{.isoft}
/// export_level=/DoIP
/// @endcode
/// @param serializedMessage Byte array to be deserialized
/// @return Deserialization result
bool DiagMessageAck::Deserialize(ara::core::Vector< uint8_t > const &serializedMessage)
{
    static uint8_t const kDiagMessageAckPackageSize{5U};
    if (serializedMessage.size() < kDiagMessageAckPackageSize) {
        return false;
    }

    std::size_t ulOffset{0U};
    sourceAddress_ = ToUnsignedInteger< uint16_t >(serializedMessage, ulOffset);  // NOLINT

    targetAddress_ = ToUnsignedInteger< uint16_t >(serializedMessage, ulOffset);  // NOLINT

    ack_ = ToUnsignedInteger< uint8_t >(serializedMessage, ulOffset);  // NOLINT

    if (serializedMessage.size() > kDiagMessageAckPackageSize) {
        uds_ = ara::core::Vector< uint8_t >(serializedMessage.begin() + static_cast< int64_t >(ulOffset),
                                            serializedMessage.end());
    }

    return true;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////

/// @brief Diagnostic negative response packet serialization
/// @code{.isoft}
/// export_level=/DoIP
/// @endcode
/// @param[out] serializedMessage Serialization result
void DiagMessageNack::Serialize(ara::core::Vector< uint8_t > &serializedMessage) const
{
    if (!uds_.empty()) {                                                                               // NOLINT
        std::ignore = serializedMessage.insert(serializedMessage.cbegin(), uds_.begin(), uds_.end());  // NOLINT
    }

    uint8_t nckTemp{static_cast< uint8_t >(nck_)};  // NOLINT
    ToByteVector< uint8_t >(nckTemp, serializedMessage);

    ToByteVector< uint16_t >(targetAddress_, serializedMessage);  // NOLINT

    ToByteVector< uint16_t >(sourceAddress_, serializedMessage);  // NOLINT
}

/// @brief Diagnostic negative response deserialization
/// @code{.isoft}
/// export_level=/DoIP
/// @endcode
/// @param[in] serializedMessage Byte array to be deserialized
/// @return Deserialization result
bool DiagMessageNack::Deserialize(ara::core::Vector< uint8_t > const &serializedMessage)
{
    static uint8_t const kDiagMessageNackPackageSize{5U};
    if (serializedMessage.size() < kDiagMessageNackPackageSize) {
        return false;
    }

    std::size_t ulOffset{0U};
    sourceAddress_ = ToUnsignedInteger< uint16_t >(serializedMessage, ulOffset);  // NOLINT

    targetAddress_ = ToUnsignedInteger< uint16_t >(serializedMessage, ulOffset);  // NOLINT

    nck_ = ToEnum< DiagNackType >(serializedMessage, ulOffset);  // NOLINT

    if (serializedMessage.size() > kDiagMessageNackPackageSize) {
        uds_ = ara::core::Vector< uint8_t >(serializedMessage.begin() + static_cast< int64_t >(ulOffset),
                                            serializedMessage.end());
    }

    return true;
}
////////////////////////////////////////////////////////////////////////////////////////

/// @brief Diagnostic packet serialization
/// @code{.isoft}
/// export_level=/DoIP
/// @endcode
/// @param[out] serializedMessage Serialized byte array
void DiagMessage::Serialize(ara::core::Vector< uint8_t > &serializedMessage) const
{
    std::ignore = serializedMessage.insert(serializedMessage.cbegin(), uds_.begin(), uds_.end());  // NOLINT

    ToByteVector< uint16_t >(targetAddress_, serializedMessage);  // NOLINT

    ToByteVector< uint16_t >(sourceAddress_, serializedMessage);  // NOLINT
}

/// @brief Diagnostic packet deserialization
/// @code{.isoft}
/// export_level=/DoIP
/// @endcode
/// @param[in] serializedMessage Byte array to be deserialized
/// @return Deserialization result
bool DiagMessage::Deserialize(ara::core::Vector< uint8_t > const &serializedMessage)
{
    static uint8_t const kDiagMessagePackageSize{5U};
    if (serializedMessage.size() < kDiagMessagePackageSize) {
        return false;
    }

    std::size_t ulOffset{0U};
    sourceAddress_ = ToUnsignedInteger< uint16_t >(serializedMessage, ulOffset);  // NOLINT

    targetAddress_ = ToUnsignedInteger< uint16_t >(serializedMessage, ulOffset);  // NOLINT

    uds_ = ara::core::Vector< uint8_t >(serializedMessage.begin() + static_cast< int64_t >(ulOffset),
                                        serializedMessage.end());

    return true;
}
//////////////////////////////////////////////////////////////////////////////////////////////

/// @brief Vehicle announcement request packet serialization
/// @code{.isoft}
/// export_level=/DoIP
/// @endcode
/// @param[out] serializedMessage Serialized byte array
void EidVehicleIdRequest::Serialize(ara::core::Vector< uint8_t > &serializedMessage) const
{
    std::ignore       = serializedMessage;
    serializedMessage = ara::core::Vector< uint8_t >(eid_.begin(), eid_.end());  // NOLINT
}

/// @brief Vehicle announcement request packet deserialization
/// @code{.isoft}
/// export_level=/DoIP
/// @endcode
/// @param[in] serializedMessage Byte array to be deserialized
/// @return Whether deserialization succeeded
bool EidVehicleIdRequest::Deserialize(ara::core::Vector< uint8_t > const &serializedMessage)
{
    static uint8_t const kEidVehicleIdRequestPackageSize{6U};
    if (serializedMessage.size() < kEidVehicleIdRequestPackageSize) {
        return false;
    }

    std::size_t ulOffset{0U};
    eid_ = ToByteArray< kEidValueLength >(serializedMessage, ulOffset);  // NOLINT

    return true;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @brief Entity status request packet serialization
/// @code{.isoft}
/// export_level=/DoIP
/// @endcode
/// @param[out] serializedMessage Serialized byte array
void EntityStatusRequest::Serialize(ara::core::Vector< uint8_t > &serializedMessage) const
{
    serializedMessage.clear();
}

/// @brief Entity status request packet deserialization
/// @code{.isoft}
/// export_level=/DoIP
/// @endcode
/// @param[in] serializedMessage Byte array to be deserialized
/// @return Whether deserialization succeeded
bool EntityStatusRequest::Deserialize(ara::core::Vector< uint8_t > const &serializedMessage)
{
    std::ignore = serializedMessage;
    return true;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @brief Entity status response packet serialization
/// @code{.isoft}
/// export_level=/DoIP
/// @endcode
/// @param[out] serializedMessage Serialized byte array
void EntityStatusResponse::Serialize(ara::core::Vector< uint8_t > &serializedMessage) const
{
    if (maxDataSize_ != 0U) {                                       // NOLINT
        ToByteVector< uint32_t >(maxDataSize_, serializedMessage);  // NOLINT
    }

    ToByteVector< uint8_t >(currentlyOpenSocket_, serializedMessage);  // NOLINT

    ToByteVector< uint8_t >(maxOpenSockets_, serializedMessage);  // NOLINT

    uint8_t entityTypeTemp{static_cast< uint8_t >(entityType_)};  // NOLINT
    ToByteVector< uint8_t >(entityTypeTemp, serializedMessage);
}

/// @brief Entity status response packet deserialization
/// @code{.isoft}
/// export_level=/DoIP
/// @endcode
/// @param[in] serializedMessage Byte array to be deserialized
/// @return Whether deserialization succeeded
bool EntityStatusResponse::Deserialize(ara::core::Vector< uint8_t > const &serializedMessage)
{
    static uint8_t const kEntityStatusResponsePackageSize{3U};
    if (serializedMessage.size() < kEntityStatusResponsePackageSize) {
        return false;
    }

    std::size_t ulOffset{0U};
    entityType_ = ToEnum< NodeType >(serializedMessage, ulOffset);  // NOLINT

    maxOpenSockets_ = ToUnsignedInteger< uint8_t >(serializedMessage, ulOffset);  // NOLINT

    currentlyOpenSocket_ = ToUnsignedInteger< uint8_t >(serializedMessage, ulOffset);  // NOLINT

    if (serializedMessage.size() > kEntityStatusResponsePackageSize) {
        maxDataSize_ = ToUnsignedInteger< uint32_t >(serializedMessage, ulOffset);  // NOLINT
    }

    return true;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @brief Generic header negative response packet serialization
/// @code{.isoft}
/// export_level=/DoIP
/// @endcode
/// @param[out] serializedMessage Serialized byte array
void GenericNack::Serialize(ara::core::Vector< uint8_t > &serializedMessage) const
{
    uint8_t nckTemp{static_cast< uint8_t >(nck_)};  // NOLINT
    ToByteVector< uint8_t >(nckTemp, serializedMessage);
}

/// @brief Generic header negative response packet deserialization
/// @code{.isoft}
/// export_level=/DoIP
/// @endcode
/// @param[in] serializedMessage Byte array to be deserialized
/// @return Whether deserialization succeeded
bool GenericNack::Deserialize(ara::core::Vector< uint8_t > const &serializedMessage)
{
    static uint8_t const kGenericNackPackageSize{1U};
    if (serializedMessage.size() < kGenericNackPackageSize) {
        return false;
    }

    std::size_t ulOffset{0U};
    nck_ = ToEnum< GenericNackType >(serializedMessage, ulOffset);  // NOLINT

    return true;
}
///////////////////////////////////////////////////////////////////////////////////////////

/// @brief Power mode request packet serialization
/// @code{.isoft}
/// export_level=/DoIP
/// @endcode
/// @param[out] serializedMessage Serialized byte array
void PowerModeRequest::Serialize(ara::core::Vector< uint8_t > &serializedMessage) const { serializedMessage.clear(); }

/// @brief Power mode request packet deserialization
/// @code{.isoft}
/// export_level=/DoIP
/// @endcode
/// @param[in] serializedMessage Byte array to be deserialized
/// @return Whether deserialization succeeded
bool PowerModeRequest::Deserialize(ara::core::Vector< uint8_t > const &serializedMessage)
{
    std::ignore = serializedMessage;
    return true;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @brief Power mode response packet serialization
/// @code{.isoft}
/// export_level=/DoIP
/// @endcode
/// @param[out] serializedMessage Serialized byte array
void PowerModeResponse::Serialize(ara::core::Vector< uint8_t > &serializedMessage) const
{
    uint8_t mode{static_cast< uint8_t >(powerMode_)};  // NOLINT
    ToByteVector< uint8_t >(mode, serializedMessage);
}

/// @brief Power mode response packet deserialization
/// @code{.isoft}
/// export_level=/DoIP
/// @endcode
/// @param[in] serializedMessage Byte array to be deserialized
/// @return Whether deserialization succeeded
bool PowerModeResponse::Deserialize(ara::core::Vector< uint8_t > const &serializedMessage)
{
    static uint8_t const kPowerModeResponsePackageSize{1U};
    if (serializedMessage.size() < kPowerModeResponsePackageSize) {
        return false;
    }

    std::size_t ulOffset{0U};
    powerMode_ = ToEnum< PowerMode >(serializedMessage, ulOffset);  // NOLINT

    return true;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @brief Route activation request packet serialization
/// @code{.isoft}
/// export_level=/DoIP
/// @endcode
/// @param[out] serializedMessage Serialized byte array
void RoutingActivationRequest::Serialize(ara::core::Vector< uint8_t > &serializedMessage) const
{
    if (oemSpecificData_ != 0U) {                                       // NOLINT
        ToByteVector< uint32_t >(oemSpecificData_, serializedMessage);  // NOLINT
    }

    ToByteVector< uint32_t >(isoReserved_, serializedMessage);  // NOLINT

    uint8_t type{static_cast< uint8_t >(activationType_)};  // NOLINT
    ToByteVector< uint8_t >(type, serializedMessage);

    ToByteVector< uint16_t >(sourceAddress_, serializedMessage);  // NOLINT
}

/// @brief Route activation request packet deserialization
/// @code{.isoft}
/// export_level=/DoIP
/// @endcode
/// @param[in] serializedMessage Byte array to be deserialized
/// @return Whether deserialization succeeded
bool RoutingActivationRequest::Deserialize(ara::core::Vector< uint8_t > const &serializedMessage)
{
    static uint8_t const kRoutingActivationRequestPackageSize{7U};
    if (serializedMessage.size() < kRoutingActivationRequestPackageSize) {
        return false;
    }

    std::size_t ulOffset{0U};
    sourceAddress_ = ToUnsignedInteger< uint16_t >(serializedMessage, ulOffset);  // NOLINT

    activationType_ = ToEnum< RoutingActivationType >(serializedMessage, ulOffset);  // NOLINT

    isoReserved_ = ToUnsignedInteger< uint32_t >(serializedMessage, ulOffset);  // NOLINT

    if (serializedMessage.size() > kRoutingActivationRequestPackageSize) {
        oemSpecificData_ = ToUnsignedInteger< uint32_t >(serializedMessage, ulOffset);  // NOLINT
    }

    return true;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @brief Route activation response packet serialization
/// @code{.isoft}
/// export_level=/DoIP
/// @endcode
/// @param[out] serializedMessage Serialized byte array
void RoutingActivationResponse::Serialize(ara::core::Vector< uint8_t > &serializedMessage) const
{
    if (oemSpecificData_ != 0U) {                                       // NOLINT
        ToByteVector< uint32_t >(oemSpecificData_, serializedMessage);  // NOLINT
    }

    ToByteVector< uint32_t >(isoReserved_, serializedMessage);  // NOLINT

    uint8_t code{static_cast< uint8_t >(responseCode_)};  // NOLINT
    ToByteVector< uint8_t >(code, serializedMessage);

    ToByteVector< uint16_t >(serverLogicalAddress_, serializedMessage);  // NOLINT

    ToByteVector< uint16_t >(clientLogicalAddress_, serializedMessage);  // NOLINT
}

/// @brief Route activation response packet deserialization
/// @code{.isoft}
/// export_level=/DoIP
/// @endcode
/// @param[in] serializedMessage Byte array to be deserialized
/// @return Whether deserialization succeeded
bool RoutingActivationResponse::Deserialize(ara::core::Vector< uint8_t > const &serializedMessage)
{
    static uint8_t const kRoutingActivationResponsePackageSize{9U};
    if (serializedMessage.size() < kRoutingActivationResponsePackageSize) {
        return false;
    }

    std::size_t ulOffset{0U};
    clientLogicalAddress_ = ToUnsignedInteger< uint16_t >(serializedMessage, ulOffset);  // NOLINT

    serverLogicalAddress_ = ToUnsignedInteger< uint16_t >(serializedMessage, ulOffset);  // NOLINT

    responseCode_ = ToEnum< RoutingActivationResponseType >(serializedMessage, ulOffset);  // NOLINT

    isoReserved_ = ToUnsignedInteger< uint32_t >(serializedMessage, ulOffset);  // NOLINT

    if (serializedMessage.size() > kRoutingActivationResponsePackageSize) {
        oemSpecificData_ = ToUnsignedInteger< uint32_t >(serializedMessage, ulOffset);  // NOLINT
    }

    return true;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @brief Vehicle announcement request packet serialization
/// @code{.isoft}
/// export_level=/DoIP
/// @endcode
/// @param[out] serializedMessage Serialized byte array
void VehicleIdRequest::Serialize(ara::core::Vector< uint8_t > &serializedMessage) const { serializedMessage.clear(); }

/// @brief Vehicle announcement request packet deserialization
/// @param[in] serializedMessage Byte array to be deserialized
/// @return Whether deserialization succeeded
bool VehicleIdRequest::Deserialize(ara::core::Vector< uint8_t > const &serializedMessage)
{
    std::ignore = serializedMessage;
    return true;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @brief Vehicle announcement response packet serialization
/// @code{.isoft}
/// export_level=/DoIP
/// @endcode
/// @param[out] serializedMessage Serialized byte array
void VehicleIdResponse::Serialize(ara::core::Vector< uint8_t > &serializedMessage) const
{
    std::ignore = serializedMessage.insert(serializedMessage.cbegin(), vinGidStatus_);  // NOLINT

    std::ignore = serializedMessage.insert(serializedMessage.cbegin(), furtherAction_);  // NOLINT

    std::ignore = serializedMessage.insert(serializedMessage.cbegin(), gid_.cbegin(), gid_.cend());  // NOLINT

    std::ignore = serializedMessage.insert(serializedMessage.cbegin(), eid_.cbegin(), eid_.cend());  // NOLINT

    ToByteVector< uint16_t >(serverLogicalAddress_, serializedMessage);  // NOLINT

    std::ignore = serializedMessage.insert(serializedMessage.cbegin(), vin_.cbegin(), vin_.cend());  // NOLINT
}

/// @brief Vehicle announcement response packet deserialization
/// @code{.isoft}
/// export_level=/DoIP
/// @endcode
/// @param[in] serializedMessage Byte array to be deserialized
/// @return Whether deserialization succeeded
bool VehicleIdResponse::Deserialize(ara::core::Vector< uint8_t > const &serializedMessage)
{
    static uint8_t const kVehicleIdResponsePackageSize{33U};
    if (serializedMessage.size() < kVehicleIdResponsePackageSize) {
        return false;
    }

    std::size_t ulOffset{0U};
    vin_ = ToByteArray< kVinValueLength >(serializedMessage, ulOffset);  // NOLINT

    serverLogicalAddress_ = ToUnsignedInteger< uint16_t >(serializedMessage, ulOffset);  // NOLINT

    eid_ = ToByteArray< kEidValueLength >(serializedMessage, ulOffset);  // NOLINT

    gid_ = ToByteArray< kGidValueLength >(serializedMessage, ulOffset);  // NOLINT

    furtherAction_ = ToUnsignedInteger< uint8_t >(serializedMessage, ulOffset);  // NOLINT

    vinGidStatus_ = ToUnsignedInteger< uint8_t >(serializedMessage, ulOffset);  // NOLINT

    return true;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @brief Vehicle announcement request packet serialization
/// @code{.isoft}
/// export_level=/DoIP
/// @endcode
/// @param[out] serializedMessage Serialized byte array
void VinVehicleIdRequest::Serialize(ara::core::Vector< uint8_t > &serializedMessage) const
{
    std::ignore = serializedMessage.insert(serializedMessage.cbegin(), vin_.cbegin(), vin_.cend());  // NOLINT
}

/// @brief Vehicle announcement request packet deserialization
/// @code{.isoft}
/// export_level=/DoIP
/// @endcode
/// @param[in] serializedMessage Byte array to be deserialized
/// @return Whether deserialization succeeded
bool VinVehicleIdRequest::Deserialize(ara::core::Vector< uint8_t > const &serializedMessage)
{
    static uint8_t const kVinVehicleIdRequestPackageSize{17U};
    if (serializedMessage.size() < kVinVehicleIdRequestPackageSize) {
        return false;
    }

    std::size_t ulOffset{0U};
    vin_ = ToByteArray< kVinValueLength >(serializedMessage, ulOffset);  // NOLINT
    return true;
}

}  // namespace doip
}  // namespace isoft