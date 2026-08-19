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
/// @file       protocol_periodic_handler.h
/// @brief      This file provides the definitions of UdsTransportProtocolPeriodicHandler and related types.
/// @details
/// @date       2021-11-23
/// @author     jiawei
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_DIAG_UDS_TRANSPORT_PROTOCOL_PERIODIC_HANDLER_H_
#define ARA_DIAG_UDS_TRANSPORT_PROTOCOL_PERIODIC_HANDLER_H_

#include <ara/core/vector.h>

#include <cstddef>

#include "protocol_types.h"
#include "uds_message.h"

namespace ara {
namespace diag {
namespace uds_transport {

/// @brief UdsTransportProtocolPeriodicHandler class to support 0x2A service from ISO.
///
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
///
/// @traceid{SWS_DM_01064}@tracestatus{draft}
class UdsTransportProtocolPeriodicHandler
{
public:
    /// @brief Reports the TP implementation and connection specific number of periodic messages.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] channelId The concrete connection to send over
    /// @returns number of periodic messages
    ///
    ///
    /// @traceid{SWS_DM_01065}@tracestatus{draft}
    virtual std::size_t GetNumberOfPeriodicMessages(ChannelID channelId) const = 0;

    /// @brief Reports the maximum payload length supported for a single periodic transmission on the channel.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] channelId The concrete connection for which the maximum payload length is reported.
    /// @returns supported payload length
    ///
    ///
    /// @traceid{SWS_DM_01066}@tracestatus{draft}
    virtual std::size_t GetMaxPayloadLength(ChannelID channelId) const = 0;

    /// @brief Sends all the messages in the list in the given order. If one message transmission fails, the send
    /// process is stopped and the PeriodicTransmitConfirmation is invoked with the number of sent messages and the same
    /// Vector with UdsMessages.
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] channelId The concrete connection to send over
    /// @param[in] messages Ordered list of messages to send at once
    ///
    ///
    /// @traceid{SWS_DM_01067}@tracestatus{draft}
    virtual void PeriodicTransmit(ChannelID channelId, ara::core::Vector< UdsMessageConstPtr > messages) = 0;

    virtual ~UdsTransportProtocolPeriodicHandler() noexcept = default;

    /// @brief copy constructor
    /// @param other
    UdsTransportProtocolPeriodicHandler(UdsTransportProtocolPeriodicHandler const& other) noexcept = default;
    /// @brief copy assignment operator
    /// @param other
    /// @return UdsTransportProtocolPeriodicHandler&
    UdsTransportProtocolPeriodicHandler& operator=(UdsTransportProtocolPeriodicHandler const& other) noexcept = default;
    /// @brief move constructor
    /// @param other
    UdsTransportProtocolPeriodicHandler(UdsTransportProtocolPeriodicHandler&& other) noexcept = default;
    /// @brief move assignment operator
    /// @param other
    /// @return UdsTransportProtocolPeriodicHandler&
    UdsTransportProtocolPeriodicHandler& operator=(UdsTransportProtocolPeriodicHandler&& other) noexcept = default;
};

}  // namespace uds_transport
}  // namespace diag
}  // namespace ara

#endif  // ARA_DIAG_UDS_TRANSPORT_PROTOCOL_PERIODIC_HANDLER_H_