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
/// @file       proxy.h
/// @brief      Communication service proxy header file
/// @details
/// @date       2021-12-31
/// @author     mazelin
/// @version    1.2.0
///
/// ================================================================

#pragma once

#include "ara/com/internal/runtime.h"
#include "ara/com/types.h"
#include "ara/core/abort.h"

/// @brief Namespace -- Internal proxy
namespace ara {
namespace com {
namespace internal {
namespace proxy {
/// @brief Binding layer proxy type
/// @code{.isoft}
/// export_level=/COM/SoC/SOA
/// @endcode
class BindProxy
{
public:
    /// @brief Constructor
    BindProxy() noexcept;
    /// @brief Destructor
    virtual ~BindProxy() noexcept;
    /// @brief Copy constructor
    /// @param other
    BindProxy(BindProxy const& other) noexcept = default;
    /// @brief Move constructor
    /// @param other
    BindProxy(BindProxy&& other) noexcept = default;
    /// @brief Copy assignment operator
    /// @param other
    /// @return BindProxy
    BindProxy& operator=(BindProxy const& other) noexcept = default;
    /// @brief Move assignment operator
    /// @param other
    /// @return BindProxy
    BindProxy& operator=(BindProxy&& other) noexcept = default;
};

/// @brief Service proxy type
/// @code{.isoft}
/// export_level=/COM/SoC/SOA
/// @endcode
class Proxy
{
public:
    /// @brief Constructor
    Proxy() noexcept;
    /// @brief Destructor
    virtual ~Proxy() noexcept;
    /// @brief Copy constructor
    /// @param other
    Proxy(Proxy const& other) noexcept = default;
    /// @brief Move constructor
    /// @param other
    Proxy(Proxy&& other) noexcept = default;
    /// @brief Copy assignment operator
    /// @param other
    /// @return Proxy
    Proxy& operator=(Proxy const& other) noexcept = default;
    /// @brief Move assignment operator
    /// @param other
    /// @return Proxy
    Proxy& operator=(Proxy&& other) noexcept = default;
    /// @brief Get service identifier
    /// @return Service identifier
    virtual ServiceIdentifier const& GetServiceIdentifier() const noexcept = 0;
};

/// @brief Binding layer proxy handle type (used for creating proxies)
/// @code{.isoft}
/// export_level=/COM/SoC/SOA
/// @endcode
class BindHandle
{
public:
    /// @brief Constructor
    /// @param[in] instanceIdentifier Instance identifier
    explicit BindHandle(InstanceIdentifier instanceIdentifier) noexcept;
    /// @brief Destructor
    virtual ~BindHandle() noexcept;
    /// @brief Copy constructor
    /// @param other
    BindHandle(BindHandle const& other) noexcept = default;
    /// @brief Move constructor
    /// @param other
    BindHandle(BindHandle&& other) noexcept = default;
    /// @brief Copy assignment operator
    /// @param other
    /// @return BindHandle
    BindHandle& operator=(BindHandle const& other) noexcept = default;
    /// @brief Move assignment operator
    /// @param other
    /// @return BindHandle
    BindHandle& operator=(BindHandle&& other) noexcept = default;
    /// @brief Create a binding layer proxy instance
    /// @param[in] proxy Service proxy
    /// @return Proxy instance pointer
    virtual std::unique_ptr< BindProxy > CreateBindProxy(ara::com::internal::proxy::Proxy& proxy) const noexcept = 0;
    /// @brief Instance identifier
    InstanceIdentifier instanceIdentifier;
};

/// @brief Template type -- Service proxy
/// @code{.isoft}
/// export_level=/COM/SoC/SOA
/// @endcode
/// @tparam TProxy Application proxy type
/// @note All constructors should be private @ref [SWS_CM_11326]
template < typename TProxy >
class ServiceProxy : public Proxy
{
public:
    /// @brief Proxy handle type (used for creating proxies)
    /// @code{.isoft}
    /// export_level=/COM/SoC/SOA
    /// @endcode
    /// @ref [SWS_CM_00312] -- Handle Type Class
    class HandleType
    {
    public:
        /// @brief Constructor
        /// @param[in] bindHandle Binding layer proxy handle
        explicit HandleType(std::shared_ptr< BindHandle > bindHandle) noexcept : bindHandle_{std::move(bindHandle)} {}
        /// @brief Destructor
        ~HandleType() noexcept = default;
        /// @brief Copy constructor
        /// @ref [SWS_CM_00317] -- Copy semantics of handle Type Class
        HandleType(HandleType const& other) noexcept = default;
        /// @brief Copy assignment operator
        /// @return HandleType&
        HandleType& operator=(HandleType const& other) noexcept = default;
        /// @brief Move constructor
        /// @ref [SWS_CM_00318] -- Move semantics of handle Type Class
        HandleType(HandleType&& other) noexcept = default;
        /// @brief Move assignment operator
        /// @return HandleType&
        HandleType& operator=(HandleType&& other) noexcept = default;
        /// @brief Operator -- equality comparison
        /// @code{.isoft}
        /// export_level=/COM/SoC/SOA
        /// @endcode
        /// @param[in] other
        /// @return bool
        bool operator==(HandleType const& other) const noexcept { return GetInstanceId() == other.GetInstanceId(); }
        /// @brief Operator -- less than comparison
        /// @code{.isoft}
        /// export_level=/COM/SoC/SOA
        /// @endcode
        /// @param[in] other
        /// @return bool
        bool operator<(HandleType const& other) const noexcept { return GetInstanceId() < other.GetInstanceId(); }
        /// @brief Get instance identifier
        /// @code{.isoft}
        /// export_level=/COM/SoC/SOA
        /// @endcode
        /// @return Instance identifier
        InstanceIdentifier const& GetInstanceId() const noexcept
        {
            static InstanceIdentifier s_Instance{{}};
            if (bindHandle_ == nullptr) {
                return s_Instance;
            }
            return bindHandle_->instanceIdentifier;
        }
        /// @brief Create a binding layer proxy instance
        /// @param[in] proxy Service proxy
        /// @return Proxy instance pointer
        std::unique_ptr< BindProxy > CreateBindProxy(ara::com::internal::proxy::Proxy& proxy) const noexcept
        {
            if (bindHandle_ == nullptr) {
                return {};
            }
            return bindHandle_->CreateBindProxy(proxy);
        }
        /// @brief Convert to string
        /// @code{.isoft}
        /// export_level=/COM/SoC/SOA
        /// @endcode
        /// @return Proxy instance string
        auto ToString() const noexcept { return FmtStr(GenK2V0(TProxy::serviceIdentifier, GetInstanceId())); }

    private:
        /// @brief Binding layer proxy handle
        std::shared_ptr< BindHandle > bindHandle_;
    };
#if __GLIBCXX__ == 20181026 || true  // j3 not support
public:
#else
protected:
#endif
    /// @brief Constructor
    /// @code{.isoft}
    /// export_level=/COM/SoC/SOA
    /// @endcode
    /// @param[in] handle Proxy handle
    /// @ref [SWS_CM_00131] -- Creation of service proxy
    explicit ServiceProxy(HandleType const& handle) noexcept
        : this_{new ThisPointer{static_cast< TProxy* >(this)}}, handle_{handle}
    {
        ComLogTrace("create service proxy", GenArg(handle_));
    }
    /// @brief Initialization function (called by derived class)
    /// @return Result object -- empty/value or error
    ara::core::Result< void > Initialize() noexcept
    {
        using Result = ara::core::Result< void >;
        ComLogTrace("initialize service proxy", GenArg(handle_));
        if (bindProxy_ != nullptr) {
            ComLogDebug("initialize service proxy ignored: bind proxy exist", GenArg(handle_));
            return Result{};
        }
        bindProxy_ = std::move(handle_.CreateBindProxy(*this));
        if (bindProxy_ == nullptr) {
            ComLogError("initialize service proxy error: create bind proxy failed", GenArg(handle_));
            return Result{MakeErrorCode(ComErrc::kErroneousFileHandle, -__LINE__)};
        }
        ComLogTrace("initialize service proxy done", GenArg(handle_));
        return Result{};
    }

public:
    /// @brief Destructor
    ~ServiceProxy() noexcept override
    {
        ComLogTrace("destroy service proxy", GenArg(handle_));
        if (this_) {
            this_->pointer_ = nullptr;
        }
    }
    /// @brief Copy constructor
    /// @param[in] other
    /// @ref [SWS_CM_00136] -- Copy semantics of service proxy class
    ServiceProxy(ServiceProxy const& other) noexcept = delete;
    /// @brief Copy assignment operator
    /// @param[in] other
    /// @return ServiceProxy&
    ServiceProxy& operator=(ServiceProxy const& other) noexcept = delete;
    /// @brief Move constructor
    /// @param[in] other
    /// @ref [SWS_CM_00137] -- Move semantics of service proxy class
    ServiceProxy(ServiceProxy&& other) noexcept
        : this_{other.this_}, handle_{std::move(other.handle_)}, bindProxy_{std::move(other.bindProxy_)}
    {
        ComLogTrace("");
        other.this_->pointer_ = static_cast< TProxy* >(this);
        other.this_           = nullptr;
    }
    /// @brief Move assignment operator
    /// @param[in] other
    /// @return ServiceProxy&
    ServiceProxy& operator=(ServiceProxy&& other) noexcept
    {
        ComLogTrace("");
        if (this != &other) {
            this_                 = other.this_;
            handle_               = std::move(other.handle_);
            bindProxy_            = std::move(other.bindProxy_);
            other.this_->pointer_ = static_cast< TProxy* >(this);
            other.this_           = nullptr;
        }
        return *this;
    }

public:
    /// @brief Create a proxy instance
    /// @code{.isoft}
    /// export_level=/COM/SoC/SOA
    /// @endcode
    /// @param[in] handle Proxy handle
    /// @return Result object -- empty/value or error
    /// @retval ComErrc::kErroneousFileHandle Erroneous file handle
    /// @retval ComErrc::kGrantEnforcementError Request rejected by the authorization enforcement layer @ref [SWS_CM_90006]
    /// @ref [SWS_CM_11326] -- Creation of an object using Named Constructor approach
    /// @ref [SWS_CM_10438] -- Exception-less creation of service proxy
    static ara::core::Result< TProxy > Create(HandleType const& handle) noexcept
    {
        using Result = ara::core::Result< TProxy >;
        try {
            ComLogTrace("create service proxy", GenArg(handle));
            TProxy proxy{handle};
            ComLogDebug("create service proxy done", GenArg(handle));
            return Result{std::move(proxy)};
        } catch (ara::core::Exception const& e) {
            ComLogError("create service proxy exception", GenArg(handle), GenArg(e.Error()));
            return Result{e.Error()};
        }
    }
    /// @brief Get service identifier
    /// @return Service identifier
    ServiceIdentifier const& GetServiceIdentifier() const noexcept override { return TProxy::serviceIdentifier; }
    /// @brief Get the proxy handle
    /// @code{.isoft}
    /// export_level=/COM/SoC/SOA
    /// @endcode
    /// @return Proxy handle
    /// @ref [SWS_CM_10383] -- GetHandle function to return the proxy instance creation handle
    /// @ref [SWS_CM_00021] -- Re-entrancy – GetHandle
    HandleType GetHandle() const noexcept { return handle_; }
    /// @brief Find service
    /// @code{.isoft}
    /// export_level=/COM/SoC/SOA
    /// @endcode
    /// @param[in] instanceIdentifier Instance identifier -- default any service instance
    /// @return Result object -- empty/value or error
    /// @retval ComErrc::kNetworkBindingFailure Network binding detected local fault
    /// @retval ComErrc::kGrantEnforcementError Request rejected by authorization enforcement layer
    /// @retval ComErrc::kPeerIsUnreachable Transport layer security handshake failed
    /// @ref [SWS_CM_00122] -- Find service with immediately returned request using Instance ID
    /// @ref [SWS_CM_00018] -- Re-entrancy – FindService
    static ara::core::Result< ServiceHandleContainer< HandleType > > FindService(
        InstanceIdentifier const& instanceIdentifier = InstanceIdentifier::MakeAny()) noexcept
    {
        using Result = ara::core::Result< ServiceHandleContainer< HandleType > >;
        ServiceHandleContainer< HandleType > handles;
        ComLogTrace("find service", GenArg(instanceIdentifier));
        auto const& comRuntime{GetInstance()};
        auto const& bindRuntimes{comRuntime.GetBindRuntimes()};
        if (bindRuntimes.empty()) {
            ComLogError("find service rejected: bind runtime not exist", GenArg(instanceIdentifier));
            return Result{MakeErrorCode(ComErrc::kNetworkBindingFailure, -__LINE__)};
        }
        ServiceHandleContainer< std::shared_ptr< ara::com::internal::proxy::BindHandle > > bindHandles;
        for (auto const& bindRuntime : bindRuntimes) {
            bindRuntime->GetAvailableServiceHandles(TProxy::serviceIdentifier, instanceIdentifier, bindHandles);
        }
        for (auto const& bindHandle : bindHandles) {
            handles.emplace_back(bindHandle);
        }
        ComLogDebug("find service done", GenArg(instanceIdentifier), GenArg(handles));
        return Result{std::move(handles)};
    }
    /// @brief Find service
    /// @code{.isoft}
    /// export_level=/COM/SoC/SOA
    /// @endcode
    /// @param[in] instanceSpecifier Instance specifier
    /// @return Result object -- empty/value or error
    /// @retval ComErrc::kNetworkBindingFailure Network binding detected local fault
    /// @retval ComErrc::kGrantEnforcementError Request rejected by authorization enforcement layer
    /// @retval ComErrc::kPeerIsUnreachable Transport layer security handshake failed
    /// @ref [SWS_CM_00622] -- Find service with immediately returned request using Instance Specifier
    /// @ref [SWS_CM_00018] -- Re-entrancy – FindService
    static ara::core::Result< ServiceHandleContainer< HandleType > > FindService(
        ara::core::InstanceSpecifier const& instanceSpecifier) noexcept
    {
        using Result = ara::core::Result< ServiceHandleContainer< HandleType > >;
        ServiceHandleContainer< HandleType > handles;
        ComLogTrace("find service", GenArg(instanceSpecifier));
        auto instanceIdentifiersRes{runtime::ResolveInstanceIDs(instanceSpecifier)};
        if (!instanceIdentifiersRes) {
            ComLogError("find service rejected: resolve instance IDs failed", GenArg(instanceSpecifier),
                        GenArg(instanceIdentifiersRes));
            return Result{instanceIdentifiersRes.Error()};
        }
        auto instanceIdentifiers{instanceIdentifiersRes.Value()};
        if (instanceIdentifiers.empty()) {
            ComLogError("find service rejected: instance IDs empty", GenArg(instanceSpecifier));
            return Result{MakeErrorCode(ComErrc::kNetworkBindingFailure, -__LINE__)};
        }
        for (auto const& instanceIdentifier : instanceIdentifiers) {
            auto handlesRes{FindService(instanceIdentifier)};
            if (!handlesRes) {
                ComLogError("find service rejected: find service failed", GenArg(instanceSpecifier),
                            GenArg(instanceIdentifier), GenArg(handlesRes));
                return Result{handlesRes.Error()};
            }
            auto handles1{std::move(handlesRes).Value()};
            handles.insert(handles.end(), std::make_move_iterator(handles1.begin()),
                           std::make_move_iterator(handles1.end()));
        }
        ComLogDebug("find service done", GenArg(instanceSpecifier), GenArg(handles));
        return Result{std::move(handles)};
    }
    /// @brief Start finding service
    /// @code{.isoft}
    /// export_level=/COM/SoC/SOA
    /// @endcode
    /// @param[in] findHandler Find service handler
    /// @param[in] instanceIdentifier Instance identifier -- default any service instance
    /// @return Result object -- empty/value or error
    /// @retval ComErrc::kNetworkBindingFailure Network binding detected local fault
    /// @retval ComErrc::kGrantEnforcementError Request rejected by authorization enforcement layer
    /// @retval ComErrc::kPeerIsUnreachable Transport layer security handshake failed
    /// @ref [SWS_CM_00123] -- Find service with handler registration using Instance ID
    /// @ref [SWS_CM_00019] -- Re-entrancy – StartFindService
    static ara::core::Result< FindServiceHandle > StartFindService(FindServiceHandler< HandleType > findHandler,
                                                                   InstanceIdentifier const& instanceIdentifier
                                                                   = InstanceIdentifier::MakeAny()) noexcept
    {
        return StartFindService(findHandler, instanceIdentifier, [](std::function< void() > const& f) { f(); });
    }
    /// @brief Start finding service
    /// @code{.isoft}
    /// export_level=/COM/SoC/SOA
    /// @endcode
    /// @param[in] findHandler Find service handler
    /// @param[in] instanceIdentifier Instance identifier
    /// @param[in] executor Executor -- the processing will take place in its context
    /// @return Result object -- empty/value or error
    /// @retval ComErrc::kNetworkBindingFailure Network binding detected local fault
    /// @retval ComErrc::kGrantEnforcementError Request rejected by authorization enforcement layer
    /// @retval ComErrc::kPeerIsUnreachable Transport layer security handshake failed
    /// @retval ComErrc::kCouldNotExecute Cannot execute (e.g., due to resource issues)
    /// @ref [SWS_CM_11352] -- Execution Context for finding service with handler registration using Instance ID
    /// @ref [SWS_CM_11353] -- Error behavior of provided Execution Context for finding service with handler
    /// registration using Instance ID
    /// @ref [SWS_CM_00019] -- Re-entrancy – StartFindService
    template < typename Executor >
    static ara::core::Result< FindServiceHandle > StartFindService(FindServiceHandler< HandleType > findHandler,
                                                                   InstanceIdentifier const& instanceIdentifier,
                                                                   Executor&& executor) noexcept
    {
        using Result = ara::core::Result< FindServiceHandle >;
        ComLogTrace("start find service", GenArg(instanceIdentifier));
        if (!executor) {
            ComLogError("start find service rejected: executor invalid");
            return Result{MakeErrorCode(ComErrc::kCouldNotExecute, -__LINE__)};
        }
        auto const& comRuntime{GetInstance()};
        auto const& bindRuntimes{comRuntime.GetBindRuntimes()};
        if (bindRuntimes.empty()) {
            ComLogError("start find service rejected: bind runtime not exist", GenArg(instanceIdentifier));
            return Result{MakeErrorCode(ComErrc::kNetworkBindingFailure, -__LINE__)};
        }
        FindServiceHandle findHandle{TProxy::serviceIdentifier, instanceIdentifier, FindServiceHandle::GenerateUID()};
        ComLogTrace("start find service: create find handle", GenArg(findHandle));
        auto findHandlerCast{
            [findHandler{std::move(findHandler)}, executor{std::move(executor)}](auto handlesCast, auto foundHandle) {
                executor([findHandler{std::move(findHandler)}, handlesCast{std::move(handlesCast)},
                          foundHandle{std::move(foundHandle)}]() {
                    ServiceHandleContainer< HandleType > handles;
                    for (auto const& handle : handlesCast) {
                        handles.emplace_back(std::move(handle));
                    }
                    ComLogDebug("handle find service: invoke callback handle service instance list changed",
                                GenArg(handles), GenArg(foundHandle));
                    findHandler(handles, foundHandle);
                    ComLogTrace("handle find service: invoke callback handle service instance list changed done",
                                GenArg(handles), GenArg(foundHandle));
                });
            }};
        for (auto const& bindRuntime : bindRuntimes) {
            bindRuntime->RegisterFindServiceHandle(findHandle, findHandlerCast);
        }
        ComLogDebug("start find service done", GenArg(findHandle));
        return Result{std::move(findHandle)};
    }
    /// @brief Start finding service
    /// @code{.isoft}
    /// export_level=/COM/SoC/SOA
    /// @endcode
    /// @param[in] findHandler Find service handler
    /// @param[in] instanceSpecifier Instance specifier
    /// @return Result object -- empty/value or error
    /// @retval ComErrc::kNetworkBindingFailure Network binding detected local fault
    /// @retval ComErrc::kGrantEnforcementError Request rejected by authorization enforcement layer
    /// @retval ComErrc::kPeerIsUnreachable Transport layer security handshake failed
    /// @ref [SWS_CM_00623] -- Find service with handler registration using Instance Specifier
    /// @ref [SWS_CM_00019] -- Re-entrancy – StartFindService
    static ara::core::Result< FindServiceHandle > StartFindService(
        FindServiceHandler< HandleType > findHandler, ara::core::InstanceSpecifier const& instanceSpecifier) noexcept
    {
        using Result = ara::core::Result< FindServiceHandle >;
        ComLogTrace("start find service", GenArg(instanceSpecifier));
        auto instanceIdentifiersRes{runtime::ResolveInstanceIDs(instanceSpecifier)};
        if (!instanceIdentifiersRes) {
            ComLogError("start find service rejected: resolve instance IDs failed", GenArg(instanceSpecifier),
                        GenArg(instanceIdentifiersRes));
            return Result{instanceIdentifiersRes.Error()};
        }
        auto instanceIdentifiers{instanceIdentifiersRes.Value()};
        if (instanceIdentifiers.empty()) {
            ComLogError("start find service rejected: instance IDs empty", GenArg(instanceSpecifier));
            return Result{MakeErrorCode(ComErrc::kNetworkBindingFailure, -__LINE__)};
        }
        return StartFindService(findHandler, instanceIdentifiers[0]);
    }
    /// @brief Stop finding service
    /// @code{.isoft}
    /// export_level=/COM/SoC/SOA
    /// @endcode
    /// @param[in] findHandle Find service handle
    /// @ref [SWS_CM_00125] -- Stop find service
    /// @ref [SWS_CM_00020] -- Re-entrancy – StopFindService
    static void StopFindService(FindServiceHandle findHandle) noexcept
    {
        ComLogTrace("stop find service", GenArg(findHandle));
        auto const& comRuntime{GetInstance()};
        auto const& bindRuntimes{comRuntime.GetBindRuntimes()};
        if (bindRuntimes.empty()) {
            ComLogWarning("stop find service ignored: bind runtime not exist", GenArg(findHandle));
            return;
        }
        for (auto const& bindRuntime : bindRuntimes) {
            bindRuntime->UnregisterFindServiceHandle(findHandle);
        }
        ComLogDebug("stop find service done", GenArg(findHandle));
    }

public:
    /// @brief Pointer wrapper type
    class ThisPointer
    {
    private:
        /// @brief Proxy pointer
        TProxy* pointer_;
        friend class ServiceProxy;
        /// @brief Constructor
        /// @param[in] pointer
        explicit ThisPointer(TProxy* pointer) noexcept : pointer_{pointer}
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
        /// @brief Move constructor
        /// @param[in] other
        ThisPointer(ThisPointer&& other) noexcept = delete;
        /// @brief Copy assignment operator
        /// @param[in] other
        /// @return ThisPointer&
        ThisPointer& operator=(ThisPointer const& other) noexcept = delete;
        /// @brief Move assignment operator
        /// @param[in] other
        /// @return ThisPointer&
        ThisPointer& operator=(ThisPointer&& other) noexcept = delete;
        /// @brief Operator -- returns reference
        /// @return Proxy reference
        TProxy& operator*() const noexcept { return *pointer_; }
        /// @brief Operator -- returns pointer
        /// @return Proxy pointer
        TProxy* operator->() const noexcept { return pointer_; }
        /// @brief Operator -- implicit conversion
        /// @return
        explicit operator bool() const noexcept { return pointer_; }
    };
    /// @brief Type alias -- Shared proxy pointer
    using SharedThis = std::shared_ptr< ThisPointer >;
    /// @brief Get a shared proxy pointer -- used for scenarios where the actual proxy pointer changes (e.g., moving proxy)
    /// @return Shared proxy pointer
    SharedThis This() const noexcept { return this_; }

private:
    /// @brief Pointer wrapper
    SharedThis this_;
    /// @brief Proxy handle
    HandleType handle_;
    /// @brief Binding layer proxy
    std::unique_ptr< BindProxy > bindProxy_;
};
}  // namespace proxy
}  // namespace internal
}  // namespace com
}  // namespace ara