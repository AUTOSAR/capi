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
/// @file       package_manager_state.cpp
/// @brief      PackageManagerState implementation
/// @details
/// @date       2024-08-15
/// @author     cuiyinli
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/UCM/FsmManager
/// @interface_level=unit
/// @trace_id_sr=SR_UCM_00001
/// @unit_name=PackageManagerState
/// @unit_description=PackageManagerState implementation
/// @endcode
///
/// ================================================================

#include "package_manager_state.h"

#include "ara/ucm/pkgmgr/error_domain_ucmerrordomain.h"
#include "util/future_ext.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief NotPermitted
/// @return result
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10043, AD_UCM_10044, AD_UCM_10045, AD_UCM_10046, AD_UCM_10047, AD_UCM_10048, AD_UCM_10049, AD_UCM_10050
/// @trace_id_dd=DD_UCM_10075, DD_UCM_10076, DD_UCM_10077, DD_UCM_10078, DD_UCM_10079, DD_UCM_10080, DD_UCM_10081, DD_UCM_10082
/// @needwork = ad
/// @endcode
static AraFutureVoid NotPermitted()
{
    return SyncCallVoid([]() { return AraResultVoid(UCMErrorDomainErrc::kOperationNotPermitted); });
}

/// @brief Activate the processed components.
///
/// @return The result of the activate operation, which specifies if the
/// operation was successful.
///
/// @uptrace={SWS_UCM_00025, c861f009a45d863eac7b7b8a9dd757e98ad66788}
/// @uptrace={SWS_UCM_00099, 70b231c56978c822310b81eacf09f6b37d8bb27b}
/// @uptrace={SWS_UCM_00100}
/// @uptrace={SWS_UCM_00101}
/// @throws no
AraFutureVoid PackageManagerState::Activate() { return NotPermitted(); }

/// @brief Finish the processing for the current set of
/// processed Software Packages. Cleanup all data of the processing
/// including the sources of the Software Packages.
///
/// @return The result of the finish operation, which specifies if the
/// operation was successful.
/// @throws no
AraFutureVoid PackageManagerState::Finish() { return NotPermitted(); }

/// @brief Process transferred Software Package.
///
/// @param id Transfer ID of Software Package.
///
/// @return The result of processing, which specifies if the
/// operation was successful.
///
/// @uptrace={SWS_UCM_00001, a665954b7a62b2349db109a9408efe084b8a7136}
/// @throws no
AraFutureVoid PackageManagerState::ProcessSwPackage(TransferIdType const& id)
{
    std::ignore = id;
    return NotPermitted();
}

/// @brief Abort an ongoing processing of a Software Package.
///
/// @param id Transfer ID of Software Package.
///
/// @return The result of cancelling operation, which specifies if the
/// it was successful.
///
/// TODO: This is not actually working AR-102527
/// @uptrace={SWS_UCM_00003}
/// @throws no
AraFutureVoid PackageManagerState::Cancel(TransferIdType const& id)
{
    std::ignore = id;
    return NotPermitted();
}

/// @brief Revert the changes done by processing (ProcessSwPackage) of one
/// or several software packages.
///
/// @return The result of the revert operation, which specifies if the
/// operation was successful.
///
/// @uptrace={SWS_UCM_00024, 172b762f2b191a7eb048ce985f423291856573bf}
/// @throws no
AraFutureVoid PackageManagerState::RevertProcessedSwPackages() { return NotPermitted(); }

/// @brief Rollback the system to the state
/// before the packages were processed.
///
/// @return The result of the rollback operation, which specifies if the
/// operation was successful.
///
/// @uptrace={SWS_UCM_00005, ee3e6bb15b678695524b76c5f32d908d094f0974}
/// @throws no
AraFutureVoid PackageManagerState::Rollback() { return NotPermitted(); }

/// @brief done
/// @throws no
void PackageManagerState::Done() noexcept {}

/// Internal message
/// @brief OnSuccess
/// @throws no
/// @return result
AraResultVoid PackageManagerState::OnSuccess() noexcept { return {}; }

/// @brief OnFailure
/// @throws no
/// @return result
AraResultVoid PackageManagerState::OnFailure() noexcept { return {}; }

/// @brief Handles successful process of the transferred software package.
/// @throws no
void PackageManagerState::OnProcessSwPackageSucceeded() noexcept {}

/// @brief Handles failed process of the transferred software package.
/// @throws no
void PackageManagerState::OnProcessSwPackageFailed() noexcept {}

/// @brief GetFinalActionType
/// @throws no
/// @return FinalActionType
FinalActionType PackageManagerState::GetFinalActionType() const noexcept { return FinalActionType::kRevert; }

/// @brief SetOwner
/// @param owner Fsm Manager object (used to perform action if needeed)
/// @throws no
void PackageManagerState::SetOwner(FsmManager* const owner) const noexcept { owner_ = owner; }

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara
