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
/// @file       server_user_base.cpp
/// @brief      This file provides DOIP server network protocol abstraction related definitions
/// @details
/// @date       2023-10-08
/// @author     zhanglipeng
/// @version    1.2.0
///
/// ================================================================

#include "server_user_base.h"

#include "isoft/doip/public_message.h"
#include "public.h"
#include "public_log.h"
#include "server_impl.h"

namespace isoft {
namespace doip {

/// @brief Constructor
/// @param[in] rSize Read buffer size
/// @param[in] server DOIP server implementation class object
ServerUserBase::ServerUserBase(size_t rSize, DoIPMessageTransportType transportType, ServerImpl* server)
    : rPos_{0U}
    , rSize_{kDoipHeadSize + rSize}
    , rBuffer_(rSize_)
    , server_{server}
    , dropDataLen_{0U}
    , transportType_{transportType}
    , naiSocknbufT_{}
{
    naiSocknbufT_.len = static_cast< int32_t >(sizeof(naiSocknbufT_.storage));
    rBuffer_.resize(rSize_);
}

/// @brief Destructor
ServerUserBase::~ServerUserBase() noexcept = default;

/// @brief Generic header negative response reply
/// @param[in] nckType Negative response code
void ServerUserBase::ResponseGenericHeaderNck(GenericNackType const nckType)
{
    GenericNack nck;
    nck.nck_ = nckType;  // NOLINT

    ara::core::Vector< uint8_t > body;
    nck.Serialize(body);

    std::ignore = SendToClient(PayloadType::kGenericNegativeAcknowledgement, body);
}

/// @brief Parse packet header, extract message body
/// @param[out] type Message type
/// @param[out] dropDataLen Packet length to discard
/// @param[out] body Message body
/// @return Parse result, reference to ParsPackageHeadCode
bool ServerUserBase::_ParsPackageHeader(Header& packageHead)
{
    /// Incomplete packet header
    if (rPos_ < kDoipHeadSize) {
        return false;
    }

    /// Extract packet header
    ara::core::Vector< uint8_t > vHead{rBuffer_.begin(), rBuffer_.begin() + kDoipHeadSize};

    /// Parse packet header
    return packageHead.Deserialize(vHead);
}

/// @brief Extract packet body based on packet header information
/// @param[in] packageHead Packet header
/// @param[out] vBody
/// @return Parse result
bool ServerUserBase::_ParsPackageBody(Header const& packageHead, ara::core::Vector< uint8_t >& vBody)
{
    /// Incomplete packet body received, continue waiting for packet arrival
    if (packageHead.payloadLength_ > (rPos_ - kDoipHeadSize)) {  // NOLINT
        return false;
    }

    /// Extract packet body related information
    vBody.assign(rBuffer_.begin() + kDoipHeadSize,
                 rBuffer_.begin() + kDoipHeadSize + packageHead.payloadLength_);  // NOLINT

    return true;
}

/// @brief Check packet header validity
/// @param packageHead Packet header data
/// @param dropDataLen Data length to discard
/// @return Check result
GenericNackType ServerUserBase::_CheckPackageHeader(Header const& packageHead)
{
    ServerConfig const& config = server_->GetConfig();
    /// SWS_DM_00475 && iso-13400 Table 16
    bool const isDefaultValueForVehicleIdentificationRequest{
        packageHead.protocolVersion_ == kDoIPVersionMask                             // NOLINT
        && (packageHead.payloadType_ == PayloadType::kVehicleIdRequest               // NOLINT
            || packageHead.payloadType_ == PayloadType::kVehicleIdRequestWithEid     // NOLINT
            || packageHead.payloadType_ == PayloadType::kVehicleIdRequestWithVin)};  // NOLINT

    if (!isDefaultValueForVehicleIdentificationRequest) {
        /// Check protocol version
        if (packageHead.protocolVersion_ != config.doipProtocolVersion_) {  // NOLINT
            /// Need to close network connection
            return GenericNackType::kInvalidProtocolVersion;
        }

        /// Check inverse protocol version
        if (packageHead.inverseprotocolVersion_ != (config.doipProtocolVersion_ ^ kDoIPVersionMask)) {  // NOLINT
            /// Need to close network connection
            return GenericNackType::kInvalidProtocolVersion;
        }
    }

    /// Check if payload type is supported
    if (transportType_ == DoIPMessageTransportType::kDoIPMessageTransportType_TCP) {
        if (packageHead.payloadType_ != PayloadType::kRoutingActivationRequest  // NOLINT
            && packageHead.payloadType_ != PayloadType::kAliveCheckResponse     // NOLINT
            && packageHead.payloadType_ != PayloadType::kDiagMessage) {         // NOLINT
            return GenericNackType::kUnsupportedPayloadType;
        }

    } else {
        if (packageHead.payloadType_ != PayloadType::kVehicleIdRequest                // NOLINT
            && packageHead.payloadType_ != PayloadType::kVehicleIdRequestWithEid      // NOLINT
            && packageHead.payloadType_ != PayloadType::kVehicleIdRequestWithVin      // NOLINT
            && packageHead.payloadType_ != PayloadType::kDoipEntityStatusRequest      // NOLINT
            && packageHead.payloadType_ != PayloadType::kDiagPowerModeInfoRequest) {  // NOLINT
            return GenericNackType::kUnsupportedPayloadType;
        }
    }

    /// Check message length
    if (packageHead.payloadLength_ > (config.requestMaxBytes)) {  // NOLINT
        return GenericNackType::kToolLargeMessage;
    }

    /// Check message length corresponding to payload type
    if ((packageHead.payloadType_ == PayloadType::kVehicleIdRequest && packageHead.payloadLength_ != 0U)       // NOLINT
        || (packageHead.payloadType_ == PayloadType::kVehicleIdRequestWithEid                                  // NOLINT
            && packageHead.payloadLength_ != 6U)                                                               // NOLINT
        || (packageHead.payloadType_ == PayloadType::kVehicleIdRequestWithVin                                  // NOLINT
            && packageHead.payloadLength_ != 17U)                                                              // NOLINT
        || (packageHead.payloadType_ == PayloadType::kRoutingActivationRequest                                 // NOLINT
            && packageHead.payloadLength_ != 11U                                                               // NOLINT
            && packageHead.payloadLength_ != 7U)                                                               // NOLINT
        || (packageHead.payloadType_ == PayloadType::kAliveCheckResponse && packageHead.payloadLength_ != 2U)  // NOLINT
        || (packageHead.payloadType_ == PayloadType::kDoipEntityStatusRequest                                  // NOLINT
            && packageHead.payloadLength_ != 0U)                                                               // NOLINT
        || (packageHead.payloadType_ == PayloadType::kDiagPowerModeInfoRequest                                 // NOLINT
            && packageHead.payloadLength_ != 0U)                                                               // NOLINT
        || (packageHead.payloadType_ == PayloadType::kDiagMessage && packageHead.payloadLength_ < 5U)) {       // NOLINT
        /// Need to close network connection
        return GenericNackType::kInvalidPayloadLength;
    }

    return GenericNackType::kGenericNackTypeEnd;
}

/// @brief Construct DOIP packet header
/// @param[in] type Protocol number
/// @param[in] bodySize Message body
/// @param[out] vHeader Packet header byte array
void ServerUserBase::_StructurePackageHeader(PayloadType const type,
                                             uint32_t const bodySize,
                                             ara::core::Vector< uint8_t >& vHeader)
{
    ServerConfig const& config = server_->GetConfig();
    Header header;
    header.protocolVersion_        = config.doipProtocolVersion_;                     // NOLINT
    header.inverseprotocolVersion_ = config.doipProtocolVersion_ ^ kDoIPVersionMask;  // NOLINT
    header.payloadType_            = type;                                            // NOLINT
    header.payloadLength_          = bodySize;                                        // NOLINT

    header.Serialize(vHeader);
}

/// @brief Trim buffer
/// @param trimSize Trim size
size_t ServerUserBase::_TrimBuffer(size_t trimSize)
{
    if (trimSize <= 0) {
        return 0;
    }

    if (trimSize > rPos_) {
        trimSize = rPos_;
    }
    ara::core::Vector< uint8_t >::iterator it{rBuffer_.begin() + trimSize};  // NOLINT
    /// Update read buffer
    std::ignore = std::copy_n(it, rPos_ - trimSize, rBuffer_.begin());

    /// Update readable position
    rPos_ -= (trimSize);

    return trimSize;
}

bool ServerUserBase::_IsIgnoreMessage()
{
    char sourceIp[kSourceIpSize];
    std::ignore = nai_sockaddr_ntop(&naiSocknbufT_.addr, naiSocknbufT_.len, sourceIp, kSourceIpSize, NAI_ADDR_ZONE);
    int const mask{255};
    std::string item;
    std::stringstream ss(sourceIp);
    ara::core::Vector< int32_t > ipParts;
    while (std::getline(ss, item, '.')) {
        ipParts.emplace_back(std::stoi(item));
    }

    // Ensure correct division into four parts
    if (ipParts.size() != 4U) {
        LogInfo() << "ServerUserBase::_IsIgnoreMessage|1 soureIp =" << sourceIp;
        return true;
    }

    if (ipParts[3] == mask) {
        LogInfo() << "ServerUserBase::_IsIgnoreMessage|2 soureIp =" << sourceIp;
        return true;
    }

    // Check if it is a directed broadcast address (host part all ones)

    if (ipParts[1] == mask && ipParts[2] == mask && ipParts[3] == mask) {
        LogInfo() << "ServerUserBase::_IsIgnoreMessage|3 soureIp =" << sourceIp;
        return true;
    }

    // Check if it is a multicast address (Class D address, range from 224.0.0.0 to 239.255.255.255)
    int32_t const startDAddress{244};
    int32_t const endDAddress{239};
    if (ipParts[0] >= startDAddress && ipParts[0] <= endDAddress) {
        LogInfo() << "ServerUserBase::_IsIgnoreMessage|4 soureIp =" << sourceIp;
        return true;
    }

    return false;
}

}  // namespace doip
}  // namespace isoft