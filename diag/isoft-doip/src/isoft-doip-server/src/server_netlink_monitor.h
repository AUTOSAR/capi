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
/// @file       server_netlink_monitor.h
/// @brief      This file provides DOIP server netlink implementation related
/// @details
/// @date       2024-06-12
/// @author     zhanglipeng
/// @version    1.2.0
///
/// ================================================================

#ifndef ISOFT_DOIP_SERVER_NETLINK_H_
#define ISOFT_DOIP_SERVER_NETLINK_H_
#include <arpa/inet.h>
#include <linux/if_addr.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <net/if.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>

#include "isoft/doip/server.h"

namespace isoft {
namespace doip {

class ServerNetlinkMonitor final
{
public:
    /// @brief Start network interface monitoring
    /// @param cb Network interface status change notification
    /// @return Start result
    int32_t Start(NetlinkMonitorCb const& cb);

    /// @brief Stop network interface monitoring
    /// @return Operation result
    int32_t Stop();

private:
    /// @brief Start network interface monitoring operation
    void _run();

    /// @brief Handle network interface up/down status change
    /// @param nlh
    void _handleLinkEvent(struct nlmsghdr* nlh);
    /// @brief Handle network interface IP address change
    /// @param nlh
    void _handleAddrEvent(struct nlmsghdr* nlh);

private:
    /// @brief Netlink descriptor
    int32_t netlinkFd_{0};

    /// @brief Callback for monitoring network interface status
    NetlinkMonitorCb cb_{};

    /// @brief Whether monitoring is running
    bool isRun_{false};

    /// @brief Maximum received data length
    static size_t const kEpollReadBufferSize{4096U};
};

}  // namespace doip
}  // namespace isoft

#endif