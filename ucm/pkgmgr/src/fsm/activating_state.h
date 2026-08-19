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
/// @file       activating_state.h
/// @brief      Activating state class
/// @details
/// @date       2024-01-01
/// @author     cuiyinli
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @export_level=/UCM/FsmManager
/// @module_path=/UCM/FsmManager
/// @interface_level=unit
/// @trace_id_sr=SR_UCM_00001,SR_UCM_00021,SR_UCM_00030
/// @unit_name=ActivatingState
/// @unit_description=Handles the activating state of the package manager.
/// @endcode
///
/// ================================================================

#ifndef ARA_UCM_PKGMGR_FSM_ACTIVATING_STATE_H_
#define ARA_UCM_PKGMGR_FSM_ACTIVATING_STATE_H_

#include "package_manager_state.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief Class representing Activating state of Package Manager
///
/// @code{.isoft}
/// @uptrace={SWS_UCM_00154, 7af5c10177916c53dcab67ff0013028824865161}
/// @uptrace={SWS_UCM_00152, 67e38cd738125fba65546238f7e255c30863e777}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10025
/// @trace_id_dd=DD_UCM_10057
/// @needwork = ad
/// @endcode
class ActivatingState final : public PackageManagerState
{
public:
    /// @brief Creates a new instance of Activating state and saves the current state
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10026
    /// @trace_id_dd=DD_UCM_10058
    /// @needwork = ad
    /// @endcode
    ActivatingState() = default;
    /// @brief destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10027
    /// @trace_id_dd=DD_UCM_10059
    /// @needwork = ad
    /// @endcode
    ~ActivatingState() final = default;

    /// @brief delete copy construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10028
    /// @trace_id_dd=DD_UCM_10060
    /// @needwork = ad
    /// @endcode
    ActivatingState(ActivatingState const& other) = delete;
    /// @brief delete copy asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10029
    /// @trace_id_dd=DD_UCM_10061
    /// @needwork = ad
    /// @endcode
    ActivatingState& operator=(ActivatingState const& other) = delete;
    /// @brief delete move construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10030
    /// @trace_id_dd=DD_UCM_10062
    /// @needwork = ad
    /// @endcode
    ActivatingState(ActivatingState&& other) = delete;
    /// @brief delete move asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10031
    /// @trace_id_dd=DD_UCM_10063
    /// @needwork = ad
    /// @endcode
    ActivatingState& operator=(ActivatingState&& other) = delete;

    /// @brief Retrieve the Status which corresponds to the state.
    ///
    /// @return The corresponding status.
    /// @throws no
    /// @code{.isoft}
    /// <inheritdoc>
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10032
    /// @trace_id_dd=DD_UCM_10064
    /// @needwork = ad
    /// @endcode
    PackageManagerStatusType GetStatus() const noexcept final { return PackageManagerStatusType::kActivating; }

    /// @brief OnSuccess
    /// @return result
    /// @throws no
    /// @code{.isoft}
    /// <inheritdoc>
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10033
    /// @trace_id_dd=DD_UCM_10065
    /// @needwork = ad
    /// @endcode
    AraResultVoid OnSuccess() noexcept final;

    /// @brief OnFailure
    /// @return result
    /// @throws no
    /// @code{.isoft}
    /// <inheritdoc>
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10034
    /// @trace_id_dd=DD_UCM_10066
    /// @needwork = ad
    /// @endcode
    AraResultVoid OnFailure() noexcept final;
};

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // ARA_UCM_PKGMGR_FSM_ACTIVATING_STATE_H_
