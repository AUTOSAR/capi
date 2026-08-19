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
/// @file       nai_timer.c
/// @brief      
/// @details
/// @date       2023-03-24
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai_evloop.h"
#include "nai/io/nai_timer.h"
#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_util.h"



nai_int_t nai_timer_set_expire(nai_timer_t* t, uint64_t usec)
{
    nai_int_t r;


    if (t->ent == 0) {
        t->timeval = usec;
        r = 0;
    } else {
        r = nai_evloop_set_expire(t->loop, t, usec);
    };

    return r;
};


nai_int_t nai_timer_set_timeout(nai_timer_t* t, nai_int_t op, int64_t usec)
{
    nai_int_t r;
    uint64_t expire;


    if (usec < -1) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    if (t->ent == 0) {
        if (usec == -1) {
            t->timeval = 0;
        } else if (op == NAI_TIMEOP_ADD && t->timeval) {
            t->timeval += usec;
        } else {
            nai_timecache_update_i();
            expire = nai_timecache_get_utick() + usec;
            switch (op) {
            case NAI_TIMEOP_SET:
            case NAI_TIMEOP_ADD:
                break;
            case NAI_TIMEOP_MIN:
                if (t->timeval && t->timeval <= expire) {
                    r = 1;
                    goto _end;
                };
                break;
            case NAI_TIMEOP_MAX:
                if (t->timeval && t->timeval >= expire) {
                    r = 1;
                    goto _end;
                };
                break;
            default:
                nai_errno = EINVAL;
                r = -1;
                goto _end;
            };

            t->timeval = expire;
        };
        r = 0;
    } else {
        r = nai_evloop_set_timeout(t->loop, t, op, usec);
    };

_end:
    return r;
};


nai_int_t nai_timer_open(nai_timer_t* t, nai_evloop_t* l)
{
    nai_int_t r;


    if (l == 0) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    if (nai_timer_is_opened(t)) {
        nai_errno = EEXIST;
        r = -1;
        goto _end;
    };

    t->extend = NAI_EV_EXTEND_TIMER;
    t->st.ttype = 1;
    r = nai_evloop_add_handle(l, t);

_end:
    return r;
};


nai_int_t nai_timer_ext_close(nai_timer_t* t)
{
    nai_int_t r;
    nai_evloop_t* l;


    assert(t->extend == NAI_EV_EXTEND_TIMER);

    l = nai_evnode_get_loop(t);
    r = nai_evloop_del_handle(l, t);
    if (r < 0) {
        goto _end;
    };

    t->extend = 0;
    t->st.ttype = 0;
    r = 0;

_end:
    return r;
};


