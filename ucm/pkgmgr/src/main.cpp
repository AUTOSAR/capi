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
/// @brief      Main entry point of the Adaptive Autosar Package Manager.
/// @details
/// @date       2024-08-15
/// @author     cuiyinli
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @export_level=/UCM/PackageManager
/// @module_path=/UCM/PackageManager
/// @interface_level=unit
/// @trace_id_sr=SR_UCM_00001
/// @unit_name=Main
/// @unit_description=Main entry point of the Adaptive Autosar Package Manager
/// @endcode
///
/// ================================================================

#include <ara/core/initialization.h>

#include <list>

#include "app/package_management_application.h"
#include "common/alias.h"
#include "config/config.h"
#include "util/string_ext.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief parse command line, get disbale_authn
/// @param argc
/// @param argv
/// @return bool
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10403
/// @trace_id_dd=DD_UCM_11158
/// @needwork = ad
/// @endcode
static bool IsDisableAuthn(std::int32_t const argc, char8_t const** const argv) noexcept
{
    std::list< AraStringView > const args(argv, argv + argc);
    AraString const disableAuthnStr{strutil::GetOption(args, "--disable-authn")};
    return ("TRUE" == strutil::Upper(disableAuthnStr));
}

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara

/// @brief The main function forwards all operations
/// to the Execute method of ara::ucm::pkgmgr::PackageManagementApplication.
/// @param argc
/// @param argv
/// @return Returns 0 if program execution was successful
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10402
/// @trace_id_dd=DD_UCM_11157
/// @needwork = ad
/// @endcode
std::int32_t main(std::int32_t const argc, ara::ucm::pkgmgr::char8_t const** const argv) noexcept
{
    // parse command line, get disbale_authn
    bool const disableAuthn{ara::ucm::pkgmgr::IsDisableAuthn(argc, argv)};

    // init evloop
    if (!static_cast< bool >(
            isoft::naicpp::GlobalGeneralEvLoop::Initialize(isoft::naicpp::GlobalGeneralEvLoop::Mode::kUserThread))) {
        return EXIT_FAILURE;
    }

    // init core
    if (!static_cast< bool >(ara::core::Initialize())) {
        return EXIT_FAILURE;
    }

    // init config
    ara::ucm::pkgmgr::Config::GetInstance()->Init(disableAuthn);

    // run app
    std::int32_t ret{0};
    {
        ara::ucm::pkgmgr::PackageManagementApplication app{};
        ret = app.Execute();
    }

    // destroy config
    ara::ucm::pkgmgr::Config::Destroy();

    // deinit core
    if (!static_cast< bool >(ara::core::Deinitialize())) {
        return EXIT_FAILURE;
    }

    return ret;
}
