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
/// @file       lnconfig.h
/// @brief      Configuration file management
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
/// @unit_name=LnConfig
/// @unit_description=Configuration file management
/// @endcode
///
/// ================================================================
///
/// {.
/// "ProcessLnMap": [.
/// {.
/// "instanceSpecifier":
/// "tsync_demo_provider_exe/TsyncProviderSwc/pPortPrototype_0".
/// "lnName":
/// "ISOFT/Development/Machine1/timeSyncModuleInstantiation_0/synchronizedTimeBaseProvider_0".
/// },.
/// {.
/// "instanceSpecifier":
/// "tsync_demo_provider_exe/TsyncProviderSwc/pOffsetPortPrototype_0",.
/// "lnName":
/// "ISOFT/Development/Machine1/timeSyncModuleInstantiation_0/offsetTimeBaseProvider_0".
/// }.
/// ].
/// }.
///
/// ================================================================

#ifndef ARA_NM_INTERNAL_LNCONFIG_H_
#define ARA_NM_INTERNAL_LNCONFIG_H_

#include <ara/core/string.h>
#include <ara/core/string_view.h>
#include <ara/core/vector.h>
#include <isoft/manifestreader/manifest_node.h>
#include <isoft/manifestreader/manifest_reader.h>

#include <cstdint>

#include "lnitem.h"

namespace ara {
namespace nm {

/// @brief Instance descriptor set
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100003
/// @trace_id_dd=DD_NM_00874
/// @needwork = ad
/// @endcode
class LnConfig final
{
public:
    /// @brief Default constructor
    /// @exception on overflow
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00041
    /// @needwork = dda
    /// @endcode
    LnConfig() = default;

    /// @brief Default destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00042
    /// @needwork = dda
    /// @endcode
    ~LnConfig() = default;

    /// @brief Move constructor
    /// @param other - Other object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00043
    /// @needwork = dda
    /// @endcode
    LnConfig(LnConfig &&other) = default;

    /// @brief Copy constructor is prohibited
    /// @param other - Other object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00044
    /// @needwork = dda
    /// @endcode
    LnConfig(LnConfig const &other) = delete;

    /// @brief    Copy is prohibited
    /// @param other - Other object
    /// @return Class object reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00045
    /// @needwork = dda
    /// @endcode
    LnConfig &operator=(LnConfig const &other) = delete;

    /// @brief    '=' operator overloading
    /// @param other An existing rvalue reference.
    /// @return Class object reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00046
    /// @needwork = dda
    /// @endcode
    LnConfig &operator=(LnConfig &&other) &noexcept = default;

    /// @brief Load configuration
    /// @param manifestPath - Configuration file path
    /// @return 0 - Success
    /// @return <0 - Failure
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00047
    /// @needwork = dda
    /// @endcode
    std::int32_t Load(ara::core::String const &manifestPath) noexcept
    {
        ara::core::StringView const path{manifestPath.c_str(), manifestPath.length()};
        ara::core::Result< std::unique_ptr< isoft::manifestreader::Manifest > > manifestRes{
            isoft::manifestreader::OpenManifest(path)};
        if (!manifestRes.HasValue()) {
            return -1;
        }
        std::unique_ptr< isoft::manifestreader::Manifest > const manifest{std::move(manifestRes).Value()};
        return manifest->Load(kProcessLnMap_, instanceSpecifierSet_);
    }

    /// @brief Get the mapped logical network name based on the instance descriptor
    /// @param instanceSpecifier - Instance descriptor
    /// @return Logical network name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00048
    /// @needwork = dda
    /// @endcode
    ara::core::StringView GetLnName(ara::core::String const &instanceSpecifier) const noexcept
    {
        ara::core::StringView sv;
        for (ara::core::Vector< LnItem >::const_iterator itm{instanceSpecifierSet_.cbegin()};
             itm != instanceSpecifierSet_.cend(); ++itm) {
            if (instanceSpecifier == itm->GetInstanceSpecifier()) {
                ara::core::String const &lnName{itm->GetLnName()};
                sv = ara::core::StringView{lnName.data(), lnName.size()};
                break;
            }
        }
        return sv;
    }

private:
    /// @brief kProcessLnMap_
    /// @code{.isoft}
    /////////////// Key ////////////////
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00049
    /// @needwork = dda
    /// @endcode
    ara::core::StringView kProcessLnMap_{"ProcessLnMap"};

    /// @brief instanceSpecifierSet_  Instance descriptor set
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00977
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< LnItem > instanceSpecifierSet_{};

};  /// class LnConfig

}  // namespace nm
}  // namespace ara

#endif  /// ARA_NM_INTERNAL_LNCONFIG_H_
