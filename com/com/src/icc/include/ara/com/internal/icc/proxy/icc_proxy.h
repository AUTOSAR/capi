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
/// @file       icc_proxy.h
/// @brief      Binding layer service proxy header file
/// @details
/// @date       2021-12-31
/// @author     mazelin
/// @version    1.2.0
///
/// ================================================================

#ifndef __COM_ICC_PROXY_H
#define __COM_ICC_PROXY_H

#include "ara/com/internal/proxy/proxy.h"
#include "icc_event.h"
#include "icc_field.h"
#include "icc_method.h"

/// @brief Namespace -- internal binding layer proxy
namespace ara {
namespace com {
namespace internal {
namespace icc {
namespace proxy {
/// @brief Template type -- binding layer service proxy
/// @code{.isoft}
/// export_level=/COM/SoC/SOA
/// @endcode
/// @tparam ServiceDesc Service description information
/// @tparam ServiceProxy Service proxy type
template < typename ServiceDesc, typename ServiceProxy >
class IccProxy : public ara::com::internal::proxy::BindProxy
{
public:
    /// @brief Type alias -- service proxy
    using Proxy = ServiceProxy;
    /// @brief Create binding layer proxy instance
    /// @param[in] proxy Service proxy
    /// @param[in] instanceIdentifier Instance identifier
    /// @return Proxy instance pointer
    static std::unique_ptr< IccProxy > Create(Proxy& proxy,
                                              ara::com::InstanceIdentifier const& instanceIdentifier) noexcept
    {
        std::ignore = proxy;
        auto optionalId{runtime::ToIccInstanceId(instanceIdentifier)};
        ICC_ASSERT(!!optionalId, -__LINE__);
        auto instanceId{*optionalId};
        ICC_ASSERT(instanceId != Message::kInstanceIdAny, -__LINE__);
        return std::make_unique< IccProxy >(instanceId);
    }
    /// @brief Create binding layer event instance
    /// @tparam Desc Description information
    /// @tparam T Event data type
    /// @return Event instance pointer
    template < typename Desc, typename T >
    auto Create(std::enable_if_t< ara::com::internal::proxy::IsEvent< T >::value >* = nullptr) noexcept
    {
        return std::make_shared< IccEvent< IccProxy, Desc, typename T::SampleType > >(*this);
    }
    /// @brief Create binding layer method instance -- one-way method
    /// @tparam Desc Description information
    /// @tparam T Method signature
    /// @return Method instance pointer
    template < typename Desc, typename T >
    auto Create(std::enable_if_t< ara::com::internal::proxy::IsFMethod< T >::value >* = nullptr) noexcept
    {
        return std::make_shared< IccFMethod< IccProxy, Desc, typename T::Signature > >(*this);
    }
    /// @brief Create binding layer method instance -- two-way void return method
    /// @tparam Desc Description information
    /// @tparam T Method signature
    /// @return Method instance pointer
    template < typename Desc, typename T >
    auto Create(std::enable_if_t< ara::com::internal::proxy::IsVMethod< T >::value >* = nullptr) noexcept
    {
        return std::make_shared< IccVMethod< IccProxy, Desc, typename T::Signature > >(*this);
    }
    /// @brief Create binding layer method instance -- two-way method
    /// @tparam Desc Description information
    /// @tparam T Method signature
    /// @return Method instance pointer
    template < typename Desc, typename T >
    auto Create(std::enable_if_t< ara::com::internal::proxy::IsRMethod< T >::value
                                  && !ara::com::internal::proxy::IsVMethod< T >::value >* = nullptr) noexcept
    {
        return std::make_shared< IccRMethod< IccProxy, Desc, typename T::Signature > >(*this);
    }
    /// @brief Create binding layer field instance -- notifyable
    /// @tparam Desc Description information
    /// @tparam T Field data type
    /// @return Field instance pointer
    template < typename Desc, typename T >
    auto Create(std::enable_if_t< ara::com::internal::proxy::IsNField< T >::value >* = nullptr) noexcept
    {
        return std::make_shared< IccNField< IccProxy, Desc, typename T::FieldType > >(*this);
    }
    /// @brief Create binding layer field instance -- settable
    /// @tparam Desc Description information
    /// @tparam T Field data type
    /// @return Field instance pointer
    template < typename Desc, typename T >
    auto Create(std::enable_if_t< ara::com::internal::proxy::IsSField< T >::value >* = nullptr) noexcept
    {
        return std::make_shared< IccSField< IccProxy, Desc, typename T::FieldType > >(*this);
    }
    /// @brief Create binding layer field instance -- gettable
    /// @tparam Desc Description information
    /// @tparam T Field data type
    /// @return Field instance pointer
    template < typename Desc, typename T >
    auto Create(std::enable_if_t< ara::com::internal::proxy::IsGField< T >::value >* = nullptr) noexcept
    {
        return std::make_shared< IccGField< IccProxy, Desc, typename T::FieldType > >(*this);
    }
    /// @brief Create binding layer field instance -- notifyable/settable
    /// @tparam Desc Description information
    /// @tparam T Field data type
    /// @return Field instance pointer
    template < typename Desc, typename T >
    auto Create(std::enable_if_t< ara::com::internal::proxy::IsNSField< T >::value >* = nullptr) noexcept
    {
        return std::make_shared< IccNSField< IccProxy, Desc, typename T::FieldType > >(*this);
    }
    /// @brief Create binding layer field instance -- notifyable/gettable
    /// @tparam Desc Description information
    /// @tparam T Field data type
    /// @return Field instance pointer
    template < typename Desc, typename T >
    auto Create(std::enable_if_t< ara::com::internal::proxy::IsNGField< T >::value >* = nullptr) noexcept
    {
        return std::make_shared< IccNGField< IccProxy, Desc, typename T::FieldType > >(*this);
    }
    /// @brief Create binding layer field instance -- settable/gettable
    /// @tparam Desc Description information
    /// @tparam T Field data type
    /// @return Field instance pointer
    template < typename Desc, typename T >
    auto Create(std::enable_if_t< ara::com::internal::proxy::IsSGField< T >::value >* = nullptr) noexcept
    {
        return std::make_shared< IccSGField< IccProxy, Desc, typename T::FieldType > >(*this);
    }
    /// @brief Create binding layer field instance -- notifyable/settable/gettable
    /// @tparam Desc Description information
    /// @tparam T Field data type
    /// @return Field instance pointer
    template < typename Desc, typename T >
    auto Create(std::enable_if_t< ara::com::internal::proxy::IsNSGField< T >::value >* = nullptr) noexcept
    {
        return std::make_shared< IccNSGField< IccProxy, Desc, typename T::FieldType > >(*this);
    }
    /// @brief Constructor
    /// @param[in] instanceId Instance identifier
    explicit IccProxy(ServiceInfo::InstanceId instanceId) noexcept
        : instanceId_{instanceId}
        , requestedHolder_{{ServiceDesc::serviceId(), instanceId_, ServiceDesc::serviceVersionMajor(),
                            ServiceDesc::serviceVersionMinor()}}
    {
        ComLogTrace("create bind proxy", GenArg(instanceId_));
    }
    /// @brief Destructor
    ~IccProxy() noexcept override { ComLogTrace("destroy bind proxy", GenArg(instanceId_)); }
    /// @brief Copy constructor
    /// @param other
    IccProxy(IccProxy const& other) noexcept = default;
    /// @brief Move constructor
    /// @param other
    IccProxy(IccProxy&& other) noexcept = default;
    /// @brief Copy assignment operator
    /// @param other
    /// @return IccProxy
    IccProxy& operator=(IccProxy const& other) noexcept = default;
    /// @brief Move assignment operator
    /// @param other
    /// @return IccProxy
    IccProxy& operator=(IccProxy&& other) noexcept = default;

private:
    template < typename Owner, typename Desc, typename T >
    friend class IccEvent;
    template < typename Owner, typename Desc, typename T, ara::com::internal::proxy::MethodMode mode >
    friend class IccMethod;
    /// @brief Instance identifier
    ServiceInfo::InstanceId instanceId_;
    /// @brief Service request holder
    IccRuntime::ServiceRequestedHolder requestedHolder_;
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
    return IccProxy< ServiceDesc, ServiceProxy >::Create(proxy, instanceIdentifier);
}
/// @brief Proxy mapper implementation type
/// @tparam ProxyImpl Proxy implementation type
template < typename ProxyImpl >
class IccProxyMapper : public IccRuntime::ProxyMapperBase
{
public:
    /// @brief Get service identifier
    /// @return Service identifier
    ServiceIdentifier const& GetServiceIdentifier() const noexcept override { return ProxyImpl::serviceIdentifier; }
    /// @brief Get service ID
    /// @return Service ID
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
}  // namespace icc
}  // namespace internal
}  // namespace com
}  // namespace ara
#endif
