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
/// @file       nai_fs_watch.c
/// @brief      
/// @details
/// @date       2023-10-29
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai/io/nai_fs_watch.h"
#include "nai/os/nai_file.h"
#include "nai/os/nai_tlocal.h"
#include "nai/event/nai_evmsg.h"
#include "nai/runtime/nai_list.h"
#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_util.h"
#include "nai/runtime/nai_log.h"



#if (NAI_HAVE_FS_WATCH)



#ifndef NAI_FS_POLL_INTERVAL
#define NAI_FS_POLL_INTERVAL    1000
#endif



#ifndef _NAI_TYPEDEF_FS_WATCH_KEY_T
#define _NAI_TYPEDEF_FS_WATCH_KEY_T
typedef struct nai_fs_watch_key_s nai_fs_watch_key_t;
#endif
#ifndef _NAI_TYPEDEF_FS_WATCH_MSG_T
#define _NAI_TYPEDEF_FS_WATCH_MSG_T
typedef struct nai_fs_watch_msg_s nai_fs_watch_msg_t;
#endif
#ifndef _NAI_TYPEDEF_FS_WATCH_UDIR_T
#define _NAI_TYPEDEF_FS_WATCH_UDIR_T
typedef struct nai_fs_watch_udir_s nai_fs_watch_udir_t;
#endif
#ifndef _NAI_TYPEDEF_FS_WATCH_SERV_T
#define _NAI_TYPEDEF_FS_WATCH_SERV_T
typedef struct nai_fs_watch_serv_s nai_fs_watch_serv_t;
#endif
#ifndef _NAI_TYPEDEF_FS_WATCH_PORT_T
#define _NAI_TYPEDEF_FS_WATCH_PORT_T
typedef struct nai_fs_watch_port_s nai_fs_watch_port_t;
#endif
#ifndef _NAI_TYPEDEF_FS_WATCH_NODE_T
#define _NAI_TYPEDEF_FS_WATCH_NODE_T
typedef struct nai_fs_watch_node_s nai_fs_watch_node_t;
#endif
#ifndef _NAI_TYPEDEF_FS_WATCH_HANDLE_T
#define _NAI_TYPEDEF_FS_WATCH_HANDLE_T
typedef struct nai_fs_watch_handle_s nai_fs_watch_handle_t;
#endif
#ifndef _NAI_TYPEDEF_FS_WATCH_SERV_GLOBAL_T
#define _NAI_TYPEDEF_FS_WATCH_SERV_GLOBAL_T
typedef struct nai_fs_watch_serv_global_s nai_fs_watch_serv_global_t;
#endif



struct nai_fs_watch_key_s {
    nai_str_t path;
    nai_int_t flags;
};


struct nai_fs_watch_msg_s {
    nai_int_t events;
    const char* path;
};


struct nai_fs_watch_udir_s {
    nai_fs_watch_t w;
    nai_fs_watch_node_t* node;
};


struct nai_fs_watch_handle_s {
    nai_evmsg_handle_t h;
    nai_fs_watch_key_t key;
};


#if (NAI_FS_WATCH_USE_WIN32)


#include "nai/io/nai_osloop.h"
#include <nai/os/nai_aio.h>


typedef struct nai_fs_watch_file_s nai_fs_watch_file_t;


struct nai_fs_watch_file_s {
    nai_rbnode_t ent;
    uint64_t ino;
    uint64_t dev;
    nai_evnode_t ev;
    nai_fs_watch_serv_t* serv;
    nai_fs_watch_port_t* port;
    nai_list_t list;
    nai_aio_t aio;
    nai_atomic32_t refs;
    union {
        struct {
            uint8_t closed;
            uint8_t pending;
        };
        uint32_t stat;
    };
    uint8_t buf[1024];
};


struct nai_fs_watch_serv_s {
    nai_evmsg_serv_t serv;
    nai_int_t uid;
    nai_rbtree_t fmap;          /**< the watch map */
};


struct nai_fs_watch_port_s {
    nai_evmsg_port_t port;
    nai_evnode_cb_f cb;         /**< pointer to the origin callback */
    nai_osloop_t* osloop;       /**< pointer to the global loop */
    nai_list_t plist;           /**< the polling list */
    nai_rbtree_t fmap;          /**< the watch map */
};


struct nai_fs_watch_node_s {
    nai_evmsg_node_t node;      /**< the fs watch node */
    nai_fs_watch_key_t key;
    nai_fs_watch_udir_t* udir;
    nai_fs_watch_file_t* file;
    nai_list_entry_t entp;
    nai_list_entry_t entf;
    nai_int_t dir;
    nai_atomic32_t refs;
};


#elif (NAI_FS_WATCH_USE_LINUX)


#define NAI_FS_WATCH_NO_GLOBAL  1


struct nai_fs_watch_serv_s {
    nai_evmsg_serv_t serv;
    nai_int_t uid;
};


struct nai_fs_watch_port_s {
    nai_evmsg_port_t port;
    nai_evnode_cb_f cb;         /**< the origin callback */
    nai_list_t plist;           /**< the polling list */
    nai_rbtree_t wmap;          /**< the watch map */
    nai_fd_t fd;                /**< the inotify fd */
    size_t buflen;              /**< the buffer length */
    uint8_t buf[2048];          /**< the buffer */
};


struct nai_fs_watch_node_s {
    nai_evmsg_node_t node;      /**< the fs watch node */
    nai_fs_watch_key_t key;
    nai_fs_watch_udir_t* udir;
    nai_list_entry_t entp;
    nai_rbnode_t entw;
    nai_int_t wd;
};


#elif (NAI_FS_WATCH_USE_KQUEUE)


#if (__darwin__)
#include "nai/io/nai_osloop.h"
#endif

#include "nai/os/nai_proc.h"
#include <sys/event.h>


typedef struct nai_fs_watch_fd_s nai_fs_watch_fd_t;
typedef struct nai_fs_watch_file_s nai_fs_watch_file_t;


struct nai_fs_watch_fd_s {
    nai_rbnode_t ent;
    uint64_t ino;
    uint64_t dev;
    nai_fd_t fd;
    nai_atomic32_t refs;
};


struct nai_fs_watch_file_s {
    nai_rbnode_t ent;
    uint64_t ino;
    uint64_t dev;
    nai_evnode_t ev;
    nai_fs_watch_port_t* port;
    nai_fs_watch_fd_t* fd;
    nai_list_t list;
    nai_atomic32_t refs;
};


struct nai_fs_watch_serv_s {
    nai_evmsg_serv_t serv;
    nai_int_t uid;
    nai_rbtree_t fmap;
    nai_fixedpool_t pool;
};


struct nai_fs_watch_port_s {
    nai_evmsg_port_t port;
    nai_evnode_cb_f cb;         /**< pointer to the origin callback */
#if (__darwin__)
    nai_osloop_t* osloop;       /**< pointer to the global loop */
#endif
    nai_list_t plist;           /**< the polling list */
    nai_rbtree_t fmap;
    nai_fd_t fd;                /**< the kqueue fd */
    nai_pid_t pid;              /**< the process id of kqueue */
    nai_int_t nevent;
    struct kevent ea[32];
};


struct nai_fs_watch_node_s {
    nai_evmsg_node_t node;      /**< the fs watch node */
    nai_fs_watch_key_t key;
    nai_fs_watch_udir_t* udir;
    nai_fs_watch_file_t* file;
    nai_list_entry_t entp;
    nai_list_entry_t entf;
};


#elif (NAI_FS_WATCH_USE_POLL)


#define NAI_FS_WATCH_NO_UDIR    1
#define NAI_FS_WATCH_NO_GLOBAL  1


#include "nai/io/nai_osloop.h"
#include "nai/os/nai_stat.h"


typedef struct nai_fs_watch_fd_s nai_fs_watch_fd_t;
typedef struct nai_fs_watch_file_s nai_fs_watch_file_t;


struct nai_fs_watch_fd_s {
    nai_int_t flags;
    nai_str_t path;
    struct {
        uint64_t ino;
        uint64_t dev;
        nai_fd_t fd;
    };
};

struct nai_fs_watch_file_s {
    nai_rbnode_t ent;
    nai_fs_watch_fd_t fd;
    nai_list_t list;
    nai_list_t entn;
    nai_int_t events;
    nai_atomic32_t refs;
    nai_stat_t stp;
    nai_stat_t stf;
};


struct nai_fs_watch_serv_s {
    nai_evmsg_serv_t serv;
    nai_atomic32_t refs;
    nai_int_t uid;
    nai_int_t rate;
    nai_osloop_t* osloop;
    nai_evnode_t timer;
    nai_rbtree_t fmap;
    nai_list_t tasks;
    nai_list_t pendings;
    nai_fixedpool_t pool;
};


struct nai_fs_watch_port_s {
    nai_evmsg_port_t port;
    nai_evnode_cb_f cb;         /**< the origin callback */
};


struct nai_fs_watch_node_s {
    nai_evmsg_node_t node;      /**< the fs watch node */
    nai_fs_watch_key_t key;
    nai_fs_watch_file_t* file;
    nai_list_entry_t entf;
};


#endif



//////////////////////////////////////////////////////////////////////////////
// platform-independent implementation


#if !(NAI_FS_WATCH_NO_UDIR)

static nai_int_t nai_fs_watch_port_node_moved(nai_evmsg_node_t* e);
static nai_int_t nai_fs_watch_process_udir(
    nai_fs_watch_t* w, nai_int_t events, const char* path);

#endif


static nai_rbnode_t** nai_fs_watch_find_impl(
    nai_rbtree_t* t, void* key, nai_rbnode_t** pparent, size_t offset)
{
    nai_int_t c;
    nai_rbnode_t** n = &nai_rbtree_root(t);
    nai_rbnode_t* parent = nai_rbtree_end(t);
    nai_fs_watch_key_t* k;
    nai_fs_watch_key_t* e;


    k = (nai_fs_watch_key_t*)key;
    while (*n) {
        parent = *n;
        e = (nai_fs_watch_key_t*)((uint8_t*)parent + offset);

#if (NAI_PATH_CASE_SENSITIVE)
        c = nai_str_cmp(&e->path, &k->path);
#else
        c = nai_str_casecmp(&e->path, &k->path);
#endif
        if (c != 0) {
            if (c >= 0) {
                n = &parent->rb_left;
            } else {
                n = &parent->rb_right;
            };
            continue;
        };

        c = e->flags - k->flags;
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


static nai_rbnode_t** nai_fs_watch_find(
    nai_rbtree_t* t, void* key, nai_rbnode_t** pparent)
{
    return nai_fs_watch_find_impl(t, 
        key, pparent, nai_offsetof(nai_fs_watch_node_t, key));
};


static nai_int_t nai_fs_watch_serv_lock_map(
    nai_evmsg_serv_t* s, nai_int_t block)
{
    nai_int_t r;


    (void)block;

    r = nai_spin_lock(&s->lock);
    if (r < 0) {
        goto _fail;
    };

    r = 0;

_end:
    return r;

_fail:
    /* failed */
    goto _end;
};


static nai_int_t nai_fs_watch_serv_unlock_map(
    nai_evmsg_serv_t* s, nai_int_t block)
{
    nai_int_t r;


    (void)block;

    r = nai_spin_unlock(&s->lock);
    if (r < 0) {
        goto _fail;
    };

    r = 0;

_end:
    return r;

_fail:
    /* failed */
    goto _end;
};


static nai_int_t nai_fs_watch_handle_init(nai_evmsg_handle_t* p, void* key)
{
    nai_int_t r;
    nai_fs_watch_key_t* k;
    nai_fs_watch_handle_t* h;


    k = (nai_fs_watch_key_t*)key;
    h = (nai_fs_watch_handle_t*)p;
    h->key.path = k->path;
    h->key.flags = k->flags;
    r = 0;

    return r;
};


static nai_int_t nai_fs_watch_handle_call(
    nai_evmsg_handle_t* p, void* data, size_t len)
{
    nai_int_t r;
    nai_fs_watch_t* w;
    nai_fs_watch_msg_t* m;
    nai_fs_watch_handle_t* h;


    (void)len;

    h = (nai_fs_watch_handle_t*)p;
    w = (nai_fs_watch_t*)h->h.handle;
    m = (nai_fs_watch_msg_t*)data;
#if (NAI_FS_WATCH_USE_WIN32)
    if (len > sizeof(*m)) {
        m->path = (char*)(m + 1);
    };
#else
    if (w->cb && (m->events & NAI_EV_ERROR)) {
        w->cb(w, nai_ev_error_from((intptr_t)m->path), 0);
    };
#endif
#if !(NAI_FS_WATCH_NO_UDIR)
    if (w->cb && (m->events & NAI_EV_TIMEOUT)) {
        if (w->cb == nai_fs_watch_process_udir) {
            w->cb(w, NAI_FS_CHANGE, m->path);
        };
    };
#endif
    if (w->cb && (m->events & NAI_FS_CHANGE)) {
        w->cb(w, NAI_FS_CHANGE, m->path);
    };
    if ((m->events & NAI_FS_MOVE_ADD) == NAI_FS_MOVE_ADD && 
        !h->h.removed && w->cb) {
        w->cb(w, NAI_FS_MOVE_ADD, m->path);
    };
    if ((m->events & NAI_FS_MOVE_DEL) == NAI_FS_MOVE_DEL && 
        !h->h.removed && w->cb) {
        w->cb(w, NAI_FS_MOVE_DEL, m->path);
    };

    r = 0;

    return r;
};



static nai_int_t nai_fs_watch_str_ref(nai_str_t* s, nai_str_t* c)
{
    nai_int_t r;
    nai_atomic32_t* ref;


    ref = (nai_atomic32_t*)((uint8_t*)nai_str(c) - sizeof(*ref));
    nai_atomic32_inc(ref);
    s[0] = c[0];
    r = 0;

    return r;
};


static nai_int_t nai_fs_watch_str_unref(nai_str_t* s)
{
    nai_int_t r;
    nai_atomic32_t* ref;


    ref = (nai_atomic32_t*)((uint8_t*)nai_str(s) - sizeof(*ref));
    r = nai_atomic32_dec(ref);
    if (r <= 0) {
        nai_free(ref);
    };

    nai_str_setn(s);
    r = 0;

    return r;
};


#if !(NAI_FS_WATCH_NO_UDIR)


static nai_int_t nai_fs_watch_process_udir(
    nai_fs_watch_t* w, nai_int_t events, const char* path)
{
    nai_int_t r;
    nai_int_t len;
    nai_fs_watch_udir_t* u;
    nai_fs_watch_node_t* n;
    nai_fs_watch_msg_t m;
    nai_evmsg_hdr_t h;
    char* name;


    u = (nai_fs_watch_udir_t*)w;
    n = u->node;

    switch (events & NAI_EV_MASK) {
    case NAI_EV_ERROR:
#if 0
        /* need boradcast error ? */
        m.events = NAI_EV_ERROR;
        m.path = (char*)(intptr_t)nai_ev_error_code(events);
        h.e = &n->node;
        h.token = n->node.token;
        h.len = sizeof(m);
        nai_evmsg_port_once_msg(n->node.port, &h, &m);
#endif
        break;

    case NAI_EV_NOTIFY:
        nai_fs_watch_close(w);
        nai_free(w);
        break;


    case NAI_FS_MOVE_ADD:
    case NAI_FS_MOVE_DEL:
        if (n == 0) {
            break;
        };

        if (path != 0) {
            len = (nai_int_t)nai_strlen(path);
            if (nai_str_len(&w->path) + 1 + len != 
                nai_str_len(&n->key.path)) {
                break;
            };

            name = nai_str(&n->key.path) + nai_str_len(&w->path) + 1;
#if (NAI_PATH_CASE_SENSITIVE)
            if (nai_strncmp(path, name, len) != 0) {
                break;
            };
#else
            if (nai_strncasecmp(path, name, len) != 0) {
                break;
            };
#endif
        };

#if !(NAI_HAVE_FS_WATCH_CHILD)

    case NAI_FS_CHANGE:
        if (n == 0) {
            break;
        };

#endif

        /* try to reload */
        r = nai_fs_watch_port_node_moved(&n->node);
        if (r != 0) {
            m.events = r;
            m.path = 0;
            h.e = &n->node;
            h.token = n->node.token;
            h.len = sizeof(m);
            nai_evmsg_port_once_msg(n->node.port, &h, &m);
        };
        break;

    default:
        break;
    };

    r = 0;

    return r;
};


static nai_int_t nai_fs_watch_open_udir_impl(
    nai_fs_watch_udir_t* u, nai_fs_watch_node_t* n)
{
    nai_int_t r;
    nai_int_t ec;
    nai_evloop_t* l;
    nai_fs_watch_port_t* w;
    intptr_t len;
    char* slash;
    char* path;
    char* parent;
    char buf[256];


    nai_fs_watch_init(&u->w);

    /* get parent */
    path = nai_str(&n->key.path);
    len = nai_path_get_root_len(path, nai_str_len(&n->key.path));
    slash = strrchr(path + len, nai_path_sep);
    if (slash) {
        len = slash - path;
    };

    if (len + 1 < (intptr_t)sizeof(buf)) {
        parent = buf;
    } else {
        parent = (char*)nai_malloc(len + 1);
        if (parent == 0) {
            r = -1;
            goto _end;
        };
    };

    nai_memcpy(parent, path, len);
    parent[len] = 0;

    /* open fs watch */
    nai_fs_watch_set_cb(&u->w, nai_fs_watch_process_udir);
    w = (nai_fs_watch_port_t*)n->node.port;
    l = nai_evnode_get_loop(&w->port.ev);
    r = nai_fs_watch_open(&u->w, l, 
        parent, NAI_FS_WATCH_CHILD | NAI_FS_WATCH_PATH);
    if (r < 0) {
        goto _fail;
    };

    n->udir = u;
    u->node = n;

    if (parent != buf) {
        nai_free(parent);
    };


_end:
    return r;

_fail:
    ec = nai_errno;
    nai_free(parent);
    nai_errno = ec;
    goto _end;
};


static nai_int_t nai_fs_watch_open_udir(nai_fs_watch_node_t* n)
{
    nai_int_t r;
    nai_int_t ec;
    size_t rootlen;
    nai_fs_watch_udir_t* u;


    rootlen = nai_path_get_root_len(
        nai_str(&n->key.path), nai_str_len(&n->key.path));
    if (rootlen == nai_str_len(&n->key.path)) {
        r = 0;
        goto _end;
    };

    u = (nai_fs_watch_udir_t*)nai_malloc(sizeof(*u));
    if (u == 0) {
        r = -1;
        goto _end;
    };

    r = nai_fs_watch_open_udir_impl(u, n);
    if (r < 0) {
        ec = nai_errno;
        nai_free(u);
        nai_errno = ec;
    };

_end:
    return r;
};


static nai_int_t nai_fs_watch_close_udir(nai_fs_watch_node_t* n)
{
    nai_int_t r;
    nai_fs_watch_udir_t* u;


    u = n->udir;
    if (u == 0) {
        r = 0;
        goto _end;
    };

    u->node = 0;
    n->udir = 0;
    r = nai_fs_watch_close(&u->w);
    assert(r >= 0);
    nai_free(u);

_end:
    return r;
};


#define NAI_FS_POLL_ADD         NAI_FS_MOVE_DEL
#define NAI_FS_POLL_DEL         NAI_FS_MOVE_ADD
#define nai_fs_poll_from(e)     (e)


static nai_int_t nai_fs_watch_poll(nai_fs_watch_node_t* n, nai_int_t event)
{
    nai_int_t r;
    nai_fs_watch_port_t* w;


    if (!(n->key.flags & (NAI_FS_WATCH_PATH|NAI_FS_WATCH_CHILD))) {
        r = 0;
        goto _end;
    };

    w = (nai_fs_watch_port_t*)n->node.port;
    switch (event) {
    case NAI_FS_POLL_ADD:
        r = nai_list_is_empty(&w->plist);
        nai_list_insert_tail(&w->plist, &n->entp);
        if (r != 0) {
            nai_evnode_set_timeout(
                &w->port.ev, NAI_TIMEOP_SET, NAI_FS_POLL_INTERVAL / 2);
        };
        break;
    case NAI_FS_POLL_DEL:
        nai_list_entry_remove(&n->entp);
        nai_list_init(&n->entp);
        break;
    default:
        assert(0);
        break;
    };

    r = 0;

_end:
    return r;
};


#endif


static nai_int_t nai_fs_watch_port_node_error(
    nai_evmsg_node_t* e, nai_int_t ecode)
{
    nai_int_t r;
    nai_evmsg_port_t* p;
    nai_evmsg_hdr_t h;
    nai_fs_watch_msg_t m;


    p = e->port;
    h.e = e;
    h.token = e->token;
    h.len = sizeof(m);
    m.events = NAI_EV_ERROR;
    m.path = (char*)(intptr_t)ecode;
    r = nai_evmsg_port_sendto(p, &h, &m);

    return r;
};


static nai_int_t nai_fs_watch_port_node_clean(nai_evmsg_node_t* e)
{
    nai_int_t r;


#if !(NAI_FS_WATCH_NO_UDIR)
    nai_fs_watch_node_t* n;


    n = (nai_fs_watch_node_t*)e;
    r = nai_fs_watch_close_udir(n);
    assert(r >= 0);
#else
    (void)e;
#endif

    r = 0;

    return r;
};


static nai_int_t nai_fs_watch_port_read(nai_evmsg_port_t* p);
static nai_int_t nai_fs_watch_port_handle(nai_evnode_t* e, nai_int_t events)
{
    nai_int_t r;
    nai_fs_watch_port_t* w;

#if !(NAI_FS_WATCH_NO_UDIR)
    nai_list_t list;
    nai_list_entry_t* i;
    nai_fs_watch_node_t* n;
    nai_fs_watch_msg_t m;
    nai_evmsg_hdr_t h;
#endif


    w = nai_containof(e, nai_fs_watch_port_t, port.ev);
    switch (events & NAI_EV_MASK) {
    case NAI_EV_READ:
        nai_fs_watch_port_read(&w->port);
        break;

#if !(NAI_FS_WATCH_NO_UDIR)
    case NAI_EV_TIMEOUT:
        if (nai_list_is_empty(&w->plist)) {
            break;
        };

        /* set next time */
        nai_evnode_set_timeout(&w->port.ev, 
            NAI_TIMEOP_ADD, NAI_FS_POLL_INTERVAL);

        /* poll children */
        m.events = NAI_EV_TIMEOUT;
        m.path = 0;
        nai_list_init(&list);
        nai_list_add_tail(&list, &w->plist);
        for ( ; !nai_list_is_empty(&list); ) {
            i = list.next;
            n = nai_containof(i, nai_fs_watch_node_t, entp);
            nai_list_entry_remove(i);
            nai_list_insert_tail(&w->plist, i);

            h.e = &n->node;
            h.token = n->node.token;
            h.len = sizeof(m);
            nai_evmsg_port_once_msg(&w->port, &h, &m);
        };
        break;
#endif

    default:
        w->cb(e, events);
        break;
    };

    r = 0;

    return r;
};



//////////////////////////////////////////////////////////////////////////////
// platform-related implementation


#if (NAI_FS_WATCH_USE_WIN32)


#include "nai/os/win/nai_windows.h"
#include "nai/os/nai_stat.h"
#include <windows.h>


static nai_int_t nai_fs_watch_test(
    nai_fs_watch_node_t* n, const char* path, size_t len)
{
    nai_int_t r;
    const char* name;


    if (nai_str_len(&n->key.path) <= len) {
        r = 0;
        goto _end;
    };

    name = nai_str(&n->key.path);
    name += nai_str_len(&n->key.path) - len - 1;
    if (name[0] != nai_path_sep || 
        nai_strncasecmp(name + 1, path, len) != 0) {
        r = 0;
        goto _end;
    };

    r = 1;

_end:
    return r;
};


#define nai_fs_watch_node_is_dir(n)                     \
    ((n)->node.share ?                                  \
        ((nai_fs_watch_node_t*)(n)->node.glob)->dir :   \
        (n)->dir)                                       \


static nai_int_t nai_fs_watch_read_dir(nai_fs_watch_file_t* f);
static nai_int_t nai_fs_watch_read_dir_complete(
    nai_aio_t* a, nai_int_t err, size_t bytes)
{
    nai_int_t r;
    nai_int_t dir;
    nai_fs_watch_serv_t* s;
    nai_fs_watch_port_t* w;
    nai_fs_watch_file_t* f;
    nai_fs_watch_node_t* n;
    nai_evmsg_hdr_t h;
    nai_list_entry_t* e;
    nai_list_t list;
    FILE_NOTIFY_INFORMATION* data;
    struct {
        nai_fs_watch_msg_t m;
        char name[NAI_PATH_MAX * 3];
    } m;


    f = nai_containof(a, nai_fs_watch_file_t, aio);
    f->pending = 0;
    if (f->closed) {
        nai_free(f);
        r = 0;
        goto _end;
    };

    if (err) {
        /* handle error */
        r = 0;
        goto _end;
    };

    /* handle data */
    data = (FILE_NOTIFY_INFORMATION*)f->buf;
    for ( ; bytes; ) {
        m.m.events = 0;
        switch (data->Action) {
        case FILE_ACTION_MODIFIED:
            m.m.events = NAI_FS_CHANGE;
            break;
        case FILE_ACTION_ADDED:
        case FILE_ACTION_RENAMED_NEW_NAME:
            m.m.events = NAI_FS_MOVE_ADD;
            break;
        case FILE_ACTION_REMOVED:
        case FILE_ACTION_RENAMED_OLD_NAME:
            m.m.events = NAI_FS_MOVE_DEL;
            break;
        default:
            assert(0);
            break;
        };

        r = (nai_int_t)nai_wcs_to_utf8(
            m.name, sizeof(m.name), 
            data->FileName, data->FileNameLength/sizeof(WCHAR));
        if (r >= 0) {
            m.name[r] = 0;
            if (f->port == 0) {
                m.m.path = 0;
                h.len = sizeof(m.m) + r + 1;

                s = f->serv;
                nai_fs_watch_serv_lock_map(&s->serv, 1);

                e = f->list.next;
                for ( ; e != &f->list; ) {
                    n = nai_containof(e, nai_fs_watch_node_t, entf);
                    e = e->next;

                    dir = nai_fs_watch_node_is_dir(n);
                    if (!dir && !nai_fs_watch_test(n, m.name, r)) {
                        continue;
                    };

                    h.e = &n->node;
                    h.token = n->node.token;
                    h.len = sizeof(m.m) + (dir ? (r + 1) : 0);
                    nai_evmsg_serv_sendto_locked(&s->serv, &h, &m);
                };

                nai_fs_watch_serv_unlock_map(&s->serv, 1);
            } else {
                h.len = sizeof(m.m);

                w = f->port;
                nai_list_init(&list);
                nai_list_add_tail(&list, &f->list);
                for ( ; !nai_list_is_empty(&list); ) {
                    e = list.next;
                    n = nai_containof(e, nai_fs_watch_node_t, entf);
                    nai_list_entry_remove(e);
                    nai_list_insert_tail(&f->list, e);

                    dir = nai_fs_watch_node_is_dir(n);
                    if (!dir && !nai_fs_watch_test(n, m.name, r)) {
                        continue;
                    };

                    m.m.path = dir ? m.name : 0;
                    h.e = &n->node;
                    h.token = n->node.token;
                    nai_evmsg_port_once_msg(&w->port, &h, &m.m);
                };
            };
        };

        if (data->NextEntryOffset == 0) {
            break;
        };
        data = (FILE_NOTIFY_INFORMATION*)(
            (uint8_t*)data + data->NextEntryOffset);
    };

    r = nai_fs_watch_read_dir(f);

_end:
    return r;
};


static nai_int_t nai_fs_watch_read_impl(nai_fs_watch_file_t* f, nai_int_t init)
{
    nai_int_t r;
    nai_int_t ec;
    nai_int_t mask;
    nai_fd_t fd;


    mask = FILE_NOTIFY_CHANGE_FILE_NAME | 
        FILE_NOTIFY_CHANGE_DIR_NAME | 
        FILE_NOTIFY_CHANGE_ATTRIBUTES | 
        FILE_NOTIFY_CHANGE_SIZE | 
        FILE_NOTIFY_CHANGE_LAST_WRITE | 
        FILE_NOTIFY_CHANGE_LAST_ACCESS | 
        FILE_NOTIFY_CHANGE_CREATION | 
        FILE_NOTIFY_CHANGE_SECURITY;

    if (init) {
        nai_aio_set_waitable(&f->aio, 1);
        r = nai_aio_set_event(&f->aio);
        nai_aio_set_waitable(&f->aio, 0);
        if (r < 0) {
            goto _end;
        };
    };

    fd = nai_evnode_get_fd(&f->ev);
    f->aio.fd = fd;
    r = ReadDirectoryChangesW(fd, 
        f->buf, (uint32_t)sizeof(f->buf), 0, mask, 0, &f->aio.cb, 0);
    if (r != 0) {
        f->pending = 1;
    } else {
        ec = GetLastError();
        switch (ec) {
        case ERROR_IO_PENDING:
            f->pending = 1;
            r = 0;
            break;
        default:
            nai_errno = nai_errno_from_oserr(ec);
            r = -1;
            break;
        };
    };

    if (r >= 0 && init) {
        nai_aio_cancel(&f->aio);
        r = nai_aio_wait(&f->aio, -1);
        f->aio.cb.hEvent = 0;
        f->pending = 0;
    };

_end:
    return r;
};


static nai_int_t nai_fs_watch_read_init(nai_fs_watch_file_t* f)
{
    return nai_fs_watch_read_impl(f, 1);
};


static nai_int_t nai_fs_watch_read_dir(nai_fs_watch_file_t* f)
{
    return nai_fs_watch_read_impl(f, 0);
};


static nai_int_t nai_fs_watch_process_dir(nai_evnode_t* e, nai_int_t events)
{
    nai_int_t r;
    nai_int_t c;
    nai_fs_watch_file_t* f;


    f = nai_containof(e, nai_fs_watch_file_t, ev);
    if (f->closed) {
        events = nai_ev_notify_from(1);
    };

    if ((events & NAI_EV_MASK) == NAI_EV_NOTIFY) {
        c = nai_ev_notify_code(events);
        switch (c) {
        case 0: /* open */
            r = nai_fs_watch_read_dir(f);
            break;

        case 1: /* close */
            if (f->pending) {
                nai_aio_cancel(&f->aio);
            };

            f->closed = 1;
            nai_evnode_close(&f->ev);
            nai_file_close(nai_evnode_get_fd(&f->ev));

            if (f->pending == 0) {
                nai_free(f);
            };
            break;

        default:
            assert(0);
            break;
        };
    };

    r = 0;

    return r;
};


static nai_rbnode_t** nai_fs_watch_find_file(
    nai_rbtree_t* t, uint64_t ino, uint64_t dev, nai_rbnode_t** pparent)
{
    int64_t c;
    nai_rbnode_t** n = &nai_rbtree_root(t);
    nai_rbnode_t* parent = nai_rbtree_end(t);
    nai_fs_watch_file_t* f;


    while (*n) {
        parent = *n;
        f = (nai_fs_watch_file_t*)parent;

        c = f->ino - ino;
        if (c != 0) {
            if (c >= 0) {
                n = &parent->rb_left;
            } else {
                n = &parent->rb_right;
            };
            continue;
        };

        c = f->dev - dev;
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


static nai_fd_t nai_fs_watch_open_fd_impl(
    const char* path, nai_int_t flags, nai_stat_t* st)
{
    nai_int_t r;
    nai_int_t ec;
    nai_int_t oflags;
    nai_fd_t fd;


    oflags = NAI_O_RDONLY | NAI_O_ASYNCIO | NAI_O_DIRECTORY;
    if (flags & NAI_FS_WATCH_SYMLINK) {
        oflags |= NAI_O_SYMLINK;
    };

    fd = nai_file_open(path, oflags);
    if (fd == NAI_FD_INVALID) {
        goto _end;
    };

    r = nai_file_stat(fd, st, NAI_STAT_BASIC);
    if (r < 0) {
        ec = nai_errno;
        nai_file_close(fd);
        nai_errno = ec;
        fd = NAI_FD_INVALID;
    };

_end:
    return fd;
};


static nai_fd_t nai_fs_watch_open_fd(
    const char* path, nai_int_t flags, nai_stat_t* st, nai_int_t* dir)
{
    nai_fd_t fd;
    char* slash;
    char buf[NAI_PATH_MAX * 3];


    fd = nai_fs_watch_open_fd_impl(path, flags, st);
    if (fd == NAI_FD_INVALID) {
        if (!(flags & NAI_FS_WATCH_PATH)) {
            goto _end;
        };

        /* nothing */
        st->st_mode = 0;
    };

    *dir = !!(st->st_mode & NAI_S_IFDIR);
    if (!*dir) {
        nai_file_close(fd);

        slash = strrchr(path, nai_path_sep);
        nai_memcpy(buf, path, slash - path);
        buf[slash - path] = 0;

        fd = nai_fs_watch_open_fd_impl(buf, flags, st);
        if (fd == NAI_FD_INVALID) {
            goto _end;
        };
        if (!(st->st_mode & NAI_S_IFDIR)) {
            nai_file_close(fd);
            nai_errno = ENOENT;
            fd = NAI_FD_INVALID;
        };
    };

_end:
    return fd;
};


static nai_int_t nai_fs_watch_attach_file(
    nai_fs_watch_node_t* e, nai_fd_t fd, nai_stat_t* st, nai_int_t dir)
{
    nai_int_t r;
    nai_int_t ec;
    nai_fs_watch_file_t* f;
    nai_fs_watch_serv_t* s;
    nai_fs_watch_port_t* w;
    nai_fs_watch_node_t* g;
    nai_evloop_t* l;
    nai_rbtree_t* t;
    nai_rbnode_t** n;
    nai_rbnode_t* parent;


    w = (nai_fs_watch_port_t*)e->node.port;
    g = (nai_fs_watch_node_t*)e->node.glob;
    s = (nai_fs_watch_serv_t*)w->port.serv;
    t = w->osloop ? &s->fmap : &w->fmap;
    n = nai_fs_watch_find_file(t, st->st_ino, st->st_dev, &parent);
    if (n[0] != 0) {
        f = (nai_fs_watch_file_t*)n[0];
    } else {
        f = (nai_fs_watch_file_t*)nai_malloc(sizeof(*f));
        if (f == 0) {
            r = -1;
            goto _fail;
        };

        f->dev = st->st_dev;
        f->ino = st->st_ino;
        f->serv = s;
        f->port = w->osloop ? 0 : w;
        f->refs = 0;
        f->closed = 0;
        f->pending = 0;
        nai_aio_init(&f->aio);
        nai_aio_set_cb(&f->aio, nai_fs_watch_read_dir_complete);
        nai_evnode_init(&f->ev);
        nai_list_init(&f->list);

        if (w->osloop) {
            l = w->osloop->loop;
        } else {
            l = nai_evnode_get_loop(&w->port.ev);
        };

        nai_evnode_set_cb(&f->ev, nai_fs_watch_process_dir);
        nai_evnode_set_fd(&f->ev, fd, NAI_FD_TYPE_FILE);
        nai_evnode_set_event(&f->ev, NAI_EV_SET, NAI_EV_ASYNC);
        if (w->osloop) {
            nai_evnode_post(&f->ev, 0);
            r = nai_fs_watch_read_init(f);
            if (r < 0) {
                goto _fail;
            };
        };
 
        r = nai_evnode_open(&f->ev, l);
        if (r < 0) {
            goto _fail;
        };

        if (w->osloop == 0) {
            r = nai_fs_watch_read_dir(f);
            if (r < 0) {
                goto _fail;
            };
        };

        nai_rbtree_link(t, &f->ent, parent, n);
        nai_rbtree_color(t, &f->ent);
    };

    if (fd != nai_evnode_get_fd(&f->ev)) {
        nai_file_close(fd);
    };

    f->refs ++;
    if (w->osloop) {
        g->file = f;
        g->refs ++;
        g->dir = dir;
        e->node.share = 1;
        nai_list_insert_tail(&f->list, &g->entf);
    } else {
        e->file = f;
        e->dir = dir;
        nai_list_insert_tail(&f->list, &e->entf);
    };
    r = 0;

_end:
    return r;

_fail:
    ec = nai_errno;
    if (f != 0) {
        nai_evnode_close(&f->ev);
        nai_free(f);
    };
    nai_file_close(fd);
    nai_errno = ec;
    goto _end;
};


#define nai_fs_watch_cmp_file(f, st)                    \
    ((f)->ino == (st)->st_ino &&                        \
     (f)->dev == (st)->st_dev)                          \


static nai_int_t nai_fs_watch_close_file(
    nai_fs_watch_node_t* n, nai_fs_watch_node_t* c);
static nai_int_t nai_fs_watch_reopen_file(
    nai_fs_watch_node_t* n, nai_fd_t fd, nai_stat_t* st, nai_int_t dir)
{
    nai_int_t r;
    nai_int_t events;
    nai_fs_watch_node_t* g;


    g = (nai_fs_watch_node_t*)n->node.glob;
    if (fd == NAI_FD_INVALID) {
        if (n->node.share) {
            if (g->file) {
                nai_fs_watch_close_file(g, n);
                events = NAI_FS_MOVE_DEL;
            } else {
                events = 0;
            };
        } else {
            if (n->file) {
                nai_fs_watch_close_file(n, n);
                events = NAI_FS_MOVE_DEL;
            } else {
                events = 0;
            };
        };

        r = 0;

    } else {
        if (n->node.share) {
            if (g->file && !nai_fs_watch_cmp_file(g->file, st)) {
                nai_fs_watch_close_file(n, n);
                nai_fs_watch_close_file(g, n);
                r = nai_fs_watch_attach_file(n, fd, st, dir);
                events = NAI_FS_MOVE_ADD;
            } else if (g->file) {
                nai_file_close(fd);
                r = 0;
                events = 0;
            } else {
                nai_fs_watch_close_file(n, n);
                r = nai_fs_watch_attach_file(n, fd, st, dir);
                events = NAI_FS_MOVE_ADD;
            };
        } else {
            if (n->file && !nai_fs_watch_cmp_file(n->file, st)) {
                nai_fs_watch_close_file(n, n);
                r = nai_fs_watch_attach_file(n, fd, st, dir);
                events = NAI_FS_MOVE_ADD;
            } else if (n->file) {
                nai_file_close(fd);
                r = 0;
                events = 0;
            } else {
                r = nai_fs_watch_attach_file(n, fd, st, dir);
                events = NAI_FS_MOVE_ADD;
            };
        };
    };

    if (r < 0) {
        goto _end;
    };

    r = events;

_end:
    return r;
};


static nai_int_t nai_fs_watch_open_file(nai_fs_watch_node_t* e)
{
    nai_int_t r;
    nai_int_t dir;
    nai_fd_t fd;
    nai_fs_watch_port_t* w;
    nai_fs_watch_node_t* g;
    nai_stat_t st;
    char* path;


    path = nai_str(&e->key.path);
    fd = nai_fs_watch_open_fd(path, e->key.flags, &st, &dir);
    if (fd == NAI_FD_INVALID) {
        r = -1;
        goto _end;
    };

    w = (nai_fs_watch_port_t*)e->node.port;
    g = (nai_fs_watch_node_t*)e->node.glob;
    if (w->osloop && g->file) {
        r = nai_fs_watch_reopen_file(e, fd, &st, dir);
    } else {
        r = nai_fs_watch_attach_file(e, fd, &st, dir);
    };


_end:
    return r;
};


static nai_int_t nai_fs_watch_close_file(
    nai_fs_watch_node_t* n, nai_fs_watch_node_t* c)
{
    nai_int_t r;
    nai_fs_watch_file_t* f;
    nai_fs_watch_serv_t* s;
    nai_fs_watch_port_t* w;
    nai_fs_watch_node_t* g;
    nai_rbtree_t* t;


    /* detach */
    if (n->node.share) {
        n->node.share = 0;
        g = (nai_fs_watch_node_t*)n->node.glob;
        g->refs --;
        if (g->refs > 0) {
            r = 0;
            goto _end;
        };

        f = g->file;
        g->file = 0;
        nai_list_entry_remove(&g->entf);
    } else {
        f = n->file;
        n->file = 0;
        nai_list_entry_remove(&n->entf);
    };

    if (f == 0) {
        r = 0;
        goto _end;
    };

    /* release */
    f->refs --;
    if (f->refs <= 0) {
        w = (nai_fs_watch_port_t*)c->node.port;
        s = (nai_fs_watch_serv_t*)w->port.serv;
        t = w->osloop ? &s->fmap : &w->fmap;
        nai_rbtree_erase(t, &f->ent);
        if (w->osloop) {
            nai_evnode_post(&f->ev, 1);
        } else {
            nai_fs_watch_process_dir(&f->ev, nai_ev_notify_from(1));
        };
    };

    r = 0;

_end:
    return r;
};


static nai_int_t nai_fs_watch_port_init(nai_evmsg_port_t* p)
{
    nai_int_t r;
    nai_fs_watch_port_t* w;


    w = (nai_fs_watch_port_t*)p;
    w->cb = 0;
    w->osloop = 0;
    nai_list_init(&w->plist);
    nai_rbtree_init(&w->fmap);
    r = 0;

    return r;
};


static nai_int_t nai_fs_watch_port_read(nai_evmsg_port_t* p)
{
    assert(0);
    (void)p;
    return 0;
};


static nai_int_t nai_fs_watch_port_open(nai_evmsg_port_t* p, nai_evloop_t* l)
{
    nai_int_t r;
    nai_fs_watch_port_t* w;


    w = (nai_fs_watch_port_t*)p;
    if (nai_strcmp(nai_evloop_get_name(l), "iocp") != 0 && 
        nai_strcmp(nai_evloop_get_name(l), "wpoll") != 0) {
        w->osloop = nai_osloop_get();
        if (w->osloop == 0) {
            r = -1;
            goto _end;
        };
    };

    w->cb = nai_evnode_get_cb(&w->port.ev);
    nai_evnode_set_cb(&w->port.ev, nai_fs_watch_port_handle);
    r = 0;

_end:
    return r;
};


static nai_int_t nai_fs_watch_port_close(nai_evmsg_port_t* p)
{
    nai_int_t r;
    nai_fs_watch_port_t* w;


    w = (nai_fs_watch_port_t*)p;
    if (w->osloop != 0) {
        nai_osloop_release(w->osloop);
        w->osloop = 0;
    };

    r = 0;

    return r;
};


static nai_int_t nai_fs_watch_port_rearm(nai_evmsg_port_t* p)
{
    (void)p;
    return 0;
};


static nai_int_t nai_fs_watch_serv_node_open(nai_evmsg_node_t* e, void* key)
{
    nai_int_t r;
    nai_fs_watch_key_t* k;
    nai_fs_watch_node_t* g;


    k = (nai_fs_watch_key_t*)key;
    g = (nai_fs_watch_node_t*)e;

    g->refs = 0;
    g->file = 0;
    g->udir = 0;
    g->key.flags = k->flags;
    nai_fs_watch_str_ref(&g->key.path, &k->path);
    nai_list_init(&g->entp);
    nai_list_init(&g->entf);
    r = 0;

    return r;
};


static nai_int_t nai_fs_watch_serv_node_close(nai_evmsg_node_t* e)
{
    nai_int_t r;
    nai_fs_watch_node_t* g;


    g = (nai_fs_watch_node_t*)e;
    assert(g->file == 0);

    /* free string */
    nai_fs_watch_str_unref(&g->key.path);
    r = 0;

    return r;
};


static nai_int_t nai_fs_watch_port_node_moved(nai_evmsg_node_t* e)
{
    nai_int_t r;
    nai_int_t dir;
    nai_fd_t fd;
    nai_fs_watch_node_t* n;
    nai_fs_watch_serv_t* s;
    nai_stat_t st;
    char* path;


    n = (nai_fs_watch_node_t*)e;
    s = (nai_fs_watch_serv_t*)n->node.port->serv;

    /* open file */
    path = nai_str(&n->key.path);
    fd = nai_fs_watch_open_fd(path, n->key.flags, &st, &dir);

    /* attach file */
    nai_fs_watch_serv_lock_map(&s->serv, 1);
    r = nai_fs_watch_reopen_file(n, fd, &st, dir);
    nai_fs_watch_serv_unlock_map(&s->serv, 1);

    if (r < 0) {
        r = 0;
    };
    if (r != 0) {
        nai_fs_watch_poll(n, nai_fs_poll_from(r));
    };


    return r;
};


static nai_int_t nai_fs_watch_port_node_open(nai_evmsg_node_t* e, void* key)
{
    nai_int_t r;
    nai_int_t ec;
    nai_fs_watch_serv_t* s;
    nai_fs_watch_node_t* n;
    nai_fs_watch_node_t* g;


    (void)key;

    n = (nai_fs_watch_node_t*)e;
    g = (nai_fs_watch_node_t*)e->glob;

    /* init */
    n->refs = 0;
    n->file = 0;
    n->udir = 0;
    n->key.path = g->key.path;
    n->key.flags = g->key.flags;
    nai_list_init(&n->entp);
    nai_list_init(&n->entf);

    /* open upper watch */
    if (n->key.flags & NAI_FS_WATCH_PATH) {
        r = nai_fs_watch_open_udir(n);
        if (r < 0) {
            goto _end;
        };
    };

    s = (nai_fs_watch_serv_t*)n->node.port->serv;
    nai_fs_watch_serv_lock_map(&s->serv, 1);

    /* open file */
    r = nai_fs_watch_open_file(n);
    if (r < 0) {
        ec = nai_errno;
    };

    nai_fs_watch_serv_unlock_map(&s->serv, 1);

    if (r < 0) {
        if (!(n->key.flags & NAI_FS_WATCH_PATH)) {
            goto _fail;
        };

        nai_fs_watch_poll(n, NAI_FS_POLL_ADD);
    };

    r = 0;

_end:
    return r;

_fail:
    nai_fs_watch_close_udir(n);
    nai_errno = ec;
    goto _end;
};


static nai_int_t nai_fs_watch_port_node_close(nai_evmsg_node_t* e)
{
    nai_int_t r;
    nai_fs_watch_serv_t* s;
    nai_fs_watch_node_t* n;


    n = (nai_fs_watch_node_t*)e;
    if (n->file != 0 || n->node.share) {
        s = (nai_fs_watch_serv_t*)n->node.port->serv;
        nai_fs_watch_serv_lock_map(&s->serv, 1);
        nai_fs_watch_close_file(n, n);
        nai_fs_watch_serv_unlock_map(&s->serv, 1);
    } else {
        nai_fs_watch_poll(n, NAI_FS_POLL_DEL);
    };

    nai_fs_watch_port_node_clean(e);
    r = 0;

    return r;
};



#elif (NAI_FS_WATCH_USE_LINUX)


#include <sys/inotify.h>


static nai_rbnode_t* nai_fs_watch_lbound_wd(nai_rbtree_t* t, nai_int_t wd)
{
    nai_int_t c;
    nai_rbnode_t** n = (nai_rbnode_t**)&nai_rbtree_root(t);
    nai_rbnode_t* parent;
    nai_rbnode_t* l = nai_rbtree_end(t);
    nai_fs_watch_node_t* e;


    while (*n) {
        parent = *n;
        e = nai_containof(parent, nai_fs_watch_node_t, entw);
        c = e->wd - wd;
        if (c >= 0) {
            l = &e->entw;
            n = &parent->rb_left;
        } else {
            n = &parent->rb_right;
        };
    };

    return l;
};


static nai_int_t nai_fs_watch_insert_wd(
    nai_rbtree_t* t, nai_fs_watch_node_t* p)
{
    nai_int_t c;
    nai_rbnode_t** n = (nai_rbnode_t**)&nai_rbtree_root(t);
    nai_rbnode_t* parent = nai_rbtree_end(t);
    nai_fs_watch_node_t* e = 0;


    while (*n) {
        parent = *n;
        e = nai_containof(parent, nai_fs_watch_node_t, entw);
        c = p->wd - e->wd;
        if (c < 0) {
            n = &parent->rb_left;
        } else {
            n = &parent->rb_right;
        };
    };

    nai_rbtree_link(t, &p->entw, parent, n);
    nai_rbtree_color(t, &p->entw);

    return 0;
};


static nai_int_t nai_fs_watch_open_wd_impl(nai_fs_watch_node_t* n)
{
    nai_int_t r;
    nai_int_t mask;
    nai_fs_watch_port_t* w;
    nai_fs_watch_key_t* key;


    mask = IN_ATTRIB | IN_MODIFY | IN_DELETE_SELF;
    mask |= IN_CREATE | IN_DELETE | 
        IN_MOVE_SELF | IN_MOVED_FROM | IN_MOVED_TO;

    key = &n->key;
    if (key->flags & NAI_FS_WATCH_SYMLINK) {
        mask |= IN_DONT_FOLLOW;
    };

    /* add watch */
    w = (nai_fs_watch_port_t*)n->node.port;
    r = inotify_add_watch(w->fd, nai_str(&key->path), mask);

    return r;
};


static nai_int_t nai_fs_watch_open_wd(nai_fs_watch_node_t* n)
{
    nai_int_t r;
    nai_fs_watch_port_t* w;


    /* add watch */
    r = nai_fs_watch_open_wd_impl(n);
    if (r < 0) {
        goto _end;
    };

    /* insert into map */
    n->wd = r;
    w = (nai_fs_watch_port_t*)n->node.port;
    nai_fs_watch_insert_wd(&w->wmap, n);

_end:
    return r;
};


static nai_int_t nai_fs_watch_close_wd(nai_fs_watch_node_t* n)
{
    nai_int_t r;
    nai_rbnode_t* i;
    nai_fs_watch_port_t* w;
    nai_fs_watch_node_t* c;


    /* remove from map */
    w = (nai_fs_watch_port_t*)n->node.port;
    nai_rbtree_erase(&w->wmap, &n->entw);

    /* is last watch ? */
    i = nai_fs_watch_lbound_wd(&w->wmap, n->wd);
    c = nai_containof(i, nai_fs_watch_node_t, entw);
    if (i == nai_rbtree_end(&w->wmap) || c->wd != n->wd) {
        inotify_rm_watch(w->fd, n->wd);
    };

    n->wd = -1;
    r = 0;

    return r;
};


static nai_int_t nai_fs_watch_port_event(nai_evmsg_port_t* p)
{
    nai_int_t r;
    size_t len;
    nai_rbnode_t* e;
    nai_fs_watch_msg_t m;
    nai_fs_watch_msg_t mm;
    nai_fs_watch_port_t* w;
    nai_fs_watch_node_t* n;
    nai_pool_t* pool;
    struct inotify_event* i;
    uint8_t* b;
    void* data;

    struct nai_fs_watch_dest_s {
        struct nai_fs_watch_dest_s* next;
        nai_evmsg_hdr_t h;
    };
    struct nai_fs_watch_dest_s* start;
    struct nai_fs_watch_dest_s* list;
    struct nai_fs_watch_dest_s* dest;


    w = (nai_fs_watch_port_t*)p;
    pool = &w->port.pool;

    /* handling fs event */
    len = w->buflen;
    b = w->buf;
    for ( ; b < w->buf + len; b += sizeof(*i) + i->len) {
        i = (struct inotify_event*)b;

        m.path = i->len ? i->name : 0;
        if (i->mask & (IN_ATTRIB|IN_MODIFY)) {
            m.events = NAI_FS_CHANGE;
        } else if (i->mask & (IN_CREATE|IN_MOVED_TO)) {
            m.events = NAI_FS_MOVE_ADD;
        } else if (i->mask & (IN_DELETE|IN_MOVED_FROM)) {
            m.events = NAI_FS_MOVE_DEL;
        } else if (i->mask & (IN_DELETE_SELF|IN_MOVE_SELF)) {
            m.path = 0;
            m.events = NAI_FS_MOVE_DEL;
        } else {
            continue;
        };

        start = 0;
        e = nai_fs_watch_lbound_wd(&w->wmap, i->wd);
        for ( ; e != nai_rbtree_end(&w->wmap); ) {
            n = nai_containof(e, nai_fs_watch_node_t, entw);
            e = nai_rbtree_next(e);
            if (n->wd != i->wd) {
                break;
            };
            if ((n->key.flags & NAI_FS_WATCH_PATH) && 
                (m.events & NAI_FS_MOVE_DEL) && m.path == 0) {
                r = nai_fs_watch_port_node_moved(&n->node);
                if (r == 0) {
                    continue;
                };

                m.events = r;
            };

            if (!(n->key.flags & NAI_FS_WATCH_CHILD) && m.path) {
                if (!(m.events & (NAI_FS_MOVE_ADD | NAI_FS_MOVE_DEL))) {
                    continue;
                };

                r = 0;
            } else {
                r = 1;
            };

            dest = (struct nai_fs_watch_dest_s*)nai_palloc(pool, sizeof(*dest));
            if (dest == 0) {
                start = 0;
                break;
            };

            dest->next = 0;
            dest->h.e = &n->node;
            dest->h.token = n->node.token;
            dest->h.len = r;
            if (start == 0) {
                start = list = dest;
            } else {
                list->next = dest;
                list = dest;
            };
        };

        dest = start;
        for ( ; dest; dest = dest->next) {
            if (dest->h.len == 0) {
                mm.events = NAI_FS_CHANGE;
                mm.path = 0;
                data = &mm;
            } else {
                data = &m;
            };
            dest->h.len = sizeof(m);
            nai_evmsg_port_call_msg(p, &dest->h, data);
        };

        nai_pool_reset(pool);
    };

    w->buflen = 0;
    r = 0;

    return r;
};


static nai_int_t nai_fs_watch_port_read(nai_evmsg_port_t* p)
{
    intptr_t r;
    nai_fs_watch_port_t* w;


    w = (nai_fs_watch_port_t*)p;
    for (;;) {
        r = nai_file_read(w->fd, 
            w->buf + w->buflen, sizeof(w->buf) - w->buflen);
        if (r < 0) {
            break;
        };
        if (r == 0) {
            break;
        };

        w->buflen = r;
        nai_fs_watch_port_event(p);
    };

    return r;
};


static nai_int_t nai_fs_watch_port_init(nai_evmsg_port_t* p)
{
    nai_int_t r;
    nai_fs_watch_port_t* w;


    w = (nai_fs_watch_port_t*)p;
    w->cb = 0;
    w->fd = NAI_FD_INVALID;
    w->buflen = 0;
    nai_list_init(&w->plist);
    nai_rbtree_init(&w->wmap);
    r = 0;

    return r;
};


static nai_int_t nai_fs_watch_port_open(nai_evmsg_port_t* p, nai_evloop_t* l)
{
    nai_int_t r;
    nai_fd_t fd;
    nai_fs_watch_port_t* w;


    (void)l;

#if (NAI_HAVE_INOTIFY_INIT1)
    fd = inotify_init1(IN_NONBLOCK | IN_CLOEXEC);
    if (fd == NAI_FD_INVALID) {
        r = -1;
        goto _end;
    };
#else
    nai_int_t ec;


    fd = inotify_init();
    if (fd == NAI_FD_INVALID) {
        r = -1;
        goto _end;
    };

    r = nai_file_set_blocking(fd, 0);
    if (r < 0) {
        goto _fail;
    };

    r = nai_file_set_cloexec(fd, 1);
    if (r < 0) {
        goto _fail;
    };

#endif

    w = (nai_fs_watch_port_t*)p;
    w->fd = fd;
    w->cb = nai_evnode_get_cb(&w->port.ev);
    nai_evnode_set_fd(&w->port.ev, fd, NAI_FD_TYPE_PIPE);
    nai_evnode_set_event(&w->port.ev, NAI_EV_SET, NAI_EV_READ);
    nai_evnode_set_cb(&w->port.ev, nai_fs_watch_port_handle);
    r = 0;

_end:
    return r;

#if !(NAI_HAVE_INOTIFY_INIT1)
_fail:
    ec = nai_errno;
    nai_file_close(fd);
    nai_errno = ec;
    goto _end;
#endif
};


static nai_int_t nai_fs_watch_port_close(nai_evmsg_port_t* p)
{
    nai_int_t r;
    nai_fs_watch_port_t* w;


    w = (nai_fs_watch_port_t*)p;
    if (w->fd != NAI_FD_INVALID) {
        nai_file_close(w->fd);
        w->fd = NAI_FD_INVALID;
    };

    nai_evnode_set_cb(&w->port.ev, w->cb);
    nai_evnode_set_fd(&w->port.ev, NAI_FD_INVALID, 0);
    r = 0;

    return r;
};


static nai_int_t nai_fs_watch_port_rearm(nai_evmsg_port_t* p)
{
    nai_int_t r;
    nai_fs_watch_port_t* w;
    nai_fs_watch_node_t* e;
    nai_rbnode_t* n;


    nai_fs_watch_port_close(p);
    r = nai_fs_watch_port_open(p, 0);
    if (r < 0) {
        goto _end;
    };

    w = (nai_fs_watch_port_t*)p;
    nai_rbtree_init(&w->wmap);
    n = nai_rbtree_begin(&p->nmap);
    for ( ; n != nai_rbtree_end(&p->nmap); ) {
        e = nai_containof(n, nai_fs_watch_node_t, node.node);
        n = nai_rbtree_next(n);
        if (e->wd < 0) {
            continue;
        };

        e->wd = -1;
        r = nai_fs_watch_open_wd(e);
        if (r < 0) {
            if (!(e->key.flags & NAI_FS_WATCH_PATH)) {
                /* notify error */
                nai_fs_watch_port_node_error(&e->node, nai_errno);
            };

            nai_fs_watch_poll(e, NAI_FS_POLL_ADD);
        };
    };

_end:
    return r;
};


static nai_int_t nai_fs_watch_port_node_moved(nai_evmsg_node_t* e)
{
    nai_int_t r;
    nai_int_t events;
    nai_fs_watch_port_t* w;
    nai_fs_watch_node_t* n;


    n = (nai_fs_watch_node_t*)e;
    r = nai_fs_watch_open_wd_impl(n);
    if (r == n->wd) {
        events = 0;      /* no event */
    } else if (r < 0) {
        nai_fs_watch_close_wd(n);
        events = NAI_FS_MOVE_DEL;
    } else {
        n->wd = r;
        w = (nai_fs_watch_port_t*)n->node.port;
        nai_fs_watch_insert_wd(&w->wmap, n);
        events = NAI_FS_MOVE_ADD;
    };

    if (events) {
        nai_fs_watch_poll(n, nai_fs_poll_from(events));
    };

    r = events;

    return r;
};


static nai_int_t nai_fs_watch_port_node_open(nai_evmsg_node_t* e, void* key)
{
    nai_int_t r;
    nai_int_t ec;
    nai_fs_watch_key_t* k;
    nai_fs_watch_node_t* n;


    n = (nai_fs_watch_node_t*)e;
    k = (nai_fs_watch_key_t*)key;

    n->wd = -1;
    n->udir = 0;
    n->key.flags = k->flags;
    nai_fs_watch_str_ref(&n->key.path, &k->path);
    nai_list_init(&n->entp);

    /* open upper watch */
    if (n->key.flags & NAI_FS_WATCH_PATH) {
        r = nai_fs_watch_open_udir(n);
        if (r < 0) {
            goto _fail;
        };
    };

    /* open file */
    r = nai_fs_watch_open_wd(n);
    if (r < 0) {
        if (!(n->key.flags & NAI_FS_WATCH_PATH)) {
            goto _fail;
        };

        nai_fs_watch_poll(n, NAI_FS_POLL_ADD);
    };

    r = 0;

_end:
    return r;

_fail:
    ec = nai_errno;
    nai_fs_watch_close_udir(n);
    nai_fs_watch_str_unref(&n->key.path);
    nai_errno = ec;
    goto _end;
};


static nai_int_t nai_fs_watch_port_node_close(nai_evmsg_node_t* e)
{
    nai_int_t r;
    nai_fs_watch_node_t* n;


    n = (nai_fs_watch_node_t*)e;
    if (n->wd >= 0) {
        nai_fs_watch_close_wd(n);
    } else {
        nai_fs_watch_poll(n, NAI_FS_POLL_DEL);
    };

    /* free string */
    nai_fs_watch_str_unref(&n->key.path);
    nai_fs_watch_port_node_clean(e);
    r = 0;

    return r;
};



#elif (NAI_FS_WATCH_USE_KQUEUE)


#include "nai/event/nai_evloop.h"
#include "nai/os/nai_stat.h"


static nai_int_t nai_fs_watch_process_file(nai_evnode_t* e, nai_int_t events)
{
    nai_int_t r;
    nai_fs_watch_file_t* f;
    nai_fs_watch_node_t* n;
    nai_fs_watch_port_t* w;
    nai_fs_watch_msg_t m;
    nai_evmsg_hdr_t h;
    nai_list_t list;
    nai_list_entry_t* i;


    m.path = 0;
    m.events = nai_ev_error_code(events); /* see nai_kqueue_wait */
    if (m.events == 0) {
        r = 0;
    } else {
        f = nai_containof(e, nai_fs_watch_file_t, ev);
        w = f->port;
        nai_list_init(&list);
        nai_list_add_tail(&list, &f->list);
        for ( ; !nai_list_is_empty(&list); ) {
            i = list.next;
            n = nai_containof(i, nai_fs_watch_node_t, entf);
            nai_list_entry_remove(i);
            nai_list_insert_tail(&f->list, i);

            m.events = events >> 8;
            if ((n->key.flags & NAI_FS_WATCH_PATH) && 
                (m.events & NAI_FS_MOVE_DEL) && m.path == 0) {
                r = nai_fs_watch_port_node_moved(&n->node);
                if (r == 0) {
                    m.events &= ~NAI_FS_MOVE_DEL;
                    if (m.events == 0) {
                        continue;
                    };
                };

                m.events &= ~NAI_FS_MOVE_DEL;
                m.events |= r;
            };

            h.e = &n->node;
            h.token = n->node.token;
            h.len = sizeof(m);
            nai_evmsg_port_call_msg(&w->port, &h, &m);
        };

        r = 0;
    };

    return r;
};


static nai_int_t nai_fs_watch_add_kevent(
    nai_fs_watch_port_t* w, nai_fs_watch_file_t* f)
{
    nai_int_t r;
    nai_int_t filter;
    nai_int_t fflags;
    nai_fd_t fd;
    struct kevent ev;


    fd = f->fd->fd;
    filter = EVFILT_VNODE;
    fflags = NOTE_ATTRIB | NOTE_WRITE | NOTE_EXTEND | NOTE_REVOKE | 
        NOTE_RENAME | NOTE_DELETE;

    EV_SET(&ev, fd, filter, EV_ADD|EV_CLEAR, 
        fflags, 0, f->ev.ent ? (void*)f->ev.ent : (void*)f);
    r = kevent(w->fd, &ev, 1, 0, 0, 0);

    return r;
};


static nai_int_t nai_fs_watch_del_kevent(
    nai_fs_watch_port_t* w, nai_fs_watch_file_t* f)
{
    nai_int_t r;
    nai_int_t filter;
    nai_int_t fflags;
    nai_fd_t fd;
    struct kevent ev;


    fd = f->fd->fd;
    filter = EVFILT_VNODE;
    fflags = NOTE_ATTRIB | NOTE_WRITE | NOTE_EXTEND | NOTE_REVOKE | 
        NOTE_RENAME | NOTE_DELETE;

    EV_SET(&ev, fd, filter, EV_DELETE, fflags, 0, 0);
    r = kevent(w->fd, &ev, 1, 0, 0, 0);
    if (r < 0) {
        nai_log_debug(NAI_LOG_CORE, 
            nai_errno, "kevent(%d, %d) failed\n", w->fd, fd);
        assert(0);
    };

    return r;
};


static nai_rbnode_t** nai_fs_watch_find_file(
    nai_rbtree_t* t, uint64_t ino, uint64_t dev, nai_rbnode_t** pparent)
{
    int64_t c;
    nai_rbnode_t** n = &nai_rbtree_root(t);
    nai_rbnode_t* parent = nai_rbtree_end(t);
    nai_fs_watch_file_t* f;


    while (*n) {
        parent = *n;
        f = (nai_fs_watch_file_t*)parent;

        c = f->ino - ino;
        if (c != 0) {
            if (c >= 0) {
                n = &parent->rb_left;
            } else {
                n = &parent->rb_right;
            };
            continue;
        };

        c = f->dev - dev;
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


static nai_int_t nai_fs_watch_open_file(nai_fs_watch_node_t* e)
{
    nai_int_t r;
    nai_int_t ec;
    nai_int_t oflags;
    nai_fd_t fd;
    nai_fs_watch_fd_t* d;
    nai_fs_watch_file_t* f;
    nai_fs_watch_serv_t* s;
    nai_fs_watch_port_t* w;
    nai_stat_t st;
    nai_rbnode_t** nd;
    nai_rbnode_t** nf;
    nai_rbnode_t* parentd;
    nai_rbnode_t* parentf;


    oflags = NAI_O_RDONLY;
    if (e->key.flags & NAI_FS_WATCH_SYMLINK) {
        oflags |= NAI_O_SYMLINK;
    };

    fd = nai_file_open(nai_str(&e->key.path), oflags);
    if (fd == NAI_FD_INVALID) {
        r = -1;
        goto _end;
    };

    f = 0;
    d = 0;
    r = nai_file_stat(fd, &st, NAI_STAT_BASIC);
    if (r < 0) {
        goto _fail;
    };

    /* lookup file watch */
    w = (nai_fs_watch_port_t*)e->node.port;
    nf = nai_fs_watch_find_file(&w->fmap, st.st_ino, st.st_dev, &parentf);
    if (nf[0] != 0) {
        f = (nai_fs_watch_file_t*)nf[0];
    } else {
        /* lookup file descriptor */
        s = (nai_fs_watch_serv_t*)w->port.serv;
        nd = nai_fs_watch_find_file(&s->fmap, st.st_ino, st.st_dev, &parentd);
        if (nd[0] != 0) {
            d = (nai_fs_watch_fd_t*)nd[0];
        } else {
            /* create file descriptor */
            d = (nai_fs_watch_fd_t*)nai_fixedpool_alloc(&s->pool);
            if (d == 0) {
                r = -1;
                goto _fail;
            };

            d->ino = st.st_ino;
            d->dev = st.st_dev;
            d->fd = fd;
            d->refs = 0;
        };

        /* create file watch */
        f = (nai_fs_watch_file_t*)nai_fixedpool_alloc(&s->pool);
        if (f == 0) {
            r = -1;
            goto _fail;
        };

        f->ino = st.st_ino;
        f->dev = st.st_dev;
        f->fd = d;
        f->port = w;
        f->refs = 0;
        nai_list_init(&f->list);
        nai_evnode_init(&f->ev);

        if (nai_evnode_get_fd(&w->port.ev) == NAI_FD_INVALID) {
            nai_evnode_set_cb(&f->ev, nai_fs_watch_process_file);
            r = nai_evnode_open(&f->ev, nai_evnode_get_loop(&w->port.ev));
            if (r < 0) {
                goto _fail;
            };
        };

        r = nai_fs_watch_add_kevent(w, f);
        if (r < 0) {
            goto _fail;
        };

        /* insert into maps */
        d->refs ++;
        nai_rbtree_link(&w->fmap, &f->ent, parentf, nf);
        nai_rbtree_color(&w->fmap, &f->ent);

        if (nd[0] == 0) {
            nai_rbtree_link(&s->fmap, &d->ent, parentd, nd);
            nai_rbtree_color(&s->fmap, &d->ent);
        };
    };

    if (fd != f->fd->fd) {
        nai_file_close(fd);
    };

    f->refs ++;
    e->file = f;
    nai_list_insert_tail(&f->list, &e->entf);
    r = 0;

_end:
    return r;

_fail:
    ec = nai_errno;
    if (d != 0 && nd[0] == 0) {
        nai_fixedpool_free(&s->pool, d);
    };
    if (f != 0 && nf[0] == 0) {
        nai_fixedpool_free(&s->pool, f);
    };
    nai_file_close(fd);
    nai_errno = ec;
    goto _end;
};


static nai_int_t nai_fs_watch_close_file(nai_fs_watch_node_t* n)
{
    nai_int_t r;
    nai_fs_watch_fd_t* d;
    nai_fs_watch_file_t* f;
    nai_fs_watch_serv_t* s;
    nai_fs_watch_port_t* w;


    /* detach */
    f = n->file;
    if (f != 0) {
        n->file = 0;
        nai_list_entry_remove(&n->entf);

        /* release */
        f->refs --;
        if (f->refs <= 0) {
            d = f->fd;
            w = (nai_fs_watch_port_t*)n->node.port;
            s = (nai_fs_watch_serv_t*)w->port.serv;
            nai_evnode_close(&f->ev);
            nai_fs_watch_del_kevent(w, f);
            nai_rbtree_erase(&w->fmap, &f->ent);
            nai_fixedpool_free(&s->pool, f);

            d->refs --;
            if (d->refs <= 0) {
                nai_file_close(d->fd);
                nai_rbtree_erase(&s->fmap, &d->ent);
                nai_fixedpool_free(&s->pool, d);
            };
        };
    };

    r = 0;

    return r;
};


static nai_int_t nai_fs_watch_port_event(nai_evmsg_port_t* p)
{
    nai_int_t r;
    nai_int_t i;
    nai_int_t fflags;
    nai_fs_watch_msg_t m;
    nai_fs_watch_port_t* w;
    nai_fs_watch_file_t* f;


    w = (nai_fs_watch_port_t*)p;

    /* handling fs events */
    for (i = 0; i < w->nevent; i ++) {
        if (w->ea[i].filter != EVFILT_VNODE) {
            continue;
        };

        fflags = w->ea[i].fflags;
        m.path = 0;
        m.events = 0;
        if (fflags & (NOTE_ATTRIB | NOTE_EXTEND | 
            NOTE_REVOKE | NOTE_WRITE)) {
            m.events |= NAI_FS_CHANGE;
        };
        if (fflags & (NOTE_RENAME | NOTE_DELETE)) {
            m.events |= NAI_FS_MOVE_DEL;
        };
        if (m.events == 0) {
            assert(0);
            continue;
        };

        f = (nai_fs_watch_file_t*)w->ea[i].udata;
        nai_fs_watch_process_file(&f->ev, m.events << 8);
    };

    w->nevent = 0;
    r = 0;

    return r;
};


static nai_int_t nai_fs_watch_port_read(nai_evmsg_port_t* p)
{
    intptr_t r;
    nai_fs_watch_port_t* w;
    struct timespec ts, *tp;


    ts.tv_sec = 0;
    ts.tv_nsec = 0;
    tp = &ts;

    w = (nai_fs_watch_port_t*)p;
    for (;;) {
        r = kevent(w->fd, 0, 0, w->ea, nai_countof(w->ea), tp);
        if (r < 0) {
            break;
        };
        if (r == 0) {
            break;
        };

        w->nevent = r;
        nai_fs_watch_port_event(p);
    };

    return r;
};


static nai_int_t nai_fs_watch_port_init(nai_evmsg_port_t* p)
{
    nai_int_t r;
    nai_fs_watch_port_t* w;


    w = (nai_fs_watch_port_t*)p;
    w->cb = 0;
    w->fd = NAI_FD_INVALID;
    w->pid = -1;
    w->nevent = 0;
    nai_list_init(&w->plist);
    nai_rbtree_init(&w->fmap);
    r = 0;

    return r;
};


static nai_int_t nai_fs_watch_port_open(nai_evmsg_port_t* p, nai_evloop_t* l)
{
    nai_int_t r;
    nai_int_t ec;
    nai_fd_t fd;
    nai_fs_watch_port_t* w;


    w = (nai_fs_watch_port_t*)p;
    if (nai_strcmp(nai_evloop_get_name(l), "kqueue") == 0) {
        fd = nai_evbase_get_fd(&l->ev);
    } else {
        /* create kqueue */
        fd = kqueue();
        if (fd == NAI_FD_INVALID) {
            r = -1;
            goto _end;
        };

        r = nai_file_set_cloexec(fd, 1);
        if (r < 0) {
            ec = nai_errno;
            nai_file_close(fd);
            nai_errno = ec;
            goto _end;
        };

        nai_evnode_set_fd(&w->port.ev, fd, NAI_FD_TYPE_PIPE);
        nai_evnode_set_event(&w->port.ev, NAI_EV_SET, NAI_EV_READ);
    };

#if (__darwin__)
    w->osloop = nai_osloop_get();
#endif

    w->pid = nai_get_pid();
    w->fd = fd;
    w->cb = nai_evnode_get_cb(&w->port.ev);
    nai_evnode_set_cb(&w->port.ev, nai_fs_watch_port_handle);
    r = 0;

_end:
    return r;
};


static nai_int_t nai_fs_watch_port_close(nai_evmsg_port_t* p)
{
    nai_int_t r;
    nai_fs_watch_port_t* w;
    nai_evloop_t* l;


    w = (nai_fs_watch_port_t*)p;
    l = nai_evnode_get_loop(&w->port.ev);
    if (l != 0) {
        if (nai_strcmp(nai_evloop_get_name(l), "kqueue") != 0) {
            if (w->pid == nai_get_pid()) {
                nai_file_close(w->fd);
            };
        };
    } else {
        if (w->fd != NAI_FD_INVALID && 
            w->fd == nai_evnode_get_fd(&w->port.ev)) {
            if (w->pid == nai_get_pid()) {
                nai_file_close(w->fd);
            };
        };
    };

#if (__darwin__)
    if (w->osloop) {
        nai_osloop_release(w->osloop);
        w->osloop = 0;
    };
#endif

    nai_evnode_set_cb(&w->port.ev, w->cb);
    nai_evnode_set_fd(&w->port.ev, NAI_FD_INVALID, 0);
    w->fd = NAI_FD_INVALID;
    w->pid = -1;
    r = 0;

    return r;
};


static nai_int_t nai_fs_watch_port_rearm(nai_evmsg_port_t* p)
{
    nai_int_t r;
    nai_fs_watch_port_t* w;
    nai_fs_watch_file_t* f;
    nai_fs_watch_node_t* e;
    nai_evloop_t* l;
    nai_rbnode_t* n;
    nai_list_entry_t* i;


    nai_fs_watch_port_close(p);
    w = (nai_fs_watch_port_t*)p;
    l = nai_evnode_get_loop(&w->port.ev);
    r = nai_fs_watch_port_open(p, l);
    if (r < 0) {
        goto _end;
    };

    n = nai_rbtree_begin(&w->fmap);
    for ( ; n != nai_rbtree_end(&w->fmap); ) {
        f = nai_containof(n, nai_fs_watch_file_t, ent);
        n = nai_rbtree_next(n);
        r = nai_fs_watch_add_kevent(w, f);
        if (r < 0) {
            /* notify error */
            i = f->list.next;
            for ( ; i != &f->list; ) {
                e = nai_containof(i, nai_fs_watch_node_t, entf);
                i = i->next;
                nai_fs_watch_port_node_error(&e->node, nai_errno);
            };
        };
    };

    r = 0;

_end:
    return r;
};


static nai_int_t nai_fs_watch_serv_node_open(nai_evmsg_node_t* e, void* key)
{
    nai_int_t r;
    nai_fs_watch_key_t* k;
    nai_fs_watch_node_t* g;


    k = (nai_fs_watch_key_t*)key;
    g = (nai_fs_watch_node_t*)e;

    g->udir = 0;
    g->file = 0;
    g->key.flags = k->flags;
    nai_fs_watch_str_ref(&g->key.path, &k->path);
    nai_list_init(&g->entp);
    nai_list_init(&g->entf);
    r = 0;

    return r;
};


static nai_int_t nai_fs_watch_serv_node_close(nai_evmsg_node_t* e)
{
    nai_int_t r;
    nai_fs_watch_node_t* g;


    g = (nai_fs_watch_node_t*)e;
    assert(g->file == 0);

    /* free string */
    nai_fs_watch_str_unref(&g->key.path);
    r = 0;

    return r;
};


static nai_int_t nai_fs_watch_port_node_moved(nai_evmsg_node_t* e)
{
    nai_int_t r;
    nai_int_t flags;
    nai_int_t events;
    nai_fs_watch_serv_t* s;
    nai_fs_watch_node_t* n;
    nai_stat_t st;


    n = (nai_fs_watch_node_t*)e;
    flags = 0;
    if (n->key.flags & NAI_FS_WATCH_SYMLINK) {
        flags = NAI_STAT_SYMLINK;
    };

    s = (nai_fs_watch_serv_t*)n->node.port->serv;
    r = nai_stat(nai_str(&n->key.path), &st, flags);
    if (r < 0) {
        if (n->file != 0) {
            nai_fs_watch_serv_lock_map(&s->serv, 1);
            nai_fs_watch_close_file(n);
            nai_fs_watch_serv_unlock_map(&s->serv, 1);
            events = NAI_FS_MOVE_DEL;
        } else {
            events = 0;
        };
    } else {
        if (n->file != 0 && (
            st.st_ino == n->file->ino || 
            st.st_dev == n->file->dev)) {
            events = 0;
        } else {
            events = n->file ? NAI_FS_MOVE_DEL : 0;
            nai_fs_watch_serv_lock_map(&s->serv, 1);
            nai_fs_watch_close_file(n);
            r = nai_fs_watch_open_file(n);
            if (r >= 0) {
                events = NAI_FS_MOVE_ADD;
            };
            nai_fs_watch_serv_unlock_map(&s->serv, 1);
        };
    };

    if (events) {
        nai_fs_watch_poll(n, nai_fs_poll_from(events));
    };

    r = events;

    return r;
};


static nai_int_t nai_fs_watch_port_node_open(nai_evmsg_node_t* e, void* key)
{
    nai_int_t r;
    nai_int_t ec;
    nai_fs_watch_serv_t* s;
    nai_fs_watch_node_t* n;
    nai_fs_watch_node_t* g;


    (void)key;

    n = (nai_fs_watch_node_t*)e;
    g = (nai_fs_watch_node_t*)e->glob;

    /* init */
    n->udir = 0;
    n->file = 0;
    n->key.path = g->key.path;
    n->key.flags = g->key.flags;
    nai_list_init(&n->entp);
    nai_list_init(&n->entf);

    /* open upper watch */
    if (n->key.flags & NAI_FS_WATCH_PATH) {
        r = nai_fs_watch_open_udir(n);
        if (r < 0) {
            goto _end;
        };
    };

    s = (nai_fs_watch_serv_t*)n->node.port->serv;
    nai_fs_watch_serv_lock_map(&s->serv, 1);

    /* open file */
    r = nai_fs_watch_open_file(n);
    if (r < 0) {
        ec = nai_errno;
    };

    nai_fs_watch_serv_unlock_map(&s->serv, 1);

    if (r < 0) {
        if (!(n->key.flags & NAI_FS_WATCH_PATH)) {
            goto _fail;
        };

        nai_fs_watch_poll(n, NAI_FS_POLL_ADD);
    };

    r = 0;

_end:
    return r;

_fail:
    nai_fs_watch_close_udir(n);
    nai_errno = ec;
    goto _end;
};


static nai_int_t nai_fs_watch_port_node_close(nai_evmsg_node_t* e)
{
    nai_int_t r;
    nai_fs_watch_serv_t* s;
    nai_fs_watch_node_t* n;


    n = (nai_fs_watch_node_t*)e;
    if (n->file != 0) {
        s = (nai_fs_watch_serv_t*)n->node.port->serv;
        nai_fs_watch_serv_lock_map(&s->serv, 1);
        nai_fs_watch_close_file(n);
        nai_fs_watch_serv_unlock_map(&s->serv, 1);
    } else {
        nai_fs_watch_poll(n, NAI_FS_POLL_DEL);
    };

    nai_fs_watch_port_node_clean(e);
    r = 0;

    return r;
};


#elif (NAI_FS_WATCH_USE_POLL)



static nai_rbnode_t** nai_fs_watch_find_file(
    nai_rbtree_t* t, nai_fs_watch_fd_t* k, nai_rbnode_t** pparent)
{
    int64_t c;
    nai_rbnode_t** n = &nai_rbtree_root(t);
    nai_rbnode_t* parent = nai_rbtree_end(t);
    nai_fs_watch_file_t* f;


    while (*n) {
        parent = *n;
        f = (nai_fs_watch_file_t*)parent;

        if ((f->fd.flags & NAI_FS_WATCH_PATH) == 0) {
            c = f->fd.ino - k->ino;
            if (c != 0) {
                if (c >= 0) {
                    n = &parent->rb_left;
                } else {
                    n = &parent->rb_right;
                };
                continue;
            };

            c = f->fd.dev - k->dev;
            if (c != 0) {
                if (c >= 0) {
                    n = &parent->rb_left;
                } else {
                    n = &parent->rb_right;
                };
                continue;
            };
        };

        c = f->fd.flags - k->flags;
        if (c != 0) {
            if (c >= 0) {
                n = &parent->rb_left;
            } else {
                n = &parent->rb_right;
            };
            continue;
        };

#if (NAI_PATH_CASE_SENSITIVE)
        c = nai_str_cmp(&f->fd.path, &k->path);
#else
        c = nai_str_casecmp(&f->fd.path, &k->path);
#endif
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


static nai_int_t nai_fs_watch_free_file(
    nai_fs_watch_serv_t* s, nai_fs_watch_file_t* f)
{
    nai_int_t r;


    if ((f->fd.flags & NAI_FS_WATCH_PATH) == 0) {
        nai_file_close(f->fd.fd);
    };
    nai_fs_watch_str_unref(&f->fd.path);
    nai_rbtree_erase(&s->fmap, &f->ent);
    nai_list_entry_remove(&f->entn);
    nai_fixedpool_free(&s->pool, f);

    r = 0;

    return r;
};


static nai_int_t nai_fs_watch_cmp_stat(nai_stat_t* old, nai_stat_t* new)
{
    nai_int_t r;


    if (old->st_ino == new->st_ino && 
        old->st_dev == new->st_dev) {
        if (old->st_mode == new->st_mode && 
            old->st_rdev == new->st_rdev && 
            old->st_gid == new->st_gid && 
            old->st_uid == new->st_uid && 
            old->st_size == new->st_size && 
            old->st_blocks == new->st_blocks && 
            old->st_blksize == new->st_blksize && 
            old->st_ctim.tv_sec == new->st_ctim.tv_sec && 
            old->st_mtim.tv_sec == new->st_mtim.tv_sec && 
            old->st_ctim.tv_nsec == new->st_ctim.tv_nsec && 
            old->st_mtim.tv_nsec == new->st_mtim.tv_nsec) {
            r = 0;
        } else {
            r = NAI_FS_CHANGE;
        };
    } else if (new->st_ino == 0 && new->st_dev == 0) {
        r = NAI_FS_MOVE_DEL;
    } else if (old->st_ino == 0 && old->st_dev == 0) {
        r = NAI_FS_MOVE_ADD;
    } else {
        r = NAI_FS_MOVE_ADD;
    };

    return r;
};


static nai_int_t nai_fs_watch_poll_interval(nai_fs_watch_serv_t* s)
{
    nai_int_t r;

    r = NAI_FS_POLL_INTERVAL >> s->rate;
    if (r < 30) {
        if (30 > NAI_FS_POLL_INTERVAL) {
            r = NAI_FS_POLL_INTERVAL;
        } else {
            r = 30;
        };
    };

    return r;
};


static nai_int_t nai_fs_watch_serv_handle(nai_evnode_t* e, nai_int_t events)
{
    nai_int_t r;
    nai_int_t ec;
    nai_int_t flags;
    nai_int_t change;
    nai_list_entry_t* l;
    nai_list_entry_t* i;
    nai_fs_watch_node_t* n;
    nai_fs_watch_file_t* f;
    nai_fs_watch_serv_t* s;
    nai_fs_watch_msg_t m;
    nai_evmsg_hdr_t h;
    nai_stat_t st;


    s = nai_containof(e, nai_fs_watch_serv_t, timer);
    switch (events & NAI_EV_MASK) {
    case NAI_EV_TIMEOUT:
        nai_fs_watch_serv_lock_map(&s->serv, 1);

        /* move tasks from the pending list to the task list */
        for ( ; !nai_list_is_empty(&s->pendings); ) {
            l = s->pendings.next;
            f = nai_containof(l, nai_fs_watch_file_t, entn);
            f->refs ++;
            nai_list_entry_remove(l);
            nai_list_insert_tail(&s->tasks, l);
        };

        nai_fs_watch_serv_unlock_map(&s->serv, 1);


        /* scan the task list */
        l = s->tasks.next;
        for ( ; l != &s->tasks; ) {
            f = nai_containof(l, nai_fs_watch_file_t, entn);
            l = l->next;
            if (f->refs == 1) {
                f->events = 0;
                continue;
            };

            flags = 0;
            if (f->fd.flags & NAI_FS_WATCH_SYMLINK) {
                flags |= NAI_STAT_SYMLINK;
            };
            r = nai_stat(nai_str(&f->fd.path), &st, flags);
            if (r < 0) {
                ec = nai_errno;
                if (ec == ENOENT) {
                    r = 0;
                } else {
                    if (f->fd.flags & NAI_FS_WATCH_PATH) {
                        f->events = 0;
                        continue;
                    };
                };

                nai_memset(&st, 0, sizeof(st));
            };

            f->events = nai_fs_watch_cmp_stat(&f->stp, &st);
            if (f->fd.flags & NAI_FS_WATCH_PATH) {
                f->stp = st;
                continue;
            };

            if (f->stf.st_dev == st.st_dev && 
                f->stf.st_ino == st.st_ino) {
                f->events &= ~NAI_FS_MOVE_DEL;
                f->stp = st;
            } else {
                if (r < 0) {
                    f->events = 0;
                } else if (f->stf.st_dev == f->stp.st_dev && 
                    f->stf.st_ino == f->stp.st_ino) {
                    f->events = NAI_FS_MOVE_DEL;
                    f->stp = st;
                } else {
                    f->events = 0;
                    f->stp = st;
                };

                r = nai_file_stat(f->fd.fd, &st, 0);
                if (r < 0) {
                    continue;
                };
            };

            f->events |= nai_fs_watch_cmp_stat(&f->stf, &st);
            f->stf = st;
        };


        nai_fs_watch_serv_lock_map(&s->serv, 1);

        /* send results */
        change = 0;
        l = s->tasks.next;
        for ( ; l != &s->tasks; ) {
            f = nai_containof(l, nai_fs_watch_file_t, entn);
            l = l->next;
            if (f->refs == 1) {
                nai_fs_watch_free_file(s, f);
                continue;
            };
            if (f->events == 0) {
                continue;
            };

            /* send message */
            m.path = 0;
            m.events = f->events;
            h.len = sizeof(m);
            i = f->list.next;
            for ( ; i != &f->list; ) {
                n = nai_containof(i, nai_fs_watch_node_t, entf);
                i = i->next;
                h.e = &n->node;
                h.token = n->node.token;
                nai_evmsg_port_sendto(n->node.port, &h, &m);
            };

            change = 1;
        };

        if (change) {
            s->rate = 4;
        } else {
            s->rate --;
            if (s->rate < 0) {
                s->rate = 0;
            };
        };;

        if (!nai_list_is_empty(&s->tasks) || 
            !nai_list_is_empty(&s->pendings)) {
            nai_evnode_set_timeout(&s->timer, 
                NAI_TIMEOP_ADD, nai_fs_watch_poll_interval(s));
        };

        nai_fs_watch_serv_unlock_map(&s->serv, 1);
        break;

    case NAI_EV_NOTIFY:
        nai_fs_watch_serv_handle(e, NAI_EV_TIMEOUT);
        nai_evnode_close(e);
        break;

    default:
        break;
    };

    r = 0;

    return r;
};


static nai_int_t nai_fs_watch_serv_active(nai_fs_watch_serv_t* s)
{
    nai_int_t r;
    nai_int_t ec;
    nai_osloop_t* l;
    nai_osloop_t* t;


    nai_osloop_lock();

    /* fetch loop */
    for (;;) {
        l = s->osloop;
        if (l != 0 && !l->term) {
            break;
        };

        nai_osloop_unlock();

        l = nai_osloop_get();
        if (l == 0) {
            r = -1;
            goto _end;
        };

        nai_osloop_lock();

        if (s->osloop != l) {
            /* always close */
            nai_evnode_close(&s->timer);

            /* open with new event loop */
            s->rate = 4;
            nai_evnode_set_cb(&s->timer, nai_fs_watch_serv_handle);
            nai_evnode_set_timeout(&s->timer, 
                NAI_TIMEOP_SET, nai_fs_watch_poll_interval(s));
            r = nai_evnode_open(&s->timer, l->loop);
            if (r < 0) {
                ec = nai_errno;
            } else {
                t = s->osloop;
                s->osloop = l;
                l = t;
            };
        } else {
            r = 0;
        };

        if (l != 0) {
            nai_osloop_unlock();
            nai_osloop_release(l);

            if (r < 0) {
                nai_errno = ec;
                goto _end;
            };

            nai_osloop_lock();
        };
    };

    nai_evnode_set_timeout(&s->timer, 
        NAI_TIMEOP_MIN, nai_fs_watch_poll_interval(s));

    nai_osloop_unlock();

    r = 0;

_end:
    return r;
};


static nai_int_t nai_fs_watch_serv_attach(nai_fs_watch_serv_t* s)
{
    nai_int_t r;


    nai_osloop_lock();

    s->refs ++;

    nai_osloop_unlock();

    r = 0;

    return r;
};


static nai_int_t nai_fs_watch_serv_detach(nai_fs_watch_serv_t* s)
{
    nai_int_t r;
    nai_osloop_t* l = 0;


    nai_osloop_lock();

    s->refs --;
    if (s->refs <= 0 && s->osloop) {
        l = s->osloop;
        s->osloop = 0;
        if (l->term) {
            nai_evnode_close(&s->timer);
        } else {
            nai_evnode_post(&s->timer, 0);
        };
    };

    nai_osloop_unlock();

    if (l != 0) {
        nai_osloop_release(l);
    };

    r = 0;

    return r;
};


static nai_int_t nai_fs_watch_open_file(nai_fs_watch_node_t* n)
{
    nai_int_t r;
    nai_int_t ec;
    nai_int_t oflags;
    nai_fd_t fd;
    nai_stat_t st;
    nai_fs_watch_fd_t k;
    nai_fs_watch_file_t* f;
    nai_fs_watch_serv_t* s;
    nai_fs_watch_port_t* w;
    nai_rbnode_t** i;
    nai_rbnode_t* parent;


    if ((n->key.flags & NAI_FS_WATCH_PATH)) {
        oflags = 0;
        if (n->key.flags & NAI_FS_WATCH_SYMLINK) {
            oflags |= NAI_STAT_SYMLINK;
        };

        r = nai_stat(nai_str(&n->key.path), &st, oflags);
        if (r < 0) {
            nai_memset(&st, 0, sizeof(st));
        };

        fd = NAI_FD_INVALID;
        k.flags = n->key.flags;
        k.path = n->key.path;
        k.fd = NAI_FD_INVALID;
        k.dev = 0;
        k.ino = 0;
    } else {
        oflags = NAI_O_RDONLY;
        if (n->key.flags & NAI_FS_WATCH_SYMLINK) {
            oflags |= NAI_O_SYMLINK;
        };

        fd = nai_file_open(nai_str(&n->key.path), oflags);
        if (fd == NAI_FD_INVALID) {
            nai_log_debug(NAI_LOG_CORE, 
                nai_errno, "open %s failed", nai_str(&n->key.path));
            r = -1;
            goto _end;
        };

        r = nai_file_stat(fd, &st, 0);
        if (r < 0) {
            ec = nai_errno;
            nai_file_close(fd);
            nai_errno = ec;
            goto _end;
        };

        k.flags = n->key.flags & NAI_FS_WATCH_SYMLINK;
        k.path = n->key.path;
        k.fd = fd;
        k.dev = st.st_dev;
        k.ino = st.st_ino;
    };


    w = (nai_fs_watch_port_t*)n->node.port;
    s = (nai_fs_watch_serv_t*)w->port.serv;
    nai_fs_watch_serv_lock_map(&s->serv, 1);

    /* find file */
    i = nai_fs_watch_find_file(&s->fmap, &k, &parent);
    if (i[0] != 0) {
        f = nai_containof(i[0], nai_fs_watch_file_t, ent);
        r = 0;
    } else {
        f = (nai_fs_watch_file_t*)nai_fixedpool_alloc(&s->pool);
        if (f == 0) {
            r = -1;
            goto _fail;
        };

        /* init file */
        nai_list_init(&f->list);
        nai_list_init(&f->entn);
        f->refs = 0;
        f->events = 0;
        f->fd = k;
        f->stf = st;
        f->stp = st;
        nai_fs_watch_str_ref(&f->fd.path, &k.path);

        /* insert to the file map */
        nai_rbtree_link(&s->fmap, &f->ent, parent, i);
        nai_rbtree_color(&s->fmap, &f->ent);

        /* add to the pending task list */
        r = nai_list_is_empty(&s->tasks) && nai_list_is_empty(&s->pendings);
        nai_list_insert_tail(&s->pendings, &f->entn);

        /* clear fd */
        fd = NAI_FD_INVALID;
    };

    /* reference file */
    f->refs ++;
    nai_list_insert_tail(&f->list, &n->entf);
    nai_fs_watch_serv_unlock_map(&s->serv, 1);

    /* active task loop */
    if (r != 0) {
        nai_fs_watch_serv_active(s);
    };

    if ((k.flags & NAI_FS_WATCH_PATH) == 0 && fd != NAI_FD_INVALID) {
        nai_file_close(fd);
    };

    n->file = f;
    r = 0;

_end:
    return r;

_fail:
    ec = nai_errno;
    nai_fs_watch_serv_unlock_map(&s->serv, 1);
    nai_file_close(fd);
    nai_errno = ec;
    goto _end;
};


static nai_int_t nai_fs_watch_close_file(nai_fs_watch_node_t* n)
{
    nai_int_t r;
    nai_fs_watch_file_t* f;
    nai_fs_watch_serv_t* s;
    nai_fs_watch_port_t* w;


    w = (nai_fs_watch_port_t*)n->node.port;
    s = (nai_fs_watch_serv_t*)w->port.serv;
    nai_fs_watch_serv_lock_map(&s->serv, 1);

    f = n->file;
    nai_list_entry_remove(&n->entf);

    f->refs --;
    if (f->refs <= 0) {
        nai_fs_watch_free_file(s, f);
    };

    nai_fs_watch_serv_unlock_map(&s->serv, 1);

    n->file = 0;
    r = 0;

    return r;
};


static nai_int_t nai_fs_watch_port_init(nai_evmsg_port_t* p)
{
    nai_int_t r;
    nai_fs_watch_port_t* w;


    w = (nai_fs_watch_port_t*)p;
    w->cb = 0;
    r = 0;

    return r;
};


static nai_int_t nai_fs_watch_port_read(nai_evmsg_port_t* p)
{
    assert(0);
    (void)p;
    return 0;
};


static nai_int_t nai_fs_watch_port_open(nai_evmsg_port_t* p, nai_evloop_t* l)
{
    nai_int_t r;
    nai_fs_watch_port_t* w;
    nai_fs_watch_serv_t* s;


    (void)l;

    w = (nai_fs_watch_port_t*)p;
    s = (nai_fs_watch_serv_t*)p->serv;
    w->cb = nai_evnode_get_cb(&w->port.ev);
    nai_evnode_set_cb(&w->port.ev, nai_fs_watch_port_handle);
    nai_fs_watch_serv_attach(s);
    r = 0;

    return r;
};


static nai_int_t nai_fs_watch_port_close(nai_evmsg_port_t* p)
{
    nai_int_t r;
    nai_fs_watch_port_t* w;
    nai_fs_watch_serv_t* s;


    w = (nai_fs_watch_port_t*)p;
    s = (nai_fs_watch_serv_t*)p->serv;
    nai_evnode_set_cb(&w->port.ev, w->cb);
    nai_fs_watch_serv_detach(s);
    r = 0;

    return r;
};


static nai_int_t nai_fs_watch_port_rearm(nai_evmsg_port_t* p)
{
    nai_int_t r;
    nai_fs_watch_serv_t* s;


    s = (nai_fs_watch_serv_t*)p->serv;
    nai_fs_watch_serv_lock_map(&s->serv, 1);
    r = !nai_list_is_empty(&s->tasks) || !nai_list_is_empty(&s->pendings);
    nai_fs_watch_serv_unlock_map(&s->serv, 1);

    if (r != 0) {
        nai_fs_watch_serv_active(s);
    };
    r = 0;

    return r;
};


static nai_int_t nai_fs_watch_port_node_open(nai_evmsg_node_t* e, void* key)
{
    nai_int_t r;
    nai_fs_watch_node_t* n;
    nai_fs_watch_key_t* k;


    (void)key;

    n = (nai_fs_watch_node_t*)e;
    k = (nai_fs_watch_key_t*)key;

    /* init */
    n->file = 0;
    n->key.flags = k->flags;
    nai_fs_watch_str_ref(&n->key.path, &k->path);
    nai_list_init(&n->entf);


    r = nai_fs_watch_open_file(n);

    return r;
};


static nai_int_t nai_fs_watch_port_node_close(nai_evmsg_node_t* e)
{
    nai_int_t r;
    nai_fs_watch_node_t* n;


    n = (nai_fs_watch_node_t*)e;
    if (n->file != 0) {
        nai_fs_watch_close_file(n);
    };

    nai_fs_watch_str_unref(&n->key.path);
    nai_fs_watch_port_node_clean(e);
    r = 0;

    return r;
};


#endif



//////////////////////////////////////////////////////////////////////////////
// fs watch


struct nai_fs_watch_serv_global_s {
    nai_int_t inited;           /**< the mark of initialized */
    nai_int_t error;            /**< the last error */
    nai_once_t once;            /**< the once */
    nai_fs_watch_serv_t serv;   /**< the service */
};


static nai_fs_watch_serv_global_t nai_fsserv = {
    0, 0, NAI_ONCE_INIT
};


static nai_evmsg_ops_t nai_fs_watch_ops = {
    "fs_watch", 
    sizeof(nai_fs_watch_port_t) - sizeof(nai_evmsg_port_t), 
    sizeof(nai_fs_watch_node_t) - sizeof(nai_evmsg_node_t), 
    sizeof(nai_fs_watch_handle_t) - sizeof(nai_evmsg_handle_t), 
    nai_fs_watch_find, 
    nai_fs_watch_serv_lock_map, 
    nai_fs_watch_serv_unlock_map, 
    nai_fs_watch_port_init, 
    nai_fs_watch_port_open, 
    0, /* port_wait */
    0, /* port_read */
    0, /* port_send */
    nai_fs_watch_port_close, 
    nai_fs_watch_port_rearm, 
#if (NAI_FS_WATCH_NO_GLOBAL)
    0, 0, 
#else
    nai_fs_watch_serv_node_open, 
    nai_fs_watch_serv_node_close, 
#endif
    nai_fs_watch_port_node_open, 
    nai_fs_watch_port_node_close, 
    nai_fs_watch_handle_init, 
    nai_fs_watch_handle_call, 
};


static void nai_fs_watch_serv_global_init()
{
    nai_int_t r;
    nai_fs_watch_serv_t* s;


    s = &nai_fsserv.serv;
    r = nai_evmsg_serv_init(&s->serv);
    if (r < 0) {
        nai_fsserv.error = nai_errno;
        goto _end;
    };

    r = nai_evmsg_serv_open(&s->serv, &nai_fs_watch_ops);
    if (r < 0) {
        nai_fsserv.error = nai_errno;
        goto _end;
    };

    s->uid = 0;
#if (NAI_FS_WATCH_USE_WIN32)
    nai_rbtree_init(&s->fmap);
#elif (NAI_FS_WATCH_USE_KQUEUE)
    nai_rbtree_init(&s->fmap);
    nai_fixedpool_init(&s->pool, sizeof(nai_fs_watch_file_t), 0, 0);
#elif (NAI_FS_WATCH_USE_POLL)
    s->rate = 0;
    s->refs = 0;
    s->osloop = 0;
    nai_evnode_init(&s->timer);
    nai_list_init(&s->tasks);
    nai_list_init(&s->pendings);
    nai_rbtree_init(&s->fmap);
    nai_fixedpool_init(&s->pool, sizeof(nai_fs_watch_file_t), 0, 0);
    (void)nai_fs_watch_port_node_error;
#endif

_end:
    nai_memory_barrier();
    nai_fsserv.inited = 1;
    return;
};


static void nai_fs_watch_serv_global_term()
{
    if (nai_fsserv.inited) {
        nai_evmsg_serv_close(&nai_fsserv.serv.serv);

#if (NAI_FS_WATCH_USE_KQUEUE)
        nai_fixedpool_close(&nai_fsserv.serv.pool);
#elif (NAI_FS_WATCH_USE_POLL)
        nai_fixedpool_close(&nai_fsserv.serv.pool);
#endif
    };

    return;
};


static nai_fs_watch_serv_t* nai_fs_watch_serv_get()
{
    nai_fs_watch_serv_t* r;


    if (nai_fsserv.inited == 0) {
        nai_once(&nai_fsserv.once, nai_fs_watch_serv_global_init);
    };
    if (nai_fsserv.error) {
        nai_errno = nai_fsserv.error;
        r = 0;
        goto _end;
    };

    r = &nai_fsserv.serv;

_end:
    return r;
};


static nai_evmsg_port_t* nai_fs_watch_get_port(nai_evloop_t* l, nai_int_t na)
{
    nai_evmsg_port_t* p;
    nai_fs_watch_serv_t* s;


    s = nai_fs_watch_serv_get();
    if (s == 0) {
        p = 0;
        goto _end;
    };

    p = nai_evmsg_port_get(&s->serv, l, na);

_end:
    return p;
};


static nai_int_t nai_fs_watch_handle(nai_evnode_t* e, nai_int_t events)
{
    nai_int_t r;
    nai_fs_watch_t* w;


    w = (nai_fs_watch_t*)e;
    if (w->cb) {
        w->cb(w, events, 0);
    };

    r = 0;

    return r;
};



nai_int_t nai_fs_watch_init(nai_fs_watch_t* w)
{
    nai_int_t r;


    w->cb = 0;
    nai_str_setn(&w->path);
    nai_evnode_init(&w->ev);
    r = 0;

    return r;
};


nai_int_t nai_fs_watch_open(
    nai_fs_watch_t* w, nai_evloop_t* l, const char* path, nai_int_t flags)
{
    intptr_t r;
    nai_int_t ec;
    nai_int_t uid;
    nai_evmsg_port_t* p;
    nai_evmsg_handle_t* h;
    nai_fs_watch_serv_t* s;
    nai_fs_watch_key_t key;
    intptr_t len;
    char* buf;


    if (l == 0) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };
    if (path == 0) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };
    if (flags & ~(
        NAI_FS_WATCH_CHILD |
        NAI_FS_WATCH_SYMLINK | NAI_FS_WATCH_PATH)) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    if (nai_fs_watch_is_opened(w)) {
        nai_errno = EEXIST;
        r = -1;
        goto _end;
    };

    buf = 0;
    len = 0;
    for (;;) {
        r = nai_path_get_full(buf + sizeof(nai_atomic32_t), len, path);
        if (r < 0) {
            goto _end;
        };
        if (r < len) {
            len = r;
            break;
        };

        len = r + 1;
        buf = (char*)nai_malloc(len + sizeof(nai_atomic32_t));
        if (buf == 0) {
            r = -1;
            goto _end;
        };
    };

    p = nai_fs_watch_get_port(l, NAI_EVMSG_PORT_GET);
    if (p == 0) {
        ec = nai_errno;
        nai_free(buf);
        nai_errno = ec;
        r = -1;
        goto _end;
    };


    *(nai_atomic32_t*)buf = 1;
    nai_str_setm(&key.path, buf + sizeof(nai_atomic32_t), len);
    key.flags = flags;
#if !(NAI_HAVE_FS_WATCH_CHILD)
    key.flags &= ~NAI_FS_WATCH_CHILD;
#endif
#if defined(_WIN32)
    key.flags &= ~NAI_FS_WATCH_SYMLINK;
#endif
    if (!(key.flags & NAI_FS_WATCH_PATH)) {
        s = (nai_fs_watch_serv_t*)p->serv;
        nai_fs_watch_serv_lock_map(&s->serv, 1);

        uid = 0;
        while (uid == 0) {
            s->uid ++;
            uid = s->uid << 8;
        };

        nai_fs_watch_serv_unlock_map(&s->serv, 1);

        key.flags |= uid;
    };

    h = nai_evmsg_port_emit_make(p, &w->ev, NAI_EVMSG_ADD, &key);
    if (h == 0) {
        ec = nai_errno;
        r = -1;
        goto _fail;
    };

    nai_evnode_set_cb(&w->ev, nai_fs_watch_handle);
    r = nai_evnode_open(&w->ev, l);
    if (r < 0) {
        ec = nai_errno;
        nai_evmsg_port_emit_cancel(p, h);
        goto _fail;
    };

    r = nai_evmsg_port_emit_exec(p, h);
    if (r < 0) {
        ec = nai_errno;

        r = nai_evnode_close(&w->ev);
        assert(r >= 0);
        goto _fail;
    };

    w->path = key.path;
    w->flags = key.flags;


_end:
    return (nai_int_t)r;

_fail:
    if (buf) {
        nai_free(buf);
    };
    nai_evmsg_port_release(p);
    nai_errno = ec;
    r = -1;
    goto _end;
};


nai_int_t nai_fs_watch_close(nai_fs_watch_t* w)
{
    nai_int_t r;
    nai_evloop_t* l;
    nai_evmsg_port_t* p;
    nai_fs_watch_key_t key;


    l = nai_fs_watch_get_loop(w);
    if (l == 0) {
        r = 0;
        goto _end;
    };

    p = nai_fs_watch_get_port(l, NAI_EVMSG_PORT_GET_NOREF);
    if (p == 0) {
        r = -1;
        goto _end;
    };

    r = nai_evnode_close(&w->ev);
    if (r < 0) {
        goto _end;
    };

    key.path = w->path;
    key.flags = w->flags;
    r = nai_evmsg_port_emit(p, &w->ev, NAI_EVMSG_DEL, &key);
    if (r < 0) {
        goto _end;
    };

    r = nai_evmsg_port_release(p);
    if (r < 0) {
        goto _end;
    };

    nai_fs_watch_str_unref(&w->path);
    r = 0;

_end:
    return r;
};



#else


nai_int_t nai_fs_watch_init(nai_fs_watch_t* w)
{
    nai_int_t r;


    w->cb = 0;
    w->flags = 0;
    nai_str_setn(&w->path);
    nai_evnode_init(&w->ev);
    r = 0;

    return r;
};


nai_int_t nai_fs_watch_open(nai_fs_watch_t* w, 
    nai_evloop_t* loop, const char* path, nai_int_t flags)
{
    nai_int_t r;


    (void)w;
    (void)loop;
    (void)path;
    (void)flags;

    nai_errno = ENOTSUP;
    r = -1;

    return r;
};


nai_int_t nai_fs_watch_close(nai_fs_watch_t* w)
{
    nai_int_t r;


    r = nai_evnode_close(&w->ev);
    if (r < 0) {
        goto _end;
    };

    w->cb = 0;

_end:
    return r;
};



#endif


#if (NAI_HAVE_FS_WATCH)


void nai_fs_watch_term()
{
    nai_fs_watch_serv_global_term();

    return;
};


#endif


