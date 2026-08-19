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
/// @file       nai_httpc.c
/// @brief      
/// @details
/// @date       2021-08-19
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_log.h"
#include "nai/service/nai_command.h"
#include "nai/service/nai_main.h"
#include "nai_httpc_core.h"


//////////////////////////////////////////////////////////////////////////////
// httpc module


static nai_sult_t nai_httpc_segment(
    nai_command_t* c, nai_command_args_t* args);


static nai_sult_t nai_httpc_initialize(nai_main_t* m);
static nai_sult_t nai_httpc_prepare(nai_main_t* m);
static nai_sult_t nai_httpc_start(nai_main_t* m);
static nai_sult_t nai_httpc_stop(nai_main_t* m);
static nai_sult_t nai_httpc_cleanup(nai_main_t* m);


static nai_command_t nai_httpc_commands[] = {
    { "httpc", nai_httpc_segment, 
      NAI_CLOC_MAIN|NAI_COPT_SEGEMENT|NAI_COPT_ARG0, 0, 0 },
    { 0 }
};


nai_module_t nai_httpc_module = {
    NAI_MODULE_HEADER,
    NAI_MODULE_MAIN, 
    nai_httpc_commands, 
    "httpc", 0, {
        nai_httpc_initialize, 
        nai_httpc_prepare, 
        nai_httpc_start, 
        nai_httpc_stop, 
        nai_httpc_cleanup
    },
};


static nai_sult_t nai_httpc_handle_modules(nai_httpc_t* h, nai_int_t preproc)
{
    nai_int_t n;
    char* name;
    nai_sult_t rc;
    nai_main_t* m;
    nai_module_t* e;
    nai_httpc_module_t * c;
    nai_httpc_module_handle_f handle;


    if (preproc) {
        name = "pre";
    } else {
        name = "post";
    };

    m = h->main;

    /* modules handle */
    for (n = 0; n < (intptr_t)nai_module_map_count(&m->modules); n ++) {
        e = nai_module_map_at(&m->modules, n);
        if (e->grp != NAI_MODULE_HTTPC) {
            continue;
        };

        c = (nai_httpc_module_t*)e->ctx;
        if (c == 0) {
            continue;
        };

        handle = preproc ? c->module_preproc : c->module_postproc;
        if (handle == 0) {
            continue;
        };

        rc = handle(h);
        if (rc < 0) {
            nai_log_error(NAI_LOG_HTTPC, nai_sult_to_errno(rc), 
                "httpc module '%s' %s-processing failed", e->name, name);
            goto _end;
        };
    };

    rc = 0;

_end:
    return rc;
};


static nai_sult_t nai_httpc_initialize(nai_main_t* m)
{
    (void)m;

    return 0;
};


static nai_sult_t nai_httpc_prepare(nai_main_t* m)
{
    nai_sult_t rc;
    nai_httpc_t* h;


    h = nai_main_local_at(m, nai_httpc_module);
    if (h == 0) {
        rc = 0; /* no httpc */
        goto _end;
    };
    if (h->prepared) {
        rc = 0;
        goto _end;
    };

    /* modules postproc handle */
    rc = nai_httpc_handle_modules(h, 0);
    if (rc < 0) {
        goto _end;
    };

    h->prepared = 1;
    rc = 0;

_end:
    return rc;
};


static nai_sult_t nai_httpc_start(nai_main_t* m)
{
    (void)m;

    return 0;
};


static nai_sult_t nai_httpc_stop(nai_main_t* m)
{
    nai_int_t r;
    nai_int_t i;
    nai_sult_t rc;
    nai_list_entry_t* e;
    nai_list_entry_t* n;
    nai_httpc_t* h;
    nai_httpc_agent_t* a;
    nai_httpc_connection_t* c;


    h = nai_main_local_at(m, nai_httpc_module);
    if (h == 0) {
        rc = 0; /* no https */
        goto _end;
    };

    for (i = 0; i < (nai_int_t)nai_countof(h->list); i ++) {
        n = h->list[i].next;
        for ( ; n != &h->list[i]; n = n->next) {
            a = nai_containof(n, nai_httpc_agent_t, ent);

            e = a->clist.next;
            for ( ; e != &a->clist; ) {
                c = nai_containof(e, nai_httpc_connection_t, c.ent);
                e = e->next;
                nai_httpc_handle_termiate(c, "stop");
            };

            e = a->cfree.next;
            for ( ; e != &a->cfree; ) {
                c = nai_containof(e, nai_httpc_connection_t, c.ent);
                e = e->next;
                nai_list_entry_remove(&c->c.ent);
                nai_pool_close(c->c.pool);
            };

            a->nlist = 0;
            a->nfree = 0;
            nai_rbtree_init(&a->cmap);
        };
    };

    r = nai_pool_close(&h->pool);
    if (r < 0) {
        rc = nai_sult_from_errno();
        nai_log_error(NAI_LOG_HTTPC, 
            nai_sult_to_errno(rc), "nai_pool_close() failed");
        goto _end;
    };

    rc = 0;

_end:
    return rc;
};


static nai_sult_t nai_httpc_cleanup(nai_main_t* m)
{
    (void)m;

    return 0;
};


static nai_sult_t nai_httpc_segment(
    nai_command_t* c, nai_command_args_t* args)
{
    nai_sult_t rc;
    nai_main_t* m;
    nai_httpc_t* h;
    nai_httpc_agent_t* a;
    nai_httpc_server_t* s;


    (void)c;

    if (args->endseg) {
        rc = 0;
        goto _end;
    };

    m = args->main;
    h = (nai_httpc_t*)nai_main_local_at(m, nai_httpc_module);
    if (h == 0) {
        rc = nai_httpc_service_create(m, args->pool, &h);
        if (rc < 0) {
            nai_log_error(NAI_LOG_HTTPC, 
                nai_sult_to_errno(rc), "create httpc service failed");
            goto _end;
        };

        nai_main_local_at(m, nai_httpc_module) = h;
        nai_local_at(&h->local, nai_httpc_core_module) = h;


        /* modules preproc handle */
        rc = nai_httpc_handle_modules(h, 1);
        if (rc < 0) {
            goto _end;
        };
    };

    a = nai_httpc_agent_add(m, "default", 0);
    s = *(nai_httpc_server_t**)a->servers.elts;
    args->grp = NAI_MODULE_HTTPC;
    args->grp_mask = NAI_HTTPC_CLOC_MAIN;
    args->clocal = s->clocal;
    rc = 0;

_end:
    return rc;
};


nai_httpc_t* nai_httpc_from_main(nai_main_t* m)
{
    return nai_main_local_at(m, nai_httpc_module);
};



