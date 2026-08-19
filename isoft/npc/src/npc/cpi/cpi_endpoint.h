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
/// @file       cpi_endpoint.h
/// @brief
/// @details
/// @date       2022-11-11
/// @author     Zan Shigang
/// @version    1.2.0
///
/// ================================================================

#ifndef __CPI_ENDPOINT_H
#define __CPI_ENDPOINT_H


#include "nai/os/nai_socket.h"
#include "nai/io/nai_event.h"
#include "nai/io/nai_io.h"
#include "npc/cpi/cpi_types.h"
#include "npc/cpi/cpi_buf.h"
#include "npc/cpi/cpi_exec.h"
#include "npc/cpi/cpi_rbtree.h"
#include <sys/socket.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define CPI_ENDPOINT_SEND 0
#define CPI_ENDPOINT_RECV 1

#define CPI_ENDPOINT_ADDR_NET  0
#define CPI_ENDPOINT_ADDR_UNIX 1

    typedef struct cpi_endpoint_s cpi_endpoint_t;
    typedef struct cpi_endpoint_net_ops_s cpi_endpoint_net_ops_t;

    typedef struct
    {
        union
        {
            nai_sockaddr_t net_addr;
            nai_sockaddr_un_t unix_addr;
        };
        int addr_len;
        int addr_type;
    } cpi_endpoint_addr_t;

    typedef enum cpi_endpoint_cb_event_e
    {
        cpi_ep_ev_recv,
        cpi_ep_ev_send,
        cpi_ep_ev_establish,
        cpi_ep_ev_destroy
    } cpi_endpoint_cb_event_t;

    typedef enum cpi_endpoint_status_e
    {
        cpi_ep_init,
        cpi_ep_listen,
        cpi_ep_connect,
        cpi_ep_connect_established,
        cpi_ep_accept_established,
        cpi_ep_closed,
    } cpi_endpoint_status_t;

    typedef int (*cpi_endpoint_net_cb_t)(cpi_endpoint_t *);
    typedef int (*cpi_endpoint_handle_cb_t)(const cpi_endpoint_t *);
    typedef int (*cpi_endpoint_cb_t)(cpi_endpoint_t *ep, cpi_endpoint_cb_event_t event);

    struct cpi_endpoint_net_ops_s
    {
        cpi_endpoint_handle_cb_t get_handle;
        cpi_endpoint_net_cb_t open;
        cpi_endpoint_net_cb_t close;
        cpi_endpoint_net_cb_t listen;
        cpi_endpoint_net_cb_t connect;
        cpi_endpoint_net_cb_t send;
        cpi_endpoint_net_cb_t recv;
        cpi_endpoint_net_cb_t error;
    };

    struct cpi_endpoint_s
    {
        int peerid;
        cpi_endpoint_addr_t addr;
        void *owner;
        nai_iobase_t iobase;
        cpi_endpoint_cb_t callback;
        cpi_endpoint_net_ops_t ops;
        cpi_endpoint_status_t status;
        cpi_buf_t *rcv_head;
        cpi_buf_t *rcv_tail;
        cpi_buf_t *snd_head;
        cpi_buf_t *snd_tail;
        cpi_buf_t *sending;
        cpi_exec_t *e_head;
        cpi_exec_t *e_tail;
        cpi_cred_t cred;
        cpi_u32_t ref;
        cpi_u32_t handshake : 1;
        cpi_u32_t cred_flag : 1;
        cpi_u32_t positive : 1;
        cpi_u32_t padding : 29;
    };

#define cpi_endpoint_isset_cred(ep)         ((ep)->cred_flag)
#define cpi_endpoint_set_cred(ep)           ((ep)->cred_flag = 1)
#define cpi_endpoint_get_cred(ep)           (&((ep)->cred))
#define cpi_endpoint_is_positive(ep)        ((ep)->positive)
#define cpi_endpoint_set_positive(ep)       ((ep)->positive = 1)
#define cpi_endpoint_set_negative(ep)       ((ep)->positive = 0)
#define cpi_endpoint_set_nettype(epaddr, t) ((epaddr)->addr_type = (t))
#define cpi_endpoint_is_net(epaddr)         ((epaddr)->addr_type == CPI_ENDPOINT_ADDR_NET)
#define cpi_endpoint_set_handshake(ep)      ((ep)->handshake = 1)
#define cpi_endpoint_get_handshake(ep)      ((ep)->handshake)
#define cpi_endpoint_ref_inc(ep)            (++(ep)->ref)
#define cpi_endpoint_ref_dec(ep)            ((ep)->ref--)
#define cpi_endpoint_get_addr(ep)           ((ep)->addr)
#define cpi_endpoint_get_first_exec(ep)     ((ep)->e_head)
#define cpi_endpoint_set_peerid(ep, i)      ((ep)->peerid = (i))
#define cpi_endpoint_get_peerid(ep)         ((ep)->peerid)
#define cpi_endpoint_get_owner(ep)          ((ep)->owner)
#define cpi_endpoint_set_status(ep, s)      ((ep)->status = (s))
#define cpi_endpoint_get_status(ep)         ((ep)->status)
#define cpi_endpoint_get_iobase(ep)         (&(ep)->iobase)
#define cpi_endpoint_get_cb(ep)             ((ep)->callback)
#define cpi_endpoint_rcv_pop(ep)                                                                                       \
    ({                                                                                                                 \
        cpi_buf_t *_b = (ep)->rcv_head;                                                                                \
        if (_b != NULL)                                                                                                \
            cpi_buf_chain_del(&(ep)->rcv_head, &(ep)->rcv_tail, _b);                                                   \
        _b;                                                                                                            \
    })
#define cpi_endpoint_snd_head(ep) ((ep)->snd_head)
#define cpi_endpoint_snd_pop(ep)                                                                                       \
    ({                                                                                                                 \
        cpi_buf_t *_b = (ep)->snd_head;                                                                                \
        if (_b != NULL)                                                                                                \
            cpi_buf_chain_del(&(ep)->snd_head, &(ep)->snd_tail, _b);                                                   \
        _b;                                                                                                            \
    })
#define cpi_endpoint_snd_remove(ep)                                                                                    \
    ({                                                                                                                 \
        cpi_buf_t *_b = (ep)->snd_tail;                                                                                \
        if (_b != NULL)                                                                                                \
            cpi_buf_chain_del(&(ep)->snd_head, &(ep)->snd_tail, _b);                                                   \
        _b;                                                                                                            \
    })
#define cpi_endpoint_e_queue(ep) &(ep)->e_head, &(ep)->e_tail

    cpi_endpoint_t *cpi_endpoint_new(cpi_endpoint_cb_t cb,
                                     cpi_endpoint_net_ops_t *ops,
                                     void *owner,
                                     nai_iobase_cb_f iocb,
                                     cpi_endpoint_addr_t *addr);
    void cpi_endpoint_free(cpi_endpoint_t *ep);
    int cpi_endpoint_cmp(const cpi_endpoint_t *ep1, const cpi_endpoint_t *ep2);
    int cpi_endpoint_id_cmp(const cpi_endpoint_t *ep1, const cpi_endpoint_t *ep2);
    void cpi_endpoint_buf_append(cpi_endpoint_t *ep, cpi_buf_t *b, int btype);
    int cpi_endpoint_get_recv_buf(cpi_endpoint_t *ep, void *buf, cpi_u32_t size);
    cpi_endpoint_t *cpi_endpoint_search_by_id(cpi_rbtree_t *eps, int appid);
    void cpi_endpoint_remove(cpi_rbtree_t *eps, cpi_endpoint_t *ep);
    int cpi_endpoint_add(cpi_rbtree_t *eps, cpi_endpoint_t *ep);
    int cpi_endpoint_addr_pton(const char *addr, int len, cpi_endpoint_addr_t *paddr);
    void cpi_endpoint_cred_get(cpi_endpoint_t *ep);

#ifdef __cplusplus
}
#endif

#endif
