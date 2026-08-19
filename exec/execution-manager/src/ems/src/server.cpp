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
/// @file       server.cpp
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
/// @unit_description=The Server of Update Management Service.
/// @interface_level=module
/// @endcode
///
/// ================================================================

#include "ara/exec/internal/ems/server.h"

#include "ara/exec/internal/ems/config.h"
#include "ara/exec/internal/log/log.h"
#include "isoft/naicpp/global_evloop.h"

#ifdef LOG
    #undef LOG
#endif

/// @brief EMS server log macro definition
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
#define LOG() log1::Log< ems::Server >()

namespace ara {
namespace exec {
namespace internal {
namespace ems {

/// @brief Open the EMC server
/// @param spMainLoop Main event loop handle
/// @return 0 success; <0 failure
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_EM_00062
/// @trace_id_dd=DD_EM_00335
/// @needwork = dda
/// @endcode
int32_t Server::Open(std::shared_ptr< isoft::naicpp::EvLoop > spMainLoop) noexcept
{
    upIpcServer_ = std::make_unique< ipc::Server >(
        nullptr, [this](uint32_t const pid, uint64_t const sid, void* const pMsg, uint32_t const msgSize) noexcept {
            std::ignore = sid;
            _HandleRequest(pid, static_cast< Message* >(pMsg), msgSize);
        });
    if (upIpcServer_ == nullptr) {
        LOGE() << "ems::Server::Open(): nullptr == upIpcServer_";
        return -1;
    }

    if (upIpcServer_->Open(std::move(spMainLoop), GetServiceName()) != 0) {
        upIpcServer_.reset();
        LOGE() << "ems::Server::Open(): Open service {" << GetServiceName() << "} failed !!!";
        return -1;
    }

    LOGD() << "ems::Server::Open(): Open service {" << GetServiceName() << "} success!!!";

    return 0;
}

/// @brief Close the EMC server
/// @return 0 success; <0 failure
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_EM_00062
/// @trace_id_dd=DD_EM_00336
/// @needwork = dda
/// @endcode
int32_t Server::Close() const noexcept
{
    if (upIpcServer_ == nullptr) {
        LOGE() << "ems::Server::Close(): nullptr == upIpcServer_";
        return -1;
    }

    return upIpcServer_->Close();
}

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
void Server::_HandleRequest(uint32_t const pid, Message const* const pMsg, uint32_t const msgSize) const noexcept
{
    if ((pMsg == nullptr) || (msgSize != sizeof(Message))) {
        LOGE() << "ems::Server::_HandleRequest(): Invalid message from pid {" << pid << "}, msgSize {" << msgSize
               << "}";
        return;
    }

    Message::State const state{pMsg->GetState()};

    LOGD() << "ems::Server::_HandleRequest(): pid {" << pid << "}, state {" << static_cast< uint32_t >(state) << "}";

    if (execStateHandler_) {
        execStateHandler_(state, pid);
    }
}

}  // namespace ems
}  // namespace internal
}  // namespace exec
}  // namespace ara
