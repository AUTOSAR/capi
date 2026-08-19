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
/// @file       path.h
/// @brief      ara path
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
/// @interface_level=module
/// @trace_id_sr=SR_UCM_00001
/// @unit_name=Path
/// @unit_description=Global path settings
/// @endcode
///
/// ================================================================

#ifndef ARA_UCM_PKGMGR_COMMON_PATH_H_
#define ARA_UCM_PKGMGR_COMMON_PATH_H_

#include "alias.h"
#include "util/noncopy.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief global path settings
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10399
/// @trace_id_dd=DD_UCM_11106
/// @needwork = ad
/// @endcode
class Path : NonCopyNonMove
{
public:
    /// @brief PathSettings
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10399
    /// @trace_id_dd=DD_UCM_11107
    /// @needwork = dd
    /// @endcode
    struct PathSettings
    {
        /// @brief ara root dir, config file for ara startup is here
        /// like ara-sysroot/ara/
        AraString araDir;
        /// @brief installed dir for software cluster named framework with category ISOFT_FRAMEWORK
        /// like ara/framework/
        AraString frameworkDir;
        /// @brief installed dir for software cluster named core with category PLATFORM_CORE
        /// like ara/core/
        AraString coreDir;
        /// @brief installed dir for normal software cluster with category APPLICATION_LAYER
        /// like ara/swcls/
        AraString swclsDir;
        /// @brief ucm data dir, which has: swps ucm_status_verX.json history.json
        /// like ara/var/ucm/
        AraString varDataDir;
        /// @brief ucm transfered software packages dir, to store zip file and extracted zip dir
        /// like ara/var/ucm//swps
        AraString swpkgsDir;
        /// @brief ucm history file
        /// like ara/var/ucm/history.json
        AraString historyPath;
        /// @brief ucm status file
        /// like ara/var/ucm/ucm_status.json
        AraString uCMStatusPath;
        /// @brief ucm status md5 file
        /// like ara/var/ucm/ucm_status.md5
        AraString uCMStatusMD5Path;
        /// @brief The location of the Machine Manifest including UCM instance info File to parse
        /// like /etc/1.1.0/machine_manifest.json
        AraString machineManifestPath;
        /// @brief The location of the Execution Manifest include max_wait_time_when_call_update_method_ms
        /// like what//////////mytodo////TODO//
        AraString executionManifestPath;
    };

public:
    /// @brief constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_11108
    /// @needwork = dda
    /// @endcode
    Path() = default;
    /// @brief destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_11109
    /// @needwork = dda
    /// @endcode
    ~Path() = default;

    /// @brief delete copy construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_11110
    /// @needwork = dda
    /// @endcode
    Path(Path const& other) = delete;
    /// @brief delete copy asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_11111
    /// @needwork = dda
    /// @endcode
    Path& operator=(Path const& other) = delete;
    /// @brief delete move construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_11112
    /// @needwork = dda
    /// @endcode
    Path(Path&& other) = delete;
    /// @brief delete move asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_11113
    /// @needwork = dda
    /// @endcode
    Path& operator=(Path&& other) = delete;

    /// @brief load path settings
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_11114
    /// @needwork = dda
    /// @endcode
    void Load() noexcept;

    /// @brief str of global path settings
    /// @throws no
    /// @return str
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_11115
    /// @needwork = dda
    /// @endcode
    AraString Str() const;

    /// @brief swcl path
    /// @param name swcl name
    /// @throws no
    /// @return str
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_11116
    /// @needwork = dda
    /// @endcode
    AraString GetSwclInstallDir(AraString const& name) const;
    /// @brief swcl path
    /// @param name swcl name
    /// @param version swcl version
    /// @throws no
    /// @return str
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_11117
    /// @needwork = dda
    /// @endcode
    AraString GetSwclInstallDir(AraString const& name, AraString const& version) const;
    /// @brief swcl path
    /// @param name swcl name
    /// @throws no
    /// @return str
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_11118
    /// @needwork = dda
    /// @endcode
    AraString GetSwclStatusPath(AraString const& name) const;
    /// @brief swcl path
    /// @param name swcl name
    /// @param version swcl version
    /// @throws no
    /// @return str
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_11119
    /// @needwork = dda
    /// @endcode
    AraString GetSwclManifestPath(AraString const& name, AraString const& version) const;
    /// @brief swcl path
    /// @param name swcl name
    /// @param version swcl version
    /// @throws no
    /// @return str
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_11120
    /// @needwork = dda
    /// @endcode
    AraString GetSwpkgManifestPath(AraString const& name, AraString const& version) const;

    /// @brief swcl path
    /// @param fullPath path
    /// @throws no
    /// @return str
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_11121
    /// @needwork = dda
    /// @endcode
    static AraString GetExtractionTargetDir(AraString const& fullPath);

    /// @brief get instance
    /// @throws no
    /// @return instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_11122
    /// @needwork = dda
    /// @endcode
    static Path& GetInstance();

    /// @brief get path settings
    /// @throws no
    /// @return path settings
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_11123
    /// @needwork = dda
    /// @endcode
    PathSettings const& GetPathSettings() const noexcept { return settings_; }

private:
    /// @brief instance
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10418
    /// @trace_id_dd=DD_UCM_11124
    /// @needwork = ad
    /// @endcode
    static std::unique_ptr< Path > s_Instance_;
    /// @brief Path Settings
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_11125
    /// @needwork = dda
    /// @endcode
    PathSettings settings_;
};

/// @brief get global path
/// @throws no
/// @return path
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10406
/// @trace_id_dd=DD_UCM_11126
/// @needwork = dd
/// @endcode
Path& GetPath();

/// @brief get global path settings
/// @throws no
/// @return path settings
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10407
/// @trace_id_dd=DD_UCM_11127
/// @needwork = dd
/// @endcode
Path::PathSettings const& GetPathSettings();

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // ARA_UCM_PKGMGR_COMMON_PATH_H_
