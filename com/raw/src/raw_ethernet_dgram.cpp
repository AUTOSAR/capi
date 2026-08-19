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
/// @file       raw_ethernet_dgram.cpp
/// @brief
/// @details
/// @date       2021-12-02
/// @author     cuijiusen
/// @version    1.2.0
///
/// ================================================================

#include <nai/io/nai_ssl.h>
#include <net/if.h>

#include <cstddef>
#include <iostream>
#include <mutex>
#include <unordered_map>

#include "ara/com/internal/raw/raw_stream_ethernet.h"
#include "ara/com/raw/raw_data_stream.h"
#include "ara/com/raw/raw_error_domain.h"
#include "ara/core/promise.h"
#ifdef HAS_ARA_IAM
    #include "ara/iam/internal/grantquery/raw.h"
#else
#endif
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
static std::mutex g_Dgram2RawMutex;
static std::unordered_map< nai_dgram_t*, RawEthernetUdp* > g_Dgram2Raw;  // NOLINT

/// @brief Raw byte stream UDP protocol constructor
/// @param[in] instance Instance specifier
/// @exception
RawEthernetUdp::RawEthernetUdp(ara::core::InstanceSpecifier const& instance) noexcept
{
    config_ = RawRuntime::GetRawConfig(instance);
#ifdef HAS_ARA_IAM
    // iam load
    ara::core::Result< void > resRaw = ara::iam::internal::grant::IAMGrantRawQuery::Initialize();
    if (!resRaw) {
        ComLogWarning("RawEthernetUdp get iam initialize failed!");
    }
#else
#endif
}

/// @brief Raw byte stream UDP protocol destructor
/// @exception
RawEthernetUdp::~RawEthernetUdp() noexcept
{
    //[SWS_CM_10483] Destructor of the RawDataStreamClient that deletes the RawDataStreamClient instance.
    // If the connection is still open, the connection should be closed and shut down before destroying
    // the RawDataStreamClient object.
    {
        std::lock_guard< std::mutex > g(g_Dgram2RawMutex);
        auto it = g_Dgram2Raw.find(&fd_);
        if (it != g_Dgram2Raw.end()) {
            g_Dgram2Raw.erase(it);
        }
    }
    std::ignore = nai_dgram_close(&fd_);
#ifdef HAS_ARA_IAM
    ara::iam::internal::grant::IAMGrantRawQuery::Deinitialize();
#else
#endif
    _SetConnectionState(ConnectionState::kRawConnectionStateNotAvailable);
}

/// @brief Raw byte stream UDP protocol server create connection function
/// @param[in] timeout
/// @return
/// @exception
ara::core::Result< void > RawEthernetUdp::WaitForConnection(int64_t timeout) noexcept
{
    ComLogInfo("RawEthernetUdp::WaitForConnection: in !");
    ara::com::raw::RawErrc errCode{ara::com::raw::RawErrc::kUnknownError};
    loop_ = isoft::naicpp::GlobalGeneralEvLoop::Get();
    do {
        if (nullptr == config_.get()) {
            ComLogError("config_.get() is null!");
            errCode = ara::com::raw::RawErrc::kAddressNotAvailable;
            break;
        }
        if (_GetConnectionState() == ConnectionState::kRawConnectionStateConnect) {
            // Existing connection
            ComLogError("_GetConnectionState is already connected!");
            errCode = ara::com::raw::RawErrc::kStreamAlreadyConnected;
            break;
        }

        std::ignore = nai_dgram_init(&fd_);

        ara::core::String bindAddress{config_->localAddress};

        // Ver 2.1 2011 Currently if multicast port is configured, the default configured IP is the multicast IP. To be fixed later when 2311 model adds local address.
        if (config_->multicastUdp > static_cast< uint16_t >(0)) {
            ara::core::String const addressPeer{config_->localAddress + ":"
                                                + std::to_string(static_cast< int32_t >(config_->multicastUdp))};
            mulAddrLen_ = static_cast< int32_t >(sizeof(mulAddr_));
            // multicast pton.
            int32_t ret{nai_sockaddr_pton(addressPeer.data(), addressPeer.length(), &mulAddr_, &mulAddrLen_)};
            if (ret < 0) {
                ComLogError("server: multicast nai_sockaddr_pton:", GetErrorMsg());
                errCode = ara::com::raw::RawErrc::kAddressNotAvailable;
                break;
            }

            if (mulAddr_.sa_family == static_cast< uint16_t >(AF_INET)) {
                bindAddress = "0.0.0.0";
            } else if (mulAddr_.sa_family == static_cast< uint16_t >(AF_INET6)) {
                bindAddress = "0:0:0:0:0:0:0:0";
            }
            // local host pton.
            ara::core::String const addrLocal{bindAddress + ":"
                                              + std::to_string(static_cast< int32_t >(config_->multicastUdp))};
            addrLen_ = static_cast< int32_t >(sizeof(peerAddr_));
            ret      = nai_sockaddr_pton(addrLocal.data(), addrLocal.length(), &peerAddr_, &addrLen_);
            if (ret < 0) {
                ComLogError("nai_sockaddr_pton error:", GetErrorMsg());
                errCode = ara::com::raw::RawErrc::kAddressNotAvailable;
                break;
            }

            // accept process in this.
            // register mapping and accept callback
            {
                std::lock_guard< std::mutex > g(g_Dgram2RawMutex);
                g_Dgram2Raw[&fd_] = this;
            }
            std::ignore = nai_dgram_set_cb(&fd_, [](nai_dgram_t* p, nai_int_t events) {
                ComLogInfo("udp get events is ", events);
                RawEthernetUdp* server{nullptr};
                {
                    std::lock_guard< std::mutex > g(g_Dgram2RawMutex);
                    auto it = g_Dgram2Raw.find(p);
                    if (it != g_Dgram2Raw.end()) {
                        server = it->second;
                    }
                }
                if (server == nullptr) {
                    ComLogError("udp get events but no server mapping", events);
                    return static_cast< int32_t >(-1);
                }
                return server->_serverReadWriteCb(events);
            });

            if (config_->multicastUdp > static_cast< uint16_t >(0)) {
                ret = nai_dgram_set_opt(&fd_, NAI_IO_MULTICAST_IF, reinterpret_cast< intptr_t >(&peerAddr_));
                if (ret < 0) {
                    ComLogError("Udp WaitForConnection set NAI_IO_MULTICAST_IF failed!");
                    errCode = ara::com::raw::RawErrc::kInterruptedBySignal;
                    break;
                };
            }

            ret = nai_dgram_bind(&fd_, loop_->GetRawEvLoop(), &peerAddr_, addrLen_);
            if (ret < 0) {
                errCode = ara::com::raw::RawErrc::kConnectionAborted;
                ComLogError("Udp WaitForConnection nai_dgram_bind failed!");
                break;
            };
        } else {
            addrLen_ = static_cast< int32_t >(sizeof(peerAddr_));
            ara::core::String const address{bindAddress + ":"
                                            + std::to_string(static_cast< int32_t >(config_->udpPort))};
            int32_t len{nai_sockaddr_pton(address.data(), address.length(), &peerAddr_, &addrLen_)};
            if (len == -1) {
                ComLogError("nai_sockaddr_pton error:", GetErrorMsg());
                errCode = ara::com::raw::RawErrc::kAddressNotAvailable;
                break;
            }

            // Set timeout
            int32_t r1{nai_dgram_set_opt(&fd_, NAI_IO_RECVTIMEO, timeout)};
            if (r1 < 0) {
                ComLogError("bdu:WaitForConnection set timeout error:", nai_errno);
                errCode = ara::com::raw::RawErrc::kStreamNotConnected;
                break;
            }

            // register mapping and accept callback
            {
                std::lock_guard< std::mutex > g(g_Dgram2RawMutex);
                g_Dgram2Raw[&fd_] = this;
            }
            std::ignore = nai_dgram_set_cb(&fd_, [](nai_dgram_t* p, nai_int_t events) {
                ComLogInfo("udp get events is ", events);
                RawEthernetUdp* server{nullptr};
                {
                    std::lock_guard< std::mutex > g(g_Dgram2RawMutex);
                    auto it = g_Dgram2Raw.find(p);
                    if (it != g_Dgram2Raw.end()) {
                        server = it->second;
                    }
                }
                if (server == nullptr) {
                    ComLogError("udp get events but no server mapping", events);
                    return static_cast< int32_t >(-1);
                }
                return server->_serverReadWriteCb(events);
            });

            int32_t r{nai_dgram_bind(&fd_, loop_->GetRawEvLoop(), &peerAddr_, addrLen_)};
            if (r < 0) {
                errCode = ara::com::raw::RawErrc::kConnectionAborted;
                ComLogError("Udp WaitForConnection nai_dgram_bind failed!");
                break;
            };
        }

        _SetConnectionState(ConnectionState::kRawConnectionStateConnect);
        return ara::core::Result< void >::FromValue();
    } while (false);
    return ara::core::Result< void >::FromError(errCode);
}

/// @brief Raw byte stream UDP protocol server read/write callback function
/// @param[in] events
/// @return
int32_t RawEthernetUdp::_serverReadWriteCb(int32_t events) noexcept
{
    ComLogInfo("udp _serverReadWriteCb event:", events);
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
        std::int32_t notifyId{nai_ev_notify_code(events)};
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
    return ret;
}
/// @brief Raw byte stream UDP protocol write data function
/// @param[in] data
/// @param[in] maxLength
/// @param[in] timeout
/// @return
/// @exception
ara::core::Result< size_t > RawEthernetUdp::WriteData(ara::com::SamplePtr< uint8_t > data,
                                                      size_t maxLength,
                                                      int64_t timeout) noexcept
{
    ComLogInfo("RawEthernetUdp::WriteData In!");
    ara::core::Promise< size_t > promise1{};
    auto future{promise1.get_future()};
    do {
        if (_GetConnectionState() != ConnectionState::kRawConnectionStateConnect) {
            promise1.SetError(ara::com::raw::RawErrc::kStreamNotConnected);
            break;
        }
        // multicast udp distinction.
        bool mulFlag{config_->multicastUdp > static_cast< uint16_t >(0) ? true : false};

        writeNotify_ = [this, &promise1, &data, &maxLength, &mulFlag]() {
            intptr_t r{-1};
            // multicast
            if (mulFlag) {
                r = nai_dgram_sendto(&fd_, data.Get(), maxLength, 0, &mulAddr_, mulAddrLen_);
            } else {
                r = nai_dgram_sendto(&fd_, data.Get(), maxLength, 0, &peerAddr_, addrLen_);
            }

            // intptr_t r{nai_dgram_write(&fd_, data.Get(), maxLength)};
            if (r >= static_cast< intptr_t >(0)) {
                promise1.set_value(r);
            } else {
                if ((nai_errno == NAI_EAGAIN) || (nai_errno == EINPROGRESS)) {
                    ComLogError(GenArg(nai_errno), GetErrorMsg());
                    // Asynchronous write operation, when returning EAGAIN or EINPROGRESS, returns 0. User decides whether to write data again.
                    promise1.set_value(static_cast< intptr_t >(0));
                } else {
                    ComLogError(GenArg(nai_errno), GetErrorMsg());
                    promise1.SetError(ara::com::raw::RawErrc::kInterruptedBySignal);
                }
            }
        };
        // Notify io thread.
        if (nai_dgram_post(&fd_, 1) < 0) {
            promise1.SetError(ara::com::raw::RawErrc::kInterruptedBySignal);
            ComLogError("RawEthernetUdp::WriteData nai_dgram_post failed!");
            break;
        }
        if (timeout == -1) {
            future.wait();
        } else {
            ara::core::future_status status;
            status = future.wait_for(std::chrono::milliseconds(timeout));
            if (status == ara::core::future_status::timeout) {
                promise1.SetError(ara::com::raw::RawErrc::kCommunicationTimeout);
                ComLogError("RawEthernetUdp::WriteData wait future timeout!");
                break;
            }
        }
    } while (false);
    return future.GetResult();
}
/// @brief Raw byte stream UDP protocol read data function
/// @param[in] maxLength
/// @param[in] timeout
/// @return
/// @exception
ara::core::Result< ReadDataResult > RawEthernetUdp::ReadData(size_t maxLength, int64_t timeout) noexcept
{
    ComLogInfo("RawEthernetUdp::ReadData in!");
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

        if (nullptr == config_.get()) {
            // Configuration not configured or loaded incorrectly
            promise1.SetError(MakeErrorCode(ara::com::raw::RawErrc::kAddressNotAvailable, __LINE__));
            ComLogError("ReadData:config.get() is null!");
            break;
        }

        if (timeoutFlag) {
            promise1.SetError(MakeErrorCode(ara::com::raw::RawErrc::kCommunicationTimeout, __LINE__));
            ComLogError("ReadData:timeout!");
            break;
        }
        ComLogInfo("UDP:recv notify");

        // if (maxLength > config_.get()->maximumTransmissionUnit) {
        //     errCode = ara::com::raw::RawErrc::kUnknownError;
        //     break;
        // }

        // std::ignore = nai_dgram_set_opt(&fd_, NAI_IO_RECVTIMEO, timeout);

        // multicast udp distinction.
        bool mulFlag{config_->multicastUdp > 0 ? true : false};

        readNotify_ = [this, &promise1, &data, &numberOfBytes, &maxLength, &mulFlag]() {
            int32_t r1{-1};
            if (mulFlag) {
                r1 = nai_dgram_recvfrom(&fd_, data.Get(), maxLength, 0, &mulAddr_, &mulAddrLen_);
            } else {
                r1 = nai_dgram_recvfrom(&fd_, data.Get(), maxLength, 0, &peerAddr_, &addrLen_);
            }

            if (r1 > 0 && r1 < static_cast< intptr_t >(maxLength)) {
                numberOfBytes = static_cast< size_t >(r1);
                rdFlag_       = false;
                promise1.set_value(ReadDataResult{std::move(data), numberOfBytes});
            } else if (r1 > 0 && r1 >= static_cast< intptr_t >(maxLength)) {
                numberOfBytes = static_cast< size_t >(r1);
                // rdCnt  = rdCnt - 1;
                promise1.set_value(ReadDataResult{std::move(data), numberOfBytes});
            } else if (r1 == 0) {
                ComLogError("peer closed:");
                promise1.SetError(MakeErrorCode(ara::com::raw::RawErrc::kConnectionClosedByPeer, __LINE__));
            } else {
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
        if (nai_dgram_post(&fd_, 0) < 0) {
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

/// @brief Raw byte stream UDP protocol close connection function
/// @param[in] timeout
/// @return
/// @exception
ara::core::Result< void > RawEthernetUdp::Shutdown(int64_t timeout) noexcept
{
    ComLogInfo("RawEthernetUdp::Shutdown in!");
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
            if (nai_dgram_close(&fd_) < 0) {
                ComLogError("Raw udp ShutDown listen and accept fd faile!");
                ComLogError(GenArg(nai_errno), GetErrorMsg());
                promise1.SetError(MakeErrorCode(ara::com::raw::RawErrc::kInterruptedBySignal, __LINE__));
                errFlag = true;
            } else {
                ComLogInfo("close listen && accept fd success.");
                promise1.set_value();
            }
        };
        // Notify io thread.
        if (nai_dgram_post(&fd_, 2) < 0) {
            ComLogError("nai_dgram_post send singal failed!");
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
        std::ignore        = nai_memset(&peerAddr_, 0, sizeof(peerAddr_));
        addrLen_           = 0;
        // clear socket related configuration.
        // nai_memset(&fd_,0,sizeof(fd_));
        // close evloop.
        this->loop_.reset();
        return ara::core::Result< void >::FromValue();
    } while (false);
    return future.GetResult();
}

/// @brief Raw byte stream UDP protocol client connection function
/// @param[in] timeout
/// @return
/// @exception
ara::core::Result< void > RawEthernetUdp::Connect(int64_t timeout) noexcept
{
    ara::com::raw::RawErrc errCode;
    loop_ = isoft::naicpp::GlobalGeneralEvLoop::Get();
    ComLogTrace("RawEthernetUdp:Connect.");
    int32_t r{-1};
    do {
        if (nullptr == config_.get()) {
            // Configuration not configured or loaded incorrectly
            ComLogError("raw udp client:config_.get() is null!");
            errCode = ara::com::raw::RawErrc::kAddressNotAvailable;
            break;
        }
        if (loop_ == nullptr) {
            ComLogError("raw udp client:loop_ is null!");
            errCode = ara::com::raw::RawErrc::kAddressNotAvailable;
            break;
        }

        if (_GetConnectionState() == ConnectionState::kRawConnectionStateConnect) {
            ComLogError("raw udp client:_GetConnectionState already connected.");
            errCode = ara::com::raw::RawErrc::kStreamAlreadyConnected;
            break;
        }

        r = nai_dgram_init(&fd_);
        if (r < 0) {
            errCode = ara::com::raw::RawErrc::kConnectionAborted;
            ComLogError("raw udp nai_dgram_init failed!");
            break;
        }

        std::ignore = nai_dgram_set_opt(&fd_, NAI_IO_SENDTIMEO, timeout);

        addrLen_ = sizeof(peerAddr_);

        // V2.1 2011 version: Because the model does not support configuring local IP. That is, if multicast port is configured, IP is configured as multicast address. Local address temporarily written as 0.0.0.0
        // To be modified when 2311 is implemented.
        if (config_->multicastUdp > static_cast< uint16_t >(0)) {
#ifdef HAS_ARA_IAM
            // multicastUdp iam check start---->
            // get pid.
            int32_t nPid = nai_get_pid();
            if (!ara::iam::internal::grant::IAMGrantRawQuery::HasRawGrant(
                    nPid, config_->localAddress, 0, 0, static_cast< uint32_t >(config_->multicastUdp))) {
                ComLogError("raw multicastUdp iam check failed!");
                return ara::core::Result< void >::FromError(ara::com::ComErrc::kGrantEnforcementError);
            }
            ComLogInfo("raw multicastUdp iam check success!");
            //--->iam check end
#else
#endif
            // local
            nai_socknbuf_t localAddr;
            localAddr.len = static_cast< int32_t >(sizeof(localAddr.storage));
            ara::core::String const address{"0.0.0.0"};
            r = nai_sockaddr_pton(address.data(), address.length(), &localAddr.addr, &localAddr.len);
            if (r == -1) {
                ComLogError("raw udp client: nai_sockaddr_pton error:", GetErrorMsg());
                errCode = ara::com::raw::RawErrc::kAddressNotAvailable;
                break;
            }

            // multicast.
            nai_socknbuf_t bindAddr;
            bindAddr.len = static_cast< int32_t >(sizeof(bindAddr));
            ara::core::String const addressPeer{config_->multicastAddress + ":"
                                                + std::to_string(static_cast< int32_t >(config_->multicastUdp))};
            // multicast pton.
            int32_t ret{nai_sockaddr_pton(addressPeer.data(), addressPeer.length(), &bindAddr.addr, &bindAddr.len)};
            if (ret < 0) {
                ComLogError("client: multicast nai_sockaddr_pton:", GetErrorMsg());
                errCode = ara::com::raw::RawErrc::kAddressNotAvailable;
                break;
            }

            // register mapping and accept callback
            {
                std::lock_guard< std::mutex > g(g_Dgram2RawMutex);
                g_Dgram2Raw[&fd_] = this;
            }
            std::ignore = nai_dgram_set_cb(&fd_, [](nai_dgram_t* p, nai_int_t events) {
                ComLogInfo("udp get events is ", events);
                RawEthernetUdp* client{nullptr};
                {
                    std::lock_guard< std::mutex > g(g_Dgram2RawMutex);
                    auto it = g_Dgram2Raw.find(p);
                    if (it != g_Dgram2Raw.end()) {
                        client = it->second;
                    }
                }
                if (client == nullptr) {
                    ComLogError("udp get events but no client mapping", events);
                    return static_cast< int32_t >(-1);
                }
                return client->_connectCb(events);
            });

            int32_t r1{nai_dgram_bind(&fd_, loop_->GetRawEvLoop(), &bindAddr.addr, bindAddr.len)};
            if (r1 < 0) {
                errCode = ara::com::raw::RawErrc::kInterruptedBySignal;
                break;
            }

            // Disable local loopback.
            r = nai_dgram_set_opt(&fd_, NAI_IO_MULTICAST_LOOP, 0);
            if (ret < 0) {
                ComLogError("client: NAI_IO_MULTICAST_LOOP is forbidden.", GetErrorMsg());
                errCode = ara::com::raw::RawErrc::kAddressNotAvailable;
                break;
            }

            // Multicast address.
            nai_sockaddr_t* sa[2];
            mulAddrLen_ = static_cast< int32_t >(sizeof(mulAddr_));
            ara::core::String addrPeer{config_->localAddress};

            r = nai_sockaddr_pton(addrPeer.data(), addrPeer.length(), &mulAddr_, &mulAddrLen_);
            if (r == -1) {
                ComLogError("raw udp client: multicastUdp nai_sockaddr_pton error:", GetErrorMsg());
                errCode = ara::com::raw::RawErrc::kConnectionAborted;
                break;
            }
            sa[0] = &mulAddr_;
            sa[1] = &localAddr.addr;
            r     = nai_dgram_set_opt(&fd_, NAI_IO_MULTICAST_JOIN, reinterpret_cast< intptr_t >(sa));
            if (r < 0) {
                errCode = ara::com::raw::RawErrc::kConnectionAborted;
                ComLogInfo(GenArg(nai_errno), GetErrorMsg());
                ComLogError("raw udp client: multicastUdp set opt error:");
                break;
            };

            // Client side multicast port reuse.
            r = nai_dgram_set_opt(&fd_, NAI_IO_REUSEADDR, 1);
            if (r < 0) {
                ComLogError("Udp WaitForConnection set NAI_IO_REUSEADDR failed!");
                errCode = ara::com::raw::RawErrc::kInterruptedBySignal;
                break;
            };

        } else {
#ifdef HAS_ARA_IAM
            // udp iam check start---->
            // get pid.
            int32_t nPid = nai_get_pid();
            if (!ara::iam::internal::grant::IAMGrantRawQuery::HasRawGrant(
                    nPid, config_->remoteAddress, 0, static_cast< uint32_t >(config_->udpPort), 0)) {
                ComLogError("raw udp iam check failed!");
                return ara::core::Result< void >::FromError(ara::com::ComErrc::kGrantEnforcementError);
            }
            ComLogInfo("raw udp iam check success!");
            //--->iam check end
#else
#endif
            ara::core::String const address{config_->localAddress + ":"
                                            + std::to_string(static_cast< int32_t >(config_->udpPort))};
            r = nai_sockaddr_pton(address.data(), address.length(), &peerAddr_, &addrLen_);
            if (r == -1) {
                ComLogError("udp connect nai_sockaddr_pton error:", GetErrorMsg());
                errCode = ara::com::raw::RawErrc::kInterruptedBySignal;
                break;
            }

            // register mapping and accept callback
            {
                std::lock_guard< std::mutex > g(g_Dgram2RawMutex);
                g_Dgram2Raw[&fd_] = this;
            }
            std::ignore = nai_dgram_set_cb(&fd_, [](nai_dgram_t* p, nai_int_t events) {
                ComLogInfo("udp get events is ", events);
                RawEthernetUdp* client{nullptr};
                {
                    std::lock_guard< std::mutex > g(g_Dgram2RawMutex);
                    auto it = g_Dgram2Raw.find(p);
                    if (it != g_Dgram2Raw.end()) {
                        client = it->second;
                    }
                }
                if (client == nullptr) {
                    ComLogError("udp get events but no client mapping", events);
                    return static_cast< int32_t >(-1);
                }
                return client->_connectCb(events);
            });
            r           = nai_dgram_connect(&fd_, loop_->GetRawEvLoop(), &peerAddr_, addrLen_);
            if (r < 0) {
                ComLogError("udp nai_dgram_connect failed! ", GetErrorMsg());
                errCode = ara::com::raw::RawErrc::kConnectionRefused;
                break;
            };
        }
        _SetConnectionState(ConnectionState::kRawConnectionStateConnect);
        return ara::core::Result< void >::FromValue();
    } while (false);
    return ara::core::Result< void >::FromError(errCode);
}

/// @brief Raw byte stream UDP protocol connection callback function
/// @param[in] events
/// @return
int32_t RawEthernetUdp::_connectCb(int32_t events) noexcept
{
    ComLogInfo("raw udp RawEthernetUdp::_connectCb get events:", events);
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
                ComLogInfo("connectCb dgrm read timeout!");
                break;
            case NAI_EV_WRITE:
                ComLogInfo("connectCb dgrm write timeout!");
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
    return ret;
}

}  // namespace raw
}  // namespace internal
}  // namespace com
}  // namespace ara