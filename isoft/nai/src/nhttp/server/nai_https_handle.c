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
/// @file       nai_https_handle.c
/// @brief      
/// @details
/// @date       2021-02-19
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_log.h"
#include "nai_https_core.h"
#include <stdlib.h>



//////////////////////////////////////////////////////////////////////////////
// https handle


static nai_sult_t nai_https_prepare_uri(nai_https_request_t* r);
static nai_sult_t nai_https_prepare_headers_in(nai_https_request_t* r);
static nai_sult_t nai_https_handle_empty(nai_https_event_t* e);
static nai_sult_t nai_https_handle_wait(nai_https_event_t* e);
static nai_sult_t nai_https_handle_request_line(nai_https_event_t* e);
static nai_sult_t nai_https_handle_headers(nai_https_event_t* e);
static nai_sult_t nai_https_handle_request(nai_https_event_t* e);
static nai_sult_t nai_https_handle_read(nai_https_event_t* e);
static nai_sult_t nai_https_handle_send(nai_https_event_t* e);
static nai_sult_t nai_https_handle_post(nai_https_connection_t* c);



typedef struct nai_https_values_s {
    nai_str_t close;
    nai_str_t chunked;
    nai_str_t keepalive;
} nai_https_values_t;


static nai_https_values_t nai_https_values = {
    .close = nai_strconst("close"), 
    .chunked = nai_strconst("chunked"), 
    .keepalive = nai_strconst("keep-alive"), 
};


static nai_sult_t nai_https_prepare_uri(nai_https_request_t* r)
{
    nai_sultp_t rc;
    nai_mem_t uri;
    nai_mem_t str;
    size_t size;


    if (r->parsed_uri.path_empty) {
        r->valid_unparsed_uri = 0;
        rc = 0;
        goto _end;
    };


    size  = nai_str_len(&r->parsed_uri.path);
    uri.ptr = nai_palloc(r->pool, size + 1);
    if (uri.ptr == 0) {
        rc = nai_sult_from_errno();
        goto _end;
    };

    str = r->parsed_uri.path;
    nai_str_setl(&uri, size);
    if (!r->parsed_uri.path_escape && 
        !r->parsed_uri.path_complex) {
        nai_memcpy(nai_str(&uri), nai_str(&str), size);
    } else {

        if (r->parsed_uri.path_escape) {
            rc = nai_uri_unescape(
                uri.ptr, size, nai_str(&str), nai_str_len(&str));
            if (rc < 0) {
                rc = nai_sult_from_errno();
                goto _end;
            };

            nai_str_setl(&uri, rc);
            str = uri;
        };

        if (r->parsed_uri.path_complex) {
            rc = nai_path_canonicalize(
                uri.ptr, size, nai_str(&str), nai_str_len(&str), '/');
            if (rc < 0) {
                rc = nai_sult_from_errno();
                goto _end;
            };

            nai_str_setl(&uri, rc);
        };
    };

    nai_str_at(&uri, nai_str_len(&uri)) = 0;
    nai_str_setm(&r->uri, nai_str(&uri), nai_str_len(&uri));
    r->args = r->parsed_uri.query;
    r->valid_unparsed_uri = 1;

    rc = 0;


_end:
    return (nai_sult_t)rc;
};


static nai_sult_t nai_https_prepare_headers_in(nai_https_request_t* r)
{
    nai_sult_t rc;
    nai_str_t val;
    nai_https_header_t* hdr;
    nai_https_protocol_t* p;


    if (r->method == NAI_HTTP_TRACE) {
        rc = NAI_HTTP_NOT_ALLOWED;
        goto _end;
    };


    /* process host */
    hdr = r->headers_in.host;
    if (hdr == 0) {
        nai_log_info(NAI_LOG_HTTPS, EINVAL, 
            "HTTP/1.1 request without Host header");
        rc = NAI_HTTP_BAD_REQUEST;
        goto _end;
    };

    /* process connection */
    hdr = r->headers_in.connection;
    if (hdr) {
        val = hdr->value;
        if (nai_str_casecmp(&val, &nai_https_values.close) == 0) {
            r->headers_in.connection_type = NAI_HTTP_CLOSE;
        } else if (nai_str_casecmp(&val, &nai_https_values.keepalive) == 0) {
            r->headers_in.connection_type = NAI_HTTP_KEEPALIVE;
        } else {
            nai_log_info(NAI_LOG_HTTPS, EINVAL, 
                "request has an invalid value of Connection: %.*s", 
                nai_str_len(&hdr->value), nai_str(&hdr->value));
        };
    };

    /* process content-length */
    hdr = r->headers_in.content_length;
    if (hdr == 0) {
        r->headers_in.content_length_n = -1;
    } else {
        rc = nai_atoul(
            &r->headers_in.content_length_n, 
            nai_str(&hdr->value), nai_str_len(&hdr->value), 0);
        if (rc < 0) {
            nai_log_info(NAI_LOG_HTTPS, nai_errno, 
                "request has an invalid value of Content-Length: %.*s", 
                nai_str_len(&hdr->value), nai_str(&hdr->value));
            rc = NAI_HTTP_BAD_REQUEST;
            goto _end;
        };
    };

    /* process transfer-encoding */
    hdr = r->headers_in.transfer_encoding;
    if (hdr) {
        if (nai_str_caseeq(&hdr->value, &nai_https_values.chunked)) {
            r->headers_in.content_length_n = -1;
            r->headers_in.chunked = 1;
        } else {
            nai_log_info(NAI_LOG_HTTPS, EINVAL, 
                "request has an invalid value of Transfer-Encoding: %.*s", 
                nai_str_len(&hdr->value), nai_str(&hdr->value));
            rc = NAI_HTTP_NOT_IMPLEMENTED;
            goto _end;
        };
    };

    /* process keep-alive */
    if (r->headers_in.connection_type == NAI_HTTP_KEEPALIVE && 
        r->headers_in.keep_alive) {
        hdr = r->headers_in.keep_alive;
        nai_atoui(&r->headers_in.keep_alive_n, 
            nai_str(&hdr->value), nai_str_len(&hdr->value), 0);
    };

    if (r->headers_in.content_length_n != (uint64_t)-1) {
        p = r->proto;
        p->read_left = r->headers_in.content_length_n;
    };

    rc = 0;

_end:
    return rc;
};


static nai_sult_t nai_https_read_buffer(nai_https_connection_t* c)
{
    nai_sultp_t rc;
    nai_buflist_t tmp;
    nai_https_t* h = nai_https_get_service(c);
    nai_https_proto_v1_t* p;


    p = (nai_https_proto_v1_t*)c->proto;
    nai_buflist_init(&tmp, 0);
    nai_buflist_insert_tail(&tmp, p->head);

    /* read from next of trans_in  */
    rc = nai_chain_next((nai_chain_t*)&h->trans_in, c, &tmp, -1);

    nai_buf_entry_remove(p->head);
    return (nai_sult_t)rc;
};


static nai_sult_t nai_https_alloc_header_buffer(nai_https_request_t* r)
{
    nai_sult_t rc;
    nai_https_server_t* s;
    nai_https_proto_v1_t* p;
    nai_https_connection_t* c;
    size_t size;

    c = r->connection;
    p = (nai_https_proto_v1_t*)r->proto;
    if (p->head && p->head != p->hsmall) {
        nai_buflist_insert_tail(&p->head_list, p->head);
        p->head_bufs ++;
        p->head = 0;
    };

    s = nai_https_get_server(c);
    if (p->head_bufs >= (intptr_t)s->max_header_buffers.num) {
        rc = NAI_HTTP_HEADER_TOO_LARGE;
        goto _end;
    };

    size = nai_str_len(&p->parse.cur);
    if (size >= s->max_header_buffers.size) {
        rc = NAI_HTTP_REQUEST_URI_TOO_LARGE;
        goto _end;
    };

    /* lookup exist */
    p->head = (nai_buf_t*)p->head_list.ent.next;
    if (p->head && 
        nai_buf_space(p->head) >= s->max_header_buffers.size) {
        nai_buf_entry_remove(p->head);
    } else {
        /* no buffer, allocate a new buffer */
        p->head = nai_buf_alloc(&c->c.bufpool, s->max_header_buffers.size);
        if (p->head == 0) {
            rc = nai_sult_from_errno();
            goto _end;
        };
    };

    if (nai_str(&p->parse.cur)) {
        size = nai_str_len(&p->parse.cur);
        nai_memcpy(nai_buf_ptr(p->head), nai_str(&p->parse.cur), size);
        p->parse.cur.ptr = (char*)nai_buf_ptr(p->head);
    };

    rc = 0;

_end:
    return rc;
};



static nai_sult_t nai_https_handle_empty(nai_https_event_t* e)
{
    nai_log_debug(NAI_LOG_HTTPS, 
        0, "empty handle with event 0x%x", e->events);
    return NAI_DECLINED;
};


#if (NAI_HAVE_SSL)


extern nai_sult_t nai_https_ssl_create(
    nai_https_connection_t* c, nai_buf_t* head);
extern nai_sult_t nai_https_ssl_verify_cert(
    nai_https_connection_t* c);
extern nai_sult_t nai_https_ssl_verify_server(
    nai_https_connection_t* c, 
    nai_https_server_t* s, const nai_mem_t* server_name);


static nai_sult_t nai_https_handle_handshake(nai_https_event_t* e)
{
    nai_sult_t rc = 0;
    nai_https_connection_t* c;


    c = (nai_https_connection_t*)e->c;
    if (e->timedout) {
        rc = nai_errno_to_sult(ETIMEDOUT);
        nai_log_crit(NAI_LOG_HTTPS, 
            nai_sult_to_errno(rc), "connection timed out");
        goto _fail;
    };


    rc = nai_ssl_handshake(c->ssl);
    if (rc < 0) {
        rc = nai_sult_from_errno();
        if (rc != NAI_E_AGAIN) {
            nai_log_error(NAI_LOG_HTTPS, 
                nai_sult_to_errno(rc), "nai_ssl_handshake() failed");
            goto _fail;
        };

        rc = NAI_DECLINED;

    } else {
        c->c.read = nai_https_handle_wait;
        c->c.send = nai_https_handle_empty;
        rc = nai_https_handle_wait(e);
    };

_end:
    return rc;

_fail:
    nai_https_connection_close(c);

    /* tell caller the connection had been closed */
    rc = nai_errno_to_sult(ECONNABORTED);
    goto _end;
};


#endif


static nai_sult_t nai_https_handle_wait(nai_https_event_t* e)
{
    nai_sult_t rc = 0;
    nai_https_server_t* s;
    nai_https_proto_v1_t* p;
    nai_https_connection_t* c;

#if (NAI_HAVE_SSL)
    uint8_t ch;
#endif


    c = (nai_https_connection_t*)e->c;
    if (e->timedout) {
        rc = nai_errno_to_sult(ETIMEDOUT);
        nai_log_crit(NAI_LOG_HTTPS, 
            nai_sult_to_errno(rc), "connection timed out");
        goto _fail;
    };

    p = (nai_https_proto_v1_t*)c->proto;
    if (p == 0) {
        p = (nai_https_proto_v1_t*)nai_palloc(c->c.pool, sizeof(*p));

        /* this is http/1.x protocol */
        p->flags = 0;
        p->version = 1;

        /* we will set read timeout */
        p->timeoset |= NAI_EV_READ;

        /* init head buffer */
        p->head = 0;
        p->hsmall = 0;
        p->head_bufs = 0;
        nai_buflist_init(&p->head_list, &c->c.bufpool);

        /* init busy buffer */
        p->busy_bufs = 0;
        nai_buflist_init(&p->busy_list, &c->c.bufpool);

        /* init read buffer */
        p->read_body = 0;
        p->read_left = -1;
        nai_buflist_init(&p->read_list, &c->c.bufpool);

        /* init send buffer */
        p->send_body = 0;
        p->send_left = 0;
        nai_buflist_init(&p->send_list, &c->c.bufpool);


        /* init parse status */
        p->parse.state = 0;
        nai_str_setn(&p->parse.cur);
        nai_str_setn(&p->parse.stash);

        /* set to connection */
        c->proto = (nai_https_protocol_t*)p;
    };


    if (p->head == 0) {
        p->head = p->hsmall;
        if (p->head == 0) {
            s = nai_https_get_server(c);
            p->head = nai_buf_alloc(&c->c.bufpool, s->min_header_buffer);
            if (p->head == 0) {
                rc = nai_sult_from_errno();
                nai_log_alert(NAI_LOG_HTTPS, 
                    nai_sult_to_errno(rc), "alloc header buffer failed");
                goto _fail;
            };

            p->hsmall = p->head;
        };
    };


    /* do read */
    rc = nai_https_read_buffer(c);
    if (rc == 0) {
        rc = nai_errno_to_sult(ECONNABORTED);
        goto _fail;
    } else if (rc < 0) {
        if (rc == NAI_E_AGAIN || rc == NAI_E_INPROGRESS) {
            /* tell caller the connection is not closed */
            rc = NAI_DECLINED;
            goto _end;
        };

        nai_log_crit(NAI_LOG_HTTPS, 
            nai_sult_to_errno(rc), "read request failed");
        goto _fail;
    };


#if (NAI_HAVE_SSL)
    /* check ssl and do handshake */
    if (c->requests <= 0 && rc == 1 && 
        c->lc->opts.ssl && c->ssl == 0) {

        ch = *nai_buf_ptr(p->head);
        if (ch & 0x80 /* SSLv2 */ || ch == 0x16 /* SSLv3/TLSv1 */) {
            rc = nai_https_ssl_create(c, p->head);
            if (rc < 0) {
                nai_log_alert(NAI_LOG_HTTPS, 
                    nai_sult_to_errno(rc), "nai_https_ssl_create() failed");
                goto _fail;
            };

            p->head = 0;
            p->hsmall = 0;
            c->c.read = nai_https_handle_handshake;
            c->c.send = nai_https_handle_handshake;
            rc = c->c.send(e);
            goto _end;
        };
    };
#endif


    assert(p->main == 0);
    assert(p->request == 0);
    rc = nai_https_request_create(c, (nai_https_protocol_t*)p);
    if (rc < 0) {
        nai_log_alert(NAI_LOG_HTTPS, 
            nai_sult_to_errno(rc), "create request failed");
        goto _fail;
    };


    /* handle request line */
    c->c.read = nai_https_handle_request_line;
    rc = c->c.read(e);


_end:
    return rc;

_fail:
    (void)rc;
    nai_https_connection_close(c);

    /* tell caller the connection had been closed */
    rc = nai_errno_to_sult(ECONNABORTED);
    goto _end;
};


static nai_sult_t nai_https_handle_request_line(nai_https_event_t* e)
{
    nai_sult_t rc;
    nai_https_request_t* r;
    nai_https_proto_v1_t* p;
    nai_https_connection_t* c;
    nai_uri_t* u;
    char* old;
    intptr_t off;


    c = (nai_https_connection_t*)e->c;
    p = (nai_https_proto_v1_t*)c->proto;
    r = p->main;

    if (e->timedout) {
        rc = nai_errno_to_sult(ETIMEDOUT);
        nai_log_crit(NAI_LOG_HTTPS, 
            nai_sult_to_errno(rc), "request timed out");
        goto _fail;
    };

    for (;;) {
        assert(p->head != 0);
        if (nai_buf_size(p->head) <= 0) {
            rc = nai_https_read_buffer(c);
            if (rc < 0) {
                if (rc != NAI_E_AGAIN && rc != NAI_E_INPROGRESS) {
                    goto _fail;
                };

                /* tell caller the connection is not closed */
                rc = NAI_DECLINED;
                goto _end;
            };
        };

        rc = nai_https_parse_request_line(r);
        if (rc != 0) {
            if (rc != NAI_E_AGAIN) {
                nai_log_info(NAI_LOG_HTTPS, EINVAL, 
                    "invalid request line, parse failed");
                goto _fail;
            };

            if (nai_buf_space(p->head) <= 0) {
                old = nai_str(&p->parse.cur);

                /* get a larger buffer */
                rc = nai_https_alloc_header_buffer(r);
                if (rc != 0) {
                    goto _fail;
                };

                /* get the offset between the new one and the old one */
                off = nai_str(&p->parse.cur) - old;
                u = &r->parsed_uri;

                /* fix all pointer */
                if (nai_str(&r->method_name)) {
                    r->method_name.ptr += off;
                };
                if (nai_str(&r->protocol)) {
                    r->protocol.ptr += off;
                };
                if (nai_str(&r->unparsed_uri)) {
                    r->unparsed_uri.ptr += off;
                };
                if (nai_str(&r->exten)) {
                    r->exten.ptr += off;
                };
                if (nai_str(&u->scheme)) {
                    u->scheme.ptr += off;
                };
                if (nai_str(&u->user)) {
                    u->user.ptr += off;
                };
                if (nai_str(&u->password)) {
                    u->password.ptr += off;
                };
                if (nai_str(&u->host)) {
                    u->host.ptr += off;
                };
                if (nai_str(&u->hostname)) {
                    u->hostname.ptr += off;
                };
                if (nai_str(&u->portname)) {
                    u->portname.ptr += off;
                };
                if (nai_str(&u->path)) {
                    u->path.ptr += off;
                };
                if (nai_str(&u->query)) {
                    u->query.ptr += off;
                };
                if (nai_str(&u->fragment)) {
                    u->fragment.ptr += off;
                };
            };
            continue;
        };

        rc = nai_https_prepare_uri(r);
        if (rc < 0) {
            goto _fail;
        };

        if (r->version < NAI_HTTP_10) {
            if (r->method != NAI_HTTP_GET) {
                nai_log_info(NAI_LOG_HTTPS, EINVAL, 
                    "HTTP/0.9 request support method GET only");
                rc = NAI_HTTP_BAD_REQUEST;
                goto _fail;
            };

            r->headers_in.content_length_n = 0;
            p->read_left = 0;

            rc = nai_https_handle_request(e);
            goto _end;
        };


        /* success */
        p->parse.state = 0;
        c->c.read = nai_https_handle_headers;
        rc = c->c.read(e);
        goto _end;
    };

_fail:
    c->c.read = nai_https_handle_read;
    c->c.send = nai_https_handle_send;
    rc = nai_https_request_finalize(r, rc);
    if (rc != NAI_DECLINED) {
        /* the request had been closed */
        goto _end;
    };

    /* process post requests */
    rc = nai_https_handle_post(c);

_end:
    return rc;
};


static nai_sult_t nai_https_handle_headers(nai_https_event_t* e)
{
    nai_sult_t rc;
    nai_https_proto_v1_t* p;
    nai_https_request_t* r;
    nai_https_connection_t* c;


    c = (nai_https_connection_t*)e->c;
    p = (nai_https_proto_v1_t*)c->proto;
    r = p->main;

    if (e->timedout) {
        rc = nai_errno_to_sult(ETIMEDOUT);
        nai_log_crit(NAI_LOG_HTTPS, 
            nai_sult_to_errno(rc), "request timed out");
        goto _fail;
    };

    for (;;) {
        assert(p->head != 0);
        if (nai_buf_size(p->head) <= 0) {
            rc = nai_https_read_buffer(c);
            if (rc < 0) {
                if (rc != NAI_E_AGAIN && rc != NAI_E_INPROGRESS) {
                    goto _fail;
                };

                /* tell caller the connection is not closed */
                rc = NAI_DECLINED;
                goto _end;
            };
        };

        rc = nai_https_parse_headers(r);
        if (rc != 0) {
            if (rc != NAI_E_AGAIN) {
                nai_log_info(NAI_LOG_HTTPS, EINVAL, 
                    "invalid request header, parse failed");
                goto _fail;
            };

            if (nai_buf_space(p->head) <= 0) {
                /* get a larger buffer */
                rc = nai_https_alloc_header_buffer(r);
                if (rc != 0) {
                    goto _fail;
                };
            };
            continue;
        };

        rc = nai_https_prepare_headers_in(r);
        if (rc != 0) {
            goto _fail;
        };

        rc = nai_https_handle_request(e);
        goto _end;
    };

_fail:
    c->c.read = nai_https_handle_read;
    c->c.send = nai_https_handle_send;
    rc = nai_https_request_finalize(r, rc);
    if (rc != NAI_DECLINED) {
        /* the request had been closed */
        goto _end;
    };

    /* process post requests */
    rc = nai_https_handle_post(c);

_end:
    return rc;
};


static nai_sult_t nai_https_handle_request(nai_https_event_t* e)
{
    nai_sult_t rc;
    nai_uri_t uri;
    nai_mem_t hostname;
    nai_https_header_t* hdr;
    nai_https_request_t* r;
    nai_https_protocol_t* p;
    nai_https_connection_t* c;
    nai_https_server_t* server;


    /* setup connection handler */
    c = (nai_https_connection_t*)e->c;
    c->c.read = nai_https_handle_read;
    c->c.send = nai_https_handle_send;

    /* setup request handler */
    p = (nai_https_protocol_t*)c->proto;
    p->timeoset = 0;
    r = p->request;
    r->send = nai_https_core_handle;


#if (NAI_HAVE_SSL)
    /* check ssl */
    if (c->requests <= 1 && 
        c->lc->opts.ssl) {

        if (c->ssl == 0) {
            nai_log_info(NAI_LOG_HTTPS, 0, 
                "http request sent to https port");
            rc = NAI_HTTP_TO_HTTPS;
            goto _fail;
        };

        rc = nai_https_ssl_verify_cert(c);
        if (rc < 0 || rc > NAI_DECLINED) {
            /* error log in ssl verify */
            goto _fail;
        };
    };
#endif

    /* get hostname */
    if (nai_str_len(&r->parsed_uri.hostname) > 0) {

        hostname = r->parsed_uri.hostname;

    } else if ((hdr = r->headers_in.host) == 0) {

        hostname = r->parsed_uri.hostname;

    } else {

        rc = nai_uri_parse(&uri, 
            nai_str(&hdr->value), nai_str_len(&hdr->value), 
            NAI_URI_STRICT | NAI_URI_HOSTONLY);
        if (rc < 0) {
            nai_log_info(NAI_LOG_HTTPS, EINVAL, 
                "request has a invalid value of Host: %.*s", 
                nai_str_len(&hdr->value), nai_str(&hdr->value));
            rc = NAI_HTTP_BAD_REQUEST;
            goto _fail;
        };

        hostname = uri.hostname;
    };


    /* find server */
    rc = nai_https_server_find(
        r->connection->lc, &hostname, &server);
    if (rc >= 0) {
        r->conf = server->conf;
    } else {
        if (rc != nai_errno_to_sult(ENOENT)) {
            nai_log_alert(NAI_LOG_HTTPS, 
                nai_sult_to_errno(rc), "nai_https_server_find() failed");
            rc = NAI_HTTP_BAD_REQUEST;
            goto _fail;
        };
    };

#if (NAI_HAVE_SSL)
    if (c->ssl_servername) {
        rc = nai_https_ssl_verify_server(c, 
            nai_https_get_server(r), &hostname);
        if (rc < 0 || rc > NAI_DECLINED) {
            /* error log in ssl verify  */
            goto _fail;
        };
    };
#endif

    /* next */
    rc = c->c.send(e);

_end:
    return rc;

_fail:
    rc = nai_https_request_finalize(r, rc);
    if (rc != NAI_DECLINED) {
        /* the request had been closed */
        goto _end;
    };

    /* process post requests */
    rc = nai_https_handle_post(c);
    goto _end;
};


static nai_sult_t nai_https_handle_read(nai_https_event_t* e)
{
    nai_sult_t rc;
    nai_https_protocol_t* p;
    nai_https_request_t* r;
    nai_https_connection_t* c;


    c = (nai_https_connection_t*)e->c;
    p = (nai_https_protocol_t*)c->proto;
    p->readtimeo = e->timedout;
    p->sendtimeo = 0;


    r = p->main;
    if (r->read == 0) {
        rc = 0;
    } else {
        rc = r->read(r);
        if (rc != NAI_DECLINED && 
            rc != NAI_E_AGAIN && rc != NAI_E_INPROGRESS) {
            rc = nai_https_request_finalize(r, rc);
            if (rc != NAI_DECLINED) {
                /* the request had been closed */
                goto _end;
            };
        };

        /* process post requests */
        rc = nai_https_handle_post(c);
    };

_end:
    return rc;
};


static nai_sult_t nai_https_handle_send(nai_https_event_t* e)
{
    nai_sult_t rc;
    nai_https_protocol_t* p;
    nai_https_request_t* r;
    nai_https_connection_t* c;


    c = (nai_https_connection_t*)e->c;
    p = (nai_https_protocol_t*)c->proto;
    p->readtimeo = 0;
    p->sendtimeo = e->timedout;


    r = p->request;
    if (r->send == 0) {
        rc = 0;
    } else {
        rc = r->send(r);
        if (rc != NAI_DECLINED && 
            rc != NAI_E_AGAIN && rc != NAI_E_INPROGRESS) {
            rc = nai_https_request_finalize(r, rc);
            if (rc != NAI_DECLINED) {
                /* the request had been closed */
                goto _end;
            };
        };

        /* process post requests */
        rc = nai_https_handle_post(c);
    };

_end:
    return rc;
};


static nai_sult_t nai_https_handle_lingering(nai_https_event_t* e)
{
    nai_sultp_t rc;
    nai_buf_t* b;
    nai_https_server_t* s;
    nai_https_proto_v1_t* p;
    nai_https_connection_t* c;
    uint64_t now;


    c = (nai_https_connection_t*)e->c;
    p = (nai_https_proto_v1_t*)c->proto;

    if (e->timedout) {
        rc = nai_errno_to_sult(NAI_ETIMEDOUT);
        goto _fail;
    };

    now = nai_tickcache_to_msec();
    if (now >= c->lingering_time) {
        rc = nai_errno_to_sult(NAI_ETIMEDOUT);
        goto _fail;
    };


    while (p->head == 0) {
        p->head = p->hsmall;
        if (p->head) {
            break;
        };

        s = nai_https_get_server(p->main);
        p->head = nai_buf_alloc(&c->c.bufpool, s->min_header_buffer);
        if (p->head == 0) {
            rc = nai_sult_from_errno();
            nai_log_alert(NAI_LOG_HTTPS, 
                nai_sult_to_errno(rc), 
                "alloc buffer failed on lingering close");
            goto _fail;
        };
        break;
    };

    b = p->head;
    b->total += b->start - (uint8_t*)b->ref.obj;
    b->start = (uint8_t*)b->ref.obj;
    for (;;) {

        rc = nai_stream_read(&c->c.str, nai_buf_ptr(b), nai_buf_space(b));
        if (rc < 0) {
            rc = nai_sult_from_errno();
            if (rc != NAI_E_AGAIN && rc != NAI_E_INPROGRESS) {
                goto _fail;
            };

            /* tell caller the connection is not closed */
            rc = NAI_DECLINED;
            goto _end;
        };
        if (rc == 0) {
            break;
        };
    };

    /* close connection */
    rc = nai_https_handle_termiate(c, "lingering close");

_end:
    return (nai_sult_t)rc;

_fail:
    rc = nai_https_request_terminate(p->main, (nai_sult_t)rc);
    goto _end;
};


static nai_sult_t nai_https_handle_post(nai_https_connection_t* c)
{
    nai_sult_t rc;
    nai_https_request_t* r;
    nai_https_protocol_t* p;


    p = (nai_https_protocol_t*)c->proto;
    for (;;) {
        r = p->post;
        if (r == 0) {
            break;
        };

        if (c->c.error) {
            rc = nai_https_request_finalize(r, 0);
            if (rc != NAI_DECLINED) {
                /* the request had been closed */
                goto _end;
            };
            break;
        };

        p->post = r->next;
        r->posted = 0;
        if (r->send == 0) {
            continue;
        };

        rc = r->send(r);
        if (rc != NAI_DECLINED && 
            rc != NAI_E_AGAIN && rc != NAI_E_INPROGRESS) {
            rc = nai_https_request_finalize(r, rc);
            if (rc != NAI_DECLINED) {
                /* the request had been closed */
                goto _end;
            };
        };
    };

    rc = NAI_DECLINED;

_end:
    return rc;
};


nai_sult_t nai_https_handle_start(nai_https_connection_t* c)
{
    nai_int_t events;
    nai_sult_t rc;
    nai_https_server_t* s;


    /* set handle */
    c->c.read = nai_https_handle_wait;
    c->c.send = nai_https_handle_empty;


    /* set read timeout */
    s = nai_https_get_server(c);
    rc = nai_stream_set_opt(&c->c.str, 
        NAI_IO_RECVTIMEO, (intptr_t)s->read_head_timeo);
    if (rc < 0) {
        rc = nai_sult_from_errno();
        nai_log_error(NAI_LOG_HTTPS, 
            nai_sult_to_errno(rc), 
            "set the timeout for read request head failed");
        goto _fail;
    };

    /* process */
    events = nai_stream_get_event(&c->c.str);
    if (events) {
        c->c.str.cb(&c->c.str, events);
    };

    /* this function is called by accept, always return 0 */
    rc = 0;

_end:
    return rc;

_fail:
    nai_https_connection_close(c);
    goto _end;
};


nai_sult_t nai_https_handle_termiate(
    nai_https_connection_t* c, const char* procname)
{
    nai_sult_t rc;
    nai_https_proto_v1_t* p;


    p = (nai_https_proto_v1_t*)c->proto;

    /* close buffer lists, 
     * because the buffer in list well be invalid after close request */
    nai_buflist_close(&p->send_list);
    nai_buflist_close(&p->read_list);
    nai_buflist_close(&p->busy_list);

    /* close request */
    if (p->main) {
        rc = nai_https_request_close(p->main);
        if (rc < 0) {
            nai_log_error(NAI_LOG_HTTPS, 
                nai_sult_to_errno(rc), 
                "close request failed on %s", procname);
        };
    };

    /* close connection */
    rc = nai_https_connection_close(c);
    if (rc < 0) {
        nai_log_error(NAI_LOG_HTTPS, 
            nai_sult_to_errno(rc), 
            "close connection failed on %s", procname);
    };

    /* tell caller the connection is closed */
    rc = nai_errno_to_sult(ECONNABORTED);

    return rc;
};


nai_sult_t nai_https_handle_keepalive(nai_https_request_t* r)
{
    size_t size;
    size_t write;
    uint64_t csize;
    nai_sult_t rc;
    nai_buf_t* b;
    nai_buf_t* d;
    nai_list_entry_t* e;
    nai_https_proto_v1_t* p;
    nai_https_server_t* s;
    nai_https_location_t* l;
    nai_https_connection_t* c;
    nai_https_event_t ev;


    c = r->connection;
    p = (nai_https_proto_v1_t*)r->proto;

    /* clear head buffers */
    b = p->head;
    if (b && nai_buf_size(b) <= 0) {
        if (b != p->hsmall) {
            nai_buflist_insert_tail(&p->head_list, b);
        };
        b = 0;
        p->head = 0;
    };
    if (p->hsmall && p->hsmall != b) {
        d = p->hsmall;
        d->total += d->start - (uint8_t*)d->ref.obj;
        d->start = (uint8_t*)d->ref.obj;
        d->size = 0;
    };
    e = p->head_list.ent.next;
    for ( ; e != &p->head_list.ent; e = e->next) {
        d = (nai_buf_t*)e;
        d->total += d->start - (uint8_t*)d->ref.obj;
        d->start = (uint8_t*)d->ref.obj;
        d->size = 0;
    };
    if (b && b != p->hsmall) {
        p->head_bufs = 1;
    } else {
        p->head_bufs = 0;
        p->head = p->hsmall;
    };


    /* move the content in body buffers to header buffer */
    if (p->reading) {

        d = p->head;

        /* calculate the remaining data size */
        csize = nai_buflist_size(&p->read_list, 0, 0);

        /* get a larger area if there is not enough space */
        if (csize > nai_buf_space(d)) {
            s = nai_https_get_server(c);
            if (csize > s->max_header_buffers.size) {
                nai_log_alert(NAI_LOG_HTTPS, ERANGE, 
                    "the body processing reads too many the content "
                    "of the next request");
                rc = NAI_HTTP_INTERNAL_SERVER_ERROR;
                goto _fail;
            };

            if (d == p->hsmall) {
                e = p->head_list.ent.next;
                if (e != &p->head_list.ent) {
                    nai_list_entry_remove(e);
                } else {
                    e = (nai_list_entry_t*)nai_buf_alloc(
                        &c->c.bufpool, s->max_header_buffers.size);
                    if (e == 0) {
                        rc = nai_sult_from_errno();
                        nai_log_alert(NAI_LOG_HTTPS, nai_sult_to_errno(rc), 
                            "alloc header buffer failed");
                        goto _fail;
                    };
                };

                d = (nai_buf_t*)e;
                p->head = d;
                p->head_bufs = 1;
            };
        };
        d->total += d->start - (uint8_t*)d->ref.obj;
        d->start = (uint8_t*)d->ref.obj;
        d->size = 0;

        /* move memory areas that may overlap to avoid overwriting */
        write = 0;
        e = p->read_list.ent.next;
        for ( ; e != &p->read_list.ent; ) {
            b = (nai_buf_t*)e;
            e = e->next;

            size = nai_buf_size(b);
            if (size <= 0) {
                nai_buf_entry_remove(b);
                nai_buf_close(b);
                continue;
            };

            /* check whether the memory area overlaps */
            if (nai_buf_ptr(b) < nai_buf_ptr(d) || 
                nai_buf_ptr(b) >= nai_buf_ptr(d) + nai_buf_total(d)) {
                /* not overlapping memory area, skip */
                write += size;
                continue;
            };

            /* move data when the overlapping area 
             * is not the first one (write != 0) and 
             * the destination address is less than the source address */
            if (write != 0 && 
                nai_buf_ptr(d) + write < nai_buf_ptr(b)) {
                nai_memmove(nai_buf_ptr(d) + write, nai_buf_ptr(b), size);
                b->start = nai_buf_ptr(d) + write;
                b->total = size;
            };

            /* stop, because there is only one overlapped memory area */
            break;
        };

        /* merge the remaining buffers into head buffer */
        write = 0;
        e = p->read_list.ent.next;
        for ( ; e != &p->read_list.ent; ) {
            b = (nai_buf_t*)e;
            e = e->next;

            size = nai_buf_size(b);
            if (size <= 0) {
                nai_buf_entry_remove(b);
                nai_buf_close(b);
                continue;
            };

            /* if the overlapping area is the first and 
             * the remaining space is sufficient, 
             * directly modify the buffer address and size to avoid copying */
            if (write == 0 && 
                nai_buf_ptr(b) >= nai_buf_ptr(d) && 
                nai_buf_ptr(b) < nai_buf_ptr(d) + nai_buf_total(d) && (
                e == &p->read_list.ent || 
                nai_buf_space(b) >= (csize - size))) {
                d->total -= b->start - d->start;
                d->start = b->start;
            } else {
                if (nai_buf_space(d) < size) {
                    nai_log_alert(NAI_LOG_HTTPS, ERANGE, 
                        "the body processing reads too many the content "
                        "of the next request");
                    rc = NAI_HTTP_INTERNAL_SERVER_ERROR;
                    goto _fail;
                };

                if (nai_buf_ptr(d) + nai_buf_size(d) != nai_buf_ptr(b)) {
                    /* we use memcpy here, 
                     * because pointer of body always greater than head, 
                     * no copy overwrite
                     */
                    nai_memcpy(nai_buf_ptr(d) + 
                        nai_buf_size(d), nai_buf_ptr(b), size);
                };
            };

            nai_buf_wcommit(d, size);
            write += size;

            assert(b->refcount == 1);
            if (b->refcount != 1) {
                nai_log_warn(NAI_LOG_HTTPS, 0, 
                    "the boby buffer is in use, there may be a leak");
            };
            nai_buf_entry_remove(b);
            nai_buf_close(b);
        };

        e = p->busy_list.ent.next;
        for ( ; e != &p->busy_list.ent; ) {
            b = (nai_buf_t*)e;
            e = e->next;

            assert(b->refcount == 1);
            if (b->refcount != 1) {
                nai_log_warn(NAI_LOG_HTTPS, 0, 
                    "the busy buffer is in use, there may be a leak");
            };
            nai_buf_entry_remove(b);
            nai_buf_close(b);
        };
    };


    /* save for set keepalive */
    l = nai_https_get_location(p->main);


    /* close request */
    rc = nai_https_request_close(p->main);
    if (rc < 0) {
        nai_log_error(NAI_LOG_HTTPS, 
            nai_sult_to_errno(rc), 
            "close request failed on handle keepalive");

        p->main = 0;
        goto _fail;
    };


    /* reset */
    p->main = 0;
    p->request = 0;
    p->post = 0;
    p->header_sent = 0;
    p->reading = 0;
    p->sending = 0;
    p->read_body = 0;
    p->read_left = -1;
    p->parse.state = 0;


    /* check content */
    b = p->head;
    if (b && nai_buf_size(b) > 0) {
        c->c.read = nai_https_handle_request_line;
        c->c.send = nai_https_handle_empty;
        s = nai_https_get_server(c);

        rc = nai_stream_set_opt(&c->c.str, 
            NAI_IO_RECVTIMEO, (intptr_t)s->read_head_timeo);
        if (rc < 0) {
            rc = nai_sult_from_errno();
            nai_log_error(NAI_LOG_HTTPS, 
                nai_sult_to_errno(rc), 
                "set the timeout for read request head failed");
            goto _fail;
        };

        rc = nai_https_request_create(c, (nai_https_protocol_t*)c->proto);
        if (rc < 0) {
            nai_log_alert(NAI_LOG_HTTPS, 
                nai_sult_to_errno(rc), 
                "create request failed on handle keepalive");
            goto _fail;
        };

    } else {

        c->c.read = nai_https_handle_wait;
        c->c.send = nai_https_handle_empty;

        rc = nai_stream_set_opt(&c->c.str, 
            NAI_IO_RECVTIMEO, (intptr_t)l->keepalive_timeo);
        if (rc < 0) {
            rc = nai_sult_from_errno();
            nai_log_error(NAI_LOG_HTTPS, 
                nai_sult_to_errno(rc), 
                "set the timeout to keep the conection failed");
            goto _fail;
        };
    };


    if (c->c.tcp) {
        if (c->c.tcp_nopush == 1 && l->tcp_nopush) {
            rc = nai_https_connection_tcp_nopush(c, 0);
            if (rc < 0) {
                if (rc != nai_errno_to_sult(ENOTSUP)) {
                    nai_log_error(NAI_LOG_HTTPS, 
                        nai_sult_to_errno(rc), "set nopush option failed");
                    goto _fail;
                };
            };
        };
        if (c->c.tcp_nodelay == 0 && l->tcp_nodelay) {
            rc = nai_https_connection_tcp_nodelay(c, 1);
            if (rc < 0) {
                nai_log_error(NAI_LOG_HTTPS, 
                    nai_sult_to_errno(rc), "set nodelay option failed");
                goto _fail;
            };
        };
    };


    ev.c = &c->c;
    ev.events = NAI_EV_READ;
    rc = c->c.read(&ev);

_end:
    return rc;

_fail:
    (void)rc;
    rc = nai_https_handle_termiate(c, "keepalive");
    goto _end;
};


nai_sult_t nai_https_handle_close(nai_https_request_t* r)
{
    nai_sult_t rc;
    nai_https_event_t e;
    nai_https_location_t* l;
    nai_https_connection_t* c;


    c = r->connection;
    l = nai_https_get_location(r);

    if (c->lingering_time == 0) {
        c->lingering_time = nai_tickcache_to_msec() + 
            l->lingering_timeomax;
    };

    /* shutdown send */
    rc = nai_stream_shutdown(&c->c.str, NAI_SOCK_WR);
    if (rc < 0) {
        rc = nai_sult_from_errno();
        nai_log_error(NAI_LOG_HTTPS, 
            nai_sult_to_errno(rc), "shudown(wr) connection failed");
        goto _fail;
    };

    /* set lingering timeout */
    rc = nai_stream_set_opt(
        &c->c.str, NAI_IO_RECVTIMEO, (intptr_t)l->lingering_timeo);
    if (rc < 0) {
        rc = nai_sult_from_errno();
        nai_log_error(NAI_LOG_HTTPS, 
            nai_sult_to_errno(rc), 
            "set the lingering timeout for the request failed");
        goto _fail;
    };

    /* set handles */
    c->c.read = nai_https_handle_lingering;
    c->c.send = nai_https_handle_empty;

    /* handle lingering */
    e.events = nai_stream_get_event(&c->c.str);
    if (e.events & NAI_EV_READ) {
        e.c = &c->c;
        rc = c->c.read(&e);
    } else {
        rc = NAI_DECLINED;
    };

_end:
    return rc;

_fail:
    rc = nai_https_request_terminate(r, rc);
    goto _end;
};


