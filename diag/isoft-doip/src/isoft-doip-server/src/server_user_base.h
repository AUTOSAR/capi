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
/// @file       server_user_base.h
/// @brief      This file provides DOIP server network protocol abstraction related definitions
/// @details
/// @date       2023-10-08
/// @author     zhanglipeng
/// @version    1.2.0
///
/// ================================================================

#ifndef ISOFT_DOIP_SERVER_USER_BASE_H_
#define ISOFT_DOIP_SERVER_USER_BASE_H_
#include <ara/core/vector.h>
#include <nai/os/nai_socket.h>

#include "isoft/doip/public_def.h"
#include "public.h"

namespace isoft {
namespace doip {

/// @brief Forward declaration of ServerImpl
class ServerImpl;

class Header;
/// @brief DOIP user base class
class ServerUserBase
{
public:
    /// @brief Constructor
    /// @param[in] rSize Buffer size
    /// @param[in] server server
    ServerUserBase(size_t rSize, DoIPMessageTransportType transportType, ServerImpl* server);

    /// @brief Destructor
    virtual ~ServerUserBase() noexcept;

    /// @brief Copy constructor
    /// @param[in] other
    ServerUserBase(ServerUserBase const& other) = delete;

    /// @brief Assignment operator
    /// @param[in] other
    /// @return ServerUserBase&
    ServerUserBase& operator=(ServerUserBase const& other) = delete;

    /// @brief Move constructor
    /// @param[in] other
    ServerUserBase(ServerUserBase&& other) = delete;

    /// @brief Move assignment operator
    /// @param[in] other
    /// @return ServerUserBase&
    ServerUserBase& operator=(ServerUserBase&& other) = delete;

public:
    /// @brief Reset read buffer
    void ResetBuffer() { rPos_ = 0U; }

    /// @brief Reply generic header negative response
    /// @param[in] nckType Error code
    void ResponseGenericHeaderNck(GenericNackType const nckType);

    /// @brief Get transport type of DOIP message
    /// @return tcp || udp
    DoIPMessageTransportType GetTransportType() { return transportType_; }

    /// @brief Get connection unique identifier
    /// @return Identifier ID
    virtual int32_t GetFd() { return 0; }

    /// @brief Connection state change
    /// @param[in] state State
    virtual void ChangeConnectionState(DoipConnectionState const state) { std::ignore = state; }

    /// @brief Abstract interface for sending data
    /// @param[in] type Message type
    /// @param[in] msg Message body
    /// @return Operation result
    virtual int32_t SendToClient(PayloadType const type, ara::core::Vector< uint8_t > const& body) = 0;

    /// @brief Set destination address for reply packet
    /// @param[in] nai_socknbuf Destination address
    void SetSockBuf(nai_socknbuf_t const& naiSocknbuf) { naiSocknbufT_ = naiSocknbuf; }

protected:
    /// @brief Parse packet header
    /// @param[out] packageHead Packet header data structure
    /// @return Parse result
    bool _ParsPackageHeader(Header& packageHead);

    /// @brief Check packet header validity
    /// @param[in] packageHead Packet header data
    /// @return Check result
    GenericNackType _CheckPackageHeader(Header const& packageHead);

    /// @brief Extract packet body based on packet header information
    /// @param[in] packageHead Packet header
    /// @param[out] vBody
    /// @return Parse result
    bool _ParsPackageBody(Header const& packageHead, ara::core::Vector< uint8_t >& vBody);

    /// @brief Construct packet header
    /// @param[in] type Protocol type
    /// @param[in] bodySize Packet body size
    /// @param[out] header Serialized byte array
    void _StructurePackageHeader(PayloadType const type,
                                 uint32_t const bodySize,
                                 ara::core::Vector< uint8_t >& vHeader);

    /// @brief Trim buffer
    /// @param trimSize Trim size
    size_t _TrimBuffer(size_t trimSize);

    /// @brief Whether to ignore received message (source IP address may be broadcast or multicast address)
    /// @return Validation result true: ignore
    bool _IsIgnoreMessage();

protected:
    /// @brief Readable position
    size_t rPos_;  /// NOLINT

    /// @brief Read buffer size
    size_t rSize_;  /// NOLINT

    /// @brief Read buffer
    ara::core::Vector< uint8_t > rBuffer_;  /// NOLINT

    /// @brief DOIP service instance
    ServerImpl* server_;  /// NOLINT

    /// Data length to discard
    size_t dropDataLen_;  /// NOLINT

    /// @brief Transport protocol type
    DoIPMessageTransportType transportType_;  /// NOLINT

    /// @brief Record peer address for sending data
    nai_socknbuf_t naiSocknbufT_;  /// NOLINT

    /// @brief Source IP size
    static const uint8_t kSourceIpSize{16U};
};

}  // namespace doip
}  // namespace isoft

#endif  // ISOFT_DOIP_SERVER_USER_BASE_H_