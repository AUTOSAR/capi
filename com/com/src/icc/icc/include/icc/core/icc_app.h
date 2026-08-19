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
/// @file       icc_app.h
/// @brief
/// @details
/// @date       2025-11-27
/// @author     Zan Shigang
/// @version    1.2.0
///
/// ================================================================

#ifndef _APP_H_ICC
#define _APP_H_ICC

#pragma once

#include "icc_const.h"
#include "icc_types.h"
#include "nai/os/nai_thread.h"
#include "nai/runtime/nai_rbtree.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#ifndef _ICC_TYPEDEF_APP_T
    #define _ICC_TYPEDEF_APP_T
    typedef struct icc_app_s icc_app_t;
#endif
#ifndef _ICC_TYPEDEF_CLIENT_T
    #define _ICC_TYPEDEF_CLIENT_T
    typedef struct icc_client_s icc_client_t;
#endif
#ifndef _ICC_TYPEDEF_CRED_T
    #define _ICC_TYPEDEF_CRED_T
    typedef struct icc_cred_s icc_cred_t;
#endif
#ifndef _ICC_TYPEDEF_ARRAY_T
    #define _ICC_TYPEDEF_ARRAY_T
    typedef struct nai_array_s nai_array_t;
#endif
#ifndef _ICC_TYPEDEF_MESSAGE_T
    #define _ICC_TYPEDEF_MESSAGE_T
    typedef struct icc_message_s icc_message_t;
#endif
#ifndef _ICC_TYPEDEF_ROUTING_T
    #define _ICC_TYPEDEF_ROUTING_T
    typedef struct icc_routing_s icc_routing_t;
#endif
#ifndef _ICC_TYPEDEF_EVENT_INFO_T
    #define _ICC_TYPEDEF_EVENT_INFO_T
    typedef struct icc_event_info_s icc_event_info_t;
#endif

    /**
 * the callback for the incoming message
 * @param   a       pointer to the application
 * @param   ud      pointer to the user data
 * @param   m       pointer to the incoming message
 * @return  >=0     on success
 * @return  <0      on fails
 */
    typedef int (*icc_app_message_f)(icc_app_t* a, void* ud, icc_message_t* m);

    /**
 * the callback for the sent message
 * @param   a       pointer to the application
 * @param   ud      pointer to the user data
 * @param   m       pointer to the incoming message
 * @param   errcode the error code during sending
 * @return  >=0     on success
 * @return  <0      on fails
 */
    typedef int (*icc_app_sent_f)(icc_app_t* a, void* ud, icc_message_t* m, int errcode);

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
    typedef int (*icc_app_subscribe_f)(icc_app_t* a,
                                       void* ud,
                                       icc_cid_t cid,
                                       icc_serv_t serv,
                                       icc_inst_t inst,
                                       icc_gid_t gid,
                                       icc_eid_t eid,
                                       const icc_cred_t* cred);

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
    typedef int (*icc_app_subscribe_ack_f)(
        icc_app_t* a, void* ud, int rcode, icc_serv_t serv, icc_inst_t inst, icc_gid_t gid, icc_eid_t eid);

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
    typedef int (*icc_app_available_f)(icc_app_t* a,
                                       void* ud,
                                       int avail,
                                       icc_serv_t serv,
                                       icc_inst_t inst,
                                       icc_version_t major,
                                       icc_versmin_t minor,
                                       const icc_cred_t* cred);

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
    typedef int (*icc_app_service_f)(icc_app_t* a,
                                     void* ud,
                                     icc_serv_t serv,
                                     icc_inst_t inst,
                                     icc_version_t major,
                                     icc_versmin_t minor,
                                     const icc_cred_t* cred);

    /**
 * the executor entry
 * @param   a       pointer to the application
 * @param   ud      pointer to the user data
 * @return  >=0     on success
 * @return  <0      on fails
 */
    typedef int (*icc_app_execute_f)(icc_app_t* a, void* ud);

    /**
 * the structure of the application
 */
    struct icc_app_s
    {
        uint32_t uid;         /**< unused */
        icc_cid_t cid;        /**< the client id */
        icc_client_t* client; /**< pointer to the client */
        icc_routing_t* rt;    /**< pointer to the routing */

        void* ud;                      /**< pointer to the user data */
        icc_app_available_f availcb;   /**< the callback for the service event */
        icc_app_sent_f sentcb;         /**< the callback for the sent message */
        icc_app_message_f msgcb;       /**< the callback for the incoming message */
        icc_app_subscribe_f subcb;     /**< the callback for subscribe event */
        icc_app_subscribe_ack_f ackcb; /**< the callback for subscribe ack event */
        icc_app_service_f findcb;      /**< the callback for hook find */
        icc_app_service_f offercb;     /**< the callback for hook offer */
    };

/**
 * set user data to the application
 * @param   a       pointer to the application
 * @param   u       pointer to the user data
 * @return  void
 */
#define icc_app_set_ud(a, u) (a)->ud = (u);

/**
 * set the callback for the service change event
 * @param   a       pointer to the application
 * @param   cb      pointer to the callback
 * @return  void
 */
#define icc_app_set_cb_available(a, cb) (a)->availcb = (cb);

/**
 * set the callback for the incoming message
 * @param   a       pointer to the application
 * @param   cb      pointer to the callback
 * @return  void
 */
#define icc_app_set_cb_message(a, cb) (a)->msgcb = (cb);

/**
 * set the callback for the sent message
 * @param   a       pointer to the application
 * @param   cb      pointer to the callback
 * @return  void
 */
#define icc_app_set_cb_sent(a, cb) (a)->sentcb = (cb);

/**
 * set the callback for subscribe event
 * @param   a       pointer to the application
 * @param   cb      pointer to the callback
 * @return  void
 */
#define icc_app_set_cb_subscribe(a, cb) (a)->subcb = (cb);

/**
 * set the callback for subscribe ack event
 * @param   a       pointer to the application
 * @param   cb      pointer to the callback
 * @return  void
 */
#define icc_app_set_cb_subscribe_ack(a, cb) (a)->ackcb = (cb);

/**
 * set the callback for hook find
 * @param   a       pointer to the application
 * @param   cb      pointer to the callback
 * @return  void
 */
#define icc_app_set_cb_find(a, cb) (a)->findcb = (cb);

/**
 * set the callback for hook offer
 * @param   a       pointer to the application
 * @param   cb      pointer to the callback
 * @return  void
 */
#define icc_app_set_cb_offer(a, cb) (a)->offercb = (cb);

    /**
 * initial the application
 * @param   p       pointer to the application
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    no error should occur during initialization, and return value is 
 *          allowed not to be processed.
 */
    ICC_EXTERN
    int icc_app_init(icc_app_t* p);

    /**
 * open the application
 * @param   p       pointer to the application
 * @param   rt      pointer to the routing which is opened
 * @param   cid     the client id of the application
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    ICC_EXTERN
    int icc_app_open(icc_app_t* p, icc_routing_t* rt, icc_cid_t cid);

    /**
 * open the application
 * @param   p       pointer to the application
 * @param   rt      pointer to the routing which is opened
 * @param   name    pointer to the name of the application
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    ICC_EXTERN
    int icc_app_open_name(icc_app_t* p, icc_routing_t* rt, const char* name);

    /**
 * close the application
 * @param   p       pointer to the application
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    ICC_EXTERN
    int icc_app_close(icc_app_t* p);

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
    ICC_EXTERN
    int icc_app_send(icc_app_t* p, icc_message_t* m);

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
    ICC_EXTERN
    int icc_app_execute(icc_app_t* p, icc_app_execute_f cb, void* ud);

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
    ICC_EXTERN
    int icc_app_offer_service(icc_app_t* p, icc_serv_t serv, icc_inst_t inst, icc_version_t major, icc_versmin_t minor);

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
    ICC_EXTERN
    int icc_app_stop_service(icc_app_t* p, icc_serv_t serv, icc_inst_t inst, icc_version_t major, icc_versmin_t minor);

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
    ICC_EXTERN
    int icc_app_offer_event(
        icc_app_t* p, icc_serv_t serv, icc_inst_t inst, icc_eid_t event, const icc_event_info_t* info);

    /**
 * stop a offered event
 * @param   p       pointer to the application
 * @param   serv    the service id
 * @param   inst    the instance id
 * @param   event   the event id
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    ICC_EXTERN
    int icc_app_stop_event(icc_app_t* p, icc_serv_t serv, icc_inst_t inst, icc_eid_t event);

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
    ICC_EXTERN
    int icc_app_request_service(
        icc_app_t* p, icc_serv_t serv, icc_inst_t inst, icc_version_t major, icc_versmin_t minor);

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
    ICC_EXTERN
    int icc_app_release_service(
        icc_app_t* p, icc_serv_t serv, icc_inst_t inst, icc_version_t major, icc_versmin_t minor);

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
    ICC_EXTERN
    int icc_app_request_event(
        icc_app_t* p, icc_serv_t serv, icc_inst_t inst, icc_eid_t event, const icc_event_info_t* info);

    /**
 * release a requested event
 * @param   p       pointer to the application
 * @param   serv    the service id
 * @param   inst    the instance id
 * @param   event   the event id
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    ICC_EXTERN
    int icc_app_release_event(icc_app_t* p, icc_serv_t serv, icc_inst_t inst, icc_eid_t event);

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
    ICC_EXTERN
    int icc_app_subscribe(
        icc_app_t* p, icc_serv_t serv, icc_inst_t inst, icc_gid_t group, icc_version_t major, icc_eid_t event);

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
    ICC_EXTERN
    int icc_app_unsubscribe(
        icc_app_t* p, icc_serv_t serv, icc_inst_t inst, icc_gid_t group, icc_version_t major, icc_eid_t event);

    /**
 * ack a subscribe
 * @param   p       pointer to the application
 * @param   sult    the result of subscribe, see @ref ICC_ERROR
 * @param   cid     the client id
 * @param   serv    the service id
 * @param   inst    the instance id
 * @param   group   the event group id
 * @param   event   the event id
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    ICC_EXTERN
    int icc_app_subscribe_ack(
        icc_app_t* p, int sult, icc_cid_t cid, icc_serv_t serv, icc_inst_t inst, icc_gid_t group, icc_eid_t event);

    /**
 * create a message for write
 * @param   p       pointer to the application
 * @retval  !=0     the address of the new message on success
 * @retval  0       an error occurred, see #nai_errno
 */
    ICC_EXTERN
    icc_message_t* icc_app_create_message(icc_app_t* p);

    /**
 * create a request message
 * @param   p       pointer to the application
 * @param   serv    the service id
 * @param   inst    the instance id
 * @param   meth    the method id
 * @retval  !=0     the address of the new message on success
 * @retval  0       an error occurred, see #nai_errno
 */
    ICC_EXTERN
    icc_message_t* icc_app_create_request(icc_app_t* p, icc_serv_t serv, icc_inst_t inst, icc_method_t meth);

    /**
 * create a response message
 * @param   p       pointer to the application
 * @param   request pointer to the request message
 * @param   sult    the value of the result, see @ref ICC_ERROR
 * @retval  !=0     the address of the new message on success
 * @retval  0       an error occurred, see #nai_errno
 */
    ICC_EXTERN
    icc_message_t* icc_app_create_response(icc_app_t* p, icc_message_t* request, uint8_t sult);

    /**
 * create a notification message
 * @param   p       pointer to the application
 * @param   serv    the service id
 * @param   inst    the instance id
 * @param   event   the event id
 * @retval  !=0     the address of the new message on success
 * @retval  0       an error occurred, see #nai_errno
 */
    ICC_EXTERN
    icc_message_t* icc_app_create_notification(icc_app_t* p, icc_serv_t serv, icc_inst_t inst, icc_eid_t event);

    //////////////////////////////////////////////////////////////////////////////
    // app dispatch

#ifndef _ICC_TYPEDEF_APPCB_T
    #define _ICC_TYPEDEF_APPCB_T
    typedef struct icc_appcb_s icc_appcb_t;
#endif

    /**
 * the structure of the callback dispatch of the application
 */
    struct icc_appcb_s
    {
        icc_app_t* app;     /**< pointer to the application */
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
    ICC_EXTERN
    int icc_appcb_init(icc_appcb_t* p);

    /**
 * open the callback dispatch
 * @param   p       pointer to the callback dispatch
 * @param   app     pointer to the application
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    ICC_EXTERN
    int icc_appcb_open(icc_appcb_t* p, icc_app_t* app);

    /**
 * close the callback dispatch
 * @param   p       pointer to the callback dispatch
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    ICC_EXTERN
    int icc_appcb_close(icc_appcb_t* p);

    /**
 * get the user data bound with the service
 * @param   p       pointer to the callback dispatch
 * @param   serv    the service id
 * @param   inst    the instance id
 * @retval  !=0     the address of the user data
 * @retval  0       an error occurred, see #nai_errno
 */
    ICC_EXTERN
    void* icc_appcb_get_ud(icc_appcb_t* p, icc_serv_t serv, icc_inst_t inst);

    /**
 * set the user data bound with the service
 * @param   p       pointer to the callback dispatch
 * @param   serv    the service id
 * @param   inst    the instance id
 * @param   ud      pointer to the user data
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    ICC_EXTERN
    int icc_appcb_set_ud(icc_appcb_t* p, icc_serv_t serv, icc_inst_t inst, void* ud);

    /**
 * set the callback for the service changed event
 * @param   p       pointer to the callback dispatch
 * @param   serv    the service id
 * @param   inst    the instance id
 * @param   cb      pointer to the callback
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    ICC_EXTERN
    int icc_appcb_set_available(icc_appcb_t* p, icc_serv_t serv, icc_inst_t inst, icc_app_available_f cb);

    /**
 * set the callback for subscribe event
 * @param   p       pointer to the callback dispatch
 * @param   serv    the service id
 * @param   inst    the instance id
 * @param   cb      pointer to the callback
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    ICC_EXTERN
    int icc_appcb_set_subscribe(icc_appcb_t* p, icc_serv_t serv, icc_inst_t inst, icc_app_subscribe_f cb);

    /**
 * set the callback for subscribe ack event
 * @param   p       pointer to the callback dispatch
 * @param   serv    the service id
 * @param   inst    the instance id
 * @param   cb      pointer to the callback
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    ICC_EXTERN
    int icc_appcb_set_subscribe_ack(icc_appcb_t* p, icc_serv_t serv, icc_inst_t inst, icc_app_subscribe_ack_f cb);

/**
 * set the callback for all sent message
 * @param   p       pointer to the callback dispatch
 * @param   s       the service id
 * @param   i       the instance id
 * @param   cb      pointer to the callback
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
#define icc_appcb_set_sent_any(p, s, i, cb) icc_appcb_set_sent((p), (s), (i), ICC_METHOD_ANY, (cb), 0)

/**
 * set the callback for all incoming message
 * @param   p       pointer to the callback dispatch
 * @param   s       the service id
 * @param   i       the instance id
 * @param   cb      pointer to the callback
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
#define icc_appcb_set_message_any(p, s, i, cb) icc_appcb_set_message((p), (s), (i), ICC_METHOD_ANY, (cb), 0)

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
    ICC_EXTERN
    int icc_appcb_set_sent(
        icc_appcb_t* p, icc_serv_t serv, icc_inst_t inst, icc_method_t meth, icc_app_sent_f cb, void* ud);

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
    ICC_EXTERN
    int icc_appcb_set_message(
        icc_appcb_t* p, icc_serv_t serv, icc_inst_t inst, icc_method_t meth, icc_app_message_f cb, void* ud);

    /**
 * test the service is available
 * @param   p       pointer to the callback dispatch
 * @param   serv    the service id
 * @param   inst    the instance id
 * @return  if it is available, return 1, otherwise return 0
 */
    ICC_EXTERN
    int icc_appcb_is_available(icc_appcb_t* p, icc_serv_t serv, icc_inst_t inst);

    /**
 * get all available services
 * @param   p       pointer to the callback dispatch
 * @param   serv    the service id
 * @param   inst    the instance id
 * @param   out     pointer to the output array
 * @retval  >=0     the number of the services on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    ICC_EXTERN
    int icc_appcb_get_availables(icc_appcb_t* p, icc_serv_t serv, icc_inst_t inst, nai_array_t* out);

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
