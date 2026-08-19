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
/// @file       impl_communication_group.h
/// @brief
/// @details
/// @date       2021-12-31
/// @author     mazelin
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_COM_CG_IMPL_CG_H
#define ARA_COM_CG_IMPL_CG_H

#include <cstdint>
#include <set>

#include "ara/com/cg/cg_error_domain.h"
#include "ara/com/internal/log/log.h"
#include "ara/com/types.h"
#include "ara/core/array.h"
#include "ara/core/future.h"
#include "ara/core/map.h"
#include "ara/core/promise.h"
#include "ara/core/string.h"
#include "ara/core/vector.h"

namespace ara {
namespace com {
namespace internal {
namespace cg {
/// @brief
template < class Skeleton, class Proxy, class Msg, class RespMsg >
class TCommunicationGroupServer
{
};

/// @brief
template < class Proxy, class Msg, class RespMsg >
class TCommunicationGroupClientProxy final
{
    /// @brief
    using SendEventFunction = std::function< void(uint32_t clientid, RespMsg const) >;

public:
    /// @brief
    /// @param[in] clientId
    /// @param[in] proxy
    /// @param[in] f
    TCommunicationGroupClientProxy(uint32_t clientId, std::shared_ptr< Proxy > proxy, SendEventFunction f) noexcept
        : clientId_{clientId}, proxy_{proxy}, sendRespMsg_{f}
    {
    }

    /// @brief
    void SetReceiveHandler() noexcept
    {
        ComLogInfo("SetReceiveHandler clientId:", clientId_);

        if (proxy_->response.IsSubscribed() == false) {
            std::ignore = proxy_->response.SetReceiveHandler([this]() { EventCallback(); });
            std::ignore = proxy_->response.Subscribe(1);
        }
    }
    /// @brief Unicast message
    /// @code{.isoft}
    /// export_level=/COM/SoC/CG
    /// @endcode
    /// @param[in] msg Message
    /// @return Future object -- asynchronous/synchronous wait result object
    ara::core::Future< void > message(Msg const msg)  // NOLINT -- Application interface name > naming convention
    {
        return proxy_->message(msg);
    };
    /// @brief
    void EventCallback() noexcept
    {
        std::ignore = proxy_->response.GetNewSamples(
            [this](auto sample) {
                (void)sample;
                sendRespMsg_(clientId_, *sample);
            },
            1);
    };
    void Unsubscribe() noexcept
    {
        if (proxy_->response.IsSubscribed()) {
            proxy_->response.Unsubscribe();
        }
    }

private:
    /// @brief client id
    uint32_t clientId_;
    /// @brief proxy pointer
    std::shared_ptr< Proxy > proxy_;
    /// @brief event callback
    SendEventFunction sendRespMsg_;
};

/// @brief
template < class Skeleton, class Proxy, class Msg, class RespMsg >
class TCommunicationGroup final : public Skeleton
{
    using ProxyPointerMap
        = ara::core::Map< uint32_t, std::unique_ptr< TCommunicationGroupClientProxy< Proxy, Msg, RespMsg > > >;
    using ListClientsOutput = typename Skeleton::listClientsOutput;
    using Skeleton::Skeleton;

public:
    /// @brief Constructor
    /// @param[in] instanceIdentifier Instance identifier
    /// @param[in] mode Method call handling mode
    explicit TCommunicationGroup(ara::com::InstanceIdentifier instanceIdentifier,
                                 ara::com::MethodCallProcessingMode mode) noexcept
        : Skeleton(std::move(instanceIdentifier), mode), proxy_{}
    {
    }
    /// @brief
    ~TCommunicationGroup() noexcept = default;
    /// @brief Move constructor
    /// @param[in] other
    TCommunicationGroup(TCommunicationGroup&& other) noexcept : Skeleton{std::move(other)} {}
    /// @brief Copy constructor
    /// @param other
    TCommunicationGroup(TCommunicationGroup const& other) noexcept = default;
    /// @brief Copy assignment operator
    /// @param other
    /// @return TCommunicationGroup
    TCommunicationGroup& operator=(TCommunicationGroup const& other) noexcept = default;
    /// @brief Move assignment operator
    /// @param[in] other
    /// @return TCommunicationGroup&
    TCommunicationGroup& operator=(TCommunicationGroup&& other) noexcept
    {
        if (this != &other) {
            Skeleton::operator=(std::move(other));
        }
        return *this;
    }

    /// @brief Broadcast message -- server side
    /// @code{.isoft}
    /// export_level=/COM/SoC/CG
    /// @endcode
    /// @param[in] msg Message
    /// @return Future object -- asynchronous/synchronous wait result object
    ara::core::Future< void > broadcast(Msg const& msg) noexcept final  // NOLINT -- Application interface name > naming convention
    {
        typename ara::core::Promise< void > promise;
        for (auto const& it : proxy_) {
            auto messageFuture{it.second->message(msg)};
            ComLogDebug("broadcast to ", it.first);
        }
        promise.set_value();
        return promise.get_future();
    };
    /// @brief Unicast message -- server side
    /// @code{.isoft}
    /// export_level=/COM/SoC/CG
    /// @endcode
    /// @param[in] clientId Client identifier
    /// @param[in] msg Message
    /// @return Future object -- asynchronous/synchronous wait result object
    ara::core::Future< void > message(uint32_t const& clientId,  // NOLINT -- Application interface name > naming convention
                                      Msg const& msg) noexcept final
    {
        typename ara::core::Promise< void > promise;
        auto it{proxy_.find(clientId)};
        if (it != proxy_.end()) {
            return it->second->message(msg);
        }
        promise.SetError(ara::com::cg::CgErrc::kWrongClientAddress);
        return promise.get_future();
    };
    /// @brief Get client list -- server side
    /// @code{.isoft}
    /// export_level=/COM/SoC/CG
    /// @endcode
    /// @return Future object -- asynchronous/synchronous wait result object
    ara::core::Future< ListClientsOutput > listClients() noexcept final  // NOLINT -- Application interface name > naming convention
    {
        ara::core::Promise< ListClientsOutput > promise;
        ListClientsOutput output;
        ara::core::Vector< uint32_t > clients;
        for (auto it = proxy_.begin(); it != proxy_.end(); it++) {
            clients.push_back(it->first);
        }
        if (clients.size() > static_cast< size_t >(0)) {
            std::lock_guard< std::mutex > lock(clientsMutex_);
            output.clients.assign(clients.begin(), clients.end());
            promise.set_value(std::move(output));

        } else {
            promise.SetError(ara::com::cg::CgErrc::kNoClients);
        }
        return promise.get_future();
    };

public:
    /// @brief
    /// @return bool
    bool InitClient(/*ara::core::StringView instance_id, int32_t clientId*/) noexcept
    {
        auto handleRes{Proxy::StartFindService(
            [this](auto handles, auto) { this->ServiceAvailabilityCallback(std::move(handles)); },
            ara::com::InstanceIdentifier::MakeAny())};
        if (!handleRes) {
            return false;
        }
        findHandle_ = std::move(handleRes).Value();
        return true;
    };

    /// @brief
    void StopFindService() noexcept { Proxy::StopFindService(findHandle_); };

    /// @brief
    /// @param[in] id
    /// @return
    uint32_t ClientID(ara::com::InstanceIdentifier const& id) noexcept
    {
        ara::core::String const someipprefix{"NSOMEIP:"};
        ara::core::String const ddsprefix{"DDS:"};
        ComLogDebug("Find Instance ", id.ToString());
        uint32_t clientid{};

        constexpr uint32_t kSomeipClientIdOffset{0x00010000};  // SOME/IP client ID offset
        constexpr uint32_t kDdsClientIdOffset{0x00020000};     // DDS client ID offset
        if (id.ToString().substr(static_cast< ara::core::String::size_type >(0), someipprefix.size()) == someipprefix) {
            clientid = kSomeipClientIdOffset + std::stoi(id.ToString().substr(someipprefix.size()).data());
        } else if (id.ToString().substr(static_cast< ara::core::String::size_type >(0), ddsprefix.size())
                   == ddsprefix) {
            clientid = kDdsClientIdOffset + std::stoi(id.ToString().substr(ddsprefix.size()).data());
        }

        ComLogDebug("Instance ", id.ToString(), GenArg(clientid));
        return clientid;
    }

    /// @brief
    /// @param[in] clientId
    /// @param[in] msg
    void SendEvent(uint32_t clientId, RespMsg const msg) noexcept
    {
        auto e{Skeleton::response.Allocate().Value()};
        e->clientID = clientId;
        e->respMsg  = msg;
        Skeleton::response.Send(std::move(e));
        return;
    };
    /// @brief
    /// @param[in] handles
    void ServiceAvailabilityCallback(ara::com::ServiceHandleContainer< typename Proxy::HandleType >
                                         handles /*,ara::com::FindServiceHandle handler*/) noexcept
    {
        std::set< uint32_t > tempClientId;
        for (auto const& it : handles) {
            std::lock_guard< std::mutex > lock(proxyMutex_);

            auto proxyRes{Proxy::Create(it)};
            if (!proxyRes) {
                continue;
            };

            auto proxy{std::make_shared< Proxy >(std::move(proxyRes).Value())};
            auto clientId{ClientID(it.GetInstanceId())};
            std::ignore = tempClientId.emplace(clientId);

            if (proxy_.find(clientId) == proxy_.end()) {
                std::ignore = proxy_.insert(std::make_pair(
                    clientId,
                    new TCommunicationGroupClientProxy< Proxy, Msg, RespMsg >(
                        clientId, proxy, [this](uint32_t clientID, RespMsg const msg) { SendEvent(clientID, msg); })));
                proxy_.find(clientId)->second->SetReceiveHandler();
            }
        }
        // If the proxy_ list is updated, remove proxies that have been disconnected, i.e., do not return client ids with invalid skeletons.
        for (auto it = proxy_.begin(); it != proxy_.end();) {
            // If the current proxy_ list does not find the client id, remove it.
            if (tempClientId.find(it->first) == tempClientId.end()) {
                (it->second)->Unsubscribe();
                std::ignore = proxy_.erase(it++);
                ComLogInfo("proxy_clientid: ", it->first, "delete");
            } else {
                it++;
            }
        }
    }

private:
    /// @brief
    ProxyPointerMap proxy_;
    /// @brief
    std::mutex proxyMutex_;
    /// @brief
    std::mutex clientsMutex_;
    /// @brief
    ara::com::FindServiceHandle findHandle_;
};
}  // namespace cg
}  // namespace internal
}  // namespace com
}  // namespace ara
#endif