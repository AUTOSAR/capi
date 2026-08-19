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
/// @file       action_generator.cpp
/// @brief      action generator implementation
/// @details
/// @date       2024-08-15
/// @author     cuiyinli
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/UCM/SoftwareClusterManager
/// @interface_level=module
/// @trace_id_sr=SR_UCM_00001
/// @unit_name=ActionGenerator
/// @unit_description=Generator for creating storage actions
/// @endcode
///
/// ================================================================

#include "action_generator.h"

#include "common/const.h"
#include "common/log.h"
#include "common/path.h"
#include "common/strtype.h"
#include "delta_update_action.h"
#include "install_action.h"
#include "os_delta_update_action.h"
#include "os_update_action.h"
#include "remove_action.h"
#include "update_action.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

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
std::unique_ptr< ReversibleAction > ActionGenerator::operator()(
    std::uint8_t const actionTypeInt,
    std::uint8_t const activateOptionTypeInt,
    //ActionType const actionType, ActivateOptionType const activateOptionType,.
    AraString const& deltaPackageApplicableVersion,
    AraString const& swclName,
    AraString const& curSwclVersion,
    AraString const& swclVersion,
    std::unique_ptr< SoftwarePackage > swpkg) const
{
    /// collect the information needed to create the proper path

    std::unique_ptr< ReversibleAction > actionUPtr{nullptr};

    // Final target path (with swclName and swclVersion added)
    AraString const targetPath{GetPath().GetSwclInstallDir(swclName, swclVersion)};
    AraString const swclPath{GetPath().GetSwclInstallDir(swclName)};

    ActionType const actionType                 = static_cast< ActionType >(actionTypeInt);
    ActivateOptionType const activateOptionType = static_cast< ActivateOptionType >(activateOptionTypeInt);
    LOGD << "actionType:" << strtype::ActionTypeToStr(actionType).c_str() << " targetPath:" << targetPath.c_str()
         << " swclPath:" << swclPath.c_str()
         << " activateOptionType:" << strtype::ActivateOptionTypeToStr(activateOptionType).c_str()
         << " deltaPackageApplicableVersion:" << deltaPackageApplicableVersion.c_str()
         << " swclName:" << swclName.c_str() << " curSWCLVer:" << curSwclVersion.c_str()
         << " swclVer:" << swclVersion.c_str();

    switch (actionType) {
        case ActionType::kInstall: {
            actionUPtr = std::make_unique< InstallAction >(targetPath, swclPath, activateOptionType,
                                                           deltaPackageApplicableVersion, swclName, curSwclVersion,
                                                           swclVersion, std::move(swpkg));
            break;
        }
        case ActionType::kRemove: {
            actionUPtr = std::make_unique< RemoveAction >(targetPath, swclPath, activateOptionType,
                                                          deltaPackageApplicableVersion, swclName, curSwclVersion,
                                                          swclVersion, std::move(swpkg));
            break;
        }
        case ActionType::kUpdate: {
            if (SoftwarePackage::IsNotDeltaPackage(deltaPackageApplicableVersion)) {  // swpkg maybe nullptr
                if (SoftwarePackage::IsOSPackage(swclName)) {
                    actionUPtr = std::make_unique< OSUpdateAction >(targetPath, swclPath, activateOptionType,
                                                                    deltaPackageApplicableVersion, swclName,
                                                                    curSwclVersion, swclVersion, std::move(swpkg));
                } else {
                    actionUPtr = std::make_unique< UpdateAction >(targetPath, swclPath, activateOptionType,
                                                                  deltaPackageApplicableVersion, swclName,
                                                                  curSwclVersion, swclVersion, std::move(swpkg));
                }
                break;
            }
            if (SoftwarePackage::IsValidDeltaPackage(deltaPackageApplicableVersion, curSwclVersion)) {
                if (SoftwarePackage::IsOSPackage(swclName)) {
                    actionUPtr = std::make_unique< OSDeltaUpdateAction >(targetPath, swclPath, activateOptionType,
                                                                         deltaPackageApplicableVersion, swclName,
                                                                         curSwclVersion, swclVersion, std::move(swpkg));
                } else {
                    actionUPtr = std::make_unique< DeltaUpdateAction >(targetPath, swclPath, activateOptionType,
                                                                       deltaPackageApplicableVersion, swclName,
                                                                       curSwclVersion, swclVersion, std::move(swpkg));
                }
                break;
            }
            break;
        }
        default: {
            break;
        }
    }

    return actionUPtr;
}

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara
