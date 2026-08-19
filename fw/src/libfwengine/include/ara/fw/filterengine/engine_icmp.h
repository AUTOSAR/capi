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
/// @file       engine_icmp.h
/// @brief      icmp rule filtering
/// @details    icmp rule filtering
/// @date       2025-05-28
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
/// @unit_name=Engine_Icmp
/// @unit_description=Firewall engine abstraction layer icmp data abstraction
/// processing class
/// @endcode
///
/// ================================================================
///
///
/// ================================================================

#ifndef _ARA_FW_ENGINE_ICMP_H_
#define _ARA_FW_ENGINE_ICMP_H_
#include <ara/core/string.h>

#include <cstdint>
#include <cstdlib>
#include <cstring>
namespace ara {
namespace fw {
namespace internal {

/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00058
/// @trace_id_dd=DD_FW_00096
/// @needwork = ad
/// @endcode
class IcmpEngine final
{
public:
    /// @brief Constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00059
    /// @trace_id_dd=DD_FW_00097
    /// @needwork = ad
    /// @endcode
    explicit IcmpEngine(
        int32_t action, int32_t inOutFlag, int32_t ipv4Or6Flag, int32_t pRate = -1, int32_t qLength = -1) noexcept;
    /// @brief Default destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00060
    /// @trace_id_dd=DD_FW_00098
    /// @needwork = ad
    /// @endcode
    ~IcmpEngine() = default;

    /// @brief Default copy assignment function
    /// @param other Another object instance of this class
    /// @returns
    /// @throws
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00061
    /// @trace_id_dd=DD_FW_00099
    /// @needwork = ad
    /// @endcode
    IcmpEngine &operator=(IcmpEngine const &other) = delete;
    /// @brief Default move assignment function
    /// @param other Another object instance of this class
    /// @returns
    /// @throws
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00062
    /// @trace_id_dd=DD_FW_00100
    /// @needwork = ad
    /// @endcode
    IcmpEngine &operator=(IcmpEngine &&other) = delete;
    /// @brief Default move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00063
    /// @trace_id_dd=DD_FW_00101
    /// @needwork = ad
    /// @endcode
    IcmpEngine(IcmpEngine &&other) = delete;
    /// @brief Default copy constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00064
    /// @trace_id_dd=DD_FW_00102
    /// @needwork = ad
    /// @endcode
    IcmpEngine(IcmpEngine const &other) noexcept = delete;
    /// @brief Set address filtering command (hook point is input.)
    /// @param iProtocol
    /// @param strProtocol

public:
    /// @brief Set Icmp Type filtering rule
    /// @param type icmp Type value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00065
    /// @trace_id_dd=DD_FW_00103
    /// @needwork = ad
    /// @endcode
    void SetTypeFilter(int32_t type) const noexcept;

    /// @brief Set Icmp Code filtering rule
    /// @param code icmp Code value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00066
    /// @trace_id_dd=DD_FW_00104
    /// @needwork = ad
    /// @endcode
    void SetCodeFilter(int32_t code) const noexcept;

    /// @brief Set whether to perform Icmp checksum rule filtering (currently the
    /// Linux kernel performs checksum by default, this function does nothing)
    /// @param checkSumFlag Whether to perform checksum flag.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00067
    /// @trace_id_dd=DD_FW_00105
    /// @needwork = ad
    /// @endcode
    void SetCheckSumFilter(bool checkSumFlag) noexcept;

private:
    /// @brief Local filter rule action.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00106
    /// @needwork = dda
    /// @endcode
    int32_t action_{-1};
    /// @brief IN/OUT Flag.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00107
    /// @needwork = dda
    /// @endcode
    int32_t inOutFlag_{-1};
    /// @brief ipv4 ipv6 Flag.(0:ipv4 | 1:ipv6)
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00108
    /// @needwork = dda
    /// @endcode
    int32_t ipv4Or6Flag_{-1};
    /// @brief Rate
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00109
    /// @needwork = dda
    /// @endcode
    int32_t packetRate_{-1};
    /// @brief Queue length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00110
    /// @needwork = dda
    /// @endcode
    int32_t queueLength_{-1};
    /// @brief log prefix identifier
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00111
    /// @needwork = dda
    /// @endcode
    ara::core::String logPrefix_{"Firewall-Drop"};
};

}  // namespace internal
}  // namespace fw
}  // namespace ara
#endif