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
/// @file       ipc_em.h
/// @brief
/// @details
/// @date       2022-11-28
/// @author     yangtao
/// @version    1.2.0
///
/// ================================================================

#ifndef IPC_EM_INNER_PROCESS_H
#define IPC_EM_INNER_PROCESS_H

#include <sys/types.h>
#if defined(IPC_EM_DEBUG)
    #include <stdio.h>
#endif

#include <isoft/ipc/ipc_em.h>

#include "../manager.h"

typedef struct ipc_em_process_s
{
    RB_ENTRY(ipc_em_process_s) node;

    pid_t pid;
    char* path;
} ipc_em_process_t;

#if defined(IPC_EM_DEBUG)
    #define pr_debug(fmt, ...) printf("IPC " fmt, ##__VA_ARGS__)

static inline void ipc_em_dump(pid_t pid, const char* path, ipc_em_process_exit_status_t status, const char* log)
{
    pr_debug("[%s] pid(%d), path(%s) status(%d)\n", log, pid, path, status);
}
#else
static inline void ipc_em_dump(pid_t pid, const char* path, ipc_em_process_exit_status_t status, const char* log)
{
    (void)pid;
    (void)log;
    (void)status;
    (void)path;
}
#endif

#endif  //IPC_EM_INNER_PROCESS_H