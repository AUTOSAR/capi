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
/// @file       update_client.cpp
/// @brief      Update client class definition
/// @details
/// @date       2023-04-01
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/ExecutionManagement/UpdateClient
/// @unit_name=UpdateClient
/// @unit_description=Used to update user software collections and manage their states.
/// @interface_level=software
/// @endcode
///
/// ================================================================

#include "ara/exec/internal/update_client.h"

#include <ara/core/promise.h>
#include <isoft/ara_fsh/filesystem_hierarchy.h>

#include "ara/exec/exec_error_domain.h"
#include "ara/exec/internal/config/execution_manifest.h"
#include "ara/exec/internal/log/log.h"
#include "ara/exec/internal/ums/client.h"

#ifdef LOG
    #undef LOG
#endif

/// @brief Update client log macro definition
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
#define LOG() ara::exec::internal::log1::Log< ara::exec::internal::UpdateClient::PrivateImpl >()

namespace ara {
namespace exec {
namespace internal {

/// @brief Update client private implementation
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
class UpdateClient::PrivateImpl
{
public:
 /// @brief Default constructor
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    PrivateImpl() noexcept;

 /// @brief Default destructor
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    ~PrivateImpl() noexcept;

 /// @brief Default copy constructor
    /// @param  other the other instance
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    PrivateImpl(PrivateImpl const &other) = delete;

 /// @brief Default move constructor
    /// @param  other the other instance
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    PrivateImpl(PrivateImpl &&other) = delete;

 /// @brief Default copy assignment function
    /// @param other the other instance
    /// @return new PrivateImpl instance
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    PrivateImpl &operator=(PrivateImpl const &other) = delete;

 /// @brief Default move assignment function
    /// @param other the other instance
    /// @return new PrivateImpl instance
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    PrivateImpl &operator=(PrivateImpl &&other) = delete;

 /// @brief Request re-parsing of process list
 /// @param invalidSwcls Invalid software cluster list
 /// @param validSwcls Valid software cluster list
 /// @param cb Callback function
 /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    int32_t UpdateUserSwclsPrivate(ara::core::Vector< ums::SwclInfo > const &invalidSwcls,
                                   ara::core::Vector< ums::SwclInfo > const &validSwcls,
                                   ums::Client::UserSwclHandler const &cb) noexcept;

 /// @brief Get user software cluster manifest file name
 /// @param cb Callback function
 /// @return 0 success; <0 failure
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    int32_t GetUserSwclManifestPath(ums::Client::UserSwclHandler const &cb) noexcept;

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
    static constexpr Char8_t const *GetLogCtxId() noexcept { return "UCLT"; }

 /// @brief Get log context description
 /// @return Log context description
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    static constexpr Char8_t const *GetLogCtxDesc() noexcept { return "The Client of Update Management Service"; }

private:
 /// @brief Update client communication protocol Client object
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    ums::Client umsClient_{};

 /// @brief Is valid
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    bool isValid_{false};
};

/// @brief Default constructor
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
UpdateClient::PrivateImpl::PrivateImpl() noexcept
{
    isoft::ara_fsh::Process const fsh;
    ara::core::String const execManifestPath{fsh.GetExecutionManifest()};
    if (execManifestPath.empty()) {
        isValid_ = false;
        return;
    }

    internal::config::ExecutionManifest execManifest;
    if (0 != execManifest.Load(execManifestPath)) {
        isValid_ = false;
        return;
    }

    if (!execManifest.IsUpdateManager()) {
        isValid_ = false;
        return;
    }

    if (0 != umsClient_.Open()) {
        LOGE() << "umsClient_.Open() error !!!";
        return;
    }

    isValid_ = true;
}

/// @brief Default destructor
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
UpdateClient::PrivateImpl::~PrivateImpl() noexcept
{
    std::ignore = umsClient_.Close();
    isValid_    = false;
}

/// @brief Get user software cluster manifest file name
/// @param cb Callback function
/// @return 0 success; <0 failure
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
int32_t UpdateClient::PrivateImpl::GetUserSwclManifestPath(ums::Client::UserSwclHandler const &cb) noexcept
{
    if (!isValid_) {
        return -1;
    }

    return umsClient_.GetUserSwclManifest(cb);
}

/// @brief Request re-parsing of process list
/// @param invalidSwcls Invalid software cluster list
/// @param validSwcls Valid software cluster list
/// @param cb Callback function
/// @return 0 success; <0 failure
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
int32_t UpdateClient::PrivateImpl::UpdateUserSwclsPrivate(ara::core::Vector< ums::SwclInfo > const &invalidSwcls,
                                                          ara::core::Vector< ums::SwclInfo > const &validSwcls,
                                                          ums::Client::UserSwclHandler const &cb) noexcept
{
    if (!isValid_) {
        return -1;
    }

    return umsClient_.UpdateUserSwcls(invalidSwcls, validSwcls, cb);
}

/// @brief Default constructor
/// @code{.isoft}
/// @interface_level=software
/// @trace_id_ad=AD_EM_00046
/// @trace_id_dd=DD_EM_00710
/// @needwork = dda
/// @endcode
UpdateClient::UpdateClient() noexcept : privateImpl_{std::make_shared< PrivateImpl >()}
{
    if (nullptr == privateImpl_) {
        LOGE() << "UpdateClient(): nullptr == privateImpl_";
    }
}

/// @brief Default destructor
/// @code{.isoft}
/// @interface_level=software
/// @trace_id_ad=AD_EM_00046
/// @trace_id_dd=DD_EM_00711
/// @needwork = dda
/// @endcode
UpdateClient::~UpdateClient() noexcept
{
    if (nullptr != privateImpl_) {
        privateImpl_.reset();
    }

    return;
}

/// @brief Update user software cluster
/// @param invalidSwcls Invalid software cluster list
/// @param validSwcls Valid software cluster list
/// @return Future used to get result
/// @code{.isoft}
/// @interface_level=software
/// @trace_id_ad=AD_EM_00046
/// @trace_id_dd=DD_EM_00716
/// @needwork = dda
/// @endcode
ara::core::Future< void > UpdateClient::UpdateUserSwcls(UpdateClient::SwclInfoList const &invalidSwcls,
                                                        UpdateClient::SwclInfoList const &validSwcls) const noexcept
{
    std::shared_ptr< ara::core::Promise< void > > promise{std::make_shared< ara::core::Promise< void > >()};
    ara::core::Future< void > future{promise->get_future()};

    if (nullptr == privateImpl_) {
        LOGE() << "ReparseProcessList(): nullptr == privateImpl_";
        promise->SetError(ara::exec::ExecErrc::kGeneralError);
        return future;
    }

    int32_t const r{privateImpl_->UpdateUserSwclsPrivate(
        invalidSwcls, validSwcls,  // PRQA S 2961
        [promise](ara::core::StringView const &, ara::exec::ExecErrc const errorCode) -> void {
            if (ara::exec::ExecErrc::kNoError != errorCode) {
                promise->SetError(errorCode);
            } else {
                promise->set_value();
            }
        })};

    if (0 != r) {
        promise->SetError(ara::exec::ExecErrc::kCommunicationError);
    }

    return future;
}

/// @brief Get current user software cluster manifest name
/// @return Future used to get result
/// @code{.isoft}
/// @interface_level=software
/// @trace_id_ad=AD_EM_00046
/// @trace_id_dd=DD_EM_00717
/// @needwork = dda
/// @endcode
ara::core::Future< ara::core::String > UpdateClient::GetUserSwclManifest() const noexcept
{
    std::shared_ptr< ara::core::Promise< ara::core::String > > promise{
        std::make_shared< ara::core::Promise< ara::core::String > >()};
    ara::core::Future< ara::core::String > future{promise->get_future()};

    if (nullptr == privateImpl_) {
        LOGE() << "GetUserSwclManifest(): nullptr == privateImpl_";
        promise->SetError(ara::exec::ExecErrc::kGeneralError);
        return future;
    }

    int32_t const r{privateImpl_->GetUserSwclManifestPath(
        [promise](ara::core::StringView const &name, ara::exec::ExecErrc const errorCode) -> void {
            if (ara::exec::ExecErrc::kNoError != errorCode) {
                promise->SetError(errorCode);
            } else {
                promise->set_value(ara::core::String{name});
            }
        })};

    if (0 != r) {
        promise->SetError(ara::exec::ExecErrc::kCommunicationError);
    }

    return future;
}

}  // namespace internal
}  // namespace exec
}  // namespace ara
