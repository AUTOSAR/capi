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
/// @file       nai_socket.c
/// @brief      
/// @details
/// @date       2020-11-28
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai/os/nai_socket.h"
#include "nai/runtime/nai_errno.h"



uint16_t nai_ntohs_ua(const uint16_t* p) {
    uint16_t n;
    uint8_t* b = (uint8_t*)p;
#if (NAI_HAVE_BIG_ENDIAN)
    n = (b[1] << 8) | b[0];
#else
    n = (b[0] << 8) | b[1];
#endif
    return n;
};


uint32_t nai_ntohl_ua(const uint32_t* p)
{
    uint32_t n;
    uint8_t* b = (uint8_t*)p;
#if (NAI_HAVE_BIG_ENDIAN)
    n = (b[3] << 24)|(b[2] << 16)|(b[1] << 8)|b[0];
#else
    n = (b[0] << 24)|(b[1] << 16)|(b[2] << 8)|b[3];
#endif
    return n;
};


uint16_t nai_htons_ua(const uint16_t* p) {
    uint16_t n;
    uint8_t* b = (uint8_t*)p;
#if (NAI_HAVE_BIG_ENDIAN)
    n = (b[1] << 8) | b[0];
#else
    n = (b[0] << 8) | b[1];
#endif
    return n;
};


uint32_t nai_htonl_ua(const uint32_t* p)
{
    uint32_t n;
    uint8_t* b = (uint8_t*)p;
#if (NAI_HAVE_BIG_ENDIAN)
    n = (b[3] << 24)|(b[2] << 16)|(b[1] << 8)|b[0];
#else
    n = (b[0] << 24)|(b[1] << 16)|(b[2] << 8)|b[3];
#endif
    return n;
};



#if defined(_WIN32)


#include "win/nai_windows.h"
#include "win/nai_wsock.h"
#pragma comment(lib, "ws2_32.lib")


#define SHUT_RD         SD_RECEIVE
#define SHUT_WR         SD_SEND
#define SHUT_RDWR       SD_BOTH

#define SO_EXCLUADDR    SO_EXCLUSIVEADDRUSE
#define SO_REUSEPORT    SO_REUSEADDR


typedef int socklen_t;
typedef SOCKET nai_sock_t;


#define close           closesocket
#define nai_sock_check  nai_sock_errno


static nai_int_t nai_sock_errno()
{
    nai_int_t ec;
    DWORD e = GetLastError();


    switch (e) {
    case WSAEWOULDBLOCK:
        ec = NAI_EAGAIN;
        break;
    default:
        ec = nai_errno_from_oserr(e);
        break;
    };

    nai_errno = ec;

    return ec;
};


#else


#include "nai/os/nai_tlocal.h"


#if (NAI_HAVE_SYS_UIO_H)
#include <sys/uio.h>
#endif
#if (NAI_HAVE_SYS_TYPES_H)
#include <sys/types.h>
#endif
#if (NAI_HAVE_SYS_SOCKET_H)
#include <sys/socket.h>
#endif
#if (NAI_HAVE_UNISTD_H)
#include <unistd.h>
#endif


typedef int nai_sock_t;


#define nai_sock_check()        (void)0
#define nai_sock_errno()        nai_thread_io_errno()


#endif


nai_fd_t nai_sock_open(nai_int_t af, nai_int_t type, nai_int_t protocol)
{
    nai_fd_t r;


#if defined(SOCK_CLOEXEC)
    type |= SOCK_CLOEXEC;
#endif

    r = (nai_fd_t)socket(af, type, protocol);
    if (r == NAI_FD_INVALID) {
        nai_sock_check();
    };

#if !defined(SOCK_CLOEXEC) && !defined(_WIN32)
    if (r >= 0) {
        if (nai_file_set_cloexec(r, 1) < 0) {
            nai_int_t ec;
            ec = nai_errno;
            nai_sock_close(r);
            nai_errno = ec;
            r = -1;
        };
    };
#endif

    return r;
};


nai_fd_t nai_sock_openat(
    const nai_sockaddr_t* name, 
    nai_int_t namelen, nai_int_t type, nai_int_t flags)
{
    nai_int_t n;
    nai_int_t ec;
    nai_fd_t r;


    r = nai_sock_open(name->sa_family, type, 0);
    if (r == NAI_FD_INVALID) {
        goto _end;
    };

    n = nai_sock_reuse(r, 1, flags);
    if (n < 0) {
        goto _fail;
    };

    n = nai_sock_bind(r, name, namelen);
    if (n < 0) {
        goto _fail;
    };

_end:
    return r;

_fail:
    ec = nai_errno;
    nai_sock_close(r);
    nai_errno = ec;
    r = NAI_FD_INVALID;
    goto _end;
};


nai_fd_t nai_sock_accept(nai_fd_t s, nai_sockaddr_t* name, nai_int_t* namelen)
{
    nai_int_t ec;
    nai_fd_t r;


    do {
#if (NAI_HAVE_ACCEPT4)
        r = (nai_fd_t)accept4(
            (nai_sock_t)s, name, (socklen_t*)namelen, SOCK_CLOEXEC);
#else
        r = (nai_fd_t)accept(
            (nai_sock_t)s, name, (socklen_t*)namelen);
#endif
        if (r != NAI_FD_INVALID) {
            break;
        };

        ec = nai_sock_errno();

    } while (ec == EINTR);

#if !(NAI_HAVE_ACCEPT4) && !defined(_WIN32)
    if (r >= 0) {
        if (nai_file_set_cloexec(r, 1) < 0) {
            ec = nai_errno;
            nai_sock_close(r);
            nai_errno = ec;
            r = -1;
        };
    };
#endif

    return r;
}


nai_int_t nai_sock_close(nai_fd_t s)
{
    nai_int_t r;


#if defined(_WIN32)
    r = close((nai_sock_t)s);
    if (r == -1) {
        nai_sock_check();
    };
#else
    r = nai_file_close(s);
#endif

    return r;
};


nai_int_t nai_sock_reuse(nai_fd_t s, nai_int_t on, nai_int_t flags)
{
    nai_int_t r;
    nai_int_t value;


    if (flags & NAI_SOCK_REUSEADDR) {
#if defined(SO_EXCLUADDR)
        value = !on;
        r = nai_sock_set_opt(s, 
            SOL_SOCKET, SO_EXCLUADDR, (char*)&value, sizeof(value));
#elif defined(SO_REUSEADDR)
        value = !!on;
        r = nai_sock_set_opt(s, 
            SOL_SOCKET, SO_REUSEADDR, (char*)&value, sizeof(value));
#else
        nai_errno = ENOTSUP;
        r = -1;
#endif
        if (r < 0) {
            goto _end;
        };
    };

    if (flags & NAI_SOCK_REUSEPORT) {
#if defined(SO_REUSEPORT)
        value = !!on;
        r = nai_sock_set_opt(s, 
            SOL_SOCKET, SO_REUSEPORT, (char*)&value, sizeof(value));
#else
        nai_errno = ENOTSUP;
        r = -1;
#endif
        if (r < 0) {
            goto _end;
        };
    };

    r = 0;

_end:
    return r;
};


nai_int_t nai_sock_bind(nai_fd_t s, 
    const nai_sockaddr_t* name, nai_int_t namelen)
{
    nai_int_t r;


    r = bind((nai_sock_t)s, name, namelen);
    if (r == -1) {
        nai_sock_check();
    };

    return r;
}


nai_int_t nai_sock_listen(nai_fd_t s, nai_int_t backlog)
{
    nai_int_t r;


    r = listen((nai_sock_t)s, backlog);
    if (r == -1) {
        nai_sock_check();
    };

    return r;
};


nai_int_t nai_sock_connect(nai_fd_t s, 
    const nai_sockaddr_t* name, nai_int_t namelen)
{
    nai_int_t r;
    nai_int_t ec;


    do {
        r = connect((nai_sock_t)s, name, namelen);
        if (r != -1) {
            break;
        };

        ec = nai_sock_errno();

    } while (ec == EINTR);

    return r;
};


nai_int_t nai_sock_shutdown(nai_fd_t s, nai_int_t how)
{
    nai_int_t r;
    nai_int_t w;


    switch (how) {
    case NAI_SOCK_RD:
        w = SHUT_RD;
        break;
    case NAI_SOCK_WR:
        w = SHUT_WR;
        break;
    case NAI_SOCK_RW:
        w = SHUT_RDWR;
        break;
    default:
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    r = shutdown((nai_sock_t)s, w);
    if (r == -1) {
        nai_sock_check();
    };

_end:
    return r;
};


nai_int_t nai_sock_set_opt(nai_fd_t s, 
    nai_int_t level, nai_int_t optname, const char* optval, nai_int_t optlen)
{
    nai_int_t r;


    r = setsockopt((nai_sock_t)s, level, optname, optval, optlen);
    if (r == -1) {
        nai_sock_check();
    };

    return r;
};


nai_int_t nai_sock_get_opt(nai_fd_t s, 
    nai_int_t level, nai_int_t optname, char* optval, nai_int_t* optlen)
{
    nai_int_t r;


    r = getsockopt((nai_sock_t)s, level, optname, optval, (socklen_t*)optlen);
    if (r == -1) {
        nai_sock_check();
    };

    return r;
};


nai_int_t nai_sock_get_sockname(
    nai_fd_t s, nai_sockaddr_t* name, nai_int_t* namelen)
{
    nai_int_t r;


    r = getsockname((nai_sock_t)s, name, (socklen_t*)namelen);
    if (r == -1) {
        nai_sock_check();
    };

    return r;
};


nai_int_t nai_sock_get_peername(
    nai_fd_t s, nai_sockaddr_t* name, nai_int_t* namelen)
{
    nai_int_t r;


    r = getpeername((nai_sock_t)s, name, (socklen_t*)namelen);
    if (r == -1) {
        nai_sock_check();
    };

    return r;
};



#if defined(_WIN32)


nai_int_t nai_sock_set_blocking(nai_fd_t s, nai_int_t on)
{
    nai_int_t r;
    DWORD nb = !on;


    r = ioctlsocket((nai_sock_t)s, FIONBIO, &nb);
    if (r == -1) {
        nai_sock_check();
    };

    return r;
};


nai_int_t nai_sock_poll(nai_fd_t s, nai_int_t events, uint32_t msec)
{
    nai_int_t r;
    struct timeval tv, *tp;
    fd_set* read = 0;
    fd_set* write = 0;
    fd_set set[2];


    if (msec == -1) {
        tp = 0;
    } else {
        tv.tv_sec = (long) (msec / 1000);
        tv.tv_usec = (long) ((msec % 1000) * 1000);
        tp = &tv;
    };

    if (events & NAI_POLL_READ) {
        read = &set[0];
        FD_ZERO(read);
        FD_SET((SOCKET)s, read);
    };
    if (events & NAI_POLL_WRITE) {
        write = &set[1];
        FD_ZERO(write);
        FD_SET((SOCKET)s, write);
    };

    r = select(1, read, write, 0, tp);
    if (r == 0) {
        nai_errno = ETIMEDOUT;
        r = -1;
    } else if (r == SOCKET_ERROR) {
        nai_sock_check();
    };

    return r;
};

intptr_t nai_sock_recv(nai_fd_t s, 
    void* p, size_t len, nai_int_t flags)
{
    intptr_t r;
    WSABUF v;
    DWORD bytes;
    DWORD opt = flags;


    v.buf = (char*)p;
    v.len = (uint32_t)len;

    r = WSARecv((nai_sock_t)s, &v, 1, &bytes, &opt, 0, 0);
    if (r == SOCKET_ERROR) {
        nai_sock_check();
        goto _end;
    };

    r = (intptr_t)bytes;

_end:
    return r;
};


intptr_t nai_sock_recvv(nai_fd_t s, 
    nai_bufvec_t* v, nai_int_t count, nai_int_t flags)
{
    intptr_t r;
    DWORD bytes;
    DWORD opt = flags;


    r = WSARecv((nai_sock_t)s, (WSABUF*)v, count, &bytes, &opt, 0, 0);
    if (r == SOCKET_ERROR) {
        nai_sock_check();
        goto _end;
    };

    r = (intptr_t)bytes;

_end:
    return r;
};


intptr_t nai_sock_recvfrom(nai_fd_t s, 
    void* p, size_t len, nai_int_t flags, 
    nai_sockaddr_t* name, nai_int_t* namelen)
{
    nai_int_t r;
    WSABUF v;
    DWORD bytes;
    DWORD opt = flags;


    v.buf = (char*)p;
    v.len = (uint32_t)len;

    r = WSARecvFrom((nai_sock_t)s, &v, 1, &bytes, &opt, name, namelen, 0, 0);
    if (r == SOCKET_ERROR) {
        nai_sock_check();
        goto _end;
    };

    r = (intptr_t)bytes;

_end:
    return r;
};


intptr_t nai_sock_recvm(nai_fd_t s, 
    nai_bufvec_t* v, nai_int_t count, nai_int_t flags, 
    nai_sockaddr_t* name, nai_int_t* namelen, 
    void* ctrl, nai_int_t* ctrllen)
{
    intptr_t r;
    DWORD bytes;
    DWORD opt = flags;


    (void)ctrllen;

    if (ctrl) {
        nai_errno = ENOTSUP;
        r = -1;
        goto _end;
    };

    r = WSARecvFrom((nai_sock_t)s, 
        (WSABUF*)v, count, &bytes, &opt, name, namelen, 0, 0);
    if (r == SOCKET_ERROR) {
        nai_sock_check();
        goto _end;
    };

    r = (intptr_t)bytes;

_end:
    return r;
};


intptr_t nai_sock_send(nai_fd_t s, 
    const void* p, size_t len, nai_int_t flags)
{
    intptr_t r;
    WSABUF v;
    DWORD bytes;


    v.buf = (char*)p;
    v.len = (uint32_t)len;

    r = WSASend((nai_sock_t)s, &v, 1, &bytes, flags, 0, 0);
    if (r == SOCKET_ERROR) {
        nai_sock_check();
        goto _end;
    };

    r = (intptr_t)bytes;

_end:
    return r;
}


intptr_t nai_sock_sendv(nai_fd_t s, 
    const nai_bufvec_t* v, nai_int_t count, nai_int_t flags)
{
    intptr_t r;
    DWORD bytes;


    r = WSASend((nai_sock_t)s, (WSABUF*)v, count, &bytes, flags, 0, 0);
    if (r == SOCKET_ERROR) {
        nai_sock_check();
        goto _end;
    };

    r = (intptr_t)bytes;

_end:
    return r;
};


intptr_t nai_sock_sendto(nai_fd_t s, 
    const void* p, size_t len, nai_int_t flags, 
    const nai_sockaddr_t* name, nai_int_t namelen)
{
    intptr_t r;
    WSABUF v;
    DWORD bytes;


    v.buf = (char*)p;
    v.len = (uint32_t)len;

    r = WSASendTo((nai_sock_t)s, &v, 1, &bytes, flags, name, namelen, 0, 0);
    if (r == SOCKET_ERROR) {
        nai_sock_check();
        goto _end;
    };

    r = (intptr_t)bytes;

_end:
    return r;
};


intptr_t nai_sock_sendm(nai_fd_t s, 
    const nai_bufvec_t* v, nai_int_t count, nai_int_t flags, 
    const nai_sockaddr_t* name, nai_int_t namelen, 
    const void* ctrl, nai_int_t ctrllen)
{
    intptr_t r;
    DWORD bytes;


    (void)ctrllen;

    if (ctrl) {
        nai_errno = ENOTSUP;
        r = -1;
        goto _end;
    };

    r = WSASendTo((nai_sock_t)s, 
        (WSABUF*)v, count, &bytes, flags, name, namelen, 0, 0);
    if (r == SOCKET_ERROR) {
        nai_sock_check();
        goto _end;
    };

    r = (intptr_t)bytes;

_end:
    return r;
};


intptr_t nai_sock_sendmm(nai_fd_t s, 
    const nai_bufarray_t* v, nai_int_t count, nai_int_t flags, 
    const nai_sockaddr_t* name, nai_int_t namelen)
{
    intptr_t r;


    if (count <= 0) {
        r = 0;
        goto _end;
    };

    r = nai_sock_sendm(s, v[0].v, v[0].count, flags, name, namelen, 0, 0);

_end:
    return r;
};


#else


nai_int_t nai_sock_set_blocking(nai_fd_t s, nai_int_t on)
{
    return nai_file_set_blocking(s, on);
};


nai_int_t nai_sock_poll(nai_fd_t s, nai_int_t events, uint32_t msec)
{
    return nai_file_poll(s, events, msec);
};


intptr_t nai_sock_recv(nai_fd_t s, 
    void* p, size_t len, nai_int_t flags)
{
    nai_int_t ec;
    intptr_t r;


    do {
        r = recv(s, p, len, flags);
        if (r != -1) {
            break;
        };

        ec = nai_sock_errno();

    } while (ec == EINTR);

    return r;
};


intptr_t nai_sock_recvv(nai_fd_t s, 
    nai_bufvec_t* v, nai_int_t count, nai_int_t flags)
{
    intptr_t r;


#if (NAI_HAVE_RECVMSG)
    r = nai_sock_recvm(s, v, count, flags, 0, 0, 0, 0);
#else

    if (count == 1) {
        r = nai_sock_recv(s, v[0].buf, v[0].len, flags);
#if (NAI_HAVE_READV)
    } else if (flags == 0) {
        r = nai_file_readv(s, v, count);
#endif
    } else if (count == 0) {
        r = 0;
    } else {
        nai_errno = ENOTSUP;
        r = -1;
    };

#endif

    return r;
};


intptr_t nai_sock_recvfrom(nai_fd_t s, 
    void* p, size_t len, nai_int_t flags, 
    nai_sockaddr_t* name, nai_int_t* namelen)
{
    nai_int_t ec;
    intptr_t r;


    do {
        r = recvfrom(s, p, len, flags, name, (socklen_t*)namelen);
        if (r != -1) {
            break;
        };

        ec = nai_sock_errno();

    } while (ec == EINTR);

    return r;
};


intptr_t nai_sock_recvm(nai_fd_t s, 
    nai_bufvec_t* v, nai_int_t count, nai_int_t flags, 
    nai_sockaddr_t* name, nai_int_t* namelen, 
    void* ctrl, nai_int_t* ctrllen)
{
    intptr_t r;


#if (NAI_HAVE_RECVMSG)

    nai_int_t ec;
    struct msghdr msg;


    msg.msg_iov = (struct iovec*)v;
    msg.msg_iovlen = count;
    msg.msg_name = (void*)name;
    msg.msg_namelen = namelen ? *namelen : 0;
#if defined(CMSG_FIRSTHDR)
    msg.msg_control = ctrl;
    msg.msg_controllen = ctrllen ? *ctrllen : 0;
    msg.msg_flags = 0;
#else
    msg.msg_accrights = (caddr_t)ctrl;
    msg.msg_accrightslen = ctrllen ? *ctrllen : 0;
#endif

    do {
        r = recvmsg(s, &msg, flags);
        if (r != -1) {
            break;
        };

        ec = nai_sock_errno();

    } while (ec == EINTR);

    if (r >= 0) {
        if (namelen) {
            namelen[0] = msg.msg_namelen;
        };
        if (ctrllen) {
#if defined(CMSG_FIRSTHDR)
            ctrllen[0] = msg.msg_controllen;
#else
            ctrllen[0] = msg.msg_accrightslen;
#endif
        };
    };

#else

    (void)ctrllen;

    if (ctrl) {
        nai_errno = ENOTSUP;
        r = -1;
    } else if (count == 1) {
        r = nai_sock_recvfrom(s, v[0].buf, v[0].len, flags, name, namelen);
#if (NAI_HAVE_READV)
    } else if (flags == 0 && name == 0) {
        r = nai_file_readv(s, v, count);
#endif
    } else if (count == 0) {
        r = 0;
    } else {
        nai_errno = ENOTSUP;
        r = -1;
    };

#endif

    return r;
};


intptr_t nai_sock_send(nai_fd_t s, 
    const void* p, size_t len, nai_int_t flags)
{
    nai_int_t ec;
    intptr_t r;


    do {
        r = send(s, p, len, flags);
        if (r != -1) {
            break;
        };

        ec = nai_sock_errno();

    } while (ec == EINTR);

    return r;
};


intptr_t nai_sock_sendv(nai_fd_t s, 
    const nai_bufvec_t* v, nai_int_t count, nai_int_t flags)
{
    intptr_t r;


#if (NAI_HAVE_SENDMSG)
    r = nai_sock_sendm(s, v, count, flags, 0, 0, 0, 0);
#else

    if (count == 1) {
        r = nai_sock_send(s, v[0].buf, v[0].len, flags);
#if (NAI_HAVE_WRITEV)
    } else if (flags == 0) {
        r = nai_file_writev(s, v, count);
#endif
    } else if (count == 0) {
        r = 0;
    } else {
        nai_errno = ENOTSUP;
        r = -1;
    };

#endif

    return r;
};


intptr_t nai_sock_sendto(nai_fd_t s, 
    const void* p, size_t len, nai_int_t flags, 
    const nai_sockaddr_t* name, nai_int_t namelen)
{
    nai_int_t ec;
    intptr_t r;


    do {
        r = sendto(s, p, len, flags, name, namelen);
        if (r != -1) {
            break;
        };

        ec = nai_sock_errno();

    } while (ec == EINTR);

    return r;
};


intptr_t nai_sock_sendm(nai_fd_t s, 
    const nai_bufvec_t* v, nai_int_t count, nai_int_t flags, 
    const nai_sockaddr_t* name, nai_int_t namelen, 
    const void* ctrl, nai_int_t ctrllen)
{
    intptr_t r;


#if (NAI_HAVE_SENDMSG)

    nai_int_t ec;
    struct msghdr msg;


    msg.msg_iov = (struct iovec*)v;
    msg.msg_iovlen = count;
    msg.msg_name = (void*)name;
    msg.msg_namelen = namelen;
#if defined(CMSG_FIRSTHDR)
    msg.msg_control = (void*)ctrl;
    msg.msg_controllen = ctrllen;
    msg.msg_flags = 0;
#else
    msg.msg_accrights = (caddr_t)ctrl;
    msg.msg_accrightslen = ctrllen;
#endif

    do {
        r = sendmsg(s, &msg, flags);
        if (r != -1) {
            break;
        };

        ec = nai_sock_errno();

    } while (ec == EINTR);

#else

    (void)ctrllen;

    if (ctrl) {
        nai_errno = ENOTSUP;
        r = -1;
    } else if (count == 1) {
        r = nai_sock_sendto(s, v[0].buf, v[0].len, flags, name, namelen);
#if (NAI_HAVE_WRITEV)
    } else if (flags == 0 && name == 0) {
        r = nai_file_writev(s, v, count);
#endif
    } else if (count == 0) {
        r = 0;
    } else {
        nai_errno = ENOTSUP;
        r = -1;
    };

#endif

    return r;
};


intptr_t nai_sock_sendmm(nai_fd_t s, 
    const nai_bufarray_t* v, nai_int_t count, nai_int_t flags, 
    const nai_sockaddr_t* name, nai_int_t namelen)
{
    intptr_t r;


#if !(__qnx__) && (NAI_HAVE_SENDMMSG)

    nai_int_t n;
    nai_int_t ec;
    struct mmsghdr msg[NAI_BUFA_MAX];

    if (count > NAI_BUFA_MAX) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    for (n = 0; n < count; n ++) {
        msg[n].msg_hdr.msg_iov = (struct iovec*)v[n].v;
        msg[n].msg_hdr.msg_iovlen = v[n].count;
        msg[n].msg_hdr.msg_name = (void*)name;
        msg[n].msg_hdr.msg_namelen = namelen;
#if defined(CMSG_FIRSTHDR)
        msg[n].msg_hdr.msg_control = 0;
        msg[n].msg_hdr.msg_controllen = 0;
        msg[n].msg_hdr.msg_flags = 0;
#else
        msg[n].msg_hdr.msg_accrights = 0;
        msg[n].msg_hdr.msg_accrightslen = 0;
#endif
        msg[n].msg_len = 0;
    };

    do {
        r = sendmmsg(s, msg, count, flags);
        if (r != -1) {
            break;
        };

        ec = nai_sock_errno();

    } while (ec == EINTR);

    if (r >= 0) {
        count = (nai_int_t)r;
        r = 0;
        for (n = 0; n < count; n ++) {
            r += msg[n].msg_len;
        };
    };

#else

    if (count <= 0) {
        r = 0;
        goto _end;
    };

    r = nai_sock_sendm(s, v[0].v, v[0].count, flags, name, namelen, 0, 0);

#endif

_end:
    return r;
};


#endif



//////////////////////////////////////////////////////////////////////////////
// deprecated apis


nai_fd_t nai_sock_create(nai_int_t af, nai_int_t type, nai_int_t protocol)
{
    return nai_sock_open(af, type, protocol);
};


nai_int_t nai_sock_blocking(nai_fd_t s, nai_int_t on)
{
    return nai_sock_set_blocking(s, on);
};


nai_int_t nai_sock_setsockopt(nai_fd_t s, 
    nai_int_t level, nai_int_t optname, const char* optval, nai_int_t optlen)
{
    return nai_sock_set_opt(s, level, optname, optval, optlen);
};


nai_int_t nai_sock_getsockopt(nai_fd_t s, 
    nai_int_t level, nai_int_t optname, char* optval, nai_int_t* optlen)
{
    return nai_sock_get_opt(s, level, optname, optval, optlen);
};


nai_int_t nai_sock_getsockname(
    nai_fd_t s, nai_sockaddr_t* name, nai_int_t* namelen)
{
    return nai_sock_get_sockname(s, name, namelen);
};


nai_int_t nai_sock_getpeername(
    nai_fd_t s, nai_sockaddr_t* name, nai_int_t* namelen)
{
    return nai_sock_get_peername(s, name, namelen);
};



