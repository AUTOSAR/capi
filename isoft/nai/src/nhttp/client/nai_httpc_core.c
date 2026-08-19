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
/// @file       nai_httpc_core.c
/// @brief      
/// @details
/// @date       2021-08-19
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_log.h"
#include "nai/runtime/nai_uri.h"
#include "nai/runtime/nai_string.h"
#include "nai/runtime/nai_strmatch.h"
#include "nai/service/nai_command.h"
#include "nai/service/nai_chain.h"
#include "nai/service/nai_main.h"
#include "nai/service/nai_main_event.h"
#include "nai/service/nai_main_agent.h"
#include "nai/service/nai_main_dns.h"
#include "nai_httpc_core.h"
#include <stdlib.h>



//////////////////////////////////////////////////////////////////////////////
// httpc core module


static nai_sult_t nai_httpc_core_preproc(nai_httpc_t* h);
static nai_sult_t nai_httpc_core_postproc(nai_httpc_t* h);
static nai_sult_t nai_httpc_core_stage_init(nai_httpc_t* h);
static nai_sult_t nai_httpc_core_agent_merge(
    nai_httpc_local_ctx_t* c, void* s, void* up);
static nai_sult_t nai_httpc_core_server_merge(
    nai_httpc_local_ctx_t* c, void* s, void* up);
static nai_sult_t nai_httpc_core_location_merge(
    nai_httpc_local_ctx_t* c, void* s, void* up);


static nai_sult_t nai_httpc_server_segment(
    nai_command_t* c, nai_command_args_t* args);
static nai_sult_t nai_httpc_server_name(
    nai_command_t* c, nai_command_args_t* args);
static nai_sult_t nai_httpc_location_segment(
    nai_command_t* c, nai_command_args_t* args);
static nai_sult_t nai_httpc_limit_except(
    nai_command_t* c, nai_command_args_t* args);
static nai_sult_t nai_httpc_internal(
    nai_command_t* c, nai_command_args_t* args);



static nai_command_optinfo_t nai_httpc_option_3state[] = {
    { nai_strconst("off"),  NAI_HTTPC_OFF },
    { nai_strconst("on"),   NAI_HTTPC_ON }, 
    { nai_strconst("always"), NAI_HTTPC_ALWAYS }, 
    { nai_strnull(), 0 }
};

static nai_command_optinfo_t nai_httpc_option_satisfy[] = {
    { nai_strconst("off"),  NAI_HTTPC_OFF },
    { nai_strconst("on"),   NAI_HTTPC_ON }, 
    { nai_strconst("all"),  NAI_HTTPC_ALWAYS }, 
    { nai_strnull(), 0 }
};

static struct {
    const char* name;
    nai_int_t method;
} nai_httpc_methods[] = {
    { "GET",       NAI_HTTP_GET },
    { "HEAD",      NAI_HTTP_HEAD },
    { "POST",      NAI_HTTP_POST },
    { "PUT",       NAI_HTTP_PUT },
    { "DELETE",    NAI_HTTP_DELETE },
    { "MKCOL",     NAI_HTTP_MKCOL },
    { "COPY",      NAI_HTTP_COPY },
    { "MOVE",      NAI_HTTP_MOVE },
    { "OPTIONS",   NAI_HTTP_OPTIONS },
    { "PROPFIND",  NAI_HTTP_PROPFIND },
    { "PROPPATCH", NAI_HTTP_PROPPATCH },
    { "LOCK",      NAI_HTTP_LOCK },
    { "UNLOCK",    NAI_HTTP_UNLOCK },
    { "PATCH",     NAI_HTTP_PATCH },
};



static nai_command_t nai_httpc_core_commands[] = {

    { "agent", nai_httpc_agent_command, 
        NAI_HTTPC_CLOC_MAIN|NAI_HTTPC_COPT_MAIN|
        NAI_COPT_SEGEMENT|NAI_COPT_ARG0, 0, 0 },
    { "min_connections", nai_command_set_int32, 
        NAI_HTTPC_CLOC_MAGENT|
        NAI_HTTPC_COPT_AGENT|NAI_COPT_ARG1, 
        nai_offsetof(nai_httpc_agent_t, min_connections), 
        NAI_OPT_INT_MIN(0) },
    { "min_free_connections", nai_command_set_int32, 
        NAI_HTTPC_CLOC_MAGENT|
        NAI_HTTPC_COPT_AGENT|NAI_COPT_ARG1, 
        nai_offsetof(nai_httpc_agent_t, min_free_connections), 
        NAI_OPT_INT_MIN(0) },

    { "server", nai_httpc_server_segment, 
        NAI_HTTPC_CLOC_MAGENT|NAI_HTTPC_COPT_AGENT|
        NAI_COPT_SEGEMENT|NAI_COPT_ARG0, 0, 0 },
    { "server_name", nai_httpc_server_name, 
        NAI_HTTPC_CLOC_MAGENT|NAI_HTTPC_CLOC_SRV|
        NAI_HTTPC_COPT_SRV|NAI_COPT_ARG1MORE, 0, 0 },
    { "opt_dns", nai_main_dns_command, 
        NAI_HTTPC_CLOC_MAGENT|NAI_HTTPC_CLOC_SRV|
        NAI_HTTPC_COPT_SRV|NAI_COPT_ARG0|NAI_COPT_ARG1, 
        nai_offsetof(nai_httpc_server_t, conn.dns), 0 },
    { "opt_agent", nai_main_agent_command, 
        NAI_HTTPC_CLOC_MAGENT|NAI_HTTPC_CLOC_SRV|
        NAI_HTTPC_COPT_SRV|NAI_COPT_ARG0|NAI_COPT_ARG1, 
        nai_offsetof(nai_httpc_server_t, conn.agent), 0 },
    { "max_request_line", nai_command_set_size, 
        NAI_HTTPC_CLOC_MAGENT|NAI_HTTPC_CLOC_SRV|
        NAI_HTTPC_COPT_SRV|NAI_COPT_ARG1,
        nai_offsetof(nai_httpc_server_t, min_header_buffer), 0 },
    { "max_header_line", nai_command_set_bufsize, 
        NAI_HTTPC_CLOC_MAGENT|NAI_HTTPC_CLOC_SRV|
        NAI_HTTPC_COPT_SRV|NAI_COPT_SEGEMENT|NAI_COPT_ARG1,
        nai_offsetof(nai_httpc_server_t, max_header_buffers), 0 },
    { "max_headers", nai_command_set_size, 
        NAI_HTTPC_CLOC_MAGENT|NAI_HTTPC_CLOC_SRV|
        NAI_HTTPC_COPT_SRV|NAI_COPT_ARG1,
        nai_offsetof(nai_httpc_server_t, max_headers), 0 },
    { "send_so_bufsize", nai_command_set_size, 
        NAI_HTTPC_CLOC_MAGENT|NAI_HTTPC_CLOC_SRV|
        NAI_HTTPC_COPT_SRV|NAI_COPT_ARG1,
        nai_offsetof(nai_httpc_server_t, send_so_bufsize), 0 },
    { "read_so_bufsize", nai_command_set_size, 
        NAI_HTTPC_CLOC_MAGENT|NAI_HTTPC_CLOC_SRV|
        NAI_HTTPC_COPT_SRV|NAI_COPT_ARG1,
        nai_offsetof(nai_httpc_server_t, read_so_bufsize), 0 },

    { "location", nai_httpc_location_segment, 
        NAI_HTTPC_CLOC_MAGENT|NAI_HTTPC_CLOC_SRV|
        NAI_HTTPC_COPT_SRV|NAI_COPT_SEGEMENT|NAI_COPT_ARG1, 0, 0 },
    { "limit_except", nai_httpc_limit_except, 
        NAI_HTTPC_CLOC_LOC|NAI_HTTPC_COPT_LOC|
        NAI_COPT_SEGEMENT|NAI_COPT_ARG1MORE, 0, 0}, 
    { "user_agent", nai_httpc_command_expn, 
        NAI_HTTPC_CLOC_MAGENT|NAI_HTTPC_CLOC_SRV|NAI_HTTPC_CLOC_LOC|
        NAI_HTTPC_COPT_LOC|NAI_COPT_ARG1,
        nai_offsetof(nai_httpc_location_t, user_agent), 0 },
    { "accept", nai_httpc_command_expn, 
        NAI_HTTPC_CLOC_MAGENT|NAI_HTTPC_CLOC_SRV|NAI_HTTPC_CLOC_LOC|
        NAI_HTTPC_COPT_LOC|NAI_COPT_ARG1,
        nai_offsetof(nai_httpc_location_t, accept), 0 },
    { "accept_encoding", nai_httpc_command_expn, 
        NAI_HTTPC_CLOC_MAGENT|NAI_HTTPC_CLOC_SRV|NAI_HTTPC_CLOC_LOC|
        NAI_HTTPC_COPT_LOC|NAI_COPT_ARG1,
        nai_offsetof(nai_httpc_location_t, accept_encoding), 0 },
    { "accept_language", nai_httpc_command_expn, 
        NAI_HTTPC_CLOC_MAGENT|NAI_HTTPC_CLOC_SRV|NAI_HTTPC_CLOC_LOC|
        NAI_HTTPC_COPT_LOC|NAI_COPT_ARG1,
        nai_offsetof(nai_httpc_location_t, accept_language), 0 },
    { "max_body_size", nai_command_set_off, 
        NAI_HTTPC_CLOC_MAGENT|NAI_HTTPC_CLOC_SRV|NAI_HTTPC_CLOC_LOC|
        NAI_HTTPC_COPT_LOC|NAI_COPT_ARG1,
        nai_offsetof(nai_httpc_location_t, max_body_size), 0 },
    { "directio", nai_command_set_off, 
        NAI_HTTPC_CLOC_MAGENT|NAI_HTTPC_CLOC_SRV|NAI_HTTPC_CLOC_LOC|
        NAI_HTTPC_COPT_LOC|NAI_COPT_ARG1,
        nai_offsetof(nai_httpc_location_t, directio), 0 },
    { "directio_align", nai_command_set_off, 
        NAI_HTTPC_CLOC_MAGENT|NAI_HTTPC_CLOC_SRV|NAI_HTTPC_CLOC_LOC|
        NAI_HTTPC_COPT_LOC|NAI_COPT_ARG1,
        nai_offsetof(nai_httpc_location_t, directio_align), 0 },
    { "send_max_chunk", nai_command_set_size, 
        NAI_HTTPC_CLOC_MAGENT|NAI_HTTPC_CLOC_SRV|NAI_HTTPC_CLOC_LOC|
        NAI_HTTPC_COPT_LOC|NAI_COPT_ARG1,
        nai_offsetof(nai_httpc_location_t, read_chunk_size), 0 },
    { "send_max_chunk", nai_command_set_size, 
        NAI_HTTPC_CLOC_MAGENT|NAI_HTTPC_CLOC_SRV|NAI_HTTPC_CLOC_LOC|
        NAI_HTTPC_COPT_LOC|NAI_COPT_ARG1,
        nai_offsetof(nai_httpc_location_t, send_chunk_size), 0 },
    { "send_lowat", nai_command_set_size, 
        NAI_HTTPC_CLOC_MAGENT|NAI_HTTPC_CLOC_SRV|NAI_HTTPC_CLOC_LOC|
        NAI_HTTPC_COPT_LOC|NAI_COPT_ARG1,
        nai_offsetof(nai_httpc_location_t, send_lowat), 0 },
    { "connect_timeout", nai_command_set_msec, 
        NAI_HTTPC_CLOC_MAGENT|NAI_HTTPC_CLOC_SRV|NAI_HTTPC_CLOC_LOC|
        NAI_HTTPC_COPT_LOC|NAI_COPT_ARG1,
        nai_offsetof(nai_httpc_location_t, connect_timeo), 0 },
    { "send_timeout", nai_command_set_msec, 
        NAI_HTTPC_CLOC_MAGENT|NAI_HTTPC_CLOC_SRV|NAI_HTTPC_CLOC_LOC|
        NAI_HTTPC_COPT_LOC|NAI_COPT_ARG1,
        nai_offsetof(nai_httpc_location_t, send_timeo), 0 },
    { "read_head_timeout", nai_command_set_msec, 
        NAI_HTTPC_CLOC_MAGENT|NAI_HTTPC_CLOC_LOC|
        NAI_HTTPC_COPT_LOC|NAI_COPT_ARG1,
        nai_offsetof(nai_httpc_location_t, read_head_timeo), 0 },
    { "read_body_timeout", nai_command_set_msec, 
        NAI_HTTPC_CLOC_MAGENT|NAI_HTTPC_CLOC_SRV|NAI_HTTPC_CLOC_LOC|
        NAI_HTTPC_COPT_LOC|NAI_COPT_ARG1,
        nai_offsetof(nai_httpc_location_t, read_body_timeo), 0 },
    { "keepalive_timeout", nai_command_set_msec, 
        NAI_HTTPC_CLOC_MAGENT|NAI_HTTPC_CLOC_SRV|NAI_HTTPC_CLOC_LOC|
        NAI_HTTPC_COPT_SRV|NAI_COPT_ARG1,
        nai_offsetof(nai_httpc_location_t, keepalive_timeo), 0 },
    { "lingering_timeout", nai_command_set_msec, 
        NAI_HTTPC_CLOC_MAGENT|NAI_HTTPC_CLOC_SRV|NAI_HTTPC_CLOC_LOC|
        NAI_HTTPC_COPT_LOC|NAI_COPT_ARG1,
        nai_offsetof(nai_httpc_location_t, lingering_timeo), 0 },
    { "lingering_time", nai_command_set_msec, 
        NAI_HTTPC_CLOC_MAGENT|NAI_HTTPC_CLOC_SRV|NAI_HTTPC_CLOC_LOC|
        NAI_HTTPC_COPT_LOC|NAI_COPT_ARG1,
        nai_offsetof(nai_httpc_location_t, lingering_timeomax), 0 },
    { "keepalive_max", nai_command_set_int32, 
        NAI_HTTPC_CLOC_MAGENT|NAI_HTTPC_CLOC_SRV|NAI_HTTPC_CLOC_LOC|
        NAI_HTTPC_COPT_LOC|NAI_COPT_ARG1,
        nai_offsetof(nai_httpc_location_t, lingering_close), 
        NAI_OPT_INT_MIN(0) },
    { "lingering_close", nai_command_set_opts, 
        NAI_HTTPC_CLOC_MAGENT|NAI_HTTPC_CLOC_SRV|NAI_HTTPC_CLOC_LOC|
        NAI_HTTPC_COPT_LOC|NAI_COPT_ARG1,
        nai_offsetof(nai_httpc_location_t, lingering_close), 
        nai_httpc_option_3state },
    { "sendfile", nai_command_set_opts, 
        NAI_HTTPC_CLOC_MAGENT|NAI_HTTPC_CLOC_SRV|NAI_HTTPC_CLOC_LOC|
        NAI_HTTPC_COPT_LOC|NAI_COPT_ARG1,
        nai_offsetof(nai_httpc_location_t, sendfile), 0 },
    { "aio", nai_command_set_opts, 
        NAI_HTTPC_CLOC_MAGENT|NAI_HTTPC_CLOC_SRV|NAI_HTTPC_CLOC_LOC|
        NAI_HTTPC_COPT_LOC|NAI_COPT_ARG1,
        nai_offsetof(nai_httpc_location_t, aio), 0 },
    { "tcp_nopush", nai_command_set_opts, 
        NAI_HTTPC_CLOC_MAGENT|NAI_HTTPC_CLOC_SRV|NAI_HTTPC_CLOC_LOC|
        NAI_HTTPC_COPT_LOC|NAI_COPT_SEGEMENT|NAI_COPT_ARG1,
        nai_offsetof(nai_httpc_location_t, tcp_nopush), 0 },
    { "tcp_nodelay", nai_command_set_opts, 
        NAI_HTTPC_CLOC_MAGENT|NAI_HTTPC_CLOC_SRV|NAI_HTTPC_CLOC_LOC|
        NAI_HTTPC_COPT_LOC|NAI_COPT_ARG1,
        nai_offsetof(nai_httpc_location_t, tcp_nodelay), 0 },
    { "etag", nai_command_set_opts, 
        NAI_HTTPC_CLOC_MAGENT|NAI_HTTPC_CLOC_SRV|NAI_HTTPC_CLOC_LOC|
        NAI_HTTPC_COPT_LOC|NAI_COPT_ARG1,
        nai_offsetof(nai_httpc_location_t, etag), 0 },
    { "chunked", nai_command_set_opts, 
        NAI_HTTPC_CLOC_MAGENT|NAI_HTTPC_CLOC_SRV|NAI_HTTPC_CLOC_LOC|
        NAI_HTTPC_COPT_LOC|NAI_COPT_ARG1,
        nai_offsetof(nai_httpc_location_t, chunked), 0 },
    { "satisfy", nai_command_set_opts, 
        NAI_HTTPC_CLOC_MAGENT|NAI_HTTPC_CLOC_SRV|NAI_HTTPC_CLOC_LOC|
        NAI_HTTPC_COPT_LOC|NAI_COPT_ARG1,
        nai_offsetof(nai_httpc_location_t, satisfy), 
        nai_httpc_option_satisfy },
    { "internal", nai_httpc_internal, 
        NAI_HTTPC_CLOC_LOC|NAI_HTTPC_COPT_LOC,
        nai_offsetof(nai_httpc_location_t, internal), 0 },
    { 0 }
};


static nai_httpc_module_t nai_httpc_core_module_ext = {
    nai_httpc_core_preproc,             /* pre-processing */
    nai_httpc_core_postproc,            /* post-processing */
    0,                                  /* main alloc */
    0, nai_httpc_core_agent_merge,      /* agent alloc */
    0, nai_httpc_core_server_merge,     /* server alloc */
    0, nai_httpc_core_location_merge,   /* location alloc */
};


nai_module_t nai_httpc_core_module = {
    NAI_MODULE_HEADER,
    NAI_MODULE_HTTPC, 
    nai_httpc_core_commands, 
    "httpc_core", &nai_httpc_core_module_ext, {
        0, 
    },
};


extern nai_sult_t nai_httpc_header_tables_init(nai_httpc_t* h);
extern nai_sult_t nai_httpc_core_chain_init(nai_httpc_t* h);
extern nai_sult_t nai_httpc_core_variable_init(nai_httpc_t* h);


static nai_sult_t nai_httpc_core_preproc(nai_httpc_t* h)
{
    nai_sult_t rc;


    /* init core variables */
    rc = nai_httpc_core_variable_init(h);
    if (rc < 0) {
        nai_log_alert(NAI_LOG_HTTPC, 
            nai_sult_to_errno(rc), 
            "init core variables of httpc failed");
        goto _end;
    };


_end:
    return rc;
};


static nai_sult_t nai_httpc_core_postproc(nai_httpc_t* h)
{
    nai_int_t n;
    nai_sult_t rc;
    nai_list_entry_t* e;
    nai_list_entry_t* list;
    nai_httpc_agent_t* d;
    nai_httpc_agent_t* a;


    /* init global header tables */
    rc = nai_httpc_header_tables_init(h);
    if (rc < 0) {
        nai_log_alert(NAI_LOG_HTTPC, 
            nai_sult_to_errno(rc), 
            "init global header tables of httpc failed");
        goto _end;
    };


    /* init core stages */
    rc = nai_httpc_core_stage_init(h);
    if (rc < 0) {
        nai_log_alert(NAI_LOG_HTTPC, 
            nai_sult_to_errno(rc), 
            "init core stages of httpc failed");
        goto _end;
    };


    /* init core chains */
    rc = nai_httpc_core_chain_init(h);
    if (rc < 0) {
        nai_log_alert(NAI_LOG_HTTPC, 
            nai_sult_to_errno(rc), 
            "init core chains of httpc failed");
        goto _end;
    };

    /* process configs */
    d = nai_httpc_agent_get(h->main, "default");
    for (n = 0; n < (nai_int_t)nai_countof(h->list); n ++) {
        list = &h->list[n];
        e = list->next;
        for ( ; e != list; e = e->next) {
            a = (nai_httpc_agent_t*)e;
            rc = nai_httpc_agent_merge(a, a == d ? 0 : d, 0, 0);
            if (rc < 0) {
                goto _end;
            };
        };
    };

    rc = 0;

_end:
    return rc;
};


static nai_sult_t nai_httpc_core_agent_merge(
    nai_httpc_local_ctx_t* c, void* s, void* up)
{
    nai_httpc_agent_t* conf = (nai_httpc_agent_t*)s;
    nai_httpc_agent_t* parent = (nai_httpc_agent_t*)up;


    (void)c;

    if (parent == 0) {
        if (conf->min_connections == NAI_OPT_UNSET_VALUE) {
            conf->min_connections = 10;
        };
        if (conf->min_free_connections == NAI_OPT_UNSET_VALUE) {
            conf->min_free_connections = 10;
        };
    } else {
        if (conf->min_connections == NAI_OPT_UNSET_VALUE) {
            conf->min_connections = parent->min_connections;
        };
        if (conf->min_free_connections == NAI_OPT_UNSET_VALUE) {
            conf->min_free_connections = parent->min_free_connections;;
        };
    };

    return 0;
};


static nai_sult_t nai_httpc_core_server_merge(
    nai_httpc_local_ctx_t* c, void* s, void* up)
{
    nai_sult_t rc;
    nai_httpc_client_t* cl;
    nai_httpc_server_t* conf = (nai_httpc_server_t*)s;
    nai_httpc_server_t* parent = (nai_httpc_server_t*)up;


    (void)c;

    if (parent == 0) {
        cl = nai_httpc_client_from_main(c->service->main);

        if (conf->version == NAI_OPT_UNSET_VALUE) {
            conf->version = NAI_HTTP_VERSION(1, 1);
        };
        if (conf->max_header_buffers.num == NAI_OPT_UNSET_SIZE) {
            conf->max_header_buffers.num = 4;
            conf->max_header_buffers.size = 8192;
        };
        if (conf->min_header_buffer == NAI_OPT_UNSET_SIZE) {
            conf->min_header_buffer = 1024;
        };
        if (conf->max_headers == NAI_OPT_UNSET_SIZE) {
            conf->max_headers = 128;
        };
        if (conf->send_so_bufsize == NAI_OPT_UNSET_SIZE) {
            conf->send_so_bufsize = 0;
        };
        if (conf->read_so_bufsize == NAI_OPT_UNSET_SIZE) {
            conf->read_so_bufsize = 0;
        };
        if (conf->conn.agent == 0) {
            if (cl) {
                conf->conn.agent = cl->attr.agent;
            } else {
                conf->conn.agent = 
                    nai_main_agent_add(c->service->main, "default", 0);
                if (conf->conn.agent == 0) {
                    rc = nai_sult_from_errno();
                    nai_log_error(NAI_LOG_HTTPC, 
                        nai_sult_to_errno(rc), "nai_main_agent_add failed");
                    goto _end;
                };
            };
        };
        if (conf->conn.dns == 0) {
            if (cl) {
                conf->conn.dns = cl->attr.dns;
            } else {
                conf->conn.dns = 
                    nai_main_dns_add(c->service->main, "default", 0);
                if (conf->conn.dns == 0) {
                    rc = nai_sult_from_errno();
                    nai_log_error(NAI_LOG_HTTPC, 
                        nai_sult_to_errno(rc), "nai_main_dns_add failed");
                    goto _end;
                };
            };
        };
    } else {
        if (conf->version == NAI_OPT_UNSET_VALUE) {
            conf->version = parent->version;
        };
        if (conf->max_header_buffers.num == NAI_OPT_UNSET_SIZE) {
            conf->max_header_buffers.num = parent->max_header_buffers.num;
            conf->max_header_buffers.size = parent->max_header_buffers.size;
        };
        if (conf->min_header_buffer == NAI_OPT_UNSET_SIZE) {
            conf->min_header_buffer = parent->min_header_buffer;
        };
        if (conf->max_headers == NAI_OPT_UNSET_SIZE) {
            conf->max_headers = parent->max_headers;
        };
        if (conf->send_so_bufsize == NAI_OPT_UNSET_SIZE) {
            conf->send_so_bufsize = parent->send_so_bufsize;
        };
        if (conf->read_so_bufsize == NAI_OPT_UNSET_SIZE) {
            conf->read_so_bufsize = parent->read_so_bufsize;
        };
        if (conf->conn.agent == 0) {
            conf->conn.agent = parent->conn.agent;
        };
        if (conf->conn.dns == 0) {
            conf->conn.dns = parent->conn.dns;
        };
    };

    rc = 0;

_end:
    return rc;
};


static nai_sult_t nai_httpc_core_location_merge(
    nai_httpc_local_ctx_t* c, void* s, void* up)
{
    nai_sult_t rc;
    nai_httpc_location_t* conf = (nai_httpc_location_t*)s;
    nai_httpc_location_t* parent = (nai_httpc_location_t*)up;


    (void)c;

    if (parent == 0) {
        if (conf->max_body_size == NAI_OPT_UNSET_VALUE) {
            conf->max_body_size = 10*1024*1024;
        };
        if (conf->max_body_size == NAI_OPT_UNSET_VALUE) {
            conf->max_body_size = 10*1024*1024;
        };
        if (conf->directio == NAI_OPT_UNSET_VALUE) {
            conf->directio = NAI_OFF_T_MAX;
        };
        if (conf->directio_align == NAI_OPT_UNSET_VALUE) {
            conf->directio_align = 512;
        };
        if (conf->read_chunk_size == NAI_OPT_UNSET_SIZE) {
            conf->read_chunk_size = 4*1024;
        };
        if (conf->send_chunk_size == NAI_OPT_UNSET_SIZE) {
            conf->send_chunk_size = 128*1024;
        };
        if (conf->send_lowat == NAI_OPT_UNSET_SIZE) {
            conf->send_lowat = 0;
        };
        if (conf->connect_timeo == NAI_OPT_UNSET_MSEC) {
            conf->connect_timeo = 30*1000;
        };
        if (conf->send_timeo == NAI_OPT_UNSET_MSEC) {
            conf->send_timeo = 60*1000;
        };
        if (conf->read_head_timeo == NAI_OPT_UNSET_MSEC) {
            conf->read_head_timeo = 60*1000;
        };
        if (conf->read_body_timeo == NAI_OPT_UNSET_MSEC) {
            conf->read_body_timeo = 60*1000;
        };
        if (conf->keepalive_timeo == NAI_OPT_UNSET_MSEC) {
            conf->keepalive_timeo = 60*1000;
        };
        if (conf->lingering_timeo == NAI_OPT_UNSET_MSEC) {
            conf->lingering_timeo = 5*1000;
        };
        if (conf->lingering_timeomax == NAI_OPT_UNSET_MSEC) {
            conf->lingering_timeomax = 30*1000;
        };
        if (conf->keepalive_max == NAI_OPT_UNSET_VALUE) {
            conf->keepalive_max = 100;
        };
        if (conf->lingering_close == NAI_OPT_UNSET_VALUE) {
            conf->lingering_close = NAI_HTTPC_ALWAYS;
        };
        if (conf->sendfile == NAI_OPT_UNSET_VALUE) {
            conf->sendfile = 1;
        };
        if (conf->aio == NAI_OPT_UNSET_VALUE) {
            conf->aio = 1;
        };
        if (conf->tcp_nopush == NAI_OPT_UNSET_VALUE) {
            conf->tcp_nopush = 0;
        };
        if (conf->tcp_nodelay == NAI_OPT_UNSET_VALUE) {
            conf->tcp_nodelay = 1;
        };
        if (conf->etag == NAI_OPT_UNSET_VALUE) {
            conf->etag = 1;
        };
        if (conf->chunked == NAI_OPT_UNSET_VALUE) {
            conf->chunked = 1;
        };
        if (conf->satisfy == NAI_OPT_UNSET_VALUE) {
            conf->satisfy = NAI_HTTPC_ALWAYS;
        };

    } else {
        if (nai_script_expn_is_null(&conf->user_agent)) {
            conf->user_agent = parent->user_agent;
        };
        if (nai_script_expn_is_null(&conf->accept)) {
            conf->accept = parent->accept;
        };
        if (nai_script_expn_is_null(&conf->accept_encoding)) {
            conf->accept_encoding = parent->accept_encoding;
        };
        if (nai_script_expn_is_null(&conf->accept_language)) {
            conf->accept_language = parent->accept_language;
        };
        if (conf->max_body_size == NAI_OPT_UNSET_VALUE) {
            conf->max_body_size = parent->max_body_size;
        };
        if (conf->directio == NAI_OPT_UNSET_VALUE) {
            conf->directio = parent->directio;
        };
        if (conf->directio_align == NAI_OPT_UNSET_VALUE) {
            conf->directio_align = parent->directio_align;
        };
        if (conf->read_chunk_size == NAI_OPT_UNSET_SIZE) {
            conf->read_chunk_size = parent->read_chunk_size;
        };
        if (conf->send_chunk_size == NAI_OPT_UNSET_SIZE) {
            conf->send_chunk_size = parent->send_chunk_size;
        };
        if (conf->send_lowat == NAI_OPT_UNSET_SIZE) {
            conf->send_lowat = parent->send_lowat;
        };
        if (conf->connect_timeo == NAI_OPT_UNSET_MSEC) {
            conf->connect_timeo = parent->connect_timeo;
        };
        if (conf->send_timeo == NAI_OPT_UNSET_MSEC) {
            conf->send_timeo = parent->send_timeo;
        };
        if (conf->read_head_timeo == NAI_OPT_UNSET_MSEC) {
            conf->read_head_timeo = parent->read_head_timeo;
        };
        if (conf->read_body_timeo == NAI_OPT_UNSET_MSEC) {
            conf->read_body_timeo = parent->read_body_timeo;
        };
        if (conf->keepalive_timeo == NAI_OPT_UNSET_MSEC) {
            conf->keepalive_timeo = parent->keepalive_timeo;
        };
        if (conf->lingering_timeo == NAI_OPT_UNSET_MSEC) {
            conf->lingering_timeo = parent->lingering_timeo;
        };
        if (conf->lingering_timeomax == NAI_OPT_UNSET_MSEC) {
            conf->lingering_timeomax = parent->lingering_timeomax;
        };
        if (conf->keepalive_max == NAI_OPT_UNSET_VALUE) {
            conf->keepalive_max = parent->keepalive_max;
        };
        if (conf->lingering_close == NAI_OPT_UNSET_VALUE) {
            conf->lingering_close = parent->lingering_close;
        };
        if (conf->sendfile == NAI_OPT_UNSET_VALUE) {
            conf->sendfile = parent->sendfile;
        };
        if (conf->aio == NAI_OPT_UNSET_VALUE) {
            conf->aio = parent->aio;
        };
        if (conf->tcp_nopush == NAI_OPT_UNSET_VALUE) {
            conf->tcp_nopush = parent->tcp_nopush;
        };
        if (conf->tcp_nodelay == NAI_OPT_UNSET_VALUE) {
            conf->tcp_nodelay = parent->tcp_nodelay;
        };
        if (conf->etag == NAI_OPT_UNSET_VALUE) {
            conf->etag = parent->etag;
        };
        if (conf->chunked == NAI_OPT_UNSET_VALUE) {
            conf->chunked = parent->chunked;
        };
        if (conf->satisfy == NAI_OPT_UNSET_VALUE) {
            conf->satisfy = parent->satisfy;
        };
    };

    rc = 0;

    return rc;
};


static nai_sult_t nai_httpc_server_segment(
    nai_command_t* c, nai_command_args_t* args)
{
    nai_sult_t rc;
    nai_httpc_t* h;
    nai_httpc_agent_t* a;
    nai_httpc_server_t* s;


    (void)c;

    if (args->endseg) {
        rc = 0;
        goto _end;
    };

    h = nai_httpc_get_service(args);
    if (h == 0) {
        rc = nai_errno_to_sult(NAI_EINTERNAL);
        nai_log_alert(NAI_LOG_HTTPC, 
            nai_sult_to_errno(rc), 
            "get httpc failed, return value is null");
        goto _end;
    };

    a = (nai_httpc_agent_t*)args->data;
    rc = nai_httpc_server_create(h, args->pool, a, &s);
    if (rc < 0) {
        nai_log_alert(NAI_LOG_HTTPC, 
            nai_sult_to_errno(rc), "create new server failed");
        goto _end;
    };

    args->grp = NAI_MODULE_HTTPC;
    args->grp_mask = NAI_HTTPC_CLOC_SRV;
    args->clocal = s->clocal;
    rc = 0;

_end:
    return rc;
};


static nai_sult_t nai_httpc_server_name(
    nai_command_t* c, nai_command_args_t* args)
{
    nai_int_t n;
    nai_sult_t rc;
    nai_str_t str;
    nai_httpc_server_t* s;
    nai_httpc_server_name_t* sn;

#if (NAI_HAVE_REGEX)
    nai_httpc_t* h;
    nai_regex_errinfo_t err;
#endif


    (void)c;

    s = nai_httpc_get_server(args);
    if (s == 0) {
        rc = nai_errno_to_sult(NAI_EINTERNAL);
        nai_log_alert(NAI_LOG_HTTPC, 
            nai_sult_to_errno(rc), 
            "get location failed, return value is null");
        goto _end;
    };

    for (n = 0; n < args->argc; n ++) {
        rc = nai_value_dups(&args->argv[n], &str, args->pool);
        if (rc < 0) {
            nai_log_error(NAI_LOG_HTTPC, 
                nai_sult_to_errno(rc), "get server name failed");
            goto _end;
        };

        /* set default server name */
        if (nai_str_len(&s->name) <= 0) {
            s->name = str;
        };

        sn = (nai_httpc_server_name_t*)nai_array_push(&s->names);
        if (sn == 0) {
            rc = nai_sult_from_errno();
            nai_log_alert(NAI_LOG_HTTPC, 
                nai_sult_to_errno(rc), "push server name failed");
            goto _end;
        };
        sn->server = s;

#if (NAI_HAVE_REGEX)
        if (nai_str_len(&str) > 0 && nai_str(&str)[0] == '~') {
            str.ptr ++;
            str.len --;
            h = nai_httpc_get_service(args);
            rc = nai_httpc_regex_compile(h, &sn->pattern, &str, 0, &err);
            if (rc < 0) {
                nai_log_error(NAI_LOG_HTTPC, 
                    nai_sult_to_errno(rc), "compile server name %s: %s%s%s", 
                    nai_str(&str)+1, err.str,
                    err.offset >= 0 ? ", at " : "", 
                    err.offset >= 0 ? nai_str(&str)+1 + err.offset : "");
                goto _end;
            };

            sn->type = NAI_HTTPC_SERVNAME_REGEX;
        } else 
#endif
        if (nai_strchr(nai_str(&str), '*') || 
            nai_strchr(nai_str(&str), '?')) {
            sn->name = str;
            sn->type = NAI_HTTPC_SERVNAME_WILD;
        } else {
            nai_str_hash(&str, 1);
            sn->name = str;
            sn->type = NAI_HTTPC_SERVNAME_NORM;
        };
    };

    rc = 0;

_end:
    return rc;
};


static nai_sult_t nai_httpc_location_segment(
    nai_command_t* c, nai_command_args_t* args)
{
    nai_int_t n;
    nai_int_t type;
    const char* p;
    nai_sult_t rc;
    nai_str_t str;
    nai_str_t mod;
    nai_httpc_t* h;
    nai_httpc_server_t* s;
    nai_httpc_location_t* u;
    nai_httpc_location_t** pl;
    nai_httpc_location_t** elts;


    (void)c;

    if (args->endseg) {
        rc = 0;
        goto _end;
    };

    if (args->argc == 1) {
        rc = nai_value_dups(&args->argv[0], &str, args->pool);
        if (rc < 0) {
            nai_log_error(NAI_LOG_HTTPC, 
                nai_sult_to_errno(rc), "get location name failed");
            goto _end;
        };
        if (nai_str_len(&str) <= 0) {
            rc = nai_errno_to_sult(EINVAL);
            nai_log_error(NAI_LOG_HTTPC, 
                nai_sult_to_errno(rc), "location name is empty");
            goto _end;
        };

        type = NAI_HTTPC_LOC_NORMAL;
        p = nai_str(&str);
        if (p[0] == '=') {
            str.ptr ++;
            str.len --;
            type = NAI_HTTPC_LOC_EXACT;
#if (NAI_HAVE_REGEX)
        } else if (nai_str_len(&str) >= 2 && p[0] == '~' && p[1] == '*') {
            str.ptr += 2;
            str.len -= 2;
            type = NAI_HTTPC_LOC_PATTERN_CASE;
        } else if (p[0] == '~') {
            str.ptr ++;
            str.len --;
            type = NAI_HTTPC_LOC_PATTERN;
#endif
        } else if (p[0] == '^') {
            str.ptr ++;
            str.len --;
            type = NAI_HTTPC_LOC_PRIORITY;
        } else if (p[0] == '@') {
            type = NAI_HTTPC_LOC_SYMBOL;
        };

    } else {
        rc = nai_value_dups(&args->argv[0], &mod, args->pool_tmp);
        if (rc < 0) {
            nai_log_error(NAI_LOG_HTTPC, 
                nai_sult_to_errno(rc), "get location modify failed");
            goto _end;
        };
        if (nai_str_len(&mod) <= 0) {
            rc = nai_errno_to_sult(EINVAL);
            nai_log_error(NAI_LOG_HTTPC, 
                nai_sult_to_errno(rc), "location modify is empty");
            goto _end;
        };

        rc = nai_value_dups(&args->argv[1], &str, args->pool);
        if (rc < 0) {
            nai_log_error(NAI_LOG_HTTPC, 
                nai_sult_to_errno(rc), "get location name failed");
            goto _end;
        };
        if (nai_str_len(&str) <= 0) {
            rc = nai_errno_to_sult(EINVAL);
            nai_log_error(NAI_LOG_HTTPC, 
                nai_sult_to_errno(rc), "location name is empty");
            goto _end;
        };

        p = nai_str(&mod);
        if (nai_str_len(&mod) == 1 && p[0] == '=') {
            type = NAI_HTTPC_LOC_EXACT;
#if (NAI_HAVE_REGEX)
        } else if (nai_str_len(&mod) == 2 && p[0] == '~' && p[1] == '*') {
            type = NAI_HTTPC_LOC_PATTERN_CASE;
        } else if (nai_str_len(&mod) == 1 && p[0] == '~') {
            type = NAI_HTTPC_LOC_PATTERN;
#endif
        } else if (nai_str_len(&mod) == 1 && p[0] == '^') {
            type = NAI_HTTPC_LOC_PRIORITY;
        } else {
            rc = nai_errno_to_sult(EINVAL);
            nai_log_error(NAI_LOG_HTTPC, 
                nai_sult_to_errno(rc), "invalid location modify");
            goto _end;
        };;
    };

    u = nai_httpc_get_location(args);
    if (u->type != NAI_HTTPC_LOC_ROOT) {
        switch (type) {
        case NAI_HTTPC_LOC_EXACT:
        case NAI_HTTPC_LOC_PATTERN:
        case NAI_HTTPC_LOC_PATTERN_CASE:
        case NAI_HTTPC_LOC_SYMBOL:
            rc = nai_errno_to_sult(EINVAL);
            nai_log_error(NAI_LOG_HTTPC, 
                nai_sult_to_errno(rc), 
                "location %s can be on server level only", 
                nai_str(&str));
            goto _end;
        default:
            if (u->type != NAI_HTTPC_LOC_NORMAL || 
                u->type != NAI_HTTPC_LOC_PRIORITY) {
                rc = nai_errno_to_sult(EINVAL);
                nai_log_error(NAI_LOG_HTTPC, 
                    nai_sult_to_errno(rc), 
                    "location %s cannot have child location", 
                    nai_str(&u->name));
                goto _end;
            };
        };
        if (type == NAI_HTTPC_LOC_NORMAL) {
            if (nai_str_len(&str) <= nai_str_len(&u->name) || 
                nai_strncmp(nai_str(&u->name), 
                nai_str(&str), nai_str_len(&u->name)) != 0) {

                rc = nai_errno_to_sult(EINVAL);
                nai_log_error(NAI_LOG_HTTPC, 
                    nai_sult_to_errno(rc), 
                    "location %s is outside location %s", 
                    nai_str(&str), nai_str(&u->name));
                goto _end;
            };
        };
    };

    s = nai_httpc_get_server(args);
    if (s == 0) {
        rc = nai_errno_to_sult(NAI_EINTERNAL);
        nai_log_alert(NAI_LOG_HTTPC, 
            nai_sult_to_errno(rc), 
            "get location failed, return value is null");
        goto _end;
    };

    elts = (nai_httpc_location_t**)s->table.list.elts;
    for (n = 0; n < (intptr_t)s->table.list.count; n ++) {
        if (nai_str_eq(&str, &elts[n]->name) && type == elts[n]->type) {
            rc = nai_errno_to_sult(EEXIST);
            nai_log_error(NAI_LOG_HTTPC, 
                nai_sult_to_errno(rc), "location '%s' already exists", 
                nai_str(&str));
            goto _end;
        };
    };

    pl = (nai_httpc_location_t**)nai_array_push(&s->table.list);
    if (pl == 0) {
        rc = nai_sult_from_errno();
        nai_log_alert(NAI_LOG_HTTPC, 
            nai_sult_to_errno(rc), "push location slot failed");
        goto _end;
    };

    h = nai_httpc_get_service(args);
    rc = nai_httpc_location_create(h, args->pool, &s->conf, pl);
    if (rc < 0) {
        nai_log_alert(NAI_LOG_HTTPC, 
            nai_sult_to_errno(rc), "create new location failed");
        goto _end;
    };

    pl[0]->name = str;
    pl[0]->type = type;
    pl[0]->parent = u;
    args->grp = NAI_MODULE_HTTPC;
    args->grp_mask = NAI_HTTPC_CLOC_LOC;
    args->clocal = pl[0]->clocal;
    rc = 0;

_end:
    return rc;
};


static nai_sult_t nai_httpc_limit_except(
    nai_command_t* c, nai_command_args_t* args)
{
    nai_int_t n, m;
    nai_sult_t rc;
    nai_mem_t str;
    nai_httpc_t* h;
    nai_httpc_location_t* l;


    (void)c;

    if (args->endseg) {
        rc = 0;
        goto _end;
    };

    l = (nai_httpc_location_t*)args->data;
    l->limit_except = 0;
    for (n = 0; n < args->argc; n ++) {
        rc = nai_value_getm(&args->argv[n], &str);
        if (rc < 0) {
            nai_log_error(NAI_LOG_HTTPC, 
                nai_sult_to_errno(rc), "get method name failed");
            goto _end;
        };

        for (m = 0; m < (nai_int_t)nai_countof(nai_httpc_methods); m ++) {
            if (nai_strncmp(nai_httpc_methods[m].name, 
                nai_str(&str), nai_str_len(&str)) == 0) {
                break;
            };
        };
        if (m >= (nai_int_t)nai_countof(nai_httpc_methods)) {
            rc = nai_errno_to_sult(EINVAL);
            nai_log_error(NAI_LOG_HTTPC, 
                nai_sult_to_errno(rc), "invalid method name '%.*s'", 
                nai_str_len(&str), nai_str(&str));
            goto _end;
        };

        l->limit_except |= (1 << n);
    };

    if (l->limit_except & (1 << NAI_HTTP_GET)) {
        l->limit_except |= (1 << NAI_HTTP_HEAD);
    };

    if (l->limit_conf == 0) {
        h = nai_httpc_get_service(l);
        rc = nai_httpc_location_create(
            h, args->pool, &l->conf, &l->limit_conf);
        if (rc < 0) {
            nai_log_alert(NAI_LOG_HTTPC, 
                nai_sult_to_errno(rc), "create new location failed");
            goto _end;
        };

        l->limit_conf->parent = l;
        l->limit_conf->sibling = l->sibling;
        l->sibling = l->limit_conf;
    };


    args->grp = NAI_MODULE_HTTPC;
    args->grp_mask = NAI_HTTPC_CLOC_LMT;
    args->clocal = l->limit_conf->clocal;

    rc = 0;

_end:
    return rc;
};


static nai_sult_t nai_httpc_internal(
    nai_command_t* c, nai_command_args_t* args)
{
    nai_sult_t rc;
    nai_httpc_location_t* l;


    (void)c;

    l = (nai_httpc_location_t*)args->data;
    l->internal = 1;

    rc = 0;
    return rc;
};


//////////////////////////////////////////////////////////////////////////////
// httpc location


nai_sult_t nai_httpc_server_node(
    nai_httpc_agent_t* a, const nai_mem_t* host, nai_int_t port, 
    nai_httpc_server_node_t** pv)
{
    intptr_t r;
    nai_sult_t rc;
    nai_rbnode_t** n = &nai_rbtree_root(&a->cmap);
    nai_rbnode_t* parent = nai_rbtree_end(&a->cmap);
    nai_httpc_t* h;
    nai_httpc_server_node_t* s;


    while (*n) {
        parent = *n;
        s = nai_containof(parent, nai_httpc_server_node_t, node);

        r = (nai_int_t)s->port - port;
        if (r != 0) {
            if (r > 0) {
                n = &parent->rb_left;
            } else {
                n = &parent->rb_right;
            };
            continue;
        };

        r = nai_str_casecmp(&s->host, host);
        if (r == 0) {
            break;
        } else if (r > 0) {
            n = &parent->rb_left;
        } else {
            n = &parent->rb_right;
        };
    };

    if (n[0] == 0) {
        h = nai_httpc_get_service(a);
        s = nai_palloc(&h->pool, sizeof(*s));
        if (s == 0) {
            rc = nai_sult_from_errno();
            goto _end;
        };

        r = nai_str_dup(&s->host, nai_str(host), nai_str_len(host), &h->pool);
        if (r < 0) {
            rc = nai_sult_from_errno();
            goto _end;
        };

        s->port = port;
        s->version = 0;
        nai_list_init(&s->list);
        nai_rbtree_link(&a->cmap, &s->node, parent, n);
        nai_rbtree_color(&a->cmap, &s->node);
    };

    if (pv) {
        pv[0] = s;
    };
    rc = 0;

_end:
    return rc;
};


nai_sult_t nai_httpc_server_find(
    nai_httpc_request_t* r, const nai_mem_t* name, 
    nai_httpc_server_t** pv)
{
    nai_int_t i;
    nai_sult_t rc;
    nai_httpc_agent_t* a;
    nai_httpc_server_t* s = 0;
    nai_httpc_server_name_t* n;
    nai_httpc_server_name_t** na;
    nai_mem_t str = *name;


    a = nai_httpc_get_agent(r);
    if (a->named.count > 0) {
        nai_str_hash(&str, 1);
        na = (nai_httpc_server_name_t**)a->named.elts;
        for (i = 0; i < (intptr_t)a->named.count; i ++) {
            n = na[i];
            if (nai_str_hashcaseeq(&n->name, &str)) {
                s = n->server;
                goto _find;
            };
        };
    };

    na = (nai_httpc_server_name_t**)a->match.elts;
    for (i = 0; i < (intptr_t)a->match.count; i ++) {
        n = na[i];
        switch (n->type) {
#if (NAI_HAVE_REGEX)
        case NAI_HTTPC_SERVNAME_REGEX:
            if (nai_httpc_regex_exec(r, &n->pattern, &str) == 0) {
                s = n->server;
                goto _find;
            };
            break;
#endif
        case NAI_HTTPC_SERVNAME_WILD:
            if (nai_wildmatch(nai_str(&n->name), 
                nai_str(&str), NAI_WMATCH_CASELESS) == 1) {
                s = n->server;
                goto _find;
            };
            break;
        default:
            break;
        };
    };

    rc = nai_errno_to_sult(ENOENT);

_end:
    return rc;

_find:
    if (pv) {
        pv[0] = s;
    };
    rc = 0;
    goto _end;
};


static nai_sult_t nai_httpc_location_find(
    nai_httpc_request_t* r, 
    const nai_str_t* uri, nai_httpc_location_t** pv)
{
    nai_int_t c;
    nai_int_t cmp;
    nai_int_t len;
    const char* str;
    nai_sult_t rc;
    nai_rbnode_t** e;
    nai_rbnode_t* parent;
    nai_httpc_server_t* s;
    nai_httpc_locations_t* t;
    nai_httpc_location_t* find = 0;
    nai_httpc_location_node_t* node;
#if (NAI_HAVE_REGEX)
    nai_int_t n;
    nai_httpc_location_entry_t* ent;
    nai_httpc_location_entry_t** elts;
#endif


    s = nai_httpc_get_server(r);
    t = &s->table;

    str = nai_str(uri);
    len = nai_str_len(uri);

    e = &nai_rbtree_root(&t->named);
    while (*e) {
        parent = *e;
        node = (nai_httpc_location_node_t*)parent;

        cmp = nai_str_len(&node->ent.key);
        if (cmp > len) {
            cmp = len;
        };

        c = nai_strncmp(str, nai_str(&node->ent.key), cmp);
        if (c) {
            e = c > 0 ? &parent->rb_left : &parent->rb_right;
            continue;
        };

        cmp = nai_str_len(&node->ent.key);
        if (len > cmp) {
            if (node->prefix) {
                find = node->prefix;
                str += cmp;
                len -= cmp;
                e = &nai_rbtree_root(&node->sub);
            } else {
                e = &parent->rb_right;
            };
            continue;
        };

        if (len == cmp) {
            if (node->exact) {
                find = node->exact;
            } else {
                find = node->prefix;
            };
            break;
        };

        if (len + 1 == cmp) {
            if (node->exact) {
                find = node->exact;
            } else if (node->prefix) {
                find = node->prefix;
            };
        };

        e = &parent->rb_left;
    };

    if (find && find->type != NAI_HTTPC_LOC_PRIORITY) {
        rc = 0;
        goto _end;
    };

#if (NAI_HAVE_REGEX)
    elts = (nai_httpc_location_entry_t**)t->patterns.elts;
    for (n = 0; n < (intptr_t)t->patterns.count; n ++) {
        ent = elts[n];
        rc = nai_httpc_regex_exec(r, &ent->pattern, (nai_mem_t*)uri);
        if (rc < 0) {
            nai_log_alert(NAI_LOG_HTTPC, 
                nai_sult_to_errno(rc), 
                "nai_regex_exec failed when find location '%.*s'", 
                nai_str_len(uri), nai_str(uri));
            goto _end;
        };
        if (rc == NAI_DECLINED) {
            continue;
        };

        find = ent->location;
        rc = 0;
        goto _end;
    };
#endif

    if (find) {
        rc = 0;
    } else {
        rc = NAI_DECLINED;
    };

_end:
    if (rc == 0) {
        pv[0] = find;
    };
    return rc;
};



//////////////////////////////////////////////////////////////////////////////
// httpc core stage


static nai_sult_t nai_httpc_core_all_route(
    nai_httpc_request_t* r, nai_httpc_stage_t* s)
{
    nai_sult_t rc;


    rc = s->handle(r);

    switch (rc) {
    case NAI_OK:
        r->stage ++;
        break;
    case NAI_DECLINED:
        r->stage ++;
        break;
    default:
        if (rc > 0 && rc < NAI_HTTP_OK) {
            r->stage ++;
            rc = NAI_DECLINED;
        };
        break;
    };

    return rc;
};


static nai_sult_t nai_httpc_core_once_route(
    nai_httpc_request_t* r, nai_httpc_stage_t* s)
{
    nai_sult_t rc;


    rc = s->handle(r);

    switch (rc) {
    case NAI_OK:
        r->stage = s->next;
        break;
    case NAI_DECLINED:
        r->stage ++;
        break;
    default:
        if (rc > 0 && rc < NAI_HTTP_OK) {
            r->stage ++;
            rc = NAI_DECLINED;
        };
        break;
    };

    return rc;
};


static nai_sult_t nai_httpc_core_rewrite_route(
    nai_httpc_request_t* r, nai_httpc_stage_t* s)
{
    nai_sult_t rc;
    nai_httpc_t* h;


    rc = s->handle(r);

    switch (rc) {
    case NAI_OK:
        r->stage ++;
        break;
    case NAI_DECLINED:
        r->stage ++;
        break;
    case NAI_DONE:
        h = nai_httpc_get_service(r);
        r->stage = h->groups[NAI_HTTPC_STAGE_DONE].start_index;
        break;
    default:
        if (rc > 0 && rc < NAI_HTTP_OK) {
            r->stage ++;
            rc = NAI_DECLINED;
        };
        break;
    };

    return rc;
};


static nai_sult_t nai_httpc_core_pre_request_route(
    nai_httpc_request_t* r, nai_httpc_stage_t* stage)
{
    nai_sult_t rc;
    nai_httpc_server_t* s;
    nai_httpc_location_t* l;


    (void)stage;

    s = nai_httpc_get_server(r);
    if (r->version == 0) {
        r->version = s->version;
    };

    /* process HTTP/0.9 */
    if (r->major < 1) {
        nai_str_setn(&r->method_name);
        r->keepalive = 0;
        r->method = NAI_HTTP_GET;
        r->headers_out.content_length_n = 0;
        r->headers_out.connection_type = NAI_HTTP_CLOSE;
        goto _end;
    };

    /* process HTTP/1.x */
    if (r->method == NAI_HTTP_GET) {
        r->headers_out.content_length_n = 0;
    };

    /* set keepalive */
    l = nai_httpc_get_location(r);
    switch (r->headers_out.connection_type) {
    case NAI_HTTP_CLOSE:
        r->keepalive = 0;
        break;
    case NAI_HTTP_KEEPALIVE:
        r->keepalive = l->keepalive_max > 0;
        break;
    default:
        r->keepalive = l->keepalive_max > 0 && r->version >= NAI_HTTP_10;
        break;
    };

_end:
    r->stage ++;
    rc = 0;

    return rc;
};


static nai_sult_t nai_httpc_core_post_rewrite_route(
    nai_httpc_request_t* r, nai_httpc_stage_t* s)
{
    nai_sult_t rc;


    if (!r->redirected) {
        rc = nai_httpc_core_pre_request_route(r, s);
        goto _end;
    };

    r->redirects --;
    if (r->redirects <= 0) {
        rc = NAI_HTTPC_INTERNAL_ERROR;
        goto _end;
    };

    r->conf = nai_httpc_get_server(r)->conf;
    r->stage = s->next;
    rc = NAI_OK;

_end:
    return rc;
};


static nai_sult_t nai_httpc_core_request_route(
    nai_httpc_request_t* r, nai_httpc_stage_t* s)
{
    nai_sult_t rc;


    rc = s->handle(r);

    switch (rc) {
    case NAI_DECLINED:
        r->stage ++;
        break;
    case NAI_OK:
        r->stage ++;
        break;
    case NAI_DONE:
        break;
    default:
        if (rc > 0 && rc < NAI_HTTP_OK) {
            r->stage ++;
            rc = NAI_DECLINED;
        };
        break;
    };

    return rc;
};


static nai_sult_t nai_httpc_core_done_route(
    nai_httpc_request_t* r, nai_httpc_stage_t* stage)
{
    nai_sult_t rc;


    (void)r;
    (void)stage;

    rc = NAI_DONE;

    return rc;
};


static nai_sult_t nai_httpc_core_find_config(
    nai_httpc_request_t* r, nai_httpc_stage_t* stage)
{
    nai_sult_t rc;
    nai_httpc_location_t* l = 0;


    r->redirected = 0;
    rc = nai_httpc_location_find(r, &r->uri, &l);
    switch (rc) {
    case NAI_OK:
        r->conf = l->conf;
        break;

    case NAI_DECLINED:
        l = nai_httpc_get_location(r);
        break;

    default:
        goto _end;
    };

    if (!r->internal && l->internal) {
        rc = NAI_HTTP_NOT_FOUND;
        goto _end;
    };

    rc = nai_httpc_core_update(r);
    if (rc < 0) {
        goto _end;
    };

    /* max_body_size not in limit_except */
    if (r->headers_in.content_length_n != (uint64_t)-1 && 
        l->max_body_size > 0 && 
        r->headers_in.content_length_n > (uint64_t)l->max_body_size) {
        rc = NAI_HTTP_REQUEST_ENTITY_TOO_LARGE;
        goto _end;
    };

    r->stage = stage->next;

_end:
    return rc;
};


static nai_int_t nai_httpc_core_compare_stage(const void* a, const void* b)
{
    nai_int_t r;
    nai_httpc_stage_entry_t* e0;
    nai_httpc_stage_entry_t* e1;


    e0 = (nai_httpc_stage_entry_t*)a;
    e1 = (nai_httpc_stage_entry_t*)b;
    r = e0->priority - e1->priority;

    return r;
};


static nai_sult_t nai_httpc_core_stage_init(nai_httpc_t* h)
{
    nai_int_t n, i;
    nai_int_t next;
    nai_int_t count;
    nai_int_t rewrite;
    nai_sult_t rc;
    nai_main_t* m = h->main;
    nai_httpc_route_f route;
    nai_httpc_stage_t* s;
    nai_httpc_stage_entry_t* entries;


    count = 1;  /* find config */
    count ++;   /* finish request */
    for (n = 0; n < NAI_HTTPC_STAGE_MAX; n ++) {
        count += (nai_int_t)h->groups[n].handles.count;
    };

    count ++;
    rewrite = 0;
    if (h->groups[NAI_HTTPC_STAGE_REWRITE].handles.count) {
        rewrite = 1;
    };

    h->stages = nai_palloc(&m->pool, sizeof(nai_httpc_stage_t) * (count+1));
    if (h->stages == 0) {
        rc = nai_sult_from_errno();
        nai_log_alert(NAI_LOG_HTTPC, 
            nai_sult_to_errno(rc), "failed to allocate stage array");
        goto _end;
    };

    s = h->stages;
    next = 0;
    for (n = 0; n < NAI_HTTPC_STAGE_MAX; n ++) {
        count = (nai_int_t)h->groups[n].handles.count;
        switch (n) {
        case NAI_HTTPC_STAGE_PREPARE:
            route = nai_httpc_core_all_route;
            break;

        case NAI_HTTPC_STAGE_SERVER:
            route = nai_httpc_core_rewrite_route;
            break;

        case NAI_HTTPC_STAGE_FIND_CONFIG:
            if (1) {
                next ++;
                s->route = nai_httpc_core_find_config;
                s->handle = 0;
                s->next = next + count;
                s ++;
            };
            route = nai_httpc_core_once_route;
            break;

        case NAI_HTTPC_STAGE_REWRITE:
            if (1) {
                /* fix start index of find_config */
                h->groups[NAI_HTTPC_STAGE_FIND_CONFIG].start_index --;
            };
            route = nai_httpc_core_rewrite_route;
            break;

        case NAI_HTTPC_STAGE_REQUEST:
            if (rewrite) {
                next ++;
                s->route = nai_httpc_core_post_rewrite_route;
                s->handle = 0;
                s->next = h->groups[NAI_HTTPC_STAGE_SERVER].start_index;
                s ++;
            } else {
                next ++;
                s->route = nai_httpc_core_pre_request_route;
                s->handle = 0;
                s->next = next;
                s ++;
            };
            route = nai_httpc_core_request_route;
            break;

        case NAI_HTTPC_STAGE_RESPONSE:
            if (1) { /* for finish request stage */
                next ++;
                s->route = 0;
                s->handle = 0;
                s->next = next;
                s ++;
            };

            route = nai_httpc_core_all_route;
            break;

        case NAI_HTTPC_STAGE_REDIRECT:
            route = nai_httpc_core_once_route;
            break;

        case NAI_HTTPC_STAGE_DONE:
            route = nai_httpc_core_done_route;
            break;

        default:
            assert(0);
            break;
        };

        entries = (nai_httpc_stage_entry_t*)h->groups[n].handles.elts;
        if (count > 1) {
            qsort(entries, count, 
                sizeof(*entries), nai_httpc_core_compare_stage);
        };

        h->groups[n].start_index = next;
        next += count;
        for (i = 0; i < count; i ++) {
            s->route = route;
            s->handle = entries[i].handle;
            s->next = next;
            s ++;
        };
    };

    /* last one */
    s->handle = 0;
    s->route = 0;
    s->next = next;

    rc = 0;

_end:
    return rc;
};


nai_sult_t nai_httpc_core_run_stage(nai_httpc_request_t* r)
{
    nai_sult_t rc;
    nai_httpc_t* h;
    nai_httpc_stage_t* st;
    nai_httpc_stage_t* stages;


    h = nai_httpc_get_service(r);
    stages = h->stages;
    for (;;) {

        st = &stages[r->stage];
        if (st->route == 0) {
            rc = NAI_OK;
            break;
        };

        /* sult process
         *      < 0, a error occurred, finish connection
         *      >= NAI_HTTP_OK, a valid response code, finish connection
         *      other continue
         */
        rc = st->route(r, st);
        if (rc < 0 || rc > NAI_DECLINED) {
            break;
        };
    };

    return rc;
};


nai_sult_t nai_httpc_core_update(nai_httpc_request_t* r)
{
    nai_sult_t rc;
    nai_httpc_location_t* l;
    nai_httpc_protocol_t* p;
    nai_httpc_connection_t* c;


    p = r->proto;
    if (p != 0) {
        p->timeoset = 0;
    };


    /* check methods */
    l = nai_httpc_get_location(r);
    if (l->limit_except & (1 << r->method)) {
        /* limit methods */
        ;
    } else {
        l = l->limit_conf;
        r->conf = l->conf;
    };


    /* set keep alive */
    c = r->connection;
    if (c != 0 && r->keepalive) {
        if (l->keepalive_timeo == 0) {
            r->keepalive = 0;
        } else if (l->keepalive_max >= c->requests) {
            r->keepalive = 0;
        };
    };


    rc = 0;

    return rc;
};


nai_sult_t nai_httpc_add_stage(nai_main_t* m, 
    nai_int_t stage, nai_int_t priority, nai_httpc_handle_f handle)
{
    nai_sult_t rc;
    nai_httpc_t* h;
    nai_httpc_stage_entry_t* ent;


    if (handle == 0) {
        rc = nai_errno_to_sult(EINVAL);
        goto _end;
    };

    if (stage < NAI_HTTPC_STAGE_SERVER || 
        stage >= NAI_HTTPC_STAGE_MAX) {
        rc = nai_errno_to_sult(EINVAL);
        goto _end;
    };

    h = nai_httpc_from_main(m);
    if (h == 0) {
        rc = nai_errno_to_sult(EPERM);
        goto _end;
    };

    ent = (nai_httpc_stage_entry_t*)nai_array_push(&h->groups[stage].handles);
    if (ent == 0) {
        rc = nai_sult_from_errno();
        nai_log_alert(NAI_LOG_HTTPC, 
            nai_sult_to_errno(rc), "push array failed when add stage");
        goto _end;
    };

    ent[0].handle = handle;
    ent[0].priority = priority;
    rc = 0;

_end:
    return rc;
};


static nai_httpc_agent_t* nai_httpc_agent_find(
    nai_httpc_t* h, const nai_str_t* name)
{
    nai_str_t str;
    nai_list_entry_t* e;
    nai_httpc_agent_t* d;


    if (!nai_list_is_empty(&h->named)) {
        str = name[0];
        nai_str_hash(&str, 0);

        e = h->named.next;
        for ( ; e != &h->named; e = e->next) {
            d = (nai_httpc_agent_t*)e;

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


nai_httpc_agent_t* nai_httpc_agent_get(nai_main_t* m, const char* name)
{
    nai_str_t str;
    nai_httpc_t* h;
    nai_httpc_agent_t* r;


    nai_str_sets(&str, name);

    h = (nai_httpc_t*)nai_main_local_at(m, nai_httpc_module);
    if (h == 0) {
        nai_errno = ENOENT;
        r = 0;
        goto _end;
    };

    r = nai_httpc_agent_find(h, &str);

_end:
    return r;
};


nai_httpc_agent_t* nai_httpc_agent_add(
    nai_main_t* m, const char* name, nai_int_t create_new)
{
    intptr_t n;
    nai_sult_t rc;
    nai_str_t str;
    nai_httpc_t* h;
    nai_httpc_agent_t* r;


    h = (nai_httpc_t*)nai_main_local_at(m, nai_httpc_module);
    if (name) {
        nai_str_sets(&str, name);
        r = nai_httpc_agent_find(h, &str);
    } else {
        r = 0;
    };

    if (r != 0) {
        if (create_new) {
            nai_errno = EEXIST;
            r = 0;
            goto _end;
        };

        goto _end;
    };

    rc = nai_httpc_agent_create(h, &m->pool, &r);
    if (rc < 0) {
        nai_errno = nai_sult_to_errno(rc);
        r = 0;
        goto _end;
    };

    /* add into list */
    if (name) {
        n = nai_str_dup(&str, 
            nai_str(&str), nai_str_len(&str), &m->pool);
        if (n < 0) {
            nai_log_alert(NAI_LOG_HTTPC, nai_errno, "nai_str_dup failed");
            r = 0;
            goto _end;
        };

        r->name = str;
        nai_str_hash(&r->name, 0);
        nai_list_insert_tail(&h->named, &r->ent);
    } else {
        nai_str_setn(&r->name);
        nai_list_insert_tail(&h->anony, &r->ent);
    };

_end:
    return r;
};


nai_sult_t nai_httpc_agent_command(
    nai_command_t* c, nai_command_args_t* args)
{
    nai_sult_t rc;
    nai_str_t str;
    nai_main_t* m;
    nai_httpc_t* h;
    nai_httpc_agent_t* r;
    nai_httpc_agent_t** pv;


    if (args->endseg) {
        rc = 0;
        goto _end;
    };

    m = args->main;
    h = nai_httpc_get_service(args);
    if (h == 0) {
        if (c->offset == 0) {
            rc = nai_errno_to_sult(EPERM);
            nai_log_alert(NAI_LOG_HTTPC, 
                nai_sult_to_errno(rc), 
                "get httpc failed, return value is null");
            goto _end;
        };

        rc = nai_httpc_service_create(m, args->pool, &h);
        if (rc < 0) {
            nai_log_error(NAI_LOG_HTTPC, 
                nai_sult_to_errno(rc), "create httpc service failed");
            goto _end;
        };

        nai_main_local_at(m, nai_httpc_module) = h;
        nai_local_at(&h->local, nai_httpc_core_module) = h;
    };


    if (c->offset != 0) {
        pv = (nai_httpc_agent_t**)(args->data + c->offset);
    } else {
        pv = 0;
    };
    if (pv != 0 && pv[0]) {
        rc = nai_errno_to_sult(EPERM);
        nai_log_alert(NAI_LOG_HTTPC, 
            nai_sult_to_errno(rc), 
            "command '%s' has set httpc agent", c->name);
        goto _end;
    };

    if (args->argc >= 1) {
        rc = nai_value_dups(&args->argv[0], &str, args->pool_tmp);
        if (rc < 0) {
            nai_log_error(NAI_LOG_HTTPC, 
                nai_sult_to_errno(rc), 
                "command '%s' get string failed", c->name);
            goto _end;
        };

    } else { /* argc == 0 */
        if (pv == 0) {
            rc = nai_errno_to_sult(EINVAL);
            nai_log_alert(NAI_LOG_HTTPC, 
                nai_sult_to_errno(rc), 
                "command '%s' required a name", c->name);
            goto _end;
        };

        nai_str_setn(&str);
    };

    r = nai_httpc_agent_add(m, nai_str(&str), 0);
    if (r == 0) {
        rc = nai_sult_from_errno();
        nai_log_error(NAI_LOG_HTTPC, 
            nai_sult_to_errno(rc), 
            "nai_httpc_agent_add(%s) failed", nai_str(&str));
        goto _end;
    };

    if (pv) {
        pv[0] = r;
    };
    if (pv && args->argc >= 1) {
        args->endseg = 1;
    } else {
        args->grp = NAI_MODULE_HTTPC;
        args->grp_mask = NAI_HTTPC_CLOC_AGENT;
        args->clocal = r->clocal;
    };

    rc = 0;

_end:
    return rc;
};


