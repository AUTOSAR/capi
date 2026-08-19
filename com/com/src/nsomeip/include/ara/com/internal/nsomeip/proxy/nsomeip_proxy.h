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
/// @file       nsomeip_proxy.h
/// @brief      Binding layer service proxy header file
/// @details
/// @date       2021-12-31
/// @author     mazelin
/// @version    1.2.0
///
/// ================================================================

#ifndef __COM_NSOMEIP_PROXY_H
#define __COM_NSOMEIP_PROXY_H

#include "ara/com/internal/proxy/proxy.h"
#include "nsomeip_event.h"
#include "nsomeip_field.h"
#include "nsomeip_method.h"

/// @brief Namespace -- internal binding layer proxy
namespace ara {
namespace com {
namespace internal {
namespace nsomeip {
namespace proxy {
/// @brief Template type -- binding layer service proxy
/// @code{.isoft}
/// export_level=/COM/SoC/SOA
/// @endcode
/// @tparam ServiceDesc Service description information
/// @tparam ServiceProxy Service proxy type
template < typename ServiceDesc, typename ServiceProxy >
class NSomeipProxy : public ara::com::internal::proxy::BindProxy
{
public:
    /// @brief Type alias -- service proxy
    using Proxy = ServiceProxy;
    /// @brief Create binding layer proxy instance
    /// @param[in] proxy Service proxy
    /// @param[in] instanceIdentifier Instance identifier
    /// @return Proxy instance pointer
    static std::unique_ptr< NSomeipProxy > Create(Proxy& proxy,
                                                  ara::com::InstanceIdentifier const& instanceIdentifier) noexcept
    {
        std::ignore = proxy;
        auto optionalId{runtime::ToNSomeipInstanceId(instanceIdentifier)};
        NSOMEIP_ASSERT(!!optionalId, -__LINE__);
        auto instanceId{*optionalId};
        NSOMEIP_ASSERT(instanceId != Message::kInstanceIdAny, -__LINE__);
        MonitorTrace(runtime::GetInstance(), trace::someip::kServiceFind, trace::ServiceType::kProxy,
                     ServiceDesc::serviceId(), instanceId, ServiceDesc::serviceIdentifier,
                     static_cast< uint8_t >(trace::InstanceState::kStateStarting), 0);
        auto ptr{std::make_unique< NSomeipProxy >(instanceId)};
        MonitorTrace(
            runtime::GetInstance(), trace::someip::kServiceFind, trace::ServiceType::kProxy, ServiceDesc::serviceId(),
            instanceId, ServiceDesc::serviceIdentifier,
            static_cast< uint8_t >(ptr ? trace::InstanceState::kStateRunning : trace::InstanceState::kStateError), 0);
        return ptr;
    }
    /// @brief Create binding layer event instance
    /// @tparam Desc Description information
    /// @tparam T Event data type
    /// @return Event instance pointer
    template < typename Desc, typename T >
    auto Create(std::enable_if_t< ara::com::internal::proxy::IsEvent< T >::value >* = nullptr) noexcept
    {
        return std::make_shared< NSomeipEvent< NSomeipProxy, Desc, typename T::SampleType > >(*this);
    }
    /// @brief Create binding layer method instance -- one-way method
    /// @tparam Desc Description information
    /// @tparam T Method signature type
    /// @return Method instance pointer
    template < typename Desc, typename T >
    auto Create(std::enable_if_t< ara::com::internal::proxy::IsFMethod< T >::value >* = nullptr) noexcept
    {
        return std::make_shared< NSomeipFMethod< NSomeipProxy, Desc, typename T::Signature > >(*this);
    }
    /// @brief Create binding layer method instance -- two-way void return method
    /// @tparam Desc Description information
    /// @tparam T Method signature type
    /// @return Method instance pointer
    template < typename Desc, typename T >
    auto Create(std::enable_if_t< ara::com::internal::proxy::IsVMethod< T >::value >* = nullptr) noexcept
    {
        return std::make_shared< NSomeipVMethod< NSomeipProxy, Desc, typename T::Signature > >(*this);
    }
    /// @brief Create binding layer method instance -- two-way method
    /// @tparam Desc Description information
    /// @tparam T Method signature type
    /// @return Method instance pointer
    template < typename Desc, typename T >
    auto Create(std::enable_if_t< ara::com::internal::proxy::IsRMethod< T >::value
                                  && !ara::com::internal::proxy::IsVMethod< T >::value >* = nullptr) noexcept
    {
        return std::make_shared< NSomeipRMethod< NSomeipProxy, Desc, typename T::Signature > >(*this);
    }
    /// @brief Create binding layer field instance -- notifiable
    /// @tparam Desc Description information
    /// @tparam T Field data type
    /// @return Field instance pointer
    template < typename Desc, typename T >
    auto Create(std::enable_if_t< ara::com::internal::proxy::IsNField< T >::value >* = nullptr) noexcept
    {
        return std::make_shared< NSomeipNField< NSomeipProxy, Desc, typename T::FieldType > >(*this);
    }
    /// @brief Create binding layer field instance -- settable
    /// @tparam Desc Description information
    /// @tparam T Field data type
    /// @return Field instance pointer
    template < typename Desc, typename T >
    auto Create(std::enable_if_t< ara::com::internal::proxy::IsSField< T >::value >* = nullptr) noexcept
    {
        return std::make_shared< NSomeipSField< NSomeipProxy, Desc, typename T::FieldType > >(*this);
    }
    /// @brief Create binding layer field instance -- gettable
    /// @tparam Desc Description information
    /// @tparam T Field data type
    /// @return Field instance pointer
    template < typename Desc, typename T >
    auto Create(std::enable_if_t< ara::com::internal::proxy::IsGField< T >::value >* = nullptr) noexcept
    {
        return std::make_shared< NSomeipGField< NSomeipProxy, Desc, typename T::FieldType > >(*this);
    }
    /// @brief Create binding layer field instance -- notifiable/settable
    /// @tparam Desc Description information
    /// @tparam T Field data type
    /// @return Field instance pointer
    template < typename Desc, typename T >
    auto Create(std::enable_if_t< ara::com::internal::proxy::IsNSField< T >::value >* = nullptr) noexcept
    {
        return std::make_shared< NSomeipNSField< NSomeipProxy, Desc, typename T::FieldType > >(*this);
    }
    /// @brief Create binding layer field instance -- notifiable/gettable
    /// @tparam Desc Description information
    /// @tparam T Field data type
    /// @return Field instance pointer
    template < typename Desc, typename T >
    auto Create(std::enable_if_t< ara::com::internal::proxy::IsNGField< T >::value >* = nullptr) noexcept
    {
        return std::make_shared< NSomeipNGField< NSomeipProxy, Desc, typename T::FieldType > >(*this);
    }
    /// @brief Create binding layer field instance -- settable/gettable
    /// @tparam Desc Description information
    /// @tparam T Field data type
    /// @return Field instance pointer
    template < typename Desc, typename T >
    auto Create(std::enable_if_t< ara::com::internal::proxy::IsSGField< T >::value >* = nullptr) noexcept
    {
        return std::make_shared< NSomeipSGField< NSomeipProxy, Desc, typename T::FieldType > >(*this);
    }
    /// @brief Create binding layer field instance -- notifiable/settable/gettable
    /// @tparam Desc Description information
    /// @tparam T Field data type
    /// @return Field instance pointer
    template < typename Desc, typename T >
    auto Create(std::enable_if_t< ara::com::internal::proxy::IsNSGField< T >::value >* = nullptr) noexcept
    {
        return std::make_shared< NSomeipNSGField< NSomeipProxy, Desc, typename T::FieldType > >(*this);
    }
    /// @brief Constructor
    /// @param[in] instanceId Instance identifier
    explicit NSomeipProxy(ServiceInfo::InstanceId instanceId) noexcept
        : instanceId_{instanceId}
        , requestedHolder_{{ServiceDesc::serviceId(), instanceId_, ServiceDesc::serviceVersionMajor(),
                            ServiceDesc::serviceVersionMinor()}}
    {
        ComLogTrace("create bind proxy", GenArg(instanceId_));
    }
    /// @brief Destructor
    ~NSomeipProxy() noexcept override
    {
        ComLogTrace("destroy bind proxy", GenArg(instanceId_));
        MonitorTrace(runtime::GetInstance(), trace::someip::kServiceFind, trace::ServiceType::kProxy,
                     ServiceDesc::serviceId(), instanceId_, ServiceDesc::serviceIdentifier,
                     static_cast< uint8_t >(trace::InstanceState::kStateStop), 0);
    }
    /// @brief Copy constructor
    /// @param other
    NSomeipProxy(NSomeipProxy const& other) noexcept = default;
    /// @brief Move constructor
    /// @param other
    NSomeipProxy(NSomeipProxy&& other) noexcept = default;
    /// @brief Copy assignment operator
    /// @param other
    /// @return NSomeipProxy
    NSomeipProxy& operator=(NSomeipProxy const& other) noexcept = default;
    /// @brief Move assignment operator
    /// @param other
    /// @return NSomeipProxy
    NSomeipProxy& operator=(NSomeipProxy&& other) noexcept = default;

private:
    template < typename Owner, typename Desc, typename T >
    friend class NSomeipEvent;
    template < typename Owner, typename Desc, typename T, ara::com::internal::proxy::MethodMode mode >
    friend class NSomeipMethod;
    /// @brief Instance identifier
    ServiceInfo::InstanceId instanceId_;
    /// @brief Service request holder
    NSomeipRuntime::ServiceRequestedHolder requestedHolder_;
};
/// @brief Create binding layer proxy instance
/// @tparam ServiceDesc Service description information
/// @tparam ServiceProxy Service proxy type
/// @param[in] proxy Service proxy
/// @param[in] instanceIdentifier Instance identifier
/// @return Proxy instance pointer
template < typename ServiceDesc, typename ServiceProxy >
auto Create(ServiceProxy& proxy, ara::com::InstanceIdentifier const& instanceIdentifier) noexcept
{
    return NSomeipProxy< ServiceDesc, ServiceProxy >::Create(proxy, instanceIdentifier);
}
/// @brief Proxy mapper implementation type
/// @tparam ProxyImpl Proxy implementation type
template < typename ProxyImpl >
class NSomeipProxyMapper : public NSomeipRuntime::ProxyMapperBase
{
public:
    /// @brief Get service identifier
    /// @return Service identifier
    ServiceIdentifier const& GetServiceIdentifier() const noexcept override { return ProxyImpl::serviceIdentifier; }
    /// @brief Get service identifier
    /// @return Service identifier
    ServiceInfo::ServiceId GetServiceId() const noexcept override { return ProxyImpl::serviceId(); }
    /// @brief Create binding layer proxy handle
    /// @param[in] instanceIdentifier Instance identifier
    /// @return Proxy handle pointer
    std::shared_ptr< ara::com::internal::proxy::BindHandle > CreateBindHandle(
        InstanceIdentifier const& instanceIdentifier) const noexcept override
    {
        /// @brief Proxy handle implementation type
        class Handle : public ara::com::internal::proxy::BindHandle
        {
        public:
            /// @brief Reference base class constructor
            using ara::com::internal::proxy::BindHandle::BindHandle;
            /// @brief Create binding layer proxy instance
            /// @param[in] proxy Service proxy
            /// @return Proxy instance pointer
            std::unique_ptr< ara::com::internal::proxy::BindProxy > CreateBindProxy(
                ara::com::internal::proxy::Proxy& proxy) const noexcept override
            {
                auto const& serviceIdentifier{proxy.GetServiceIdentifier()};
                auto const serviceId{ProxyImpl::serviceId()};
                ComLogDebug("create bind proxy", GenArg(serviceIdentifier), GenArg(serviceId),
                            GenArg(instanceIdentifier));
                return ProxyImpl::CreateBindProxy(static_cast< typename ProxyImpl::Proxy& >(proxy), instanceIdentifier);
            }
        };
        return std::make_shared< Handle >(instanceIdentifier);
    }
};
}  // namespace proxy
}  // namespace nsomeip
}  // namespace internal
}  // namespace com
}  // namespace ara
#endif
