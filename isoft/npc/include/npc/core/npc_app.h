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
/// @file       npc_app.h
/// @brief
/// @details
/// @date       2022-08-29
/// @author     xn
/// @version    1.2.0
///
/// ================================================================

#ifndef _APP_H_NPC
#define _APP_H_NPC

#pragma once

#include "npc_types.h"
#include "npc_const.h"
#include "nai/os/nai_thread.h"
#include "nai/runtime/nai_rbtree.h"
#include "npc/cpi/cpi.h"
#include "npc/cpi/cpi_string.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#ifndef _NPC_TYPEDEF_APP_T
    #define _NPC_TYPEDEF_APP_T
    typedef cpi_app_t npc_app_t;
#endif
#ifndef _NAI_TYPEDEF_ARRAY_T
    #define _NAI_TYPEDEF_ARRAY_T
    typedef struct nai_array_s nai_array_t;
#endif
#ifndef _NPC_TYPEDEF_MESSAGE_T
    #define _NPC_TYPEDEF_MESSAGE_T
    typedef struct npc_message_s npc_message_t;
#endif
#ifndef _NPC_TYPEDEF_EVENT_INFO_T
    #define _NPC_TYPEDEF_EVENT_INFO_T
    typedef cpi_event_info_t npc_event_info_t;
#endif

    typedef cpi_conf_t npc_conf_t;

    typedef cpi_conf_item_type_t npc_conf_item_type_t;

    typedef cpi_string_t npc_string_t;

    typedef cpi_app_msg_cb_t npc_app_message_f;

    typedef cpi_app_sent_cb_t npc_app_sent_f;

    typedef cpi_app_sub_cb_t npc_app_subscribe_f;

    typedef cpi_app_suback_cb_t npc_app_subscribe_ack_f;

    typedef cpi_app_avail_cb_t npc_app_available_f;

    typedef cpi_app_service_cb_t npc_app_service_f;

    typedef cpi_app_cred_init_cb_t npc_app_cred_f;

    typedef struct npc_service_info_s
    {
        npc_serv_t serv;
        npc_inst_t inst;
        npc_version_t major;
        npc_versmin_t minor;
    } npc_service_info_t;
#define npc_app_new cpi_app_new

#define npc_app_init cpi_app_init

#define npc_app_open cpi_app_open

#define npc_app_close cpi_app_destroy

#define npc_app_free cpi_app_free

#define npc_app_send cpi_app_send

#define npc_app_offer_service cpi_app_offer_service

#define npc_app_stop_service cpi_app_stop_service

#define npc_app_offer_event cpi_app_offer_event

#define npc_app_stop_event cpi_app_stop_event

#define npc_app_request_service cpi_app_request_service

#define npc_app_release_service cpi_app_release_service

#define npc_app_request_event cpi_app_request_event

#define npc_app_release_event cpi_app_release_event

#define npc_app_subscribe cpi_app_subscribe

#define npc_app_unsubscribe cpi_app_unsubscribe

#define npc_app_subscribe_ack cpi_app_subscribe_ack

#define npc_app_create_message cpi_app_create_message

#define npc_app_create_request cpi_app_create_request

#define npc_app_create_response cpi_app_create_response

#define npc_app_create_notification cpi_app_create_notification

#define npc_app_init_notification cpi_app_init_notification

#define npc_app_ptr_in_shm cpi_app_ptr_in_shm

#define npc_app_get_conf cpi_app_get_conf

#define npc_conf_init cpi_conf_init

#define npc_conf_close cpi_conf_close

#define npc_conf_update cpi_conf_update

#define npc_error cpi_error

#define npc_app_set_evloop cpi_app_set_evloop

#define npc_string cpi_string

#define npc_string_set cpi_string_set

#define npc_string_nset cpi_string_nset

#define npc_string_ref cpi_string_ref

#define npc_string_free cpi_string_free

#define npc_app_set_cb_offer cpi_app_set_cb_iam_offer

#define npc_app_set_cb_find cpi_app_set_cb_iam_find

#define npc_app_set_ud cpi_app_set_ud

#define npc_app_set_cb_available cpi_app_set_cb_available

#define npc_app_set_cb_message cpi_app_set_cb_message

#define npc_app_set_cb_sent cpi_app_set_cb_sent

#define npc_app_set_cb_subscribe cpi_app_set_cb_subscribe

#define npc_app_set_cb_subscribe_ack cpi_app_set_cb_subscribe_ack

#define npc_app_set_cb_iam_subscribe cpi_app_set_cb_iam_subscribe

#define npc_app_set_cb_iam_message cpi_app_set_cb_iam_message

#define npc_app_shm_alloc cpi_app_shm_alloc

#define npc_app_shm_free cpi_app_shm_free

    //////////////////////////////////////////////////////////////////////////////
    // app dispatch

#ifndef _NPC_TYPEDEF_APPCB_T
    #define _NPC_TYPEDEF_APPCB_T
    typedef struct npc_appcb_s npc_appcb_t;
#endif

    /**
 * the structure of the callback dispatch of the application
 */
    struct npc_appcb_s
    {
        npc_app_t* app;     /**< pointer to the application */
        nai_spin_t lock;    /**< the spin lock */
        nai_rbtree_t servs; /**< the serivce map */
    };

    /**
 * initial the callback dispatch
 * @param   p       pointer to the callback dispatch
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    no error should occur during initialization, and return value is 
 *          allowed not to be processed.
 */
    NPC_EXTERN
    int npc_appcb_init(npc_appcb_t* p);

    /**
 * open the callback dispatch
 * @param   p       pointer to the callback dispatch
 * @param   app     pointer to the application
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NPC_EXTERN
    int npc_appcb_open(npc_appcb_t* p, npc_app_t* app);

    /**
 * close the callback dispatch
 * @param   p       pointer to the callback dispatch
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NPC_EXTERN
    int npc_appcb_close(npc_appcb_t* p);

    /**
 * get the user data bound with the service
 * @param   p       pointer to the callback dispatch
 * @param   serv    the service id
 * @param   inst    the instance id
 * @retval  !=0     the address of the user data
 * @retval  0       an error occurred, see #nai_errno
 */
    NPC_EXTERN
    void* npc_appcb_get_ud(npc_appcb_t* p, npc_serv_t serv, npc_inst_t inst);

    /**
 * set the user data bound with the service
 * @param   p       pointer to the callback dispatch
 * @param   serv    the service id
 * @param   inst    the instance id
 * @param   ud      pointer to the user data
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NPC_EXTERN
    int npc_appcb_set_ud(npc_appcb_t* p, npc_serv_t serv, npc_inst_t inst, void* ud);

    /**
 * set the callback for the service changed event
 * @param   p       pointer to the callback dispatch
 * @param   serv    the service id
 * @param   inst    the instance id
 * @param   cb      pointer to the callback
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NPC_EXTERN
    int npc_appcb_set_available(npc_appcb_t* p, npc_serv_t serv, npc_inst_t inst, npc_app_available_f cb);

    /**
 * set the callback for subscribe event
 * @param   p       pointer to the callback dispatch
 * @param   serv    the service id
 * @param   inst    the instance id
 * @param   cb      pointer to the callback
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NPC_EXTERN
    int npc_appcb_set_subscribe(npc_appcb_t* p, npc_serv_t serv, npc_inst_t inst, npc_app_subscribe_f cb);

    /**
 * set the callback for subscribe ack event
 * @param   p       pointer to the callback dispatch
 * @param   serv    the service id
 * @param   inst    the instance id
 * @param   cb      pointer to the callback
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NPC_EXTERN
    int npc_appcb_set_subscribe_ack(npc_appcb_t* p, npc_serv_t serv, npc_inst_t inst, npc_app_subscribe_ack_f cb);

/**
 * set the callback for all sent message
 * @param   p       pointer to the callback dispatch
 * @param   s       the service id
 * @param   i       the instance id
 * @param   cb      pointer to the callback
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
#define npc_appcb_set_sent_any(p, s, i, cb) npc_appcb_set_sent((p), (s), (i), NPC_METHOD_ANY, (cb), 0)

/**
 * set the callback for all incoming message
 * @param   p       pointer to the callback dispatch
 * @param   s       the service id
 * @param   i       the instance id
 * @param   cb      pointer to the callback
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
#define npc_appcb_set_message_any(p, s, i, cb) npc_appcb_set_message((p), (s), (i), NPC_METHOD_ANY, (cb), 0)

    /**
 * set the callback for the specical sent message
 * @param   p       pointer to the callback dispatch
 * @param   serv    the service id
 * @param   inst    the instance id
 * @param   meth    the method id
 * @param   cb      pointer to the callback
 * @param   ud      pointer to the user data
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NPC_EXTERN
    int npc_appcb_set_sent(
        npc_appcb_t* p, npc_serv_t serv, npc_inst_t inst, npc_method_t meth, npc_app_sent_f cb, void* ud);

    /**
 * set the callback for the specical incoming message
 * @param   p       pointer to the callback dispatch
 * @param   serv    the service id
 * @param   inst    the instance id
 * @param   meth    the method id
 * @param   cb      pointer to the callback
 * @param   ud      pointer to the user data
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NPC_EXTERN
    int npc_appcb_set_message(
        npc_appcb_t* p, npc_serv_t serv, npc_inst_t inst, npc_method_t meth, npc_app_message_f cb, void* ud);

    /**
 * test the service is available
 * @param   p       pointer to the callback dispatch
 * @param   serv    the service id
 * @param   inst    the instance id
 * @return  if it is available, return 1, otherwise return 0
 */
    NPC_EXTERN
    int npc_appcb_is_available(npc_appcb_t* p, npc_serv_t serv, npc_inst_t inst);

    /**
 * get all available services
 * @param   p       pointer to the callback dispatch
 * @param   serv    the service id
 * @param   inst    the instance id
 * @param   out     pointer to the output array
 * @retval  >=0     the number of the services on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NPC_EXTERN
    int npc_appcb_get_availables(npc_appcb_t* p, npc_serv_t serv, npc_inst_t inst, nai_array_t* out);

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
