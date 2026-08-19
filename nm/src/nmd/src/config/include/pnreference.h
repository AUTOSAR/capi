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
/// @file       pnreference.h
/// @brief      Partial network reference
/// @details
/// @date       2022-06-10
/// @author     hejunwei
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/NetworkManager/config
/// @interface_level=Module
/// @trace_id_sr=SRS_NM_00001
/// @unit_name=PnReferenceConfig
/// @unit_description=Partial network reference
/// @endcode
///
/// ================================================================

#ifndef _ARA_NM_CONFIGURE_NMPNREFERENCE_H_
#define _ARA_NM_CONFIGURE_NMPNREFERENCE_H_
#include <ara/core/map.h>
#include <ara/core/string.h>
#include <ara/core/vector.h>
#include <isoft/define.h>
#include <isoft/manifestreader/manifest_node.h>
#include <isoft/manifestreader/manifest_reader.h>

namespace ara {
namespace nm {
namespace internal {

/// @brief Partial network reference, pn-Reference in logic network-handle
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100049
/// @trace_id_dd=DD_NM_00788
/// @needwork = ad
/// @endcode
class PnReferenceConfig final
{
public:
    /// @brief constructor of PnReferenceConfig.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00180
    /// @needwork = dda
    /// @endcode
    PnReferenceConfig() = default;

    /// @brief copy constructor is default.
    /// @param other class instance.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00181
    /// @needwork = dda
    /// @endcode
    PnReferenceConfig(PnReferenceConfig const &other) = default;

    /// @brief copy operator is default.
    /// @param other class instance.
    /// @returns class instance reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00182
    /// @needwork = dda
    /// @endcode
    PnReferenceConfig &operator=(PnReferenceConfig const &other) = default;

    /// @brief move constructor is default
    /// @param other class instance.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00183
    /// @needwork = dda
    /// @endcode
    PnReferenceConfig(PnReferenceConfig &&other) = default;

    /// @brief move operator is default.
    /// @param other class instance.
    /// @returns class instance reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00184
    /// @needwork = dda
    /// @endcode
    PnReferenceConfig &operator=(PnReferenceConfig &&other) = default;

    /// @brief Destructor of PnReferenceConfig.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00185
    /// @needwork = dda
    /// @endcode
    ~PnReferenceConfig() = default;

    /// @brief load PnReferenceConfig-configuration
    /// @param node PnReferenceConfig-configuration node
    /// @returns kSuccess ok
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00186
    /// @needwork = dda
    /// @endcode
    std::int32_t ManifestLoader(isoft::manifestreader::ManifestNode const &node) noexcept
    {
        this->pnIndex_ = node.GetValue(std::move(ara::core::StringView("pnindex")), ara::core::String());
        return isoft::kSuccess;
    }

    /// @brief Get referenced partial network name
    /// @returns Partial network name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00187
    /// @needwork = dda
    /// @endcode
    ara::core::String GetPnIndex() const noexcept { return pnIndex_; }

#if ARA_NM_DEBUG
    /// @brief Print debug information
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00188
    /// @needwork = dda
    /// @endcode
    void Debug() const noexcept { std::cout << "pnindex: " << pnIndex_ << std::endl; }
#endif

private:
    /// @brief pnIndex_
    /// Partial network name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00189
    /// @needwork = dda
    /// @endcode
    ara::core::String pnIndex_;
};

}  // namespace internal
}  // namespace nm
}  // namespace ara
#endif /* _ARA_NM_CONFIGURE_NMPNREFERENCE_H_ */
