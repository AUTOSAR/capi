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
/// @brief      ara-loader entry
/// @details
/// @date       2023-03-06
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#include <sys/stat.h>
#include <unistd.h>

#include <iostream>
#include <string>

#include "ara/loader/manager.h"

namespace {

/// @brief char redefinition
using Char8_t = char;

/// @brief Print help information
void Help() noexcept
{
    std::cout << "Usage:" << std::endl;
    std::cout << "ara-loader [OPTION] [OPTARG]" << std::endl;
    std::cout << std::endl;
    std::cout << "Option:" << std::endl;
    std::cout << "OPT     ARG              COMMENT" << std::endl;
    std::cout << " -s     ARA_SYSROOT      the top dir wich include ara. default is \"/\"" << std::endl;
    std::cout << " -v     None             DebugMode, show more Verbose log info" << std::endl;
    std::cout << " -d     DEBUG_PORT       enable debug server and set the port" << std::endl;
    std::cout << " -r     None             DebugMode, disable resource group" << std::endl;
}

/// @brief Run in background mode
/// @return 0 success; <0 failure
int32_t RunInDaemon() noexcept
{
    int32_t const pid{fork()};

    if (pid < 0) {
        perror("fork");
        return -1;
    }

    if (pid > 0) {
        return EXIT_SUCCESS;
    }

    // become session leader
    if (-1 == setsid()) {
        perror("setsid()");
        return -1;
    }

    // clear umask
    std::ignore = umask(0U);

    // close file discriptor
    int32_t const kMaxFdNumber{1024};
    for (int32_t fd{0}; fd < kMaxFdNumber; fd++) {
        std::ignore = close(fd);
    }
    return 0;
}
}  // namespace

/// @brief Program entry function
/// @param argc Number of arguments
/// @param argv Argument information
/// @return Return value
int32_t main(int32_t const argc, Char8_t** const argv) noexcept
{
    try {
        std::string araSysroot;
        ara::loader::DebugConfig debugMode;
        int32_t opt{0};
        int32_t ret{0};
        while (true) {
            opt = getopt(argc, argv, "s:d:vr");  // NOLINT
            if (-1 == opt) {
                break;
            }

            switch (opt) {
                case 's': {
                    araSysroot = optarg;
                    break;
                }
                case 'd': {
                    debugMode.SetServerPort(static_cast< uint16_t >(std::stoi(optarg)));
                    break;
                }
                case 'v': {
                    debugMode.SetVerbose(true);
                    break;
                }
                case 'r': {
                    debugMode.DisableResourceGroup(true);
                    break;
                }
                default: {
                    Help();
                    ret = -1;
                    break;
                }
            }
        }

        if (ret < 0) {
            return -1;
        }

        if (!debugMode.IsDebugOn()) {
            if (0 != RunInDaemon()) {
                return -1;
            }
        }

        std::unique_ptr< ara::loader::Manager > const loadManager{ara::loader::Manager::CreateManager(araSysroot)};
        if (nullptr == loadManager) {
            std::cout << "nullptr == loadManager" << std::endl;
            return -1;
        }

        if (0 != loadManager->Boot(debugMode)) {
            std::cout << "ERROR on loader->Boot()" << std::endl;
            return -1;
        }
    } catch (...) {
    }
    return 0;
}
