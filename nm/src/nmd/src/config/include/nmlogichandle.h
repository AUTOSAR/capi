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
/// @file       nmlogichandle.h
/// @brief      Network handle configuration
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
/// @unit_name=NmLogicHandleConfig
/// @unit_description=Network handle configuration
/// @endcode
///
/// ================================================================

#ifndef _ARA_NM_CONFIGURE_NMLOGICHANDLE_H_
#define _ARA_NM_CONFIGURE_NMLOGICHANDLE_H_
#include <ara/core/string.h>
#include <ara/core/vector.h>
#include <isoft/define.h>
#include <isoft/manifestreader/manifest_node.h>
#include <isoft/manifestreader/manifest_reader.h>

#include "etherreference.h"
#include "pnreference.h"

namespace ara {
namespace nm {
namespace internal {

/// @brief logic network configuration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100055
/// @trace_id_dd=DD_NM_00794
/// @needwork = ad
/// @endcode
class NmLogicHandleConfig final
{
public:
    /// @brief constructor of EtherReferenceConfig.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00341
    /// @needwork = dda
    /// @endcode
    NmLogicHandleConfig() = default;

    /// @brief copy constructor is default.
    /// @param other class instance.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00342
    /// @needwork = dda
    /// @endcode
    NmLogicHandleConfig(NmLogicHandleConfig const &other) = default;

    /// @brief copy operator is default.
    /// @param other class instance.
    /// @returns class instance reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00343
    /// @needwork = dda
    /// @endcode
    NmLogicHandleConfig &operator=(NmLogicHandleConfig const &other) = default;

    /// @brief move constructor is default
    /// @param other class instance.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00344
    /// @needwork = dda
    /// @endcode
    NmLogicHandleConfig(NmLogicHandleConfig &&other) = default;

    /// @brief move operator is default.
    /// @param other class instance.
    /// @returns class instance reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00345
    /// @needwork = dda
    /// @endcode
    NmLogicHandleConfig &operator=(NmLogicHandleConfig &&other) = default;

    /// @brief Destructor of NmLogicHandleConfig.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00346
    /// @needwork = dda
    /// @endcode
    ~NmLogicHandleConfig() = default;

    /// @brief logic network configuration
    /// @param node logic network configuration node
    /// @returns 0 ok
    /// @code{.isoft}
    /// @trace_id_sws= {SWS_ANM_00066}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00347
    /// @needwork = dda
    /// @endcode
    std::int32_t ManifestLoader(isoft::manifestreader::ManifestNode const &node) noexcept
    {
        this->handleIndex_ = node.GetValue(std::move(ara::core::StringView("nmlogichandleindex")), ara::core::String());
        static_cast< void >(node.Load(std::move(ara::core::StringView("partialnetworkarray")), this->pncRefVec_));
        static_cast< void >(node.Load(std::move(ara::core::StringView("ethernetconnectorarray")), this->etherRefVec_));
        this->lnInstanceSpecifier_
            = node.GetValue(std::move(ara::core::StringView("lninstancespecifier")), ara::core::String());
        return 0;
    }

    /// @brief Get logical network name
    /// @returns Logical network name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00348
    /// @needwork = dda
    /// @endcode
    ara::core::String GetHandleIndex() const noexcept { return handleIndex_; }

    /// @brief get pn reference vector
    /// @returns pn reference vector
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00349
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< PnReferenceConfig > const &GetPncRefVec() const noexcept { return pncRefVec_; }

    /// @brief get ethernet connector reference vector
    /// @returns ethernet connector reference vector
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00350
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< EtherReferenceConfig > const &GetEtherRefVec() const noexcept { return etherRefVec_; }

    /// @brief get logic network service instance specifier
    /// @returns logic network service instance specifier
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00351
    /// @needwork = dda
    /// @endcode
    ara::core::String GetLnInstanceSpecifier() const noexcept { return lnInstanceSpecifier_; }

#if ARA_NM_DEBUG
    /// @brief Print debug information
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00352
    /// @needwork = dda
    /// @endcode
    void Debug() const noexcept
    {
        std::cout << "nmlogichandleindex: " << handleIndex_ << std::endl;
        std::cout << "partialnetworkarray: [" << std::endl;
        for (ara::core::Vector< PnReferenceConfig >::const_iterator itPn{pncRefVec_.begin()}; pncRefVec_.end() != itPn;
             ++itPn) {
            itPn->Debug();
        }
        std::cout << "]" << std::endl;
        std::cout << "ethernetconnectorarray: [" << std::endl;
        for (ara::core::Vector< EtherReferenceConfig >::const_iterator itEther{etherRefVec_.begin()};
             etherRefVec_.end() != itEther; ++itEther) {
            itEther->Debug();
        }
        std::cout << "]" << std::endl;
        std::cout << "lninstancespecifier: " << lnInstanceSpecifier_ << std::endl;
    }
#endif

private:
    /// @brief handleIndex_
    /// logic network name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00353
    /// @needwork = dda
    /// @endcode
    ara::core::String handleIndex_;

    /// @brief pncRefVec_
    /// pn-Reference in logic network
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00354
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< PnReferenceConfig > pncRefVec_;

    /// @brief etherRefVec_
    /// ethernet-Reference in logic network
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00355
    /// @needwork = dda
    /// @endcode
    ara::core::Vector< EtherReferenceConfig > etherRefVec_;

    /// @brief lnInstanceSpecifier_
    /// Specifier of someip-skeleton
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_NM_00000
    /// @trace_id_dd=DD_NM_00356
    /// @needwork = dda
    /// @endcode
    ara::core::String lnInstanceSpecifier_;
};

}  // namespace internal
}  // namespace nm
}  // namespace ara
#endif /* _ARA_NM_CONFIGURE_NMLOGICHANDLE_H_ */
