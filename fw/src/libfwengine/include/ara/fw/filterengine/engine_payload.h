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
/// @file       engine_payload.h
/// @brief      payload rule filtering
/// @details    payload rule filtering
/// @date       2025-10-15
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
/// @unit_name=Engine_Payload
/// @unit_description=Firewall engine abstraction layer payload data abstraction
/// processing class
/// @endcode
///
/// ================================================================
///
///
/// ================================================================

#ifndef _ARA_FW_ENGINE_PAYLOAD_H_
#define _ARA_FW_ENGINE_PAYLOAD_H_
#include <ara/core/string.h>

#include <cstdint>
#include <cstdlib>
#include <cstring>
namespace ara {
namespace fw {
namespace internal {

/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00018
/// @trace_id_dd=DD_FW_00024
/// @needwork = ad
/// @endcode
class PayloadEngine final
{
public:
    /// @brief Constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00019
    /// @trace_id_dd=DD_FW_00025
    /// @needwork = ad
    /// @endcode
    explicit PayloadEngine(int32_t action, int32_t inOutFlag, int32_t pRate = -1, int32_t qLength = -1) noexcept;
    /// @brief Default destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00020
    /// @trace_id_dd=DD_FW_00026
    /// @needwork = ad
    /// @endcode
    ~PayloadEngine() = default;

    /// @brief Default copy assignment function
    /// @param other Another object instance of this class
    /// @returns
    /// @throws
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00021
    /// @trace_id_dd=DD_FW_00027
    /// @needwork = ad
    /// @endcode
    PayloadEngine &operator=(PayloadEngine const &other) = delete;
    /// @brief Default move assignment function
    /// @param other Another object instance of this class
    /// @returns
    /// @throws
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00022
    /// @trace_id_dd=DD_FW_00028
    /// @needwork = ad
    /// @endcode
    PayloadEngine &operator=(PayloadEngine &&other) = delete;
    /// @brief Default move constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00023
    /// @trace_id_dd=DD_FW_00029
    /// @needwork = ad
    /// @endcode
    PayloadEngine(PayloadEngine &&other) = delete;
    /// @brief Default copy constructor
    /// @param other Another object instance of this class
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00024
    /// @trace_id_dd=DD_FW_00030
    /// @needwork = ad
    /// @endcode
    PayloadEngine(PayloadEngine const &other) noexcept = delete;
    /// @brief Set address filtering command (hook point is input.)
    /// @param iProtocol
    /// @param strProtocol

public:
    /// @brief Set UDP port range filtering
    /// @param offset Offset position
    /// @param value Value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00025
    /// @trace_id_dd=DD_FW_00031
    /// @needwork = ad
    /// @endcode
    void SetOffsetAndValueFilter(int32_t offset, int32_t value) const noexcept;

private:
    /// @brief Local filter rule action.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00032
    /// @needwork = dda
    /// @endcode
    int32_t action_{-1};
    /// @brief IN/OUT Flag.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00033
    /// @needwork = dda
    /// @endcode
    int32_t inOutFlag_{-1};
    /// @brief Rate
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00034
    /// @needwork = dda
    /// @endcode
    int32_t packetRate_{-1};
    /// @brief Queue length
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00035
    /// @needwork = dda
    /// @endcode
    int32_t queueLength_{-1};
    /// @brief log prefix identifier
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00036
    /// @needwork = dda
    /// @endcode
    ara::core::String logPrefix_{"Firewall-Drop"};
};

}  // namespace internal
}  // namespace fw
}  // namespace ara
#endif