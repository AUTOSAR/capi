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
/// @file       communication_group.h
/// @brief      Communication Group header file
/// @details
/// @date       2021-12-31
/// @author     mazelin
/// @version    1.2.0
///
/// ================================================================

#pragma once

#include <functional>
#include <memory>

#include "ara/core/future.h"
#include "ara/core/vector.h"

namespace ara {
namespace com {
namespace cg {
/// @brief serve large amount of clients
using ClientId = uint32_t;
/// @brief clients
using ClientList = ara::core::Vector< ClientId >;

/// @brief Abstract base class for Communication Group Server and Client
class CommunicationGroup
{
public:
    CommunicationGroup() noexcept          = default;
    virtual ~CommunicationGroup() noexcept = default;

    // no copy or move ..., for now.
    CommunicationGroup(CommunicationGroup const& other) noexcept = delete;
    CommunicationGroup(CommunicationGroup&& other) noexcept      = delete;
    CommunicationGroup& operator=(CommunicationGroup const& other) noexcept = delete;
    CommunicationGroup& operator=(CommunicationGroup&& other) noexcept = delete;
};

/// @brief Template class for clients of a Communication Group
template < class CgMsg, class CgRespMsg >
class CgClient : public CommunicationGroup
{
public:
    /// @brief MessageFunc is invoked by CgClient, if a message is received
    using MessageFunc = std::function< void(CgMsg const&) >;

    explicit CgClient(MessageFunc mf) noexcept : messageFunc_{mf} {}
    ~CgClient() noexcept override            = default;
    CgClient(CgClient const& other) noexcept = delete;
    CgClient(CgClient&& other) noexcept      = delete;
    CgClient& operator=(CgClient const& other) noexcept = delete;
    CgClient& operator=(CgClient&& other) noexcept = delete;

    ara::core::Future< void > Respond(CgRespMsg const& msg);

private:
    MessageFunc messageFunc_;
};

/// @brief Template class for server of a Communication Group
template < class CgMsg, class CgRespMsg >
class CgServer : public CommunicationGroup
{
public:
    /// @brief ResponseFunc is invoked, if a response from a client arrives
    using ResponseFunc = std::function< void(CgRespMsg const&, ClientId const) >;

    explicit CgServer(ResponseFunc rf) noexcept : responseFunc_{rf} {}
    ~CgServer() noexcept override            = default;
    CgServer(CgServer const& other) noexcept = delete;
    CgServer(CgServer&& other) noexcept      = delete;
    CgServer& operator=(CgServer const& other) noexcept = delete;
    CgServer& operator=(CgServer&& other) noexcept = delete;

    ara::core::Future< void > Broadcast(CgMsg const& msg);
    ara::core::Future< void > SendMessage(CgMsg const& msg, ClientId const client);
    ara::core::Future< ClientList > ListClients();

private:
    ResponseFunc responseFunc_;
};
}  // namespace cg
}  // namespace com
}  // namespace ara
