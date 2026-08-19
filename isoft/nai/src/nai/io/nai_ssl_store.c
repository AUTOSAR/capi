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
/// @file       nai_ssl_store.c
/// @brief      
/// @details
/// @date       2023-04-03
/// @author     xn
/// @version    1.2.0
///
/// ================================================================



#include "nai/io/nai_ssl.h"


#if (NAI_HAVE_SSL)


#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_log.h"
#include "nai/runtime/nai_util.h"
#include "nai/runtime/nai_list.h"
#include "nai/runtime/nai_rbtree.h"
#include "nai/runtime/nai_zone.h"
#include "nai/os/nai_thread.h"



typedef struct nai_ssl_mement_s nai_ssl_mement_t;
typedef struct nai_ssl_memmap_s nai_ssl_memmap_t;
typedef struct nai_ssl_memstore_s nai_ssl_memstore_t;


struct nai_ssl_mement_s {
    nai_rbnode_t node;
    nai_list_t ent;
    uint32_t hash;
    uint32_t klen;
    uint32_t vlen;
    time_t expire;
};


struct nai_ssl_memmap_s {
    nai_rbtree_t map;
    nai_list_t qexpire;
    nai_atomic32_t lock;
};


struct nai_ssl_memstore_s {
    nai_ssl_memmap_t* map;
    nai_zone_t* zone;
    union {
        struct {
            uint32_t own:1;
            uint32_t create:1;
            uint32_t shared:1;
        };
        uint32_t flags;
    };
};


#define nai_ssl_mement_kptr(e)                          \
    ((char*)((e) + 1))                                  \


#define nai_ssl_mement_vptr(e)                          \
    ((char*)((e) + 1) + (e)->klen)                      \



static nai_int_t nai_ssl_memmap_lock(nai_ssl_memmap_t* m)
{
    nai_int_t r;


    nai_atomic32_lock(&m->lock);
    r = 0;

    return r;
};


static nai_int_t nai_ssl_memmap_unlock(nai_ssl_memmap_t* m)
{
    nai_int_t r;


    nai_atomic32_unlock(&m->lock);
    r = 0;

    return r;
};


static nai_rbnode_t** nai_ssl_memmap_find(
    nai_ssl_memmap_t* m, const nai_mem_t* k, nai_rbnode_t** pparent)
{
    nai_int_t c;
    char* s;
    nai_rbnode_t** n = &nai_rbtree_root(&m->map);
    nai_rbnode_t* parent = nai_rbtree_end(&m->map);
    nai_ssl_mement_t* e;


    while (*n) {
        parent = *n;
        e = nai_containof(parent, nai_ssl_mement_t, node);
        if (e->hash != k->hash) {
            if (e->hash > k->hash) {
                n = &parent->rb_left;
            } else {
                n = &parent->rb_right;
            };
            continue;
        };

        if (e->klen != nai_str_len(k)) {
            if (e->klen > nai_str_len(k)) {
                n = &parent->rb_left;
            } else {
                n = &parent->rb_right;
            };
            continue;
        };

        s = nai_ssl_mement_kptr(e);
        c = nai_memcmp(s, nai_str(k), e->klen);
        if (c == 0) {
            break;
        } else if (c >= 0) {
            n = &parent->rb_left;
        } else {
            n = &parent->rb_right;
        };
    };

    if (pparent) {
        pparent[0] = parent;
    };

    return n;
};


static nai_int_t nai_ssl_memstore_set(
    void* ud, const nai_mem_t* k, const nai_mem_t* v, time_t expire)
{
    nai_int_t r;
    nai_int_t ec;
    size_t len;
    nai_rbnode_t** n;
    nai_rbnode_t* parent;
    nai_mem_t key;
    nai_ssl_mement_t* e;
    nai_ssl_memmap_t* m;
    nai_ssl_memstore_t* s;


    s = (nai_ssl_memstore_t*)ud;
    m = s->map;

    key = *k;
    nai_str_hash(&key, 0);
    nai_ssl_memmap_lock(m);


    n = nai_ssl_memmap_find(m, &key, &parent);
    if (n[0] != 0) {
        e = nai_containof(n[0], nai_ssl_mement_t, node);
        if (e->vlen > nai_str_len(v)) {
            e->expire = expire;
            e->vlen = nai_str_len(v);
            nai_memcpy(nai_ssl_mement_vptr(e), nai_str(v), e->vlen);
            nai_list_insert_tail(&m->qexpire, &e->ent);
            n = 0;
        } else {
            nai_list_entry_remove(&e->ent);
            nai_rbtree_erase(&m->map, &e->node);
            nai_zfree(s->zone, e);
            n = nai_ssl_memmap_find(m, &key, &parent);
        };
    };

    if (n != 0) {
        len = nai_str_len(k) + nai_str_len(v);
        e = (nai_ssl_mement_t*)nai_zalloc(s->zone, sizeof(*e) + len);
        if (e == 0) {
            if (!nai_list_is_empty(&m->qexpire)) {
                e = nai_containof(m->qexpire.next, nai_ssl_mement_t, ent);
                nai_list_entry_remove(&e->ent);
                nai_rbtree_erase(&m->map, &e->node);
                nai_zfree(s->zone, e);
            };

            e = (nai_ssl_mement_t*)nai_zalloc(s->zone, sizeof(*e) + len);
            if (e == 0) {
                ec = nai_errno;
                r = -1;
                goto _fail;
            };
        };

        e->expire = expire;
        e->hash = key.hash;
        e->klen = key.len;
        e->vlen = v->len;
        nai_memcpy(nai_ssl_mement_kptr(e), nai_str(k), e->klen);
        nai_memcpy(nai_ssl_mement_vptr(e), nai_str(v), e->vlen);

        nai_rbtree_link(&m->map, &e->node, parent, n);
        nai_rbtree_color(&m->map, &e->node);
        nai_list_insert_tail(&m->qexpire, &e->ent);
    };


    nai_ssl_memmap_unlock(m);
    r = 0;

_end:
    return r;

_fail:
    ec = nai_errno;
    nai_ssl_memmap_unlock(m);
    nai_errno = ec;
    goto _end;
};


static nai_int_t nai_ssl_memstore_get(
    void* ud, const nai_mem_t* k, nai_mem_t* v)
{
    nai_int_t r;
    nai_int_t ec;
    time_t now;
    nai_rbnode_t** n;
    nai_mem_t key;
    nai_ssl_mement_t* e;
    nai_ssl_memmap_t* m;
    nai_ssl_memstore_t* s;


    s = (nai_ssl_memstore_t*)ud;
    m = s->map;

    now = nai_time()/(1000*1000);
    key = *k;
    nai_str_hash(&key, 0);
    nai_ssl_memmap_lock(m);


    n = nai_ssl_memmap_find(m, &key, 0);
    if (n[0] == 0) {
        ec = ENOENT;
        r = -1;
        goto _fail;
    };

    e = nai_containof(n[0], nai_ssl_mement_t, node);
    if (e->expire <= now) {
        nai_list_entry_remove(&e->ent);
        nai_rbtree_erase(&m->map, &e->node);
        nai_zfree(s->zone, e);
        ec = ENOENT;
        r = -1;
        goto _fail;
    };

    r = e->vlen;
    if (v == 0 || (nai_int_t)nai_str_len(v) < r) {
        ec = ERANGE;
        goto _fail;
    };

    nai_memcpy(nai_str(v), nai_ssl_mement_vptr(e), r);
    nai_str_setl(v, r);

    nai_ssl_memmap_unlock(m);

_end:
    return r;

_fail:
    nai_ssl_memmap_unlock(m);
    nai_errno = ec;
    goto _end;
};


static nai_int_t nai_ssl_memstore_unset(void* ud, const nai_mem_t* k)
{
    nai_int_t r;
    nai_rbnode_t** n;
    nai_mem_t key;
    nai_ssl_mement_t* e;
    nai_ssl_memmap_t* m;
    nai_ssl_memstore_t* s;


    s = (nai_ssl_memstore_t*)ud;
    m = s->map;

    key = *k;
    nai_str_hash(&key, 0);
    nai_ssl_memmap_lock(m);


    n = nai_ssl_memmap_find(m, &key, 0);
    if (n[0] != 0) {
        e = nai_containof(n[0], nai_ssl_mement_t, node);
        nai_list_entry_remove(&e->ent);
        nai_rbtree_erase(&m->map, &e->node);
        nai_zfree(s->zone, e);
    };


    nai_ssl_memmap_unlock(m);
    r = 0;

    return r;
};


nai_int_t nai_ssl_memstore_close(void* ud)
{
    nai_int_t r;
    nai_ssl_memstore_t* s;


    s = (nai_ssl_memstore_t*)ud;
    if (s->own) {
        nai_free(s->map);
    };

    nai_free(s);
    r = 0;

    return r;
};


static nai_ssl_store_ops_t nai_ssl_memstore = {
    nai_ssl_memstore_set, 
    nai_ssl_memstore_get, 
    nai_ssl_memstore_unset, 
    nai_ssl_memstore_close
};


static nai_ssl_memstore_t* 
    nai_ssl_memstore_from(void* mem, size_t size, nai_int_t own)
{
    nai_zone_t* z;
    nai_ssl_memmap_t* m;
    nai_ssl_memstore_t* s;


    assert(size >= 4096);


    m = (nai_ssl_memmap_t*)mem;
    z = (nai_zone_t*)(m + 1);
    z = nai_zone_from(z, 
        size - ((uint8_t*)z - (uint8_t*)mem), NAI_ZONE_MEMORY, 0);
    if (z == 0) {
        s = 0;
        goto _end;
    };

    s = (nai_ssl_memstore_t*)nai_malloc(sizeof(*s));
    if (s == 0) {
        goto _end;
    };

    s->map = m;
    s->zone = z;
    s->flags = 0;
    s->own = !!(own);
    s->shared = 0;
    m->lock = 0;
    nai_list_init(&m->qexpire);
    nai_rbtree_init(&m->map);


_end:
    return s;
};


nai_int_t nai_ssl_store_open(nai_ssl_store_t* s, size_t size)
{
    nai_int_t r;
    nai_int_t ec;
    void* mem;
    nai_ssl_memstore_t* m;


    if (size < 4096) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    mem = nai_malloc(size);
    if (mem == 0) {
        r = -1;
        goto _end;
    };

    m = nai_ssl_memstore_from(mem, size, 1);
    if (m == 0) {
        ec = nai_errno;
        r = -1;
        goto _fail;
    };

    s->ops = &nai_ssl_memstore;
    s->ud = m;
    r = 0;

_end:
    return r;

_fail:
    nai_free(mem);
    nai_errno = ec;
    goto _end;
};


nai_int_t nai_ssl_store_from(nai_ssl_store_t* s, void* mem, size_t size)
{
    nai_int_t r;
    nai_ssl_memstore_t* m;


    if (size < 4096) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    m = nai_ssl_memstore_from(mem, size, 0);
    if (m == 0) {
        r = -1;
        goto _end;
    };

    s->ops = &nai_ssl_memstore;
    s->ud = m;
    r = 0;

_end:
    return r;
};


nai_int_t nai_ssl_store_set(
    nai_ssl_store_t* s, const nai_mem_t* k, const nai_mem_t* v, 
    time_t expire)
{
    nai_int_t r;


    if (s->ops == 0) {
        nai_errno = EPERM;
        r = -1;
        goto _end;
    };

    r = s->ops->set(s->ud, k, v, expire);

_end:
    return r;
};


nai_int_t nai_ssl_store_get(
    nai_ssl_store_t* s, const nai_mem_t* k, nai_mem_t* v)
{
    nai_int_t r;


    if (s->ops == 0) {
        nai_errno = EPERM;
        r = -1;
        goto _end;
    };

    r = s->ops->get(s->ud, k, v);

_end:
    return r;
};


nai_int_t nai_ssl_store_unset(nai_ssl_store_t* s, const nai_mem_t* k)
{
    nai_int_t r;


    if (s->ops == 0) {
        nai_errno = EPERM;
        r = -1;
        goto _end;
    };

    r = s->ops->unset(s->ud, k);

_end:
    return r;
};


nai_int_t nai_ssl_store_close(nai_ssl_store_t* s)
{
    nai_int_t r;


    if (s->ops == 0) {
        r = 0;
        goto _end;
    };

    r = s->ops->close(s->ud);
    if (r < 0) {
        goto _end;
    };

    nai_ssl_store_init(s);

_end:
    return r;
};



#endif


