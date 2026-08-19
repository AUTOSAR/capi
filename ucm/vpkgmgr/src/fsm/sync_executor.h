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
/// @file       sync_executor.h
/// @brief      SyncExecutor class definition
/// @details
/// @date       2024-07-20
/// @author     hanzhibo
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// export_level=/
/// @module_path=/UCM Master/Fsm
/// @interface_level=unit
/// @trace_id_sr=SR_UCM_00035
/// @unit_name=SyncExecutor
/// @unit_description=SyncExecutor class definition
/// @endcode
///
/// ================================================================

#ifndef _EXECUTOR_SYNC_EXECUTOR_H__
#define _EXECUTOR_SYNC_EXECUTOR_H__

#include <ara/core/promise.h>
#include <ara/log/log_stream.h>
#include <ara/log/logger.h>
#include <ara/ucm/pkgmgr/vehiclepackagemanagement_common.h>

#include "utils/alias.h"

namespace ara {
namespace ucm {
namespace vpkgmgr {

/// @brief SwPackageInventoryOutput
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
using SwPackageInventoryOutput = pkgmgr::VehiclePackageManagement::SwPackageInventoryOutput;

/// @brief SyncExecutor
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00008
/// @trace_id_dd=DD_UCM_Master_00042
/// @needwork = ad
/// @endcode
class SyncExecutor
{
public:
    /// @brief constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00043
    /// @needwork = dda
    /// @endcode
    SyncExecutor() = delete;
    /// @brief constructor
    /// @param softPkg
    /// @param promise
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00044
    /// @needwork = dda
    /// @endcode
    SyncExecutor(pkgmgr::SwNameVersionVectorType const& softPkg,
                 ara::core::Promise< SwPackageInventoryOutput >&& promise)
        : kSoftPkg{softPkg.begin(),
                   softPkg.begin() + static_cast< pkgmgr::SwNameVersionVectorType::difference_type >(softPkg.size())}
        , pr_{std::move(promise)}
        , log_{ara::log::CreateLogger(std::move(ara::core::StringView("#UCM")),
                                      std::move(ara::core::StringView("SyncExecutor context")),
                                      ara::log::LogLevel::kVerbose)}
    {
    }
    /// @brief destructor
    ///~SyncExecutor() = default;

    /// @brief Executor
    /// @return int
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00045
    /// @needwork = dda
    /// @endcode
    int32_t Executor();

private:
    /// @brief kSoftPkg
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00046
    /// @needwork = dda
    /// @endcode
    AraList< ::ara::ucm::pkgmgr::SwNameVersionType > const kSoftPkg;

    /// @brief pr_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00047
    /// @needwork = dda
    /// @endcode
    ara::core::Promise< SwPackageInventoryOutput > pr_;

    /// @brief Logger for logging errors
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00048
    /// @needwork = dda
    /// @endcode
    ara::log::Logger& log_;
};

}  // namespace vpkgmgr
}  // namespace ucm
}  // namespace ara
#endif