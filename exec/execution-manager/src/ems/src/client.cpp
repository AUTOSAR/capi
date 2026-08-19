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
/// @file       client.cpp
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
/// @unit_description=The Client of Update Management Service.
/// @interface_level=module
/// @endcode
///
/// ================================================================

#include "ara/exec/internal/ems/client.h"

#include "ara/exec/internal/config/sysconfig.h"
#include "ara/exec/internal/ems/config.h"
#include "ara/exec/internal/log/log.h"

#ifdef LOG
    #undef LOG
#endif

/// @brief EMS client log macro definition
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
#define LOG() log1::Log< ems::Client >()

namespace ara {
namespace exec {
namespace internal {
namespace ems {

/// @brief Open the EMC client
/// @param spMainLoop Main event loop handle
/// @return 0 success; <0 failure
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_EM_00061
/// @trace_id_dd=DD_EM_00351
/// @needwork = dda
/// @endcode
int32_t Client::Open(std::shared_ptr< isoft::naicpp::EvLoop > spMainLoop) noexcept
{
    if (spIpcClient_) {
        LOGE() << "ems::Client::Open(): Already Opened !!!";
        return -1;
    }

    spIpcClient_ = std::make_shared< ipc::Client >();
    if (spIpcClient_ == nullptr) {
        LOGE() << "ems::Client::Open(): Failed to create spIpcClient_";
        return -1;
    }

    if (0 != spIpcClient_->Open(std::move(spMainLoop), config::GetExecutionManagerIpcName(), GetServiceName())) {
        spIpcClient_.reset();
        return -1;
    }

    return 0;
}

/// @brief Close the EMC client
/// @return 0 success; <0 failure
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_EM_00061
/// @trace_id_dd=DD_EM_00353
/// @needwork = dda
/// @endcode
int32_t Client::Close() noexcept
{
    if (spIpcClient_ == nullptr) {
        LOGE() << "ems::Client::Close(): nullptr == spIpcClient_";
        return -1;
    }

    if (0 != spIpcClient_->Close()) {
        return -1;
    }

    spIpcClient_.reset();
    return 0;
}

/// @brief Report the execution state
/// @param state State
/// @return 0 success; <0 failure
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_ad=AD_EM_00061
/// @trace_id_dd=DD_EM_00355
/// @needwork = dda
/// @endcode
int32_t Client::_ReportExecutionState(Message::State const& state) noexcept
{
    if (spIpcClient_ == nullptr) {
        LOGE() << "ems::Client::ReportExecutionState(): nullptr == spIpcClient_";
        return -1;
    }

    ems::Message msg{};
    msg.SetState(state);

    LOGD() << "ems::Client:ReportExecutionState(): pid:" << getpid() << ", state:" << static_cast< uint16_t >(state);

    return spIpcClient_->Post(static_cast< void* >(&msg), static_cast< std::uint32_t >(sizeof(msg)));
}

}  // namespace ems
}  // namespace internal
}  // namespace exec
}  // namespace ara
