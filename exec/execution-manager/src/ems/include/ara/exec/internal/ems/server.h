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
/// @file       server.h
/// @brief      Execution client communication protocol Server class definition
/// @details
/// @date       2023-04-01
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/ExecutionManagement/EMS
/// @unit_name=Server
/// @unit_description=The Server of Execution Management Service.
/// @interface_level=module
/// @endcode
///
/// ================================================================

#ifndef _ARA_EXEC_INTERNAL_EMS_SERVER_H_
#define _ARA_EXEC_INTERNAL_EMS_SERVER_H_

#include "ara/exec/internal/ems/message.h"
#include "ara/exec/internal/ipc/server.h"

namespace ara {
namespace exec {
namespace internal {
namespace ems {

/// @brief EMS server
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_EM_10002
/// @trace_id_ad=AD_EM_00062
/// @trace_id_dd=DD_EM_00332
/// @needwork = ad
/// @endcode
class Server
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
    static constexpr Char8_t const* GetLogCtxId() noexcept { return "EMSS"; }

    /// @brief Get the log context description
    /// @return Log context description
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    static constexpr Char8_t const* GetLogCtxDesc() noexcept { return "The Server Of Execution Management Service"; }

    /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00062
    /// @trace_id_dd=DD_EM_00333
    /// @needwork = dda
    /// @endcode
    Server() = default;

    /// @brief Default destructor
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00062
    /// @trace_id_dd=DD_EM_00334
    /// @needwork = dda
    /// @endcode
    ~Server() = default;

    /// @brief Disable move constructor
    /// @param other the other Server
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00062
    /// @trace_id_dd=DD_EM_00816
    /// @needwork = dda
    /// @endcode
    Server(Server&& other) noexcept = delete;

    /// @brief Disable copy constructor
    /// @param other the other Server
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00062
    /// @trace_id_dd=DD_EM_00817
    /// @needwork = dda
    /// @endcode
    Server(Server const& other) noexcept = delete;

    /// @brief Disable move assignment
    /// @param other the other Server
    /// @return New server instance
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00062
    /// @trace_id_dd=DD_EM_00818
    /// @needwork = dda
    /// @endcode
    Server& operator=(Server&& other) noexcept = delete;

    /// @brief Disable copy assignment
    /// @param other the other Server
    /// @return New server instance
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00062
    /// @trace_id_dd=DD_EM_00819
    /// @needwork = dda
    /// @endcode
    Server& operator=(Server const& other) noexcept = delete;

    /// @brief Open the EMC server
    /// @param spMainLoop Main event loop handle
    /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00062
    /// @trace_id_dd=DD_EM_00335
    /// @needwork = dda
    /// @endcode
    int32_t Open(std::shared_ptr< isoft::naicpp::EvLoop > spMainLoop) noexcept;

    /// @brief Close the EMC server
    /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00062
    /// @trace_id_dd=DD_EM_00336
    /// @needwork = dda
    /// @endcode
    int32_t Close() const noexcept;

    /// @brief Execution state callback function type
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    using ExecStateHandler = std::function< void(Message::State const& state, uint32_t pid) >;

    /// @brief Register the report callback function
    /// @param cb Callback function
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00062
    /// @trace_id_dd=DD_EM_00337
    /// @needwork = dda
    /// @endcode
    void OnReport(ExecStateHandler const& cb) noexcept { execStateHandler_ = cb; }

private:
    /// @brief Handle request messages
    /// @param pid Process ID
    /// @param pMsg Message content
    /// @param msgSize Message content size
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00062
    /// @trace_id_dd=DD_EM_00338
    /// @needwork = dda
    /// @endcode
    void _HandleRequest(uint32_t const pid, Message const* const pMsg, uint32_t const msgSize) const noexcept;

private:
    /// @brief  ipc server handle
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00062
    /// @trace_id_dd=DD_EM_00339
    /// @needwork = dda
    /// @endcode
    std::unique_ptr< ipc::Server > upIpcServer_{nullptr};  // PRQA S 2026

    /// @brief User callback function
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00062
    /// @trace_id_dd=DD_EM_00340
    /// @needwork = dda
    /// @endcode
    ExecStateHandler execStateHandler_{nullptr};

};  ///< class Server

}  // namespace ems
}  // namespace internal
}  // namespace exec
}  // namespace ara

#endif  ///< _ARA_EXEC_INTERNAL_EMS_SERVER_H_
