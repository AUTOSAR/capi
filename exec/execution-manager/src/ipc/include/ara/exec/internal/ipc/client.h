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
/// @brief      IPC connection management Client class definition
/// @details
/// @date       2024-04-01
/// @author     xueliang.bao
/// @version    1.2.0
///
/// ================================================================

#ifndef SERVICE_IPC_BASE_CLIENT_H_
#define SERVICE_IPC_BASE_CLIENT_H_

#include "ara/core/string.h"
#include "isoft/ipccpp/client.h"
#include "isoft/ipccpp/packet.h"
#include "isoft/naicpp/evloop.h"

namespace ara {
namespace exec {
namespace internal {
namespace ipc {

/// @brief IPC client class
class Client
{
public:
    /// @brief Callback function definition for handling response messages
    /// @param pRspMsg Response message
    /// @param msgSize Response message size
    /// @param errorCode Error code
    using ResponseHandler = std::function< void(void *const pRspMsg, uint32_t const msgSize, int32_t const errorCode) >;

    /// @brief Default constructor
    Client() noexcept = default;

    /// @brief Default destructor
    ~Client() noexcept = default;

    /// @brief Disable copy constructor
    /// @param other Other client
    Client(Client const &other) = delete;

    /// @brief Disable move constructor
    /// @param other Other client
    Client(Client &&other) noexcept = delete;

    /// @brief Disable move assignment
    /// @param other Other client
    /// @return New client
    Client &operator=(Client &&other) = delete;

    /// @brief Disable copy assignment
    /// @param other Other client
    /// @return New client
    Client &operator=(Client const &other) = delete;

    /// @brief Custom constructor
    /// @param rspHandler Response message handler function
    explicit Client(ResponseHandler rspHandler) noexcept : fResponseHandler_{std::move(rspHandler)} {}

    /// @brief Open the IPC client
    /// @param spMainLoop Main event loop handle
    /// @param serviceProvider Service provider name
    /// @param serviceName Name of the service to connect to
    /// @return 0 success; <0 failure
    int32_t Open(std::shared_ptr< isoft::naicpp::EvLoop > spMainLoop,
                 ara::core::StringView const &serviceProvider,
                 ara::core::StringView const &serviceName) noexcept;

 /// @brief Close IPC client
 /// @return 0 success; <0 failure
    int32_t Close() noexcept;

 /// @brief Send message asynchronously
 /// @param pMsg Message to send
 /// @param msgSize Message length
 /// @param timeout Timeout setting
 /// @return 0 success; <0 failure
    int32_t SendAsync(void const *const pMsg, uint64_t const msgSize, int32_t const &timeout) noexcept
    {
        return _SendMessage(pMsg, msgSize, false, timeout);
    }

 /// @brief Send subscription message
 /// @param pMsg Message to send
 /// @param msgSize Message length
 /// @return 0 success; <0 failure
    int32_t Subscribe(void const *const pMsg, uint64_t const msgSize) noexcept
    {
        return _SendMessage(pMsg, msgSize, true, -1);
    }

 /// @brief Push message, no response required
 /// @param pMsg Message to send
 /// @param msgSize Message length
 /// @return 0 success; <0 failure
    int32_t Post(void const *const pMsg, uint32_t const msgSize) noexcept;

private:
 /// @brief Send asynchronous message
 /// @param pMsg Message to send
 /// @param msgSize Message size
 /// @param multiReply Whether multiple replies are required
 /// @param timeout Timeout duration
 /// @return 0 success; <0 failure
    int32_t _SendMessage(void const *const pMsg,
                         uint64_t const msgSize,
                         bool const multiReply,
                         int32_t const &timeout) noexcept;

 /// @brief Process response message
 /// @param pCtx Context information
 /// @param status Connection status
 /// @param pRspPacket Response message
 /// @exception std::bad_alloc Thrown when memory allocation fails
    static void HandleResponse(void *const pCtx,
                               isoft::ipc::IPCClientHandlerStatus const status,
                               isoft::ipc::IPCPacket *const pRspPacket);

private:
 /// @brief IPC client
    std::shared_ptr< isoft::ipc::IPCClient > spIpcClient_{nullptr};

 /// @brief Response message handler function
    ResponseHandler fResponseHandler_;

 /// @brief Client event loop
    std::shared_ptr< isoft::naicpp::EvLoop > spMainLoop_{nullptr};
};

}  // namespace ipc
}  // namespace internal
}  // namespace exec
}  // namespace ara

#endif  ///< SERVICE_IPC_BASE_CLIENT_H_
