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
/// @file       files_node.h
/// @brief      files node
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
/// @unit_name=FilesNode
/// @unit_description=Parse files
/// @endcode
///
/// ================================================================

#ifndef ARA_UCM_PKGMGR_PARSING_FILES_NODE_H_
#define ARA_UCM_PKGMGR_PARSING_FILES_NODE_H_

#include "common/alias.h"
#include "common/rjson_manifest.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief parse files
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10257
/// @trace_id_dd=DD_UCM_10443
/// @needwork = ad
/// @endcode
class FilesNode
{
public:
    /// @brief ParseFileList
    /// @param node
    /// @param key
    /// @param files
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10444
    /// @needwork = dda
    /// @endcode
    void ParseFileList(RManifestNode const& node,
                       AraStringView const& key,
                       AraMap< AraString, AraString >& files) const noexcept
    {
        AraString const value{RManifestNodeGetSS(node, "")};
        if (!value.empty()) {
            files[key.data()] = value;
            return;
        }

        AraString stNextKey{key};
        if (!stNextKey.empty()) {
            stNextKey += "/";
        }
        std::function< void(AraStringView const& memberName, RManifestNode const& subNode) > caller;
        caller = [this, &stNextKey, &files](AraStringView const& memberName, RManifestNode const& subNode) {
            ParseFileList(subNode, stNextKey + memberName.data(), files);
        };
        std::ignore = node.IterateObject(std::move(AraStringView("")), caller);
    }

    /// @brief ManifestLoader
    /// @param node
    /// @throws no
    /// @return result of load
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10445
    /// @needwork = dda
    /// @endcode
    int32_t ManifestLoader(RManifestNode const& node)
    {
        ParseFileList(node, "", files_);
        return 0;
    }

    /// @brief GetFiles
    /// @throws no
    /// @return files_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10446
    /// @needwork = dda
    /// @endcode
    AraMap< AraString, AraString > const& GetFiles() const noexcept { return files_; }

private:
    /// @brief Files
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10447
    /// @needwork = dda
    /// @endcode
    AraMap< AraString, AraString > files_{};
};

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // ARA_UCM_PKGMGR_PARSING_FILES_NODE_H_
