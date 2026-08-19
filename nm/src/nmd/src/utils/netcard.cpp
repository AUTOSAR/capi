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
/// @file       netcard.cpp
/// @brief      NIC operation utility methods
/// @details
/// @date       2022-06-10
/// @author     hejunwei
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/NetworkManager/utils
/// @interface_level=Module
/// @trace_id_sr=SRS_NM_00001
/// @unit_name=utils
/// @unit_description=NIC operation utility methods
/// @endcode
///
/// ================================================================

#include "include/netcard.h"

#include <arpa/inet.h>
#include <ifaddrs.h>
#include <linux/ethtool.h>
#include <linux/if.h>
#include <linux/sockios.h>
#include <net/if.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <iomanip>
#include <sstream>
#include <string>

#include "common/common.h"
#include "include/utils.h"

namespace ara {
namespace nm {
namespace internal {

/// @brief Check if the NIC is link-up.
/// @param ipv4Addr NIC IP address.
/// @returns true found network adapter
bool CheckNetCard(ara::core::String const &ipv4Addr) noexcept
{
    struct ifaddrs *ifap{nullptr};

    if (0 > getifaddrs(&ifap)) {
        return false;
    }

    struct ifaddrs *p{ifap};
    std::uint32_t ifindex{0U};
    while (true) {
        if (nullptr == p) {
            break;
        }
        ifindex = if_nametoindex(p->ifa_name);
        if (0U == ifindex) {
            break;
        }
        std::uint32_t const iffLoopBak{static_cast< std::uint32_t >(IFF_LOOPBACK)};  /// just for qac
        if (iffLoopBak == (p->ifa_flags & iffLoopBak)) {
            p = p->ifa_next;
            continue;
        }
        std::uint32_t const iffLowerup{static_cast< std::uint32_t >(IFF_LOWER_UP)};  /// just for qac
        if (!(iffLowerup == (p->ifa_flags & iffLowerup))) {
            p = p->ifa_next;
            continue;
        }
        if (p->ifa_addr != nullptr) {
            void *const tmpAddrPtr{&(reinterpret_cast< struct sockaddr_in * >(p->ifa_addr))->sin_addr};
            ara::core::Array< NmChar, INET_ADDRSTRLEN > addressBuffer{};
            addressBuffer.fill('\0');
            static_cast< void >(
                inet_ntop(AF_INET, tmpAddrPtr, addressBuffer.data(), static_cast< std::uint32_t >(INET_ADDRSTRLEN)));
            if (0 == ipv4Addr.compare(addressBuffer.data())) {
                break;
            }
        }
        p = p->ifa_next;
    }
    freeifaddrs(ifap);
    bool bRet{false};
    if (0U < ifindex) {
        bRet = true;
    }
    return bRet;
}

/// @brief Get NIC name.
/// @param ipv4Addr IP address
/// @returns NIC name
ara::core::String GetNetCardName(ara::core::String const &ipv4Addr) noexcept
{
    struct ifaddrs *ifap{nullptr};
    ara::core::String ifName{};

    if (0 > getifaddrs(&ifap)) {
        return ifName;
    }

    struct ifaddrs *p{ifap};
    std::uint32_t ifindex{0U};
    std::uint32_t const ifLoopBack{static_cast< std::uint32_t >(IFF_LOOPBACK)};
    std::uint32_t const ifLowerup{static_cast< std::uint32_t >(IFF_LOWER_UP)};

    while (true) {
        if (nullptr == p) {
            break;
        }
        ifindex = if_nametoindex(p->ifa_name);
        if (0U == ifindex) {
            break;
        }
        if (ifLoopBack == (ifLoopBack & p->ifa_flags)) {
            p = p->ifa_next;
            continue;
        }
        if (ifLowerup != (ifLowerup & p->ifa_flags)) {
            p = p->ifa_next;
            continue;
        }
        if (p->ifa_addr != nullptr) {
            void *const tmpAddrPtr{&(reinterpret_cast< struct sockaddr_in * >(p->ifa_addr))->sin_addr};
            ara::core::Array< NmChar, INET_ADDRSTRLEN > addressBuffer{};
            addressBuffer.fill('\0');
            static_cast< void >(
                inet_ntop(AF_INET, tmpAddrPtr, addressBuffer.data(), static_cast< std::uint32_t >(INET_ADDRSTRLEN)));
            if (0 == ipv4Addr.compare(addressBuffer.data())) {
                ifName = p->ifa_name;
                break;
            }
        }
        p = p->ifa_next;
    }
    freeifaddrs(ifap);
    return ifName;
}

/// @brief Get NIC MAC address.
/// @param deviceName Device name
/// @param addrBytes MAC address
/// @returns 0 if operation succeeded
std::int32_t GetMacAddress(ara::core::String const &deviceName, std::uint8_t addrBytes[]) noexcept
{
    /// Get the MAC address of the specified NIC
    struct ifreq ifMac
    {
    };
    std::int32_t ifIndex{0};
    std::int32_t sockfd{0};

    /// Get NIC index
    ifIndex = static_cast< std::int32_t >(if_nametoindex(deviceName.c_str()));
    if (0 == ifIndex) {
        return -1;
    }
    std::ignore = memset(&ifMac, 0, sizeof(ifMac));
    sockfd      = ::socket(AF_INET, SOCK_DGRAM, 0);
    if (0 > sockfd) {
        return kNmConstNegactive2;
    }
    std::size_t macStrlen{sizeof(ifMac.ifr_name)};
    if (macStrlen > deviceName.length()) {
        macStrlen = deviceName.length();
    }
    std::ignore       = strncpy(ifMac.ifr_name, deviceName.c_str(), macStrlen);
    std::ignore       = ifMac.ifr_name[0];  // just for qac
    ifMac.ifr_ifindex = ifIndex;
    std::uint64_t const requestId{static_cast< std::uint64_t >(SIOCGIFHWADDR)};
    if (0 > ioctl(sockfd, requestId, &ifMac)) {
        std::ignore = close(sockfd);
        return kNmConstNegactive3;
    }

    static_cast< void >(::memmove(addrBytes, ifMac.ifr_hwaddr.sa_data, kNmMacLen));
    std::ignore = close(sockfd);

    return 0;
}

/// @brief Get the multicast MAC address corresponding to the specified multicast IP.
/// @param multicastIp Multicast IP
/// @param muticastMAcaddr Multicast MAC address
/// @returns 0 if operation succeeded
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100000
/// @trace_id_dd=DD_NM_00919
/// @needwork = ad
/// @endcode
std::int32_t GetMulticastMacAddress(ara::core::String const &multicastIp,
                                    std::uint8_t muticastMAcaddr[kNmMacLen]) noexcept
{
    uint32_t ipv4Address{0U};
    std::string const ipv4String(multicastIp.c_str());
    std::stringstream ss{ipv4String};
    std::uint32_t part{0U};
    NmChar delim{0};
    for (std::size_t i{0U}; i < kNmConst2U; ++i) {
        ss >> part;
        ipv4Address = static_cast< std::uint32_t >(ipv4Address | (part << ((kNmConst3U - i) * kNmConst8U)));
        // if (i < kNmConst3U).
        {
            ss >> delim;
            if (delim != kNmDot) {
                return -1;
            }
        }
    }
    uint32_t const macLow23Bits{ipv4Address & 0x007FFFFFU};
    muticastMAcaddr[0]          = 0x01U;
    muticastMAcaddr[kNmConst1U] = 0x00U;
    muticastMAcaddr[kNmConst2U] = kNmConstMacByte2;
    muticastMAcaddr[kNmConst3U] = static_cast< std::uint8_t >((macLow23Bits & kNmConstMacFF3) >> kNmConst8U);
    muticastMAcaddr[kNmConst4U] = static_cast< std::uint8_t >((macLow23Bits & kNmConstMacFF4) >> kNmConst8U);
    muticastMAcaddr[kNmConst5U] = static_cast< std::uint8_t >(macLow23Bits & kNmConstMacFF5);
    return 0;
}

/// @brief Check by NIC name. If the NIC is link-down, it cannot be found by IP.
/// @param  deviceName ifName
/// @return  Whether the search succeeded
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100001
/// @trace_id_dd=DD_NM_00920
/// @needwork = ad
/// @endcode
std::int32_t CheckNetcardByName(ara::core::String const &deviceName) noexcept
{
    std::int32_t sockfd{0};
    struct ifreq ifr
    {
    };
    struct ethtool_value edata
    {
    };
    std::int32_t ifIndex{0};

    /// Get NIC index
    ifIndex = static_cast< std::int32_t >(if_nametoindex(deviceName.c_str()));
    if (0 == ifIndex) {
        return -1;
    }

    // Create a socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        return -1;
    }

    // Initialize ifreq structure
    std::ignore = memset(&ifr, 0, sizeof(ifr));
    std::size_t macStrlen{sizeof(ifr.ifr_name)};
    if (macStrlen > deviceName.length()) {
        macStrlen = deviceName.length();
    }
    std::ignore = strncpy(ifr.ifr_name, deviceName.c_str(), macStrlen);

    // Initialize ethtool_value structure
    edata.cmd    = ETHTOOL_GLINK;
    ifr.ifr_data = reinterpret_cast< caddr_t >(&edata);

    // Use ioctl to get link status
    if (ioctl(sockfd, SIOCETHTOOL, &ifr) == -1) {
        std::ignore = close(sockfd);
        return kNmConstNegactive2;
    }

    if (1U == edata.data) {
        /// Get the MAC address of the specified NIC
        struct ifreq ifMac
        {
        };
        std::ignore = memset(&ifMac, 0, sizeof(ifMac));
        static_cast< void >(strncpy(ifMac.ifr_name, deviceName.c_str(), macStrlen));
        ifMac.ifr_ifindex = ifIndex;
        std::uint64_t const requestId{static_cast< std::uint64_t >(SIOCGIFHWADDR)};
        if (0 > ioctl(sockfd, requestId, &ifMac)) {
            std::ignore = close(sockfd);
            return kNmConstNegactive3;
        }
    }
    std::ignore = close(sockfd);
    return 0;
}

/// @brief Initialize to check if the NIC is abnormal. If abnormal, record DTC event.
/// @brief Check NIC: three consecutive times. If the NIC does not exist or getting the MAC address fails,
/// @param ifName NIC name
/// @returns true if check is normal
bool InitCheckNetCard(ara::core::String const &ifName) noexcept
{
    for (std::uint16_t i{0U}; i < kInitCheckCount; i++) {
        std::int32_t const errcode{CheckNetcardByName(ifName)};
        NmLogger().LogDebug() << "NMEtherStateMachine, CheckNetcardByName errcode=" << errcode;
        if (0 == errcode) {
            return true;
        }
    }
    return false;
}

}  // namespace internal
}  // namespace nm
}  // namespace ara