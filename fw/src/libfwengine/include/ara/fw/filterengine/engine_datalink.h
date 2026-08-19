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
/// @file       engine_datalink.h
/// @brief      datalink Rule
/// @details    datalink Rule
/// @date       2025-05-23
/// @author     jzy
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/fw/Firewall Engine Abstraction Layer
/// @interface_level=module
/// export_level=module
/// @trace_id_sr=SR_FW_0010
/// @unit_name=Engine_Datalink
/// @unit_description=Firewall engine abstraction layer common part
/// @endcode
///
/// ================================================================
///
///
/// ================================================================

#ifndef _ARA_FW_ENGINE_DATALINK_H_
#define _ARA_FW_ENGINE_DATALINK_H_
#include <ara/core/string.h>

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <vector>

#include "ara/fw/common/common.h"
namespace ara {
namespace fw {
namespace internal {

/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00089
/// @trace_id_dd=DD_FW_00143
/// @needwork = ad
/// @endcode
class DataLinkEngine final
{
public:
    /// @brief Constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00090
    /// @trace_id_dd=DD_FW_00144
    /// @needwork = ad
    /// @endcode
    explicit DataLinkEngine(int32_t action, int32_t inOutFlag, int32_t pRate = -1, int32_t qLength = -1) noexcept;
    /// @brief Default destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00091
    /// @trace_id_dd=DD_FW_00145
    /// @needwork = ad
    /// @endcode
    ~DataLinkEngine() = default;

    /// @brief Default copy assignment function
    /// @param other Another object instance of this class
    /// @returns
    /// @throws
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00092
    /// @trace_id_dd=DD_FW_00146
    /// @needwork = ad
    /// @endcode
    DataLinkEngine &operator=(DataLinkEngine const &other) = delete;
    /// @brief Default move assignment function
    /// @param other Another object instance of this class
    /// @returns
    /// @throws
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00093
    /// @trace_id_dd=DD_FW_00147
    /// @needwork = ad
    /// @endcode
    DataLinkEngine &operator=(DataLinkEngine &&other) = delete;
    /// @brief Default move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00094
    /// @trace_id_dd=DD_FW_00148
    /// @needwork = ad
    /// @endcode
    DataLinkEngine(DataLinkEngine &&other) = delete;
    /// @brief Default copy constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00095
    /// @trace_id_dd=DD_FW_00149
    /// @needwork = ad
    /// @endcode
    DataLinkEngine(DataLinkEngine const &other) noexcept = delete;
    /// @brief Set address filtering command (hook point is input.)
    /// @param iProtocol
    /// @param strProtocol

public:
    /// @brief Set source MAC address and destination MAC address rule filtering
    /// @param srcMacAddr Source MAC address
    /// @param srcMacAddrMask Source address MASK
    /// @param destMacAddr Destination MAC address
    /// @param destMacAddrMask Destination address MASK
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00096
    /// @trace_id_dd=DD_FW_00150
    /// @needwork = ad
    /// @endcode
    void SetMacAddressFilter(ara::core::String const &srcMacAddr,
                             ara::core::String const &srcMacAddrMask,
                             ara::core::String const &destMacAddr,
                             ara::core::String const &destMacAddrMask) noexcept;

    /// @brief Set ether type filtering
    /// @param etherType  ether type.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00097
    /// @trace_id_dd=DD_FW_00151
    /// @needwork = ad
    /// @endcode
    void SetEtherTypeFilter(int32_t etherType) const noexcept;

private:
    /// @brief Parse MAC address string
    /// @param macStr
    /// @param macBytes
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00152
    /// @needwork = dda
    /// @endcode
    bool _parseMAC(const ara::core::String &macStr, std::vector< uint8_t > &macBytes) noexcept;

    /// @brief Assemble into MAC address string
    /// @param macBytes
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00153
    /// @needwork = dda
    /// @endcode
    ara::core::String _formatMAC(const std::vector< uint8_t > &macBytes) noexcept;

    /// @brief Get the corresponding MAC range of the firewall through MAC and
    /// subnet mask
    /// @param mac MAC address
    /// @param macMask MAC address subnet mask
    /// @return Get the corresponding ip range.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00154
    /// @needwork = dda
    /// @endcode
    ara::core::String _getMacRange(ara::core::String const &mac, ara::core::String const &macMask) noexcept;

    /// @brief Determine whether the character is a hexadecimal data character
    /// @param c Character
    /// @return Yes/No
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00155
    /// @needwork = dda
    /// @endcode
    bool _isHexChar(char8_t c) noexcept;

private:
    /// @brief Local filter rule action.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00156
    /// @needwork = dda
    /// @endcode
    int32_t action_{-1};
    /// @brief IN/OUT Flag.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00157
    /// @needwork = dda
    /// @endcode
    int32_t inOutFlag_{-1};
    /// @brief Rate
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00158
    /// @needwork = dda
    /// @endcode
    int32_t packetRate_{-1};
    /// @brief Queue length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00159
    /// @needwork = dda
    /// @endcode
    int32_t queueLength_{-1};
    /// @brief log prefix identifier
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00160
    /// @needwork = dda
    /// @endcode
    ara::core::String logPrefix_{"Firewall-Drop"};
};

}  // namespace internal
}  // namespace fw
}  // namespace ara
#endif