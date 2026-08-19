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
/// @file       nai_httpc_client.c
/// @brief      
/// @details
/// @date       2021-08-26
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai/runtime/nai_util.h"
#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_log.h"
#include "nai/service/nai_main.h"
#include "nai/service/nai_main_event.h"
#include "nai/service/nai_main_dns.h"
#include "nai/service/nai_main_agent.h"
#include "nai_httpc_core.h"
#include <stdarg.h>


extern 
nai_int_t nai_main_set_fmtv(
    nai_main_t* m, const char* fmt, va_list va);

extern 
nai_int_t nai_main_lock_segment(nai_main_t* m);


extern 
nai_int_t nai_main_unlock_segment(nai_main_t* m);



static nai_sult_t nai_httpc_client_prepare(nai_main_t* m);


static nai_command_t nai_httpc_client_commands[] = {

    { "client_dns", nai_main_dns_command, 
        NAI_HTTPC_CLOC_MAGENT|NAI_HTTPC_COPT_MAIN|
        NAI_COPT_SEGEMENT|NAI_COPT_ARG0|NAI_COPT_ARG1, 
        nai_offsetof(nai_httpc_client_t, attr.dns), 0 },
    { "client_agent", nai_main_agent_command, 
        NAI_HTTPC_CLOC_MAGENT|NAI_HTTPC_COPT_MAIN|
        NAI_COPT_SEGEMENT|NAI_COPT_ARG0|NAI_COPT_ARG1, 
        nai_offsetof(nai_httpc_client_t, attr.agent), 0 },
};


static nai_httpc_module_t nai_httpc_client_module_ext = {
    0,                                  /* init */
    0,                                  /* main alloc */
    0, 0,                               /* agent alloc */
    0, 0,                               /* server alloc */
    0, 0,                               /* location alloc */
};


static nai_module_t nai_httpc_client_module = {
    NAI_MODULE_HEADER,
    NAI_MODULE_HTTPC, 
    nai_httpc_client_commands, 
    "httpc_client", &nai_httpc_client_module_ext, {
        0, 
        nai_httpc_client_prepare, 
        0, 
    },
};


static nai_sult_t nai_httpc_client_prepare(nai_main_t* m)
{
    nai_sult_t rc;
    nai_httpc_agent_t* a;
    nai_httpc_client_t* c;


    a = nai_httpc_agent_get(m, "default");
    if (a == 0) {
        rc = nai_sult_from_errno();
        nai_log_error(NAI_LOG_HTTPC, 
            nai_sult_to_errno(rc), "nai_httpc_agent_get failed");
        goto _end;
    };

    c = nai_httpc_get_service_conf(a, nai_httpc_client_module);
    if (c == 0) {
        rc = nai_errno_to_sult(EINVAL);
        nai_log_error(NAI_LOG_HTTPC, 
            nai_sult_to_errno(rc), "nai_httpc_get_service_conf returned null");
        goto _end;
    };

    if (c->attr.dns == 0) {
        c->attr.dns = nai_main_dns_add(m, "default", 0);
        if (c->attr.dns == 0) {
            rc = nai_sult_from_errno();
            nai_log_error(NAI_LOG_HTTPC, 
                nai_sult_to_errno(rc), "nai_main_agent_add failed");
            goto _end;
        };
    };
    if (c->attr.agent == 0) {
        c->attr.agent = nai_main_agent_add(m, "default", 0);
        if (c->attr.agent == 0) {
            rc = nai_sult_from_errno();
            nai_log_error(NAI_LOG_HTTPC, 
                nai_sult_to_errno(rc), "nai_main_agent_add failed");
            goto _end;
        };
    };

    rc = 0;

_end:
    return rc;
};


static nai_int_t nai_httpc_client_conf_init(nai_httpc_client_t* c)
{
    nai_int_t r;
    nai_int_t ec;
    nai_main_t* m;
    nai_httpc_agent_t* a;


    m = (nai_main_t*)nai_malloc(sizeof(*m));
    if (m == 0) {
        r = -1;
        goto _end;
    };

    r = nai_main_init(m);
    if (r < 0) {
        goto _fail;
    };

    r = nai_main_add_module(m, &nai_httpc_client_module);
    if (r < 0) {
        goto _fail;
    };

    r = nai_main_set_optv(m, "httpc");
    if (r < 0) {
        goto _fail;
    };

    r = nai_main_set_opts(m, "agent", "default");
    if (r < 0) {
        goto _fail;
    };

    r = nai_main_lock_segment(m);
    if (r < 0) {
        goto _fail;
    };

    a = nai_httpc_agent_get(m, "default");
    if (a == 0) {
        goto _fail;
    };

    nai_httpc_get_service_conf(a, nai_httpc_client_module) = c;
    c->main = m;

_end:
    return r;

_fail:
    ec = nai_errno;
    nai_main_close(m);
    nai_free(m);
    nai_errno = ec;
    goto _end;
};


nai_int_t nai_httpc_client_set_root(
    nai_httpc_client_t* c, const char* root, const char* conf)
{
    nai_int_t r;


    if (c->main == 0) {
        r = nai_httpc_client_conf_init(c);
        if (r < 0) {
            goto _end;
        };
    };

    r = nai_main_set_root(c->main, root, conf);

_end:
    return r;
};


nai_int_t nai_httpc_client_set_optv(
    nai_httpc_client_t* c, const char* name)
{
    nai_int_t r;


    if (c->main == 0) {
        r = nai_httpc_client_conf_init(c);
        if (r < 0) {
            goto _end;
        };
    };

    r = nai_main_set_optv(c->main, name);

_end:
    return r;
};


nai_int_t nai_httpc_client_set_opti(
    nai_httpc_client_t* c, const char* name, intptr_t v)
{
    nai_int_t r;


    if (c->main == 0) {
        r = nai_httpc_client_conf_init(c);
        if (r < 0) {
            goto _end;
        };
    };

    r = nai_main_set_opti(c->main, name, v);

_end:
    return r;
};


nai_int_t nai_httpc_client_set_optf(
    nai_httpc_client_t* c, const char* name, float v)
{
    nai_int_t r;


    if (c->main == 0) {
        r = nai_httpc_client_conf_init(c);
        if (r < 0) {
            goto _end;
        };
    };

    r = nai_main_set_optf(c->main, name, v);

_end:
    return r;
};


nai_int_t nai_httpc_client_set_optd(
    nai_httpc_client_t* c, const char* name, double v)
{
    nai_int_t r;


    if (c->main == 0) {
        r = nai_httpc_client_conf_init(c);
        if (r < 0) {
            goto _end;
        };
    };

    r = nai_main_set_optd(c->main, name, v);

_end:
    return r;
};


nai_int_t nai_httpc_client_set_opts(
    nai_httpc_client_t* c, const char* name, const char* str)
{
    nai_int_t r;


    if (c->main == 0) {
        r = nai_httpc_client_conf_init(c);
        if (r < 0) {
            goto _end;
        };
    };

    r = nai_main_set_opts(c->main, name, str);

_end:
    return r;
};


nai_int_t nai_httpc_client_set_optm(
    nai_httpc_client_t* c, const char* name, const char* str)
{
    nai_int_t r;


    if (c->main == 0) {
        r = nai_httpc_client_conf_init(c);
        if (r < 0) {
            goto _end;
        };
    };

    r = nai_main_set_optm(c->main, name, str);

_end:
    return r;
};


nai_int_t nai_httpc_client_set_line(
    nai_httpc_client_t* c, const char* str, size_t size)
{
    nai_int_t r;


    if (c->main == 0) {
        r = nai_httpc_client_conf_init(c);
        if (r < 0) {
            goto _end;
        };
    };

    r = nai_main_set_line(c->main, str, size);

_end:
    return r;
};


nai_int_t nai_httpc_client_set_fmts(
    nai_httpc_client_t* c, const char* fmt, ...)
{
    nai_int_t r;
    va_list va;


    if (c->main == 0) {
        r = nai_httpc_client_conf_init(c);
        if (r < 0) {
            goto _end;
        };
    };

    va_start(va, fmt);
    r = nai_main_set_fmtv(c->main, fmt, va);
    va_end(va);

_end:
    return r;
};


nai_int_t nai_httpc_client_set_block(
    nai_httpc_client_t* c, const char* str, size_t size)
{
    nai_int_t r;


    if (c->main == 0) {
        r = nai_httpc_client_conf_init(c);
        if (r < 0) {
            goto _end;
        };
    };

    r = nai_main_set_block(c->main, str, size);

_end:
    return r;
};


nai_int_t nai_httpc_client_open(
    nai_httpc_client_t* c, nai_evloop_t* l)
{
    nai_int_t r;
    nai_main_t* m;
    nai_main_event_t* e;


    m = c->main;
    if (c->agent == 0) {

        if (m == 0) {
            r = nai_httpc_client_conf_init(c);
            if (r < 0) {
                goto _end;
            };
        };

        m = c->main;
        r = nai_main_unlock_segment(m);
        if (r < 0) {
            goto _end;
        };

        /* end of agent */
        r = nai_main_set_optv(m, "end");
        if (r < 0) {
            goto _end;
        };

        /* end of httpc */
        r = nai_main_set_optv(m, "end");
        if (r < 0) {
            goto _end;
        };

        c->agent = nai_httpc_agent_get(m, "default");
        if (c->agent == 0) {
            goto _end;
        };
    };

    if (l != 0) {
        e = nai_main_event_get(m);
        if (e->loop == 0) {
            e->loop = l;
        };
    };

    /* start */
    r = nai_main_open(m);

_end:
    return r;
};


nai_int_t nai_httpc_client_reuse(nai_httpc_client_t* c)
{
    nai_int_t r;
    nai_main_t* m;


    m = c->main;
    if (m != 0) {
        r = nai_main_reuse(m);
        if (r < 0) {
            goto _end;
        };
    };

    r = 0;

_end:
    return r;
};


nai_int_t nai_httpc_client_close(nai_httpc_client_t* c)
{
    nai_int_t r;
    nai_main_t* m;


    m = c->main;
    if (m != 0) {
        r = nai_main_close(m);
        if (r < 0) {
            goto _end;
        };

        nai_free(m);
        c->main = 0;
    };

    c->agent = 0;
    c->attr.dns = 0;
    c->attr.agent = 0;
    r = 0;

_end:
    return r;
};


nai_httpc_client_t* nai_httpc_client_from_main(nai_main_t* m)
{
    nai_httpc_agent_t* a;
    nai_httpc_client_t* c;


    a = nai_httpc_agent_get(m, "default");
    if (a == 0) {
        c = 0;
        goto _end;
    };

    c = nai_httpc_get_service_conf(a, nai_httpc_client_module);

_end:
    return c;
};


