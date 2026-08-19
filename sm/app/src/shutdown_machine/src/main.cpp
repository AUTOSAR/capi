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
/// @brief      Shutdown application, implements shutting down the AP platform;
/// @details    The default implementation sends a SIGTERM signal to Execution Management;
/// @details    Users can re-implement this application to shut down the OS;
/// @date       2024-06-25
/// @author     zhibo.han
/// @version    1.2.0
///
/// ================================================================

#include <ara/core/initialization.h>
#include <ara/exec/execution_client.h>
#include <ara/log/logger.h>
#include <unistd.h>

#include <csignal>
#include <iostream>

#include "function_thread_safe.h"

// Atomic flag for exit after SIGTERM caught
std::atomic_bool g_ContinueExecution{true};

void SigTermHandler(int signal)
{
    if (signal == SIGTERM) {
        // set atomic exit flag
        g_ContinueExecution = false;
    }
}

bool RegisterSigTermHandler()
{
    struct sigaction sa
    {
    };
    sa.sa_handler = SigTermHandler;
    sa.sa_flags   = 0;
    sigemptyset(&sa.sa_mask);
    // register signal handler
    if (sigaction(SIGTERM, &sa, nullptr) == -1) {
        // Could not register a SIGTERM signal handler
        return false;
    }
    return true;
}

int main(int argc, char** argv)
{
    (void)argc;
    (void)argv;

    // Do not buffer output
    setvbuf(stdout, nullptr, _IONBF, 0);

    if (!ara::core::Initialize()) {
        return EXIT_FAILURE;
    }

    int ret{0};
    {
        // Register exit signal handling
        if (!RegisterSigTermHandler()) {
            std::cerr << "Unable to register signal handler" << std::endl;
        }

        // ara::exec::ExecutionClient{}.ReportExecutionState(ara::exec::ExecutionState::kRunning);
        // logger.LogInfo() << "ReportExecutionState kRunning";

        std::cout << "Start shutting down..." << std::endl;

        // Parent process ID
        pid_t emdPid = getppid();

        // Send SIGTERM signal to emd process
        std::cout << "Try to send SIGTERM to emdPid:" << emdPid << std::endl;
        ret = kill(emdPid, SIGTERM);
        if (0 != ret) {  // Whether successful
            int error = errno;
            std::cerr << "Kill failed with error:" << ara::core::internal::safe_strerror(error) << std::endl;
        }

        std::cout << "Shutting down end." << std::endl;
    }

    if (!ara::core::Deinitialize()) {
        return EXIT_FAILURE;
    }

    return ret;
}
