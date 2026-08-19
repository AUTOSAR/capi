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
/// @file       nai_evbase.h
/// @brief      
/// @details
/// @date       2020-11-28
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#ifndef _EVBASE_H_NAI
#define _EVBASE_H_NAI

#pragma once

#include "nai/io/nai_event.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#ifndef _NAI_TYPEDEF_EVBASE_T
    #define _NAI_TYPEDEF_EVBASE_T
    typedef struct nai_evbase_s nai_evbase_t;
#endif
#ifndef _NAI_TYPEDEF_EVBASE_OPS_T
    #define _NAI_TYPEDEF_EVBASE_OPS_T
    typedef struct nai_evbase_ops_s nai_evbase_ops_t;
#endif
#ifndef _NAI_TYPEDEF_AIO_PORT_T
    #define _NAI_TYPEDEF_AIO_PORT_T
    typedef struct nai_aio_port_s nai_aio_port_t;
#endif

    //////////////////////////////////////////////////////////////////////////////
    // event base

#define NAI_EVBASE_BACKEND 1

    /**
 * the structure of the event base opeartions
 */
    struct nai_evbase_ops_s
    {
        const char* name;    /**< the model name */
        nai_int_t feature_m; /**< the main feature */
        nai_int_t feature_e; /**< the enhanced feature */

        void* (*open)(nai_int_t flags);
        nai_int_t (*add)(void* e, nai_evnode_t* h, nai_int_t events);
        nai_int_t (*del)(void* e, nai_evnode_t* h);
        nai_int_t (*set)(void* e, nai_evnode_t* h, nai_int_t events);
        nai_int_t (*wait)(void* e, uint32_t msec);
        nai_int_t (*fetch)(void* e, nai_evloop_ent_t* list[2]);
        nai_int_t (*submit)(void* e);
        nai_int_t (*signal)(void* e);
        nai_int_t (*close)(void* e);
        nai_int_t (*rearm)(void* e);
        nai_fd_t (*get_fd)(void* e);
        nai_aio_port_t* (*get_port)(void* e);
    };

    /**
 * the structure of the event base
 */
    struct nai_evbase_s
    {
        nai_evbase_ops_t* ops; /**< pointer to the event base opeartions */
        void* ud;              /**< pointer to user data */
    };

#define NAI_EV_INITSIZE 512

#define nai_evbase_is_opened(e)      ((e)->ops != 0)
#define nai_evbase_name(e)           ((e)->ops ? (e)->ops->name : 0)
#define nai_evbase_feature(e)        ((e)->ops ? (e)->ops->feature_m : 0)
#define nai_evbase_add(e, h, events) ((e)->ops->add((e)->ud, (h), (events)))
#define nai_evbase_del(e, h)         ((e)->ops->del((e)->ud, (h)))
#define nai_evbase_set(e, h, events) ((e)->ops->set((e)->ud, (h), (events)))
#define nai_evbase_wait(e, t)        ((e)->ops->wait((e)->ud, (t)))
#define nai_evbase_fetch(e, l)       ((e)->ops->fetch((e)->ud, (l)))
#define nai_evbase_submit(e)         ((e)->ops->submit((e)->ud))
#define nai_evbase_signal(e)         ((e)->ops->signal((e)->ud))
#define nai_evbase_rearm(e)          ((e)->ops->rearm((e)->ud))
#define nai_evbase_get_fd(e)         ((e)->ops->get_fd((e)->ud))
#define nai_evbase_get_port(e)       ((e)->ops->get_port((e)->ud))

    /**
 * initial the event base
 * @param   e       pointer to the event base
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    no error should occur during initialization, and return value is 
 *          allowed not to be processed.
 */
    nai_int_t nai_evbase_init(nai_evbase_t* e);

    /**
 * open the event base
 * @param   e       pointer to the event base
 * @param   optional the optional features, see @ref NAI_EV_FEAT
 * @param   require the required features, see @ref NAI_EV_FEAT
 * @param   name    the name of the event base implementation
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_evbase_open(nai_evbase_t* e, nai_int_t optional, nai_int_t require, const char* name);

    /**
 * close the event base
 * @param   e       pointer to the event base
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_evbase_close(nai_evbase_t* e);

    //////////////////////////////////////////////////////////////////////////////
    // event signal

#ifndef _NAI_TYPEDEF_EVSIGNAL_T
    #define _NAI_TYPEDEF_EVSIGNAL_T
    typedef struct nai_evsignal_s nai_evsignal_t;
#endif

    /**
 * the structure of the event signal
 */
    struct nai_evsignal_s
    {
        nai_int_t signaled; /**< is already signal seted */
        nai_int_t count;    /**< the count of signal seted */

        union
        {
            struct
            {
                nai_fd_t in;  /**< the input file descriptor of signal */
                nai_fd_t out; /**< the output file descriptor of signal */
            };
            nai_fd_t fds[2]; /**< the file descriptors of signal */
        };
    };

    /**
 * initial the event signal
 * @param   s       pointer to the event signal
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    no error should occur during initialization, and return value is 
 *          allowed not to be processed.
 */
    nai_int_t nai_evsignal_init(nai_evsignal_t* s);

    /**
 * open the event signal
 * @param   s       pointer to the event signal
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_evsignal_open(nai_evsignal_t* s);

    /**
 * set a signal to the event signal
 * @param   s       pointer to the event signal
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_evsignal_set(nai_evsignal_t* s);

    /**
 * reset the event signal
 * @param   s       pointer to the event signal
 * @param   et      is tht event loop worked in edge trigger mode
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_evsignal_reset(nai_evsignal_t* s, nai_int_t et);

    /**
 * close the event signal
 * @param   s       pointer to the event signal
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_evsignal_close(nai_evsignal_t* s);

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
