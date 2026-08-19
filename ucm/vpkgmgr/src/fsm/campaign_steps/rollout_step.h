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
/// @file       rollout_step.h
/// @brief      RolloutStep impl
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
/// @unit_name=RolloutStep
/// @unit_description=RolloutStep impl
/// @endcode
///
/// ================================================================

#ifndef ARA_UCM_VPKGMGR_ROLLOUT_STEP_H_
#define ARA_UCM_VPKGMGR_ROLLOUT_STEP_H_

#include <ara/core/error_code.h>
#include <ara/core/map.h>
#include <ara/core/result.h>
#include <ara/core/string.h>
#include <ara/core/vector.h>

#include <memory>
#include <utility>

#include "fsm/campaign_steps/ucm_step.h"

namespace ara {
namespace ucm {
namespace vpkgmgr {

/// @brief RollStep installation/upgrade step class
///
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00292
/// @trace_id_dd=DD_UCM_Master_00615
/// @needwork = ad
/// @endcode
class RolloutStep final
{
public:
    /// @brief Define alias
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_
    /// @trace_id_dd=DD_UCM_
    /// @needwork = no
    /// @endcode
    using Ptr = std::shared_ptr< RolloutStep >;

    /// @brief Constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00293
    /// @trace_id_dd=DD_UCM_Master_00616
    /// @needwork = ad
    /// @endcode
    RolloutStep() = default;
    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00294
    /// @trace_id_dd=DD_UCM_Master_00617
    /// @needwork = ad
    /// @endcode
    ~RolloutStep() = default;

    /// @brief Copy constructor
    /// @param other Source object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00295
    /// @trace_id_dd=DD_UCM_Master_00618
    /// @needwork = ad
    /// @endcode
    RolloutStep(RolloutStep const& other) = delete;
    /// @brief Move constructor
    /// @param other Source object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00296
    /// @trace_id_dd=DD_UCM_Master_00619
    /// @needwork = ad
    /// @endcode
    RolloutStep(RolloutStep&& other) = delete;
    /// @brief Copy assignment operator
    /// @param other Source object
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00297
    /// @trace_id_dd=DD_UCM_Master_00620
    /// @needwork = ad
    /// @endcode
    RolloutStep& operator=(RolloutStep const& other) = delete;
    /// @brief Move assignment operator
    /// @param other Source object
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00298
    /// @trace_id_dd=DD_UCM_Master_00621
    /// @needwork = ad
    /// @endcode
    RolloutStep& operator=(RolloutStep&& other) = delete;

    /// @brief Load vehicle package upgrade/update step information and generate corresponding RollStep, UcmStep, SwpStep operation step instances
    /// @param rolloutInfo Vehicle package upgrade/update step information
    /// @return 0 on success, non-zero on failure
    /// @throws no
    ///
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00299
    /// @trace_id_dd=DD_UCM_Master_00622
    /// @needwork = ad
    /// @endcode
    int32_t Load(RolloutStepInfoPtr const& rolloutInfo);

    /// @brief Get the safety policy set in the step
    /// @return Returns the safety policy
    /// @throws no
    ///
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00300
    /// @trace_id_dd=DD_UCM_Master_00623
    /// @needwork = ad
    /// @endcode
    ara::core::String GetSafetyPolicy() const { return safetyPolicy_; }

    /// @brief Execute, including transferring software package to UCM and installing the update
    /// @return Returns void or a specific error code
    /// @throws no
    ///
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00301
    /// @trace_id_dd=DD_UCM_Master_00624
    /// @needwork = ad
    /// @endcode
    ara::core::Result< void > Execute();

    /// @brief Perform activation step
    /// @return Returns void or a specific error code
    /// @throws no
    ///
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00302
    /// @trace_id_dd=DD_UCM_Master_00625
    /// @needwork = ad
    /// @endcode
    ara::core::Result< void > Activate();

    /// @brief Cancel the currently ongoing active step
    /// @return Returns void or a specific error code
    /// @throws no
    ///
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00303
    /// @trace_id_dd=DD_UCM_Master_00626
    /// @needwork = ad
    /// @endcode
    ara::core::Result< void > Cancel();

    /// @brief Get the short name of RollStep
    /// @return Returns the short name of RollStep
    /// @throws no
    ///
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00304
    /// @trace_id_dd=DD_UCM_Master_00627
    /// @needwork = ad
    /// @endcode
    ara::core::String GetShortName() const { return shortName_; }

private:
    /// @brief Logger for logging errors
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00628
    /// @needwork = dda
    /// @endcode
    ara::log::Logger& log_{ara::log::CreateLogger(std::move(ara::core::StringView("#UCM")),
                                                  std::move(ara::core::StringView("RolloutStep context")),
                                                  ara::log::LogLevel::kVerbose)};

    /// @brief Short name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00629
    /// @needwork = dda
    /// @endcode
    ara::core::String shortName_;
    /// @brief Safety policy
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00630
    /// @needwork = dda
    /// @endcode
    ara::core::String safetyPolicy_;

    /// @brief Parallel execution of ucm steps
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00631
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< UcmStep::Ptr > ucmSteps_;
};

}  // namespace vpkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // ARA_UCM_VPKGMGR_ROLLOUT_STEP_H_
