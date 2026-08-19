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
/// @brief
/// @details
/// @date       2025-09-17
/// @author     yangjinbiao
/// @version    1.2.0
///
/// ================================================================

#include <ara/core/initialization.h>
#include <ara/log/logger.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <tuple>

#include "api_single.h"
#include "define.h"
#include "main_logic.h"
#include "sys_benchmark.h"

int main(int argc, char *argv[])
{
    // Initialize configuration structure

    Config config;
    std::int32_t methodfromComand{-1};

    // Parse command line arguments, find the configuration file path
    std::string configFilePath{"./etc/testCPU.ini"};
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if ((arg == "-c" || arg == "--config") && i + 1 < argc) {
            configFilePath = argv[i + 1];
            i++;  // Skip the configuration file path parameter
        } else if ((arg == "-m" || arg == "--method") && i + 1 < argc) {
            methodfromComand = std::atoi(argv[i + 1]);
            i++;  // Skip the configuration file path parameter
        } else if (arg == "--net") {
            config.netMode = true;
            return 0;
        }
    }
    std::cerr << "configfile : " << configFilePath << std::endl;
    // Load the configuration file (if specified)
    if (!configFilePath.empty()) {
        if (!ConfigHelper::load(configFilePath, config)) {
            std::cerr << "Failed to load configuration file, exiting " << std::endl;
            return 0;
        }
    }

    // Initialize ARA core
    if (!ara::core::Initialize()) {
        return EXIT_FAILURE;
    }

    // Check necessary configuration parameters
    if (config.msgCount <= 0) {
        std::cerr << "Error: Message count must be positive" << std::endl;
        ara::core::Deinitialize();
        return EXIT_FAILURE;
    }

    if (methodfromComand > 0) {
        config.testMethod = methodfromComand;
    }

    if (config.testApi) {
        ////////////////// Collect system information-------------------------------------
        SystemBenchmark sysbe;
        sysbe.run();
    }

    if (config.netMode) {
        int sockfd;
        struct sockaddr_in servaddr, cliaddr;
        socklen_t len;
        TLVMessage buffer;

        // Create UDP socket
        if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
            perror("socket creation failed");
            exit(EXIT_FAILURE);
        }

        // Initialize server address structure
        memset(&servaddr, 0, sizeof(servaddr));
        memset(&cliaddr, 0, sizeof(cliaddr));

        servaddr.sin_family      = AF_INET;
        servaddr.sin_addr.s_addr = INADDR_ANY;
        servaddr.sin_port        = htons(config.port);

        // Bind socket to port
        if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
            perror("bind failed");
            exit(EXIT_FAILURE);
        }

        std::cout << "Consumer listening on port " << config.port << "..." << std::endl;
        len = sizeof(cliaddr);

        while (true) {
            // Receive data
            ssize_t n = recvfrom(sockfd, &buffer, sizeof(TLVMessage), MSG_WAITALL, (struct sockaddr *)&cliaddr, &len);

            if (n < 0) {
                perror("recvfrom failed");
                std::cerr << "recvfrom failed" << std::endl;
                continue;
            }

            // Convert network byte order to host byte order
            uint32_t type   = ntohl(buffer.type);
            uint32_t length = ntohl(buffer.length);
            std::ignore     = type;
            std::ignore     = length;
            // std::ignore = MainLogicCpuUsage(config.msgCount, config.testMethod);

            ara::log::Logger &logger1 = ara::log::CreateLogger("ctx1", "LTApp0", ara::log::LogLevel::kVerbose);
            ara::log::Logger &logger2 = ara::log::CreateLogger("ctx2", "LTApp0", ara::log::LogLevel::kVerbose);
            ara::log::Logger &logger3 = ara::log::CreateLogger("ctx3", "LTApp0", ara::log::LogLevel::kVerbose);
            ApiSingle btester;
            // std::cerr<<"start methoend :
            // "<<std::to_string(config.testMethod)<<std::endl;
            switch (config.testMethod) {
                case 0: {
                    btester.RunIdleTest("000000000000000 just a func");
                    break;
                }
                case 1: {
                    btester.RunLoggerTest(logger1);
                    break;
                }
                case 2: {
                    btester.RunLoggerTest(logger2);
                    break;
                }
                case 3: {
                    btester.RunLoggerTest(logger3);
                    break;
                }
                case 4: {
                    btester.RunCoutTest();
                    break;
                }
                case 5: {
                    btester.RunTimestampedCoutTest();
                    break;
                }
                case 6: {
                    btester.RunCustomCachedFileWriteTest("./btmp.log");
                    break;
                }
                case 7: {
                    btester.RunDirectFileWriteTest("./dtmp.log");
                    break;
                }
                default:
                    break;
            }
        }

        close(sockfd);

    } else {
        std::ignore = MainLogicCpuUsage(config.msgCount, config.testMethod);
    }

    // Deinitialize ARA core
    if (!ara::core::Deinitialize()) {
        return EXIT_FAILURE;
    }

    return 0;
}
