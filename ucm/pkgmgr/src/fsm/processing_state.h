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
/// @file       processing_state.h
/// @brief      Processing state class
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
/// @trace_id_sr=SR_UCM_00001
/// @unit_name=ProcessingState
/// @unit_description=Processing state class
/// @endcode
///
/// ================================================================

#ifndef ARA_UCM_PKGMGR_FSM_BUSY_STATE_H_
#define ARA_UCM_PKGMGR_FSM_BUSY_STATE_H_

#include "package_manager_state.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief Class representing Idle (default) state of Package Manager
/// It is designed according to the State Pattern
///
/// @code{.isoft}
/// @uptrace={SWS_UCM_00007, 1f06ff0dbb62befd08052e7695c09b0f3a05cbbf}
/// @uptrace={SWS_UCM_00149, 61b54aa452c270fcd403583612b1f7e35f2eb6a9}
/// @uptrace={SWS_UCM_00151, d2171d66c6e68ebb5f493b817dec0c4d1b7fc4c4}
/// @uptrace={SWS_UCM_00017, c996b4c85a5e28498d0e050575af97caba0913f4}
/// @uptrace={SWS_UCM_00083, 1efd62a7a067e892f5b4b946e33f5320e0a9972d}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10079
/// @trace_id_dd=DD_UCM_10134
/// @needwork = ad
/// @endcode
class ProcessingState final : public PackageManagerState
{
public:
    /// @brief Creates a new instance of Processing state with given id
    /// and interrupt token and saves the currentstate.
    ///
    /// @param token token for cancel implementation
    /// @param id currently processed package id
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10080
    /// @trace_id_dd=DD_UCM_10135
    /// @needwork = ad
    /// @endcode
    ProcessingState() = default;
    /// @brief destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10081
    /// @trace_id_dd=DD_UCM_10136
    /// @needwork = ad
    /// @endcode
    ~ProcessingState() final = default;

    /// @brief delete copy construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10082
    /// @trace_id_dd=DD_UCM_10137
    /// @needwork = ad
    /// @endcode
    ProcessingState(ProcessingState const& other) = delete;
    /// @brief delete copy asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10083
    /// @trace_id_dd=DD_UCM_10138
    /// @needwork = ad
    /// @endcode
    ProcessingState& operator=(ProcessingState const& other) = delete;
    /// @brief delete move construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10084
    /// @trace_id_dd=DD_UCM_10139
    /// @needwork = ad
    /// @endcode
    ProcessingState(ProcessingState&& other) = delete;
    /// @brief delete move asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10085
    /// @trace_id_dd=DD_UCM_10140
    /// @needwork = ad
    /// @endcode
    ProcessingState& operator=(ProcessingState&& other) = delete;

    /// @brief Retrieve the Status which corresponds to the state.
    ///
    /// @return The corresponding status.
    /// @throws no
    /// @code{.isoft}
    /// <inheritdoc>
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10086
    /// @trace_id_dd=DD_UCM_10141
    /// @needwork = ad
    /// @endcode
    PackageManagerStatusType GetStatus() const noexcept final { return PackageManagerStatusType::kProcessing; }

    /// @brief ProcessSwPackage
    /// @param id TransferIdType
    /// @return future with result
    /// @throws no
    /// @code{.isoft}
    /// <inheritdoc>
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10087
    /// @trace_id_dd=DD_UCM_10142
    /// @needwork = ad
    /// @endcode
    AraFutureVoid ProcessSwPackage(TransferIdType const& id) noexcept final;

    /// @brief Cancel
    /// @param id TransferIdType
    /// @return future with result
    /// @throws no
    /// @code{.isoft}
    /// <inheritdoc>
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10088
    /// @trace_id_dd=DD_UCM_10143
    /// @needwork = ad
    /// @endcode
    AraFutureVoid Cancel(TransferIdType const& id) noexcept final;

    /// @brief RevertProcessedSwPackages
    /// @return future with result
    /// @throws no
    /// @code{.isoft}
    /// <inheritdoc>
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10089
    /// @trace_id_dd=DD_UCM_10144
    /// @needwork = ad
    /// @endcode
    AraFutureVoid RevertProcessedSwPackages() noexcept final;

    /// @brief OnProcessSwPackageSucceeded
    /// @throws no
    /// @code{.isoft}
    /// <inheritdoc>
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10090
    /// @trace_id_dd=DD_UCM_10145
    /// @needwork = ad
    /// @endcode
    void OnProcessSwPackageSucceeded() noexcept final;

    /// @brief OnProcessSwPackageFailed
    /// @throws no
    /// @code{.isoft}
    /// <inheritdoc>
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10091
    /// @trace_id_dd=DD_UCM_10146
    /// @needwork = ad
    /// @endcode
    void OnProcessSwPackageFailed() noexcept final;
};

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // ARA_UCM_PKGMGR_FSM_BUSY_STATE_H_
