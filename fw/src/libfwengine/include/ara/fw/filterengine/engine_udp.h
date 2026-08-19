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
/// @file       engine_udp.h
/// @brief      udp rule filtering
/// @details    udp rule filtering
/// @date       2025-06-09
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
/// @unit_name=Engine_Udp
/// @unit_description=Firewall engine abstraction layer udp data abstraction
/// processing class
/// @endcode
///
/// ================================================================
///
///
/// ================================================================

#ifndef _ARA_FW_ENGINE_UDP_H_
#define _ARA_FW_ENGINE_UDP_H_
#include <ara/core/string.h>

#include <cstdint>
#include <cstdlib>
#include <cstring>
namespace ara {
namespace fw {
namespace internal {

/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00080
/// @trace_id_dd=DD_FW_00129
/// @needwork = ad
/// @endcode
class UdpEngine final
{
public:
    /// @brief Constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00081
    /// @trace_id_dd=DD_FW_00130
    /// @needwork = ad
    /// @endcode
    explicit UdpEngine(int32_t action, int32_t inOutFlag, int32_t pRate = -1, int32_t qLength = -1) noexcept;
    /// @brief Default destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00082
    /// @trace_id_dd=DD_FW_00131
    /// @needwork = ad
    /// @endcode
    ~UdpEngine() = default;

    /// @brief Default copy assignment function
    /// @param other Another object instance of this class
    /// @returns
    /// @throws
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00083
    /// @trace_id_dd=DD_FW_00132
    /// @needwork = ad
    /// @endcode
    UdpEngine &operator=(UdpEngine const &other) = delete;
    /// @brief Default move assignment function
    /// @param other Another object instance of this class
    /// @returns
    /// @throws
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00084
    /// @trace_id_dd=DD_FW_00133
    /// @needwork = ad
    /// @endcode
    UdpEngine &operator=(UdpEngine &&other) = delete;
    /// @brief Default move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00085
    /// @trace_id_dd=DD_FW_00134
    /// @needwork = ad
    /// @endcode
    UdpEngine(UdpEngine &&other) = delete;
    /// @brief Default copy constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00086
    /// @trace_id_dd=DD_FW_00135
    /// @needwork = ad
    /// @endcode
    UdpEngine(UdpEngine const &other) noexcept = delete;
    /// @brief Set address filtering command (hook point is input.)
    /// @param iProtocol
    /// @param strProtocol

public:
    /// @brief Set UDP port range filtering
    /// @param sPortMin Source address port minimum value
    /// @param sPortMax Source address port maximum value
    /// @param pPortMin Destination address port minimum value
    /// @param pPortMax Destination address port maximum value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00087
    /// @trace_id_dd=DD_FW_00136
    /// @needwork = ad
    /// @endcode
    void SetPortRangeFilter(int32_t sPortMin, int32_t sPortMax, int32_t pPortMin, int32_t pPortMax) const noexcept;

    /// @brief Set whether to perform udp checksum rule filtering (currently the
    /// Linux kernel performs checksum by default, this function does nothing)
    /// @param checkSumFlag Whether to perform checksum flag.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00088
    /// @trace_id_dd=DD_FW_00137
    /// @needwork = ad
    /// @endcode
    void SetCheckSumFilter(bool checkSumFlag) const noexcept;

private:
    /// @brief Local filter rule action.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00138
    /// @needwork = dda
    /// @endcode
    int32_t action_{-1};
    /// @brief IN/OUT Flag.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00139
    /// @needwork = dda
    /// @endcode
    int32_t inOutFlag_{-1};
    /// @brief Rate
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00140
    /// @needwork = dda
    /// @endcode
    int32_t packetRate_{-1};
    /// @brief Queue length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00141
    /// @needwork = dda
    /// @endcode
    int32_t queueLength_{-1};
    /// @brief log prefix identifier
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00142
    /// @needwork = dda
    /// @endcode
    ara::core::String logPrefix_{"Firewall-Drop"};
};

}  // namespace internal
}  // namespace fw
}  // namespace ara
#endif