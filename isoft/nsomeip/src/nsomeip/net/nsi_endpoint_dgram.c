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
/// @file       nsi_endpoint_dgram.c
/// @brief
/// @details
/// @date       2021-05-11
/// @author     xn
/// @version    1.2.0
///
/// ================================================================

#include "nai/os/nai_proc.h"
#include "nsomeip/core/nsi_log.h"
#include "nsi_network.h"
#include "nai/io/nai_io.h"
#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_util.h"


#if (NAI_HAVE_UDP_SEGMENT)
#if (NAI_HAVE_NETINET_UDP_H)
#include "netinet/udp.h"
#endif
#endif


typedef struct nsi_endpoint_bind_s nsi_dgram_bind_t;


typedef struct nsi_dgram_s {
    /* header some as nsi_iobase_t */
    nai_dgram_t io;

    /* bind name */
    nai_rbnode_t node;
    nsi_endpoint_name_t name;

    /* endpoint */
    nsi_endpoint_t* ep;

    /* map of conn id to peername */
    nai_bufpool_t pool;
    nai_rbtree_t conns;
    nai_rbtree_t names;
    uint32_t nextc;


    /* flags and stats */
    union {
        struct {
            uint32_t server:1;
            uint32_t connected:1;
            uint32_t posted:1;
            uint32_t read:3;
            uint32_t send:3;
            uint32_t rebind:1;
            uint32_t retry:8;
        };
        uint32_t flags;
    };


    /* for read */
    struct {
        nai_buf_t* buf;             /**< pointer to read buffer */
        nai_list_entry_t queued;    /**< the list of tp-message */
        nsi_endpoint_name_t from;   /**< the address of incoming message */
        nsi_msghdr_t hdr;           /**< the message hdr of incoming message */
        nsi_msghdr_tp_t tp;         /**< the tp hdr of incoming message */
        uint32_t qcount;            /**< the count of queued tp-message */
        struct {
            nsi_serv_t serv;        /**< the service id */
            nsi_method_t method;    /**< the method id */
            int16_t sig;            /**< is signal */
        } evt;                      /**< the lookup cache of event */

        uint8_t gro;                /**< is enable gro */
    } in;


    /* for send */
    struct {
        nai_list_entry_t waiting;   /**< the list of waiting send messages */
        nai_bufwalk_t walk;         /**< the bufwalk */
        nsi_dgram_bind_t* to;       /**< pointer to the dest */
        size_t qsize;               /**< the queued size */
        uint32_t paylen;            /**< the left payload length */
        int16_t mcount;             /**< the count of messages in sending */
        int16_t vcount;             /**< the count of buffer in sending */
        int16_t scount;             /**< the count of segment in sending */
        int16_t snext;              /**< the next segment index */
        uint16_t ssize;             /**< the size of segment */
        struct {
            nsi_serv_t serv;        /**< the service id */
            nsi_inst_t inst;        /**< the instance id */
            nsi_method_t method;    /**< the method id */
            int8_t sig;             /**< is signal */
            int8_t is;              /**< current message is signal */
        } evt;                      /**< the lookup cache of event */

        uint8_t gso;                /**< is enable gso */
        uint8_t zerocopy;
        uint8_t znotified;
        uint8_t zwaiting;
        uint8_t zpending;

        union {
            nsi_message_t* msg;     /**< pointer to sending message */
            nsi_message_t* mv[8];   /**< the array of sending messages */
        };
        union {
            nsi_msghdr_t hv[8];     /**< the array of sending message hdr */
            struct {
                nsi_msghdr_t hdr;   /**< the message hdr of sending message */
                nsi_msghdr_tp_t tp; /**< the tp hdr of sending message */
            };
            struct {
                nsi_msghdr_t hdr;   /**< the message hdr of sending message */
                nsi_msghdr_tp_t tp; /**< the tp hdr of sending message */
            } tv[32];
        };

        nai_bufarray_t sa[32];      /**< the array of sending segment */
        nai_bufvec_t va[NAI_BUFV_MAX];
    } out;

} nsi_dgram_t;


//////////////////////////////////////////////////////////////////////////////
// dgram


/* PRS_SOMEIP_00724 */
/* PRS_SOMEIP_00729 */
#define NSI_MSG_TP_ALIGN            16u


#define NSI_DGRAM_MAX_SIZE          1400
#define NSI_DGRAM_SEND_NONE         0
#define NSI_DGRAM_MAKE_MSG          1
#define NSI_DGRAM_MAKE_TP_MSG       2
#define NSI_DGRAM_SEND_MSG          3


#define NSI_DGRAM_TIME              (1 * 1000)
#define NSI_DGRAM_TIME_TP           (NSI_DGRAM_TIME)
#define NSI_DGRAM_TIME_BIND         (120 * 1000)


#define nsi_dgram_bind_find_conn    nsi_endpoint_bind_find_conn
#define nsi_dgram_bind_find_name    nsi_endpoint_bind_find_name


#define nsi_message_time(m)         *nsi_message_time_ptr(m)

#define nsi_message_last_buf(m)     *nsi_message_last_buf_ptr(m)


static uint64_t* nsi_message_time_ptr(nsi_message_t* m)
{
    /* member 'cred' of message is aligned 8 bytes, use to store time */
    assert((nai_offsetof(nsi_message_t, cred) & (sizeof(void*)-1)) == 0);
    return (uint64_t*)&m->cred;
};


static nai_buf_t** nsi_message_last_buf_ptr(nsi_message_t* m)
{
    /* member 'cred' of message is aligned 8 bytes, use to store time */
    assert((nai_offsetof(nsi_message_t, cred) & (sizeof(void*)-1)) == 0);
    return (nai_buf_t**)((char*)&m->cred + sizeof(nai_buf_t*));
};


static int nsi_dgram_bind(nsi_dgram_t* c);


static nsi_dgram_bind_t* nsi_dgram_add_bind(
    nsi_dgram_t* c, const nai_sockaddr_t* name, int namelen)
{
    int cid;
    nai_rbnode_t** n;
    nai_rbnode_t* parent;
    nsi_dgram_bind_t* b;


    if (name->sa_family != c->name.addr->sa_family) {
        nai_errno = EINVAL;
        b = 0;
        goto _end;
    };

    n = nsi_dgram_bind_find_name(&c->names, name, namelen, &parent);
    if (n[0]) {
        b = nai_containof(n[0], nsi_dgram_bind_t, entn);
    } else {
        b = (nsi_dgram_bind_t*)
            nai_bufpool_xalloc(&c->pool, sizeof(*b) + namelen);
        if (b == 0) {
            nai_log_alert(NSI_LOG_CORE, 
                nai_errno, "failed to allocate bind name");
            goto _end;
        };

        b->refs = 0;
        b->mcast = 0;
        b->banned = -1;
        b->uid = 0;
        b->name.len = namelen;
        b->name.addr = (nai_sockaddr_t*)(b + 1);
        nai_memcpy(b->name.addr, name, namelen);
        nai_rbtree_link(&c->names, &b->entn, parent, n);
        nai_rbtree_color(&c->names, &b->entn);

        /* allocate connection id */
        while (1) {
            cid = ++ c->nextc;
            if (cid <= 0) {
                c->nextc = 0;
                continue;
            };

            n = nsi_dgram_bind_find_conn(&c->conns, cid, &parent);
            if (n[0]) {
                /* connection id exists */
                continue;
            };

            b->cid = cid;
            nai_rbtree_link(&c->conns, &b->entc, parent, n);
            nai_rbtree_color(&c->conns, &b->entc);
            break;
        };
    };


_end:
    return b;
};


static nsi_dgram_bind_t* nsi_dgram_get_bind(
    nsi_dgram_t* c, const nai_sockaddr_t* name, int namelen)
{
    nai_rbnode_t** n;
    nsi_dgram_bind_t* b;


    n = nsi_dgram_bind_find_name(&c->names, name, namelen, 0);
    if (n[0] == 0) {
        b = 0;
        goto _end;
    };

    b = nai_containof(n[0], nsi_dgram_bind_t, entn);

_end:
    return b;
};


static int nsi_dgram_incoming_is_signal(nsi_dgram_t* c, nsi_msghdr_t* h)
{
    int r;


    if (c->in.evt.serv != h->serv || 
        c->in.evt.method != h->method) {
        c->in.evt.sig = nsi_endpoint_is_signal(
            c->ep, h->serv, NSI_INSTANCE_ANY, h->method);
        c->in.evt.serv = h->serv;
        c->in.evt.method = h->method;
    };

    r = c->in.evt.sig;

    return r;
};


static int nsi_dgram_message_is_signal(nsi_dgram_t* c, nsi_message_t* m)
{
    int r;


    if (c->out.evt.serv != m->hdr.serv || 
        c->out.evt.inst != m->inst || 
        c->out.evt.method != m->hdr.method) {
        c->out.evt.sig = nsi_endpoint_is_signal(
            c->ep, m->hdr.serv, m->inst, m->hdr.method);
        c->out.evt.serv = m->hdr.serv;
        c->out.evt.inst = m->inst;
        c->out.evt.method = m->hdr.method;
    };

    r = c->out.evt.sig;

    return r;
};


static int nsi_dgram_realloc_buf(nsi_dgram_t* c)
{
    return nsi_endpoint_realloc_buf(c->ep, &c->in.buf);
};


static int nsi_dgram_handle_connect(nsi_dgram_t* c, nsi_connid_t cid)
{
    return nsi_endpoint_handle_connect(c->ep, cid);
};


static int nsi_dgram_handle_incoming(nsi_dgram_t* c, nsi_message_t* m)
{
    return nsi_endpoint_handle_incoming(c->ep, m);
};


static int nsi_dgram_handle_sent(nsi_dgram_t* c, int errcode)
{
    int n;
    nsi_message_t* m;
    nai_list_entry_t list;

    nai_list_init(&list);

    for (n = 0; n < c->out.mcount; n ++) {
        m = c->out.mv[n];
        assert(m != 0);
        nai_list_entry_remove(&m->ent);
        nai_list_insert_tail(&list, &m->ent);
    };

    c->out.msg = 0;
    c->out.mcount = 0;
    c->send = NSI_DGRAM_SEND_NONE;
    return nsi_endpoint_handle_sent(c->ep, &list, &c->out.qsize, errcode);
};


static int nsi_dgram_handle_alert(nsi_dgram_t* c, int errcode)
{
    return nsi_endpoint_handle_except(c->ep, 0, errcode, 1);
};


static int nsi_dgram_handle_error(nsi_dgram_t* c, int errcode)
{
    return nsi_endpoint_handle_except(c->ep, 0, errcode, 0);
};


static int nsi_dgram_assemble_tp(
    nsi_msghdr_t* h, nsi_msghdr_tp_t* t, uint8_t* buf, size_t size)
{
    int r;
    uint32_t cpy;
    uint32_t len;
    uint32_t off;
    uint8_t* dst;
    nsi_msghdr_t hdr;
    nsi_msghdr_tp_t tp;


    r = 0;
    len = h->len - NSI_MSGEXT_SIZE - sizeof(tp);
    off = t->off;
    dst = buf;

    for ( ; ; ) {
        if (size < (sizeof(hdr) + sizeof(tp))) {
            break;
        };

        nsi_msghdr_ntoh_ua(&hdr, (nsi_msghdr_t*)buf);

        /* test type */
        if (!nsi_msgtype_is_tp(hdr.type)) {
            break;
        };

        /* test size */
        if (size < hdr.len + NSI_MSGHDR_SIZE) {
            break;
        };

        /* test header */
        if (h->serv != hdr.serv || 
            h->method != hdr.method || 
            h->client != hdr.client || 
            h->session != hdr.session) {
            break;
        };

        /* get tp offset and flags. */
        tp.info = nai_ntohl_ua((uint32_t*)(buf + sizeof(hdr)));

        /* test offset */
        /* PRS_SOMEIP_00724 */
        /* PRS_SOMEIP_00729 */
        if (tp.off * NSI_MSG_TP_ALIGN != off * NSI_MSG_TP_ALIGN + len) {
            break;
        };


        /* do memcpy */
        cpy = hdr.len - NSI_MSGEXT_SIZE - sizeof(tp);
        nai_memcpy(dst, buf + sizeof(hdr) + sizeof(tp), cpy);


        /* move pointers */
        dst += cpy;
        len += cpy;
        buf += hdr.len + NSI_MSGHDR_SIZE;
        size -= hdr.len + NSI_MSGHDR_SIZE;
        r ++;

        /* test is last one */
        if (tp.more == 0) {
            t->more = 0;
            break;
        };
    };

    h->len = len + NSI_MSGEXT_SIZE + sizeof(tp);

    return r;
};


static int nsi_dgram_handle_read(nsi_dgram_t* c)
{
    int ec;
    int namelen;
    int is_signal;
    intptr_t r;
    size_t size;
    size_t read;
    size_t total;
    uint32_t mss;
    uint32_t gro;
    uint32_t cid;
    uint64_t now;
    nai_buf_t* b;
    nai_buf_t* d;
    nai_buf_t* p;
    nai_list_entry_t* e;
    nsi_dgram_bind_t* a;
    nsi_network_t* n;
    nsi_endpoint_t* ep;
    nsi_message_t* m;
    nsi_msghdr_t* h;
    nsi_msghdr_t hdr;
    nsi_msghdr_tp_t tp;
    nai_bufvec_t va[2];
    char buf[140];


    if (c->in.buf == 0) {
        r = nsi_dgram_realloc_buf(c);
        if (r < 0) {
            ec = nai_errno;
            nai_log_alert(NSI_LOG_CORE, ec, 
                "dgram endpoint(%s) failed to create read buffer", 
                nai_str(&c->ep->host));
            goto _alert;
        };
    };

    ep = c->ep;
    n = ep->net;
    now = 0;
    mss = n->mss;
    gro = n->recv_segs;
    gro = mss * (c->in.gro ? gro : 1);

    b = c->in.buf;
    va[0].buf = (uint8_t*)&c->in.hdr;
    va[0].len = sizeof(c->in.hdr) + sizeof(c->in.tp);
    while (1) {
        total = nai_buf_total(b);
        assert(total >= n->mss);

        va[1].buf = nai_buf_ptr(b) + va[0].len;
        va[1].len = total - va[0].len;
        namelen = c->in.from.len;
        r = nai_dgram_recvm(
            &c->io, va, 2, 0, c->in.from.addr, &namelen, 0, 0);
        if (r < 0) {
            ec = nai_errno;
            if (ec == NAI_EAGAIN || ec == NAI_EINPROGRESS) {
                r = NAI_DECLINED;
                goto _end;
            };

            nai_log_crit(NSI_LOG_CORE, 
                ec, "dgram endpoint(%s) recvive failed", 
                nai_str(&c->ep->host));
            goto _error;
        };

        cid = -1;
        size = 0;
        read = r;

        /* try to handle multi-messages in one dgram, 
         * PRS_SOMEIP_00140
         */
        for (;;) {
            read -= size;
            if (read <= 0) {
                /* no buffer */
                break;
            };

            size = read;
            if (size < NSI_MSGHDR_SIZE) {
                nai_log_warn(NSI_LOG_CORE, EINVAL, 
                    "dgram endpoint(%s) get an invalid message is too small", 
                    nai_str(&c->ep->host));

                /* drop whole buffer */
                break;
            };

            if (cid == (nsi_connid_t)-1) {
                h = &c->in.hdr;
                nsi_msghdr_ntoh(&hdr, h);
            } else {
                h = (nsi_msghdr_t*)nai_buf_ptr(b);
                nsi_msghdr_ntoh_ua(&hdr, h);
            };


            /* handle message size */
            if (hdr.len != size - NSI_MSGHDR_SIZE) {
                if (hdr.len > size - NSI_MSGHDR_SIZE) {
                    /* a truncated data, drop */
                    nai_log_warn(NSI_LOG_CORE, EINVAL, 
                        "dgram endpoint(%s) get a truncated message, drop it", 
                        nai_str(&c->ep->host));

                    /* drop whole buffer */
                    break;
                };

                size = hdr.len + NSI_MSGHDR_SIZE;
            };


            /* handle signal */
            is_signal = nsi_dgram_incoming_is_signal(c, &hdr);
            if (is_signal) {
                /* keep the same length as a normal head */
                hdr.len += NSI_MSGEXT_SIZE - NSI_SIGEXT_SIZE;
                hdr.client = 0;
                hdr.session = 0;
                hdr.type = NSI_MT_NOTIFICATION;
                hdr.code = 0;
                hdr.protocol = -1;
                hdr.interface = -1;
            } else {
                if (size < sizeof(hdr)) {
                    nai_log_warn(NSI_LOG_CORE, EINVAL, 
                        "dgram endpoint(%s) "
                        "get an invalid message is too small", 
                        nai_str(&c->ep->host));

                    /* drop whole buffer */
                    break;
                };
            };


            /* get current time */
            if (now == 0) {
                now = nai_tickcache_to_msec();
            };

            if (cid == (nsi_connid_t)-1) {
                /* server: add into address */
                if (c->server) {
                    a = nsi_dgram_add_bind(c, c->in.from.addr, namelen);
                    if (a == 0) {
                        ec = nai_errno;
                        nai_log_error(NSI_LOG_CORE, ec, 
                            "dgram server endpoint(%s) add name failed", 
                            nai_str(&c->ep->host));
                        r = -1;
                        goto _alert;
                    };

                    a->time = now;

                    /* check banned address */
                    if (a->banned == (uint8_t)-1) {
                        if (c->ep->local) {
                            a->banned = 0;
                        } else {
                            a->banned = !nsi_endpoint_is_subnet(
                                c->ep, &c->in.from);
                        };
                    };
                    if (a->banned) {
                        if (nai_log_is_enabled_debug(&nsi_log_core)) {
                            buf[0] = 0;
                            nai_sockaddr_ntop(c->in.from.addr, 
                                c->in.from.len, buf, sizeof(buf), 0);
                            nai_log_debug(NSI_LOG_CORE, 0, 
                                "dgram server endpoint(%s) reject dgram "
                                "from %s", nai_str(&c->ep->host), buf);
                        };

                        /* drop whole buffer */
                        break;
                    };

                    cid = a->cid;

                /* client: find connection id by address */
                } else if (nsi_endpoint_name_equal(&c->ep->name, &c->in.from)) {
                    cid = 0;
                    a = 0;
                } else {
                    a = nsi_dgram_get_bind(c, c->in.from.addr, namelen);
                    if (a == 0) {
                        nai_log_info(NSI_LOG_CORE, 0, 
                            "dgram client endpoint(%s) "
                            "receive a message from unknown address", 
                            nai_str(&c->ep->host));

                        /* drop whole buffer */
                        break;
                    };

                    a->time = now;
                    cid = a->cid;
                };
            };

            b->size = 0;
            nai_buf_wcommit(b, size);

            if (nsi_msgtype_is_tp(hdr.type)) {

                if (hdr.len < NSI_MSGEXT_SIZE + sizeof(tp)) {
                    nai_log_warn(NSI_LOG_CORE, EINVAL, 
                        "dgram endpoint(%s) get a truncated tp message,"
                        "drop it", nai_str(&c->ep->host));

                    /* drop whole buffer */
                    break;
                };

                hdr.type = nsi_msgtype_from_tp(hdr.type);

                /* find exists tp-message, different conn/client/session
                 * PRS_SOMEIP_00740
                 */
                e = c->in.queued.next;
                for ( ; e != &c->in.queued; e = e->next) {
                    m = (nsi_message_t*)e;
                    if (m->conn == cid && 
                        m->hdr.serv == hdr.serv && 
                        m->hdr.method == hdr.method && 
                        m->hdr.client == hdr.client && 
                        m->hdr.session == hdr.session) {
                        break;
                    };
                };
                if (e != &c->in.queued) {
                    if (m->hdr.type != hdr.type) {
                        nai_log_warn(NSI_LOG_CORE, EINVAL, 
                            "dgram endpoint(%s) get a tp message "
                            "which's type(%d) is different from "
                            "the type(%d) of exists one, drop it", 
                            nai_str(&c->ep->host), hdr.type, m->hdr.type);

                        /* mark buffer in using */
                        nai_buf_rcommit(b, size);

                        /* process left buffer */
                        continue;
                    };
                } else {
                    if (c->in.qcount >= n->tpm_max) {
                        nai_log_warn(NSI_LOG_CORE, nai_errno, 
                            "dgram endpoint(%s) get too many tp message, "
                            "drop oldest one", 
                            nai_str(&c->ep->host));

                        m = (nsi_message_t*)c->in.queued.prev;
                        nai_list_entry_remove(&m->ent);
                        nsi_message_close(m);
                        c->in.qcount --;
                    };

                    m = nsi_network_create_message(n, NSI_USAGE_READ);
                    if (m == 0) {
                        ec = nai_errno;
                        nai_log_alert(NSI_LOG_CORE, ec, 
                            "dgram endpoint(%s) failed to create tp message", 
                            nai_str(&c->ep->host));
                        goto _alert;
                    };

                    m->conn = cid;
                    m->hdr = hdr;
                    m->hdr.len = NSI_MSGEXT_SIZE;
                    nsi_message_last_buf(m) = 0;

                    if (nai_list_is_empty(&c->in.queued)) {
                        r = nai_dgram_set_timeout(
                            &c->io, NAI_TIMEOP_SET, NSI_DGRAM_TIME);
                        if (r < 0) {
                            ec = nai_errno;
                            nai_log_error(NSI_LOG_CORE, ec, 
                                "dgram endpoint(%s) set timeout failed", 
                                nai_str(&c->ep->host));
                            goto _alert_msg;
                        };
                    };

                    nai_list_insert_head(&c->in.queued, &m->ent);
                    c->in.qcount ++;
                };

                /* update tp-message expire time */
                nsi_message_time(m) = now;


                /* get tp offset and flags. */
                tp.info = nai_ntohl_ua((uint32_t*)(h+1));

                /* small packets, do fast assemble tp-message, 
                 * guess that multiple consecutive small packets(<1440) are 
                 * currently received.
                 * the overhead of splicing small packets by copying will be 
                 * lower than directly appending to the buffer list of the 
                 * message, and reduce the overhead of traversing 
                 * the buffer list when the message is subsequently processed.
                 */
                if (gro && tp.more && read > size) {
                    r = nsi_dgram_assemble_tp(
                        &hdr, &tp, nai_buf_ptr(b) + size, read - size);
                    if (r > 0) {
                        size = hdr.len + NSI_MSGHDR_SIZE;
                        size += r * (sizeof(hdr) + sizeof(tp));
                        nai_buf_wcommit(b, size - nai_buf_size(b));
                    };
                };

                p = nsi_message_last_buf(m);
                if (p && p->ref.buf == b) {
                    /* create buffer for reduce call nai_atomic32_inc/dec */
                    d = nai_buf_from_rmemory(m->payload.pool, 
                        nai_buf_ptr(b) + sizeof(hdr) + sizeof(tp), 
                        hdr.len - NSI_MSGEXT_SIZE - sizeof(tp), 0);
                    if (d == 0) {
                        ec = nai_errno;
                        nai_log_alert(NSI_LOG_CORE, ec, 
                            "dgram endpoint(%s) create tp buffer failed", 
                            nai_str(&c->ep->host));
                        nai_list_entry_remove(&m->ent);
                        goto _alert_msg;
                    };

                    /* set reference buffer */
                    d->typeref = NAI_BUF_REF;
                    d->ref.buf = p;
                    p->refcount ++;
                } else {
                    /* create a sub-buffer to reference data */
                    d = nai_buf_sub(m->payload.pool, b, 
                        sizeof(hdr) + sizeof(tp), 
                        hdr.len - NSI_MSGEXT_SIZE - sizeof(tp), 1);
                    if (d == 0) {
                        ec = nai_errno;
                        nai_log_alert(NSI_LOG_CORE, ec, 
                            "dgram endpoint(%s) create tp sub-buffer failed", 
                            nai_str(&c->ep->host));
                        nai_list_entry_remove(&m->ent);
                        goto _alert_msg;
                    };
                };

                /* mark buffer in using */
                nai_buf_rcommit(b, size);

                /* big packet, do slow assemble tp-message */
                /* PRS_SOMEIP_00724 */
                /* PRS_SOMEIP_00729 */
                r = nsi_message_assemble_tp(
                    m, d, tp.off * NSI_MSG_TP_ALIGN, tp.more);
                if (r < 0) {  /* tp-message is error */
                    nai_log_warn(NSI_LOG_CORE, nai_errno, 
                        "dgram endpoint(%s) get an error tp message, drop it", 
                        nai_str(&c->ep->host));

                    nai_list_entry_remove(&m->ent);
                    nsi_message_close(m);
                    c->in.qcount --;

                    /* process left buffer */
                    continue;
                };

                /* save last buffer for reduce call nai_atomic32_inc/dec */
                if (d->ref.buf == b && 
                    m->payload.ent.prev == &d->ent) {
                    nsi_message_last_buf(m) = d;
                };

                /* need more parts */
                if (r == 0) { 
                    if (m->hdr.len > n->tpm_limit) {
                        ec = EMSGSIZE;
                        nai_log_alert(NSI_LOG_CORE, ec, 
                            "dgram endpoint(%s/%d) "
                            "get an tp message with large size(%d), drop it", 
                            nai_str(&c->ep->host), cid, hdr.len);

                        nai_list_entry_remove(&m->ent);
                        nsi_message_close(m);
                        c->in.qcount --;
                    };

                    /* process left buffer */
                    continue;
                };

                /* tp-message received complete */
                nai_list_entry_remove(&m->ent);
                c->in.qcount --;

            } else {
                /* alloc a message */
                m = nsi_network_create_message(n, NSI_USAGE_READ);
                if (m == 0) {
                    ec = nai_errno;
                    nai_log_alert(NSI_LOG_CORE, ec, 
                        "dgram endpoint(%s) failed to create message", 
                        nai_str(&c->ep->host));
                    goto _alert;
                };


                /* create a sub-buffer to reference data */
                if (is_signal) {
                    nai_memcpy(nai_buf_ptr(b) + NSI_MSGHDR_SIZE, 
                        (char*)h + NSI_MSGHDR_SIZE, 
                        sizeof(hdr) + sizeof(tp) - NSI_MSGHDR_SIZE);

                    d = nai_buf_sub(
                        m->payload.pool, b, 
                        NSI_MSGHDR_SIZE, hdr.len - NSI_MSGEXT_SIZE, 1);
                } else {
                    nai_memcpy(nai_buf_ptr(b) + sizeof(hdr), 
                        h + 1, sizeof(tp));

                    d = nai_buf_sub(
                        m->payload.pool, b, 
                        sizeof(hdr), hdr.len - NSI_MSGEXT_SIZE, 1);
                };
                if (d == 0) {
                    ec = nai_errno;
                    nai_log_alert(NSI_LOG_CORE, ec, 
                        "dgram endpoint(%s) failed to create sub-buffer", 
                        nai_str(&c->ep->host));
                    goto _alert_msg;
                };


                m->conn = cid;
                m->hdr = hdr;
                nai_buflist_insert_tail(&m->payload, d);
                nai_buf_rcommit(b, size);
            };


            m->inst = 0;
            m->flags = 0;
            if (cid != 0) {
                m->cred.type = NSI_CRED_ADDRESS;
                m->cred.len = a->name.len;
                nai_memcpy(&m->cred.addr, a->name.addr, a->name.len);
            } else {
                m->cred.type = NSI_CRED_ADDRESS;
                m->cred.len = ep->name.len;
                nai_memcpy(&m->cred.addr, ep->name.addr, ep->name.len);
            };

            r = nsi_dgram_handle_incoming(c, m);
            if (r != NAI_DECLINED) {
                goto _end;
            };
        };

        if (nai_buf_total(b) < gro) {
            r = nsi_dgram_realloc_buf(c);
            if (r < 0) {
                ec = nai_errno;
                nai_log_alert(NSI_LOG_CORE, ec, 
                    "dgram endpoint(%s) failed to create read buffer", 
                    nai_str(&c->ep->host));
                goto _alert;
            };

            b = c->in.buf;
        };
    };


    r = NAI_DECLINED;

_end:
    return (int)r;

_error:
    (void)r;
    r = nsi_dgram_handle_error(c, ec);
    goto _end;

_alert:
    (void)r;
    r = nsi_dgram_handle_alert(c, ec);
    goto _end;

_alert_msg:
    (void)r;
    nsi_message_close(m);
    r = nsi_dgram_handle_alert(c, ec);
    goto _end;
};


static int nsi_dgram_make_tp_msg(
    nsi_dgram_t* c, nsi_message_t* m, uint32_t mss)
{
    int ec;
    int v;
    int u;
    intptr_t r;
    uint32_t len;
    nai_bufarray_t va;


    /* fix the value of mss, 
     * because the value of tp-header's offset should be multiples of 16 bytes
     */
    /* PRS_SOMEIP_00724 */
    /* PRS_SOMEIP_00729 */
    len = sizeof(c->out.tv[0]);
    mss = nai_aligndown(mss - len, NSI_MSG_TP_ALIGN) + len;


    v = 0;
    u = 0;

    while (u < (int)nai_countof(c->out.tv)) {
        len = sizeof(c->out.tv[0]);
        c->out.va[v].buf = (uint8_t*)&c->out.tv[u];
        c->out.va[v].len = len;

        len = mss - len;
        if (len > c->out.paylen) {
            len = c->out.paylen;
        };

        r = nai_bufwalk_to_rbufarray(&c->out.walk, 
            &va, c->out.va+v+1, nai_countof(c->out.va)-v-1, 
            len, 1);
        if (r < 0) {
            ec = nai_errno;
            if (u > 0 && ec == EOVERFLOW) {
                break;
            };

            nai_log_alert(NSI_LOG_CORE, ec, 
                "dgram endpoint(%s) get send bufarray failed", 
                nai_str(&c->ep->host));

            r = -ec;
            goto _end;
        };
        if (r != len) {
            nai_log_error(NSI_LOG_CORE, EINVAL, 
                "dgram endpoint(%s) send a message(%d) "
                "which length(%d) isn't equal "
                "with the length(%d) of payload", 
                nai_str(&c->ep->host), m->hdr.method, 
                m->hdr.len, 
                m->hdr.len - (uint32_t)(c->out.paylen - r));
            assert(0);

            c->out.qsize += r - len;
            len = (uint32_t)r;
            m->hdr.len -= (c->out.paylen - len);
            c->out.paylen = len;
        };

        /* PRS_SOMEIP_00724 */
        /* PRS_SOMEIP_00729 */
        c->out.tv[u].tp.off = m->hdr.len;
        c->out.tv[u].tp.off -= NSI_MSGEXT_SIZE + c->out.paylen;
        c->out.tv[u].tp.off /= NSI_MSG_TP_ALIGN;
        c->out.tv[u].tp.res = 0;
        c->out.tv[u].tp.more = c->out.paylen > len;
        c->out.tv[u].tp.info = nai_htonl(c->out.tv[u].tp.info);
        c->out.tv[u].hdr = c->out.hdr;
        c->out.tv[u].hdr.len = nai_htonl(
            NSI_MSGEXT_SIZE + sizeof(c->out.tp) + len);

        c->out.sa[u].v = &c->out.va[v];
        c->out.sa[u].count = va.count + 1;

        u ++;
        v += va.count + 1;
        c->out.paylen -= len;

        if (v >= (int)nai_countof(c->out.va) || 
            c->out.paylen <= 0) {
            break;
        };
    };

    c->out.vcount = v;
    c->out.scount = u;
    c->out.snext = 0;
    c->out.ssize = mss;
    c->send = NSI_DGRAM_SEND_MSG;
    r = 0;

_end:
    return (int)r;
};


static int nsi_dgram_make_msg(nsi_dgram_t* c, nsi_message_t* m, uint32_t mss)
{
    int ec;
    int v;
    int u;
    intptr_t r;
    uint32_t len;
    nai_list_entry_t* e;
    nai_bufarray_t va;


    len = 0;
    v = 0;
    u = 0;

    /* try to send multi-messages in one dgram
     * PRS_SOMEIP_00140
     */
    for ( ; ; ) {

        /* put header */
        c->out.va[v].buf = (uint8_t*)&c->out.hv[u];
        if (c->out.evt.is) {
            c->out.hv[u].len = nai_htonl(m->hdr.len - 
                (NSI_MSGEXT_SIZE - NSI_SIGEXT_SIZE));
            c->out.va[v].len = NSI_MSGHDR_SIZE;
        } else {
            c->out.va[v].len = sizeof(c->out.hv[0]);
        };

        /* put payload */
        r = nai_buflist_to_rbufarray(&m->payload, 
            &va, c->out.va+v+1, nai_countof(c->out.va)-v-1, 
            NAI_SIZE_T_MAX, 1);
        if (r < 0) {
            ec = nai_errno;
            if (u > 0 && ec == EOVERFLOW) {
                break;
            };

            nai_log_alert(NSI_LOG_CORE, ec, 
                "dgram endpoint(%s) get send bufarray failed", 
                nai_str(&c->ep->host));

            r = -ec;
            goto _end;
        };
        if (r != (intptr_t)(m->hdr.len - NSI_MSGEXT_SIZE)) {
            nai_log_error(NSI_LOG_CORE, EINVAL, 
                "dgram endpoint(%s) send a message(%d) "
                "which length(%d) isn't equal "
                "with the length(%d) of payload", 
                nai_str(&c->ep->host), m->hdr.method, 
                m->hdr.len, 
                (uint32_t)(r + NSI_MSGEXT_SIZE));
            assert(0);

            /* fix the length */
            c->out.qsize += r - (m->hdr.len - NSI_MSGEXT_SIZE);
            m->hdr.len = (uint32_t)(r + NSI_MSGEXT_SIZE);

            /* write to buffer again */
            if (c->out.evt.is) {
                c->out.hv[u].len = nai_htonl(m->hdr.len - 
                    (NSI_MSGEXT_SIZE - NSI_SIGEXT_SIZE));
            } else {
                c->out.hv[u].len = nai_htonl(m->hdr.len);
            };
        };

        /* add message length */
        len += (uint32_t)r;
        len += (uint32_t)c->out.va[v].len;

        /* out of mss ? */
        if (len > mss) {
            if (u > 0) {
                break;
            };

            r = -EOVERFLOW;
            goto _end;
        };

        /* append */
        u ++;
        v += va.count + 1;

        /* has enough space ? */
        if (v > (int)nai_countof(c->out.va) - 2) {
            break;
        };
        if (u >= (int)nai_countof(c->out.mv)) {
            break;
        };


        /* find next message with same dest address */
        e = m->ent.next;
        for ( ; e != &c->out.waiting; e = e->next) {
            m = (nsi_message_t*)e;
            if (m->conn == c->out.to->cid) {
                break;
            };
        };
        if (e == &c->out.waiting) {
            break;
        };

        /* check signal */
        if (m->hdr.type != NSI_MT_NOTIFICATION) {
            c->out.evt.is = 0;
        } else {
            c->out.evt.is = nsi_dgram_message_is_signal(c, m);
        };

        /* out of mss ? */
        r = m->hdr.len + NSI_MSGHDR_SIZE;
        if (c->out.evt.is) {
            r -= NSI_MSGEXT_SIZE - NSI_SIGEXT_SIZE;
        };
        if (len + (uint32_t)r > mss) {
            break;
        };

        /* save */
        c->out.mv[u] = m;
        nsi_msghdr_hton(&c->out.hv[u], &m->hdr);
    };

    c->out.mcount = (int16_t)u;
    c->out.vcount = (int16_t)v;
    c->out.scount = 1;
    c->out.snext = 0;
    c->out.ssize = mss;
    c->out.sa[0].v = c->out.va;
    c->out.sa[0].count = v;
    c->send = NSI_DGRAM_SEND_MSG;
    r = 0;

_end:
    return (int)r;
};


#if (NAI_HAVE_UDP_SEGMENT)

typedef struct nsi_dgram_gso_s {
    char data[CMSG_SPACE(sizeof(uint16_t))];
} nsi_dgram_gso_t, nsi_dgram_gro_t;

#endif


static intptr_t nsi_dgram_send_msg(
    nsi_dgram_t* c, uint32_t n, uint32_t cnt, uint32_t mss, uint32_t gso)
{
    intptr_t r;
    int flags;


#if (NAI_HAVE_UDP_SEGMENT)

    if (gso) {
        struct cmsghdr* cm;
        nsi_dgram_gso_t gm;

        cm = (struct cmsghdr*)gm.data;
        cm->cmsg_level = SOL_UDP;
        cm->cmsg_type = UDP_SEGMENT;
        cm->cmsg_len = CMSG_LEN(sizeof(uint16_t));
        *(uint16_t*)CMSG_DATA(cm) = (uint16_t)mss;
        r = nai_dgram_sendm(&c->io, 
            c->out.sa[n].v, c->out.sa[n+cnt-1].v - 
            c->out.sa[n].v + c->out.sa[n+cnt-1].count, 0, 
            c->out.to->name.addr, c->out.to->name.len, 
            gm.data, sizeof(gm.data));

    } else {

#else
        (void)gso;
#endif

        if (cnt <= 1) {
            r = nai_dgram_sendm(
                &c->io, c->out.sa[n].v, c->out.sa[n].count, 0, 
                c->out.to->name.addr, c->out.to->name.len, 0, 0);
        } else {
            flags = 0;

#if defined(SO_ZEROCOPY) && defined(MSG_ZEROCOPY)
            if (c->out.zerocopy) {
                flags = MSG_ZEROCOPY;
            };
#endif

            r = nai_dgram_sendmm(
                &c->io, c->out.sa + n, cnt, flags, 
                c->out.to->name.addr, c->out.to->name.len);

#if defined(SO_ZEROCOPY) && defined(MSG_ZEROCOPY)
            if (flags && r > 0) {
                c->out.zpending += (uint16_t)((r + mss - 1) / mss);
            };
#endif

        };

#if (NAI_HAVE_UDP_SEGMENT)
    };
#endif


    return r;
};


#if defined(SO_ZEROCOPY) && defined(MSG_ZEROCOPY)


#include <linux/errqueue.h>


static int nsi_dgram_handle_zc_stat(nsi_dgram_t* c)
{
    int ec;
    intptr_t r;
    struct cmsghdr *cm;
    struct sock_extended_err *se;
    int hi;
    int lo;
    int count;
    int ctrllen;
    char ctrl[100];


    if (!c->out.zwaiting || !c->out.znotified) {
        r = 0;
        goto _end;
    };

    ctrllen = sizeof(ctrl);
    r = nai_sock_recvm(
        nai_dgram_get_fd(&c->io), 
        0, 0, MSG_ERRQUEUE, 0, 0, ctrl, &ctrllen);
    if (r < 0) {
        ec = nai_errno;
        if (ec == NAI_EAGAIN) {
            r = 0;
        };
        goto _end;
    };

    c->out.znotified = 0;

    cm = (struct cmsghdr*)ctrl;
    if (!(cm->cmsg_level == SOL_IP && cm->cmsg_type == IP_RECVERR) && 
        !(cm->cmsg_level == SOL_IPV6 && cm->cmsg_type == IPV6_RECVERR)) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    se = (void*)CMSG_DATA(cm);
    if (se->ee_origin != SO_EE_ORIGIN_ZEROCOPY) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };
    if (se->ee_errno != 0) {
        nai_errno = se->ee_errno;
        r = -1;
        goto _end;
    };
    if (se->ee_code & SO_EE_CODE_ZEROCOPY_COPIED) {
        /* device is not supported */
        c->out.zerocopy = 0;
    };

    hi = se->ee_data;
    lo = se->ee_info;
    count = hi - lo + 1;
    c->out.zpending -= (uint16_t)count;
    if (c->out.zpending == 0) {
        c->out.zwaiting = 0;
        r = 1;
    } else {
        r = 0;
    };

_end:
    return r;
};

#endif


static int nsi_dgram_handle_send(nsi_dgram_t* c)
{
    int ec;
    intptr_t r;
    uint32_t u;
    uint32_t l;
    uint32_t gso;
    uint32_t mss;
    uint32_t ssm;
    uint32_t segs;
    uint64_t now;
    nai_rbnode_t** n;
    nsi_message_t* m;
    nsi_dgram_bind_t* b;


#if defined(NSI_TEST)
    /* prevent sending too fast under the loopback address */
    size_t total = 0;
#endif


    now = 0;
    mss = c->ep->net->mss;
    gso = c->out.gso;
    segs = gso ? c->ep->net->send_segs : nai_countof(c->out.tv)/2;
    while (1) {

        m = c->out.msg;
        if (m == 0) {
            if (nai_list_is_empty(&c->out.waiting)) {
                break;
            };

            m = (nsi_message_t*)c->out.waiting.next;
            c->out.msg = m;
            c->out.mcount = 1;
            nsi_msghdr_hton(&c->out.hdr, &m->hdr);

            /* check signal */
            if (m->hdr.type != NSI_MT_NOTIFICATION) {
                c->out.evt.is = 0;
            } else {
                c->out.evt.is = (char)nsi_dgram_message_is_signal(c, m);
            };

            /* verify message header */
            if (m->hdr.len < mss) {
                c->send = NSI_DGRAM_MAKE_MSG;
                c->out.paylen = 0;
            } else if (c->out.evt.is) {
                if (m->hdr.len - NSI_DGRAM_MAKE_MSG < mss) {
                    c->send = NSI_DGRAM_MAKE_MSG;
                    c->out.paylen = 0;
                } else {
                    /* signal is not supported tp message */
                    nsi_dgram_handle_sent(c, ERANGE);
                    continue;
                };
            } else {
                c->send = NSI_DGRAM_MAKE_TP_MSG;
                c->out.paylen = m->hdr.len - NSI_MSGEXT_SIZE;
                c->out.hdr.type = nsi_msgtype_to_tp(m->hdr.type);
                nai_bufwalk_init(&c->out.walk, &m->payload);
            };


            /* find address */
            if (m->conn == 0 && !c->server) {
                c->out.to = (nsi_endpoint_bind_t*)c->ep;
            } else {
                if (c->out.to == 0 || 
                    c->out.to == (nsi_endpoint_bind_t*)c->ep || 
                    c->out.to->cid != m->conn) {
                    /* lookup dest */
                    n = nsi_dgram_bind_find_conn(&c->conns, m->conn, 0);
                    if (n[0] == 0) {
                        r = nsi_dgram_handle_sent(c, ENOTCONN);
                        if (r != NAI_DECLINED) {
                            goto _end;
                        };
                        continue;
                    };

                    b = nai_containof(n[0], nsi_dgram_bind_t, entc);
                    c->out.to = b;
                } else {
                    b = c->out.to;
                };

                /* update expire time of bind name */
                if (now == 0) {
                    now = nai_tickcache_to_msec();
                };
                b->time = now;
            };
        };

        /* output */
        while (c->send) {
            switch (c->send) {
            case NSI_DGRAM_MAKE_TP_MSG:

                r = nsi_dgram_make_tp_msg(c, m, mss);
                if (r < 0) {
                    ec = (int)-r;
                    goto _alert;
                };
                break;

            case NSI_DGRAM_MAKE_MSG:

                r = nsi_dgram_make_msg(c, m, mss);
                if (r < 0) {
                    ec = (int)-r;
                    goto _alert;
                };

                /* fallthrough */

            case NSI_DGRAM_SEND_MSG:

                /* todo: some systems do not support sendmsg and 
                 * need to be combined and sent
                 */
                ssm = c->out.ssize;
                for ( ; c->out.snext < c->out.scount; ) {
                    u = c->out.snext;
                    l = c->out.scount - u;

#if defined(NSI_TEST)
                    /* prevent sending too fast under the loopback address */
                    if (total >= 32*1024) {
                        nai_dgram_post(&c->io, NAI_EV_WRITE);
                        r = NAI_DECLINED;
                        goto _end;
                    };

                    if (l * ssm + total > 32*1024 && !gso) {
                        l = (uint32_t)((32*1024 - total) + ssm -1) / ssm;
                    };
#endif

                    if (l > segs) {
                        l = segs;
                    };

                    r = nsi_dgram_send_msg(c, u, l, ssm, gso);
                    if (r < 0) {
                        ec = nai_errno;
                        if (ec == NAI_EAGAIN || ec == NAI_EINPROGRESS) {
                            r = NAI_DECLINED;
                            goto _end;
                        };

                        nai_log_crit(NSI_LOG_CORE, 
                            ec, "dgram endpoint(%s) send failed", 
                            nai_str(&c->ep->host));
                        goto _error;
                    };

                    c->out.snext += (uint32_t)(r + ssm - 1) / ssm;

#if defined(NSI_TEST)
                    /* prevent sending too fast under the loopback address */
                    total += r;
#endif
                };

#if defined(SO_ZEROCOPY) && defined(MSG_ZEROCOPY)
                if (c->out.zpending) {
                    c->out.zwaiting = 1;
                    r = nsi_dgram_handle_zc_stat(c);
                    if (r < 0) {
                        ec = nai_errno;
                        nai_log_crit(NSI_LOG_CORE, ec, 
                            "dgram endpoint(%s) wait zc failed", 
                            nai_str(&c->ep->host));
                        goto _error;
                    };
                    if (r <= 0) {
                        r = NAI_DECLINED;
                        goto _end;
                    };

                    /* zerocopy completed */
                };
#endif

                if (c->out.paylen > 0) {
                    c->send = NSI_DGRAM_MAKE_TP_MSG;

#if defined(NSI_TEST)
                    /* prevent sending too fast under the loopback address */
                    if (total >= 32*1024) {
                        nai_dgram_post(&c->io, NAI_EV_WRITE);
                        r = NAI_DECLINED;
                        goto _end;
                    };
#endif
                    continue;
                };

                r = nsi_dgram_handle_sent(c, 0);
                break;

            default:
                assert(0);
                ec = EINVAL;
                nai_log_error(NSI_LOG_CORE, 
                    ec, "dgram endpoint(%s) with error send stat %d", 
                    nai_str(&c->ep->host), c->send);
                goto _error;
            };
        };
    };

    (void)r;

    r = NAI_DECLINED;

_end:
    return (int)r;

_error:
    r = nsi_dgram_handle_sent(c, ec);
    goto _end;

_alert:
    r = nsi_dgram_handle_sent(c, ec);
    if (r != NAI_DECLINED) {
        goto _end;
    };

    r = nsi_dgram_handle_alert(c, ec);
    goto _end;
};


static int nsi_dgram_handle_timeout(nsi_dgram_t* p)
{
    int r;
    uint64_t now;
    uint64_t next;
    uint64_t expire;
    nai_rbnode_t* n;
    nai_list_entry_t* e;
    nsi_message_t* m;
    nsi_dgram_bind_t* b;


    now = nai_tickcache_to_msec();

    /* remove timedout incoming tp-message */
    e = p->in.queued.next;
    for ( ; e != &p->in.queued; ) {
        m = (nsi_message_t*)e;
        e = e->next;

        expire = nsi_message_time(m) + (NSI_DGRAM_TIME_TP);
        if (expire > now) {
            continue;
        };

        nai_log_crit(NSI_LOG_CORE, 0, 
            "dgram endpoint(%s) discard timed out "
            "incoming tp-message(%d), service(%d) client(0x%x) session(%d)", 
            nai_str(&p->ep->host), m->hdr.type, 
            m->hdr.serv, m->hdr.client, m->hdr.session);
        nai_list_entry_remove(&m->ent);
        nsi_message_close(m);
    };

    /* remove timedout peer addresses */
    next = now + NSI_DGRAM_TIME_BIND;
    expire = now / NSI_DGRAM_TIME_BIND * 
        NSI_DGRAM_TIME_BIND + NSI_DGRAM_TIME/2;
    if (now < expire) {
        nai_log_debug(NSI_LOG_CORE, 0, 
            "dgram endpoint(%s) do timedout(%ld) check for peer addresses", 
            nai_str(&p->ep->host), expire);

        n = nai_rbtree_begin(&p->names);
        for ( ; n != nai_rbtree_end(&p->names); ) {
            b = nai_containof(n, nsi_dgram_bind_t, entn);
            n = nai_rbtree_next(n);

            if (b->refs > 0) {
                continue;
            };

            expire = b->time + (NSI_DGRAM_TIME_BIND);
            if (expire > now) {
                if (next > expire) {
                    next = expire;
                }
                continue;
            };

            nai_log_info(NSI_LOG_CORE, 0, 
                "dgram endpoint(%s) remove timed out bind name(%d)", 
                nai_str(&p->ep->host), b->cid);
            nai_rbtree_erase(&p->names, &b->entn);
            nai_rbtree_erase(&p->conns, &b->entc);
            nai_bufpool_xfree(&p->pool, b, sizeof(*b) + b->name.len);
        };
    };

    if (!nai_list_is_empty(&p->in.queued)) {
        next = now + NSI_DGRAM_TIME;
    };
    next += NSI_DGRAM_TIME - 1;
    next /= NSI_DGRAM_TIME;
    next *= NSI_DGRAM_TIME;
    r = nai_dgram_set_timeout(
        &p->io, NAI_TIMEOP_SET, (uint32_t)(next - now));
    if (r < 0) {
        nai_log_error(NSI_LOG_CORE, nai_errno, 
            "dgram endpoint(%s) set timeout failed", 
            nai_str(&p->ep->host));
    };

    return r;
};


static int nsi_dgram_handle(nai_dgram_t* p, int events)
{
    int r;
    int ec;
    nsi_dgram_t* c;
    nsi_endpoint_name_t name;


    c = (nsi_dgram_t*)p;

#if defined(SO_ZEROCOPY) && defined(MSG_ZEROCOPY)
    if (events & NAI_EV_ERROR) {
        ec = nai_ev_error_code(events);
        if (ec == 0) {
            c->out.znotified = 1;
            r = nsi_dgram_handle_zc_stat(c);
            if (r < 0) {
                ec = nai_errno;
                nai_log_crit(NSI_LOG_CORE, ec, 
                    "dgram endpoint(%s) wait zc failed", 
                    nai_str(&c->ep->host));

                events = nai_ev_error_from(ec);
            } else if (r > 0) {
                events |= NAI_EV_WRITE;
            } else {
                events &= ~NAI_EV_ERROR;
            };
        };
    };
#endif

    if (events & (NAI_EV_READ|NAI_EV_WRITE)) {

        if (events & NAI_EV_TIMEOUT) {
            r = nsi_dgram_handle_error(c, ETIMEDOUT);
            goto _end;
        };

        if (c->connected == 0) {
            c->connected = 1;
            c->retry = 0;

            /* get interface */
            if (c->server) {
                r = nsi_endpoint_get_interface(c->ep, &name);
                if (r < 0) {
                    name = c->name;
                };
            } else {
                name = c->name;
            };

            /* set multicast interface */
            r = nai_dgram_set_opt(p, NAI_IO_MULTICAST_IF, (intptr_t)name.addr);
            if (r < 0) {
                r = nsi_dgram_handle_error(c, nai_errno);
                goto _end;
            };

            r = nsi_dgram_handle_connect(c, 0);
            if (r != NAI_DECLINED) {
                goto _end;
            };
        };

        if (events & NAI_EV_READ) {
            r = nsi_dgram_handle_read(c);
            if (r != NAI_DECLINED) {
                goto _end;
            };
        };

        if (events & NAI_EV_WRITE) {
            r = nsi_dgram_handle_send(c);
            if (r != NAI_DECLINED) {
                goto _end;
            };
        };

    } else {

        switch (events & NAI_EV_MASK) {
        case NAI_EV_NOTIFY:
            switch (nai_ev_notify_code(events)) {
            case NAI_EV_WRITE:
                c->posted = 0;
                r = nsi_dgram_handle_send(c);
                goto _end;
            default:
                break;
            };

            break;

        case NAI_EV_TIMEOUT:
            if (c->rebind) {
                r = nsi_dgram_bind(c);
            } else {
                r = nsi_dgram_handle_timeout(c);
            };

            break;

        case NAI_EV_ERROR:
            ec = nai_ev_error_code(events);
            r = nsi_dgram_handle_error(c, ec);
            goto _end;

        default:
            /* nothing */
            break;
        };
    };

    (void)r;

    r = NAI_DECLINED;

_end:
    return r;
};



static nsi_dgram_t* nsi_dgram_create(
    nsi_endpoint_t* e, const nsi_endpoint_name_t* name)
{
    int r;
    int ec;
    int len;
    nsi_dgram_t* c;


    len = name ? name->len : 0;
    c = (nsi_dgram_t*)nsi_network_alloc(e->net, sizeof(*c) + len);
    if (c == 0) {
        nai_log_error(NSI_LOG_CORE, 
            nai_errno, "allocate dgram endpoint(%s) failed", 
            nai_str(&e->host));
        goto _end;
    };

    nai_bufpool_init(&c->pool);
    r = nai_bufpool_open(&c->pool, 512, 0);
    if (r < 0) {
        nai_log_alert(NSI_LOG_CORE, 
            nai_errno, "dgram endpoint(%s) failed to create bufpool", 
            nai_str(&e->host));
        goto _fail;
    };

    /* allocate a buffer to receive address */
    c->in.from.len = 64;
    c->in.from.addr = (nai_sockaddr_t*)
        nai_bufpool_xalloc(&c->pool, c->in.from.len);
    if (c->in.from.addr == 0) {
        nai_log_alert(NSI_LOG_CORE, 
            nai_errno, "dgram endpoint(%s) failed to allocate address buffer", 
            nai_str(&e->host));
        goto _fail;
    };

    /* init dgram */
    c->ep = e;
    c->server = 0;
    c->flags = 0;
    c->nextc = 0;
    c->in.buf = 0;
    c->in.qcount = 0;
    c->in.evt.serv = NSI_SERVICE_ANY;
    c->in.evt.method = NSI_METHOD_ANY;
    c->in.gro = 0;
    c->out.to = 0;
    c->out.msg = 0;
    c->out.qsize = 0;
    c->out.mcount = 0;
    c->out.vcount = 0;
    c->out.scount = 0;
    c->out.snext = 0;
    c->out.ssize = 0;
    c->out.paylen = 0;
    c->out.evt.serv = NSI_SERVICE_ANY;
    c->out.evt.inst = NSI_INSTANCE_ANY;
    c->out.evt.method = NSI_METHOD_ANY;
    c->out.gso = 0;
    c->out.zerocopy = 0;
    c->out.zpending = 0;
    c->out.zwaiting = 0;
    c->out.znotified = 0;
    nai_rbtree_init(&c->conns);
    nai_rbtree_init(&c->names);
    nai_list_init(&c->in.queued);
    nai_list_init(&c->out.waiting);

    if (name == 0) {
        c->name = e->name;
    } else {
        c->name.addr = (nai_sockaddr_t*)(c + 1);
        c->name.len = len;
        nai_memcpy(c->name.addr, name->addr, len);
    };

    /* init dgram io */
    nai_dgram_init(&c->io);
    nai_dgram_set_cb(&c->io, nsi_dgram_handle);


_end:
    return c;

_fail:
    ec = nai_errno;
    nai_bufpool_close(&c->pool);
    nsi_network_free(e->net, c);
    nai_errno = ec;
    c = 0;
    goto _end;
};


static int nsi_dgram_set_opts(nsi_dgram_t* c, nsi_network_t* n)
{
    int r;

#if defined(SO_SNDLOWAT) || defined(SO_ZEROCOPY) || (NAI_HAVE_UDP_SEGMENT)
    int val;
#endif
#if defined(SO_SNDLOWAT)
    int ec;
    intptr_t bsize;
#endif


#if (NAI_HAVE_UDP_SEGMENT)
    if (n->recv_segs > 1) {
        val = 1;
        r = -1;
#if defined(UDP_GRO)
        r = nai_sock_set_opt(
            nai_dgram_get_fd(&c->io), 
            IPPROTO_UDP, UDP_GRO, (char*)&val, sizeof(val));
        if (r < 0) {
            nai_log_warn(NSI_LOG_CORE, nai_errno, 
                "dgram endpoint(%s) enable recv offload failed", 
                nai_str(&c->ep->host));
        };
#endif
        c->in.gro = r >= 0;
    };
    if (n->send_segs > 1) {
        val = n->mss;
        r = nai_sock_set_opt(
            nai_dgram_get_fd(&c->io), 
            SOL_UDP, UDP_SEGMENT, (char*)&val, sizeof(val));
        if (r < 0) {
            nai_log_warn(NSI_LOG_CORE, nai_errno, 
                "dgram endpoint(%s) enable send offload failed", 
                nai_str(&c->ep->host));
        };

        c->out.gso = r >= 0;

        if (r >= 0) {
            val = 0;
            nai_sock_set_opt(
                nai_dgram_get_fd(&c->io), 
                SOL_UDP, UDP_SEGMENT, (char*)&val, sizeof(val));
        };
    };
#else
    if (n->recv_segs > 1 || n->send_segs > 1) {
        nai_log_info(NSI_LOG_CORE, 0, 
            "the platform not suppoted offload.");
    };
#endif

#if defined(SO_ZEROCOPY) && defined(MSG_ZEROCOPY)
    if (n->use_zerocopy && !c->ep->local && 
        n->mss > 10 * 1024 && !c->out.gso) {
        val = 1;
        r = nai_sock_set_opt(
            nai_dgram_get_fd(&c->io), 
            SOL_SOCKET, SO_ZEROCOPY, (char*)&val, sizeof(val));
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE, nai_errno, 
                "dgram endpoint(%s) set zc(%d) failed", 
                nai_str(&c->ep->host), val);
        } else {
            r = nai_dgram_set_except(&c->io, 1);
            if (r < 0) {
                nai_log_crit(NSI_LOG_CORE, nai_errno, 
                    "dgram endpoint(%s) set except(%d) failed", 
                    nai_str(&c->ep->host), 1);
                goto _end;
            };

            c->out.zerocopy = 1;
        };
    };
#endif

#if !defined(NSI_TEST)
    r = nai_dgram_set_opt(&c->io, NAI_IO_MULTICAST_LOOP, 0);
    if (r < 0) {
        nai_log_crit(NSI_LOG_CORE, nai_errno, 
            "dgram endpoint(%s) disable multicast loop failed", 
            nai_str(&c->ep->host));
        goto _end;
    };
#endif

    if (n->recv_buf_udp != (uint32_t)-1) {
        r = nai_dgram_set_opt(&c->io, NAI_IO_RECVBUF, n->recv_buf_udp);
        if (r < 0) {
            nai_log_crit(NSI_LOG_CORE, nai_errno, 
                "dgram endpoint(%s) set recv bufsize(%d) failed", 
                nai_str(&c->ep->host), n->recv_buf_udp);
            goto _end;
        };
    };
    if (n->send_buf_udp != (uint32_t)-1) {
        r = nai_dgram_set_opt(&c->io, NAI_IO_SENDBUF, n->send_buf_udp);
        if (r < 0) {
            nai_log_crit(NSI_LOG_CORE, nai_errno, 
                "dgram endpoint(%s) set send bufsize(%d) failed", 
                nai_str(&c->ep->host), n->send_buf_udp);
            goto _end;
        };
    };

#if defined(SO_SNDLOWAT)
    r = nai_dgram_get_opt(&c->io, NAI_IO_SENDBUF, &bsize);
    if (r < 0) {
        nai_log_crit(NSI_LOG_CORE, nai_errno, 
            "dgram endpoint(%s) get send bufsize failed", 
            nai_str(&c->ep->host));
        goto _end;
    };

    val = bsize / 2;
    if (val >= 4096) {
        r = nai_sock_set_opt(
            nai_dgram_get_fd(&c->io), 
            SOL_SOCKET, SO_SNDLOWAT, (char*)&val, sizeof(val));
        if (r < 0) {
            ec = nai_errno;
            if (ec != ENOPROTOOPT) {
                nai_log_crit(NSI_LOG_CORE, ec, 
                    "dgram endpoint(%s) set sndlowat(%d) failed", 
                    nai_str(&c->ep->host), val);
                goto _end;
            };
        };
    };
#endif

    r = 0;

_end:
    return r;
};


static int nsi_dgram_wait_bind(nsi_dgram_t* c)
{
    int r;
    nsi_network_t* n;


    /* set timer and try bind again */
    if (nai_dgram_is_opened(&c->io)) {
        r = nai_dgram_set_timeout(
            &c->io, NAI_TIMEOP_SET, 100 << c->retry);
    } else if (nai_evnode_is_opened(&c->io.ev)) {
        r = nai_evnode_set_timeout(
            &c->io.ev, NAI_TIMEOP_SET, 100 << c->retry);
    } else {
        n = c->ep->net;
        nai_evnode_set_cb(&c->io.ev, (nai_evnode_cb_f)nsi_dgram_handle);
        nai_evnode_set_timeout(
            &c->io.ev, NAI_TIMEOP_SET, 100 << c->retry);

        r = nai_evnode_open(&c->io.ev, n->loop);
    };
    if (r < 0) {
        goto _end;
    };

    c->rebind = 1;
    if (c->retry < 8) {
        c->retry ++;
    };

    r = 0;

_end:
    return r;
};


static int nsi_dgram_bind(nsi_dgram_t* c)
{
    int r;
    int ec;
    nsi_network_t* n;


    nai_log_debug(NSI_LOG_CORE, 0, 
        "dgram endpoint(%s) start %s", 
        nai_str(&c->ep->host), 
        c->rebind ? "rebind" : "bind");


    /* do bind */
    r = nsi_iobase_bind((nsi_iobase_t*)c, 0);
    if (r < 0) {
        ec = nai_errno;
        nai_log_crit(NSI_LOG_CORE, 
            ec, "dgram endpoint(%s) bind address failed", 
            nai_str(&c->ep->host));

        switch (ec) {
        case EADDRINUSE:
        case EADDRNOTAVAIL:
            c->retry = 8;
            goto _error;

        default:
            goto _alert;
        };
    };


    /* do set options */
    n = c->ep->net;
    nai_dgram_set_opt(&c->io, NAI_IO_SENDTIMEO, n->send_timeo);
    nai_dgram_set_timeout(&c->io, NAI_TIMEOP_SET, NSI_DGRAM_TIME_BIND);
    nai_dgram_set_polling(&c->io, n->use_polling);

    /* do open */
    r = nai_dgram_open(&c->io, n->loop);
    if (r < 0) {
        ec = nai_errno;
        nai_log_crit(NSI_LOG_CORE, 
            ec, "dgram endpoint(%s) open failed", 
            nai_str(&c->ep->host));
        goto _alert;
    };

    r = nsi_dgram_set_opts(c, n);
    if (r >= 0) {
        c->rebind = 0;
    } else {
        ec = nai_errno;
        nai_log_crit(NSI_LOG_CORE, 
            ec, "dgram endpoint(%s) set options failed", 
            nai_str(&c->ep->host));
        goto _alert;
    };


_end:
    return r;

_error:
    if (c->rebind) {
        /* set timer and try bind again */
        r = nsi_dgram_wait_bind(c);
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE, nai_errno, 
                "dgram endpoint(%s) failed to start wait bind", 
                nai_str(&c->ep->host));

            goto _alert;
        };

        r = 0;
    };
    goto _end;

_alert:
    if (c->rebind) {
        r = nsi_dgram_handle_alert(c, ec);
    };
    goto _end;
};


extern nsi_endpoint_ops_t nsi_dgram_server_ops;
extern nsi_endpoint_ops_t nsi_dgram_client_ops;


static int nsi_dgram_map_conn(
    nsi_dgram_t* c, nsi_endpoint_t* s, nsi_connid_t cid)
{
    int r;
    nai_rbnode_t** n;
    nsi_dgram_t* u;
    nsi_dgram_bind_t* b;
    nsi_endpoint_name_t name;
    nai_socknbuf_in_t nbuf;


    if (s->ops == &nsi_dgram_server_ops || 
        s->ops == &nsi_dgram_client_ops) {
        u = (nsi_dgram_t*)s->ctx;
        if (u == 0) {
            nai_log_error(NSI_LOG_CORE, EINVAL, 
                "dgram endpoint(%s) cannot "
                "map connection with an invalid endpoint", 
                nai_str(&c->ep->host));
            nai_errno = EINVAL;
            r = -1;
            goto _end;
        };

        n = nsi_dgram_bind_find_conn(&u->conns, cid, 0);
        if (n[0] == 0) {
            nai_log_error(NSI_LOG_CORE, ENOENT, 
                "dgram endpoint(%s) cannot map connection in endpoint(%s)", 
                nai_str(&c->ep->host), nai_str(&s->host));
            nai_errno = ENOENT;
            r = -1;
            goto _end;
        };

        b = nai_containof(n[0], nsi_dgram_bind_t, entc);
        name = b->name;

    } else {

        name.addr = &nbuf.addr;
        name.len = sizeof(nbuf.storage);

        r = nsi_endpoint_get_name(s, cid, &name);
        if (r < 0) {
            goto _end;
        };
    };

    b = nsi_dgram_add_bind(c, name.addr, name.len);
    if (b == 0) {
        r = -1;
        goto _end;
    };

    r = b->cid;

_end:
    return r;
};


static int nsi_dgram_get_name(
    nsi_dgram_t* c, nsi_connid_t cid, nsi_endpoint_name_t* name)
{
    int r;
    nai_rbnode_t** n;
    nsi_dgram_bind_t* b;
    nai_socknbuf_t nbuf;
    nsi_endpoint_name_t addr;


    if (cid == 0) {
        if (c->server) {
            addr = c->ep->name;
        } else {
            nbuf.len = sizeof(nbuf.storage);
            r = nai_sock_get_sockname(
                nai_dgram_get_fd(&c->io), &nbuf.addr, &nbuf.len);
            if (r < 0) {
                nai_log_crit(NSI_LOG_CORE, nai_errno, 
                    "dgram endpoint(%s) get socket bind address failed", 
                    nai_str(&c->ep->host));
                goto _end;
            };

            addr.addr = &nbuf.addr;
            addr.len = nbuf.len;
        };
    } else {
        n = nsi_dgram_bind_find_conn(&c->conns, cid, 0);
        if (n[0] == 0) {
            nai_errno = ENOENT;
            r = -1;
            goto _end;
        };

        b = nai_containof(n[0], nsi_dgram_bind_t, entc);
        addr = b->name;
    };

    if (name) {
        if (name->addr && name->len > addr.len) {
            nai_memcpy(name->addr, addr.addr, addr.len);
        };
        name->len = addr.len;
    };

    r = 0;

_end:
    return r;
};


static int nsi_dgram_bind_conn(nsi_dgram_t* c, nsi_connid_t cid)
{
    int r;
    nai_rbnode_t** n;
    nai_rbnode_t* parent;
    nsi_dgram_bind_t* b;


    n = nsi_dgram_bind_find_conn(&c->conns, cid, &parent);
    if (n[0] == 0) {
        nai_errno = ENOENT;
        r = -1;
        goto _end;
    };

    b = nai_containof(n[0], nsi_dgram_bind_t, entc);
    b->banned = 0;
    b->refs ++;

    r = 0;

_end:
    return r;
};

static int nsi_dgram_rebind_name(
    nsi_dgram_t* c, nsi_endpoint_name_t* in)
{
    int r;
    nsi_dgram_bind_t* b;
    nai_sockaddr_t* sa[2];
    nsi_endpoint_name_t name;


    b = nsi_dgram_add_bind(c, in->addr, in->len);
    if (b == 0) {
        r = -1;
        goto _end;
    };

    sa[0] = b->name.addr;
    sa[1] = c->name.addr;

    r = nsi_endpoint_get_interface(c->ep, &name);
    if (r >= 0) {
        sa[1] = name.addr;
    };

    r = nai_dgram_set_opt(
        &c->io, NAI_IO_MULTICAST_JOIN, (intptr_t)sa);
    if (r < 0 && nai_errno != EADDRINUSE) {
        nai_log_crit(NSI_LOG_CORE, nai_errno,
            "dgram endpoint(%s) rejoin multicast failed",
            nai_str(&c->ep->host));

        if (nai_errno == ENODEV) {
            nai_dgram_set_opt(
                &c->io, NAI_IO_MULTICAST_DROP, (intptr_t)sa);
        } else if (nai_errno == ENOBUFS) {
            /* meet unrecoverable scenario */
            nai_log_crit(NSI_LOG_CORE, nai_errno,
                "meet unrecoverable situation for joining muliticast and raise SIGTERM");
            raise(SIGTERM);
        };

        goto _end;
    };


    r = b->cid;

_end:
    return r;
};


static int nsi_dgram_bind_name(
    nsi_dgram_t* c, nsi_endpoint_name_t* in, int mcast, int temp)
{
    int r;
    nsi_dgram_bind_t* b;
    nai_sockaddr_t* sa[2];
    nsi_endpoint_name_t name;


    if (in == 0) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    b = nsi_dgram_add_bind(c, in->addr, in->len);
    if (b == 0) {
        r = -1;
        goto _end;
    };

    if (b->mcast == 0 && mcast) {
        sa[0] = b->name.addr;
        sa[1] = c->name.addr;

        r = nsi_endpoint_get_interface(c->ep, &name);
        if (r >= 0) {
            sa[1] = name.addr;
        };

        r = nai_dgram_set_opt(
            &c->io, NAI_IO_MULTICAST_JOIN, (intptr_t)sa);
        if (r < 0) {
            nai_log_crit(NSI_LOG_CORE, nai_errno, 
                "dgram endpoint(%s) join multicast failed", 
                nai_str(&c->ep->host));
            goto _end;
        };

        b->mcast = 1;
    };

    b->time = nai_tickcache_to_msec();
    b->banned = 0;
    if (temp == 0) {
        b->refs ++;
    };

    r = b->cid;


_end:
    return r;
};


static int nsi_dgram_unbind_name(nsi_dgram_t* c, nsi_connid_t cid)
{
    int r;
    nai_rbnode_t** n;
    nai_rbnode_t* parent;
    nsi_dgram_bind_t* b;
    nai_sockaddr_t* sa[2];
    nsi_endpoint_name_t name;


    n = nsi_dgram_bind_find_conn(&c->conns, cid, &parent);
    if (n[0] == 0) {
        nai_errno = ENOENT;
        r = -1;
        goto _end;
    };

    b = nai_containof(n[0], nsi_dgram_bind_t, entc);
    if (b->refs > 1) {
        b->refs --;
        r = 0;
        goto _end;
    };

    if (b->mcast) {
        sa[0] = b->name.addr;
        sa[1] = c->name.addr;

        r = nsi_endpoint_get_interface(c->ep, &name);
        if (r >= 0) {
            sa[1] = name.addr;
        };

        r = nai_dgram_set_opt(
            &c->io, NAI_IO_MULTICAST_DROP, (intptr_t)sa);
        if (r < 0) {
            nai_log_crit(NSI_LOG_CORE, nai_errno, 
                "dgram endpoint(%s) leave multicast failed", 
                nai_str(&c->ep->host));
            goto _end;
        };

        b->mcast = 0;
    };

    b->refs = 0;
    r = 0;

_end:
    return r;
};


static int nsi_dgram_query_ucode(nsi_dgram_t* c, nsi_connid_t conn)
{
    int r;
    nai_rbnode_t** n;
    nsi_dgram_bind_t* b;


    n = nsi_dgram_bind_find_conn(&c->names, conn, 0);
    if (n[0] == 0) {
        nai_errno = ENOENT;
        r = -1;
        goto _end;
    };

    b = nai_containof(n[0], nsi_dgram_bind_t, entc);
    r = b->uid;

_end:
    return r;
};


static int nsi_dgram_reset_ucode(nsi_dgram_t* c)
{
    int r;
    nai_rbnode_t* e;
    nsi_dgram_bind_t* b;


    e = nai_rbtree_begin(&c->conns);
    for ( ; e != nai_rbtree_end(&c->conns); ) {
        b = nai_containof(e, nsi_dgram_bind_t, entc);
        e = nai_rbtree_next(e);
        b->uid = 0;
    };

    r = 0;

    return r;
};


static int nsi_dgram_send(
    nsi_dgram_t* c, nsi_message_t* m, uint32_t uid, int flags)
{
    int r;
    nai_rbnode_t** n;
    nsi_dgram_bind_t* b;


    if (uid) {
        n = nsi_dgram_bind_find_conn(&c->conns, m->conn, 0);
        if (n[0] == 0) {
            goto _skip;
        };

        b = nai_containof(n[0], nsi_dgram_bind_t, entc);
        if (b->uid == uid) {
            goto _skip;
        };

        b->uid = uid;
    };

    if (flags & NSI_SEND_MSGDUP) {
        m = nsi_message_dup(m, 0);
        if (m == 0) {
            nai_log_alert(NSI_LOG_CORE, nai_errno, 
                "dgram endpoint(%s) dupilcate message failed", 
                nai_str(&c->ep->host));

            r = -1;
            goto _end;
        };
    };

    c->out.qsize += m->hdr.len;
    nai_list_insert_tail(&c->out.waiting, &m->ent);
    if (c->out.msg == 0 && nai_dgram_is_writable(&c->io)) {
        do {
            if (flags & NSI_SEND_POSTED) {
                if (c->posted) {
                    break;
                };
                r = nai_dgram_post(&c->io, NAI_EV_WRITE);
                if (r >= 0) {
                    c->posted = 1;
                    break;
                };

                nai_log_alert(NSI_LOG_CORE, nai_errno, 
                    "dgram endpoint(%s) do post failed, send now", 
                    nai_str(&c->ep->host));
            };

            r = nsi_dgram_handle_send(c);

        } while (0);
    };

    (void)r;

    r = 0;

_end:
    return r;

_skip:
    /* is a notify message, unneed callback sent */
    if (!(flags & NSI_SEND_MSGDUP)) {
        nsi_message_close(m);
    };
    r = 0;
    goto _end;
};


static int nsi_dgram_flush(nsi_dgram_t* c)
{
    int r;


    if (nai_dgram_is_writable(&c->io)) {
        r = nsi_dgram_handle_send(c);
    } else {
        r = 0;
    };

    return r;
};


static int nsi_dgram_close(nsi_dgram_t* c, nai_list_entry_t* msgs)
{
    int r;
    nsi_message_t* m;

    nai_log_info(NSI_LOG_CORE, nai_errno, "dgram endpoint(%s) closed", nai_str(&c->ep->host));

    r = nsi_iobase_close((nsi_iobase_t*)c, 0);
    if (r < 0) {
        nai_log_error(NSI_LOG_CORE, 
            nai_errno, "dgram endpoint(%s) close failed", 
            nai_str(&c->ep->host));
        goto _end;
    };

    if (c->in.buf) {
        nsi_endpoint_release_buf(c->ep, c->in.buf);
        c->in.buf = 0;
    };

    if (!nai_list_is_empty(&c->out.waiting)) {
        c->out.msg = 0;
        c->out.mcount = 0;
        c->out.qsize = 0;
        nai_list_add_tail(msgs, &c->out.waiting);
    };

    while (!nai_list_is_empty(&c->in.queued)) {
        m = (nsi_message_t*)c->in.queued.next;
        nai_list_entry_remove(&m->ent);
        nsi_message_close(m);
    };
    nai_bufpool_close(&c->pool);
    nsi_network_free(c->ep->net, c);
    r = 0;

_end:
    return r;
};



//////////////////////////////////////////////////////////////////////////////
// dgram server


static int nsi_dgram_server_open(nsi_endpoint_t* p)
{
    int r;
    int ec;
    nsi_network_t* n;
    nsi_dgram_t* c;
    nai_list_entry_t msgs;


    c = nsi_dgram_create(p, 0);
    if (c == 0) {
        r = -1;
        goto _end;
    };

    n = p->net;
    nai_dgram_set_opt(&c->io, NAI_IO_REUSEADDR, 1);
    nai_dgram_set_opt(&c->io, NAI_IO_SENDTIMEO, n->send_timeo);
    nai_dgram_set_polling(&c->io, n->use_polling);
    r = nai_dgram_bind(&c->io, n->loop, p->name.addr, p->name.len);
    if (r < 0) {
        ec = nai_errno;
        nai_log_crit(NSI_LOG_CORE, 
            ec, "dgram endpoint(%s) bind address failed", 
            nai_str(&p->host));
        goto _fail;
    };

    r = nsi_dgram_set_opts(c, n);
    if (r < 0) {
        ec = nai_errno;
        nai_log_crit(NSI_LOG_CORE, 
            ec, "dgram endpoint(%s) set options failed", 
            nai_str(&p->host));
        goto _fail;
    };


    c->server = 1;
    p->server = 1;
    p->connected = 0;
    p->ctx = c;
    r = 0;


_end:
    return r;

_fail:
    nai_list_init(&msgs);
    nsi_dgram_close(c, &msgs);
    nai_errno = ec;
    r = -1;
    goto _end;
};


static int nsi_dgram_server_map_conn(
    nsi_endpoint_t* p, nsi_endpoint_t* s, nsi_connid_t cid)
{
    int r;
    nsi_dgram_t* c;


    c = (nsi_dgram_t*)p->ctx;
    r = nsi_dgram_map_conn(c, s, cid);


    return r;
};


static int nsi_dgram_server_get_name(
    nsi_endpoint_t* p, nsi_connid_t cid, nsi_endpoint_name_t* name)
{
    int r;
    nsi_dgram_t* c;


    c = (nsi_dgram_t*)p->ctx;
    r = nsi_dgram_get_name(c, cid, name);


    return r;
};


static int nsi_dgram_server_get_opt(
    nsi_endpoint_t* p, int opt, intptr_t* value)
{
    int r;
    nsi_dgram_t* c;


    c = (nsi_dgram_t*)p->ctx;
    r = nai_dgram_get_opt(&c->io, opt, value);

    return r;
};


static int nsi_dgram_server_set_opt(
    nsi_endpoint_t* p, int opt, intptr_t value)
{
    int r;
    nai_rbnode_t** n;
    nsi_dgram_t* c;


    c = (nsi_dgram_t*)p->ctx;
    switch (opt) {
    case NSI_EOPT_CHECK_CSTAT:
        if (value == 0) {
            r = 1;
        } else {
            n = nsi_dgram_bind_find_conn(&c->names, (nsi_connid_t)value, 0);
            r = n[0] != 0;
        };
        break;
    case NSI_EOPT_QUERY_UCODE:
        r = nsi_dgram_query_ucode(c, (nsi_connid_t)value);
        break;
    case NSI_EOPT_RESET_UCODE:
        r = nsi_dgram_reset_ucode(c);
        break;
    case NSI_EOPT_BIND_NAME:
        r = nsi_dgram_bind_name(c, (nsi_endpoint_name_t*)value, 0, 0);
        break;
    case NSI_EOPT_BIND_TEMP:
        r = nsi_dgram_bind_name(c, (nsi_endpoint_name_t*)value, 0, 1);
        break;
    case NSI_EOPT_BIND_MCAST:
        r = nsi_dgram_bind_name(c, (nsi_endpoint_name_t*)value, 1, 0);
        break;
    case NSI_EOPT_BIND_CONN:
        r = nsi_dgram_bind_conn(c, (nsi_connid_t)value);
        break;
    case NSI_EOPT_UNBIND_NAME:
        r = nsi_dgram_unbind_name(c, (nsi_connid_t)value);
        break;
    case NSI_EOPT_REBIND_MCAST:
        r = nsi_dgram_rebind_name(c, (nsi_endpoint_name_t*)value);
        break;
    default:
        r = nai_dgram_set_opt(&c->io, opt, value);
        break;
    };

    return r;
};


static int nsi_dgram_server_send(
    nsi_endpoint_t* p, nsi_message_t* m, uint32_t uid, int flags)
{
    int r;
    nsi_dgram_t* c;
    nsi_network_t* n;


    n = p->net;
    c = (nsi_dgram_t*)p->ctx;
    if (c->out.qsize >= n->queue_mix_limit) {
        nai_errno = ENOSPC;
        r = -1;
        goto _end;
    };

    r = nsi_dgram_send(c, m, uid, flags);

_end:
    return r;
};


static int nsi_dgram_server_flush(nsi_endpoint_t* p)
{
    int r;
    nsi_dgram_t* c;


    c = (nsi_dgram_t*)p->ctx;
    r = nsi_dgram_flush(c);

    return r;
};


static int nsi_dgram_server_close(nsi_endpoint_t* p)
{
    int r;
    size_t qsize;
    nsi_dgram_t* c;
    nai_list_entry_t msgs;


    c = (nsi_dgram_t*)p->ctx;
    if (c == 0) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    /* close dgram */
    qsize = c->out.qsize;
    nai_list_init(&msgs);
    r = nsi_dgram_close(c, &msgs);
    if (r < 0) {
        goto _end;
    };

    /* callback messages */
    nsi_endpoint_handle_sent(p, &msgs, &qsize, ECANCELED);

    /* unlink */
    p->connected = 0;
    p->ctx = 0;
    r = 0;

_end:
    return r;
};



//////////////////////////////////////////////////////////////////////////////
// dgram client


static int nsi_dgram_client_open(nsi_endpoint_t* p)
{
    int r;
    int ec;
    nsi_network_t* n;
    nsi_dgram_t* c;
    nai_list_entry_t msgs;
    nsi_endpoint_name_t name;
    nai_socknbuf_in_t nbuf;


    n = p->net;
    if (p->local) {
        name = p->name;
    } else {
        nbuf.len = sizeof(nbuf.storage);
        r = nsi_network_map_route(n, 
            p->name.addr, p->name.len, &nbuf.addr, &nbuf.len, 0);
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE, 
                nai_errno, "failed to get dgram endpoint(%s) bind address", 
                nai_str(&p->host));
            goto _end;
        };

        name.addr = &nbuf.addr;
        name.len = nbuf.len;
    };

    c = nsi_dgram_create(p, &name);
    if (c == 0) {
        r = -1;
        goto _end;
    };

    if (p->waiting) {
        c->rebind = 1;
    };

    r = nsi_dgram_bind(c);
    if (r < 0) {
        ec = nai_errno;
        nai_list_init(&msgs);
        nsi_dgram_close(c, &msgs);
        nai_errno = ec;
        goto _end;
    };

    p->connected = 0;
    p->ctx = c;
    r = 0;

_end:
    return r;
};


static int nsi_dgram_client_map_conn(
    nsi_endpoint_t* p, nsi_endpoint_t* s, nsi_connid_t cid)
{
    int r;
    nsi_dgram_t* c;


    c = (nsi_dgram_t*)p->ctx;
    r = nsi_dgram_map_conn(c, s, cid);


    return r;
};


static int nsi_dgram_client_get_name(
    nsi_endpoint_t* p, nsi_connid_t cid, nsi_endpoint_name_t* name)
{
    int r;
    nsi_dgram_t* c;


    c = (nsi_dgram_t*)p->ctx;
    r = nsi_dgram_get_name(c, cid, name);


    return r;
};


static int nsi_dgram_client_get_opt(
    nsi_endpoint_t* p, int opt, intptr_t* value)
{
    int r;
    nsi_dgram_t* c;


    c = (nsi_dgram_t*)p->ctx;
    r = nai_dgram_get_opt(&c->io, opt, value);

    return r;
};


static int nsi_dgram_client_set_opt(
    nsi_endpoint_t* p, int opt, intptr_t value)
{
    int r;
    nai_rbnode_t** n;
    nsi_dgram_t* c;


    c = (nsi_dgram_t*)p->ctx;
    switch (opt) {
    case NSI_EOPT_CHECK_CSTAT:
        if (value == 0) {
            r = c->connected;
        } else {
            n = nsi_dgram_bind_find_conn(&c->names, (nsi_connid_t)value, 0);
            r = n[0] != 0;
        };
        break;
    case NSI_EOPT_QUERY_UCODE:
        r = nsi_dgram_query_ucode(c, (nsi_connid_t)value);
        break;
    case NSI_EOPT_RESET_UCODE:
        r = nsi_dgram_reset_ucode(c);
        break;
    case NSI_EOPT_BIND_NAME:
        r = nsi_dgram_bind_name(c, (nsi_endpoint_name_t*)value, 0, 0);
        break;
    case NSI_EOPT_BIND_TEMP:
        r = nsi_dgram_bind_name(c, (nsi_endpoint_name_t*)value, 0, 1);
        break;
    case NSI_EOPT_BIND_MCAST:
        r = nsi_dgram_bind_name(c, (nsi_endpoint_name_t*)value, 1, 0);
        break;
    case NSI_EOPT_BIND_CONN:
        r = nsi_dgram_bind_conn(c, (nsi_connid_t)value);
        break;
    case NSI_EOPT_UNBIND_NAME:
        r = nsi_dgram_unbind_name(c, (nsi_connid_t)value);
        break;
    default:
        r = nai_dgram_set_opt(&c->io, opt, value);
        break;
    };

    return r;
};


static int nsi_dgram_client_send(
    nsi_endpoint_t* p, nsi_message_t* m, uint32_t uid, int flags)
{
    int r;
    nsi_dgram_t* c;
    nsi_network_t* n;


    n = p->net;
    c = (nsi_dgram_t*)p->ctx;
    if (c->out.qsize >= n->queue_limit) {
        nai_errno = ENOSPC;
        r = -1;
        goto _end;
    };

    r = nsi_dgram_send(c, m, uid, flags);

_end:
    return r;
};


static int nsi_dgram_client_flush(nsi_endpoint_t* p)
{
    int r;
    nsi_dgram_t* c;


    c = (nsi_dgram_t*)p->ctx;
    r = nsi_dgram_flush(c);

    return r;
};


static int nsi_dgram_client_close(nsi_endpoint_t* p)
{
    int r;
    size_t qsize;
    nsi_dgram_t* c;
    nai_list_entry_t msgs;


    c = (nsi_dgram_t*)p->ctx;
    if (c == 0) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    /* close dgram */
    qsize = c->out.qsize;
    nai_list_init(&msgs);
    r = nsi_dgram_close(c, &msgs);
    if (r < 0) {
        goto _end;
    };

    /* callback messages */
    nsi_endpoint_handle_sent(p, &msgs, &qsize, ECANCELED);

    /* unlink */
    p->connected = 0;
    p->ctx = 0;
    r = 0;

_end:
    return r;
};



nsi_endpoint_ops_t nsi_dgram_server_ops = {
    nsi_dgram_server_open, 
    nsi_dgram_server_map_conn, 
    nsi_dgram_server_get_name, 
    nsi_dgram_server_get_opt, 
    nsi_dgram_server_set_opt, 
    nsi_dgram_server_send, 
    nsi_dgram_server_flush, 
    nsi_dgram_server_close
};

nsi_endpoint_ops_t nsi_dgram_client_ops = {
    nsi_dgram_client_open, 
    nsi_dgram_client_map_conn, 
    nsi_dgram_client_get_name, 
    nsi_dgram_client_get_opt, 
    nsi_dgram_client_set_opt, 
    nsi_dgram_client_send, 
    nsi_dgram_client_flush, 
    nsi_dgram_client_close
};


