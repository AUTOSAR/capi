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
/// @file       smcheck.h
/// @brief      Process access API legality check
/// @details
/// @date       2022-06-10
/// @author     hejunwei
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/NetworkManager/lib
/// @interface_level=unit
/// @trace_id_sr=SRS_NM_00001
/// @unit_name=SMCheck
/// @unit_description=Process access API legality check
/// @endcode
///
/// ================================================================

#ifndef ARA_NM_INTERNAL_SMCHECK_H_
#define ARA_NM_INTERNAL_SMCHECK_H_

#include <ara/core/string.h>
#include <ara/core/string_view.h>
#include <ara/core/vector.h>
#include <isoft/ara_fsh/filesystem_hierarchy.h>
#include <isoft/manifestreader/manifest_node.h>
#include <isoft/manifestreader/manifest_reader.h>

#include <cstdint>

namespace ara {
namespace nm {

/// @brief SM process check
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100004
/// @trace_id_dd=DD_NM_00875
/// @needwork = ad
/// @endcode
class SMCheck final
{
public:
    /// @brief Default constructor
    /// @exception on overflow
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00052
    /// @needwork = dda
    /// @endcode
    SMCheck() { Load(); };

    /// @brief Default destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00053
    /// @needwork = dda
    /// @endcode
    ~SMCheck() = default;

    /// @brief Move constructor
    /// @param other - Other object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00054
    /// @needwork = dda
    /// @endcode
    SMCheck(SMCheck &&other) = default;

    /// @brief Copy constructor is prohibited
    /// @param other - Other object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00055
    /// @needwork = dda
    /// @endcode
    SMCheck(SMCheck const &other) = delete;

    /// @brief    Copy is prohibited
    /// @param other - Other object
    /// @return Class object reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00056
    /// @needwork = dda
    /// @endcode
    SMCheck &operator=(SMCheck const &other) = delete;

    /// @brief    '=' operator overloading
    /// @param other An existing rvalue reference.
    /// @return Class object reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00057
    /// @needwork = dda
    /// @endcode
    SMCheck &operator=(SMCheck &&other) &noexcept = default;

    /// @brief Load configuration
    /// @return 0 - Success
    /// @return <0 - Failure
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00058
    /// @needwork = dda
    /// @endcode
    void Load() noexcept
    {
        ara::core::String const mapFile{isoft::ara_fsh::Process().GetExecutionManifest()};
        ara::core::StringView const path{mapFile.c_str(), mapFile.length()};
        ara::core::Result< std::unique_ptr< isoft::manifestreader::Manifest > > manifestRes{
            isoft::manifestreader::OpenManifest(path)};
        if (!manifestRes.HasValue()) {
            return;
        }
        std::unique_ptr< isoft::manifestreader::Manifest > const manifest{std::move(manifestRes).Value()};
        this->smFlag_ = manifest->GetValue(kFunctionClusterAffiliation_, ara::core::String(""));
    }

    /// @brief Check whether it is a state management process access
    /// @return true if accessible
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00059
    /// @needwork = dda
    /// @endcode
    static bool CheckAccessPermission() noexcept
    {
        static SMCheck const kCheckObj;
        bool ret{false};
        if ("STATE_MANAGEMENT" == kCheckObj.smFlag_) {
            ret = true;
        }
        return ret;
    }

private:
    /// @brief kFunctionClusterAffiliation_
    /// @code{.isoft}
    /////////////// Key ////////////////
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00060
    /// @needwork = dda
    /// @endcode
    ara::core::StringView kFunctionClusterAffiliation_{"functionClusterAffiliation"};

    /// @brief smFlag_ - SM process flag
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00978
    /// @needwork = dda
    /// @endcode
    ara::core::String smFlag_{};

};  /// class SMCheck

}  // namespace nm
}  // namespace ara

#endif  /// ARA_NM_INTERNAL_SMCHECK_H_
