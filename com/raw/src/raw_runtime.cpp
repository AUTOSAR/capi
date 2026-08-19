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
/// @file       raw_runtime.cpp
/// @brief
/// @details
/// @date       2021-12-02
/// @author     cuijiusen
/// @version    1.2.0
///
/// ================================================================

#include "ara/com/internal/raw/raw_runtime.h"

#include "ara/com/internal/raw/raw_config.h"
#include "ara/com/internal/raw/raw_stream_ethernet.h"
#include "ara/com/internal/runtime.h"
#include "isoft/ara_fsh/process.h"
#include "nai/runtime/nai_errno.h"
#include "nai/runtime/nai_util.h"
namespace ara {
namespace com {
namespace internal {
namespace raw {
/// @brief global RawRuntime object
std::unique_ptr< RawRuntime > g_RawRuntime{};  // NOLINT

/// @brief Raw byte stream runtime initialization function
/// @return Result object -- empty/value or error
ara::core::Result< void > Initialize() noexcept
{
    ComLogTrace("");
    assert(g_RawRuntime == nullptr);
    g_RawRuntime = std::make_unique< RawRuntime >();
    ara::core::Result< void > ret{g_RawRuntime->Initialize()};
    return ret;
}

/// @brief Raw byte stream runtime deinitialization function
/// @return Result object -- empty/value or error
ara::core::Result< void > Deinitialize() noexcept
{
    ComLogTrace("");
    assert(g_RawRuntime != nullptr);
    std::unique_ptr< RawRuntime > const pRuntime{std::move(g_RawRuntime)};
    return pRuntime->Deinitialize();
}

namespace runtime {
/// @brief Raw byte stream get runtime instance function
/// @return
/// @exception
RawRuntime& GetInstance() noexcept
{
    assert(ara::com::internal::raw::g_RawRuntime != nullptr);
    return *ara::com::internal::raw::g_RawRuntime;
}

/// @brief Raw byte stream runtime load configuration function
/// @return int32_t Zero on success, non-zero otherwise.
/// @exception
int32_t LoadRawConf() noexcept
{
    isoft::ara_fsh::Process proc;
    auto manifestFile{proc.GetConfig("raw.json")};
    ComLogDebug("LoadConfig", GenArg(manifestFile));

    if (!RawMappingManager::LoadConfig(manifestFile.c_str())) {
        return -1;
    }
    return 0;
}
}  // namespace runtime

/// @brief Runtime constructor
/// @exception
RawRuntime::RawRuntime() noexcept { ComLogTrace(""); }
/// @brief Runtime destructor
/// @exception
RawRuntime::~RawRuntime() noexcept { ComLogTrace(""); }

/// @brief Raw byte stream runtime initialization function
/// @return void if successful, otherwise an error code indicating the error.
/// @exception
ara::core::Result< void > RawRuntime::Initialize() noexcept
{
    /// @brief ara::core::Result<void>
    using Result = ara::core::Result< void >;
    ComLogTrace("");
    if (nai_init() < 0) {
        return Result{MakeErrorCode(ComErrc::kCommunicationStackError, -__LINE__)};
    }
    int32_t ret{RAW_CHECK(runtime::LoadRawConf(), "load config failed")};
    if (ret < 0) {
        return Result{MakeErrorCode(ComErrc::kCommunicationStackError, -__LINE__)};
    }
    return Result::FromValue();
}

/// @brief Raw byte stream runtime deinitialization function
/// @return void if successful, otherwise an error code indicating the error.
/// @exception
ara::core::Result< void > RawRuntime::Deinitialize() noexcept
{
    /// @brief ara::core::Result<void>
    using Result = ara::core::Result< void >;
    ComLogTrace("");
    return Result::FromValue();
}
/// @brief Raw byte stream runtime create instance function
/// @param[in] instance InstanceSpecifier
/// @return instance obj pointer
/// @exception no
std::unique_ptr< BaseRaw > RawRuntime::CreateInstance(ara::core::InstanceSpecifier const& instance) noexcept
{
    std::unique_ptr< BaseRaw > newServer;
    std::shared_ptr< RawConfig > config{RawMappingManager::GetRawConfig(instance)};
    if (config != nullptr) {
        if (config->tcpPort > static_cast< uint16_t >(0)) {
            newServer = ::std::make_unique< RawEthernetTcp >(instance);
        } else if ((config->udpPort > static_cast< uint16_t >(0))
                   || (config->multicastUdp > static_cast< uint16_t >(0))) {
            newServer = ::std::make_unique< RawEthernetUdp >(instance);
        }
    }
    return newServer;
}

}  // namespace raw
}  // namespace internal
}  // namespace com
}  // namespace ara