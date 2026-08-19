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
/// @file       etherreference.h
/// @brief      Connector reference
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
/// @unit_name=EtherReferenceConfig
/// @unit_description=Connector reference
/// @endcode
///
/// ================================================================

#ifndef _ARA_NM_CONFIGURE_ETHERREFERENCE_H_
#define _ARA_NM_CONFIGURE_ETHERREFERENCE_H_
#include <ara/core/map.h>
#include <ara/core/string.h>
#include <ara/core/vector.h>
#include <isoft/define.h>
#include <isoft/manifestreader/manifest_node.h>
#include <isoft/manifestreader/manifest_reader.h>

namespace ara {
namespace nm {
namespace internal {

/// @brief ethernet connector reference in logic network or partial network
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100053
/// @trace_id_dd=DD_NM_00792
/// @needwork = ad
/// @endcode
class EtherReferenceConfig final
{
public:
    /// @brief constructor of EtherReferenceConfig.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00329
    /// @needwork = dda
    /// @endcode
    EtherReferenceConfig() = default;

    /// @brief copy constructor is default.
    /// @param other class instance.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00330
    /// @needwork = dda
    /// @endcode
    EtherReferenceConfig(EtherReferenceConfig const &other) = default;

    /// @brief copy operator is default.
    /// @param other class instance.
    /// @returns class instance reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00331
    /// @needwork = dda
    /// @endcode
    EtherReferenceConfig &operator=(EtherReferenceConfig const &other) = default;

    /// @brief move constructor is default
    /// @param other class instance.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00332
    /// @needwork = dda
    /// @endcode
    EtherReferenceConfig(EtherReferenceConfig &&other) = default;

    /// @brief move operator is default.
    /// @param other class instance.
    /// @returns class instance reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00333
    /// @needwork = dda
    /// @endcode
    EtherReferenceConfig &operator=(EtherReferenceConfig &&other) = default;

    /// @brief Destructor of EtherReferenceConfig.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00334
    /// @needwork = dda
    /// @endcode
    ~EtherReferenceConfig() = default;

    /// @brief load ethernet connector configuration
    /// @param node ethernet connector configuration node
    /// @returns kSuccess ok
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00335
    /// @needwork = dda
    /// @endcode
    std::int32_t ManifestLoader(isoft::manifestreader::ManifestNode const &node) noexcept
    {
        this->etherName_
            = node.GetValue(std::move(ara::core::StringView("ethernetcommunicationconnector")), ara::core::String());
        return isoft::kSuccess;
    }

    /// @brief get ether reference name
    /// @returns etherName_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00336
    /// @needwork = dda
    /// @endcode
    ara::core::String GetEherName() const noexcept { return etherName_; }

#if ARA_NM_DEBUG
    /// @brief Print debug information
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00337
    /// @needwork = dda
    /// @endcode
    void Debug() const noexcept { std::cout << "ethernetcommunicationconnector: " << etherName_ << std::endl; }
#endif

private:
    /// @brief etherName_
    /// ethernet connector name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00338
    /// @needwork = dda
    /// @endcode
    ara::core::String etherName_;
};

}  // namespace internal
}  // namespace nm
}  // namespace ara
#endif /* _ARA_NM_CONFIGURE_ETHERREFERENCE_H_ */
