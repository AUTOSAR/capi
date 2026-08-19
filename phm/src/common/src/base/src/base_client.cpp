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
/// @file       base_client.cpp
/// @brief
/// @details
/// @date       2024-06-20
/// @author     wangyanlong
/// @version    1.2.0
///
/// ================================================================

#include "ara/phm/internal/com/base_client.h"

#include <isoft/ipccpp/buffer.h>
#include <isoft/ipccpp/utility.h>

#include <cerrno>
#include <cstring>

#include "ara/phm/internal/phm_log.h"

namespace ara {
namespace phm {
namespace internal {
namespace com {
namespace common {
/// @brief
/// @param addr
BaseClient::BaseClient(ara::core::String addr) noexcept
    : kAddr{std::move(addr)}, ipcClient_{nullptr}, mainLoop_{nullptr}, opened_{false}, kPid{nai_get_pid()}, mutex_{}
{
    std::cout << "BaseClient " << kAddr << std::endl;
}

/// @brief
BaseClient::~BaseClient() noexcept { std::ignore = Close(); }

/// @brief
/// @param mainLoop
/// @return
int32_t BaseClient::Open(std::shared_ptr< isoft::naicpp::EvLoop > const& mainLoop) noexcept
{
    std::lock_guard< std::mutex > const lock{mutex_};
    if (opened_) {
        LOG_INFO << kAddr.c_str() << " is already opened";
        return 0;
    }

    if (nullptr == mainLoop) {
        std::ignore
            = isoft::naicpp::GlobalGeneralEvLoop::Initialize(isoft::naicpp::GlobalGeneralEvLoop::Mode::kInsideThread);
        mainLoop_ = isoft::naicpp::GlobalGeneralEvLoop::Get();
    } else {
        mainLoop_ = mainLoop;
    }

    if (nullptr == mainLoop_) {
        LOG_ERROR << "mainloop is null";
        return -1;
    }

    int32_t const ret{isoft::ipc::IPCInitNaiUDS(mainLoop_)};
    if (ret == -1) {
        LOG_ERROR << "IPCInitNaiUDS failed";
        return -1;
    }

    ipcClient_ = isoft::ipc::IPCClient::Create(kAddr.c_str());
    if (nullptr == ipcClient_) {
        LOG_ERROR << "create ipc client failed, client addr " << kAddr.c_str();
        return -1;
    }

    if (0 != ipcClient_->Start()) {
        LOG_ERROR << "start ipc client failed, addr " << kAddr.c_str();
        ipcClient_->Release();
        std::ignore = isoft::ipc::IPCDeInitNaiUDS();
        return -1;
    }

    opened_ = true;
    LOG_INFO << "client " << kAddr.c_str() << " opened.";
    return 0;
}

/// @brief
/// @return
int32_t BaseClient::Open() noexcept { return Open(std::shared_ptr< isoft::naicpp::EvLoop >{nullptr}); }

/// @brief
/// @return
int32_t BaseClient::Close() noexcept
{
    std::lock_guard< std::mutex > const lock{mutex_};
    if (!opened_) {
        std::cout << "BaseClient::Close, client " << kAddr.c_str() << " is already closed\n";
        return 0;
    }

    if (nullptr == ipcClient_) {
        LOG_WARN << "ipc client is null";
        return -1;
    }

    int32_t ret{0};
    if (0 != ipcClient_->Stop()) {
        LOG_ERROR << "stop client " << kAddr.c_str() << " failed";
        ret = -1;
    }

    ipcClient_->Release();
    if (-1 == isoft::ipc::IPCDeInitNaiUDS()) {
        LOG_ERROR << "IPCDeInitNaiUDS failed";
        ret = -1;
    }

    mainLoop_.reset();
    opened_ = false;
    LOG_INFO << "client " << kAddr.c_str() << " closed.";
    return ret;
}

/// @brief
/// @param chunk
/// @return
isoft::ipc::IPCPacket* BaseClient::_MakeAndFillRequest(Chunk const& chunk) noexcept
{
    std::size_t const len{chunk.size()};
    if (len == static_cast< size_t >(0)) {
        LOG_ERROR << "len is 0 of client " << kAddr.c_str();
        return nullptr;
    }

    isoft::ipc::IPCPacket* const request{ipcClient_->MakeRequest()};
    if (request == nullptr) {
        LOG_ERROR << "make request failed, addr " << kAddr.c_str();
        return nullptr;
    }

    isoft::ipc::IPCPacketBufferReference* const ipcBuffer{request->AppendBuffer(len)};
    if (ipcBuffer == nullptr) {
        LOG_ERROR << "append buffer failed.";
        std::ignore = isoft::ipc::IPCPacket::Release(request);
        return nullptr;
    }

    uint8_t* const buf{ipcBuffer->GetPtr()};
    if (buf == nullptr) {
        LOG_ERROR << "ptr of buffer is null, len " << len;
        std::ignore = isoft::ipc::IPCPacket::Release(request);
        return nullptr;
    }

    std::ignore = memcpy(buf, chunk.data(), len);
    int32_t const ret{ipcBuffer->SetLen(len)};
    if (ret != 0) {
        /// TODO(wangyanlong): clang-tidy suggests that the std::strerror function may throw an exception, QAC does not recommend using errno
        LOG_ERROR << "set buffer len failed, errno " << std::strerror(errno) << ", len " << len;  // NOLINT
        return nullptr;
    }
    return request;
}

/// @brief
/// @return
bool BaseClient::IsOpened() noexcept
{
    std::lock_guard< std::mutex > const lock{mutex_};
    return opened_;
}

}  // namespace common
}  // namespace com
}  // namespace internal
}  // namespace phm
}  // namespace ara