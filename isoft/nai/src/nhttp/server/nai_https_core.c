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
/// @file       nai_https_core.c
/// @brief      
/// @details
/// @date       2021-01-16
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
#include "nai_https_core.h"
#include <stdlib.h>



//////////////////////////////////////////////////////////////////////////////
// https core module


static nai_sult_t nai_https_core_preproc(nai_https_t* h);
static nai_sult_t nai_https_core_postproc(nai_https_t* h);
static nai_sult_t nai_https_core_stage_init(nai_https_t* h);
static nai_sult_t nai_https_core_server_merge(
    nai_https_local_ctx_t* c, void* s, void* up);
static nai_sult_t nai_https_core_location_merge(
    nai_https_local_ctx_t* c, void* s, void* up);


static nai_sult_t nai_https_listen_merge(nai_https_listening_t* l);
static nai_sult_t nai_https_bind(
    nai_https_t* h, nai_https_server_t* hs, 
    const nai_sockaddr_t* name, nai_int_t namelen, 
    const nai_https_listening_opt_t* opts);


static nai_sult_t nai_https_server_segment(
    nai_command_t* c, nai_command_args_t* args);
static nai_sult_t nai_https_server_name(
    nai_command_t* c, nai_command_args_t* args);
static nai_sult_t nai_https_listen(
    nai_command_t* c, nai_command_args_t* args);
static nai_sult_t nai_https_location_segment(
    nai_command_t* c, nai_command_args_t* args);
static nai_sult_t nai_https_root(
    nai_command_t* c, nai_command_args_t* args);
static nai_sult_t nai_https_type(
    nai_command_t* c, nai_command_args_t* args);
static nai_sult_t nai_https_limit_except(
    nai_command_t* c, nai_command_args_t* args);
static nai_sult_t nai_https_internal(
    nai_command_t* c, nai_command_args_t* args);



static nai_command_optinfo_t nai_https_option_3state[] = {
    { nai_strconst("off"),  NAI_HTTPS_OFF },
    { nai_strconst("on"),   NAI_HTTPS_ON }, 
    { nai_strconst("always"), NAI_HTTPS_ALWAYS }, 
    { nai_strnull(), 0 }
};

static nai_command_optinfo_t nai_https_option_satisfy[] = {
    { nai_strconst("off"),  NAI_HTTPS_OFF },
    { nai_strconst("on"),   NAI_HTTPS_ON }, 
    { nai_strconst("all"),  NAI_HTTPS_ALWAYS }, 
    { nai_strnull(), 0 }
};

static struct {
    const char* name;
    nai_int_t method;
} nai_https_methods[] = {
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



static nai_command_t nai_https_core_commands[] = {
    { "server", nai_https_server_segment, 
        NAI_HTTPS_CLOC_MAIN|NAI_COPT_SEGEMENT|NAI_COPT_ARG0, 0, 0 },
    { "server_name", nai_https_server_name, 
        NAI_HTTPS_CLOC_MAIN|NAI_HTTPS_CLOC_SRV|
        NAI_HTTPS_COPT_SRV|NAI_COPT_ARG1MORE, 0, 0 },
    { "opt_dns", nai_main_dns_command, 
        NAI_HTTPS_CLOC_MAIN|NAI_HTTPS_CLOC_SRV|
        NAI_HTTPS_COPT_SRV|NAI_COPT_ARG0|NAI_COPT_ARG1, 
        nai_offsetof(nai_https_server_t, conn.dns), 0 },
    { "opt_agent", nai_main_agent_command, 
        NAI_HTTPS_CLOC_MAIN|NAI_HTTPS_CLOC_SRV|
        NAI_HTTPS_COPT_SRV|NAI_COPT_ARG0|NAI_COPT_ARG1, 
        nai_offsetof(nai_https_server_t, conn.agent), 0 },
    { "listen", nai_https_listen, 
        NAI_HTTPS_CLOC_MAIN|NAI_HTTPS_CLOC_SRV|
        NAI_HTTPS_COPT_SRV|NAI_COPT_ARG1|NAI_COPT_ARG1MORE, 0, 0 },
    { "max_request_line", nai_command_set_size, 
        NAI_HTTPS_CLOC_MAIN|NAI_HTTPS_CLOC_SRV|
        NAI_HTTPS_COPT_SRV|NAI_COPT_ARG1,
        nai_offsetof(nai_https_server_t, min_header_buffer), 0 },
    { "max_header_line", nai_command_set_bufsize, 
        NAI_HTTPS_CLOC_MAIN|NAI_HTTPS_CLOC_SRV|
        NAI_HTTPS_COPT_SRV|NAI_COPT_SEGEMENT|NAI_COPT_ARG1,
        nai_offsetof(nai_https_server_t, max_header_buffers), 0 },
    { "max_headers", nai_command_set_size, 
        NAI_HTTPS_CLOC_MAIN|NAI_HTTPS_CLOC_SRV|
        NAI_HTTPS_COPT_SRV|NAI_COPT_ARG1,
        nai_offsetof(nai_https_server_t, max_headers), 0 },
    { "send_so_bufsize", nai_command_set_size, 
        NAI_HTTPS_CLOC_MAIN|NAI_HTTPS_CLOC_SRV|
        NAI_HTTPS_COPT_SRV|NAI_COPT_ARG1,
        nai_offsetof(nai_https_server_t, send_so_bufsize), 0 },
    { "read_so_bufsize", nai_command_set_size, 
        NAI_HTTPS_CLOC_MAIN|NAI_HTTPS_CLOC_SRV|
        NAI_HTTPS_COPT_SRV|NAI_COPT_ARG1,
        nai_offsetof(nai_https_server_t, read_so_bufsize), 0 },
    { "read_head_timeout", nai_command_set_msec, 
        NAI_HTTPS_CLOC_MAIN|NAI_HTTPS_CLOC_SRV|
        NAI_HTTPS_COPT_SRV|NAI_COPT_ARG1,
        nai_offsetof(nai_https_server_t, read_head_timeo), 0 },

    { "location", nai_https_location_segment, 
        NAI_HTTPS_CLOC_MAIN|NAI_HTTPS_CLOC_SRV|
        NAI_HTTPS_COPT_SRV|NAI_COPT_SEGEMENT|NAI_COPT_ARG1, 0, 0 },
    { "root", nai_https_root, 
        NAI_HTTPS_CLOC_MAIN|NAI_HTTPS_CLOC_SRV|NAI_HTTPS_CLOC_LOC|
        NAI_HTTPS_COPT_LOC|NAI_COPT_ARG1, 0, 0 },
    { "default_type", nai_command_set_string, 
        NAI_HTTPS_CLOC_MAIN|NAI_HTTPS_CLOC_SRV|NAI_HTTPS_CLOC_LOC|
        NAI_HTTPS_COPT_LOC|NAI_COPT_ARG1,
        nai_offsetof(nai_https_location_t, default_type), 0 },
    { "type", nai_https_type, 
        NAI_HTTPS_CLOC_MAIN|NAI_HTTPS_CLOC_SRV|NAI_HTTPS_CLOC_LOC|
        NAI_HTTPS_COPT_LOC|NAI_COPT_ARG1,
        nai_offsetof(nai_https_location_t, types), 0 },
    { "limit_except", nai_https_limit_except, 
        NAI_HTTPS_CLOC_LOC|
        NAI_HTTPS_COPT_LOC|NAI_COPT_SEGEMENT|NAI_COPT_ARG1MORE, 0, 0}, 
    { "max_body_size", nai_command_set_off, 
        NAI_HTTPS_CLOC_MAIN|NAI_HTTPS_CLOC_SRV|NAI_HTTPS_CLOC_LOC|
        NAI_HTTPS_COPT_LOC|NAI_COPT_ARG1,
        nai_offsetof(nai_https_location_t, max_body_size), 0 },
    { "directio", nai_command_set_off, 
        NAI_HTTPS_CLOC_MAIN|NAI_HTTPS_CLOC_SRV|NAI_HTTPS_CLOC_LOC|
        NAI_HTTPS_COPT_LOC|NAI_COPT_ARG1,
        nai_offsetof(nai_https_location_t, directio), 0 },
    { "directio_align", nai_command_set_off, 
        NAI_HTTPS_CLOC_MAIN|NAI_HTTPS_CLOC_SRV|NAI_HTTPS_CLOC_LOC|
        NAI_HTTPS_COPT_LOC|NAI_COPT_ARG1,
        nai_offsetof(nai_https_location_t, directio_align), 0 },
    { "send_max_chunk", nai_command_set_size, 
        NAI_HTTPS_CLOC_MAIN|NAI_HTTPS_CLOC_SRV|NAI_HTTPS_CLOC_LOC|
        NAI_HTTPS_COPT_LOC|NAI_COPT_ARG1,
        nai_offsetof(nai_https_location_t, read_chunk_size), 0 },
    { "send_max_chunk", nai_command_set_size, 
        NAI_HTTPS_CLOC_MAIN|NAI_HTTPS_CLOC_SRV|NAI_HTTPS_CLOC_LOC|
        NAI_HTTPS_COPT_LOC|NAI_COPT_ARG1,
        nai_offsetof(nai_https_location_t, send_chunk_size), 0 },
    { "send_lowat", nai_command_set_size, 
        NAI_HTTPS_CLOC_MAIN|NAI_HTTPS_CLOC_SRV|NAI_HTTPS_CLOC_LOC|
        NAI_HTTPS_COPT_LOC|NAI_COPT_ARG1,
        nai_offsetof(nai_https_location_t, send_lowat), 0 },
    { "send_timeout", nai_command_set_msec, 
        NAI_HTTPS_CLOC_MAIN|NAI_HTTPS_CLOC_SRV|NAI_HTTPS_CLOC_LOC|
        NAI_HTTPS_COPT_LOC|NAI_COPT_ARG1,
        nai_offsetof(nai_https_location_t, send_timeo), 0 },
    { "read_body_timeout", nai_command_set_msec, 
        NAI_HTTPS_CLOC_MAIN|NAI_HTTPS_CLOC_SRV|NAI_HTTPS_CLOC_LOC|
        NAI_HTTPS_COPT_LOC|NAI_COPT_ARG1,
        nai_offsetof(nai_https_location_t, read_body_timeo), 0 },
    { "keepalive_timeout", nai_command_set_msec, 
        NAI_HTTPS_CLOC_MAIN|NAI_HTTPS_CLOC_SRV|NAI_HTTPS_CLOC_LOC|
        NAI_HTTPS_COPT_SRV|NAI_COPT_ARG1,
        nai_offsetof(nai_https_location_t, keepalive_timeo), 0 },
    { "lingering_timeout", nai_command_set_msec, 
        NAI_HTTPS_CLOC_MAIN|NAI_HTTPS_CLOC_SRV|NAI_HTTPS_CLOC_LOC|
        NAI_HTTPS_COPT_LOC|NAI_COPT_ARG1,
        nai_offsetof(nai_https_location_t, lingering_timeo), 0 },
    { "lingering_time", nai_command_set_msec, 
        NAI_HTTPS_CLOC_MAIN|NAI_HTTPS_CLOC_SRV|NAI_HTTPS_CLOC_LOC|
        NAI_HTTPS_COPT_LOC|NAI_COPT_ARG1,
        nai_offsetof(nai_https_location_t, lingering_timeomax), 0 },
    { "keepalive_max", nai_command_set_int32, 
        NAI_HTTPS_CLOC_MAIN|NAI_HTTPS_CLOC_SRV|NAI_HTTPS_CLOC_LOC|
        NAI_HTTPS_COPT_LOC|NAI_COPT_ARG1,
        nai_offsetof(nai_https_location_t, lingering_close), 
        NAI_OPT_INT_MIN(0) },
    { "lingering_close", nai_command_set_opts, 
        NAI_HTTPS_CLOC_MAIN|NAI_HTTPS_CLOC_SRV|NAI_HTTPS_CLOC_LOC|
        NAI_HTTPS_COPT_LOC|NAI_COPT_ARG1,
        nai_offsetof(nai_https_location_t, lingering_close), 
        nai_https_option_3state },
    { "sendfile", nai_command_set_opts, 
        NAI_HTTPS_CLOC_MAIN|NAI_HTTPS_CLOC_SRV|NAI_HTTPS_CLOC_LOC|
        NAI_HTTPS_COPT_LOC|NAI_COPT_ARG1,
        nai_offsetof(nai_https_location_t, sendfile), 0 },
    { "aio", nai_command_set_opts, 
        NAI_HTTPS_CLOC_MAIN|NAI_HTTPS_CLOC_SRV|NAI_HTTPS_CLOC_LOC|
        NAI_HTTPS_COPT_LOC|NAI_COPT_ARG1,
        nai_offsetof(nai_https_location_t, aio), 0 },
    { "tcp_nopush", nai_command_set_opts, 
        NAI_HTTPS_CLOC_MAIN|NAI_HTTPS_CLOC_SRV|NAI_HTTPS_CLOC_LOC|
        NAI_HTTPS_COPT_LOC|NAI_COPT_SEGEMENT|NAI_COPT_ARG1,
        nai_offsetof(nai_https_location_t, tcp_nopush), 0 },
    { "tcp_nodelay", nai_command_set_opts, 
        NAI_HTTPS_CLOC_MAIN|NAI_HTTPS_CLOC_SRV|NAI_HTTPS_CLOC_LOC|
        NAI_HTTPS_COPT_LOC|NAI_COPT_ARG1,
        nai_offsetof(nai_https_location_t, tcp_nodelay), 0 },
    { "etag", nai_command_set_opts, 
        NAI_HTTPS_CLOC_MAIN|NAI_HTTPS_CLOC_SRV|NAI_HTTPS_CLOC_LOC|
        NAI_HTTPS_COPT_LOC|NAI_COPT_ARG1,
        nai_offsetof(nai_https_location_t, etag), 0 },
    { "chunked", nai_command_set_opts, 
        NAI_HTTPS_CLOC_MAIN|NAI_HTTPS_CLOC_SRV|NAI_HTTPS_CLOC_LOC|
        NAI_HTTPS_COPT_LOC|NAI_COPT_ARG1,
        nai_offsetof(nai_https_location_t, chunked), 0 },
    { "satisfy", nai_command_set_opts, 
        NAI_HTTPS_CLOC_MAIN|NAI_HTTPS_CLOC_SRV|NAI_HTTPS_CLOC_LOC|
        NAI_HTTPS_COPT_LOC|NAI_COPT_ARG1,
        nai_offsetof(nai_https_location_t, satisfy), 
        nai_https_option_satisfy },
    { "internal", nai_https_internal, 
        NAI_HTTPS_CLOC_LOC|NAI_HTTPS_COPT_LOC,
        nai_offsetof(nai_https_location_t, internal), 0 },
    { 0 }
};


static nai_https_module_t nai_https_core_module_ext = {
    nai_https_core_preproc,             /* pre-processing */
    nai_https_core_postproc,            /* post-processing */
    0,                                  /* main alloc */
    0, nai_https_core_server_merge,     /* server alloc */
    0, nai_https_core_location_merge,   /* location alloc */
};


nai_module_t nai_https_core_module = {
    NAI_MODULE_HEADER,
    NAI_MODULE_HTTPS, 
    nai_https_core_commands, 
    "https_core", &nai_https_core_module_ext, {
        0, 
    },
};


extern nai_sult_t nai_https_uri_delims_init();
extern nai_sult_t nai_https_header_tables_init(nai_https_t* h);
extern nai_sult_t nai_https_core_chain_init(nai_https_t* h);
extern nai_sult_t nai_https_core_variable_init(nai_https_t* h);


static nai_sult_t nai_https_core_preproc(nai_https_t* h)
{
    nai_sult_t rc;


    /* init core variables */
    rc = nai_https_core_variable_init(h);
    if (rc < 0) {
        nai_log_alert(NAI_LOG_HTTPS, 
            nai_sult_to_errno(rc), 
            "init core variables of https failed");
        goto _end;
    };


_end:
    return rc;
};


static nai_sult_t nai_https_core_postproc(nai_https_t* h)
{
    nai_int_t n;
    nai_sult_t rc;
    nai_https_server_t* d;
    nai_https_server_t* s;
    nai_https_server_t** sa;
    nai_https_listening_t* l;
    nai_https_listening_t** la;


    /* init uri delims */
    rc = nai_https_uri_delims_init(h);
    if (rc < 0) {
        nai_log_alert(NAI_LOG_HTTPS, 
            nai_sult_to_errno(rc), 
            "init uri delims of http failed");
        goto _end;
    };


    /* init global header tables */
    rc = nai_https_header_tables_init(h);
    if (rc < 0) {
        nai_log_alert(NAI_LOG_HTTPS, 
            nai_sult_to_errno(rc), 
            "init global header tables of http failed");
        goto _end;
    };


    /* init core stages */
    rc = nai_https_core_stage_init(h);
    if (rc < 0) {
        nai_log_alert(NAI_LOG_HTTPS, 
            nai_sult_to_errno(rc), 
            "init core stages of http failed");
        goto _end;
    };


    /* init core chains */
    rc = nai_https_core_chain_init(h);
    if (rc < 0) {
        nai_log_alert(NAI_LOG_HTTPS, 
            nai_sult_to_errno(rc), 
            "init core chains of http failed");
        goto _end;
    };


    /* process configs */
    sa = (nai_https_server_t**)h->servers.elts;
    s = sa[0];
    for (n = 0; n < (intptr_t)h->servers.count; n ++) {
        d = sa[n];
        rc = nai_https_server_merge(d, s == d ? 0 : s, 0, 0);
        if (rc < 0) {
            goto _end;
        };
    };

    /* process listens */
    la = (nai_https_listening_t**)h->listening.elts;
    for (n = 0; n < (intptr_t)h->listening.count; n ++) {
        l = la[n];
        rc = nai_https_listen_merge(l);
        if (rc < 0) {
            goto _end;
        };
    };

    rc = 0;

_end:
    return rc;
};


static nai_sult_t nai_https_core_server_merge(
    nai_https_local_ctx_t* c, void* s, void* up)
{
    nai_sult_t rc;
    nai_https_server_t* conf = (nai_https_server_t*)s;
    nai_https_server_t* parent = (nai_https_server_t*)up;


    (void)c;

    if (parent == 0) {
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
        if (conf->read_head_timeo == NAI_OPT_UNSET_MSEC) {
            conf->read_head_timeo = 60*1000;
        };
        if (conf->conn.agent == 0) {
            conf->conn.agent = 
                nai_main_agent_add(c->service->main, "default", 0);
            if (conf->conn.agent == 0) {
                rc = nai_sult_from_errno();
                nai_log_error(NAI_LOG_HTTPS, 
                    nai_sult_to_errno(rc), "nai_main_agent_add failed");
                goto _end;
            };
        };
        if (conf->conn.dns == 0) {
            conf->conn.dns = 
                nai_main_dns_add(c->service->main, "default", 0);
            if (conf->conn.dns == 0) {
                rc = nai_sult_from_errno();
                nai_log_error(NAI_LOG_HTTPS, 
                    nai_sult_to_errno(rc), "nai_main_dns_add failed");
                goto _end;
            };
        };
    } else {
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
        if (conf->read_head_timeo == NAI_OPT_UNSET_MSEC) {
            conf->read_head_timeo = parent->read_head_timeo;
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


static nai_sult_t nai_https_core_location_merge(
    nai_https_local_ctx_t* c, void* s, void* up)
{
    char* str;
    nai_sult_t rc;
    nai_https_location_t* conf = (nai_https_location_t*)s;
    nai_https_location_t* parent = (nai_https_location_t*)up;


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
        if (conf->send_timeo == NAI_OPT_UNSET_MSEC) {
            conf->send_timeo = 60*1000;
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
            conf->lingering_close = NAI_HTTPS_ALWAYS;
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
            conf->satisfy = NAI_HTTPS_ALWAYS;
        };

        if (nai_script_expn_is_null(&conf->root)) {
            nai_str_setc(&conf->root.value, "");
        };

        rc = nai_hashsnap_build(&conf->types_hash, 
            conf->types.elts, 
            (nai_int_t)conf->types.count, 
            (nai_int_t)conf->types.count * 2 + 1, 64);

    } else {
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
        if (conf->send_timeo == NAI_OPT_UNSET_MSEC) {
            conf->send_timeo = parent->send_timeo;
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

        if (nai_script_expn_is_null(&conf->root)) {
            conf->root = parent->root;
            conf->alias = parent->alias;
        };

        if (conf->types.count <= 0) {
            conf->types_hash = parent->types_hash;
            rc = 0;
        } else {
            rc = nai_hashsnap_build(&conf->types_hash, 
                conf->types.elts, 
                (nai_int_t)conf->types.count, 
                (nai_int_t)conf->types.count * 2 + 1, 64);
        };

    };

    if (rc < 0) {
        rc = nai_sult_from_errno();
        nai_log_alert(NAI_LOG_HTTPS, 
            nai_sult_to_errno(rc), "build hash snap for types failed");
        goto _end;
    };

    if (parent && conf->keepalive_timeo == parent->keepalive_timeo) {
        conf->keepalive_timeo_str = parent->keepalive_timeo_str;
    } else {
        str = (char*)nai_palloc(c->pool, sizeof("timeout=") + NAI_INT_T_LEN);
        nai_sprintf(str, "timeout=%d", (nai_int_t)(conf->keepalive_timeo/1000));
        nai_str_sets(&conf->keepalive_timeo_str, str);
    };

_end:
    return rc;
};



static nai_sult_t nai_https_server_segment(
    nai_command_t* c, nai_command_args_t* args)
{
    nai_sult_t rc;
    nai_https_t* h;
    nai_https_server_t* s;


    (void)c;

    if (args->endseg) {
        rc = 0;
        goto _end;
    };

    h = nai_https_get_service(args);
    if (h == 0) {
        rc = nai_errno_to_sult(NAI_EINTERNAL);
        nai_log_alert(NAI_LOG_HTTPS, 
            nai_sult_to_errno(rc), "get https failed, return value is null");
        goto _end;
    };

    rc = nai_https_server_create(h, args->pool, &s);
    if (rc < 0) {
        nai_log_alert(NAI_LOG_HTTPS, 
            nai_sult_to_errno(rc), "create new server failed");
        goto _end;
    };

    args->grp = NAI_MODULE_HTTPS;
    args->grp_mask = NAI_HTTPS_CLOC_SRV;
    args->clocal = s->clocal;
    rc = 0;

_end:
    return rc;
};


static nai_sult_t nai_https_server_name(
    nai_command_t* c, nai_command_args_t* args)
{
    nai_int_t n;
    nai_sult_t rc;
    nai_str_t str;
#if (NAI_HAVE_REGEX)
    nai_regex_errinfo_t err;
#endif
    nai_https_server_t* s;
    nai_https_server_name_t* sn;


    (void)c;

    s = nai_https_get_server(args);
    if (s == 0) {
        rc = nai_errno_to_sult(NAI_EINTERNAL);
        nai_log_alert(NAI_LOG_HTTPS, 
            nai_sult_to_errno(rc), "get location failed, return value is null");
        goto _end;
    };

    for (n = 0; n < args->argc; n ++) {
        rc = nai_value_dups(&args->argv[n], &str, args->pool);
        if (rc < 0) {
            nai_log_error(NAI_LOG_HTTPS, 
                nai_sult_to_errno(rc), "get server name failed");
            goto _end;
        };

        /* set default server name */
        if (nai_str_len(&s->name) <= 0) {
            s->name = str;
        };

        sn = (nai_https_server_name_t*)nai_array_push(&s->names);
        if (sn == 0) {
            rc = nai_sult_from_errno();
            nai_log_alert(NAI_LOG_HTTPS, 
                nai_sult_to_errno(rc), "push server name failed");
            goto _end;
        };
        sn->server = s;

#if (NAI_HAVE_REGEX)
        if (nai_str_len(&str) > 0 && nai_str(&str)[0] == '~') {
            rc = nai_regex_compile(
                &sn->pattern, nai_str(&str)+1, 0, &err, args->pool);
            if (rc < 0) {
                rc = nai_sult_from_errno();
                nai_log_error(NAI_LOG_HTTPS, 
                    nai_sult_to_errno(rc), "compile server name %s: %s%s%s", 
                    nai_str(&str)+1, err.str,
                    err.offset >= 0 ? ", at " : "", 
                    err.offset >= 0 ? nai_str(&str)+1 + err.offset : "");
                goto _end;
            };

            sn->type = NAI_HTTPS_SERVNAME_REGEX;
        } else 
#endif
        if (nai_strchr(nai_str(&str), '*') || 
            nai_strchr(nai_str(&str), '?')) {
            sn->name = str;
            sn->type = NAI_HTTPS_SERVNAME_WILD;
        } else {
            nai_str_hash(&str, 1);
            sn->name = str;
            sn->type = NAI_HTTPS_SERVNAME_NORM;
        };
    };

    rc = 0;

_end:
    return rc;
};


static nai_sult_t nai_https_listen(
    nai_command_t* c, nai_command_args_t* args)
{
    intptr_t n;
    nai_int_t port;
    char ch[2] = {0};
    nai_sultp_t rc;
    nai_uri_t uri;
    nai_str_t str;
    nai_str_t tmp;
    nai_socknbuf_t nbuf;
    nai_socknbuf_t* list;
    nai_https_t* h;
    nai_https_server_t* s;
    nai_https_listening_opt_t opts = {0};


    (void)c;

    s = nai_https_get_server(args);
    if (s == 0) {
        rc = nai_errno_to_sult(NAI_EINTERNAL);
        nai_log_error(NAI_LOG_HTTPS, 
            nai_sult_to_errno((nai_int_t)rc), 
            "get location failed, return value is null");
        goto _end;
    };


    for (n = 1; n < args->argc; n ++) {
        rc = nai_value_dups(&args->argv[n], &str, args->pool_tmp);
        if (rc < 0) {
            nai_log_error(NAI_LOG_HTTPS, 
                nai_sult_to_errno((nai_int_t)rc), "get listen options failed");
            goto _end;
        };

        if (nai_strcmp(nai_str(&str), "default") == 0) {
            opts.default_one = 1;
            continue;
        };
#if (NAI_HAVE_SSL)
        if (nai_strcmp(nai_str(&str), "ssl") == 0) {
            opts.ssl = 1;
            continue;
        };
#endif
        if (nai_strcmp(nai_str(&str), "reuse") == 0) {
            opts.reuse = 1;
            opts.seted = 1;
            continue;
        };
        if (nai_strncmp(nai_str(&str), "backlog=", 8) == 0) {
            rc = nai_atoi(&opts.backlog, 
                nai_str(&str)+8, nai_str_len(&str)-8, 0);
            if (rc < 0) {
                rc = nai_sult_from_errno();
                nai_log_error(NAI_LOG_HTTPS, 
                    nai_sult_to_errno((nai_int_t)rc), 
                    "invalid option backlog: '%s'", nai_str(&str));
                goto _end;
            };
            if (opts.backlog < 0) {
                rc = nai_errno_to_sult(EINVAL);
                nai_log_error(NAI_LOG_HTTPS, 
                    nai_sult_to_errno((nai_int_t)rc), 
                    "invalid option backlog: '%s'", nai_str(&str));
                goto _end;
            };
            opts.seted = 1;
            continue;
        };
        if (nai_strncmp(nai_str(&str), "rcvbuf=", 7) == 0) {
            rc = nai_parse_size(&opts.recvbuf, 
                nai_str(&str)+7, nai_str_len(&str)-7);
            if (rc < 0) {
                rc = nai_sult_from_errno();
                nai_log_error(NAI_LOG_HTTPS, 
                    nai_sult_to_errno((nai_int_t)rc), 
                    "invalid option rcvbuf: '%s'", nai_str(&str));
                goto _end;
            };
            opts.seted = 1;
            continue;
        };
        if (nai_strncmp(nai_str(&str), "sndbuf=", 7) == 0) {
            nai_str_setm(&tmp, nai_str(&str)+7, nai_str_len(&str)-7);
            rc = nai_parse_size(&opts.sendbuf, 
                nai_str(&str)+7, nai_str_len(&str)-7);
            if (rc < 0) {
                rc = nai_sult_from_errno();
                nai_log_error(NAI_LOG_HTTPS, 
                    nai_sult_to_errno((nai_int_t)rc), 
                    "invalid option sndbuf: '%s'", nai_str(&str));
                goto _end;
            };
            opts.seted = 1;
            continue;
        };

        rc = nai_errno_to_sult(EINVAL);
        nai_log_error(NAI_LOG_HTTPS, 
            nai_sult_to_errno((nai_int_t)rc), 
            "unknown listen option '%s'", nai_str(&str));
        goto _end;
    };


    h = nai_https_get_service(s);
    rc = nai_value_geti(&args->argv[0], &port);
    if (rc >= 0) {
        /* port only case */
        /* add ipv4 address */
        nbuf.len = sizeof(nbuf.addr_in4);
        nai_sockaddr_mk_inet(AF_INET, 0, port, &nbuf.addr, &nbuf.len);
        rc = nai_https_bind(h, s, &nbuf.addr, nbuf.len, &opts);
        if (rc < 0) {
            nai_log_crit(NAI_LOG_HTTPS, 
                nai_sult_to_errno((nai_int_t)rc), 
                "bind 0.0.0.0:%d failed", port);
            goto _end;
        };

#if (NAI_HAVE_SOCKADDR_IN6)
        /* add ipv6 address */
        nbuf.len = sizeof(nbuf.addr_in6);
        nai_sockaddr_mk_inet(AF_INET6, 0, port, &nbuf.addr, &nbuf.len);
        rc = nai_https_bind(h, s, &nbuf.addr, nbuf.len, &opts);
        if (rc < 0) {
            nai_log_crit(NAI_LOG_HTTPS, 
                nai_sult_to_errno((nai_int_t)rc), "bind [::]:%d failed", port);
            goto _end;
        };
#endif

    } else {
        rc = nai_value_dups(&args->argv[0], &str, args->pool_tmp);
        if (rc < 0) {
            nai_log_error(NAI_LOG_HTTPS, 
                nai_sult_to_errno((nai_int_t)rc), "get listen address failed");
            goto _end;
        };

        /* parse host */
        rc = nai_uri_parse(&uri, 
            nai_str(&str), nai_str_len(&str), NAI_URI_STRICT|NAI_URI_HOSTONLY);
        if (rc < 0) {
            rc = nai_sult_from_errno();
            nai_log_error(NAI_LOG_HTTPS, 
                nai_sult_to_errno((nai_int_t)rc), 
                "parse listen address failed, %s", nai_str(&str));
            goto _end;
        };

        /* truncate string */
        if (uri.hostname.ptr) {
            ch[0] = uri.hostname.ptr[uri.hostname.len];
            uri.hostname.ptr[uri.hostname.len] = 0;
        };
        if (uri.portname.ptr) {
            ch[1] = uri.portname.ptr[uri.portname.len];
            uri.portname.ptr[uri.portname.len] = 0;
        };

        /* query the list of socket address */
        n = 0;
        list = 0;
        for (;;) {
            rc = nai_sockaddr_list(
                nai_str(&uri.hostname), nai_str(&uri.portname), list, n);
            if (rc < 0) {
                rc = nai_sult_from_errno();
                nai_log_crit(NAI_LOG_HTTPS, 
                    nai_sult_to_errno((nai_int_t)rc), 
                    "parse listen address failed, %s", nai_str(&str));
                goto _end;
            };

            if (rc <= n) {
                break;
            };

            n = rc;
            list = (nai_socknbuf_t*)nai_palloc(args->pool_tmp, n);
        };

        /* restore terminal character */
        if (uri.hostname.ptr) {
            uri.hostname.ptr[uri.hostname.len] = ch[0];
        };
        if (uri.portname.ptr) {
            uri.portname.ptr[uri.portname.len] = ch[1];
        };
        /* walk on address list */
        while (list->len) {
            rc = nai_https_bind(h, 
                s, &list->addr, list->len, &opts);
            if (rc < 0) {
                nai_log_crit(NAI_LOG_HTTPS, 
                    nai_sult_to_errno((nai_int_t)rc), "bind %.*s failed", 
                    nai_str_len(&uri.host), nai_str(&uri.host));
                goto _end;
            };

            list = (nai_socknbuf_t*)((uint8_t*)list + list->len);
        };
    };

    rc = 0;

_end:
    return (nai_sult_t)rc;
};


static nai_sult_t nai_https_location_segment(
    nai_command_t* c, nai_command_args_t* args)
{
    nai_int_t n;
    nai_int_t type;
    const char* p;
    nai_sult_t rc;
    nai_str_t str;
    nai_str_t mod;
    nai_https_t* h;
    nai_https_server_t* s;
    nai_https_location_t* u;
    nai_https_location_t** pl;
    nai_https_location_t** elts;


    (void)c;

    if (args->endseg) {
        rc = 0;
        goto _end;
    };

    if (args->argc == 1) {
        rc = nai_value_dups(&args->argv[0], &str, args->pool);
        if (rc < 0) {
            nai_log_error(NAI_LOG_HTTPS, 
                nai_sult_to_errno(rc), "get location name failed");
            goto _end;
        };
        if (nai_str_len(&str) <= 0) {
            rc = nai_errno_to_sult(EINVAL);
            nai_log_error(NAI_LOG_HTTPS, 
                nai_sult_to_errno(rc), "location name is empty");
            goto _end;
        };

        type = NAI_HTTPS_LOC_NORMAL;
        p = nai_str(&str);
        if (p[0] == '=') {
            str.ptr ++;
            str.len --;
            type = NAI_HTTPS_LOC_EXACT;
#if (NAI_HAVE_REGEX)
        } else if (nai_str_len(&str) >= 2 && p[0] == '~' && p[1] == '*') {
            str.ptr += 2;
            str.len -= 2;
            type = NAI_HTTPS_LOC_PATTERN_CASE;
        } else if (p[0] == '~') {
            str.ptr ++;
            str.len --;
            type = NAI_HTTPS_LOC_PATTERN;
#endif
        } else if (p[0] == '^') {
            str.ptr ++;
            str.len --;
            type = NAI_HTTPS_LOC_PRIORITY;
        } else if (p[0] == '@') {
            type = NAI_HTTPS_LOC_SYMBOL;
        };

    } else {
        rc = nai_value_dups(&args->argv[0], &mod, args->pool_tmp);
        if (rc < 0) {
            nai_log_error(NAI_LOG_HTTPS, 
                nai_sult_to_errno(rc), "get location modify failed");
            goto _end;
        };
        if (nai_str_len(&mod) <= 0) {
            rc = nai_errno_to_sult(EINVAL);
            nai_log_error(NAI_LOG_HTTPS, 
                nai_sult_to_errno(rc), "location modify is empty");
            goto _end;
        };

        rc = nai_value_dups(&args->argv[1], &str, args->pool);
        if (rc < 0) {
            nai_log_error(NAI_LOG_HTTPS, 
                nai_sult_to_errno(rc), "get location name failed");
            goto _end;
        };
        if (nai_str_len(&str) <= 0) {
            rc = nai_errno_to_sult(EINVAL);
            nai_log_error(NAI_LOG_HTTPS, 
                nai_sult_to_errno(rc), "location name is empty");
            goto _end;
        };

        p = nai_str(&mod);
        if (nai_str_len(&mod) == 1 && p[0] == '=') {
            type = NAI_HTTPS_LOC_EXACT;
#if (NAI_HAVE_REGEX)
        } else if (nai_str_len(&mod) == 2 && p[0] == '~' && p[1] == '*') {
            type = NAI_HTTPS_LOC_PATTERN_CASE;
        } else if (nai_str_len(&mod) == 1 && p[0] == '~') {
            type = NAI_HTTPS_LOC_PATTERN;
#endif
        } else if (nai_str_len(&mod) == 1 && p[0] == '^') {
            type = NAI_HTTPS_LOC_PRIORITY;
        } else {
            rc = nai_errno_to_sult(EINVAL);
            nai_log_error(NAI_LOG_HTTPS, 
                nai_sult_to_errno(rc), "invalid location modify");
            goto _end;
        };;
    };

    u = nai_https_get_location(args);
    if (u->type != NAI_HTTPS_LOC_ROOT) {
        switch (type) {
        case NAI_HTTPS_LOC_EXACT:
        case NAI_HTTPS_LOC_PATTERN:
        case NAI_HTTPS_LOC_PATTERN_CASE:
        case NAI_HTTPS_LOC_SYMBOL:
            rc = nai_errno_to_sult(EINVAL);
            nai_log_error(NAI_LOG_HTTPS, 
                nai_sult_to_errno(rc), 
                "location %s can be on server level only", 
                nai_str(&str));
            goto _end;
        default:
            if (u->type != NAI_HTTPS_LOC_NORMAL || 
                u->type != NAI_HTTPS_LOC_PRIORITY) {
                rc = nai_errno_to_sult(EINVAL);
                nai_log_error(NAI_LOG_HTTPS, 
                    nai_sult_to_errno(rc), 
                    "location %s cannot have child location", 
                    nai_str(&u->name));
                goto _end;
            };
        };
        if (type == NAI_HTTPS_LOC_NORMAL) {
            if (nai_str_len(&str) <= nai_str_len(&u->name) || 
                nai_strncmp(nai_str(&u->name), 
                nai_str(&str), nai_str_len(&u->name)) != 0) {

                rc = nai_errno_to_sult(EINVAL);
                nai_log_error(NAI_LOG_HTTPS, 
                    nai_sult_to_errno(rc), 
                    "location %s is outside location %s", 
                    nai_str(&str), nai_str(&u->name));
                goto _end;
            };
        };
    };

    s = nai_https_get_server(args);
    if (s == 0) {
        rc = nai_errno_to_sult(NAI_EINTERNAL);
        nai_log_alert(NAI_LOG_HTTPS, 
            nai_sult_to_errno(rc), 
            "get location failed, return value is null");
        goto _end;
    };

    elts = (nai_https_location_t**)s->table.list.elts;
    for (n = 0; n < (intptr_t)s->table.list.count; n ++) {
        if (nai_str_eq(&str, &elts[n]->name) && type == elts[n]->type) {
            rc = nai_errno_to_sult(EEXIST);
            nai_log_error(NAI_LOG_HTTPS, 
                nai_sult_to_errno(rc), 
                "location '%s' already exists", nai_str(&str));
            goto _end;
        };
    };

    pl = (nai_https_location_t**)nai_array_push(&s->table.list);
    if (pl == 0) {
        rc = nai_sult_from_errno();
        nai_log_alert(NAI_LOG_HTTPS, 
            nai_sult_to_errno(rc), "push location slot failed");
        goto _end;
    };

    h = nai_https_get_service(args);
    rc = nai_https_location_create(h, args->pool, &s->conf, pl);
    if (rc < 0) {
        nai_log_alert(NAI_LOG_HTTPS, 
            nai_sult_to_errno(rc), "create new location failed");
        goto _end;
    };

    pl[0]->name = str;
    pl[0]->type = type;
    pl[0]->parent = u;
    args->grp = NAI_MODULE_HTTPS;
    args->grp_mask = NAI_HTTPS_CLOC_LOC;
    args->clocal = pl[0]->clocal;
    rc = 0;

_end:
    return rc;
};


static nai_sult_t nai_https_root(nai_command_t* c, nai_command_args_t* args)
{
    nai_int_t alias;
    intptr_t n;
    char* b;
    nai_sult_t rc;
    nai_str_t str;
    nai_https_t* h;
    nai_https_location_t* l;


    (void)c;

    l = (nai_https_location_t*)args->data;
    if (l == 0) {
        rc = nai_errno_to_sult(NAI_EINTERNAL);
        nai_log_error(NAI_LOG_HTTPS, 
            nai_sult_to_errno(rc), 
            "get location failed, return value is null");
        goto _end;
    };

    alias = nai_strcmp(c->name, "alias") != 0;
    if (nai_script_expn_is_null(&l->root) == 0) {
        rc = nai_errno_to_sult(EPERM);
        if (alias == (l->alias != 0)) {
            nai_log_error(NAI_LOG_HTTPS, 
                nai_sult_to_errno(rc), 
                "the '%s' was already specified", c->name);
        } else {
            nai_log_error(NAI_LOG_HTTPS, 
                nai_sult_to_errno(rc), 
                "the '%s' was already specified before the '%s'", 
                l->alias ? "alias" : "root", c->name);
        };
        goto _end;
    };

    if (l->type == NAI_HTTPS_LOC_SYMBOL && alias) {
        rc = nai_errno_to_sult(EPERM);
        nai_log_error(NAI_LOG_HTTPS, 
            nai_sult_to_errno(rc), 
            "the 'alias' connot be used in the symbol location");
        goto _end;
    };


    rc = nai_value_getm(&args->argv[0], (nai_mem_t*)&str);
    if (rc < 0) {
        rc = nai_value_dups(&args->argv[0], &str, args->pool_tmp);
        if (rc < 0) {
            nai_log_error(NAI_LOG_HTTPS, 
                nai_sult_to_errno(rc), "get location name failed");
            goto _end;
        };
    };


    if (nai_strstr(nai_str(&str), "$document_root") || 
        nai_strstr(nai_str(&str), "${document_root}")) {
        rc = nai_errno_to_sult(EINVAL);
        nai_log_error(NAI_LOG_HTTPS, 
            nai_sult_to_errno(rc), 
            "the variable '$document_root' connot be used in the '%s'", 
            c->name);
        goto _end;
    };

    if (nai_strstr(nai_str(&str), "$realpath_root") || 
        nai_strstr(nai_str(&str), "${realpath_root}")) {
        rc = nai_errno_to_sult(EINVAL);
        nai_log_error(NAI_LOG_HTTPS, 
            nai_sult_to_errno(rc), 
            "the variable '$realpath_root' connot be used in the '%s'", 
            c->name);
        goto _end;
    };

    /* remove last '/' */
    if (alias == 0 && 
        nai_str_len(&str) > 0 && 
        nai_str_at(&str, nai_str_len(&str)-1) == '/') {
        str.len --;
    };

    if (nai_str_len(&str) <= 0 || 
        nai_str_at(&str, 0) == '$') {
        n = nai_str_dup(&str, nai_str(&str), nai_str_len(&str), args->pool);
        if (n < 0) {
            rc = nai_sult_from_errno();
            nai_log_error(NAI_LOG_HTTPS, 
                nai_sult_to_errno(rc), "duplicate string failed");
            goto _end;
        };
    } else {
        n = nai_path_canonicalize(
            0, 0, nai_str(&str), nai_str_len(&str), nai_path_sep);
        if (n < 0) {
            rc = nai_sult_from_errno();
            nai_log_error(NAI_LOG_HTTPS, 
                nai_sult_to_errno(rc), 
                "nai_path_canonicalize('%.*s') failed", 
                nai_str_len(&str), nai_str(&str));
            goto _end;
        };

        b = (char*)nai_palloc(args->pool, n + 1);
        if (b == 0) {
            rc = nai_sult_from_errno();
            nai_log_alert(NAI_LOG_HTTPS, 
                nai_sult_to_errno(rc), "nai_palloc failed");
            goto _end;
        };

        n = nai_path_canonicalize(
            b, n + 1, nai_str(&str), nai_str_len(&str), nai_path_sep);
        if (n < 0) {
            rc = nai_sult_from_errno();
            nai_log_error(NAI_LOG_HTTPS, 
                nai_sult_to_errno(rc), 
                "nai_path_canonicalize('%.*s') failed", 
                nai_str_len(&str), nai_str(&str));
            goto _end;
        };

        nai_str_setm(&str, b, n);
    };

    h = nai_https_get_service(l);
    rc = nai_https_expn_compile(h, &l->root, &str);
    if (rc < 0) {
        nai_log_error(NAI_LOG_HTTPS, 
            nai_sult_to_errno(rc), 
            "nai_https_expn_compile('%.*s') failed", 
                nai_str_len(&str), nai_str(&str));
        goto _end;
    };

    l->alias = alias;
    if (l->type == NAI_HTTPS_LOC_PATTERN || 
        l->type == NAI_HTTPS_LOC_PATTERN_CASE) {
        l->alias = NAI_INT_T_MAX;
    };

    rc = 0;

_end:
    return rc;
};


static nai_sult_t nai_https_type(
    nai_command_t* c, nai_command_args_t* args)
{
    nai_sult_t rc;
    nai_str_t ext;
    nai_str_t type;
    nai_str_t* ptype;
    nai_hash_value_t* elt;
    nai_https_location_t* l;


    (void)c;

    rc = nai_value_dups(&args->argv[0], &ext, args->pool);
    if (rc < 0) {
        nai_log_error(NAI_LOG_HTTPS, 
            nai_sult_to_errno(rc), "get exten name failed");
        goto _end;
    };

    rc = nai_value_dups(&args->argv[1], &type, args->pool);
    if (rc < 0) {
        nai_log_error(NAI_LOG_HTTPS, 
            nai_sult_to_errno(rc), "get mime type failed");
        goto _end;
    };

    ptype = (nai_str_t*)nai_palloc(args->pool, sizeof(*ptype));
    if (ptype) {
        rc = nai_sult_from_errno();
        nai_log_alert(NAI_LOG_HTTPS, 
            nai_sult_to_errno(rc), "alloc memory to store mime type failed");
        goto _end;
    };

    l = (nai_https_location_t*)args->data;
    elt = (nai_hash_value_t*)nai_array_push(&l->types);
    if (ptype) {
        rc = nai_sult_from_errno();
        nai_log_alert(NAI_LOG_HTTPS, 
            nai_sult_to_errno(rc), "push array to store type failed");
        goto _end;
    };

    nai_str_hash(&ext, 1);
    ptype[0] = type;
    elt->key = ext;
    elt->value = ptype;

    rc = 0;

_end:
    return rc;
};


static nai_sult_t nai_https_limit_except(
    nai_command_t* c, nai_command_args_t* args)
{
    nai_int_t n, m;
    nai_sult_t rc;
    nai_mem_t str;
    nai_https_t* h;
    nai_https_location_t* l;


    (void)c;

    if (args->endseg) {
        rc = 0;
        goto _end;
    };

    l = (nai_https_location_t*)args->data;
    l->limit_except = 0;
    for (n = 0; n < args->argc; n ++) {
        rc = nai_value_getm(&args->argv[n], &str);
        if (rc < 0) {
            nai_log_error(NAI_LOG_HTTPS, 
                nai_sult_to_errno(rc), "get method name failed");
            goto _end;
        };

        for (m = 0; m < (nai_int_t)nai_countof(nai_https_methods); m ++) {
            if (nai_strncmp(nai_https_methods[m].name, 
                nai_str(&str), nai_str_len(&str)) == 0) {
                break;
            };
        };
        if (m >= (nai_int_t)nai_countof(nai_https_methods)) {
            rc = nai_errno_to_sult(EINVAL);
            nai_log_error(NAI_LOG_HTTPS, 
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
        h = nai_https_get_service(l);
        rc = nai_https_location_create(
            h, args->pool, &l->conf, &l->limit_conf);
        if (rc < 0) {
            nai_log_alert(NAI_LOG_HTTPS, 
                nai_sult_to_errno(rc), "create new location failed");
            goto _end;
        };

        l->limit_conf->name = l->name;
        l->limit_conf->type = NAI_HTTPS_LOC_NONAME;
        l->limit_conf->parent = l;
        l->limit_conf->sibling = l->sibling;
        l->sibling = l->limit_conf;
    };


    args->grp = NAI_MODULE_HTTPS;
    args->grp_mask = NAI_HTTPS_CLOC_LMT;
    args->clocal = l->limit_conf->clocal;

    rc = 0;

_end:
    return rc;
};


static nai_sult_t nai_https_internal(
    nai_command_t* c, nai_command_args_t* args)
{
    nai_sult_t rc;
    nai_https_location_t* l;


    (void)c;

    l = (nai_https_location_t*)args->data;
    l->internal = 1;

    rc = 0;
    return rc;
};


//////////////////////////////////////////////////////////////////////////////
// https listen



static nai_int_t nai_https_accept(
    nai_listening_t* p, nai_fd_t f, const nai_sockname_t* name)
{
    nai_sult_t rc;
    nai_https_connection_t* c;


    /* create connection */
    rc = nai_https_connection_create(p, f, name, &c);
    if (rc < 0) {
        nai_log_alert(NAI_LOG_HTTPS, 
            nai_sult_to_errno(rc), "alloc connection failed");
        goto _end;
    };


    /* open stream */
    rc = nai_stream_open(&c->c.str, nai_server_get_loop(&p->l));
    if (rc < 0) {
        rc = nai_sult_from_errno();
        nai_log_error(NAI_LOG_HTTPS, 
            nai_sult_to_errno(rc), "open stream of connection failed");
        goto _end;
    };


    /* connection start */
    nai_https_handle_start(c);


    rc = 0;

_end:
    if (rc < 0) {
        if (c) {
            nai_https_connection_close(c);
        } else if (f != NAI_FD_INVALID) {
            nai_sock_close(f);
        };
        nai_errno = nai_sult_to_errno(rc);
        rc = -1;
    };
    return rc;
};


static nai_sult_t nai_https_bind(
    nai_https_t* h, nai_https_server_t* hs, 
    const nai_sockaddr_t* name, nai_int_t namelen, 
    const nai_https_listening_opt_t* opts)
{
    nai_int_t n;
    nai_sult_t rc;
    nai_listening_t* s;
    nai_main_t* m = h->main;
    nai_main_event_t* e;
    nai_https_server_t** ps;
    nai_https_listening_t* l;
    nai_https_listening_t** pl;


    e = nai_main_event_get(m);

    s = nai_listening_map_find(&e->ls, name, namelen);
    if (s) {
        if (s->cb != nai_https_accept) {
            rc = nai_errno_to_sult(EEXIST);
            nai_log_error(NAI_LOG_HTTPS, 
                nai_sult_to_errno(rc), 
                "the address has already been bind by other service");
            goto _end;
        };
    } else {
        l = (nai_https_listening_t*)nai_palloc(&m->pool, sizeof(*l));
        if (l == 0) {
            rc = nai_sult_from_errno();
            nai_log_error(NAI_LOG_HTTPS, 
                nai_sult_to_errno(rc), "alloc struct of listen failed");
            goto _end;
        };

        l->default_serv = 0;
        nai_memset(&l->opts, 0, sizeof(l->opts));
        nai_array_init(&l->servers, 
            sizeof(nai_https_server_t*), &m->pool);
        nai_array_init(&l->names, 
            sizeof(nai_https_server_name_t*), &m->pool);
        nai_array_init(&l->match, 
            sizeof(nai_https_server_name_t*), &m->pool);

        s = nai_listening_map_bind(&e->ls, name, namelen, l, nai_https_accept);
        if (s == 0) {
            rc = nai_sult_from_errno();
            nai_log_error(NAI_LOG_HTTPS, 
                nai_sult_to_errno(rc), "bind address failed");
            goto _end;
        };

        h = nai_https_get_service(hs);
        pl = (nai_https_listening_t**)nai_array_push(&h->listening);
        if (pl == 0) {
            rc = nai_sult_from_errno();
            nai_log_error(NAI_LOG_HTTPS, 
                nai_sult_to_errno(rc), "add a new listen into https failed");
            goto _end;
        };

        pl[0] = l;
    };

    l = (nai_https_listening_t*)s->key;
    ps = (nai_https_server_t**)l->servers.elts;
    for (n = 0; n < (intptr_t)l->servers.count; n ++) {
        if (ps[n] == hs) {
            rc = nai_errno_to_sult(EEXIST);
            nai_log_error(NAI_LOG_HTTPS, 
                nai_sult_to_errno(rc), "the address has already been bind");
            goto _end;
        };
    };

    if (opts->default_one) {
        if (l->default_serv) {
            rc = nai_errno_to_sult(EINVAL);
            nai_log_error(NAI_LOG_HTTPS, 
                nai_sult_to_errno(rc), "default server settings conflict");
            goto _end;
        };

        l->default_serv = hs;
    };
    if (opts->seted) {
        if (l->opts.seted) {
            rc = nai_errno_to_sult(EINVAL);
            nai_log_error(NAI_LOG_HTTPS, 
                nai_sult_to_errno(rc), "listen settings conflict");
            goto _end;
        };

        l->opts.seted = 1;
        s->reuse = l->opts.reuse = opts->reuse;
        s->backlog = l->opts.backlog = opts->backlog;
        s->sendbuf = (uint32_t)(l->opts.sendbuf = opts->sendbuf);
        s->recvbuf = (uint32_t)(l->opts.recvbuf = opts->recvbuf);
    };

    l->opts.ssl |= opts->ssl;


    ps = (nai_https_server_t**)nai_array_push(&l->servers);
    if (ps == 0) {
        rc = nai_sult_from_errno();
        nai_log_error(NAI_LOG_HTTPS, 
            nai_sult_to_errno(rc), 
            "add a new server into https_listen failed");
        goto _end;
    };

    ps[0] = hs;
    rc = 0;

_end:
    return rc;
};


static nai_sult_t nai_https_listen_merge(nai_https_listening_t* l)
{
    nai_int_t i, k;
    nai_sult_t rc;
    nai_https_server_t* s;
    nai_https_server_t** sa;
    nai_https_server_name_t* n;
    nai_https_server_name_t* na;
    nai_https_server_name_t** nn;


    /* set first server as default server when default server isn't set */
    sa = (nai_https_server_t**)l->servers.elts;
    if (l->default_serv == 0) {
        l->default_serv = sa[0];
    };

    /* push exact names */
    for (i = 0; i < (intptr_t)l->servers.count; i ++) {
        s = sa[i];
        na = (nai_https_server_name_t*)s->names.elts;
        for (k = 0; k < (intptr_t)s->names.count; k ++) {
            n = &na[k];
            if (n->type != NAI_HTTPS_SERVNAME_NORM) {
                continue;
            };

            rc = nai_https_server_find(l, (nai_mem_t*)&n->name, 0);
            if (rc < 0) {
                goto _end;
            };

            nn = (nai_https_server_name_t**)nai_array_push(&l->names);
            if (nn == 0) {
                rc = nai_sult_from_errno();
                nai_log_alert(NAI_LOG_HTTPS, 
                    nai_sult_to_errno(rc), 
                    "push name of server into listen failed");
                goto _end;
            };

            nn[0] = n;
        };
    };

    /* push patterns */
    for (i = 0; i < (intptr_t)l->servers.count; i ++) {
        s = sa[i];
        na = (nai_https_server_name_t*)s->names.elts;
        for (k = 0; k < (intptr_t)s->names.count; k ++) {
            n = &na[k];
            if (n->type == NAI_HTTPS_SERVNAME_NORM) {
                continue;
            };

            nn = (nai_https_server_name_t**)nai_array_push(&l->match);
            if (nn == 0) {
                rc = nai_sult_from_errno();
                nai_log_alert(NAI_LOG_HTTPS, 
                    nai_sult_to_errno(rc), 
                    "push pattern of server into listen failed");
                goto _end;
            };

            nn[0] = n;
        };
    };


    rc = 0;

_end:
    return rc;
};


nai_sult_t nai_https_server_find(
    nai_https_listening_t* l, const nai_mem_t* name, 
    nai_https_server_t** pv)
{
    nai_int_t i;
    nai_sult_t rc;
    nai_https_server_t* s = 0;
    nai_https_server_name_t* n;
    nai_https_server_name_t** na;
    nai_mem_t str = *name;


    if (l->names.count > 0) {
        nai_str_hash(&str, 1);
        na = (nai_https_server_name_t**)l->names.elts;
        for (i = 0; i < (intptr_t)l->names.count; i ++) {
            n = na[i];
            if (nai_str_hashcaseeq(&n->name, &str)) {
                s = n->server;
                goto _find;
            };
        };
    };

    na = (nai_https_server_name_t**)l->match.elts;
    for (i = 0; i < (intptr_t)l->match.count; i ++) {
        n = na[i];
        switch (n->type) {
#if (NAI_HAVE_REGEX)
        case NAI_HTTPS_SERVNAME_REGEX:
            if (nai_regex_exec(&n->pattern, 
                nai_str(&str), nai_str_len(&str), 0, 0) == 0) {
                s = n->server;
                goto _find;
            };
            break;
#endif
        case NAI_HTTPS_SERVNAME_WILD:
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


//////////////////////////////////////////////////////////////////////////////
// https location


static nai_sult_t nai_https_location_find(
    nai_https_locations_t* t, const nai_str_t* uri, nai_https_location_t** pv)
{
    nai_int_t c;
    nai_int_t cmp;
    nai_int_t len;
    const char* str;
    nai_sult_t rc;
    nai_rbnode_t** e;
    nai_rbnode_t* parent;
    nai_https_location_t* find = 0;
    nai_https_location_node_t* node;
#if (NAI_HAVE_REGEX)
    nai_int_t n;
    nai_https_location_entry_t* ent;
    nai_https_location_entry_t** elts;
#endif


    str = nai_str(uri);
    len = nai_str_len(uri);

    e = &nai_rbtree_root(&t->named);
    while (*e) {
        parent = *e;
        node = (nai_https_location_node_t*)parent;

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

    if (find && find->type != NAI_HTTPS_LOC_PRIORITY) {
        rc = 0;
        goto _end;
    };

    str = nai_str(uri);
    len = nai_str_len(uri);

#if (NAI_HAVE_REGEX)
    elts = (nai_https_location_entry_t**)t->patterns.elts;
    for (n = 0; n < (intptr_t)t->patterns.count; n ++) {
        ent = elts[n];
        c = nai_regex_exec(&ent->pattern, str, len, 0, 0);
        if (c < 0) {
            if (c == -1) {
                continue;
            };

            rc = nai_sult_from_errno();
            nai_log_alert(NAI_LOG_HTTPS, 
                nai_sult_to_errno(rc), 
                "nai_regex_exec failed when find location '%.*s', error %d", 
                len, str, c);
            goto _end;
        };

        find = ent->loc;
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
// https core stage


static nai_sult_t nai_https_core_all_route(
    nai_https_request_t* r, nai_https_stage_t* s)
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


static nai_sult_t nai_https_core_once_route(
    nai_https_request_t* r, nai_https_stage_t* s)
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


static nai_sult_t nai_https_core_find_config(
    nai_https_request_t* r, nai_https_stage_t* stage)
{
    nai_sult_t rc;
    nai_https_server_t* s;
    nai_https_location_t* l = 0;


    r->redirected = 0;
    s = nai_https_get_server(r);

    rc = nai_https_location_find(&s->table, &r->uri, &l);
    switch (rc) {
    case NAI_OK:
        r->conf = l->conf;
        break;

    case NAI_DECLINED:
        l = nai_https_get_location(r);
        break;

    default:
        goto _end;
    };

    if (!r->internal && l->internal) {
        rc = NAI_HTTP_NOT_FOUND;
        goto _end;
    };

    rc = nai_https_core_update(r);
    if (rc < 0) {
        goto _end;
    };

    /* max_body_size not in limit_except */
    if (r == nai_https_request_main(r) && 
        r->headers_in.content_length_n != (uint64_t)-1 && 
        l->max_body_size > 0 && 
        r->headers_in.content_length_n > (uint64_t)l->max_body_size) {
        rc = NAI_HTTP_REQUEST_ENTITY_TOO_LARGE;
        goto _end;
    };

    r->stage = stage->next;

_end:
    return rc;
};


static nai_sult_t nai_https_core_rewrite_route(
    nai_https_request_t* r, nai_https_stage_t* s)
{
    nai_sult_t rc;
    nai_https_t* h;


    rc = s->handle(r);

    switch (rc) {
    case NAI_OK:
        r->stage ++;
        break;
    case NAI_DECLINED:
        r->stage ++;
        break;
    case NAI_DONE:
        h = nai_https_get_service(r);
        r->stage = h->groups[NAI_HTTPS_STAGE_LOG].start_index;
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


static nai_sult_t nai_https_core_post_rewrite_route(
    nai_https_request_t* r, nai_https_stage_t* s)
{
    nai_sult_t rc;


    if (!r->redirected) {
        r->stage ++;
        rc = NAI_OK;
        goto _end;
    };

    r->redirects --;
    if (r->redirects <= 0) {
        rc = NAI_HTTP_INTERNAL_SERVER_ERROR;
        goto _end;
    };

    r->conf = nai_https_get_server(r)->conf;
    r->stage = s->next;
    rc = NAI_OK;

_end:
    return rc;
};


static nai_sult_t nai_https_core_access_route(
    nai_https_request_t* r, nai_https_stage_t* s)
{
    nai_sult_t rc;
    nai_https_t* h;
    nai_https_location_t* l;


    if (r != nai_https_request_main(r)) {
        rc = NAI_OK;
        goto _end;
    };

    rc = s->handle(r);

    switch (rc) {
    case NAI_DECLINED:
        r->stage ++;
        break;
    case NAI_DONE:
        h = nai_https_get_service(r);
        r->stage = h->groups[NAI_HTTPS_STAGE_LOG].start_index;
        break;
    case NAI_OK:
        l = nai_https_get_location(r);
        if (l->satisfy == NAI_HTTPS_ALWAYS) {
            r->stage ++;
        } else {
            r->access = 0;
            r->stage = s->next;
            if (r->headers_out.www_authenticate) {
                nai_list_entry_remove(&r->headers_out.www_authenticate->ent);
                r->headers_out.www_authenticate = 0;
            };
        };
        break;
    case NAI_HTTP_FORBIDDEN:
    case NAI_HTTP_UNAUTHORIZED:
        l = nai_https_get_location(r);
        if (l->satisfy != NAI_HTTPS_ALWAYS) {
            if (r->access != NAI_HTTP_UNAUTHORIZED) {
                r->access = rc;
            };
            r->stage ++;
            rc = NAI_OK;
        };
        break;
    default:
        if (rc > 0 && rc < NAI_HTTP_OK) {
            r->stage ++;
            rc = NAI_DECLINED;
        };
        break;
    };

_end:
    return rc;
};


static nai_sult_t nai_https_core_post_access_route(
    nai_https_request_t* r, nai_https_stage_t* s)
{
    nai_sult_t rc;


    rc = r->access;
    if (rc) {
        r->access = 0;

        if (rc == NAI_HTTP_UNAUTHORIZED) {
        };

        if (rc == NAI_HTTP_FORBIDDEN) {
            nai_log_error(NAI_LOG_HTTPS, 0, "request forbidden");
        };

        goto _end;
    };

    r->stage = s->next;

_end:
    return rc;
};


static nai_sult_t nai_https_core_content_route(
    nai_https_request_t* r, nai_https_stage_t* s)
{
    size_t len;
    nai_sult_t rc;


    rc = s->handle(r);
    if (rc != NAI_DECLINED) {
        goto _end;
    };

    s ++;
    if (s->route) {
        r->stage ++;
        rc = NAI_DECLINED;
        goto _end;
    };

    len = nai_str_len(&r->uri);
    if (len > 0 && nai_str_at(&r->uri, len - 1) == '/') {
        rc = NAI_HTTP_FORBIDDEN;
        goto _end;
    };

    rc = NAI_HTTP_NOT_FOUND;

_end:
    return rc;
};


static nai_int_t nai_https_core_compare_stage(const void* a, const void* b)
{
    nai_int_t r;
    nai_https_stage_entry_t* e0;
    nai_https_stage_entry_t* e1;


    e0 = (nai_https_stage_entry_t*)a;
    e1 = (nai_https_stage_entry_t*)b;
    r = e0->priority - e1->priority;

    return r;
};


static nai_sult_t nai_https_core_stage_init(nai_https_t* h)
{
    nai_int_t n, i;
    nai_int_t next;
    nai_int_t count;
    nai_int_t access, rewrite;
    nai_sult_t rc;
    nai_main_t* m = h->main;
    nai_https_route_f route;
    nai_https_stage_t* s;
    nai_https_stage_entry_t* entries;


    count = 1; /* find config */
    for (n = 0; n < NAI_HTTPS_STAGE_RESPONSE; n ++) {
        count += (nai_int_t)h->groups[n].handles.count;
    };

    access = 0;
    if (h->groups[NAI_HTTPS_STAGE_ACCESS].handles.count) {
        count ++;
        access = 1;
    };
    rewrite = 0;
    if (h->groups[NAI_HTTPS_STAGE_REWRITE].handles.count) {
        count ++;
        rewrite = 1;
    };

    h->stages = nai_palloc(&m->pool, sizeof(nai_https_stage_t) * (count+1));
    if (h->stages == 0) {
        rc = nai_sult_from_errno();
        nai_log_alert(NAI_LOG_HTTPS, 
            nai_sult_to_errno(rc), "failed to allocate stage array");
        goto _end;
    };

    s = h->stages;
    next = 0;
    for (n = 0; n < NAI_HTTPS_STAGE_RESPONSE; n ++) {
        count = (nai_int_t)h->groups[n].handles.count;
        switch (n) {
        case NAI_HTTPS_STAGE_PREPARE:
            route = nai_https_core_all_route;
            break;

        case NAI_HTTPS_STAGE_SERVER:
            route = nai_https_core_rewrite_route;
            break;

        case NAI_HTTPS_STAGE_FIND_CONFIG:
            if (1) {
                next ++;
                s->route = nai_https_core_find_config;
                s->handle = 0;
                s->next = next + count;
                s ++;
            };
            route = nai_https_core_once_route;
            break;

        case NAI_HTTPS_STAGE_REWRITE:
            if (1) {
                /* fix start index of find_config */
                h->groups[NAI_HTTPS_STAGE_FIND_CONFIG].start_index --;
            };
            route = nai_https_core_rewrite_route;
            break;

        case NAI_HTTPS_STAGE_ACCESS:
            if (rewrite) {
                next ++;
                s->route = nai_https_core_post_rewrite_route;
                s->handle = 0;
                s->next = h->groups[NAI_HTTPS_STAGE_FIND_CONFIG].start_index;
                s ++;
            };

            route = nai_https_core_access_route;
            break;

        case NAI_HTTPS_STAGE_CONTENT:
            if (access) {
                next ++;
                s->route = nai_https_core_post_access_route;
                s->handle = 0;
                s->next = next;
                s ++;
            };

            route = nai_https_core_content_route;
            break;

        default:
            assert(0);
            break;
        };

        entries = (nai_https_stage_entry_t*)h->groups[n].handles.elts;
        if (count > 1) {
            qsort(entries, count, 
                sizeof(*entries), nai_https_core_compare_stage);
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


nai_sult_t nai_https_core_run_stage(nai_https_request_t* r)
{
    nai_sult_t rc;
    nai_https_t* h;
    nai_https_stage_t* st;
    nai_https_stage_t* stages;


    h = nai_https_get_service(r);
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


nai_sult_t nai_https_core_handle(nai_https_request_t* r)
{
    nai_sult_t rc;
    nai_https_t* h;


    if (r->internal == 0) {
        switch (r->headers_in.connection_type) {
        case NAI_HTTP_CLOSE:
            r->keepalive = 0;
            break;
        case NAI_HTTP_KEEPALIVE:
            r->keepalive = 1;
            break;
        case 0:
            r->keepalive = r->version >= NAI_HTTP_10;
            break;
        default:
            break;
        };

        r->stage = 0;

    } else {

        h = nai_https_get_service(r);
        r->stage = h->groups[NAI_HTTPS_STAGE_SERVER].start_index;
    };

    r->send = nai_https_core_run_stage;
    rc = r->send(r);

    return rc;
};


nai_sult_t nai_https_core_update(nai_https_request_t* r)
{
    nai_sult_t rc;
    nai_https_location_t* l;
    nai_https_protocol_t* p;
    nai_https_connection_t* c;


    p = r->proto;
    if (p->main == r) {
        p->timeoset &= NAI_EV_WRITE; /* clear read timeout seted flags */
    };
    if (p->request == r) {
        p->timeoset &= NAI_EV_READ;  /* clear send timeout seted flags */
    };


    /* check methods */
    l = nai_https_get_location(r);
    if (l->limit_except & (1 << r->method)) {
        /* limit methods */
    } else {
        l = l->limit_conf;
        r->conf = l->conf;
    };


    /* set keep alive */
    c = r->connection;
    if (r->keepalive) {
        if (l->keepalive_timeo == 0) {
            r->keepalive = 0;
        } else if (l->keepalive_max >= c->requests) {
            r->keepalive = 0;
        };
    };


    rc = 0;

    return rc;
};


nai_sult_t nai_https_add_stage(
    nai_main_t* m, nai_int_t stage, nai_int_t priority, 
    nai_https_handle_f handle)
{
    nai_sult_t rc;
    nai_https_t* h;
    nai_https_stage_entry_t* ent;


    if (handle == 0) {
        rc = nai_errno_to_sult(EINVAL);
        goto _end;
    };

    if (stage < NAI_HTTPS_STAGE_SERVER || 
        stage >= NAI_HTTPS_STAGE_MAX) {
        rc = nai_errno_to_sult(EINVAL);
        goto _end;
    };

    h = nai_https_from_main(m);
    if (h == 0) {
        rc = nai_errno_to_sult(EPERM);
        goto _end;
    };

    ent = (nai_https_stage_entry_t*)nai_array_push(&h->groups[stage].handles);
    if (ent == 0) {
        rc = nai_sult_from_errno();
        nai_log_alert(NAI_LOG_HTTPS, 
            nai_sult_to_errno(rc), "push array failed when add stage");
        goto _end;
    };

    ent[0].handle = handle;
    ent[0].priority = priority;
    rc = 0;

_end:
    return rc;
};


