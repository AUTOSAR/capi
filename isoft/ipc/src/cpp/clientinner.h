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
/// @file       clientinner.h
/// @brief
/// @details
/// @date       2022-09-20
/// @author     yangtao
/// @version    1.2.0
///
/// ================================================================

#ifndef ISOFT_IPC_CLIENT_INNER_CPP_H
#define ISOFT_IPC_CLIENT_INNER_CPP_H

#include <isoft/ipc/client.h>

#include <cstring>
#include <list>
#include <map>
#include <mutex>

#include "isoft/ipccpp/client.h"

namespace isoft {
namespace ipc {

#define IPC_ADDRESS_LENGTH_MAX (108 - 1 + 5)

class IPCClientInner
{
private:
    struct ClientCBContext
    {
        void* context;
        IPCClientHandler handler;
        ClientCBContext() : context(nullptr){};
    };

    struct ClientConnCBContext
    {
        void* context;
        IPCClientConnectionHandler handler;
        ClientConnCBContext() : context(nullptr){};
    };

public:
    ClientCBContext* SetSendHandler(IPCClientHandler handler, void* context, ipc_client_handler_t* handlerC);
    void SetConnCallback(IPCClientConnectionHandler handler, void* context);

    bool SendCallback(void* context, ipc_client_handler_status_s status, ipc_packet_t* reponse_packet);
    bool ConnCallback(void* context, ipc_client_connection_status_t status);

private:
    friend class IPCClient;
    friend class IPCClientInnerManager;
    IPCClientInner(const char* peer_name);
    ~IPCClientInner();

private:
    ipc_client_t* _clientC;
    IPCClient* _clientCPP;
    std::list< IPCClientInner::ClientCBContext* > _handlerList;
    IPCClientInner::ClientConnCBContext* _connHandler;

    std::mutex _handlerListLock;
    std::mutex _connHandlerLock;

    char* _peerName;
};

class IPCClientInnerManager
{
public:
    static IPCClientInnerManager* GetInstance();

public:
    IPCClientInner* CreateClientInner();

    std::weak_ptr< IPCClient > GetClient(const char* peer_name);
    void InsertClient(const char* peer_name, std::weak_ptr< IPCClient > client);

    void ReleaseClient(const char* peer_name);

    void SendCallback(void* context, ipc_client_handler_status_s status, ipc_packet_t* reponse_packet);

    void ConnCallback(void* context, ipc_client_connection_status_s status);

private:
    IPCClientInnerManager() = default;
    ~IPCClientInnerManager();

private:
    static IPCClientInnerManager* _instance;

private:
    struct ptrCmp
    {
        bool operator()(const char* s1, const char* s2) const { return strcmp(s1, s2) < 0; }
    };

    std::map< const char*, std::weak_ptr< IPCClient >, ptrCmp > _clientMap;

    std::mutex _clientListLock;
};

}  // namespace ipc
}  // namespace isoft
#endif
