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
/// @file       em_client.h
/// @brief      em client
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
/// @unit_name=ExecClient
/// @unit_description=em client
/// @endcode
///
/// ================================================================

#ifndef ARA_UCM_PKGMGR_EM_EXEC_CLIENT_H_
#define ARA_UCM_PKGMGR_EM_EXEC_CLIENT_H_

#include "common/alias.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief em client
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10379
/// @trace_id_dd=DD_UCM_10891
/// @needwork = ad
/// @endcode
class ExecClient
{
public:
    /// @brief constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10380
    /// @trace_id_dd=DD_UCM_10892
    /// @needwork = ad
    /// @endcode
    ExecClient() = delete;
    /// @brief destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10381
    /// @trace_id_dd=DD_UCM_10893
    /// @needwork = ad
    /// @endcode
    ~ExecClient() noexcept = delete;

    /// @brief delete copy construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10382
    /// @trace_id_dd=DD_UCM_10894
    /// @needwork = ad
    /// @endcode
    ExecClient(ExecClient const& other) = delete;
    /// @brief delete copy asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10383
    /// @trace_id_dd=DD_UCM_10895
    /// @needwork = ad
    /// @endcode
    ExecClient& operator=(ExecClient const& other) = delete;
    /// @brief delete move construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10384
    /// @trace_id_dd=DD_UCM_10896
    /// @needwork = ad
    /// @endcode
    ExecClient(ExecClient&& other) = delete;
    /// @brief delete move asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10385
    /// @trace_id_dd=DD_UCM_10897
    /// @needwork = ad
    /// @endcode
    ExecClient& operator=(ExecClient&& other) = delete;

    /// @brief report exec state with running
    /// @throws no
    /// @return no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10386
    /// @trace_id_dd=DD_UCM_10898
    /// @needwork = ad
    /// @endcode
    static void ReportExecutionStateRunning() noexcept;

    /// @brief get current user swcl manifest
    /// @throws no
    /// @return str
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10387
    /// @trace_id_dd=DD_UCM_10899
    /// @needwork = ad
    /// @endcode
    static AraString GetCurrentUserSWCLManifest() noexcept;

    /// @brief delete move asign
    /// @param disableSwcls disabled swcl list
    /// @param enableSwcls enabled swcl list
    /// @throws no
    /// @return bool
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10388
    /// @trace_id_dd=DD_UCM_10900
    /// @trace_id_sr=RS_UCM_00018
    /// @needwork = ad
    /// @endcode
    static bool ReparseProcessList(AraList< SwClusterInfoType > const& disableSwcls,
                                   AraList< SwClusterInfoType > const& enableSwcls) noexcept;
};

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // ARA_UCM_PKGMGR_EM_EXEC_CLIENT_H_
