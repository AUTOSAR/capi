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
/// @file       server.cpp
/// @brief      This file provides DOIP server related interface definitions
/// @details
/// @date       2023-10-08
/// @author     zhanglipeng
/// @version    1.2.0
///
/// ================================================================

#include "isoft/doip/server.h"

#include "isoft/doip/doip_error_domain.h"
#include "public_log.h"
#include "server_impl.h"

namespace isoft {
namespace doip {

/// @brief Constructor
/// @code{.isoft}
/// export_level=/DoIP
/// @endcode
/// @param[in] config Configuration
/// @param[in] requestHandler DOIP message callback
Server::Server(ServerConfig const& config, ClientRequestCb const& requestHandler)
    : serverImpl_{std::make_unique< ServerImpl >(config, requestHandler)}
{
}

/// @brief Destructor
Server::~Server() noexcept { serverImpl_.reset(); }

/// @brief Activate DOIP network service
/// @code{.isoft}
/// export_level=/DoIP
/// @endcode
/// @param[in] cb Diagnostic connection activation callback
/// @return Activation result
ara::core::Result< void > Server::Start(DiagConnectionActivateCb const& cb)
{
    if (serverImpl_.get() == nullptr) {
        LogError() << "Server::Start|serverImpl_ is nullptr";
        return ara::core::Result< void >::FromError(DoipErrorCode::kDoipInternalErr);
    }

    return serverImpl_->Start(cb);
}

/// @brief Stop DOIP service
/// @code{.isoft}
/// export_level=/DoIP
/// @endcode
/// @return Operation result
ara::core::Result< void > Server::Stop()
{
    if (serverImpl_.get() == nullptr) {
        LogError() << "Server::Stop|serverImpl_ is nullptr";
        return ara::core::Result< void >::FromError(DoipErrorCode::kDoipInternalErr);
    }

    return serverImpl_->Stop();
}

/// @brief Reply DOIP message
/// @code{.isoft}
/// export_level=/DoIP
/// @endcode
/// @param[in] fd sock fd
/// @param[in] protocloType Protocol number
/// @param[in] msg Message body
/// @param[in] isSend Whether to send this message
/// @return Operation result
bool Server::SendDoIPMessage(int32_t const fd,
                             PayloadType const protocloType,
                             ara::core::Vector< std::uint8_t > const& msg,
                             bool const isSend)
{
    if (serverImpl_.get() == nullptr) {
        LogError() << "Server::SendDoIPMessage|serverImpl_ is nullptr";
        return false;
    }

    return serverImpl_->SendDoIPMessage(fd, protocloType, msg, isSend);
}

/// @brief Start sending vehicle announcements
/// @code{.isoft}
/// export_level=/DoIP
/// @endcode
/// @param[in] body Announcement packet
void Server::BroadcastAnnouncement(ara::core::Vector< uint8_t > const& body)
{
    if (serverImpl_.get() == nullptr) {
        LogError() << "Server::BroadcastAnnouncement|serverImpl_ is nullptr";
        return;
    }

    serverImpl_->BroadcastAnnouncement(body);
}

/// @brief Get MAC
/// @code{.isoft}
/// export_level=/DoIP
/// @endcode
/// @return mac
MacAddress Server::GetMacAddress()
{
    if (serverImpl_.get() == nullptr) {
        LogError() << "Server::GetMacAddress|serverImpl_ is nullptr";
        return MacAddress{};
    }

    return serverImpl_->GetMacAddress();
}

/// @brief Start network interface status monitoring (up, down, IP change)
/// @return Operation result
ara::core::Result< void > Server::StartNetlinkMonitor(NetlinkMonitorCb const& cb)
{
    return ServerImpl::StartNetlinkMonitor(cb);
}

/// @brief Stop network interface status monitoring
/// @return Operation result
ara::core::Result< void > Server::StopNetlinkMonitor() { return ServerImpl::StopNetlinkMonitor(); }

}  // namespace doip
}  // namespace isoft
