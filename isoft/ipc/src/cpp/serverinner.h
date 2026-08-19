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
/// @file       serverinner.h
/// @brief
/// @details
/// @date       2022-09-20
/// @author     yangtao
/// @version    1.2.0
///
/// ================================================================

#ifndef ISOFT_IPC_SERVER_INNER_CPP_H
#define ISOFT_IPC_SERVER_INNER_CPP_H

#include <list>
#include <mutex>

#include "isoft/ipccpp/server.h"

namespace isoft {
namespace ipc {

class IPCServer;
class IPCServerInner
{
public:
    void SetHandler(IPCServerHandleType type, IPCServerHandler handler, void* context);

    bool Callback(void* context, ipc_server_handle_type_t type, ipc_packet_t* packet);

protected:
    friend class IPCServer;
    friend class IPCServerInnerManager;

    IPCServerInner();
    ~IPCServerInner();

private:
    ipc_server_t* _serverC;
    IPCServer* _serverCPP;

    typedef struct
    {
        void* context;
        ipc_server_handle_type_t handlerType;
        IPCServerHandler handler;
    } CBContext;

    std::list< CBContext* > _handlerList;

    std::mutex _handlerListLock;
};

class IPCServerInnerManager
{
public:
    static IPCServerInnerManager* GetInstance();

public:
    IPCServerInner* CreateServerInner();
    void DeleteServerInner(IPCServerInner* serverInner);

    void Callback(void* context, ipc_server_handle_type_t type, ipc_packet_t* packet);

private:
    IPCServerInnerManager() = default;
    ~IPCServerInnerManager();

private:
    static IPCServerInnerManager* _instance;

private:
    std::list< IPCServerInner* > _serverList;
    std::mutex _serverListLock;
};
}  // namespace ipc
}  // namespace isoft

#endif
