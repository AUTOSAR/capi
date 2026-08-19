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
/// @file       nai_evmsg.c
/// @brief      
/// @details
/// @date       2023-10-22
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#include "nai_evmsg.h"
#include "nai_evloop.h"
#include "nai/runtime/nai_list.h"
#include "nai/runtime/nai_util.h"
#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_log.h"



typedef struct nai_evmsg_s {
    nai_list_entry_t ent;
    nai_evmsg_hdr_t hdr;
    union {
        void* ptr;
        uint8_t data[sizeof(void*)];
    };
} nai_evmsg_t;



static nai_int_t nai_evmsg_port_handle(nai_evnode_t* e, nai_int_t events);
static nai_int_t nai_evmsg_port_handle_emit(
    nai_evmsg_port_t* p, nai_evmsg_handle_t* call);


static nai_rbnode_t** nai_evmsg_serv_find(
    nai_evmsg_serv_t* s, void* key, nai_rbnode_t** pparent)
{

    nai_rbnode_t** n;


    n = s->ops->find(&s->nmap, key, pparent);

    return n;
};


static nai_int_t nai_evmsg_serv_lock_map(
    nai_evmsg_serv_t* s, nai_int_t block)
{
    return s->ops->serv_lock_map(s, block);
};


static nai_int_t nai_evmsg_serv_unlock_map(
    nai_evmsg_serv_t* s, nai_int_t block)
{
    return s->ops->serv_unlock_map(s, block);
};


static nai_int_t nai_evmsg_port_init(
    nai_evmsg_port_t* p, nai_evmsg_serv_t* s)
{
    nai_int_t r;
    size_t size_node;
    size_t size_handle;


    size_node = sizeof(nai_evmsg_node_t) + s->ops->size_node;
    size_handle = sizeof(nai_evmsg_handle_t) + s->ops->size_handle;

    nai_evnode_init(&p->ev);
    nai_spin_init(&p->lock);
    nai_cond_init(&p->cond);
    nai_list_init(&p->ents);
    nai_list_init(&p->entl);
    nai_list_init(&p->msgs);
    nai_list_init(&p->pending);
    nai_rbtree_init(&p->nmap);
    nai_pool_init(&p->pool, 1024);
    nai_fixedpool_init(&p->poole, size_node, 0, 0);
    nai_fixedpool_init(&p->poolh, size_handle, 0, 0);
    p->serv = 0;
    p->refs = 0;
    p->token = 0;
    p->locking = 0;
    p->inusing = 0;
    p->removed = 0;
    r = s->ops->port_init(p);

    return r;
};


static nai_int_t nai_evmsg_port_open(
    nai_evmsg_port_t* p, nai_evmsg_serv_t* s, nai_evloop_t* l)
{
    nai_int_t r;
    nai_int_t ec;


    r = nai_spin_open(&p->lock, 0);
    if (r < 0) {
        goto _end;
    };

    p->serv = s;
    if (l == 0) {
        r = nai_cond_open(&p->cond, 0);
        if (r < 0) {
            goto _fail;
        };
    } else {
        nai_evnode_set_cb(&p->ev, nai_evmsg_port_handle);
    };

    r = s->ops->port_open(p, l);
    if (r < 0) {
        goto _fail;
    };

    if (l != 0) {
        r = nai_evnode_open(&p->ev, l);
    };


_end:
    return r;

_fail:
    ec = nai_errno;
    s->ops->port_close(p);
    nai_cond_close(&p->cond);
    nai_spin_close(&p->lock);
    nai_errno = ec;
    goto _end;
};


static nai_int_t nai_evmsg_port_close(nai_evmsg_port_t* p)
{
    nai_int_t r;
    nai_int_t ec;
    nai_evmsg_serv_t* s;


    if (p->serv == 0) {
        r = 0;
        goto _end;
    };

    r = nai_evnode_close(&p->ev);
    if (r < 0) {
        ec = nai_errno;
        if (ec != EPERM) {
            goto _end;
        };

        r = nai_evnode_post(&p->ev, NAI_EVMSG_PORT_CLOSE);
        if (r == 0) {
            r = 1;
        };

        goto _end;
    };

    if (p->inusing == 0) {
        s = p->serv;
        r = s->ops->port_close(p);
        if (r < 0) {
            goto _end;
        };

        nai_list_init(&p->msgs);
        nai_list_init(&p->pending);
        nai_rbtree_init(&p->nmap);
        nai_pool_close(&p->pool);
        nai_fixedpool_close(&p->poole);
        nai_fixedpool_close(&p->poolh);
        nai_spin_close(&p->lock);
        nai_cond_close(&p->cond);
        p->serv = 0;
        p->token = 0;
        p->locking = 0;
        p->removed = 0;
    };

    r = 0;

_end:
    return r;
};


static nai_int_t nai_evmsg_port_free_handles(
    nai_evmsg_port_t* p, nai_list_t* list)
{
    nai_int_t r;
    nai_list_entry_t* e;
    nai_list_entry_t* n;


    e = list->next;
    for ( ; e != list; ) {
        n = e;
        e = e->next;

        nai_fixedpool_free(&p->poolh, n);
    };

    r = 0;

    return r;
};


static nai_rbnode_t** nai_evmsg_port_find(
    nai_evmsg_port_t* p, void* key, nai_rbnode_t** pparent)
{
    nai_rbnode_t** n;
    nai_evmsg_serv_t* s;


    s = p->serv;
    n = s->ops->find(&p->nmap, key, pparent);

    return n;
};


static nai_int_t nai_evmsg_port_lock(nai_evmsg_port_t* p)
{
    return nai_spin_lock(&p->lock);
};


static nai_int_t nai_evmsg_port_unlock(nai_evmsg_port_t* p)
{
    return nai_spin_unlock(&p->lock);
};



static nai_int_t nai_evmsg_port_node_open(
    nai_evmsg_port_t* p, nai_evmsg_node_t* e, void* key)
{
    nai_int_t r;
    nai_int_t ec;
    nai_rbnode_t** n;
    nai_rbnode_t* parent;
    nai_evmsg_serv_t* s;
    nai_evmsg_node_t* g;


    s = p->serv;
    if (s->ops->serv_node_open == 0) {
        e->glob = 0;
        r = s->ops->port_node_open(e, key);
        goto _end;
    };


    /* lock */
    nai_evmsg_serv_lock_map(s, 1);

    /* find global node or create a new one */
    n = nai_evmsg_serv_find(s, key, &parent);
    if (n[0] != 0) {
        g = (nai_evmsg_node_t*)n[0];
    } else {
        g = (nai_evmsg_node_t*)nai_fixedpool_alloc(&s->poole);
        if (g == 0) {
            ec = nai_errno;
            r = -1;
            goto _fail;
        };

        s->token ++;
        if (s->token < 0) {
            s->token = 1;
        };

        g->refs = 0;
        g->share = 0;
        g->tag = NAI_EVMSG_TAG_ENTRY;
        g->token = s->token;
        g->port = 0;
        g->glob = 0;
        nai_list_init(&g->ent);
        nai_list_init(&g->list);

        r = s->ops->serv_node_open(g, key);
        if (r < 0) {
            ec = nai_errno;
            g->tag = NAI_EVMSG_TAG_NONE;
            g->token = 0;
            nai_fixedpool_free(&s->poole, g);
            goto _fail;
        };

        /* insert global node into map */
        nai_rbtree_link(&s->nmap, &g->node, parent, n);
        nai_rbtree_color(&s->nmap, &g->node);
    };

    g->refs ++;
    nai_evmsg_serv_unlock_map(s, 1);


    /* open message node */
    e->glob = g;
    r = s->ops->port_node_open(e, key);
    if (r < 0) {
        ec = nai_errno;
    };


    nai_evmsg_serv_lock_map(s, 1);

    /* link to global node */
    if (r >= 0) {
        nai_list_insert_tail(&g->list, &e->ent);
    } else {
        g->refs --;
        if (g->refs <= 0) {
            s->ops->serv_node_close(g);
            g->tag = NAI_EVMSG_TAG_NONE;
            g->token = 0;
            nai_rbtree_erase(&s->nmap, &g->node);
            nai_fixedpool_free(&s->poole, g);
        };
    };

    nai_evmsg_serv_unlock_map(s, 1);

    if (r < 0) {
        nai_errno = ec;
    };


_end:
    return r;

_fail:
    nai_evmsg_serv_unlock_map(s, 1);
    nai_errno = ec;
    goto _end;
};


static nai_int_t nai_evmsg_port_node_close(
    nai_evmsg_port_t* p, nai_evmsg_node_t* e)
{
    nai_int_t r;
    nai_int_t ec;
    nai_evmsg_serv_t* s;
    nai_evmsg_node_t* g;


    s = p->serv;

    /* lock */
    g = e->glob;
    if (g != 0) {
        nai_evmsg_serv_lock_map(s, 1);

        /* remove from service message node */
        nai_list_entry_remove(&e->ent);

        /* unlock */
        nai_evmsg_serv_unlock_map(s, 1);
    };


    /* close message node */
    r = s->ops->port_node_close(e);
    if (g != 0) {
        if (r < 0) {
            ec = nai_errno;
        };

        nai_evmsg_serv_lock_map(s, 1);

        /* try to close service message node */
        if (r < 0) {
            nai_list_insert_tail(&g->list, &e->ent);
        } else {
            g->refs --;
            if (g->refs <= 0) {
                s->ops->serv_node_close(g);
                g->tag = NAI_EVMSG_TAG_NONE;
                g->token = 0;
                nai_rbtree_erase(&s->nmap, &g->node);
                nai_fixedpool_free(&s->poole, g);
            };
        };

        /* unlock */
        nai_evmsg_serv_unlock_map(s, 1);

        if (r < 0) {
            nai_errno = ec;
        };
    };


    return r;
};


static nai_int_t nai_evmsg_port_node_remove(
    nai_evmsg_port_t* p, nai_evmsg_node_t* e)
{
    nai_int_t r;


    r = nai_evmsg_port_node_close(p, e);
    if (r >= 0) {
        e->tag = NAI_EVMSG_TAG_NONE;
        e->token = 0;
        nai_rbtree_erase(&p->nmap, &e->node);
        nai_fixedpool_free(&p->poole, e);
    };

    r = 0;

    return r;
};



static nai_int_t nai_evmsg_port_send(
    nai_evmsg_port_t* p, nai_evmsg_node_t* e, void* data, size_t len)
{
    nai_int_t r;
    nai_int_t ec;
    nai_int_t empty;
    nai_evmsg_t* msg;


    nai_evmsg_port_lock(p);

    assert(sizeof(*msg) <= p->poolh.size);

    /* allocate message */
    msg = (nai_evmsg_t*)nai_fixedpool_alloc(&p->poolh);
    if (msg == 0) {
        ec = nai_errno;
        goto _fail;
    };

    /* init message */
    msg->hdr.e = e;
    msg->hdr.token = e->token;
    msg->hdr.len = (int32_t)len;
    if (len <= sizeof(msg->data)) {
        nai_memcpy(msg->data, data, len);
    } else {
        msg->ptr = nai_palloc(&p->pool, len);
        if (msg->ptr == 0) {
            ec = nai_errno;
            nai_fixedpool_free(&p->poolh, msg);
            goto _fail;
        };
        nai_memcpy(msg->ptr, data, len);
    };

    /* insert message */
    empty = nai_list_is_empty(&p->msgs);
    nai_list_insert_tail(&p->msgs, &msg->ent);


    /* notify */
    if (empty) {
        if (nai_evnode_is_opened(&p->ev)) {
            r = nai_evnode_post(&p->ev, NAI_EVMSG_PORT_READ);
        } else {
            r = nai_cond_signal(&p->cond);
        };
        if (r < 0) {
            ec = nai_errno;
            goto _fail;
        };
    };

    /* unlock */
    nai_evmsg_port_unlock(p);

    r = 0;

_end:
    return r;

_fail:
    /* failed */
    nai_evmsg_port_unlock(p);
    nai_errno = ec;
    r = -1;
    goto _end;
};


static nai_int_t nai_evmsg_port_read_lock(nai_evmsg_port_t* p)
{
    nai_int_t r;
    nai_evmsg_serv_t* s;


    s = p->serv;

    while (1) {
        if (p->locking == 0) {
            p->locking = 1;
            break;
        };

        /* get a thread local condition */
        s->cond = nai_thread_local_cond();
        if (s->cond == 0) {
            r = -1;
            goto _end;
        };

        /* switch to global lock */
        nai_mutex_lock(&s->mutex);
        nai_evmsg_port_unlock(p);

        /* wait for handling */
        while (p->locking) {
            nai_cond_wait(s->cond, &s->mutex);
        };

        /* switch to port lock */
        nai_mutex_unlock(&s->mutex);
        nai_evmsg_port_lock(p);

        s->cond = 0;
    };

    r = 0;

_end:
    return r;
};


static nai_int_t nai_evmsg_port_read_unlock(nai_evmsg_port_t* p)
{
    nai_int_t r;


    /* handle opeartions */
    p->locking = 0;
    nai_evmsg_port_handle_emit(p, 0);

    r = 0;

    return r;
};


static nai_int_t nai_evmsg_port_read_impl(nai_evmsg_port_t* p, uint32_t msec)
{
    nai_int_t r;
    nai_int_t ec;
    nai_int_t locking;
    uint64_t now;
    uint64_t abstime;
    nai_list_t list;
    nai_list_entry_t* i;
    nai_list_entry_t* n;
    nai_evmsg_t* m;
    nai_evmsg_serv_t* s;
    nai_evmsg_node_t* e;
    nai_evmsg_handle_t* h;
    void* data;


    s = p->serv;
    nai_evmsg_port_lock(p);


    if (msec != 0) {
        if (msec == (uint32_t)-1) {
            abstime = INT64_MAX;
        } else {
            now = nai_tickcache_to_usec();
            abstime = now + msec * 1000;
        };

        while (nai_list_is_empty(&p->msgs)) {

            /* switch to global lock */
            nai_mutex_lock(&s->mutex);
            nai_evmsg_port_unlock(p);

            /* waiting */
            r = nai_cond_timedwait(&p->cond, &s->mutex, abstime);
            if (r < 0) {
                ec = nai_errno;
            };

            /* switch to port lock */
            nai_mutex_unlock(&s->mutex);
            nai_evmsg_port_lock(p);

            if (r < 0) {
                nai_errno = ec;
                goto _end;
            };
        };
    };


    locking = 0;

    for ( ; !nai_list_is_empty(&p->msgs); ) {

        /* lock before handling messages */
        if (nai_evnode_get_loop(&p->ev) == 0 && !locking) {
            r = nai_evmsg_port_read_lock(p);
            if (r < 0) {
                ec = nai_errno;
                goto _fail;
            };

            locking = 1;
        };

        nai_list_init(&list);
        nai_list_add_tail(&list, &p->msgs);

        nai_evmsg_port_unlock(p);


        i = list.next;
        for ( ; i != &list; i = i->next) {
            m = (nai_evmsg_t*)i;
            e = m->hdr.e;
            if (e->token != m->hdr.token || 
                e->tag != NAI_EVMSG_TAG_ENTRY) {
                continue;
            };

            if (m->hdr.len <= (int32_t)sizeof(m->data)) {
                data = m->data;
            } else {
                data = m->ptr;
            };

            n = e->list.next;
            for ( ; n != &e->list; ) {
                h = (nai_evmsg_handle_t*)n;

                if (h->removed == 0) {
                    h->inusing = 1;
                    s->ops->handle_call(h, data, m->hdr.len);
                    h->inusing = 0;
                };

                n = n->next;

                if (h->removed) {
                    nai_list_entry_remove(&h->ent);
                    nai_list_insert_head(&list, &h->ent);
                    if (nai_list_is_empty(&e->list)) {
                        nai_evmsg_port_node_remove(p, e);
                    };
                };
            };
        };


        nai_evmsg_port_lock(p);

        nai_evmsg_port_free_handles(p, &list);
    };


    /* unlock before exit */
    if (locking) {
        nai_evmsg_port_read_unlock(p);
        locking = 0;
    };

    nai_pool_reset(&p->pool);

    nai_evmsg_port_unlock(p);

    r = 0;

_end:
    return r;

_fail:
    ec = nai_errno;
    nai_evmsg_port_unlock(p);
    nai_errno = ec;
    goto _end;
};


static nai_int_t nai_evmsg_port_read(nai_evmsg_port_t* p, uint32_t msec)
{
    nai_int_t r;
    nai_int_t ec;
    nai_int_t locking;
    nai_evmsg_serv_t* s;


    s = p->serv;
    if (s->ops->port_wait == 0) {
        r = nai_evmsg_port_read_impl(p, msec);
        goto _end;
    };

    /* custom read */
    locking = 0;
    while (1) {
        r = s->ops->port_wait(p, msec);
        if (r < 0) {
            ec = nai_errno;
            break;
        };
        if (r == 0) {
            break;
        };

        /* lock before handling messages */
        if (nai_evnode_get_loop(&p->ev) == 0 && !locking) {
            nai_evmsg_port_lock(p);

            r = nai_evmsg_port_read_lock(p);
            if (r < 0) {
                ec = nai_errno;
            };

            nai_evmsg_port_unlock(p);

            if (r < 0) {
                break;
            };

            locking = 1;
        };

        msec = 0;
        r = s->ops->port_read(p);
        if (r < 0) {
            ec = nai_errno;
            break;
        };
    };

    if (locking) {
        /* unlock after handling */
        nai_evmsg_port_lock(p);
        nai_evmsg_port_read_unlock(p);
        nai_evmsg_port_unlock(p);
    };

    if (r < 0) {
        nai_errno = ec;
    };

_end:
    return r;
};



static nai_int_t nai_evmsg_port_handle_add(
    nai_evmsg_port_t* p, nai_evmsg_handle_t* c, nai_list_t* flist)
{
    nai_int_t r;
    nai_int_t ec;
    nai_list_entry_t* i;
    nai_rbnode_t** n;
    nai_rbnode_t* parent;
    nai_evmsg_node_t* e;
    nai_evmsg_handle_t* h;


    n = nai_evmsg_port_find(p, c+1, &parent);
    if (n[0] != 0) {
        e = (nai_evmsg_node_t*)n[0];
        i = e->list.next;
        for ( ; i != &e->list; i = i->next) {
            h = (nai_evmsg_handle_t*)i;
            if (h->handle == c->handle) {
                break;
            };
        };
        if (i != &e->list) {
            h->removed = 0;
            nai_list_insert_tail(flist, &c->ent);
        } else {
            c->inusing = 0;
            c->removed = 0;
            nai_list_insert_tail(&e->list, &c->ent);
        };
    } else {
        /* allocate a new message node */
        e = (nai_evmsg_node_t*)nai_fixedpool_alloc(&p->poole);
        if (e == 0) {
            r = -1;
            goto _end;
        };

        p->token ++;
        if (p->token < 0) {
            p->token = 1;
        };

        /* add a new message node into map */
        e->share = 0;
        e->tag = NAI_EVMSG_TAG_ENTRY;
        e->token = p->token;
        e->port = p;
        r = nai_evmsg_port_node_open(p, e, c+1);
        if (r < 0) {
            ec = nai_errno;
            e->tag = NAI_EVMSG_TAG_NONE;
            e->token = 0;
            nai_fixedpool_free(&p->poole, e);
            nai_errno = ec;
            r = -1;
            goto _end;
        };

        /* add message node to map */
        nai_rbtree_link(&p->nmap, &e->node, parent, n);
        nai_rbtree_color(&p->nmap, &e->node);


        /* add message handle */
        c->inusing = 0;
        c->removed = 0;
        nai_list_init(&e->list);
        nai_list_insert_tail(&e->list, &c->ent);
    };

    r = 0;

_end:
    return r;
};


static nai_int_t nai_evmsg_port_handle_del(
    nai_evmsg_port_t* p, nai_evmsg_handle_t* c, nai_list_t* flist)
{
    nai_int_t r;
    nai_list_entry_t* i;
    nai_rbnode_t** n;
    nai_evmsg_node_t* e;
    nai_evmsg_handle_t* h;


    /* find message node in map */
    n = nai_evmsg_port_find(p, c+1, 0);
    if (n[0] == 0) {
        nai_list_insert_tail(flist, &c->ent);
        r = 0;
        goto _end;
    };

    /* remove message handle from message node */
    e = (nai_evmsg_node_t*)n[0];
    i = e->list.next;
    for ( ; i != &e->list; i = i->next) {
        h = (nai_evmsg_handle_t*)i;
        if (h->handle == c->handle) {
            if (h->inusing) {
                h->removed = 1;
            } else {
                nai_list_entry_remove(&h->ent);
                nai_list_insert_tail(flist, &h->ent);
            };
            break;
        };
    };

    /* try remove message node from map */
    if (nai_list_is_empty(&e->list)) {
        nai_evmsg_port_node_remove(p, e);
    };

    /* remove message handle */
    nai_list_insert_tail(flist, &c->ent);
    r = 0;

_end:
    return r;
};


static nai_int_t nai_evmsg_port_handle_done(nai_evmsg_port_t* p)
{
    nai_int_t r;
    nai_evmsg_serv_t* s;


    s = p->serv;
    if (nai_evnode_get_loop(&p->ev) == 0 && s->reading) {
        nai_mutex_lock(&s->mutex);

        if (s->cond) {
            nai_cond_signal(s->cond);
        };

        nai_mutex_unlock(&s->mutex);
    };

    r = 0;

    return r;
};


static nai_int_t nai_evmsg_port_handle_emit(
    nai_evmsg_port_t* p, nai_evmsg_handle_t* call)
{
    nai_int_t r;
    nai_int_t ec;
    nai_list_t list;
    nai_list_t free;
    nai_list_entry_t* e;
    nai_evmsg_handle_t* c;


    if (p->locking) {
        r = 0;
        goto _end;
    };

    p->locking = 1;

    while (!nai_list_is_empty(&p->pending)) {

        nai_list_init(&free);
        nai_list_init(&list);
        nai_list_add_tail(&list, &p->pending);

        nai_evmsg_port_unlock(p);


        e = list.next;
        for ( ; e != &list; ) {
            c = (nai_evmsg_handle_t*)e;
            e = e->next;

            nai_list_entry_remove(&c->ent);

            switch (c->op) {
            case NAI_EVMSG_ADD:
                r = nai_evmsg_port_handle_add(p, c, &free);
                if (r < 0) {
                    ec = nai_errno;

                    /* in calling, direct return */
                    if (call) {
                        goto _fail;
                    };

                    /* notify an error */
                    if (c->handle && c->handle->cb) {
                        c->handle->cb(c->handle, nai_ev_error_from(ec));
                    };
                    nai_list_insert_tail(&free, &c->ent);
                };
                break;

            case NAI_EVMSG_DEL:
                r = nai_evmsg_port_handle_del(p, c, &free);
                if (r < 0) {
                    assert(0);
                };
                break;

            default:
                assert(0);
                nai_list_insert_tail(&free, &c->ent);
                break;
            };
        };

        nai_evmsg_port_lock(p);

        nai_evmsg_port_free_handles(p, &free);
    };


    p->locking = 0;
    nai_evmsg_port_handle_done(p);
    r = 0;

_end:
    return r;

_fail:
    if (call == c) {
        nai_list_insert_tail(&free, &c->ent);
        r = -1;
    } else {
        nai_list_insert_tail(&list, &c->ent);
        r = 0;
    };

    nai_evmsg_port_lock(p);
    nai_list_add_head(&p->pending, &list);
    nai_evmsg_port_free_handles(p, &free);

    p->locking = 0;
    nai_evmsg_port_handle_done(p);

    if (r < 0) {
        nai_errno = ec;
    };
    goto _end;
};


static nai_int_t nai_evmsg_port_handle(nai_evnode_t* e, nai_int_t events)
{
    nai_int_t r;
    nai_int_t signum;
    nai_evmsg_port_t* p;


    p = nai_containof(e, nai_evmsg_port_t, ev);
    p->inusing = 1;

    if ((events & NAI_EV_MASK) == NAI_EV_NOTIFY) {
        signum = nai_ev_notify_code(events);
        switch (signum) {
        case NAI_EVMSG_PORT_READ:
            nai_evmsg_port_read(p, 0);
            break;
        case NAI_EVMSG_PORT_EMIT:
            nai_evmsg_port_lock(p);
            nai_evmsg_port_handle_emit(p, 0);
            nai_evmsg_port_unlock(p);
            break;
        case NAI_EVMSG_PORT_CLOSE:
            p->removed = 1;
            break;
        default:
            assert(0);
            break;
        };
    };

    p->inusing = 0;
    if (p->removed) {
        nai_evmsg_port_close(p);
        nai_free(p);
    };

    r = 0;

    return r;
};



//////////////////////////////////////////////////////////////////////////////
// event message service


nai_int_t nai_evmsg_serv_init(nai_evmsg_serv_t* s)
{
    nai_int_t r;


    nai_mutex_init(&s->mutex);
    nai_spin_init(&s->lock);
    nai_list_init(&s->ports);
    nai_rbtree_init(&s->nmap);
    nai_fixedpool_init(&s->poole, sizeof(nai_evmsg_node_t), 0, 0);
    s->token = 0;
    s->reading = 0;
    s->cond = 0;
    s->ops = 0;
    r = 0;

    return r;
};


nai_int_t nai_evmsg_serv_open(nai_evmsg_serv_t* s, nai_evmsg_ops_t* ops)
{
    nai_int_t r;
    nai_int_t ec;
    size_t size_node;


    r = nai_spin_open(&s->lock, 0);
    if (r < 0) {
        goto _end;
    };

    r = nai_mutex_open(&s->mutex, 0);
    if (r < 0) {
        goto _fail;
    };

    size_node = sizeof(nai_evmsg_node_t) + ops->size_node;
    nai_fixedpool_init(&s->poole, size_node, 0, 0);
    s->ops = ops;
    r = 0;

_end:
    return r;

_fail:
    ec = nai_errno;
    nai_spin_close(&s->lock);
    nai_mutex_close(&s->mutex);
    nai_errno = ec;
    goto _end;
};


nai_int_t nai_evmsg_serv_wait(nai_evmsg_serv_t* s, uint32_t msec)
{
    nai_int_t r;
    nai_int_t ec;
    nai_evmsg_port_t* p;


    /* get default port */
    p = nai_evmsg_port_get(s, 0, NAI_EVMSG_PORT_GET_NA);
    if (p == 0) {
        r = -1;
        goto _end;
    };


    /* mark reading */
    r = nai_atomic32_cas(&s->reading, 0, 1);
    if (r != 0) {
        /* release port */
        nai_evmsg_port_release(p);
        nai_errno = EBUSY;
        r = -1;
        goto _end;
    };


    /* read messages */
    r = nai_evmsg_port_read(p, msec);

    /* mark read finish */
    s->reading = 0;


    /* save errno */
    if (r < 0) {
        ec = nai_errno;
    };

    /* release port */
    nai_evmsg_port_release(p);

    /* restore errno */
    if (r < 0) {
        nai_errno = ec;
    };


_end:
    return r;
};


nai_int_t nai_evmsg_serv_send(
    nai_evmsg_serv_t* s, void* key, void* data, size_t len)
{
    nai_int_t r;


    nai_evmsg_serv_lock_map(s, 0);

    nai_evmsg_serv_send_locked(s, key, data, len);

    nai_evmsg_serv_unlock_map(s, 0);

    r = 0;

    return r;
};


nai_int_t nai_evmsg_serv_send_locked(
    nai_evmsg_serv_t* s, void* key, void* data, size_t len)
{
    nai_int_t r;
    nai_rbnode_t** n;
    nai_list_entry_t* i;
    nai_evmsg_node_t* g;
    nai_evmsg_node_t* e;


    n = nai_evmsg_serv_find(s, key, 0);
    if (n[0]) {
        g = (nai_evmsg_node_t*)n[0];
        i = g->list.next;
        if (s->ops->port_send) {
            for ( ; i != &g->list; i = i->next) {
                e = nai_containof(i, nai_evmsg_node_t, ent);
                if (e->share) {
                    s->ops->port_send(e->port, e, data, len);
                };
            };
        } else {
            for ( ; i != &g->list; i = i->next) {
                e = nai_containof(i, nai_evmsg_node_t, ent);
                if (e->share) {
                    nai_evmsg_port_send(e->port, e, data, len);
                };
            };
        };
    };

    r = 0;

    return r;
};


nai_int_t nai_evmsg_serv_sendto(
    nai_evmsg_serv_t* s, nai_evmsg_hdr_t* m, void* data)
{
    nai_int_t r;


    nai_evmsg_serv_lock_map(s, 0);

    nai_evmsg_serv_sendto_locked(s, m, data);

    nai_evmsg_serv_unlock_map(s, 0);

    r = 0;

    return r;
};


nai_int_t nai_evmsg_serv_sendto_locked(
    nai_evmsg_serv_t* s, nai_evmsg_hdr_t* m, void* data)
{
    nai_int_t r;
    nai_list_entry_t* i;
    nai_evmsg_node_t* g;
    nai_evmsg_node_t* e;


    g = m->e;
    if (g->token == m->token && 
        g->tag == NAI_EVMSG_TAG_ENTRY) {
        i = g->list.next;
        if (s->ops->port_send) {
            for ( ; i != &g->list; i = i->next) {
                e = nai_containof(i, nai_evmsg_node_t, ent);
                if (e->share) {
                    s->ops->port_send(e->port, e, data, m->len);
                };
            };
        } else {
            for ( ; i != &g->list; i = i->next) {
                e = nai_containof(i, nai_evmsg_node_t, ent);
                if (e->share) {
                    nai_evmsg_port_send(e->port, e, data, m->len);
                };
            };
        };
    };

    r = 0;

    return r;
};


nai_int_t nai_evmsg_serv_close(nai_evmsg_serv_t* s)
{
    nai_int_t r;


    if (nai_rbtree_begin(&s->nmap) != nai_rbtree_end(&s->nmap)) {
        nai_log_debug(NAI_LOG_CORE, 0, 
            "memory leak, "
            "there are legacy %s event nodes that are not released", 
            s->ops->name);

        assert(0);
    };
    if (!nai_list_is_empty(&s->ports)) {
        nai_log_debug(NAI_LOG_CORE, 0, 
            "memory leak, there are legacy %s event port "
            "and associated event loops that are not released", 
            s->ops->name);

        assert(0);
    };

    nai_mutex_close(&s->mutex);
    nai_spin_close(&s->lock);
    nai_fixedpool_close(&s->poole);
    nai_list_init(&s->ports);
    nai_rbtree_init(&s->nmap);
    s->token = 0;
    s->reading = 0;
    s->cond = 0;
    s->ops = 0;
    r = 0;

    return r;
};



//////////////////////////////////////////////////////////////////////////////
// event message port


nai_evmsg_port_t* nai_evmsg_port_get(
    nai_evmsg_serv_t* s, nai_evloop_t* l, nai_int_t na)
{
    nai_int_t r;
    nai_int_t ec;
    nai_evloop_t* d;
    nai_list_entry_t* e;
    nai_evmsg_port_t* p;


    nai_spin_lock(&s->lock);

    /* find port */
    e = s->ports.next;
    for ( ; e != &s->ports; e = e->next) {
        p = nai_containof(e, nai_evmsg_port_t, ents);
        d = nai_evnode_get_loop(&p->ev);
        if (d == l) {
            if (na <= NAI_EVMSG_PORT_GET_NA) {
                goto _done;
            } else {
                nai_spin_unlock(&s->lock);
                goto _end;
            };
        };
    };

    if (na) {
        ec = ENOENT;
        p = 0;
        goto _fail;
    };

    /* allocate port */
    p = (nai_evmsg_port_t*)nai_malloc(sizeof(*p) + s->ops->size_port);
    if (s == 0) {
        ec = nai_errno;
        goto _fail;
    };

    /* init port */
    r = nai_evmsg_port_init(p, s);
    if (r < 0) {
        ec = nai_errno;
        goto _fail;
    };

    /* open port */
    r = nai_evmsg_port_open(p, s, l);
    if (r < 0) {
        ec = nai_errno;
        goto _fail;
    };

    /* bind port to event loop */
    if (l != 0) {
        nai_spin_lock(&l->lock);
        nai_list_insert_tail(&l->ports, &p->entl);
        l->pcount ++;
        p->refs ++;
        nai_spin_unlock(&l->lock);
    };

    nai_list_insert_tail(&s->ports, &p->ents);


_done:
    p->refs ++;
    nai_spin_unlock(&s->lock);

_end:
    return p;

_fail:
    nai_spin_unlock(&s->lock);

    if (p != 0) {
        nai_free(p);
    };

    nai_errno = ec;
    p = 0;
    goto _end;
};


nai_int_t nai_evmsg_port_release(nai_evmsg_port_t* p)
{
    nai_int_t r;
    nai_evmsg_serv_t* s;


    s = p->serv;

    nai_spin_lock(&s->lock);

    p->refs --;
    if (p->refs <= 0) {
        nai_list_entry_remove(&p->ents);
    };

    nai_spin_unlock(&s->lock);

    if (p->refs <= 0) {
        r = nai_evmsg_port_close(p);
        if (r < 0) {
            goto _end;
        } else if (r > 0) {
            /* incomplete, do nothing */
            ;
        } else if (p->inusing) {
            /* in using, mark only */
            p->removed = 1;
        } else {
            nai_free(p);
        };
    };

    r = 0;

_end:
    return r;
};


nai_int_t nai_evmsg_port_rearm(nai_evmsg_port_t* p)
{
    nai_int_t r;
    nai_evmsg_serv_t* s;


    r = 0;
    s = p->serv;
    if (s->ops->port_rearm) {
        r = s->ops->port_rearm(p);
    };

    return r;
};


nai_evmsg_handle_t* nai_evmsg_port_emit_make(
    nai_evmsg_port_t* p, nai_evnode_t* e, nai_int_t op, void* key)
{
    nai_int_t ec;
    nai_evmsg_serv_t* s;
    nai_evmsg_handle_t* h;


    s = p->serv;

    nai_evmsg_port_lock(p);


    h = (nai_evmsg_handle_t*)nai_fixedpool_alloc(&p->poolh);
    if (h == 0) {
        ec = nai_errno;
        nai_evmsg_port_unlock(p);
        nai_errno = ec;
        goto _end;
    };

    h->op = op;
    h->handle = e;
    s->ops->handle_init(h, key);
    nai_list_insert_tail(&p->pending, &h->ent);


_end:
    return h;
};


nai_int_t nai_evmsg_port_emit_exec(
    nai_evmsg_port_t* p, nai_evmsg_handle_t* h)
{
    nai_int_t r;
    nai_int_t ec;
    nai_int_t inloop;


    /* already locked */

    inloop = nai_evnode_in_dispatch(&p->ev);
    if (inloop) {
        r = nai_evmsg_port_handle_emit(p, h);
        if (r < 0) {
            ec = nai_errno;
        };
    };


    nai_evmsg_port_unlock(p);


    if (inloop) {
        if (r < 0) {
            nai_errno = ec;
        };
    } else {
        nai_evnode_post(&p->ev, NAI_EVMSG_PORT_EMIT);
        r = 1;
    };

    return r;
};


nai_int_t nai_evmsg_port_emit_cancel(
    nai_evmsg_port_t* p, nai_evmsg_handle_t* h)
{
    nai_int_t r;


    /* already locked */

    nai_list_entry_remove(&h->ent);
    nai_fixedpool_free(&p->poolh, h);

    nai_evmsg_port_unlock(p);
    r = 0;

    return r;
};


nai_int_t nai_evmsg_port_emit(
    nai_evmsg_port_t* p, nai_evnode_t* e, nai_int_t op, void* key)
{
    nai_int_t r;
    nai_evmsg_handle_t* h;


    h = nai_evmsg_port_emit_make(p, e, op, key);
    if (h == 0) {
        r = -1;
        goto _end;
    };

    r = nai_evmsg_port_emit_exec(p, h);

_end:
    return r;
};


nai_int_t nai_evmsg_port_call_key(
    nai_evmsg_port_t* p, void* key, void* data, size_t len)
{
    nai_int_t r;
    nai_rbnode_t** n;
    nai_evmsg_node_t* e;
    nai_evmsg_hdr_t m;


    n = nai_evmsg_port_find(p, key, 0);
    if (n[0] == 0) {
        r = 0;
        goto _end;
    };

    e = (nai_evmsg_node_t*)n[0];
    m.e = e;
    m.token = e->token;
    m.len = (int32_t)len;
    r = nai_evmsg_port_call_msg(p, &m, data);

_end:
    return r;
};


nai_int_t nai_evmsg_port_call_msg(
    nai_evmsg_port_t* p, nai_evmsg_hdr_t* m, void* data)
{
    nai_int_t r;
    nai_list_t list;
    nai_list_entry_t* i;
    nai_evmsg_serv_t* s;
    nai_evmsg_node_t* e;
    nai_evmsg_handle_t* h;


    e = m->e;
    if (e->token != m->token || 
        e->tag != NAI_EVMSG_TAG_ENTRY) {
        r = 0;
        goto _end;
    };

    nai_list_init(&list);

    s = p->serv;
    i = e->list.next;
    for ( ; i != &e->list; ) {
        h = (nai_evmsg_handle_t*)i;
        i = i->next;

        if (h->removed == 0) {
            h->inusing = 1;
            s->ops->handle_call(h, data, m->len);
            h->inusing = 0;
        };

        if (h->removed) {
            nai_list_entry_remove(&h->ent);
            nai_list_insert_head(&list, &h->ent);
            if (nai_list_is_empty(&e->list)) {
                nai_evmsg_port_node_remove(p, e);
            };
        };
    };

    if (!nai_list_is_empty(&list)) {
        nai_evmsg_port_lock(p);
        nai_evmsg_port_free_handles(p, &list);
        nai_evmsg_port_unlock(p);
    };

    r = 0;

_end:
    return r;
};


nai_int_t nai_evmsg_port_once_key(
    nai_evmsg_port_t* p, void* key, void* data, size_t len)
{
    nai_int_t r;
    nai_int_t ec;


    p->inusing = 1;

    r = nai_evmsg_port_call_key(p, key, data, len);

    p->inusing = 0;
    if (p->removed) {
        if (r < 0) {
            ec = nai_errno;
        };

        nai_evmsg_port_close(p);
        nai_free(p);

        if (r < 0) {
            nai_errno = ec;
        };
    };

    return r;
};


nai_int_t nai_evmsg_port_once_msg(
    nai_evmsg_port_t* p, nai_evmsg_hdr_t* m, void* data)
{
    nai_int_t r;
    nai_int_t ec;


    p->inusing = 1;

    r = nai_evmsg_port_call_msg(p, m, data);

    p->inusing = 0;
    if (p->removed) {
        if (r < 0) {
            ec = nai_errno;
        };

        nai_evmsg_port_close(p);
        nai_free(p);

        if (r < 0) {
            nai_errno = ec;
        };
    };

    return r;
};


nai_int_t nai_evmsg_port_sendto(
    nai_evmsg_port_t* p, nai_evmsg_hdr_t* m, void* data)
{
    nai_int_t r;


    r = nai_evmsg_port_send(p, m->e, data, m->len);

    return r;
};


