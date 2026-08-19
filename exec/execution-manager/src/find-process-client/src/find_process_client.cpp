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
/// @file       find_process_client.cpp
/// @brief      Process find client
/// @details
/// @date       2022-09-13
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/ExecutionManagement/FindProcessClient
/// @unit_name=FindProcessClient
/// @unit_description=Used to find process information by PID or FQN.
/// @interface_level=software
/// @endcode
///
/// ================================================================

#include "ara/exec/internal/find_process_client.h"

#include <ara/core/result.h>
#include <isoft/ara_fsh/filesystem_hierarchy.h>

#include <memory>

#include "ara/exec/exec_error_domain.h"
#include "ara/exec/internal/fps/find_process.h"
#include "ara/exec/internal/log/log.h"

#ifdef LOG
    #undef LOG
#endif

/// @brief Process find client log macro definition
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
#define LOG() ara::exec::internal::log1::Log< FindProcessClient::PrivateImpl >()

namespace ara {
namespace exec {
namespace internal {

/// @brief Process find client private implementation
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
class FindProcessClient::PrivateImpl
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
    static constexpr Char8_t const *GetLogCtxId() noexcept { return "FPCT"; }

    /// @brief Get the log context description
    /// @return Log context description
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    static constexpr Char8_t const *GetLogCtxDesc() noexcept { return "The Client of Find Process Service"; }

    /// @brief Constructor
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    PrivateImpl() noexcept;

    /// @brief Get the error code
    /// @return Error code
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    ara::exec::ExecErrc GetErrorCode() const noexcept { return errorCode_; }

    /// @brief Close procFinder
    /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    int32_t Close() noexcept { return procFinder_.Close(); }

    /// @brief Find the process name by process pid
    /// @param pid Process ID
    /// @param procName Process name
    /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    std::int32_t GetNameByPid(std::uint32_t const pid, ara::core::String &procName) noexcept
    {
        return procFinder_.GetNameByPid(pid, procName);
    }

    /// @brief Find the process ID by process name
    /// @param procName Process name
    /// @param pid Process ID
    /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    int32_t GetPidByName(ara::core::String const &procName, uint32_t &pid) noexcept
    {
        return procFinder_.GetPidByName(procName, pid);
    }

private:
    /// @brief Process finder object
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    fps::FindProcess procFinder_{};

    /// @brief Error code
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    ara::exec::ExecErrc errorCode_{ara::exec::ExecErrc::kNoError};
};

/// @brief Constructor
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
FindProcessClient::PrivateImpl::PrivateImpl() noexcept
{
    int32_t const r{procFinder_.Open()};
    if (0 != r) {
        LOGE() << "procFinder_.Open(): " << r;
        errorCode_ = ara::exec::ExecErrc::kCommunicationError;
        return;
    }
}

/// @brief Constructor
/// @code{.isoft}
/// @interface_level=software
/// @trace_id_ad=AD_EM_00053
/// @trace_id_dd=DD_EM_00704
/// @needwork = dda
/// @endcode
FindProcessClient::FindProcessClient() noexcept : privateImpl_{std::make_shared< PrivateImpl >()}
{
    if (nullptr == privateImpl_) {
        LOGE() << "nullptr == std::make_shared<PrivateImpl>()";
        return;
    }
}

/// @brief Destructor
/// @code{.isoft}
/// @interface_level=software
/// @trace_id_ad=AD_EM_00053
/// @trace_id_dd=DD_EM_00705
/// @needwork = dda
/// @endcode
FindProcessClient::~FindProcessClient() noexcept
{
    if (nullptr != privateImpl_) {
        int32_t const r{privateImpl_->Close()};
        if (0 != r) {
            LOGE() << "privateImpl_->Close(): " << r;
        }
    }
}

/// @brief Find the corresponding process name by PID
/// @param pid Process PID
/// @param procName Process FQN
/// @return void success; failure returns ExecErrorDomain error
///         ara::exec::ExecErrc::kInvalidArguments The provided PID is invalid, no corresponding process name found
///         ara::exec::ExecErrc::kCommunicationError IPC communication error, unable to establish communication link with EM
///         ara::exec::ExecErrc::kGeneralError Internal error
/// @code{.isoft}
/// @interface_level=software
/// @trace_id_ad=AD_EM_00053
/// @trace_id_dd=DD_EM_00706
/// @needwork = dda
/// @endcode
ara::core::Result< void > FindProcessClient::FindByPid(uint32_t const pid, ara::core::String &procName) const noexcept
{
    if (nullptr == privateImpl_) {
        return ara::core::Result< void >::FromError(ara::exec::ExecErrc::kGeneralError);
    }

    if (privateImpl_->GetErrorCode() != ara::exec::ExecErrc::kNoError) {
        return ara::core::Result< void >::FromError(ara::exec::ExecErrc::kCommunicationError);
    }

    if (0 != privateImpl_->GetNameByPid(pid, procName)) {
        return ara::core::Result< void >::FromError(ara::exec::ExecErrc::kInvalidArguments);
    }

    return ara::core::Result< void >::FromValue();
}

/// @brief Find the corresponding pid by process name
/// @param procName Process FQN
/// @param pid Process PID
/// @return void success; failure returns ExecErrorDomain error
///         ara::exec::ExecErrc::kInvalidArguments The provided process name is invalid, no corresponding pid found
///         ara::exec::ExecErrc::kCommunicationError IPC communication error, unable to establish communication link with EM
///         ara::exec::ExecErrc::kGeneralError Internal error
/// @code{.isoft}
/// @interface_level=software
/// @trace_id_ad=AD_EM_00053
/// @trace_id_dd=DD_EM_00707
/// @needwork = dda
/// @endcode
ara::core::Result< void > FindProcessClient::FindByName(ara::core::String const &procName, uint32_t &pid) const noexcept
{
    if (nullptr == privateImpl_) {
        return ara::core::Result< void >::FromError(ara::exec::ExecErrc::kGeneralError);
    }

    if (privateImpl_->GetErrorCode() != ara::exec::ExecErrc::kNoError) {
        return ara::core::Result< void >::FromError(ara::exec::ExecErrc::kCommunicationError);
    }

    if (0 != privateImpl_->GetPidByName(procName, pid)) {
        return ara::core::Result< void >::FromError(ara::exec::ExecErrc::kInvalidArguments);
    }

    return ara::core::Result< void >::FromValue();
}

}  // namespace internal
}  // namespace exec
}  // namespace ara
