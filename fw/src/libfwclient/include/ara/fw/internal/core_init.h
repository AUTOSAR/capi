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
/// @file       core_init.h
/// @brief      FW Daemon event loop etc. initialization processing class
/// @details    FW Daemon event loop etc. initialization processing class
/// @date       2024-12-16
/// @author     jzy
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/fw/IPC initialization
/// @interface_level=module
/// export_level=module
/// @trace_id_sr=SR_FW_0001
/// @unit_name=CoreInit
/// @endcode
///
/// ================================================================
///
///
/// ================================================================

#ifndef PH_FW_CORE_INIT_H_
#define PH_FW_CORE_INIT_H_
#include <ara/core/initialization.h>
#include <ara/exec/execution_client.h>
#include <isoft/ipccpp/debug.h>
#include <isoft/naicpp/terminating_handler.h>

#include <cassert>

#include "ara/fw/internal/log_api.h"
namespace ara {
namespace fw {
namespace internal {
/// @brief CoreInit operation
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00000
/// @trace_id_dd=DD_FW_00481
/// @needwork = dda
/// @endcode
class CoreInit
{
public:
    /// @brief Parameterized constructor
    /// @param bUseThread Whether to use threads
    /// @param stIpcFileName IPC file name
    /// @throws
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00482
    /// @needwork = dda
    /// @endcode
    CoreInit(bool const bUseThread, std::string const &stIpcFileName)
    {
        if (bUseThread) {
            // Initialize EvLoop: If blocking on the main thread is needed, EvLoop
            // must be manually initialized with kUserThread parameter before
            // core::init
            std::ignore = ::isoft::naicpp::GlobalGeneralEvLoop::Initialize(
                ::isoft::naicpp::GlobalGeneralEvLoop::Mode::kUserThread);
        }

        // ara::core::initialize.
        ara::core::Result< void > const resultInit{ara::core::Initialize()};
        assert(resultInit.HasValue());
        ara::fw::internal::LogInfo() << "ara::core::Initialize sucess";

        if (bUseThread) {
            pLifeCycle_ = std::make_unique< isoft::naicpp::TerminatingHandler >();
            std::ignore = pLifeCycle_->Register();
        }
        LogInfo() << "pLifeCycle_->Register()";

        pIpcDebugInit_ = std::make_unique< isoft::ipc::debug::MIpcDebugInit >(stIpcFileName);
        assert(false == pIpcDebugInit_->IsHaveError());
        LogInfo() << "ipc socket create sucess";
    }

    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00483
    /// @needwork = dda
    /// @endcode
    virtual ~CoreInit() noexcept
    {
        pIpcDebugInit_.reset();
        pLifeCycle_.reset();
        // std::ignore = ara::core::Deinitialize();
    }
    /// @brief Default move constructor
    /// @param other
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00484
    /// @needwork = dda
    /// @endcode
    CoreInit(CoreInit &&other) = delete;
    /// @brief Default move assignment function
    /// @param other
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00485
    /// @needwork = dda
    /// @endcode
    CoreInit &operator=(CoreInit &&other) = delete;
    /// @brief Default copy assignment function
    /// @param other
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00486
    /// @needwork = dda
    /// @endcode
    CoreInit &operator=(CoreInit const &other) = delete;
    /// @brief Default copy constructor
    /// @param otherThe return value of this function
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00487
    /// @needwork = dda
    /// @endcode
    CoreInit(CoreInit const &other) = delete;

private:
    /// @brief continueExecution_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00488
    /// @needwork = dda
    /// @endcode
    std::atomic_bool continueExecution_{true};
    /// @brief IPC initialization
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00489
    /// @needwork = dda
    /// @endcode
    std::unique_ptr< isoft::ipc::debug::MIpcDebugInit > pIpcDebugInit_{nullptr};
    /// @brief Lifecycle
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_FW_00000
    /// @trace_id_dd=DD_FW_00490
    /// @needwork = dda
    /// @endcode
    std::unique_ptr< isoft::naicpp::TerminatingHandler > pLifeCycle_{nullptr};
};

}  // namespace internal
}  // namespace fw
}  // namespace ara
#endif