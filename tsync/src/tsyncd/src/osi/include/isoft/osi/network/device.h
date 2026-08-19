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
/// @file       device.h
/// @brief      OSI network module device class
/// @details
/// @date       2023-01-09
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#ifndef ISOFT_OSI_NETWORK_DEVICE_H_
#define ISOFT_OSI_NETWORK_DEVICE_H_

#include <ara/core/string.h>

#include <cstdint>
#include <string>
#include <vector>

#include "isoft/osi/network/ether.h"

namespace isoft {
namespace osi {
namespace network {

/// @brief basic network device information
class Device final
{
public:
    /// @name devIndex - device index number
    std::uint32_t devIndex{0};

    /// @name name - device name
    std::string name;

    /// @name isLowerUp - whether online
    bool isLowerUp{false};
};

/// @brief get the list of local network devices
/// @param netdevs - device list
/// @return 0 - success
/// @return <0 - failure
std::int32_t GetDevices(std::vector< Device > &netdevs) noexcept;

/// @brief get the MAC address of the specified network card
/// @param deviceName - network device name
/// @param addr - used to store the obtained MAC address
/// @return 0 - success
/// @return <0 - failure
std::int32_t GetMacAddress(std::string const &deviceName, Ether::Address &addr) noexcept;

/// @brief find network card name by network card IP
/// @param ipAddr - network card IP
/// @return network card name
ara::core::String GetDeviceName(ara::core::String const &ipAddr) noexcept;

}  // namespace network
}  // namespace osi
}  // namespace isoft

#endif  /// ISOFT_OSI_NETWORK_DEVICE_H_
