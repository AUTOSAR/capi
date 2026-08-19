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
/// @file       service_x35.cpp
/// @brief      This file implements service_x35 functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=transfer_managment
/// @trace_id_sr=3c60794a-89d5-44eb-9913-58d9a89f1805
/// @unit_name=service_x35.h
/// @unit_description=This file provides service_x35 functionality
/// @interface_level=unit
/// @endcode
///
/// ================================================================

#include "service_x35.h"

#include <bitset>

#include "common/log.h"
namespace isoft {
namespace uds {
namespace server {
ServiceX35::ServiceX35() noexcept : transferManagerPtr_{} {}

std::int32_t ServiceX35::Initialize(std::shared_ptr< TransferManager > const& transferManagerPtr,
                                    uint32_t const p4ServerMax) noexcept
{
    if (transferManagerPtr.get() == nullptr) {
        LogError() << "ServiceX35::Initialize| ServiceX35::Initialize is nullptr!";
        return -1;
    }
    transferManagerPtr_ = transferManagerPtr;

    p4ServerMax_ = p4ServerMax;
    return 0;
}

std::vector< ValidatorPtr > ServiceX35::GetSidValidator() noexcept
{
    std::uint8_t const sid{0x35U};
    if (transferManagerPtr_.get() == nullptr) {
        LogError() << "ServiceX35::GetSidValidator| transferManagerPtr_ is nullptr! please check Initialize function!";
        return {};
    }
    return transferManagerPtr_->GetSidValidator(sid);
}





void ServiceX35::Process(std::shared_ptr< RequestContext > context) noexcept
{
    if (transferManagerPtr_.get() == nullptr) {
        LogError() << "ServiceX35::Process| transferManagerPtr_ is nullptr! please check Initialize function!";
        return;
    }
    return transferManagerPtr_->Process(std::move(context));
}

void ServiceX35::Confirm(RequestContext& request, ResponseStatusType status) noexcept
{
    if (transferManagerPtr_.get() == nullptr) {
        LogError() << "ServiceX35::Confirm| transferManagerPtr_ is nullptr! please check Initialize function!";
        return;
    }
    return transferManagerPtr_->Confirm(request, status);
}
}  // namespace server
}  // namespace uds
}  // namespace isoft
