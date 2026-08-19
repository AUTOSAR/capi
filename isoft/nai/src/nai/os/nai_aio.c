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
/// @file       nai_aio.c
/// @brief      
/// @details
/// @date       2020-11-28
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai/os/nai_aio.h"
#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_log.h"
#include "nai/runtime/nai_util.h"


#if (NAI_AIO_USE_WIN32)


#include "nai/runtime/nai_util.h"
#include "win/nai_windows.h"
#include "win/nai_wsock.h"
#include <ioapiset.h>


#define nai_aio_set_fd(a, f) {                          \
    (a)->fd = (f);                                      \
    (a)->postpro = 0;                                   \
}                                                       \


#define nai_aio_return(a, bytes)                        \
    (bytes)                                             \


nai_aio_feature_t nai_aiofeat = {
    .support = 1, 
    .accept = 0, 
    .acceptex = 0, 
    .connect = 0, 
    .readv = 0, 
    .readv_sim = 0, 
    .writev = 0, 
    .writev_sim = 0, 
    .recv = 1, 
    .recvv = 1, 
    .recvv_sim = 1, 
    .recvfrom = 1, 
    .recvmsg = 1, 
    .recvmsg_sim = 1, 
    .recvmmsg = 0, 
    .recvmmsg_sim = 1, 
    .send = 1, 
    .sendv = 1, 
    .sendv_sim = 1, 
    .sendto = 1, 
    .sendmsg = 1, 
    .sendmsg_sim = 1, 
    .sendmmsg = 0, 
    .sendmmsg_sim = 1, 
    .sendfile = 0, 
    .sendfile_sim = 0, 
    .sendfilev = 0, 
    .sendfilev_sim = 0
};


nai_int_t nai_aio_init(nai_aio_t* a)
{
    nai_memset(&a->cb, 0x0, sizeof(a->cb));
    a->waitable = 0;
    a->fd = NAI_FD_INVALID;
    a->handle = 0;
    return 0;
};


nai_int_t nai_aio_set_port(nai_aio_t* a, nai_aio_port_t* p)
{
    return 0;
};


nai_int_t nai_aio_set_in_loop(nai_aio_t* a, nai_int_t in_loop)
{
    (void)a;
    (void)in_loop;

    return 0;
};


nai_int_t nai_aio_set_waitable(nai_aio_t* a, nai_int_t waitable)
{
    nai_int_t r;


    a->waitable = !!waitable;
    r = 0;

    return r;
};


nai_int_t nai_aio_wait(nai_aio_t* a, uint32_t msec)
{
    nai_int_t r;
    DWORD stat;
    HANDLE h;


    h = a->cb.hEvent;
    if (h == 0) {
        nai_errno = ENOTSUP;
        r = -1;
        goto _end;
    };

    /* unmark event */
    h = (HANDLE)((intptr_t)h & ~0x1);

    /* waiting */
    stat = WaitForSingleObject(h, msec);
    switch (stat) {
    case WAIT_OBJECT_0:
        r = 0;
        break;
    case WAIT_ABANDONED:
        r = -1;
        break;
    case WAIT_TIMEOUT:
        nai_errno = ETIMEDOUT;
        r = -1;
        break;
    default:
        nai_errno = nai_errno_from_oserr(GetLastError());
        r = -1;
        break;
    };

_end:
    return r;
};


nai_int_t nai_aio_cancel(nai_aio_t* a)
{
    nai_int_t r;
    DWORD ec;


    if (!CancelIoEx(a->fd, &a->cb)) {
        ec = GetLastError();
        if (ec == ERROR_NOT_FOUND) {
            nai_errno = EINVAL;
        } else if (ec == ERROR_OPERATION_ABORTED) {
            if (a->cb.hEvent) {
                /* waiting behavior */
                r = 0;
                goto _end;
            };
            nai_errno = EINPROGRESS;
        } else {
            nai_errno = nai_errno_from_oserr(ec);
        };
        r = -1;
        goto _end;
    };

#if 0 /* blocked forever */
    /* cancel successed, waiting the opeation compleated, see msdn */
    nai_aio_result(a, 1);
#endif
    if (a->cb.hEvent) {
        /* waiting behavior */
        r = 0;
    } else {
        /* default behavior */
        nai_errno = EINPROGRESS;
        r = -1;
    };

_end:
    return r;
};


intptr_t nai_aio_result(nai_aio_t* a, nai_int_t wait)
{
    nai_int_t r;
    DWORD ec;
    DWORD bytes;


    if (!GetOverlappedResult(a->fd, &a->cb, &bytes, wait)) {
        ec = GetLastError();
        if (ec == ERROR_IO_INCOMPLETE || 
            ec == ERROR_IO_PENDING) {
            nai_errno = EINPROGRESS;
        } else if (ec == ERROR_OPERATION_ABORTED) {
            nai_errno = ECANCELED;
        } else {
            nai_errno = nai_errno_from_oserr(ec);
        };
        r = -1;
        goto _end;
    };

    if (a->postpro) {
        r = nai_aio_postpro(a);
        if (r < 0) {
            goto _end;
        };
    };

    r = bytes;

_end:
    return r;
};


nai_int_t nai_aio_postpro(nai_aio_t* a)
{
    nai_int_t r;

    r = nai_sock_set_opt(a->fd, 
        SOL_SOCKET, SO_UPDATE_CONNECT_CONTEXT, 0, 0);

    if (r >= 0) {
        a->postpro = 0;
    };

    return r;
};


nai_int_t nai_aio_acceptex(nai_aio_t* a, nai_fd_t l, 
    nai_fd_t s, void* p, size_t size, nai_int_t extralen)
{
    nai_int_t r;
    DWORD ec;


    if (!nai_wsockops.AcceptEx) {
        nai_errno = ENOTSUP;
        r = -1;
        goto _end;
    };

    nai_aio_set_fd(a, s);
    r = nai_aio_set_event(a);
    if (r < 0) {
        goto _end;
    };

    if (!nai_wsockops.AcceptEx((SOCKET)l, 
        (SOCKET)s, p, (DWORD)size, extralen/2, extralen/2, 0, &a->cb)) {
        ec = GetLastError();
        switch (ec) {
        case ERROR_IO_PENDING:
            nai_errno = EINPROGRESS;
            r = -1;
            goto _end;
        default:
            nai_errno = nai_errno_from_oserr(ec);
            r = -1;
            goto _end;
        };
    };

    r = 0;

_end:
    return r;

};


nai_int_t nai_aio_acceptex_end(nai_fd_t l, 
    nai_fd_t s, void* p, size_t size, nai_int_t extralen, 
    nai_sockaddr_t** sockname, nai_int_t* socknamelen, 
    nai_sockaddr_t** peername, nai_int_t* peernamelen)
{
    nai_int_t r;
    nai_int_t namelen;
    nai_sockaddr_t* name;


    r = nai_sock_set_opt(s, 
        SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, (char*)&l, sizeof(l));
    if (r < 0) {
        goto _end;
    };

    if (sockname == 0) {
        sockname = &name;
    };
    if (socknamelen == 0) {
        socknamelen = &namelen;
    };
    if (peername == 0) {
        peername = &name;
    };
    if (peernamelen == 0) {
        peernamelen = &namelen;
    };

    if (nai_wsockops.GetAcceptExSockaddrs) {
        nai_wsockops.GetAcceptExSockaddrs(
            p, (DWORD)size, extralen/2, extralen/2, 
            sockname, socknamelen, 
            peername, peernamelen);
    } else {
        nai_errno = ENOTSUP;
        r = -1;
        goto _end;
    };

    r = 0;

_end:
    return r;
};


nai_fd_t nai_aio_accept(nai_aio_t* a, 
    nai_fd_t l, nai_sockaddr_t* name, nai_int_t* namelen)
{
    nai_errno = ENOTSUP;
    return NAI_FD_INVALID;
};


nai_int_t nai_aio_connect(nai_aio_t* a, 
    nai_fd_t s, const nai_sockaddr_t* name, nai_int_t namelen)
{
    nai_int_t r;
    DWORD ec;
    nai_socknbuf_t nbuf;


    if (!nai_wsockops.ConnectEx) {
        nai_errno = ENOTSUP;
        r = -1;
        goto _end;
    };

    nbuf.len = namelen;
    nai_memset(&nbuf.addr, 0, namelen);
    nbuf.addr.sa_family = name->sa_family;
    r = nai_sock_bind(s, &nbuf.addr, nbuf.len);
    if (r < 0) {
        if (nai_errno != EINVAL) {
            goto _end;
        };
        /* maybe already bind, ignore */
    };
    /* ignore error */

    nai_aio_set_fd(a, s);
    r = nai_aio_set_event(a);
    if (r < 0) {
        goto _end;
    };

    a->postpro = 1;
    if (!nai_wsockops.ConnectEx(
        (SOCKET)s, name, namelen, 0, 0, 0, &a->cb)) {
        ec = GetLastError();
        switch (ec) {
        case ERROR_IO_PENDING:
            nai_errno = EINPROGRESS;
            r = -1;
            goto _end;
        default:
            nai_errno = nai_errno_from_oserr(ec);
            r = -1;
            goto _end;
        };
    };

    r = nai_aio_postpro(a);

_end:
    return r;
};


intptr_t nai_aio_read(nai_aio_t* a, 
    nai_fd_t fd, void* p, size_t size, nai_off64_t offset)
{
    intptr_t r;
    DWORD ec;
    DWORD bytes;


    nai_aio_set_fd(a, fd);
    nai_aio_set_offset(a, offset);
    r = nai_aio_set_event(a);
    if (r < 0) {
        goto _end;
    };

    r = ReadFile(fd, p, (DWORD)size, &bytes, &a->cb);
    if (r != 0) {
        r = nai_aio_return(a, bytes);
    } else {
        ec = GetLastError();
        switch (ec) {
        case ERROR_IO_PENDING:
            nai_errno = EINPROGRESS;
            r = -1;
            break;
        default:
            nai_errno = nai_errno_from_oserr(ec);
            r = -1;
            break;
        };
    };

_end:
    return r;
};


intptr_t nai_aio_readv(nai_aio_t* a, 
    nai_fd_t fd, nai_bufvec_t* v, nai_int_t count, nai_off64_t offset)
{
    (void)a;
    (void)fd;
    (void)v;
    (void)count;
    (void)offset;

    nai_errno = ENOTSUP;
    return -1;
};


intptr_t nai_aio_write(nai_aio_t* a, 
    nai_fd_t fd, const void* p, size_t size, nai_off64_t offset)
{
    intptr_t r;
    DWORD ec;
    DWORD bytes;


    nai_aio_set_fd(a, fd);
    nai_aio_set_offset(a, offset);
    r = nai_aio_set_event(a);
    if (r < 0) {
        goto _end;
    };

    r = WriteFile(fd, p, (DWORD)size, &bytes, &a->cb);
    if (r != 0) {
        r = nai_aio_return(a, bytes);
    } else {
        ec = GetLastError();
        switch (ec) {
        case ERROR_IO_PENDING:
            nai_errno = EINPROGRESS;
            r = -1;
            break;
        default:
            nai_errno = nai_errno_from_oserr(ec);
            r = -1;
            break;
        };
    };

_end:
    return r;
};


intptr_t nai_aio_writev(nai_aio_t* a, 
    nai_fd_t fd, const nai_bufvec_t* v, nai_int_t count, nai_off64_t offset)
{
    (void)a;
    (void)fd;
    (void)v;
    (void)count;
    (void)offset;

    nai_errno = ENOTSUP;
    return -1;
};


intptr_t nai_aio_recv(nai_aio_t* a, 
    nai_fd_t s, void* p, size_t size, nai_int_t flags)
{
    intptr_t r;
    nai_bufvec_t v;


    nai_aio_set_fd(a, s);
    r = nai_aio_set_event(a);
    if (r < 0) {
        goto _end;
    };

    v.buf = (uint8_t*)p;
    v.len = size;
    r = nai_aio_recvv(a, s, &v, 1, flags);

_end:
    return r;
};


intptr_t nai_aio_recvv(nai_aio_t* a, 
    nai_fd_t s, nai_bufvec_t* v, nai_int_t count, nai_int_t flags)
{
    intptr_t r;
    DWORD ec;
    DWORD bytes;
    DWORD opt = flags;


    nai_aio_set_fd(a, s);
    r = nai_aio_set_event(a);
    if (r < 0) {
        goto _end;
    };

    r = WSARecv((SOCKET)s, 
        (WSABUF*)v, count, &bytes, &opt, &a->cb, 0);
    if (r != SOCKET_ERROR) {
        r = nai_aio_return(a, bytes);
    } else {
        ec = GetLastError();
        switch (ec) {
        case ERROR_IO_PENDING:
            nai_errno = EINPROGRESS;
            break;
        default:
            nai_errno = nai_errno_from_oserr(ec);
            break;
        };
    };

_end:
    return r;
};


intptr_t nai_aio_recvfrom(nai_aio_t* a, nai_fd_t s, 
    void* p, size_t size, nai_int_t flags, 
    nai_sockaddr_t* name, nai_int_t* namelen)
{
    intptr_t r;
    WSABUF v;
    DWORD ec;
    DWORD bytes;
    DWORD opt = flags;


    nai_aio_set_fd(a, s);
    r = nai_aio_set_event(a);
    if (r < 0) {
        goto _end;
    };

    v.buf = (char*)p;
    v.len = (uint32_t)size;
    r = WSARecvFrom((SOCKET)s, 
        &v, 1, &bytes, &opt, name, namelen, &a->cb, 0);
    if (r != SOCKET_ERROR) {
        r = nai_aio_return(a, bytes);
    } else {
        ec = GetLastError();
        switch (ec) {
        case ERROR_IO_PENDING:
            nai_errno = EINPROGRESS;
            break;
        default:
            nai_errno = nai_errno_from_oserr(ec);
            break;
        };
    };

_end:
    return r;
}


intptr_t nai_aio_recvm(nai_aio_t* a, nai_fd_t s, 
    nai_bufvec_t* v, nai_int_t count, nai_int_t flags, 
    nai_sockaddr_t* name, nai_int_t* namelen, 
    void* ctrl, nai_int_t* ctrllen)
{
    intptr_t r;
    DWORD ec;
    DWORD bytes;
    DWORD opt = flags;


    (void)ctrllen;

    if (ctrl) {
        nai_errno = ENOTSUP;
        r = -1;
        goto _end;
    };

    nai_aio_set_fd(a, s);
    r = nai_aio_set_event(a);
    if (r < 0) {
        goto _end;
    };

    r = WSARecvFrom((SOCKET)s, 
        (WSABUF*)v, count, &bytes, &opt, name, namelen, &a->cb, 0);
    if (r != SOCKET_ERROR) {
        r = nai_aio_return(a, bytes);
    } else {
        ec = GetLastError();
        switch (ec) {
        case ERROR_IO_PENDING:
            nai_errno = EINPROGRESS;
            break;
        default:
            nai_errno = nai_errno_from_oserr(ec);
            break;
        };
    };

_end:
    return r;
};


intptr_t nai_aio_send(nai_aio_t* a, 
    nai_fd_t s, const void* p, size_t size, nai_int_t flags)
{
    intptr_t r;
    nai_bufvec_t v;


    nai_aio_set_fd(a, s);
    r = nai_aio_set_event(a);
    if (r < 0) {
        goto _end;
    };

    v.buf = (uint8_t*)p;
    v.len = size;
    r = nai_aio_sendv(a, s, &v, 1, flags);

_end:
    return r;
};


intptr_t nai_aio_sendv(nai_aio_t* a, 
    nai_fd_t s, const nai_bufvec_t* v, nai_int_t count, nai_int_t flags)
{
    intptr_t r;
    DWORD ec;
    DWORD bytes;


    nai_aio_set_fd(a, s);
    r = nai_aio_set_event(a);
    if (r < 0) {
        goto _end;
    };

    r = WSASend((SOCKET)s, (WSABUF*)v, count, &bytes, flags, &a->cb, 0);
    if (r != SOCKET_ERROR) {
        r = nai_aio_return(a, bytes);
    } else {
        ec = GetLastError();
        switch (ec) {
        case ERROR_IO_PENDING:
            nai_errno = EINPROGRESS;
            break;
        default:
            nai_errno = nai_errno_from_oserr(ec);
            break;
        };
    };

_end:
    return r;
};


intptr_t nai_aio_sendto(nai_aio_t* a, nai_fd_t s, 
    const void* p, size_t size, nai_int_t flags, 
    const nai_sockaddr_t* name, nai_int_t namelen)
{
    intptr_t r;
    WSABUF v;
    DWORD ec;
    DWORD bytes;


    nai_aio_set_fd(a, s);
    r = nai_aio_set_event(a);
    if (r < 0) {
        goto _end;
    };

    v.buf = (char*)p;
    v.len = (uint32_t)size;
    r = WSASendTo((SOCKET)s, 
        &v, 1, &bytes, flags, name, namelen, &a->cb, 0);
    if (r != SOCKET_ERROR) {
        r = nai_aio_return(a, bytes);
    } else {
        ec = GetLastError();
        switch (ec) {
        case ERROR_IO_PENDING:
            nai_errno = EINPROGRESS;
            break;
        default:
            nai_errno = nai_errno_from_oserr(ec);
            break;
        };
    };

_end:
    return r;
};


intptr_t nai_aio_sendm(nai_aio_t* a, nai_fd_t s, 
    const nai_bufvec_t* v, nai_int_t count, nai_int_t flags, 
    const nai_sockaddr_t* name, nai_int_t namelen, 
    const void* ctrl, nai_int_t ctrllen)
{
    intptr_t r;
    DWORD ec;
    DWORD bytes;


    (void)ctrllen;

    if (ctrl) {
        nai_errno = ENOTSUP;
        r = -1;
        goto _end;
    };

    nai_aio_set_fd(a, s);
    r = nai_aio_set_event(a);
    if (r < 0) {
        goto _end;
    };

    r = WSASendTo((SOCKET)s, 
        (WSABUF*)v, count, &bytes, flags, name, namelen, &a->cb, 0);
    if (r != SOCKET_ERROR) {
        r = nai_aio_return(a, bytes);
    } else {
        ec = GetLastError();
        switch (ec) {
        case ERROR_IO_PENDING:
            nai_errno = EINPROGRESS;
            break;
        default:
            nai_errno = nai_errno_from_oserr(ec);
            break;
        };
    };

_end:
    return r;
};


intptr_t nai_aio_sendmm(nai_aio_t* a, nai_fd_t s, 
    const nai_bufarray_t* v, nai_int_t count, nai_int_t flags, 
    const nai_sockaddr_t* name, nai_int_t namelen)
{
    intptr_t r;

    if (count <= 0) {
        r = nai_aio_sendm(a, 
            s, 0, 0, flags, name, namelen, 0, 0);
    } else {
        r = nai_aio_sendm(a, 
            s, v[0].v, v[0].count, flags, name, namelen, 0, 0);
    };

    return r;
};


intptr_t nai_aio_sendfile(nai_aio_t* a, 
    nai_fd_t s, const nai_filechunk_t* c, size_t* submit)
{
    intptr_t r;
    nai_int_t ecnt;
    size_t esize;
    size_t bsize;
    nai_filechunk_t d;
    TRANSMIT_FILE_BUFFERS b;
    TRANSMIT_PACKETS_ELEMENT e[NAI_BUFV_MAX];
    DWORD ec;


    if (nai_wsockops.TransmitPackets) {

        d = *c;
        ecnt = nai_filechunk_to_packets(&d, e, nai_countof(e), &esize);
        if (ecnt <= 0) {
            ;
        };

        if (submit) {
            submit[0] = esize;
        };

        nai_aio_set_fd(a, s);
        r = nai_aio_set_event(a);
        if (r < 0) {
            goto _end;
        };

        r = nai_wsockops.TransmitPackets(
            (SOCKET)s, e, ecnt, 0, &a->cb, NAI_SENDFILE_FLAGS);
        if (r != 0) {
            r = nai_aio_return(a, nai_aio_result(a, 0));
        } else {
            ec = GetLastError();
            switch (ec) {
            case ERROR_IO_PENDING:
                nai_errno = EINPROGRESS;
                r = -1;
                break;
            default:
                nai_errno = nai_errno_from_oserr(ec);
                r = -1;
                break;
            };
        };

    } else if (nai_wsockops.TransmitFile) {

        if (c->hcnt > 1 || c->fd == NAI_FD_INVALID) {
            if (submit) {
                submit[0] = c->hsize;
            };
            r = nai_aio_sendv(a, s, c->hvec, c->hcnt, 0);
            /* only submit once */
            goto _end;
        };

        if (c->fd != NAI_FD_INVALID) {

            bsize = c->size;
            if (c->hcnt == 1) {
                b.Head = c->hvec[0].buf;
                b.HeadLength = (uint32_t)c->hvec[0].len;
                bsize += b.HeadLength;
            };
            if (c->tcnt > 0) {
                b.Tail = c->tvec[0].buf;
                b.TailLength = (uint32_t)c->tvec[0].len;
                bsize += b.TailLength;
            };

            if (submit) {
                submit[0] = bsize;
            };

            nai_aio_set_fd(a, s);
            nai_aio_set_offset(a, c->off);
            r = nai_aio_set_event(a);
            if (r < 0) {
                goto _end;
            };

            r = nai_wsockops.TransmitFile(
                (SOCKET)s, c->fd, (uint32_t)c->size, 0, &a->cb, &b, 
                NAI_SENDFILE_FLAGS);
            if (r != 0) {
                r = nai_aio_return(a, nai_aio_result(a, 0));
            } else {
                ec = GetLastError();
                switch (ec) {
                case ERROR_IO_PENDING:
                    nai_errno = EINPROGRESS;
                    r = -1;
                    break;
                default:
                    nai_errno = nai_errno_from_oserr(ec);
                    r = -1;
                    break;
                };
            };
            /* only submit once */
            goto _end;
        };

        if (c->tcnt > 0) {
            if (submit) {
                submit[0] = c->tsize;
            };
            r = nai_aio_sendv(a, s, c->tvec, c->tcnt, 0);
            /* only submit once */
            goto _end;
        };

    } else {

        nai_errno = ENOTSUP;
        r = -1;
    };

_end:
    return r;
};


intptr_t nai_aio_sendfilev(nai_aio_t* a, 
    nai_fd_t s, const nai_filevec_t* v, nai_int_t count, size_t* submit)
{
    intptr_t r;
    nai_int_t ec;
    nai_int_t ecnt;
    size_t esize;
    size_t bsize;
    nai_filearray_t fa;
    nai_filechunk_t fc;
    nai_bufvec_t b[NAI_BUFV_MAX];
    TRANSMIT_PACKETS_ELEMENT e[NAI_BUFV_MAX];


    if (nai_wsockops.TransmitPackets) {

        fa.v = (nai_filevec_t*)v;
        fa.count = count;
        ecnt = nai_filearray_to_packets(&fa, e, nai_countof(e), &esize);
        if (ecnt <= 0) {
            ;
        };

        if (submit) {
            submit[0] = esize;
        };

        nai_aio_set_fd(a, s);
        r = nai_aio_set_event(a);
        if (r < 0) {
            goto _end;
        };

        r = nai_wsockops.TransmitPackets(
            (SOCKET)s, e, ecnt, 0, &a->cb, NAI_SENDFILE_FLAGS);
        if (r != 0) {
            r = nai_aio_return(a, nai_aio_result(a, 0));
        } else {
            ec = GetLastError();
            switch (ec) {
            case ERROR_IO_PENDING:
                nai_errno = EINPROGRESS;
                r = -1;
                break;
            default:
                nai_errno = nai_errno_from_oserr(ec);
                r = -1;
                break;
            };
        };

    } else if (nai_wsockops.TransmitFile) {

        fa.v = (nai_filevec_t*)v;
        fa.count = count;
        bsize = nai_filearray_to_filechunk(&fa, &fc, b, nai_countof(b), 1);
        if (bsize <= 0) {
            ;
        };

        r = nai_aio_sendfile(a, s, &fc, submit);
        goto _end;

    } else {

        nai_errno = ENOTSUP;
        r = -1;
        goto _end;
    };

_end:
    return r;
};



#else


#if (NAI_AIO_USE_URING)

#define NAI_AIO_SUPPORT         1
#define NAI_AIO_READ            1
#define NAI_AIO_WRITE           1
#define NAI_AIO_READV           1
#define NAI_AIO_WRITEV          1
#define NAI_AIO_ACCEPT          1
#define NAI_AIO_CONNECT         1
#define NAI_AIO_SOCKET          1
#define NAI_AIO_SENDFILE        1

#elif (NAI_AIO_USE_LINUX)

#define NAI_AIO_SUPPORT         1
#define NAI_AIO_READ            1
#define NAI_AIO_READV           1
#define NAI_AIO_WRITE           1
#define NAI_AIO_WRITEV          1

#elif (NAI_AIO_USE_POSIX)

#define NAI_AIO_SUPPORT         1
#define NAI_AIO_READ            1
#define NAI_AIO_WRITE           1

#if (NAI_HAVE_AIO_READV)
#define NAI_AIO_READV           1
#endif
#if (NAI_HAVE_AIO_WRITEV)
#define NAI_AIO_WRITEV          1
#endif

#endif



#if (NAI_AIO_NOWAIT)


typedef nai_int_t (*nai_bio_op_f)(
    nai_aio_t* a, nai_int_t first, uint32_t msec);


#if (NAI_AIO_READ)

static nai_int_t nai_aio_bread(nai_aio_t* a, nai_int_t first, uint32_t msec)
{
    nai_int_t r;
    nai_int_t ec;
    intptr_t n;

    nai_fd_t fd = a->cb.aio_fildes;
    nai_off64_t offset = a->cb.aio_offset;
    size_t size = a->cb.aio_nbytes;
    void* p = (void*)a->cb.aio_buf;


    for (;;) {
        if (first == 0) {
            r = nai_file_poll(fd, NAI_POLL_READ, msec);
            if (r < 0) {
                break;
            };
        };

        if (offset == -1) {
            n = nai_file_read(fd, p, size);
        } else {
            n = nai_file_pread(fd, p, size, offset);
        };
        if (n >= 0) {
            a->cb.aio_nbytes = n;
            a->code = 0;
            r = 0;
            break;
        };

        ec = nai_errno;
        if (ec != NAI_EAGAIN) {
            a->cb.aio_nbytes = -1;
            a->code = ec;
            r = -1;
            break;
        };

        if (first) {
            nai_errno = NAI_EINPROGRESS;
            a->cb.aio_data = (uintptr_t)nai_aio_bread;
            a->waiting = 1;
            a->code = -1;
            r = -1;
            break;
        };
    };

    return r;
};

#endif

#if (NAI_AIO_WRITE)

static nai_int_t nai_aio_bwrite(nai_aio_t* a, nai_int_t first, uint32_t msec)
{
    nai_int_t r;
    nai_int_t ec;
    intptr_t n;

    nai_fd_t fd = a->cb.aio_fildes;
    nai_off64_t offset = a->cb.aio_offset;
    size_t size = a->cb.aio_nbytes;
    const void* p = (void*)a->cb.aio_buf;


    for (;;) {
        if (first == 0) {
            r = nai_file_poll(fd, NAI_POLL_WRITE, msec);
            if (r < 0) {
                break;
            };
        };

        if (offset == -1) {
            n = nai_file_write(fd, p, size);
        } else {
            n = nai_file_pwrite(fd, p, size, offset);
        };
        if (n >= 0) {
            a->cb.aio_nbytes = n;
            a->code = 0;
            r = 0;
            break;
        };

        ec = nai_errno;
        if (ec != NAI_EAGAIN) {
            a->cb.aio_nbytes = -1;
            a->code = ec;
            r = -1;
            break;
        };

        if (first) {
            nai_errno = NAI_EINPROGRESS;
            a->cb.aio_data = (uintptr_t)nai_aio_bwrite;
            a->waiting = 1;
            a->code = -1;
            r = -1;
            break;
        };
    };

    return r;
};

#endif


#endif



#if (NAI_AIO_USE_URING)


#include "unix/nai_linux.h"
#include "nai/os/nai_thread.h"


nai_aio_feature_t nai_aiofeat = {
    .support = 1, 
    .accept = 1, 
    .acceptex = 0, 
    .connect = 1, 
    .readv = 1, 
    .readv_sim = 1, 
    .writev = 1, 
    .writev_sim = 1, 
    .recv = 1, 
    .recvv = 1, 
    .recvv_sim = 1, 
    .recvfrom = 1, 
    .recvmsg = 1, 
    .recvmsg_sim = 1, 
    .recvmmsg = 0, 
    .recvmmsg_sim = 1, 
    .send = 1, 
    .sendv = 1, 
    .sendv_sim = 1, 
    .sendto = 1, 
    .sendmsg = 1, 
    .sendmsg_sim = 1, 
    .sendmmsg = 0, 
    .sendmmsg_sim = 1, 
    .sendfile = 0, 
    .sendfile_sim = 0, 
    .sendfilev = 0, 
    .sendfilev_sim = 0
};


static nai_uring_t* nai_aio_prepare(
    nai_aio_t* a, struct io_uring_sqe** p, 
    const void* data, size_t datalen, nai_int_t postpro)
{
    nai_int_t batch;
    void* copy;
    nai_uring_t* r;
    nai_aio_port_t* port;
    struct io_uring_sqe* q;


    if (a->waitable == 0 && a->port) {
        port = (nai_aio_port_t*)a->port;
        r = (nai_uring_t*)port->ctx;
        a->waiting = 0;
        if (!a->internal && r->mt) {
            r = r->mt;      /* not internal use mt-uring */
        };
    } else {
        r = nai_uring_global_get();
        if (r == 0) {
            goto _end;
        };

        a->waiting = 1;
    };

    batch = nai_uring_is_batch(r);

    /* need lock */
    nai_uring_lock(r);

    if (!batch && !postpro) {
        a->data = (void*)data; /* no copy direct use */
        a->postpro = 0;
    } else if (datalen <= 0) {
        a->data = 0;
        a->postpro = 0;
    } else {
        /* in batch mode, 
         * we will delay the commit, so we need to copy the parameters
         */
        copy = nai_uring_alloc(r, datalen);
        if (copy == 0) {
            r = 0;
            goto _end;
        };

        if (data) {
            nai_memcpy(copy, data, datalen);
        };

        a->data = copy;
        a->datalen = datalen;
        a->postpro = !!postpro;
    };

    q = io_uring_get_sqe(&r->queue);
    if (q == 0) {
        if (batch && datalen > 0) {
            nai_uring_free(r, a->data, a->datalen);
            a->data = 0;
        };

        nai_uring_unlock(r);

        nai_log_alert(NAI_LOG_CORE, 
            EBUSY, "io_uring_get_sqe failed");

        nai_errno = EBUSY;
        a->waiting = 0;
        r = 0;
        goto _end;
    };

    p[0] = q;
    a->code = -1;

_end:
    return r;
};


static nai_int_t nai_aio_submit(nai_aio_t* a, nai_uring_t* u, nai_int_t force)
{
    nai_int_t r;


    (void)a;

    if (nai_uring_is_batch(u) && !force) {
        u->count ++;
        if (u->count < u->batch) {
            nai_uring_unlock(u);
            goto _end;
        };
    };

    r = io_uring_submit(&u->queue);
    u->count = 0;
    nai_uring_unlock(u);

    if (r < 0) {
        nai_log_alert(NAI_LOG_CORE, 
            -r, "io_uring_submit failed");
    };

_end:
    nai_errno = EINPROGRESS;
    r = -1;

    return r;
};


nai_int_t nai_aio_init(nai_aio_t* a)
{
    a->port = 0;
    a->cond = 0;
    a->data = 0;
    a->datalen = 0;
    a->sult = 0;
    a->code = 0;
    a->internal = 0;
    a->waitable = 0;
    a->waiting = 0;
    a->handle = 0;
    return 0;
};


nai_int_t nai_aio_set_port(nai_aio_t* a, nai_aio_port_t* p)
{
    a->port = p;
    return 0;
};


nai_int_t nai_aio_set_in_loop(nai_aio_t* a, nai_int_t in_loop)
{
    a->internal = !!in_loop;
    return 0;
};


nai_int_t nai_aio_set_waitable(nai_aio_t* a, nai_int_t waitable)
{
    a->waitable = !!waitable;
    return 0;
};


nai_int_t nai_aio_wait(nai_aio_t* a, uint32_t msec)
{
    nai_int_t r;


    if (a->code != -1) {
        r = 0;
        goto _end;
    };

    if (a->waiting == 0) {
        nai_errno = ENOTSUP;
        r = -1;
        goto _end;
    };

    r = nai_uring_global_wait(a, msec);

_end:
    return r;
};


nai_int_t nai_aio_cancel(nai_aio_t* a)
{
    nai_int_t r;
    nai_uring_t* u;
    nai_aio_port_t* port;
    struct io_uring_sqe* sqe;


    if (a->code != -1) {
        r = 0;
        goto _end;
    };

    if (a->waiting == 0 && a->port) {
        port = (nai_aio_port_t*)a->port;
        u = (nai_uring_t*)port->ctx;
        if (!a->internal && u->mt) {
            u = u->mt;      /* not internal use mt-uring */
        };
    } else {
        u = nai_uring_global_get();
        if (u == 0) {
            r = -1;
            goto _end;
        };
    };

    /* lock */
    nai_uring_lock(u);

    sqe = io_uring_get_sqe(&u->queue);
    if (sqe == 0) {
        nai_uring_unlock(u);
        nai_errno = EBUSY;
        r = -1;
        goto _end;
    };

    io_uring_prep_cancel(sqe, a, 0);
    io_uring_sqe_set_data(sqe, (void*)0);

    /* unlock in submit */
    r = nai_aio_submit(a, u, 1);

_end:
    return r;
};


intptr_t nai_aio_result(nai_aio_t* a, nai_int_t wait)
{
    intptr_t r;


    if (a->code != -1) {
        (void)0;
    } else if (a->waiting == 0) {
        nai_errno = EINPROGRESS;
        r = -1;
        goto _end;
    } else {
        r = nai_aio_wait(a, wait ? -1 : 0);
        if (r < 0) {
            goto _end;
        };
    };

    r = a->sult;
    if (r < 0) {
        nai_errno = a->code;
    };

_end:
    return r;
};


intptr_t nai_aio_read(nai_aio_t* a, 
    nai_fd_t fd, void* p, size_t size, nai_off64_t offset)
{
    nai_int_t r;
    nai_uring_t* u;
    struct io_uring_sqe* sqe;


    u = nai_aio_prepare(a, &sqe, 0, 0, 0);
    if (u == 0) {
        r = -1;
        goto _end;
    };

    io_uring_prep_read(sqe, fd, p, size, offset);
    io_uring_sqe_set_data(sqe, a);

    r = nai_aio_submit(a, u, 0);

_end:
    return r;
};


intptr_t nai_aio_write(nai_aio_t* a, 
    nai_fd_t fd, const void* p, size_t size, nai_off64_t offset)
{
    nai_int_t r;
    nai_uring_t* u;
    struct io_uring_sqe* sqe;


    u = nai_aio_prepare(a, &sqe, 0, 0, 0);
    if (u == 0) {
        r = -1;
        goto _end;
    };

    io_uring_prep_write(sqe, fd, p, size, offset);
    io_uring_sqe_set_data(sqe, a);

    r = nai_aio_submit(a, u, 0);

_end:
    return r;
};


intptr_t nai_aio_readv(nai_aio_t* a, 
    nai_fd_t fd, nai_bufvec_t* v, nai_int_t count, nai_off64_t offset)
{
    nai_int_t r;
    nai_uring_t* u;
    struct io_uring_sqe* sqe;


    u = nai_aio_prepare(a, &sqe, v, sizeof(*v) * count, 0);
    if (u == 0) {
        r = -1;
        goto _end;
    };

    io_uring_prep_readv(sqe, fd, a->data, count, offset);
    io_uring_sqe_set_data(sqe, a);

    r = nai_aio_submit(a, u, 0);

_end:
    return r;
};


intptr_t nai_aio_writev(nai_aio_t* a, 
    nai_fd_t fd, const nai_bufvec_t* v, nai_int_t count, nai_off64_t offset)
{
    nai_int_t r;
    nai_uring_t* u;
    struct io_uring_sqe* sqe;


    u = nai_aio_prepare(a, &sqe, v, sizeof(*v) * count, 0);
    if (u == 0) {
        r = -1;
        goto _end;
    };

    io_uring_prep_writev(sqe, fd, a->data, count, offset);
    io_uring_sqe_set_data(sqe, a);

    r = nai_aio_submit(a, u, 0);

_end:
    return r;
};


nai_int_t nai_aio_accept(nai_aio_t* a, 
    nai_fd_t l, nai_sockaddr_t* name, nai_int_t* namelen)
{
    nai_int_t r;
    nai_uring_t* u;
    struct io_uring_sqe* sqe;


    u = nai_aio_prepare(a, &sqe, 0, 0, 0);
    if (u == 0) {
        r = -1;
        goto _end;
    };

    io_uring_prep_accept(sqe, l, name, (socklen_t*)namelen, SOCK_CLOEXEC);
    io_uring_sqe_set_data(sqe, a);

    r = nai_aio_submit(a, u, 0);

_end:
    return r;
};


nai_int_t nai_aio_connect(nai_aio_t* a, 
    nai_fd_t s, const nai_sockaddr_t* name, nai_int_t namelen)
{
    nai_int_t r;
    nai_uring_t* u;
    struct io_uring_sqe* sqe;


    u = nai_aio_prepare(a, &sqe, name, namelen, 0);
    if (u == 0) {
        r = -1;
        goto _end;
    };

    io_uring_prep_connect(sqe, s, a->data, namelen);
    io_uring_sqe_set_data(sqe, a);

    r = nai_aio_submit(a, u, 0);

_end:
    return r;
};


nai_int_t nai_aio_connect_end(nai_fd_t s)
{
    (void)s;
    return 0;
};


intptr_t nai_aio_recv(nai_aio_t* a, 
    nai_fd_t s, void* p, size_t size, nai_int_t flags)
{
    nai_int_t r;
    nai_uring_t* u;
    struct io_uring_sqe* sqe;


    u = nai_aio_prepare(a, &sqe, 0, 0, 0);
    if (u == 0) {
        r = -1;
        goto _end;
    };

    io_uring_prep_recv(sqe, s, p, size, flags);
    io_uring_sqe_set_data(sqe, a);

    r = nai_aio_submit(a, u, 0);

_end:
    return r;
};


intptr_t nai_aio_recvv(nai_aio_t* a, 
    nai_fd_t s, nai_bufvec_t* v, nai_int_t count, nai_int_t flags)
{
    return nai_aio_recvm(a, s, v, count, flags, 0, 0, 0, 0);
};


intptr_t nai_aio_recvfrom(nai_aio_t* a, nai_fd_t s, 
    void* p, size_t size, nai_int_t flags, 
    nai_sockaddr_t* name, nai_int_t* namelen)
{
    nai_int_t r;
    nai_bufvec_t v;


    v.buf = p;
    v.len = size;
    r = nai_aio_recvm(a, s, &v, 1, flags, name, namelen, 0, 0);

    return r;
};


typedef struct nai_uring_postpro_recvmsg_s {
    nai_uring_postpro_t hdr;
    nai_int_t* namelen;
    nai_int_t* ctrllen;
} nai_uring_postpro_recvmsg_t;


static nai_int_t nai_uring_postpro_recvmsg(nai_uring_postpro_t* p)
{
    nai_int_t r;
    struct msghdr* msg;
    nai_uring_postpro_recvmsg_t* post;


    post = (nai_uring_postpro_recvmsg_t*)p;
    msg = (struct msghdr*)(post + 1);
    if (post->namelen) {
        post->namelen[0] = msg->msg_namelen;
    };
    if (post->ctrllen) {
#if defined(CMSG_FIRSTHDR)
        post->ctrllen[0] = msg->msg_controllen;
#else
        post->ctrllen[0] = msg->msg_accrightslen;
#endif
    };

    r = 0;

    return r;
};


intptr_t nai_aio_recvm(nai_aio_t* a, nai_fd_t s, 
    nai_bufvec_t* v, nai_int_t count, nai_int_t flags, 
    nai_sockaddr_t* name, nai_int_t* namelen, 
    void* ctrl, nai_int_t* ctrllen)
{
    nai_int_t r;
    nai_uring_t* u;
    nai_uring_postpro_recvmsg_t* post;
    struct msghdr* msg;
    struct io_uring_sqe* sqe;


    u = nai_aio_prepare(a, &sqe, 0, 
        sizeof(*post) + sizeof(*msg) + sizeof(*v) * count, 1);
    if (u == 0) {
        r = -1;
        goto _end;
    };

    post = (nai_uring_postpro_recvmsg_t*)a->data;
    post->hdr.handle = nai_uring_postpro_recvmsg;
    post->namelen = namelen;
    post->ctrllen = ctrllen;

    msg = (struct msghdr*)(post + 1);
    msg->msg_iov = (struct iovec*)(msg + 1);
    nai_memcpy(msg + 1, v, sizeof(*v) * count);

    msg->msg_iovlen = count;
    msg->msg_name = (void*)name;
    msg->msg_namelen = namelen ? *namelen : 0;
#if defined(CMSG_FIRSTHDR)
    msg->msg_control = ctrl;
    msg->msg_controllen = ctrllen ? *ctrllen : 0;
    msg->msg_flags = 0;
#else
    msg->msg_accrights = (caddr_t)ctrl;
    msg->msg_accrightslen = ctrllen ? *ctrllen : 0;
#endif


    io_uring_prep_recvmsg(sqe, s, msg, flags);
    io_uring_sqe_set_data(sqe, a);

    r = nai_aio_submit(a, u, 0);

_end:
    return r;
};


intptr_t nai_aio_send(nai_aio_t* a, 
    nai_fd_t s, const void* p, size_t size, nai_int_t flags)
{
    nai_int_t r;
    nai_uring_t* u;
    struct io_uring_sqe* sqe;


    u = nai_aio_prepare(a, &sqe, 0, 0, 0);
    if (u == 0) {
        r = -1;
        goto _end;
    };

    io_uring_prep_send(sqe, s, p, size, flags);
    io_uring_sqe_set_data(sqe, a);

    r = nai_aio_submit(a, u, 0);

_end:
    return r;
};


intptr_t nai_aio_sendv(nai_aio_t* a, 
    nai_fd_t s, const nai_bufvec_t* v, nai_int_t count, nai_int_t flags)
{
    return nai_aio_sendm(a, s, v, count, flags, 0, 0, 0, 0);
};


intptr_t nai_aio_sendto(nai_aio_t* a, nai_fd_t s, 
    const void* p, size_t size, nai_int_t flags, 
    const nai_sockaddr_t* name, nai_int_t namelen)
{
    nai_int_t r;
    nai_bufvec_t v;


    v.buf = (void*)p;
    v.len = size;
    r = nai_aio_sendm(a, s, &v, 1, flags, name, namelen, 0, 0);

    return r;
};


intptr_t nai_aio_sendm(nai_aio_t* a, nai_fd_t s, 
    const nai_bufvec_t* v, nai_int_t count, nai_int_t flags, 
    const nai_sockaddr_t* name, nai_int_t namelen, 
    const void* ctrl, nai_int_t ctrllen)
{
    nai_int_t r;
    uint8_t* buf;
    nai_uring_t* u;
    struct msghdr mhdr;
    struct msghdr* msg;
    struct io_uring_sqe* sqe;


    u = nai_aio_prepare(a, &sqe, 0, 
        sizeof(*msg) + sizeof(*v) * count + namelen + ctrllen, 0);
    if (u == 0) {
        r = -1;
        goto _end;
    };

    if (a->data == 0) {
        msg = &mhdr;
        msg->msg_iov = (struct iovec*)v;
        msg->msg_name = (void*)name;
#if defined(CMSG_FIRSTHDR)
        msg->msg_control = (void*)ctrl;
#else
        msg->msg_accrights = (caddr_t)ctrl;
#endif
    } else {
        msg = (struct msghdr*)a->data;
        msg->msg_iov = (struct iovec*)(msg + 1);
        nai_memcpy(msg->msg_iov, v, sizeof(*v) * count);
        buf = (uint8_t*)(msg->msg_iov + count);

        msg->msg_name = (void*)(namelen ? buf : 0);
        nai_memcpy(msg->msg_name, name, namelen);

#if defined(CMSG_FIRSTHDR)
        msg->msg_control = (void*)(ctrllen ? buf + namelen : 0);
        nai_memcpy(msg->msg_control, ctrl, ctrllen);
#else
        msg->msg_accrights = (caddr_t)(ctrllen ? buf + namelen : 0);
        nai_memcpy(msg->msg_accrights, ctrl, ctrllen);
#endif
    };

    msg->msg_iovlen = count;
    msg->msg_namelen = namelen;
#if defined(CMSG_FIRSTHDR)
    msg->msg_controllen = ctrllen;
    msg->msg_flags = 0;
#else
    msg->msg_accrightslen = ctrllen;
#endif


    io_uring_prep_sendmsg(sqe, s, msg, flags);
    io_uring_sqe_set_data(sqe, a);

    r = nai_aio_submit(a, u, 0);

_end:
    return r;
};


intptr_t nai_aio_sendmm(nai_aio_t* a, nai_fd_t s, 
    const nai_bufarray_t* v, nai_int_t count, nai_int_t flags, 
    const nai_sockaddr_t* name, nai_int_t namelen)
{
    intptr_t r;

    if (count <= 0) {
        r = 0;
    } else {
        r = nai_aio_sendm(a, 
            s, v[0].v, v[0].count, flags, name, namelen, 0, 0);
    };

    return r;
};


intptr_t nai_aio_sendfile(nai_aio_t* a, 
    nai_fd_t s, const nai_filechunk_t* c, size_t* submit)
{
#if 1
    (void)a;
    (void)s;
    (void)c;
    (void)submit;

    nai_errno = ENOTSUP;
    return -1;
#else
    intptr_t r;
    nai_uring_t* u;
    struct io_uring_sqe* sqe;


    if (c->hcnt > 0) {
        if (submit) {
            submit[0] = c->hsize;
        };
        r = nai_aio_sendv(a, s, c->hvec, c->hcnt, 0);
        /* only submit once */
        goto _end;
    };

    if (c->fd != NAI_FD_INVALID) {

        u = nai_aio_prepare(a, &sqe, 0, 0, 0);
        if (u == 0) {
            r = -1;
            goto _end;
        };

        io_uring_prep_splice(sqe, c->fd, c->off, s, -1, c->size, 0);
        io_uring_sqe_set_data(sqe, a);

        r = nai_aio_submit(a, u);

        /* only submit once */
        goto _end;
    };

    if (c->tcnt > 0) {
        if (submit) {
            submit[0] = c->tsize;
        };
        r = nai_aio_sendv(a, s, c->tvec, c->tcnt, 0);
        /* only submit once */
        goto _end;
    };

    r = 0;

_end:
    return r;
#endif
};


intptr_t nai_aio_sendfilev(nai_aio_t* a, 
    nai_fd_t s, const nai_filevec_t* v, nai_int_t count, size_t* submit)
{
    intptr_t r;
    size_t bsize;
    nai_filearray_t fa;
    nai_filechunk_t fc;
    nai_bufvec_t b[NAI_BUFV_MAX];


    fa.v = (nai_filevec_t*)v;
    fa.count = count;
    bsize = nai_filearray_to_filechunk(&fa, &fc, b, nai_countof(b), 1);
    if (bsize <= 0) {
        ;
    };

    r = nai_aio_sendfile(a, s, &fc, submit);

    return r;
};



#elif (NAI_AIO_USE_LINUX)


#include "unix/nai_linux.h"


nai_aio_feature_t nai_aiofeat = {
    .support = 1, 
    .accept = 0, 
    .acceptex = 0, 
    .connect = 0, 
    .readv = 1, 
    .readv_sim = 1, 
    .writev = 1, 
    .writev_sim = 1, 
    .recv = 0, 
    .recvv = 0, 
    .recvv_sim = 0, 
    .recvfrom = 0, 
    .recvmsg = 0, 
    .recvmsg_sim = 0, 
    .recvmmsg = 0, 
    .recvmmsg_sim = 0, 
    .send = 0, 
    .sendv = 0, 
    .sendv_sim = 0, 
    .sendto = 0, 
    .sendmsg = 0, 
    .sendmsg_sim = 0, 
    .sendmmsg = 0, 
    .sendmmsg_sim = 0, 
    .sendfile = 0, 
    .sendfile_sim = 0, 
    .sendfilev = 0, 
    .sendfilev_sim = 0
};


static aio_context_t nai_aio_prepare(nai_aio_t* a, nai_int_t cmd)
{
    aio_context_t r;
    nai_aio_port_t* port;


    if (a->waitable == 0 && a->port) {
        port = (nai_aio_port_t*)a->port;
        r = port->ctx;
        a->waiting = 0;
        a->cb.aio_flags = IOCB_FLAG_RESFD;
        a->cb.aio_resfd = port->fd;
    } else {
        r = nai_aio_global_get();
        if (r == 0) {
            goto _end;
        };

        a->waiting = 1;
        a->cb.aio_flags = 0;
        a->cb.aio_resfd = -1;
    };

    a->code = -1;
    a->cb.aio_data = (uintptr_t)a;
    a->cb.aio_lio_opcode = cmd;

_end:
    return r;
};


static nai_int_t nai_aio_submit(nai_aio_t* a, aio_context_t ctx)
{
    nai_int_t r;
    nai_int_t ec;
    struct iocb* pcb[1];


    pcb[0] = &a->cb;
    r = io_submit(ctx, 1, pcb);
    if (r == 1) {
        nai_errno = NAI_EINPROGRESS;
        a->code = -1;
    } else {
        ec = nai_errno;
        if (ec == EAGAIN) {
            ec = EBUSY;
        };
        a->cb.aio_nbytes = -1;
        a->code = ec;
    };

    r = -1;

    return r;
};


nai_int_t nai_aio_init(nai_aio_t* a)
{
    nai_memset(&a->cb, 0, sizeof(a->cb));
    a->port = 0;
    a->cond = 0;
    a->code = 0;
    a->waitable = 0;
    a->waiting = 0;
    a->handle = 0;
    return 0;
};


nai_int_t nai_aio_set_port(nai_aio_t* a, nai_aio_port_t* p)
{
    a->port = p;
    return 0;
};


nai_int_t nai_aio_set_in_loop(nai_aio_t* a, nai_int_t in_loop)
{
    (void)a;
    (void)in_loop;

    return 0;
};


nai_int_t nai_aio_set_waitable(nai_aio_t* a, nai_int_t waitable)
{
    a->waitable = !!waitable;
    return 0;
};


nai_int_t nai_aio_wait(nai_aio_t* a, uint32_t msec)
{
    nai_int_t r;


    if (a->waiting == 0) {
        nai_errno = ENOTSUP;
        r = -1;
        goto _end;
    };

    if (a->code != -1) {
        r = 0;
    } else {
        r = nai_aio_global_wait(a, msec);
    };

_end:
    return r;
};


nai_int_t nai_aio_cancel(nai_aio_t* a)
{
    nai_int_t r;
    nai_aio_port_t* port;
    aio_context_t ctx;
    struct io_event sult;


    port = a->port;
    if (a->waiting || !port) {
        r = 0;
        goto _end;
    };

    if (a->code != -1) {
        r = 0;
        goto _end;
    };

    if (a->waiting == 0 && a->port) {
        port = (nai_aio_port_t*)a->port;
        ctx = port->ctx;
    } else {
        ctx = nai_aio_global_get();
        if (ctx == 0) {
            r = -1;
            goto _end;
        };
    };

    r = io_cancel(ctx, &a->cb, &sult);
    if (r < 0) {
        if (nai_errno == EAGAIN) {
            nai_errno = EINPROGRESS;
        };
        goto _end;
    };

    if (sult.res < 0) {
        a->cb.aio_nbytes = -1;
        a->code = -sult.res;
    } else {
        a->cb.aio_nbytes = sult.res;
        a->code = 0;
    };

    r = 0;

_end:
    return r;
};


intptr_t nai_aio_result(nai_aio_t* a, nai_int_t wait)
{
    intptr_t r;


    if (a->code != -1) {
        (void)0;
    } else if (a->waiting == 0) {
        nai_errno = EINPROGRESS;
        r = -1;
        goto _end;
    } else {
        r = nai_aio_wait(a, wait ? -1 : 0);
        if (r < 0) {
            goto _end;
        };
    };

    r = a->cb.aio_nbytes;
    if (r < 0) {
        nai_errno = a->code;
    };

_end:
    return r;
};


intptr_t nai_aio_read(nai_aio_t* a, 
    nai_fd_t fd, void* p, size_t size, nai_off64_t offset)
{
    nai_int_t r;
    aio_context_t ctx;


    ctx = nai_aio_prepare(a, IOCB_CMD_PREAD);
    if (ctx == 0) {
        r = -1;
        goto _end;
    };

    a->cb.aio_fildes = fd;
    a->cb.aio_buf = (uintptr_t)p;
    a->cb.aio_nbytes = size;
    a->cb.aio_offset = offset;
    r = nai_aio_submit(a, ctx);

_end:
    return r;
};


intptr_t nai_aio_write(nai_aio_t* a, 
    nai_fd_t fd, const void* p, size_t size, nai_off64_t offset)
{
    nai_int_t r;
    aio_context_t ctx;


    ctx = nai_aio_prepare(a, IOCB_CMD_PWRITE);
    if (ctx == 0) {
        r = -1;
        goto _end;
    };

    a->cb.aio_fildes = fd;
    a->cb.aio_buf = (uintptr_t)p;
    a->cb.aio_nbytes = size;
    a->cb.aio_offset = offset;
    r = nai_aio_submit(a, ctx);

_end:
    return r;
};


intptr_t nai_aio_readv(nai_aio_t* a, 
    nai_fd_t fd, nai_bufvec_t* v, nai_int_t count, nai_off64_t offset)
{
    nai_int_t r;
    aio_context_t ctx;


    ctx = nai_aio_prepare(a, IOCB_CMD_PREADV);
    if (ctx == 0) {
        r = -1;
        goto _end;
    };

    a->cb.aio_fildes = fd;
    a->cb.aio_buf = (uintptr_t)v;
    a->cb.aio_nbytes = sizeof(*v) * count;
    a->cb.aio_offset = offset;
    r = nai_aio_submit(a, ctx);

_end:
    return r;
};


intptr_t nai_aio_writev(nai_aio_t* a, 
    nai_fd_t fd, const nai_bufvec_t* v, nai_int_t count, nai_off64_t offset)
{
    nai_int_t r;
    aio_context_t ctx;


    ctx = nai_aio_prepare(a, IOCB_CMD_PWRITEV);
    if (ctx == 0) {
        r = -1;
        goto _end;
    };

    a->cb.aio_fildes = fd;
    a->cb.aio_buf = (uintptr_t)v;
    a->cb.aio_nbytes = sizeof(*v) * count;
    a->cb.aio_offset = offset;
    r = nai_aio_submit(a, ctx);

_end:
    return r;
};



#elif (NAI_HAVE_AIO_H)


#include "nai/os/nai_tlocal.h"


#include <string.h>
#if (NAI_HAVE_SYS_TIME_H)
#include <sys/time.h>
#endif


nai_aio_feature_t nai_aiofeat = {
    .support = 1, 
    .accept = 0, 
    .acceptex = 0, 
    .connect = 0, 
#if (NAI_AIO_READV)
    .readv = 1, 
    .readv_sim = 1, 
#else
    .readv = 0, 
    .readv_sim = 0, 
#endif
#if (NAI_AIO_WRITEV)
    .writev = 1, 
    .writev_sim = 1, 
#else
    .writev = 0, 
    .writev_sim = 0, 
#endif
    .recv = 0, 
    .recvv = 0, 
    .recvv_sim = 0, 
    .recvfrom = 0, 
    .recvmsg = 0, 
    .recvmsg_sim = 0, 
    .recvmmsg = 0, 
    .recvmmsg_sim = 0, 
    .send = 0, 
    .sendv = 0, 
    .sendv_sim = 0, 
    .sendto = 0, 
    .sendmsg = 0, 
    .sendmsg_sim = 0, 
    .sendmmsg = 0, 
    .sendmmsg_sim = 0, 
    .sendfile = 0, 
    .sendfile_sim = 0, 
    .sendfilev = 0, 
    .sendfilev_sim = 0
};



static nai_int_t nai_aio_prepare(nai_aio_t* a)
{
#if defined(SIGEV_KEVENT)
    nai_aio_port_t* port;


    port = (nai_aio_port_t*)a->port;
    if (a->waitable == 0 && port) {
        a->waiting = 0;
        a->cb.aio_sigevent.sigev_notify_kqueue = port->fd;
        a->cb.aio_sigevent.sigev_notify = SIGEV_KEVENT;
        a->cb.aio_sigevent.sigev_value.sival_ptr = a;
    } else {
        a->waiting = 1;
        a->cb.aio_sigevent.sigev_notify = SIGEV_NONE;
    };
#else
    a->waiting = 1;
#endif

    a->cb.aio_reqprio = 0;

    return 0;
};


nai_int_t nai_aio_init(nai_aio_t* a)
{
    nai_memset(&a->cb, 0, sizeof(a->cb));
    a->port = 0;
    a->code = 0;
    a->waitable = 0;
    a->waiting = 0;
    a->handle = 0;
    return 0;
};


nai_int_t nai_aio_set_port(nai_aio_t* a, nai_aio_port_t* p)
{
    a->port = p;
    return 0;
};


nai_int_t nai_aio_set_in_loop(nai_aio_t* a, nai_int_t in_loop)
{
    (void)a;
    (void)in_loop;

    return 0;
};


nai_int_t nai_aio_set_waitable(nai_aio_t* a, nai_int_t waitable)
{
    a->waitable = !!waitable;
    return 0;
};


nai_int_t nai_aio_wait(nai_aio_t* a, uint32_t msec)
{
    nai_int_t r;
    struct aiocb const* cb[1];
    struct timespec* tp;
    struct timespec ts;


    if (a->code != -1) {
        r = 0;
        goto _end;
    };

    if (a->waiting == 0) {
        nai_errno = ENOTSUP;
        r = -1;
        goto _end;
    };

    if (msec == (uint32_t)-1) {
        tp = 0;
    } else {
        ts.tv_sec = (long)(msec / 1000);
        ts.tv_nsec = (long)(msec % 1000 * 1000 * 1000);
        tp = &ts;
    };

    cb[0] = &a->cb;
    do {
        r = aio_suspend(cb, 1, tp);
    } while (r < 0 && nai_thread_io_continue(&nai_errno));

_end:
    return r;
};


nai_int_t nai_aio_cancel(nai_aio_t* a)
{
    nai_int_t r;


    r = aio_cancel(a->cb.aio_fildes, &a->cb);

    switch (r) {
    case AIO_NOTCANCELED:
        r = aio_error(&a->cb);
        break;
    case AIO_CANCELED:
    case AIO_ALLDONE:
        if (a->waiting) {
            /* waiting behavior */
            r = 0;
        } else {
            /* default behavior */
            nai_errno = EINPROGRESS;
            r = -1;
        };
        break;
    default:
        break;
    };

    return r;
};


intptr_t nai_aio_result(nai_aio_t* a, nai_int_t wait)
{
    nai_int_t r;
    struct aiocb const* cb[1];


    switch (a->code) {
    case 0:
        r = a->cb.aio_nbytes;
        break;
    case -1:
        r = aio_error(&a->cb);
        if (r < 0) {
            goto _end;
        };
        if (r == EINPROGRESS) {
            if (!wait) {
                nai_errno = r;
                r = -1;
                goto _end;
            };
            if (a->waiting == 0) {
                nai_errno = ENOTSUP;
                r = -1;
                goto _end;
            };

            cb[0] = &a->cb;
            do {
                r = aio_suspend(cb, 1, 0);
            } while (r < 0 && nai_thread_io_continue(&nai_errno));

            if (r < 0) {
                goto _end;
            };

            r = aio_error(&a->cb);
            if (r < 0) {
                goto _end;
            };

            /* on freebsd, aio_return must be called */
        };

        r = aio_return(&a->cb);
        if (r < 0) {
            a->code = nai_errno;
        } else {
            a->code = 0;
            a->cb.aio_nbytes = r;
        };
        break;

    default:
        nai_errno = a->code;
        r = -1;
        break;
    };

_end:
    return r;
};


intptr_t nai_aio_read(nai_aio_t* a, 
    nai_fd_t fd, void* p, size_t size, nai_off64_t offset)
{
    intptr_t r;
    nai_int_t ec;


    nai_aio_prepare(a);
    a->cb.aio_fildes = fd;
    a->cb.aio_offset = offset;
    a->cb.aio_buf = p;
    a->cb.aio_nbytes = size;

    r = aio_read(&a->cb);
    if (r >= 0) {
        nai_errno = EINPROGRESS;
        a->code = -1;
        r = -1;
    } else {
        ec = nai_errno;
        if (ec == EAGAIN) {
            ec = EBUSY;
            nai_errno = ec;
        };
        a->code = ec;
    };

    return r;
};


intptr_t nai_aio_write(nai_aio_t* a, 
    nai_fd_t fd, const void* p, size_t size, nai_off64_t offset)
{
    intptr_t r;
    nai_int_t ec;


    nai_aio_prepare(a);
    a->cb.aio_fildes = fd;
    a->cb.aio_offset = offset;
    a->cb.aio_buf = (void*)p;
    a->cb.aio_nbytes = size;

    r = aio_write(&a->cb);
    if (r >= 0) {
        nai_errno = EINPROGRESS;
        a->code = -1;
        r = -1;
    } else {
        ec = nai_errno;
        if (ec == EAGAIN) {
            ec = EBUSY;
            nai_errno = ec;
        };
        a->code = ec;
    };

    return r;
};


#if (NAI_AIO_READV)

intptr_t nai_aio_readv(nai_aio_t* a, 
    nai_fd_t fd, nai_bufvec_t* v, nai_int_t count, nai_off64_t offset)
{
    intptr_t r;
    nai_int_t ec;


    nai_aio_prepare(a);
    a->cb.aio_fildes = fd;
    a->cb.aio_offset = offset;
    a->cb.aio_iov = (struct iovec*)v;
    a->cb.aio_iovcnt = count;

    r = aio_readv(&a->cb);
    if (r >= 0) {
        nai_errno = EINPROGRESS;
        a->code = -1;
        r = -1;
    } else {
        ec = nai_errno;
        if (ec == EAGAIN) {
            ec = EBUSY;
            nai_errno = ec;
        };
        a->code = ec;
    };

    return r;
};

#endif


#if (NAI_AIO_WRITEV)

intptr_t nai_aio_writev(nai_aio_t* a, 
    nai_fd_t fd, const nai_bufvec_t* v, nai_int_t count, nai_off64_t offset)
{
    intptr_t r;


    nai_aio_prepare(a);
    a->cb.aio_fildes = fd;
    a->cb.aio_offset = offset;
    a->cb.aio_iov = (struct iovec*)v;
    a->cb.aio_iovcnt = count;

    r = aio_writev(&a->cb);
    if (r >= 0) {
        nai_errno = EINPROGRESS;
        a->code = -1;
        r = -1;
    } else {
        ec = nai_errno;
        if (ec == EAGAIN) {
            ec = EBUSY;
            nai_errno = ec;
        };
        a->code = ec;
    };

    return r;
};

#endif



#else


nai_aio_feature_t nai_aiofeat = {
    .support = 0, 
    .accept = 0, 
    .acceptex = 0, 
    .connect = 0, 
    .readv = 0, 
    .readv_sim = 0, 
    .writev = 0, 
    .writev_sim = 0, 
    .recv = 0, 
    .recvv = 0, 
    .recvv_sim = 0, 
    .recvfrom = 0, 
    .recvmsg = 0, 
    .recvmsg_sim = 0, 
    .recvmmsg = 0, 
    .recvmmsg_sim = 0, 
    .send = 0, 
    .sendv = 0, 
    .sendv_sim = 0, 
    .sendto = 0, 
    .sendmsg = 0, 
    .sendmsg_sim = 0, 
    .sendmmsg = 0, 
    .sendmmsg_sim = 0, 
    .sendfile = 0, 
    .sendfile_sim = 0, 
    .sendfilev = 0, 
    .sendfilev_sim = 0
};


#endif



#if !(NAI_AIO_SUPPORT)

nai_int_t nai_aio_init(nai_aio_t* a)
{
    a->cb = 0;
    a->handle = 0;
    return 0;
};

nai_int_t nai_aio_set_port(nai_aio_t* a, nai_aio_port_t* p)
{
    (void)a;
    (void)p;

    nai_errno = ENOTSUP;
    return -1;
};


nai_int_t nai_aio_set_in_loop(nai_aio_t* a, nai_int_t in_loop)
{
    (void)a;
    (void)in_loop;

    nai_errno = ENOTSUP;
    return -1;
};


nai_int_t nai_aio_set_waitable(nai_aio_t* a, nai_int_t waitable)
{
    (void)a;
    (void)waitable;

    nai_errno = ENOTSUP;
    return -1;
};

nai_int_t nai_aio_wait(nai_aio_t* a, uint32_t msec)
{
    (void)a;
    (void)msec;

    nai_errno = ENOTSUP;
    return -1;
};

nai_int_t nai_aio_cancel(nai_aio_t* a)
{
    (void)a;

    nai_errno = ENOTSUP;
    return -1;
};

intptr_t nai_aio_result(nai_aio_t* a, nai_int_t wait)
{
    (void)a;
    (void)wait;

    nai_errno = ENOTSUP;
    return -1;
};

#endif


#if !(NAI_AIO_ACCEPTEX)

nai_int_t nai_aio_acceptex(nai_aio_t* a, nai_fd_t l, 
    nai_fd_t s, void* p, size_t size, nai_int_t extralen)
{
    (void)a;
    (void)l;
    (void)s;
    (void)p;
    (void)size;
    (void)extralen;

    nai_errno = ENOTSUP;
    return -1;
};

nai_int_t nai_aio_acceptex_end(nai_fd_t l, 
    nai_fd_t s, void* p, size_t size, nai_int_t extralen, 
    nai_sockaddr_t** sockname, nai_int_t* socknamelen, 
    nai_sockaddr_t** peername, nai_int_t* peernamelen)
{
    (void)l;
    (void)s;
    (void)p;
    (void)size;
    (void)extralen;
    (void)sockname;
    (void)socknamelen;
    (void)peername;
    (void)peernamelen;

    nai_errno = ENOTSUP;
    return -1;
};

#endif


#if !(NAI_AIO_ACCEPT)


nai_int_t nai_aio_accept(nai_aio_t* a, 
    nai_fd_t l, nai_sockaddr_t* name, nai_int_t* namelen)
{
    (void)a;
    (void)l;
    (void)name;
    (void)namelen;

    nai_errno = ENOTSUP;
    return -1;
};


#endif


#if !(NAI_AIO_CONNECT)

nai_int_t nai_aio_connect(nai_aio_t* a, 
    nai_fd_t s, const nai_sockaddr_t* name, nai_int_t namelen)
{
    (void)a;
    (void)s;
    (void)name;
    (void)namelen;

    nai_errno = ENOTSUP;
    return -1;
};

nai_int_t nai_aio_connect_end(nai_fd_t s)
{
    (void)s;

    nai_errno = ENOTSUP;
    return -1;
};


#endif


#if !(NAI_AIO_READ)

intptr_t nai_aio_read(nai_aio_t* a, nai_fd_t fd, 
    void* p, size_t size, nai_off64_t offset)
{
    (void)a;
    (void)fd;
    (void)p;
    (void)size;
    (void)offset;

    nai_errno = ENOTSUP;
    return -1;
};

#endif

#if !(NAI_AIO_READV)

intptr_t nai_aio_readv(nai_aio_t* a, 
    nai_fd_t fd, nai_bufvec_t* v, nai_int_t count, nai_off64_t offset)
{
    (void)a;
    (void)fd;
    (void)v;
    (void)count;
    (void)offset;

    nai_errno = ENOTSUP;
    return -1;
};

#endif

#if !(NAI_AIO_WRITE)

intptr_t nai_aio_write(nai_aio_t* a, nai_fd_t fd, 
    const void* p, size_t size, nai_off64_t offset)
{
    (void)a;
    (void)fd;
    (void)p;
    (void)size;
    (void)offset;

    nai_errno = ENOTSUP;
    return -1;
};

#endif

#if !(NAI_AIO_WRITEV)

intptr_t nai_aio_writev(nai_aio_t* a, 
    nai_fd_t fd, const nai_bufvec_t* v, nai_int_t count, nai_off64_t offset)
{
    (void)a;
    (void)fd;
    (void)v;
    (void)count;
    (void)offset;

    nai_errno = ENOTSUP;
    return -1;
};

#endif


#if !(NAI_AIO_SOCKET)

intptr_t nai_aio_recv(nai_aio_t* a, 
    nai_fd_t s, void* p, size_t size, nai_int_t flags)
{
    (void)a;
    (void)s;
    (void)p;
    (void)size;
    (void)flags;

    nai_errno = ENOTSUP;
    return -1;
};

intptr_t nai_aio_recvv(nai_aio_t* a, 
    nai_fd_t s, nai_bufvec_t* v, nai_int_t count, nai_int_t flags)
{
    (void)a;
    (void)s;
    (void)v;
    (void)count;
    (void)flags;

    nai_errno = ENOTSUP;
    return -1;
};

intptr_t nai_aio_recvfrom(nai_aio_t* a, nai_fd_t s, 
    void* p, size_t size, nai_int_t flags, 
    nai_sockaddr_t* name, nai_int_t* namelen)
{
    (void)a;
    (void)s;
    (void)p;
    (void)size;
    (void)flags;
    (void)name;
    (void)namelen;

    nai_errno = ENOTSUP;
    return -1;
};

intptr_t nai_aio_recvm(nai_aio_t* a, nai_fd_t s, 
    nai_bufvec_t* v, nai_int_t count, nai_int_t flags, 
    nai_sockaddr_t* name, nai_int_t* namelen, 
    void* ctrl, nai_int_t* ctrllen)
{
    (void)a;
    (void)s;
    (void)v;
    (void)count;
    (void)flags;
    (void)name;
    (void)namelen;
    (void)ctrl;
    (void)ctrllen;

    nai_errno = ENOTSUP;
    return -1;
};

intptr_t nai_aio_send(nai_aio_t* a, 
    nai_fd_t s, const void* p, size_t size, nai_int_t flags)
{
    (void)a;
    (void)s;
    (void)p;
    (void)size;
    (void)flags;

    nai_errno = ENOTSUP;
    return -1;
};

intptr_t nai_aio_sendv(nai_aio_t* a, 
    nai_fd_t s, const nai_bufvec_t* v, nai_int_t count, nai_int_t flags)
{
    (void)a;
    (void)s;
    (void)v;
    (void)count;
    (void)flags;

    nai_errno = ENOTSUP;
    return -1;
};

intptr_t nai_aio_sendto(nai_aio_t* a, nai_fd_t s, 
    const void* p, size_t size, nai_int_t flags, 
    const nai_sockaddr_t* name, nai_int_t namelen)
{
    (void)a;
    (void)s;
    (void)p;
    (void)size;
    (void)flags;
    (void)name;
    (void)namelen;

    nai_errno = ENOTSUP;
    return -1;
};

intptr_t nai_aio_sendm(nai_aio_t* a, nai_fd_t s, 
    const nai_bufvec_t* v, nai_int_t count, nai_int_t flags, 
    const nai_sockaddr_t* name, nai_int_t namelen, 
    const void* ctrl, nai_int_t ctrllen)
{
    (void)a;
    (void)s;
    (void)v;
    (void)count;
    (void)flags;
    (void)name;
    (void)namelen;
    (void)ctrl;
    (void)ctrllen;

    nai_errno = ENOTSUP;
    return -1;
};

intptr_t nai_aio_sendmm(nai_aio_t* a, nai_fd_t s, 
    const nai_bufarray_t* v, nai_int_t count, nai_int_t flags, 
    const nai_sockaddr_t* name, nai_int_t namelen)
{
    (void)a;
    (void)s;
    (void)v;
    (void)count;
    (void)flags;
    (void)name;
    (void)namelen;

    nai_errno = ENOTSUP;
    return -1;
};

#endif


#if !(NAI_AIO_SENDFILE)

intptr_t nai_aio_sendfile(nai_aio_t* a, 
    nai_fd_t s, const nai_filechunk_t* c, size_t* submit)
{
    (void)a;
    (void)s;
    (void)c;
    (void)submit;

    nai_errno = ENOTSUP;
    return -1;
};

intptr_t nai_aio_sendfilev(nai_aio_t* a, 
    nai_fd_t s, const nai_filevec_t* v, nai_int_t count, size_t* submit)
{
    (void)a;
    (void)s;
    (void)v;
    (void)count;
    (void)submit;

    nai_errno = ENOTSUP;
    return -1;
};

#endif



#endif

