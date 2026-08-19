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
/// @file       client.h
/// @brief      Execution client communication protocol Client class definition
/// @details
/// @date       2023-04-01
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/ExecutionManagement/EMS
/// @unit_name=Client
/// @unit_description=The Client of Execution Management Service.
/// @interface_level=module
/// @endcode
///
/// ================================================================

#ifndef _ARA_EXEC_INTERNAL_EMS_CLIENT_H_
#define _ARA_EXEC_INTERNAL_EMS_CLIENT_H_

#include "ara/exec/internal/ems/message.h"
#include "ara/exec/internal/ipc/client.h"

namespace ara {
namespace exec {
namespace internal {
namespace ems {

/// @brief EMS client
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_EM_10002
/// @trace_id_ad=AD_EM_00061
/// @trace_id_dd=DD_EM_00348
/// @needwork = ad
/// @endcode
class Client
{
public:
    /// @brief Redefine char
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
    static constexpr Char8_t const* GetLogCtxId() noexcept { return "EMSC"; }

    /// @brief Get the log context description
    /// @return Log context description
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    static constexpr Char8_t const* GetLogCtxDesc() noexcept { return "The Client Of Execution Management Service"; }

    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00061
    /// @trace_id_dd=DD_EM_00349
    /// @needwork = dda
    /// @endcode
    Client() = default;

    /// @brief Default destructor
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00061
    /// @trace_id_dd=DD_EM_00350
    /// @needwork = dda
    /// @endcode
    ~Client() = default;

    /// @brief Disable move constructor
    /// @param other the other Config
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00061
    /// @trace_id_dd=DD_EM_00808
    /// @needwork = dda
    /// @endcode
    Client(Client&& other) noexcept = delete;

    /// @brief Disable copy constructor
    /// @param other the other Config
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00061
    /// @trace_id_dd=DD_EM_00809
    /// @needwork = dda
    /// @endcode
    Client(Client const& other) noexcept = delete;

    /// @brief Disable move assignment
    /// @param other the other Config
    /// @return New client instance
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00061
    /// @trace_id_dd=DD_EM_00810
    /// @needwork = dda
    /// @endcode
    Client& operator=(Client&& other) noexcept = delete;

    /// @brief Disable copy assignment
    /// @param other the other Config
    /// @return New client instance
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00061
    /// @trace_id_dd=DD_EM_00811
    /// @needwork = dda
    /// @endcode
    Client& operator=(Client const& other) noexcept = delete;

    /// @brief Open the EMC client
    /// @param spMainLoop Main event loop handle
    /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00061
    /// @trace_id_dd=DD_EM_00351
    /// @needwork = dda
    /// @endcode
    int32_t Open(std::shared_ptr< isoft::naicpp::EvLoop > spMainLoop) noexcept;

    /// @brief Open the EMC client
    /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00061
    /// @trace_id_dd=DD_EM_00352
    /// @needwork = dda
    /// @endcode
    int32_t Open() noexcept { return Open(nullptr); }

    /// @brief Close the EMC client
    /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00061
    /// @trace_id_dd=DD_EM_00353
    /// @needwork = dda
    /// @endcode
    int32_t Close() noexcept;

    /// @brief Report the running state
    /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00061
    /// @trace_id_dd=DD_EM_00354
    /// @needwork = dda
    /// @endcode
    int32_t ReportRunning() noexcept { return _ReportExecutionState(Message::State::kRunning); }

private:
    /// @brief Report the execution state
    /// @param state State
    /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00061
    /// @trace_id_dd=DD_EM_00355
    /// @needwork = dda
    /// @endcode
    int32_t _ReportExecutionState(Message::State const& state) noexcept;

private:
    /// @brief IPC client
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00061
    /// @trace_id_dd=DD_EM_00356
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< ipc::Client > spIpcClient_{nullptr};  // PRQA S 2026

};  ///< class Client

}  // namespace ems
}  // namespace internal
}  // namespace exec
}  // namespace ara

#endif  ///< _ARA_EXEC_INTERNAL_EMS_CLIENT_H_
