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
/// @file       nai_fs_watch.h
/// @brief      the filesystem watcher
/// @details
/// @date       2023-10-29
/// @author     xn
/// @version    1.2.0
///
/// ================================================================


/**
 */

#ifndef _FS_WATCH_H_NAI
#define _FS_WATCH_H_NAI

#pragma once

#include "nai/io/nai_event.h"
#include "nai/runtime/nai_string.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#if defined(_WIN32)
    #define NAI_FS_WATCH_USE_WIN32 1
#elif (NAI_HAVE_SYS_INOTIFY_H)
    #define NAI_FS_WATCH_USE_LINUX 1
#elif (NAI_HAVE_KQUEUE)
    #define NAI_FS_WATCH_USE_KQUEUE 1
#else
    #define NAI_FS_WATCH_USE_POLL 1
    /* the max value of poll interval time */
    #define NAI_FS_POLL_INTERVAL  1000
#endif

#if (NAI_FS_WATCH_USE_WIN32)
    #define NAI_HAVE_FS_WATCH       1
    #define NAI_HAVE_FS_WATCH_CHILD 1
#elif (NAI_FS_WATCH_USE_LINUX)
    #define NAI_HAVE_FS_WATCH       1
    #define NAI_HAVE_FS_WATCH_CHILD 1
#elif (NAI_FS_WATCH_USE_KQUEUE)
    #define NAI_HAVE_FS_WATCH 1
#elif (NAI_FS_WATCH_USE_POLL)
    #define NAI_HAVE_FS_WATCH 1
#endif

/**
 * @anchor  NAI_FS_EVENT
 * @name    NAI_FS_EVENT        the flags of the fs event
 * @{
 */
#define NAI_FS_CHANGE   NAI_EV_READ
#define NAI_FS_MOVE_DEL NAI_EV_WRITE
#define NAI_FS_MOVE_ADD NAI_EV_SPECIFIC
/** @} */

/**
 * @anchor  NAI_FS_WATCH
 * @name    NAI_FS_WATCH        the open flags of the fs watch handler
 * @{
 */
#define NAI_FS_WATCH_DEFAULT 0x0 /**< watch the file node */
#define NAI_FS_WATCH_CHILD   0x1 /**< watch the file node and childs */
#define NAI_FS_WATCH_SYMLINK 0x2 /**< watch the symlink itself */
#define NAI_FS_WATCH_PATH    0x4 /**< watch the specified path */
    /** @} */

#ifndef _NAI_TYPEDEF_FS_WATCH_T
    #define _NAI_TYPEDEF_FS_WATCH_T
    typedef struct nai_fs_watch_s nai_fs_watch_t;
#endif
#ifndef _NAI_TYPEDEF_FS_WATCH_CB_F
    #define _NAI_TYPEDEF_FS_WATCH_CB_F
    typedef nai_int_t (*nai_fs_watch_cb_f)(nai_fs_watch_t* e, nai_int_t event, const char* child);
#endif

    /**
 * the structure of the fs watch handler
 */
    struct nai_fs_watch_s
    {
        nai_evnode_t ev;      /**< the event node */
        nai_fs_watch_cb_f cb; /**< the callback of the fs event */
        nai_str_t path;       /**< the string view of the watching path */
        nai_int_t flags;      /**< the open flags */
    };

/**
 * test the fs watch handler is opened
 * @param   w       pointer to the fs watch handler
 * @return  a bool, indicates whether it is opened
 */
#define nai_fs_watch_is_opened(w) nai_evnode_is_opened(&(w)->ev)

/**
 * test the caller thread is the event loop thread
 * @param   w       pointer to the fs watch handler
 * @return  a bool, indicates whether it is in the event loop thread
 */
#define nai_fs_watch_in_dispatch(w) nai_evnode_in_dispatch(&(w)->ev)

/**
 * get the event loop associated with the fs watch handler
 * @param   w       pointer to the fs watch handler
 * @return  the address of the event loop
 */
#define nai_fs_watch_get_loop(w) nai_evnode_get_loop(&(w)->ev)

/**
 * set the callback of the fs watch handler
 * @param   w       pointer to the fs watch handler
 * @param   c       the callback of the fs event
 * @return  void
 */
#define nai_fs_watch_set_cb(w, c)                                                                                      \
    {                                                                                                                  \
        (w)->cb = (c);                                                                                                 \
    }

/**
 * send a uesr signal to the fs watch handler
 * @param   w       pointer to the fs watch handler
 * @param   sigid   the id of the user signal
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    this function is thread-safe, can be called in any thread.
 * @note    this function can be called before open the watch handler
 * @note    the user callback will be called with event #NAI_EV_NOTIFY
 *          use #nai_ev_notify_code to extract 'sigid'
 */
#define nai_fs_watch_post(w, sigid) nai_evnode_post(&(w)->ev, (sigid))

    /**
 * initial the fs watch handler
 * @param   w       pointer to the fs watch handler
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    no error should occur during initialization, and return value is 
 *          allowed not to be processed.
 */
    NAI_EXTERN
    nai_int_t nai_fs_watch_init(nai_fs_watch_t* w);

    /**
 * open the fs watch handler
 * @param   w       pointer to the fs watch handler
 * @param   loop    pointer to the event loop
 * @param   path    pointer to the watching path
 * @param   flags   the open flags, see @ref NAI_FS_WATCH.
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    the user callback be called with event @ref NAI_FS_EVENT 
 *          when the watching file is changed
 */
    NAI_EXTERN
    nai_int_t nai_fs_watch_open(nai_fs_watch_t* w, nai_evloop_t* loop, const char* path, nai_int_t flags);

    /**
 * close the fs watch handler
 * @param   w       pointer to the fs watch handler
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    if the fs watch handler is opened, 
 *          this function must be called in event loop thread
 */
    NAI_EXTERN
    nai_int_t nai_fs_watch_close(nai_fs_watch_t* w);

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
