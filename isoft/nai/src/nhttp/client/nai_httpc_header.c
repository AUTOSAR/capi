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
/// @file       nai_httpc_header.c
/// @brief      
/// @details
/// @date       2021-08-19
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai/runtime/nai_hash.h"
#include "nai/runtime/nai_pool.h"
#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_log.h"
#include "nai/runtime/nai_util.h"
#include "nai/service/nai_main.h"
#include "nai_httpc_core.h"



static nai_sult_t nai_httpc_header_in_string(
    nai_pool_t* pool, void* d, 
    const nai_httpc_header_key_t* h, 
    const nai_str_t* k, const nai_str_t* v);
static nai_sult_t nai_httpc_header_in_string_multi(
    nai_pool_t* pool, void* d, 
    const nai_httpc_header_key_t* h, 
    const nai_str_t* k, const nai_str_t* v);
static nai_sult_t nai_httpc_header_in_string_unique(
    nai_pool_t* pool, void* d, 
    const nai_httpc_header_key_t* h, 
    const nai_str_t* k, const nai_str_t* v);


static nai_sult_t nai_httpc_header_out_string(
    nai_pool_t* pool, void* d, 
    const nai_httpc_header_key_t* h, 
    const nai_str_t* k, const nai_str_t* v);
static nai_sult_t nai_httpc_header_out_string_multi(
    nai_pool_t* pool, void* d, 
    const nai_httpc_header_key_t* h, 
    const nai_str_t* k, const nai_str_t* v);
static nai_sult_t nai_httpc_header_out_string_unique(
    nai_pool_t* pool, void* d, 
    const nai_httpc_header_key_t* h, 
    const nai_str_t* k, const nai_str_t* v);


nai_httpc_header_in_keys_t nai_httpc_header_in = {
    .content_length = {
        nai_offsetof(nai_httpc_headers_in_t, content_length), 
        nai_strconst("Content-Length"), 
        nai_httpc_header_in_string_unique, 
    }, 
    .content_range = {
        nai_offsetof(nai_httpc_headers_in_t, content_range), 
        nai_strconst("Content-Range"), 
        nai_httpc_header_in_string_unique, 
    },
    .content_type = {
        nai_offsetof(nai_httpc_headers_in_t, content_type), 
        nai_strconst("Content-Type"), 
        nai_httpc_header_in_string_unique, 
    }, 
    .content_encoding = {
        nai_offsetof(nai_httpc_headers_in_t, content_encoding), 
        nai_strconst("Content-Encoding"), 
        nai_httpc_header_in_string_unique, 
    }, 
    .connection = {
        nai_offsetof(nai_httpc_headers_in_t, connection), 
        nai_strconst("Connection"), 
        nai_httpc_header_in_string, 
    }, 
    .server = {
        nai_offsetof(nai_httpc_headers_in_t, server), 
        nai_strconst("Server"), 
        nai_httpc_header_in_string_unique, 
    }, 
    .date = {
        nai_offsetof(nai_httpc_headers_in_t, date), 
        nai_strconst("Date"), 
        nai_httpc_header_in_string_unique, 
    }, 
    .location = {
        nai_offsetof(nai_httpc_headers_in_t, location), 
        nai_strconst("Location"), 
        nai_httpc_header_in_string_unique, 
    }, 
    .refresh = {
        nai_offsetof(nai_httpc_headers_in_t, refresh), 
        nai_strconst("Refresh"), 
        nai_httpc_header_in_string_unique, 
    }, 
    .last_modified = {
        nai_offsetof(nai_httpc_headers_in_t, last_modified), 
        nai_strconst("Last-Modified"), 
        nai_httpc_header_in_string_unique, 
    }, 
    .transfer_encoding = {
        nai_offsetof(nai_httpc_headers_in_t, transfer_encoding), 
        nai_strconst("Transfer-Encoding"), 
        nai_httpc_header_in_string_unique, 
    }, 
    .accept_ranges = {
        nai_offsetof(nai_httpc_headers_in_t, accept_ranges), 
        nai_strconst("Accept-Ranges"), 
        nai_httpc_header_in_string_unique, 
    }, 
    .www_authenticate = {
        nai_offsetof(nai_httpc_headers_in_t, www_authenticate), 
        nai_strconst("WWW-Authenticate"), 
        nai_httpc_header_in_string_unique, 
    }, 
    .expires = {
        nai_offsetof(nai_httpc_headers_in_t, expires), 
        nai_strconst("Expires"), 
        nai_httpc_header_in_string_unique, 
    }, 
    .keep_alive = {
        nai_offsetof(nai_httpc_headers_in_t, keep_alive), 
        nai_strconst("Keep-Alive"), 
        nai_httpc_header_in_string, 
    }, 
    .etag = {
        nai_offsetof(nai_httpc_headers_in_t, etag), 
        nai_strconst("Etag"), 
        nai_httpc_header_in_string_unique, 
    }, 
    .set_cookie = {
        nai_offsetof(nai_httpc_headers_in_t, cookies), 
        nai_strconst("Set-Cookie"), 
        nai_httpc_header_in_string_multi, 
    }, 
};


nai_httpc_header_out_keys_t nai_httpc_header_out = {
    .content_length = {
        nai_offsetof(nai_httpc_headers_out_t, content_length), 
        nai_strconst("Content-Length"), 
        nai_httpc_header_out_string_unique, 
    }, 
    .content_range = {
        nai_offsetof(nai_httpc_headers_out_t, content_range), 
        nai_strconst("Content-Range"), 
        nai_httpc_header_out_string_unique, 
    }, 
    .content_type = {
        nai_offsetof(nai_httpc_headers_out_t, content_type), 
        nai_strconst("Content-Type"), 
        nai_httpc_header_out_string, 
    }, 
    .connection = {
        nai_offsetof(nai_httpc_headers_out_t, connection), 
        nai_strconst("Connection"), 
        nai_httpc_header_out_string, 
    }, 
    .host = {
        nai_offsetof(nai_httpc_headers_out_t, host), 
        nai_strconst("Host"), 
        nai_httpc_header_out_string_unique, 
    }, 
    .if_modified_since = {
        nai_offsetof(nai_httpc_headers_out_t, if_modified_since), 
        nai_strconst("If-Modified-Since"), 
        nai_httpc_header_out_string_unique, 
    }, 
    .if_unmodified_since = {
        nai_offsetof(nai_httpc_headers_out_t, if_unmodified_since), 
        nai_strconst("If-Unmodified-Since"), 
        nai_httpc_header_out_string_unique, 
    }, 
    .if_match = {
        nai_offsetof(nai_httpc_headers_out_t, if_match), 
        nai_strconst("If-Match"), 
        nai_httpc_header_out_string_unique, 
    }, 
    .if_none_match = {
        nai_offsetof(nai_httpc_headers_out_t, if_none_match), 
        nai_strconst("If-None-Match"), 
        nai_httpc_header_out_string_unique, 
    }, 
    .user_agent = {
        nai_offsetof(nai_httpc_headers_out_t, user_agent), 
        nai_strconst("User-Agent"), 
        nai_httpc_header_out_string, 
    }, 
    .referer = {
        nai_offsetof(nai_httpc_headers_out_t, referer), 
        nai_strconst("Referer"), 
        nai_httpc_header_out_string, 
    }, 
    .range = {
        nai_offsetof(nai_httpc_headers_out_t, range), 
        nai_strconst("Range"), 
        nai_httpc_header_out_string, 
    }, 
    .if_range = {
        nai_offsetof(nai_httpc_headers_out_t, if_range), 
        nai_strconst("If-Range"), 
        nai_httpc_header_out_string_unique, 
    },
    .transfer_encoding = {
        nai_offsetof(nai_httpc_headers_out_t, transfer_encoding), 
        nai_strconst("Transfer-Encoding"), 
        nai_httpc_header_out_string_unique, 
    }, 
    .te = {
        nai_offsetof(nai_httpc_headers_out_t, te), 
        nai_strconst("TE"), 
        nai_httpc_header_out_string, 
    }, 
    .expect = {
        nai_offsetof(nai_httpc_headers_out_t, expect), 
        nai_strconst("Expect"), 
        nai_httpc_header_out_string_unique, 
    }, 
    .upgrade = {
        nai_offsetof(nai_httpc_headers_out_t, upgrade), 
        nai_strconst("Upgrade"), 
        nai_httpc_header_out_string, 
    }, 
    .accept = {
        nai_offsetof(nai_httpc_headers_out_t, accept), 
        nai_strconst("Accept"), 
        nai_httpc_header_out_string, 
    }, 
    .accept_language = {
        nai_offsetof(nai_httpc_headers_out_t, accept_language), 
        nai_strconst("Accept-Language"), 
        nai_httpc_header_out_string, 
    }, 
    .accept_encoding = {
        nai_offsetof(nai_httpc_headers_out_t, accept_encoding), 
        nai_strconst("Accept-Encoding"), 
        nai_httpc_header_out_string, 
    }, 
    .via = {
        nai_offsetof(nai_httpc_headers_out_t, via), 
        nai_strconst("Via"), 
        nai_httpc_header_out_string, 
    }, 
    .authorization = {
        nai_offsetof(nai_httpc_headers_out_t, authorization), 
        nai_strconst("Authorization"), 
        nai_httpc_header_out_string_unique, 
    }, 
    .keep_alive = {
        nai_offsetof(nai_httpc_headers_out_t, keep_alive), 
        nai_strconst("Keep-Alive"), 
        nai_httpc_header_out_string, 
    }, 
    .cookie = {
        -1, 
        nai_strconst("Cookie"), 
        nai_httpc_header_out_string_multi, 
    }, 
};


static nai_sult_t nai_httpc_header_in_string(
    nai_pool_t* pool, void* d, 
    const nai_httpc_header_key_t* h, 
    const nai_str_t* k, const nai_str_t* v)
{
    nai_sult_t rc;
    nai_httpc_header_t* e;
    nai_httpc_header_t** p;
    nai_httpc_headers_in_t* in = (nai_httpc_headers_in_t*)d;


    p = (nai_httpc_header_t**)((uint8_t*)d + h->offset);
    e = nai_palloc(pool, sizeof(*e));
    if (e == 0) {
        rc = nai_sult_from_errno();
        nai_log_alert(NAI_LOG_HTTPC, 
            nai_sult_to_errno(rc), "alloc input header node failed");
        goto _end;
    };

    nai_list_insert_tail(&in->list, &e->ent);
    p[0] = e;
    e->name = *k;
    e->value = *v;
    rc = 0;

_end:
    return rc;
};


static nai_sult_t nai_httpc_header_in_string_unique(
    nai_pool_t* pool, void* d, 
    const nai_httpc_header_key_t* h, 
    const nai_str_t* k, const nai_str_t* v)
{
    nai_sult_t rc;
    nai_httpc_header_t* e;
    nai_httpc_header_t** p;
    nai_httpc_headers_in_t* in = (nai_httpc_headers_in_t*)d;


    p = (nai_httpc_header_t**)((uint8_t*)d + h->offset);
    if (*p) {
        nai_log_error(NAI_LOG_HTTPC, EEXIST, 
            "client sent repeated header ‘%s: %s', previous '%s: %s'", 
            nai_str(k), nai_str(v), 
            nai_str(&(*p)->name), nai_str(&(*p)->value));
        rc = NAI_HTTPC_BAD_RESPONSE;
        goto _end;
    };

    e = nai_palloc(pool, sizeof(*e));
    if (e == 0) {
        rc = nai_sult_from_errno();
        nai_log_alert(NAI_LOG_HTTPC, 
            nai_sult_to_errno(rc), "alloc input header node failed");
        goto _end;
    };

    nai_list_insert_tail(&in->list, &e->ent);
    p[0] = e;
    e->name = *k;
    e->value = *v;
    rc = 0;

_end:
    return rc;
};


static nai_sult_t nai_httpc_header_in_string_multi(
    nai_pool_t* pool, void* d, 
    const nai_httpc_header_key_t* h, 
    const nai_str_t* k, const nai_str_t* v)
{
    nai_sult_t rc;
    nai_array_t* a;
    nai_httpc_header_t* e;
    nai_httpc_header_t** p;
    nai_httpc_headers_in_t* in = (nai_httpc_headers_in_t*)d;


    e = nai_palloc(pool, sizeof(*e));
    if (e == 0) {
        rc = nai_sult_from_errno();
        nai_log_alert(NAI_LOG_HTTPC, 
            nai_sult_to_errno(rc), "alloc input header node failed");
        goto _end;
    };

    a = (nai_array_t*)((uint8_t*)d + h->offset);
    p = (nai_httpc_header_t**)nai_array_push(a);
    if (p == 0) {
        rc = nai_sult_from_errno();
        nai_log_alert(NAI_LOG_HTTPC, 
            nai_sult_to_errno(rc), "push input multi header failed");
        goto _end;
    };

    nai_list_insert_tail(&in->list, &e->ent);
    e->name = *k;
    e->value = *v;
    p[0] = e;
    rc = 0;

_end:
    return rc;
};


static nai_sult_t nai_httpc_header_out_string(
    nai_pool_t* pool, void* d, 
    const nai_httpc_header_key_t* h, 
    const nai_str_t* k, const nai_str_t* v)
{
    nai_sult_t rc;
    nai_httpc_header_t* e;
    nai_httpc_header_t** p;
    nai_httpc_headers_out_t* out = (nai_httpc_headers_out_t*)d;


    p = (nai_httpc_header_t**)((uint8_t*)d + h->offset);
    e = nai_palloc(pool, sizeof(*e));
    if (e == 0) {
        rc = nai_sult_from_errno();
        nai_log_alert(NAI_LOG_HTTPC, 
            nai_sult_to_errno(rc), "alloc input header node failed");
        goto _end;
    };

    nai_list_insert_tail(&out->list, &e->ent);
    e->name = *k;
    e->value = *v;
    p[0] = e;
    rc = 0;

_end:
    return rc;
};


static nai_sult_t nai_httpc_header_out_string_unique(
    nai_pool_t* pool, void* d, 
    const nai_httpc_header_key_t* h, 
    const nai_str_t* k, const nai_str_t* v)
{
    nai_sult_t rc;
    nai_httpc_header_t* e;
    nai_httpc_header_t** p;
    nai_httpc_headers_out_t* out = (nai_httpc_headers_out_t*)d;


    p = (nai_httpc_header_t**)((uint8_t*)d + h->offset);
    if (*p) {
        e = *p;
    } else {;
        e = nai_palloc(pool, sizeof(*e));
        if (e == 0) {
            rc = nai_sult_from_errno();
            nai_log_alert(NAI_LOG_HTTPC, 
                nai_sult_to_errno(rc), "alloc output header node failed");
            goto _end;
        };

        nai_list_insert_tail(&out->list, &e->ent);
        p[0] = e;
    };

    e->name = *k;
    e->value = *v;
    rc = 0;

_end:
    return rc;
};


static nai_sult_t nai_httpc_header_out_string_multi(
    nai_pool_t* pool, void* d, 
    const nai_httpc_header_key_t* h, 
    const nai_str_t* k, const nai_str_t* v)
{
    nai_sult_t rc;
    nai_array_t* a;
    nai_httpc_header_t* e;
    nai_httpc_header_t** p;
    nai_httpc_headers_out_t* out = (nai_httpc_headers_out_t*)d;


    e = nai_palloc(pool, sizeof(*e));
    if (e == 0) {
        rc = nai_sult_from_errno();
        nai_log_alert(NAI_LOG_HTTPC, 
            nai_sult_to_errno(rc), "alloc output header node failed");
        goto _end;
    };

    if (h->offset != -1) {
        a = (nai_array_t*)((uint8_t*)d + h->offset);
        p = (nai_httpc_header_t**)nai_array_push(a);
        if (p == 0) {
            rc = nai_sult_from_errno();
            nai_log_alert(NAI_LOG_HTTPC, 
                nai_sult_to_errno(rc), "push output multi header failed");
            goto _end;
        };

        p[0] = e;
    };

    nai_list_insert_tail(&out->list, &e->ent);
    e->name = *k;
    e->value = *v;
    rc = 0;

_end:
    return rc;
};


static struct {
    nai_int_t inited;
} nai_httpc_header = {
    0
};


#define nai_httpc_header_in_count  ((nai_int_t)         \
    (sizeof(nai_httpc_header_in) /                      \
     sizeof(nai_httpc_header_key_t)))                   \

#define nai_httpc_header_out_count ((nai_int_t)         \
    (sizeof(nai_httpc_header_out) /                     \
     sizeof(nai_httpc_header_key_t)))                   \


static nai_sult_t nai_httpc_header_init()
{
    nai_int_t n, m;
    nai_int_t count;
    nai_httpc_header_key_t* elts;


    if (nai_httpc_header.inited == 0) {
        for (m = 0; m < 2; m ++) {
            if (m == 0) {
                count = nai_httpc_header_in_count;
                elts = (nai_httpc_header_key_t*)&nai_httpc_header_in;
            } else {
                count = nai_httpc_header_out_count;
                elts = (nai_httpc_header_key_t*)&nai_httpc_header_out;
            };
            for (n = 0; n < count; n ++) {
                nai_str_hash(&elts[n].name, 1);
            };
        };

        nai_httpc_header.inited = 1;
    };

    (void)nai_httpc_header_in_string;
    return 0;
};


nai_sult_t nai_httpc_header_tables_init(nai_httpc_t* h)
{
    nai_int_t n, l;
    nai_int_t count;
    nai_sult_t rc;
    nai_main_t* m;
    nai_hashsnap_t* snap;
    nai_hash_value_t* elts;
    nai_httpc_header_key_t* keys;


    nai_httpc_header_init();


    count = nai_httpc_header_in_count;
    if (count < nai_httpc_header_out_count) {
        count = nai_httpc_header_out_count;
    };

    m = h->main;
    elts = nai_palloc(nai_main_get_temp_pool(m), sizeof(*elts) * count);
    if (elts == 0) {
        rc = nai_sult_from_errno();
        goto _end;
    };

    for (l = 0; l < 2; l ++) {
        if (l == 0) {
            snap = &h->headers_in;
            keys = (nai_httpc_header_key_t*)&nai_httpc_header_in;
            count = nai_httpc_header_in_count;
        } else {
            snap = &h->headers_out;
            keys = (nai_httpc_header_key_t*)&nai_httpc_header_out;
            count = nai_httpc_header_out_count;
        };

        for (n = 0; n < count; n ++) {
            elts[n].key = keys[n].name;
            elts[n].value = &keys[n];
        };

        rc = nai_hashsnap_build(snap, elts, count, 512, 64);
        if (rc < 0) {
            rc = nai_sult_from_errno();
            goto _end;
        };
    };

_end:
    return rc;
};


nai_sult_t nai_httpc_headers_set(
    nai_httpc_request_t* r, void* h, 
    const nai_httpc_header_key_t* key, const nai_str_t* value, 
    nai_int_t flags)
{
    nai_sultp_t rc;
    nai_str_t dval;


    if (flags & NAI_HTTP_DUP_VALUE) {
        rc = nai_str_dup(&dval, nai_str(value), nai_str_len(value), r->pool);
        if (rc < 0) {
            rc = nai_sult_from_errno();
            goto _end;
        };
    } else {
        dval = *value;
    };

    rc = key->set(r->pool, h, key, &key->name, &dval);

_end:
    return (nai_sult_t)rc;
};


nai_sult_t nai_httpc_headers_in_add(
    nai_httpc_request_t* r, 
    const nai_str_t* key, const nai_str_t* value, nai_int_t flags)
{
    nai_sultp_t rc;
    nai_str_t name;
    nai_str_t dkey;
    nai_str_t dval;
    nai_list_entry_t* ent;
    nai_list_entry_t* list;
    nai_httpc_t* h;
    nai_httpc_header_t* e;
    nai_httpc_header_t* f;
    nai_httpc_header_key_t* hkey;


    h = nai_httpc_get_service(r);

    name = *key;
    nai_str_hash(&name, 1);

    /* lookup predefined key */
    hkey = nai_hashsnap_find(&h->headers_in, &name, 1);
    if (hkey) {
        rc = nai_httpc_headers_set(r, &r->headers_in, hkey, value, flags);
        goto _end;
    };

    /* duplicate value */
    if (flags & NAI_HTTP_DUP_VALUE) {
        rc = nai_str_dup(&dval, nai_str(value), nai_str_len(value), r->pool);
        if (rc < 0) {
            rc = nai_sult_from_errno();
            goto _end;
        };
    } else {
        dval = *value;
    };

    e = 0;

    /* lookup exists header */
    if (flags & NAI_HTTP_SET) {
        list = &r->headers_in.list;
        ent = list->next;
        for ( ; ent != list; ent = ent->next) {
            f = (nai_httpc_header_t*)ent;
            if (nai_str_hashcaseeq(&f->name, &name)) {
                e = f;
                break;
            };
        };
    };

    /* set or insert header */
    if (e != 0) {
        e->value = dval;
    } else {
        e = nai_palloc(r->pool, sizeof(*e));
        if (e == 0) {
            rc = nai_sult_from_errno();
            goto _end;
        };

        if (flags & NAI_HTTP_DUP_KEY) {
            rc = nai_str_dup(&dkey, nai_str(key), nai_str_len(key), r->pool);
            if (rc < 0) {
                rc = nai_sult_from_errno();
                goto _end;
            };

            dkey.hash = name.hash;
        } else {
            dkey = name;
        };

        e->name = dkey;
        e->value = dval;
        nai_list_insert_tail(&r->headers_in.list, &e->ent);
    };

    rc = 0;

_end:
    return (nai_sult_t)rc;
};


nai_sult_t nai_httpc_headers_out_add(
    nai_httpc_request_t* r, 
    const nai_str_t* key, const nai_str_t* value, nai_int_t flags)
{
    nai_sultp_t rc;
    nai_str_t name;
    nai_str_t dkey;
    nai_str_t dval;
    nai_list_entry_t* ent;
    nai_list_entry_t* list;
    nai_httpc_t* h;
    nai_httpc_header_t* e;
    nai_httpc_header_t* f;
    nai_httpc_header_key_t* hkey;


    h = nai_httpc_get_service(r);

    name = *key;
    nai_str_hash(&name, 1);

    /* lookup predefined key */
    hkey = nai_hashsnap_find(&h->headers_out, &name, 1);
    if (hkey) {
        rc = nai_httpc_headers_set(r, &r->headers_out, hkey, value, flags);
        goto _end;
    };

    /* duplicate value */
    if (flags & NAI_HTTP_DUP_VALUE) {
        rc = nai_str_dup(&dval, nai_str(value), nai_str_len(value), r->pool);
        if (rc < 0) {
            rc = nai_sult_from_errno();
            goto _end;
        };
    } else {
        dval = *value;
    };

    e = 0;

    /* lookup exists header */
    if (flags & NAI_HTTP_SET) {
        list = &r->headers_out.list;
        ent = list->next;
        for ( ; ent != list; ent = ent->next) {
            f = (nai_httpc_header_t*)ent;
            if (nai_str_hashcaseeq(&f->name, &name)) {
                e = f;
                break;
            };
        };
    };

    /* set or insert header */
    if (e != 0) {
        e->value = dval;
    } else {
        e = nai_palloc(r->pool, sizeof(*e));
        if (e == 0) {
            rc = nai_sult_from_errno();
            goto _end;
        };

        if (flags & NAI_HTTP_DUP_KEY) {
            rc = nai_str_dup(&dkey, nai_str(key), nai_str_len(key), r->pool);
            if (rc < 0) {
                rc = nai_sult_from_errno();
                goto _end;
            };

            dkey.hash = name.hash;
        } else {
            dkey = name;
        };

        e->name = dkey;
        e->value = dval;
        nai_list_insert_tail(&r->headers_out.list, &e->ent);
    };

    rc = 0;

_end:
    return (nai_sult_t)rc;
};


nai_sult_t nai_httpc_headers_in_find(
    nai_httpc_request_t* r, 
    const nai_str_t* key, nai_httpc_header_t** pv)
{
    nai_sult_t rc;
    nai_str_t name;
    nai_list_entry_t* e;
    nai_list_entry_t* list;
    nai_httpc_header_t* h;


    name = *key;
    nai_str_hash(&name, 1);

    list = &r->headers_in.list;
    e = list->next;
    for ( ; e != list; e = e->next) {
        h = (nai_httpc_header_t*)e;
        if (nai_str_hashcaseeq(&h->name, &name)) {
            if (pv) {
                pv[0] = h;
            };
            rc = 0;
            goto _end;
        };
    };

    rc = nai_errno_to_sult(ENOENT);

_end:
    return rc;
};


nai_sult_t nai_httpc_headers_out_find(
    nai_httpc_request_t* r, 
    const nai_str_t* key, nai_httpc_header_t** pv)
{
    nai_sult_t rc;
    nai_str_t name;
    nai_list_entry_t* e;
    nai_list_entry_t* list;
    nai_httpc_header_t* h;


    name = *key;
    nai_str_hash(&name, 1);

    list = &r->headers_out.list;
    e = list->next;
    for ( ; e != list; e = e->next) {
        h = (nai_httpc_header_t*)e;
        if (nai_str_hashcaseeq(&h->name, &name)) {
            if (pv) {
                pv[0] = h;
            };
            rc = 0;
            goto _end;
        };
    };

    rc = nai_errno_to_sult(ENOENT);

_end:
    return rc;
};


