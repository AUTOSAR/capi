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
/// @file       nai_https_user_module.c
/// @brief      
/// @details
/// @date       2021-08-10
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_log.h"
#include "nai/service/nai_main.h"
#include "nai/service/nai_module.h"
#include "nhttp/server/nai_https_user.h"
#include "nai_https_core.h"



typedef struct nai_https_user_handle_s {
    nai_hash_str_t ent;
    nai_https_handle_f handle;
    nai_int_t stage;
} nai_https_user_handle_t;


typedef struct nai_https_user_s {
    nai_int_t slots;
    nai_int_t masks;
    nai_hashmap_t handles;
} nai_https_user_t;


typedef struct nai_https_user_ctx_s {
    void* vec[1];
} nai_https_user_ctx_t;


typedef struct nai_https_user_loc_s {
    nai_https_handle_f handles[NAI_HTTPS_STAGE_MAX];
} nai_https_user_loc_t;


static nai_sult_t nai_https_user_init(nai_main_t* m);
static nai_sult_t nai_https_user_preproc(nai_https_t* h);
static nai_sult_t nai_https_user_postproc(nai_https_t* h);
static nai_sult_t nai_https_user_location_alloc(
    nai_https_local_ctx_t* c, void** pv);
static nai_sult_t nai_https_user_location_merge(
    nai_https_local_ctx_t* c, void* s, void* up);


static nai_sult_t nai_https_use_handle(
    nai_command_t* c, nai_command_args_t* args);


static nai_sult_t nai_https_user_prepare(nai_https_request_t* r);
static nai_sult_t nai_https_user_server(nai_https_request_t* r);
static nai_sult_t nai_https_user_find_config(nai_https_request_t* r);
static nai_sult_t nai_https_user_rewrite(nai_https_request_t* r);
static nai_sult_t nai_https_user_access(nai_https_request_t* r);
static nai_sult_t nai_https_user_content(nai_https_request_t* r);
static nai_sult_t nai_https_user_response(nai_https_request_t* r);
static nai_sult_t nai_https_user_log(nai_https_request_t* r);


static struct {
    const char* name;
    nai_https_handle_f handle;
} nai_https_user_stages[] = {
    { "prepare", nai_https_user_prepare }, 
    { "server", nai_https_user_server }, 
    { "find_config", nai_https_user_find_config }, 
    { "rewrite", nai_https_user_rewrite }, 
    { "access", nai_https_user_access }, 
    { "content", nai_https_user_content }, 
    { "response", nai_https_user_response }, 
    { "log", nai_https_user_log }, 
};


static nai_command_t nai_https_user_commands[] = {
    { "use", nai_https_use_handle, 
        NAI_HTTPS_CLOC_MAIN|NAI_HTTPS_CLOC_SRV|NAI_HTTPS_CLOC_LOC|
        NAI_HTTPS_COPT_LOC|NAI_COPT_ARG2, 0, 0 },
    { 0 }
};


static nai_https_module_t nai_https_user_module_ext = {
    nai_https_user_preproc,             /* pre-processing */
    nai_https_user_postproc,            /* post-processing */
    0,                                  /* main alloc */
    0, 0,                               /* server alloc */
    nai_https_user_location_alloc,      /* location alloc */
    nai_https_user_location_merge,      /* location merge */
};


nai_module_t nai_https_user_module = {
    NAI_MODULE_HEADER,
    NAI_MODULE_HTTPS, 
    nai_https_user_commands, 
    "https_user", &nai_https_user_module_ext, {
        nai_https_user_init, 
        0, 
        0, 
        0, 
        0, 
    },
};


static nai_sult_t nai_https_user_init(nai_main_t* m)
{
    nai_int_t r;
    nai_sult_t rc;
    nai_https_user_t* u;


    u = (nai_https_user_t*)nai_main_local_at(m, nai_https_user_module);
    if (u != 0) {
        rc = 0;
        goto _end;
    };

    u = (nai_https_user_t*)nai_palloc(&m->pool, sizeof(*u));
    if (u == 0) {
        rc = nai_sult_from_errno();
        goto _end;
    };

    u->slots = 0;
    u->masks = 0;
    nai_hashmap_init(&u->handles, &m->pool);

    r = nai_hashmap_open(&u->handles, 17, NAI_HASH_STRING);
    if (r < 0) {
        rc = nai_sult_from_errno();
        goto _end;
    };

    nai_main_local_at(m, nai_https_user_module) = u;

    rc = 0;

_end:
    return rc;
};


static nai_sult_t nai_https_user_preproc(nai_https_t* h)
{
    (void)h;
    return 0;
};


static nai_sult_t nai_https_user_postproc(nai_https_t* h)
{
    nai_int_t n;
    nai_sult_t rc;
    nai_main_t* m;
    nai_https_user_t* u;


    m = h->main;
    u = nai_main_local_at(m, nai_https_user_module);

    for (n = 0; n < (nai_int_t)nai_countof(nai_https_user_stages); n ++) {
        if ((u->masks & (1 << n)) == 0) {
            continue;
        };
        if (nai_https_user_stages[n].handle == 0) {
            continue;
        };

        rc = nai_https_add_stage(m, n, 
            NAI_HTTPS_HPRIO_NORMAL, nai_https_user_stages[n].handle);
        if (rc < 0) {
            nai_log_alert(NAI_LOG_HTTPS, 
                nai_sult_to_errno(rc), "add user '%s' stage failed", 
                nai_https_user_stages[n].name);
            goto _end;
        };
    };

    rc = 0;

_end:
    return rc;
};


static nai_sult_t nai_https_user_location_alloc(
    nai_https_local_ctx_t* c, void** pv)
{
    nai_int_t n;
    nai_sult_t rc;
    nai_https_user_loc_t* l;


    l = (nai_https_user_loc_t*)nai_palloc(c->pool, sizeof(*l));
    if (l == 0) {
        rc = nai_sult_from_errno();
        nai_log_alert(NAI_LOG_HTTPS, 
            nai_sult_to_errno(rc), "alloc location conf of user failed");
        goto _end;
    };

    for (n = 0; n < NAI_HTTPS_STAGE_MAX; n ++) {
        l->handles[n] = (nai_https_handle_f)-1;
    };

    pv[0] = l;
    rc = 0;

_end:
    return rc;
};


static nai_sult_t nai_https_user_location_merge(
    nai_https_local_ctx_t* c, void* s, void* up)
{
    nai_int_t n;
    nai_sult_t rc;
    nai_https_user_loc_t* conf = (nai_https_user_loc_t*)s;
    nai_https_user_loc_t* parent = (nai_https_user_loc_t*)up;


    (void)c;

    if (parent == 0) {
        for (n = 0; n < NAI_HTTPS_STAGE_MAX; n ++) {
            if (conf->handles[n] == (nai_https_handle_f)-1) {
                conf->handles[n] = 0;
            };
        };
    } else {
        for (n = 0; n < NAI_HTTPS_STAGE_MAX; n ++) {
            if (conf->handles[n] == (nai_https_handle_f)-1) {
                conf->handles[n] = parent->handles[n];
            };
        };
    };

    rc = 0;

    return rc;
};


static nai_sult_t nai_https_use_handle(
    nai_command_t* c, nai_command_args_t* args)
{
    nai_int_t n;
    nai_int_t stage;
    nai_sult_t rc;
    nai_mem_t cmp;
    nai_mem_t sname;
    nai_mem_t hname;
    nai_hash_elt_t* ent;
    nai_https_user_t* u;
    nai_https_user_loc_t* ul;
    nai_https_user_handle_t* h;


    (void)c;

    rc = nai_value_getm(&args->argv[0], &sname);
    if (rc < 0) {
        nai_log_error(NAI_LOG_HTTPS, 
            nai_sult_to_errno(rc), "get stage name failed");
        goto _end;
    };

    rc = nai_value_getm(&args->argv[1], &hname);
    if (rc < 0) {
        nai_log_error(NAI_LOG_HTTPS, 
            nai_sult_to_errno(rc), "get handle name failed");
        goto _end;
    };

    stage = -1;
    for (n = 0; n < (nai_int_t)nai_countof(nai_https_user_stages); n ++) {
        nai_str_sets(&cmp, nai_https_user_stages[n].name);
        if (nai_str_caseeq(&sname, &cmp)) {
            stage = n;
            break;
        };
    };
    if (stage == -1) {
        rc = nai_errno_to_sult(EINVAL);
        nai_log_error(NAI_LOG_HTTPS, 
            nai_sult_to_errno(rc), "unknown stage '%.*s'", 
            nai_str_len(&sname), nai_str(&sname));
        goto _end;
    };

    nai_str_hash(&hname, 0);
    u = nai_main_local_at(args->main, nai_https_user_module);
    ent = nai_hashmap_str_find(&u->handles, (nai_str_t*)&hname, 0);
    if (ent == 0) {
        rc = nai_errno_to_sult(EINVAL);
        nai_log_error(NAI_LOG_HTTPS, 
            nai_sult_to_errno(rc), "unknown handle '%.*s'", 
            nai_str_len(&hname), nai_str(&hname));
        goto _end;
    };

    h = (nai_https_user_handle_t*)ent;
    if (h->stage != stage) {
        rc = nai_errno_to_sult(EINVAL);
        nai_log_error(NAI_LOG_HTTPS, 
            nai_sult_to_errno(rc), 
            "'%.*s' is not a handle of stage '%.*s'", 
            nai_str_len(&sname), nai_str(&sname), 
            nai_str_len(&hname), nai_str(&hname));
        goto _end;
    };


    ul = (nai_https_user_loc_t*)args->data;
    ul->handles[stage] = h->handle;
    u->masks |= (1 << stage);
    rc = 0;

_end:
    return rc;
};


static nai_sult_t nai_https_user_handle(
    nai_https_request_t* r, nai_int_t stage)
{
    nai_int_t n = stage;
    nai_sult_t rc;
    nai_https_user_loc_t* l;


    l = nai_https_get_location_conf(r, nai_https_user_module);
    if (l->handles[n] == 0) {
        rc = NAI_DECLINED;
    } else {
        rc = l->handles[n](r);
    };

    return rc;
};


static nai_sult_t nai_https_user_prepare(nai_https_request_t* r)
{
    nai_int_t n = NAI_HTTPS_STAGE_PREPARE;
    return nai_https_user_handle(r, n);
};


static nai_sult_t nai_https_user_server(nai_https_request_t* r)
{
    nai_int_t n = NAI_HTTPS_STAGE_SERVER;
    return nai_https_user_handle(r, n);
};


static nai_sult_t nai_https_user_find_config(nai_https_request_t* r)
{
    nai_int_t n = NAI_HTTPS_STAGE_FIND_CONFIG;
    return nai_https_user_handle(r, n);
};


static nai_sult_t nai_https_user_rewrite(nai_https_request_t* r)
{
    nai_int_t n = NAI_HTTPS_STAGE_REWRITE;
    return nai_https_user_handle(r, n);
};


static nai_sult_t nai_https_user_access(nai_https_request_t* r)
{
    nai_int_t n = NAI_HTTPS_STAGE_ACCESS;
    return nai_https_user_handle(r, n);
};


static nai_sult_t nai_https_user_content(nai_https_request_t* r)
{
    nai_int_t n = NAI_HTTPS_STAGE_CONTENT;
    return nai_https_user_handle(r, n);
};


static nai_sult_t nai_https_user_response(nai_https_request_t* r)
{
    nai_int_t n = NAI_HTTPS_STAGE_RESPONSE;
    return nai_https_user_handle(r, n);
};


static nai_sult_t nai_https_user_log(nai_https_request_t* r)
{
    nai_int_t n = NAI_HTTPS_STAGE_LOG;
    return nai_https_user_handle(r, n);
};


static nai_int_t nai_https_user_start(nai_main_t* m)
{
    nai_int_t r;


    if (nai_module_map_find(&m->modules, 
        nai_https_user_module.name) == 0) {
        r = nai_module_map_push(&m->modules, &nai_https_user_module);
        if (r < 0) {
            goto _end;
        };

        r = nai_https_user_module.init(m);
        if (r < 0) {
            nai_errno = nai_sult_to_errno(r);
            r = -1;
            goto _end;
        };
    };

    r = 0;

_end:
    return r;
};


void** nai_https_get_slot(nai_https_request_t* r, nai_int_t slot)
{
    void** p;
    nai_main_t* m;
    nai_https_t* h;
    nai_https_user_t* u;
    nai_https_user_ctx_t* c;


    h = nai_https_get_service(r);
    m = h->main;
    u = nai_main_local_at(m, nai_https_user_module);

    if (slot >= u->slots) {
        nai_errno = EINVAL;
        p = 0;
        goto _end;
    };

    c = (nai_https_user_ctx_t*)nai_local_at(r->local, nai_https_user_module);
    if (c == 0) {
        c = (nai_https_user_ctx_t*)
            nai_palloc(r->pool, sizeof(void*) * u->slots);
        if (c == 0) {
            nai_log_error(NAI_LOG_HTTPS, 
                nai_errno, "alloc user local store failed");
            p = 0;
            goto _end;
        };

        nai_pzero(c->vec, u->slots);
        nai_local_at(r->local, nai_https_user_module) = c;
    };

    p = &c->vec[slot];

_end:
    return p;
};


nai_int_t nai_https_add_slot(nai_main_t* m)
{
    nai_int_t r;
    nai_https_user_t* u;


    r = nai_https_user_start(m);
    if (r < 0) {
        goto _end;
    };

    u = nai_main_local_at(m, nai_https_user_module);
    r = u->slots ++;

_end:
    return r;
};


nai_int_t nai_https_add_handle(nai_main_t* m, 
    const char* name, nai_int_t stage, nai_https_handle_f handle)
{
    nai_int_t r;
    nai_str_t key;
    nai_hash_elt_t* ent;
    nai_https_user_t* u;
    nai_https_user_handle_t* h;


    r = nai_https_user_start(m);
    if (r < 0) {
        goto _end;
    };

    u = nai_main_local_at(m, nai_https_user_module);

    nai_str_sets(&key, name);
    nai_str_hash(&key, 0);
    ent = nai_hashmap_str_find(&u->handles, &key, 0);
    if (ent != 0) {
        nai_errno = EEXIST;
        r = -1;
        goto _end;
    };

    h = (nai_https_user_handle_t*)nai_palloc(&m->pool, sizeof(*h));
    if (h == 0) {
        r = -1;
        goto _end;
    };

    if (nai_str_dup(&h->ent.key, 
        nai_str(&key), nai_str_len(&key), &m->pool) < 0) {
        r = -1;
        goto _end;
    };

    nai_str_hash(&h->ent.key, 0);
    h->stage = stage;
    h->handle = handle;

    nai_hashmap_insert(&u->handles, (nai_hash_elt_t*)h);
    r = 0;

_end:
    return r;
};


nai_int_t nai_https_find_handle(nai_main_t* m, 
    const char* name, nai_int_t stage, nai_https_handle_f* pv)
{
    nai_int_t r;
    nai_str_t key;
    nai_hash_elt_t* ent;
    nai_https_user_t* u;
    nai_https_user_handle_t* h;


    (void)stage;

    if (nai_module_map_find(&m->modules, 
        nai_https_user_module.name) == 0) {
        nai_errno = ENOENT;
        r = -1;
        goto _end;
    };

    u = nai_main_local_at(m, nai_https_user_module);
    if (u == 0) {
        nai_errno = ENOENT;
        r = -1;
        goto _end;
    };

    nai_str_sets(&key, name);
    nai_str_hash(&key, 0);
    ent = nai_hashmap_str_find(&u->handles, &key, 0);
    if (ent == 0) {
        nai_errno = ENOENT;
        r = -1;
        goto _end;
    };

    h = (nai_https_user_handle_t*)ent;
    if (pv[0]) {
        pv[0] = h->handle;
    };

    r = 0;

_end:
    return r;
};


