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
/// @brief      main
/// @details
/// @date       2024-06-26
/// @author     lianglongxiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/StateManagement/Main
/// @unit_name=Main
/// @interface_level=software
/// @unit_description=main
/// @trace_id_sr=SR_SM_
/// @endcode
///
/// ================================================================

#ifdef LOG_MODE
    #include <ara/log/logger.h>
#endif  // LOG_MODE

#include <ara/core/initialization.h>

#include "event_management/event_manager.h"

namespace {
using Char8_t = char;
}  // namespace
/*___________________________________________________________________________________________________________
 * main
 */
/// @brief main entry
/// @param argc the argc
/// @param argv the argv
/// @return int
/// @throws Exception thrown
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
int32_t main(int32_t const argc, Char8_t **const argv)
{
    int32_t ret{0};
    try {  // Do not buffer output
        std::ignore = setvbuf(stdout, nullptr, _IONBF, 0U);
        if (ara::core::Initialize().HasValue() == false) {
            return EXIT_FAILURE;
        }
        std::ignore = argc;
        std::ignore = argv;
        {
            // Get logger
            ara::log::Logger &logger{
                ara::log::CreateLogger((ara::core::StringView{"MAIN"}), (ara::core::StringView{"smd main"}))};
            logger.LogInfo() << "Main, starting.";
            ara::sm::event_management::EventManager eventManager;
            if (!eventManager.Init()) {
                logger.LogError() << "eventManager, Init failed";
                return EXIT_FAILURE;
            }

            if (!eventManager.Run()) {
                logger.LogError() << "eventManager, Run failed";
                return EXIT_FAILURE;
            }

            eventManager.Stop();
            logger.LogInfo() << "Main, ending.";
        }
        if (ara::core::Deinitialize().HasValue() == false) {
            ret = EXIT_FAILURE;
        }

    } catch (...) {
    }
    return ret;
}