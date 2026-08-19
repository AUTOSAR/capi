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
/// @file       nai_server.c
/// @brief      
/// @details
/// @date       2023-09-19
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai_iobase.h"


extern nai_server_ops_t nai_server_sock_nb;
extern nai_server_ops_t nai_server_sock_aio;


static nai_server_ops_t* nai_server_ops[] = {
    &nai_server_sock_nb, 
    &nai_server_sock_aio, 
};


static nai_server_ops_t* nai_server_ops_find(
    nai_int_t type, nai_int_t polling, nai_evloop_t* l)
{
    nai_int_t n;
    nai_int_t c;
    nai_int_t t, f;
    nai_server_ops_t* p;
    nai_server_ops_t* r;


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
    for (n = 0; n < (nai_int_t)nai_countof(nai_server_ops); n ++) {
        p = nai_server_ops[n];

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


nai_int_t nai_server_open(nai_server_t* s, nai_evloop_t* l)
{
    nai_int_t r;
    nai_int_t type;
    nai_server_ops_t* ops;


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
    ops = nai_server_ops_find(type, s->st.polling, l);
    if (ops == 0) {
        r = -1;
        goto _end;
    };

    s->st.type = NAI_IO_TYPE_SERVER;
    s->st.mode &= ~NAI_IO_WRITE;
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


nai_int_t nai_server_bind(nai_server_t* s, 
    nai_evloop_t* l, const nai_sockaddr_t* name, nai_int_t namelen)
{
    nai_int_t r;
    nai_int_t type;
    nai_server_ops_t* ops;


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

    ops = nai_server_ops_find(type, s->st.polling, l);
    if (ops == 0) {
        r = -1;
        goto _end;
    };

    s->st.type = NAI_IO_TYPE_SERVER;
    s->st.mode &= ~NAI_IO_WRITE;
    s->st.ops = (nai_iobase_ops_t*)ops;
    r = ops->bind(s, l, name, namelen);
    if (r < 0) {
        s->st.ops = 0;
        goto _end;
    };

_end:
    return r;
};


nai_fd_t nai_server_accept(
    nai_server_t* s, nai_sockaddr_t* name, nai_int_t* namelen)
{
    nai_fd_t r;
    nai_server_ops_t* ops;


    ops = (nai_server_ops_t*)s->st.ops;
    if (ops == 0) {
        nai_errno = NAI_ECLOSED;
        r = NAI_FD_INVALID;
        goto _end;
    };
    if (s->st.type != NAI_IO_TYPE_SERVER) {
        nai_errno = ENOTSUP;
        r = NAI_FD_INVALID;
        goto _end;
    };

    r = ops->accept(s, name, namelen);

_end:
    return r;
};


nai_int_t nai_server_cache(
    nai_server_t* s, nai_int_t opt, void* buf, nai_int_t len)
{
    nai_int_t r;
    nai_server_ops_t* ops;


    ops = (nai_server_ops_t*)s->st.ops;
    if (ops == 0) {
        nai_errno = NAI_ECLOSED;
        r = -1;
        goto _end;
    };
    if (s->st.type != NAI_IO_TYPE_SERVER) {
        nai_errno = ENOTSUP;
        r = -1;
        goto _end;
    };

    r = ops->cache(s, opt, buf, len);

_end:
    return r;
};


