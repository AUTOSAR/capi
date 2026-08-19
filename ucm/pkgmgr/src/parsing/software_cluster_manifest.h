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
/// @file       software_cluster_manifest.h
/// @brief      software cluster manifest
/// @details
/// @date       2024-08-15
/// @author     cuiyinli
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @export_level=/UCM/SoftwarePackageManager
/// @module_path=/UCM/SoftwarePackageManager
/// @interface_level=unit
/// @trace_id_sr=SR_UCM_00001
/// @unit_name=SoftwareClusterManifest
/// @unit_description=Data structure representing the manifest of a software cluster
/// @endcode
///
/// ================================================================

#ifndef ARA_UCM_PKGMGR_PARSING_SOFTWARE_CLUSTER_MANIFEST_H_
#define ARA_UCM_PKGMGR_PARSING_SOFTWARE_CLUSTER_MANIFEST_H_

#include "common/alias.h"
#include "types/software_cluster_dependency.h"
#include "util/string_ext.h"
#include "version.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief Data structure representing the manifest of a software cluster.
///
/// @code{.isoft}
/// @uptrace={SWS_UCM_00112, eca80d916febfe9d67a7cdae6334a0f19ae58ace}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10259
/// @trace_id_dd=DD_UCM_10457
/// @needwork = ad
/// @endcode
struct SoftwareClusterManifest
{
    /// @brief Category
    AraString category{""};
    /// @brief ReleaseNotes
    AraString releaseNotes{""};
    /// @brief Licenses
    AraVectorString licenses{};
    /// @brief PreviousVersion
    Version previousVersion{};
    /// @brief ShortName
    AraString shortName{""};
    /// @brief FQN
    AraString fQN{""};
    /// @brief uUID
    AraString uUID{""};
    /// @brief vendorID
    AraString vendorID{""};
    /// @brief vendorSignature
    AraString vendorSignature{""};
    /// @brief certSerialNumber
    AraString certSerialNumber{""};
    /// @brief certIssuer
    AraString certIssuer{""};
    /// @brief version
    Version version{};
    /// @brief typeApproval
    AraString typeApproval{""};
    /// @brief kRequiredSoftwareClusters
    AraVectorString const kRequiredSoftwareClusters{};
    /// @brief claimedFunctionGroups
    AraVectorString claimedFunctionGroups{};
    /// @brief dependsOnFormula
    SoftwareClusterDependencyFormula dependsOnFormula{};
    /// @brief conflictsTo
    ///SoftwareClusterDependencyFormula conflictsTo = {false};
    SoftwareClusterDependencyFormula conflictsTo{false};
    /// @brief containedExecutables
    ///SoftwareClusterDependencyFormula conflictsTor(false);
    AraVectorString containedExecutables{};
    /// @brief containedProcesses
    AraVectorString containedProcesses{};
    /// @brief Files
    AraMap< AraString, AraString > files{};
};

/// @brief utility for software cluster manifest
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10260
/// @trace_id_dd=DD_UCM_10458
/// @needwork = ad
/// @endcode
class SwclManifestUtil
{
public:
    /// @brief constructor
    /// @param swclMnf
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10459
    /// @needwork = dda
    /// @endcode
    explicit SwclManifestUtil(SoftwareClusterManifest const& swclMnf) noexcept : swclMnf_{swclMnf} {}

    /// @brief convert SoftwareClusterManifest to string
    /// @return string
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10460
    /// @needwork = dda
    /// @endcode
    AraString Str() const
    {
        return "SoftwareClusterManifest{Category:" + swclMnf_.category + ", ReleaseNotes:" + swclMnf_.releaseNotes
               + ", Licenses:" + strutil::JoinVector(swclMnf_.licenses)
               + ", PreviousVersion:" + swclMnf_.previousVersion.ToString() + ", ShortName:" + swclMnf_.shortName
               + ", FQN:" + swclMnf_.fQN + ", uUID:" + swclMnf_.uUID + ", vendorID:" + swclMnf_.vendorID
               + ", vendorSignature:" + swclMnf_.vendorSignature + ", certSerialNumber:" + swclMnf_.certSerialNumber
               + ", certIssuer:" + swclMnf_.certIssuer + ", version:" + swclMnf_.version.ToString()
               + ", typeApproval:" + swclMnf_.typeApproval +

               ", claimedFunctionGroups:" + strutil::JoinVector(swclMnf_.claimedFunctionGroups)
               + ", dependsOnFormula:" + "dependsOnFormula" +  ////mytodo////
               ", conflictsTo:" + "conflictsTo" +              ////mytodo////
               ", containedExecutables:" + strutil::JoinVector(swclMnf_.containedExecutables) + ", containedProcesses:"
               + strutil::JoinVector(swclMnf_.containedProcesses) + ", Files:" + strutil::JoinMap(swclMnf_.files) + "}";
    }

    /// @brief short info of SoftwareClusterManifest
    /// @return string
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10461
    /// @needwork = dda
    /// @endcode
    AraString ShortInfo() const
    {
        return "SoftwareClusterManifest{Category:" + swclMnf_.category + ", ShortName:" + swclMnf_.shortName
               + ", FQN:" + swclMnf_.fQN + ", version:" + swclMnf_.version.ToString() + "}";
    }

private:
    /// @brief swclMnf_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10462
    /// @needwork = dda
    /// @endcode
    SoftwareClusterManifest const& swclMnf_;
};

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // ARA_UCM_PKGMGR_PARSING_SOFTWARE_CLUSTER_MANIFEST_H_
