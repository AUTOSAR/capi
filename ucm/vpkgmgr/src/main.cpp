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
/// @brief      Program entry point
/// @details
/// @date       2024-07-20
/// @author     hanzhibo
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// export_level=/
/// @module_path=/UCM Master/Main
/// @interface_level=unit
/// @trace_id_sr=SR_UCM_00035
/// @unit_name=Main
/// @unit_description=Program entry point
/// @endcode
///
/// ================================================================

#include <ara/core/initialization.h>
#include <ara/log/logger.h>
#include <isoft/ipccpp/debug.h>
#include <isoft/naicpp/terminating_handler.h>

#include "utils/config/config.h"
#include "utils/helper.h"
#include "utils/types.h"
#include "vehicle_package_application.h"

namespace ara {
namespace ucm {
namespace vpkgmgr {

/// @brief parse command line, get disbale_authn
/// @param argc
/// @param argv
/// @return bool
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00488
/// @trace_id_dd=DD_UCM_Master_00945
/// @needwork = dd
/// @endcode
static bool IsDisableAuthn(std::int32_t const argc, ara::ucm::vpkgmgr::char8_t const** const argv) noexcept
{
    std::list< ara::core::StringView > const args(argv, argv + argc);
    ara::core::String const disableAuthnStr{ara::ucm::vpkgmgr::helper::GetOption(args, "--disable-authn")};
    return ("TRUE" == ara::ucm::vpkgmgr::helper::ToUpper(disableAuthnStr));
}

/// @brief vpkgmgr main
/// @param disableAuthn
/// @return int
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00488
/// @trace_id_dd=DD_UCM_Master_00944
/// @needwork = dd
/// @endcode
static std::int32_t VpkgmgrMain(bool const disableAuthn) noexcept
{
    // // Do not buffer output
    ///setvbuf(stdout, NULL, _IONBF, 0);.

    ///isoft::ipc::debug::MIpcDebugInit const ipcInit("ucmm");.
    ara::core::Result< void > ret{
        isoft::naicpp::GlobalGeneralEvLoop::Initialize(isoft::naicpp::GlobalGeneralEvLoop::Mode::kUserThread, true)};
    if (!ret.HasValue()) {
        std::cout << "GlobalGeneralEvLoop::Initialize failed!, errmsg " << ret.Error().Message().data() << std::endl;
        return EXIT_FAILURE;
    }

    ret = ara::core::Initialize();
    if (!ret.HasValue()) {
        std::cout << "core::Initialize failed!, errmsg " << ret.Error().Message().data() << std::endl;
        return EXIT_FAILURE;
    }

    // Initialize configuration parameters
    ara::ucm::vpkgmgr::Config::GetInstance()->Init(disableAuthn);

    int32_t result{0};
    {
        std::shared_ptr< isoft::naicpp::EvLoop > const mainLoop{isoft::naicpp::GlobalGeneralEvLoop::Get()};
        ara::ucm::vpkgmgr::VehiclePackageManagementApplication app;
        if (app.Initialize()) {
            // run
            isoft::naicpp::TerminatingHandler lc{[&app]() noexcept { app.Stop(); }};
            if (0 != lc.Register(mainLoop)) {
                std::cout << "failed to lc.Init()." << std::endl;
                result = EXIT_FAILURE;
            } else {
                app.Run();
            }
        }
        app.Uninitialize();
    }

    // Configuration deinitialization
    ara::ucm::vpkgmgr::Config::Destroy();

    // Destroy default logger
    ara::ucmm::common::DestroyDefaultLogger();
    if (!ara::core::Deinitialize().HasValue()) {
        return EXIT_FAILURE;
    }

    return result;
}

}  // namespace vpkgmgr
}  // namespace ucm
}  // namespace ara

/// @brief main
/// @param argc
/// @param argv
/// @return int
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00488
/// @trace_id_dd=DD_UCM_Master_00943
/// @needwork = ad
/// @endcode
std::int32_t main(std::int32_t const argc, ara::ucm::vpkgmgr::char8_t const** const argv) noexcept
{
    // Parse command line to get disbale_authn
    bool const disableAuthn{ara::ucm::vpkgmgr::IsDisableAuthn(argc, argv)};

    // run vpkgmgr
    return ara::ucm::vpkgmgr::VpkgmgrMain(disableAuthn);
}
