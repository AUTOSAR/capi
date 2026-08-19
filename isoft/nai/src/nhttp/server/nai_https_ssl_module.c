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
/// @file       nai_https_ssl_module.c
/// @brief      
/// @details
/// @date       2021-03-03
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
#include "nai_https_core.h"


typedef struct nai_https_ssl_srv_s {
    nai_ssl_ctx_t* ctx;
    nai_ssl_store_t ocsp_store;
    nai_array_t certs;
    nai_array_t certs_expn;
    nai_array_t keys;
    nai_array_t keys_expn;
    nai_array_t* passwords;
    nai_str_t dhparam;
    nai_str_t ecdh_curve;
    nai_str_t client_cert;
    nai_str_t trusted_cert;
    nai_str_t crl;
    nai_str_t ciphers;
    nai_str_t ocsp_uri;
    nai_int_t protocols;
    nai_int_t prefer_server_ciphers;
    nai_int_t early_data;
    nai_int_t reject_handshake;
    nai_int_t verify;
    nai_int_t verify_depth;
    nai_int_t ocsp;
    nai_int_t session_type;
    size_t session_cache_size;
    time_t session_timeout;
} nai_https_ssl_srv_t;


static nai_sult_t nai_https_ssl_preproc(nai_https_t* h);
static nai_sult_t nai_https_ssl_postproc(nai_https_t* h);
static nai_sult_t nai_https_ssl_server_alloc(
    nai_https_local_ctx_t* c, void** pv);
static nai_sult_t nai_https_ssl_server_merge(
    nai_https_local_ctx_t* c, void* s, void* up);


static nai_sult_t nai_https_ssl_load_certs(
    nai_https_local_ctx_t* c, nai_https_ssl_srv_t* ssl);



static nai_command_optinfo_t  nai_https_ssl_protocols[] = {
    { nai_strconst("SSLv2"), NAI_SSL_SSLv2 },
    { nai_strconst("SSLv3"), NAI_SSL_SSLv3 },
    { nai_strconst("TLSv1"), NAI_SSL_TLSv1 },
    { nai_strconst("TLSv1.1"), NAI_SSL_TLSv1_1 },
    { nai_strconst("TLSv1.2"), NAI_SSL_TLSv1_2 },
    { nai_strconst("TLSv1.3"), NAI_SSL_TLSv1_3 },
    { nai_strnull(), 0 }
};

static nai_command_optinfo_t  nai_https_ssl_verify[] = {
    { nai_strconst("off"), NAI_SSL_VERIFY_OFF },
    { nai_strconst("on"), NAI_SSL_VERIFY_ON },
    { nai_strconst("optional"), NAI_SSL_VERIFY_OPTIONAL },
    { nai_strconst("optional_no_ca"), NAI_SSL_VERIFY_NO_CA },
    { nai_strnull(), 0 }
};

static nai_command_optinfo_t  nai_https_ssl_ocsp[] = {
    { nai_strconst("off"), NAI_SSL_OCSP_OFF },
    { nai_strconst("on"), NAI_SSL_OCSP_ON },
    { nai_strconst("leaf"), NAI_SSL_OCSP_LEAF },
    { nai_strnull(), 0 }
};

static nai_command_optinfo_t  nai_https_ssl_session[] = {
    { nai_strconst("off"), NAI_SSL_SESSION_OFF }, 
    { nai_strconst("none"), NAI_SSL_SESSION_NONE }, 
    { nai_strconst("builtin"), NAI_SSL_SESSION_SERVER }, 
};

static nai_command_t nai_https_ssl_commands[] = {
    { "ssl_certs", nai_command_set_array, 
        NAI_HTTPS_CLOC_MAIN|NAI_HTTPS_CLOC_SRV|
        NAI_HTTPS_COPT_SRV|NAI_COPT_ARG1MORE, 
        nai_offsetof(nai_https_ssl_srv_t, certs), 
        NAI_OPT_ARRAY_STR },
    { "ssl_keys", nai_command_set_array, 
        NAI_HTTPS_CLOC_MAIN|NAI_HTTPS_CLOC_SRV|
        NAI_HTTPS_COPT_SRV|NAI_COPT_ARG1MORE, 
        nai_offsetof(nai_https_ssl_srv_t, certs), 
        NAI_OPT_ARRAY_STR },
    { "ssl_password_file", nai_ssl_command_password, 
        NAI_HTTPS_CLOC_MAIN|NAI_HTTPS_CLOC_SRV|
        NAI_HTTPS_COPT_SRV|NAI_COPT_ARG1, 
        nai_offsetof(nai_https_ssl_srv_t, passwords), 0 },
    { "ssl_dhparam", nai_command_set_string, 
        NAI_HTTPS_CLOC_MAIN|NAI_HTTPS_CLOC_SRV|
        NAI_HTTPS_COPT_SRV|NAI_COPT_ARG1, 
        nai_offsetof(nai_https_ssl_srv_t, dhparam), 0 },
    { "ssl_ecdh_curve", nai_command_set_string, 
        NAI_HTTPS_CLOC_MAIN|NAI_HTTPS_CLOC_SRV|
        NAI_HTTPS_COPT_SRV|NAI_COPT_ARG1, 
        nai_offsetof(nai_https_ssl_srv_t, ecdh_curve), 0 },
    { "ssl_crl", nai_command_set_string, 
        NAI_HTTPS_CLOC_MAIN|NAI_HTTPS_CLOC_SRV|
        NAI_HTTPS_COPT_SRV|NAI_COPT_ARG1, 
        nai_offsetof(nai_https_ssl_srv_t, crl), 0 },
    { "ssl_ciphers", nai_command_set_string, 
        NAI_HTTPS_CLOC_MAIN|NAI_HTTPS_CLOC_SRV|
        NAI_HTTPS_COPT_SRV|NAI_COPT_ARG1, 
        nai_offsetof(nai_https_ssl_srv_t, ciphers), 0 },
    { "ssl_client_cert", nai_command_set_string, 
        NAI_HTTPS_CLOC_MAIN|NAI_HTTPS_CLOC_SRV|
        NAI_HTTPS_COPT_SRV|NAI_COPT_ARG1, 
        nai_offsetof(nai_https_ssl_srv_t, client_cert), 0 },
    { "ssl_trusted_cert", nai_command_set_string, 
        NAI_HTTPS_CLOC_MAIN|NAI_HTTPS_CLOC_SRV|
        NAI_HTTPS_COPT_SRV|NAI_COPT_ARG1, 
        nai_offsetof(nai_https_ssl_srv_t, trusted_cert), 0 },
    { "ssl_protocols", nai_command_set_opts, 
        NAI_HTTPS_CLOC_MAIN|NAI_HTTPS_CLOC_SRV|
        NAI_HTTPS_COPT_SRV|NAI_COPT_ARG1MORE, 
        nai_offsetof(nai_https_ssl_srv_t, protocols), 
        nai_https_ssl_protocols },
    { "ssl_early_data", nai_command_set_opts, 
        NAI_HTTPS_CLOC_MAIN|NAI_HTTPS_CLOC_SRV|
        NAI_HTTPS_COPT_SRV|NAI_COPT_ARG1, 
        nai_offsetof(nai_https_ssl_srv_t, early_data), 0 },
    { "ssl_perfer_server_ciphers", nai_command_set_opts, 
        NAI_HTTPS_CLOC_MAIN|NAI_HTTPS_CLOC_SRV|
        NAI_HTTPS_COPT_SRV|NAI_COPT_ARG1, 
        nai_offsetof(nai_https_ssl_srv_t, prefer_server_ciphers), 0 },
    { "ssl_verify", nai_command_set_opts, 
        NAI_HTTPS_CLOC_MAIN|NAI_HTTPS_CLOC_SRV|
        NAI_HTTPS_COPT_SRV|NAI_COPT_ARG1, 
        nai_offsetof(nai_https_ssl_srv_t, verify), 
        nai_https_ssl_verify },
    { "ssl_verify_depth", nai_command_set_opts, 
        NAI_HTTPS_CLOC_MAIN|NAI_HTTPS_CLOC_SRV|
        NAI_HTTPS_COPT_SRV|NAI_COPT_ARG1, 
        nai_offsetof(nai_https_ssl_srv_t, verify_depth), 0 },
    { "ssl_ocsp", nai_command_set_opts, 
        NAI_HTTPS_CLOC_MAIN|NAI_HTTPS_CLOC_SRV|
        NAI_HTTPS_COPT_SRV|NAI_COPT_ARG1, 
        nai_offsetof(nai_https_ssl_srv_t, ocsp), 
        nai_https_ssl_ocsp },
    { "ssl_ocsp_uri", nai_command_set_string, 
        NAI_HTTPS_CLOC_MAIN|NAI_HTTPS_CLOC_SRV|
        NAI_HTTPS_COPT_SRV|NAI_COPT_ARG1, 
        nai_offsetof(nai_https_ssl_srv_t, ocsp_uri), 0 },
    { "ssl_session_cache", nai_command_set_opts, 
        NAI_HTTPS_CLOC_MAIN|NAI_HTTPS_CLOC_SRV|
        NAI_HTTPS_COPT_SRV|NAI_COPT_ARG1, 
        nai_offsetof(nai_https_ssl_srv_t, session_type), 
        nai_https_ssl_session },
    { "ssl_session_timeout", nai_command_set_sec, 
        NAI_HTTPS_CLOC_MAIN|NAI_HTTPS_CLOC_SRV|
        NAI_HTTPS_COPT_SRV|NAI_COPT_ARG1, 
        nai_offsetof(nai_https_ssl_srv_t, session_timeout), 
        nai_https_ssl_session },
    { "ssl_reject_handshake", nai_command_set_opts, 
        NAI_HTTPS_CLOC_MAIN|NAI_HTTPS_CLOC_SRV|
        NAI_HTTPS_COPT_SRV|NAI_COPT_ARG1, 
        nai_offsetof(nai_https_ssl_srv_t, reject_handshake), 0 },
    { 0 }, 
};


static nai_https_module_t nai_https_ssl_module_ext = {
    nai_https_ssl_preproc,              /* pre-processing */
    nai_https_ssl_postproc,             /* post-processing */
    0,                                  /* main alloc */
    nai_https_ssl_server_alloc,         /* server alloc */
    nai_https_ssl_server_merge,         /* server merge */
    0, 0,                               /* location alloc */
};


nai_module_t nai_https_ssl_module = {
    NAI_MODULE_HEADER,
    NAI_MODULE_HTTPS, 
    nai_https_ssl_commands, 
    "https_ssl", &nai_https_ssl_module_ext, {
        0, 
    },
};



static intptr_t nai_https_ssl_get_full_path(
    void* ud, char* buf, size_t buflen, const char* path, size_t len)
{
    intptr_t r;
    nai_main_t* m;


    m = (nai_main_t*)ud;
    r = nai_main_copy_full_path(m, buf, buflen, path, len);

    return r;
};

static nai_ssl_ctx_ops_t nai_https_ssl_ctx_ops = {
    nai_https_ssl_get_full_path, 
};



static void nai_https_ssl_ctx_cleanup(void* p)
{
    nai_int_t r;
    nai_sult_t rc;
    nai_https_ssl_srv_t* ssl = (nai_https_ssl_srv_t*)p;


    r = nai_ssl_ctx_close(ssl->ctx);
    if (r < 0) {
        rc = nai_sult_from_errno();
        nai_log_error(NAI_LOG_HTTPS, 
            nai_sult_to_errno(rc), "nai_ssl_ctx_close() failed");
    };

    r = nai_ssl_store_close(&ssl->ocsp_store);
    if (r < 0) {
        rc = nai_sult_from_errno();
        nai_log_error(NAI_LOG_HTTPS, 
            nai_sult_to_errno(rc), "nai_ssl_store_close() failed");
    };
};


static nai_sult_t nai_https_ssl_preproc(nai_https_t* h)
{
    (void)h;
    return 0;
};


static nai_sult_t nai_https_ssl_postproc(nai_https_t* h)
{
    (void)h;
    return 0;
};


static nai_sult_t nai_https_ssl_server_alloc(
    nai_https_local_ctx_t* c, void** pv)
{
    nai_sult_t rc;
    nai_https_ssl_srv_t* s;


    (void)pv;

    s = (nai_https_ssl_srv_t*)nai_palloc(c->pool, sizeof(*s));
    if (s == 0) {
        rc = nai_sult_from_errno();
        nai_log_alert(NAI_LOG_HTTPS, 
            nai_sult_to_errno(rc), "alloc server conf of ssl failed");
        goto _end;
    };

    nai_ssl_store_init(&s->ocsp_store);
    nai_str_setn(&s->client_cert);
    nai_str_setn(&s->trusted_cert);
    nai_str_setn(&s->dhparam);
    nai_str_setn(&s->ecdh_curve);
    nai_str_setn(&s->crl);
    nai_str_setn(&s->ciphers);
    nai_str_setn(&s->ocsp_uri);
    nai_array_init(&s->certs, sizeof(nai_str_t), c->pool);
    nai_array_init(&s->certs_expn, sizeof(nai_script_expn_t), c->pool);
    nai_array_init(&s->keys, sizeof(nai_str_t), c->pool);
    nai_array_init(&s->keys_expn, sizeof(nai_script_expn_t), c->pool);
    s->ctx = 0;
    s->passwords = 0;
    s->protocols = NAI_OPT_UNSET_VALUE;
    s->prefer_server_ciphers = NAI_OPT_UNSET_VALUE;
    s->early_data = NAI_OPT_UNSET_VALUE;
    s->reject_handshake = NAI_OPT_UNSET_VALUE;
    s->verify = NAI_OPT_UNSET_VALUE;
    s->verify_depth = NAI_OPT_UNSET_VALUE;
    s->ocsp = NAI_OPT_UNSET_VALUE;
    s->session_type = NAI_OPT_UNSET_VALUE;
    s->session_cache_size = NAI_OPT_UNSET_VALUE;
    s->session_timeout = NAI_OPT_UNSET_VALUE;

    pv[0] = s;
    rc = 0;

_end:
    return rc;
};


static nai_sult_t nai_https_ssl_server_merge(
    nai_https_local_ctx_t* c, void* s, void* up)
{
    nai_int_t r;
    nai_int_t inherit;
    nai_sult_t rc;
    nai_str_t* certs;
    nai_https_server_t* srv;
    nai_https_ssl_srv_t* conf = (nai_https_ssl_srv_t*)s;
    nai_https_ssl_srv_t* parent = (nai_https_ssl_srv_t*)up;


    inherit = 0;
    if (parent == 0) {
        if (conf->protocols == NAI_OPT_UNSET_VALUE) {
            conf->protocols = 
                NAI_SSL_TLSv1|NAI_SSL_TLSv1_1|NAI_SSL_TLSv1_2|NAI_SSL_TLSv1_3;
        };
        if (conf->prefer_server_ciphers == NAI_OPT_UNSET_VALUE) {
            conf->prefer_server_ciphers = 1;
        };
        if (conf->early_data == NAI_OPT_UNSET_VALUE) {
            conf->early_data = 0;
        };
        if (conf->reject_handshake == NAI_OPT_UNSET_VALUE) {
            conf->reject_handshake = 0;
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
        if (conf->session_type == NAI_OPT_UNSET_VALUE) {
            conf->session_type = NAI_SSL_SESSION_OFF;
            conf->session_cache_size = 0;
        };
        if (conf->session_timeout == NAI_OPT_UNSET_VALUE) {
            conf->session_timeout = 300;
        };
    } else {
        if (conf->certs.count == 0) {
            conf->certs = parent->certs;
            inherit ++;
        };
        if (conf->keys.count == 0) {
            conf->keys = parent->keys;
            inherit ++;
        };
        if (conf->passwords == 0) {
            conf->passwords = parent->passwords;
            inherit ++;
        };
        if (nai_str(&conf->client_cert) == 0) {
            conf->client_cert = parent->client_cert;
            inherit ++;
        };
        if (nai_str(&conf->trusted_cert) == 0) {
            conf->trusted_cert = parent->trusted_cert;
            inherit ++;
        };
        if (nai_str(&conf->dhparam) == 0) {
            conf->dhparam = parent->dhparam;
            inherit ++;
        };
        if (nai_str(&conf->ecdh_curve) == 0) {
            conf->ecdh_curve = parent->ecdh_curve;
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
        if (conf->protocols == NAI_OPT_UNSET_VALUE) {
            conf->protocols = parent->protocols;
            inherit ++;
        };
        if (conf->prefer_server_ciphers == NAI_OPT_UNSET_VALUE) {
            conf->prefer_server_ciphers = parent->prefer_server_ciphers;
            inherit ++;
        };
        if (conf->early_data == NAI_OPT_UNSET_VALUE) {
            conf->early_data = parent->early_data;
        };
        if (conf->reject_handshake == NAI_OPT_UNSET_VALUE) {
            conf->reject_handshake = parent->reject_handshake;
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
        if (conf->session_type == NAI_OPT_UNSET_VALUE) {
            conf->session_type = parent->session_type;
            conf->session_cache_size = parent->session_cache_size;
            inherit ++;
        };
        if (conf->session_timeout == NAI_OPT_UNSET_VALUE) {
            conf->session_timeout = parent->session_timeout;
            inherit ++;
        };
    };

    if (!c->done) {
        rc = 0;
        goto _end;
    };

    if (inherit >= 17) {
        conf->ctx = parent->ctx;
        rc = 0;
        goto _end;
    };

    if (conf->keys.count < conf->certs.count) {
        rc = nai_errno_to_sult(EINVAL);
        nai_log_error(NAI_LOG_HTTPS, 
            nai_sult_to_errno(rc), "ssl_keys is less than ssl_certs");
        goto _end;
    };

    conf->ctx = (nai_ssl_ctx_t*)nai_palloc(c->pool, sizeof(*conf->ctx));
    if (conf->ctx == 0) {
        rc = nai_sult_from_errno();
        nai_log_alert(NAI_LOG_HTTPS, 
            nai_sult_to_errno(rc), "allocate ssl context failed");
        goto _end;
    };

    nai_ssl_ctx_init(conf->ctx);
    nai_ssl_ctx_set_ops(conf->ctx, 
        &nai_https_ssl_ctx_ops, c->service->main);

    r = nai_pool_add_cleanup(c->pool, nai_https_ssl_ctx_cleanup, conf);
    if (r < 0) {
        rc = nai_sult_from_errno();
        nai_log_error(NAI_LOG_HTTPS, 
            nai_sult_to_errno(rc), "ssl context add cleanup failed");
        goto _end;
    };

    r = nai_ssl_ctx_open(conf->ctx, conf->protocols);
    if (r < 0) {
        rc = nai_sult_from_errno();
        nai_log_error(NAI_LOG_HTTPS, 
            nai_sult_to_errno(rc), "nai_ssl_ctx_open() failed");
        goto _end;
    };

    /* add certs */
    rc = nai_https_ssl_load_certs(c, conf);
    if (rc < 0) {
        goto _end;
    };

    if (nai_str_len(&conf->trusted_cert) > 0) {
        r = nai_ssl_ctx_trusted_certificate(conf->ctx, &conf->trusted_cert);
        if (r < 0) {
            rc = nai_sult_from_errno();
            nai_log_error(NAI_LOG_HTTPS, 
                nai_sult_to_errno(rc), 
                "nai_ssl_ctx_trusted_certificate('%s') failed", 
                nai_str(&conf->trusted_cert));
            goto _end;
        };
    };

    if (nai_str_len(&conf->crl) > 0) {
        r = nai_ssl_ctx_crl(conf->ctx, &conf->crl);
        if (r < 0) {
            rc = nai_sult_from_errno();
            nai_log_error(NAI_LOG_HTTPS, 
                nai_sult_to_errno(rc), "nai_ssl_ctx_crl('%s') failed", 
                nai_str(&conf->crl));
            goto _end;
        };
    };

    if (nai_str_len(&conf->dhparam) > 0) {
        r = nai_ssl_ctx_dhparam(conf->ctx, &conf->dhparam);
        if (r < 0) {
            rc = nai_sult_from_errno();
            nai_log_error(NAI_LOG_HTTPS, 
                nai_sult_to_errno(rc), "nai_ssl_ctx_dhparam('%s') failed", 
                nai_str(&conf->dhparam));
            goto _end;
        };
    };

    if (nai_str_len(&conf->ecdh_curve) > 0) {
        r = nai_ssl_ctx_ecdh_curve(
            conf->ctx, nai_str(&conf->ecdh_curve));
        if (r < 0) {
            rc = nai_sult_from_errno();
            nai_log_error(NAI_LOG_HTTPS, 
                nai_sult_to_errno(rc), "nai_ssl_ctx_ecdh_curve() failed");
            goto _end;
        };
    };

    if (nai_str_len(&conf->ciphers) > 0) {
        r = nai_ssl_ctx_ciphers(conf->ctx, nai_str(&conf->ciphers));
        if (r < 0) {
            rc = nai_sult_from_errno();
            nai_log_error(NAI_LOG_HTTPS, 
                nai_sult_to_errno(rc), "nai_ssl_ctx_ciphers() failed");
            goto _end;
        };
    };

    if (conf->prefer_server_ciphers) {
        r = nai_ssl_ctx_prefer_server_ciphers(conf->ctx);
        if (r < 0) {
            rc = nai_sult_from_errno();
            nai_log_error(NAI_LOG_HTTPS, 
                nai_sult_to_errno(rc), 
                "nai_ssl_ctx_prefer_server_ciphers() failed");
            goto _end;
        };
    };

    if (conf->early_data) {
        /* nothing */
        ;
    };


    r = nai_ssl_ctx_verify_depth(conf->ctx, conf->verify_depth);
    if (r < 0) {
        rc = nai_sult_from_errno();
        nai_log_error(NAI_LOG_HTTPS, 
            nai_sult_to_errno(rc), "nai_ssl_ctx_verify_depth() failed");
        goto _end;
    };

    if (conf->verify) {
        if (conf->verify < NAI_SSL_VERIFY_NO_CA && 
            nai_str_len(&conf->client_cert) <= 0) {
            rc = nai_errno_to_sult(EINVAL);
            nai_log_error(NAI_LOG_HTTPS, 
                nai_sult_to_errno(rc), 
                "require a valid client certificate to enable ssl_verify");
            goto _end;
        };

        if (nai_str_len(&conf->client_cert) > 0) {
            r = nai_ssl_ctx_client_certificate(conf->ctx, &conf->client_cert);
            if (r < 0) {
                rc = nai_sult_from_errno();
                nai_log_error(NAI_LOG_HTTPS, 
                    nai_sult_to_errno(rc), 
                    "nai_ssl_ctx_client_certificate('%s') failed", 
                    nai_str(&conf->client_cert));
                goto _end;
            };
        };
    };

    if (conf->ocsp) {
        if (conf->verify == NAI_SSL_VERIFY_NO_CA) {
            rc = nai_errno_to_sult(EINVAL);
            nai_log_error(NAI_LOG_HTTPS, 
                nai_sult_to_errno(rc), 
                "'ssl_ocsp' is incompatible with "
                "'ssl_verify(optional_no_ca)'");
            goto _end;
        };

        srv = nai_https_get_server(c);
        r = nai_ssl_ctx_conn_attr(conf->ctx, &srv->conn);
        if (r < 0) {
            rc = nai_sult_from_errno();
            nai_log_error(NAI_LOG_HTTPS, 
                nai_sult_to_errno(rc), 
                "nai_ssl_ctx_conn_attr() failed");
            goto _end;
        };

        r = nai_ssl_ctx_ocsp(conf->ctx, conf->ocsp, &conf->ocsp_uri);
        if (r < 0) {
            rc = nai_sult_from_errno();
            nai_log_error(NAI_LOG_HTTPS, 
                nai_sult_to_errno(rc), 
                "nai_ssl_ctx_ocsp('%d', '%s') failed", 
                conf->ocsp, nai_str(&conf->ocsp_uri));
            goto _end;
        };

        r = nai_ssl_store_open(&conf->ocsp_store, 16*1024);
        if (r < 0) {
            rc = nai_sult_from_errno();
            nai_log_error(NAI_LOG_HTTPS, 
                nai_sult_to_errno(rc), 
                "nai_ssl_store_open() failed");
            goto _end;
        };

        r = nai_ssl_ctx_ocsp_store(conf->ctx, &conf->ocsp_store);
        if (r < 0) {
            rc = nai_sult_from_errno();
            nai_log_error(NAI_LOG_HTTPS, 
                nai_sult_to_errno(rc), 
                "nai_ssl_ctx_ocsp_store() failed");
            goto _end;
        };
    };


    r = nai_ssl_ctx_session_cache(conf->ctx, 
        conf->session_type, conf->session_cache_size);
    if (r < 0) {
        rc = nai_sult_from_errno();
        nai_log_error(NAI_LOG_HTTPS, 
            nai_sult_to_errno(rc), "nai_ssl_ctx_session() failed");
        goto _end;
    };

    if (conf->session_type != NAI_SSL_SESSION_OFF) {
        r = nai_ssl_ctx_session_timeout(conf->ctx, conf->session_timeout);
        if (r < 0) {
            rc = nai_sult_from_errno();
            nai_log_error(NAI_LOG_HTTPS, 
                nai_sult_to_errno(rc), "nai_ssl_ctx_session_timeout() failed");
            goto _end;
        };

        certs = (nai_str_t*)conf->certs.elts;
        r = nai_ssl_ctx_session_unique_id(
            conf->ctx, "https", certs, conf->certs.count);
        if (r < 0) {
            rc = nai_sult_from_errno();
            nai_log_error(NAI_LOG_HTTPS, 
                nai_sult_to_errno(rc), 
                "nai_ssl_ctx_session_unique_id() failed");
            goto _end;
        };
    };

    rc = 0;

_end:
    return rc;
};


static nai_sult_t nai_https_ssl_load_certs(
    nai_https_local_ctx_t* c, nai_https_ssl_srv_t* srv)
{
    nai_int_t r;
    nai_int_t n;
    nai_int_t count;
    nai_int_t passcount;
    nai_int_t is_expn;
    nai_str_t* certs;
    nai_str_t* keys;
    nai_str_t* passwords;
    nai_script_expn_t* certs_expn;
    nai_script_expn_t* keys_expn;
    nai_https_t* h;
    nai_array_t* passdup;
    nai_sult_t rc;


    count = srv->certs.count;
    if (count <= 0) {
        rc = 0;
        goto _end;
    };

    is_expn = 0;
    certs = (nai_str_t*)srv->certs.elts;
    keys = (nai_str_t*)srv->keys.elts;
    for (n = 0; n < count; n ++) {
        if (nai_script_is_expn(&certs[0])) {
            is_expn = 1;
            break;
        };
        if (nai_script_is_expn(&keys[0])) {
            is_expn = 1;
            break;
        };
    };

    if (srv->passwords) {
        passcount = (nai_int_t)srv->passwords->count;
        passwords = (nai_str_t*)srv->passwords->elts;
    } else {
        passcount = 0;
        passwords = 0;
    };
    if (!is_expn) {
        for (n = 0; n < count; n ++) {
            r = nai_ssl_ctx_certificate(
                srv->ctx, &certs[n], &keys[n], passwords, passcount);
            if (r < 0) {
                rc = nai_sult_from_errno();
                nai_log_error(NAI_LOG_HTTPS, 
                    nai_sult_to_errno(rc), 
                    "nai_ssl_ctx_certificate('%s', '%s') failed", 
                    nai_str(&certs[n]) , nai_str(&keys[n]));
                goto _end;
            };
        };
    } else {
        certs_expn = (nai_script_expn_t*)
            nai_array_push_n(&srv->certs_expn, count);
        if (certs_expn == 0) {
            rc = nai_sult_from_errno();
            nai_log_alert(NAI_LOG_HTTPS, 
                nai_sult_to_errno(rc), 
                "allocate expression of certs failed");
            goto _end;
        };

        keys_expn = (nai_script_expn_t*)
            nai_array_push_n(&srv->keys_expn, count);
        if (keys_expn == 0) {
            rc = nai_sult_from_errno();
            nai_log_alert(NAI_LOG_HTTPS, 
                nai_sult_to_errno(rc), 
                "allocate expression of keys failed");
            goto _end;
        };


        h = c->service;
        for (n = 0; n < count; n ++) {
            rc = nai_https_expn_compile(h, &certs_expn[n], &certs[n]);
            if (rc < 0) {
                nai_log_error(NAI_LOG_HTTPS, 
                    nai_sult_to_errno(rc), 
                    "compile certs expression '%s' failed", 
                    nai_str(&certs[n]));
                goto _end;
            };

            rc = nai_https_expn_compile(h, &keys_expn[n], &certs[n]);
            if (rc < 0) {
                nai_log_error(NAI_LOG_HTTPS, 
                    nai_sult_to_errno(rc), 
                    "compile keys expression '%s' failed", 
                    nai_str(&certs[n]));
                goto _end;
            };

            if (nai_script_expn_is_variable(&certs_expn[n]) || 
                nai_script_expn_is_variable(&keys_expn[n])) {
                continue;
            };

            r = nai_ssl_ctx_certificate(
                srv->ctx, &certs[n], &keys[n], passwords, passcount);
            if (r < 0) {
                rc = nai_sult_from_errno();
                nai_log_error(NAI_LOG_HTTPS, 
                    nai_sult_to_errno(rc), 
                    "nai_ssl_ctx_certificate('%s', '%s') failed", 
                    nai_str(&certs[n]) , nai_str(&keys[n]));
                goto _end;
            };
        };

        if (srv->passwords && 
            srv->passwords->pool != c->pool) {
            passdup = nai_ssl_dup_password(c->pool, srv->passwords);
            if (passdup == 0) {
                rc = nai_sult_from_errno();
                nai_log_error(NAI_LOG_HTTPS, 
                    nai_sult_to_errno(rc), "nai_ssl_dup_password() failed");
                goto _end;
            };

            srv->passwords = passdup;
        };
    };

    rc = 0;

_end:
    return rc;
};



typedef struct nai_https_ssl_s {
    nai_ssl_t ssl;
    nai_buf_t* buf;
    nai_mem_t server_name;
} nai_https_ssl_t;


static nai_ssl_ops_t nai_https_ssl_head_ops;
static nai_ssl_ops_t nai_https_ssl_ops;


static void nai_https_ssl_cleanup(void* up)
{
    nai_ssl_t* ssl = (nai_ssl_t*)up;
    nai_ssl_close(ssl);
};


static nai_int_t nai_https_ssl_lookup_certs(void* ud)
{
    nai_int_t r;
    nai_int_t n;
    nai_int_t count;
    nai_int_t passcount;
    nai_sult_t rc;
    nai_str_t cert;
    nai_str_t key;
    nai_str_t* passwords;
    nai_script_expn_t* certs;
    nai_script_expn_t* keys;
    nai_https_ssl_t* s;
    nai_https_request_t* q;
    nai_https_protocol_t* p;
    nai_https_connection_t* c;
    nai_https_ssl_srv_t* srv;


    c = (nai_https_connection_t*)ud;
    s = (nai_https_ssl_t*)c->ssl;
    if (s->ssl.handshaked) {
        r = 0;
        goto _end;
    };

    srv = nai_https_get_server_conf(c, nai_https_ssl_module);
    count = (nai_int_t)srv->certs_expn.count;
    if (count <= 0) {
        r = 0;
        goto _end;
    };

    q = 0;
    p = c->proto;
    if (p->main == 0) {
        rc = nai_https_request_create(c, p);
        if (rc < 0) {
            goto _fail;
        };

        q = p->main;
        q->logged = 1;
        c->requests --;
    };


    if (srv->passwords) {
        passcount = (nai_int_t)srv->passwords->count;
        passwords = (nai_str_t*)srv->passwords->elts;
    } else {
        passcount = 0;
        passwords = 0;
    };

    keys = srv->keys_expn.elts;
    certs = srv->certs_expn.elts;
    for (n = 0; n < count; n ++) {
        if (!nai_script_expn_is_variable(&certs[n]) && 
            !nai_script_expn_is_variable(&keys[n])) {
            continue;
        };

        rc = nai_https_expn_value(p->main, &certs[n], &cert, 0);
        if (rc < 0) {
            nai_log_error(NAI_LOG_HTTPS, 
                nai_sult_to_errno(rc), 
                "get value of ssl cert '%s' failed", 
                nai_str(&certs[n].value));
            goto _fail;
        };

        rc = nai_https_expn_value(p->main, &keys[n], &key, 0);
        if (rc < 0) {
            nai_log_error(NAI_LOG_HTTPS, 
                nai_sult_to_errno(rc), 
                "get value of ssl key '%s' failed", 
                nai_str(&certs[n].value));
            goto _fail;
        };

        r = nai_ssl_certificate(
            &s->ssl, &cert, &key, passwords, passcount);
        if (r < 0) {
            rc = nai_sult_from_errno();
            nai_log_error(NAI_LOG_HTTPS, 
                nai_sult_to_errno(rc), 
                "dynamic load cert '%s' and key '%s' failed", 
                nai_str(&cert), nai_str(&key));
            goto _fail;
        };
    };

    if (q != 0) {
        nai_https_request_close(q);
    };

    r = 0;

_end:
    return r;

_fail:
    if (q != 0) {
        nai_https_request_close(q);
    };
    r = -1;
    goto _end;
};


static nai_int_t nai_https_ssl_lookup_server(void* ud, const char* name)
{
    nai_int_t r;
    nai_sult_t rc;
    nai_mem_t hostname;
    nai_https_ssl_t* s;
    nai_https_ssl_srv_t* srv;
    nai_https_server_t* server;
    nai_https_connection_t* c;


    c = (nai_https_connection_t*)ud;
    s = (nai_https_ssl_t*)c->ssl;
    if (s->ssl.handshaked) {
        r = -1;
        goto _end;
    };

    if (name == 0) {
        goto _done;
    };

    nai_str_sets(&hostname, name);
    if (nai_str_len(&hostname) <= 0) {
        goto _done;
    };


    rc = nai_https_server_find(c->lc, &hostname, &server);
    if (rc < 0) {
        if (rc != nai_errno_to_sult(ENOENT)) {
            nai_log_alert(NAI_LOG_HTTPS, 
                nai_sult_to_errno(rc), "nai_https_server_find() failed");
            r = -1;
            goto _end;
        };

        goto _done;
    };

    s->server_name = hostname;
    c->ssl_servername = &s->server_name;


    /* update new conf */
    c->conf = server->conf;


    /* change new ssl ctx */
    srv = nai_https_get_server_conf(c, nai_https_ssl_module);
    if (srv->ctx->ctx) {
        r = nai_ssl_set_ctx(&s->ssl, srv->ctx);
        if (r < 0) {
            nai_log_alert(NAI_LOG_HTTPS, 
                nai_errno, "nai_ssl_set_ctx() failed");
            goto _end;
        };
    };


_done:
    srv = nai_https_get_server_conf(c, nai_https_ssl_module);
    if (srv->reject_handshake) {
        s->ssl.rejected = 1;
        r = -1;
        goto _end;
    };

    r = 0;

_end:
    return r;
};


static void* nai_https_ssl_realloc(void* ud, void* ptr, size_t size)
{
    void* r;
    nai_https_connection_t* c;


    c = (nai_https_connection_t*)ud;
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


static intptr_t nai_https_ssl_read(void* ud, void* buf, size_t size)
{
    intptr_t r;
    nai_https_ssl_t* s;
    nai_https_connection_t* c;


    c = (nai_https_connection_t*)ud;
    s = (nai_https_ssl_t*)c->ssl;
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
            c->ssl->ops = &nai_https_ssl_ops;
        };
    };

    return r;
};


static nai_ssl_ops_t nai_https_ssl_ops = {
    nai_https_ssl_lookup_certs, 
    nai_https_ssl_lookup_server, 
    0, 0, /* cookies */
    nai_ssl_iobase_poll, 
    nai_ssl_iobase_want, 
    nai_ssl_iobase_emit, 
    nai_ssl_iobase_get_loop, 
    nai_ssl_iobase_get_opt, 
    nai_ssl_iobase_get_peer, 
    nai_https_ssl_realloc, 
    (nai_ssl_read_f)nai_stream_read, 
    (nai_ssl_write_f)nai_stream_write, 
    (nai_ssl_recvm_f)nai_stream_recvm, 
    (nai_ssl_sendm_f)nai_stream_sendm, 
    (nai_ssl_sendfile_f)nai_stream_sendfile, 
    0, 
};

static nai_ssl_ops_t nai_https_ssl_head_ops = {
    nai_https_ssl_lookup_certs, 
    nai_https_ssl_lookup_server, 
    0, 0, /* cookies */
    nai_ssl_iobase_poll, 
    nai_ssl_iobase_want, 
    nai_ssl_iobase_emit, 
    nai_ssl_iobase_get_loop, 
    nai_ssl_iobase_get_opt, 
    nai_ssl_iobase_get_peer, 
    nai_https_ssl_realloc, 
    nai_https_ssl_read, 
    (nai_ssl_write_f)nai_stream_write, 
    (nai_ssl_recvm_f)nai_stream_recvm, 
    (nai_ssl_sendm_f)nai_stream_sendm, 
    (nai_ssl_sendfile_f)nai_stream_sendfile, 
    0, 
};


nai_sult_t nai_https_ssl_create(
    nai_https_connection_t* c, nai_buf_t* head)
{
    nai_int_t r;
    nai_sult_t rc;
    nai_ssl_ops_t* ops;
    nai_https_ssl_t* s;
    nai_https_ssl_srv_t* srv;
    nai_https_location_t* l;


    /* set nodelay for ssl */
    l = nai_https_get_location(c);
    if (c->c.tcp_nodelay == 0 && l->tcp_nodelay) {
        rc = nai_https_connection_tcp_nodelay(c, 1);
        if (rc < 0) {
            nai_log_error(NAI_LOG_HTTPS, 
                nai_sult_to_errno(rc), "set nodelay option failed");
            goto _end;
        };
    };


    /* create a ssl */
    s = (nai_https_ssl_t*)nai_palloc(c->c.pool, sizeof(*s));
    if (s == 0) {
        rc = nai_sult_from_errno();
        nai_log_alert(NAI_LOG_HTTPS, 
            nai_sult_to_errno(rc), "alloc nai_ssl_t for https failed");
        goto _end;
    };

    if (head) {
        ops = &nai_https_ssl_head_ops;
    } else {
        ops = &nai_https_ssl_ops;
    };

    s->buf = head;
    nai_ssl_init(&s->ssl);
    nai_ssl_set_ops(&s->ssl, ops, c);

    /* add cleanup */
    r = nai_pool_add_cleanup(c->c.pool, nai_https_ssl_cleanup, s);
    if (r < 0) {
        rc = nai_sult_from_errno();
        nai_log_alert(NAI_LOG_HTTPS, 
            nai_sult_to_errno(rc), "nai_pool_add_cleanup() failed");
        goto _end;
    };

    /* open ssl */
    srv = nai_https_get_server_conf(c, nai_https_ssl_module);
    r = nai_ssl_open(&s->ssl, srv->ctx, 0);
    if (r < 0) {
        rc = nai_sult_from_errno();
        nai_log_alert(NAI_LOG_HTTPS, 
            nai_sult_to_errno(rc), "nai_ssl_open(accept) failed");
        goto _end;
    };

    c->ssl = &s->ssl;
    rc = 0;

_end:
    return rc;
};


nai_sult_t nai_https_ssl_verify_cert(
    nai_https_connection_t* c)
{
    nai_int_t r;
    nai_int_t ec;
    nai_sult_t rc;
    nai_https_ssl_t* s;
    nai_https_ssl_srv_t* srv;


    s = (nai_https_ssl_t*)c->ssl;
    srv = nai_https_get_server_conf(c, nai_https_ssl_module);

    if (srv->verify != NAI_SSL_VERIFY_OFF) {
        r = nai_ssl_verify(&s->ssl, srv->verify);
        if (r < 0) {
            ec = nai_errno;
            if (ec == ENOENT) {
                rc = NAI_HTTP_NO_CERT;
            } else {
                rc = NAI_HTTP_CERT_ERROR;
            };
            goto _end;
        };
    };

    rc = 0;

_end:
    return rc;
};


nai_sult_t nai_https_ssl_verify_server(
    nai_https_connection_t* c, 
    nai_https_server_t* s, const nai_mem_t* hostname)
{
    nai_sult_t rc;
    nai_https_ssl_srv_t* srv;


    if (c->ssl_servername) {
        srv = nai_https_get_server_conf(s, nai_https_ssl_module);
        if (srv->verify != NAI_SSL_VERIFY_OFF && 
            nai_str_caseeq(c->ssl_servername, hostname) != 0) {
            rc = NAI_HTTP_MISDIRECTED_REQUEST;
            goto _end;
        };
    };

    rc = 0;

_end:
    return rc;
};


#endif

