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
/// @file       nai_ssl_ocsp.c
/// @brief      
/// @details
/// @date       2023-03-31
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai/io/nai_ssl_ctx.h"


#if (NAI_HAVE_SSL)


#include "nai/io/nai_ssl_ocsp.h"
#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_time.h"
#include "nai/runtime/nai_log.h"
#include "nai/runtime/nai_util.h"



#ifndef OPENSSL_NO_OCSP


#if (NAI_HAVE_OPENSSL) || (NAI_HAVE_LIBRESSL)
#include <openssl/ocsp.h>
#elif (NAI_HAVE_WOLFSSL)
#include <wolfssl/openssl/ocsp.h>
#endif



#ifndef _NAI_TYPEDEF_SSL_OCSP_CONF_T
#define _NAI_TYPEDEF_SSL_OCSP_CONF_T
typedef struct nai_ssl_ocsp_conf_s nai_ssl_ocsp_conf_t;
#endif


struct nai_ssl_ocsp_conf_s {
    nai_pool_t pool;
    nai_conn_attr_t conn;
    nai_uri_t uri;
    nai_ssl_store_t* store;
    nai_ssl_store_t* private;
    nai_atomic32_t lock;
    nai_int_t mode;
};



static void nai_ssl_ocsp_cleanup_store(void* p)
{
    nai_ssl_store_t* st;


    st = (nai_ssl_store_t*)p;
    nai_ssl_store_close(st);
    return;
};


static nai_ssl_ocsp_conf_t* 
    nai_ssl_ctx_get_ocsp_conf(nai_ssl_ctx_t* s, nai_int_t na)
{
    nai_pool_t pool;
    nai_ssl_ocsp_conf_t* c;


    c = (nai_ssl_ocsp_conf_t*)SSL_CTX_get_ex_data(s->ctx, nai_ssl.ocsp_conf);
    if (c != 0 || na) {
        goto _end;
    };

    nai_pool_init(&pool, 4000);
    c = (nai_ssl_ocsp_conf_t*)nai_palloc(&pool, sizeof(*c));
    if (c == 0) {
        goto _end;
    };

    nai_pool_init(&c->pool, 0);
    nai_pool_exchange(&c->pool, &pool);
    nai_conn_attr_init(&c->conn);
    nai_uri_init(&c->uri);
    c->store = 0;
    c->private = 0;
    c->lock = 0;
    c->mode = NAI_SSL_OCSP_OFF;
    SSL_CTX_set_ex_data(s->ctx, nai_ssl.ocsp_conf, c);

_end:
    return c;
};


static nai_ssl_store_t* 
    nai_ssl_ctx_get_ocsp_store(nai_ssl_ctx_t* ctx, nai_int_t na)
{
    nai_int_t r;
    nai_int_t ec;
    nai_ssl_store_t* st;
    nai_ssl_ocsp_conf_t* cf;


    cf = nai_ssl_ctx_get_ocsp_conf(ctx, 1);
    if (cf->store && nai_ssl_store_is_opened(cf->store)) {
        st = cf->store;
    } else if (cf->private || na) {
        st = cf->private;
    } else {
        nai_atomic32_lock(&cf->lock);

        if (cf->private == 0) {
            st = nai_palloc(&cf->pool, sizeof(*st));
            if (st == 0) {
                ec = nai_errno;
                goto _fail;
            };

            nai_ssl_store_init(st);

            r = nai_ssl_store_open(st, 16*1024);
            if (r < 0) {
                ec = nai_errno;
                goto _fail;
            };

            r = nai_pool_add_cleanup(
                &cf->pool, nai_ssl_ocsp_cleanup_store, st);
            if (r < 0) {
                ec = nai_errno;
                goto _fail;
            };

            cf->private = st;
        };

        nai_atomic32_unlock(&cf->lock);

        st = cf->private;
    };

_end:
    return st;

_fail:
    if (st) {
        nai_ssl_store_close(st);
        nai_pool_free_last(&cf->pool, st, sizeof(*st));
    };
    nai_atomic32_unlock(&cf->lock);
    nai_errno = ec;
    st = 0;
    goto _end;
};



#define NAI_SSL_OCSP_STAT_NONE      0
#define NAI_SSL_OCSP_STAT_CONNECT   1
#define NAI_SSL_OCSP_STAT_SEND      2
#define NAI_SSL_OCSP_STAT_STATUS    3
#define NAI_SSL_OCSP_STAT_HEADER    4
#define NAI_SSL_OCSP_STAT_BODY      5
#define NAI_SSL_OCSP_STAT_FETCH     6
#define NAI_SSL_OCSP_STAT_DONE      7


#define NAI_SSL_OCSP_REQSIZE        4096
#define NAI_SSL_OCSP_RCVSIZE        8192


#define nai_copymem(d, s, l) (                          \
    nai_memcpy((d), (s), (l)), (l))                     \

#define nai_copystr(d, s) (                             \
    nai_copymem((d), nai_str(s), nai_str_len(s)))       \

#define nai_copyconst(d, s) (                           \
    nai_copymem((d), (s), sizeof(s) - 1))               \

#define nai_copychar(d, c) (                            \
    *(d) = (c), 1)                                      \



static nai_int_t nai_ssl_ocsp_make_request_impl(
    nai_ssl_ocsp_t* p, nai_uri_t* uri)
{
    nai_int_t r;
    nai_int_t ec;
    nai_int_t port;
    intptr_t len;
    intptr_t b64;
    char* b;
    char* q;
    OCSP_CERTID* id;
    OCSP_REQUEST* req;


    if (p->out.buf == 0) {
        len = NAI_SSL_OCSP_REQSIZE + NAI_SSL_OCSP_RCVSIZE;
        p->out.buf = nai_palloc(&p->pool, len);
        if (p->out.buf == 0) {
            r = -1;
            goto _end;
        };

        p->out.size = 0;
        p->out.alloc = NAI_SSL_OCSP_REQSIZE;

        p->in.buf = p->out.buf + p->out.alloc;
        p->in.size = 0;
        p->in.alloc = NAI_SSL_OCSP_RCVSIZE;
    };

    req = OCSP_REQUEST_new();
    if (req == 0) {
        nai_ssl_set_errno(ENOMEM);
        r = -1;
        goto _end;
    };

    id = OCSP_cert_to_id(0, p->cert, p->issuer);
    if (id == 0) {
        nai_ssl_set_errno(ENOMEM);
        r = -1;
        goto _fail;
    };

    if (OCSP_request_add0_id(req, id) == 0) {
        nai_ssl_set_errno(ENOMEM);
        r = -1;
        goto _fail;
    };
#if defined(LIBWOLFSSL_VERSION_HEX)
    OCSP_CERTID_free(id);
#endif
    id = 0;

    len = i2d_OCSP_REQUEST(req, 0);
    if (len <= 0) {
        nai_ssl_set_errno(EINVAL);
        r = -1;
        goto _fail;
    };

    if (len > p->out.alloc) {
        nai_errno = ERANGE;
        r = -1;
        goto _end;
    };

    q = p->out.buf;
    len = i2d_OCSP_REQUEST(req, (uint8_t**)&q);
    if (len <= 0) {
        nai_ssl_set_errno(EINVAL);
        r = -1;
        goto _fail;
    };

    b64 = nai_encode_base64_len(len);
    b = nai_palloc(&p->pool, b64);
    if (b == 0) {
        r = -1;
        goto _fail;
    };

    q = p->out.buf;
    b64 = nai_encode_base64(b, b64, q, len);
    OCSP_REQUEST_free(req);


    /* append status line */
    q = p->out.buf;
    q += nai_copyconst(q, "GET ");
    q += nai_copystr(q, &uri->path);
    if (nai_str_len(&uri->path) <= 0 || 
        nai_str_at(&uri->path, nai_str_len(&uri->path)-1) != '/') {
        q += nai_copychar(q, '/');
    };

    len = nai_uri_escape(q, 
        p->out.alloc - (q - (char*)p->out.buf), 
        b, b64, NAI_ESCAPE_URI_COMPONENT);
    if (len < 0) {
        r = -1;
        goto _end;
    };
    if (len >= p->out.alloc - (q - (char*)p->out.buf)) {
        assert(0);
        nai_errno = ERANGE;
        r = -1;
        goto _end;
    };

    nai_pool_free_last(&p->pool, b, b64);
    q += len;


    /* append headers */
    len = sizeof(" HTTP/1.0\r\n") - 1 + 
        sizeof("Host: ") - 1 + nai_str_len(&uri->hostname) + 
        sizeof("\r\n") - 1 + 
        sizeof("\r\n") - 1;
    if (len >= p->out.alloc - (q - (char*)p->out.buf)) {
        assert(0);
        nai_errno = ERANGE;
        r = -1;
        goto _end;
    };

    q += nai_copyconst(q, " HTTP/1.0\r\n");
    q += nai_copyconst(q, "Host: ");
    q += nai_copystr(q, &uri->hostname);
    q += nai_copyconst(q, "\r\n");
    q += nai_copyconst(q, "\r\n");
    p->out.size = (q - (char*)p->out.buf);


    /* get port */
    if (nai_str_len(&p->uri.portname) <= 0) {
        p->port = 80;
    } else {
        r = nai_atoi(&port, 
            nai_str(&p->uri.portname), nai_str_len(&p->uri.portname), 0);
        if (r < 0) {
            goto _end;
        };

        p->port = port;
    };

    /* get host */
    r = nai_str_dup(&p->host, 
        nai_str(&uri->hostname), nai_str_len(&uri->hostname), &p->pool);

_end:
    return r;

_fail:
    ec = nai_errno;

    OCSP_REQUEST_free(req);

    if (id) {
        OCSP_CERTID_free(id);
    };

    nai_errno = ec;
    goto _end;
};


static nai_int_t nai_ssl_ocsp_make_request(nai_ssl_ocsp_t* p)
{
    nai_int_t r;
    nai_int_t ec;
    nai_int_t len;
    char* str;
    nai_uri_t uri;
    STACK_OF(OPENSSL_STRING)* aia;


    if (nai_str_len(&p->uri.host) > 0) {
        r = nai_ssl_ocsp_make_request_impl(p, &p->uri);
        goto _end;
    };

    /* get ocsp server uri from certificate */
    aia = X509_get1_ocsp(p->cert);
    if (aia == NULL) {
        nai_ssl_clear_errno();
        r = 0;
        goto _end;
    }

#if OPENSSL_VERSION_NUMBER >= 0x10000000L
    str = sk_OPENSSL_STRING_value(aia, 0);
#else
    str = sk_value(aia, 0);
#endif

    len = nai_strlen(str);
    r = nai_uri_parse(&uri, str, len, NAI_URI_STRICT);
    if (r < 0) {
        goto _fail;
    };

    if (nai_str_len(&uri.scheme) != 4 || 
        nai_strncmp(nai_str(&uri.scheme), "http", 4) != 0) {
        nai_errno = EINVAL;
        r = -1;
        goto _fail;
    };

    r = nai_ssl_ocsp_make_request_impl(p, &uri);
    if (r < 0) {
        goto _fail;
    };

    X509_email_free(aia);
    r = 0;

_end:
    return r;

_fail:
    ec = nai_errno;
    X509_email_free(aia);
    nai_errno = ec;
    goto _end;
};


static nai_int_t nai_ssl_ocsp_make_key(nai_ssl_ocsp_t* p)
{
    nai_int_t r;
    uint8_t* ptr;
    X509_NAME* name;
    ASN1_INTEGER* serial;


    if (nai_str_len(&p->key) <= 0) {
        ptr = nai_palloc(&p->pool, 60);
        if (ptr == 0) {
            r = -1;
            goto _end;
        };

        nai_str_setm(&p->key, ptr, 60);
    };

    if (p->mkey) {
        r = 0;
        goto _end;
    };

    ptr = (uint8_t*)nai_str(&p->key);

    name = X509_get_subject_name(p->issuer);
    if (X509_NAME_digest(name, EVP_sha1(), ptr, 0) == 0) {
        nai_ssl_set_errno(EINVAL);
        r = -1;
        goto _end;
    };

    ptr += 20;

    if (X509_pubkey_digest(p->issuer, EVP_sha1(), ptr, 0) == 0) {
        nai_ssl_set_errno(EINVAL);
        r = -1;
        goto _end;
    };

    ptr += 20;

    serial = X509_get_serialNumber(p->cert);
    if (serial->length > 20) {
        nai_errno = ERANGE;
        r = -1;
        goto _end;
    };

    nai_memcpy(ptr, serial->data, serial->length);
    nai_memset(ptr + serial->length, 0, 20 - serial->length);
    p->mkey = 1;
    r = 0;

_end:
    return r;
};


static time_t nai_ssl_ocsp_time(ASN1_GENERALIZEDTIME *asn1time)
{
    nai_int_t r;
    nai_int_t ec;
    BIO* b;
    char* value;
    size_t len;
    time_t t;


    b = BIO_new(BIO_s_mem());
    if (b == 0) {
        nai_ssl_set_errno(ENOMEM);
        t = -1;
        goto _end;
    };

    ASN1_GENERALIZEDTIME_print(b, asn1time);
    len = BIO_get_mem_data(b, &value);

    r = nai_time_parse(&t, value, len);
    if (r < 0) {
        ec = nai_errno;
        t = -1;
    };

    BIO_free(b);

    if (r < 0) {
        nai_errno = ec;
    };

_end:
    return t;
};


static nai_int_t nai_ssl_ocsp_extract(
    nai_ssl_ocsp_t* p, const char* ptr, intptr_t len)
{
    nai_int_t r;
    nai_int_t ec;
    nai_int_t status;
    time_t expire;
    OCSP_CERTID* id;
    OCSP_RESPONSE* ocsp;
    OCSP_BASICRESP* basic;
    ASN1_GENERALIZEDTIME* thisupdate;
    ASN1_GENERALIZEDTIME* nextupdate;


    id = 0;
    ocsp = 0;
    basic = 0;

    /* check the response */
    ocsp = d2i_OCSP_RESPONSE(NULL, (const uint8_t**)&ptr, len);
    if (ocsp == 0) {
        nai_ssl_set_errno(ENOMEM);
        r = -1;
        goto _fail;
    };

    r = OCSP_response_status(ocsp);
    if (r != OCSP_RESPONSE_STATUS_SUCCESSFUL) {
        nai_ssl_clear_errno();
        r = 0;
        goto _fail;
    };

    basic = OCSP_response_get1_basic(ocsp);
    if (basic == 0) {
        nai_ssl_set_errno(ENOMEM);
        r = -1;
        goto _fail;
    };

    /* OCSP_TRUSTOTHER || OCSP_NOVERIFY */
    r = OCSP_basic_verify(basic, p->certs, p->store, 0);
    if (r < 0) {
        nai_ssl_set_errno(ENOMEM);
        r = -1;
        goto _fail;
    };
    if (r == 0) {
        nai_ssl_set_errno(EACCES);
        r = -1;
        goto _fail;
    };

    id = OCSP_cert_to_id(0, p->cert, p->issuer);
    if (id == 0) {
        nai_ssl_set_errno(ENOMEM);
        r = -1;
        goto _fail;
    };

    r = OCSP_resp_find_status(
        basic, id, &status, 0, 0, &thisupdate, &nextupdate);
    if (r == 0) {
        nai_ssl_clear_errno();
        goto _fail;
    };

    r = OCSP_check_validity(thisupdate, nextupdate, 300, -1);
    if (r == 0) {
        nai_ssl_clear_errno();
        goto _fail;
    };

    if (nextupdate) {
        expire = nai_ssl_ocsp_time(nextupdate);
        if (expire == (time_t)-1) {
            goto _fail;
        };
    } else {
        expire = NAI_INTPTR_T_MAX;
    };

    OCSP_CERTID_free(id);
    OCSP_BASICRESP_free(basic);
    OCSP_RESPONSE_free(ocsp);

    p->status = status;
    p->expire = expire;
    r = 0;


_end:
    return r;

_fail:
    if (r < 0) {
        ec = nai_errno;
    };

    if (id) {
        OCSP_CERTID_free(id);
    };

    if (basic) {
        OCSP_BASICRESP_free(basic);
    };

    if (ocsp) {
        OCSP_RESPONSE_free(ocsp);
    };

    if (r < 0) {
        nai_errno = ec;
    };

    goto _end;
};


static nai_int_t nai_ssl_ocsp_store_cache(nai_ssl_ocsp_t* p)
{
    nai_int_t r;
    nai_mem_t val;
    nai_ssl_store_t* st;


    st = nai_ssl_ctx_get_ocsp_store(p->ctx, 0);
    if (st == 0) {
        r = -1;
        goto _end;
    };

    r = nai_ssl_ocsp_make_key(p);
    if (r < 0) {
        goto _end;
    };

    nai_str_setm(&val, &p->status, sizeof(p->status));
    r = nai_ssl_store_set(st, &p->key, &val, p->expire);

_end:
    return r;
};


static nai_int_t nai_ssl_ocsp_fetch_stapling(nai_ssl_ocsp_t* p)
{
    nai_int_t r;
    nai_int_t len;
    uint8_t* ptr;


    if (p->next != 1) {
        r = 0;
        goto _end;
    };
    if (p->ssl == 0) {
        r = 0;
        goto _end;
    };

    len = SSL_get_tlsext_status_ocsp_resp(p->ssl->ssl, &ptr);
    nai_log_debug(NAI_LOG_CORE, 0, 
        "get ocsp response, ptr %p, len %d", ptr, len);
    if (ptr == 0) {
        r = 0;
        goto _end;
    };

    r = nai_ssl_ocsp_extract(p, (const char*)ptr, len);
    if (r >= 0) {
        nai_ssl_ocsp_store_cache(p);
    };


_end:
    return r;
};


static nai_int_t nai_ssl_ocsp_fetch_cache(nai_ssl_ocsp_t* p)
{
    nai_int_t r;
    nai_int_t ec;
    uint32_t status;
    nai_mem_t val;
    nai_ssl_store_t* st;


    st = nai_ssl_ctx_get_ocsp_store(p->ctx, 1);
    if (st == 0) {
        r = nai_ssl_ocsp_fetch_stapling(p);
        goto _end;
    };

    r = nai_ssl_ocsp_make_key(p);
    if (r < 0) {
        goto _end;
    };

    nai_str_setm(&val, &status, sizeof(status));
    r = nai_ssl_store_get(st, &p->key, &val);
    if (r < 0) {
        ec = nai_errno;
        if (ec == ENOENT) {
            r = nai_ssl_ocsp_fetch_stapling(p);
        };
        goto _end;
    };

    p->status = status;

_end:
    return r;
};


static nai_int_t nai_ssl_ocsp_select(
    nai_stream_t* s, const nai_dns_result_t* rs)
{
    intptr_t r;
    nai_int_t count;
    nai_ssl_ocsp_t* p;


    count = nai_dns_result_count(rs, -1);
    if (count <= 1) {
        r = 0;
        goto _end;
    };

    p = (nai_ssl_ocsp_t*)s;
    r = nai_dns_result_copy(rs, &p->dst.rs, 0, 0);
    if (r <= 0) {
        r = 0;
        goto _end;
    };

    if (r > p->dst.alloc) {
        if (r < 512) {
            r = 512;
        };
        p->dst.buf = nai_palloc(&p->pool, r);
        if (p->dst.buf == 0) {
            r = 0;
            goto _end;
        };

        p->dst.alloc = r;
    };

    r = nai_dns_result_copy(rs, &p->dst.rs, p->dst.buf, p->dst.alloc);
    if (r < 0) {
        r = 0;
        goto _end;
    };

    p->dst.count = count;
    r = 0;

_end:
    return (nai_int_t)r;
};


static nai_int_t nai_ssl_ocsp_connect(nai_ssl_ocsp_t* p, nai_int_t retry)
{
    nai_int_t r;
    nai_socknbuf_t nbuf;


    p->stat = NAI_SSL_OCSP_STAT_CONNECT;
    nai_stream_set_opt(&p->str, NAI_IO_RECVTIMEO, -1);
    nai_stream_set_opt(&p->str, NAI_IO_SENDTIMEO, 30*1000);

    if (retry) {
        if (p->dst.index >= p->dst.count) {
            nai_errno = ENOENT;
            r = -1;
            goto _end;
        };
        r = 0;
    } else {
        p->dst.index = 0;
        p->dst.count = 0;
        r = nai_stream_connect_host(&p->str, &p->conn, &p->host, p->port);
        if (r >= 0) {
            goto _end;
        };
    };

    while (p->dst.index < p->dst.count) {
        nbuf.len = sizeof(nbuf.storage);
        r = nai_dns_result_sockaddr(
            &p->dst.rs, p->dst.index, -1, &nbuf.addr, &nbuf.len);
        if (r < 0) {
            break;
        };

        p->dst.index ++;
        r = nai_stream_connect_addr(&p->str, &p->conn, &nbuf.addr, nbuf.len);
        if (r >= 0) {
            break;
        };
    };

_end:
    return r;
};


static nai_int_t nai_ssl_ocsp_next(nai_ssl_ocsp_t* p, nai_int_t cache_only);


static nai_int_t nai_ssl_ocsp_handle(nai_stream_t* s, nai_int_t events)
{
    intptr_t r;
    nai_int_t ec;
    nai_ssl_ocsp_t* p;


    p = (nai_ssl_ocsp_t*)s;
    if (events & NAI_EV_ERROR) {
        ec = nai_ev_error_code(events);
        r = -1;
        goto _next;
    };

    if (!(events & (NAI_EV_READ|NAI_EV_WRITE))) {
        r = 0;
        goto _end;
    };

    if (events & NAI_EV_TIMEOUT) {
        ec = ETIMEDOUT;
        r = -1;
        goto _next;
    };

    while (1) {
        switch (p->stat) {
        case NAI_SSL_OCSP_STAT_CONNECT:
            p->stat = NAI_SSL_OCSP_STAT_SEND;
            p->req.cur = p->out.buf;
            p->req.cend = p->out.buf + p->out.size;

            /* fallthrough */

        case NAI_SSL_OCSP_STAT_SEND:
            r = nai_stream_write(s, p->req.cur, p->req.cend - p->req.cur);
            if (r < 0) {
                ec = nai_errno;
                if (ec == NAI_EAGAIN || ec == NAI_EINPROGRESS) {
                    goto _end;
                };

                goto _next;
            };

            p->req.cur += r;
            if (p->req.cur < p->req.cend) {
                continue;
            };

            p->stat = NAI_SSL_OCSP_STAT_STATUS;
            p->req.parse.state = 0;
            p->req.start = p->in.buf;
            p->req.cur = p->in.buf;
            p->req.cend = p->in.buf;

            /* fallthrough */

        case NAI_SSL_OCSP_STAT_STATUS:
        case NAI_SSL_OCSP_STAT_HEADER:
        case NAI_SSL_OCSP_STAT_BODY:
            r = nai_stream_read(s, p->req.cend, 
                p->in.alloc - (p->req.cend - p->req.start));
            if (r < 0) {
                ec = nai_errno;
                if (ec == NAI_EAGAIN || ec == NAI_EINPROGRESS) {
                    goto _end;
                };

                goto _next;
            };
            if (r == 0) {
                if (p->stat != NAI_SSL_OCSP_STAT_BODY) {
                    ec = ECONNREFUSED;
                    r = -1;
                    goto _next;
                };

                p->stat = NAI_SSL_OCSP_STAT_FETCH;
                break;
            };

            p->req.cend += r;
            switch (p->stat) {
            case NAI_SSL_OCSP_STAT_STATUS:
                r = nai_sample_parse_status_line(&p->req);
                if (r < 0) {
                    ec = nai_sult_to_errno(r);
                    if (ec == NAI_EAGAIN) {
                        continue;
                    };

                    goto _next;
                };

                if (p->req.status != 200) {
                    ec = ECONNREFUSED;
                    goto _next;
                };

                p->stat = NAI_SSL_OCSP_STAT_HEADER;
                p->req.parse.state = 0;

                /* fallthrough */

            case NAI_SSL_OCSP_STAT_HEADER:
                r = nai_sample_parse_header_skip(&p->req);
                if (r < 0) {
                    ec = nai_sult_to_errno(r);
                    if (ec == NAI_EAGAIN) {
                        continue;
                    };

                    goto _next;
                };
                if (r != NAI_DONE) {
                    break;
                };


                p->stat = NAI_SSL_OCSP_STAT_BODY;

                /* fallthrough */

            case NAI_SSL_OCSP_STAT_BODY:
                continue;

            default:
                assert(0);
                break;
            };
            break;

        case NAI_SSL_OCSP_STAT_FETCH:
            r = nai_ssl_ocsp_extract(p, p->req.cur, p->req.cend - p->req.cur);
            if (r < 0) {
                ec = nai_errno;
                p->status = -ec;
                goto _fail;
            };

            goto _next;

        default:
            assert(0);
            ec = ECONNREFUSED;
            r = -1;
            goto _fail;
        };
    };


_end:
    return (nai_int_t)r;

_fail:
    /* stop */
    if (p->blocking) {
        nai_errno = ec;
        goto _end;
    };

    /* update stat */
    p->stat = NAI_SSL_OCSP_STAT_DONE;

    /* callback */
    if (p->cb) {
        p->cb(p, p->ud);
    };

    r = 0;
    goto _end;

_next:
    if (r >= 0) {
        if (p->status != V_OCSP_CERTSTATUS_UNKNOWN) {
            nai_ssl_ocsp_store_cache(p);
        };
    };


    if (p->blocking) {
        nai_stream_close(s);
        r = r < 0 ? 0 : 1;
        goto _end;
    };

    nai_stream_close(s);

    if (r < 0) {
        r = nai_ssl_ocsp_connect(p, 1);
        if (r < 0) {
            ec = nai_errno;
            goto _fail;
        };

    } else {
        if (p->status != V_OCSP_CERTSTATUS_GOOD) {
            ec = 0;
            goto _fail;
        };

        r = nai_ssl_ocsp_next(p, 0);

        if (p->stat == NAI_SSL_OCSP_STAT_DONE) {
            if (r < 0) {
                ec = nai_errno;
            } else {
                ec = 0;
            };
            goto _fail;
        };
    };

    goto _end;
};


static nai_int_t nai_ssl_ocsp_request(nai_ssl_ocsp_t* p)
{
    nai_int_t r;
    nai_int_t retry;
    nai_int_t blocking;


    r = nai_ssl_ocsp_make_request(p);
    if (r < 0) {
        goto _end;
    };

    nai_stream_set_cb(&p->str, nai_ssl_ocsp_handle);
    nai_stream_set_blocking(&p->str, p->blocking);
    nai_conn_attr_set_sel(&p->conn, nai_ssl_ocsp_select);

    retry = 0;
    blocking = p->blocking;
    for ( ; ; retry = 1) {
        r = nai_ssl_ocsp_connect(p, retry);
        if (r < 0) {
            break;
        };

        if (!blocking) {
            nai_errno = EAGAIN;
            r = -1;
            break;
        };

        r = nai_ssl_ocsp_handle(&p->str, NAI_EV_READ|NAI_EV_WRITE);
        if (r < 0) {
            break;
        };
        if (r > 0) {
            break;
        };
    };

_end:
    return r;
};


static nai_int_t nai_ssl_ocsp_next(nai_ssl_ocsp_t* p, nai_int_t cache_only)
{
    nai_int_t r;
    nai_int_t ec;
    nai_int_t n;


    n = sk_X509_num(p->certs);

    for (;;) {
        if (p->next == n - 1 || (
            p->mode == NAI_SSL_OCSP_LEAF && p->next == 1)) {
            r = 0;
            break;
        };

        /* get cert to validate */
        p->status = V_OCSP_CERTSTATUS_UNKNOWN;
        p->cert = sk_X509_value(p->certs, p->next);
        p->issuer = sk_X509_value(p->certs, p->next + 1);
        p->next ++;
        p->mkey = 0;

        /* lookup cert in cache */
        r = nai_ssl_ocsp_fetch_cache(p);
        if (r < 0) {
            ec = nai_errno;
            p->status = -ec;
            break;
        };

        /* check the status of cert */
        if (p->status == V_OCSP_CERTSTATUS_GOOD) {
            continue;
        };
        if (cache_only || p->status != V_OCSP_CERTSTATUS_UNKNOWN) {
            break;
        };

        /* do request */
        r = nai_ssl_ocsp_request(p);
        if (r < 0) {
            ec = nai_errno;
            if (ec == EAGAIN) {
                goto _end;
            };

            break;
        };

        if (p->status != V_OCSP_CERTSTATUS_GOOD) {
            break;
        };
    };

    p->stat = NAI_SSL_OCSP_STAT_DONE;

_end:
    return r;
};


static nai_int_t nai_ssl_ocsp_result(nai_ssl_ocsp_t* p)
{
    nai_int_t r;


    switch (p->status) {
    case V_OCSP_CERTSTATUS_GOOD:
        r = 0;
        break;
    case V_OCSP_CERTSTATUS_REVOKED:
        nai_errno = EACCES;
        r = -1;
        break;
    case V_OCSP_CERTSTATUS_UNKNOWN:
        nai_errno = ENOENT;
        r = -1;
        break;
    default:
        assert(p->status < 0);
        nai_errno = -p->status;
        r = -1;
        break;
    };

    return r;
};


nai_ssl_ocsp_t* nai_ssl_ocsp_new()
{
    nai_pool_t pool;
    nai_ssl_ocsp_t* p;


    nai_pool_init(&pool, 4096*4);

    p = (nai_ssl_ocsp_t*)nai_palloc(&pool, sizeof(*p));
    if (p == 0) {
        goto _end;
    };

    nai_ssl_ocsp_init(p);
    nai_pool_exchange(&p->pool, &pool);

_end:
    return p;
};


nai_int_t nai_ssl_ocsp_open(
    nai_ssl_ocsp_t* p, nai_ssl_ctx_t* ctx, 
    nai_ssl_t* ssl, STACK_OF(X509)* certs, nai_int_t mode)
{
    nai_int_t r;


    if (certs == 0) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    if (ctx) {
        p->store = SSL_CTX_get_cert_store(ctx->ctx);
        if (p->store == 0) {
            nai_ssl_set_errno(EINVAL);
            r = -1;
            goto _end;
        };
    };

    p->ctx = ctx;
    p->ssl = ssl;
    p->certs = certs;
    p->next = 0;
    p->mode = mode;
    p->stat = NAI_SSL_OCSP_STAT_NONE;
    p->status = V_OCSP_CERTSTATUS_UNKNOWN;
    if (p->conn.agent == 0 || p->conn.agent->loop == 0) {
        p->blocking = 1;
    };

    r = 0;

_end:
    return r;
};


nai_int_t nai_ssl_ocsp_close(nai_ssl_ocsp_t* p)
{
    nai_int_t r;


    if (p->stat == NAI_SSL_OCSP_STAT_NONE) {
        r = 0;
        goto _end;
    };

    r = nai_stream_close(&p->str);
    if (r < 0) {
        goto _end;
    };

    if (p->certs != 0) {
        sk_X509_pop_free(p->certs, X509_free);
        p->certs = 0;
    };

    p->ctx = 0;
    p->ssl = 0;
    p->stat = NAI_SSL_OCSP_STAT_NONE;
    p->status = V_OCSP_CERTSTATUS_UNKNOWN;
    p->expire = 0;
    p->store = 0;
    p->cert = 0;
    p->issuer = 0;
    p->next = 0;
    p->mkey = 0;
    p->blocking = 0;
    p->in.buf = 0;
    p->in.size = 0;
    p->in.alloc = 0;
    p->out.buf = 0;
    p->out.size = 0;
    p->out.alloc = 0;
    p->dst.buf = 0;
    p->dst.index = 0;
    p->dst.count = 0;

    nai_str_setn(&p->host);
    nai_pool_close(&p->pool);
    r = 0;


_end:
    return r;
};


nai_int_t nai_ssl_ocsp_verify(nai_ssl_ocsp_t* p)
{
    nai_int_t r;


    switch (p->stat) {
    case NAI_SSL_OCSP_STAT_NONE:
        r = nai_ssl_ocsp_next(p, 0);
        if (r < 0) {
            break;
        };

        /* fallthrough */

    case NAI_SSL_OCSP_STAT_DONE:
        r = nai_ssl_ocsp_result(p);
        break;

    default:
        nai_errno = EAGAIN;
        r = -1;
        break;
    };

    return r;
};


nai_int_t nai_ssl_ocsp_verify_cache(
    nai_ssl_ctx_t* ctx, 
    nai_ssl_t* ssl, STACK_OF(X509)* certs, nai_int_t mode)
{
    nai_int_t r;
    nai_ssl_ocsp_t oc;
    char buf[60];


    nai_ssl_ocsp_init(&oc);
    nai_str_setm(&oc.key, buf, sizeof(buf));

    r = nai_ssl_ocsp_open(&oc, ctx, ssl, certs, mode);
    if (r < 0) {
        goto _end;
    };

    r = nai_ssl_ocsp_next(&oc, 1);
    if (r < 0) {
        goto _end;
    };

    r = nai_ssl_ocsp_result(&oc);

_end:
    return r;
};



#define NAI_SSL_OCSP_SULT_PENDING   1
#define NAI_SSL_OCSP_SULT_OK        2
#define NAI_SSL_OCSP_SULT_REVOKED   3
#define NAI_SSL_OCSP_SULT_FAIL      4
#define NAI_SSL_OCSP_SULT_UNKNOWN   5


static nai_int_t nai_ssl_finish_ocsp(nai_ssl_ocsp_t* p, void* ud)
{
    nai_int_t r;
    nai_int_t ec;
    nai_ssl_t* s;
    nai_ssl_ops_t* ops;


    s = (nai_ssl_t*)ud;
    r = nai_ssl_ocsp_verify(p);
    if (r >= 0) {
        s->ocsp = NAI_SSL_OCSP_SULT_OK;
    } else {
        ec = nai_errno;
        switch (ec) {
        case EACCES:
            s->ocsp = NAI_SSL_OCSP_SULT_REVOKED;
            break;
        case ENOENT:
            s->ocsp = NAI_SSL_OCSP_SULT_UNKNOWN;
            break;
        case EPERM:
        default:
            s->ocsp = NAI_SSL_OCSP_SULT_FAIL;
            break;
        };
    };

    nai_ssl_ocsp_close(p);
    SSL_set_ex_data(s->ssl, nai_ssl.ocsp, 0);


    ops = s->ops;
    if (ops && ops->emit) {
        ops->emit(s->ud, NAI_EV_READ|NAI_EV_WRITE);
    };

    r = 0;

    return r;
};


nai_int_t nai_ssl_prepare_ocsp(nai_ssl_t* s)
{
    nai_int_t r;
    nai_ssl_ocsp_conf_t* cf;


    cf = nai_ssl_ctx_get_ocsp_conf(s->ctx, 1);
    if (cf == 0 || cf->mode == NAI_SSL_OCSP_OFF) {
        r = 0;
        goto _end;
    };


    SSL_set_tlsext_status_type(s->ssl, TLSEXT_STATUSTYPE_ocsp);
    r = 0;

_end:
    return r;
};


nai_int_t nai_ssl_verify_ocsp(nai_ssl_t* s)
{
    nai_int_t r;
    nai_int_t ec;
    nai_ssl_ocsp_t* p;
    nai_ssl_ocsp_conf_t* cf;
    X509* cert;
    X509_STORE* store;
    X509_STORE_CTX* store_ctx;
    STACK_OF(X509)* certs;
    STACK_OF(X509)* chain;


    if (s->ocsp != 0) {
        switch (s->ocsp) {
        case NAI_SSL_OCSP_SULT_PENDING:
            nai_errno = EAGAIN;
            r = -1;
            break;
        case NAI_SSL_OCSP_SULT_OK:
            r = 0;
            break;
        case NAI_SSL_OCSP_SULT_REVOKED:
            nai_errno = EACCES;
            r = -1;
            break;
        case NAI_SSL_OCSP_SULT_FAIL:
            nai_errno = EACCES;
            r = -1;
            break;
        case NAI_SSL_OCSP_SULT_UNKNOWN:
            nai_errno = ENOENT;
            r = -1;
            break;
        default:
            assert(0);
            nai_errno = EACCES;
            r = -1;
            break;
        };

        goto _end;
    };


    cf = nai_ssl_ctx_get_ocsp_conf(s->ctx, 1);
    if (cf == 0 || cf->mode == NAI_SSL_OCSP_OFF) {
        r = 0;
        goto _end;
    };

    r = SSL_get_verify_result(s->ssl);
    if (r != X509_V_OK) {

        certs = SSL_get_peer_cert_chain(s->ssl);
        if (certs) {
            certs = X509_chain_up_ref(certs);
        };

    } else {

#if (OPENSSL_VERSION_NUMBER >= 0x10100000L && !defined LIBRESSL_VERSION_NUMBER)
        certs = SSL_get0_verified_chain(s->ssl);
        if (certs) {
            certs = X509_chain_up_ref(certs);
        };
#else
        certs = 0;
#endif

        if (certs == 0) {
            store_ctx = 0;
            cert = SSL_get_peer_certificate(s->ssl);
            if (cert == 0) {
                r = 0;
                goto _end;
            };

            store = SSL_CTX_get_cert_store(s->ctx->ctx);
            if (store == 0) {
                nai_ssl_set_errno(EINVAL);
                ec = nai_errno;
                r = -1;
                goto _fail;
            };

            store_ctx = X509_STORE_CTX_new();
            if (store_ctx == 0) {
                nai_ssl_set_errno(ENOMEM);
                ec = nai_errno;
                r = -1;
                goto _fail;
            };

            chain = SSL_get_peer_cert_chain(s->ssl);

            if (X509_STORE_CTX_init(store_ctx, store, cert, chain) == 0) {
                nai_ssl_set_errno(ENOMEM);
                ec = nai_errno;
                r = -1;
                goto _fail;
            };

            r = X509_verify_cert(store_ctx);
            if (r <= 0) {
                nai_ssl_set_errno(EACCES);
                ec = nai_errno;
                r = -1;
                goto _fail;
            };

            certs = X509_STORE_CTX_get1_chain(store_ctx);
            if (certs == 0) {
                nai_ssl_set_errno(EACCES);
                ec = nai_errno;
                r = -1;
                goto _fail;
            };

            X509_STORE_CTX_free(store_ctx);
            X509_free(cert);
        };
    };

    store_ctx = 0;
    cert = 0;

    r = nai_ssl_ocsp_verify_cache(s->ctx, s, certs, cf->mode);
    if (r >= 0) {
        sk_X509_pop_free(certs, X509_free);
        s->ocsp = NAI_SSL_OCSP_SULT_OK;
        goto _end;
    };

    ec = nai_errno;
    if (ec != ENOENT) {
        goto _fail;
    };

    p = nai_ssl_ocsp_new();
    if (p == 0) {
        ec = nai_errno;
        goto _fail;
    };

    SSL_set_ex_data(s->ssl, nai_ssl.ocsp, p);
    nai_ssl_ocsp_set_uri(p, &cf->uri);
    nai_ssl_ocsp_set_conn(p, &cf->conn);
    nai_ssl_ocsp_set_cb(p, nai_ssl_finish_ocsp, s);
    if (s->ops && s->ops->get_opt) {
        nai_ssl_ocsp_set_blocking(p, 
            s->ops->get_opt(s->ud, NAI_SSL_OPT_BLOCKING));
    };

    nai_ssl_ocsp_open(p, s->ctx, s, certs, cf->mode);
    certs = 0;

    r = nai_ssl_ocsp_verify(p);
    if (r >= 0) {
        nai_ssl_ocsp_close(p);
        SSL_set_ex_data(s->ssl, nai_ssl.ocsp, 0);

        s->ocsp = NAI_SSL_OCSP_SULT_OK;
    } else {
        ec = nai_errno;
        if (ec != EAGAIN) {
            nai_ssl_ocsp_close(p);
            goto _fail;
        };

        s->ocsp = NAI_SSL_OCSP_SULT_PENDING;
    };


_end:
    return r;

_fail:
    if (certs) {
        sk_X509_pop_free(certs, X509_free);
    };

    if (cert) {
        X509_free(cert);
    };
    if (store_ctx) {
        X509_STORE_CTX_free(store_ctx);
    };

    switch (ec) {
    case EACCES:
        s->ocsp = NAI_SSL_OCSP_SULT_REVOKED;
        break;
    case ENOENT:
        s->ocsp = NAI_SSL_OCSP_SULT_UNKNOWN;
        break;
    case EPERM:
    default:
        s->ocsp = NAI_SSL_OCSP_SULT_FAIL;
        break;
    };
    nai_errno = ec;
    goto _end;
};


nai_int_t nai_ssl_cleanup_ocsp(nai_ssl_t* s)
{
    nai_int_t r;
    nai_ssl_ocsp_t* p;


    p = (nai_ssl_ocsp_t*)SSL_get_ex_data(s->ssl, nai_ssl.ocsp);
    if (p != 0) {
        nai_ssl_ocsp_close(p);
        SSL_set_ex_data(s->ssl, nai_ssl.ocsp, 0);
    };

    r = 0;

    return r;
};



nai_int_t nai_ssl_ctx_conn_attr(nai_ssl_ctx_t* s, const nai_conn_attr_t* attr)
{
    nai_int_t r;
    nai_ssl_ocsp_conf_t* c;


    c = nai_ssl_ctx_get_ocsp_conf(s, 0);
    if (c == 0) {
        r = -1;
        goto _end;
    };

    c->conn = *attr;
    r = 0;

_end:
    return r;
};


nai_int_t nai_ssl_ctx_ocsp(
    nai_ssl_ctx_t* s, nai_int_t mode, const nai_str_t* ocsp)
{
    nai_int_t r;
    nai_str_t str;
    nai_uri_t uri;
    nai_ssl_ocsp_conf_t* c;


    c = nai_ssl_ctx_get_ocsp_conf(s, 0);
    if (c == 0) {
        r = -1;
        goto _end;
    };

    if (ocsp && nai_str_len(ocsp) > 0) {
        r = nai_str_dup(&str, nai_str(ocsp), nai_str_len(ocsp), &c->pool);
        if (r < 0) {
            goto _end;
        };

        r = nai_uri_parse(&uri, 
            nai_str(&str), nai_str_len(&str), NAI_URI_STRICT);
        if (r < 0) {
            goto _fail;
        };
        if (nai_str_len(&uri.scheme) != 4 || 
            nai_strncmp(nai_str(&uri.scheme), "http", 4) != 0) {
            nai_errno = EINVAL;
            r = -1;
            goto _fail;
        };

        c->uri = uri;

    } else {
        nai_uri_init(&c->uri);
    };

    c->mode = mode;
    r = 0;

_end:
    return r;

_fail:
    nai_pool_free_last(&c->pool, 
        nai_str(&str), nai_str_len(&str)+1);
    goto _end;
};


nai_int_t nai_ssl_ctx_ocsp_store(nai_ssl_ctx_t* s, nai_ssl_store_t* store)
{
    nai_int_t r;
    nai_ssl_ocsp_conf_t* c;


    c = nai_ssl_ctx_get_ocsp_conf(s, 0);
    if (c == 0) {
        r = -1;
        goto _end;
    };

    c->store = store;
    r = 0;

_end:
    return r;
};


nai_int_t nai_ssl_ctx_cleanup_ocsp(nai_ssl_ctx_t* s)
{
    nai_int_t r;
    nai_ssl_ocsp_conf_t* c;


    c = (nai_ssl_ocsp_conf_t*)SSL_CTX_get_ex_data(s->ctx, nai_ssl.ocsp_conf);
    if (c != 0) {
        nai_pool_close(&c->pool);
        SSL_CTX_set_ex_data(s->ctx, nai_ssl.ocsp_conf, 0);
    };

    r = 0;

    return r;
};


#else


nai_ssl_ocsp_t* nai_ssl_ocsp_new()
{
    nai_pool_t pool;
    nai_ssl_ocsp_t* p;


    nai_pool_init(&pool, 512);

    p = (nai_ssl_ocsp_t*)nai_palloc(&pool, sizeof(*p));
    if (p == 0) {
        goto _end;
    };

    nai_ssl_ocsp_init(p);
    nai_pool_exchange(&p->pool, &pool);

_end:
    return p;
};


nai_int_t nai_ssl_ocsp_open(
    nai_ssl_ocsp_t* p, nai_ssl_ctx_t* ctx, 
    nai_ssl_t* ssl, STACK_OF(X509)* certs, nai_int_t mode)
{
    nai_int_t r;


    (void)p;
    (void)ctx;
    (void)ssl;
    (void)certs;
    (void)mode;

    nai_errno = ENOTSUP;
    r = -1;

    return r;
};


nai_int_t nai_ssl_ocsp_close(nai_ssl_ocsp_t* p)
{
    nai_int_t r;


    if (p->stat == NAI_SSL_OCSP_STAT_NONE) {
        r = 0;
        goto _end;
    };

    r = nai_stream_close(&p->str);
    if (r < 0) {
        goto _end;
    };

    if (p->certs != 0) {
        sk_X509_pop_free(p->certs, X509_free);
        p->certs = 0;
    };

    p->ctx = 0;
    p->ssl = 0;
    p->stat = NAI_SSL_OCSP_STAT_NONE;
    p->status = V_OCSP_CERTSTATUS_UNKNOWN;
    p->expire = 0;
    p->cert = 0;
    p->issuer = 0;
    p->next = 0;
    p->mkey = 0;
    p->blocking = 0;
    p->in.buf = 0;
    p->in.size = 0;
    p->in.alloc = 0;
    p->out.buf = 0;
    p->out.size = 0;
    p->out.alloc = 0;
    p->dst.buf = 0;
    p->dst.index = 0;
    p->dst.count = 0;

    nai_str_setn(&p->host);
    nai_pool_close(&p->pool);
    r = 0;


_end:
    return r;
};


nai_int_t nai_ssl_ocsp_verify(nai_ssl_ocsp_t* p)
{
    nai_int_t r;


    (void)p;

    nai_errno = ENOTSUP;
    r = -1;

    return r;
};


nai_int_t nai_ssl_ocsp_verify_cache(
    nai_ssl_ctx_t* ctx, 
    nai_ssl_t* ssl, STACK_OF(X509)* certs, nai_int_t mode)
{
    nai_int_t r;


    (void)ctx;
    (void)ssl;
    (void)certs;
    (void)mode;

    nai_errno = ENOTSUP;
    r = -1;

    return r;
};



nai_int_t nai_ssl_ctx_conn_attr(
    nai_ssl_ctx_t* s, const nai_conn_attr_t* attr)
{
    nai_int_t r;


    (void)s;
    (void)attr;

    nai_errno = ENOTSUP;
    r = -1;

    return r;
};


nai_int_t nai_ssl_ctx_ocsp(
    nai_ssl_ctx_t* s, nai_int_t mode, const nai_str_t* ocsp)
{
    nai_int_t r;


    (void)s;
    (void)mode;
    (void)ocsp;

    nai_errno = ENOTSUP;
    r = -1;

    return r;
};


nai_int_t nai_ssl_ctx_ocsp_store(nai_ssl_ctx_t* s, nai_ssl_store_t* store)
{
    nai_int_t r;


    (void)s;
    (void)store;

    nai_errno = ENOTSUP;
    r = -1;

    return r;
};


nai_int_t nai_ssl_ctx_cleanup_ocsp(nai_ssl_ctx_t* c)
{
    (void)s;
    return 0;
};



nai_int_t nai_ssl_prepare_ocsp(nai_ssl_t* s)
{
    (void)s;
    return 0;
};


nai_int_t nai_ssl_verify_ocsp(nai_ssl_t* s)
{
    (void)s;
    return 0;
};


nai_int_t nai_ssl_cleanup_ocsp(nai_ssl_t* s)
{
    (void)s;
    return 0;
};



#endif


#endif

