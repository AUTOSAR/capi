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
/// @brief      Template class for encapsulating log parameters
/// @details
/// @date       2024-06-28
/// @author     yangjinbiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/LOG/DltBackend
/// @interface_level=unit
/// @trace_id_sr=LOG_SR_00001,LOG_SR_00003,LOG_SR_00009
/// @unit_name=daemon
/// @unit_description=Backend of Dlt module, used to support log and command forwarding.
/// @endcode
///
/// ================================================================

#include <ara/core/initialization.h>
#include <isoft/ara_fsh/platform.h>
#include <isoft/ara_fsh/process.h>

#include "Utils/src/private_log.h"
#include "logd/daemon.h"

/// @brief
/// @param argc
/// @param argv
/// @return
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_LOG_00327
/// @trace_id_dd=DD_LOG_01723
/// @needwork = ad
/// @endcode

int32_t main(int const argc, char *argv[])
{
    std::ignore = argc;
    std::ignore = argv;

    ara::core::Result< void > result{
        isoft::naicpp::GlobalGeneralEvLoop::Initialize(isoft::naicpp::GlobalGeneralEvLoop::Mode::kUserThread)};
    if (!result.HasValue()) {
        return EXIT_FAILURE;
    }
    if (!ara::core::Initialize().HasValue()) {
        std::cout << "#LOGD# ECU1 LOG Initialize failed" << std::endl;
        return (EXIT_FAILURE);
    }
    {
        // Need to ensure all resources are released before calling Deinitialize
        ara::log::internal::Daemon daemonInstatnce;
        std::int32_t const ret{daemonInstatnce.Init()};
        if (ret < 0) {
            std::ignore = daemonInstatnce.Destroy();
            return -1;
        }
        daemonInstatnce.Run();
        /// Used to release various resources early. There are issues when releasing in the destructor. Need to execute before the ara::core::Deinitialize() function
        // std::ignore = daemonInstatnce.Destroy();
        LOGVERBOSE(__func__) << " app main  exit ";
    }
    LOGVERBOSE(__func__) << " starting   ara::core::Deinitialize    ";
    //// Release all nai resources before this line
    if (!ara::core::Deinitialize().HasValue()) {
        LOGVERBOSE(__func__) << " Daemon  ara::core::Deinitialize error  ";
        return -1;
    }
    LOGVERBOSE(__func__) << " Daemon  ara::core::Deinitialize() ok ";
}
