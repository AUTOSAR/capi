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
/// @file       buffer.h
/// @brief
/// @details
/// @date       2022-09-20
/// @author     yangtao
/// @version    1.2.0
///
/// ================================================================

#ifndef IPC_BUFFER_PUBLIC_HEADER
#define IPC_BUFFER_PUBLIC_HEADER

#include <isoft/ipc/packet.h>
#include <nai/io/nai_buf.h>

// ipc_buffer_s is nai_buf_t
struct __attribute__((__unused__)) ipc_buffer_t
{
    void *no_used;
};

#define IPC_GLOBAL_PACKET_MEMPOOL_MAX_SIZE 1 << 24

int ipc_buffer_init_pool(nai_bufpool_t *pool, size_t size);

int ipc_buffer_from_pool(nai_bufpool_t *pool, nai_pool_t *parent);

int ipc_buffer_close_pool(nai_bufpool_t *pool);

ipc_buffer_t *ipc_buffer_alloc(nai_bufpool_t *pool, size_t len);

ipc_buffer_t *ipc_buffer_alloc_from_user(nai_bufpool_t *pool, void *userdata, size_t len, ipc_free_func_t free);

ipc_buffer_t *ipc_buffer_dup(nai_bufpool_t *pool, ipc_buffer_t *buf);

int ipc_buffer_init_list(nai_buflist_t *list, nai_bufpool_t *pool);

ipc_buffer_t *ipc_buffer_get_buffer(nai_buflist_t *list);

ipc_buffer_t *ipc_buffer_append(nai_buflist_t *list, ipc_buffer_t *buf);

int ipc_buffer_close_list(nai_buflist_t *list);

int ipc_buffer_close(ipc_buffer_t *buf);

#endif  // IPC_BUFFER_PUBLIC_HEADER
