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
/// @file       find_process_client.h
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

#ifndef ARA_EXEC_FIND_PROCESS_CLIENT_H_
#define ARA_EXEC_FIND_PROCESS_CLIENT_H_

#include <ara/core/result.h>
#include <ara/core/string.h>
#include <ara/exec/exec_error_domain.h>

#include <cstdint>
#include <memory>

namespace ara {
namespace exec {
namespace internal {

/// @brief Find process client, used for modules such as IAM to obtain the process FQN based on PID
/// @code{.isoft}
/// @interface_level=software
/// @trace_id_sr=SR_EM_10008
/// @trace_id_ad=AD_EM_00053
/// @trace_id_dd=DD_EM_00703
/// @needwork = ad
/// @endcode
class FindProcessClient final
{
public:
    /// @brief Constructor
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_EM_00053
    /// @trace_id_dd=DD_EM_00704
    /// @needwork = dda
    /// @endcode
    FindProcessClient() noexcept;

    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_EM_00053
    /// @trace_id_dd=DD_EM_00705
    /// @needwork = dda
    /// @endcode
    ~FindProcessClient() noexcept;

    /// @brief  Default copy constructor
    /// @param  other the other instance
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    FindProcessClient(FindProcessClient const& other) = delete;

    /// @brief  Default move constructor
    /// @param  other the other instance
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    FindProcessClient(FindProcessClient&& other) = delete;

    /// @brief Default copy assignment function
    /// @param other the other instance
    /// @return new find process client instance
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    FindProcessClient& operator=(FindProcessClient const& other) = delete;

    /// @brief Default move assignment function
    /// @param other the other instance
    /// @return new find process client instance
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    FindProcessClient& operator=(FindProcessClient&& other) = delete;

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
    ara::core::Result< void > FindByPid(uint32_t const pid, ara::core::String& procName) const noexcept;  // PRQA S 2024

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
    ara::core::Result< void > FindByName(ara::core::String const& procName, uint32_t& pid) const noexcept;

private:
    /// @brief Private implementation class declaration
    class PrivateImpl;

    /// @brief Private implementation
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00053
    /// @trace_id_dd=DD_EM_00708
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< PrivateImpl > privateImpl_;
};

}  // namespace internal
}  // namespace exec
}  // namespace ara

#endif  ///<  ARA_EXEC_FIND_PROCESS_CLIENT_H_
