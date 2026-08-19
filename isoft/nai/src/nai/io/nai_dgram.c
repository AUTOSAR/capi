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
/// @file       nai_dgram.c
/// @brief      
/// @details
/// @date       2020-11-28
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai_iobase.h"


extern nai_dgram_ops_t nai_dgram_sock_nb;
extern nai_dgram_ops_t nai_dgram_sock_aio;


static nai_dgram_ops_t* nai_dgram_ops[] = {
    &nai_dgram_sock_nb, 
    &nai_dgram_sock_aio, 
};


static nai_dgram_ops_t* nai_dgram_ops_find(
    nai_int_t type, nai_int_t polling, nai_evloop_t* l)
{
    nai_int_t n;
    nai_int_t c;
    nai_int_t t, f;
    nai_dgram_ops_t* p;
    nai_dgram_ops_t* r;


    t = (NAI_EV_FEAT_FDSTART << type);

    if (l == 0) {
        f = NAI_EV_FEAT_BLOCK | t;
    } else {
        if (polling) {
            f = nai_evloop_get_feature(l, NAI_EV_FEAT_POLL);
        } else {
            f = 0;
        };
        if (!(f & t)) {
            f = nai_evloop_get_fd_feature(l, type);
        };
    };

    r = 0;
    for (n = 0; n < (nai_int_t)nai_countof(nai_dgram_ops); n ++) {
        p = nai_dgram_ops[n];

        /* check io model */
        c = p->require & f;
        c &= NAI_EV_FEAT_MODELMASK;
        if (c == 0) {
            continue;
        };

        /* check fd type */
        c = p->require & f & t;
        c &= NAI_EV_FEAT_FDMASK;
        if (c == 0) {
            continue;
        };

        r = p;
        break;
    };

    if (r == 0) {
        nai_errno = ENOTSUP;
    };

    return r;
};


nai_int_t nai_dgram_open(nai_dgram_t* s, nai_evloop_t* l)
{
    nai_int_t r;
    nai_int_t type;
    nai_dgram_ops_t* ops;


    if (s->st.ops) {
        /* already opened, we can reopen pseudo iobase */
        if (!(s->st.ops->require & NAI_IOBASE_PSEUDO)) {
            nai_errno = EALREADY;
            r = -1;
            goto _end;
        };
    };
    if (nai_evnode_get_fd(&s->ev) == NAI_FD_INVALID) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    /* no event loop, set blocking  */
    if (l == 0) {
        s->st.blocking = 1;
    };

    type = nai_evnode_get_type(&s->ev);
    ops = nai_dgram_ops_find(type, s->st.polling, l);
    if (ops == 0) {
        r = -1;
        goto _end;
    };

    s->st.type = NAI_IO_TYPE_DGRAM;
    s->st.ops = (nai_iobase_ops_t*)ops;
    r = s->st.ops->open(s, l);
    if (r < 0) {
        s->st.ops = 0;
        goto _end;
    };


    r = 0;

_end:
    return r;
};


nai_int_t nai_dgram_bind(nai_dgram_t* s, 
    nai_evloop_t* l, const nai_sockaddr_t* name, nai_int_t namelen)
{
    nai_int_t r;
    nai_int_t type;
    nai_dgram_ops_t* ops;


    if (s->st.ops) {
        /* already opened, we can reopen pseudo iobase */
        if (!(s->st.ops->require & NAI_IOBASE_PSEUDO)) {
            nai_errno = EALREADY;
            r = -1;
            goto _end;
        };
    };
    if (nai_evnode_get_fd(&s->ev) == NAI_FD_INVALID) {
        type = NAI_FD_TYPE_SOCK;
    } else {
        type = nai_evnode_get_type(&s->ev);
        if (type != NAI_FD_TYPE_SOCK) {
            nai_errno = ENOTSUP;
            r = -1;
            goto _end;
        };
    };

    /* no event loop, set blocking  */
    if (l == 0) {
        s->st.blocking = 1;
    };

    ops = nai_dgram_ops_find(type, s->st.polling, l);
    if (ops == 0) {
        r = -1;
        goto _end;
    };

    s->st.type = NAI_IO_TYPE_DGRAM;
    s->st.ops = (nai_iobase_ops_t*)ops;
    r = ops->bind(s, l, name, namelen);
    if (r < 0) {
        s->st.ops = 0;
        goto _end;
    };

_end:
    return r;
};


nai_int_t nai_dgram_connect(nai_dgram_t* s, 
    nai_evloop_t* l, const nai_sockaddr_t* name, nai_int_t namelen)
{
    nai_int_t r;
    nai_int_t type;
    nai_dgram_ops_t* ops;


    if (s->st.ops) {
        /* already opened, we can reopen pseudo iobase */
        if (!(s->st.ops->require & NAI_IOBASE_PSEUDO)) {
            nai_errno = EALREADY;
            r = -1;
            goto _end;
        };
    };
    if (nai_evnode_get_fd(&s->ev) == NAI_FD_INVALID) {
        type = NAI_FD_TYPE_SOCK;
    } else {
        type = nai_evnode_get_type(&s->ev);
        if (type != NAI_FD_TYPE_SOCK) {
            nai_errno = ENOTSUP;
            r = -1;
            goto _end;
        };
    };

    /* no event loop, set blocking  */
    if (l == 0) {
        s->st.blocking = 1;
    };

    ops = nai_dgram_ops_find(type, s->st.polling, l);
    if (ops == 0) {
        r = -1;
        goto _end;
    };

    s->st.type = NAI_IO_TYPE_DGRAM;
    s->st.ops = (nai_iobase_ops_t*)ops;
    r = ops->connect(s, l, name, namelen);
    if (r < 0) {
        s->st.ops = 0;
        goto _end;
    };

_end:
    return r;
};


nai_int_t nai_dgram_bypass(nai_dgram_t* s, 
    nai_dgram_t* l, const nai_sockaddr_t* name, nai_int_t namelen)
{
    nai_int_t r;
    nai_int_t ec;
    nai_int_t to;
    nai_int_t own;
    nai_fd_t fo;
    nai_fd_t fd;
    nai_socknbuf_t nlocal;


    if (s->st.ops) {
        nai_errno = EALREADY;
        r = -1;
        goto _end;
    };

    nlocal.len = sizeof(nlocal.addr);
    r = nai_sock_get_sockname(
        nai_iobase_get_fd(l), &nlocal.addr, &nlocal.len);
    if (r < 0) {
        goto _end;
    };

    fd = nai_sock_open(nlocal.addr.sa_family, SOCK_DGRAM, 0);
    if (fd == NAI_FD_INVALID) {
        r = -1;
        goto _end;
    };

    fo = nai_dgram_get_fd(s);
    to = nai_dgram_get_type(s);
    own = nai_dgram_get_fdown(s);

    nai_dgram_set_fdown(s, 1);
    nai_dgram_set_fd(s, fd, NAI_FD_TYPE_SOCK);


    r = nai_dgram_reuse_port(s, 1);
    if (r < 0) {
        goto _fail;
    };

    r = nai_sock_bind(fd, &nlocal.addr, nlocal.len);
    if (r < 0) {
        goto _fail;
    };

    r = nai_sock_connect(fd, name, namelen);
    if (r < 0) {
        goto _fail;
    };

    r = nai_dgram_open(s, nai_dgram_get_loop(l));
    if (r < 0) {
        goto _fail;
    };

    if (own && fo != NAI_FD_INVALID) {
        switch (to) {
        case NAI_FD_TYPE_FILE:
        case NAI_FD_TYPE_PIPE:
        case NAI_FD_TYPE_DEVC:
            nai_file_close(fo);
            break;
        case NAI_FD_TYPE_SOCK:
            nai_sock_close(fo);
            break;
        default:
            break;
        };
    };


_end:
    return r;

_fail:
    ec = nai_errno;
    nai_dgram_set_fdown(s, own);
    nai_dgram_set_fd(s, fo, to);
    nai_sock_close(fd);
    nai_errno = ec;
    goto _end;
};


nai_int_t nai_dgram_reuse_port(nai_dgram_t* s, nai_int_t connect)
{
    nai_int_t r;


    r = nai_dgram_set_opt(s, NAI_IO_REUSEADDR, 1);
    if (r < 0) {
        goto _end;
    };

#if (__linux__)
    if (!connect) {
#endif
        r = nai_dgram_set_opt(s, NAI_IO_REUSEPORT, 1);
        if (r < 0) {
            goto _end;
        };

#if (__linux__)
    };
#else
    (void)connect;
#endif

_end:
    return r;
};


