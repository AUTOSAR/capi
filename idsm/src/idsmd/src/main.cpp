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
/// @brief      Process entry function
/// @details
/// @date       2023-01-16
/// @author     niuliming
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/idsm/event receiver handler
/// @interface_level=module
/// @trace_id_sr=SRS_IDSM_0001
/// @unit_name=EventProperPool
/// @unit_description=
/// @endcode
///
/// ================================================================

#include <ara/core/initialization.h>
#include <ara/exec/execution_client.h>
#include <isoft/ipccpp/debug.h>
#include <isoft/naicpp/global_evloop.h>
#include <isoft/naicpp/terminating_handler.h>

#include <cstdio>
#include <thread>

#include "ara/idsm/internal/common.h"
#include "config/idsm_struct_init.h"
#include "log/idsm_log.h"
#include "server/idsm_server.h"
#include "transmit/idsm_transmit.h"
namespace {
/// @brief Define char_8 data type
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = no
/// @endcode
using char_8 = char;  // NOLINT
/// @brief Get IPC name
/// @return IPC name
/// @code{.isoft}
/// @unit_name=GetIpcFileName
/// @interface_level=unit
/// @needwork = no
/// @endcode
// constexpr ara::crypto::char8_t const* GetIpcFileName() noexcept { return "idsmd"; }
}  // namespace

/// @brief Process entry function
/// @param argc Number of process arguments
/// @param argv Process arguments
/// @return Process execution result, 0 for normal, non-zero for abnormal
/// @code{.isoft}
/// @unit_name=main
/// @interface_level=unit
/// @needwork = no
/// @endcode
int32_t main(int32_t const argc, char_8* argv[]) noexcept
{
    if (argc == 0) {
    }
    if (argv == nullptr) {
    }
    try {
        // Only used for IPC debugging detached from execution management
        isoft::ipc::debug::MIpcDebugInit const ipcInit{"idsmd"};

        ara::core::Result< void > result{
            isoft::naicpp::GlobalGeneralEvLoop::Initialize(isoft::naicpp::GlobalGeneralEvLoop::Mode::kUserThread)};
        if (!result.HasValue()) {
            return EXIT_FAILURE;
        }
        /// @details Platform other module initialization
        result = ara::core::Initialize();
        if (!result.HasValue()) {
            LOG_ERROR << "ara::core::Initialize fail, error:" << result.Error().Message().data();
            return EXIT_FAILURE;
        }
        LOG_DEBUG << " ara::core::Initialize successful, Startup idsm.";
        /// @details Configuration file parsing
        if (ara::idsm::IdsmStruct::GetInstance()->StructInit() != ara::idsm::IdsmErrorCode::kIdsmSuccess) {
            LOG_ERROR << "configuration file read and parse fail.";
            ara::core::Result< void > const resVoid{ara::core::Deinitialize()};
            if (!resVoid.HasValue()) {
                LOG_ERROR << "ara core Deinitialize fail.";
            }
            return EXIT_FAILURE;
        }

        {
            /// @details IDSM server initialization
            ara::idsm::IdsmServer server{};
            LOG_DEBUG << "Startup initialize server.";
            int32_t ret{-1};
            ret = server.Init();
            if (ret != 0) {
                LOG_ERROR << "initialize server fail.";
                ara::core::Result< void > const resVoid{ara::core::Deinitialize()};
                if (!resVoid.HasValue()) {
                    LOG_ERROR << "ara core Deinitialize fail.";
                }
                ara::idsm::IdsmErrorCode const idsmErroCode{ara::idsm::IdsmStruct::GetInstance()->StructDeInit()};
                if (idsmErroCode != ara::idsm::IdsmErrorCode::kIdsmSuccess) {
                    LOG_ERROR << "Cleanup configuration file fail.";
                }
                return EXIT_FAILURE;
            }
            LOG_DEBUG << "Startup register term signal.";
            /// @details Register callback function with event loop
            std::shared_ptr< isoft::naicpp::EvLoop > const mainLoop{isoft::naicpp::GlobalGeneralEvLoop::Get()};
            isoft::naicpp::TerminatingHandler lc{[&server]() noexcept { server.Stop(); }};
            if (0 != lc.Register(mainLoop)) {
                LOG_ERROR << "register term signal fail, errno:" << ara::idsm::SysErr();
                ara::core::Result< void > const resVoid{ara::core::Deinitialize()};
                if (!resVoid.HasValue()) {
                    LOG_ERROR << "ara core Deinitialize fail.";
                }
                ara::idsm::IdsmErrorCode const idsmErroCode{ara::idsm::IdsmStruct::GetInstance()->StructDeInit()};
                if (idsmErroCode != ara::idsm::IdsmErrorCode::kIdsmSuccess) {
                    LOG_ERROR << "Cleanup configuration file fail.";
                }
                return EXIT_FAILURE;
            }
            LOG_DEBUG << "Startup background storage thread.";
            /// @details Start backend sending process to send security events to IDSR
            ara::idsm::IdsmWriter writer{};
            writer.StartMonitor();
            // writer.Init();
            // std::thread t{ara::idsm::IdsmWriter::Run, &writer};
            // t.detach();

            /// @details Report process status to execution management, destroy immediately after reporting
            {
                LOG_DEBUG << "Startup report running state";
                ara::exec::ExecutionClient const execClient{};
                ara::core::Result< void > const res{
                    execClient.ReportExecutionState(ara::exec::ExecutionState::kRunning)};
                if (!res.HasValue()) {
                    LOG_ERROR << "report running state fail.";
                }
            }
            /// @details Start event loop
            LOG_DEBUG << "Startup event main loop, block main thread";
            ret = server.Start();
            if (ret != 0) {
                LOG_ERROR << "event main loop fail.";
            }
            writer.StopMonitor();
        }
        LOG_DEBUG << "event main loop finished.";

        /// @details Cleanup work before process exit
        if (ara::idsm::IdsmStruct::GetInstance()->StructDeInit() != ara::idsm::IdsmErrorCode::kIdsmSuccess) {
            LOG_ERROR << "Cleanup configuration file fail.";
            return EXIT_FAILURE;
        }
        LOG_DEBUG << "Cleanup configuration file successful.";
        if (!ara::core::Deinitialize().HasValue()) {
            printf("after deinitialize, ISD ara::core::Deinitialize fail.\n");
            return EXIT_FAILURE;
        }
        printf("after deinitialize, ISD ara::core::Deinitialize successful.\n");
        return EXIT_SUCCESS;
    } catch (...) {
        printf("after deinitialize, ISD Error idsm catch exception, exceptional exit.\n");
        return EXIT_FAILURE;
    }
}