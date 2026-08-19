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
/// @file       engine_tcp.h
/// @brief      tcp rule filtering
/// @details    tcp rule filtering
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
/// @unit_name=Engine_Tcp
/// @unit_description=Firewall engine abstraction layer tcp data abstraction
/// processing class
/// @endcode
///
/// ================================================================
///
///
/// ================================================================

#ifndef _ARA_FW_ENGINE_TCP_H_
#define _ARA_FW_ENGINE_TCP_H_
#include <ara/core/string.h>

#include <cstdint>
#include <cstdlib>
#include <cstring>
namespace ara {
namespace fw {
namespace internal {

/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00068
/// @trace_id_dd=DD_FW_00112
/// @needwork = ad
/// @endcode
class TcpEngine final
{
public:
    /// @brief Constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00069
    /// @trace_id_dd=DD_FW_00113
    /// @needwork = ad
    /// @endcode
    explicit TcpEngine(int32_t action, int32_t inOutFlag, int32_t pRate = -1, int32_t qLength = -1) noexcept;
    /// @brief Default destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00070
    /// @trace_id_dd=DD_FW_00114
    /// @needwork = ad
    /// @endcode
    ~TcpEngine() = default;

    /// @brief Default copy assignment function
    /// @param other Another object instance of this class
    /// @returns
    /// @throws
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00071
    /// @trace_id_dd=DD_FW_00115
    /// @needwork = ad
    /// @endcode
    TcpEngine &operator=(TcpEngine const &other) = delete;
    /// @brief Default move assignment function
    /// @param other Another object instance of this class
    /// @returns
    /// @throws
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00072
    /// @trace_id_dd=DD_FW_00116
    /// @needwork = ad
    /// @endcode
    TcpEngine &operator=(TcpEngine &&other) = delete;
    /// @brief Default move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00073
    /// @trace_id_dd=DD_FW_00117
    /// @needwork = ad
    /// @endcode
    TcpEngine(TcpEngine &&other) = delete;
    /// @brief Default copy constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00074
    /// @trace_id_dd=DD_FW_00118
    /// @needwork = ad
    /// @endcode
    TcpEngine(TcpEngine const &other) noexcept = delete;
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
    /// @trace_id_ad=AD_FW_00075
    /// @trace_id_dd=DD_FW_00119
    /// @needwork = ad
    /// @endcode
    void SetTcpPortRangeFilter(int32_t sPortMin, int32_t sPortMax, int32_t pPortMin, int32_t pPortMax) const noexcept;

    /// @brief Set whether to perform udp checksum rule filtering (currently the
    /// Linux kernel performs checksum by default, this function does nothing)
    /// @param checkSumFlag Whether to perform checksum flag.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00076
    /// @trace_id_dd=DD_FW_00120
    /// @needwork = ad
    /// @endcode
    void SetTcpCheckSumFilter(bool checkSumFlag) const noexcept;

    /// @brief TCP max session connection count filtering.
    /// @param maxSessionConn TCP set maximum connection count.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00077
    /// @trace_id_dd=DD_FW_00121
    /// @needwork = ad
    /// @endcode
    void SetTcpMaxSeesionFilter(int32_t maxSessionConn) const noexcept;

    /// @brief TCP set state transition Flags filtering.
    /// @param tcpSmFlags tcp state transition flags
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00078
    /// @trace_id_dd=DD_FW_00122
    /// @needwork = ad
    /// @endcode
    void SetTcpSMFlagsFilter(bool tcpSmFlags) const noexcept;

    /// @brief TCP set TCP session timeout filtering.
    /// @param timeout tcp session timeout time.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00079
    /// @trace_id_dd=DD_FW_00123
    /// @needwork = ad
    /// @endcode
    void SetTcpTimeOutFilter(int32_t timeout) const noexcept;

private:
    /// @brief Local filter rule action.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00124
    /// @needwork = dda
    /// @endcode
    int32_t action_{-1};
    /// @brief IN/OUT Flag.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00125
    /// @needwork = dda
    /// @endcode
    int32_t inOutFlag_{-1};
    /// @brief Rate
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00126
    /// @needwork = dda
    /// @endcode
    int32_t packetRate_{-1};
    /// @brief Queue length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00127
    /// @needwork = dda
    /// @endcode
    int32_t queueLength_{-1};
    /// @brief log prefix identifier
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00128
    /// @needwork = dda
    /// @endcode
    ara::core::String logPrefix_{"Firewall-Drop"};
};

}  // namespace internal
}  // namespace fw
}  // namespace ara
#endif