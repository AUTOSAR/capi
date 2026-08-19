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
/// @file       npc_method.h
/// @brief      Binding layer service skeleton method header file
/// @details
/// @date       2023-10-07
/// @author     mazelin
/// @version    1.2.0
///
/// ================================================================

#ifndef __COM_NPC_SKELETON_METHOD_H
#define __COM_NPC_SKELETON_METHOD_H

#include "../npc_runtime.h"
#include "ara/com/internal/skeleton/method.h"

/// @brief Namespace -- internal binding layer skeleton
namespace ara {
namespace com {
namespace internal {
namespace npc {
namespace skeleton {
/// @brief Template type -- binding layer service skeleton method
/// @code{.isoft}
/// export_level=/COM/SoC/SOA
/// @endcode
/// @tparam Owner Service element owner
/// @tparam Desc Description info
/// @tparam Callback Callback method signature
template < typename Owner, typename Desc, typename Callback >
class NpcMethod
{
public:
    /// @brief Constructor
    /// @param[in] owner Service element owner
    /// @param[in] callback Callback method getter
    NpcMethod(Owner& owner, std::function< Callback() >&& callback) noexcept
        : owner_{owner}, callback_{std::move(callback)}
    {
        static_assert(Message::IsMethodId(Desc::methodId()), "invalid method id");
        ComLogTrace("create bind skeleton method", GenArg(Desc::methodId()));
        auto dataId{ara::com::e2exf::StatusHandler::GetDataId(Desc::serviceId(), owner_.instanceId_, Desc::methodId())};
        if (ara::com::e2exf::Transformer::E2E_IsProtected(dataId)) {
            e2eInfo_ = {dataId};
        }
        ComLogTrace("create bind skeleton method done", GenArg(Desc::methodId()));
    }
    /// @brief Destructor
    ~NpcMethod() noexcept { ComLogTrace("destroy bind skeleton method", GenArg(Desc::methodId())); }
    /// @brief Copy constructor
    /// @param other
    NpcMethod(NpcMethod const& other) noexcept = default;
    /// @brief Move constructor
    /// @param other
    NpcMethod(NpcMethod&& other) noexcept = default;
    /// @brief Copy assignment operator
    /// @param other
    /// @return NpcMethod
    NpcMethod& operator=(NpcMethod const& other) noexcept = default;
    /// @brief Move assignment operator
    /// @param other
    /// @return NpcMethod
    NpcMethod& operator=(NpcMethod&& other) noexcept = default;
    /// @brief Register method receiver -- call before offering service
    /// @return Result object -- empty/value or error
    ara::core::Result< void > RegisterReceiver() noexcept
    {
        using Result = ara::core::Result< void >;
        ComLogTrace("register bind method receiver", GenArg(Desc::methodId()));
        auto& runtime{runtime::GetInstance()};
        auto ret{npc_appcb_set_message(runtime.GetAppCB(), Desc::serviceId(), owner_.instanceId_, Desc::methodId(),
                                       NpcMethod::OnRequest, this)};
        if (ret < 0) {
            ComLogError("register bind method receiver error: invoke stack register request callback failed",
                        GenArg(ret), GenArg(Desc::methodId()));
            return Result{MakeErrorCode(ComErrc::kCommunicationStackError, ret)};
        }
        ComLogDebug("register bind method receiver done", GenArg(Desc::methodId()));
        return Result{};
    }
    /// @brief Unregister method receiver -- call after stopping service offering
    /// @return Result object -- empty/value or error
    ara::core::Result< void > UnregisterReceiver() noexcept
    {
        using Result = ara::core::Result< void >;
        ComLogTrace("unregister bind method receiver", GenArg(Desc::methodId()));
        auto& runtime{runtime::GetInstance()};
        auto ret{npc_appcb_set_message(runtime.GetAppCB(), Desc::serviceId(), owner_.instanceId_, Desc::methodId(),
                                       nullptr, nullptr)};
        if (ret < 0) {
            ComLogError("unregister bind method receiver error: invoke stack unregister request callback failed",
                        GenArg(ret), GenArg(Desc::methodId()));
            return Result{MakeErrorCode(ComErrc::kCommunicationStackError, ret)};
        }
        ComLogDebug("unregister bind method receiver done", GenArg(Desc::methodId()));
        return Result{};
    }

private:
    /// @brief Callback method when a call request message is received
    /// @param[in] a Implementation layer application
    /// @param[in] ud User data
    /// @param[in] m Message
    /// @return Result -- 0: success; !0: failure;
    static int32_t OnRequest(npc_app_t* a, void* ud, npc_message_t* m) noexcept
    {
        TimeRecorder(TimeNM::Timer, "NPCOnRequest");
        std::ignore = a;
        ComLogDebug("on bind method request", GenArg(Desc::methodId()), GenArg(m));
        auto* thisObj{reinterpret_cast< NpcMethod* >(ud)};
        return thisObj->_handleRequest(std::shared_ptr< npc_message_t >(m, npc_message_close));
    }
    /// @brief Handle call request message
    /// @param[in] request Request message
    /// @return Result -- 0: success; !0: failure;
    int32_t _handleRequest(std::shared_ptr< npc_message_t > const& request) noexcept
    {
        TimeRecorder(TimeNM::Timer, "NPCHandleRequest(mode)");
        switch ((**owner_.skeleton_).GetMethodCallProcessingMode()) {
            case MethodCallProcessingMode::kPoll: {
                std::lock_guard< std::mutex > lock(owner_.callsMutex_);
                owner_.pendingCalls_.emplace_back([this, request]() { std::ignore = _handleRequest(request.get()); });
            } break;
            case MethodCallProcessingMode::kEvent: {
                auto& runtime{internal::GetInstance()};
                auto threadPool{runtime.GetThreadPool(Desc::serviceIdentifier.ToString()).get()};
                if (threadPool == nullptr) {
                    threadPool = runtime.GetThreadPool(runtime.GetProcessName()).get();
                }
                if (threadPool != nullptr && threadPool->Size() > 1) {
                    threadPool->Dispatch([this, request]() { std::ignore = _handleRequest(request.get()); });
                } else {
                    std::thread([this, request]() { std::ignore = _handleRequest(request.get()); }).detach();
                }
            } break;
            case MethodCallProcessingMode::kEventSingleThread: {
                auto& runtime{internal::GetInstance()};
                auto threadPool{runtime.GetThreadPool(Desc::serviceIdentifier.ToString()).get()};
                if (threadPool == nullptr) {
                    threadPool = runtime.GetThreadPool(runtime.GetProcessName()).get();
                }
                if (threadPool != nullptr && threadPool->Size() <= 1) {
                    threadPool->Dispatch([this, request]() { std::ignore = _handleRequest(request.get()); });
                } else {
                    return _handleRequest(request.get());
                }
            } break;
        }
        return 0;
    }
    /// @brief Handle call request message
    /// @param[in] request Request message
    /// @return Result -- 0: success; !0: failure;
    int32_t _handleRequest(npc_message_t* request) noexcept
    {
        TimeRecorder(TimeNM::Timer, "NPCHandleRequest");
        TimeStart(TimeNM::Timer, "NPCHandleRequest(GetCaller)");
        auto callback{callback_()};
        TimeStop(TimeNM::Timer, "NPCHandleRequest(GetCaller)");
        NPC_ASSERT(!!callback, "handle bind method request exception: get caller failed", GenArg(Desc::serviceId()),
                   GenArg(Desc::methodId()), GenArg(request));
        return _handleRequest(std::move(callback), request);
    }
    /// @brief Handle call request message
    /// @tparam R Method return type
    /// @tparam Args Method parameter list type
    /// @param[in] callback Callback method
    /// @param[in] request Request message
    /// @return Result -- 0: success; !0: failure;
    template < typename R, typename... Args >
    int32_t _handleRequest(std::function< R(Args...) >&& callback, npc_message_t* request) noexcept
    {
        TimeRecorder(TimeNM::Timer, "NPCHandleRequest(forward)");
        return _handleRequest(std::move(callback), request, std::index_sequence_for< Args... >{});
    }
    /// @brief Handle call request message
    /// @tparam Args Method parameter list type
    /// @tparam kI Method parameter index list
    /// @param[in] callback Callback method
    /// @param[in] request Request message
    /// @return Result -- 0: success; !0: failure;
    template < typename... Args, size_t... kI >
    int32_t _handleRequest(std::function< void(Args...) >&& callback,
                           npc_message_t* request,
                           std::index_sequence< kI... >) noexcept
    {
        TimeRecorder(TimeNM::Timer, "NPCHandleRequest(void)");
        ComLogTrace("handle bind method request", GenArg(Desc::methodId()), GenArg(request));
        TimeStart(TimeNM::Timer, "NPCHandleRequest::Verify&SecOC");
        int32_t ret{};
        /// [TR_SOMEIP_00187] If the first 2 bits of the return code are reserved, then verify the last 6 bits
        auto rcode{request->hdr.code};
        if ((rcode & Message::kReturnCodeReservedMask) == Message::kReturnCodeReservedMask) {
            rcode &= Message::kReturnCodeValidMask;
        }
        /// [TR_SOMEIP_00539] If the return code is invalid (0x01-0x1F), then ignore the request
        if (rcode >= Message::kReturnCodeNotOk && rcode <= Message::kReturnCodeReservedMax4Generic) {
            ret = -__LINE__;
            ComLogWarning("handle bind method request rejected: verify return code invalid", GenArg(rcode),
                          GenArg(Desc::methodId()), GenArg(request));
            MonitorTrace(runtime::GetInstance(), trace::userdefine::kRecvRequest, trace::ServiceType::kSkeleton,
                         Desc::serviceId(), request->inst, Desc::serviceIdentifier, Desc::methodId(), "methodName",
                         false, request->hdr.session, request->hdr.type,
                         static_cast< uint32_t >(ComErrc::kCommunicationStackError), trace::FormatArgs(""));
            return ret;
        }
        rcode = runtime::VerifyRequest< Desc >(*request, false);
        if (rcode != Message::kReturnCodeOk) {
            ret = -__LINE__;
            ComLogWarning("handle bind method request rejected: verify request invalid", GenArg(rcode),
                          GenArg(Desc::methodId()), GenArg(request));
            MonitorTrace(runtime::GetInstance(), trace::userdefine::kRecvRequest, trace::ServiceType::kSkeleton,
                         Desc::serviceId(), request->inst, Desc::serviceIdentifier, Desc::methodId(), "methodName",
                         false, request->hdr.session, request->hdr.type,
                         static_cast< uint32_t >(ComErrc::kCommunicationStackError), trace::FormatArgs(""));
            return ret;
        }
#ifdef HAS_COM_SECOC
        // secoc rx process
        auto secOcDataId{secoc::SecOC4SOMEIP::GetSecOCDataId(request->hdr.serv, request->inst, request->hdr.method,
                                                             request->hdr.type)};
        if (secOcDataId > 0) {
            ret = secoc::SecOC4SOMEIP::SecOCRxHandle(secOcDataId, request);
            if (ret < 0) {
                ComLogWarning("invoke SecOC Rx handle", GenArg(ret));
                MonitorTrace(runtime::GetInstance(), trace::userdefine::kRecvRequest, trace::ServiceType::kSkeleton,
                             Desc::serviceId(), request->inst, Desc::serviceIdentifier, Desc::methodId(), "methodName",
                             false, request->hdr.session, request->hdr.type,
                             static_cast< uint32_t >(ComErrc::kCommunicationStackError), trace::FormatArgs(""));
                return ret;
            }
            ComLogDebug("invoke SecOC Rx handle", GenArg(ret));
        }
#else
#endif
        MonitorTrace(runtime::GetInstance(), trace::userdefine::kRecvRequest, trace::ServiceType::kSkeleton,
                     Desc::serviceId(), request->inst, Desc::serviceIdentifier, Desc::methodId(), "methodName", false,
                     request->hdr.session, request->hdr.type, 0U, trace::FormatArgs(""));
        TimeStop(TimeNM::Timer, "NPCHandleRequest::Verify&SecOC");
        TimeStart(TimeNM::Timer, "NPCHandleRequest::MakeArgs");
        std::tuple< std::remove_cv_t< std::remove_reference_t< Args > >... > args;
        if (e2eInfo_) {  // enable e2e
            serialize::E2EPayload payload{*request, e2eInfo_->dataId};
            e2e::MessageCounter messageCounter{};
            e2e::E2EErrorCode e2eErrorCode{e2e::E2EErrc::kNoNewData};
            e2e::DataID dataID{e2eInfo_->dataId[0]};
            ret = runtime::Deserialize< Desc >(payload, std::get< kI >(args)...);
            if (ret < 0 || !payload.r) {
                ComLogWarning("handle bind method request rejected: deserialize E2E request failed", GenArg(ret),
                              GenArg(Desc::methodId()), GenArg(request));
                MonitorTrace(runtime::GetInstance(), trace::userdefine::kDeserialize, trace::ServiceType::kSkeleton,
                             Desc::serviceId(), request->inst, Desc::serviceIdentifier, Desc::methodId(), "methodName",
                             false, request->hdr.session, request->hdr.type,
                             static_cast< uint32_t >(ComErrc::kCommunicationStackError), trace::FormatArgs(""));
                _handleE2EError(dataID, e2eErrorCode, messageCounter);
                return ret;
            }
            auto status{payload.r.Value().GetProfileCheckStatus()};
            messageCounter = payload.r.Value().GetCounter();
            if (status != e2e::ProfileCheckStatus::kOk) {
                if (status != e2e::ProfileCheckStatus::kNoNewData && status != e2e::ProfileCheckStatus::kNotAvailable) {
                    e2eErrorCode = ProfileCheckStatusToErrorCode(status);
                }
                ComLogWarning("handle bind method request rejected: check E2E request failed", GenArg(status),
                              GenArg(Desc::methodId()), GenArg(request));
                MonitorTrace(runtime::GetInstance(), trace::userdefine::kDeserialize, trace::ServiceType::kSkeleton,
                             Desc::serviceId(), request->inst, Desc::serviceIdentifier, Desc::methodId(), "methodName",
                             false, request->hdr.session, request->hdr.type,
                             static_cast< uint32_t >(ComErrc::kCommunicationStackError), trace::FormatArgs(""));
                _handleE2EError(dataID, e2eErrorCode, messageCounter);
                return 0;
            }
        } else {
            ret = runtime::Deserialize< Desc >(*request, std::get< kI >(args)...);
            if (ret < 0) {
                ComLogWarning("handle bind method request rejected: deserialize request failed", GenArg(ret),
                              GenArg(Desc::methodId()), GenArg(request));
                MonitorTrace(runtime::GetInstance(), trace::userdefine::kDeserialize, trace::ServiceType::kSkeleton,
                             Desc::serviceId(), request->inst, Desc::serviceIdentifier, Desc::methodId(), "methodName",
                             false, request->hdr.session, request->hdr.type,
                             static_cast< uint32_t >(ComErrc::kCommunicationStackError), trace::FormatArgs(""));
                return ret;
            }
        }
        MonitorTrace(runtime::GetInstance(), trace::userdefine::kDeserialize, trace::ServiceType::kSkeleton,
                     Desc::serviceId(), request->inst, Desc::serviceIdentifier, Desc::methodId(), "methodName", false,
                     request->hdr.session, request->hdr.type, 0U, trace::FormatArgs(std::get< kI >(args)...));
        TimeStop(TimeNM::Timer, "NPCHandleRequest::MakeArgs");
        TimeStart(TimeNM::Timer, "NPCHandleCall");
        ComLogDebug("handle bind method request: invoke callback handle bind method call", GenArg(Desc::methodId()),
                    GenArg(request), GenArg(std::get< kI >(args)...));
        callback(std::get< kI >(args)...);
        ComLogTrace("handle bind method request: invoke callback handle bind method call done",
                    GenArg(Desc::methodId()), GenArg(request), GenArg(std::get< kI >(args)...));
        TimeStop(TimeNM::Timer, "NPCHandleCall");
        return 0;
    }
    /// @brief Handle call request message
    /// @tparam R Method return type
    /// @tparam Args Method parameter list type
    /// @tparam kI Method parameter index list
    /// @param[in] callback Callback method
    /// @param[in] request Request message
    /// @return Result -- 0: success; !0: failure;
    template < typename R, typename... Args, size_t... kI >
    int32_t _handleRequest(std::function< ara::core::Future< R >(Args...) >&& callback,
                           npc_message_t* request,
                           std::index_sequence< kI... >) noexcept
    {
        TimeRecorder(TimeNM::Timer, "NPCHandleRequest(value)");
        using Result = ara::core::Result< R >;
        ComLogTrace("handle bind method request", GenArg(Desc::methodId()), GenArg(request));
        TimeStart(TimeNM::Timer, "NPCHandleRequest::Verify&SecOC");
        int32_t ret{};
        auto rcode{request->hdr.code};
        /// [TR_SOMEIP_00187] If the first 2 bits of the return code are reserved, then verify the last 6 bits
        if ((rcode & Message::kReturnCodeReservedMask) == Message::kReturnCodeReservedMask) {
            rcode &= Message::kReturnCodeValidMask;
        }
        /// [TR_SOMEIP_00539] If the return code is invalid (0x01-0x1F), then ignore the request
        if (rcode != Message::kReturnCodeOk) {
            ret = -__LINE__;
            ComLogWarning("handle bind method request rejected: verify return code invalid", GenArg(rcode),
                          GenArg(Desc::methodId()), GenArg(request));
            MonitorTrace(runtime::GetInstance(), trace::userdefine::kRecvRequest, trace::ServiceType::kSkeleton,
                         Desc::serviceId(), request->inst, Desc::serviceIdentifier, Desc::methodId(), "methodName",
                         false, request->hdr.session, request->hdr.type,
                         static_cast< uint32_t >(ComErrc::kCommunicationStackError), trace::FormatArgs(""));
            return ret;
        }
        rcode = runtime::VerifyRequest< Desc >(*request, true);
        if (rcode != Message::kReturnCodeOk) {
            ComLogWarning("handle bind method request: call bind method error: verify request invalid", GenArg(rcode),
                          GenArg(Desc::methodId()), GenArg(request));
            MonitorTrace(runtime::GetInstance(), trace::userdefine::kRecvRequest, trace::ServiceType::kSkeleton,
                         Desc::serviceId(), request->inst, Desc::serviceIdentifier, Desc::methodId(), "methodName",
                         false, request->hdr.session, request->hdr.type,
                         static_cast< uint32_t >(ComErrc::kCommunicationStackError), trace::FormatArgs(""));
            return _sendResponse(request, rcode, Result{MakeErrorCode(ComErrc::kGrantEnforcementError, rcode)});
        }
#ifdef HAS_COM_SECOC
        // secoc rx process
        auto secOcDataId{secoc::SecOC4SOMEIP::GetSecOCDataId(request->hdr.serv, request->inst, request->hdr.method,
                                                             request->hdr.type)};
        if (secOcDataId > 0) {
            ret = secoc::SecOC4SOMEIP::SecOCRxHandle(secOcDataId, request);
            if (ret < 0) {
                ComLogWarning("invoke SecOC Rx handle", GenArg(ret));
                MonitorTrace(runtime::GetInstance(), trace::userdefine::kRecvRequest, trace::ServiceType::kSkeleton,
                             Desc::serviceId(), request->inst, Desc::serviceIdentifier, Desc::methodId(), "methodName",
                             false, request->hdr.session, request->hdr.type,
                             static_cast< uint32_t >(ComErrc::kCommunicationStackError), trace::FormatArgs(""));
                rcode = Message::kReturnCodeNotOk;
                return _sendResponse(request, rcode, Result{MakeErrorCode(ComErrc::kGrantEnforcementError, rcode)});
            }
            ComLogDebug("invoke SecOC Rx handle", GenArg(ret));
        }
#else
#endif
        MonitorTrace(runtime::GetInstance(), trace::userdefine::kRecvRequest, trace::ServiceType::kSkeleton,
                     Desc::serviceId(), request->inst, Desc::serviceIdentifier, Desc::methodId(), "methodName", false,
                     request->hdr.session, request->hdr.type, 0U, trace::FormatArgs(""));
        TimeStop(TimeNM::Timer, "NPCHandleRequest::Verify&SecOC");
        TimeStart(TimeNM::Timer, "NPCHandleRequest::MakeArgs");
        std::tuple< std::remove_cv_t< std::remove_reference_t< Args > >... > args;
        if (e2eInfo_) {  // enable e2e
            serialize::E2EPayload payload{*request, e2eInfo_->dataId};
            e2e::MessageCounter messageCounter{};
            e2e::E2EErrorCode e2eErrorCode{e2e::E2EErrc::kNotAvailable};
            e2e::DataID dataID{e2eInfo_->dataId[0]};
            ret = runtime::Deserialize< Desc >(payload, std::get< kI >(args)...);
            if (ret < 0 || !payload.r) {
                ComLogWarning("handle bind method request: call bind method error: deserialize E2E request failed",
                              GenArg(ret), GenArg(Desc::methodId()), GenArg(request));
                MonitorTrace(runtime::GetInstance(), trace::userdefine::kDeserialize, trace::ServiceType::kSkeleton,
                             Desc::serviceId(), request->inst, Desc::serviceIdentifier, Desc::methodId(), "methodName",
                             false, request->hdr.session, request->hdr.type,
                             static_cast< uint32_t >(ComErrc::kCommunicationStackError), trace::FormatArgs(""));
                _handleE2EError(dataID, e2eErrorCode, messageCounter);
                rcode = Message::kReturnCodeE2ERepeated;
                return _sendResponse(request, rcode, Result{MakeErrorCode(e2eErrorCode, rcode)});
            }
            auto status{payload.r.Value().GetProfileCheckStatus()};
            messageCounter = payload.r.Value().GetCounter();
            if (status != e2e::ProfileCheckStatus::kOk) {
                if (status != e2e::ProfileCheckStatus::kNoNewData && status != e2e::ProfileCheckStatus::kNotAvailable) {
                    e2eErrorCode = ProfileCheckStatusToErrorCode(status);
                }
                ComLogWarning("handle bind method request: call bind method error: check E2E request failed",
                              GenArg(status), GenArg(Desc::methodId()), GenArg(request));
                _handleE2EError(dataID, e2eErrorCode, messageCounter);
                rcode = Message::kReturnCodeE2ERepeated;
                MonitorTrace(runtime::GetInstance(), trace::userdefine::kDeserialize, trace::ServiceType::kSkeleton,
                             Desc::serviceId(), request->inst, Desc::serviceIdentifier, Desc::methodId(), "methodName",
                             false, request->hdr.session, request->hdr.type,
                             static_cast< uint32_t >(ComErrc::kCommunicationStackError), trace::FormatArgs(""));
                return _sendResponse(request, rcode, Result{MakeErrorCode(e2eErrorCode, rcode)});
            }
        } else {
            ret = runtime::Deserialize< Desc >(*request, std::get< kI >(args)...);
            if (ret < 0) {
                ComLogWarning("handle bind method request: call bind method error: deserialize request failed",
                              GenArg(ret), GenArg(Desc::methodId()), GenArg(request));
                rcode = Message::kReturnCodeMalformedMessage;
                MonitorTrace(runtime::GetInstance(), trace::userdefine::kDeserialize, trace::ServiceType::kSkeleton,
                             Desc::serviceId(), request->inst, Desc::serviceIdentifier, Desc::methodId(), "methodName",
                             false, request->hdr.session, request->hdr.type,
                             static_cast< uint32_t >(ComErrc::kCommunicationStackError), trace::FormatArgs(""));
                return _sendResponse(request, rcode, Result{MakeErrorCode(ComErrc::kGrantEnforcementError, rcode)});
            }
        }
        MonitorTrace(runtime::GetInstance(), trace::userdefine::kDeserialize, trace::ServiceType::kSkeleton,
                     Desc::serviceId(), request->inst, Desc::serviceIdentifier, Desc::methodId(), "methodName", false,
                     request->hdr.session, request->hdr.type, 0U, trace::FormatArgs(std::get< kI >(args)...));
        TimeStop(TimeNM::Timer, "NPCHandleRequest::MakeArgs");
        TimeStart(TimeNM::Timer, "NPCHandleCall");
        ComLogDebug("handle bind method request: invoke callback handle bind method call", GenArg(Desc::methodId()));
        auto future{callback(std::get< kI >(args)...)};
        // NPC_ASSERT(future.is_ready(), "invalid future");
        ComLogTrace("handle bind method request: invoke callback handle bind method call done",
                    GenArg(Desc::methodId()));
        auto result{std::move(future).GetResult()};
        if (!result) {
            rcode = Message::kReturnCodeMalformedMessage;
            bool violation{true};
            auto error{result.Error()};
            for (auto possibleError : Desc::possibleErrors()) {
                if (error == possibleError) {
                    violation = false;
                }
            }
            if (violation) {
                /// [SWS_CM_10447] Do not respond to unmodeled errors
                ret = -__LINE__;
                ComLogWarning("handle bind method request: call bind method discarded: application error unmodelled",
                              GenArg(result), GenArg(Desc::methodId()), GenArg(request));
                MonitorTrace(runtime::GetInstance(), trace::userdefine::kDeserialize, trace::ServiceType::kSkeleton,
                             Desc::serviceId(), request->inst, Desc::serviceIdentifier, Desc::methodId(), "methodName",
                             false, request->hdr.session, request->hdr.type,
                             static_cast< uint32_t >(ComErrc::kCommunicationStackError), trace::FormatArgs(""));
                return ret;
            }
        }
        TimeStop(TimeNM::Timer, "NPCHandleCall");
        return _sendResponse(request, rcode, result);
    }
    /// @brief Send call response message
    /// @param[in] request Call response message
    /// @param[in] rcode Error code
    /// @param[in] result Response result
    /// @return Result -- 0: success; !0: failure;
    template < typename T >
    int32_t _sendResponse(npc_message_t* request, uint8_t rcode, ara::core::Result< T > const& result) const noexcept
    {
        TimeRecorder(TimeNM::Timer, "NPCSendResponse");
        ComLogTrace("send bind method response", GenArg(Desc::methodId()), GenArg(request), GenArg(rcode),
                    GenArg(result));
        int32_t ret{};
        auto& runtime{runtime::GetInstance()};
        request->hdr.code = rcode;  // E2E serialization response payload requires rcode
        auto response{runtime::CreateResponse< Desc >(runtime.GetApp(), request, result)};
        if (response == nullptr) {
            ret = -__LINE__;
            ComLogError("send bind method response error: create response failed", GenArg(Desc::methodId()),
                        GenArg(request), GenArg(rcode), GenArg(result));
            MonitorTrace(runtime::GetInstance(), trace::userdefine::kSerialize, trace::ServiceType::kSkeleton,
                         Desc::serviceId(), request->inst, Desc::serviceIdentifier, request->hdr.method, "methodName",
                         false, request->hdr.session, static_cast< uint8_t >(trace::MsgType::kMethodResponse),
                         static_cast< uint32_t >(ComErrc::kCommunicationStackError), trace::FormatValue(result));
            return ret;
        }
        MonitorTrace(runtime::GetInstance(), trace::userdefine::kSerialize, trace::ServiceType::kSkeleton,
                     Desc::serviceId(), request->inst, Desc::serviceIdentifier, request->hdr.method, "methodName",
                     false, request->hdr.session, static_cast< uint8_t >(trace::MsgType::kMethodResponse), 0U,
                     trace::FormatValue(result));
        response->hdr.code = rcode;
        ret                = runtime.Send(response);
        if (ret < 0) {
            ComLogError("send bind method response error: send response failed", GenArg(ret), GenArg(Desc::methodId()),
                        GenArg(request), GenArg(rcode), GenArg(result));
            MonitorTrace(runtime::GetInstance(), trace::userdefine::kSendResponse, trace::ServiceType::kSkeleton,
                         Desc::serviceId(), request->inst, Desc::serviceIdentifier, request->hdr.method, "methodName",
                         false, request->hdr.session, static_cast< uint8_t >(trace::MsgType::kMethodResponse),
                         static_cast< uint32_t >(ComErrc::kCommunicationStackError), trace::FormatValue(result));
            return ret;
        }
        ComLogDebug("send bind method response done", GenArg(Desc::methodId()), GenArg(request), GenArg(rcode),
                    GenArg(result));
        MonitorTrace(runtime::GetInstance(), trace::userdefine::kSendResponse, trace::ServiceType::kSkeleton,
                     Desc::serviceId(), request->inst, Desc::serviceIdentifier, request->hdr.method, "methodName",
                     false, request->hdr.session, static_cast< uint8_t >(trace::MsgType::kMethodResponse), 0U,
                     trace::FormatValue(result));
        return ret;
    }
    /// @brief Handle E2E error
    /// @param[in] dataID Data identifier
    /// @param[in] e2eErrorCode Error code
    /// @param[in] messageCounter Message counter
    /// [SWS_CM_90464]{DRAFT} E2E Error Handler - Invocation dE2EErrorHandler shall be invoked from
    /// within a separate thread by the Communication Management software in case E2E_check reports an
    /// E2E error.c(RS_CM_00401, RS_CM_00402)
    void _handleE2EError(e2e::DataID dataID,
                         e2e::E2EErrorCode e2eErrorCode,
                         e2e::MessageCounter messageCounter) const noexcept
    {
        ComLogTrace("handle bind method request E2E error", GenArg(Desc::methodId()), GenArg(dataID),
                    GenArg(e2eErrorCode), GenArg(messageCounter));
        auto& runtime{internal::GetInstance()};
        auto threadPool{runtime.GetThreadPool(Desc::serviceIdentifier.ToString()).get()};
        if (threadPool == nullptr) {
            threadPool = runtime.GetThreadPool(runtime.GetProcessName()).get();
        }
        if (threadPool != nullptr && threadPool->Size() >= 1) {
            threadPool->Dispatch([this, e2eErrorCode, dataID, messageCounter]() {
                (**owner_.skeleton_).E2EErrorHandler(e2eErrorCode, dataID, messageCounter);
            });
        } else {
            std::thread([this, e2eErrorCode, dataID, messageCounter]() {
                (**owner_.skeleton_).E2EErrorHandler(e2eErrorCode, dataID, messageCounter);
            }).detach();
        }
    }

private:
    /// @brief Service element owner
    Owner& owner_;
    /// @brief E2E information type
    struct E2EInfo
    {
        /// @brief E2E data identifier
        ara::com::e2exf::IdlistType dataId;
    };
    /// @brief Optional E2E information
    ara::core::Optional< E2EInfo > e2eInfo_;
    /// @brief Callback method getter -- for indirectly getting the callback method
    std::function< Callback() > callback_;
};
}  // namespace skeleton
}  // namespace npc
}  // namespace internal
}  // namespace com
}  // namespace ara
#endif
