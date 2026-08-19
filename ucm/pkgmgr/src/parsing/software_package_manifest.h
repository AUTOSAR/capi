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
/// @file       software_package_manifest.h
/// @brief      software package manifest
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
/// @unit_name=SoftwarePackageManifest
/// @unit_description=software package manifest
/// @endcode
///
/// ================================================================

#ifndef ARA_UCM_PKGMGR_PARSING_SOFTWARE_PACKAGE_MANIFEST_H_
#define ARA_UCM_PKGMGR_PARSING_SOFTWARE_PACKAGE_MANIFEST_H_

#include "ara/ucm/pkgmgr/impl_type_actiontype.h"
#include "ara/ucm/pkgmgr/impl_type_activateoptiontype.h"
#include "common/alias.h"
#include "common/strtype.h"
#include "util/string_ext.h"
#include "version.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief Manifest of a software package
///
/// @code{.isoft}
/// @uptrace={SWS_UCM_00029, 734d4556fd4ea60d429916dc074a4057f12ab71f}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10256
/// @trace_id_dd=DD_UCM_10410
/// @needwork = ad
/// @endcode
class SoftwarePackageManifest
{
public:
    /// @brief constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10411
    /// @needwork = dda
    /// @endcode
    SoftwarePackageManifest() = default;

    /// @brief convert SoftwarePackageManifest to string
    /// @return string
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10412
    /// @needwork = dda
    /// @endcode
    AraString Str() const
    {
        return "SoftwarePackageManifest{actionType:" + strtype::ActionTypeToStr(actionType)
               + ", activationAction:" + strtype::ActivateOptionTypeToStr(activationAction) + ", Category:" + category
               + ", compressedSoftwarePackageSize:" + std::to_string(compressedSoftwarePackageSize)
               + ", diagnosticAddress:" + diagnosticAddress + ", isDeltaPackage:" + isDeltaPackage
               + ", deltaPackageApplicableVersion:" + deltaPackageApplicableVersion
               + ", minUCMSupportedVersion:" + minUCMSupportedVersion.ToString()
               + ", maxUCMSupportedVersion:" + maxUCMSupportedVersion.ToString() + ", packagerID:" + packagerID
               + ", packagerSignature:" + packagerSignature + ", certSerialNumber:" + certSerialNumber
               + ", certIssuer:" + certIssuer + ", ShortName:" + shortName + ", typeApproval:" + typeApproval
               + ", ucmIdentifier:" + ucmIdentifier + ", uUID:" + uUID + ", Files:" + strutil::JoinMap(files) + "}";
    }

    /// @brief short info of SoftwarePackageManifest
    /// @return string
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10413
    /// @needwork = dda
    /// @endcode
    AraString ShortInfo() const
    {
        return "SoftwarePackageManifest{actionType:" + strtype::ActionTypeToStr(actionType)
               + ", activationAction:" + strtype::ActivateOptionTypeToStr(activationAction) + ", Category:" + category
               + ", deltaPackageApplicableVersion:" + deltaPackageApplicableVersion + ", ShortName:" + shortName + "}";
    }

public:
    /// @brief actionType
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10414
    /// @needwork = dda
    /// @endcode
    ActionType actionType{ActionType::kUpdate};
    /// @brief activationAction
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10415
    /// @needwork = dda
    /// @endcode
    ActivateOptionType activationAction{ActivateOptionType::kWaitForReboot};
    /// @brief Category
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10416
    /// @needwork = dda
    /// @endcode
    AraString category{
        ""};  // Not actually used yet (and SoftwarePackage does not have this element, SoftwareCluster should have this element)
    /// @brief compressedSoftwarePackageSize
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10417
    /// @needwork = dda
    /// @endcode
    uint32_t compressedSoftwarePackageSize{0U};
    /// @brief diagnosticAddress
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10418
    /// @needwork = dda
    /// @endcode
    AraString diagnosticAddress{
        ""};  // Not actually used yet (and SoftwarePackage does not have this element, SoftwareCluster should have this element)
    /// @brief isDeltaPackage
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10419
    /// @needwork = dda
    /// @endcode
    AraString isDeltaPackage{""};  // Not actually used yet
    /// @brief deltaPackageApplicableVersion
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10420
    /// @needwork = dda
    /// @endcode
    AraString deltaPackageApplicableVersion{""};
    /// @brief minUCMSupportedVersion
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10421
    /// @needwork = dda
    /// @endcode
    Version minUCMSupportedVersion{};
    /// @brief maxUCMSupportedVersion
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10422
    /// @needwork = dda
    /// @endcode
    Version maxUCMSupportedVersion{};
    /// @brief packagerID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10423
    /// @needwork = dda
    /// @endcode
    AraString packagerID{""};  // Not actually used yet
    /// @brief packagerSignature
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10424
    /// @needwork = dda
    /// @endcode
    AraString packagerSignature{""};  // Get certificate information
    /// @brief certSerialNumber
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10425
    /// @needwork = dda
    /// @endcode
    AraString certSerialNumber{""};  // Get the certificate serial number
    /// @brief certIssuer
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10426
    /// @needwork = dda
    /// @endcode
    AraString certIssuer{""};  // Get the certificate issuer
    /// @brief ShortName
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10427
    /// @needwork = dda
    /// @endcode
    AraString shortName{""};  // Not actually used yet
    /// @brief typeApproval
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10428
    /// @needwork = dda
    /// @endcode
    AraString typeApproval{""};  // Not actually used yet (and SoftwarePackage does not have this element)
    /// @brief ucmIdentifier
    /// PackageManagementApplication::GetUcmId takes the value from machine manifest for PackageManagerImpl::GetId to return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10429
    /// @needwork = dda
    /// @endcode
    AraString ucmIdentifier{""};  // Not actually used yet (and SoftwarePackage does not have this element)
    /// @brief uncompressedSoftwarePackageSize
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10430
    /// @needwork = dda
    /// @endcode
    uint32_t uncompressedSoftwarePackageSize{0U};  // Not actually used yet
    /// @brief uUID
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10431
    /// @needwork = dda
    /// @endcode
    AraString uUID{""};  // Not actually used yet
    /// @brief Files
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10432
    /// @needwork = dda
    /// @endcode
    AraMap< AraString, AraString > files{};
};

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // ARA_UCM_PKGMGR_PARSING_SOFTWARE_PACKAGE_MANIFEST_H_
