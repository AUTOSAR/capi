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
/// @file       server_session.c
/// @brief
/// @details
/// @date       2022-12-07
/// @author     yangtao
/// @version    1.2.0
///
/// ================================================================

#include "server_session.h"

static int ipc_server_session_compare(ipc_server_session_t* left, ipc_server_session_t* right)
{
    int result;
    if (left->seq < right->seq)
        result = -1;
    else if (left->seq > right->seq)
        result = 1;
    else
        result = 0;
    return result;
}

typedef RB_HEAD(ipc_server_session_table_s, ipc_server_session_s) ipc_server_session_table_t;
RB_GENERATE_INTERNAL(ipc_server_session_table_s, ipc_server_session_s, node, ipc_server_session_compare, static)

typedef struct ipc_server_session_runtime_s
{
    ipc_spinlock_t server_session_lock;
    ipc_server_session_table_t server_session_table;  // server request table
} ipc_server_session_runtime_t;

static ipc_server_session_runtime_t rt = {
    .server_session_lock  = IPC_SPINLOCK_INITIALIZER,
    .server_session_table = RB_INITIALIZER(&rt.server_session_table),
};

ipc_server_session_t* ipc_server_session_get(ipc_session_id_t session_id)
{
    if (session_id == 0)
        return NULL;

    uint64_t seq                  = session_id;
    ipc_server_session_t dummy    = {.seq = seq};
    ipc_server_session_t* session = NULL;

    ipc_spinlock_lock(&rt.server_session_lock);
    session = RB_FIND(ipc_server_session_table_s, &rt.server_session_table, &dummy);
    ipc_spinlock_unlock(&rt.server_session_lock);

    return session;
}

void ipc_server_session_insert(ipc_packet_t* packet)
{
    ipc_server_session_t* session;
    session = ipc_server_session_create(packet);
    if (session) {
        ipc_spinlock_lock(&rt.server_session_lock);
        RB_INSERT(ipc_server_session_table_s, &rt.server_session_table, session);
        ipc_spinlock_unlock(&rt.server_session_lock);
    }

    return;
}

void ipc_sever_session_remove_by_seq(uint64_t seq)
{
    ipc_server_session_t dummy    = {.seq = seq};
    ipc_server_session_t* session = NULL;

    ipc_spinlock_lock(&rt.server_session_lock);
    session = RB_FIND(ipc_server_session_table_s, &rt.server_session_table, &dummy);
    if (session)
        RB_REMOVE(ipc_server_session_table_s, &rt.server_session_table, session);
    ipc_spinlock_unlock(&rt.server_session_lock);

    if (session)
        ipc_server_session_end(session);

    return;
}

void ipc_server_session_remove_by_pid(pid_t pid)
{
    ipc_server_session_t* session = NULL;

    ipc_spinlock_lock(&rt.server_session_lock);
    RB_FOREACH(session, ipc_server_session_table_s, &rt.server_session_table)
    {
        if (session && session->pid == pid) {
            RB_REMOVE(ipc_server_session_table_s, &rt.server_session_table, session);
        }
    }
    ipc_spinlock_unlock(&rt.server_session_lock);

    if (session)
        ipc_server_session_end(session);

    return;
}

ipc_server_session_pool_t ipc_global_server_session_pool;
ipc_spinlock_t ipc_server_session_pool_lock = IPC_SPINLOCK_INITIALIZER;

int init_server_session_module(void)
{
    int r;
    ipc_server_session_pool_t* p = &ipc_global_server_session_pool;

    nai_list_init(&p->list);
    p->lock  = &ipc_server_session_pool_lock;
    p->size  = 64 * 1024;
    p->alloc = 0;
    r        = 0;

    return r;
}

int deinit_server_session_module(void)
{
    int r;
    ipc_server_session_pool_t* p = &ipc_global_server_session_pool;
    ipc_server_session_t* session;
    nai_list_entry_t* e;
    nai_bufpool_t* pool;

#if defined(IPC_SERVER_SESSION_LEAK)
    if (p->alloc) {
        assert(0);
    }
#endif

    e = p->list.next;
    for (; e != &p->list;) {
        session = (ipc_server_session_t*)e;
        e       = e->next;

        nai_list_entry_remove(&session->ent);
        pool = &session->bufpool;
        nai_bufpool_close(pool);
    };

    r = 0;

    return r;
}

static ipc_server_session_t* ipc_server_session_pool_get(ipc_server_session_pool_t* p)
{
    /* get free message */
    if (p == NULL)
        return NULL;

    nai_list_entry_t* e;
    ipc_server_session_t* session = NULL;

    ipc_spinlock_lock(p->lock);

    e = p->list.next;
    if (e != &p->list) {
#if defined(IPC_SERVER_SESSION_LEAK)
        p->alloc++;
#endif
        nai_list_entry_remove(e);
        session = (ipc_server_session_t*)e;
    };

    ipc_spinlock_unlock(p->lock);

    return session;
}

static int ipc_server_session_pool_put(ipc_server_session_pool_t* p, ipc_server_session_t* session)
{
    /* release free message */
    if (p == NULL || session == NULL)
        return -1;

    ipc_server_session_dump(session, "ipc_server_session_pool_put");

    ipc_spinlock_lock(p->lock);
#if defined(IPC_SERVER_SESSION_LEAK)
    p->alloc--;
#endif
    nai_list_insert_head(&p->list, &session->ent);

    ipc_spinlock_unlock(p->lock);

    return 0;
}

ipc_server_session_t* ipc_server_session_create(ipc_packet_t* packet)
{
    int r, ec;
    ipc_server_session_t* session = NULL;
    ipc_server_session_pool_t* pool;
    nai_bufpool_t bufpool;

    if (packet == NULL)
        goto L_RETURN;

    pool = &ipc_global_server_session_pool;

    session = ipc_server_session_pool_get(pool);
    if (session == NULL) {
        r = nai_bufpool_create(&bufpool, pool ? pool->size : 0, 0);
        if (r < 0)
            goto L_RETURN;

        session = (ipc_server_session_t*)nai_bufpool_xalloc(&bufpool, sizeof(ipc_server_session_t));
        if (session == NULL) {
            ec = errno;
            nai_bufpool_close(&bufpool);
            errno = ec;
            goto L_RETURN;
        }
        memset(session, 0, sizeof(ipc_server_session_t));

        session->bufpool = bufpool;
        session->pool    = pool;

#if defined(IPC_SERVER_SESSION_LEAK)
        ipc_spinlock_lock(pool->lock);
        pool->alloc++;
        ipc_spinlock_unlock(pool->lock);
#endif
    }

    session->seq        = packet->seq;
    session->pid        = packet->pid;
    session->peer       = ipc_peer_ref(packet->peer);
    session->session_id = packet->session_id;

    ipc_server_session_dump(session, "ipc_server_session_pool_get");

L_RETURN:
    return session;
}

void ipc_server_session_end(ipc_server_session_t* session)
{
    nai_bufpool_t* bufpool;

    if (session) {
        ipc_peer_release(session->peer);

        if (session->pool) {
            ipc_server_session_pool_put(session->pool, session);
        } else {
            bufpool = &session->bufpool;
            nai_bufpool_close(bufpool);
        }
    }
}
