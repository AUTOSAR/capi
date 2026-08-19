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
/// @file       diag_channel.cpp
/// @brief      This file provides the definition of the Diagnostic Connection class
/// @details
/// @date       2024-12-03
/// @author     zhanglipeng
/// @version    1.2.0
///
/// ================================================================

#include "diag_channel.h"

#include <iostream>

#include "channel_id_manager.h"
#include "doip_config.h"
#include "log/log.h"
namespace ara {
namespace diag {
namespace doip {
/// @brief Constructor
/// @param[in] doipService DoIP server
/// @param[in] sa sa
/// @param[in] udsChannelId channelID
/// @param[in] networkId Network port ID
/// @param[in] fd sock fd
/// @param[in] localIp Local IP, changed to pass by value, rvalue input to avoid copy, lvalue input will cause copy
/// @param[in] localPort Local port
/// @param[in] peerIp Peer IP, same as localIp
/// @param[in] peerPort Peer port
DiagChannel::DiagChannel(DoIPServer* const doipService,
                         uint16_t const sa,
                         isoft::uds::server::ChannelIdentifier const udsChannelId,
                         uint8_t const networkId,
                         int32_t const fd,
                         ara::core::String localIp,
                         uint16_t const localPort,
                         ara::core::String peerIp,
                         uint16_t const peerPort)
    : doipService_{doipService}
    , sa_{sa}
    , udsChannelId_{udsChannelId}
    , networkId_{networkId}
    , fd_{fd}
    , localIp_{std::move(localIp)}
    , localPort_{localPort}
    , peerIp_{std::move(peerIp)}
    , peerPort_{peerPort}
{
}

/// @brief Destructor
/// @throw unknown
DiagChannel::~DiagChannel() noexcept { ChannelIdManager::GetInstance().RemoveChannelId(udsChannelId_); }

isoft::uds::Result< bool > DiagChannel::Respond(std::shared_ptr< isoft::uds::server::Message >& response)
{
    if (response.get() == nullptr) {
        common::LogError() << "DiagChannel::Respond|response is closed";
        return isoft::uds::Result< bool >::FromValue(false);
    }

    if (fd_ == -1) {
        common::LogError() << "DiagChannel::Respond|diag channel is closed";
        return isoft::uds::Result< bool >::FromValue(false);
    }

    if (doipService_ == nullptr) {
        common::LogError() << "DiagChannel::Respond|doipService_ is nullptr";
        return isoft::uds::Result< bool >::FromValue(false);
    }

    isoft::doip::DiagMessage diag;
    diag.sourceAddress_ = response->GetSA();  // NOLINT
    diag.targetAddress_ = response->GetTA();  // NOLINT
    std::ignore
        = diag.uds_.insert(diag.uds_.cbegin(), response->GetBody().cbegin(), response->GetBody().cend());  // NOLINT
    ara::core::Vector< uint8_t > body;
    diag.Serialize(body);
    bool const sendRes = doipService_->SendDoIPMessage(networkId_, fd_, isoft::doip::PayloadType::kDiagMessage, body);
    common::LogVerbose() << "DiagChannel::Respond|success sendRes =" << sendRes << "networkId =" << networkId_
                         << "fd =" << fd_ << "sa =" << sa_ << "uds_channel_id =" << udsChannelId_
                         << "uds_size =" << response->GetBody().size();

    return isoft::uds::Result< bool >::FromValue(true);
}

isoft::uds::Result< void > DiagChannel::Respond(
    std::vector< std::shared_ptr< isoft::uds::server::Message > >& responses)
{
    if (responses.empty()) {
        common::LogError() << "DiagChannel::Respond|multiple response is empty";
        return isoft::uds::Result< void >::FromError(0);
    }

    if (fd_ == -1) {
        common::LogError() << "DiagChannel::Respond|multiple diag channel is closed";
        return isoft::uds::Result< void >::FromError(0);
    }

    if (doipService_ == nullptr) {
        common::LogError() << "DiagChannel::Respond|multiple doipService_ is nullptr";
        return isoft::uds::Result< void >::FromError(0);
    }

    for (auto& entry : responses) {
        std::ignore = Respond(entry);
    }

    common::LogVerbose() << "DiagChannel::Respond|multiple success networkId =" << networkId_ << "fd =" << fd_
                         << "sa =" << sa_ << "uds_channel_id =" << udsChannelId_
                         << "responses_size =" << responses.size();

    return {};
}

bool DiagChannel::ReestablishAfterRestarted(uint16_t const ta)
{
    common::LogInfo() << "DiagChannel::NotifyReestablishment|channelId =" << udsChannelId_ << "ta =" << ta
                      << "networkId =" << networkId_ << "fd =" << fd_;
    ChannelIdManager::GetInstance().SaveChannelId(udsChannelId_, ta);
    return true;
}

size_t DiagChannel::GetMaxPayloadLength()
{
    size_t const payloadSize = DoIPConfig::GetConfig().MaxRequestBytes();
    return payloadSize;
}

/// @brief Send positive diagnostic response. Use this interface when the upper layer allows receiving diagnostic messages.
/// @param[in] sa sa
/// @param[in] ta ta
/// @return True: Sending succeeded False: Sending failed
/// @throw unknown
bool DiagChannel::SendDiagAck(uint16_t const sa, uint16_t const ta)
{
    if (doipService_ == nullptr) {
        common::LogError() << "DiagConnection::SendDiagAck|doipService_ is null sa = " << sa << "ta = " << ta;
        return false;
    }

    isoft::doip::DiagMessageAck ack;
    ack.sourceAddress_ = sa;  // NOLINT
    ack.targetAddress_ = ta;  // NOLINT
    ack.ack_           = 0U;  // NOLINT
    ara::core::Vector< uint8_t > body;
    ack.Serialize(body);
    return doipService_->SendDoIPMessage(networkId_, fd_, isoft::doip::PayloadType::kDiagMessagePositiveAcknowledgement,
                                         body);
}

/// @brief Send diagnostic rejection response. Use this interface when the upper layer rejects diagnostic messages.
/// @param[in] errorCode Rejection reason
/// @param[in] sa sa
/// @param[in] ta ta
/// @return True: Sending succeeded False: Sending failed
/// @throw unknown
bool DiagChannel::SendDiagNack(isoft::doip::DiagNackType const errorCode, uint16_t const sa, uint16_t const ta)
{
    if (doipService_ == nullptr) {
        common::LogError() << "DiagConnection::SendDiagNack|doipService_ is null sa = " << sa << "ta = " << ta;
        return false;
    }

    isoft::doip::DiagMessageNack nck;
    nck.sourceAddress_ = sa;         // NOLINT
    nck.targetAddress_ = ta;         // NOLINT
    nck.nck_           = errorCode;  // NOLINT
    ara::core::Vector< uint8_t > body;
    nck.Serialize(body);
    return doipService_->SendDoIPMessage(networkId_, fd_, isoft::doip::PayloadType::kDiagMessageNegativeAcknowledgement,
                                         body);
}

/// @brief Stop
void DiagChannel::Stop() noexcept { fd_ = -1; }

}  // namespace doip
}  // namespace diag
}  // namespace ara