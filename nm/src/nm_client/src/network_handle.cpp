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
/// @file       network_handle.cpp
/// @brief      Network handle instance user interface class
/// @details
/// @date       2022-06-10
/// @author     hejunwei
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/NetworkManager/lib
/// @interface_level=software
/// @trace_id_sr=SRS_NM_00001
/// @unit_name=NetworkHandle
/// @unit_description=Network handle instance user interface class
/// @endcode
///
/// ================================================================

#include "include/public/network_handle.h"

#include <isoft/ara_fsh/filesystem_hierarchy.h>

#include "include/internal/lnconfig.h"
#include "include/internal/nmipc.h"
#include "include/internal/smcheck.h"
#include "include/public/nm_error_domain.h"
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/writer.h"

namespace ara {
namespace nm {

/// @brief Specialization declaration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100000
/// @trace_id_dd=DD_NM_00973
/// @needwork = ad
/// @endcode
template ara::core::Result< void > NetworkHandle::RegisterNetworkStateChangeNotifier<
    std::function< void(NetworkHandle::NetworkStateChangeNotifier &, NetworkHandle::NetworkStateType const &) > & >(
    NetworkHandle::NetworkStateChangeNotifier,
    std::function< void(NetworkHandle::NetworkStateChangeNotifier &, NetworkHandle::NetworkStateType const &) > &);

/// @brief Specialization declaration
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_NM_100001
/// @trace_id_dd=DD_NM_00974
/// @needwork = ad
/// @endcode
template ara::core::Result< void > NetworkHandle::RegisterNetworkRequestedStateChangeNotifier<
    std::function< void(NetworkHandle::NetworkStateChangeNotifier &, NetworkHandle::NetworkStateType const &) > & >(
    NetworkHandle::NetworkStateChangeNotifier,
    std::function< void(NetworkHandle::NetworkStateChangeNotifier &, NetworkHandle::NetworkStateType const &) > &);

/// @brief Constructor
/// @param specifier Descriptor
NetworkHandle::NetworkHandle(ara::core::InstanceSpecifier const &specifier) noexcept
{
    ara::core::String const mapFile{isoft::ara_fsh::Process().GetConfig("network_handle_map.json")};
    LnConfig config;
    std::int32_t const r{config.Load(mapFile)};
    if (0 == r) {
        ara::core::String const spcifierStr{specifier.ToString().data()};
        ara::core::StringView const lnRef{config.GetLnName(spcifierStr)};
        if (lnRef.length() > 0U) {
            lnName_ = ara::core::String{lnRef.data()};
        }
    }
    if (!lnName_.empty()) {
        ipcPtr_ = NmIpc::GetNmIpcProHandler();
    }
    networkStateChangeNotifier_       = nullptr;
    networkReuestStateChangeNotifier_ = nullptr;
}

/// @brief Destructor
NetworkHandle::~NetworkHandle() noexcept
{
    ipcPtr_ = nullptr;
    NmIpc::CloseNmIpcProHandler();
}

/// @brief Get network state
/// @return Network state
ara::core::Result< NetworkStateType > NetworkHandle::GetNetworkState() const noexcept
{
#ifndef ARA_NM_NOCHECK_SM_PROCESS
    if (!SMCheck::CheckAccessPermission()) {
        return ara::core::Result< NetworkStateType >::FromError(NmErrc::kAccessWithoutPermission);
    }
#endif
    if (nullptr != ipcPtr_) {
        rapidjson::Document document;
        std::ignore = document.SetObject();
        rapidjson::Document::AllocatorType &rapidallocator{document.GetAllocator()};

        std::ignore = document.AddMember(std::move(rapidjson::GenericStringRef< NmChar >("cmd")),
                                         static_cast< std::int32_t >(internal::IpcCommand::kGetNetworkCurrentState),
                                         document.GetAllocator());
        std::ignore = document.AddMember(std::move(rapidjson::GenericStringRef< NmChar >("instance")),
                                         rapidjson::Value().SetString(lnName_.c_str(), rapidallocator), rapidallocator);

        // Convert Document to string
        rapidjson::StringBuffer buffer;
        rapidjson::Writer< rapidjson::StringBuffer > writer{buffer};
        std::ignore = document.Accept(writer);

        ara::core::String sendstr{buffer.GetString()};
        std::ignore = sendstr.append("\n");
        std::uint32_t serverCode{0U};
        std::int32_t const ret{ipcPtr_->DoIpcSyncRequest(sendstr, serverCode)};
        if (0 == ret) {
            return ara::core::Result< NetworkStateType >::FromValue(static_cast< NetworkStateType >(serverCode));
        }
    }
    return ara::core::Result< NetworkStateType >::FromError(NmErrc::kServiceNotAvailable);
}

/// @brief  Get network request state
/// @return Network request state
ara::core::Result< NetworkStateType > NetworkHandle::GetNetworkRequestedState() const noexcept
{
#ifndef ARA_NM_NOCHECK_SM_PROCESS
    if (!SMCheck::CheckAccessPermission()) {
        return ara::core::Result< NetworkStateType >::FromError(NmErrc::kAccessWithoutPermission);
    }
#endif
    if (nullptr != ipcPtr_) {
        rapidjson::Document document;
        std::ignore = document.SetObject();
        rapidjson::Document::AllocatorType &rapidallocator{document.GetAllocator()};

        std::ignore = document.AddMember(std::move(rapidjson::GenericStringRef< NmChar >("cmd")),
                                         static_cast< std::int32_t >(internal::IpcCommand::kGetNetworkRequestState),
                                         document.GetAllocator());
        std::ignore = document.AddMember(std::move(rapidjson::GenericStringRef< NmChar >("instance")),
                                         rapidjson::Value().SetString(lnName_.c_str(), rapidallocator), rapidallocator);

        // Convert Document to string
        rapidjson::StringBuffer buffer;
        rapidjson::Writer< rapidjson::StringBuffer > writer{buffer};
        std::ignore = document.Accept(writer);

        ara::core::String sendstr{buffer.GetString()};
        std::ignore = sendstr.append("\n");
        std::uint32_t serverCode{0U};
        std::int32_t const ret{ipcPtr_->DoIpcSyncRequest(sendstr, serverCode)};
        if (0 == ret) {
            return ara::core::Result< NetworkStateType >::FromValue(static_cast< NetworkStateType >(serverCode));
        }
    }
    return ara::core::Result< NetworkStateType >::FromError(NmErrc::kServiceNotAvailable);
}

/// @brief Set network request state
/// @param networkState Request state
/// @return Setting result
ara::core::Result< void > NetworkHandle::SetNetworkRequestedState(NetworkStateType const networkState) noexcept
{
#ifndef ARA_NM_NOCHECK_SM_PROCESS
    if (!SMCheck::CheckAccessPermission()) {
        return ara::core::Result< void >::FromError(NmErrc::kAccessWithoutPermission);
    }
#endif

    if (nullptr != ipcPtr_) {
        rapidjson::Document document;
        std::ignore = document.SetObject();
        rapidjson::Document::AllocatorType &rapidallocator{document.GetAllocator()};

        std::ignore = document.AddMember(std::move(rapidjson::GenericStringRef< NmChar >("cmd")),
                                         static_cast< std::int32_t >(internal::IpcCommand::kSetNetworkRequestState),
                                         document.GetAllocator());
        std::ignore = document.AddMember(std::move(rapidjson::GenericStringRef< NmChar >("instance")),
                                         rapidjson::Value().SetString(lnName_.c_str(), rapidallocator), rapidallocator);
        std::ignore = document.AddMember(std::move(rapidjson::GenericStringRef< NmChar >("requeststate")),
                                         static_cast< std::int32_t >(networkState), document.GetAllocator());

        // Convert Document to string
        rapidjson::StringBuffer buffer;
        rapidjson::Writer< rapidjson::StringBuffer > writer{buffer};
        std::ignore = document.Accept(writer);

        ara::core::String sendstr{buffer.GetString()};
        std::ignore = sendstr.append("\n");
        std::uint32_t serverCode{0U};
        std::int32_t const ret{ipcPtr_->DoIpcSyncRequest(sendstr, serverCode)};
        std::ignore = serverCode;
        if (0 == ret) {
            return ara::core::Result< void >::FromValue();
        }
    }
    return ara::core::Result< void >::FromError(NmErrc::kServiceNotAvailable);
}

/// @brief Register network state change notification
/// @param notifier Notification receiver
/// @return Registration result
ara::core::Result< void > NetworkHandle::RegisterNetworkStateChangeNotifier(
    NetworkStateChangeNotifier notifier) noexcept
{
#ifndef ARA_NM_NOCHECK_SM_PROCESS
    if (!SMCheck::CheckAccessPermission()) {
        return ara::core::Result< void >::FromError(NmErrc::kAccessWithoutPermission);
    }
#endif

    if (nullptr != ipcPtr_) {
        if (nullptr != notifier) {
            networkStateChangeNotifier_ = std::move(notifier);
            NmIpc::IpcChangeNotifier ipcNotifier{[this](std::uint32_t const state) mutable -> void {
                NetworkStateType const currentState{static_cast< NetworkStateType >(state)};
                this->networkStateChangeNotifier_(currentState);
            }};
            IpcLnNotifierPair const notifierPair{ipcNotifier, nullptr};
            std::int32_t const ret{ipcPtr_->RegisterNotification(
                lnName_, internal::IpcCommand::kRegisterNetworkStateChangeNotifier, notifierPair)};
            if (0 == ret) {
                return ara::core::Result< void >::FromValue();
            }
        }
    }
    return ara::core::Result< void >::FromError(NmErrc::kServiceNotAvailable);
}

/// @brief Register network state change notification
/// @param notifier Notification receiver
/// @param executor Executor
/// @return Registration result
template < typename ExecutorT >
ara::core::Result< void > NetworkHandle::RegisterNetworkStateChangeNotifier(NetworkStateChangeNotifier notifier,
                                                                            ExecutorT &&executor) noexcept
{
#ifndef ARA_NM_NOCHECK_SM_PROCESS
    if (!SMCheck::CheckAccessPermission()) {
        return ara::core::Result< void >::FromError(NmErrc::kAccessWithoutPermission);
    }
#endif

    if (nullptr != ipcPtr_) {
        if ((nullptr != notifier)) {
            networkStateChangeNotifier_ = std::move(notifier);
            NmIpc::IpcChangeNotifier ipcNotifier{[this](std::uint32_t const state) mutable -> void {
                NetworkStateType const currentState{static_cast< NetworkStateType >(state)};
                this->networkStateChangeNotifier_(currentState);
            }};
            NmIpc::IpcChangeNotifierExecutor executorWrapper{
                std::move([executor2 = std::forward< ExecutorT >(executor), this](
                              NmIpc::IpcChangeNotifier const &n, std::uint32_t const state) mutable -> void {
                    /// n is just a placeholder
                    std::ignore = n;
                    NetworkStateType const currentState{static_cast< NetworkStateType >(state)};
                    executor2(this->networkStateChangeNotifier_, currentState);
                })};

            IpcLnNotifierPair const notifierPair{ipcNotifier, executorWrapper};
            std::int32_t const ret{ipcPtr_->RegisterNotification(
                lnName_, internal::IpcCommand::kRegisterNetworkStateChangeNotifier, notifierPair)};
            if (0 == ret) {
                return ara::core::Result< void >::FromValue();
            }
        } else {
            NmLogger().LogError() << "RegisterNetworkStateChangeNotifier invalid parameter";
        }
    }
    return ara::core::Result< void >::FromError(NmErrc::kServiceNotAvailable);
}

/// @brief Unregister network state change notification
void NetworkHandle::UnregisterNetworkStateChangeNotifier() noexcept
{
    if (nullptr != ipcPtr_) {
        networkStateChangeNotifier_ = nullptr;
        std::int32_t const ret{
            ipcPtr_->UnregisterNotification(lnName_, internal::IpcCommand::kUnregisterNetworkStateChangeNotifier)};
        if (0 != ret) {
            NmLogger().LogError() << "UnregisterNetworkStateChangeNotifier ret" << ret;
        }
    }
}

/// @brief Register network request state change notification
/// @param notifier Notification receiver
/// @return Registration result
ara::core::Result< void > NetworkHandle::RegisterNetworkRequestedStateChangeNotifier(
    NetworkStateChangeNotifier notifier) noexcept
{
#ifndef ARA_NM_NOCHECK_SM_PROCESS
    if (!SMCheck::CheckAccessPermission()) {
        return ara::core::Result< void >::FromError(NmErrc::kAccessWithoutPermission);
    }
#endif

    if (nullptr != ipcPtr_) {
        networkReuestStateChangeNotifier_ = std::move(notifier);
        NmIpc::IpcChangeNotifier ipcNotifier{[this](std::uint32_t const state) mutable -> void {
            NetworkStateType const currentState{static_cast< NetworkStateType >(state)};
            this->networkReuestStateChangeNotifier_(currentState);
        }};
        IpcLnNotifierPair notifierPair{ipcNotifier, nullptr};
        std::int32_t const ret{ipcPtr_->RegisterNotification(
            lnName_, internal::IpcCommand::kRegisterNetworkRequestedStateChangeNotifier, notifierPair)};
        if (0 == ret) {
            return ara::core::Result< void >::FromValue();
        }
    }
    return ara::core::Result< void >::FromError(NmErrc::kServiceNotAvailable);
}

/// @brief Register network request state change notification
/// @param notifier Notification receiver
/// @param executor Executor
/// @return Registration result
template < typename ExecutorT >
ara::core::Result< void > NetworkHandle::RegisterNetworkRequestedStateChangeNotifier(
    NetworkStateChangeNotifier notifier, ExecutorT &&executor) noexcept
{
    if (nullptr != ipcPtr_) {
        if (nullptr != notifier) {
            /// ipcNotifier placeholder, actually uses notifier
            networkReuestStateChangeNotifier_ = std::move(notifier);
            NmIpc::IpcChangeNotifier ipcNotifier{[this](std::uint32_t const state) mutable -> void {
                NetworkStateType const currentState{static_cast< NetworkStateType >(state)};
                this->networkReuestStateChangeNotifier_(currentState);
            }};
            NmIpc::IpcChangeNotifierExecutor executorWrapper{
                std::move([executor2 = std::forward< ExecutorT >(executor), this](
                              NmIpc::IpcChangeNotifier const &n, std::uint32_t const state) mutable -> void {
                    /// n is just a placeholder
                    std::ignore = n;
                    NetworkStateType const currentState{static_cast< NetworkStateType >(state)};
                    executor2(this->networkReuestStateChangeNotifier_, currentState);
                })};
            IpcLnNotifierPair const notifierPair{ipcNotifier, executorWrapper};
            std::int32_t const ret{ipcPtr_->RegisterNotification(
                lnName_, internal::IpcCommand::kRegisterNetworkRequestedStateChangeNotifier, notifierPair)};
            if (0 == ret) {
                return ara::core::Result< void >::FromValue();
            }
        } else {
            NmLogger().LogError() << "RegisterNetworkRequestedStateChangeNotifier invalid parameter";
        }
    }
    return ara::core::Result< void >::FromError(NmErrc::kServiceNotAvailable);
}

/// @brief Unregister network request state change notification
void NetworkHandle::UnregisterNetworkRequestedStateChangeNotifier() noexcept
{
    if (nullptr != ipcPtr_) {
        std::int32_t const ret{ipcPtr_->UnregisterNotification(
            lnName_, internal::IpcCommand::kUnregisterNetworkRequestedStateChangeNotifier)};
        if (0 != ret) {
            NmLogger().LogError() << "UnregisterNetworkRequestedStateChangeNotifier ret" << ret;
        }
        networkReuestStateChangeNotifier_ = nullptr;
    }
}

}  // namespace nm
}  // namespace ara
