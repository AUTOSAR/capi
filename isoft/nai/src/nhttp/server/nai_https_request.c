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
/// @file       nai_https_request.c
/// @brief      
/// @details
/// @date       2021-01-25
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_log.h"
#include "nai/service/nai_main_event.h"
#include "nai_https_core.h"



//////////////////////////////////////////////////////////////////////////////
// https request create


#define nai_https_headers_count(t, first, last) ((      \
    nai_offsetof(t, last) - nai_offsetof(t, first)) /   \
    sizeof(void*) + 1)                                  \


#define nai_https_headers_in_count()                    \
    nai_https_headers_count(nai_https_headers_in_t,     \
    content_length, keep_alive)                         \


#define nai_https_headers_out_count()                   \
    nai_https_headers_count(nai_https_headers_out_t,    \
    content_length, etag)                               \



static nai_sult_t nai_https_request_alloc(
    nai_https_connection_t* c, nai_https_request_t** pv)
{
    nai_sult_t rc;
    nai_pool_t pool;
    nai_https_request_t* r;


    /* pool from connection's pool */
    nai_pool_from(&pool, c->c.pool);

    /* alloc request from pool */
    r = (nai_https_request_t*)nai_palloc(&pool, sizeof(*r));
    if (r == 0) {
        rc = nai_sult_from_errno();
        nai_log_alert(NAI_LOG_HTTPS, 
            nai_sult_to_errno(rc), "alloc request failed");
        goto _end;
    };

    /* alloc space to inplace pool */
    r->pool = (nai_pool_t*)nai_palloc(&pool, sizeof(*r->pool));
    if (r->pool == 0) {
        rc = nai_sult_from_errno();
        nai_log_alert(NAI_LOG_HTTPS, 
            nai_sult_to_errno(rc), "alloc request pool failed");
        goto _end;
    };

    /* alloc space to inplace buffer pool */
    r->bufpool = (nai_bufpool_t*)nai_palloc(&pool, sizeof(*r->bufpool));
    if (r->bufpool == 0) {
        rc = nai_sult_from_errno();
        nai_log_alert(NAI_LOG_HTTPS, 
            nai_sult_to_errno(rc), "alloc request buffer pool failed");
        goto _end;
    };

    /* alloc local data of main request */
    rc = nai_https_local_alloc(nai_https_get_service(c), &pool, &r->local);
    if (rc < 0) {
        nai_log_alert(NAI_LOG_HTTPS, 
            nai_sult_to_errno(rc), "alloc local of main request failed");
        goto _end;
    };

    /* alloc headers_in's list for main request only */
    r->headers_in.list = (nai_list_entry_t*)
        nai_palloc(&pool, sizeof(nai_list_entry_t));
    if (r->headers_in.list == 0) {
        rc = nai_sult_from_errno();
        nai_log_alert(NAI_LOG_HTTPS, 
            nai_sult_to_errno(rc), 
            "alloc input headers of main request failed");
        goto _end;
    };
    r->headers_in.flags = 0;
    r->headers_in.keep_alive_n = -1;
    r->headers_in.content_length_n = -1;
    nai_pzero((void**)&r->headers_in.ent, nai_https_headers_in_count());
    nai_list_init(r->headers_in.list);
    nai_array_init(&r->headers_in.cookies, 
        sizeof(nai_https_header_t*), r->pool);


    /* init output headers */
    r->headers_out.flags = 0;
    r->headers_out.last_modified_time = -1;
    r->headers_out.content_length_n = -1;
    nai_pzero((void**)&r->headers_out.ent, nai_https_headers_out_count());
    nai_list_init(&r->headers_out.list);
    nai_array_init(&r->headers_out.link, 
        sizeof(nai_https_header_t*), r->pool);
    nai_array_init(&r->headers_out.cache_control, 
        sizeof(nai_https_header_t*), r->pool);


    /* exchange */
    nai_pool_init(r->pool, 0);
    nai_pool_exchange(r->pool, &pool);

    /* create buffer pool */
    rc = nai_bufpool_from(r->bufpool, r->pool, 0);
    if (rc < 0) {
        rc = nai_sult_from_errno();
        nai_log_alert(NAI_LOG_HTTPS, 
            nai_sult_to_errno(rc), "create request buffer pool failed");
        nai_pool_exchange(&pool, r->pool);
        goto _end;
    };

    /* init regex and values */
    r->values = 0;
#if (NAI_HAVE_REGEX)
    r->ncapture = 0;
    r->captures = 0;
    r->match_data = 0;
#endif

    /* init part members */
    r->connection = c;


    /* return */
    pv[0] = r;
    rc = 0;

_end:
    nai_pool_close(&pool);
    return rc;
};


nai_sult_t nai_https_request_create(
    nai_https_connection_t* c, nai_https_protocol_t* p)
{
    nai_sult_t rc;
    nai_https_request_t* r = 0;


    rc = nai_https_request_alloc(c, &r);
    if (rc < 0) {
        nai_log_alert(NAI_LOG_HTTPS, 
            nai_sult_to_errno(rc), "alloc main request failed");
        goto _end;
    };

    /* inhert connection's conf */
    r->conf = c->conf;

    /* no parent */
    r->parent = 0;
    r->next = 0;
    r->postpone = 0;

    /* init request */
    r->method = NAI_HTTP_UNKNOWN;
    r->version = 0;
    nai_str_setn(&r->request_line);
    nai_str_setn(&r->method_name);
    nai_str_setn(&r->protocol);
    nai_str_setn(&r->unparsed_uri);
    nai_str_setn(&r->uri);
    nai_str_setn(&r->args);
    nai_str_setn(&r->exten);
    nai_uri_init(&r->parsed_uri);

    /* init response */
    r->status = 0;
    r->err_status = 0;
    nai_str_setn(&r->status_line);
    nai_buflist_init(&r->content, r->bufpool);

    /* read and send handle */
    r->read = 0;
    r->send = 0;

    /* init status */
    r->stage = 0;
    r->access = 0;
    r->flags = 0;
    r->test_expect = 1;
    r->keepalive = 1;
    r->lingering_close = 1;
    r->redirects = 15;
    r->subrequests = 15;

    /* time */
    r->start_msec = nai_tickcache_to_msec();

    /* link to connection */
    r->connection = c;
    r->proto = p;
    p->main = r;
    p->request = r;
    c->requests ++;


    /* success */
    rc = 0;

_end:
    return rc;
};


nai_sult_t nai_https_request_close(nai_https_request_t* r)
{
    intptr_t n;
    nai_sult_t rc;
    nai_https_t* h;
    nai_https_stage_group_t* g;
    nai_https_protocol_t* p;
    nai_https_postpone_t* post;
    nai_https_handle_f* elts;
    nai_https_handle_f handle;


    if (r->logged) {
        goto _end;
    };


    /* run log filters */
    h = nai_https_get_service(r);
    g = &h->groups[NAI_HTTPS_STAGE_LOG];
    elts = (nai_https_handle_f*)g->handles.elts;
    for (n = 0; n < (intptr_t)g->handles.count; n ++) {
        handle = elts[n];

        rc = handle(r);
        switch (rc) {
        case NAI_OK:
            continue;

        case NAI_DECLINED:
            continue;

        case NAI_DONE:
            rc = 0;
            goto _end;

        default:
            break;
        };

        if (rc < 0) {
            if (rc == NAI_EAGAIN || rc == NAI_EINPROGRESS) {
                nai_log_alert(NAI_LOG_HTTPS, 0, 
                    "log filter should not return EAGAIN");

                rc = NAI_HTTP_INTERNAL_SERVER_ERROR;
            };
        } else {
            nai_log_alert(NAI_LOG_HTTPS, 0, 
                "log filter return a unknown code: %d", rc);
        };

        break;
    };


_end:
    (void)rc;

    /* free content of postoned */
    post = r->postpone;
    for ( ; post; post = post->next) {
        if (post->request) {
            continue;
        };

        nai_buflist_close(&post->content);
    };

    /* free content */
    nai_buflist_close(&r->content);


    /* free request */
    p = r->proto;
    p->main = 0;
    p->request = 0;
    nai_pool_close(r->pool);
    rc = 0;

    return rc;
};



//////////////////////////////////////////////////////////////////////////////
// https request response


nai_sult_t nai_https_set_exten(nai_https_request_t* r)
{
    nai_sult_t rc;
    nai_int_t n;
    nai_int_t len;
    const char* uri;

    nai_str_setn(&r->exten);

    uri = nai_str(&r->uri);
    len = nai_str_len(&r->uri);
    for (n = len-1; n >= 0; n --) {
        if (uri[n] == '.') {
            nai_str_setm(&r->exten, uri+n+1, len-n-1);
            break;
        };
        if (uri[n] == '/') {
            break;
        };
    };

    rc = 0;
    return rc;
};


nai_sult_t nai_https_set_content_type(nai_https_request_t* r)
{
    nai_sult_t rc;
    nai_str_t* ptype;
    nai_https_location_t* l;


    if (r->headers_out.content_type) {
        rc = 0;
        goto _end;
    };

    ptype = 0;
    l = nai_https_get_location(r);
    if (nai_str_len(&r->exten) > 0) {
        nai_str_hash(&r->exten, 1);
        ptype = nai_hashsnap_find(&l->types_hash, (nai_str_t*)&r->exten, 1);
    };
    if (ptype) {
        rc = nai_https_headers_out_set(r, content_type, ptype, 0);
    } else {
        rc = nai_https_headers_out_set(r, content_type, &l->default_type, 0);
    };
    if (rc < 0) {
        rc = nai_sult_from_errno();
        nai_log_alert(NAI_LOG_HTTPS, 
            nai_sult_to_errno(rc), "set content type failed");
        goto _end;
    };

_end:
    return rc;
};


nai_sult_t nai_https_map_to_path(
    nai_https_request_t* r, 
    nai_str_t* path, size_t* root_length, size_t reserved)
{
    char* uri;
    char* out;
    size_t uri_len;
    size_t out_len;
    size_t alias;
    size_t space;
    nai_sult_t rc;
    nai_reserved_t rev;
    nai_https_location_t* l;


    l = nai_https_get_location(r);

    uri = nai_str(&r->uri);
    uri_len = nai_str_len(&r->uri);
    alias = l->alias;
    space = reserved;
    if (alias == NAI_INT_T_MAX) {
        alias = 0;
        space += uri_len;
    } else {
        space += uri_len - alias;
    };

    if (space == 0 && 
        !nai_script_expn_is_variable(&l->root) == 0) {
        path[0] = l->root.value;
        out = nai_str(path);
        out_len = nai_str_len(path);
        if (nai_path_is_absolute(out, out_len)) {
            rc = 0;
            goto _end;
        };
    };

    space ++; /* force dup stirng */
    rev.pre = 0;
    rev.post = space;
    rc = nai_https_expn_value(r, &l->root, path, &rev);
    if (rc < 0) {
        goto _end;
    };

    out = nai_str(path);
    out_len = nai_str_len(path);
    if (nai_path_is_absolute(out, out_len) == 0) {
        nai_memmove(out+1, out, out_len + 1);
        out[0] = nai_path_sep;
        out_len ++;
    };
    if (root_length) {
        root_length[0] = out_len;
    };

    if (uri_len - alias > 0) {
        nai_memcpy(out + out_len, uri + alias, uri_len - alias);
        out_len += uri_len - alias;
        out[out_len] = 0;
        nai_str_setl(path, out_len);
    };
    rc = 0;

_end:
    return rc;
};


nai_sult_t nai_https_redirect_symbol(
    nai_https_request_t* r, const nai_str_t* name)
{
    nai_sult_t rc;
    nai_https_t* h;
    nai_https_server_t* s;
    nai_https_location_entry_t* ent;


    if (r->redirects <= 0) {
        nai_log_error(NAI_LOG_HTTPS, 0, 
            "too many redirects when redirect to symbol %.*s", 
            nai_str_len(name), nai_str(name));

        rc = NAI_HTTP_INTERNAL_SERVER_ERROR;
        goto _end;
    };

    if (nai_str_len(&r->uri) <= 0) {
        nai_log_error(NAI_LOG_HTTPS, 0, 
            "empty uri in redirect to symbol %.*s", 
            nai_str_len(name), nai_str(name));

        rc = NAI_HTTP_INTERNAL_SERVER_ERROR;
        goto _end;
    };


    h = nai_https_get_service(r);
    s = nai_https_get_server(r);

    ent = (nai_https_location_entry_t*)
        nai_rbtree_str_find(&s->table.symbols, name, 0);
    if (ent == 0) {
        nai_log_error(NAI_LOG_HTTPS, ENOENT, 
            "could not find symbol %.*s", 
            nai_str_len(name), nai_str(name));

        rc = NAI_HTTP_INTERNAL_SERVER_ERROR;
        goto _end;
    };

    r->redirects --;
    r->internal = 1;
    r->redirected = 0;
    r->conf = ent->loc->conf;

    nai_pzero(r->local->data + 1, h->max_modules);

    rc = nai_https_core_update(r);
    if (rc < 0) {
        goto _end;
    };

    r->stage = h->groups[NAI_HTTPS_STAGE_REWRITE].start_index;
    r->send = nai_https_core_run_stage;
    rc = r->send(r);

_end:
    return rc;
};


nai_sult_t nai_https_redirect_internal(
    nai_https_request_t* r, const nai_str_t* uri, const nai_str_t* args)
{
    nai_sult_t rc;
    nai_https_t* h;
    nai_https_server_t* s;


    if (r->redirects <= 0) {
        nai_log_error(NAI_LOG_HTTPS, 0, 
            "too many redirects when internally redirecting to %.*s", 
            nai_str_len(uri), nai_str(uri));

        rc = NAI_HTTP_INTERNAL_SERVER_ERROR;
        goto _end;
    };

    h = nai_https_get_service(r);
    s = nai_https_get_server(r);

    r->redirects --;
    r->internal = 1;
    r->conf = s->conf;

    nai_pzero(r->local->data + 1, h->max_modules);

    /* set uri */
    r->uri = *uri;
    r->valid_unparsed_uri = 0;
    if (args) {
        r->args = *(nai_mem_t*)args;
    } else {
        nai_str_setn(&r->args);
    };

    /* update exten */
    nai_https_set_exten(r);

    /* update conf */
    rc = nai_https_core_update(r);
    if (rc < 0) {
        goto _end;
    };

    r->send = nai_https_core_handle;
    rc = r->send(r);

_end:
    return rc;
};


nai_sult_t nai_https_subrequest(
    nai_https_request_t* r, const nai_str_t* uri, const nai_str_t* args, 
    nai_https_request_t** pv)
{
    nai_sult_t rc;
    nai_https_request_t* s;
    nai_https_connection_t* c;
    nai_https_postpone_t* p;
    nai_https_postpone_t** prev;


    if (r->subrequests == 0) {
        nai_log_error(NAI_LOG_HTTPS, 0, 
            "reached the maximum depth of the subrequest "
            "when internally redirecting to %.*s", 
            nai_str_len(uri), nai_str(uri));

        rc = NAI_HTTP_INTERNAL_SERVER_ERROR;
        goto _end;
    };

    /* alloc sub request */
    s = (nai_https_request_t*)nai_palloc(r->pool, sizeof(*s));
    if (s == 0) {
        rc = nai_sult_from_errno();
        nai_log_error(NAI_LOG_HTTPS, 
            nai_sult_to_errno(rc), 
            "alloc subrequest failed, depth %d, uri %.*s", 
            r->subrequests, nai_str_len(uri), nai_str(uri));
        goto _end;
    };

    c = r->connection;
    s->pool = r->pool;
    s->bufpool = r->bufpool;
    s->proto = r->proto;
    s->connection = r->connection;


    /* alloc local data of main request */
    rc = nai_https_local_alloc(nai_https_get_service(c), s->pool, &s->local);
    if (rc < 0) {
        nai_log_alert(NAI_LOG_HTTPS, 
            nai_sult_to_errno(rc), "alloc local of subrequest failed");
        goto _end;
    };

    /* inherit conf from parent */
    s->conf = nai_https_get_server(r)->conf;

    /* inherit input headers from parent */
    s->headers_in = r->headers_in;

    /* init output headers */
    nai_pzero((void**)r->headers_out.ent, nai_https_headers_out_count());
    nai_list_init(&s->headers_out.list);
    s->headers_out.content_length_n = -1;


    /* alloc postoned for subrequest */
    p = (nai_https_postpone_t*)nai_palloc(s->pool, sizeof(*p));
    if (p == 0) {
        rc = nai_sult_from_errno();
        nai_log_alert(NAI_LOG_HTTPS, 
            nai_sult_to_errno(rc), "alloc postoned for subrequest failed");
        goto _end;
    };

    /* put into post handle list */
    rc = nai_https_post(s);
    if (rc < 0) {
        goto _end;
    };

    /* put into parent's postoned list */
    p->request = s;
    p->next =  0;
    prev = &r->postpone;
    for ( ; ; prev = &prev[0]->next) {
        if (prev[0] == 0) {
            prev[0] = p;
            break;
        };
    };
    if (r->proto->request == r) {
        r->proto->request = s;
        r->proto->timeoset &= NAI_EV_READ; /* clear send timeout seted flags */
    };

    /* set uri */
    s->uri = *uri;
    s->valid_unparsed_uri = 0;
    if (args) {
        r->args = *(nai_mem_t*)args;
    } else {
        nai_str_setn(&r->args);
    };

    /* set exten */
    nai_https_set_exten(r);

    /* inherit request line from parent */
    s->request_line = r->request_line;
    s->unparsed_uri = r->unparsed_uri;
    s->version = r->version;
    s->protocol = r->protocol;

    /* use GET method */
    s->method = NAI_HTTP_GET;
    nai_str_setc(&s->method_name, "GET");

    /* init status line */
    s->status = 0;
    nai_str_setn(&s->status_line);
    nai_buflist_init(&s->content, r->bufpool);

    /* init handle */
    s->read = 0;
    s->send = nai_https_core_handle;

    /* init stats */
    s->access = 0;
    s->flags = 0;
    s->host_complex = r->host_complex;
    s->test_expect = 1;
    s->internal = 1;
    s->redirects = 15;
    s->subrequests = r->subrequests - 1;

    /* time */
    s->start_msec = nai_tickcache_to_msec();

    /* link to parent */
    s->parent = r;
    s->next = 0;
    s->postpone = 0;


    if (pv) {
        pv[0] = s;
    };
    rc = 0;

_end:
    return rc;
};


nai_sult_t nai_https_post(nai_https_request_t* r)
{
    nai_sult_t rc;
    nai_https_protocol_t* p;
    nai_https_request_t** prev;


    if (r->posted) {
        rc = 0;
        goto _end;
    };

    p = r->proto;
    prev = &p->post;
    for ( ; ; prev = &prev[0]->next) {
        if (prev[0] == 0) {
            prev[0] = r;
            break;
        };
    };

    r->next = 0;
    r->posted = 1;
    rc = 0;

_end:
    return rc;
};


nai_sult_t nai_https_response(nai_https_request_t* r)
{
    intptr_t n;
    nai_sult_t rc;
    nai_https_t* h;
    nai_https_stage_group_t* g;
    nai_https_handle_f* elts;
    nai_https_handle_f handle;


    if (r->responsed) {
        rc = NAI_E_OPERATION;
        nai_log_alert(NAI_LOG_HTTPS, 
            nai_sult_to_errno(rc), "request is already responsed");
        goto _end;
    };

    if (r->err_status) {
        if (r->status != r->err_status) {
            r->status = r->err_status;
            nai_str_setn(&r->status_line);
        };
    } else if (r->status == 0) {
        r->status = NAI_HTTP_OK;
    };

    h = nai_https_get_service(r);
    g = &h->groups[NAI_HTTPS_STAGE_RESPONSE];
    elts = (nai_https_handle_f*)g->handles.elts;
    for (n = 0; n < (intptr_t)g->handles.count; n ++) {
        handle = elts[n];

        rc = handle(r);
        switch (rc) {
        case NAI_OK:
            continue;

        case NAI_DECLINED:
            continue;

        case NAI_DONE:
            rc = 0;
            goto _end;

        default:
            break;
        };

        if (rc < 0) {
            if (rc == NAI_EAGAIN || rc == NAI_EINPROGRESS) {
                nai_log_alert(NAI_LOG_HTTPS, 0, 
                    "response filter should not return EAGAIN");

                rc = NAI_HTTP_INTERNAL_SERVER_ERROR;
            };
        } else {
            nai_log_alert(NAI_LOG_HTTPS, 0, 
                "response filter return a unknown code: %d", rc);
        };

        goto _end;
    };


    if (r->version < NAI_HTTP_10) {
        rc = NAI_OK;
        goto _end;
    };

    if (r->method == NAI_HTTP_HEAD) {
        r->header_only = 1;
    };

    switch (r->status) {
    case NAI_HTTP_OK:
    case NAI_HTTP_PARTIAL_CONTENT:
    case NAI_HTTP_NOT_MODIFIED:
        break;

    case NAI_HTTP_NO_CONTENT:
        r->header_only = 1;
        r->headers_out.content_length_n = -1;
        if (r->headers_out.content_length) {
            nai_list_entry_remove(&r->headers_out.content_length->ent);
        };
        if (r->headers_out.content_type) {
            nai_list_entry_remove(&r->headers_out.content_type->ent);
        };

        /* fallthrough */

    default:
        r->headers_out.last_modified_time = -1;
        if (r->headers_out.last_modified) {
            nai_list_entry_remove(&r->headers_out.last_modified->ent);
        };
        break;
    };


    rc = 0;

_end:
    r->responsed = 1;
    return rc;
};


//////////////////////////////////////////////////////////////////////////////
// https request finalize



static nai_sult_t nai_https_request_flush(nai_https_request_t* r)
{
    nai_sultp_t rc;
    nai_https_protocol_t* p;


    p = r->proto;
    if (p->sendtimeo) {
        rc = nai_errno_to_sult(NAI_ETIMEDOUT);
        goto _end;
    };

    /* write all data */
    while (1) {
        rc = nai_https_write(r, 0, -1);
        if (rc < 0) {
            break;
        };
        if (rc == 0) {
            r->send = 0;
            break;
        };
    };

_end:
    return (nai_sult_t)rc;
};


static nai_sult_t nai_https_request_discard(nai_https_request_t* r)
{
    nai_sultp_t rc;
    nai_buflist_t tmp;
    nai_https_protocol_t* p;


    p = r->proto;
    if (p->readtimeo) {
        rc = nai_errno_to_sult(NAI_ETIMEDOUT);
        goto _end;
    };

    nai_buflist_init(&tmp, r->bufpool);

    /* write all data */
    while (1) {
        rc = nai_https_read(r, &tmp, 8*1024);
        if (rc < 0) {
            break;
        };

        nai_buflist_close(&tmp);

        if (rc == 0) {
            rc = NAI_DECLINED;
            r->read = 0;
            break;
        };
    };

_end:
    return (nai_sult_t)rc;
};


static nai_sult_t nai_https_request_finished(nai_https_request_t* r)
{
    (void)r;

    /* do nothing */
    return 0;
};



static nai_sult_t nai_https_request_finish(nai_https_request_t* r)
{
    nai_sult_t rc;
    nai_https_connection_t* c;


    r->read = nai_https_request_discard;
    r->send = nai_https_request_flush;


    c = r->connection;
    if (nai_stream_get_event(&c->c.str) & NAI_EV_READ) {
        rc = r->read(r);
        if (rc < 0 && rc != NAI_E_AGAIN && rc != NAI_E_INPROGRESS) {
            goto _done;
        };
    };
    if (nai_stream_get_event(&c->c.str) & NAI_EV_WRITE) {
        rc = r->send(r);
        if (rc == 0) {
            goto _done;
        };
        if (rc <= 0 && rc != NAI_E_AGAIN && rc != NAI_E_INPROGRESS) {
            goto _done;
        };
    };

    rc = NAI_DECLINED;
    return rc;

_done:
    return nai_https_request_finalize(r, rc);
};


static nai_sult_t nai_https_request_next(nai_https_request_t* r)
{
    nai_sult_t rc;
    nai_https_location_t* l;
    nai_https_protocol_t* p;
    nai_https_connection_t* c;


    c = r->connection;
    p = r->proto;
    l = nai_https_get_location(r);


    if (p->read_left != 0) {
        r->keepalive = 0;
        r->lingering_close = 1;
    };


    if (l->keepalive_timeo > 0 && 
        r->keepalive) {
        rc = nai_https_handle_keepalive(r);
        goto _end;
    };


    switch (l->lingering_close) {
    case NAI_HTTPS_ON:
        if (!r->lingering_close && 
            !(nai_stream_get_event(&c->c.str) & NAI_EV_READ)) {
            break;
        };

        /* fallthrough */

    case NAI_HTTPS_ALWAYS:
        rc = nai_https_handle_close(r);
        goto _end;

    default:
        break;
    };


    rc = nai_https_request_terminate(r, 0);

_end:
    return rc;
};


nai_sult_t nai_https_request_finalize(
    nai_https_request_t* r, nai_sult_t sult)
{
    nai_sult_t rc;
    nai_https_event_t e;
    nai_https_request_t* rp;
    nai_https_protocol_t* p;
    nai_https_connection_t* c;


    rc = sult;
    if (rc == NAI_DECLINED) {
        goto _end;
    };

    c = r->connection;
    p = r->proto;


    /* check is http status */
    if (nai_sult_is_http_status(rc)) {
        rc = nai_sult_to_http_status(rc);
    };


    /* the request close with an error */
    if (rc < 0 || c->c.error) {
        rc = nai_https_request_terminate(r, rc);
        goto _end;
    };


    /* special response code */
    if (rc >= NAI_HTTP_MOVED_PERMANENTLY || 
        rc == NAI_HTTP_CREATED || 
        rc == NAI_HTTP_NO_CONTENT) {

        rc = nai_https_request_special_response(r, rc);
        if (rc < 0) {
            if (rc == NAI_EAGAIN || rc == NAI_EINPROGRESS) {
                rc = NAI_DECLINED;
            } else {
                rc = nai_https_request_terminate(r, rc);
            };
            goto _end;
        };
    };

    if (r->responsed == 0) {
        rc = nai_https_response(r);
        if (rc < 0 || rc > NAI_DECLINED) {
            rc = nai_https_request_finalize(r, rc);
            goto _end;
        };
    };

    /* subrequest case */
    if (r != p->main) {

        /* the request has content need to send */
        if (r->postpone || !nai_buflist_is_empty(&r->content)) {
            rc = nai_https_request_finish(r);
            goto _end;
        };


        if (r != p->request) {
            r->send = nai_https_request_finished;
            rc = NAI_DECLINED;
        } else {

            rp = r->parent;
            if (rp->postpone && rp->postpone->request == r) {
                rp->postpone = rp->postpone->next;
            };

            r->done = 1;
            p->request = rp;
            p->timeoset &= NAI_EV_READ; /* clear send timeout seted flags */

            if (nai_stream_get_event(&c->c.str) & NAI_EV_WRITE) {
                /* simulate a write event */
                e.c = &c->c;
                e.events = NAI_EV_WRITE;
                rc = c->c.send(&e);
            } else {
                rc = NAI_DECLINED;
            };
        };

        goto _end;
    };

    /* main requeset case */
    /* there is still data that has not been sent */
    if (r->postpone || 
        p->header_sent == 0 || p->send_left > 0 || 
        !nai_buflist_is_empty(&r->content)) {
        rc = nai_https_request_finish(r);
        goto _end;
    };

    r->read = 0;
    r->send = 0;
    r->done = 1;

    if (!c->c.closed) {
        rc = nai_https_request_next(r);
    } else {
        rc = nai_https_request_terminate(r, 0);
    };

_end:
    return rc;
};


nai_sult_t nai_https_request_terminate(
    nai_https_request_t* r, nai_sult_t sult)
{
    nai_sult_t rc;
    nai_https_connection_t* c;
    nai_https_protocol_t* p;
    nai_https_request_t* m;


    c = r->connection;
    p = r->proto;
    m = p->main;
    if (sult > 0 && (m->status == 0 || !p->header_sent)) {
        m->status = sult;
    };

    if (c->locked > 0) {
        c->c.error = 1;
        r->send = nai_https_request_finished;
        rc = NAI_DECLINED;
        goto _end;
    };

    /* close send list first, 
     * because the buffer in list well be invalid after close request */
    nai_buflist_close(&p->send_list);


    /* close request */
    rc = nai_https_request_close(m);
    if (rc < 0) {
        nai_log_error(NAI_LOG_HTTPS, 
            nai_sult_to_errno(rc), 
            "close request failed on terminate");
    };

    /* close connection */
    rc = nai_https_connection_close(c);
    if (rc < 0) {
        nai_log_error(NAI_LOG_HTTPS, 
            nai_sult_to_errno(rc), 
            "close connection failed on terminate");
    };

    /* tell caller the request had been closed */
    rc = nai_errno_to_sult(ECONNABORTED);

_end:
    return rc;
};


