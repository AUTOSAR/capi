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
/// @file       diag_channel.h
/// @brief      This file provides the definition of the diagnostic connection class
/// @details
/// @date       2024-12-03
/// @author     zhanglipeng
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_DIAG_DOIP_DIAG_CONNECTION_H_
#define ARA_DIAG_DOIP_DIAG_CONNECTION_H_
#include <isoft/doip/public_def.h>
#include <isoft/doip/public_message.h>
#include <isoft/uds/channel.h>
#include <isoft/uds/common.h>

#include "common.h"
#include "doip_server.h"
namespace ara {
namespace diag {
namespace doip {

/// @brief Meta information length
std::uint8_t const kMetaInfoLen{8U};

/// @brief Forward declaration of DoipService
class DoIPServer;

/// @brief Diagnostic channel, used to handle communication at the diagnostic level.
class DiagChannel : public isoft::uds::server::Channel
{
public:
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
    DiagChannel(DoIPServer* const doipService,
                uint16_t const sa,
                isoft::uds::server::ChannelIdentifier const udsChannelId,
                uint8_t const networkId,
                int32_t const fd,
                ara::core::String localIp,
                uint16_t const localPort,
                ara::core::String peerIp,
                uint16_t const peerPort);

    /// @brief Destructor
    /// @throw unknown
    ~DiagChannel() noexcept override;

    /// @brief Copy constructor
    /// @param[in] other
    /// @throw unknown
    DiagChannel(DiagChannel const& other) = delete;

    /// @brief Assignment operation
    /// @param[in] right
    /// @return Reference
    /// @throw unknown
    DiagChannel& operator=(DiagChannel const& right) = delete;

    /// @brief Move constructor
    /// @param[in] right
    /// @throw unknown
    DiagChannel(DiagChannel&& right) = delete;

    /// @brief Move operation
    /// @param[in] right
    /// @return Reference
    /// @throw unknown
    DiagChannel& operator=(DiagChannel&& right) = delete;

    isoft::uds::Result< bool > Respond(std::shared_ptr< isoft::uds::server::Message >& response) override;
    isoft::uds::Result< void > Respond(
        std::vector< std::shared_ptr< isoft::uds::server::Message > >& responses) override;
    bool ReestablishAfterRestarted(uint16_t const ta) override;

    isoft::uds::server::ChannelIdentifier GetIdentifier() override { return udsChannelId_; }
    size_t GetMaxPayloadLength() override;

    std::string GetLocalIp() override { return std::string(localIp_.c_str()); }
    std::uint16_t GetLocalPort() override { return localPort_; }
    std::string GetRemoteIp() override { return std::string(peerIp_.c_str()); }
    std::uint16_t GetRemotePort() override { return peerPort_; }

    /// @brief Send positive diagnostic response. Use this interface when the upper layer allows receiving diagnostic messages.
    /// @param[in] sa sa
    /// @param[in] ta ta
    /// @return True: Sending succeeded False: Sending failed
    /// @throw unknown
    bool SendDiagAck(uint16_t const sa, uint16_t const ta);

    /// @brief Send diagnostic rejection response. Use this interface when the upper layer rejects diagnostic messages.
    /// @param[in] errorCode Rejection reason
    /// @param[in] sa sa
    /// @param[in] ta ta
    /// @return True: Sending succeeded False: Sending failed
    /// @throw unknown
    bool SendDiagNack(isoft::doip::DiagNackType const errorCode, uint16_t const sa, uint16_t const ta);

    /// @brief Get SA
    /// @return sa
    uint16_t GetSa() const { return sa_; }

    /// @brief Get network port ID
    /// @return Network port ID
    /// @throw unknown
    uint8_t NetworkId() const noexcept { return networkId_; }

    /// @brief Get FD
    /// @return fd
    /// @throw unknown
    int32_t Fd() const noexcept { return fd_; }

    /// @brief Stop
    /// @throw unknown
    void Stop() noexcept;

private:
    /// @name doipService_
    /// Implementation object of DoIP interface, implementing the interface exposed to the diagnostic service layer
    DoIPServer* doipService_;

    /// @brief Diagnostic SA
    uint16_t sa_;

    /// @name udsChannelId_
    /// Channel ID, ensuring the uniqueness of the diagnostic connection object
    isoft::uds::server::ChannelIdentifier udsChannelId_;

    /// @brief Network port ID
    uint8_t networkId_;

    /// @brief sock
    int32_t fd_;

    /// @brief Local IP
    ara::core::String localIp_;

    /// @brief Local port
    uint16_t localPort_;

    /// @brief Peer IP
    ara::core::String peerIp_;

    /// @brief Peer port
    uint16_t peerPort_;
};

}  // namespace doip
}  // namespace diag
}  // namespace ara
#endif  // ARA_DIAG_DOIP_DIAG_CONNECTION_H_