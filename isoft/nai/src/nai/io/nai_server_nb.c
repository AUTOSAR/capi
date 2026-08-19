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
/// @file       nai_server_nb.c
/// @brief      
/// @details
/// @date       2023-09-19
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai_iobase.h"


//////////////////////////////////////////////////////////////////////////////
// nonblock server


static nai_int_t nai_server_nb_open(nai_server_t* s, nai_evloop_t* l);
static nai_int_t nai_server_nb_close(nai_server_t* s);
static nai_int_t nai_server_nb_bind(
    nai_server_t* s, nai_evloop_t* l, 
    const nai_sockaddr_t* name, nai_int_t namelen);
static nai_int_t nai_server_nb_setopt(
    nai_server_t* s, nai_int_t opt, intptr_t value);
static nai_int_t nai_server_nb_getopt(
    nai_server_t* s, nai_int_t opt, intptr_t* value);
static nai_int_t nai_server_nb_shutdown(nai_server_t* s, nai_int_t how);
static nai_int_t nai_server_nb_cache(
    nai_server_t* s, nai_int_t opt, void* buf, nai_int_t len);
static nai_fd_t nai_server_nb_accept(
    nai_server_t* s, nai_sockaddr_t* name, nai_int_t* namelen);


#define NAI_NB_FEAT (NAI_EV_FEAT_EDGE|NAI_EV_FEAT_LEVEL|NAI_EV_FEAT_BLOCK)


nai_server_ops_t nai_server_sock_nb = {
    "sock-server-nb", 
    NAI_NB_FEAT|NAI_EV_FEAT_SOCK,
    nai_server_nb_open, 
    nai_server_nb_setopt,
    nai_server_nb_getopt, 
    nai_server_nb_shutdown, 
    nai_server_nb_close, 
    nai_server_nb_bind, 
    nai_server_nb_cache, 
    nai_server_nb_accept, 
};


//////////////////////////////////////////////////////////////////////////////
// open, bind, close, setopt and getopt 


typedef struct nai_iobase_nb_s nai_server_nb_t;


#define nai_server_nb_init(s)                       \
    nai_iobase_nb_init((nai_iobase_t*)s)            \


#define nai_server_nb_add_evloop(s, l)              \
    nai_iobase_nb_add_evloop((nai_iobase_t*)s, l)   \


#undef  nai_server_blocked
#define nai_server_blocked      nai_iobase_nb_blocked



static nai_int_t nai_server_nb_open(nai_server_t* s, nai_evloop_t* l)
{
    nai_evnode_set_priority(&s->ev, 1);
    return nai_iobase_nb_open(s, l, 0);
};


static nai_int_t nai_server_nb_close(nai_server_t* s)
{
    return nai_iobase_nb_close(s);
};


static nai_int_t nai_server_nb_bind(
    nai_server_t* s, nai_evloop_t* l, 
    const nai_sockaddr_t* name, nai_int_t namelen)
{
    nai_evnode_set_priority(&s->ev, 1);
    return nai_iobase_nb_bind(s, l, name, namelen, 
        nai_iobase_is_message(s) ? SOCK_SEQPACKET : SOCK_STREAM);
};


static nai_int_t nai_server_nb_getopt(
    nai_server_t* s, nai_int_t opt, intptr_t* value)
{
    return nai_iobase_nb_getopt(s, opt, value);
};


static nai_int_t nai_server_nb_setopt(
    nai_server_t* s, nai_int_t opt, intptr_t value)
{
    return nai_iobase_nb_setopt(s, opt, value);
};


static nai_int_t nai_server_nb_shutdown(nai_server_t* s, nai_int_t how)
{
    (void)s;
    (void)how;

    return 0;
};


static nai_fd_t nai_server_nb_accept(
    nai_server_t* s, nai_sockaddr_t* name, nai_int_t* namelen)
{
    nai_int_t r;
    nai_int_t ec;
    nai_fd_t a;
    nai_fd_t fd;


    if (!(s->st.mode & NAI_EV_READ)) {
        nai_errno = EACCES;
        a = NAI_FD_INVALID;
        goto _end;
    };


    fd = nai_evnode_get_fd(&s->ev);


    while (1) {
        a = nai_sock_accept(fd, name, namelen);
        if (a != NAI_FD_INVALID) {
            break;
        };

        ec = nai_errno;
        if (ec != NAI_EAGAIN || !s->st.blocking) {
            goto _fail;
        };

        /* in blocking, EAGAIN means opeartion is timedout */
        if (s->st.blockset) {
            nai_errno = ETIMEDOUT;
            goto _end;
        };

        assert(!s->st.blockset);
        assert(!s->st.timeosup);
        r = nai_server_sock_wait(s, 0);
        if (r < 0) {
            ec = nai_errno;
            goto _fail;
        };
    };


_end:
    return a;

_fail:
    if (ec == NAI_EAGAIN) {
        r = nai_server_blocked(s, NAI_EV_READ);
        if (r >= 0) {
            nai_errno = NAI_EAGAIN;
        };
    };
    a = NAI_FD_INVALID;
    goto _end;
};


static nai_int_t nai_server_nb_cache(
    nai_server_t* s, nai_int_t opt, void* buf, nai_int_t len)
{
    (void)s;
    (void)opt;
    (void)buf;
    (void)len;

    nai_errno = ENOENT;
    return -1;
};

