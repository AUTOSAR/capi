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
/// @file       raw_ethernet_stream.cpp
/// @brief
/// @details
/// @date       2021-12-02
/// @author     cuijiusen
/// @version    1.2.0
///
/// ================================================================

#include <net/if.h>

#include <cstddef>
#include <iostream>
#include <mutex>
#include <unordered_map>

#include "ara/com/com_error_domain.h"
#include "ara/com/internal/raw/raw_stream_ethernet.h"
#include "ara/com/raw/raw_data_stream.h"
#include "ara/com/raw/raw_error_domain.h"
#include "ara/core/promise.h"
#include "isoft/naicpp/global_evloop.h"
#ifdef HAS_ARA_IAM
    #include "ara/iam/internal/grantquery/raw.h"
#else
#endif
#include "nai/io/nai_event.h"
#include "nai/io/nai_signal.h"
#include "nai/os/nai_proc.h"
#include "nai/runtime/nai_errno.h"

inline char* GetErrorMsg() noexcept
{
    return strerror(nai_errno);  // NOLINT -- compatibility>[concurrency-mt-unsafe]
}
namespace ara {
namespace com {
namespace internal {
namespace raw {
static std::mutex g_Server2RawMutex;
static std::unordered_map< nai_server_t*, RawEthernetTcp* > g_Server2Raw;  // NOLINT
static std::mutex g_Stream2RawMutex;
static std::unordered_map< nai_stream_t*, RawEthernetTcp* > g_Stream2Raw;  // NOLINT

/// @brief Raw byte stream TCP protocol constructor
/// @param[in] instance
/// @exception
RawEthernetTcp::RawEthernetTcp(ara::core::InstanceSpecifier const& instance) noexcept
{
    config_ = RawRuntime::GetRawConfig(instance);
#ifdef HAS_ARA_IAM
    // iam load
    ara::core::Result< void > resRaw = ara::iam::internal::grant::IAMGrantRawQuery::Initialize();
    if (!resRaw) {
        ComLogWarning("RawEthernetTcp get iam initialize failed!");
    }
#else
#endif
}

/// @brief Raw byte stream TCP protocol destructor
/// @exception
RawEthernetTcp::~RawEthernetTcp() noexcept
{
    //[SWS_CM_10483] Destructor of the RawDataStreamClient that deletes the RawDataStreamClient instance.
    // If the connection is still open, the connection should be closed and shut down before destroying
    // the RawDataStreamClient object.
    {
        std::lock_guard< std::mutex > g(g_Server2RawMutex);
        auto it = g_Server2Raw.find(&server_);
        if (it != g_Server2Raw.end()) {
            g_Server2Raw.erase(it);
        }
    }
    {
        std::lock_guard< std::mutex > g(g_Stream2RawMutex);
        auto it = g_Stream2Raw.find(&fd_);
        if (it != g_Stream2Raw.end()) {
            g_Stream2Raw.erase(it);
        }
    }
    std::ignore = nai_server_close(&server_);
    std::ignore = nai_stream_close(&fd_);
#ifdef HAS_ARA_IAM
    ara::iam::internal::grant::IAMGrantRawQuery::Deinitialize();
#else
#endif
    _SetConnectionState(ConnectionState::kRawConnectionStateNotAvailable);
}

/// @brief Raw byte stream TCP protocol server create connection function
/// @param[in] timeout
/// @return
/// @exception
ara::core::Result< void > RawEthernetTcp::WaitForConnection(int64_t timeout) noexcept
{
    ComLogInfo("RawEthernetTcp WaitForConnection in!");
    ara::com::raw::RawErrc errCode{ara::com::raw::RawErrc::kUnknownError};
    // get evloop
    loop_     = isoft::naicpp::GlobalGeneralEvLoop::Get();
    int32_t r = -1;
    do {
        if (nullptr == config_.get()) {
            // Configuration not configured or loaded incorrectly
            errCode = ara::com::raw::RawErrc::kAddressNotAvailable;
            ComLogError("config_.get() is null!");
            break;
        }
        if (_GetConnectionState() == ConnectionState::kRawConnectionStateConnect) {
            // Existing connection
            errCode = ara::com::raw::RawErrc::kStreamAlreadyConnected;
            ComLogError("_GetConnectionState already connected!");
            break;
        }
        r = nai_server_init(&server_);
        if (r < 0) {
            ComLogError("nai_server_init error:", nai_errno);
            errCode = ara::com::raw::RawErrc::kStreamNotConnected;
            break;
        }

        nai_socknbuf_t addr;
        nai_server_set_backlog(&server_, 10);
        addr.len = static_cast< int32_t >(sizeof(addr.storage));
        ara::core::String const address{config_->localAddress + ":"
                                        + std::to_string(static_cast< int32_t >(config_->tcpPort))};
        r = nai_sockaddr_pton(address.data(), address.length(), &addr.addr, &addr.len);
        if (r < 0) {
            ComLogError("nai_sockaddr_pton error:", nai_errno);
            errCode = ara::com::raw::RawErrc::kAddressNotAvailable;
            break;
        }

        // accept process in this.
        // register mapping and accept callback
        {
            std::lock_guard< std::mutex > g(g_Server2RawMutex);
            g_Server2Raw[&server_] = this;
        }
        std::ignore = nai_server_set_cb(&server_, [](nai_server_t* p, int32_t events) {
            ComLogInfo("server get events is ", events);
            RawEthernetTcp* server{nullptr};
            {
                std::lock_guard< std::mutex > g(g_Server2RawMutex);
                auto it = g_Server2Raw.find(p);
                if (it != g_Server2Raw.end()) {
                    server = it->second;
                }
            }
            if (server == nullptr) {
                ComLogError("server get events but no server mapping", events);
                return static_cast< int32_t >(-1);
            }
            return server->_rawAccept();
        });

        // no blocking mode.
        r = nai_server_bind(&server_, loop_->GetRawEvLoop(), &addr.addr, addr.len);
        if (r < 0) {
            ComLogError("nai_server_bind error:", nai_errno);
            errCode = ara::com::raw::RawErrc::kAddressNotAvailable;
            break;
        }

        std::unique_lock< std::mutex > lock(rawMux_);
        if (timeout == -1) {
            cv_.wait(lock);
        } else {
            // Calculate wait_for end wait time. If timeout, record errCode. Otherwise, consider it was normally interrupted during wait.
            auto start  = std::chrono::system_clock::now();
            std::ignore = cv_.wait_for(lock, std::chrono::milliseconds(timeout));
            auto end    = std::chrono::system_clock::now();
            std::chrono::milliseconds duration{std::chrono::duration_cast< std::chrono::milliseconds >(end - start)};
            if (duration.count() >= timeout) {
                errCode = ara::com::raw::RawErrc::kCommunicationTimeout;
                break;
            }
        }
        return ara::core::Result< void >::FromValue();
    } while (false);
    ComLogInfo("WaitForConnection end ");
    return ara::core::Result< void >::FromError(errCode);
}

/// @brief Raw byte stream TCP protocol server create connection function
/// @return success:0 falied: -1
int32_t RawEthernetTcp::_rawAccept() noexcept
{
    ComLogInfo("_rawAccept in! ");
    nai_sockaddr_t name;
    int32_t nameLen{};
    int32_t r{};
    int32_t ret{};
    while (true) {
        // ara::com::raw::RawErrc errCode;
        nai_fd_t f{nai_server_accept(&server_, &name, &nameLen)};
        if (f == static_cast< nai_fd_t >(NAI_FD_INVALID)) {
            ComLogError(GenArg(nai_errno), GetErrorMsg());
            ComLogError("RawEthernetTcp::nai_server_accept return failed!!");
            std::ignore = nai_server_close(&server_);
            ret         = -1;
            break;
        };
        r = nai_stream_init(&fd_);
        if (r < 0) {
            ComLogError(GenArg(nai_errno), GetErrorMsg());
            ComLogError("RawEthernetTcp::nai_stream_init return failed!!");
            ret = -1;
            break;
        }
        r = nai_stream_set_fd(&fd_, f, NAI_FD_TYPE_SOCK);
        if (r < 0) {
            ComLogError(GenArg(nai_errno), GetErrorMsg());
            ComLogError("RawEthernetTcp::nai_stream_set_fd return failed!!");
            ret = -1;
            break;
        }

        // set accept cb.
        // register mapping and accept callback
        {
            std::lock_guard< std::mutex > g(g_Stream2RawMutex);
            g_Stream2Raw[&fd_] = this;
        }
        r = nai_stream_set_cb(&fd_, [](nai_stream_t* const p, std::int32_t const events) {
            ComLogInfo("tcp get events is ", events);
            RawEthernetTcp* server{nullptr};
            {
                std::lock_guard< std::mutex > g(g_Stream2RawMutex);
                auto it = g_Stream2Raw.find(p);
                if (it != g_Stream2Raw.end()) {
                    server = it->second;
                }
            }
            if (server == nullptr) {
                ComLogError("tcp get events but no server mapping", events);
                return static_cast< int32_t >(-1);
            }
            return server->_acceptCb(events);
        });
        if (r < 0) {
            ComLogError(GenArg(nai_errno), GetErrorMsg());
            ComLogError("RawEthernetTcp::nai_stream_set_cb return failed!!");
            ret = -1;
            break;
        }
        r = nai_stream_set_fdown(&fd_, 1);
        int32_t rOpen{nai_stream_open(&fd_, loop_->GetRawEvLoop())};
        if (rOpen < 0) {
            ComLogError(GenArg(nai_errno), GetErrorMsg());
            ComLogError("RawEthernetTcp::nai_stream_open return failed!!");
            ret = -1;
            break;
        };
        _SetConnectionState(ConnectionState::kRawConnectionStateConnect);
        ret = 0;
        break;
    }
    cv_.notify_all();
    ComLogInfo("_rawAccept end!");
    return ret;
}

/// @brief Raw byte stream TCP protocol accept processing callback function
/// @param[in] data
/// @param[in] maxLength
/// @param[in] timeout
/// @return success:0 failed:-1
/// @exception
int32_t RawEthernetTcp::_acceptCb(std::int32_t const events) noexcept
{
    ComLogInfo("acceptCallBack cb. events:", events);
    if (NAI_EV_ERROR == (events & NAI_EV_ERROR)) {
        ComLogInfo("acceptCallBack event error", GenArg(nai_errno), GetErrorMsg());
        // rawCode = ara::com::raw::RawErrc::kUnknownError;
    };
    if (NAI_EV_READ == (events & NAI_EV_READ)) {
        std::unique_lock< std::mutex > lock(rawMux_);
        // Every time there is a read event, this flag is set to true.
        rdFlag_ = true;
        cv_.notify_all();
    }
    if (NAI_EV_WRITE == (events & NAI_EV_WRITE)) {
        ComLogInfo("acceptCallBack write event get!");
    }
    if (NAI_EV_ERROR == (events & NAI_EV_ERROR)) {
        ComLogInfo("acceptCallBack client close!!");
    }

    if (NAI_EV_TIMEOUT == (events & NAI_EV_TIMEOUT)) {
        switch (events & (NAI_EV_READ | NAI_EV_WRITE)) {
            case NAI_EV_READ:
                ComLogInfo("acceptCallBack stream read timeout!");
                break;
            case NAI_EV_WRITE:
                ComLogInfo("acceptCallBack stream write timeout!");
                break;
            default:
                break;
        };
    };

    if (NAI_EV_NOTIFY == (events & NAI_EV_NOTIFY)) {
        std::int32_t notifyId = nai_ev_notify_code(events);
        switch (notifyId) {
            // user notify read .
            case 0:
                ComLogInfo("user define read!");
                if (readNotify_) {
                    readNotify_();
                    readNotify_ = []() {};
                }
                break;
            // user nofity write.
            case 1:
                ComLogInfo("user define write!");
                if (writeNotify_) {
                    writeNotify_();
                    writeNotify_ = []() {};
                }
                break;
            // user notify close.
            case 2:
                ComLogInfo("user define close!");
                if (closeNotify_) {
                    closeNotify_();
                    closeNotify_ = []() {};
                }
                break;
            default:
                break;
        }
    }
    return 0;
}
/// @brief Raw byte stream TCP protocol write data function
/// @param[in] data
/// @param[in] maxLength
/// @param[in] timeout
/// @return Result<size_t> error : success
/// @exception
ara::core::Result< size_t > RawEthernetTcp::WriteData(ara::com::SamplePtr< uint8_t > data,
                                                      size_t maxLength,
                                                      int64_t timeout) noexcept
{
    ComLogInfo("RawEthernetTcp::WriteData in!");
    ara::core::Promise< size_t > promise1{};
    auto future = promise1.get_future();
    do {
        if (_GetConnectionState() != ConnectionState::kRawConnectionStateConnect) {
            promise1.SetError(ara::com::raw::RawErrc::kStreamNotConnected);
            ComLogError("_GetConnectionState is not connected!");
            break;
        }

        writeNotify_ = [this, &promise1, &data, &maxLength]() {
            intptr_t r{nai_stream_write(&fd_, data.Get(), maxLength)};
            if (r > static_cast< intptr_t >(0)) {
                promise1.set_value(r);
            }
            // close socket.
            else if (r == 0) {
                ComLogError("WriteData peer closed.");
                promise1.SetError(ara::com::raw::RawErrc::kConnectionClosedByPeer);
            } else {
                if ((nai_errno == NAI_EAGAIN) || (nai_errno == EINPROGRESS)) {
                    ComLogInfo(GenArg(nai_errno), GetErrorMsg());
                    // Asynchronous write operation, when returning EAGAIN or EINPROGRESS, returns 0. User decides whether to write data again.
                    promise1.set_value(0);
                } else {
                    ComLogError("WriteData r < 0");
                    promise1.SetError(ara::com::raw::RawErrc::kInterruptedBySignal);
                }
            }
        };
        // Notify io thread.
        if (nai_stream_post(&fd_, 1) < 0) {
            ComLogError("WriteData nai_stream_post failed!");
            promise1.SetError(ara::com::raw::RawErrc::kInterruptedBySignal);
            break;
        }
        if (timeout == -1) {
            future.wait();
        } else {
            ara::core::future_status status;
            status = future.wait_for(std::chrono::milliseconds(timeout));
            if (status == ara::core::future_status::timeout) {
                ComLogError("WriteData future wait timeout.");
                promise1.SetError(ara::com::raw::RawErrc::kCommunicationTimeout);
                break;
            }
        }
    } while (false);
    return future.GetResult();
}

/// @brief Raw byte stream TCP protocol read data function
/// @param[in] maxLength
/// @param[in] timeout
/// @return ReadDataResult error : success
/// @exception
ara::core::Result< ReadDataResult > RawEthernetTcp::ReadData(size_t maxLength, int64_t timeout) noexcept
{
    ComLogInfo("RawEthernetTcp::ReadData in!");
    ara::com::SamplePtr< uint8_t > data{new uint8_t[maxLength], [](uint8_t const* ptr) { delete[] ptr; }};
    std::ignore = nai_memset(data.Get(), 0, maxLength);
    ara::core::Promise< ReadDataResult > promise1{};
    auto future = promise1.get_future();
    bool timeoutFlag{false};
    size_t numberOfBytes{};
    do {
        // If the user does not call WaitForConnection, the socket status is incorrect, should exit directly with error.
        if (_GetConnectionState() != ConnectionState::kRawConnectionStateConnect) {
            ComLogError("ReadData: _GetConnectionState is not aveilable!");
            promise1.SetError(MakeErrorCode(ara::com::raw::RawErrc::kStreamNotConnected, __LINE__));
            break;
        }
        std::unique_lock< std::mutex > lock(rawMux_);
        if (timeout == -1) {
            cv_.wait(lock, [this] {
                if ((this->rdFlag_) || (this->downReady_)) {
                    return true;
                };
                return false;
            });
        } else {
            std::ignore = cv_.wait_for(lock, std::chrono::milliseconds(timeout), [this, &timeoutFlag] {
                if ((this->rdFlag_) || (this->downReady_)) {
                    return true;
                };
                timeoutFlag = true;
                return false;
            });
        }

        if (timeoutFlag) {
            promise1.SetError(MakeErrorCode(ara::com::raw::RawErrc::kCommunicationTimeout, __LINE__));
            break;
        }
        ComLogInfo("recv notify");

        readNotify_ = [this, &promise1, &data, &numberOfBytes, &maxLength]() {
            intptr_t r1 = nai_stream_read(&fd_, static_cast< void* >(data.Get()), maxLength);
            if (r1 > 0 && r1 < static_cast< intptr_t >(maxLength)) {
                numberOfBytes = static_cast< size_t >(r1);
                rdFlag_       = false;
                promise1.set_value(ReadDataResult{std::move(data), numberOfBytes});
            } else if (r1 > 0 && r1 >= static_cast< intptr_t >(maxLength)) {
                numberOfBytes = static_cast< size_t >(r1);
                promise1.set_value(ReadDataResult{std::move(data), numberOfBytes});
            } else if (r1 == 0) {
                ComLogError("peer closed:");
                promise1.SetError(MakeErrorCode(ara::com::raw::RawErrc::kConnectionClosedByPeer, __LINE__));
            } else {
                // if socket error is EAGAIN then read again.
                if (nai_errno == NAI_EAGAIN) {
                    ComLogInfo("ReadData EAGAIN! please read again!");
                    promise1.set_value(ReadDataResult{std::move(data), 0});
                } else {
                    ComLogError(GenArg(nai_errno), GetErrorMsg());
                    _SetConnectionState(ConnectionState::kRawConnectionStateNotAvailable);
                    promise1.SetError(MakeErrorCode(ara::com::raw::RawErrc::kConnectionRefused, __LINE__));
                }
            }
        };
        // Notify io thread.
        if (nai_stream_post(&fd_, 0) < 0) {
            promise1.SetError(MakeErrorCode(ara::com::raw::RawErrc::kInterruptedBySignal, __LINE__));
            break;
        }
        if (timeout != -1) {
            ara::core::future_status status;
            status = future.wait_for(std::chrono::milliseconds(timeout));
            // Timeout occurred and future not returned. Return timeout error.
            if (status == ara::core::future_status::timeout) {
                promise1.SetError(MakeErrorCode(ara::com::raw::RawErrc::kCommunicationTimeout, __LINE__));
                break;
            }
        } else {
            future.wait();
        }
    } while (false);
    return future.GetResult();
}

/// @brief Raw byte stream TCP protocol close connection function
/// @param[in] timeout
/// @return
/// @exception
ara::core::Result< void > RawEthernetTcp::Shutdown(int64_t timeout) noexcept
{
    // ara::core::Result<void> res;
    ara::core::Promise< void > promise1;
    std::atomic_bool errFlag{false};
    auto future = promise1.get_future();
    do {
        downReady_ = true;
        rdFlag_    = false;
        if (_GetConnectionState() == ConnectionState::kRawConnectionStateConnect) {
            _SetConnectionState(ConnectionState::kRawConnectionStateWaitForConnect);
        } else {
            promise1.SetError(MakeErrorCode(ara::com::raw::RawErrc::kStreamNotConnected, __LINE__));
        }
        closeNotify_ = [this, &promise1, &errFlag]() {
            if ((nai_server_close(&server_) < 0) || (nai_stream_close(&fd_) < 0)) {
                ComLogError("Raw Stream ShutDown listen and accept fd faile!");
                ComLogError(GenArg(nai_errno), GetErrorMsg());
                promise1.SetError(MakeErrorCode(ara::com::raw::RawErrc::kInterruptedBySignal, __LINE__));
                errFlag = true;
            } else {
                ComLogInfo("close listen && accept fd success.");
                promise1.set_value();
            }
        };
        // Notify io thread.
        if (nai_stream_post(&fd_, 2) < 0) {
            ComLogError("nai_stream_post send singal failed!");
            if (!errFlag) {
                promise1.SetError(MakeErrorCode(ara::com::raw::RawErrc::kInterruptedBySignal, __LINE__));
            }
            break;
        }

        if (timeout != -1) {
            ara::core::future_status status;
            status = future.wait_for(std::chrono::milliseconds(timeout));
            // Timeout occurred and future not returned. Return timeout error.
            if (status == ara::core::future_status::timeout) {
                if (!errFlag) {
                    promise1.SetError(MakeErrorCode(ara::com::raw::RawErrc::kCommunicationTimeout, __LINE__));
                }
                break;
            }
        } else {
            future.wait();
        }

        // Notify read wait part to exit.
        std::unique_lock< std::mutex > lock(rawMux_);
        cv_.notify_all();

        this->readNotify_  = nullptr;
        this->writeNotify_ = nullptr;
        this->closeNotify_ = nullptr;
        // clear socket related configuration.
        // nai_memset(&server_,0,sizeof(server_));
        // nai_memset(&fd_,0,sizeof(fd_));
        // close evloop.
        this->loop_.reset();
        return ara::core::Result< void >::FromValue();
    } while (false);
    return future.GetResult();
}

/// @brief Raw byte stream TCP protocol client connection function
/// @param[in] timeout
/// @return error : success
/// @exception
ara::core::Result< void > RawEthernetTcp::Connect(int64_t timeout) noexcept
{
    ComLogTrace("RawEthernetTcp:Connect.");
    ara::com::raw::RawErrc errCode{ara::com::raw::RawErrc::kUnknownError};
    // get evloop
    loop_ = isoft::naicpp::GlobalGeneralEvLoop::Get();
    do {
        if (nullptr == config_.get()) {
            // Configuration not configured or loaded incorrectly
            ComLogError("connect: config_.get is null!");
            errCode = ara::com::raw::RawErrc::kAddressNotAvailable;
            break;
        }
#ifdef HAS_ARA_IAM
        // iam check start--->
        int32_t nPid = nai_get_pid();
        if (!ara::iam::internal::grant::IAMGrantRawQuery::HasRawGrant(nPid, config_->remoteAddress, config_->tcpPort, 0,
                                                                      0)) {
            ComLogError("raw tcp iam check failed!");
            return ara::core::Result< void >::FromError(ara::com::ComErrc::kGrantEnforcementError);
        }
        ComLogInfo("raw tcp iam check success!");
        // iam check end---->
#else
#endif
        if (_GetConnectionState() == ConnectionState::kRawConnectionStateConnect) {
            errCode = ara::com::raw::RawErrc::kStreamAlreadyConnected;
            break;
        }
        std::ignore = nai_stream_init(&fd_);

        nai_socknbuf_t addr;
        addr.len = static_cast< int32_t >(sizeof(addr.storage));

        ara::core::String address{config_->remoteAddress + ":"
                                  + std::to_string(static_cast< int32_t >(config_->tcpPort))};
        int32_t len = nai_sockaddr_pton(address.data(), -1, &addr.addr, &addr.len);
        if (len == -1) {
            ComLogError("error:", GetErrorMsg());
            errCode = ara::com::raw::RawErrc::kAddressNotAvailable;
            break;
        }

        // register mapping and accept callback
        {
            std::lock_guard< std::mutex > g(g_Stream2RawMutex);
            g_Stream2Raw[&fd_] = this;
        }
        std::ignore = nai_stream_set_cb(&fd_, [](nai_stream_t* p, int32_t events) {
            ComLogInfo("tcp get events is ", events);
            RawEthernetTcp* client{nullptr};
            {
                std::lock_guard< std::mutex > g(g_Stream2RawMutex);
                auto it = g_Stream2Raw.find(p);
                if (it != g_Stream2Raw.end()) {
                    client = it->second;
                }
            }
            if (client == nullptr) {
                ComLogError("tcp get events but no client mapping", events);
                return static_cast< int32_t >(-1);
            }
            return client->_connectCb(events);
        });

        int32_t r{nai_stream_connect(&fd_, loop_->GetRawEvLoop(), &addr.addr, addr.len)};
        if (r < 0) {
            std::ignore = nai_stream_close(&fd_);
            ComLogError("client connect callback error:", GetErrorMsg());
            errCode = ara::com::raw::RawErrc::kConnectionRefused;
            break;
        }

        std::unique_lock< std::mutex > lock(rawMux_);
        if (timeout == -1) {
            cv_.wait(lock);
        } else {
            // Calculate wait_for end wait time. If timeout, record errCode. Otherwise, consider it was normally interrupted during wait.
            auto start  = std::chrono::system_clock::now();
            std::ignore = cv_.wait_for(lock, std::chrono::milliseconds(timeout));
            auto end    = std::chrono::system_clock::now();
            std::chrono::milliseconds duration{std::chrono::duration_cast< std::chrono::milliseconds >(end - start)};
            if (duration.count() >= timeout) {
                errCode = ara::com::raw::RawErrc::kCommunicationTimeout;
                break;
            }
        }
        return ara::core::Result< void >::FromValue();
    } while (false);
    return ara::core::Result< void >::FromError(errCode);
}

/// @brief Raw byte stream TCP protocol client connection callback function
/// @param[in] events
/// @return success: 0 falied: -1
/// @exception
int32_t ara::com::internal::raw::RawEthernetTcp::_connectCb(int32_t events) noexcept
{
    ComLogInfo("raw client! connectCb: events", events);
    int32_t ret{};
    if (NAI_EV_ERROR == (events & NAI_EV_ERROR)) {
        ComLogInfo("connectCb event error", GenArg(nai_errno), GetErrorMsg());
        ret = -1;
    };
    if (NAI_EV_READ == (events & NAI_EV_READ)) {
        std::unique_lock< std::mutex > lock(rawMux_);
        // Every time there is a read event, this flag is set to true.
        rdFlag_ = true;
        cv_.notify_all();
        ComLogInfo("connectCb read event get!");
    }
    if (NAI_EV_WRITE == (events & NAI_EV_WRITE)) {
        ComLogInfo("connectCb write event get!");
    }
    if (NAI_EV_ERROR == (events & NAI_EV_ERROR)) {
        ComLogInfo("connectCb client close!!");
    }

    if (NAI_EV_TIMEOUT == (events & NAI_EV_TIMEOUT)) {
        switch (events & (NAI_EV_READ | NAI_EV_WRITE)) {
            case NAI_EV_READ:
                ComLogInfo("connectCb stream read timeout!");
                break;
            case NAI_EV_WRITE:
                ComLogInfo("connectCb stream write timeout!");
                break;
            default:
                break;
        };
    };
    if (NAI_EV_NOTIFY == (events & NAI_EV_NOTIFY)) {
        std::int32_t notifyId = nai_ev_notify_code(events);
        switch (notifyId) {
            // user notify read .
            case 0:
                ComLogInfo("client.user define read!");
                if (readNotify_) {
                    readNotify_();
                    readNotify_ = []() {};
                }
                break;
            // user nofity write.
            case 1:
                ComLogInfo("client.user define write!");
                if (writeNotify_) {
                    writeNotify_();
                    writeNotify_ = []() {};
                }
                break;
            // user notify close.
            case 2:
                if (closeNotify_) {
                    closeNotify_();
                    closeNotify_ = []() {};
                }
                break;
            default:
                break;
        }
    }
    if (NAI_EV_SIGNAL == (events & NAI_EV_SIGNAL)) {
        // case 0:
    }
    cv_.notify_all();
    _SetConnectionState(ConnectionState::kRawConnectionStateConnect);
    return ret;
}

}  // namespace raw
}  // namespace internal
}  // namespace com
}  // namespace ara