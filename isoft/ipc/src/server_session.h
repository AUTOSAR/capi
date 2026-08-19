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
/// @file       server_session.h
/// @brief
/// @details
/// @date       2022-12-07
/// @author     yangtao
/// @version    1.2.0
///
/// ================================================================

#ifndef IPC_INNER_SERVER_SESSION_H
#define IPC_INNER_SERVER_SESSION_H

#if defined(IPC_SERVER_SESSION_DEBUG)
    #include <stdio.h>
#endif
#include <errno.h>
#include <isoft/ipc/packet.h>
#include <nai/runtime/nai_list.h>
#include <nai/runtime/nai_pool.h>

#include "packet.h"
#include "peer.h"
#include "tree.h"
#include "utility.h"

typedef struct ipc_server_session_pool_s
{
    ipc_spinlock_t *lock;
    nai_list_entry_t list; /* the list of free session */
    size_t size;
    size_t alloc;
} ipc_server_session_pool_t;

typedef struct ipc_server_session_s
{
    nai_list_entry_t ent;
    RB_ENTRY(ipc_server_session_s) node;

    pid_t pid;
    uint64_t seq;
    ipc_peer_t *peer;
    ipc_session_id_t session_id;

    nai_bufpool_t bufpool;
    ipc_server_session_pool_t *pool;
} ipc_server_session_t;

ipc_server_session_t *ipc_server_session_get(ipc_session_id_t session_id);
void ipc_server_session_insert(ipc_packet_t *packet);
void ipc_sever_session_remove_by_seq(uint64_t seq);
void ipc_server_session_remove_by_pid(pid_t pid);

int init_server_session_module(void);
int deinit_server_session_module(void);

ipc_server_session_t *ipc_server_session_create(ipc_packet_t *packet);

void ipc_server_session_end(ipc_server_session_t *session);

#if defined(IPC_SERVER_SESSION_DEBUG)
    #define pr_debug(fmt, ...) printf("IPC " fmt, ##__VA_ARGS__)

static inline void ipc_server_session_dump(ipc_server_session_t *session, const char *log)
{
    if (session) {
        pr_debug("[%s] Session(%p), session->seq(%d), session->pid(%d), session->peer(%p), session->session_id(%ld)\n",
                 log, (void *)session, session->seq, session->pid, (void *)session->peer, session->session_id);
    } else {
        pr_debug("[%s] Session is NULL\n", log);
    }
}
#else
static inline void ipc_server_session_dump(ipc_server_session_t *session, const char *log)
{
    (void)session;
    (void)log;
}
#endif

#endif  //IPC_INNER_SERVER_SESSION_H
