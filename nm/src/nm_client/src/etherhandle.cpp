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
/// @file       etherhandle.cpp
/// @brief      State machine instance user interface class
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
/// @unit_name=EtherHandle
/// @unit_description=State machine instance user interface class
/// @endcode
///
/// ================================================================

#include "include/public/etherhandle.h"

#include "include/internal/nmipc.h"
#include "include/public/nm_error_domain.h"
#include "rapidjson/document.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/writer.h"

namespace ara {
namespace nm {

/// @brief Constructor
/// @param ipAddr IP address
EtherHandle::EtherHandle(ara::core::String const &ipAddr) noexcept
{
    ipAddr_ = ipAddr;
    if (!ipAddr_.empty()) {
        ipcPtr_ = NmIpc::GetNmIpcProHandler();
    }
    stateChangeNotifier_ = nullptr;
    pnRequestNotifier_   = nullptr;
    pnReleaseNotifier_   = nullptr;
    nodeResultnotifier_  = nullptr;
}

/// @brief Destructor
EtherHandle::~EtherHandle() noexcept
{
    ipcPtr_ = nullptr;
    NmIpc::CloseNmIpcProHandler();
}

/// @brief Get state machine state
/// @return State machine state
ara::core::Result< SMState > EtherHandle::GetEtherStateMachineState() const noexcept
{
    if (nullptr != ipcPtr_) {
        rapidjson::Document document;
        std::ignore = document.SetObject();
        rapidjson::Document::AllocatorType &rapidallocator{document.GetAllocator()};

        std::ignore = document.AddMember(std::move(rapidjson::GenericStringRef< NmChar >("cmd")),
                                         static_cast< std::int32_t >(internal::IpcCommand::kGetEtherState),
                                         document.GetAllocator());
        std::ignore = document.AddMember(std::move(rapidjson::GenericStringRef< NmChar >("instance")),
                                         rapidjson::Value().SetString(ipAddr_.c_str(), rapidallocator), rapidallocator);

        // Convert Document to string
        rapidjson::StringBuffer buffer;
        rapidjson::Writer< rapidjson::StringBuffer > writer{buffer};
        std::ignore = document.Accept(writer);

        ara::core::String sendstr{buffer.GetString()};
        std::ignore = sendstr.append("\n");
        std::uint32_t serverCode{0U};
        std::int32_t const ret{ipcPtr_->DoIpcSyncRequest(sendstr, serverCode)};
        if ((static_cast< std::uint32_t >(SMState::kReadySleep) >= serverCode) && (0 == ret)) {
            return ara::core::Result< SMState >::FromValue(static_cast< SMState >(serverCode));
        }
    }
    return ara::core::Result< SMState >::FromError(NmErrc::kServiceNotAvailable);
}

/// @brief  Set state machine state
/// @param state State machine state
/// @return Setting result
ara::core::Result< void > EtherHandle::SetEtherStateMachineSate(SMState const state) noexcept
{
    if (nullptr != ipcPtr_) {
        rapidjson::Document document;
        std::ignore = document.SetObject();
        rapidjson::Document::AllocatorType &rapidallocator{document.GetAllocator()};

        std::ignore = document.AddMember(std::move(rapidjson::GenericStringRef< NmChar >("cmd")),
                                         static_cast< std::int32_t >(internal::IpcCommand::kSetEtherState),
                                         document.GetAllocator());
        std::ignore = document.AddMember(std::move(rapidjson::GenericStringRef< NmChar >("instance")),
                                         rapidjson::Value().SetString(ipAddr_.c_str(), rapidallocator), rapidallocator);
        std::ignore = document.AddMember(std::move(rapidjson::GenericStringRef< NmChar >("requeststate")),
                                         static_cast< std::int32_t >(state), document.GetAllocator());

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

/// @brief Request node detection
/// @param notifier Notifier
/// @return Request result
ara::core::Result< void > EtherHandle::RequestDetectNode(DetectNodeResultNotifier const &notifier) noexcept
{
    if (nullptr != ipcPtr_) {
        nodeResultnotifier_ = notifier;
        NmIpc::IpcEtherNotifierPair const ipcPair{nodeResultnotifier_, nullptr};
        std::int32_t ret{ipcPtr_->RegisterNotification(ipAddr_, internal::IpcCommand::kNotifyPresentNodeList, ipcPair)};
        if (0 == ret) {
            rapidjson::Document document;
            std::ignore = document.SetObject();
            rapidjson::Document::AllocatorType &rapidallocator{document.GetAllocator()};

            std::ignore = document.AddMember(std::move(rapidjson::GenericStringRef< NmChar >("cmd")),
                                             static_cast< std::int32_t >(internal::IpcCommand::kRequestDetectNode),
                                             document.GetAllocator());
            std::ignore
                = document.AddMember(std::move(rapidjson::GenericStringRef< NmChar >("instance")),
                                     rapidjson::Value().SetString(ipAddr_.c_str(), rapidallocator), rapidallocator);

            // Convert Document to string
            rapidjson::StringBuffer buffer;
            rapidjson::Writer< rapidjson::StringBuffer > writer{buffer};
            std::ignore = document.Accept(writer);

            ara::core::String sendstr{buffer.GetString()};
            std::ignore = sendstr.append("\n");
            std::uint32_t serverCode{0U};
            ret         = ipcPtr_->DoIpcSyncRequest(sendstr, serverCode);
            std::ignore = serverCode;
            if (0 == ret) {
                return ara::core::Result< void >::FromValue();
            }
        }
    }
    return ara::core::Result< void >::FromError(NmErrc::kServiceNotAvailable);
}

/// @brief Wakeup notification
/// @return Notification result
ara::core::Result< void > EtherHandle::NotifyWakeup() noexcept
{
    if (nullptr != ipcPtr_) {
        rapidjson::Document document;
        std::ignore = document.SetObject();
        rapidjson::Document::AllocatorType &rapidallocator{document.GetAllocator()};

        std::ignore = document.AddMember(std::move(rapidjson::GenericStringRef< NmChar >("cmd")),
                                         static_cast< std::int32_t >(internal::IpcCommand::kNotifyWakeup),
                                         document.GetAllocator());
        std::ignore = document.AddMember(std::move(rapidjson::GenericStringRef< NmChar >("instance")),
                                         rapidjson::Value().SetString(ipAddr_.c_str(), rapidallocator), rapidallocator);

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

/// @brief Set message control type
/// @param type Message control type
/// @return Setting result
ara::core::Result< void > EtherHandle::SetMessageCtrlType(MessageCtrlType const type) noexcept
{
    if (nullptr != ipcPtr_) {
        rapidjson::Document document;
        std::ignore = document.SetObject();
        rapidjson::Document::AllocatorType &rapidallocator{document.GetAllocator()};

        std::ignore = document.AddMember(std::move(rapidjson::GenericStringRef< NmChar >("cmd")),
                                         static_cast< std::int32_t >(internal::IpcCommand::kSetMessageCtrlType),
                                         document.GetAllocator());
        std::ignore = document.AddMember(std::move(rapidjson::GenericStringRef< NmChar >("instance")),
                                         rapidjson::Value().SetString(ipAddr_.c_str(), rapidallocator), rapidallocator);
        std::ignore = document.AddMember(std::move(rapidjson::GenericStringRef< NmChar >("requesttype")),
                                         static_cast< std::int32_t >(type), document.GetAllocator());
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

/// @brief Get message control type
/// @return Message control type
ara::core::Result< MessageCtrlType > EtherHandle::GetMessageCtrlType() noexcept
{
    if (nullptr != ipcPtr_) {
        rapidjson::Document document;
        std::ignore = document.SetObject();
        rapidjson::Document::AllocatorType &rapidallocator{document.GetAllocator()};

        std::ignore = document.AddMember(std::move(rapidjson::GenericStringRef< NmChar >("cmd")),
                                         static_cast< std::int32_t >(internal::IpcCommand::kGetMessageCtrlType),
                                         document.GetAllocator());
        std::ignore = document.AddMember(std::move(rapidjson::GenericStringRef< NmChar >("instance")),
                                         rapidjson::Value().SetString(ipAddr_.c_str(), rapidallocator), rapidallocator);

        // Convert Document to string
        rapidjson::StringBuffer buffer;
        rapidjson::Writer< rapidjson::StringBuffer > writer{buffer};
        std::ignore = document.Accept(writer);

        ara::core::String sendstr{buffer.GetString()};
        std::ignore = sendstr.append("\n");
        std::uint32_t serverCode{0U};
        std::int32_t const ret{ipcPtr_->DoIpcSyncRequest(sendstr, serverCode)};
        if ((static_cast< std::uint32_t >(MessageCtrlType::kDisableAll) >= serverCode) && (0 == ret)) {
            return ara::core::Result< MessageCtrlType >::FromValue(static_cast< MessageCtrlType >(serverCode));
        }
    }
    return ara::core::Result< MessageCtrlType >::FromError(NmErrc::kServiceNotAvailable);
}

/// @brief Get initialization status
/// @return Initialization status
ara::core::Result< std::uint32_t > EtherHandle::GetInitState() noexcept
{
    if (nullptr != ipcPtr_) {
        rapidjson::Document document;
        std::ignore = document.SetObject();
        rapidjson::Document::AllocatorType &rapidallocator{document.GetAllocator()};

        std::ignore = document.AddMember(std::move(rapidjson::GenericStringRef< NmChar >("cmd")),
                                         static_cast< std::int32_t >(internal::IpcCommand::kGetInitState),
                                         document.GetAllocator());
        std::ignore = document.AddMember(std::move(rapidjson::GenericStringRef< NmChar >("instance")),
                                         rapidjson::Value().SetString(ipAddr_.c_str(), rapidallocator), rapidallocator);

        // Convert Document to string
        rapidjson::StringBuffer buffer;
        rapidjson::Writer< rapidjson::StringBuffer > writer{buffer};
        std::ignore = document.Accept(writer);

        ara::core::String sendstr{buffer.GetString()};
        std::ignore = sendstr.append("\n");
        std::uint32_t serverCode{0U};
        std::int32_t const ret{ipcPtr_->DoIpcSyncRequest(sendstr, serverCode)};
        if (0 == ret) {
            return ara::core::Result< std::uint32_t >::FromValue(serverCode);
        }
    }
    return ara::core::Result< std::uint32_t >::FromError(NmErrc::kServiceNotAvailable);
}

/// @brief Get external request PN list
/// @return External request PN list
ara::core::Result< ara::core::String > EtherHandle::GetExternalPnRequestList() noexcept
{
    if (nullptr != ipcPtr_) {
        rapidjson::Document document;
        std::ignore = document.SetObject();
        rapidjson::Document::AllocatorType &rapidallocator{document.GetAllocator()};

        std::ignore = document.AddMember(std::move(rapidjson::GenericStringRef< NmChar >("cmd")),
                                         static_cast< std::int32_t >(internal::IpcCommand::kGetExternalPnRequestList),
                                         document.GetAllocator());
        std::ignore = document.AddMember(std::move(rapidjson::GenericStringRef< NmChar >("instance")),
                                         rapidjson::Value().SetString(ipAddr_.c_str(), rapidallocator), rapidallocator);

        // Convert Document to string
        rapidjson::StringBuffer buffer;
        rapidjson::Writer< rapidjson::StringBuffer > writer{buffer};
        std::ignore = document.Accept(writer);

        ara::core::String sendstr{buffer.GetString()};
        std::ignore = sendstr.append("\n");
        ara::core::String pnList;
        std::int32_t const ret{ipcPtr_->DoIpcSyncRequest(sendstr, pnList)};
        if (0 == ret) {
            return ara::core::Result< ara::core::String >::FromValue(ara::core::String{pnList.c_str()});
        }
    }
    return ara::core::Result< ara::core::String >::FromError(NmErrc::kServiceNotAvailable);
}

/// @brief Register state machine state change notifier
/// @param notifier Notifier
/// @return Whether registration succeeded
ara::core::Result< void > EtherHandle::RegisterEtherStateChangeNotifier(
    EtherStateChangeNotifier const &notifier) noexcept
{
    if (nullptr != ipcPtr_) {
        stateChangeNotifier_ = notifier;
        NmIpc::EtherNotifier ipcCall{[notifier](ara::core::String const &content) mutable -> void {
            SMState const state{static_cast< SMState >(std::stoul(content.c_str()))};
            notifier(state);
        }};
        NmIpc::IpcEtherNotifierPair const ipcPair{ipcCall, nullptr};
        std::int32_t const ret{
            ipcPtr_->RegisterNotification(ipAddr_, internal::IpcCommand::kRegisterEtherStateChanged, ipcPair)};
        if (0 == ret) {
            return ara::core::Result< void >::FromValue();
        }
    }
    return ara::core::Result< void >::FromError(NmErrc::kServiceNotAvailable);
}

/// @brief Unregister state machine state change notifier
void EtherHandle::UnregisterEtherStateChangeNotifier() noexcept
{
    if (nullptr != ipcPtr_) {
        stateChangeNotifier_ = nullptr;
        std::int32_t const ret{
            ipcPtr_->UnregisterNotification(ipAddr_, internal::IpcCommand::kUnregisterEtherStateChanged)};
        if (0 != ret) {
            NmLogger().LogError() << "UnregisterEtherStateChangeNotifier ret" << ret;
        }
    }
}

/// @brief Register external PN request notifier
/// @param notifier Notifier
/// @return Whether registration succeeded
ara::core::Result< void > EtherHandle::RegisterExternalPnRequestNotifier(ExternalPnNotifier const &notifier) noexcept
{
    if (nullptr != ipcPtr_) {
        pnRequestNotifier_ = notifier;
        NmIpc::IpcEtherNotifierPair const ipcPair{notifier, nullptr};
        std::int32_t const ret{
            ipcPtr_->RegisterNotification(ipAddr_, internal::IpcCommand::kRegisterExternalPnRequest, ipcPair)};
        if (0 == ret) {
            return ara::core::Result< void >::FromValue();
        }
    }
    return ara::core::Result< void >::FromError(NmErrc::kServiceNotAvailable);
}

/// @brief Unregister external PN request notifier
void EtherHandle::UnregisterExternalPnRequestNotifier() noexcept
{
    if (nullptr != ipcPtr_) {
        pnRequestNotifier_ = nullptr;
        std::int32_t const ret{
            ipcPtr_->UnregisterNotification(ipAddr_, internal::IpcCommand::kUnregisterExternalPnRequest)};
        if (0 != ret) {
            NmLogger().LogError() << "UnregisterExternalPnRequestNotifier ret" << ret;
        }
    }
}

/// @brief Register external PN release notifier
/// @param notifier Notifier
/// @return Whether registration succeeded
ara::core::Result< void > EtherHandle::RegisterExternalPnReleaseNotifier(ExternalPnNotifier const &notifier) noexcept
{
    if (nullptr != ipcPtr_) {
        pnReleaseNotifier_ = notifier;
        NmIpc::IpcEtherNotifierPair const ipcPair{notifier, nullptr};
        std::int32_t const ret{
            ipcPtr_->RegisterNotification(ipAddr_, internal::IpcCommand::kRegisterExternalPnRelease, ipcPair)};
        if (0 == ret) {
            return ara::core::Result< void >::FromValue();
        }
    }
    return ara::core::Result< void >::FromError(NmErrc::kServiceNotAvailable);
}

/// @brief Unregister external PN release notifier
void EtherHandle::UnregisterExternalPnReleaseNotifier() noexcept
{
    if (nullptr != ipcPtr_) {
        pnReleaseNotifier_ = nullptr;
        std::int32_t const ret{
            ipcPtr_->UnregisterNotification(ipAddr_, internal::IpcCommand::kUnregisterExternalPnRelease)};
        if (0 != ret) {
            NmLogger().LogError() << "UnregisterExternalPnReleaseNotifier ret" << ret;
        }
    }
}

}  // namespace nm
}  // namespace ara