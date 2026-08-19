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
/// @file       session.h
/// @brief
/// @details
/// @date       2022-09-20
/// @author     yangtao
/// @version    1.2.0
///
/// ================================================================

#ifndef IPC_INNER_SESSION_H
#define IPC_INNER_SESSION_H

#include <pthread.h>

#if defined(IPC_SESSION_DEBUG)
    #include <stdio.h>
#endif
#include <assert.h>
#include <errno.h>
#include <nai/io/nai_buf.h>
#include <nai/io/nai_event.h>
#include <nai/os/nai_system.h>
#include <nai/runtime/nai_list.h>
#include <nai/runtime/nai_pool.h>

#include "client.h"
#include "queue.h"
#include "server.h"
#include "tree.h"
#include "utility.h"

typedef struct ipc_session_pool_s
{
    ipc_spinlock_t* lock;
    nai_list_entry_t list; /* the list of free session */
    size_t size;
    size_t alloc;
} ipc_session_pool_t;

typedef struct ipc_session_s ipc_session_t;
typedef void ipc_session_timeout_handler(void* context, ipc_session_t* session);
struct ipc_session_s
{
    nai_list_entry_t ent;

    TAILQ_ENTRY(ipc_session_s) outbound_node;

    ipc_packet_t* message;  // to be sent
    ipc_packet_t* request;
    ipc_packet_t* response;

    ipc_client_t* client;
    ipc_client_handler_t handler;
    ipc_session_id_t session_id;

    void* context;
    pthread_mutex_t* cvl;
    pthread_cond_t* cv;

    uint64_t deadline;
    struct
    {
        nai_evnode_t node;
        ipc_session_timeout_handler* handler;
        void* context;
    } timer;

    ipc_client_handler_status_t status;
    int errcode;

    nai_bufpool_t bufpool;
    ipc_session_pool_t* pool;

    RB_ENTRY(ipc_session_s) tree;
};

ipc_session_t* ipc_session_table_get(ipc_session_t* session);
void ipc_session_table_insert(ipc_session_t* session);
void ipc_session_table_remove(ipc_session_t* session);

int init_session_module(void);
int deinit_session_module(void);

void ipc_session_initialize(ipc_session_t* session);

ipc_session_t* ipc_session_create(void);

int ipc_session_start_timer(
    ipc_session_t* session, int timeout, nai_evloop_t* loop, ipc_session_timeout_handler* handler, void* context);

int ipc_session_stop_timer(ipc_session_t* session);

#define ipc_session_has_timer(session) ((session)->timer.handler != NULL)

void ipc_session_end(ipc_session_t* session, int errcode);

void ipc_session_delete(ipc_session_t* session);

#if defined(IPC_SESSION_DEBUG)
    #define pr_debug(fmt, ...) printf("IPC " fmt, ##__VA_ARGS__)

static inline void ipc_session_dump(ipc_session_t* session, const char* log)
{
    if (session) {
        pr_debug(
            "[%s] Session(%p), session->message(%p), session->client(%p), session->handler(%p), "
            "session->session_id(%ld)\n",
            log, (void*)session, (void*)session->message, (void*)session->client, (void*)session->handler,
            session->session_id);
    } else {
        pr_debug("[%s] Session is NULL\n", log);
    }
}
#else
static inline void ipc_session_dump(ipc_session_t* session, const char* log)
{
    (void)session;
    (void)log;
}
#endif

#endif  // IPC_INNER_SESSION_H
