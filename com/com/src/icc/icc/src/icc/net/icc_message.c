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
/// @file       icc_message.c
/// @brief
/// @details
/// @date       2025-11-27
/// @author     Zan Shigang
/// @version    1.2.0
///
/// ================================================================

#include "icc/net/icc_message.h"

#include "icc/core/icc_log.h"
#include "nai/os/nai_socket.h"
#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_util.h"

uint16_t icc_ntohs_ua(const uint16_t* p)
{
    uint16_t n;
    uint8_t* b = (uint8_t*)p;
#if (NAI_HAVE_BIG_ENDIAN)
    n = (b[1] << 8) | b[0];
#else
    n = (b[0] << 8) | b[1];
#endif
    return n;
};

uint32_t icc_ntohl_ua(const uint32_t* p)
{
    uint32_t n;
    uint8_t* b = (uint8_t*)p;
#if (NAI_HAVE_BIG_ENDIAN)
    n = (b[3] << 24) | (b[2] << 16) | (b[1] << 8) | b[0];
#else
    n = (b[0] << 24) | (b[1] << 16) | (b[2] << 8) | b[3];
#endif
    return n;
};

uint16_t icc_htons_ua(const uint16_t* p)
{
    uint16_t n;
    uint8_t* b = (uint8_t*)p;
#if (NAI_HAVE_BIG_ENDIAN)
    n = (b[1] << 8) | b[0];
#else
    n = (b[0] << 8) | b[1];
#endif
    return n;
};

uint32_t icc_htonl_ua(const uint32_t* p)
{
    uint32_t n;
    uint8_t* b = (uint8_t*)p;
#if (NAI_HAVE_BIG_ENDIAN)
    n = (b[3] << 24) | (b[2] << 16) | (b[1] << 8) | b[0];
#else
    n = (b[0] << 24) | (b[1] << 16) | (b[2] << 8) | b[3];
#endif
    return n;
};

int icc_msghdr_ntoh(icc_msghdr_t* d, const icc_msghdr_t* s)
{
    int r;

    d->serv      = nai_ntohs(s->serv);
    d->method    = nai_ntohs(s->method);
    d->len       = nai_ntohl(s->len);
    d->client    = nai_ntohs(s->client);
    d->session   = nai_ntohs(s->session);
    d->protocol  = s->protocol;
    d->interface = s->interface;
    d->type      = s->type;
    d->code      = s->code;
    r            = 0;

    return r;
};

int icc_msghdr_hton(icc_msghdr_t* d, const icc_msghdr_t* s)
{
    int r;

    d->serv      = nai_htons(s->serv);
    d->method    = nai_htons(s->method);
    d->len       = nai_htonl(s->len);
    d->client    = nai_htons(s->client);
    d->session   = nai_htons(s->session);
    d->protocol  = s->protocol;
    d->interface = s->interface;
    d->type      = s->type;
    d->code      = s->code;
    r            = 0;

    return r;
};

int icc_msghdr_ntoh_ua(icc_msghdr_t* d, const icc_msghdr_t* s)
{
    int r;

    d->serv      = icc_ntohs_ua(&s->serv);
    d->method    = icc_ntohs_ua(&s->method);
    d->len       = icc_ntohl_ua(&s->len);
    d->client    = icc_ntohs_ua(&s->client);
    d->session   = icc_ntohs_ua(&s->session);
    d->protocol  = s->protocol;
    d->interface = s->interface;
    d->type      = s->type;
    d->code      = s->code;
    r            = 0;

    return r;
};

int icc_msghdr_hton_ua(icc_msghdr_t* d, const icc_msghdr_t* s)
{
    int r;

    d->serv      = icc_htons_ua(&s->serv);
    d->method    = icc_htons_ua(&s->method);
    d->len       = icc_htonl_ua(&s->len);
    d->client    = icc_htons_ua(&s->client);
    d->session   = icc_htons_ua(&s->session);
    d->protocol  = s->protocol;
    d->interface = s->interface;
    d->type      = s->type;
    d->code      = s->code;
    r            = 0;

    return r;
};

    //////////////////////////////////////////////////////////////////////////////
    // message

#if 1 || defined(_DEBUG)
    #define ICC_MESSAGE_LEAK 1
#endif

int icc_msgpool_init(icc_msgpool_t* p, size_t size, nai_spin_t* lock)
{
    int r;

    nai_list_init(&p->list);
    p->lock  = lock;
    p->size  = size;
    p->alloc = 0;
    r        = 0;

    return r;
};

int icc_msgpool_close(icc_msgpool_t* p)
{
    int r;
    icc_message_t* m;
    nai_list_entry_t* e;
    nai_bufpool_t pool;

    if (p->alloc != 0) {
        nai_log_crit(ICC_LOG_CORE, 0, "the message object may be leaked, %d unreleased", (uint32_t)p->alloc);
        assert(0);
    };
    e = p->list.next;
    for (; e != &p->list;) {
        m = (icc_message_t*)e;
        e = e->next;

        nai_list_entry_remove(&m->ent);
        pool = *(m->payload.pool);
        nai_bufpool_close(&pool);
    };

    r = 0;

    return r;
};

icc_message_t* icc_message_create(icc_msgpool_t* p)
{
    int r;
    int ec;
    icc_message_t* m;
    nai_list_entry_t* e;
    nai_spin_t* lock;
    nai_bufpool_t pool;

    /* find free message */
    if (p != 0) {
        lock = p->lock;
        if (lock == 0) {
            e = p->list.next;
            if (e != &p->list) {
#if defined(ICC_MESSAGE_LEAK)
                p->alloc++;
#endif
                nai_list_entry_remove(e);

                m        = (icc_message_t*)e;
                m->refc  = 1;
                m->flags = 0;
                goto _end;
            };
        } else {
            nai_spin_lock(lock);

            e = p->list.next;
            if (e != &p->list) {
#if defined(ICC_MESSAGE_LEAK)
                p->alloc++;
#endif
                nai_list_entry_remove(e);
                nai_spin_unlock(lock);

                m        = (icc_message_t*)e;
                m->refc  = 1;
                m->flags = 0;
                goto _end;
            };

            nai_spin_unlock(lock);
        };
    };

    /* allocate a new message */
    r = nai_bufpool_create(&pool, p ? p->size : 0, 0);
    if (r < 0) {
        nai_log_alert(ICC_LOG_CORE, nai_errno, "failed to allocate buffer pool of message");
        m = 0;
        goto _end;
    };

    m = (icc_message_t*)nai_bufpool_malloc(&pool, sizeof(*m) + sizeof(pool));
    if (m == 0) {
        ec = nai_errno;
        nai_log_alert(ICC_LOG_CORE, ec, "failed to allocate message from buffer pool");
        nai_bufpool_close(&pool);
        nai_errno = ec;
        goto _end;
    };

    nai_memset(&m->hdr, 0, sizeof(m->hdr));
    nai_buflist_init(&m->payload, (nai_bufpool_t*)(m + 1));
    m->payload.pool[0] = pool;
    m->pool            = p;
    m->refm            = 0;
    m->refc            = 1;
    m->inst            = 0;
    m->flags           = 0;

#if defined(ICC_MESSAGE_LEAK)
    if (p != 0) {
        lock = p->lock;
        if (lock != 0) {
            nai_spin_lock(lock);
        };
        p->alloc++;
        if (lock != 0) {
            nai_spin_unlock(lock);
        };
    };
#endif

_end:
    return m;
};

icc_message_t* icc_message_dup(icc_message_t* m, int mt)
{
    int ec;
    icc_message_t* r;
    icc_message_t* o;
    nai_buf_t* b;
    nai_buf_t* d;
    nai_list_entry_t* e;

    r = icc_message_create(m->pool);
    if (r == 0) {
        goto _end;
    };

    if (m->refm == 0) {
        o = m;
    } else {
        o = m->refm;
    };

    /* dup payload */
    e = o->payload.ent.next;
    for (; e != &o->payload.ent; e = e->next) {
        b = (nai_buf_t*)e;
        if (nai_buf_size(b) <= 0) {
            continue;
        };

        d = nai_buf_dup(r->payload.pool, b, 0);
        if (d == 0) {
            ec = nai_errno;
            nai_log_alert(ICC_LOG_CORE, ec, "failed to duplicate the payload of message");
            icc_message_close(r);
            nai_errno = ec;
            r         = 0;
            goto _end;
        };

        nai_buflist_insert_tail(&r->payload, d);
    };
    if (mt) {
        nai_buflist_set_threadsafe(&r->payload);
    };

    /* copy meta data */
    r->inst  = o->inst;
    r->hdr   = o->hdr;
    r->flags = o->flags;

    /* ref */
    r->refm = o;
    nai_atomic32_inc(&o->refc);

_end:
    return r;
};

int icc_message_ref(icc_message_t* m)
{
    int r;

    nai_atomic32_inc(&m->refc);
    r = 0;

    return r;
};

int icc_message_close(icc_message_t* m)
{
    int r;
    nai_spin_t* lock;
    icc_msgpool_t* p;
    nai_bufpool_t pool;

    if (nai_atomic32_dec(&m->refc) > 0) {
        r = 0;
        goto _end;
    };

    /* should free payload before close reference message */
    nai_buflist_close(&m->payload);

    if (m->refm) {
        icc_message_close(m->refm);
        m->refm = 0;
    };

    p = m->pool;
    if (p != 0) {
        lock = p->lock;
        if (lock) {
            nai_spin_lock(lock);
        };

#if defined(ICC_MESSAGE_LEAK)
        p->alloc--;
#endif
        nai_list_insert_head(&p->list, &m->ent);

        if (lock) {
            nai_spin_unlock(lock);
        };

        r = 0;
        goto _end;
    };

    pool = *(m->payload.pool);
    r    = nai_bufpool_close(&pool);

_end:
    return r;
};

int icc_message_read_start(icc_message_io_t* p, icc_message_t* m, size_t offset)
{
    int r;
    nai_buf_t* b;
    nai_list_entry_t* e;

    e = m->payload.ent.next;
    for (; e != &m->payload.ent; e = e->next) {
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
        r         = -1;
        goto _end;
    };

    r = 0;

_end:
    return r;
};

int icc_message_read_end(icc_message_io_t* p)
{
    int r;

    p->msg = 0;
    p->buf = 0;
    p->ptr = 0;
    p->len = 0;
    r      = 0;

    return r;
};

intptr_t icc_message_read(icc_message_io_t* p, void* buf, size_t len)
{
    intptr_t r;
    size_t size;
    size_t total;
    nai_buf_t* b;
    nai_list_entry_t* e;
    icc_message_t* m;

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

intptr_t icc_message_read_least(icc_message_io_t* p, void* buf, size_t len)
{
    intptr_t r;

    r = icc_message_read(p, buf, len);
    if (r >= 0 && r < (intptr_t)len) {
        nai_errno = EOVERFLOW;
        r         = -1;
    };

    return r;
};

intptr_t icc_message_read_skip(icc_message_io_t* p, size_t len) { return icc_message_read(p, 0, len); };

int icc_message_write_start(icc_message_io_t* p, icc_message_t* m)
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
    r      = 0;

    return r;
};

int icc_message_write_end(icc_message_io_t* p)
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
    r      = 0;

    return r;
};

static nai_buf_t* icc_message_alloc_buf(icc_message_t* m, size_t len)
{
    size_t size;
    nai_buf_t* b;

    size = len;
    if (size < 2000) {
        size = 2000;
    } else {
        size = nai_align(size, 4096);
        if (size - 48 >= len) {
            size -= 48; /* space for metadata of allocator */
        } else {
            size = len; /* space for metadata of allocator */
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

int icc_message_reserve(icc_message_io_t* p, size_t len)
{
    int r;
    size_t size;
    nai_buf_t* b;
    icc_message_t* m;

    size = p->len;
    m    = p->msg;
    b    = p->buf;
    if (b) {
        /* we need to make sure that the current position is pointing 
         * to the tail.
         */
        if (size > nai_buf_space(b)) {
            nai_errno = EINVAL;
            r         = -1;
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
        b = icc_message_alloc_buf(m, len - size);
        if (b == 0) {
            nai_log_alert(ICC_LOG_CORE, nai_errno, "failed to allocate buffer when prealloc");

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

int icc_message_insert(icc_message_io_t* p, const void* buf, size_t len)
{
    int r;
    size_t size;
    nai_buf_t* b;
    nai_buf_t* d;
    icc_message_t* m;

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
                b      = (nai_buf_t*)b->ent.next;
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
                nai_log_alert(ICC_LOG_CORE, nai_errno, "failed to cut buffer when insert message");
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
        nai_log_alert(ICC_LOG_CORE, nai_errno, "failed to allocate rbuffer when insert message");
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

intptr_t icc_message_write(icc_message_io_t* p, const void* buf, size_t len)
{
    intptr_t r;
    size_t size;
    size_t total;
    nai_buf_t* b;
    icc_message_t* m;

    /* write to exists buffer */
    total = 0;
    m     = p->msg;
    b     = p->buf;
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

            b      = (nai_buf_t*)b->ent.next;
            p->buf = b;
            p->ptr = nai_buf_ptr(b) + nai_buf_size(b);
            p->len = nai_buf_space(b);
        };

        size = p->len;
        if (size > len - total) {
            size = len - total;
        };

        nai_memcpy(p->ptr, (char*)buf + total, size);

        p->ptr += size;
        p->len -= size;
        total += size;
        if (total >= len) {
            r = total;
            goto _end;
        };
    };

    while (total < len) {
        b = icc_message_alloc_buf(m, len - total);
        if (b == 0) {
            nai_log_alert(ICC_LOG_CORE, nai_errno, "failed to allocate buffer when write message");
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

intptr_t icc_message_overwrite(icc_message_io_t* p, const void* buf, size_t len)
{
    intptr_t r;
    size_t size;
    size_t total;
    nai_buf_t* b;
    icc_message_t* m;

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
            r         = -1;
            goto _end;
        };

        b = (nai_buf_t*)&m->payload.ent;
    };

    while (total < len) {
        if (b->ent.next == &m->payload.ent) {
            nai_errno = EOVERFLOW;
            r         = -1;
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
