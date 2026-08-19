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
/// @file       nai_evloop.h
/// @brief      
/// @details
/// @date       2020-11-28
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


#ifndef _EVLOOP_H_NAI
#define _EVLOOP_H_NAI

#pragma once

#include "nai/os/nai_thread.h"
#include "nai/runtime/nai_list.h"
#include "nai/runtime/nai_pool.h"
#include "nai/runtime/nai_rbtree.h"
#include "nai_evbase.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    //////////////////////////////////////////////////////////////////////////////
    // time cache

#define NAI_EV_TIME_UPDATE_FREQ 64 /* the freq of time cache update */
#define NAI_EV_TIME_MAX_ERROR   3  /* the max error of timer */

    typedef struct nai_timedata_s nai_timedata_t;
    typedef struct nai_timecache_s nai_timecache_t;

    /**
 * the structure of the time data
 */
    struct nai_timedata_s
    {
        uint64_t utime; /**< the cache of time, in micro-second */
        uint64_t utick; /**< the cache of tick, in micro-second */
        uint64_t mtick; /**< the cache of tick, in milli-second */
    };

    /**
 * the structure of the time data cache
 */
    struct nai_timecache_s
    {
        nai_int_t access;     /**< the count of call get time */
        nai_int_t slot_data;  /**< the slot of current times */
        nai_int_t slot_fmts;  /**< the slot of current formats */
        nai_atomic32_t lock;  /**< the lock to sync */
        nai_timedata_t* data; /**< the current times */
        nai_timefmts_t* fmts; /**< the current formats */
    };

    extern nai_timecache_t nai_timecache;
    extern void nai_timecache_update_impl();

#define nai_timecache_get_utime() ((uint64_t)nai_timecache.data->utime)

#define nai_timecache_get_utick() ((uint64_t)nai_timecache.data->utick)

#define nai_timecache_get_mtick() ((uint64_t)nai_timecache.data->mtick)

#define nai_timecache_update_i()                                                                                       \
    {                                                                                                                  \
        nai_timecache_update_impl();                                                                                   \
        nai_memory_barrier();                                                                                          \
        nai_timecache.access = 0;                                                                                      \
    }

#define nai_timecache_access()                                                                                         \
    {                                                                                                                  \
        nai_timecache.access++;                                                                                        \
        if (nai_timecache.access > NAI_EV_TIME_UPDATE_FREQ)                                                            \
            nai_timecache_update_i();                                                                                  \
    }

//////////////////////////////////////////////////////////////////////////////
// the event loop

/**
 * @anchor  NAI_EV_SIG_INDEX
 * @name    NAI_EV_SIG_INDEX        the index of the iobase signal
 * @{
 */
#define NAI_EV_SIG_TIMER    0
#define NAI_EV_SIG_ADD      1
#define NAI_EV_SIG_NOTIFY   2
#define NAI_EV_SIG_NOTIFY_0 2
#define NAI_EV_SIG_NOTIFY_1 3
#define NAI_EV_SIG_NOTIFY_2 4
#define NAI_EV_SIG_NOTIFY_3 5
#define NAI_EV_SIG_INTERNAL 6
#define NAI_EV_SIG_INTR     7
#define NAI_EV_SIG_MAX      8
    /** @} */

#define NAI_EV_NOTIFY_INTERNAL 4

#ifndef _NAI_TYPEDEF_EVTIMER_T
    #define _NAI_TYPEDEF_EVTIMER_T
    typedef struct nai_evtimer_s nai_evtimer_t;
#endif

    typedef struct nai_timenode_s nai_timenode_t;
    typedef struct nai_timequeue_s nai_timequeue_t;
    typedef struct nai_evloop_sigqueue_s nai_evloop_sigqueue_t;
    typedef struct nai_evloop_watch_s nai_evloop_watch_t;
    typedef struct nai_evloop_task_s nai_evloop_task_t;
    typedef struct nai_evloop_func_s nai_evloop_func_t;
    typedef struct nai_evloop_link_s nai_evloop_link_t;
    typedef struct nai_evloop_slot_s nai_evloop_slot_t;
    typedef struct nai_evloop_slot_s nai_evloop_mail_t;

    /**
 * the structure of the time node
 */
    struct nai_timenode_s
    {
        nai_rbnode_t node; /**< the tree node of timer */
        uint64_t value;    /**< the value of time */
    };

    /**
 * the structure of the time queue
 */
    struct nai_timequeue_s
    {
        nai_rbtree_t root; /**< the tree of timers */
    };

    /**
 * the structure of the signal queue of the event loop
 */
    struct nai_evloop_sigqueue_s
    {
        uint32_t count;         /**< count of signal queue */
        nai_evloop_ent_t* list; /**< signal queue need process immediately */
    };

    /**
 * the structure of the event loop watch
 */
    struct nai_evloop_watch_s
    {
        nai_list_entry_t ent; /**< the list entry */
        nai_evnode_t* h;      /**< pointer to the event node */
        nai_int_t stage;      /**< the stage name */
        nai_int_t locked;     /**< is locked by process */
    };

    /**
 * the structure of the event loop task
 */
    struct nai_evloop_task_s
    {
        nai_list_entry_t ent; /**< the list entry */
        nai_task_f cb;        /**< pointer to the task callback */
        void* ud;             /**< pointer to the task argument */
    };

    /**
 * the structure of the event loop func
 */
    struct nai_evloop_func_s
    {
        nai_evnode_cb_f cb;      /**< the callback of the event node */
        nai_evloop_link_t* link; /**< pointer to the event link */
        nai_task_f func;         /**< pointer to the task callback */
        void* ud;                /**< pointer to the task argument */
    };

    /**
 * the structure of the event loop link between slot and mail
 */
    struct nai_evloop_link_s
    {
        nai_list_entry_t ents;   /**< the list entry of slot */
        nai_list_entry_t entm;   /**< the list entry of mail */
        nai_evloop_slot_t* slot; /**< pointer to the source slot */
        nai_evloop_ent_t* dest;  /**< pointer to the dest */
        nai_int_t sig;           /**< the notify id */
        uint8_t type;            /**< the dest type, 0 signal, 1 task */
    };

    /**
 * the structure of the event loop event slot
 */
    struct nai_evloop_slot_s
    {
        nai_rbnode_t node;

        /* key */
        union
        {
            intptr_t key; /**< the key of the slot */
            void* keyp;   /**< the key of the mail */
        };

        /* value */
        uint32_t temp : 1;  /**< is a temporary slot */
        nai_list_t list;    /**< the list of link between slot and mail */
        nai_evloop_t* loop; /**< pointer to the event loop */
    };

    /**
 * the structure of the event loop entry
 * the size of nai_evloop_ent_t is 64, 
 * the size is equal with cacheline size on 64bit os
 */
    struct nai_evloop_ent_s
    {
        nai_evloop_ent_t* next;    /**< pointer to next process ent */
        nai_evloop_ent_t* signext; /**< pointer to next signal */
        nai_evnode_t* h;           /**< pointer to handle */
        int16_t key;               /**< used by event implemnent */
        struct {
            uint16_t sig : 15;         /**< the signals bit vector */
            uint16_t mail : 1;         /**< is with mailslot */
        };
        struct {
            uint32_t events : 28;      /**< the incoming events and errno */
            uint32_t mt : 1;           /**< is allocated from pool(0:st 1:mt) */
            uint32_t ttype : 1;        /**< the timer type: 0 normal,1 high-res */
            uint32_t catching : 1;     /**< is enabled error catching */
            uint32_t priority : 1;     /**< the process priority, 0 low, 1 high */
        };
        nai_timenode_t timer;      /**< the time queue node */
    };

    /**
 * the structure of the event loop
 */
    struct nai_evloop_s
    {
        nai_evbase_t ev;        /**< the event base */
        nai_evnode_t post;      /**< the event node for post internal event */
        nai_evtimer_t* timer;   /**< pointer to the high resolution timer */
        nai_spin_t lock;        /**< the lock for concurrent queue */
        nai_fixedpool_t pools;  /**< the loop mem pool for signal-thread */
        nai_fixedpool_t poolm;  /**< the loop mem pool for multi-thread */
        nai_fixedpool_t pooles; /**< the extra mem pool for single-thread */
        nai_fixedpool_t poolem; /**< the extra mem pool for multi-thread */
        nai_fixedpool_t poolus; /**< the ultra mem pool for single-thread */
        nai_fixedpool_t poolum; /**< the ultra mem pool for multi-thread */

        nai_mutex_t mux;        /**< the wait lock */
        nai_cond_t cond;        /**< the wait condition */
        nai_int_t run;          /**< the run count */
        nai_int_t run_last;     /**< the last recorded run count */
        nai_int_t wait_running; /**< the count of threads waiting for running */
        nai_int_t wait_break;   /**< the count of threads waiting for exit */
        uint32_t closing : 1;   /**< is closing */
        uint32_t backwork : 1;  /**< is backwork available */
        nai_atomic32_t ecount;  /**< the count of the event node */
        nai_atomic32_t pcount;  /**< the count of the event port */
        nai_tid_t tid;          /**< the dispatch thread id */

        nai_int_t timechg[2];       /**< is time queue changed */
        uint64_t timenext[2];       /**< the next wakeup time */
        uint64_t timeomax;          /**< the max timeout value */
        nai_timequeue_t timeq[2];   /**< the time queue */
        nai_evloop_sigqueue_t sigq; /**< the signal queue */
        nai_list_t taskq;           /**< the task queue */
        nai_list_t stages[2];       /**< the list of stages */
        nai_list_t watchs;          /**< the list of incoming watchs */
        nai_list_t setups;          /**< the list of setuping watchs */
        nai_list_t ports;           /**< the list of event ports */

        nai_int_t nexts;        /**< the next slot id */
        nai_rbtree_t slots;     /**< the map of slot */
        nai_rbtree_t mails;     /**< the map of mail */
        nai_evloop_mail_t mail; /**< the global mail */

        nai_int_t threads_refs;   /**< the refcount of backend io threads */
        nai_task_pool_t* threads; /**< the backend io threads */

        nai_int_t fsize;    /**< the size of fd array */
        nai_int_t fcount;   /**< the count of fd in the fd array */
        nai_evnode_t** fds; /**< pointer to the fd array */
    };

    /**
 * add a watch handler into the event loop
 * @param   l       pointer to the event loop
 * @param   h       pointer to the event node
 * @param   name    the stage name to watch, see @ref NAI_EV_STAGE_NAME.
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_evloop_add_watch(nai_evloop_t* l, nai_evnode_t* h, nai_int_t name);

    /**
 * delete the watch handler from the event loop
 * @param   l       pointer to the event loop
 * @param   h       pointer to the event node
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_evloop_del_watch(nai_evloop_t* l, nai_evnode_t* h);

    /**
 * add an event node into the event loop
 * @param   l       pointer to the event loop
 * @param   h       pointer to the event node
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_evloop_add_handle(nai_evloop_t* l, nai_evnode_t* h);

    /**
 * set the event of the event node
 * @param   l       pointer to the event loop
 * @param   h       pointer to the event node
 * @param   events  the value of events, accept #NAI_EV_IOE and #NAI_EV_ASYNC
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_evloop_set_handle(nai_evloop_t* l, nai_evnode_t* h, nai_int_t events);

    /**
 * delete the event node from the event loop
 * @param   l       pointer to the event loop
 * @param   h       pointer to the event node
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_evloop_del_handle(nai_evloop_t* l, nai_evnode_t* h);

    /**
 * send an user signal to the event node
 * @param   l       pointer to the event loop
 * @param   h       pointer to the event node
 * @param   sig     the id of the user signal, the value range is 0-3
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_evloop_set_signal(nai_evloop_t* l, nai_evnode_t* h, nai_int_t sig);

    /**
 * send the user signals to the event node
 * @param   l       pointer to the event loop
 * @param   h       pointer to the event node
 * @param   sigbits the mask of the user signals
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_evloop_set_signals(nai_evloop_t* l, nai_evnode_t* h, nai_int_t sigbits);

    /**
 * set an absolute user timer
 * @param   l       pointer to the event loop
 * @param   h       pointer to the event node
 * @param   usec    the value of absolute tick, in micro-seconds, 0 to disable
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_evloop_set_expire(nai_evloop_t* l, nai_evnode_t* h, uint64_t usec);

    /**
 * set a relative user timer
 * @param   l       pointer to the event loop
 * @param   h       pointer to the event node
 * @param   op      the operation of timeout, see @ref NAI_TIMEOP
 * @param   usec    the value of timeout, a positive number, -1 to disable
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_evloop_set_timeout(nai_evloop_t* l, nai_evnode_t* h, nai_int_t op, int64_t usec);

    /**
 * bind an user signal to the mail slot
 * @param   l       pointer to the event loop
 * @param   h       pointer to the event node
 * @param   slot    the id of the mail slot
 * @param   sig     the id of the signal to bind
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_evloop_join_slot(nai_evloop_t* l, nai_evnode_t* h, nai_int_t slot, nai_int_t sig);

    /**
 * unbind an user signal from the mail slot
 * @param   l       pointer to the event loop
 * @param   h       pointer to the event node
 * @param   slot    the id of the mail slot
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    nai_int_t nai_evloop_part_slot(nai_evloop_t* l, nai_evnode_t* h, nai_int_t slot);

//////////////////////////////////////////////////////////////////////////////
// extend of event node

/**
 * @anchor  NAI_EV_EXTEND_TYPE
 * @name    NAI_EV_EXTEND_TYPE  the type of the event node extend
 * @{
 */
#define NAI_EV_EXTEND_NONE   0x0
#define NAI_EV_EXTEND_SIGNAL 0x1
#define NAI_EV_EXTEND_WATCH  0x2
#define NAI_EV_EXTEND_TIMER  0x3
    /** @} */

    typedef nai_int_t (*nai_evnode_ext_close)(nai_evnode_t* e);

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
