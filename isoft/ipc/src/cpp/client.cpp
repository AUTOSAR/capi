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
/// @file       client.cpp
/// @brief
/// @details
/// @date       2022-09-20
/// @author     yangtao
/// @version    1.2.0
///
/// ================================================================

#include "isoft/ipccpp/client.h"

#include <iostream>

#include "clientinner.h"
#include "packetinner.h"
using namespace std;
namespace isoft {
namespace ipc {

typedef void (*ipc_client_handler_t)(void *context, ipc_client_handler_status_t status, ipc_packet_t *reponse_packet);

std::shared_ptr< IPCClient > IPCClient::Create(const char *peer_name)
{
    std::shared_ptr< IPCClient > client;
    client = IPCClientInnerManager::GetInstance()->GetClient(peer_name).lock();
    if (client == nullptr) {
        ipc_client_t *raw{ipc_client_create(peer_name)};
        if (raw != nullptr) {
            client.reset(new IPCClient(raw, peer_name));
            IPCClientInnerManager::GetInstance()->InsertClient(peer_name, client);
        }
    } else if (client->_clientInner->_clientC == nullptr) {
        ipc_client_t *raw{ipc_client_create(peer_name)};
        if (raw != nullptr) {
            client->_clientInner->_clientC = raw;
        }
    }
    return client;
}

IPCClient::IPCClient(ipc_client_t *client, const char *peer_name)
{
    _clientInner = new IPCClientInner(peer_name);
    if (_clientInner != nullptr) {
        _clientInner->_clientC   = client;
        _clientInner->_clientCPP = this;
    }
}

IPCClient::~IPCClient()
{
    IPCClientInnerManager::GetInstance()->ReleaseClient(_clientInner->_peerName);
    if (_clientInner != nullptr)
        delete _clientInner;
    _clientInner = nullptr;
}

void IPCClient::Release()
{
    if (_clientInner->_clientC != nullptr) {
        ipc_client_release(_clientInner->_clientC);
    }
    _clientInner->_clientC = nullptr;
}

/**
 * @brief Client starts receiving messages
 */
int IPCClient::Start()
{
    if (_clientInner->_clientC != nullptr)
        return ipc_client_start(_clientInner->_clientC);
    return -1;
}

/**
 * @brief Client stops receiving messages
 */
int IPCClient::Stop()
{
    if (_clientInner->_clientC != nullptr) {
        return ipc_client_stop(_clientInner->_clientC);
    }
    return 0;
}

void IPCClient::SetConnectionHandler(IPCClientConnectionHandler handler, void *context)
{
    if (_clientInner != nullptr)
        _clientInner->SetConnCallback(handler, context);
}

IPCPacket *IPCClient::MakeRequest()
{
    if (_clientInner->_clientC != nullptr) {
        ipc_packet_t *packetC = ipc_client_make_request(_clientInner->_clientC);
        if (packetC != nullptr) {
            return IPCPacket::Create(packetC);
        }
    }
    return nullptr;
}

// notify
int IPCClient::Post(IPCPacket *request_packet)
{
    int status = -1;
    if (_clientInner->_clientC != nullptr)
        status = ipc_client_post(_clientInner->_clientC, request_packet->GetData());

    IPCPacketInnerManager::GetInstance()->ReleasePacket(request_packet);
    return status;
}

// call
int IPCClient::SendAsync(
    IPCPacket *request_packet, bool is_multi_reply, IPCClientHandler handler, const int &timeout, void *context)
{
    int status = -1;
    if (_clientInner->_clientC != nullptr) {
        ipc_client_handler_t inner_handler;
        IPCClientInner::ClientCBContext *cb = _clientInner->SetSendHandler(handler, context, &inner_handler);

        status = ipc_client_send_async(_clientInner->_clientC, timeout, is_multi_reply, request_packet->GetData(),
                                       inner_handler, cb);
    }

    IPCPacketInnerManager::GetInstance()->ReleasePacket(request_packet);
    return status;
}

int IPCClient::SendSync(IPCPacket *request_packet, IPCPacket **response_packet, const int &timeout)
{
    int r = -1;
    if (_clientInner->_clientC != nullptr) {
        ipc_packet_t *resC = NULL;
        r                  = ipc_client_send_sync(_clientInner->_clientC, timeout, request_packet->GetData(), &resC);
        if (r == 0) {
            *response_packet = IPCPacket::Create(resC);
        }
    }
    IPCPacketInnerManager::GetInstance()->ReleasePacket(request_packet);
    return r;
}
}  // namespace ipc
}  // namespace isoft
