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
/// @file       nai_main_dns.c
/// @brief      
/// @details
/// @date       2021-08-08
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai/service/nai_main.h"
#include "nai/service/nai_main_dns.h"
#include "nai/service/nai_main_event.h"
#include "nai/service/nai_command.h"
#include "nai/io/nai_dns.h"
#include "nai/runtime/nai_string.h"
#include "nai/runtime/nai_list.h"
#include "nai/runtime/nai_log.h"
#include "nai/runtime/nai_errno.h"



typedef struct nai_main_dns_s {
    nai_list_entry_t ent;
    nai_str_t name;
    nai_dns_t dns;
    nai_local_t* local;
} nai_main_dns_t;


typedef struct nai_main_dns_ctx_s {
    union {
        nai_list_entry_t list[2];
        struct {
            nai_list_entry_t named; /**< the list of named dns */
            nai_list_entry_t anony; /**< the list of anonymous dns */
        };
    };
} nai_main_dns_ctx_t;



static nai_sult_t nai_main_dns_init(nai_main_t* m);
static nai_sult_t nai_main_dns_prepare(nai_main_t* m);
static nai_sult_t nai_main_dns_start(nai_main_t* m);
static nai_sult_t nai_main_dns_stop(nai_main_t* m);
static nai_sult_t nai_main_dns_cleanup(nai_main_t* m);


static nai_sult_t nai_main_dns_default(
    nai_command_t* c, nai_command_args_t* args);
static nai_sult_t nai_main_dns_server(
    nai_command_t* c, nai_command_args_t* args);
static nai_sult_t nai_main_dns_host(
    nai_command_t* c, nai_command_args_t* args);
static nai_sult_t nai_main_dns_resend(
    nai_command_t* c, nai_command_args_t* args);


static nai_command_optinfo_t nai_dns_features[] = {
    { nai_strconst("tcp"), 0x01 }, 
    { nai_strconst("ipv4"),  0x02 },
    { nai_strnull(), 0 }
};

static nai_command_optinfo_t nai_dns_default[] = {
    { nai_strconst("all"), 0 }, 
    { nai_strconst("server"),  NAI_DNS_NO_HOST },
    { nai_strconst("host"),  NAI_DNS_NO_SERVER },
    { nai_strnull(), 0 }
};


static nai_command_t nai_main_dns_commands[] = {
    { "dns", nai_main_dns_command, 
        NAI_CLOC_MAIN|NAI_COPT_SEGEMENT|NAI_COPT_ARG0|NAI_COPT_ARG1 },
    { "default", nai_main_dns_default, 
        NAI_CLOC_DNS|NAI_COPT_ARG1, 
        0, nai_dns_default }, 
    { "server", nai_main_dns_server, 
        NAI_CLOC_DNS|NAI_COPT_ARG1 }, 
    { "host", nai_main_dns_host, 
        NAI_CLOC_DNS|NAI_COPT_ARG2MORE }, 
    { "expire", nai_command_set_msec32, 
        NAI_CLOC_DNS|NAI_COPT_ARG1, 
        nai_offsetof(nai_main_dns_t, dns.expire), 0}, 
    { "resend", nai_main_dns_resend, 
        NAI_CLOC_DNS|NAI_COPT_ARG1|NAI_COPT_ARG2, 
        nai_offsetof(nai_main_dns_t, dns.resend), 0}, 
    { "option", nai_command_set_opts, 
        NAI_CLOC_DNS|NAI_COPT_ARG1MORE,
        nai_offsetof(nai_main_dns_t, dns.flags), nai_dns_features },
    { 0 }
};


nai_module_t nai_main_dns_module = {
    NAI_MODULE_HEADER, 
    NAI_MODULE_MAIN, 
    nai_main_dns_commands, 
    "dns", 0, {
        nai_main_dns_init, 
        nai_main_dns_prepare, 
        nai_main_dns_start, 
        nai_main_dns_stop, 
        nai_main_dns_cleanup 
    }
};


#define nai_main_dns_ctx(m)                             \
    (nai_main_dns_ctx_t*)                               \
    nai_main_local_at((m), nai_main_dns_module)         \


static nai_main_dns_t* nai_main_dns_find(
    nai_main_dns_ctx_t* dc, const nai_str_t* name)
{
    nai_str_t str;
    nai_list_entry_t* e;
    nai_main_dns_t* d;


    if (!nai_list_is_empty(&dc->named)) {
        str = name[0];
        nai_str_hash(&str, 0);

        e = dc->named.next;
        for ( ; e != &dc->named; e = e->next) {
            d = (nai_main_dns_t*)e;

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


static nai_sult_t nai_main_dns_default(
    nai_command_t* c, nai_command_args_t* args)
{
    nai_int_t r;
    nai_int_t flags;
    nai_sult_t rc;
    nai_main_dns_t* d;


    d = (nai_main_dns_t*)args->data;
    if (d == 0) {
        nai_log_alert(NAI_LOG_CORE, 
            EINVAL, "current dns client is null");
        rc = nai_errno_to_sult(EINVAL);
        goto _end;
    };

    args->data = (void*)&flags;
    rc = nai_command_set_opts(c, args);
    args->data = (void*)d;
    if (rc < 0) {
        goto _end;
    };

    r = nai_dns_add_default(&d->dns, flags);
    if (r < 0) {
        rc = nai_sult_from_errno();
        nai_log_error(NAI_LOG_CORE, 
            nai_sult_to_errno(rc), 
            "nai_dns_add_default(%.*s) failed", 
            nai_str_len(&d->name), nai_str(&d->name));
        goto _end;
    };

    rc = 0;

_end:
    return rc;
};


static nai_sult_t nai_main_dns_server(
    nai_command_t* c, nai_command_args_t* args)
{
    nai_int_t r;
    nai_sult_t rc;
    nai_mem_t str;
    nai_main_dns_t* d;


    d = (nai_main_dns_t*)args->data;
    if (d == 0) {
        nai_log_alert(NAI_LOG_CORE, 
            EINVAL, "current dns client is null");
        rc = nai_errno_to_sult(EINVAL);
        goto _end;
    };

    rc = nai_value_getm(&args->argv[0], &str);
    if (rc < 0) {
        nai_log_error(NAI_LOG_CORE, 
            nai_sult_to_errno(rc), 
            "invalid argument of command '%s'", c->name);
        goto _end;
    };

    r = nai_dns_add_server(&d->dns, &str, 1);
    if (r < 0) {
        rc = nai_sult_from_errno();
        nai_log_error(NAI_LOG_CORE, 
            nai_sult_to_errno(rc), 
            "nai_dns_add_server(%.*s) failed", 
            nai_str_len(&str), nai_str(&str));
        goto _end;
    };

    rc = 0;

_end:
    return rc;
};


static nai_sult_t nai_main_dns_host(
    nai_command_t* c, nai_command_args_t* args)
{
    nai_int_t r;
    nai_int_t n;
    nai_sult_t rc;
    nai_mem_t host;
    nai_mem_t addr;
    nai_main_dns_t* d;


    d = (nai_main_dns_t*)args->data;
    if (d == 0) {
        nai_log_alert(NAI_LOG_CORE, 
            EINVAL, "current dns client is null");
        rc = nai_errno_to_sult(EINVAL);
        goto _end;
    };

    rc = nai_value_getm(&args->argv[0], &addr);
    if (rc < 0) {
        nai_log_error(NAI_LOG_CORE, 
            nai_sult_to_errno(rc), 
            "invalid address of command '%s'", c->name);
        goto _end;
    };


    for (n = 1; n < args->argc; n ++) {
        rc = nai_value_getm(&args->argv[n], &host);
        if (rc < 0) {
            nai_log_error(NAI_LOG_CORE, 
                nai_sult_to_errno(rc), 
                "invalid host %d of command '%s'", n, c->name);
            goto _end;
        };

        r = nai_dns_add_static_addr(&d->dns, &host, &addr);
        if (r < 0) {
            rc = nai_sult_from_errno();
            nai_log_error(NAI_LOG_CORE, 
                nai_sult_to_errno(rc), 
                "nai_dns_add_static_addr(%.*s, %.*s) failed", 
                nai_str_len(&host), nai_str(&host), 
                nai_str_len(&addr), nai_str(&addr));
            goto _end;
        };
    };

    rc = 0;

_end:
    return rc;
};


static nai_sult_t nai_main_dns_resend(
    nai_command_t* c, nai_command_args_t* args)
{
    nai_int_t times;
    nai_sult_t rc;
    nai_main_dns_t* d;


    d = (nai_main_dns_t*)args->data;
    if (d == 0) {
        nai_log_alert(NAI_LOG_CORE, 
            EINVAL, "current dns client is null");
        rc = nai_errno_to_sult(EINVAL);
        goto _end;
    };

    if (args->argc > 1) {
        rc = nai_value_geti(&args->argv[1], &times);
        if (rc < 0) {
            nai_log_alert(NAI_LOG_CORE, 
                EINVAL, "invalid times of command %s", c->name);
            goto _end;
        };
    };

    rc = nai_command_set_msec32(c, args);
    if (rc >= 0 && 
        args->argc > 1) {
        d->dns.resends = times;
    };

_end:
    return rc;
};


static nai_sult_t nai_main_dns_init(nai_main_t* m)
{
    nai_sult_t rc;
    nai_main_dns_ctx_t* p;


    p = (nai_main_dns_ctx_t*)nai_palloc(&m->pool, sizeof(*p));
    if (p == 0) {
        rc = nai_sult_from_errno();
        nai_log_alert(NAI_LOG_CORE, 
            nai_sult_to_errno(rc), "alloc nai_main_dns_ctx_t failed");
        goto _end;
    };


    nai_list_init(&p->named);
    nai_list_init(&p->anony);
    nai_main_local_at(m, nai_main_dns_module) = p;
    rc = 0;

_end:
    return rc;
};


static nai_sult_t nai_main_dns_prepare(nai_main_t* m)
{
    nai_int_t r;
    nai_int_t n;
    nai_sult_t rc;
    nai_list_entry_t* e;
    nai_list_entry_t* list;
    nai_main_dns_t* d;
    nai_main_dns_ctx_t* p;


    p = nai_main_dns_ctx(m);
    if (p == 0) {
        rc = nai_errno_to_sult(NAI_EINTERNAL);
        nai_log_alert(NAI_LOG_CORE, 
            nai_sult_to_errno(rc), "context of nai_main_dns_t is null");
        goto _end;
    };

    /* clear local and load default servers */
    for (n = 0; n < (nai_int_t)nai_countof(p->list); n ++) {
        list = &p->list[n];
        e = list->next;
        for ( ; e != list; e = e->next) {
            d = (nai_main_dns_t*)e;
            d->local = 0;

            if (d->dns.servers.count > 0) {
                continue;
            };

            r = nai_dns_add_default(&d->dns, NAI_DNS_NO_HOST);
            if (r < 0) {
                rc = nai_sult_from_errno();
                nai_log_error(NAI_LOG_CORE, 
                    nai_sult_to_errno(rc), "nai_dns_add_default() failed");
                goto _end;
            };
        };
    };

    rc = 0;

_end:
    return rc;
};


static nai_sult_t nai_main_dns_start(nai_main_t* m)
{
    nai_int_t r;
    nai_int_t n;
    nai_sult_t rc;
    nai_list_entry_t* e;
    nai_list_entry_t* list;
    nai_main_dns_t* d;
    nai_main_dns_ctx_t* p;
    nai_main_event_t* ev;


    p = nai_main_dns_ctx(m);
    if (p == 0) {
        rc = nai_errno_to_sult(NAI_EINTERNAL);
        nai_log_alert(NAI_LOG_CORE, 
            nai_sult_to_errno(rc), "context of nai_main_dns_t is null");
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
            d = (nai_main_dns_t*)e;
            r = nai_dns_open(&d->dns, ev->loop);
            if (r < 0) {
                rc = nai_sult_from_errno();
                nai_log_alert(NAI_LOG_CORE, 
                    nai_sult_to_errno(rc), "nai_dns_open failed");
                goto _end;
            };
        };
    };

    rc = 0;

_end:
    return rc;
};


static nai_sult_t nai_main_dns_stop(nai_main_t* m)
{
    nai_int_t r;
    nai_int_t n;
    nai_sult_t rc;
    nai_list_entry_t* e;
    nai_list_entry_t* list;
    nai_main_dns_t* d;
    nai_main_dns_ctx_t* p;


    p = nai_main_dns_ctx(m);
    if (p == 0) {
        rc = nai_errno_to_sult(NAI_EINTERNAL);
        nai_log_alert(NAI_LOG_CORE, 
            nai_sult_to_errno(rc), "context of nai_main_dns_t is null");
        goto _end;
    };

    for (n = 0; n < (nai_int_t)nai_countof(p->list); n ++) {
        list = &p->list[n];
        e = list->next;
        for ( ; e != list; e = e->next) {
            d = (nai_main_dns_t*)e;
            r = nai_dns_reuse(&d->dns);
            if (r < 0) {
                rc = nai_sult_from_errno();
                nai_log_alert(NAI_LOG_CORE, 
                    nai_sult_to_errno(rc), "nai_dns_reuse failed");
                goto _end;
            };
        };
    };

    rc = 0;

_end:
    return rc;
};


static nai_sult_t nai_main_dns_cleanup(nai_main_t* m)
{
    nai_int_t r;
    nai_int_t n;
    nai_sult_t rc;
    nai_list_entry_t* e;
    nai_list_entry_t* list;
    nai_main_dns_t* d;
    nai_main_dns_ctx_t* p;


    p = nai_main_dns_ctx(m);
    if (p == 0) {
        rc = nai_errno_to_sult(NAI_EINTERNAL);
        nai_log_alert(NAI_LOG_CORE, 
            nai_sult_to_errno(rc), "context of nai_main_dns_t is null");
        goto _end;
    };

    for (n = 0; n < (nai_int_t)nai_countof(p->list); n ++) {
        list = &p->list[n];
        e = list->next;
        for ( ; e != list; e = e->next) {
            d = (nai_main_dns_t*)e;
            r = nai_dns_close(&d->dns);
            if (r < 0) {
                rc = nai_sult_from_errno();
                nai_log_alert(NAI_LOG_CORE, 
                    nai_sult_to_errno(rc), "nai_dns_close failed");
                goto _end;
            };
        };

        nai_list_init(list);
    };

    rc = 0;

_end:
    return rc;
};


nai_dns_t* nai_main_dns_get(nai_main_t* m, const char* name)
{
    nai_dns_t* r;
    nai_str_t str;
    nai_main_dns_t* d;
    nai_main_dns_ctx_t* p;


    nai_str_sets(&str, name);

    p = nai_main_dns_ctx(m);
    d = nai_main_dns_find(p, &str);
    if (d != 0) {
        r = &d->dns;
        goto _end;
    };

    r = 0;

_end:
    return r;
};


nai_dns_t* nai_main_dns_add(
    nai_main_t* m, const char* name, nai_int_t create_new)
{
    intptr_t n;
    nai_dns_t* r;
    nai_str_t str;
    nai_main_dns_t* d;
    nai_main_dns_ctx_t* p;


    p = nai_main_dns_ctx(m);
    if (name) {
        nai_str_sets(&str, name);
        d = nai_main_dns_find(p, &str);
    } else {
        d = 0;
    };

    if (d != 0) {
        if (create_new) {
            nai_errno = EEXIST;
            r = 0;
            goto _end;
        };

        r = &d->dns;
        goto _end;
    };

    d = (nai_main_dns_t*)nai_palloc(&m->pool, sizeof(*d));
    if (d == 0) {
        nai_log_alert(NAI_LOG_CORE, 
            nai_errno, "alloc nai_main_dns_t failed");
        r = 0;
        goto _end;
    };

    /* initialize */
    nai_dns_init(&d->dns);
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

    r = &d->dns;

_end:
    return r;
};


nai_sult_t nai_main_dns_command(
    nai_command_t* c, nai_command_args_t* args)
{
    nai_sult_t rc;
    nai_str_t str;
    nai_dns_t* r;
    nai_main_t* m;
    nai_main_dns_t* d;
    nai_dns_t** pv;


    m = args->main;
    if (args->endseg) {
        rc = 0;
        goto _end;
    };

    if (c->offset != 0) {
        pv = (nai_dns_t**)(args->data + c->offset);
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

    r = nai_main_dns_add(args->main, nai_str(&str), 0);
    if (r == 0) {
        rc = nai_sult_from_errno();
        nai_log_error(NAI_LOG_CORE, EINVAL, 
            "nai_main_dns_add(%s) failed", nai_str(&str));
        goto _end;
    };

    if (pv) {
        pv[0] = r;
    };
    if (pv && args->argc >= 1) {
        args->endseg = 1;
    } else {
        d = nai_containof(r, nai_main_dns_t, dns);
        if (d->local == 0) {
            /* only used during configuration, using temporary memory pool */
            d->local = nai_main_local_alloc(m, 0, args->pool_tmp);
            if (d->local == 0) {
                rc = nai_sult_from_errno();
                nai_log_alert(NAI_LOG_CORE, 
                    nai_sult_to_errno(rc), "alloc local data of dns failed");
                goto _end;
            };

            nai_local_at(d->local, nai_main_dns_module) = d;
        };

        args->grp = NAI_MODULE_MAIN;
        args->grp_mask = NAI_CLOC_DNS;
        args->clocal = &d->local;
    };

    rc = 0;

_end:
    return rc;
};


