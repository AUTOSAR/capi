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
/// @file       packet.c
/// @brief
/// @details
/// @date       2022-09-20
/// @author     yangtao
/// @version    1.2.0
///
/// ================================================================

#include "packet.h"

#include "event.h"

int ipc_pktpool_init_flag                 = 0;
atomic_uintptr_t ipc_pktpool_init_counter = 0;
ipc_spinlock_t ipc_pktpool_init_flag_lock = IPC_SPINLOCK_INITIALIZER;

// Global packet pool for read and write
ipc_pktpool_t ipc_global_rpktpool;  // 0
ipc_pktpool_t ipc_global_wpktpool;

ipc_spinlock_t ipc_global_rpktpool_lock = IPC_SPINLOCK_INITIALIZER;
ipc_spinlock_t ipc_global_wpktpool_lock = IPC_SPINLOCK_INITIALIZER;

int ipc_pktpool_init(ipc_pktpool_t *p, size_t size, ipc_spinlock_t *lock)
{
    int r;

    nai_list_init(&p->list);
    p->lock  = lock;
    p->size  = size;
    p->alloc = 0;
    r        = 0;

    return r;
}

int ipc_pktpool_close(ipc_pktpool_t *p)
{
    int r;
    ipc_packet_t *packet;
    nai_list_entry_t *e;
    nai_bufpool_t pool;

#if defined(IPC_PACKET_LEAK)
    if (p->alloc) {
        assert(0);
    }
#endif

    e = p->list.next;
    for (; e != &p->list;) {
        packet = (ipc_packet_t *)e;
        e      = e->next;

        nai_list_entry_remove(&packet->ent);
        pool = *(packet->list.pool);
        nai_bufpool_close(&pool);
    };

    r = 0;

    return r;
}

int init_packet_module(void)
{
    int r = 0;

    ipc_spinlock_lock(&ipc_pktpool_init_flag_lock);

    if (atomic_fetch_add(&ipc_pktpool_init_counter, 1) == 0) {
        r = ipc_pktpool_init(&ipc_global_rpktpool, 64 * 1024, &ipc_global_rpktpool_lock);
        if (r) {
            goto L_RETURN;
        }

        r = ipc_pktpool_init(&ipc_global_wpktpool, 64 * 1024, &ipc_global_wpktpool_lock);
        if (r) {
            ipc_pktpool_close(&ipc_global_rpktpool);
            goto L_RETURN;
        }

        ipc_pktpool_init_flag = 1;
    }

L_RETURN:
    ipc_spinlock_unlock(&ipc_pktpool_init_flag_lock);

    return r;
}

int deinit_packet_module(void)
{
    int r = 0;

    ipc_spinlock_lock(&ipc_pktpool_init_flag_lock);

    if (atomic_fetch_sub(&ipc_pktpool_init_counter, 1) == 1) {
        r = ipc_pktpool_close(&ipc_global_wpktpool);
        if (r) {
            goto L_RETURN;
        }

        r = ipc_pktpool_close(&ipc_global_rpktpool);
        if (r) {
            goto L_RETURN;
        }

        ipc_pktpool_init_flag = 0;
    }

L_RETURN:
    ipc_spinlock_unlock(&ipc_pktpool_init_flag_lock);

    return r;
}

static ipc_packet_t *ipc_pktpool_get(ipc_pktpool_t *p)
{
    /* get free message */
    if (p == NULL)
        return NULL;

    nai_list_entry_t *e;
    ipc_packet_t *packet = NULL;

    ipc_spinlock_lock(p->lock);

    e = p->list.next;
    if (e != &p->list) {
#if defined(IPC_PACKET_LEAK)
        p->alloc++;
#endif
        nai_list_entry_remove(e);
        packet = (ipc_packet_t *)e;
    };

    ipc_spinlock_unlock(p->lock);

    return packet;
}

static int ipc_pktpool_put(ipc_pktpool_t *p, ipc_packet_t *packet)
{
    /* release free message */
    if (p == NULL || packet == NULL)
        return -1;

    ipc_packet_dump(packet, "ipc_pktpool_put");

    ipc_spinlock_lock(p->lock);
#if defined(IPC_PACKET_LEAK)
    p->alloc--;
#endif
    nai_list_insert_head(&p->list, &packet->ent);

    ipc_spinlock_unlock(p->lock);

    return 0;
}

ipc_packet_t *ipc_packet_create(int isWritePool)
{
    int r, ec;
    ipc_pktpool_t *pool;
    ipc_packet_t *packet;
    nai_bufpool_t bufpool;

    if (ipc_pktpool_init_flag == 0)
        return NULL;

    if (isWritePool)
        pool = &ipc_global_wpktpool;
    else
        pool = &ipc_global_rpktpool;

    packet = ipc_pktpool_get(pool);
    if (packet == NULL) {
        r = nai_bufpool_create(&bufpool, pool ? pool->size : 0, 0);
        if (r < 0)
            goto L_RETURN;

        packet = (ipc_packet_t *)nai_bufpool_xalloc(&bufpool, sizeof(ipc_packet_t) + sizeof(bufpool));
        if (packet == NULL) {
            ec = errno;
            nai_bufpool_close(&bufpool);
            errno = ec;
            goto L_RETURN;
        }
        memset(packet, 0, sizeof(ipc_packet_t));

        ipc_buffer_init_list(&packet->list, (nai_bufpool_t *)(packet + 1));
        packet->list.pool[0] = bufpool;
        packet->pool         = pool;

#if defined(IPC_PACKET_LEAK)
        ipc_spinlock_lock(pool->lock);
        pool->alloc++;
        ipc_spinlock_unlock(pool->lock);
#endif
    }

    atomic_init(&packet->ref_count, 0);
    packet->ref_packet = NULL;
    packet->pid        = 0;
    packet->session_id = 0;
    packet->peer       = NULL;
    packet->kind       = IPC_PACKET_KIND_NULL;
    packet->service    = 0;
    packet->model      = 0;
    packet->seq        = 0;

    ipc_packet_ref(packet);

    ipc_packet_dump(packet, "ipc_pktpool_get");

L_RETURN:
    return packet;
}

ipc_packet_t *ipc_packet_ref(ipc_packet_t *packet)
{
    if (packet == NULL)
        goto L_RETURN;

    atomic_fetch_add(&packet->ref_count, 1);

L_RETURN:
    return packet;
}

ipc_packet_t *ipc_packet_dup_payload(ipc_packet_t *packet, ipc_packet_t *duplicate_packet)
{
    ipc_packet_t *des;

    if (packet == NULL)
        goto L_RETURN;

    if (duplicate_packet->ref_packet) {
        des = duplicate_packet->ref_packet;
    } else {
        des = duplicate_packet;
    }
    nai_buflist_set_threading(&des->list);

    // dup buffers from des buffer list
    ipc_buffer_t *cur = ipc_packet_get_buffer(des);
    while (cur) {
        ipc_buffer_t *dup_buffer = ipc_buffer_dup(packet->list.pool, cur);
        ipc_buffer_append(&packet->list, dup_buffer);

        cur = ipc_buffer_get_next(des, cur);
    }

    packet->ref_packet = ipc_packet_ref(des);

L_RETURN:
    return packet;
}

void ipc_packet_release(ipc_packet_t *packet)
{
    nai_bufpool_t bufpool;
    if (packet == NULL)
        return;

    if (atomic_fetch_sub(&packet->ref_count, 1) == 1) {
        if (packet->peer != NULL) {
            ipc_peer_release(packet->peer);
        }
        if (packet->ref_packet != NULL) {
            ipc_packet_release(packet->ref_packet);
            packet->ref_packet = NULL;
        }

        if (packet->pool) {
            ipc_buffer_close_list(&packet->list);
            ipc_pktpool_put(packet->pool, packet);
        } else {
            bufpool = *(packet->list.pool);
            nai_bufpool_close(&bufpool);
        }
    }
}

ipc_buffer_t *ipc_packet_get_buffer(ipc_packet_t *packet)
{
    ipc_buffer_t *buf = NULL;

    if (packet == NULL)
        goto L_RETURN;

    buf = ipc_buffer_get_buffer(&packet->list);

L_RETURN:
    return buf;
}

ipc_buffer_t *ipc_buffer_get_next(ipc_packet_t *packet, ipc_buffer_t *cur)
{
    if (packet == NULL)
        return NULL;

    nai_list_entry_t *head = &(packet->list.ent);

    if (cur == NULL)
        return NULL;

    nai_buf_t *buf         = (nai_buf_t *)cur;
    nai_list_entry_t *next = buf->ent.next;

    if (next == head)
        return NULL;

    return (ipc_buffer_t *)next;
}

ipc_buffer_t *ipc_packet_append_buffer(ipc_packet_t *packet, size_t len)
{
    ipc_buffer_t *buf = NULL;

    if (packet == NULL || len <= 0 || len > UINT16_MAX)
        goto L_RETURN;

    buf = ipc_buffer_alloc(packet->list.pool, len);
    if (buf == NULL) {
        goto L_RETURN;
    }

    ipc_buffer_append(&packet->list, buf);

L_RETURN:
    return buf;
}

ipc_buffer_t *ipc_packet_append_user_buffer(ipc_packet_t *packet, void *buf, size_t len, ipc_free_func_t free)
{
    ipc_buffer_t *b = NULL;

    if (packet == NULL || buf == NULL || len <= 0)
        goto L_RETURN;

    b = ipc_buffer_alloc_from_user(packet->list.pool, buf, len, free);
    if (b == NULL) {
        goto L_RETURN;
    }

    ipc_buffer_append(&packet->list, b);

L_RETURN:
    return b;
}

pid_t ipc_packet_get_peer_pid(ipc_packet_t *packet)
{
    if (packet == NULL)
        return 0;

    return packet->pid;
}

ipc_session_id_t ipc_packet_get_session_id(ipc_packet_t *packet)
{
    if (packet == NULL)
        return 0;

    return packet->seq;
}