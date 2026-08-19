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
/// @file       icc_method.h
/// @brief      Binding layer service proxy method header file
/// @details
/// @date       2021-12-31
/// @author     mazelin
/// @version    1.2.0
///
/// ================================================================

#ifndef __COM_ICC_PROXY_METHOD_H
#define __COM_ICC_PROXY_METHOD_H

#include "../icc_runtime.h"
#include "ara/com/internal/proxy/method.h"
#include "ara/core/promise.h"

/// @brief Namespace -- internal binding layer proxy
namespace ara {
namespace com {
namespace internal {
namespace icc {
namespace proxy {
/// @brief Template type -- binding layer service proxy method -- declaration
/// @tparam Owner Service element owner
/// @tparam Desc Description information
/// @tparam T Method signature type
/// @tparam mode Method mode -- default two-way method
template < typename Owner,
           typename Desc,
           typename T,
           ara::com::internal::proxy::MethodMode mode = ara::com::internal::proxy::MethodMode::kHasReturn >
class IccMethod;
/// @brief Type alias -- one-way method
/// @tparam Owner Service element owner
/// @tparam Desc Description information
/// @tparam T Method signature type
template < typename Owner, typename Desc, typename T >
using IccFMethod = IccMethod<
    Owner,
    Desc,
    std::enable_if_t<
        ara::com::internal::proxy::IsFMethod<
            ara::com::internal::proxy::Method< T, ara::com::internal::proxy::MethodMode::kNonReturn > >::value,
        T >,
    ara::com::internal::proxy::MethodMode::kNonReturn >;
/// @brief Type alias -- two-way void return method
/// @tparam Owner Service element owner
/// @tparam Desc Description information
/// @tparam T Method signature type
template < typename Owner, typename Desc, typename T >
using IccVMethod = IccMethod<
    Owner,
    Desc,
    std::enable_if_t<
        ara::com::internal::proxy::IsVMethod<
            ara::com::internal::proxy::Method< T, ara::com::internal::proxy::MethodMode::kHasReturn > >::value,
        T >,
    ara::com::internal::proxy::MethodMode::kHasReturn >;
/// @brief Type alias -- two-way method
/// @tparam Owner Service element owner
/// @tparam Desc Description information
/// @tparam T Method signature type
template < typename Owner, typename Desc, typename T >
using IccRMethod = IccMethod<
    Owner,
    Desc,
    std::enable_if_t<
        ara::com::internal::proxy::IsRMethod<
            ara::com::internal::proxy::Method< T, ara::com::internal::proxy::MethodMode::kHasReturn > >::value,
        T >,
    ara::com::internal::proxy::MethodMode::kHasReturn >;
/// @brief Template type -- method implementation type -- specialization for one-way method
/// @code{.isoft}
/// export_level=/COM/SoC/SOA
/// @endcode
/// @tparam Owner Service element owner
/// @tparam Desc Description information
/// @tparam Args Method parameter list type
template < typename Owner, typename Desc, typename... Args >
class IccMethod< Owner, Desc, void(Args...), ara::com::internal::proxy::MethodMode::kNonReturn >
    : public ara::com::internal::proxy::MethodBase< void(Args...), ara::com::internal::proxy::MethodMode::kNonReturn >
{
public:
    /// @brief Type alias -- method base type
    using MethodBase
        = ara::com::internal::proxy::MethodBase< void(Args...), ara::com::internal::proxy::MethodMode::kNonReturn >;
    /// @brief Type alias -- method signature
    using Signature = typename MethodBase::Signature;
    /// @brief Type alias -- return type
    using Output = typename MethodBase::Output;
    /// @brief Constructor
    /// @param[in] owner Service element owner
    explicit IccMethod(Owner& owner) noexcept : owner_{owner}
    {
        static_assert(Message::IsMethodId(Desc::methodId()), "invalid method id");
        ComLogTrace("create bind proxy method", GenArg(Desc::methodId()));
    }
    /// @brief Destructor
    ~IccMethod() noexcept override { ComLogTrace("destroy bind proxy method", GenArg(Desc::methodId())); }
    /// @brief Copy constructor
    /// @param other
    IccMethod(IccMethod const& other) noexcept = default;
    /// @brief Move constructor
    /// @param other
    IccMethod(IccMethod&& other) noexcept = default;
    /// @brief Copy assignment operator
    /// @param other
    /// @return IccMethod
    IccMethod& operator=(IccMethod const& other) noexcept = default;
    /// @brief Move assignment operator
    /// @param other
    /// @return IccMethod
    IccMethod& operator=(IccMethod&& other) noexcept = default;
    /// @brief Operator -- call
    /// @param[in] args Parameter list
    /// @ref [SWS_CM_90435]
    /// @ref [SWS_CM_10301]
    void operator()(Args&&... args) noexcept override { _request(std::forward< Args >(args)...); }

private:
    /// @brief Call request
    /// @param[in] args Parameter list
    void _request(Args&&... args) noexcept
    {
        TimeRecorder(TimeNM::Timer, "ICCRequest(F)");
        ComLogTrace("call bind method", GenArg(Desc::methodId()), GenArg(args...));
        auto& runtime{runtime::GetInstance()};
        Message::SessionId sessionId{};
        {
            std::lock_guard< std::mutex > lock(mutex_);
            sessionId = ++sessionId_ != 0 ? sessionId_ : ++sessionId_;
        }
        auto request{runtime::CreateRequest< Desc >(runtime.GetApp(), owner_.instanceId_,
                                                    Message::kMessageTypeRequestNoReturn, sessionId,
                                                    std::forward< Args >(args)...)};
        if (request == nullptr) {
            ComLogError("call bind method error: create request failed", GenArg(Desc::methodId()), GenArg(sessionId),
                        GenArg(args...));
            return;
        }
        auto ret{runtime.Send(request)};
        if (ret < 0) {
            ComLogError("call bind method error: send request failed", GenArg(ret), GenArg(Desc::methodId()),
                        GenArg(sessionId), GenArg(args...));
            return;
        }
        ComLogDebug("call bind method done", GenArg(Desc::methodId()), GenArg(sessionId), GenArg(args...));
    }

private:
    /// @brief Service element owner
    Owner& owner_;
    /// @brief Session identifier
    Message::SessionId sessionId_{};
    /// @brief Mutex
    std::mutex mutex_;
};
/// @brief Template type -- method implementation type -- specialization for two-way method
/// @code{.isoft}
/// export_level=/COM/SoC/SOA
/// @endcode
/// @tparam Owner Service element owner
/// @tparam Desc Description information
/// @tparam R Method return type
/// @tparam Args Method parameter list type
template < typename Owner, typename Desc, typename R, typename... Args >
class IccMethod< Owner, Desc, R(Args...), ara::com::internal::proxy::MethodMode::kHasReturn >
    : public ara::com::internal::proxy::MethodBase< R(Args...), ara::com::internal::proxy::MethodMode::kHasReturn >
{
public:
    /// @brief Type alias -- method base type
    using MethodBase
        = ara::com::internal::proxy::MethodBase< R(Args...), ara::com::internal::proxy::MethodMode::kHasReturn >;
    /// @brief Type alias -- method signature
    using Signature = typename MethodBase::Signature;
    /// @brief Type alias -- return type
    using Output = typename MethodBase::Output;
    /// @brief Constructor
    /// @param[in] owner Service element owner
    explicit IccMethod(Owner& owner) noexcept : owner_{owner}
    {
        static_assert(Message::IsMethodId(Desc::methodId()), "invalid method id");
        ComLogTrace("create bind proxy method", GenArg(Desc::methodId()));
        auto dataId{ara::com::e2exf::StatusHandler::GetDataId(Desc::serviceId(), owner_.instanceId_, Desc::methodId())};
        if (ara::com::e2exf::Transformer::E2E_IsProtected(dataId)) {
            e2eInfo_ = {dataId, e2exf::Transformer::E2E_check(dataId, nullptr)};
        }
        auto& runtime{runtime::GetInstance()};
        auto ret{icc_appcb_set_message(runtime.GetAppCB(), Desc::serviceId(), owner_.instanceId_, Desc::methodId(),
                                       IccMethod::OnResponse, this)};
        if (ret < 0) {
            ComLogError("create bind proxy method error: invoke stack register response callback failed", GenArg(ret),
                        GenArg(Desc::methodId()));
        }
        ComLogTrace("create bind proxy method done", GenArg(Desc::methodId()));
    }
    /// @brief Destructor
    ~IccMethod() noexcept override
    {
        ComLogTrace("destroy bind proxy method", GenArg(Desc::methodId()));
        auto& runtime{runtime::GetInstance()};
        auto ret{icc_appcb_set_message(runtime.GetAppCB(), Desc::serviceId(), owner_.instanceId_, Desc::methodId(),
                                       nullptr, nullptr)};
        if (ret < 0) {
            ComLogError("destroy bind proxy method error: invoke stack unregister response callback failed",
                        GenArg(ret), GenArg(Desc::methodId()));
        }
        std::lock_guard< std::mutex > lock(mutex_);
        for (auto& it : pendingCalls_) {
            auto sessionId{it.first};
            auto promise{std::move(it.second)};
            ComLogWarning("destroy bind proxy method: call bind method discarded: response broken",
                          GenArg(Desc::methodId()), GenArg(sessionId));
            promise.SetError(MakeErrorCode(ara::core::future_errc::broken_promise, __LINE__));
        }
        pendingCalls_.clear();
        ComLogTrace("destroy bind proxy method done", GenArg(Desc::methodId()));
    }
    /// @brief Copy constructor
    /// @param other
    IccMethod(IccMethod const& other) noexcept = default;
    /// @brief Move constructor
    /// @param other
    IccMethod(IccMethod&& other) noexcept = default;
    /// @brief Copy assignment operator
    /// @param other
    /// @return IccMethod
    IccMethod& operator=(IccMethod const& other) noexcept = default;
    /// @brief Move assignment operator
    /// @param other
    /// @return IccMethod
    IccMethod& operator=(IccMethod&& other) noexcept = default;
    /// @brief Operator -- call
    /// @param[in] args Parameter list
    /// @return Future object -- asynchronous/synchronous waiting result object
    ara::core::Future< Output > operator()(Args&&... args) noexcept override
    {
        return _startRequest(std::forward< Args >(args)...);
    }
    /// @brief Get global E2E SMState
    /// @return Result of the last called E2E_check function
    e2e::SMState GetSMState() const noexcept override
    {
        if (!e2eInfo_ || !e2eInfo_->result) {
            return e2e::SMState::kStateMDisabled;
        }
        return e2eInfo_->result.Value().GetSMState();
    }

private:
    /// @brief Start call request
    /// @param[in] args Parameter list
    /// @return Future object -- asynchronous/synchronous waiting result object
    ara::core::Future< Output > _startRequest(Args&&... args) noexcept
    {
        TimeRecorder(TimeNM::Timer, "ICCRequest(R)");
        ComLogTrace("call bind method", GenArg(Desc::methodId()), GenArg(args...));
        ara::core::Promise< Output > promise;
        auto future{promise.get_future()};
        auto& runtime{runtime::GetInstance()};
        Message::SessionId sessionId{};
        {
            std::lock_guard< std::mutex > lock(mutex_);
            sessionId = ++sessionId_ != 0 ? sessionId_ : ++sessionId_;
        }
        auto request{runtime::CreateRequest< Desc >(runtime.GetApp(), owner_.instanceId_, Message::kMessageTypeRequest,
                                                    sessionId, std::forward< Args >(args)...)};
        if (request == nullptr) {
            ComLogError("call bind method error: create request failed", GenArg(Desc::methodId()), GenArg(sessionId),
                        GenArg(args...));
            promise.SetError(MakeErrorCode(ComErrc::kCommunicationStackError, -__LINE__));
            return future;
        }
        // TODO(jiawei) ara::core::Future can Cancel the method call [SWS_CM_00194]{DRAFT}
        // Therefore timeout is handled by the application layer (SWS_CM_00192)
        typename PendingCalls::iterator it{};
        {
            std::lock_guard< std::mutex > lock(mutex_);
            it = pendingCalls_.emplace(request->hdr.session, std::move(promise)).first;
        }
        auto ret{runtime.Send(request)};
        if (ret < 0) {
            ComLogWarning("call bind method error: send request failed", GenArg(ret), GenArg(Desc::methodId()),
                          GenArg(sessionId), GenArg(args...));
            {
                std::lock_guard< std::mutex > lock(mutex_);
                promise     = std::move(it->second);
                std::ignore = pendingCalls_.erase(it);
            }
            promise.SetError(MakeErrorCode(ComErrc::kNetworkBindingFailure, ret));
            return future;
        }
        ComLogDebug("call bind method pending", GenArg(Desc::methodId()), GenArg(sessionId), GenArg(args...));
        return future;
    }
    /// @brief Callback function when a response message is received
    /// @param[in] a Implementation layer application
    /// @param[in] ud User data
    /// @param[in] m Message
    /// @return Result -- 0: success; !0: failure;
    static int32_t OnResponse(icc_app_t* a, void* ud, icc_message_t* m) noexcept
    {
        std::ignore = a;
        TimeRecorder(TimeNM::Timer, "ICCOnResponse");
        ComLogDebug("on bind method response", GenArg(Desc::methodId()), GenArg(m));
        auto& runtime{internal::GetInstance()};
        auto threadPool{runtime.GetThreadPool(Desc::serviceIdentifier.ToString()).get()};
        if (threadPool == nullptr) {
            threadPool = runtime.GetThreadPool(runtime.GetProcessName()).get();
        }
        if (threadPool != nullptr) {
            threadPool->Dispatch([=] {
                auto* thisObj{reinterpret_cast< IccMethod* >(ud)};
                std::ignore = thisObj->_handleResponse(std::shared_ptr< icc_message_t >(m, icc_message_close));
            });
            return 0;
        }
        auto* thisObj{reinterpret_cast< IccMethod* >(ud)};
        return thisObj->_handleResponse(std::shared_ptr< icc_message_t >(m, icc_message_close));
    }
    /// @brief Process response message
    /// @param[in] response Response message
    /// @return Result -- 0: success; !0: failure;
    /// @ref [SWS_CM_10358]
    int32_t _handleResponse(std::shared_ptr< icc_message_t > const& response) noexcept
    {
        ComLogTrace("handle bind method response", GenArg(Desc::methodId()), GenArg(response));
        int32_t ret{};
        ara::core::Promise< Output > promise;
        {
            std::lock_guard< std::mutex > lock(mutex_);
            auto it{pendingCalls_.find(response->hdr.session)};
            if (it == pendingCalls_.end()) {
                ret = -__LINE__;
                ComLogWarning("handle bind method response rejected: find request failed", GenArg(Desc::methodId()),
                              GenArg(response));
                return ret;
            }
            promise = std::move(it->second);
            pendingCalls_.erase(it);
            ComLogTrace("handle bind method response: call bind method resumed", GenArg(Desc::methodId()),
                        GenArg(response));
        }
        TimeStart(TimeNM::Timer, "ICCResponse::Verify&SecOC");
        auto rcode{runtime::VerifyResponse< Desc >(*response)};
        if (rcode != Message::kReturnCodeOk) {
            ret = -__LINE__;
            ComLogWarning("handle bind method response: call bind method error: verify response invalid", GenArg(rcode),
                          GenArg(Desc::methodId()), GenArg(response));
            promise.SetError(MakeErrorCode(ComErrc::kNetworkBindingFailure, rcode));
            return ret;
        }
#ifdef HAS_COM_SECOC
        // secoc rx process
        auto secOcDataId{secoc::SecOC4SOMEIP::GetSecOCDataId(response->hdr.serv, response->inst, response->hdr.method,
                                                             response->hdr.type)};
        if (secOcDataId > 0) {
            ret = secoc::SecOC4SOMEIP::SecOCRxHandle(secOcDataId, response.get());
            if (ret < 0) {
                ComLogWarning("invoke SecOC Rx handle", GenArg(ret));
                promise.SetError(MakeErrorCode(ComErrc::kGrantEnforcementError, ret));
                return ret;
            }
            ComLogDebug("invoke SecOC Rx handle", GenArg(ret));
        }
#else
#endif
        TimeStop(TimeNM::Timer, "ICCResponse::Verify&SecOC");
#if 0  // Error domain identifier source not provided @ref [SWS_CM_10358]
        if (Message::IsResponse(response->hdr.type) && response->hdr.code > Message::kReturnCodeReservedMax4Generic) {
            ara::core::ErrorCode value{response->hdr.code - Message::kReturnCodeReservedMax4Generic};
            ComLogTrace("handle bind method response: backward compatible application error", GenArg(value),
                        GenArg(Desc::methodId()), GenArg(response));
            ret = runtime::Deserialize< Desc >(*response, value);
            if (ret >= 0) {
                ComLogDebug("handle bind method response: call bind method done: backward compatible application error",
                            GenArg(value), GenArg(Desc::methodId()), GenArg(response));
                promise.SetError(std::move(value));
                return 0;
            }
        }
#endif
        if (response->hdr.code != Message::kReturnCodeOk) {
            ara::core::ErrorCode value{ComErrc::kCommunicationStackError};
            if (e2eInfo_) {  // enable e2e
                serialize::E2EPayload payload{*response, e2eInfo_->dataId};
                ret              = runtime::Deserialize< Desc >(payload, value);
                e2eInfo_->result = payload.r;
                if (ret < 0) {
                    ComLogWarning(
                        "handle bind method response: call bind method error: deserialize E2E response failed",
                        GenArg(ret), GenArg(Desc::methodId()), GenArg(response));
                    promise.SetError(MakeErrorCode(ComErrc::kCommunicationStackError, ret));
                    return ret;
                }
                if (!e2eInfo_->result) {
                    ComLogWarning("handle bind method response: call bind method error: check E2E response failed",
                                  GenArg(e2eInfo_->result.Error()), GenArg(Desc::methodId()), GenArg(response));
                    promise.SetError(e2eInfo_->result.Error());
                    return 0;
                }
                auto status{e2eInfo_->result.Value().GetProfileCheckStatus()};
                if (status != e2e::ProfileCheckStatus::kOk) {
                    ret = -__LINE__;
                    ComLogWarning("handle bind method response: call bind method error: check E2E response failed",
                                  GenArg(status), GenArg(Desc::methodId()), GenArg(response));
                    promise.SetError(e2e::ProfileCheckStatusToErrorCode(status));
                    return ret;
                }
            } else {
                ret = runtime::Deserialize< Desc >(*response, value);
                if (ret < 0) {
                    ComLogWarning("handle bind method response: call bind method error: deserialize response failed",
                                  GenArg(ret), GenArg(Desc::methodId()), GenArg(response));
                    promise.SetError(MakeErrorCode(ComErrc::kCommunicationStackError, ret));
                    return ret;
                }
            }
            ComLogDebug("handle bind method response: call bind method done", GenArg(value), GenArg(Desc::methodId()),
                        GenArg(response));
            promise.SetError(std::move(value));
            return 0;
        }
        return _handleResponse(response, promise);
    }
    /// @brief Process response message -- return value
    /// @tparam U Return type
    /// @param[in] response Response message
    /// @param[in] promise Promise of the related call request
    /// @return Result -- 0: success; !0: failure;
    template < typename U = Output >
    int32_t _handleResponse(std::shared_ptr< icc_message_t > const& response,
                            ara::core::Promise< Output >& promise,
                            std::enable_if_t< !std::is_same< U, void >::value >* = nullptr) noexcept
    {
        TimeRecorder(TimeNM::Timer, "ICCHandleResponse(value)");
        Output value;
        int32_t ret{};
        if (e2eInfo_) {  // enable e2e
            serialize::E2EPayload payload{*response, e2eInfo_->dataId};
            ret              = runtime::Deserialize< Desc >(payload, value);
            e2eInfo_->result = payload.r;
            if (ret < 0) {
                ComLogWarning("handle bind method response: call bind method error: deserialize E2E response failed",
                              GenArg(ret), GenArg(Desc::methodId()), GenArg(response));
                promise.SetError(MakeErrorCode(ComErrc::kCommunicationStackError, ret));
                return ret;
            }
            if (!e2eInfo_->result) {
                ComLogWarning("handle bind method response: call bind method error: check E2E response failed",
                              GenArg(e2eInfo_->result.Error()), GenArg(Desc::methodId()), GenArg(response));
                promise.SetError(e2eInfo_->result.Error());
                return 0;
            }
            auto status{e2eInfo_->result.Value().GetProfileCheckStatus()};
            if (status != e2e::ProfileCheckStatus::kOk) {
                ret = -__LINE__;
                ComLogWarning("handle bind method response: call bind method error: check E2E response failed",
                              GenArg(status), GenArg(Desc::methodId()), GenArg(response));
                promise.SetError(e2e::ProfileCheckStatusToErrorCode(status));
                return ret;
            }
        } else {
            ret = runtime::Deserialize< Desc >(*response, value);
            if (ret < 0) {
                ComLogWarning("handle bind method response: call bind method error: deserialize response failed",
                              GenArg(ret), GenArg(Desc::methodId()), GenArg(response));
                promise.SetError(MakeErrorCode(ComErrc::kCommunicationStackError, ret));
                return ret;
            }
        }
        ComLogDebug("handle bind method response: call bind method done", GenArg(value), GenArg(Desc::methodId()),
                    GenArg(response));
        promise.set_value(std::move(value));
        return 0;
    }
    /// @brief Process response message -- void return
    /// @tparam U Return type
    /// @param[in] response Response message
    /// @param[in] promise Promise of the related call request
    /// @return Result -- 0: success; !0: failure;
    template < typename U = Output >
    int32_t _handleResponse(std::shared_ptr< icc_message_t > const& response,
                            ara::core::Promise< Output >& promise,
                            std::enable_if_t< std::is_same< U, void >::value >* = nullptr) noexcept
    {
        TimeRecorder(TimeNM::Timer, "ICCHandleResponse(void)");
        int32_t ret{};
        if (e2eInfo_) {  // enable e2e
            serialize::E2EPayload payload{*response, e2eInfo_->dataId};
            ret              = runtime::Deserialize< Desc >(payload);
            e2eInfo_->result = payload.r;
            if (ret < 0) {
                ComLogWarning("handle bind method response: call bind method error: deserialize E2E response failed",
                              GenArg(ret), GenArg(Desc::methodId()), GenArg(response));
                promise.SetError(MakeErrorCode(ComErrc::kCommunicationStackError, ret));
                return ret;
            }
            if (!e2eInfo_->result) {
                ComLogWarning("handle bind method response: call bind method error: check E2E response failed",
                              GenArg(e2eInfo_->result.Error()), GenArg(Desc::methodId()), GenArg(response));
                promise.SetError(e2eInfo_->result.Error());
                return 0;
            }
            auto status{e2eInfo_->result.Value().GetProfileCheckStatus()};
            if (status != e2e::ProfileCheckStatus::kOk) {
                ret = -__LINE__;
                ComLogWarning("handle bind method response: call bind method error: check E2E response failed",
                              GenArg(status), GenArg(Desc::methodId()), GenArg(response));
                promise.SetError(e2e::ProfileCheckStatusToErrorCode(status));
                return ret;
            }
        } else {
            ;  // do nothing
        }
        ComLogDebug("handle bind method response: call bind method done", GenArg(Desc::methodId()), GenArg(response));
        promise.set_value();
        return 0;
    }

private:
    /// @brief Service element owner
    Owner& owner_;
    /// @brief E2E information type
    struct E2EInfo
    {
        /// @brief E2E data identifier
        ara::com::e2exf::IdlistType dataId;
        /// @brief E2E check result -- current
        e2exf::E2EResult result;
    };
    /// @brief Optional E2E information
    ara::core::Optional< E2EInfo > e2eInfo_;
    /// @brief Session identifier
    Message::SessionId sessionId_{};
    /// @brief Type alias -- pending call set
    using PendingCalls = ara::core::Map< Message::SessionId, ara::core::Promise< Output > >;
    /// @brief Pending call set
    PendingCalls pendingCalls_;
    /// @brief Mutex
    std::mutex mutex_;
};
}  // namespace proxy
}  // namespace icc
}  // namespace internal
}  // namespace com
}  // namespace ara
#endif
