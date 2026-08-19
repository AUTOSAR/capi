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
/// @file       nsomeip_skeleton.h
/// @brief      Binding layer service skeleton header file
/// @details
/// @date       2021-12-31
/// @author     mazelin
/// @version    1.2.0
///
/// ================================================================

#ifndef __COM_NSOMEIP_SKELETON_H
#define __COM_NSOMEIP_SKELETON_H

#include <deque>

#include "ara/com/internal/skeleton/skeleton.h"
#include "nsomeip_event.h"
#include "nsomeip_field.h"
#include "nsomeip_method.h"
#ifdef HAS_COM_SECOC
    #include "ara/com/internal/secoc/secoc_someip.h"
#else
#endif

/// @brief Namespace -- internal binding layer skeleton
namespace ara {
namespace com {
namespace internal {
namespace nsomeip {
namespace skeleton {
/// @brief Template type -- binding layer service skeleton
/// @code{.isoft}
/// export_level=/COM/SoC/SOA
/// @endcode
/// @tparam ServiceDesc Service description information
/// @tparam ServiceSkeleton Service skeleton type
template < typename ServiceDesc, typename ServiceSkeleton >
class NSomeipSkeleton : public ara::com::internal::skeleton::BindSkeleton
{
public:
    /// @brief Type alias -- service skeleton
    using Skeleton = ServiceSkeleton;
    /// @brief Create binding layer skeleton instance
    /// @param[in] skeleton Service skeleton
    /// @param[in] instanceIdentifier Instance identifier
    /// @return Skeleton instance pointer
    static std::unique_ptr< NSomeipSkeleton > Create(Skeleton& skeleton,
                                                     ara::com::InstanceIdentifier const& instanceIdentifier) noexcept
    {
        auto optionalId{runtime::ToNSomeipInstanceId(instanceIdentifier)};
        NSOMEIP_ASSERT(!!optionalId, -__LINE__);
        auto& bindRuntime{runtime::GetInstance()};
        auto serviceId{ServiceDesc::serviceId()};
        auto instanceId{*optionalId};
        auto* serviceInfo{bindRuntime.GetPServiceInfo(serviceId, instanceId)};
        NSOMEIP_ASSERT(serviceInfo != nullptr && serviceInfo->IsProvided(), -__LINE__);
        return std::make_unique< NSomeipSkeleton >(static_cast< PServiceInfo& >(*serviceInfo), bindRuntime, skeleton);
    }
    /// @brief Create binding layer event instance
    /// @tparam Desc Description information
    /// @tparam T Event data type
    /// @return Event instance pointer
    template < typename Desc, typename T >
    auto Create(std::enable_if_t< ara::com::internal::skeleton::IsEvent< T >::value >* = nullptr) noexcept
    {
        auto event{std::make_shared< NSomeipEvent< NSomeipSkeleton, Desc, typename T::SampleType > >(*this)};
        afterOffer_.emplace_back([event]() { return event->OfferNotification(); });
        beforeStopOffer_.emplace_back([event]() { return event->StopOfferNotification(); });
        return event;
    }
    /// @brief Create binding layer method instance
    /// @tparam Desc Description information
    /// @tparam R Method return type
    /// @tparam Args Method parameter list type
    /// @param[in] f Member function pointer
    template < typename Desc, typename R, typename... Args >
    void Create(R (Skeleton::*f)(Args...)) noexcept
    {
        using Callback = std::function< R(Args...) >;
        auto method{std::make_shared< NSomeipMethod< NSomeipSkeleton, Desc, Callback > >(
            *this, std::function< Callback() >{
                       [this, f]() { return ara::com::internal::skeleton::ToFunction(f, **skeleton_); }})};
        beforeOffer_.emplace_back([method]() { return method->RegisterReceiver(); });
        afterStopOffer_.emplace_back([method]() { return method->UnregisterReceiver(); });
    }
    /// @brief Create binding layer field instance -- notifiable
    /// @tparam Desc Description information
    /// @tparam T Field data type
    /// @return Field instance pointer
    template < typename Desc, typename T >
    auto Create(std::enable_if_t< ara::com::internal::skeleton::IsNField< T >::value >* = nullptr) noexcept
    {
        auto field{std::make_shared< NSomeipNField< NSomeipSkeleton, Desc, typename T::FieldType > >(*this)};
        // [TODO] Provide notification immediately after service creation (solve error reporting issue when updating field before service provision)
        afterCreate_.emplace_back([field]() { return field->OfferNotification(); });
        beforeDestroy_.emplace_back([field]() { return field->StopOfferNotification(); });
        using Result = ara::core::Result< void >;
        Result result{};
        for (auto& after : afterCreate_) {
            auto res{after()};
            if (res) {
                continue;
            }
            result = std::move(res);
            ComLogError("create bind skeleton field(n) error: invoke after create failed", GenArg(pServiceInfo_),
                        GenArg(result));
            break;
        }
        afterCreate_.clear();
        return field;
    }
    /// @brief Create binding layer field instance -- settable
    /// @tparam Desc Description information
    /// @tparam T Field data type
    /// @return Field instance pointer
    template < typename Desc, typename T >
    auto Create(std::enable_if_t< ara::com::internal::skeleton::IsSField< T >::value >* = nullptr) noexcept
    {
        auto field{std::make_shared< NSomeipSField< NSomeipSkeleton, Desc, typename T::FieldType > >(*this)};
        beforeOffer_.emplace_back([field]() { return field->RegisterSetReceiver(); });
        afterStopOffer_.emplace_back([field]() { return field->UnregisterSetReceiver(); });
        return field;
    }
    /// @brief Create binding layer field instance -- gettable
    /// @tparam Desc Description information
    /// @tparam T Field data type
    /// @return Field instance pointer
    template < typename Desc, typename T >
    auto Create(std::enable_if_t< ara::com::internal::skeleton::IsGField< T >::value >* = nullptr) noexcept
    {
        auto field{std::make_shared< NSomeipGField< NSomeipSkeleton, Desc, typename T::FieldType > >(*this)};
        beforeOffer_.emplace_back([field]() { return field->RegisterGetReceiver(); });
        afterStopOffer_.emplace_back([field]() { return field->UnregisterGetReceiver(); });
        return field;
    }
    /// @brief Create binding layer field instance -- notifiable/settable
    /// @tparam Desc Description information
    /// @tparam T Field data type
    /// @return Field instance pointer
    template < typename Desc, typename T >
    auto Create(std::enable_if_t< ara::com::internal::skeleton::IsNSField< T >::value >* = nullptr) noexcept
    {
        auto field{std::make_shared< NSomeipNSField< NSomeipSkeleton, Desc, typename T::FieldType > >(*this)};
        // [TODO] Provide notification immediately after service creation (solve error reporting issue when updating field before service provision)
        afterCreate_.emplace_back([field]() { return field->OfferNotification(); });
        beforeDestroy_.emplace_back([field]() { return field->StopOfferNotification(); });
        beforeOffer_.emplace_back([field]() { return field->RegisterSetReceiver(); });
        afterStopOffer_.emplace_back([field]() { return field->UnregisterSetReceiver(); });
        using Result = ara::core::Result< void >;
        Result result{};
        for (auto& after : afterCreate_) {
            auto res{after()};
            if (res) {
                continue;
            }
            result = std::move(res);
            ComLogError("create bind skeleton field(ns) error: invoke after create failed", GenArg(pServiceInfo_),
                        GenArg(result));
            break;
        }
        afterCreate_.clear();
        return field;
    }
    /// @brief Create binding layer field instance -- notifiable/gettable
    /// @tparam Desc Description information
    /// @tparam T Field data type
    /// @return Field instance pointer
    template < typename Desc, typename T >
    auto Create(std::enable_if_t< ara::com::internal::skeleton::IsNGField< T >::value >* = nullptr) noexcept
    {
        auto field{std::make_shared< NSomeipNGField< NSomeipSkeleton, Desc, typename T::FieldType > >(*this)};
        // [TODO] Provide notification immediately after service creation (solve error reporting issue when updating field before service provision)
        afterCreate_.emplace_back([field]() { return field->OfferNotification(); });
        beforeDestroy_.emplace_back([field]() { return field->StopOfferNotification(); });
        beforeOffer_.emplace_back([field]() { return field->RegisterGetReceiver(); });
        afterStopOffer_.emplace_back([field]() { return field->UnregisterGetReceiver(); });
        using Result = ara::core::Result< void >;
        Result result{};
        for (auto& after : afterCreate_) {
            auto res{after()};
            if (res) {
                continue;
            }
            result = std::move(res);
            ComLogError("create bind skeleton field(ng) error: invoke after create failed", GenArg(pServiceInfo_),
                        GenArg(result));
            break;
        }
        afterCreate_.clear();
        return field;
    }
    /// @brief Create binding layer field instance -- settable/gettable
    /// @tparam Desc Description information
    /// @tparam T Field data type
    /// @return Field instance pointer
    template < typename Desc, typename T >
    auto Create(std::enable_if_t< ara::com::internal::skeleton::IsSGField< T >::value >* = nullptr) noexcept
    {
        auto field{std::make_shared< NSomeipSGField< NSomeipSkeleton, Desc, typename T::FieldType > >(*this)};
        beforeOffer_.emplace_back([field]() { return field->RegisterSetReceiver(); });
        afterStopOffer_.emplace_back([field]() { return field->UnregisterSetReceiver(); });
        beforeOffer_.emplace_back([field]() { return field->RegisterGetReceiver(); });
        afterStopOffer_.emplace_back([field]() { return field->UnregisterGetReceiver(); });
        return field;
    }
    /// @brief Create binding layer field instance -- notifiable/settable/gettable
    /// @tparam Desc Description information
    /// @tparam T Field data type
    /// @return Field instance pointer
    template < typename Desc, typename T >
    auto Create(std::enable_if_t< ara::com::internal::skeleton::IsNSGField< T >::value >* = nullptr) noexcept
    {
        auto field{std::make_shared< NSomeipNSGField< NSomeipSkeleton, Desc, typename T::FieldType > >(*this)};
        // [TODO] Provide notification immediately after service creation (solve error reporting issue when updating field before service provision)
        afterCreate_.emplace_back([field]() { return field->OfferNotification(); });
        beforeDestroy_.emplace_back([field]() { return field->StopOfferNotification(); });
        beforeOffer_.emplace_back([field]() { return field->RegisterSetReceiver(); });
        afterStopOffer_.emplace_back([field]() { return field->UnregisterSetReceiver(); });
        beforeOffer_.emplace_back([field]() { return field->RegisterGetReceiver(); });
        afterStopOffer_.emplace_back([field]() { return field->UnregisterGetReceiver(); });
        using Result = ara::core::Result< void >;
        Result result{};
        for (auto& after : afterCreate_) {
            auto res{after()};
            if (res) {
                continue;
            }
            result = std::move(res);
            ComLogError("create bind skeleton field(nsg) error: invoke after create failed", GenArg(pServiceInfo_),
                        GenArg(result));
            break;
        }
        afterCreate_.clear();
        return field;
    }
    /// @brief Constructor
    /// @param[in] pServiceInfo Service information -- skeleton side
    /// @param[in] bindRuntime Binding layer runtime
    /// @param[in] skeleton Service skeleton
    NSomeipSkeleton(PServiceInfo const& pServiceInfo, NSomeipRuntime& bindRuntime, Skeleton& skeleton) noexcept
        : pServiceInfo_{pServiceInfo}
        , bindRuntime_{bindRuntime}
        , skeleton_{skeleton.This()}
        , instanceId_{pServiceInfo.instanceId}
    {
        ComLogTrace("create bind skeleton", GenArg(pServiceInfo_));
    }
    /// @brief Destructor
    ~NSomeipSkeleton() noexcept override
    {
        using Result = ara::core::Result< void >;
        Result result{};
        ComLogTrace("destroy bind skeleton", GenArg(pServiceInfo_));
        /// User may not call StopOfferService when terminating process, send monitoring log again during destruction
        MonitorTrace(runtime::GetInstance(), trace::someip::kServiceOffered, trace::ServiceType::kSkeleton,
                     ServiceDesc::serviceId(), pServiceInfo_.instanceId, ServiceDesc::serviceIdentifier,
                     static_cast< uint8_t >(trace::InstanceState::kStateStop), 0);
        for (auto& before : beforeDestroy_) {
            auto res{before()};
            if (res) {
                continue;
            }
            result = std::move(res);
            ComLogError("destroy bind skeleton error: invoke before destroy failed", GenArg(pServiceInfo_),
                        GenArg(result));
        }
        std::lock_guard< std::mutex > lock(callsMutex_);
        pendingCalls_.clear();
    }
    /// @brief Copy constructor
    /// @param other
    NSomeipSkeleton(NSomeipSkeleton const& other) noexcept = default;
    /// @brief Move constructor
    /// @param other
    NSomeipSkeleton(NSomeipSkeleton&& other) noexcept = default;
    /// @brief Copy assignment operator
    /// @param other
    /// @return NSomeipSkeleton
    NSomeipSkeleton& operator=(NSomeipSkeleton const& other) noexcept = default;
    /// @brief Move assignment operator
    /// @param other
    /// @return NSomeipSkeleton
    NSomeipSkeleton& operator=(NSomeipSkeleton&& other) noexcept = default;
    /// @brief Provide service
    /// @return Result object -- void/value or error
    ara::core::Result< void > OfferService() noexcept override
    {
        using Result = ara::core::Result< void >;
        Result result{};
        ComLogTrace("offer bind service", GenArg(pServiceInfo_));
        MonitorTrace(runtime::GetInstance(), trace::someip::kServiceOffered, trace::ServiceType::kSkeleton,
                     ServiceDesc::serviceId(), pServiceInfo_.instanceId, ServiceDesc::serviceIdentifier,
                     static_cast< uint8_t >(trace::InstanceState::kStateStarting), 0);
        for (auto& before : beforeOffer_) {
            auto res{before()};
            if (res) {
                continue;
            }
            result = std::move(res);
            ComLogError("offer bind service error: invoke before offer failed", GenArg(pServiceInfo_), GenArg(result));
            MonitorTrace(runtime::GetInstance(), trace::someip::kServiceOffered, trace::ServiceType::kSkeleton,
                         ServiceDesc::serviceId(), pServiceInfo_.instanceId, ServiceDesc::serviceIdentifier,
                         static_cast< uint8_t >(trace::InstanceState::kStateError),
                         static_cast< uint32_t >(ComErrc::kCommunicationStackError));
            return result;
        }
        auto serviceId{pServiceInfo_.serviceId};
        auto instanceId{pServiceInfo_.instanceId};
        auto major{pServiceInfo_.version.major};
        auto minor{pServiceInfo_.version.minor};
        auto ret{bindRuntime_.OfferService({serviceId, instanceId, major, minor})};
        if (ret < 0) {
            result.EmplaceError(MakeErrorCode(ComErrc::kServiceNotOffered, ret));
            ComLogError("offer bind service error: invoke offer bind service failed", GenArg(pServiceInfo_),
                        GenArg(result));
            MonitorTrace(runtime::GetInstance(), trace::someip::kServiceOffered, trace::ServiceType::kSkeleton,
                         ServiceDesc::serviceId(), pServiceInfo_.instanceId, ServiceDesc::serviceIdentifier,
                         static_cast< uint8_t >(trace::InstanceState::kStateError),
                         static_cast< uint32_t >(ComErrc::kCommunicationStackError));
            return result;
        }
        for (auto& after : afterOffer_) {
            auto res{after()};
            if (res) {
                continue;
            }
            result = std::move(res);
            ComLogError("offer bind service error: invoke after offer failed", GenArg(pServiceInfo_), GenArg(result));
            MonitorTrace(runtime::GetInstance(), trace::someip::kServiceOffered, trace::ServiceType::kSkeleton,
                         ServiceDesc::serviceId(), pServiceInfo_.instanceId, ServiceDesc::serviceIdentifier,
                         static_cast< uint8_t >(trace::InstanceState::kStateError),
                         static_cast< uint32_t >(ComErrc::kCommunicationStackError));
            return result;
        }
        ComLogDebug("offer bind service done", GenArg(pServiceInfo_));
        MonitorTrace(runtime::GetInstance(), trace::someip::kServiceOffered, trace::ServiceType::kSkeleton,
                     ServiceDesc::serviceId(), pServiceInfo_.instanceId, ServiceDesc::serviceIdentifier,
                     static_cast< uint8_t >(trace::InstanceState::kStateRunning), 0);
        return result;
    }
    /// @brief Stop providing service
    void StopOfferService() noexcept override
    {
        using Result = ara::core::Result< void >;
        Result result{};
        ComLogTrace("stop offer bind service", GenArg(pServiceInfo_));
        MonitorTrace(runtime::GetInstance(), trace::someip::kServiceOffered, trace::ServiceType::kSkeleton,
                     ServiceDesc::serviceId(), pServiceInfo_.instanceId, ServiceDesc::serviceIdentifier,
                     static_cast< uint8_t >(trace::InstanceState::kStateStopping), 0);
        for (auto& before : beforeStopOffer_) {
            auto res{before()};
            if (res) {
                continue;
            }
            result = std::move(res);
            ComLogError("stop offer bind service error: invoke before stop offer failed", GenArg(pServiceInfo_),
                        GenArg(result));
        }
        auto serviceId{pServiceInfo_.serviceId};
        auto instanceId{pServiceInfo_.instanceId};
        auto major{pServiceInfo_.version.major};
        auto minor{pServiceInfo_.version.minor};
        auto ret{bindRuntime_.StopService({serviceId, instanceId, major, minor})};
        if (ret < 0) {
            result.EmplaceError(MakeErrorCode(ComErrc::kServiceNotOffered, ret));
            ComLogError("stop offer bind service error: invoke stop service failed", GenArg(pServiceInfo_),
                        GenArg(result));
        }
        for (auto& after : afterStopOffer_) {
            auto res{after()};
            if (res) {
                continue;
            }
            result = std::move(res);
            ComLogError("stop offer bind service error: invoke after stop offer failed", GenArg(pServiceInfo_),
                        GenArg(result));
        }
        ComLogDebug("stop offer bind service done", GenArg(pServiceInfo_));
        MonitorTrace(runtime::GetInstance(), trace::someip::kServiceOffered, trace::ServiceType::kSkeleton,
                     ServiceDesc::serviceId(), pServiceInfo_.instanceId, ServiceDesc::serviceIdentifier,
                     static_cast< uint8_t >(trace::InstanceState::kStateStop), 0);
    }
    /// @brief Process next method call
    /// @return Future object -- asynchronous/synchronous waiting for result object
    ara::core::Future< bool > ProcessNextMethodCall() noexcept override
    {
        NSOMEIP_ASSERT((**skeleton_).GetMethodCallProcessingMode() == MethodCallProcessingMode::kPoll,
                       "process next bind method call exception: method call processing mode invalid");
        PendingCall call;
        {
            std::lock_guard< std::mutex > lock(callsMutex_);
            if (!pendingCalls_.empty()) {
                call = std::move(pendingCalls_.front());
                pendingCalls_.pop_front();
            }
        }
        ara::core::Promise< bool > promise;
        if (call) {
            // TODO(?): Make this truly async
            call();
            promise.set_value(true);
        } else {
            promise.set_value(false);
        }
        return promise.get_future();
    }

private:
    template < typename Owner, typename Desc, typename T >
    friend class NSomeipEvent;
    template < typename Owner, typename Desc, typename Callback >
    friend class NSomeipMethod;
    /// @brief Service information -- skeleton side
    PServiceInfo const& pServiceInfo_;
    /// @brief Binding layer runtime
    NSomeipRuntime& bindRuntime_;
    /// @brief Shared skeleton pointer
    typename Skeleton::SharedThis skeleton_;
    /// @brief Instance identifier
    ServiceInfo::InstanceId instanceId_;
    /// @brief Type alias -- pending method call
    using PendingCall = std::function< void(void) >;
    /// @brief Type alias -- pending method call queue
    using PendingCalls = std::deque< PendingCall >;
    /// @brief Pending method call queue
    PendingCalls pendingCalls_;
    /// @brief Mutex -- for pending method call
    std::mutex callsMutex_;
    /// @brief Function list -- after service creation
    ara::core::Vector< std::function< ara::core::Result< void >() > > afterCreate_;
    /// @brief Function list -- before service destruction
    ara::core::Vector< std::function< ara::core::Result< void >() > > beforeDestroy_;
    /// @brief Function list -- before service provision
    ara::core::Vector< std::function< ara::core::Result< void >() > > beforeOffer_;
    /// @brief Function list -- after service provision
    ara::core::Vector< std::function< ara::core::Result< void >() > > afterOffer_;
    /// @brief Function list -- before stopping service provision
    ara::core::Vector< std::function< ara::core::Result< void >() > > beforeStopOffer_;
    /// @brief Function list -- after stopping service provision
    ara::core::Vector< std::function< ara::core::Result< void >() > > afterStopOffer_;
};
/// @brief Create binding layer skeleton instance
/// @tparam ServiceDesc Service description information
/// @tparam ServiceSkeleton Service skeleton type
/// @param[in] skeleton Service skeleton
/// @param[in] instanceIdentifier Instance identifier
/// @return Skeleton instance pointer
template < typename ServiceDesc, typename ServiceSkeleton >
auto Create(ServiceSkeleton& skeleton, ara::com::InstanceIdentifier const& instanceIdentifier) noexcept
{
    return NSomeipSkeleton< ServiceDesc, ServiceSkeleton >::Create(skeleton, instanceIdentifier);
}
/// @brief Skeleton mapper implementation type
/// @tparam SkeletonImpl Skeleton implementation type
template < typename SkeletonImpl >
class NSomeipSkeletonMapper : public NSomeipRuntime::SkeletonMapperBase
{
public:
    /// @brief Get service identifier
    /// @return Service identifier
    ServiceIdentifier const& GetServiceIdentifier() const noexcept override { return SkeletonImpl::serviceIdentifier; }
    /// @brief Get service identifier
    /// @return Service identifier
    ServiceInfo::ServiceId GetServiceId() const noexcept override { return SkeletonImpl::serviceId(); }
    /// @brief Create binding layer skeleton instance
    /// @param[in] skeleton Service skeleton
    /// @param[in] instanceIdentifier Instance identifier
    /// @return Skeleton instance pointer
    std::unique_ptr< ara::com::internal::skeleton::BindSkeleton > CreateBindSkeleton(
        ara::com::internal::skeleton::Skeleton& skeleton,
        InstanceIdentifier const& instanceIdentifier) const noexcept override
    {
        auto const mode{skeleton.GetMethodCallProcessingMode()};
        auto const& serviceIdentifier{skeleton.GetServiceIdentifier()};
        auto const serviceId{SkeletonImpl::serviceId()};
        if (mode == MethodCallProcessingMode::kEvent && ara::com::e2exf::StatusHandler::HasE2E(serviceId)) {
            ComLogError("create bind skeleton error: method call processing mode wrong", GenArg(serviceIdentifier),
                        GenArg(serviceId), GenArg(instanceIdentifier));
            return nullptr;
        }
        ComLogDebug("create bind skeleton", GenArg(serviceIdentifier), GenArg(serviceId), GenArg(instanceIdentifier));
        return SkeletonImpl::CreateBindSkeleton(static_cast< typename SkeletonImpl::Skeleton& >(skeleton),
                                                instanceIdentifier);
    }
};
}  // namespace skeleton
}  // namespace nsomeip
}  // namespace internal
}  // namespace com
}  // namespace ara
#endif
