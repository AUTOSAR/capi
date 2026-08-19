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
/// @brief      Definition of IPC connection management Server class
/// @details
/// @date       2024-04-01
/// @author     xueliang.bao
/// @version    1.2.0
///
/// ================================================================

#ifndef SERVICE_IPC_BASE_SERVER_H_
#define SERVICE_IPC_BASE_SERVER_H_

#include <cstddef>

#include "ara/core/string_view.h"
#include "isoft/ipccpp/packet.h"
#include "isoft/ipccpp/server.h"
#include "isoft/naicpp/evloop.h"

namespace ara {
namespace exec {
namespace internal {
namespace ipc {

/// @brief IPC server class, used to handle connections and requests from IpcClient
class Server
{
public:
 /// @brief Handler function for establishing or disconnecting connections
    using ConnectionHandler = std::function< void(isoft::ipc::IPCServerHandleType const) >;

 /// @brief Definition of message handler function
    using RequestHandler = std::function< void(uint32_t const, uint64_t const, void *const, uint32_t const) >;

 /// @brief Default constructor
    Server() = default;

 /// @brief Destructor
    ~Server() = default;

 /// @brief Prohibit copy construction
 /// @param other Other server
    Server(Server const &other) = delete;

 /// @brief Prohibit move construction
 /// @param other Other server
    Server(Server &&other) noexcept = delete;

 /// @brief Prohibit move assignment
 /// @param other Other server
 /// @return New server
    Server &operator=(Server &&other) = delete;

 /// @brief Prohibit copy assignment
 /// @param other Other client
 /// @return New server
    Server &operator=(Server const &other) = delete;

 /// @brief Custom constructor
 /// @param connHandler Handles connection requests
 /// @param reqHandler Handles data requests
    Server(ConnectionHandler connHandler, RequestHandler reqHandler) noexcept
        : fConnHandler_{std::move(connHandler)}, fReqHandler_{std::move(reqHandler)}
    {
    }

 /// @brief Open IPC server
 /// @param spMainLoop Main event loop handle
 /// @param serviceName Service name provided
 /// @return 0 success; <0 failure
    int32_t Open(std::shared_ptr< isoft::naicpp::EvLoop > spMainLoop,
                 ara::core::StringView const &serviceName) noexcept;

 /// @brief Close IPC server
 /// @return 0 success; <0 failure
    int32_t Close() noexcept;

 /// @brief Send response to IpcClient
 /// @param sid Session ID
 /// @param pRspMsg Response message
 /// @param msgSize Message size
 /// @param isLastRsp Whether it is the last response
 /// @return 0 success; <0 failure
    int32_t SendResponse(uint64_t const sid,
                         const void *const pRspMsg,
                         size_t const msgSize,
                         bool const isLastRsp) const noexcept;

private:
 /// @brief Handle IpcClient connection/disconnection request
 /// @param pCtx Context pointer
 /// @param type Request type
 /// @param pReqPacket Request packet
 /// @exception std::runtime_error If processing fails
    static void HandleConnection(void *const pCtx,
                                 isoft::ipc::IPCServerHandleType const type,
                                 isoft::ipc::IPCPacket const *const pReqPacket);

 /// @brief Handle IpcClient request
 /// @param pCtx Context pointer
 /// @param type Request type
 /// @param pReqPacket Request packet
 /// @exception std::runtime_error If processing fails
    static void HandleRequest(void *const pCtx,
                              isoft::ipc::IPCServerHandleType const type,
                              isoft::ipc::IPCPacket *const pReqPacket);

private:
    /// @brief ipc server handler
    std::unique_ptr< isoft::ipc::IPCServer > upIpcServer_{nullptr};

 /// @brief IpcClient connection/disconnection request handler
    ConnectionHandler fConnHandler_{nullptr};

 /// @brief IpcClient request handler
    RequestHandler fReqHandler_{nullptr};

 /// @brief Server event loop
    std::shared_ptr< isoft::naicpp::EvLoop > spMainLoop_{nullptr};
};

}  // namespace ipc
}  // namespace internal
}  // namespace exec
}  // namespace ara

#endif  ///< SERVICE_IPC_BASE_SERVER_H_
