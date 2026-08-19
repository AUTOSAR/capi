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
/// @brief      AutoSar-fw firewall daemon program.
/// @details    fw daemon program
/// @date       2024-12-13
/// @author     jzy
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/fw/main process
/// @interface_level=module
/// @trace_id_sr=SR_FW_0001
/// @unit_name=Main
/// @unit_description=Firewall processing entry function
/// @endcode
///
/// ================================================================
///
/// #include <isoft/ipccpp/debug.h>
///
/// ================================================================

#include <ara/core/string.h>

#include "ara/fw/common/common.h"
#include "ara/fw/filterengine/delete.h"
#include "ara/fw/internal/core_init.h"
#include "ara/fw/internal/ipc_name.h"
#include "ara/fw/internal/log_api.h"
#include "fwmanager/fw_manager.h"
#include "logsync/logmonitor.h"

//********************************/

/// @brief fw main function
/// @brief SOME/IP protocol header structure
/// @param argc Number of parameters of main function
/// @param argv Parameter list of main function
/// @return Normal/Abnormal exit
/// @code{.isoft}
/// @throws exception-object exception description
/// @interface_level=unit
/// @trace_id_ad=AD_FW_00000
/// @trace_id_dd=DD_FW_01015
/// @needwork = dda
/// @endcode
int32_t main(int32_t const argc, ara::fw::internal::char8_t **const argv)
{
    if (argc == 0) {
    }
    if (argv == nullptr) {
    }

    try {
        std::string const nFileName{ara::fw::internal::GetkFWIpcFile_ServerTest()};

        // Initialize CoreInit
        ara::fw::internal::CoreInit const coreInit{true, nFileName};  // {nFileName.data(),nFileName.size()}};
        ara::fw::internal::LogInfo() << "---- AP_fw..IPC  Evloop  Initialize Success";

        // fw destructor should be destroyed before Deinitialize
        {
            // fwManager  working start.
            ara::fw::internal::FWManager fw;

            // ipc init  manifest reader.
            if (!fw.Init()) {
                ara::fw::internal::LogError() << "fwManager:Init() failed!";
                return EXIT_FAILURE;
            }

            // starting   idsm sync logdata.
            ara::fw::internal::LogMonitor logMonitor{};
            std::thread t{&ara::fw::internal::LogMonitor::Start, &logMonitor};
            t.detach();

            // starting.  wait for fw status change from  IPC event.
            if (!fw.Run()) {
                ara::fw::internal::LogError() << "fwManager:Start() failed!";
            }

            // fw needs to clear nft filter rule set before closing the process.
            ara::fw::internal::FilterRulesDelete(1);
            ara::fw::internal::FilterRulesDelete(0);

            // fw needs to log and report to IDSM before the process closes
            // stop
            fw.Stop();
        }
        // ara::core::Deinitialize.
        std::ignore = ara::core::Deinitialize();

    } catch (...) {
        printf(
            "after deinitialize, ISD Error fw catch exception, exceptional "
            "exit.\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
//********************************/
