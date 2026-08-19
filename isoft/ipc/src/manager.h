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
/// @file       manager.h
/// @brief
/// @details
/// @date       2022-09-20
/// @author     yangtao
/// @version    1.2.0
///
/// ================================================================

#ifndef IPC_MANAGER_HEADER
#define IPC_MANAGER_HEADER

#include <errno.h>
#include <isoft/ipc/client.h>
#include <isoft/ipc/ipc.h>
#include <isoft/ipc/packet.h>
#include <isoft/ipc/server.h>
#include <nai/io/nai_event.h>
#include <nai/os/nai_system.h>
#include <pthread.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "packet.h"
#include "peer.h"
#include "queue.h"
#include "server.h"
#include "server_session.h"
#include "session.h"
#include "tree.h"
#include "utility.h"

void ipc_assert_eventloop(nai_evloop_t* event_loop);

int ipc_startup(nai_evloop_t* event_loop);

int ipc_cleanup(void);

int ipc_module_cleanup_delayed(void);

int ipc_client_send(ipc_client_t* client,
                    ipc_packet_t* request,
                    ipc_packet_t** response,
                    ipc_client_handler_t handler,
                    void* context,
                    int timeout);

int ipc_server_send_response(ipc_server_t* server, ipc_packet_t* response);

typedef struct ipc_internal_message_s
{
    pid_t pid;
} ipc_internal_message_t;

#endif  // IPC_MANAGER_HEADER
