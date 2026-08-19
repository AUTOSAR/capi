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
/// @file       hcaext_client.cpp
/// @brief
/// @details
/// @date       2024-06-20
/// @author     wangyanlong
/// @version    1.2.0
///
/// ================================================================

#include "ara/phm/internal/com/hcaext_client.h"

#include "ara/phm/internal/phm_log.h"

namespace ara {
namespace phm {
namespace internal {
namespace hcaextcom {

/// @brief When Offer, permit the recoveryhandler to be called.
/// @param instance instance specifier of health channel action.
/// @return int32_t 0 success; other failed.
int32_t Client::Offer(ara::core::String const& instance) noexcept
{
    LOG_INFO << "offer " << instance.c_str();

    Message const msg{EventType::kOffer, instance, TimeStamp::GetMs()};
    messageSerializer_.SetMsg(msg);
    Chunk const chunk{messageSerializer_.Serialize()};
    if (chunk.size() == static_cast< size_t >(0)) {
        LOG_ERROR << "serialize error.";
        return -1;
    }

    isoft::ipc::IPCPacket* const request{_MakeAndFillRequest(chunk)};
    if (request == nullptr) {
        LOG_ERROR << "_MakeAndFillRequest error.";
        return -1;
    }
    std::shared_ptr< isoft::ipc::IPCClient > ipcClient{GetIpcClient()};
    int32_t const ret{ipcClient->Post(request)};
    if (0 != ret) {
        LOG_ERROR << "ipc client " << GetAddr().c_str() << " post error.";
        return -1;
    }
    return 0;
}

/// @brief When StopOffer, do not permit the recoveryhandler to be called.
/// @param instance instance specifier of health channel action.
/// @return int32_t 0 success; other failed.
int32_t Client::StopOffer(ara::core::String const& instance) noexcept
{
    LOG_INFO << "stop offer " << instance.c_str();

    Message const msg{EventType::kStopOffer, instance, TimeStamp::GetMs()};
    messageSerializer_.SetMsg(msg);
    Chunk const chunk{messageSerializer_.Serialize()};
    if (chunk.size() == static_cast< size_t >(0)) {
        LOG_ERROR << "serialize error.";
        return -1;
    }

    isoft::ipc::IPCPacket* const request{_MakeAndFillRequest(chunk)};
    if (request == nullptr) {
        LOG_ERROR << "_MakeAndFillRequest error.";
        return -1;
    }
    std::shared_ptr< isoft::ipc::IPCClient > ipcClient{GetIpcClient()};
    int32_t const ret{ipcClient->Post(request)};
    if (0 != ret) {
        LOG_ERROR << "ipc client " << GetAddr().c_str() << " post error.";
        return -1;
    }
    return 0;
}

}  // namespace hcaextcom
}  // namespace internal
}  // namespace phm
}  // namespace ara
