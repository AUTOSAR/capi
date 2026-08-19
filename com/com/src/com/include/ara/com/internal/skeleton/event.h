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
/// @file       event.h
/// @brief      Communication service skeleton event header file
/// @details
/// @date       2021-12-31
/// @author     mazelin
/// @version    1.2.0
///
/// ================================================================

#pragma once

#include "ara/com/types.h"

/// @brief Namespace -- Internal skeleton
namespace ara {
namespace com {
namespace internal {
namespace skeleton {
/// @brief Template type -- Service skeleton event
/// @tparam T Event data type
template < typename T >
class Event;

/// @brief Template type -- Check if it is an event type (No)
/// @tparam T
template < typename T >
struct IsEvent : std::false_type
{
};
/// @brief Template type -- Check if it is an event type (Yes)
/// @tparam T
template < typename T >
struct IsEvent< Event< T > > : std::true_type
{
};

/// @brief Template type -- Event base type
/// @code{.isoft}
/// export_level=/COM/SoC/SOA
/// @endcode
/// @tparam T Event data type
template < typename T >
class EventBase
{
public:
    /// @brief Type alias -- Sample data type
    using SampleType = std::remove_cv_t< std::remove_reference_t< T > >;
    /// @brief Constructor
    EventBase() noexcept { ComLogTrace(""); }
    /// @brief Destructor
    virtual ~EventBase() noexcept { ComLogTrace(""); }
    /// @brief Copy constructor
    /// @param other
    EventBase(EventBase const& other) noexcept = default;
    /// @brief Move constructor
    /// @param other
    EventBase(EventBase&& other) noexcept = default;
    /// @brief Copy assignment operator
    /// @param other
    /// @return EventBase
    EventBase& operator=(EventBase const& other) noexcept = default;
    /// @brief Move assignment operator
    /// @param other
    /// @return EventBase
    EventBase& operator=(EventBase&& other) noexcept = default;
    /// @brief Allocate sample
    /// @return Result object -- empty/value or error
    virtual ara::core::Result< SampleAllocateePtr< SampleType > > Allocate() noexcept
    {
        using Result = ara::core::Result< SampleAllocateePtr< SampleType > >;
        ComLogTrace("allocate bind event sample");
        // TODO(jiawei-l6) SampleAllocateePtr & Allocate implements object pool
        SampleAllocateePtr< SampleType > sample{new SampleType{}};
        return sample ? Result{sample} : Result{MakeErrorCode(ComErrc::kSampleAllocationFailure, -__LINE__)};
    }
    /// @brief Send event
    /// @param[in] sample Sample pointer
    /// @return Result object -- empty/value or error
    virtual ara::core::Result< void > Send(SampleAllocateePtr< SampleType > sample) noexcept = 0;
    /// @brief Send event
    /// @param[in] sample Sample data
    /// @return Result object -- empty/value or error
    virtual ara::core::Result< void > Send(SampleType const& sample) noexcept = 0;

protected:  ///< @note Used for fields
    /// @brief Check if it is a field
    /// @return bool
    virtual bool IsField() const noexcept  // NOLINT -- Virtual function naming convention > protected member function naming convention
    {
        return false;
    }
};

/// @brief Template type -- Event type
/// @code{.isoft}
/// export_level=/COM/SoC/SOA
/// @endcode
/// @tparam T Event data type
template < typename T >
class Event
{
public:
    /// @brief Type alias -- Event implementation type
    using EventImpl = EventBase< T >;
    /// @brief Type alias -- Sample data type
    using SampleType = typename EventImpl::SampleType;
    /// @brief Constructor
    Event() noexcept { ComLogTrace("create service skeleton event"); }
    /// @brief Destructor
    ~Event() noexcept
    {
        ComLogTrace("destroy service skeleton event");
        std::lock_guard< std::mutex > lock(mutex_);
        impls_.clear();
    }
    /// @brief Copy constructor
    /// @param[in] other
    Event(Event const& other) noexcept = delete;
    /// @brief Copy assignment operator
    /// @param[in] other
    /// @return Event&
    Event& operator=(Event const& other) noexcept = delete;
    /// @brief Move constructor
    /// @param[in] other
    Event(Event&& other) noexcept : lock_{other.mutex_}, impls_ { std::move(other.impls_) }
#ifdef HAS_COM_EVENT_SEND_TRIGGER
    , sample_{std::move(other.sample_)}, sendContext_ { std::move(other.sendContext_) }
#endif  // HAS_COM_EVENT_SEND_TRIGGER
    {
        ComLogTrace("");
        other.impls_.clear();
        lock_.unlock();
    }
    /// @brief Move assignment operator
    /// @param[in] other
    /// @return Event&
    Event& operator=(Event&& other) noexcept
    {
        ComLogTrace("");
        if (this != &other) {
            std::lock(mutex_, other.mutex_);
            std::lock_guard< std::mutex > lock1(mutex_, std::adopt_lock);
            std::lock_guard< std::mutex > lock2(other.mutex_, std::adopt_lock);
            impls_ = std::move(other.impls_);
            other.impls_.clear();
#ifdef HAS_COM_EVENT_SEND_TRIGGER
            sample_      = std::move(other.sample_);
            sendContext_ = std::move(other.sendContext_);
#endif  // HAS_COM_EVENT_SEND_TRIGGER
        }
        return *this;
    }
    /// @brief Operator -- increment
    /// @param[in] impl
    /// @return Event&
    Event& operator+=(std::shared_ptr< EventImpl >&& impl) noexcept
    {
        ComLogTrace("add bind skeleton event");
        std::lock_guard< std::mutex > lock(mutex_);
        impls_.emplace_back(std::move(impl));
        return *this;
    }
    /// @brief Allocate sample
    /// @code{.isoft}
    /// export_level=/COM/SoC/SOA
    /// @endcode
    /// @return Result object -- empty/value or error
    /// @retval ComErrc::kSampleAllocationFailure Sample allocation failure (i.e., shared memory allocation/acquisition failed)
    /// @retval ComErrc::kIllegalUseOfAllocate Illegal use of allocate (i.e., using custom allocator instead of shared memory and failing)
    /// @ref [SWS_CM_90438] -- Allocating data for event transfer
    /// @ref [SWS_CM_00013] -- Re-entrancy – Allocate
    ara::core::Result< SampleAllocateePtr< SampleType > > Allocate() noexcept
    {
        TimeRecorder(TimeNM::Timer, "APINotify::Allocate");
        using Result = ara::core::Result< SampleAllocateePtr< SampleType > >;
        ComLogTrace("allocate service event sample");
        std::lock_guard< std::mutex > lock(mutex_);
        if (impls_.empty()) {
            ComLogError("allocate service event sample rejected: bind event not exist");
            return Result{MakeErrorCode(ComErrc::kCommunicationStackError, -__LINE__)};
        }
        Result result{{}};
        for (auto& impl : impls_) {
            auto res{impl->Allocate()};
            if (!res || res.Value()) {
                result = std::move(res);
                break;
            }
        }
        return result;
    }
    /// @brief Send event
    /// @code{.isoft}
    /// export_level=/COM/SoC/SOA
    /// @endcode
    /// @param[in] sample Sample pointer
    /// @return Result object -- empty/value or error
    /// @retval ComErrc::kServiceNotOffered Service not offered
    /// @retval ComErrc::kCommunicationLinkError Communication link error
    /// @retval ComErrc::kNoClients No connected clients
    /// @retval ComErrc::kCommunicationStackError Communication stack error (e.g., network stack, network binding, or communication framework error)
    /// @ref [SWS_CM_90437] -- Send event where Communication Management is responsible for the data
    /// @ref [SWS_CM_00012] -- Re-entrancy – Send
    ara::core::Result< void > Send(SampleAllocateePtr< SampleType > sample) noexcept
    {
        TimeRecorder(TimeNM::Timer, "APINotify::Send(pointer)");
        using Result = ara::core::Result< void >;
        ComLogTrace("send service event sample", GenArg(sample));
        std::lock_guard< std::mutex > lock(mutex_);
        if (impls_.empty()) {
            ComLogError("send service event sample rejected: bind event not exist", GenArg(sample));
            return Result{MakeErrorCode(ComErrc::kCommunicationStackError, -__LINE__)};
        }
        Result result{};
#ifdef HAS_COM_EVENT_SEND_TRIGGER
        if (sample == nullptr) {
            // Send sample is empty
            if (sample_ == nullptr) {
                // Internal sample does not exist, create sample
                for (auto& impl : impls_) {
                    auto sampleRes{impl->Allocate()};
                    if (!sampleRes) {
                        ComLogError("send service event sample error: allocate sample failed", GenArg(sampleRes));
                        return Result{std::move(sampleRes).Error()};
                    }
                    sample = std::move(sampleRes).Value();
                    ComLogDebug("send service event sample: use allocate sample", GenArg(sample));
                    break;
                }
            } else {
                // Internal sample exists, use internal sample
                sample = sample_;
                ComLogDebug("send service event sample: reuse internal sample", GenArg(sample));
            }
        }
        if (sample_ == nullptr) {
            // Internal sample does not exist, use send sample
            sample_ = sample;
            ComLogDebug("send service event sample: init internal sample", GenArg(sample_));
        }
        if (sendContext_ != nullptr) {
            // Send context exists
            bool same{false};
            if (sample.get() == sample_.get()) {
                // Internal sample reference
                same = true;
                if (sendContext_->sendTrigger.updater != nullptr) {
                    // Sample updater exists, callback
                    sendContext_->sendTrigger.updater(*sample_);
                    ComLogDebug("send service event sample: invoke callback update internal sample", GenArg(sample_));
                }
            } else {
                // Non-internal sample reference
                if (sendContext_->sendTrigger.comparator != nullptr) {
                    // Sample comparator exists, callback
                    same = sendContext_->sendTrigger.comparator(*sample, *sample_);
                    ComLogDebug("send service event sample: invoke callback compare sample", GenArg(same));
                }
            }
            auto now{std::chrono::steady_clock::now()};
            if (same) {
                if (sendContext_->sendTrigger.minimumSendInterval.count() == 0) {
                    // Samples are the same and minimum send interval does not exist, do not send
                    ComLogDebug("send service event sample ignored: send same", GenArg(sendContext_));
                    return result;
                }
                if (now - sendContext_->last < sendContext_->sendTrigger.minimumSendInterval) {
                    // Samples are the same and minimum send interval exists but not exceeded, do not send
                    ComLogDebug("send service event sample ignored: send same and too fast", GenArg(sendContext_));
                    return result;
                }
            }
            if (sendContext_->sendTrigger.dataUpdatePeriod.count() != 0
                && sendContext_->sendTrigger.resetUpdatePeriod) {
                // Periodic send exists and send reset interval allowed, reset timer
                isoft::naicpp::GlobalGeneralEvLoop::Get()->MakeTimer(
                    sendContext_->timer, sendContext_->sendTrigger.dataUpdatePeriod.count(), [this]() {
                        // Periodic send callback, update timer
                        sendContext_->timer->UpdateTime(sendContext_->sendTrigger.dataUpdatePeriod.count());
                        ComLogTrace("cyclic send service event sample", GenArg(sample_));
                        auto sendRes{Send(sample_)};
                        if (!sendRes) {
                            ComLogWarning("cyclic send service event sample error", GenArg(sendRes), GenArg(sample_));
                            return;
                        }
                        ComLogDebug("cyclic send service event sample done", GenArg(sample_));
                    });
            }
            // Update send time
            sendContext_->last = now;
        }
        // Update internal sample pointer
        sample_ = sample;
#endif  // HAS_COM_EVENT_SEND_TRIGGER
        for (auto& impl : impls_) {
            auto res{impl->Send(sample)};
            if (!res) {
                result = std::move(res);
            }
        }
        return result;
    }
    /// @brief Send event
    /// @code{.isoft}
    /// export_level=/COM/SoC/SOA
    /// @endcode
    /// @param[in] sample Sample value
    /// @return Result object -- empty/value or error
    /// @retval ComErrc::kServiceNotOffered Service not offered
    /// @retval ComErrc::kCommunicationLinkError Communication link error
    /// @retval ComErrc::kNoClients No connected clients
    /// @retval ComErrc::kCommunicationStackError Communication stack error (e.g., network stack, network binding, or communication framework error)
    /// @ref [SWS_CM_00162] -- Send event where application is responsible for the data
    /// @ref [SWS_CM_00012] -- Re-entrancy – Send
    ara::core::Result< void > Send(SampleType const& sample) noexcept
    {
        TimeRecorder(TimeNM::Timer, "APINotify::Send(value)");
        using Result = ara::core::Result< void >;
        ComLogTrace("send service event sample", GenArg(sample));
        std::lock_guard< std::mutex > lock(mutex_);
        if (impls_.empty()) {
            ComLogError("send service event sample rejected: bind event not exist", GenArg(sample));
            return Result{MakeErrorCode(ComErrc::kCommunicationStackError, -__LINE__)};
        }
        Result result{};
#ifdef HAS_COM_EVENT_SEND_TRIGGER
        if (sample_ == nullptr) {
            // Internal sample does not exist, create
            for (auto& impl : impls_) {
                auto sampleRes{impl->Allocate()};
                if (!sampleRes) {
                    ComLogError("send service event sample error: allocate sample failed", GenArg(sampleRes));
                    return Result{std::move(sampleRes).Error()};
                }
                sample_ = std::move(sampleRes).Value();
                ComLogDebug("send service event sample: init internal sample", GenArg(sample_));
                break;
            }
        }
        if (sendContext_ != nullptr) {
            // Send context exists
            bool same{false};
            if (&sample == sample_.get()) {
                // Internal sample reference
                same = true;
                if (sendContext_->sendTrigger.updater != nullptr) {
                    // Sample updater exists, callback
                    sendContext_->sendTrigger.updater(*sample_);
                    ComLogDebug("send service event sample: invoke callback update internal sample", GenArg(sample_));
                }
            } else {
                // Non-internal sample reference
                if (sendContext_->sendTrigger.comparator != nullptr) {
                    // Sample comparator exists, callback
                    same = sendContext_->sendTrigger.comparator(sample, *sample_);
                    ComLogDebug("send service event sample: invoke callback compare sample", GenArg(same));
                }
            }
            auto now{std::chrono::steady_clock::now()};
            if (same) {
                if (sendContext_->sendTrigger.minimumSendInterval.count() == 0) {
                    // Samples are the same and minimum send interval does not exist, do not send
                    ComLogDebug("send service event sample ignored: send same", GenArg(sendContext_));
                    return result;
                }
                if (now - sendContext_->last < sendContext_->sendTrigger.minimumSendInterval) {
                    // Samples are the same and minimum send interval exists but not exceeded, do not send
                    ComLogDebug("send service event sample ignored: send same and too fast", GenArg(sendContext_));
                    return result;
                }
            }
            if (sendContext_->sendTrigger.dataUpdatePeriod.count() != 0
                && sendContext_->sendTrigger.resetUpdatePeriod) {
                // Periodic send exists and send reset interval allowed, reset timer
                isoft::naicpp::GlobalGeneralEvLoop::Get()->MakeTimer(
                    sendContext_->timer, sendContext_->sendTrigger.dataUpdatePeriod.count(), [this]() {
                        // Periodic send callback, update timer
                        sendContext_->timer->UpdateTime(sendContext_->sendTrigger.dataUpdatePeriod.count());
                        ComLogTrace("cyclic send service event sample", GenArg(sample_));
                        auto sendRes{Send(sample_)};
                        if (!sendRes) {
                            ComLogWarning("cyclic send service event sample error", GenArg(sendRes), GenArg(sample_));
                            return;
                        }
                        ComLogDebug("cyclic send service event sample done", GenArg(sample_));
                    });
            }
            // Update send time
            sendContext_->last = now;
        }
        // Update internal sample value
        *sample_ = sample;
#endif  // HAS_COM_EVENT_SEND_TRIGGER
        for (auto& impl : impls_) {
            auto res{impl->Send(sample)};
            if (!res) {
                result = std::move(res);
            }
        }
        return result;
    }
#ifdef HAS_COM_EVENT_SEND_TRIGGER
    /// @brief Set send trigger
    /// @code{.isoft}
    /// export_level=/COM/SoC/SOA
    /// @endcode
    /// @param[in] sendTrigger Send trigger
    /// @return Result object -- empty/value or error
    /// @retval ComErrc::kServiceNotOffered Service not offered
    /// @retval ComErrc::kCommunicationLinkError Communication link error
    /// @retval ComErrc::kNoClients No connected clients
    /// @retval ComErrc::kCommunicationStackError Communication stack error (e.g., network stack, network binding, or communication framework error)
    /// @ref [TPS_MANI_03210] -- Specification of event specific communication attributes
    /// @ref [TPS_MANI_03210] -- Specification of field specific communication attributes
    /// @ref [TPS_MANI_03212] -- Specification of initial value for a field
    /// @ref [TPS_MANI_01107] -- Specification of intentions for the sender of events or field notifiers
    ara::core::Result< void > SetSendTrigger(SendTrigger< SampleType > sendTrigger) noexcept
    {
        using Result = ara::core::Result< void >;
        ComLogTrace("set service event send trigger", GenVal(sendTrigger));
        std::lock_guard< std::mutex > lock(mutex_);
        Result result{};
        if (sendContext_ == nullptr) {
            // Send context does not exist, create
            sendContext_ = std::make_unique< SendContext< SampleType > >();
            ComLogDebug("set service event send trigger: init send context", GenVal(sendContext_));
        }
        sendContext_->sendTrigger = std::move(sendTrigger);
        if (sendContext_->sendTrigger.dataUpdatePeriod.count() == 0) {
            // Periodic send interval does not exist, reset timer
            sendContext_->timer.reset();
        } else {
            // Periodic send interval exists, set timer
            isoft::naicpp::GlobalGeneralEvLoop::Get()->MakeTimer(
                sendContext_->timer, sendContext_->sendTrigger.dataUpdatePeriod.count(), [this]() {
                    // Update timer
                    sendContext_->timer->UpdateTime(sendContext_->sendTrigger.dataUpdatePeriod.count());
                    ComLogTrace("cyclic send service event sample", GenArg(sample_));
                    auto sendRes{Send(sample_)};
                    if (!sendRes) {
                        ComLogWarning("cyclic send service event sample error", GenArg(sendRes), GenArg(sample_));
                        return;
                    }
                    ComLogDebug("cyclic send service event sample done", GenArg(sample_));
                });
        }
        ComLogDebug("set service event send trigger done", GenVal(sendContext_));
        return result;
    }
#endif  // HAS_COM_EVENT_SEND_TRIGGER

private:
    /// @brief Mutex
    mutable std::mutex mutex_;
    /// @brief Unique lock
    std::unique_lock< std::mutex > lock_;
    /// @brief Set of event implementations
    std::list< std::shared_ptr< EventImpl > > impls_;
#ifdef HAS_COM_EVENT_SEND_TRIGGER
    /// @brief Sample cache
    SampleAllocateePtr< SampleType > sample_;
    /// @brief Send context
    std::unique_ptr< SendContext< SampleType > > sendContext_;
#endif  // HAS_COM_EVENT_SEND_TRIGGER
};
}  // namespace skeleton
}  // namespace internal
}  // namespace com
}  // namespace ara