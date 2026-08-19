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
/// @file       cpi.h
/// @brief
/// @details
/// @date       2023-02-10
/// @author     Zan Shigang
/// @version    1.2.0
///
/// ================================================================

#ifndef __CPI_H
#define __CPI_H

#include "nai/os/nai_socket.h"
#include "npc/cpi/cpi_types.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    typedef struct cpi_app_s cpi_app_t;
    typedef struct cpi_rbtree_s cpi_conf_t;
    typedef struct cpi_conf_item_s cpi_conf_item_t;
    typedef struct cpi_string_s cpi_string_t;
    typedef struct cpi_event_info_s cpi_event_info_t;
    typedef struct cpi_string_s cpi_string_t;

    typedef enum cpi_conf_item_type_e
    {
        cpi_conf_type_string,
        cpi_conf_type_num,
        cpi_conf_type_true,
        cpi_conf_type_false,
        cpi_conf_type_null
    } cpi_conf_item_type_t;
    struct cpi_event_info_s
    {
        cpi_s32_t type;
        cpi_s32_t reliability;
        cpi_s32_t ngroup;
        cpi_u16_t *groups;
    };
    typedef void (*cpi_app_cred_init_cb_t)(npc_cred_t *, void *);
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

    NPC_EXTERN
    void cpi_app_set_cb_iam_subscribe(cpi_app_t *app, cpi_app_sub_cb_t cb);
    NPC_EXTERN
    void cpi_app_set_cb_iam_message(cpi_app_t *app, cpi_app_msg_cb_t cb);
    NPC_EXTERN
    void cpi_app_set_cb_iam_offer(cpi_app_t *app, cpi_app_service_cb_t cb);
    NPC_EXTERN
    void cpi_app_set_cb_iam_find(cpi_app_t *app, cpi_app_service_cb_t cb);
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
    int cpi_conf_update(cpi_conf_t *conf, cpi_conf_item_type_t type, cpi_string_t *key, void *val);
    NPC_EXTERN
    cpi_conf_item_t *cpi_conf_search(cpi_conf_t *conf, cpi_string_t *key);
    NPC_EXTERN
    cpi_conf_t *cpi_conf_init(void);
    NPC_EXTERN
    void cpi_conf_close(cpi_conf_t *cf);
    NPC_EXTERN
    char *cpi_error(int err, void *buf, cpi_size_t len);
    NPC_EXTERN
    void *cpi_app_shm_alloc(cpi_app_t *app, cpi_u16_t svc, cpi_u16_t inst, size_t size);
    NPC_EXTERN
    void cpi_app_shm_free(void *ptr, size_t val_size);

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
