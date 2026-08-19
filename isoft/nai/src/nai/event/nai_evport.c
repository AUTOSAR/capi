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
/// @file       nai_evport.c
/// @brief      
/// @details
/// @date       2021-02-12
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai_evport.h"
#include "nai/runtime/nai_errno.h"


#if (NAI_HAVE_AIO_PORT)


#if (NAI_AIO_USE_POSIX) && defined(SIGEV_KEVENT)
extern nai_evbase_ops_t nai_kqueue;
static nai_evbase_ops_t* nai_aio_port_base = &nai_kqueue;
#elif (NAI_AIO_USE_URING) && (NAI_HAVE_EVENTFD)
extern nai_evbase_ops_t nai_io_uring;
static nai_evbase_ops_t* nai_aio_port_base = &nai_io_uring;
#elif (NAI_AIO_USE_LINUX) && (NAI_HAVE_EVENTFD)
extern nai_evbase_ops_t nai_io_linux;
static nai_evbase_ops_t* nai_aio_port_base = &nai_io_linux;
#endif



static nai_int_t nai_evport_handle(nai_evport_t* p, nai_int_t event)
{
    nai_int_t r;
    nai_evloop_ent_t* list[2];


    (void)event;

    r = nai_evbase_wait(&p->back, 0);
    if (r < 0) {
        goto _end;
    };

    r = nai_evbase_fetch(&p->back, list);

_end:
    return r;
};


nai_int_t nai_evport_init(nai_evport_t* p)
{
    nai_evbase_init(&p->back);
    nai_evnode_init(&p->ev);
    return 0;
};


nai_int_t nai_evport_open(nai_evport_t* p, nai_evbase_ops_t* ops, void* e)
{
    nai_int_t r;
    nai_fd_t fd;


    /* open evbase */
    p->back.ops = nai_aio_port_base;
    p->back.ud = p->back.ops->open(NAI_EVBASE_BACKEND);
    if (p->back.ud == 0) {
        r = -1;
        goto _end;
    };

    p->ent.h = &p->ev;
    p->ent.key = -1;
    p->ent.mt = 0;
    p->ent.mail = 0;
    p->ent.catching = 0;
    p->ent.priority = 0;
    p->ent.events = 0;
    p->ent.sig = 0;
    p->ent.signext = 0;
    p->ent.ttype = 0;
    p->ent.timer.value = 0;
    p->ev.ent = &p->ent;

    fd = nai_evbase_get_fd(&p->back);
    if (fd != NAI_FD_INVALID) {
        nai_evnode_set_event(&p->ev, NAI_EV_SET, NAI_EV_READ);
        nai_evnode_set_fd(&p->ev, fd, NAI_FD_TYPE_DEVC);
        nai_evnode_set_cb(&p->ev, (nai_evnode_cb_f)nai_evport_handle);
    };

    r = ops->add(e, &p->ev, p->ev.st.seted);

_end:
    return r;
};


nai_int_t nai_evport_close(nai_evport_t* p)
{
    nai_int_t r;


    r = nai_evbase_close(&p->back);
    if (r < 0) {
        goto _end;
    };

    nai_evnode_init(&p->ev);
    r = 0;

_end:
    return r;
};


#else


nai_int_t nai_evport_init(nai_evport_t* p)
{
    p->unused = 0;
    return 0;
};


nai_int_t nai_evport_open(nai_evport_t* p, nai_evbase_ops_t* ops, void* e)
{
    (void)p;
    (void)ops;
    (void)e;

    return 0;
};


nai_int_t nai_evport_close(nai_evport_t* p)
{
    (void)p;

    return 0;
};


nai_int_t nai_evport_set(nai_evport_t* p, nai_evnode_t* h, nai_int_t events)
{
    nai_int_t r;


    (void)p;
    (void)h;

    if (events & NAI_EV_ASYNC) {
        nai_errno = ENOTSUP;
        r = -1;
        goto _end;
    };

    r = 0;

_end:
    return r;
};


#endif

