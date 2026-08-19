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
/// @file       ucm_step.h
/// @brief      UcmStep implementation
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
/// @unit_name=UcmStep
/// @unit_description=UcmStep implementation
/// @endcode
///
/// ================================================================

#ifndef ARA_UCM_VPKGMGR_UCM_STEP_H_
#define ARA_UCM_VPKGMGR_UCM_STEP_H_

#include <ara/core/future.h>
#include <ara/core/promise.h>
#include <ara/core/vector.h>
#include <ara/log/logger.h>
#include <ara/ucm/pkgmgr/error_domain_ucmerrordomain.h>

#include <memory>
#include <mutex>
#include <utility>
#include <vector>

#include "fsm/campaign_steps/swpkg_step.h"
#include "package_management_service/package_management_app.h"

namespace ara {
namespace ucm {
namespace vpkgmgr {

/// @brief UcmStep step
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00271
/// @trace_id_dd=DD_UCM_Master_00587
/// @needwork = ad
/// @endcode
class UcmStep final
{
public:
    /// @brief Define alias
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_
    /// @trace_id_dd=DD_UCM_
    /// @needwork = no
    /// @endcode
    using Ptr = std::shared_ptr< UcmStep >;

    /// @brief Constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00272
    /// @trace_id_dd=DD_UCM_Master_00588
    /// @needwork = ad
    /// @endcode
    UcmStep() = default;
    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00273
    /// @trace_id_dd=DD_UCM_Master_00589
    /// @needwork = ad
    /// @endcode
    ~UcmStep() = default;

    /// @brief Copy constructor
    /// @param other Source object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00274
    /// @trace_id_dd=DD_UCM_Master_00590
    /// @needwork = ad
    /// @endcode
    UcmStep(UcmStep const& other) = delete;
    /// @brief Move constructor
    /// @param other Source object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00275
    /// @trace_id_dd=DD_UCM_Master_00591
    /// @needwork = ad
    /// @endcode
    UcmStep(UcmStep&& other) = delete;
    /// @brief Copy assignment operator
    /// @param other Source object
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00276
    /// @trace_id_dd=DD_UCM_Master_00592
    /// @needwork = ad
    /// @endcode
    UcmStep& operator=(UcmStep const& other) = delete;
    /// @brief Move assignment operator
    /// @param other Source object
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00277
    /// @trace_id_dd=DD_UCM_Master_00593
    /// @needwork = ad
    /// @endcode
    UcmStep& operator=(UcmStep&& other) = delete;

    /// @brief Load configuration
    /// @param ucmInfo Configuration information for the ucm step
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00278
    /// @trace_id_dd=DD_UCM_Master_00594
    /// @needwork = ad
    /// @endcode
    void Load(UcmStepInfoPtr const& ucmInfo);

    /// @brief Get short name
    /// @return Returns short name
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00279
    /// @trace_id_dd=DD_UCM_Master_00595
    /// @needwork = ad
    /// @endcode
    inline ara::core::String GetShortName() const { return shortName_; }

    /// Function that handles this UCM in a separate thread

    /// @brief Execute UcmStep in a separate thread
    /// @param promise
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00280
    /// @trace_id_dd=DD_UCM_Master_00596
    /// @needwork = ad
    /// @endcode
    void UcmThread(ara::core::Promise< void >&& promise);

    /// @brief Execute activation
    /// @param promise
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00281
    /// @trace_id_dd=DD_UCM_Master_00597
    /// @needwork = ad
    /// @endcode
    void Activate(ara::core::Promise< void >&& promise);

private:
    /// @brief Short name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00598
    /// @needwork = dda
    /// @endcode
    ara::core::String shortName_;
    /// @brief UCM identifier
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00599
    /// @needwork = dda
    /// @endcode
    ara::core::String ucmId_;
    /// @brief Safety policy
    ///ara::core::String safetyPolicy_;.

    /// @brief All swpSteps contained in ucmStep
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00600
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< SwpStep::Ptr > swpSteps_;

    /// @brief Activation step
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00648
    /// @needwork = dda
    /// @endcode
    ActivationStep::Ptr activationStepPtr_;

    /// @brief Logger for logging errors
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00601
    /// @needwork = dda
    /// @endcode
    ara::log::Logger& log_{ara::log::CreateLogger(std::move(ara::core::StringView("#UCM")),
                                                  std::move(ara::core::StringView("UcmStep context")),
                                                  ara::log::LogLevel::kVerbose)};
};

}  // namespace vpkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // ARA_UCM_VPKGMGR_UCM_STEP_H_
