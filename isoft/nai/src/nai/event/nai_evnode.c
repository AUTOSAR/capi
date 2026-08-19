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
/// @file       nai_evnode.c
/// @brief      
/// @details
/// @date       2020-11-28
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai_evloop.h"
#include "nai/os/nai_file.h"
#include "nai/runtime/nai_errno.h"



nai_int_t nai_evnode_init(nai_evnode_t* h)
{
    h->loop = 0;
    h->cb = 0;
    h->fd = NAI_FD_INVALID;
    h->type = 0;
    h->extend = 0;
    h->ent = 0;
    h->st.seted = 0;
    h->st.sigbits = 0;
    h->st.except = 0;
    h->st.ttype = 0;
    h->st.catching = 0;
    h->st.priority = 0;
    h->key = 0;
    h->timeval = 0;
    return 0;
};


nai_int_t nai_evnode_set_cb(nai_evnode_t* h, nai_evnode_cb_f cb)
{
    h->cb = cb;
    return 0;
};


nai_int_t nai_evnode_set_fd(
    nai_evnode_t* h, nai_fd_t fd, nai_int_t type)
{
    nai_int_t r;
    nai_evloop_ent_t* e;


    e = h->ent;
    if (e && e->key != -1) {
        nai_errno = EPERM;
        r = -1;
        goto _end;
    };

    h->fd = fd;
    h->type = type;
    r = 0;

_end:
    return r;
};


nai_int_t nai_evnode_set_event(
    nai_evnode_t* h, nai_int_t op, nai_int_t events)
{
    nai_int_t r;
    nai_int_t ev;


    switch (op) {
    case NAI_EV_ADD:
        ev = h->st.seted;
        ev |= events;
        break;
    case NAI_EV_DEL:
        ev = h->st.seted;
        ev &= ~events;
        break;
    case NAI_EV_SET:
    default:
        ev = events;
        break;
    };

    if (h->ent == 0 || 
        h->ent->key == -1) {
        ev &= NAI_EV_IOE|NAI_EV_ASYNC;
        h->st.seted = ev | (h->st.seted & ~(NAI_EV_IOE|NAI_EV_ASYNC));
        r = 0;
    } else {
        r = nai_evloop_set_handle(h->loop, h, ev);
    };

    return r;
};


nai_int_t nai_evnode_set_catching(nai_evnode_t* h, nai_int_t on)
{
    nai_int_t r;
    nai_int_t val;


    val = !!on;
    if (h->ent == 0) {
        h->st.catching = val;
        r = 0;
    } else {
        if (!nai_evloop_in_dispatch(h->loop)) {
            nai_errno = EPERM;
            r = -1;
        } else {
            h->st.catching = val;
            h->ent->catching = val;
            r = 0;
        };
    };

    return r;
};


nai_int_t nai_evnode_set_priority(nai_evnode_t* h, nai_int_t pri)
{
    nai_int_t r;
    nai_int_t val;


    if (pri < 0 || pri > 1) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    val = pri;
    if (h->ent == 0) {
        h->st.priority = val;
        r = 0;
    } else {
        if (!nai_evloop_in_dispatch(h->loop)) {
            nai_errno = EPERM;
            r = -1;
        } else {
            h->st.priority = val;
            h->ent->priority = val;
            r = 0;
        };
    };

_end:
    return r;
};


nai_int_t nai_evnode_set_expire(nai_evnode_t* h, uint64_t msec)
{
    nai_int_t r;
    int64_t usec;


    usec = msec * 1000;

    if (h->ent == 0) {
        h->timeval = usec;
        r = 0;
    } else {
        r = nai_evloop_set_expire(h->loop, h, usec);
    };

    return r;
};


nai_int_t nai_evnode_set_timeout(nai_evnode_t* h, nai_int_t op, int32_t msec)
{
    nai_int_t r;
    int64_t usec;
    uint64_t expire;


    if (msec < -1) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    if (h->ent == 0) {
        if (msec == -1) {
            h->timeval = 0;
        } else if (op == NAI_TIMEOP_ADD && h->timeval) {
            usec = (int64_t)msec * 1000;
            h->timeval += usec;
        } else {
            usec = (int64_t)msec * 1000;
            if (h->st.ttype) {
                expire = nai_tick_to_usec() + usec;
            } else {
                nai_timecache_access();
                expire = nai_timecache_get_mtick() * 1000 + usec;
            };
            switch (op) {
            case NAI_TIMEOP_SET:
            case NAI_TIMEOP_ADD:
                break;
            case NAI_TIMEOP_MIN:
                if (h->timeval && h->timeval <= expire) {
                    r = 1;
                    goto _end;
                };
                break;
            case NAI_TIMEOP_MAX:
                if (h->timeval && h->timeval >= expire) {
                    r = 1;
                    goto _end;
                };
                break;
            default:
                nai_errno = EINVAL;
                r = -1;
                goto _end;
            };

            h->timeval = expire;
        };
        r = 0;
    } else {
        if (msec == -1) {
            usec = -1;
        } else {
            usec = (int64_t)msec * 1000;
        }
        r = nai_evloop_set_timeout(h->loop, h, op, usec);
    };

_end:
    return r;
};


nai_int_t nai_evnode_post_impl(nai_evnode_t* h, nai_int_t sigid)
{
    nai_int_t r;


    if (h->loop == 0) {
        h->st.sigbits |= (1<<sigid);
        r = 0;
    } else {
        r = nai_evloop_set_signal(h->loop, h, (NAI_EV_SIG_NOTIFY+sigid));
    };

    return r;
};


nai_int_t nai_evnode_post(nai_evnode_t* h, nai_int_t sigid)
{
    nai_int_t r;


    if (sigid < 0 || sigid >= 4) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    if (h->loop == 0) {
        h->st.sigbits |= (1<<sigid);
        r = 0;
    } else {
        r = nai_evloop_set_signal(h->loop, h, (NAI_EV_SIG_NOTIFY+sigid));
    };

_end:
    return r;
};


nai_int_t nai_evnode_join(nai_evnode_t* h, nai_int_t slot, nai_int_t sigid)
{
    nai_int_t r;


    if (h->ent == 0) {
        nai_errno = NAI_ECLOSED;
        r = -1;
        goto _end;
    };

    if (slot < 0) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    if (sigid < 0) {
        r = nai_evloop_part_slot(h->loop, h, slot);
    } else if (sigid < 4) {
        r = nai_evloop_join_slot(h->loop, h, slot, (NAI_EV_SIG_NOTIFY+sigid));
    } else {
        nai_errno = EINVAL;
        r = -1;
    };

_end:
    return r;
};


nai_int_t nai_evnode_open(nai_evnode_t* h, nai_evloop_t* l)
{
    nai_int_t r;


    if (l == 0) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    r = nai_evloop_add_handle(l, h);

_end:
    return r;
};



extern nai_int_t nai_signal_ext_close(nai_evnode_t* e);
extern nai_int_t nai_watch_ext_close(nai_evnode_t* e);
extern nai_int_t nai_timer_ext_close(nai_evnode_t* e);


static nai_evnode_ext_close nai_evnode_exts[] = {
    0, 
    nai_signal_ext_close, 
    nai_watch_ext_close, 
    nai_timer_ext_close, 
};


nai_int_t nai_evnode_close(nai_evnode_t* h)
{
    nai_int_t r;
    nai_evloop_t* l;


    switch (h->extend) {
    case NAI_EV_EXTEND_NONE:
        l = h->loop;
        if (l != 0) {
            r = nai_evloop_del_handle(l, h);
        } else {
            r = 0;
        };
        break;

    default:
        if (h->extend < nai_countof(nai_evnode_exts)) {
            r = nai_evnode_exts[h->extend](h);
        } else {
            nai_errno = EINVAL;
            r = -1;
        };
        break;
    };

    return r;
};


