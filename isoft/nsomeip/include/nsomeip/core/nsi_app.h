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
/// @file       nsi_app.h
/// @brief
/// @details
/// @date       2021-05-16
/// @author     xn
/// @version    1.2.0
///
/// ================================================================

#ifndef _APP_H_NSI
#define _APP_H_NSI

#pragma once

#include "nsi_types.h"
#include "nsi_const.h"
#include "nai/os/nai_thread.h"
#include "nai/runtime/nai_rbtree.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#ifndef _NSI_TYPEDEF_APP_T
    #define _NSI_TYPEDEF_APP_T
    typedef struct nsi_app_s nsi_app_t;
#endif
#ifndef _NAI_TYPEDEF_ARRAY_T
    #define _NAI_TYPEDEF_ARRAY_T
    typedef struct nai_array_s nai_array_t;
#endif
#ifndef _NSI_TYPEDEF_CLIENT_T
    #define _NSI_TYPEDEF_CLIENT_T
    typedef struct nsi_client_s nsi_client_t;
#endif
#ifndef _NSI_TYPEDEF_CRED_T
    #define _NSI_TYPEDEF_CRED_T
    typedef struct nsi_cred_s nsi_cred_t;
#endif
#ifndef _NSI_TYPEDEF_MESSAGE_T
    #define _NSI_TYPEDEF_MESSAGE_T
    typedef struct nsi_message_s nsi_message_t;
#endif
#ifndef _NSI_TYPEDEF_ROUTING_T
    #define _NSI_TYPEDEF_ROUTING_T
    typedef struct nsi_routing_s nsi_routing_t;
#endif
#ifndef _NSI_TYPEDEF_EVENT_INFO_T
    #define _NSI_TYPEDEF_EVENT_INFO_T
    typedef struct nsi_event_info_s nsi_event_info_t;
#endif

    /**
 * the callback for the incoming message
 * @param   a       pointer to the application
 * @param   ud      pointer to the user data
 * @param   m       pointer to the incoming message
 * @return  >=0     on success
 * @return  <0      on fails
 */
    typedef int (*nsi_app_message_f)(nsi_app_t* a, void* ud, nsi_message_t* m);

    /**
 * the callback for the sent message
 * @param   a       pointer to the application
 * @param   ud      pointer to the user data
 * @param   m       pointer to the incoming message
 * @param   errcode the error code during sending
 * @return  >=0     on success
 * @return  <0      on fails
 */
    typedef int (*nsi_app_sent_f)(nsi_app_t* a, void* ud, nsi_message_t* m, int errcode);

    /**
 * the callback for subscribe event
 * @param   a       pointer to the application
 * @param   ud      pointer to the user data
 * @param   cid     the id of subscriber
 * @param   serv    the subscirbed service
 * @param   inst    the subscribed instance
 * @param   gid     the subscribed group
 * @param   eid     the subscribed event
 * @param   cred    pointer to the cred
 * @return  >=0     on success
 * @return  <0      on fails
 */
    typedef int (*nsi_app_subscribe_f)(nsi_app_t* a,
                                       void* ud,
                                       nsi_cid_t cid,
                                       nsi_serv_t serv,
                                       nsi_inst_t inst,
                                       nsi_gid_t gid,
                                       nsi_eid_t eid,
                                       const nsi_cred_t* cred);

    /**
 * the callback for subscribe ack event
 * @param   a       pointer to the application
 * @param   ud      pointer to the user data
 * @param   rcode   the reuslt code of subscribe
 * @param   serv    the subscirbed service id
 * @param   inst    the subscribed instance id
 * @param   gid     the subscribed group id
 * @param   eid     the subscribed event id
 * @return  >=0     on success
 * @return  <0      on fails
 */
    typedef int (*nsi_app_subscribe_ack_f)(
        nsi_app_t* a, void* ud, int rcode, nsi_serv_t serv, nsi_inst_t inst, nsi_gid_t gid, nsi_eid_t eid);

    /**
 * the callback for the service change event
 * @param   a       pointer to the application
 * @param   ud      pointer to the user data
 * @param   avail   new stat of the service
 * @param   serv    the serivce id
 * @param   inst    the instance id
 * @param   major   the major version
 * @param   minor   the minor version
 * @param   cred    pointer to the cred of notified client
 * @return  >=0     on success
 * @return  <0      on fails
 */
    typedef int (*nsi_app_available_f)(nsi_app_t* a,
                                       void* ud,
                                       int avail,
                                       nsi_serv_t serv,
                                       nsi_inst_t inst,
                                       nsi_version_t major,
                                       nsi_versmin_t minor,
                                       const nsi_cred_t* cred);

    /**
 * the callback for hook find or offer
 * @param   a       pointer to the application
 * @param   ud      pointer to the user data
 * @param   serv    the subscirbed service id
 * @param   inst    the subscribed instance id
 * @param   major   the major version
 * @param   minor   the minor version
 * @param   cred    pointer to the cred
 * @return  >=0     on success
 * @return  <0      on fails
 */
    typedef int (*nsi_app_service_f)(nsi_app_t* a,
                                     void* ud,
                                     nsi_serv_t serv,
                                     nsi_inst_t inst,
                                     nsi_version_t major,
                                     nsi_versmin_t minor,
                                     const nsi_cred_t* cred);

    /**
 * the executor entry
 * @param   a       pointer to the application
 * @param   ud      pointer to the user data
 * @return  >=0     on success
 * @return  <0      on fails
 */
    typedef int (*nsi_app_execute_f)(nsi_app_t* a, void* ud);

    /**
 * the structure of the application
 */
    struct nsi_app_s
    {
        uint32_t uid;         /**< unused */
        nsi_cid_t cid;        /**< the client id */
        nsi_client_t* client; /**< pointer to the client */
        nsi_routing_t* rt;    /**< pointer to the routing */

        void* ud;                      /**< pointer to the user data */
        nsi_app_available_f availcb;   /**< the callback for the service event */
        nsi_app_sent_f sentcb;         /**< the callback for the sent message */
        nsi_app_message_f msgcb;       /**< the callback for the incoming message */
        nsi_app_subscribe_f subcb;     /**< the callback for subscribe event */
        nsi_app_subscribe_ack_f ackcb; /**< the callback for subscribe ack event */
        nsi_app_service_f findcb;      /**< the callback for hook find */
        nsi_app_service_f offercb;     /**< the callback for hook offer */
    };

/**
 * set user data to the application
 * @param   a       pointer to the application
 * @param   u       pointer to the user data
 * @return  void
 */
#define nsi_app_set_ud(a, u) (a)->ud = (u);

/**
 * set the callback for the service change event
 * @param   a       pointer to the application
 * @param   cb      pointer to the callback
 * @return  void
 */
#define nsi_app_set_cb_available(a, cb) (a)->availcb = (cb);

/**
 * set the callback for the incoming message
 * @param   a       pointer to the application
 * @param   cb      pointer to the callback
 * @return  void
 */
#define nsi_app_set_cb_message(a, cb) (a)->msgcb = (cb);

/**
 * set the callback for the sent message
 * @param   a       pointer to the application
 * @param   cb      pointer to the callback
 * @return  void
 */
#define nsi_app_set_cb_sent(a, cb) (a)->sentcb = (cb);

/**
 * set the callback for subscribe event
 * @param   a       pointer to the application
 * @param   cb      pointer to the callback
 * @return  void
 */
#define nsi_app_set_cb_subscribe(a, cb) (a)->subcb = (cb);

/**
 * set the callback for subscribe ack event
 * @param   a       pointer to the application
 * @param   cb      pointer to the callback
 * @return  void
 */
#define nsi_app_set_cb_subscribe_ack(a, cb) (a)->ackcb = (cb);

/**
 * set the callback for hook find
 * @param   a       pointer to the application
 * @param   cb      pointer to the callback
 * @return  void
 */
#define nsi_app_set_cb_find(a, cb) (a)->findcb = (cb);

/**
 * set the callback for hook offer
 * @param   a       pointer to the application
 * @param   cb      pointer to the callback
 * @return  void
 */
#define nsi_app_set_cb_offer(a, cb) (a)->offercb = (cb);

    /**
 * initial the application
 * @param   p       pointer to the application
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    no error should occur during initialization, and return value is 
 *          allowed not to be processed.
 */
    NSI_EXTERN
    int nsi_app_init(nsi_app_t* p);

    /**
 * open the application
 * @param   p       pointer to the application
 * @param   rt      pointer to the routing which is opened
 * @param   cid     the client id of the application
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NSI_EXTERN
    int nsi_app_open(nsi_app_t* p, nsi_routing_t* rt, nsi_cid_t cid);

    /**
 * open the application
 * @param   p       pointer to the application
 * @param   rt      pointer to the routing which is opened
 * @param   name    pointer to the name of the application
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NSI_EXTERN
    int nsi_app_open_name(nsi_app_t* p, nsi_routing_t* rt, const char* name);

    /**
 * close the application
 * @param   p       pointer to the application
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NSI_EXTERN
    int nsi_app_close(nsi_app_t* p);

    /**
 * send a message
 * @param   p       pointer to the application
 * @param   m       pointer to the message
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    this function will take over the ownership of the message when 
 *          it is sent successfully. at this time, the caller should no longer 
 *          manipulate the message including closing
 */
    NSI_EXTERN
    int nsi_app_send(nsi_app_t* p, nsi_message_t* m);

    /**
 * synchronous execution in the background thread of the application
 * @param   p       pointer to the application
 * @param   cb      pointer to the executor
 * @param   ud      pointer to the user data
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    this function will be blocked until the process execution is 
 *          complete.
 */
    NSI_EXTERN
    int nsi_app_execute(nsi_app_t* p, nsi_app_execute_f cb, void* ud);

    /**
 * offer a service
 * @param   p       pointer to the application
 * @param   serv    the service id
 * @param   inst    the instance id
 * @param   major   the major version
 * @param   minor   the minor version
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NSI_EXTERN
    int nsi_app_offer_service(nsi_app_t* p, nsi_serv_t serv, nsi_inst_t inst, nsi_version_t major, nsi_versmin_t minor);

    /**
 * stop a offered service
 * @param   p       pointer to the application
 * @param   serv    the service id
 * @param   inst    the instance id
 * @param   major   the major version
 * @param   minor   the minor version
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NSI_EXTERN
    int nsi_app_stop_service(nsi_app_t* p, nsi_serv_t serv, nsi_inst_t inst, nsi_version_t major, nsi_versmin_t minor);

    /**
 * offer an event
 * @param   p       pointer to the application
 * @param   serv    the service id
 * @param   inst    the instance id
 * @param   event   the event id
 * @param   info    pointer to the event information
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NSI_EXTERN
    int nsi_app_offer_event(
        nsi_app_t* p, nsi_serv_t serv, nsi_inst_t inst, nsi_eid_t event, const nsi_event_info_t* info);

    /**
 * stop a offered event
 * @param   p       pointer to the application
 * @param   serv    the service id
 * @param   inst    the instance id
 * @param   event   the event id
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NSI_EXTERN
    int nsi_app_stop_event(nsi_app_t* p, nsi_serv_t serv, nsi_inst_t inst, nsi_eid_t event);

    /**
 * request a service
 * @param   p       pointer to the application
 * @param   serv    the service id
 * @param   inst    the instance id
 * @param   major   the major version
 * @param   minor   the minor version
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    this function can be used to find services by setting inst or 
 *          major or minor to any value
 */
    NSI_EXTERN
    int nsi_app_request_service(
        nsi_app_t* p, nsi_serv_t serv, nsi_inst_t inst, nsi_version_t major, nsi_versmin_t minor);

    /**
 * release a requested service
 * @param   p       pointer to the application
 * @param   serv    the service id
 * @param   inst    the instance id
 * @param   major   the major version
 * @param   minor   the minor version
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NSI_EXTERN
    int nsi_app_release_service(
        nsi_app_t* p, nsi_serv_t serv, nsi_inst_t inst, nsi_version_t major, nsi_versmin_t minor);

    /**
 * request an event
 * @param   p       pointer to the application
 * @param   serv    the service id
 * @param   inst    the instance id
 * @param   event   the event id
 * @param   info    pointer to the event information
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NSI_EXTERN
    int nsi_app_request_event(
        nsi_app_t* p, nsi_serv_t serv, nsi_inst_t inst, nsi_eid_t event, const nsi_event_info_t* info);

    /**
 * release a requested event
 * @param   p       pointer to the application
 * @param   serv    the service id
 * @param   inst    the instance id
 * @param   event   the event id
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NSI_EXTERN
    int nsi_app_release_event(nsi_app_t* p, nsi_serv_t serv, nsi_inst_t inst, nsi_eid_t event);

    /**
 * subscribe an event group
 * @param   p       pointer to the application
 * @param   serv    the service id
 * @param   inst    the instance id
 * @param   group   the event group id
 * @param   major   the major version
 * @param   event   the event id
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NSI_EXTERN
    int nsi_app_subscribe(
        nsi_app_t* p, nsi_serv_t serv, nsi_inst_t inst, nsi_gid_t group, nsi_version_t major, nsi_eid_t event);

    /**
 * unsubscribe an event group
 * @param   p       pointer to the application
 * @param   serv    the service id
 * @param   inst    the instance id
 * @param   group   the event group id
 * @param   major   the major version
 * @param   event   the event id
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NSI_EXTERN
    int nsi_app_unsubscribe(
        nsi_app_t* p, nsi_serv_t serv, nsi_inst_t inst, nsi_gid_t group, nsi_version_t major, nsi_eid_t event);

    /**
 * ack a subscribe
 * @param   p       pointer to the application
 * @param   sult    the result of subscribe, see @ref NSI_ERROR
 * @param   cid     the client id
 * @param   serv    the service id
 * @param   inst    the instance id
 * @param   group   the event group id
 * @param   event   the event id
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NSI_EXTERN
    int nsi_app_subscribe_ack(
        nsi_app_t* p, int sult, nsi_cid_t cid, nsi_serv_t serv, nsi_inst_t inst, nsi_gid_t group, nsi_eid_t event);

    /**
 * create a message for write
 * @param   p       pointer to the application
 * @retval  !=0     the address of the new message on success
 * @retval  0       an error occurred, see #nai_errno
 */
    NSI_EXTERN
    nsi_message_t* nsi_app_create_message(nsi_app_t* p);

    /**
 * create a request message
 * @param   p       pointer to the application
 * @param   serv    the service id
 * @param   inst    the instance id
 * @param   meth    the method id
 * @retval  !=0     the address of the new message on success
 * @retval  0       an error occurred, see #nai_errno
 */
    NSI_EXTERN
    nsi_message_t* nsi_app_create_request(nsi_app_t* p, nsi_serv_t serv, nsi_inst_t inst, nsi_method_t meth);

    /**
 * create a response message
 * @param   p       pointer to the application
 * @param   request pointer to the request message
 * @param   sult    the value of the result, see @ref NSI_ERROR
 * @retval  !=0     the address of the new message on success
 * @retval  0       an error occurred, see #nai_errno
 */
    NSI_EXTERN
    nsi_message_t* nsi_app_create_response(nsi_app_t* p, nsi_message_t* request, uint8_t sult);

    /**
 * create a notification message
 * @param   p       pointer to the application
 * @param   serv    the service id
 * @param   inst    the instance id
 * @param   event   the event id
 * @retval  !=0     the address of the new message on success
 * @retval  0       an error occurred, see #nai_errno
 */
    NSI_EXTERN
    nsi_message_t* nsi_app_create_notification(nsi_app_t* p, nsi_serv_t serv, nsi_inst_t inst, nsi_eid_t event);

    //////////////////////////////////////////////////////////////////////////////
    // app dispatch

#ifndef _NSI_TYPEDEF_APPCB_T
    #define _NSI_TYPEDEF_APPCB_T
    typedef struct nsi_appcb_s nsi_appcb_t;
#endif

    /**
 * the structure of the callback dispatch of the application
 */
    struct nsi_appcb_s
    {
        nsi_app_t* app;     /**< pointer to the application */
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
    NSI_EXTERN
    int nsi_appcb_init(nsi_appcb_t* p);

    /**
 * open the callback dispatch
 * @param   p       pointer to the callback dispatch
 * @param   app     pointer to the application
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NSI_EXTERN
    int nsi_appcb_open(nsi_appcb_t* p, nsi_app_t* app);

    /**
 * close the callback dispatch
 * @param   p       pointer to the callback dispatch
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NSI_EXTERN
    int nsi_appcb_close(nsi_appcb_t* p);

    /**
 * get the user data bound with the service
 * @param   p       pointer to the callback dispatch
 * @param   serv    the service id
 * @param   inst    the instance id
 * @retval  !=0     the address of the user data
 * @retval  0       an error occurred, see #nai_errno
 */
    NSI_EXTERN
    void* nsi_appcb_get_ud(nsi_appcb_t* p, nsi_serv_t serv, nsi_inst_t inst);

    /**
 * set the user data bound with the service
 * @param   p       pointer to the callback dispatch
 * @param   serv    the service id
 * @param   inst    the instance id
 * @param   ud      pointer to the user data
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NSI_EXTERN
    int nsi_appcb_set_ud(nsi_appcb_t* p, nsi_serv_t serv, nsi_inst_t inst, void* ud);

    /**
 * set the callback for the service changed event
 * @param   p       pointer to the callback dispatch
 * @param   serv    the service id
 * @param   inst    the instance id
 * @param   cb      pointer to the callback
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NSI_EXTERN
    int nsi_appcb_set_available(nsi_appcb_t* p, nsi_serv_t serv, nsi_inst_t inst, nsi_app_available_f cb);

    /**
 * set the callback for subscribe event
 * @param   p       pointer to the callback dispatch
 * @param   serv    the service id
 * @param   inst    the instance id
 * @param   cb      pointer to the callback
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NSI_EXTERN
    int nsi_appcb_set_subscribe(nsi_appcb_t* p, nsi_serv_t serv, nsi_inst_t inst, nsi_app_subscribe_f cb);

    /**
 * set the callback for subscribe ack event
 * @param   p       pointer to the callback dispatch
 * @param   serv    the service id
 * @param   inst    the instance id
 * @param   cb      pointer to the callback
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NSI_EXTERN
    int nsi_appcb_set_subscribe_ack(nsi_appcb_t* p, nsi_serv_t serv, nsi_inst_t inst, nsi_app_subscribe_ack_f cb);

/**
 * set the callback for all sent message
 * @param   p       pointer to the callback dispatch
 * @param   s       the service id
 * @param   i       the instance id
 * @param   cb      pointer to the callback
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
#define nsi_appcb_set_sent_any(p, s, i, cb) nsi_appcb_set_sent((p), (s), (i), NSI_METHOD_ANY, (cb), 0)

/**
 * set the callback for all incoming message
 * @param   p       pointer to the callback dispatch
 * @param   s       the service id
 * @param   i       the instance id
 * @param   cb      pointer to the callback
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
#define nsi_appcb_set_message_any(p, s, i, cb) nsi_appcb_set_message((p), (s), (i), NSI_METHOD_ANY, (cb), 0)

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
    NSI_EXTERN
    int nsi_appcb_set_sent(
        nsi_appcb_t* p, nsi_serv_t serv, nsi_inst_t inst, nsi_method_t meth, nsi_app_sent_f cb, void* ud);

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
    NSI_EXTERN
    int nsi_appcb_set_message(
        nsi_appcb_t* p, nsi_serv_t serv, nsi_inst_t inst, nsi_method_t meth, nsi_app_message_f cb, void* ud);

    /**
 * test the service is available
 * @param   p       pointer to the callback dispatch
 * @param   serv    the service id
 * @param   inst    the instance id
 * @return  if it is available, return 1, otherwise return 0
 */
    NSI_EXTERN
    int nsi_appcb_is_available(nsi_appcb_t* p, nsi_serv_t serv, nsi_inst_t inst);

    /**
 * get all available services
 * @param   p       pointer to the callback dispatch
 * @param   serv    the service id
 * @param   inst    the instance id
 * @param   out     pointer to the output array
 * @retval  >=0     the number of the services on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NSI_EXTERN
    int nsi_appcb_get_availables(nsi_appcb_t* p, nsi_serv_t serv, nsi_inst_t inst, nai_array_t* out);

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
