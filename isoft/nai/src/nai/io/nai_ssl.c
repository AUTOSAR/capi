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
/// @file       nai_ssl.c
/// @brief      
/// @details
/// @date       2021-03-01
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai/io/nai_ssl_ctx.h"


#if (NAI_HAVE_SSL)


#include "nai/io/nai_ssl_ocsp.h"
#include "nai/io/nai_io.h"
#include "nai/io/nai_buf.h"
#include "nai/io/nai_event.h"
#include "nai/io/nai_async_load.h"
#include "nai/os/nai_aio.h"
#include "nai/os/nai_tlocal.h"
#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_log.h"
#include "nai/runtime/nai_util.h"

#if (NAI_HAVE_OPENSSL) || (NAI_HAVE_LIBRESSL)
#include <openssl/bio.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#elif (NAI_HAVE_WOLFSSL)
#include <wolfssl/openssl/bio.h>
#include <wolfssl/openssl/x509.h>
#include <wolfssl/openssl/x509v3.h>
#define DTLS_get_data_mtu wolfSSL_GetMaxOutputSize
#endif


#if (OPENSSL_VERSION_NUMBER < 0x10100000L) || \
    (defined(LIBRESSL_VERSION_NUMBER) && LIBRESSL_VERSION_NUMBER < 0x20700000L)


#define BIO_set_init(b, val) (b)->init = (val)
#define BIO_set_data(b, val) (b)->ptr = (val)
#define BIO_set_shutdown(b, val) (b)->shutdown = (val)
#define BIO_get_init(b) (b)->init
#define BIO_get_data(b) (b)->ptr
#define BIO_get_shutdown(b) (b)->shutdown
#define BIO_up_ref(b) ((b)->references ++)


#endif



//////////////////////////////////////////////////////////////////////////////
// ssl



#ifndef NAI_SSL_MIN_WBUF
#define NAI_SSL_MIN_WBUF        (4096-48)
#endif
#ifndef NAI_SSL_MAX_WBUF
#define NAI_SSL_MAX_WBUF        (32*1024)
#endif


#ifndef NAI_BUF_READSIZE
#define NAI_BUF_READSIZE        (2*4096)
#endif

#ifndef NAI_SSL_READSIZE
#define NAI_SSL_READSIZE        (512*1024)
#endif
#ifndef NAI_SSL_WRITESIZE
#define NAI_SSL_WRITESIZE       (512*1024)
#endif



typedef struct nai_ssl_wbuf_s {
    uint8_t* buf;
    uint32_t alloc;
    uint32_t rpos;
    uint32_t wpos;
} nai_ssl_wbuf_t;



static void* nai_ssl_alloc(nai_ssl_t* s, size_t size)
{
    void* r;
    nai_ssl_ops_t* ops;


    ops = s->ops;
    if (ops && ops->realloc) {
        r = ops->realloc(s->ud, 0, size);
    } else {
        r = nai_malloc(size);
    };

    return r;
};


static void nai_ssl_free(nai_ssl_t* s, void* ptr)
{
    nai_ssl_ops_t* ops;


    ops = s->ops;
    if (ops && ops->realloc) {
        ops->realloc(s->ud, ptr, 0);
    } else {
        nai_free(ptr);
    };

    return;
};


static intptr_t nai_ssl_flush_buffer(nai_ssl_t* s)
{
    intptr_t r;
    nai_ssl_wbuf_t* wb;


    assert(s->wbuf);
    assert(s->buffered);
    wb = (nai_ssl_wbuf_t*)s->wbuf;

    s->buffered = 0;
    r = nai_ssl_write(s, wb->buf + wb->rpos, wb->wpos - wb->rpos);
    if (r < 0) {
        s->buffered = 1;
        goto _end;
    };

    wb->rpos += r;
    if (wb->rpos >= wb->wpos) {
        if (s->tls && s->wsize != wb->alloc) {
            nai_ssl_free(s, wb);
            s->wbuf = 0;
        } else {
            wb->rpos = 0;
            wb->wpos = 0;
        };
    } else {
        s->buffered = 1;
    };


_end:
    return r;
};


#ifdef SSL_READ_EARLY_DATA_SUCCESS


static nai_int_t nai_ssl_early_handshake(nai_ssl_t* s)
{
    nai_int_t r;
    nai_int_t ec;
    uint8_t byte;
    size_t readbytes;


    nai_ssl_clear_errno();

    r = SSL_read_early_data(s->ssl, &byte, 1, &readbytes);

    if (r == SSL_READ_EARLY_DATA_FINISH) {
        s->early_hand = 0;
        r = nai_ssl_handshake(s);
        goto _end;
    };

    if (r == SSL_READ_EARLY_DATA_SUCCESS) {
        s->early_hand = 0;
        s->early_read = 1;
        s->early_write = 1;
        if (readbytes) {
            s->early_byte = byte;
            s->early_byte_get = 1;
        };

        r = nai_ssl_verify_ocsp(s);
        if (r < 0) {
            goto _end;
        };

        s->handshaked = 1;
        r = 0;
        goto _end;
    };

    ec = SSL_get_error(s->ssl, r);
    switch (ec) {
    case SSL_ERROR_WANT_READ:
        nai_errno = NAI_EAGAIN;
        break;
    case SSL_ERROR_WANT_WRITE:
        nai_errno = NAI_EAGAIN;
        break;
    case SSL_ERROR_SYSCALL:
        s->error = 1;
        break;
    case SSL_ERROR_ZERO_RETURN:
        nai_errno = ECONNABORTED;
        s->error = 1;
        break;
    default:
        nai_ssl_set_errno(0);
        s->error = 1;
        break;
    };

    r = -1;

_end:
    return r;
};


static intptr_t nai_ssl_early_read(nai_ssl_t* s, void* buf, size_t size)
{
    nai_int_t r;
    nai_int_t ec;
    size_t bytes;
    intptr_t total;
    uint8_t* data;


    nai_ssl_clear_errno();

    if (size <= 0) {
        r = 0;
        goto _end;
    };

    total = 0;
    data = (uint8_t*)buf;
    if (s->early_byte_get) {
        s->early_byte_get = 0;
        data[0] = s->early_byte;

        total += 1;
        data += 1;
        size -= 1;
        if (size <= 0) {
            r = total;
            goto _end;
        };
    };

    for (;;) {

        r = SSL_read_early_data(s->ssl, data, size, &bytes);

#ifndef SSL_OP_NO_RENEGOTIATION
        if (s->renegotiation) {
            nai_ssl_clear_errno();
            nai_errno = ECONNABORTED;
            r = -1;
            goto _end;
        };
#endif

        if (r == SSL_READ_EARLY_DATA_FINISH) {
            s->early_read = 0;
            s->early_write = 0;
            if (total == 0) {
                r = nai_ssl_read(s, data, size);
                goto _end;
            };
            break;
        };

        if (r == SSL_READ_EARLY_DATA_SUCCESS) {
            total += bytes;
            data += bytes;
            size -= bytes;
            if (size > 0) {
                continue;
            };
            break;
        };

        ec = SSL_get_error(s->ssl, r);
        switch (ec) {
        case SSL_ERROR_WANT_READ:
            goto _again;
        case SSL_ERROR_WANT_WRITE:
            if (s->ops->want) {
                s->ops->want(s->ud, NAI_EV_WRITE);
            };
            goto _again;
        case SSL_ERROR_ZERO_RETURN:
            break;
        case SSL_ERROR_SYSCALL:
            s->error = 1;
            r = -1;
            goto _end;
        default:
            nai_ssl_set_errno(0);
            s->error = 1;
            r = -1;
            goto _end;
        };

        break;
    };

    r = total;

_end:
    return r;

_again:
    if (total > 0) {
        r = total;
    } else {
        nai_errno = NAI_EAGAIN;
        r = -1;
    };
    goto _end;
};


static intptr_t nai_ssl_early_write(nai_ssl_t* s, const void* buf, size_t size)
{
    nai_int_t r;
    nai_int_t ec;
    size_t bytes;
    intptr_t total;
    uint8_t* data;


    nai_ssl_clear_errno();

    data = (uint8_t*)buf;
    total = 0;
    for (;;) {

        r = SSL_write_early_data(s->ssl, data, size, &bytes);

        if (r > 0) {
            total += r;
            data += r;
            size -= r;
            if (size > 0) {
                continue;
            };
            break;
        };

        ec = SSL_get_error(s->ssl, r);
        switch (ec) {
        case SSL_ERROR_WANT_READ:
            if (s->ops->want) {
                s->ops->want(s->ud, NAI_EV_READ);
            };
            goto _again;
        case SSL_ERROR_WANT_WRITE:
            goto _again;
        case SSL_ERROR_ZERO_RETURN:
            nai_errno = ECONNABORTED;
            s->error = 1;
            r = -1;
            break;
        case SSL_ERROR_SYSCALL:
            s->error = 1;
            r = -1;
            goto _end;
        default:
            nai_ssl_set_errno(0);
            s->error = 1;
            r = -1;
            goto _end;
        };

        break;
    };

    r = total;

_end:
    return r;

_again:
    if (total > 0) {
        r = total;
    } else {
        nai_errno = NAI_EAGAIN;
        r = -1;
    };
    goto _end;
};


#endif



static nai_int_t nai_ssl_open_intern(nai_ssl_t* s, 
    const nai_ssl_ctx_t* ssl, nai_int_t connect, BIO* bio)
{
    nai_int_t r;


    s->ssl = SSL_new(ssl->ctx);
    if (s->ssl == 0) {
        nai_ssl_set_errno(ENOMEM);
        nai_log_alert(NAI_LOG_CORE, 
            nai_errno, "SSL_new(0x%p) failed", ssl->ctx);
        r = -1;
        goto _end;
    };

    BIO_set_data(bio, s);
    BIO_set_init(bio, 1);
    BIO_set_shutdown(bio, 0);
    SSL_set_bio(s->ssl, bio, bio);
    SSL_set_ex_data(s->ssl, nai_ssl.connection, s);
    bio = 0;

    s->ctx = (nai_ssl_ctx_t*)ssl;
    s->tls = ssl->tls;
    s->connect = !!connect;

    if (s->ktls != (uint32_t)3) {
        nai_ssl_enable_ktls(s, s->ktls);
    };

    nai_ssl_prepare_ocsp(s);

    if (connect) {
        SSL_set_connect_state(s->ssl);

#ifdef SSL_READ_EARLY_DATA_SUCCESS
        if (s->tls && SSL_CTX_get_max_early_data(ssl->ctx)) {
            s->early_hand = 0;
            s->early_write = 1;
        };
#endif
    } else {
        SSL_set_accept_state(s->ssl);

#ifdef SSL_OP_NO_RENEGOTIATION
        SSL_set_options(s->ssl, SSL_OP_NO_RENEGOTIATION);
#endif
#ifdef SSL_READ_EARLY_DATA_SUCCESS
        if (s->tls && SSL_CTX_get_max_early_data(ssl->ctx)) {
            s->early_hand = 1;
        };
#endif
    };

    r = 0;

_end:
    return r;
};


nai_int_t nai_ssl_support_certs_cb()
{
#ifdef SSL_R_CERT_CB_ERROR
    return 1;
#else
    return 0;
#endif
};


nai_int_t nai_ssl_support_ktls()
{
#if (NAI_HAVE_KTLS)
    return 1;
#else
    return 0;
#endif
};


nai_int_t nai_ssl_open(nai_ssl_t* s, 
    const nai_ssl_ctx_t* ssl, nai_int_t connect)
{
    nai_int_t r;
    BIO* bio;


    if (s->ssl) {
        nai_errno = EALREADY;
        r = -1;
        goto _end;
    };

    bio = BIO_new((ssl->tls || connect) ? nai_ssl.meth : nai_ssl.meth_dgram);
    if (bio == 0) {
        nai_ssl_set_errno(ENOMEM);
        nai_log_alert(NAI_LOG_CORE, 
            nai_errno, "BIO_new('nai_bio') failed");
        r = -1;
        goto _end;
    };

    r = nai_ssl_open_intern(s, ssl, connect, bio);
    if (r < 0) {
        BIO_free(bio);
    };

_end:
    return r;
};


nai_int_t nai_ssl_certificate(nai_ssl_t* s, 
    const nai_str_t* cert, const nai_str_t* key, 
    const nai_str_t* passwords, nai_int_t count)
{
    nai_int_t r;
#if 0
    nai_int_t n;
    X509* temp;
#endif
    nai_ssl_cert_t c = {0};
    nai_ssl_pkey_t k = {0};


    r = nai_ssl_load_certificate(s->ctx, &c, cert);
    if (r < 0) {
        nai_log_error(NAI_LOG_CORE, 
            nai_errno, "nai_ssl_load_certificate() failed");
        goto _end;
    };

    r = nai_ssl_load_certificate_key(s->ctx, &k, key, passwords, count);
    if (r < 0) {
        nai_log_error(NAI_LOG_CORE, 
            nai_errno, "nai_ssl_load_certificate_key() failed");
        goto _end;
    };

    if (SSL_use_certificate(s->ssl, c.cert) == 0) {
        nai_ssl_set_errno(0);
        nai_log_alert(NAI_LOG_CORE, 
            nai_errno, "SSL_use_certificate(\"%.*s\") failed", 
            nai_str_len(cert), nai_str(cert));
        r = -1;
        goto _end;
    }


#ifdef SSL_set0_chain

    if (SSL_set0_chain(s->ssl, c.chain) == 0) {
        nai_ssl_set_errno(0);
        nai_log_alert(NAI_LOG_CORE, 
            nai_errno, "SSL_set0_chain(\"%.*s\") failed", 
            nai_str_len(cert), nai_str(cert));
        r = -1;
        goto _end;
    };

    c.chain = 0;

#endif

#if 0
    /* SSL_set0_chain() is only available in OpenSSL 1.0.2+ */
    n = sk_X509_num(c.chain);

    while (n --) {
        temp = sk_X509_shift(c.chain);
        if (SSL_add_extra_chain_cert(s->ssl, temp) == 0) {
            nai_ssl_set_errno(0);
            nai_log_alert(NAI_LOG_CORE, 
                nai_errno, "SSL_add_extra_chain_cert(\"%.*s\") failed", 
                nai_str_len(cert), nai_str(cert));
            r = -1;
            goto _end;
        };
    };

    sk_X509_free(c.chain);
    c.chain = 0;

#endif

    if (SSL_use_PrivateKey(s->ssl, k.pkey) == 0) {
        nai_ssl_set_errno(0);
        nai_log_alert(NAI_LOG_CORE, 
            nai_errno, "SSL_use_PrivateKey(\"%.*s\") failed", 
            nai_str_len(key), nai_str(key));
        r = -1;
        goto _end;
    };


    r = 0;

_end:
    if (k.pkey) {
        EVP_PKEY_free(k.pkey);
    };
    if (c.cert) {
        X509_free(c.cert);
    };
    if (c.chain) {
        sk_X509_pop_free(c.chain, X509_free);
    };

    return r;
};


nai_int_t nai_ssl_set_ctx(nai_ssl_t* s, const nai_ssl_ctx_t* ssl)
{
    nai_int_t r;


    if (ssl->ctx == 0) {
        r = 0;
        goto _end;
    };
    if (ssl == s->ctx) {
        r = 0;
        goto _end;
    };

    if (SSL_set_SSL_CTX(s->ssl, ssl->ctx) == NULL) {
        nai_ssl_set_errno(0);
        nai_log_alert(NAI_LOG_CORE, 
            nai_errno, "SSL_set_SSL_CTX() failed");
        r = -1;
        goto _end;
    };

    /*
     * SSL_set_SSL_CTX() only changes certs as of 1.0.0d
     * adjust other things we care about
     */

    SSL_set_verify(s->ssl, 
        SSL_CTX_get_verify_mode(ssl->ctx),
        SSL_CTX_get_verify_callback(ssl->ctx));

    SSL_set_verify_depth(s->ssl, SSL_CTX_get_verify_depth(ssl->ctx));

#if OPENSSL_VERSION_NUMBER >= 0x009080dfL
    /* only in 0.9.8m+ */
    SSL_clear_options(s->ssl, 
        SSL_get_options(s->ssl) &
        ~SSL_CTX_get_options(ssl->ctx));

#endif

    SSL_set_options(s->ssl, SSL_CTX_get_options(ssl->ctx));

#ifdef SSL_OP_NO_RENEGOTIATION
    SSL_set_options(s->ssl, SSL_OP_NO_RENEGOTIATION);
#endif

    r = 0;

_end:
    return r;
};


nai_int_t nai_ssl_set_host(nai_ssl_t* s, const nai_str_t* name)
{
    nai_int_t r;


    r = SSL_set_tlsext_host_name(s->ssl, nai_str(name));
    if (r == 0) {
        nai_log_error(NAI_LOG_CORE, 0, 
            "SSL_set_tlsext_host_name('%s') failed", nai_str(name));
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    r = 0;

_end:
    return r;
};


nai_int_t nai_ssl_handshake(nai_ssl_t* s)
{
    nai_int_t r;
    nai_int_t ec;
    nai_ssl_ops_t* ops;


    ops = s->ops;
    if (ops == 0) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

#ifdef SSL_READ_EARLY_DATA_SUCCESS
    if (s->early_hand) {
        r = nai_ssl_early_handshake(s);
        goto _end;
    };
#endif

    if (s->ocsp) {
        goto _ocsp;
    };


    nai_ssl_clear_errno();

    s->timeoset = 0;

    r = SSL_do_handshake(s->ssl);
    if (r != 1) {

        ec = SSL_get_error(s->ssl, r);
        switch (ec) {
        case SSL_ERROR_WANT_READ:
            goto _again;
        case SSL_ERROR_WANT_WRITE:
            goto _again;
        case SSL_ERROR_SYSCALL:
            s->error = 1;
            break;
        case SSL_ERROR_ZERO_RETURN:
            nai_errno = ECONNABORTED;
            s->error = 1;
            break;
        default:
            nai_ssl_set_errno(0);
            s->error = 1;
            break;
        };

        r = -1;
        goto _end;
    };


_ocsp:
    r = nai_ssl_verify_ocsp(s);
    if (r < 0) {
        goto _end;
    };

    s->handshaked = 1;
    r = 0;

_end:
    return r;

_again:
    nai_errno = NAI_EAGAIN;
    r = -1;
    goto _end;
};


#ifndef X509_CHECK_FLAG_ALWAYS_CHECK_SUBJECT


static nai_int_t nai_ssl_equal_name(
    const nai_mem_t *name, ASN1_STRING *pattern)
{
    nai_int_t r;
    const char* s;
    const char* p;
    const char* send;
    size_t slen;
    size_t plen;


    s = nai_str(name);
    slen = nai_str_len(name);

    p = (char*)ASN1_STRING_data(pattern);
    plen = ASN1_STRING_length(pattern);


    if (slen == plen && 
        nai_strncasecmp(s, p, plen) == 0) {
        r = 1;
        goto _end;
    };

    if (plen > 2 && p[0] == '*' && p[1] == '.') {
        plen -= 1;
        p += 1;

        send = s + slen;

        s = nai_strnchr(s, slen, '.');
        if (s == 0) {
            r = 0;
            goto _end;
        }

        slen = send - s;

        if (plen == slen && 
            nai_strncasecmp(s, p, plen) == 0) {
            r = 1;
            goto _end;
        };
    };

    r = 0;

_end:
    return r;
};


#endif


static nai_int_t nai_ssl_verify_host(
    nai_ssl_t* s, const nai_str_t* name, nai_int_t opt)
{
    nai_int_t r;
    nai_int_t ec;
    X509* cert;


    r = SSL_get_verify_result(s->ssl);
    if (r != X509_V_OK) {
        nai_log_debug(NAI_LOG_CORE, 0, 
            "SSL_get_verify_result failed, error %s", 
            X509_verify_cert_error_string(r));
        nai_errno = EACCES;
        r = -1;
        goto _end;
    };

    cert = SSL_get_peer_certificate(s->ssl);
    if (cert == 0) {
        nai_errno = ENOENT;
        r = -1;
        goto _end;
    };


    if (nai_str_len(name) <= 0) {
        if (opt == NAI_SSL_VERIFY_ON) {
            goto _fail;
        } else {
            goto _ok;
        };
    };


#ifdef X509_CHECK_FLAG_ALWAYS_CHECK_SUBJECT

    /* X509_check_host() is only available in OpenSSL 1.0.2+ */
    if (X509_check_host(cert, 
        (char*)nai_str(name), nai_str_len(name), 0, NULL) != 1) {
        goto _fail;
    }; 

    goto _ok;

#else
    {
        nai_int_t n, count;
        X509_NAME* sname;
        ASN1_STRING* str;
        X509_NAME_ENTRY* ent;
        GENERAL_NAME* altname;
        STACK_OF(GENERAL_NAME)* altnames;

        /*
         * As per RFC6125 and RFC2818, we check subjectAltName extension,
         * and if it's not present - commonName in Subject is checked.
         */
        altnames = X509_get_ext_d2i(cert, NID_subject_alt_name, NULL, NULL);

        if (altnames) {

            count = sk_GENERAL_NAME_num(altnames);
            for (n = 0; n < count; n ++) {

                altname = sk_GENERAL_NAME_value(altnames, n);
                if (altname->type != GEN_DNS) {
                    continue;
                };

                str = altname->d.dNSName;

                if (nai_ssl_equal_name(name, str)) {
                    GENERAL_NAMES_free(altnames);
                    goto _ok;
                };
            };

            GENERAL_NAMES_free(altnames);
            ec = EACCES;
            goto _fail;
        };

        /*
         * If there is no subjectAltName extension, check commonName
         * in Subject.  While RFC2818 requires to only check "most specific"
         * CN, both Apache and OpenSSL check all CNs, and so do we.
         */
        sname = X509_get_subject_name(cert);
        if (sname == 0) {
            goto _fail;
        };

        n = -1;
        for (;;) {
            n = X509_NAME_get_index_by_NID(sname, NID_commonName, n);
            if (n < 0) {
                goto _fail;
            };

            ent = X509_NAME_get_entry(sname, n);
            str = X509_NAME_ENTRY_get_data(ent);

            if (nai_ssl_equal_name(name, str)) {
                goto _ok;
            };
        };
    };

#endif

_ok:
    X509_free(cert);
    r = 0;

_end:
    return r;

_fail:
    X509_free(cert);
    ec = EACCES;
    nai_errno = ec;
    r = -1;
    goto _end;
};


#define nai_ssl_verify_error_optional(n) (                  \
    n == X509_V_ERR_DEPTH_ZERO_SELF_SIGNED_CERT ||          \
    n == X509_V_ERR_SELF_SIGNED_CERT_IN_CHAIN ||            \
    n == X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT_LOCALLY ||    \
    n == X509_V_ERR_CERT_UNTRUSTED ||                       \
    n == X509_V_ERR_UNABLE_TO_VERIFY_LEAF_SIGNATURE         \
)                                                           \


nai_int_t nai_ssl_verify(nai_ssl_t* s, nai_int_t opt)
{
    nai_int_t r;
    X509* cert;
    nai_str_t name;
    const char* host;


    if (!opt) {
        r = 0;
        goto _end;
    };

    if (s->connect) {
        host = SSL_get_servername(s->ssl, TLSEXT_NAMETYPE_host_name);
        if (host) {
            nai_str_sets(&name, host);
        } else {
            nai_str_setn(&name);
        };

        r = nai_ssl_verify_host(s, &name, opt);
        goto _end;
    };

    r = SSL_get_verify_result(s->ssl);
    if (r != X509_V_OK && (
        opt != NAI_SSL_VERIFY_NO_CA || 
        !nai_ssl_verify_error_optional(r))) {

        nai_log_debug(NAI_LOG_CORE, 0, 
            "SSL_get_verify_result failed, error %s", 
            X509_verify_cert_error_string(r));

        SSL_CTX_remove_session(
            s->ctx->ctx, SSL_get0_session(s->ssl));

        nai_errno = EACCES;
        r = -1;
        goto _end;
    };

    if (opt == NAI_SSL_VERIFY_ON) {
        cert = SSL_get_peer_certificate(s->ssl);
        if (cert == 0) {

            SSL_CTX_remove_session(
                s->ctx->ctx, SSL_get0_session(s->ssl));

            nai_errno = ENOENT;
            r = -1;
            goto _end;
        };

        X509_free(cert);
    };

    r = 0;

_end:
    return r;
};


nai_int_t nai_ssl_shutdown(nai_ssl_t* s, nai_int_t how)
{
    nai_int_t r;
    nai_int_t ec;
    nai_int_t mode;
    nai_int_t retry;


    switch (how) {
    case NAI_SOCK_RD:
        mode = SSL_RECEIVED_SHUTDOWN;
        break;
    case NAI_SOCK_WR:
        mode = SSL_SENT_SHUTDOWN;
        break;
    case NAI_SOCK_RW:
        mode = SSL_RECEIVED_SHUTDOWN|SSL_SENT_SHUTDOWN;
        break;
    default:
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    if (SSL_in_init(s->ssl)) {
        r = 0;
        goto _end;
    };

    if (s->error) {
        SSL_set_quiet_shutdown(s->ssl, 1);
    };

    SSL_set_shutdown(s->ssl, mode);

    nai_ssl_clear_errno();

    retry = 2;
    for (;;) {

        r = SSL_shutdown(s->ssl);
        if (r == 1) {
            break;
        };

        if (r == 0 && retry -- > 1) {
            continue;
        };

        ec = SSL_get_error(s->ssl, r);
        switch (ec) {
        case SSL_ERROR_WANT_READ:
            nai_errno = NAI_EAGAIN;
            break;
        case SSL_ERROR_WANT_WRITE:
            nai_errno = NAI_EAGAIN;
            break;
        case SSL_ERROR_ZERO_RETURN:
            nai_errno = ECONNABORTED;
            s->error = 1;
            break;
        case SSL_ERROR_SYSCALL:
            s->error = 1;
            break;
        default:
            nai_ssl_set_errno(0);
            s->error = 1;
            break;
        };

        r = -1;
        goto _end;
    };

    r = 0;

_end:
    return r;
};


nai_int_t nai_ssl_close(nai_ssl_t* s)
{
    nai_int_t r;
    uint8_t* m;
    nai_async_load_t* l;


    if (s->ssl) {
        m = (uint8_t*)SSL_get_ex_data(s->ssl, nai_ssl.extend);
        if (m != 0) {
            nai_free(m);
        };

        nai_ssl_cleanup_ocsp(s);

        SSL_free(s->ssl);
        s->ssl = 0;
    };

    if (s->wbuf) {
        nai_ssl_free(s, s->wbuf);
        s->wbuf = 0;
    };

    if (s->load) {
        l = (nai_async_load_t*)s->load;
        nai_async_load_close(l);
        s->load = 0;
        s->loadstat = NAI_ASYNC_LOAD_UNINIT;
    };

    s->ctx = 0;
    s->flags = 0;
    s->ktls = 3;
    r = 0;

    return r;
};


nai_int_t nai_ssl_enable_wbuf(nai_ssl_t* s, size_t size)
{
    nai_int_t r;
    size_t nsize;
    nai_ssl_wbuf_t* wb;


    if (size <= 0) {
        nsize = 0;
    } else if (size > NAI_SSL_MAX_WBUF) {
        nsize = NAI_SSL_MAX_WBUF;
    } else if (size < NAI_SSL_MIN_WBUF) {
        nsize = NAI_SSL_MIN_WBUF;
    } else {
        nsize = (uint16_t)size;
    };

    wb = (nai_ssl_wbuf_t*)s->wbuf;
    if (wb && !s->buffered && s->tls) {
        if (nsize != (size_t)wb->alloc) {
            assert(wb->rpos == wb->wpos);
            nai_ssl_free(s, wb);
            s->wbuf = 0;
        };
    };

    if (nsize > 0) {
        s->buffer = 1;
        s->wsize = nsize;
        r = 1;
    } else {
        s->buffer = 0;
        s->wsize = 0;
        r = 0;
    };


    return r;
};


nai_int_t nai_ssl_enable_ktls(nai_ssl_t* s, nai_int_t on)
{
    nai_int_t r;


#if (NAI_HAVE_KTLS)
    if (s->ssl == 0) {
        s->ktls = !!on;
        r = !!on;
    } else {
        if (on) {
            SSL_set_options(s->ssl, SSL_OP_ENABLE_KTLS);
            r = 1;
        } else {
            SSL_clear_options(s->ssl, SSL_OP_ENABLE_KTLS);
            r = 0;
        };

        s->ktls = 3;
    };
#else
    (void)s;
    (void)on;

    r = 0;
#endif

    return r;
};


nai_int_t nai_ssl_enable_load(nai_ssl_t* s, nai_int_t on)
{
    nai_int_t r;
    nai_async_load_t* l;


    if (s->loadfile == !!on) {
        r = s->loadfile;
        goto _end;
    };

    if (s->loadfile && 
        s->loadstat == NAI_ASYNC_LOAD_DONE) {
        l = (nai_async_load_t*)s->load;
        if (l != 0) {
            nai_async_load_close(l);
            s->load = 0;
            s->loadstat = NAI_ASYNC_LOAD_UNINIT;
        };
    };

    s->loadfile = !!on;
    r = s->loadfile;

_end:
    return r;
};


intptr_t nai_ssl_read(nai_ssl_t* s, void* buf, size_t size)
{
    nai_int_t r;
    nai_int_t ec;
    intptr_t total;
    uint8_t* data;
    nai_ssl_ops_t* ops;


    ops = s->ops;
    if (ops == 0) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

#ifdef SSL_READ_EARLY_DATA_SUCCESS
    if (s->early_read) {
        r = nai_ssl_early_read(s, buf, size);
        goto _end;
    };
#endif

    nai_ssl_clear_errno();

    data = (uint8_t*)buf;
    total = 0;
    for (;;) {

        r = SSL_read(s->ssl, data, size);

#ifndef SSL_OP_NO_RENEGOTIATION
        if (s->renegotiation) {
            nai_ssl_clear_errno();
            nai_errno = ECONNABORTED;
            r = -1;
            goto _end;
        };
#endif

        if (r == 0) {
            break;
        };
        if (r > 0) {

            total += r;
            data += r;
            size -= r;
            if (size > 0) {
                continue;
            };
            break;

        };

        ec = SSL_get_error(s->ssl, r);
        switch (ec) {
        case SSL_ERROR_WANT_READ:
            goto _again;
        case SSL_ERROR_WANT_WRITE:
            if (ops->want) {
                ops->want(s->ud, NAI_EV_WRITE);
            };
            goto _again;
        case SSL_ERROR_ZERO_RETURN:
            break;
        case SSL_ERROR_SYSCALL:
            s->error = 1;
            r = -1;
            goto _end;
        default:
            nai_ssl_set_errno(0);
            s->error = 1;
            r = -1;
            goto _end;
        };

        break;
    };

    r = total;

_end:
    return r;

_again:
    if (total > 0) {
        r = total;
    } else {
        nai_errno = NAI_EAGAIN;
        r = -1;
    };
    goto _end;
};


intptr_t nai_ssl_readv(nai_ssl_t* s, nai_bufvec_t* v, nai_int_t count)
{
    intptr_t r;
    intptr_t total;
    nai_int_t ec;
    nai_int_t n;
    nai_int_t mtu;
    char* buf;
    nai_tlocal_t* t;


    if (count > 1 && !s->tls) {

        mtu = nai_dtls_get_mtu(s);
        if (mtu <= (intptr_t)v[0].len) {
            r = nai_ssl_read(s, v[0].buf, v[0].len);
            goto _end;
        };

        t = nai_tlocal_get();
        if (t == 0) {
            r = -1;
            goto _end;
        };

        /**
         * lock the memory, becuase nai_thread_local_mem may be called 
         * inside nai_ssl_read again
         */
        buf = nai_tlocal_mem(t, mtu, 1);
        if (buf == 0) {
            r = -1;
            goto _end;
        };

        r = nai_ssl_read(s, buf, mtu);
        if (r < 0) {
            ec = nai_errno;
            nai_tlocal_mem_unlock(t, buf);
            nai_errno = ec;
            goto _end;
        };

        n = 0;
        total = 0;
        while ((r - total) > 0 && n < count) {
            if ((r - total) <= (intptr_t)v[n].len) {
                nai_memcpy(v[n].buf, buf + total, (r - total));
                total = r;
                break;
            } else {
                nai_memcpy(v[n].buf, buf + total, v[n].len);
                total += v[n].len;
            };
        };
        if (total < r) {
            /* truncated */
        };

        nai_tlocal_mem_unlock(t, buf);

    } else {

        total = 0;
        n = 0;
        for ( ; n < count; n ++) {
            if (v[n].len == 0) {
                continue;
            };

            r = nai_ssl_read(s, v[n].buf, v[n].len);
            if (r < 0) {
                if (total > 0 && nai_errno == NAI_EAGAIN) {
                    break;
                };

                r = -1;
                goto _end;
            };
            if (r == 0) {
                break;
            };

            total += r;
            if ((size_t)r < v[n].len) {
                break;
            };
        };
    };

    r = total;

_end:
    return r;
};


intptr_t nai_ssl_readq(nai_ssl_t* s, nai_buflist_t* in, size_t limit)
{
    intptr_t r;
    nai_int_t message;
    size_t total;
    size_t max;
    size_t read;
    nai_buflist_t full;
    nai_bufarray_t ba;
    nai_bufvec_t b[NAI_BUFV_MAX];


    nai_list_init(&full.ent);

    message = !s->tls;
    max = NAI_SSL_WRITESIZE;
    if (message) {
        max = limit;
    };

    total = 0;
    while (!nai_buflist_is_empty(in)) {
        read = max;
        if (read > limit) {
            read = limit;
        };

        r = nai_buflist_to_wbufarray(
            in, &ba, b, nai_countof(b), read, message);
        if (r < 0) {
            goto _end;
        };
        if (r == 0) {
            break;
        };

        r = nai_ssl_readv(s, ba.v, ba.count);
        if (r == 0) {
            break;
        };
        if (r < 0) {
            if (total > 0 && nai_errno == NAI_EAGAIN) {
                break;
            };
            goto _end;
        };


        total += r;
        nai_buflist_fulled(in, &full, r);

        if (message) {
            break;
        };

        limit -= r;
        if (limit <= 0) {
            break;
        };
    };

    r = total;

_end:
    nai_buflist_add_head(in, &full);
    return r;
};



intptr_t nai_ssl_write(nai_ssl_t* s, const void* buf, size_t size)
{
    nai_int_t r;
    nai_int_t ec;
    intptr_t total;
    uint8_t* data;
    uint32_t rpos;
    uint32_t wpos;
    nai_ssl_wbuf_t* wb;
    nai_ssl_ops_t* ops;


    ops = s->ops;
    if (ops == 0) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    if (s->buffered) {
        nai_log_debug(NAI_LOG_CORE, 0, 
            "the buffer or content was switched when the previous buffered "
            "content was not sent completely");

        if (s->tls) {
            wb = (nai_ssl_wbuf_t*)s->wbuf;
            assert(wb != 0);

            rpos = wb->rpos;
            wpos = wb->wpos;
            if (size <= wb->alloc - rpos) {
                nai_memcpy(wb->buf + wpos, 
                    buf + (wpos - rpos), size - (wpos - rpos));
                wb->wpos = rpos + size;
            };
        };

        r = nai_ssl_flush_buffer(s);
        goto _end;
    };

    switch (s->loadstat) {
    case NAI_ASYNC_LOAD_UNINIT:
        break;
    case NAI_ASYNC_LOAD_DONE:
        nai_ssl_load_unhits(s);
        break;
    default:
        /* has content in loader, user should keep call writeq */
        /* discard loaded content */
        r = nai_ssl_load_discard(s);
        if (r < 0) {
            goto _end;
        };
        break;
    };

#ifdef SSL_READ_EARLY_DATA_SUCCESS
    if (s->early_write) {
        r = nai_ssl_early_write(s, buf, size);
        goto _end;
    };
#endif

    nai_ssl_clear_errno();

    data = (uint8_t*)buf;
    total = 0;
    for (;;) {

        r = SSL_write(s->ssl, data, size);

        if (r > 0) {
            total += r;
            data += r;
            size -= r;
            if (size > 0) {
                continue;
            };
            break;
        };

        ec = SSL_get_error(s->ssl, r);
        switch (ec) {
        case SSL_ERROR_WANT_READ:
            if (ops->want) {
                ops->want(s->ud, NAI_EV_READ);
            };
            goto _again;
        case SSL_ERROR_WANT_WRITE:
            goto _again;
        case SSL_ERROR_ZERO_RETURN:
            nai_errno = ECONNABORTED;
            s->error = 1;
            r = -1;
            break;
        case SSL_ERROR_SYSCALL:
            s->error = 1;
            r = -1;
            goto _end;
        default:
            nai_ssl_set_errno(0);
            s->error = 1;
            r = -1;
            goto _end;
        };

        break;
    };

    r = total;

_end:
    return r;

_again:
    if (total > 0) {
        r = total;
    } else {
        nai_errno = NAI_EAGAIN;
        r = -1;
    };
    goto _end;
};


intptr_t nai_ssl_writev(nai_ssl_t* s, const nai_bufvec_t* v, nai_int_t count)
{
    intptr_t r;
    intptr_t total;
    nai_int_t n;
    uint32_t alloc;
    uint32_t rpos;
    uint32_t wpos;
    nai_ssl_wbuf_t* wb;
    nai_ssl_ops_t* ops;


    ops = s->ops;
    if (ops == 0) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    if (s->buffered && !s->tls) {
        r = nai_ssl_flush_buffer(s);
        goto _end;
    };

    switch (s->loadstat) {
    case NAI_ASYNC_LOAD_UNINIT:
        break;
    case NAI_ASYNC_LOAD_DONE:
        nai_ssl_load_unhits(s);
        break;
    default:
        /* has content in loader, user should keep call writeq */
        /* discard loaded content */
        r = nai_ssl_load_discard(s);
        if (r < 0) {
            goto _end;
        };
        break;
    };

    if (s->buffered || (
        !s->early_write && 
        count > 1 && (s->buffer || !s->tls))) {

        wb = (nai_ssl_wbuf_t*)s->wbuf;
        if (wb == 0) {
            if (s->tls) {
                alloc = s->wsize;
            } else {
                alloc = nai_dtls_get_mtu(s);
            };

            wb = nai_ssl_alloc(s, sizeof(*wb) + alloc);
            if (wb == 0) {
                r = -1;
                goto _end;
            };

            wb->buf = (uint8_t*)(wb + 1);
            wb->alloc = alloc;
            wb->rpos = 0;
            wb->wpos = 0;
        };

        n = 0;
        rpos = wb->rpos;
        wpos = wb->wpos;

        /* skip bytes of copied data */
        if (rpos < wpos) {
            for ( ; n < count; n ++) {
                if (v[n].len < wpos - rpos) {
                    rpos += v[n].len;
                } else if (v[n].len < wb->alloc - rpos) {
                    nai_memcpy(wb->buf + wpos, 
                        v[n].buf + (wpos - rpos), v[n].len - (wpos - rpos));
                    wpos += v[n].len - (wpos - rpos);
                    n ++;
                    break;
                } else {
                    nai_memcpy(wb->buf + wpos, 
                        v[n].buf + (wpos - rpos), wb->alloc - wpos);
                    wpos = wb->alloc;
                    break;
                };
            };
        };
        /* write new data to buffer */
        if (wpos < wb->alloc) {
            for ( ; n < count; n ++) {
                if (v[n].len < wb->alloc - wpos) {
                    nai_memcpy(wb->buf + wpos, v[n].buf, v[n].len);
                    wpos += v[n].len;
                } else {
                    if (!s->tls) {
                        nai_errno = EMSGSIZE;
                        r = -1;
                        goto _end;
                    };
                    if (wpos - rpos <= 0) {
                        /* buffer is large enough, direct send */
                        r = nai_ssl_write(s, v[n].buf, v[n].len);
                        goto _end;
                    };

                    nai_memcpy(wb->buf + wpos, v[n].buf, wb->alloc - wpos);
                    wpos = wb->alloc;
                    break;
                };
            };
        };

        wb->wpos = wpos;
        if (wpos > rpos) {
            s->buffered = 1;
            r = nai_ssl_flush_buffer(s);
        } else {
            r = 0;
        };

    } else {

        total = 0;
        n = 0;
        for ( ; n < count; n ++) {
            if (v[n].len == 0) {
                continue;
            };

            r = nai_ssl_write(s, v[n].buf, v[n].len);
            if (r < 0) {
                if (total > 0 && nai_errno == NAI_EAGAIN) {
                    break;
                };

                r = -1;
                goto _end;
            };

            total += r;
            if ((size_t)r < v[n].len) {
                break;
            };
        };

        r = total;
    };


_end:
    return r;
};


static intptr_t nai_ssl_to_rbufarray(
    nai_buflist_t* l, nai_bufarray_t* a, 
    nai_bufvec_t* v, nai_int_t count, size_t limit)
{
    intptr_t r;
    size_t size;
    size_t total = 0;
    nai_off64_t offs;
    nai_buf_t* b;
    nai_list_entry_t* e;


    a->v = v;
    a->count = 0;
    if (limit <= 0) {
        r = 0;
        goto _end;
    };

    e = l->ent.next;
    for ( ; e != &l->ent; e = e->next) {
        b = (nai_buf_t*)e;
        size = nai_buf_size(b);
        if (size <= 0) {
            continue;
        };

        if (nai_buf_in_file(b)) {
            break;
        };

        if (size > limit) {
            size = limit;
        };

        if (!nai_buf_in_memory(b)) {
            if (size > NAI_BUF_READSIZE) {
                if (!nai_buf_in_file(b) || !b->directio) {
                    size = NAI_BUF_READSIZE;
                } else {
                    offs = nai_buf_offset(b) + NAI_BUF_READSIZE;
                    offs = nai_aligndown(offs, nai_sector_size);
                    size = (size_t)(offs - nai_buf_offset(b));
                };
            };
            b = nai_buf_cut(l->pool, b, size, 1);
            if (b == 0) {
                r = -1;
                goto _end;
            } else if (nai_buf_size(b) < size) {
                size = nai_buf_size(b);
            };
            e = (nai_list_entry_t*)b;
        };

        v->buf = b->start;
        v->len = size;
        v ++;
        total += size;

        a->count ++;
        if (a->count >= count) {
            break;
        };

        limit -= size;
        if (limit <= 0) {
            break;
        };
    };

    r = total;

_end:
    return r;
};


extern nai_int_t nai_bio_in_ktls_send(BIO* b);


intptr_t nai_ssl_writeq(nai_ssl_t* s, nai_buflist_t* out, size_t limit)
{
    intptr_t r;
    nai_int_t ec;
    nai_int_t message;
    nai_int_t ktls;
    size_t total;
    size_t max;
    size_t write;
    nai_ssl_ops_t* ops;
    nai_buf_t* f;
    nai_bufarray_t ba;
    nai_bufvec_t b[NAI_BUFV_MAX];


    ops = s->ops;
    if (ops == 0) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

#if NAI_SIZEOF_SIZE_T == 4
    /* prevent overflow */
    if (limit >= NAI_INT_T_MAX) {
        limit = NAI_INT_T_MAX;
    };
#endif

    message = !s->tls;
    max = NAI_SSL_WRITESIZE;
    if (message) {
        max = limit;
    };

    ktls = nai_bio_in_ktls_send(SSL_get_wbio(s->ssl));
    if (ktls) {
        if (ops->get_opt && 
            ops->sendfile) {
            ktls = ops->get_opt(s->ud, NAI_SSL_OPT_SENDFILE);
        } else {
            ktls = 0;
        };
    };

    total = 0;
    if (ktls) {

        while (!nai_buflist_is_empty(out)) {
            write = max;
            if (write > limit) {
                write = limit;
            };

            r = nai_ssl_to_rbufarray(
                out, &ba, b, nai_countof(b), limit);
            if (r < 0) {
                goto _end;
            };
            if (r > 0) {
                r = nai_ssl_writev(s, ba.v, ba.count);
            } else {
                if (nai_buflist_is_empty(out)) {
                    break;
                };

                if (s->buffered) {
                    nai_log_debug(NAI_LOG_CORE, 0, 
                        "the buffer or content was switched when the previous "
                        "buffered content was not sent completely");

                    r = nai_ssl_flush_buffer(s);
                } else {
                    nai_buflist_rcommit(out, 0);
                    f = (nai_buf_t*)out->ent.next;
                    assert(nai_buf_in_file(f));

                    /* direct sendfile to kernel */
                    r = ops->sendfile(s->ud, 
                        nai_buf_fd(f), nai_buf_size(f), nai_buf_offset(f));
                };
            };
            if (r < 0) {
                ec = nai_errno;
                if (ec == NAI_EINPROGRESS) {
                    ec = NAI_EAGAIN;
                    if (total <= 0) {
                        nai_errno = ec;
                    };
                };
                if (total > 0 && ec == NAI_EAGAIN) {
                    break;
                };
                goto _end;
            };

            total += r;
            r = nai_buflist_rcommit(out, r);
            if (r < 0) {
                goto _end;
            };

            limit -= r;
            if (limit <= 0) {
                break;
            };
        };

    } else {

        switch (s->loadstat) {
        case NAI_ASYNC_LOAD_UNINIT:
            if (!s->loadfile) {
                break;
            };
            if (!nai_ssl_load_available(s)) {
                break;
            };

            /* fallthrough */

        case NAI_ASYNC_LOAD_DONE:
            /* fallthrough */

        default:
            r = nai_ssl_load_and_flush(s, out, limit);
            goto _end;
        };


        while (!nai_buflist_is_empty(out)) {
            write = max;
            if (write > limit) {
                write = limit;
            };

            r = nai_buflist_to_rbufarray(
                out, &ba, b, nai_countof(b), limit, message);
            if (r < 0) {
                goto _end;
            };
            if (r == 0) {
                nai_buflist_rcommit(out, 0);
                break;
            };

            r = nai_ssl_writev(s, ba.v, ba.count);
            if (r < 0) {
                if (total > 0 && nai_errno == NAI_EAGAIN) {
                    break;
                };
                goto _end;
            };

            total += r;
            r = nai_buflist_rcommit(out, r);
            if (r < 0) {
                goto _end;
            };

            if (message) {
                break;
            };

            limit -= r;
            if (limit <= 0) {
                break;
            };
        };
    };

    r = total;

_end:
    return r;
};


intptr_t nai_ssl_sendfile(nai_ssl_t* s, 
    nai_fd_t fd, size_t size, nai_off64_t offset)
{
    intptr_t r;
    nai_int_t ec;
    nai_int_t ktls;
    nai_ssl_ops_t* ops;
    nai_buf_t* b;
    nai_buflist_t list;
    struct {
        nai_buf_t b;
        nai_buf_extra_t e;
    } buf;


    ops = s->ops;
    if (ops == 0) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    if (s->buffered) {
        nai_log_debug(NAI_LOG_CORE, 0, 
            "the buffer or content was switched when the previous "
            "buffered content was not sent completely");

        r = nai_ssl_flush_buffer(s);
        goto _end;
    };

    /* checking */
    ktls = nai_bio_in_ktls_send(SSL_get_wbio(s->ssl));
    if (ktls) {
        if (ops->get_opt && 
            ops->sendfile) {
            ktls = ops->get_opt(s->ud, NAI_SSL_OPT_SENDFILE);
        } else {
            ktls = 0;
        };
    };

    if (ktls) {
        /* direct sendfile to kernel */
        r = ops->sendfile(s->ud, fd, size, offset);
        if (r < 0) {
            if (nai_errno == NAI_EINPROGRESS) {
                nai_errno = NAI_EAGAIN;
            };
        };
    } else {
        if (!s->loadfile) {
            nai_errno = ENOTSUP;
            r = -1;
            goto _end;
        };

        /* make temprary file buffer */
        buf.b.offset = offset;
        buf.b.size = size;
        buf.b.total = size;
        buf.b.flags = 0;
        buf.b.type = NAI_BUF_FILE;
        buf.b.typeref = NAI_BUF_FILE;
        buf.b.extra = 1;
        buf.b.refcount = 2;
        buf.b.ref.obj = 0;
        buf.b.pool = 0;
        buf.e.fd = fd;
        buf.e.ops = 0;

        nai_buflist_init(&list, 0);
        nai_buflist_insert_tail(&list, &buf.b);

        r = nai_ssl_writeq(s, &list, size);

        /* check bufflist */
        if (!nai_buflist_is_empty(&list)) {
            b = (nai_buf_t*)list.ent.next;

            /* test memory leak */
            assert(nai_buf_in_file(b));


            if (nai_buf_in_memory(b)) {
                nai_log_debug(NAI_LOG_CORE, 0, 
                    "the iobase is not support sendfile, "
                    "we will discard the unsent content which is "
                    "transported in memory");

                if (r < 0) {
                    ec = nai_errno;
                };

                nai_buflist_close(&list);

                if (r < 0) {
                    nai_errno = ec;
                };
            };
        };
    };


_end:
    return r;
};



//////////////////////////////////////////////////////////////////////////////
// dtls server



nai_int_t nai_dtls_listen(
    nai_ssl_t* s, nai_sockaddr_t* name, nai_int_t* namelen)
{
    nai_int_t r;
    nai_int_t ec;
    nai_int_t mtu;
    nai_int_t async_read;
    BIO* b;
    nai_ssl_dgram_t* d;
    nai_ssl_ops_t* ops;
    nai_bufvec_t v;


    ops = s->ops;
    if (ops == 0) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    if (s->listened) {
        r = nai_dtls_drop(s);
        if (r < 0) {
            goto _end;
        };
    };

    /* get dgram context */
    d = (nai_ssl_dgram_t*)SSL_get_ex_data(s->ssl, nai_ssl.extend);
    if (d == 0) {
        b = SSL_get_rbio(s->ssl);
        if (ops->get_opt) {
            async_read = ops->get_opt(s->ud, NAI_SSL_OPT_ASYNCREAD);
        } else {
            async_read = 0;
        };

        if (async_read) {
            mtu = BIO_ctrl(b, BIO_CTRL_DGRAM_QUERY_MTU, 0, 0);
        } else {
            mtu = 0;
        };
        d = (nai_ssl_dgram_t*)nai_malloc(sizeof(*d) + mtu);
        if (d == 0) {
            r = -1;
            goto _end;
        };

        d->size = mtu;
        d->async = !!async_read;
        d->wpending = 0;
        d->rbuf = (uint8_t*)(d + 1);
        d->wbuf = 0;
        d->peer.len = 0;
        d->peer.addr.sa_family = AF_UNSPEC;

        SSL_set_ex_data(s->ssl, nai_ssl.extend, d);
    };


    nai_ssl_clear_errno();

    for (;;) {

        if (d->wpending) {
            v.buf = (uint8_t*)d->wbuf;
            v.len = d->wlen;
            r = ops->sendm(s->ud, 
                &v, 1, 0, &d->peer.addr, d->peer.len, 0, 0);
            if (r < 0) {
                goto _end;
            };

            d->wpending = 0;
        };

#if defined(LIBWOLFSSL_VERSION_HEX)
        /* wolfssl without DTLSv1_listen, 
         * and i don't know why SSL_accept isn't working */
        nai_errno = ENOTSUP;
        r = -1;
        goto _end;
#else
        r = DTLSv1_listen(s->ssl, nai_ssl.temp);
#endif
        if (r > 0) {
            break;
        };

        ec = SSL_get_error(s->ssl, r);
        switch (ec) {
        case SSL_ERROR_WANT_READ:
            nai_errno = NAI_EAGAIN;
            break;
        case SSL_ERROR_WANT_WRITE:
            nai_errno = NAI_EAGAIN;
            break;
        case SSL_ERROR_ZERO_RETURN:
            nai_errno = ECONNABORTED;
            s->error = 1;
            break;
        case SSL_ERROR_SYSCALL:
            s->error = 1;
            break;
        default:
            nai_ssl_set_errno(0);
            s->error = 1;
            break;
        };

        r = -1;
        goto _end;
    };

    if (namelen) {
        if (name && namelen[0] >= d->peer.len) {
            nai_memcpy(name, &d->peer.addr, d->peer.len);
        };

        namelen[0] = d->peer.len;
    };

    s->listened = 1;
    r = 0;

_end:
    return r;
};


nai_int_t nai_dtls_bypass(nai_ssl_t* s, nai_ssl_t* l)
{
    nai_int_t r;
    BIO* b;
    BIO* n;
    nai_ssl_t t;
    nai_ssl_dgram_t* p;


    if (!l->listened) {
        nai_errno = ENOTCONN;
        r = -1;
        goto _end;
    };

    /* backup the listen ssl */
    t.ctx = l->ctx;
    t.ssl = l->ssl;
    t.flags = l->flags;

    /* create new bio first */
    if (s != 0) {
        n = BIO_new(nai_ssl.meth);
        if (n == 0) {
            r = -1;
            goto _end;
        };
    } else {
        n = 0;
    };

    /* get old bio */
    b = SSL_get_rbio(t.ssl);

    /* reopen the listen ssl */
    l->ctx = 0;
    l->ssl = 0;
    l->flags = 0;
    l->ktls = 3;
    r = nai_ssl_open_intern(l, t.ctx, 0, b);
    if (r < 0) {
        /* failed, restore */
        l->ctx = t.ctx;
        l->ssl = t.ssl;
        l->flags = t.flags;

        if (n != 0) {
            BIO_free(n);
        };
        goto _end;
    };


    /* move dgram context */
    p = (nai_ssl_dgram_t*)SSL_get_ex_data(t.ssl, nai_ssl.extend);
    SSL_set_ex_data(l->ssl, nai_ssl.extend, p);
    SSL_set_ex_data(t.ssl, nai_ssl.extend, 0);


    /* move to the dest ssl */
    if (s != 0) {
        s->ctx = t.ctx;
        s->ssl = t.ssl;
        s->flags = t.flags;
        s->listened = 0;
        BIO_up_ref(b);
        BIO_set_data(n, s);
        BIO_set_init(n, 1);
        SSL_set_bio(s->ssl, n, n);
        SSL_set_ex_data(s->ssl, nai_ssl.connection, s);
    } else {
        /* drop it */
        nai_ssl_close(&t);
    };

    r = 0;

_end:
    return r;
};


nai_int_t nai_dtls_drop(nai_ssl_t* s)
{
    nai_int_t r;


    r = nai_dtls_bypass(0, s);

    return r;
};


nai_int_t nai_dtls_handshake_timeout(nai_ssl_t* s)
{
    nai_int_t r;
    nai_int_t ec;
    nai_ssl_ops_t* ops;


    ops = s->ops;
    if (ops == 0) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    nai_ssl_clear_errno();

    s->timeoset = 0;

    r = DTLSv1_handle_timeout(s->ssl);
    if (r != 1) {

        ec = SSL_get_error(s->ssl, r);
        switch (ec) {
        case SSL_ERROR_WANT_READ:
            goto _again;
        case SSL_ERROR_WANT_WRITE:
            goto _again;
        case SSL_ERROR_SYSCALL:
            s->error = 1;
            break;
        case SSL_ERROR_ZERO_RETURN:
            nai_errno = ECONNABORTED;
            s->error = 1;
            break;
        default:
            nai_ssl_set_errno(0);
            s->error = 1;
            break;
        };

        r = -1;
        goto _end;
    };

    r = 0;

_end:
    return r;

_again:
    nai_errno = NAI_EAGAIN;
    r = -1;
    goto _end;
};

nai_int_t nai_dtls_get_mtu(nai_ssl_t* s)
{
    nai_int_t r;

#if OPENSSL_VERSION_NUMBER < 0x10101000L || defined(LIBRESSL_VERSION_NUMBER)
    /* Both OpenSSL less than 1.1.1 and LibreSSL do not support DTLS_get_data_mtu
       so we return -1 to indicate that the MTU cannot be determined. */
    /* TODO: Implement alternative MTU determination method.
       You can refer to the implementation of the dtls_get_data_mtu function 
       in https://github.com/cernekee/openconnect/blob/master/openssl-dtls.c */
    r = -1;
#else
    r = (nai_int_t)DTLS_get_data_mtu(s->ssl);
#endif

    return r;
};


//////////////////////////////////////////////////////////////////////////////
// default ssl operations implementation 
// which assume custom user data points to an iobase


nai_int_t nai_ssl_iobase_poll(void* ud, nai_int_t event)
{
    nai_int_t r;
    nai_iobase_t* c;


    c = (nai_iobase_t*)ud;
    r = !!(nai_iobase_get_event(c) & event);

    return r;
};


nai_int_t nai_ssl_iobase_want(void* ud, nai_int_t event)
{
    nai_int_t r;
    nai_iobase_t* c;


    c = (nai_iobase_t*)ud;
    r = (nai_iobase_get_event(c) & event);
    if (r) {
        c->st.wants |= r;
    };

    return r;
};


nai_int_t nai_ssl_iobase_emit(void* ud, nai_int_t event)
{
    nai_int_t r;
    nai_iobase_t* c;


    c = (nai_iobase_t*)ud;
    if (c->cb) {
        c->cb(c, event);
    };
    r = 0;

    return r;
};


nai_evloop_t* nai_ssl_iobase_get_loop(void* ud)
{
    nai_evloop_t* l;
    nai_iobase_t* c;


    c = (nai_iobase_t*)ud;
    l = nai_iobase_get_loop(c);

    return l;
};


nai_int_t nai_ssl_iobase_get_opt(void* ud, nai_int_t which)
{
    nai_int_t r;
    intptr_t n;
    nai_iobase_t* c;


    c = (nai_iobase_t*)ud;
    switch (which) {
    case NAI_SSL_OPT_ASYNCREAD:
        r = nai_iobase_get_opt(c, NAI_IO_FEAT_MODEL, &n);
        if (r >= 0 && n) {
            r = !(n & NAI_EV_FEAT_ASYNC);
        } else {
            r = 0;
        };
        break;

    case NAI_SSL_OPT_SENDFILE:
        if (!c->st.sendfile) {
            r = 0;
            break;
        };

        r = nai_iobase_get_opt(c, NAI_IO_FEAT_SENDFILE, &n);
        if (r >= 0 && n) {
            r = 1;
        } else {
            r = 0;
        };
        break;

    case NAI_SSL_OPT_BLOCKING:
        r = c->st.blocking;
        break;

    default:
        r = 0;
        break;
    };

    return r;
};


nai_int_t nai_ssl_iobase_get_peer(
    void* ud, nai_sockaddr_t* name, nai_int_t* namelen)
{
    nai_int_t r;
    nai_iobase_t* c;


    c = (nai_iobase_t*)ud;
    r = nai_sock_get_peername(nai_iobase_get_fd(c), name, namelen);

    return r;
};


nai_int_t nai_ssl_iobase_get_socket(void* ud)
{
    nai_fd_t r;
    nai_iobase_t* c;


    c = (nai_iobase_t*)ud;
    r = nai_iobase_get_fd(c);

    return r;
};


nai_int_t nai_ssl_iobase_get_mtu(void* ud)
{
    nai_int_t r;


    (void)ud;
    r = 1500;

    return r;
};


nai_int_t nai_ssl_iobase_set_timer(void* ud, uint32_t msec)
{
    nai_int_t r;
    nai_iobase_t* c;


    c = (nai_iobase_t*)ud;
    r = nai_iobase_set_timeout(c, NAI_TIMEOP_SET, msec);

    return r;
};



#endif

