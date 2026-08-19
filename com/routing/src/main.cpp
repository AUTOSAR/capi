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
/// @file       main.cpp
/// @brief      Routing implementation file
/// @details
/// @date       2022-06-27
/// @author     mazelin
/// @version    1.2.0
///
/// ================================================================
///
/// #undef HAS_NPC_BINDING
/// #undef HAS_NSOMEIP_BINDING
///
/// ================================================================

#include "ara/com/internal/runtime.h"
#ifdef HAS_NPC_BINDING
    #include "ara/com/internal/npc/npc_runtime.h"
#else
#endif  // HAS_NPC_BINDING
#ifdef HAS_NSOMEIP_BINDING
    #include "ara/com/internal/nsomeip/nsomeip_runtime.h"
#else
#endif  // HAS_NSOMEIP_BINDING
#ifdef HAS_ARA_LOG
    #include "ara/log/internal/initialization.h"
#else
#endif  // HAS_ARA_LOG
#ifdef HAS_ARA_EXEC_EXECUTION_CLIENT
    #include "ara/exec/execution_client.h"
    #include "isoft/core/ipc/ipc.h"
#else
#endif  // HAS_ARA_EXEC_EXECUTION_CLIENT
#include "isoft/naicpp/terminating_handler.h"

namespace ara {
namespace com {
namespace internal {
/// @brief Initialization -- routing side
/// @return Result object -- empty/value or error
ara::core::Result< void > Runtime::Initialize() noexcept
{
    using Result = ara::core::Result< void >;
    Result result{};
#ifdef HAS_NPC_BINDING
    if (auto res{ara::com::internal::npc::Initialize(true)}) {
    } else {
        result = std::move(res);
    }
#else
#endif  // HAS_NPC_BINDING
#ifdef HAS_NSOMEIP_BINDING
    if (auto res{ara::com::internal::nsomeip::Initialize(true)}) {
    } else {
        result = std::move(res);
    }
#else
#endif  // HAS_NSOMEIP_BINDING
    return result;
}
/// @brief Deinitialization -- routing side
/// @return Result object -- empty/value or error
ara::core::Result< void > Runtime::Deinitialize() noexcept
{
    using Result = ara::core::Result< void >;
    Result result{};
#ifdef HAS_NPC_BINDING
    if (auto res{ara::com::internal::npc::Deinitialize()}) {
    } else {
        result = std::move(res);
    }
#else
#endif  // HAS_NPC_BINDING
#ifdef HAS_NSOMEIP_BINDING
    if (auto res{ara::com::internal::nsomeip::Deinitialize()}) {
    } else {
        result = std::move(res);
    }
#else
#endif  // HAS_NSOMEIP_BINDING
    return result;
}
}  // namespace internal
}  // namespace com
}  // namespace ara

/// @brief Routing type
/// @code{.isoft}
/// export_level=/COM/SoC/SOA
/// @endcode
class Routing
{
public:
    /// @brief Destructor
    ~Routing() noexcept { isoft::naicpp::GlobalGeneralEvLoop::Deinitialize(); }
    /// @brief Constructor
    Routing() noexcept = default;
    /// @brief Copy constructor
    /// @param other
    Routing(Routing const& other) noexcept = delete;
    /// @brief Move constructor
    /// @param other
    Routing(Routing&& other) noexcept = default;
    /// @brief Copy assignment function
    /// @param other
    /// @return Routing
    Routing& operator=(Routing const& other) noexcept = delete;
    /// @brief Move assignment function
    /// @param other
    /// @return Routing
    Routing& operator=(Routing&& other) noexcept = default;

public:
    /// @brief Start routing
    /// @code{.isoft}
    /// export_level=/COM/SoC/SOA
    /// @endcode
    /// @return Result -- 0: success; !0: failure;
    int32_t Start() noexcept
    {
        // FmtNM::EnableColor(true);
        // ComLog().EnableLogger(false);
        if (!(result_ = isoft::naicpp::GlobalGeneralEvLoop::Initialize(
                  isoft::naicpp::GlobalGeneralEvLoop::Mode::kUserThread))) {
            ComLogError("initialize evloop failed", GenArg(result_));
            return Stop();
        }
#ifdef HAS_ARA_EXEC_EXECUTION_CLIENT
        if (!(result_ = isoft::core::Ipc::Initialize())) {
            ComLogError("initialize ipc failed", GenArg(result_));
            return Stop();
        }
        deinitializes_.emplace_back(isoft::core::Ipc::Deinitialize);
#else
#endif  // HAS_ARA_EXEC_EXECUTION_CLIENT
#ifdef HAS_ARA_LOG
        if (!(result_ = ara::log::internal::Initialize())) {
            ComLogError("initialize log failed", GenArg(result_));
            return Stop();
        }
        deinitializes_.emplace_back(ara::log::internal::Deinitialize);
#else
#endif  // HAS_ARA_LOG
        if (!(result_ = ara::com::internal::Initialize())) {
            ComLogError("initialize com failed", GenArg(result_));
            return Stop();
        }
        deinitializes_.emplace_back(ara::com::internal::Deinitialize);
        if (ara::com::internal::GetInstance().GetBindRuntimes().empty()) {
            ComLogError("start routing failed, no binding");
            result_.EmplaceError(ara::com::ComErrc::kNetworkBindingFailure, -__LINE__);
            return Stop();
        }
        isoft::naicpp::TerminatingHandler terminatingHandler{};
        auto ret{terminatingHandler.Register()};
        if (ret != 0) {
            ComLogError("register terminating handler failed", GenArg(ret));
            result_.EmplaceError(ara::com::ComErrc::kCommunicationStackError, -__LINE__);
            return Stop();
        }
#ifdef HAS_ARA_EXEC_EXECUTION_CLIENT
        ara::exec::ExecutionClient{}.ReportExecutionState(ara::exec::ExecutionState::kRunning);
        ComLogInfo("report execution state running");
#else
#endif  // HAS_ARA_EXEC_EXECUTION_CLIENT
        ret = isoft::naicpp::GlobalGeneralEvLoop::Get()->Run(true);
        if (ret != 0) {
            ComLogError("run evloop failed", GenArg(ret));
            result_.EmplaceError(ara::com::ComErrc::kCommunicationStackError, -__LINE__);
            return Stop();
        }
        return Stop();
    }
    /// @brief Stop routing
    /// @code{.isoft}
    /// export_level=/COM/SoC/SOA
    /// @endcode
    /// @return Result -- 0: success; !0: failure;
    int32_t Stop() noexcept
    {
        for (auto it{deinitializes_.rbegin()}; it != deinitializes_.rend(); ++it) {
            auto& deinitialize{*it};
            auto res{deinitialize()};
            if (!res) {
                result_ = std::move(res);
                ComLogError("deinitialize failed", GenArg(result_));
            }
        }
        deinitializes_.clear();
        return !result_ ? -__LINE__ : 0;
    }

private:
    /// @brief Result object -- empty/value or error
    ara::core::Result< void > result_;
    /// @brief Deinitialization callback function set
    ara::core::Vector< std::function< ara::core::Result< void >() > > deinitializes_;
};

int main() { return Routing().Start(); }