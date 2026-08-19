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
/// @brief      main of PHMD.
/// @details
/// @date       2024-06-05
/// @author     wangyanlong
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/PlatformHealthManagement/PhmContext
/// @unit_description=main of PHMD.
/// @unit_name=main
/// @interface_level=module
/// @endcode
///
/// ================================================================

#include <ara/core/initialization.h>
#include <ara/exec/execution_client.h>
#include <isoft/naicpp/global_evloop.h>
#include <isoft/naicpp/terminating_handler.h>

#include "ara/phm/internal/phm_log.h"
#include "phm_context.h"

namespace ara {
namespace phm {
namespace internal {

/// @brief log exception.
/// @needwork = no
class ExceptionLogger final
{
public:
    static void Log(const char* const what) noexcept
    {
        if (what == nullptr) {
            return;
        }
        std::cout << "phm exceprion:" << what << std::endl;
    }
};
}  // namespace internal
}  // namespace phm
}  // namespace ara

/// @brief main of PHMD
/// @return 0 success; other fail
/// @throws QAC
/// @needwork = no
int32_t main()
{
    try {
        std::cout << "start phmd\n";

        ara::core::Result< void > res{
            isoft::naicpp::GlobalGeneralEvLoop::Initialize(isoft::naicpp::GlobalGeneralEvLoop::Mode::kUserThread)};
        if (!res.HasValue()) {
            std::cout << "init event loop failed.\n";
            return -1;
        }
        std::shared_ptr< isoft::naicpp::EvLoop > mainLoop{isoft::naicpp::GlobalGeneralEvLoop::Get()};

        res = ara::core::Initialize();
        if (!res.HasValue()) {
            std::cout << "core init failed.\n";
            return -1;
        }

        {
            std::unique_ptr< ara::phm::internal::PhmContext > phmContext{
                ara::phm::internal::PhmContext::GetInstanceUnique()};
            if (phmContext == nullptr) {
                LOG_ERROR << "PhmContext is null.";
                return -1;
            }

            isoft::naicpp::TerminatingHandler termHandler{[&phmContext]() {
                LOG_INFO << "phm recv signal term.";
                std::ignore = phmContext->Stop();
                std::cout << "after stop phm.\n";

                std::ignore = phmContext->Destroy();
                std::cout << "after destroy phm.\n";
            }};
            if (0 != termHandler.Register(mainLoop)) {
                std::cout << "termHandler.Register() error!" << std::endl;
                return EXIT_FAILURE;
            }

            if (phmContext->Init() < 0) {
                std::cout << "init phm context failed.\n";
                std::ignore = phmContext->Stop();
                std::ignore = phmContext->Destroy();
                return -1;
            }

            {
                ara::exec::ExecutionClient const execClient;
                ara::core::Result< void > const result{
                    execClient.ReportExecutionState(ara::exec::ExecutionState::kRunning)};
                if (!result.HasValue()) {
                    std::cout << "report running to em failed.\n";
                    std::ignore = phmContext->Stop();
                    std::ignore = phmContext->Destroy();
                    return -1;
                }
            }

            if (phmContext->Run() < 0) {
                std::cout << "run phm context failed.\n";
                std::ignore = phmContext->Stop();
                std::ignore = phmContext->Destroy();
                return -1;
            }
            std::cout << "before destroy phm context" << std::endl;
        }
        std::cout << "phm context destroyed" << std::endl;

        mainLoop.reset();
        if (!ara::core::Deinitialize().HasValue()) {
            std::cout << "deinit core failed.\n";
            return -1;
        }
        return 0;
    } catch (std::exception& e) {
        ara::phm::internal::ExceptionLogger::Log(e.what());
        return -1;
    } catch (...) {
        ara::phm::internal::ExceptionLogger::Log("unknown");
        return -1;
    }
}