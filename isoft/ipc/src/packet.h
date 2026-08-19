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
/// @file       packet.h
/// @brief
/// @details
/// @date       2022-09-20
/// @author     yangtao
/// @version    1.2.0
///
/// ================================================================

#ifndef IPC_INNER_PACKET_H
#define IPC_INNER_PACKET_H

#include <errno.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#if defined(IPC_PACKET_DEBUG)
    #include <stdio.h>
#endif

#include <isoft/ipc/packet.h>
#include <nai/runtime/nai_list.h>
#include <nai/runtime/nai_pool.h>

#include "buffer.h"
#include "peer.h"
#include "tree.h"
#include "utility.h"

enum
{
    IPC_PACKET_KIND_NULL = 0,
    IPC_PACKET_KIND_CLIENT_CREATED,
    IPC_PACKET_KIND_CLIENT_PASSED,
    IPC_PACKET_KIND_CLIENT_RECEIVED,
    IPC_PACKET_KIND_SERVER_CREATED,
    IPC_PACKET_KIND_SERVER_PASSED,
    IPC_PACKET_KIND_SERVER_RECEIVED,
};

enum
{
    IPC_MESSAGE_TYPE_NULL = 0,
    IPC_MESSAGE_REQUEST_POST,
    IPC_MESSAGE_REQUEST_CALL,
    IPC_MESSAGE_REQUEST_MULTI_REPLY,
    IPC_MESSAGE_RESPONSE,
    IPC_MESSAGE_RESPONSE_COMPLETE,
    IPC_MESSAGE_CONNECTING,
    IPC_MESSAGE_CONNECTED,
    IPC_MESSAGE_DISCONNECT,
    IPC_MESSAGE_EM_BROADCAST,
};

typedef struct ipc_pktpool_s
{
    ipc_spinlock_t *lock;
    nai_list_entry_t list; /**< the list of free packet */
    size_t size;
    size_t alloc;
} ipc_pktpool_t;

struct ipc_packet_s
{
    nai_list_entry_t ent;
    RB_ENTRY(ipc_packet_s) node;

    atomic_uintptr_t ref_count;
    ipc_packet_t *ref_packet;

    uint64_t seq;
    ipc_peer_t *peer;
    ipc_session_id_t session_id;
    pid_t pid;
    uint32_t service;
    uint8_t kind;
    uint8_t model;

    ipc_pktpool_t *pool;
    nai_buflist_t list;
};

int init_packet_module(void);
int deinit_packet_module(void);

static inline void ipc_packet_set_kind(ipc_packet_t *packet, uint8_t kind)
{
    switch (kind) {
        case IPC_PACKET_KIND_CLIENT_CREATED:
        case IPC_PACKET_KIND_CLIENT_PASSED:
        case IPC_PACKET_KIND_CLIENT_RECEIVED:
        case IPC_PACKET_KIND_SERVER_CREATED:
        case IPC_PACKET_KIND_SERVER_PASSED:
        case IPC_PACKET_KIND_SERVER_RECEIVED:
            packet->kind = kind;
            break;
        default:
            assert(0);  // BUG
    }
}

static inline uint8_t ipc_packet_get_kind(ipc_packet_t *packet) { return packet->kind; }

static inline bool ipc_packet_check_kind(ipc_packet_t *packet, uint8_t kind) { return (packet->kind == kind); }

static inline bool ipc_packet_is_request_in_client(ipc_packet_t *packet)
{
    switch (packet->kind) {
        case IPC_PACKET_KIND_CLIENT_CREATED:
        case IPC_PACKET_KIND_CLIENT_PASSED:
            return true;
        default:
            return false;
    }
}

static inline bool ipc_packet_is_request_from_client(ipc_packet_t *packet)
{
    switch (packet->kind) {
        case IPC_PACKET_KIND_SERVER_RECEIVED:
            return true;
        default:
            return false;
    }
}

static inline bool ipc_packet_is_response_in_server(ipc_packet_t *packet)
{
    switch (packet->kind) {
        case IPC_PACKET_KIND_SERVER_CREATED:
        case IPC_PACKET_KIND_SERVER_PASSED:
            return true;
        default:
            return false;
    }
}

static inline bool ipc_packet_is_response_from_server(ipc_packet_t *packet)
{
    switch (packet->kind) {
        case IPC_PACKET_KIND_CLIENT_RECEIVED:
            return true;
        default:
            return false;
    }
}

static inline void ipc_packet_reduce_received_kind(ipc_packet_t *packet)
{
    switch (packet->model) {
        case IPC_MESSAGE_REQUEST_POST:
        case IPC_MESSAGE_REQUEST_CALL:
        case IPC_MESSAGE_REQUEST_MULTI_REPLY:
        case IPC_MESSAGE_CONNECTING:
        case IPC_MESSAGE_DISCONNECT:
        case IPC_MESSAGE_EM_BROADCAST:
            packet->kind = IPC_PACKET_KIND_SERVER_RECEIVED;
            break;
        case IPC_MESSAGE_RESPONSE:
        case IPC_MESSAGE_RESPONSE_COMPLETE:
        case IPC_MESSAGE_CONNECTED:
            packet->kind = IPC_PACKET_KIND_CLIENT_RECEIVED;
            break;
        default:
            assert(0);  // BUG
    }
}

#if defined(IPC_PACKET_DEBUG)
    #define pr_debug(fmt, ...) printf("IPC " fmt, ##__VA_ARGS__)

static inline void ipc_packet_dump(ipc_packet_t *packet, const char *log)
{
    const char *kind;
    const char *model;

    switch (packet->kind) {
        case IPC_PACKET_KIND_NULL:
            kind = "IPC_PACKET_KIND_NULL";
            break;
        case IPC_PACKET_KIND_CLIENT_CREATED:
            kind = "IPC_PACKET_KIND_CLIENT_CREATED";
            break;
        case IPC_PACKET_KIND_CLIENT_PASSED:
            kind = "IPC_PACKET_KIND_CLIENT_PASSED";
            break;
        case IPC_PACKET_KIND_CLIENT_RECEIVED:
            kind = "IPC_PACKET_KIND_CLIENT_RECEIVED";
            break;
        case IPC_PACKET_KIND_SERVER_CREATED:
            kind = "IPC_PACKET_KIND_SERVER_CREATED";
            break;
        case IPC_PACKET_KIND_SERVER_PASSED:
            kind = "IPC_PACKET_KIND_SERVER_PASSED";
            break;
        case IPC_PACKET_KIND_SERVER_RECEIVED:
            kind = "IPC_PACKET_KIND_SERVER_RECEIVED";
            break;
        default:
            kind = "unkown: check for bug";
    }

    switch (packet->model) {
        case IPC_MESSAGE_TYPE_NULL:
            model = "IPC_MESSAGE_TYPE_NULL";
            break;
        case IPC_MESSAGE_REQUEST_POST:
            model = "IPC_MESSAGE_REQUEST_POST";
            break;
        case IPC_MESSAGE_REQUEST_CALL:
            model = "IPC_MESSAGE_REQUEST_CALL";
            break;
        case IPC_MESSAGE_REQUEST_MULTI_REPLY:
            model = "IPC_MESSAGE_REQUEST_MULTI_REPLY";
            break;
        case IPC_MESSAGE_RESPONSE:
            model = "IPC_MESSAGE_RESPONSE";
            break;
        case IPC_MESSAGE_RESPONSE_COMPLETE:
            model = "IPC_MESSAGE_RESPONSE_COMPLETE";
            break;
        case IPC_MESSAGE_CONNECTING:
            model = "IPC_MESSAGE_CONNECTING";
            break;
        case IPC_MESSAGE_CONNECTED:
            model = "IPC_MESSAGE_CONNECTED";
            break;
        case IPC_MESSAGE_DISCONNECT:
            model = "IPC_MESSAGE_DISCONNECT";
            break;
        case IPC_MESSAGE_EM_BROADCAST:
            model = "IPC_MESSAGE_EM_BROADCAST";
            break;
        default:
            model = "unkown: check for bug";
    }

    if (packet) {
        pr_debug("[%s] Packet(%p), packet->kind(%s), packet->model(%s), packet->peer(%p), packet->session_id(%ld)\n",
                 log, (void *)packet, kind, model, (void *)packet->peer, packet->session_id);
    } else {
        pr_debug("[%s] Packet is NULL\n", log);
    }
}
#else
static inline void ipc_packet_dump(ipc_packet_t *packet, const char *log)
{
    (void)packet;
    (void)log;
}
#endif

ipc_packet_t *ipc_packet_create(int isWritePool);

ipc_packet_t *ipc_packet_dup_payload(ipc_packet_t *packet, ipc_packet_t *duplicate_packet);

#endif  // IPC_INNER_PACKET_H
