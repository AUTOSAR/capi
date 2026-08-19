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
/// @file       nai_module.c
/// @brief      
/// @details
/// @date       2020-11-28
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai/service/nai_module.h"
#include "nai/service/nai_main.h"
#include "nai/service/nai_command.h"
#include "nai/runtime/nai_log.h"
#include "nai/runtime/nai_util.h"
#include "nai/runtime/nai_errno.h"
#include "nai/os/nai_thread.h"


extern nai_int_t nai_modules_max;
extern nai_module_t* nai_modules[];


nai_atomic32_t nai_module_index;


nai_int_t nai_module_map_init(nai_module_map_t* l)
{
    nai_int_t r;


    nai_array_init(&l->a, sizeof(nai_module_t*), 0);
    l->main = 0;
    l->last_grp = 0;
    l->last_index = 0;
    r = 0;

    return r;
};


nai_int_t nai_module_map_open(nai_module_map_t* l, nai_main_t* m)
{
    nai_int_t r;


    if (l->main) {
        nai_errno = EPERM;
        r = -1;
        goto _end;
    };

    assert(m != 0);

    r = nai_array_init(&l->a, sizeof(nai_module_t*), &m->pool);
    if (r < 0) {
        goto _end;
    };

    r = nai_array_reserve(&l->a, nai_modules_max/2);
    if (r < 0) {
        goto _end;
    };

    l->main = m;

_end:
    return r;
};


nai_int_t nai_module_map_push(nai_module_map_t* l, nai_module_t* m)
{
    nai_int_t r;
    nai_int_t index;
    nai_slot_t* s;
    nai_module_t* n;
    nai_module_t** c;
    nai_module_t** cend;


    if (l->main == 0) {
        nai_errno = EPERM;
        r = -1;
        goto _end;
    };

    if (m->index == 0) {
        index = nai_atomic32_add(&nai_module_index, 1);
        if (index > nai_modules_max) {
            nai_log_alert(NAI_LOG_CORE, nai_errno, "too many module_map");
            r = -1;
            goto _end;
        };
        m->index = index;
    };

    c = (nai_module_t**)nai_array_push(&l->a);
    if (c == 0) {
        nai_log_alert(NAI_LOG_CORE, nai_errno, "nai_array_push failed");
        r = -1;
        goto _end;
    };
    c[0] = m;


    s = l->main->local->slot[0];
    if (m->grp == NAI_MODULE_MAIN) {
        s[m->index] = m->index;
    } else {
        /* assign group slot */
        if (m->grp != l->last_grp) {
            l->last_grp = m->grp;
            l->last_index = 0;

            c = (nai_module_t**)l->a.elts;
            cend = c + l->a.count;
            for ( ; c < cend; c ++) {
                n = c[0];
                if (n->grp != m->grp) {
                    continue;
                };
                if (l->last_index < s[n->index]) {
                    l->last_index = s[n->index];
                };
            };
        };

        l->last_grp = m->grp;
        l->last_index += 1;
        s[m->index] = l->last_index;
    };

    r = 0;

_end:
    return r;
};


static const char* nai_hookname[] = {
    "init", "prepare", 
    "start", "stop", 
    "cleanup"
};


nai_int_t nai_module_map_on(nai_module_map_t* l, nai_int_t index)
{
    nai_int_t r;
    nai_int_t n;
    nai_int_t ec;
    nai_sult_t rc;
    nai_module_t* m;
    nai_int_t (**hook)(nai_main_t*);


    if (index < 0 || index >= (nai_int_t)nai_countof(nai_hookname)) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    for (n = 0; n < (intptr_t)nai_module_map_count(l); n ++) {
        m = nai_module_map_at(l, n);

        hook = &m->init;
        if (hook[index] == 0) {
            continue;
        };

        rc = hook[index](l->main);
        if (rc < 0) {
            ec = nai_sult_to_errno(rc);
            nai_log_alert(NAI_LOG_CORE, 
                ec ,"%s module '%s' failed", nai_hookname[index], m->name);

            nai_errno = ec;
            r = -1;
            goto _end;
        };
    };

    r = 0;

_end:
    return r;
};


nai_int_t nai_module_map_count_grp(nai_module_map_t* l, nai_int_t grp)
{
    nai_int_t r;
    nai_int_t n;
    nai_module_t* m;


    r = 0;
    for (n = 0; n < (intptr_t)nai_module_map_count(l); n ++) {
        m = nai_module_map_at(l, n);

        if (m->grp == grp) {
            r ++;
        };
    };

    return r;
};


nai_module_t* nai_module_map_find(
    nai_module_map_t* l, const char* name)
{
    nai_module_t* r = 0;
    nai_module_t* m;
    nai_module_t** c;
    nai_module_t** cend;


    c = (nai_module_t**)l->a.elts;
    cend = c + l->a.count;
    for ( ; c < cend; c ++) {
        m = c[0];
        if (nai_strcasecmp(name, m->name) == 0) {
            r = m;
            continue;
        };
    };

    if (r == 0) {
        nai_errno = ENOENT;
    };
    return r;
};


nai_command_t* nai_module_map_command(nai_module_map_t* l, 
    nai_int_t grp, uint32_t grp_mask, const char* name, nai_module_t** p)
{
    nai_command_t* r = 0;
    nai_command_t* e;
    nai_module_t* m;
    nai_module_t** c;
    nai_module_t** cend;


    c = (nai_module_t**)l->a.elts;
    cend = c + l->a.count;
    for ( ; c < cend; c ++) {
        m = c[0];
        if (m->grp != grp) {
            continue;
        };

        e = m->commands;
        for ( ; e->name; e ++) {
            if (nai_strcasecmp(name, e->name) == 0) {
                break;
            };
        };
        if (e->name) {
            if (p) {
                p[0] = m;
            };
            r = e;

            /* match group mask */
            if (e->flags & grp_mask) {
                break;
            };
        };
    };

    if (r == 0) {
        nai_errno = ENOENT;
    };
    return r;
};


nai_int_t nai_module_map_close(nai_module_map_t* l)
{
    nai_array_close(&l->a);
    l->last_grp = 0;
    l->last_index = 0;
    l->main = 0;

    return 0;
};

