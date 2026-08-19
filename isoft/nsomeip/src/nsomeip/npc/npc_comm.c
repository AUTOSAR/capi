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
/// @file       npc_comm.c
/// @brief
/// @details
/// @date       2025-03-07
/// @author     Zan Shigang
/// @version    1.2.0
///
/// ================================================================

#include "npc_comm.h"
#include "npc_port.h"
#include "npc_bufpool.h"
#include "npc_message.h"
#include "nsomeip/net/nsi_message.h"
#include "nsomeip/net/nsi_network.h"
#include "nsomeip/core/nsi_log.h"
#include "nsomeip/core/nsi_const.h"
#include "nai/runtime/nai_list.h"
#include "nai/runtime/nai_util.h"
#include "nai/runtime/nai_errno.h"
#include <stdarg.h>



//////////////////////////////////////////////////////////////////////////////
// comm manage


static nai_rbnode_t** npc_comm_find(npc_comm_t* c, 
    npc_serv_t serv, npc_inst_t inst, nai_rbnode_t** pparent)
{
    nai_rbnode_t** n = &nai_rbtree_root(&c->ifs);
    nai_rbnode_t* parent = nai_rbtree_end(&c->ifs);
    npc_comif_t* e;
    npc_servkey_t v;


    v.serv = serv;
    v.inst = inst;

    while (*n) {
        parent = *n;
        e = (npc_comif_t*)parent;
        if (e->servinst == v.servinst) {
            break;
        } else if (e->servinst >= v.servinst) {
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


static int npc_comm_free_messages(npc_comm_t* c, nai_list_t* list);


static int npc_comm_handle(nai_evnode_t* n, int event)
{
    int r;
    nai_list_t list;
    npc_comm_t* c;


    (void)event;

    c = nai_containof(n, npc_comm_t, ev);


    /* lock */
    nai_spin_lock(&c->lock);

    /* move list from comm manage */
    nai_list_init(&list);
    nai_list_add_tail(&list, &c->mlist);

    /* unlock */
    nai_spin_unlock(&c->lock);


    /* free message list */
    r = npc_comm_free_messages(c, &list);


    return r;
};


int npc_comm_init(npc_comm_t* c)
{
    int r;


    nai_evnode_init(&c->ev);
    nai_evnode_set_cb(&c->ev, npc_comm_handle);
    nai_rbtree_init(&c->ifs);
    nai_spin_init(&c->lock);
    nai_list_init(&c->mlist);
    nai_list_init(&c->clist);
    c->ud = 0;
    r = 0;

    return r;
};


int npc_comm_open(npc_comm_t* c, nai_evloop_t* l)
{
    int r;
    int ec;


    if (l == 0) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    r = nai_spin_open(&c->lock, 0);
    if (r < 0) {
        goto _end;
    };

    r = nai_evnode_open(&c->ev, l);
    if (r < 0) {
        goto _fail;
    };

_end:
    return r;

_fail:
    ec = nai_errno;
    nai_spin_close(&c->lock);
    nai_errno = ec;
    goto _end;
};


int npc_comm_close(npc_comm_t* c)
{
    int r;
    npc_comif_t* e;
    nai_rbnode_t* n;


    r = nai_evnode_close(&c->ev);
    if (r < 0) {
        goto _end;
    };

    r = npc_comm_handle(&c->ev, NAI_EV_NOTIFY_FROM(0));
    if (r < 0) {
        goto _end;
    };

    n = nai_rbtree_begin(&c->ifs);
    for ( ; n != nai_rbtree_end(&c->ifs); ) {
        e = nai_containof(n, npc_comif_t, ent);
        n = nai_rbtree_next(n);
        r = npc_comif_state(e, NPC_STATE_QUIT);
        if (r < 0) {
            nai_log_error(NPC_LOG_CORE, nai_errno, 
                "the comm interface can't close, "
                "some messages aren't released.");

            goto _end;
        };

        npc_comif_close(e);
    };

    assert(nai_list_is_empty(&c->mlist));
    assert(nai_list_is_empty(&c->clist));

    nai_spin_close(&c->lock);
    c->ud = 0;
    r = 0;

_end:
    return r;
};



//////////////////////////////////////////////////////////////////////////////
// comm interface


typedef struct npc_comif_hold_s {
    nai_rbnode_t ent;
    uint32_t uid;
    int refs;
} npc_comif_hold_t;


static nai_rbnode_t** npc_comif_find_hold(
    npc_comif_t* p, uint32_t uid, nai_rbnode_t** pparent)
{
    nai_rbnode_t** n = &nai_rbtree_root(&p->holds);
    nai_rbnode_t* parent = nai_rbtree_end(&p->holds);
    npc_comif_hold_t* e;


    while (*n) {
        parent = *n;
        e = (npc_comif_hold_t*)parent;
        if (e->uid == uid) {
            break;
        } else if (e->uid >= uid) {
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


static int npc_comif_free(npc_comif_t* p)
{
    int r;
    int n;
    npc_message_t* m;
    nai_bufpool_t pool;
    nai_list_entry_t* e;


    /* free messages */
    for (n = 0; n < 2; n ++) {
        e = p->mlist[n].next;
        for ( ; e != &p->mlist[n]; ) {
            m = nai_containof(e, npc_message_t, ent);
            e = e->next;

            nai_list_entry_remove(&m->ent);
            pool = *(m->payload.pool);
            nai_bufpool_close(&pool);
        };
    };

    /* free shared memory */
    npc_zone_close(&p->zone);
    npc_shmz_close(&p->shmz);

    /* free */
    pool = p->pool;
    nai_bufpool_close(&pool);
    r = 0;

    return r;
};


npc_comif_t* npc_comif_create(
    npc_comm_t* c, 
    npc_comif_ops_t* ops, void* ctx, 
    const char* path, 
    const npc_shmz_attr_t* attr, int flags, ...)
{
    int r;
    int ec;
    int mode;
    int create;
    va_list va;
    npc_port_t* port;
    npc_comif_t* p;
    nai_rbnode_t** n;
    nai_rbnode_t* parent;
    nai_bufpool_t pool;


    /* find from map, unneed lock */
    n = npc_comm_find(c, attr->serv, attr->inst, &parent);
    if (n[0] != 0) {
        nai_errno = EEXIST;
        p = 0;
        goto _end;
    };

    r = nai_bufpool_create(&pool, 1000, 0);
    if (r < 0) {
        p = 0;
        goto _end;
    };

    /* allocate comm interface */
    p = (npc_comif_t*)nai_bufpool_alloc(&pool, sizeof(*p));
    if (p == 0) {
        goto _fail;
    };

    /* initial comm interface */
    npc_shmz_init(&p->shmz);
    npc_zone_init(&p->zone, 0);
    nai_list_init(&p->mlist[0]);
    nai_list_init(&p->mlist[1]);
    nai_rbtree_init(&p->ports);
    nai_rbtree_init(&p->holds);
    p->pool = pool;
    p->com = c;
    p->ctx = ctx;
    p->ops = ops;
    p->refs = 1;
    p->refm = 0;
    p->stat = 0;
    p->self = attr->self;
    p->serv = attr->serv;
    p->inst = attr->inst;


    /* allocate default port */
    port = (npc_port_t*)nai_bufpool_alloc(&pool, sizeof(*p));
    if (port == 0) {
        goto _fail;
    };

    /* initial default port */
    nai_list_init(&port->out.send);
    nai_list_init(&port->out.wait);
    port->cif = p;
    port->mref = 0;
    port->stat = 0;
    port->self = p->self;
    port->mode = NPC_PORT_SEND;
    port->local = 1;
    port->mport = 0;
    port->mpeer = 0;
    port->mqueue = 0;
    p->defport = port;


    /* open shared memory zone */
    create = flags & (NAI_O_CREAT|NAI_O_TMPFILE);
    if (create) {
        p->local = 1;

        va_start(va, flags);
        mode = va_arg(va, int);
        va_end(va);

        r = npc_shmz_open(&p->shmz, path, attr, flags, mode);
    } else {
        r = npc_shmz_open(&p->shmz, path, attr, flags);
    };
    if (r < 0) {
        goto _fail;
    };

    /* open local memory zone from shared memory zone */
    r = npc_zone_open(&p->zone, &p->shmz, npc_shmz_is_owner(&p->shmz), 0);
    if (r < 0) {
        goto _fail;
    };

    /* insert into map, do lock */
    nai_spin_lock(&c->lock);
    nai_rbtree_link(&c->ifs, &p->ent, parent, n);
    nai_rbtree_color(&c->ifs, &p->ent);
    nai_spin_unlock(&c->lock);

    nai_log_debug(NPC_LOG_CORE, 0, 
        "create the comif(%d, %d)", p->serv, p->inst);

_end:
    return p;

_fail:
    ec = nai_errno;
    if (p != 0) {
        npc_shmz_close(&p->shmz);
        npc_zone_close(&p->zone);
    };
    nai_bufpool_close(&pool);
    nai_errno = ec;
    p = 0;
    goto _end;
};


int npc_comif_release(npc_comif_t* p)
{
    int r;
    npc_comm_t* c;
    nai_atomic32_t refs;


    c = p->com;
    assert(c != 0);

    /* lock */
    nai_spin_lock(&c->lock);

    /* release */
    p->refs --;
    refs = p->refs;
    if (refs > 0) {
        nai_spin_unlock(&c->lock);
        r = 0;
        goto _end;
    };

    /* remove from free list */
    nai_list_entry_remove(&p->entl);

    /* unlock */
    nai_spin_unlock(&c->lock);


    /* free */
    r = npc_comif_free(p);

_end:
    return r;
};


int npc_comif_clean(npc_comif_t* p, uint16_t peer)
{
    int r;
    int refs;
    uint32_t uid;
    npc_port_t* t;
    npc_comif_hold_t* h;
    nai_rbnode_t* i;
    nai_rbnode_t** n;
    nai_rbnode_t* parent;


    nai_log_debug(NPC_LOG_CORE, 0, 
        "clean peer(0x%x) from the comif(%d, %d)", 
        peer, p->serv, p->inst);


    if (!p->local) {
        nai_errno = EPERM;
        r = -1;
        goto _end;
    };

    refs = 0;

    /* walk port tree to find posts which belong to the peer */
    /* mark the ports */
    i = nai_rbtree_begin(&p->ports);
    for ( ; i != nai_rbtree_end(&p->ports); ) {
        t = nai_containof(i, npc_port_t, ent);
        i = nai_rbtree_next(i);
        if (t->mode & NPC_PORT_SEND) {
            npc_port_remove(t, peer);
        } else if (t->mport->source == peer) {
            t->mport->closed = 1;
            if (t->closed && 
                t->in.last == t->in.next) {
                npc_port_close(t);
                continue;
            };

            t->clean = 1;
            refs ++;
        } else {
            /* nothing */
            ;
        };
    };

    /* no ports, clean directly */
    if (refs <= 0) {
        r = npc_shmz_clean(&p->shmz, peer);
        goto _end;
    };

    uid = peer | 0x10000;
    n = npc_comif_find_hold(p, uid, &parent);
    if (n[0] != 0) {
        h = (npc_comif_hold_t*)n[0];
    } else {
        h = (npc_comif_hold_t*)nai_bufpool_alloc(&p->pool, sizeof(*h));
        if (h == 0) {
            nai_log_alert(NPC_LOG_CORE, nai_errno, 
                "failed to allocate the context of cleaning");
            r = -1;
            goto _end;
        };

        h->uid = uid;
        nai_rbtree_link(&p->holds, &h->ent, parent, n);
        nai_rbtree_color(&p->holds, &h->ent);
    };

    h->refs = refs;
    r = npc_shmz_chown(&p->shmz, peer, uid);

_end:
    return r;
};


int npc_comif_close(npc_comif_t* p)
{
    int r;
    npc_comm_t* c;
    npc_port_t* t;
    nai_rbnode_t* i;


    nai_log_debug(NPC_LOG_CORE, 0, 
        "close the comif(%d, %d)", p->serv, p->inst);


    c = p->com;
    assert(c != 0);


    /* lock */
    nai_spin_lock(&c->lock);

    /* remove from comm manager, do lock */
    nai_rbtree_erase(&c->ifs, &p->ent);
    nai_list_insert_tail(&c->clist, &p->entl);
    p->quit = 0;
    p->ops = 0;
    p->ctx = 0;

    /* unlock */
    nai_spin_unlock(&c->lock);


    /* walk port tree to close all closable ports */
    i = nai_rbtree_begin(&p->ports);
    for ( ; i != nai_rbtree_end(&p->ports); ) {
        t = nai_containof(i, npc_port_t, ent);
        i = nai_rbtree_next(i);
        if (t->closed) {
            npc_port_close(t);
        };
    };

    /* release comm interface */
    r = npc_comif_release(p);

    return r;
};


int npc_comif_event(npc_comif_t* p, uint32_t event)
{
    int r;
    npc_port_t* e;
    npc_port_ev_t ev;
    nai_rbnode_t** n;


    ev.value = event;
    n = npc_port_find(p, ev.port, 0);
    if (n[0] == 0) {
        r = 0;
        goto _end;
    };

    nai_log_info(NPC_LOG_CORE, 
        0, "comif event %d %d event:%x ev.event:%x", 
        p->refs, p->refm, event, ev.event);
    e = nai_containof(n[0], npc_port_t, ent);
    switch (ev.event) {
    case NPC_PORT_READABLE:
        npc_port_handle_read(e);
        break;

    case NPC_PORT_SENDABLE:
        npc_port_handle_send(e);
        break;

    case NPC_PORT_REMOVABLE:
        npc_port_handle_remove(e);
        break;

    default:
        assert(0);
        break;
    };

    r = 0;

_end:
    return r;
};


int npc_comif_state(npc_comif_t* p, int state)
{
    int r;
    int ec;
    int changed = 0;
    npc_comm_t* c;
    npc_port_t* t;
    nai_rbnode_t* e;


    switch (state) {
    case NPC_STATE_OPEN:
        if (p->quit) {
            p->quit = 0;
            changed = 1;

        };
        r = 0;
        break;

    case NPC_STATE_QUIT:
        c = p->com;

        nai_spin_lock(&c->lock);

        if (p->quit == 0) {
            p->quit = 1;
            changed = 1;
        };

        if (p->refm > 0) {
            ec = EAGAIN;
            r = -1;
        } else {
            r = 0;
        };

        nai_spin_unlock(&c->lock);

        break;

    default:
        assert(0);
        ec = EINVAL;
        r = -1;
        break;
    };

    if (changed) {
        e = nai_rbtree_begin(&p->ports);
        for ( ; e != nai_rbtree_end(&p->ports); ) {
            t = nai_containof(e, npc_port_t, ent);
            e = nai_rbtree_next(e);
            npc_port_handle_state(t);
        };
    };

    if (r < 0) {
        nai_errno = ec;
    };

    return r;
};


int npc_comif_release_hold(npc_comif_t* p, uint16_t peer)
{
    int r;
    uint32_t uid;
    nai_rbnode_t** n;
    npc_comif_hold_t* h;


    uid = peer | 0x10000;
    n = npc_comif_find_hold(p, uid, 0);
    if (n[0] == 0) {
        nai_errno = ENOENT;
        r = -1;
        goto _end;
    };


    h = (npc_comif_hold_t*)n[0];
    h->refs --;
    if (h->refs > 0) {
        r = 0;
    } else {
        nai_rbtree_erase(&p->holds, &h->ent);
        nai_bufpool_free(&p->pool, h);

        r = npc_shmz_clean(&p->shmz, uid);
    };

_end:
    return r;
};



//////////////////////////////////////////////////////////////////////////////
// message


static int npc_comm_free_messages(npc_comm_t* c, nai_list_t* list)
{
    int r;
    nai_list_t* listp;
    nai_list_entry_t* e;
    npc_port_t* p;
    npc_comif_t* q;
    npc_message_t* m;


    e = list->next;
    for ( ; e != list; ) {
        m = nai_containof(e, npc_message_t, ent);
        e = e->next;

        p = m->port;
        if (!(p->mode & NPC_PORT_SEND)) {
            npc_port_handle_seen(p, m);
        };
    };


    /* lock */
    nai_spin_lock(&c->lock);

    /* dereference and move to free list */
    e = list->next;
    for ( ; e != list; ) {
        m = nai_containof(e, npc_message_t, ent);
        e = e->next;

        p = m->port;
        q = p->cif;

        /* move to free list */
        if (!(p->mode & NPC_PORT_SEND)) {
            listp = &q->mlist[NPC_USAGE_READ];
        } else {
            listp = &q->mlist[NPC_USAGE_SEND];
        };
        nai_list_insert_tail(listp, &m->ent);


        /* dereference */
        q->refm --;
        q->refs --;

        if (q->refs <= 0) {

            nai_list_entry_remove(&q->entl);

            /* free comm interface */
            nai_spin_unlock(&c->lock);
            npc_comif_free(q);
            nai_spin_lock(&c->lock);

        } else if (q->refm <= 0 && q->quit && q->ops) {

            /* notify quitable */
            nai_spin_unlock(&c->lock);
            q->ops->quit(q->ctx, 0);
            nai_spin_lock(&c->lock);

        } else {

            /* nothing */
            ;
        };
    };

    /* unlock */
    nai_spin_unlock(&c->lock);

    r = 0;

    return r;
};


npc_message_t* npc_message_create_locked(npc_comif_t* p, int usage)
{
    int r;
    int ec;
    npc_comm_t* c;
    npc_message_t* m;
    nai_list_t* list;
    nai_list_entry_t* e;
    nai_bufpool_t pool;


    /* already locked */


    /* find exists message */
    list = &p->mlist[!!usage];
    e = list->next;
    if (e != list) {
        m = nai_containof(e, npc_message_t, ent);
        nai_list_entry_remove(&m->ent);
        goto _find;
    };


    /* allocate a new message */
    r = npc_bufpool_open(&pool, &p->zone);
    if (r < 0) {
        ec = nai_errno;
        nai_log_alert(NPC_LOG_CORE, ec, 
            "failed to allocate buffer pool of message");
        goto _fail;
    };

    m = (npc_message_t*)
        nai_bufpool_malloc(&pool, sizeof(*m) + sizeof(pool));
    if (m == 0) {
        ec = nai_errno;
        nai_log_alert(NPC_LOG_CORE, ec, 
            "failed to allocate message from buffer pool");

        nai_bufpool_close(&pool);
        goto _fail;
    };

    npc_cred_init(&m->cred);
    nai_memset(&m->hdr, 0, sizeof(m->hdr));
    nai_buflist_init(&m->payload, (nai_bufpool_t*)(m + 1));
    m->payload.pool[0] = pool;

_find:
    /* reference comm manage */
    //m->hdr.unserialized = 0;
    p->refm ++;
    p->refs ++;

    c = p->com;
    nai_spin_unlock(&c->lock);

    m->port = p->defport;
    m->refc = 1;
    m->refm = 0;
    m->inst = 0;
    m->flags = 0;

_end:
    return m;

_fail:
    c = p->com;
    nai_spin_unlock(&c->lock);
    nai_errno = ec;
    m = 0;
    goto _end;
};


npc_message_t* npc_message_create(npc_comif_t* p, int usage)
{
    npc_comm_t* c;
    npc_message_t* m;


    c = p->com;
    nai_spin_lock(&c->lock);

    m = npc_message_create_locked(p, usage);

    return m;
};


npc_message_t* npc_comm_create_message(
    npc_comm_t* c, npc_serv_t serv, npc_inst_t inst, int usage)
{
    int ec;
    npc_comif_t* p;
    npc_message_t* m;
    nai_rbnode_t** n;


    /* lock */
    nai_spin_lock(&c->lock);


    /* find from map, unneed lock */
    n = npc_comm_find(c, serv, inst, 0);
    if (n[0] == 0) {
        ec = ENOENT;
        goto _fail;
    };


    /* already quited */
    p = nai_containof(n[0], npc_comif_t, ent);
    if (p->quit) {
        ec = ENOENT;
        goto _fail;
    };

    /* create message */
    m = npc_message_create_locked(p, usage);


_end:
    return m;


_fail:
    /* unlock */
    nai_spin_unlock(&c->lock);

    nai_errno = ec;
    m = 0;
    goto _end;
};


int nsi_npc_message_close(npc_message_t* m)
{
    int r;
    int sync;
    int empty;
    nai_list_t list;
    npc_port_t* p;
    npc_comm_t* c;
    npc_comif_t* q;


    if (nai_atomic32_dec(&m->refc) > 0) {
        r = 0;
        goto _end;
    };

    /* should free payload before close reference message */
    nai_buflist_close(&m->payload);

    if (m->refm) {
        nsi_npc_message_close(m->refm);
        m->refm = 0;
    };

    p = m->port;
    q = p->cif;
    c = q->com;


    sync = nai_evloop_in_dispatch(nai_evnode_get_loop(&c->ev));
    if (sync) {
        nai_list_init(&list);
        nai_list_insert_tail(&list, &m->ent);
        r = npc_comm_free_messages(c, &list);
        goto _end;
    };


    /* lock */
    nai_spin_lock(&c->lock);


    if ((p->mode & NPC_PORT_SEND) && 
        (q->refs > 1 && q->refm > 1)) {

        /* a send message, put into free list directly */
        nai_list_insert_tail(&q->mlist[NPC_USAGE_SEND], &m->ent);

        /* decrement reference counts */
        q->refm --;
        q->refs --;

    } else {

        /* put into free list and notify comm manage to handle */
        empty = nai_list_is_empty(&c->mlist);

        nai_list_insert_tail(&c->mlist, &m->ent);

        if (empty) {
            r = nai_evnode_set_timeout(&c->ev, NAI_TIMEOP_SET, 100);
            if (r < 0) {
                /* fixme: ignore error */
            };
        };
    };

    /* unlock */
    nai_spin_unlock(&c->lock);

    r = 0;

_end:
    return r;
};

npc_message_t* nsi_npc_message_dup(npc_message_t* m, int mt)
{
    int ec;
    int usage;
    npc_port_t* p;
    npc_message_t* r;
    npc_message_t* o;
    nai_buf_t* b;
    nai_buf_t* d;
    nai_list_entry_t* e;


    p = m->port;
    p = p->cif->defport;
    usage = (p->mode & NPC_PORT_SEND) ? NPC_USAGE_SEND : NPC_USAGE_READ;

    r = nsi_network_create_message(p->cif->com->ud, usage);
    if (r == 0) {
        goto _end;
    };


    if (m->refm == 0) {
        o = m;
    } else {
        o = m->refm;
    };


    /* dup payload */
    e = o->payload.ent.next;
    for ( ; e != &o->payload.ent; e = e->next) {
        b = (nai_buf_t*)e;
        if (nai_buf_size(b) <= 0) {
            continue;
        };

        d = nai_buf_dup(r->payload.pool, b, 0);
        if (d == 0) {
            ec = nai_errno;
            nai_log_alert(NPC_LOG_CORE, ec, 
                "failed to duplicate the payload of message");

            nsi_npc_message_close(r);
            nai_errno = ec;
            r = 0;
            goto _end;
        };

        nai_buflist_insert_tail(&r->payload, d);
    };
    if (mt) {
        nai_buflist_set_threadsafe(&r->payload);
    };


    /* copy meta data */
    r->inst = o->inst;
    r->hdr = o->hdr;


    /* ref */
    r->refm = o;
    nai_atomic32_inc(&o->refc);


_end:
    return r;
};

npc_message_t* nsi_npc_message_create(nsi_msgpool_t* p)
{
    npc_message_t* m;
    m = 0;
    if (p && p->ud) {
        m = npc_comm_create_message(p->ud, NSI_SERVICE_ANY, NSI_INSTANCE_ANY, p->usage);
        if (m != 0) {
            m->pool = p;
        };
    };
    return m;
};

int nsi_npc_msgpool_close(nsi_msgpool_t* p)
{
    p->ops = 0;
    p->ud = 0;
    return 0;
};

nsi_msgpool_ops_t nsi_npc_pool_ops = {
    nsi_npc_message_create,
    nsi_npc_message_dup,
    nsi_npc_message_close,
    nsi_npc_msgpool_close
};







