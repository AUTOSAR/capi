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
/// @file       seccomp.cpp
/// @brief      OS interface layer safe computing series operations
/// @details
/// @date       2023-03-08
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#include "isoft/osi/security/seccomp.h"

#include <sched.h>

#include <cerrno>

#include "seccomp.h"

namespace isoft {
namespace osi {
namespace security {
namespace seccomp {

/// @brief Prohibit creation of child processes, implemented by prohibiting the fork system call
/// @return 0 success; <0 failure
int32_t DisableSysCallFork() noexcept
{
    scmp_filter_ctx ctx = seccomp_init(SCMP_ACT_ALLOW);
    if (nullptr == ctx) {
        return -1;
    }

 /// Principle: Under glibc, both fork and pthread_create use the clone system call at the bottom, the difference lies in the parameters of clone. The notable difference is that the former does not specify
 /// a user stack (nullptr), while the latter specifies a user stack, so it is only necessary to judge the second parameter (arg1) of the clone system call to distinguish whether fork
 /// or pthread_create is called. However, users can also use the clone function to create a process and set the second parameter of clone to non-null, thus bypassing this restriction.
 /// After analysis, the first parameter (flags) of clone, the CLONE_THREAD flag is used to create a thread, and not setting this flag is used to create a process, so it is only necessary
 /// to judge whether the CLONE_THREAD flag is set in the first parameter of the clone system call to distinguish whether fork or pthread_create, as well as clone process, is called
 /// The SCMP_ACT_KILL_PROCESS flag can be used to kill the caller when the clone system call is used, which is not conducive to user debugging, so instead
 /// the SCMP_ACT_ERRNO(EACCES) flag is used, returning EACCES error (Permission denied) when the fork/clone system call is used
    int32_t ret{seccomp_rule_add(ctx, SCMP_ACT_ERRNO(EACCES), SCMP_SYS(clone), 1U,
                                 SCMP_A0(SCMP_CMP_MASKED_EQ, CLONE_THREAD, 0U))};
    if (0 != ret) {
        return -1;
    }

 /// For the rules to take effect, the kernel needs to enable the following options:
    ///     CONFIG_HAVE_ARCH_SECCOMP=y
    ///     CONFIG_HAVE_ARCH_SECCOMP_FILTER=y
    ///     CONFIG_SECCOMP=y
    ///     CONFIG_SECCOMP_FILTER=y

    ret = seccomp_load(ctx);
    if (0 != ret) {
        return -1;
    }

    seccomp_release(ctx);

    return 0;
}

}  // namespace seccomp
}  // namespace security
}  // namespace osi
}  // namespace isoft
