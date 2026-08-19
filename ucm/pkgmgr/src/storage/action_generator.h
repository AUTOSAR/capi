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
/// @file       action_generator.h
/// @brief      action generator
/// @details
/// @date       2024-08-15
/// @author     cuiyinli
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @export_level=/UCM/SoftwareClusterManager
/// @module_path=/UCM/SoftwareClusterManager
/// @interface_level=module
/// @trace_id_sr=SR_UCM_00001
/// @unit_name=ActionGenerator
/// @unit_description=Generator for creating storage actions
/// @endcode
///
/// ================================================================

#ifndef ARA_UCM_PKGMGR_STORAGE_ACTION_GENERATOR_H_
#define ARA_UCM_PKGMGR_STORAGE_ACTION_GENERATOR_H_

#include <memory>

#include "ara/ucm/pkgmgr/impl_type_actiontype.h"
#include "parsing/software_package_manifest.h"
#include "reversible_action.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief Generator for creating storage actions.
///
/// Storage actions for regular SWCL use different directories
/// in a filesystem to organize the extraction of the existing
/// platform before the changes are applied to the system.
/// Storage actions for system updates will be implemented by
/// special actions.
///
/// @code{.isoft}
/// @uptrace={SWS_UCM_00001, a665954b7a62b2349db109a9408efe084b8a7136}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10313
/// @trace_id_dd=DD_UCM_10646
/// @needwork = ad
/// @endcode
class ActionGenerator
{
public:
    /// @brief Create a processing action for a software package
    /// @param actionType
    /// @param activateOptionType
    /// @param deltaPackageApplicableVersion
    /// @param swclName
    /// @param curSwclVersion
    /// @param swclVersion
    /// @param swpkg
    ///
    /// Builds the appropriate reversible action based on software package data
    ///
    /// @return An action implementing functions Execute and Cleanup
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10647
    /// @needwork = dda
    /// @endcode
    std::unique_ptr< ReversibleAction > operator()(
        std::uint8_t const actionTypeInt,
        std::uint8_t const activateOptionTypeInt,
        // ActionType const actionType, ActivateOptionType const activateOptionType,.
        AraString const& deltaPackageApplicableVersion,
        AraString const& swclName,
        AraString const& curSwclVersion,
        AraString const& swclVersion,
        std::unique_ptr< SoftwarePackage > swpkg) const;
};

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // ARA_UCM_PKGMGR_STORAGE_ACTION_GENERATOR_H_
