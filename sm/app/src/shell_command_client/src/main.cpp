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
/// @date       2024-06-25
/// @author     lianglongxiao
/// @version    1.2.0
///
/// ================================================================

#include <bits/stdint-intn.h>
#include <getopt.h>

#include <chrono>
#include <iostream>
#include <memory>

#include "shell_command_client.h"

namespace {
void PrintHelp()
{
    std::cout
        << "Usage: shell_request_client. All parameters are case sensitive. Options:\n"
        << "  -h, --help              Print this help message and exit\n"
        << "  -f, --fg                Option: FunctionGroup\n"
        << "  -S, --sm                Option: StateMachine\n"
        << "  -s, --set               Option: Set state\n"
        << "  -g, --get               Option: Get state\n"
        << "  -G, --get_all_infos     Option: Get all infomations about sm\n"
        << "  Caution! input parameter is needed: FGFQN.State or SMFQN.TransitionRequestNumber when '-s' is given; "
           "FGFQN or SMFQN when '-g' is given\n"
        << "  Examples:\n"
        << "  ./shell_request_client -f -g /ISOFT/FunctionGroupSet/Machine1/MachineFG                                  "
           "  Get MachineFG's current state\n"
        << "  ./shell_request_client -f -s /ISOFT/FunctionGroupSet/Machine1/MachineFG.Startup                          "
           "  Set MachineFG's state to Startup\n"
        << "  ./shell_request_client -S -g /ISOFT/Development/Machine1/StateManagementModuleInstantiation/MachineSM    "
           "  Get MachineSM's current state\n"
        << "  ./shell_request_client -S -s /ISOFT/Development/Machine1/StateManagementModuleInstantiation/MachineSM.1  "
           "  Set MachineSM's state by transition request number\n"
        << "  ./shell_request_client -G     Get all infomations about sm\n";
}
}  // namespace

int32_t main(int32_t const argc, char* argv[])
{
    try {
        std::string const fg{"fg"};
        std::string const sm{"sm"};
        std::string const setState{"s"};
        std::string const getState{"g"};

        std::string type{};
        std::string direction{};
        std::string inputParasStr{};
        std::string fgFQN{};
        std::string fgState{};
        std::string smFQN{};
        std::string smTransitionRequestNumberStr{};
        std::string const split{"."};

        // Do not buffer output
        std::ignore = setvbuf(stdout, nullptr, _IONBF, 0UL);

        ShellCommandClient client;

        int32_t option;
        bool optionSet{false};
        bool optionGet{false};
        bool optionGetAll{false};
        bool optionFg{false};
        bool optionSm{false};

        // Define long parameters
        struct option const long_options[]{{"set", no_argument, nullptr, 's'},            // NOLINT
                                           {"get", no_argument, nullptr, 'g'},            // NOLINT
                                           {"getAllInfos", no_argument, nullptr, 'G'},    // NOLINT
                                           {"functionGroup", no_argument, nullptr, 'f'},  // NOLINT
                                           {"stateMachine", no_argument, nullptr, 'S'},   // NOLINT
                                           {"help", no_argument, nullptr, 'h'},           // NOLINT
                                           {nullptr, 0, nullptr, 0}};                     // NOLINT

        // Parse parameters
        do {                                                                    // NOLINT
            option = getopt_long(argc, argv, "hsgSfG", long_options, nullptr);  // NOLINT
            switch (option) {                                                   // NOLINT
                case 'h': {                                                     // NOLINT
                    PrintHelp();                                                // NOLINT
                } break;                                                        // NOLINT
                case 's': {                                                     // NOLINT
                    optionSet = true;                                           // NOLINT
                    direction = setState;                                       // NOLINT
                } break;                                                        // NOLINT
                case 'g': {                                                     // NOLINT
                    optionGet = true;                                           // NOLINT
                    direction = getState;                                       // NOLINT
                } break;                                                        // NOLINT
                case 'S': {                                                     // NOLINT
                    optionSm = true;                                            // NOLINT
                    type     = sm;                                              // NOLINT
                } break;                                                        // NOLINT
                case 'f': {                                                     // NOLINT
                    optionFg = true;                                            // NOLINT
                    type     = fg;                                              // NOLINT
                } break;                                                        // NOLINT
                case 'G': {                                                     // NOLINT
                    optionGetAll = true;                                        // NOLINT
                } break;                                                        // NOLINT
                case '?': {                                                     // NOLINT
                } break;                                                        // NOLINT
                default: {                                                      // NOLINT
                } break;                                                        // NOLINT
            }                                                                   // NOLINT
        } while (option != -1);                                                 // NOLINT

        if (optionGet && optionSet) {
            std::cerr << "'--set' and '--get' can't be assigned together!\n";
            return EXIT_FAILURE;
        }

        if (optionSm && optionFg) {
            std::cerr << "'--sm' and '--fg' can't be assigned together!\n";
            return EXIT_FAILURE;
        }

        // Process inputParasStr
        if (optind < argc) {
            inputParasStr = argv[optind];
        } else if (optionSm || optionFg) {
            std::cerr
                << "Pease give the input parameter: FGFQN.State or SMFQN.TransitionRequestNumber when '-s' is given; "
                   "FGFQN or SMFQN when '-g' is given!\n";
            return EXIT_FAILURE;
        } else if (optionGetAll) {
        } else {
            return 0;
        }

        size_t const splitPos{inputParasStr.find(split)};
        if (0 == direction.compare(setState)) {
            if (std::string::npos == splitPos) {
                std::cerr << "Set state needs another para, fgState or smTransitionRequestNumber!\n";
                return EXIT_FAILURE;
            }
            if (0 == type.compare(fg)) {
                fgFQN       = inputParasStr.substr(0UL, splitPos);
                fgState     = inputParasStr.substr(splitPos + 1UL);
                int32_t res = client.SetFgState(fgFQN, fgState);
                if (0 != res) {
                    std::cerr << "SetFgState failed: " << fgFQN << "." << fgState << ", error: " << res << std::endl;
                    return 1;
                }

                return 0;
            }
            if (0 == type.compare(sm)) {
                smFQN                        = inputParasStr.substr(0UL, splitPos);
                smTransitionRequestNumberStr = inputParasStr.substr(splitPos + 1UL);
                try {
                    uint32_t const smTransitionRequestNumber{
                        static_cast< uint32_t >(std::stoi(smTransitionRequestNumberStr))};
                    int32_t res = client.SetSmState(smFQN, smTransitionRequestNumber);
                    if (0 != res) {
                        std::cerr << "SetSmState failed: " << smFQN << "." << smTransitionRequestNumber
                                  << ", error: " << res << std::endl;
                        return 1;
                    }
                    return 0;
                } catch (std::invalid_argument const& e) {
                    std::cerr << "Invalid smTransitionRequestNumberStr: " << e.what() << std::endl;
                } catch (std::out_of_range const& e) {
                    std::cerr << "Out of range: " << e.what() << std::endl;
                }
            }
            std::cerr << "Please give the correct type: " << fg << " or " << sm << "!\n";
        } else if (0 == direction.compare(getState)) {
            if (0 == type.compare(fg)) {
                fgFQN       = inputParasStr;
                int32_t res = client.GetFgState(fgFQN);
                if (0 != res) {
                    std::cerr << "GetFgState failed: " << fgFQN << ", error: " << res << std::endl;
                    return 1;
                }
                return 0;
            }
            if (0 == type.compare(sm)) {
                smFQN       = inputParasStr;
                int32_t res = client.GetSmState(smFQN);
                if (0 != res) {
                    std::cerr << "GetSmState failed: " << smFQN << ", error: " << res << std::endl;
                    return 1;
                }
                return 0;
            }
            std::cerr << "Please give the correct type: " << fg << " or " << sm << "!\n";
        } else if (optionGetAll) {
            int32_t const ret{client.GetAllInfos()};
            if (0 == ret) {
                return client.SubscribeSMStateInfo();
            }
            return ret;
        } else {
            std::cerr << "Please give the correct direction: " << setState << " or " << getState << "!\n";
        }
    } catch (...) {
    }
    return 0;
}
