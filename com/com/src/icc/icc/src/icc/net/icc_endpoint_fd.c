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
/// @file       icc_endpoint_fd.c
/// @brief
/// @details
/// @date       2025-11-27
/// @author     Zan Shigang
/// @version    1.2.0
///
/// ================================================================

#include "icc/core/icc_log.h"
#include "icc_network.h"
#include "nai/io/nai_io.h"
#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_util.h"

typedef struct icc_stream_s
{
    /* header some as nsi_iobase_t */
    nai_stream_t io;

    /* endpoint */
    icc_endpoint_t* ep;

    /* flags and stats */
    union
    {
        struct
        {
            uint32_t server : 1;
            uint32_t connected : 1;
            uint32_t posted : 1;
            uint32_t read : 2;
            uint32_t reconnect : 1;
            uint32_t closing : 1;
            uint32_t retry : 8;
        };
        uint32_t flags;
    };

    /* for read */
    struct
    {
        nai_buf_t* buf;          /**< pointer to the read buffer */
        icc_message_t* incoming; /**< pointer to the incoming message */
        size_t msglen;           /**< the left size of message to receive */
    } in;

    /* for send */
    struct
    {
        nai_buflist_t bufs;       /**< the list of sending buffers */
        nai_bufwalk_t walk;       /**< the buf walk */
        nai_list_entry_t queued;  /**< the list of sending messages */
        nai_list_entry_t waiting; /**< the list of waiting messages */
        size_t qsize;             /**< the size of queued in bytes */
        size_t bsize;             /**< the size of buffer vectors */
        int16_t mcount;           /**< the count of sending messages */
        int16_t vcount;           /**< the count of buffer in sending */
        int16_t vnext;            /**< the next index of buffer vector */
        int16_t vmore;            /**< is no more buffer vector */
        nai_bufvec_t va[NAI_BUFV_MAX];
    } out;
} icc_stream_t;

static int icc_stream_realloc_buf(icc_stream_t* c) { return icc_endpoint_realloc_buf(c->ep, &c->in.buf); };

static int icc_stream_handle_incoming(icc_stream_t* c, icc_message_t* m)
{
    int r;

    r = icc_endpoint_handle_incoming(c->ep, m);

    return r;
};

static int icc_stream_handle_sent(icc_stream_t* c, nai_list_entry_t* list, int errcode)
{
    int r;

    r = icc_endpoint_handle_sent(c->ep, list, &c->out.qsize, errcode);

    return r;
};

static int icc_stream_handle_send(icc_stream_t* c)
{
    int ec;
    int len;
    intptr_t r;
    icc_message_t* m;
    nai_bufvec_t* v;
    nai_bufarray_t va;

    while (1) {
        while (c->out.mcount > 0) {
            assert(!nai_buflist_is_empty(&c->out.bufs));
            assert(!nai_list_is_empty(&c->out.queued));

            while (c->out.vnext < c->out.vcount) {
                r = nai_stream_writev(&c->io, c->out.va + c->out.vnext, c->out.vcount - c->out.vnext);
                if (r <= 0) {
                    ec = nai_errno;
                    if (ec == NAI_EAGAIN || ec == NAI_EINPROGRESS) {
                        r = NAI_DECLINED;
                        goto _end;
                    };

                    nai_log_crit(ICC_LOG_CORE, ec, "endpoint(%d:%s) send failed", c->ep->icc_type,
                                 nai_str(&c->ep->icc_identifier));
                    goto _end;
                };

                c->out.bsize -= r;
                for (; c->out.vnext < c->out.vcount;) {
                    v = &c->out.va[c->out.vnext];
                    if (r < (intptr_t)v->len) {
                        v->len -= r;
                        v->buf += r;
                        break;
                    };

                    r -= v->len;
                    c->out.vnext++;
                };

                if (c->out.vnext >= c->out.vcount) {
                    c->out.vcount = 0;
                } else {
                    if (c->out.bsize > 256 * 1024 || c->out.vmore == 0) {
                        continue;
                    };

                    if (c->out.vnext) {
                        c->out.vcount -= c->out.vnext;
                        nai_memmove(c->out.va, c->out.va + c->out.vnext, c->out.vcount * sizeof(c->out.va[0]));
                    };
                };

                break;
            };

            r = nai_bufwalk_to_rbufarray(&c->out.walk, &va, c->out.va + c->out.vcount,
                                         nai_countof(c->out.va) - c->out.vcount, 1024 * 1024 - c->out.bsize, 0);
            if (r < 0) {
                ec = nai_errno;
                nai_log_alert(ICC_LOG_CORE, ec, "stream endpoint(%d:%s) get send bufarray failed", c->ep->icc_type,
                              nai_str(&c->ep->icc_identifier));

                goto _end;
            };

            if (r > 0) {
                c->out.vcount += (int)va.count;
                c->out.vnext = 0;
                c->out.bsize += r;
                continue;
            };
            if (c->out.bsize > 0) {
                assert(c->out.vcount > 0);
                assert(c->out.vmore != 0);
                c->out.vnext = 0;
                c->out.vmore = 0;
                continue;
            };

            assert(c->out.vcount <= 0);
            c->out.mcount = 0;
            nai_buflist_close(&c->out.bufs);
            r = icc_stream_handle_sent(c, &c->out.queued, 0);
            if (r != NAI_DECLINED) {
                goto _end;
            };
        };

        while (c->out.mcount < 8 && !nai_list_is_empty(&c->out.waiting)) {
            m   = (icc_message_t*)c->out.waiting.next;
            len = m->hdr.len;
            /* add message to output queue */
            nai_list_entry_remove(&m->ent);
            nai_list_insert_tail(&c->out.queued, &m->ent);

            /* move payload to output buffers */
            c->out.bufs.pool = m->payload.pool;
            r                = nai_buflist_move_last(&c->out.bufs, &m->payload, len - 4, 0, 0);
            if (r < 0) {
                ec = nai_errno;
                nai_log_alert(ICC_LOG_CORE, ec, "endpoint(%d:%s) failed to move buffer", c->ep->icc_type,
                              nai_str(&c->ep->icc_identifier));
                goto _end;
            };
            if (r != len) {
                assert(0);
            };
            c->out.mcount++;
        };

        if (c->out.mcount <= 0) {
            break;
        };

        c->out.vcount = 0;
        c->out.vnext  = 0;
        c->out.vmore  = 1;
        c->out.bsize  = 0;
        nai_bufwalk_init(&c->out.walk, &c->out.bufs);
    };

    r = NAI_DECLINED;

_end:
    return (int)r;
};

static int icc_stream_send(icc_endpoint_t* p, icc_message_t* m, uint32_t uid, int flags)
{
    int r;
    icc_stream_t* c;

    (void)uid;
    (void)flags;

    r = 0;
    c = (icc_stream_t*)p->ctx;

    if (c->out.qsize >= 10 * 1024 * 1024) {
        nai_log_alert(ICC_LOG_CORE, nai_errno, "endpoint(%d:%s) reach queue size limit", c->ep->icc_type,
                      nai_str(&c->ep->icc_identifier));
        icc_message_close(m);
    };

    c->out.qsize += m->hdr.len;
    nai_list_insert_tail(&c->out.waiting, &m->ent);
    if (c->out.mcount <= 0 && nai_stream_is_writable(&c->io)) {
        do {
            r = icc_stream_handle_send(c);

        } while (0);
    };

    return r;
};

static int icc_stream_handle_read(icc_stream_t* c)
{
    int ec;
    size_t needspace;
    intptr_t r;
    uint32_t len;
    size_t size;
    size_t total;
    nai_buf_t* b;
    nai_buf_t* d;
    icc_message_t* m;
    icc_network_t* n;
    icc_endpoint_t* ep;

    if (c->in.buf == 0) {
        r = icc_stream_realloc_buf(c);
        if (r < 0) {
            nai_log_alert(ICC_LOG_CORE, nai_errno, "endpoint(%d:%s) failed to create read buffer", c->ep->icc_type,
                          nai_str(&c->ep->icc_identifier));
            goto _alert;
        };
    };

    ep        = c->ep;
    n         = ep->net;
    b         = c->in.buf;
    needspace = 4;
    while (1) {
        size  = nai_buf_size(b);
        total = nai_buf_total(b);
        assert(total > size);
        if (total - size < needspace + 60) {
            r = icc_stream_realloc_buf(c);
            if (r < 0) {
                ec = nai_errno;
                nai_log_alert(ICC_LOG_CORE, ec, "endpoint(%d:%s) failed to create read buffer", c->ep->icc_type,
                              nai_str(&c->ep->icc_identifier));
                goto _alert;
            };
            b     = c->in.buf;
            size  = nai_buf_size(b);
            total = nai_buf_total(b);
        };
        r = nai_stream_read(&c->io, nai_buf_ptr(b) + size + needspace, total - size - needspace);
        if (r < 0) {
            ec = nai_errno;
            if (ec == NAI_EAGAIN || ec == NAI_EINPROGRESS) {
                r = NAI_DECLINED;
                goto _end;
            };

            nai_log_crit(ICC_LOG_CORE, ec, "endpoint(%d:%s) receive failed", c->ep->icc_type,
                         nai_str(&c->ep->icc_identifier));
            goto _error;
        };
        if (r == 0) {
            nai_log_debug(ICC_LOG_CORE, 0, "endpoint(%d:%s) receive(%d) to the end", c->ep->icc_type,
                          nai_str(&c->ep->icc_identifier), (uint32_t)(total - size));

            goto _end;
        };
        len = r;
        len = icc_htonl_ua(&len);
        nai_memcpy(nai_buf_ptr(b) + size, &len, 4);
        size += 4;
        size += r;
        nai_buf_wcommit(b, r + 4);

        while (1) {
            c->in.msglen = r + 4;
            m = c->in.incoming = icc_network_create_message(n, ICC_USAGE_READ);
            if (m == 0) {
                nai_log_alert(ICC_LOG_CORE, nai_errno, "endpoint(%d:%s) failed to create message", c->ep->icc_type,
                              nai_str(&c->ep->icc_identifier));
                goto _alert;
            };

            m->hdr.serv     = ep->serv;
            m->inst         = ep->inst;
            m->hdr.protocol = 0;

            d = nai_buf_sub(m->payload.pool, b, 0, c->in.msglen, 1);
            if (d == 0) {
                nai_log_alert(ICC_LOG_CORE, nai_errno,
                              "stream endpoint(%d:%s) "
                              "failed to create sub-buffer",
                              c->ep->icc_type, nai_str(&c->ep->icc_identifier));
                goto _alert;
            };

            nai_buf_rcommit(b, c->in.msglen);
            nai_buflist_insert_tail(&m->payload, d);

            m->hdr.len = c->in.msglen;

            size -= c->in.msglen;
            c->in.msglen   = 0;
            c->in.incoming = 0;

            r = icc_stream_handle_incoming(c, m);
            if (r != NAI_DECLINED) {
                goto _end;
            };

            if (size == 0) {
                break;
            };
        };
    };

    r = NAI_DECLINED;

_end:
    return (int)r;

_error:
    goto _end;

_alert:
    goto _end;
};

static int icc_stream_handle(nai_stream_t* p, int events)
{
    int r;
    icc_stream_t* c;

    r = 0;
    c = (icc_stream_t*)p;

    (void)c;

    if (events & (NAI_EV_READ | NAI_EV_WRITE)) {
        if (events & NAI_EV_TIMEOUT) {
            goto _end;
        };

        if (events & NAI_EV_READ) {
            r = icc_stream_handle_read(c);
            if (r != NAI_DECLINED) {
                goto _end;
            };
        };

        if (events & NAI_EV_WRITE) {
            //r = nsi_stream_handle_send(c);
            if (r != NAI_DECLINED) {
                goto _end;
            };
        };

    } else {
        switch (events & NAI_EV_MASK) {
            case NAI_EV_NOTIFY:
                switch (nai_ev_notify_code(events)) {
                    case NAI_EV_WRITE:
                        //r = nsi_stream_handle_send(c);
                        goto _end;
                    case NAI_EV_READ:
                        r = icc_stream_handle(&c->io, NAI_EV_READ);
                        goto _end;
                    default:
                        break;
                };

                break;

            default:
                /* nothing */
                break;
        };
    };

    r = NAI_DECLINED;

_end:
    return r;
};

static int icc_stream_create(icc_endpoint_t* e)
{
    int r;
    int ec;
    icc_stream_t* c;
    nai_fd_t fd;

    c = (icc_stream_t*)icc_network_alloc(e->net, sizeof(*c));
    if (c == 0) {
        nai_log_alert(ICC_LOG_CORE, nai_errno, "allocate stream endpoint(%d:%s) failed", e->icc_type,
                      nai_str(&e->icc_identifier));
        r = -1;
        goto _end;
    };

    c->ep = e;

    c->flags       = 0;
    c->in.buf      = 0;
    c->in.incoming = 0;
    c->in.msglen   = 0;

    c->out.qsize  = 0;
    c->out.mcount = 0;
    c->out.vcount = 0;
    c->out.vnext  = 0;

    nai_buflist_init(&c->out.bufs, 0);
    nai_list_init(&c->out.queued);
    nai_list_init(&c->out.waiting);

    nai_stream_init(&c->io);
    nai_stream_set_cb(&c->io, icc_stream_handle);

    fd = nai_file_open(nai_str(&e->icc_identifier), O_RDWR);
    if (fd == NAI_FD_INVALID) {
        nai_log_alert(ICC_LOG_CORE, nai_errno, "endpoint(%d:%s) create failed", e->icc_type,
                      nai_str(&e->icc_identifier));
        r = -1;
        goto _end;
    };

    nai_iobase_set_mode(&c->io, NAI_IO_READWRITE);
    nai_iobase_set_fd(&c->io, fd, NAI_FD_TYPE_DEVC);
    nai_iobase_set_fdown(&c->io, 1);

    r = nai_stream_open(&c->io, c->ep->net->loop);
    if (r < 0) {
        ec = nai_errno;
        nai_log_alert(ICC_LOG_CORE, ec, "endpoint(%d:%s) failed to open stream", e->icc_type,
                      nai_str(&e->icc_identifier));
        r = -1;
        goto _end;
    };

    e->ctx       = c;
    e->connected = 1;
    r            = 0;

_end:
    return r;
};

int icc_fd_endpoint_open(icc_endpoint_t* e)
{
    icc_stream_create(e);
    return 0;
};

int icc_fd_endpoint_send(icc_endpoint_t* e, icc_message_t* m, uint32_t uid, int flags)
{
    int r;

    r = icc_stream_send(e, m, uid, flags);

    return r;
};

int icc_fd_endpoint_flush(icc_endpoint_t* e)
{
    (void)e;
    return 0;
};

int icc_fd_endpoint_close(icc_endpoint_t* e)
{
    (void)e;
    return 0;
};

icc_endpoint_ops_t icc_endpoint_fd_ops
    = {icc_fd_endpoint_open, icc_fd_endpoint_send, icc_fd_endpoint_flush, icc_fd_endpoint_close};
