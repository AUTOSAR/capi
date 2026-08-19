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
/// @file       main.cpp
/// @brief
/// @details
/// @date       2021-12-31
/// @author     mazelin
/// @version    1.2.0
///
/// ================================================================

#include <cassert>
#include <csignal>
#include <thread>

#include "ara/com/internal/cg/cg_instance.h"
#include "ara/com/internal/cg/cg_manager.h"
#include "ara/com/internal/cg/cg_runtime.h"
#include "ara/com/internal/cg/impl_communication_group.h"
#include "ara/core/initialization.h"
#ifdef HAS_ARA_EXEC_EXECUTION_CLIENT
    #include "ara/exec/execution_client.h"
#else
#endif  // HAS_ARA_EXEC_EXECUTION_CLIENT

// Atomic flag for exit after SIGTERM caught
std::atomic_bool g_ContinueExecution{true};

void SigTermHandler(int signal) noexcept
{
    if (signal == SIGTERM) {
        // set atomic exit flag
        g_ContinueExecution = false;
    }
}

bool RegisterSigTermHandler() noexcept
{
    struct sigaction sa
    {
    };
    sa.sa_handler = SigTermHandler;
    sa.sa_flags   = 0;
    std::ignore   = sigemptyset(&sa.sa_mask);
    // register signal handler
    if (sigaction(SIGTERM, &sa, nullptr) == -1) {
        // Could not register a SIGTERM signal handler
        return false;
    }
    return true;
}

int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;
    if (!ara::core::Initialize()) {
        return EXIT_FAILURE;
    }
    {
        if (!RegisterSigTermHandler()) {
            ComLogError("Unable to register signal handler");
        }
#ifdef HAS_ARA_EXEC_EXECUTION_CLIENT
        ara::exec::ExecutionClient{}.ReportExecutionState(ara::exec::ExecutionState::kRunning);
        ComLogInfo("report execution state running");
#else
#endif  // HAS_ARA_EXEC_EXECUTION_CLIENT
        using CommunicationGroupMgr = ara::com::internal::cg::CommunicationGroupMgr;
        CommunicationGroupMgr::StartCommunicationGroupService();
        constexpr auto kMainLoopInterval{std::chrono::milliseconds(100)};
        while (g_ContinueExecution) {
            std::this_thread::sleep_for(kMainLoopInterval);
        }
        CommunicationGroupMgr::StopCommunicationGroupService();
    }
    if (!ara::core::Deinitialize()) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}