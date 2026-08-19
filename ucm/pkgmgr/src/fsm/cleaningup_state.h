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
/// @file       cleaningup_state.h
/// @brief      CleaningUp state class
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
/// @trace_id_sr=SR_UCM_00001, SR_UCM_00015
/// @unit_name=CleaningUpState
/// @unit_description=Handles the cleanup state of the package manager.
/// @endcode
///
/// ================================================================

#ifndef ARA_UCM_PKGMGR_FSM_CLEANINGUP_STATE_H_
#define ARA_UCM_PKGMGR_FSM_CLEANINGUP_STATE_H_

#include "common/alias.h"
#include "package_manager_state.h"
#include "types/impl_type_finalactiontype.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief Class representing CleaningUp state of Package Manager
/// It is designed according to the State Pattern
///
/// @code{.isoft}
/// @uptrace={SWS_UCM_00163, e230e1493a7e326b50004053df49f1ecb2cfe532}
/// @uptrace={SWS_UCM_00164, 595772a0a72c9b6532bd31b77a762920a5a3e404}
/// @uptrace={SWS_UCM_00127, 371487b3dddf330637cecedd78deb9168ed58b17}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10014
/// @trace_id_dd=DD_UCM_10045
/// @needwork = ad
/// @endcode
class CleaningUpState final : public PackageManagerState
{
public:
    /// @brief Creates a new instance of CleaningUp state and saves the current state
    ///
    /// @param finalAction commit or revert, defines if all actions should be reverted in CLEANING_UP state
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10015
    /// @trace_id_dd=DD_UCM_10046
    /// @needwork = ad
    /// @endcode
    explicit CleaningUpState(FinalActionType const finalAction) noexcept;
    /// @brief destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10016
    /// @trace_id_dd=DD_UCM_10047
    /// @needwork = ad
    /// @endcode
    ~CleaningUpState() final = default;

    /// @brief delete copy construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10017
    /// @trace_id_dd=DD_UCM_10048
    /// @needwork = ad
    /// @endcode
    CleaningUpState(CleaningUpState const& other) = delete;
    /// @brief delete copy asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10018
    /// @trace_id_dd=DD_UCM_10049
    /// @needwork = ad
    /// @endcode
    CleaningUpState& operator=(CleaningUpState const& other) = delete;
    /// @brief delete move construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10019
    /// @trace_id_dd=DD_UCM_10050
    /// @needwork = ad
    /// @endcode
    CleaningUpState(CleaningUpState&& other) = delete;
    /// @brief delete move asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10020
    /// @trace_id_dd=DD_UCM_10051
    /// @needwork = ad
    /// @endcode
    CleaningUpState& operator=(CleaningUpState&& other) = delete;

    /// @brief Retrieve the Status which corresponds to the state.
    ///
    /// @return The corresponding status.
    /// @throws no
    /// @code{.isoft}
    /// <inheritdoc>
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10021
    /// @trace_id_dd=DD_UCM_10052
    /// @needwork = ad
    /// @endcode
    PackageManagerStatusType GetStatus() const noexcept final { return PackageManagerStatusType::kCleaningUp; }

    /// @brief OnFailure
    /// @return result
    /// @throws no
    /// @code{.isoft}
    /// Internal message
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10022
    /// @trace_id_dd=DD_UCM_10053
    /// @needwork = ad
    /// @endcode
    AraResultVoid OnFailure() noexcept final;

    /// @brief Done
    /// @throws no
    /// @code{.isoft}
    /// <inheritdoc>
    /// @uptrace={SWS_UCM_00147, 3feb72562c5062cc1614f9d61e8d84dd7bbda203}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10023
    /// @trace_id_dd=DD_UCM_10054
    /// @needwork = ad
    /// @endcode
    void Done() noexcept final;

    // Get FinalActionType
    /// @brief GetFinalActionType
    /// @return FinalActionType
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10024
    /// @trace_id_dd=DD_UCM_10055
    /// @needwork = ad
    /// @endcode
    FinalActionType GetFinalActionType() const noexcept final { return finalAction_; }

private:
    /// @brief finalAction_
    /// @code{.isoft}
    /// Defines, if all executed actions at the cleanup should be reverted or not.
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10056
    /// @needwork = dda
    /// @endcode
    FinalActionType finalAction_;
};

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // ARA_UCM_PKGMGR_FSM_CLEANINGUP_STATE_H_
