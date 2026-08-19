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
/// @file       adaptive_application.cpp
/// @brief      Adaptive application implementation
/// @details
/// @date       2024-08-15
/// @author     cuiyinli
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/UCM/PackageManager
/// @interface_level=unit
/// @trace_id_sr=SR_UCM_00001
/// @unit_name=AdaptiveApplicationImpl
/// @unit_description=Adaptive application implementation
/// @endcode
///
/// ================================================================

#include "adaptive_application.h"

#include "common/log.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief A method represents the lifecycle of adaptive application.
/// Shall be called from main() function (return app.Execute())
///
/// @return Execution status
std::int32_t AdaptiveApplication::Execute() noexcept
{
    if (_initialize()) {
        Run();
        _terminate();
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
}

/// @brief Performs at initialize step actions.
/// Calls OnInitialize() which could be reimplemented bu specific application
///
/// @return Initialize success status (Was it successful or not)
bool AdaptiveApplication::_initialize() noexcept
{
    mainLoop_ = isoft::naicpp::GlobalGeneralEvLoop::Get();

    if (!OnInitialize()) {
        return false;
    }

    if (0 != terminatingHandler_.Register()) {
        LOGE << "failed to Register TerminatingHandler";
        return false;
    }

    return true;
}

/// @brief Performs actions that are required during app termination
/// Calls OnTerminate() which could be reimplemented by specific app
void AdaptiveApplication::_terminate() noexcept { OnTerminate(); }

/// @brief A method to be executed at initialize step.
/// Default implementation simply returns true and does nothing
/// Could be reimplemented for the specific application (if needed)
///
/// @return Initialize success status (Was it successful or not)
bool AdaptiveApplication::OnInitialize() noexcept { return true; }

/// @brief A method to be executed at terminate step.
/// Default implementation simply does nothing
/// Could be reimplemented for the specific application (if needed)
void AdaptiveApplication::OnTerminate() noexcept {}

/// @brief Waiting until SIG_TERM will be received.
/// Could be called from Run() method for specific application
void AdaptiveApplication::_WaitUntilTermination() noexcept
{
    LOGI << "begin...";

    std::int32_t const ret{mainLoop_->Run(true)};
    mainLoop_ = nullptr;

    LOGI << "end with ret:" << ret;
}

/// @brief run a function with evloop
/// @param fun main process
/// @return Execution status
std::int32_t AdaptiveApplication::_Add2EvLoop2Exec(std::function< void() >&& fun) noexcept
{
    return mainLoop_->Exec(std::move(fun));
}

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara
