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
/// @file       runtime.cpp
/// @brief      Communication runtime implementation file
/// @details
/// @date       2021-12-31
/// @author     mazelin
/// @version    1.2.0
///
/// ================================================================

#include "ara/com/internal/runtime.h"

#include "ara/com/com_error_domain.h"
#ifdef HAS_ARA_LOG
    #include "ara/log/internal/initialization.h"
#else
#endif
#ifdef HAS_MONITOR_LOG
    #include "ara/com/internal/trace/trace.h"
#endif
#include "isoft/ara_fsh/process.h"
#include "isoft/manifestreader/manifest_reader.h"

namespace ara {
namespace com {
namespace internal {
/// @brief Communication runtime implementation type
class Runtime::Impl
{
public:
    /// @brief Load configuration
    ara::core::Result< void > LoadConfig() noexcept  // NOLINT
    {
        using Result = ara::core::Result< void >;
        ComLogTrace("load com config", GenArg(this));
        Result result{};
        int32_t ret{};
        isoft::ara_fsh::Process proc;
        auto manifestFile{proc.GetConfig("manifest.json")};
        // constexpr auto manifestFile{"etc/manifest.json"};
        ComLogTrace("load com config", GenArg(manifestFile));
        auto manifestRes{isoft::manifestreader::OpenManifest(manifestFile.c_str())};
        if (!manifestRes) {
            ComLogError("load com config error: open config failed", GenArg(manifestRes), GenArg(manifestFile));
            result.EmplaceError(std::move(manifestRes).Error());
            return result;
        }
        auto manifest{std::move(manifestRes).Value()};
        bool hasProcessName{false};
        /// @brief Instance specifier to instance name set mapping
        ara::core::Map< ara::core::String, std::set< ara::core::String > > specifier2Names;
        std::ignore = manifest->IterateObject("", [&](auto key1, auto& node1) {
            if (ret != isoft::kSuccess) {
                return;
            }
            if (key1 == "name") {
                // Load process name
                if ((ret = node1.Load("", processName_)) != isoft::kSuccess) {
                    ComLogError("load node failed", GenArg(ret), GenArg(key1));
                    return;
                }
                if (processName_.empty()) {
                    ComLogError("load node invalid", GenArg(ret), GenArg(key1), GenArg(processName_));
                    return;
                }
                hasProcessName = true;
#ifdef HAS_MONITOR_LOG
            } else if (key1 == "fullQualifiedName") {
                // Load thread pool size
                if ((ret = node1.Load("", fullQualifiedName_)) != isoft::kSuccess) {
                    ComLogError("load node failed", GenArg(ret), GenArg(key1));
                    return;
                }
                if (fullQualifiedName_.empty()) {
                    ComLogError("load node invalid", GenArg(ret), GenArg(key1), GenArg(fullQualifiedName_));
                    return;
                }
#endif
            } else if (key1 == "thread_pool_size") {
                // Load thread pool size
                if ((ret = node1.Load("", threadPoolSize_)) != isoft::kSuccess) {
                    ComLogError("load node failed", GenArg(ret), GenArg(key1));
                    return;
                }
            } else if (key1 == "service_instance_to_port_prototype_mapping") {
                // Load instance specifier to instance name mapping set
                std::ignore = node1.IterateArray("", [&](auto idx2In, auto& node2) {
                    auto idx2{static_cast< int64_t >(idx2In)};
                    if (ret != isoft::kSuccess) {
                        return;
                    }
                    ara::core::String specifier{};
                    ara::core::String name{};
                    if ((ret = node2.Load("instance_specifier", specifier)) != isoft::kSuccess) {
                        ComLogError("load node failed", GenArg(ret), GenArg(key1), GenArg(idx2),
                                    GenVal("instance_specifier"));
                        return;
                    }
                    if ((ret = node2.Load("service_instance", name)) != isoft::kSuccess) {
                        ComLogError("load node failed", GenArg(ret), GenArg(key1), GenArg(idx2),
                                    GenVal("service_instance"));
                        return;
                    }
                    std::ignore = specifier2Names[std::move(specifier)].emplace(std::move(name));
                });
            }
        });
        if (ret != isoft::kSuccess) {
            ComLogError("load com config error: parse config failed", GenArg(ret), GenArg(manifestFile));
            result.EmplaceError(MakeErrorCode(ComErrc::kCommunicationStackError, ret));
            return result;
        }
        if (!(hasProcessName)) {
            ret = -__LINE__;
            ComLogError("load com config error: load node missed", GenArg(ret), GenArg(hasProcessName));
            result.EmplaceError(MakeErrorCode(ComErrc::kCommunicationStackError, ret));
            return result;
        }
        if (!specifier2Names.empty()) {
            auto siManifestFile{proc.GetConfig("service_instance_manifest.json")};
            // constexpr auto siManifestFile{"etc/service_instance_manifest.json"};
            ComLogTrace("load com config", GenArg(siManifestFile));
            auto siManifestRes{isoft::manifestreader::OpenManifest(siManifestFile.c_str())};
            if (!siManifestRes) {
                ComLogError("load com config error: open config failed", GenArg(siManifestRes), GenArg(siManifestFile));
                result.EmplaceError(std::move(siManifestRes).Error());
                return result;
            }
            auto siManifest{std::move(siManifestRes).Value()};
            /// @brief Instance name to instance identifier mapping -- Skeleton side
            ara::core::Map< ara::core::String, ara::core::String > pName2Identifier;
            /// @brief Instance name to instance identifier mapping -- Proxy side
            ara::core::Map< ara::core::String, ara::core::String > rName2Identifier;
            std::ignore = siManifest->IterateObject("ap_service_instances", [&](auto& key1, auto& node1) {
                if (ret != isoft::kSuccess) {
                    return;
                }
                if (key1 == "provided_ap_service_instances") {
                    // Load instance name to instance identifier mapping set (P)
                    std::ignore = node1.IterateArray("", [&](auto idx2In, auto& node2) {
                        auto idx2{static_cast< int64_t >(idx2In)};
                        if (ret != isoft::kSuccess) {
                            return;
                        }
                        ara::core::String name{};
                        ara::core::String identifier{};
                        if ((ret = node2.Load("name", name)) != isoft::kSuccess) {
                            ComLogError("load node failed", GenArg(ret), GenArg(key1), GenArg(idx2), GenVal("name"));
                            return;
                        }
                        if ((ret = node2.Load("instance_id", identifier)) != isoft::kSuccess) {
                            ComLogError("load node failed", GenArg(ret), GenArg(key1), GenArg(idx2),
                                        GenVal("instance_id"));
                            return;
                        }
                        std::ignore = pName2Identifier.emplace(std::move(name), std::move(identifier));
                    });
                } else if (key1 == "required_ap_service_instances") {
                    // Load instance name to instance identifier mapping set (R)
                    std::ignore = node1.IterateArray("", [&](auto idx2In, auto& node2) {
                        auto idx2{static_cast< int64_t >(idx2In)};
                        if (ret != isoft::kSuccess) {
                            return;
                        }
                        ara::core::String name{};
                        ara::core::String identifier{};
                        if ((ret = node2.Load("name", name)) != isoft::kSuccess) {
                            ComLogError("load node failed", GenArg(ret), GenArg(key1), GenArg(idx2), GenVal("name"));
                            return;
                        }
                        if ((ret = node2.Load("instance_id", identifier)) != isoft::kSuccess) {
                            ComLogError("load node failed", GenArg(ret), GenArg(key1), GenArg(idx2),
                                        GenVal("instance_id"));
                            return;
                        }
                        std::ignore = rName2Identifier.emplace(std::move(name), std::move(identifier));
                    });
                }
            });
            if (ret != isoft::kSuccess) {
                ComLogError("load com config error: parse config failed", GenArg(ret), GenArg(siManifestFile));
                result.EmplaceError(MakeErrorCode(ComErrc::kCommunicationStackError, ret));
                return result;
            }
            size_t pMapCount{};
            size_t rMapCount{};
            for (auto const& itSpecifier2Name : specifier2Names) {
                auto specifierRes{ara::core::InstanceSpecifier::Create(itSpecifier2Name.first)};
                if (!specifierRes) {
                    ret = -__LINE__;
                    ComLogError("load com config error: load node invalid", GenArg(specifierRes),
                                GenArg(itSpecifier2Name));
                    result.EmplaceError(MakeErrorCode(ComErrc::kCommunicationStackError, ret));
                    return result;
                }
                auto specifier{std::move(specifierRes).Value()};
                auto const& names{itSpecifier2Name.second};
                for (auto const& name : names) {
                    auto itPName2Identifier{pName2Identifier.find(name)};
                    if (itPName2Identifier != pName2Identifier.end()) {
                        auto& identifier{itPName2Identifier->second};
                        pSpecifier2Identifiers_[specifier].emplace_back(std::move(identifier));
                        ++pMapCount;
                        continue;
                    }
                    auto itRName2Identifier{rName2Identifier.find(name)};
                    if (itRName2Identifier != rName2Identifier.end()) {
                        auto& identifier{itRName2Identifier->second};
                        rSpecifier2Identifiers_[specifier].emplace_back(std::move(identifier));
                        ++rMapCount;
                        continue;
                    }
                    ret = -__LINE__;
                    ComLogError("load com config error: load node missed", GenArg(ret), GenArg(name));
                    result.EmplaceError(MakeErrorCode(ComErrc::kCommunicationStackError, ret));
                    return result;
                }
            }
            if (!(pMapCount == pName2Identifier.size())) {
                ret = -__LINE__;
                ComLogError("load com config error: load node missed", GenArg(ret), GenArg(specifier2Names),
                            GenArg(pName2Identifier));
                result.EmplaceError(MakeErrorCode(ComErrc::kCommunicationStackError, ret));
                return result;
            }
            if (!(rMapCount == rName2Identifier.size())) {
                ret = -__LINE__;
                ComLogError("load com config error: load node missed", GenArg(ret), GenArg(specifier2Names),
                            GenArg(rName2Identifier));
                result.EmplaceError(MakeErrorCode(ComErrc::kCommunicationStackError, ret));
                return result;
            }
        }
#ifdef HAS_MONITOR_LOG
        auto monitorFile{proc.GetConfig("monitor.json")};
        ComLogTrace("load com config", GenArg(monitorFile));
        auto monitorRes{trace::ComTrace::Instance().LoadConfig(monitorFile.c_str())};
        if (!monitorRes) {
            ComLogError("load com config error: load config failed", GenArg(monitorRes), GenArg(monitorFile));
            result.EmplaceError(std::move(monitorRes).Error());
            return result;
        }
#endif
        if (threadPoolSize_ != 0) {
            ComLogTrace("load com config: create thread pool");
            std::ignore
                = threadPools_.emplace(processName_, std::make_unique< ThreadPool >(processName_, threadPoolSize_));
        }
        ComLogDebug("load com config done", GenArg(this));
        return result;
    }
    /// @brief Unload configuration
    ara::core::Result< void > UnloadConfig() noexcept
    {
        using Result = ara::core::Result< void >;
        ComLogTrace("unload com config", GenArg(this));
        Result result{};
        for (auto itThreadPool{threadPools_.begin()}; itThreadPool != threadPools_.end();) {
            auto const& name{itThreadPool->first};
            if (name == processName_) {
                ++itThreadPool;
                continue;
            }
            ComLogTrace("unload com config: destroy thread pool", GenArg(name));
            std::ignore = threadPools_.erase(itThreadPool++);
        }
        if (threadPoolSize_ != 0) {
            ComLogTrace("unload com config: destroy thread pool", GenArg(processName_));
            std::ignore     = threadPools_.erase(processName_);
            threadPoolSize_ = 0;
        }
        threadPools_.clear();
        bindRuntimes_.clear();
        errorDomains_.clear();
        rSpecifier2Identifiers_.clear();
        pSpecifier2Identifiers_.clear();
        threadPoolSize_ = 0;
        processName_.clear();
#ifdef HAS_MONITOR_LOG
        fullQualifiedName_.clear();
#endif
        ComLogDebug("unload com config done", GenArg(this));
        return result;
    }
    /// @brief Register error domain
    /// @param[in] domain Error domain
    void RegisterErrorDomain(ara::core::ErrorDomain const& domain) noexcept
    {
        ComLogTrace("register com error domain", GenArg(domain));
        std::ignore = errorDomains_.emplace(domain.Id(), std::cref(domain));
    }
    /// @brief Get error domain
    /// @param[in] id Error domain identifier
    /// @return Pointer to error domain
    ara::core::ErrorDomain const* GetErrorDomain(ara::core::ErrorDomain::IdType id) const noexcept
    {
        auto it{errorDomains_.find(id)};
        return it == errorDomains_.end() ? nullptr : &it->second;
    }
    /// @brief Get process name
    ara::core::StringView GetProcessName() const noexcept { return processName_; }
    /// @brief Instance specifier to instance identifier mapping set type
    using Specifier2Identifiers = ara::core::Map< ara::core::InstanceSpecifier, InstanceIdentifierContainer >;
    /// @brief Get instance specifier to instance identifier mapping set -- Skeleton side
    Specifier2Identifiers const& GetPSpecifier2Identifiers() const noexcept { return pSpecifier2Identifiers_; }
    /// @brief Get instance specifier to instance identifier mapping set -- Proxy side
    Specifier2Identifiers const& GetRSpecifier2Identifiers() const noexcept { return rSpecifier2Identifiers_; }
    /// @brief Get thread pool
    /// @param[in] name Thread pool name
    /// @return Pointer to thread pool
    std::unique_ptr< ThreadPool > const& GetThreadPool(ara::core::StringView name) const noexcept
    {
        static std::unique_ptr< ThreadPool > s_Instance{};
        auto it{threadPools_.find(name.data())};
        if (it == threadPools_.end()) {
            return s_Instance;
        }
        return it->second;
    }
    /// @brief Create thread pool
    /// @param[in] name Thread pool name
    /// @param[in] size Thread pool size
    /// @return Pointer to thread pool
    std::unique_ptr< ThreadPool > const& CreateThreadPool(ara::core::StringView name, size_t size) noexcept
    {
        ComLogTrace("create com thread pool", GenArg(name), GenArg(size));
        if (name.empty() || size == 0) {
            ComLogError("create com thread pool rejected: create name or size invalid", GenArg(name), GenArg(size));
            return GetThreadPool("");
        }
        auto& threadPool{threadPools_[name.data()]};
        if (threadPool != nullptr) {
            ComLogWarning("create com thread pool reused: create name repeat", GenArg(name));
            return threadPool;
        }
        threadPool = std::make_unique< ThreadPool >(name, size);
        ComLogDebug("create com thread pool done", GenArg(name), GenArg(size));
        return threadPool;
    }
    /// @brief Destroy thread pool
    /// @return bool
    bool DestroyThreadPool(ara::core::StringView name) noexcept
    {
        ComLogTrace("destroy com thread pool", GenArg(name));
        if (name == processName_) {
            ComLogWarning("destroy com thread pool invalid", GenArg(name));
            return false;
        }
        auto destroy{threadPools_.erase(name.data()) != 0};
        ComLogDebug("destroy com thread pool done", GenArg(name), GenArg(destroy));
        return destroy;
    }
    /// @brief Register a binding layer runtime instance
    /// @param[in] bindRuntime Binding layer runtime instance
    void RegisterBindRuntime(BindRuntime&& bindRuntime) noexcept
    {
        ComLogTrace("register bind runtime", GenArg(bindRuntime));
        bindRuntimes_.emplace_back(std::move(bindRuntime));
    }
    /// @brief Get the set of binding layer runtime instances
    BindRuntimes const& GetBindRuntimes() const noexcept { return bindRuntimes_; }
    /// @brief Get the instance specifier to which the set of instance identifiers belongs
    /// @raram[in] instanceIdentifiers Set of instance identifiers
    /// @return Instance specifier to which the set of instance identifiers belongs
    ara::core::Vector< ara::core::InstanceSpecifier > GetInstanceSpecifier(
        InstanceIdentifierContainer const& instanceIdentifiers) const noexcept
    {
        // Find which set in the set of lists the passed element set is a subset of
        std::set< ara::core::InstanceSpecifier > instanceSpecifiers;
        for (auto const& itPSpecifier2Identifier : pSpecifier2Identifiers_) {
            auto const& pInstanceSpecifier{itPSpecifier2Identifier.first};
            auto const& pInstanceIdentifiers{itPSpecifier2Identifier.second};
            bool include{false};
            for (auto const& instanceIdentifier : instanceIdentifiers) {
                if (std::find(pInstanceIdentifiers.begin(), pInstanceIdentifiers.end(), instanceIdentifier)
                    == pInstanceIdentifiers.end()) {
                    include = false;
                    break;
                }
                include = true;
            }
            if (include) {
                instanceSpecifiers.emplace(pInstanceSpecifier);
            }
        }
        for (auto const& itRSpecifier2Identifier : rSpecifier2Identifiers_) {
            auto const& rInstanceSpecifier{itRSpecifier2Identifier.first};
            auto const& rInstanceIdentifiers{itRSpecifier2Identifier.second};
            bool include{false};
            for (auto const& instanceIdentifier : instanceIdentifiers) {
                if (std::find(rInstanceIdentifiers.begin(), rInstanceIdentifiers.end(), instanceIdentifier)
                    == rInstanceIdentifiers.end()) {
                    include = false;
                    break;
                }
                include = true;
            }
            if (include) {
                instanceSpecifiers.emplace(rInstanceSpecifier);
            }
        }
        if (instanceSpecifiers.empty()) {
            ComLogError("get instance specifier not exist", GenArg(instanceIdentifiers));
        } else {
            ComLogTrace("get instance specifier exist", GenArg(instanceSpecifiers));
        }
        return {instanceSpecifiers.begin(), instanceSpecifiers.end()};
    }
#ifdef HAS_MONITOR_LOG
    /// @brief Get the full qualified name
    /// @return Full qualified name
    ara::core::String const& GetFullQualifiedName() const noexcept { return fullQualifiedName_; }
#endif

    /// @brief Type alias -- Mark as enumerable
    using IsEnumerableTag = void;
    /// @brief Enumeration operation -- enumerate members of this type via a passed function object
    /// @tparam F Passed function object type
    /// @param[in] fun Passed function object
    template < typename F >
    void enumerate(F& fun)  // NOLINT -- Template interface name > naming convention
    {
        fun("name", processName_);
#ifdef HAS_MONITOR_LOG
        fun("fqn", fullQualifiedName_);
#endif
        fun("pspec->ids", pSpecifier2Identifiers_);
        fun("rspec->ids", rSpecifier2Identifiers_);
        fun("error domains", errorDomains_);
        fun("bind runtimes", bindRuntimes_);
        fun("thread pools", threadPools_);
    }

private:
    /// @brief Type alias -- Error domain set
    using ErrorDomains = ara::core::Map< ara::core::ErrorDomain::IdType, ara::core::ErrorDomain const& >;
    /// @brief Error domain set
    ErrorDomains errorDomains_;
    /// @brief Process name
    ara::core::String processName_;
#ifdef HAS_MONITOR_LOG
    /// @brief Full qualified name
    ara::core::String fullQualifiedName_;
#endif
    /// @brief Instance specifier to instance identifier mapping set -- Skeleton side
    Specifier2Identifiers pSpecifier2Identifiers_;
    /// @brief Instance specifier to instance identifier mapping set -- Proxy side
    Specifier2Identifiers rSpecifier2Identifiers_;
    /// @brief Thread pool set type
    using ThreadPools = ara::core::Map< ara::core::String, std::unique_ptr< ThreadPool > >;
    /// @brief Thread pool set
    ThreadPools threadPools_;
    /// @brief Process thread pool size
    size_t threadPoolSize_{0};
    /// @brief Binding layer runtime instance set
    BindRuntimes bindRuntimes_;
};
/// @brief Communication runtime instance pointer
std::unique_ptr< Runtime > g_ComRuntime{};  // NOLINT
/// @brief Deserialization function set
std::list< std::function< ara::core::Result< void >() > > g_Deinitializes;  // NOLINT
ara::core::Result< void > Initialize() noexcept                             // NOLINT
{
    using Result = ara::core::Result< void >;
#ifdef HAS_ARA_LOG
#else
    std::ignore = LogNM::Logger::GetInstance< LogNM::LoggerNai >().SetNaiLog(&nai_log_core);
#endif
    ComLogTrace("initialize com runtime", GenArg(g_ComRuntime));
    Result result{};
    assert(g_ComRuntime == nullptr);
    g_ComRuntime = std::make_unique< Runtime >();
    if (auto res{g_ComRuntime->impl_->LoadConfig()}) {
        g_Deinitializes.emplace_front([&] { return g_ComRuntime->impl_->UnloadConfig(); });
        g_ComRuntime->impl_->RegisterErrorDomain(ara::core::GetCoreErrorDomain());
        g_ComRuntime->impl_->RegisterErrorDomain(ara::core::GetFutureErrorDomain());
        g_ComRuntime->impl_->RegisterErrorDomain(ara::com::GetComErrorDomain());
        g_ComRuntime->impl_->RegisterErrorDomain(ara::com::e2e::GetE2EErrorDomain());
    } else {
        std::ignore = Deinitialize();
        result      = std::move(res);
        return result;
    }
    if (auto res{g_ComRuntime->Initialize()}) {
        g_Deinitializes.emplace_front([&] { return g_ComRuntime->Deinitialize(); });
    } else {
        std::ignore = Deinitialize();
        result      = std::move(res);
        return result;
    }
    ComLogDebug("initialize com runtime done", GenArg(g_ComRuntime));
    return result;
}
ara::core::Result< void > Deinitialize() noexcept
{
    using Result = ara::core::Result< void >;
    ComLogTrace("deinitialize com runtime", GenArg(g_ComRuntime));
    Result result{};
    assert(g_ComRuntime != nullptr);
    while (!g_Deinitializes.empty()) {
        auto deinitialize{std::move(g_Deinitializes.front())};
        g_Deinitializes.pop_front();
        if (auto res{deinitialize()}) {
        } else {
            result = std::move(res);
        }
    }
    g_ComRuntime = nullptr;
    ComLogDebug("deinitialize com runtime done", GenArg(g_ComRuntime));
    return result;
}
Runtime& GetInstance() noexcept
{
    assert(g_ComRuntime != nullptr);
    return *g_ComRuntime;
}
Runtime::Runtime() noexcept : impl_{std::make_unique< Runtime::Impl >()}
{
    ComLogTrace("create com runtime", GenArg(this));
}
Runtime::~Runtime() noexcept { ComLogTrace("destroy com runtime", GenArg(this)); }
void Runtime::RegisterErrorDomain(ara::core::ErrorDomain const& domain) noexcept { impl_->RegisterErrorDomain(domain); }
ara::core::ErrorDomain const* Runtime::GetErrorDomain(ara::core::ErrorDomain::IdType id) const noexcept
{
    return impl_->GetErrorDomain(id);
}
ara::core::StringView Runtime::GetProcessName() const noexcept { return impl_->GetProcessName(); }
std::unique_ptr< ThreadPool > const& Runtime::GetThreadPool(ara::core::StringView name) const noexcept
{
    return impl_->GetThreadPool(name);
}
std::unique_ptr< ThreadPool > const& Runtime::CreateThreadPool(ara::core::StringView name, size_t size) noexcept
{
    return impl_->CreateThreadPool(name, size);
}
bool Runtime::DestroyThreadPool(ara::core::StringView name) noexcept { return impl_->DestroyThreadPool(name); }
void Runtime::RegisterBindRuntime(BindRuntime&& bindRuntime) noexcept
{
    impl_->RegisterBindRuntime(std::move(bindRuntime));
}
Runtime::BindRuntimes const& Runtime::GetBindRuntimes() const noexcept { return impl_->GetBindRuntimes(); }
ara::core::Vector< ara::core::InstanceSpecifier > Runtime::GetInstanceSpecifier(
    InstanceIdentifierContainer const& instanceIdentifiers) const noexcept
{
    return impl_->GetInstanceSpecifier(instanceIdentifiers);
}
#ifdef HAS_MONITOR_LOG
ara::core::String const& Runtime::GetFullQualifiedName() const noexcept { return impl_->GetFullQualifiedName(); }
#endif
BindRuntime::BindRuntime() noexcept { ComLogTrace("create bind runtime", GenArg(this)); }
BindRuntime::~BindRuntime() noexcept { ComLogTrace("destroy bind runtime", GenArg(this)); }
}  // namespace internal
namespace runtime {
ara::core::Result< InstanceIdentifierContainer > ResolveInstanceIDs(
    ara::core::InstanceSpecifier const& instanceSpecifier) noexcept
{
    using Result = ara::core::Result< InstanceIdentifierContainer >;
    ComLogTrace("resolve instance IDs", GenArg(instanceSpecifier));
    auto& impl{internal::GetInstance().impl_};
    auto const& pSpecifier2Identifiers{impl->GetPSpecifier2Identifiers()};
    auto itPSpecifier2Identifier{pSpecifier2Identifiers.find(instanceSpecifier)};
    if (itPSpecifier2Identifier != pSpecifier2Identifiers.end()) {
        auto const& instanceIdentifiers{itPSpecifier2Identifier->second};
        ComLogDebug("resolve instance IDs(P) done", GenArg(instanceSpecifier), GenArg(instanceIdentifiers));
        return Result{instanceIdentifiers};
    }
    auto const& rSpecifier2Identifiers{impl->GetRSpecifier2Identifiers()};
    auto itRSpecifier2Identifier{rSpecifier2Identifiers.find(instanceSpecifier)};
    if (itRSpecifier2Identifier != rSpecifier2Identifiers.end()) {
        auto const& instanceIdentifiers{itRSpecifier2Identifier->second};
        ComLogDebug("resolve instance IDs(R) done", GenArg(instanceSpecifier), GenArg(instanceIdentifiers));
        return Result{instanceIdentifiers};
    }
    ComLogWarning("resolve instance IDs failed", GenArg(instanceSpecifier));
    return Result{MakeErrorCode(ComErrc::kInstanceIDCouldNotBeResolved, -__LINE__)};
}
}  // namespace runtime
}  // namespace com
}  // namespace ara