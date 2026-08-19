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
/// @file       dtc_manager.cpp
/// @brief      This file implements dtc_manager functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=fault_management
/// @trace_id_sr=2a9a2235-e901-4a80-8cb3-16d2d114c719
/// @unit_name=DtcManager
/// @unit_description=This file provides dtc_manager functionality
/// @interface_level=unit
/// @endcode
///
/// ================================================================

#include "fault_management/dtc_manager.h"

#include "fault_management/dtc.h"
#include "fault_management/dtc_group.h"
#include "fault_management/fault_management_runtime_context.h"
#include "fault_management/memory_destination.h"

namespace isoft {
namespace uds {
namespace server {

DtcManager::DtcManager(FaultManagementRuntimeContext& context) noexcept : context_{context} {}


int32_t DtcManager::ClearDTC(uint32_t const groupOfDtc, bool const isUserDefined, uint8_t const memIdUserDefined) const
{
    std::shared_ptr< MemoryDestination > dest{context_.GetPrimaryDestination()};
    if (isUserDefined) {
        dest = context_.GetDestination(static_cast< uint32_t >(memIdUserDefined));
    }
    if (nullptr == dest) {
        LogWarn() << "DtcManager::ClearDTC -> not found destination.";
        return static_cast< int32_t >(NrcErrc::kRequestOutOfRange);
    }

    if (!_CheckCondition(groupOfDtc)) {
        LogWarn() << "DtcManager::ClearDTC -> _CheckCondition|it is not all clear condition is ready";
        return static_cast< int32_t >(NrcErrc::kRequestOutOfRange);
    }

    int32_t err{0};
    std::shared_ptr< Dtc > const dtc{context_.GetDtcByValue(groupOfDtc)};
    std::shared_ptr< DtcGroup > const group{context_.GetDtcGroup(groupOfDtc)};
    if (dtc != nullptr) {
        err = dest->Clear(dtc, true);
    } else if (group != nullptr) {
        err = dest->Clear(group, true);
    } else {
        LogWarn() << "DtcManager::ClearDTC -> not found dtc or group id =" << groupOfDtc;
        return static_cast< int32_t >(NrcErrc::kRequestOutOfRange);
    }

    if (0 != err) {
        LogWarn() << "DtcManager::ClearDTC -> clear err=" << err;
    }
    return err;
}


bool DtcManager::_CheckCondition(uint32_t const groupOfDtc) const noexcept
{
    bool ret{true};
    if (!_IsDTCClearAllowedByCommonProperties(groupOfDtc)) {
        ret = false;
    }

    return ret;
}


bool DtcManager::_IsDTCConfigured(uint32_t const groupOfDtc) const
{
    // [SWS_DM_00092].
    if (nullptr != context_.GetDtcByValue(groupOfDtc)) {
        return true;
    }
    if (nullptr != context_.GetDtcGroup(groupOfDtc)) {
        return true;
    }

    return false;
}


bool DtcManager::_IsForbiddenClear(uint32_t const groupOfDtc) const
{
    // [SWS_DM_00122].
    // [SWS_DM_00163].
    // [SWS_DM_00164].
    std::shared_ptr< Dtc > const dtc{context_.GetDtcByValue(groupOfDtc)};
    std::shared_ptr< DtcGroup > const group{context_.GetDtcGroup(groupOfDtc)};
    if (dtc != nullptr) {
        return dtc->IsForbiddenClear();
    }
    if (group != nullptr) {
        return group->IsForbiddenClear();
    }

    return false;
}


bool DtcManager::_IsDTCClearAllowedByCommonProperties(uint32_t const groupOfDTCs) const noexcept
{
    // [SWS_DM_00159].
    // [SWS_DM_00160].
    bool allowed{true};
    if ((context_.GetCommonProps()->GetClearDtcLimitation() == DiagnosticClearDtcLimitationEnum::kClearAllDtcs)
        && (kGroupOfAllDTCs != groupOfDTCs)) {
        allowed = false;
    }

    return allowed;
}

}  // namespace server
}  // namespace uds
}  // namespace isoft
