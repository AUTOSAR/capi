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
/// @file       nai_agent_io.c
/// @brief      
/// @details
/// @date       2021-04-18
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai_agent_io.h"
#include "nai/io/nai_dns.h"
#include "nai/runtime/nai_uri.h"
#include "nai/runtime/nai_log.h"


//////////////////////////////////////////////////////////////////////////////
// pseudo iobase functions



static nai_int_t nai_agent_iobase_open(nai_iobase_t* s, nai_evloop_t* l)
{
    (void)s;
    (void)l;

    return 0;
};


static nai_int_t nai_agent_iobase_bind(
    nai_iobase_t* s, nai_evloop_t* l, 
    const nai_sockaddr_t* name, nai_int_t namelen)
{
    (void)s;
    (void)l;
    (void)name;
    (void)namelen;

    return 0;
};


static nai_int_t nai_agent_iobase_connect(
    nai_iobase_t* s, nai_evloop_t* l, 
    const nai_sockaddr_t* name, nai_int_t namelen)
{
    (void)s;
    (void)l;
    (void)name;
    (void)namelen;

    return 0;
};


static nai_int_t nai_agent_iobase_close(nai_iobase_t* s)
{
    nai_int_t r;
    nai_iobase_t* c;
    nai_iobase_ops_t* ops;
    nai_agent_context_t* p;


    c = (nai_iobase_t*)s;
    p = (nai_agent_context_t*)c->agent;
    ops = (nai_iobase_ops_t*)p->ops;
    if (ops) {
        r = ops->close(s);
        if (r < 0) {
            goto _end;
        };

        p->ops = 0;
    } else {
        r = nai_evnode_close(&c->ev);
        if (r < 0) {
            goto _end;
        };
    };

    c->st.type = 0;
    c->st.ops = 0;
    r = 0;

_end:
    return r;
};


static nai_int_t nai_agent_iobase_setopt(
    nai_iobase_t* s, nai_int_t opt, intptr_t value)
{
    nai_int_t r;
    nai_iobase_t* c;
    nai_iobase_ops_t* ops;
    nai_agent_context_t* p;
#if (NAI_HAVE_SSL)
    nai_agent_buf_t* b;
#endif


    c = (nai_iobase_t*)s;
    p = (nai_agent_context_t*)c->agent;
    switch (opt) {
    case NAI_IO_LOADFILE:
#if (NAI_HAVE_SSL)
        if (p->ssl) {
            b = (nai_agent_buf_t*)p->ctx;
            r = nai_ssl_enable_load(b->ssl, value);
            break;
        };
#endif
        /* fallthrough */

    default:
        ops = (nai_iobase_ops_t*)p->ops;
        if (ops) {
            r = ops->setopt(s, opt, value);
            goto _end;
        };

        r = nai_iobase_default_setopt(s, opt, value);
        break;
    };

_end:
    return r;
};


static nai_int_t nai_agent_iobase_getopt(
    nai_iobase_t* s, nai_int_t opt, intptr_t* value)
{
    nai_int_t r;
    nai_iobase_t* c;
    nai_iobase_ops_t* ops;
    nai_agent_context_t* p;
#if (NAI_HAVE_SSL)
    nai_agent_buf_t* b;
#endif


    c = (nai_iobase_t*)s;
    p = (nai_agent_context_t*)c->agent;
    switch (opt) {
    case NAI_IO_LOADFILE:
#if (NAI_HAVE_SSL)
        if (p->ssl) {
            b = (nai_agent_buf_t*)p->ctx;
            value[0] = b->ssl->loadfile;
            r = 0;
            break;
        };
#endif
        /* fallthrough */

    case NAI_IO_FEAT_VECTORIO:
#if (NAI_HAVE_SSL)
        if (p->ssl) {
            value[0] = 0;
            r = 0;
            break;
        };
#endif
        /* fallthrough */

    case NAI_IO_FEAT_SENDFILE:
#if (NAI_HAVE_SSL)
        if (p->ssl && !nai_ssl_support_ktls()) {
            value[0] = 0;
            r = 0;
            break;
        };
#endif
        /* fallthrough */

    default:
        ops = (nai_iobase_ops_t*)p->ops;
        if (ops) {
            r = ops->getopt(s, opt, value);
            goto _end;
        };

        r = nai_iobase_default_getopt(s, opt, value);
        break;
    };

_end:
    return r;
};


static nai_int_t nai_agent_stream_no_shutdown(nai_stream_t* s, nai_int_t how)
{
    (void)s;
    (void)how;

    nai_errno = ENOTCONN;
    return -1;
};

static intptr_t nai_agent_stream_no_read(
    nai_stream_t* s, void* buf, size_t len)
{
    (void)s;
    (void)buf;
    (void)len;

    nai_errno = ENOTCONN;
    return -1;
};

static intptr_t nai_agent_stream_no_readv(
    nai_stream_t* s, nai_bufvec_t* v, nai_int_t count)
{
    (void)s;
    (void)v;
    (void)count;

    nai_errno = ENOTCONN;
    return -1;
};

static intptr_t nai_agent_stream_no_write(
    nai_stream_t* s, const void* buf, size_t len)
{
    (void)s;
    (void)buf;
    (void)len;

    nai_errno = ENOTCONN;
    return -1;
};

static intptr_t nai_agent_stream_no_writev(
    nai_stream_t* s, const nai_bufvec_t* v, nai_int_t count)
{
    (void)s;
    (void)v;
    (void)count;

    nai_errno = ENOTCONN;
    return -1;
};

static intptr_t nai_agent_stream_no_readq(
    nai_stream_t* s, nai_buflist_t* in, size_t limit)
{
    (void)s;
    (void)in;
    (void)limit;

    nai_errno = ENOTCONN;
    return -1;
};

static intptr_t nai_agent_stream_no_writeq(
    nai_stream_t* s, nai_buflist_t* out, size_t limit)
{
    (void)s;
    (void)out;
    (void)limit;

    nai_errno = ENOTCONN;
    return -1;
};


#define nai_agent_dgram_no_read     nai_agent_stream_no_read
#define nai_agent_dgram_no_readv    nai_agent_stream_no_readv
#define nai_agent_dgram_no_readq    nai_agent_stream_no_readq
#define nai_agent_dgram_no_write    nai_agent_stream_no_write
#define nai_agent_dgram_no_writev   nai_agent_stream_no_writev
#define nai_agent_dgram_no_writeq   nai_agent_stream_no_writeq


static nai_int_t nai_agent_dgram_no_shutdown(nai_dgram_t* s, nai_int_t how)
{
    (void)s;
    (void)how;

    nai_errno = ENOTCONN;
    return -1;
};

static intptr_t nai_agent_dgram_no_recv(nai_dgram_t* s, 
    void* buf, size_t len, nai_int_t flags, 
    nai_sockaddr_t* name, nai_int_t* namelen)
{
    (void)s;
    (void)buf;
    (void)len;
    (void)flags;
    (void)name;
    (void)namelen;

    nai_errno = ENOTCONN;
    return -1;
};

static intptr_t nai_agent_dgram_no_recvm(nai_dgram_t* s, 
    nai_bufvec_t* v, nai_int_t count, nai_int_t flags, 
    nai_sockaddr_t* name, nai_int_t* namelen, 
    void* ctrl, nai_int_t* ctrllen)
{
    (void)s;
    (void)v;
    (void)count;
    (void)flags;
    (void)name;
    (void)namelen;
    (void)ctrl;
    (void)ctrllen;

    nai_errno = ENOTCONN;
    return -1;
};

static intptr_t nai_agent_dgram_no_send(nai_dgram_t* s, 
    const void* buf, size_t len, nai_int_t flags, 
    const nai_sockaddr_t* name, nai_int_t namelen)
{
    (void)s;
    (void)buf;
    (void)len;
    (void)flags;
    (void)name;
    (void)namelen;

    nai_errno = ENOTCONN;
    return -1;
};

static intptr_t nai_agent_dgram_no_sendm(nai_dgram_t* s, 
    const nai_bufvec_t* v, nai_int_t count, nai_int_t flags, 
    const nai_sockaddr_t* name, nai_int_t namelen, 
    const void* ctrl, nai_int_t ctrllen)
{
    (void)s;
    (void)v;
    (void)count;
    (void)flags;
    (void)name;
    (void)namelen;
    (void)ctrl;
    (void)ctrllen;

    nai_errno = ENOTCONN;
    return -1;
};

static intptr_t nai_agent_dgram_no_sendmm(nai_dgram_t* s, 
    const nai_bufarray_t* v, nai_int_t count, nai_int_t flags, 
    const nai_sockaddr_t* name, nai_int_t namelen)
{
    (void)s;
    (void)v;
    (void)count;
    (void)flags;
    (void)name;
    (void)namelen;

    nai_errno = ENOTCONN;
    return -1;
};


static nai_stream_ops_t nai_agent_stream = {
    "agent-stream", 
    0, 
    nai_agent_iobase_open, 
    nai_agent_iobase_setopt, 
    nai_agent_iobase_getopt, 
    nai_agent_stream_no_shutdown, 
    nai_agent_iobase_close, 
    nai_agent_iobase_bind, 
    nai_agent_iobase_connect, 
    nai_agent_stream_no_read, 
    nai_agent_stream_no_readv, 
    nai_agent_stream_no_readq, 
    nai_agent_stream_no_write, 
    nai_agent_stream_no_writev, 
    nai_agent_stream_no_writeq, 
    nai_agent_dgram_no_recv, 
    nai_agent_dgram_no_recvm, 
    nai_agent_dgram_no_send, 
    nai_agent_dgram_no_sendm, 
    nai_agent_dgram_no_sendmm, 
};


static nai_dgram_ops_t nai_agent_dgram = {
    "agent-dgram", 
    0, 
    nai_agent_iobase_open, 
    nai_agent_iobase_setopt, 
    nai_agent_iobase_getopt, 
    nai_agent_dgram_no_shutdown, 
    nai_agent_iobase_close, 
    nai_agent_iobase_bind, 
    nai_agent_iobase_connect, 
    nai_agent_dgram_no_read, 
    nai_agent_dgram_no_readv, 
    nai_agent_dgram_no_readq, 
    nai_agent_dgram_no_write, 
    nai_agent_dgram_no_writev, 
    nai_agent_dgram_no_writeq, 
    nai_agent_dgram_no_recv, 
    nai_agent_dgram_no_recvm, 
    nai_agent_dgram_no_send, 
    nai_agent_dgram_no_sendm, 
    nai_agent_dgram_no_sendmm, 
};


static nai_int_t nai_agent_init_iobase_ops(nai_iobase_ops_t* p, nai_int_t type)
{
    nai_int_t r;
    nai_int_t n;
    nai_int_t count;
    size_t size;
    void** d;
    void** s;

    switch (type) {
    case NAI_IO_TYPE_STREAM:
        s = (void**)&nai_agent_stream.open;
        size = sizeof(nai_agent_stream) - nai_offsetof(nai_iobase_ops_t, open);
        count = (nai_int_t)(size/sizeof(*d));
        break;
    case NAI_IO_TYPE_DGRAM:
        s = (void**)&nai_agent_dgram.open;
        size = sizeof(nai_agent_dgram) - nai_offsetof(nai_iobase_ops_t, open);
        count = (nai_int_t)(size/sizeof(*d));
        break;
    default:
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    d = (void**)&p->open;
    for (n = 0; n < count; n ++) {
        if (d[n] == 0) {
            d[n] = s[n];
        };
    };

    r = 0;

_end:
    return r;
};



//////////////////////////////////////////////////////////////////////////////
// buf connection


#define nai_agent_buf_empty(b)      (nai_str_len(&(b)->mem) <= 0)


static intptr_t nai_agent_buf_read(
    nai_agent_buf_t* b, void* buf, size_t size)
{
    char* str;
    size_t len;

    str = nai_str(&b->mem);
    len = nai_str_len(&b->mem);

    if (size > len) {
        size = len;
    };

    nai_memcpy(buf, str, size);

    str += size;
    len -= size;
    nai_str_setm(&b->mem, str, len);
    return size;
};


static nai_int_t nai_agent_stream_restore(nai_stream_t* s)
{
    nai_int_t r;
    nai_iobase_t* c;
    nai_agent_context_t* p;
#if (NAI_HAVE_SSL)
    nai_agent_buf_t* b;
#endif


    c = (nai_iobase_t*)s;
    p = (nai_agent_context_t*)c->agent;
#if (NAI_HAVE_SSL)
    if (p->ssl) {
        b = (nai_agent_buf_t*)p->ctx;
        r = nai_agent_pseudo_open(
            c, 0, NAI_AGENT_WARP_SSL, NAI_IO_TYPE_STREAM);

        if (r >= 0) {
            p->ctx = b->ssl;
        } else {
            nai_log_error(NAI_LOG_CORE, 
                nai_errno, "switch to ssl stream failed");
        };

    } else {
#endif

        c->st.ops = p->ops;
        nai_agent_context_free(c);
        r = 0;

#if (NAI_HAVE_SSL)
    };
#endif

    return r;
};


static nai_int_t nai_agent_stream_shutdown(nai_stream_t* s, nai_int_t how)
{
    nai_int_t r;
    nai_iobase_t* c;
    nai_stream_ops_t* ops;
    nai_agent_context_t* p;
#if (NAI_HAVE_SSL)
    nai_agent_buf_t* b;
#endif


    c = (nai_iobase_t*)s;
    p = (nai_agent_context_t*)c->agent;
#if (NAI_HAVE_SSL)
    if (p->ssl) {
        b = (nai_agent_buf_t*)p->ctx;
        r = nai_ssl_shutdown(b->ssl, how);
    } else {
#endif
        ops = (nai_stream_ops_t*)p->ops;
        if (ops == 0) {
            nai_errno = ENOTCONN;
            r = -1;
            goto _end;
        };

        r = ops->shutdown(s, how);

#if (NAI_HAVE_SSL)
    };
#endif

_end:
    return r;
};


static intptr_t nai_agent_stream_read(
    nai_stream_t* s, void* buf, size_t len)
{
    intptr_t r;
    nai_iobase_t* c;
    nai_agent_context_t* p;
    nai_agent_buf_t* b;


    c = (nai_iobase_t*)s;
    p = (nai_agent_context_t*)c->agent;
    b = (nai_agent_buf_t*)p->ctx;
    if (b == 0) {
        nai_errno = ENOTCONN;
        r = -1;
        goto _end;
    };

    r = nai_agent_buf_read(b, buf, len);
    if (r < 0) {
        goto _end;
    };
    if (nai_agent_buf_empty(b)) {
        nai_agent_stream_restore(s);
    };

_end:
    return r;
};


static intptr_t nai_agent_stream_readv(
    nai_stream_t* s, nai_bufvec_t* v, nai_int_t count)
{
    intptr_t r;
    intptr_t total;
    nai_int_t n;
    nai_iobase_t* c;
    nai_agent_context_t* p;
    nai_agent_buf_t* b;


    c = (nai_iobase_t*)s;
    p = (nai_agent_context_t*)c->agent;
    b = (nai_agent_buf_t*)p->ctx;
    if (b == 0) {
        nai_errno = ENOTCONN;
        r = -1;
        goto _end;
    };

    total = 0;
    n = 0;
    for ( ; n < (intptr_t)count; n ++) {
        if (v[n].len == 0) {
            continue;
        };

        r = nai_agent_buf_read(b, v[n].buf, v[n].len);
        if (r < 0) {
            goto _end;
        };
        if (nai_agent_buf_empty(b)) {
            nai_agent_stream_restore(s);
            total += r;
            break;
        };
        if (r == 0) {
            break;
        };

        total += r;
        if (r < (intptr_t)v[n].len) {
            break;
        };
    };

    r = total;

_end:
    return r;
};


static intptr_t nai_agent_stream_readq(
    nai_stream_t* s, nai_buflist_t* in, size_t limit)
{
    intptr_t r;
    size_t total;
    size_t read;
    nai_iobase_t* c;
    nai_agent_context_t* p;
    nai_agent_buf_t* b;
    nai_bufvec_t v;
    nai_buflist_t full;


    nai_list_init(&full.ent);

    c = (nai_iobase_t*)s;
    p = (nai_agent_context_t*)c->agent;
    b = (nai_agent_buf_t*)p->ctx;
    if (b == 0) {
        nai_errno = ENOTCONN;
        r = -1;
        goto _fail;
    };

    total = 0;
    while (!nai_buflist_is_empty(in)) {
        read = 4096;
        if (read > limit) {
            read = limit;
        };

        r = nai_buflist_to_wbufvec(in, &v, read, 0);
        if (r < 0) {
            goto _fail;
        };
        if (r == 0) {
            break;
        };

        r = nai_agent_buf_read(b, v.buf, v.len);
        if (r < 0) {
            goto _fail;
        };
        if (nai_agent_buf_empty(b)) {
            nai_agent_stream_restore(s);
            total += r;
            break;
        };
        if (r == 0) {
            break;
        };


        total += r;
        nai_buflist_fulled(in, &full, r);

        limit -= r;
        if (limit <= 0) {
            break;
        };
    };

    r = total;

_fail:
    nai_buflist_add_head(in, &full);
    return r;
};


static intptr_t nai_agent_stream_write(
    nai_stream_t* s, const void* buf, size_t len)
{
    intptr_t r;
    nai_iobase_t* c;
    nai_stream_ops_t* ops;
    nai_agent_context_t* p;
#if (NAI_HAVE_SSL)
    nai_agent_buf_t* b;
#endif


    c = (nai_iobase_t*)s;
    p = (nai_agent_context_t*)c->agent;
#if (NAI_HAVE_SSL)
    if (p->ssl) {
        b = (nai_agent_buf_t*)p->ctx;
        r = nai_ssl_write(b->ssl, buf, len);
    } else {
#endif
        ops = (nai_stream_ops_t*)p->ops;
        if (ops == 0) {
            nai_errno = ENOTCONN;
            r = -1;
            goto _end;
        };

        r = ops->write(s, buf, len);

#if (NAI_HAVE_SSL)
    };
#endif

_end:
    return r;
};


static intptr_t nai_agent_stream_writev(
    nai_stream_t* s, const nai_bufvec_t* v, nai_int_t count)
{
    intptr_t r;
    nai_iobase_t* c;
    nai_stream_ops_t* ops;
    nai_agent_context_t* p;
#if (NAI_HAVE_SSL)
    nai_agent_buf_t* b;
#endif


    c = (nai_iobase_t*)s;
    p = (nai_agent_context_t*)c->agent;
#if (NAI_HAVE_SSL)
    if (p->ssl) {
        b = (nai_agent_buf_t*)p->ctx;
        r = nai_ssl_writev(b->ssl, v, count);
    } else {
#endif
        ops = (nai_stream_ops_t*)p->ops;
        if (ops == 0) {
            nai_errno = ENOTCONN;
            r = -1;
            goto _end;
        };

        r = ops->write(s, v, count);

#if (NAI_HAVE_SSL)
    };
#endif

_end:
    return r;
};


static intptr_t nai_agent_stream_writeq(
    nai_stream_t* s, nai_buflist_t* out, size_t limit)
{
    intptr_t r;
    nai_iobase_t* c;
    nai_stream_ops_t* ops;
    nai_agent_context_t* p;
#if (NAI_HAVE_SSL)
    nai_agent_buf_t* b;
#endif


    c = (nai_iobase_t*)s;
    p = (nai_agent_context_t*)c->agent;
#if (NAI_HAVE_SSL)
    if (p->ssl) {
        b = (nai_agent_buf_t*)p->ctx;
        r = nai_ssl_writeq(b->ssl, out, limit);
    } else {
#endif
        ops = (nai_stream_ops_t*)p->ops;
        if (ops == 0) {
            nai_errno = ENOTCONN;
            r = -1;
            goto _end;
        };

        r = ops->writeq(s, out, limit);

#if (NAI_HAVE_SSL)
    };
#endif

_end:
    return r;
};


static intptr_t nai_agent_stream_recv(nai_stream_t* s, 
    void* buf, size_t len, nai_int_t flags, 
    nai_sockaddr_t* name, nai_int_t* namelen)
{
    intptr_t r;


    (void)flags;
    (void)namelen;

    if (name) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    r = nai_agent_stream_read(s, buf, len);

_end:
    return r;
};


static intptr_t nai_agent_stream_recvm(nai_stream_t* s, 
    nai_bufvec_t* v, nai_int_t count, nai_int_t flags, 
    nai_sockaddr_t* name, nai_int_t* namelen, 
    void* ctrl, nai_int_t* ctrllen)
{
    intptr_t r;


    (void)flags;
    (void)namelen;
    (void)ctrl;

    if (name) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    r = nai_agent_stream_readv(s, v, count);

    if (r >= 0 && ctrllen) {
        ctrllen[0] = 0;
    };

_end:
    return r;
};


static intptr_t nai_agent_stream_send(nai_stream_t* s, 
    const void* buf, size_t len, nai_int_t flags, 
    const nai_sockaddr_t* name, nai_int_t namelen)
{
    intptr_t r;
    nai_iobase_t* c;
    nai_stream_ops_t* ops;
    nai_agent_context_t* p;
#if (NAI_HAVE_SSL)
    nai_agent_buf_t* b;
#endif


    c = (nai_iobase_t*)s;
    p = (nai_agent_context_t*)c->agent;
#if (NAI_HAVE_SSL)
    if (p->ssl) {
        if (name) {
            nai_errno = EINVAL;
            r = -1;
            goto _end;
        };

        b = (nai_agent_buf_t*)p->ctx;
        r = nai_ssl_write(b->ssl, buf, len);
    } else {
#endif
        ops = (nai_stream_ops_t*)p->ops;
        if (ops == 0) {
            nai_errno = ENOTCONN;
            r = -1;
            goto _end;
        };

        r = ops->send(s, buf, len, flags, name, namelen);

#if (NAI_HAVE_SSL)
    };
#endif

_end:
    return r;
};


static intptr_t nai_agent_stream_sendm(nai_stream_t* s, 
    const nai_bufvec_t* v, nai_int_t count, nai_int_t flags, 
    const nai_sockaddr_t* name, nai_int_t namelen, 
    const void* ctrl, nai_int_t ctrllen)
{
    intptr_t r;
    nai_iobase_t* c;
    nai_stream_ops_t* ops;
    nai_agent_context_t* p;
#if (NAI_HAVE_SSL)
    nai_agent_buf_t* b;
#endif


    c = (nai_iobase_t*)s;
    p = (nai_agent_context_t*)c->agent;
#if (NAI_HAVE_SSL)
    if (p->ssl) {
        if (name) {
            nai_errno = EINVAL;
            r = -1;
            goto _end;
        };
        if (ctrl) {
            nai_errno = EINVAL;
            r = -1;
            goto _end;
        };

        b = (nai_agent_buf_t*)p->ctx;
        r = nai_ssl_writev(b->ssl, v, count);
    } else {
#endif
        ops = (nai_stream_ops_t*)p->ops;
        if (ops == 0) {
            nai_errno = ENOTCONN;
            r = -1;
            goto _end;
        };

        r = ops->sendm(s, v, count, flags, name, namelen, ctrl, ctrllen);

#if (NAI_HAVE_SSL)
    };
#endif

_end:
    return r;
};


static intptr_t nai_agent_stream_sendmm(nai_stream_t* s, 
    const nai_bufarray_t* v, nai_int_t count, nai_int_t flags, 
    const nai_sockaddr_t* name, nai_int_t namelen)
{
    intptr_t r;
    nai_iobase_t* c;
    nai_stream_ops_t* ops;
    nai_agent_context_t* p;
#if (NAI_HAVE_SSL)
    nai_agent_buf_t* b;
#endif


    c = (nai_iobase_t*)s;
    p = (nai_agent_context_t*)c->agent;
#if (NAI_HAVE_SSL)
    if (p->ssl) {
        if (name) {
            nai_errno = EINVAL;
            r = -1;
            goto _end;
        };
        if (count < 0) {
            r = 0;
            goto _end;
        };

        b = (nai_agent_buf_t*)p->ctx;
        r = nai_ssl_writev(b->ssl, v[0].v, v[0].count);
    } else {
#endif
        ops = (nai_stream_ops_t*)p->ops;
        if (ops == 0) {
            nai_errno = ENOTCONN;
            r = -1;
            goto _end;
        };

        r = ops->sendmm(s, v, count, flags, name, namelen);

#if (NAI_HAVE_SSL)
    };
#endif

_end:
    return r;
};


//////////////////////////////////////////////////////////////////////////////
// ssl connection


#if (NAI_HAVE_SSL)


static nai_int_t nai_agent_ssl_shutdown(nai_stream_t* s, nai_int_t how)
{
    nai_int_t r;
    nai_iobase_t* c;
    nai_agent_context_t* p;


    c = (nai_iobase_t*)s;
    p = (nai_agent_context_t*)c->agent;
    r = nai_ssl_shutdown((nai_ssl_t*)p->ctx, how);

    return r;
};


static nai_int_t nai_agent_ssl_setopt(
    nai_iobase_t* s, nai_int_t opt, intptr_t value)
{
    nai_int_t r;
    nai_iobase_t* c;
    nai_iobase_ops_t* ops;
    nai_agent_context_t* p;


    c = (nai_iobase_t*)s;
    p = (nai_agent_context_t*)c->agent;
    switch (opt) {
    case NAI_IO_LOADFILE:
#if (NAI_HAVE_SSL)
        r = nai_ssl_enable_load((nai_ssl_t*)p->ctx, value);
        break;
#endif
    default:
        ops = (nai_iobase_ops_t*)p->ops;
        if (ops) {
            r = ops->setopt(s, opt, value);
            goto _end;
        };

        r = nai_iobase_default_setopt(s, opt, value);
        break;
    };

_end:
    return r;
};


static nai_int_t nai_agent_ssl_getopt(
    nai_iobase_t* s, nai_int_t opt, intptr_t* value)
{
    nai_int_t r;
    nai_iobase_t* c;
    nai_iobase_ops_t* ops;
    nai_agent_context_t* p;


    c = (nai_iobase_t*)s;
    p = (nai_agent_context_t*)c->agent;
    switch (opt) {
    case NAI_IO_LOADFILE:
        value[0] = ((nai_ssl_t*)p->ctx)->loadfile;
        r = 0;
        break;
    case NAI_IO_FEAT_VECTORIO:
        value[0] = 0;
        r = 0;
        break;
    case NAI_IO_FEAT_SENDFILE:
        if (!nai_ssl_support_ktls()) {
            value[0] = 0;
            r = 0;
            break;
        };

        /* fallthrough */

    default:
        ops = (nai_iobase_ops_t*)p->ops;
        if (ops) {
            r = ops->getopt(s, opt, value);
            goto _end;
        };

        r = nai_iobase_default_getopt(s, opt, value);
        break;
    };

_end:
    return r;
};


static intptr_t nai_agent_ssl_read(
    nai_stream_t* s, void* buf, size_t len)
{
    intptr_t r;
    nai_iobase_t* c;
    nai_agent_context_t* p;


    c = (nai_iobase_t*)s;
    p = (nai_agent_context_t*)c->agent;
    r = nai_ssl_read((nai_ssl_t*)p->ctx, buf, len);

    return r;
};


static intptr_t nai_agent_ssl_readv(
    nai_stream_t* s, nai_bufvec_t* v, nai_int_t count)
{
    intptr_t r;
    nai_iobase_t* c;
    nai_agent_context_t* p;


    c = (nai_iobase_t*)s;
    p = (nai_agent_context_t*)c->agent;
    r = nai_ssl_readv((nai_ssl_t*)p->ctx, v, count);

    return r;
};


static intptr_t nai_agent_ssl_readq(
    nai_stream_t* s, nai_buflist_t* in, size_t limit)
{
    intptr_t r;
    nai_iobase_t* c;
    nai_agent_context_t* p;


    c = (nai_iobase_t*)s;
    p = (nai_agent_context_t*)c->agent;
    r = nai_ssl_readq((nai_ssl_t*)p->ctx, in, limit);

    return r;
};


static intptr_t nai_agent_ssl_write(
    nai_stream_t* s, const void* buf, size_t len)
{
    intptr_t r;
    nai_iobase_t* c;
    nai_agent_context_t* p;


    c = (nai_iobase_t*)s;
    p = (nai_agent_context_t*)c->agent;
    r = nai_ssl_write((nai_ssl_t*)p->ctx, buf, len);

    return r;
};


static intptr_t nai_agent_ssl_writev(
    nai_stream_t* s, const nai_bufvec_t* v, nai_int_t count)
{
    intptr_t r;
    nai_iobase_t* c;
    nai_agent_context_t* p;


    c = (nai_iobase_t*)s;
    p = (nai_agent_context_t*)c->agent;
    r = nai_ssl_writev((nai_ssl_t*)p->ctx, v, count);

    return r;
};


static intptr_t nai_agent_ssl_writeq(
    nai_stream_t* s, nai_buflist_t* out, size_t limit)
{
    intptr_t r;
    nai_iobase_t* c;
    nai_agent_context_t* p;


    c = (nai_iobase_t*)s;
    p = (nai_agent_context_t*)c->agent;
    r = nai_ssl_writeq((nai_ssl_t*)p->ctx, out, limit);

    return r;
};


#endif


//////////////////////////////////////////////////////////////////////////////
// posude connection


typedef struct nai_agent_iobase_ops_s {
    nai_int_t inited;
    struct {
        nai_stream_ops_t stream;
        nai_dgram_ops_t dgram;
    } ops[NAI_AGENT_WARP_MAX];
} nai_agent_iobase_ops_t;


#define NAI_AGENT_PSEUDO_FEAT (                     \
    NAI_EV_FEAT_MODELMASK|NAI_EV_FEAT_FDMASK|       \
    NAI_IOBASE_PSEUDO)                              \


static nai_int_t nai_agent_dns_close(nai_iobase_t* s);
static nai_agent_iobase_ops_t nai_agent_iobase_ops = {
    0, {
    {{ .name = "dns-stream", .require = NAI_AGENT_PSEUDO_FEAT, 
       .close = nai_agent_dns_close }, 
     { .name = "dns-dgram" , .require = NAI_AGENT_PSEUDO_FEAT, 
       .close = nai_agent_dns_close }}, 

    {{ .name = "connect-stream", .require = NAI_AGENT_PSEUDO_FEAT }, 
     { .name = "connect-dgram" , .require = NAI_AGENT_PSEUDO_FEAT }}, 

    {{ .name = "buf-stream", 
       .require = NAI_AGENT_PSEUDO_FEAT, 
       .shutdown = nai_agent_stream_shutdown, 
       .read = nai_agent_stream_read, 
       .readv = nai_agent_stream_readv, 
       .readq = nai_agent_stream_readq, 
       .write = nai_agent_stream_write, 
       .writev = nai_agent_stream_writev, 
       .writeq = nai_agent_stream_writeq, 
       .recv = nai_agent_stream_recv, 
       .recvm = nai_agent_stream_recvm, 
       .send = nai_agent_stream_send, 
       .sendm = nai_agent_stream_sendm, 
       .sendmm = nai_agent_stream_sendmm, }, 
     { .name = "buf-dgram" , .require = NAI_AGENT_PSEUDO_FEAT }}, 

#if (NAI_HAVE_SSL)
    {{ .name = "ssl-stream", 
       .require = NAI_AGENT_PSEUDO_FEAT, 
       .shutdown = nai_agent_ssl_shutdown, 
       .getopt = nai_agent_ssl_getopt, 
       .setopt = nai_agent_ssl_setopt, 
       .read = nai_agent_ssl_read, 
       .readv = nai_agent_ssl_readv, 
       .readq = nai_agent_ssl_readq, 
       .write = nai_agent_ssl_write, 
       .writev = nai_agent_ssl_writev, 
       .writeq = nai_agent_ssl_writeq, }, 
     { .name = "ssl-dgram" , 
       .require = NAI_AGENT_PSEUDO_FEAT, 
       .shutdown = nai_agent_ssl_shutdown, 
       .getopt = nai_agent_ssl_getopt, 
       .setopt = nai_agent_ssl_setopt, 
       .read = nai_agent_ssl_read, 
       .readv = nai_agent_ssl_readv, 
       .readq = nai_agent_ssl_readq, 
       .write = nai_agent_ssl_write, 
       .writev = nai_agent_ssl_writev, 
       .writeq = nai_agent_ssl_writeq, }}, 
#endif
    }, 
};


static nai_int_t nai_agent_dns_close(nai_iobase_t* s)
{
    nai_int_t r;
    nai_iobase_t* c;
    nai_agent_context_t* p;
    nai_dns_query_t* q;


    c = (nai_iobase_t*)s;
    p = (nai_agent_context_t*)c->agent;
    q = (nai_dns_query_t*)p->ctx;
    if (q != 0) {
        r = nai_dns_query_cancel(q);
        if (r < 0) {
            goto _end;
        };
    };

    if (p->ops) {
        r = p->ops->close(s);
        if (r < 0) {
            goto _end;
        };

        p->ops = 0;

    } else {
        r = nai_evnode_close(&c->ev);
        if (r < 0) {
            goto _end;
        };
    };

    c->st.type = 0;
    c->st.ops = 0;
    r = 0;


_end:
    return r;
};


static nai_int_t nai_agent_except_handle(nai_evnode_t* n, nai_int_t events)
{
    nai_int_t r;
    nai_int_t ec;
    nai_int_t signum;
    nai_iobase_t* c;
    nai_agent_context_t* p;


    c = (nai_iobase_t*)n;
    if (events & NAI_EV_NOTIFY) {

        /* check notification code */
        signum = nai_ev_notify_code(events);
        if (signum == NAI_IOBASE_SIGNAL) {
            /* special notification code means connection failed, 
             * make an error event to user.
             */
            p = (nai_agent_context_t*)c->agent;
            if (p && p->error && c->cb) {
                ec = p->error;
                p->error = 0;
                events = nai_ev_error_from(ec);
            };
        };
    };

    if (c->cb) {
        c->cb(c, events);
    };

    r = 0;

    return r;
};


nai_int_t nai_agent_failed(nai_iobase_t* c, nai_int_t error)
{
    nai_int_t r;
    nai_evloop_t* l;
    nai_agent_context_t* p;


    if (nai_evnode_is_opened(&c->ev)) {

        /* check current thread */
        l = nai_evnode_get_loop(&c->ev);
        if (nai_evloop_in_dispatch(l) == 0) {
            /* we can't process error in other thread, 
             * save error code and post event to the event loop thread.
             */
            p = (nai_agent_context_t*)c->agent;
            p->error = error;
            nai_evnode_set_cb(&c->ev, nai_agent_except_handle);
            nai_iobase_post_signal(c);
            r = -1;
            goto _end;
        };
    };

    if (c->cb) {
        c->cb(c, nai_ev_error_from(error));
    };

    r = 0;

_end:
    return r;
};


nai_int_t nai_agent_context_create(
    nai_iobase_t* c, nai_pool_t* pool)
{
    nai_int_t r;
    nai_int_t ec;
    nai_pool_t tmp;
    nai_agent_context_t* p;


    if (c->agent != -1) {
        p = (nai_agent_context_t*)c->agent;
    } else {
        p = 0;
    };

    assert(c->st.agent == (p!=0));
    if (p != 0) {
        r = 0;
        goto _end;
    };

    if (pool == 0) {
        if (c->st.connection) {
            pool = ((nai_connection_t*)c)->pool;
        } else {
            pool = &tmp;
            nai_pool_init(pool, 512);
        };
    };

    p = (nai_agent_context_t*)nai_palloc(pool, sizeof(*p));
    if (p == 0) {
        nai_log_alert(NAI_LOG_CORE, 
            nai_errno, "allocaate agent context failed");
        goto _fail;
    };

    p->ops = 0;
    p->ctx = 0;
    p->error = 0;
    p->flags = 0;

    if (pool != &tmp) {
        p->pool = pool;
    } else {
        p->pool = (nai_pool_t*)nai_palloc(pool, sizeof(*pool));
        if (p->pool == 0) {
            nai_log_alert(NAI_LOG_CORE, 
                nai_errno, "allocaate pool of agent context failed");
            goto _fail;
        };

        p->own = 1;
        nai_pool_init(p->pool, 512);
        nai_pool_exchange(p->pool, pool);
    };

    c->st.agent = 1;
    c->agent = (intptr_t)p;
    r = 0;

_end:
    return r;

_fail:
    if (pool == &tmp) {
        ec = nai_errno;
        nai_pool_close(&tmp);
        nai_errno = ec;
    };
    r = -1;
    goto _end;
};


nai_int_t nai_agent_context_free(nai_iobase_t* c)
{
    nai_int_t r;
    nai_agent_context_t* p;


    if (c->st.agent == 0) {
        r = 0;
        goto _end;
    };

    p = (nai_agent_context_t*)c->agent;
    if (p->own) {
        nai_pool_close(p->pool);
    };

    c->st.agent = 0;
    c->agent = 0;
    r = 0;

_end:
    return r;
};


nai_int_t nai_agent_pseudo_open(
    nai_iobase_t* c, nai_evloop_t* loop, nai_int_t func, nai_int_t proto)
{
    nai_int_t r;
    nai_int_t n;
    nai_int_t count;
    nai_iobase_ops_t* ops;
    nai_agent_context_t* p;


    /* inititalize ops interfaces */
    count = nai_countof(nai_agent_iobase_ops.ops);
    if (func < 0 || func >= count) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };
    if (nai_agent_iobase_ops.inited == 0) {
        for (n = 0; n < count; n ++) {
            nai_agent_init_iobase_ops((nai_iobase_ops_t*)
                &nai_agent_iobase_ops.ops[n].dgram, NAI_IO_TYPE_DGRAM);
            nai_agent_init_iobase_ops((nai_iobase_ops_t*)
                &nai_agent_iobase_ops.ops[n].stream, NAI_IO_TYPE_STREAM);
        };
        nai_memory_barrier();
        nai_agent_iobase_ops.inited = 1;
    };

    /* get ops interface */
    switch (proto) {
    case NAI_IO_TYPE_STREAM:
        ops = (nai_iobase_ops_t*)&nai_agent_iobase_ops.ops[func].stream;
        break;
    case NAI_IO_TYPE_DGRAM:
        ops = (nai_iobase_ops_t*)&nai_agent_iobase_ops.ops[func].dgram;
        break;
    default:
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };


    /* create */
    r = nai_agent_context_create(c, 0);
    if (r < 0) {
        nai_log_alert(NAI_LOG_CORE, 
            nai_errno, "create agent context failed");
        goto _end;
    };


    /* setup ops */
    p = (nai_agent_context_t*)c->agent;
    if (p->ops == 0 && 
        c->st.ops && !(
        c->st.ops->require & NAI_IOBASE_PSEUDO)) {
        p->ops = c->st.ops;
    };
    c->st.ops = ops;
    c->st.type = proto;


    /* construct pseudo connection 
     * to make the connection look like it is connecting
     */
    if (loop == 0) {
        r = 0;
    } else if (nai_evnode_is_opened(&c->ev)) {
        r = !nai_evloop_in_dispatch(loop);
    } else {
        nai_evnode_set_cb(&c->ev, nai_agent_except_handle);
        r = nai_evnode_open(&c->ev, loop);
        if (r < 0) {
            nai_log_alert(NAI_LOG_CORE, 
                nai_errno, "open the event node failed");
            goto _end;
        };
    };

_end:
    return r;
};



//////////////////////////////////////////////////////////////////////////////
// node connect


nai_int_t nai_agent_make_sockaddr(
    nai_socknbuf_in_t* name, 
    nai_int_t type, const void* addr, nai_int_t port)
{
    nai_int_t r;


    switch (type) {
    case NAI_AGENT_ADDR_IN:
        name->len = sizeof(name->storage);
        r = nai_sockaddr_mk_inet(
            AF_INET, addr, port, &name->addr, &name->len);
        break;

#if (NAI_HAVE_SOCKADDR_IN6)
    case NAI_AGENT_ADDR_IN6:
        name->len = sizeof(name->storage);
        r = nai_sockaddr_mk_inet(
            AF_INET6, addr, port, &name->addr, &name->len);
        break;
#endif

    default:
        nai_errno = EAFNOSUPPORT;
        r = -1;
        break;
    };

    return r;
};


nai_int_t nai_agent_node_open(nai_agent_node_t* n, 
    nai_agent_t* p, const nai_uri_t* attr, size_t extra)
{
    nai_int_t r;
    nai_int_t port;
    nai_int_t type;
    char* mem;
    char* src;
    size_t len;
    nai_mem_t addr;
    nai_socknbuf_in_t nbuf;
    nai_agent_nconf_t* ac;


    addr = attr->hostname;
    nbuf.len = sizeof(nbuf.storage);
    r = nai_sockaddr_pton(
        nai_str(&addr), nai_str_len(&addr), &nbuf.addr, &nbuf.len);
    if (r < 0) {
        type = NAI_AGENT_ADDR_DOMAIN;
    } else {
        switch (nbuf.addr.sa_family) {
        case AF_INET:
            nai_str_setm(&addr, 
                &nbuf.addr_in4.sin_addr, sizeof(nbuf.addr_in4.sin_addr));
            type = NAI_AGENT_ADDR_IN;
            break;
#if (NAI_HAVE_SOCKADDR_IN6)
        case AF_INET6:
            nai_str_setm(&addr, 
                &nbuf.addr_in6.sin6_addr, sizeof(nbuf.addr_in6.sin6_addr));
            type = NAI_AGENT_ADDR_IN6;
            break;
#endif
        default:
            nai_errno = EAFNOSUPPORT;
            r = -1;
            goto _end;
        };
    };

    if (nai_str_len(&attr->portname) <= 0) {
        port = 0;
    } else {
        r = nai_atoi(&port, 
            nai_str(&attr->portname), nai_str_len(&attr->portname), 0);
        if (r < 0) {
            goto _end;
        };
    };

    len = extra + (type == NAI_AGENT_ADDR_DOMAIN) + 
        nai_str_len(&addr) + nai_str_len(&attr->host) + 
        nai_str_len(&attr->user) + nai_str_len(&attr->password);


    ac = (nai_agent_nconf_t*)(p->pool ? 
        nai_palloc(p->pool, sizeof(*ac) + len) :
        nai_malloc(sizeof(*ac) + len));
    if (ac == 0) {
        r = -1;
        goto _end;
    };

    ac->agent = p;
    ac->flags = 0;
    ac->type = type;
    ac->port = port;

    mem = (char*)(ac + 1) + extra;
    src = nai_str(&attr->host);
    len = nai_str_len(&attr->host);
    nai_memcpy(mem, src, len);
    nai_str_setm(&ac->host, mem, len);
    mem += len;

    src = nai_str(&attr->user);
    len = nai_str_len(&attr->user);
    nai_memcpy(mem, src, len);
    nai_str_setm(&ac->user, mem, len);
    mem += len;

    src = nai_str(&attr->password);
    len = nai_str_len(&attr->password);
    nai_memcpy(mem, src, len);
    nai_str_setm(&ac->password, mem, len);
    mem += len;

    src = nai_str(&addr);
    len = nai_str_len(&addr);
    nai_memcpy(mem, src, len);
    nai_str_setm(&ac->addr, mem, len);
    mem += len;
    if (type == NAI_AGENT_ADDR_DOMAIN) {
        mem[0] = 0;
        mem ++;
    };

    nai_str_setm(&ac->hostname, 
        nai_str(&ac->host), nai_str_len(&attr->hostname));
    nai_str_setm(&ac->portname, 
        nai_str(&ac->host) + nai_str_len(&ac->hostname), 
        nai_str_len(&attr->portname));

    n->ctx = ac;
    r = 0;

_end:
    return r;
};


nai_int_t nai_agent_node_close(nai_agent_node_t* n)
{
    nai_int_t r;
    nai_agent_nconf_t* ac;


    ac = (nai_agent_nconf_t*)n->ctx;
    if (ac && ac->agent->pool == 0) {
        nai_free(n->ctx);
    };

    n->ctx = 0;
    r = 0;

    return r;
};


typedef struct nai_agent_node_dns_query_s {
    nai_dns_query_t query;
    nai_iobase_t* conn;
    nai_agent_node_t* node;
    nai_agent_dest_t dest;
    uint16_t async:1;
} nai_agent_node_dns_query_t;



static nai_int_t nai_agent_node_do_connect(
    nai_agent_attr_t* a, nai_iobase_t* c, 
    nai_agent_dest_t* dest)
{
    nai_int_t r;
    nai_int_t domain;
    nai_int_t addrlen;
    void* addr;
    nai_agent_node_t* n;
    nai_agent_nconf_t* ac;
    nai_agent_context_t* cc;


    n = a->node;
    ac = (nai_agent_nconf_t*)n->ctx;
    if (ac->agent->loop && dest->temp) {
        r = nai_agent_context_create(c, 0);
        if (r < 0) {
            nai_log_alert(NAI_LOG_CORE, 
                nai_errno, "create agent context failed");
            goto _end;
        };

        /* domain with null-terminated */
        cc = (nai_agent_context_t*)c->agent;
        domain = dest->type == NAI_AGENT_ADDR_DOMAIN;
        addrlen = nai_str_len(&dest->addr);
        addr = nai_palloc(cc->pool, addrlen + domain);
        if (addr == 0) {
            nai_log_alert(NAI_LOG_CORE, 
                nai_errno, "duplicate agent dest address failed");
            r = -1;
            goto _end;
        };

        nai_memcpy(addr, nai_str(&dest->addr), addrlen + domain);
        nai_str_setm((nai_str_t*)&dest->addr, addr, addrlen);
    };

    r = ac->connect(a, c, dest);

_end:
    return r;
};


static nai_int_t nai_agent_node_dns_result(
    nai_dns_query_t* d, const nai_dns_result_t* rs)
{
    nai_int_t r;
    nai_int_t ec;
    nai_int_t async;
    nai_mem_t* host;
    nai_iobase_t* c;
    nai_agent_attr_t a;
    nai_agent_node_t* p;
    nai_agent_node_dns_query_t* q;
    nai_agent_context_t* cc;
    nai_agent_nconf_t* ac;
    nai_agent_dest_t dest;
    nai_socknbuf_in_t nbuf;
    const nai_sockaddr_info_t* si;


    /* get connect arguments */
    q = nai_containof(d, nai_agent_node_dns_query_t, query);
    p = q->node;
    c = q->conn;
    host = &q->query.name;
    async = q->async;


    /* check dns result */
    if (rs->error) {
        nai_log_error(NAI_LOG_CORE, 
            rs->error, "dns query node '%.*s' failed", 
            nai_str_len(host), nai_str(host));

        ec = rs->error;
        goto _fail;
    };


    /* get address from dns result */
    nbuf.len = sizeof(nbuf.storage);
    r = nai_dns_result_sockaddr(
        rs, -1, -1, &nbuf.addr, &nbuf.len);
    if (r < 0) {
        nai_log_error(NAI_LOG_CORE, 
            nai_errno, "get dns result of node '%.*s' failed", 
            nai_str_len(host), nai_str(host));

        ec = nai_errno;
        goto _fail;
    };

    /* set port */
    si = nai_sockaddr_info(nbuf.addr.sa_family);
    if (si == 0) {
        nai_log_warn(NAI_LOG_CORE, 
            EAFNOSUPPORT, "agent node with unknown address family(%d)", 
            nbuf.addr.sa_family);

        ec = EAFNOSUPPORT;
        goto _fail;
    };

    ac = (nai_agent_nconf_t*)p->ctx;
    nai_sockaddr_set_port(si, &nbuf.addr, ac->port);


    /* make attr */
    a.node = p;
    a.dns = d->dns;

    /* make dest */
    dest = q->dest;
    nai_sockname_setbuf(&dest.proxy, &nbuf);


    /* free memory of query context  */
    cc = (nai_agent_context_t*)c->agent;
    cc->ctx = 0;
    nai_pool_free_last(cc->pool, q, sizeof(*q));
    q = 0;


    /* do connect */
    r = nai_agent_node_do_connect(&a, c, &dest);
    if (r < 0) {
        ec = nai_errno;
        nai_log_error(NAI_LOG_CORE, 
            nai_errno, "agent start connect node %.*s failed", 
            nai_str_len(host), nai_str(host));

        /* reconstruct pseudo connection for user close it in callback */
        goto _fail;
    };


_end:
    return r;

_fail:
    if (q != 0) {
        cc = (nai_agent_context_t*)c->agent;
        nai_pool_free_last(cc->pool, q, sizeof(*q));
    };
    if (async) {
        r = nai_agent_failed(c, ec);
    } else {
        nai_errno = ec;
        r = -1;
    };
    goto _end;
};


static nai_int_t nai_agent_node_dns_query(
    nai_agent_attr_t* a, nai_iobase_t* c, 
    nai_agent_dest_t* dest)
{
    nai_int_t r;
    nai_int_t ec;
    nai_int_t mt;
    nai_int_t domain;
    nai_int_t addrlen;
    void* addr;
    nai_mem_t name;
    nai_dns_result_t rs;
    nai_agent_node_t* n;
    nai_agent_nconf_t* ac;
    nai_agent_context_t* cc;
    nai_agent_node_dns_query_t* q;


    n = a->node;
    ac = (nai_agent_nconf_t*)n->ctx;
    name = ac->addr;


    /* construct pseudo connection 
     * to make the connection look like it is connecting
     */
    r = nai_agent_pseudo_open(
        c, ac->agent->loop, NAI_AGENT_WARP_DNS, dest->proto);
    if (r < 0) {
        nai_log_error(NAI_LOG_CORE, 
            nai_errno, "open pseudo connection failed");
        goto _end;
    };

    mt = r;
    cc = (nai_agent_context_t*)c->agent;
    cc->ctx = 0;


    /* duplicate temp dest address */
    addrlen = nai_str_len(&dest->addr);
    if (dest->temp == 0) {
        addr = nai_str(&dest->addr);
    } else {
        /* domain with null-terminated */
        domain = dest->type == NAI_AGENT_ADDR_DOMAIN;
        addr = nai_palloc(cc->pool, addrlen + domain);
        if (addr == 0) {
            ec = nai_errno;
            nai_log_alert(NAI_LOG_CORE, 
                ec, "duplicate host '%.*s' failed", 
                nai_str_len(&name), nai_str(&name));

            goto _fail;
        };

        nai_memcpy(addr, nai_str(&dest->addr), addrlen + domain);
    };


    /* prepare dns query arguments */
    q = (nai_agent_node_dns_query_t*)nai_palloc(cc->pool, sizeof(*q));
    if (q == 0) {
        ec = nai_errno;
        nai_log_alert(NAI_LOG_CORE, 
            ec, "allocate dns query context failed");
        goto _fail;
    };

    q->node = n;
    q->conn = c;
    q->async = 1;
    q->dest.proto = dest->proto;
    q->dest.port = dest->port;
    q->dest.type = dest->type;
    q->dest.temp = 0;
    nai_str_setm(&q->dest.addr, addr, addrlen);
    nai_dns_query_init(&q->query);
    nai_dns_query_set_cb(&q->query, nai_agent_node_dns_result);
    nai_dns_query_set_name(&q->query, &name);
    cc->ctx = q;


    /* start dns query */
    r = nai_dns_query_submit(&q->query, a->dns, &rs);
    if (r < 0) {
        ec = nai_errno;
        if (ec != NAI_EINPROGRESS) {
            nai_pool_free_last(cc->pool, q, sizeof(*q));
            nai_log_error(NAI_LOG_CORE, 
                ec, "start dns query node '%.*s' failed", 
                nai_str_len(&name), nai_str(&name));
            goto _fail;
        };

        r = mt;
        goto _end;
    };

    /* complete */
    q->async = 0;
    r = nai_agent_node_dns_result(&q->query, &rs);
    if (r < 0) {
        ec = nai_errno;
        goto _fail;
    };

_end:
    return r;

_fail:
    if (mt) {
        /* we can't close the event node in other thread, 
         * save error code and post event to the event loop thread.
         */
        nai_agent_failed(c, ec);
        r = 1;
    } else {
        nai_iobase_close(c);
        nai_errno = ec;
        r = -1;
    };

    goto _end;
};


static nai_int_t nai_agent_node_query_host(
    nai_agent_attr_t* a, const nai_mem_t* hostname, nai_socknbuf_in_t* nbuf)
{
    intptr_t r;
    nai_dns_query_t q;
    nai_dns_result_t rs;
    uint8_t buf[1024];


    if (a->dns) {
        nai_dns_query_init(&q);
        nai_dns_query_set_name(&q, hostname);
        r = nai_dns_query(&q, a->dns, &rs, buf, sizeof(buf));
        if (r >= 0) {
            nbuf->len = sizeof(nbuf->storage);
            r = nai_dns_result_sockaddr(
                &rs, -1, -1, &nbuf->addr, &nbuf->len);
            goto _end;
        };
        if (nai_errno != EDEADLK) {
            goto _end;
        };
    };


    r = nai_sockaddr_list(
        nai_str(hostname), 0, nbuf, sizeof(*nbuf));
    if (r < 0) {
        nai_log_error(NAI_LOG_CORE, 
            nai_errno, "query host '%.*s' through the system failed", 
            nai_str_len(hostname), nai_str(hostname));

        goto _end;
    };

    r = 0;

_end:
    return (nai_int_t)r;
};


nai_int_t nai_agent_node_connect(
    nai_agent_attr_t* a, nai_iobase_t* c, 
    nai_agent_dest_t* dest)
{
    intptr_t r;
    nai_agent_node_t* n;
    nai_agent_nconf_t* ac;
    nai_socknbuf_in_t nbuf;


    n = a->node;
    ac = (nai_agent_nconf_t*)n->ctx;
    if (ac->type == NAI_AGENT_ADDR_DOMAIN) {
        /* asynchronous dns query and connect */
        if (a->dns && ac->agent->loop && !nai_iobase_is_blocking(c)) {
            r = nai_agent_node_dns_query(a, c, dest);
            goto _end;
        };

        /* synchronous dns query */
        r = nai_agent_node_query_host(a, &ac->addr, &nbuf);
        if (r < 0) {
            nai_log_error(NAI_LOG_CORE, 
                nai_errno, "query node '%.*s' failed", 
                nai_str_len(&ac->addr), nai_str(&ac->addr));

            goto _end;
        };
    } else {
        r = nai_agent_make_sockaddr(
            &nbuf, ac->type, nai_str(&ac->addr), ac->port);
        if (r < 0) {
            nai_log_error(NAI_LOG_CORE, 
                nai_errno, "make socket address failed");
            goto _end;
        };
    };

    nai_sockname_setbuf(&dest->proxy, &nbuf);
    r = nai_agent_node_do_connect(a, c, dest);

_end:
    return (nai_int_t)r;
};


