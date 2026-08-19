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
/// @file       packet.h
/// @brief
/// @details
/// @date       2022-09-20
/// @author     yangtao
/// @version    1.2.0
///
/// ================================================================

#ifndef IPC_PACKET_PUBLIC_HEADER
#define IPC_PACKET_PUBLIC_HEADER

#ifdef __cplusplus
extern "C"
{
#endif

#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

    typedef struct ipc_buffer_s ipc_buffer_t;

    typedef struct ipc_packet_s ipc_packet_t;

    /**
 * @brief Get the buffer of the packet
 */

    /**
 * @brief Get the buffer of the packet
 * @param packet Operation packet object
 * @return Returns buffer address on success, returns NULL on error
 */
    extern ipc_buffer_t *ipc_packet_get_buffer(ipc_packet_t *packet);

    /**
 * @brief Append to the packet buffer
 * @param packet Operation packet object
 * @param len Buffer size
 * @return Returns buffer address on success, returns NULL on error
 */
    extern ipc_buffer_t *ipc_packet_append_buffer(ipc_packet_t *packet, size_t len);

    /**
 * @breif user-defined free function
 */
    typedef void (*ipc_free_func_t)(void *context, uint8_t *addr, size_t size);

    /**
 * @brief Append user-defined buffer
 * @param packet Operation packet object
 * @param buf User-defined buffer address
 * @param len User-defined buffer size
 * @param len User-defined buffer free function
 * @return Returns buffer address on success, returns NULL on error
 */
    extern ipc_buffer_t *ipc_packet_append_user_buffer(ipc_packet_t *packet,
                                                       void *buf,
                                                       size_t len,
                                                       ipc_free_func_t free);

    /**
 * @brief Get the actual operation address in the buffer
 * @param buffer Operation buffer object
 * @return Returns the actual operation address
 */
    extern uint8_t *ipc_buffer_get_ptr(ipc_buffer_t *buffer);

    /**
 * @brief Get the next buffer of the specified buffer object in the packet
 * @param packet Operation packet object
 * @param buffer Operation buffer object
 * @return Returns buffer address on success, otherwise returns NULL
 */
    extern ipc_buffer_t *ipc_buffer_get_next(ipc_packet_t *packet, ipc_buffer_t *buffer);

    /**
 * @brief Get the used length of the buffer
 * @param buffer Operation buffer object
 * @return Returns the actual used memory size of the buffer
 */
    extern size_t ipc_buffer_get_len(ipc_buffer_t *buffer);

    /**
 * @brief Set the used length of the buffer
 * @param buffer Operation buffer object
 * @param len Set used size of the buffer
 * @return Returns 0 on success, returns -1 on error
 */
    extern int ipc_buffer_set_len(ipc_buffer_t *buffer, size_t len);

    /**
 * @brief Get the allocated length of the buffer
 * @param buffer Operation buffer object
 * @return Returns the actual allocated memory size of the buffer
 */
    extern size_t ipc_buffer_get_size(ipc_buffer_t *buffer);

    /**
 * @breif packet ref
 * @param packet Operation packet object
 * @return Returns the packet
 */
    extern ipc_packet_t *ipc_packet_ref(ipc_packet_t *packet);

    /**
 * @breif packet release
 * @param packet Operation packet object
 * @return Returns the packet
 */
    extern void ipc_packet_release(ipc_packet_t *packet);

    /**
 * @brief Get the sender's pid of the packet
 * @param packet Operation packet object
 * @return Returns the pid
 */
    extern pid_t ipc_packet_get_peer_pid(ipc_packet_t *packet);

    /**
 * @brief Define session id
 */
    typedef uint64_t ipc_session_id_t;

    /**
 * @brief Read session id from request packet
 * @param packet Operation packet object
 * @return Returns the session id
 */
    extern ipc_session_id_t ipc_packet_get_session_id(ipc_packet_t *packet);

#ifdef __cplusplus
}
#endif
#endif
