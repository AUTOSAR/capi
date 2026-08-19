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
/// @file       nsi_endpoint_npc.c
/// @brief
/// @details
/// @date       2021-05-11
/// @author     xn
/// @version    1.2.0
///
/// ================================================================

#include "nsomeip/core/nsi_log.h"
#include "nsomeip/core/nsi_proxy.h"
#include "nsomeip/core/nsi_routing.h"
#include "nai/os/nai_shmem.h"
#include "nsi_network.h"
#include "nsomeip/npc/npc_port.h"
#include "nsomeip/npc/npc_comm.h"
#include "nsomeip/npc/npc_message.h"


#define NPC_MSG_SYN_TYPE    0x01
#define NPC_MSG_ACK_TYPE    0x02
#define NPC_MSG_SND_TYPE    0x03

extern nsi_endpoint_ops_t nsi_npc_client_ops;
extern nsi_endpoint_ops_t nsi_npc_server_ops;

typedef struct nsi_endpoint_bind_s nsi_npc_bind_t;

typedef struct npc_msg_sync_s {
    char        shmpath[60];
    uint16_t    serv;
    uint16_t    inst;
    uint32_t    segcount;
    uint32_t    segsize;
    uint32_t    peer;
    npc_mref_t  portread;
    npc_mref_t  portwrite;
} npc_msg_sync_t;

typedef struct nsi_npc_s {
    /* header some as nsi_iobase_t */
    nai_stream_t io;

    /* bind name */
    nai_rbnode_t node;
    nsi_endpoint_name_t name;

    /* endpoint */
    nsi_endpoint_t* ep;

    /* shm struct */
    npc_comif_t* comif;
    npc_port_t* portread;
    npc_port_t* portwrite;

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
            uint32_t posted:1;
            uint32_t reconnect:1;
            uint32_t npcsyn:1;
            uint32_t npcack:1;
            uint32_t retry:8;
        };
        uint32_t flags;
    };

    /* for read */
    struct {
        char readbuffer[256];       /**< read buffer */
    } in;


    /* for send */
    struct {
        char npcsynmsg[256];        /**< syn msg */
        char npcackmsg[1];          /**< ack msg */
        char npcnotifymsg[8];       /**< notify msg */

        nai_list_entry_t queued;    /**< the list of sending messages */
        nai_list_entry_t waiting;   /**< the list of waiting messages */
        size_t qsize;               /**< the size of queued in bytes */
        int16_t mcount;             /**< the count of sending messages */

        uint8_t zerocopy;
        uint8_t znotified;
        uint8_t zwaiting;
        uint8_t zpending;
    } out;

} nsi_npc_t;


typedef struct nsi_npc_server_s {
    nai_server_t io;                /**< listen socket */
    nai_bufpool_t pool;             /**< allocate */
    nai_rbtree_t clients;           /**< all clients */
    nai_rbtree_t conns;             /**< map of bind connection id */
    nai_rbtree_t names;             /**< map of bind address */
    nsi_endpoint_t* ep;             /**< pointer to the endpoint */
    uint32_t nextc;                 /**< next connection id */
    nai_socknbuf_t nbuf;            /**< accept address */
    npc_comif_t* comif;             /**< shm interface */
} nsi_npc_server_t;


static nai_rbnode_t** nsi_npc_find(
    nai_rbtree_t* t, uint32_t cid, nai_rbnode_t** pparent);
static int nsi_npc_handle_error(nsi_npc_t* c, int errcode);
static int nsi_npc_handle_incoming(nsi_npc_t* c, nsi_message_t* m);

//////////////////////////////////////////////////////////////////////////////

static int npc_comif_ops_server_post(void *ctx, uint16_t peer, uint32_t event);
static int npc_comif_ops_test(void *ctx, uint16_t eid, uint16_t uid);
static int npc_comif_ops_server_read(void *ctx, npc_port_t *p);
static int npc_comif_ops_server_quit(void *ctx, npc_port_t *p);
static int npc_comif_ops_client_post(void *ctx, uint16_t peer, uint32_t event);
static int npc_comif_ops_client_read(void *ctx, npc_port_t *p);
static int npc_comif_ops_client_quit(void *ctx, npc_port_t *p);
static int nsi_npc_send_buffer(nsi_npc_t* c, void* buffer, int len);

static npc_comif_ops_t comif_server_ops = {
    "npc_comif",
    npc_comif_ops_server_post,
    npc_comif_ops_test,
    npc_comif_ops_server_read,
    npc_comif_ops_server_quit,
};

static npc_comif_ops_t comif_client_ops = {
    "npc_comif",
    npc_comif_ops_client_post,
    npc_comif_ops_test,
    npc_comif_ops_client_read,
    npc_comif_ops_client_quit,
};

int npc_comif_ops_server_post(void *ctx, uint16_t peer, uint32_t event)
{
    int r;
    nsi_npc_t* c;
    nsi_npc_server_t* s;
    nai_rbnode_t** n;

    s = (nsi_npc_server_t*)ctx;
    n = nsi_npc_find(&s->clients, peer, 0);
    if (n[0]) {
        c = nai_containof(n[0], nsi_npc_t, ent);
    } else {
        r = -1;
        goto _out;
    };

    nai_memcpy(&c->out.npcnotifymsg[4], &event, 4);
    r = nsi_npc_send_buffer(c, c->out.npcnotifymsg, 8);
    if (r >= 0) {
    } else {
    };

_out:
    return r;
};

int npc_comif_ops_test(void *ctx, uint16_t eid, uint16_t uid)
{
    (void)ctx;
    (void)eid;
    (void)uid;
    return 0;
};

int npc_comif_ops_server_read(void *ctx, npc_port_t *p)
{
    int r;
    npc_message_t *m;
    nsi_npc_t* c;
    nsi_npc_server_t* s;
    nai_rbnode_t** n;
    s = (nsi_npc_server_t*)ctx;
    r = 0;

    while (1) {
        m = npc_port_read(p);
        if (m == 0) {
            if (nai_errno != EAGAIN) {
                r = -1;
            };
            goto _out;
        };

        n = nsi_npc_find(&s->clients, p->owner, 0);
        if (n[0]) {
            c = nai_containof(n[0], nsi_npc_t, ent);
        } else {
            r = -1;
            goto _out;
        };

        m->conn = c->cid;
        r = nsi_npc_handle_incoming(c, m);
        if (r != NAI_DECLINED) {
            goto _out;
        };
    };

_out:
    return r;
};

int npc_comif_ops_server_quit(void *ctx, npc_port_t *p)
{
    int r;
    (void)ctx;
    (void)p;
    r = 0;
    return r;
};

int npc_comif_ops_client_post(void *ctx, uint16_t peer, uint32_t event)
{
    int r;
    nsi_npc_t* c;

    (void)peer;

    c = (nsi_npc_t*)ctx;
    r = 0;
    nai_memcpy(&c->out.npcnotifymsg[4], &event, 4);

    r = nsi_npc_send_buffer(c, c->out.npcnotifymsg, 8);
    if (r >= 0) {
    } else {
    };
    return r;
};

int npc_comif_ops_client_read(void *ctx, npc_port_t *p)
{
    int r;
    npc_message_t *m;
    nsi_npc_t* s;
    s = (nsi_npc_t*)ctx;
    r = 0;

    while (1) {
        m = npc_port_read(p);
        if (m == 0) {
            if (nai_errno != EAGAIN) {
                r = -1;
            };
            goto out;
        };

        r = nsi_npc_handle_incoming(s, m);
        if (r != NAI_DECLINED) {
            goto out;
        };
    };

out:
    return r;
};

int npc_comif_ops_client_quit(void *ctx, npc_port_t *p)
{
    int r;
    (void)ctx;
    (void)p;
    r = 0;
    return r;
};

//////////////////////////////////////////////////////////////////////////////
// npc


/* npc read stats */
#define nsi_npc_WAIT             0
#define nsi_npc_HEAD             1
#define nsi_npc_PAYLOAD          2


static int nsi_npc_connect(nsi_npc_t* c);
static int nsi_npc_reset(nsi_npc_t* c, nai_list_entry_t* msgs);
static int nsi_npc_close(nsi_npc_t* c, nai_list_entry_t* msgs);


nai_rbnode_t** nsi_npc_find(
    nai_rbtree_t* t, uint32_t cid, nai_rbnode_t** pparent)
{
    nai_rbnode_t** n = &nai_rbtree_root(t);
    nai_rbnode_t* parent = nai_rbtree_end(t);
    nsi_npc_t* e;


    while (*n) {
        parent = *n;
        e = nai_containof(parent, nsi_npc_t, ent);
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


int nsi_npc_send_buffer(nsi_npc_t* c, void* buffer, int len)
{
    int r;
    int n;
    int need;
    need = len;
    r = 0;

_repeat:
    n = nai_stream_send(&c->io, (char*)buffer + len - need, need, 0);
    if (n >= 0) {
        if (n == need) {
            goto _end;
        } else {
            need = len - n;
            goto _repeat;
        }; 
    } else {
        if (nai_errno == NAI_EAGAIN || nai_errno == NAI_EINPROGRESS) {
            r = NAI_DECLINED;
            nai_iobase_set_mode(&c->io, NAI_IO_WRITE);
        } else {
            r = n;
            goto _end;
        };
    };
_end:
    return r;
};


static int nsi_npc_read_buffer(nsi_npc_t* c, void* buffer, int len)
{
    int r;
    int n;
    r = 0;
    n = nai_iobase_read(&c->io, buffer, len);
    if (n >= 0) {
        r = n;
        goto _end; 
    } else {
        if (nai_errno == NAI_EAGAIN || nai_errno == NAI_EINPROGRESS) {
            r = 0;
            nai_iobase_set_mode(&c->io, NAI_IO_READ);
        } else {
            r = n;
            goto _end;
        };
    };
_end:
    return r;
};


static int nsi_npc_send_npcsync(nsi_npc_t* c)
{
    return nsi_npc_send_buffer(c, &c->out.npcsynmsg, sizeof(npc_msg_sync_t) + 1);
};


static int nsi_npc_send_npcack(nsi_npc_t* c)
{
    return nsi_npc_send_buffer(c, &c->out.npcackmsg, sizeof(c->out.npcackmsg));
};


static int nsi_npc_handle_npcsyn(nsi_npc_t* c)
{
    int r;
    npc_msg_sync_t* syn;
    npc_shmz_attr_t shm_attr;

    r = nsi_npc_read_buffer(c, c->in.readbuffer, 256);
    if (r > 0) {
        if (c->in.readbuffer[0] == NPC_MSG_SYN_TYPE) {
            if ((r - 1) == sizeof(npc_msg_sync_t)) {
                syn = (npc_msg_sync_t*)(&c->in.readbuffer[1]);
                shm_attr.serv = syn->serv;
                shm_attr.inst = syn->inst;
                shm_attr.self = syn->peer;
                shm_attr.seg_size = syn->segsize;
                shm_attr.seg_count = syn->segcount;
                c->comif = npc_comif_create(c->ep->net->npc_comm,
                    &comif_client_ops, c, syn->shmpath, &shm_attr, NAI_O_RDWR);
                if (c->comif == 0) {
                    nai_log_alert(NSI_LOG_CORE, nai_errno, 
                        "npc client endpoint create comif failed");
                    goto _fail;
                };
                c->portread = npc_port_attach(c->comif, syn->portwrite, NPC_PORT_READ);
                if (c->portread == 0) {
                    goto _fail;
                };
                c->portwrite = npc_port_attach(c->comif, syn->portread, NPC_PORT_SEND);
                if (c->portwrite == 0) {
                    goto _fail;
                };
                r = nsi_npc_send_npcack(c);
                if (r < 0) {
                    goto _fail;
                } else {
                };
            };
        };
    } else if (r == 0) {
        r = NAI_DECLINED;
    };
_end:
    return r;
_fail:
    r = nsi_npc_handle_error(c, ECONNABORTED);
    goto _end;
};


static int nsi_npc_handle_npcack(nsi_npc_t* c)
{
    int r;
    r = nsi_npc_read_buffer(c, c->in.readbuffer, 256);
    if (r > 0) {
        if (c->in.readbuffer[0] == NPC_MSG_ACK_TYPE) {
            r = 0;
        };
    } else if (r == 0) {
        r = NAI_DECLINED;
    };
    return r;
};


static int nsi_npc_handle_connect(nsi_npc_t* c)
{
    int r;
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
#else
#warn "the platform can not get peer pid."
#endif

    };

    r = nsi_endpoint_handle_connect(c->ep, c->cid);

    return r;
};


int nsi_npc_handle_incoming(nsi_npc_t* c, nsi_message_t* m)
{
    return nsi_endpoint_handle_incoming(c->ep, m);
};


static int nsi_npc_handle_except(nsi_npc_t* c, int errcode, int alert)
{
    int r;
    int n;
    int ec;
    size_t qsize;
    nsi_endpoint_t* e;
    nsi_npc_server_t* s;
    nai_list_entry_t msgs;


    e = c->ep;
    r = nsi_endpoint_handle_except(e, c->cid, errcode, alert);
    if (r != NAI_DECLINED) {
        goto _end;
    };

    if (!alert) {
        qsize = c->out.qsize;
        nai_list_init(&msgs);

        /* close npc */
        if (c->server) {
            s = (nsi_npc_server_t*)e->ctx;
            nai_rbtree_erase(&s->clients, &c->ent);
            r = nsi_npc_close(c, &msgs);
        } else {
            /* do reconnect */
            r = nsi_npc_reset(c, &msgs);
        };
        if (r < 0) {
            nai_log_alert(NSI_LOG_CORE, nai_errno, 
                "nsi_npc_%s failed", c->server ? "close" : "reset");
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


static int nsi_npc_handle_alert(nsi_npc_t* c, int errcode)
{
    return nsi_npc_handle_except(c, errcode, 1);
};


int nsi_npc_handle_error(nsi_npc_t* c, int errcode)
{
    return nsi_npc_handle_except(c, errcode, 0);
};


static int nsi_npc_handle_read(nsi_npc_t* c)
{
    int ec;
    int r;
    int n;
    char cmd[8];
    int cmdpos,bufpos;
    uint32_t event;

    cmdpos = 0;

    while (1) {
        n = nai_iobase_read(&c->io, c->in.readbuffer, 256);
        if (n < 0) {
            ec = nai_errno;
            if (ec == NAI_EAGAIN || ec == NAI_EINPROGRESS) {
                r = NAI_DECLINED;
                goto _end;
            };
            nai_log_crit(NSI_LOG_CORE, 
                ec, "npc endpoint(%s/%d) receive failed", 
                nai_str(&c->ep->host), c->cid);
            goto _error;
        };
        if (n == 0) {
            nai_log_debug(NSI_LOG_CORE, 0, 
                "npc endpoint read return 0");
            r = nsi_npc_handle_error(c, ECONNABORTED);
            goto _end;
        };
      
        bufpos = 0;
        while (1) {
            if (n + cmdpos >= 8) {
                nai_memcpy(&cmd[cmdpos], &c->in.readbuffer[bufpos], 8 - cmdpos);
                bufpos = bufpos + (8 - cmdpos);
                n = n - (8 - cmdpos);
                cmdpos = 0;
                if (cmd[0] == NPC_MSG_SND_TYPE) {
                    event = *(uint32_t*)&cmd[4];

                    r = npc_comif_event(c->comif, event);
                    if (r < 0) {
                        ec = nai_errno;
                        r = nsi_npc_handle_alert(c, ec);
                        goto _end;
                    };
                } else {
                    nai_log_debug(NSI_LOG_CORE, 0, 
                        "unknown msg type");
                    r = nsi_npc_handle_alert(c, EINVAL);
                    goto _end;
                };
                if (n == 0) {
                    break;
                };
            } else {
                nai_memcpy(&cmd[cmdpos], &c->in.readbuffer[bufpos], n);
                cmdpos = cmdpos + n;
                break;
            };
        };
    };

    r = NAI_DECLINED;
    
_end:
    return (int)r;

_error:
    r = nsi_npc_handle_error(c, ec);
    goto _end;
};


#if defined(SO_ZEROCOPY)


#include <linux/errqueue.h>


static int nsi_npc_handle_zc_stat(nsi_npc_t* c)
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


static int nsi_npc_handle_send(nsi_npc_t* c)
{
    int ec;
    int r;
    nsi_message_t* m;
    
    while (1) {
        while (c->out.mcount > 0) {
            m = (nsi_message_t*)(c->out.queued.next);
            nai_list_entry_remove(&m->ent);
            c->out.qsize -= m->hdr.len;
            r = npc_port_send(c->portwrite, m);
            if (r < 0) {
                ec = nai_errno;
                nsi_npc_message_close(m);
                goto _error;
            };
            c->out.mcount --;
        };

        c->out.mcount = 0;

        while (c->out.mcount < 8 && 
            !nai_list_is_empty(&c->out.waiting)) {
            m = (nsi_message_t*)c->out.waiting.next;
            /* add message to output queue */
            nai_list_entry_remove(&m->ent);
            nai_list_insert_tail(&c->out.queued, &m->ent);
            c->out.mcount ++;
        };

        if (c->out.mcount <= 0) {
            break;
        };
    };

    r = NAI_DECLINED;

_end:
    return (int)r;

_error:
    r = nsi_npc_handle_error(c, ec);
    goto _end;
};


static int nsi_npc_handle(nai_stream_t* p, int events)
{
    int r;
    int ec;
    nsi_npc_t* c;

    c = (nsi_npc_t*)p;

#if defined(SO_ZEROCOPY)
    if (events & NAI_EV_ERROR) {
        ec = nai_ev_error_code(events);
        if (ec == 0) {
            c->out.znotified = 1;
            r = nsi_npc_handle_zc_stat(c);
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
            r = nsi_npc_handle_error(c, ETIMEDOUT);
            goto _end;
        };

        if (c->npcack == 0) {
            if (c->server == 1) {
                if (c->npcsyn == 0) {
                    nai_log_info(NSI_LOG_CORE, 
                        0, "server send syn");
                    r = nsi_npc_send_npcsync(c);
                    if(r == 0) {
                        nai_log_info(NSI_LOG_CORE, 
                            0, "set npcsyn to 1 for conn %d", c->cid);
                        c->npcsyn = 1;
                    } else {
                        if (r != NAI_DECLINED) {
                            goto _end;
                        };
                    };
                } else if (c->npcack == 0) {
                    if (events & NAI_EV_READ) {
                        nai_log_info(NSI_LOG_CORE, 
                            0, "server handle ack");
                        r = nsi_npc_handle_npcack(c);
                        if (r == 0) {
                            nai_log_info(NSI_LOG_CORE, 
                                0, "set npcack to 1 for conn %d", c->cid);
                            c->npcack = 1;
                        } else {
                            if (r != NAI_DECLINED) {
                                goto _end;
                            };
                        };
                    };
                };
            } else {
                if (events & NAI_EV_READ) {
                    r = nsi_npc_handle_npcsyn(c);
                    if (r == 0) {
                        nai_log_info(NSI_LOG_CORE, 
                            0, "set npcack to 1 for conn %d", c->cid);
                        c->npcack = 1;
                    } else {
                        if (r != NAI_DECLINED) {
                            goto _end;
                        };
                    };
                };
            };
            if (c->npcack != 1) {
                r = NAI_DECLINED;
                goto _end;
            };
        };

        if (c->connected == 0) {
            nai_log_info(NSI_LOG_CORE, 
                0, "Unconnected becomes to CONNECTED");
            c->connected = 1;
            c->retry = 0;
            r = nsi_npc_handle_connect(c);
            if (r != NAI_DECLINED) {
                goto _end;
            };
        };

        if (events & NAI_EV_READ) {
            r = nsi_npc_handle_read(c);
            if (r != NAI_DECLINED) {
                goto _end;
            };
        };

        if (events & NAI_EV_WRITE) {
            r = nsi_npc_handle_send(c);
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
                r = nsi_npc_handle_send(c);
                goto _end;
            case NAI_EV_READ:
                r = nsi_npc_handle(&c->io, NAI_EV_READ);
                goto _end;
            default:
                break;
            };

            break;

        case NAI_EV_TIMEOUT:
            if (c->reconnect) {
                nsi_npc_connect(c);
            };

            break;

        case NAI_EV_ERROR:
            ec = nai_ev_error_code(events);
            r = nsi_npc_handle_error(c, ec);
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



static nsi_npc_t* nsi_npc_create(
    nsi_endpoint_t* e, const nsi_endpoint_name_t* name)
{
    int len;
    nsi_npc_t* c;


    len = name ? name->len : 0;
    c = (nsi_npc_t*)nsi_network_alloc(e->net, sizeof(*c) + len);
    if (c == 0) {
        nai_log_alert(NSI_LOG_CORE, nai_errno, 
            "allocate npc endpoint(%s/%d) failed", 
            nai_str(&e->host), 0);
        goto _end;
    };

    c->cid = 0;
    c->uid = 0;
    c->pid = 0;
    c->ep = e;
    c->server = 0;
    c->flags = 0;
    c->out.qsize = 0;
    c->out.mcount = 0;
    c->out.zerocopy = 0;
    c->out.zpending = 0;
    c->out.zwaiting = 0;
    c->out.znotified = 0;
    c->out.npcsynmsg[0] = NPC_MSG_SYN_TYPE;
    c->out.npcackmsg[0] = NPC_MSG_ACK_TYPE;
    c->out.npcnotifymsg[0] = NPC_MSG_SND_TYPE;
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
    nai_stream_set_cb(&c->io, nsi_npc_handle);

_end:
    return c;
};


static int nsi_npc_set_opts(nsi_npc_t* c, nsi_network_t* n)
{
    int r;
#if defined(SO_SNDLOWAT)
    int val;
    int ec;
    intptr_t bsize;
#endif

    if (n->recv_buf_tcp != (uint32_t)-1) {
        r = nai_stream_set_opt(&c->io, NAI_IO_RECVBUF, n->recv_buf_tcp);
        if (r < 0) {
            nai_log_crit(NSI_LOG_CORE, nai_errno, 
                "npc endpoint(%s) set recv bufsize(%d) failed", 
                nai_str(&c->ep->host), n->recv_buf_tcp);
            goto _end;
        };
    };
    if (n->send_buf_tcp != (uint32_t)-1) {
        r = nai_stream_set_opt(&c->io, NAI_IO_SENDBUF, n->send_buf_tcp);
        if (r < 0) {
            nai_log_crit(NSI_LOG_CORE, nai_errno, 
                "npc endpoint(%s) set send bufsize(%d) failed", 
                nai_str(&c->ep->host), n->send_buf_tcp);
            goto _end;
        };
    };

#if defined(SO_SNDLOWAT)
    r = nai_stream_get_opt(&c->io, NAI_IO_SENDBUF, &bsize);
    if (r < 0) {
        nai_log_crit(NSI_LOG_CORE, nai_errno, 
            "npc endpoint(%s) get send bufsize failed", 
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
                    "npc endpoint(%s/%d) set sndlowat(%d) failed", 
                    nai_str(&c->ep->host), c->cid, val);
                goto _end;
            };
        };
    };
#endif

    r = 0;

_end:
    return r;
};


static int nsi_npc_wait_connect(nsi_npc_t* c)
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
        nai_evnode_set_cb(&c->io.ev, (nai_evnode_cb_f)nsi_npc_handle);
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


static int nsi_npc_connect(nsi_npc_t* c)
{
    int r;
    int ec;
    int blocking;
    int events;
    nsi_network_t* n;
    nsi_endpoint_name_t* dest;

    nai_log_debug(NSI_LOG_CORE, 0, 
        "npc endpoint(%s/%d) start %s", 
        nai_str(&c->ep->host), c->cid, 
        c->reconnect ? "reconnect" : "connect");

    /* do bind */
    r = nsi_iobase_bind((nsi_iobase_t*)c, 1);
    if (r < 0) {
        ec = nai_errno;
        nai_log_error(NSI_LOG_CORE, ec, 
            "npc endpoint(%s/%d) bind address failed", 
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

    nai_stream_set_blocking(&c->io, 0);

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
                    "npc endpoint(%s/%d) set blocking(0) failed", 
                    nai_str(&c->ep->host), c->cid);

                goto _error;
            };

            r = nai_stream_post(&c->io, NAI_EV_READ);
            if (r < 0) {
                nai_log_crit(NSI_LOG_CORE, nai_errno, 
                    "npc endpoint(%s/%d) post failed", 
                    nai_str(&c->ep->host), c->cid);

                goto _error;
            };
        };

        r = nsi_npc_set_opts(c, n);
        if (r < 0) {
            nai_log_crit(NSI_LOG_CORE, nai_errno, 
                "npc endpoint(%s/%d) set options failed", 
                nai_str(&c->ep->host), c->cid);

            goto _error;
        };

        c->reconnect = 0;

        if (!c->server && c->retry) {
            events = nai_stream_get_event(&c->io);
            if (events) {
                r = nai_stream_post(&c->io, events);
                if (r < 0) {
                    nai_log_crit(NSI_LOG_CORE, nai_errno, 
                        "npc endpoint(%s/%d) post failed", 
                        nai_str(&c->ep->host), c->cid);

                    goto _error;
                };
            };
        };

    } else {
        if (c->reconnect) {
            nai_log_crit(NSI_LOG_CORE, nai_errno, 
                "npc endpoint(%s/%d) connect fails directly", 
                nai_str(&c->ep->host), c->cid);

            goto _error;
        };
    };

_end:
    return r;

_error:
    if (c->reconnect) {
        /* set timer and try connect again */
        r = nsi_npc_wait_connect(c);
        if (r < 0) {
            ec = nai_errno;
            nai_log_error(NSI_LOG_CORE, ec, 
                "npc endpoint(%s/%d) failed to start wait connect", 
                nai_str(&c->ep->host), c->cid);

            goto _alert;
        };

        r = 0;
    };
    goto _end;

_alert:
    if (c->reconnect) {
        r = nsi_npc_handle_alert(c, ec);
    };
    goto _end;
};


static int nsi_npc_send(
    nsi_npc_t* c, nsi_message_t* m, uint32_t uid, int flags)
{
    int r;
    nsi_message_t* o;
    npc_port_t* port;

    if (uid) {
        if (uid == c->uid) {
            r = 0;
            goto _skip;
        };
        c->uid = uid;
    };

    if (flags & NSI_SEND_MSGDUP) {
        o = m;
        port = m->port;
        m->port = c->comif->defport;
        m = nsi_message_dup(m, 0);
        o->port = port;
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
                    "npc endpoint(%s) do post failed, send now", 
                    nai_str(&c->ep->host));
            };

            r = nsi_npc_handle_send(c);

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


static int nsi_npc_flush(nsi_npc_t* c)
{
    int r;

    if (nai_stream_is_writable(&c->io)) {
        r = nsi_npc_handle_send(c);
    } else {
        r = 0;
    };

    return r;
};


static int nsi_npc_close_impl(
    nsi_npc_t* c, nai_list_entry_t* msgs, int reset)
{
    int r;
    uint16_t peer;

    if (c->connected) {
        if (c->comif != 0) {
            peer = 0xffff;
            if (c->server) {
                peer = c->cid;
            };
            npc_comif_clean(c->comif, peer);
         
            if (c->portread != 0) {
                npc_port_close(c->portread);
            };
            if (c->portwrite !=0) {
                npc_port_close(c->portwrite);
            };

            if (!c->server) {
                npc_comif_close(c->comif);
            };
            c->portread = 0;
            c->portwrite = 0;
            c->comif = 0;
        };

        c->connected = 0;

        if (reset) {
            /* reset stream requires reservation of address, 
             * we can't call shutdown because it will release binding 
             * address, so disable events only.
             */
            r = nai_stream_set_mode(&c->io, 0);
            if (r < 0) {
                nai_log_error(NSI_LOG_CORE, nai_errno, 
                    "npc endpoint(%s/%d) disable i/o events failed", 
                    nai_str(&c->ep->host), c->cid);
            };
        } else {
            r = nai_stream_shutdown(&c->io, NAI_SOCK_WR);
            if (r < 0) {
                nai_log_error(NSI_LOG_CORE, nai_errno, 
                    "npc endpoint(%s/%d) shutdown failed", 
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
                "npc endpoint(%s/%d) close failed", 
                nai_str(&c->ep->host), c->cid);
            goto _error;
        };
    };

    if (reset) {
        /* set timer and waiting to connect */
        r = nsi_npc_wait_connect(c);
        if (r < 0) {
            nai_log_error(NSI_LOG_CORE, nai_errno, 
                "npc endpoint(%s/%d) failed to start wait connect", 
                nai_str(&c->ep->host), c->cid);
            goto _alert;
        };
    };

    nai_list_add_tail(msgs, &c->out.waiting);
    nai_list_add_tail(msgs, &c->out.queued);
    c->out.qsize = 0;
    c->out.mcount = 0;
    c->out.zerocopy = 0;
    c->out.zwaiting = 0;
    c->out.zpending = 0;
    c->out.znotified = 0;
    c->npcack = 0;
    c->npcsyn = 0;
    r = 0;

_end:
    return r;

_error:
    if (!reset) {
        goto _end;
    };
    r = nsi_npc_handle_error(c, nai_errno);
    goto _end;

_alert:
    r = nsi_npc_handle_alert(c, nai_errno);
    goto _end;
};


static int nsi_npc_reset(nsi_npc_t* c, nai_list_entry_t* msgs)
{
    int r;

    r = nsi_npc_close_impl(c, msgs, 1);

    return r;
};


static int nsi_npc_close(nsi_npc_t* c, nai_list_entry_t* msgs)
{
    int r;

    r = nsi_npc_close_impl(c, msgs, 0);
    if (r < 0) {
        goto _end;
    };

    nsi_network_free(c->ep->net, c);
    r = 0;

_end:
    return r;
};


//////////////////////////////////////////////////////////////////////////////
// stream server

#define nsi_npc_bind_find_conn   nsi_endpoint_bind_find_conn
#define nsi_npc_bind_find_name   nsi_endpoint_bind_find_name

static int nsi_npc_server_alert(nsi_npc_server_t* s, int errcode)
{
    return nsi_endpoint_handle_except(s->ep, 0, errcode, 1);
};

static int nsi_npc_server_alloc_cid(nsi_npc_server_t* s, nsi_npc_t* c)
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

        n = nsi_npc_find(&s->clients, cid, 0);
        if (n[0]) {
            /* connection id exists */
            continue;
        };

        n = nsi_npc_bind_find_conn(&s->conns, cid, &parent);
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

static int nsi_npc_server_accept(nai_server_t* l, int events)
{
    int r;
    int ec;
    nai_fd_t fd;
    nsi_npc_server_t* s;
    nsi_npc_bind_t* b;
    npc_msg_sync_t* syn;
    nsi_npc_t* c;
    nai_rbnode_t** n;
    nai_rbnode_t* parent;
    nai_list_entry_t msgs;
    nsi_network_t* net;
    nsi_endpoint_name_t name;
    nai_str_t* shmpath;
    char buf[140];

    s = (nsi_npc_server_t*)l;
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

        nai_log_debug(NSI_LOG_CORE, 0, 
                        "server endpoint(%s) accept connection",nai_str(&s->ep->host));

        name.len = s->nbuf.len;
        name.addr = &s->nbuf.addr;
        n = nsi_npc_bind_find_name(&s->names, name.addr, name.len, 0);
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

        c = nsi_npc_create(s->ep, &name);
        if (c == 0) {
            ec = nai_errno;
            nai_sock_close(fd);
            nai_log_alert(NSI_LOG_CORE, ec, 
                "server endpoint(%s) failed to allocate stream", 
                nai_str(&s->ep->host));

            r = nsi_npc_server_alert(s, ec);
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
            r = nsi_npc_set_opts(c, net);
            if (r < 0) {
                ec = nai_errno;
                nai_log_crit(NSI_LOG_CORE, 
                    ec, "stream endpoint(%s) set options failed", 
                    nai_str(&c->ep->host));
            };
        };
        if (r < 0) {
            nai_list_init(&msgs);
            nsi_npc_close(c, &msgs);
            r = nsi_npc_server_alert(s, ec);
            if (r != NAI_DECLINED) {
                break;
            };
            continue;
        };

        /* mark is server */
        c->server = 1;

        /* find exists cid or allocate a new one */
        n = nsi_npc_bind_find_name(&s->names, name.addr, name.len, 0);
        if (n[0] == 0) {
            nsi_npc_server_alloc_cid(s, c);
        } else {
            b = nai_containof(n[0], nsi_npc_bind_t, entn);
            c->cid = b->cid;
        };

        /* */
        c->comif = s->comif;
        c->portread = npc_port_create(s->comif, 1, 1024, NPC_PORT_READ, c->cid);
        c->portwrite = npc_port_create(s->comif, 1, 1024, NPC_PORT_SEND);
        if (c->portwrite != 0) {
            r = npc_port_add(c->portwrite, c->cid);
        };
        if ((c->portread == 0) || (c->portwrite == 0) || (r < 0)) {
            nai_list_init(&msgs);
            nsi_npc_close(c, &msgs);
            ec = nai_errno;
            r = nsi_npc_server_alert(s, ec);
            if (r != NAI_DECLINED) {
                break;
            };
            continue;
        };

        c->portread->owner = c->cid;
        c->portwrite->owner = c->cid;

        syn = (npc_msg_sync_t*)(&c->out.npcsynmsg[1]);
        shmpath = &s->ep->net->rt->conf->net.npc_shm_path;
        nai_memcpy(syn->shmpath, nai_str(shmpath), nai_str_len(shmpath));
        syn->shmpath[nai_str_len(shmpath)] = 0;
        syn->segsize = s->ep->net->npc_segsize;
        syn->segcount = s->ep->net->npc_segcount;
        syn->inst = s->ep->inst;
        syn->serv = s->ep->serv;
        syn->peer = c->cid;
        syn->portread = c->portread->mref;
        syn->portwrite = c->portwrite->mref;
        nai_log_info(NSI_LOG_CORE, 
            0, "port read is %x and port write is %x", 
            syn->portread, syn->portwrite);
        c->out.npcsynmsg[0] = NPC_MSG_SYN_TYPE;
        c->out.npcackmsg[0] = NPC_MSG_ACK_TYPE;
        c->out.npcnotifymsg[0] = NPC_MSG_SND_TYPE;

        /* insert into the map of clients */
        n = nsi_npc_find(&s->clients, c->cid, &parent);
        assert(n[0] == 0);
        nai_rbtree_link(&s->clients, &c->ent, parent, n);
        nai_rbtree_color(&s->clients, &c->ent);

        /* complete */
        events = nai_stream_get_event(&c->io);
        if (events) {
            nai_log_debug(NSI_LOG_CORE, 0, "nsi_npc_server_accept get events is %d",events);
            r = nsi_npc_handle(&c->io, events);
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


static int nsi_npc_server_bind_name(
    nsi_npc_server_t* s, nsi_endpoint_name_t* name, int temp)
{
    int r;
    nai_rbnode_t** n;
    nai_rbnode_t* parent;
    nai_rbnode_t* ent;
    nsi_npc_t* c;
    nsi_npc_bind_t* b;

    if (name->addr == 0 || 
        name->addr->sa_family != s->ep->name.addr->sa_family) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    n = nsi_npc_bind_find_name(&s->names, name->addr, name->len, &parent);
    if (n[0] != 0) {
        b = nai_containof(n[0], nsi_npc_bind_t, entn);
    } else {
        b = (nsi_npc_bind_t*)
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
            c = nai_containof(ent, nsi_npc_t, ent);
            if (nsi_endpoint_name_equal(name, &c->name)) {
                break;
            };
        };
        if (ent != nai_rbtree_end(&s->clients)) {
            b->cid = c->cid;
        } else {
            b->cid = nsi_npc_server_alloc_cid(s, 0);
        };

        n = nsi_npc_bind_find_conn(&s->conns, b->cid, &parent);
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


static int nsi_npc_server_bind_conn(
    nsi_npc_server_t* c, nsi_connid_t cid)
{
    int r;
    nai_rbnode_t** n;
    nai_rbnode_t* parent;
    nsi_npc_bind_t* b;

    n = nsi_npc_bind_find_conn(&c->clients, cid, &parent);
    if (n[0] == 0) {
        nai_errno = ENOENT;
        r = -1;
        goto _end;
    };

    b = nai_containof(n[0], nsi_npc_bind_t, entn);
    r = nsi_npc_server_bind_name(c, &b->name, 0);

_end:
    return r;
};


static int nsi_npc_server_unbind_name(
    nsi_npc_server_t* s, nsi_connid_t cid)
{
    int r;
    nai_rbnode_t** n;
    nai_rbnode_t* parent;
    nsi_npc_bind_t* b;

    n = nsi_npc_bind_find_conn(&s->conns, cid, &parent);
    if (n[0] == 0) {
        nai_errno = ENOENT;
        r = -1;
        goto _end;
    };

    b = nai_containof(n[0], nsi_npc_bind_t, entc);
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


static int nsi_npc_server_open(nsi_endpoint_t* p)
{
    int r;
    int ec;
    nsi_network_t* n;
    nsi_npc_server_t* s;
    npc_shmz_attr_t shm_attr;

    s = (nsi_npc_server_t*)nsi_network_alloc(p->net, sizeof(*s));
    if (s == 0) {
        nai_log_alert(NSI_LOG_CORE, 
            nai_errno, "allocate server endpoint(%s) failed", 
            nai_str(&p->host));
        r = -1;
        goto _end;
    };
    nai_shm_unlink(nai_str(&p->net->rt->conf->net.npc_shm_path));
    shm_attr.serv = p->serv;
    shm_attr.inst = p->inst;
    shm_attr.self = 0xffff;
    shm_attr.seg_size = p->net->npc_segsize;
    shm_attr.seg_count = p->net->npc_segcount;
    s->comif = npc_comif_create(p->net->npc_comm,
        &comif_server_ops, s, nai_str(&p->net->rt->conf->net.npc_shm_path), &shm_attr, NAI_O_CREAT|NAI_O_RDWR, 0644);
    if (s->comif == 0) {
        nai_log_alert(NSI_LOG_CORE, nai_errno, 
            "npc server endpoint create comif failed");
        goto _fail;
    };

    s->ep = p;
    s->nextc = 0;
    nai_rbtree_init(&s->clients);
    nai_rbtree_init(&s->conns);
    nai_rbtree_init(&s->names);
    nai_server_init(&s->io);
    nai_server_set_opt(&s->io, NAI_IO_REUSEADDR, 1);
    nai_server_set_cb(&s->io, nsi_npc_server_accept);

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
    r = 0;

_end:
    return r;

_fail:
    ec = nai_errno;
    nsi_network_free(p->net, s);
    if (s->comif) {
        npc_comif_close(s->comif);
    };
    nai_errno = ec;
    r = -1;
    goto _end;
};


static int nsi_npc_server_map_conn(
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


static int nsi_npc_server_get_name(
    nsi_endpoint_t* p, nsi_connid_t cid, nsi_endpoint_name_t* name)
{
    int r;
    nai_rbnode_t** n;
    nsi_npc_t* c;
    nsi_npc_server_t* s;
    nsi_npc_bind_t* b;
    nsi_endpoint_name_t addr;

    s = (nsi_npc_server_t*)p->ctx;
    if (cid == 0) {
        addr = p->name;
    } else {
        n = nsi_npc_find(&s->clients, cid, 0);
        if (n[0] != 0) {
            c = nai_containof(n[0], nsi_npc_t, ent);
            addr = c->name;
        } else {
            n = nsi_npc_bind_find_conn(&s->conns, cid, 0);
            if (n[0] == 0) {
                nai_errno = ENOENT;
                r = -1;
                goto _end;
            };

            b = nai_containof(n[0], nsi_npc_bind_t, entn);
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


static int nsi_npc_server_get_opt(
    nsi_endpoint_t* p, int opt, intptr_t* value)
{
    int r;
    nsi_npc_server_t* s;

    s = (nsi_npc_server_t*)p->ctx;
    r = nai_stream_get_opt(&s->io, opt, value);

    return r;
};


static int nsi_npc_server_set_opt(
    nsi_endpoint_t* p, int opt, intptr_t value)
{
    int r;
    nai_rbnode_t** n;
    nai_rbnode_t* e;
    nsi_npc_t* c;
    nsi_npc_server_t* s;

    s = (nsi_npc_server_t*)p->ctx;
    switch (opt) {
    case NSI_EOPT_CHECK_CSTAT:
        if (value == 0) {
            r = 1;
        } else {
            n = nsi_npc_find(&s->clients, (nsi_connid_t)value, 0);
            r = n[0] != 0;
        };
        break;
    case NSI_EOPT_QUERY_UCODE:
        n = nsi_npc_find(&s->clients, (nsi_connid_t)value, 0);
        if (n[0] == 0) {
            nai_errno = ENOENT;
            r = -1;
            break;
        };

        c = nai_containof(n[0], nsi_npc_t, ent);
        r = c->uid;
        break;
    case NSI_EOPT_RESET_UCODE:
        e = nai_rbtree_begin(&s->clients);
        for ( ; e != nai_rbtree_end(&s->clients); ) {
            c = (nsi_npc_t*)e;
            e = nai_rbtree_next(e);
            c->uid = 0;
        };
        r = 0;
        break;
    case NSI_EOPT_BIND_NAME:
        r = nsi_npc_server_bind_name(s, (nsi_endpoint_name_t*)value, 0);
        break;
    case NSI_EOPT_BIND_TEMP:
        r = nsi_npc_server_bind_name(s, (nsi_endpoint_name_t*)value, 1);
        break;
    case NSI_EOPT_BIND_MCAST:
        nai_errno = ENOTSUP;
        r = -1;
        break;
    case NSI_EOPT_BIND_CONN:
        r = nsi_npc_server_bind_conn(s, (nsi_connid_t)value);
        break;
    case NSI_EOPT_UNBIND_NAME:
        r = nsi_npc_server_unbind_name(s, (nsi_connid_t)value);
        break;
    default:
        r = nai_stream_set_opt(&s->io, opt, value);
        break;
    };

    return r;
};


static int nsi_npc_server_send(
    nsi_endpoint_t* p, nsi_message_t* m, uint32_t uid, int flags)
{
    int r;
    nai_rbnode_t** n;
    nsi_npc_t* c;
    nsi_npc_server_t* s;
    nsi_network_t* t;

    s = (nsi_npc_server_t*)p->ctx;
    n = nsi_npc_find(&s->clients, m->conn, 0);
    if (n[0] == 0) {
        nai_errno = ENOENT;
        r = -1;
        goto _end;
    };

    t = p->net;
    c = nai_containof(n[0], nsi_npc_t, ent);
    if (c->out.qsize >= (p->local ? t->queue_mix_limit : t->queue_limit)) {
        if (!nsi_is_proxy_message(m)) {
            nai_errno = ENOSPC;
            r = -1;
            goto _end;
        };
    };

    r = nsi_npc_send(c, m, uid, flags);

_end:
    return r;
};


static int nsi_npc_server_flush(nsi_endpoint_t* p)
{
    int r;

    (void)p;

    r = 0;

    return r;
};


static int nsi_npc_server_close(nsi_endpoint_t* p)
{
    int r;
    size_t qsize;
    nai_rbnode_t* e;
    nsi_npc_t* c;
    nsi_npc_server_t* s;
    nai_list_entry_t msgs;

    s = (nsi_npc_server_t*)p->ctx;
    if (s == 0) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

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
        c = nai_containof(e, nsi_npc_t, ent);
        e = nai_rbtree_next(e);
        qsize += c->out.qsize;
        nai_rbtree_erase(&s->clients, &c->ent);
        nsi_npc_close(c, &msgs);
    };

    /* callback messages */
    nsi_endpoint_handle_sent(p, &msgs, &qsize, ECANCELED);

    /* free */
    nai_bufpool_close(&s->pool);
    nsi_network_free(p->net, s);
    npc_comif_close(s->comif);

    /* unlink */
    p->connected = 0;
    p->ctx = 0;
    r = 0;

_end:
    return r;
};


//////////////////////////////////////////////////////////////////////////////
// stream client


static int nsi_npc_client_open(nsi_endpoint_t* p)
{
    int r;
    int ec;
    nsi_npc_t* c;
    nai_list_entry_t msgs;

    c = nsi_npc_create(p, 0);
    if (c == 0) {
        r = -1;
        goto _end;
    };

    if (p->waiting) {
        c->reconnect = 1;
    };

    r = nsi_npc_connect(c);
    if (r < 0) {
        ec = nai_errno;
        nai_list_init(&msgs);
        nsi_npc_close(c, &msgs);
        nai_errno = ec;
        goto _end;
    };

    p->connected = 0;
    p->ctx = c;
    r = 0;

_end:
    return r;
};


static int nsi_npc_client_map_conn(
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


static int nsi_npc_client_get_name(
    nsi_endpoint_t* p, nsi_connid_t cid, nsi_endpoint_name_t* name)
{
    int r;
    nsi_npc_t* c;
    nsi_endpoint_name_t addr;

    if (cid) {
        nai_errno = ENOENT;
        r = -1;
        goto _end;
    };

    c = (nsi_npc_t*)p->ctx;
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


static int nsi_npc_client_get_opt(
    nsi_endpoint_t* p, int opt, intptr_t* value)
{
    int r;
    nsi_npc_t* c;

    c = (nsi_npc_t*)p->ctx;
    r = nai_stream_get_opt(&c->io, opt, value);

    return r;
};


static int nsi_npc_client_set_opt(
    nsi_endpoint_t* p, int opt, intptr_t value)
{
    int r;
    nsi_npc_t* c;

    c = (nsi_npc_t*)p->ctx;
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


static int nsi_npc_client_send(
    nsi_endpoint_t* p, nsi_message_t* m, uint32_t uid, int flags)
{
    int r;
    nsi_npc_t* c;
    nsi_network_t* n;

    n = p->net;
    c = (nsi_npc_t*)p->ctx;
    if (c->out.qsize >= (p->local ? n->queue_mix_limit : n->queue_limit)) {
        if (!nsi_is_proxy_message(m)) {
            nai_errno = ENOSPC;
            r = -1;
            goto _end;
        };
    };

    r = nsi_npc_send(c, m, uid, flags);

_end:
    return r;
};


static int nsi_npc_client_flush(nsi_endpoint_t* p)
{
    int r;
    nsi_npc_t* c;

    c = (nsi_npc_t*)p->ctx;
    r = nsi_npc_flush(c);

    return r;
};


static int nsi_npc_client_close(nsi_endpoint_t* p)
{
    int r;
    //size_t qsize;
    nsi_npc_t* c;
    nai_list_entry_t msgs;

    c = (nsi_npc_t*)p->ctx;
    if (c == 0) {
        nai_errno = EINVAL;
        r = -1;
        goto _end;
    };

    /* close stream */
    nai_list_init(&msgs);
    r = nsi_npc_close(c, &msgs);
    if (r < 0) {
        goto _end;
    };

    /* unlink */
    p->connected = 0;
    p->ctx = 0;
    r = 0;

_end:
    return r;
};



nsi_endpoint_ops_t nsi_npc_server_ops = {
    nsi_npc_server_open, 
    nsi_npc_server_map_conn, 
    nsi_npc_server_get_name, 
    nsi_npc_server_get_opt, 
    nsi_npc_server_set_opt, 
    nsi_npc_server_send, 
    nsi_npc_server_flush, 
    nsi_npc_server_close
};

nsi_endpoint_ops_t nsi_npc_client_ops = {
    nsi_npc_client_open, 
    nsi_npc_client_map_conn, 
    nsi_npc_client_get_name, 
    nsi_npc_client_get_opt, 
    nsi_npc_client_set_opt, 
    nsi_npc_client_send, 
    nsi_npc_client_flush, 
    nsi_npc_client_close
};


