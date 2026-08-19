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
/// @file       consts.h
/// @brief      Constant definitions
/// @details
/// @date       2024-07-20
/// @author     hanzhibo
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// export_level=/UCM Master/Utils
/// @module_path=/UCM Master/Utils
/// @interface_level=module
/// @trace_id_sr=SR_UCM_00035
/// @unit_name=consts
/// @unit_description=Constant definitions
/// @endcode
///
/// ================================================================

#ifndef _VPKGMGR_CONSTS_H_
#define _VPKGMGR_CONSTS_H_

#include <ara/core/string_view.h>
#include <ara/ucm/pkgmgr/impl_type_packagemanagerstatustype.h>

#include "utils/types.h"

namespace ara {
namespace ucm {
namespace vpkgmgr {

/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00485
/// @trace_id_dd=DD_UCM_Master_00017
/// @needwork = ad
/// @endcode
int32_t const kVpkgSuccess{0};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00485
/// @trace_id_dd=DD_UCM_Master_00018
/// @needwork = dd
/// @endcode
int32_t const kVpkgError{1};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00485
/// @trace_id_dd=DD_UCM_Master_00019
/// @needwork = dd
/// @endcode
uint32_t const kGlobalWaitTime{200U};

// vps
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00485
/// @trace_id_dd=DD_UCM_Master_00020
/// @needwork = dd
/// @endcode
constexpr const char8_t* kNone{"none"};

/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define UUID "uuid"
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define SHORT_NAME "shortName"
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define FQN "fqn"
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define VEHICLE_ROLLOUT_STEPS "rolloutQualification"
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define SAFETY_POLICY    "safetyPolicy"
#define SAFETY_CONDITION "safetyCondition"
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define UCM_STEPS "ucmProcessing"
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define SOFTWARE_PACKAGE_STEPS "softwarePackageStep"
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define UCMS "ucm"
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define UCM "ucm"

/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define ACTIVATION_SWITCH "activationSwitch"
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define PROCESS "process"
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define STORING "storing"
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define TRANSFER "transfer"
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define SOFTWARE_PACKAGE "softwarePackage"
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define DRIVER_NOTIFICATIONS "driverNotification"
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define APPROVAL_REQUIRED "approvalRequired"
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define NOTIFICATION_STATE "notificationState"
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define IDENTIFIER "identifier"
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define UCM_MODULE_INSTANTIATION "ucmModuleInstantiation"
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define UCM_ID "ucmId"
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define UCM_MASTER_FALLBACK "ucmMasterFallback"

/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define CERTIFICATE_SERIAL_NUMBER "certSerialNumber"
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define CERTIFICATE_ISSUER "certIssuer"
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define REPOSITORY "repository"

// swps swcls
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define ACTION_TYPE "actionType"
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define ACTIVATION_ACTION_TYPE "activationAction"
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define TYPE_APPROVAL "typeApproval"
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define LICENSE "license"
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define RELEASE_NOTES "releaseNotes"
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define COMPRESSED_SOFTWARE_PACKAGE_SIZE "compressedSoftwarePackageSize"
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define UNCOMPRESSED_SOFTWARE_PACKAGE_SIZE "uncompressedSoftwarePackageSize"
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define PACKAGER_ID "packagerId"
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define PACKAGER_SIGNATURE "packagerSignature"
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define MIN_UCM_SUPPORTED_VERSION "minUCMSupportedVersion"
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define MAX_UCM_SUPPORTED_VERSION "maxUCMSupportedVersion"
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define VERSION "version"
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define SOFTWARE_CLUSTER "softwareCluster"

// per
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define UCMIDS "ucmIds"
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define DISABLE_CAMPAIGN "disable_campaign"
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define VPKG_ZIP_NAME "vpkg_zip"
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define VPKG_CAMPAGIN_STATE "campagin_state"

// env str
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define KV_MODEL_IDENTIFIER "ucmmd/ucmm_root/SwPackageTransferStatusPRPort"
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
#define CAMPAIGN_HISTORY_KV_MODEL_IDENTIFIER "ucmmd/ucmm_root/CampaignHistoryPRPort"

/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00485
/// @trace_id_dd=DD_UCM_Master_00021
/// @needwork = dd
/// @endcode
constexpr const char8_t* kUcmmVersion{"1.0.0"};  // PRQA S 2428,4151

// ucmm status file name
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00485
/// @trace_id_dd=DD_UCM_Master_00022
/// @needwork = dd
/// @endcode
constexpr const char8_t* kStatusFileName{"status.json"};  // PRQA S 2428,4151
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00485
    /// @trace_id_dd=DD_UCM_Master_00023
    /// @needwork = dd
    /// @endcode
constexpr const char8_t* kStatusMD5FileName{"status.md5"};  // PRQA S 2428,4151

// ucmm status information
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00485
/// @trace_id_dd=DD_UCM_Master_00024
/// @needwork = dd
/// @endcode
constexpr const char8_t* kUCMMVersion{"Version"};  // PRQA S 2428,4151
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00485
    /// @trace_id_dd=DD_UCM_Master_00025
    /// @needwork = dd
    /// @endcode
constexpr const char8_t* kUCMMPreCampaignState{"PreCampaignState"};  // PRQA S 2428,4151
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00485
    /// @trace_id_dd=DD_UCM_Master_00026
    /// @needwork = dd
    /// @endcode
constexpr const char8_t* kUCMMCampaignState{"CampaignState"};  // PRQA S 2428,4151
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00485
    /// @trace_id_dd=DD_UCM_Master_00027
    /// @needwork = dd
    /// @endcode
constexpr const char8_t* kUCMMVPKTransferID{"VPKTransferID"};  // PRQA S 2428,4151
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00485
    /// @trace_id_dd=DD_UCM_Master_00028
    /// @needwork = dd
    /// @endcode
constexpr const char8_t* kUCMMBeginFinish{"BeginFinish"};  // PRQA S 2428,4151
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00485
    /// @trace_id_dd=DD_UCM_Master_00029
    /// @needwork = dd
    /// @endcode
constexpr const char8_t* kUCMMLastHeatBeatTime{"LastHeatBeatTime"};  // PRQA S 2428,4151
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00485
    /// @trace_id_dd=DD_UCM_Master_00030
    /// @needwork = dd
    /// @endcode
constexpr const char8_t* kUCMMAllowCampaign{"AllowCampaign"};  // PRQA S 2428,4151
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00485
    /// @trace_id_dd=DD_UCM_Master_00031
    /// @needwork = dd
    /// @endcode
constexpr const char8_t* kUCMMSWPKGs{"SWPKGs"};  // PRQA S 2428,4151
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00485
    /// @trace_id_dd=DD_UCM_Master_00032
    /// @needwork = dd
    /// @endcode
constexpr const char8_t* kUCMMSWPFQN{"SWPFQN"};  // PRQA S 2428,4151
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00485
    /// @trace_id_dd=DD_UCM_Master_00033
    /// @needwork = dd
    /// @endcode
constexpr const char8_t* kUCMMTransferID{"TransferID"};  // PRQA S 2428,4151
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00485
    /// @trace_id_dd=DD_UCM_Master_00034
    /// @needwork = dd
    /// @endcode
constexpr const char8_t* kUCMMCampaignHistoryKey{"CampaignHistoryKey"};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00485
/// @trace_id_dd=DD_UCM_Master_00035
/// @needwork = dd
/// @endcode
constexpr const char8_t* kUCMMCancelCause{"CancelCause"};  // PRQA S 2428,4151

// MD5 Digest length
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00485
/// @trace_id_dd=DD_UCM_Master_00036
/// @needwork = dd
/// @endcode
constexpr size_t kMD5DigestLen{16U};  // PRQA S 2427

/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00485
/// @trace_id_dd=DD_UCM_Master_00037
/// @needwork = dd
/// @endcode
constexpr std::uint64_t kMaxDurationOfUnconnect{20U * 60U * 1000U};  // 20min

// Vehicle package manifest file name
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00485
/// @trace_id_dd=DD_UCM_Master_00038
/// @needwork = dd
/// @endcode
constexpr const char8_t* kVpkgManifestFileName{"ucm_vps.json"};  // PRQA S 2428,4151

// Vehicle package manifest signature file name
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00485
/// @trace_id_dd=DD_UCM_Master_00039
/// @needwork = dd
/// @endcode
constexpr const char8_t* kVpkgManifestSigFileName{"ucm_vps.sig"};  // PRQA S 2428,4151

// Key prefix for Campaign history record persistence storage
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00485
/// @trace_id_dd=DD_UCM_Master_00040
/// @needwork = dd
/// @endcode
constexpr const char8_t* kCampaignHistoryKeyPrefix{"campaign"};  // PRQA S 2428,4151

// Key prefix for persistent storage of UCM transfer information
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00485
/// @trace_id_dd=DD_UCM_Master_00041
/// @needwork = dd
/// @endcode
constexpr const char8_t* kUCMTransferInfoKeyPrefix{"UCMTransferInfoKey_"};

}  // namespace vpkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // _VPKGMGR_CONSTS_H_