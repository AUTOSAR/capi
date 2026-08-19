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
/// @file       swp_step_activation.h
/// @brief      ActivationStep impl
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
/// @unit_name=ActivationStep
/// @unit_description=ActivationStep impl
/// @endcode
///
/// ================================================================

#ifndef ARA_UCM_VPKGMGR_ACTIVATION_STEP_H_
#define ARA_UCM_VPKGMGR_ACTIVATION_STEP_H_

#include <ara/core/error_code.h>
#include <ara/core/map.h>
#include <ara/core/result.h>
#include <ara/core/string.h>
#include <ara/log/logger.h>
#include <ara/ucm/pkgmgr/impl_type_swnametype.h>
#include <ara/ucm/pkgmgr/impl_type_transferidtype.h>

#include <memory>

namespace ara {
namespace ucm {
namespace vpkgmgr {

/// @brief ActivationStep step
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00317
/// @trace_id_dd=DD_UCM_Master_00650
/// @needwork = ad
/// @endcode
class ActivationStep final
{
public:
    /// @brief Define alias
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_
    /// @trace_id_dd=DD_UCM_
    /// @needwork = no
    /// @endcode
    using Ptr = std::shared_ptr< ActivationStep >;

    /// @brief Constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00318
    /// @trace_id_dd=DD_UCM_Master_00651
    /// @needwork = ad
    /// @endcode
    ActivationStep() = default;
    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00319
    /// @trace_id_dd=DD_UCM_Master_00652
    /// @needwork = ad
    /// @endcode
    ~ActivationStep() = default;

    /// @brief Copy constructor
    /// @param other Source object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00320
    /// @trace_id_dd=DD_UCM_Master_00653
    /// @needwork = ad
    /// @endcode
    ActivationStep(ActivationStep const& other) = delete;
    /// @brief Move constructor
    /// @param other Source object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00321
    /// @trace_id_dd=DD_UCM_Master_00654
    /// @needwork = ad
    /// @endcode
    ActivationStep(ActivationStep&& other) = delete;
    /// @brief Copy assignment operator
    /// @param other Source object
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00322
    /// @trace_id_dd=DD_UCM_Master_00655
    /// @needwork = ad
    /// @endcode
    ActivationStep& operator=(ActivationStep const& other) = delete;
    /// @brief Move assignment operator
    /// @param other Source object
    /// @return *this
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00323
    /// @trace_id_dd=DD_UCM_Master_00656
    /// @needwork = ad
    /// @endcode
    ActivationStep& operator=(ActivationStep&& other) = delete;

    /// @brief Execute the Activation step, i.e. trigger UCM Subordinate to activate the SwPackage identified by the
    /// current Transfer ID
    /// @param ucmId The ID of the UCM subordinate that shall compute the action
    /// @return result
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00324
    /// @trace_id_dd=DD_UCM_Master_00657
    /// @needwork = ad
    /// @endcode
    ara::core::Result< void > Execute(ara::core::String const& ucmId) const;

private:
    /// @brief Logger for logging errors
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00658
    /// @needwork = dda
    /// @endcode
    ara::log::Logger& log_{ara::log::CreateLogger(std::move(ara::core::StringView("#UCM")),
                                                  std::move(ara::core::StringView("SwpStep context")),
                                                  ara::log::LogLevel::kVerbose)};
};

}  // namespace vpkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // ARA_UCM_VPKGMGR_ACTIVATION_STEP_H_
