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
/// @file       nai_ssl_ctx.c
/// @brief      
/// @details
/// @date       2022-10-22
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai/io/nai_ssl_ctx.h"


#if (NAI_HAVE_SSL)


#include "nai/io/nai_ssl_ocsp.h"
#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_log.h"
#include "nai/runtime/nai_util.h"
#include "nai/runtime/nai_pool.h"
#include "nai/runtime/nai_array.h"
#include "nai/runtime/nai_string.h"
#include "nai/service/nai_command.h"

#if (NAI_HAVE_OPENSSL) || (NAI_HAVE_LIBRESSL)
#include <openssl/crypto.h>
#include <openssl/rand.h>
#include <openssl/dh.h>
#include <openssl/err.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/pem.h>
#ifndef OPENSSL_NO_ENGINE
#include <openssl/engine.h>
#endif
#elif (NAI_HAVE_WOLFSSL)
#include <wolfssl/openssl/crypto.h>
#include <wolfssl/openssl/rand.h>
#include <wolfssl/openssl/dh.h>
#include <wolfssl/openssl/err.h>
#include <wolfssl/openssl/x509.h>
#include <wolfssl/openssl/x509v3.h>
#include <wolfssl/openssl/pem.h>
#ifndef OPENSSL_NO_ENGINE
#include <wolfssl/openssl/engine.h>
#endif

#ifndef ERR_LIB_SYS
#define ERR_LIB_SYS             2
#endif
#ifndef SSL_SESSION_CACHE_MAX_SIZE_DEFAULT
#define SSL_SESSION_CACHE_MAX_SIZE_DEFAULT (1024*20)
#endif

#endif


#if (OPENSSL_VERSION_NUMBER < 0x30000000L && !defined ERR_peek_error_data)

#define ERR_peek_error_data(d, f)                       \
    ERR_peek_error_line_data(NULL, NULL, d, f)          \

#endif


static void nai_ssl_error(nai_int_t level, const char* msg)
{
    nai_int_t n;
    nai_int_t flags;
    const char* data;
    char buf[256];


    (void)level;

    n = ERR_peek_error_data(&data, &flags);

    ERR_error_string_n(n, buf, sizeof(buf));

    if (*data && (flags & ERR_TXT_STRING)) {
        nai_log_error(NAI_LOG_CORE, 
            0, "%s %s, %s", msg, buf, data);
    } else {
        nai_log_error(NAI_LOG_CORE, 
            0, "%s %s", msg, buf);
    };

    (void)ERR_get_error();
};


void nai_ssl_set_errno(nai_int_t e)
{
    nai_int_t ec;
    nai_int_t last = e;


    while ((ec = ERR_peek_error())) {
        last = (ERR_GET_REASON(ec)) + ((ERR_GET_LIB(ec) - ERR_LIB_SYS) * 10000);
        nai_ssl_error(NAI_LOG_ALERT, "SSL");
    };

    ERR_clear_error();

    if (last < 10000) {
        nai_errno = last;
    } else {
        nai_errno = nai_errno_from_ssl(last);
    };
};


void nai_ssl_clear_errno()
{
    nai_int_t ec;

    while ((ec = ERR_peek_error())) {
        nai_ssl_error(NAI_LOG_ALERT, "ignoring SSL");
    };

    ERR_clear_error();
};



//////////////////////////////////////////////////////////////////////////////
// ssl context



nai_ssl_lib_t nai_ssl = {
    0, 0, NAI_ONCE_INIT
};


extern nai_int_t nai_ssl_bio_init();
extern nai_int_t nai_ssl_bio_term();


static nai_int_t nai_ssl_get_full_path(
    nai_ssl_ctx_t* ctx, nai_str_t* out, 
    const char* ptr, size_t len, char* buf, size_t buflen)
{
    intptr_t r;
    nai_int_t ec;
    nai_ssl_ctx_ops_t* ops;


    r = nai_path_is_absolute(ptr, len);

    ops = ctx->ops;
    if (!r && ops && ops->get_full_path) {
        r = ops->get_full_path(ctx->ud, buf, buflen, ptr, len);
        if (r < 0) {
            nai_log_error(NAI_LOG_CORE, 
                nai_errno, "get full path failed");
            r = -1;
            goto _end;
        };
        if (r >= (intptr_t)buflen) {
            ec = ERANGE;
            nai_log_error(NAI_LOG_CORE, 
                ec, "the path is too large");
            nai_errno = ec;
            r = -1;
            goto _end;
        };
        nai_str_setm(out, buf, r);
    } else if ((intptr_t)len < 0) {
        nai_str_sets(out, ptr);
    } else {
        nai_str_setm(out, ptr, len);
    };

    r = 0;

_end:
    return (nai_int_t)r;
};


static BIO* nai_ssl_open_bio(
    nai_ssl_ctx_t* ctx, const nai_str_t* path)
{
    nai_int_t r;
    nai_int_t len;
    const char* ptr;
    nai_str_t out;
    BIO* bio;
    char buf[NAI_PATH_MAX];


    ptr = nai_str(path);
    len = nai_str_len(path);
    if (nai_strncmp(ptr, "data:", sizeof("data:") - 1) == 0) {

        bio = BIO_new_mem_buf(
            ptr + (sizeof("data:") - 1),
            len - (sizeof("data:") - 1));
        if (bio == 0) {
            nai_ssl_set_errno(ENOMEM);
            nai_log_alert(NAI_LOG_CORE, 
                nai_errno, "BIO_new_mem_buf() failed");
            goto _end;
        }
    } else {

        r = nai_ssl_get_full_path(ctx, &out, ptr, len, buf, sizeof(buf));
        if (r < 0) {
            nai_log_error(NAI_LOG_CORE, 
                nai_errno, "nai_ssl_get_full_path('%.*s' failed", 
                len, ptr);
            bio = 0;
            goto _end;
        };

        bio = BIO_new_file(nai_str(&out), "r");
        if (bio == 0) {
            nai_ssl_set_errno(ENOMEM);
            nai_log_error(NAI_LOG_CORE, 
                nai_errno, "BIO_new_file('%s') failed", nai_str(&out));
            goto _end;
        }
    };

_end:
    return bio;
};


static int nai_ssl_password_handle(
    char* buf, int size, int rwflag, void* ud)
{
    nai_int_t r;
    nai_str_t* pwd = (nai_str_t*)ud;


    if (rwflag) {
        nai_log_error(NAI_LOG_CORE, 0,
            "nai_ssl_password_handle can not encrypt");
        r = 0;
        goto _end;
    };

    if (pwd == 0) {
        r = 0;
        goto _end;
    };

    if (nai_str_len(pwd) > (size_t)size) {
        nai_log_error(NAI_LOG_CORE, 0,
            "password is truncated to %d bytes", size);
    } else {
        size = nai_str_len(pwd);
    };

    nai_memcpy(buf, nai_str(pwd), size);
    r = size;

_end:
    return r;
};


nai_int_t nai_ssl_load_certificate(
    nai_ssl_ctx_t* ctx, nai_ssl_cert_t* p, const nai_str_t* cert)
{
    nai_int_t r;
    nai_int_t ec;
    BIO* bio;
    X509* x509 = 0;
    X509* temp;
    STACK_OF(X509)* chain = 0;


    /* open file */
    bio = nai_ssl_open_bio(ctx, cert);
    if (bio == 0) {
        nai_log_alert(NAI_LOG_CORE, 
            nai_errno, "nai_ssl_open_bio(\"%s\") failed", nai_str(cert));
        r = -1;
        goto _end;
    };

    /* load certificate */
    x509 = PEM_read_bio_X509_AUX(bio, 0, 0, 0);
    if (x509 == 0) {
        nai_ssl_set_errno(EINVAL);
        nai_log_error(NAI_LOG_CORE, 
            nai_errno, "PEM_read_bio_X509_AUX() failed");
        r = -1;
        goto _end;
    };

    /* rest of the chain */
    chain = sk_X509_new_null();
    if (chain == 0) {
        nai_ssl_set_errno(ENOMEM);
        nai_log_error(NAI_LOG_CORE, 
            nai_errno, "sk_X509_new_null() failed");
        r = -1;
        goto _end;
    };

    for (;;) {

        temp = PEM_read_bio_X509(bio, 0, 0, 0);
        if (temp == 0) {
            ec = ERR_peek_last_error();
            if (ERR_GET_LIB(ec) == ERR_LIB_PEM && 
                ERR_GET_REASON(ec) == PEM_R_NO_START_LINE) {
                /* end of file */
                ERR_clear_error();
                break;
            };

            /* some real error */
            nai_ssl_set_errno(EINVAL);
            nai_log_error(NAI_LOG_CORE, 
                nai_errno, "PEM_read_bio_X509() failed");
            r = -1;
            goto _end;
        };

        if (sk_X509_push(chain, temp) == 0) {
            nai_ssl_set_errno(ENOMEM);
            nai_log_error(NAI_LOG_CORE, 
                nai_errno, "sk_X509_push() failed");
            r = -1;
            goto _end;
        };
    };

    p->cert = x509;
    p->chain = chain;
    r = 0;

_end:
    if (bio) {
        BIO_free(bio);
    };
    if (r < 0) {
        if (x509) {
            X509_free(x509);
            x509 = 0;
        };
        if (chain) {
            sk_X509_pop_free(chain, X509_free);
        };
    };
    return r;
};


nai_int_t nai_ssl_load_certificate_key(
    nai_ssl_ctx_t* ctx, nai_ssl_pkey_t* p, 
    const nai_str_t* key, const nai_str_t* passwords, nai_int_t count)
{
    nai_int_t r;
    nai_int_t len;
    const char* ptr;
    BIO* bio = 0;
    pem_password_cb* cb;

#ifndef OPENSSL_NO_ENGINE
    ENGINE* engine = 0;
    char engine_id[256];
#endif


    ptr = nai_str(key);
    len = nai_str_len(key);

    if (nai_strncmp(ptr, "engine:", sizeof("engine:") - 1) == 0) {

#ifndef OPENSSL_NO_ENGINE

        const char* start;
        const char* last;

        start = ptr + sizeof("engine:") - 1;
        last = nai_strnchr(p, len - (start - ptr), ':');
        if (last == NULL) {
            nai_errno = EINVAL;
            nai_log_error(NAI_LOG_CORE, 
                nai_errno, "invalid format about 'engine:...'");
            r = -1;
            goto _end;
        }

        if (last - start >= (intptr_t)nai_countof(engine_id)) {
            nai_errno = EINVAL;
            nai_log_error(NAI_LOG_CORE, 
                nai_errno, "the name of engine is too large");
            r = -1;
            goto _end;
        };

        nai_memcpy(engine_id, p, last - start);
        engine_id[last - start] = '\0';
        engine = ENGINE_by_id(engine_id);
        if (engine == 0) {
            nai_ssl_set_errno(ENOENT);
            nai_log_error(NAI_LOG_CORE, 
                nai_errno, "ENGINE_by_id() failed");
            r = -1;
            goto _end;
        };


        p->pkey = ENGINE_load_private_key(engine, last+1, 0, 0);
        if (p->pkey == 0) {
            nai_ssl_set_errno(EINVAL);
            nai_log_error(NAI_LOG_CORE, 
                nai_errno, "ENGINE_load_private_key() failed");
            r = -1;
            goto _end;
        };

        r = 0;

#else
        (void)len;

        nai_log_error(NAI_LOG_CORE, ENOTSUP, 
            "loading \"engine:...\" certificate keys is not supported");
        nai_errno = ENOTSUP;
        r = -1;

#endif

        goto _end;
    }

    /* open file */
    bio = nai_ssl_open_bio(ctx, key);
    if (bio == 0) {
        nai_log_alert(NAI_LOG_CORE, 
            nai_errno, "nai_ssl_open_bio(\"%s\") failed", nai_str(key));
        r = -1;
        goto _end;
    };

    if (count <= 0) {
        count = 1;
        cb = 0;
    } else {
        cb = nai_ssl_password_handle;
    };

    for (;;) {

        p->pkey = PEM_read_bio_PrivateKey(bio, 0, cb, (void*)passwords);
        if (p->pkey != 0) {
            break;
        };

        count --;
        if (count <= 0) {
            nai_ssl_set_errno(EINVAL);
            nai_log_error(NAI_LOG_CORE, 
                nai_errno, "PEM_read_bio_PrivateKey() failed");
            r = -1;
            goto _end;
        };

        ERR_clear_error();
        BIO_reset(bio);
        passwords ++;
    };

    r = 0;

_end:
    if (bio) {
        BIO_free(bio);
    };
#ifndef OPENSSL_NO_ENGINE
    if (engine) {
        ENGINE_free(engine);
    };
#endif
    return r;
};


nai_int_t nai_ssl_load_cert_crl(
    nai_ssl_ctx_t* ctx, nai_ssl_cert_crl_t* p, 
    const nai_str_t* cert)
{
    nai_int_t r;
    BIO* bio;
    STACK_OF(X509_INFO)* chain;


    /* open file */
    bio = nai_ssl_open_bio(ctx, cert);
    if (bio == 0) {
        nai_log_alert(NAI_LOG_CORE, 
            nai_errno, "nai_ssl_open_bio(\"%s\") failed", nai_str(cert));
        r = -1;
        goto _end;
    };

    chain = PEM_X509_INFO_read_bio(bio, 0, 0, 0);
    if (chain == 0) {
        nai_ssl_set_errno(EINVAL);
        nai_log_error(NAI_LOG_CORE, 
            nai_errno, "PEM_X509_INFO_read_bio() failed");
        r = -1;
        goto _end;
    };

    p->chain = chain;
    r = 0;

_end:
    if (bio) {
        BIO_free(bio);
    };
    return r;
};


#define NAI_SSL_ADD_CERT        1
#define NAI_SSL_ADD_CRL         2
#define NAI_SSL_ADD_NAME        4
#define NAI_SSL_ADD_CERT_NAME   3


static nai_int_t nai_ssl_ctx_add_name(
    nai_ssl_ctx_t* ssl, X509* x509, const nai_str_t* cert)
{
    nai_int_t r;
    nai_int_t n, count;
    X509_NAME* name;
    X509_NAME* temp;
    STACK_OF(X509_NAME)* chain;


    chain = SSL_CTX_get_client_CA_list(ssl->ctx);
    if (chain != 0) {
        name = X509_get_subject_name(x509);
        count = sk_X509_NAME_num(chain);
        for (n = 0; n < count; n ++) {
            temp = sk_X509_NAME_value(chain, n);
            if (X509_NAME_cmp(temp, name) == 0) {
                r = 0;
                goto _end;
            };
        };
    };

    r = SSL_CTX_add_client_CA(ssl->ctx, x509);
    if (r == 0) {
        nai_ssl_set_errno(ENOMEM);
        nai_log_alert(NAI_LOG_CORE, 
            nai_errno, "SSL_CTX_add_client_CA(\"%.*s\") failed", 
            nai_str_len(cert), nai_str(cert));
        r = -1;
    };

_end:
    return r;
};


static nai_int_t nai_ssl_ctx_add_cert_crl(
    nai_ssl_ctx_t* ssl, nai_ssl_cert_crl_t* p, 
    const nai_str_t* cert, nai_int_t options)
{
    nai_int_t r;
    nai_int_t n, add;
    nai_int_t count;
    X509_INFO* inf;
    X509_STORE* store;


    store = SSL_CTX_get_cert_store(ssl->ctx);
    if (store == 0) {
        nai_ssl_set_errno(EINVAL);
        nai_log_alert(NAI_LOG_CORE, 
            nai_errno, "SSL_CTX_get_cert_store() failed");
        r = -1;
        goto _end;
    };

    add = 0;
    count = sk_X509_INFO_num(p->chain);
    for (n = 0; n < count; n ++) {
        inf = sk_X509_INFO_value(p->chain, n);
        if (inf->x509) {
            if (!(options & NAI_SSL_ADD_CERT_NAME)) {
                continue;
            };

            if ((options & NAI_SSL_ADD_CERT) && 
                X509_STORE_add_cert(store, inf->x509) == 0) {
                nai_ssl_set_errno(ENOMEM);
                nai_log_alert(NAI_LOG_CORE, 
                    nai_errno, "X509_STORE_add_cert(\"%.*s\") failed", 
                    nai_str_len(cert), nai_str(cert));
                r = -1;
                goto _end;
            };
            if ((options & NAI_SSL_ADD_NAME) && 
                nai_ssl_ctx_add_name(ssl, inf->x509, cert) < 0) {
                nai_log_alert(NAI_LOG_CORE, 
                    nai_errno, "nai_ssl_ctx_add_name(\"%.*s\") failed", 
                    nai_str_len(cert), nai_str(cert));
                r = -1;
                goto _end;
            };

            add ++;
        };
        if (inf->crl) {
            if (!(options & NAI_SSL_ADD_CRL)) {
                continue;
            };

            if (X509_STORE_add_crl(store, inf->crl) == 0) {
                nai_ssl_set_errno(ENOMEM);
                nai_log_alert(NAI_LOG_CORE, 
                    nai_errno, "X509_STORE_add_crl(\"%.*s\") failed", 
                    nai_str_len(cert), nai_str(cert));
                r = -1;
                goto _end;
            };

            add ++;
        }
    };

    r = add;

_end:
    return r;
};


static void nai_ssl_lib_init()
{
    nai_int_t r;
    nai_int_t n;


#if OPENSSL_VERSION_NUMBER >= 0x10100003L

    if (OPENSSL_init_ssl(OPENSSL_INIT_LOAD_CONFIG, 0) == 0) {
        nai_ssl_set_errno(ENOMEM);
        nai_log_alert(NAI_LOG_CORE, 
            nai_errno, "OPENSSL_init_ssl() failed");
        r = -1;
        goto _end;
    }

    /*
     * OPENSSL_init_ssl() may leave errors in the error queue
     * while returning success
     */
    ERR_clear_error();

#else

    OPENSSL_config(0);
    SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();

#endif


#ifndef SSL_OP_NO_COMPRESSION

    STACK_OF(SSL_COMP)* ssl_comps;
    /*
     * Disable gzip compression in OpenSSL prior to 1.0.0 version,
     * this saves about 522K per connection.
     */
    ssl_comps = SSL_COMP_get_compression_methods();
    n = sk_SSL_COMP_num(ssl_comps);
    while (n--) {
        sk_SSL_COMP_pop(ssl_comps);
    };

#endif

    n = 0;
    for ( ; n < (nai_int_t)3; n ++) {

        nai_ssl.index[n] = SSL_get_ex_new_index(0, 0, 0, 0, 0);
        if (nai_ssl.index[n] == -1) {
            nai_ssl_set_errno(ENOMEM);
            nai_log_alert(NAI_LOG_CORE, 
                nai_errno, "SSL_get_ex_new_index() failed");
            r = -1;
            goto _end;
        };
    };
    for ( ; n < (nai_int_t)nai_countof(nai_ssl.index)-3; n ++) {

        nai_ssl.index[n] = SSL_CTX_get_ex_new_index(0, 0, 0, 0, 0);
        if (nai_ssl.index[n] == -1) {
            nai_ssl_set_errno(ENOMEM);
            nai_log_alert(NAI_LOG_CORE, 
                nai_errno, "SSL_CTX_get_ex_new_index() failed");
            r = -1;
            goto _end;
        };
    };
    for ( ; n < (nai_int_t)nai_countof(nai_ssl.index); n ++) {
        nai_ssl.index[n] = X509_get_ex_new_index(0, 0, 0, 0, 0);
        if (nai_ssl.index[n] == -1) {
            nai_ssl_set_errno(ENOMEM);
            nai_log_alert(NAI_LOG_CORE, 
                nai_errno, "X509_get_ex_new_index() failed");
            r = -1;
            goto _end;
        };
    };

    r = RAND_bytes(nai_ssl.secret, sizeof(nai_ssl.secret));
    if (!r) {
        nai_ssl_set_errno(ENOMEM);
        nai_log_alert(NAI_LOG_CORE, 
            nai_errno, "RAND_bytes() failed");
        r = -1;
        goto _end;
    };

#if OPENSSL_VERSION_NUMBER >= 0x10101000L && \
    !(defined(LIBRESSL_VERSION_NUMBER) || defined(LIBWOLFSSL_VERSION_HEX))
    nai_ssl.temp = BIO_ADDR_new();
    if (nai_ssl.temp == 0) {
        nai_ssl_set_errno(ENOMEM);
        nai_log_alert(NAI_LOG_CORE, 
            nai_errno, "BIO_ADDR_new() failed");
        r = -1;
        goto _end;
    };
#endif

    r = nai_ssl_bio_init();
    if (r < 0) {
        goto _end;
    };

    r = 0;

_end:
    if (r < 0) {
        nai_ssl.error = nai_errno;
    };

    nai_memory_barrier();
    nai_ssl.inited = 1;
};


static void nai_ssl_lib_term()
{
    if (nai_ssl.inited) {

#if OPENSSL_VERSION_NUMBER < 0x10100000L || defined(LIBRESSL_VERSION_NUMBER)
        CRYPTO_cleanup_all_ex_data();
        CONF_modules_free();
        ERR_free_strings();
        EVP_cleanup();
#ifndef OPENSSL_NO_COMP
        SSL_COMP_free_compression_methods();
#endif
#ifndef OPENSSL_NO_ENGINE
        ENGINE_cleanup();
#endif
        ERR_remove_thread_state(0);
#endif

#if OPENSSL_VERSION_NUMBER >= 0x10101000L && \
    !(defined(LIBRESSL_VERSION_NUMBER) || defined(LIBWOLFSSL_VERSION_HEX))
        if (nai_ssl.temp != 0) {
            BIO_ADDR_free(nai_ssl.temp);
            nai_ssl.temp = 0;
        };
#endif
#if OPENSSL_VERSION_NUMBER >= 0x10101000L
        OPENSSL_cleanup();
#endif

        nai_ssl_bio_term();
    };


    return;
};


static int nai_ssl_verify_callback(int ok, X509_STORE_CTX *x509_store)
{
    nai_int_t err;
    nai_int_t depth;
    char* subject;
    char* issuer;
    X509* cert;
    X509_NAME* sname;
    X509_NAME* iname;


    if (nai_log_is_enabled_debug(&nai_log_core)) {
        err = X509_STORE_CTX_get_error(x509_store);
        cert = X509_STORE_CTX_get_current_cert(x509_store);
        depth = X509_STORE_CTX_get_error_depth(x509_store);

        sname = X509_get_subject_name(cert);
        if (sname) {
            subject = X509_NAME_oneline(sname, 0, 0);
            if (subject == 0) {
                nai_ssl_clear_errno();
                nai_log_debug(NAI_LOG_CORE, 0, "X509_NAME_oneline() failed");
            };
        } else {
            subject = 0;
        };

        iname = X509_get_issuer_name(cert);
        if (iname) {
            issuer = X509_NAME_oneline(iname, 0, 0);
            if (issuer == 0) {
                nai_ssl_clear_errno();
                nai_log_debug(NAI_LOG_CORE, 0, "X509_NAME_oneline() failed");
            };
        } else {
            issuer = 0;
        };

        nai_log_debug(NAI_LOG_CORE, 0,
           "verify:%d, error:%d, depth:%d, subject:\"%s\", issuer:\"%s\"",
           ok, err, depth, 
           subject ? subject : "(none)", 
           issuer ? issuer : "(none)");

        if (subject) {
            OPENSSL_free(subject);
        };

        if (issuer) {
            OPENSSL_free(issuer);
        };
    };

    return 1;
};


static void nai_ssl_info_handle(const SSL* ssl, int where, int ret)
{
#ifndef SSL_OP_NO_RENEGOTIATION
    nai_ssl_t* s;


    (void)ret;

    if ((where & SSL_CB_HANDSHAKE_START)
        && SSL_is_server((SSL*)ssl)) {
        s = (nai_ssl_t*)SSL_get_ex_data(ssl, nai_ssl.connection);
        if (s && s->handshaked) {
            s->renegotiation = 1;
            s->error = 1;
        };
    }

#else

    (void)ssl;
    (void)ret;
    (void)where;

#endif
};


#if OPENSSL_VERSION_NUMBER < 0x10100001L && !defined(LIBRESSL_VERSION_NUMBER)

static RSA* nai_ssl_rsa_callback(SSL* ssl, int is_export, int length)
{
    static RSA* key;


    (void)ssl;
    (void)is_export;

    if (length != 512) {
        return 0;
    };

#if OPENSSL_VERSION_NUMBER < 0x10100003L && !defined(OPENSSL_NO_DEPRECATED)

    if (key == 0) {
        key = RSA_generate_key(512, RSA_F4, 0, 0);
    };

#endif

    return key;
};

#endif


#ifdef SSL_R_CERT_CB_ERROR


static int nai_ssl_cert_callback(SSL* ssl, void *arg)
{
    nai_int_t r;
    nai_ssl_t* s;


    (void)arg;


    s = (nai_ssl_t*)SSL_get_ex_data(ssl, nai_ssl.connection);

    if (s->handshaked) {
        r = 0;
    } else {
        if (s->ops && s->ops->lookup_certs) {
            r = s->ops->lookup_certs(s->ud);
        } else {
            r = 0;
        };

        if (r < 0) {
            r = 0;
        } else {
            r = 1;
        };
    };

    return r;
};

#endif


#ifdef SSL_CTRL_SET_TLSEXT_HOSTNAME


static int nai_ssl_server_callback(SSL* ssl, int* ad, void* arg)
{
    nai_int_t r;
    nai_ssl_t* s;
    const char* name;


    (void)arg;


    s = (nai_ssl_t*)SSL_get_ex_data(ssl, nai_ssl.connection);
    if (s->handshaked) {
        ad[0] = SSL_AD_NO_RENEGOTIATION;
        r = SSL_TLSEXT_ERR_ALERT_FATAL;
        goto _end;
    };

    name = SSL_get_servername(ssl, TLSEXT_NAMETYPE_host_name);

    if (s->ops && s->ops->lookup_server) {
        r = s->ops->lookup_server(s->ud, name);
    } else {
        r = 0;
    };

    if (s->rejected) {
        ad[0] = SSL_AD_UNRECOGNIZED_NAME;
        r = SSL_TLSEXT_ERR_ALERT_FATAL;
    } else if (r >= 0) {
        r = SSL_TLSEXT_ERR_OK;
    } else {
        ad[0] = SSL_AD_INTERNAL_ERROR;
        r = SSL_TLSEXT_ERR_ALERT_FATAL;
    };

_end:
    return r;
};


#endif


#if OPENSSL_VERSION_NUMBER >= 0x10000000L && !defined(LIBWOLFSSL_VERSION_HEX)


static nai_int_t nai_ssl_token_generate(
    nai_ssl_t* s, unsigned char* buf, unsigned int buflen)
{
    nai_int_t r;
    nai_int_t len;
    uint32_t rlen;
    nai_ssl_dgram_t* d;
    nai_socknbuf_t* n;
    nai_socknbuf_t nbuf;
    uint8_t data[128];


    if (nai_ssl.inited == 0) {
        nai_once(&nai_ssl.once, nai_ssl_lib_init);
    };
    if (nai_ssl.error) {
        nai_errno = nai_ssl.error;
        r = -1;
        goto _end;
    };

    d = (nai_ssl_dgram_t*)SSL_get_ex_data(s->ssl, nai_ssl.extend);
    if (d != 0) {
        n = &d->peer;
    } else if (s->ops && s->ops->get_peer) {
        nbuf.len = sizeof(nbuf.storage);
        r = s->ops->get_peer(s->ud, &nbuf.addr, &nbuf.len);
        if (r < 0) {
            goto _end;
        };

        n = &nbuf;
    } else {
        nai_errno = ENOTCONN;
        r = -1;
        goto _end;
    };

    switch (n->addr.sa_family) {
    case AF_INET:
        len = sizeof(n->addr_in4.sin_port);
        memcpy(data, &n->addr_in4.sin_port, len);
        memcpy(data + len, &n->addr_in4.sin_addr, 
            sizeof(n->addr_in4.sin_addr));
        len += sizeof(n->addr_in4.sin_addr);
        break;
#if (NAI_HAVE_SOCKADDR_IN6)
    case AF_INET6:
        len = sizeof(n->addr_in6.sin6_port);
        memcpy(data, &n->addr_in6.sin6_port, len);
        memcpy(data + len, &n->addr_in6.sin6_addr, 
            sizeof(n->addr_in6.sin6_addr));
        len += sizeof(n->addr_in6.sin6_addr);
        break;
#endif
#if (NAI_HAVE_SOCKADDR_UN)
    case AF_UNIX:
        len = n->len - nai_offsetof(nai_sockaddr_un_t, sun_path);
        memcpy(data, &n->addr_un.sun_path, len);
        break;
#endif
    default:
        nai_errno = ENOTCONN;
        r = -1;
        goto _end;
    };

    if (buf && buflen >= EVP_MAX_MD_SIZE) {
        HMAC(EVP_sha1(), 
            nai_ssl.secret, sizeof(nai_ssl.secret),
            data, len, buf, &rlen);
    } else {
        rlen = EVP_MAX_MD_SIZE;
    };

    r = rlen;

_end:
    return r;
};


#if OPENSSL_VERSION_NUMBER >= 0x10101000L

static int nai_ssl_cookie_verify(SSL* ssl, 
    const unsigned char* cookie, unsigned int cookie_len)

#else

static int nai_ssl_cookie_verify(SSL* ssl, 
    unsigned char* cookie, unsigned int cookie_len)

#endif
{
    nai_int_t r;
    nai_ssl_t* s;
    uint8_t result[EVP_MAX_MD_SIZE];


    s = (nai_ssl_t*)SSL_get_ex_data(ssl, nai_ssl.connection);

    if (s->ops == 0) {
        r = 0;
    } else {
        if (s->ops->cookie_verify) {
            r = s->ops->cookie_verify(s->ud, (const char*)cookie, cookie_len);
        } else {
            r = nai_ssl_token_generate(s, result, sizeof(result));
            if (r < 0) {
                r = 0;
                goto _end;
            };

            if (r == (nai_int_t)cookie_len && 
                nai_memcmp(result, cookie, r) == 0) {
                r = 1;
            } else {
                r = 0;
            };
        };
    };

_end:
    return r;
};


static int nai_ssl_cookie_generate(SSL* ssl, 
    unsigned char* cookie, unsigned int* cookie_len)
{
    nai_int_t r;
    size_t len;
    nai_ssl_t* s;
    uint8_t result[EVP_MAX_MD_SIZE];


    s = (nai_ssl_t*)SSL_get_ex_data(ssl, nai_ssl.connection);

    if (s->ops == 0) {
        r = 0;
    } else {
        if (s->ops->cookie_generate) {
            len = DTLS1_COOKIE_LENGTH;
            r = s->ops->cookie_generate(s->ud, (char*)cookie, &len);
            if (r > 0) {
                cookie_len[0] = (nai_int_t)len;
            };
        } else {
            r = nai_ssl_token_generate(s, result, sizeof(result));
            if (r < 0) {
                r = 0;
                goto _end;
            };

            assert(r <= DTLS1_COOKIE_LENGTH);
            nai_memcpy(cookie, result, r);
            cookie_len[0] = r;
        };
    };

_end:
    return r;
};


#endif



nai_int_t nai_ssl_ctx_open(nai_ssl_ctx_t* ssl, nai_int_t methods)
{
    nai_int_t r;
    nai_int_t tls;
    SSL_CTX* ctx;


    if (nai_ssl.inited == 0) {
        nai_once(&nai_ssl.once, nai_ssl_lib_init);
    };
    if (nai_ssl.error) {
        nai_errno = nai_ssl.error;
        r = -1;
        goto _end;
    };

    if ((methods & NAI_SSL_TLS) && (methods & NAI_SSL_DTLS)) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };
    if ((methods & NAI_SSL_TLS)) {
        tls = 1;
    } else if ((methods & NAI_SSL_DTLS)) {
        tls = 0;
    } else {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    ctx = SSL_CTX_new(tls ? SSLv23_method() : DTLS_method());
    if (ctx == 0) {
        nai_ssl_set_errno(ENOMEM);
        nai_log_alert(NAI_LOG_CORE, 
            nai_errno, "SSL_CTX_new() failed");
        r = -1;
        goto _end;
    };


#ifdef SSL_OP_MICROSOFT_SESS_ID_BUG
    SSL_CTX_set_options(ctx, SSL_OP_MICROSOFT_SESS_ID_BUG);
#endif

#ifdef SSL_OP_NETSCAPE_CHALLENGE_BUG
    SSL_CTX_set_options(ctx, SSL_OP_NETSCAPE_CHALLENGE_BUG);
#endif

    /* server side options */

#ifdef SSL_OP_SSLREF2_REUSE_CERT_TYPE_BUG
    SSL_CTX_set_options(ctx, SSL_OP_SSLREF2_REUSE_CERT_TYPE_BUG);
#endif

#ifdef SSL_OP_MICROSOFT_BIG_SSLV3_BUFFER
    SSL_CTX_set_options(ctx, SSL_OP_MICROSOFT_BIG_SSLV3_BUFFER);
#endif

#ifdef SSL_OP_MSIE_SSLV2_RSA_PADDING
    /* this option allow a potential SSL 2.0 rollback (CAN-2005-2969) */
    SSL_CTX_set_options(ctx, SSL_OP_MSIE_SSLV2_RSA_PADDING);
#endif

#ifdef SSL_OP_SSLEAY_080_CLIENT_DH_BUG
    SSL_CTX_set_options(ctx, SSL_OP_SSLEAY_080_CLIENT_DH_BUG);
#endif

#ifdef SSL_OP_TLS_D5_BUG
    SSL_CTX_set_options(ctx, SSL_OP_TLS_D5_BUG);
#endif

#ifdef SSL_OP_TLS_BLOCK_PADDING_BUG
    SSL_CTX_set_options(ctx, SSL_OP_TLS_BLOCK_PADDING_BUG);
#endif

#ifdef SSL_OP_DONT_INSERT_EMPTY_FRAGMENTS
    SSL_CTX_set_options(ctx, SSL_OP_DONT_INSERT_EMPTY_FRAGMENTS);
#endif

    SSL_CTX_set_options(ctx, SSL_OP_SINGLE_DH_USE);

    if (tls) {
#if OPENSSL_VERSION_NUMBER >= 0x009080dfL
        /* only in 0.9.8m+ */
        SSL_CTX_clear_options(ctx,
            SSL_OP_NO_SSLv2|SSL_OP_NO_SSLv3|SSL_OP_NO_TLSv1);
#endif

        if (!(methods & NAI_SSL_SSLv2)) {
            SSL_CTX_set_options(ctx, SSL_OP_NO_SSLv2);
        }
        if (!(methods & NAI_SSL_SSLv3)) {
            SSL_CTX_set_options(ctx, SSL_OP_NO_SSLv3);
        }
        if (!(methods & NAI_SSL_TLSv1)) {
            SSL_CTX_set_options(ctx, SSL_OP_NO_TLSv1);
        }
#ifdef SSL_OP_NO_TLSv1_1
        SSL_CTX_clear_options(ctx, SSL_OP_NO_TLSv1_1);
        if (!(methods & NAI_SSL_TLSv1_1)) {
            SSL_CTX_set_options(ctx, SSL_OP_NO_TLSv1_1);
        }
#endif
#ifdef SSL_OP_NO_TLSv1_2
        SSL_CTX_clear_options(ctx, SSL_OP_NO_TLSv1_2);
        if (!(methods & NAI_SSL_TLSv1_2)) {
            SSL_CTX_set_options(ctx, SSL_OP_NO_TLSv1_2);
        }
#endif
#ifdef SSL_OP_NO_TLSv1_3
        SSL_CTX_clear_options(ctx, SSL_OP_NO_TLSv1_3);
        if (!(methods & NAI_SSL_TLSv1_3)) {
            SSL_CTX_set_options(ctx, SSL_OP_NO_TLSv1_3);
        }
#endif
#ifdef SSL_CTX_set_min_proto_version
        SSL_CTX_set_min_proto_version(ctx, 0);
        SSL_CTX_set_max_proto_version(ctx, TLS1_2_VERSION);
#endif

#ifdef TLS1_3_VERSION
        SSL_CTX_set_min_proto_version(ctx, 0);
        SSL_CTX_set_max_proto_version(ctx, TLS1_3_VERSION);
#endif
    } else {
#ifdef SSL_OP_NO_DTLSv1
        SSL_CTX_clear_options(ctx, SSL_OP_NO_DTLSv1);
        if (!(methods & NAI_SSL_DTLSv1)) {
            SSL_CTX_set_options(ctx, SSL_OP_NO_DTLSv1);
        }
#endif
#ifdef SSL_OP_NO_DTLSv1_2
        SSL_CTX_clear_options(ctx, SSL_OP_NO_DTLSv1_2);
        if (!(methods & NAI_SSL_DTLSv1_2)) {
            SSL_CTX_set_options(ctx, SSL_OP_NO_DTLSv1_2);
        }
#endif
#ifdef SSL_CTX_set_min_proto_version
        SSL_CTX_set_min_proto_version(ctx, 0);
        SSL_CTX_set_max_proto_version(ctx, DTLS1_2_VERSION);
#endif
    };

#ifdef SSL_OP_NO_COMPRESSION
    SSL_CTX_set_options(ctx, SSL_OP_NO_COMPRESSION);
#endif

#ifdef SSL_OP_NO_ANTI_REPLAY
    SSL_CTX_set_options(ctx, SSL_OP_NO_ANTI_REPLAY);
#endif

#ifdef SSL_OP_NO_CLIENT_RENEGOTIATION
    SSL_CTX_set_options(ctx, SSL_OP_NO_CLIENT_RENEGOTIATION);
#endif

#ifdef SSL_MODE_RELEASE_BUFFERS
    SSL_CTX_set_mode(ctx, SSL_MODE_RELEASE_BUFFERS);
#endif

#ifdef SSL_MODE_NO_AUTO_CHAIN
    SSL_CTX_set_mode(ctx, SSL_MODE_NO_AUTO_CHAIN);
#endif

    SSL_CTX_set_read_ahead(ctx, 1);

#ifdef SSL_R_CERT_CB_ERROR
    SSL_CTX_set_cert_cb(ctx, nai_ssl_cert_callback, ssl);
#endif

#ifdef SSL_CTRL_SET_TLSEXT_HOSTNAME
    SSL_CTX_set_tlsext_servername_callback(ctx, nai_ssl_server_callback);
#endif

    SSL_CTX_set_info_callback(ctx, nai_ssl_info_handle);


#if OPENSSL_VERSION_NUMBER >= 0x10000000L && !defined(LIBWOLFSSL_VERSION_HEX)
    SSL_CTX_set_cookie_verify_cb(ctx, nai_ssl_cookie_verify);
    SSL_CTX_set_cookie_generate_cb(ctx, nai_ssl_cookie_generate);
#endif

    /* ok */
    ssl->ctx = ctx;
    ssl->tls = tls;
    r = 0;

_end:
    return r;
};


nai_int_t nai_ssl_ctx_close(nai_ssl_ctx_t* ssl)
{
    X509* cert;
    X509* next;


    if (ssl->ctx) {
        cert = SSL_CTX_get_ex_data(ssl->ctx, nai_ssl.certificate);
        while (cert) {
            next = X509_get_ex_data(cert, nai_ssl.certificate_next);
            X509_free(cert);
            cert = next;
        };

        nai_ssl_ctx_cleanup_ocsp(ssl);

        SSL_CTX_free(ssl->ctx);
    };

    return 0;
}


nai_int_t nai_ssl_ctx_certificate(
    nai_ssl_ctx_t* ssl, 
    const nai_str_t* cert, const nai_str_t* key, 
    const nai_str_t* passwords, nai_int_t count)
{
    nai_int_t r;
#ifndef SSL_CTX_set0_chain
    nai_int_t n;
    X509* temp;
#endif
    nai_ssl_cert_t c = {0};
    nai_ssl_pkey_t k = {0};


    r = nai_ssl_load_certificate(ssl, &c, cert);
    if (r < 0) {
        nai_log_error(NAI_LOG_CORE, 
            nai_errno, "nai_ssl_load_certificate() failed");
        goto _end;
    };

    r = nai_ssl_load_certificate_key(ssl, &k, key, passwords, count);
    if (r < 0) {
        nai_log_error(NAI_LOG_CORE, 
            nai_errno, "nai_ssl_load_certificate_key() failed");
        goto _end;
    };

    if (SSL_CTX_use_certificate(ssl->ctx, c.cert) == 0) {
        nai_ssl_set_errno(EINVAL);
        nai_log_alert(NAI_LOG_CORE, 
            nai_errno, "SSL_CTX_use_certificate(\"%.*s\") failed", 
            nai_str_len(cert), nai_str(cert));
        r = -1;
        goto _end;
    }

    if (X509_set_ex_data(c.cert, nai_ssl.certificate_next,
        SSL_CTX_get_ex_data(ssl->ctx, nai_ssl.certificate)) == 0) {
        nai_ssl_set_errno(ENOMEM);
        nai_log_alert(NAI_LOG_CORE, 
            nai_errno, "X509_set_ex_data() failed");
        r = -1;
        goto _end;
    }

    if (SSL_CTX_set_ex_data(ssl->ctx, nai_ssl.certificate, c.cert) == 0) {
        nai_ssl_set_errno(ENOMEM);
        nai_log_alert(NAI_LOG_CORE, 
            nai_errno, "SSL_CTX_set_ex_data() failed");
        r = -1;
        goto _end;
    }

    /*
     * Note that x509 is not freed here, but will be instead freed in
     * ngx_ssl_cleanup_ctx().  This is because we need to preserve all
     * certificates to be able to iterate all of them through exdata
     * (ngx_ssl_certificate_index, ngx_ssl_next_certificate_index),
     * while OpenSSL can free a certificate if it is replaced with another
     * certificate of the same type.
     */
    c.cert = 0;

#ifdef SSL_CTX_set0_chain

    if (SSL_CTX_set0_chain(ssl->ctx, c.chain) == 0) {
        nai_ssl_set_errno(EINVAL);
        nai_log_alert(NAI_LOG_CORE, 
            nai_errno, "SSL_CTX_set0_chain(\"%.*s\") failed", 
            nai_str_len(cert), nai_str(cert));
        r = -1;
        goto _end;
    };

    c.chain = 0;

#else

    /* SSL_CTX_set0_chain() is only available in OpenSSL 1.0.2+ */
    n = sk_X509_num(c.chain);

    while (n --) {
        temp = sk_X509_shift(c.chain);
        if (SSL_CTX_add_extra_chain_cert(ssl->ctx, temp) == 0) {
            nai_ssl_set_errno(ENOMEM);
            nai_log_alert(NAI_LOG_CORE, 
                nai_errno, "SSL_CTX_add_extra_chain_cert(\"%.*s\") failed", 
                nai_str_len(cert), nai_str(cert));
            r = -1;
            goto _end;
        };
    };

    sk_X509_free(c.chain);
    c.chain = 0;

#endif

    if (SSL_CTX_use_PrivateKey(ssl->ctx, k.pkey) == 0) {
        nai_ssl_set_errno(EINVAL);
        nai_log_alert(NAI_LOG_CORE, 
            nai_errno, "SSL_CTX_use_PrivateKey(\"%.*s\") failed", 
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


nai_int_t nai_ssl_ctx_verify_depth(nai_ssl_ctx_t* ssl, nai_int_t depth)
{
    nai_int_t r;


    SSL_CTX_set_verify(ssl->ctx,
        SSL_CTX_get_verify_mode(ssl->ctx), nai_ssl_verify_callback);
    SSL_CTX_set_verify_depth(ssl->ctx, depth);
    r = 0;

    return r;
};

nai_int_t nai_ssl_ctx_set_verify(nai_ssl_ctx_t* ssl, nai_int_t mode)
{
    nai_int_t r;


    SSL_CTX_set_verify(ssl->ctx,
        mode, nai_ssl_verify_callback);
    r = 0;

    return r;
};

nai_int_t nai_ssl_ctx_prefer_server_ciphers(nai_ssl_ctx_t* ssl)
{
    nai_int_t r;


    SSL_CTX_set_options(ssl->ctx, SSL_OP_CIPHER_SERVER_PREFERENCE);
    r = 0;

    return r;
};


nai_int_t nai_ssl_ctx_client_certificate(
    nai_ssl_ctx_t* ssl, const nai_str_t* cert)
{
    nai_int_t r;
    nai_ssl_cert_crl_t c = {0};


    r = nai_ssl_load_cert_crl(ssl, &c, cert);
    if (r < 0) {
        nai_log_error(NAI_LOG_CORE, 
            nai_errno, "nai_ssl_load_cert_crl() failed");
        goto _end;
    };

    r = nai_ssl_ctx_add_cert_crl(ssl, &c, cert, NAI_SSL_ADD_CERT_NAME);
    if (r < 0) {
        nai_log_error(NAI_LOG_CORE, 
            nai_errno, "nai_ssl_ctx_add_cert_crl() failed");
        goto _end;
    };

_end:
    if (c.chain) {
        sk_X509_INFO_pop_free(c.chain, X509_INFO_free);
    };

    return r;
};


nai_int_t nai_ssl_ctx_trusted_certificate(
    nai_ssl_ctx_t* ssl, const nai_str_t* cert)
{
    nai_int_t r;
    nai_ssl_cert_crl_t c = {0};


    r = nai_ssl_load_cert_crl(ssl, &c, cert);
    if (r < 0) {
        nai_log_error(NAI_LOG_CORE, 
            nai_errno, "nai_ssl_load_cert_crl() failed");
        goto _end;
    };

    r = nai_ssl_ctx_add_cert_crl(ssl, &c, cert, NAI_SSL_ADD_CERT);
    if (r < 0) {
        nai_log_error(NAI_LOG_CORE, 
            nai_errno, "nai_ssl_ctx_add_cert_crl() failed");
        goto _end;
    };

_end:
    if (c.chain) {
        sk_X509_INFO_pop_free(c.chain, X509_INFO_free);
    };

    return r;
};


nai_int_t nai_ssl_ctx_crl(nai_ssl_ctx_t* ssl, const nai_str_t* path)
{
    nai_int_t r;
    nai_ssl_cert_crl_t c = {0};


    r = nai_ssl_load_cert_crl(ssl, &c, path);
    if (r < 0) {
        nai_log_error(NAI_LOG_CORE, 
            nai_errno, "nai_ssl_load_cert_crl() failed");
        goto _end;
    };

    r = nai_ssl_ctx_add_cert_crl(ssl, &c, path, NAI_SSL_ADD_CRL);
    if (r < 0) {
        nai_log_error(NAI_LOG_CORE, 
            nai_errno, "nai_ssl_ctx_add_cert_crl() failed");
        goto _end;
    };

_end:
    if (c.chain) {
        sk_X509_INFO_pop_free(c.chain, X509_INFO_free);
    };

    return r;
};


nai_int_t nai_ssl_ctx_dhparam(nai_ssl_ctx_t* ssl, const nai_str_t* data)
{
    nai_int_t r;
    DH* dh = 0;
    BIO* bio;


    bio = nai_ssl_open_bio(ssl, data);
    if (bio == 0) {
        nai_log_alert(NAI_LOG_CORE, 
            nai_errno, "nai_ssl_open_bio(\"%s\") failed", nai_str(data));
        r = -1;
        goto _end;
    };

    dh = PEM_read_bio_DHparams(bio, 0, 0, 0);
    if (dh == 0) {
        nai_ssl_set_errno(EINVAL);
        nai_log_alert(NAI_LOG_CORE, 
            nai_errno, "PEM_read_bio_DHparams(\"%s\") failed", nai_str(data));
        r = -1;
        goto _end;
    }

    SSL_CTX_set_tmp_dh(ssl->ctx, dh);
    r = 0;

_end:
    if (dh) {
        DH_free(dh);
    };
    if (bio) {
        BIO_free(bio);
    };
    return r;
};


nai_int_t nai_ssl_ctx_ciphers(nai_ssl_ctx_t* ssl, const char* ciphers)
{
    nai_int_t r;


    if (SSL_CTX_set_cipher_list(ssl->ctx, ciphers) == 0) {
        nai_ssl_set_errno(EINVAL);
        nai_log_alert(NAI_LOG_CORE, 
            nai_errno, "SSL_CTX_set_cipher_list(\"%s\") failed", ciphers);
        r = -1;
        goto _end;
    };


#if OPENSSL_VERSION_NUMBER < 0x10100001L && !defined(LIBRESSL_VERSION_NUMBER)
    /* a temporary 512-bit RSA key is required for export versions of MSIE */
    SSL_CTX_set_tmp_rsa_callback(ssl->ctx, nai_ssl_rsa_callback);
#endif

    r = 0;

_end:
    return r;
};


nai_int_t nai_ssl_ctx_ecdh_curve(nai_ssl_ctx_t* ssl, const char* name)
{
    nai_int_t r;

#ifdef OPENSSL_NO_ECDH

    r = 0;

#else

#if (defined SSL_CTX_set1_curves_list || defined SSL_CTRL_SET_CURVES_LIST)

    SSL_CTX_set_options(ssl->ctx, SSL_OP_SINGLE_ECDH_USE);

    if (nai_strcmp(name, "auto") == 0) {
#if SSL_CTRL_SET_ECDH_AUTO
        SSL_CTX_set_ecdh_auto(ssl->ctx, 1);
#endif
    } else {
        if (SSL_CTX_set1_curves_list(ssl->ctx, name) == 0) {
            nai_ssl_set_errno(EINVAL);
            nai_log_alert(NAI_LOG_CORE, 
                nai_errno, "SSL_CTX_set1_curves_list(\"%s\") failed", name);
            r = -1;
            goto _end;
        };
    };

#else

    nai_int_t nid;
    EC_KEY* ecdh;

    if (nai_strcmp(name, "auto") == 0) {
        name = "prime256v1";
    };

    nid = OBJ_sn2nid(name);
    if (nid == 0) {
        nai_ssl_set_errno(EINVAL);
        nai_log_alert(NAI_LOG_CORE, 
            nai_errno, "OBJ_sn2nid(\"%s\") failed", name);
        r = -1;
        goto _end;
    };

    ecdh = EC_KEY_new_by_curve_name(nid);
    if (ecdh == 0) {
        nai_ssl_set_errno(EINVAL);
        nai_log_alert(NAI_LOG_CORE, 
            nai_errno, "EC_KEY_new_by_curve_name(\"%s\") failed", name);
        r = -1;
        goto _end;
    };

    SSL_CTX_set_options(ssl->ctx, SSL_OP_SINGLE_ECDH_USE);
    SSL_CTX_set_tmp_ecdh(ssl->ctx, ecdh);
    EC_KEY_free(ecdh);

#endif

    r = 0;

_end:

#endif

    return r;
};


nai_int_t nai_ssl_ctx_enable_ktls(nai_ssl_ctx_t* ssl, nai_int_t on)
{
    nai_int_t r;


#if (NAI_HAVE_KTLS)
    if (on) {
        SSL_CTX_set_options(ssl->ctx, SSL_OP_ENABLE_KTLS);
        r = 1;
    } else {
        SSL_CTX_clear_options(ssl->ctx, SSL_OP_ENABLE_KTLS);
        r = 0;
    };
#else
    (void)ssl;
    (void)on;

    r = 0;
#endif


    return r;
};


nai_int_t nai_ssl_ctx_session_cache(
    nai_ssl_ctx_t* ssl, nai_int_t type, size_t cache_size)
{
    nai_int_t r;


    switch (type) {
    case NAI_SSL_SESSION_OFF:
        SSL_CTX_set_session_cache_mode(ssl->ctx, SSL_SESS_CACHE_OFF);
        break;
    case NAI_SSL_SESSION_NONE:
        SSL_CTX_set_session_cache_mode(ssl->ctx, 
            SSL_SESS_CACHE_SERVER | 
            SSL_SESS_CACHE_NO_AUTO_CLEAR | 
            SSL_SESS_CACHE_NO_INTERNAL_STORE);
        break;
    case NAI_SSL_SESSION_SERVER:
        SSL_CTX_set_session_cache_mode(ssl->ctx, 
            SSL_SESS_CACHE_SERVER | 
            SSL_SESS_CACHE_NO_INTERNAL);
        if (cache_size != 0) {
            SSL_CTX_sess_set_cache_size(
                ssl->ctx, cache_size);
        } else {
            SSL_CTX_sess_set_cache_size(
                ssl->ctx, SSL_SESSION_CACHE_MAX_SIZE_DEFAULT);
        };
        break;
    case NAI_SSL_SESSION_CLIENT:
        SSL_CTX_set_session_cache_mode(ssl->ctx, 
            SSL_SESS_CACHE_CLIENT |
            SSL_SESS_CACHE_NO_INTERNAL);
        break;
    default:
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    r = 0;

_end:
    return r;
};


nai_int_t nai_ssl_ctx_session_timeout(nai_ssl_ctx_t* ssl, nai_int_t sec)
{
    nai_int_t r;


    SSL_CTX_set_timeout(ssl->ctx, (long)sec);
    r = 0;

    return r;
};


nai_int_t nai_ssl_ctx_session_unique_id(nai_ssl_ctx_t* ssl, 
    const char* prefix, const nai_str_t* extra, nai_int_t count)
{
    nai_int_t r;
    nai_int_t n, size;
    X509* cert;
    X509_NAME* name;
    STACK_OF(X509_NAME)* list;
    EVP_MD_CTX* md;
    uint32_t len;
    uint8_t buf[EVP_MAX_MD_SIZE];

    /*
     * Session ID context is set based on the string provided,
     * the server certificates, and the client CA list.
     */

    md = EVP_MD_CTX_create();
    if (md == 0) {
        nai_ssl_set_errno(ENOMEM);
        nai_log_alert(NAI_LOG_CORE, 
            nai_errno, "EVP_MD_CTX_create() failed");
        r = -1;
        goto _end;
    };

    if (EVP_DigestInit_ex(md, EVP_sha1(), NULL) == 0) {
        nai_ssl_set_errno(EINVAL);
        nai_log_alert(NAI_LOG_CORE, 
            nai_errno, "EVP_DigestInit_ex() failed");
        r = -1;
        goto _end;
    };

    if (EVP_DigestUpdate(md, prefix, strlen(prefix)) == 0) {
        nai_ssl_set_errno(EINVAL);
        nai_log_alert(NAI_LOG_CORE, 
            nai_errno, "EVP_DigestUpdate() failed");
        r = -1;
        goto _end;
    };

    cert = SSL_CTX_get_ex_data(ssl->ctx, nai_ssl.certificate);
    for ( ; cert; ) {
        if (X509_digest(cert, EVP_sha1(), buf, &len) == 0) {
            nai_ssl_set_errno(EINVAL);
            nai_log_alert(NAI_LOG_CORE, 
                nai_errno, "X509_digest() failed");
            r = -1;
            goto _end;
        }

        if (EVP_DigestUpdate(md, buf, len) == 0) {
            nai_ssl_set_errno(EINVAL);
            nai_log_alert(NAI_LOG_CORE, 
                nai_errno, "EVP_DigestUpdate() failed");
            r = -1;
            goto _end;
        };

        cert = X509_get_ex_data(cert, nai_ssl.certificate_next);
    };

    for (n = 0; n < count; n ++) {
        if (EVP_DigestUpdate(md, 
            nai_str(&extra[n]), nai_str_len(&extra[n])) == 0) {
            nai_ssl_set_errno(EINVAL);
            nai_log_alert(NAI_LOG_CORE, 
                nai_errno, "EVP_DigestUpdate() failed");
            r = -1;
            goto _end;
        };
    };

    list = SSL_CTX_get_client_CA_list(ssl->ctx);

    if (list != 0) {
        size = sk_X509_NAME_num(list);
        for (n = 0; n < size; n ++) {
            name = sk_X509_NAME_value(list, n);

            if (X509_NAME_digest(name, EVP_sha1(), buf, &len) == 0) {
                nai_ssl_set_errno(EINVAL);
                nai_log_alert(NAI_LOG_CORE, 
                    nai_errno, "X509_NAME_digest() failed");
                r = -1;
                goto _end;
            };

            if (EVP_DigestUpdate(md, buf, len) == 0) {
                nai_ssl_set_errno(EINVAL);
                nai_log_alert(NAI_LOG_CORE, 
                    nai_errno, "EVP_DigestUpdate() failed");
                r = -1;
                goto _end;
            };
        }
    };

    if (EVP_DigestFinal_ex(md, buf, &len) == 0) {
        nai_ssl_set_errno(EINVAL);
        nai_log_alert(NAI_LOG_CORE, 
            nai_errno, "EVP_DigestFinal_ex() failed");
        r = -1;
        goto _end;
    };


    if (SSL_CTX_set_session_id_context(ssl->ctx, buf, len) == 0) {
        nai_ssl_set_errno(ENOMEM);
        nai_log_alert(NAI_LOG_CORE, 
            nai_errno, "SSL_CTX_set_session_id_context() failed");
        r = -1;
        goto _end;
    };

    r = 0;

_end:
    if (md) {
        EVP_MD_CTX_destroy(md);
    };
    return r;
};



nai_int_t nai_ssl_read_password(nai_array_t* a, const char* path)
{
    nai_int_t ec;
    nai_int_t eof;
    intptr_t r;
    intptr_t len;
    nai_fd_t fd;
    nai_str_t* s;
    char* p;
    char* pend;
    char* end;
    char buf[4096];


    fd = nai_file_open(path, NAI_O_RDONLY, 0);
    if (fd == NAI_FD_INVALID) {
        r = -1;
        goto _end;
    };

    p = buf;
    pend = buf;

    for (;;) {
        r = nai_file_read(fd, pend, sizeof(buf) - (pend-p));
        if (r < 0) {
            ec = nai_errno;
            goto _fail;
        };

        eof = r == 0;
        pend += r;
        for (;;) {
            end = nai_strnchr(p, pend-p, '\n');
            if (end == 0) {
                if (eof) {
                    end = pend;
                } else {
                    break;
                };
            };

            len = end - p;
            if (len && end[-1] == '\r') {
                len --;
            }

            if (len) {
                s = (nai_str_t*)nai_array_push(a);
                if (s == 0) {
                    ec = nai_errno;
                    goto _fail;
                };

                r = nai_str_dup(s, p, len, a->pool);
                if (r < 0) {
                    a->count --;
                    ec = nai_errno;
                    goto _fail;
                };
            };

            if (eof) {
                p = pend;
                break;
            };

            p = end + 1;
        };

        len = pend - p;
        if (len >= (intptr_t)sizeof(buf)) {
            ec = ERANGE;
            goto _fail;
        };

        if (eof) {
            break;
        };

        if (p != buf) {
            nai_memcpy(buf, p, len);
            pend = buf + len;
            p = buf;
        };
    };

    nai_file_close(fd);
    r = 0;


_end:
    return r;

_fail:
    nai_file_close(fd);
    nai_errno = ec;
    r = -1;
    goto _end;
};


void nai_ssl_cleanup_password(void* p)
{
    nai_int_t len;
    intptr_t i;
    char* buf;
    nai_str_t* s;
    nai_array_t* a;


    a = (nai_array_t*)p;
    s = (nai_str_t*)a->elts;
    for (i = 0; i < (intptr_t)a->count; i ++) {
        buf = nai_str(&s[i]);
        len = nai_str_len(&s[i]);
        if (buf == 0) {
            continue;
        };

        nai_memset(buf, 0, len);
        if (a->pool == 0) {
            nai_free(buf);
        };
    };

    nai_array_close(a);
    return;
};


nai_array_t* nai_ssl_dup_password(nai_pool_t* p, const nai_array_t* a)
{
    nai_int_t r;
    nai_int_t len;
    intptr_t i;
    char* buf;
    nai_str_t* s;
    nai_str_t* d;
    nai_array_t* n;


    n = (nai_array_t*)nai_palloc(p, sizeof(*n));
    if (n == 0) {
        goto _end;
    };

    nai_array_init(n, sizeof(*d), p);

    if (a->count <= 0) {
        goto _end;
    };

    d = (nai_str_t*)nai_array_push_n(n, a->count);
    if (d == 0) {
        goto _fail;
    };

    s = (nai_str_t*)a->elts;
    for (i = 0; i < (intptr_t)a->count; i ++) {
        buf = nai_str(&s[i]);
        len = nai_str_len(&s[i]);
        r = nai_str_dup(&d[i], buf, len, p);
        if (r < 0) {
            goto _fail;
        };
    };


    r = nai_pool_add_cleanup(p, nai_ssl_cleanup_password, n);
    if (r < 0) {
        n = 0;
        goto _end;
    };

_end:
    return n;

_fail:
    n = 0;
    goto _end;
};


nai_array_t* nai_ssl_load_password(nai_pool_t* p, const char* path)
{
    nai_int_t r;
    nai_array_t* n;


    n = (nai_array_t*)nai_palloc(p, sizeof(*n));
    if (n == 0) {
        goto _end;
    };

    nai_array_init(n, sizeof(nai_str_t), p);
    r = nai_ssl_read_password(n, path);
    if (r < 0) {
        goto _fail;
    };

    r = nai_pool_add_cleanup(p, nai_ssl_cleanup_password, n);
    if (r < 0) {
        goto _fail;
    };

_end:
    return n;

_fail:
    n = 0;
    goto _end;
};


nai_sult_t nai_ssl_command_password(
    nai_command_t* c, nai_command_args_t* args)
{
    nai_sult_t rc;
    nai_str_t path;
    nai_array_t* a;
    nai_array_t** ptr;


    ptr = (nai_array_t**)(args->data + c->offset);
    if (ptr[0]) {
        rc = nai_errno_to_sult(EPERM);
        nai_log_error(NAI_LOG_CORE, 
            nai_sult_to_errno(rc), 
            "command '%s' is dupilcate", c->name);
        goto _end;
    };

    rc = nai_value_dups(&args->argv[0], &path, args->pool_tmp);
    if (rc < 0) {
        nai_log_error(NAI_LOG_CORE, 
            nai_sult_to_errno(rc), 
            "command '%s' get string failed", c->name);
        goto _end;
    };

    a = nai_ssl_load_password(args->pool_tmp, nai_str(&path));
    if (a == 0) {
        rc = nai_sult_from_errno();
        nai_log_error(NAI_LOG_CORE, 
            nai_sult_to_errno(rc), 
            "nai_ssl_load_password('%s') failed", nai_str(&path));
        goto _end;
    };

    ptr[0] = a;
    rc = 0;

_end:
    return rc;
};


#endif


void nai_ssl_term()
{
#if (NAI_HAVE_SSL)
    nai_ssl_lib_term();
#endif

    return;
};


