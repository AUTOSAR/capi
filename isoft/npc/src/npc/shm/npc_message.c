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
/// @file       npc_message.c
/// @brief
/// @details
/// @date       2022-08-29
/// @author     xn
/// @version    1.2.0
///
/// ================================================================

#include "npc/core/npc_message.h"
#include "npc/core/npc_log.h"
#include "nai/os/nai_thread.h"
#include "nai/runtime/nai_util.h"


int npc_message_ref(npc_message_t* m)
{
    int r;


    nai_atomic32_inc(&m->refc);
    r = 0;

    return r;
};



int npc_message_read_start(
    npc_message_io_t* p, npc_message_t* m, size_t offset)
{
    int r;
    nai_buf_t* b;
    nai_list_entry_t* e;


    e = m->payload.ent.next;
    for ( ; e != &m->payload.ent; e = e->next) {
        b = (nai_buf_t*)e;
        if (offset > nai_buf_size(b)) {
            offset -= nai_buf_size(b);
            continue;
        };

        break;
    };
    if (e != &m->payload.ent) {
        p->ptr = nai_buf_ptr(b) + offset;
        p->len = nai_buf_size(b) - offset;
        p->buf = b;
        p->msg = m;
    } else if (offset == 0) {
        p->ptr = 0;
        p->len = 0;
        p->buf = (nai_buf_t*)e;
        p->msg = m;
    } else {
        nai_errno = ERANGE;
        r = -1;
        goto _end;
    };

    r = 0;

_end:
    return r;
};


int npc_message_read_end(npc_message_io_t* p)
{
    int r;

    p->msg = 0;
    p->buf = 0;
    p->ptr = 0;
    p->len = 0;
    r = 0;

    return r;
};


intptr_t npc_message_read(npc_message_io_t* p, void* buf, size_t len)
{
    intptr_t r;
    size_t size;
    size_t total;
    nai_buf_t* b;
    nai_list_entry_t* e;
    npc_message_t* m;


    if (p->len <= 0) {
        total = 0;
    } else {

        size = p->len;
        if (size > len) {
            size = len;
        };

        if (buf != 0) {
            nai_memcpy(buf, p->ptr, size);
        };

        p->ptr += size;
        p->len -= size;
        if (size >= len) {
            r = size;
            goto _end;
        };

        total = size;
    };


    m = p->msg;
    while (total < len) {
        e = p->buf->ent.next;
        if (e == &m->payload.ent) {
            break;
        };

        b = (nai_buf_t*)e;

        size = nai_buf_size(b);
        if (size > len - total) {
            size = len - total;
        };

        if (buf != 0) {
            nai_memcpy((char*)buf + total, nai_buf_ptr(b), size);
        };

        total += size;
        p->buf = b;
        p->ptr = nai_buf_ptr(b) + size;
        p->len = nai_buf_size(b) - size;
        if (p->len > 0) {
            break;
        };
    };

    r = total;

_end:
    return r;
};


intptr_t npc_message_read_least(npc_message_io_t* p, void* buf, size_t len)
{
    intptr_t r;


    r = npc_message_read(p, buf, len);
    if (r >= 0 && r < (intptr_t)len) {
        nai_errno = EOVERFLOW;
        r = -1;
    };

    return r;
};


intptr_t npc_message_read_skip(npc_message_io_t* p, size_t len)
{
    return npc_message_read(p, 0, len);
};



int npc_message_write_start(
    npc_message_io_t* p, npc_message_t* m)
{
    int r;
    nai_buf_t* b;
    nai_list_entry_t* e;


    /* find the first space after the content */
    b = 0;
    e = m->payload.ent.prev;
    while (e != &m->payload.ent) {
        b = (nai_buf_t*)e;
        e = e->prev;

        /* check content size */
        if (nai_buf_size(b) > 0) {
            break;
        };

        /* we can't write a reference buf */
        if (b->typeref == NAI_BUF_REF) {
            break;
        };
    };

    if (b == 0) {
        p->buf = 0;
        p->ptr = 0;
        p->len = 0;
    } else {
        p->buf = b;
        p->ptr = nai_buf_ptr(b) + nai_buf_size(b);
        if (b->typeref != NAI_BUF_REF) {
            p->len = nai_buf_space(b);
        } else {
            /* we can't write a reference buf */
            p->len = 0;
        };
    };

    p->msg = m;
    r = 0;

    return r;
};


int npc_message_write_end(npc_message_io_t* p)
{
    int r;
    size_t size;
    nai_buf_t* b;


    b = p->buf;
    if (b) {
        size = p->ptr - nai_buf_ptr(b);
        size -= nai_buf_size(b);
        nai_buf_wcommit(b, size);
        p->msg->hdr.len += (uint32_t)size;
    };


    p->msg = 0;
    p->buf = 0;
    p->ptr = 0;
    p->len = 0;
    r = 0;

    return r;
};


static nai_buf_t* npc_message_alloc_buf(npc_message_t* m, size_t len)
{
    size_t size;
    nai_buf_t* b;


    size = len;
    if (size < 2000) {
        size = 2000;
    } else {
        size = nai_align(size, 4096);
        if (size - 48 >= len) {
            size -= 48;             /* space for metadata of allocator */
        } else {
            size = len;             /* space for metadata of allocator */
        };
    };

    b = nai_buf_alloc(m->payload.pool, size);
    if (b == 0) {
        goto _end;
    };

    nai_buflist_insert_tail(&m->payload, b);

_end:
    return b;
};


int npc_message_reserve(npc_message_io_t* p, size_t len)
{
    int r;
    size_t size;
    nai_buf_t* b;
    npc_message_t* m;


    size = p->len;
    m = p->msg;
    b = p->buf;
    if (b) {
        /* we need to make sure that the current position is pointing 
         * to the tail.
         */
        if (size > nai_buf_space(b)) {
            nai_errno = EINVAL;
            r = -1;
            goto _end;
        };

        while (b->ent.next != &m->payload.ent) {
            b = (nai_buf_t*)b->ent.next;
            size += nai_buf_space(b);
            if (size > len) {
                break;
            };
        };
    };

    if (size < len) {
        b = npc_message_alloc_buf(m, len - size);
        if (b == 0) {
            nai_log_alert(NPC_LOG_CORE, nai_errno, 
                "failed to allocate buffer when prealloc");

            r = -1;
            goto _end;
        };

        if (p->buf == 0) {
            p->buf = b;
            p->ptr = nai_buf_ptr(b) + nai_buf_size(b);
            p->len = nai_buf_space(b);
        };
    };

    r = 0;

_end:
    return r;
};


int npc_message_insert(npc_message_io_t* p, const void* buf, size_t len)
{
    int r;
    size_t size;
    nai_buf_t* b;
    nai_buf_t* d;
    npc_message_t* m;


    m = p->msg;
    b = p->buf;
    if (b) {
        size = p->ptr - nai_buf_ptr(b);
        if (size > nai_buf_size(b)) {
            size -= nai_buf_size(b);
            nai_buf_wcommit(b, size);
            m->hdr.len += (uint32_t)size;
        };
    };

    if (p->len <= 0) {
        /* no space, try to find next buffer */
        if (b != 0) {
            if (b->ent.next != &m->payload.ent) {
                b = (nai_buf_t*)b->ent.next;
                p->buf = b;
                p->ptr = nai_buf_ptr(b) + nai_buf_size(b);
                p->len = nai_buf_space(b);
            } else {
                b = 0;
            };
        };
    } else {
        size = p->ptr - nai_buf_ptr(b);
        if (size > 0) {
            /* cut buffer for insert 'buf' before space */
            b = nai_buf_cut(m->payload.pool, p->buf, size, 1);
            if (b == 0) {
                nai_log_alert(NPC_LOG_CORE, nai_errno, 
                    "failed to cut buffer when insert message");
                r = -1;
                goto _end;
            };

            b = p->buf;
        } else {
            /* nothing */
            ;
        };
    };

    d = nai_buf_from_rmemory(m->payload.pool, buf, len, 0);
    if (d == 0) {
        nai_log_alert(NPC_LOG_CORE, nai_errno, 
            "failed to allocate rbuffer when insert message");
        r = -1;
        goto _end;
    };

    if (b == 0) {
        nai_buflist_insert_tail(&m->payload, d);
        p->buf = d;
        p->ptr = nai_buf_ptr(d) + len;
        p->len = 0;
    } else {
        nai_buf_insert_before(b, d);
    };

    m->hdr.len += (uint32_t)len;
    r = 0;

_end:
    return r;
};


void *npc_message_buf_alloc(npc_message_t* m, size_t size)
{
    uint8_t *r;
    npc_message_t **tmp;
    nai_buf_t* b;


    b = nai_buf_alloc(m->payload.pool, size + sizeof(npc_message_t *) + 2);
    if (b == 0) {
        nai_log_alert(NPC_LOG_CORE, nai_errno,
            "failed to allocate buffer when start write message");
        r = 0;
        goto _end;
    };

    nai_buf_wcommit(b, size + sizeof(npc_message_t *) + 2);
    nai_buflist_insert_tail(&m->payload, b);

    tmp = (npc_message_t **)(nai_buf_ptr(b) + size + 2);
    *tmp = m;
    nai_buf_ptr(b)[size] = nai_buf_ptr(b)[size + 1] = 0;
    r = nai_buf_ptr(b);
    m->hdr.len = size;

_end:
    return r;
};

intptr_t npc_message_write(npc_message_io_t* p, const void* buf, size_t len)
{
    intptr_t r;
    size_t size;
    size_t total;
    nai_buf_t* b;
    npc_message_t* m;


    /* write to exists buffer */
    total = 0;
    m = p->msg;
    b = p->buf;
    while (b) {
        /* no space, try to find next buffer */
        if (p->len <= 0) {
            size = p->ptr - nai_buf_ptr(b);
            size -= nai_buf_size(b);
            nai_buf_wcommit(b, size);
            m->hdr.len += (uint32_t)size;

            if (b->ent.next == &m->payload.ent) {
                break;
            };

            b = (nai_buf_t*)b->ent.next;
            p->buf = b;
            p->ptr = nai_buf_ptr(b) + nai_buf_size(b);
            p->len = nai_buf_space(b);
        };

        size = p->len;
        if (size > len - total) {
            size = len - total;
        };

        nai_memcpy(p->ptr, buf, size);

        p->ptr += size;
        p->len -= size;
        total += size;
        if (total >= len) {
            r = total;
            goto _end;
        };
    };


    while (total < len) {

        b = npc_message_alloc_buf(m, len - total);
        if (b == 0) {
            nai_log_alert(NPC_LOG_CORE, nai_errno, 
                "failed to allocate buffer when write message");
            r = -1;
            goto _end;
        };

        size = nai_buf_total(b);
        if (size > len - total) {
            size = len - total;
        };

        nai_memcpy(nai_buf_ptr(b), (char*)buf + total, size);

        total += size;
        p->buf = b;
        p->ptr = nai_buf_ptr(b) + size;
        p->len = nai_buf_total(b) - size;
        if (nai_buf_total(b) > size) {
            break;
        };

        nai_buf_wcommit(b, size);
        m->hdr.len += (uint32_t)size;
    };

    r = total;

_end:
    return r;
};


intptr_t npc_message_overwrite(
    npc_message_io_t* p, const void* buf, size_t len)
{
    intptr_t r;
    size_t size;
    size_t total;
    nai_buf_t* b;
    npc_message_t* m;


    if (p->len <= 0) {
        total = 0;
    } else {

        size = p->len;
        if (size > len) {
            size = len;
        };

        nai_memcpy(p->ptr, buf, size);

        if (size >= len) {
            r = size;
            goto _end;
        };

        total = size;
    };

    m = p->msg;
    b = p->buf;
    if (b == 0) {
        if (nai_buflist_is_empty(&m->payload)) {
            nai_errno = EOVERFLOW;
            r = -1;
            goto _end;
        };

        b = (nai_buf_t*)&m->payload.ent;
    };

    while (total < len) {
        if (b->ent.next == &m->payload.ent) {
            nai_errno = EOVERFLOW;
            r = -1;
            goto _end;
        };

        b = (nai_buf_t*)b->ent.next;

        size = nai_buf_total(b);
        if (size > len - total) {
            size = len - total;
        };

        nai_memcpy(nai_buf_ptr(b), (char*)buf + total, size);

        total += size;
    };

    r = total;

_end:
    return r;
};

