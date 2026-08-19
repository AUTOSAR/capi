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
/// @file       nai_main_task.c
/// @brief      
/// @details
/// @date       2021-11-01
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai/service/nai_main.h"
#include "nai/service/nai_main_task.h"
#include "nai/service/nai_command.h"
#include "nai/os/nai_task.h"
#include "nai/runtime/nai_string.h"
#include "nai/runtime/nai_list.h"
#include "nai/runtime/nai_log.h"
#include "nai/runtime/nai_errno.h"



typedef struct nai_main_task_ctx_s {
    nai_list_entry_t list;
} nai_main_task_ctx_t;


#ifndef _NAI_TYPEDEF_MAIN_TASKPOOL_T
#define _NAI_TYPEDEF_MAIN_TASKPOOL_T
typedef struct nai_main_task_pool_s nai_main_task_pool_t;
#endif

struct nai_main_task_pool_s {
    nai_list_entry_t ent;
    nai_str_t name;
    nai_task_pool_t pool;
    nai_int_t threads;
    nai_int_t max_queued;
};


static nai_sult_t nai_main_task_ctx_init(nai_main_t* m);
static nai_sult_t nai_main_task_ctx_prepare(nai_main_t* m);
static nai_sult_t nai_main_task_ctx_start(nai_main_t* m);
static nai_sult_t nai_main_task_ctx_stop(nai_main_t* m);


static nai_command_t nai_main_task_commands[] = {
    { "task_pool", nai_main_task_pool_command, 
        NAI_CLOC_MAIN|NAI_COPT_ARG1|NAI_COPT_ARG2|NAI_COPT_ARG3 },
    { 0 }
};


nai_module_t nai_main_task_module = {
    NAI_MODULE_HEADER, 
    NAI_MODULE_MAIN, 
    nai_main_task_commands, 
    "task_pool", 0, {
        nai_main_task_ctx_init, 
        nai_main_task_ctx_prepare, 
        nai_main_task_ctx_start, 
        nai_main_task_ctx_stop, 
        0 
    }
};


#define nai_main_task_ctx(m)                            \
    (nai_main_task_ctx_t*)                              \
    nai_main_local_at((m), nai_main_task_module)        \


static void nai_main_task_pool_cleanup(void* lp)
{
    nai_main_task_pool_t* p;
    p = (nai_main_task_pool_t*)lp;
    nai_task_pool_close(&p->pool);
};


static nai_sult_t nai_main_task_ctx_init(nai_main_t* m)
{
    nai_sult_t rc;
    nai_main_task_ctx_t* c;


    c = (nai_main_task_ctx_t*)nai_palloc(&m->pool, sizeof(*c));
    if (c == 0) {
        rc = nai_sult_from_errno();
        nai_log_alert(NAI_LOG_CORE, 
            nai_sult_to_errno(rc), 
            "alloc context of nai_main_task_ctx_t failed");
        goto _end;
    };

    nai_list_init(&c->list);
    nai_main_local_at(m, nai_main_task_module) = c;
    rc = 0;

_end:
    return rc;
};


static nai_sult_t nai_main_task_ctx_prepare(nai_main_t* m)
{
    nai_sult_t rc;
    nai_list_entry_t* e;
    nai_main_task_ctx_t* c;
    nai_main_task_pool_t* p;


    c = nai_main_task_ctx(m);
    if (c == 0) {
        rc = nai_errno_to_sult(NAI_EINTERNAL);
        nai_log_alert(NAI_LOG_CORE, 
            nai_sult_to_errno(rc), "context of nai_main_task_t is null");
        goto _end;
    };

    e = c->list.next;
    for ( ; e != &c->list; e = e->next) {
        p = (nai_main_task_pool_t*)e;
        if (p->threads == NAI_OPT_UNSET_VALUE) {
            p->threads = nai_nprocs(0);
        };
        if (p->max_queued == NAI_OPT_UNSET_VALUE) {
            p->max_queued = 4096;
        } else if (p->max_queued < 1024) {
            p->max_queued = 1024;
        };
    };

    rc = 0;

_end:
    return rc;
};


static nai_sult_t nai_main_task_ctx_start(nai_main_t* m)
{
    nai_int_t r;
    nai_sult_t rc;
    nai_list_entry_t* e;
    nai_main_task_ctx_t* c;
    nai_main_task_pool_t* p;


    c = nai_main_task_ctx(m);
    if (c == 0) {
        rc = nai_errno_to_sult(NAI_EINTERNAL);
        nai_log_alert(NAI_LOG_CORE, 
            nai_sult_to_errno(rc), "context of nai_main_task_t is null");
        goto _end;
    };

    e = c->list.next;
    for ( ; e != &c->list; e = e->next) {
        p = (nai_main_task_pool_t*)e;
        r = nai_task_pool_set_max_queued(&p->pool, p->max_queued);
        if (r < 0) {
            rc = nai_sult_from_errno();
            nai_log_alert(NAI_LOG_CORE, 
                nai_sult_to_errno(rc), "nai_task_pool_set_max_queued failed");
            goto _end;
        };

        r = nai_task_pool_open(&p->pool, p->threads, 0);
        if (r < 0) {
            rc = nai_sult_from_errno();
            nai_log_alert(NAI_LOG_CORE, 
                nai_sult_to_errno(rc), "nai_task_pool_open failed");
            goto _end;
        };
    };

    rc = 0;

_end:
    return rc;
};


static nai_sult_t nai_main_task_ctx_stop(nai_main_t* m)
{
    nai_int_t r;
    nai_sult_t rc;
    nai_list_entry_t* e;
    nai_main_task_ctx_t* c;
    nai_main_task_pool_t* p;


    c = nai_main_task_ctx(m);
    if (c == 0) {
        rc = nai_errno_to_sult(NAI_EINTERNAL);
        nai_log_alert(NAI_LOG_CORE, 
            nai_sult_to_errno(rc), "context of nai_main_task_t is null");
        goto _end;
    };

    e = c->list.next;
    for ( ; e != &c->list; e = e->next) {
        p = (nai_main_task_pool_t*)e;
        r = nai_task_pool_close(&p->pool);
        if (r < 0) {
            rc = nai_sult_from_errno();
            nai_log_alert(NAI_LOG_CORE, 
                nai_sult_to_errno(rc), "nai_task_pool_close failed");
            goto _end;
        };
    };

    rc = 0;

_end:
    return rc;
};


nai_sult_t nai_main_task_pool_command(
    nai_command_t* c, nai_command_args_t* args)
{
    nai_int_t threads;
    nai_int_t max_queued;
    nai_sult_t rc;
    nai_str_t str;
    nai_task_pool_t* r;
    nai_task_pool_t** pv;
    nai_main_task_pool_t* p;


    (void)c;

    if (c->offset != 0) {
        pv = (nai_task_pool_t**)(args->data + c->offset);
    } else {
        pv = 0;
    };
    if (pv != 0 && pv[0]) {
        rc = nai_errno_to_sult(EEXIST);
        nai_log_alert(NAI_LOG_CORE, 
            nai_sult_to_errno(rc), 
            "command '%s' has set dns client", c->name);
        goto _end;
    };

    rc = nai_value_dups(&args->argv[0], &str, args->pool_tmp);
    if (rc < 0) {
        nai_log_alert(NAI_LOG_CORE, 
            nai_sult_to_errno(rc), "get string failed");
        goto _end;
    };

    r = nai_main_task_pool_add(args->main, nai_str(&str), 0);
    if (r == 0) {
        rc = nai_sult_from_errno();
        nai_log_error(NAI_LOG_CORE, 
            nai_sult_to_errno(rc), "new task pool failed");
        goto _end;
    };

    p = nai_containof(r, nai_main_task_pool_t, pool);

    if (args->argc >= 2) {
        rc = nai_value_geti(&args->argv[1], &threads);
        if (rc < 0) {
            nai_log_error(NAI_LOG_CORE, 
                nai_sult_to_errno(rc), "invalid value of threads");
            goto _end;
        };
        if (threads <= 0) {
            nai_log_error(NAI_LOG_CORE, EINVAL, 
                "invalid value of threads '%d'", threads);
            rc = nai_errno_to_sult(EINVAL);
            goto _end;
        };

        p->threads = threads;
    };

    if (args->argc >= 3) {
        rc = nai_value_geti(&args->argv[2], &max_queued);
        if (rc < 0) {
            nai_log_error(NAI_LOG_CORE, 
                nai_sult_to_errno(rc), "invalid value of max_queued");
            goto _end;
        };

        if (max_queued <= 0) {
            nai_log_error(NAI_LOG_CORE, EINVAL, 
                "invalid value of max_queued %d", max_queued);
            rc = nai_errno_to_sult(EINVAL);
            goto _end;
        };

        p->max_queued = max_queued;
    };

    if (pv != 0) {
        pv[0] = &p->pool;
    };

    rc = 0;

_end:
    return rc;
};


nai_task_pool_t* nai_main_task_pool_add(
    nai_main_t* m, const char* name, nai_int_t create_new)
{
    intptr_t n;
    nai_int_t ec;
    nai_str_t str;
    nai_list_entry_t* e;
    nai_main_task_ctx_t* c;
    nai_main_task_pool_t* p;
    nai_task_pool_t* r;


    c = nai_main_task_ctx(m);
    if (c == 0) {
        ec = NAI_EINTERNAL;
        nai_log_alert(NAI_LOG_CORE, 
            ec, "context of nai_main_task_pool_t is null");

        nai_errno = ec;
        r = 0;
        goto _end;
    };

    nai_str_sets(&str, name);
    nai_str_hash(&str, 0);

    e = c->list.next;
    for ( ; e != &c->list; e = e->next) {
        p = (nai_main_task_pool_t*)e;
        if (nai_str_hasheq(&p->name, &str)) {
            break;
        };
    };

    if (e != &c->list) {
        /* found, t != 0 */
        if (create_new) {
            nai_errno = EEXIST;
            r = 0;
            goto _end;
        };
    } else {
        n = nai_str_dup(&str, nai_str(&str), nai_str_len(&str), &m->pool);
        if (n < 0) {
            nai_log_alert(NAI_LOG_CORE, nai_errno, "nai_str_dup failed");
            r = 0;
            goto _end;
        };

        p = nai_palloc(&m->pool, sizeof(*p));
        if (p == 0) {
            nai_log_alert(NAI_LOG_CORE, nai_errno, "alloc task pool failed");
            r = 0;
            goto _end;
        };

        p->name = str;
        p->threads = NAI_OPT_UNSET_VALUE;
        p->max_queued = NAI_OPT_UNSET_VALUE;
        nai_task_pool_init(&p->pool, 0);

        nai_pool_add_cleanup(&m->pool, nai_main_task_pool_cleanup, p);
        nai_list_insert_tail(&c->list, &p->ent);
    };

    r = &p->pool;

_end:
    return r;
};


nai_task_pool_t* nai_main_task_pool_get(nai_main_t* m, const char* name)
{
    nai_int_t ec;
    nai_str_t str;
    nai_list_entry_t* e;
    nai_main_task_ctx_t* c;
    nai_main_task_pool_t* p;
    nai_task_pool_t* r;


    c = nai_main_task_ctx(m);
    if (c == 0) {
        ec = NAI_EINTERNAL;
        nai_log_alert(NAI_LOG_CORE, 
            ec, "context of nai_main_task_pool_t is null");

        nai_errno = ec;
        r = 0;
        goto _end;
    };

    nai_str_sets(&str, name);
    nai_str_hash(&str, 0);

    e = c->list.next;
    for ( ; e != &c->list; e = e->next) {
        p = (nai_main_task_pool_t*)e;
        if (nai_str_hasheq(&p->name, &str)) {
            break;
        };
    };

    if (e != &c->list) {
        /* found, t != 0 */
        r = &p->pool;
    } else {
        nai_errno = ENOENT;
        r = 0;
    };

_end:
    return r;
};


nai_int_t nai_main_task_pool_set(
    nai_main_t* m, const char* name, nai_int_t threads, nai_int_t max_queued)
{
    nai_int_t r;
    nai_int_t ec;
    nai_str_t str;
    nai_list_entry_t* e;
    nai_main_task_ctx_t* c;
    nai_main_task_pool_t* p;


    if (threads < 0) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };
    if (max_queued < 0) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    c = nai_main_task_ctx(m);
    if (c == 0) {
        ec = NAI_EINTERNAL;
        nai_log_alert(NAI_LOG_CORE, 
            ec, "context of nai_main_task_pool_t is null");

        nai_errno = ec;
        r = -1;
        goto _end;
    };

    nai_str_sets(&str, name);
    nai_str_hash(&str, 0);

    e = c->list.next;
    for ( ; e != &c->list; e = e->next) {
        p = (nai_main_task_pool_t*)e;
        if (nai_str_hasheq(&p->name, &str)) {
            break;
        };
    };

    if (e == &c->list) {
        nai_errno = ENOENT;
        r = -1;
        goto _end;
    };

    p->threads = threads;
    p->max_queued = max_queued;
    r = 0;

_end:
    return r;
};



