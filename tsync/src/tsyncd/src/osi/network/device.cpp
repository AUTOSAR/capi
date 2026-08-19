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
/// @file       device.cpp
/// @brief      OSI network module device class
/// @details
/// @date       2023-01-09
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#include "isoft/osi/network/device.h"

#include <cstring>
#include <string>
#include <vector>

#include "ara/tsync/internal/common.h"
#include "isoft/osi/network/ether.h"

// clang-format off
#include <sys/ioctl.h>  // NOLINT
#include <sys/types.h>  // NOLINT
#include <ifaddrs.h>    // NOLINT
#include <arpa/inet.h>  // NOLINT
#include <net/if.h>     // NOLINT
#include <linux/if.h>   // NOLINT
// clang-format on

namespace isoft {
namespace osi {
namespace network {

/// @brief get the list of local network devices
/// @param netdevs - device list
/// @return 0 - success
/// @return <0 - failure
std::int32_t GetDevices(std::vector< Device > &netdevs) noexcept
{
    struct ifaddrs *ifap{nullptr};

    if (0 > getifaddrs(&ifap)) {
        return ara::tsync::internal::kRET_E1;
    }

    struct ifaddrs *p{ifap};
    std::uint32_t ifindex{0U};
    while (nullptr != p) {
        ifindex = if_nametoindex(p->ifa_name);
        if (0U == ifindex) {
            break;
        }
        /// Get network device index number
        Device netdev;
        netdev.devIndex = ifindex;

        if (static_cast< std::uint32_t >(IFF_LOOPBACK) == (p->ifa_flags & static_cast< std::uint32_t >(IFF_LOOPBACK))) {
            p = p->ifa_next;
            continue;
        }

        /// Get network device name
        netdev.name = p->ifa_name;

        /// Default is active
        netdev.isLowerUp = true;

        netdevs.emplace_back(netdev);

        p = p->ifa_next;
    }

    freeifaddrs(ifap);
    return 0;
}

/// @brief find network card name by network card IP
/// @param ipAddr - network card IP
/// @return network card name
ara::core::String GetDeviceName(ara::core::String const &ipAddr) noexcept
{
    struct ifaddrs *ifap{nullptr};

    ara::core::String netName;
    if (0 > getifaddrs(&ifap)) {
        return netName;
    }

    struct ifaddrs *p{ifap};
    std::uint32_t ifindex{0U};
    while (nullptr != p) {
        ifindex = if_nametoindex(p->ifa_name);
        if (0U == ifindex) {
            break;
        }

        if (static_cast< std::uint32_t >(IFF_LOOPBACK) == (p->ifa_flags & static_cast< std::uint32_t >(IFF_LOOPBACK))) {
            p = p->ifa_next;
            continue;
        }

        if (p->ifa_addr != nullptr) {
            void *const tmpAddrPtr{&(reinterpret_cast< struct sockaddr_in * >(p->ifa_addr))->sin_addr};
            std::array< ara::tsync::internal::char8_t, INET_ADDRSTRLEN > addressBuffer{};
            std::uint32_t const addLen{static_cast< std::uint32_t >(INET_ADDRSTRLEN)};
            static_cast< void >(inet_ntop(AF_INET, tmpAddrPtr, addressBuffer.data(), addLen));
            if (0 == ipAddr.compare(addressBuffer.data())) {
                netName = ara::core::String(p->ifa_name);
                break;
            }
        }
        p = p->ifa_next;
    }
    freeifaddrs(ifap);
    return netName;
}

/// @brief get the MAC address of the specified network card
/// @param deviceName - network device name
/// @param addr - used to store the obtained MAC address
/// @return 0 - success
/// @return <0 - failure
std::int32_t GetMacAddress(std::string const &deviceName, Ether::Address &addr) noexcept
{
    /// Get the MAC address of the specified network card
    ifreq ifMac{};
    std::int32_t ifIndex{0};
    std::int32_t sockfd{0};

    /// Get network card index number
    ifIndex = static_cast< std::int32_t >(if_nametoindex(deviceName.c_str()));
    if (0 == ifIndex) {
        return ara::tsync::internal::kRET_E1;
    }
    std::ignore = memset(&ifMac, 0, sizeof(ifMac));
    sockfd      = ::socket(AF_INET, SOCK_DGRAM, 0);
    if (0 > sockfd) {
        return ara::tsync::internal::kRET_E2;
    }
    std::size_t macStrlen{sizeof(ifMac.ifr_name)};
    if (macStrlen > deviceName.length()) {
        macStrlen = deviceName.length();
    }
    std::ignore
        = strncpy(static_cast< ara::tsync::internal::char8_t * >(&ifMac.ifr_name[0]), deviceName.c_str(), macStrlen);
    std::ignore       = ifMac.ifr_name[0];
    ifMac.ifr_ifindex = ifIndex;
    std::uint64_t const requestId{static_cast< std::uint64_t >(SIOCGIFHWADDR)};
    if (0 > ioctl(sockfd, requestId, &ifMac)) {
        return ara::tsync::internal::kRET_E3;
    }
    std::ignore = ::memmove(static_cast< std::uint8_t * >(&addr.addrBytes[0]),
                            static_cast< ara::tsync::internal::char8_t * >(&ifMac.ifr_hwaddr.sa_data[0]),
                            Ether::Address::kLength);
    std::ignore = addr.addrBytes[0];
    return 0;
}

}  // namespace network
}  // namespace osi
}  // namespace isoft
