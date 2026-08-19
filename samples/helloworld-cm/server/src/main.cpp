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

#include <cassert>
#include <csignal>
#include <thread>

#include "ara/core/initialization.h"
#include "hello/world/cm/servicehelloworld_skeleton.h"
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

class ServiceHelloWorldImp : public hello::world::cm::skeleton::ServiceHelloWorldSkeleton
{
public:
    using Skeleton = hello::world::cm::skeleton::ServiceHelloWorldSkeleton;

    ServiceHelloWorldImp(ara::core::InstanceSpecifier instanceSpec, ara::com::MethodCallProcessingMode mode)
        : Skeleton(std::move(instanceSpec), mode)
    {
    }

    virtual ara::core::Future< hello::world::cm::ServiceHelloWorld::EchoMethodOutput > EchoMethod(const ::String &msg)
    {
        ara::log::Logger &logger = ara::log::CreateLogger("#COM", "helloworld-cm-server", ara::log::LogLevel::kDebug);
        logger.LogInfo() << "iSOFT for CAPI: EchoMethod call from client :" << msg;

        // event send
        auto data = std::move(testEvent.Allocate()).Value();
        data->push_back('a');
        data->push_back('b');
        data->push_back('c');
        testEvent.Send(std::move(data));

        hello::world::cm::ServiceHelloWorld::EchoMethodOutput output;
        output.echo = " From service iSOFT for CAPI:" + msg;
        ara::core::Promise< hello::world::cm::ServiceHelloWorld::EchoMethodOutput > promise;
        promise.set_value(std::move(output));
        return promise.get_future();
    }
};

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    if (!ara::core::Initialize()) {
        return EXIT_FAILURE;
    }

    ara::log::Logger &logger = ara::log::CreateLogger("#COM", "com-serverd", ara::log::LogLevel::kDebug);

    if (!RegisterSigTermHandler()) {
        logger.LogError() << "iSOFT for CAPI: Unable to register signal handler";
    }

    {  // auto release
        ara::core::InstanceSpecifier instanceSpec{"serverd/server/helloworld_PPort"};
        auto skeleton = ServiceHelloWorldImp::Create< ServiceHelloWorldImp >(instanceSpec).Value();
        skeleton.OfferService();

        ara::exec::ExecutionClient{}.ReportExecutionState(ara::exec::ExecutionState::kRunning);
        logger.LogInfo() << "iSOFT for CAPI: ReportExecutionState kRunning";

        while (continueExecution) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
        skeleton.StopOfferService();
    }
    if (!ara::core::Deinitialize()) {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
