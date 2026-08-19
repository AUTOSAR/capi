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
/// @file       define.h
/// @brief
/// @details
/// @date       2025-09-17
/// @author     yangjinbiao
/// @version    1.2.0
///
/// ================================================================

#ifndef CONFIG_HELPER_H
#define CONFIG_HELPER_H

#include <cstdint>
#include <iostream>
#include <string>

#include "ini_parser.h"

struct TLVMessage
{
    uint32_t type;     // 4-byte type
    uint32_t length;   // 4-byte length
    char value[1016];  // 1016-byte data
};
struct Config
{
    bool showHelp    = false;
    bool netMode     = false;
    bool showVersion = false;
    bool testApi     = false;
    std::string outputFile;
    std::int32_t testMethod = 0;
    std::int64_t msgCount   = 0;
    std::string version     = "1.0.0";
    std::int32_t port       = 8888;
};

class ConfigHelper
{
public:
    static bool load(const std::string& filePath, Config& config)
    {
        IniParser parser;
        if (!parser.load(filePath)) {
            std::cerr << "Unable to open configuration file: " << filePath << std::endl;
            return false;
        }

        config.showHelp    = parser.getBool("show_help", false);
        config.netMode     = parser.getBool("net_mode", false);
        config.showVersion = parser.getBool("show_version", false);
        config.testApi     = parser.getBool("test_api", false);
        config.outputFile  = parser.getString("output_file", "");
        config.testMethod  = parser.getInt("test_method", 0);
        config.msgCount    = static_cast< std::int64_t >(parser.getInt("msg_count", 0));
        config.port        = parser.getInt("port", 8888);

        // Special handling for version number
        std::string ver = parser.getString("version", "");
        if (!ver.empty()) {
            config.version = ver;
        }

        return true;
    }

    static void printHelp(Config& config)
    {
        std::cout << "Available configuration parameters:\n"
                  << "  showHelp: " << (config.showHelp ? "true" : "false") << "\n"
                  << "  netMode: " << (config.netMode ? "true" : "false") << "\n"
                  << "  showVersion: " << (config.showVersion ? "true" : "false") << "\n"
                  << "  testApi: " << (config.testApi ? "true" : "false") << "\n"
                  << "  outputFile: " << config.outputFile << "\n"
                  << "  testMethod: " << config.testMethod << "\n"
                  << "  msgCount: " << config.msgCount << "\n"
                  << "  version: " << config.version << "\n"
                  << "  port: " << config.port << std::endl;
    }
};

#endif
