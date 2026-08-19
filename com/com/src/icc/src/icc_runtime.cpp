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
/// @file       icc_runtime.cpp
/// @brief      Binding layer runtime implementation file
/// @details
/// @date       2021-12-31
/// @author     mazelin
/// @version    1.2.0
///
/// ================================================================

#include "ara/com/internal/icc/icc_runtime.h"

#include "ara/com/internal/proxy/proxy.h"
#include "ara/com/internal/skeleton/skeleton.h"
#include "icc/core/icc_routing.h"
#include "isoft/ara_fsh/process.h"
#include "isoft/manifestreader/manifest_reader.h"
#include "isoft/naicpp/global_evloop.h"
#include "nai/os/nai_stat.h"
#ifdef HAS_COM_SECOC
    #include "ara/com/internal/secoc/secoc_someip.h"
#else
#endif
#ifdef HAS_ARA_IAM
    #include "ara/iam/internal/grant/serialization_error_domain.h"
    #include "ara/iam/internal/grantquery/com.h"
#else
#endif

namespace ara {
namespace com {
namespace internal {
namespace icc {
/// @brief Flag indicating whether it is a daemon
bool g_IsDaemon{false};
/// @brief Binding layer runtime instance pointer
std::unique_ptr< IccRuntime > g_IccRuntime{};
ara::core::Result< void > Initialize(bool isDaemon) noexcept
{
    using Result = ara::core::Result< void >;
    ComLogTrace("initialize bind runtime", GenArg(isDaemon), GenArg(g_IccRuntime));
    Result result{};
    assert(g_IccRuntime == nullptr);
    g_IsDaemon   = isDaemon;
    g_IccRuntime = std::make_unique< IccRuntime >();
    if (auto res{g_IccRuntime->Initialize()}) {
    } else {
        std::ignore = Deinitialize();
        result      = std::move(res);
        ComLogError("initialize bind runtime error", GenArg(result), GenArg(isDaemon), GenArg(g_IccRuntime));
        return result;
    }
    ComLogDebug("initialize bind runtime done", GenArg(isDaemon), GenArg(g_IccRuntime));
    return result;
}
ara::core::Result< void > Deinitialize() noexcept
{
    using Result = ara::core::Result< void >;
    ComLogTrace("deinitialize bind runtime", GenArg(g_IccRuntime));
    Result result{};
    assert(g_IccRuntime != nullptr);
    if (auto res{g_IccRuntime->Deinitialize()}) {
    } else {
        result = std::move(res);
    }
    g_IccRuntime = nullptr;
    ComLogDebug("deinitialize bind runtime done", GenArg(g_IccRuntime));
    return result;
}

namespace runtime {
IccRuntime& GetInstance() noexcept
{
    assert(g_IccRuntime != nullptr);
    return *g_IccRuntime;
}
/// @brief Binding layer instance identifier prefix
ara::core::String const& InstanceIdentifierPrefix()
{
    static const ara::core::String kInstanceIdentifierPrefix{"ICC:"};
    return kInstanceIdentifierPrefix;
}
InstanceIdentifier ToInstanceIdentifier(ServiceInfo::InstanceId const& input) noexcept
{
    if (input == Message::kInstanceIdAny) {
        return InstanceIdentifier{InstanceIdentifierPrefix() + InstanceIdentifier::kAny};
    }
    return InstanceIdentifier{InstanceIdentifierPrefix() + std::to_string(static_cast< uint32_t >(input))};
}
ara::core::Optional< ServiceInfo::InstanceId > ToIccInstanceId(InstanceIdentifier const& input) noexcept
{
    if (input.IsAny(InstanceIdentifierPrefix())) {
        return Message::kInstanceIdAny;
    }
    if (input.ToString().substr(0, InstanceIdentifierPrefix().size()) != InstanceIdentifierPrefix()) {
        ComLogTrace("convert to instance id ignored: verify prefix invalid", GenArg(input),
                    GenArg(InstanceIdentifierPrefix()));
        return {};
    }
    try {
        return static_cast< ServiceInfo::InstanceId >(
            std::stoi(input.ToString().substr(InstanceIdentifierPrefix().size()).data()));
    } catch (std::exception const& e) {
        // Conversion failed, return empty value
        ComLogDebug("convert to instance id ignored: convert failed", GenArg(e.what()), GenArg(input));
        return {};
    }
}
}  // namespace runtime
/// @brief Binding layer runtime implementation type
class IccRuntime::Impl : public internal::BindRuntime
{
public:
    /// @brief Constructor
    Impl() noexcept
    {
        ComLogTrace("create bind runtime", GenArg(this));
        ICC_ASSERT(icc_conf_init(&conf_), -__LINE__);
        ICC_ASSERT(icc_routing_init(&routing_), -__LINE__);
        ICC_ASSERT(icc_app_init(&app_), -__LINE__);
        if (!g_IsDaemon) {
            ICC_ASSERT(icc_appcb_init(&appcb_), -__LINE__);
        }
    }
    /// @brief Destructor
    ~Impl() noexcept override { ComLogTrace("destroy bind runtime", GenArg(this)); }
    Impl(Impl const& other) noexcept = delete;
    Impl(Impl&& other) noexcept      = delete;
    Impl& operator=(Impl const& other) noexcept = delete;
    Impl& operator=(Impl&& other) noexcept = delete;
    /// @brief Initialize
    /// @return Result object -- empty/value or error
    ara::core::Result< void > Initialize() noexcept
    {
        using Result = ara::core::Result< void >;
        ComLogTrace("initialize bind runtime", GenArg(this));
        Result result{};
        if (auto res{ICC_CHECK(InitializeBind(), -__LINE__)}) {
            deinitializes_.emplace_front([&] { return ICC_CHECK(DeinitializeBind(), -__LINE__); });
        } else {
            result = std::move(res);
            return result;
        }
        if (auto res{ICC_CHECK(InitializeICC(), -__LINE__)}) {
            deinitializes_.emplace_front([&] { return ICC_CHECK(DeinitializeICC(), -__LINE__); });
        } else {
            result = std::move(res);
            return result;
        }
        if (auto res{ICC_CHECK(InitializeE2E(), -__LINE__)}) {
            deinitializes_.emplace_front([&] { return ICC_CHECK(DeinitializeE2E(), -__LINE__); });
        } else {
            result = std::move(res);
            return result;
        }
#ifdef HAS_COM_SECOC
        if (auto res{ICC_CHECK(InitializeSecOC(), -__LINE__)}) {
            deinitializes_.emplace_front([&] { return ICC_CHECK(DeinitializeSecOC(), -__LINE__); });
        } else {
            result = std::move(res);
            return result;
        }
#else
#endif
#ifdef HAS_ARA_IAM
        if (auto res{ICC_CHECK(InitializeIAM(), -__LINE__)}) {
            deinitializes_.emplace_front([&] { return ICC_CHECK(DeinitializeIAM(), -__LINE__); });
        } else {
            result = std::move(res);
            return result;
        }
#else
#endif
        return result;
    }
    /// @brief Deinitialize
    /// @return Result object -- empty/value or error
    ara::core::Result< void > Deinitialize() noexcept
    {
        using Result = ara::core::Result< void >;
        ComLogTrace("deinitialize bind runtime", GenArg(this));
        Result result{};
        while (!deinitializes_.empty()) {
            auto deinitialize{std::move(deinitializes_.front())};
            deinitializes_.pop_front();
            if (auto res{deinitialize()}) {
            } else {
                result = std::move(res);
            }
        }
        return result;
    }
    /// @brief Get implementation layer application
    /// @return Implementation layer application pointer
    icc_app_t* GetApp() noexcept { return &app_; }
    /// @brief Get implementation layer application callback
    /// @return Implementation layer application callback pointer
    icc_appcb_t* GetAppCB() noexcept { return &appcb_; }
    /// @brief Get service information -- skeleton side
    /// @return Service information pointer
    ServiceInfo* GetPServiceInfo(decltype(ServiceInfo::serviceId) serviceId,
                                 decltype(ServiceInfo::instanceId) instanceId) const noexcept
    {
        auto itServiceInfo{pServiceInfos_.find({serviceId, instanceId})};
        if (itServiceInfo == pServiceInfos_.end()) {
            return nullptr;
        }
        auto const& serviceInfo{itServiceInfo->second};
        return serviceInfo.get();
    }
    /// @brief Get service information -- proxy side
    /// @return Service information pointer
    ServiceInfo* GetRServiceInfo(decltype(ServiceInfo::serviceId) serviceId,
                                 decltype(ServiceInfo::instanceId) instanceId) const noexcept
    {
        auto itServiceInfo{rServiceInfos_.find({serviceId, instanceId})};
        if (itServiceInfo == rServiceInfos_.end()) {
            return nullptr;
        }
        auto const& serviceInfo{itServiceInfo->second};
        return serviceInfo.get();
    }
    /// @brief Register proxy mapper
    /// @tparam mapper Proxy mapper instance
    void RegisterProxyMapper(std::shared_ptr< ProxyMapperBase > const& mapper) noexcept
    {
        ComLogTrace("register bind proxy mapper", GenArg(mapper->GetServiceIdentifier()),
                    GenArg(mapper->GetServiceId()));
        auto& mapper1{serviceIdentifier2RMappers_[mapper->GetServiceIdentifier()][mapper->GetServiceId()]};
        auto& mapper2{rMappers_[mapper->GetServiceId()]};
        ICC_ASSERT(mapper1 == nullptr && mapper2 == nullptr, "register proxy mapper repeat");
        mapper1 = mapper2 = mapper;
    }
    /// @brief Register skeleton mapper
    /// @tparam mapper Skeleton mapper instance
    void RegisterSkeletonMapper(std::shared_ptr< SkeletonMapperBase > const& mapper) noexcept
    {
        ComLogTrace("register bind skeleton mapper", GenArg(mapper->GetServiceIdentifier()),
                    GenArg(mapper->GetServiceId()));
        auto& mapper1{serviceIdentifier2PMappers_[mapper->GetServiceIdentifier()][mapper->GetServiceId()]};
        auto& mapper2{pMappers_[mapper->GetServiceId()]};
        ICC_ASSERT(mapper1 == nullptr && mapper2 == nullptr, "register skeleton mapper repeat");
        mapper1 = mapper2 = mapper;
    }

public:
    /// @brief Initialize binding layer
    /// @return Result object -- empty/value or error
    ara::core::Result< void > InitializeBind() noexcept
    {
        using Result = ara::core::Result< void >;
        ComLogTrace("initialize bind", GenArg(this));
        Result result{};
        int32_t ret{};
        isoft::ara_fsh::Process proc;
        auto siManifestFile{proc.GetConfig("service_instance_manifest.json")};
        // constexpr auto siManifestFile{"etc/service_instance_manifest.json"};
        ComLogTrace("initialize bind: load config", GenArg(siManifestFile));
        auto siManifestRes{isoft::manifestreader::OpenManifest(siManifestFile.c_str())};
        if (!siManifestRes) {
            // Ignored here, initializing the communication runtime maps instance descriptors and instance identifier sets. If opening fails at this point, it only means the application does not require instance deployment configuration.
            ComLogDebug("initialize bind ignored: open config failed", GenArg(siManifestRes), GenArg(siManifestFile));
            // result.EmplaceError(std::move(siManifestRes).Error());
            return result;
        }
        auto siManifest{std::move(siManifestRes).Value()};
        std::ignore = siManifest->IterateObject("ap_service_instances", [&](auto& key1, auto& node1) {
            if (ret != isoft::kSuccess) {
                return;
            }
            if (key1 == "provided_ap_service_instances") {
                std::ignore = node1.IterateArray("", [&](auto idx2In, auto& node2) {
                    auto idx2{static_cast< int64_t >(idx2In)};
                    if (ret != isoft::kSuccess) {
                        return;
                    }
                    ara::core::String instanceId{};
                    if ((ret = node2.Load("instance_id", instanceId)) != isoft::kSuccess) {
                        ComLogError("load node failed", GenArg(ret), GenArg(key1), GenArg(idx2), "instance_id");
                        return;
                    }
                    InstanceIdentifier instanceIdentifier{instanceId};
                    ret = this->LoadPServiceInfo(std::move(instanceIdentifier), node2);
                    if (ret != isoft::kSuccess) {
                        ComLogError("load node failed", GenArg(ret), GenArg(key1), GenArg(idx2));
                        return;
                    }
                });
            } else if (key1 == "required_ap_service_instances") {
                std::ignore = node1.IterateArray("", [&](auto idx2In, auto& node2) {
                    auto idx2{static_cast< int64_t >(idx2In)};
                    if (ret != isoft::kSuccess) {
                        return;
                    }
                    ara::core::String instanceId{};
                    if ((ret = node2.Load("instance_id", instanceId)) != isoft::kSuccess) {
                        ComLogError("load node failed", GenArg(ret), GenArg(key1), GenArg(idx2), "instance_id");
                        return;
                    }
                    InstanceIdentifier instanceIdentifier{instanceId};
                    ret = this->LoadRServiceInfo(std::move(instanceIdentifier), node2);
                    if (ret != isoft::kSuccess) {
                        ComLogError("load node failed", GenArg(ret), GenArg(key1), GenArg(idx2));
                        return;
                    }
                });
            }
        });
        if (ret != isoft::kSuccess) {
            ComLogError("initialize bind error: parse config failed", GenArg(ret), GenArg(siManifestFile));
            result.EmplaceError(MakeErrorCode(ComErrc::kCommunicationStackError, ret));
            return result;
        }
        auto& comRuntime{internal::GetInstance()};
        for (auto& itServiceInfo : pServiceInfos_) {
            auto& serviceInfo{itServiceInfo.second};
            auto& instanceId{serviceInfo->instanceId};
            auto* pServiceInfo{reinterpret_cast< PServiceInfo* >(serviceInfo.get())};
            auto& threadPoolSize{pServiceInfo->threadPoolSize};
            if (pServiceInfo->threadPoolSize != 0) {
                auto instanceIdentifier{runtime::ToInstanceIdentifier(instanceId)};
                comRuntime.CreateThreadPool(instanceIdentifier.ToString(), threadPoolSize);
            }
        }
        for (auto& itServiceInfo : rServiceInfos_) {
            auto& serviceInfo{itServiceInfo.second};
            auto& instanceId{serviceInfo->instanceId};
            auto* rServiceInfo{reinterpret_cast< RServiceInfo* >(serviceInfo.get())};
            auto& threadPoolSize{rServiceInfo->threadPoolSize};
            if (threadPoolSize != 0) {
                auto instanceIdentifier{runtime::ToInstanceIdentifier(instanceId)};
                comRuntime.CreateThreadPool(instanceIdentifier.ToString(), threadPoolSize);
            }
        }
        ComLogTrace("initialize bind done", GenArg(this));
        return result;
    }
    /// @brief Deinitialize binding layer
    /// @return Result object -- empty/value or error
    ara::core::Result< void > DeinitializeBind() noexcept
    {
        using Result = ara::core::Result< void >;
        ComLogTrace("deinitialize bind", GenArg(this));
        Result result{};
        auto& comRuntime{internal::GetInstance()};
        for (auto& itServiceInfo : pServiceInfos_) {
            auto& serviceInfo{itServiceInfo.second};
            auto& instanceId{serviceInfo->instanceId};
            auto* pServiceInfo{reinterpret_cast< PServiceInfo* >(serviceInfo.get())};
            auto& threadPoolSize{pServiceInfo->threadPoolSize};
            if (threadPoolSize != 0) {
                auto instanceIdentifier{runtime::ToInstanceIdentifier(instanceId)};
                comRuntime.DestroyThreadPool(instanceIdentifier.ToString());
            }
        }
        for (auto& itServiceInfo : rServiceInfos_) {
            auto& serviceInfo{itServiceInfo.second};
            auto& instanceId{serviceInfo->instanceId};
            auto* rServiceInfo{reinterpret_cast< RServiceInfo* >(serviceInfo.get())};
            auto& threadPoolSize{rServiceInfo->threadPoolSize};
            if (threadPoolSize != 0) {
                auto instanceIdentifier{runtime::ToInstanceIdentifier(instanceId)};
                comRuntime.DestroyThreadPool(instanceIdentifier.ToString());
            }
        }
        pServiceInfos_.clear();
        rServiceInfos_.clear();
        ComLogTrace("deinitialize bind done", GenArg(this));
        return result;
    }
    /// @brief Load ServiceInfo -- skeleton side
    /// @return Result -- 0: success; !0: failure;
    int32_t LoadPServiceInfo(InstanceIdentifier&& instanceIdentifier,
                             isoft::manifestreader::ManifestNode const& node2) noexcept
    {
        ComLogTrace("load bind provided service info", GenArg(instanceIdentifier));
        int32_t ret{};
        auto optionalId{runtime::ToIccInstanceId(instanceIdentifier)};
        if (!optionalId) {  // Conversion failed, instance identifier not supported
            return ret;
        }
        auto pServiceInfo{std::make_unique< PServiceInfo >(std::move(instanceIdentifier))};
        pServiceInfo->type       = ServiceInfo::Type::kProvided;
        pServiceInfo->instanceId = *optionalId;
#if 1  // Load PServiceInfo
        bool hasServiceId{false};
        bool hasVersion{false};
        bool hasServiceOfferTimeToLive{false};
        std::ignore = node2.IterateObject("", [&](auto& key3, auto& node3) {
            if (ret != isoft::kSuccess) {
                return;
            }
            if (key3 == "name") {
                auto& name{pServiceInfo->name};
                if ((ret = node3.Load("", name)) != isoft::kSuccess) {
                    ComLogError("load node failed", GenArg(ret), GenArg(key3));
                    return;
                }
            } else if (key3 == "service_interface_id") {
                auto& serviceId{pServiceInfo->serviceId};
                if ((ret = node3.Load("", serviceId)) != isoft::kSuccess) {
                    ComLogError("load node failed", GenArg(ret), GenArg(key3));
                    return;
                }
                hasServiceId = true;
            } else if (key3 == "service_interface_version") {
                auto& version{pServiceInfo->version};
                auto& major{version.major};
                auto& minor{version.minor};
                if ((ret = node3.Load("major_version", major)) != isoft::kSuccess) {
                    ComLogError("load node failed", GenArg(ret), GenArg(key3), GenVal("major_version"));
                    return;
                }
                if ((ret = node3.Load("minor_version", minor)) != isoft::kSuccess) {
                    ComLogError("load node failed", GenArg(ret), GenArg(key3), GenVal("minor_version"));
                    return;
                }
                hasVersion = true;
            } else if (key3 == "load_balancing_priority") {
                auto& loadBalancingPriority{pServiceInfo->loadBalancingPriority};
                if ((ret = node3.Load("", loadBalancingPriority)) != isoft::kSuccess) {
                    ComLogError("load node failed", GenArg(ret), GenArg(key3));
                    return;
                }
            } else if (key3 == "load_balancing_weight") {
                auto& loadBalancingWeight{pServiceInfo->loadBalancingWeight};
                if ((ret = node3.Load("", loadBalancingWeight)) != isoft::kSuccess) {
                    ComLogError("load node failed", GenArg(ret), GenArg(key3));
                    return;
                }
            } else if (key3 == "sd_server_config") {
                auto& sdServerConfig{pServiceInfo->sdServerConfig};
                std::ignore = node3.IterateObject("", [&](auto& key4, auto& node4) {
                    if (ret != isoft::kSuccess) {
                        return;
                    }
                    if (key4 == "initial_offer_behavior") {
                        auto& initialOfferBehavior{sdServerConfig.initialOfferBehavior};
                        bool hasInitialDelayMaxValue{false};
                        bool hasInitialDelayMinValue{false};
                        std::ignore = node4.IterateObject("", [&](auto& key5, auto& node5) {
                            if (ret != isoft::kSuccess) {
                                return;
                            }
                            if (key5 == "initial_delay_max_value") {
                                auto& initialDelayMaxValue{initialOfferBehavior.initialDelayMaxValue};
                                if ((ret = node5.Load("", initialDelayMaxValue)) != isoft::kSuccess) {
                                    ComLogError("load node failed", GenArg(ret), GenArg(key3), GenArg(key4),
                                                GenArg(key5));
                                    return;
                                }
                                hasInitialDelayMaxValue = true;
                            } else if (key5 == "initial_delay_min_value") {
                                auto& initialDelayMinValue{initialOfferBehavior.initialDelayMinValue};
                                if ((ret = node5.Load("", initialDelayMinValue)) != isoft::kSuccess) {
                                    ComLogError("load node failed", GenArg(ret), GenArg(key3), GenArg(key4),
                                                GenArg(key5));
                                    return;
                                }
                                hasInitialDelayMinValue = true;
                            } else if (key5 == "initial_repetitions_base_delay") {
                                auto& initialRepetitionsBaseDelay{initialOfferBehavior.initialRepetitionsBaseDelay};
                                if ((ret = node5.Load("", initialRepetitionsBaseDelay)) != isoft::kSuccess) {
                                    ComLogError("load node failed", GenArg(ret), GenArg(key3), GenArg(key4),
                                                GenArg(key5));
                                    return;
                                }
                            } else if (key5 == "initial_repetitions_max") {
                                auto& initialRepetitionsMax{initialOfferBehavior.initialRepetitionsMax};
                                if ((ret = node5.Load("", initialRepetitionsMax)) != isoft::kSuccess) {
                                    ComLogError("load node failed", GenArg(ret), GenArg(key3), GenArg(key4),
                                                GenArg(key5));
                                    return;
                                }
                            }
                        });
                        if (ret != isoft::kSuccess) {
                            return;
                        }
                        // [TODO] Check impact on platform startup, enable after upgrader implementation
                        if (false && !(hasInitialDelayMaxValue && hasInitialDelayMinValue)) {
                            ret = -__LINE__;
                            ComLogError("load node missed", GenArg(ret), GenArg(hasInitialDelayMaxValue),
                                        GenArg(hasInitialDelayMinValue));
                            return;
                        }
                    } else if (key4 == "offer_cyclic_delay") {
                        auto& offerCyclicDelay{sdServerConfig.offerCyclicDelay};
                        if ((ret = node4.Load("", offerCyclicDelay)) != isoft::kSuccess) {
                            ComLogError("load node failed", GenArg(ret), GenArg(key3), GenArg(key4));
                            return;
                        }
                    } else if (key4 == "request_response_delay") {
                        auto& requestResponseDelay{sdServerConfig.requestResponseDelay};
                        auto& maxValue{requestResponseDelay.maxValue};
                        auto& minValue{requestResponseDelay.minValue};
                        if ((ret = node4.Load("max_value", maxValue)) != isoft::kSuccess) {
                            ComLogError("load node failed", GenArg(ret), GenArg(key3), GenArg(key4),
                                        GenVal("max_value"));
                            return;
                        }
                        if ((ret = node4.Load("min_value", minValue)) != isoft::kSuccess) {
                            ComLogError("load node failed", GenArg(ret), GenArg(key3), GenArg(key4),
                                        GenVal("min_value"));
                            return;
                        }
                    } else if (key4 == "service_offer_time_to_live") {
                        auto& serviceOfferTimeToLive{sdServerConfig.serviceOfferTimeToLive};
                        if ((ret = node4.Load("", serviceOfferTimeToLive)) != isoft::kSuccess) {
                            ComLogError("load node failed", GenArg(ret), GenArg(key3), GenArg(key4));
                            return;
                        }
                        hasServiceOfferTimeToLive = true;
                    }
                });
            } else if (key3 == "udp_collection_buffer_size_threshold") {
                auto& udpCollectionBufferSizeThreshold{pServiceInfo->udpCollectionBufferSizeThreshold};
                if ((ret = node3.Load("", udpCollectionBufferSizeThreshold)) != isoft::kSuccess) {
                    ComLogError("load node failed", GenArg(ret), GenArg(key3));
                    return;
                }
            } else if (key3 == "thread_pool_size") {
                auto& threadPoolSize{pServiceInfo->threadPoolSize};
                if ((ret = node3.Load("", threadPoolSize)) != isoft::kSuccess) {
                    ComLogError("load node failed", GenArg(ret), GenArg(key3));
                    return;
                }
            }
        });
        if (ret != isoft::kSuccess) {
            return ret;
        }
        // [TODO] Check impact on platform startup, enable after upgrader implementation
        if (false && !(hasServiceId && hasVersion && hasServiceOfferTimeToLive)) {
            ret = -__LINE__;
            ComLogError("load node missed", GenArg(ret), GenArg(hasServiceId), GenArg(hasVersion),
                        GenArg(hasServiceOfferTimeToLive));
            return ret;
        }
#endif
#if 1  // Load PGroupInfo
        std::ignore = node2.IterateArray("event_groups", [&](auto idx3In, auto& node3) {
            auto idx3{static_cast< int64_t >(idx3In)};
            if (ret != isoft::kSuccess) {
                return;
            }
            ret = this->LoadPGroupInfo(*pServiceInfo, node3);
            if (ret != isoft::kSuccess) {
                ComLogError("load node failed", GenArg(ret), GenArg(idx3));
                return;
            }
        });
        if (ret != isoft::kSuccess) {
            return ret;
        }
#endif
#if 1  // Load PEventInfo
        std::ignore = node2.IterateArray("events", [&](auto idx3In, auto& node3) {
            auto idx3{static_cast< int64_t >(idx3In)};
            if (ret != isoft::kSuccess) {
                return;
            }
            ret = this->LoadPEventInfo(*pServiceInfo, node3);
            if (ret != isoft::kSuccess) {
                ComLogError("load node failed", GenArg(ret), GenArg(idx3));
                return;
            }
        });
        if (ret != isoft::kSuccess) {
            return ret;
        }
#endif
#if 1  // Load PMethodInfo
        std::ignore = node2.IterateArray("methods", [&](auto idx3In, auto& node3) {
            auto idx3{static_cast< int64_t >(idx3In)};
            if (ret != isoft::kSuccess) {
                return;
            }
            ret = this->LoadPMethodInfo(*pServiceInfo, node3);
            if (ret != isoft::kSuccess) {
                ComLogError("load node failed", GenArg(ret), GenArg(idx3));
                return;
            }
        });
        if (ret != isoft::kSuccess) {
            return ret;
        }
#endif
#if 1  // Load PFieldInfo
        std::ignore = node2.IterateArray("fields", [&](auto idx3In, auto& node3) {
            auto idx3{static_cast< int64_t >(idx3In)};
            if (ret != isoft::kSuccess) {
                return;
            }
            ret = this->LoadPFieldInfo(*pServiceInfo, node3);
            if (ret != isoft::kSuccess) {
                ComLogError("load node failed", GenArg(ret), GenArg(idx3));
                return;
            }
        });
        if (ret != isoft::kSuccess) {
            return ret;
        }
#endif
        auto& serviceInfo{pServiceInfos_[pServiceInfo->ID()]};
        if (serviceInfo != nullptr) {
            ret = -__LINE__;
            ComLogError("load node repeat", GenArg(ret), GenArg(pServiceInfo->ID()));
            return ret;
        }
        serviceInfo = std::move(pServiceInfo);
        return ret;
    }
    /// @brief Load ServiceInfo -- proxy side
    /// @return Result -- 0: success; !0: failure;
    int32_t LoadRServiceInfo(InstanceIdentifier&& instanceIdentifier,
                             isoft::manifestreader::ManifestNode const& node2) noexcept
    {
        ComLogTrace("load bind required service info", GenArg(instanceIdentifier));
        int32_t ret{};
        auto optionalId{runtime::ToIccInstanceId(instanceIdentifier)};
        if (!optionalId) {  // Conversion failed, instance identifier not supported
            return ret;
        }
        auto rServiceInfo{std::make_unique< RServiceInfo >(std::move(instanceIdentifier))};
        rServiceInfo->type       = ServiceInfo::Type::kRequired;
        rServiceInfo->instanceId = *optionalId;
#if 1  // Load RServiceInfo
        bool hasServiceId{false};
        bool hasVersion{false};
        bool hasServiceFindTimeToLive{false};
        std::ignore = node2.IterateObject("", [&](auto& key3, auto& node3) {
            if (ret != isoft::kSuccess) {
                return;
            }
            if (key3 == "name") {
                auto& name{rServiceInfo->name};
                if ((ret = node3.Load("", name)) != isoft::kSuccess) {
                    ComLogError("load node failed", GenArg(ret), GenArg(key3));
                    return;
                }
            } else if (key3 == "service_interface_id") {
                auto& serviceId{rServiceInfo->serviceId};
                if ((ret = node3.Load("", serviceId)) != isoft::kSuccess) {
                    ComLogError("load node failed", GenArg(ret), GenArg(key3));
                    return;
                }
                hasServiceId = true;
            } else if (key3 == "service_interface_version") {
                auto& version{rServiceInfo->version};
                auto& major{version.major};
                auto& minor{version.minor};
                if ((ret = node3.Load("major_version", major)) != isoft::kSuccess) {
                    ComLogError("load node failed", GenArg(ret), GenArg(key3), GenVal("major_version"));
                    return;
                }
                if ((ret = node3.Load("minor_version", minor)) != isoft::kSuccess) {
                    ComLogError("load node failed", GenArg(ret), GenArg(key3), GenVal("minor_version"));
                    return;
                }
                hasVersion = true;
            } else if (key3 == "blacklisted_versions") {
                std::ignore = node3.IterateArray("", [&](auto idx4In, auto& node4) {
                    auto idx4{static_cast< int64_t >(idx4In)};
                    if (ret != isoft::kSuccess) {
                        return;
                    }
                    ServiceInfo::Version version{};
                    auto& major{version.major};
                    auto& minor{version.minor};
                    if ((ret = node4.Load("major_version", major)) != isoft::kSuccess) {
                        ComLogError("load node failed", GenArg(ret), GenArg(key3), GenArg(idx4),
                                    GenVal("major_version"));
                        return;
                    }
                    if ((ret = node4.Load("minor_version", minor)) != isoft::kSuccess) {
                        ComLogError("load node failed", GenArg(ret), GenArg(key3), GenArg(idx4),
                                    GenVal("minor_version"));
                        return;
                    }
                    rServiceInfo->blacklistedVersions.emplace_back(version);
                });
            } else if (key3 == "required_minor_version") {
                auto& requiredMinorVersion{rServiceInfo->requiredMinorVersion};
                if (node3.IsString()) {
                    ara::core::String stRequiredMinorVersion;
                    if ((ret = node3.Load("", stRequiredMinorVersion)) != isoft::kSuccess) {
                        ComLogError("load node failed", GenArg(ret), GenArg(key3));
                        return;
                    }
                    if (stRequiredMinorVersion == "ANY" || stRequiredMinorVersion == "ALL") {
                        requiredMinorVersion = Message::kMinorVersionAny;
                    } else {
                        ret = -__LINE__;
                        ComLogError("load node failed", GenArg(ret), GenArg(key3));
                        return;
                    }
                } else {
                    if ((ret = node3.Load("", requiredMinorVersion)) != isoft::kSuccess) {
                        ComLogError("load node failed", GenArg(ret), GenArg(key3));
                        return;
                    }
                }
            } else if (key3 == "sd_client_config") {
                auto& sdClientConfig{rServiceInfo->sdClientConfig};
                std::ignore = node3.IterateObject("", [&](auto& key4, auto& node4) {
                    if (ret != isoft::kSuccess) {
                        return;
                    }
                    if (key4 == "initial_find_behavior") {
                        auto& initialFindBehavior{sdClientConfig.initialFindBehavior};
                        bool hasInitialDelayMaxValue{false};
                        bool hasInitialDelayMinValue{false};
                        std::ignore = node4.IterateObject("", [&](auto& key5, auto& node5) {
                            if (ret != isoft::kSuccess) {
                                return;
                            }
                            if (key5 == "initial_delay_max_value") {
                                auto& initialDelayMaxValue{initialFindBehavior.initialDelayMaxValue};
                                if ((ret = node5.Load("", initialDelayMaxValue)) != isoft::kSuccess) {
                                    ComLogError("load node failed", GenArg(ret), GenArg(key3), GenArg(key4),
                                                GenArg(key5));
                                    return;
                                }
                                hasInitialDelayMaxValue = true;
                            } else if (key5 == "initial_delay_min_value") {
                                auto& initialDelayMinValue{initialFindBehavior.initialDelayMinValue};
                                if ((ret = node5.Load("", initialDelayMinValue)) != isoft::kSuccess) {
                                    ComLogError("load node failed", GenArg(ret), GenArg(key3), GenArg(key4),
                                                GenArg(key5));
                                    return;
                                }
                                hasInitialDelayMinValue = true;
                            } else if (key5 == "initial_repetitions_base_delay") {
                                auto& initialRepetitionsBaseDelay{initialFindBehavior.initialRepetitionsBaseDelay};
                                if ((ret = node5.Load("", initialRepetitionsBaseDelay)) != isoft::kSuccess) {
                                    ComLogError("load node failed", GenArg(ret), GenArg(key3), GenArg(key4),
                                                GenArg(key5));
                                    return;
                                }
                            } else if (key5 == "initial_repetitions_max") {
                                auto& initialRepetitionsMax{initialFindBehavior.initialRepetitionsMax};
                                if ((ret = node5.Load("", initialRepetitionsMax)) != isoft::kSuccess) {
                                    ComLogError("load node failed", GenArg(ret), GenArg(key3), GenArg(key4),
                                                GenArg(key5));
                                    return;
                                }
                            }
                        });
                        if (ret != isoft::kSuccess) {
                            return;
                        }
                        // [TODO] Check impact on platform startup, enable after upgrader implementation
                        if (false && !(hasInitialDelayMaxValue && hasInitialDelayMinValue)) {
                            ret = -__LINE__;
                            ComLogError("load node missed", GenArg(ret), GenArg(hasInitialDelayMaxValue),
                                        GenArg(hasInitialDelayMinValue));
                            return;
                        }
                    } else if (key4 == "service_find_time_to_live") {
                        auto& serviceFindTimeToLive{sdClientConfig.serviceFindTimeToLive};
                        if ((ret = node4.Load("", serviceFindTimeToLive)) != isoft::kSuccess) {
                            ComLogError("load node failed", GenArg(ret), GenArg(key3), GenArg(key4));
                            return;
                        }
                        hasServiceFindTimeToLive = true;
                    }
                });
            } else if (key3 == "version_driven_find_behavior") {
                auto& versionDrivenFindBehavior{rServiceInfo->versionDrivenFindBehavior};
                ara::core::String stVersionDrivenFindBehavior{};
                if ((ret = node3.Load("", stVersionDrivenFindBehavior)) != isoft::kSuccess) {
                    ComLogError("load node failed", GenArg(ret), GenArg(key3));
                    return;
                }
                if (stVersionDrivenFindBehavior == "exactOrAnyMinorVersion") {
                    versionDrivenFindBehavior = RServiceInfo::VersionDrivenFindBehavior::kExactOrAnyMinorVersion;
                } else if (stVersionDrivenFindBehavior == "minimumMinorVersion") {
                    versionDrivenFindBehavior = RServiceInfo::VersionDrivenFindBehavior::kMinimumMinorVersion;
                } else {
                    ret = -__LINE__;
                    ComLogError("load node invalid", GenArg(ret), GenArg(key3), GenArg(stVersionDrivenFindBehavior));
                    return;
                }
            } else if (key3 == "thread_pool_size") {
                auto& threadPoolSize{rServiceInfo->threadPoolSize};
                if ((ret = node3.Load("", threadPoolSize)) != isoft::kSuccess) {
                    ComLogError("load node failed", GenArg(ret), GenArg(key3));
                    return;
                }
            }
        });
        if (ret != isoft::kSuccess) {
            return ret;
        }
        // [TODO] Check impact on platform startup, enable after upgrader implementation
        if (false && !(hasServiceId && hasVersion && hasServiceFindTimeToLive)) {
            ret = -__LINE__;
            ComLogError("load node missed", GenArg(ret), GenArg(hasServiceId), GenArg(hasVersion),
                        GenArg(hasServiceFindTimeToLive));
            return ret;
        }
#endif
#if 1  // Load RGroupInfo
        std::ignore = node2.IterateArray("event_groups", [&](auto idx3In, auto& node3) {
            auto idx3{static_cast< int64_t >(idx3In)};
            if (ret != isoft::kSuccess) {
                return;
            }
            ret = this->LoadRGroupInfo(*rServiceInfo, node3);
            if (ret != isoft::kSuccess) {
                ComLogError("load node failed", GenArg(ret), GenArg(idx3));
                return;
            }
        });
        if (ret != isoft::kSuccess) {
            return ret;
        }
#endif
#if 1  // Load REventInfo
        std::ignore = node2.IterateArray("events", [&](auto idx3In, auto& node3) {
            auto idx3{static_cast< int64_t >(idx3In)};
            if (ret != isoft::kSuccess) {
                return;
            }
            ret = this->LoadREventInfo(*rServiceInfo, node3);
            if (ret != isoft::kSuccess) {
                ComLogError("load node failed", GenArg(ret), GenArg(idx3));
                return;
            }
        });
        if (ret != isoft::kSuccess) {
            return ret;
        }
#endif
#if 1  // Load RMethodInfo
        std::ignore = node2.IterateArray("methods", [&](auto idx3In, auto& node3) {
            auto idx3{static_cast< int64_t >(idx3In)};
            if (ret != isoft::kSuccess) {
                return;
            }
            ret = this->LoadRMethodInfo(*rServiceInfo, node3);
            if (ret != isoft::kSuccess) {
                ComLogError("load node failed", GenArg(ret), GenArg(idx3));
                return;
            }
        });
        if (ret != isoft::kSuccess) {
            return ret;
        }
#endif
#if 1  // Load RFieldInfo
        std::ignore = node2.IterateArray("fields", [&](auto idx3In, auto& node3) {
            auto idx3{static_cast< int64_t >(idx3In)};
            if (ret != isoft::kSuccess) {
                return;
            }
            ret = this->LoadRFieldInfo(*rServiceInfo, node3);
            if (ret != isoft::kSuccess) {
                ComLogError("load node failed", GenArg(ret), GenArg(idx3));
                return;
            }
        });
        if (ret != isoft::kSuccess) {
            return ret;
        }
#endif
        auto& serviceInfo{rServiceInfos_[rServiceInfo->ID()]};
        if (serviceInfo != nullptr) {
            ret = -__LINE__;
            ComLogError("load node repeat", GenArg(ret), GenArg(rServiceInfo->ID()));
            return ret;
        }
        serviceInfo = std::move(rServiceInfo);
        return ret;
    }
    /// @brief Load GroupInfo -- skeleton side
    /// @return Result -- 0: success; !0: failure;
    int32_t LoadPGroupInfo(PServiceInfo& pServiceInfo, isoft::manifestreader::ManifestNode const& node3) noexcept
    {
        ComLogTrace("load bind provided group info", GenArg(pServiceInfo));
        int32_t ret{};
        auto pGroupInfo{std::make_unique< PGroupInfo >(pServiceInfo)};
        bool hasGroup{false};
        bool hasMulticastThreshold{false};
        std::ignore = node3.IterateObject("", [&](auto& key4, auto& node4) {
            if (ret != isoft::kSuccess) {
                return;
            }
            if (key4 == "event_group_id") {
                auto& groupId{pGroupInfo->groupId};
                if ((ret = node4.Load("", groupId)) != isoft::kSuccess) {
                    ComLogError("load node failed", GenArg(ret), GenArg(key4));
                    return;
                }
                hasGroup = true;
            } else if (key4 == "events") {
                ara::core::Vector< decltype(EventInfo::eventId) > eventIds{};
                if ((ret = node4.Load("", eventIds)) != isoft::kSuccess) {
                    ComLogError("load node failed", GenArg(ret), GenArg(key4));
                    return;
                }
                for (auto it{eventIds.begin()}; it != eventIds.end(); ++it) {
                    auto& eventId{*it};
                    pGroupInfo->eventInfos.emplace(eventId, nullptr);
                }
            } else if (key4 == "multicast_threshold") {
                auto& multicastThreshold{pGroupInfo->multicastThreshold};
                if ((ret = node4.Load("", multicastThreshold)) != isoft::kSuccess) {
                    ComLogError("load node failed", GenArg(ret), GenArg(key4));
                    return;
                }
                hasMulticastThreshold = true;
            } else if (key4 == "sd_server_event_group_timing_config") {
                auto& sdServerEventGroupTimingConfig{pGroupInfo->sdServerEventGroupTimingConfig};
                std::ignore = node4.IterateObject("", [&](auto& key5, auto& node5) {
                    if (ret != isoft::kSuccess) {
                        return;
                    }
                    if (key5 == "request_response_delay") {
                        auto& requestResponseDelay{sdServerEventGroupTimingConfig.requestResponseDelay};
                        auto& maxValue{requestResponseDelay.maxValue};
                        auto& minValue{requestResponseDelay.minValue};
                        if ((ret = node5.Load("max_value", maxValue)) != isoft::kSuccess) {
                            ComLogError("load node failed", GenArg(ret), GenArg(key4), GenArg(key5),
                                        GenVal("max_value"));
                            return;
                        }
                        if ((ret = node5.Load("min_value", minValue)) != isoft::kSuccess) {
                            ComLogError("load node failed", GenArg(ret), GenArg(key4), GenArg(key5),
                                        GenVal("min_value"));
                            return;
                        }
                    }
                });
            }
        });
        if (ret != isoft::kSuccess) {
            return ret;
        }
        // [TODO] Check impact on platform startup, enable after upgrader implementation
        if (false && !(hasGroup && hasMulticastThreshold)) {
            ret = -__LINE__;
            ComLogError("load node missed", GenArg(ret), GenArg(hasGroup), GenArg(hasMulticastThreshold));
            return ret;
        }
        auto& groupInfo{pServiceInfo.groupInfos[pGroupInfo->groupId]};
        if (groupInfo != nullptr) {
            ret = -__LINE__;
            ComLogError("load node repeat", GenArg(ret), GenArg(pGroupInfo->groupId));
            return ret;
        }
        groupInfo = std::move(pGroupInfo);
        return ret;
    }
    /// @brief Load GroupInfo -- proxy side
    /// @return Result -- 0: success; !0: failure;
    int32_t LoadRGroupInfo(RServiceInfo& rServiceInfo, isoft::manifestreader::ManifestNode const& node3) noexcept
    {
        ComLogTrace("load bind required group info", GenArg(rServiceInfo));
        int32_t ret{};
        auto rGroupInfo{std::make_unique< RGroupInfo >(rServiceInfo)};
        bool hasGroup{false};
        bool hasTimeToLive{false};
        std::ignore = node3.IterateObject("", [&](auto& key4, auto& node4) {
            if (ret != isoft::kSuccess) {
                return;
            }
            if (key4 == "event_group_id") {
                auto& groupId{rGroupInfo->groupId};
                if ((ret = node4.Load("", groupId)) != isoft::kSuccess) {
                    ComLogError("load node failed", GenArg(ret), GenArg(key4));
                    return;
                }
                hasGroup = true;
            } else if (key4 == "events") {
                ara::core::Vector< decltype(EventInfo::eventId) > eventIds{};
                if ((ret = node4.Load("", eventIds)) != isoft::kSuccess) {
                    ComLogError("load node failed", GenArg(ret), GenArg(key4));
                    return;
                }
                for (auto it{eventIds.begin()}; it != eventIds.end(); ++it) {
                    auto& eventId{*it};
                    rGroupInfo->eventInfos.emplace(eventId, nullptr);
                }
            } else if (key4 == "sd_client_event_group_timing_config") {
                auto& sdClientEventGroupTimingConfig{rGroupInfo->sdClientEventGroupTimingConfig};
                std::ignore = node4.IterateObject("", [&](auto& key5, auto& node5) {
                    if (ret != isoft::kSuccess) {
                        return;
                    }
                    if (key5 == "request_response_delay") {
                        auto& requestResponseDelay{sdClientEventGroupTimingConfig.requestResponseDelay};
                        auto& maxValue{requestResponseDelay.maxValue};
                        auto& minValue{requestResponseDelay.minValue};
                        if ((ret = node5.Load("max_value", maxValue)) != isoft::kSuccess) {
                            ComLogError("load node failed", GenArg(ret), GenArg(key4), GenArg(key5),
                                        GenVal("max_value"));
                            return;
                        }
                        if ((ret = node5.Load("min_value", minValue)) != isoft::kSuccess) {
                            ComLogError("load node failed", GenArg(ret), GenArg(key4), GenArg(key5),
                                        GenVal("min_value"));
                            return;
                        }
                    } else if (key5 == "subscribe_event_group_retry_delay") {
                        auto& subscribeEventGroupRetryDelay{
                            sdClientEventGroupTimingConfig.subscribeEventGroupRetryDelay};
                        if ((ret = node5.Load("", subscribeEventGroupRetryDelay)) != isoft::kSuccess) {
                            ComLogError("load node failed", GenArg(ret), GenArg(key4), GenArg(key5));
                            return;
                        }
                    } else if (key5 == "subscribe_event_group_retry_max") {
                        auto& subscribeEventGroupRetryMax{sdClientEventGroupTimingConfig.subscribeEventGroupRetryMax};
                        if ((ret = node5.Load("", subscribeEventGroupRetryMax)) != isoft::kSuccess) {
                            ComLogError("load node failed", GenArg(ret), GenArg(key4), GenArg(key5));
                            return;
                        }
                    } else if (key5 == "time_to_live") {
                        auto& timeToLive{sdClientEventGroupTimingConfig.timeToLive};
                        if ((ret = node5.Load("", timeToLive)) != isoft::kSuccess) {
                            ComLogError("load node failed", GenArg(ret), GenArg(key4), GenArg(key5));
                            return;
                        }
                        hasTimeToLive = true;
                    }
                });
            }
        });
        if (ret != isoft::kSuccess) {
            return ret;
        }
        // [TODO] Check impact on platform startup, enable after upgrader implementation
        if (false && !(hasGroup && hasTimeToLive)) {
            ret = -__LINE__;
            ComLogError("load node missed", GenArg(ret), GenArg(hasGroup), GenArg(hasTimeToLive));
            return ret;
        }
        auto& groupInfo{rServiceInfo.groupInfos[rGroupInfo->groupId]};
        if (groupInfo != nullptr) {
            ret = -__LINE__;
            ComLogError("load node repeat", GenArg(ret), GenArg(rGroupInfo->groupId));
            return ret;
        }
        groupInfo = std::move(rGroupInfo);
        return ret;
    }
    /// @brief Load EventInfo -- skeleton side
    /// @return Result -- 0: success; !0: failure;
    int32_t LoadPEventInfo(PServiceInfo& pServiceInfo, isoft::manifestreader::ManifestNode const& node3) noexcept
    {
        ComLogTrace("load bind provided event info", GenArg(pServiceInfo));
        int32_t ret{};
        auto pEventInfo{std::make_unique< PEventInfo >(pServiceInfo)};
        bool hasEvent{false};
        bool hasTransportProtocol{false};
        std::ignore = node3.IterateObject("", [&](auto& key4, auto& node4) {
            if (ret != isoft::kSuccess) {
                return;
            }
            if (key4 == "event_id") {
                auto& eventId{pEventInfo->eventId};
                if ((ret = node4.Load("", eventId)) != isoft::kSuccess) {
                    ComLogError("load node failed", GenArg(ret), GenArg(key4));
                    return;
                }
                if (!Message::IsEventId(eventId)) {
                    ret = -__LINE__;
                    ComLogError("load node invalid", GenArg(ret), GenArg(key4), GenArg(eventId));
                    return;
                }
                for (auto it{pServiceInfo.groupInfos.begin()}; it != pServiceInfo.groupInfos.end(); ++it) {
                    auto& itPGroupInfo{*it};
                    auto const& groupId{itPGroupInfo.first};
                    auto& pGroupInfo{itPGroupInfo.second};
                    auto& pEventInfos{pGroupInfo->eventInfos};
                    auto itPEventInfo{pEventInfos.find(eventId)};
                    if (itPEventInfo == pEventInfos.end()) {
                        continue;
                    }
                    auto& eventInfo{itPEventInfo->second};
                    if (eventInfo != nullptr) {
                        ret = -__LINE__;
                        ComLogError("load node repeat", GenArg(ret), GenArg(key4), GenArg(eventId));
                        return;
                    }
                    eventInfo = pEventInfo.get();
                    auto& groupInfo{pEventInfo->groupInfos[groupId]};
                    if (groupInfo != nullptr) {
                        ret = -__LINE__;
                        ComLogError("load node repeat", GenArg(ret), GenArg(key4), GenArg(eventId), GenArg(groupId));
                        return;
                    }
                    groupInfo = pGroupInfo.get();
                }
                if (pEventInfo->groupInfos.empty()) {
                    ret = -__LINE__;
                    ComLogError("load node missed", GenArg(ret), GenArg(key4), GenArg(eventId));
                    return;
                }
                hasEvent = true;
            } else if (key4 == "maximum_segment_length") {
                auto& maximumSegmentLength{pEventInfo->maximumSegmentLength};
                if ((ret = node4.Load("", maximumSegmentLength)) != isoft::kSuccess) {
                    ComLogError("load node failed", GenArg(ret), GenArg(key4));
                    return;
                }
            } else if (key4 == "separation_time") {
                auto& separationTime{pEventInfo->separationTime};
                if ((ret = node4.Load("", separationTime)) != isoft::kSuccess) {
                    ComLogError("load node failed", GenArg(ret), GenArg(key4));
                    return;
                }
            } else if (key4 == "serializer") {
                auto& serializer{pEventInfo->serializer};
                ara::core::String stSerializer{};
                if ((ret = node4.Load("", stSerializer)) != isoft::kSuccess) {
                    ComLogError("load node failed", GenArg(ret), GenArg(key4));
                    return;
                }
                if (stSerializer.empty() || stSerializer == "someip") {
                    serializer = EventInfo::Serializer::kSomeip;
                } else if (stSerializer == "signalBased") {
                    serializer = EventInfo::Serializer::kSignalBased;
                } else {
                    ret = -__LINE__;
                    ComLogError("load node invalid", GenArg(ret), GenArg(key4), GenArg(stSerializer));
                    return;
                }
            } else if (key4 == "transport_protocol") {
                auto& transportProtocol{pEventInfo->transportProtocol};
                ara::core::String stTransportProtocol{};
                if ((ret = node4.Load("", stTransportProtocol)) != isoft::kSuccess) {
                    ComLogError("load node failed", GenArg(ret), GenArg(key4));
                    return;
                }
                if (stTransportProtocol == "TCP") {
                    transportProtocol = Message::kTransportProtocolTCP;
                } else if (stTransportProtocol == "UDP") {
                    transportProtocol = Message::kTransportProtocolUDP;
                } else if (stTransportProtocol == "Both") {
                    transportProtocol = Message::kTransportProtocolBoth;
                } else {
                    ret = -__LINE__;
                    ComLogError("load node invalid", GenArg(ret), GenArg(key4), GenArg(stTransportProtocol));
                    return;
                }
                hasTransportProtocol = true;
            } else if (key4 == "collection_props") {
                auto& collectionProps{pEventInfo->collectionProps};
                std::ignore = node4.IterateObject("", [&](auto& key5, auto& node5) {
                    if (ret != isoft::kSuccess) {
                        return;
                    }
                    if (key5 == "udp_collection_buffer_timeout") {
                        auto& udpCollectionBufferTimeout{collectionProps.udpCollectionBufferTimeout};
                        if ((ret = node5.Load("", udpCollectionBufferTimeout)) != isoft::kSuccess) {
                            ComLogError("load node failed", GenArg(ret), GenArg(key4), GenArg(key5));
                            return;
                        }
                    } else if (key5 == "udp_collection_trigger") {
                        auto& udpCollectionTrigger{collectionProps.udpCollectionTrigger};
                        ara::core::String stUdpCollectionTrigger{};
                        if ((ret = node5.Load("", stUdpCollectionTrigger)) != isoft::kSuccess) {
                            ComLogError("load node failed", GenArg(ret), GenArg(key4), GenArg(key5));
                            return;
                        }
                        if (stUdpCollectionTrigger.empty() || stUdpCollectionTrigger == "never") {
                            udpCollectionTrigger = PEventInfo::CollectionProps::UdpCollectionTrigger::kNever;
                        } else if (stUdpCollectionTrigger == "always") {
                            udpCollectionTrigger = PEventInfo::CollectionProps::UdpCollectionTrigger::kAlways;
                        } else {
                            ret = -__LINE__;
                            ComLogError("load node invalid", GenArg(ret), GenArg(key4), GenArg(key5),
                                        GenArg(stUdpCollectionTrigger));
                            return;
                        }
                    }
                });
            }
        });
        if (ret != isoft::kSuccess) {
            return ret;
        }
        // [TODO] Check impact on platform startup, enable after upgrader implementation
        if (false && !(hasEvent && hasTransportProtocol)) {
            ret = -__LINE__;
            ComLogError("load node missed", GenArg(ret), GenArg(hasEvent), GenArg(hasTransportProtocol));
            return ret;
        }
        auto& eventInfo{pServiceInfo.eventInfos[pEventInfo->eventId]};
        if (eventInfo != nullptr) {
            ret = -__LINE__;
            ComLogError("load node repeat", GenArg(ret), GenArg(pEventInfo->eventId));
            return ret;
        }
        eventInfo = std::move(pEventInfo);
        return ret;
    }
    /// @brief Load EventInfo -- proxy side
    /// @return Result -- 0: success; !0: failure;
    int32_t LoadREventInfo(RServiceInfo& rServiceInfo, isoft::manifestreader::ManifestNode const& node3) noexcept
    {
        ComLogTrace("load bind required event info", GenArg(rServiceInfo));
        int32_t ret{};
        auto rEventInfo{std::make_unique< REventInfo >(rServiceInfo)};
        bool hasEvent{false};
        bool hasTransportProtocol{false};
        std::ignore = node3.IterateObject("", [&](auto& key4, auto& node4) {
            if (ret != isoft::kSuccess) {
                return;
            }
            if (key4 == "event_id") {
                auto& eventId{rEventInfo->eventId};
                if ((ret = node4.Load("", eventId)) != isoft::kSuccess) {
                    ComLogError("load node failed", GenArg(ret), GenArg(key4));
                    return;
                }
                if (!Message::IsEventId(eventId)) {
                    ret = -__LINE__;
                    ComLogError("load node invalid", GenArg(ret), GenArg(key4), GenArg(eventId));
                    return;
                }
                for (auto it{rServiceInfo.groupInfos.begin()}; it != rServiceInfo.groupInfos.end(); ++it) {
                    auto& itRGroupInfo{*it};
                    auto const& groupId{itRGroupInfo.first};
                    auto& rGroupInfo{itRGroupInfo.second};
                    auto& rEventInfos{rGroupInfo->eventInfos};
                    auto itREventInfo{rEventInfos.find(eventId)};
                    if (itREventInfo == rEventInfos.end()) {
                        continue;
                    }
                    auto& eventInfo{itREventInfo->second};
                    if (eventInfo != nullptr) {
                        ret = -__LINE__;
                        ComLogError("load node repeat", GenArg(ret), GenArg(key4), GenArg(eventId));
                        return;
                    }
                    eventInfo = rEventInfo.get();
                    auto& groupInfo{rEventInfo->groupInfos[groupId]};
                    if (groupInfo != nullptr) {
                        ret = -__LINE__;
                        ComLogError("load node repeat", GenArg(ret), GenArg(key4), GenArg(eventId), GenArg(groupId));
                        return;
                    }
                    groupInfo = rGroupInfo.get();
                }
                if (rEventInfo->groupInfos.empty()) {
                    ret = -__LINE__;
                    ComLogError("load node missed", GenArg(ret), GenArg(key4), GenArg(eventId));
                    return;
                }
                hasEvent = true;
            } else if (key4 == "maximum_segment_length") {
                auto& maximumSegmentLength{rEventInfo->maximumSegmentLength};
                if ((ret = node4.Load("", maximumSegmentLength)) != isoft::kSuccess) {
                    ComLogError("load node failed", GenArg(ret), GenArg(key4));
                    return;
                }
            } else if (key4 == "separation_time") {
                auto& separationTime{rEventInfo->separationTime};
                if ((ret = node4.Load("", separationTime)) != isoft::kSuccess) {
                    ComLogError("load node failed", GenArg(ret), GenArg(key4));
                    return;
                }
            } else if (key4 == "serializer") {
                auto& serializer{rEventInfo->serializer};
                ara::core::String stSerializer{};
                if ((ret = node4.Load("", stSerializer)) != isoft::kSuccess) {
                    ComLogError("load node failed", GenArg(ret), GenArg(key4));
                    return;
                }
                if (stSerializer.empty() || stSerializer == "someip") {
                    serializer = EventInfo::Serializer::kSomeip;
                } else if (stSerializer == "signalBased") {
                    serializer = EventInfo::Serializer::kSignalBased;
                } else {
                    ret = -__LINE__;
                    ComLogError("load node invalid", GenArg(ret), GenArg(key4), GenArg(stSerializer));
                    return;
                }
            } else if (key4 == "transport_protocol") {
                auto& transportProtocol{rEventInfo->transportProtocol};
                ara::core::String stTransportProtocol{};
                if ((ret = node4.Load("", stTransportProtocol)) != isoft::kSuccess) {
                    ComLogError("load node failed", GenArg(ret), GenArg(key4));
                    return;
                }
                if (stTransportProtocol == "TCP") {
                    transportProtocol = Message::kTransportProtocolTCP;
                } else if (stTransportProtocol == "UDP") {
                    transportProtocol = Message::kTransportProtocolUDP;
                } else if (stTransportProtocol == "Both") {
                    transportProtocol = Message::kTransportProtocolBoth;
                } else {
                    ret = -__LINE__;
                    ComLogError("load node invalid", GenArg(ret), GenArg(key4), GenArg(stTransportProtocol));
                    return;
                }
                hasTransportProtocol = true;
            }
        });
        if (ret != isoft::kSuccess) {
            return ret;
        }
        // [TODO] Check impact on platform startup, enable after upgrader implementation
        if (false && !(hasEvent && hasTransportProtocol)) {
            ret = -__LINE__;
            ComLogError("load node missed", GenArg(ret), GenArg(hasEvent), GenArg(hasTransportProtocol));
            return ret;
        }
        auto& eventInfo{rServiceInfo.eventInfos[rEventInfo->eventId]};
        if (eventInfo != nullptr) {
            ret = -__LINE__;
            ComLogError("load node repeat", GenArg(ret), GenArg(rEventInfo->eventId));
            return ret;
        }
        eventInfo = std::move(rEventInfo);
        return ret;
    }
    /// @brief Load MethodInfo -- skeleton side
    /// @return Result -- 0: success; !0: failure;
    int32_t LoadPMethodInfo(PServiceInfo& pServiceInfo, isoft::manifestreader::ManifestNode const& node3) noexcept
    {
        ComLogTrace("load bind provided method info", GenArg(pServiceInfo));
        int32_t ret{};
        auto pMethodInfo{std::make_unique< PMethodInfo >(pServiceInfo)};
        bool hasMethod{false};
        bool hasTransportProtocol{false};
        std::ignore = node3.IterateObject("", [&](auto& key4, auto& node4) {
            if (ret != isoft::kSuccess) {
                return;
            }
            if (key4 == "method_id") {
                auto& methodId{pMethodInfo->methodId};
                if ((ret = node4.Load("", methodId)) != isoft::kSuccess) {
                    ComLogError("load node failed", GenArg(ret), GenArg(key4));
                    return;
                }
                if (!Message::IsMethodId(methodId)) {
                    ret = -__LINE__;
                    ComLogError("load node invalid", GenArg(ret), GenArg(key4), GenArg(methodId));
                    return;
                }
                hasMethod = true;
            } else if (key4 == "maximum_segment_length_request") {
                auto& maximumSegmentLengthRequest{pMethodInfo->maximumSegmentLengthRequest};
                if ((ret = node4.Load("", maximumSegmentLengthRequest)) != isoft::kSuccess) {
                    ComLogError("load node failed", GenArg(ret), GenArg(key4));
                    return;
                }
            } else if (key4 == "maximum_segment_length_response") {
                auto& maximumSegmentLengthResponse{pMethodInfo->maximumSegmentLengthResponse};
                if ((ret = node4.Load("", maximumSegmentLengthResponse)) != isoft::kSuccess) {
                    ComLogError("load node failed", GenArg(ret), GenArg(key4));
                    return;
                }
            } else if (key4 == "separation_time_request") {
                auto& separationTimeRequest{pMethodInfo->separationTimeRequest};
                if ((ret = node4.Load("", separationTimeRequest)) != isoft::kSuccess) {
                    ComLogError("load node failed", GenArg(ret), GenArg(key4));
                    return;
                }
            } else if (key4 == "separation_time_response") {
                auto& separationTimeResponse{pMethodInfo->separationTimeResponse};
                if ((ret = node4.Load("", separationTimeResponse)) != isoft::kSuccess) {
                    ComLogError("load node failed", GenArg(ret), GenArg(key4));
                    return;
                }
            } else if (key4 == "transport_protocol") {
                auto& transportProtocol{pMethodInfo->transportProtocol};
                ara::core::String stTransportProtocol{};
                if ((ret = node4.Load("", stTransportProtocol)) != isoft::kSuccess) {
                    ComLogError("load node failed", GenArg(ret), GenArg(key4));
                    return;
                }
                if (stTransportProtocol == "TCP") {
                    transportProtocol = Message::kTransportProtocolTCP;
                } else if (stTransportProtocol == "UDP") {
                    transportProtocol = Message::kTransportProtocolUDP;
                } else if (stTransportProtocol == "Both") {
                    transportProtocol = Message::kTransportProtocolBoth;
                } else {
                    ret = -__LINE__;
                    ComLogError("load node invalid", GenArg(ret), GenArg(key4), GenArg(stTransportProtocol));
                    return;
                }
                hasTransportProtocol = true;
            } else if (key4 == "collection_props") {
                auto& collectionProps{pMethodInfo->collectionProps};
                std::ignore = node4.IterateObject("", [&](auto& key5, auto& node5) {
                    if (ret != isoft::kSuccess) {
                        return;
                    }
                    if (key5 == "udp_collection_buffer_timeout") {
                        auto& udpCollectionBufferTimeout{collectionProps.udpCollectionBufferTimeout};
                        if ((ret = node5.Load("", udpCollectionBufferTimeout)) != isoft::kSuccess) {
                            ComLogError("load node failed", GenArg(ret), GenArg(key4), GenArg(key5));
                            return;
                        }
                    } else if (key5 == "udp_collection_trigger") {
                        auto& udpCollectionTrigger{collectionProps.udpCollectionTrigger};
                        ara::core::String stUdpCollectionTrigger{};
                        if ((ret = node5.Load("", stUdpCollectionTrigger)) != isoft::kSuccess) {
                            ComLogError("load node failed", GenArg(ret), GenArg(key4), GenArg(key5));
                            return;
                        }
                        if (stUdpCollectionTrigger.empty() || stUdpCollectionTrigger == "never") {
                            udpCollectionTrigger = PMethodInfo::CollectionProps::UdpCollectionTrigger::kNever;
                        } else if (stUdpCollectionTrigger == "always") {
                            udpCollectionTrigger = PMethodInfo::CollectionProps::UdpCollectionTrigger::kAlways;
                        } else {
                            ret = -__LINE__;
                            ComLogError("load node invalid", GenArg(ret), GenArg(key4), GenArg(key5),
                                        GenArg(stUdpCollectionTrigger));
                            return;
                        }
                    }
                });
            }
        });
        if (ret != isoft::kSuccess) {
            return ret;
        }
        // [TODO] Check impact on platform startup, enable after upgrader implementation
        if (false && !(hasMethod && hasTransportProtocol)) {
            ret = -__LINE__;
            ComLogError("load node missed", GenArg(ret), GenArg(hasMethod), GenArg(hasTransportProtocol));
            return ret;
        }
        auto& methodInfo{pServiceInfo.methodInfos[pMethodInfo->methodId]};
        if (methodInfo != nullptr) {
            ret = -__LINE__;
            ComLogError("load node repeat", GenArg(ret), GenArg(pMethodInfo->methodId));
            return ret;
        }
        methodInfo = std::move(pMethodInfo);
        return ret;
    }
    /// @brief Load MethodInfo -- proxy side
    /// @return Result -- 0: success; !0: failure;
    int32_t LoadRMethodInfo(RServiceInfo& rServiceInfo, isoft::manifestreader::ManifestNode const& node3) noexcept
    {
        ComLogTrace("load bind required method info", GenArg(rServiceInfo));
        int32_t ret{};
        auto rMethodInfo{std::make_unique< RMethodInfo >(rServiceInfo)};
        bool hasMethod{false};
        bool hasTransportProtocol{false};
        std::ignore = node3.IterateObject("", [&](auto& key4, auto& node4) {
            if (ret != isoft::kSuccess) {
                return;
            }
            if (key4 == "method_id") {
                auto& methodId{rMethodInfo->methodId};
                if ((ret = node4.Load("", methodId)) != isoft::kSuccess) {
                    ComLogError("load node failed", GenArg(ret), GenArg(key4));
                    return;
                }
                if (!Message::IsMethodId(methodId)) {
                    ret = -__LINE__;
                    ComLogError("load node invalid", GenArg(ret), GenArg(key4), GenArg(methodId));
                    return;
                }
                hasMethod = true;
            } else if (key4 == "maximum_segment_length_request") {
                auto& maximumSegmentLengthRequest{rMethodInfo->maximumSegmentLengthRequest};
                if ((ret = node4.Load("", maximumSegmentLengthRequest)) != isoft::kSuccess) {
                    ComLogError("load node failed", GenArg(ret), GenArg(key4));
                    return;
                }
            } else if (key4 == "maximum_segment_length_response") {
                auto& maximumSegmentLengthResponse{rMethodInfo->maximumSegmentLengthResponse};
                if ((ret = node4.Load("", maximumSegmentLengthResponse)) != isoft::kSuccess) {
                    ComLogError("load node failed", GenArg(ret), GenArg(key4));
                    return;
                }
            } else if (key4 == "separation_time_request") {
                auto& separationTimeRequest{rMethodInfo->separationTimeRequest};
                if ((ret = node4.Load("", separationTimeRequest)) != isoft::kSuccess) {
                    ComLogError("load node failed", GenArg(ret), GenArg(key4));
                    return;
                }
            } else if (key4 == "separation_time_response") {
                auto& separationTimeResponse{rMethodInfo->separationTimeResponse};
                if ((ret = node4.Load("", separationTimeResponse)) != isoft::kSuccess) {
                    ComLogError("load node failed", GenArg(ret), GenArg(key4));
                    return;
                }
            } else if (key4 == "transport_protocol") {
                auto& transportProtocol{rMethodInfo->transportProtocol};
                ara::core::String stTransportProtocol{};
                if ((ret = node4.Load("", stTransportProtocol)) != isoft::kSuccess) {
                    ComLogError("load node failed", GenArg(ret), GenArg(key4));
                    return;
                }
                if (stTransportProtocol == "TCP") {
                    transportProtocol = Message::kTransportProtocolTCP;
                } else if (stTransportProtocol == "UDP") {
                    transportProtocol = Message::kTransportProtocolUDP;
                } else if (stTransportProtocol == "Both") {
                    transportProtocol = Message::kTransportProtocolBoth;
                } else {
                    ret = -__LINE__;
                    ComLogError("load node invalid", GenArg(ret), GenArg(key4), GenArg(stTransportProtocol));
                    return;
                }
                hasTransportProtocol = true;
            } else if (key4 == "collection_props") {
                auto& collectionProps{rMethodInfo->collectionProps};
                std::ignore = node4.IterateObject("", [&](auto& key5, auto& node5) {
                    if (ret != isoft::kSuccess) {
                        return;
                    }
                    if (key5 == "udp_collection_buffer_timeout") {
                        auto& udpCollectionBufferTimeout{collectionProps.udpCollectionBufferTimeout};
                        if ((ret = node5.Load("", udpCollectionBufferTimeout)) != isoft::kSuccess) {
                            ComLogError("load node failed", GenArg(ret), GenArg(key4), GenArg(key5));
                            return;
                        }
                    } else if (key5 == "udp_collection_trigger") {
                        auto& udpCollectionTrigger{collectionProps.udpCollectionTrigger};
                        ara::core::String stUdpCollectionTrigger{};
                        if ((ret = node5.Load("", stUdpCollectionTrigger)) != isoft::kSuccess) {
                            ComLogError("load node failed", GenArg(ret), GenArg(key4), GenArg(key5));
                            return;
                        }
                        if (stUdpCollectionTrigger.empty() || stUdpCollectionTrigger == "never") {
                            udpCollectionTrigger = RMethodInfo::CollectionProps::UdpCollectionTrigger::kNever;
                        } else if (stUdpCollectionTrigger == "always") {
                            udpCollectionTrigger = RMethodInfo::CollectionProps::UdpCollectionTrigger::kAlways;
                        } else {
                            ret = -__LINE__;
                            ComLogError("load node invalid", GenArg(ret), GenArg(key4), GenArg(key5),
                                        GenArg(stUdpCollectionTrigger));
                            return;
                        }
                    }
                });
            }
        });
        if (ret != isoft::kSuccess) {
            return ret;
        }
        // [TODO] Check impact on platform startup, enable after upgrader implementation
        if (false && !(hasMethod && hasTransportProtocol)) {
            ret = -__LINE__;
            ComLogError("load node missed", GenArg(ret), GenArg(hasMethod), GenArg(hasTransportProtocol));
            return ret;
        }
        auto& methodInfo{rServiceInfo.methodInfos[rMethodInfo->methodId]};
        if (methodInfo != nullptr) {
            ret = -__LINE__;
            ComLogError("load node repeat", GenArg(ret), GenArg(rMethodInfo->methodId));
            return ret;
        }
        methodInfo = std::move(rMethodInfo);
        return ret;
    }
    /// @brief Load FieldInfo -- skeleton side
    /// @return Result -- 0: success; !0: failure;
    int32_t LoadPFieldInfo(PServiceInfo& pServiceInfo, isoft::manifestreader::ManifestNode const& node3) noexcept
    {
        ComLogTrace("load bind provided field info", GenArg(pServiceInfo));
        int32_t ret{};
        auto pFieldInfo{std::make_unique< PFieldInfo >(pServiceInfo)};
        decltype(EventInfo::eventId) notifier{};
        decltype(MethodInfo::methodId) setter{};
        decltype(MethodInfo::methodId) getter{};
        if (node3.Load("notifier", notifier) == isoft::kSuccess) {
            auto itPEventInfo{pServiceInfo.eventInfos.find(notifier)};
            if (itPEventInfo == pServiceInfo.eventInfos.end()) {
                ret = -__LINE__;
                ComLogError("load node missed", GenArg(ret));
                return ret;
            }
            auto& pEventInfo{itPEventInfo->second};
            if (pEventInfo->fieldInfo != nullptr) {
                ret = -__LINE__;
                ComLogError("load node repeat", GenArg(ret), GenArg(pEventInfo->eventId));
                return ret;
            }
            pEventInfo->fieldInfo    = pFieldInfo.get();
            pFieldInfo->notifierInfo = pEventInfo.get();
        }
        if (node3.Load("setter", setter) == isoft::kSuccess) {
            auto itPMethodInfo{pServiceInfo.methodInfos.find(setter)};
            if (itPMethodInfo == pServiceInfo.methodInfos.end()) {
                ret = -__LINE__;
                ComLogError("load node missed", GenArg(ret));
                return ret;
            }
            auto& pMethodInfo{itPMethodInfo->second};
            if (pMethodInfo->fieldInfo != nullptr) {
                ret = -__LINE__;
                ComLogError("load node repeat", GenArg(ret), GenArg(pMethodInfo->methodId));
                return ret;
            }
            pMethodInfo->fieldInfo = pFieldInfo.get();
            pFieldInfo->setterInfo = pMethodInfo.get();
        }
        if (node3.Load("getter", getter) == isoft::kSuccess) {
            auto itPMethodInfo{pServiceInfo.methodInfos.find(getter)};
            if (itPMethodInfo == pServiceInfo.methodInfos.end()) {
                ret = -__LINE__;
                ComLogError("load node missed", GenArg(ret));
                return ret;
            }
            auto& pMethodInfo{itPMethodInfo->second};
            if (pMethodInfo->fieldInfo != nullptr) {
                ret = -__LINE__;
                ComLogError("load node repeat", GenArg(ret), GenArg(pMethodInfo->methodId));
                return ret;
            }
            pMethodInfo->fieldInfo = pFieldInfo.get();
            pFieldInfo->getterInfo = pMethodInfo.get();
        }
        if (pFieldInfo->notifierInfo == nullptr && pFieldInfo->setterInfo == nullptr
            && pFieldInfo->getterInfo == nullptr) {
            ret = -__LINE__;
            ComLogError("load node missed", GenArg(ret));
            return ret;
        }
        pServiceInfo.fieldInfos.emplace(std::move(pFieldInfo));
        return ret;
    }
    /// @brief Load FieldInfo -- proxy side
    /// @return Result -- 0: success; !0: failure;
    int32_t LoadRFieldInfo(RServiceInfo& rServiceInfo, isoft::manifestreader::ManifestNode const& node3) noexcept
    {
        ComLogTrace("load bind required field info", GenArg(rServiceInfo));
        int32_t ret{};
        auto rFieldInfo{std::make_unique< RFieldInfo >(rServiceInfo)};
        decltype(EventInfo::eventId) notifier{};
        decltype(MethodInfo::methodId) setter{};
        decltype(MethodInfo::methodId) getter{};
        if (node3.Load("notifier", notifier) == isoft::kSuccess) {
            auto itREventInfo{rServiceInfo.eventInfos.find(notifier)};
            if (itREventInfo == rServiceInfo.eventInfos.end()) {
                ret = -__LINE__;
                ComLogError("load node missed", GenArg(ret));
                return ret;
            }
            auto& rEventInfo{itREventInfo->second};
            if (rEventInfo->fieldInfo != nullptr) {
                ret = -__LINE__;
                ComLogError("load node repeat", GenArg(ret), GenArg(rEventInfo->eventId));
                return ret;
            }
            rEventInfo->fieldInfo    = rFieldInfo.get();
            rFieldInfo->notifierInfo = rEventInfo.get();
        }
        if (node3.Load("setter", setter) == isoft::kSuccess) {
            auto itRMethodInfo{rServiceInfo.methodInfos.find(setter)};
            if (itRMethodInfo == rServiceInfo.methodInfos.end()) {
                ret = -__LINE__;
                ComLogError("load node missed", GenArg(ret));
                return ret;
            }
            auto& rMethodInfo{itRMethodInfo->second};
            if (rMethodInfo->fieldInfo != nullptr) {
                ret = -__LINE__;
                ComLogError("load node repeat", GenArg(ret), GenArg(rMethodInfo->methodId));
                return ret;
            }
            rMethodInfo->fieldInfo = rFieldInfo.get();
            rFieldInfo->setterInfo = rMethodInfo.get();
        }
        if (node3.Load("getter", getter) == isoft::kSuccess) {
            auto itRMethodInfo{rServiceInfo.methodInfos.find(getter)};
            if (itRMethodInfo == rServiceInfo.methodInfos.end()) {
                ret = -__LINE__;
                ComLogError("load node missed", GenArg(ret));
                return ret;
            }
            auto& rMethodInfo{itRMethodInfo->second};
            if (rMethodInfo->fieldInfo != nullptr) {
                ret = -__LINE__;
                ComLogError("load node repeat", GenArg(ret), GenArg(rMethodInfo->methodId));
                return ret;
            }
            rMethodInfo->fieldInfo = rFieldInfo.get();
            rFieldInfo->getterInfo = rMethodInfo.get();
        }
        if (rFieldInfo->notifierInfo == nullptr && rFieldInfo->setterInfo == nullptr
            && rFieldInfo->getterInfo == nullptr) {
            ret = -__LINE__;
            ComLogError("load node missed", GenArg(ret));
            return ret;
        }
        rServiceInfo.fieldInfos.emplace(std::move(rFieldInfo));
        return ret;
    }
    /// @brief Initialize implementation layer
    /// @return Result object -- empty/value or error
    ara::core::Result< void > InitializeICC() noexcept
    {
        using Result = ara::core::Result< void >;
        ComLogTrace("initialize stack", GenArg(this));
        Result result{};
        int32_t ret{};
        // Open implementation layer configuration
        isoft::ara_fsh::Process proc;
        auto manifestFile{proc.GetConfig("icc.json")};
        // constexpr auto manifestFile{"etc/icc.json"};
        ComLogTrace("initialize stack: load config", GenArg(manifestFile));
        auto manifestRes{isoft::manifestreader::OpenManifest(manifestFile.c_str())};
        if (!manifestRes) {
            // Ignore no-service application
            if (pServiceInfos_.empty() && rServiceInfos_.empty() && !g_IsDaemon) {
                ComLogDebug("initialize stack ignored: app service info not exist",
                            GenArg(runtime::InstanceIdentifierPrefix()));
                return result;
            }
            ComLogError("initialize stack error: open config failed", GenArg(manifestRes), GenArg(manifestFile));
            result.EmplaceError(std::move(manifestRes).Error());
            return result;
        }
        // Bind event loop
        using EvLoop = isoft::naicpp::GlobalGeneralEvLoop;
        evloop_      = EvLoop::Get();
        // Load implementation layer configuration
        auto manifest{std::move(manifestRes).Value()};
        using Loader = std::function< void(ara::core::StringView const&, isoft::manifestreader::ManifestNode const&) >;
        Loader loader{[&](auto& key, auto& node) {
            if (ret != isoft::kSuccess) {
                return;
            }
            if (node.IsObject()) {
                if (!key.empty()) {
                    std::ignore = icc_conf_setopts(&conf_, "begin", key.data());
                }
                std::ignore = node.IterateObject("", loader);
                if (!key.empty()) {
                    std::ignore = icc_conf_setopts(&conf_, "end", "");
                }
            } else if (node.IsArray()) {
                std::ignore = node.IterateArray("", [&](auto idx, auto& subNode) {
                    std::ignore = idx;
                    loader(key, subNode);
                });
            } else if (node.IsString()) {
                ara::core::String value{};
                if (node.Load("", value) != isoft::kSuccess) {
                    return;
                }
                if ((key == "netname") && (value.front() != '/')) {
                    isoft::ara_fsh::Platform platform;
                    value = platform.GetUnixDomainSocketDir() + '/' + value;
                }
                std::ignore = icc_conf_setopts(&conf_, key.data(), value.c_str());
            } else if (node.IsInt()) {
                int32_t value{};
                if ((ret = node.Load("", value)) != isoft::kSuccess) {
                    ComLogError("load node failed", GenArg(ret), GenArg(key));
                    return;
                }
                std::ignore = icc_conf_setopti(&conf_, key.data(), value);
            } else if (node.IsNumber()) {
                double value{};
                if ((ret = node.Load("", value)) != isoft::kSuccess) {
                    ComLogError("load node failed", GenArg(ret), GenArg(key));
                    return;
                }
                std::ignore = icc_conf_setopti(&conf_, key.data(), value);
            } else {
                ret = -__LINE__;
                ComLogError("load node invalid", GenArg(ret), GenArg(key));
                return;
            }
        }};
        std::ignore = manifest->IterateObject("", loader);
        if (ret != isoft::kSuccess) {
            ComLogError("initialize stack error: parse config failed", GenArg(ret), GenArg(manifestFile));
            result.EmplaceError(MakeErrorCode(ComErrc::kCommunicationStackError, ret));
            return result;
        }
        ret = ICC_CHECK(icc_conf_finish(&conf_), -__LINE__);
        if (ret < 0) {
            result.EmplaceError(MakeErrorCode(ComErrc::kCommunicationStackError, -__LINE__));
            return result;
        }
        ret = ICC_CHECK(icc_routing_set_conf(&routing_, &conf_), -__LINE__);
        if (ret < 0) {
            result.EmplaceError(MakeErrorCode(ComErrc::kCommunicationStackError, -__LINE__));
            return result;
        }
        // Open implementation layer routing
        ret = ICC_CHECK(icc_routing_open(&routing_, evloop_->GetRawEvLoop(), ICC_RTYPE_AUTO), -__LINE__);
        if (ret < 0) {
            result.EmplaceError(MakeErrorCode(ComErrc::kCommunicationStackError, -__LINE__));
            return result;
        }
        // Open implementation layer application
        auto& comRuntime{internal::GetInstance()};
        ret = ICC_CHECK(icc_app_open_name(&app_, &routing_, comRuntime.GetProcessName().data()), -__LINE__);
        if (ret < 0) {
            result.EmplaceError(MakeErrorCode(ComErrc::kCommunicationStackError, -__LINE__));
            return result;
        }
        if (!g_IsDaemon) {
            ret = ICC_CHECK(icc_appcb_open(&appcb_, &app_), -__LINE__);
            if (ret < 0) {
                result.EmplaceError(MakeErrorCode(ComErrc::kCommunicationStackError, -__LINE__));
                return result;
            }
        } else {
            ;  // do nothing
        }
        return result;
    }
    /// @brief Deinitialize implementation layer
    /// @return Result object -- empty/value or error
    ara::core::Result< void > DeinitializeICC() noexcept
    {
        using Result = ara::core::Result< void >;
        ComLogTrace("deinitialize stack", GenArg(this));
        Result result{};
        // Ignore implementation layer deinitialization (not initialized)
        if (evloop_ == nullptr) {
            ComLogDebug("deinitialize stack ignored: stack uninitialized");
            return result;
        }
        int32_t ret{};
        if (!g_IsDaemon) {
            ret = ICC_CHECK(icc_appcb_close(&appcb_), -__LINE__);
            if (ret < 0) {
                result.EmplaceError(MakeErrorCode(ComErrc::kCommunicationStackError, -__LINE__));
            }
        }
        // Close implementation layer application
        ret = ICC_CHECK(icc_app_close(&app_), -__LINE__);
        if (ret < 0) {
            result.EmplaceError(MakeErrorCode(ComErrc::kCommunicationStackError, -__LINE__));
        }
        // Close implementation layer routing
        ret = ICC_CHECK(icc_routing_close(&routing_), -__LINE__);
        if (ret < 0) {
            result.EmplaceError(MakeErrorCode(ComErrc::kCommunicationStackError, -__LINE__));
        }
        // Unload implementation layer configuration
        ret = ICC_CHECK(icc_conf_close(&conf_), -__LINE__);
        if (ret < 0) {
            result.EmplaceError(MakeErrorCode(ComErrc::kCommunicationStackError, -__LINE__));
        }
        // Unbind event loop
        evloop_ = nullptr;
        return result;
    }
    /// @brief Initialize E2E
    /// @return Result object -- empty/value or error
    ara::core::Result< void > InitializeE2E() noexcept
    {
        using Result        = ara::core::Result< void >;
        using Format        = ara::com::e2exf::Format;
        using StatusHandler = ara::com::e2exf::StatusHandler;
        using Transformer   = ara::com::e2exf::Transformer;
        ComLogTrace("initialize E2E", GenArg(this));
        Result result{};
        if (!g_IsDaemon) {
            isoft::ara_fsh::Process proc;
            auto bindingFile{proc.GetConfig("e2e_dataid_mapping.json")};
            auto e2exfFile{proc.GetConfig("e2e_statemachines.json")};
            nai_stat_t sbindingFile;
            nai_stat_t se2exfFile;
            if (nai_stat(bindingFile.data(), &sbindingFile, NAI_STAT_BASIC) >= 0
                && nai_stat(e2exfFile.data(), &se2exfFile, NAI_STAT_BASIC) >= 0) {
                if (!StatusHandler::Configure(ara::core::String(bindingFile), Format::kJson,
                                              ara::core::String(e2exfFile), Format::kJson)) {
                    ComLogError("initialize E2E error: configure status handler failed", GenArg(bindingFile),
                                GenArg(e2exfFile));
                    result.EmplaceError(MakeErrorCode(ComErrc::kCommunicationStackError, __LINE__));
                    return result;
                }
                if (!Transformer::Configure(ara::core::String(e2exfFile), Format::kJson)) {
                    ComLogError("initialize E2E error: configure transformer failed", GenArg(e2exfFile));
                    result.EmplaceError(MakeErrorCode(ComErrc::kCommunicationStackError, __LINE__));
                    return result;
                }
            }
        } else {
            ;  // do nothing
        }
        return result;
    }
    /// @brief Deinitialize E2E
    /// @return Result object -- empty/value or error
    ara::core::Result< void > DeinitializeE2E() noexcept
    {
        using Result = ara::core::Result< void >;
        ComLogTrace("deinitialize E2E", GenArg(this));
        Result result{};
        if (!g_IsDaemon) {  // NOLINT -- compatibility > [bugprone-branch-clone]
            ;               // do nothing
        } else {
            ;  // do nothing
        }
        return result;
    }
#ifdef HAS_COM_SECOC
    /// @brief Initialize SecOC
    /// @return Result object -- empty/value or error
    ara::core::Result< void > InitializeSecOC() noexcept
    {
        using Result                 = ara::core::Result< void >;
        using FVM                    = ara::com::secoc::FVM;
        using SecOCProfileManager    = ara::com::internal::secoc::SecOCProfileManager;
        using SecOC4SOMEIP           = ara::com::internal::secoc::SecOC4SOMEIP;
        using SecOC4Signal           = ara::com::internal::secoc::SecOC4Signal;
        using SecOCStatisticsManager = ara::com::internal::secoc::SecOCStatisticsManager;
        ComLogTrace("initialize secoc", GenArg(this));
        Result result{};
        if (!g_IsDaemon) {
            isoft::ara_fsh::Process const proc;
            ara::core::String secOCProtoclConfig{proc.GetConfig("secoc.json")};
            ara::core::String secOCBindingConfig{proc.GetConfig("secoc_binding.json")};
            if (!SecOCProfileManager::LoadConfig(secOCProtoclConfig)) {
                ComLogDebug("initialize secoc ignored: find correct secoc protocol configuration file failed",
                            GenArg(secOCProtoclConfig));
                // result.EmplaceError(MakeErrorCode(ComErrc::kCommunicationStackError, -__LINE__));
                return result;
            }
            FVM::Initialize();
            SecOCStatisticsManager::Initialize();
            std::ignore = SecOC4SOMEIP::LoadConfig(secOCBindingConfig);
            std::ignore = SecOC4Signal::LoadConfig(secOCBindingConfig);
            ComLogDebug("initialize secoc activated");
        } else {
            ;  // do nothing
        }
        return result;
    }
    /// @brief Deinitialize SecOC
    /// @return Result object -- empty/value or error
    ara::core::Result< void > DeinitializeSecOC() noexcept
    {
        using Result = ara::core::Result< void >;
        ComLogTrace("deinitialize secoc", GenArg(this));
        Result result{};
        if (!g_IsDaemon) {  // NOLINT -- compatibility > [bugprone-branch-clone]
            ;               // do nothing
        } else {
            ;  // do nothing
        }
        return result;
    }
#else
#endif
#ifdef HAS_ARA_IAM
    /// @brief Initialize IAM
    /// @return Result object -- empty/value or error
    ara::core::Result< void > InitializeIAM() noexcept
    {
        using Result = ara::core::Result< void >;
        ComLogTrace("initialize iam", GenArg(this));
        Result result{};
        if (!g_IsDaemon) {
            ;  // do nothing
        } else {
            if (auto res{ara::iam::internal::grant::IAMGrantComQuery::Initialize()}) {
    #if 0  // Binding layer differences
                icc_app_set_cb_offer(&app_, Impl::OnIAMOfferService);
                icc_app_set_cb_find(&app_, Impl::OnIAMFindService);
                icc_app_set_cb_subscribe(&app_, Impl::OnIAMSubscribe);
                icc_app_set_cb_message(&app_, Impl::OnIAMMessage);
    #else
    #endif
                ComLogDebug("initialize iam grant activated");
            } else {
                if (res.Error() == ara::iam::internal::grant::GrantSerializationErrc::kFileNotFound) {
                    ComLogDebug("initialize iam grant ignored");
                    return result;
                }
                result = std::move(res);
                ComLogWarning("initialize iam grant failed", GenArg(result));
                return result;
            }
        }
        return result;
    }
    /// @brief Deinitialize IAM
    /// @return Result object -- empty/value or error
    ara::core::Result< void > DeinitializeIAM() noexcept
    {
        using Result = ara::core::Result< void >;
        ComLogTrace("deinitialize iam", GenArg(this));
        Result result{};
        if (!g_IsDaemon) {
            ;  // do nothing
        } else {
            ComLogDebug("deinitialize iam grant deactivated");
            icc_app_set_cb_offer(&app_, nullptr);
            icc_app_set_cb_find(&app_, nullptr);
            icc_app_set_cb_subscribe(&app_, nullptr);
            icc_app_set_cb_message(&app_, nullptr);
            ara::iam::internal::grant::IAMGrantComQuery::Deinitialize();
        }
        return result;
    }
#else
#endif
public:
    /// @brief Type alias -- marks enumerable
    using IsEnumerableTag = void;
    /// @brief Enumerate operation -- enumerates members of this type via a passed function body
    /// @tparam F Passed function body type
    /// @param[in] fun Passed function body
    template < typename F >
    void enumerate(F& fun)  // NOLINT -- template interface name > naming convention
    {
        fun("cid", app_.cid);
        fun("pinfos", pServiceInfos_);
        fun("rinfos", rServiceInfos_);
    }

private:
    /// @brief Deserialization function set
    std::list< std::function< ara::core::Result< void >() > > deinitializes_;
    /// @brief Type alias -- mapper set -- skeleton side
    using PMappers = ara::core::Map< ServiceInfo::ServiceId, std::shared_ptr< SkeletonMapperBase > >;
    /// @brief Type alias -- mapper set -- proxy side
    using RMappers = ara::core::Map< ServiceInfo::ServiceId, std::shared_ptr< ProxyMapperBase > >;
    /// @brief Type alias -- service identifier to mapper set -- skeleton side
    using ServiceIdentifier2PMappers = ara::core::Map< ServiceIdentifier, PMappers >;
    /// @brief Type alias -- service identifier to mapper set -- proxy side
    using ServiceIdentifier2RMappers = ara::core::Map< ServiceIdentifier, RMappers >;
    /// @brief Mapper set -- skeleton side
    PMappers pMappers_;
    /// @brief Mapper set -- proxy side
    RMappers rMappers_;
    /// @brief Service identifier to mapper set -- skeleton side
    ServiceIdentifier2PMappers serviceIdentifier2PMappers_;
    /// @brief Service identifier to mapper set -- proxy side
    ServiceIdentifier2RMappers serviceIdentifier2RMappers_;
    /// @brief Service instance information set -- skeleton side
    ara::core::Map< ServiceInfo::InstanceID, std::unique_ptr< ServiceInfo > > pServiceInfos_{};
    /// @brief Service instance information set -- proxy side
    ara::core::Map< ServiceInfo::InstanceID, std::unique_ptr< ServiceInfo > > rServiceInfos_{};
    /// @brief Event loop pointer
    std::shared_ptr< isoft::naicpp::EvLoop > evloop_{};
    /// @brief Implementation layer configuration
    icc_conf_t conf_{};
    /// @brief Implementation layer routing
    icc_routing_t routing_{};
    /// @brief Implementation layer application
    icc_app_t app_{};
    /// @brief Implementation layer application callback
    icc_appcb_t appcb_{};
    /// @brief Service discovery handle callback data type
    struct FindHandleCBData
    {
        /// @brief Service discovery handler
        FindServiceHandler< std::shared_ptr< ara::com::internal::proxy::BindHandle > > findHandler;
        /// @brief Request holder
        std::unique_ptr< ServiceRequestedHolder > requestedHolder;
    };
    /// @brief Service discovery handle comparator
    struct FindHandleComparer
    {
        /// @brief Operator -- less than comparison
        /// @param[in] lhs Left operand
        /// @param[in] rhs Right operand
        /// @return lhs < rhs
        bool operator()(FindServiceHandle const& lhs, FindServiceHandle const& rhs) const noexcept
        {
            if (!(lhs.serviceIdentifier == rhs.serviceIdentifier)) {
                return lhs.serviceIdentifier < rhs.serviceIdentifier;
            }
            return lhs.instanceIdentifier < rhs.instanceIdentifier;
        }
    };
    /// @brief Type alias -- service discovery handle to callback data multimap
    using FindHandle2CBData
        = std::multimap< FindServiceHandle, std::shared_ptr< FindHandleCBData >, FindHandleComparer >;
    /// @brief Service discovery handle to callback data multimap
    FindHandle2CBData findHandle2CBData_;
    /// @brief Type alias -- mutex
    using Mutex = std::recursive_mutex;
    /// @brief Mutex -- for notifying available services
    Mutex findHandle2CBDataMutex_;
    /// @brief Type alias -- available instance information set
    using AvailableInstanceInfos = std::set< ServiceInfo::InstanceUID >;
    /// @brief Available instance information set
    AvailableInstanceInfos availableInstanceInfos_;
    /// @brief Mutex -- for recording available services
    Mutex availableInstanceInfosMutex_;
    /// @brief Service instance request reference count
    ara::core::Map< ServiceInfo::InstanceUID, size_t > requestedServices_;
    /// @brief Mutex -- for requesting services
    std::mutex requestedServicesMutex_;
    /// @brief Event group information
    struct GroupInfo
    {
        /// @brief Event group identifier to event identifier set mapping
        ara::core::Map< icc_gid_t, ara::core::Vector< icc_eid_t > > gid2Ids;
        /// @brief Event identifier to subscription event confirmation callback mapping
        ara::core::Map< icc_eid_t, std::function< void(int32_t rcode) > > ackId2Listener;
    };
    /// @brief Instance key to event group information mapping
    ara::core::Map< ServiceInfo::InstanceID, GroupInfo > activeGroups_;
    /// @brief Mutex -- for event groups
    std::mutex activeGroupsMutex_;

public:
    /// @brief Create binding layer skeleton instance -- skeleton side
    /// @param[in] skeleton Service skeleton
    /// @param[in] instanceIdentifier Instance identifier
    /// @param[in] bindSkeletons Skeleton instance pointer set
    void CreateBindSkeleton(ara::com::internal::skeleton::Skeleton& skeleton,
                            InstanceIdentifier const& instanceIdentifier,
                            ara::core::Vector< std::unique_ptr< ara::com::internal::skeleton::BindSkeleton > >&
                                bindSkeletons) noexcept override
    {
        auto const& serviceIdentifier{skeleton.GetServiceIdentifier()};
        ComLogTrace("create bind skeleton", GenArg(serviceIdentifier), GenArg(instanceIdentifier));
        auto optionalId{runtime::ToIccInstanceId(instanceIdentifier)};
        if (!optionalId) {
            ComLogDebug("create bind skeleton ignored: to instance id failed", GenArg(serviceIdentifier),
                        GenArg(instanceIdentifier));
            return;
        }
        auto itPMappers{serviceIdentifier2PMappers_.find(serviceIdentifier)};
        if (itPMappers == serviceIdentifier2PMappers_.end()) {
            ComLogDebug("create bind skeleton ignored: find mappers failed", GenArg(serviceIdentifier),
                        GenArg(instanceIdentifier));
            return;
        }
        auto& pMappers{itPMappers->second};
        for (auto it{pMappers.begin()}; it != pMappers.end(); ++it) {
            auto& pMapper{*it};
            auto& mapper{pMapper.second};
            auto serviceId{mapper->GetServiceId()};
            auto instanceId{*optionalId};
            auto* serviceInfo{GetPServiceInfo(serviceId, instanceId)};
            if (serviceInfo == nullptr) {
                continue;
            }
            auto bindSkeleton{mapper->CreateBindSkeleton(skeleton, instanceIdentifier)};
            if (bindSkeleton == nullptr) {
                continue;
            }
            bindSkeletons.emplace_back(std::move(bindSkeleton));
        }
    }
    /// @brief Offer service -- skeleton side
    /// @param[in] instanceUID Service instance unique identifier
    /// @return Result -- 0: success; !0: failure;
    int32_t OfferService(ServiceInfo::InstanceUID const& instanceUID) noexcept
    {
        ComLogDebug("offer stack service", GenArg(instanceUID));
        auto ret{ICC_CHECK(icc_app_offer_service(&app_, std::get< 0 >(instanceUID), std::get< 1 >(instanceUID),
                                                 std::get< 2 >(instanceUID), std::get< 3 >(instanceUID)),
                           "offer stack service error: invoke stack offer service failed", GenArg(instanceUID))};
        if (ret < 0) {
            return ret;
        }
        ret = ICC_CHECK(
            icc_appcb_set_subscribe(&appcb_, std::get< 0 >(instanceUID), std::get< 1 >(instanceUID), Impl::OnSubscribe),
            "offer stack service error: invoke stack set subscribe callback failed", GenArg(instanceUID));
        return ret;
    }
    /// @brief Stop offering service -- skeleton side
    /// @param[in] instanceUID Service instance unique identifier
    /// @return Result -- 0: success; !0: failure;
    int32_t StopService(ServiceInfo::InstanceUID const& instanceUID) noexcept
    {
        ComLogDebug("stop stack service", GenArg(instanceUID));
        int32_t result{};
        auto ret{
            ICC_CHECK(icc_appcb_set_subscribe(&appcb_, std::get< 0 >(instanceUID), std::get< 1 >(instanceUID), nullptr),
                      "stop stack service error: invoke stack unset subscribe callback failed", GenArg(instanceUID))};
        if (ret < 0) {
            result = ret;
        }
        ret = ICC_CHECK(icc_app_stop_service(&app_, std::get< 0 >(instanceUID), std::get< 1 >(instanceUID),
                                             std::get< 2 >(instanceUID), std::get< 3 >(instanceUID)),
                        "stop stack service error: invoke stack stop service failed", GenArg(instanceUID));
        if (ret < 0) {
            result = ret;
        }
        return result;
    }

private:
#ifdef HAS_ARA_IAM
    /// @brief Receive service offering
    /// @param[in] a Implementation layer application
    /// @param[in] ud User data
    /// @param[in] serv Service identifier
    /// @param[in] inst Instance identifier
    /// @param[in] major Service major version
    /// @param[in] minor Service minor version
    /// @param[in] cred Credential information
    /// @return Result -- 0: success; !0: failure;
    static int32_t OnIAMOfferService(icc_app_t* a,
                                     void* ud,
                                     icc_serv_t serv,
                                     icc_inst_t inst,
                                     icc_version_t major,
                                     icc_versmin_t minor,
                                     icc_cred_t const* cred) noexcept
    {
        std::ignore = a;
        std::ignore = ud;
        std::ignore = major;
        std::ignore = minor;
        ComLogDebug("on iam offer service", GenArg(serv), GenArg(inst), GenArg(major), GenArg(minor));
        if (cred == nullptr) {
            return -1;
        }
        bool ret{ara::iam::internal::grant::IAMGrantComQuery::HasOfferServiceGrant(cred->pid, serv, inst)};
        if (!ret) {
            ComLogDebug("iam grant forbidden");
            return -1;
        }
        ComLogTrace("iam grant success");
        return 0;
    }
#else
#endif
public:
    /// @brief Get set of available service instance handles -- for service discovery -- proxy side
    /// @param[in] serviceIdentifier Service identifier
    /// @param[in] instanceIdentifier Instance identifier
    /// @param[in] bindHandles Set of available service handles
    /// @ref [SWS_CM_00202]
    /// @ref [SWS_CM_10202]
    void GetAvailableServiceHandles(ServiceIdentifier const& serviceIdentifier,
                                    InstanceIdentifier const& instanceIdentifier,
                                    ServiceHandleContainer< std::shared_ptr< ara::com::internal::proxy::BindHandle > >&
                                        bindHandles) noexcept override
    {
        ComLogTrace("get available service handles", GenArg(serviceIdentifier), GenArg(instanceIdentifier));
        auto optionalId{runtime::ToIccInstanceId(instanceIdentifier)};
        if (!optionalId) {  // Conversion failed, instance identifier not supported
            ComLogDebug("get available service handles ignored: to instance id failed", GenArg(serviceIdentifier),
                        GenArg(instanceIdentifier));
            return;
        }
        auto itRMappers{serviceIdentifier2RMappers_.find(serviceIdentifier)};
        if (itRMappers == serviceIdentifier2RMappers_.end()) {
            ComLogDebug("get available service handles ignored: find mappers failed", GenArg(serviceIdentifier),
                        GenArg(instanceIdentifier));
            return;
        }
        auto& rMappers{itRMappers->second};
        for (auto it{rMappers.begin()}; it != rMappers.end(); ++it) {
            auto& rMapper{*it};
            auto& mapper{rMapper.second};
            auto serviceId{mapper->GetServiceId()};
            auto instanceId{*optionalId};
            auto requestMajor{Message::kMajorVersionAny};
            auto requestMinor{Message::kMinorVersionAny};
            auto* serviceInfo{GetRServiceInfo(serviceId, instanceId)};
            if (serviceInfo != nullptr) {
                auto& rServiceInfo{reinterpret_cast< RServiceInfo& >(*serviceInfo)};
                requestMajor = rServiceInfo.version.major;
                requestMinor = rServiceInfo.version.minor;
                if (rServiceInfo.versionDrivenFindBehavior
                    == RServiceInfo::VersionDrivenFindBehavior::kMinimumMinorVersion) {
                    requestMinor = Message::kMinorVersionAny;
                }
            } else if (instanceId != Message::kInstanceIdAny) {
                ComLogDebug("get available service handles skipped: get service info failed", GenArg(serviceId),
                            GenArg(instanceId));
                continue;
            }
            // Trigger a query, callback destructor will automatically end the query
            ServiceRequestedHolder requestedHolder{
                ServiceInfo::InstanceUID{serviceId, instanceId, requestMajor, requestMinor}};
            // Get active service instances (from implementation layer cache, based on given service/instance information, and with compatible version)
            nai_array_t out;
            auto ret{ICC_CHECK(nai_array_init(&out, sizeof(icc_service_info_t), nullptr), -__LINE__)};
            if (ret < 0) {
                continue;
            }
            ret = ICC_CHECK(icc_appcb_get_availables(&appcb_, serviceId, instanceId, &out), GenArg(serviceId),
                            GenArg(instanceId));
            if (ret < 0) {
                std::ignore = nai_array_close(&out);
                continue;
            }
            auto* availableInfo{reinterpret_cast< icc_service_info_t* >(out.elts)};
            for (decltype(out.count) i{}; i < out.count; ++i, ++availableInfo) {
                auto instanceUID{ServiceInfo::InstanceUID{availableInfo->serv, availableInfo->inst,
                                                          availableInfo->major, availableInfo->minor}};
                std::lock_guard< Mutex > lock(availableInstanceInfosMutex_);
                auto itPair{availableInstanceInfos_.emplace(instanceUID)};
                if (!itPair.second) {
                    ComLogDebug("get available service handles skipped: update service instance available repeat",
                                GenArg(instanceUID));
                    continue;
                }
            }
            std::ignore = nai_array_close(&out);
            {
                std::lock_guard< Mutex > lock(availableInstanceInfosMutex_);
                for (auto itAvailableInstanceInfo{availableInstanceInfos_.begin()};
                     itAvailableInstanceInfo != availableInstanceInfos_.end();) {
                    auto const& instanceUID{*itAvailableInstanceInfo};
                    auto available{
                        icc_appcb_is_available(&appcb_, std::get< 0 >(instanceUID), std::get< 1 >(instanceUID))};
                    auto instanceAvailable{available == ICC_AVAIL_CHANGED || available == ICC_AVAIL_ONLINED};
                    if (!instanceAvailable) {
                        ComLogDebug("get available service handles skipped: update service instance unavailable",
                                    GenArg(instanceUID));
                        availableInstanceInfos_.erase(itAvailableInstanceInfo++);
                        continue;
                    }
                    ++itAvailableInstanceInfo;
                    if (serviceInfo != nullptr) {
                        auto& rServiceInfo{reinterpret_cast< RServiceInfo& >(*serviceInfo)};
                        if (!_verifyServiceAvailable(rServiceInfo, instanceUID)) {
                            ComLogDebug(
                                "get available service handles skipped: verify service instance available failed",
                                GenArg(instanceUID));
                            continue;
                        }
                    } else if (std::get< 0 >(instanceUID) != serviceId) {
                        ComLogDebug("get available service handles skipped: verify service id failed",
                                    GenArg(instanceUID));
                        continue;
                    }
                    auto ins{runtime::ToInstanceIdentifier(std::get< 1 >(instanceUID))};
                    bindHandles.emplace_back(mapper->CreateBindHandle(ins));
                }
            }
        }
        ComLogDebug("get available service handles done", GenArg(serviceIdentifier), GenArg(instanceIdentifier),
                    GenArg(bindHandles));
    }
    /// @brief Register service discovery handle -- for starting service discovery -- proxy side
    /// @param[in] findHandle Service discovery handle
    /// @param[in] findHandler Service discovery handler
    void RegisterFindServiceHandle(FindServiceHandle const& findHandle,
                                   FindServiceHandler< std::shared_ptr< ara::com::internal::proxy::BindHandle > > const&
                                       findHandler) noexcept override
    {
        ComLogTrace("register find service handle", GenArg(findHandle));
        auto optionalId{runtime::ToIccInstanceId(findHandle.instanceIdentifier)};
        if (!optionalId) {  // Conversion failed, instance identifier not supported
            ComLogDebug("register find service handle ignored: to instance id failed", GenArg(findHandle));
            return;
        }
        auto itRMappers{serviceIdentifier2RMappers_.find(findHandle.serviceIdentifier)};
        if (itRMappers == serviceIdentifier2RMappers_.end()) {
            ComLogDebug("register find service handle ignored: find mappers failed", GenArg(findHandle));
            return;
        }
        auto& rMappers{itRMappers->second};
        for (auto it{rMappers.begin()}; it != rMappers.end(); ++it) {
            auto& rMapper{*it};
            auto& mapper{rMapper.second};
            auto serviceId{mapper->GetServiceId()};
            auto instanceId{*optionalId};
            auto requestMajor{Message::kMajorVersionAny};
            auto requestMinor{Message::kMinorVersionAny};
            auto* serviceInfo{GetRServiceInfo(serviceId, instanceId)};
            if (serviceInfo != nullptr) {
                auto& rServiceInfo{reinterpret_cast< RServiceInfo& >(*serviceInfo)};
                requestMajor = rServiceInfo.version.major;
                requestMinor = rServiceInfo.version.minor;
                if (rServiceInfo.versionDrivenFindBehavior
                    == RServiceInfo::VersionDrivenFindBehavior::kMinimumMinorVersion) {
                    requestMinor = Message::kMinorVersionAny;
                }
            } else if (instanceId != Message::kInstanceIdAny) {
                ComLogDebug("register find service handle skipped: get service info failed", GenArg(serviceId),
                            GenArg(instanceId));
                continue;
            }
            // Synchronously register this layer's callback data before registration (because the callback process involves double locks at this layer and the lower layer, to ensure concurrency, avoid deadlock, and maintain registration query timing, the smart pointer scheme is adopted: reserve first, then construct trigger query)
            auto enableSetAvailable{false};
            auto findHandleCBData{std::make_shared< FindHandleCBData >()};
            findHandleCBData->findHandler = findHandler;
            {
                std::lock_guard< Mutex > lock(findHandle2CBDataMutex_);
                enableSetAvailable = findHandle2CBData_.count(findHandle) == 0;
                std::ignore        = findHandle2CBData_.emplace(findHandle, findHandleCBData);
            }
            // Register callback
            if (enableSetAvailable) {
                ICC_ASSERT(icc_appcb_get_ud(&appcb_, serviceId, instanceId) == nullptr,
                           "register find service handle error: invoke stack set callback userdata repeat",
                           GenArg(serviceId), GenArg(instanceId));
                ICC_ASSERT(icc_appcb_set_ud(&appcb_, serviceId, instanceId, this),
                           "register find service handle error: invoke stack set callback userdata failed",
                           GenArg(serviceId), GenArg(instanceId));
                ICC_ASSERT(icc_appcb_set_available(&appcb_, serviceId, instanceId, Impl::OnServiceAvailable),
                           "register find service handle error: invoke stack set available callback failed",
                           GenArg(serviceId), GenArg(instanceId));
            }
            // Trigger query after registration
            findHandleCBData->requestedHolder = std::make_unique< ServiceRequestedHolder >(
                ServiceInfo::InstanceUID{serviceId, instanceId, requestMajor, requestMinor});
        }
        ServiceHandleContainer< std::shared_ptr< ara::com::internal::proxy::BindHandle > > bindHandles;
        GetAvailableServiceHandles(findHandle.serviceIdentifier, findHandle.instanceIdentifier, bindHandles);
        if (!bindHandles.empty() && findHandle2CBData_.count(findHandle) != 0) {
            ComLogDebug("register find service handle: invoke callback handle service instance list",
                        GenArg(bindHandles), GenArg(findHandle));
            findHandler(bindHandles, findHandle);
            ComLogTrace("register find service handle: invoke callback handle service instance list done",
                        GenArg(bindHandles), GenArg(findHandle));
        }
        ComLogDebug("register find service handle done", GenArg(findHandle));
    }
    /// @brief Unregister service discovery handle -- for stopping service discovery -- proxy side
    /// @param[in] findHandle Service discovery handle
    void UnregisterFindServiceHandle(FindServiceHandle const& findHandle) noexcept override
    {
        ComLogTrace("unregister find service handle", GenArg(findHandle));
        auto enableUnsetAvailable{false};
        {
            std::lock_guard< Mutex > lock(findHandle2CBDataMutex_);
            auto itPair{findHandle2CBData_.equal_range(findHandle)};
            for (auto it{itPair.first}; it != itPair.second; ++it) {
                if (findHandle.uid != it->first.uid) {
                    continue;
                }
                std::ignore          = findHandle2CBData_.erase(it);
                enableUnsetAvailable = findHandle2CBData_.count(findHandle) == 0;
                break;
            }
        }
        if (enableUnsetAvailable) {
            auto optionalId{runtime::ToIccInstanceId(findHandle.instanceIdentifier)};
            if (!optionalId) {  // Conversion failed, instance identifier not supported
                ComLogDebug("unregister find service handle ignored: to instance id failed", GenArg(findHandle));
                return;
            }
            auto itRMappers{serviceIdentifier2RMappers_.find(findHandle.serviceIdentifier)};
            if (itRMappers == serviceIdentifier2RMappers_.end()) {
                ComLogDebug("unregister find service handle ignored: find mappers failed", GenArg(findHandle));
                return;
            }
            auto& rMappers{itRMappers->second};
            for (auto it{rMappers.begin()}; it != rMappers.end(); ++it) {
                auto& rMapper{*it};
                auto& mapper{rMapper.second};
                auto serviceId{mapper->GetServiceId()};
                auto instanceId{*optionalId};
                ICC_CHECK(icc_appcb_get_ud(&appcb_, serviceId, instanceId) != nullptr,
                          "unregister find service handle error: invoke stack unset callback userdata repeat",
                          GenArg(serviceId), GenArg(instanceId));
                ICC_CHECK(icc_appcb_set_available(&appcb_, serviceId, instanceId, nullptr),
                          "unregister find service handle error: invoke stack unset callback failed", GenArg(serviceId),
                          GenArg(instanceId));
                ICC_CHECK(icc_appcb_set_ud(&appcb_, serviceId, instanceId, nullptr),
                          "unregister find service handle error: invoke stack unset callback userdata failed",
                          GenArg(serviceId), GenArg(instanceId));
            }
        }
        ComLogDebug("unregister find service handle done", GenArg(findHandle));
    }
    /// @brief Request service -- proxy side
    /// @param[in] instanceUID Service instance unique identifier
    void RequestService(ServiceInfo::InstanceUID const& instanceUID) noexcept
    {
        ComLogTrace("request service", GenArg(instanceUID));
        auto enableRequest{false};
        {
            std::lock_guard< std::mutex > lock(requestedServicesMutex_);
            auto& refcount{requestedServices_[instanceUID]};
            enableRequest = ++refcount == 1;
        }
        if (enableRequest) {
            ComLogDebug("request service: invoke stack request service", GenArg(instanceUID));
            ICC_ASSERT(icc_appcb_set_subscribe_ack(&appcb_, std::get< 0 >(instanceUID), std::get< 1 >(instanceUID),
                                                   Impl::OnSubscribeAck),
                       "request service error: invoke stack set subscribe ack callback failed", GenArg(instanceUID));
            ICC_ASSERT(icc_app_request_service(&app_, std::get< 0 >(instanceUID), std::get< 1 >(instanceUID),
                                               std::get< 2 >(instanceUID), std::get< 3 >(instanceUID)),
                       "request service error: invoke stack request service failed", GenArg(instanceUID));
        }
    }
    /// @brief Release service request -- proxy side
    /// @param[in] instanceUID Service instance unique identifier
    void ReleaseService(ServiceInfo::InstanceUID const& instanceUID) noexcept
    {
        ComLogTrace("release service", GenArg(instanceUID));
        auto enableRelease{false};
        {
            std::lock_guard< std::mutex > lock(requestedServicesMutex_);
            auto it{requestedServices_.find(instanceUID)};
            ICC_CHECK(it != requestedServices_.end(), "release service error: find requested services failed",
                      GenArg(instanceUID));
            auto& refcount{it->second};
            enableRelease = --refcount == 0;
            if (enableRelease) {
                std::ignore = requestedServices_.erase(it);
            }
        }
        if (enableRelease) {
            ComLogDebug("release service: invoke stack release service", GenArg(instanceUID));
            ICC_CHECK(icc_app_release_service(&app_, std::get< 0 >(instanceUID), std::get< 1 >(instanceUID),
                                              std::get< 2 >(instanceUID), std::get< 3 >(instanceUID)),
                      "release service error: invoke stack release service failed", GenArg(instanceUID));
            ICC_CHECK(
                icc_appcb_set_subscribe_ack(&appcb_, std::get< 0 >(instanceUID), std::get< 1 >(instanceUID), nullptr),
                "release service error: invoke stack unset subscribe ack callback failed", GenArg(instanceUID));
        }
    }

private:
#ifdef HAS_ARA_IAM
    /// @brief Receive service discovery
    /// @param[in] a Implementation layer application
    /// @param[in] ud User data
    /// @param[in] serv Service identifier
    /// @param[in] inst Instance identifier
    /// @param[in] major Service major version
    /// @param[in] minor Service minor version
    /// @param[in] cred Credential information
    /// @return Result -- 0: success; !0: failure;
    static int32_t OnIAMFindService(icc_app_t* a,
                                    void* ud,
                                    icc_serv_t serv,
                                    icc_inst_t inst,
                                    icc_version_t major,
                                    icc_versmin_t minor,
                                    icc_cred_t const* cred) noexcept
    {
        std::ignore = a;
        std::ignore = ud;
        std::ignore = major;
        std::ignore = minor;
        ComLogDebug("on iam find service", GenArg(serv), GenArg(inst), GenArg(major), GenArg(minor));
        if (cred == nullptr) {
            ComLogWarning("iam grant failed, invalid cred");
            return -1;
        }
        if (!ara::iam::internal::grant::IAMGrantComQuery::HasFindServiceGrant(cred->pid, serv, inst)) {
            ComLogDebug("iam grant forbidden");
            return -1;
        }
        ComLogTrace("iam grant success");
        return 0;
    }
#else
#endif
    /// @brief Callback function when service availability changes -- proxy side
    /// @param[in] a Implementation layer application
    /// @param[in] ud User data
    /// @param[in] avail Service availability
    /// @param[in] serv Service identifier
    /// @param[in] inst Instance identifier
    /// @param[in] major Service major version
    /// @param[in] minor Service minor version
    /// @param[in] cred Credential information
    /// @return Result -- 0: success; !0: failure;
    static int32_t OnServiceAvailable(icc_app_t* a,
                                      void* ud,
                                      int32_t avail,
                                      icc_serv_t serv,
                                      icc_inst_t inst,
                                      icc_version_t major,
                                      icc_versmin_t minor,
                                      icc_cred_t const* cred) noexcept
    {
        std::ignore = a;
        std::ignore = major;
        std::ignore = minor;
        std::ignore = cred;
        ComLogDebug("on service available", GenArg(serv), GenArg(inst), GenArg(avail));
        auto* thisObj{reinterpret_cast< Impl* >(ud)};
        return thisObj->_handleServiceAvailable({serv, inst, major, minor}, avail);
    }
    /// @brief Process service availability change -- proxy side
    /// @param[in] instanceUID Service instance unique identifier
    /// @param[in] available Service availability
    /// @return Result -- 0: success; !0: failure;
    /// @ref [SWS_CM_00202]
    /// @ref [SWS_CM_10202]
    int32_t _handleServiceAvailable(ServiceInfo::InstanceUID const& instanceUID, int32_t available) noexcept
    {
        auto instanceAvailable{available == ICC_AVAIL_CHANGED || available == ICC_AVAIL_ONLINED};
        ComLogTrace("handle service available", GenArg(instanceUID), GenArg(available));
        // Update list of available (available and application-supported) services
        if (instanceAvailable) {
            std::lock_guard< Mutex > lock(availableInstanceInfosMutex_);
            auto itPair{availableInstanceInfos_.emplace(instanceUID)};
            if (!itPair.second) {
                ComLogDebug("handle service available ignored: update service instance available repeat",
                            GenArg(instanceUID));
                return available;
            }
        } else {
            std::lock_guard< Mutex > lock(availableInstanceInfosMutex_);
            auto size{availableInstanceInfos_.erase(instanceUID)};
            if (size == 0) {
                ComLogDebug("handle service available ignored: update service instance unavailable repeat",
                            GenArg(instanceUID));
                return available;
            }
        }
        // Get currently application-supported service information from registered service map via service identifier
        auto itMapper{rMappers_.find(std::get< 0 >(instanceUID))};
        ICC_ASSERT(itMapper != rMappers_.end(), GenArg(instanceUID));
        auto& mapper{itMapper->second};
        // Callback registered service discovery handle set (associated service discovery handler)
        auto const& serviceIdentifier{mapper->GetServiceIdentifier()};
        auto itRMappers{serviceIdentifier2RMappers_.find(serviceIdentifier)};
        auto& rMappers{itRMappers->second};
        for (auto it{rMappers.begin()}; it != rMappers.end(); ++it) {
            auto& rMapper{*it};
            auto& mapper2{rMapper.second};
            // Callback service discovery handle set (specified instance identifier)
            auto* serviceInfo{GetRServiceInfo(std::get< 0 >(instanceUID), std::get< 1 >(instanceUID))};
            if (serviceInfo != nullptr) {
                auto& rServiceInfo{reinterpret_cast< RServiceInfo& >(*serviceInfo)};
                auto instanceIdentifier{runtime::ToInstanceIdentifier(std::get< 1 >(instanceUID))};
                std::lock_guard< Mutex > lock(findHandle2CBDataMutex_);
                auto itPair{findHandle2CBData_.equal_range({serviceIdentifier, instanceIdentifier, 0})};
                for (auto it2{itPair.first}; it2 != itPair.second;) {
                    auto itCurrent{it2++};
                    auto findServiceHandle{itCurrent->first};
                    auto findHandleCBData{itCurrent->second};
                    // Determine if currently available service matches the requirement
                    if (instanceAvailable && _verifyServiceAvailable(rServiceInfo, instanceUID)) {  // Notify specified instance available
                        ComLogDebug("handle service available: invoke callback handle specific instance available",
                                    GenArg(instanceUID), GenArg(findServiceHandle));
                        findHandleCBData->findHandler({mapper2->CreateBindHandle(findServiceHandle.instanceIdentifier)},
                                                      findServiceHandle);
                        ComLogTrace("handle service available: invoke callback handle specific instance available done",
                                    GenArg(instanceUID), GenArg(findServiceHandle));
                    } else {  // Notify specified instance unavailable
                        ComLogDebug("handle service available: invoke callback handle specific instance unavailable",
                                    GenArg(instanceUID), GenArg(findServiceHandle));
                        findHandleCBData->findHandler({}, findServiceHandle);
                        ComLogTrace(
                            "handle service available: invoke callback handle specific instance unavailable done",
                            GenArg(instanceUID), GenArg(findServiceHandle));
                    }
                }
            }
            ServiceHandleContainer< std::shared_ptr< ara::com::internal::proxy::BindHandle > > bindHandles;
            serviceInfo = GetRServiceInfo(std::get< 0 >(instanceUID), Message::kInstanceIdAny);
            {
                std::lock_guard< Mutex > lock(availableInstanceInfosMutex_);
                auto lower{availableInstanceInfos_.lower_bound({std::get< 0 >(instanceUID), 0, 0, 0})};
                auto upper{availableInstanceInfos_.upper_bound({std::get< 0 >(instanceUID), Message::kInstanceIdAny,
                                                                Message::kMajorVersionAny, Message::kMinorVersionAny})};
                for (auto it2{lower}; it2 != upper; ++it2) {
                    auto const& ins{*it2};
                    if (serviceInfo != nullptr) {
                        auto& rServiceInfo{reinterpret_cast< RServiceInfo& >(*serviceInfo)};
                        // Determine if currently available service matches the requirement
                        if (_verifyServiceAvailable(rServiceInfo, ins)) {
                            auto instanceIdentifier{runtime::ToInstanceIdentifier(std::get< 1 >(ins))};
                            bindHandles.emplace_back(mapper2->CreateBindHandle(instanceIdentifier));
                        }
                    } else {
                        auto instanceIdentifier{runtime::ToInstanceIdentifier(std::get< 1 >(ins))};
                        bindHandles.emplace_back(mapper2->CreateBindHandle(instanceIdentifier));
                    }
                }
            }
            // Callback service discovery handle set (any instance identifier)
            auto anyInstanceIdentifier{InstanceIdentifier::MakeAny()};
            {
                std::lock_guard< Mutex > lock(findHandle2CBDataMutex_);
                auto itPair{findHandle2CBData_.equal_range({serviceIdentifier, anyInstanceIdentifier, 0})};
                for (auto it2{itPair.first}; it2 != itPair.second;) {
                    auto itCurrent{it2++};
                    auto findServiceHandle{itCurrent->first};
                    auto findHandleCBData{itCurrent->second};
                    ComLogDebug("handle service available: invoke callback handle any instance list changed",
                                GenArg(instanceUID), GenArg(findServiceHandle));
                    findHandleCBData->findHandler(bindHandles, findServiceHandle);
                    ComLogTrace("handle service available: invoke callback handle any instance list changed done",
                                GenArg(instanceUID), GenArg(findServiceHandle));
                }
            }
            // Callback service discovery handle set (any binding instance identifier)
            auto anyIccInstanceIdentifier{runtime::ToInstanceIdentifier(Message::kInstanceIdAny)};
            {
                std::lock_guard< Mutex > lock(findHandle2CBDataMutex_);
                auto itPair{findHandle2CBData_.equal_range({serviceIdentifier, anyIccInstanceIdentifier, 0})};
                for (auto it2{itPair.first}; it2 != itPair.second;) {
                    auto itCurrent{it2++};
                    auto findServiceHandle{itCurrent->first};
                    auto findHandleCBData{itCurrent->second};
                    ComLogDebug("handle service available: invoke callback handle service instance list changed",
                                GenArg(instanceUID), GenArg(findServiceHandle));
                    findHandleCBData->findHandler(bindHandles, findServiceHandle);
                    ComLogTrace("handle service available: invoke callback handle service instance list changed done",
                                GenArg(instanceUID), GenArg(findServiceHandle));
                }
            }
        }
        return 0;
    }
    /// @brief Verify if the given service instance matches the required service information
    /// @param[in] rServiceInfo Service information
    /// @param[in] instanceUID Service instance unique identifier
    /// @return bool
    bool _verifyServiceAvailable(RServiceInfo const& rServiceInfo, ServiceInfo::InstanceUID const& instanceUID) noexcept
    {
        if (rServiceInfo.version.major != std::get< 2 >(instanceUID)) {
            return false;
        }
        if (rServiceInfo.versionDrivenFindBehavior == RServiceInfo::VersionDrivenFindBehavior::kMinimumMinorVersion) {
            if (rServiceInfo.requiredMinorVersion > std::get< 3 >(instanceUID)) {
                return false;
            }
        } else if (rServiceInfo.requiredMinorVersion != Message::kMinorVersionAny) {
            if (rServiceInfo.requiredMinorVersion != std::get< 3 >(instanceUID)) {
                return false;
            }
        }
        bool instanceDisable{false};
        for (auto const& blacklistedVersion : rServiceInfo.blacklistedVersions) {
            if (blacklistedVersion.minor == std::get< 3 >(instanceUID)
                && (blacklistedVersion.major == std::get< 2 >(instanceUID)
                    || blacklistedVersion.major == Message::kMajorVersionUndefinedBlacklisted)) {
                instanceDisable = true;
                break;
            }
        }
        if (instanceDisable) {
            return false;
        }
        if (rServiceInfo.instanceId != std::get< 1 >(instanceUID)
            && rServiceInfo.instanceId != Message::kInstanceIdAny) {
            return false;
        }
        return true;
    }

public:
    /// @brief Subscribe to event -- proxy side
    /// @param[in] serviceId Service identifier
    /// @param[in] instanceId Instance identifier
    /// @param[in] major Service major version
    /// @param[in] eid Event identifier
    /// @param[in] ev Event information
    /// @param[in] callback Subscription event confirmation callback
    /// @return Result -- 0: success; !0: failure;
    int32_t Subscribe(ServiceInfo::ServiceId serviceId,
                      ServiceInfo::InstanceId instanceId,
                      ServiceInfo::MajorVersion major,
                      ServiceInfo::EventId eid,
                      icc_event_info_t* ev,
                      std::function< void(int32_t rcode) >&& callback) noexcept
    {
        ComLogDebug("subscribe stack event", GenArg(serviceId), GenArg(instanceId), GenArg(major), GenArg(eid));
        ICC_CHECK(eid != Message::kMethodIdAny, "subscribe stack event refused: event id invalid", GenArg(serviceId),
                  GenArg(instanceId), GenArg(major), GenArg(eid));
        int32_t result{};
        {
            std::lock_guard< std::mutex > lock(activeGroupsMutex_);
            auto& groupInfo{activeGroups_[{serviceId, instanceId}]};
            auto& eventIds{groupInfo.gid2Ids[ev->groups[0]]};
            if (std::find(eventIds.begin(), eventIds.end(), eid) == eventIds.end()) {
                groupInfo.gid2Ids[ev->groups[0]].emplace_back(eid);
            }
            groupInfo.ackId2Listener[eid] = std::move(callback);
        }
        auto ret{ICC_CHECK(icc_app_request_event(&app_, serviceId, instanceId, eid, ev),
                           "subscribe stack event error: invoke stack request event failed", GenArg(serviceId),
                           GenArg(instanceId), GenArg(major), GenArg(eid))};
        if (ret < 0) {
            result = ret;
            return result;
        }
        ret = ICC_CHECK(icc_app_subscribe(&app_, serviceId, instanceId, ev->groups[0], major, eid),
                        "subscribe stack event error: invoke stack subscribe failed", GenArg(serviceId),
                        GenArg(instanceId), GenArg(major), GenArg(eid));
        if (ret < 0) {
            result = ret;
            return result;
        }
        return result;
    }
    /// @brief Unsubscribe from event -- proxy side
    /// @param[in] serviceId Service identifier
    /// @param[in] instanceId Instance identifier
    /// @param[in] major Service major version
    /// @param[in] eid Event identifier
    /// @param[in] ev Event information
    /// @return Result -- 0: success; !0: failure;
    int32_t Unsubscribe(ServiceInfo::ServiceId serviceId,
                        ServiceInfo::InstanceId instanceId,
                        ServiceInfo::MajorVersion major,
                        ServiceInfo::EventId eid,
                        icc_event_info_t* ev) noexcept
    {
        ComLogDebug("unsubscribe stack event", GenArg(serviceId), GenArg(instanceId), GenArg(major), GenArg(eid));
        ICC_CHECK(eid != Message::kMethodIdAny, "unsubscribe stack event refused: event id invalid", GenArg(serviceId),
                  GenArg(instanceId), GenArg(major), GenArg(eid));
        int32_t result{};
        auto ret{ICC_CHECK(icc_app_unsubscribe(&app_, serviceId, instanceId, ev->groups[0], major, eid),
                           "unsubscribe stack event error: invoke stack unsubscribe failed", GenArg(serviceId),
                           GenArg(instanceId), GenArg(major), GenArg(eid))};
        if (ret < 0) {
            result = ret;
        }
        ret = ICC_CHECK(icc_app_release_event(&app_, serviceId, instanceId, eid),
                        "unsubscribe stack event error: invoke stack release event failed", GenArg(serviceId),
                        GenArg(instanceId), GenArg(major), GenArg(eid));
        if (ret < 0) {
            result = ret;
        }
        {
            std::lock_guard< std::mutex > lock(activeGroupsMutex_);
            auto& groupInfo{activeGroups_[{serviceId, instanceId}]};
            ret = ICC_CHECK(groupInfo.ackId2Listener.erase(eid) == 1,
                            "unsubscribe stack event error: find subscriber(event) failed", GenArg(serviceId),
                            GenArg(instanceId), GenArg(major), GenArg(eid));
            if (ret < 0) {
                result = ret;
            }
        }
        return result;
    }

private:
#ifdef HAS_ARA_IAM
    /// @brief Receive subscription event
    /// @param[in] a Implementation layer application
    /// @param[in] ud User data
    /// @param[in] cid Client identifier
    /// @param[in] serv Service identifier
    /// @param[in] inst Instance identifier
    /// @param[in] gid Event group identifier
    /// @param[in] eid Event identifier
    /// @param[in] cred Credential information
    /// @return Result -- 0: success; !0: failure;
    static int32_t OnIAMSubscribe(icc_app_t* a,
                                  void* ud,
                                  icc_cid_t cid,
                                  icc_serv_t serv,
                                  icc_inst_t inst,
                                  icc_gid_t gid,
                                  icc_eid_t eid,
                                  icc_cred_t const* cred) noexcept
    {
        std::ignore = a;
        std::ignore = ud;
        std::ignore = cid;
        std::ignore = gid;
        ComLogDebug("on iam subscribe event", GenArg(cid), GenArg(serv), GenArg(inst), GenArg(gid), GenArg(eid));
        if (cred == nullptr) {
            ComLogWarning("iam grant failed, invalid cred");
            return -1;
        }
        switch (cred->type) {
            case ICC_CRED_PID:
                if (!ara::iam::internal::grant::IAMGrantComQuery::HasEventGrant(cred->pid, serv, inst, eid)) {
                    ComLogDebug("iam grant forbidden");
                    return -1;
                }
                break;
            case ICC_CRED_ADDRESS:
                if (cred->addr.in.sa_family == AF_INET) {
                    char ip[INET_ADDRSTRLEN]{};
                    if (nai_inet_ntop(AF_INET, &(cred->addr.in4.sin_addr), ip, sizeof(ip)) == nullptr) {
                        ComLogWarning("iam grant failed, invalid ipv4 address");
                        return -1;
                    }
                    ara::core::String ipv4{ip};
                    ara::core::String lipv4{};
                    auto port{cred->addr.in4.sin_port};
                    int32_t const lport{-1};
                    ComLogDebug("iam grant", GenArg(ipv4), GenArg(port));
                    if (!ara::iam::internal::grant::IAMGrantComQuery::HasEventRemoteIpGrant(ipv4, port, lipv4, lport,
                                                                                            serv, inst, eid)) {
                        ComLogDebug("iam grant forbidden");
                        return -1;
                    }
                } else if (cred->addr.in.sa_family == AF_INET6) {
                    char ip[INET6_ADDRSTRLEN]{};
                    if (nai_inet_ntop(AF_INET6, &(cred->addr.in6.sin6_addr), ip, sizeof(ip)) == nullptr) {
                        ComLogWarning("iam grant failed, invalid ipv6 address");
                        return -1;
                    }
                    ara::core::String ipv6(ip);
                    ara::core::String lipv6{};
                    auto port{cred->addr.in6.sin6_port};
                    int32_t const lport{-1};
                    ComLogDebug("iam grant", GenArg(ipv6), GenArg(port));
                    if (!ara::iam::internal::grant::IAMGrantComQuery::HasEventRemoteIpGrant(ipv6, port, lipv6, lport,
                                                                                            serv, inst, eid)) {
                        ComLogDebug("iam grant forbidden");
                        return -1;
                    }
                } else {
                    ComLogWarning("iam grant failed, invalid address type");
                    return -1;
                }
                break;
            case ICC_CRED_LOCAL:  // NOLINT -- compatibility > [bugprone-branch-clone]
                break;
            case ICC_CRED_CERT:
                break;
            default:
                break;
        }
        ComLogTrace("iam grant success");
        return 0;
    }
#else
#endif
    /// @brief Receive subscription event -- skeleton side
    /// @param[in] a Implementation layer application
    /// @param[in] ud User data
    /// @param[in] cid Client identifier
    /// @param[in] serv Service identifier
    /// @param[in] inst Instance identifier
    /// @param[in] gid Event group identifier
    /// @param[in] eid Event identifier
    /// @param[in] cred Credential information
    /// @return Result -- 0: success; !0: failure;
    static int32_t OnSubscribe(icc_app_t* a,
                               void* ud,
                               icc_cid_t cid,
                               icc_serv_t serv,
                               icc_inst_t inst,
                               icc_gid_t gid,
                               icc_eid_t eid,
                               icc_cred_t const* cred) noexcept
    {
        std::ignore = a;
        std::ignore = ud;
        std::ignore = cred;
        ComLogDebug("on subscribe event", GenArg(cid), GenArg(serv), GenArg(inst), GenArg(gid), GenArg(eid));
        auto* thisObj{runtime::GetInstance().impl_};
        return thisObj->_handleSubscribe(cid, serv, inst, gid, eid);
    }
    /// @brief Process subscription event -- skeleton side
    /// @param[in] cid Client identifier
    /// @param[in] serviceId Service identifier
    /// @param[in] instanceId Instance identifier
    /// @param[in] gid Event group identifier
    /// @param[in] eid Event identifier
    /// @return Result -- 0: success; !0: failure;
    int32_t _handleSubscribe(Message::ClientId cid,
                             ServiceInfo::ServiceId serviceId,
                             ServiceInfo::InstanceId instanceId,
                             ServiceInfo::EventGroupId gid,
                             ServiceInfo::EventId eid) noexcept
    {
        ComLogTrace("handle subscribe event", GenArg(cid), GenArg(serviceId), GenArg(instanceId), GenArg(gid),
                    GenArg(eid));
        // TODO(jiawei-l6) check iam && gid && eid
        return ICC_CHECK(icc_app_subscribe_ack(&app_, Message::kReturnCodeOk, cid, serviceId, instanceId, gid, eid),
                         "handle subscribe event error: invoke stack subscribe ack failed", GenArg(cid),
                         GenArg(serviceId), GenArg(instanceId), GenArg(gid), GenArg(eid));
    }
    /// @brief Receive subscription event acknowledgment -- proxy side
    /// @param[in] a Implementation layer application
    /// @param[in] ud User data
    /// @param[in] rcode Return code
    /// @param[in] serv Service identifier
    /// @param[in] inst Instance identifier
    /// @param[in] gid Event group identifier
    /// @param[in] eid Event identifier
    /// @return Result -- 0: success; !0: failure;
    static int32_t OnSubscribeAck(
        icc_app_t* a, void* ud, int32_t rcode, icc_serv_t serv, icc_inst_t inst, icc_gid_t gid, icc_eid_t eid) noexcept
    {
        std::ignore = a;
        std::ignore = ud;
        ComLogDebug("on subscribe event ack", GenArg(rcode), GenArg(serv), GenArg(inst), GenArg(gid), GenArg(eid));
        auto* thisObj{runtime::GetInstance().impl_};
        return thisObj->_handleSubscribeAck(rcode, serv, inst, gid, eid);
    }
    /// @brief Process subscription event acknowledgment -- proxy side
    /// @param[in] rcode Return code
    /// @param[in] serviceId Service identifier
    /// @param[in] instanceId Instance identifier
    /// @param[in] gid Event group identifier
    /// @param[in] eid Event identifier
    /// @return Result -- 0: success; !0: failure;
    int32_t _handleSubscribeAck(int32_t rcode,
                                ServiceInfo::ServiceId serviceId,
                                ServiceInfo::InstanceId instanceId,
                                ServiceInfo::EventGroupId gid,
                                ServiceInfo::EventId eid) noexcept
    {
        ComLogTrace("handle subscribe event ack", GenArg(rcode), GenArg(serviceId), GenArg(instanceId), GenArg(gid),
                    GenArg(eid));
        ara::core::Vector< std::function< void(int32_t) > > callbacks;
        {
            std::lock_guard< std::mutex > lock(activeGroupsMutex_);
            auto itGroups{activeGroups_.find({serviceId, instanceId})};
            if (itGroups == activeGroups_.end()) {
                ComLogWarning("handle subscribe event ack ignored: find subscriber(service) failed", GenArg(rcode),
                              GenArg(serviceId), GenArg(instanceId), GenArg(gid), GenArg(eid));
                return -__LINE__;
            }
            auto& groupInfo{itGroups->second};
            if (eid == Message::kMethodIdAny) {
                // Notify all events belonging to this event group
                auto itGid2Ids{groupInfo.gid2Ids.find(gid)};
                if (itGid2Ids == groupInfo.gid2Ids.end()) {
                    ComLogWarning("handle subscribe event ack ignored: find subscriber(group) failed", GenArg(rcode),
                                  GenArg(serviceId), GenArg(instanceId), GenArg(gid), GenArg(eid));
                    return -__LINE__;
                }
                auto& eventIds{itGid2Ids->second};
                for (auto it{eventIds.begin()}; it != eventIds.end(); ++it) {
                    auto& eventId{*it};
                    auto itAckId2Listener{groupInfo.ackId2Listener.find(eventId)};
                    if (itAckId2Listener == groupInfo.ackId2Listener.end()) {
                        continue;
                    }
                    auto& listenerCB{itAckId2Listener->second};
                    callbacks.emplace_back(listenerCB);
                }
            } else {
                // Notify specified event belonging to this event group
                auto itAckId2Listener{groupInfo.ackId2Listener.find(eid)};
                if (itAckId2Listener == groupInfo.ackId2Listener.end()) {
                    ComLogWarning("handle subscribe event ack ignored: find subscriber(event) failed", GenArg(rcode),
                                  GenArg(serviceId), GenArg(instanceId), GenArg(gid), GenArg(eid));
                    return -__LINE__;
                }
                auto& listenerCB{itAckId2Listener->second};
                callbacks.emplace_back(listenerCB);
            }
        }
        for (auto it{callbacks.begin()}; it != callbacks.end(); ++it) {
            auto& callback{*it};
            callback(rcode);
        }
        return 0;
    }

public:
    /// @brief Send message
    /// @param[in] m Message
    /// @return Result -- 0: success; !0: failure;
    int32_t SendMessage(icc_message_t* m) noexcept
    {
        TimeRecorder(TimeNM::Timer, "ICCSendMessage");
        ComLogTrace("send message", GenArg(m));
        auto ret{ICC_CHECK(icc_app_send(&app_, m), "send message error: invoke stack send failed", GenArg(m))};
        if (ret < 0) {
            std::ignore = icc_message_close(m);
        }
        return ret;
    }

private:
#ifdef HAS_ARA_IAM
    /// @brief Receive message
    /// @param[in] a Implementation layer application
    /// @param[in] ud User data
    /// @param[in] m Message
    /// @return Result -- 0: success; !0: failure;
    static int32_t OnIAMMessage(icc_app_t* a, void* ud, icc_message_t* m) noexcept
    {
        std::ignore = a;
        std::ignore = ud;
        ComLogDebug("on iam message", GenArg(m));
        if (m == nullptr) {
            ComLogWarning("iam grant failed, invalid message");
            return -1;
        }
        switch (m->cred.type) {
            case ICC_CRED_PID:
                // method grant check.
                if (Message::IsRequest(m->hdr.type) || Message::IsRequestNoReturn(m->hdr.type)) {
                    if (!ara::iam::internal::grant::IAMGrantComQuery::HasMethodGrant(m->cred.pid, m->hdr.serv, m->inst,
                                                                                     m->hdr.method)) {
                        ComLogDebug("iam method grant forbidden");
                        return -1;
                    }
                    ComLogTrace("iam method grant success");
                }
                // event notify send grant check.
                if (Message::IsEvent(m->hdr.type)) {
                    if (!ara::iam::internal::grant::IAMGrantComQuery::HasEventGrant(m->cred.pid, m->hdr.serv, m->inst,
                                                                                    m->hdr.method)) {
                        ComLogDebug("iam event grant forbidden");
                        return -1;
                    }
                    ComLogTrace("iam event send grant success");
                }
                break;
            case ICC_CRED_ADDRESS:
                if (m->cred.addr.in.sa_family == AF_INET) {
                    char ip[INET_ADDRSTRLEN]{};
                    if (nai_inet_ntop(AF_INET, &(m->cred.addr.in4.sin_addr), ip, sizeof(ip)) == nullptr) {
                        ComLogWarning("iam grant failed, invalid ipv4 address");
                        return -1;
                    }
                    // test local ip start--->
                    char ipl[INET_ADDRSTRLEN]{};
                    if (nai_inet_ntop(PF_INET, &(m->cred.addr.in), ipl, sizeof(ipl)) == nullptr) {
                        ComLogWarning("iam grant failed, invalid ipv4 address");
                        return -1;
                    }
                    ara::core::String ipv{ipl};
                    // auto port{m->cred.addr.in4.sin_port};
                    ComLogDebug("iam grant local IP:", GenArg(ipv));

                    // test local ip end--->
                    ara::core::String ipv4{ip};
                    ara::core::String lipv4{};
                    auto port{m->cred.addr.in4.sin_port};
                    int32_t const lport{-1};
                    ComLogDebug("iam grant", GenArg(ipv4), GenArg(port));
                    // remote method grant check.
                    if (Message::IsRequest(m->hdr.type) || Message::IsRequestNoReturn(m->hdr.type)) {
                        if (!ara::iam::internal::grant::IAMGrantComQuery::HasMethodRemoteIpGrant(
                                ipv4, (m->reliable == 1U) ? "TCP" : "UDP", port, lipv4, lport, m->hdr.serv, m->inst,
                                m->hdr.method)) {
                            ComLogDebug("iam remote method grant forbidden");
                            return -1;
                        }
                        ComLogTrace("iam remote method grant success");
                    }
                    // remote event grant check.
                    if (Message::IsEvent(m->hdr.type)) {
                        if (!ara::iam::internal::grant::IAMGrantComQuery::HasEventRemoteIpGrant(
                                ipv4, port, lipv4, lport, m->hdr.serv, m->inst, m->hdr.method)) {
                            ComLogDebug("iam remote event grant forbidden");
                            return -1;
                        }
                        ComLogTrace("iam remote event grant success");
                    }
                } else if (m->cred.addr.in.sa_family == AF_INET6) {
                    char ip[INET6_ADDRSTRLEN]{};
                    if (nai_inet_ntop(AF_INET6, &(m->cred.addr.in6.sin6_addr), ip, sizeof(ip)) == nullptr) {
                        ComLogWarning("iam grant failed, invalid ipv6 address");
                        return -1;
                    }
                    ara::core::String ipv6(ip);
                    ara::core::String lipv6{};
                    auto port{m->cred.addr.in6.sin6_port};
                    int32_t const lport{-1};
                    ComLogDebug("iam grant", GenArg(ipv6), GenArg(port));
                    // remote method grant check.
                    if (Message::IsRequest(m->hdr.type) || Message::IsRequestNoReturn(m->hdr.type)) {
                        if (!ara::iam::internal::grant::IAMGrantComQuery::HasMethodRemoteIpGrant(
                                ipv6, (m->reliable == 1U) ? "TCP" : "UDP", port, lipv6, lport, m->hdr.serv, m->inst,
                                m->hdr.method)) {
                            ComLogDebug("iam remote method grant forbidden");
                            return -1;
                        }
                        ComLogTrace("iam remote method grant success");
                    }
                    // remote event grant check.
                    if (Message::IsEvent(m->hdr.type)) {
                        if (!ara::iam::internal::grant::IAMGrantComQuery::HasEventRemoteIpGrant(
                                ipv6, port, lipv6, lport, m->hdr.serv, m->inst, m->hdr.method)) {
                            ComLogDebug("iam remote event grant forbidden");
                            return -1;
                        }
                        ComLogTrace("iam remote event grant success");
                    }
                } else {
                    ComLogWarning("iam grant failed, invalid address type");
                    return -1;
                }
                break;
            case ICC_CRED_LOCAL:  // NOLINT -- compatibility > [bugprone-branch-clone]
                break;
            case ICC_CRED_CERT:
                break;
            default:
                break;
        }
        ComLogTrace("iam grant success");
        return 0;
    }
#else
#endif
};

IccRuntime::ProxyMapperBase::ProxyMapperBase() noexcept { ComLogTrace(""); }
IccRuntime::ProxyMapperBase::~ProxyMapperBase() noexcept { ComLogTrace(""); }
IccRuntime::SkeletonMapperBase::SkeletonMapperBase() noexcept { ComLogTrace(""); }
IccRuntime::SkeletonMapperBase::~SkeletonMapperBase() noexcept { ComLogTrace(""); }

IccRuntime::ServiceRequestedHolder::ServiceRequestedHolder(ServiceInfo::InstanceUID instanceUIDIn) noexcept
    : instanceUID{std::move(instanceUIDIn)}
{
    auto& bindRuntime{runtime::GetInstance()};
    bindRuntime.impl_->RequestService(instanceUID);
}
IccRuntime::ServiceRequestedHolder::~ServiceRequestedHolder() noexcept
{
    auto& bindRuntime{runtime::GetInstance()};
    bindRuntime.impl_->ReleaseService(instanceUID);
}

IccRuntime::IccRuntime() noexcept { ComLogTrace("create bind runtime"); }
IccRuntime::~IccRuntime() noexcept { ComLogTrace("destroy bind runtime"); }
ara::core::Result< void > IccRuntime::Initialize() noexcept
{
    using Result = ara::core::Result< void >;
    ComLogTrace("initialize bind runtime");
    Result result{};
    if (impl_ != nullptr) {
        ComLogError("initialize bind runtime repeat");
        result.EmplaceError(MakeErrorCode(ComErrc::kCommunicationStackError, -__LINE__));
        return result;
    }
    auto impl{std::make_unique< Impl >()};
    impl_ = impl.get();
    if (auto res{impl_->Initialize()}) {
        ;  // do nothing
    } else {
        result = std::move(res);
    }
    auto& comRuntime{internal::GetInstance()};
    comRuntime.RegisterBindRuntime(std::move(impl));
    return result;
}
ara::core::Result< void > IccRuntime::Deinitialize() noexcept
{
    using Result = ara::core::Result< void >;
    ComLogTrace("deinitialize bind runtime");
    Result result{};
    if (impl_ == nullptr) {
        ComLogError("deinitialize bind runtime repeat");
        result.EmplaceError(MakeErrorCode(ComErrc::kCommunicationStackError, -__LINE__));
        return result;
    }
    if (auto res{impl_->Deinitialize()}) {
        ;  // do nothing
    } else {
        result = std::move(res);
    }
    impl_ = nullptr;
    return result;
}
void IccRuntime::_registerProxyMapper(std::shared_ptr< ProxyMapperBase > const& mapper) noexcept
{
    impl_->RegisterProxyMapper(mapper);
}
void IccRuntime::_registerSkeletonMapper(std::shared_ptr< SkeletonMapperBase > const& mapper) noexcept
{
    impl_->RegisterSkeletonMapper(mapper);
}
ServiceInfo* IccRuntime::GetPServiceInfo(decltype(ServiceInfo::serviceId) serviceId,
                                         decltype(ServiceInfo::instanceId) instanceId) const noexcept
{
    return impl_->GetPServiceInfo(serviceId, instanceId);
}
ServiceInfo* IccRuntime::GetRServiceInfo(decltype(ServiceInfo::serviceId) serviceId,
                                         decltype(ServiceInfo::instanceId) instanceId) const noexcept
{
    return impl_->GetRServiceInfo(serviceId, instanceId);
}
icc_app_t* IccRuntime::GetApp() noexcept { return impl_->GetApp(); }
icc_appcb_t* IccRuntime::GetAppCB() noexcept { return impl_->GetAppCB(); }
int32_t IccRuntime::OfferService(ServiceInfo::InstanceUID const& instanceUID) noexcept
{
    return impl_->OfferService(instanceUID);
}
int32_t IccRuntime::StopService(ServiceInfo::InstanceUID const& instanceUID) noexcept
{
    return impl_->StopService(instanceUID);
}
int32_t IccRuntime::SubscribeEvent(ServiceInfo::ServiceId serviceId,
                                   ServiceInfo::InstanceId instanceId,
                                   ServiceInfo::MajorVersion major,
                                   ServiceInfo::EventId eid,
                                   icc_event_info_t* ev,
                                   std::function< void(int32_t rcode) >&& callback) noexcept
{
    return impl_->Subscribe(serviceId, instanceId, major, eid, ev, std::forward< decltype(callback) >(callback));
}
int32_t IccRuntime::UnsubscribeEvent(ServiceInfo::ServiceId serviceId,
                                     ServiceInfo::InstanceId instanceId,
                                     ServiceInfo::MajorVersion major,
                                     ServiceInfo::EventId eid,
                                     icc_event_info_t* ev) noexcept
{
    return impl_->Unsubscribe(serviceId, instanceId, major, eid, ev);
}
int32_t IccRuntime::Send(icc_message_t* m) noexcept { return impl_->SendMessage(m); }
}  // namespace icc
}  // namespace internal
}  // namespace com
}  // namespace ara