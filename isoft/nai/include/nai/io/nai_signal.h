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
/// @file       nai_signal.h
/// @brief      signal implementation bound to the event loop
/// @details
/// @date       2021-11-15
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


/**
 * @details system signal processing has a relatively large limitation, 
 *          and only functions with signal safety can be used. the application 
 *          usually does not meet this requirement. in order to circumvent 
 *          these restrictions, the signal is forwarded to the event loop 
 *          through a safe mechanism, so that the signal processing is 
 *          the same as a normal io event.
 *
 * @details the code example is as follows:
 *
 * @par     implement a signal callback
 * @code
 *          nai_int_t my_signal_cb(nai_signal_t* s, nai_int_t events)
 *          {
 *              nai_int_t r;
 *              nai_int_t signum;
 *
 *              switch (events & NAI_EV_MASK) {
 *              case NAI_SIGNAL:
 *                  signum = nai_ev_signal_code(events);
 *                  switch (signum) {
 *                  case SIGCHLD:
 *                      // todo
 *                      break;
 *                  default:
 *                      break;
 *                  };
 *                  break;
 *              default:
 *                  break;
 *              };
 *          }
 * @endcode
 *
 * @par     open the signal handler
 * @code
 *          nai_int_t r;
 *          nai_signal_t s;
 *          nai_evloop_t* loop;
 *
 *
 *          nai_signal_init(&s);
 *          nai_signal_set_cb(&s, my_signal_cb);
 *
 *          r = nai_signal_open(&s, loop, SIGCHLD);
 *          if (r < 0) {
 *              goto _fail;
 *          };
 * @endcode
 *
 * @par     synchronous waiting for signal
 * @code
 *          nai_int_t r;
 *          nai_signal_t s;
 *
 *          nai_signal_init(&s);
 *          nai_signal_set_cb(&s, my_signal_cb);
 *
 *          r = nai_signal_open(&s, 0, SIGCHLD);
 *          if (r < 0) {
 *              goto _fail;
 *          };
 *
 *          r = nai_signal_wait(-1);
 *          if (r < 0) {
 *              goto _fail;
 *          };
 *
 *          r = nai_signal_close(&s);
 *          if (r < 0) {
 *              goto _fail;
 *          };
 * @endcode
 */

#ifndef _SIGNAL_H_NAI
#define _SIGNAL_H_NAI

#pragma once

#include "nai/io/nai_event.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/** the system signal event */
#define NAI_EV_SIGNAL NAI_EV_SPECIFIC

/**
 * @anchor  NAI_EV_SIGNUM
 * @name    NAI_EV_SIGNUM       the help macro of signal number
 * @{
 */

/** deprecated, will be removed */
#define NAI_EV_SIGNAL_FROM nai_ev_signal_from
/** deprecated, will be removed */
#define NAI_EV_SIGNAL_CODE nai_ev_signal_code

#define nai_ev_signal_from(c) (NAI_EV_SIGNAL | ((uint32_t)(c) << NAI_EV_VALUE_SHIFT))
#define nai_ev_signal_code(e) (((e)&NAI_EV_VALUE_MASK) >> NAI_EV_VALUE_SHIFT)

    /** @} */

#ifndef _NAI_TYPEDEF_SIGNAL_T
    #define _NAI_TYPEDEF_SIGNAL_T
    typedef struct nai_evnode_s nai_signal_t;
#endif
#ifndef _NAI_TYPEDEF_SIGNAL_CB_F
    #define _NAI_TYPEDEF_SIGNAL_CB_F
    typedef nai_int_t (*nai_signal_cb_f)(nai_signal_t* s, nai_int_t events);
#endif

/**
 * initial the signal handler
 * @param   s       pointer to the signal handler
 * @return  void
 */
#define nai_signal_init(s)                                                                                             \
    {                                                                                                                  \
        nai_evnode_init(s);                                                                                            \
    }

/**
 * test the signal handler is opened
 * @param   s       pointer to the signal handler
 * @return  a bool, indicates whether it is opened
 */
#define nai_signal_is_opened(s) ((s)->extend == NAI_EV_EXTEND_SIGNAL)

/**
 * test the caller thread is the event loop thread
 * @param   s       pointer to the signal handler
 * @return  a bool, indicates whether it is in the event loop thread
 */
#define nai_signal_in_dispatch(s) nai_evnode_in_dispatch(s)

/**
 * get the event loop associated with the signal handler
 * @param   s       pointer to the signal handler
 * @return  the address of the event loop
 */
#define nai_signal_get_loop(s) nai_evnode_get_loop(s)

/**
 * set the signal num of the signal handler
 * @param   s       pointer to the signal handler
 * @return  the value of signal num
 */
#define nai_signal_get_num(s) ((nai_int_t)(s)->extval)

/**
 * set the callback of the signal handler
 * @param   s       pointer to the signal handler
 * @param   c       the callback of the event
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
#define nai_signal_set_cb(s, c) nai_evnode_set_cb((s), (c))

/**
 * set a user timer
 * @param   s       pointer to the signal handler
 * @param   op      the operation of timeout, see @ref NAI_TIMEOP
 * @param   msec    the value of timeout, a positive number, -1 to disable
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    this function is thread-safe, can be called in any thread.
 * @note    this function can be called before open the signal handler
 */
#define nai_signal_set_timeout(s, op, msec) nai_evnode_set_timeout((s), (op), (msec))

/**
 * send a uesr signal to the signal handler
 * @param   s       pointer to the signal handler
 * @param   sigid   the id of the user signal
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    this function is thread-safe, can be called in any thread.
 * @note    this function can be called before open the signal handler
 * @note    the user callback will be called with event #NAI_EV_NOTIFY
 *          use #nai_ev_notify_code to extract 'sigid'
 * @note    repeatedly sending the same signal will not generate additional 
 *          events until the signal is processed
 */
#define nai_signal_post(s, sigid) nai_evnode_post((s), (sigid))

/**
 * close the signal handler
 * @param   s       pointer to the signal handler
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    if the signal handler is opened, 
 *          this function must be called in the event loop thread
 */
#define nai_signal_close(s) nai_evnode_close(s)

    /**
 * open the signal handler
 * @param   s       pointer to the signal handler
 * @param   loop    pointer to the event loop, optional
 * @param   signum  the value of signal num to listen
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    the user callback will be called with event #NAI_EV_SIGNAL 
 *          when the system signal arrives
 * @note    if the signal handler open without an event loop, 
 *          use nai_signal_wait to wait singal
 */
    NAI_EXTERN
    nai_int_t nai_signal_open(nai_signal_t* s, nai_evloop_t* loop, nai_int_t signum);

    /**
 * wait the signal handler which is opened without the event loop
 * @param   msec    the wait timeout, in milli-seconds
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    when one or more signals arrive, this function will return 
 *          immediately after processing.
 */
    NAI_EXTERN
    nai_int_t nai_signal_wait(uint32_t msec);

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
