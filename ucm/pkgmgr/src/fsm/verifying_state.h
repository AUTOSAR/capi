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
/// @file       verifying_state.h
/// @brief      Verifying state class
/// @details
/// @date       2024-08-15
/// @author     cuiyinli
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @export_level=/UCM/FsmManager
/// @module_path=/UCM/FsmManager
/// @interface_level=unit
/// @trace_id_sr=SR_UCM_00001,SR_UCM_00021
/// @unit_name=VerifyingState
/// @unit_description=Verifying state class
/// @endcode
///
/// ================================================================

#ifndef ARA_UCM_PKGMGR_FSM_VERIFYING_STATE_H_
#define ARA_UCM_PKGMGR_FSM_VERIFYING_STATE_H_

#include "package_manager_state.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief Class representing Verifying state of Package Manager
/// It is designed according to the State Pattern
///
/// TODO: check function group state AR-87037
/// @code{.isoft}
/// @uptrace={SWS_UCM_00085}
/// TODO: since we fake the verification this is currently invalid
/// AR-87037
/// @uptrace={SWS_UCM_00107}
/// @uptrace={SWS_UCM_00108}
/// TODO: Implement the automatic rollback based on verifying result
/// @uptrace={SWS_UCM_00155}
/// @uptrace={SWS_UCM_00126, f071c00af3554920e1527d31308542ae480c746f}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10003
/// @trace_id_dd=DD_UCM_10034
/// @needwork = ad
/// @endcode
class VerifyingState final : public PackageManagerState
{
public:
    /// @brief Creates a new instance of Verifying state and saves the current state
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10004
    /// @trace_id_dd=DD_UCM_10035
    /// @needwork = ad
    /// @endcode
    VerifyingState() = default;
    /// @brief destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10005
    /// @trace_id_dd=DD_UCM_10036
    /// @needwork = ad
    /// @endcode
    ~VerifyingState() final = default;

    /// @brief delete copy construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10006
    /// @trace_id_dd=DD_UCM_10037
    /// @needwork = ad
    /// @endcode
    VerifyingState(VerifyingState const& other) = delete;
    /// @brief delete copy asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10007
    /// @trace_id_dd=DD_UCM_10038
    /// @needwork = ad
    /// @endcode
    VerifyingState& operator=(VerifyingState const& other) = delete;
    /// @brief delete move construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10008
    /// @trace_id_dd=DD_UCM_10039
    /// @needwork = ad
    /// @endcode
    VerifyingState(VerifyingState&& other) = delete;
    /// @brief delete move asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10009
    /// @trace_id_dd=DD_UCM_10040
    /// @needwork = ad
    /// @endcode
    VerifyingState& operator=(VerifyingState&& other) = delete;

    /// @brief Retrieve the Status which corresponds to the state.
    ///
    /// @return The corresponding status.
    /// @throws no
    /// @code{.isoft}
    /// <inheritdoc>
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10010
    /// @trace_id_dd=DD_UCM_10041
    /// @needwork = ad
    /// @endcode
    PackageManagerStatusType GetStatus() const noexcept final { return PackageManagerStatusType::kVerifying; }

    /// @brief Rollback the system to the state before the packages were processed.
    /// @return The result of the rollback operation
    /// @throws no
    /// @code{.isoft}
    /// <inheritdoc>
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10011
    /// @trace_id_dd=DD_UCM_10042
    /// @needwork = ad
    /// @endcode
    AraFutureVoid Rollback() final;

    /// @brief OnSuccess
    /// @throws no
    /// @return result
    /// @code{.isoft}
    /// <inheritdoc>
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10012
    /// @trace_id_dd=DD_UCM_10043
    /// @needwork = ad
    /// @endcode
    AraResultVoid OnSuccess() noexcept final;

    /// @brief OnFailure
    /// @throws no
    /// @return result
    /// @code{.isoft}
    /// <inheritdoc>
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10013
    /// @trace_id_dd=DD_UCM_10044
    /// @needwork = ad
    /// @endcode
    AraResultVoid OnFailure() noexcept final;
};

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // ARA_UCM_PKGMGR_FSM_VERIFYING_STATE_H_
