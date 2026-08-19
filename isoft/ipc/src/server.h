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
/// @file       server.h
/// @brief
/// @details
/// @date       2022-09-20
/// @author     yangtao
/// @version    1.2.0
///
/// ================================================================

#ifndef IPC_INNER_SERVER_H
#define IPC_INNER_SERVER_H

#include <isoft/ipc/server.h>
#include <stdlib.h>

#include "tree.h"
#include "utility.h"

typedef struct
{
    ipc_server_handler_t handler;
    void* context;
} ipc_server_handle_node_t;

struct ipc_server_s
{
    RB_ENTRY(ipc_server_s) node;

    uint32_t service;
    ipc_server_handle_node_t handles[5];
};

static inline int ipc_server_type_to_index(ipc_server_handle_type_t type) { return (int)(type - 1); }

static inline void* ipc_server_get_ctx(ipc_server_t* server, ipc_server_handle_type_t type)
{
    int index = ipc_server_type_to_index(type);

    return (server->handles[index]).context;
}

static inline ipc_server_handler_t ipc_server_get_handler(ipc_server_t* server, ipc_server_handle_type_t type)
{
    int index = ipc_server_type_to_index(type);

    return (server->handles[index]).handler;
}

typedef struct ipc_server_connection_s
{
    RB_ENTRY(ipc_server_connection_s) node;

    pid_t pid;
    ipc_server_t* server;
} ipc_server_connection_t;

ipc_server_t* ipc_server_connection_get_server(pid_t pid);

int ipc_server_connection_insert(ipc_server_t* server, pid_t pid);

int ipc_server_connection_remove(pid_t pid);

ipc_server_t* ipc_server_table_get(ipc_server_t* server);
void ipc_server_table_insert(ipc_server_t* server);
void ipc_server_table_remove(ipc_server_t* server);

#endif  // IPC_INNER_SERVER_H
