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
/// @file       buffer.c
/// @brief
/// @details
/// @date       2022-09-20
/// @author     yangtao
/// @version    1.2.0
///
/// ================================================================

#include "buffer.h"

int ipc_buffer_init_pool(nai_bufpool_t *pool, size_t size)
{
    nai_bufpool_init(pool);

    return nai_bufpool_create(pool, size, 0);
}

int ipc_buffer_from_pool(nai_bufpool_t *pool, nai_pool_t *parent) { return nai_bufpool_from(pool, parent, 0); }

int ipc_buffer_close_pool(nai_bufpool_t *pool) { return nai_bufpool_close(pool); }

ipc_buffer_t *ipc_buffer_alloc(nai_bufpool_t *pool, size_t len)
{
    nai_buf_t *buf;

    buf = nai_buf_alloc(pool, len);
    if (buf == NULL) {
        goto L_RETURN;
    }

L_RETURN:
    return (ipc_buffer_t *)buf;
}

typedef struct ipc_buffer_user_obj_s
{
    void *userdata;
    size_t len;
    ipc_free_func_t freefunc;
    nai_bufpool_t *pool;
} ipc_buffer_user_obj_t;

static int ipc_buffer_free_from_user(void *p)
{
    if (p == NULL)
        return -1;

    ipc_buffer_user_obj_t *obj = (ipc_buffer_user_obj_t *)p;
    void *userdata             = obj->userdata;
    ipc_free_func_t freefunc   = obj->freefunc;
    nai_bufpool_t *pool        = obj->pool;

    if (freefunc) {
        freefunc(NULL, userdata, obj->len);
    }

    nai_bufpool_xfree(pool, obj, sizeof(ipc_buffer_user_obj_t) + sizeof(nai_buf_ops_t));

    return 0;
}

ipc_buffer_t *ipc_buffer_alloc_from_user(nai_bufpool_t *pool, void *userdata, size_t len, ipc_free_func_t freefunc)
{
    nai_buf_t *buf             = NULL;
    ipc_buffer_user_obj_t *obj = NULL;
    nai_buf_ops_t *ops         = NULL;

    obj = (ipc_buffer_user_obj_t *)nai_bufpool_xalloc(pool, sizeof(ipc_buffer_user_obj_t) + sizeof(nai_buf_ops_t));
    if (obj == NULL) {
        goto L_RETURN;
    }
    obj->userdata = userdata;
    obj->len      = len;
    obj->freefunc = freefunc;
    obj->pool     = pool;

    ops       = (nai_buf_ops_t *)(obj + 1);
    ops->free = ipc_buffer_free_from_user;

    buf = nai_buf_from_object(pool, userdata, len, ops, (void *)obj);

L_RETURN:
    if (buf == NULL) {
        if (obj)
            nai_bufpool_xfree(pool, obj, sizeof(ipc_buffer_user_obj_t) + sizeof(nai_buf_ops_t));
    }

    return (ipc_buffer_t *)buf;
}

ipc_buffer_t *ipc_buffer_dup(nai_bufpool_t *pool, ipc_buffer_t *buf)
{
    if (buf == NULL)
        return NULL;

    nai_buf_t *b = nai_buf_dup(pool, (nai_buf_t *)buf, 1);

    return (ipc_buffer_t *)b;
}

int ipc_buffer_init_list(nai_buflist_t *list, nai_bufpool_t *pool)
{
    nai_buflist_init(list, pool);

    return 0;
}

ipc_buffer_t *ipc_buffer_get_buffer(nai_buflist_t *list)
{
    if (nai_buflist_is_empty(list)) {
        return NULL;
    }

    return (ipc_buffer_t *)(list->ent.next);
}

ipc_buffer_t *ipc_buffer_append(nai_buflist_t *list, ipc_buffer_t *buf)
{
    if (buf == NULL)
        return NULL;

    nai_buflist_insert_tail(list, (nai_buf_t *)buf);

    return buf;
}

int ipc_buffer_close_list(nai_buflist_t *list) { return nai_buflist_close(list); }

uint8_t *ipc_buffer_get_ptr(ipc_buffer_t *buf)
{
    if (buf == NULL)
        return NULL;

    nai_buf_t *b = (nai_buf_t *)buf;

    return (uint8_t *)nai_buf_ptr(b);
}

size_t ipc_buffer_get_len(ipc_buffer_t *buf)
{
    if (buf == NULL)
        return 0;

    nai_buf_t *b = (nai_buf_t *)buf;

    return (size_t)nai_buf_size(b);
}

int ipc_buffer_set_len(ipc_buffer_t *buf, size_t len)
{
    if (buf == NULL || len <= 0)
        return -1;

    nai_buf_t *b = (nai_buf_t *)buf;
    b->size      = len;

    return 0;
}

size_t ipc_buffer_get_size(ipc_buffer_t *buf)
{
    if (buf == NULL)
        return 0;

    nai_buf_t *b = (nai_buf_t *)buf;

    return (size_t)nai_buf_total(b);
}

int ipc_buffer_close(ipc_buffer_t *buf)
{
    if (buf == NULL)
        return 0;

    nai_buf_t *b = (nai_buf_t *)buf;

    return nai_buf_close(b);
}
