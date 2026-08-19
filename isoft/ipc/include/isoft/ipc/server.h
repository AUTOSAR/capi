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
/// @date       2021-11-18
/// @author     gaohuiming
/// @version    1.2.0
///
/// ================================================================

#ifndef IPC_SERVER_PUBLIC_HEADER
#define IPC_SERVER_PUBLIC_HEADER

#ifdef __cplusplus
extern "C"
{
#endif

#include <isoft/ipc/packet.h>
#include <stdbool.h>

    /*
 * @brief ipc_server_t
 */
    typedef struct ipc_server_s ipc_server_t;

    /*
 * @brief Create a server object.
 * @param name Service name, 4 bytes in size
 * @param handler Message processing handler
 * @param context User-defined transparent data
 * @return Returns server object on success, otherwise returns NULL and sets errno
 */
    extern ipc_server_t *ipc_server_create(const char *name);

    /*
 * @brief Release the server object.
 * @param server Operation server object
 * @return void
 */
    extern void ipc_server_release(ipc_server_t *server);

    /*
 * @brief Run the server
 * @param server Operation server object
 * @return Returns 0 on success, returns -1 and sets errno on failure
 */
    extern int ipc_server_start(ipc_server_t *server);

    /*
 * @brief Stop the server.
 * @param server Operation server object
 * @return Returns 0 on success, returns -1 on error and sets errno
 */
    extern int ipc_server_stop(ipc_server_t *server);

    // handler type enum ipc_server_handle_type_s
    enum ipc_server_handle_type_s
    {
        IPC_SERVER_HANDLER_CONNECT = 1,
        IPC_SERVER_HANDLER_DISCONNECT,
        IPC_SERVER_HANDLER_POST,
        IPC_SERVER_HANDLER_ONCE,
        IPC_SERVER_HANDLER_MULTI,
    };

    typedef enum ipc_server_handle_type_s ipc_server_handle_type_t;
    /*
 * @brief User-defined handler prototype.
 * @param context User-defined transparent data
 * @param type Handler type
 * @param packet Packet to be processed
 * @return
 */
    typedef void (*ipc_server_handler_t)(void *context, ipc_server_handle_type_t type, ipc_packet_t *packet);

    /*
 * @brief Set the user-defined handler
 * @param server Operation server object
 * @param handler Handler for processing packets
 * @param context User-defined transparent data
 * @return
 */
    extern void ipc_server_set_handler(ipc_server_t *server,
                                       ipc_server_handle_type_t type,
                                       ipc_server_handler_t handler,
                                       void *context);

    /**
 * @brief Create a response packet with payload length len, and set the response session id
 * @param server Operation server object
 * @param session_id Confirm the target session for the response via session id
 * @return Returns response packet address on success, returns NULL on error
 */
    extern ipc_packet_t *ipc_server_make_response(ipc_server_t *server, ipc_session_id_t session_id);

    /**
 * @brief Copy a response packet using the payload of the reference packet as the source, and set the response session id
 * @param server Operation server object
 * @param session_id Confirm the target session for the response via session id
 * @param packet reference packet
 * @return Returns response packet address on success, returns NULL on error
 */
    extern ipc_packet_t *ipc_server_dup_response(ipc_server_t *server,
                                                 ipc_session_id_t session_id,
                                                 ipc_packet_t *packet);

    /**
 * @brief Send response to the corresponding client
 * @param server Operation server object
 * @param response_packet The actual response packet to send
 * @param complete Indicates whether this response packet is the last
 * @return Returns 0 on success, returns -1 on error and sets errno
 */
    extern int ipc_server_send(ipc_server_t *server, ipc_packet_t *response_packet, bool complete);

    /**
 * @brief Check if the current session id is valid
 * @param session_id Operation session id
 * @return Returns 0 if session is valid, -1 if invalid
 */
    extern int ipc_server_check_session_id(ipc_server_t *server, ipc_session_id_t session_id);

#ifdef __cplusplus
}
#endif

#endif  // IPC_SERVER_PUBLIC_HEADER
