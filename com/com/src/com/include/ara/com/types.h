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
/// @file       types.h
/// @brief      Communication type header file
/// @details
/// @date       2021-12-31
/// @author     mazelin
/// @version    1.2.0
///
/// ================================================================
///
/// @ref [SWS_CM_01020] -- Folder structure
/// @ref [SWS_CM_01013] -- Types header file existence
/// @ref [SWS_CM_01019] -- Data Type declarations in Types header file
///
/// ================================================================

#pragma once

#include <chrono>
#include <functional>

#include "ara/com/e2e/e2e.h"
#include "ara/core/instance_specifier.h"
#include "ara/core/vector.h"
#include "com_error_domain.h"
#include "internal/base.h"
#ifdef HAS_COM_EVENT_SEND_TRIGGER
    #include "isoft/naicpp/global_evloop.h"
#endif  // HAS_COM_EVENT_SEND_TRIGGER

/// @brief Namespace -- Communication
/// @ref [SWS_CM_01018] -- Types header file namespace
namespace ara {
namespace com {
/// @brief Method invocation processing mode enumeration -- Skeleton side
/// @code{.isoft}
/// export_level=/COM/SoC
/// @endcode
/// @ref [SWS_CM_00301] -- Method Call Processing Mode
enum class MethodCallProcessingMode : uint8_t
{
    kPoll,              ///< Polling mode
    kEvent,             ///< Parallel event-driven mode
    kEventSingleThread  ///< Serial event-driven mode
};

/// @brief Service instance identifier type
/// @code{.isoft}
/// export_level=/COM/SoC
/// @endcode
/// @ref [SWS_CM_00302] -- Instance Identifier Class
class InstanceIdentifier
{
public:
    /// @brief Constructor
    /// @code{.isoft}
    /// export_level=/COM/SoC
    /// @endcode
    /// @param[in] value Instance identifier value
    explicit InstanceIdentifier(ara::core::StringView value) noexcept;
    /// @brief Destructor
    ~InstanceIdentifier() noexcept;
    /// @brief Copy constructor
    /// @param[in] other
    InstanceIdentifier(InstanceIdentifier const& other) noexcept;
    /// @brief Move constructor
    /// @param[in] other
    InstanceIdentifier(InstanceIdentifier&& other) noexcept;
    /// @brief Copy assignment operator
    /// @param[in] other
    /// @return InstanceIdentifier&
    InstanceIdentifier& operator=(InstanceIdentifier const& other) noexcept;
    /// @brief Move assignment operator
    /// @param[in] other
    /// @return InstanceIdentifier&
    InstanceIdentifier& operator=(InstanceIdentifier&& other) noexcept;
    /// @brief Operator -- equality comparison
    /// @param[in] other
    /// @return bool
    bool operator==(InstanceIdentifier const& other) const noexcept;
    /// @brief Operator -- less than comparison
    /// @param[in] other
    /// @return bool
    bool operator<(InstanceIdentifier const& other) const noexcept;
    /// @brief Convert to string
    /// @return Instance identifier string
    ara::core::StringView ToString() const noexcept;

public:
    /// @brief Check if it is an any instance identifier
    /// @code{.isoft}
    /// export_level=/COM/SoC
    /// @endcode
    /// @param[in] prefix Instance identifier prefix
    /// @return bool
    bool IsAny(ara::core::String const& prefix) const noexcept;
    /// @brief Construct an any instance identifier
    /// @code{.isoft}
    /// export_level=/COM/SoC
    /// @endcode
    /// @return Instance identifier
    static InstanceIdentifier MakeAny() noexcept;
    /// @brief Any keyword, used for service discovery @ref FindService or @ref StartFindService
    /// @code{.isoft}
    /// export_level=/COM/SoC
    /// @endcode
    static constexpr char kAny[]{"ANY"};

private:
    /// @brief Service instance identifier implementation type -- Declaration
    class Impl;
    /// @brief Service instance identifier implementation
    std::shared_ptr< Impl > impl_;
};

/// @brief Instance identifier container type
/// @code{.isoft}
/// export_level=/COM/SoC
/// @endcode
/// @ref [SWS_CM_00319] -- Instance Identifier Container Class
using InstanceIdentifierContainer = ara::core::Vector< InstanceIdentifier >;

/// @brief Find service handle -- Proxy side
/// @code{.isoft}
/// export_level=/COM/SoC
/// @endcode
/// @details Returned by each asynchronous discovery call, used to stop this asynchronous discovery
/// @ref [SWS_CM_00303] -- Find Service Handle
class FindServiceHandle
{
public:
    /// @brief Constructor
    FindServiceHandle() noexcept;
    /// @brief Constructor
    /// @param[in] serviceIdentifier Service identifier
    /// @param[in] instanceIdentifier Instance identifier
    /// @param[in] uid Unique identifier
    FindServiceHandle(internal::ServiceIdentifier serviceIdentifier,
                      InstanceIdentifier instanceIdentifier,
                      uint32_t uid) noexcept;
    /// @brief Destructor
    ~FindServiceHandle() noexcept;
    /// @brief Copy constructor
    /// @param[in] other
    FindServiceHandle(FindServiceHandle const& other) = default;
    /// @brief Copy assignment operator
    /// @param[in] other
    /// @return FindServiceHandle&
    FindServiceHandle& operator=(FindServiceHandle const& other) noexcept = default;
    /// @brief Move constructor
    /// @param[in] other
    FindServiceHandle(FindServiceHandle&& other) noexcept = default;
    /// @brief Move assignment operator
    /// @param[in] other
    /// @return FindServiceHandle&
    FindServiceHandle& operator=(FindServiceHandle&& other) noexcept = default;
    /// @brief Operator -- equality comparison
    /// @param[in] other
    /// @return bool
    bool operator==(FindServiceHandle const& other) const noexcept;
    /// @brief Operator -- less than comparison
    /// @param[in] other
    /// @return bool
    bool operator<(FindServiceHandle const& other) const noexcept;
    /// @brief Convert to string
    /// @return Find service handle string
    ara::core::String ToString() const noexcept;
    /// @brief Generate a unique identifier
    /// @return Unique identifier
    static uint32_t GenerateUID() noexcept;

public:
    /// @brief Service identifier
    internal::ServiceIdentifier serviceIdentifier;
    /// @brief Instance identifier
    InstanceIdentifier instanceIdentifier;
    /// @brief Unique identifier
    uint32_t uid;
};

/// @brief Service handle container -- Proxy side
/// @code{.isoft}
/// export_level=/COM/SoC
/// @endcode
/// @details After calling the discovery service, obtain a list of service handles
/// @tparam HandleType Service handle type
/// @ref [SWS_CM_00304] -- Service Handle Container
template < typename HandleType >
using ServiceHandleContainer = ara::core::Vector< HandleType >;

/// @brief Find service handler -- Proxy side
/// @code{.isoft}
/// export_level=/COM/SoC
/// @endcode
/// @tparam HandleType Service handle type
/// @ref [SWS_CM_00383] -- Find Service Handler
template < typename HandleType >
using FindServiceHandler = std::function< void(ServiceHandleContainer< HandleType >, FindServiceHandle) >;

/// @brief Event sample data pointer template type -- Proxy side
/// @code{.isoft}
/// export_level=/COM/SoC
/// @endcode
/// @details 1. Holds the event sample data object (similar to a pointer)
/// @details 2. Manages the allocation and deallocation of event sample data objects (e.g., memory object pool, used with Event::GetNewSamples mechanism)
/// @details 3. Provides common operation interfaces for event sample data
/// @note Vendors are allowed to change this implementation
/// @tparam T Sample data type
/// @ref [SWS_CM_00306] -- Sample Pointer
template < typename T >
class SamplePtr
{
private:
    std::shared_ptr< T > ptr_{};
    e2e::ProfileCheckStatus status_{e2e::ProfileCheckStatus::kOk};

public:
    /// @brief Constructor
    /// @param[in] ptr Sample pointer
    /// @param[in] func Sample pointer deleter
    /// @param[in] status E2E check status -- default OK
    template < typename F >
    explicit constexpr SamplePtr(T* ptr,
                                 F&& func,
                                 e2e::ProfileCheckStatus status = e2e::ProfileCheckStatus::kOk) noexcept
        : ptr_{ptr, std::move(func)}, status_{status}
    {
    }
    /// @brief Constructor
    /// @param[in] ptr Sample pointer
    /// @param[in] status E2E check status -- default OK
    explicit constexpr SamplePtr(T* ptr, e2e::ProfileCheckStatus status = e2e::ProfileCheckStatus::kOk) noexcept
        : ptr_{ptr}, status_{status}
    {
    }
    /// @brief Constructor
    /// @param[in] status E2E check status -- default OK
    explicit constexpr SamplePtr(e2e::ProfileCheckStatus status = e2e::ProfileCheckStatus::kOk) noexcept
        : status_{status}
    {
    }
    /// @brief Constructor -- null pointer
    explicit constexpr SamplePtr(std::nullptr_t) noexcept {}
    /// @brief Destructor
    ~SamplePtr() noexcept = default;
    /// @brief Overloaded assignment operator -- null pointer
    /// @return SamplePtr&
    SamplePtr& operator=(std::nullptr_t) noexcept
    {
        ptr_    = nullptr;
        status_ = e2e::ProfileCheckStatus::kOk;
        return *this;
    }
    /// @brief Copy constructor
    SamplePtr(SamplePtr const&) noexcept = default;
    /// @brief Copy assignment operator
    /// @return SamplePtr&
    SamplePtr& operator=(SamplePtr const&) noexcept = default;
    /// @brief Move constructor
    /// @param[in] other
    SamplePtr(SamplePtr&& other) noexcept : ptr_{std::move(other.ptr_)}, status_{std::move(other.status_)} {}
    /// @brief Move assignment operator
    /// @param[in] other
    /// @return SamplePtr&
    SamplePtr& operator=(SamplePtr&& other) noexcept
    {
        if (this != &other) {
            ptr_    = std::move(other.ptr_);
            status_ = std::move(other.status_);
        }
        return *this;
    }
    /// @brief Operator -- returns reference
    /// @code{.isoft}
    /// export_level=/COM/SoC
    /// @endcode
    /// @return Sample reference
    T& operator*() const noexcept { return ptr_.operator*(); }
    /// @brief Operator -- returns pointer
    /// @code{.isoft}
    /// export_level=/COM/SoC
    /// @endcode
    /// @return Sample pointer
    T* operator->() const noexcept { return ptr_.operator->(); }
    /// @brief Operator -- implicit conversion
    /// @code{.isoft}
    /// export_level=/COM/SoC
    /// @endcode
    /// @return bool
    explicit operator bool() const noexcept { return bool(ptr_); }
    /// @brief Get the actual pointer
    /// @code{.isoft}
    /// export_level=/COM/SoC
    /// @endcode
    /// @return Actual pointer
    T* Get() const noexcept { return ptr_.get(); }
    /// @brief Swap sample pointers
    /// @param[in] other
    void Swap(SamplePtr& other) noexcept
    {
        std::swap(ptr_, other.ptr_);
        std::swap(status_, other.status_);
    }
    /// @brief Reset sample pointer
    void Reset(std::nullptr_t) noexcept { ptr_.reset(); }
    /// @brief Get E2E check status
    /// @code{.isoft}
    /// export_level=/COM/SoC
    /// @endcode
    /// @return E2E check status
    /// @ref [SWS_CM_90420] -- E2E ProfileCheckStatus of a sample
    e2e::ProfileCheckStatus GetProfileCheckStatus() const noexcept { return status_; }
};

/// @brief Event sample data pointer template type -- Skeleton side
/// @code{.isoft}
/// export_level=/COM/SoC
/// @endcode
/// @details 1. Holds the event sample data object (similar to a pointer)
/// @details 2. Manages the allocation and deallocation of event sample data objects (e.g., memory object pool, used with Event::Allocate mechanism)
/// @note Vendors are allowed to change this implementation
/// @tparam T Sample data type
/// @ref [SWS_CM_00308] -- Sample Allocatee Pointer
template < typename T >
using SampleAllocateePtr = std::shared_ptr< T >;

/// @brief Event receive handler -- Proxy side
/// @code{.isoft}
/// export_level=/COM/SoC
/// @endcode
/// @note Only notifies reception, does not return available data
/// @ref [SWS_CM_00309] -- Event Receive Handler
using EventReceiveHandler = std::function< void() >;

/// @brief Event subscription state enumeration -- Proxy side
/// @code{.isoft}
/// export_level=/COM/SoC
/// @endcode
/// @ref [SWS_CM_00310] -- Subscription State
enum class SubscriptionState : uint8_t
{
    kSubscribed,
    kNotSubscribed,
    kSubscriptionPending
};

/// @brief Event subscription state change handler -- Proxy side
/// @code{.isoft}
/// export_level=/COM/SoC
/// @endcode
/// @ref [SWS_CM_00311] -- Subscription State Changed Handler
/// @ref [SWS_CM_00313] -- Call SubscriptionStateChangeHandler with kSubscriptionPending
/// @ref [SWS_CM_00314] -- Call SubscriptionStateChangeHandler with kSubscribed
/// @ref [SWS_CM_00315] -- Reestablishing an active subscription
using SubscriptionStateChangeHandler = std::function< void(SubscriptionState) >;

#ifdef HAS_COM_EVENT_SEND_TRIGGER
/// @brief Send trigger template -- Skeleton side
/// @code{.isoft}
/// export_level=/COM/SoC
/// @endcode
/// @ref [TPS_MANI_03210] -- Specification of event specific communication attributes
/// @ref [TPS_MANI_03210] -- Specification of field specific communication attributes
/// @ref [TPS_MANI_03212] -- Specification of initial value for a field
/// @ref [TPS_MANI_01107] -- Specification of intentions for the sender of events or field notifiers
/// @details | Send change comparator | Periodic send interval | Reset periodic send interval | Minimum send interval | Mode |
/// @details | 0 | 0 | 0 | 0 | Disabled |
/// @details | 0 | 0 | 1 | 1 | None |
/// @details | 0 | 1 | 0 | 0 | Cyclic |
/// @details | 0 | 1 | 0 | 1 | CyclicEnableInterval |
/// @details | 0 | 1 | 1 | 0 | CyclicEnableReset |
/// @details | 0 | 1 | 1 | 1 | CyclicEnableResetAndInterval |
/// @details | 1 | 0 | 0 | 0 | OnChange |
/// @details | 1 | 0 | 0 | 1 | OnChangeEnableInterval |
/// @details | 1 | 0 | 1 | 0 | Equivalent to OnChange |
/// @details | 1 | 0 | 1 | 1 | Equivalent to OnChangeEnableInterval |
/// @details | 1 | 1 | 0 | 0 | CyclicAndOnChange |
/// @details | 1 | 1 | 0 | 1 | CyclicAndOnChangeEnableInterval |
/// @details | 1 | 1 | 1 | 0 | CyclicAndOnChangeEnableReset |
/// @details | 1 | 1 | 1 | 1 | CyclicAndOnChangeEnableResetAndInterval |
template < typename T >
struct SendTrigger
{
    /// @brief Internal sample updater
    /// @details Can be set for all scenarios where direct update of internal samples is desired
    std::function< void(T&) > updater{};
    /// @brief Send change comparator
    /// @details Must be set for all scenarios requiring comparison
    std::function< bool(const T& /*new*/, const T& /*old*/) > comparator{};
    /// @brief Periodic send interval
    /// @details Data is sent cyclically at fixed time intervals, defined by dataUpdatePeriod
    std::chrono::milliseconds dataUpdatePeriod{};
    /// @brief Reset periodic send interval
    /// @brief Whether active sending resets the update cycle, defined by dataUpdatePeriod
    bool resetUpdatePeriod{};
    /// @brief Minimum send interval
    /// @details Data is sent arbitrarily as needed, defined by minimumSendInterval
    std::chrono::milliseconds minimumSendInterval{};
    /// @brief Whether to use end-to-end protection
    /// @details Defines whether to use end-to-end protection, optional values are true, false
    bool usesEndToEndProtection{};

    /// @brief Type alias -- Mark as enumerable
    using IsEnumerableTag = void;
    /// @brief Enumeration operation -- enumerate members of this type via a passed function object
    /// @tparam F Passed function object type
    /// @param[in] fun Passed function object
    template < typename F >
    void enumerate(F& fun)  // NOLINT -- Template interface name > naming convention
    {
        fun("update", updater != nullptr ? "enable" : "disable");
        fun("compare", comparator != nullptr ? "enable" : "disable");
        fun("period", dataUpdatePeriod.count());
        fun("reset", resetUpdatePeriod ? "enable" : "disable");
        fun("interval", minimumSendInterval.count());
        fun("e2e", usesEndToEndProtection ? "enable" : "disable");
    }

    /// @brief Preset mode -- Disable sending (Disabled)
    /// @details Behavior:
    /// @details - Send/Update send directly (no period, no comparison)
    static SendTrigger Disabled() noexcept { return {}; }
    /// @brief Preset mode -- Periodic sending (Cyclic)
    /// @details Behavior:
    /// @details - Send/Update send directly (do not reset send period)
    /// @details - Periodic sending possible (no minimum interval)
    /// @details - No trigger comparison
    static SendTrigger Cyclic(std::chrono::milliseconds dataUpdatePeriodIn) noexcept
    {
        SendTrigger sendTrigger{};
        sendTrigger.dataUpdatePeriod    = dataUpdatePeriodIn;
        sendTrigger.resetUpdatePeriod   = false;
        sendTrigger.minimumSendInterval = std::chrono::milliseconds{0};
        return sendTrigger;
    }
    /// @brief Preset mode -- Periodic sending with minimum interval (CyclicEnableInterval)
    /// @details Behavior:
    /// @details - Send/Update send directly (do not reset send period, with minimum interval)
    /// @details - Periodic sending possible (with minimum interval)
    /// @details - No trigger comparison
    static SendTrigger CyclicEnableInterval(std::chrono::milliseconds dataUpdatePeriodIn,
                                            std::chrono::milliseconds minimumSendIntervalIn) noexcept
    {
        SendTrigger sendTrigger{};
        sendTrigger.dataUpdatePeriod    = dataUpdatePeriodIn;
        sendTrigger.resetUpdatePeriod   = false;
        sendTrigger.minimumSendInterval = minimumSendIntervalIn;
        return sendTrigger;
    }
    /// @brief Preset mode -- Periodic sending resettable (CyclicEnableReset)
    /// @details Behavior:
    /// @details - Send/Update send directly (send period can be reset)
    /// @details - Periodic sending possible (no minimum interval)
    /// @details - No trigger comparison
    static SendTrigger CyclicEnableReset(std::chrono::milliseconds dataUpdatePeriodIn) noexcept
    {
        SendTrigger sendTrigger{};
        sendTrigger.dataUpdatePeriod    = dataUpdatePeriodIn;
        sendTrigger.resetUpdatePeriod   = true;
        sendTrigger.minimumSendInterval = std::chrono::milliseconds{0};
        return sendTrigger;
    }
    /// @brief Preset mode -- Periodic sending resettable with minimum interval (CyclicEnableResetAndInterval)
    /// @details Behavior:
    /// @details - Send/Update send directly (send period can be reset, with minimum interval)
    /// @details - Periodic sending possible (with minimum interval)
    /// @details - No trigger comparison
    static SendTrigger CyclicEnableResetAndInterval(std::chrono::milliseconds dataUpdatePeriodIn,
                                                    std::chrono::milliseconds minimumSendIntervalIn) noexcept
    {
        SendTrigger sendTrigger{};
        sendTrigger.dataUpdatePeriod    = dataUpdatePeriodIn;
        sendTrigger.resetUpdatePeriod   = true;
        sendTrigger.minimumSendInterval = minimumSendIntervalIn;
        return sendTrigger;
    }
    /// @brief Preset mode -- Trigger on change (OnChange)
    /// @details Behavior:
    /// @details - Send/Update conditional sending (send on difference)
    /// @details - No periodic sending
    /// @details - Trigger comparison possible
    static SendTrigger OnChange(std::function< bool(const T&, const T&) > comparatorIn) noexcept
    {
        SendTrigger sendTrigger{};
        sendTrigger.comparator = std::move(comparatorIn);
        return sendTrigger;
    }
    /// @brief Preset mode -- Trigger on change with minimum interval (OnChangeEnableInterval)
    /// @details Behavior:
    /// @details - Send/Update conditional sending (exceeds minimum send interval, send on difference)
    /// @details - No periodic sending
    /// @details - Trigger comparison possible
    static SendTrigger OnChangeEnableInterval(std::chrono::milliseconds minimumSendIntervalIn,
                                              std::function< bool(const T&, const T&) > comparatorIn) noexcept
    {
        SendTrigger sendTrigger{};
        sendTrigger.comparator          = std::move(comparatorIn);
        sendTrigger.minimumSendInterval = minimumSendIntervalIn;
        return sendTrigger;
    }
    /// @brief Preset mode -- Periodic or change trigger (CyclicAndOnChange)
    /// @details Behavior:
    /// @details - Send/Update conditional sending (send on difference or period reached)
    /// @details - Periodic sending possible (no minimum interval)
    /// @details - Trigger comparison possible
    static SendTrigger CyclicAndOnChange(std::chrono::milliseconds dataUpdatePeriodIn,
                                         std::function< bool(const T&, const T&) > comparatorIn) noexcept
    {
        SendTrigger sendTrigger{};
        sendTrigger.comparator          = std::move(comparatorIn);
        sendTrigger.dataUpdatePeriod    = dataUpdatePeriodIn;
        sendTrigger.resetUpdatePeriod   = false;
        sendTrigger.minimumSendInterval = std::chrono::milliseconds{0};
        return sendTrigger;
    }
    /// @brief Preset mode -- Periodic or change trigger with minimum interval (CyclicAndOnChangeEnableInterval)
    /// @details Behavior:
    /// @details - Send/Update conditional sending (exceeds minimum send interval, send on difference or period reached)
    /// @details - Periodic sending possible (with minimum interval)
    /// @details - Trigger comparison possible
    static SendTrigger CyclicAndOnChangeEnableInterval(std::chrono::milliseconds dataUpdatePeriodIn,
                                                       std::chrono::milliseconds minimumSendIntervalIn,
                                                       std::function< bool(const T&, const T&) > comparatorIn) noexcept
    {
        SendTrigger sendTrigger{};
        sendTrigger.comparator          = std::move(comparatorIn);
        sendTrigger.dataUpdatePeriod    = dataUpdatePeriodIn;
        sendTrigger.resetUpdatePeriod   = false;
        sendTrigger.minimumSendInterval = minimumSendIntervalIn;
        return sendTrigger;
    }
    /// @brief Preset mode -- Periodic or change trigger resettable (CyclicAndOnChangeEnableReset)
    /// @details Behavior:
    /// @details - Send/Update conditional sending (send period can be reset, send on difference or period reached)
    /// @details - Periodic sending possible (no minimum interval)
    /// @details - Trigger comparison possible
    static SendTrigger CyclicAndOnChangeEnableReset(std::chrono::milliseconds dataUpdatePeriodIn,
                                                    std::function< bool(const T&, const T&) > comparatorIn) noexcept
    {
        SendTrigger sendTrigger{};
        sendTrigger.comparator          = std::move(comparatorIn);
        sendTrigger.dataUpdatePeriod    = dataUpdatePeriodIn;
        sendTrigger.resetUpdatePeriod   = true;
        sendTrigger.minimumSendInterval = std::chrono::milliseconds{0};
        return sendTrigger;
    }
    /// @brief Preset mode -- Periodic or change trigger resettable with minimum interval (CyclicAndOnChangeEnableResetAndInterval)
    /// @details Behavior:
    /// @details - Send/Update conditional sending (send period can be reset, exceeds minimum send interval, send on difference or period reached)
    /// @details - Periodic sending possible (with minimum interval)
    /// @details - Trigger comparison possible
    static SendTrigger CyclicAndOnChangeEnableResetAndInterval(
        std::chrono::milliseconds dataUpdatePeriodIn,
        std::chrono::milliseconds minimumSendIntervalIn,
        std::function< bool(const T&, const T&) > comparatorIn) noexcept
    {
        SendTrigger sendTrigger{};
        sendTrigger.comparator          = std::move(comparatorIn);
        sendTrigger.dataUpdatePeriod    = dataUpdatePeriodIn;
        sendTrigger.resetUpdatePeriod   = true;
        sendTrigger.minimumSendInterval = minimumSendIntervalIn;
        return sendTrigger;
    }
};
/// @brief Send context template -- Skeleton side
/// @code{.isoft}
/// export_level=/COM/SoC
/// @endcode
/// @ref [TPS_MANI_03210] -- Specification of event specific communication attributes
/// @ref [TPS_MANI_03210] -- Specification of field specific communication attributes
/// @ref [TPS_MANI_03212] -- Specification of initial value for a field
/// @ref [TPS_MANI_01107] -- Specification of intentions for the sender of events or field notifiers
template < typename T >
struct SendContext
{
    /// @brief Send trigger
    SendTrigger< T > sendTrigger{};
    /// @brief Last send time
    std::chrono::steady_clock::time_point last{};
    /// @brief Timer handle
    isoft::naicpp::EvLoop::TimerPtr timer{};

    /// @brief Type alias -- Mark as enumerable
    using IsEnumerableTag = void;
    /// @brief Enumeration operation -- enumerate members of this type via a passed function object
    /// @tparam F Passed function object type
    /// @param[in] fun Passed function object
    template < typename F >
    void enumerate(F& fun)  // NOLINT -- Template interface name > naming convention
    {
        fun("trigger", sendTrigger);
        fun("elapsed(us)", (std::chrono::steady_clock::now() - last).count());
    }
};
#endif  // HAS_COM_EVENT_SEND_TRIGGER

#ifdef HAS_COM_EVENT_LOST_CHECKER
/// @brief Sample loss status -- Proxy side
/// @code{.isoft}
/// export_level=/COM/SoC
/// @endcode
struct SampleLostStatus
{
    /// @brief Number of lost samples
    size_t totalCount{};
    /// @brief Increment since last status read
    size_t totalCountChange{};

    /// @brief Type alias -- Mark as enumerable
    using IsEnumerableTag = void;
    /// @brief Enumeration operation -- enumerate members of this type via a passed function object
    /// @tparam F Passed function object type
    /// @param[in] fun Passed function object
    template < typename F >
    void enumerate(F& fun)  // NOLINT -- Template interface name > naming convention
    {
        fun(totalCount);
        fun(totalCountChange);
    }
};
/// @brief Sample loss handler -- Proxy side
/// @code{.isoft}
/// export_level=/COM/SoC
/// @endcode
/// @note Only notifies loss, does not return available data
using SampleLostHandler = std::function< void(SampleLostStatus const&) >;
#endif  // HAS_COM_EVENT_LOST_CHECKER
}  // namespace com
}  // namespace ara