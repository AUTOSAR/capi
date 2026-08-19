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
/// @file       nai_hash.c
/// @brief      
/// @details
/// @date       2021-01-27
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai/runtime/nai_hash.h"
#include "nai/runtime/nai_pool.h"
#include "nai/runtime/nai_util.h"
#include "nai/runtime/nai_errno.h"



//////////////////////////////////////////////////////////////////////////////
// hash map


nai_int_t nai_hashmap_init(nai_hashmap_t* p, nai_pool_t* pool)
{
    nai_int_t r;


    p->pool = pool;
    p->hash = 0;
    p->hashsize = 0;
    p->hash_voff = 0;
    p->hash_vtype = 0;
    r = 0;

    return r;
};


nai_int_t nai_hashmap_open(nai_hashmap_t* p, size_t hsize, nai_int_t type)
{
    nai_int_t r;
    uint16_t hoff;
    uint16_t htype;


    switch (type) {
    case NAI_HASH_STRING:
        hoff = nai_offsetof(nai_hash_str_t, key.hash);
        htype = NAI_HASH_T32;
        break;
    case NAI_HASH_INT32:
        hoff = nai_offsetof(nai_hash_int32_t, key);
        htype = NAI_HASH_T32;
        break;
    case NAI_HASH_INT64:
        hoff = nai_offsetof(nai_hash_int64_t, key);
        htype = NAI_HASH_T64;
        break;
    default:
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    r = nai_hashmap_open_user(p, hsize, hoff, htype);

_end:
    return r;
};


nai_int_t nai_hashmap_open_user(nai_hashmap_t* p, 
    size_t hsize, uint16_t hoff, uint16_t htype)
{
    nai_int_t r;


    if (htype != NAI_HASH_T32 && 
        htype != NAI_HASH_T64) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    p->hash = (nai_hash_elt_t**)(p->pool ? 
            nai_palloc(p->pool, hsize*sizeof(*p->hash)) : 
            nai_malloc(hsize*sizeof(*p->hash)));
    if (p->hash == 0) {
        r = -1;
        goto _end;
    };

    p->hashsize = (uint32_t)hsize;
    p->hash_voff = hoff;
    p->hash_vtype = htype;

    nai_pzero((void**)p->hash, p->hashsize);
    r = 0;

_end:
    return r;
};


nai_int_t nai_hashmap_close(nai_hashmap_t* p)
{
    nai_int_t r;


    if (p->hash) {
        if (p->pool == 0) {
            nai_free(p->hash);
        };

        p->hash = 0;
    };

    p->hashsize = 0;
    p->hash_voff = 0;
    p->hash_vtype = 0;
    r = 0;

    return r;
};


#define nai_hash_key(e, size, off, type)                    \
    ((type) == NAI_HASH_T32 ?                               \
        (uint32_t)(*(uint32_t*)((uint8_t*)e + off)%(size)) :\
        (uint32_t)(*(uint64_t*)((uint8_t*)e + off)%(size))) \


nai_int_t nai_hashmap_insert(nai_hashmap_t* p, nai_hash_elt_t* e)
{
    nai_int_t r;
    uint32_t key;


    key = nai_hash_key(e, p->hashsize, p->hash_voff, p->hash_vtype);
    e->next = p->hash[key];
    p->hash[key] = e;
    r = 0;

    return r;
};


nai_int_t nai_hashmap_remove(nai_hashmap_t* p, nai_hash_elt_t* e)
{
    nai_int_t r;
    uint32_t key;
    nai_hash_elt_t* list;
    nai_hash_elt_t** next;


    key = nai_hash_key(e, p->hashsize, p->hash_voff, p->hash_vtype);

    next = &p->hash[key];
    for (;;) {
        list = *next;
        if (list == 0) {
            nai_errno = EEXIST;
            r = -1;
            break;
        };
        if (list == e) {
            next[0] = e->next;
            r = 0;
            break;
        };

        next = &list->next;
    };

    return r;
};


nai_hash_elt_t* nai_hashmap_next(nai_hashmap_t* p, nai_hash_elt_t* e)
{
    uint32_t key;
    nai_hash_elt_t* r;


    if (e == 0) {
        key = 0;
    } else if (e->next) {
        r = e->next;
        goto _end;
    } else {
        key = nai_hash_key(e, p->hashsize, p->hash_voff, p->hash_vtype);
    };

    for ( ; ; key ++) {
        if (key >= p->hashsize) {
            r = 0;
            break;
        };
        r = p->hash[key];
        if (r) {
            break;
        };
    };

_end:
    return r;
};


nai_hash_elt_t* nai_hashmap_str_find(
    nai_hashmap_t* p, const nai_str_t* s, nai_int_t cases)
{
    nai_hash_elt_t* r;
    nai_hash_str_t* e;


    r = p->hash[s->hash % p->hashsize];
    if (cases == 0) {
        for ( ; r; r = r->next) {
            e = (nai_hash_str_t*)r;
            if (nai_str_hasheq(&e->key, s)) {
                break;
            };
        };
    } else {
        for ( ; r; r = r->next) {
            e = (nai_hash_str_t*)r;
            if (nai_str_hashcaseeq(&e->key, s)) {
                break;
            };
        };
    };

    return r;
};


nai_hash_elt_t* nai_hashmap_ptr_find(nai_hashmap_t* p, const void* key)
{
    nai_hash_elt_t* r;
    nai_hash_intptr_t* e;


    r = p->hash[(intptr_t)key % p->hashsize];
    for ( ; r; r = r->next) {
        e = (nai_hash_intptr_t*)r;
        if (e->key.p == key) {
            break;
        };
    };

    return r;
};


nai_hash_elt_t* nai_hashmap_int32_find(nai_hashmap_t* p, uint32_t key)
{
    nai_hash_elt_t* r;
    nai_hash_int32_t* e;


    r = p->hash[key % p->hashsize];
    for ( ; r; r = r->next) {
        e = (nai_hash_int32_t*)r;
        if (e->key.u == key) {
            break;
        };
    };

    return r;
};


nai_hash_elt_t* nai_hashmap_int64_find(nai_hashmap_t* p, uint64_t key)
{
    nai_hash_elt_t* r;
    nai_hash_int64_t* e;


    r = p->hash[key % p->hashsize];
    for ( ; r; r = r->next) {
        e = (nai_hash_int64_t*)r;
        if (e->key.u == key) {
            break;
        };
    };

    return r;
};


//////////////////////////////////////////////////////////////////////////////
// hash snap


typedef struct nai_hashsnap_elt_s {
    void* value;
    union {
        struct {
            uint32_t len:16;
            uint32_t hash:14;
            uint32_t next:2;
        };
        struct {
            uint32_t hashlen:30;
            uint32_t unuse:2;
        };
    };
    char key[0];
} nai_hashsnap_elt_t;


#define nai_hashsnap_eltsize(len)                           \
    nai_align(len +                                         \
    nai_offsetof(nai_hashsnap_elt_t, key), sizeof(void*))   \


nai_int_t nai_hashsnap_init(nai_hashsnap_t* p, nai_pool_t* pool)
{
    p->pool = pool;
    p->snap = 0;
    p->hashsize = 0;
    return 0;
};


nai_int_t nai_hashsnap_close(nai_hashsnap_t* p)
{
    if (p->snap) {
        if (p->pool == 0) {
            nai_free(p->snap);
        };
        p->snap = 0;
        p->hashsize = 0;
    };

    return 0;
};


nai_int_t nai_hashsnap_build(
    nai_hashsnap_t* p, const nai_hash_value_t* v, 
    nai_int_t count, nai_int_t hsize_max, size_t bucket)
{
    nai_int_t r;
    nai_int_t ec;
    nai_int_t n;
    nai_int_t found;
    nai_int_t hsize;
    size_t key;
    size_t size;
    size_t next;
    size_t alloc;
    size_t total = 0;
    uint32_t* hash = 0;
    uint8_t* data;
    nai_hashsnap_elt_t* c;


    if (hsize_max <= 0) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    for (n = 0; n < count; n ++) {
        size = nai_hashsnap_eltsize(v[n].key.len);
        if (size > bucket) {
            nai_errno = EOVERFLOW;
            r = -1;
            goto _end;
        };
        total += size;
    };

    alloc = sizeof(*hash) * hsize_max;
    alloc = nai_align(alloc, sizeof(void*));
    alloc += nai_align(total, sizeof(void*));


    hash = (uint32_t*)(p->pool ? 
        nai_palloc(p->pool, alloc) : nai_malloc(alloc));
    if (hash == 0) {
        r = -1;
        goto _end;
    };

    hsize = (nai_int_t)(count / (bucket / sizeof(*c)));
    hsize = hsize ? hsize : 1;

    /* find a suitable hash size 
     * and compute every bucket size */
    found = 0;
    for ( ; hsize < hsize_max; hsize ++) {
        nai_memset(hash, 0, sizeof(*hash) * hsize);
        for (n = 0; n < count; n ++) {
            size = nai_hashsnap_eltsize(v[n].key.len);
            key = (v[n].key.hash & 0x3fff) % hsize;
            hash[key] += (uint32_t)size;
            if (hash[key] > bucket) {
                break;
            };
        };
        if (n >= count) {
            found = 1;
            break;
        };
    };
    if (!found) {
        nai_errno = EOVERFLOW;
        r = -1;
        goto _end;
    };


    next = nai_align(sizeof(*hash) * hsize, sizeof(void*));
    for (n = 0; n < hsize; n ++) {
        size = hash[n];
        if (size > 0) {
            c = (nai_hashsnap_elt_t*)((uint8_t*)hash + next);
            c->next = 2; /* mark list is empty */
            hash[n] = (uint32_t)next;
            next += size;
        };
    };

    for (n = 0; n < count; n ++) {
        key = (v[n].key.hash & 0x3fff) % hsize;
        assert(hash[key] > 0);

        data = (uint8_t*)hash + hash[key];
        for (;;) {
            c = (nai_hashsnap_elt_t*)data;
            if (c->next == 2) {
                /* empty list, next is offset of first cache */
                break;
            } else if (c->next == 0) {
                /* last one of list 
                 * set c->next with offset of next cache
                 */
                c->next = 1;
                data += nai_hashsnap_eltsize(c->len);
                break;
            } else {
                /* access next one */
                assert(c->next == 1);
                data += nai_hashsnap_eltsize(c->len);
            };
        };

        c = (nai_hashsnap_elt_t*)data;
        c->value = v[n].value;
        c->next = 0;
        c->hash = (uint16_t)v[n].key.hash;
        c->len = v[n].key.len;
        nai_memcpy(c->key, nai_str(&v[n].key), nai_str_len(&v[n].key));
    };


    nai_hashsnap_close(p);
    p->snap = hash;
    p->hashsize = hsize;
    hash = 0;
    r = 0;

_end:
    if (r < 0) {
        if (hash && p->pool == 0) {
            ec = nai_errno;
            nai_free(hash);
            nai_errno = ec;
        };
    };
    return r;
};


void* nai_hashsnap_find(
    nai_hashsnap_t* p, const nai_str_t* str, nai_int_t cases)
{
    void* r;
    nai_int_t hkey;
    nai_int_t hsize;
    size_t next;
    uint8_t* data;
    uint32_t* hash;
    nai_hashsnap_elt_t s;
    nai_hashsnap_elt_t* c;


    hsize = p->hashsize;
    hash = p->snap;

    hkey = str->hash & 0x3fff;
    next = hash[hkey % hsize];
    if (next == 0) {
        r = 0;
        goto _end;
    };

    data = (uint8_t*)hash + next;
    s.len = nai_str_len(str);
    s.hash = hkey;
    s.next = 0;
    if (cases) {
        for (;;) {

            c = (nai_hashsnap_elt_t*)data;
            if (c->hashlen == s.hashlen) {
                if (nai_strncasecmp(c->key, nai_str(str), s.len) == 0) {
                    r = c->value;
                    goto _end;
                };
            };
            if (c->next == 0) {
                break;
            };

            data += nai_hashsnap_eltsize(c->len);
        };
    } else {
        for (;;) {

            c = (nai_hashsnap_elt_t*)data;
            if (c->hashlen == s.hashlen) {
                if (nai_strncmp(c->key, nai_str(str), s.len) == 0) {
                    r = c->value;
                    goto _end;
                };
            };
            if (c->next == 0) {
                break;
            };

            data += nai_hashsnap_eltsize(c->len);
        };
    };

    r = 0;

_end:
    return r;
};


