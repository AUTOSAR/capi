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
/// @file       nai_httpc_ssl_module.c
/// @brief      
/// @details
/// @date       2021-09-01
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai/io/nai_ssl.h"

#if (NAI_HAVE_SSL)


#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_log.h"
#include "nai/service/nai_main.h"
#include "nai/service/nai_module.h"
#include "nai/service/nai_main_agent.h"
#include "nai/service/nai_main_dns.h"
#include "nai_httpc_core.h"


typedef struct nai_httpc_ssl_srv_s {
    nai_ssl_ctx_t* ctx;
    nai_ssl_store_t ocsp_store;
    nai_script_expn_t cert;
    nai_script_expn_t key;
    nai_array_t* passwords;
    nai_str_t name;
    nai_str_t ciphers;
    nai_str_t trusted_cert;
    nai_str_t crl;
    nai_str_t ocsp_uri;
    nai_int_t protocols;
    nai_int_t send_name;
    nai_int_t verify;
    nai_int_t verify_depth;
    nai_int_t ocsp;
    nai_int_t session_cache;
} nai_httpc_ssl_srv_t;


static nai_sult_t nai_httpc_ssl_preproc(nai_httpc_t* h);
static nai_sult_t nai_httpc_ssl_postproc(nai_httpc_t* h);
static nai_sult_t nai_httpc_ssl_server_alloc(
    nai_httpc_local_ctx_t* c, void** pv);
static nai_sult_t nai_httpc_ssl_server_merge(
    nai_httpc_local_ctx_t* c, void* s, void* up);


static nai_sult_t nai_httpc_ssl_load_certs(
    nai_httpc_local_ctx_t* c, nai_httpc_ssl_srv_t* srv);



static nai_command_optinfo_t  nai_httpc_ssl_protocols[] = {
    { nai_strconst("SSLv2"), NAI_SSL_SSLv2 },
    { nai_strconst("SSLv3"), NAI_SSL_SSLv3 },
    { nai_strconst("TLSv1"), NAI_SSL_TLSv1 },
    { nai_strconst("TLSv1.1"), NAI_SSL_TLSv1_1 },
    { nai_strconst("TLSv1.2"), NAI_SSL_TLSv1_2 },
    { nai_strconst("TLSv1.3"), NAI_SSL_TLSv1_3 },
    { nai_strnull(), 0 }
};

static nai_command_optinfo_t  nai_httpc_ssl_ocsp[] = {
    { nai_strconst("off"), NAI_SSL_OCSP_OFF },
    { nai_strconst("on"), NAI_SSL_OCSP_ON },
    { nai_strconst("leaf"), NAI_SSL_OCSP_LEAF },
    { nai_strnull(), 0 }
};

static nai_command_t nai_httpc_ssl_commands[] = {
    { "ssl_name", nai_command_set_string, 
        NAI_HTTPC_CLOC_MAGENT|NAI_HTTPC_CLOC_SRV|
        NAI_HTTPC_COPT_SRV|NAI_COPT_ARG1, 
        nai_offsetof(nai_httpc_ssl_srv_t, name), 
        NAI_OPT_ARRAY_STR },
    { "ssl_cert", nai_httpc_command_expn, 
        NAI_HTTPC_CLOC_MAGENT|NAI_HTTPC_CLOC_SRV|
        NAI_HTTPC_COPT_SRV|NAI_COPT_ARG1, 
        nai_offsetof(nai_httpc_ssl_srv_t, cert), 
        NAI_OPT_ARRAY_STR },
    { "ssl_key", nai_httpc_command_expn, 
        NAI_HTTPC_CLOC_MAGENT|NAI_HTTPC_CLOC_SRV|
        NAI_HTTPC_COPT_SRV|NAI_COPT_ARG1, 
        nai_offsetof(nai_httpc_ssl_srv_t, key), 
        NAI_OPT_ARRAY_STR },
    { "ssl_password_file", nai_ssl_command_password, 
        NAI_HTTPC_CLOC_MAGENT|NAI_HTTPC_CLOC_SRV|
        NAI_HTTPC_COPT_SRV|NAI_COPT_ARG1, 
        nai_offsetof(nai_httpc_ssl_srv_t, passwords), 0 },
    { "ssl_crl", nai_command_set_string, 
        NAI_HTTPC_CLOC_MAGENT|NAI_HTTPC_CLOC_SRV|
        NAI_HTTPC_COPT_SRV|NAI_COPT_ARG1, 
        nai_offsetof(nai_httpc_ssl_srv_t, crl), 0 },
    { "ssl_ciphers", nai_command_set_string, 
        NAI_HTTPC_CLOC_MAGENT|NAI_HTTPC_CLOC_SRV|
        NAI_HTTPC_COPT_SRV|NAI_COPT_ARG1, 
        nai_offsetof(nai_httpc_ssl_srv_t, ciphers), 0 },
    { "ssl_trusted_cert", nai_command_set_string, 
        NAI_HTTPC_CLOC_MAGENT|NAI_HTTPC_CLOC_SRV|
        NAI_HTTPC_COPT_SRV|NAI_COPT_ARG1, 
        nai_offsetof(nai_httpc_ssl_srv_t, trusted_cert), 0 },
    { "ssl_protocols", nai_command_set_opts, 
        NAI_HTTPC_CLOC_MAGENT|NAI_HTTPC_CLOC_SRV|
        NAI_HTTPC_COPT_SRV|NAI_COPT_ARG1MORE, 
        nai_offsetof(nai_httpc_ssl_srv_t, protocols), 
        nai_httpc_ssl_protocols },
    { "ssl_verify", nai_command_set_opts, 
        NAI_HTTPC_CLOC_MAGENT|NAI_HTTPC_CLOC_SRV|
        NAI_HTTPC_COPT_SRV|NAI_COPT_ARG1, 
        nai_offsetof(nai_httpc_ssl_srv_t, verify), 0 },
    { "ssl_verify_depth", nai_command_set_int, 
        NAI_HTTPC_CLOC_MAGENT|NAI_HTTPC_CLOC_SRV|
        NAI_HTTPC_COPT_SRV|NAI_COPT_ARG1, 
        nai_offsetof(nai_httpc_ssl_srv_t, verify_depth), 0 },
    { "ssl_ocsp", nai_command_set_opts, 
        NAI_HTTPC_CLOC_MAGENT|NAI_HTTPC_CLOC_SRV|
        NAI_HTTPC_COPT_SRV|NAI_COPT_ARG1, 
        nai_offsetof(nai_httpc_ssl_srv_t, ocsp), 
        nai_httpc_ssl_ocsp },
    { "ssl_ocsp_uri", nai_command_set_string, 
        NAI_HTTPC_CLOC_MAGENT|NAI_HTTPC_CLOC_SRV|
        NAI_HTTPC_COPT_SRV|NAI_COPT_ARG1, 
        nai_offsetof(nai_httpc_ssl_srv_t, ocsp_uri), 0 },
    { "ssl_session_cache", nai_command_set_opts, 
        NAI_HTTPC_CLOC_MAGENT|NAI_HTTPC_CLOC_SRV|
        NAI_HTTPC_COPT_SRV|NAI_COPT_ARG1, 
        nai_offsetof(nai_httpc_ssl_srv_t, session_cache), 0 },
    { 0 }
};


static nai_httpc_module_t nai_httpc_ssl_module_ext = {
    nai_httpc_ssl_preproc,              /* pre-processing */
    nai_httpc_ssl_postproc,             /* post-processing */
    0,                                  /* main alloc */
    0, 0,                               /* agent alloc and merge */
    nai_httpc_ssl_server_alloc,         /* server alloc */
    nai_httpc_ssl_server_merge,         /* server merge */
    0, 0,                               /* location alloc and merge */
};


nai_module_t nai_httpc_ssl_module = {
    NAI_MODULE_HEADER,
    NAI_MODULE_HTTPC, 
    nai_httpc_ssl_commands, 
    "httpc_ssl", &nai_httpc_ssl_module_ext, {
        0, 
    },
};



static intptr_t nai_httpc_ssl_get_full_path(
    void* ud, char* buf, size_t buflen, const char* path, size_t len)
{
    intptr_t r;
    nai_main_t* m;


    m = (nai_main_t*)ud;
    r = nai_main_copy_full_path(m, buf, buflen, path, len);

    return r;
};

static nai_ssl_ctx_ops_t nai_httpc_ssl_ctx_ops = {
    nai_httpc_ssl_get_full_path, 
};



static void nai_httpc_ssl_ctx_cleanup(void* p)
{
    nai_int_t r;
    nai_sult_t rc;
    nai_httpc_ssl_srv_t* ssl = (nai_httpc_ssl_srv_t*)p;


    r = nai_ssl_ctx_close(ssl->ctx);
    if (r < 0) {
        rc = nai_sult_from_errno();
        nai_log_error(NAI_LOG_HTTPC, 
            nai_sult_to_errno(rc), "nai_ssl_ctx_close() failed");
    };

    r = nai_ssl_store_close(&ssl->ocsp_store);
    if (r < 0) {
        rc = nai_sult_from_errno();
        nai_log_error(NAI_LOG_HTTPC, 
            nai_sult_to_errno(rc), "nai_ssl_store_close() failed");
    };
};


static nai_sult_t nai_httpc_ssl_preproc(nai_httpc_t* h)
{
    (void)h;
    return 0;
};


static nai_sult_t nai_httpc_ssl_postproc(nai_httpc_t* h)
{
    (void)h;
    return 0;
};


static nai_sult_t nai_httpc_ssl_server_alloc(
    nai_httpc_local_ctx_t* c, void** pv)
{
    nai_sult_t rc;
    nai_httpc_ssl_srv_t* s;


    (void)pv;

    s = (nai_httpc_ssl_srv_t*)nai_palloc(c->pool, sizeof(*s));
    if (s == 0) {
        rc = nai_sult_from_errno();
        nai_log_alert(NAI_LOG_HTTPC, 
            nai_sult_to_errno(rc), "alloc server conf of ssl failed");
        goto _end;
    };

    nai_ssl_store_init(&s->ocsp_store);
    nai_str_setn(&s->name);
    nai_str_setn(&s->trusted_cert);
    nai_str_setn(&s->crl);
    nai_str_setn(&s->ciphers);
    nai_str_setn(&s->ocsp_uri);
    nai_script_expn_init(&s->cert);
    nai_script_expn_init(&s->key);
    s->ctx = 0;
    s->passwords = 0;
    s->protocols = NAI_OPT_UNSET_VALUE;
    s->send_name = NAI_OPT_UNSET_VALUE;
    s->verify = NAI_OPT_UNSET_VALUE;
    s->verify_depth = NAI_OPT_UNSET_VALUE;
    s->ocsp = NAI_OPT_UNSET_VALUE;
    s->session_cache = NAI_OPT_UNSET_VALUE;

    pv[0] = s;
    rc = 0;

_end:
    return rc;
};


static nai_sult_t nai_httpc_ssl_server_merge(
    nai_httpc_local_ctx_t* c, void* s, void* up)
{
    nai_int_t r;
    nai_int_t inherit;
    nai_sult_t rc;
    nai_httpc_server_t* srv;
    nai_httpc_ssl_srv_t* conf = (nai_httpc_ssl_srv_t*)s;
    nai_httpc_ssl_srv_t* parent = (nai_httpc_ssl_srv_t*)up;


    inherit = 0;
    if (parent == 0) {
        if (conf->protocols == NAI_OPT_UNSET_VALUE) {
            conf->protocols = 
                NAI_SSL_TLSv1|NAI_SSL_TLSv1_1|NAI_SSL_TLSv1_2|NAI_SSL_TLSv1_3;
        };
        if (conf->send_name == NAI_OPT_UNSET_VALUE) {
            conf->send_name = 0;
        };
        if (conf->verify == NAI_OPT_UNSET_VALUE) {
            conf->verify = 0;
        };
        if (conf->verify_depth == NAI_OPT_UNSET_VALUE) {
            conf->verify_depth = 1;
        };
        if (conf->ocsp == NAI_OPT_UNSET_VALUE) {
            conf->ocsp = NAI_SSL_OCSP_OFF;
        };
        if (conf->session_cache == NAI_OPT_UNSET_VALUE) {
            conf->session_cache = 0;
        };
    } else {
        if (nai_str_len(&conf->name) == 0) {
            conf->name = parent->name;
        };
        if (nai_script_expn_is_null(&conf->cert)) {
            conf->cert = parent->cert;
            inherit ++;
        };
        if (nai_script_expn_is_null(&conf->key)) {
            conf->key = parent->key;
            inherit ++;
        };
        if (nai_str(&conf->trusted_cert) == 0) {
            conf->trusted_cert = parent->trusted_cert;
            inherit ++;
        };
        if (nai_str(&conf->crl) == 0) {
            conf->crl = parent->crl;
            inherit ++;
        };
        if (nai_str(&conf->ciphers) == 0) {
            conf->ciphers = parent->ciphers;
            inherit ++;
        };
        if (nai_str(&conf->ocsp_uri) == 0) {
            conf->ocsp_uri = parent->ocsp_uri;
            inherit ++;
        };
        if (conf->passwords == 0) {
            conf->passwords = parent->passwords;
            inherit ++;
        };
        if (conf->protocols == NAI_OPT_UNSET_VALUE) {
            conf->protocols = parent->protocols;
            inherit ++;
        };
        if (conf->send_name == NAI_OPT_UNSET_VALUE) {
            conf->send_name = parent->send_name;
        };
        if (conf->verify == NAI_OPT_UNSET_VALUE) {
            conf->verify = parent->verify;
            inherit ++;
        };
        if (conf->verify_depth == NAI_OPT_UNSET_VALUE) {
            conf->verify_depth = parent->verify_depth;
            inherit ++;
        };
        if (conf->ocsp == NAI_OPT_UNSET_VALUE) {
            conf->ocsp = parent->ocsp;
            inherit ++;
        };
        if (conf->session_cache == NAI_OPT_UNSET_VALUE) {
            conf->session_cache = parent->session_cache;
            inherit ++;
        };
    };

    if (!c->done) {
        rc = 0;
        goto _end;
    };

    if (inherit >= 12) {
        conf->ctx = parent->ctx;
        rc = 0;
        goto _end;
    };

    if (!nai_script_expn_is_empty(&conf->cert)) {
        if (nai_script_expn_is_empty(&conf->key)) {
            rc = nai_errno_to_sult(EINVAL);
            nai_log_error(NAI_LOG_HTTPC, 
                nai_sult_to_errno(rc), "ssl_cert %s require a valid ssl_key", 
                nai_str(&conf->cert.value));
            goto _end;
        };
    };

    conf->ctx = (nai_ssl_ctx_t*)nai_palloc(c->pool, sizeof(*conf->ctx));
    if (conf->ctx == 0) {
        rc = nai_sult_from_errno();
        nai_log_alert(NAI_LOG_HTTPC, 
            nai_sult_to_errno(rc), "allocate ssl context failed");
        goto _end;
    };

    nai_ssl_ctx_init(conf->ctx);
    nai_ssl_ctx_set_ops(conf->ctx, 
        &nai_httpc_ssl_ctx_ops, c->service->main);


    r = nai_pool_add_cleanup(c->pool, nai_httpc_ssl_ctx_cleanup, conf);
    if (r < 0) {
        rc = nai_sult_from_errno();
        nai_log_error(NAI_LOG_HTTPC, 
            nai_sult_to_errno(rc), "ssl context add cleanup failed");
        goto _end;
    };

    r = nai_ssl_ctx_open(conf->ctx, conf->protocols);
    if (r < 0) {
        rc = nai_sult_from_errno();
        nai_log_error(NAI_LOG_HTTPC, 
            nai_sult_to_errno(rc), "nai_ssl_ctx_open() failed");
        goto _end;
    };

    /* set cert */
    rc = nai_httpc_ssl_load_certs(c, conf);
    if (rc < 0) {
        goto _end;
    };

    if (nai_str_len(&conf->ciphers) > 0) {
        r = nai_ssl_ctx_ciphers(conf->ctx, nai_str(&conf->ciphers));
        if (r < 0) {
            rc = nai_sult_from_errno();
            nai_log_error(NAI_LOG_HTTPC, 
                nai_sult_to_errno(rc), "nai_ssl_ctx_ciphers() failed");
            goto _end;
        };
    };
    if (nai_str_len(&conf->crl) > 0) {
        r = nai_ssl_ctx_crl(conf->ctx, &conf->crl);
        if (r < 0) {
            rc = nai_sult_from_errno();
            nai_log_error(NAI_LOG_HTTPC, 
                nai_sult_to_errno(rc), "nai_ssl_ctx_crl('%s') failed", 
                nai_str(&conf->crl));
            goto _end;
        };
    };


    r = nai_ssl_ctx_verify_depth(conf->ctx, conf->verify_depth);
    if (r < 0) {
        rc = nai_sult_from_errno();
        nai_log_error(NAI_LOG_HTTPC, 
            nai_sult_to_errno(rc), "nai_ssl_ctx_verify_depth() failed");
        goto _end;
    };

    if (conf->verify) {
        if (nai_str_len(&conf->trusted_cert) <= 0) {
            rc = nai_errno_to_sult(EINVAL);
            nai_log_error(NAI_LOG_HTTPC, 
                nai_sult_to_errno(rc), 
                "require a valid trusted certificate to enable ssl_verify");
            goto _end;
        };

        r = nai_ssl_ctx_trusted_certificate(conf->ctx, &conf->trusted_cert);
        if (r < 0) {
            rc = nai_sult_from_errno();
            nai_log_error(NAI_LOG_HTTPC, 
                nai_sult_to_errno(rc), 
                "nai_ssl_ctx_trusted_certificate('%s') failed", 
                nai_str(&conf->trusted_cert));
            goto _end;
        };
    };

    if (conf->ocsp) {
        srv = nai_httpc_get_server(c);
        r = nai_ssl_ctx_conn_attr(conf->ctx, &srv->conn);
        if (r < 0) {
            rc = nai_sult_from_errno();
            nai_log_error(NAI_LOG_HTTPC, 
                nai_sult_to_errno(rc), 
                "nai_ssl_ctx_conn_attr() failed");
            goto _end;
        };

        r = nai_ssl_ctx_ocsp(conf->ctx, conf->ocsp, &conf->ocsp_uri);
        if (r < 0) {
            rc = nai_sult_from_errno();
            nai_log_error(NAI_LOG_HTTPC, 
                nai_sult_to_errno(rc), 
                "nai_ssl_ctx_ocsp('%d', '%s') failed", 
                conf->ocsp, nai_str(&conf->ocsp_uri));
            goto _end;
        };

        r = nai_ssl_store_open(&conf->ocsp_store, 16*1024);
        if (r < 0) {
            rc = nai_sult_from_errno();
            nai_log_error(NAI_LOG_HTTPC, 
                nai_sult_to_errno(rc), 
                "nai_ssl_store_open() failed");
            goto _end;
        };

        r = nai_ssl_ctx_ocsp_store(conf->ctx, &conf->ocsp_store);
        if (r < 0) {
            rc = nai_sult_from_errno();
            nai_log_error(NAI_LOG_HTTPC, 
                nai_sult_to_errno(rc), 
                "nai_ssl_ctx_ocsp_store() failed");
            goto _end;
        };
    };


    if (conf->session_cache) {
        r = nai_ssl_ctx_session_cache(conf->ctx, NAI_SSL_SESSION_CLIENT, 0);
        if (r < 0) {
            rc = nai_sult_from_errno();
            nai_log_error(NAI_LOG_HTTPC, 
                nai_sult_to_errno(rc), "nai_ssl_ctx_session_cache() failed");
            goto _end;
        };
    };

    rc = 0;

_end:
    return rc;
};


static nai_sult_t nai_httpc_ssl_load_certs(
    nai_httpc_local_ctx_t* c, nai_httpc_ssl_srv_t* srv)
{
    nai_int_t r;
    nai_int_t passcount;
    nai_str_t* passwords;
    nai_array_t* passdup;
    nai_sult_t rc;


    if (nai_script_expn_is_empty(&srv->cert)) {
        rc = 0;
        goto _end;
    };

    if (nai_script_expn_is_variable(&srv->cert) || 
        nai_script_expn_is_variable(&srv->key)) {

        if (srv->passwords && 
            srv->passwords->pool != c->pool) {
            passdup = nai_ssl_dup_password(c->pool, srv->passwords);
            if (passdup == 0) {
                rc = nai_sult_from_errno();
                nai_log_error(NAI_LOG_HTTPC, 
                    nai_sult_to_errno(rc), "nai_ssl_dup_password failed");
                goto _end;
            };

            srv->passwords = passdup;
        };

    } else {

        if (srv->passwords) {
            passcount = (nai_int_t)srv->passwords->count;
            passwords = (nai_str_t*)srv->passwords->elts;
        } else {
            passcount = 0;
            passwords = 0;
        };

        r = nai_ssl_ctx_certificate(srv->ctx, 
            &srv->cert.value, &srv->key.value, passwords, passcount);
        if (r < 0) {
            rc = nai_sult_from_errno();
            nai_log_error(NAI_LOG_HTTPC, 
                nai_sult_to_errno(rc), 
                "nai_ssl_ctx_certificate('%s', '%s') failed", 
                nai_str(&srv->cert.value), nai_str(&srv->key.value));
            goto _end;
        };
    };

    rc = 0;

_end:
    return rc;
};



typedef struct nai_httpc_ssl_s {
    nai_ssl_t ssl;
    nai_str_t name;
    nai_buf_t* buf;
} nai_httpc_ssl_t;


static nai_ssl_ops_t nai_httpc_ssl_head_ops;
static nai_ssl_ops_t nai_httpc_ssl_ops;


static void nai_httpc_ssl_cleanup(void* up)
{
    nai_httpc_ssl_t* s = (nai_httpc_ssl_t*)up;
    nai_ssl_close(&s->ssl);
};


static void* nai_httpc_ssl_realloc(void* ud, void* ptr, size_t size)
{
    void* r;
    nai_httpc_connection_t* c;


    c = (nai_httpc_connection_t*)ud;
    if (ptr) {
        nai_bufpool_free(&c->c.bufpool, ptr);
    };

    if (size > 0) {
        r = nai_bufpool_alloc(&c->c.bufpool, size);
    } else {
        r = 0;
    };

    return r;
};


static intptr_t nai_httpc_ssl_read(void* ud, void* buf, size_t size)
{
    intptr_t r;
    nai_httpc_ssl_t* s;
    nai_httpc_connection_t* c;


    c = (nai_httpc_connection_t*)ud;
    s = (nai_httpc_ssl_t*)c->ssl;
    if (s->buf == 0) {
        r = nai_stream_read(&c->c.str, buf, size);
    } else {
        r = nai_buf_size(s->buf);
        if (r > (intptr_t)size) {
            r = size;
        };

        nai_memcpy(buf, nai_buf_ptr(s->buf), r);

        nai_buf_rcommit(s->buf, r);
        if (nai_buf_size(s->buf) <= 0) {
            nai_buf_close(s->buf);
            s->buf = 0;

            /* changed to the cb which without ssl_head handle */
            c->ssl->ops = &nai_httpc_ssl_ops;
        };
    };

    return r;
};


static nai_ssl_ops_t nai_httpc_ssl_ops = {
    0, /* lookup cert */
    0, /* lookup server */
    0, 0, /* cookies */
    nai_ssl_iobase_poll, 
    nai_ssl_iobase_want, 
    nai_ssl_iobase_emit, 
    nai_ssl_iobase_get_loop, 
    nai_ssl_iobase_get_opt, 
    nai_ssl_iobase_get_peer, 
    nai_httpc_ssl_realloc, 
    (nai_ssl_read_f)nai_stream_read, 
    (nai_ssl_write_f)nai_stream_write, 
    (nai_ssl_recvm_f)nai_stream_recvm, 
    (nai_ssl_sendm_f)nai_stream_sendm, 
    (nai_ssl_sendfile_f)nai_stream_sendfile, 
    0, 
};

static nai_ssl_ops_t nai_httpc_ssl_head_ops = {
    0, /* lookup cert */
    0, /* lookup server */
    0, 0, /* cookies */
    nai_ssl_iobase_poll, 
    nai_ssl_iobase_want, 
    nai_ssl_iobase_emit, 
    nai_ssl_iobase_get_loop, 
    nai_ssl_iobase_get_opt, 
    nai_ssl_iobase_get_peer, 
    nai_httpc_ssl_realloc, 
    nai_httpc_ssl_read, 
    (nai_ssl_write_f)nai_stream_write, 
    (nai_ssl_recvm_f)nai_stream_recvm, 
    (nai_ssl_sendm_f)nai_stream_sendm, 
    (nai_ssl_sendfile_f)nai_stream_sendfile, 
    0, 
};


nai_sult_t nai_httpc_ssl_create(
    nai_httpc_connection_t* c, nai_buf_t* head)
{
    nai_int_t r;
    nai_int_t passcount;
    nai_sult_t rc;
    nai_str_t name;
    nai_str_t cert;
    nai_str_t key;
    nai_str_t* passwords;
    nai_ssl_ops_t* ops;
    nai_httpc_ssl_t* s;
    nai_httpc_ssl_srv_t* srv;
    nai_httpc_request_t* req;
    nai_httpc_location_t* l;


    /* set nodelay for ssl */
    l = nai_httpc_get_location(c->proto->request);
    if (c->c.tcp && 
        c->c.tcp_nodelay == 0 && l->tcp_nodelay) {
        rc = nai_httpc_connection_tcp_nodelay(c, 1);
        if (rc < 0) {
            nai_log_alert(NAI_LOG_HTTPC, 
                nai_sult_to_errno(rc), "set nodelay option failed");
            goto _end;
        };
    };


    /* create a ssl */
    s = (nai_httpc_ssl_t*)nai_palloc(c->c.pool, sizeof(*s));
    if (s == 0) {
        rc = nai_sult_from_errno();
        nai_log_alert(NAI_LOG_HTTPC, 
            nai_sult_to_errno(rc), "alloc nai_ssl_t for httpc failed");
        goto _end;
    };

    if (head) {
        ops = &nai_httpc_ssl_head_ops;
    } else {
        ops = &nai_httpc_ssl_ops;
    };
    s->buf = head;
    nai_ssl_init(&s->ssl);
    nai_ssl_set_ops(&s->ssl, ops, c);

    /* add cleanup */
    r = nai_pool_add_cleanup(c->c.pool, nai_httpc_ssl_cleanup, s);
    if (r < 0) {
        rc = nai_sult_from_errno();
        nai_log_alert(NAI_LOG_HTTPC, 
            nai_sult_to_errno(rc), "nai_pool_add_cleanup() failed");
        goto _end;
    };


    /* open ssl */
    srv = nai_httpc_get_server_conf(c, nai_httpc_ssl_module);
    r = nai_ssl_open(&s->ssl, srv->ctx, 1);
    if (r < 0) {
        rc = nai_sult_from_errno();
        nai_log_alert(NAI_LOG_HTTPC, 
            nai_sult_to_errno(rc), "nai_ssl_open(connect) failed");
        goto _end;
    };

    /* set cert and key */
    if (!nai_script_expn_is_empty(&srv->cert) && (
        nai_script_expn_is_variable(&srv->cert) || 
        nai_script_expn_is_variable(&srv->key))) {
        req = c->proto->request;
        rc = nai_httpc_expn_value(req, &srv->cert, &cert, 0);
        if (rc < 0) {
            nai_log_error(NAI_LOG_HTTPC, 
                nai_sult_to_errno(rc), 
                "get value of ssl cert '%s' failed", 
                nai_str(&srv->key.value));
            goto _end;
        };

        rc = nai_httpc_expn_value(req, &srv->key, &key, 0);
        if (rc < 0) {
            nai_log_error(NAI_LOG_HTTPC, 
                nai_sult_to_errno(rc), 
                "get value of ssl key '%s' failed", 
                nai_str(&srv->key.value));
            goto _end;
        };

        if (srv->passwords) {
            passcount = (nai_int_t)srv->passwords->count;
            passwords = (nai_str_t*)srv->passwords->elts;
        } else {
            passcount = 0;
            passwords = 0;
        };

        r = nai_ssl_ctx_certificate(
            srv->ctx, &cert, &key, passwords, passcount);
        if (r < 0) {
            rc = nai_sult_from_errno();
            nai_log_error(NAI_LOG_HTTPC, 
                nai_sult_to_errno(rc), 
                "nai_ssl_certificate('%s', '%s') failed", 
                nai_str(&cert), nai_str(&key));
            goto _end;
        };
    };


    /* set verify name */
    if (srv->verify || srv->send_name) {
        if (nai_str_len(&srv->name) > 0) {
            name = srv->name;
        } else {
            name = c->server->host;
        };

        s->name = name;
        r = nai_ssl_set_host(&s->ssl, &s->name);
        if (r < 0) {
            rc = nai_sult_from_errno();
            nai_log_alert(NAI_LOG_HTTPC, 
                nai_sult_to_errno(rc), "nai_ssl_set_host() failed");
            goto _end;
        };
    };

    c->ssl = &s->ssl;
    rc = 0;

_end:
    return rc;
};


nai_sult_t nai_httpc_ssl_verify_cert(
    nai_httpc_connection_t* c)
{
    nai_int_t r;
    nai_int_t ec;
    nai_sult_t rc;
    nai_httpc_ssl_t* s;
    nai_httpc_ssl_srv_t* srv;


    s = (nai_httpc_ssl_t*)c->ssl;
    srv = nai_httpc_get_server_conf(c, nai_httpc_ssl_module);

    if (srv->verify) {
        r = nai_ssl_verify(&s->ssl, NAI_SSL_VERIFY_ON);
        if (r < 0) {
            ec = nai_errno;
            if (ec == ENOENT) {
                rc = NAI_HTTPC_NO_CERT;
            } else {
                rc = NAI_HTTPC_CERT_ERROR;
            };
            goto _end;
        };
    };

    rc = 0;

_end:
    return rc;
};


#endif


