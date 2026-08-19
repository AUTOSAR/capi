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
/// @file       runtime.h
/// @brief      Communication runtime header file
/// @details    Provides functionality that cannot be truly assigned to proxy/skeleton domains, concerning cross-cutting concerns
/// @date       2021-12-31
/// @author     mazelin
/// @version    1.2.0
///
/// ================================================================
///
/// @ref AUTOSAR_EXP_ARAComAPI [Runtime Interface][5.2] and [Runtime][6.4]
///
/// ================================================================

#pragma once

#include "ara/com/internal/threadpool.h"
#include "ara/com/types.h"

namespace ara {
namespace com {
namespace runtime {
/// @brief Convert instance specifier
/// @code{.isoft}
/// export_level=/COM/SoC
/// @endcode
/// @param[in] instanceSpecifier Instance specifier
/// @return Result object -- empty/value or error
/// @ref [SWS_CM_00118] -- Method Instance Specifier Translation
ara::core::Result< InstanceIdentifierContainer > ResolveInstanceIDs(
    ara::core::InstanceSpecifier const& instanceSpecifier) noexcept;
}  // namespace runtime
namespace internal {
/// @brief Initialize communication (for caller usage)
/// @code{.isoft}
/// export_level=/COM
/// @endcode
/// @return Result object -- empty/value or error
ara::core::Result< void > Initialize() noexcept;
/// @brief Deinitialize communication (for caller usage)
/// @code{.isoft}
/// export_level=/COM
/// @endcode
/// @return Result object -- empty/value or error
ara::core::Result< void > Deinitialize() noexcept;
namespace proxy {
/// @brief Binding layer proxy handle type (for creating proxies) -- Declaration
class BindHandle;
}  // namespace proxy
namespace skeleton {
/// @brief Binding layer skeleton type -- Declaration
class BindSkeleton;
/// @brief Service skeleton type -- Declaration
class Skeleton;
}  // namespace skeleton
/// @brief Binding layer runtime type
/// @code{.isoft}
/// export_level=/COM/SoC/SOA
/// @endcode
class BindRuntime
{
public:
    /// @brief Constructor
    BindRuntime() noexcept;
    /// @brief Destructor
    virtual ~BindRuntime() noexcept;
    /// @brief Copy constructor
    /// @param other
    BindRuntime(BindRuntime const& other) noexcept = default;
    /// @brief Move constructor
    /// @param other
    BindRuntime(BindRuntime&& other) noexcept = default;
    /// @brief Copy assignment operator
    /// @param other
    /// @return BindRuntime
    BindRuntime& operator=(BindRuntime const& other) noexcept = default;
    /// @brief Move assignment operator
    /// @param other
    /// @return BindRuntime
    BindRuntime& operator=(BindRuntime&& other) noexcept = default;
    /// @brief Create a binding layer skeleton instance -- Skeleton side
    /// @param[in] skeleton Service skeleton
    /// @param[in] instanceIdentifier Instance identifier
    /// @param[in] bindSkeletons Set of skeleton instance pointers
    virtual void CreateBindSkeleton(
        skeleton::Skeleton& skeleton,
        InstanceIdentifier const& instanceIdentifier,
        ara::core::Vector< std::unique_ptr< skeleton::BindSkeleton > >& bindSkeletons) noexcept = 0;
    /// @brief Get set of available service instance handles -- For service discovery -- Proxy side
    /// @param[in] serviceIdentifier Service identifier
    /// @param[in] instanceIdentifier Instance identifier
    /// @param[in] bindHandles Set of available service handles
    virtual void GetAvailableServiceHandles(
        ServiceIdentifier const& serviceIdentifier,
        InstanceIdentifier const& instanceIdentifier,
        ServiceHandleContainer< std::shared_ptr< proxy::BindHandle > >& bindHandles) noexcept = 0;
    /// @brief Register a find service handler -- For starting service discovery -- Proxy side
    /// @param[in] findHandle Find service handle
    /// @param[in] findHandler Find service handler
    virtual void RegisterFindServiceHandle(
        FindServiceHandle const& findHandle,
        FindServiceHandler< std::shared_ptr< proxy::BindHandle > > const& findHandler) noexcept = 0;
    /// @brief Unregister a find service handler -- For stopping service discovery -- Proxy side
    /// @param[in] findHandle Find service handle
    virtual void UnregisterFindServiceHandle(FindServiceHandle const& findHandle) noexcept = 0;
};
/// @brief Communication runtime type
/// @code{.isoft}
/// export_level=/COM/SoC/SOA
/// @endcode
/// @ref [RS_AP_00140] -- Making Adaptive Runtime classes final
class Runtime final
{
public:
    /// @brief Constructor
    Runtime() noexcept;
    /// @brief Destructor
    ~Runtime() noexcept;
    /// @brief Copy constructor
    /// @param other
    Runtime(Runtime const& other) noexcept = delete;
    /// @brief Move constructor
    /// @param other
    Runtime(Runtime&& other) noexcept = default;
    /// @brief Copy assignment operator
    /// @param other
    /// @return Runtime
    Runtime& operator=(Runtime const& other) noexcept = delete;
    /// @brief Move assignment operator
    /// @param other
    /// @return Runtime
    Runtime& operator=(Runtime&& other) noexcept = default;

private:
    /// @brief Initialize -- Declaration (implemented by the user)
    /// @return Result object -- empty/value or error
    ara::core::Result< void > Initialize() noexcept;  // NOLINT -- Standard interface name > naming convention
    /// @brief Deinitialize -- Declaration (implemented by the user)
    /// @return Result object -- empty/value or error
    ara::core::Result< void > Deinitialize() noexcept;  // NOLINT -- Standard interface name > naming convention
    /// @brief Initialize communication (friend)
    friend ara::core::Result< void > Initialize() noexcept;
    /// @brief Deinitialize communication (friend)
    friend ara::core::Result< void > Deinitialize() noexcept;
    /// @brief Convert instance specifier (friend)
    /// @return Result object -- empty/value or error
    friend ara::core::Result< InstanceIdentifierContainer > runtime::ResolveInstanceIDs(
        ara::core::InstanceSpecifier const& instanceSpecifier) noexcept;

public:
    /// @brief Initialize -- Declaration (implemented by the diagnostic module)
    /// @return Result object -- empty/value or error
    ara::core::Result< void > InitializeForDiag() noexcept;
    /// @brief Initialize -- Declaration (implemented by the communication group module)
    /// @return Result object -- empty/value or error
    ara::core::Result< void > InitializeForComCG() noexcept;

public:
    /// @brief Register error domain
    /// @param[in] domain Error domain
    void RegisterErrorDomain(ara::core::ErrorDomain const& domain) noexcept;
    /// @brief Get error domain
    /// @param[in] id Error domain identifier
    /// @return Pointer to error domain
    ara::core::ErrorDomain const* GetErrorDomain(ara::core::ErrorDomain::IdType id) const noexcept;
    /// @brief Get process name
    /// @return Process name
    ara::core::StringView GetProcessName() const noexcept;
    /// @brief Get thread pool
    /// @param[in] name Thread pool name
    /// @return Pointer to thread pool
    std::unique_ptr< ThreadPool > const& GetThreadPool(ara::core::StringView name) const noexcept;
    /// @brief Create thread pool
    /// @param[in] name Thread pool name
    /// @param[in] size Thread pool size
    /// @return Pointer to thread pool
    std::unique_ptr< ThreadPool > const& CreateThreadPool(ara::core::StringView name, size_t size) noexcept;
    /// @brief Destroy thread pool
    /// @return bool
    bool DestroyThreadPool(ara::core::StringView name) noexcept;
    /// @brief Binding layer runtime instance type
    using BindRuntime = std::unique_ptr< internal::BindRuntime >;
    /// @brief Binding layer runtime instance set type
    using BindRuntimes = ara::core::Vector< BindRuntime >;
    /// @brief Register a binding layer runtime instance
    /// @param[in] bindRuntime Binding layer runtime instance
    void RegisterBindRuntime(BindRuntime&& bindRuntime) noexcept;
    /// @brief Get the set of binding layer runtime instances
    BindRuntimes const& GetBindRuntimes() const noexcept;
    /// @brief Get the instance specifier to which the instance identifier set belongs
    /// @return Instance specifier to which the instance identifier set belongs
    ara::core::Vector< ara::core::InstanceSpecifier > GetInstanceSpecifier(
        InstanceIdentifierContainer const& instanceIdentifiers) const noexcept;
#ifdef HAS_MONITOR_LOG
    /// @brief Get the full qualified name
    /// @return Full qualified name
    ara::core::String const& GetFullQualifiedName() const noexcept;
#endif

    /// @brief Type alias -- Mark as enumerable
    using IsEnumerableTag = void;
    /// @brief Enumeration operation -- enumerate members of this type via a passed function object
    /// @tparam F Passed function object type
    /// @param[in] fun Passed function object
    template < typename F >
    void enumerate(F& fun)  // NOLINT -- Template interface name > naming convention
    {
        fun(impl_);
    }

private:
    /// @brief Communication runtime implementation type -- Declaration
    class Impl;
    /// @brief Communication runtime implementation
    std::unique_ptr< Impl > impl_;
};
/// @brief Get the runtime instance
/// @code{.isoft}
/// export_level=/COM/SoC/SOA
/// @endcode
Runtime& GetInstance() noexcept;
}  // namespace internal
}  // namespace com
}  // namespace ara