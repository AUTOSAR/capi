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
/// @file       help_executor.h
/// @brief      HelpExecutor header
/// @details
/// @date       2024-08-15
/// @author     cuiyinli
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @export_level=/UCM/FsmManager
/// @module_path=/UCM/FsmManager
/// @interface_level=unit
/// @trace_id_sr=SR_UCM_00001
/// @unit_name=HelpExecutor
/// @unit_description=HelpExecutor header
/// @endcode
///
/// ================================================================

#ifndef PACKAGE_MANAGER_HELP_EXECUTOR_H
#define PACKAGE_MANAGER_HELP_EXECUTOR_H

#include "base_executor.h"
#include "common/alias.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief SoftwareCluster
class SoftwareCluster;

/// @brief HelpExecutor
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10389
/// @trace_id_dd=DD_UCM_10901
/// @needwork = ad
/// @endcode
class HelpExecutor : public BaseExecutor
{
public:
    /// @brief OSUpdateType
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10389
    /// @trace_id_dd=DD_UCM_10902
    /// @needwork = dd
    /// @endcode
    enum class OSUpdateType : int8_t
    {
        kOSActivate          = 0,
        kOSVerify            = 1,
        kOSRollback          = 2,
        kOSExceptionRollback = 3
    };

    /// @brief SwclUpdateType
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10389
    /// @trace_id_dd=DD_UCM_10903
    /// @needwork = dd
    /// @endcode
    enum class SwclUpdateType : int8_t
    {
        kSwclActivate = 0,
        kSwclVerify   = 1,
        kSwclRollback = 2
    };

public:
    /// @brief constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10904
    /// @needwork = dda
    /// @endcode
    HelpExecutor() = default;
    /// @brief destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10905
    /// @needwork = dda
    /// @endcode
    ~HelpExecutor() override = default;

    /// @brief delete copy construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10906
    /// @needwork = dda
    /// @endcode
    HelpExecutor(HelpExecutor const& other) = delete;
    /// @brief delete copy asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10907
    /// @needwork = dda
    /// @endcode
    HelpExecutor& operator=(HelpExecutor const& other) = delete;
    /// @brief delete move construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10908
    /// @needwork = dda
    /// @endcode
    HelpExecutor(HelpExecutor&& other) = delete;
    /// @brief delete move asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10909
    /// @needwork = dda
    /// @endcode
    HelpExecutor& operator=(HelpExecutor&& other) = delete;

    /// @brief RebootOrReparse
    /// @param type
    /// @return result
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10910
    /// @needwork = dda
    /// @endcode
    AraResultVoid RebootOrReparse(HelpExecutor::SwclUpdateType const type) const noexcept;

    /// @brief OSUpdateOperate
    /// @param type
    /// @return result
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10911
    /// @needwork = dda
    /// @endcode
    static AraResultVoid OSUpdateOperate(OSUpdateType const type) noexcept;

    /// @brief SwclFGRemoveMachineFG
    /// @param fgList
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10912
    /// @needwork = dda
    /// @endcode
    static void SwclFGRemoveMachineFG(AraVectorString& fgList) noexcept;
    /// @brief SwclFGChange
    /// @param type
    /// @return result
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10913
    /// @needwork = dda
    /// @endcode
    AraResultVoid SwclFGChange(SwclUpdateType const type) const noexcept;

    /// @brief GetAllFG
    /// @return vector of string
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10914
    /// @needwork = dda
    /// @endcode
    AraVectorString GetAllFG() noexcept;

private:
    /// @brief _getEnableSwcls
    /// @param disableSwcls
    /// @param enableSwcls
    /// @param type
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10915
    /// @needwork = dda
    /// @endcode
    void _getEnableSwcls(AraList< SwClusterInfoType >& disableSwcls,
                         AraList< SwClusterInfoType >& enableSwcls,
                         SwclUpdateType const type) const noexcept;

    /// @brief _swclFGMatch
    /// @param swcl
    /// @param type
    /// @return bool
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10916
    /// @needwork = dda
    /// @endcode
    bool _swclFGMatch(SoftwareCluster const& swcl, SwclUpdateType const type) const noexcept;
};

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // PACKAGE_MANAGER_HELP_EXECUTOR_H
