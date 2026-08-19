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
/// @file       npc_zone.c
/// @brief
/// @details
/// @date       2022-07-09
/// @author     xn
/// @version    1.2.0
///
/// ================================================================

#include "npc/shm/npc_zone.h"
#include "nai/os/nai_file.h"
#include "nai/os/nai_mman.h"
#include "nai/os/nai_thread.h"
#include "nai/runtime/nai_util.h"
#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_rbtree.h"
#include <stdarg.h>


//////////////////////////////////////////////////////////////////////////////
// shared memory zone


typedef struct npc_maddr_s npc_maddr_t;


struct npc_maddr_s {
    uint32_t zone;
    uint32_t zoff;
};


#define npc_maddr_2ref(a, s)                            \
    (((a)->zone << (s)) | (a)->zoff)                    \

#define npc_maddr_set(a, r, s) {                        \
    (a)->zone = (r) >> (s);                             \
    (a)->zoff = (r) & ((1 << (s)) - 1);                 \
}                                                       \



struct npc_msect_s {
    union {
        struct {
            uint16_t next;
            uint16_t prev;
        };
        uint32_t owner;
    };

    struct {
        uint32_t count:31;
        uint32_t alloc:1;
    };
};


struct npc_msegm_s {
    uint16_t tag;
    union {
        struct {
            uint16_t valid:1;
            uint16_t inuse:1;
        };
        uint16_t flags;
    };
    uint32_t sec_start;
    uint32_t sec_end;
};


struct npc_mzone_s {
    uint16_t tag;
    uint16_t serv;
    uint16_t inst;
    uint32_t owner;

    uint32_t seg_size;
    uint32_t seg_shift;
    uint16_t seg_count;
    uint16_t sec_count;

    uint32_t seg_start;
    uint32_t sec_start;
    uint32_t sec_psize;

    npc_mstat_t stat;

    uint16_t log_valid;
    uint16_t log_size;
    uint8_t log_data[256];
};



#define NPC_MZONE_TAG           ('M' + ('Z' << 8))

#define NPC_MZONE_LOG_END       (0)
#define NPC_MZONE_LOG_MSTAT     (1)
#define NPC_MZONE_LOG_MSEGM     (2)
#define NPC_MZONE_LOG_MSECT     (3)


#define NPC_MSEGM_TAG           ('M' + ('S' << 8))
#define NPC_MSEGM_MAX           (256)
#define NPC_MSEGM_MAXSIZE       (512*1024*1024)
#define NPC_MSEGM_MINSIZE       (256*1024)
#define NPC_MSEGM_MINSHIFT      (18)


#define NPC_MSECT_LCOUNT        (16)
#define NPC_MSECT_MAX           (16*1024)
#define NPC_MSECT_SIZE          (256*1024)
#define NPC_MSECT_SHIFT         (18)



static int npc_msect_plist_index(size_t len)
{
    uint32_t index;


#if 1
    /* 
     * entries
     * 0.  1    --  1
     * 1.  4
     * 2.  8
     * 3.  12   --  3
     * 4.  16
     * 5.  32
     * 6.  48   --  3
     * 7.  64
     * 8.  128
     * 9.  192  --  3
     * 10. 256
     * 11. 512
     * 12. 768  --  3
     * 13. 1024
     * 14. 2048 --  2
     * 15. ----
     */

    if (len <= 1) {
        index = 0;
    } else if (len <= ((4 - 1) * 4)) {      //  3 slot,   4 - 12
        index = (uint32_t)((len +  3) >> 2) - 1 + 1;
    } else if (len <= ((4 - 1) * 16)) {     //  3 slot,  16 - 48
        index = (uint32_t)((len + 15) >> 4) - 1 + 4;
    } else if (len <= ((4 - 1) * 64)) {     //  3 slot,  64 - 192
        index = (uint32_t)((len + 63) >> 6) - 1 + 7;
    } else if (len <= ((4 - 1) * 256)) {    //  3 slot, 256 - 768
        index = (uint32_t)((len + 255) >> 8) - 1 + 10;
    } else if (len <= ((3 - 1) * 1024)) {   //  2 slot,  1k - 2k
        index = (uint32_t)((len + 1023) >> 10) - 1 + 13;
    } else {
        index = 15;
    };
#else
    size_t m;

    m = 1;
    for (index = 0; index < 15; index ++) {
        if (len <= m) {
            break;
        };
        m <<= 1;
    };
#endif

    return index;
};


static void npc_msect_list_init(npc_msect_t* s, uint16_t list)
{
    s[list].next = list;
    s[list].prev = list;
};


static void npc_msect_insert_after(
    npc_msect_t* s, uint16_t list, uint16_t ent)
{
    uint16_t next;


    next = s[list].next;
    s[ent].prev = list;
    s[ent].next = next;
    s[list].next = ent;
    s[next].prev = ent;
};


static void npc_msect_insert_before(
    npc_msect_t* s, uint16_t list, uint16_t ent)
{
    uint16_t prev;


    prev = s[list].prev;
    s[ent].next = list;
    s[ent].prev = prev;
    s[list].prev = ent;
    s[prev].next = ent;
};


static void npc_msect_entry_remove(npc_msect_t* s, uint16_t ent)
{
    uint16_t next;
    uint16_t prev;


    next = s[ent].next;
    prev = s[ent].prev;
    s[prev].next = next;
    s[next].prev = prev;
};



static void npc_mzone_log_mstat(npc_mzone_t* z)
{
    uint32_t used;


    used = sizeof(z->stat) + 1;
    assert(z->log_size + used < sizeof(z->log_data));
    z->log_data[z->log_size] = NPC_MZONE_LOG_MSTAT;
    nai_memcpy(z->log_data + z->log_size + 1, &z->stat, sizeof(z->stat));
    z->log_size += used;
};


static void npc_mzone_log_msegm(npc_mzone_t* z, uint16_t ent)
{
    uint32_t used;
    npc_msegm_t* s;


    used = sizeof(*s) + 3;
    assert(z->log_size + used < sizeof(z->log_data));
    z->log_data[z->log_size] = NPC_MZONE_LOG_MSEGM;
    z->log_data[z->log_size + 1] = (uint8_t)(ent & 0xff);
    z->log_data[z->log_size + 2] = (uint8_t)(ent >> 8);
    s = (npc_msegm_t*)((uint8_t*)z + z->seg_start + ent * sizeof(*s));
    nai_memcpy(z->log_data + z->log_size + 3, s, sizeof(*s));
    z->log_size += used;
};


static void npc_mzone_log_msect(npc_mzone_t* z, uint16_t ent)
{
    uint32_t used;
    npc_msect_t* s;


    used = sizeof(*s) + 3;
    assert(z->log_size + used < sizeof(z->log_data));
    z->log_data[z->log_size] = NPC_MZONE_LOG_MSECT;
    z->log_data[z->log_size + 1] = (uint8_t)(ent & 0xff);
    z->log_data[z->log_size + 2] = (uint8_t)(ent >> 8);
    s = (npc_msect_t*)((uint8_t*)z + z->sec_start + ent * sizeof(*s));
    nai_memcpy(z->log_data + z->log_size + 3, s, sizeof(*s));
    z->log_size += used;
};


static void npc_mzone_log_msect_last(npc_mzone_t* z, uint16_t ent)
{
    npc_msect_t* s;


    s = (npc_msect_t*)((uint8_t*)z + z->sec_start + ent * sizeof(*s));
    if (s->count > 1) {
        npc_mzone_log_msect(z, ent + s->count - 1);
    };
};


static void npc_mzone_log_msect_remove(npc_mzone_t* z, uint16_t ent)
{
    uint16_t next;
    uint16_t prev;
    npc_msect_t* s;


    s = (npc_msect_t*)((uint8_t*)z + z->sec_start + ent * sizeof(*s));
    next = s->next;
    prev = s->prev;
    if (next == prev) {
        npc_mzone_log_msect(z, next);
    } else {
        npc_mzone_log_msect(z, next);
        npc_mzone_log_msect(z, prev);
    };
};


static void npc_mzone_log_msect_insert_before(npc_mzone_t* z, uint16_t ent)
{
    uint16_t prev;
    npc_msect_t* s;


    s = (npc_msect_t*)((uint8_t*)z + z->sec_start + ent * sizeof(*s));
    prev = s->prev;
    if (ent == prev) {
        npc_mzone_log_msect(z, ent);
    } else {
        npc_mzone_log_msect(z, ent);
        npc_mzone_log_msect(z, prev);
    };
};


static void npc_mzone_log_submit(npc_mzone_t* z)
{
    assert(z->log_size < sizeof(z->log_data));
    z->log_data[z->log_size] = NPC_MZONE_LOG_END;
    z->log_size ++;
    nai_memory_barrier();
    z->log_valid = 1;
};


static void npc_mzone_log_finish(npc_mzone_t* z)
{
    z->log_valid = 0;
    nai_memory_barrier();
    z->log_size = 0;
};


static int npc_mzone_log_restore(npc_mzone_t* z)
{
    int r;
    uint8_t op;
    uint16_t n;
    uint16_t ent;
    uint32_t used;
    npc_msegm_t* s;
    npc_msect_t* c;


    if (!z->log_valid) {
        z->log_size = 0;
        r = 0;
        goto _end;
    };


    n = 0;
    op = -1;
    for ( ; n < z->log_size; ) {
        op = z->log_data[n];
        switch (op) {
        case NPC_MZONE_LOG_MSTAT:
            used = 1 + sizeof(z->stat);
            if (n + used > z->log_size) {
                nai_errno = EINVAL;
                r = -1;
                goto _end;
            };

            nai_memcpy(&z->stat, z->log_data + n + 1, sizeof(z->stat));
            n += used;
            break;

        case NPC_MZONE_LOG_MSEGM:
            used = 1 + sizeof(*s);
            if (n + used > z->log_size) {
                nai_errno = EINVAL;
                r = -1;
                goto _end;
            };

            ent = z->log_data[n + 1];
            ent |= z->log_data[n + 2] << 8;
            s = (npc_msegm_t*)((uint8_t*)z + z->seg_start + ent * sizeof(*s));
            nai_memcpy(s, z->log_data + n + 3, sizeof(*s));
            n += used;
            break;

        case NPC_MZONE_LOG_MSECT:
            used = 1 + sizeof(*c);
            if (n + used > z->log_size) {
                nai_errno = EINVAL;
                r = -1;
                goto _end;
            };

            ent = z->log_data[n + 1];
            ent |= z->log_data[n + 2] << 8;
            c = (npc_msect_t*)((uint8_t*)z + z->sec_start + ent * sizeof(*c));
            nai_memcpy(c, z->log_data + n + 3, sizeof(*c));
            n += used;
            break;

        case NPC_MZONE_LOG_END:
            n += 1;
            break;

        default:
            nai_errno = EINVAL;
            r = -1;
            goto _end;
        };
    };
    if (op != NPC_MZONE_LOG_END) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    z->log_valid = 0;
    nai_memory_barrier();
    z->log_size = 0;
    r = 0;

_end:
    return r;
};



static int npc_mzone_init(npc_mzone_t* z, const npc_shmz_attr_t* a)
{
    int r;
    int n;
    int index;
    uint32_t used;
    uint32_t align;
    uint32_t shift;
    npc_msegm_t* s;
    npc_msect_t* c;
    npc_msect_t* m;


    align = NPC_MSEGM_MINSIZE;
    shift = NPC_MSEGM_MINSHIFT;
    while (align < a->seg_size) {
        align <<= 1;
        shift ++;
    };

    /* initialize mzone */
    z->tag = 0;
    z->serv = a->serv;
    z->inst = a->inst;
    z->owner = a->self;
    z->seg_size = nai_align(a->seg_size, NPC_MSECT_SIZE);
    z->seg_shift = shift;
    z->seg_count = a->seg_count;
    z->sec_count = z->seg_size / NPC_MSECT_SIZE;
    z->stat.total = z->sec_count;
    z->stat.used = 1;
    z->log_valid = 0;
    z->log_size = 0;
    used = sizeof(*z);

    z->seg_start = used;
    used += z->seg_count * sizeof(*s);

    z->sec_start = used;
    z->sec_psize = sizeof(npc_msect_t) * (
        NPC_MSECT_LCOUNT + z->seg_count * z->sec_count);
    used += z->sec_psize;


    /* initialize segments */
    s = (npc_msegm_t*)((uint8_t*)z + z->seg_start);
    for (n = 0; n < z->seg_count; n ++) {
        s[n].tag = NPC_MSEGM_TAG;
        s[n].flags = 0;
        s[n].sec_start = NPC_MSECT_LCOUNT + (n * z->sec_count);
        s[n].sec_end = s[n].sec_start + z->sec_count;
        if (s[n].sec_end > NPC_MSECT_MAX) {
            s[n].sec_end = NPC_MSECT_MAX;
        };
    };

    /* initialize the list of sections */
    c = (npc_msect_t*)((uint8_t*)z + z->sec_start);
    for (n = 0; n < NPC_MSECT_LCOUNT; n ++) {
        npc_msect_list_init(c, n);
    };

    /* initialize first segment */
    s[0].valid = 1;
    s[0].inuse = 1;

    /* initialzie first section */
    m = c + s[0].sec_start;
    nai_memset(m, 0x0, sizeof(npc_msect_t) * z->sec_count);
    m[0].alloc = 1;
    m[0].count = 1;
    m[0].owner = a->self;
    m[1].alloc = 0;
    m[1].count = z->sec_count - 1;
    m[z->sec_count - 1].prev = s[0].sec_start + 1;

    /* insert into list */
    index = npc_msect_plist_index(m[1].count);
    npc_msect_insert_after(c, index, s->sec_start + 1);

    /* ok */
    nai_memory_barrier();
    z->tag = NPC_MZONE_TAG;
    r = 0;

    return r;
};


static int npc_mzone_verify(npc_mzone_t* z, const npc_shmz_attr_t* a)
{
    int r;
    uint64_t total;


    if (z->tag != NPC_MZONE_TAG) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    if (a != 0) {
        if (z->serv != a->serv) {
            nai_errno = EINVAL;
            r = -1;
            goto _end;
        };
        if (z->inst != a->inst) {
            nai_errno = EINVAL;
            r = -1;
            goto _end;
        };
    };

    if (z->seg_count <= 0) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    if (z->seg_count > NPC_MSEGM_MAX) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    if (z->seg_size < NPC_MSEGM_MINSIZE) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    if (z->seg_size > NPC_MSEGM_MAXSIZE) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    if (z->seg_size & (NPC_MSECT_SIZE - 1)) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    if (z->seg_size > ((uint32_t)1 << z->seg_shift)) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    total = z->seg_count << z->seg_shift;
    if (total > (uint64_t)NPC_MSECT_MAX * NPC_MSECT_SIZE) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };


    r = 0;

_end:
    return r;
};




struct npc_shmz_segm_s {
    nai_rbnode_t node;
    int refs;
    void* start;
};


static nai_rbnode_t** npc_shmz_find(
    npc_shmz_t* z, const void* ptr, nai_rbnode_t** pparent)
{
    nai_rbnode_t** n = &nai_rbtree_root(&z->smap);
    nai_rbnode_t* parent = nai_rbtree_end(&z->smap);
    npc_shmz_segm_t* c;


    while (*n) {
        parent = *n;
        c = nai_containof(parent, npc_shmz_segm_t, node);
        if (c->start > ptr) {
            n = &parent->rb_left;
        } else if (c->start + z->seg_size <= ptr) {
            n = &parent->rb_right;
        } else {
            break;
        };
    };

    if (pparent) {
        pparent[0] = parent;
    };

    return n;
};


static int npc_shmz_insert(npc_shmz_t* p, int index, void* m)
{
    int r;
    nai_rbnode_t** n;
    nai_rbnode_t* parent;
    npc_shmz_segm_t* s;


    s = &p->svec[index];
    s->refs = 0;
    s->start = m;

    n = npc_shmz_find(p, s->start, &parent);
    assert(n[0] == 0);
    nai_rbtree_link(&p->smap, &s->node, parent, n);
    r = 0;

    return r;
};


int npc_shmz_init(npc_shmz_t* p)
{
    int r;


    nai_mmap_init(&p->mmap);
    nai_spin_init(&p->lock);
    nai_rbtree_init(&p->smap);
    p->fd = NAI_FD_INVALID;
    p->mz = 0;
    p->mseg = 0;
    p->msec = 0;
    p->svec = 0;
    p->self = 0;
    p->sec_first = 0;
    p->seg_size = 0;
    p->seg_shift = 0;
    p->seg_count = 0;
    r = 0;

    return r;
};


int npc_shmz_open(npc_shmz_t* p, 
    const char* path, const npc_shmz_attr_t* a, 
    int flags, ...)
{
    intptr_t r;
    int ec;
    int n;
    int mode;
    int open;
    int create;
    int access;
    size_t init;
    size_t size;
    uint32_t shift;
    uint64_t align;
    void* ptr = 0;
    va_list va;
    npc_mzone_t* mz;
    npc_mzone_t mh;
    npc_shmz_attr_t attr;


    if (flags & NAI_O_TRUNC) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };
    if (flags & NAI_O_TMPFILE) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    if (a == 0) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    open = flags & (NAI_O_CREAT | NAI_O_EXCL);
    if (open & NAI_O_CREAT) {
        size = a->seg_size;
        align = NPC_MSEGM_MINSIZE;
        shift = NPC_MSEGM_MINSHIFT;
        while (align < size) {
            align <<= 1;
            shift ++;
        };

        mh.tag = NPC_MZONE_TAG;
        mh.serv = a->serv;
        mh.inst = a->inst;
        mh.seg_size = nai_align(size, NPC_MSECT_SIZE);
        mh.seg_shift = shift;
        mh.seg_count = a->seg_count;

        r = npc_mzone_verify(&mh, a);
        if (r < 0) {
            nai_errno = EINVAL;
            r = -1;
            goto _end;
        };

    } else {
        size = 0;
    };

    create = flags & (NAI_O_CREAT|NAI_O_TMPFILE);
    if (create) {
        va_start(va, flags);
        mode = va_arg(va, int);
        va_end(va);
    };


    switch (flags & (NAI_O_RDONLY|NAI_O_WRONLY|NAI_O_RDWR)) {
    case NAI_O_RDONLY:
        if (open & NAI_O_CREAT) {
            nai_errno = EINVAL;
            r = -1;
            goto _end;
        };
        access = NAI_MPROT_READ;
        break;
    case NAI_O_WRONLY:
        ec = EINVAL;
        r = -1;
        goto _end;
    case NAI_O_RDWR:
        access = NAI_MPROT_READ|NAI_MPROT_WRITE;
        break;
    default:
        ec = EINVAL;
        r = -1;
        goto _end;
    };
    if (flags & NAI_O_EXECUTE) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };


    if (create == 0) {
        p->fd = nai_file_open(path, flags);
    } else {
        p->fd = nai_file_open(path, flags, mode);
    };
    if (p->fd == NAI_FD_INVALID) {
        r = -1;
        goto _end;
    };


    init = -1;
    r = nai_file_lock(p->fd, (access & NAI_MPROT_WRITE) ? 0 : NAI_FLOCK_READ);
    if (r < 0) {
        ec = nai_errno;
        goto _fail;
    };

    init = nai_file_get_size(p->fd);
    if (init == (uint64_t)-1) {
        ec = nai_errno;
        goto _fail;
    };

    if (!(open & NAI_O_CREAT)) {    /* open only */
        if (init == 0) {
            ec = ENOENT;
            goto _fail;
        };

        create = 0;
    } else {        /* open and create */
        if (init > 0 && (open & NAI_O_EXCL)) {
            ec = EEXIST;
            goto _fail;
        };

        if (init == 0) {
            r = nai_file_set_size(p->fd, size);
            if (r < 0) {
                ec = nai_errno;
                goto _fail;
            };

            create = 1;
        } else {
            create = 0;
        };
    };


    if (!create) {
        r = nai_file_pread(p->fd, &mh, sizeof(mh), 0);
        if (r < 0) {
            ec = nai_errno;
            goto _fail;
        };
        if (r != sizeof(mh)) {
            ec = EINVAL;
            goto _fail;
        };

        r = npc_mzone_verify(&mh, a);
        if (r < 0) {
            ec = nai_errno;
            goto _fail;
        };
        if (init % mh.seg_size != 0) {
            ec = EINVAL;
            goto _fail;
        };

        attr.self = a->self;
        attr.serv = mh.serv;
        attr.inst = mh.inst;
        attr.seg_count = mh.seg_count;
        attr.seg_size = mh.seg_size;
        a = &attr;
    };


    p->access = access;
    r = nai_mmap_open(&p->mmap, p->fd);
    if (r < 0) {
        ec = nai_errno;
        goto _fail;
    };


    assert(a != 0);
    ptr = nai_mmap(&p->mmap, 0, a->seg_size, 0, NAI_M_SHARED | access);
    if (ptr == 0) {
        ec = nai_errno;
        goto _fail;
    };

    mz = (npc_mzone_t*)ptr;
    if (create) {
        r = npc_mzone_init(mz, a);
        r |= nai_msync(ptr, sizeof(*mz), NAI_MSYNC_SYNC);
    } else {
        r = npc_mzone_verify(mz, a);
    };
    if (r < 0) {
        ec = nai_errno;
        goto _fail;
    };

    r = nai_file_unlock(p->fd);
    if (r < 0) {
        ec = nai_errno;
        goto _fail;
    };

    r = nai_spin_open(&p->lock, 0);
    if (r < 0) {
        ec = nai_errno;
        goto _fail;
    };

    p->mz = mz;
    p->ms = &mz->stat;
    p->mseg = (npc_msegm_t*)((uint8_t*)mz + mz->seg_start);
    p->msec = (npc_msect_t*)((uint8_t*)mz + mz->sec_start);
    p->self = a->self;
    p->sec_first = 0;
    p->seg_size = mz->seg_size;
    p->seg_shift = mz->seg_shift;
    p->seg_count = mz->seg_count;
    p->svec = (npc_shmz_segm_t*)nai_malloc(sizeof(*p->svec) * p->seg_count);
    if (p->svec == 0) {
        ec = nai_errno;
        goto _fail;
    };

    for (n = 0; n < (int)p->seg_count; n ++) {
        p->svec[n].refs = 0;
        p->svec[n].start = 0;
    };

    npc_shmz_insert(p, 0, ptr);
    r = 0;


_end:
    return (int)r;

_fail:
    if (ptr != 0) {
        nai_munmap(&p->mmap, ptr, a->seg_size);
    };

    npc_shmz_close(p);

    if (open & NAI_O_CREAT) {
        if ((open & NAI_O_EXCL) ||      /* case create new */
            (init == 0)) {              /* case open always */
            nai_file_unlink(path);
        };
    };

    nai_errno = ec;
    r = -1;
    goto _end;
};


int npc_shmz_close(npc_shmz_t* p)
{
    int r;
    int n;


    if (p->svec) {
        for (n = 0; n < (int)p->seg_count; n ++) {
            if (!p->svec[n].start) {
                continue;
            };

            nai_munmap(&p->mmap, p->svec[n].start, p->seg_size);
        };

        nai_rbtree_init(&p->smap);
        nai_free(p->svec);
        p->svec = 0;
        p->mz = 0;
        p->mseg = 0;
        p->msec = 0;
        p->sec_first = 0;
        p->seg_count = 0;
        p->seg_size = 0;
    };

    r = nai_mmap_close(&p->mmap);
    if (r < 0) {
        goto _end;
    };

    if (p->fd != NAI_FD_INVALID) {
        r = nai_file_close(p->fd);
        if (r < 0) {
            goto _end;
        };

        p->fd = NAI_FD_INVALID;
    };

    nai_spin_close(&p->lock);
    r = 0;

_end:
    return r;
};


npc_mref_t npc_shmz_alloc(npc_shmz_t* p, size_t size)
{
    intptr_t r;
    int ec;
    int n, l;
    int found;
    uint16_t ent;
    uint16_t sec;
    uint16_t send;
    uint16_t ret;
    uint16_t list;
    size_t count;
    intptr_t left;
    void* ptr;
    npc_mref_t m = 0;
    npc_maddr_t a;
    npc_mzone_t* z;
    npc_msegm_t* s;
    npc_msect_t* c;
    npc_msect_t* u;


    if (size > p->seg_size) {
        nai_errno = ENOMEM;
        goto _end;
    };
    if (!(p->access & NAI_MPROT_WRITE)) {
        nai_errno = EACCES;
        r = -1;
        goto _end;
    };


    found = 0;
    count = nai_align(size + !size, NPC_MSECT_SIZE);
    count >>= NPC_MSECT_SHIFT;
    z = p->mz;
    s = p->mseg;
    c = p->msec;


    r = nai_file_lock(p->fd, 0);
    if (r < 0) {
        goto _end;
    };


    while (1) {
        r = npc_mzone_log_restore(z);
        if (r < 0) {
            ec = nai_errno;
            break;
        };

        n = npc_msect_plist_index(count);
        for ( ; n < NPC_MSECT_LCOUNT; n ++) {

            list = n;
            ent = c[list].next;
            for ( ; ent != list; ) {
                sec = ent;
                ent = c[ent].next;

                /* test the size of unused sections */
                u = c + sec;
                left = u->count - count;
                if (left < 0) {
                    continue;
                };


                /* do log before modify */
                npc_mzone_log_mstat(z);
                npc_mzone_log_msect(z, sec);
                npc_mzone_log_msect(z, sec + u->count - 1);
                if (left <= 0) {
                    npc_mzone_log_msect_remove(z, sec);
                } else {
                    npc_mzone_log_msect(z, sec + left);
                    if (left > 1) {
                        npc_mzone_log_msect(z, sec + left - 1);
                    };

                    l = npc_msect_plist_index(left);
                    if (l != n) {
                        npc_mzone_log_msect_remove(z, sec);
                        npc_mzone_log_msect_insert_before(z, l);
                    };
                };

                /* submit log */
                npc_mzone_log_submit(z);


                /* remove allocated sections */
                if (left <= 0) {
                    npc_msect_entry_remove(c, sec);
                } else {
                    if (left > 1) {
                        u[left-1].prev = sec;
                    };
                    u->count = left;

                    /* adjust the position of the remaining sections */
                    l = npc_msect_plist_index(left);
                    if (l != n) {
                        npc_msect_entry_remove(c, sec);
                        npc_msect_insert_before(c, l, sec);
                    };
                };

                /* initial allocated sections */
                ret = sec + left;
                c[ret].alloc = 1;
                c[ret].count = count;
                c[ret].owner = p->self;
                if (count > 1) {
                    c[ret + count - 1].alloc = 1;
                    c[ret + count - 1].count = -1;
                };

                /* update stat */
                z->stat.used += count;

                /* finish log */
                npc_mzone_log_finish(z);


                /* get address */
                a.zone = (ret - NPC_MSECT_LCOUNT) / z->sec_count;
                a.zoff = (ret - NPC_MSECT_LCOUNT) % z->sec_count;
                a.zoff <<= NPC_MSECT_SHIFT;

                found = 1;
                break;
            };

            if (found) {
                break;
            };
        };

        if (found) {
            break;
        };


        /* find an unused segment */
        for (n = 0; n < (int)z->seg_count; n ++) {
            if (!s[n].valid || !s[n].inuse) {
                break;
            };
        };
        if (n >= (int)z->seg_count) {
            ec = ENOMEM;
            break;
        };

        /* grow up file */
        sec = s[n].sec_start;
        send = s[n].sec_end;
        if (!s[n].valid) {
            r = nai_file_set_size(p->fd, (n + 1) * p->seg_size);
            if (r < 0) {
                ec = nai_errno;
                break;
            };

            /* initialize sections */
            nai_memset(c + sec, 0, (send - sec) * sizeof(*c));
        };


        /* get list index */
        l = npc_msect_plist_index(send - sec);


        /* do log before modify */
        npc_mzone_log_mstat(z);
        npc_mzone_log_msegm(z, n);
        npc_mzone_log_msect_insert_before(z, l);
        npc_mzone_log_submit(z);


        /* enalbe segment and insert sections */
        c[sec].count = send - sec;
        c[send].prev = sec;
        s[n].valid = 1;
        s[n].inuse = 1;
        npc_msect_insert_before(c, l, sec);

        /* update stat */
        z->stat.total += send - sec;

        /* finish log */
        npc_mzone_log_finish(z);
    };


    r = nai_file_unlock(p->fd);
    if (r < 0) {
        /* fixme: unhandled error */
        goto _end;
    };

    if (!found) {
        nai_errno = ec;
        goto _end;
    };


    /* to mref */
    m = npc_maddr_2ref(&a, p->seg_shift);

    /* map segment to memory */
    ptr = npc_shmz_hold(p, m);
    if (ptr == 0) {
        (void)npc_shmz_free(p, m);
        nai_errno = ENOMEM;
        m = 0;
        goto _end;
    };

_end:
    return m;
};


static int npc_shmz_free_sects(npc_shmz_t* p, uint32_t zone, uint32_t sec)
{
    int r;
    int n, l;
    int ncon;
    int pcon;
    uint16_t new;
    uint32_t next;
    uint32_t prev;
    uint32_t count;
    uint32_t concat;
    npc_mzone_t* z;
    npc_msegm_t* s;
    npc_msect_t* c;


    z = p->mz;
    s = p->mseg + zone;
    c = p->msec;

    r = npc_mzone_log_restore(z);
    if (r < 0) {
        goto _end;
    };


    count = c[sec].count;
    next = sec + count;
    prev = sec - 1;
    ncon = (next < s->sec_end && !c[next].alloc);
    pcon = (prev >= s->sec_start && !c[prev].alloc);


    /* do log before modify */
    npc_mzone_log_mstat(z);
    npc_mzone_log_msect(z, sec);
    npc_mzone_log_msect_last(z, sec);

    /* do log concat */
    concat = count;
    if (pcon) {

        /* concat prev and next */
        if (ncon) {
            concat += c[next].count;
            npc_mzone_log_msect(z, next);
            npc_mzone_log_msect_last(z, next);
            npc_mzone_log_msect_remove(z, next);
        };

        if (c[prev].count == 0) {
            prev = c[prev].prev;
        };
        npc_mzone_log_msect(z, prev);
        npc_mzone_log_msect_last(z, prev);
        concat += c[prev].count;
        n = npc_msect_plist_index(c[prev].count);
        new = prev;

    } else if (ncon) {

        /* concat next */
        npc_mzone_log_msect(z, next);
        npc_mzone_log_msect_last(z, next);
        npc_mzone_log_msect_insert_before(z, next);
        npc_mzone_log_msect_remove(z, next);
        concat += c[next].count;
        n = npc_msect_plist_index(c[next].count);
        new = sec;

    } else {
        n = -1;
        new = sec;
    };

    /* get list index */
    l = npc_msect_plist_index(concat);

    /* do log move list */
    if (new == s->sec_start && 
        concat == s->sec_end - s->sec_start) {

        /* segment is freed */
        npc_mzone_log_msegm(z, zone);
        npc_mzone_log_msect_remove(z, new);

    } else if (l != n) {
        if (n >= 0) {
            npc_mzone_log_msect_remove(z, new);
        };

        /* bigger after merging, move to tail */
        npc_mzone_log_msect_insert_before(z, l);
    };


    /* submit log */
    npc_mzone_log_submit(z);


    /* clear allocated mark */
    c[sec].owner = 0;
    c[sec].alloc = 0;
    if (count > 1) {
        c[sec + count - 1].alloc = 0;
        c[sec + count - 1].count = 0;
    };

    /* concat */
    if (pcon) {

        /* concat prev and next */
        if (ncon) {
            npc_msect_entry_remove(c, next);
            c[sec].count += c[next].count;
            c[next].count = 0;
        };

        next = sec;
        sec = prev;

        c[sec].count += c[next].count;
        c[sec + concat - 1].prev = sec;
        c[next].count = 0;

    } else if (ncon) {

        /* concat next */
        npc_msect_insert_before(c, next, sec);
        npc_msect_entry_remove(c, next);

        c[sec].count += c[next].count;
        c[sec + concat - 1].prev = sec;
        c[next].count = 0;

    } else {

        if (concat > 1) {
            c[sec + concat - 1].prev = sec;
        };
    };

    /* move list */
    if (new == s->sec_start && 
        concat == s->sec_end - s->sec_start) {

        /* segment is freed */
        npc_msect_entry_remove(c, new);
        s->inuse = 0;

    } else if (l != n) {
        if (n >= 0) {
            npc_msect_entry_remove(c, new);
        };

        /* bigger after merging, move to tail */
        npc_msect_insert_before(c, l, new);
    };

    /* update stat */
    z->stat.used -= count;

    /* finish log */
    npc_mzone_log_finish(z);

    r = 0;

_end:
    return r;
};


int npc_shmz_free(npc_shmz_t* p, npc_mref_t m)
{
    int r;
    int ec;
    uint16_t sec;
    npc_maddr_t a;
    npc_msegm_t* s;
    npc_msect_t* c;


    npc_maddr_set(&a, m, p->seg_shift);

    if (a.zone >= p->seg_count) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };
    if (a.zoff >= p->seg_size) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };
    if (a.zoff & (NPC_MSECT_SIZE-1)) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };
    if (!(p->access & NAI_MPROT_WRITE)) {
        nai_errno = EACCES;
        r = -1;
        goto _end;
    };

    s = p->mseg + a.zone;
    c = p->msec;

    r = nai_file_lock(p->fd, 0);
    if (r < 0) {
        goto _end;
    };

    if (!s->valid || !s->inuse) {
        ec = EINVAL;
        r = -1;
        goto _fail;
    };

    sec = s->sec_start + (a.zoff >> NPC_MSECT_SHIFT);
    if (!c[sec].alloc) {
        ec = EINVAL;
        r = -1;
        goto _fail;
    };
    if (c[sec].owner != p->self) {
        ec = EINVAL;
        r = -1;
        goto _fail;
    };

    r = npc_shmz_free_sects(p, a.zone, sec);
    if (r < 0) {
        ec = nai_errno;
        goto _fail;
    };

    ec = 0;

_fail:
    /* unlock */
    r = nai_file_unlock(p->fd);
    if (r < 0) {
        /* fixme: unhandled error */
        goto _end;
    };

    if (ec != 0) {
        nai_errno = ec;
        r = -1;
        goto _end;
    };

    /* unhold memory */
    r = npc_shmz_unhold(p, m);
    assert(r >= 0);

_end:
    return r;
};


npc_mref_t npc_shmz_2ref(npc_shmz_t* p, const void* ptr)
{
    int r;
    int ec;
    npc_mref_t m;
    npc_maddr_t a;
    npc_shmz_segm_t* s;
    nai_rbnode_t** n;


    if (p->seg_count > 1) {
        r = nai_spin_lock(&p->lock);
        if (r < 0) {
            m = 0;
            goto _end;
        };
    };

    n = npc_shmz_find(p, ptr, 0);
    if (n[0] == 0) {
        ec = EINVAL;
        m = 0;
        goto _fail;
    };

    s = nai_containof(n[0], npc_shmz_segm_t, node);
    if (s->start == 0) {
        ec = ENOMEM;
        m = 0;
        goto _fail;
    };

    a.zone = s - p->svec;
    a.zoff = (uint8_t*)ptr - (uint8_t*)s->start;
    m = npc_maddr_2ref(&a, p->seg_shift);
    ec = 0;

_fail:
    if (p->seg_count > 1) {
        nai_spin_unlock(&p->lock);
    };

    if (m == 0) {
        nai_errno = ec;
    };

_end:
    return m;
};


void* npc_shmz_2ptr(npc_shmz_t* p, npc_mref_t m)
{
    void* r;
    void* start;
    npc_maddr_t a;
    npc_shmz_segm_t* s;


    npc_maddr_set(&a, m, p->seg_shift);

    if (a.zone >= p->seg_count) {
        nai_errno = EINVAL;
        r = 0;
        goto _end;
    };
    if (a.zoff >= p->seg_size) {
        nai_errno = EINVAL;
        r = 0;
        goto _end;
    };

    /* unmap memory */
    s = &p->svec[a.zone];
    start = s->start;
    if (start == 0) {
        nai_errno = ENOMEM;
        r = 0;
        goto _end;
    };

    r = (uint8_t*)start + a.zoff;

_end:
    return r;
};


void* npc_shmz_hold(npc_shmz_t* p, npc_mref_t m)
{
    int n;
    int ec;
    void* r;
    void* ptr;
    npc_maddr_t a;
    npc_shmz_segm_t* s;


    npc_maddr_set(&a, m, p->seg_shift);

    if (a.zone >= p->seg_count) {
        nai_errno = EINVAL;
        r = 0;
        goto _end;
    };
    if (a.zoff >= p->seg_size) {
        nai_errno = EINVAL;
        r = 0;
        goto _end;
    };


    n = nai_spin_lock(&p->lock);
    if (n < 0) {
        r = 0;
        goto _end;
    };


    /* map segment to memory */
    s = &p->svec[a.zone];
    if (s->start == 0) {
        ptr = nai_mmap(&p->mmap, 0, 
            p->seg_size, a.zone * p->seg_size, NAI_M_SHARED | p->access);
        if (ptr == 0) {
            ec = nai_errno;
            r = 0;
            goto _fail;
        };

        npc_shmz_insert(p, a.zone, ptr);
    };

    s->refs ++;
    r = (uint8_t*)s->start + a.zoff;
    ec = 0;

_fail:
    nai_spin_unlock(&p->lock);

    if (r == 0) {
        nai_errno = ec;
    };

_end:
    return r;
};


int npc_shmz_unhold(npc_shmz_t* p, npc_mref_t m)
{
    int r;
    int ec;
    npc_maddr_t a;
    npc_shmz_segm_t* s;


    npc_maddr_set(&a, m, p->seg_shift);

    if (a.zone >= p->seg_count) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };
    if (a.zoff >= p->seg_size) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };


    r = nai_spin_lock(&p->lock);
    if (r < 0) {
        goto _end;
    };


    s = &p->svec[a.zone];
    if (s->start == 0) {
        ec = EINVAL;
        r = -1;
        goto _fail;
    };

    /* unmap memory */
    s->refs --;
    if (s->refs <= 0) {
        if (!p->mseg[a.zone].valid || !p->mseg[a.zone].inuse) {
            r = nai_munmap(&p->mmap, s->start, p->seg_size);
            if (r < 0) {
                ec = nai_errno;
                goto _fail;
            };

            s->start = 0;
            nai_rbtree_erase(&p->smap, &s->node);
        };
    };

    r = 0;

_fail:
    nai_spin_unlock(&p->lock);

    if (r < 0) {
        nai_errno = ec;
    };

_end:
    return r;
};


int npc_shmz_2ref_array(npc_shmz_t* p, npc_mref_t* m, void** a, int count)
{
    int r;
    int ec = 0;
    int i;
    npc_maddr_t t;
    npc_shmz_segm_t* s;
    nai_rbnode_t** n;


    if (p->seg_count > 1) {
        r = nai_spin_lock(&p->lock);
        if (r < 0) {
            goto _end;
        };
    };

    for (i = 0; i < count; i ++) {
        n = npc_shmz_find(p, a[i], 0);
        if (n[0] == 0) {
            ec = EINVAL;
            m[i] = 0;
            continue;
        };

        s = nai_containof(n[0], npc_shmz_segm_t, node);
        if (s->start == 0) {
            ec = ENOMEM;
            m[i] = 0;
            continue;
        };

        t.zone = s - p->svec;
        t.zoff = (uint8_t*)a[i] - (uint8_t*)s->start;
        m[i] = npc_maddr_2ref(&t, p->seg_shift);
    };

    if (p->seg_count > 1) {
        nai_spin_unlock(&p->lock);
    };

    if (ec) {
        nai_errno = ec;
    };
    r = 0;

_end:
    return r;
};


int npc_shmz_2ptr_array(npc_shmz_t* p, void** a, npc_mref_t* m, int count)
{
    int r;
    int ec = 0;
    int i;
    void* start;
    npc_maddr_t t;
    npc_shmz_segm_t* s;


    for (i = 0; i < count; i ++) {
        npc_maddr_set(&t, m[i], p->seg_shift);

        if (t.zone >= p->seg_count) {
            ec = EINVAL;
            a[i] = 0;
            continue;
        };
        if (t.zoff >= p->seg_size) {
            ec = EINVAL;
            a[i] = 0;
            continue;
        };

        /* unmap memory */
        s = &p->svec[t.zone];
        start = s->start;
        if (start == 0) {
            ec = ENOMEM;
            a[i] = 0;
            continue;
        };

        a[i] = (uint8_t*)start + t.zoff;
    };

    if (ec) {
        nai_errno = ec;
    };

    r = 0;

    return r;
};


int npc_shmz_hold_array(npc_shmz_t* p, void** a, npc_mref_t* m, int count)
{
    int r;
    int i;
    int n;
    int ec;
    void* ptr;
    npc_maddr_t t;
    npc_shmz_segm_t* s;


    n = nai_spin_lock(&p->lock);
    if (n < 0) {
        r = -1;
        goto _end;
    };


    for (i = 0; i < count; i ++) {
        npc_maddr_set(&t, m[i], p->seg_shift);

        if (t.zone >= p->seg_count) {
            ec = EINVAL;
            r = -1;
            goto _fail;
        };
        if (t.zoff >= p->seg_size) {
            ec = EINVAL;
            r = -1;
            goto _fail;
        };


        /* map segment to memory */
        s = &p->svec[t.zone];
        if (s->start == 0) {
            ptr = nai_mmap(&p->mmap, 0, 
                p->seg_size, t.zone * p->seg_size, NAI_M_SHARED | p->access);
            if (ptr == 0) {
                ec = nai_errno;
                r = 0;
                goto _fail;
            };

            npc_shmz_insert(p, t.zone, ptr);
        };

        s->refs ++;
        a[i] = (uint8_t*)s->start + t.zoff;
    };

    r = 0;

_fail:
    nai_spin_unlock(&p->lock);

    if (r < 0) {
        if (i > 0) {
            npc_shmz_unhold_array(p, m, i);
        };
        nai_errno = ec;
    };

_end:
    return r;
};


int npc_shmz_unhold_array(npc_shmz_t* p, npc_mref_t* m, int count)
{
    int r;
    int ec;
    int i;
    npc_maddr_t t;
    npc_shmz_segm_t* s;


    r = nai_spin_lock(&p->lock);
    if (r < 0) {
        goto _end;
    };


    for (i = 0; i < count; i ++) {
        npc_maddr_set(&t, m[i], p->seg_shift);

        if (t.zone >= p->seg_count) {
            continue;
        };
        if (t.zoff >= p->seg_size) {
            continue;
        };

        s = &p->svec[t.zone];
        if (s->start == 0) {
            continue;
        };

        /* unmap memory */
        s->refs --;
        if (s->refs <= 0) {
            if (!p->mseg[t.zone].valid || !p->mseg[t.zone].inuse) {
                r = nai_munmap(&p->mmap, s->start, p->seg_size);
                if (r < 0) {
                    ec = nai_errno;
                    goto _fail;
                };

                s->start = 0;
                nai_rbtree_erase(&p->smap, &s->node);
            };
        };
    };

    r = 0;

_fail:
    nai_spin_unlock(&p->lock);

    if (r < 0) {
        nai_errno = ec;
    };

_end:
    return r;
};


int npc_shmz_is_owner(npc_shmz_t* p)
{
    int r;


    if (p->mz && p->self == p->mz->owner) {
        r = 1;
    } else {
        r = 0;
    };

    return r;
};


int npc_shmz_clean(npc_shmz_t* p, uint32_t uid)
{
    int r;
    int ec;
    uint32_t n;
    uint32_t sec, send;
    uint32_t pre;
    npc_msegm_t* s;
    npc_msect_t* c;


    s = p->mseg;
    c = p->msec;


    /* lock */
    r = nai_file_lock(p->fd, 0);
    if (r < 0) {
        goto _end;
    };


    /* scan all segments */
    for (n = 0; n < p->seg_count; n ++) {
        if (!s[n].inuse) {
            continue;
        };

        /* scan sections in segment */
        pre = s[n].sec_start;
        sec = s[n].sec_start;
        send = s[n].sec_end;
        for ( ; sec < send; ) {
            if (c[sec].alloc && c[sec].owner == uid) {
                r = npc_shmz_free_sects(p, n, sec);
                if (r < 0) {
                    ec = nai_errno;
                    goto _fail;
                };
                if (c[sec].count == 0) {
                    sec = pre;
                };
            };

            assert(c[sec].count > 0);
            pre = sec;
            sec += c[sec].count;
        };
    };

    ec = 0;

_fail:
    /* unlock */
    r = nai_file_unlock(p->fd);
    if (r < 0) {
        /* fixme: ignore error */
        goto _end;
    };

    if (ec != 0) {
        nai_errno = ec;
        r = -1;
        goto _end;
    };

    r = 0;

_end:
    return r;
};


int npc_shmz_chown(npc_shmz_t* p, uint32_t old, uint32_t uid)
{
    int r;
    int n;
    int sec, send;
    npc_msegm_t* s;
    npc_msect_t* c;


    s = p->mseg;
    c = p->msec;


    /* lock */
    r = nai_file_lock(p->fd, 0);
    if (r < 0) {
        goto _end;
    };


    /* scan all segments */
    for (n = 0; n < (int)p->seg_count; n ++) {
        if (!s[n].inuse) {
            continue;
        };

        /* scan sections in segment */
        sec = s[n].sec_start;
        send = s[n].sec_end;
        for ( ; sec < send; ) {
            if (c[sec].alloc && c[sec].owner == old) {
                c[sec].owner = uid;
            };

            assert(c[sec].count > 0);
            sec += c[sec].count;
        };
    };

    /* unlock */
    r = nai_file_unlock(p->fd);
    if (r < 0) {
        /* fixme: ignore error */
        goto _end;
    };

    r = 0;

_end:
    return r;
};


//////////////////////////////////////////////////////////////////////////////
// memory zone



int npc_zone_init(npc_zone_t* p, size_t fsize)
{
    int r;


    nai_spin_init(&p->lock);
    nai_zone_init(&p->zone);
    nai_zone_open(&p->zone, NAI_ZONE_DEVICE, fsize);
    p->shm = 0;
    p->first = 0;
    p->grow_size = 0;
    r = 0;

    return r;
};



int npc_zone_open(npc_zone_t* p, npc_shmz_t* z, int first, size_t grow_size)
{
    int r;
    int ec;
    void* ptr;
    size_t used;
    size_t size;
    npc_mref_t m;


    if (grow_size <= 0) {
        grow_size = NPC_MSECT_SIZE * 16;
    };
    if (grow_size < 4096) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };
    if (grow_size > z->seg_size) {
        grow_size = nai_aligndown(z->seg_size, NPC_MSECT_SIZE);
    } else {
        grow_size = nai_align(grow_size, NPC_MSECT_SIZE);
    };

    if (first) {
        if (z->sec_first) {
            nai_errno = EINVAL;
            r = -1;
            goto _end;
        };
        if (z->self != z->mz->owner) {
            nai_errno = EINVAL;
            r = -1;
            goto _end;
        };

        z->sec_first = 1;
        used = sizeof(*z->mz);
        used += z->mz->seg_count * sizeof(*z->mseg);
        used += z->mz->sec_psize;
        size = NPC_MSECT_SIZE - used;
        ptr = (uint8_t*)z->mz + used;
    } else {
        size = NPC_MSECT_SIZE;
        m = npc_shmz_alloc(z, size);
        if (m == 0) {
            r = -1;
            goto _end;
        };

        ptr = npc_shmz_2ptr(z, m);
    };

    p->shm = z;
    p->first = first;
    p->grow_size = grow_size;
    r = nai_zone_offer(&p->zone, ptr, size);
    if (r < 0) {
        goto _fail;
    };

    r = nai_spin_open(&p->lock, 0);
    if (r < 0) {
        goto _fail;
    };

_end:
    return r;

_fail:
    ec = nai_errno;
    if (first) {
        z->sec_first = 0;
    } else {
        npc_shmz_free(z, m);
    };
    nai_errno = ec;
    p->shm = 0;
    p->first = 0;
    p->grow_size = 0;
    r = -1;
    goto _end;
};


int npc_zone_close(npc_zone_t* p)
{
    int r;
    int ec;
    size_t used;
    void* ptr;
    void* first;
    npc_mref_t m;
    npc_shmz_t* z;


    r = nai_zone_reset(&p->zone);
    if (r < 0) {
        goto _end;
    };

    if (p->first) {
        z = p->shm;
        used = sizeof(*z->mz);
        used += z->mz->seg_count * sizeof(*z->mseg);
        used += z->mz->sec_psize;
        first = (uint8_t*)z->mz + used;
    } else {
        first = 0;
    };

    while (1) {
        ptr = nai_zone_eject(&p->zone, 0);
        if (ptr == 0) {
            ec = nai_errno;
            if (ec != ENOENT) {
                assert(0);
                r = -1;
                goto _end;
            };

            break;
        };

        if (ptr == first) {
            p->first = 0;
            p->shm->sec_first = 0;
        } else {
            m = npc_shmz_2ref(p->shm, ptr);
            if (m == 0) {
                assert(0);
                r = -1;
                goto _end;
            };

            r = npc_shmz_free(p->shm, m);
            if (r < 0) {
                assert(0);
                goto _end;
            };
        };
    };

    nai_spin_close(&p->lock);
    p->shm = 0;
    p->grow_size = 0;
    r = 0;

_end:
    return r;
};


void* npc_zalloc(npc_zone_t* p, size_t size)
{
    void* r;
    int n;
    int ec;
    size_t alloc;
    size_t align;
    void* ptr;
    npc_mref_t m;


    if (p->shm == 0) {
        nai_errno = EINVAL;
        r = 0;
        goto _end;
    };

    /* lock */
    nai_spin_lock(&p->lock);

    /* do alloc */
    r = nai_zalloc(&p->zone, size);
    while (r == 0) {
        ec = nai_errno;
        if (ec != ENOMEM) {
            break;
        };

        align = p->grow_size;
        alloc = size;
        for ( ; align < alloc; align <<= 1) {
            /* nothing */
            ;
        };

        m = npc_shmz_alloc(p->shm, align);
        if (m == 0) {
            ec = nai_errno;
            break;
        };

        ptr = npc_shmz_2ptr(p->shm, m);
        assert(ptr != 0);
        n = nai_zone_offer(&p->zone, ptr, align);
        if (n < 0) {
            ec = nai_errno;
            npc_shmz_free(p->shm, m);
            break;
        };

        r = nai_zalloc(&p->zone, size);
        assert(r != 0);
        if (r == 0) {
            ec = nai_errno;
        };

        break;
    };

    /* unlock */
    nai_spin_unlock(&p->lock);

    if (r == 0) {
        nai_errno = ec;
    };

_end:
    return r;
};


int npc_zfree(npc_zone_t* p, void* ptr)
{
    int r;
    int ec;


    if (p->shm == 0) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    /* lock */
    nai_spin_lock(&p->lock);

    /* do free */
    r = nai_zfree(&p->zone, ptr);
    if (r < 0) {
        ec = nai_errno;
    };

    /* unlock */
    nai_spin_unlock(&p->lock);

    if (r < 0) {
        nai_errno = ec;
    };

_end:
    return r;
};


