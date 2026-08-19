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
/// @file       nai_main_agent.c
/// @brief      
/// @details
/// @date       2021-08-08
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai/service/nai_main.h"
#include "nai/service/nai_main_agent.h"
#include "nai/service/nai_main_dns.h"
#include "nai/service/nai_main_event.h"
#include "nai/service/nai_command.h"
#include "nai/io/nai_agent.h"
#include "nai/runtime/nai_string.h"
#include "nai/runtime/nai_list.h"
#include "nai/runtime/nai_uri.h"
#include "nai/runtime/nai_log.h"
#include "nai/runtime/nai_errno.h"



typedef struct nai_main_agent_s {
    nai_list_entry_t ent;
    nai_str_t name;
    nai_agent_t agent;
    nai_local_t* local;
} nai_main_agent_t;


typedef struct nai_main_agent_ctx_s {
    union {
        nai_list_entry_t list[2];
        struct {
            nai_list_entry_t named; /**< the list of named agent */
            nai_list_entry_t anony; /**< the list of anonymous agent */
        };
    };
} nai_main_agent_ctx_t;



static nai_sult_t nai_main_agent_init(nai_main_t* m);
static nai_sult_t nai_main_agent_prepare(nai_main_t* m);
static nai_sult_t nai_main_agent_start(nai_main_t* m);
static nai_sult_t nai_main_agent_stop(nai_main_t* m);
static nai_sult_t nai_main_agent_cleanup(nai_main_t* m);


static nai_sult_t nai_main_agent_server(
    nai_command_t* c, nai_command_args_t* args);
static nai_sult_t nai_main_agent_rule(
    nai_command_t* c, nai_command_args_t* args);


static nai_command_optinfo_t nai_agent_protos[] = {
    { nai_strconst("tcp"), 1 << NAI_IO_TYPE_STREAM }, 
    { nai_strconst("udp"), 1 << NAI_IO_TYPE_DGRAM }, 
    { nai_strconst("both"), 
        (1 << NAI_IO_TYPE_STREAM) | (1 << NAI_IO_TYPE_DGRAM) }, 
    { nai_strnull(), 0 }
};


static nai_command_t nai_main_agent_commands[] = {
    { "agent", nai_main_agent_command, 
        NAI_CLOC_MAIN|NAI_COPT_SEGEMENT|NAI_COPT_ARG0|NAI_COPT_ARG1 },
    { "server", nai_main_agent_server, 
        NAI_CLOC_AGENT|NAI_COPT_ARG2 }, 
    { "rule", nai_main_agent_rule, 
        NAI_CLOC_AGENT|NAI_COPT_ARG2|NAI_COPT_ARG3 }, 
    { 0 }
};


nai_module_t nai_main_agent_module = {
    NAI_MODULE_HEADER, 
    NAI_MODULE_MAIN, 
    nai_main_agent_commands, 
    "agent", 0, {
        nai_main_agent_init, 
        nai_main_agent_prepare, 
        nai_main_agent_start, 
        nai_main_agent_stop, 
        nai_main_agent_cleanup 
    }
};


#define nai_main_agent_ctx(m)                             \
    (nai_main_agent_ctx_t*)                               \
    nai_main_local_at((m), nai_main_agent_module)         \


static nai_main_agent_t* nai_main_agent_find(
    nai_main_agent_ctx_t* dc, const nai_str_t* name)
{
    nai_str_t str;
    nai_list_entry_t* e;
    nai_main_agent_t* d;


    if (!nai_list_is_empty(&dc->named)) {
        str = name[0];
        nai_str_hash(&str, 0);

        e = dc->named.next;
        for ( ; e != &dc->named; e = e->next) {
            d = (nai_main_agent_t*)e;

            if (nai_str_hasheq(&d->name, &str)) {
                goto _end;
            };
        };
    };

    nai_errno = ENOENT;
    d = 0;

_end:
    return d;
};


static nai_sult_t nai_main_agent_server(
    nai_command_t* c, nai_command_args_t* args)
{
    nai_int_t r;
    nai_sult_t rc;
    nai_str_t str;
    nai_mem_t uri;
    nai_uri_t u;
    nai_main_agent_t* d;


    d = (nai_main_agent_t*)args->data;
    if (d == 0) {
        nai_log_alert(NAI_LOG_CORE, 
            EINVAL, "current dns client is null");
        rc = nai_errno_to_sult(EINVAL);
        goto _end;
    };

    rc = nai_value_dups(&args->argv[0], &str, args->pool_tmp);
    if (rc < 0) {
        nai_log_error(NAI_LOG_CORE, 
            nai_sult_to_errno(rc), 
            "invalid argument 'name' of command '%s'", c->name);
        goto _end;
    };

    rc = nai_value_getm(&args->argv[1], &uri);
    if (rc < 0) {
        nai_log_error(NAI_LOG_CORE, 
            nai_sult_to_errno(rc), 
            "invalid argument 'uri' of command '%s'", c->name);
        goto _end;
    };

    r = nai_uri_parse(&u, nai_str(&uri), nai_str_len(&uri), NAI_URI_STRICT);
    if (r < 0) {
        rc = nai_sult_from_errno();
        nai_log_error(NAI_LOG_CORE, 
            nai_sult_to_errno(rc), 
            "invalid argument 'uri' of command '%s'", c->name);
        goto _end;
    };

    r = nai_agent_add_node_uri(&d->agent, (nai_mem_t*)&str, &u);
    if (r < 0) {
        rc = nai_sult_from_errno();
        nai_log_error(NAI_LOG_CORE, 
            nai_sult_to_errno(rc), 
            "nai_agent_add_node_uri(%.*s, %.*s) failed", 
            nai_str_len(&str), nai_str(&str), 
            nai_str_len(&uri), nai_str(&uri));
        goto _end;
    };

    rc = 0;

_end:
    return rc;
};


static nai_sult_t nai_main_agent_rule(
    nai_command_t* c, nai_command_args_t* args)
{
    nai_int_t r;
    nai_int_t m;
    nai_int_t found;
    nai_int_t proto_bits;
    nai_sult_t rc;
    nai_str_t str;
    nai_mem_t pattern;
    nai_mem_t proto;
    nai_main_agent_t* d;
    nai_command_optinfo_t* opts;


    d = (nai_main_agent_t*)args->data;
    if (d == 0) {
        nai_log_alert(NAI_LOG_CORE, 
            EINVAL, "current dns client is null");
        rc = nai_errno_to_sult(EINVAL);
        goto _end;
    };

    rc = nai_value_dups(&args->argv[0], &str, args->pool_tmp);
    if (rc < 0) {
        nai_log_error(NAI_LOG_CORE, 
            nai_sult_to_errno(rc), 
            "invalid argument 'name' of command '%s'", c->name);
        goto _end;
    };

    rc = nai_value_getm(&args->argv[1], &pattern);
    if (rc < 0) {
        nai_log_error(NAI_LOG_CORE, 
            nai_sult_to_errno(rc), 
            "invalid argument 'pattern' of command '%s'", c->name);
        goto _end;
    };

    proto_bits = 1 << NAI_IO_TYPE_STREAM;
    if (args->argc > 2) {
        rc = nai_value_getm(&args->argv[2], &proto);
        if (rc < 0) {
            nai_log_error(NAI_LOG_CORE, 
                nai_sult_to_errno(rc), 
                "invalid argument 'proto' of command '%s'", c->name);
            goto _end;
        };

        found = 0;
        opts = nai_agent_protos;
        for (m = 0; opts[m].name.ptr; m ++) {
            if (nai_str_caseeq(&proto, &opts[m].name)) {
                proto_bits = opts[m].value;
                found = 1;
                break;
            };
        };
        if (!found) {
            nai_log_error(NAI_LOG_CORE, EINVAL, 
                "invalid option value '%s' of command '%s'", 
                nai_str(&proto), c->name);
            rc = nai_errno_to_sult(EINVAL);
            goto _end;
        };
    };

    r = nai_agent_add_rule(
        &d->agent, (nai_mem_t*)&str, &pattern, proto_bits);
    if (r < 0) {
        rc = nai_sult_from_errno();
        nai_log_error(NAI_LOG_CORE, 
            nai_sult_to_errno(rc), 
            "nai_agent_add_rule('%.*s' '%.*s') failed", 
            nai_str_len(&str), nai_str(&str), 
            nai_str_len(&pattern), nai_str(&pattern));
        goto _end;
    };

    rc = 0;

_end:
    return rc;
};


static nai_sult_t nai_main_agent_init(nai_main_t* m)
{
    nai_sult_t rc;
    nai_main_agent_ctx_t* p;


    p = (nai_main_agent_ctx_t*)nai_palloc(&m->pool, sizeof(*p));
    if (p == 0) {
        rc = nai_sult_from_errno();
        nai_log_alert(NAI_LOG_CORE, 
            nai_sult_to_errno(rc), "alloc nai_main_agent_ctx_t failed");
        goto _end;
    };


    nai_list_init(&p->named);
    nai_list_init(&p->anony);
    nai_main_local_at(m, nai_main_agent_module) = p;
    rc = 0;

_end:
    return rc;
};


static nai_sult_t nai_main_agent_prepare(nai_main_t* m)
{
    nai_sult_t rc;
    nai_main_agent_ctx_t* p;


    p = nai_main_agent_ctx(m);
    if (p == 0) {
        rc = nai_errno_to_sult(NAI_EINTERNAL);
        nai_log_alert(NAI_LOG_CORE, 
            nai_sult_to_errno(rc), "context of nai_main_agent_t is null");
        goto _end;
    };

    rc = 0;

_end:
    return rc;
};


static nai_sult_t nai_main_agent_start(nai_main_t* m)
{
    nai_int_t r;
    nai_int_t n;
    nai_sult_t rc;
    nai_list_entry_t* e;
    nai_list_entry_t* list;
    nai_main_agent_t* d;
    nai_main_agent_ctx_t* p;
    nai_main_event_t* ev;


    p = nai_main_agent_ctx(m);
    if (p == 0) {
        rc = nai_errno_to_sult(NAI_EINTERNAL);
        nai_log_alert(NAI_LOG_CORE, 
            nai_sult_to_errno(rc), "context of nai_main_agent_t is null");
        goto _end;
    };

    ev = nai_main_event_get(m);
    if (ev == 0 || 
        ev->loop == 0) {
        nai_log_alert(NAI_LOG_CORE, 
            EPERM, "cannot open dns clients without the event loop");
        rc = nai_errno_to_sult(EPERM);
        goto _end;
    };

    for (n = 0; n < (nai_int_t)nai_countof(p->list); n ++) {
        list = &p->list[n];
        e = list->next;
        for ( ; e != list; e = e->next) {
            d = (nai_main_agent_t*)e;
            r = nai_agent_open(&d->agent, ev->loop);
            if (r < 0) {
                rc = nai_sult_from_errno();
                nai_log_alert(NAI_LOG_CORE, 
                    nai_sult_to_errno(rc), "nai_agent_open failed");
                goto _end;
            };
        };
    };

    rc = 0;

_end:
    return rc;
};


static nai_sult_t nai_main_agent_stop(nai_main_t* m)
{
    nai_int_t r;
    nai_int_t n;
    nai_sult_t rc;
    nai_list_entry_t* e;
    nai_list_entry_t* list;
    nai_main_agent_t* d;
    nai_main_agent_ctx_t* p;


    p = nai_main_agent_ctx(m);
    if (p == 0) {
        rc = nai_errno_to_sult(NAI_EINTERNAL);
        nai_log_alert(NAI_LOG_CORE, 
            nai_sult_to_errno(rc), "context of nai_main_agent_t is null");
        goto _end;
    };

    for (n = 0; n < (nai_int_t)nai_countof(p->list); n ++) {
        list = &p->list[n];
        e = list->next;
        for ( ; e != list; e = e->next) {
            d = (nai_main_agent_t*)e;
            r = nai_agent_reuse(&d->agent);
            if (r < 0) {
                rc = nai_sult_from_errno();
                nai_log_alert(NAI_LOG_CORE, 
                    nai_sult_to_errno(rc), "nai_agent_reuse failed");
                goto _end;
            };
        };
    };

    rc = 0;

_end:
    return rc;
};


static nai_sult_t nai_main_agent_cleanup(nai_main_t* m)
{
    nai_int_t r;
    nai_int_t n;
    nai_sult_t rc;
    nai_list_entry_t* e;
    nai_list_entry_t* list;
    nai_main_agent_t* d;
    nai_main_agent_ctx_t* p;


    p = nai_main_agent_ctx(m);
    if (p == 0) {
        rc = nai_errno_to_sult(NAI_EINTERNAL);
        nai_log_alert(NAI_LOG_CORE, 
            nai_sult_to_errno(rc), "context of nai_main_agent_t is null");
        goto _end;
    };

    for (n = 0; n < (nai_int_t)nai_countof(p->list); n ++) {
        list = &p->list[n];
        e = list->next;
        for ( ; e != list; e = e->next) {
            d = (nai_main_agent_t*)e;
            r = nai_agent_close(&d->agent);
            if (r < 0) {
                rc = nai_sult_from_errno();
                nai_log_alert(NAI_LOG_CORE, 
                    nai_sult_to_errno(rc), "nai_agent_close failed");
                goto _end;
            };
        };

        nai_list_init(list);
    };

    rc = 0;

_end:
    return rc;
};


nai_agent_t* nai_main_agent_get(nai_main_t* m, const char* name)
{
    nai_agent_t* r;
    nai_str_t str;
    nai_main_agent_t* d;
    nai_main_agent_ctx_t* p;


    nai_str_sets(&str, name);

    p = nai_main_agent_ctx(m);
    d = nai_main_agent_find(p, &str);
    if (d != 0) {
        r = &d->agent;
        goto _end;
    };

    r = 0;

_end:
    return r;
};


nai_agent_t* nai_main_agent_add(
    nai_main_t* m, const char* name, nai_int_t create_new)
{
    intptr_t n;
    nai_agent_t* r;
    nai_str_t str;
    nai_main_agent_t* d;
    nai_main_agent_ctx_t* p;


    p = nai_main_agent_ctx(m);
    if (name) {
        nai_str_sets(&str, name);
        d = nai_main_agent_find(p, &str);
    } else {
        d = 0;
    };

    if (d != 0) {
        if (create_new) {
            nai_errno = EEXIST;
            r = 0;
            goto _end;
        };

        r = &d->agent;
        goto _end;
    };

    d = (nai_main_agent_t*)nai_palloc(&m->pool, sizeof(*d));
    if (d == 0) {
        nai_log_alert(NAI_LOG_CORE, 
            nai_errno, "alloc nai_main_agent_t failed");
        r = 0;
        goto _end;
    };

    /* initialize */
    nai_agent_init(&d->agent, &m->pool);
    d->local = 0;

    /* add into list */
    if (name) {
        n = nai_str_dup(&str, 
            nai_str(&str), nai_str_len(&str), &m->pool);
        if (n < 0) {
            nai_log_alert(NAI_LOG_CORE, nai_errno, "nai_str_dup failed");
            r = 0;
            goto _end;
        };

        d->name = str;
        nai_str_hash(&d->name, 0);
        nai_list_insert_tail(&p->named, &d->ent);
    } else {
        nai_str_setn(&d->name);
        nai_list_insert_tail(&p->anony, &d->ent);
    };

    r = &d->agent;

_end:
    return r;
};


nai_sult_t nai_main_agent_command(
    nai_command_t* c, nai_command_args_t* args)
{
    nai_sult_t rc;
    nai_str_t str;
    nai_agent_t* r;
    nai_main_t* m;
    nai_main_agent_t* d;
    nai_agent_t** pv;


    m = args->main;
    if (args->endseg) {
        rc = 0;
        goto _end;
    };

    if (c->offset != 0) {
        pv = (nai_agent_t**)(args->data + c->offset);
    } else {
        pv = 0;
    };
    if (pv != 0 && pv[0]) {
        rc = nai_errno_to_sult(EPERM);
        nai_log_alert(NAI_LOG_CORE, 
            nai_sult_to_errno(rc), 
            "command '%s' has set dns client", c->name);
        goto _end;
    };

    if (args->argc >= 1) {
        rc = nai_value_dups(&args->argv[0], &str, args->pool_tmp);
        if (rc < 0) {
            nai_log_error(NAI_LOG_CORE, 
                nai_sult_to_errno(rc), 
                "command '%s' get string failed", c->name);
            goto _end;
        };

    } else { /* argc == 0 */
        if (pv == 0) {
            nai_log_alert(NAI_LOG_CORE, EINVAL, 
                "command '%s' required a name", c->name);
            rc = nai_errno_to_sult(EINVAL);
            goto _end;
        };

        nai_str_setn(&str);
    };

    r = nai_main_agent_add(args->main, nai_str(&str), 0);
    if (r == 0) {
        rc = nai_sult_from_errno();
        nai_log_error(NAI_LOG_CORE, EINVAL, 
            "nai_main_agent_add(%s) failed", nai_str(&str));
        goto _end;
    };

    if (pv) {
        pv[0] = r;
    };
    if (pv && args->argc >= 1) {
        args->endseg = 1;
    } else {
        d = nai_containof(r, nai_main_agent_t, agent);
        if (d->local == 0) {
            /* only used during configuration, using temporary memory pool */
            d->local = nai_main_local_alloc(m, 0, args->pool_tmp);
            if (d->local == 0) {
                rc = nai_sult_from_errno();
                nai_log_alert(NAI_LOG_CORE, 
                    nai_sult_to_errno(rc), "alloc local data of dns failed");
                goto _end;
            };

            nai_local_at(d->local, nai_main_agent_module) = d;
        };

        args->grp = NAI_MODULE_MAIN;
        args->grp_mask = NAI_CLOC_AGENT;
        args->clocal = &d->local;
    };

    rc = 0;

_end:
    return rc;
};


