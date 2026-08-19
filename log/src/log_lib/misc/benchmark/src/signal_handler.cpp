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
/// @file       signal_handler.cpp
/// @brief
/// @details
/// @date       2025-09-17
/// @author     yangjinbiao
/// @version    1.2.0
///
/// ================================================================

#include "signal_handler.h"

#include <csignal>

// Declaration of global atomic variables, controlling program execution
extern std::atomic_bool continueExecution;

SignalHandler::SignalHandler(std::atomic_bool &continueFlag) : continueExecution_(continueFlag) {}

void SignalHandler::SigTermHandler(int signal, std::atomic_bool &continueFlag)
{
    if (signal == SIGTERM) {
        continueFlag = false;
    }
}

bool SignalHandler::Register()
{
    struct sigaction sa;
    sa.sa_handler = [](int signal) {
        // Access the global continueExecution variable
        SigTermHandler(signal, continueExecution);
    };
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);

    return sigaction(SIGTERM, &sa, NULL) == 0;
}
