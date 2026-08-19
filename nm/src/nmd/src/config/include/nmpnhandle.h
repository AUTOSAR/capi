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
/// @file       nmpnhandle.h
/// @brief      Partial network configuration
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
/// @unit_name=NmPnHandleConfig
/// @unit_description=Partial network configuration
/// @endcode
///
/// ================================================================

#ifndef _ARA_NM_CONFIGURE_NMPNHANDLE_H_
#define _ARA_NM_CONFIGURE_NMPNHANDLE_H_
#include <ara/core/map.h>
#include <ara/core/string.h>
#include <ara/core/vector.h>
#include <isoft/define.h>
#include <isoft/manifestreader/manifest_node.h>
#include <isoft/manifestreader/manifest_reader.h>

#include "etherreference.h"

namespace ara {
namespace nm {
namespace internal {

/// @brief partial network configuration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100050
/// @trace_id_dd=DD_NM_00789
/// @needwork = ad
/// @endcode
class NmPnHandleConfig final
{
public:
    /// @brief constructor of NmPnHandleConfig.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00191
    /// @needwork = dda
    /// @endcode
    NmPnHandleConfig() = default;

    /// @brief copy constructor is default.
    /// @param other class instance.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00192
    /// @needwork = dda
    /// @endcode
    NmPnHandleConfig(NmPnHandleConfig const &other) = default;

    /// @brief copy operator is default.
    /// @param other class instance.
    /// @returns class instance reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00193
    /// @needwork = dda
    /// @endcode
    NmPnHandleConfig &operator=(NmPnHandleConfig const &other) = default;

    /// @brief move constructor is default
    /// @param other class instance.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00194
    /// @needwork = dda
    /// @endcode
    NmPnHandleConfig(NmPnHandleConfig &&other) = default;

    /// @brief move operator is default.
    /// @param other class instance.
    /// @returns class instance reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00195
    /// @needwork = dda
    /// @endcode
    NmPnHandleConfig &operator=(NmPnHandleConfig &&other) = default;

    /// @brief Destructor of NmLogicHandleConfig.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00196
    /// @needwork = dda
    /// @endcode
    ~NmPnHandleConfig() = default;

    /// @brief load partial network configuration
    /// @param node partial network configuration node
    /// @returns kSuccess ok
    /// @code{.isoft}
    /// @trace_id_sws= {SWS_ANM_00067}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00197
    /// @needwork = dda
    /// @endcode
    std::int32_t ManifestLoader(isoft::manifestreader::ManifestNode const &node) noexcept
    {
        this->pnIndex_ = node.GetValue(std::move(ara::core::StringView("pnindex")), ara::core::String());
        this->pnId_
            = static_cast< std::uint16_t >(node.GetValue(std::move(ara::core::StringView("pncidentifier")), 0U));
        return node.Load(std::move(ara::core::StringView("ethernetconnectorarray")), this->ethernetVec_);
    }

    /// @brief get ethernet connector reference vector
    /// @returns ethernet connector reference vector
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00198
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< EtherReferenceConfig > const &GetEthernetVec() const noexcept { return ethernetVec_; }

    /// @brief Get partial network name
    /// @returns Partial network name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00199
    /// @needwork = dda
    /// @endcode
    ara::core::String GetPnIndex() const noexcept { return pnIndex_; }

    /// @brief get partial network id
    /// @returns partial network id
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00200
    /// @needwork = dda
    /// @endcode
    std::uint16_t GetPnId() const noexcept { return pnId_; }

#if ARA_NM_DEBUG
    /// @brief Print debug information
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00201
    /// @needwork = dda
    /// @endcode
    void Debug() const noexcept
    {
        std::cout << "pnindex: " << pnIndex_ << std::endl;
        std::cout << "pncidentifier: " << pnId_ << std::endl;
        std::cout << "ethernetconnectorarray: [" << std::endl;
        for (ara::core::Vector< EtherReferenceConfig >::const_iterator itEther{ethernetVec_.begin()};
             ethernetVec_.end() != itEther; ++itEther) {
            itEther->Debug();
        }
        std::cout << "]" << std::endl;
    }
#endif

private:
    /// @brief pnIndex_
    /// partial network name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00202
    /// @needwork = dda
    /// @endcode
    ara::core::String pnIndex_{};

    /// @brief pnId_
    /// Identifer of the Partial Network Cluster. This number represents the
    /// absolute bit position of this Partial Network Cluster in the NM Pdu
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00203
    /// @needwork = dda
    /// @endcode
    std::uint16_t pnId_{0};

    /// @brief ethernetVec_
    /// ethernet-Reference in partial network
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00204
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< EtherReferenceConfig > ethernetVec_{};
};

}  // namespace internal
}  // namespace nm
}  // namespace ara
#endif /* _ARA_NM_CONFIGURE_NMPNHANDLE_H_ */
