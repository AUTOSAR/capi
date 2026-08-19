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
/// @file       service_x38.cpp
/// @brief      This file implements service_x38 functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=transfer_managment
/// @trace_id_sr=a8c4265c-ca44-40c6-bb88-956fa3042bc3
/// @unit_name=service_x38.h
/// @unit_description=This file provides service_x38 functionality
/// @interface_level=unit
/// @endcode
///
/// ================================================================

#include "service_x38.h"

#include "common/log.h"
#include "common/serialize.h"
#include "isoft/uds/uds_nrc_error_domain.h"
namespace isoft {
namespace uds {
namespace server {

ServiceX38::ServiceX38() noexcept : transferManagerPtr_{nullptr} {}

std::int32_t ServiceX38::Initialize(std::shared_ptr< TransferManager > const& transferManagerPtr,
                                    uint32_t const p4ServerMax) noexcept
{
    if (transferManagerPtr.get() == nullptr) {
        LogError() << "ServiceX38::Initialize| ServiceX38::Initialize is nullptr!";
        return -1;
    }
    transferManagerPtr_ = transferManagerPtr;

    p4ServerMax_ = p4ServerMax;
    return 0;
}

std::vector< ValidatorPtr > ServiceX38::GetSidValidator() noexcept
{
    std::uint8_t const sid{0x38U};
    if (transferManagerPtr_.get() == nullptr) {
        LogError() << "ServiceX38::GetSidValidator| transferManagerPtr_ is nullptr! please check Initialize function!";
        return {};
    }
    return transferManagerPtr_->GetSidValidator(sid);
}





void ServiceX38::Process(std::shared_ptr< RequestContext > context) noexcept
{
    if (transferManagerPtr_.get() == nullptr) {
        LogError() << "ServiceX38::Process| transferManagerPtr_ is nullptr! please check Initialize function!";
        return;
    }
    return transferManagerPtr_->Process(context);
}

void ServiceX38::Confirm(RequestContext& request, ResponseStatusType status) noexcept
{
    if (transferManagerPtr_.get() == nullptr) {
        LogError() << "ServiceX38::Confirm| transferManagerPtr_ is nullptr! please check Initialize function!";
        return;
    }
    return transferManagerPtr_->Confirm(request, status);
}
}  // namespace server
}  // namespace uds
}  // namespace isoft
