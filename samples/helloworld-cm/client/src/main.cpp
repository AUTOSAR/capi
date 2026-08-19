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

/// ===========================================================================================
///
/// @file       main.cpp
/// @brief
/// @details
/// @date       2022-06-02
/// @author     jiawei
/// @version    1.0.0
///
/// ===========================================================================================

#include <ara/exec/execution_client.h>
#include <ara/log/logger.h>

#include <csignal>
#include <thread>

#include "ara/core/initialization.h"
#include "ara/core/promise.h"
#include "hello/world/cm/servicehelloworld_proxy.h"

namespace {

// Atomic flag for exit after SIGTERM caught
std::atomic_bool continueExecution{true};

void SigTermHandler(int signal)
{
    if (signal == SIGTERM) {
        // set atomic exit flag
        continueExecution = false;
    }
}

bool RegisterSigTermHandler()
{
    struct sigaction sa;
    sa.sa_handler = SigTermHandler;
    sa.sa_flags   = 0;
    sigemptyset(&sa.sa_mask);
    // register signal handler
    if (sigaction(SIGTERM, &sa, NULL) == -1) {
        // Could not register a SIGTERM signal handler
        return false;
    }
    return true;
}

}  // namespace

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    if (!ara::core::Initialize()) {
        return EXIT_FAILURE;
    }

    ara::log::Logger &logger = ara::log::CreateLogger("#COM", "com-clientd", ara::log::LogLevel::kDebug);

    if (!RegisterSigTermHandler()) {
        logger.LogError() << "iSOFT for CAPI: Helloworld-cm-Client Unable to register signal handler";
    }

    {  // auto release
        ara::exec::ExecutionClient{}.ReportExecutionState(ara::exec::ExecutionState::kRunning);
        logger.LogInfo() << "iSOFT for CAPI: Helloworld-cm-Client ReportExecutionState kRunning";

        using Proxy = hello::world::cm::proxy::ServiceHelloWorldProxy;
        ara::core::Promise< Proxy > promise;  // stack var!!
        auto future = promise.get_future();
        Proxy::StartFindService(
            [&promise, &logger](auto handles, auto handler) {
                if (handles.empty())
                    return;
                static bool promise_already_satisfied{false};
                if (promise_already_satisfied)
                    return;
                promise_already_satisfied = true;
                logger.LogInfo() << "iSOFT for CAPI: Helloworld-cm-Client StartFindService CB called";
                promise.set_value(std::move(Proxy::Create(handles[0])).Value());
                Proxy::StopFindService(handler);
            },
            ara::com::InstanceIdentifier::MakeAny());
        uint32_t nLoopCount{0};
        auto proxy = future.get();
        while (continueExecution) {
            std::string stMsg = "Com-Client-Test[";
            stMsg += std::to_string(nLoopCount + 1);
            stMsg += "]";
            logger.LogInfo() << "iSOFT for CAPI: Helloworld-cm-Client [" << nLoopCount << "] call EchoMethod recv echo:"
                             << proxy.EchoMethod(stMsg.c_str()).GetResult().Value().echo;
            nLoopCount += 1;
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    }

    if (!ara::core::Deinitialize()) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
