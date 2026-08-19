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
/// @file       execution_client.h
/// @brief      Execution client
/// @details
/// @date       2022-01-01
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/ExecutionManagement/ExecutionClient
/// @unit_name=ExecutionClient
/// @unit_description=Used to report the execution state of processes to the execution management system.
/// @interface_level=software
/// @endcode
///
/// ================================================================

#ifndef ARA_EXEC_EXECUTION_CLIENT_H_
#define ARA_EXEC_EXECUTION_CLIENT_H_

#include <ara/core/result.h>

#include <cstdint>
#include <memory>

namespace ara {
namespace exec {

/// @brief  Execution state enumeration type
/// @code{.isoft}
/// @interface_level=software
/// @trace_id_sr=SR_EM_10001
/// @trace_id_ad=AD_EM_00029
/// @trace_id_dd=DD_EM_00771
/// @needwork = dd
/// @endcode
enum class ExecutionState : uint8_t
{
    /// @brief Running state
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_sws=SWS_EM_2000
    /// @trace_id_ad=AD_EM_00029
    /// @trace_id_dd=DD_EM_00805
    /// @needwork = dd
    /// @endcode
    kRunning = 0,

    /// @brief Terminating state
    /// deprecated in (R20-11)
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_sws=SWS_EM_2000
    /// @trace_id_ad=AD_EM_00029
    /// @trace_id_dd=DD_EM_00806
    /// @needwork = dd
    /// @endcode
    kTerminating = 1
};

/// @brief Execution client class, used for all processes to report execution state to execution management
/// @code{.isoft}
/// @interface_level=software
/// @trace_id_sws=SWS_EM_2001
/// @trace_id_sr=SR_EM_10002
/// @trace_id_ad=AD_EM_00029
/// @trace_id_dd=DD_EM_00772
/// @needwork = ad
/// @endcode
class ExecutionClient final
{
public:
    /// @brief Constructor
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_sws=SWS_EM_2030
    /// @trace_id_ad=AD_EM_00029
    /// @trace_id_dd=DD_EM_00773
    /// @needwork = dda
    /// @endcode
    ExecutionClient() noexcept;

    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_sws=SWS_EM_2002
    /// @trace_id_ad=AD_EM_00029
    /// @trace_id_dd=DD_EM_00774
    /// @needwork = dda
    /// @endcode
    ~ExecutionClient() noexcept;

    /// @brief Disable copy constructor
    /// @param other the other ExecutionClient
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_EM_00029
    /// @trace_id_dd=DD_EM_00775
    /// @needwork = dda
    /// @endcode
    ExecutionClient(ExecutionClient const& other) = delete;

    /// @brief Disable move constructor
    /// @param other the other ExecutionClient
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_EM_00029
    /// @trace_id_dd=DD_EM_00776
    /// @needwork = dda
    /// @endcode
    ExecutionClient(ExecutionClient&& other) = delete;

    /// @brief Disable copy assignment operator
    /// @param other the other ExecutionClient
    /// @return New execution client instance
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_EM_00029
    /// @trace_id_dd=DD_EM_00777
    /// @needwork = dda
    /// @endcode
    ExecutionClient& operator=(ExecutionClient const& other) = delete;

    /// @brief Disable move assignment operator
    /// @param other the other ExecutionClient
    /// @return New execution client instance
    /// @code{.isoft}
    /// @interface_level=software
    /// @trace_id_ad=AD_EM_00029
    /// @trace_id_dd=DD_EM_00778
    /// @needwork = dda
    /// @endcode
    ExecutionClient& operator=(ExecutionClient&& other) = delete;

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
    ara::core::Result< void > ReportExecutionState(ExecutionState const state) const noexcept;

private:
    /// @brief Private implementation class forward declaration
    class PrivateImpl;

    /// @brief Private implementation
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00029
    /// @trace_id_dd=DD_EM_00780
    /// @needwork = dda
    /// @endcode
    /// NOTE: It should be defined as unique_ptr, but low-version compilers have incomplete support for forward declarations and will report compilation errors, so it is changed to shared_ptr.
    std::shared_ptr< PrivateImpl > privateImpl_{nullptr};
};

}  // namespace exec
}  // namespace ara

#endif  ///<  ARA_EXEC_EXECUTION_CLIENT_H_
