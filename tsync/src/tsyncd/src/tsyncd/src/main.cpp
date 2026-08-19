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
/// @brief      time synchronization background service program entry
/// @details
/// @date       2023-02-11
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#include <ara/core/initialization.h>
#include <isoft/naicpp/terminating_handler.h>

#include "ara/tsync/internal/aralogadaptor/aralogadaptor.h"
#include "ara/tsync/internal/common.h"
#include "ara/tsync/internal/daemon.h"
#include "ara/tsync/internal/log/logger.h"
namespace ara {
namespace tsync {
namespace internal {

#if ARA_TSYNC_DEBUG_WITHOUT_EM
    #include <isoft/ipccpp/debug.h>

    #include "ara/log/internal/initialization.h"
    #include "ara/tsync/internal/common.h"
    #include "ara/tsync/internal/timebase/proxy/config.h"

std::int32_t InitIpc()
{
    // Initialize ipc
    // Clear the folder used for ipc communication
    if (0 != isoft::ipc::debug::IPCEnvDestroy()) {
        perror("isoft::ipc::debug::IPCEnvDestroy()");
        return ara::tsync::internal::kRET_E1;
    }
    // Create the folder used for ipc communication
    if (0 != isoft::ipc::debug::IPCEnvInit()) {
        perror("isoft::ipc::debug::IPCEnvInit()");
        return ara::tsync::internal::kRET_E2;
    }

    // Create ipc
    if (0 > isoft::ipc::debug::CreateIPC(ara::tsync::internal::timebase::proxy::kDaemonName)) {
        perror("isoft::ipc::debug::CreateIPC()");
        return ara::tsync::internal::kRET_E3;
    }

    return 0;
}

std::int32_t DestroyIpc()
{
    // Clear ipc
    if (0 > isoft::ipc::debug::DestroyIPC(ara::tsync::internal::timebase::proxy::kDaemonName)) {
        perror("isoft::ipc::debug::DestroyIPC()");
        return ara::tsync::internal::kRET_E3;
    }
    // Clear the folder used for ipc communication
    if (0 != isoft::ipc::debug::IPCEnvDestroy()) {
        perror("isoft::ipc::debug::IPCEnvDestroy()");
        return ara::tsync::internal::kRET_E1;
    }
    return 0;
}
#endif
/// @brief help message prompt
static void Help() noexcept
{
    std::cout << "Usage:" << std::endl;
    std::cout << "emd [OPTION] [OPTARG]" << std::endl;
    std::cout << std::endl;
    std::cout << "Option:" << std::endl;
    std::cout << "OPT     ARG              COMMENT" << std::endl;
    std::cout << " -a     None             allow provider to send sync/followup "
                 "packet without setting time, by "
                 "default, it is disabled."
              << std::endl;
}

}  // namespace internal
}  // namespace tsync
}  // namespace ara

/// @brief daemon entry function
/// @param argc - number of parameters
/// @param argv - parameter values
/// @return 0 execution success, others failure
std::int32_t main(std::int32_t const argc, char **const argv) noexcept
{
    bool bAllowDisPach{false};
    bool bInvalidPara{false};
    std::int32_t opt{0};
    do {
        opt = getopt(argc, argv, "a");  // NOLINT
        if (-1 != opt) {
            if (opt == 'a') {
                bAllowDisPach = true;
            } else {
                bInvalidPara = true;
            }
        }
        if (bInvalidPara) {
            break;
        }
    } while (-1 != opt);
    if (bInvalidPara) {
        ara::tsync::internal::Help();
        return (EXIT_FAILURE);
    }
    static_cast< void >(
        isoft::naicpp::GlobalGeneralEvLoop::Initialize(isoft::naicpp::GlobalGeneralEvLoop::Mode::kUserThread));

    ara::tsync::internal::tslog::Logger::SetOutputHandler(ara::tsync::internal::tsaralog::AraLogAdaptor::Log2AraLog);

#if ARA_TSYNC_DEBUG_WITHOUT_EM
    // ara::log::LogLevel const logLevel = ara::log::LogLevel::kVerbose;.
    // ara::log::LogMode const logMode = static_cast<ara::log::LogMode>(.
    // static_cast<uint8_t>(ara::log::LogMode::kConsole) |
    // static_cast<uint8_t>(ara::log::LogMode::kRemote));.
    ara::log::internal::Initialize();
    if (0 != ara::tsync::internal::InitIpc()) {
        return (EXIT_FAILURE);
    }
#else
    if (!ara::core::Initialize().HasValue()) {
        return (EXIT_FAILURE);
    }
#endif
    {
        isoft::naicpp::TerminatingHandler termHandler{};
        if (0 != termHandler.Register(std::shared_ptr< isoft::naicpp::EvLoop >{nullptr})) {
            std::cout << "termHandler.Register(): " << ara::tsync::internal::GetErrString() << std::endl;
            return (EXIT_FAILURE);
        }
        ara::tsync::internal::Daemon tsyncDaemon{};
        if (0 == tsyncDaemon.Init(bAllowDisPach)) {
            tsyncDaemon.Run();
        }
        tsyncDaemon.Destroy();
    }

#if ARA_TSYNC_DEBUG_WITHOUT_EM
    std::ignore = ara::tsync::internal::DestroyIpc();
#else
    if (!ara::core::Deinitialize().HasValue()) {
        return EXIT_FAILURE;
    }
#endif

    return 0;
}
