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
/// @brief      Update client communication protocol Server class definition
/// @details
/// @date       2023-04-01
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/ExecutionManagement/UMS
/// @unit_name=Server
/// @unit_description=The Server of Update Management Service.
/// @interface_level=module
/// @endcode
///
/// ================================================================

#ifndef _ARA_EXEC_INTERNAL_UMS_SERVER_H_
#define _ARA_EXEC_INTERNAL_UMS_SERVER_H_

#include "ara/core/vector.h"
#include "ara/exec/internal/ipc/server.h"
#include "ara/exec/internal/ums/message.h"
#include "ara/exec/internal/ums/swcl_info.h"

namespace ara {
namespace exec {
namespace internal {
namespace ums {

/// @brief UMS server definition
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_EM_00063,SR_EM_00066
/// @trace_id_ad=AD_EM_00087
/// @trace_id_dd=DD_EM_00245
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

 /// @brief Get log context ID
 /// @return Log context ID
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    static constexpr Char8_t const *GetLogCtxId() noexcept { return "UMSS"; }

 /// @brief Get log context description
 /// @return Log context description
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    static constexpr Char8_t const *GetLogCtxDesc() noexcept { return "The Server Of Update Management Service"; }

 /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00087
    /// @trace_id_dd=DD_EM_00246
    /// @needwork = dda
    /// @endcode
    Server() = default;

 /// @brief Default destructor
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00087
    /// @trace_id_dd=DD_EM_00247
    /// @needwork = dda
    /// @endcode
    ~Server() = default;

 /// @brief Default copy construction
 /// @param other Other Server object
    Server(Server const &other) = delete;

 /// @brief Default move construction
 /// @param other Other Server object
    Server(Server &&other) = default;

 /// @brief Default copy assignment
 /// @param other Other Server object
 /// @return New Server object
    Server &operator=(Server const &other) = delete;

 /// @brief Default move assignment
 /// @param other Other Server object
 /// @return New Server object
    Server &operator=(Server &&other) = default;

 /// @brief Open UMS server
 /// @param spMainLoop Main event loop handle
 /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00087
    /// @trace_id_dd=DD_EM_00248
    /// @needwork = dda
    /// @endcode
    int32_t Open(std::shared_ptr< isoft::naicpp::EvLoop > spMainLoop) noexcept;

 /// @brief Close UMS client
 /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00087
    /// @trace_id_dd=DD_EM_00250
    /// @needwork = dda
    /// @endcode
    int32_t Close() const noexcept;

 /// @brief Callback function type definition for GetUserSwclManifest operation
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    using GetUserSwclHandler = std::function< void() >;

 /// @brief Callback function type definition for UpdateUserSwcl operation
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    using UpdateUserSwclHandler
        = std::function< void(ara::core::Vector< SwclInfo > const &, ara::core::Vector< SwclInfo > const &) >;

 /// @brief Register callback function for UpdateUserSwcl operation
 /// @param cb Callback function
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00087
    /// @trace_id_dd=DD_EM_00251
    /// @needwork = dda
    /// @endcode
    void OnUpdateUserSwcl(UpdateUserSwclHandler const &cb) noexcept { fUpdateUserSwclHandler_ = cb; }

 /// @brief Register callback function for GetUserSwclManifest operation
 /// @param cb Callback function
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00087
    /// @trace_id_dd=DD_EM_00252
    /// @needwork = dda
    /// @endcode
    void OnGetUserSwclManifest(GetUserSwclHandler const &cb) noexcept { fGetUserSwclManifestHandler_ = cb; }

 /// @brief Reply completion status of UpdateUserSwcl operation
 /// @param isSuccess Success or not
 /// @return 0 reply operation call success; <0 reply operation call failure
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00087
    /// @trace_id_dd=DD_EM_00253
    /// @needwork = dda
    /// @endcode
    int32_t ResponseUpdateUserSwcls(bool const isSuccess) const noexcept;

 /// @brief Reply completion status of GetUserSwclManifest operation
 /// @param name Process list name
 /// @return 0 reply operation call success; <0 reply operation call failure
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00087
    /// @trace_id_dd=DD_EM_00254
    /// @needwork = dda
    /// @endcode
    int32_t ResponseGetUserSwclManifest(ara::core::StringView const &name) const noexcept;

private:
 /// @brief UMS server connection callback function
 /// @param type Connection type
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00087
    /// @trace_id_dd=DD_EM_00255
    /// @needwork = dda
    /// @endcode
    void _HandleConnection(isoft::ipc::IPCServerHandleType const type) noexcept;

 /// @brief Handle request message
 /// @param sid Session ID
 /// @param pMsg Message body
 /// @param msgSize Message body size
 /// @exception std::runtime_error If processing fails
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00087
    /// @trace_id_dd=DD_EM_00256
    /// @needwork = dda
    /// @endcode
    void _HandleRequest(uint64_t const sid, Message const *const pMsg, uint32_t const msgSize);

 /// @brief Reply common function
 /// @param sid Session ID
 /// @param opt Operation code
 /// @param userSwclManifest User software cluster manifest
 /// @param errorCode Error code
 /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00087
    /// @trace_id_dd=DD_EM_00257
    /// @needwork = dda
    /// @endcode
    int32_t _SendResponse(uint64_t const &sid,
                          Message::Operation const &opt,
                          ara::core::StringView const &userSwclManifest,
                          ara::exec::ExecErrc const &errorCode) const noexcept;

private:
 /// @brief IPC Server handle
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00087
    /// @trace_id_dd=DD_EM_00258
    /// @needwork = dda
    /// @endcode
    std::unique_ptr< ipc::Server > upIpcServer_{nullptr};  // PRQA S 2026

 /// @brief Session ID of UpdateUserSwcl request
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00087
    /// @trace_id_dd=DD_EM_00259
    /// @needwork = dda
    /// @endcode
    isoft::ipc::IPCSessionId updateUserSwclSid_{0U};

 /// @brief UpdateUserSwcl callback function
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00087
    /// @trace_id_dd=DD_EM_00260
    /// @needwork = dda
    /// @endcode
    UpdateUserSwclHandler fUpdateUserSwclHandler_{nullptr};

 /// @brief Session ID of getUserSwclManifest request
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00087
    /// @trace_id_dd=DD_EM_00261
    /// @needwork = dda
    /// @endcode
    isoft::ipc::IPCSessionId getUserSwclManifestSid_{0U};

 /// @brief getUserSwclManifest callback function
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00087
    /// @trace_id_dd=DD_EM_00262
    /// @needwork = dda
    /// @endcode
    GetUserSwclHandler fGetUserSwclManifestHandler_{nullptr};

};  ///< class Server

}  // namespace ums
}  // namespace internal
}  // namespace exec
}  // namespace ara

#endif  ///< _ARA_EXEC_INTERNAL_UMS_SERVER_H_
