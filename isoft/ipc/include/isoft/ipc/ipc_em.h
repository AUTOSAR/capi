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

#ifndef IPC_EM_PUBLIC_HEADER
#define IPC_EM_PUBLIC_HEADER

#ifdef __cplusplus
extern "C"
{
#endif

    /**
 * @brief Process exit status
 */
    enum ipc_em_process_exit_status_s
    {
        IPC_EM_PROCESS_EXIT_SUCCESS = 0,
        IPC_EM_PROCESS_EXIT_WITH_FAILURE,
    };
    typedef enum ipc_em_process_exit_status_s ipc_em_process_exit_status_t;

    /**
 * @brief Insert new process information into the process table
 * @param pid Process pid
 * @param path Socket address information
 * @return Returns 0 on success, returns -1 and sets errno on failure
 */
    extern int ipc_em_insert(pid_t pid, const char* path);

    /**
 * @brief Remove process information from the process table
 * @param pid Process pid
 * @param status Process exit status
 * @return Returns 0 on success, returns -1 and sets errno on failure
 */
    extern int ipc_em_remove(pid_t pid, ipc_em_process_exit_status_t status);

#ifdef __cplusplus
}
#endif

#endif  // IPC_EM_PUBLIC_HEADER