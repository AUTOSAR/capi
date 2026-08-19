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
/// @file       nai_iobase.c
/// @brief      
/// @details
/// @date       2020-11-28
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai_iobase.h"
#include "nai_agent_io.h"
#include "nai/os/nai_tlocal.h"
#include "nai/runtime/nai_log.h"


#if defined(_WIN32)
#include <winsock2.h>
#define SO_EXCLUADDR SO_EXCLUSIVEADDRUSE
#define SO_REUSEPORT SO_REUSEADDR
#endif
#if (NAI_HAVE_SYS_TIME_H)
#include <sys/time.h>
#endif


//////////////////////////////////////////////////////////////////////////////
// iobase


nai_int_t nai_iobase_init(nai_iobase_t* s)
{
    nai_int_t r;


    nai_evnode_init(&s->ev);
    s->cb = 0;
    s->offset = -1;
    s->st.flags = 0;
    s->st.mode = NAI_EV_READ|NAI_EV_WRITE;
    s->st.timeochg = 0;
    s->st.reuseaddr = 3;
    s->st.reuseport = 3;
    s->st.ops = 0;
    s->st.ctx = 0;
    s->st.timer[0] = s->st.timer[1] = s->st.timer[2] = 0;
    s->st.timeout[0] = s->st.timeout[1] = -1;
    r = 0;

    return r;
};


nai_int_t nai_iobase_set_cb(nai_iobase_t* s, nai_iobase_cb_f cb)
{
    s->cb = cb;
    return 0;
};


nai_int_t nai_iobase_set_fd(nai_iobase_t* s, nai_fd_t fd, nai_int_t type)
{
    nai_int_t r;


    if (s->st.ops) {
        nai_errno = EALREADY;
        r = -1;
        goto _end;
    };

    r = nai_evnode_set_fd(&s->ev, fd, type);

_end:
    return r;
};


nai_int_t nai_iobase_set_fdown(nai_iobase_t* s, nai_int_t own)
{
    s->st.fdown = !!own;
    return 0;
};


nai_int_t nai_iobase_set_subtype(nai_iobase_t* s, nai_int_t type)
{
    nai_int_t r;


    if (s->st.ops) {
        nai_errno = EALREADY;
        r = -1;
        goto _end;
    };

    s->st.subtype = type;
    r = 0;

_end:
    return r;
};


nai_int_t nai_iobase_set_mode(nai_iobase_t* s, nai_int_t mode)
{
    nai_int_t r;
    nai_iobase_ops_t* ops;


    ops = (nai_iobase_ops_t*)s->st.ops;
    if (ops == 0) {
        s->st.mode = mode;
        r = 0;
    } else {
        r = ops->setopt(s, NAI_IO_MODE, mode);
    };

    return r;
};


nai_int_t nai_iobase_set_wait(nai_iobase_t* s, nai_int_t events)
{
    nai_int_t r;
    nai_iobase_ops_t* ops;


    ops = (nai_iobase_ops_t*)s->st.ops;
    if (ops) {
        nai_errno = EALREADY;
        r = -1;
        goto _end;
    };

    r = nai_iobase_blocked(s, s->st.mode & events);

_end:
    return r;
};


nai_int_t nai_iobase_set_polling(nai_iobase_t* s, nai_int_t pref)
{
    nai_int_t r;
    nai_iobase_ops_t* ops;


    ops = (nai_iobase_ops_t*)s->st.ops;
    if (ops) {
        nai_errno = EALREADY;
        r = -1;
        goto _end;
    };

    s->st.polling = !!pref;
    r = 0;

_end:
    return r;
};


nai_int_t nai_iobase_set_blocking(nai_iobase_t* s, nai_int_t on)
{
    nai_int_t r;
    nai_iobase_ops_t* ops;


    ops = (nai_iobase_ops_t*)s->st.ops;
    if (ops == 0) {
        s->st.blocking = !!on;
        r = 0;
    } else {
        r = ops->setopt(s, NAI_IO_BLOCKING, on);
    };

    return r;
};


nai_int_t nai_iobase_set_except(nai_iobase_t* s, nai_int_t on)
{
    nai_int_t r;
    nai_int_t ev;
    nai_iobase_ops_t* ops;


    ops = (nai_iobase_ops_t*)s->st.ops;
    if (ops == 0) {
        s->ev.st.except = !!on;
        r = 0;
    } else if (nai_iobase_is_blocking(s)) {
        s->ev.st.except = !!on;
        r = 0;
    } else {
        ev = s->ev.st.seted;
        if (on) {
            ev |= NAI_EV_EXCEPT;
        } else {
            ev &= ~NAI_EV_EXCEPT;
        };
        r = nai_evnode_set_event(&s->ev, NAI_EV_SET, ev);
        if (r >= 0) {
            s->ev.st.except = !!on;
        };
    };

    return r;
};


nai_int_t nai_iobase_set_timeout(nai_iobase_t* s, nai_int_t op, int32_t msec)
{
    nai_int_t r;
    nai_int_t ev;
    nai_int_t e = 2;
    nai_int_t get;
    uint32_t now;


    if (msec < -1) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    if (s->st.blocking) {
        nai_errno = EPERM;
        r = -1;
        goto _end;
    };

    get = 0;

    /* user custom timer */
    ev = 1<<e;
    if (msec != -1) {
        if (op == NAI_TIMEOP_ADD) {
            now = s->st.timer[e];
        } else {
            get = 1;
            now = nai_tickcache_to_msec32();
            switch (op) {
            case NAI_TIMEOP_SET:
            case NAI_TIMEOP_ADD:
                break;
            case NAI_TIMEOP_MIN:
                if ((s->st.timerset & ev) && 
                    (int32_t)(s->st.timer[e] - now) <= msec) {
                    /* do nothing */
                    r = 1;
                    goto _end;
                };
                break;
            case NAI_TIMEOP_MAX:
                if ((s->st.timerset & ev) && 
                    (int32_t)(s->st.timer[e] - now) >= msec) {
                    /* do nothing */
                    r = 1;
                    goto _end;
                };
                break;
            default:
                nai_errno = EINVAL;
                r = -1;
                goto _end;
            };
        };
        s->st.timerset |= ev;
        s->st.timer[e] = now + msec;
        if (s->st.timer[e] == 0) {
            s->st.timer[e] = 1;
        };
    } else {
        if (!(s->st.timerset & ev)) {
            r = 0;
            goto _end;
        };
        s->st.timerset &= ~ev;
        s->st.timer[e] = 0;
    };

    /* update timer */
    if (s->st.timerset) {
        if (get == 0) {
            now = nai_tickcache_to_msec32();
        };
        r = nai_iobase_update_timer(s, now);
    } else {
        if (!s->st.timerevt) {
            r = 0;
            goto _end;
        };
        r = nai_evnode_set_timeout(&s->ev, 0, -1);
        if (r >= 0) {
            s->st.timerevt = 0;
        };
    };

_end:
    return r;
};


nai_int_t nai_iobase_set_opt(nai_iobase_t* s, nai_int_t opt, intptr_t value)
{
    nai_int_t r;
    nai_iobase_ops_t* ops;


    ops = (nai_iobase_ops_t*)s->st.ops;
    if (ops) {
        r = ops->setopt(s, opt, value);
        goto _end;
    };

    r = nai_iobase_default_setopt(s, opt, value);

_end:
    return r;
};


nai_int_t nai_iobase_get_opt(nai_iobase_t* s, nai_int_t opt, intptr_t* value)
{
    nai_int_t r;
    nai_iobase_ops_t* ops;


    ops = (nai_iobase_ops_t*)s->st.ops;
    if (ops) {
        r = ops->getopt(s, opt, value);
        goto _end;
    };

    r = nai_iobase_default_getopt(s, opt, value);

_end:
    return r;
};


nai_int_t nai_iobase_get_pending(nai_iobase_t* s)
{
    nai_int_t r;
    intptr_t value;
    nai_iobase_ops_t* ops;


    ops = (nai_iobase_ops_t*)s->st.ops;
    if (ops) {
        r = ops->getopt(s, NAI_IO_PENDING, &value);
    } else {
        r = nai_iobase_default_getopt(s, NAI_IO_PENDING, &value);
    };

    r = (nai_int_t)value;

    return r;
};


nai_int_t nai_iobase_inherit(nai_iobase_t* s, nai_server_t* l)
{
    nai_int_t r;
    nai_int_t type;


    if (s->st.ops) {
        nai_errno = EALREADY;
        r = -1;
        goto _end;
    };
    if (l->st.type != NAI_IO_TYPE_SERVER) {
        nai_errno = ENOTSUP;
        r = -1;
        goto _end;
    };

    type = nai_evnode_get_type(&l->ev);
    if (type == NAI_FD_TYPE_NONE) {
        nai_errno = ENOTSUP;
        r = -1;
        goto _end;
    };
    if (type != nai_evnode_get_type(&s->ev)) {
        nai_errno = ENOTSUP;
        r = -1;
        goto _end;
    };


    s->st.subtype = l->st.subtype;
    s->st.family = l->st.family;
    s->st.sendfile = l->st.sendfile;
    s->st.loadfile = l->st.loadfile;
    s->st.blocking = l->st.blocking;
    s->st.blockset = l->st.blockset;
    s->st.timeosup = l->st.timeosup;
    s->st.timeoset = l->st.timeoset;
#if defined(_WIN32)
    /* on win32, the accepted socket inherit the listening socket */
    s->st.timeochg = l->st.timeochg;
#else
    s->st.timeochg = l->st.timeoset;
#endif
    s->st.timeout[0] = l->st.timeout[0];
    s->st.timeout[1] = l->st.timeout[1];
    r = 0;

_end:
    return r;
};


nai_int_t nai_iobase_post(nai_iobase_t* s, nai_int_t sigid)
{
    nai_int_t r;


    r = nai_evnode_post(&s->ev, sigid);

    return r;
};


nai_int_t nai_iobase_join(nai_iobase_t* s, nai_int_t slot, nai_int_t sigid)
{
    nai_int_t r;


    r = nai_evnode_join(&s->ev, slot, sigid);

    return r;
};


nai_int_t nai_iobase_return_loop(nai_iobase_t* s)
{
    nai_int_t r;


    if (s->st.ops == 0) {
        nai_errno = NAI_ECLOSED;
        r = -1;
        goto _end;
    };
    if (s->ev.loop == 0) {
        nai_errno = NAI_ECLOSED;
        r = -1;
        goto _end;
    };

    if (s->st.blocking && 
        !nai_evloop_in_dispatch(s->ev.loop)) {
        r = nai_iobase_post_signal(s);
    } else {
        r = 0;
    };

_end:
    return r;
};


nai_int_t nai_iobase_close(nai_iobase_t* s)
{
    nai_int_t r;
    nai_fd_t fd;
    nai_iobase_ops_t* ops;


    ops = (nai_iobase_ops_t*)s->st.ops;
    if (ops) {
        r = ops->close(s);
    } else {
        r = nai_evnode_close(&s->ev);
    };
    if (r < 0) {
        goto _end;
    };


    /* reset option */
    s->st.family = 0;
    s->st.blocked = 0;
    s->st.blockset = 0;
    s->st.timeochg = s->st.timeoset;

    /* free agent context */
    if (s->st.agent) {
        nai_agent_context_free(s);
    };

    /* free fd */
    fd = nai_evnode_get_fd(&s->ev);
    if (s->st.fdown && fd != NAI_FD_INVALID) {
        switch (nai_evnode_get_type(&s->ev)) {
        case NAI_FD_TYPE_FILE:
        case NAI_FD_TYPE_PIPE:
        case NAI_FD_TYPE_DEVC:
            nai_file_close(fd);
            break;
        case NAI_FD_TYPE_SOCK:
            nai_sock_close(fd);
            break;
        default:
            break;
        };

        nai_evnode_set_fd(&s->ev, NAI_FD_INVALID, NAI_FD_TYPE_NONE);
    };

    s->st.fdown = 0;
    r = 0;

_end:
    return r;
};


nai_int_t nai_iobase_finalize(nai_iobase_t* s)
{
    nai_int_t r;
    nai_iobase_ctx_t* a;


    if (nai_iobase_get_pending(s)) {
        /* do cancel */
        r = nai_iobase_set_opt(s, NAI_IO_CANCEL, NAI_IO_READWRITE);
        if (r < 0) {
            goto _end;
        };
    };
    if (nai_iobase_get_pending(s)) {
        /* remove events */
        nai_iobase_set_mode(s, 0);
        nai_iobase_set_except(s, 0);

        /* mark 'finalize' */
        a = (nai_iobase_ctx_t*)s->st.ctx;
        a->finalize = 1;
        nai_errno = EINPROGRESS;
        r = -1;
    } else {
        r = nai_iobase_close(s);
    };

_end:
    return r;
};


nai_int_t nai_iobase_shutdown(nai_iobase_t* s, nai_int_t how)
{
    nai_int_t r;
    nai_iobase_ops_t* ops;


    ops = (nai_iobase_ops_t*)s->st.ops;
    if (ops == 0) {
        nai_errno = NAI_ECLOSED;
        r = -1;
        goto _end;
    };

    r = ops->shutdown(s, how);

_end:
    return r;
};


intptr_t nai_iobase_read(nai_iobase_t* s, void* buf, size_t len)
{
    intptr_t r;
    nai_stream_ops_t* ops;


    ops = (nai_stream_ops_t*)s->st.ops;
    if (ops == 0) {
        nai_errno = NAI_ECLOSED;
        r = -1;
        goto _end;
    };

    switch (s->st.type) {
    case NAI_IO_TYPE_STREAM:
    case NAI_IO_TYPE_DGRAM:
        r = ops->read(s, buf, len);
        break;
    default:
        nai_errno = ENOTSUP;
        r = -1;
        break;
    };

_end:
    return r;
};


intptr_t nai_iobase_readv(
    nai_iobase_t* s, nai_bufvec_t* v, nai_int_t count)
{
    intptr_t r;
    nai_stream_ops_t* ops;


    ops = (nai_stream_ops_t*)s->st.ops;
    if (ops == 0) {
        nai_errno = NAI_ECLOSED;
        r = -1;
        goto _end;
    };

    switch (s->st.type) {
    case NAI_IO_TYPE_STREAM:
    case NAI_IO_TYPE_DGRAM:
        r = ops->readv(s, v, count);
        break;
    default:
        nai_errno = ENOTSUP;
        r = -1;
        break;
    };

_end:
    return r;
};


intptr_t nai_iobase_write(nai_iobase_t* s, const void* buf, size_t len)
{
    intptr_t r;
    nai_stream_ops_t* ops;


    ops = (nai_stream_ops_t*)s->st.ops;
    if (ops == 0) {
        nai_errno = NAI_ECLOSED;
        r = -1;
        goto _end;
    };

    switch (s->st.type) {
    case NAI_IO_TYPE_STREAM:
    case NAI_IO_TYPE_DGRAM:
        r = ops->write(s, buf, len);
        break;
    default:
        nai_errno = ENOTSUP;
        r = -1;
        break;
    };

_end:
    return r;
};


intptr_t nai_iobase_writev(
    nai_iobase_t* s, const nai_bufvec_t* v, nai_int_t count)
{
    intptr_t r;
    nai_stream_ops_t* ops;


    ops = (nai_stream_ops_t*)s->st.ops;
    if (ops == 0) {
        nai_errno = NAI_ECLOSED;
        r = -1;
        goto _end;
    };

    switch (s->st.type) {
    case NAI_IO_TYPE_STREAM:
    case NAI_IO_TYPE_DGRAM:
        r = ops->writev(s, v, count);
        break;
    default:
        nai_errno = ENOTSUP;
        r = -1;
        break;
    };

_end:
    return r;
};


intptr_t nai_iobase_readq(nai_iobase_t* s, nai_buflist_t* in, size_t limit)
{
    intptr_t r;
    nai_stream_ops_t* ops;


    ops = (nai_stream_ops_t*)s->st.ops;
    if (ops == 0) {
        nai_errno = NAI_ECLOSED;
        r = -1;
        goto _end;
    };

    switch (s->st.type) {
    case NAI_IO_TYPE_STREAM:
    case NAI_IO_TYPE_DGRAM:
        r = ops->readq(s, in, limit);
        break;
    default:
        nai_errno = ENOTSUP;
        r = -1;
        break;
    };

_end:
    return r;
};


intptr_t nai_iobase_writeq(nai_iobase_t* s, nai_buflist_t* out, size_t limit)
{
    intptr_t r;
    nai_stream_ops_t* ops;


    ops = (nai_stream_ops_t*)s->st.ops;
    if (ops == 0) {
        nai_errno = NAI_ECLOSED;
        r = -1;
        goto _end;
    };

    switch (s->st.type) {
    case NAI_IO_TYPE_STREAM:
    case NAI_IO_TYPE_DGRAM:
        r = ops->writeq(s, out, limit);
        break;
    default:
        nai_errno = ENOTSUP;
        r = -1;
        break;
    };

_end:
    return r;
};


intptr_t nai_iobase_sendfile(nai_iobase_t* s, 
    nai_fd_t fd, size_t size, nai_off64_t offset)
{
    intptr_t r;
    intptr_t sup;
    nai_int_t ec;
    nai_buf_t* b;
    nai_buflist_t list;
    struct {
        nai_buf_t b;
        nai_buf_extra_t e;
    } buf;


    if (!s->st.loadfile) {
        if (!s->st.sendfile) {
            nai_errno = ENOTSUP;
            r = -1;
            goto _end;
        };

        r = nai_iobase_get_opt(s, NAI_IO_FEAT_SENDFILE, &sup);
        if (r < 0) {
            goto _end;
        };

        if (!sup) {
            nai_errno = ENOTSUP;
            r = -1;
            goto _end;
        };
    };


    /* make temprary file buffer */
    buf.b.offset = offset;
    buf.b.size = size;
    buf.b.total = size;
    buf.b.flags = 0;
    buf.b.type = NAI_BUF_FILE;
    buf.b.typeref = NAI_BUF_FILE;
    buf.b.extra = 1;
    buf.b.refcount = 2;
    buf.b.ref.obj = 0;
    buf.b.pool = 0;
    buf.e.fd = fd;
    buf.e.ops = 0;

    nai_buflist_init(&list, 0);
    nai_buflist_insert_tail(&list, &buf.b);


    /* sendfile */
    r = nai_iobase_writeq(s, &list, size);


    /* check bufflist */
    if (!nai_buflist_is_empty(&list)) {
        b = (nai_buf_t*)list.ent.next;

        /* test memory leak */
        assert(nai_buf_in_file(b));


        if (nai_buf_in_memory(b)) {
            nai_log_debug(NAI_LOG_CORE, 0, 
                "the iobase is not support sendfile, "
                "we will discard the unsent content which is "
                "transported in memory");

            if (r < 0) {
                ec = nai_errno;
            };

            nai_buflist_close(&list);

            if (r < 0) {
                nai_errno = ec;
            };
        };
    };

_end:
    return r;
};


intptr_t nai_iobase_recv(nai_iobase_t* s, 
    void* buf, size_t len, nai_int_t flags)
{
    intptr_t r;
    nai_stream_ops_t* ops;


    ops = (nai_stream_ops_t*)s->st.ops;
    if (ops == 0) {
        nai_errno = NAI_ECLOSED;
        r = -1;
        goto _end;
    };

    switch (s->st.type) {
    case NAI_IO_TYPE_STREAM:
    case NAI_IO_TYPE_DGRAM:
        r = ops->recv(s, buf, len, flags, 0, 0);
        break;
    default:
        nai_errno = ENOTSUP;
        r = -1;
        break;
    };

_end:
    return r;
};


intptr_t nai_iobase_recvv(nai_iobase_t* s, 
    nai_bufvec_t* v, nai_int_t count, nai_int_t flags)
{
    intptr_t r;
    nai_stream_ops_t* ops;


    ops = (nai_stream_ops_t*)s->st.ops;
    if (ops == 0) {
        nai_errno = NAI_ECLOSED;
        r = -1;
        goto _end;
    };

    switch (s->st.type) {
    case NAI_IO_TYPE_STREAM:
    case NAI_IO_TYPE_DGRAM:
        r = ops->recvm(s, v, count, flags, 0, 0, 0, 0);
        break;
    default:
        nai_errno = ENOTSUP;
        r = -1;
        break;
    };

_end:
    return r;
};


intptr_t nai_iobase_recvfrom(nai_iobase_t* s, 
    void* buf, size_t len, nai_int_t flags, 
    nai_sockaddr_t* name, nai_int_t* namelen)
{
    intptr_t r;
    nai_dgram_ops_t* ops;


    ops = (nai_dgram_ops_t*)s->st.ops;
    if (ops == 0) {
        nai_errno = NAI_ECLOSED;
        r = -1;
        goto _end;
    };
    if (s->st.type != NAI_IO_TYPE_DGRAM) {
        nai_errno = ENOTSUP;
        r = -1;
        goto _end;
    };

    r = ops->recv(s, buf, len, flags, name, namelen);

_end:
    return r;
};


intptr_t nai_iobase_send(nai_iobase_t* s, 
    const void* buf, size_t len, nai_int_t flags)
{
    intptr_t r;
    nai_stream_ops_t* ops;


    ops = (nai_stream_ops_t*)s->st.ops;
    if (ops == 0) {
        nai_errno = NAI_ECLOSED;
        r = -1;
        goto _end;
    };

    switch (s->st.type) {
    case NAI_IO_TYPE_STREAM:
    case NAI_IO_TYPE_DGRAM:
        r = ops->send(s, buf, len, flags, 0, 0);
        break;
    default:
        nai_errno = ENOTSUP;
        r = -1;
        break;
    };

_end:
    return r;
};


intptr_t nai_iobase_sendv(nai_iobase_t* s, 
    const nai_bufvec_t* v, nai_int_t count, nai_int_t flags)
{
    intptr_t r;
    nai_stream_ops_t* ops;


    ops = (nai_stream_ops_t*)s->st.ops;
    if (ops == 0) {
        nai_errno = NAI_ECLOSED;
        r = -1;
        goto _end;
    };

    switch (s->st.type) {
    case NAI_IO_TYPE_STREAM:
    case NAI_IO_TYPE_DGRAM:
        r = ops->sendm(s, v, count, flags, 0, 0, 0, 0);
        break;
    default:
        nai_errno = ENOTSUP;
        r = -1;
        break;
    };

_end:
    return r;
};


intptr_t nai_iobase_sendto(nai_iobase_t* s, 
    const void* buf, size_t len, nai_int_t flags, 
    const nai_sockaddr_t* name, nai_int_t namelen)
{
    intptr_t r;
    nai_dgram_ops_t* ops;


    ops = (nai_dgram_ops_t*)s->st.ops;
    if (ops == 0) {
        nai_errno = NAI_ECLOSED;
        r = -1;
        goto _end;
    };
    if (s->st.type != NAI_IO_TYPE_DGRAM) {
        nai_errno = ENOTSUP;
        r = -1;
        goto _end;
    };

    r = ops->send(s, buf, len, flags, name, namelen);

_end:
    return r;
};


intptr_t nai_iobase_recvm(nai_iobase_t* s, 
    nai_bufvec_t* v, nai_int_t count, nai_int_t flags, 
    nai_sockaddr_t* name, nai_int_t* namelen, 
    void* ctrl, nai_int_t* ctrllen)
{
    intptr_t r;
    nai_stream_ops_t* ops;


    ops = (nai_stream_ops_t*)s->st.ops;
    if (ops == 0) {
        nai_errno = NAI_ECLOSED;
        r = -1;
        goto _end;
    };

    switch (s->st.type) {
    case NAI_IO_TYPE_STREAM:
    case NAI_IO_TYPE_DGRAM:
        r = ops->recvm(s, v, count, flags, name, namelen, ctrl, ctrllen);
        break;
    default:
        nai_errno = ENOTSUP;
        r = -1;
        break;
    };

_end:
    return r;
};


intptr_t nai_iobase_sendm(nai_iobase_t* s, 
    const nai_bufvec_t* v, nai_int_t count, nai_int_t flags, 
    const nai_sockaddr_t* name, nai_int_t namelen, 
    const void* ctrl, nai_int_t ctrllen)
{
    intptr_t r;
    nai_stream_ops_t* ops;


    ops = (nai_stream_ops_t*)s->st.ops;
    if (ops == 0) {
        nai_errno = NAI_ECLOSED;
        r = -1;
        goto _end;
    };

    switch (s->st.type) {
    case NAI_IO_TYPE_STREAM:
    case NAI_IO_TYPE_DGRAM:
        r = ops->sendm(s, v, count, flags, name, namelen, ctrl, ctrllen);
        break;
    default:
        nai_errno = ENOTSUP;
        r = -1;
        break;
    };

_end:
    return r;
};


intptr_t nai_iobase_sendmm(nai_iobase_t* s, 
    const nai_bufarray_t* v, nai_int_t count, nai_int_t flags, 
    const nai_sockaddr_t* name, nai_int_t namelen)
{
    intptr_t r;
    nai_stream_ops_t* ops;


    ops = (nai_stream_ops_t*)s->st.ops;
    if (ops == 0) {
        nai_errno = NAI_ECLOSED;
        r = -1;
        goto _end;
    };

    switch (s->st.type) {
    case NAI_IO_TYPE_STREAM:
    case NAI_IO_TYPE_DGRAM:
        r = ops->sendmm(s, v, count, flags, name, namelen);
        break;
    default:
        nai_errno = ENOTSUP;
        r = -1;
        break;
    };

_end:
    return r;
};



//////////////////////////////////////////////////////////////////////////////
// iobase timer operations


nai_int_t nai_iobase_update_timer(nai_iobase_t* s, uint32_t now)
{
    nai_int_t r;
    nai_int_t n;
    nai_int_t first;
    nai_int_t t = 0, to;


    if (!s->st.timerset) {
        r = 0;
        goto _end;
    };

    /* find recent timer */
    first = 1;
    for (n = 0; n < 3; n ++) {
        if (!(s->st.timerset & (1<<n))) {
            continue;
        };
        if (first) {
            first = 0;
            t = s->st.timer[n];
        } else if ((nai_int_t)(t - s->st.timer[n]) > 0) {
            t = s->st.timer[n];
        };
    };

    assert(!first);
    to = t - now;
    if (to < 0) {
        to = 0;
    };

    r = nai_evnode_set_timeout(&s->ev, 0, to);
    if (r >= 0) {
        s->st.timerevt = 1;
    };

_end:
    return r;
};


nai_int_t nai_iobase_pop_timer(nai_iobase_t* s, uint32_t now)
{
    nai_int_t r;
    nai_int_t n;
    nai_int_t t, ev;

    if (!s->st.timerset) {
        r = 0;
        goto _end;
    };

    /* find recent timer */
    for (n = 0; n < 3; n ++) {
        ev = (1<<n);
        if (!(s->st.timerset & ev)) {
            continue;
        };

        /* check timer */
        t = s->st.timer[n] - now;
        if (t > 0) {
            continue;
        };

        /* unset timer flag */
        s->st.timerset &= ~ ev;

        /* if n is 2 means user's timer, return NAI_EV_TIMEOUT */
        if (n == 2) {
            ev = NAI_EV_TIMEOUT;
        };

        r = ev;
        goto _end;
    };

    r = 0;

_end:
    return r;
};


nai_int_t nai_iobase_set_timer(nai_iobase_t* s, nai_int_t events)
{
    nai_int_t r;
    nai_int_t n;
    nai_int_t ev;
    uint32_t now;

    /* if (s->ev.loop == 0) { */
    /*     return 0; */
    /* }; */

    events &= s->st.timeoset;
    if (events == 0) {
        r = 0;
        goto _end;
    };

    now = nai_tickcache_to_msec32();
    s->st.timerset |= events;
    for (n = 0; n < 2; n ++) {
        ev = (1<<n);
        if (!(events & ev)) {
            continue;
        };
        s->st.timer[n] = now + s->st.timeout[n];
    };

    r = nai_iobase_update_timer(s, now);

_end:
    return r;
};


nai_int_t nai_iobase_kill_timer(nai_iobase_t* s, nai_int_t events)
{
    nai_int_t r = 0;
    nai_int_t unset;
    uint32_t now;


    unset = s->st.timerset & events;
    if (unset) {
        s->st.timerset &= ~events;
        if (s->st.timerset) {
            now = nai_tickcache_to_msec32();
            r = nai_iobase_update_timer(s, now);
        } else if (s->st.timerevt) {
            s->st.timerevt = 0;
            r = nai_evnode_set_timeout(&s->ev, 0, -1);
        };
    };

    return r;
};


nai_int_t nai_iobase_blocked(nai_iobase_t* s, nai_int_t events)
{
    s->st.blocked |= events;
    return nai_iobase_set_timer(s, events);
};


nai_int_t nai_iobase_unblocked(nai_iobase_t* s, nai_int_t events)
{
    s->st.blocked &= ~events;
    return nai_iobase_kill_timer(s, events);
};


//////////////////////////////////////////////////////////////////////////////
// iobase fd operations


#if !defined(SO_RCVTIMEO)


nai_int_t nai_iobase_set_fd_timeo(nai_iobase_t* s, nai_int_t rw)
{
    (void)s;
    (void)rw;

    return 0;
};


#else


static nai_int_t sockopt_timeo[2] = { 
    SO_RCVTIMEO, SO_SNDTIMEO
};

nai_int_t nai_iobase_set_fd_timeo(nai_iobase_t* s, nai_int_t rw)
{
    nai_int_t r;
    nai_fd_t fd;
    uint32_t timeo;


    if (nai_evnode_get_type(&s->ev) != NAI_FD_TYPE_SOCK) {
        r = 0;
        goto _end;
    };


    timeo = s->st.timeout[rw];
    if (timeo == 0) {
        timeo = 1;      /* zero means never timeout */
    } else if (timeo == (uint32_t)-1) {
        timeo = 0;      /* never timeout */
    };

    fd = nai_evnode_get_fd(&s->ev);

#if defined(_WIN32)

    r = nai_sock_set_opt(fd, 
        SOL_SOCKET, sockopt_timeo[rw], (char*)&timeo, sizeof(timeo));

#else

    struct timeval to;
    to.tv_sec = timeo / 1000;
    to.tv_usec = timeo % 1000 * 1000;
    r = nai_sock_set_opt(fd, 
        SOL_SOCKET, sockopt_timeo[rw], (char*)&to, sizeof(to));

#endif

_end:
    return r;
};


#endif


nai_int_t nai_iobase_set_fd_blocking_impl(nai_iobase_t* s, nai_int_t on)
{
    nai_int_t r;
    nai_int_t type;
    nai_fd_t fd;


    fd = nai_evnode_get_fd(&s->ev);
    type = nai_evnode_get_type(&s->ev);
    switch (type) {
    case NAI_FD_TYPE_SOCK:
        r = nai_sock_set_blocking(fd, on);
        break;

    case NAI_FD_TYPE_FILE:
        r = 0;
        break;

#if !defined(_WIN32)
    default:
        r = nai_file_set_blocking(fd, on);
        break;
#else
    default:
        r = 0;
        break;
#endif
    };

    if (r >= 0) {
        s->st.blockset = on;
    };

    return r;
};


nai_int_t nai_iobase_set_fd_blocking_init(nai_iobase_t* s, nai_int_t on)
{
    nai_int_t r;


    if (!s->st.timeosup) {
        if (on) {
            on = !(s->st.timeoset&s->st.mode);
        };
    };

    r = nai_iobase_set_fd_blocking_impl(s, on);

    return r;
};


nai_int_t nai_iobase_set_fd_blocking(nai_iobase_t* s, nai_int_t on)
{
    nai_int_t r;


    if (!s->st.timeosup) {
        if (on) {
            on = !(s->st.timeoset&s->st.mode);
        };
    };
    if (s->st.blockset == on) {
        r = 0;
    } else {
        r = nai_iobase_set_fd_blocking_impl(s, on);
    };

    return r;
};


nai_int_t nai_iobase_sock_shutdown(nai_iobase_t* s, nai_int_t how)
{
    nai_int_t r;
    nai_int_t ec;
    nai_int_t type;


    type = nai_evnode_get_type(&s->ev);
    if (type != NAI_FD_TYPE_SOCK) {
        r = 0;
        goto _end;
    };

    r = nai_sock_shutdown(nai_evnode_get_fd(&s->ev), how);
    if (r < 0) {
        ec = nai_errno;
        if (ec == ENOTCONN) {
            r = 0;
        };
    };

_end:
    return r;
};


nai_int_t nai_iobase_sock_wait(nai_iobase_t* s, nai_int_t rw)
{
    nai_fd_t fd;

    fd = nai_evnode_get_fd(&s->ev);
    return nai_sock_poll(fd, (1<<rw), s->st.timeout[rw]);
};


nai_int_t nai_iobase_file_wait(nai_iobase_t* s, nai_int_t rw)
{
    nai_fd_t fd;

    fd = nai_evnode_get_fd(&s->ev);
    return nai_file_poll(fd, (1<<rw), s->st.timeout[rw]);
};


nai_int_t nai_iobase_sock_get_error(nai_iobase_t* s)
{
    nai_int_t r;
    nai_int_t ec;
    nai_int_t len;
    nai_fd_t fd;

    fd = nai_evnode_get_fd(&s->ev);

    /* get socket last error */
    len = sizeof(ec);
    r = nai_sock_get_opt(
        fd, SOL_SOCKET, SO_ERROR, (char*)&ec, &len);
    if (r == 0 && ec) {
        nai_errno = ec;
        r = -1;
    };

    return r;
};


nai_int_t nai_iobase_sock_set_family(nai_iobase_t* s, nai_int_t af)
{
    nai_int_t r;


    switch (af) {
#if (NAI_HAVE_SOCKADDR_IN)
    case AF_INET:
        s->st.family = 1;
        break;
#endif
#if (NAI_HAVE_SOCKADDR_IN6)
    case AF_INET6:
        s->st.family = 2;
        break;
#endif
    default:
        s->st.family = 3;
        break;
    };

    r = 0;

    return r;
};


nai_int_t nai_iobase_sock_get_family(nai_iobase_t* s)
{
    nai_int_t r;
    nai_int_t value;
    nai_fd_t fd;


    fd = nai_evnode_get_fd(&s->ev);
    if (s->st.family == 0) {
#if defined(SO_DOMAIN)
        nai_int_t n;
        n = sizeof(value);
        r = nai_sock_get_opt(fd, SOL_SOCKET, SO_DOMAIN, (char*)&value, &n);
#else
        nai_socknbuf_t nbuf;
        nbuf.len = sizeof(nbuf.storage);
        r = nai_sock_get_sockname(fd, &nbuf.addr, &nbuf.len);
        value = nbuf.addr.sa_family;
#endif
        if (r < 0) {
            goto _end;
        };

        switch (value) {
#if (NAI_HAVE_SOCKADDR_IN)
        case AF_INET:
            s->st.family = 1;
            break;
#endif
#if (NAI_HAVE_SOCKADDR_IN6)
        case AF_INET6:
            s->st.family = 2;
            break;
#endif
        default:
            s->st.family = 3;
            break;
        };
    };

    r = 0;

_end:
    return r;
};


nai_int_t nai_iobase_sock_setopt(
    nai_iobase_t* s, nai_int_t opt, intptr_t val)
{
    nai_int_t r;
    nai_int_t ec;
    nai_int_t value;
    nai_fd_t fd;
    nai_sockaddr_t** sa;


    fd = nai_evnode_get_fd(&s->ev);
    if (fd == NAI_FD_INVALID) {
        ec = ENOTSUP;
        goto _fail;
    };
    if (nai_evnode_get_type(&s->ev) != NAI_FD_TYPE_SOCK) {
        ec = ENOTSUP;
        goto _fail;
    };

    switch (opt) {
    default:
        ec = ENOTSUP;
        goto _fail;

#if defined(SO_RCVBUF)
    case NAI_IO_RECVBUF:
        value = (nai_int_t)val;
        r = nai_sock_set_opt(fd, 
            SOL_SOCKET, SO_SNDBUF, (char*)&value, sizeof(value));
        break;
#endif

#if defined(SO_SNDBUF)
    case NAI_IO_SENDBUF:
        value = (nai_int_t)val;
        r = nai_sock_set_opt(fd, 
            SOL_SOCKET, SO_RCVBUF, (char*)&value, sizeof(value));
        break;
#endif

#if defined(SO_LINGER)
    case NAI_IO_LINGER: {
            struct linger li;
            li.l_onoff = val >= 0;
            li.l_linger = (nai_int_t)val;
            r = nai_sock_set_opt(fd, 
                SOL_SOCKET, SO_LINGER, (char*)&li, sizeof(li));
        };
        break;
#endif

#if !defined(TCP_NOPUSH) && defined(TCP_CORK)
#define TCP_NOPUSH TCP_CORK
#endif
#if defined(TCP_NOPUSH)
    case NAI_IO_NOPUSH:
        value = !!val;
        r = nai_sock_set_opt(fd, 
            IPPROTO_TCP, TCP_NOPUSH, (char*)&value, sizeof(value));
        break;
#endif

#if defined(TCP_NODELAY)
    case NAI_IO_NODELAY:
        value = !!val;
        r = nai_sock_set_opt(fd, 
            IPPROTO_TCP, TCP_NODELAY, (char*)&value, sizeof(value));
        break;
#endif

#if defined(SO_EXCLUADDR)
    case NAI_IO_REUSEADDR:
        value = !val;
        r = nai_sock_set_opt(fd, 
            SOL_SOCKET, SO_EXCLUADDR, (char*)&value, sizeof(value));
        break;
#elif defined(SO_REUSEADDR)
    case NAI_IO_REUSEADDR:
        value = !!val;
        r = nai_sock_set_opt(fd, 
            SOL_SOCKET, SO_REUSEADDR, (char*)&value, sizeof(value));
        break;
#endif

#if defined(SO_REUSEPORT)
    case NAI_IO_REUSEPORT:
        value = !!val;
        r = nai_sock_set_opt(fd, 
            SOL_SOCKET, SO_REUSEPORT, (char*)&value, sizeof(value));
        break;
#endif

    case NAI_IO_MULTICAST_JOIN:
    case NAI_IO_MULTICAST_DROP:
        sa = (nai_sockaddr_t**)val;
        if (sa == 0 || sa[0] == 0) {
            ec = EINVAL;
            goto _fail;
        };
        if (sa[1] && sa[0]->sa_family != sa[1]->sa_family) {
            ec = EINVAL;
            goto _fail;
        };

        value = opt == NAI_IO_MULTICAST_JOIN;
        switch (sa[0]->sa_family) {
#if defined(IP_ADD_MEMBERSHIP)
        case AF_INET: {
            struct ip_mreq m4;
            nai_sockaddr_in4_t** sa4 = (nai_sockaddr_in4_t**)sa;
            m4.imr_multiaddr = sa4[0]->sin_addr;
            if (sa4[1] && 
                sa4[1]->sin_addr.s_addr != m4.imr_multiaddr.s_addr) {
                m4.imr_interface = sa4[1]->sin_addr;
            } else {
                m4.imr_interface.s_addr = 0;
            };
            r = nai_sock_set_opt(fd, IPPROTO_IP, 
                value ? IP_ADD_MEMBERSHIP : IP_DROP_MEMBERSHIP, 
                (char*)&m4, sizeof(m4));
            break;
        };
#endif
#if defined(IPV6_ADD_MEMBERSHIP)
        case AF_INET6: {
            struct ipv6_mreq m6;
            nai_sockaddr_in6_t** sa6 = (nai_sockaddr_in6_t**)sa;
            m6.ipv6mr_multiaddr = sa6[0]->sin6_addr;
            if (sa6[0]->sin6_scope_id) {
                m6.ipv6mr_interface = sa6[0]->sin6_scope_id;
            } else if (sa6[1] != 0) {
                m6.ipv6mr_interface = sa6[1]->sin6_scope_id;
            } else {
                m6.ipv6mr_interface = 0;
            };
            r = nai_sock_set_opt(fd, IPPROTO_IPV6, 
                value ? IPV6_ADD_MEMBERSHIP : IPV6_DROP_MEMBERSHIP, 
                (char*)&m6, sizeof(m6));
            break;
        };
#endif
        default:
            ec = ENOTSUP;
            goto _fail;
        };
        break;

    case NAI_IO_MULTICAST_IF:
        sa = (nai_sockaddr_t**)&val;
        if (sa[0] == 0) {
            ec = EINVAL;
            goto _fail;
        };
        switch (sa[0]->sa_family) {
#if defined(IP_MULTICAST_IF)
        case AF_INET: {
            nai_sockaddr_in4_t* sa4 = (nai_sockaddr_in4_t*)sa[0];
            r = nai_sock_set_opt(fd, IPPROTO_IP, 
                IP_MULTICAST_IF, (char*)&sa4->sin_addr, sizeof(sa4->sin_addr));
            break;
        };
#endif
#if defined(IPV6_MULTICAST_IF)
        case AF_INET6: {
            nai_sockaddr_in6_t* sa6 = (nai_sockaddr_in6_t*)sa[0];
            r = nai_sock_set_opt(fd, IPPROTO_IPV6, 
                IPV6_MULTICAST_IF, 
                (char*)&sa6->sin6_scope_id, sizeof(sa6->sin6_scope_id));
            break;
        };
#endif
        default:
            ec = ENOTSUP;
            goto _fail;
        };
        break;

    case NAI_IO_MULTICAST_LOOP:
        if (s->st.family == 0) {
            r = nai_iobase_sock_get_family(s);
            if (r < 0) {
                break;
            };
        };
        switch (s->st.family) {
#if defined(IP_MULTICAST_LOOP)
        case 1: /* af_inet */
            value = !!val;
            r = nai_sock_set_opt(fd, IPPROTO_IP, 
                IP_MULTICAST_LOOP, (char*)&value, sizeof(value));
            break;
#endif
#if defined(IPV6_MULTICAST_LOOP)
        case 2:
            value = !!val;
            r = nai_sock_set_opt(fd, IPPROTO_IPV6, 
                IPV6_MULTICAST_LOOP, (char*)&value, sizeof(value));
            break;
#endif
        default:
            ec = ENOTSUP;
            goto _fail;
        };
        break;

    case NAI_IO_MULTICAST_TTL:
        if (s->st.family == 0) {
            r = nai_iobase_sock_get_family(s);
            if (r < 0) {
                break;
            };
        };
        switch (s->st.family) {
#if defined(IP_MULTICAST_TTL)
        case 1: /* af_inet */
            value = (nai_int_t)val;
            r = nai_sock_set_opt(fd, IPPROTO_IP, 
                IP_MULTICAST_TTL, (char*)&value, sizeof(value));
            break;
#endif
#if defined(IPV6_MULTICAST_HOPS)
        case 2:
            value = (nai_int_t)val;
            r = nai_sock_set_opt(fd, IPPROTO_IPV6, 
                IPV6_MULTICAST_HOPS, (char*)&value, sizeof(value));
            break;
#endif
        default:
            ec = ENOTSUP;
            goto _fail;
        };
        break;
    };


_end:
    return r;

_fail:
    nai_errno = ec;
    r = -1;
    goto _end;
};


nai_int_t nai_iobase_sock_getopt(
    nai_iobase_t* s, nai_int_t opt, intptr_t* val)
{
    nai_int_t r;
    nai_int_t n;
    nai_int_t* value;
    nai_fd_t fd;


    fd = nai_evnode_get_fd(&s->ev);
    if (fd == NAI_FD_INVALID) {
        goto _fail;
    };
    if (nai_evnode_get_type(&s->ev) != NAI_FD_TYPE_SOCK) {
        goto _fail;
    };

#if !(NAI_HAVE_BIG_ENDIAN) || NAI_SIZEOF_VOID_P == 4
    value = (nai_int_t*)val;
#if NAI_SIZEOF_VOID_P == 8
    value[1] = 0;
#endif
#elif NAI_SIZEOF_VOID_P == 8
    value = (nai_int_t*)val;
    value[0] = 0;
    value ++;
#else
#error "unknown pointer size cannot correct memory layout"
#endif

    switch (opt) {
    default:
        goto _fail;

#if defined(SO_SNDBUF)
    case NAI_IO_RECVBUF:
        n = sizeof(nai_int_t);
        r = nai_sock_get_opt(fd, 
            SOL_SOCKET, SO_SNDBUF, (char*)value, &n);
        break;
#endif

#if defined(SO_RCVBUF)
    case NAI_IO_SENDBUF:
        n = sizeof(nai_int_t);
        r = nai_sock_get_opt(fd, 
            SOL_SOCKET, SO_RCVBUF, (char*)value, &n);
        break;
#endif

#if defined(SO_LINGER)
    case NAI_IO_LINGER: {
            struct linger li;
            n = sizeof(li);
            r = nai_sock_get_opt(fd, 
                SOL_SOCKET, SO_LINGER, (char*)&li, &n);
            if (r >= 0) {
                if (li.l_onoff == 0) {
                    value[0] = -1;
                } else {
                    value[0] = li.l_linger;
                };
            };
        };
        break;
#endif

#if !defined(TCP_NOPUSH) && defined(TCP_CORK)
#define TCP_NOPUSH TCP_CORK
#endif
#if defined(TCP_NOPUSH)
    case NAI_IO_NOPUSH:
        n = sizeof(nai_int_t);
        r = nai_sock_get_opt(fd, 
            IPPROTO_TCP, TCP_NOPUSH, (char*)value, &n);
        break;
#endif

#if defined(TCP_NODELAY)
    case NAI_IO_NODELAY:
        n = sizeof(nai_int_t);
        r = nai_sock_get_opt(fd, 
            IPPROTO_TCP, TCP_NODELAY, (char*)value, &n);
        break;
#endif

#if defined(SO_EXCLUADDR)
    case NAI_IO_REUSEADDR:
        n = sizeof(nai_int_t);
        r = nai_sock_get_opt(fd, 
            SOL_SOCKET, SO_EXCLUADDR, (char*)value, &n);
        if (r >= 0) {
            value[0] = !value[0];
        };
        break;
#elif defined(SO_REUSEADDR)
    case NAI_IO_REUSEADDR:
        n = sizeof(nai_int_t);
        r = nai_sock_get_opt(fd, 
            SOL_SOCKET, SO_REUSEADDR, (char*)value, &n);
        break;
#endif

#if defined(SO_REUSEPORT)
    case NAI_IO_REUSEPORT:
        n = sizeof(nai_int_t);
        r = nai_sock_get_opt(fd, 
            SOL_SOCKET, SO_REUSEPORT, (char*)value, &n);
        break;
#endif

    case NAI_IO_MULTICAST_LOOP:
        if (s->st.family == 0) {
            r = nai_iobase_sock_get_family(s);
            if (r < 0) {
                break;
            };
        };
        switch (s->st.family) {
#if defined(IP_MULTICAST_LOOP)
        case 1: /* af_inet */
            n = sizeof(nai_int_t);
            r = nai_sock_get_opt(fd, 
                IPPROTO_IP, IP_MULTICAST_LOOP, (char*)value, &n);
            break;
#endif
#if defined(IPV6_MULTICAST_LOOP)
        case 2:
            n = sizeof(nai_int_t);
            r = nai_sock_get_opt(fd, 
                IPPROTO_IPV6, IPV6_MULTICAST_LOOP, (char*)value, &n);
            break;
#endif
        default:
            nai_errno = ENOTSUP;
            r = -1;
            break;
        };
        break;

    case NAI_IO_MULTICAST_TTL:
        if (s->st.family == 0) {
            r = nai_iobase_sock_get_family(s);
            if (r < 0) {
                break;
            };
        };
        switch (s->st.family) {
#if defined(IP_MULTICAST_TTL)
        case 1: /* af_inet */
            n = sizeof(nai_int_t);
            r = nai_sock_get_opt(fd, 
                IPPROTO_IP, IP_MULTICAST_TTL, (char*)value, &n);
            break;
#endif
#if defined(IPV6_MULTICAST_HOPS)
        case 2:
            n = sizeof(nai_int_t);
            r = nai_sock_get_opt(fd, 
                IPPROTO_IPV6, IPV6_MULTICAST_HOPS, (char*)value, &n);
            break;
#endif
        default:
            nai_errno = ENOTSUP;
            r = -1;
            break;
        };
        break;
    };

    return r;

_fail:
    nai_errno = ENOTSUP;
    r = -1;
    return r;
};


nai_int_t nai_iobase_default_setopt(
    nai_iobase_t* s, nai_int_t opt, intptr_t value)
{
    nai_int_t r;
    nai_int_t n;


    switch (opt) {
    case NAI_IO_MODE:
        s->st.mode = (nai_int_t)value;
        r = 0;
        break;

    case NAI_IO_BLOCKING:
        if (s->st.blocking == !!value) {
            r = 0;
            break;
        };

        if (value) {
            /* will become blocking, auto kill timer */
            r = nai_iobase_set_timeout(s, NAI_TIMEOP_SET, -1);
            if (r < 0) {
                break;
            };
        };

        s->st.blocking = !!value;
        r = 0;
        break;

    case NAI_IO_RECVTIMEO:
    case NAI_IO_SENDTIMEO:
        n = opt-NAI_IO_RECVTIMEO;
        if (s->st.timeout[n] != value) {
            s->st.timeout[n] = (nai_int_t)value;
            s->st.timeochg |= (1<<n);
            if (value != -1) {
                s->st.timeoset |= (1<<n);
            } else {
                s->st.timeoset &= ~(1<<n);
            };
        };
        r = 0;
        break;

    case NAI_IO_SENDFILE:
        s->st.sendfile = !!value;
        r = 0;
        break;

    case NAI_IO_LOADFILE:
        s->st.loadfile = !!value;
        r = 0;
        break;

#if defined(SO_REUSEADDR) || defined(SO_EXCLUADDR)
    case NAI_IO_REUSEADDR:
        if (nai_iobase_get_fd(s) == NAI_FD_INVALID) {
            s->st.reuseaddr = !!value;
            r = 0;
        } else {
            s->st.reuseaddr = 3;
            r = nai_iobase_sock_setopt(s, opt, value);
        };
        break;
#endif

#if defined(SO_REUSEPORT)
    case NAI_IO_REUSEPORT:
        if (nai_iobase_get_fd(s) == NAI_FD_INVALID) {
            s->st.reuseport = !!value;
            r = 0;
        } else {;
            s->st.reuseport = 3;
            r = nai_iobase_sock_setopt(s, opt, value);
        };
        break;
#endif

    case NAI_IO_CANCEL:
        r = 0;
        break;

    default:
        r = nai_iobase_sock_setopt(s, opt, value);
        break;
    };

    return r;
};


nai_int_t nai_iobase_default_getopt(
    nai_iobase_t* s, nai_int_t opt, intptr_t* value)
{
    nai_int_t r;
    nai_iobase_ctx_t* c;


    switch (opt) {
    case NAI_IO_MODE:
        value[0] = s->st.mode;
        r = 0;
        break;

    case NAI_IO_BLOCKING:
        value[0] = s->st.blocking;
        r = 0;
        break;

    case NAI_IO_PENDING:
        c = (nai_iobase_ctx_t*)s->st.ctx;
        if (c == 0 || s->st.blocking) {
            value[0] = 0;
        } else {
            value[0] = nai_iobase_ctx_pending(c);
        };
        r = 0;
        break;

    case NAI_IO_RECVTIMEO:
    case NAI_IO_SENDTIMEO:
        value[0] = s->st.timeout[opt-NAI_IO_RECVTIMEO];
        r = 0;
        break;

    case NAI_IO_SENDFILE:
        value[0] = s->st.sendfile;
        r = 0;
        break;

    case NAI_IO_LOADFILE:
        value[0] = s->st.loadfile;
        r = 0;
        break;

    case NAI_IO_FEAT_SENDFILE:
        value[0] = 0;
        r = 0;
        break;

    case NAI_IO_FEAT_VECTORIO:
        value[0] = 0;
        r = 0;
        break;

    case NAI_IO_FEAT_MODEL:
        value[0] = NAI_EV_FEAT_BLOCK;
        r = 0;
        break;

#if defined(SO_REUSEADDR) || defined(SO_EXCLUADDR)
    case NAI_IO_REUSEADDR:
        if (s->st.reuseaddr != 3) {
            value[0] = s->st.reuseaddr;
            r = 0;
        } else if (nai_iobase_get_fd(s) == NAI_FD_INVALID) {
#if defined(SO_EXCLUADDR) /* winsock */
            value[0] = 1;
#else
            value[0] = 0;
#endif
            r = 0;
        } else {
            r = nai_iobase_sock_getopt(s, opt, value);
        };
        break;
#endif

#if defined(SO_REUSEPORT)
    case NAI_IO_REUSEPORT:
        if (s->st.reuseport != 3) {
            value[0] = s->st.reuseport;
            r = 0;
        } else if (nai_iobase_get_fd(s) == NAI_FD_INVALID) {
            value[0] = 0;
            r = 0;
        } else {
            r = nai_iobase_sock_getopt(s, opt, value);
        };
        break;
#endif

    default:
        r = nai_iobase_sock_getopt(s, opt, value);
        break;
    };

    return r;
};



nai_int_t nai_iobase_ctx_wait(
    nai_iobase_ctx_t* p, nai_int_t which, uint32_t timeout)
{
    nai_int_t r;
    nai_int_t ec;
    nai_int_t stat;
    uint64_t timer;
    nai_mutex_t* m;


    m = nai_thread_mutex();
    if (m == 0) {
        r = -1;
        goto _end;
    };

    if (timeout == (uint32_t)-1) {
        timer = 0;
    } else {
        timer = nai_tickcache_to_usec() + (uint64_t)timeout * 1000;
    };


    nai_mutex_lock(m);

    if (p->wait) {
        ec = EBUSY;
        r = -1;
        goto _exit;
    };

    switch (which) {
    case 0: /* read */
        assert(p->proto != NAI_IOBASE_CTX_NB);
        stat = p->readstat;
        break;
    case 1: /* send */
        assert(p->proto != NAI_IOBASE_CTX_NB);
        stat = p->sendstat;
        break;
    case 2: /* load */
        stat = p->loadstat;
        break;
    default:
        assert(0);
        stat = 0;
        break;
    };
    if (stat != NAI_IOBASE_STAT_PENDING) {
        r = 0;
        goto _exit;
    }

    /* get thread local condition */
    p->wait = nai_thread_local_cond();
    if (p->wait == 0) {
        ec = nai_errno;
        r = -1;
        goto _exit;
    };

    /* wait loop */
    for (;;) {
        switch (which) {
        case 0: /* read */
            stat = p->readstat;
            break;
        case 1: /* send */
            stat = p->sendstat;
            break;
        case 2: /* load */
            stat = p->loadstat;
            break;
        default:
            assert(0);
            stat = 0;
            break;
        };
        if (stat != NAI_IOBASE_STAT_PENDING) {
            r = 0;
            break;
        };

        if (timer == 0) {
            r = nai_cond_wait(p->wait, m);
        } else {
            r = nai_cond_timedwait(p->wait, m, timer);
        };
        if (r < 0) {
            break;
        };
    };

    /* detach */
    p->wait = 0;

    if (r < 0) {
        ec = nai_errno;
    };


_exit:
    nai_mutex_unlock(m);
    if (r < 0) {
        nai_errno = ec;
    };

_end:
    return r;
};


nai_int_t nai_iobase_ctx_signal(nai_iobase_ctx_t* p)
{
    nai_int_t r;
    nai_mutex_t* m;


    m = nai_thread_mutex();
    if (m == 0) {
        r = -1;
        goto _end;
    };

    nai_mutex_lock(m);

    if (p->wait) {
        nai_cond_signal(p->wait);
    };

    nai_mutex_unlock(m);

    r = 0;

_end:
    return r;
};


nai_int_t nai_iobase_ctx_pending(nai_iobase_ctx_t* p)
{
    nai_int_t r;


    r = p->readstat == NAI_IOBASE_STAT_PENDING ? NAI_IO_READ : 0;
    if (p->sendstat == NAI_IOBASE_STAT_PENDING || 
        p->loadstat == NAI_IOBASE_STAT_PENDING) {
        r |= NAI_IO_WRITE;
    };

    return r;
};

