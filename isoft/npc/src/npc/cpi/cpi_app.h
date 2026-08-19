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
/// @file       cpi_app.h
/// @brief
/// @details
/// @date       2022-11-11
/// @author     Zan Shigang
/// @version    1.2.0
///
/// ================================================================

#ifndef __CPI_APP_H
#define __CPI_APP_H


#include "nai/io/nai_event.h"
#include "npc/shm/npc_comm.h"
#include "npc/core/npc_types.h"
#include "npc/cpi/cpi_conf.h"
#include "npc/cpi/cpi_event.h"
#include "npc/cpi/cpi_rbtree.h"
#include "npc/cpi/cpi_lock.h"
#include "npc/cpi/cpi_endpoint.h"
#include "npc/cpi/cpi_exec.h"
#include <pthread.h>
#include <sys/socket.h>

#ifdef __cplusplus
extern "C"
{
#endif

#define CPI_APP_RETRY_MAX 150

    typedef struct cpi_app_s cpi_app_t;
    typedef struct cpi_app_find_svc_s cpi_app_find_svc_t;
    typedef struct cpi_app_exec_pack_s cpi_app_exec_pack_t;
    typedef struct cpi_app_message_list_s cpi_app_message_list_t;

    typedef void (*cpi_app_quit_cb_t)(cpi_app_t *);
    typedef int (*cpi_app_sub_cb_t)(cpi_app_t *app,
                                    void *ud,
                                    unsigned int id,
                                    cpi_u16_t svc,
                                    cpi_u16_t inst,
                                    cpi_u16_t gid,
                                    cpi_u16_t eid,
                                    const cpi_cred_t *cred);
    typedef int (*cpi_app_suback_cb_t)(
        cpi_app_t *app, void *ud, int code, cpi_u16_t svc, cpi_u16_t inst, cpi_u16_t gid, cpi_u16_t eid);
    typedef int (*cpi_app_avail_cb_t)(
        cpi_app_t *app, void *ud, int code, cpi_u16_t svc, cpi_u16_t inst, cpi_u8_t major, cpi_u32_t minor);
    typedef int (*cpi_app_msg_cb_t)(cpi_app_t *app, void *ud, cpi_message_t *m);
    typedef int (*cpi_app_sent_cb_t)(cpi_app_t *app, void *ud, cpi_message_t *m, int errcode);
    typedef int (*cpi_app_service_cb_t)(cpi_app_t *a,
                                        void *ud,
                                        cpi_u16_t serv,
                                        cpi_u16_t inst,
                                        cpi_u8_t major,
                                        cpi_u32_t minor,
                                        const cpi_cred_t *cred);

    typedef enum cpi_app_type_e
    {
        app_type_unknown,
        app_type_dis,
        app_type_app
    } cpi_app_type_t;

    struct cpi_app_s
    {
        int id;
        cpi_endpoint_addr_t svc_addr;
        cpi_endpoint_addr_t dis_addr;
        nai_evloop_t *evloop;
        cpi_app_type_t type;
        cpi_rbtree_t *endpoints;
        cpi_rbtree_t *endpoints_id;
        cpi_rbtree_t *conf;
        cpi_rbtree_t *services;
        cpi_rbtree_t *find_services; /*only for discovery*/
        npc_comm_t shm_comm;
        void *data;
        cpi_spin_t lock;
        nai_evnode_t node;
        nai_evnode_t send_node;
        nai_evnode_t dis_node;
        cpi_exec_t *head;
        cpi_exec_t *tail;

        cpi_app_quit_cb_t quit_cb;
        cpi_endpoint_t *discovery;
        cpi_app_message_list_t *msg_head;
        cpi_app_message_list_t *msg_tail;
        cpi_app_sub_cb_t subscribe_callback;
        cpi_app_suback_cb_t subscribe_ack_callback;
        cpi_app_avail_cb_t avail_callback;
        cpi_app_msg_cb_t message_callback;
        cpi_app_sent_cb_t sent_callback;
        cpi_app_service_cb_t iam_offer_callback;
        cpi_app_service_cb_t iam_find_callback;
        cpi_app_sub_cb_t iam_subscribe_callback;
        cpi_app_msg_cb_t iam_message_callback;
        cpi_s64_t send_timeout;
        cpi_u32_t quit_flag;
    };

    struct cpi_app_find_svc_s
    {
        int appid;
        cpi_u16_t svc;
        cpi_u16_t inst;
        cpi_u32_t major;
        cpi_u32_t minor;
        struct cpi_app_find_svc_s *prev;
        struct cpi_app_find_svc_s *next;
    };

    struct cpi_app_exec_pack_s
    {
        cpi_app_t *app;
        int appid;
        cpi_u16_t instance_id;
        cpi_u16_t service_id;
        cpi_u32_t major;
        cpi_u32_t minor;
        cpi_u16_t eid;
        cpi_u16_t gid;
        int code;
        cpi_event_info_t *info;
        cpi_message_t *m;
    };

    struct cpi_app_message_list_s
    {
        cpi_message_t *m;
        cpi_u64_t ts;
        cpi_u64_t retry;
        struct cpi_app_message_list_s *prev;
        struct cpi_app_message_list_s *next;
    };

    /*
 * internal functions
 */
    cpi_endpoint_net_ops_t *cpi_app_ep_get_listen_ops(void);
    cpi_endpoint_net_ops_t *cpi_app_ep_get_connect_ops(void);
    int cpi_app_connect_ep_cb(cpi_endpoint_t *ep, cpi_endpoint_cb_event_t event);
    int cpi_app_ep_iocb(nai_iobase_t *base, int events);
    int cpi_app_ep_connect_peerid_send(cpi_endpoint_t *ep);
    int cpi_app_find_svc_register(
        cpi_app_t *app, int appid, cpi_u16_t svc, cpi_u16_t inst, cpi_u32_t major, cpi_u32_t minor);
    void cpi_app_find_svc_unregister(
        cpi_app_t *app, int appid, cpi_u16_t svc, cpi_u16_t inst, cpi_u32_t major, cpi_u32_t minor);
    int cpi_app_find_svc_notice(cpi_app_t *app,
                                int appid,
                                cpi_u16_t svc,
                                cpi_u16_t inst,
                                cpi_u32_t major,
                                cpi_u32_t minor,
                                int status,
                                cpi_endpoint_addr_t *addr,
                                cpi_s8ptr_t shm_path,
                                cpi_u32_t seg_size,
                                cpi_u32_t seg_count);
    cpi_endpoint_t *cpi_app_connect_service(void *service);

/*
 * external macros and functions
 */
#define cpi_app_is_quit(papp)                  ((papp)->quit_flag)
#define cpi_app_get_svc_addr(papp)             ((papp)->svc_addr)
#define cpi_app_get_conf(papp)                 ((papp)->conf)
#define cpi_app_get_sent_cb(papp)              ((papp)->sent_callback)
#define cpi_app_set_sent_cb(papp, cb)          ((papp)->sent_callback = (cpi_app_sent_cb_t)(cb))
#define cpi_app_get_msg_cb(papp)               ((papp)->message_callback)
#define cpi_app_set_msg_cb(papp, cb)           ((papp)->message_callback = (cpi_app_msg_cb_t)(cb))
#define cpi_app_get_shm_comm(papp)             (&(papp)->shm_comm)
#define cpi_app_is_discovery(papp)             ((papp)->type == app_type_dis)
#define cpi_app_set_avail_cb(papp, cb)         ((papp)->avail_callback = (cpi_app_avail_cb_t)(cb))
#define cpi_app_get_avail_cb(papp)             ((papp)->avail_callback)
#define cpi_app_get_id(papp)                   ((papp)->id)
#define cpi_app_get_sub_cb(papp)               ((papp)->subscribe_callback)
#define cpi_app_set_sub_cb(papp, cb)           ((papp)->subscribe_callback = (cpi_app_sub_cb_t)(cb))
#define cpi_app_get_suback_cb(papp)            ((papp)->subscribe_ack_callback)
#define cpi_app_set_suback_cb(papp, cb)        ((papp)->subscribe_ack_callback = (cpi_app_suback_cb_t)(cb))
#define cpi_app_evloop_get(papp)               ((papp)->evloop)
#define cpi_app_evloop_set(papp, ev)           ((papp)->evloop = (ev))
#define cpi_app_spin_lock(papp)                cpi_spin_lock(&(papp)->lock)
#define cpi_app_spin_unlock(papp)              cpi_spin_unlock(&(papp)->lock)
#define cpi_app_get_conf(papp)                 ((papp)->conf)
#define cpi_app_set_data(papp, d)              ((papp)->data = (d))
#define cpi_app_get_data(papp)                 ((papp)->data)
#define cpi_app_set_quit_cb(papp, cb)          ((papp)->quit_cb = (cb))
#define cpi_app_get_evnode(papp)               (&(papp)->node)
#define cpi_app_get_execq(papp)                &(papp)->head, &(papp)->tail
#define cpi_app_get_dis_evnode(papp)           (&(papp)->dis_node)
#define cpi_app_iam_offer_cb_set(papp, cb)     ((papp)->iam_offer_callback = (cb))
#define cpi_app_iam_offer_cb_get(papp)         ((papp)->iam_offer_callback)
#define cpi_app_iam_find_cb_set(papp, cb)      ((papp)->iam_find_callback = (cb))
#define cpi_app_iam_find_cb_get(papp)          ((papp)->iam_find_callback)
#define cpi_app_iam_subscribe_cb_set(papp, cb) ((papp)->iam_subscribe_callback = (cb))
#define cpi_app_iam_subscribe_cb_get(papp)     ((papp)->iam_subscribe_callback)
#define cpi_app_iam_message_cb_set(papp, cb)   ((papp)->iam_message_callback = (cb))
#define cpi_app_iam_message_cb_get(papp)       ((papp)->iam_message_callback)

    NPC_EXTERN
    cpi_app_t *cpi_app_new(cpi_conf_t *conf);
    NPC_EXTERN
    int cpi_app_init(cpi_app_t *app, cpi_conf_t *conf);
    NPC_EXTERN
    int cpi_app_open(cpi_app_t *app);
    NPC_EXTERN
    void cpi_app_destroy(cpi_app_t *app);
    NPC_EXTERN
    void cpi_app_free(cpi_app_t *app);
    NPC_EXTERN
    int cpi_app_offer_service(cpi_app_t *p, cpi_u16_t svc, cpi_u16_t inst, cpi_u32_t major, cpi_u32_t minor);
    NPC_EXTERN
    int cpi_app_stop_service(cpi_app_t *app, cpi_u16_t svc, cpi_u16_t inst, cpi_u32_t major, cpi_u32_t minor);
    NPC_EXTERN
    int cpi_app_request_service(cpi_app_t *app, cpi_u16_t svc, cpi_u16_t inst, cpi_u32_t major, cpi_u32_t minor);
    NPC_EXTERN
    int cpi_app_release_service(cpi_app_t *app, cpi_u16_t svc, cpi_u16_t inst, cpi_u32_t major, cpi_u32_t minor);
    NPC_EXTERN
    int cpi_app_offer_event(cpi_app_t *app, cpi_u16_t svc, cpi_u16_t inst, cpi_u16_t evid, cpi_event_info_t *info);
    NPC_EXTERN
    int cpi_app_stop_event(cpi_app_t *app, cpi_u16_t svc, cpi_u16_t inst, cpi_u16_t evid);
    NPC_EXTERN
    int cpi_app_request_event(cpi_app_t *app, cpi_u16_t svc, cpi_u16_t inst, cpi_u16_t eid, cpi_event_info_t *info);
    NPC_EXTERN
    int cpi_app_release_event(cpi_app_t *app, cpi_u16_t svc, cpi_u16_t inst, cpi_u16_t eid);
    NPC_EXTERN
    int cpi_app_subscribe(cpi_app_t *app, cpi_u16_t svc, cpi_u16_t inst, cpi_u16_t gid, cpi_u32_t major, cpi_u16_t eid);
    NPC_EXTERN
    int cpi_app_subscribe_ack(
        cpi_app_t *app, int code, int appid, cpi_u16_t svc, cpi_u16_t inst, cpi_u16_t gid, cpi_u16_t eid);
    NPC_EXTERN
    int cpi_app_unsubscribe(
        cpi_app_t *app, cpi_u16_t svc, cpi_u16_t inst, cpi_u16_t gid, cpi_u32_t major, cpi_u16_t eid);
    NPC_EXTERN
    cpi_message_t *cpi_app_create_message(cpi_app_t *app, cpi_u16_t svc, cpi_u16_t inst);
    NPC_EXTERN
    cpi_message_t *cpi_app_create_request(cpi_app_t *app, cpi_u16_t svc, cpi_u16_t inst, cpi_method_t meth);
    NPC_EXTERN
    cpi_message_t *cpi_app_create_response(cpi_app_t *app, cpi_message_t *r, cpi_u8_t sult);
    NPC_EXTERN
    cpi_message_t *cpi_app_create_notification(cpi_app_t *app, cpi_u16_t svc, cpi_u16_t inst, cpi_u16_t event);
    NPC_EXTERN
    void cpi_app_init_notification(cpi_message_t *m, cpi_app_t *app, cpi_u16_t svc, cpi_u16_t inst, cpi_u16_t event);
    NPC_EXTERN
    int cpi_app_ptr_in_shm(cpi_app_t *app, cpi_u16_t svc, cpi_u16_t inst, const void *ptr);
    NPC_EXTERN
    int cpi_app_send(cpi_app_t *app, cpi_message_t *m);
    NPC_EXTERN
    void cpi_app_set_ud(cpi_app_t *app, void *data);
    NPC_EXTERN
    void cpi_app_set_cb_available(cpi_app_t *app, cpi_app_avail_cb_t cb);
    NPC_EXTERN
    void cpi_app_set_cb_message(cpi_app_t *app, cpi_app_msg_cb_t cb);
    NPC_EXTERN
    void cpi_app_set_cb_sent(cpi_app_t *app, cpi_app_sent_cb_t cb);
    NPC_EXTERN
    void cpi_app_set_cb_subscribe(cpi_app_t *app, cpi_app_sub_cb_t cb);
    NPC_EXTERN
    void cpi_app_set_cb_subscribe_ack(cpi_app_t *app, cpi_app_suback_cb_t cb);
    NPC_EXTERN
    void cpi_app_set_evloop(cpi_app_t *app, void *ev);
    NPC_EXTERN
    void cpi_app_set_cb_iam_offer(cpi_app_t *app, cpi_app_service_cb_t cb);
    NPC_EXTERN
    void cpi_app_set_cb_iam_find(cpi_app_t *app, cpi_app_service_cb_t cb);
    NPC_EXTERN
    void cpi_app_set_cb_iam_subscribe(cpi_app_t *app, cpi_app_sub_cb_t cb);
    NPC_EXTERN
    void cpi_app_set_cb_iam_message(cpi_app_t *app, cpi_app_msg_cb_t cb);
    NPC_EXTERN
    void *cpi_app_shm_alloc(cpi_app_t *app, cpi_u16_t svc, cpi_u16_t inst, size_t size);
    NPC_EXTERN
    void cpi_app_shm_free(void *ptr, size_t val_size);

#ifdef __cplusplus
}
#endif

#endif
