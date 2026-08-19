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
/// @file       nai_https.c
/// @brief      
/// @details
/// @date       2021-01-16
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_log.h"
#include "nai/service/nai_command.h"
#include "nai/service/nai_main.h"
#include "nai/service/nai_listening.h"
#include "nai_https_core.h"



//////////////////////////////////////////////////////////////////////////////
// https module


static nai_sult_t nai_https_segment(
    nai_command_t* c, nai_command_args_t* args);


static nai_sult_t nai_https_initialize(nai_main_t* m);
static nai_sult_t nai_https_prepare(nai_main_t* m);
static nai_sult_t nai_https_start(nai_main_t* m);
static nai_sult_t nai_https_stop(nai_main_t* m);
static nai_sult_t nai_https_cleanup(nai_main_t* m);


static nai_command_t nai_https_commands[] = {
    { "https", nai_https_segment, 
      NAI_CLOC_MAIN|NAI_COPT_SEGEMENT|NAI_COPT_ARG0, 0, 0 },
    { 0 }
};


nai_module_t nai_https_module = {
    NAI_MODULE_HEADER,
    NAI_MODULE_MAIN, 
    nai_https_commands, 
    "https", 0, {
        nai_https_initialize, 
        nai_https_prepare, 
        nai_https_start, 
        nai_https_stop, 
        nai_https_cleanup
    },
};


static nai_sult_t nai_https_handle_modules(nai_https_t* h, nai_int_t preproc)
{
    nai_int_t n;
    char* name;
    nai_sult_t rc;
    nai_main_t* m;
    nai_module_t* e;
    nai_https_module_t * c;
    nai_https_module_handle_f handle;


    if (preproc) {
        name = "pre";
    } else {
        name = "post";
    };

    m = h->main;

    /* modules handle */
    for (n = 0; n < (intptr_t)nai_module_map_count(&m->modules); n ++) {
        e = nai_module_map_at(&m->modules, n);
        if (e->grp != NAI_MODULE_HTTPS) {
            continue;
        };

        c = (nai_https_module_t*)e->ctx;
        if (c == 0) {
            continue;
        };

        handle = preproc ? c->module_preproc : c->module_postproc;
        if (handle == 0) {
            continue;
        };

        rc = handle(h);
        if (rc < 0) {
            nai_log_error(NAI_LOG_HTTPS, nai_sult_to_errno(rc), 
                "https module '%s' %s-processing failed", e->name, name);
            goto _end;
        };
    };

    rc = 0;

_end:
    return rc;
};


static nai_sult_t nai_https_initialize(nai_main_t* m)
{
    (void)m;

    return 0;
};


static nai_sult_t nai_https_prepare(nai_main_t* m)
{
    nai_sult_t rc;
    nai_https_t* h;


    h = nai_main_local_at(m, nai_https_module);
    if (h == 0) {
        rc = 0; /* no https */
        goto _end;
    };
    if (h->prepared) {
        rc = 0;
        goto _end;
    };

    /* modules postproc handle */
    rc = nai_https_handle_modules(h, 0);
    if (rc < 0) {
        goto _end;
    };

    h->prepared = 1;
    rc = 0;

_end:
    return rc;
};


static nai_sult_t nai_https_start(nai_main_t* m)
{
    (void)m;

    return 0;
};


static nai_sult_t nai_https_stop(nai_main_t* m)
{
    nai_sult_t rc;
    nai_list_entry_t* e;
    nai_https_t* h;
    nai_https_connection_t* c;


    h = nai_main_local_at(m, nai_https_module);
    if (h == 0) {
        rc = 0; /* no https */
        goto _end;
    };

    e = h->clist.next;
    for ( ; e != &h->clist; ) {
        c = nai_containof(e, nai_https_connection_t, c.ent);
        e = e->next;
        nai_https_handle_termiate(c, "stop");
    };

    e = h->cfree.next;
    for ( ; e != &h->cfree; ) {
        c = nai_containof(e, nai_https_connection_t, c.ent);
        e = e->next;
        nai_list_entry_remove(&c->c.ent);
        nai_pool_close(c->c.pool);
    };

    h->nlist = 0;
    h->nfree = 0;
    rc = 0;

_end:
    return rc;
};


static nai_sult_t nai_https_cleanup(nai_main_t* m)
{
    (void)m;

    return 0;
};


static nai_sult_t nai_https_segment(
    nai_command_t* c, nai_command_args_t* args)
{
    nai_sult_t rc;
    nai_main_t* m;
    nai_https_t* h;
    nai_https_server_t* s;


    (void)c;

    if (args->endseg) {
        rc = 0;
        goto _end;
    };

    m = args->main;
    h = (nai_https_t*)nai_main_local_at(m, nai_https_module);
    if (h == 0) {
        rc = nai_https_service_create(m, args->pool, &h);
        if (rc < 0) {
            goto _end;
        };

        nai_main_local_at(m, nai_https_module) = h;
        nai_local_at(&h->local, nai_https_core_module) = h;


        /* modules preproc handle */
        rc = nai_https_handle_modules(h, 1);
        if (rc < 0) {
            goto _end;
        };
    };

    s = *(nai_https_server_t**)h->servers.elts;
    args->grp = NAI_MODULE_HTTPS;
    args->grp_mask = NAI_HTTPS_CLOC_MAIN;
    args->clocal = s->clocal;
    rc = 0;

_end:
    return rc;
};


nai_https_t* nai_https_from_main(nai_main_t* m)
{
    return nai_main_local_at(m, nai_https_module);
};


