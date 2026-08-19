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
/// @file       daemon.cpp
/// @brief      NM service process management
/// @details
/// @date       2022-06-10
/// @author     hejunwei
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @trace_id_sr=SRS_NM_00001
/// @unit_name=Daemon
/// @unit_description=NM service process management
/// @module_path=/NetworkManager/nmm
/// @interface_level=unit
/// @endcode
///
/// ================================================================

#if ARA_NM_WITH_EXEC
    ///
    /// ================================================================
    #include <ara/exec/execution_client.h>
#endif
#include <isoft/naicpp/terminating_handler.h>

#include "include/daemon.h"

namespace ara {
namespace nm {
namespace internal {

/// @brief run of Daemon
void Daemon::Run() noexcept
{
    mainLoop_ = isoft::naicpp::GlobalGeneralEvLoop::Get();

    if (0 != udpNm_.Open()) {
        NmLogger().LogError() << "nmMgr.Open() error";
        return;
    }
    std::function< void() > terminateCallBack{[this]() noexcept -> void { udpNm_.Close(); }};
    isoft::naicpp::TerminatingHandler termHandler{std::move(terminateCallBack)};
    if (0 != termHandler.Register(mainLoop_)) {
        NmLogger().LogError() << "termHandler.Register() failed: ";
        return;
    }
#if ARA_NM_WITH_EXEC
    {
        ara::exec::ExecutionClient const execClient;
        NmLogger().LogDebug() << "ReportExecutionState kRunning";
        static_cast< void >(execClient.ReportExecutionState(ara::exec::ExecutionState::kRunning));
    }
#endif
    static_cast< void >(mainLoop_->Run(true));
}

}  // namespace internal
}  // namespace nm
}  // namespace ara
