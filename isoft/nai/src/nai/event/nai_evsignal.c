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
/// @file       nai_evsignal.c
/// @brief      
/// @details
/// @date       2020-11-28
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai_evbase.h"
#include "nai/nai_config.h"
#include "nai/os/nai_pipe.h"
#include "nai/os/nai_socket.h"
#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_log.h"
#include "nai/runtime/nai_util.h"



#if (NAI_HAVE_EVENTFD)
#include <sys/eventfd.h>
#endif


nai_int_t nai_evsignal_init(nai_evsignal_t* s)
{
    s->signaled = 0;
    s->count = 0;
    s->in = NAI_FD_INVALID;
    s->out = NAI_FD_INVALID;
    return 0;
};


nai_int_t nai_evsignal_open(nai_evsignal_t* s)
{
    nai_int_t r;


#if (NAI_HAVE_EVENTFD)

    nai_int_t oflags;

#if !defined(EFD_CLOEXEC)
    nai_int_t ec;
    oflags = 0;
#else
    oflags = EFD_CLOEXEC;
#endif

    s->in = eventfd(0, oflags);
    if (s->in == NAI_FD_INVALID) {
        r = -1;
        goto _end;
    };

#if !defined(EFD_CLOEXEC)
    r = nai_file_set_cloexec(s->in, 1);
    if (r < 0) {
        ec = nai_errno;
        nai_file_close(s->in);
        s->in = NAI_FD_INVALID;
        nai_errno = ec;
        goto _end;
    };
#endif

    s->out = s->in;
    r = 0;

_end:

#else

    nai_int_t ec;

#if defined(_WIN32)
#define NAI_EV_USE_SOCKET       1
#endif

#if (NAI_EV_USE_SOCKET)

    nai_int_t nodelay;
    nai_int_t addrlen;
    nai_fd_t l = NAI_FD_INVALID;
    nai_socknbuf_in_t nbuf;
    nai_socknbuf_in_t addr;


    addrlen = sizeof(addr);
    r = nai_sockaddr_pton("127.0.0.1", 
        sizeof("127.0.0.1") - 1, (nai_sockaddr_t*)&addr, &addrlen);
    if (r < 0) {
        goto _end;
    }

    l = nai_sock_open(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (l == NAI_FD_INVALID) {
        r = -1;
        goto _end;
    };

    r = nai_sock_bind(l, (nai_sockaddr_t*)&addr, addrlen);
    if (r < 0) {
        goto _end;
    };

    r = nai_sock_listen(l, 0);
    if (r < 0) {
        goto _end;
    };

    nbuf.len = sizeof(nbuf.storage);
    r = nai_sock_get_sockname(l, &nbuf.addr, &nbuf.len);
    if (r < 0) {
        goto _end;
    };

    s->in = nai_sock_open(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s->in == NAI_FD_INVALID) {
        r = -1;
        goto _end;
    };

    r = nai_sock_bind(s->in, (nai_sockaddr_t*)&addr, addrlen);
    if (r < 0) {
        goto _end;
    };

    r = nai_sock_connect(s->in, &nbuf.addr, nbuf.len);
    if (r < 0) {
        goto _end;
    };

    s->out = nai_sock_accept(l, 0, 0);
    if (s->out == NAI_FD_INVALID) {
        r = -1;
        goto _end;
    };

    nodelay = 1;
    r = nai_sock_set_opt(s->out, 
        IPPROTO_TCP, TCP_NODELAY, (const char*)&nodelay, sizeof(nai_int_t));
    if (r < 0) {
        goto _end;
    };

    r = nai_sock_set_blocking(s->in, 0);

#else

    r = nai_pipe(s->fds, 0);
    if (r < 0) {
        goto _end;
    };

    r = nai_file_set_blocking(s->in, 0);

#endif

_end:

    if (r < 0) {
        ec = nai_errno;
        nai_evsignal_close(s);
    };

#if (NAI_EV_USE_SOCKET)
    if (l != NAI_FD_INVALID) {
        nai_sock_close(l);
    };
#endif

    if (r < 0) {
        nai_errno = ec;
    };


#endif


    return r;
};


nai_int_t nai_evsignal_set(nai_evsignal_t* s)
{
    nai_int_t r;
    uint64_t c;


    if (s->signaled) {
        r = 0;
    } else {
        s->signaled = 1;

#if (NAI_HAVE_EVENTFD)
        c = 1;
        r = (nai_int_t)nai_file_write(s->out, &c, sizeof(c));
#elif !defined(_WIN32)
        c = 0;
        r = (nai_int_t)nai_file_write(s->out, &c, 1);
#else
        c = 0;
        r = (nai_int_t)nai_sock_send(s->out, &c, 1, 0);
#endif
        if (r < 0) {
            s->signaled = 0;
        };
    };

    return r;
};


nai_int_t nai_evsignal_reset(nai_evsignal_t* s, nai_int_t et)
{
    nai_int_t r;
    nai_int_t ec;
#if (NAI_HAVE_EVENTFD)
    uint8_t buf[8];
#else
    uint8_t buf[128];
#endif


    /* signal is reached, if 'signaled' is not seted, 
     * indicates out-of-sync issues caused by concurrent modifications.
     */
    if (!s->signaled) {
        nai_log_debug(NAI_LOG_CORE, 0, 
            "a problem with concurrent modifications may have occurred");
    };


#if (NAI_HAVE_EVENTFD)
    if (et) { /* is edge tigger event mode */
        s->count ++;
        if (s->count != -1) {
            /* there is a problem with concurrent modifications here */
            s->signaled = 0;
            r = 0;
            goto _end;
        };
    };
#else
    (void)et;
#endif


#if !defined(_WIN32)
    r = (nai_int_t)nai_file_read(s->in, buf, sizeof(buf));
#else
    r = (nai_int_t)nai_sock_recv(s->in, buf, sizeof(buf), 0);
#endif
    if (r > 0) {
        /* ok */
        ;
    } else if (r == 0) {
        /* end of file, why? */
        assert(0);
    } else {
        ec = nai_errno;
        if (ec != EAGAIN) {
            /* read error */
            nai_log_error(NAI_LOG_CORE, ec, "read signal fd failed");
        } else {
            /* ignore */
            r = 0;
        };
    };

    /* there is a problem with concurrent modifications here */
    s->signaled = 0;


#if (NAI_HAVE_EVENTFD)
_end:
#endif
    return r;
};


nai_int_t nai_evsignal_close(nai_evsignal_t* s)
{
    nai_int_t r;


    if (s->in != NAI_FD_INVALID) {
        /* nai_sock_close eq nai_file_close on !win32 */
        nai_sock_close(s->in);
    };
    if (s->out != NAI_FD_INVALID && s->out != s->in) {
        /* nai_sock_close eq nai_file_close on !win32 */
        nai_sock_close(s->out);
    };

    s->in = NAI_FD_INVALID;
    s->out = NAI_FD_INVALID;
    s->signaled = 0;
    r = 0;

    return r;
};

