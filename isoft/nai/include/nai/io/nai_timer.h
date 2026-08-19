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
/// @file       nai_timer.h
/// @brief      the high-res timer implementation
/// @details
/// @date       2023-03-24
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


/**
 * @details the high-res timer provides microsecond-level precision 
 *          timing. the specific precision value depends on the system 
 *          implementation. for example, it can only reach about 
 *          16 milliseconds on the win10 version.
 *
 * @par     implement a timer callback
 * @code
 *          nai_int_t my_timer_cb(nai_timer_t* t, nai_int_t events)
 *          {
 *              nai_int_t r;
 *
 *              switch (events & NAI_EV_MASK) {
 *              case NAI_TIMER:
 *                  // todo
 *                  break;
 *              default:
 *                  break;
 *              };
 *
 *              r = 0;
 *
 *              return r;
 *          };
 * @endcode
 *
 * @par     start a high-res timer
 * @code
 *          nai_int_t r;
 *          nai_timer_t s;
 *          nai_evloop_t* loop;
 *
 *          nai_timer_init(&t);
 *          nai_timer_set_cb(&t, my_timer_cb);
 *          nai_timer_set_timeout(&t, NAI_TIMEOP_SET, 500);
 *
 *          r = nai_timer_open(&s, loop);
 *          if (r < 0) {
 *              goto _fail;
 *          };
 * @endcode
 *
 */

#ifndef _TIMER_H_NAI
#define _TIMER_H_NAI

#pragma once

#include "nai/io/nai_event.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#ifndef _NAI_TYPEDEF_TIMER_T
    #define _NAI_TYPEDEF_TIMER_T
    typedef struct nai_evnode_s nai_timer_t;
#endif
#ifndef _NAI_TYPEDEF_TIMER_CB_T
    #define _NAI_TYPEDEF_TIMER_CB_T
    typedef nai_int_t (*nai_timer_cb_f)(nai_timer_t* t, nai_int_t events);
#endif

/**
 * initial the high-res timer
 * @param   t       pointer to the high-res timer
 * @return  void
 */
#define nai_timer_init(t)                                                                                              \
    {                                                                                                                  \
        nai_evnode_init(t);                                                                                            \
    }

/**
 * test the high-res timer is opened
 * @param   t       pointer to the high-res timer
 * @return  a bool, indicates whether it is opened
 */
#define nai_timer_is_opened(t) nai_evnode_is_opened(t)

/**
 * test the caller thread is the event loop thread
 * @param   t       pointer to the high-res timer
 * @return  a bool, indicates whether it is in the event loop thread
 */
#define nai_timer_in_dispatch(t) nai_evnode_in_dispatch(t)

/**
 * get the event loop associated with the high-res timer
 * @param   t       pointer to the high-res timer
 * @return  the address of the event loop
 */
#define nai_timer_get_loop(t) nai_evnode_get_loop(t)

/**
 * set the callback of the high-res timer
 * @param   t       pointer to the high-res timer
 * @param   c       the callback of the event
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
#define nai_timer_set_cb(t, c) nai_evnode_set_cb((t), (c))

/**
 * send a uesr signal to the high-res timer
 * @param   t       pointer to the high-res timer
 * @param   sigid   the id of the user signal
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    this function is thread-safe, can be called in any thread.
 * @note    this function can be called before open the high-res timer
 * @note    the user callback will be called with event #NAI_EV_NOTIFY
 *          use #nai_ev_notify_code to extract 'sigid'
 * @note    repeatedly sending the same signal will not generate additional 
 *          events until the signal is processed
 */
#define nai_timer_post(t, sigid) nai_evnode_post((t), (sigid))

/**
 * close the high-res timer
 * @param   t       pointer to the high-res timer
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    if the high-res timer is opened, 
 *          this function must be called in event loop thread
 */
#define nai_timer_close(t) nai_evnode_close(t)

    /**
 * set a user timer
 * @param   t       pointer to the high-res timer
 * @param   usec    the value of absolute tick, in micro-seconds, 
 *                  0 to disable
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    this function can be called in any thread, but not thread-safe.
 * @note    this function can be called before open the high-res timer
 */
    NAI_EXTERN
    nai_int_t nai_timer_set_expire(nai_timer_t* t, uint64_t usec);

    /**
 * set a user timer
 * @param   t       pointer to the high-res timer
 * @param   op      the operation of timeout, see @ref NAI_TIMEOP
 * @param   usec    the value of timeout, in micro-seconds, 
 *                  a positive number, -1 to disable
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    this function can be called in any thread, but not thread-safe.
 * @note    this function can be called before open the high-res timer
 */
    NAI_EXTERN
    nai_int_t nai_timer_set_timeout(nai_timer_t* t, nai_int_t op, int64_t usec);

    /**
 * open the high-res timer
 * @param   t       pointer to the high-res timer
 * @param   l       pointer to the event loop
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_timer_open(nai_timer_t* t, nai_evloop_t* l);

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
