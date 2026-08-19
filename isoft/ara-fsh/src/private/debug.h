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
/// @file       debug.h
/// @brief
/// @details
/// @date       2022-06-13
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#ifndef _ISOFT_ARA_FSH_DEBUG_H_
#define _ISOFT_ARA_FSH_DEBUG_H_

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <iostream>
#include <sstream>

#if ISOFT_ARA_FSH_DEBUG
    #define LOG_ERROR(msg) (std::cerr << "#ARA-FSH Error " << __FILE__ << ":" << __LINE__ << " " << msg)
#else
    #define LOG_ERROR(msg)                                                                                             \
        do {                                                                                                           \
            std::stringstream ss;                                                                                      \
            ss << msg;                                                                                                 \
        } while (0)
#endif

#endif
