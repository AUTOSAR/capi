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
/// @file       nai_httpc_core_variable.c
/// @brief      
/// @details
/// @date       2021-09-17
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai_httpc_core.h"
#include "nhttp/client/nai_httpc_script.h"
#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_log.h"
#include <stdio.h>
#include <stdarg.h>



static nai_sult_t nai_httpc_variable_capture(
    void* c, uintptr_t ud, nai_script_value_t* v);
static nai_sult_t nai_httpc_variable_string(
    void* c, uintptr_t ud, nai_script_value_t* v);
static nai_sult_t nai_httpc_variable_header(
    void* c, uintptr_t ud, nai_script_value_t* v);
static nai_sult_t nai_httpc_variable_headers(
    void* c, uintptr_t ud, nai_script_value_t* v);
static nai_sult_t nai_httpc_variable_headers_impl(
    void* c, uintptr_t ud, nai_script_value_t* v, char sep);
static nai_sult_t nai_httpc_variable_unknown_header_in(
    void* c, uintptr_t ud, nai_script_value_t* v);
static nai_sult_t nai_httpc_variable_unknown_header_out(
    void* c, uintptr_t ud, nai_script_value_t* v);


static nai_sult_t nai_httpc_variable_request_time(
    void* c, uintptr_t ud, nai_script_value_t* v);
static nai_sult_t nai_httpc_variable_scheme(
    void* c, uintptr_t ud, nai_script_value_t* v);
static nai_sult_t nai_httpc_variable_https(
    void* c, uintptr_t ud, nai_script_value_t* v);
static nai_sult_t nai_httpc_variable_is_args(
    void* c, uintptr_t ud, nai_script_value_t* v);
static nai_sult_t nai_httpc_variable_arg(
    void* c, uintptr_t ud, nai_script_value_t* v);
static nai_sult_t nai_httpc_variable_content_length(
    void* c, uintptr_t ud, nai_script_value_t* v);

static nai_sult_t nai_httpc_variable_status(
    void* c, uintptr_t ud, nai_script_value_t* v);
static nai_sult_t nai_httpc_variable_sent_content_length(
    void* c, uintptr_t ud, nai_script_value_t* v);


static nai_sult_t nai_httpc_variable_protocol_uint64(
    void* c, uintptr_t ud, nai_script_value_t* v);


static nai_sult_t nai_httpc_variable_connection_time(
    void* c, uintptr_t ud, nai_script_value_t* v);
static nai_sult_t nai_httpc_variable_connection_addr(
    void* c, uintptr_t ud, nai_script_value_t* v);
static nai_sult_t nai_httpc_variable_connection_port(
    void* c, uintptr_t ud, nai_script_value_t* v);

static nai_sult_t nai_httpc_variable_msec(
    void* c, uintptr_t ud, nai_script_value_t* v);
static nai_sult_t nai_httpc_variable_time_local(
    void* c, uintptr_t ud, nai_script_value_t* v);
static nai_sult_t nai_httpc_variable_time_iso8601(
    void* c, uintptr_t ud, nai_script_value_t* v);



static nai_script_variable_t nai_httpc_core_vars[] = {

    /* request */
    { nai_strconst("status"), nai_httpc_variable_status, 0 }, 
    { nai_strconst("scheme"), nai_httpc_variable_scheme, 0 },
    { nai_strconst("https"), nai_httpc_variable_https, 0 },
    { nai_strconst("host"), nai_httpc_variable_string, 0, 
      nai_offsetof(nai_httpc_request_t, parsed_uri.hostname), 0 },
    { nai_strconst("method"), nai_httpc_variable_string, 0, 
      nai_offsetof(nai_httpc_request_t, method_name), 0 }, 
    { nai_strconst("uri"), nai_httpc_variable_string, 0,
      nai_offsetof(nai_httpc_request_t, uri), NAI_SCRIPT_VAR_NOCACHEABLE },
    { nai_strconst("document_uri"), nai_httpc_variable_string, 0,
      nai_offsetof(nai_httpc_request_t, uri), NAI_SCRIPT_VAR_NOCACHEABLE },
    { nai_strconst("request_uri"), nai_httpc_variable_string, 0,
      nai_offsetof(nai_httpc_request_t, unparsed_uri), 0 },
    { nai_strconst("is_args"), nai_httpc_variable_is_args, 0 },
    { nai_strconst("args"), nai_httpc_variable_string, 0, 
      nai_offsetof(nai_httpc_request_t, args), 0 },
    { nai_strconst("arg_"), nai_httpc_variable_arg, 0,
      0, NAI_SCRIPT_VAR_NOCACHEABLE| NAI_SCRIPT_VAR_PREFIX },
    { nai_strconst("query_string"), nai_httpc_variable_string, 0,
      nai_offsetof(nai_httpc_request_t, parsed_uri.query), 0 },

    { nai_strconst("request_time"), nai_httpc_variable_request_time, 0,
      nai_offsetof(nai_httpc_request_t, args), 0 },

    /* input headers */
    { nai_strconst("content_length"), nai_httpc_variable_content_length, 0 },
    { nai_strconst("content_type"), nai_httpc_variable_header, 0,
      nai_offsetof(nai_httpc_request_t, headers_in.content_type), 0 },
    { nai_strconst("http_connection"), nai_httpc_variable_header, 0,
      nai_offsetof(nai_httpc_request_t, headers_in.connection), 0 },
    { nai_strconst("http_keep_alive"), nai_httpc_variable_header, 0,
      nai_offsetof(nai_httpc_request_t, headers_in.keep_alive), 0 },
    { nai_strconst("http_location"), nai_httpc_variable_header, 0,
      nai_offsetof(nai_httpc_request_t, headers_in.location), 0 },
    { nai_strconst("http_last_modified"), nai_httpc_variable_header, 0,
      nai_offsetof(nai_httpc_request_t, headers_in.last_modified), 0 },
    { nai_strconst("http_transfer_encoding"), nai_httpc_variable_header, 0,
      nai_offsetof(nai_httpc_request_t, headers_in.transfer_encoding), 0 },
    { nai_strconst("http_"), nai_httpc_variable_unknown_header_in, 0,
      0, NAI_SCRIPT_VAR_PREFIX },

    /* output headers */
    { nai_strconst("sent_http_content_length"), 
      nai_httpc_variable_sent_content_length, 0 },
    { nai_strconst("sent_http_host"), nai_httpc_variable_header, 0, 
      nai_offsetof(nai_httpc_request_t, headers_out.host), 0 },
    { nai_strconst("sent_http_user_agent"), nai_httpc_variable_header, 0,
      nai_offsetof(nai_httpc_request_t, headers_out.user_agent), 0 },
    { nai_strconst("sent_http_referer"), nai_httpc_variable_header, 0,
      nai_offsetof(nai_httpc_request_t, headers_out.referer), 0 },
    { nai_strconst("sent_http_content_type"), nai_httpc_variable_header, 0,
      nai_offsetof(nai_httpc_request_t, headers_out.content_type), 0 },
    { nai_strconst("sent_http_transfer_encoding"), nai_httpc_variable_header, 0,
      nai_offsetof(nai_httpc_request_t, headers_out.transfer_encoding), 0 },
    { nai_strconst("sent_http_"), nai_httpc_variable_unknown_header_out, 0,
      0, NAI_SCRIPT_VAR_PREFIX },

    /* protocol */
    { nai_strconst("body_sent"), nai_httpc_variable_protocol_uint64, 0, 
      nai_offsetof(nai_httpc_protocol_t, send_body), 
      NAI_SCRIPT_VAR_NOCACHEABLE },
    { nai_strconst("body_read"), nai_httpc_variable_protocol_uint64, 0, 
      nai_offsetof(nai_httpc_protocol_t, read_body), 
      NAI_SCRIPT_VAR_NOCACHEABLE },

    /* connection */
    { nai_strconst("connection_time"), 
      nai_httpc_variable_connection_time, 0, 0, 0 },
    { nai_strconst("client_addr"), 
      nai_httpc_variable_connection_addr, 0, NAI_HTTPC_SOCKNAME, 0 },
    { nai_strconst("client_port"), 
      nai_httpc_variable_connection_port, 0, NAI_HTTPC_SOCKNAME, 0 },
    { nai_strconst("remote_addr"), 
      nai_httpc_variable_connection_addr, 0, NAI_HTTPC_PEERNAME, 0 },
    { nai_strconst("remote_port"), 
      nai_httpc_variable_connection_port, 0, NAI_HTTPC_PEERNAME, 0 },
    { nai_strconst("proxy_addr"), 
      nai_httpc_variable_connection_addr, 0, NAI_HTTPC_PROXYNAME, 0 },
    { nai_strconst("proxy_port"), 
      nai_httpc_variable_connection_port, 0, NAI_HTTPC_PROXYNAME, 0 },

    { nai_strconst("msec"), 
      nai_httpc_variable_msec, 0, 0, NAI_SCRIPT_VAR_NOCACHEABLE },
    { nai_strconst("time_local"), 
      nai_httpc_variable_time_local, 0, 0, NAI_SCRIPT_VAR_NOCACHEABLE },
    { nai_strconst("time_iso8601"), 
      nai_httpc_variable_time_iso8601, 0, 0, NAI_SCRIPT_VAR_NOCACHEABLE },
};



nai_sult_t nai_httpc_core_variable_init(nai_httpc_t* h)
{
    nai_int_t n;
    char name[2];
    nai_sult_t rc;
    nai_script_variable_t var;


    rc = nai_script_add_variables(&h->vars, 
        nai_httpc_core_vars, nai_countof(nai_httpc_core_vars));
    if (rc < 0) {
        nai_log_error(NAI_LOG_HTTPC, 
            nai_sult_to_errno(rc), "add core variables failed");
        goto _end;
    };

    nai_str_setm(&var.name, name, 1);
    var.get = nai_httpc_variable_capture;
    var.set = 0;
    var.flags = NAI_SCRIPT_VAR_NOCACHEABLE;
    for (n = 0; n < 10; n ++) {
        name[0] = '0' + n;
        name[1] = 0;
        var.ud = n;
        rc = nai_script_add_variables(&h->vars, &var, 1);
        if (rc < 0) {
            nai_log_error(NAI_LOG_HTTPC, 
                nai_sult_to_errno(rc), 
                "add core variable '%s' failed", name);
            goto _end;
        };
    };

_end:
    return rc;
};


#if NAI_SIZEOF_SIZE_T == 4
#define nai_httpc_uintptr_value     nai_httpc_uint32_value
#else
#define nai_httpc_uintptr_value     nai_httpc_uint64_value
#endif


static nai_sult_t nai_httpc_uint32_value(
    nai_pool_t* p, nai_str_t* s, uint32_t v)
{
    nai_int_t len;
    uint32_t n;
    char* buf;
    char* d;
    nai_sult_t rc;


    len = NAI_INT32_T_LEN + 1;
    buf = (char*)nai_palloc(p, len);
    if (buf == 0) {
        nai_str_setm(s, 0, 0);
        rc = nai_sult_from_errno();
        goto _end;
    };

    len = 0;
    n = v;
    d = buf + NAI_INT32_T_LEN;
    while (n) {
        d --;
        d[0] = n % 10 + '0';
        n /= 10;
        len ++;
    };

    d[len] = 0;
    nai_memcpy(buf, d, len + 1);
    nai_str_setm(s, buf, len);
    rc = 0;

_end:
    return rc;
};


static nai_sult_t nai_httpc_uint64_value(
    nai_pool_t* p, nai_str_t* s, uint64_t v)
{
    nai_int_t len;
    uint64_t n;
    char* buf;
    char* d;
    nai_sult_t rc;


    len = NAI_INT64_T_LEN + 1;
    buf = (char*)nai_palloc(p, len);
    if (buf == 0) {
        nai_str_setm(s, 0, 0);
        rc = nai_sult_from_errno();
        goto _end;
    };

    len = 0;
    n = v;
    d = buf + NAI_INT32_T_LEN;
    while (n) {
        d --;
        d[0] = n % 10 + '0';
        n /= 10;
        len ++;
    };

    d[len] = 0;
    nai_memcpy(buf, d, len + 1);
    nai_str_setm(s, buf, len);
    rc = 0;

_end:
    return rc;
};


static nai_sult_t nai_httpc_print_value(
    nai_pool_t* p, nai_str_t* s, size_t max, const char* fmt, ...)
{
    char* buf;
    size_t len;
    va_list va;
    nai_sult_t rc;


    len = max;
    buf = (char*)nai_palloc(p, len);
    if (buf == 0) {
        nai_str_setm(s, 0, 0);
        rc = nai_sult_from_errno();
        goto _end;
    };

    va_start(va, fmt);
    len = nai_vsnprintf(buf, len, fmt, va);
    va_end(va);

    nai_str_setm(s, buf, len);
    rc = 0;

_end:
    return rc;
};


static nai_sult_t nai_httpc_variable_capture(
    void* c, uintptr_t ud, nai_script_value_t* v)
{
    nai_sult_t rc;


#if (NAI_HAVE_REGEX)
    nai_int_t n;
    nai_httpc_request_t* r;


    n = (nai_int_t)ud;
    r = (nai_httpc_request_t*)c;
    if (r->ncapture <= n) {
        v->len = 0;
        v->not_found = 1;
        rc = 0;
        goto _end;
    };

    v->len = r->captures[n*2+1]-r->captures[n*2];
    v->ptr = (char*)r->match_data + r->captures[n*2];
    rc = 0;

_end:
    return rc;
#else
    (void)c;
    (void)ud;

    v->len = 0;
    v->not_found = 1;
    rc = 0;

    return rc;
#endif
};


static nai_sult_t nai_httpc_variable_string(
    void* c, uintptr_t ud, nai_script_value_t* v)
{
    nai_mem_t* s;


    s = (nai_mem_t*)((uint8_t*)c + ud);
    v->len = s->len;
    v->ptr = s->ptr;

    return 0;
};


static nai_sult_t nai_httpc_variable_header(
    void* c, uintptr_t ud, nai_script_value_t* v)
{
    nai_sult_t rc;
    nai_httpc_header_t* h;


    h = *(nai_httpc_header_t**)((uint8_t*)c + ud);
    if (h == 0) {
        v->len = 0;
        v->not_found = 1;
        v->no_cacheable = 1;
        rc = 0;
        goto _end;
    };

    v->len = h->value.len;
    v->ptr = h->value.ptr;
    v->no_cacheable = 0;
    rc = 0;

_end:
    return rc;
};


static nai_sult_t nai_httpc_variable_headers_impl(
    void* c, uintptr_t ud, nai_script_value_t* v, char sep)
{
    nai_int_t n;
    size_t len;
    char* p;
    char* buf;
    nai_sult_t rc;
    nai_array_t* a;
    nai_httpc_header_t** h;
    nai_httpc_request_t* r;


    r = (nai_httpc_request_t*)c;
    a = (nai_array_t*)((uint8_t*)c + ud);

    if (a->count <= 0) {
        v->len = 0;
        v->not_found = 1;
        v->no_cacheable = 1;
        rc = 0;
        goto _end;
    };

    h = (nai_httpc_header_t**)a->elts;
    len = a->count * 2 - 2;
    for (n = 0; n < (intptr_t)a->count; n ++) {
        len += h[n]->value.len;
    };

    buf = (char*)nai_palloc(r->pool, len);
    if (buf == 0) {
        rc = nai_sult_from_errno();
        goto _end;
    };


    p = buf;
    for (n = 0; ; ) {
        nai_memcpy(p, h[n]->value.ptr, h[n]->value.len);
        p += h[n]->value.len;
        n ++;
        if (n >= (intptr_t)a->count) {
            break;
        };

        *p ++ = sep;
        *p ++ = ' ';
    };

    v->len = (uint32_t)len;
    v->ptr = buf;
    v->no_cacheable = 0;
    rc = 0;


_end:
    return rc;
};


static nai_sult_t nai_httpc_variable_headers(
    void* c, uintptr_t ud, nai_script_value_t* v)
{
    return nai_httpc_variable_headers_impl(c, ud, v, ',');
};


static nai_sult_t nai_httpc_variable_unknown_header(
    void* c, uintptr_t ud, nai_script_value_t* v, 
    nai_int_t from, nai_int_t prefix)
{
    nai_sult_t rc;
    nai_str_t k;
    nai_str_t* s;
    nai_httpc_header_t* h;
    nai_httpc_request_t* r;


    r = (nai_httpc_request_t*)c;
    s = (nai_str_t*)ud;
    nai_str_setm(&k, nai_str(s) + prefix, nai_str_len(s) - prefix);

    switch (from) {
    case 0:
        rc = nai_httpc_headers_in_find(r, &k, &h);
        break;
    case 1:
        rc = nai_httpc_headers_out_find(r, &k, &h);
        break;
    default:
        assert(0);
        rc = NAI_E_INTERNAL;
        goto _end;
    };

    if (rc != 0) {
        v->len = 0;
        v->not_found = 1;
        v->no_cacheable = 1;
        goto _end;
    };

    v->len = h->value.len;
    v->ptr = h->value.ptr;
    v->no_cacheable = 0;
    rc = 0;

_end:
    return rc;
};


static nai_sult_t nai_httpc_variable_unknown_header_in(
    void* c, uintptr_t ud, nai_script_value_t* v)
{
    return nai_httpc_variable_unknown_header(c, ud, v, 0, 5);
};


static nai_sult_t nai_httpc_variable_unknown_header_out(
    void* c, uintptr_t ud, nai_script_value_t* v)
{
    return nai_httpc_variable_unknown_header(c, ud, v, 1, 10);
};


static nai_sult_t nai_httpc_variable_protocol_uint64(
    void* c, uintptr_t ud, nai_script_value_t* v)
{
    uint64_t* n;
    nai_sult_t rc;
    nai_str_t s;
    nai_httpc_request_t* r;


    r = (nai_httpc_request_t*)c;
    if (r->proto == 0) {
        v->len = 0;
        v->not_found = 1;
        v->no_cacheable = 1;
        rc = 0;
        goto _end;
    };

    n = (uint64_t*)((uint8_t*)r->proto + ud);

    rc = nai_httpc_uint64_value(r->pool, &s, *n);
    if (rc < 0) {
        goto _end;
    };

    v->len = s.len;
    v->ptr = s.ptr;
    v->no_cacheable = 0;

_end:
    return rc;
};



static nai_sult_t nai_httpc_variable_scheme(
    void* c, uintptr_t ud, nai_script_value_t* v)
{
    nai_sult_t rc;
    nai_str_t s;
    nai_httpc_request_t* r;


    (void)ud;

    r = (nai_httpc_request_t*)c;
    if (r->ssl) {
        nai_str_setc(&s, "https");
    } else {
        nai_str_setc(&s, "http");
    };

    v->len = s.len;
    v->ptr = s.ptr;
    rc = 0;

    return rc;
};


static nai_sult_t nai_httpc_variable_https(
    void* c, uintptr_t ud, nai_script_value_t* v)
{
    nai_sult_t rc;
    nai_str_t s;
    nai_httpc_request_t* r;


    (void)ud;

    r = (nai_httpc_request_t*)c;
    if (r->ssl) {
        nai_str_setc(&s, "on");
    } else {
        nai_str_setc(&s, "");
    };

    v->len = s.len;
    v->ptr = s.ptr;
    rc = 0;

    return rc;
};


static nai_sult_t nai_httpc_variable_status(
    void* c, uintptr_t ud, nai_script_value_t* v)
{
    uint32_t status;
    nai_sult_t rc;
    nai_str_t s;
    nai_httpc_request_t* r;


    (void)ud;

    r = (nai_httpc_request_t*)c;
    if (r->responsed == 0) {
        v->len = 0;
        v->not_found = 1;
        v->no_cacheable = 1;
        rc = 0;
        goto _end;
    };

    status = r->status;
    if (status == 0 && r->version < NAI_HTTP_11) {
        status = 9;
    };

    if (status >= 100) {
        rc = nai_httpc_uint32_value(r->pool, &s, status);
    } else {
        rc = nai_httpc_print_value(r->pool, &s, 4, "%03u", status);
    };
    if (rc < 0) {
        goto _end;
    };

    v->len = s.len;
    v->ptr = s.ptr;
    v->no_cacheable = 0;
    rc = 0;

_end:
    return rc;
};


static nai_sult_t nai_httpc_variable_is_args(
    void* c, uintptr_t ud, nai_script_value_t* v)
{
    nai_sult_t rc;
    nai_str_t s;
    nai_httpc_request_t* r;


    (void)ud;

    r = (nai_httpc_request_t*)c;
    if (nai_str_len(&r->args) > 0) {
        nai_str_setc(&s, "?");
    } else {
        nai_str_setc(&s, "");
    };

    v->len = s.len;
    v->ptr = s.ptr;
    rc = 0;

    return rc;
};


static nai_sult_t nai_httpc_variable_arg(
    void* c, uintptr_t ud, nai_script_value_t* v)
{
    nai_int_t prefix;
    nai_sult_t rc;
    nai_mem_t k, d;
    nai_str_t* s;
    nai_httpc_request_t* r;


    prefix = sizeof("arg_") - 1;
    r = (nai_httpc_request_t*)c;
    s = (nai_str_t*)ud;
    nai_str_setm(&k, nai_str(s) + prefix, nai_str_len(s) - prefix);


    rc = nai_uri_parse_value(
        &k, &d, nai_str(&r->args), nai_str_len(&r->args));
    if (rc < 0) {
        v->len = 0;
        v->not_found = 1;
        rc = 0;
        goto _end;
    };

    v->len = nai_str_len(&d);
    v->ptr = nai_str(&d);
    rc = 0;

_end:
    return rc;
};


static nai_sult_t nai_httpc_variable_content_length(
    void* c, uintptr_t ud, nai_script_value_t* v)
{
    nai_sult_t rc;
    nai_str_t s;
    nai_httpc_header_t* h;
    nai_httpc_request_t* r;


    (void)ud;

    r = (nai_httpc_request_t*)c;
    if (r->headers_in.content_length) {
        h = r->headers_in.content_length;
        v->len = h->value.len;
        v->ptr = h->value.ptr;
        v->no_cacheable = 0;
    } else if (r->headers_in.content_length_n == (uint64_t)-1) {
        v->len = 0;
        v->not_found = 1;
        v->no_cacheable = 1;
    } else {
        rc = nai_httpc_uint64_value(
            r->pool, &s, r->headers_in.content_length_n);
        if (rc < 0) {
            goto _end;
        };

        v->len = s.len;
        v->ptr = s.ptr;
        v->no_cacheable = 0;
    };

    rc = 0;

_end:
    return rc;
};


static nai_sult_t nai_httpc_variable_sent_content_length(
    void* c, uintptr_t ud, nai_script_value_t* v)
{
    nai_sult_t rc;
    nai_str_t s;
    nai_httpc_header_t* h;
    nai_httpc_request_t* r;


    (void)ud;

    r = (nai_httpc_request_t*)c;
    if (r->headers_out.content_length) {
        h = r->headers_in.content_length;
        v->len = h->value.len;
        v->ptr = h->value.ptr;
        v->no_cacheable = 0;
    } else if (r->headers_out.content_length_n == (uint64_t)-1) {
        v->len = 0;
        v->not_found = 1;
        v->no_cacheable = 1;
    } else {
        rc = nai_httpc_uint64_value(
            r->pool, &s, r->headers_out.content_length_n);
        if (rc < 0) {
            goto _end;
        };

        v->len = s.len;
        v->ptr = s.ptr;
        v->no_cacheable = 0;
    };

    rc = 0;

_end:
    return rc;
};


static nai_sult_t nai_httpc_variable_request_time(
    void* c, uintptr_t ud, nai_script_value_t* v)
{
    int64_t msec;
    nai_sult_t rc;
    nai_str_t s;
    nai_httpc_request_t* r;


    (void)ud;

    r = (nai_httpc_request_t*)c;
    if (r->start_msec == 0) {
        v->len = 0;
        v->not_found = 1;
        v->no_cacheable = 1;
        rc = 0;
        goto _end;
    };

    msec = nai_tickcache_to_msec() - r->start_msec;
    if (msec < 0) {
        msec = 0;
    };

    rc = nai_httpc_print_value(r->pool, &s, 
        NAI_INT64_T_LEN + 5, "%" NAI_INT64_FMT "u.%03u", 
        msec / 1000, (nai_int_t)(msec % 1000));
    if (rc < 0) {
        goto _end;
    };

    v->len = s.len;
    v->ptr = s.ptr;
    v->no_cacheable = 0;

_end:
    return rc;
};


static nai_sult_t nai_httpc_variable_connection_time(
    void* c, uintptr_t ud, nai_script_value_t* v)
{
    int64_t msec;
    nai_sult_t rc;
    nai_str_t s;
    nai_httpc_request_t* r;


    (void)ud;

    r = (nai_httpc_request_t*)c;
    if (r->connection == 0) {
        v->len = 0;
        v->not_found = 1;
        v->no_cacheable = 1;
        rc = 0;
        goto _end;
    };

    msec = nai_tickcache_to_msec() - r->connection->start_msec;
    if (msec < 0) {
        msec = 0;
    };

    rc = nai_httpc_print_value(r->pool, &s, 
        NAI_INT64_T_LEN + 5, "%" NAI_INT64_FMT "u.%03u", 
        msec / 1000, (nai_int_t)(msec % 1000));
    if (rc < 0) {
        goto _end;
    };

    v->len = s.len;
    v->ptr = s.ptr;
    v->no_cacheable = 0;

_end:
    return rc;
};


static nai_sult_t nai_httpc_variable_connection_addr(
    void* p, uintptr_t ud, nai_script_value_t* v)
{
    nai_sult_t rc;
    nai_str_t s;
    nai_httpc_request_t* r;
    nai_httpc_connection_t* c;


    (void)ud;

    r = (nai_httpc_request_t*)p;
    c = r->connection;
    if (c == 0) {
        v->len = 0;
        v->not_found = 1;
        v->no_cacheable = 1;
        rc = 0;
        goto _end;
    };

    rc = nai_httpc_connection_get_name(c, (nai_int_t)ud, &s, 0);
    if (rc < 0) {
        goto _end;
    };

    v->len = s.len;
    v->ptr = s.ptr;
    v->no_cacheable = 0;

_end:
    return rc;
};


static nai_sult_t nai_httpc_variable_connection_port(
    void* p, uintptr_t ud, nai_script_value_t* v)
{
    nai_int_t port;
    nai_sult_t rc;
    nai_str_t s;
    nai_httpc_request_t* r;
    nai_httpc_connection_t* c;


    (void)ud;

    r = (nai_httpc_request_t*)p;
    c = r->connection;
    if (c == 0) {
        v->len = 0;
        v->not_found = 1;
        v->no_cacheable = 1;
        rc = 0;
        goto _end;
    };

    rc = nai_httpc_connection_get_name(c, (nai_int_t)ud, 0, &port);
    if (rc < 0) {
        goto _end;
    };

    rc = nai_httpc_uint32_value(r->pool, &s, port);
    if (rc < 0) {
        goto _end;
    };

    v->len = s.len;
    v->ptr = s.ptr;
    v->no_cacheable = 0;

_end:
    return rc;
};


static nai_sult_t nai_httpc_variable_msec(
    void* c, uintptr_t ud, nai_script_value_t* v)
{
    int64_t msec;
    nai_sult_t rc;
    nai_str_t s;
    nai_httpc_request_t* r;


    (void)ud;

    r = (nai_httpc_request_t*)c;

    msec = nai_tickcache_to_msec();
    rc = nai_httpc_print_value(r->pool, &s, 
        NAI_INT64_T_LEN + 5, "%" NAI_INT64_FMT "u.%03u", 
        msec / 1000, (nai_int_t)(msec % 1000));
    if (rc < 0) {
        goto _end;
    };

    v->len = s.len;
    v->ptr = s.ptr;
    v->no_cacheable = 0;

_end:
    return rc;
};


static nai_sult_t nai_httpc_variable_time_local(
    void* c, uintptr_t ud, nai_script_value_t* v)
{
    nai_int_t len;
    char* buf;
    nai_sult_t rc;
    nai_timefmts_t* fmts;
    nai_httpc_request_t* r;


    (void)ud;

    r = (nai_httpc_request_t*)c;
    len = sizeof(fmts->http_log_time);
    buf = (char*)nai_palloc(r->pool, len);
    if (buf == 0) {
        rc = nai_sult_from_errno();
        goto _end;
    };

    fmts = nai_timecache_fmts();
    nai_memcpy(buf, fmts->iso8601_time, len);

    v->len = len-1;
    v->ptr = buf;
    v->no_cacheable = 0;
    rc = 0;

_end:
    return rc;
};


static nai_sult_t nai_httpc_variable_time_iso8601(
    void* c, uintptr_t ud, nai_script_value_t* v)
{
    nai_int_t len;
    char* buf;
    nai_sult_t rc;
    nai_timefmts_t* fmts;
    nai_httpc_request_t* r;


    (void)ud;

    r = (nai_httpc_request_t*)c;
    len = sizeof(fmts->iso8601_time);
    buf = (char*)nai_palloc(r->pool, len);
    if (buf == 0) {
        rc = nai_sult_from_errno();
        goto _end;
    };

    fmts = nai_timecache_fmts();
    nai_memcpy(buf, fmts->iso8601_time, len);

    v->len = len-1;
    v->ptr = buf;
    v->no_cacheable = 0;
    rc = 0;

_end:
    return rc;
};


