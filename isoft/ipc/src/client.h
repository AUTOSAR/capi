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
/// @file       client.h
/// @brief
/// @details
/// @date       2022-09-20
/// @author     yangtao
/// @version    1.2.0
///
/// ================================================================

#ifndef IPC_INNER_CLIENT_H
#define IPC_INNER_CLIENT_H

#include <isoft/ipc/client.h>
#include <isoft/ipc/ipc.h>
#include <stdatomic.h>
#include <string.h>

#include "peer.h"
#include "service.h"
#include "tree.h"
#include "utility.h"

struct ipc_client_s
{
    RB_ENTRY(ipc_client_s) node;

    atomic_uintptr_t ref_count;

    ipc_peer_t* peer;
    uint32_t service;
    char service_name[IPC_SERVICE_NAME_LENGTH + 1];

    ipc_client_connection_handler_t conn_handler;
    void* conn_context;
};

static inline void ipc_client_set_service_name(ipc_client_t* client, const char* service_name)
{
    strncpy(client->service_name, service_name, IPC_SERVICE_NAME_LENGTH);
    client->service_name[IPC_SERVICE_NAME_LENGTH] = '\0';
}

ipc_client_t* ipc_client_ref(ipc_client_t* client);
void ipc_client_insert_to_table(ipc_client_t* client);
void ipc_client_remove_from_table(ipc_client_t* client);
ipc_client_t* ipc_client_find(uint32_t service, ipc_peer_t* peer);

#endif  // IPC_INNER_CLIENT_H
