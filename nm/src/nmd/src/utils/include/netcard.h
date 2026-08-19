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
/// @file       netcard.h
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

#ifndef _ARA_NM_NETCARD_H_
#define _ARA_NM_NETCARD_H_
#include <ara/core/string.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include <cstdint>

namespace ara {
namespace nm {
namespace internal {

/// @brief Check if the NIC is link-up.
/// @param ipv4Addr NIC IP address.
/// @returns true link-up
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100069
/// @trace_id_dd=DD_NM_00813
/// @needwork = ad
/// @endcode
bool CheckNetCard(ara::core::String const &ipv4Addr) noexcept;

/// @brief Get NIC name.
/// @param ipv4Addr IP address
/// @returns NIC name
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100070
/// @trace_id_dd=DD_NM_00814
/// @needwork = ad
/// @endcode
ara::core::String GetNetCardName(ara::core::String const &ipv4Addr) noexcept;

/// @brief Get NIC MAC address.
/// @param deviceName Device name
/// @param addrBytes MAC address
/// @returns 0 if operation succeeded
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100071
/// @trace_id_dd=DD_NM_00815
/// @needwork = ad
/// @endcode
std::int32_t GetMacAddress(ara::core::String const &deviceName, std::uint8_t addrBytes[]) noexcept;

/// @brief Get the multicast MAC address corresponding to the specified multicast IP.
/// @param multicastIp Multicast IP
/// @param muticastMAcaddr Multicast MAC address
/// @returns 0 if operation succeeded
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100072
/// @trace_id_dd=DD_NM_00816
/// @needwork = ad
/// @endcode
std::int32_t GetMulticastMacAddress(ara::core::String const &multicastIp,
                                    std::uint8_t muticastMAcaddr[ETH_ALEN]) noexcept;

/// @brief Initialize to check if the NIC is abnormal. If abnormal, record DTC event.
/// @param ifName NIC name
/// @returns true if check is normal
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100073
/// @trace_id_dd=DD_NM_00817
/// @needwork = ad
/// @endcode
bool InitCheckNetCard(ara::core::String const &ifName) noexcept;

}  // namespace internal
}  // namespace nm
}  // namespace ara

#endif  // _ARA_NM_NETCARD_H_