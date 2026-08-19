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
/// @file       software_cluster_manifest_parser.cpp
/// @brief      software cluster manifest parser implementation
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
/// @unit_name=SoftwareClusterManifestParser
/// @unit_description=Implementation of the parser for the software cluster manifest
/// @endcode
///
/// ================================================================

#include "software_cluster_manifest_parser.h"

#include "common/log.h"
#include "files_node.h"
#include "types/software_cluster_dependency.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief Parses a manifest
/// @param manifest The manifest json
/// @returns The parsed manifest object representing the json content
/// @throws no
std::unique_ptr< SoftwareClusterManifest > SoftwareClusterManifestParser::Parse(RManifestUPtr const& manifest)
{
    std::unique_ptr< SoftwareClusterManifest > swclMnfUPtr{std::make_unique< SoftwareClusterManifest >()};
    std::unique_ptr< SoftwareClusterManifest > swclMnfNull{nullptr};

    // use Check to return nullptr when load failed with invalid key
    AraString versionStr;
    RManifestLoadAndStrictCheck(manifest, "version", versionStr, swclMnfNull);
    swclMnfUPtr->version = Version(versionStr);

    RManifestLoadAndCheck(manifest, kDependsOnKey, swclMnfUPtr->dependsOnFormula, swclMnfNull);
    RManifestLoadAndCheck(manifest, kConflictsToKey, swclMnfUPtr->conflictsTo, swclMnfNull);

    AraVector< AraString > containedExecutables{};
    RManifestLoadAndCheck(manifest, kContainedExecutables, containedExecutables, swclMnfNull);
    swclMnfUPtr->containedExecutables = AraVectorString(containedExecutables.begin(), containedExecutables.end());

    AraVector< AraString > containedProcesses{};
    RManifestLoadAndCheck(manifest, kContainedProcesses, containedProcesses, swclMnfNull);
    swclMnfUPtr->containedProcesses = AraVectorString(containedProcesses.begin(), containedProcesses.end());

    RManifestLoad(manifest, "category", swclMnfUPtr->category);
    ///std::ignore = manifest->Load(std::move(AraStringView("category")), swclMnfUPtr->Category);.///////////mydel//
    ///assert((&(swclMnfUPtr->Category)) != nullptr);.//////mydel
    ///if ((&(swclMnfUPtr->Category)) == nullptr) {;}//////mydel
    ///std::ignore = swclMnfUPtr->Category;./////////////mydel
    RManifestLoad(manifest, "releaseNotes", swclMnfUPtr->releaseNotes);
    RManifestLoad(manifest, "shortName", swclMnfUPtr->shortName);
    RManifestLoad(manifest, "fullQualifiedName", swclMnfUPtr->fQN);
    RManifestLoad(manifest, "uuid", swclMnfUPtr->uUID);
    RManifestLoad(manifest, "vendorID", swclMnfUPtr->vendorID);
    RManifestLoad(manifest, "vendorSignature", swclMnfUPtr->vendorSignature);
    RManifestLoad(manifest, "certSerialNumber", swclMnfUPtr->certSerialNumber);
    RManifestLoad(manifest, "certIssuer", swclMnfUPtr->certIssuer);
    RManifestLoad(manifest, "typeApproval", swclMnfUPtr->typeApproval);

    std::int32_t ret{0};
    ret = manifest->IterateArray(std::move(AraStringView(kClaimedFunctionGroups)),
                                 [&swclMnfUPtr](size_t, RManifestNode const& node) {
                                     swclMnfUPtr->claimedFunctionGroups.push_back(RManifestNodeGetSS(node, ""));
                                 });  // copy from old impl::GetAllFunctionGroups
    RManifestLoadResultCheck(kClaimedFunctionGroups, ret, swclMnfNull);

    ret = manifest->IterateArray(std::move(AraStringView(kLicenses)),
                                 [&swclMnfUPtr](size_t, RManifestNode const& node) {
                                     swclMnfUPtr->licenses.push_back(RManifestNodeGetSS(node, ""));
                                 });
    RManifestLoadResultCheck(kLicenses, ret, swclMnfNull);

    FilesNode fnode;
    RManifestLoadAndCheck(manifest, "files", fnode, swclMnfNull);
    swclMnfUPtr->files = fnode.GetFiles();

    return swclMnfUPtr;
}

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara
