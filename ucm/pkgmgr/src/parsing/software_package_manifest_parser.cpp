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
/// @file       software_package_manifest_parser.cpp
/// @brief      software package manifest parser implementation
/// @details
/// @date       2024-08-15
/// @author     cuiyinli
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/UCM/SoftwarePackageManager
/// @interface_level=unit
/// @trace_id_sr=SR_UCM_00001
/// @unit_name=SoftwarePackageManifestParser
/// @unit_description=software package manifest parser implementation
/// @endcode
///
/// ================================================================

#include "software_package_manifest_parser.h"

#include "common/log.h"
#include "files_node.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief Parses a manifest
/// @param manifest The manifest json
/// @returns The parsed manifest object representing the json content
/// @throws no
std::unique_ptr< SoftwarePackageManifest > SoftwarePackageManifestParser::Parse(RManifestUPtr const& manifest)
{
    std::unique_ptr< SoftwarePackageManifest > swpkgMnfUPtr{std::make_unique< SoftwarePackageManifest >()};
    std::unique_ptr< SoftwarePackageManifest > swpkgMnfNull{nullptr};

    // use Check to return nullptr when load failed with invalid key
    RManifestLoadAndCheck(manifest, "deltaPackageApplicableVersion", swpkgMnfUPtr->deltaPackageApplicableVersion,
                          swpkgMnfNull);

    AraResult< ActionType > const actionTypeRet{ParseActionType(manifest)};
    if (!actionTypeRet.HasValue()) {
        RManifestLoadResultCheck("actionType", static_cast< std::int32_t >(actionTypeRet.Error().Value()),
                                 swpkgMnfNull);
    }
    swpkgMnfUPtr->actionType = actionTypeRet.Value();

    AraResult< ActivateOptionType > const activationActionRet{ParseactivationAction(manifest)};
    if (!activationActionRet.HasValue()) {
        RManifestLoadResultCheck("activationAction", static_cast< std::int32_t >(activationActionRet.Error().Value()),
                                 swpkgMnfNull);
    }
    swpkgMnfUPtr->activationAction = activationActionRet.Value();

    RManifestLoad(manifest, "category", swpkgMnfUPtr->category);
    RManifestLoad(manifest, "compressedSoftwarePackageSize", swpkgMnfUPtr->compressedSoftwarePackageSize);
    RManifestLoad(manifest, "isDeltaPackage", swpkgMnfUPtr->isDeltaPackage);

    AraString minUCMSupportedVersionStr;
    RManifestLoad(manifest, "minUCMSupportedVersion", minUCMSupportedVersionStr);
    swpkgMnfUPtr->minUCMSupportedVersion = Version(minUCMSupportedVersionStr);
    AraString maxUCMSupportedVersionStr;
    RManifestLoad(manifest, "maxUCMSupportedVersion", maxUCMSupportedVersionStr);
    swpkgMnfUPtr->maxUCMSupportedVersion = Version(maxUCMSupportedVersionStr);

    RManifestLoad(manifest, "packagerID", swpkgMnfUPtr->packagerID);
    RManifestLoad(manifest, "packagerSignature", swpkgMnfUPtr->packagerSignature);
    RManifestLoad(manifest, "certSerialNumber", swpkgMnfUPtr->certSerialNumber);
    RManifestLoad(manifest, "certIssuer", swpkgMnfUPtr->certIssuer);
    RManifestLoad(manifest, "shortName", swpkgMnfUPtr->shortName);
    RManifestLoad(manifest, "uncompressedSoftwarePackageSize", swpkgMnfUPtr->uncompressedSoftwarePackageSize);
    RManifestLoad(manifest, "uuid", swpkgMnfUPtr->uUID);

    FilesNode fnode;
    RManifestLoadAndCheck(manifest, "files", fnode, swpkgMnfNull);
    swpkgMnfUPtr->files = fnode.GetFiles();

    return swpkgMnfUPtr;
}

/// @brief Helper method to parse the actionType attribute
/// @param manifest The manifest json
/// @returns ActionType
/// @throws no
AraResult< ActionType > SoftwarePackageManifestParser::ParseActionType(RManifestUPtr const& manifest)
{
    AraString actionTypeStr;
    RManifestLoad(manifest, "actionType", actionTypeStr);

    if (actionTypeStr == "update") {
        return AraResult< ActionType >::FromValue(ActionType::kUpdate);
    }
    if (actionTypeStr == "install") {
        return AraResult< ActionType >::FromValue(ActionType::kInstall);
    }
    if (actionTypeStr == "remove") {
        return AraResult< ActionType >::FromValue(ActionType::kRemove);
    }

    return AraResult< ActionType >::FromError(RManifestReaderErrc::kInvalidTypeRequested);
}

/// @brief Helper method to parse the activationAction attribute
/// @param manifest The manifest json
/// @returns ActivateOptionType
/// @throws no
AraResult< ActivateOptionType > SoftwarePackageManifestParser::ParseactivationAction(RManifestUPtr const& manifest)
{
    AraString activationActionStr;
    RManifestLoad(manifest, "activationAction", activationActionStr);

    if (activationActionStr == "restartApplication") {
        return AraResult< ActivateOptionType >::FromValue(ActivateOptionType::kRestartApplication);
    }
    if (activationActionStr == "reboot") {
        return AraResult< ActivateOptionType >::FromValue(ActivateOptionType::kReboot);
    }
    if (activationActionStr == "waitForReboot") {
        return AraResult< ActivateOptionType >::FromValue(ActivateOptionType::kWaitForReboot);
    }

    return AraResult< ActivateOptionType >::FromError(RManifestReaderErrc::kInvalidTypeRequested);
}

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara
