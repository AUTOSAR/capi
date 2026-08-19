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
/// @file       package_manager_state.h
/// @brief      base class for PackageManager state
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
/// @unit_name=PackageManagerState
/// @unit_description=base class for PackageManager state
/// @endcode
///
/// ================================================================

#ifndef ARA_UCM_PKGMGR_FSM_PACKAGE_MANAGER_STATE_H_
#define ARA_UCM_PKGMGR_FSM_PACKAGE_MANAGER_STATE_H_

#include "common/alias.h"
#include "types/impl_type_finalactiontype.h"
#include "util/noncopy.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief FsmManager
class FsmManager;

/// @brief This class is the base class for all classes
/// that represent Package Manager States
/// such as Ready, Processing and so on
///
/// @code{.isoft}
/// @uptrace={SWS_UCM_00086, f3294056ffc96f271bb0043e5cfcdaa4abfd068c}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10035
/// @trace_id_dd=DD_UCM_10067
/// @needwork = ad
/// @endcode
class PackageManagerState : NonCopyNonMove
{
public:
    /// @brief constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10036
    /// @trace_id_dd=DD_UCM_10068
    /// @needwork = ad
    /// @endcode
    PackageManagerState() = default;
    /// @brief destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10037
    /// @trace_id_dd=DD_UCM_10069
    /// @needwork = ad
    /// @endcode
    virtual ~PackageManagerState() = default;

    /// @brief delete copy construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10038
    /// @trace_id_dd=DD_UCM_10070
    /// @needwork = ad
    /// @endcode
    PackageManagerState(PackageManagerState const& other) = delete;
    /// @brief delete copy asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10039
    /// @trace_id_dd=DD_UCM_10071
    /// @needwork = ad
    /// @endcode
    PackageManagerState& operator=(PackageManagerState const& other) = delete;
    /// @brief delete move construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10040
    /// @trace_id_dd=DD_UCM_10072
    /// @needwork = ad
    /// @endcode
    PackageManagerState(PackageManagerState&& other) = delete;
    /// @brief delete move asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10041
    /// @trace_id_dd=DD_UCM_10073
    /// @needwork = ad
    /// @endcode
    PackageManagerState& operator=(PackageManagerState&& other) = delete;

    /// @brief Retrieve the Status which corresponds to the state.
    ///
    /// @return The corresponding status.
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10042
    /// @trace_id_dd=DD_UCM_10074
    /// @needwork = ad
    /// @endcode
    virtual PackageManagerStatusType GetStatus() const = 0;

    /// @brief Activate the processed components.
    ///
    /// @return The result of the activate operation, which specifies if the
    /// operation was successful.
    ///
    /// @throws no
    /// @code{.isoft}
    /// @uptrace={SWS_UCM_00025, c861f009a45d863eac7b7b8a9dd757e98ad66788}
    /// @uptrace={SWS_UCM_00099, 70b231c56978c822310b81eacf09f6b37d8bb27b}
    /// @uptrace={SWS_UCM_00100}
    /// @uptrace={SWS_UCM_00101}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10043
    /// @trace_id_dd=DD_UCM_10075
    /// @needwork = ad
    /// @endcode
    virtual AraFutureVoid Activate();

    /// @brief Finish the processing for the current set of
    /// processed Software Packages. Cleanup all data of the processing
    /// including the sources of the Software Packages.
    ///
    /// @return The result of the finish operation, which specifies if the
    /// operation was successful.
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10044
    /// @trace_id_dd=DD_UCM_10076
    /// @needwork = ad
    /// @endcode
    virtual AraFutureVoid Finish();

    /// @brief Process transferred Software Package.
    ///
    /// @param id Transfer ID of Software Package.
    ///
    /// @return The result of processing, which specifies if the
    /// operation was successful.
    ///
    /// @throws no
    /// @code{.isoft}
    /// @uptrace={SWS_UCM_00001, a665954b7a62b2349db109a9408efe084b8a7136}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10045
    /// @trace_id_dd=DD_UCM_10077
    /// @needwork = ad
    /// @endcode
    virtual AraFutureVoid ProcessSwPackage(TransferIdType const& id);

    /// @brief Abort an ongoing processing of a Software Package.
    ///
    /// @param id Transfer ID of Software Package.
    ///
    /// @return The result of cancelling operation, which specifies if the
    /// it was successful.
    ///
    /// TODO: This is not actually working AR-102527
    /// @throws no
    /// @code{.isoft}
    /// @uptrace={SWS_UCM_00003}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10046
    /// @trace_id_dd=DD_UCM_10078
    /// @needwork = ad
    /// @endcode
    virtual AraFutureVoid Cancel(TransferIdType const& id);

    /// @brief Revert the changes done by processing (ProcessSwPackage) of one
    /// or several software packages.
    ///
    /// @return The result of the revert operation, which specifies if the
    /// operation was successful.
    ///
    /// @throws no
    /// @code{.isoft}
    /// @uptrace={SWS_UCM_00024, 172b762f2b191a7eb048ce985f423291856573bf}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10047
    /// @trace_id_dd=DD_UCM_10079
    /// @needwork = ad
    /// @endcode
    virtual AraFutureVoid RevertProcessedSwPackages();

    /// @brief Rollback the system to the state
    /// before the packages were processed.
    ///
    /// @return The result of the rollback operation, which specifies if the
    /// operation was successful.
    ///
    /// @throws no
    /// @code{.isoft}
    /// @uptrace={SWS_UCM_00005, ee3e6bb15b678695524b76c5f32d908d094f0974}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10048
    /// @trace_id_dd=DD_UCM_10080
    /// @needwork = ad
    /// @endcode
    virtual AraFutureVoid Rollback();

    /// @brief Handles successful process of the transferred software package.
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10049
    /// @trace_id_dd=DD_UCM_10081
    /// @needwork = ad
    /// @endcode
    virtual void OnProcessSwPackageSucceeded() noexcept;

    /// @brief Handles failed process of the transferred software package.
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10050
    /// @trace_id_dd=DD_UCM_10082
    /// @needwork = ad
    /// @endcode
    virtual void OnProcessSwPackageFailed() noexcept;

    /// @brief done
    /// @throws no
    /// @code{.isoft}
    /// Internal message
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10051
    /// @trace_id_dd=DD_UCM_10083
    /// @needwork = ad
    /// @endcode
    virtual void Done() noexcept;

    /// @brief OnSuccess
    /// @throws no
    /// @return result
    /// @code{.isoft}
    /// Internal message
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10052
    /// @trace_id_dd=DD_UCM_10084
    /// @needwork = ad
    /// @endcode
    virtual AraResultVoid OnSuccess() noexcept;

    /// @brief OnFailure
    /// @throws no
    /// @return result
    /// @code{.isoft}
    /// Internal message
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10053
    /// @trace_id_dd=DD_UCM_10085
    /// @needwork = ad
    /// @endcode
    virtual AraResultVoid OnFailure() noexcept;

    /// @brief GetFinalActionType
    /// @throws no
    /// @return FinalActionType
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10054
    /// @trace_id_dd=DD_UCM_10086
    /// @needwork = ad
    /// @endcode
    virtual FinalActionType GetFinalActionType() const noexcept;

    /// @brief SetOwner
    /// @param owner Fsm Manager object (used to perform action if needeed)
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10055
    /// @trace_id_dd=DD_UCM_10087
    /// @needwork = ad
    /// @endcode
    void SetOwner(FsmManager* const owner) const noexcept;

protected:
    /// @brief Logger for logging current state
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10088
    /// @needwork = dda
    /// @endcode
    mutable FsmManager* owner_{nullptr};  // NOLINT
};

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // ARA_UCM_PKGMGR_FSM_PACKAGE_MANAGER_STATE_H_
