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
/// @file       method.h
/// @brief      Communication service proxy method header file
/// @details
/// @date       2021-12-31
/// @author     mazelin
/// @version    1.2.0
///
/// ================================================================

#pragma once

#include "ara/com/types.h"
#include "ara/core/future.h"

/// @brief Namespace -- Internal proxy
namespace ara {
namespace com {
namespace internal {
namespace proxy {
/// @brief Method mode enumeration
/// @code{.isoft}
/// export_level=/COM/SoC/SOA
/// @endcode
enum class MethodMode : uint8_t
{
    kNonReturn,  ///< One-way method
    kHasReturn,  ///< Two-way method
};

/// @brief Template type -- Service proxy method -- Declaration
/// @tparam T Method signature type
/// @tparam mode Method mode -- default two-way method
template < typename T, MethodMode mode = MethodMode::kHasReturn >
class Method;

/// @brief Template type -- Check if it is a method type (No)
/// @tparam T
template < typename T >
struct IsMethod : std::false_type
{
};
/// @brief Template type -- Check if it is a method type (Yes)
/// @tparam T
template < typename T, MethodMode mode >
struct IsMethod< Method< T, mode > > : std::true_type
{
};
/// @brief Template type -- Check if it is a one-way method type (No)
/// @tparam T
template < typename T >
struct IsFMethod : std::false_type
{
};
/// @brief Template type -- Check if it is a one-way method type (Yes)
/// @tparam T
template < typename... Args >
struct IsFMethod< Method< void(Args...), MethodMode::kNonReturn > > : std::true_type
{
};
/// @brief Template type -- Check if it is a two-way void return method type (No)
/// @tparam T
template < typename T >
struct IsVMethod : std::false_type
{
};
/// @brief Template type -- Check if it is a two-way void return method type (Yes)
/// @tparam T
template < typename... Args >
struct IsVMethod< Method< void(Args...), MethodMode::kHasReturn > > : std::true_type
{
};
/// @brief Template type -- Check if it is a two-way method type (No)
/// @tparam T
template < typename T >
struct IsRMethod : std::false_type
{
};
/// @brief Template type -- Check if it is a two-way method type (Yes)
/// @tparam T
template < typename R, typename... Args >
struct IsRMethod< Method< R(Args...), MethodMode::kHasReturn > > : std::true_type
{
};

/// @brief Type alias -- One-way method
template < typename T, typename = std::enable_if_t< IsFMethod< Method< T, MethodMode::kNonReturn > >::value, T > >
using FMethod = Method< T, MethodMode::kNonReturn >;
/// @brief Type alias -- Two-way void return method
template < typename T, typename = std::enable_if_t< IsVMethod< Method< T, MethodMode::kHasReturn > >::value, T > >
using VMethod = Method< T, MethodMode::kHasReturn >;
/// @brief Type alias -- Two-way method
template < typename T, typename = std::enable_if_t< IsRMethod< Method< T, MethodMode::kHasReturn > >::value, T > >
using RMethod = Method< T, MethodMode::kHasReturn >;

/// @brief Template type -- Method base type -- Declaration
/// @tparam T Method signature type
template < typename T, MethodMode mode >
class MethodBase;
/// @brief Template type -- Method base type -- Specialized for one-way method
/// @code{.isoft}
/// export_level=/COM/SoC/SOA
/// @endcode
/// @tparam Args Method parameter list types
template < typename... Args >
class MethodBase< void(Args...), MethodMode::kNonReturn >
{
public:
    /// @brief Type alias -- Method signature
    using Signature = void(Args...);
    /// @brief Type alias -- Return type
    using Output = void;
    /// @brief Constructor
    MethodBase() noexcept { ComLogTrace(""); }
    /// @brief Destructor
    virtual ~MethodBase() noexcept { ComLogTrace(""); }
    /// @brief Copy constructor
    /// @param other
    MethodBase(MethodBase const& other) noexcept = default;
    /// @brief Move constructor
    /// @param other
    MethodBase(MethodBase&& other) noexcept = default;
    /// @brief Copy assignment operator
    /// @param other
    /// @return MethodBase
    MethodBase& operator=(MethodBase const& other) noexcept = default;
    /// @brief Move assignment operator
    /// @param other
    /// @return MethodBase
    MethodBase& operator=(MethodBase&& other) noexcept = default;
    /// @brief Operator -- call
    /// @param[in] args Parameter list
    virtual void operator()(Args&&... args) noexcept = 0;
};
/// @brief Template type -- Method base type -- Specialized for two-way method
/// @code{.isoft}
/// export_level=/COM/SoC/SOA
/// @endcode
/// @tparam R Method return type
/// @tparam Args Method parameter list types
template < typename R, typename... Args >
class MethodBase< R(Args...), MethodMode::kHasReturn >
{
public:
    /// @brief Type alias -- Method signature
    using Signature = R(Args...);
    /// @brief Type alias -- Return type
    using Output = std::remove_cv_t< std::remove_reference_t< R > >;
    /// @brief Constructor
    MethodBase() noexcept { ComLogTrace(""); }
    /// @brief Destructor
    virtual ~MethodBase() noexcept { ComLogTrace(""); }
    /// @brief Copy constructor
    /// @param other
    MethodBase(MethodBase const& other) noexcept = default;
    /// @brief Move constructor
    /// @param other
    MethodBase(MethodBase&& other) noexcept = default;
    /// @brief Copy assignment operator
    /// @param other
    /// @return MethodBase
    MethodBase& operator=(MethodBase const& other) noexcept = default;
    /// @brief Move assignment operator
    /// @param other
    /// @return MethodBase
    MethodBase& operator=(MethodBase&& other) noexcept = default;
    /// @brief Operator -- call
    /// @param[in] args Parameter list
    /// @return Future object -- asynchronous/synchronous wait for result object
    virtual ara::core::Future< Output > operator()(Args&&... args) noexcept = 0;
    /// @brief Get the global E2E SMState
    /// @return Result of the last called E2E_check function
    virtual e2e::SMState GetSMState() const noexcept { return e2e::SMState{}; }
};

/// @brief Template type -- Service proxy method -- Specialized for one-way method
/// @code{.isoft}
/// export_level=/COM/SoC/SOA
/// @endcode
/// @tparam Args Method parameter list types
template < typename... Args >
class Method< void(Args...), MethodMode::kNonReturn >
{
public:
    /// @brief Type alias -- Method implementation type
    using MethodImpl = MethodBase< void(Args...), MethodMode::kNonReturn >;
    /// @brief Type alias -- Method signature
    using Signature = typename MethodImpl::Signature;
    /// @brief Type alias -- Return type
    using Output = typename MethodImpl::Output;
    /// @brief Constructor
    Method() noexcept { ComLogTrace("create service proxy method"); }
    /// @brief Destructor
    ~Method() noexcept { ComLogTrace("destroy service proxy method"); }
    /// @brief Copy constructor
    /// @param[in] other
    Method(Method const& other) noexcept = delete;
    /// @brief Copy assignment operator
    /// @param[in] other
    /// @return Method&
    Method& operator=(Method const& other) noexcept = delete;
    /// @brief Move constructor
    Method(Method&& other) noexcept = default;
    /// @brief Move assignment operator
    /// @return Method&
    Method& operator=(Method&& other) noexcept = default;
    /// @brief Overloaded assignment operator
    /// @param[in] impl
    /// @return Method&
    Method& operator=(std::shared_ptr< MethodImpl >&& impl) noexcept
    {
        ComLogTrace("assign bind proxy method");
        impl_ = std::move(impl);
        return *this;
    }
    /// @brief Operator -- call
    /// @code{.isoft}
    /// export_level=/COM/SoC/SOA
    /// @endcode
    /// @param[in] args Parameter list
    /// @ref [SWS_CM_90435] -- Initiate a Fire and Forget method call
    void operator()(Args&&... args) noexcept
    {
        TimeRecorder(TimeNM::Timer, "APIRequest(F)");
        ComLogTrace("call service proxy method");
        assert(impl_);
        impl_->operator()(std::forward< Args >(args)...);
    }

private:
    std::shared_ptr< MethodImpl > impl_;
};
/// @brief Template type -- Method base type -- Specialized for two-way method
/// @code{.isoft}
/// export_level=/COM/SoC/SOA
/// @endcode
/// @tparam R Method return type
/// @tparam Args Method parameter list types
template < typename R, typename... Args >
class Method< R(Args...), MethodMode::kHasReturn >
{
public:
    /// @brief Type alias -- Method implementation type
    using MethodImpl = MethodBase< R(Args...), MethodMode::kHasReturn >;
    /// @brief Type alias -- Method signature
    using Signature = typename MethodImpl::Signature;
    /// @brief Type alias -- Return type
    using Output = typename MethodImpl::Output;
    /// @brief Constructor
    Method() noexcept { ComLogTrace("create service proxy method"); }
    /// @brief Destructor
    ~Method() noexcept { ComLogTrace("destroy service proxy method"); }
    /// @brief Copy constructor
    /// @param[in] other
    Method(Method const& other) noexcept = delete;
    /// @brief Copy assignment operator
    /// @param[in] other
    /// @return Method&
    Method& operator=(Method const& other) noexcept = delete;
    /// @brief Move constructor
    Method(Method&& other) noexcept = default;
    /// @brief Move assignment operator
    /// @return Method&
    Method& operator=(Method&& other) noexcept = default;
    /// @brief Overloaded assignment operator
    /// @param[in] impl
    /// @return Method&
    Method& operator=(std::shared_ptr< MethodImpl >&& impl) noexcept
    {
        ComLogTrace("assign bind proxy method");
        impl_ = std::move(impl);
        return *this;
    }
    /// @brief Operator -- call
    /// @code{.isoft}
    /// export_level=/COM/SoC/SOA
    /// @endcode
    /// @param[in] args Parameter list
    /// @return Future object -- asynchronous/synchronous wait for result object
    /// @ref [SWS_CM_00196] -- Initiate a method call
    /// @ref [SWS_CM_00032] -- Re-entrancy – Method call operator
    /// @ref [SWS_CM_10440] -- Aborting method calls in case of locally detected failures
    /// @ref [SWS_CM_00194] -- Cancel the method call
    /// @ref [SWS_CM_00195] -- Retrieving results of the method call
    /// @ref [SWS_CM_00192] -- Synchronous behavior of method call
    /// @ref [SWS_CM_00193] -- Asynchronous behavior of method call with polling
    /// @ref [SWS_CM_00197] -- Asynchronous behavior of method call with notification
    ara::core::Future< Output > operator()(Args&&... args) noexcept
    {
        TimeRecorder(TimeNM::Timer, "APIRequest(R)");
        ComLogTrace("call service proxy method");
        assert(impl_);
        return impl_->operator()(std::forward< Args >(args)...);
    }
    /// @brief Get the global E2E SMState
    /// @code{.isoft}
    /// export_level=/COM/SoC/SOA
    /// @endcode
    /// @return Result of the last called E2E_check function
    e2e::SMState GetSMState() noexcept
    {
        ComLogTrace("");
        assert(impl_);
        return impl_->GetSMState();
    }

private:
    std::shared_ptr< MethodImpl > impl_;
};
}  // namespace proxy
}  // namespace internal
}  // namespace com
}  // namespace ara