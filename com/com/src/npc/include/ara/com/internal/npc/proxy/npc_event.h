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
/// @file       npc_event.h
/// @brief      Binding layer service proxy event header file
/// @details
/// @date       2021-12-31
/// @author     mazelin
/// @version    1.2.0
///
/// ================================================================

#ifndef __COM_NPC_PROXY_EVENT_H
#define __COM_NPC_PROXY_EVENT_H

#include "../npc_runtime.h"
#include "ara/com/internal/proxy/event.h"
#ifdef HAS_COM_SECOC
    #include "ara/com/internal/secoc/secoc_signal.h"
    #include "ara/com/internal/secoc/secoc_someip.h"
#else
#endif

/// @brief Namespace -- internal binding layer proxy
namespace ara {
namespace com {
namespace internal {
namespace npc {
namespace proxy {
/// @brief Template type -- binding layer service proxy event
/// @code{.isoft}
/// export_level=/COM/SoC/SOA
/// @endcode
/// @tparam Owner Service element owner
/// @tparam Desc Description info
/// @tparam T Event data type
template < typename Owner, typename Desc, typename T >
class NpcEvent : public virtual ara::com::internal::proxy::EventBase< T >
{
public:
    /// @brief Type alias -- event base type
    using EventBase = ara::com::internal::proxy::EventBase< T >;
    /// @brief Type alias -- sample data type
    using SampleType = typename EventBase::SampleType;
    /// @brief Type alias -- new sample callback signature
    using NewSamplesCallback = typename EventBase::NewSamplesCallback;
    /// @brief Constructor
    /// @tparam c Specialization condition -- whether it is signal-based (false)
    /// @param[in] owner Service element owner
    template < bool c = signal::isSignalBased< Desc >() >
    explicit NpcEvent(Owner& owner, std::enable_if_t< !c >* = nullptr) noexcept : owner_{owner}
    {
        static_assert(Message::IsEventId(Desc::eventId()), "invalid event id");
        ComLogTrace("create bind proxy event", GenArg(Desc::eventId()));
        auto dataId{e2exf::StatusHandler::GetDataId(Desc::serviceId(), owner_.instanceId_, Desc::eventId())};
        if (e2exf::Transformer::E2E_IsProtected(dataId)) {
            e2eInfo_ = {dataId, e2exf::Transformer::E2E_check(dataId, nullptr)};
        }
        ComLogTrace("create bind proxy event done", GenArg(Desc::eventId()));
    }
    /// @brief Constructor
    /// @tparam c Specialization condition -- whether it is signal-based (true)
    /// @param[in] owner Service element owner
    template < bool c = signal::isSignalBased< Desc >() >
    explicit NpcEvent(Owner& owner, std::enable_if_t< c >* = nullptr) noexcept : owner_{owner}
    {
        static_assert(Message::IsEventId(Desc::eventId()), "invalid event id");
        static_assert(signal::isS2SSource< Desc >(), "unsupport auto forward");
        ComLogTrace("create bind proxy event", GenArg(Desc::eventId()));
        auto dataId{e2exf::StatusHandler::GetDataId(Desc::serviceId(), owner_.instanceId_, Desc::eventId())};
        if (e2exf::Transformer::E2E_IsProtected(dataId)) {
            e2eInfo_ = {dataId, e2exf::Transformer::E2E_check(dataId, nullptr)};
        }
        ComLogTrace("create bind proxy event done", GenArg(Desc::eventId()));
    }
    /// @brief Destructor
    ~NpcEvent() noexcept override { ComLogTrace("destroy bind proxy event", GenArg(Desc::eventId())); }
    /// @brief Copy constructor
    /// @param other
    NpcEvent(NpcEvent const& other) noexcept = default;
    /// @brief Move constructor
    /// @param other
    NpcEvent(NpcEvent&& other) noexcept = default;
    /// @brief Copy assignment operator
    /// @param other
    /// @return NpcEvent
    NpcEvent& operator=(NpcEvent const& other) noexcept = default;
    /// @brief Move assignment operator
    /// @param other
    /// @return NpcEvent
    NpcEvent& operator=(NpcEvent&& other) noexcept = default;
    /// @brief Subscribe to event
    /// @param[in] maxSampleCount Maximum number of samples to receive
    /// @return Result object -- empty/value or error
    ara::core::Result< void > Subscribe(size_t maxSampleCount) noexcept override
    {
        using Result = ara::core::Result< void >;
        ComLogTrace("subscribe bind event", GenArg(Desc::eventId()), GenArg(maxSampleCount));
        if (subscriptionState_ != SubscriptionState::kNotSubscribed) {
            if (maxSampleCount != maxSampleCount_) {
                ComLogError("subscribe bind event refused: max sample count incompatibility", GenArg(Desc::eventId()),
                            GenArg(maxSampleCount), GenArg(maxSampleCount_));
                MonitorTrace(runtime::GetInstance(), trace::userdefine::kEventSubscribe, trace::ServiceType::kProxy,
                             Desc::serviceId(), owner_.instanceId_, Desc::serviceIdentifier, Desc::eventId(),
                             "eventName", this->IsField(), static_cast< uint32_t >(ComErrc::kCommunicationStackError));
                return Result{MakeErrorCode(ComErrc::kMaxSampleCountNotRealizable, -__LINE__)};
            }
            ComLogDebug("subscribe bind event ignored: subscription repeat", GenArg(Desc::eventId()));
            return Result{};
        }
        if (maxSampleCount < 1) {
            ComLogError("subscribe bind event refused: max sample count < 1", GenArg(Desc::eventId()),
                        GenArg(maxSampleCount));
            MonitorTrace(runtime::GetInstance(), trace::userdefine::kEventSubscribe, trace::ServiceType::kProxy,
                         Desc::serviceId(), owner_.instanceId_, Desc::serviceIdentifier, Desc::eventId(), "eventName",
                         this->IsField(), static_cast< uint32_t >(ComErrc::kCommunicationStackError));
            return Result{MakeErrorCode(ComErrc::kMaxSampleCountNotRealizable, -__LINE__)};
        }
        maxSampleCount_ = maxSampleCount;
        auto& runtime{runtime::GetInstance()};
        auto ret{npc_appcb_set_message(runtime.GetAppCB(), Desc::serviceId(), owner_.instanceId_, Desc::eventId(),
                                       NpcEvent::OnNotification, this)};
        if (ret < 0) {
            ComLogError("subscribe bind event error: invoke stack register notification callback failed", GenArg(ret),
                        GenArg(Desc::eventId()));
            MonitorTrace(runtime::GetInstance(), trace::userdefine::kEventSubscribe, trace::ServiceType::kProxy,
                         Desc::serviceId(), owner_.instanceId_, Desc::serviceIdentifier, Desc::eventId(), "eventName",
                         this->IsField(), static_cast< uint32_t >(ComErrc::kCommunicationStackError));
            return Result{MakeErrorCode(ComErrc::kCommunicationStackError, ret)};
        }
        _setSubscriptionState(SubscriptionState::kSubscriptionPending);
        ComLogDebug("subscribe bind event pending", GenArg(Desc::eventId()));
        npc_event_info_t ev{};
        ev.reliability = NPC_RT_UNKNOWN;  // Client event reliability is determined by the server side
        ev.type        = this->IsField() ? NPC_ET_FIELD : NPC_ET_EVENT;
        auto eventGroups{Desc::eventGroups()};
        ev.groups = const_cast< decltype(ev.groups) >(eventGroups.data());
        ev.ngroup = static_cast< decltype(ev.ngroup) >(eventGroups.size());
        ret       = runtime.SubscribeEvent(
            Desc::serviceId(), owner_.instanceId_, Desc::serviceVersionMajor(), Desc::eventId(), &ev,
            [this](int32_t rcode) {
                ComLogTrace("handle subscribe bind event ack", GenArg(Desc::eventId()), GenArg(rcode));
                if (rcode != Message::kReturnCodeOk) {
                    _setSubscriptionState(SubscriptionState::kSubscriptionPending);
                    ComLogWarning("subscribe bind event nack", GenArg(Desc::eventId()), GenArg(rcode));
                    MonitorTrace(runtime::GetInstance(), trace::userdefine::kEventSubscribe, trace::ServiceType::kProxy,
                                 Desc::serviceId(), owner_.instanceId_, Desc::serviceIdentifier, Desc::eventId(),
                                 "eventName", this->IsField(),
                                 static_cast< uint32_t >(ComErrc::kCommunicationStackError));
                    return;
                }
                _setSubscriptionState(SubscriptionState::kSubscribed);
                ComLogDebug("subscribe bind event ack", GenArg(Desc::eventId()));
            });
        if (ret < 0) {
            ComLogError("subscribe bind event error: subscribe event group failed", GenArg(ret),
                        GenArg(Desc::eventId()));
            MonitorTrace(runtime::GetInstance(), trace::userdefine::kEventSubscribe, trace::ServiceType::kProxy,
                         Desc::serviceId(), owner_.instanceId_, Desc::serviceIdentifier, Desc::eventId(), "eventName",
                         this->IsField(), static_cast< uint32_t >(ComErrc::kCommunicationStackError));
            return Result{MakeErrorCode(ComErrc::kCommunicationStackError, ret)};
        }
        return Result{};
    }
    /// @brief Unsubscribe from event
    void Unsubscribe() noexcept override
    {
        ComLogTrace("unsubscribe bind event", GenArg(Desc::eventId()));
        if (subscriptionState_ == SubscriptionState::kNotSubscribed) {
            ComLogDebug("unsubscribe bind event ignored: unsubscription repeat", GenArg(Desc::eventId()));
            return;
        }
        npc_event_info_t ev{};
        auto eventGroups{Desc::eventGroups()};
        ev.groups = const_cast< decltype(ev.groups) >(eventGroups.data());
        ev.ngroup = static_cast< decltype(ev.ngroup) >(eventGroups.size());
        auto& runtime{runtime::GetInstance()};
        auto ret{runtime.UnsubscribeEvent(Desc::serviceId(), owner_.instanceId_, Desc::serviceVersionMajor(),
                                          Desc::eventId(), &ev)};
        if (ret < 0) {
            ComLogError("unsubscribe bind event error: unsubscribe event group failed", GenArg(ret),
                        GenArg(Desc::eventId()));
        }
        ret = npc_appcb_set_message(runtime.GetAppCB(), Desc::serviceId(), owner_.instanceId_, Desc::eventId(), nullptr,
                                    nullptr);
        if (ret < 0) {
            ComLogError("unsubscribe bind event error: invoke stack unregister notification callback failed",
                        GenArg(ret), GenArg(Desc::eventId()));
        }
        _setSubscriptionState(SubscriptionState::kNotSubscribed);
        // Clear caches to
        // 1. save memory and
        // 2. ensure that they're not bigger than maxSampleCount on the next call of Subscribe
        std::lock_guard< std::mutex > lock(mutex_);
        queue_.clear();
        ComLogDebug("unsubscribe bind event done", GenArg(Desc::eventId()));
        MonitorTrace(runtime::GetInstance(), trace::userdefine::kEventUnsubscribe, trace::ServiceType::kProxy,
                     Desc::serviceId(), owner_.instanceId_, Desc::serviceIdentifier, Desc::eventId(), "eventName",
                     this->IsField(), 0);
    }
    /// @brief Get event subscription status
    /// @return Event subscription status
    SubscriptionState GetSubscriptionState() const noexcept override { return subscriptionState_; }
    /// @brief Set event receive handler
    /// @param[in] handler Receive handler
    /// @return Result object -- empty/value or error
    ara::core::Result< void > SetReceiveHandler(EventReceiveHandler handler) noexcept override
    {
        using Result = ara::core::Result< void >;
        std::lock_guard< std::mutex > lock(mutex_);
        receiveHandler_ = handler;
        return Result{};
    }
    /// @brief Reset event receive handler
    /// @return Result object -- empty/value or error
    ara::core::Result< void > UnsetReceiveHandler() noexcept override
    {
        using Result = ara::core::Result< void >;
        std::lock_guard< std::mutex > lock(mutex_);
        receiveHandler_ = nullptr;
        return Result{};
    }
    /// @brief Set event subscription status change handler
    /// @param[in] handler Subscription status change handler
    /// @return Result object -- empty/value or error
    ara::core::Result< void > SetSubscriptionStateChangeHandler(
        SubscriptionStateChangeHandler handler) noexcept override
    {
        using Result = ara::core::Result< void >;
        std::lock_guard< std::mutex > lock(mutex_);
        stateChangeHandler_ = handler;
        return Result{};
    }
    /// @brief Reset event subscription status change handler
    void UnsetSubscriptionStateChangeHandler() noexcept override
    {
        std::lock_guard< std::mutex > lock(mutex_);
        stateChangeHandler_ = nullptr;
    }
#ifdef HAS_COM_EVENT_LOST_CHECKER
    /// @brief Set event sample loss handler
    /// @param[in] handler Sample loss handler
    /// @return Result object -- empty/value or error
    ara::core::Result< void > SetSampleLostHandler(SampleLostHandler handler) noexcept override
    {
        using Result = ara::core::Result< void >;
        std::lock_guard< std::mutex > lock(mutex_);
        lostHandler_ = handler;
        return Result{};
    }
    /// @brief Reset event sample loss handler
    void UnsetSampleLostHandler() noexcept override
    {
        std::lock_guard< std::mutex > lock(mutex_);
        lostHandler_ = nullptr;
    }
#endif  // HAS_COM_EVENT_LOST_CHECKER
    /// @brief Get new sample
    /// @param[in] callback Callback function for processing new samples
    /// @param[in] maxNumberOfSamples Maximum number of samples to get
    /// @return Result object -- empty/value or error
    /// @ref [SWS_CM_90406]
    /// @ref [SWS_CM_90407]
    /// @ref [SWS_CM_90408]
    /// @ref [SWS_CM_90410]
    /// @ref [SWS_CM_90411]
    /// @ref [SWS_CM_90412]
    /// @ref [SWS_CM_90413]
    /// @ref [SWS_CM_90415]
    /// @ref [SWS_CM_90417]
    ara::core::Result< size_t > GetNewSamples(NewSamplesCallback&& callback,
                                              size_t maxNumberOfSamples) noexcept override
    {
        TimeRecorder(TimeNM::Timer, "NPCNotify::GetNewSamples");
        ComLogTrace("get bind event new sample", GenArg(Desc::eventId()), GenArg(maxNumberOfSamples));
        return GetNewSamplesImpl(std::forward< NewSamplesCallback >(callback), maxNumberOfSamples);
    }
    ara::core::Result< size_t > GetNewSamplesImpl(NewSamplesCallback&& callback, size_t maxNumberOfSamples) noexcept
    {
        TimeRecorder(TimeNM::Timer, "NPCNotify::GetNewSamplesImpl");
        using Result = ara::core::Result< size_t >;
        {
            std::lock_guard< std::mutex > lock(mutex_);
            if (queue_.empty()) {
                if (e2eInfo_) {
                    e2eInfo_->result = e2exf::Transformer::E2E_check(e2eInfo_->dataId, nullptr);
                }
                ComLogWarning("get bind event new sample canceled: no new samples", GenArg(Desc::eventId()));
                return Result{MakeErrorCode(ComErrc::kMaxSamplesReached, -__LINE__)};
            }
        }
        decltype(maxNumberOfSamples) passedNumberOfSamples{};
        while (true) {
            using Sample = SamplePtr< SampleType const >;
            Sample sample;
            {
                std::lock_guard< std::mutex > lock(mutex_);
                /// [SWS_CM_00703] Condition 1: The specified maximum number of samples has been obtained from the underlying cache
                if (passedNumberOfSamples == maxNumberOfSamples) {
                    break;
                }
                /// [SWS_CM_00703] Condition 2: The subscription maximum number of samples has been obtained from the underlying cache
                /// [SWS_CM_00703] Condition 3: No new data samples are available in the underlying receive buffer
                if (queue_.empty()) {
                    ComLogDebug("get bind event new sample done: max number of samples has been reached",
                                GenArg(Desc::eventId()));
                    break;
                }
                /// [SWS_CM_00703] Step 1: Get the next received event data sample from the underlying receive buffer
                auto message{std::move(queue_.front())};
                queue_.pop_front();
                ComLogTrace("get bind event new sample: notify bind event dequeue", GenArg(Desc::eventId()),
                            GenArg(queue_.size()), GenArg(message));
                ++passedNumberOfSamples;
                /// [SWS_CM_00703] Step 2: Deserialize data if necessary
                if (e2eInfo_) {  // enable e2e
                    serialize::E2EPayload payload{*message, e2eInfo_->dataId};
                    auto value{std::make_unique< SampleType >()};
                    auto ret{_deserialize(payload, *value)};
                    e2eInfo_->result = payload.r;
                    if (ret < 0) {
                        ComLogWarning("get bind event new sample skipped: deserialize E2E notification failed",
                                      GenArg(ret), GenArg(Desc::eventId()), GenArg(message));
                        MonitorTrace(runtime::GetInstance(), trace::userdefine::kDeserialize,
                                     trace::ServiceType::kProxy, Desc::serviceId(), owner_.instanceId_,
                                     Desc::serviceIdentifier, Desc::eventId(), "eventName", this->IsField(),
                                     message->hdr.session, static_cast< uint8_t >(trace::MsgType::kEventNotify),
                                     static_cast< uint32_t >(ComErrc::kCommunicationStackError),
                                     trace::FormatValue(""));
                        continue;
                    }
                    if (!e2eInfo_->result) {
                        ComLogWarning("get bind event new sample skipped: check E2E notification failed",
                                      GenArg(e2eInfo_->result.Error()), GenArg(Desc::eventId()), GenArg(message));
                        continue;
                    }
                    /// [SWS_CM_00703] Step 3: Place the deserialized data sample of SampleType in the local cache
                    sample = Sample{value.release(), e2eInfo_->result.Value().GetProfileCheckStatus()};
                } else {
#if 0  // Binding layer differences
                    auto value{std::make_unique< SampleType >()};
                    auto ret{_deserialize(*message, *value)};
                    if (ret < 0) {
                        MonitorTrace(runtime::GetInstance(), trace::userdefine::kDeserialize,
                                     trace::ServiceType::kProxy, Desc::serviceId(), owner_.instanceId_,
                                     Desc::serviceIdentifier, Desc::eventId(), "eventName", this->IsField(),
                                     message->hdr.session, static_cast< uint8_t >(trace::MsgType::kEventNotify),
                                     static_cast< uint32_t >(ComErrc::kCommunicationStackError),
                                     trace::FormatValue(""));
                        ComLogWarning("get bind event new sample skipped: deserialize notification failed", GenArg(ret),
                                      GenArg(Desc::eventId()), GenArg(message));
                        continue;
                    }
                    /// [SWS_CM_00703] Step 3: Place the deserialized data sample of SampleType in the local cache
                    sample = Sample{value.release()};
#else
                    if (npc_message_unserialized_get(message)) {
                        auto value{reinterpret_cast< SampleType* >(
                            nai_buf_ptr(reinterpret_cast< nai_buf_t* >(message->payload.ent.next)))};
                        sample = Sample{value, [message](auto) {}};  // Sample destructor triggers message destructor, triggering shared memory reclamation
                    } else {
                        auto value{std::make_unique< SampleType >()};
                        auto ret{_deserialize(*message, *value)};
                        if (ret < 0) {
                            ComLogWarning("get bind event new sample skipped: deserialize notification failed",
                                          GenArg(ret), GenArg(Desc::eventId()), GenArg(message));
                            continue;
                        }
                        /// [SWS_CM_00703] Step 3: Place the deserialized data sample of SampleType in the local cache
                        sample = Sample{value.release()};
                    }
#endif
                }
                MonitorTrace(runtime::GetInstance(), trace::userdefine::kDeserialize, trace::ServiceType::kProxy,
                             Desc::serviceId(), owner_.instanceId_, Desc::serviceIdentifier, Desc::eventId(),
                             "eventName", this->IsField(), message->hdr.session,
                             static_cast< uint8_t >(trace::MsgType::kEventNotify), 0U, trace::FormatValue(*sample));
            }
            /// [SWS_CM_00703] Step 4: Call the user-provided processing function referencing the sample data in the local cache
            ComLogDebug("get bind event new sample: invoke callback handle event new sample", GenArg(Desc::eventId()));
            callback(std::move(sample));
            ComLogTrace("get bind event new sample: invoke callback handle event new sample done",
                        GenArg(Desc::eventId()));
        }
        return Result{passedNumberOfSamples};
    }
    /// @brief Get remaining sample count
    /// @return Remaining sample count
    size_t GetFreeSampleCount() noexcept override
    {
        std::lock_guard< std::mutex > lock(mutex_);
        auto size{queue_.size()};
        return maxSampleCount_ < size ? 0 : maxSampleCount_ - size;
    }
    /// @brief Get last received E2E result
    /// @return Result object -- E2E value or error
    e2exf::E2EResult GetResult() const noexcept override
    {
        if (!e2eInfo_) {
            return e2exf::E2EResult{{}};
        }
        return e2eInfo_->result;
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
    /// @brief Callback function when a notification message is received
    /// @param[in] a Implementation layer application
    /// @param[in] ud User data
    /// @param[in] m Message
    /// @return Result -- 0: success; !0: failure;
    static int32_t OnNotification(npc_app_t* a, void* ud, npc_message_t* m) noexcept
    {
        TimeRecorder(TimeNM::Timer, "NPCNotify::OnNotification");
        std::ignore = a;
        ComLogDebug("on bind event notification", GenArg(Desc::eventId()), GenArg(m));
        auto& runtime{internal::GetInstance()};
        auto threadPool{runtime.GetThreadPool(Desc::serviceIdentifier.ToString()).get()};
        if (threadPool == nullptr) {
            threadPool = runtime.GetThreadPool(runtime.GetProcessName()).get();
        }
        if (threadPool != nullptr) {
            threadPool->Dispatch([=] {
                auto* thisObj{reinterpret_cast< NpcEvent* >(ud)};
                thisObj->_handleNotification(std::shared_ptr< npc_message_t >(m, npc_message_close));
            });
            return 0;
        }
        auto* thisObj{reinterpret_cast< NpcEvent* >(ud)};
        return thisObj->_handleNotification(std::shared_ptr< npc_message_t >(m, npc_message_close));
    }
    /// @brief Handle notification message
    /// @param[in] message Message
    /// @return Result -- 0: success; !0: failure;
    int32_t _handleNotification(std::shared_ptr< npc_message_t > const& message) noexcept
    {
        TimeStart(TimeNM::Timer, "NPCNotify::Verify&SecOC");
        ComLogTrace("handle bind event notification", GenArg(message));
        int32_t ret{};
        auto rcode{message->hdr.code};
        /// [TR_SOMEIP_00187] If the first 2 bits of the return code are reserved, then verify the last 6 bits
        if ((rcode & Message::kReturnCodeReservedMask) == Message::kReturnCodeReservedMask) {
            rcode &= Message::kReturnCodeValidMask;
        }
        /// [TR_SOMEIP_00539] If the return code is invalid (0x01-0x1F), then ignore the request
        if (rcode != Message::kReturnCodeOk) {
            ret = -__LINE__;
            ComLogWarning("handle bind event notification rejected: verify return code invalid",
                          GenArg(Desc::eventId()), GenArg(message), GenArg(rcode));
            MonitorTrace(
                runtime::GetInstance(), trace::userdefine::kRecvEvent, trace::ServiceType::kProxy, Desc::serviceId(),
                message->inst, Desc::serviceIdentifier, Desc::eventId(), "eventName", false, message->hdr.session,
                (ret != Message::kReturnCodeOk) ? static_cast< uint32_t >(ComErrc::kCommunicationStackError) : 0U,
                trace::FormatValue(""));
            return ret;
        }
        rcode = runtime::VerifyNotification< Desc >(*message);
        if (rcode != Message::kReturnCodeOk) {
            ret = -__LINE__;
            ComLogWarning("handle bind event notification rejected: verify notification invalid",
                          GenArg(Desc::eventId()), GenArg(message), GenArg(rcode));
            MonitorTrace(
                runtime::GetInstance(), trace::userdefine::kRecvEvent, trace::ServiceType::kProxy, Desc::serviceId(),
                message->inst, Desc::serviceIdentifier, Desc::eventId(), "eventName", false, message->hdr.session,
                (ret != Message::kReturnCodeOk) ? static_cast< uint32_t >(ComErrc::kCommunicationStackError) : 0U,
                trace::FormatValue(""));
            return ret;
        }
#ifdef HAS_COM_SECOC
        // secoc signal rx process
        auto secOcSignalDataId{secoc::SecOC4Signal::GetSecOCDataId(
            (message->hdr.serv << (sizeof(Message::ServiceId) * CHAR_BIT)) + message->hdr.method)};
        if (secOcSignalDataId > 0) {
            ret = secoc::SecOC4Signal::SecOCRxHandle(secOcSignalDataId, message.get());
            if (ret < 0) {
                ComLogWarning("invoke SecOC Signal Rx handle", GenArg(ret));
                MonitorTrace(
                    runtime::GetInstance(), trace::userdefine::kRecvEvent, trace::ServiceType::kProxy,
                    Desc::serviceId(), message->inst, Desc::serviceIdentifier, Desc::eventId(), "eventName", false,
                    message->hdr.session,
                    (ret != Message::kReturnCodeOk) ? static_cast< uint32_t >(ComErrc::kCommunicationStackError) : 0U,
                    trace::FormatValue(""));
                return ret;
            }
            ComLogDebug("invoke SecOC Signal Rx handle", GenArg(ret));
        }
        // secoc someip rx process
        auto secOcDataId{secoc::SecOC4SOMEIP::GetSecOCDataId(message->hdr.serv, message->inst, message->hdr.method,
                                                             message->hdr.type)};
        if (secOcDataId > 0) {
            ret = secoc::SecOC4SOMEIP::SecOCRxHandle(secOcDataId, message.get());
            if (ret < 0) {
                ComLogWarning("invoke SecOC Rx handle", GenArg(ret));
                MonitorTrace(
                    runtime::GetInstance(), trace::userdefine::kRecvEvent, trace::ServiceType::kProxy,
                    Desc::serviceId(), message->inst, Desc::serviceIdentifier, Desc::eventId(), "eventName", false,
                    message->hdr.session,
                    (ret != Message::kReturnCodeOk) ? static_cast< uint32_t >(ComErrc::kCommunicationStackError) : 0U,
                    trace::FormatValue(""));
                return ret;
            }
            ComLogDebug("invoke SecOC Rx handle", GenArg(ret));
        }
#else
#endif
        MonitorTrace(runtime::GetInstance(), trace::userdefine::kRecvEvent, trace::ServiceType::kProxy,
                     Desc::serviceId(), message->inst, Desc::serviceIdentifier, Desc::eventId(), "eventName", false,
                     message->hdr.session,
                     (ret != Message::kReturnCodeOk) ? static_cast< uint32_t >(ComErrc::kCommunicationStackError) : 0U,
                     trace::FormatValue(""));
        TimeStop(TimeNM::Timer, "NPCNotify::Verify&SecOC");
        TimeRecorder(TimeNM::Timer, "NPCNotify::HandleRecv");
        EventReceiveHandler receiveHandler;
#ifdef HAS_COM_EVENT_LOST_CHECKER
        SampleLostHandler lostHandler;
        SampleLostStatus lostStatus;
#endif  // HAS_COM_EVENT_LOST_CHECKER
        {
            std::lock_guard< std::mutex > lock(mutex_);
#ifdef HAS_COM_EVENT_LOST_CHECKER
            if (Desc::sessionHandling() == serialize::SessionHandling::kSessionHandlingActive) {
                // If session ID is active, then check increment
                auto sessionId{message->hdr.session};
                if (sessionId == 0) {
                    // Session ID invalid, warning + receive + do not update session
                    ComLogWarning("handle bind event notification: verify message session id == 0",
                                  GenArg(Desc::eventId()), GenArg(message));
                } else if (sessionId_ == 0) {
                    // First time receiving, receive + update session
                    sessionId_ = sessionId;
                } else {
                    // Not first time receiving
                    if (sessionId_ + 1 == 0) {
                        // Session ID reaches maximum value, wrap around
                        ++sessionId_;
                    }
                    auto delta{sessionId - sessionId_};
                    if (delta > 1) {
                        // Session ID too large, warning + receive + update session + notify loss
                        auto lostCount{delta - 1};
                        ComLogWarning("handle bind event notification: verify message > next session id",
                                      GenArg(Desc::eventId()), GenArg(message), GenArg(lostCount));
                        lostStatus_.totalCount += lostCount;
                        lostStatus_.totalCountChange += lostCount;
                        if (lostHandler_) {
                            // Get sample loss handler
                            lostHandler = lostHandler_;
                            // Get sample loss status
                            lostStatus = lostStatus_;
                            // Reset to 0, accumulate subsequently
                            lostStatus_.totalCountChange = 0;
                        }
                    } else if (delta == 1) {
                        // Session ID normal, receive + update session
                    } else if (delta == 0) {
                        // Session ID duplicate, warning + receive + update session
                        ComLogWarning("handle bind event notification: verify message == current session id",
                                      GenArg(Desc::eventId()), GenArg(message));
                    } else {
                        // Session ID rollback, warning + receive + update session
                        ComLogWarning("handle bind event notification: verify message < current session id",
                                      GenArg(Desc::eventId()), GenArg(message), GenArg(sessionId_));
                    }
                    // Update session ID
                    sessionId_ = sessionId;
                }
            }
#endif  // HAS_COM_EVENT_LOST_CHECKER

            // Update notification queue (cache latest)
            ComLogTrace("handle bind event notification: notify bind event enqueue", GenArg(Desc::eventId()),
                        GenArg(queue_.size()), GenArg(message));
            if (!queue_.empty() && (maxSampleCount_ == queue_.size())) {
                queue_.pop_front();
            }
            queue_.emplace_back(std::move(message));
            // Get receive handler
            receiveHandler = receiveHandler_;
        }
        if (receiveHandler) {
            ComLogDebug("handle bind event notification: invoke callback handle notification arrived",
                        GenArg(Desc::eventId()), GenArg(queue_.size()));
            receiveHandler();
            ComLogTrace("handle bind event notification: invoke callback handle notification arrived done",
                        GenArg(Desc::eventId()), GenArg(queue_.size()));
        }
#ifdef HAS_COM_EVENT_LOST_CHECKER
        if (lostHandler) {
            ComLogDebug("handle bind event notification: invoke callback handle sample lost", GenArg(Desc::eventId()));
            lostHandler(lostStatus);
            ComLogTrace("handle bind event notification: invoke callback handle sample lost done",
                        GenArg(Desc::eventId()));
        }
#endif  // HAS_COM_EVENT_LOST_CHECKER
        return 0;
    }
    /// @brief Set event subscription status
    /// @param[in] state Event subscription status
    void _setSubscriptionState(SubscriptionState state) noexcept
    {
        ComLogTrace("set bind event subscription status", GenArg(Desc::eventId()), GenArg(state));
        if (subscriptionState_ == state) {
            return;
        }
        // TODO(jiawei-l2)
        // If service becomes unavailable, need to change subscription status to suspended and automatically renew when service becomes available (confirm if implementation layer already implements auto-renewal)
        // @ref [SWS_CM_00313] [SWS_CM_00314] [SWS_CM_00315]
        subscriptionState_ = state;
        SubscriptionStateChangeHandler stateChangeHandler;
        {
            std::lock_guard< std::mutex > lock(mutex_);
            stateChangeHandler = stateChangeHandler_;
        }
        if (stateChangeHandler) {
            ComLogDebug("set bind event subscription status: invoke callback handle subscription status changed",
                        GenArg(Desc::eventId()), GenArg(state));
            stateChangeHandler(state);
            ComLogTrace("set bind event subscription status: invoke callback handle subscription status changed done",
                        GenArg(Desc::eventId()), GenArg(state));
        }
        if (SubscriptionState::kSubscribed == subscriptionState_) {
            MonitorTrace(runtime::GetInstance(), trace::userdefine::kEventSubscribe, trace::ServiceType::kProxy,
                         Desc::serviceId(), owner_.instanceId_, Desc::serviceIdentifier, Desc::eventId(), "eventName",
                         this->IsField(), 0);
        }
    }
    /// @brief Deserialize message to value and return number of bytes read or error code
    /// @tparam c Specialization condition -- whether it is signal-based (false)
    /// @param[in] message Message
    /// @param[in] value Sample value
    /// @return Result -- >= 0: number of bytes read; < 0: error code;
    template < bool c = signal::isSignalBased< Desc >() >
    int32_t _deserialize(npc_message_t const& message, SampleType& value, std::enable_if_t< !c >* = nullptr) noexcept
    {
        return runtime::Deserialize< Desc >(message, value);
    }
    /// @brief Deserialize message to value and return number of bytes read or error code
    /// @tparam c Specialization condition -- whether it is signal-based (true)
    /// @param[in] message Message
    /// @param[in] value Sample value
    /// @return Result -- >= 0: number of bytes read; < 0: error code;
    template < bool c = signal::isSignalBased< Desc >() >
    int32_t _deserialize(npc_message_t const& message, SampleType& value, std::enable_if_t< c >* = nullptr) noexcept
    {
        signal::S2S< signal::headerId< Desc >() > wrapper{value};
        return runtime::Deserialize< Desc >(message, wrapper);
    }
    /// @brief Deserialize E2E payload message to value and return number of bytes read or error code
    /// @tparam c Specialization condition -- whether it is signal-based (false)
    /// @param[in] message Message
    /// @param[in] value Sample value
    /// @return Result -- >= 0: number of bytes read; < 0: error code;
    template < bool c = signal::isSignalBased< Desc >() >
    int32_t _deserialize(serialize::E2EPayload const& message,
                         SampleType& value,
                         std::enable_if_t< !c >* = nullptr) noexcept
    {
        return runtime::Deserialize< Desc >(message, value);
    }
    /// @brief Deserialize E2E payload message to value and return number of bytes read or error code
    /// @tparam c Specialization condition -- whether it is signal-based (true)
    /// @param[in] message Message
    /// @param[in] value Sample value
    /// @return Result -- >= 0: number of bytes read; < 0: error code;
    template < bool c = signal::isSignalBased< Desc >() >
    int32_t _deserialize(serialize::E2EPayload const& message,
                         SampleType& value,
                         std::enable_if_t< c >* = nullptr) noexcept
    {
        signal::S2S< signal::headerId< Desc >() > wrapper{value};
        return runtime::Deserialize< Desc >(message, wrapper);
    }

private:
    /// @brief Instance identifier
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
    /// @brief Maximum number of samples to receive
    size_t maxSampleCount_{1};
    /// @brief Type alias -- receive notification queue
    using Queue = std::deque< std::shared_ptr< npc_message_t > >;
    /// @brief Receive notification queue
    Queue queue_;
    /// @brief Receive handler
    EventReceiveHandler receiveHandler_;
    /// @brief Subscription status change handler
    SubscriptionStateChangeHandler stateChangeHandler_;
    /// @brief Subscription status -- current
    SubscriptionState subscriptionState_{SubscriptionState::kNotSubscribed};
#ifdef HAS_COM_EVENT_LOST_CHECKER
    /// @brief Sample loss handler
    SampleLostHandler lostHandler_;
    /// @brief Sample loss status
    SampleLostStatus lostStatus_;
    /// @brief Session identifier
    Message::SessionId sessionId_{};
#endif  // HAS_COM_EVENT_LOST_CHECKER
    /// @brief Mutex
    std::mutex mutex_;
};
}  // namespace proxy
}  // namespace npc
}  // namespace internal
}  // namespace com
}  // namespace ara
#endif