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
/// @file       nai_watch.c
/// @brief      
/// @details
/// @date       2023-10-17
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai_evloop.h"
#include "nai/io/nai_watch.h"
#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_util.h"



nai_int_t nai_watch_open(nai_watch_t* w, nai_evloop_t* l, nai_int_t stage)
{
    nai_int_t r;


    if (l == 0) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    r = nai_evloop_add_watch(l, w, stage);

_end:
    return r;
};


nai_int_t nai_watch_ext_close(nai_watch_t* w)
{
    nai_int_t r;
    nai_evloop_t* l;


    assert(w->extend == NAI_EV_EXTEND_WATCH);

    l = nai_evnode_get_loop(w);
    r = nai_evloop_del_watch(l, w);

    return r;
};


