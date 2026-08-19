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
/// @file       nai_main_event.c
/// @brief      
/// @details
/// @date       2020-11-28
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai/service/nai_main.h"
#include "nai/service/nai_main_event.h"
#include "nai/service/nai_main_task.h"
#include "nai/service/nai_command.h"
#include "nai/runtime/nai_log.h"
#include "nai/runtime/nai_errno.h"
#include "nai/os/nai_thread.h"



static nai_sult_t nai_main_event_init(nai_main_t* m);
static nai_sult_t nai_main_event_prepare(nai_main_t* m);
static nai_sult_t nai_main_event_start(nai_main_t* m);
static nai_sult_t nai_main_event_stop(nai_main_t* m);
static nai_sult_t nai_main_event_cleanup(nai_main_t* m);


static nai_sult_t nai_main_event_segment(
    nai_command_t* c, nai_command_args_t* args);
static nai_sult_t nai_main_event_use(
    nai_command_t* c, nai_command_args_t* args);
static nai_sult_t nai_main_event_max_connections(
    nai_command_t* c, nai_command_args_t* args);


static nai_command_optinfo_t nai_event_features[] = {
    { nai_strconst("level"), NAI_EV_FEAT_LEVEL }, 
    { nai_strconst("edge"),  NAI_EV_FEAT_EDGE },
    { nai_strconst("async"), NAI_EV_FEAT_ASYNC }, 
    { nai_strconst("block"), NAI_EV_FEAT_BLOCK }, 
    { nai_strconst("file"),  NAI_EV_FEAT_FILE }, 
    { nai_strconst("pipe"),  NAI_EV_FEAT_PIPE }, 
    { nai_strconst("sock"),  NAI_EV_FEAT_SOCK }, 
    { nai_strnull(), 0 }
};


static nai_command_t nai_main_event_commands[] = {
    { "event", nai_main_event_segment, 
        NAI_CLOC_MAIN|NAI_COPT_SEGEMENT|NAI_COPT_ARG0, 0, 0 },
    { "use", nai_main_event_use, 
        NAI_CLOC_EVENT|NAI_COPT_ARG1, 
        nai_offsetof(nai_main_event_t, name), 0 },
    { "required", nai_command_set_opts, 
        NAI_CLOC_EVENT|NAI_COPT_ARG1MORE, 
        nai_offsetof(nai_main_event_t, required), nai_event_features },
    { "optional", nai_command_set_opts, 
        NAI_CLOC_EVENT|NAI_COPT_ARG1MORE,
        nai_offsetof(nai_main_event_t, optional), nai_event_features },
    { "timeval", nai_command_set_msec, 
        NAI_CLOC_EVENT|NAI_COPT_ARG1, 
        nai_offsetof(nai_main_event_t, timeval), 0 },
    { "io_task_pool", nai_main_task_pool_command, 
        NAI_CLOC_EVENT|NAI_COPT_ARG1, 
        nai_offsetof(nai_main_event_t, task_pool), 0 },
    { "max_connections", nai_main_event_max_connections, 
        NAI_CLOC_EVENT|NAI_COPT_ARG1, 0, 0 },
    { 0 }
};


nai_module_t nai_main_event_module = {
    NAI_MODULE_HEADER, 
    NAI_MODULE_MAIN, 
    nai_main_event_commands, 
    "event", 0, {
        nai_main_event_init, 
        nai_main_event_prepare, 
        nai_main_event_start, 
        nai_main_event_stop, 
        nai_main_event_cleanup 
    }
};


nai_main_event_t* nai_main_event_get(nai_main_t* m)
{
    return nai_main_local_at(m, nai_main_event_module);
};


nai_int_t nai_main_event_inc_connection(nai_main_event_t* e)
{
    nai_atomic_inc(&e->connections);
    return 0;
};


nai_int_t nai_main_event_dec_connection(nai_main_event_t* e)
{
    nai_atomic_dec(&e->connections);
    return 0;
};


static nai_sult_t nai_main_event_init(nai_main_t* m)
{
    nai_sult_t rc;
    nai_main_event_t* e;


    e = (nai_main_event_t*)nai_palloc(&m->pool, sizeof(*e));
    if (e == 0) {
        rc = nai_sult_from_errno();
        nai_log_alert(NAI_LOG_CORE, 
            nai_sult_to_errno(rc), "alloc nai_main_event_t failed");
        goto _end;
    };

    nai_listening_map_init(&e->ls, &m->pool);
    nai_str_setc(&e->name, "");
    e->loop = 0;
    e->task_pool = 0;
    e->optional = 0;
    e->required = 0;
    e->timeval = -1;
    e->max_connections = 1000;
    e->connections = 0;
    e->flags = 0;

    nai_main_local_at(m, nai_main_event_module) = e;
    rc = 0;

_end:
    return rc;
};


static nai_sult_t nai_main_event_prepare(nai_main_t* m)
{
    nai_int_t r;
    nai_sult_t rc;
    nai_main_event_t* e;


    e = (nai_main_event_t*)nai_main_local_at(m, nai_main_event_module);
    if (e->loop) {
        rc = 0;         /* user provide the event loop */
        goto _end;
    };

    e->loop = nai_evloop_new();
    if (e->loop == 0) {
        rc = nai_sult_from_errno();
        nai_log_alert(NAI_LOG_CORE, 
            nai_sult_to_errno(rc), "nai_evloop_new failed");
        goto _end;
    };

    /* open the event loop */
    r = nai_evloop_open(e->loop, e->optional, e->required, e->name.ptr);
    if (r < 0) {
        rc = nai_sult_from_errno();
        nai_log_alert(NAI_LOG_CORE, 
            nai_sult_to_errno(rc), "nai_evloop_open failed");

        nai_evloop_close(e->loop);
        e->loop = 0;
        goto _end;
    };

    e->own = 1;
    rc = 0;

_end:
    return rc;
};


static nai_sult_t nai_main_event_start(nai_main_t* m)
{
    nai_int_t r;
    nai_sult_t rc;
    nai_main_event_t* e;


    e = (nai_main_event_t*)nai_main_local_at(m, nai_main_event_module);
    r = nai_listening_map_start(&e->ls, e->loop);
    if (r < 0) {
        rc = nai_sult_from_errno();
        nai_log_alert(NAI_LOG_CORE, 
            nai_sult_to_errno(rc), "nai_servers_strat failed");
        goto _end;
    };

    /* set the io threads of event loop */
    if (e->task_pool != 0) {
        r = nai_evloop_set_io_threads(e->loop, e->task_pool);
        if (r < 0) {
            rc = nai_sult_from_errno();
            nai_log_alert(NAI_LOG_CORE, 
                nai_sult_to_errno(rc), "nai_evloop_set_io_threads failed");
            goto _end;
        };
    };

    /* set the time interval of event loop */
    if (e->timeval != -1) {
        nai_evloop_set_timeval(e->loop, e->timeval);
    };

    rc = 0;

_end:
    return rc;
};


static nai_sult_t nai_main_event_stop(nai_main_t* m)
{
    nai_int_t r;
    nai_sult_t rc;
    nai_main_event_t* e;


    e = (nai_main_event_t*)nai_main_local_at(m, nai_main_event_module);
    if (e->loop) {
        if (e->task_pool) {
            r = nai_evloop_set_io_threads(e->loop, 0);
            if (r < 0) {
                rc = nai_sult_from_errno();
                nai_log_alert(NAI_LOG_CORE, 
                    nai_sult_to_errno(rc), "nai_evloop_set_io_threads failed");
                goto _end;
            };
        };

        r = nai_listening_map_stop(&e->ls);
        if (r < 0) {
            rc = nai_sult_from_errno();
            nai_log_alert(NAI_LOG_CORE, 
                nai_sult_to_errno(rc), "nai_listening_map_stop failed");
            goto _end;
        };
    };

    rc = 0;

_end:
    return rc;
};


static nai_sult_t nai_main_event_cleanup(nai_main_t* m)
{
    nai_int_t r;
    nai_sult_t rc;
    nai_main_event_t* e;


    e = (nai_main_event_t*)nai_main_local_at(m, nai_main_event_module);
    r = nai_listening_map_close(&e->ls);
    if (r < 0) {
        rc = nai_sult_from_errno();
        nai_log_alert(NAI_LOG_CORE, 
            nai_sult_to_errno(rc), "nai_listening_map_close failed");
        goto _end;
    };

    if (e->loop) {
        if (e->own) {
            r = nai_evloop_close(e->loop);
            if (r < 0) {
                rc = nai_sult_from_errno();
                nai_log_alert(NAI_LOG_CORE, 
                    nai_sult_to_errno(rc), "nai_evloop_close failed");
                goto _end;
            };

            e->own = 0;
            e->loop = 0;
        };
    };

    rc = 0;

_end:
    return rc;
};


static nai_sult_t nai_main_event_segment(
    nai_command_t* c, nai_command_args_t* args)
{
    (void)c;

    if (args->endseg == 0) {
        args->grp_mask = NAI_CLOC_EVENT;
    };
    return 0;
};


static nai_sult_t nai_main_event_use(
    nai_command_t* c, nai_command_args_t* args)
{
    nai_sult_t rc;

    rc = nai_command_set_string(c, args);
    if (rc < 0) {
        goto _end;
    };

    rc = 0;

_end:
    return rc;
};


static nai_sult_t nai_main_event_max_connections(
    nai_command_t* c, nai_command_args_t* args)
{
    nai_int_t connections;
    nai_sult_t rc;
    nai_main_event_t* e;


    (void)c;

    rc = nai_value_geti(&args->argv[0], &connections);
    if (rc < 0) {
        nai_log_error(NAI_LOG_CORE, 
            nai_sult_to_errno(rc), "get argument failed");
        goto _end;
    };

    if (connections <= 0) {
        rc = nai_errno_to_sult(EINVAL);
        nai_log_error(NAI_LOG_CORE, 
            nai_sult_to_errno(rc), 
            "max_connections shuold be greater than zero");

        goto _end;
    };

    e = (nai_main_event_t*)args->data;
    e->max_connections = connections;
    rc = 0;

_end:
    return rc;
};

