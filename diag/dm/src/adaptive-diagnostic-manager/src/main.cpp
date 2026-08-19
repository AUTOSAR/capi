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
/// @brief      This file provides the main function of the Diagnostic Manager.
/// @details
/// @date       2024-10-12
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================

#include <ara/core/initialization.h>
#include <isoft/uds/server.h>

#include <cstdlib>

#include "diagnostic_manager.h"
/// @brief Main function of the Diagnostic Manager
/// @code{.isoft}
/// export_level=/Diagnostics/Diagnostic Manager/Main
/// @endcode
/// @param[in] argc Number of parameters
/// @param[in] argv Parameter list

/// @return Execution code
/// @throw
std::int32_t main(std::int32_t const argc, char** const argv)
{
    std::ignore = argc;
    std::ignore = argv;
    if (!isoft::naicpp::GlobalGeneralEvLoop::Initialize(isoft::naicpp::GlobalGeneralEvLoop::Mode::kUserThread)
             .HasValue()) {
        return __LINE__;
    }

    if (!ara::core::Initialize().HasValue()) {
        return __LINE__;
    }

    {
        ara::diag::dmd::DiagnosticManager dm;

        if (dm.Run() != 0) {
            return __LINE__;
        }
    }

    if (!ara::core::Deinitialize().HasValue()) {
        return __LINE__;
    }

    return EXIT_SUCCESS;
}
