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
/// @file       base_server.h
/// @brief
/// @details
/// @date       2024-06-20
/// @author     wangyanlong
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_PHM_INTERNAL_COM_COMMON_SERVER_H_
#define ARA_PHM_INTERNAL_COM_COMMON_SERVER_H_

#include <ara/core/string.h>
#include <isoft/ipccpp/packet.h>
#include <isoft/ipccpp/server.h>
#include <isoft/naicpp/global_evloop.h>
#include <nai/os/nai_proc.h>

#include <memory>
#include <mutex>

namespace ara {
namespace phm {
namespace internal {
namespace com {
namespace common {

/// @brief BaseServer, the base class of server in com.
class BaseServer
{
public:
    /// @brief Destructor.
    virtual ~BaseServer() noexcept;

    /// @brief The copy constructor shall not be used.
    /// @param obj The object to be copied.
    BaseServer(BaseServer& obj) = delete;

    /// @brief The copy assignment shall not be used.
    /// @param obj The object to be copied.
    /// @return The copied object.
    BaseServer& operator=(BaseServer const& obj) = delete;

    /// @brief The move constructor shall not be used.
    /// @param obj The object to be moved.
    BaseServer(BaseServer&& obj) = delete;

    /// @brief The move assignment shall not be used.
    /// @param obj The object to be moved.
    /// @return The moved object.
    BaseServer& operator=(BaseServer const&& obj) = delete;

    /// @brief Open ipc server.
    /// @param mainLoop event loop.
    /// @return 0, success; other failed.
    int32_t Open(std::shared_ptr< isoft::naicpp::EvLoop > const& mainLoop) noexcept;

    /// @brief Open ipc server.
    /// @return 0, success; other failed.
    int32_t Open() noexcept;

    /// @brief Close ipc server.
    /// @return 0, success; other failed.
    int32_t Close() noexcept;

    /// @brief Returns whether the server is opened.
    /// @return true, server is opened; false, server is not opened.
    bool IsOpened() noexcept;

    /// @brief Server handler shall be implemented by child class.
    /// @param packet contains data.
    virtual void ServerHandler(isoft::ipc::IPCPacket* packet) noexcept = 0;

protected:
    /// @brief Constructor.
    /// @param addr addr of server.
    explicit BaseServer(ara::core::String addr) noexcept;

private:
    /// @brief The callback of ipc server.
    /// @throws QAC
    /// @param context ipc context.
    /// @param type type of ipc event.
    /// @param packet contains data.
    static void IPCServerHandler(void* const context,
                                 isoft::ipc::IPCServerHandleType const type,
                                 isoft::ipc::IPCPacket* const packet);

    /// @brief Called when connection or disconnection occurs.
    /// @throws QAC
    /// @param context ipc context.
    /// @param type type of connection or disconnection.
    /// @param packet contains data.
    void _connectionHandler(void const* const context,
                            ::isoft::ipc::IPCServerHandleType const type,
                            ::isoft::ipc::IPCPacket* const packet) const;

protected:
    /// @brief set packet_.
    /// @param packet new packet.
    void _SetPacket(isoft::ipc::IPCPacket* const packet) noexcept;

    /// @brief returns packat_.
    /// @return packat_
    isoft::ipc::IPCPacket* const& _GetPacket() const noexcept;

    /// @brief returns ipc server.
    /// @return ipc server.
    std::shared_ptr< isoft::ipc::IPCServer > _GetIpcServer() const noexcept;

private:
    /// @brief the addr of server.
    ara::core::String const kAddr;

    /// @brief ipc server.
    std::shared_ptr< isoft::ipc::IPCServer > ipcServer_;

    /// @brief the event loop to process ipc event.
    std::shared_ptr< isoft::naicpp::EvLoop > mainLoop_;

    /// @brief ipc packet.
    isoft::ipc::IPCPacket* packet_;  /////

    /// @brief true, server is opened; false, not opened.
    bool opened_;

    /// @brief id of process this server is running in.
    nai_pid_t pid_;

    /// @brief lock
    std::mutex mutex_;
};  // class Server

}  // namespace common
}  // namespace com
}  // namespace internal
}  // namespace phm
}  // namespace ara

#endif  // ARA_PHM_INTERNAL_COM_COMMON_SERVER_H_
