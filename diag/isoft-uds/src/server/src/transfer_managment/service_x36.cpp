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
/// @file       service_x36.cpp
/// @brief      This file implements service_x36 functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=transfer_managment
/// @trace_id_sr=d127a46b-b430-4945-b255-b3e288a35ada
/// @unit_name=service_x36.h
/// @unit_description=This file provides service_x36 functionality
/// @interface_level=unit
/// @endcode
///
/// ================================================================

#include "service_x36.h"

#include "common/log.h"

namespace isoft {
namespace uds {
namespace server {
ServiceX36::ServiceX36() noexcept : transferManagerPtr_{nullptr} {}

std::int32_t ServiceX36::Initialize(std::shared_ptr< TransferManager > const& transferManagerPtr,
                                    TransferManagment& config) noexcept
{
    if (transferManagerPtr.get() == nullptr) {
        LogError() << "ServiceX36::Initialize| ServiceX36::Initialize is nullptr!";
        return -1;
    }
    transferManagerPtr_ = transferManagerPtr;

    if (config.requestDownload.get() != nullptr) {
        mapP4ServerMax_[RequestServerType::kDownload] = config.requestDownload->p4ServerMax_0x37;
    }

    if (config.requestUpload.get() != nullptr) {
        mapP4ServerMax_[RequestServerType::kUpload] = config.requestUpload->p4ServerMax_0x37;
    }

    if (config.requestFileTransfer.get() != nullptr) {
        mapP4ServerMax_[RequestServerType::kFileTransfer] = config.requestFileTransfer->p4ServerMax_0x37;
    }
    return 0;
}

std::vector< ValidatorPtr > ServiceX36::GetSidValidator() noexcept
{
    std::uint8_t const sid{0x36U};
    if (transferManagerPtr_.get() == nullptr) {
        LogError() << "ServiceX36::GetSidValidator| transferManagerPtr_ is nullptr! please check Initialize function!";
        return {};
    }
    return transferManagerPtr_->GetSidValidator(sid);
}





void ServiceX36::Process(std::shared_ptr< RequestContext > context) noexcept
{
    if (transferManagerPtr_.get() == nullptr) {
        LogError() << "ServiceX36::Process| transferManagerPtr_ is nullptr! please check Initialize function!";
        return;
    }
    return transferManagerPtr_->Process(std::move(context));
}

void ServiceX36::Confirm(RequestContext& request, ResponseStatusType status) noexcept
{
    if (transferManagerPtr_.get() == nullptr) {
        LogError() << "ServiceX36::Confirm| transferManagerPtr_ is nullptr! please check Initialize function!";
        return;
    }
    return transferManagerPtr_->Confirm(request, status);
}

std::uint32_t ServiceX36::GetP4ServerMax(std::int16_t const subfunction, std::int32_t const assistParam) noexcept
{
    std::ignore = subfunction;
    std::ignore = assistParam;
    if (transferManagerPtr_.get() == nullptr) {
        LogError() << "ServiceX36::GetP4ServerMax|transferManagerPtr_ is nullptr! please check Initialize function!";
        return 0U;
    }

    RequestServerType& type                              = transferManagerPtr_->GetRequestServerTpye();
    std::map< RequestServerType, uint32_t >::iterator it = mapP4ServerMax_.find(type);
    if (it == mapP4ServerMax_.end()) {
        LogWarn() << "ServiceX36::GetP4ServerMax|mapP4ServerMax don't have p4, request+type ="
                  << static_cast< uint16_t >(type);
        return 0U;
    }

    return it->second;
}

}  // namespace server
}  // namespace uds
}  // namespace isoft
