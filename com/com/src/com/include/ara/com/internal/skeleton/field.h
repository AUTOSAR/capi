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
/// @file       field.h
/// @brief      Communication service skeleton field header file
/// @details
/// @date       2021-12-31
/// @author     mazelin
/// @version    1.2.0
///
/// ================================================================

#pragma once

#include "ara/core/promise.h"
#include "event.h"

/// @brief Namespace -- Internal skeleton
namespace ara {
namespace com {
namespace internal {
namespace skeleton {
/// @brief Field mode enumeration
/// @code{.isoft}
/// export_level=/COM/SoC/SOA
/// @endcode
enum FieldMode
{
    kN   = 1 << 0,       ///< Notifiable
    kS   = 1 << 1,       ///< Settable
    kG   = 1 << 2,       ///< Gettable
    kNS  = kN | kS,      ///< Notifiable/Settable
    kNG  = kN | kG,      ///< Notifiable/Gettable
    kSG  = kS | kG,      ///< Settable/Gettable
    kNSG = kN | kS | kG  ///< Notifiable/Settable/Gettable
};

/// @brief Template type -- Service skeleton field -- Declaration
/// @tparam T Field data type
/// @tparam mode Field mode -- Default Notifiable/Settable/Gettable
template < typename T, FieldMode mode = FieldMode::kNSG >
class Field;

/// @brief Type alias -- Notifiable field
template < typename T >
using NField = Field< T, FieldMode::kN >;
/// @brief Type alias -- Settable field
template < typename T >
using SField = Field< T, FieldMode::kS >;
/// @brief Type alias -- Gettable field
template < typename T >
using GField = Field< T, FieldMode::kG >;
/// @brief Type alias -- Notifiable/Settable field
template < typename T >
using NSField = Field< T, FieldMode::kNS >;
/// @brief Type alias -- Notifiable/Gettable field
template < typename T >
using NGField = Field< T, FieldMode::kNG >;
/// @brief Type alias -- Settable/Gettable field
template < typename T >
using SGField = Field< T, FieldMode::kSG >;
/// @brief Type alias -- Notifiable/Settable/Gettable field
template < typename T >
using NSGField = Field< T, FieldMode::kNSG >;

/// @brief Template type -- Check if it is a field type (No)
/// @tparam T
template < typename T >
struct IsField : std::false_type
{
};
/// @brief Template type -- Check if it is a field type (Yes)
/// @tparam T
template < typename T, FieldMode mode >
struct IsField< Field< T, mode > > : std::true_type
{
};
/// @brief Template type -- Check if it is a notifiable field type (No)
/// @tparam T
template < typename T >
struct IsNField : std::false_type
{
};
/// @brief Template type -- Check if it is a notifiable field type (Yes)
/// @tparam T
template < typename T >
struct IsNField< NField< T > > : std::true_type
{
};
/// @brief Template type -- Check if it is a settable field type (No)
/// @tparam T
template < typename T >
struct IsSField : std::false_type
{
};
/// @brief Template type -- Check if it is a settable field type (Yes)
/// @tparam T
template < typename T >
struct IsSField< SField< T > > : std::true_type
{
};
/// @brief Template type -- Check if it is a gettable field type (No)
/// @tparam T
template < typename T >
struct IsGField : std::false_type
{
};
/// @brief Template type -- Check if it is a gettable field type (Yes)
/// @tparam T
template < typename T >
struct IsGField< GField< T > > : std::true_type
{
};
/// @brief Template type -- Check if it is a notifiable/settable field type (No)
/// @tparam T
template < typename T >
struct IsNSField : std::false_type
{
};
/// @brief Template type -- Check if it is a notifiable/settable field type (Yes)
/// @tparam T
template < typename T >
struct IsNSField< NSField< T > > : std::true_type
{
};
/// @brief Template type -- Check if it is a notifiable/gettable field type (No)
/// @tparam T
template < typename T >
struct IsNGField : std::false_type
{
};
/// @brief Template type -- Check if it is a notifiable/gettable field type (Yes)
/// @tparam T
template < typename T >
struct IsNGField< NGField< T > > : std::true_type
{
};
/// @brief Template type -- Check if it is a settable/gettable field type (No)
/// @tparam T
template < typename T >
struct IsSGField : std::false_type
{
};
/// @brief Template type -- Check if it is a settable/gettable field type (Yes)
/// @tparam T
template < typename T >
struct IsSGField< SGField< T > > : std::true_type
{
};
/// @brief Template type -- Check if it is a notifiable/settable/gettable field type (No)
/// @tparam T
template < typename T >
struct IsNSGField : std::false_type
{
};
/// @brief Template type -- Check if it is a notifiable/settable/gettable field type (Yes)
/// @tparam T
template < typename T >
struct IsNSGField< NSGField< T > > : std::true_type
{
};

/// @brief Check if notifiable
/// @tparam T Field data type
/// @return bool
template < typename T >
constexpr bool EnableNField() noexcept
{
    return IsNField< T >::value || IsNSField< T >::value || IsNGField< T >::value || IsNSGField< T >::value;
}
/// @brief Check if settable
/// @tparam T Field data type
/// @return bool
template < typename T >
constexpr bool EnableSField() noexcept
{
    return IsSField< T >::value || IsNSField< T >::value || IsSGField< T >::value || IsNSGField< T >::value;
}
/// @brief Check if gettable
/// @tparam T Field data type
/// @return bool
template < typename T >
constexpr bool EnableGField() noexcept
{
    return IsGField< T >::value || IsNGField< T >::value || IsSGField< T >::value || IsNSGField< T >::value;
}

/// @brief Check if has a notifier
/// @param[in] mode Field mode
/// @return bool
constexpr bool HasNotifier(FieldMode mode) noexcept { return (mode & FieldMode::kN) != 0; }
/// @brief Check if has a setter
/// @param[in] mode Field mode
/// @return bool
constexpr bool HasSetter(FieldMode mode) noexcept { return (mode & FieldMode::kS) != 0; }
/// @brief Check if has a getter
/// @param[in] mode Field mode
/// @return bool
constexpr bool HasGetter(FieldMode mode) noexcept { return (mode & FieldMode::kG) != 0; }

/// @brief Template type -- Notifiable field base type
/// @code{.isoft}
/// export_level=/COM/SoC/SOA
/// @endcode
/// @tparam T Field data type
/// @tparam bool Notifier exists by default
template < typename T, bool = true >
class NFieldBase : public virtual EventBase< T >
{
public:
    /// @brief Type alias -- Field data type
    using FieldType = std::remove_cv_t< std::remove_reference_t< T > >;
    /// @brief Constructor
    NFieldBase() noexcept { ComLogTrace(""); }
    /// @brief Destructor
    ~NFieldBase() noexcept override { ComLogTrace(""); }
    /// @brief Copy constructor
    /// @param other
    NFieldBase(NFieldBase const& other) noexcept = default;
    /// @brief Move constructor
    /// @param other
    NFieldBase(NFieldBase&& other) noexcept = default;
    /// @brief Copy assignment operator
    /// @param other
    /// @return NFieldBase
    NFieldBase& operator=(NFieldBase const& other) noexcept = default;
    /// @brief Move assignment operator
    /// @param other
    /// @return NFieldBase
    NFieldBase& operator=(NFieldBase&& other) noexcept = default;

protected:  ///< @note Used for fields
    /// @brief Check if it is a field
    /// @return bool
    bool IsField() const noexcept override  // NOLINT -- Virtual function naming convention > protected member function naming convention
    {
        return true;
    }
};
/// @brief Template type -- Non-notifiable field base type
/// @tparam T Field data type
template < typename T >
class NFieldBase< T, false >
{
};

/// @brief Template type -- Settable field base type
/// @code{.isoft}
/// export_level=/COM/SoC/SOA
/// @endcode
/// @tparam T Field data type
/// @tparam bool Setter exists by default
template < typename T, bool = true >
class SFieldBase
{
public:
    /// @brief Type alias -- Field data type
    using FieldType = std::remove_cv_t< std::remove_reference_t< T > >;
    /// @brief Type alias -- Set handler signature
    using SetSignature = ara::core::Future< FieldType >(FieldType const&);
    /// @brief Constructor
    SFieldBase() noexcept { ComLogTrace(""); }
    /// @brief Destructor
    virtual ~SFieldBase() noexcept { ComLogTrace(""); }
    /// @brief Copy constructor
    /// @param other
    SFieldBase(SFieldBase const& other) noexcept = default;
    /// @brief Move constructor
    /// @param other
    SFieldBase(SFieldBase&& other) noexcept = default;
    /// @brief Copy assignment operator
    /// @param other
    /// @return SFieldBase
    SFieldBase& operator=(SFieldBase const& other) noexcept = default;
    /// @brief Move assignment operator
    /// @param other
    /// @return SFieldBase
    SFieldBase& operator=(SFieldBase&& other) noexcept = default;
    /// @brief Register set handler
    /// @param[in] setHandler Set handler
    /// @return Result object -- empty/value or error
    virtual ara::core::Result< void > RegisterSetHandler(std::function< SetSignature > setHandler) noexcept = 0;
};
/// @brief Template type -- Non-settable field base type
/// @tparam T Field data type
template < typename T >
class SFieldBase< T, false >
{
};

/// @brief Template type -- Gettable field base type
/// @code{.isoft}
/// export_level=/COM/SoC/SOA
/// @endcode
/// @tparam T Field data type
/// @tparam bool Getter exists by default
template < typename T, bool = true >
class GFieldBase
{
public:
    /// @brief Type alias -- Field data type
    using FieldType = std::remove_cv_t< std::remove_reference_t< T > >;
    /// @brief Type alias -- Get handler signature
    using GetSignature = ara::core::Future< FieldType >();
    /// @brief Constructor
    GFieldBase() noexcept { ComLogTrace(""); }
    /// @brief Destructor
    virtual ~GFieldBase() noexcept { ComLogTrace(""); }
    /// @brief Copy constructor
    /// @param other
    GFieldBase(GFieldBase const& other) noexcept = default;
    /// @brief Move constructor
    /// @param other
    GFieldBase(GFieldBase&& other) noexcept = default;
    /// @brief Copy assignment operator
    /// @param other
    /// @return GFieldBase
    GFieldBase& operator=(GFieldBase const& other) noexcept = default;
    /// @brief Move assignment operator
    /// @param other
    /// @return GFieldBase
    GFieldBase& operator=(GFieldBase&& other) noexcept = default;
    /// @brief Register get handler
    /// @param[in] getHandler Get handler
    /// @return Result object -- empty/value or error
    virtual ara::core::Result< void > RegisterGetHandler(std::function< GetSignature > getHandler) noexcept = 0;
};
/// @brief Template type -- Non-gettable field base type
/// @tparam T Field data type
template < typename T >
class GFieldBase< T, false >
{
};

/// @brief Template type -- Field base type
/// @code{.isoft}
/// export_level=/COM/SoC/SOA
/// @endcode
/// @tparam T Field data type
/// @tparam mode Field mode
template < typename T, FieldMode mode >
class FieldBase
    : public virtual NFieldBase< T, HasNotifier(mode) >
    , public virtual SFieldBase< T, HasSetter(mode) >
    , public virtual GFieldBase< T, HasGetter(mode) >
{
public:
    /// @brief Type alias -- Field data type
    using FieldType = std::remove_cv_t< std::remove_reference_t< T > >;
};

/// @brief Template type -- Field type
/// @code{.isoft}
/// export_level=/COM/SoC/SOA
/// @endcode
/// @tparam T Field data type
/// @tparam mode Field mode
template < typename T, FieldMode mode >
class Field
{
public:
    /// @brief Type alias -- Field implementation type
    using FieldImpl = FieldBase< T, mode >;
    /// @brief Type alias -- Field data type
    using FieldType = typename FieldImpl::FieldType;
    /// @brief Type alias -- Set handler signature
    using SetSignature = typename SFieldBase< T >::SetSignature;
    /// @brief Type alias -- Get handler signature
    using GetSignature = typename GFieldBase< T >::GetSignature;
    /// @brief Constructor
    Field() noexcept { ComLogTrace("create service skeleton field"); }
    /// @brief Destructor
    ~Field() noexcept
    {
        ComLogTrace("destroy service skeleton field");
        std::lock_guard< std::mutex > lock(mutex_);
        impls_.clear();
    }
    /// @brief Copy constructor
    /// @param[in] other
    Field(Field const& other) noexcept = delete;
    /// @brief Copy assignment operator
    /// @param[in] other
    /// @return Field&
    Field& operator=(Field const& other) noexcept = delete;
    /// @brief Move constructor
    /// @param[in] other
    Field(Field&& other) noexcept
        : lock_{other.mutex_}
        , setHandler_{std::move(other.setHandler_)}
        , getHandler_{std::move(other.getHandler_)}
        , sample_
    {
        std::move(other.sample_)
    }
#ifdef HAS_COM_EVENT_SEND_TRIGGER
    , sendContext_ { std::move(other.sendContext_) }
#endif  // HAS_COM_EVENT_SEND_TRIGGER
    {
        ComLogTrace("");
        for (auto impl : other.impls_) {
            operator+=(std::move(impl));
        }
        other.impls_.clear();
        lock_.unlock();
    }
    /// @brief Move assignment operator
    /// @param[in] other
    /// @return Field&
    Field& operator=(Field&& other) noexcept
    {
        ComLogTrace("");
        if (this != &other) {
            std::lock(mutex_, other.mutex_);
            std::lock_guard< std::mutex > lock1(mutex_, std::adopt_lock);
            std::lock_guard< std::mutex > lock2(other.mutex_, std::adopt_lock);
            for (auto impl : other.impls_) {
                operator+=(std::move(impl));
            }
            other.impls_.clear();
            setHandler_ = std::move(other.setHandler_);
            getHandler_ = std::move(other.getHandler_);
            sample_     = std::move(other.sample_);
#ifdef HAS_COM_EVENT_SEND_TRIGGER
            sendContext_ = std::move(other.sendContext_);
#endif  // HAS_COM_EVENT_SEND_TRIGGER
        }
        return *this;
    }
    /// @brief Operator -- increment -- no setter/no getter
    /// @param[in] impl
    /// @return Field&
    template < FieldMode m = mode >
    std::enable_if_t< !HasSetter(m) && !HasGetter(m), Field& > operator+=(std::shared_ptr< FieldImpl >&& impl) noexcept
    {
        ComLogTrace("add bind skeleton field");
        std::lock_guard< std::mutex > lock(mutex_);
        impls_.emplace_back(std::move(impl));
        return *this;
    }
    /// @brief Operator -- increment -- has setter/no getter
    /// @param[in] impl
    /// @return Field&
    template < FieldMode m = mode >
    std::enable_if_t< HasSetter(m) && !HasGetter(m), Field& > operator+=(std::shared_ptr< FieldImpl >&& impl) noexcept
    {
        ComLogTrace("add bind skeleton field");
        std::lock_guard< std::mutex > lock(mutex_);
        impl->RegisterSetHandler(_makeSetHandler());
        impls_.emplace_back(std::move(impl));
        return *this;
    }
    /// @brief Operator -- increment -- no setter/has getter
    /// @param[in] impl
    /// @return Field&
    template < FieldMode m = mode >
    std::enable_if_t< !HasSetter(m) && HasGetter(m), Field& > operator+=(std::shared_ptr< FieldImpl >&& impl) noexcept
    {
        ComLogTrace("add bind skeleton field");
        std::lock_guard< std::mutex > lock(mutex_);
        impl->RegisterGetHandler(_makeGetHandler());
        impls_.emplace_back(std::move(impl));
        return *this;
    }
    /// @brief Operator -- increment -- has setter/has getter
    /// @param[in] impl
    /// @return Field&
    template < FieldMode m = mode >
    std::enable_if_t< HasSetter(m) && HasGetter(m), Field& > operator+=(std::shared_ptr< FieldImpl >&& impl) noexcept
    {
        ComLogTrace("add bind skeleton field");
        std::lock_guard< std::mutex > lock(mutex_);
        impl->RegisterSetHandler(_makeSetHandler());
        impl->RegisterGetHandler(_makeGetHandler());
        impls_.emplace_back(std::move(impl));
        return *this;
    }
    /// @brief Allocate sample -- has notifier
    /// @code{.isoft}
    /// export_level=/COM/SoC/SOA
    /// @endcode
    /// @return Result object -- empty/value or error
    /// @retval ComErrc::kSampleAllocationFailure Sample allocation failure (i.e., shared memory allocation/acquisition failed)
    /// @retval ComErrc::kIllegalUseOfAllocate Illegal use of allocate (i.e., using custom allocator instead of shared memory and failing)
    /// @ref [SWS_CM_90438] -- Allocating data for event transfer
    /// @ref [SWS_CM_00013] -- Re-entrancy – Allocate
    template < bool c = HasNotifier(mode), typename = std::enable_if_t< c > >
    ara::core::Result< SampleAllocateePtr< FieldType > > Allocate() noexcept
    {
        TimeRecorder(TimeNM::Timer, "APINotify::Allocate");
        using Result = ara::core::Result< SampleAllocateePtr< FieldType > >;
        ComLogTrace("allocate service field sample");
        std::lock_guard< std::mutex > lock(mutex_);
        if (impls_.empty()) {
            ComLogError("allocate service field sample rejected: bind field not exist");
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
    /// @brief Update field -- has notifier
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
    template < bool c = HasNotifier(mode), typename = std::enable_if_t< c > >
    ara::core::Result< void > Update(SampleAllocateePtr< FieldType > sample) noexcept
    {
        TimeRecorder(TimeNM::Timer, "APINotify::Update(pointer)");
        using Result = ara::core::Result< void >;
        ComLogTrace("update service field sample", GenArg(sample));
        std::lock_guard< std::mutex > lock(mutex_);
        if (impls_.empty()) {
            ComLogError("update service field sample rejected: bind field not exist", GenArg(sample));
            return Result{MakeErrorCode(ComErrc::kCommunicationStackError, -__LINE__)};
        }
        Result result{};
        if (sample == nullptr) {
            // Send sample is empty
            if (sample_ == nullptr) {
                // Internal sample does not exist, create sample
                for (auto& impl : impls_) {
                    auto sampleRes{impl->Allocate()};
                    if (!sampleRes) {
                        ComLogError("update service field sample error: allocate sample failed", GenArg(sampleRes));
                        return Result{std::move(sampleRes).Error()};
                    }
                    sample = std::move(sampleRes).Value();
                    ComLogDebug("update service field sample: use allocate sample", GenArg(sample));
                    break;
                }
            } else {
                // Internal sample exists, use internal sample
                sample = sample_;
                ComLogDebug("update service field sample: reuse internal sample", GenArg(sample));
            }
        }
        if (sample_ == nullptr) {
            // Internal sample does not exist, use send sample
            sample_ = sample;
            ComLogDebug("update service field sample: init internal sample", GenArg(sample_));
        }
#ifdef HAS_COM_EVENT_SEND_TRIGGER
        if (sendContext_ != nullptr) {
            // Send context exists
            bool same{false};
            if (sample.get() == sample_.get()) {
                // Internal sample reference
                same = true;
                if (sendContext_->sendTrigger.updater != nullptr) {
                    // Sample updater exists, callback
                    sendContext_->sendTrigger.updater(*sample_);
                    ComLogDebug("update service field sample: invoke callback update internal sample", GenArg(sample_));
                }
            } else {
                // Non-internal sample reference
                if (sendContext_->sendTrigger.comparator != nullptr) {
                    // Sample comparator exists, callback
                    same = sendContext_->sendTrigger.comparator(*sample, *sample_);
                    ComLogDebug("update service field sample: invoke callback compare sample", GenArg(same));
                }
            }
            auto now{std::chrono::steady_clock::now()};
            if (same) {
                if (sendContext_->sendTrigger.minimumSendInterval.count() == 0) {
                    // Samples are the same and minimum send interval does not exist, do not send
                    ComLogDebug("send service field sample ignored: send same", GenArg(sendContext_));
                    return result;
                }
                if (now - sendContext_->last < sendContext_->sendTrigger.minimumSendInterval) {
                    // Samples are the same and minimum send interval exists but not exceeded, do not send
                    ComLogDebug("send service field sample ignored: send same and too fast", GenArg(sendContext_));
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
                        ComLogTrace("cyclic update service field sample", GenArg(sample_));
                        auto sendRes{Update(sample_)};
                        if (!sendRes) {
                            ComLogWarning("cyclic update service field sample error", GenArg(sendRes), GenArg(sample_));
                            return;
                        }
                        ComLogDebug("cyclic update service field sample done", GenArg(sample_));
                    });
            }
            // Update send time
            sendContext_->last = now;
        }
#endif  // HAS_COM_EVENT_SEND_TRIGGER                                                                                  \
    // Update internal sample pointer
        sample_ = sample;
        for (auto& impl : impls_) {
            auto res{impl->Send(sample)};
            if (!res) {
                result = std::move(res);
            }
        }
        return result;
    }
    /// @brief Update field -- has notifier
    /// @code{.isoft}
    /// export_level=/COM/SoC/SOA
    /// @endcode
    /// @param[in] sample Sample value
    /// @return Result object -- empty/value or error
    /// @retval ComErrc::kServiceNotOffered Service not offered
    /// @retval ComErrc::kCommunicationLinkError Communication link error
    /// @retval ComErrc::kNoClients No connected clients
    /// @retval ComErrc::kCommunicationStackError Communication stack error (e.g., network stack, network binding, or communication framework error)
    /// @ref [SWS_CM_00119] -- Update Function
    /// @ref [SWS_CM_00120] -- Provision of an update notification event for a Field
    /// @ref [SWS_CM_00016] -- Re-entrancy – Update
    template < bool c = HasNotifier(mode), typename = std::enable_if_t< c > >
    ara::core::Result< void > Update(FieldType const& sample) noexcept
    {
        TimeRecorder(TimeNM::Timer, "APINotify::Update(value)");
        using Result = ara::core::Result< void >;
        ComLogTrace("update service field sample", GenArg(sample));
        std::lock_guard< std::mutex > lock(mutex_);
        if (impls_.empty()) {
            ComLogError("update service field sample rejected: bind field not exist", GenArg(sample));
            return Result{MakeErrorCode(ComErrc::kCommunicationStackError, -__LINE__)};
        }
        Result result{};
        if (sample_ == nullptr) {
            // Internal sample does not exist, create
            for (auto& impl : impls_) {
                auto sampleRes{impl->Allocate()};
                if (!sampleRes) {
                    ComLogError("update service field sample error: allocate sample failed", GenArg(sampleRes));
                    return Result{std::move(sampleRes).Error()};
                }
                sample_ = std::move(sampleRes).Value();
                ComLogDebug("update service field sample: init internal sample", GenArg(sample_));
                break;
            }
        }
#ifdef HAS_COM_EVENT_SEND_TRIGGER
        if (sendContext_ != nullptr) {
            // Send context exists
            bool same{false};
            if (&sample == sample_.get()) {
                // Internal sample reference
                same = true;
                if (sendContext_->sendTrigger.updater != nullptr) {
                    // Sample updater exists, callback
                    sendContext_->sendTrigger.updater(*sample_);
                    ComLogDebug("update service field sample: invoke callback update internal sample", GenArg(sample_));
                }
            } else {
                // Non-internal sample reference
                if (sendContext_->sendTrigger.comparator != nullptr) {
                    // Sample comparator exists, callback
                    same = sendContext_->sendTrigger.comparator(sample, *sample_);
                    ComLogDebug("update service field sample: invoke callback compare sample", GenArg(same));
                }
            }
            auto now{std::chrono::steady_clock::now()};
            if (same) {
                if (sendContext_->sendTrigger.minimumSendInterval.count() == 0) {
                    // Samples are the same and minimum send interval does not exist, do not send
                    ComLogDebug("send service field sample ignored: send same", GenArg(sendContext_));
                    return result;
                }
                if (now - sendContext_->last < sendContext_->sendTrigger.minimumSendInterval) {
                    // Samples are the same and minimum send interval exists but not exceeded, do not send
                    ComLogDebug("send service field sample ignored: send same and too fast", GenArg(sendContext_));
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
                        ComLogTrace("cyclic update service field sample", GenArg(sample_));
                        auto sendRes{Update(sample_)};
                        if (!sendRes) {
                            ComLogWarning("cyclic update service field sample error", GenArg(sendRes), GenArg(sample_));
                            return;
                        }
                        ComLogDebug("cyclic update service field sample done", GenArg(sample_));
                    });
            }
            // Update send time
            sendContext_->last = now;
        }
#endif  // HAS_COM_EVENT_SEND_TRIGGER                                                                                  \
    // Update internal sample value
        *sample_ = sample;
        for (auto& impl : impls_) {
            auto res{impl->Send(sample)};
            if (!res) {
                result = std::move(res);
            }
        }
        return result;
    }
    /// @brief Update field -- no notifier/has getter
    /// @code{.isoft}
    /// export_level=/COM/SoC/SOA
    /// @endcode
    /// @param[in] sample Sample value
    /// @return Result object -- empty/value or error
    template < bool c = !HasNotifier(mode) && HasGetter(mode) >
    std::enable_if_t< c, ara::core::Result< void > > Update(FieldType const& sample) noexcept
    {
        using Result = ara::core::Result< void >;
        ComLogTrace("update service field sample", GenArg(sample));
        std::lock_guard< std::mutex > lock(mutex_);
        if (impls_.empty()) {
            ComLogError("update service field sample rejected: bind field not exist", GenArg(sample));
            return Result{MakeErrorCode(ComErrc::kCommunicationStackError, -__LINE__)};
        }
        Result result{};
        if (!getHandler_) {
            if (sample_ == nullptr) {
                // Internal sample does not exist, create
                sample_.reset(new FieldType{sample});
                ComLogDebug("update service field sample: init internal sample", GenArg(sample_));
            } else {
                // Update internal sample value
                ComLogDebug("update service field sample: sync internal sample", GenArg(sample_), GenArg(sample));
                *sample_ = sample;
            }
        }
        return result;
    }
#ifdef HAS_COM_EVENT_SEND_TRIGGER
    /// @brief Set send trigger -- has notifier
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
    template < bool c = HasNotifier(mode), typename = std::enable_if_t< c > >
    ara::core::Result< void > SetSendTrigger(SendTrigger< FieldType > sendTrigger) noexcept
    {
        using Result = ara::core::Result< void >;
        ComLogTrace("set service field send trigger", GenVal(sendTrigger));
        std::lock_guard< std::mutex > lock(mutex_);
        Result result{};
        if (sendContext_ == nullptr) {
            // Send context does not exist, create
            sendContext_ = std::make_unique< SendContext< FieldType > >();
            ComLogDebug("set service field send trigger: init send context", GenVal(sendContext_));
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
                    ComLogTrace("cyclic update service field sample", GenArg(sample_));
                    auto sendRes{Update(sample_)};
                    if (!sendRes) {
                        ComLogWarning("cyclic update service field sample error", GenArg(sendRes), GenArg(sample_));
                        return;
                    }
                    ComLogDebug("cyclic update service field sample done", GenArg(sample_));
                });
        }
        ComLogDebug("set service field send trigger done", GenVal(sendContext_));
        return result;
    }
#endif  // HAS_COM_EVENT_SEND_TRIGGER
    /// @brief Register set handler -- has setter
    /// @code{.isoft}
    /// export_level=/COM/SoC/SOA
    /// @endcode
    /// @param[in] setHandler Set handler
    /// @return Result object -- empty/value or error
    /// @ref [SWS_CM_11363] -- Error behaviour of provided Execution Context for registering Setters
    /// @ref [SWS_CM_00116] -- Registering Setters
    /// @ref [SWS_CM_00117] -- Existence of RegisterSetHandler method
    /// @ref [SWS_CM_00015] -- Re-entrancy – RegisterSetHandler
    template < bool c = HasSetter(mode), typename = std::enable_if_t< c > >
    ara::core::Result< void > RegisterSetHandler(std::function< SetSignature > setHandler) noexcept
    {
        return RegisterSetHandler(setHandler, [](std::function< void() > const& f) { f(); });
    }
    /// @brief Register set handler -- has setter
    /// @code{.isoft}
    /// export_level=/COM/SoC/SOA
    /// @endcode
    /// @param[in] setHandler Set handler
    /// @param[in] executor Executor -- the processing will take place in its context
    /// @return Result object -- empty/value or error
    /// @retval ComErrc::kCouldNotExecute Cannot execute (e.g., due to resource issues)
    /// @ref [SWS_CM_11363] -- Error behaviour of provided Execution Context for registering Setters
    /// @ref [SWS_CM_11362] -- Execution Context for registering Setters
    /// @ref [SWS_CM_00117] -- Existence of RegisterSetHandler method
    /// @ref [SWS_CM_00015] -- Re-entrancy – RegisterSetHandler
    template < typename Executor, bool c = HasSetter(mode), typename = std::enable_if_t< c > >
    ara::core::Result< void > RegisterSetHandler(std::function< SetSignature > setHandler, Executor&& executor) noexcept
    {
        using Result = ara::core::Result< void >;
        ComLogTrace("register service field set handler");
        if (!setHandler) {
            ComLogError("register service field set handler rejected: handler invalid");
            return Result{MakeErrorCode(ComErrc::kServiceNotOffered, -__LINE__)};
        }
        if (!executor) {
            ComLogError("register service field set handler rejected: executor invalid");
            return Result{MakeErrorCode(ComErrc::kCouldNotExecute, -__LINE__)};
        }
        std::lock_guard< std::mutex > lock(mutex_);
        if (impls_.empty()) {
            ComLogError("register service field set handler rejected: bind field not exist");
            return Result{MakeErrorCode(ComErrc::kCommunicationStackError, -__LINE__)};
        }
        setHandler_ = [setHandler{std::move(setHandler)}, executor{std::move(executor)}](FieldType const& sample) {
            auto promise{std::make_shared< ara::core::Promise< FieldType > >()};
            auto future{promise->get_future()};
            executor([setHandler{std::move(setHandler)}, sample{std::move(sample)}, promise{std::move(promise)}]() {
                auto res{setHandler(sample).GetResult()};
                if (!res) {
                    ComLogError("handle service field set error", GenArg(res), GenArg(sample));
                    promise->SetError(std::move(res).Error());
                    return;
                }
                ComLogDebug("handle service field set done", GenArg(sample));
                promise->set_value(std::move(res).Value());
            });
            future.wait();
            return future;
        };
        return Result{};
    }
    /// @brief Register get handler -- has getter
    /// @code{.isoft}
    /// export_level=/COM/SoC/SOA
    /// @endcode
    /// @param[in] getHandler Get handler
    /// @return Result object -- empty/value or error
    /// @retval ComErrc::kCouldNotExecute Cannot execute (e.g., due to resource issues)
    /// @ref [SWS_CM_11361] -- Error behaviour of provided Execution Context for registering Getters
    /// @ref [SWS_CM_00114] -- Registering Getters
    /// @ref [SWS_CM_00115] -- Existence of RegisterGetHandler method
    /// @ref [SWS_CM_00014] -- Re-entrancy – RegisterGetHandler
    template < bool c = HasGetter(mode), typename = std::enable_if_t< c > >
    ara::core::Result< void > RegisterGetHandler(std::function< GetSignature > getHandler) noexcept
    {
        return RegisterGetHandler(getHandler, [](std::function< void() > const& f) { f(); });
    }
    /// @brief Register get handler -- has getter
    /// @code{.isoft}
    /// export_level=/COM/SoC/SOA
    /// @endcode
    /// @param[in] getHandler Get handler
    /// @param[in] executor Executor -- the processing will take place in its context
    /// @return Result object -- empty/value or error
    /// @retval ComErrc::kCouldNotExecute Cannot execute (e.g., due to resource issues)
    /// @ref [SWS_CM_11361] -- Error behaviour of provided Execution Context for registering Getters
    /// @ref [SWS_CM_11360] -- Execution Context for registering Getters
    /// @ref [SWS_CM_00115] -- Existence of RegisterGetHandler method
    /// @ref [SWS_CM_00014] -- Re-entrancy – RegisterGetHandler
    template < typename Executor, bool c = HasGetter(mode), typename = std::enable_if_t< c > >
    ara::core::Result< void > RegisterGetHandler(std::function< GetSignature > getHandler, Executor&& executor) noexcept
    {
        using Result = ara::core::Result< void >;
        ComLogTrace("register service field get handler");
        if (!getHandler) {
            ComLogError("register service field get handler rejected: handler invalid");
            return Result{MakeErrorCode(ComErrc::kServiceNotOffered, -__LINE__)};
        }
        if (!executor) {
            ComLogError("register service field get handler rejected: executor invalid");
            return Result{MakeErrorCode(ComErrc::kCouldNotExecute, -__LINE__)};
        }
        std::lock_guard< std::mutex > lock(mutex_);
        if (impls_.empty()) {
            ComLogError("register service field get handler rejected: bind field not exist");
            return Result{MakeErrorCode(ComErrc::kCommunicationStackError, -__LINE__)};
        }
        getHandler_ = [getHandler{std::move(getHandler)}, executor{std::move(executor)}]() {
            auto promise{std::make_shared< ara::core::Promise< FieldType > >()};
            auto future{promise->get_future()};
            executor([getHandler{std::move(getHandler)}, promise{std::move(promise)}]() {
                auto res{getHandler().GetResult()};
                if (!res) {
                    ComLogError("handle service field get error", GenArg(res));
                    promise->SetError(res.Error());
                    return;
                }
                ComLogDebug("handle service field get done", GenArg(res));
                promise->set_value(res.Value());
            });
            future.wait();
            return future;
        };
        return Result{};
    }
    /// @brief Ensure field is usable
    /// @return Result object -- empty/value or error
    /// @retval ComErrc::kFieldValueIsNotValid Field value is not valid @ref ServiceSkeleton::OfferService
    /// @retval ComErrc::kSetHandlerNotSet Field set handler not set @ref ServiceSkeleton::OfferService
    /// @ref [SWS_CM_00128] -- Ensuring the existence of valid Field values
    /// @ref [SWS_CM_00129] -- Ensuring the existence of SetHandler
    ara::core::Result< void > EnsureAvailable() const noexcept
    {
        using Result = ara::core::Result< void >;
        ComLogTrace("ensure service field available");
        std::lock_guard< std::mutex > lock(mutex_);
        if (HasNotifier(mode) && !sample_) {
            ComLogError("ensure service field available rejected: internal sample uninitialized");
            return Result{MakeErrorCode(ComErrc::kFieldValueIsNotValid, -__LINE__)};
        }
        if (HasSetter(mode) && !setHandler_) {
            ComLogError("ensure service field available rejected: set handler unregistered");
            return Result{MakeErrorCode(ComErrc::kSetHandlerNotSet, -__LINE__)};
        }
        if (HasGetter(mode) && !getHandler_ && !sample_) {
            ComLogError(
                "ensure service field available rejected: get handler unregistered and internal sample uninitialized");
            return Result{MakeErrorCode(ComErrc::kFieldValueIsNotValid, -__LINE__)};
        }
        return Result{};
    }

private:
    /// @brief Construct a set handler
    /// @return Set handler
    std::function< SetSignature > _makeSetHandler() noexcept
    {
        return std::bind(&Field::_handleSet, this, std::placeholders::_1);
    }
    /// @brief Construct a get handler
    /// @return Get handler
    std::function< GetSignature > _makeGetHandler() noexcept { return std::bind(&Field::_handleGet, this); }
    /// @brief Update field -- no notifier/no getter
    /// @param[in] sample Field value
    /// @return Result object -- empty/value or error
    template < FieldMode m = mode >
    std::enable_if_t< !HasNotifier(m) && !HasGetter(m), ara::core::Result< void > >
    Update(  // NOLINT -- Standard interface name > naming convention
        FieldType const& sample) noexcept
    {
        std::ignore  = sample;
        using Result = ara::core::Result< void >;
        return Result{};
    }
    /// @brief Handle set method
    /// @return Future object -- asynchronous/synchronous wait for result object
    ara::core::Future< FieldType > _handleSet(FieldType const& sample) noexcept
    {
        decltype(setHandler_) setHandler;
        {
            std::lock_guard< std::mutex > lock(mutex_);
            assert(setHandler_);  // @ref EnsureAvailable
            setHandler = setHandler_;
        }
        ComLogDebug("handle service field set: invoke callback handle set", GenArg(sample));
        ara::core::Promise< FieldType > promise;
        auto res{setHandler(sample).GetResult()};
        if (!res) {
            promise.SetError(res.Error());
            return promise.get_future();
        }
        promise.set_value(res.Value());
        Update(res.Value());  // [TODO] enable SField::Update(current private) or specialized SField::_handleSet?
        return promise.get_future();
    }
    /// @brief Handle get method
    /// @return Future object -- asynchronous/synchronous wait for result object
    ara::core::Future< FieldType > _handleGet() const noexcept
    {
        decltype(getHandler_) getHandler;
        {
            std::lock_guard< std::mutex > lock(mutex_);
            if (!getHandler_) {
                assert(sample_);  // @ref EnsureAvailable
                ara::core::Promise< FieldType > promise;
                ComLogDebug("handle service field get: get internal sample", GenArg(sample_));
                promise.set_value(*sample_);
                return promise.get_future();
            }
            getHandler = getHandler_;
        }
        ComLogDebug("handle service field get: invoke callback handle get");
        return getHandler();
    }

private:
    /// @brief Mutex
    mutable std::mutex mutex_;
    /// @brief Unique lock
    std::unique_lock< std::mutex > lock_;
    /// @brief Field implementation set
    std::list< std::shared_ptr< FieldImpl > > impls_;
    /// @brief Set handler
    std::function< SetSignature > setHandler_;
    /// @brief Get handler
    std::function< GetSignature > getHandler_;
    /// @brief Sample cache
    SampleAllocateePtr< FieldType > sample_;
#ifdef HAS_COM_EVENT_SEND_TRIGGER
    /// @brief Send context
    std::unique_ptr< SendContext< FieldType > > sendContext_;
#endif  // HAS_COM_EVENT_SEND_TRIGGER
};
}  // namespace skeleton
}  // namespace internal
}  // namespace com
}  // namespace ara