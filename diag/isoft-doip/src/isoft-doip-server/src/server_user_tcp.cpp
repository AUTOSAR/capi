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
/// @file       server_user_tcp.cpp
/// @brief      This file provides DOIP server TCP related definitions
/// @details
/// @date       2023-10-08
/// @author     zhanglipeng
/// @version    1.2.0
///
/// ================================================================

#include "server_user_tcp.h"

#include "isoft/doip/public_message.h"
#include "nai/runtime/nai_errno.h"
#include "public_log.h"
#include "server_impl.h"
#include "server_listen.h"
#include "server_user_manager.h"
#include "thread/therad_safe.h"

namespace isoft {
namespace doip {

/// @brief Constructor
/// @param[in] stream tcp stream
/// @param[in] server DOIP service object
/// @param[in] bufferSize Read buffer size
ServerUserTcp::ServerUserTcp(std::unique_ptr< PublicTcpStream > stream,
                             ServerImpl* server,
                             size_t bufferSize,
                             ServerListen* listen)
    : ServerUserBase(bufferSize, DoIPMessageTransportType::kDoIPMessageTransportType_TCP, server)
    , sa_{0U}
    , stream_{std::move(stream)}
    , connectionState_{DoipConnectionState::kListen}
    , initialInactivityTimer_{}
    , generalInactivityTimer_{}
    , mapAliveCheckInfo_{}
    , localIp_{}
    , localPort_{0U}
    , peerIp_{}
    , peerPort_{0U}
    , aliveCheckNum_{0U}
    , streamLock_{}
    , isHandshakeFinished_{false}
    , listen_{listen}
{
    LogDebug() << "ServerUserTcp::ServerUserTcp";
}

/// @brief Destructor
ServerUserTcp::~ServerUserTcp() noexcept
{
    {
        std::unique_lock< std::recursive_mutex > const lock{streamLock_};
        if (stream_.get() != nullptr && server_ != nullptr) {
#ifdef NAI_ENABLE_SSL
            ServerConfig const& config = server_->GetConfig();
            if (config.isUseTls) {
                // std::ignore = stream_->ShutdownTLS();

                SSL_shutdown(ssl_);
                SSL_free(ssl_);
                ssl_ = nullptr;
            }
#endif
            stream_.reset();
        }
    }

    connectionState_ = DoipConnectionState::kListen;
    if (initialInactivityTimer_.get() != nullptr) {
        initialInactivityTimer_.reset();
    }
    if (generalInactivityTimer_.get() != nullptr) {
        generalInactivityTimer_.reset();
    }

    _releaseAliveCheckTimer(false);
    LogDebug() << "ServerUserTcp::~ServerUserTcp";
}

/// @brief Initialize
/// @return Initialization result
int32_t ServerUserTcp::Initialize()
{
    if (listen_ == nullptr) {
        LogWarn() << "ServerUserTcp::Initialize|listen is nullptr";
        return -1;
    }

    if (stream_.get() == nullptr) {
        LogWarn() << "ServerUserTcp::Initialize|listen is nullptr";
        return -1;
    }

    if (server_ == nullptr) {
        LogWarn() << "ServerUserTcp::Initialize|server_ is nullptr !";
        return -1;
    }

    /// Create new message callback
    PublicTcpStream::TcpNewMsgCallback newMsgCb
        = [this](PublicTcpStream const* const stream, int32_t const events) -> int32_t {
        if (stream_.get() != stream) {
            LogError() << "ServerUserTcp::Initialize|recv data but stream_ != stream !";
            return -1;
        }

        LogDebug() << "ServerUserTcp::Initialize|sock event fd =" << GetFd() << "events =" << events;

        /// Handle socket events
        return _handleEvent(events);
    };

    /// Set callback
    int32_t const res = stream_->SetNewMsgCallback(newMsgCb);
    if (res == -1) {
        LogError() << "ServerUserTcp::Initialize|SetNewMsgCallback fail, errorno ="
                   << isoft::threadsafe::Safe_Strerror(errno);
        return -1;
    }

    /// Get local IP and port information
    stream_->GetLocalIpAndPort(localIp_, localPort_);

    /// Get peer IP and port information
    stream_->GetPeerIpAndPort(peerIp_, peerPort_);

    /// Get configuration
    ServerConfig const& config = server_->GetConfig();

    if (config.isUseTls) {
#ifdef NAI_ENABLE_SSL
        ssl_ = SSL_new(listen_->GetCtx());
        if (ssl_ == nullptr) {
            LogWarn() << "ServerUserTcp::Initialize|create ssl fail";
            ERR_print_errors_fp(stderr);
            return -1;
        }

        int32_t const fd = stream_->GetFd();
        if (SSL_set_fd(ssl_, fd) <= 0) {
            LogWarn() << "ServerUserTcp::Initialize|SSL_set_fd fail";
            ERR_print_errors_fp(stderr);
            return -1;
        }
#endif
    }

    /// Set connection initial state
    ChangeConnectionState(DoipConnectionState::kInitialized);
    return res;
}

/// @brief Get 4-tuple information
/// @param[out] localIp Local IP
/// @param[out] localPort Local port
/// @param[out] peerIp Peer IP
/// @param[out] peerPort Peer port
void ServerUserTcp::GetSocketAddressInfo(ara::core::String& localIp,
                                         uint16_t& localPort,
                                         ara::core::String& peerIp,
                                         uint16_t& peerPort)
{
    std::ignore = localIp;
    std::ignore = localPort;
    std::ignore = peerIp;
    std::ignore = peerPort;

    localIp   = localIp_;
    localPort = localPort_;
    peerIp    = peerIp_;
    peerPort  = peerPort_;
}

/// @brief Request alive check
/// @param[in] cb Check result callback
void ServerUserTcp::DoAliveCheckRequest(std::function< void(bool) > const& cb, int32_t const targetFd)
{
    LogInfo() << "ServerImpl::DoAliveCheckRequest|to fd =" << GetFd() << "do alive check,"
              << "targetFd =" << targetFd;
    AliveCheckRequest req;
    ara::core::Vector< uint8_t > vBody;
    req.Serialize(vBody);
    std::ignore = SendToClient(PayloadType::kAliveCheckRequest, vBody);

    std::shared_ptr< naicpp::EvLoop > evloop = server_->GetEvLoop();
    if (evloop.get() == nullptr) {
        LogError() << "ServerUserTcp::DoAliveCheckRequest|evloop is nullptr fd =" << GetFd();
        return;
    }

    std::shared_ptr< naicpp::EvNodeTimer > aliveCheckTimer{nullptr};
    int32_t const timout{static_cast< int32_t >(server_->GetConfig().aliveCheckTimer)};
    int32_t const res = evloop->MakeTimer(aliveCheckTimer, timout, [this, targetFd]() {
        ChangeConnectionState(DoipConnectionState::kFinalize);
        _releaseAliveCheckTimer(false, targetFd);
    });

    if (res >= 0) {
        AliveCheckInfo info;
        info.aliveCheckTimer = aliveCheckTimer;
        info.aliveCheckCb    = cb;
        mapAliveCheckInfo_.emplace(targetFd, info);
        LogInfo() << "ServerImpl::DoAliveCheckRequest|make timer success fd =" << GetFd() << "targetFd =" << targetFd;
    } else {
        LogError() << "ServerImpl::DoAliveCheckRequest|make timer error res =" << res << "fd =" << GetFd()
                   << "targetFd =" << targetFd;
    }
}

/// @brief Reply alive check
/// @param[in] sa Source logical address
void ServerUserTcp::DoAliveCheckResponse(uint16_t const sa)
{
    bool const isAlive{sa == sa_};
    if (!isAlive) {
        ChangeConnectionState(DoipConnectionState::kFinalize);
    }

    _releaseAliveCheckTimer(isAlive);
}

/// @brief Update general inactivity timer
void ServerUserTcp::UpdateGeneralInactivityTimer()
{
    if (generalInactivityTimer_.get() == nullptr) {
        return;
    }

    ServerConfig const& config     = server_->GetConfig();
    int32_t const generalTimeoutMs = static_cast< int32_t >(config.generalInactivityTimer * kSecondToMsTransform);
    std::ignore                    = generalInactivityTimer_->UpdateTime(generalTimeoutMs);
}

/// @brief Change socket state
/// @param[in] state State
void ServerUserTcp::ChangeConnectionState(DoipConnectionState const state)
{
    std::unique_lock< std::recursive_mutex > const lock{streamLock_};
    if (stream_.get() == nullptr) {
        LogError() << "ServerUserTcp::ChangeConnectionState|stream_ is nullptr !";
        return;
    }

    if (server_ == nullptr) {
        LogError() << "ServerUserTcp::ChangeConnectionState|server_ is nullptr !";
        return;
    }

    std::shared_ptr< naicpp::EvLoop > evloop = server_->GetEvLoop();
    if (evloop.get() == nullptr) {
        LogError() << "ServerUserTcp::ChangeConnectionState|evloop is nullptr !";
        return;
    }

    /// Set state
    connectionState_ = state;
    /// Get configuration
    ServerConfig const& config = server_->GetConfig();

    switch (connectionState_) {
        case DoipConnectionState::kInitialized: {
            int32_t const initialTimeoutMs
                = static_cast< int32_t >(config.initialInactivityTimer * kSecondToMsTransform);
            std::ignore = evloop->MakeTimer(initialInactivityTimer_, initialTimeoutMs, [this]() {
                LogInfo() << "ServerUserTcp::ChangeConnectionState|kInitialized fd =" << GetFd();
                ChangeConnectionState(DoipConnectionState::kFinalize);
            });
        } break;
        case DoipConnectionState::kConfirmation: {
            initialInactivityTimer_.reset();
            int32_t const generalTimeoutMs
                = static_cast< int32_t >(config.generalInactivityTimer * kSecondToMsTransform);
            std::ignore = evloop->MakeTimer(generalInactivityTimer_, generalTimeoutMs, [this]() {
                LogInfo() << "ServerUserTcp::ChangeConnectionState|kConfirmation fd =" << GetFd();
                ChangeConnectionState(DoipConnectionState::kFinalize);
            });
        } break;
        case DoipConnectionState::kActive: {
            initialInactivityTimer_.reset();
            int32_t const generalTimeoutMs
                = static_cast< int32_t >(config.generalInactivityTimer * kSecondToMsTransform);
            std::ignore = evloop->MakeTimer(generalInactivityTimer_, generalTimeoutMs, [this]() {
                LogInfo() << "ServerUserTcp::ChangeConnectionState|kActive fd =" << GetFd();
                ChangeConnectionState(DoipConnectionState::kFinalize);
            });
        } break;
        case DoipConnectionState::kFinalize: {
            if (stream_.get() == nullptr) {
                LogError() << "ServerUserTcp::ChangeConnectionState|finalizeHandle stream_ is nullptr !";
                return;
            }
            /// Notify upper layer that diagnostic connection is disconnected
            server_->ProcessDiagConnection(this, false);

            /// Remove self from manager
            int32_t fd  = stream_->GetFd();
            std::ignore = evloop->Exec([this, fd]() { ServerUserManager::GetInstance().RemoveTcpUser(fd); });
        } break;
        default: {
            LogError() << "ServerUserTcp::InitConnectionStateHandle|unknown state !";
        } break;
    }
}

/// @brief Send message to peer
/// @param[in] type Protocol number
/// @param[in] body Message body
/// @return Send result
int32_t ServerUserTcp::SendToClient(PayloadType const type, ara::core::Vector< uint8_t > const& body)
{
    if (server_ == nullptr) {
        LogError() << "ServerUserTcp::SendToClient|server_ is nullptr !";
        return -1;
    }

    if (connectionState_ == DoipConnectionState::kFinalize) {
        LogWarn() << "ServerUserTcp::SendToClient|connectionState_ is close curState ="
                  << ara::log::LogHex8{static_cast< uint8_t >(connectionState_)};
        return -1;
    }

    std::unique_lock< std::recursive_mutex > const lock{streamLock_};
    if (stream_.get() == nullptr) {
        LogError() << "ServerUserTcp::SendToClient|stream_ is nullptr";
        return -1;
    }

    /// Construct packet header
    ara::core::Vector< uint8_t > msg;
    _StructurePackageHeader(type, static_cast< uint32_t >(body.size()), msg);

    /// Assemble complete packet
    std::ignore = msg.insert(msg.cend(), body.begin(), body.end());

    /// Update general activity timer
    UpdateGeneralInactivityTimer();

    /// Get configuration
    ServerConfig const& config = server_->GetConfig();

    int32_t res = -1;
    if (config.isUseTls) {
#ifdef NAI_ENABLE_SSL
        res = SSL_write(ssl_, msg.data(), msg.size());
#endif
    } else {
        res = static_cast< int32_t >(stream_->Write(msg.data(), msg.size(), config.isUseTls));
    }

    // int32_t const res{static_cast< int32_t >(stream_->Write(msg.data(), msg.size(), config.isUseTls))};
    // if (res == -1) {
    //     LogError() << "ServerUserTcp::SendToClient|fd =" << GetFd() << "errornum = " << errno
    //                << " err =" << isoft::threadsafe::Safe_Strerror(errno);
    // }

    LogDebug() << "ServerUserTcp::SendToClient|fd =" << GetFd()
               << "type =" << ara::log::LogHex16{static_cast< uint16_t >(type)} << "bodySize =" << body.size()
               << "totalSize =" << msg.size() << "sendSize =" << res << "tls =" << config.isUseTls;
    return res;
}

/// @brief Handle socket events
/// @param[in] events Socket events
/// @return Handling result
int32_t ServerUserTcp::_handleEvent(int32_t const events)
{
    if (server_ == nullptr) {
        LogError() << "ServerUserTcp::_handleEvent|server_ is nullptr !";
        return -1;
    }

    if (stream_.get() == nullptr) {
        LogError() << "ServerUserTcp::_handleEvent|stream_ is nullptr !";
        return -1;
    }

    if ((events & NAI_EV_ERROR) == NAI_EV_ERROR) {
        ChangeConnectionState(DoipConnectionState::kFinalize);
        LogWarn() << "ServerUserTcp::_handleEvent|recv data NAI_EV_ERROR fd =" << GetFd()
                  << "error =" << isoft::threadsafe::Safe_Strerror(errno);
        return -2;
    }

    /// Get configuration
    ServerConfig const& config = server_->GetConfig();

    int32_t ec{0};
#ifdef NAI_ENABLE_SSL
    if (config.isUseTls) {
        if (!isHandshakeFinished_) {
            // int32_t r = stream_->SSLHandshake();
            // if (r == -1) {
            //     ec = nai_errno;
            //     if (ec == NAI_EAGAIN || ec == NAI_EINPROGRESS) {
            //         LogDebug() << "ServerUserTcp::_handleEvent|SSLHandshake please try later";
            //         return 0;
            //     }

            //     LogError() << "ServerUserTcp::_handleEvent|SSLHandshake fail, error =" << strerror(errno);
            //     return -1;
            // }

            // r = stream_->SSLVerify(NAI_SSL_VERIFY_NO_CA);
            // if (r == -1) {
            //     LogError() << "ServerUserTcp::_handleEvent|SSLVerify fail, error =" << strerror(errno);
            //     return -1;
            // }

            // LogInfo() << "ServerUserTcp::_handleEvent|SSLHandshake success fd =" << GetFd();
            // isHandshakeFinished_ = true;

            do {
                int32_t const accept_res = SSL_accept(ssl_);
                int32_t const err        = SSL_get_error(ssl_, accept_res);
                if (err == SSL_ERROR_WANT_READ || err == SSL_ERROR_WANT_WRITE) {
                    LogInfo() << "ServerUserTcp::_handleEvent|SSLHandshake please try later";
                    return 0;
                }
                if (accept_res == 1) {
                    LogInfo() << "ServerUserTcp::_handleEvent|SSLHandshake success";
                    break;
                }

                LogWarn() << "ServerUserTcp::_handleEvent|SSL_accept fail, accept_res =" << accept_res;
                ChangeConnectionState(DoipConnectionState::kFinalize);
                ERR_print_errors_fp(stderr);
                return -1;
            } while (false);

            if (!config.ca.empty()) {
                // Verify server certificate
                X509* cert = SSL_get_peer_certificate(ssl_);
                if (!cert) {
                    // Server did not provide certificate
                    LogWarn() << "ServerUserTcp::_handleEvent|client no offer certificate";
                    ChangeConnectionState(DoipConnectionState::kFinalize);
                    ERR_print_errors_fp(stderr);
                    return -1;
                }

                // Check certificate verification result
                long const verify_result = SSL_get_verify_result(ssl_);
                if (verify_result != X509_V_OK) {
                    // Certificate verification failed
                    LogWarn() << "ServerUserTcp::_handleEvent|certificate authenticate fail, verify_result = "
                              << verify_result;
                    ChangeConnectionState(DoipConnectionState::kFinalize);
                    X509_free(cert);
                    ERR_print_errors_fp(stderr);
                    return false;
                }

                isHandshakeFinished_ = true;
                LogInfo() << "ServerUserTcp::_handleEvent|verify client success, fd =" << GetFd();
                X509_free(cert);
            }
        }
    }
#endif

    /// No readable data
    if ((events & NAI_EV_READ) == 0) {
        return 0;
    }

    if (_IsIgnoreMessage()) {
        ChangeConnectionState(DoipConnectionState::kFinalize);
        LogWarn() << "ServerUserTcp::_handleEvent|recv broadcast or multicast data";
        return -3;
    }

    while (true) {
        if (dropDataLen_ > 0) {
            size_t const originalDropDataLen = dropDataLen_;
            size_t const originalPos         = rPos_;
            size_t finialLen                 = _TrimBuffer(dropDataLen_);
            dropDataLen_ -= finialLen;
            LogInfo() << "ServerUserTcp::_handleEvent|drop step 1 data originalDropDataLen =" << originalDropDataLen
                      << "originalPos =" << originalPos << "finialLen =" << finialLen << "dropDataLen =" << dropDataLen_
                      << "rPos" << rPos_ << "fd =" << GetFd();
        }

        int32_t revRes = -1;
        if (config.isUseTls) {
#ifdef NAI_ENABLE_SSL
            revRes = SSL_read(ssl_, &(*rBuffer_.begin()) + rPos_, rSize_ - rPos_);
#endif
        } else {
            revRes = static_cast< std::int32_t >(
                stream_->Read(&(*rBuffer_.begin()) + rPos_, rSize_ - rPos_, config.isUseTls));
        };

        // int32_t revRes{
        //     static_cast< std::int32_t >(stream_->Read(&(*rBuffer_.begin()) + rPos_, rSize_ - rPos_,
        //     config.isUseTls))};
        if (revRes == 0) {
            ChangeConnectionState(DoipConnectionState::kFinalize);
            LogDebug() << "ServerUserTcp::_handleEvent|close fd=" << GetFd();
            break;
        }

        if (revRes < 0) {
            ec = nai_errno;
            if (ec == NAI_EAGAIN || ec == NAI_EINPROGRESS) {
                LogDebug() << "ServerUserTcp::_handleEvent|read please try later"
                           << isoft::threadsafe::Safe_Strerror(errno) << "fd =" << GetFd();
                break;
            }

            LogError() << "ServerUserTcp::_handleEvent|read data error, msg ="
                       << isoft::threadsafe::Safe_Strerror(errno) << "fd =" << GetFd();
            break;
        }

        /// Update readable data length
        rPos_ += static_cast< size_t >(revRes);

        /// Process DOIP packet
        while (true) {
            if (dropDataLen_ > 0) {
                size_t const originalDropDataLen = dropDataLen_;
                size_t const originalPos         = rPos_;
                size_t finialLen                 = _TrimBuffer(dropDataLen_);
                dropDataLen_ -= finialLen;
                LogInfo() << "ServerUserTcp::_handleEvent|drop step 2 data originalDropDataLen =" << originalDropDataLen
                          << "originalPos =" << originalPos << "finialLen =" << finialLen
                          << "dropDataLen =" << dropDataLen_ << "rPos =" << rPos_ << "fd =" << GetFd();
            }

            /// Extract packet header
            Header head;
            if (!_ParsPackageHeader(head)) {
                break;
            }

            /// Check buffer overflow
            if (rPos_ == rSize_ && (head.payloadLength_ > (rPos_ - kDoipHeadSize))) {  // NOLINT
                LogError() << "ServerUserTcp::_handleEvent|buffer is overflow  rPos_ =" << rPos_ << "fd =" << GetFd()
                           << ",rSize_ =" << rSize_;
                /// Return header negative response kToolLargeMessage
                ResponseGenericHeaderNck(GenericNackType::kToolLargeMessage);
                dropDataLen_ += head.payloadLength_ + kDoipHeadSize;  // NOLINT
                break;
            }

            LogDebug() << "ServerUserTcp::_handleEvent|recvieve doip msg, fd =" << GetFd()
                       << "payloadType =" << ara::log::LogHex16{static_cast< uint16_t >(head.payloadType_)}  // NOLINT
                       << "payloadLength =" << head.payloadLength_;                                          // NOLINT

            /// Check packet header
            GenericNackType const nack = _CheckPackageHeader(head);
            if (nack != GenericNackType::kGenericNackTypeEnd) {
                if (head.payloadType_ != PayloadType::kGenericNegativeAcknowledgement) {  // NOLINT
                    ResponseGenericHeaderNck(nack);
                }

                if (nack == GenericNackType::kInvalidPayloadLength
                    || nack == GenericNackType::kInvalidProtocolVersion) {
                    ChangeConnectionState(DoipConnectionState::kFinalize);
                }

                dropDataLen_ += head.payloadLength_ + kDoipHeadSize;  // NOLINT
                LogError() << "ServerUserTcp::_handleEvent|_CheckPackageHeader fail|nack ="
                           << ara::log::LogHex8{static_cast< uint8_t >(nack)} << "protocolVersion ="
                           << ara::log::LogHex8{static_cast< uint8_t >(head.protocolVersion_)}  // NOLINT
                           << "payloadType ="
                           << ara::log::LogHex16{static_cast< uint16_t >(head.payloadType_)}  // NOLINT
                           << "payloadLength =" << head.payloadLength_ << "fd =" << GetFd();  // NOLINT
                continue;
            }

            /// Failed to extract packet body, insufficient length for a complete packet
            ara::core::Vector< uint8_t > body;
            if (!_ParsPackageBody(head, body)) {
                break;
            }

            std::ignore = _TrimBuffer(head.payloadLength_ + kDoipHeadSize);  // NOLINT

            /// In inactive state, only accept route activation messages
            if (connectionState_ != DoipConnectionState::kActive
                && head.payloadType_ != PayloadType::kRoutingActivationRequest) {  // NOLINT
                LogInfo() << "ServerUserTcp::_handleEvent|not activation but recv other msg fd =" << GetFd();
                break;
            }

            /// Update general activity timer
            UpdateGeneralInactivityTimer();

            /// Process data
            server_->DoipMessageProcessCenter(this, head.payloadType_, body);  // NOLINT
        }
    }

    return 0;
}  // namespace doip

void ServerUserTcp::_releaseAliveCheckTimer(bool const isAlive, int32_t const targetFd)
{
    if (mapAliveCheckInfo_.empty()) {
        return;
    }

    if (targetFd == 0) {
        for (auto&& pair : mapAliveCheckInfo_) {
            AliveCheckInfo& info = pair.second;
            if (info.aliveCheckCb) {
                info.aliveCheckCb(isAlive);
            }

            if (info.aliveCheckTimer.get() != nullptr) {
                info.aliveCheckTimer.reset();
            }
        }

        LogDebug() << "ServerUserTcp::_releaseAliveCheckTimer|size =" << mapAliveCheckInfo_.size();
        mapAliveCheckInfo_.clear();
    } else {
        auto iter = mapAliveCheckInfo_.find(targetFd);
        if (iter == mapAliveCheckInfo_.end()) {
            return;
        }

        if (iter->second.aliveCheckCb == nullptr) {
            LogError() << "ServerUserTcp::_releaseAliveCheckTimer|alive_check_cb is nullptr targetFd =" << targetFd;
            return;
        }

        if (iter->second.aliveCheckTimer.get() == nullptr) {
            LogError() << "ServerUserTcp::_releaseAliveCheckTimer|aliveCheckTimer is nullptr targetFd =" << targetFd;
            return;
        }

        LogDebug() << "ServerUserTcp::_releaseAliveCheckTimer|targetFd =" << targetFd;

        iter->second.aliveCheckCb(isAlive);
        iter->second.aliveCheckTimer.reset();
        mapAliveCheckInfo_.erase(iter);
    }
}

}  // namespace doip
}  // namespace isoft