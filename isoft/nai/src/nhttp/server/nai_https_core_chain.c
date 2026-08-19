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
/// @file       nai_https_core_chain.c
/// @brief      
/// @details
/// @date       2021-02-19
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_log.h"
#include "nai/runtime/nai_uri.h"
#include "nai/runtime/nai_time.h"
#include "nai/service/nai_chain.h"
#include "nai/service/nai_main.h"
#include "nai_https_core.h"
#include <stdlib.h>
#include <time.h>



typedef struct nai_https_values_s {
    nai_str_t expect;
} nai_https_values_t;


static nai_https_values_t nai_https_values = {
    .expect = nai_strconst("100-continue"), 
};


static nai_str_t nai_https_continue_line = 
    nai_strconst("HTTP/1.1 100 Continue" CRLF CRLF);

static nai_str_t nai_https_server_line = nai_strconst("Server: nhttp" CRLF);



//////////////////////////////////////////////////////////////////////////////
// https core header output filter


static nai_str_t nai_https_status_lines[] = {

#define NAI_HTTPS_2XX_LAST  207

    nai_strconst("200 OK"),
    nai_strconst("201 Created"),
    nai_strconst("202 Accepted"),
    nai_strnull(),  /* "203 Non-Authoritative Information" */
    nai_strconst("204 No Content"),
    nai_strnull(),  /* "205 Reset Content" */
    nai_strconst("206 Partial Content"),

    /* nai_strnull(), */  /* "207 Multi-Status" */

#define NAI_HTTPS_3XX_OFF   (NAI_HTTPS_2XX_LAST - 200)
#define NAI_HTTPS_3XX_LAST  309

    /* nai_strnull(), */  /* "300 Multiple Choices" */

    nai_strconst("301 Moved Permanently"),
    nai_strconst("302 Moved Temporarily"),
    nai_strconst("303 See Other"),
    nai_strconst("304 Not Modified"),
    nai_strnull(),  /* "305 Use Proxy" */
    nai_strnull(),  /* "306 unused" */
    nai_strconst("307 Temporary Redirect"),
    nai_strconst("308 Permanent Redirect"),

#define NAI_HTTPS_4XX_OFF   (NAI_HTTPS_3XX_LAST - 301 + NAI_HTTPS_3XX_OFF)
#define NAI_HTTPS_4XX_LAST  430

    nai_strconst("400 Bad Request"),
    nai_strconst("401 Unauthorized"),
    nai_strconst("402 Payment Required"),
    nai_strconst("403 Forbidden"),
    nai_strconst("404 Not Found"),
    nai_strconst("405 Not Allowed"),
    nai_strconst("406 Not Acceptable"),
    nai_strnull(),  /* "407 Proxy Authentication Required" */
    nai_strconst("408 Request Time-out"),
    nai_strconst("409 Conflict"),
    nai_strconst("410 Gone"),
    nai_strconst("411 Length Required"),
    nai_strconst("412 Precondition Failed"),
    nai_strconst("413 Request Entity Too Large"),
    nai_strconst("414 Request-URI Too Large"),
    nai_strconst("415 Unsupported Media Type"),
    nai_strconst("416 Requested Range Not Satisfiable"),
    nai_strnull(),  /* "417 Expectation Failed" */
    nai_strnull(),  /* "418 unused" */
    nai_strnull(),  /* "419 unused" */
    nai_strnull(),  /* "420 unused" */
    nai_strconst("421 Misdirected Request"),
    nai_strnull(),  /* "422 Unprocessable Entity" */
    nai_strnull(),  /* "423 Locked" */
    nai_strnull(),  /* "424 Failed Dependency" */
    nai_strnull(),  /* "425 unused" */
    nai_strnull(),  /* "426 Upgrade Required" */
    nai_strnull(),  /* "427 unused" */
    nai_strnull(),  /* "428 Precondition Required" */
    nai_strconst("429 Too Many Requests"),

#define NAI_HTTPS_5XX_OFF   (NAI_HTTPS_4XX_LAST - 400 + NAI_HTTPS_4XX_OFF)
#define NAI_HTTPS_5XX_LAST  508

    nai_strconst("500 Internal Server Error"),
    nai_strconst("501 Not Implemented"),
    nai_strconst("502 Bad Gateway"),
    nai_strconst("503 Service Temporarily Unavailable"),
    nai_strconst("504 Gateway Time-out"),
    nai_strconst("505 HTTP Version Not Supported"),
    nai_strnull(),        /* "506 Variant Also Negotiates" */
    nai_strconst("507 Insufficient Storage"),

    /* nai_strnull(), */  /* "508 unused" */
    /* nai_strnull(), */  /* "509 unused" */
    /* nai_strnull(), */  /* "510 Not Extended" */


};




#define nai_copymem(d, s, l) (                          \
    nai_memcpy((d), (s), (l)), (l))                     \

#define nai_copystr(d, s) (                             \
    nai_copymem((d), nai_str(s), nai_str_len(s)))       \

#define nai_copyconst(d, s) (                           \
    nai_copymem((d), (s), sizeof(s) - 1))               \

#define nai_copychar(d, c) (                            \
    *(d) = (c), 1)                                      \




static nai_sultp_t nai_https_core_header_out(
    nai_https_request_t* r, nai_buflist_t* out, size_t limit)
{
    nai_int_t status;
    int64_t bsize;
    char* buf;
    size_t len;
    nai_str_t status_line;
    nai_sultp_t rc;
    nai_list_entry_t* e;
    nai_https_header_t* hdr;
    nai_https_proto_v1_t* p;
    nai_https_connection_t* c;
    nai_https_location_t* l;
    nai_buflist_info_t i;
    nai_buf_t* b;


    (void)limit;

    c = r->connection;
    p = (nai_https_proto_v1_t*)r->proto;
    l = nai_https_get_location(r);

    /* try get content-length */
    if (l && !r->headers_out.chunked) {
        if (r->headers_out.content_length_n == (uint64_t)-1 && 
            r->headers_out.content_length == 0) {
            bsize = nai_buflist_size(out, 1, &i);
            if (bsize < 0) {
                rc = nai_sult_from_errno();
                goto _fail;
            };

            if (i.eos) {
                r->headers_out.content_length_n = bsize;
            };
        };
    };

    /* count the length of status line */
    len = sizeof("HTTP/1.1 ") - 1 + 
        sizeof(CRLF) - 1 + 
        sizeof(CRLF) - 1;

    if (nai_str_len(&r->status_line) > 0) {
        status_line = r->status_line;
        len += nai_str_len(&status_line);
    } else {
        status = r->status;
        if (status >= NAI_HTTP_OK && status < NAI_HTTPS_2XX_LAST) {
            status -= NAI_HTTP_OK;
            status_line = nai_https_status_lines[status];
            len += nai_str_len(&status_line);
        } else if (status >= NAI_HTTP_MOVED_PERMANENTLY && 
            status < NAI_HTTPS_3XX_LAST) {
            status -= NAI_HTTP_MOVED_PERMANENTLY - NAI_HTTPS_3XX_OFF;
            status_line = nai_https_status_lines[status];
            len += nai_str_len(&status_line);
        } else if (status >= NAI_HTTP_BAD_REQUEST && 
            status < NAI_HTTPS_4XX_LAST) {
            status -= NAI_HTTP_BAD_REQUEST - NAI_HTTPS_4XX_OFF;
            status_line = nai_https_status_lines[status];
            len += nai_str_len(&status_line);
        } else if (status >= NAI_HTTP_INTERNAL_SERVER_ERROR && 
            status < NAI_HTTPS_5XX_LAST) {
            status -= NAI_HTTP_INTERNAL_SERVER_ERROR - NAI_HTTPS_5XX_OFF;
            status_line = nai_https_status_lines[status];
            len += nai_str_len(&status_line);
        } else {
            nai_str_setn(&status_line);
            len += NAI_INT_T_LEN + 1;
        };
    };

    /* count the length of header lines */
    if (r->headers_out.server == 0) {
        len += nai_str_len(&nai_https_server_line);
    };
    if (r->headers_out.date == 0) {
        len += sizeof("Date: ") + NAI_TIME_RFC822_LEN + sizeof(CRLF) - 2;
    };
    if (r->headers_out.last_modified == 0 && 
        r->headers_out.last_modified_time != 0) {
        len += sizeof("Last-Modified: ") + 
            NAI_TIME_RFC822_LEN + sizeof(CRLF) - 2;
    };
    if (r->headers_out.content_length == 0 && 
        r->headers_out.content_length_n != (uint64_t)-1) {
        len += sizeof("Content-Length: ") + NAI_INT64_T_LEN + sizeof(CRLF) - 2;
    };

    if (r->headers_out.chunked) {
        len += sizeof("Transfer-Encoding: chunked" CRLF) - 1;
    };
    if (r->status == NAI_HTTP_SWITCHING_PROTOCOLS) {
        len += sizeof("Connection: upgrade" CRLF) - 1;
    } else if (r->keepalive == 0) {
        len += sizeof("Connection: close" CRLF) - 1;
    } else {
        len += sizeof("Connection: keep-alive" CRLF) - 1;
        if (l->keepalive_timeo > 1000) {
            len += sizeof("Keep-Alive: " CRLF) - 1 + 
                nai_str_len(&l->keepalive_timeo_str);
        };
    };

    e = r->headers_out.list.next;
    for ( ; e != &r->headers_out.list; e = e->next) {
        hdr = (nai_https_header_t*)e;
        len += nai_str_len(&hdr->name);
        len += nai_str_len(&hdr->value);
        len += sizeof(": ") + sizeof(CRLF) - 2;
    };


    /* alloc header buffer */
    b = nai_buf_alloc(&c->c.bufpool, len);
    if (b == 0) {
        rc = nai_sult_from_errno();
        goto _fail;
    };

    buf = (char*)nai_buf_ptr(b);


    /* output status line */
    buf += nai_copyconst(buf, "HTTP/1.1 ");
    if (nai_str_len(&status_line) > 0) {
        buf += nai_copymem(buf, 
            nai_str(&status_line), nai_str_len(&status_line));
    } else {
        buf += nai_sprintf(buf, "%03u ", r->status);
    };
    buf += nai_copychar(buf, '\r');
    buf += nai_copychar(buf, '\n');


    if (r->headers_out.server == 0) {
        buf += nai_copystr(buf, &nai_https_server_line);
    };

    if (r->headers_out.date == 0) {
        buf += nai_copyconst(buf, "Date: ");
        buf += nai_time_to_rfc822(
            nai_timecache_to_sec(), buf, NAI_TIME_RFC822_LEN);
        buf += nai_copychar(buf, '\r');
        buf += nai_copychar(buf, '\n');
    };

    if (r->status == NAI_HTTP_SWITCHING_PROTOCOLS) {
        buf += nai_copyconst(buf, "Connection: upgrade" CRLF);
    } else if (r->keepalive == 0) {
        buf += nai_copyconst(buf, "Connection: close" CRLF);
    } else {
        buf += nai_copyconst(buf, "Connection: keep-alive" CRLF);
        if (l->keepalive_timeo > 1000) {
            buf += nai_copyconst(buf, "Keep-Alive: ");
            buf += nai_copystr(buf, &l->keepalive_timeo_str);
            buf += nai_copychar(buf, '\r');
            buf += nai_copychar(buf, '\n');
        };
    };

    if (r->headers_out.content_length == 0 && 
        r->headers_out.content_length_n != (uint64_t)-1) {
        buf += nai_copyconst(buf, "Content-Length: ");
        buf += nai_sprintf(buf, "%" NAI_INT64_FMT "u", 
            r->headers_out.content_length_n);
        buf += nai_copychar(buf, '\r');
        buf += nai_copychar(buf, '\n');
    };

    if (r->headers_out.last_modified == 0 && 
        r->headers_out.last_modified_time != 0) {
        buf += nai_copyconst(buf, "Last-Modified: ");
        buf += nai_time_to_rfc822(r->headers_out.last_modified_time, 
            buf, NAI_TIME_RFC822_LEN);
        buf += nai_copychar(buf, '\r');
        buf += nai_copychar(buf, '\n');
    };

    if (r->headers_out.chunked) {
        buf += nai_copyconst(buf, "Transfer-Encoding: chunked" CRLF);
    };

    e = r->headers_out.list.next;
    for ( ; e != &r->headers_out.list; e = e->next) {
        hdr = (nai_https_header_t*)e;
        buf += nai_copystr(buf, &hdr->name);
        buf += nai_copychar(buf, ':');
        buf += nai_copychar(buf, ' ');
        buf += nai_copystr(buf, &hdr->value);
        buf += nai_copychar(buf, '\r');
        buf += nai_copychar(buf, '\n');
    };

    buf += nai_copychar(buf, '\r');
    buf += nai_copychar(buf, '\n');
    nai_buf_wcommit(b, buf - (char*)nai_buf_ptr(b));
    nai_buflist_insert_tail(&p->send_list, b);
    p->send_left += nai_buf_size(b);


    p->header_sent = 1;
    r->test_expect = 0;
    rc = 0;

_fail:
    return rc;
};


static nai_sultp_t nai_https_core_body_send(
    nai_chain_t* n, void* ctx, nai_buflist_t* out, size_t limit)
{
    nai_sultp_t rc;
    nai_sultp_t rs;
    nai_https_t* h;
    nai_https_request_t* r;
    nai_https_protocol_t* p;
    nai_https_connection_t* c;


    (void)n;

    r = (nai_https_request_t*)ctx;
    assert(r != 0);

    h = nai_https_get_service(r);
    p = r->proto;
    c = r->connection;
    if (nai_buflist_is_empty(&p->send_list)) {
        rc = nai_chain_this(&h->trans_out, c, out, limit);
        if (rc < 0) {
            goto _end;
        };
    } else if (out == 0) {
        while (p->send_left > 0) {
            rc = nai_chain_this(&h->trans_out, c, &p->send_list, -1);
            if (rc < 0) {
                goto _end;
            };

            p->send_left -= rc;
        };

        rc = 0;
    } else {

        if (p->send_left > 512*1024) {
            while (p->send_left > 32*1024) {
                rc = nai_chain_this(&h->trans_out, c, &p->send_list, -1);
                if (rc < 0) {
                    goto _end;
                };

                p->send_left -= rc;
            };
        };

        rc = nai_buflist_move(&p->send_list, out, limit, 0, 0);
        if (rc < 0) {
            rc = nai_sult_from_errno();
            goto _end;
        };

        p->send_left += rc;

        while (nai_buflist_is_empty(&p->send_list)) {
            rs = nai_chain_this(&h->trans_out, c, &p->send_list, -1);
            if (rs < 0) {
                if (rs != NAI_E_AGAIN && rs != NAI_E_INPROGRESS) {
                    rc = rs;
                    goto _end;
                };

                if (rc == 0) {
                    rc = NAI_E_AGAIN;
                };
                break;
            };

            p->send_left -= rc;
        };
    };

_end:
    return rc;
};


static nai_sultp_t nai_https_core_body_out(
    nai_chain_t* n, void* ctx, nai_buflist_t* out, size_t limit)
{
    char* s;
    intptr_t m;
    uint64_t bsize;
    nai_sultp_t rc;
    nai_buf_t* b;
    nai_buf_t* t;
    nai_buflist_t temp;
    nai_buflist_info_t i;
    nai_https_request_t* r;
    nai_https_protocol_t* p;


    (void)n;

    r = (nai_https_request_t*)ctx;
    assert(r != 0);


    p = r->proto;
    if (!p->header_sent) {
        rc = nai_https_core_header_out(r, out, limit);
        if (rc < 0) {
            goto _end;
        };

        p->sending = 1;
    };

    if (!r->headers_out.chunked) {
        bsize = r->headers_out.content_length_n - p->send_body;
        if (bsize > limit) {
            bsize = limit;
        };

        rc = nai_https_core_body_send(n, ctx, out, (size_t)bsize);
        if (rc > 0) {
            p->send_body += rc;
            if (p->send_body >= r->headers_out.content_length_n) {
                p->sending = 0;
            };
        };

        goto _end;

    };

    if (out != 0) {
        if (p->sending == 0) {
            goto _flush;
        };

        if (p->send_left > 512 * 1024) {
            rc = nai_https_core_body_send(n, ctx, 0, -1);
            if (rc < 0) {
                goto _end;
            };
        };

        /* move chunk */
        b = t = 0;
        nai_buflist_init(&temp, p->send_list.pool);
        rc = nai_buflist_move(&temp, out, limit, 1, &i);
        if (rc < 0) {
            rc = nai_sult_from_errno();
            nai_log_error(NAI_LOG_HTTPS, 
                nai_sult_to_errno(rc), "nai_buflist_move failed");
            goto _fail;
        };
        if (rc >= (intptr_t)limit) {
            rc = NAI_E_INTERNAL;
            nai_log_error(NAI_LOG_HTTPS, 
                nai_sult_to_errno(rc), 
                "the return of nai_buflist_move not as expect");
            goto _fail;
        };

        /* flush only */
        bsize = rc;
        if (bsize <= 0 && !i.eos) {
            nai_buflist_close(&temp);
            goto _flush;
        };

        /* allocate chunk header */
        b = nai_buf_alloc(
            p->send_list.pool, sizeof("0000000000000000" CRLF CRLF));
        if (b == 0) {
            rc = nai_sult_from_errno();
            nai_log_alert(NAI_LOG_HTTPS, 
                nai_sult_to_errno(rc), "allocate chunk header failed");
            goto _fail;
        };

        /* allocate chunk tailers */
        if (i.eos) {
            t = nai_buf_from_rmemory(
                p->send_list.pool, "0" CRLF CRLF, 5, 0);
            if (t == 0) {
                rc = nai_sult_from_errno();
                nai_log_alert(NAI_LOG_HTTPS, 
                    nai_sult_to_errno(rc), "allocate chunk tailers failed");
                goto _fail;
            };
        };


        /* add chunk header */
        s = (char*)nai_buf_ptr(b);
        if (p->send_body) {
            s[0] = '\r';
            s[1] = '\n';
            s += 2;
        };
        s += nai_snprintf(s, 
            nai_buf_space(b) - 2, "%" NAI_INTPTR_FMT "x", (size_t)bsize);
        s[0] = '\r';
        s[1] = '\n';
        s += 2;
        m = s - (char*)nai_buf_ptr(b);

        nai_buf_wcommit(b, m);
        nai_buflist_insert_tail(&p->send_list, b);
        p->send_left += m;

        /* add chunk */
        nai_buflist_add_tail(&p->send_list, &temp);
        p->send_left += bsize;
        p->send_body += bsize;

        /* add chunk tailers */
        if (t != 0) {
            nai_buflist_insert_tail(&p->send_list, t);
            p->send_left += nai_buf_size(t);
            p->sending = 0;
            r->headers_out.content_length_n = p->send_body;
        };
    };


_flush:
    rc = nai_https_core_body_send(n, ctx, 0, -1);

_end:
    return rc;

_fail:
    nai_buflist_add_head(out, &temp);
    nai_buf_close(b);
    if (t != 0) {
        nai_buf_close(t);
    };
    goto _end;
};



//////////////////////////////////////////////////////////////////////////////
// https core body input filter


static nai_sultp_t nai_https_core_body_in(
    nai_chain_t* n, void* ctx, nai_buflist_t* in, size_t limit)
{
    nai_int_t got;
    nai_int_t chunked;
    size_t left;
    size_t size;
    size_t total;
    nai_sultp_t rc;
    nai_buf_t* b;
    nai_buf_t* d;
    nai_buf_t* last;
    nai_bufpool_t* pool;
    nai_list_entry_t* e;
    nai_https_t* h;
    nai_https_location_t* l;
    nai_https_proto_v1_t* p;
    nai_https_request_t* r;
    nai_https_connection_t* c;


    (void)n;

    r = (nai_https_request_t*)ctx;
    assert(r != 0);


    p = (nai_https_proto_v1_t*)r->proto;
    if (r != p->main) {
        rc = 0;
        goto _end;
    };

    l = nai_https_get_location(r);
    if (p->read_body > (uint64_t)l->max_body_size) {
        rc = NAI_HTTP_REQUEST_ENTITY_TOO_LARGE;
        rc = nai_sult_from_http_status(rc);
        goto _end;
    };


    c = r->connection;
    h = nai_https_get_service(r);

    /* send 100 continue */
    if (r->test_expect) {
        r->test_expect = 0;

        if (r->version >= NAI_HTTP_11 && 
            r->headers_in.expect && nai_str_caseeq(
            &r->headers_in.expect->value, 
            &nai_https_values.expect)) {

            b = nai_buf_from_rmemory(&c->c.bufpool, 
                nai_str(&nai_https_continue_line), 
                nai_str_len(&nai_https_continue_line), 0);
            if (b == 0) {
                rc = nai_sult_from_errno();
                nai_log_alert(NAI_LOG_HTTPS, 
                    nai_sult_to_errno(rc), 
                    "alloc buffer failed when send 100-continue");
                goto _end;
            };

            nai_buflist_insert_tail(&p->send_list, b);
            p->send_left += nai_buf_size(b);

            rc = nai_chain_this(&h->trans_out, c, 0, -1);
            if (rc < 0 && 
                rc != NAI_E_AGAIN && rc != NAI_E_INPROGRESS) {
                goto _end;
            };
        };
    };

    /* reach the end of conntent */
    if (p->read_left == 0) {
        rc = 0;
        goto _end;
    };

    if (p->reading == 0) {

        nai_buflist_init(&p->read_list, r->bufpool);

        /* clone content from head buffer */
        d = p->head;
        size = nai_buf_size(d);
        if (size > 0 || nai_buf_space(d) >= 256) {

            b = nai_buf_from_rmemory(r->bufpool, nai_buf_ptr(d), size, 0);
            if (b == 0) {
                rc = nai_sult_from_errno();
                nai_log_alert(NAI_LOG_HTTPS, 
                    nai_sult_to_errno(rc), 
                    "alloc buffer failed when read body");
                goto _end;
            };

            nai_buf_rcommit(d, size);
            b->total += nai_buf_space(d);
            nai_buflist_insert_tail(&p->read_list, b);
        };

        /* clone the head buffer space for read body data */
        e = p->head_list.ent.next;
        for ( ; e != &p->head_list.ent; e = e->next) {
            d = (nai_buf_t*)e;
            if (nai_buf_space(d) < 256) {
                continue;
            };

            b = nai_buf_from_wmemory(r->bufpool, 
                nai_buf_ptr(d) + nai_buf_size(d), nai_buf_space(d), 0);
            if (b == 0) {
                rc = nai_sult_from_errno();
                nai_log_alert(NAI_LOG_HTTPS, 
                    nai_sult_to_errno(rc), 
                    "alloc buffer failed when read body");
                goto _end;
            };

            nai_buflist_insert_tail(&p->read_list, b);
        };

        p->reading = 1;
        p->parse.state = 0;
    };

    /* select pool */
    pool = in->pool;
    if (pool == 0) {
        pool = r->bufpool;
    };

    /* get last buffer */
    e = in->ent.next;
    if (e == &in->ent) {
        last = 0;
    } else {
        last = (nai_buf_t*)e;
    };

    chunked = r->headers_in.chunked;

    /* body read loop */
    got = 1;
    total = 0;
    for (;;) {

        e = p->read_list.ent.next;
        if (e == &p->read_list.ent) {
            goto _read;
        };

        b = (nai_buf_t*)e;
        size = nai_buf_size(b);
        if (size <= 0) {
            if (b->refcount == 1) {
                /* no reference, reset it */
                b->total += b->start - (uint8_t*)b->ref.obj;
                b->start = (uint8_t*)b->ref.obj;

            } else if (nai_buf_space(b) <= 0) {
                nai_buf_entry_remove(b);
                nai_buflist_insert_tail(&p->busy_list, b);
                p->busy_bufs ++;
                continue;
            };

            /* if next buffer have content, move current to last */
            e = e->next;
            d = (nai_buf_t*)e;
            if (e != &p->read_list.ent && nai_buf_size(d) > 0) {
                nai_buf_entry_remove(b);
                nai_buflist_insert_tail(&p->read_list, b);
                continue;
            };

            /* if current one has enough space, we unneed alloc more buffer */
            if (nai_buf_space(b) > 256) {
                e = (nai_list_entry_t*)b;
            };

            goto _read;
        };

        /* process content */
        if (chunked && p->read_left == (uint64_t)-1) {

            rc = nai_https_parse_chunked(r, b);
            if (rc != 0) {
                if (rc > 0) {
                    rc = nai_sult_from_http_status(rc);
                    goto _end;
                } else if (rc != NAI_E_AGAIN) {
                    goto _end;
                };

                assert(nai_buf_size(b) == 0);

            } else if (p->read_left == 0) {
                /* finished */
                break;
            } else if (nai_buf_size(b) > 0) {
                continue;
            };

            /* no content left, we can reuse it */
            b->start -= size;
            b->total += size;

            /* if next buffer have content, move current to last */
            e = e->next;
            d = (nai_buf_t*)e;
            if (e != &p->read_list.ent && nai_buf_size(d) > 0) {
                nai_buf_entry_remove(b);
                nai_buflist_insert_tail(&p->read_list, b);
                continue;
            };

            /* no content in body_list, fail to read */

        } else {

            left = NAI_SIZE_T_MAX;
            if (left > p->read_left) {
                left = (size_t)p->read_left;
            }
            if (left > limit) {
                left = limit;
            };

            if (size > left) {

                size = left;
                if (last && nai_buf_ptr(b) == 
                    nai_buf_ptr(last) + nai_buf_size(last) && 
                    last->ref.buf == b) {

                    /* sequential buffer, no new nodes need to be allocated */
                    last->size += size;
                    last->total += size;

                } else {

                    /* clone a buffer reference */
                    d = nai_buf_sub(pool, b, 0, size, 0);
                    if (d == 0) {
                        rc = nai_sult_from_errno();
                        nai_log_alert(NAI_LOG_HTTPS, 
                            nai_sult_to_errno(rc), 
                            "dup buffer failed when read body");
                        goto _end;
                    };

                    nai_buflist_insert_tail(in, d);
                };

                nai_buf_rcommit(b, size);

            } else {

                if (last && nai_buf_ptr(b) == 
                    nai_buf_ptr(last) + nai_buf_size(last) && 
                    last->ref.buf == b) {

                    /* sequential buffer, no new nodes need to be allocated */
                    last->size += size;
                    last->total += size;

                } else {

                    /* clone a buffer reference because it has space */
                    d = nai_buf_sub(pool, b, 0, size, 0);
                    if (d == 0) {
                        rc = nai_sult_from_errno();
                        nai_log_alert(NAI_LOG_HTTPS, 
                            nai_sult_to_errno(rc), 
                            "dup buffer failed when read body");
                        goto _end;
                    };

                    nai_buflist_insert_tail(in, d);
                    last = d;
                };

                /* remove buffer if it no more space */
                if (nai_buf_space(b) <= 0) {
                    nai_buf_entry_remove(b);
                    nai_buflist_insert_tail(&p->busy_list, b);
                    p->busy_bufs ++;
                } else {
                    nai_buf_rcommit(b, size);
                };
            };

            p->read_body += size;
            total += size;
            limit -= size;

            if (p->read_left != (uint64_t)-1) {
                p->read_left -= size;
                if (p->read_left == 0) {
                    if (chunked) {
                        p->read_left = -1;
                    } else {
                        /* finished */
                        break;
                    };
                };
            };

            if (limit <= 0) {
                break;
            };

            continue;
        };

_read:
        /* if e isn't point to a buffer, means need more space to read */
        if (e == &p->read_list.ent) {
            /* if the free buffer is not found last time, 
             * it will not be searched again. */
            if (got) {
                got = 0;
                e = p->busy_list.ent.next;
                for ( ; e != &p->busy_list.ent; e = e->next) {
                    b = (nai_buf_t*)e;

                    if (b->refcount == 1) {
                        b->total += b->start - (uint8_t*)b->ref.obj;
                        b->start = (uint8_t*)b->ref.obj;
                        b->size = 0;
                        p->busy_bufs --;
                        nai_buf_entry_remove(b);
                        got = 1;
                        break;
                    };
                };
            };

            /* if there is no available buffer, allocate a new one */
            if (got == 0) {
                b = nai_buf_alloc(r->bufpool, l->read_chunk_size);
                if (b == 0) {
                    rc = nai_sult_from_errno();
                    nai_log_alert(NAI_LOG_HTTPS, 
                        nai_sult_to_errno(rc), 
                        "alloc buffer failed when read body");
                    goto _end;
                };
            };

            nai_buflist_insert_tail(&p->read_list, b);
        };

        /* reading */
        rc = nai_chain_next(&h->trans_in, c, &p->read_list, -1);
        if (rc < 0) {
            if (total == 0 || (
                rc != NAI_E_AGAIN && rc != NAI_E_INPROGRESS)) {
                goto _end;
            };

            break;
        };
        if (rc == 0) {
            p->read_left = 0;
            break;
        };
    };

    rc = total;

_end:
    return rc;
};


//////////////////////////////////////////////////////////////////////////////
// https core postoned filter


static nai_sultp_t nai_https_core_postpone_queue(
    nai_https_request_t* r, nai_buflist_t* out, size_t limit)
{
    nai_sultp_t rc;
    nai_https_postpone_t* post;
    nai_https_postpone_t* prev;


    /* find last one */
    prev = r->postpone;
    for ( ; prev; prev = prev->next) {
        if (prev->next == 0) {
            break;
        };
    };

    /* append new content */
    if (prev && prev->request == 0) {

        /* last one is a data queue, directly append new data to the end */
        rc = nai_buflist_move(&prev->content, out, limit, 0, 0);

    } else {

        /* last one isn't a data queue, create a new one */
        post = (nai_https_postpone_t*)nai_palloc(r->pool, sizeof(*post));
        if (post == 0) {
            rc = nai_sult_from_errno();
            nai_log_alert(NAI_LOG_HTTPS, 
                nai_sult_to_errno(rc), 
                "alloc postoned node failed when queue write data");
            goto _end;
        };

        /* initialize */
        post->request = 0;
        post->next = 0;
        nai_buflist_init(&post->content, r->bufpool);

        /* append to end of the postoned list */
        if (prev != 0) {
            prev->next = post;
        } else {
            r->postpone = post;
        };

        /* append new data */
        rc = nai_buflist_move(&post->content, out, limit, 0, 0);
    };

_end:
    return rc;
};


static nai_sultp_t nai_https_core_postpone_out(
    nai_chain_t* n, void* ctx, nai_buflist_t* out, size_t limit)
{
    nai_sultp_t rc;
    nai_sultp_t rs;
    nai_https_postpone_t* post;
    nai_https_protocol_t* p;
    nai_https_request_t* r;


    r = (nai_https_request_t*)ctx;
    assert(r != 0);

    p = r->proto;
    if (r != p->request) {
        if (out == 0) {
            rc = NAI_E_AGAIN;
        } else {
            /* queue in waiting list */
            rc = nai_https_core_postpone_queue(r, out, limit);
            if (rc == 0) {
                rc = NAI_E_AGAIN;
            };
        };
        return rc;
    };

    if (r->postpone == 0) {
        rc = nai_chain_next(n, ctx, out, limit);
        goto _end;
    };


    if (out == 0) {
        rc = NAI_E_AGAIN;
    } else {
        /* queue in waiting list */
        rc = nai_https_core_postpone_queue(r, out, limit);
        if (rc < 0) {
            goto _end;
        };
        if (rc == 0) {
            rc = NAI_E_AGAIN;
        };
    };


    do {

        post = r->postpone;
        if (post->request) {
            rs = nai_https_post(post->request);
            if (rs < 0) {
                rc = rs;
                goto _end;
            };

            p->request = post->request;
            r->postpone = post->next;
            goto _end;
        };

        for (;;) {
            rs = nai_chain_next(n, ctx, &post->content, -1);
            if (rs < 0) {
                if (rs != NAI_E_AGAIN && 
                    rs != NAI_E_INPROGRESS) {
                    rc = rs;
                };
                goto _end;
            };
            if (nai_buflist_is_empty(&post->content)) {
                break;
            };
        };

        r->postpone = post->next;

    } while (r->postpone);


    if (rc == NAI_E_AGAIN) {
        rc = 0;
    };

_end:
    return rc;
};


//////////////////////////////////////////////////////////////////////////////
// https core io chains


static nai_sultp_t nai_https_core_stream_in(
    nai_chain_t* n, void* ctx, nai_buflist_t* in, size_t limit)
{
    nai_int_t ec;
    nai_sultp_t rc;
    nai_https_connection_t* c;


    (void)n;

    c = (nai_https_connection_t*)ctx;
    assert(c != 0);

    if (in == 0) {
        rc = 0;
    } else {
#if (NAI_HAVE_SSL)
        if (c->ssl) {
            rc = nai_ssl_readq(c->ssl, in, limit);
        } else
#endif
        rc = nai_stream_readq(&c->c.str, in, limit);

        if (rc < 0) {
            ec = nai_errno;
            if (ec != NAI_EAGAIN && ec != NAI_EINPROGRESS) {
                nai_https_connection_get_name(c, NAI_HTTPS_PEERNAME, 0, 0);
                nai_log_error(NAI_LOG_HTTPS, ec, 
                    "read connection(%.*s:%d) failed", 
                    nai_str_len(&c->c.peer_host), nai_str(&c->c.peer_host), 
                    c->c.peer_port);
            };

            rc = nai_errno_to_sult(ec);

        } else if (rc == 0) {
            if (nai_buflist_space(in, 1)) {
                c->c.closed = 1;
            };
        };
    };

    return rc;
};


static nai_sultp_t nai_https_core_stream_out(
    nai_chain_t* n, void* ctx, nai_buflist_t* out, size_t limit)
{
    nai_int_t ec;
    nai_sultp_t rc;
    nai_https_connection_t* c;


    (void)n;

    c = (nai_https_connection_t*)ctx;
    assert(c != 0);

    if (out == 0) {
        rc = 0;
    } else {
#if (NAI_HAVE_SSL)
        if (c->ssl) {
            rc = nai_ssl_writeq(c->ssl, out, limit);
        } else
#endif
        rc = nai_stream_writeq(&c->c.str, out, limit);

        if (rc < 0) {
            ec = nai_errno;
            if (ec != NAI_EAGAIN && ec != NAI_EINPROGRESS) {
                nai_https_connection_get_name(c, NAI_HTTPS_PEERNAME, 0, 0);
                nai_log_error(NAI_LOG_HTTPS, ec, 
                    "write connection(%.*s:%d) failed", 
                    nai_str_len(&c->c.peer_host), nai_str(&c->c.peer_host), 
                    c->c.peer_port);
            };

            rc = nai_errno_to_sult(ec);
        };
    };

    return rc;
};


static nai_sultp_t nai_https_core_trans_in(
    nai_chain_t* n, void* ctx, nai_buflist_t* in, size_t limit)
{
    nai_sultp_t rc;
    nai_buf_t* b;
    nai_https_protocol_t* p;
    nai_https_proto_v1_t* v;
    nai_https_request_t* r;
    nai_https_connection_t* c;
    nai_https_location_t* l;
    char* str;
    size_t len;
    size_t copy;


    if (in == 0) {
        rc = 0;
        goto _end;
    };


    c = (nai_https_connection_t*)ctx;
    assert(c != 0);

    p = c->proto;
    if (p == 0) {
        rc = nai_chain_next(n, c, in, limit);
        goto _end;
    };

    /* set read timeout */
    if (!(p->timeoset & NAI_EV_READ)) {
        r = p->main;
        l = r != 0 ? 
            nai_https_get_location(r) : 
            nai_https_get_location(c);

        rc = nai_stream_set_opt(&c->c.str, 
            NAI_IO_RECVTIMEO, (uint32_t)l->read_body_timeo);
        if (rc < 0) {
            rc = nai_sult_from_errno();
            nai_log_error(NAI_LOG_HTTPS, 
                nai_sult_to_errno(rc), 
                "set the timeout for read request body failed");
            goto _end;
        };

        p->timeoset |= NAI_EV_READ;
    };

    copy = 0;
    if (p->version == 1) {
        v = (nai_https_proto_v1_t*)p;
        b = v->head;
        if (b && nai_buf_size(b)) {
            str = (char*)nai_buf_ptr(b);
            len = nai_buf_size(b);
            if (len > limit) {
                len = limit;
            };

            rc = nai_buflist_write(in, str, len, 0);
            if (rc < 0) {
                rc = nai_sult_from_errno();
                goto _end;
            };

            /* need more data */
            nai_buf_rcommit(b, rc);
            copy = rc;
        };
    };

    /* read from next chain */
    if (limit - copy > 0) {
        rc = nai_chain_next(n, c, in, limit - copy);
        if (rc < 0) {
            if (copy <= 0 || (
                rc != NAI_E_AGAIN && rc != NAI_E_INPROGRESS)) {
                goto _end;
            };
            rc = 0;
        };

        copy += rc;
    };


    rc = copy;


_end:
    return rc;
};


static nai_sultp_t nai_https_core_trans_out(
    nai_chain_t* n, void* ctx, nai_buflist_t* out, size_t limit)
{
    nai_sultp_t rc;
    nai_https_protocol_t* p;
    nai_https_request_t* r;
    nai_https_connection_t* c;
    nai_https_location_t* l;


    c = (nai_https_connection_t*)ctx;
    assert(c != 0);

    p = c->proto;
    if (p == 0) {
        rc = nai_chain_next(n, c, out, limit);
        goto _end;
    };

    /* set write timeout */
    if (!(p->timeoset & NAI_EV_WRITE)) {
        r = p->main;
        l = r != 0 ? 
            nai_https_get_location(r) : 
            nai_https_get_location(c);

        rc = nai_stream_set_opt(&c->c.str, 
            NAI_IO_SENDTIMEO, (uint32_t)l->send_timeo);
        if (rc < 0) {
            rc = nai_sult_from_errno();
            nai_log_error(NAI_LOG_HTTPS, 
                nai_sult_to_errno(rc), 
                "set the timeout for send request failed");
            goto _end;
        };

        p->timeoset |= NAI_EV_WRITE;
    };

    rc = nai_chain_next(n, c, out, limit);

_end:
    return rc;
};


static nai_sultp_t nai_https_core_proto_in(
    nai_chain_t* n, void* ctx, nai_buflist_t* in, size_t limit)
{
    nai_sultp_t rc;
    nai_https_request_t* r;


    r = (nai_https_request_t*)ctx;
    assert(r != 0);

    rc = nai_chain_next(n, r, in, limit);

    return rc;
};


static nai_sultp_t nai_https_core_proto_out(
    nai_chain_t* n, void* ctx, nai_buflist_t* out, size_t limit)
{
    nai_sultp_t rc;
    nai_https_request_t* r;


    r = (nai_https_request_t*)ctx;
    assert(r != 0);

    /* output request content first */
    if (!nai_buflist_is_empty(&r->content) && 
        out != &r->content) {

        while (!nai_buflist_is_empty(&r->content)) {
            rc = nai_chain_next(n, ctx, &r->content, -1);
            if (rc < 0) {
                goto _end;
            };
        };
    };

    /* output */
    rc = nai_chain_next(n, r, out, limit);

_end:
    return rc;
};


static nai_sult_t nai_https_core_add_chain(nai_chain_t* l, 
    nai_chain_t* c, const char* name, nai_filter_f f, nai_int_t priority)
{
    nai_sult_t r;

    nai_chain_init(c, name, f, priority);
    r = nai_chain_link(l, c);
    if (r < 0) {
        r = nai_sult_from_errno();
    };

    return r;
};


static nai_sult_t nai_https_core_new_chain(nai_chain_t* l, 
    nai_pool_t* p, const char* name, nai_filter_f f, nai_int_t priority)
{
    nai_sult_t r;
    nai_chain_t* c;

    c = nai_palloc(p, sizeof(*c));
    if (c == 0) {
        r = nai_sult_from_errno();
        goto _end;
    };

    r = nai_https_core_add_chain(l, c, name, f, priority);

_end:
    return r;
};


nai_sult_t nai_https_core_chain_init(nai_https_t* h)
{
    nai_sult_t rc;
    nai_main_t* m = h->main;
    nai_pool_t* p = &m->pool;


    nai_chain_init(&h->proto_in, "top", nai_https_core_proto_in, 0);
    nai_chain_init(&h->proto_out, "top", nai_https_core_proto_out, 0);

    /* add core input filters */
    rc = nai_https_core_new_chain(&h->proto_in, p, 
        "body", nai_https_core_body_in, NAI_HTTPS_FTYPE_TRANSPORT - 5);
    if (rc < 0) {
        nai_log_alert(NAI_LOG_HTTPS, 
            nai_sult_to_errno(rc), 
            "add input filter '%s' failed", "body");
        goto _end;
    };
    rc = nai_https_core_add_chain(&h->proto_in, &h->trans_in, 
        "trans", nai_https_core_trans_in, NAI_HTTPS_FTYPE_TRANSPORT - 4);
    if (rc < 0) {
        nai_log_alert(NAI_LOG_HTTPS, 
            nai_sult_to_errno(rc), 
            "add input filter '%s' failed", "trans");
        goto _end;
    };
    rc = nai_https_core_new_chain(&h->proto_in, p, 
        "stream", nai_https_core_stream_in, NAI_HTTPS_FTYPE_SOURCE);
    if (rc < 0) {
        nai_log_alert(NAI_LOG_HTTPS, 
            nai_sult_to_errno(rc), 
            "add input filter '%s' failed", "stream");
        goto _end;
    };

    /* add core output filters */
    rc = nai_https_core_new_chain(&h->proto_out, p, 
        "postpone", nai_https_core_postpone_out, NAI_HTTPS_FTYPE_PROTOCOL - 5);
    if (rc < 0) {
        nai_log_alert(NAI_LOG_HTTPS, 
            nai_sult_to_errno(rc), 
            "add output filter '%s' failed", "postpone");
        goto _end;
    };
    rc = nai_https_core_new_chain(&h->proto_out, p, 
        "body", nai_https_core_body_out, NAI_HTTPS_FTYPE_TRANSPORT - 5);
    if (rc < 0) {
        nai_log_alert(NAI_LOG_HTTPS, 
            nai_sult_to_errno(rc), 
            "add output filter '%s' failed", "header");
        goto _end;
    };
    rc = nai_https_core_add_chain(&h->proto_out, &h->trans_out, 
        "trans", nai_https_core_trans_out, NAI_HTTPS_FTYPE_TRANSPORT - 4);
    if (rc < 0) {
        nai_log_alert(NAI_LOG_HTTPS, 
            nai_sult_to_errno(rc), 
            "add output filter '%s' failed", "trans");
        goto _end;
    };
    rc = nai_https_core_new_chain(&h->proto_out, p, 
        "stream", nai_https_core_stream_out, NAI_HTTPS_FTYPE_SOURCE);
    if (rc < 0) {
        nai_log_alert(NAI_LOG_HTTPS, 
            nai_sult_to_errno(rc), 
            "add output filter '%s' failed", "stream");
        goto _end;
    };

_end:
    return rc;
};


