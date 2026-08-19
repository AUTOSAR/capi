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
/// @brief      Process entry
/// @details
/// @date       2022-06-10
/// @author     hejunwei
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @trace_id_sr=SRS_NM_00001
/// @unit_name=Daemon
/// @unit_description=Process entry
/// @module_path=/NetworkManager/nmm
/// @interface_level=unit
/// @endcode
///
/// ================================================================

#include <ara/core/initialization.h>

#include "include/daemon.h"
/// @brief the main entry of nm service
/// @returns Result of implementation specific error code.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100005
/// @trace_id_dd=DD_NM_00897
/// @needwork = ad
/// @endcode
std::int32_t main() noexcept
{
    if (!isoft::naicpp::GlobalGeneralEvLoop::Initialize(isoft::naicpp::GlobalGeneralEvLoop::Mode::kUserThread)
             .HasValue()) {
        return EXIT_FAILURE;
    }
    if (!ara::core::Initialize().HasValue()) {
        return EXIT_FAILURE;
    }
    {
        ara::nm::internal::NmLogger().LogDebug() << "hello world";
        ara::nm::internal::Daemon nmDaemon;
        nmDaemon.Run();
    }
    if (!ara::core::Deinitialize().HasValue()) {
        return EXIT_FAILURE;
    }
    return 0;
}
