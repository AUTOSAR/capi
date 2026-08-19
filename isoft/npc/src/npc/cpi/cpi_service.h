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
/// @file       cpi_service.h
/// @brief
/// @details
/// @date       2022-11-11
/// @author     Zan Shigang
/// @version    1.2.0
///
/// ================================================================

#ifndef __CPI_SERVICE_H
#define __CPI_SERVICE_H


#include "nai/os/nai_socket.h"
#include "nai/io/nai_event.h"
#include "npc/shm/npc_comm.h"
#include "npc/shm/npc_port.h"
#include "npc/cpi/cpi_types.h"
#include "npc/cpi/cpi_rbtree.h"
#include "npc/cpi/cpi_event.h"
#include "npc/cpi/cpi_app.h"
#include "npc/cpi/cpi_const.h"
#include "npc/cpi/cpi_endpoint.h"
#include <arpa/inet.h>

#ifdef __cplusplus
extern "C"
{
#endif

    typedef enum cpi_service_role_e cpi_service_role_t;
    typedef enum cpi_service_status_e cpi_service_status_t;
    typedef struct cpi_service_s cpi_service_t;
    typedef struct cpi_service_peer_s cpi_service_peer_t;

    enum cpi_service_status_e
    {
        svc_offline = 0,
        svc_online  = 1
    };

    enum cpi_service_role_e
    {
        svc_provider,
        svc_consumer,
        svc_holder
    };

    struct cpi_service_peer_s
    {
        npc_port_t *read;
        npc_port_t *write;
        int appid;
        int in_chain;
        struct cpi_service_peer_s *prev;
        struct cpi_service_peer_s *next;
    };

    struct cpi_service_s
    {
        cpi_app_t *app;
        int appid;
        cpi_endpoint_addr_t svc_addr;
        cpi_u16_t instance_id;
        cpi_u16_t service_id;
        cpi_u32_t major;
        cpi_u32_t minor;
        cpi_u32_t seg_size;
        cpi_u32_t seg_count;
        cpi_s8_t shm_path[CPI_SHM_PATH_LEN];
        cpi_service_role_t role;
        cpi_service_status_t status;
        nai_evnode_t node;
        cpi_rbtree_t *groups;
        cpi_rbtree_t *events;
        cpi_rbtree_t *subscriptions;
        cpi_rbtree_t *peers; /* only for service providor */
        npc_comif_t *shm_comif;
        cpi_u16_t msg_queue_len;
        /* only for service consumer */
        npc_port_t *read;
        npc_port_t *write;
        npc_mref_t read_addr;
        npc_mref_t write_addr;
        /* flags */
        cpi_u32_t quit : 1;
        cpi_u32_t padding : 31;
    };

#define cpi_service_get_port(obj, type)     ((obj)->type)
#define cpi_service_get_port_ref(obj, type) ((obj)->type->mref)
#define cpi_service_get_comif(svc)          ((svc)->shm_comif)
#define cpi_service_set_comif(svc, cif)     ((svc)->shm_comif = (cif))
#define cpi_service_get_shm_path(svc)       ((svc)->shm_path)
#define cpi_service_get_seg_size(svc)       ((svc)->seg_size)
#define cpi_service_get_seg_count(svc)      ((svc)->seg_count)
#define cpi_service_set_status(svc, st)     ((svc)->status = (st))
#define cpi_service_get_status(svc)         ((svc)->status)
#define cpi_service_get_appid(svc)          ((svc)->appid)
#define cpi_service_get_role(svc)           ((svc)->role)
#define cpi_service_get_serviceid(svc)      ((svc)->service_id)
#define cpi_service_get_instanceid(svc)     ((svc)->instance_id)
#define cpi_service_get_major(svc)          ((svc)->major)
#define cpi_service_get_minor(svc)          ((svc)->minor)
#define cpi_service_get_app(svc)            ((svc)->app)
#define cpi_service_get_addr(svc)           ((svc)->svc_addr)
#define cpi_service_set_addr(svc, a)        ((svc)->svc_addr = (a))
#define cpi_service_set_ids(svc, aid, iid, sid, ma, mi)                                                                \
    ((svc)->appid = (aid), (svc)->instance_id = (iid), (svc)->service_id = (sid), (svc)->major = (ma),                 \
     (svc)->minor = (mi))
#define cpi_service_get_appid(svc) ((svc)->appid)
#define cpi_service_get_node(svc)  (&(svc)->node)
#define cpi_service_online(svc)    ((svc)->status = svc_online)
#define cpi_service_offline(svc)   ((svc)->status = svc_offline)
#define cpi_service_is_online(svc) ((svc)->status == svc_online)

    cpi_service_t *cpi_service_new(void *app,
                                   nai_evloop_t *evloop,
                                   int appid,
                                   cpi_u16_t instance_id,
                                   cpi_u16_t service_id,
                                   cpi_u32_t major,
                                   cpi_u32_t minor,
                                   cpi_service_role_t role,
                                   const char *shm_path,
                                   cpi_u32_t seg_size,
                                   cpi_u32_t seg_count,
                                   cpi_u16_t msg_queue_len);
    void cpi_service_free(cpi_service_t *svc);
    void cpi_service_consumer_port_free(cpi_service_t *s);
    int cpi_service_cmp(const cpi_service_t *s1, const cpi_service_t *s2);
    cpi_service_t *cpi_service_search(cpi_rbtree_t *services, cpi_u16_t svc, cpi_u16_t inst);
    int cpi_service_remove(cpi_rbtree_t *services, cpi_u16_t svc, cpi_u16_t inst);
    int cpi_service_event_remove(cpi_service_t *svc, cpi_u16_t eid);
    int cpi_service_event_add(cpi_service_t *svc, cpi_u16_t eid, cpi_event_info_t *info);
    int cpi_service_subscription_set(cpi_service_t *svc, int appid, cpi_u16_t gid, cpi_u16_t eid);
    int cpi_service_subscription_remove(cpi_service_t *svc, int appid, cpi_u16_t gid, cpi_u16_t eid);
    int cpi_service_auto_subscribe(cpi_service_t *svc, cpi_endpoint_t *ep);
    int cpi_service_remove_and_disconnect(
        cpi_app_t *app, cpi_u16_t svc, cpi_u16_t inst, cpi_u32_t major, cpi_u32_t minor);
    int cpi_service_peer_add(cpi_service_t *svc, int appid);
    void cpi_service_peer_remove(cpi_service_t *svc, int appid);
    int cpi_service_request_port(cpi_service_t *svc, cpi_endpoint_t *ep);
    cpi_service_peer_t *cpi_service_peer_search(cpi_service_t *svc, int appid);
    int cpi_service_port_attach(cpi_service_t *svc, npc_mref_t wr, npc_mref_t rd);
    int cpi_service_send_check(cpi_service_t *svc, cpi_message_t *m);
    int cpi_service_send(cpi_service_t *svc, cpi_message_t *m);
    void cpi_service_reregister_service(cpi_rbtree_t *services);
    int cpi_service_consumer_comif_create(cpi_service_t *svc);

#ifdef __cplusplus
}
#endif

#endif
