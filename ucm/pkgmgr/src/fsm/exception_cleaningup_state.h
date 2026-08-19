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
/// @file       exception_cleaningup_state.h
/// @brief      Exception CleaningUp state class
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
/// @trace_id_sr=SR_UCM_00001,SR_UCM_00027
/// @unit_name=ExceptionCleaningUpState
/// @unit_description=Exception CleaningUp state class
/// @endcode
///
/// ================================================================

#ifndef ARA_UCM_PKGMGR_FSM_EXCEPTION_CLEANINGUP_STATE_H_
#define ARA_UCM_PKGMGR_FSM_EXCEPTION_CLEANINGUP_STATE_H_

#include "package_manager_state.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief Class representing ExceptionCleaningUp state of Package Manager
/// It is designed according to the State Pattern
///
/// @code{.isoft}
/// @uptrace={SWS_UCM_00163, e230e1493a7e326b50004053df49f1ecb2cfe532}
/// @uptrace={SWS_UCM_00164, 595772a0a72c9b6532bd31b77a762920a5a3e404}
/// @uptrace={SWS_UCM_00127, 371487b3dddf330637cecedd78deb9168ed58b17}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10131
/// @trace_id_dd=DD_UCM_10204
/// @needwork = ad
/// @endcode
class ExceptionCleaningUpState final : public PackageManagerState
{
public:
    /// @brief Creates a new instance of ExceptionCleaningUp state and saves the current state
    ///
    /// @param revert defines if all actions should be reverted in CLEANING_UP state
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10132
    /// @trace_id_dd=DD_UCM_10205
    /// @needwork = ad
    /// @endcode
    ExceptionCleaningUpState() = default;
    /// @brief destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10133
    /// @trace_id_dd=DD_UCM_10206
    /// @needwork = ad
    /// @endcode
    ~ExceptionCleaningUpState() final = default;

    /// @brief delete copy construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10134
    /// @trace_id_dd=DD_UCM_10207
    /// @needwork = ad
    /// @endcode
    ExceptionCleaningUpState(ExceptionCleaningUpState const& other) = delete;
    /// @brief delete copy asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10135
    /// @trace_id_dd=DD_UCM_10208
    /// @needwork = ad
    /// @endcode
    ExceptionCleaningUpState& operator=(ExceptionCleaningUpState const& other) = delete;
    /// @brief delete move construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10136
    /// @trace_id_dd=DD_UCM_10209
    /// @needwork = ad
    /// @endcode
    ExceptionCleaningUpState(ExceptionCleaningUpState&& other) = delete;
    /// @brief delete move asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10137
    /// @trace_id_dd=DD_UCM_10210
    /// @needwork = ad
    /// @endcode
    ExceptionCleaningUpState& operator=(ExceptionCleaningUpState&& other) = delete;

    /// @brief Retrieve the Status which corresponds to the state.
    ///
    /// @return The corresponding status.
    /// @throws no
    /// @code{.isoft}
    /// <inheritdoc>
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10138
    /// @trace_id_dd=DD_UCM_10211
    /// @needwork = ad
    /// @endcode
    PackageManagerStatusType GetStatus() const noexcept final { return PackageManagerStatusType::kExceptionCleaningUp; }

    /// @brief Done
    /// @throws no
    /// @code{.isoft}
    /// <inheritdoc>
    /// @uptrace={SWS_UCM_00147, 3feb72562c5062cc1614f9d61e8d84dd7bbda203}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10139
    /// @trace_id_dd=DD_UCM_10212
    /// @needwork = ad
    /// @endcode
    void Done() noexcept final;
};

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // ARA_UCM_PKGMGR_FSM_EXCEPTION_CLEANINGUP_STATE_H_
