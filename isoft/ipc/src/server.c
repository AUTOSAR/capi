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
/// @file       server.c
/// @brief
/// @details
/// @date       2022-11-28
/// @author     yangtao
/// @version    1.2.0
///
/// ================================================================

#include "server.h"

static int ipc_server_connection_table_compare(ipc_server_connection_t* left, ipc_server_connection_t* right)
{
    int result;
    if (left->pid < right->pid)
        result = -1;
    else if (left->pid > right->pid)
        result = 1;
    else
        result = 0;
    return result;
}

typedef RB_HEAD(ipc_server_connection_table_s, ipc_server_connection_s) ipc_server_connection_table_t;
RB_GENERATE_INTERNAL(
    ipc_server_connection_table_s, ipc_server_connection_s, node, ipc_server_connection_table_compare, static inline)

static int ipc_server_compare(ipc_server_t* left, ipc_server_t* right)
{
    int result;
    if (left->service < right->service)
        result = -1;
    else if (left->service > right->service)
        result = 1;
    else
        result = 0;
    return result;
}

typedef RB_HEAD(ipc_server_table_s, ipc_server_s) ipc_server_table_t;
RB_GENERATE_INTERNAL(ipc_server_table_s, ipc_server_s, node, ipc_server_compare, static)

typedef struct ipc_server_runtime_s
{
    ipc_spinlock_t connection_table_lock;
    ipc_server_connection_table_t connection_table;
    ipc_spinlock_t server_table_lock;
    ipc_server_table_t server_table;
} ipc_server_runtime_t;

static ipc_server_runtime_t rt = {
    .connection_table_lock = IPC_SPINLOCK_INITIALIZER,
    .connection_table      = RB_INITIALIZER(&rt.connection_table),
    .server_table_lock     = IPC_SPINLOCK_INITIALIZER,
    .server_table          = RB_INITIALIZER(&rt.server_table),
};

static ipc_server_connection_t* ipc_server_connection_create(void)
{
    ipc_server_connection_t* conn;
    conn = (ipc_server_connection_t*)malloc(sizeof(ipc_server_connection_t));

    return conn;
}

static void ipc_server_connection_free(ipc_server_connection_t* conn)
{
    if (conn) {
        free(conn);
    }
}

static int ipc_server_connection_table_insert(ipc_server_runtime_t* runtime, ipc_server_t* server, pid_t pid)
{
    ipc_server_connection_t* conn;
    int status = -1;

    conn = ipc_server_connection_create();
    if (conn == NULL) {
        return status;
    }
    conn->server = server;
    conn->pid    = pid;

    ipc_spinlock_lock(&runtime->connection_table_lock);
    if (RB_INSERT(ipc_server_connection_table_s, &runtime->connection_table, conn)) {
        ipc_server_connection_free(conn);
    } else {
        status = 0;
    }
    ipc_spinlock_unlock(&runtime->connection_table_lock);

    return status;
}

static ipc_server_connection_t* ipc_server_connection_table_get(ipc_server_runtime_t* runtime, pid_t pid)
{
    if (pid == 0)
        return NULL;

    ipc_server_connection_t dummy = {.pid = pid};
    ipc_server_connection_t* conn = NULL;

    ipc_spinlock_lock(&runtime->connection_table_lock);
    conn = RB_FIND(ipc_server_connection_table_s, &runtime->connection_table, &dummy);
    ipc_spinlock_unlock(&runtime->connection_table_lock);

    return conn;
}

static int ipc_server_connection_table_remove(ipc_server_runtime_t* runtime, pid_t pid)
{
    ipc_server_connection_t dummy = {.pid = pid};
    ipc_server_connection_t* conn = NULL;
    int status                    = -1;

    ipc_spinlock_lock(&runtime->connection_table_lock);
    conn = RB_FIND(ipc_server_connection_table_s, &runtime->connection_table, &dummy);
    if (conn) {
        RB_REMOVE(ipc_server_connection_table_s, &runtime->connection_table, conn);
        status = 0;
    }
    ipc_spinlock_unlock(&runtime->connection_table_lock);

    ipc_server_connection_free(conn);

    return status;
}

ipc_server_t* ipc_server_connection_get_server(pid_t pid)
{
    ipc_server_connection_t* conn = NULL;
    ipc_server_runtime_t* runtime = &rt;
    ipc_server_t* server          = NULL;

    conn = ipc_server_connection_table_get(runtime, pid);
    if (conn) {
        server = conn->server;
    }

    return server;
}

int ipc_server_connection_insert(ipc_server_t* server, pid_t pid)
{
    int r;
    ipc_server_runtime_t* runtime = &rt;

    r = ipc_server_connection_table_insert(runtime, server, pid);

    return r;
}

int ipc_server_connection_remove(pid_t pid)
{
    int r                         = 0;
    ipc_server_runtime_t* runtime = &rt;

    ipc_server_connection_table_remove(runtime, pid);

    return r;
}

ipc_server_t* ipc_server_table_get(ipc_server_t* server)
{
    if (server == NULL)
        return NULL;
    ipc_server_t* dummy = NULL;
    ipc_spinlock_lock(&rt.server_table_lock);
    dummy = RB_FIND(ipc_server_table_s, &rt.server_table, server);
    ipc_spinlock_unlock(&rt.server_table_lock);

    return dummy;
}

void ipc_server_table_insert(ipc_server_t* server)
{
    if (server == NULL)
        return;

    ipc_spinlock_lock(&rt.server_table_lock);
    RB_INSERT(ipc_server_table_s, &rt.server_table, server);
    ipc_spinlock_unlock(&rt.server_table_lock);
}

void ipc_server_table_remove(ipc_server_t* server)
{
    if (server == NULL)
        return;

    ipc_spinlock_lock(&rt.server_table_lock);
    if (RB_FIND(ipc_server_table_s, &rt.server_table, server))
        RB_REMOVE(ipc_server_table_s, &rt.server_table, server);
    ipc_spinlock_unlock(&rt.server_table_lock);
}
