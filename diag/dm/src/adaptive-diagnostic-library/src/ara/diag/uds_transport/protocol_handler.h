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
/// @file       protocol_handler.h
/// @brief      This file provides the definitions of UdsTransportProtocolHandler and related types.
/// @details
/// @date       2021-11-23
/// @author     jiawei
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_DIAG_UDS_TRANSPORT_PROTOCOL_HANDLER_H_
#define ARA_DIAG_UDS_TRANSPORT_PROTOCOL_HANDLER_H_

#include <ara/core/result.h>

#include <cstdint>
#include <memory>

#include "protocol_mgr.h"
#include "protocol_periodic_handler.h"
#include "protocol_types.h"

namespace ara {
namespace diag {
namespace uds_transport {

/// @brief Abstract Class, which a specific UDS Transport Protocol (plugin) shall subclass.
///
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
///
/// @traceid{SWS_DM_00315}@tracestatus{draft}
class UdsTransportProtocolHandler
{
protected:
    /// @brief The UdsTransportProtocolMgr used/provided by the DM/DCM.
    ///
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    ///
    /// @traceid{SWS_DM_09025}@tracestatus{draft}
    UdsTransportProtocolMgr& transportprotocolManager;  // NOLINT

public:
    /// @brief Constructor of UdsTransportProtocolHandler.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] handlerId the handler ID used by DM to identify this handler. This is just a number/identification
    /// given by the DM core when instantiating a UdsTransportProtocolHandler instance to be able to distinguish it from
    /// other handler-plugins or built-in UdsTransportProtocolHandler implementations.
    /// @param[in] transportProtocolMgr reference to UdsTransportProtocolMgr owned by this DM, with which
    /// UdsTransportProtocolHandler instance shall interact.
    ///
    ///
    /// @traceid{SWS_DM_09015}@tracestatus{draft}
    explicit UdsTransportProtocolHandler(UdsTransportProtocolHandlerID const handlerId,
                                         UdsTransportProtocolMgr& transportProtocolMgr)
        : transportprotocolManager(transportProtocolMgr), kHandlerIdent(handlerId){};

    /// @brief Result of Initialize handler.
    ///
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    ///
    /// @traceid{SWS_DM_09017}@tracestatus{draft}
    enum class InitializationResult : std::uint8_t
    {
        kInitializeOk     = 0,
        kInitializeFailed = 1
    };

    /// @brief Destructor of UdsTransportProtocolHandler.
    ///
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    ///
    /// @traceid{SWS_DM_09016}@tracestatus{draft}
    virtual ~UdsTransportProtocolHandler() = default;

    /// @brief copy constructor
    /// @param other
    UdsTransportProtocolHandler(UdsTransportProtocolHandler const& other) noexcept = default;
    /// @brief copy assignment operator
    /// @brief UdsTransportProtocolHandler contains const members, so copy assignment is deleted
    /// @param other
    /// @return UdsTransportProtocolHandler&
    UdsTransportProtocolHandler& operator=(UdsTransportProtocolHandler const& other) noexcept = delete;
    /// @brief move constructor
    /// @param other
    UdsTransportProtocolHandler(UdsTransportProtocolHandler&& other) noexcept = default;
    /// @brief move assignment operator
    /// @brief UdsTransportProtocolHandler contains const members, so move assignment is deleted
    /// @param other
    /// @return UdsTransportProtocolHandler&
    UdsTransportProtocolHandler& operator=(UdsTransportProtocolHandler&& other) noexcept = delete;

    /// @brief Initializes handler.
    /// Must be called before Start(). The idea is to have "initialization" of handler-plugin separated from its ctor.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @return kInitializeOk if initialization was successful, else kInitializeFailed.
    ///
    ///
    /// @traceid{SWS_DM_00319}@tracestatus{draft}
    virtual InitializationResult Initialize() = 0;

    /// @brief Start processing the implemented Uds Transport Protocol.
    /// The implementation shall call its superclass Start() method as there might be some stack specific
    /// implementation. Implementation shall be asynchronous as DM might start many/different
    /// UdsTransportProtocolHandler in parallel and strong serialization of all those starts just unnecessarily slows
    /// down DM startup.
    ///
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    ///
    /// @traceid{SWS_DM_00322}@tracestatus{draft}
    virtual void Start() = 0;

    /// @brief Method to indicate that this UdsTransportProtocolHandler should terminate.
    /// If UdsTransportProtocolHandler has stopped, it shall call
    /// UdsTransportProtocolMgr::HandlerStopped(UdsTransportProtocolHandlerID)
    /// After return from Stop(), the handler-plugin shall NOT call to UdsTransportProtocolMgr with any other method but
    /// UdsTransportProtocolMgr::HandlerStopped()
    ///
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    ///
    /// @traceid{SWS_DM_00323}@tracestatus{draft}
    virtual void Stop() = 0;

    /// @brief Return the UdsTransportProtocolHandlerID, which was given to the implementation during construction (ctor
    /// call).
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @return UdsTransportProtocolHandlerID.
    ///
    ///
    /// @traceid{SWS_DM_00325}@tracestatus{draft}
    virtual UdsTransportProtocolHandlerID GetHandlerID() const { return kHandlerIdent; };

    /// @brief Tells the UdsTransportProtocolHandler, that it shall notify the DM core
    /// via UdsTransportProtocolMgr::ChannelReestablished())
    /// if the given channel has been re-established after next UdsTransportProtocolHandler::Start().
    /// The main purpose of this method is to allow DM to provide an ECU-Reset (0x11 service), with configuration option
    /// "Pos. response AFTER reset". In this scenario the request for 0x11 will be received on a certain channel with
    /// identifying tuple <p_x, c_y> (GlobalChannelIdentifier). Then the ECU-Reset takes place and after ECU-Restart all
    /// UdsProtocolHandlers/plugins get restarted via call to UdsTransportProtocolHandler::Start().
    /// Now there are two expectations, when this method has been called before and returned "true":
    /// - IF the same remote client connects to the UdsProtocolHandler, it shall get a channel identification with the
    /// same identifying tuple <p_x, c_y> as last time.
    /// - it shall call UdsTransportProtocolMgr::ChannelReestablished(GlobalChannelIdentifier<p_x, c_y>)
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @note: IF the underlying network layer of the UdsTransportProtocolHandler isn't really connection based (e.g. a
    /// UDP based protocol), then the UdsTransportProtocolHandler shall call
    /// UdsTransportProtocolMgr::ChannelReestablished() after UdsTransportProtocolHandler::Start() as soon as it
    /// detects/assumes that the remote client/tester will be reachable again.
    /// @note: The detection/decision, whether the "same" client reconnects as before is an UdsProtocolHandler
    /// implementation specific decision. The general expectation is: If the channel is set up from exactly the same
    /// remote network-endpoint, it typically shall be given the same channelID (c_y part of the tuple).
    /// To support this functionality the implementation at least has to store non-volatile, that this notification has
    /// to be done. Further it might be needed to store some additional connection specific info non-volatile to make
    /// sure, that the same channelID (c_y part of the tuple) can be reassigned. This is the case if the mapping of
    /// protocol specific channel info -> channelID isn't a stable bijective mapping! Small example: The underlying
    /// network protocol, which UdsProtocolHandler implements is based on TCP. At the point in time, where the 0x11 SI
    /// request is received on channel identified by <p_x, c_y> the DM calls NotifyReestablishment() on this channelID.
    /// Now the implementation of UdsProtocolHandler stores non-volatile in the context of this call:
    /// - the NetworkEndpoint (IP-address and port number) of the channel
    /// - the NetworkEndpoint (IP-address and port number) of the local port (because in this example, the
    /// UdsTransportProtocolHandler listens on/supports different ports)
    /// - the channelID (c_y part) it has currently assigned.
    /// After restart this channelID only shall be reused for a channel with exactly the same NetworkEndpoint addresses
    /// as stored non-volatile. If this channelID then gets reassigned, then
    /// UdsTransportProtocolMgr::ChannelReestablished() has to be called.
    /// @param[in] channelId channelID, whose re-establishment shall be notified to UdsTransportProtocolMgr
    /// @returns true if notification request is accepted and can be fulfilled.
    ///
    ///
    /// @traceid{SWS_DM_00326}@tracestatus{draft}
    virtual bool NotifyReestablishment(ChannelID channelId) = 0;

    /// @brief Transmit a Uds message via the underlying Uds Transport Protocol channel.
    /// This transmit API covers T_Data.req of ISO 14229-2 Figure 2.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] message The message to be transmitted as a UdsMessage::Ptr (unique_ptr style).
    /// UdsTransportProtocolHandler has to give back this UdsMessage::Ptr via
    /// UdsTransportProtocolMgr::TransmitConfirmation() to signal, that it is done with this message.
    /// @param[in] channelId identification of channel on which to transmit.
    ///
    ///
    /// @traceid{SWS_DM_00327}@tracestatus{draft}
    virtual void Transmit(UdsMessageConstPtr message, ChannelID channelId) = 0;

    /// @brief Returns the corresponding periodic TP handler.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @returns UdsTransportProtocolPeriodicHandler reference if periodic transmissions are supported on this transport
    /// protocol, an error if not supported
    /// @modify by jiawei UdsTransportProtocolPeriodicHandler& to std::shared_ptr<UdsTransportProtocolPeriodicHandler>
    /// case ara::core::Result not support <&>
    ///
    ///
    /// @traceid{SWS_DM_01068}@tracestatus{draft}
    virtual ara::core::Result< std::shared_ptr< UdsTransportProtocolPeriodicHandler > > GetPeriodicHandler() = 0;

private:
    /// @brief The id of the handler (shall be set by initializer list of ctor.)
    ///
    /// @traceid{SWS_DM_00324}@tracestatus{draft}
    UdsTransportProtocolHandlerID const kHandlerIdent;
};

}  // namespace uds_transport
}  // namespace diag
}  // namespace ara

#endif  // ARA_DIAG_UDS_TRANSPORT_PROTOCOL_HANDLER_H_