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
/// @file       skeleton.h
/// @brief      Communication service skeleton header file
/// @details
/// @date       2021-12-31
/// @author     mazelin
/// @version    1.2.0
///
/// ================================================================

#pragma once

#include "ara/com/internal/runtime.h"
#include "ara/com/types.h"
#include "ara/core/promise.h"

/// @brief Namespace -- Internal skeleton
namespace ara {
namespace com {
namespace internal {
namespace skeleton {
/// @brief Binding layer skeleton type
/// @code{.isoft}
/// export_level=/COM/SoC/SOA
/// @endcode
class BindSkeleton
{
public:
    /// @brief Constructor
    BindSkeleton() noexcept;
    /// @brief Copy constructor
    /// @param other
    BindSkeleton(BindSkeleton const& other) noexcept = default;
    /// @brief Move constructor
    /// @param other
    BindSkeleton(BindSkeleton&& other) noexcept = default;
    /// @brief Copy assignment operator
    /// @param other
    /// @return
    BindSkeleton& operator=(BindSkeleton const& other) noexcept = default;
    /// @brief Move assignment operator
    /// @param other
    /// @return
    BindSkeleton& operator=(BindSkeleton&& other) noexcept = default;
    /// @brief Destructor
    virtual ~BindSkeleton() noexcept;
    /// @brief Offer service
    /// @return Result object -- empty/value or error
    virtual ara::core::Result< void > OfferService() noexcept = 0;
    /// @brief Stop offering service
    virtual void StopOfferService() noexcept = 0;
    /// @brief Process next method call
    /// @return Future object -- asynchronous/synchronous wait for result object
    virtual ara::core::Future< bool > ProcessNextMethodCall() noexcept = 0;
};

/// @brief Service skeleton type
/// @code{.isoft}
/// export_level=/COM/SoC/SOA
/// @endcode
class Skeleton
{
public:
    /// @brief Constructor
    Skeleton() noexcept;
    /// @brief Destructor
    virtual ~Skeleton() noexcept;
    /// @brief Copy constructor
    /// @param other
    Skeleton(Skeleton const& other) noexcept = default;
    /// @brief Move constructor
    /// @param other
    Skeleton(Skeleton&& other) noexcept = default;
    /// @brief Copy assignment operator
    /// @param other
    /// @return
    Skeleton& operator=(Skeleton const& other) noexcept = default;
    /// @brief Move assignment operator
    /// @param other
    /// @return
    Skeleton& operator=(Skeleton&& other) noexcept = default;
    /// @brief Get service identifier
    /// @return Service identifier
    virtual ServiceIdentifier const& GetServiceIdentifier() const noexcept = 0;
    /// @brief Offer service
    /// @return Result object -- empty/value or error
    virtual ara::core::Result< void > OfferService() noexcept = 0;
    /// @brief Stop offering service
    virtual void StopOfferService() noexcept = 0;
    /// @brief Process next method call
    /// @return Future object -- asynchronous/synchronous wait for result object
    virtual ara::core::Future< bool > ProcessNextMethodCall() noexcept = 0;
    /// @brief Get method call processing mode
    /// @return Method call processing mode
    virtual MethodCallProcessingMode GetMethodCallProcessingMode() const noexcept = 0;
};

/// @brief Template type -- Service skeleton
/// @code{.isoft}
/// export_level=/COM/SoC/SOA
/// @endcode
/// @tparam TSkeleton Application skeleton type
/// @note All constructors should be private @ref [SWS_CM_11326]
template < typename TSkeleton >
class ServiceSkeleton : public Skeleton
{
#if __GLIBCXX__ == 20181026 || true  // j3 not support
public:
#else
protected:
#endif
    /// @brief Constructor
    /// @code{.isoft}
    /// export_level=/COM/SoC/SOA
    /// @endcode
    /// @param[in] instanceIdentifier Instance identifier
    /// @param[in] mode Method call processing mode -- default parallel event-driven mode
    /// @ref [SWS_CM_00130] -- Creation of service skeleton using Instance ID
    /// @ref [SWS_CM_00198] -- Set service method processing mode
    explicit ServiceSkeleton(
        InstanceIdentifier instanceIdentifier,  // NOLINT -- Compatibility > [performance-unnecessary-value-param]
        MethodCallProcessingMode mode = MethodCallProcessingMode::kEvent)
        : ServiceSkeleton{InstanceIdentifierContainer{instanceIdentifier}, mode}  // NOLINT -- If no longer compatible, pass through check
    {
        ComLogTrace("create service skeleton", GenArg(instanceIdentifier), GenArg(mode));
    }
    /// @brief Constructor
    /// @code{.isoft}
    /// export_level=/COM/SoC/SOA
    /// @endcode
    /// @param[in] instanceSpecifier Instance specifier
    /// @param[in] mode Method call processing mode -- default parallel event-driven mode
    /// @ref [SWS_CM_00152] -- Creation of service skeleton using Instance Spec
    /// @ref [SWS_CM_00198] -- Set service method processing mode
    explicit ServiceSkeleton(
        ara::core::InstanceSpecifier instanceSpecifier,  // NOLINT -- Compatibility > [performance-unnecessary-value-param]
        MethodCallProcessingMode mode = MethodCallProcessingMode::kEvent)
        : ServiceSkeleton{runtime::ResolveInstanceIDs(instanceSpecifier).ValueOrThrow(), mode}
    {
        ComLogTrace("create service skeleton", GenArg(instanceSpecifier), GenArg(mode));
    }
    /// @brief Constructor
    /// @code{.isoft}
    /// export_level=/COM/SoC/SOA
    /// @endcode
    /// @param[in] instanceIdentifiers Set of instance identifiers
    /// @param[in] mode Method call processing mode -- default parallel event-driven mode
    /// @ref [SWS_CM_00153] -- Creation of service skeleton using Instance ID Container
    /// @ref [SWS_CM_00198] -- Set service method processing mode
    explicit ServiceSkeleton(
        InstanceIdentifierContainer instanceIdentifiers,  // NOLINT -- Compatibility > [performance-unnecessary-value-param]
        MethodCallProcessingMode mode = MethodCallProcessingMode::kEvent)
        : this_{new ThisPointer{static_cast< TSkeleton* >(this)}}
        , bindSkeletonsIterator_{bindSkeletons_.end()}
        , instanceIdentifiers_{instanceIdentifiers}  // NOLINT -- If no longer compatible, pass through check
        , mode_{mode}
    {
        ComLogTrace("create service skeleton", GenArg(instanceIdentifiers), GenArg(mode));
    }
    /// @brief Initialization function (called by derived class)
    /// @return Result object -- empty/value or error
    ara::core::Result< void > Initialize() noexcept
    {
        using Result = ara::core::Result< void >;
        std::lock_guard< std::mutex > lock(mutex_);
        ComLogTrace("initialize service skeleton", GenArg(instanceIdentifiers_), GenArg(mode_));
        if (!bindSkeletons_.empty()) {
            ComLogDebug("initialize service skeleton ignored: bind skeleton exist", GenArg(instanceIdentifiers_));
            return Result{};
        }
        auto const& comRuntime{GetInstance()};
        if (comRuntime.GetInstanceSpecifier(instanceIdentifiers_).empty()) {
            ComLogError("initialize service skeleton rejected: get instance specifier failed",
                        GenArg(instanceIdentifiers_));
            return Result{MakeErrorCode(ComErrc::kInstanceIDCouldNotBeResolved, -__LINE__)};
        }
        auto const& bindRuntimes{comRuntime.GetBindRuntimes()};
        if (bindRuntimes.empty()) {
            ComLogError("initialize service skeleton rejected: bind runtime not exist", GenArg(instanceIdentifiers_));
            return Result{MakeErrorCode(ComErrc::kNetworkBindingFailure, -__LINE__)};
        }
        for (auto it{instanceIdentifiers_.begin()}; it != instanceIdentifiers_.end();) {
            auto const& instanceIdentifier{*it};
            if (InstanceIdentifierss().find(instanceIdentifier) != InstanceIdentifierss().end()) {
                ComLogError("initialize service skeleton rejected: create bind skeleton repeat",
                            GenArg(instanceIdentifier));
                return Result{MakeErrorCode(ComErrc::kInstanceIDCouldNotBeResolved, -__LINE__)};
            }
            auto size{bindSkeletons_.size()};
            for (auto const& bindRuntime : bindRuntimes) {
                bindRuntime->CreateBindSkeleton(*this, instanceIdentifier, bindSkeletons_);
                if (size != bindSkeletons_.size()) {
                    break;
                }
            }
            if (size == bindSkeletons_.size()) {
                ComLogTrace("initialize service skeleton skipped: create bind skeleton empty",
                            GenArg(instanceIdentifier));
                it = instanceIdentifiers_.erase(it);
                continue;
            }
            ++it;
            std::ignore = InstanceIdentifierss().emplace(instanceIdentifier);
        }
        if (bindSkeletons_.empty()) {
            ComLogError("initialize service skeleton error: create bind skeleton failed", GenArg(instanceIdentifiers_));
            return Result{MakeErrorCode(ComErrc::kNetworkBindingFailure, -__LINE__)};
        }
        ComLogDebug("initialize service skeleton done", GenArg(instanceIdentifiers_), GenArg(mode_));
        return Result{};
    }

public:
    /// @brief Destructor
    ~ServiceSkeleton() noexcept override
    {
        ComLogTrace("destroy service skeleton", GenArg(instanceIdentifiers_), GenArg(mode_));
        std::lock_guard< std::mutex > lock(mutex_);
        bindSkeletons_.clear();
        bindSkeletonsIterator_ = bindSkeletons_.end();
        for (auto it{instanceIdentifiers_.begin()}; it != instanceIdentifiers_.end(); ++it) {
            auto& instanceIdentifier{*it};
            std::ignore = InstanceIdentifierss().erase(instanceIdentifier);
        }
        if (this_) {
            this_->pointer_ = nullptr;
        }
    }
    /// @brief Copy constructor
    /// @param[in] other
    /// @ref [SWS_CM_00134] -- Copy semantics of service skeleton class
    ServiceSkeleton(ServiceSkeleton const& other) noexcept = delete;
    /// @brief Copy assignment operator
    /// @param[in] other
    /// @return ServiceSkeleton&
    ServiceSkeleton& operator=(ServiceSkeleton const& other) noexcept = delete;
    /// @brief Move constructor
    /// @param[in] other
    /// @ref [SWS_CM_00135] -- Move semantics of service skeleton class
    ServiceSkeleton(ServiceSkeleton&& other) noexcept
        : lock_{other.mutex_}
        , this_{other.this_}
        , bindSkeletons_{std::move(other.bindSkeletons_)}
        , bindSkeletonsIterator_{bindSkeletons_.end()}
        , instanceIdentifiers_{std::move(other.instanceIdentifiers_)}
        , mode_{std::move(other.mode_)}
    {
        ComLogTrace("");
        other.bindSkeletons_.clear();
        other.bindSkeletonsIterator_ = other.bindSkeletons_.end();
        other.this_->pointer_        = static_cast< TSkeleton* >(this);
        other.this_                  = nullptr;
        lock_.unlock();
    }
    /// @brief Move assignment operator
    /// @param[in] other
    /// @return ServiceSkeleton&
    ServiceSkeleton& operator=(ServiceSkeleton&& other) noexcept
    {
        ComLogTrace("");
        if (this != &other) {
            std::lock(mutex_, other.mutex_);
            std::lock_guard< std::mutex > lock1(mutex_, std::adopt_lock);
            std::lock_guard< std::mutex > lock2(other.mutex_, std::adopt_lock);
            this_                  = other.this_;
            bindSkeletons_         = std::move(other.bindSkeletons_);
            bindSkeletonsIterator_ = bindSkeletons_.end();
            instanceIdentifiers_   = std::move(other.instanceIdentifiers_);
            mode_                  = std::move(other.mode_);
            other.bindSkeletons_.clear();
            other.bindSkeletonsIterator_ = other.bindSkeletons_.end();
            other.this_->pointer_        = static_cast< TSkeleton* >(this);
            other.this_                  = nullptr;
        }
        return *this;
    }

public:
    /// @brief Create a skeleton instance
    /// @code{.isoft}
    /// export_level=/COM/SoC/SOA
    /// @endcode
    /// @param[in] instanceIdentifier Instance identifier
    /// @param[in] mode Method call processing mode -- default parallel event-driven mode
    /// @return Result object -- empty/value or error
    /// @retval ComErrc::kWrongMethodCallProcessingMode Wrong method processing mode @ref [SWS_CM_10467]
    /// @retval ComErrc::kGrantEnforcementError Request rejected by the authorization enforcement layer @ref [SWS_CM_90005]
    /// @ref [SWS_CM_11326] -- Creation of an object using Named Constructor approach
    /// @ref [SWS_CM_10435] -- Exception-less creation of service skeleton using Instance ID
    /// @ref [SWS_CM_00198] -- Set service method processing mode
    template < typename ClassToBeCreated = TSkeleton >
    static ara::core::Result< ClassToBeCreated > Create(InstanceIdentifier const& instanceIdentifier,
                                                        MethodCallProcessingMode mode
                                                        = MethodCallProcessingMode::kEvent) noexcept
    {
        using Result = ara::core::Result< ClassToBeCreated >;
        static_assert(std::is_base_of< TSkeleton, ClassToBeCreated >::value, "invalid class to be created");
        try {
            ComLogTrace("create service skeleton", GenArg(instanceIdentifier), GenArg(mode));
            ClassToBeCreated skeleton{instanceIdentifier, mode};
            ComLogDebug("create service skeleton done", GenArg(instanceIdentifier), GenArg(mode));
            return Result{std::move(skeleton)};
        } catch (ara::core::Exception const& e) {
            ComLogError("create service skeleton exception", GenArg(instanceIdentifier), GenArg(mode),
                        GenArg(e.Error()));
            return Result{e.Error()};
        }
    }
    /// @brief Create a skeleton instance
    /// @code{.isoft}
    /// export_level=/COM/SoC/SOA
    /// @endcode
    /// @param[in] instanceSpecifier Instance specifier
    /// @param[in] mode Method call processing mode -- default parallel event-driven mode
    /// @return Result object -- empty/value or error
    /// @retval ComErrc::kWrongMethodCallProcessingMode Wrong method processing mode @ref [SWS_CM_10467]
    /// @retval ComErrc::kGrantEnforcementError Request rejected by the authorization enforcement layer @ref [SWS_CM_90005]
    /// @ref [SWS_CM_11326] -- Creation of an object using Named Constructor approach
    /// @ref [SWS_CM_10436] -- Exception-less creation of service skeleton using Instance Spec
    /// @ref [SWS_CM_00198] -- Set service method processing mode
    template < typename ClassToBeCreated = TSkeleton >
    static ara::core::Result< ClassToBeCreated > Create(ara::core::InstanceSpecifier const& instanceSpecifier,
                                                        MethodCallProcessingMode mode
                                                        = MethodCallProcessingMode::kEvent) noexcept
    {
        using Result = ara::core::Result< ClassToBeCreated >;
        static_assert(std::is_base_of< TSkeleton, ClassToBeCreated >::value, "invalid class to be created");
        try {
            ComLogTrace("create service skeleton", GenArg(instanceSpecifier), GenArg(mode));
            ClassToBeCreated skeleton{instanceSpecifier, mode};
            ComLogDebug("create service skeleton done", GenArg(instanceSpecifier), GenArg(mode));
            return Result{std::move(skeleton)};
        } catch (ara::core::Exception const& e) {
            ComLogError("create service skeleton exception", GenArg(instanceSpecifier), GenArg(mode),
                        GenArg(e.Error()));
            return Result{e.Error()};
        }
    }
    /// @brief Create a skeleton instance
    /// @code{.isoft}
    /// export_level=/COM/SoC/SOA
    /// @endcode
    /// @param[in] instanceIdentifiers Set of instance identifiers
    /// @param[in] mode Method call processing mode -- default parallel event-driven mode
    /// @return Result object -- empty/value or error
    /// @retval ComErrc::kWrongMethodCallProcessingMode Wrong method processing mode @ref [SWS_CM_10467]
    /// @retval ComErrc::kGrantEnforcementError Request rejected by the authorization enforcement layer @ref [SWS_CM_90005]
    /// @ref [SWS_CM_11326] -- Creation of an object using Named Constructor approach
    /// @ref [SWS_CM_10437] -- Exception-less creation of service skeleton using Instance ID Container
    /// @ref [SWS_CM_00198] -- Set service method processing mode
    template < typename ClassToBeCreated = TSkeleton >
    static ara::core::Result< ClassToBeCreated > Create(InstanceIdentifierContainer const& instanceIdentifiers,
                                                        MethodCallProcessingMode mode
                                                        = MethodCallProcessingMode::kEvent) noexcept
    {
        using Result = ara::core::Result< ClassToBeCreated >;
        static_assert(std::is_base_of< TSkeleton, ClassToBeCreated >::value, "invalid class to be created");
        try {
            ComLogTrace("create service skeleton", GenArg(instanceIdentifiers), GenArg(mode));
            ClassToBeCreated skeleton{instanceIdentifiers, mode};
            ComLogDebug("create service skeleton done", GenArg(instanceIdentifiers), GenArg(mode));
            return Result{std::move(skeleton)};
        } catch (ara::core::Exception const& e) {
            ComLogError("create service skeleton exception", GenArg(instanceIdentifiers), GenArg(mode),
                        GenArg(e.Error()));
            return Result{e.Error()};
        }
    }
    /// @brief Get service identifier
    /// @return Service identifier
    ServiceIdentifier const& GetServiceIdentifier() const noexcept override { return TSkeleton::serviceIdentifier; }
    /// @brief Offer service
    /// @code{.isoft}
    /// export_level=/COM/SoC/SOA
    /// @endcode
    /// @return Result object -- empty/value or error
    /// @retval ComErrc::kFieldValueIsNotValid Field value is not valid @ref [SWS_CM_00128]
    /// @retval ComErrc::kSetHandlerNotSet Field set handler not set @ref [SWS_CM_00129]
    /// @ref [SWS_CM_00101] -- Method to offer a service
    /// @ref [SWS_CM_00010] -- Re-entrancy – OfferService
    ara::core::Result< void > OfferService() noexcept override
    {
        using Result = ara::core::Result< void >;
        ComLogTrace("offer service", GenArg(instanceIdentifiers_), GenArg(mode_));
        std::lock_guard< std::mutex > lock(mutex_);
        if (bindSkeletons_.empty()) {
            ComLogError("offer service rejected: bind skeleton not exist", GenArg(instanceIdentifiers_), GenArg(mode_));
            return Result{MakeErrorCode(ComErrc::kServiceNotOffered, -__LINE__)};
        }
        Result result{};
        for (auto& bindSkeleton : bindSkeletons_) {
            auto res{bindSkeleton->OfferService()};
            if (!res) {
                result = std::move(res);
            }
        }
        ComLogDebug("offer service done", GenArg(instanceIdentifiers_), GenArg(mode_));
        return result;
    }
    /// @brief Stop offering service
    /// @code{.isoft}
    /// export_level=/COM/SoC/SOA
    /// @endcode
    /// @ref [SWS_CM_00111] -- Method to stop offering a service
    /// @ref [SWS_CM_00011] -- Re-entrancy – StopOfferService
    void StopOfferService() noexcept override
    {
        ComLogTrace("stop offer service", GenArg(instanceIdentifiers_), GenArg(mode_));
        std::lock_guard< std::mutex > lock(mutex_);
        if (bindSkeletons_.empty()) {
            ComLogWarning("stop offer service ignored: bind skeleton not exist", GenArg(instanceIdentifiers_),
                          GenArg(mode_));
            return;
        }
        for (auto& bindSkeleton : bindSkeletons_) {
            bindSkeleton->StopOfferService();
        }
        ComLogDebug("stop offer service done", GenArg(instanceIdentifiers_), GenArg(mode_));
    }
    /// @brief Process next method call
    /// @code{.isoft}
    /// export_level=/COM/SoC/SOA
    /// @endcode
    /// @return Future object -- asynchronous/synchronous wait for result object
    /// @retval ComErrc::kCouldNotExecute Cannot execute (e.g., due to resource issues)
    /// @ref [SWS_CM_11351] -- Error behaviour of provided Execution Context for process service method invocation
    /// @ref [SWS_CM_10362] -- Raising checked errors for application errors
    /// @ref [SWS_CM_00199] -- Process Service method invocation
    ara::core::Future< bool > ProcessNextMethodCall() noexcept override
    {
        return ProcessNextMethodCall([](std::function< void() > const& f) { f(); });
    }
    /// @brief Process next method call
    /// @code{.isoft}
    /// export_level=/COM/SoC/SOA
    /// @endcode
    /// @param[in] executor Executor -- the processing will take place in its context
    /// @return Future object -- asynchronous/synchronous wait for result object
    /// @retval ComErrc::kCouldNotExecute Cannot execute (e.g., due to resource issues)
    /// @ref [SWS_CM_11351] -- Error behaviour of provided Execution Context for process service method invocation
    /// @ref [SWS_CM_10362] -- Raising checked errors for application errors
    /// @ref [SWS_CM_11350] -- Execution Context for process service method invocation
    template < typename Executor >
    ara::core::Future< bool > ProcessNextMethodCall(Executor&& executor) noexcept
    {
        ara::core::Promise< bool > promise;
        if (!executor) {
            ComLogError("process next service method call rejected: executor invalid", GenArg(instanceIdentifiers_),
                        GenArg(mode_));
            promise.SetError(MakeErrorCode(ComErrc::kCouldNotExecute, -__LINE__));
            return promise.get_future();
        }
        if (bindSkeletons_.empty()) {
            ComLogError("process next service method call rejected: bind skeleton not exist",
                        GenArg(instanceIdentifiers_), GenArg(mode_));
            promise.SetError(MakeErrorCode(ComErrc::kNetworkBindingFailure, -__LINE__));
            return promise.get_future();
        }
        executor([&]() {
            std::lock_guard< std::mutex > lock(mutex_);
            auto iterator{bindSkeletonsIterator_};
            do {
                if (bindSkeletonsIterator_ == bindSkeletons_.end()) {
                    bindSkeletonsIterator_ = bindSkeletons_.begin();
                    if (bindSkeletonsIterator_ == bindSkeletons_.end()) {
                        break;
                    }
                }
                auto res{(*bindSkeletonsIterator_++)->ProcessNextMethodCall().GetResult()};
                if (!res) {
                    ComLogDebug("process next service method call error: process bind method call failed", GenArg(res),
                                GenArg(instanceIdentifiers_), GenArg(mode_));
                    promise.SetError(std::move(res).Error());
                    return;
                }
                if (res.Value()) {
                    ComLogTrace("process next service method call done", GenArg(instanceIdentifiers_), GenArg(mode_));
                    promise.set_value(std::move(res).Value());
                    return;
                }
            } while (bindSkeletonsIterator_ != iterator);
            ComLogTrace("process next service method call ignored", GenArg(instanceIdentifiers_), GenArg(mode_));
            promise.set_value(false);
        });
        return promise.get_future();
    }
    /// @brief Get method call processing mode
    /// @code{.isoft}
    /// export_level=/COM/SoC/SOA
    /// @endcode
    /// @return Method call processing mode
    MethodCallProcessingMode GetMethodCallProcessingMode() const noexcept override { return mode_; }
    /// @brief E2E error handler -- default
    /// @code{.isoft}
    /// export_level=/COM/SoC/SOA
    /// @endcode
    /// @param[in] errorCode ProfileCheckStatus obtained from kE2ECheck
    /// @param[in] dataID Data identifier
    /// @param[in] messageCounter E2E counter of the received request message
    virtual void E2EErrorHandler(ara::com::e2e::E2EErrorCode errorCode,
                                 ara::com::e2e::DataID dataID,
                                 ara::com::e2e::MessageCounter messageCounter) noexcept
    {
        std::ignore = errorCode;
        std::ignore = dataID;
        std::ignore = messageCounter;
    };

public:
    /// @brief Pointer wrapper type
    class ThisPointer
    {
    private:
        /// @brief Skeleton pointer
        TSkeleton* pointer_;
        friend class ServiceSkeleton;
        /// @brief Constructor
        /// @param[in] pointer
        explicit ThisPointer(TSkeleton* pointer) noexcept : pointer_{pointer}
        {
            ComLogTrace("");
            assert(pointer_ != nullptr);
        }

    public:
        /// @brief Destructor
        ~ThisPointer() noexcept
        {
            ComLogTrace("");
            assert(pointer_ == nullptr);
        }
        /// @brief Copy constructor
        /// @param[in] other
        ThisPointer(ThisPointer const& other) noexcept = delete;
        /// @brief Copy assignment operator
        /// @param[in] other
        /// @return ThisPointer&
        ThisPointer& operator=(ThisPointer const& other) noexcept = delete;
        /// @brief Move constructor
        /// @param[in] other
        ThisPointer(ThisPointer&& other) noexcept = delete;
        /// @brief Move assignment operator
        /// @param[in] other
        /// @return ThisPointer&
        ThisPointer& operator=(ThisPointer&& other) noexcept = delete;
        /// @brief Operator -- returns reference
        /// @return Skeleton reference
        TSkeleton& operator*() const noexcept { return *pointer_; }
        /// @brief Operator -- returns pointer
        /// @return Skeleton pointer
        TSkeleton* operator->() const noexcept { return pointer_; }
        /// @brief Operator -- implicit conversion
        /// @return bool
        explicit operator bool() const noexcept { return pointer_; }
    };
    /// @brief Type alias -- Shared skeleton pointer
    using SharedThis = std::shared_ptr< ThisPointer >;
    /// @brief Get a shared skeleton pointer -- used for scenarios where the actual skeleton pointer changes (e.g., moving skeleton)
    /// @return Shared skeleton pointer
    SharedThis This() const noexcept { return this_; }

private:
    /// @brief Mutex
    mutable std::mutex mutex_;
    /// @brief Unique lock
    std::unique_lock< std::mutex > lock_;
    /// @brief Pointer wrapper
    SharedThis this_;
    /// @brief Set of binding layer skeletons
    ara::core::Vector< std::unique_ptr< BindSkeleton > > bindSkeletons_;
    /// @brief Binding layer skeleton set iterator
    typename decltype(bindSkeletons_)::const_iterator bindSkeletonsIterator_;
    /// @brief Set of instance identifiers
    InstanceIdentifierContainer instanceIdentifiers_;
    /// @brief Method call processing mode
    MethodCallProcessingMode mode_{MethodCallProcessingMode::kEvent};
    /// @brief Get the set of instance identifiers -- interface unique
    /// @return Set of instance identifiers
    static std::set< InstanceIdentifier >& InstanceIdentifierss() noexcept
    {
        static std::set< InstanceIdentifier > s_Instance;
        return s_Instance;
    }
};
}  // namespace skeleton
}  // namespace internal
}  // namespace com
}  // namespace ara