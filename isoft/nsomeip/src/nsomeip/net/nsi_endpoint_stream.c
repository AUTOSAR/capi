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
/// @file       nsi_endpoint_stream.c
/// @brief
/// @details
/// @date       2021-05-11
/// @author     xn
/// @version    1.2.0
///
/// ================================================================

#include "nsomeip/core/nsi_log.h"
#include "nsomeip/core/nsi_proxy.h"
#include "nsi_network.h"
#include "nai/os/nai_socket.h"
#include "nai/io/nai_io.h"
#include "nai/io/nai_ssl.h"
#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_util.h"



typedef struct nsi_endpoint_bind_s nsi_stream_bind_t;


typedef struct nsi_stream_s {
    /* header some as nsi_iobase_t */
    nai_stream_t io;

    /* bind name */
    nai_rbnode_t node;
    nsi_endpoint_name_t name;

    /* endpoint */
    nsi_endpoint_t* ep;

    /* connection node */
    nai_rbnode_t ent;
    uint32_t cid;
    uint32_t uid;

    /* peer pid */
    int pid;

    /* flags and stats */
    union {
        struct {
            uint32_t server:1;
            uint32_t connected:1;
            uint32_t ssl_initalized:1;
            uint32_t ssl_handshaked:1;
            uint32_t posted:1;
            uint32_t read:2;
            uint32_t reconnect:1;
            uint32_t closing:1;
            uint32_t retry:8;
        };
        uint32_t flags;
    };

    /* for read */
    struct {
        nai_buf_t* buf;             /**< pointer to the read buffer */
        nsi_message_t* incoming;    /**< pointer to the incoming message */
        size_t msglen;              /**< the left size of message to receive */
        struct {
            nsi_serv_t serv;        /**< the service id */
            nsi_method_t method;    /**< the method id */
            int16_t sig;            /**< is signal */
        } evt;                      /**< the lookup cache of event */

        int16_t rcvlowat;           /**< the value of lowat */
    } in;


    /* for send */
    struct {
        nai_buflist_t bufs;         /**< the list of sending buffers */
        nai_bufwalk_t walk;         /**< the buf walk */
        nai_list_entry_t queued;    /**< the list of sending messages */
        nai_list_entry_t waiting;   /**< the list of waiting messages */
        size_t qsize;               /**< the size of queued in bytes */
        size_t bsize;               /**< the size of buffer vectors */
        int16_t mcount;             /**< the count of sending messages */
        int16_t vcount;             /**< the count of buffer in sending */
        int16_t vnext;              /**< the next index of buffer vector */
        int16_t vmore;              /**< is no more buffer vector */
        struct {
            nsi_serv_t serv;        /**< the service id */
            nsi_inst_t inst;        /**< the instance id */
            nsi_method_t method;    /**< the method id */
            int16_t sig;            /**< is signal */
        } evt;                      /**< the lookup cache of event */

        uint8_t zerocopy;
        uint8_t znotified;
        uint8_t zwaiting;
        uint8_t zpending;

        nai_bufvec_t va[NAI_BUFV_MAX];
    } out;

#if defined(NAI_HAVE_SSL)
    /* SSL Connection */
    nai_ssl_t ssl;
#endif

} nsi_stream_t;


typedef struct nsi_stream_server_s {
    nai_server_t io;                /**< listen socket */
#if defined(NAI_HAVE_SSL)
    nai_ssl_t ssl;                  /**< SSL connection */
#endif
    nai_bufpool_t pool;             /**< allocate */
    nai_rbtree_t clients;           /**< all clients */
    nai_rbtree_t conns;             /**< map of bind connection id */
    nai_rbtree_t names;             /**< map of bind address */
    nsi_endpoint_t* ep;             /**< pointer to the endpoint */
    uint32_t nextc;                 /**< next connection id */
    nai_socknbuf_t nbuf;            /**< accept address */
} nsi_stream_server_t;



//////////////////////////////////////////////////////////////////////////////
// stream


/* stream read stats */
#define NSI_STREAM_WAIT             0
#define NSI_STREAM_HEAD             1
#define NSI_STREAM_PAYLOAD          2


static int nsi_stream_connect(nsi_stream_t* c);
static int nsi_stream_reset(nsi_stream_t* c, nai_list_entry_t* msgs);
static int nsi_stream_close(nsi_stream_t* c, nai_list_entry_t* msgs);

static int nsi_stream_tls_open(void *nsi_stream, int listener);
static int nsi_stream_tls_handshake(nsi_stream_t* c);
static void nsi_stream_tls_close(void *nsi_stream, int listener);

static nai_rbnode_t** nsi_stream_find(
    nai_rbtree_t* t, uint32_t cid, nai_rbnode_t** pparent)
{
    nai_rbnode_t** n = &nai_rbtree_root(t);
    nai_rbnode_t* parent = nai_rbtree_end(t);
    nsi_stream_t* e;


    while (*n) {
        parent = *n;
        e = nai_containof(parent, nsi_stream_t, ent);
        if (e->cid == cid) {
            break;
        } else if (e->cid >= cid) {
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


static int nsi_stream_realloc_buf(nsi_stream_t* c)
{
    return nsi_endpoint_realloc_buf(c->ep, &c->in.buf);
};


static int nsi_stream_incoming_is_signal(nsi_stream_t* c, nsi_msghdr_t* h)
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


static int nsi_stream_message_is_signal(nsi_stream_t* c, nsi_message_t* m)
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


static int nsi_stream_handle_connect(nsi_stream_t* c)
{
    int r;
    int recursion;
    nsi_network_t* net;
    nsi_endpoint_t* ep;


    ep = c->ep;
    if (ep->local) {

#if defined(SO_PEERCRED)

        int len;
        struct ucred cred;


        len = sizeof(cred);
        r = nai_sock_get_opt(
            nai_stream_get_fd(&c->io), 
            SOL_SOCKET, SO_PEERCRED, (char*)&cred, &len);
        if (r >= 0) {
            c->pid = cred.pid;
        } else {
            c->pid = 0;
        };

#elif defined(LOCAL_PEERPID)

        int len;
        int pid;


        len = sizeof(pid);
        r = nai_sock_get_opt(
            nai_stream_get_fd(&c->io), 
            SOL_LOCAL, LOCAL_PEERPID, (char*)&pid, &len);
        if (r >= 0) {
            c->pid = pid;
        } else {
            c->pid = 0;
        };

#elif defined(LOCAL_PEERCRED)

        int len;
        struct xucred cred;


        len = sizeof(cred);
        r = nai_sock_get_opt(
            nai_stream_get_fd(&c->io), 
            SOL_LOCAL, LOCAL_PEERCRED, (char*)&cred, &len);
        if (r >= 0) {
            c->pid = cred.pid;
        } else {
            c->pid = 0;
        };

#elif defined(_WIN32)
        c->pid = 0;
#elif (__qnx__)
        /* So far, there is no suitable API to implement it in qnx 7.1. */
        c->pid = 0;
#else
#warning "the platform can not get peer pid."
#endif

    };

    net = c->ep->net;

    if (c->ep->pexit) {
        recursion = 1;
    } else {
        recursion = 0;
    };

    r = nsi_endpoint_handle_connect(c->ep, c->cid);

    if (c->closing) {
        if (!recursion) {
            nsi_network_free(net, c);
        };
        r = 0;
    };

    return r;
};


static int nsi_stream_handle_incoming(nsi_stream_t* c, nsi_message_t* m)
{
    int r;
    int recursion;
    nsi_network_t* net;

    net = c->ep->net;

    if (c->ep->pexit) {
        recursion = 1;
    } else {
        recursion = 0;
    };

    r = nsi_endpoint_handle_incoming(c->ep, m);

    if (c->closing) {
        if (!recursion) {
            nsi_network_free(net, c);
        };
        r = 0;
    };

    return r;
};


static int nsi_stream_handle_sent(
    nsi_stream_t* c, nai_list_entry_t* list, int errcode)
{
    int r;
    int recursion;
    nsi_network_t* net;

    net = c->ep->net;

    if (c->ep->pexit) {
        recursion = 1;
    } else {
        recursion = 0;
    };

    r = nsi_endpoint_handle_sent(c->ep, list, &c->out.qsize, errcode);

    if (c->closing) {
        if (!recursion) {
            nsi_network_free(net, c);
        };
        r = 0;
    };

    return r;
};


static int nsi_stream_handle_except(nsi_stream_t* c, int errcode, int alert)
{
    int r;
    int n;
    int ec;
    int recursion;
    size_t qsize;
    nsi_endpoint_t* e;
    nsi_network_t* net;
    nsi_stream_server_t* s;
    nai_list_entry_t msgs;

    e = c->ep;
    net = e->net;

    if (c->ep->pexit) {
        recursion = 1;
    } else {
        recursion = 0;
    };

    r = nsi_endpoint_handle_except(e, c->cid, errcode, alert);

    if (c->closing) {
        if (!recursion) {
            nsi_network_free(net, c);
        };
        r = 0;
    };

    if (r != NAI_DECLINED) {
        goto _end;
    };

    if (!alert) {
        qsize = c->out.qsize;
        nai_list_init(&msgs);

        /* close stream */
        if (c->server) {
            s = (nsi_stream_server_t*)e->ctx;
            nai_rbtree_erase(&s->clients, &c->ent);
            nsi_stream_tls_close(c, 0);
            r = nsi_stream_close(c, &msgs);
        } else {
            /* do reconnect */
            r = nsi_stream_reset(c, &msgs);
        };
        if (r < 0) {
            nai_log_alert(NSI_LOG_CORE, nai_errno, 
                "nsi_stream_%s failed", c->server ? "close" : "reset");
        };

        /* release message */
        if (!nai_list_is_empty(&msgs)) {
            if (r < 0) {
                ec = nai_errno;
            };

            n = nsi_endpoint_handle_sent(e, &msgs, &qsize, ECANCELED);
            if (n != NAI_DECLINED) {
                goto _end;
            };

            if (r < 0) {
                nai_errno = ec;
            };
        };
    };


_end:
    return r;
};


static int nsi_stream_handle_alert(nsi_stream_t* c, int errcode)
{
    return nsi_stream_handle_except(c, errcode, 1);
};


static int nsi_stream_handle_error(nsi_stream_t* c, int errcode)
{
    return nsi_stream_handle_except(c, errcode, 0);
};

#if defined(NAI_HAVE_SSL)
static int nsi_stream_read(nsi_stream_t* c, void *buf, size_t len){
    int r;

    if (!c->ep->tls) {
        r = nai_stream_read(&c->io, buf, len);
    } else {
        r = nai_ssl_read(&c->ssl, buf, len);
    }

    return r;
}
#else
static int nsi_stream_read(nsi_stream_t* c, void *buf, size_t len){
    int r;

    r = nai_stream_read(&c->io, buf, len);

    return r;
}
#endif

static int nsi_stream_handle_read(nsi_stream_t* c)
{
    int ec;
    int enough;
    int is_signal;
    intptr_t r;
    size_t size;
    size_t total;
    size_t needspace;
    nai_buf_t* b;
    nai_buf_t* d;
    nsi_msghdr_t* h;
    nsi_msghdr_t hdr;
    nsi_message_t* m;
    nsi_network_t* n;
    nsi_endpoint_t* ep;


    if (c->in.buf == 0) {
        r = nsi_stream_realloc_buf(c);
        if (r < 0) {
            ec = nai_errno;
            nai_log_alert(NSI_LOG_CORE, ec, 
                "stream endpoint(%s/%d) failed to create read buffer", 
                nai_str(&c->ep->host), c->cid);
            goto _alert;
        };
    };

    ep = c->ep;
    n = ep->net;
    b = c->in.buf;
    while (1) {
        size = nai_buf_size(b);
        total = nai_buf_total(b);
        assert(total > size);

        r = nsi_stream_read(c, nai_buf_ptr(b) + size, total - size);

        if (r < 0) {
            ec = nai_errno;
            if (ec == NAI_EAGAIN || ec == NAI_EINPROGRESS) {
#if defined(SO_RCVLOWAT)
                int val;

                if (ec == NAI_EAGAIN && !ep->local) {
                    if (c->in.incoming && (c->in.msglen-size) > 64*1024) {
                        val = 64*1024;
                    } else {
                        val = 1;
                    };
                    if (val != c->in.rcvlowat) {
                        r = nai_sock_set_opt(
                            nai_stream_get_fd(&c->io), 
                            SOL_SOCKET, SO_RCVLOWAT, (char*)&val, sizeof(val));
                        if (r < 0) {
                            ec = nai_errno;
                            nai_log_crit(NSI_LOG_CORE, ec, 
                                "stream endpoint(%s/%d) "
                                "set rcvlowat(%d) failed", 
                                nai_str(&c->ep->host), c->cid, val);
                            goto _error;
                        };

                        c->in.rcvlowat = (int16_t)val;
                    };
                };
#endif
                r = NAI_DECLINED;
                goto _end;
            };

            nai_log_crit(NSI_LOG_CORE, 
                ec, "stream endpoint(%s/%d) recvive failed", 
                nai_str(&c->ep->host), c->cid);
            goto _error;
        };
        if (r == 0) {
            nai_log_debug(NSI_LOG_CORE, 0, 
                "stream endpoint(%s/%d) receive(%d) to the end", 
                nai_str(&c->ep->host), c->cid, (uint32_t)(total - size));

            r = nsi_stream_handle_error(c, ECONNABORTED);
            goto _end;
        };

        size += r;
        nai_buf_wcommit(b, r);


        enough = 1;
        while (enough) {
            switch (c->read) {
            case NSI_STREAM_WAIT:
                nai_log_debug(NSI_LOG_CORE, 0, 
                    "stream endpoint(%s/%d) start recv_timer", 
                    nai_str(&c->ep->host), c->cid);

                r = nai_stream_set_opt(
                    &c->io, NAI_IO_RECVTIMEO, n->recv_timeo);
                if (r < 0) {
                    nai_log_crit(NSI_LOG_CORE, nai_errno, 
                        "stream endpoint(%s/%d) failed to set recv_timer", 
                        nai_str(&c->ep->host), c->cid);
                };

                c->read = NSI_STREAM_HEAD;

                /* fallthrough */

            case NSI_STREAM_HEAD:
                if (size < NSI_MSGHDR_SIZE) {
                    enough = 0;
                    needspace = sizeof(nsi_msghdr_t) - size;
                    break;
                };

                h = (nsi_msghdr_t*)nai_buf_ptr(b);
                nsi_msghdr_ntoh_ua_1st_half(&hdr, h);

                if (hdr.len > n->msg_limit) {
                    ec = EMSGSIZE;
                    nai_log_error(NSI_LOG_CORE, ec, 
                        "stream endpoint(%s/%d) "
                        "get message with invalid size(%d)", 
                        nai_str(&c->ep->host), c->cid, hdr.len);
                    goto _error;
                };

                if (c->ep->local) {
                    is_signal = 0;
                } else {
                    is_signal = nsi_stream_incoming_is_signal(c, &hdr);
                };

                if (is_signal) {
                    /* keep the same length as a normal head */
                    hdr.len += NSI_MSGEXT_SIZE - NSI_SIGEXT_SIZE;
                    hdr.client = 0;
                    hdr.session = 0;
                    hdr.type = NSI_MT_NOTIFICATION;
                    hdr.code = 0;
                    hdr.interface = -1;
                    hdr.protocol = -1;
                } else {
                    if (size < sizeof(nsi_msghdr_t)) {
                        enough = 0;
                        needspace = sizeof(nsi_msghdr_t) - size;
                        break;
                    };
                    nsi_msghdr_ntoh_ua_2nd_half(&hdr, h);
                };

                m = c->in.incoming = 
                    nsi_network_create_message(n, NSI_USAGE_READ);
                if (m == 0) {
                    ec = nai_errno;
                    nai_log_alert(NSI_LOG_CORE, ec, 
                        "stream endpoint(%s/%d) failed to create message", 
                        nai_str(&c->ep->host), c->cid);
                    goto _alert;
                };

                m->hdr = hdr;
                m->inst = 0;
                m->conn = c->cid;
                m->reliable = 1;

                if (ep->local) {
                    m->cred.type = NSI_CRED_PID;
                    m->cred.len = sizeof(m->cred.pid);
                    m->cred.pid = c->pid;
                } else if (c->server) {
                    m->cred.type = NSI_CRED_ADDRESS;
                    m->cred.len = c->name.len;
                    nai_memcpy(&m->cred.addr, c->name.addr, c->name.len);
                } else {
                    m->cred.type = NSI_CRED_ADDRESS;
                    m->cred.len = ep->name.len;
                    nai_memcpy(&m->cred.addr, ep->name.addr, ep->name.len);
                };

                if (is_signal) {
                    size -= NSI_MSGHDR_SIZE;
                    nai_buf_rcommit(b, NSI_MSGHDR_SIZE);
                } else {
                    size -= sizeof(nsi_msghdr_t);
                    nai_buf_rcommit(b, sizeof(nsi_msghdr_t));
                };
                c->read = NSI_STREAM_PAYLOAD;
                c->in.msglen = m->hdr.len - (
                    NSI_MSGEXT_SIZE - NSI_SIGEXT_SIZE);

                /* fallthrough */

            case NSI_STREAM_PAYLOAD:
                m = c->in.incoming;
                if (size < c->in.msglen) {
                    total = nai_buf_total(b);
                    if (total <= size) {
                        d = nai_buf_sub(m->payload.pool, b, 0, size, 1);
                        if (d == 0) {
                            ec = nai_errno;
                            nai_log_alert(NSI_LOG_CORE, ec, 
                                "stream endpoint(%s/%d) "
                                "failed to create sub-buffer", 
                                nai_str(&c->ep->host), c->cid);
                            goto _alert;
                        };

                        nai_buf_rcommit(b, size);
                        nai_buflist_insert_tail(&m->payload, d);
                        c->in.msglen -= size;
                        size = 0;
                    };

                    enough = 0;
                    needspace = 0;

                } else {
                    d = nai_buf_sub(m->payload.pool, b, 0, c->in.msglen, 1);
                    if (d == 0) {
                        ec = nai_errno;
                        nai_log_alert(NSI_LOG_CORE, ec, 
                            "stream endpoint(%s/%d) "
                            "failed to create sub-buffer", 
                            nai_str(&c->ep->host), c->cid);
                        goto _alert;
                    };

                    nai_buf_rcommit(b, c->in.msglen);
                    nai_buflist_insert_tail(&m->payload, d);
                    size -= c->in.msglen;
                    c->in.msglen = 0;
                    c->in.incoming = 0;

                    r = nsi_stream_handle_incoming(c, m);
                    if (r != NAI_DECLINED) {
                        goto _end;
                    };

                    if (size > 0) {
                        c->read = NSI_STREAM_HEAD;
                    } else {
                        nai_log_debug(NSI_LOG_CORE, 0, 
                            "stream endpoint(%s/%d) stop recv_timer", 
                            nai_str(&c->ep->host), c->cid);

                        enough = 0;
                        needspace = sizeof(nsi_msghdr_t);
                        c->read = NSI_STREAM_WAIT;
                        r = nai_stream_set_opt(&c->io, NAI_IO_RECVTIMEO, -1);
                        if (r < 0) {
                            nai_log_crit(NSI_LOG_CORE, nai_errno, 
                                "stream endpoint(%s/%d) "
                                "failed to cancel recv_timer", 
                                nai_str(&c->ep->host), c->cid);
                        };
                    };
                };

                break;

            default:
                assert(0);
                ec = EINVAL;
                goto _error;
            };
        };

        /**
         * the condition of realloc:
         * 1. space is not enough
         * 2. no space and content
         */
        total = nai_buf_total(b);
        if (total - size < needspace || (
            total - size == 0 && size == 0)) {
            r = nsi_stream_realloc_buf(c);
            if (r < 0) {
                ec = nai_errno;
                nai_log_alert(NSI_LOG_CORE, ec, 
                    "stream endpoint(%s/%d) failed to create read buffer", 
                    nai_str(&c->ep->host), c->cid);
                goto _alert;
            };

            b = c->in.buf;
        };
    };

    r = NAI_DECLINED;

_end:
    return (int)r;

_error:
    r = nsi_stream_handle_error(c, ec);
    goto _end;

_alert:
    r = nsi_stream_handle_alert(c, ec);
    goto _end;
};


#if defined(SO_ZEROCOPY) && defined(MSG_ZEROCOPY) && !defined(NAI_HAVE_SSL)


#include <linux/errqueue.h>


static int nsi_stream_handle_zc_stat(nsi_stream_t* c)
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
        nai_stream_get_fd(&c->io), 
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

#if defined(NAI_HAVE_SSL)
static int nsi_stream_write(nsi_stream_t* c, int flags)
{
    int r;

    if (c->ep->tls) {
        r = nai_ssl_write(&c->ssl, c->out.va[c->out.vnext].buf, c->out.va[c->out.vnext].len);
    } else {
        r = nai_stream_sendm(&c->io,
            c->out.va + c->out.vnext,
            c->out.vcount - c->out.vnext, flags, 0, 0, 0, 0);
    }

    return r;
}
#else
static int nsi_stream_write(nsi_stream_t* c, int flags)
{
    int r;

    r = nai_stream_sendm(&c->io,
        c->out.va + c->out.vnext,
        c->out.vcount - c->out.vnext, flags, 0, 0, 0, 0);

    return r;
}
#endif

static int nsi_stream_handle_send(nsi_stream_t* c)
{
    int ec;
    int len;
    int is_signal;
    int flags;
    intptr_t r;
    nai_buf_t* b;
    nsi_msghdr_t* h;
    nsi_message_t* m;
    nai_bufvec_t* v;
    nai_bufarray_t va;


    while (1) {
        while (c->out.mcount > 0) {
            assert(!nai_buflist_is_empty(&c->out.bufs));
            assert(!nai_list_is_empty(&c->out.queued));

            while (c->out.vnext < c->out.vcount) {
#if defined(SO_ZEROCOPY) && defined(MSG_ZEROCOPY) && !defined(NAI_HAVE_SSL)
                if (c->out.zerocopy && 
                    c->out.bsize >= (size_t)(
                    c->out.vcount - c->out.vnext) * 10*1024) {
                    flags = MSG_ZEROCOPY;
                } else {
                    flags = 0;
                };
#else
                flags = 0;
#endif
                r = nsi_stream_write(c, flags);
                if (r < 0) {
                    ec = nai_errno;
                    if (ec == NAI_EAGAIN || ec == NAI_EINPROGRESS) {
                        r = NAI_DECLINED;
                        goto _end;
                    };

                    nai_log_crit(NSI_LOG_CORE, 
                        ec, "stream endpoint(%s/%d) send failed", 
                        nai_str(&c->ep->host), c->cid);
                    goto _error;
                };

#if defined(SO_ZEROCOPY) && defined(MSG_ZEROCOPY) && !defined(NAI_HAVE_SSL)
                c->out.zpending += flags ? 1 : 0;
#endif
                c->out.bsize -= r;
                for ( ; c->out.vnext < c->out.vcount; ) {
                    v = &c->out.va[c->out.vnext];
                    if (r < (intptr_t)v->len) {
                        v->len -= r;
                        v->buf += r;
                        break;
                    };

                    r -= v->len;
                    c->out.vnext ++;
                };

                if (c->out.vnext >= c->out.vcount) {
                    c->out.vcount = 0;
                } else {
                    if (c->out.bsize > 256*1024 || 
                        c->out.vmore == 0) {
                        continue;
                    };

                    if (c->out.vnext) {
                        c->out.vcount -= c->out.vnext;
                        nai_memmove(c->out.va, 
                            c->out.va + c->out.vnext, 
                            c->out.vcount * sizeof(c->out.va[0]));
                    };
                };

                break;
            };

            r = nai_bufwalk_to_rbufarray(&c->out.walk, &va, 
                c->out.va + c->out.vcount, 
                nai_countof(c->out.va) - c->out.vcount, 
                1024*1024 - c->out.bsize, 0);
            if (r < 0) {
                ec = nai_errno;
                nai_log_alert(NSI_LOG_CORE, ec, 
                    "stream endpoint(%s) get send bufarray failed", 
                    nai_str(&c->ep->host));

                goto _alert;
            };

            if (r > 0) {
                c->out.vcount += (int)va.count;
                c->out.vnext = 0;
                c->out.bsize += r;
                continue;
            };
            if (c->out.bsize > 0) {
                assert(c->out.vcount > 0);
                assert(c->out.vmore != 0);
                c->out.vnext = 0;
                c->out.vmore = 0;
                continue;
            };

#if defined(SO_ZEROCOPY) && defined(MSG_ZEROCOPY) && !defined(NAI_HAVE_SSL)
            if (c->out.zpending) {
                c->out.zwaiting = 1;
                r = nsi_stream_handle_zc_stat(c);
                if (r < 0) {
                    ec = nai_errno;
                    nai_log_crit(NSI_LOG_CORE, ec, 
                        "stream endpoint(%s/%d) wait zc failed", 
                        nai_str(&c->ep->host), c->cid);
                    goto _error;
                };
                if (r <= 0) {
                    r = NAI_DECLINED;
                    goto _end;
                };

                /* zerocopy completed */
            };
#endif

            assert(c->out.vcount <= 0);
            c->out.mcount = 0;
            nai_buflist_close(&c->out.bufs);
            r = nsi_stream_handle_sent(c, &c->out.queued, 0);
            if (r != NAI_DECLINED) {
                goto _end;
            };
        };

        while (c->out.mcount < 8 && 
            !nai_list_is_empty(&c->out.waiting)) {

            m = (nsi_message_t*)c->out.waiting.next;
            if (m->hdr.type != NSI_MT_NOTIFICATION || 
                c->ep->local) {
                is_signal = 0;
            } else {
                is_signal = nsi_stream_message_is_signal(c, m);
            };


            /* allocate header buffer */
            b = nai_buf_alloc(m->payload.pool, sizeof(m->hdr));
            if (b == 0) {
                ec = nai_errno;
                nai_log_alert(NSI_LOG_CORE, ec, 
                    "stream endpoint(%s/%d) failed to create buffer", 
                    nai_str(&c->ep->host), c->cid);
                goto _alert;
            };


            /* fill header */
            h = (nsi_msghdr_t*)nai_buf_ptr(b);
            nsi_msghdr_hton(h, &m->hdr);

            len = m->hdr.len;
            if (is_signal) {
                len -= NSI_MSGEXT_SIZE - NSI_SIGEXT_SIZE;
                h->len = nai_htonl(len);
                nai_buf_wcommit(b, NSI_MSGHDR_SIZE);
            } else {
                nai_buf_wcommit(b, sizeof(m->hdr));
            };

            /* add head to payload */
            nai_buflist_insert_head(&m->payload, b);


            /* add message to output queue */
            nai_list_entry_remove(&m->ent);
            nai_list_insert_tail(&c->out.queued, &m->ent);


            /* move payload to output buffers */
            len += NSI_MSGHDR_SIZE;
            c->out.bufs.pool = m->payload.pool;
            r = nai_buflist_move(&c->out.bufs, &m->payload, len, 0, 0);
            if (r < 0) {
                ec = nai_errno;
                nai_log_alert(NSI_LOG_CORE, ec, 
                    "stream endpoint(%s/%d) failed to move buffer", 
                    nai_str(&c->ep->host), c->cid);
                goto _alert;
            };
            if (r != len) {
                nai_log_error(NSI_LOG_CORE, EINVAL, 
                    "stream endpoint(%s/%d) send a message(%d) "
                    "which length(%d) isn't equal "
                    "with the length(%d) of payload", 
                    nai_str(&c->ep->host), c->cid, m->hdr.method, 
                    (uint32_t)(len - NSI_MSGHDR_SIZE), 
                    (uint32_t)(r - NSI_MSGHDR_SIZE));
                assert(0);

                /* fix the length */
                m->hdr.len = (uint32_t)(r - NSI_MSGHDR_SIZE);
                c->out.qsize += r - len;

                /* write to buffer again */
                h->len = nai_htonl(m->hdr.len);

                /* fix the length of signal header */
                if (is_signal) {
                    m->hdr.len += NSI_MSGEXT_SIZE - NSI_SIGEXT_SIZE;
                };
            };

            c->out.mcount ++;
        };

        if (c->out.mcount <= 0) {
            break;
        };

        c->out.vcount = 0;
        c->out.vnext = 0;
        c->out.vmore = 1;
        c->out.bsize = 0;
        nai_bufwalk_init(&c->out.walk, &c->out.bufs);
    };

    r = NAI_DECLINED;

_end:
    return (int)r;

_error:
    r = nsi_stream_handle_error(c, ec);
    goto _end;

_alert:
    r = nsi_stream_handle_alert(c, ec);
    goto _end;
};


static int nsi_stream_handle(nai_stream_t* p, int events)
{
    int r = 0;
    int ec;
    nsi_stream_t* c;


    c = (nsi_stream_t*)p;

#if defined(SO_ZEROCOPY) && defined(MSG_ZEROCOPY) && !defined(NAI_HAVE_SSL)
    if (events & NAI_EV_ERROR) {
        ec = nai_ev_error_code(events);
        if (ec == 0) {
            c->out.znotified = 1;
            r = nsi_stream_handle_zc_stat(c);
            if (r < 0) {
                ec = nai_errno;
                nai_log_crit(NSI_LOG_CORE, ec, 
                    "stream endpoint(%s/%d) wait zc failed", 
                    nai_str(&c->ep->host), c->cid);

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

        if (c->reconnect) {
            r = NAI_DECLINED;
            goto _end;
        };
        if (events & NAI_EV_TIMEOUT) {
            r = nsi_stream_handle_error(c, ETIMEDOUT);
            goto _end;
        };

        if (c->connected == 0) {
            c->connected = 1;
            c->retry = 0;
            r = nsi_stream_handle_connect(c);
            if (r != NAI_DECLINED) {
                goto _end;
            };
        };

        if (c->ep->tls && c->ssl_handshaked == 0) {
            r = nsi_stream_tls_handshake(c);
            goto _end;
        }

        if (events & NAI_EV_READ) {
            r = nsi_stream_handle_read(c);
            if (r != NAI_DECLINED) {
                goto _end;
            };
        };

        if (events & NAI_EV_WRITE) {
            r = nsi_stream_handle_send(c);
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
                if (!c->connected)
                    goto _end;
                if (c->ep->tls && c->ssl_handshaked == 0)
                    goto _end;
                r = nsi_stream_handle_send(c);
                goto _end;
            case NAI_EV_READ:
                r = nsi_stream_handle(&c->io, NAI_EV_READ);
                goto _end;
            default:
                break;
            };

            break;

        case NAI_EV_TIMEOUT:
            if (c->reconnect) {
                nsi_stream_connect(c);
            };

            break;

        case NAI_EV_ERROR:
            ec = nai_ev_error_code(events);
            r = nsi_stream_handle_error(c, ec);
            goto _end;

        default:
            /* nothing */
            break;
        };
    };

    r = NAI_DECLINED;

_end:
    return r;
};



static nsi_stream_t* nsi_stream_create(
    nsi_endpoint_t* e, const nsi_endpoint_name_t* name)
{
    int len;
    nsi_stream_t* c;


    len = name ? name->len : 0;
    c = (nsi_stream_t*)nsi_network_alloc(e->net, sizeof(*c) + len);
    if (c == 0) {
        nai_log_alert(NSI_LOG_CORE, nai_errno, 
            "allocate stream endpoint(%s/%d) failed", 
            nai_str(&e->host), 0);
        goto _end;
    };

    c->cid = 0;
    c->uid = 0;
    c->pid = 0;
    c->ep = e;
    c->server = 0;
    c->flags = 0;
    c->in.buf = 0;
    c->in.incoming = 0;
    c->in.msglen = 0;
    c->in.evt.serv = NSI_SERVICE_ANY;
    c->in.evt.method = NSI_METHOD_ANY;
    c->in.rcvlowat = 1;
    c->out.qsize = 0;
    c->out.mcount = 0;
    c->out.vcount = 0;
    c->out.vnext = 0;
    c->out.evt.serv = NSI_SERVICE_ANY;
    c->out.evt.inst = NSI_INSTANCE_ANY;
    c->out.evt.method = NSI_METHOD_ANY;
    c->out.zerocopy = 0;
    c->out.zpending = 0;
    c->out.zwaiting = 0;
    c->out.znotified = 0;
    nai_buflist_init(&c->out.bufs, 0);
    nai_list_init(&c->out.queued);
    nai_list_init(&c->out.waiting);

    if (name == 0) {
        c->name = e->name;
    } else {
        c->name.addr = (nai_sockaddr_t*)(c + 1);
        c->name.len = len;
        nai_memcpy(c->name.addr, name->addr, len);
    };

    nai_stream_init(&c->io);
    nai_stream_set_cb(&c->io, nsi_stream_handle);


_end:
    return c;
};


static int nsi_stream_set_opts(nsi_stream_t* c, nsi_network_t* n)
{
    int r;

#if defined(SO_SNDLOWAT) || defined(SO_ZEROCOPY) || defined(SO_KEEPALIVE)
    int val;
#endif
#if defined(SO_SNDLOWAT)
    int ec;
    intptr_t bsize;
#endif


#if defined(SO_ZEROCOPY) && defined(MSG_ZEROCOPY) && !defined(NAI_HAVE_SSL)
    if (n->use_zerocopy && !c->ep->local) {
        val = 1;
        r = nai_sock_set_opt(
            nai_stream_get_fd(&c->io), 
            SOL_SOCKET, SO_ZEROCOPY, (char*)&val, sizeof(val));
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE, nai_errno, 
                "stream endpoint(%s/%d) set zc(%d) failed", 
                nai_str(&c->ep->host), c->cid, val);
        } else {
            r = nai_stream_set_except(&c->io, 1);
            if (r < 0) {
                nai_log_crit(NSI_LOG_CORE, nai_errno, 
                    "stream endpoint(%s/%d) set except(%d) failed", 
                    nai_str(&c->ep->host), c->cid, 1);
                goto _end;
            };

            c->out.zerocopy = 1;
        };
    };
#endif

    if (n->recv_buf_tcp != (uint32_t)-1) {
        r = nai_stream_set_opt(&c->io, NAI_IO_RECVBUF, n->recv_buf_tcp);
        if (r < 0) {
            nai_log_crit(NSI_LOG_CORE, nai_errno, 
                "stream endpoint(%s) set recv bufsize(%d) failed", 
                nai_str(&c->ep->host), n->recv_buf_tcp);
            goto _end;
        };
    };
    if (n->send_buf_tcp != (uint32_t)-1) {
        r = nai_stream_set_opt(&c->io, NAI_IO_SENDBUF, n->send_buf_tcp);
        if (r < 0) {
            nai_log_crit(NSI_LOG_CORE, nai_errno, 
                "stream endpoint(%s) set send bufsize(%d) failed", 
                nai_str(&c->ep->host), n->send_buf_tcp);
            goto _end;
        };
    };

#if defined(SO_SNDLOWAT)
    r = nai_stream_get_opt(&c->io, NAI_IO_SENDBUF, &bsize);
    if (r < 0) {
        nai_log_crit(NSI_LOG_CORE, nai_errno, 
            "stream endpoint(%s) get send bufsize failed", 
            nai_str(&c->ep->host));
        goto _end;
    };

    val = bsize / 2;
    if (val >= 4096) {
        r = nai_sock_set_opt(
            nai_stream_get_fd(&c->io), 
            SOL_SOCKET, SO_SNDLOWAT, (char*)&val, sizeof(val));
        if (r < 0) {
            ec = nai_errno;
            if (ec != ENOPROTOOPT) {
                nai_log_crit(NSI_LOG_CORE, ec, 
                    "stream endpoint(%s/%d) set sndlowat(%d) failed", 
                    nai_str(&c->ep->host), c->cid, val);
                goto _end;
            };
        };
    };
#endif

#if defined(__linux__)
/* the macro for linux is to make the platform more explicit */
#if defined(SO_KEEPALIVE) && defined(TCP_KEEPIDLE) && defined(TCP_KEEPCNT) && defined(TCP_KEEPINTVL)
    if (n->keepalive_enable && (!c->ep->local) && (!c->ep->server)) {
    /*only reliable non-local endpoint should apply, see PRS_SOMEIPSD_00527 */

        val = n->keepalive_enable;
        r = nai_sock_set_opt(
            nai_stream_get_fd(&c->io),
            SOL_SOCKET, SO_KEEPALIVE, (char*)&val, sizeof(val));
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE, nai_errno,
                "stream endpoint(%s/%d) set keepalive_enable(%d) failed",
                nai_str(&c->ep->host), c->cid, val);
            goto _end;
        }

        val = n->keepalive_idle;
        r = nai_sock_set_opt(
            nai_stream_get_fd(&c->io),
            IPPROTO_TCP, TCP_KEEPIDLE, (char*)&val, sizeof(val));
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE, nai_errno,
                "stream endpoint(%s/%d) set keepalive_idle(%d) failed",
                nai_str(&c->ep->host), c->cid, val);
            goto _end;
        }

        val = n->keepalive_cnt;
        r = nai_sock_set_opt(
            nai_stream_get_fd(&c->io),
            IPPROTO_TCP, TCP_KEEPCNT, (char*)&val, sizeof(val));
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE, nai_errno,
                "stream endpoint(%s/%d) set keepalive_cnt(%d) failed",
                nai_str(&c->ep->host), c->cid, val);
            goto _end;
        }

        val = n->keepalive_interval;
        r = nai_sock_set_opt(
            nai_stream_get_fd(&c->io),
            IPPROTO_TCP, TCP_KEEPINTVL, (char*)&val, sizeof(val));
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE, nai_errno,
                "stream endpoint(%s/%d) set keepalive_interval(%d) failed",
                nai_str(&c->ep->host), c->cid, val);
            goto _end;
        }
    };
#endif
#endif

    r = 0;

_end:
    return r;
};


static int nsi_stream_wait_connect(nsi_stream_t* c)
{
    int r;
    nsi_network_t* n;


    /* set timer and try connect again */
    if (nai_stream_is_opened(&c->io)) {
        r = nai_stream_set_timeout(
            &c->io, NAI_TIMEOP_SET, 100 << c->retry);
    } else if (nai_evnode_is_opened(&c->io.ev)) {
        r = nai_evnode_set_timeout(
            &c->io.ev, NAI_TIMEOP_SET, 100 << c->retry);
    } else {
        n = c->ep->net;
        nai_evnode_set_cb(&c->io.ev, (nai_evnode_cb_f)nsi_stream_handle);
        nai_evnode_set_timeout(
            &c->io.ev, NAI_TIMEOP_SET, 100 << c->retry);

        r = nai_evnode_open(&c->io.ev, n->loop);
    };
    if (r < 0) {
        goto _end;
    };

    c->reconnect = 1;
    if (c->retry < 8) {
        c->retry ++;
    };

    r = 0;

_end:
    return r;
};


static int nsi_stream_connect(nsi_stream_t* c)
{
    int r;
    int ec;
    int blocking;
    nsi_network_t* n;
    nsi_endpoint_name_t* dest;


    nai_log_info(NSI_LOG_CORE, 0, 
        "stream endpoint(%s/%d) start %s", 
        nai_str(&c->ep->host), c->cid, 
        c->reconnect ? "reconnect" : "connect");


    /* do bind */
    r = nsi_iobase_bind((nsi_iobase_t*)c, 1);
    if (r < 0) {
        ec = nai_errno;
        nai_log_error(NSI_LOG_CORE, ec, 
            "stream endpoint(%s/%d) bind address failed", 
            nai_str(&c->ep->host), c->cid);

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
    nai_stream_set_mode(&c->io, NAI_IO_READWRITE);
    nai_stream_set_opt(&c->io, NAI_IO_RECVTIMEO, -1);
    nai_stream_set_opt(&c->io, NAI_IO_SENDTIMEO, n->send_timeo);
    if (c->ep->local == 0) {
        nai_stream_set_opt(&c->io, NAI_IO_NODELAY, 1);
    };

    /* do connect */
    blocking = 0;
    dest = &c->ep->name;
    if (c->ep->local && 
        nai_evloop_is_asyncio(n->loop, NAI_FD_TYPE_SOCK)) {
        /* if backend is aio and endpoint is unix socket, 
         * we should connect in blocking mode
         */
        nai_stream_set_blocking(&c->io, 1);
        blocking = 1;
    };

    r = nai_stream_connect(&c->io, n->loop, dest->addr, dest->len);
    if (r >= 0) {
        /* if backend is aio and endpoint is unix socket, 
         * we should connect in blocking mode
         */
        if (blocking) {
            r = nai_stream_set_blocking(&c->io, 0);
            if (r < 0) {
                nai_log_crit(NSI_LOG_CORE, nai_errno, 
                    "stream endpoint(%s/%d) set blocking(0) failed", 
                    nai_str(&c->ep->host), c->cid);

                goto _error;
            };

            r = nai_stream_post(&c->io, NAI_EV_READ);
            if (r < 0) {
                nai_log_crit(NSI_LOG_CORE, nai_errno, 
                    "stream endpoint(%s/%d) post failed", 
                    nai_str(&c->ep->host), c->cid);

                goto _error;
            };
        };

        r = nsi_stream_set_opts(c, n);
        if (r < 0) {
            nai_log_crit(NSI_LOG_CORE, nai_errno, 
                "stream endpoint(%s/%d) set options failed", 
                nai_str(&c->ep->host), c->cid);

            goto _error;
        };

        c->reconnect = 0;

    } else {
        if (c->reconnect) {
            nai_log_crit(NSI_LOG_CORE, nai_errno, 
                "stream endpoint(%s/%d) connect fails directly", 
                nai_str(&c->ep->host), c->cid);

            goto _error;
        };
    };


_end:
    return r;

_error:
    if (c->reconnect) {
        /* set timer and try connect again */
        r = nsi_stream_wait_connect(c);
        if (r < 0) {
            ec = nai_errno;
            nai_log_error(NSI_LOG_CORE, ec, 
                "stream endpoint(%s/%d) failed to start wait connect", 
                nai_str(&c->ep->host), c->cid);

            goto _alert;
        };

        r = 0;
    };
    goto _end;

_alert:
    if (c->reconnect) {
        r = nsi_stream_handle_alert(c, ec);
    };
    goto _end;
};


static int nsi_stream_send(
    nsi_stream_t* c, nsi_message_t* m, uint32_t uid, int flags)
{
    int r;


    if (uid) {
        if (uid == c->uid) {
            r = 0;
            goto _skip;
        };

        c->uid = uid;
    };

    if (flags & NSI_SEND_MSGDUP) {
        m = nsi_message_dup(m, 0);
        if (m == 0) {
            nai_log_alert(NSI_LOG_CORE, nai_errno, 
                "stream endpoint(%s/%d) dupilcate message failed", 
                nai_str(&c->ep->host), c->cid);

            r = -1;
            goto _end;
        };
    };

    c->out.qsize += m->hdr.len;
    nai_list_insert_tail(&c->out.waiting, &m->ent);
    if (c->out.mcount <= 0 && nai_stream_is_writable(&c->io)) {
        do {
            if (flags & NSI_SEND_POSTED) {
                if (c->posted) {
                    break;
                };
                r = nai_stream_post(&c->io, NAI_EV_WRITE);
                if (r >= 0) {
                    c->posted = 1;
                    break;
                };

                nai_log_alert(NSI_LOG_CORE, nai_errno, 
                    "stream endpoint(%s) do post failed, send now", 
                    nai_str(&c->ep->host));
            };

            r = nsi_stream_handle_send(c);

        } while (0);
    };

    (void)r;

    r = 0;

_end:
    return r;

_skip:
    (void)r;

    if (!(flags & NSI_SEND_MSGDUP)) {
        nsi_message_close(m);
    };
    r = 0;
    goto _end;
};


static int nsi_stream_flush(nsi_stream_t* c)
{
    int r;


    if (nai_stream_is_writable(&c->io)) {
        r = nsi_stream_handle_send(c);
    } else {
        r = 0;
    };

    return r;
};

/* drop any non-proxy messages of c->out.waiting */
static void nsi_stream_drop_pending(nsi_stream_t *c)
{
    nsi_message_t* m;
    nai_list_entry_t* current;
    nai_list_entry_t* cursor;

    /* c->out.waiting is the header node */
    cursor = c->out.waiting.next;

    while (cursor != &c->out.waiting) {
        current = cursor;
        cursor = cursor->next;

        m = (nsi_message_t*) current;
        if (nsi_is_proxy_message(m))
            continue;

        nai_list_entry_remove(&m->ent);
        c->out.qsize -= m->hdr.len;
        nsi_message_close(m);
    }
}


static int nsi_stream_close_impl(
    nsi_stream_t* c, nai_list_entry_t* msgs, int reset)
{
    int r;

    nai_log_info(NSI_LOG_CORE, 0,
        "stream endpoint(%s/%d) close with connected(%d), reset(%d)",
        nai_str(&c->ep->host), c->cid, c->connected, reset);

    if (c->connected) {
        c->connected = 0;

        if (reset) {
            /* reset stream requires reservation of address, 
             * we can't call shutdown because it will release binding 
             * address, so disable events only.
             */
            r = nai_stream_set_mode(&c->io, 0);
            if (r < 0) {
                nai_log_error(NSI_LOG_CORE, nai_errno, 
                    "stream endpoint(%s/%d) disable i/o events failed", 
                    nai_str(&c->ep->host), c->cid);
            };
        } else {
            r = nai_stream_shutdown(&c->io, NAI_SOCK_WR);
            if (r < 0) {
                nai_log_error(NSI_LOG_CORE, nai_errno, 
                    "stream endpoint(%s/%d) shutdown failed", 
                    nai_str(&c->ep->host), c->cid);
            };
        };
    };

    if (reset == 0 || c->ep->local) {
        /* in case endpoint is a local stream, 
         * we unneed hold the address, close it directly
         */
        r = nsi_iobase_close((nsi_iobase_t*)c, 1);
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE, nai_errno, 
                "stream endpoint(%s/%d) close failed", 
                nai_str(&c->ep->host), c->cid);
            goto _error;
        };

        if (c->in.buf) {
            nsi_endpoint_release_buf(c->ep, c->in.buf);
            c->in.buf = 0;
        };
    };

    if (reset) {
        /* set timer and waiting to connect */
        r = nsi_stream_wait_connect(c);
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE, nai_errno, 
                "stream endpoint(%s/%d) failed to start wait connect", 
                nai_str(&c->ep->host), c->cid);
            goto _alert;
        };

        /* reset read stat */
        c->read = NSI_STREAM_WAIT;

        /**
         * we don't close socket to hold the address on reset.
         * if it it asyncio, there may be unfinished operations, 
         * it will continue to access the buffer, 
         * will destroy the receive buffer, 
         * keep it to avoid affecting other programs.
         *
         * discard content only */
        if (c->in.buf) {
            c->in.buf->size = 0;
        };
    };

    if (c->in.incoming) {
        nsi_message_close(c->in.incoming);
        c->in.incoming = 0;
    };

    nai_list_add_tail(msgs, &c->out.waiting);
    nai_list_add_tail(msgs, &c->out.queued);
    nai_buflist_close(&c->out.bufs);
    c->out.qsize = 0;
    c->out.mcount = 0;
    c->out.vcount = 0;
    c->out.vnext = 0;
    c->out.zerocopy = 0;
    c->out.zwaiting = 0;
    c->out.zpending = 0;
    c->out.znotified = 0;
    r = 0;

_end:
    return r;

_error:
    if (!reset) {
        goto _end;
    };
    r = nsi_stream_handle_error(c, nai_errno);
    goto _end;

_alert:
    r = nsi_stream_handle_alert(c, nai_errno);
    goto _end;
};


static int nsi_stream_reset(nsi_stream_t* c, nai_list_entry_t* msgs)
{
    int r;

    nsi_stream_tls_close(c, 0);

    r = nsi_stream_close_impl(c, msgs, 1);


    return r;
};


static int nsi_stream_close(nsi_stream_t* c, nai_list_entry_t* msgs)
{
    int r;


    r = nsi_stream_close_impl(c, msgs, 0);
    if (r < 0) {
        goto _end;
    };

    if (c->ep->pexit) {
        c->closing = 1;
        r = 0;
        goto _end;
    };

    nsi_network_free(c->ep->net, c);
    r = 0;


_end:
    return r;
};



//////////////////////////////////////////////////////////////////////////////
// stream server


#define nsi_stream_bind_find_conn   nsi_endpoint_bind_find_conn
#define nsi_stream_bind_find_name   nsi_endpoint_bind_find_name



static int nsi_stream_server_alert(nsi_stream_server_t* s, int errcode)
{
    return nsi_endpoint_handle_except(s->ep, 0, errcode, 1);
};


static int nsi_stream_server_alloc_cid(nsi_stream_server_t* s, nsi_stream_t* c)
{
    int cid;
    nai_rbnode_t** n;
    nai_rbnode_t* parent;

    while (1) {
        cid = ++ s->nextc;
        if (cid <= 0) {
            s->nextc = 0;
            continue;
        };

        n = nsi_stream_find(&s->clients, cid, 0);
        if (n[0]) {
            /* connection id exists */
            continue;
        };

        n = nsi_stream_bind_find_conn(&s->conns, cid, &parent);
        if (n[0]) {
            /* connection id exists */
            continue;
        };

        if (c != 0) {
            c->cid = cid;
        };
        break;
    };

    return cid;
};


static int nsi_stream_server_accept(nai_server_t* l, int events)
{
    int r;
    int ec = 0;
    nai_fd_t fd;
    nsi_stream_server_t* s;
    nsi_stream_bind_t* b;
    nsi_stream_t* c;
    nai_rbnode_t** n;
    nai_rbnode_t* parent;
    nai_list_entry_t msgs;
    nsi_network_t* net;
    nsi_endpoint_name_t name;
    char buf[140];


    s = (nsi_stream_server_t*)l;
    if (!(events & NAI_EV_READ)) {
        if (events & NAI_EV_TIMEOUT) {
            /* nothing to do */
        };

        r = 0;
        goto _end;
    };


    while (1) {
        s->nbuf.len = sizeof(s->nbuf.storage);
        fd = nai_server_accept(&s->io, &s->nbuf.addr, &s->nbuf.len);
        if (fd == NAI_FD_INVALID) {
            ec = nai_errno;
            if (ec != NAI_EAGAIN && ec != NAI_EINPROGRESS) {
                nai_log_crit(NSI_LOG_CORE, ec, 
                    "server endpoint(%s) failed to accepted", 
                    nai_str(&s->ep->host));
            };
            break;
        };


        name.len = s->nbuf.len;
        name.addr = &s->nbuf.addr;
        n = nsi_stream_bind_find_name(&s->names, name.addr, name.len, 0);
        if (n[0] == 0) {
            if (!s->ep->local && !nsi_endpoint_is_subnet(s->ep, &name)) {
                nai_sock_close(fd);

                if (nai_log_is_enabled_debug(&nsi_log_core)) {
                    buf[0] = 0;
                    nai_sockaddr_ntop(name.addr, 
                        name.len, buf, sizeof(buf), 0);
                    nai_log_debug(NSI_LOG_CORE, 0, 
                        "server endpoint(%s) reject connection from %s", 
                        nai_str(&s->ep->host), buf);
                };
                continue;
            };
        };


        c = nsi_stream_create(s->ep, &name);
        if (c == 0) {
            ec = nai_errno;
            nai_sock_close(fd);
            nai_log_alert(NSI_LOG_CORE, ec, 
                "server endpoint(%s) failed to allocate stream", 
                nai_str(&s->ep->host));

            r = nsi_stream_server_alert(s, ec);
            if (r != NAI_DECLINED) {
                break;
            };
            continue;
        };


        net = c->ep->net;
        nai_stream_set_fd(&c->io, fd, NAI_FD_TYPE_SOCK);
        nai_stream_set_fdown(&c->io, 1);
        nai_stream_set_opt(&c->io, NAI_IO_SENDTIMEO, net->send_timeo);
        if (c->ep->local == 0) {
            nai_stream_set_opt(&c->io, NAI_IO_NODELAY, 1);
        };

        /* open stream */
        r = nai_stream_open(&c->io, nai_server_get_loop(&s->io));
        if (r < 0) {
            ec = nai_errno;
            nai_log_alert(NSI_LOG_CORE, ec, 
                "server endpoint(%s) failed to open stream", 
                nai_str(&s->ep->host));
        } else {
            r = nsi_stream_set_opts(c, net);
            if (r < 0) {
                ec = nai_errno;
                nai_log_crit(NSI_LOG_CORE, 
                    ec, "stream endpoint(%s) set options failed", 
                    nai_str(&c->ep->host));
            } else {
                r = nsi_stream_tls_open(c, 0);
            };
        };
        if (r < 0) {
            nai_list_init(&msgs);
            nsi_stream_close(c, &msgs);

            r = nsi_stream_server_alert(s, ec);
            if (r != NAI_DECLINED) {
                break;
            };
            continue;
        };

        /* mark is server */
        c->server = 1;

        /* find exists cid or allocate a new one */
        n = nsi_stream_bind_find_name(&s->names, name.addr, name.len, 0);
        if (n[0] == 0) {
            nsi_stream_server_alloc_cid(s, c);
        } else {
            b = nai_containof(n[0], nsi_stream_bind_t, entn);
            c->cid = b->cid;
        };


        /* insert into the map of clients */
        n = nsi_stream_find(&s->clients, c->cid, &parent);
        if(n[0]) {
            /* in case client side reuse the old ip:port to connecting */
            nsi_stream_handle_except(nai_containof(n[0], nsi_stream_t, ent), ECONNABORTED, 0);
            n = nsi_stream_find(&s->clients, c->cid, &parent);
        }
        assert(n[0] == 0);
        nai_rbtree_link(&s->clients, &c->ent, parent, n);
        nai_rbtree_color(&s->clients, &c->ent);


        /* complete */
        events = nai_stream_get_event(&c->io);
        if (events) {
            /* calls to SSL_do_handshake() from server are unintended */
            if (s->ep->tls) {
                continue;
            }
            r = nsi_stream_handle(&c->io, events);
            if (r != NAI_DECLINED) {
                break;
            };
        };
    };

    (void)r;

    r = 0;

_end:
    return r;
};


static int nsi_stream_server_bind_name(
    nsi_stream_server_t* s, nsi_endpoint_name_t* name, int temp)
{
    int r;
    nai_rbnode_t** n;
    nai_rbnode_t* parent;
    nai_rbnode_t* ent;
    nsi_stream_t* c;
    nsi_stream_bind_t* b;


    if (name->addr == 0 || 
        name->addr->sa_family != s->ep->name.addr->sa_family) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    n = nsi_stream_bind_find_name(&s->names, name->addr, name->len, &parent);
    if (n[0] != 0) {
        b = nai_containof(n[0], nsi_stream_bind_t, entn);
    } else {
        b = (nsi_stream_bind_t*)
            nai_bufpool_xalloc(&s->pool, sizeof(*b) + name->len);
        if (b == 0) {
            nai_log_alert(NSI_LOG_CORE, nai_errno, 
                "server endpoint(%s) allocate bind name failed", 
                nai_str(&s->ep->host));
            r = -1;
            goto _end;
        };

        b->refs = 0;
        b->uid = 0;
        b->mcast = 0;
        b->banned = 0;
        b->name.len = name->len;
        b->name.addr = (nai_sockaddr_t*)(b + 1);
        nai_memcpy(b->name.addr, name->addr, name->len);
        nai_rbtree_link(&s->names, &b->entn, parent, n);
        nai_rbtree_color(&s->names, &b->entn);


        /* find in clients */
        ent = nai_rbtree_begin(&s->clients);
        for ( ; ent != nai_rbtree_end(&s->clients); 
            ent = nai_rbtree_next(ent)) {
            c = nai_containof(ent, nsi_stream_t, ent);
            if (nsi_endpoint_name_equal(name, &c->name)) {
                break;
            };
        };
        if (ent != nai_rbtree_end(&s->clients)) {
            b->cid = c->cid;
        } else {
            b->cid = nsi_stream_server_alloc_cid(s, 0);
        };

        n = nsi_stream_bind_find_conn(&s->conns, b->cid, &parent);
        assert(n[0] == 0);
        nai_rbtree_link(&s->conns, &b->entc, parent, n);
        nai_rbtree_color(&s->conns, &b->entc);
    };

    if (temp == 0) {
        b->refs ++;
    };

    r = b->cid;

_end:
    return r;
};


static int nsi_stream_server_bind_conn(
    nsi_stream_server_t* c, nsi_connid_t cid)
{
    int r;
    nai_rbnode_t** n;
    nai_rbnode_t* parent;
    nsi_stream_bind_t* b;


    n = nsi_stream_bind_find_conn(&c->clients, cid, &parent);
    if (n[0] == 0) {
        nai_errno = ENOENT;
        r = -1;
        goto _end;
    };

    b = nai_containof(n[0], nsi_stream_bind_t, entn);
    r = nsi_stream_server_bind_name(c, &b->name, 0);

_end:
    return r;
};


static int nsi_stream_server_unbind_name(
    nsi_stream_server_t* s, nsi_connid_t cid)
{
    int r;
    nai_rbnode_t** n;
    nai_rbnode_t* parent;
    nsi_stream_bind_t* b;


    n = nsi_stream_bind_find_conn(&s->conns, cid, &parent);
    if (n[0] == 0) {
        nai_errno = ENOENT;
        r = -1;
        goto _end;
    };

    b = nai_containof(n[0], nsi_stream_bind_t, entc);
    b->refs --;
    if (b->refs <= 0) {
        nai_rbtree_erase(&s->conns, &b->entc);
        nai_rbtree_erase(&s->names, &b->entn);
        nai_bufpool_xfree(&s->pool, b, sizeof(*b) + b->name.len);
    };

    r = 0;

_end:
    return r;
};


static int nsi_stream_server_open(nsi_endpoint_t* p)
{
    int r;
    int ec;
    nsi_network_t* n;
    nsi_stream_server_t* s;


    s = (nsi_stream_server_t*)nsi_network_alloc(p->net, sizeof(*s));
    if (s == 0) {
        nai_log_alert(NSI_LOG_CORE, 
            nai_errno, "allocate server endpoint(%s) failed", 
            nai_str(&p->host));
        r = -1;
        goto _end;
    };

    nai_bufpool_init(&s->pool);
    r = nai_bufpool_open(&s->pool, 512, 0);
    if (r < 0) {
        nai_log_alert(NSI_LOG_CORE, nai_errno, 
            "server endpoint(%s) failed to create bufpool", 
            nai_str(&p->host));
        goto _fail;
    };

    s->ep = p;
    s->nextc = 0;
    nai_rbtree_init(&s->clients);
    nai_rbtree_init(&s->conns);
    nai_rbtree_init(&s->names);
    nai_server_init(&s->io);
    nai_server_set_opt(&s->io, NAI_IO_REUSEADDR, 1);
    nai_server_set_cb(&s->io, nsi_stream_server_accept);

    n = p->net;
    r = nai_server_bind(&s->io, n->loop, p->name.addr, p->name.len);
    if (r < 0) {
        nai_log_alert(NSI_LOG_CORE, nai_errno, 
            "server endpoint(%s) bind socket address failed", 
            nai_str(&p->host));
        goto _fail;
    };

    p->connected = 1;
    p->ctx = s;

    r = nsi_stream_tls_open(s, 1);
    if (r < 0) {
        goto _fail;
    };

    r = 0;

_end:
    return r;

_fail:
    ec = nai_errno;
    nai_bufpool_close(&s->pool);
    nsi_network_free(p->net, s);
    nai_errno = ec;
    r = -1;
    goto _end;
};


static int nsi_stream_server_map_conn(
    nsi_endpoint_t* p, nsi_endpoint_t* s, nsi_connid_t cid)
{
    int r;

    (void)p;
    (void)s;
    (void)cid;

    nai_errno = ENOTSUP;
    r = -1;

    return r;
};


static int nsi_stream_server_get_name(
    nsi_endpoint_t* p, nsi_connid_t cid, nsi_endpoint_name_t* name)
{
    int r;
    nai_rbnode_t** n;
    nsi_stream_t* c;
    nsi_stream_server_t* s;
    nsi_stream_bind_t* b;
    nsi_endpoint_name_t addr;


    s = (nsi_stream_server_t*)p->ctx;
    if (cid == 0) {
        addr = p->name;
    } else {
        n = nsi_stream_find(&s->clients, cid, 0);
        if (n[0] != 0) {
            c = nai_containof(n[0], nsi_stream_t, ent);
            addr = c->name;
        } else {
            n = nsi_stream_bind_find_conn(&s->conns, cid, 0);
            if (n[0] == 0) {
                nai_errno = ENOENT;
                r = -1;
                goto _end;
            };

            b = nai_containof(n[0], nsi_stream_bind_t, entn);
            addr = b->name;
        };
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


static int nsi_stream_server_get_opt(
    nsi_endpoint_t* p, int opt, intptr_t* value)
{
    int r;
    nsi_stream_server_t* s;


    s = (nsi_stream_server_t*)p->ctx;
    r = nai_stream_get_opt(&s->io, opt, value);

    return r;
};


static int nsi_stream_server_set_opt(
    nsi_endpoint_t* p, int opt, intptr_t value)
{
    int r;
    nai_rbnode_t** n;
    nai_rbnode_t* e;
    nsi_stream_t* c;
    nsi_stream_server_t* s;


    s = (nsi_stream_server_t*)p->ctx;
    switch (opt) {
    case NSI_EOPT_CHECK_CSTAT:
        if (value == 0) {
            r = 1;
        } else {
            n = nsi_stream_find(&s->clients, (nsi_connid_t)value, 0);
            r = n[0] != 0;
        };
        break;
    case NSI_EOPT_QUERY_UCODE:
        n = nsi_stream_find(&s->clients, (nsi_connid_t)value, 0);
        if (n[0] == 0) {
            nai_errno = ENOENT;
            r = -1;
            break;
        };

        c = nai_containof(n[0], nsi_stream_t, ent);
        r = c->uid;
        break;
    case NSI_EOPT_RESET_UCODE:
        e = nai_rbtree_begin(&s->clients);
        for ( ; e != nai_rbtree_end(&s->clients); ) {
            c = (nsi_stream_t*)e;
            e = nai_rbtree_next(e);
            c->uid = 0;
        };
        r = 0;
        break;
    case NSI_EOPT_BIND_NAME:
        r = nsi_stream_server_bind_name(s, (nsi_endpoint_name_t*)value, 0);
        break;
    case NSI_EOPT_BIND_TEMP:
        r = nsi_stream_server_bind_name(s, (nsi_endpoint_name_t*)value, 1);
        break;
    case NSI_EOPT_BIND_MCAST:
        nai_errno = ENOTSUP;
        r = -1;
        break;
    case NSI_EOPT_BIND_CONN:
        r = nsi_stream_server_bind_conn(s, (nsi_connid_t)value);
        break;
    case NSI_EOPT_UNBIND_NAME:
        r = nsi_stream_server_unbind_name(s, (nsi_connid_t)value);
        break;
    default:
        r = nai_stream_set_opt(&s->io, opt, value);
        break;
    };

    return r;
};


static int nsi_stream_server_send(
    nsi_endpoint_t* p, nsi_message_t* m, uint32_t uid, int flags)
{
    int r;
    nai_rbnode_t** n;
    nsi_stream_t* c;
    nsi_stream_server_t* s;
    nsi_network_t* t;


    s = (nsi_stream_server_t*)p->ctx;
    n = nsi_stream_find(&s->clients, m->conn, 0);
    if (n[0] == 0) {
        nai_errno = ENOENT;
        r = -1;
        goto _end;
    };

    t = p->net;
    c = nai_containof(n[0], nsi_stream_t, ent);
    if (c->out.qsize >= (p->local ? t->queue_mix_limit : t->queue_limit)) {
        nsi_stream_drop_pending(c);
        if (!nsi_is_proxy_message(m)) {
            nai_errno = ENOSPC;
            r = -1;
            goto _end;
        };
    };

    r = nsi_stream_send(c, m, uid, flags);

_end:
    return r;
};


static int nsi_stream_server_flush(nsi_endpoint_t* p)
{
    int r;


    (void)p;

    r = 0;

    return r;
};


static int nsi_stream_server_close(nsi_endpoint_t* p)
{
    int r;
    size_t qsize;
    nai_rbnode_t* e;
    nsi_stream_t* c;
    nsi_stream_server_t* s;
    nai_list_entry_t msgs;


    s = (nsi_stream_server_t*)p->ctx;
    if (s == 0) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    nsi_stream_tls_close(s, 1);

    /* close listening */
    r = nai_server_close(&s->io);
    if (r < 0) {
        nai_log_alert(NSI_LOG_CORE, nai_errno, 
            "server endpoint(%s) close socket failed", 
            nai_str(&p->host));
        goto _end;
    };

    /* close all stream */
    qsize = 0;
    nai_list_init(&msgs);
    e = nai_rbtree_begin(&s->clients);
    for ( ; e != nai_rbtree_end(&s->clients); ) {
        c = nai_containof(e, nsi_stream_t, ent);
        e = nai_rbtree_next(e);
        qsize += c->out.qsize;
        nai_rbtree_erase(&s->clients, &c->ent);
        nsi_stream_tls_close(c, 0);
        nsi_stream_close(c, &msgs);
    };

    /* callback messages */
    nsi_endpoint_handle_sent(p, &msgs, &qsize, ECANCELED);


    /* free */
    nai_bufpool_close(&s->pool);
    nsi_network_free(p->net, s);

    /* unlink */
    p->connected = 0;
    p->ctx = 0;
    r = 0;


_end:
    return r;
};


//////////////////////////////////////////////////////////////////////////////
// stream client


static int nsi_stream_client_open(nsi_endpoint_t* p)
{
    int r;
    int ec;
    nsi_stream_t* c;
    nsi_endpoint_name_t name;
    nai_socknbuf_in_t nbuf;
    nai_list_entry_t msgs;


    if (p->local == 0) {

        nbuf.len = sizeof(nbuf.storage);
        r = nsi_network_map_route(p->net, 
            p->name.addr, p->name.len, &nbuf.addr, &nbuf.len, 0);
        if (r < 0) {
            ec = nai_errno;
            nai_log_error(NSI_LOG_CORE, ec, 
                "failed to get stream endpoint(%s/%d) bind address", 
                nai_str(&p->host), 0);

            goto _end;
        };

        name.len = nbuf.len;
        name.addr = &nbuf.addr;
    };

    c = nsi_stream_create(p, p->local ? 0 : &name);
    if (c == 0) {
        r = -1;
        goto _end;
    };

    if (p->waiting) {
        c->reconnect = 1;
    };

    r = nsi_stream_connect(c);
    if ( r >= 0) {
        r = nsi_stream_tls_open(c, 0);
    }

    if (r < 0) {
        ec = nai_errno;
        nai_list_init(&msgs);
        nsi_stream_close(c, &msgs);
        nai_errno = ec;
        goto _end;
    };

    p->connected = 0;
    p->ctx = c;
    r = 0;

_end:
    return r;
};


static int nsi_stream_client_map_conn(
    nsi_endpoint_t* p, nsi_endpoint_t* s, nsi_connid_t cid)
{
    int r;

    (void)p;
    (void)s;
    (void)cid;

    nai_errno = ENOTSUP;
    r = -1;

    return r;
};


static int nsi_stream_client_get_name(
    nsi_endpoint_t* p, nsi_connid_t cid, nsi_endpoint_name_t* name)
{
    int r;
    nsi_stream_t* c;
    nsi_endpoint_name_t addr;


    if (cid) {
        nai_errno = ENOENT;
        r = -1;
        goto _end;
    };

    c = (nsi_stream_t*)p->ctx;
    if (c->connected == 0) {
        nai_errno = ENOTCONN;
        r = -1;
        goto _end;
    };

    addr = c->name;
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


static int nsi_stream_client_get_opt(
    nsi_endpoint_t* p, int opt, intptr_t* value)
{
    int r;
    nsi_stream_t* c;


    c = (nsi_stream_t*)p->ctx;
    r = nai_stream_get_opt(&c->io, opt, value);

    return r;
};


static int nsi_stream_client_set_opt(
    nsi_endpoint_t* p, int opt, intptr_t value)
{
    int r;
    nsi_stream_t* c;


    c = (nsi_stream_t*)p->ctx;
    switch (opt) {
    case NSI_EOPT_CHECK_CSTAT:
        if (value == 0) {
            r = c->connected;
        } else {
            nai_errno = ENOENT;
            r = -1;
        };
        break;
    case NSI_EOPT_QUERY_UCODE:
        if (value != 0) {
            nai_errno = EINVAL;
            r = -1;
            break;
        };

        r = c->uid;
        break;
    case NSI_EOPT_RESET_UCODE:
        c->uid = 0;
        r = 0;
        break;
    default:
        r = nai_stream_set_opt(&c->io, opt, value);
        break;
    };

    return r;
};


static int nsi_stream_client_send(
    nsi_endpoint_t* p, nsi_message_t* m, uint32_t uid, int flags)
{
    int r;
    nsi_stream_t* c;
    nsi_network_t* n;


    n = p->net;
    c = (nsi_stream_t*)p->ctx;
    if (c->out.qsize >= (p->local ? n->queue_mix_limit : n->queue_limit)) {
        nsi_stream_drop_pending(c);
        if (!nsi_is_proxy_message(m)) {
            nai_errno = ENOSPC;
            r = -1;
            goto _end;
        };
    };

    r = nsi_stream_send(c, m, uid, flags);

_end:
    return r;
};


static int nsi_stream_client_flush(nsi_endpoint_t* p)
{
    int r;
    nsi_stream_t* c;


    c = (nsi_stream_t*)p->ctx;
    r = nsi_stream_flush(c);

    return r;
};


static int nsi_stream_client_close(nsi_endpoint_t* p)
{
    int r;
    size_t qsize;
    nsi_stream_t* c;
    nai_list_entry_t msgs;


    c = (nsi_stream_t*)p->ctx;
    if (c == 0) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    nsi_stream_tls_close(c, 0);

    /* close stream */
    qsize = c->out.qsize;
    nai_list_init(&msgs);
    r = nsi_stream_close(c, &msgs);
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



nsi_endpoint_ops_t nsi_stream_server_ops = {
    nsi_stream_server_open, 
    nsi_stream_server_map_conn, 
    nsi_stream_server_get_name, 
    nsi_stream_server_get_opt, 
    nsi_stream_server_set_opt, 
    nsi_stream_server_send, 
    nsi_stream_server_flush, 
    nsi_stream_server_close
};

nsi_endpoint_ops_t nsi_stream_client_ops = {
    nsi_stream_client_open, 
    nsi_stream_client_map_conn, 
    nsi_stream_client_get_name, 
    nsi_stream_client_get_opt, 
    nsi_stream_client_set_opt, 
    nsi_stream_client_send, 
    nsi_stream_client_flush, 
    nsi_stream_client_close
};



#if defined(NAI_HAVE_SSL)
static nai_ssl_ops_t nai_stream_tls_ops = {
    0, /* lookup cert */
    0, /* lookup server */
    0, /* cookie verify */
    0, /* cookie generate */
    nai_ssl_iobase_poll,
    nai_ssl_iobase_want,
    nai_ssl_iobase_emit,
    nai_ssl_iobase_get_loop,
    nai_ssl_iobase_get_opt,
    nai_ssl_iobase_get_peer,
    0, /* realloc */
    (nai_ssl_read_f)nai_stream_read,
    (nai_ssl_write_f)nai_stream_write,
    (nai_ssl_recvm_f)nai_stream_recvm,
    (nai_ssl_sendm_f)nai_stream_sendm,
    (nai_ssl_sendfile_f)nai_stream_sendfile,
    nai_ssl_iobase_get_socket,
};

static int nsi_stream_tls_open(void *nsi_stream, int listener)
{
    nsi_stream_server_t *server;
    nsi_stream_t *stream;
    nsi_endpoint_t* p;
    nai_ssl_t *ssl;
    nai_stream_t *io;
    int ssl_initialized;

    int r = 0;

    if (listener) {
        server = (nsi_stream_server_t *) nsi_stream;
        p = server->ep;
        ssl = &server->ssl;
        io = &server->io;
        ssl_initialized = 0;    /* newly socket shouldn't have been setup */
    } else {
        stream = (nsi_stream_t *)nsi_stream;
        p = stream->ep;
        ssl = &stream->ssl;
        io = &stream->io;
        ssl_initialized = stream->ssl_initalized;
    }

    if (!p->tls) {
        return 0;
    }
    if (ssl_initialized) {
        return 0;
    }

    memset((void *)ssl, 0, sizeof(*ssl));
    nai_ssl_init(ssl);
    nai_ssl_set_ops(ssl, &nai_stream_tls_ops, io);

    if (p->server) {
        r = nai_ssl_open(ssl, &(p->ssl_ctx), 0);
    } else {
        r = nai_ssl_open(ssl, &(p->ssl_ctx), 1);
    }
    if (r < 0) {
        nai_log_alert(NSI_LOG_CORE, nai_errno,
            "endpoint(%s) SSL setup failed",
            nai_str(&p->host));
    } else {
        if (!listener) {
            stream->ssl_initalized = 1;
        }
    }

    return r;
}

static int nsi_stream_tls_handshake(nsi_stream_t* c)
{
    int r;

    r = nsi_stream_tls_open(c, 0);
    if (r < 0) {
        return r;
    };

    r = nai_ssl_handshake(&c->ssl);
    if (r < 0) {
        if (nai_errno == EAGAIN || nai_errno == EINPROGRESS) {
            r = NAI_DECLINED;
        };
    } else {
        if (c->ep->server) {
            r = nai_ssl_verify(&c->ssl, NAI_SSL_VERIFY_NO_CA);
        };
        if (r >= 0) {
            c->ssl_handshaked = 1;
        };
    }
    nai_log_info(NSI_LOG_CORE, 0, "%s endpoint %s ssl handshake with connid %d, rc: %d",
        (c->ep->server)? "server": "client", nai_str(&c->ep->host), c->cid, r);

    return r;
}

static void nsi_stream_tls_close(void *nsi_stream, int listener)
{
    int r;
    nai_ssl_t* s = NULL;
    nsi_stream_t *stream;
    nsi_stream_server_t* server;

    if (listener) {
        server = (nsi_stream_server_t *) nsi_stream;
        if (server->ep->tls) {
            s = &server->ssl;
        }
    } else {
        stream = (nsi_stream_t *) nsi_stream;
        if (stream->ssl_initalized) {
            s = &stream->ssl;
            stream->ssl_initalized = 0;
            stream->ssl_handshaked = 0;
        }
    }

    if (s) {
        r = nai_ssl_shutdown(s, NAI_SOCK_RW);
        if (r < 0) {
            nai_log_warn(NSI_LOG_CORE, 0, "ssl shutdown failed, rc:%d", r);
        }
        r = nai_ssl_close(s);
        if (r < 0) {
            nai_log_warn(NSI_LOG_CORE, 0, "ssl close failed, rc:%d", r);
        }
    }
}
#else
static int nsi_stream_tls_open(void *nsi_stream, int listener) { (void)nsi_stream; (void) listener; return 0; }
static int nsi_stream_tls_handshake(nsi_stream_t* c) { (void)c; return 0; }
static void nsi_stream_tls_close(void *nsi_stream, int listener) { (void)nsi_stream; (void)listener; return; }
#endif
