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
/// @date       2021-11-18
/// @author     gaohuiming
/// @version    1.2.0
///
/// ================================================================

#ifndef IPC_CLIENT_PUBLIC_HEADER
#define IPC_CLIENT_PUBLIC_HEADER

#ifdef __cplusplus
extern "C"
{
#endif

#include <isoft/ipc/packet.h>
#include <stdbool.h>

    typedef struct ipc_client_s ipc_client_t;

    /**
 * @brief Create a client handle that can access the corresponding server
 * @param address Corresponding server name
 * @return Returns client address on success, returns NULL on failure and sets errno
 */
    extern ipc_client_t *ipc_client_create(const char *address);

    /**
 * @brief Release the client
 * @param client
 */
    extern void ipc_client_release(ipc_client_t *client);

    /**
 * @brief Client starts sending and receiving messages
 * @param client
 * @return Returns 0 on success, < 0 on failure
 */
    extern int ipc_client_start(ipc_client_t *client);

    /**
 * @brief Client stops sending and receiving messages
 * @param client
 * @return Returns 0 on success
 */
    extern int ipc_client_stop(ipc_client_t *client);

    /**
 * @brief Connection status
 */
    enum ipc_client_connection_status_s
    {
        IPC_CLIENT_HANDLER_STATUS_CONNECTED = 6,
        IPC_CLIENT_HANDLER_STATUS_DISCONNECTED,
    };
    typedef enum ipc_client_connection_status_s ipc_client_connection_status_t;

    typedef void (*ipc_client_connection_handler_t)(void *context, ipc_client_connection_status_t status);

    extern void ipc_client_set_connection_handler(ipc_client_t *client,
                                                  ipc_client_connection_handler_t handler,
                                                  void *context);

    /**
 * @brief Handler data status
 */
    enum ipc_client_handler_status_s
    {
        IPC_CLIENT_HANDLER_STATUS_ERR = 1,  // Status reception error, and set errno
        IPC_CLIENT_HANDLER_STATUS_RECIEVED,
        IPC_CLIENT_HANDLER_STATUS_RECIEVED_CONTINUE,
        IPC_CLIENT_HANDLER_STATUS_RECIEVED_COMPLETED = IPC_CLIENT_HANDLER_STATUS_RECIEVED,
    };
    typedef enum ipc_client_handler_status_s ipc_client_handler_status_t;

    typedef void (*ipc_client_handler_t)(void *context,
                                         ipc_client_handler_status_t status,
                                         ipc_packet_t *reponse_packet);

    /**
 * @brief Create a request packet to send to the corresponding server
 * @param client
 * @return Returns packet address on success, returns NULL on failure
 */
    extern ipc_packet_t *ipc_client_make_request(ipc_client_t *client);

    /**
 * @brief Notification type request
 * @param client
 * @param packet Created using ipc_packet_make_request
 * @return Returns 0 on success, returns -1 and sets errno on failure
 */
    int ipc_client_post(ipc_client_t *client, ipc_packet_t *request_packet);

    /**
 * @brief Asynchronous remote call
 * @param client
 * @param timeout Specifies the maximum wait time, positive integer or -1 (unlimited), in milliseconds
 * @param is_multi_reply Specifies whether the sent request will receive multiple responses
 * @param packet Created using ipc_packet_make_request
 * @param handler Callback handler function, cannot be empty
 * @param context User-defined data
 * @return Returns 0 on success, returns -1 and sets errno on failure
 */
    int ipc_client_send_async(ipc_client_t *client,
                              int timeout,
                              bool is_multi_reply,
                              ipc_packet_t *request_packet,
                              ipc_client_handler_t handler,
                              void *context);

    /**
 * @brief Synchronous remote call
 * @param client
 * @param timeout Specifies the maximum wait time, positive integer or -1 (unlimited), in milliseconds
 * @param request_packet Created using ipc_packet_make_request
 * @param response_packet Response message handle, written on success
 * @return Returns 0 on success, returns -1 and sets errno on failure
 */
    int ipc_client_send_sync(ipc_client_t *client,
                             int timeout,
                             ipc_packet_t *request_packet,
                             ipc_packet_t **response_packet);
#ifdef __cplusplus
}
#endif
#endif
