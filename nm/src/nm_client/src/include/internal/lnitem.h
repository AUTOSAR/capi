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
/// @file       lnitem.h
/// @brief      Configuration item management
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
/// @unit_name=LnItem
/// @unit_description=Configuration item management
/// @endcode
///
/// ================================================================

#ifndef ARA_NM_INTERNAL_LNITEM_H_
#define ARA_NM_INTERNAL_LNITEM_H_

#include <ara/core/string.h>
#include <ara/core/string_view.h>
#include <ara/core/vector.h>
#include <isoft/manifestreader/manifest_node.h>
#include <isoft/manifestreader/manifest_reader.h>

#include <cstdint>

namespace ara {
namespace nm {

/// @brief Instance descriptor
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100000
/// @trace_id_dd=DD_NM_00870
/// @needwork = ad
/// @endcode
class LnItem final
{
public:
    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00002
    /// @needwork = dda
    /// @endcode
    LnItem() = default;

    /// @brief Default destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00003
    /// @needwork = dda
    /// @endcode
    ~LnItem() = default;

    /// @brief Default move constructor
    /// @param other - Object reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00004
    /// @needwork = dda
    /// @endcode
    LnItem(LnItem &&other) = default;

    /// @brief Default copy constructor
    /// @param other - Object reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00005
    /// @needwork = dda
    /// @endcode
    LnItem(LnItem const &other) = default;

    /// @brief Copy assignment
    /// @param other - Object reference
    /// @return Self reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00006
    /// @needwork = dda
    /// @endcode
    LnItem &operator=(LnItem const &other) & = default;

    /// @brief Move assignment
    /// @param other - Object reference
    /// @return Self reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00007
    /// @needwork = dda
    /// @endcode
    LnItem &operator=(LnItem &&other) = default;

    /// @brief Get instance descriptor
    /// @return Instance descriptor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00008
    /// @needwork = dda
    /// @endcode
    ara::core::String const &GetInstanceSpecifier() const noexcept { return instanceSpecifier_; }

    /// @brief Get logical network name
    /// @return Logical network name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00009
    /// @needwork = dda
    /// @endcode
    ara::core::String const &GetLnName() const noexcept { return lnName_; }

    /// @brief Load function required by manifestReader interface
    /// @param node - manifest node
    /// @return kSucess/ErrorCode
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00010
    /// @needwork = dda
    /// @endcode
    std::int32_t ManifestLoader(isoft::manifestreader::ManifestNode const &node) noexcept
    {
        this->instanceSpecifier_ = node.GetValue(kInstanceSpecifier_, ara::core::String());
        this->lnName_            = node.GetValue(kLnName_, ara::core::String());
        return isoft::kSuccess;
    }

private:
    /// @brief kLnName_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00011
    /// @needwork = dda
    /// @endcode
    ara::core::StringView kLnName_{"lnName"};

    /// @brief kInstanceSpecifier_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00012
    /// @needwork = dda
    /// @endcode
    ara::core::StringView kInstanceSpecifier_{"instanceSpecifier"};

    /// @brief instanceSpecifier_  Instance descriptor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00975
    /// @needwork = dda
    /// @endcode
    ara::core::String instanceSpecifier_{};

    /// @brief lnName_  Logical network name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00976
    /// @needwork = dda
    /// @endcode
    ara::core::String lnName_{};
};

}  // namespace nm
}  // namespace ara

#endif  /// ARA_NM_INTERNAL_LNITEM_H_
