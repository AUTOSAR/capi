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
/// @file       npc_port.c
/// @brief
/// @details
/// @date       2022-08-29
/// @author     xn
/// @version    1.2.0
///
/// ================================================================

#include "npc/shm/npc_port.h"
#include "npc/shm/npc_comm.h"
#include "npc/core/npc_message.h"
#include "npc/core/npc_log.h"
#include "nai/runtime/nai_list.h"
#include "nai/runtime/nai_util.h"
#include "nai/runtime/nai_errno.h"
#include <stdarg.h>
#include "npc/cpi/cpi_error.h"


#define NPC_MPORT_TAG           ('M' + ('P' << 8))


extern int npc_comif_release(npc_comif_t* p);
extern int npc_comif_release_hold(npc_comif_t* p, uint16_t peer);


extern npc_message_t* npc_message_create(npc_comif_t* p, int usage);
static npc_message_t* npc_port_unpack_message(npc_port_t* p, npc_mref_t msg);
static npc_mref_t npc_port_dopack_message(npc_port_t* p, npc_message_t* m);
static int npc_port_free_messages(npc_port_t* p, nai_list_t* list, int sent);
static int npc_port_dead(npc_port_t* p);
static int npc_port_seen(npc_port_t* p);
static int npc_port_skip(npc_port_t* p);
static int npc_port_queue(npc_port_t* p, npc_message_t* m);
static int npc_port_post(npc_port_t* p, int event, uint16_t peer);
static int npc_port_quit(npc_port_t* p);
static int npc_port_test(npc_port_t* p, uint16_t eid, uint16_t uid);
static int npc_port_boradcast(npc_port_t* p);


nai_rbnode_t** npc_port_find(
    npc_comif_t* p, npc_mref_t mref, nai_rbnode_t** pparent)
{
    nai_rbnode_t** n = &nai_rbtree_root(&p->ports);
    nai_rbnode_t* parent = nai_rbtree_end(&p->ports);
    npc_port_t* e;


    while (*n) {
        parent = *n;
        e = (npc_port_t*)parent;
        if (e->mref == mref) {
            break;
        } else if (e->mref >= mref) {
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



npc_port_t* npc_port_attach(
    npc_comif_t* q, npc_mref_t mref, int mode)
{
    int ec;
    uint16_t i = 0;
    size_t size;
    npc_comm_t* c;
    npc_port_t* p;
    npc_mport_t* m;
    npc_mref_t* mq;
    npc_mpeer_t* mp;
    nai_rbnode_t** n;
    nai_rbnode_t* parent;


    if (mode & ~(NPC_PORT_READ|NPC_PORT_SEND)) {
        nai_errno = EINVAL;
        p = 0;
        goto _end;
    };

    n = npc_port_find(q, mref, &parent);
    if (n[0] != 0) {
        p = nai_containof(n[0], npc_port_t, ent);
        if (p->closed) {
            nai_log_debug(NPC_LOG_CORE, 0, 
                "attach an exists port(0x%x, 0x%x)", 
                p->mref, p->mport->randid);

            p->closed = 0;
        } else {
            nai_errno = EEXIST;
            p = 0;
        };
        goto _end;
    };

    m = npc_shmz_hold(&q->shmz, mref);
    if (m == 0) {
        p = 0;
        goto _end;
    };

    if (m->tag != NPC_MPORT_TAG) {
        ec = EINVAL;
        goto _fail;
    };
    if (m->type & ~(NPC_PORT_READ|NPC_PORT_SEND)) {
        ec = EINVAL;
        goto _fail;
    };
    if (m->pcount <= 0 || m->qcount <= 0) {
        ec = EINVAL;
        goto _fail;
    };
    if (m->pstart != sizeof(*m)) {
        ec = EINVAL;
        goto _fail;
    };
    if (m->qstart != m->pstart + sizeof(*mp) * m->pcount) {
        ec = EINVAL;
        goto _fail;
    };

    mp = (npc_mpeer_t*)((uint8_t*)m + m->pstart);
    mq = (npc_mref_t*)((uint8_t*)m + m->qstart);


    if (mode & NPC_PORT_SEND) {
        size = sizeof(*p);

        if (q->self != m->source) {
            ec = EACCES;
            goto _fail;
        };
    } else {
        size = nai_offsetof(npc_port_t, in.stat);
        size += nai_alignup(m->qcount, 8) / 8;
        if (size < sizeof(*p)) {
            size = sizeof(*p);
        };

        for (i = 0; i < m->pcount; i ++) {
            if (mp[i].peer == q->self) {
                break;
            };
        };
        if (i >= m->pcount) {
            ec = EACCES;
            goto _fail;
        };
    };

    p = (npc_port_t*)nai_bufpool_alloc(&q->pool, size);
    if (p == 0) {
        ec = nai_errno;
        goto _fail;
    };

    p->cif = q;
    p->mref = mref;
    p->stat = 0;
    p->self = q->self;
    p->mode = mode | m->type;
    p->local = 0;
    p->mport = m;
    p->mpeer = mp;
    p->mqueue = mq;

    if (mode & NPC_PORT_SEND) {
        nai_list_init(&p->out.send);
        nai_list_init(&p->out.wait);
    } else {
        p->in.slot = i;
        p->in.last = mp[i].next;
        p->in.next = mp[i].next;
        nai_memset(p->in.stat, 0, 
            size - nai_offsetof(npc_port_t, in.stat));
    };


    nai_rbtree_link(&q->ports, &p->ent, parent, n);
    nai_rbtree_color(&q->ports, &p->ent);

    c = q->com;
    nai_spin_lock(&c->lock);
    q->refs ++;
    nai_spin_unlock(&c->lock);

    nai_log_debug(NPC_LOG_CORE, 0, 
        "attach a new port(0x%x, 0x%x)", p->mref, p->mport->randid);

_end:
    return p;

_fail:
    npc_shmz_unhold(&q->shmz, mref);
    nai_errno = ec;
    p = 0;
    goto _end;
};


npc_port_t* npc_port_create(
    npc_comif_t* q, uint16_t pcount, uint16_t qcount, int flags, ...)
{
    int ec;
    int peer;
    size_t size;
    npc_comm_t* c;
    npc_port_t* p;
    npc_mport_t* m;
    npc_mref_t* mq;
    npc_mpeer_t* mp;
    nai_rbnode_t** n;
    nai_rbnode_t* parent;
    va_list va;


    if (pcount <= 0) {
        nai_errno = EINVAL;
        p = 0;
        goto _end;
    };
    if (qcount <= 0 && qcount >= 0x8000) {
        nai_errno = EINVAL;
        p = 0;
        goto _end;
    };

    if (flags & NPC_PORT_SEND) {
        peer = q->self;
    } else {
        va_start(va, flags);
        peer = va_arg(va, int);
        va_end(va);

        if (peer < 0 || peer >= 0x10000) {
            nai_errno = EINVAL;
            p = 0;
            goto _end;
        };
    };


    size = sizeof(*m);
    size += pcount * sizeof(*mp);
    size += qcount * sizeof(*mq);

    m = (npc_mport_t*)npc_zalloc(&q->zone, size);
    if (m == 0) {
        p = 0;
        goto _end;
    };

    m->tag = NPC_MPORT_TAG;
    m->type = flags & ~(NPC_PORT_READ|NPC_PORT_SEND);
    m->randid = (uint16_t)(nai_rand32() & 0x7fff);
    m->source = peer;
    m->closed = 0;
    m->head = 0;
    m->last = 0;
    m->pstart = sizeof(*m);
    m->pcount = pcount;
    m->qstart = sizeof(*m) + sizeof(*mp) * pcount;
    m->qcount = qcount;
    mp = (npc_mpeer_t*)((uint8_t*)m + m->pstart);
    mq = (npc_mref_t*)((uint8_t*)m + m->qstart);

    nai_memset(mp, 0, sizeof(*mp) * m->pcount);
    nai_memset(mq, 0, sizeof(*mq) * m->qcount);


    if (flags & NPC_PORT_SEND) {
        size = sizeof(*p);
    } else {
        size = nai_offsetof(npc_port_t, in.stat);
        size += nai_alignup(qcount, 8) / 8;
        if (size < sizeof(*p)) {
            size = sizeof(*p);
        };
    };

    p = (npc_port_t*)nai_bufpool_alloc(&q->pool, size);
    if (p == 0) {
        goto _fail;
    };

    p->cif = q;
    p->mref = npc_shmz_2ref(&q->shmz, m);
    p->stat = 0;
    p->self = q->self;
    p->mode = flags;
    p->local = 1;
    p->mport = m;
    p->mpeer = mp;
    p->mqueue = mq;

    if (flags & NPC_PORT_SEND) {
        nai_list_init(&p->out.send);
        nai_list_init(&p->out.wait);
    } else {
        p->mpeer[0].peer = p->self;
        p->in.slot = 0;
        p->in.next = 0;
        p->in.last = 0;
        nai_memset(p->in.stat, 0, 
            size - nai_offsetof(npc_port_t, in.stat));
    };


    n = npc_port_find(q, p->mref, &parent);
    assert(n[0] == 0);
    nai_rbtree_link(&q->ports, &p->ent, parent, n);
    nai_rbtree_color(&q->ports, &p->ent);

    c = q->com;
    nai_spin_lock(&c->lock);
    q->refs ++;
    nai_spin_unlock(&c->lock);

    nai_log_debug(NPC_LOG_CORE, 0, 
        "create a new port(0x%x, 0x%x)", p->mref, p->mport->randid);

_end:
    return p;

_fail:
    ec = nai_errno;
    npc_zfree(&q->zone, m);
    nai_errno = ec;
    p = 0;
    goto _end;
};


int npc_port_close(npc_port_t* p)
{
    int r;
    uint16_t last;
    void* ptr;
    npc_mref_t* q;
    npc_mport_t* t;
    npc_comif_t* cif;


    cif = p->cif;
    if (p->mode & NPC_PORT_SEND) {
        /* check closable */
        if (!npc_port_dead(p) && (
            p->local || 
            p->mport->last != p->mport->head)) {
            if (!p->closed) {
                /* we need keep port until all messages are sent and 
                 * all reader is closed.
                 * mark closed.
                 */
                p->quit = 0;
                p->closed = 1;
            };

            nai_log_debug(NPC_LOG_CORE, 0, 
                "the port(0x%x, 0x%x) can't be closed, "
                "which with undead peer or inused messages", 
                p->mref, p->mport->randid);

            r = 0;
            goto _end;
        };

        if (p->mport->last != p->mport->head) {
            nai_log_warn(NPC_LOG_CORE, 0, 
                "the sending port has messages are sending");

            /* free packages */
            t = p->mport;
            q = p->mqueue;
            last = t->last;
            while (last != t->head) {
                ptr = npc_shmz_2ptr(&cif->shmz, q[last]);
                assert(ptr != 0);
                npc_zfree(&cif->zone, ptr);

                last ++;
                if (last >= t->qcount) {
                    last = 0;
                };
            };

        };

        /* close messages */
        npc_port_free_messages(p, &p->out.send, 0);
        npc_port_free_messages(p, &p->out.wait, 0);

        /* mark sender is closed */
        p->mport->closed = 1;

        /* tell server */
        if (!p->local) {
            r = npc_port_post(p, NPC_PORT_REMOVABLE, p->mpeer[0].peer);
            if (r < 0) {
                /* fixme: ignore error */
                ;
            };
        };

    } else {
        /* check closable */
        if (p->in.last != p->in.next || (
            p->local && !npc_port_dead(p))) {
            if (!p->closed) {
                if (p->in.last != p->in.next) {
                    nai_log_warn(NPC_LOG_CORE, 0, 
                        "the reading port has messages are reading");
                };

                /* we need keep port until all messages are closed and 
                 * the sender is closed.
                 * mark closed
                 */
                p->quit = 0;
                p->closed = 1;
            };

            nai_log_debug(NPC_LOG_CORE, 0, 
                "the port(0x%x, 0x%x) can't be closed, "
                "which with undead peer or inused messages", 
                p->mref, p->mport->randid);

            r = 0;
            goto _end;
        };

        /* mark reader is closed */
        p->mpeer[p->in.slot].closed = 1;

        /* tell server */
        if (!p->local) {
            r = npc_port_post(p, NPC_PORT_REMOVABLE, p->mport->source);
            if (r < 0) {
                /* fixme: ignore error */
                ;
            };
        };
    };

    nai_log_debug(NPC_LOG_CORE, 0, 
        "close the port(0x%x, 0x%x)", p->mref, p->mport->randid);

    /* clean holds */
    if (p->clean) {
        r = npc_comif_release_hold(cif, p->mport->source);
        if (r < 0) {
            nai_log_error(NPC_LOG_CORE, nai_errno, 
                "npc_comif_release_hold should not failed");

            /* fixme: ignore error */
            ;
        };
    };

    /* remove from comm interface */
    nai_rbtree_erase(&cif->ports, &p->ent);

    /* free port */
    if (p->local) {
        r = npc_zfree(&cif->zone, p->mport);
    } else {
        r = npc_shmz_unhold(&cif->shmz, p->mref);
    };
    if (r < 0) {
        nai_log_error(NPC_LOG_CORE, nai_errno, 
            "npc_zfree/npc_shmz_unhold should not failed");

        /* fixme: ignore error */
        ;
    };

    /* free */
    nai_bufpool_free(&cif->pool, p);

    /* release */
    npc_comif_release(cif);

    r = 0;

_end:
    return r;
};


int npc_port_state(npc_port_t* p, int state)
{
    int r;
    int ec;
    int changed = 0;


    switch (state) {
    case NPC_STATE_OPEN:
        if (p->quit) {
            p->quit = 0;
            changed = 1;
        };

        r = 0;
        break;

    case NPC_STATE_QUIT:
        if (p->quit == 0) {
            p->quit = 1;
            changed = 1;
        };

        if (p->mode & NPC_PORT_SEND) {
            /* is quitable */
            if (p->mport->last != p->mport->head) {
                ec = EAGAIN;
                r = -1;
            } else {
                r = 0;
            };
        } else {
            /* is quitable */
            if (p->in.last != p->in.next) {
                ec = EAGAIN;
                r = -1;
            } else {
                r = 0;
            };
        };
        break;

    default:
        assert(0);
        ec = EINVAL;
        r = -1;
        break;
    };

    if (changed) {
        npc_port_handle_state(p);
    };

    if (r < 0) {
        nai_errno = ec;
    };

    return r;
};


int npc_port_add(npc_port_t* p, uint16_t peer)
{
    int r;
    int n;
    int empty = -1;
    npc_mport_t* t;
    npc_mpeer_t* u;


    nai_log_debug(NPC_LOG_CORE, 0, 
        "add peer(%d) into the port(0x%x, 0x%x)", 
        peer, p->mref, p->mport->randid);

    if (!p->local) {
        nai_errno = EPERM;
        r = -1;
        goto _end;
    };

    /* find exist */
    t = p->mport;
    u = p->mpeer;
    for (n = 0; n < (int)t->pcount; n ++) {
        if (u[n].peer == peer) {
            if (u[n].closed) {
                empty = n;
                break;
            };

            nai_errno = EEXIST;
            r = -1;
            goto _end;
        };
        if (empty == -1 && u[n].peer == 0) {
            empty = n;
        };
    };

    if (empty == -1) {
        nai_errno = ERANGE;
        r = -1;
        goto _end;
    };

    /* add */
    u[empty].closed = 0;
    u[empty].next = t->head;
    nai_memory_barrier();
    u[empty].peer = peer;
    r = 0;

_end:
    return r;
};


int npc_port_remove(npc_port_t* p, uint16_t peer)
{
    int r;
    int n;
    npc_mport_t* t;
    npc_mpeer_t* u;


    nai_log_debug(NPC_LOG_CORE, 0, 
        "remove peer(%d) into the port(0x%x, 0x%x)", 
        peer, p->mref, p->mport->randid);


    if (!p->local) {
        nai_errno = EPERM;
        r = -1;
        goto _end;
    };


    /* find exist */
    t = p->mport;
    u = p->mpeer;
    for (n = 0; n < (int)t->pcount; n ++) {
        if (u[n].peer == peer) {
            break;
        };
    };
    if (n >= (int)t->pcount) {
        nai_errno = ENOENT;
        r = -1;
        goto _end;
    };


    /* remove */
    u[n].peer = 0;
    nai_memory_barrier();

    /* update sending queue */
    if (p->mode & NPC_PORT_SEND) {
        npc_port_handle_send(p);
    };

    r = 0;

_end:
    return r;
};

int npc_port_is_full(npc_port_t *p)
{
    int r = 0;
    uint16_t left;
    npc_mport_t* t;


    if (!(p->mode & NPC_PORT_SEND)) {
        nai_errno = EPERM;
        r = -1;
        goto _end;
    };
    if (p->quit || p->cif->quit) {
        nai_errno = EPERM;
        r = -1;
        goto _end;
    };


    t = p->mport;

    /* get left space of queue */
    if (t->head >= t->last) {
        left = t->last + (t->qcount - t->head);
    } else {
        left = t->last + (0 - t->head);
    };
    assert(left > 0);

    /* queue is full */
    if (left <= 1) {
        r = 1;
        goto _end;
    };

_end:
    return r;
}

int npc_port_send(npc_port_t* p, npc_message_t* m)
{
    int r;
    uint16_t left;
    npc_mport_t* t;


    if (!(p->mode & NPC_PORT_SEND)) {
        nai_errno = EPERM;
        r = -1;
        goto _end;
    };
    if (p->quit || p->cif->quit) {
        nai_errno = EPERM;
        r = -1;
        goto _end;
    };


    t = p->mport;

    /* get left space of queue */
    if (t->head >= t->last) {
        left = t->last + (t->qcount - t->head);
    } else {
        left = t->last + (0 - t->head);
    };
    assert(left > 0);

    /* queue is full */
    if (left <= 1) {
        nai_errno = EAGAIN;
        r = -1;
        goto _end;
    };


    r = npc_port_queue(p, m);
    if (r < 0) {
        goto _end;
    };

    r = npc_port_boradcast(p);
    if (r < 0) {
        nai_log_error(NPC_LOG_CORE, 
            nai_errno, "npc_port_boradcast failed");

        /* fixme: ignore error */
        r = 0;
    };


_end:
    return r;
};


npc_message_t* npc_port_read(npc_port_t* p)
{
    int ec;
    uint16_t i;
    npc_mref_t n;
    npc_mport_t* t;
    npc_message_t* m;


    if ((p->mode & NPC_PORT_SEND)) {
        nai_errno = EPERM;
        m = 0;
        goto _end;
    };
    if (p->quit || p->cif->quit) {
        nai_errno = EPERM;
        m = 0;
        goto _end;
    };

    t = p->mport;

    for (;;) {
        if (!(p->in.next < t->head) && 
            !(p->in.next > t->head)) {
            nai_errno = EAGAIN;
            m = 0;
            goto _end;
        };

        /* get message */
        n = p->mqueue[p->in.next];
        m = npc_port_unpack_message(p, n);
        if (m == 0) {
            nai_log_error(NPC_LOG_CORE, 
                nai_errno, "unpack message failed, skip it");
        } else {
            /* mark read stat */
            i = p->in.next;
            p->in.stat[i/8] |= (1 << (i&7));
            m->qindex = i;
        };

        /* update head of queue */
        p->in.next ++;
        if (p->in.next >= t->qcount) {
            p->in.next = 0;
        };

        if (m != 0) {
            break;
        };

        ec = nai_errno;
        if (ec != EEXIST) {
            break;
        };
    };

_end:
    return m;
};



typedef struct npc_mvec_s {
    npc_mref_t m;
    uint32_t len;
} npc_mvec_t;


typedef struct npc_mpacket_s {
    npc_msghdr_t hdr;
    uint8_t uid;
    uint16_t vcount;
    npc_mvec_t vec[1];
} npc_mpacket_t;


int npc_port_handle_read(npc_port_t* p)
{
    int r;
    npc_comif_t* cif;


    cif = p->cif;
    if (p->closed || 
        p->quit || cif->quit) {
        /* seek to head */
        npc_port_skip(p);
    } else {
        if (cif->ops) {
            cif->ops->read(cif->ctx, p);
        };
    };

    r = 0;

    return r;
};


int npc_port_handle_send(npc_port_t* p)
{
    int r;
    int ec;
    int n;
    int count;
    int sent;
    uint16_t i;
    uint16_t len;
    uint16_t left;
    uint16_t last;
    uint16_t tail;
    uint16_t inuse;
    void* ptr;
    npc_comif_t* cif;
    npc_mport_t* t;
    npc_mpeer_t* u;
    npc_mref_t* q;
    npc_message_t* m;
    nai_list_t list;
    nai_list_entry_t* e;


    t = p->mport;
    u = p->mpeer;

    /* scan all peer and count messages in use */
    inuse = 0;
    count = t->pcount;
    for (n = 0; n < count; n ++) {
        if (u[n].peer == 0) {
            continue;
        };

        i = u[n].next;
        if (i < t->head) {
            len = t->head + (0 - i);
        } else {
            len = t->head + (t->qcount - i);
        };
        if (inuse < len) {
            inuse = len;
        };
    };

    /* get new last pointer */
    if (inuse <= t->head) {
        tail = t->head + (0 - inuse);
    } else {
        tail = t->head + (t->qcount - inuse);
        if (tail >= t->qcount) {
            tail -= t->qcount;
        };
    };
    if (tail == t->last) {
        /* no sent message */
        r = 0;
        goto _end;
    };


    nai_list_init(&list);

    last = t->last;

    /* seek last pointer and move sent messages into temp list */
    cif = p->cif;
    q = p->mqueue;
    while (tail != last) {

        ptr = npc_shmz_2ptr(&cif->shmz, q[last]);
        assert(ptr != 0);
        npc_zfree(&cif->zone, ptr);

        last ++;
        if (last >= t->qcount) {
            last = 0;
        };

        e = p->out.send.next;
        m = (npc_message_t*)e;
        m->cookie = 0;
        nai_list_entry_remove(&m->ent);
        nai_list_insert_tail(&list, &m->ent);
    };

    t->last = last;


    /* post blocked messages */
    if (!(p->closed || p->quit || cif->quit)) {
        sent = 0;
        left = t->qcount - inuse;
        for ( ; left > 1; left --) {
            if (nai_list_is_empty(&p->out.wait)) {
                break;
            };

            e = p->out.wait.next;
            m = (npc_message_t*)e;
            nai_list_entry_remove(&m->ent);

            /* do post */
            r = npc_port_queue(p, m);
            if (r < 0) {
                ec = nai_errno;
                m->cookie = ec;
                nai_list_insert_tail(&list, &m->ent);
                nai_log_error(NPC_LOG_CORE, 
                    ec, "queue message into port failed");
            } else {
                sent = 1;
            };
        };


        /* boradcast to all peer */
        if (sent) {
            r = npc_port_boradcast(p);
            if (r < 0) {
                nai_log_error(NPC_LOG_CORE, 
                    nai_errno, "npc_port_boradcast failed");

                /* fixme: ignore error */
                ;
            };
        };
    };


    /* is quitable */
    if (t->last == t->head) {
        if (p->closed) {
            npc_port_close(p);
        } else if (p->quit) {
            npc_port_quit(p);
        } else {
            /* nothing */
            ;
        };
    };


    /* free sent message */
    npc_port_free_messages(p, &list, 1);

    r = 0;

_end:
    return r;
};


int npc_port_handle_seen(npc_port_t* p, npc_message_t* m)
{
    int r;
    int n;
    int count;
    uint16_t i;
    npc_comif_t* cif;
    npc_mpacket_t* pak;
    npc_mref_t* ref;
    uint8_t data[2048];


    assert(!(p->mode & NPC_PORT_SEND));

    /* test */
    i = m->qindex;
    if (!(p->in.stat[i/8] & (1 << (i&7)))) {
        /* already seen */
        nai_log_warn(NPC_LOG_CORE, 0, 
            "the message(0x%p, %d) has been seen", m, i);

        r = 0;
        goto _end;
    };

    cif = p->cif;
    pak = (npc_mpacket_t*)npc_shmz_2ptr(&cif->shmz, p->mqueue[i]);


    /* get reference */
    count = pak->vcount;
    ref = (npc_mref_t*)data;
    for (n = 0; n < count; n ++) {
        ref[n] = pak->vec[n].m;
    };

    /* unhold memory */
    npc_shmz_unhold(&cif->shmz, p->mqueue[n]);
    npc_shmz_unhold_array(&cif->shmz, ref, count);

    /* unmark read stat */
    p->in.stat[i/8] &= ~(1 << (i&7));


    /* move pointer */
    if (i == p->in.last) {
        npc_port_seen(p);
    };


    r = 0;

_end:
    return r;
};


int npc_port_handle_remove(npc_port_t* p)
{
    int r;
    int n;
    npc_mport_t* t;
    npc_mpeer_t* u;


    nai_log_debug(NPC_LOG_CORE, 0, 
        "handle removable event of the port(0x%x, 0x%x)", 
        p->mref, p->mport->randid);


    if (!p->local) {
        nai_errno = EPERM;
        r = -1;
        goto _end;
    };


    if (p->mode & NPC_PORT_SEND) {
        /* remove closed */
        t = p->mport;
        u = p->mpeer;
        for (n = 0; n < (int)t->pcount; n ++) {
            if (u[n].peer == 0) {
                continue;
            };

            /* remove */
            if (u[n].closed) {
                u[n].peer = 0;
            };
        };

        nai_memory_barrier();

        /* update sending queue */
        npc_port_handle_send(p);

    } else {
        if (p->closed) {
            npc_port_close(p);
        };
    };

    r = 0;

_end:
    return r;
};


int npc_port_handle_state(npc_port_t* p)
{
    int r;
    npc_comif_t* cif;


    nai_log_debug(NPC_LOG_CORE, 0, 
        "handle state changed of the port(0x%x, 0x%x)", 
        p->mref, p->mport->randid);


    cif = p->cif;
    if (p->mode & NPC_PORT_SEND) {
        if (cif->quit || p->quit) {
            /* clean messages */
            npc_port_free_messages(p, &p->out.wait, 0);
            ;
        } else if (!cif->quit && !p->quit) {
            /* nothing */
            ;
        };

    } else {
        if (cif->quit || p->quit) {
            /* nothing */
            ;
        } else if (!cif->quit && !p->quit) {
            /* seek to head */
            npc_port_skip(p);
        };
    };

    r = 0;

    return r;
};


static int npc_port_quit(npc_port_t* p)
{
    int r;
    npc_comif_t* cif;


    cif = p->cif;
    if (cif->ops) {
        r = cif->ops->quit(cif->ctx, p);
    } else {
        r = 0;
    };

    return r;
};


static int npc_port_test(npc_port_t* p, uint16_t eid, uint16_t uid)
{
    int r;
    npc_comif_t* cif;


    cif = p->cif;
    if (cif->ops) {
        r = cif->ops->test(cif->ctx, eid, uid);
    } else {
        r = 0;
    };

    return r;
};


static int npc_port_boradcast(npc_port_t* p)
{
    int r;
    int ec = 0;
    int n;
    int count;
    npc_mport_t* t;
    npc_mpeer_t* u;


    t = p->mport;
    u = p->mpeer;
    count = t->pcount;
    for (n = 0; n < count; n ++) {
        if (u[n].peer == 0) {
            continue;
        };

        r = npc_port_post(p, NPC_PORT_READABLE, u[n].peer);
        if (r < 0) {
            ec = nai_errno;
        };
    };

    if (ec != 0) {
        nai_errno = ec;
        r = -1;
    } else {
        r = 0;
    };

    return r;
};


static int npc_port_dead(npc_port_t* p)
{
    int r;
    int n;
    int count;
    npc_mpeer_t* u;
    npc_comif_t* cif;


    cif = p->cif;
    if (cif->ops == 0) {
        r = 1;
    } else if (p->mode & NPC_PORT_SEND) {
        u = p->mpeer;
        count = p->mport->pcount;
        for (n = 0; n < count; n ++) {
            if (u[n].closed) {
                continue;
            };
            if (u[n].peer == 0) {
                continue;
            };

            break;
        };

        r = n >= count;
    } else {
        r = p->mport->closed;
    };

    return r;
};


static int npc_port_post(npc_port_t* p, int event, uint16_t peer)
{
    int r;
    npc_comif_t* cif;
    npc_port_ev_t ev;


    cif = p->cif;
    if (cif->ops) {
        ev.event = event;
        ev.port = p->mref;
        r = cif->ops->post(cif->ctx, peer, ev.value);
    } else {
        r = 0;
    };

    return r;
};


static int npc_port_seen(npc_port_t* p)
{
    int r;
    int n;
    int last;
    int count;
    uint16_t i;
    npc_comif_t* cif;
    npc_mport_t* t;
    npc_mpeer_t* u;


    t = p->mport;
    for ( ; ; ) {
        p->in.last ++;
        if (p->in.last >= t->qcount) {
            p->in.last = 0;
        };

        if (p->in.last == p->in.next) {
            break;
        };

        i = p->in.last;
        if (p->in.stat[i/8] & (1 << (i&7))) {
            break;
        };
    };

    /* update next pointer */
    i = t->last;
    u = p->mpeer;
    last = u[p->in.slot].next == i;
    u[p->in.slot].next = p->in.last;
    nai_memory_barrier();

    /* test to reduce posting event */
    if (last) {
        cif = p->cif;
        if (cif->ops) {
            count = t->pcount;
            for (n = 0; n < count; n ++) {
                if (u[n].peer == 0) {
                    continue;
                };
                if (u[n].next == i) {
                    break;
                };
            };

            /* if all peer are already seen, post event */
            if (n >= (int)t->pcount) {
                npc_port_post(p, NPC_PORT_SENDABLE, t->source);
            };
        };
    };

    /* is quitable */
    if (p->in.last == p->in.next) {
        if (p->closed) {
            npc_port_close(p);
        } else if (p->quit) {
            npc_port_quit(p);
        } else {
            /* nothing */
            ;
        };
    };

    r = 0;

    return r;
};


static int npc_port_skip(npc_port_t* p)
{
    int r;
    uint16_t i;


    /* seek to head */
    p->in.next = p->mport->head;

    if (p->in.last != p->in.next) {
        i = p->in.last;
        if (!(p->in.stat[i/8] & (1 << (i&7)))) {
            npc_port_seen(p);
        };
    };

    r = 0;

    return r;
};


static int npc_port_queue(npc_port_t* p, npc_message_t* m)
{
    int r;
    uint16_t head;
    npc_mref_t pak;
    npc_mport_t* t;


    t = p->mport;

    /* packet message */
    pak = npc_port_dopack_message(p, m);
    if (pak == 0) {
        nai_log_error(NPC_LOG_CORE, nai_errno, 
            "pack message(%d) into port failed", m->hdr.len);

        r = -1;
        goto _end;
    };

    /* put into queue */
    head = t->head ;
    p->mqueue[head] = pak;

    /* update head of queue */
    nai_memory_barrier();
    head ++;
    if (head >= t->qcount) {
        head = 0;
    };
    t->head = head;

    /* queue to sending list */
    nai_list_insert_tail(&p->out.send, &m->ent);
    r = 0;

_end:
    return r;
};


static int npc_port_free_messages(npc_port_t* p, nai_list_t* list, int sent)
{
    int r;
    npc_message_t* m;
    nai_list_entry_t* e;


    (void)p;

    e = list->next;
    for ( ; e != list; ) {
        m = (npc_message_t*)e;
        e = e->next;
        m->sent = !!sent;
        nai_list_entry_remove(&m->ent);
        npc_message_close(m);
    };

    r = 0;

    return r;
};


static npc_mref_t npc_port_dopack_message(npc_port_t* p, npc_message_t* m)
{
    int n;
    int count;
    size_t size;
    nai_buf_t* b;
    nai_list_entry_t* e;
    npc_comif_t* cif;
    npc_mref_t r;
    npc_mref_t* ref;
    npc_mpacket_t* pak;
    void** ptr;
    uint32_t* len;
    uint8_t* buf;
    uint8_t* mem;
    uint8_t data[2048];


    n = 0;
    count = sizeof(data) / (sizeof(*ptr) + sizeof(*len) + sizeof(*ref));
    ptr = (void**)data;
    len = (uint32_t*)(ptr + count);
    ref = (npc_mref_t*)(len + count);
    e = m->payload.ent.next;
    for ( ; e != &m->payload.ent; ) {
        b = (nai_buf_t*)e;
        e = e->next;

        if (n >= count) {
            nai_log_warn(NPC_LOG_CORE, 0, 
                "the vector(%d +) of the message is too large", n);

            nai_errno = ERANGE;
            r = 0;
            goto _end;
        };

        size = nai_buf_size(b);
        if (size <= 0) {
            continue;
        };

        if (!nai_buf_in_memory(b)) {
            b = nai_buf_cut(m->payload.pool, b, size, 1);
            if (b == 0) {
                nai_log_alert(NPC_LOG_CORE, nai_errno, 
                    "failed to convert buffer(%d) into memory", (int)size);
                r = 0;
                goto _end;
            };
        };

        ptr[n] = nai_buf_ptr(b);
        len[n] = nai_buf_size(b);
        n ++;
    };

    /* pointer to reference */
    cif = p->cif;
    count = n;
    npc_shmz_2ref_array(&cif->shmz, ref, ptr, count);


    size = nai_offsetof(npc_mpacket_t, vec) + sizeof(npc_mvec_t) * count;
    for (n = 0; n < count; n ++) {
        if (ref[n] == 0) {
            size += len[n];
        };
    };

    /* allocate buffer of packet */
    buf = (uint8_t*)npc_zalloc(&cif->zone, size);
    if (buf == 0) {
        nai_log_alert(NPC_LOG_CORE, nai_errno, 
            "failed to allocate packet which need %d bytes", (int)size);

        r = 0;
        goto _end;
    };

    /* get reference */
    r = npc_shmz_2ref(&cif->shmz, buf);

    /* fill header */
    pak = (npc_mpacket_t*)buf;
    pak->hdr = m->hdr;
    pak->uid = m->unique_id;
    pak->vcount = count;

    /* fill buffer vector */
    mem = (uint8_t*)(buf + 
        nai_offsetof(npc_mpacket_t, vec) + sizeof(npc_mvec_t) * count);
    for (n = 0; n < count; n ++) {
        pak->vec[n].m = ref[n];
        pak->vec[n].len = len[n];
        if (pak->vec[n].m == 0) {
            pak->vec[n].m = r + (mem - buf);
            nai_memcpy(mem, ptr[n], len[n]);
            mem += len[n];
        };
    };


_end:
    return r;
};


static npc_message_t* npc_port_unpack_message(npc_port_t* p, npc_mref_t msg)
{
    int r;
    int ec;
    int n;
    int max;
    int count;
    nai_buf_t* b;
    npc_message_t* m;
    npc_comif_t* cif;
    npc_mpacket_t* pak;
    npc_mref_t* ref;
    void** ptr;
    uint8_t data[2048];


    cif = p->cif;
    pak = (npc_mpacket_t*)npc_shmz_hold(&cif->shmz, msg);
    if (pak == 0) {
        m = 0;
        goto _end;
    };

    max = sizeof(data) / (sizeof(*ptr) + sizeof(*ref));
    count = pak->vcount;
    if (count > max) {
        nai_log_warn(NPC_LOG_CORE, 0, 
            "the vector(%d) of the message is too large", pak->vcount);

        npc_shmz_unhold(&cif->shmz, msg);
        nai_errno = ERANGE;
        m = 0;
        goto _end;
    };

    if (pak->hdr.method == NPC_MT_NOTIFICATION && cif->ops) {
        r = npc_port_test(p, pak->hdr.method, pak->uid);
        if (r != 0) {
            npc_shmz_unhold(&cif->shmz, msg);
            nai_errno = EEXIST;
            m = 0;
            goto _end;
        };
    };

    ptr = (void**)data;
    ref = (npc_mref_t*)(ptr + count);
    for (n = 0; n < count; n ++) {
        ref[n] = pak->vec[n].m;
    };

    m = npc_message_create(cif, NPC_USAGE_READ);
    if (m == 0) {
        nai_log_alert(NPC_LOG_CORE, 
            nai_errno, "failed to create a read message");
        goto _fail;
    };

    r = npc_shmz_hold_array(&cif->shmz, ptr, ref, count);
    if (r < 0) {
        goto _fail;
    };

    m->port = p;
    m->hdr = pak->hdr;
    m->unique_id = pak->uid;
    for (n = 0; n < count; n ++) {
        b = nai_buf_from_rmemory(
            m->payload.pool, ptr[n], pak->vec[n].len, 0);
        if (b == 0) {
            nai_log_alert(NPC_LOG_CORE, 
                nai_errno, "failed to create a read buffer");
            goto _fail;
        };

        nai_buflist_insert_tail(&m->payload, b);
    };

_end:
    return m;

_fail:
    ec = nai_errno;

    if (m != 0) {
        npc_message_close(m);
        m = 0;
    };

    npc_shmz_unhold(&cif->shmz, msg);
    npc_shmz_unhold_array(&cif->shmz, ref, count);
    nai_errno = ec;
    goto _end;
};

