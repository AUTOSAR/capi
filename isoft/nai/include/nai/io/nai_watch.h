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
/// @file       nai_watch.h
/// @brief      the event loop watcher
/// @details
/// @date       2023-10-17
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


/**
 * @details this implementation makes callbacks before and after 
 *          event polling.
 * @details the watcher in 'pre' stage will be called before the 
 *          event polling.
 * @details the watcher in 'post' stage will be called after the 
 *          event polling.
 */

#ifndef _WATCH_H_NAI
#define _WATCH_H_NAI

#pragma once

#include "nai/io/nai_event.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#ifndef _NAI_TYPEDEF_WATCH_T
    #define _NAI_TYPEDEF_WATCH_T
    typedef struct nai_evnode_s nai_watch_t;
#endif
#ifndef _NAI_TYPEDEF_WATCH_CB_F
    #define _NAI_TYPEDEF_WATCH_CB_F
    typedef nai_int_t (*nai_watch_cb_f)(nai_watch_t* s, nai_int_t events);
#endif

/**
 * initial the watch handler
 * @param   w       pointer to the watch handler
 * @return  void
 */
#define nai_watch_init(w)                                                                                              \
    {                                                                                                                  \
        nai_evnode_init(w);                                                                                            \
    }

/**
 * test the watch handler is opened
 * @param   w       pointer to the watch handler
 * @return  a bool, indicates whether it is opened
 */
#define nai_watch_is_opened(w) ((w)->extend == NAI_EV_EXTEND_WATCH)

/**
 * test the caller thread is the event loop thread
 * @param   w       pointer to the watch handler
 * @return  a bool, indicates whether it is in the event loop thread
 */
#define nai_watch_in_dispatch(w) nai_evnode_in_dispatch(w)

/**
 * get the event loop associated with the watch handler
 * @param   w       pointer to the watch handler
 * @return  the address of the event loop
 */
#define nai_watch_get_loop(w) nai_evnode_get_loop(w)

/**
 * get the stage name of the watch handler
 * @param   w       pointer to the watch handler
 * @return  the value of stage name, see @ref NAI_EV_STAGE
 */
#define nai_watch_get_stage(w) ((nai_int_t)(w)->extval)

/**
 * set the callback of the watch handler
 * @param   w       pointer to the watch handler
 * @param   c       the callback of the event
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
#define nai_watch_set_cb(w, c) nai_evnode_set_cb((w), (c))

/**
 * send a uesr signal to the watch handler
 * @param   w       pointer to the watch handler
 * @param   sigid   the id of the user signal
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    this function is thread-safe, can be called in any thread.
 * @note    this function can be called before open the watch handler
 * @note    the user callback will be called with event #NAI_EV_NOTIFY
 *          use #nai_ev_notify_code to extract 'sigid'
 * @note    repeatedly sending the same signal will not generate additional 
 *          events until the signal is processed
 */
#define nai_watch_post(w, sigid) nai_evnode_post((w), (sigid))

/**
 * close the watch handler
 * @param   w       pointer to the watch handler
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    if the watch handler is opened, 
 *          this function must be called in the event loop thread
 */
#define nai_watch_close(w) nai_evnode_close(w)

    /**
 * open the watch handler
 * @param   w       pointer to the watch handler
 * @param   loop    pointer to the event loop
 * @param   stage   the stage name to watch, see @ref NAI_EV_STAGE_NAME.
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    the user callback will be called with #NAI_EV_STAGE in each loop, 
 *          use #nai_ev_stage_code to extract 'stage'
 */
    NAI_EXTERN
    nai_int_t nai_watch_open(nai_watch_t* w, nai_evloop_t* loop, nai_int_t stage);

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
