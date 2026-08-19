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
/// @file       public_message.h
/// @brief      This file provides DOIP message packet definition related
/// @details
/// @date       2023-10-08
/// @author     zhanglipeng
/// @version    1.2.0
///
/// ================================================================

#ifndef ISOFT_DOIP_PUBLIC_MESSAGE_H_
#define ISOFT_DOIP_PUBLIC_MESSAGE_H_
#include <ara/core/vector.h>

#include "isoft/doip/public_def.h"

namespace isoft {
namespace doip {

/// @brief Byte offset
std::size_t const kByteOffset{8U};

/// @brief Byte mask
uint8_t const kByteMask{0xFFU};

/// @brief Convert an unsigned integer to a byte array
/// @tparam T Data type
/// @param[in] unsignedInteger Data
/// @param[out] vector Conversion result
template < typename T >
void ToByteVector(T unsignedInteger, ara::core::Vector< uint8_t > &vector)
{
    const auto cCastedByteOffset{static_cast< T >(kByteOffset)};
    const std::size_t cUnsignedIntegerSize{sizeof(T)};

    for (std::size_t i{0U}; i < cUnsignedIntegerSize; ++i) {
        auto ucByte{static_cast< uint8_t >(unsignedInteger & kByteMask)};
        std::ignore     = vector.insert(vector.cbegin(), ucByte);
        unsignedInteger = unsignedInteger >> cCastedByteOffset;
    }
}

/// @brief Convert unsigned integer in a byte array
/// @tparam Data type
/// @param vector Byte array
/// @param[in,out] offset Byte array offset
/// @return Conversion result
template < typename T >
T ToUnsignedInteger(const ara::core::Vector< uint8_t > &vector, std::size_t &offset)
{
    const auto cCastedByteOffset{static_cast< T >(kByteOffset)};
    const std::size_t cUnsignedIntegerSize{sizeof(T)};

    auto ulOffsetUpperBound{static_cast< std::size_t >(offset + cUnsignedIntegerSize)};

    T result{vector[offset]};

    for (++offset; offset < ulOffsetUpperBound; ++offset) {
        result = result << cCastedByteOffset;
        result += vector[offset];
    }

    return result;
}

/// @brief Convert enumeration in a byte array
/// @tparam Data type
/// @param vector Byte array
/// @param [in,out] offset Byte array offset
/// @return Conversion result
template < typename T >
T ToEnum(const ara::core::Vector< uint8_t > &vector, std::size_t &offset)
{
    auto resultInt{ToUnsignedInteger< typename std::underlying_type< T >::type >(vector, offset)};
    auto result{static_cast< T >(resultInt)};

    return result;
}

/// @brief Convert a byte vector to a byte array
/// @tparam array size
/// @param vector Byte array
/// @param [in,out] offset Byte array offset
/// @return Conversion result
template < std::size_t n >
ara::core::Array< uint8_t, n > ToByteArray(const ara::core::Vector< uint8_t > &vector, std::size_t &offset)
{
    auto beginItr = vector.cbegin() + static_cast< int64_t >(offset);
    ara::core::Array< uint8_t, n > result{};
    std::ignore = std::copy_n(beginItr, n, result.begin());
    offset += n;

    return result;
}

/// @brief Convert byte array to string
/// @tparam string size
/// @param vector Byte array
/// @param[in,out] offset Byte array offset
/// @return Conversion result
template < std::size_t n >
ara::core::String ToString(const ara::core::Vector< uint8_t > &vector, std::size_t &offset)
{
    auto beginItr{vector.cbegin() + static_cast< int64_t >(offset)};
    offset += n;
    auto endItr{vector.cbegin() + static_cast< int64_t >(offset)};
    ara::core::String result(beginItr, endItr);

    return result;
}

/// @brief DOIP packet header
class Header
{
public:
    /// @brief Protocol version
    uint8_t protocolVersion_{0U};  // NOLINT

    /// @brief Inverse protocol version
    uint8_t inverseprotocolVersion_{0U};  // NOLINT

    /// @brief Protocol number
    PayloadType payloadType_{PayloadType::kDiagMessage};  // NOLINT

    /// @brief Packet body length
    uint32_t payloadLength_{0U};  // NOLINT

public:
    /// @brief Serialize
    /// @param[out] serializedMessage
    void Serialize(ara::core::Vector< uint8_t > &serializedMessage) const;

    /// @brief Deserialize
    /// @param[in] serializedMessage
    /// @return true success
    bool Deserialize(ara::core::Vector< uint8_t > const &serializedMessage);
};

/// @brief Generic header negative response
class GenericNack
{
public:
    /// @brief Header negative response code
    GenericNackType nck_{GenericNackType::kUnsupportedPayloadType};  // NOLINT

public:
    /// @brief Serialize
    /// @param[out] serializedMessage
    void Serialize(ara::core::Vector< uint8_t > &serializedMessage) const;

    /// @brief Deserialize
    /// @param[in] serializedMessage
    /// @return true success
    bool Deserialize(ara::core::Vector< uint8_t > const &serializedMessage);
};

/// @brief Alive check request
/// @code{.isoft}
/// export_level=/DoIP
/// @endcode
class AliveCheckRequest
{
public:
    /// @brief Serialize
    /// @code{.isoft}
    /// export_level=/DoIP
    /// @endcode
    /// @param[out] serializedMessage
    void Serialize(ara::core::Vector< uint8_t > &serializedMessage) const;

    /// @brief Deserialize
    /// @code{.isoft}
    /// export_level=/DoIP
    /// @endcode
    /// @param[in] serializedMessage
    /// @return true success
    bool Deserialize(ara::core::Vector< uint8_t > const &serializedMessage);
};

/// @brief Alive check response
/// @code{.isoft}
/// export_level=/DoIP
/// @endcode
class AliveCheckResponse
{
public:
    /// @brief sa
    uint16_t sourceAddress_{0U};  // NOLINT

public:
    /// @brief Serialize
    /// @code{.isoft}
    /// export_level=/DoIP
    /// @endcode
    /// @param[out] serializedMessage
    void Serialize(ara::core::Vector< uint8_t > &serializedMessage) const;

    /// @brief Deserialize
    /// @code{.isoft}
    /// export_level=/DoIP
    /// @endcode
    /// @param[in] serializedMessage
    /// @return true success
    bool Deserialize(ara::core::Vector< uint8_t > const &serializedMessage);
};

/// @brief Diagnostic message positive response
/// @code{.isoft}
/// export_level=/DoIP
/// @endcode
class DiagMessageAck
{
public:
    /// @brief sa
    uint16_t sourceAddress_{0U};  // NOLINT

    /// @brief ta
    uint16_t targetAddress_{0U};  // NOLINT

    /// @brief Positive response code
    uint8_t ack_{0U};  // NOLINT

    /// @brief Diagnostic message
    ara::core::Vector< uint8_t > uds_{};  // NOLINT

public:
    /// @brief Serialize
    /// @code{.isoft}
    /// export_level=/DoIP
    /// @endcode
    /// @param[out] serializedMessage
    void Serialize(ara::core::Vector< uint8_t > &serializedMessage) const;

    /// @brief Deserialize
    /// @code{.isoft}
    /// export_level=/DoIP
    /// @endcode
    /// @param[in] serializedMessage
    /// @return true success
    bool Deserialize(ara::core::Vector< uint8_t > const &serializedMessage);
};

/// @brief Diagnostic message negative response
/// @code{.isoft}
/// export_level=/DoIP
/// @endcode
class DiagMessageNack
{
public:
    /// @brief sa
    uint16_t sourceAddress_{0U};  // NOLINT

    /// @brief ta
    uint16_t targetAddress_{0U};  // NOLINT

    /// @brief Negative response code
    DiagNackType nck_{DiagNackType::kUnknownNetwork};  // NOLINT

    /// @brief Diagnostic message
    ara::core::Vector< uint8_t > uds_{};  // NOLINT

public:
    /// @brief Serialize
    /// @code{.isoft}
    /// export_level=/DoIP
    /// @endcode
    /// @param[out] serializedMessage
    void Serialize(ara::core::Vector< uint8_t > &serializedMessage) const;

    /// @brief Deserialize
    /// @code{.isoft}
    /// export_level=/DoIP
    /// @endcode
    /// @param[in] serializedMessage
    /// @return true success
    bool Deserialize(ara::core::Vector< uint8_t > const &serializedMessage);
};

/// @brief Diagnostic message
/// @code{.isoft}
/// export_level=/DoIP
/// @endcode
class DiagMessage
{
public:
    /// @brief sa
    uint16_t sourceAddress_{0U};  // NOLINT

    /// @brief ta
    uint16_t targetAddress_{0U};  // NOLINT

    /// @brief Diagnostic message
    ara::core::Vector< uint8_t > uds_{};  // NOLINT

public:
    /// @brief Serialize
    /// @code{.isoft}
    /// export_level=/DoIP
    /// @endcode
    /// @param[out] serializedMessage
    void Serialize(ara::core::Vector< uint8_t > &serializedMessage) const;

    /// @brief Deserialize
    /// @code{.isoft}
    /// export_level=/DoIP
    /// @endcode
    /// @param[in] serializedMessage
    /// @return true success
    bool Deserialize(ara::core::Vector< uint8_t > const &serializedMessage);
};

/// @brief Entity status request
/// @code{.isoft}
/// export_level=/DoIP
/// @endcode
class EntityStatusRequest
{
public:
    /// @brief Serialize
    /// @code{.isoft}
    /// export_level=/DoIP
    /// @endcode
    /// @param[out] serializedMessage
    void Serialize(ara::core::Vector< uint8_t > &serializedMessage) const;

    /// @brief Deserialize
    /// @code{.isoft}
    /// export_level=/DoIP
    /// @endcode
    /// @param[in] serializedMessage
    /// @return true success
    bool Deserialize(ara::core::Vector< uint8_t > const &serializedMessage);
};

/// @brief Entity status response
/// @code{.isoft}
/// export_level=/DoIP
/// @endcode
class EntityStatusResponse
{
public:
    /// @brief DOIP node type
    NodeType entityType_{NodeType::kDoipGateway};  // NOLINT

    /// @brief Maximum supported socket connections
    uint8_t maxOpenSockets_{0U};  // NOLINT

    /// @brief Current active socket connections
    uint8_t currentlyOpenSocket_{0U};  // NOLINT

    /// @brief Maximum size of DOIP packet that can be processed at one time
    uint32_t maxDataSize_{0U};  // NOLINT

public:
    /// @brief Serialize
    /// @code{.isoft}
    /// export_level=/DoIP
    /// @endcode
    /// @param[out] serializedMessage
    void Serialize(ara::core::Vector< uint8_t > &serializedMessage) const;

    /// @brief Deserialize
    /// @code{.isoft}
    /// export_level=/DoIP
    /// @endcode
    /// @param[in] serializedMessage
    /// @return true success
    bool Deserialize(ara::core::Vector< uint8_t > const &serializedMessage);
};

/// @brief Power mode request
/// @code{.isoft}
/// export_level=/DoIP
/// @endcode
class PowerModeRequest
{
public:
    /// @brief Serialize
    /// @code{.isoft}
    /// export_level=/DoIP
    /// @endcode
    /// @param[out] serializedMessage
    void Serialize(ara::core::Vector< uint8_t > &serializedMessage) const;

    /// @brief Deserialize
    /// @code{.isoft}
    /// export_level=/DoIP
    /// @endcode
    /// @param[in] serializedMessage
    /// @return true success
    bool Deserialize(ara::core::Vector< uint8_t > const &serializedMessage);
};

/// @brief Power mode response
/// @code{.isoft}
/// export_level=/DoIP
/// @endcode
class PowerModeResponse
{
public:
    /// @brief Vehicle power mode
    PowerMode powerMode_{PowerMode::kNotReady};  // NOLINT

public:
    /// @brief Serialize
    /// @code{.isoft}
    /// export_level=/DoIP
    /// @endcode
    /// @param[out] serializedMessage
    void Serialize(ara::core::Vector< uint8_t > &serializedMessage) const;

    /// @brief Deserialize
    /// @code{.isoft}
    /// export_level=/DoIP
    /// @endcode
    /// @param[in] serializedMessage
    /// @return true success
    bool Deserialize(ara::core::Vector< uint8_t > const &serializedMessage);
};

/// @brief Route activation request
/// @code{.isoft}
/// export_level=/DoIP
/// @endcode
class RoutingActivationRequest
{
public:
    /// @brief Client logical address
    uint16_t sourceAddress_{0U};  // NOLINT

    /// @brief Route activation type
    RoutingActivationType activationType_{RoutingActivationType::kRoutingActivationType_Default};  // NOLINT

    /// @brief ISO reserved field
    uint32_t isoReserved_{0x00000000U};  // NOLINT

    /// @brief OEM use field
    uint32_t oemSpecificData_{0U};  // NOLINT

public:
    /// @brief Serialize
    /// @code{.isoft}
    /// export_level=/DoIP
    /// @endcode
    /// @param[out] serializedMessage
    void Serialize(ara::core::Vector< uint8_t > &serializedMessage) const;

    /// @brief Deserialize
    /// @code{.isoft}
    /// export_level=/DoIP
    /// @endcode
    /// @param[in] serializedMessage
    /// @return true success
    bool Deserialize(ara::core::Vector< uint8_t > const &serializedMessage);
};

/// @brief Route activation response
/// @code{.isoft}
/// export_level=/DoIP
/// @endcode
class RoutingActivationResponse
{
public:
    /// @brief Client logical address
    uint16_t clientLogicalAddress_{0U};  // NOLINT

    /// @brief Server logical address
    uint16_t serverLogicalAddress_{0U};  // NOLINT

    /// @brief Route activation return operation code
    RoutingActivationResponseType responseCode_{RoutingActivationResponseType::kSuccessful};  // NOLINT

    /// @brief ISO reserved field
    uint32_t isoReserved_{0x00000000U};  // NOLINT

    /// @brief OEM use field
    uint32_t oemSpecificData_{0U};  // NOLINT

public:
    /// @brief Serialize
    /// @code{.isoft}
    /// export_level=/DoIP
    /// @endcode
    /// @param[out] serializedMessage
    void Serialize(ara::core::Vector< uint8_t > &serializedMessage) const;

    /// @brief Deserialize
    /// @code{.isoft}
    /// export_level=/DoIP
    /// @endcode
    /// @param[in] serializedMessage
    /// @return true success
    bool Deserialize(ara::core::Vector< uint8_t > const &serializedMessage);
};

/// @brief Vehicle announcement request
/// @code{.isoft}
/// export_level=/DoIP
/// @endcode
class VehicleIdRequest
{
public:
    /// @brief Serialize
    /// @code{.isoft}
    /// export_level=/DoIP
    /// @endcode
    /// @param[out] serializedMessage
    void Serialize(ara::core::Vector< uint8_t > &serializedMessage) const;

    /// @brief Deserialize
    /// @code{.isoft}
    /// export_level=/DoIP
    /// @endcode
    /// @param[in] serializedMessage
    /// @return true success
    bool Deserialize(ara::core::Vector< uint8_t > const &serializedMessage);
};

/// @brief Vehicle announcement request with VIN
/// @code{.isoft}
/// export_level=/DoIP
/// @endcode
class VinVehicleIdRequest
{
public:
    VinValue vin_{};  // NOLINT

public:
    /// @brief Serialize
    /// @code{.isoft}
    /// export_level=/DoIP
    /// @endcode
    /// @param[out] serializedMessage
    void Serialize(ara::core::Vector< uint8_t > &serializedMessage) const;

    /// @brief Deserialize
    /// @code{.isoft}
    /// export_level=/DoIP
    /// @endcode
    /// @param[in] serializedMessage
    /// @return true success
    bool Deserialize(ara::core::Vector< uint8_t > const &serializedMessage);
};

/// @brief Vehicle announcement request with EID
/// @code{.isoft}
/// export_level=/DoIP
/// @endcode
class EidVehicleIdRequest
{
public:
    EidValue eid_{};  // NOLINT

public:
    /// @brief Serialize
    /// @code{.isoft}
    /// export_level=/DoIP
    /// @endcode
    /// @param[out] serializedMessage
    void Serialize(ara::core::Vector< uint8_t > &serializedMessage) const;

    /// @brief Deserialize
    /// @code{.isoft}
    /// export_level=/DoIP
    /// @endcode
    /// @param[in] serializedMessage
    /// @return true success
    bool Deserialize(ara::core::Vector< uint8_t > const &serializedMessage);
};

/// @brief Vehicle announcement response
/// @code{.isoft}
/// export_level=/DoIP
/// @endcode
class VehicleIdResponse
{
public:
    VinValue vin_{};                     // NOLINT
    uint16_t serverLogicalAddress_{0U};  // NOLINT
    EidValue eid_{};                     // NOLINT
    GidValue gid_{};                     // NOLINT
    uint8_t furtherAction_{0U};          // NOLINT
    uint8_t vinGidStatus_{0U};           // NOLINT

public:
    /// @brief Serialize
    /// @code{.isoft}
    /// export_level=/DoIP
    /// @endcode
    /// @param[out] serializedMessage
    void Serialize(ara::core::Vector< uint8_t > &serializedMessage) const;

    /// @brief Deserialize
    /// @code{.isoft}
    /// export_level=/DoIP
    /// @endcode
    /// @param[in] serializedMessage
    /// @return true success
    bool Deserialize(ara::core::Vector< uint8_t > const &serializedMessage);
};

}  // namespace doip
}  // namespace isoft

#endif  // ISOFT_DOIP_PUBLIC_MESSAGE_H_