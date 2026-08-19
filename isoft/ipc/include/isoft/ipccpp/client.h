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

#ifndef ISOFT_IPC_CLIENT_CPP_H
#define ISOFT_IPC_CLIENT_CPP_H

#include <isoft/ipc/client.h>
#include <stdbool.h>
#include <stdint.h>

#include <functional>
#include <memory>

#include "isoft/ipccpp/packet.h"

namespace isoft {
namespace ipc {

/**
 * @brief Connection status
        IPC_CLIENT_HANDLER_STATUS_CONNECTED
        IPC_CLIENT_HANDLER_STATUS_DISCONNECTED
 */
typedef enum ipc_client_connection_status_s IPCClientConnectionStatus;

/**
 * @brief Connection callback function prototype
 */
typedef std::function< void(void*, IPCClientConnectionStatus) > IPCClientConnectionHandler;

/**
 * @brief Asynchronous request status
        IPC_CLIENT_HANDLER_STATUS_TIMEOUT     =  1,
        IPC_CLIENT_HANDLER_STATUS_RECIEVED,
        IPC_CLIENT_HANDLER_STATUS_RECIEVED_CONTINUE,
        IPC_CLIENT_HANDLER_STATUS_RECIEVED_COMPLETED = IPC_CLIENT_HANDLER_STATUS_RECIEVED,
};
 */
typedef enum ipc_client_handler_status_s IPCClientHandlerStatus;

/**
 * @brief Asynchronous request callback function prototype
 */
typedef std::function< void(void*, IPCClientHandlerStatus, IPCPacket*) > IPCClientHandler;

class IPCClientInner;
class IPCClient
{
public:
    /**
     * @brief Create a Client object
     * @param peer_name Service address, format is "process name/service name"
     * @return Returns client object on success, if the client corresponding to peer_name already exists, returns the original client object. Returns empty client object on failure and sets errno
     */
    static std::shared_ptr< IPCClient > Create(const char* peer_name);

    /**
     * @brief Unregister the Client from the IPC module
     */
    void Release();

    /**
     * @brief Client starts receiving messages
     * @return Returns 0 on success, returns -1 and sets errno on failure
     */
    int Start();

    /**
     * @brief Client stops receiving messages
     * @return Returns 0 on success, returns -1 and sets errno on failure
     */
    int Stop();

    /**
     * @brief Set the connection response handler
     * @param handler Response handler
     * @param context User-defined data
     */
    void SetConnectionHandler(IPCClientConnectionHandler handler, void* context);

    /**
     * @brief Create a request message
     * @return Message handle
     */
    IPCPacket* MakeRequest();

    /**
     * @brief Notification type request
     * @param request_packet Created using MakeRequest
     * @return Returns 0 on success, returns -1 and sets errno on failure
     */
    int Post(IPCPacket* request_packet);

    /**
     * @brief Asynchronous remote call
     * @param request_packet Request message handle, created using MakeRequest
     * @param is_multi_reply Specifies whether the sent request will receive multiple responses
     * @param handler Handler for subsequent logic, cannot be empty
     * @param timeout Maximum time for response message, in milliseconds, specifies the maximum wait time, otherwise the system default value is used
     * @param context User-defined data
     * @return Returns 0 on success, returns -1 and sets errno on failure
     */
    int SendAsync(
        IPCPacket* request_packet, bool is_multi_reply, IPCClientHandler handler, const int& timeout, void* context);
    /**
     * @brief Synchronous remote call
     * @param request_packet Request message handle, created using MakeRequest
     * @param response_packet Response message handle address, written on success
     * @param timeout Maximum time for response message, in milliseconds, specifies the maximum wait time, otherwise the system default value is used
     * @return Returns 0 on success, returns -1 and sets errno on failure
     */
    int SendSync(IPCPacket* request_packet, IPCPacket** response_packet, const int& timeout);

public:
    IPCClient(const IPCClient&) = delete;

    IPCClient& operator=(const IPCClient&) = delete;

    IPCClient(IPCClient&&) = delete;

    IPCClient& operator=(IPCClient&&) = delete;

    virtual ~IPCClient();

protected:
    IPCClient(ipc_client_t* client, const char* peer_name);

private:
    friend class IPCClientInnerManager;
    IPCClientInner* _clientInner;
};
}  // namespace ipc
}  // namespace isoft

#endif
