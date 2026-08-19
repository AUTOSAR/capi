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
/// @file       icc_event.h
/// @brief      Binding layer service skeleton event header file
/// @details
/// @date       2021-12-31
/// @author     mazelin
/// @version    1.2.0
///
/// ================================================================

#ifndef __COM_ICC_SKELETON_EVENT_H
#define __COM_ICC_SKELETON_EVENT_H

#include "../icc_runtime.h"
#include "ara/com/internal/skeleton/event.h"

/// @brief Namespace -- internal binding layer skeleton
namespace ara {
namespace com {
namespace internal {
namespace icc {
namespace skeleton {
/// @brief Template type -- binding layer service skeleton event
/// @code{.isoft}
/// export_level=/COM/SoC/SOA
/// @endcode
/// @tparam Owner Service element owner
/// @tparam Desc Description information
/// @tparam T Event data type
template < typename Owner, typename Desc, typename T >
class IccEvent : public virtual ara::com::internal::skeleton::EventBase< T >
{
public:
    /// @brief Type alias -- event base type
    using EventBase = ara::com::internal::skeleton::EventBase< T >;
    /// @brief Type alias -- sample data type
    using SampleType = typename EventBase::SampleType;
    /// @brief Constructor
    /// @tparam c Specialization condition -- whether it is an S2S conversion target (no)
    /// @param[in] owner Service element owner
    template < bool c = signal::isSignalBased< Desc >() && signal::isS2STarget< Desc >() >
    explicit IccEvent(Owner& owner, std::enable_if_t< !c >* = nullptr) noexcept : owner_{owner}
    {
        static_assert(Message::IsEventId(Desc::eventId()), "invalid event id");
        ComLogTrace("create bind skeleton event", GenArg(Desc::eventId()));
    }
    /// @brief Constructor
    /// @tparam c Specialization condition -- whether it is an S2S conversion target (yes)
    /// @param[in] owner Service element owner
    template < bool c = signal::isSignalBased< Desc >() && signal::isS2STarget< Desc >() >
    explicit IccEvent(Owner& owner, std::enable_if_t< c >* = nullptr) noexcept : owner_{owner}
    {
        static_assert(Message::IsEventId(Desc::eventId()), "invalid event id");
        ComLogTrace("create bind skeleton event", GenArg(Desc::eventId()));
        signal::S2S< signal::headerId< Desc >() >::Sender() = [this](auto& value) { return this->SendImpl(value); };
        ComLogTrace("create bind skeleton event done", GenArg(Desc::eventId()));
    }
    /// @brief Destructor
    ~IccEvent() noexcept override { ComLogTrace("destroy bind skeleton event", GenArg(Desc::eventId())); }
    /// @brief Copy constructor
    /// @param other
    IccEvent(IccEvent const& other) noexcept = default;
    /// @brief Move constructor
    /// @param other
    IccEvent(IccEvent&& other) noexcept = default;
    /// @brief Copy assignment operator
    /// @param other
    /// @return IccEvent
    IccEvent& operator=(IccEvent const& other) noexcept = default;
    /// @brief Move assignment operator
    /// @param other
    /// @return IccEvent
    IccEvent& operator=(IccEvent&& other) noexcept = default;
    /// @brief Offer event notification -- call after offering service
    /// @return Result object -- empty/value or error
    ara::core::Result< void > OfferNotification() noexcept
    {
        using Result = ara::core::Result< void >;
        ComLogTrace("offer bind event", GenArg(Desc::eventId()));
        icc_event_info_t ev{};
        ev.reliability = ICC_RT_UNKNOWN;  // TODO(jiawei-l5) Server-side event reliability is currently only configured via config
        ev.type        = this->IsField() ? ICC_ET_FIELD : ICC_ET_EVENT;
        auto eventGroups{Desc::eventGroups()};
        ev.groups = const_cast< decltype(ev.groups) >(eventGroups.data());
        ev.ngroup = static_cast< decltype(ev.ngroup) >(eventGroups.size());
        auto& runtime{runtime::GetInstance()};
        auto ret{icc_app_offer_event(runtime.GetApp(), Desc::serviceId(), owner_.instanceId_, Desc::eventId(), &ev)};
        if (ret < 0) {
            ComLogError("offer bind event error: invoke stack offer bind event failed", GenArg(ret),
                        GenArg(Desc::eventId()));
            return Result{MakeErrorCode(ComErrc::kCommunicationStackError, ret)};
        }
        ComLogDebug("offer bind event done", GenArg(Desc::eventId()));
        return Result{};
    }
    /// @brief Stop offering event notification -- call before stopping service offering
    /// @return Result object -- empty/value or error
    ara::core::Result< void > StopOfferNotification() noexcept
    {
        using Result = ara::core::Result< void >;
        ComLogTrace("stop offer bind event", GenArg(Desc::eventId()));
        auto& runtime{runtime::GetInstance()};
        auto ret{icc_app_stop_event(runtime.GetApp(), Desc::serviceId(), owner_.instanceId_, Desc::eventId())};
        if (ret < 0) {
            ComLogError("stop offer bind event error: invoke stack stop event failed", GenArg(ret),
                        GenArg(Desc::eventId()));
            return Result{MakeErrorCode(ComErrc::kCommunicationStackError, ret)};
        }
        ComLogDebug("stop offer bind event done", GenArg(Desc::eventId()));
        return Result{};
    }
    /// @brief Send sample
    /// @param[in] sample Sample data
    /// @return Result object -- empty/value or error
    ara::core::Result< void > Send(SampleAllocateePtr< SampleType > sample) noexcept override
    {
        return Send(*sample);  // TODO(jiawei-l5) use share memory from sample
    }
    /// @brief Send sample
    /// @param[in] sample Sample data
    /// @return Result object -- empty/value or error
    ara::core::Result< void > Send(SampleType const& sample) noexcept override { return DoSend(sample); }
    /// @brief Send sample
    /// @tparam c Specialization condition -- whether based on signal (no)
    /// @param[in] sample Sample data
    /// @return Result object -- empty/value or error
    template < bool c = signal::isSignalBased< Desc >() >
    ara::core::Result< void > DoSend(SampleType const& sample, std::enable_if_t< !c >* = nullptr) noexcept
    {
        return SendImpl(sample);
    }
    /// @brief Send sample
    /// @tparam c Specialization condition -- whether based on signal (yes)
    /// @param[in] sample Sample data
    /// @return Result object -- empty/value or error
    template < bool c = signal::isSignalBased< Desc >() >
    ara::core::Result< void > DoSend(SampleType const& sample, std::enable_if_t< c >* = nullptr) noexcept
    {
        return signal::S2S< signal::headerId< Desc >() >::Send(sample);
    }
    /// @brief Send sample
    /// @param[in] sample Sample data
    /// @return Result object -- empty/value or error
    ara::core::Result< void > SendImpl(SampleType const& sample) noexcept
    {
        TimeRecorder(TimeNM::Timer, "ICCNotify::Send(value)");
        using Result = ara::core::Result< void >;
        ComLogTrace("notify bind event", GenArg(Desc::eventId()));
        auto& runtime{runtime::GetInstance()};
        Message::SessionId sessionId{};
        {
            std::lock_guard< std::mutex > lock(mutex_);
            if (Desc::sessionHandling() == serialize::SessionHandling::kSessionHandlingActive) {
                sessionId = ++sessionId_ != 0 ? sessionId_ : ++sessionId_;
            }
        }
        auto notification{runtime::CreateNotification< Desc >(runtime.GetApp(), owner_.instanceId_, sessionId, sample)};
        if (notification == nullptr) {
            ComLogError("notify bind event error: create notification failed", GenArg(Desc::eventId()));
            return Result{MakeErrorCode(ComErrc::kCommunicationStackError, -__LINE__)};
        }
        auto ret{runtime.Send(notification)};
        if (ret < 0) {
            ComLogWarning("notify bind event error: send notification failed", GenArg(ret), GenArg(Desc::eventId()));
            return Result{MakeErrorCode(ComErrc::kNetworkBindingFailure, ret)};
        }
        ComLogDebug("notify bind event done", GenArg(Desc::eventId()));
        return Result{};
    }

private:
    /// @brief Service element owner
    Owner& owner_;
    /// @brief Session identifier
    Message::SessionId sessionId_{};
    /// @brief Mutex
    std::mutex mutex_;
};
}  // namespace skeleton
}  // namespace icc
}  // namespace internal
}  // namespace com
}  // namespace ara
#endif
