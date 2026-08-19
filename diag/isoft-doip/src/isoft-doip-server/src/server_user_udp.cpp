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
/// @file       server_user_udp.cpp
/// @brief      This file provides DOIP server UDP related definitions
/// @details
/// @date       2023-10-08
/// @author     zhanglipeng
/// @version    1.2.0
///
/// ================================================================

#include "server_user_udp.h"

#include "isoft/doip/public_message.h"
#include "public_log.h"
#include "public_udp_dgram.h"
#include "server_impl.h"
#include "thread/therad_safe.h"

namespace isoft {
namespace doip {

/// @brief Constructor
/// @param[in] fd sock fd
/// @param[in] udp udp dgram
/// @param[in] server DOIP service implementation
/// @param[in] bufferSize Read buffer size
ServerUserUdp::ServerUserUdp(int32_t fd, std::shared_ptr< PublicUdpDgram > udp, ServerImpl* server, size_t bufferSize)
    : ServerUserBase(bufferSize, DoIPMessageTransportType::kDoIPMessageTransportType_UDP, server)
    , fd_{fd}
    , udp_{std::move(udp)}
{
}

/// @brief Destructor
ServerUserUdp::~ServerUserUdp() noexcept
{
    std::unique_lock< std::recursive_mutex > const lock{udpLock_};
    udp_.reset();
}

/// @brief Process received UDP data
void ServerUserUdp::Process(uint8_t* data, size_t const size)
{
    {
        std::unique_lock< std::recursive_mutex > const lock{udpLock_};
        if (udp_.get() == nullptr) {
            LogError() << "ServerUserUdp::Process|udp_ is nullptr";
            _releaseSelf();
            return;
        }

        if (data == nullptr) {
            LogError() << "ServerUserUdp::Process|data is nullptr";
            _releaseSelf();
            return;
        }

        // int64_t const revSize = udp_->Recv(&(*rBuffer_.begin()), rSize_, &naiSocknbufT_.addr, &naiSocknbufT_.len);
        // if (revSize == -1 || revSize == 0) {
        //     LogError() << "ServerUserUdp::Process|revSize is " << revSize;
        //     _releaseSelf();
        //     return;
        // }

        // rPos_ += static_cast< size_t >(revSize);

        // LogDebug() << "ServerUserUdp::Process|recv data size =" << revSize << "rPos =" << rPos_ << "fd =" << fd_;

        rBuffer_.assign(data, data + size);

        rPos_ += size;

        // rPos_ += static_cast< size_t >(revSize);

        LogDebug() << "ServerUserUdp::Process|recv data size =" << size << "rPos =" << rPos_ << "fd =" << fd_;
    }

    if (_IsIgnoreMessage()) {
        LogError() << "ServerUserUdp::Process|recv broadcast or multicast data fd =" << fd_;
        _releaseSelf();
        return;
    }

    // std::set<ara::core::String> const& out_list_ip4 = server_->GetAllIpv4();
    // std::set<ara::core::String>::iterator it = out_list_ip4.find(source_ip);
    // if (it != out_list_ip4.end()) {
    //     return;
    // }

#ifndef UDP_MESSAGE_SPECIAL_HANDLE
    /// Extract packet header
    Header head;
    if (!_ParsPackageHeader(head)) {
        ResetBuffer();
        _releaseSelf();
        LogWarn() << "ServerUserUdp::Process|header not complete, read_size =" << rPos_;
        return;
    }

    /// Check packet header
    GenericNackType const nack = _CheckPackageHeader(head);
    if (nack != GenericNackType::kGenericNackTypeEnd) {                              // NOLINT
        if (head.payloadType_ != PayloadType::kVehicleAnnoucementIdResponse          // NOLINT
            && head.payloadType_ != PayloadType::kGenericNegativeAcknowledgement) {  // NOLINT
            ResponseGenericHeaderNck(nack);
        }

        if (head.payloadType_ != PayloadType::kVehicleAnnoucementIdResponse) {  // NOLINT
            LogError() << "ServerUserUdp::Process|_CheckPackageHeader fail|nack ="
                       << ara::log::LogHex8{static_cast< uint8_t >(nack)}  // NOLINT
                       << "protocolVersion ="
                       << ara::log::LogHex8{static_cast< uint8_t >(head.protocolVersion_)}                   // NOLINT
                       << "payloadType =" << ara::log::LogHex16{static_cast< uint16_t >(head.payloadType_)}  // NOLINT
                       << "payloadLength =" << head.payloadLength_ << "fd" << fd_;                           // NOLINT
        }
        ResetBuffer();
        _releaseSelf();
        return;
    }

    /// Extract packet body
    ara::core::Vector< uint8_t > body;
    if (!_ParsPackageBody(head, body)) {
        ResetBuffer();
        _releaseSelf();
        LogWarn() << "ServerUserUdp::Process|body not complete, body_size =" << rPos_
                  << "header_size =" << head.payloadLength_;
        return;
    }

    if ((head.payloadLength_ + kDoipHeadSize) != rPos_) {
        ResponseGenericHeaderNck(GenericNackType::kInvalidPayloadLength);
        ResetBuffer();
        _releaseSelf();
        return;
    }

    LogDebug() << "ServerUserUdp::Process|receive doip msg,"
               << "msgType =" << ara::log::LogHex16{static_cast< uint16_t >(head.payloadType_)}  // NOLINT
               << "bodySize =" << body.size() << "fd =" << fd_;

    ResetBuffer();

    /// Process data
    server_->DoipMessageProcessCenter(this, head.payloadType_, body);  // NOLINT
#else
    size_t i{0U};
    bool isNeedRelease{true};
    while (rPos_ > 0) {
        /// Extract packet header
        Header head;
        if (!_ParsPackageHeader(head)) {
            LogError() << "ServerUserUdp::Process|head pars err rPos =" << rPos_ << "i =" << i << "fd =" << fd_;
            break;
        }

        LogDebug() << "ServerUserUdp::Process|multiple receive doip msg,"
                   << "msgType =" << ara::log::LogHex16{static_cast< uint16_t >(head.payloadType_)}  // NOLINT
                   << "payloadLength =" << head.payloadLength_ << "fd =" << fd_ << "i =" << i;       // NOLINT

        /// Check packet header
        GenericNackType const nack = _CheckPackageHeader(head);
        if (nack != GenericNackType::kGenericNackTypeEnd) {
            if (head.payloadType_ != PayloadType::kVehicleAnnoucementIdResponse          // NOLINT
                && head.payloadType_ != PayloadType::kGenericNegativeAcknowledgement) {  // NOLINT
                ResponseGenericHeaderNck(nack);                                          // NOLINT
            }
            std::ignore = _TrimBuffer(head.payloadLength_ + kDoipHeadSize);  // NOLINT
            ++i;
            if (head.payloadType_ != PayloadType::kVehicleAnnoucementIdResponse) {  // NOLINT
                LogError() << "ServerUserUdp::Process|multiple _CheckPackageHeader fail|nack ="
                           << ara::log::LogHex8{static_cast< uint8_t >(nack)} << "protocolVersion ="
                           << ara::log::LogHex8{static_cast< uint8_t >(head.protocolVersion_)}  // NOLINT
                           << "payloadType ="
                           << ara::log::LogHex16{static_cast< uint16_t >(head.payloadType_)}            // NOLINT
                           << "payloadLength =" << head.payloadLength_ << "fd =" << fd_ << "i =" << i;  // NOLINT
            }

            continue;
        }

        /// Extract packet body
        ara::core::Vector< uint8_t > body;
        if (!_ParsPackageBody(head, body)) {
            LogError() << "ServerUserUdp::Process|body pars err rPos =" << rPos_ << "i =" << i << "fd =" << fd_;
            break;
        }

        /// Process data, only process the first packet
        if (i == 0U) {
            isNeedRelease = false;
            server_->DoipMessageProcessCenter(this, head.payloadType_, body);  // NOLINT
        } else {                                                               /// Return negative response for remaining packets
            ResponseGenericHeaderNck(GenericNackType::kReceiveBufferOverflow);
        }
        std::ignore = _TrimBuffer(head.payloadLength_ + kDoipHeadSize);  // NOLINT
        ++i;
    }

    ResetBuffer();
    // Clear cache
    if (isNeedRelease) {
        _releaseSelf();
    }

#endif
}

/// @brief Send data to peer
/// @param[in] type Protocol number
/// @param[in] body Message body
/// @return Send result
int32_t ServerUserUdp::SendToClient(PayloadType const type, ara::core::Vector< uint8_t > const& body)
{
    std::unique_lock< std::recursive_mutex > const lock{udpLock_};
    if (udp_.get() == nullptr) {
        LogError() << "ServerUserUdp::SendToClient|udp_ is nullptr";
        return -1;
    }

    /// Construct packet header
    ara::core::Vector< uint8_t > msg;
    _StructurePackageHeader(type, static_cast< uint32_t >(body.size()), msg);

    /// Assemble complete packet
    std::ignore = msg.insert(msg.cend(), body.begin(), body.end());
    int32_t res = static_cast< int32_t >(udp_->Send(msg.data(), msg.size(), &naiSocknbufT_.addr, naiSocknbufT_.len));
    if (res == -1) {
        LogError() << "ServerUserUdp::SendToClient|err:" << isoft::threadsafe::Safe_Strerror(errno) << "fd =" << fd_;
        return res;
    }

    LogDebug() << "ServerUserUdp::SendToClient|type =" << ara::log::LogHex16{static_cast< uint16_t >(type)}
               << "bodySize =" << body.size() << "totalSize =" << msg.size() << "sendSize =" << res << "fd =" << fd_;
    return res;
}

void ServerUserUdp::_releaseSelf()
{
    if (server_ == nullptr) {
        LogError() << "ServerUserUdp::_releaseSelf|server_ is nullptr !";
        return;
    }

    server_->DelayRemoveUdpUser(fd_);
}

}  // namespace doip
}  // namespace isoft