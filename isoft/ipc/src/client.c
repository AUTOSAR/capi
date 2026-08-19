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
/// @file       client.c
/// @brief
/// @details
/// @date       2023-01-03
/// @author     yangtao
/// @version    1.2.0
///
/// ================================================================

#include "client.h"

static int ipc_client_compare_by_key(uint32_t service, ipc_peer_t *peer, ipc_client_t *right)
{
    if (right->peer != peer) {
        return (int)((char *)right->peer - (char *)peer);
    } else {
        return (int)(right->service - service);
    }
}

static int ipc_client_compare(ipc_client_t *left, ipc_client_t *right)
{
    return ipc_client_compare_by_key(left->service, left->peer, right);
}

typedef RB_HEAD(ipc_client_table_s, ipc_client_s) ipc_client_table_t;
RB_GENERATE_INTERNAL(ipc_client_table_s, ipc_client_s, node, ipc_client_compare, static)

RB_CUSTOM_CLIENT_INTERNAL(
    ipc_client_table_s, ipc_client_s, node, ipc_client_compare_by_key, uint32_t, ipc_peer_t, static)

typedef struct ipc_client_runtime_s
{
    ipc_spinlock_t client_table_lock;
    ipc_client_table_t client_table;
} ipc_client_runtime_t;

static ipc_client_runtime_t rt = {
    .client_table_lock = IPC_SPINLOCK_INITIALIZER,
    .client_table      = RB_INITIALIZER(&rt.client_table),
};

void ipc_client_insert_to_table(ipc_client_t *client)
{
    ipc_spinlock_lock(&rt.client_table_lock);
    RB_INSERT(ipc_client_table_s, &rt.client_table, client);
    ipc_spinlock_unlock(&rt.client_table_lock);
}

void ipc_client_remove_from_table(ipc_client_t *client)
{
    ipc_spinlock_lock(&rt.client_table_lock);
    RB_REMOVE(ipc_client_table_s, &rt.client_table, client);
    ipc_spinlock_unlock(&rt.client_table_lock);
}

ipc_client_t *ipc_client_find(uint32_t service, ipc_peer_t *peer)
{
    ipc_client_t *client;
    client = RB_CLIENT_EXACT_FIND(ipc_client_table_s, &rt.client_table, service, peer);

    return client;
}
