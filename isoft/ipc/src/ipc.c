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
/// @file       ipc.c
/// @brief
/// @details
/// @date       2022-09-20
/// @author     yangtao
/// @version    1.2.0
///
/// ================================================================

#include <isoft/ipc/client.h>
#include <isoft/ipc/ipc.h>
#include <isoft/ipc/packet.h>
#include <isoft/ipc/server.h>
#include <stdatomic.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "client.h"
#include "event.h"
#include "manager.h"
#include "peer.h"
#include "server.h"
#include "service.h"

int ipc_init_nai_unix_socket(nai_evloop_t *event_loop) { return ipc_startup(event_loop); }

int ipc_deinit_nai_unix_socket(void) { return ipc_cleanup(); }

ipc_client_t *ipc_client_create(const char *address)
{
    ipc_client_t *client = NULL;
    size_t length;
    char fullname[IPC_ADDRESS_LENGTH_MAX + 1];
    char *program_name = fullname;
    char *service_name;
    uint32_t service;
    ipc_peer_t *peer;

    errno = EINVAL;
    if (address == NULL) {
        goto L_RETURN;
    }
    length = strlen(address);

    if (!ipc_service_check_path(address, length)) {
        goto L_RETURN;
    }

    strncpy(fullname, address, IPC_ADDRESS_LENGTH_MAX);
    fullname[length]                               = '\0';
    fullname[length - IPC_SERVICE_NAME_LENGTH - 1] = '\0';
    service_name                                   = fullname + length - IPC_SERVICE_NAME_LENGTH;
    service                                        = ipc_service_extract_id_from_name(service_name);

    // hold a reference
    peer = ipc_peer_find(program_name);
    if (peer == NULL) {
        goto L_RETURN;
    }

    client = ipc_client_find(service, peer);
    if (client != NULL) {
        goto L_PEER;
    }

    client = (ipc_client_t *)malloc(sizeof(ipc_client_t));
    if (client == NULL) {
        goto L_PEER;
    }

    atomic_init(&client->ref_count, 0);
    ipc_client_set_service_name(client, service_name);
    client->service      = service;
    client->conn_handler = NULL;
    client->peer         = peer;

    ipc_client_ref(client);
    ipc_client_insert_to_table(client);
    goto L_RETURN;

L_PEER:
    ipc_peer_release(peer);

L_RETURN:
    return client;
}

void ipc_client_set_connection_handler(ipc_client_t *client, ipc_client_connection_handler_t handler, void *context)
{
    if (client == NULL || handler == NULL)
        goto L_RETURN;

    client->conn_handler = handler;
    client->conn_context = context;

L_RETURN:
    return;
}

ipc_client_t *ipc_client_ref(ipc_client_t *client)
{
    if (client == NULL)
        goto L_RETURN;

    atomic_fetch_add(&client->ref_count, 1);

L_RETURN:
    return client;
}

void ipc_client_release(ipc_client_t *client)
{
    if (client == NULL)
        return;

    if (atomic_fetch_sub(&client->ref_count, 1) == 1) {
        ipc_client_remove_from_table(client);

        ipc_peer_release(client->peer);

        free(client);
    }
}

ipc_server_t *ipc_server_create(const char *name)
{
    ipc_server_t *server = NULL;

    errno = EINVAL;
    if (name == NULL)
        goto L_RETURN;

    if (!ipc_service_check_name(name)) {
        goto L_RETURN;
    }

    server = (ipc_server_t *)malloc(sizeof(ipc_server_t));
    if (server == NULL)
        goto L_RETURN;

    server->service = ipc_service_extract_id_from_name(name);

    for (int i = 0; i < 5; i++) {
        memset(&server->handles[i], 0, sizeof(ipc_server_handle_node_t));
    }

L_RETURN:
    return server;
}

void ipc_server_set_handler(ipc_server_t *server,
                            ipc_server_handle_type_t type,
                            ipc_server_handler_t handler,
                            void *context)
{
    if (server == NULL || handler == NULL)
        goto L_RETURN;

    int index = ipc_server_type_to_index(type);

    ipc_server_handle_node_t *node = &server->handles[index];
    node->handler                  = handler;
    node->context                  = context;

L_RETURN:
    return;
}

void ipc_server_release(ipc_server_t *server) { free(server); }

ipc_packet_t *ipc_client_make_request(ipc_client_t *client)
{
    ipc_packet_t *packet = ipc_packet_create(1);
    if (packet == NULL) {
        goto L_RETURN;
    }

    ipc_packet_set_kind(packet, IPC_PACKET_KIND_CLIENT_CREATED);
    packet->peer    = ipc_peer_ref(client->peer);
    packet->service = client->service;

L_RETURN:
    return packet;
}

int ipc_client_post(ipc_client_t *client, ipc_packet_t *request_packet)
{
    int r = -1;

    errno = EINVAL;
    if (client == NULL || request_packet == NULL)
        goto L_RETURN;

    request_packet->model = IPC_MESSAGE_REQUEST_POST;
    r                     = ipc_client_send(client, request_packet, NULL, NULL, NULL, 0);

L_RETURN:
    return r;
}

int ipc_client_send_async(ipc_client_t *client,
                          int timeout,
                          bool is_multi_reply,
                          ipc_packet_t *request_packet,
                          ipc_client_handler_t handler,
                          void *context)
{
    int r = -1;

    errno = EINVAL;
    if (client == NULL || request_packet == NULL || timeout < -1 || timeout == 0)
        goto L_RETURN;
    if (handler == NULL)
        goto L_RETURN;

    request_packet->model = IPC_MESSAGE_REQUEST_CALL;
    if (is_multi_reply) {
        request_packet->model = IPC_MESSAGE_REQUEST_MULTI_REPLY;
    }
    r = ipc_client_send(client, request_packet, NULL, handler, context, timeout);

L_RETURN:
    return r;
}

int ipc_client_send_sync(ipc_client_t *client,
                         int timeout,
                         ipc_packet_t *request_packet,
                         ipc_packet_t **response_packet)
{
    int r = -1;

    errno = EINVAL;
    if (client == NULL || request_packet == NULL || timeout < -1 || timeout == 0)
        goto L_RETURN;

    request_packet->model = IPC_MESSAGE_REQUEST_CALL;
    r                     = ipc_client_send(client, request_packet, response_packet, NULL, NULL, timeout);

L_RETURN:
    return r;
}

ipc_packet_t *ipc_server_make_response(ipc_server_t *server, ipc_session_id_t session_id)
{
    if (server == NULL)
        return NULL;

    ipc_server_session_t *session = ipc_server_session_get(session_id);
    if (session == NULL)
        return NULL;

    ipc_packet_t *response = ipc_packet_create(1);
    if (response == NULL) {
        goto L_RETURN;
    }

    ipc_packet_set_kind(response, IPC_PACKET_KIND_SERVER_CREATED);
    response->peer       = ipc_peer_ref(session->peer);
    response->session_id = session->session_id;
    response->seq        = session->seq;
    response->service    = server->service;

L_RETURN:
    return response;
}

ipc_packet_t *ipc_server_dup_response(ipc_server_t *server, ipc_session_id_t session_id, ipc_packet_t *packet)
{
    if (server == NULL)
        return NULL;

    ipc_server_session_t *session = ipc_server_session_get(session_id);
    if (session == NULL)
        return NULL;

    ipc_packet_t *response = ipc_packet_create(1);
    if (response == NULL) {
        goto L_RETURN;
    }

    ipc_packet_dup_payload(response, packet);

    ipc_packet_set_kind(response, IPC_PACKET_KIND_SERVER_CREATED);
    response->peer       = ipc_peer_ref(session->peer);
    response->session_id = session->session_id;
    response->seq        = session->seq;
    response->service    = server->service;

L_RETURN:
    return response;
}

int ipc_server_send(ipc_server_t *server, ipc_packet_t *response_packet, bool complete)
{
    int r = -1;

    errno = EINVAL;
    if (server == NULL || response_packet == NULL)
        goto L_RETURN;

    ipc_packet_dump(response_packet, "ipc_server_send");

    response_packet->model = IPC_MESSAGE_RESPONSE;
    if (complete) {
        response_packet->model = IPC_MESSAGE_RESPONSE_COMPLETE;
    }

    r = ipc_server_send_response(server, response_packet);

L_RETURN:
    return r;
}

int ipc_server_check_session_id(ipc_server_t *server, ipc_session_id_t session_id)
{
    if (server == NULL)
        return -1;

    ipc_server_session_t *session = ipc_server_session_get(session_id);
    if (session == NULL)
        return -1;

    return 0;
}
