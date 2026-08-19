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
/// @file       ipc_em.c
/// @brief
/// @details
/// @date       2022-11-28
/// @author     yangtao
/// @version    1.2.0
///
/// ================================================================

#include "ipc_em.h"

#include "../event.h"
#include "../service.h"

static int ipc_em_process_compare(ipc_em_process_t *left, ipc_em_process_t *right)
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

typedef RB_HEAD(ipc_em_process_table_s, ipc_em_process_s) ipc_em_process_table_t;
RB_GENERATE_INTERNAL(ipc_em_process_table_s, ipc_em_process_s, node, ipc_em_process_compare, static inline)

typedef struct ipc_em_runtime_s
{
    ipc_spinlock_t process_table_lock;
    ipc_em_process_table_t process_table;
} ipc_em_runtime_t;

static ipc_em_runtime_t rt = {
    .process_table_lock = IPC_SPINLOCK_INITIALIZER,
    .process_table      = RB_INITIALIZER(&rt.process_table),
};

static ipc_em_process_t *ipc_em_process_create(void)
{
    ipc_em_process_t *process;
    process = (ipc_em_process_t *)malloc(sizeof(ipc_em_process_t));

    return process;
}

static void ipc_em_process_free(ipc_em_process_t *process)
{
    if (process) {
        free(process->path);
        free(process);
    }
}

static int ipc_em_process_table_insert(ipc_em_runtime_t *runtime, pid_t pid, const char *path)
{
    ipc_em_process_t *process;
    int status = -1;

    ipc_em_process_t dummy = {.pid = pid};
    ipc_spinlock_lock(&runtime->process_table_lock);
    process = RB_FIND(ipc_em_process_table_s, &runtime->process_table, &dummy);
    ipc_spinlock_unlock(&runtime->process_table_lock);

    if (process) {
        return status;
    }

    process = ipc_em_process_create();
    if (process == NULL) {
        return status;
    }
    process->pid  = pid;
    process->path = strdup(path);

    ipc_spinlock_lock(&runtime->process_table_lock);
    if (RB_INSERT(ipc_em_process_table_s, &runtime->process_table, process)) {
        ipc_em_process_free(process);
    } else {
        status = 0;
    }
    ipc_spinlock_unlock(&runtime->process_table_lock);

    return status;
}

static int ipc_em_process_table_remove(ipc_em_runtime_t *runtime, pid_t pid)
{
    ipc_em_process_t dummy = {.pid = pid};
    ipc_em_process_t *process;
    int status = -1;

    ipc_spinlock_lock(&runtime->process_table_lock);
    process = RB_FIND(ipc_em_process_table_s, &runtime->process_table, &dummy);
    if (process) {
        RB_REMOVE(ipc_em_process_table_s, &runtime->process_table, process);
        status = 0;
    }
    ipc_spinlock_unlock(&runtime->process_table_lock);

    ipc_em_process_free(process);

    return status;
}

const char *ipc_em_fake_service = "fake";
static ipc_packet_t *ipc_em_personate_packet(const char *path)
{
    ipc_packet_t *packet;

    packet = ipc_packet_create(1);
    if (packet == NULL) {
        goto L_RETURN;
    }

    ipc_packet_set_kind(packet, IPC_PACKET_KIND_CLIENT_CREATED);
    packet->service = ipc_service_extract_id_from_name(ipc_em_fake_service);
    packet->peer    = ipc_peer_find(path);

L_RETURN:
    return packet;
}

static int ipc_em_channel_send(const char *path, pid_t pid)
{
    int r = -1;
    ipc_buffer_t *buffer;
    uint8_t *buf;

    ipc_packet_t *packet = ipc_em_personate_packet(path);
    if (packet == NULL) {
        goto L_RETURN;
    }

    ipc_internal_message_t message = {
        .pid = pid,
    };

    buffer = ipc_packet_append_buffer(packet, sizeof(ipc_internal_message_t));
    buf    = ipc_buffer_get_ptr(buffer);
    memcpy(buf, &message, sizeof(ipc_internal_message_t));
    ipc_buffer_set_len(buffer, sizeof(ipc_internal_message_t));

    packet->model = IPC_MESSAGE_EM_BROADCAST;
    r             = ipc_client_send(NULL, packet, NULL, NULL, NULL, 0);

L_RETURN:
    return r;
}

static void ipc_em_process_broadcast(ipc_em_runtime_t *runtime, pid_t pid)
{
    ipc_em_process_t *process;

    ipc_spinlock_lock(&runtime->process_table_lock);
    RB_FOREACH(process, ipc_em_process_table_s, &runtime->process_table)
    {
        if (process && process->pid != pid) {
            ipc_em_channel_send(process->path, pid);
        }
    }
    ipc_spinlock_unlock(&runtime->process_table_lock);
}

int ipc_em_insert(pid_t pid, const char *path)
{
    int r;
    ipc_em_runtime_t *runtime = &rt;

    ipc_em_dump(pid, path, 0, "ipc_em_insert");

    r = ipc_em_process_table_insert(runtime, pid, path);

    return r;
}

int ipc_em_remove(pid_t pid, ipc_em_process_exit_status_t status)
{
    int r                     = 0;
    ipc_em_runtime_t *runtime = &rt;

    ipc_em_dump(pid, NULL, status, "ipc_em_remove");

    if (status == IPC_EM_PROCESS_EXIT_WITH_FAILURE) {
        r = ipc_em_process_table_remove(runtime, pid);
        if (r == 0) {
            ipc_em_process_broadcast(runtime, pid);
        }
    }

    return r;
}
