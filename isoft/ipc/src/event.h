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
/// @file       event.h
/// @brief
/// @details
/// @date       2021-11-18
/// @author     gaohuiming
/// @version    1.2.0
///
/// ================================================================

#ifndef _EVENT_H_IPC
#define _EVENT_H_IPC

#include <assert.h>
#include <errno.h>
#include <stdatomic.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#if defined(IPC_EVNODE_DEBUG)
    #include <stdio.h>
#endif

#include <isoft/ipc/packet.h>
#include <nai/io/nai_event.h>

#include "packet.h"
#include "tree.h"
#include "utility.h"

typedef struct
{
    void* context;
    ipc_packet_t* (*peek_message)(void* context);
    void (*on_sent)(void* context, ipc_packet_t* message);
    void (*on_received)(void* context, ipc_packet_t* message);
    void (*on_error)(void* context, ipc_packet_t* message);
} ipc_event_manager_t;

typedef ipc_event_manager_t ipc_channel_manager_t;

/*
 * @brief Initialize the runtime.
 * @param event_loop the handle of a libnai event loop
 * @param manager the handle of a channel manager
 * @return 0 on success, -1 on failure with errno set
 */

int ipc_event_startup(nai_evloop_t* event_loop, ipc_event_manager_t* manager);
int ipc_event_notify(void);
int ipc_event_close(void);
int ipc_event_cleanup(void);

static inline int ipc_channel_startup(nai_evloop_t* event_loop, ipc_channel_manager_t* manager)
{
    ipc_event_manager_t* real_manager;
    real_manager = (ipc_event_manager_t*)manager;
    return ipc_event_startup(event_loop, real_manager);
}
static inline int ipc_channel_notify(void) { return ipc_event_notify(); }
static inline int ipc_channel_close(void) { return ipc_event_close(); }
static inline int ipc_channel_cleanup(void) { return ipc_event_cleanup(); }

#if defined(IPC_EVNODE_DEBUG)
    #define pr_debug(fmt, ...) printf("IPC " fmt, ##__VA_ARGS__)

static inline void ipc_init_dump(int status, int fd, const char* log)
{
    if (fd > 0) {
        pr_debug("[%s] status(%d), fd(%d)\n", log, status, fd);
    } else {
        pr_debug("[%s] status(%d)\n", log, status);
    }
}

static inline void ipc_init_counter_dump(int counter, const char* log)
{
    pr_debug("[%s] init_counter(%d)\n", log, counter);
}

static inline void ipc_evnode_dump(nai_evloop_t* event_loop, nai_evnode_t* evnode, const char* log)
{
    pr_debug("[%s] event_loop(%p), evnode_get_loop(%p)\n", log, (void*)event_loop, (void*)nai_evnode_get_loop(evnode));
    pr_debug("[%s] evnode_is_opened(%d), evnode_in_dispatch(%d), evnode_get_fd(%d)\n", log,
             nai_evnode_is_opened(evnode), nai_evnode_in_dispatch(evnode), nai_evnode_get_fd(evnode));
}
#else
static inline void ipc_init_dump(int status, int fd, const char* log)
{
    (void)status;
    (void)fd;
    (void)log;
}

static inline void ipc_init_counter_dump(int counter, const char* log)
{
    (void)counter;
    (void)log;
}

static inline void ipc_evnode_dump(nai_evloop_t* event_loop, nai_evnode_t* evnode, const char* log)
{
    (void)event_loop;
    (void)evnode;
    (void)log;
}
#endif

#endif
