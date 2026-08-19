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
/// @file       server_netlink_monitor.cpp
/// @brief      This file provides DOIP server netlink implementation related
/// @details
/// @date       2024-06-12
/// @author     zhanglipeng
/// @version    1.2.0
///
/// ================================================================

#include "server_netlink_monitor.h"

#include <thread>

#include "public_log.h"
#include "thread/therad_safe.h"

namespace isoft {
namespace doip {

// Helper function for parsing rtattr
void Parse_Rtattr(struct rtattr **tb, int max, struct rtattr *rta, int len)
{
    while (RTA_OK(rta, len)) {
        if (rta->rta_type <= max) {
            tb[rta->rta_type] = rta;
        }
        rta = RTA_NEXT(rta, len);  // NOLINT
    }
}

int32_t ServerNetlinkMonitor::Start(NetlinkMonitorCb const &cb)
{
    netlinkFd_ = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
    if (netlinkFd_ == -1) {
        LogError() << "ServerNetlinkMonitor::Start|init netlink socket fails!"
                   << isoft::threadsafe::Safe_Strerror(errno);
        return 1;
    }

    struct sockaddr_nl sa
    {
    };
    memset(&sa, 0, sizeof(sa));
    sa.nl_family = AF_NETLINK;
    sa.nl_pid    = getpid();
    sa.nl_groups = RTMGRP_LINK | RTMGRP_IPV4_IFADDR;
    if (bind(netlinkFd_, reinterpret_cast< struct sockaddr * >(&sa), sizeof(sa)) < 0) {
        LogError() << "ServerNetlinkMonitor::Start|bind netlink socket fails!"
                   << isoft::threadsafe::Safe_Strerror(errno);
        close(netlinkFd_);
        return 2;
    }

    isRun_ = true;
    cb_    = cb;
    std::thread t{&ServerNetlinkMonitor::_run, this};
    t.detach();
    LogInfo() << "ServerNetlinkMonitor::Start|monitor success";
    return 0;
}

int32_t ServerNetlinkMonitor::Stop()
{
    isRun_ = false;
    return 0;
}

void ServerNetlinkMonitor::_run()
{
    char buffer[kEpollReadBufferSize];
    for (;;) {
        if (!isRun_) {
            break;
        }

        struct sockaddr_nl snl
        {
        };
        socklen_t len = sizeof(snl);

        ssize_t dataSize
            = recvfrom(netlinkFd_, buffer, sizeof(buffer), 0, reinterpret_cast< struct sockaddr * >(&snl), &len);
        if (dataSize < 0) {
            LogError() << "ServerNetlinkMonitor::_run|recvfrom dataSize =" << dataSize
                       << "error =" << isoft::threadsafe::Safe_Strerror(errno);
            continue;
        }

        LogInfo() << "ServerNetlinkMonitor::_run|recvfrom success dataSize =" << dataSize;

        for (struct nlmsghdr *nh = reinterpret_cast< struct nlmsghdr * >(buffer); NLMSG_OK(nh, dataSize);
             nh                  = NLMSG_NEXT(nh, dataSize))  // NOLINT
        {
            switch (nh->nlmsg_type) {
                case RTM_NEWLINK: {
                    _handleLinkEvent(nh);
                } break;
                case RTM_NEWADDR: {
                    _handleAddrEvent(nh);
                } break;
                default:
                    break;
            }
        }
    }

    close(netlinkFd_);
}  // namespace doip

/// @brief Handle network interface up/down status change
/// @param nlh
void ServerNetlinkMonitor::_handleLinkEvent(struct nlmsghdr *nlh)
{
    if (nlh == nullptr) {
        LogError() << "ServerNetlinkMonitor::_handleLinkEvent|nlh is nullptr";
        return;
    }

    struct ifinfomsg *ifi = reinterpret_cast< struct ifinfomsg * >((reinterpret_cast< char * >(nlh)) + NLMSG_LENGTH(0));
    if (ifi == nullptr) {
        LogError() << "ServerNetlinkMonitor::_handleLinkEvent|ifi is nullptr";
        return;
    }

    struct rtattr *tb[IFLA_MAX + 1] = {nullptr};
    Parse_Rtattr(tb, IFLA_MAX, IFLA_RTA(ifi), nlh->nlmsg_len - NLMSG_SPACE(sizeof(*ifi)));  // NOLINT
    if (ifi->ifi_index <= 0) {
        LogError() << "ServerNetlinkMonitor::_handleLinkEvent|ifi_index error index =" << ifi->ifi_index;
        return;
    }

    if ((ifi->ifi_flags & IFF_RUNNING) != IFF_RUNNING) {
        ara::core::String strIP = "";
        cb_(ifi->ifi_index, strIP);
    }

    LogDebug() << "ServerNetlinkMonitor::_handleLinkEvent|netinterface state change index =" << ifi->ifi_index
               << "curState =" << ((ifi->ifi_flags & IFF_RUNNING) == IFF_RUNNING ? "Up" : "Down");
}

/// @brief Handle network interface IP address change
/// @param nlh
void ServerNetlinkMonitor::_handleAddrEvent(struct nlmsghdr *nlh)
{
    if (nlh == nullptr) {
        LogError() << "ServerNetlinkMonitor::_handleAddrEvent|nlh is nullptr";
        return;
    }

    struct ifaddrmsg *ifa
        = reinterpret_cast< struct ifaddrmsg * >(((reinterpret_cast< char * >(nlh)) + NLMSG_LENGTH(0)));
    if (ifa == nullptr) {
        LogError() << "ServerNetlinkMonitor::_handleAddrEvent|ifa is nullptr";
        return;
    }

    struct rtattr *tb[IFA_MAX + 1] = {nullptr};
    Parse_Rtattr(tb, IFA_MAX, IFA_RTA(ifa), nlh->nlmsg_len - NLMSG_SPACE(sizeof(*ifa)));  // NOLINT
    if (ifa->ifa_index <= 0) {
        LogError() << "ServerNetlinkMonitor::_handleAddrEvent|ifi_index error index =" << ifa->ifa_index;
        return;
    }

    if (tb[IFA_LOCAL] == nullptr) {
        LogError() << "ServerNetlinkMonitor::_handleAddrEvent|ta index 2 is nullptr";
        return;
    }

    if (ifa->ifa_family != AF_INET) {
        LogError() << "ServerNetlinkMonitor::_handleAddrEvent|family error";
        return;
    }

    char addrStr[RTMGRP_IPV4_IFADDR] = {0};
    inet_ntop(ifa->ifa_family, RTA_DATA(tb[IFA_LOCAL]), addrStr, sizeof(addrStr));  // NOLINT
    ara::core::String strIP{addrStr};
    cb_(static_cast< int32_t >(ifa->ifa_index), strIP);

    LogDebug() << "ServerNetlinkMonitor::_handleAddrEvent|netinterface ip change index =" << ifa->ifa_index
               << "cruState =" << nlh->nlmsg_type << "ip =" << addrStr;
}

}  // namespace doip
}  // namespace isoft