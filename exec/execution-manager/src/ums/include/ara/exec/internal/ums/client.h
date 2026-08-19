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
/// @brief      Update client communication protocol Client class definition
/// @details
/// @date       2023-04-01
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/ExecutionManagement/UMS
/// @unit_name=Client
/// @unit_description=The Client of Update Management Service.
/// @interface_level=module
/// @endcode
///
/// ================================================================

#ifndef _ARA_EXEC_INTERNAL_UMS_CLIENT_H_
#define _ARA_EXEC_INTERNAL_UMS_CLIENT_H_

#include "ara/core/vector.h"
#include "ara/exec/internal/ipc/client.h"
#include "ara/exec/internal/ums/message.h"
#include "ara/exec/internal/ums/swcl_info.h"

namespace ara {
namespace exec {
namespace internal {
namespace ums {

/// @brief UMS client
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_sr=SR_EM_10007
/// @trace_id_ad=AD_EM_00086
/// @trace_id_dd=DD_EM_00285
/// @needwork = ad
/// @endcode
class Client final
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
    static constexpr Char8_t const* GetLogCtxId() noexcept { return "UMSC"; }

 /// @brief Get log context description
 /// @return Log context description
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    static constexpr Char8_t const* GetLogCtxDesc() noexcept { return "The Client Of Update Management Service"; }

 /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00086
    /// @trace_id_dd=DD_EM_00286
    /// @needwork = dda
    /// @endcode
    Client() = default;

 /// @brief Default destructor
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00086
    /// @trace_id_dd=DD_EM_00287
    /// @needwork = dda
    /// @endcode
    ~Client() = default;

 /// @brief Disable copy construction
 /// @param other Other object
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00086
    /// @trace_id_dd=DD_EM_00288
    /// @needwork = dda
    /// @endcode
    Client(Client const& other) = delete;

 /// @brief Disable move construction
 /// @param other Other object
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00086
    /// @trace_id_dd=DD_EM_00289
    /// @needwork = dda
    /// @endcode
    Client(Client&& other) noexcept = delete;

 /// @brief Disable copy assignment
 /// @param other Other object
 /// @return Object
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00086
    /// @trace_id_dd=DD_EM_00290
    /// @needwork = dda
    /// @endcode
    Client& operator=(Client const& other) noexcept = delete;

 /// @brief Disable move assignment
 /// @param other Other object
 /// @return Object
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00086
    /// @trace_id_dd=DD_EM_00291
    /// @needwork = dda
    /// @endcode
    Client& operator=(Client&& other) noexcept = delete;

 /// @brief Open UMS client
 /// @param spMainLoop Main event loop handle
 /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00086
    /// @trace_id_dd=DD_EM_00292
    /// @needwork = dda
    /// @endcode
    int32_t Open(std::shared_ptr< isoft::naicpp::EvLoop > spMainLoop) noexcept;

 /// @brief Open UMS client
 /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00086
    /// @trace_id_dd=DD_EM_00293
    /// @needwork = dda
    /// @endcode
    int32_t Open() noexcept { return Open(nullptr); }

 /// @brief Close UMS client
 /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00086
    /// @trace_id_dd=DD_EM_00294
    /// @needwork = dda
    /// @endcode
    int32_t Close() noexcept;

 /// @brief GetFunctionGroupState callback function
 /// @param Name Retrieved UserSwclManifest path, ignore if not needed
 /// @param errorCode Error code
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    using UserSwclHandler
        = std::function< void(ara::core::StringView const& name, ara::exec::ExecErrc const errorCode) >;

 /// @brief Get user software cluster manifest, called asynchronously
 /// @param cb Callback function, called when result is returned
 /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00086
    /// @trace_id_dd=DD_EM_00295
    /// @needwork = dda
    /// @endcode
    int32_t GetUserSwclManifest(UserSwclHandler const& cb) noexcept
    {
        std::shared_ptr< Message > const spMsg{std::make_shared< Message >()};
        spMsg->SetOperation(Message::Operation::kGetUserSwclManifest);
        spMsg->SetPayloadSize(0U);

        return _SendMessage(spMsg.get(), static_cast< uint64_t >(spMsg->GetSize()), cb);
    }

 /// @brief Update user software cluster, called asynchronously
 /// @param invalidSwcls Invalid software cluster list
 /// @param validSwcls Valid software cluster list
 /// @param cb Callback function, called when result is returned
 /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=module
    /// @trace_id_ad=AD_EM_00086
    /// @trace_id_dd=DD_EM_00296
    /// @needwork = dda
    /// @endcode
    int32_t UpdateUserSwcls(ara::core::Vector< SwclInfo > const& invalidSwcls,
                            ara::core::Vector< SwclInfo > const& validSwcls,
                            UserSwclHandler const& cb) noexcept;

private:
 /// @brief IPC asynchronous send operation
 /// @param spMsg Message body
 /// @param msgSize Message body size
 /// @param cb Callback function
 /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00086
    /// @trace_id_dd=DD_EM_00297
    /// @needwork = dda
    /// @endcode
    int32_t _SendMessage(Message* pMsg, uint64_t const msgSize, UserSwclHandler const& cb) noexcept;

 /// @brief response message handler function
 /// @param pRspMsg Message body
 /// @param msgSize Message body size
 /// @param errorCode Error code
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00086
    /// @trace_id_dd=DD_EM_00298
    /// @needwork = dda
    /// @endcode
    void _HandleResponse(void* const pRspMsg, uint32_t const msgSize, int32_t const errorCode) const noexcept;

private:
 /// @brief IPC client handle
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00086
    /// @trace_id_dd=DD_EM_00299
    /// @needwork = dda
    /// @endcode
    std::shared_ptr< ipc::Client > spIpcClient_{nullptr};  // PRQA S 2026

 /// @brief GetUserSwclManifest operation callback function
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00086
    /// @trace_id_dd=DD_EM_00300
    /// @needwork = dda
    /// @endcode
    UserSwclHandler fGetUserSwclManifestCb_{nullptr};
 /// @brief UpdateUserSwcl operation callback function
    /// @code{.isoft}
    /// @interface_level=none
    /// @trace_id_ad=AD_EM_00086
    /// @trace_id_dd=DD_EM_00301
    /// @needwork = dda
    /// @endcode
    UserSwclHandler fUpdateUserSwclCb_{nullptr};

};  ///< class Client

}  // namespace ums
}  // namespace internal
}  // namespace exec
}  // namespace ara

#endif  ///< _ARA_EXEC_INTERNAL_UMS_CLIENT_H_
