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

#include "ara/exec/internal/ums/client.h"

#include <bits/stdint-intn.h>
#include <isoft/serialize/serialize.h>

#include "ara/exec/internal/config/sysconfig.h"
#include "ara/exec/internal/log/log.h"
#include "ara/exec/internal/ums/config.h"

#ifdef LOG
    #undef LOG
#endif

/// @brief UMS client log macro definition
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
#define LOG() log1::Log< ums::Client >()

namespace ara {
namespace exec {
namespace internal {
namespace ums {

/// @brief Open UMS client
/// @param spMainLoop Main event loop handle
/// @return 0 success; <0 failure
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_EM_00086
/// @trace_id_dd=DD_EM_00292
/// @needwork = dda
/// @endcode
int32_t Client::Open(std::shared_ptr< isoft::naicpp::EvLoop > spMainLoop) noexcept
{
    if (spIpcClient_) {
        LOGE() << "ums::Client::Open(): Already Opened !!!";
        return -1;
    }

    spIpcClient_ = std::make_shared< ipc::Client >(
        [this](void *const pRspMsg, uint32_t const msgSize, int32_t const errorCode) noexcept {
            _HandleResponse(pRspMsg, msgSize, errorCode);
        });

    if (spIpcClient_ == nullptr) {
        LOGE() << "ums::Client::Open(): Failed to create spIpcClient_";
        return -1;
    }

    if (0 != spIpcClient_->Open(std::move(spMainLoop), config::GetExecutionManagerIpcName(), GetServiceName())) {
        spIpcClient_.reset();
        return -1;
    }

    return 0;
}

/// @brief Close UMS client
/// @return 0 success; <0 failure
/// @code{.isoft}
/// @interface_level=module
/// @trace_id_ad=AD_EM_00086
/// @trace_id_dd=DD_EM_00294
/// @needwork = dda
/// @endcode
int32_t Client::Close() noexcept
{
    if (spIpcClient_ == nullptr) {
        LOGE() << "ums::Client::Close(): nullptr == spIpcClient_";
        return -1;
    }

    return spIpcClient_->Close();
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
int32_t Client::UpdateUserSwcls(ara::core::Vector< ums::SwclInfo > const &invalidSwcls,
                                ara::core::Vector< ums::SwclInfo > const &validSwcls,
                                UserSwclHandler const &cb) noexcept
{
    ara::core::Vector< uint8_t > payload;
    isoft::serialize::Buffer< ara::core::Vector< uint8_t > > buffer{payload};
    std::ignore = isoft::serialize::Serialize(buffer, invalidSwcls, validSwcls);  // PRQA S 4127

    uint64_t const msgSize{Message::GetSize(payload.size())};
    std::vector< uint8_t > tmpMsg(msgSize);
    Message *const pReqMsg{new (tmpMsg.data()) Message};
    if (pReqMsg == nullptr) {
        LOGE() << "ums::Client::UpdateUserSwcls(): Alloc Memory for Message failed !!!";
        return -1;
    }

    pReqMsg->SetOperation(Message::Operation::kUpdateUserSwcl);
    pReqMsg->SetUserSwcls(payload);

    int32_t ret{_SendMessage(pReqMsg, msgSize, cb)};
    pReqMsg->~Message();

    return ret;
}  // PRQA S 2706

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
int32_t Client::_SendMessage(Message *pMsg, uint64_t const msgSize, UserSwclHandler const &cb) noexcept
{
    if (spIpcClient_ == nullptr) {
        LOGE() << "ums::Client::SendMessage(): nullptr == spIpcClient_";
        return -1;
    }

    LOGD() << "ums::Client::SendMessage(): opt {" << static_cast< uint8_t >(pMsg->GetOperation()) << "}";

    switch (pMsg->GetOperation()) {
        case Message::Operation::kGetUserSwclManifest: {
            fGetUserSwclManifestCb_ = cb;
            break;
        }

        case Message::Operation::kUpdateUserSwcl: {
            fUpdateUserSwclCb_ = cb;
            break;
        }

        default: {
            LOGE() << "ums::Client::SendMessage(): Unkown Request";
            break;
        }
    }
    constexpr int32_t kTimeout{3000};
    if (0 > spIpcClient_->SendAsync(pMsg, msgSize, kTimeout)) {
        LOGE() << "ums::Client::SendMessage() failed !!!";
        return -1;
    }

    return 0;
}

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
void Client::_HandleResponse(void *const pRspMsg, uint32_t const msgSize, int32_t const errorCode) const noexcept
{
 /// Communication failure, return error to all sessions and clear sessions
    if (errorCode != 0) {
        LOGE() << "ums::Client::HandleResponse(): communication error -" << errorCode;
        return;
    }

    if ((pRspMsg == nullptr) || (msgSize == 0U)) {
        LOGE() << "ums::Client::HandleResponse(): Invalid response message, msgSize =" << msgSize;
        return;
    }

    Message *const pMsg{static_cast< Message * >(pRspMsg)};
    if (pMsg->GetSize() != msgSize) {
        LOGE() << "ums::Client::HandleResponse(): Invalid Message Size, expected msg size =" << pMsg->GetSize();
        return;
    }

    Message::Operation const opt{pMsg->GetOperation()};

    UserSwclHandler cb{nullptr};
    switch (opt) {
        case Message::Operation::kGetUserSwclManifest: {
            cb = fGetUserSwclManifestCb_;
            break;
        }

        case Message::Operation::kUpdateUserSwcl: {
            cb = fUpdateUserSwclCb_;
            break;
        }

        default: {
            LOGE() << "ums::Client::HandleResponse(): Unsupported Operation {" << static_cast< uint16_t >(opt) << "}";
            break;
        }
    }

    if (cb == nullptr) {
        LOGE() << "ums::Client::HandleResponse(): nullptr == cb";
        return;
    }

    ara::core::StringView name;
    pMsg->GetUserSwclManifest(name);

    cb(name, pMsg->GetErrorCode());

    return;
}

}  // namespace ums
}  // namespace internal
}  // namespace exec
}  // namespace ara
