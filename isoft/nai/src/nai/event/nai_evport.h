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
/// @file       nai_evport.h
/// @brief      
/// @details
/// @date       2021-02-12
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#ifndef _EVPORT_H_NAI
#define _EVPORT_H_NAI

#pragma once

#include "nai/os/nai_aio.h"

#if (NAI_AIO_USE_POSIX) && defined(SIGEV_KEVENT)

    #define NAI_HAVE_AIO_PORT      1
    #define NAI_AIO_PORT_IMPL_FEAT NAI_EV_FEAT_FILE | NAI_EV_FEAT_PIPE | NAI_EV_FEAT_DEVC

#elif (NAI_AIO_USE_URING) && (NAI_HAVE_EVENTFD)

    #define NAI_HAVE_AIO_PORT      1
    #define NAI_AIO_PORT_IMPL_FEAT NAI_EV_FEAT_SOCK | NAI_EV_FEAT_FILE | NAI_EV_FEAT_PIPE | NAI_EV_FEAT_DEVC

#elif (NAI_AIO_USE_LINUX) && (NAI_HAVE_EVENTFD)

    #define NAI_HAVE_AIO_PORT      1
    #define NAI_AIO_PORT_IMPL_FEAT NAI_EV_FEAT_FILE | NAI_EV_FEAT_PIPE | NAI_EV_FEAT_DEVC

#endif

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#include "nai_evloop.h"

    typedef struct nai_evport_s nai_evport_t;

#if (NAI_HAVE_AIO_PORT)

    /**
 * the structure of the event port
 */
    struct nai_evport_s
    {
        nai_evnode_t ev;      /**< the event node */
        nai_evloop_ent_t ent; /**< the entry of event loop */
        nai_evbase_t back;    /**< the backend event base */
    };

    /**
 * test whether the event port is opened
 * @param   p       pointer to the event port
 * @return  if it is opened, return 1, otherwise return 0
 */
    #define nai_evport_is_opened(p) nai_evbase_is_opened(&(p)->back)

    /**
 * set the event of the event node
 * @param   p       pointer to the event port
 * @param   h       pointer to the event node
 * @param   ev      the value of events, accept #NAI_EV_IOE and #NAI_EV_ASYNC
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    #define nai_evport_set(p, h, ev) nai_evbase_set(&(p)->back, (h), (ev))

    /**
 * submit all queued opeartions to the system
 * @param   p       pointer to the event port
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    #define nai_evport_submit(p) nai_evbase_submit(&(p)->back)

    /**
 * get the address of the aio port
 * @param   p       pointer to the event port
 * @return  the address of the aio port
 */
    #define nai_evport_get_port(p) nai_evbase_get_port(&(p)->back)

#else

/**
 * the structure of the event port
 */
struct nai_evport_s
{
    nai_int_t unused; /**< unused */
};

    /**
 * test whether the event port is opened
 * @param   p       pointer to the event port
 * @return  if it is opened, return 1, otherwise return 0
 */
    #define nai_evport_is_opened(p) ((void)(p), 0)

    /**
 * submit all queued opeartions to the system
 * @param   p       pointer to the event port
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    #define nai_evport_submit(p)    ((void)(p), 0)

    /**
 * get the address of the aio port
 * @param   p       pointer to the event port
 * @return  the address of the aio port
 */
    #define nai_evport_get_port(p)  (((void)(p)), (nai_aio_port_t*)0)

/**
 * set the event of the event node
 * @param   p       pointer to the event port
 * @param   h       pointer to the event node
 * @param   events  the value of events, accept #NAI_EV_IOE and #NAI_EV_ASYNC
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
nai_int_t nai_evport_set(nai_evport_t* p, nai_evnode_t* h, nai_int_t events);

#endif

    /**
 * initial the event port
 * @param   p       pointer to the event port
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    no error should occur during initialization, and return value is 
 *          allowed not to be processed.
 */
    nai_int_t nai_evport_init(nai_evport_t* p);

    /**
 * open the event port
 * @param   p       pointer to the event port
 * @param   ops     pointer to the event base opeartions
 * @param   e       pointer to the context of the event base
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_evport_open(nai_evport_t* p, nai_evbase_ops_t* ops, void* e);

    /**
 * close the event port
 * @param   p       pointer to the event port
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_evport_close(nai_evport_t* p);

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
