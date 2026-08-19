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
/// @file       swp_step_process.h
/// @brief      ProcessStep impl
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
/// @unit_name=ProcessStep
/// @unit_description=ProcessStep impl
/// @endcode
///
/// ================================================================

#ifndef ARA_UCM_VPKGMGR_PROCESS_STEP_H_
#define ARA_UCM_VPKGMGR_PROCESS_STEP_H_

#include <ara/core/error_code.h>
#include <ara/core/map.h>
#include <ara/core/promise.h>
#include <ara/core/result.h>
#include <ara/log/log_stream.h>
#include <ara/log/logger.h>
#include <ara/ucm/pkgmgr/impl_type_swnametype.h>
#include <ara/ucm/pkgmgr/impl_type_transferidtype.h>

#include <memory>

#include "fsm/parsing/step_info.h"
#include "package_management_service/package_management_app.h"

namespace ara {
namespace ucm {
namespace vpkgmgr {

/// @brief ProcessStep step
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00282
/// @trace_id_dd=DD_UCM_Master_00602
/// @needwork = ad
/// @endcode
class ProcessStep final
{
public:
    /// @brief Define alias
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_
    /// @trace_id_dd=DD_UCM_
    /// @needwork = no
    /// @endcode
    using Ptr = std::shared_ptr< ProcessStep >;

    /// @brief Constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00283
    /// @trace_id_dd=DD_UCM_Master_00603
    /// @needwork = ad
    /// @endcode
    ProcessStep() = default;
    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00284
    /// @trace_id_dd=DD_UCM_Master_00604
    /// @needwork = ad
    /// @endcode
    ~ProcessStep() = default;

    /// @brief Copy constructor
    /// @param other Source object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00285
    /// @trace_id_dd=DD_UCM_Master_00605
    /// @needwork = ad
    /// @endcode
    ProcessStep(ProcessStep const& other) = delete;
    /// @brief Move constructor
    /// @param other Source object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00286
    /// @trace_id_dd=DD_UCM_Master_00606
    /// @needwork = ad
    /// @endcode
    ProcessStep(ProcessStep&& other) = delete;
    /// @brief Copy assignment operator
    /// @param other Source object
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00287
    /// @trace_id_dd=DD_UCM_Master_00607
    /// @needwork = ad
    /// @endcode
    ProcessStep& operator=(ProcessStep const& other) = delete;
    /// @brief Move assignment operator
    /// @param other Source object
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00288
    /// @trace_id_dd=DD_UCM_Master_00608
    /// @needwork = ad
    /// @endcode
    ProcessStep& operator=(ProcessStep&& other) = delete;

    /// @brief GetSpkgRef
    /// @param processInfo
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00289
    /// @trace_id_dd=DD_UCM_Master_00609
    /// @needwork = ad
    /// @endcode
    void Load(ProcessStepInfoPtr const& processInfo);

    /// @brief Execute ProcessThread in a separate thread
    /// @param ucmId
    /// @param swclNameToTid
    /// @param promise
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00290
    /// @trace_id_dd=DD_UCM_Master_00610
    /// @needwork = ad
    /// @endcode
    void ProcessThread(
        ara::core::String const& ucmId,
        ara::core::Map< ara::core::String, std::pair< bool, pkgmgr::TransferIdType > >* const swclNameToTid,
        ara::core::Promise< void >&& promise) const;

    /// @brief GetSpkgRef
    /// @return string of spkgRef_
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00291
    /// @trace_id_dd=DD_UCM_Master_00611
    /// @needwork = ad
    /// @endcode
    ara::core::String GetSpkgRef() const { return spkgRef_; }

private:
    /// @brief Execute the Process step, i.e. trigger UCM Subordinate to process the SwPackage identified by the current
    /// Transfer ID
    ///
    /// @param ucmId The ID of the UCM subordinate that shall compute the action
    /// @param swclNameToTid Map that link a software cluster name to a Transfer ID
    /// Map<ara::core::String, TransferIdType>& swclNameToTid
    /// @return result
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00612
    /// @needwork = dda
    /// @endcode
    ara::core::Result< void > _execute(
        ara::core::String const& ucmId,
        ara::core::Map< ara::core::String, std::pair< bool, pkgmgr::TransferIdType > >& swclNameToTid) const;

    // /// @brief  Clean up software package
    // /// @param pmAppPtr
    // /// @param transferId
    // /// @throws no
    // void cleanSWP(PackageManagementAppPtr pmAppPtr, pkgmgr::TransferIdType const& transferId) const;.

private:
    /// @brief spkgRef_ reference to the Software Package to be processed
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00613
    /// @needwork = dda
    /// @endcode
    ara::core::String spkgRef_;

    /// @brief Logger
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00614
    /// @needwork = dda
    /// @endcode
    ara::log::Logger& log_{ara::log::CreateLogger(std::move(ara::core::StringView("#UCM")),
                                                  std::move(ara::core::StringView("ProcessStep context")),
                                                  ara::log::LogLevel::kVerbose)};
};

}  // namespace vpkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // ARA_UCM_VPKGMGR_PROCESS_STEP_H_
