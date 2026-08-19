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
/// @file       nai_open_cache.c
/// @brief      
/// @details
/// @date       2022-04-12
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai/service/nai_open_cache.h"
#include "nai/os/nai_system.h"
#include "nai/runtime/nai_list.h"
#include "nai/runtime/nai_pool.h"
#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_util.h"



struct nai_open_cache_file_s {
    nai_rbnode_t node;
    nai_list_entry_t ent;

    nai_open_cache_t* cache;
    nai_fd_t fd;
    nai_str_t name;
    nai_stat_t st;

    uint64_t access;

    nai_int_t use;
    nai_int_t refs;
    nai_int_t closed;
    nai_int_t oflags;

    struct {
        nai_int_t valid;
        nai_int_t error;
        size_t root_length;
        uint64_t update;
    } sult[3];
};


#if (NAI_HAVE_OPENAT)
#define NAI_USE_OPENAT          1
#endif



#if (NAI_USE_OPENAT) && defined(O_PATH)


static nai_int_t nai_open_file_stat(
    nai_fd_t fd, nai_stat_t* st, nai_int_t flags)
{
    static nai_int_t use_fstat = 1;

    nai_int_t r;
    nai_int_t ec;


    if (use_fstat) {
        r = nai_file_stat(fd, st, flags);
        if (r >= 0) {
            goto _end;
        };

        ec = nai_errno;
        if (ec != EBADF) {
            goto _end;
        };

        use_fstat = 0;
    };

    r = nai_stat_at(fd, "", st, flags);

_end:
    return r;
};

#else

#define nai_open_file_stat nai_file_stat

#endif


static nai_int_t nai_open_file(
    const nai_str_t* name, const nai_open_attr_t* attr, 
    nai_int_t oflags, nai_open_file_t* of, nai_stat_t* st)
{
    nai_int_t r;
    nai_int_t flags;
    nai_int_t verify;
    nai_int_t stated = 0;
    intptr_t len;
    nai_fd_t fd = NAI_FD_INVALID;
    nai_stat_t st_fd;
    nai_stat_t st_cur;
    nai_stat_t st_root;
    char* p;
    char* pend;
    char* slash;
    char path[NAI_PATH_MAX];

#if (NAI_USE_OPENAT)
    nai_int_t ec;
    nai_fd_t at;
#else
    nai_int_t stat;
    intptr_t copy;
    char* last;
#endif


    /* fix open flags */
    flags = oflags;
    verify = attr->verify;
    if (verify == NAI_OPEN_VERIFY_LINK) {
        flags |= NAI_O_NOFOLLOW;
    };

    /* get file name */
    p = nai_str(name);
    pend = p + nai_str_len(name);

    /* when enabling directio, 
     * need to query the file length before opening the file
     */
    if (attr->directio != (uint64_t)-1) {
        if (st == 0) {
            st = &st_fd;
        };

        if (verify != NAI_OPEN_VERIFY_OWNER) {
            r = nai_stat(p, st, NAI_STAT_BASIC);
        } else {
            r = nai_stat(p, st, NAI_STAT_PERM);
        };
        if (r < 0) {
            goto _end;
        };

        stated = 1;
        if (!(st->st_mode & NAI_S_IFDIR)) {
            if (st->st_size >= attr->directio) {
                flags |= NAI_O_DIRECT;
            };
        };
    };

    if (verify == NAI_OPEN_VERIFY_OFF) {
        if (of != 0) {
            fd = nai_file_open(p, flags, 0644);
            if (fd == NAI_FD_INVALID) {
                r = -1;
                goto _end;
            };
        };

#if (NAI_USE_OPENAT)
        at = NAI_FD_INVALID;
#endif
        goto _done;
    };

#if (NAI_USE_OPENAT)

    /* open root directory */
    if (attr->root_length) {
        len = attr->root_length;
        nai_memcpy(path, p, len);
        p += len;
        path[len] = 0;

        at = nai_file_open(path, NAI_O_SEARCH, 0);

    } else if (nai_path_is_absolute(p, pend - p)) {
        slash = nai_strnchr(p, pend - p, nai_path_sep);
        if (slash == 0) {
            slash = pend;
        } else {
            slash ++;
        };

        len = slash - p;
        nai_memcpy(path, p, len);
        p += len;
        path[len] = 0;

        at = nai_file_open(path, NAI_O_SEARCH, 0);

    } else {
        at = NAI_FD_CWD;
    };

    /* query the status of the root directory to verify the owner */
    if (verify == NAI_OPEN_VERIFY_OWNER) {
        r = nai_open_file_stat(at, &st_root, NAI_STAT_PERM);
        if (r < 0) {
            goto _end;
        };
    };

    /* work each compement to lookup symlink */
    for (;;) {
        for ( ; p < pend; p ++) {
            if (!nai_path_is_sep(*p)) {
                break;
            };
        };

        slash = nai_strnchr(p, pend - p, nai_path_sep);
        if (slash == 0) {
            break;
        } else {
            slash ++;
        };

        len = slash - p;
        nai_memcpy(path, p, len);
        p += len;
        path[len] = 0;

        if (verify == NAI_OPEN_VERIFY_LINK) {
            fd = nai_file_openat(at, path, NAI_O_SEARCH|NAI_O_NOFOLLOW, 0);
        } else {
            fd = nai_file_openat(at, path, NAI_O_SEARCH, 0);
        };
        if (fd == NAI_FD_INVALID) {
            goto _fail;
        };

        if (verify == NAI_OPEN_VERIFY_OWNER) {
            r = nai_open_file_stat(fd, &st_cur, NAI_STAT_PERM);
            if (r < 0) {
                goto _fail;
            };

            if (st_cur.st_uid != st_root.st_uid) {
                nai_errno = ELOOP;
                goto _fail;
            };
        };

        if (at != NAI_FD_CWD) {
            r = nai_file_close(at);
            if (r < 0) {
                goto _fail;
            };
        };

        at = fd;
        fd = NAI_FD_INVALID;
    };

    if (p == pend) {
        p = ".";
    };

    if (of != 0) {
        /* open file */
        fd = nai_file_openat(at, p, flags, 0);
        if (fd == NAI_FD_INVALID) {
            goto _fail;
        };
    } else {
        /* if need to verify the owner or get the status of a file, 
         * using the parent directory will be faster.
         */
        if (!stated && 
            (verify == NAI_OPEN_VERIFY_OWNER || st)) {

            if (st == 0) {
                st = &st_fd;
            };

            if (verify == NAI_OPEN_VERIFY_OWNER) {
                r = nai_stat_at(at, p, st, NAI_STAT_PERM);
            } else {
                r = nai_stat_at(at, p, st, NAI_STAT_BASIC);
            };
            if (r < 0) {
                goto _fail;
            };

            stated = 1;
        };
    };

    if (at != NAI_FD_INVALID && at != NAI_FD_CWD) {
        nai_file_close(at);
        at = NAI_FD_INVALID;
    };

#else

    copy = attr->root_length;
    nai_memcpy(path, p, copy);
    p += copy;
    path[copy] = 0;


    /* query the status of the root directory to verify the owner */
    if (verify == NAI_OPEN_VERIFY_OWNER) {
        r = nai_stat(path, &st_root, NAI_STAT_PERM);
        if (r < 0) {
            goto _end;
        };

        stat = NAI_STAT_PERM;
    } else {
        stat = NAI_STAT_SYMLINK;
    };

    /* work each compement to lookup symlink */
    for (;;) {
        last = p;
        for ( ; p < pend; p ++) {
            if (!nai_path_is_sep(*p)) {
                break;
            };
        };

        slash = nai_strnchr(p, pend - p, nai_path_sep);
        if (slash == 0) {
            break;
        } else {
            slash ++;
        };

        p = slash;
        len = slash - last;
        nai_memcpy(path + copy, last, len);
        copy += len;
        path[copy] = 0;

        r = nai_stat(path, &st_cur, stat);
        if (r < 0) {
            goto _fail;
        };

        if (verify == NAI_OPEN_VERIFY_OWNER) {
            if (st_cur.st_uid != st_root.st_uid) {
                goto _fail;
            };
        };

        if (st_cur.st_mode & NAI_S_IFLNK) {
            nai_errno = ELOOP;
            goto _fail;
        };
    };

    /* reset pointer 'p' */
    p = nai_str(name);

    /* open file */
    if (of != 0) {
        fd = nai_file_open(p, flags, 0);
        if (fd == NAI_FD_INVALID) {
            /* no file to close */
            r = -1;
            goto _end;
        };
    };

#endif

_done:

    /* when you need to verify the owner or get the status of a file */
    if (!stated && (
        verify == NAI_OPEN_VERIFY_OWNER || st)) {

        if (st == 0) {
            st = &st_fd;
        };

        if (fd == NAI_FD_INVALID) {
            if (verify != NAI_OPEN_VERIFY_OWNER) {
                r = nai_stat(p, st, NAI_STAT_BASIC);
            } else {
                r = nai_stat(p, st, NAI_STAT_PERM);
            };
        } else {
            if (verify != NAI_OPEN_VERIFY_OWNER) {
                r = nai_open_file_stat(fd, st, NAI_STAT_BASIC);
            } else {
                r = nai_open_file_stat(fd, st, NAI_STAT_PERM);
            };
        };
        if (r < 0) {
            goto _fail;
        };
    };

    if (verify == NAI_OPEN_VERIFY_OWNER) {
        if (st->st_uid != st_root.st_uid) {
            nai_errno = ELOOP;
            goto _fail;
        };
    };

    if (of != 0) {
        of->fd = fd;
        of->cf = 0;
        of->oflags = flags;
    };

    r = 0;

_end:
    return r;

_fail:

#if (NAI_USE_OPENAT)

    ec = nai_errno;

    if (at != NAI_FD_INVALID && at != NAI_FD_CWD) {
        nai_file_close(at);
    };

    if (fd != NAI_FD_INVALID) {
        nai_file_close(fd);
    };

    nai_errno = ec;

#endif

    r = -1;

    goto _end;
};


static nai_int_t nai_stat_file(
    const nai_str_t* name, const nai_open_attr_t* attr, 
    nai_stat_t* st)
{
    nai_int_t r;
    const char* p;


    if (attr->verify == NAI_OPEN_VERIFY_OFF) {
        p = nai_str(name);
        r = nai_stat(p, st, NAI_STAT_BASIC);
        goto _end;
    };

    r = nai_open_file(name, attr, NAI_O_SEARCH, 0, st);
    if (r < 0) {
        goto _end;
    };

_end:
    return r;
};


static nai_int_t nai_open_cache_update(
    nai_open_cache_t* p, 
    nai_open_cache_file_t* cf, 
    const nai_open_attr_t* attr, uint64_t now)
{
    nai_int_t r;
    nai_int_t v;
    nai_int_t oflags;
    nai_stat_t st;
    nai_open_file_t of;


    v = attr->verify;

    if (cf->fd != NAI_FD_INVALID) {
        r = nai_stat_file(&cf->name, attr, &st);
        if (r < 0) {
            cf->fd = NAI_FD_INVALID;
            cf->oflags = 0;
            cf->sult[v].valid = 1;
            cf->sult[v].error = nai_errno;
            cf->sult[v].update = now;
            goto _end;
        };

        if (cf->st.st_ino == st.st_ino) {
            cf->st = st;
            cf->sult[v].valid = 1;
            cf->sult[v].error = 0;
            cf->sult[v].update = now;
            goto _end;
        };
    };

    oflags = p->oflags | NAI_O_RDONLY;

    r = nai_open_file(&cf->name, attr, oflags, &of, &st);
    if (r < 0) {
        cf->sult[v].valid = 1;
        cf->sult[v].error = nai_errno;
        cf->sult[v].update = now;
        goto _end;
    };

    cf->fd = of.fd;
    cf->st = st;
    cf->oflags = of.oflags;
    cf->sult[v].valid = 1;
    cf->sult[v].error = 0;
    cf->sult[v].root_length = attr->root_length;
    cf->sult[v].update = now;
    r = 0;

_end:
    return r;
};


static void nai_open_cache_expired(nai_open_cache_t* p, nai_int_t force)
{
    int64_t timeo;
    uint64_t now;
    nai_list_entry_t* e;
    nai_open_cache_file_t* cf;


    now = nai_time();

    e = p->queue.next;
    for ( ; e != &p->queue;) {
        cf = nai_containof(e, nai_open_cache_file_t, ent);
        e = e->next;

        if (!force && (
            (timeo = now - cf->access) < 0 || 
            (uint64_t)timeo < p->expire_timeo)) {
            break;
        };

        nai_rbtree_erase(&p->cmap, &cf->node);
        nai_list_entry_remove(&cf->ent);
        p->count --;
        force = 0;

        if (cf->refs > 0) {
            cf->closed = 1;
            continue;
        };

        if (cf->fd != NAI_FD_INVALID) {
            nai_file_close(cf->fd);
        };
        nai_free(cf);
    };

    return;
};


static void nai_open_cache_cleanup(void* u)
{
    nai_open_cache_t* p;
    nai_open_cache_file_t* cf;


    cf = (nai_open_cache_file_t*)u;

    if (!cf->closed) {
        p = cf->cache;

        /* update access time */
        cf->access = nai_time();
        nai_list_entry_remove(&cf->ent);
        nai_list_insert_tail(&p->queue, &cf->ent);

        /* cleanup expired cache */
        nai_open_cache_expired(p, 0);
    };

    cf->refs --;
    if (cf->refs > 0) {
        goto _end;
    };

    if (cf->fd != NAI_FD_INVALID) {
        p = cf->cache;
        if (cf->closed || cf->use >= (intptr_t)p->min_use) {
            nai_file_close(cf->fd);
            cf->fd = NAI_FD_INVALID;
        };
    };

    if (cf->closed) {
        nai_free(cf);
    };

_end:
    return;
};


static void nai_open_file_cleanup(void* u)
{
    nai_fd_t fd;


    fd = (nai_fd_t)(intptr_t)u;
    nai_file_close(fd);

    return;
};


static nai_rbnode_t** nai_open_cache_lookup(
    nai_open_cache_t* p, const nai_str_t* name, nai_rbnode_t** pparent)
{
    intptr_t r;
    nai_rbnode_t** n = &nai_rbtree_root(&p->cmap);
    nai_rbnode_t* parent = nai_rbtree_end(&p->cmap);
    nai_open_cache_file_t* c;


    while (*n) {
        parent = *n;
        c = nai_containof(parent, nai_open_cache_file_t, node);

        r = (nai_int_t)c->name.hash - name->hash;
        if (r != 0) {
            if (r > 0) {
                n = &parent->rb_left;
            } else {
                n = &parent->rb_right;
            };
            continue;
        };

        r = nai_str_casecmp(&c->name, name);
        if (r == 0) {
            break;
        } else if (r > 0) {
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



nai_int_t nai_open_cache_init(nai_open_cache_t* p)
{
    nai_int_t r;


    nai_rbtree_init(&p->cmap);
    nai_list_init(&p->queue);
    p->count = 0;
    p->max_count = 128;
    p->min_use = 1;
    p->expire_timeo = 30 * 1000;
    r = 0;

    return r;
};


nai_int_t nai_open_cache_open(nai_open_cache_t* p)
{
    nai_int_t r;

    if (p->oflags & (NAI_O_CREAT|NAI_O_EXCL|NAI_O_TMPFILE)) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    r = 0;

_end:
    return r;
};


nai_int_t nai_open_cache_close(nai_open_cache_t* p)
{
    nai_int_t r;
    nai_list_entry_t* e;
    nai_open_cache_file_t* cf;


    e = p->queue.next;
    for ( ; e != &p->queue; ) {
        cf = nai_containof(e, nai_open_cache_file_t, ent);
        e = e->next;

        if (cf->refs > 0) {
            cf->closed = 1;
            continue;
        };

        if (cf->fd != NAI_FD_INVALID) {
            nai_file_close(cf->fd);
        };

        nai_free(cf);
    };

    nai_rbtree_init(&p->cmap);
    nai_list_init(&p->queue);
    r = 0;

    return r;
};


nai_int_t nai_open_and_stat(
    nai_open_cache_t* p, 
    const nai_str_t* path, nai_open_attr_t* attr)
{
    intptr_t r;
    nai_int_t ec;
    nai_int_t v;
    int64_t timeo;
    uint64_t now;
    nai_str_t name;
    nai_open_file_t* of;
    nai_open_cache_file_t cp;
    nai_open_cache_file_t* cf;
    nai_rbnode_t** n;
    nai_rbnode_t* parent;
    char buf[NAI_PATH_MAX];


    v = attr->verify;
    if (v < NAI_OPEN_VERIFY_OFF || 
        v > NAI_OPEN_VERIFY_OWNER) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    if (p == 0) {
        of = attr->of;
        if (of == 0) {
            r = nai_stat_file(path, attr, &attr->st);
            goto _end;
        };

        r = nai_open_file(path, 
            attr, NAI_O_RDONLY, of, &attr->st);

        goto _end;
    };

    if (p->oflags & (NAI_O_CREAT|NAI_O_EXCL|NAI_O_TMPFILE)) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    /* fixup path */
    r = nai_path_get_real(buf, sizeof(buf), nai_str(path));
    if (r < 0) {
        goto _end;
    };

    nai_str_setm(&name, buf, r);
    nai_str_hash(&name, 0);


    now = nai_time();

    /* lookup cache */
    n = nai_open_cache_lookup(p, &name, &parent);
    if (n[0]) {
        cf = (nai_open_cache_file_t*)n[0];

        /* found */
        if (cf->fd == NAI_FD_INVALID && (
            cf->sult[v].valid == 0 || 
            cf->sult[v].error == 0)) {

            assert(cf->refs <= 0);

            r = nai_open_cache_update(p, cf, attr, now);
            if (r < 0) {
                goto _end;
            };

            goto _found;
        };

        ;
        if (cf->sult[v].valid && 
            cf->sult[v].root_length == attr->root_length && (
            (timeo = now - cf->sult[v].update) < 0 || 
            (uint64_t)timeo < attr->valid_timeo)) {

            /* use cache directly */
            goto _found;
        };


        cp = *cf;
        r = nai_open_cache_update(p, &cp, attr, now);
        if (r < 0) {
            r = -1;
            goto _end;
        };

        if (cf->fd != cp.fd && cf->refs) {
            cf->closed = 1;
            nai_rbtree_erase(&p->cmap, &cf->node);
            nai_list_entry_remove(&cf->ent);
            p->count --;

            cp.sult[0].valid = 0;
            cp.sult[1].valid = 0;
            cp.sult[2].valid = 0;
            cp.sult[v].valid = 1;

            n = nai_open_cache_lookup(p, &name, &parent);
            assert(n[0] == 0);
            goto _create;
        };

        if (cf->fd != cp.fd) {
            if (cf->fd != NAI_FD_INVALID) {
                nai_file_close(cf->fd);
            };

            cf->fd = cp.fd;
            cf->sult[0].valid = 0;
            cf->sult[1].valid = 0;
            cf->sult[2].valid = 0;
        };

        cf->st = cp.st;
        cf->sult[v] = cp.sult[v];
        goto _found;
    };


    cp.name = name;
    cp.fd = NAI_FD_INVALID;
    cp.oflags = 0;
    cp.sult[0].valid = 0;
    cp.sult[1].valid = 0;
    cp.sult[2].valid = 0;

    r = nai_open_cache_update(p, &cp, attr, now);
    if (r < 0) {
        goto _end;
    };


_create:
    if (p->count >= p->max_count) {
        nai_open_cache_expired(p, 1);
    };

    cf = (nai_open_cache_file_t*)
        nai_malloc(sizeof(*cf) + nai_str_len(&name) + 1);
    if (cf == 0) {
        if (cp.fd != NAI_FD_INVALID) {
            ec = nai_errno;
            nai_file_close(cp.fd);
            nai_errno = ec;
        };

        r = -1;
        goto _end;
    };

    /* initial file */
    cf->cache = p;
    cf->fd = cp.fd;
    cf->st = cp.st;
    cf->use = 0;
    cf->refs = 0;
    cf->closed = 0;
    cf->oflags = cp.oflags;
    cf->sult[0] = cp.sult[0];
    cf->sult[1] = cp.sult[1];
    cf->sult[2] = cp.sult[2];

    /* set name */
    nai_str_setm(&cf->name, (char*)(cf + 1), nai_str_len(&name));
    nai_memcpy((char*)(cf + 1), nai_str(&name), nai_str_len(&name) + 1);

    /* link */
    nai_rbtree_link(&p->cmap, &cf->node, parent, n);
    nai_rbtree_color(&p->cmap, &cf->node);
    nai_list_init(&cf->ent);
    p->count ++;


_found:
    /* update access time */
    cf->use ++;
    cf->access = now;
    nai_list_entry_remove(&cf->ent);
    nai_list_insert_tail(&p->queue, &cf->ent);

    /* check error */
    if (cf->sult[v].error) {
        nai_errno = cf->sult[v].error;
        r = -1;
        goto _end;
    };

    /* copy stat */
    attr->st = cf->st;

    /* reference the file descriptor */
    of = attr->of;
    if (of) {
        of->oflags = cf->oflags;
        of->fd = cf->fd;
        if (cf->fd != NAI_FD_INVALID) {
            of->cf = cf;
            cf->refs ++;
        };
    };

_end:
    return (nai_int_t)r;
};


nai_int_t nai_open_file_close(nai_open_file_t* of)
{
    nai_int_t r;


    if (of->fd == NAI_FD_INVALID) {
        r = 0;
        goto _end;
    };

    if (of->cf) {
        nai_open_cache_cleanup(of->cf);
    } else {
        nai_file_close(of->fd);
    };

    of->cf = 0;
    of->fd = NAI_FD_INVALID;
    of->oflags = 0;
    r = 0;

_end:
    return r;
};


nai_int_t nai_open_file_add_cleanup(nai_open_file_t* of, nai_pool_t* pool)
{
    nai_int_t r;


    if (of->fd == NAI_FD_INVALID) {
        r = 0;
        goto _end;
    };

    if (of->cf) {
        r = nai_pool_add_cleanup(pool, 
            nai_open_cache_cleanup, of->cf);
    } else {
        r = nai_pool_add_cleanup(pool, 
            nai_open_file_cleanup, (void*)(intptr_t)of->fd);
    };

_end:
    return r;
};


