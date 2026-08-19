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

#include "ara/exec/internal/ums/server.h"

#include <bits/stdint-intn.h>

#include "ara/exec/internal/log/log.h"
#include "ara/exec/internal/ums/config.h"
#include "isoft/naicpp/global_evloop.h"
#include "isoft/serialize/serialize.h"

#ifdef LOG
    #undef LOG
#endif

/// @brief UMS server log macro definition
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
#define LOG() log1::Log< ums::Server >()

namespace ara {
namespace exec {
namespace internal {
namespace ums {

/// @brief Open UMS server
/// @param spMainLoop Main event loop handle
/// @return 0 success; <0 failure
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_EM_00087
/// @trace_id_dd=DD_EM_00248
/// @needwork = dda
/// @endcode
int32_t Server::Open(std::shared_ptr< isoft::naicpp::EvLoop > spMainLoop) noexcept
{
    upIpcServer_ = std::make_unique< ipc::Server >(
        [this](isoft::ipc::IPCServerHandleType const type) noexcept { _HandleConnection(type); },
        [this](uint32_t const, uint64_t const sid, void *const pMsg, uint32_t const msgSize) {
            _HandleRequest(sid, static_cast< Message * >(pMsg), msgSize);
        });
    if (upIpcServer_ == nullptr) {
        LOGE() << "ums::Server::Open(): nullptr == upIpcServer_";
        return -1;
    }

    if (upIpcServer_->Open(std::move(spMainLoop), GetServiceName()) != 0) {
        upIpcServer_.reset();
        LOGE() << "ums::Server::Open(): Open service {" << GetServiceName() << "} failed !!!";
        return -1;
    }

    LOGD() << "ums::Server::Open(): Open service {" << GetServiceName() << "} success!!!";

    return 0;
}

/// @brief Close UMS client
/// @return 0 success; <0 failure
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_EM_00087
/// @trace_id_dd=DD_EM_00250
/// @needwork = dda
/// @endcode
int32_t Server::Close() const noexcept
{
    if (upIpcServer_ == nullptr) {
        LOGE() << "ums::Server::Close(): nullptr == upIpcServer_";
        return -1;
    }

    return upIpcServer_->Close();
}

/// @brief UMS server connection callback function
/// @param type Connection type
/// @code{.isoft}
/// @interface_level=none
/// @trace_id_ad=AD_EM_00087
/// @trace_id_dd=DD_EM_00255
/// @needwork = dda
/// @endcode
void Server::_HandleConnection(isoft::ipc::IPCServerHandleType const type) noexcept
{
    if (type == IPC_SERVER_HANDLER_CONNECT) {
        LOGI() << "ums::Server::_HandleConnection(): Client Connected.";
    } else if (type == IPC_SERVER_HANDLER_DISCONNECT) {
        LOGI() << "ums::Server::_HandleConnection(): Client Disconnected.";
        updateUserSwclSid_      = 0U;
        getUserSwclManifestSid_ = 0U;
    } else {
    }
}

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
void Server::_HandleRequest(uint64_t const sid, Message const *const pMsg, uint32_t const msgSize)
{
    if ((pMsg == nullptr) || (pMsg->GetSize() != static_cast< uint16_t >(msgSize))) {
        LOGE() << "ums::Server::_HandleRequest(): nullptr == pMsg || msgSize != pMsg->GetSize()";
        return;
    }

    Message::Operation const opt{pMsg->GetOperation()};
    switch (opt) {
        case Message::Operation::kGetUserSwclManifest: {
            getUserSwclManifestSid_ = sid;
            if (fGetUserSwclManifestHandler_) {
                fGetUserSwclManifestHandler_();
            } else {
                LOGE() << "ums::Server::_HandleRequest(): nullptr == fGetUserSwclManifestHandler_";
            }
            break;
        }

        case Message::Operation::kUpdateUserSwcl: {
            updateUserSwclSid_ = sid;

            uint8_t const *const swclsInfoBegin{pMsg->GetPayload()};
            uint8_t const *const swclInfoEnd{swclsInfoBegin + pMsg->GetPayloadSize()};  // PRQA S 2934
            ara::core::Vector< uint8_t > payload{swclsInfoBegin, swclInfoEnd};

            isoft::serialize::Buffer< ara::core::Vector< uint8_t > > const buffer{payload};
            ara::core::Vector< ums::SwclInfo > invalidSwcls;
            ara::core::Vector< ums::SwclInfo > validSwcls;
            std::ignore = isoft::serialize::Deserialize(buffer, invalidSwcls, validSwcls);

            LOGD() << "================= Invalid swcls =================";
            for (auto const &swcl : invalidSwcls) {  // PRQA S 2961
                LOGD() << "swclName =" << swcl.swclName << ", swclVer =" << swcl.swclVer;
            }
            LOGD() << "================= Valid swcls =================";
            for (auto const &swcl : validSwcls) {  // PRQA S 2961
                LOGD() << "swclName =" << swcl.swclName << ", swclVer =" << swcl.swclVer;
            }
            LOGD() << "================= =========== =================";

            if (fUpdateUserSwclHandler_) {
                fUpdateUserSwclHandler_(invalidSwcls, validSwcls);
            } else {
                LOGE() << "ums::Server::_HandleRequest(): nullptr == fUpdateUserSwclHandler_";
            }
            break;
        }

        default: {
            LOGE() << "ums::Server::_HandleRequest(): invalid opt {" << static_cast< uint16_t >(opt) << "}";
            break;
        }
    }
}

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
int32_t Server::_SendResponse(uint64_t const &sid,
                              Message::Operation const &opt,
                              ara::core::StringView const &userSwclManifest,
                              ara::exec::ExecErrc const &errorCode) const noexcept
{
    if (upIpcServer_ == nullptr) {
        LOGE() << "ums::Server::SendResponse(): nullptr == upIpcServer_";
        return -1;
    }

    if (0U == sid) {
        LOGE() << "ums::Server::SendResponse(): 0 == sid";
        return -1;
    }

    std::size_t const msgSize{Message::CalculateSize(userSwclManifest)};
    std::vector< uint8_t > tmpMsg(msgSize);
    Message *const pRspMsg{new (tmpMsg.data()) Message};
    if (pRspMsg == nullptr) {
        LOGE() << "ums::Server::SendResponse(): nullptr == pRspMsg";
        return -1;
    }

    pRspMsg->SetOperation(opt);
    pRspMsg->SetErrorCode(errorCode);
    pRspMsg->SetUserSwclManifest(userSwclManifest);

    LOGD() << "ums::Server::SendResponse(): Opt {" << static_cast< uint16_t >(opt) << "}, UserSwclManifest {"
           << userSwclManifest << "}, Result {" << static_cast< std::int32_t >(errorCode) << "}";

    int32_t const ret{upIpcServer_->SendResponse(sid, pRspMsg, msgSize, true)};
    if (ret != 0) {
        LOGE() << "ums::Server::SendResponse(): upIpcServer_->SendResponse() failed, ret {" << ret << "}";
    }

    pRspMsg->~Message();
    return ret;
}  // PRQA S 2707

/// @brief Reply completion status of UpdateUserSwcl operation
/// @param isSuccess Success or not
/// @return 0 reply operation call success; <0 reply operation call failure
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_EM_00087
/// @trace_id_dd=DD_EM_00253
/// @needwork = dda
/// @endcode
int32_t Server::ResponseUpdateUserSwcls(bool const isSuccess) const noexcept
{
    ara::exec::ExecErrc e;
    if (isSuccess) {
        e = ara::exec::ExecErrc::kNoError;
    } else {
        e = ara::exec::ExecErrc::kGeneralError;
    }
    return _SendResponse(updateUserSwclSid_, Message::Operation::kUpdateUserSwcl, "", e);
}

/// @brief Reply completion status of GetUserSwclManifest operation
/// @param name Process list name
/// @return 0 reply operation call success; <0 reply operation call failure
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_EM_00087
/// @trace_id_dd=DD_EM_00254
/// @needwork = dda
/// @endcode
int32_t Server::ResponseGetUserSwclManifest(ara::core::StringView const &name) const noexcept
{
    ara::exec::ExecErrc e;
    if (name.empty()) {
        e = ara::exec::ExecErrc::kGeneralError;
    } else {
        e = ara::exec::ExecErrc::kNoError;
    }
    return _SendResponse(getUserSwclManifestSid_, Message::Operation::kGetUserSwclManifest, name, e);
}

}  // namespace ums
}  // namespace internal
}  // namespace exec
}  // namespace ara
