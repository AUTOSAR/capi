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
/// @file       cg_runtime.cpp
/// @brief
/// @details
/// @date       2021-12-31
/// @author     mazelin
/// @version    1.2.0
///
/// ================================================================

#include "ara/com/internal/cg/cg_runtime.h"

#include "ara/com/internal/cg/cg_config.h"
#include "ara/com/internal/runtime.h"
#include "isoft/ara_fsh/process.h"
#include "nai/os/nai_proc.h"
#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_util.h"
namespace ara {
namespace com {
namespace internal {
namespace cg {
/// @brief global cg runtime object
std::unique_ptr< CgRuntime > g_CgRuntime{};  // NOLINT
/// @brief
/// @return Result object -- empty/value or error
ara::core::Result< void > Initialize() noexcept
{
    ComLogTrace("");
    assert(g_CgRuntime == nullptr);
    g_CgRuntime = std::make_unique< CgRuntime >();
    return g_CgRuntime->Initialize();
}

/// @brief
/// @return Result object -- empty/value or error
ara::core::Result< void > Deinitialize() noexcept
{
    ComLogTrace("");
    assert(g_CgRuntime != nullptr);
    auto runtime{std::move(g_CgRuntime)};
    return runtime->Deinitialize();
}

namespace runtime {
/// @brief
/// @return
CgRuntime& GetInstance() noexcept
{
    assert(ara::com::internal::cg::g_CgRuntime != nullptr);
    return *ara::com::internal::cg::g_CgRuntime;
}

/// @brief
/// @return
int32_t LoadCgConf() noexcept
{
    isoft::ara_fsh::Process proc;
    auto manifestFile{proc.GetConfig("cg_manifest.json")};
    if (!CgMappingManager::LoadConfig(manifestFile.c_str())) {
        ComLogError("LoadConfig", GenArg(manifestFile), "failed");
        return -1;
    }
    ComLogDebug("LoadConfig done", GenArg(manifestFile));
    return 0;
}

}  // namespace runtime

/// @brief
CgRuntime::CgRuntime() noexcept { ComLogTrace(""); }
/// @brief
CgRuntime::~CgRuntime() noexcept { ComLogTrace(""); }
/// @brief
// CgRuntime::CgRuntime(CgRuntime&& other) noexcept : cgServiceList_{std::move(other.cgServiceList_)} { ComLogTrace(""); }
/// @brief move assignment operator of the CgRuntime
/// @param[in] other the object to be move.
/// @return CgRuntime object
CgRuntime& CgRuntime::operator=(CgRuntime&& other) noexcept
{
    cgServiceList_ = std::move(other.cgServiceList_);
    return *this;
}
/// @brief
/// @return Result object -- empty/value or error
ara::core::Result< void > CgRuntime::Initialize() noexcept
{
    using Result = ara::core::Result< void >;
    ComLogTrace("");
    if (nai_init() < 0) {
        return Result{MakeErrorCode(ComErrc::kCommunicationStackError, -__LINE__)};
    }
    auto ret{CG_CHECK(runtime::LoadCgConf(), "load config failed")};
    if (ret < 0) {
        return Result{MakeErrorCode(ComErrc::kCommunicationStackError, -__LINE__)};
    }
    return Result{};
}

/// @brief
/// @return Result object -- empty/value or error
ara::core::Result< void > CgRuntime::Deinitialize() noexcept
{
    using Result = ara::core::Result< void >;
    ComLogTrace("");
    return Result{};
}

/// @brief
/// @return bool
bool CgRuntime::StartCgService() noexcept
{
    ComLogDebug("Start CommunicationGroupService ...");
    bool ret{true};
    for (auto const& it : cgServiceList_) {
        auto is{GetServerInstanceSpecifier(it.first.ToString())};
        if (is.length() == static_cast< size_t >(0)) {
            ret = false;
            break;
        }
        ara::core::InstanceSpecifier instanceSpec{is};
        std::ignore = it.second->OfferService(instanceSpec);

        it.second->StartFindService();

        ara::com::InstanceIdentifierContainer instanceIDs{ara::com::runtime::ResolveInstanceIDs(instanceSpec).Value()};
        ComLogInfo("Start CommunicationGroupService ...", "InstanceSpecifier:", is, " instanceIDs:", instanceIDs);
    }
    return ret;
}

/// @brief
/// @return bool
bool CgRuntime::StopCgService() noexcept
{
    ComLogDebug("Stop CommunicationGroupService ...");
    bool ret{true};
    for (auto const& it : cgServiceList_) {
        it.second->StopFindService();
        it.second->StopOfferService();
        ComLogInfo("Stop CommunicationGroupService ...", "service name:", it.first.ToString());
    }
    return ret;
}

/// @brief
void CgRuntime::ProcessNextMethodCall() noexcept
{
    for (auto const& it : cgServiceList_) {
        std::ignore = it.second->ProcessNextMethodCall();
    }
    return;
}

/// @brief
/// @param[in] name
/// @return
ara::core::String CgRuntime::GetServerInstanceSpecifier(ara::core::StringView const& name) noexcept
{
    return CgMappingManager::GetServerInstanceSpecifier(ara::core::String(name));
}
}  // namespace cg
}  // namespace internal
}  // namespace com
}  // namespace ara