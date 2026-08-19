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
/// @file       nai_https_script.c
/// @brief      
/// @details
/// @date       2021-09-22
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai_https_core.h"
#include "nhttp/server/nai_https_script.h"
#include "nai/service/nai_main.h"
#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_log.h"


static nai_sult_t nai_https_prepare_values(nai_https_request_t* r)
{
    nai_int_t n;
    nai_int_t count;
    nai_sult_t rc;
    nai_https_t* h;


    if (r->values) {
        rc = 0;
        goto _end;
    };

    h = nai_https_get_service(r);
    count = (nai_int_t)nai_script_count_variable(&h->vars);

    /* alloc variable values */
    r->values = (nai_script_value_t*)
        nai_palloc(r->pool, sizeof(*r->values) * count);
    if (r->values == 0) {
        rc = nai_sult_from_errno();
        nai_log_alert(NAI_LOG_HTTPS, 
            nai_sult_to_errno(rc), "allocate variable values failed");
        goto _end;
    };

    for (n = 0; n < count; n ++) {
        r->values[n].ptr = 0;
        r->values[n].info = 0;
    };

    rc = 0;

_end:
    return rc;
};


nai_sult_t nai_https_script_exec(
    nai_https_request_t* r, const void* code, nai_script_value_t* sp)
{
    nai_sult_t rc;
    nai_https_t* h;
    nai_script_t s;


    rc = nai_https_prepare_values(r);
    if (rc < 0) {
        goto _end;
    };

    h = nai_https_get_service(r);
    nai_script_init(&s, &h->vars);

    s.values = r->values;
    s.sp = sp;
    s.ctx = r;
    s.pos = 0;
    rc = nai_script_exec(&s, code);

_end:
    return rc;
};


nai_sult_t nai_https_expn_value(
    nai_https_request_t* r, const nai_script_expn_t* e, 
    nai_str_t* v, const nai_reserved_t* rev)
{
    nai_sult_t rc;
    nai_https_t* h;
    nai_script_t s;


    rc = nai_https_prepare_values(r);
    if (rc < 0) {
        goto _end;
    };

    h = nai_https_get_service(r);
    nai_script_init(&s, &h->vars);

    s.values = r->values;
    s.ctx = r;
    rc = nai_script_expn_value(&s, e, v, rev, r->pool);

_end:
    return rc;
};


nai_sult_t nai_https_command_expn(
    nai_command_t* c, nai_command_args_t* args)
{
    nai_sult_t rc;
    nai_str_t str;
    nai_https_t* h;
    nai_script_expn_t* e;


    rc = nai_value_dups(&args->argv[0], &str, args->pool);
    if (rc < 0) {
        nai_log_error(NAI_LOG_CORE, 
            nai_sult_to_errno(rc), 
            "command '%s' get string failed", c->name);
        goto _end;
    };

    h = nai_https_get_service(args);
    e = (nai_script_expn_t*)(args->data + c->offset);
    rc = nai_https_expn_compile(h, e, &str);
    if (rc < 0) {
        nai_log_error(NAI_LOG_CORE, 
            nai_sult_to_errno(rc), 
            "command '%s' compile expn '%s' failed", c->name, nai_str(&str));
        goto _end;
    };

_end:
    return rc;
};


#if (NAI_HAVE_REGEX)


static nai_sult_t nai_https_var_not_found(
    void* c, uintptr_t ud, nai_script_value_t* v)
{
    (void)c;
    (void)ud;

    v->not_found = 1;
    v->len = 0;

    return 0;
};


nai_sult_t nai_https_regex_compile(
    nai_https_t* h, nai_https_regex_t* re, 
    const nai_str_t* pattern, nai_int_t flags, nai_regex_errinfo_t* ei)
{
    nai_int_t r;
    nai_int_t n;
    uint8_t* v;
    nai_sult_t rc;
    nai_main_t* m;
    nai_mem_t name;
    nai_regex_named_t rn;


    m = h->main;
    r = nai_regex_compile(&re->re, nai_str(pattern), flags, ei, &m->pool);
    if (r < 0) {
        rc = nai_sult_from_errno();
        goto _end;
    };

    r = nai_regex_get_named(&re->re, &rn);
    if (r == NAI_REGEX_NO_MATCH) {
        re->var = 0;
        re->count = 0;
        rc = 0;
        goto _end;
    }
    if (r < 0) {
        rc = nai_sult_from_errno();
        goto _end;
    };

    re->name = *pattern;
    re->count = rn.count;
    re->var = (nai_https_regex_var_t*)
        nai_palloc(&m->pool, rn.count * sizeof(*re->var));
    if (re->var) {
        rc = nai_sult_from_errno();
        goto _end;
    };

    for (n = 0; n < rn.count; n ++) {
        v = nai_regex_named_at(&rn, n);
        nai_str_sets(&name, nai_regex_named_str(v));

        rc = nai_script_add_variable(
            &h->vars, &name, nai_https_var_not_found, 0, 0, 0);
        if (rc < 0) {
            goto _end;
        };

        re->var[n].capture = nai_regex_named_id(v);
        re->var[n].index = nai_script_get_variable(&h->vars, &name, 0);
    };

    if (h->ncapture < re->count) {
        h->ncapture = re->count;
    };

    rc = 0;

_end:
    return rc;
};


nai_sult_t nai_https_regex_exec(
    nai_https_request_t* r, nai_https_regex_t* re, const nai_mem_t* str)
{
    nai_int_t n;
    nai_int_t c;
    nai_int_t len;
    nai_sult_t rc;
    nai_https_t* h;
    nai_script_value_t* v;


    if (re->re.sub <= 0) {
        len = 0;
    } else {
        h = nai_https_get_service(r);

        len = nai_regex_match_size(h->ncapture);
        if (r->captures == 0) {
            r->captures = (nai_int_t*)
                nai_palloc(r->pool, len * sizeof(nai_int_t));
            if (r->captures == 0) {
                rc = nai_sult_from_errno();
                goto _end;
            };
        };
    };

    rc = nai_https_prepare_values(r);
    if (rc < 0) {
        goto _end;
    };

    n = nai_regex_exec(&re->re, 
        nai_str(str), nai_str_len(str), r->captures, len);
    if (n == NAI_REGEX_NO_MATCH) {
        rc = NAI_DECLINED;
        goto _end;
    };
    if (n < 0) {
        rc = nai_sult_from_errno();
        goto _end;
    };

    r->ncapture = n;
    r->match_data = nai_str(str);

    for (n = 0; n < re->count; n ++) {
        c = re->var[n].capture;
        v = r->values + re->var[n].index;
        v->valid = 1;
        v->len = r->captures[c*2+1]-r->captures[c*2];
        v->ptr = (char*)r->match_data + r->captures[c*2];
    };

    rc = 0;


_end:
    return rc;
};


#endif


