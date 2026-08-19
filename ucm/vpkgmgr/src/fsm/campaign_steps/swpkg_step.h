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
/// @file       swpkg_step.h
/// @brief      SwpStep implementation
/// @details
/// @date       2022-05-28
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
/// @unit_name=SwpStep
/// @unit_description=SwpStep implementation
/// @endcode
///
/// ================================================================

#ifndef ARA_UCM_VPKGMGR_SWPKG_STEP_H_
#define ARA_UCM_VPKGMGR_SWPKG_STEP_H_

#include <ara/core/vector.h>

#include <atomic>
#include <memory>

#include "consts.h"
#include "fsm/campaign_steps/swp_step_activation.h"
#include "fsm/campaign_steps/swp_step_process.h"
#include "fsm/campaign_steps/swp_step_transfer.h"

namespace ara {
namespace ucm {
namespace vpkgmgr {

/// @brief swp base step, contains TransferStep, ProcessStep, ActivateStep
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00305
/// @trace_id_dd=DD_UCM_Master_00632
/// @needwork = ad
/// @endcode
class SwpStep final
{
public:
    /// @brief Define alias
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_
    /// @trace_id_dd=DD_UCM_
    /// @needwork = no
    /// @endcode
    using Ptr = std::shared_ptr< SwpStep >;

    /// @brief Constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00306
    /// @trace_id_dd=DD_UCM_Master_00633
    /// @needwork = ad
    /// @endcode
    SwpStep() = default;
    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00307
    /// @trace_id_dd=DD_UCM_Master_00634
    /// @needwork = ad
    /// @endcode
    ~SwpStep() = default;

    /// @brief Copy constructor
    /// @param other Source object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00308
    /// @trace_id_dd=DD_UCM_Master_00635
    /// @needwork = ad
    /// @endcode
    SwpStep(SwpStep const& other) = delete;
    /// @brief Move constructor
    /// @param other Source object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00309
    /// @trace_id_dd=DD_UCM_Master_00636
    /// @needwork = ad
    /// @endcode
    SwpStep(SwpStep&& other) = delete;
    /// @brief Copy assignment operator
    /// @param other Source object
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00310
    /// @trace_id_dd=DD_UCM_Master_00637
    /// @needwork = ad
    /// @endcode
    SwpStep& operator=(SwpStep const& other) = delete;
    /// @brief Move assignment operator
    /// @param other Source object
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00311
    /// @trace_id_dd=DD_UCM_Master_00638
    /// @needwork = ad
    /// @endcode
    SwpStep& operator=(SwpStep&& other) = delete;

    /// @brief Load swpStep configuration information
    /// @param swpInfo
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00312
    /// @trace_id_dd=DD_UCM_Master_00639
    /// @needwork = ad
    /// @endcode
    void Load(SwpStepInfoPtr const& swpInfo);

    /// @brief Install/Upgrade
    /// @param swclNameToTid
    /// @return Returns void on success, corresponding error code on failure
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00313
    /// @trace_id_dd=DD_UCM_Master_00640
    /// @needwork = ad
    /// @endcode
    ara::core::Result< void > Execute(
        ara::core::Map< ara::core::String, std::pair< bool, pkgmgr::TransferIdType > >& swclNameToTid);

    // /// @brief Activate
    // /// @param ucmId
    // /// @return Returns void on success, corresponding error code on failure
    // /// @throws no
    // /// @code{.isoft}
    // /// @interface_level=unit
    // /// @trace_id_ad=AD_UCM_Master_00314
    // /// @trace_id_dd=DD_UCM_Master_00641
    // /// @needwork = ad
    // /// @endcode
    // ara::core::Result< void > Activate(ara::core::String const& ucmId);

    /// @brief Cancel
    /// @return Returns void on success, corresponding error code on failure
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00315
    /// @trace_id_dd=DD_UCM_Master_00642
    /// @needwork = ad
    /// @endcode
    ara::core::Result< void > Cancel();

    /// @brief Getter to access SwpStepName_
    /// @return Returns short name
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00316
    /// @trace_id_dd=DD_UCM_Master_00643
    /// @needwork = ad
    /// @endcode
    inline ara::core::String GetShortName() const { return shortName_; }

private:
    /// @brief name of this Software Package Step
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00644
    /// @needwork = dda
    /// @endcode
    ara::core::String shortName_;
    /// @brief Safety policy
    ///ara::core::String safetyPolicy_;.
    /// @brief UCM identifier
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00645
    /// @needwork = dda
    /// @endcode
    ara::core::String ucmId_;

    /// @brief Transfer step
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00646
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< TransferStep::Ptr > transferStepVector_;
    /// @brief Execution step
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00647
    /// @needwork = dda
    /// @endcode
    ProcessStep::Ptr processStepPtr_;
    // /// @brief Activation step
    // /// @code{.isoft}
    // /// @interface_level=unit
    // /// @trace_id_ad=AD_UCM_Master_00000
    // /// @trace_id_dd=DD_UCM_Master_00648
    // /// @needwork = dda
    // /// @endcode
    // ActivationStep::Ptr activationStepPtr_;

    /// @brief Logger for logging errors
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00649
    /// @needwork = dda
    /// @endcode
    ara::log::Logger& log_{ara::log::CreateLogger(std::move(ara::core::StringView("#UCM")),
                                                  std::move(ara::core::StringView("SwpStep context")),
                                                  ara::log::LogLevel::kVerbose)};
};

}  // namespace vpkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // ARA_UCM_VPKGMGR_SWPKG_STEP_H_
