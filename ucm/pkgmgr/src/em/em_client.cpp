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
/// @file       em_client.cpp
/// @brief      em client implementation
/// @details
/// @date       2024-08-15
/// @author     cuiyinli
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/UCM/FsmManager
/// @interface_level=unit
/// @trace_id_sr=SR_UCM_00001
/// @unit_name=ExecClient
/// @unit_description=em client implementation
/// @endcode
///
/// ================================================================

#include "em_client.h"

#include "ara/exec/execution_client.h"
#include "ara/exec/internal/ums/swcl_info.h"
#include "ara/exec/internal/update_client.h"
#include "common/log.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief ExecutionClient
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
using ExecutionClient = ara::exec::ExecutionClient;
/// @brief ExecutionState
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
using ExecutionState = ara::exec::ExecutionState;
/// @brief UpdateClient
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
using UpdateClient = ara::exec::internal::UpdateClient;
/// @brief SwclInfoShort
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
using SwclInfoShort = ara::exec::internal::ums::SwclInfo;
/// @brief SwclInfoList
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_
/// @trace_id_dd=DD_UCM_
/// @needwork = no
/// @endcode
using SwclInfoList = AraVector< SwclInfoShort >;

/// @brief report exec state with running
/// @throws no
/// @return no
void ExecClient::ReportExecutionStateRunning() noexcept
{
    AraResultVoid const ret{ExecutionClient().ReportExecutionState(ExecutionState::kRunning)};
    if (!ret.HasValue()) {
        LOGD << "failed, errc:" << ret.Error().Message().data();
    }
}

/// @brief get current user swcl manifest
/// @throws no
/// @return str
AraString ExecClient::GetCurrentUserSWCLManifest() noexcept
{
    LOGI << "call...";

    ///AraResult<AraString> const ret{
    ///    std::move(UpdateClient().GetCurrentUserSWCLManifest().GetResult())};.//mydel////
    AraResult< AraString > const ret{UpdateClient().GetUserSwclManifest().GetResult()};
    if (!ret.HasValue()) {
        LOGD << "failed, errc:" << ret.Error().Message().data();
        return AraString();
    }

    return ret.Value();
}

/// @brief delete move asign
/// @param disableSwcls disabled swcl list
/// @param enableSwcls enabled swcl list
/// @throws no
/// @return bool
bool ExecClient::ReparseProcessList(AraList< SwClusterInfoType > const& disableSwcls,
                                    AraList< SwClusterInfoType > const& enableSwcls) noexcept
{
    LOGI << "call...";

    SwclInfoList disabledSwcls;
    SwclInfoList enabledSwcls;
    for (SwClusterInfoType const& info : disableSwcls) {
        disabledSwcls.emplace_back(SwclInfoShort{info.Name, info.Version});
    }
    for (SwClusterInfoType const& info : enableSwcls) {
        enabledSwcls.emplace_back(SwclInfoShort{info.Name, info.Version});
    }

    ///AraResultVoid const ret{
    ///    std::move(UpdateClient().ReparseProcessList().GetResult())};.//mydel////
    AraResultVoid const ret{UpdateClient().UpdateUserSwcls(disabledSwcls, enabledSwcls).GetResult()};
    if (!ret.HasValue()) {
        LOGD << "failed, errc:" << ret.Error().Message().data();
        return false;
    }

    return true;
}

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara
