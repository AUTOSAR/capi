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
/// @file       const.h
/// @brief      Common constants definition
/// @details
/// @date       2024-08-15
/// @author     cuiyinli
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @export_level=/UCM/Utils
/// @module_path=/UCM/Utils
/// @interface_level=unit
/// @trace_id_sr=SR_UCM_00001
/// @unit_name=Const
/// @unit_description=Common constants definition
/// @endcode
///
/// ================================================================

#ifndef UCM_PKGMGR_SRC_COMMON_CONST_H_
#define UCM_PKGMGR_SRC_COMMON_CONST_H_

#include <ara/core/string_view.h>

#include <cassert>

#include "alias.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief AraStringView
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
using AraStringView = ara::core::StringView;

// Common constant definitions
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10056
/// @trace_id_dd=DD_UCM_11015
/// @needwork = dd
/// @endcode
constexpr int kBuffLen{1024};  // PRQA S 2427
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_
    /// @trace_id_dd=DD_UCM_11016
    /// @needwork = no
    /// @endcode
constexpr size_t kMD5DigestLen{16U};  // PRQA S 2427
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10399
    /// @trace_id_dd=DD_UCM_11017
    /// @needwork = dd
    /// @endcode
constexpr const char* kPathSeparator{"/"};  // PRQA S 2428,4151

// Name of the ISOFT_CORE_OS type OS software set
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10252
/// @trace_id_dd=DD_UCM_11018
/// @needwork = dd
/// @endcode
constexpr const char* kOS_SWCL_NAME{"os"};  // PRQA S 2428,4151

// Name of the ISOFT_FRAMEWORK type framework software set
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10252
/// @trace_id_dd=DD_UCM_11019
/// @needwork = dd
/// @endcode
constexpr const char* kFRAMEWORK_SWCL_NAME{"framework"};  // PRQA S 2428,4151

// Name of the platform type software cluster
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10252
/// @trace_id_dd=DD_UCM_11020
/// @needwork = dd
/// @endcode
constexpr const char* kCORE_SWCL_NAME{"core"};  // PRQA S 2428,4151

// Name of the directory where the platform type software cluster's applications are located
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_11021
/// @needwork = no
/// @endcode
constexpr const char* kPLATFORM_CORE_SWCL_APP_PARENT_DIR{"apps"};  // PRQA S 2428,4151

// Name of the directory where the platform type software cluster's configuration is located
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10056
/// @trace_id_dd=DD_UCM_11022
/// @needwork = dd
/// @endcode
constexpr const char* kPLATFORM_CORE_SWCL_ETC_DIR{"etc"};  // PRQA S 2428,4151

// Application-level software set list configuration and MD5 filename
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10056
/// @trace_id_dd=DD_UCM_11023
/// @needwork = dd
/// @endcode
constexpr const char* kAPPLICATION_SWCL_LIST_CFG_FILE_NAME{"run_time_application_swcl_list.json"};  // PRQA S 2428,4151
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10056
    /// @trace_id_dd=DD_UCM_11024
    /// @needwork = dd
    /// @endcode
constexpr const char* kAPPLICATION_SWCL_LIST_MD5_FILE_NAME{"run_time_application_swcl_list.md5"};  // PRQA S 2428,4151

// Application-level software set list filename prefix and suffix
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10056
/// @trace_id_dd=DD_UCM_11025
/// @needwork = dd
/// @endcode
constexpr const char* kSWCLListFileNamePrefix{"run_time_application_swcl_list_ver"};  // PRQA S 2428,4151
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10056
    /// @trace_id_dd=DD_UCM_11026
    /// @needwork = dd
    /// @endcode
constexpr const char* kSWCLListFileNamePostfix{".json"};  // PRQA S 2428,4151

// Keys related to the software set list
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10056
/// @trace_id_dd=DD_UCM_11027
/// @needwork = dd
/// @endcode
constexpr const char* kSwcls{"swcls"};  // PRQA S 2428,4151
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10056
    /// @trace_id_dd=DD_UCM_11028
    /// @needwork = dd
    /// @endcode
constexpr const char* kSwclName{"swcl_name"};  // PRQA S 2428,4151
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10056
    /// @trace_id_dd=DD_UCM_11029
    /// @needwork = dd
    /// @endcode
constexpr const char* kSwclFQN{"swcl_fqn"};  // PRQA S 2428,4151
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10056
    /// @trace_id_dd=DD_UCM_11030
    /// @needwork = dd
    /// @endcode
constexpr const char* kSwclCategory{"category"};  // PRQA S 2428,4151
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10056
    /// @trace_id_dd=DD_UCM_11031
    /// @needwork = dd
    /// @endcode
constexpr const char* kSwclVersion{"swcl_version"};  // PRQA S 2428,4151
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10311
    /// @trace_id_dd=DD_UCM_11032
    /// @needwork = dd
    /// @endcode
constexpr const char* kDependsOn{"dependsOn"};  // PRQA S 2428,4151
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10311
    /// @trace_id_dd=DD_UCM_11033
    /// @needwork = dd
    /// @endcode
constexpr const char* kExecutableList{"executable_list"};  // PRQA S 2428,4151
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10311
    /// @trace_id_dd=DD_UCM_11034
    /// @needwork = dd
    /// @endcode
constexpr const char* kProcessList{"process_list"};  // PRQA S 2428,4151

// Boot option filename prefix and suffix
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10056
/// @trace_id_dd=DD_UCM_11035
/// @needwork = dd
/// @endcode
constexpr const char* kBootOptionFileNamePrefix{"ara_ver"};  // PRQA S 2428,4151
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10056
    /// @trace_id_dd=DD_UCM_11036
    /// @needwork = dd
    /// @endcode
constexpr const char* kBootOptionFileNamePostfix{".json"};  // PRQA S 2428,4151
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10056
    /// @trace_id_dd=DD_UCM_11037
    /// @needwork = dd
    /// @endcode
constexpr const char* kBootOptionMD5FileNamePostfix{".md5"};  // PRQA S 2428,4151

// Configuration in ara_ver
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_11038
/// @needwork = no
/// @endcode
constexpr const char* kBaseSection{"base"};  // PRQA S 2428,4151
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_
    /// @trace_id_dd=DD_UCM_11039
    /// @needwork = no
    /// @endcode
constexpr const char* kPlatformFramework{"platform_framework"};  // PRQA S 2428,4151
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_
    /// @trace_id_dd=DD_UCM_11040
    /// @needwork = no
    /// @endcode
constexpr const char* kPlatformCore{"platform_core"};  // PRQA S 2428,4151
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_
    /// @trace_id_dd=DD_UCM_11041
    /// @needwork = no
    /// @endcode
constexpr const char* kApplicationSWCLS{"application_swcls"};  // PRQA S 2428,4151
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_
    /// @trace_id_dd=DD_UCM_11042
    /// @needwork = no
    /// @endcode
constexpr const char* kPlatformVar{"platform_var"};  // PRQA S 2428,4151

// Names of framework & core version configuration items in ara_ver
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10056
/// @trace_id_dd=DD_UCM_11043
/// @needwork = dd
/// @endcode
constexpr const char* kFrameworkCoreSection{"ucm"};  // PRQA S 2428,4151
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10056
    /// @trace_id_dd=DD_UCM_11044
    /// @needwork = dd
    /// @endcode
constexpr const char* kFrameworkVersion{"framework_version"};  // PRQA S 2428,4151
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10056
    /// @trace_id_dd=DD_UCM_11045
    /// @needwork = dd
    /// @endcode
constexpr const char* kCoreVersion{"core_version"};  // PRQA S 2428,4151

// UCM status filename
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10399
/// @trace_id_dd=DD_UCM_11046
/// @needwork = dd
/// @endcode
constexpr const char* kUCMStatusFileName{"ucm_status.json"};  // PRQA S 2428,4151
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10399
    /// @trace_id_dd=DD_UCM_11047
    /// @needwork = dd
    /// @endcode
constexpr const char* kUCMStatusMD5FileName{"ucm_status.md5"};  // PRQA S 2428,4151

// UCM status information
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10325
/// @trace_id_dd=DD_UCM_11048
/// @needwork = dd
/// @endcode
constexpr const char* kFixedUCMVersion{"1.0.0"};
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10104
/// @trace_id_dd=DD_UCM_11049
/// @needwork = dd
/// @endcode
constexpr const char* kUCMVersion{"Version"};  // PRQA S 2428,4151
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10104
    /// @trace_id_dd=DD_UCM_11050
    /// @needwork = dd
    /// @endcode
constexpr const char* kUCMCurrentStatus{"CurrentStatus"};  // PRQA S 2428,4151
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10104
    /// @trace_id_dd=DD_UCM_11051
    /// @needwork = dd
    /// @endcode
constexpr const char* kUCMFinalAction{"FinalAction"};  // PRQA S 2428,4151
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10104
    /// @trace_id_dd=DD_UCM_11052
    /// @needwork = dd
    /// @endcode
constexpr const char* kUCMProcessedSoftwarePackages{"ProcessedSoftwarePackages"};  // PRQA S 2428,4151
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10104
    /// @trace_id_dd=DD_UCM_11053
    /// @needwork = dd
    /// @endcode
constexpr const char* kUCMProcessedSoftwarePackageActionResolution{"ActionResolution"};  // PRQA S 2428,4151
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10104
    /// @trace_id_dd=DD_UCM_11054
    /// @needwork = dd
    /// @endcode
constexpr const char* kUCMProcessedSoftwarePackageActionTimeStamp{"ActionTimeStamp"};  // PRQA S 2428,4151
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10102
    /// @trace_id_dd=DD_UCM_11055
    /// @needwork = dd
    /// @endcode
constexpr const char* kUCMProcessedSoftwarePackageActionType{"ActionType"};  // PRQA S 2428,4151
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10102
    /// @trace_id_dd=DD_UCM_11056
    /// @needwork = dd
    /// @endcode
constexpr const char* kUCMProcessedSoftwarePackageActivateOptionType{"ActivateOptionType"};  // PRQA S 2428,4151
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10102
    /// @trace_id_dd=DD_UCM_11057
    /// @needwork = dd
    /// @endcode
constexpr const char* kUCMProcessedSoftwarePackagedeltaPackageApplicableVersion{
    "deltaPackageApplicableVersion"};  // PRQA S 2428,4151
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10102
/// @trace_id_dd=DD_UCM_11058
/// @needwork = dd
/// @endcode
constexpr const char* kUCMProcessedSoftwarePackageSwclName{"SwclName"};  // PRQA S 2428,4151
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10102
    /// @trace_id_dd=DD_UCM_11059
    /// @needwork = dd
    /// @endcode
constexpr const char* kUCMProcessedSoftwarePackageCurSwclVersion{"CurSwclVersion"};  // PRQA S 2428,4151
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10102
    /// @trace_id_dd=DD_UCM_11060
    /// @needwork = dd
    /// @endcode
constexpr const char* kUCMProcessedSoftwarePackageSwclVersion{"SwclVersion"};  // PRQA S 2428,4151

// Name of the directory where the software set's configuration is located
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10066
/// @trace_id_dd=DD_UCM_11061
/// @needwork = dd
/// @endcode
constexpr const char* kETC_DIR_NAME{"etc"};  // PRQA S 2428,4151

// Name of the directory where the software set's binaries are located
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10252
/// @trace_id_dd=DD_UCM_11062
/// @needwork = dd
/// @endcode
constexpr const char* kBIN_DIR_NAME{"bin"};  // PRQA S 2428,4151

// KV persistent storage identifier
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10324
/// @trace_id_dd=DD_UCM_11063
/// @needwork = dd
/// @endcode
constexpr const char* kKV_MODEL_IDENTIFIER{
    "ucmd/package_manager_root/SwPackageTransferStatusPRPort"};  // PRQA S 2428,4151

// Key for function group information in the machine manifest
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10389
/// @trace_id_dd=DD_UCM_11064
/// @needwork = dd
/// @endcode
constexpr const char* kFunctionGroups{"functionGroups"};  // PRQA S 2428,4151

// Software set status filename
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10399
/// @trace_id_dd=DD_UCM_11065
/// @needwork = dd
/// @endcode
constexpr const char* kSwclStatusFileName{"swcl_status.json"};

// Software set manifest filename
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10252
/// @trace_id_dd=DD_UCM_11066
/// @needwork = dd
/// @endcode
constexpr const char* kSwclManifestFileName{"swcl_manifest.json"};  // PRQA S 2428,4151

// Software set manifest signature filename
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10252
/// @trace_id_dd=DD_UCM_11067
/// @needwork = dd
/// @endcode
constexpr const char* kSwclManifestSigFileName{"swcl_manifest.sig"};  // PRQA S 2428,4151

// Software package manifest filename
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10252
/// @trace_id_dd=DD_UCM_11068
/// @needwork = dd
/// @endcode
constexpr const char* kSwpkgManifestFileName{"swpkg_manifest.json"};  // PRQA S 2428,4151

// Software package manifest signature filename
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10252
/// @trace_id_dd=DD_UCM_11069
/// @needwork = dd
/// @endcode
constexpr const char* kSwpkgManifestSigFileName{"swpkg_manifest.sig"};  // PRQA S 2428,4151

// Key for containedExecutables in the software set manifest
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10253
/// @trace_id_dd=DD_UCM_11070
/// @needwork = dd
/// @endcode
constexpr const char* kContainedExecutables{"containedExecutables"};  // PRQA S 2428,4151

// Key for containedProcesses in the software set manifest
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10253
/// @trace_id_dd=DD_UCM_11071
/// @needwork = dd
/// @endcode
constexpr const char* kContainedProcesses{"containedProcesses"};  // PRQA S 2428,4151

// Key for claimedFunctionGroups in the software set manifest
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10253
/// @trace_id_dd=DD_UCM_11072
/// @needwork = dd
/// @endcode
constexpr const char* kClaimedFunctionGroups{"claimedFunctionGroups"};  // PRQA S 2428,4151

// Key for claimedFunctionGroups in the software set manifest
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10253
/// @trace_id_dd=DD_UCM_11073
/// @needwork = dd
/// @endcode
constexpr const char* kLicenses{"licenses"};  // PRQA S 2428,4151

/** Standard name for Machine State Function Group */
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10389
/// @trace_id_dd=DD_UCM_11074
/// @needwork = dd
/// @endcode
constexpr const char* kMachineFunctionGroupName{"MachineFG"};  // PRQA S 2428,4151

// Persistent configuration filename
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_11075
/// @needwork = no
/// @endcode
constexpr const char* kStorageManifestFileName{"storage_manifest.json"};  // PRQA S 2428,4151

// Function group configuration filename
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10389
/// @trace_id_dd=DD_UCM_11076
/// @needwork = dd
/// @endcode
constexpr const char* kFunctionGroupsFileName{"function_groups.json"};

// "dependsOn": [.
//       { // SoftwareClusterDependencyFormula.
//           "category": "functionalDependency/structuralDependency",.  // may not exist
//           "operator": "and/or",.      // may not exist
//           "part": [ // may not exist, can consider this SoftwareClusterDependencyFormula as true.
//               { // SoftwareClusterDependencyCompareCondition .
//                   "compareType": "isEqual/isGreaterThan/isGreaterThanOrEqual/isLessThan/isLessThanOrEqual/", .
//                   "considerBuildNumber": "true/false", .
//                   "softwareCluster": "swcl1",.   // may not exist???
//                   "version": "0.1.1"
//               }.
//           ].
//       }.
//   ].
// Keys related to software set dependencies
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10253
/// @trace_id_dd=DD_UCM_11077
/// @needwork = dd
/// @endcode
constexpr const char* kDependsOnKey{"dependsOn"};  // PRQA S 2428,4151
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10253
    /// @trace_id_dd=DD_UCM_11078
    /// @needwork = dd
    /// @endcode
constexpr const char* kConflictsToKey{"conflictsTo"};  // PRQA S 2428,4151

/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10253
/// @trace_id_dd=DD_UCM_11079
/// @needwork = dd
/// @endcode
constexpr const char* kSWCLDepFormulaCategory{"category"};  // PRQA S 2428,4151
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10253
    /// @trace_id_dd=DD_UCM_11080
    /// @needwork = dd
    /// @endcode
constexpr const char* kSWCLDepFormulaOperator{"operator"};  // PRQA S 2428,4151
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10253
    /// @trace_id_dd=DD_UCM_11081
    /// @needwork = dd
    /// @endcode
constexpr const char* kSWCLDepFormulaParts{"part"};  // PRQA S 2428,4151

/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10253
/// @trace_id_dd=DD_UCM_11082
/// @needwork = dd
/// @endcode
constexpr const char* kSWCLDepCompCondCompareType{"compareType"};  // PRQA S 2428,4151
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10253
    /// @trace_id_dd=DD_UCM_11083
    /// @needwork = dd
    /// @endcode
constexpr const char* kSWCLDepCompCondConsiderBuildNumber{"considerBuildNumber"};  // PRQA S 2428,4151
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10253
    /// @trace_id_dd=DD_UCM_11084
    /// @needwork = dd
    /// @endcode
constexpr const char* kSWCLDepCompCondSWCL{"softwareCluster"};  // PRQA S 2428,4151
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10253
    /// @trace_id_dd=DD_UCM_11085
    /// @needwork = dd
    /// @endcode
constexpr const char* kSWCLDepCompCondVersion{"version"};  // PRQA S 2428,4151

// Related to software set dependencies
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10253
/// @trace_id_dd=DD_UCM_11086
/// @needwork = dd
/// @endcode
constexpr const char* kSWCLDepFormulaCatFunctionalDep{"functionalDependency"};  // PRQA S 2428,4151
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10253
    /// @trace_id_dd=DD_UCM_11087
    /// @needwork = dd
    /// @endcode
constexpr const char* kSWCLDepFormulaCatStructuralDep{"structuralDependency"};  // PRQA S 2428,4151

/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10253
/// @trace_id_dd=DD_UCM_11088
/// @needwork = dd
/// @endcode
constexpr const char* kSWCLDepFormulaOperatorAnd{"and"};  // PRQA S 2428,4151
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10253
    /// @trace_id_dd=DD_UCM_11089
    /// @needwork = dd
    /// @endcode
constexpr const char* kSWCLDepFormulaOperatorOr{"or"};  // PRQA S 2428,4151

/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10253
/// @trace_id_dd=DD_UCM_11090
/// @needwork = dd
/// @endcode
constexpr const char* kSWCLDepFormulaCompCondOperatorIsEqual{"isEqual"};  // PRQA S 2428,4151
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10253
    /// @trace_id_dd=DD_UCM_11091
    /// @needwork = dd
    /// @endcode
constexpr const char* kSWCLDepFormulaCompCondOperatorIsGreaterThan{"isGreaterThan"};  // PRQA S 2428,4151
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10253
    /// @trace_id_dd=DD_UCM_11092
    /// @needwork = dd
    /// @endcode
constexpr const char* kSWCLDepFormulaCompCondOperatorIsGreaterThanOrEqual{"isGreaterThanOrEqual"};  // PRQA S 2428,4151
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10253
    /// @trace_id_dd=DD_UCM_11093
    /// @needwork = dd
    /// @endcode
constexpr const char* kSWCLDepFormulaCompCondOperatorIsLessThan{"isLessThan"};  // PRQA S 2428,4151
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10253
    /// @trace_id_dd=DD_UCM_11094
    /// @needwork = dd
    /// @endcode
constexpr const char* kSWCLDepFormulaCompCondOperatorIsLessThanOrEqual{"isLessThanOrEqual"};  // PRQA S 2428,4151

// Software set category
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10067
/// @trace_id_dd=DD_UCM_11095
/// @needwork = dd
/// @endcode
constexpr const char* kIsoftOSSWCLCategory{"ISOFT_CORE_OS"};  // PRQA S 2428,4151
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10067
    /// @trace_id_dd=DD_UCM_11096
    /// @needwork = dd
    /// @endcode
constexpr const char* kIsoftFrameworkSWCLCategory{"ISOFT_FRAMEWORK"};  // PRQA S 2428,4151
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10067
    /// @trace_id_dd=DD_UCM_11097
    /// @needwork = dd
    /// @endcode
constexpr const char* kPlatformCoreSWCLCategory{"PLATFORM_CORE"};  // PRQA S 2428,4151
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10067
    /// @trace_id_dd=DD_UCM_11098
    /// @needwork = dd
    /// @endcode
constexpr const char* kAppLayerSWCLCategory{"APPLICATION_LAYER"};  // PRQA S 2428,4151

// Application software set list configuration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10056
/// @trace_id_dd=DD_UCM_11099
/// @needwork = dd
/// @endcode
constexpr const char* kApplicationSWCLListSection{"run_time_application_swcl_list"};  // PRQA S 2428,4151
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10056
    /// @trace_id_dd=DD_UCM_11100
    /// @needwork = dd
    /// @endcode
constexpr const char* kFileNameKey{"file_name"};  // PRQA S 2428,4151
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10056
    /// @trace_id_dd=DD_UCM_11101
    /// @needwork = dd
    /// @endcode
constexpr const char* kMD5Key{"md5"};  // PRQA S 2428,4151

// Framework & core software set list filename
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10066
/// @trace_id_dd=DD_UCM_11102
/// @needwork = dd
/// @endcode
constexpr const char* kFrameworkCoreListFileName{"run_time_framework_core_list.json"};  // PRQA S 2428,4151

// Size of the blocks to be received with TransferData method.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10333
/// @trace_id_dd=DD_UCM_11103
/// @needwork = dd
/// @endcode
constexpr size_t kUCM_DATA_BLOCK_SIZE{10U * 1024U * 1024U};  // 10M // PRQA S 2427

// Script filename to be executed for OS update
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10270
/// @trace_id_dd=DD_UCM_11104
/// @needwork = dd
/// @endcode
constexpr const char* kUpdateOSScriptName{"updateOS"};  // PRQA S 2428,4151

// Waiting time to find the UpdateRequest service
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10002
/// @trace_id_dd=DD_UCM_11105
/// @needwork = dd
/// @endcode
constexpr const char* kMaxWaitTimeWhenCallUpdateMethodKey{"max_wait_time_when_call_update_method_ms"};
///constexpr auto kDefaultCheckPeriod = std::chrono::milliseconds(2000);////////not use auto///////mytodo

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // UCM_PKGMGR_SRC_COMMON_CONST_H_
