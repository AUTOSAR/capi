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
/// @file       sync_executor.cpp
/// @brief      SyncExecutor class implementation
/// @details
/// @date       2024-07-20
/// @author     hanzhibo
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/UCM Master/Fsm
/// @interface_level=unit
/// @trace_id_sr=SR_UCM_00035
/// @unit_name=SyncExecutor
/// @unit_description=SyncExecutor class implementation
/// @endcode
///
/// ================================================================

#include "sync_executor.h"

#include "fsm/fsm_manager.h"
#include "package_management_service/find_ucm_subs.h"
#include "utils/ucmm_log.h"
#include "utils/version.h"

namespace ara {
namespace ucm {
namespace vpkgmgr {

/// @brief Executor
/// @return int
/// @throws no
int32_t SyncExecutor::Executor()
{
    log_.LogDebug() << "SyncExecutor::Executor(), begin.";

    FsmManager::GetInstance()->CleanUpAllUCMs();

    SwPackageInventoryOutput out{};
    AraList< ::ara::ucm::pkgmgr::SwClusterInfoType > const swInfos{FindUcmSubs::GetInstance()->GetAllSwClusterInfo()};
    for (auto const& availableSwPackage : kSoftPkg) {
        // TBD: only simple <major>.<minor>.<patch> pattern can be compared. should be improved
        bool found{false};
        if (std::find_if(swInfos.begin(), swInfos.end(),
                         [&availableSwPackage, &found](pkgmgr::SwClusterInfoType const& swInfo) {
                             Version const currentVer{swInfo.Version};
                             Version const availVer{availableSwPackage.Version};
                             if (swInfo.Name == availableSwPackage.Name) {
                                 found = true;
                                 if (availVer > currentVer) {
                                     LOG_INFO << "sync: will update spkg name=" << swInfo.Name.c_str()
                                              << "version=" << availableSwPackage.Version.c_str();
                                     return true;
                                 }
                             }
                             return false;
                         })
            != swInfos.end()) {
            out.RequiredSoftwarePackages.push_back(availableSwPackage);
        }

        if (!found) {
            log_.LogDebug() << "SyncExecutor::Executor(), will update spkg name=" << availableSwPackage.Name.c_str()
                            << "because of not found.";
            out.RequiredSoftwarePackages.push_back(availableSwPackage);
        }
    }
    pr_.set_value(out);

    std::ignore = FsmManager::GetInstance()->TriggerIdle(pkgmgr::CampaignStateType::kSyncing);

    log_.LogDebug() << "SyncExecutor::Executor(), end.";
    return 0;
}

}  // namespace vpkgmgr
}  // namespace ucm
}  // namespace ara