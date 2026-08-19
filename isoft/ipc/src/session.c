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
/// @file       session.c
/// @brief
/// @details
/// @date       2022-09-20
/// @author     yangtao
/// @version    1.2.0
///
/// ================================================================

#include "session.h"

#include <stdlib.h>
#include <string.h>

#include "event.h"

static int ipc_session_compare(ipc_session_t* left, ipc_session_t* right)
{
    uintptr_t lo = (uintptr_t)left;
    uintptr_t ro = (uintptr_t)right;
    int result;
    if (lo < ro)
        result = -1;
    else if (lo > ro)
        result = 1;
    else
        result = 0;
    return result;
}

typedef RB_HEAD(ipc_session_table_s, ipc_session_s) ipc_session_table_t;
RB_GENERATE_INTERNAL(ipc_session_table_s, ipc_session_s, tree, ipc_session_compare, static)

typedef struct ipc_session_runtime_s
{
    ipc_spinlock_t session_lock;
    ipc_session_table_t session_table;  // client session table
} ipc_session_runtime_t;

static ipc_session_runtime_t rt = {
    .session_lock  = IPC_SPINLOCK_INITIALIZER,
    .session_table = RB_INITIALIZER(&rt.session_table),
};

ipc_session_t* ipc_session_table_get(ipc_session_t* session)
{
    if (session == NULL)
        return NULL;

    ipc_session_t* dummy = NULL;

    ipc_spinlock_lock(&rt.session_lock);
    dummy = RB_FIND(ipc_session_table_s, &rt.session_table, session);
    ipc_spinlock_unlock(&rt.session_lock);

    return dummy;
}

void ipc_session_table_insert(ipc_session_t* session)
{
    if (session == NULL)
        return;

    ipc_spinlock_lock(&rt.session_lock);
    RB_INSERT(ipc_session_table_s, &rt.session_table, session);
    ipc_spinlock_unlock(&rt.session_lock);
}

void ipc_session_table_remove(ipc_session_t* session)
{
    if (session == NULL)
        return;

    ipc_session_t* dummy = NULL;

    ipc_spinlock_lock(&rt.session_lock);
    dummy = RB_FIND(ipc_session_table_s, &rt.session_table, session);
    if (dummy)
        RB_REMOVE(ipc_session_table_s, &rt.session_table, session);
    ipc_spinlock_unlock(&rt.session_lock);
}

ipc_session_pool_t ipc_global_session_pool;
ipc_spinlock_t ipc_session_pool_lock = IPC_SPINLOCK_INITIALIZER;

int init_session_module(void)
{
    int r;
    ipc_session_pool_t* p = &ipc_global_session_pool;

    nai_list_init(&p->list);
    p->lock  = &ipc_session_pool_lock;
    p->size  = 64 * 1024;
    p->alloc = 0;
    r        = 0;

    return r;
}

int deinit_session_module(void)
{
    int r;
    ipc_session_pool_t* p = &ipc_global_session_pool;
    ipc_session_t* session;
    nai_list_entry_t* e;
    nai_bufpool_t* pool;

#if defined(IPC_SESSION_LEAK)
    if (p->alloc) {
        assert(0);
    }
#endif

    e = p->list.next;
    for (; e != &p->list;) {
        session = (ipc_session_t*)e;
        e       = e->next;

        nai_list_entry_remove(&session->ent);
        pool = &session->bufpool;
        nai_bufpool_close(pool);
    };

    r = 0;

    return r;
}

static ipc_session_t* ipc_session_pool_get(ipc_session_pool_t* p)
{
    /* get free message */
    if (p == NULL)
        return NULL;

    nai_list_entry_t* e;
    ipc_session_t* session = NULL;

    ipc_spinlock_lock(p->lock);

    e = p->list.next;
    if (e != &p->list) {
#if defined(IPC_SESSION_LEAK)
        p->alloc++;
#endif
        nai_list_entry_remove(e);
        session = (ipc_session_t*)e;
    };

    ipc_spinlock_unlock(p->lock);

    return session;
}

static int ipc_session_pool_put(ipc_session_pool_t* p, ipc_session_t* session)
{
    /* release free message */
    if (p == NULL || session == NULL)
        return -1;

    ipc_session_dump(session, "ipc_session_pool_put");

    ipc_spinlock_lock(p->lock);
#if defined(IPC_SESSION_LEAK)
    p->alloc--;
#endif
    nai_list_insert_head(&p->list, &session->ent);

    ipc_spinlock_unlock(p->lock);

    return 0;
}

void ipc_session_initialize(ipc_session_t* session) { memset(session, 0, sizeof(ipc_session_t)); }

ipc_session_t* ipc_session_create(void)
{
    int r, ec;
    ipc_session_t* session = NULL;
    ipc_session_pool_t* pool;
    nai_bufpool_t bufpool;

    pool = &ipc_global_session_pool;

    session = ipc_session_pool_get(pool);
    if (session == NULL) {
        r = nai_bufpool_create(&bufpool, pool ? pool->size : 0, 0);
        if (r < 0)
            goto L_RETURN;

        session = (ipc_session_t*)nai_bufpool_xalloc(&bufpool, sizeof(ipc_session_t));
        if (session == NULL) {
            ec = errno;
            nai_bufpool_close(&bufpool);
            errno = ec;
            goto L_RETURN;
        }

        ipc_session_initialize(session);

        session->bufpool = bufpool;
        session->pool    = pool;

#if defined(IPC_SESSION_LEAK)
        ipc_spinlock_lock(pool->lock);
        pool->alloc++;
        ipc_spinlock_unlock(pool->lock);
#endif
    }

    session->message    = NULL;
    session->request    = NULL;
    session->response   = NULL;
    session->client     = NULL;
    session->handler    = NULL;
    session->session_id = 0;
    session->context    = NULL;
    session->cvl        = NULL;
    session->cv         = NULL;
    session->deadline   = 0;
    memset(&session->timer, 0, sizeof(session->timer));
    session->status  = IPC_CLIENT_HANDLER_STATUS_ERR;
    session->errcode = 0;

    ipc_session_dump(session, "ipc_session_pool_get");

L_RETURN:
    return session;
}

void ipc_session_delete(ipc_session_t* session)
{
    nai_bufpool_t* bufpool;

    if (session) {
        if (session->pool) {
            ipc_session_pool_put(session->pool, session);
        } else {
            bufpool = &session->bufpool;
            nai_bufpool_close(bufpool);
        }
    }
}

int ipc_session_stop_timer(ipc_session_t* session)
{
    int status = -1;

    if (nai_evnode_close(&session->timer.node) == 0) {
        session->timer.handler = NULL;
        status                 = 0;
    }

    return status;
}

static int ipc_session_on_timeout(nai_evnode_t* node, int event)
{
    (void)event;

    ipc_session_t* session               = ipc_container_of(node, ipc_session_t, timer.node);
    ipc_session_timeout_handler* handler = session->timer.handler;
    ipc_session_stop_timer(session);
    handler(session->timer.context, session);
    return 0;
}

int ipc_session_start_timer(
    ipc_session_t* session, int timeout, nai_evloop_t* loop, ipc_session_timeout_handler* handler, void* context)
{
    int status         = -1;
    nai_evnode_t* node = &session->timer.node;

    if (loop == NULL)
        goto L_RETURN;

    if (nai_evnode_init(node) != 0)
        goto L_RETURN;
    if (nai_evnode_set_cb(node, ipc_session_on_timeout) != 0)
        goto L_RETURN;
    if (nai_evnode_set_timeout(node, NAI_TIMEOP_SET, timeout) != 0)
        goto L_RETURN;
    if (nai_evnode_open(node, loop) != 0)
        goto L_RETURN;
    session->timer.handler = handler;
    session->timer.context = context;
    status                 = 0;

L_RETURN:
    return status;
}

void ipc_session_end(ipc_session_t* session, int errcode)
{
    pthread_cond_t* cv     = session->cv;
    ipc_packet_t* response = NULL;

    errno = errcode;
    if (session->handler == NULL) {
        if (session->message->model == IPC_MESSAGE_REQUEST_CALL) {
            pthread_mutex_lock(session->cvl);
            session->errcode = errcode;
            session->cv      = NULL;
            pthread_cond_signal(cv);
            pthread_mutex_unlock(session->cvl);
        }
    } else {
        if (session->response) {
            response = session->response;
        }

        session->handler(session->context, session->status, response);
        ipc_session_delete(session);
    }
}
