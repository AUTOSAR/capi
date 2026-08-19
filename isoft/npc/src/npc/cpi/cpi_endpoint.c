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
/// @file       cpi_endpoint.c
/// @brief
/// @details
/// @date       2022-11-11
/// @author     Zan Shigang
/// @version    1.2.0
///
/// ================================================================

#include "nai/os/nai_socket.h"
#include "npc/cpi/cpi_app.h"
#include "npc/cpi/cpi_mm.h"
#include "npc/cpi/cpi_endpoint.h"
#include "npc/cpi/cpi_buf.h"
#include "npc/cpi/cpi_error.h"
#include "npc/cpi/cpi_frame.h"

cpi_endpoint_t *
cpi_endpoint_new(cpi_endpoint_cb_t cb,
                 cpi_endpoint_net_ops_t *ops,
                 void *owner,
                 nai_iobase_cb_f iocb,
                  cpi_endpoint_addr_t *addr)
{
    cpi_endpoint_t *ep = (cpi_endpoint_t *)cpi_malloc(sizeof(cpi_endpoint_t));
    if (ep == NULL)
        goto out;

    ep->peerid = -1;
    if (addr == NULL)
        memset(&ep->addr, 0, sizeof(cpi_endpoint_addr_t));
    else
        ep->addr = *addr;
    ep->callback = cb;
    ep->ops = *ops;
    ep->status = cpi_ep_init;
    ep->owner = owner;
    ep->rcv_head = ep->rcv_tail = NULL;
    ep->snd_head = ep->snd_tail = NULL;
    ep->sending = NULL;
    ep->e_head = ep->e_tail = NULL;
    nai_iobase_init(&ep->iobase);
    nai_iobase_set_cb(&ep->iobase, iocb);
    npc_cred_init(&ep->cred);
    ep->ref = 0;
    ep->handshake = 0;
    ep->positive = 0;
    ep->cred_flag = 0;

out:
    return ep;
}

void cpi_endpoint_free(cpi_endpoint_t *ep)
{
    cpi_buf_t *b;
    cpi_exec_t *e;
    if (ep != NULL) {
        nai_iobase_close(&ep->iobase);
        if (ep->sending != NULL)
            cpi_buf_free(ep->sending);
        while ((b = ep->rcv_head) != NULL) {
            cpi_buf_chain_del(&ep->rcv_head, &ep->rcv_tail, b);
            cpi_buf_free(b);
        }
        while ((b = ep->snd_head) != NULL) {
            cpi_buf_chain_del(&ep->snd_head, &ep->snd_tail, b);
            cpi_buf_free(b);
        }
        while ((e = ep->e_head) != NULL) {
            cpi_exec_no_transfer(e, ep);
            e->error = CPI_ECLOS;
            cpi_exec_set_error(e, CPI_ECLOS);
            cpi_exec_feedback(e);
        }
        cpi_free(ep);
    }
}

int cpi_endpoint_cmp(const cpi_endpoint_t *ep1, const cpi_endpoint_t *ep2)
{
    return ep1->ops.get_handle(ep1) - ep2->ops.get_handle(ep2);
}

int cpi_endpoint_id_cmp(const cpi_endpoint_t *ep1, const cpi_endpoint_t *ep2)
{
    return ep1->peerid - ep2->peerid;
}

void cpi_endpoint_buf_append(cpi_endpoint_t *ep, cpi_buf_t *b, int btype)
{
    if (btype == CPI_ENDPOINT_RECV) {
        cpi_buf_chain_add(&ep->rcv_head, &ep->rcv_tail, b);
    } else {
        if (!cpi_endpoint_get_handshake(ep) && ep->snd_head == NULL) {
            cpi_app_ep_connect_peerid_send(ep);
        }
        cpi_buf_chain_add(&ep->snd_head, &ep->snd_tail, b);
    }
}

int cpi_endpoint_get_recv_buf(cpi_endpoint_t *ep, void *buf, cpi_u32_t size)
{
    int r = RET(CPI_ESUCC);
    cpi_buf_t *b;
    cpi_u32_t bsize = 0;
    cpi_u8ptr_t p = (cpi_u8ptr_t)buf;

    for (b = ep->rcv_head; b != NULL; b = b->next) {
        bsize += cpi_buf_get_left_size(b);
        if (bsize >= size)
            break;
    }
    if (b != NULL) {
        while ((b = ep->rcv_head) != NULL) {
            if (cpi_buf_get_left_size(b) > size) {
                memcpy(p, cpi_buf_get_pos(b), size);
                cpi_buf_inc_pos(b, size);
                break;
            }
            memcpy(p, cpi_buf_get_pos(b), cpi_buf_get_left_size(b));
            p += cpi_buf_get_left_size(b);
            size -= cpi_buf_get_left_size(b);
            cpi_buf_free(cpi_endpoint_rcv_pop(ep));
        }
    } else {
        r = RET(CPI_EBLEN);
    }
    return r;
}

cpi_endpoint_t *cpi_endpoint_search_by_id(cpi_rbtree_t *eps, int appid)
{
    cpi_rbtree_node_t *rn;
    cpi_endpoint_t e, *ep;

    e.peerid = appid;
    rn = cpi_rbtree_search(eps, &e);
    if (cpi_rbtree_null(rn)) {
        ep = NULL;
    } else {
        ep = (cpi_endpoint_t *)(rn->data);
    }

    return ep;
}

void cpi_endpoint_remove(cpi_rbtree_t *eps, cpi_endpoint_t *ep)
{
    cpi_rbtree_node_t *rn;

    rn = cpi_rbtree_search(eps, ep);
    if (!cpi_rbtree_null(rn)) {
        cpi_rbtree_delete(rn);
        cpi_rbtree_node_free(rn);
    }
}

int cpi_endpoint_add(cpi_rbtree_t *eps, cpi_endpoint_t *ep)
{
    int r = RET(CPI_ESUCC);
    cpi_rbtree_node_t *rn;

    if ((rn = cpi_rbtree_node_new(eps, ep)) == NULL) {
        r = RET(CPI_ENMEM);
        goto out;
    }
    cpi_rbtree_insert(rn);

out:
    return r;
}

int cpi_endpoint_addr_pton(const char *addr, int len, cpi_endpoint_addr_t *paddr)
{
    paddr->addr_len = cpi_endpoint_is_net(paddr)? sizeof(paddr->net_addr): sizeof(paddr->unix_addr);
    return nai_sockaddr_pton(addr, len, &paddr->net_addr, &paddr->addr_len);
}

void cpi_endpoint_cred_get(cpi_endpoint_t *ep)
{
    int r;
#if defined(SO_PEERCRED)

    int len;
    struct ucred cred;


    len = sizeof(cred);
    r = nai_sock_getsockopt(ep->ops.get_handle(ep),
        SOL_SOCKET, SO_PEERCRED, (char*)&cred, &len);
    if (r >= 0) {
        ep->cred.type = NPC_CRED_PID;
        ep->cred.len = sizeof(ep->cred.pid);
        ep->cred.pid = cred.pid;
    } else {
        ep->cred.pid = 0;
        npc_cred_init(&ep->cred);
    }

#elif defined(LOCAL_PEERPID)

    int len;
    int pid;


    len = sizeof(pid);
    r = nai_sock_getsockopt(ep->ops.get_handle(ep),
        SOL_LOCAL, LOCAL_PEERPID, (char*)&pid, &len);
    if (r >= 0) {
        ep->cred.type = NPC_CRED_PID;
        ep->cred.len = sizeof(ep->cred.pid);
        ep->cred.pid = pid;
    } else {
        ep->cred.pid = 0;
        npc_cred_init(&ep->cred);
    }

#elif defined(LOCAL_PEERCRED)

    int len;
    struct xucred cred;

    len = sizeof(cred);
    r = nai_sock_getsockopt(ep->ops.get_handle(ep),
        SOL_LOCAL, LOCAL_PEERCRED, (char*)&cred, &len);
    if (r >= 0) {
        ep->cred.type = NPC_CRED_PID;
        ep->cred.len = sizeof(ep->cred.pid);
        ep->cred.pid = cred.pid;
    } else {
        ep->cred.pid = 0;
        npc_cred_init(&ep->cred);
    }

#elif defined(_WIN32)
    ep->cred.type = NPC_CRED_PID;
    ep->cred.len = sizeof(ep->cred.pid);
    ep->cred.pid = 0;
#else
#warn "the platform can not get peer pid."
#endif
    cpi_endpoint_set_cred(ep);
}

