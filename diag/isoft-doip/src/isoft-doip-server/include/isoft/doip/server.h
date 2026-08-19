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
/// @file       server.h
/// @brief      This file provides DOIP server related interface definitions
/// @details
/// @date       2023-10-08
/// @author     zhanglipeng
/// @version    1.2.0
///
/// ================================================================

#ifndef ISOFT_DOIP_SERVER_H_
#define ISOFT_DOIP_SERVER_H_
#include <ara/core/result.h>
#include <ara/core/vector.h>

#include "isoft/doip/config.h"
#include "isoft/doip/public_def.h"
#pragma once
namespace isoft {
namespace doip {

/// @brief DOIP client request notification
/// Parameter 1 socket fd, Parameter 2 DOIP message type
/// Parameter 3 DOIP message payload (mainly notifies the upper layer of two message types: route activation and diagnostic request, other protocols are handled internally)
using ClientRequestCb = std::function< void(
    int32_t const fd, PayloadType const protocloType, ara::core::Vector< std::uint8_t > const& msg) >;

/// @brief Diagnostic connection open and close handling callback
using DiagConnectionActivateCb = std::function< void(int32_t const fd,
                                                     uint16_t const sa,
                                                     ara::core::String const& localIp,
                                                     uint16_t const localPort,
                                                     ara::core::String const& peerIp,
                                                     uint16_t const peerPort,
                                                     bool const state) >;

/// @brief Network interface change callback notification
/// Parameter 1 network interface index
/// Parameter 3 network interface IP address
using NetlinkMonitorCb = std::function< void(int32_t const networkId, ara::core::String const& ip) >;

/// @brief Forward declaration of ServerImpl
class ServerImpl;
/// @brief DOIP Server
/// @code{.isoft}
/// export_level=/DoIP
/// @endcode
class Server final
{
public:
    /// @brief Constructor
    /// @code{.isoft}
    /// export_level=/DoIP
    /// @endcode
    /// @param config Configuration
    /// @param requestHandler New message callback
    Server(ServerConfig const& config, ClientRequestCb const& requestHandler);

    /// @brief Destructor
    /// @code{.isoft}
    /// export_level=/DoIP
    /// @endcode
    ~Server() noexcept;

    /// @brief Copy constructor
    /// @code{.isoft}
    /// export_level=/DoIP
    /// @endcode
    /// @param[in] other
    Server(Server const& other) = delete;

    /// @brief Assignment operator
    /// @code{.isoft}
    /// export_level=/DoIP
    /// @endcode
    /// @param[in] other
    /// @return Server reference
    Server& operator=(Server const& other) = delete;

    /// @brief Move constructor
    /// @code{.isoft}
    /// export_level=/DoIP
    /// @endcode
    /// @param[in] other
    Server(Server&& other) = delete;

    /// @brief Assignment operator
    /// @code{.isoft}
    /// export_level=/DoIP
    /// @endcode
    /// @param[in] other
    /// @return Server reference
    Server& operator=(Server&& other) = delete;

public:
    /// @brief Activate network services (TCP & UDP)
    /// @code{.isoft}
    /// export_level=/DoIP
    /// @endcode
    /// @param[in] cb Callback for opening a diagnostic connection
    /// @return Activation result
    ara::core::Result< void > Start(DiagConnectionActivateCb const& cb);

    /// @brief Deactivate network services (TCP & UDP)
    /// @code{.isoft}
    /// export_level=/DoIP
    /// @endcode
    ara::core::Result< void > Stop();

    /// @brief Reply DOIP message
    /// @code{.isoft}
    /// export_level=/DoIP
    /// @endcode
    /// @param[in] uds Diagnostic message
    bool SendDoIPMessage(int32_t const fd,
                         PayloadType const protocloType,
                         ara::core::Vector< std::uint8_t > const& msg,
                         bool const isSend = true);

    /// @brief Broadcast vehicle announcement
    /// @code{.isoft}
    /// export_level=/DoIP
    /// @endcode
    /// @param[in] body Announcement message body
    void BroadcastAnnouncement(ara::core::Vector< uint8_t > const& body);

    /// @brief Get MAC address
    /// @code{.isoft}
    /// export_level=/DoIP
    /// @endcode
    /// @return mac
    MacAddress GetMacAddress();

    /// @brief Start network interface status monitoring (up, down, IP change)
    /// @return Operation result
    static ara::core::Result< void > StartNetlinkMonitor(NetlinkMonitorCb const& cb);

    /// @brief Stop network interface status monitoring
    /// @return Operation result
    static ara::core::Result< void > StopNetlinkMonitor();

private:
    /// @brief DOIP internal implementation object
    std::unique_ptr< ServerImpl > serverImpl_;
};

}  // namespace doip
}  // namespace isoft

#endif  // ISOFT_DOIP_SERVER_H_