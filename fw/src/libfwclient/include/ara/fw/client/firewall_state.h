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
/// @file       firewall_state.h
/// @brief      Firewall external interface
/// @details    Firewall external interface
/// @date       2025-03-21
/// @author     jzy
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/fw/lib client
/// @interface_level=module
/// export_level=module
/// @trace_id_sr=SR_FW_0011
/// @unit_name=Firewall_Libapi
/// @unit_description=Firewall external API interface class
/// @endcode
///
/// ================================================================
///
///
/// ================================================================

#include <ara/core/future.h>
#include <ara/core/promise.h>
#include <ara/core/result.h>

#include "ara/core/instance_specifier.h"
#include "ara/fw/client/fw_client.h"

namespace ara {
namespace fw {

/// @brief Firewall switching class
/// @code{.isoft}
/// @tparam EnumT
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00112
/// @trace_id_dd=DD_FW_00188
/// @needwork = ad
/// @endcode
template < typename EnumT >
class FirewallStateSwitchInterface final
{
public:
    /// @brief Firewall switching destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00113
    /// @trace_id_dd=DD_FW_00189
    /// @needwork = ad
    /// @endcode
    ~FirewallStateSwitchInterface() noexcept = default;

    /// @brief Firewall instance.
    /// @param instance Instance descriptor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00114
    /// @trace_id_dd=DD_FW_00190
    /// @needwork = ad
    /// @endcode
    explicit FirewallStateSwitchInterface(ara::core::InstanceSpecifier instance) noexcept
        : clientInstanceSpec_(std::move(instance))
    {
    }

    /// @brief Firewall switching copy constructor
    /// @param se Copy instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00115
    /// @trace_id_dd=DD_FW_00191
    /// @needwork = ad
    /// @endcode
    FirewallStateSwitchInterface(const FirewallStateSwitchInterface &se) = delete;

    /// @brief Delete copy operator
    /// @param se Switching class parameter
    /// @return Delete copy operator
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00116
    /// @trace_id_dd=DD_FW_00192
    /// @needwork = ad
    /// @endcode
    FirewallStateSwitchInterface &operator=(const FirewallStateSwitchInterface &se) = delete;

    /// @brief Firewall
    /// @param se Firewall switching class parameter
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00117
    /// @trace_id_dd=DD_FW_00193
    /// @needwork = ad
    /// @endcode
    FirewallStateSwitchInterface(FirewallStateSwitchInterface &&se) noexcept
        : clientInstanceSpec_(std::move(se.clientInstanceSpec_)){};

    /// @brief Move semantics operator
    /// @param se Firewall switching class parameter
    /// @return Moved instance descriptor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00118
    /// @trace_id_dd=DD_FW_00194
    /// @needwork = ad
    /// @endcode
    FirewallStateSwitchInterface &operator=(FirewallStateSwitchInterface &&se) noexcept
    {
        clientInstanceSpec_ = std::move(se.clientInstanceSpec_);
        return *this;
    }

    /// @brief Firewall state switching processing function
    /// @param firewallState Firewall state
    /// @return Switching result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00119
    /// @trace_id_dd=DD_FW_00195
    /// @needwork = ad
    /// @endcode
    ara::core::Future< void > SwitchFirewallState(EnumT firewallState) noexcept
    {
        ara::core::Promise< void > promise{};
        ara::fw::internal::FWClient *const client{ara::fw::internal::FWClient::GetInstance()};
        ara::core::Result< void > ret{
            client->SwitchFirewallState(clientInstanceSpec_, static_cast< uint32_t >(firewallState))};
        if (!ret.HasValue()) {
            promise.SetError(std::move(ret).Error());
            return promise.get_future();
        }
        promise.set_value();
        return promise.get_future();
    }

private:
    /// @brief Instance descriptor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00196
    /// @needwork = dda
    /// @endcode
    ara::core::InstanceSpecifier clientInstanceSpec_;
};

}  // namespace fw
}  // namespace ara
