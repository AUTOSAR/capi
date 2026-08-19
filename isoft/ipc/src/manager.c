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
/// @file       manager.c
/// @brief
/// @details
/// @date       2022-09-20
/// @author     yangtao
/// @version    1.2.0
///
/// ================================================================

#include "manager.h"

#include "event.h"
#include "utility.h"

struct ipc_outbound_s
{
    ipc_spinlock_t lock;
    TAILQ_HEAD(ipc_outbound_queue_t, ipc_session_s) queue;
};

typedef struct ipc_outbound_s ipc_outbound_t;

static void ipc_outbound_append_session(ipc_outbound_t* outbound, ipc_session_t* session)
{
    ipc_spinlock_lock(&outbound->lock);
    TAILQ_INSERT_TAIL(&outbound->queue, session, outbound_node);
    ipc_spinlock_unlock(&outbound->lock);
}

static ipc_session_t* ipc_outbound_peek_session(ipc_outbound_t* outbound)
{
    ipc_session_t* session = NULL;
    ipc_spinlock_lock(&outbound->lock);
    if (!TAILQ_EMPTY(&outbound->queue))
        session = TAILQ_FIRST(&outbound->queue);
    ipc_spinlock_unlock(&outbound->lock);
    return session;
}

static void ipc_outbound_remove_session(ipc_outbound_t* outbound, ipc_session_t* session)
{
    ipc_spinlock_lock(&outbound->lock);
    TAILQ_REMOVE(&outbound->queue, session, outbound_node);
    ipc_spinlock_unlock(&outbound->lock);
}

static ipc_session_t* ipc_outbound_take_session(ipc_outbound_t* outbound)
{
    ipc_session_t* session = NULL;
    ipc_spinlock_lock(&outbound->lock);
    if (!TAILQ_EMPTY(&outbound->queue)) {
        session = TAILQ_FIRST(&outbound->queue);
        TAILQ_REMOVE(&outbound->queue, session, outbound_node);
    }
    ipc_spinlock_unlock(&outbound->lock);
    return session;
}

typedef enum
{
    IPC_MODULE_STATUS_NULL = 0,
    IPC_MODULE_STATUS_SETUP,
    IPC_MODULE_STATUS_TO_CANCEL
} ipc_module_status_s;
typedef ipc_module_status_s ipc_module_status_t;

typedef struct
{
    ipc_spinlock_t lock;
    nai_evloop_t* loop;
    int init_counter;
    ipc_module_status_t module_status;
    ipc_channel_manager_t manager;
    ipc_outbound_t outbound;
} ipc_runtime_t;

atomic_uintptr_t global_server_seq;

static ipc_packet_t* ipc_runtime_peek_message(void* context)
{
    ipc_runtime_t* runtime = (ipc_runtime_t*)context;
    ipc_session_t* session = NULL;
    ipc_packet_t* message  = NULL;
    uint64_t now;

    do {
        session = ipc_outbound_peek_session(&runtime->outbound);
        if (session == NULL)
            goto L_RETURN;

        if (session->deadline != 0) {
            now = nai_time() / 1000;
            if (now >= session->deadline) {
                if (ipc_session_has_timer(session))
                    ipc_session_stop_timer(session);
                ipc_outbound_remove_session(&runtime->outbound, session);
                session->status = IPC_CLIENT_HANDLER_STATUS_ERR;
                ipc_packet_release(session->message);
                ipc_session_end(session, ETIMEDOUT);
                session = NULL;
            }
        }
    } while (session == NULL);
    message = session->message;
    ipc_packet_dump(message, "ipc_runtime_peek_message");

L_RETURN:
    return message;
}

static void ipc_runtime_on_timeout(void* context, ipc_session_t* session)
{
    //ipc_runtime_t* runtime = (ipc_runtime_t*)context;
    (void)context;

    assert(ipc_packet_is_request_in_client(session->message));
    assert(session->message->model != IPC_MESSAGE_REQUEST_POST);

    ipc_session_table_remove(session);

    ipc_packet_dump(session->message, "ipc_runtime_on_timeout");

    session->status = IPC_CLIENT_HANDLER_STATUS_ERR;
    ipc_packet_release(session->message);
    ipc_session_end(session, ETIMEDOUT);
}

static void ipc_runtime_on_sent(void* context, ipc_packet_t* message)
{
    ipc_runtime_t* runtime = (ipc_runtime_t*)context;
    ipc_session_t* session = NULL;

    uint64_t now;

    ipc_packet_dump(message, "ipc_runtime_on_sent");

    session = ipc_outbound_take_session(&runtime->outbound);
    assert(session != NULL);

    if (session->message->model == IPC_MESSAGE_EM_BROADCAST) {
        ipc_session_delete(session);
        return;
    }

    if (ipc_packet_is_request_in_client(session->message)) {
        if (session->message->model == IPC_MESSAGE_REQUEST_POST || session->message->model == IPC_MESSAGE_DISCONNECT) {
            ipc_session_delete(session);
        } else {
            if (session->deadline == 0) {
                ipc_session_table_insert(session);
                ipc_packet_ref(session->message);
            } else {
                now = nai_time() / 1000;
                if (now >= session->deadline) {
                    session->status = IPC_CLIENT_HANDLER_STATUS_ERR;
                    ipc_session_end(session, ETIMEDOUT);
                } else {
                    if (ipc_session_start_timer(session, (int)(session->deadline - now), runtime->loop,
                                                ipc_runtime_on_timeout, runtime)
                        != 0) {
                        session->status = IPC_CLIENT_HANDLER_STATUS_ERR;
                        ipc_session_end(session, errno);
                    } else {
                        ipc_session_table_insert(session);
                        ipc_packet_ref(session->message);
                    }
                }
            }
        }
    } else if (ipc_packet_is_response_in_server(message)) {
        if (message->model == IPC_MESSAGE_RESPONSE_COMPLETE) {
            ipc_sever_session_remove_by_seq(message->seq);
        }

        ipc_session_delete(session);
    } else {
        assert(0);  // BUG
    }
}

static int ipc_internal_handle_message(ipc_packet_t* packet)
{
    ipc_internal_message_t message;
    ipc_server_t* server;
    int status = -1;

    ipc_buffer_t* buffer = ipc_packet_get_buffer(packet);
    if (buffer) {
        uint8_t* buf = ipc_buffer_get_ptr(buffer);
        size_t len   = ipc_buffer_get_len(buffer);
        memcpy(&message, buf, len);

        pid_t pid = message.pid;
        if (pid) {
            server = ipc_server_connection_get_server(pid);
            if (server) {
                packet->model   = IPC_MESSAGE_DISCONNECT;
                packet->pid     = pid;
                packet->service = server->service;

                status = 0;
            }
        }
    }

    return status;
}

static void ipc_server_handle_message(ipc_server_t* server, ipc_packet_t* message)
{
    int model = message->model;
    ipc_server_handle_type_t type;

    ipc_packet_dump(message, "ipc_server_handle_message");

    if (model == IPC_MESSAGE_CONNECTING || model == IPC_MESSAGE_REQUEST_CALL
        || model == IPC_MESSAGE_REQUEST_MULTI_REPLY) {
        ipc_server_connection_insert(server, message->pid);
    }

    if (model == IPC_MESSAGE_DISCONNECT) {
        ipc_server_connection_remove(message->pid);
    }

    if (model == IPC_MESSAGE_CONNECTING) {
        ipc_packet_t* response = ipc_packet_create(1);
        if (response == NULL) {
            return;
        }

        ipc_packet_set_kind(response, IPC_PACKET_KIND_SERVER_CREATED);
        response->model      = IPC_MESSAGE_CONNECTED;
        response->peer       = ipc_peer_ref(message->peer);
        response->service    = message->service;
        response->session_id = message->session_id;

        ipc_server_send_response(server, response);
    }

    switch (model) {
        case IPC_MESSAGE_CONNECTING:
            type = IPC_SERVER_HANDLER_CONNECT;
            break;
        case IPC_MESSAGE_DISCONNECT:
            type = IPC_SERVER_HANDLER_DISCONNECT;
            break;
        case IPC_MESSAGE_REQUEST_POST:
            type = IPC_SERVER_HANDLER_POST;
            break;
        case IPC_MESSAGE_REQUEST_CALL:
            type = IPC_SERVER_HANDLER_ONCE;
            break;
        case IPC_MESSAGE_REQUEST_MULTI_REPLY:
            type = IPC_SERVER_HANDLER_MULTI;
            break;
        default:
            return;
    }

    ipc_server_handler_t handler = ipc_server_get_handler(server, type);
    void* ctx                    = ipc_server_get_ctx(server, type);

    if (handler)
        handler(ctx, type, message);
}

static void ipc_client_handle_message(ipc_session_t* session, ipc_packet_t* message)
{
    int message_model = message->model;
    int session_model = session->message->model;
    ipc_client_handler_status_t status;

    ipc_packet_dump(message, "ipc_client_handle_message");

    switch (message_model) {
        case IPC_MESSAGE_CONNECTED:
            if (session_model == IPC_MESSAGE_CONNECTING) {
                status = IPC_CLIENT_HANDLER_STATUS_CONNECTED;
            }
            break;
        case IPC_MESSAGE_RESPONSE:
            if (session_model == IPC_MESSAGE_REQUEST_MULTI_REPLY) {
                status = IPC_CLIENT_HANDLER_STATUS_RECIEVED_CONTINUE;
            } else {
                status = IPC_CLIENT_HANDLER_STATUS_RECIEVED;
            }
            break;
        case IPC_MESSAGE_RESPONSE_COMPLETE:
            status = IPC_CLIENT_HANDLER_STATUS_RECIEVED_COMPLETED;
            break;

        default:
            return;
    }

    session->handler(session->context, status, message);
}

static void ipc_runtime_on_received(void* context, ipc_packet_t* message)
{
    //ipc_runtime_t* runtime = (ipc_runtime_t*)context;
    (void)context;

    ipc_server_t dummy = {.service = message->service};
    ipc_server_t* server;
    ipc_session_t* session;

    ipc_packet_dump(message, "ipc_runtime_on_received");

    if (message->model == IPC_MESSAGE_EM_BROADCAST) {
        if (ipc_internal_handle_message(message) == 0) {
            dummy.service = message->service;
        } else {
            goto L_RETURN;
        }
    }

    if (ipc_packet_is_request_from_client(message)) {
        server = ipc_server_table_get(&dummy);

        if (server == NULL) {
            goto L_RETURN;
        }

        if (message->model == IPC_MESSAGE_DISCONNECT) {
            ipc_server_session_remove_by_pid(message->pid);
        }

        if (message->model == IPC_MESSAGE_REQUEST_MULTI_REPLY || message->model == IPC_MESSAGE_REQUEST_CALL) {
            message->seq = atomic_fetch_add(&global_server_seq, 1);
            ipc_server_session_insert(message);
        }

        ipc_server_handle_message(server, message);
    } else if (ipc_packet_is_response_from_server(message)) {
        session = (ipc_session_t*)message->session_id;
        session = ipc_session_table_get(session);

        if (session == NULL) {
            goto L_RETURN;
        }

        if (ipc_session_has_timer(session))
            ipc_session_stop_timer(session);

        if (session->message->model == IPC_MESSAGE_REQUEST_CALL) {
            ipc_session_table_remove(session);

            ipc_packet_release(session->message);

            if (session->handler) {
                session->response = message;
            } else {
                session->response = ipc_packet_ref(message);
            }
            session->status = IPC_CLIENT_HANDLER_STATUS_RECIEVED_COMPLETED;
            ipc_session_end(session, 0);
        } else {
            if (message->model == IPC_MESSAGE_CONNECTED || message->model == IPC_MESSAGE_RESPONSE_COMPLETE) {
                ipc_session_table_remove(session);
            }

            ipc_client_handle_message(session, message);

            if (message->model == IPC_MESSAGE_CONNECTED || message->model == IPC_MESSAGE_RESPONSE_COMPLETE) {
                ipc_packet_release(session->message);
                ipc_session_delete(session);
            }
        }
    } else {
        assert(0);  // BUG
    }

L_RETURN:
    return;
}

static void ipc_runtime_on_error(void* context, ipc_packet_t* message)
{
    ipc_runtime_t* runtime = (ipc_runtime_t*)context;
    ipc_session_t* session = NULL;

    ipc_packet_dump(message, "ipc_runtime_on_error");

    session = ipc_outbound_take_session(&runtime->outbound);
    if (session == NULL) {
        // receiving error
        return;
    }

    if (message != NULL) {
        // sending error
        if (ipc_session_has_timer(session))
            ipc_session_stop_timer(session);
        session->status = IPC_CLIENT_HANDLER_STATUS_ERR;
        ipc_session_end(session, errno);
    } else {
        // receiving error
    }
}

static ipc_runtime_t rt = {
    .lock                 = IPC_SPINLOCK_INITIALIZER,
    .loop                 = NULL,
    .init_counter         = 0,
    .module_status        = IPC_MODULE_STATUS_NULL,
    .manager.context      = &rt,
    .manager.peek_message = ipc_runtime_peek_message,
    .manager.on_sent      = ipc_runtime_on_sent,
    .manager.on_received  = ipc_runtime_on_received,
    .manager.on_error     = ipc_runtime_on_error,
    .outbound.lock        = IPC_SPINLOCK_INITIALIZER,
    .outbound.queue       = TAILQ_HEAD_INITIALIZER(rt.outbound.queue),
};

// call with rt.lock
static int ipc_module_startup(nai_evloop_t* event_loop)
{
    int status = 0;

    if (rt.module_status == IPC_MODULE_STATUS_NULL) {
        status = init_packet_module();
        if (status)
            goto L_RETURN;
        status = init_session_module();
        if (status)
            goto L_RETURN;
        status = init_server_session_module();
        if (status)
            goto L_RETURN;

        ipc_assert(rt.loop == NULL);
        status = ipc_channel_startup(event_loop, &rt.manager);
        if (status)
            goto L_RETURN;
        else
            rt.loop = event_loop;

        rt.module_status = IPC_MODULE_STATUS_SETUP;
    } else if (rt.module_status == IPC_MODULE_STATUS_TO_CANCEL) {
        ipc_assert(rt.loop == NULL);
        rt.loop = event_loop;

        rt.module_status = IPC_MODULE_STATUS_SETUP;
    } else {
        ipc_assert(rt.module_status == IPC_MODULE_STATUS_SETUP);
    }

    // protect by rt.lock
    // Initialize only once
    if (global_server_seq == 0) {
        atomic_init(&global_server_seq, 1);
    }

L_RETURN:
    return status;
}

static int ipc_module_cleanup(void)
{
    int status = 0;

    ipc_assert(rt.loop != NULL);

    status = ipc_channel_close();
    if (status == -1) {
        goto L_RETURN;
    } else {
        rt.loop = NULL;
    }
    rt.module_status = IPC_MODULE_STATUS_TO_CANCEL;

L_RETURN:
    return status;
}

int ipc_module_cleanup_delayed(void)
{
    int status = 0;

    ipc_spinlock_lock(&rt.lock);

    if (rt.module_status == IPC_MODULE_STATUS_TO_CANCEL) {
        status = deinit_server_session_module();
        if (status == -1) {
            goto L_UNLOCK;
        }
        status = deinit_session_module();
        if (status) {
            goto L_UNLOCK;
        }
        status = deinit_packet_module();
        if (status == -1) {
            goto L_UNLOCK;
        }

        status = ipc_channel_cleanup();
        if (status == -1) {
            goto L_UNLOCK;
        }

        rt.module_status = IPC_MODULE_STATUS_NULL;
    }

L_UNLOCK:
    ipc_spinlock_unlock(&rt.lock);

    return status;
}

int ipc_startup(nai_evloop_t* event_loop)
{
    int status = -1;

    ipc_assert(event_loop != NULL);

    ipc_spinlock_lock(&rt.lock);

    rt.init_counter++;
    ipc_init_counter_dump(rt.init_counter, "ipc_startup");
    if (rt.init_counter == 1) {
        status = ipc_module_startup(event_loop);
    } else {
        ipc_assert(rt.loop == event_loop);
        status = 0;
    }

    ipc_spinlock_unlock(&rt.lock);
    return status;
}

int ipc_cleanup(void)
{
    int status = -1;

    ipc_spinlock_lock(&rt.lock);

    ipc_assert(rt.init_counter > 0);
    rt.init_counter--;
    ipc_init_counter_dump(rt.init_counter, "ipc_cleanup");
    if (rt.init_counter == 0) {
        status = ipc_module_cleanup();
    } else {
        status = 0;
    }

    ipc_spinlock_unlock(&rt.lock);
    return status;
}

void ipc_connect_handle(void* context, ipc_client_handler_status_t status, ipc_packet_t* reponse_packet);

static int ipc_client_connect(ipc_client_t* client)
{
    int status = -1;

    if (client->conn_handler == NULL) {
        status = 0;
        goto L_RETURN;
    }

    ipc_packet_t* packet = ipc_packet_create(1);
    if (packet == NULL) {
        goto L_RETURN;
    }

    ipc_packet_set_kind(packet, IPC_PACKET_KIND_CLIENT_CREATED);
    packet->model   = IPC_MESSAGE_CONNECTING;
    packet->peer    = ipc_peer_ref(client->peer);
    packet->service = client->service;

    status = ipc_client_send(client, packet, NULL, ipc_connect_handle, (void*)client, 1000);

L_RETURN:
    return status;
}
void ipc_connect_handle(void* context, ipc_client_handler_status_t status, ipc_packet_t* reponse_packet)
{
    (void)reponse_packet;

    if (context == NULL)
        return;

    ipc_client_t* client = (ipc_client_t*)context;
    if (client->conn_handler) {
        if (status == IPC_CLIENT_HANDLER_STATUS_ERR) {
            if (errno == ETIMEDOUT)
                ipc_client_connect(client);
        } else {
            (client->conn_handler)(client->conn_context, IPC_CLIENT_HANDLER_STATUS_CONNECTED);
        }
    }
}

int ipc_client_start(ipc_client_t* client) { return ipc_client_connect(client); }

static int ipc_client_disconnect(ipc_client_t* client)
{
    ipc_packet_t* packet = ipc_packet_create(1);
    if (packet == NULL) {
        return -1;
    }

    ipc_packet_set_kind(packet, IPC_PACKET_KIND_CLIENT_CREATED);
    packet->model   = IPC_MESSAGE_DISCONNECT;
    packet->peer    = ipc_peer_ref(client->peer);
    packet->service = client->service;

    return ipc_client_send(client, packet, NULL, NULL, NULL, 0);
}

int ipc_client_stop(ipc_client_t* client) { return ipc_client_disconnect(client); }

int ipc_server_start(ipc_server_t* server)
{
    ipc_server_table_insert(server);
    return 0;
}

int ipc_server_stop(ipc_server_t* server)
{
    ipc_server_table_remove(server);
    return 0;
}

int ipc_client_send(ipc_client_t* client,
                    ipc_packet_t* request,
                    ipc_packet_t** response,
                    ipc_client_handler_t handler,
                    void* context,
                    int timeout)
{
    int status        = -1;
    uint64_t deadline = 0;
    ipc_session_t dummy;
    ipc_session_t* session;
    pthread_mutex_t cvl = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t cv   = PTHREAD_COND_INITIALIZER;

    errno = EINVAL;
    if (request == NULL || timeout < -1)
        goto L_RETURN;

    if (timeout > 0)
        deadline = nai_time() / 1000 + (uint64_t)timeout;

    if (handler == NULL && request->model == IPC_MESSAGE_REQUEST_CALL) {
        session = &dummy;
        ipc_session_initialize(session);
    } else {
        session = ipc_session_create();
        if (session == NULL)
            goto L_RETURN;
    }

    session->message             = request;
    session->session_id          = (uintptr_t)session;
    session->message->session_id = session->session_id;
    session->request             = request;
    session->response            = NULL;
    session->client              = ipc_client_ref(client);
    session->handler             = handler;
    session->context             = context;
    session->deadline            = deadline;

    if (request->model == IPC_MESSAGE_REQUEST_CALL) {
        if (handler == NULL) {
            session->cvl = &cvl;
            session->cv  = &cv;
        }
    }

    ipc_packet_dump(request, "ipc_client_send");

    ipc_outbound_append_session(&rt.outbound, session);

    ipc_channel_notify();

    ipc_packet_set_kind(request, IPC_PACKET_KIND_CLIENT_PASSED);
    status = 0;
    if (request->model == IPC_MESSAGE_REQUEST_CALL) {
        if (handler == NULL) {
            pthread_mutex_lock(&cvl);
            if (session->cv != NULL)
                pthread_cond_wait(&cv, &cvl);
            pthread_mutex_unlock(&cvl);
            status = session->errcode;
            if (status == 0) {
                if (response != NULL) {
                    *response = session->response;
                }
            } else {
                errno  = status;
                status = -1;
            }
        }
    }

L_RETURN:
    return status;
}

int ipc_server_send_response(ipc_server_t* server, ipc_packet_t* response)
{
    int status = -1;
    ipc_session_t* session;

    errno = EINVAL;
    if (server == NULL || response == NULL)
        goto L_RETURN;

    session = ipc_session_create();
    if (session == NULL)
        goto L_RETURN;

    session->message  = response;
    session->request  = NULL;
    session->response = NULL;
    session->handler  = NULL;
    session->context  = NULL;
    session->deadline = 0;

    ipc_outbound_append_session(&rt.outbound, session);

    ipc_channel_notify();
    ipc_packet_set_kind(response, IPC_PACKET_KIND_SERVER_PASSED);
    status = 0;

L_RETURN:
    return status;
}
