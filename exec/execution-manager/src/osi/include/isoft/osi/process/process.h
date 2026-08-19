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
/// @file       process.h
/// @brief      OS interface layer safe computing series operations
/// @details
/// @date       2023-03-08
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#ifndef ISOFT_OSI_PROC_H_
#define ISOFT_OSI_PROC_H_

#include <sys/prctl.h>
#include <sys/resource.h>

#include <csignal>  // PRQA S 1014, 5187
#include <cstdint>
#include <cstdio>  // PRQA S 5188
#include <tuple>

namespace isoft {
namespace osi {
namespace proc {

/// @brief Set process attribute to exit when parent process exits
/// @return 0 success; <0 failure
inline int32_t ExitFollowupFather() noexcept { return prctl(PR_SET_PDEATHSIG, SIGKILL); }  // PRQA S 3130

/// @brief Limit process heap and memory usage
/// @param heapBytes Heap usage limit in bytes
/// @param memBytes Memory usage limit in bytes
/// @return 0 success; <0 failure
inline int32_t LimitProcResourceUse(uint32_t const heapBytes, uint32_t const memBytes) noexcept
{
    struct rlimit limit
    {
    };
    if (heapBytes > 0UL) {
        std::ignore    = getrlimit(RLIMIT_DATA, &limit);
        limit.rlim_cur = heapBytes;
        if (0 != setrlimit(RLIMIT_DATA, &limit)) {
            perror("Limit process heap usage error");
            return -1;
        }
    }

    if (memBytes > 0UL) {
        std::ignore    = getrlimit(RLIMIT_AS, &limit);
        limit.rlim_cur = memBytes;
        if (0 != setrlimit(RLIMIT_AS, &limit)) {
            perror("Limit process memory usage error");
            return -1;
        }
    }

    return 0;
}

}  // namespace proc
}  // namespace osi
}  // namespace isoft

#endif  ///< ISOFT_OSI_PROC_H_