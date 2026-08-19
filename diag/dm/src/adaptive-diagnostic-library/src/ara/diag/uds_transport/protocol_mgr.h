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
/// @file       protocol_mgr.h
/// @brief      This file provides the definitions of UdsTransportProtocolMgr and related types.
/// @details
/// @date       2021-11-23
/// @author     jiawei
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_DIAG_UDS_TRANSPORT_PROTOCOL_MGR_H_
#define ARA_DIAG_UDS_TRANSPORT_PROTOCOL_MGR_H_

#include <ara/core/span.h>
#include <ara/core/vector.h>

#include <cstddef>
#include <cstdint>
#include <tuple>
#include <utility>

#include "protocol_types.h"
#include "uds_message.h"

namespace ara {
namespace diag {
namespace uds_transport {

/// @brief Abstract class for implementing message forwarding infrastructure.
///
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
///
/// @traceid{SWS_DM_00306}@tracestatus{draft}
class UdsTransportProtocolMgr
{
public:
    ///
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    ///
    /// @traceid{SWS_DM_00307}@tracestatus{draft}
    enum class TransmissionResult : std::uint8_t
    {
        kTransmitOk     = 0,
        kTransmitFailed = 1
    };
    ///
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    ///
    /// @traceid{SWS_DM_00384}@tracestatus{draft}
    enum class IndicationResult : std::uint8_t
    {
        kIndicationOk                   = 0,
        kIndicationOccupied             = 1,
        kIndicationOverflow             = 2,
        kIndicationUnknownTargetAddress = 3
    };

    /// @brief  Type of tuple to pack UdsTransportProtocolHandlerID and ChannelID together, to form a global unique
    /// (among all used UdsTransportProtocolHandlers within DM) identifier of a UdsTransportProtocol channel.
    ///
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    ///
    /// @traceid{SWS_DM_09021}@tracestatus{draft}
    using GlobalChannelIdentifier = std::tuple< UdsTransportProtocolHandlerID, ChannelID >;

    /// @brief  Indicates a message start.
    /// This is an interface, which is just served/called by UdsTransportProtocolHandlers, which return true from
    /// UdsTransportProtocolHandlers::isStartOfMessageIndicationSupported().
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] sourceAddr UDS source address of message
    /// @param[in] targetAddr UDS target address of message
    /// @param[in] type indication whether its is phys/func request
    /// @param[in] globalChannelId transport protocol channel on which message start happened
    /// @param[in] size size in bytes of the UdsMessage starting from SID.
    /// @param[in] priority the priority of the given message, used for prioritization of conversations.
    /// @param[in] protocolKind identifier of protocol kind associated to message
    /// @param[in] payloadInfo View onto the first received payload bytes, if any. This view shall be used only within
    /// this function call. It is recommended that the TP provides at least the first two bytes of the request message,
    /// so the DM can identify a functional TesterPresent.
    /// @return Pair of IndicationResult and a pointer to UdsMessage owned/created by DM core and returned to the
    /// handler to get filled.
    ///
    ///
    /// @traceid{SWS_DM_00309}@tracestatus{draft}
    virtual std::pair< IndicationResult, UdsMessagePtr > IndicateMessage(
        UdsMessage::Address sourceAddr,
        UdsMessage::Address targetAddr,
        UdsMessage::TargetAddressType type,
        GlobalChannelIdentifier globalChannelId,
        std::size_t size,
        Priority priority,
        ProtocolKind protocolKind,
        ara::core::Span< const std::uint8_t > payloadInfo)
        = 0;

    /// @brief  Indicates, that the message indicated via IndicateMessage() has failure and will not lead to a final
    /// HandleMessage() call.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] message the pointer to UdsMessage handed back over to the session layer.
    ///
    ///
    /// @traceid{SWS_DM_00310}@tracestatus{draft}
    virtual void NotifyMessageFailure(UdsMessagePtr message) = 0;

    /// @brief  Hands over a valid received Uds message (currently this is only a request type) from transport layer to
    /// session layer.
    /// It corresponds to T_Data.ind of Figure 2 from ISO 14229-2. The behavior is asynchronously. I.e. the UdsMessage
    /// is handed over to Session Layer and it is expected, that it "instantly" returns, which means, that real
    /// processing of the message shall be done asynchronously!
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] message The Uds message ptr (unique_ptr semantics) with the request. Ownership of the UdsMessage is
    /// given back to the generic DM core here.
    ///
    ///
    /// @traceid{SWS_DM_00311}@tracestatus{draft}
    virtual void HandleMessage(UdsMessagePtr message) = 0;

    /// @brief  notification about the outcome of a transmit request called by core DM at the handler via
    /// UdsTransportProtocolHandler::Transmit
    /// This transmit API covers T_Data.con of ISO 14229-2 Figure 2.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] message for which message (created in IndicateMessage()) this is the confirmation.
    /// @param[in] result Result of transmission. In case UDS message could be transmitted on network layer:
    /// kTransmitOk),
    ///        kTransmitFailed else.
    ///
    ///
    /// @traceid{SWS_DM_00312}@tracestatus{draft}
    virtual void TransmitConfirmation(UdsMessageConstPtr message, TransmissionResult result) = 0;

    /// @brief Confirmation of sent messages and number.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] messages The same ordered list of messages previously passed to
    /// UdsTransportProtocolPeriodicHandler::PeriodicTransmit.
    /// @param[in] numberOfSentMessages The number of successfully sent messages from the "messages" list.
    ///
    ///
    /// @traceid{SWS_DM_01069}@tracestatus{draft}
    virtual void PeriodicTransmitConfirmation(ara::core::Vector< UdsMessageConstPtr > messages,
                                              std::size_t numberOfSentMessages)
        = 0;

    /// @brief  notification call from the given transport channel, that it has been reestablished since the last
    /// (Re)Start from the UdsTransportProtocolHandler to which this channel belongs. To activate this notification a
    /// previous call to UdsTransportProtocolHandler::NotifyReestablishment() has to be done. See further documentation
    /// at UdsTransportProtocolHandler::NotifyReestablishment().
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] globalChannelId transport protocol channel, which is available again.
    ///
    ///
    /// @traceid{SWS_DM_00313}@tracestatus{draft}
    virtual void ChannelReestablished(GlobalChannelIdentifier globalChannelId) = 0;

    /// @brief  notification from handler, that it has stopped now (e.g. closed down network connections, freed
    /// resources, etc...)
    /// This callback is expected as a reaction from handler to a call to UdsTransportProtocolHandler::Stop.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] handlerId indication, which plugin stopped.
    ///
    ///
    /// @traceid{SWS_DM_00314}@tracestatus{draft}
    virtual void HandlerStopped(UdsTransportProtocolHandlerID handlerId) = 0;

    /// @brief Destructor of UdsTransportProtocolMgr
    ///
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    virtual ~UdsTransportProtocolMgr() noexcept = default;

    /// @brief copy constructor
    /// @param other
    UdsTransportProtocolMgr(UdsTransportProtocolMgr const& other) noexcept = default;
    /// @brief copy assignment operator
    /// @param other
    /// @return UdsTransportProtocolMgr&
    UdsTransportProtocolMgr& operator=(UdsTransportProtocolMgr const& other) noexcept = default;
    /// @brief move constructor
    /// @param other
    UdsTransportProtocolMgr(UdsTransportProtocolMgr&& other) noexcept = default;
    /// @brief move assignment operator
    /// @param other
    /// @return UdsTransportProtocolMgr&
    UdsTransportProtocolMgr& operator=(UdsTransportProtocolMgr&& other) noexcept = default;
};

}  // namespace uds_transport
}  // namespace diag
}  // namespace ara

#endif  // ARA_DIAG_UDS_TRANSPORT_PROTOCOL_MGR_H_