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
/// @file       nai_task.h
/// @brief      the task pool
/// @details
/// @date       2023-09-23
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


/**
 * @details this implementation is a task pool with fixed threads
 *
 * @details the code example is as follows:
 *
 * @par     open the task pool
 * @code
 *          nai_int_t r;
 *          nai_int_t threads;
 *          nai_task_pool_t t;
 *          nai_thread_att_t* a;
 *
 *          nai_task_pool_init(&t, 0);
 *
 *          r = nai_task_pool_open(&t, threads, a);
 *          if (r < 0) {
 *              goto _fail;
 *          };
 * @endcode
 *
 * @par     implement a task
 * @code
 *          typedef struct mu_args_s {
 *              nai_task_pool_t* t;
 *              void* ud;
 *          } my_args_t;
 *
 *          nai_int_t my_task(void* p)
 *          {
 *              my_args_t* a = (my_args_t*)p;
 *
 *              // do work
 *              ...
 *
 *              // notify
 *              nai_task_pool_complete(a->t);
 *          };
 * @endcode
 *
 * @par     queue a new task and wait to completed
 * @code
 *          nai_int_t r;
 *          nai_task_pool_t* t;
 *          void* ud;
 *          my_args a;
 *
 *          a.t = t;
 *          a.ud = ud;
 *
 *          r = nai_task_pool_queue(t, my_task, ud, 1);
 *          if (r < 0) {
 *              goto _fail;
 *          };
 * @endcode
 */

#ifndef _TASK_H_NAI
#define _TASK_H_NAI

#pragma once

#include "nai/os/nai_thread.h"
#include "nai/runtime/nai_array.h"
#include "nai/runtime/nai_pool.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#ifndef _NAI_TYPEDEF_TASK_T
    #define _NAI_TYPEDEF_TASK_T
    typedef struct nai_task_s nai_task_t;
#endif

#ifndef _NAI_TYPEDEF_TASK_POOL_T
    #define _NAI_TYPEDEF_TASK_POOL_T
    typedef struct nai_task_pool_s nai_task_pool_t;
#endif

/**
 * the task callback
 */
#ifndef _NAI_TYPEDEF_TASK_F
    #define _NAI_TYPEDEF_TASK_F
    typedef nai_int_t (*nai_task_f)(void* args);
#endif

/**
 * the task interruption helper
 */
#ifndef _NAI_TYPEDEF_TASK_BREAK_F
    #define _NAI_TYPEDEF_TASK_BREAK_F
    typedef nai_int_t (*nai_task_break_f)(nai_task_t* t, nai_thread_t tid);
#endif

    /**
 * the sturcture of the task
 */
    struct nai_task_s
    {
        void* data; /**< pointer to the internal data */
        struct
        {
            uint32_t key : 30;       /**< the cancel key */
            uint32_t canceling : 1;  /**< is canceling */
            uint32_t cancelable : 1; /**< is cancelable */
        };
    };

/**
 * initial the task key
 * @param   t       pointer to the task
 * @param   c       the mark of cancelable
 * @return  void
 */
#define nai_task_init(t, c)                                                                                            \
    {                                                                                                                  \
        (t)->data       = 0;                                                                                           \
        (t)->key        = 0;                                                                                           \
        (t)->cancelable = !!(c);                                                                                       \
    }

/**
 * test whether the task is canceling
 * @param   t       pointer to the task
 * @return  if it is canceling, return 1, otherwise return 0
 */
#define nai_task_is_canceling(t) ((t)->canceling)

    /**
 * waiting the task
 * @param   t       pointer to the task
 * @param   msec    the value of timeout, a positive number, -1 is infinite
 * @retval  0       on success
 * @retval  <0      an error code returned, see @ref NAI_SULT
 */
    NAI_EXTERN
    nai_int_t nai_task_wait(nai_task_t* t, int32_t msec);

    /**
 * cancel the task
 * @param   t       pointer to the task
 * @param   fn      pointer to the task interruption helper
 * @retval  0       on success
 * @retval  <0      an error code returned, see @ref NAI_SULT
 * @note    if the task is in execution, the task interruption helper will 
 *          be called and ensure synchronization through the mutex of 
 *          the task pool, being careful not to cause deadlocks.
 * @note    if the task is in execution, 
 *          the cancel operation will fail and set nai_errno to EINPROCESS.
 * @note    if the task has canceled, 
 *          the cancel operation will fail and set nai_errno to ECANCELED.
 * @note    if the task has ended, 
 *          the cancel operation will fail and set nai_errno to ENOENT.
 */
    NAI_EXTERN
    nai_int_t nai_task_cancel(nai_task_t* t, nai_task_break_f fn);

    /**
 * the structure of the task pool
 */
    struct nai_task_pool_s
    {
        nai_array_t threads;    /**< the array of the backend-threads */
        nai_mutex_t lock;       /**< the mutex */
        nai_cond_t cond;        /**< the condition for queue task */
        nai_cond_t complete;    /**< the condition for waiting tasks */
        nai_fixedpool_t pool;   /**< the fixed pool of task */
        nai_list_entry_t queue; /**< the queue of pending task */
        nai_list_entry_t wait;  /**< the queue of waiting task */
        nai_atomic32_t next;    /**< the next key */
        nai_int_t count;        /**< the count of queued task */
        nai_int_t max_count;    /**< the max of queued task */
        nai_int_t arg_size;     /**< the size of argument */

        union
        {
            struct
            {
                uint32_t wall : 16;  /**< the count of waiting */
                uint32_t ending : 1; /**< the ending mark */
            };
            uint32_t flags;
        };
    };

    /**
 * initial the task pool
 * @param   p       pointer to the task pool
 * @param   arg_size the size of argument which to copy
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    no error should occur during initialization, and return value is 
 *          allowed not to be processed.
 */
    NAI_EXTERN
    nai_int_t nai_task_pool_init(nai_task_pool_t* p, nai_int_t arg_size);

    /**
 * open the task pool
 * @param   p       pointer to the task pool
 * @param   threads the number of the backend-threads
 * @param   a       pointer to the thread attributes, can be null
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_task_pool_open(nai_task_pool_t* p, nai_int_t threads, const nai_thread_attr_t* a);

    /**
 * get the count of the queued tasks
 * @param   p       pointer to the task pool
 * @return  the number of queued tasks
 */
    NAI_EXTERN
    nai_int_t nai_task_pool_get_queued(nai_task_pool_t* p);

    /**
 * get the max of the queued tasks
 * @param   p       pointer to the task pool
 * @return  the value of max queued tasks
 * @note    this function can be called before open the task pool
 */
    NAI_EXTERN
    nai_int_t nai_task_pool_get_max_queued(nai_task_pool_t* p);

    /**
 * set the max of the queued tasks
 * @param   p       pointer to the task pool
 * @param   max_queued the value of max queued tasks
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    this function can be called before open the task pool
 */
    NAI_EXTERN
    nai_int_t nai_task_pool_set_max_queued(nai_task_pool_t* p, nai_int_t max_queued);

    /**
 * queue a new task to the task pool
 * @param   p       pointer to the task pool
 * @param   entry   the task callback
 * @param   args    pointer to the task argument
 * @param   wait    whether to wait for the task execution to complete
 * @param   t       pointer to return the task information, optional
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_task_pool_queue(nai_task_pool_t* p, nai_task_f entry, void* args, nai_int_t wait, nai_task_t* t);

    /**
 * queue a new task to the task pool and copy the argument of the task
 * @param   p       pointer to the task pool
 * @param   entry   the task callback
 * @param   args    pointer to the task argument
 * @param   wait    whether to wait for the task execution to complete
 * @param   t       pointer to return the task information, optional
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_task_pool_queue_copy(nai_task_pool_t* p, nai_task_f entry, void* args, nai_int_t wait, nai_task_t* t);

    /**
 * notify the caller that the task is complete
 * @param   p       pointer to the task pool
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    this function should be used in the backend-thread
 */
    NAI_EXTERN
    nai_int_t nai_task_pool_complete(nai_task_pool_t* p);

    /**
 * waiting all task to completed
 * @param   p       pointer to the task pool
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_task_pool_wait(nai_task_pool_t* p);

    /**
 * close the task pool
 * @param   p       pointer to the task pool
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    NAI_EXTERN
    nai_int_t nai_task_pool_close(nai_task_pool_t* p);

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
