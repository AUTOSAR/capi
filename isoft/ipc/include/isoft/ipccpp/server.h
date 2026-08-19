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

#ifndef ISOFT_IPC_SERVER_CPP_H
#define ISOFT_IPC_SERVER_CPP_H

#include <functional>
#include <memory>

#include "isoft/ipc/server.h"
#include "isoft/ipccpp/packet.h"

namespace isoft {

namespace ipc {

/* @brief User-defined handler type
 *IPC_SERVER_HANDLER_CONNECT
 *IPC_SERVER_HANDLER_DISCONNECT
 *IPC_SERVER_HANDLER_POST
 *IPC_SERVER_HANDLER_ONCE
 *IPC_SERVER_HANDLER_MULTI
 */

typedef enum ipc_server_handle_type_s IPCServerHandleType;

/*
 * @brief User-defined handler prototype.
 * @param void* User-defined transparent data
 * @param IPCServerHandleType Handler type
 * @param IPCPacket* Packet to be processed
 * @return
 */
typedef std::function< void(void*, IPCServerHandleType, IPCPacket*) > IPCServerHandler;

class IPCServerInner;
class IPCServer
{
public:
    /**
     * @brief Create a Server object.
     * @param name Service name, 4 bytes in size
     * @return Returns 0 on success, returns -1 and sets errno on failure
     */
    static std::unique_ptr< IPCServer > Create(const char* name);

    /**
     * @brief Unregister the service.
     * @return none
     */
    void Release();

    /**
     * @brief Run the server, start providing services externally.
     * @return Returns 0 on success, returns -1 and sets errno on failure
     */
    int Start();

    /**
     * @brief Stop providing services externally.
     * @return Returns 0 on success, returns -1 and sets errno on failure
     */
    int Stop();

public:
    /**
     * @brief Create a response message
     * @param session_id Session id corresponding to the client
     * @return Message handle
     */
    IPCPacket* MakeResponse(IPCSessionId session_id);

    /**
     * @brief Create a response packet for multi call using the packet's payload as the source
     * @param session_id Corresponding client session id
     * @param packet Source message handle
     * @return Message handle
     */
    IPCPacket* DupResponse(IPCSessionId session_id, IPCPacket* packet);

    /**
     * @brief Send response to the corresponding client
     * @param response_packet Message to be sent
     * @param complete Indicates whether this response is the last response
     * @return 0 success, -1 failure, set errno. Error could be client does not exist
     */
    int Send(IPCPacket* response_packet, bool complete);

    /**
     * @brief Set the message response handler
     * @param type Response handler type
     * @param handler Response handler
     * @param context User-defined data
     */
    void SetHandler(IPCServerHandleType type, IPCServerHandler handler, void* context);

    /**
     * @brief Check if the current session id is valid
     * @param session_id Operation session id
     * @return Returns 0 if session is valid, -1 if invalid
     */
    int CheckSessionID(IPCSessionId session_id);

public:
    IPCServer(const IPCServer&) = delete;
    IPCServer& operator=(const IPCServer&) = delete;
    IPCServer(IPCServer&&)                 = delete;
    IPCServer& operator=(IPCServer&&) = delete;
    virtual ~IPCServer();

protected:
    IPCServer(ipc_server_t* server);

private:
    IPCServerInner* _serverInner;
};
}  // namespace ipc
}  // namespace isoft
#endif
