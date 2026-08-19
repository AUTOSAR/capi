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
/// @file       base_client.h
/// @brief
/// @details
/// @date       2024-06-20
/// @author     wangyanlong
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_PHM_INTERNAL_COM_COMMON_CLIENT_H_
#define ARA_PHM_INTERNAL_COM_COMMON_CLIENT_H_

#include <ara/core/string.h>
#include <isoft/ipccpp/client.h>
#include <isoft/naicpp/evloop.h>
#include <isoft/naicpp/global_evloop.h>
#include <nai/os/nai_proc.h>

#include <memory>
#include <mutex>

#include "ara/phm/internal/types.h"

namespace ara {
namespace phm {
namespace internal {
namespace com {
namespace common {

/// @brief BaseClient, the base class of client in com.
class BaseClient
{
public:
    /// @brief Destructor.
    virtual ~BaseClient() noexcept;

    /// @brief The copy constructor shall not be used.
    /// @param obj The object to be copied.
    BaseClient(BaseClient& obj) = delete;

    /// @brief The copy assignment shall not be used.
    /// @param obj The object to be copied.
    /// @return The copied object.
    BaseClient& operator=(BaseClient const& obj) = delete;

    /// @brief The move constructor shall not be used.
    /// @param obj The object to be moved.
    BaseClient(BaseClient&& obj) = delete;

    /// @brief The move assignment shall not be used.
    /// @param obj The object to be moved.
    /// @return The moved object.
    BaseClient& operator=(BaseClient const&& obj) = delete;

    /// @brief Open ipc client.
    /// @param mainLoop event loop.
    /// @return 0, success; other failed.
    int32_t Open(std::shared_ptr< isoft::naicpp::EvLoop > const& mainLoop) noexcept;

    /// @brief Open ipc client.
    /// @return 0, success; other failed.
    int32_t Open() noexcept;

    /// @brief Close ipc client.
    /// @return 0, success; other failed.
    int32_t Close() noexcept;

    /// @brief Returns whether the client is opened.
    /// @return true, client is opened; false, client is not opened.
    bool IsOpened() noexcept;

    /// @brief returns addr.
    /// @return addr
    ara::core::String GetAddr() const noexcept { return kAddr; }

    /// @brief returns ipc client.
    /// @return ipc client.
    std::shared_ptr< isoft::ipc::IPCClient > GetIpcClient() const noexcept { return ipcClient_; }

    /// @brief returns main loop.
    /// @return main loop.
    std::shared_ptr< isoft::naicpp::EvLoop > GetMainLoop() const noexcept { return mainLoop_; }

    /// @brief returns opend.
    /// @return opend.
    bool GetOpened() const noexcept { return opened_; }

    /// @brief returns pid.
    /// @return pid.
    nai_pid_t GetPid() const noexcept { return kPid; }

protected:
    /// @brief Construct a BaseClient.
    /// @param addr addr of client.
    explicit BaseClient(ara::core::String addr) noexcept;

    /// @brief make and fill the request of ipc.
    /// @param chunk data to put into request.
    /// @return isoft::ipc::IPCPacket* const the request.
    isoft::ipc::IPCPacket* _MakeAndFillRequest(Chunk const& chunk) noexcept;

private:
    /// @brief the addr of client.
    ara::core::String const kAddr;

    /// @brief ipc client.
    std::shared_ptr< isoft::ipc::IPCClient > ipcClient_;

    /// @brief the event loop to process ipc event.
    std::shared_ptr< isoft::naicpp::EvLoop > mainLoop_;

    /// @brief true, client is opened; false, client is not opened.
    bool opened_;

    /// @brief id of process this client is running in.
    nai_pid_t const kPid;

    /// @brief lock.
    std::mutex mutex_;
};  // class Client

}  // namespace common
}  // namespace com
}  // namespace internal
}  // namespace phm
}  // namespace ara

#endif  // ARA_PHM_INTERNAL_COM_COMMON_CLIENT_H_
