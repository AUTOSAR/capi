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
/// @file       execution_client.cpp
/// @brief      Execution client
/// @details
/// @date       2022-01-01
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path = /ExecutionManagement/ExecutionClient
/// @unit_name = ExecutionClient
/// @unit_description = The ExecutionClient class is used to report the execution state of processes to the execution management system.
/// @interface_level = software
/// @endcode
///
/// ================================================================

#include "ara/exec/execution_client.h"

#include <ara/core/result.h>
#include <isoft/ara_fsh/filesystem_hierarchy.h>

#include <memory>

#include "ara/exec/exec_error_domain.h"
#include "ara/exec/internal/config/execution_manifest.h"
#include "ara/exec/internal/ems/client.h"
#include "ara/exec/internal/log/log.h"

#ifdef LOG
    #undef LOG
#endif

/// @brief Execution client log macro definition
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
#define LOG() internal::log1::Log< ExecutionClient::PrivateImpl >()

namespace ara {
namespace exec {

/// @brief Execution client private implementation
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
class ExecutionClient::PrivateImpl
{
public:
    /// @brief char type redefinition
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    using Char8_t = char;

    /// @brief Get the log context ID
    /// @return Log context ID
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    static constexpr Char8_t const* GetLogCtxId() noexcept { return "ECLT"; }

    /// @brief Get the log context description
    /// @return Log context description
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    static constexpr Char8_t const* GetLogCtxDesc() noexcept { return "The Client of Execution Management"; }

    /// @brief Constructor
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    PrivateImpl() noexcept;

    /// @brief Whether valid
    /// @return true valid; false invalid
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    bool IsValid() const noexcept { return isValid_; }

    /// @brief Set validity
    /// @param isValid true valid; false invalid
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    void SetValidity(bool const isValid) noexcept { isValid_ = isValid; }

    /// @brief Open the client
    /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    int32_t Open() noexcept { return emsClient_.Open(); }

    /// @brief Close the client
    /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    int32_t Close() noexcept { return emsClient_.Close(); }

    /// @brief Report execution state
    /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    std::int32_t ReportRunning() noexcept { return emsClient_.ReportRunning(); }

private:
    /// @brief Execution client communication protocol Client object
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    internal::ems::Client emsClient_{};

    /// @brief Indicates whether the current client can report state
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    bool isValid_{false};
};

/// @brief Constructor
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
ExecutionClient::PrivateImpl::PrivateImpl() noexcept
{
    /// @code{.isoft}
    /// @trace_id_sws=SWS_EM_01403
    /// @endcode
    /// When called by a non-reporting process, ara::exec::ExecutionClient::ReportExecutionState returns error kCommunicationError
    isoft::ara_fsh::Process const fsh;
    ara::core::String const execManifestPath{ara::core::String{fsh.GetExecutionManifest()}};
    if (execManifestPath.empty()) {
        LOGW() << "No ExecutionManifest found !!!";
        return;
    }

    internal::config::ExecutionManifest execManifest;
    if (0 != execManifest.Load(execManifestPath)) {
        LOGW() << "Load ExecutionManifest {" << execManifestPath << "} failed !!!";
        return;
    }

    if (execManifest.HasConfigError()) {
        LOGW() << "Load ExecutionManifest {" << execManifestPath << "} failed !!!";
        return;
    }

    if (true != execManifest.IsReportingProcess()) {
        LOGW() << "Non-Reporting Process should not report Running State !!!";
        return;
    }

    isValid_ = true;
}

/// @brief Constructor
/// @code{.isoft}
/// @interface_level=software
/// @trace_id_sws=SWS_EM_2030
/// @trace_id_ad=AD_EM_00029
/// @trace_id_dd=DD_EM_00773
/// @needwork = dda
/// @endcode
ExecutionClient::ExecutionClient() noexcept
{
    privateImpl_ = std::make_shared< PrivateImpl >();
    if (nullptr == privateImpl_) {
        LOGE() << "ExecutionClient(): create failed !!!";
        return;
    }

    if (!privateImpl_->IsValid()) {
        LOGE() << "ExecutionClient(): is invalid !!!";
        return;
    }

    if (0 != privateImpl_->Open()) {
        privateImpl_->SetValidity(false);
        LOGE() << "ExecutionClient(): Open failed !!!";
        return;
    }
}

/// @brief Destructor
/// @code{.isoft}
/// @interface_level=software
/// @trace_id_sws=SWS_EM_2002
/// @trace_id_ad=AD_EM_00029
/// @trace_id_dd=DD_EM_00774
/// @needwork = dda
/// @endcode
ExecutionClient::~ExecutionClient() noexcept
{
    if (privateImpl_ == nullptr) {
        return;
    }

    if (privateImpl_->IsValid()) {
        privateImpl_->SetValidity(false);
        std::ignore = privateImpl_->Close();
    }
}

/// @brief Report the execution state of the process
/// @param state Execution state enumeration value of the process
/// @return Status result
/// @code{.isoft}
/// @interface_level=software
/// @trace_id_sws=SWS_EM_2003
/// @trace_id_ad=AD_EM_00029
/// @trace_id_dd=DD_EM_00779
/// @needwork = dda
/// @endcode
ara::core::Result< void > ExecutionClient::ReportExecutionState(ExecutionState const state) const noexcept
{
    if (nullptr == privateImpl_) {
        return ara::core::Result< void >::FromError(ara::exec::ExecErrc::kGeneralError);
    }
    /// @code{.isoft}
    /// @trace_id_sws=SWS_EM_01403
    /// @endcode
    /// When called by a non-reporting process, ara::exec::ExecutionClient::ReportExecutionState returns error kCommunicationError
    if (!privateImpl_->IsValid()) {
        LOGE() << "Process is configured as non-reporting process, should not report execution state !";
        return ara::core::Result< void >::FromError(ara::exec::ExecErrc::kCommunicationError);
    }

    if (ExecutionState::kTerminating == state) {
        return ara::core::Result< void >::FromValue();
    }

    if (ExecutionState::kRunning != state) {
        return ara::core::Result< void >::FromError(ara::exec::ExecErrc::kInvalidArguments);
    }

    static bool s_HasReported{false};
    if (s_HasReported) {
        LOGW() << "Execution State has been reported, should not report again !!!";
        return ara::core::Result< void >::FromError(ara::exec::ExecErrc::kCommunicationError);
    }

    /// @code{.isoft}
    /// @trace_id_sws=SWS_EM_01401 Only allow the process to report its own state
    /// @endcode
    if (0 != privateImpl_->ReportRunning()) {
        LOGE() << "Report Running State failed !!!";
        return ara::core::Result< void >::FromError(ara::exec::ExecErrc::kCommunicationError);
    }
    s_HasReported = true;

    return ara::core::Result< void >::FromValue();
}

}  // namespace exec
}  // namespace ara
