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
/// @file       nai_event.h
/// @brief      the event loop
/// @details
/// @date       2021-01-01
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


/**
 * @details the event loop implements the event model in most systems. 
 *          the supported list is as follows: \n
 *          1. epoll   linux \n
 *          2. uring   linux \n
 *          3. kqueue  *bsd, mac os \n
 *          4. iocp    windows \n
 *          5. wpoll   windows \n
 *          6. poll    all \n
 *          7. select  all \n
 *
 * @details the behavior of event loop on io events 
 *          depends on the implementation of the backend. for example, 
 *          when the backend is iocp, event loop is a proactive model 
 *          and does not have the ability to poll files.
 *
 * @details the event node has the functions of io event dispatching, 
 *          timers and custom signals, and functions other than io event 
 *          dispatching do not depend on the implementation of the backend. 
 *          these can be used alone, for example, the event node is 
 *          only used for timers, and there is no need to provide 
 *          file descriptors at this time.
 *
 * @details the code example is as follows:
 *
 * @par     create a new event loop
 * @code
 *          nai_int_t r;
 *          nai_evloop_t* l;
 *
 *          l = nai_evloop_new();
 *          if (l == 0) {
 *              goto _fail;
 *          };
 *
 *          // if the backend name is not specified, 
 *          // it will be automatically selected internally
 *          r = nai_evloop_open(l, 0, 0, "epoll");
 *          if (r < 0) {
 *              goto _fail;
 *          };
 * @endcode
 *
 * @par     run the event loop
 * @code
 *          nai_thread_api my_thread_proc(void* p)
 *          {
 *              nai_int_t r;
 *              nai_evloop_t* l;
 *
 *              l = (nai_evloop_t*)p;
 *              r = nai_evloop_dispatch(l);
 *
 *              // exited
 *              return 0;
 *          }
 * @endcode
 *
 * @par     perform dispatch in a new thread
 * @code
 *          nai_int_t r;
 *          nai_evloop_t* l;
 *          nai_thread_t t;
 *
 *          r = nai_thread_create(&t, 0, my_thread_proc, l);
 *          if (r < 0) {
 *              goto _fail;
 *          };
 *
 *          // wait until dispatch is running in new thread
 *          r = nai_evloop_wait(l, NAI_EV_WAIT_RUNNING);
 *          if (r < 0) {
 *              goto _fail;
 *          };
 *
 * @endcode
 *
 * @par     implement a complete event callback
 * @code
 *          nai_int_t my_event_cb(nai_evnode_t* n, nai_int_t events)
 *          {
 *              nai_int_t r;
 *              nai_int_t error;
 *              nai_int_t notify_id;
 *
 *              if (events & NAI_EV_ERROR) {
 *                  error = nai_ev_error_code(events);
 *                  if (error) {
 *                      // Error occurred
 *                  } else {
 *                      // Error event: POLLERR, EPOLLERR
 *                  };
 *
 *                  // May include remaining read/write events, need to continue processing
 *                  events &= ~NAI_EV_ERROR;
 *              };
 *
 *              switch (events & NAI_EV_MASK) {
 *              case NAI_EV_READ:
 *              case NAI_EV_WRITE:
 *              case NAI_EV_READ|NAI_EV_WRITE:
 *                  // Read/write events
 *                  break;
 *
 *              case NAI_EV_EXCEPT:
 *                  // Urgent events
 *                  break;
 *
 *              case NAI_EV_TIMEOUT:
 *                  // User-defined timer
 *                  break;
 *
 *              caee NAI_EV_NOTIFY:
 *                  // User-defined event
 *                  notify_id = nai_ev_notify_code(events);
 *                  switch (notify_id) {
 *                  case 0:
 *                  case 1:
 *                  case 2:
 *                      break;
 *                  };
 *                  break;
 *              };
 *
 *              r = 0;
 *
 *              return r;
 *          };
 * @endcode
 *
 * @par     join an event node into the event loop
 * @code
 *          nai_int_t r;
 *          nai_fd_t fd;
 *          nai_evloop_t* l;
 *          nai_evnode_t* n;
 *
 *          nai_evnode_init(n);
 *          nai_evnode_set_cb(n, my_event_cb);
 *          nai_evnode_set_fd(n, fd, NAI_FD_SOCK);
 *
 *          r = nai_evnode_open(n, l);
 *          if (r < 0) {
 *              goto _fail;
 *          };
 * @endcode
 *
 * @par     start a timer
 * @code
 *          nai_int_t r;
 *          nai_evnode_t* n;
 *
 *          // the timed events will occur after 100ms
 *          r = nai_evnode_set_timeout(n, NAI_TIME_SET, 100);
 *          if (r < 0) {
 *              goto _fail;
 *          };
 * @endcode
 *
 * @par     send a signal to the event node in other thread
 * @code
 *          nai_int_t r;
 *          nai_int_t sigid = 1;
 *          nai_evnode_t* n;
 *
 *          r = nai_evnode_post(n, sigid);
 *          if (r < 0) {
 *              goto _fail;
 *          };
 * @endcode
 */

#ifndef _EVENT_H_NAI
#define _EVENT_H_NAI

#pragma once

#include "nai/runtime/nai_types.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    //////////////////////////////////////////////////////////////////////////////
    // time cache

#ifndef NAI_TYPEDEF_TIMEFMTS_T
    #define NAI_TYPEDEF_TIMEFMTS_T
    typedef struct nai_timefmts_s nai_timefmts_t;
#endif

    /**
 * the structure of the time format strings
 */
    struct nai_timefmts_s
    {
        /** current time, in seconds */
        time_t sec;
        /** the time string of rfc822 format */
        char rfc822_time[sizeof("Mon, 28 Sep 1970 06:00:00 GMT")];
        /** the time string of iso8601 format */
        char iso8601_time[sizeof("1970-09-28T12:00:00+06:00")];
        /** the time string of error log format */
        char err_log_time[sizeof("1970/09/28 12:00:00")];
        /** the time stirng of http log format */
        char http_log_time[sizeof("28/Sep/1970:12:00:00 +0600")];
    };

/**
 * get time from time cache which is updated by the event loop
 * @return  the value of current time, in seconds
 * @note    this function should be used with the event loop, or manual update.
 */
#define nai_timecache_to_sec() (nai_timecache_to_usec() / (1000 * 1000))

    /**
 * manual update time cache
 * @return  the value of current tick
 * @note    this function must be used with the event loop, 
 *          because the time is updated by the event loop
 */
    NAI_EXTERN
    nai_int_t nai_timecache_update();

    /**
 * get time from time cache which is updated by the event loop
 * @return  the value of current time, in micro-seconds
 * @note    this function should be used with the event loop, or manual update.
 */
    NAI_EXTERN
    uint64_t nai_timecache_to_usec();

    /**
 * get tick from time cache which is updated by the event loop
 * @return  the value of current tick, in micro-seconds
 * @note    this function should be used with the event loop, or manual update.
 */
    NAI_EXTERN
    uint64_t nai_tickcache_to_usec();

    /**
 * get tick from time cache which is updated by the event loop
 * @return  the value of current tick, in milli-seconds
 * @note    this function should be used with the event loop, or manual update.
 */
    NAI_EXTERN
    uint64_t nai_tickcache_to_msec();

    /**
 * get tick from time cache which is updated by the event loop
 * @return  the value of current tick, in milli-seconds
 * @note    this function should be used with the event loop, or manual update.
 */
    NAI_EXTERN
    uint32_t nai_tickcache_to_msec32();

    /**
 * get current time format strings
 * @return  the format of current time
 * @note    this function should be used with the event loop, or manual update.
 */
    NAI_EXTERN
    nai_timefmts_t* nai_timecache_fmts();

//////////////////////////////////////////////////////////////////////////////
// events

/**
 * @anchor  NAI_EV_OP
 * @name    NAI_EV_OP           the flag of event operation
 * @{
 */
#define NAI_EV_ADD 0x00 /**< add events: ev |= val */
#define NAI_EV_DEL 0x01 /**< remove events: ev &= ~val */
#define NAI_EV_SET 0x02 /**< assign events: ev = val */
/** @} */

/**
 * @anchor  NAI_EV_EVENT
 * @name    NAI_EV_EVENT        the flags of event
 * @{
 */
#define NAI_EV_READ     0x01 /**< the read event */
#define NAI_EV_WRITE    0x02 /**< the write event */
#define NAI_EV_EXCEPT   0x04 /**< the urgent event */
#define NAI_EV_IO       0x03 /**< equal NAI_EV_READ|WRITE */
#define NAI_EV_IOE      0x07 /**< equal NAI_EV_READ|WRITE|EXCEPT */
#define NAI_EV_TIMEOUT  0x08 /**< the timer is timedout */
#define NAI_EV_ERROR    0x10 /**< an error occurred */
#define NAI_EV_NOTIFY   0X20 /**< the user notification  */
#define NAI_EV_STAGE    0x40 /**< the stage event */
#define NAI_EV_MASK     0x7f /**< the mask of event */
#define NAI_EV_SPECIFIC 0x04 /**< the specific event, ie signal */
#define NAI_EV_ASYNC    0x08 /**< to bind async model, not an event */
    /** @} */

    /**
 * @anchor  NAI_EV_VALUE
 * @name    NAI_EV_VALUE        the help macro of event value
 * @{
 */

#define NAI_EV_VALUE_MASK  0xffffff00
#define NAI_EV_VALUE_SHIFT 8

/** deprecated, will be removed */
#define NAI_EV_ERROR_FROM nai_ev_error_from
/** deprecated, will be removed */
#define NAI_EV_ERROR_CODE nai_ev_error_code
/** deprecated, will be removed */
#define NAI_EV_NOTIFY_FROM nai_ev_notify_from
/** deprecated, will be removed */
#define NAI_EV_NOTIFY_CODE nai_ev_notify_code

#define nai_ev_error_from(c) (NAI_EV_ERROR | ((uint32_t)(c) << NAI_EV_VALUE_SHIFT))
#define nai_ev_error_code(e) ((nai_int_t)((e)&NAI_EV_VALUE_MASK) >> NAI_EV_VALUE_SHIFT)

#define nai_ev_stage_from(c) (NAI_EV_STAGE | ((uint32_t)(c) << NAI_EV_VALUE_SHIFT))
#define nai_ev_stage_code(e) ((nai_int_t)((e)&NAI_EV_VALUE_MASK) >> NAI_EV_VALUE_SHIFT)

#define nai_ev_notify_from(c) (NAI_EV_NOTIFY | ((uint32_t)(c) << NAI_EV_VALUE_SHIFT))
#define nai_ev_notify_code(e) ((nai_int_t)((e)&NAI_EV_VALUE_MASK) >> NAI_EV_VALUE_SHIFT)

/** @} */

/**
 * @anchor  NAI_EV_FEAT
 * @name    NAI_EV_FEAT         the flag of feature
 * @{
 */
#define NAI_EV_FEAT_LEVEL     0x01 /**< non-block: level tigger */
#define NAI_EV_FEAT_EDGE      0x02 /**< non-block: edge tigger */
#define NAI_EV_FEAT_POLL      0x03 /**< non-block: level or edge */
#define NAI_EV_FEAT_ASYNC     0x04 /**< async io model: iocp, aio */
#define NAI_EV_FEAT_BLOCK     0x08 /**< block mode */
#define NAI_EV_FEAT_MODELMASK 0x0F /**< mask of poll type */
#define NAI_EV_FEAT_FDSTART   0x08
#define NAI_EV_FEAT_FILE      0x10  /**< 0x08 << NAI_FD_TYPE_FILE */
#define NAI_EV_FEAT_PIPE      0x20  /**< 0x08 << NAI_FD_TYPE_PIPE */
#define NAI_EV_FEAT_DEVC      0x40  /**< 0x08 << NAI_FD_TYPE_DEVC */
#define NAI_EV_FEAT_SOCK      0x80  /**< 0x08 << NAI_FD_TYPE_SOCK */
#define NAI_EV_FEAT_FDMASK    0xF0  /**< mask of fd type */
#define NAI_EV_FEAT_AIO_PORT  0x100 /**< the model can poll aio ops */
/** @} */

/**
 * @anchor  NAI_EV_STAGE_NAME
 * @name    NAI_EV_STAGE_NAME   the name of stage
 * @{
 */
#define NAI_EV_STAGE_PRE  0 /**< the stage run before polling */
#define NAI_EV_STAGE_POST 1 /**< the stage run after polling */
#define NAI_EV_STAGE_MAX  2
/** @} */

/**
 * @anchor  NAI_EV_WAIT
 * @name    NAI_EV_WAIT         the flag of wait mode
 * @{
 */
#define NAI_EV_WAIT_RUNNING 0x1 /**< wait until event loop is running */
#define NAI_EV_WAIT_BREAK   0x2 /**< wait until event loop is stoped */
#define NAI_EV_WAIT_CHANGED 0x3 /**< wait until running state is changed */
/** @} */

/**
 * @anchor  NAI_TIMEOP
 * @name    NAI_TIMEOP          the flag of timeout setting operation
 * @{
 */
#define NAI_TIMEOP_SET 0 /**< set new */
#define NAI_TIMEOP_MIN 1 /**< set min(cur, new) */
#define NAI_TIMEOP_MAX 2 /**< set max(cur, new) */
#define NAI_TIMEOP_ADD 3 /**< set last + new */
/** @} */

/**
 * the max static slot id
 */
#define NAI_EV_SLOT_STATIC_MAX 0x10000

    //////////////////////////////////////////////////////////////////////////////
    // event loop

#ifndef _NAI_TYPEDEF_TASK_F
    #define _NAI_TYPEDEF_TASK_F
    typedef nai_int_t (*nai_task_f)(void* args);
#endif
#ifndef _NAI_TYPEDEF_TASK_POOL_T
    #define _NAI_TYPEDEF_TASK_POOL_T
    typedef struct nai_task_pool_s nai_task_pool_t;
#endif
#ifndef _NAI_TYPEDEF_AIO_PORT_T
    #define _NAI_TYPEDEF_AIO_PORT_T
    typedef struct nai_aio_port_s nai_aio_port_t;
#endif
#ifndef _NAI_TYPEDEF_EVNODE_T
    #define _NAI_TYPEDEF_EVNODE_T
    typedef struct nai_evnode_s nai_evnode_t;
#endif
#ifndef _NAI_TYPEDEF_EVLOOP_T
    #define _NAI_TYPEDEF_EVLOOP_T
    typedef struct nai_evloop_s nai_evloop_t;
#endif
#ifndef _NAI_TYPEDEF_EVLOOP_ENT_T
    #define _NAI_TYPEDEF_EVLOOP_ENT_T
    typedef struct nai_evloop_ent_s nai_evloop_ent_t;
#endif
#ifndef _NAI_TYPEDEF_EVLOOP_FILTER_F
    #define _NAI_TYPEDEF_EVLOOP_FILTER_F
    typedef nai_int_t (*nai_evloop_filter_f)(nai_evloop_t* l, void* ud);
#endif

/**
 * test the event loop support asynchronous io
 * @param   l       pointer to the event loop
 * @param   t       the type of file desciption, see @ref NAI_FD_TYPE
 * @return  if supported, return 1, otherwise return 0
 */
#define nai_evloop_is_asyncio(l, t) (nai_evloop_get_fd_feature((l), (t)) & NAI_EV_FEAT_ASYNC)

/**
 * get required the flags of open file
 * @param   l       pointer to the event loop
 * @param   t       the type of file desciption, see @ref NAI_FD_TYPE
 * @return  the value of the flags
 */
#define nai_evloop_get_oflags(l, t) (nai_evloop_is_asyncio((l), (t)) ? NAI_O_ASYNCIO : 0)

    /**
 * create a new event loop
 * @return  the address of the new event loop
 */
    NAI_EXTERN
    nai_evloop_t* nai_evloop_new();

    /**
 * get the implementation name of the event loop
 * @param   l       pointer to the event loop
 * @return  if success the address of the name string is returned, 
 *          otherwise null is returned, see #nai_errno.
 */
    NAI_EXTERN
    const char* nai_evloop_get_name(nai_evloop_t* l);

    /**
 * get the aio port of the event loop
 * @param   l       pointer to the event loop
 * @return  if success the address of the aio port is returned, 
 *          otherwise null is returned, see #nai_errno.
 */
    NAI_EXTERN
    nai_aio_port_t* nai_evloop_get_aio_port(nai_evloop_t* l);

    /**
 * get the features of the event model
 * @param   l       pointer to the event loop
 * @param   which   which model, see #NAI_EV_FEAT_POLL and #NAI_EV_FEAT_ASYNC
 * @retval  >=0     the flags of feature, see @ref NAI_EV_FEAT
 * @retval  -1      if the event loop isn't opened
 */
    NAI_EXTERN
    nai_int_t nai_evloop_get_feature(nai_evloop_t* l, nai_int_t which);

    /**
 * get the features of the file description type
 * @param   l       pointer to the event loop
 * @param   type    the type of file desciption, see @ref NAI_FD_TYPE
 * @retval  >=0     the flags of feature, see @ref NAI_EV_FEAT
 * @retval  -1      if the event loop isn't opened
 */
    NAI_EXTERN
    nai_int_t nai_evloop_get_fd_feature(nai_evloop_t* l, nai_int_t type);

    /**
 * get the maximum poll waiting time value
 * @param   l       pointer to the event loop
 * @return  the value of waiting time on success, in milli-seconds
 */
    NAI_EXTERN
    nai_int_t nai_evloop_get_timeval(nai_evloop_t* l);

    /**
 * set the maximum poll waiting time value
 * @param   l       pointer to the event loop
 * @param   msec    the time value, in milli-seconds
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_evloop_set_timeval(nai_evloop_t* l, int32_t msec);

    /**
 * set backend io threads
 * @param   l       pointer to the event loop
 * @param   p       pointer to the task pool
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    the backend io threads for platforms that do not support 
 *          the asynchronous file feature, and file operations are performed 
 *          on the backend io threads.
 * @note    p is set to null to remove the current io threads, but 
 *          it will fail because there is an object holding the io threads.
 */
    NAI_EXTERN
    nai_int_t nai_evloop_set_io_threads(nai_evloop_t* l, nai_task_pool_t* p);

    /**
 * open the event loop
 * @param   l       pointer to the event loop which isn't opened
 * @param   optional the optional features, see @ref NAI_EV_FEAT
 * @param   require the required features, see @ref NAI_EV_FEAT
 * @param   name    the name of the event loop implementation, 
 *                  supported list:\n
 *                  1. epoll   linux \n
 *                  2. uring   linux \n
 *                  3. kqueue  *bsd, mac os \n
 *                  4. iocp    windows \n
 *                  5. wpoll   windows \n
 *                  6. poll    all \n
 *                  7. select  all \n
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_evloop_open(nai_evloop_t* l, nai_int_t optional, nai_int_t require, const char* name);

    /**
 * test the current thread is the dispatch thread
 * @param   l       pointer to the event loop
 * @retval  1       means is in dispatch thread
 * @retval  0       means is not in dispatch thread
 */
    NAI_EXTERN
    nai_int_t nai_evloop_in_dispatch(nai_evloop_t* l);

    /**
 * run the event loop
 * @param   l       pointer to the event loop
 * @retval  >=0     the exit code
 * @retval  -1      an error occurred, see #nai_errno
 * @note    the function will not exist until nai_evloop_break be called
 */
    NAI_EXTERN
    nai_int_t nai_evloop_dispatch(nai_evloop_t* l);

    /**
 * make new mail slots
 * @param   l       pointer to the event loop
 * @param   count   the count of the mail slots
 * @retval  >=0     the start id of new mail slots
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_evloop_make_slot(nai_evloop_t* l, nai_int_t count);

    /**
 * drop the mail slots
 * @param   l       pointer to the event loop
 * @param   slot    the start id of the mail slots
 * @param   count   the count of the mail slots
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_evloop_drop_slot(nai_evloop_t* l, nai_int_t slot, nai_int_t count);

    /**
 * make a function and join to the mail slot
 * @param   l       pointer to the event loop
 * @param   slot    the id of the mail slot
 * @param   cb      pointer to the task callback
 * @param   ud      pointer to the task argument
 * @retval  >=0     the id of the function is returned on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_evloop_make_func(nai_evloop_t* l, nai_int_t slot, nai_task_f cb, void* ud);

    /**
 * drop the function and move from the mail slot
 * @param   l       pointer to the event loop
 * @param   func    the id of the function
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    doing in other threads cannot stop running.
 * @note    doing in the event loop thread ensures that they are 
 *          no longer running.
 */
    NAI_EXTERN
    nai_int_t nai_evloop_drop_func(nai_evloop_t* l, nai_int_t func);

    /**
 * post a signal to the mail slot, all linked nodes will be notified
 * @param   l       pointer to the event loop
 * @param   slot    the id of the mail slot
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_evloop_post(nai_evloop_t* l, nai_int_t slot);

    /**
 * queue a task to the event loop and execute synchronously
 * @param   l       pointer to the event loop
 * @param   cb      pointer to the task callback
 * @param   ud      pointer to the task argument
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    this function returns the task's return value and errno 
 *          when executed successfully
 */
    NAI_EXTERN
    nai_int_t nai_evloop_exec(nai_evloop_t* l, nai_task_f cb, void* ud);

    /**
 * queue a task to the event loop to execute
 * @param   l       pointer to the event loop
 * @param   cb      pointer to the task callback
 * @param   ud      pointer to the task argument
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_evloop_queue(nai_evloop_t* l, nai_task_f cb, void* ud);

    /**
 * break the event loop with an exit code
 * @param   l       pointer to the event loop
 * @param   code    the exit code will be returned by nai_evloop_dispatch.
 *                  must be a positive number
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    the return value of nai_evloop_dispatch cannot be determined 
 *          when nai_evloop_break is repeatedly called
 */
    NAI_EXTERN
    nai_int_t nai_evloop_break(nai_evloop_t* l, int16_t code);

    /**
 * wait the event loop
 * @param   l       pointer to the event loop
 * @param   until   the flags of waiting, see @ref NAI_EV_WAIT
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    this function is used to synchronize with the dispatch thread. 
 *          when the event loop is put into other threads for execution, 
 *          concurrent access may occur when the event node is opened. 
 *          because the opening process uses nai_evnode_in_dispatch to 
 *          determine whether to perform synchronization processing, 
 *          thread safety cannot be guaranteed in this case. when the 
 *          dispatching thread has entered execution, no more concurrency 
 *          conflicts will occur.
 */
    NAI_EXTERN
    nai_int_t nai_evloop_wait(nai_evloop_t* l, nai_int_t until);

    /**
 * handling undispatched notifications
 * @param   l       pointer to the event loop
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    this function is mainly used to process unprocessed notifications 
 *          after the event dispatching exits. you should use nai_evloop_wait 
 *          to wait for the end of the dispatch before processing.
 */
    NAI_EXTERN
    nai_int_t nai_evloop_done(nai_evloop_t* l);

    /**
 * rearm the event loop, used after fork in child process
 * @param   l       pointer to the event loop
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    this function is used after fork in child process, 
 *          the purpose is to replace all resources shared with the 
 *          parent process.
 */
    NAI_EXTERN
    nai_int_t nai_evloop_rearm(nai_evloop_t* l);

    /**
 * close and free the event loop
 * @param   l       pointer to the event loop
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    this function will failed, 
 *          when the event nodes accoss with the the event loop are not cloesd
 */
    NAI_EXTERN
    nai_int_t nai_evloop_close(nai_evloop_t* l);

    //////////////////////////////////////////////////////////////////////////////
    // event node

#ifndef _NAI_TYPEDEF_EVNODE_CB_F
    #define _NAI_TYPEDEF_EVNODE_CB_F
    typedef nai_int_t (*nai_evnode_cb_f)(nai_evnode_t*, nai_int_t events);
#endif

#ifndef _NAI_TYPEDEF_EVBITS_T
    #define _NAI_TYPEDEF_EVBITS_T
    typedef union nai_evbits_s nai_evbits_t;
#endif

    /**
 * the structure for easy access @ref NAI_EV_EVENT
 */
    union nai_evbits_s
    {
        uint8_t events; /**< the value of events */
        struct
        {
            uint8_t read : 1;     /**< indicates whether it is readable */
            uint8_t write : 1;    /**< indicates whether it is writable */
            uint8_t except : 1;   /**< indicates whether it is urgent event */
            uint8_t timedout : 1; /**< indicates whether it is timedout */
            uint8_t error : 1;    /**< indicates whether an error occurred */
            uint8_t notify : 1;   /**< means a notification */
            uint8_t stage : 1;    /**< means a stage event */
            uint8_t reserved : 1; /**< reserved bits */
        };
    };

    /**
 * the structure of the event node
 */
    struct nai_evnode_s
    {
        nai_evnode_cb_f cb; /**< the callback of the event node */

        /* the status of the event node, don't modify */
        nai_evloop_t* loop;    /**< pointer to the event loop */
        nai_evloop_ent_t* ent; /**< pointer to the internal entry */
        nai_fd_t fd;           /**< the file descriptor */
        uint8_t type : 4;      /**< the file descriptor type */
        uint8_t extend : 4;    /**< the extend type */
        uint8_t extval;        /**< the extend value: nai_signal_t */

        /* the status of the event node, don't modify */
        union
        {
            struct
            {
                uint16_t seted : 4;    /**< the seted events */
                uint16_t sigbits : 5;  /**< the seted sigs before opened */
                uint16_t ttype : 1;    /**< the timer type: 0 normal,1 high-res */
                uint16_t except : 1;   /**< used for iobase */
                uint16_t catching : 1; /**< is error catching enabled on events */
                uint16_t priority : 1; /**< the priority value, 0 low, 1 high */
            };
            int16_t code; /**< the interrput code */
        } st;

        /* the key of the event node, don't modify */
        uint32_t key;

        /* the data of the event node, don't modify */
        uint64_t timeval; /**< the value of the timer */
    };

/**
 * @name    nai_evnode_defines      the functions of the event node
 * @{
 */

/**
 * test the event node is opened
 * @param   h       pointer to the event node
 * @return  a bool, indicates whether it is opened
 */
#define nai_evnode_is_opened(h) ((h)->loop != 0)

/**
 * test the caller thread is the event loop thread
 * @param   h       pointer to the event node
 * @return  a bool, indicates whether it is in the event loop thread
 * @note    this function always return 1 when it without the event loop
 */
#define nai_evnode_in_dispatch(h) ((h)->loop ? nai_evloop_in_dispatch((h)->loop) : 1)

/**
 * get the event loop associated with the event node
 * @param   h       pointer to the event node
 * @return  the address of the event loop
 */
#define nai_evnode_get_loop(h) ((nai_evloop_t*)(h)->loop)

/**
 * get the event handle of the event node
 * @param   h       pointer to the event node
 * @return  the callback of the event handle, see #nai_evnode_cb_f
 */
#define nai_evnode_get_cb(h) ((nai_evnode_cb_f)(h)->cb)

/**
 * get the file descriptor associated with the event node
 * @param   h       pointer to the event node
 * @return  the file descriptor
 */
#define nai_evnode_get_fd(h) ((nai_fd_t)(h)->fd)

/**
 * get the type of file descriptor
 * @param   h       pointer to the event node
 * @return  the type of file descriptor, see @ref NAI_FD_TYPE
 */
#define nai_evnode_get_type(h) ((nai_int_t)(h)->type)

    /** @} */

    /**
 * initial the event node
 * @param   h       pointer to the event node
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    no error should occur during initialization, and return value is 
 *          allowed not to be processed.
 */
    NAI_EXTERN
    nai_int_t nai_evnode_init(nai_evnode_t* h);

    /**
 * set the callback of the event node
 * @param   h       pointer to the event node
 * @param   cb      the callback of the event node, see #nai_evnode_cb_f
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_evnode_set_cb(nai_evnode_t* h, nai_evnode_cb_f cb);

    /**
 * binding a file descriptor with the event node
 * @param   h       pointer to the event node
 * @param   fd      the file descriptor which to bind with the event node
 * @param   type    the type of file descriptor, see @ref NAI_FD_TYPE
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_evnode_set_fd(nai_evnode_t* h, nai_fd_t fd, nai_int_t type);

    /**
 * set the interset events
 * @param   h       pointer to the event node
 * @param   op      the operation of event, see @ref NAI_EV_OP
 * @param   events  the value of events, accept #NAI_EV_IOE only
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_evnode_set_event(nai_evnode_t* h, nai_int_t op, nai_int_t events);

    /**
 * set up error catching
 * @param   h       pointer to the event node
 * @param   on      indicates whether error catching is enabled on event
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    this function can be called before open the event node
 * @note    this function can be called in the event loop thread only
 * @note    use to catch the connection error and post an error event.
 * @note    there is a performance overhead and 
 *          should be used to catch errors during connecting.
 */
    NAI_EXTERN
    nai_int_t nai_evnode_set_catching(nai_evnode_t* h, nai_int_t on);

    /**
 * set the priority of event node
 * @param   h       pointer to the event node
 * @param   pri     the value of priority, 0 is low, 1 is high
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    this function can be called before open the event node
 * @note    this function can be called in the event loop thread only
 * @note    the event loop priority process the event of event node 
 *          which is set with high priority
 */
    NAI_EXTERN
    nai_int_t nai_evnode_set_priority(nai_evnode_t* h, nai_int_t pri);

    /**
 * set an absolute user timer
 * @param   h       pointer to the event node
 * @param   msec    the value of absolute tick, in milli-seconds, 0 to disable
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    this function can be called in any thread, but not thread-safe.
 * @note    this function can be called before open the event node
 */
    NAI_EXTERN
    nai_int_t nai_evnode_set_expire(nai_evnode_t* h, uint64_t msec);

    /**
 * set a relative user timer
 * @param   h       pointer to the event node
 * @param   op      the operation of timeout, see @ref NAI_TIMEOP
 * @param   msec    the value of timeout, a positive number, -1 to disable
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    this function can be called in any thread, but not thread-safe.
 * @note    this function can be called before open the event node
 */
    NAI_EXTERN
    nai_int_t nai_evnode_set_timeout(nai_evnode_t* h, nai_int_t op, int32_t msec);

    /**
 * post a uesr signal to the event node
 * @param   h       pointer to the event node
 * @param   sigid   the id of the user signal, the value range is 0-3
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    this function is thread-safe, can be called in any thread.
 * @note    this function can be called before open the event node
 * @note    the callback will be called with event #NAI_EV_NOTIFY
 *          use #nai_ev_notify_code to extract 'sigid'
 * @note    repeatedly sending the same signal will not generate additional 
 *          events until the signal is processed
 */
    NAI_EXTERN
    nai_int_t nai_evnode_post(nai_evnode_t* h, nai_int_t sigid);

    /**
 * bind an user signal to the mail slot to receive signal
 * @param   h       pointer to the event node
 * @param   slot    the id of the mail slot
 * @param   sigid   the id of the user signal to bind, -1 to unbind
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    this function is thread-safe, can be called in any thread.
 * @note    this function must be called after open the event node
 * @note    the callback will be called with event #NAI_EV_NOTIFY
 *          use #nai_ev_notify_code to extract 'sigid'
 */
    NAI_EXTERN
    nai_int_t nai_evnode_join(nai_evnode_t* h, nai_int_t slot, nai_int_t sigid);

    /**
 * add the event node to the event loop
 * @param   h       pointer to the event node
 * @param   l       pointer to the event loop
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_evnode_open(nai_evnode_t* h, nai_evloop_t* l);

    /**
 * close the event node
 * @param   h       pointer to the event node
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    if the event node is opened, 
 *          this function must be called in event loop thread
 */
    NAI_EXTERN
    nai_int_t nai_evnode_close(nai_evnode_t* h);

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
