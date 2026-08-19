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
/// @file       fault_manager.cpp
/// @brief      This file implements fault_manager functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=fault_management
/// @trace_id_sr=e5bc6e7e-4e1a-4b00-a4a2-be7e734a19b8
/// @unit_name=FaultManager
/// @unit_description=This file provides fault_manager functionality
/// @interface_level=
/// @endcode
///
/// ================================================================

#include "fault_management/fault_manager.h"

#include "common/log.h"
#include "fault_management/control_dtc_setting.h"
#include "fault_management/dtc_filter.h"
#include "fault_management/dtc_manager.h"
#include "fault_management/fault_management_runtime_context.h"
#include "isoft/uds/uds_nrc_error_domain.h"

namespace isoft {
namespace uds {
namespace server {


FaultManager::FaultManager()
{
    runtimeContext_ = std::make_shared< FaultManagementRuntimeContext >();
    pDtcMgr_        = std::make_shared< DtcManager >(*runtimeContext_);
    dtcFilter_      = std::make_shared< DtcFilter >(*runtimeContext_);
}

FaultManager::~FaultManager() noexcept = default;


std::int32_t FaultManager::Initialize(FaultManagement& config,
                                      std::shared_ptr< PersistenceInterface >& per,
                                      std::shared_ptr< DiagnosticDataManager >& dataManager)
{
    if (runtimeContext_.get() == nullptr) {
        LogWarn() << "FaultManager::Initialize -> runtimeContext_ is nullptr";
        return -1;
    }

    std::int32_t const ret{runtimeContext_->Initialize(config, per, dataManager)};
    initOk_ = (ret == 0);
    if (ret != 0) {
        LogInfo() << "FaultManager::Initialize -> runtimeContext_->Initialize fail. err=" << ret;
    }
    return 0;
}


std::int32_t FaultManager::Start()
{
    if (!initOk_) {
        return 0;
    }
    int32_t const res = runtimeContext_->Start();
    if (res != 0) {
        LogInfo() << "FaultManager::Start -> runtimeContext_->OfferServices fail. res=" << res;
    }
    return 0;
}


void FaultManager::Stop()
{
    if (!initOk_) {
        return;
    }
    runtimeContext_->Stop();
}


int32_t FaultManager::ClearDTC(uint32_t const groupOfDtc, bool const isUserDefined, uint8_t const memIdUserDefined)
{
    LogVerbose() << "FaultManager::ClearDTC from uds. groupOfDtc=" << groupOfDtc << ", isUserDefined=" << isUserDefined
                 << ", memIdUserDefined" << memIdUserDefined;

    if (pDtcMgr_.get() == nullptr) {
        LogWarn() << "FaultManager::ClearDTC pDtcMgr_ is nullptr";
        return static_cast< int32_t >(NrcErrc::kGeneralReject);
    }
    return pDtcMgr_->ClearDTC(groupOfDtc, isUserDefined, memIdUserDefined);
}

int32_t FaultManager::ControlDTCSetting(bool const on)
{
    LogVerbose() << "FaultManager::ControlDTCSetting from uds. on=" << on;
    std::shared_ptr< ControlDtcSetting > setting{runtimeContext_->GetDtcSetting()};
    if (setting == nullptr) {
        LogWarn() << "FaultManager::ControlDTCSetting setting is nullptr";
        return -1;
    }

    std::ignore = setting->Set(on);

    return 0;
}


std::shared_ptr< DtcFilter >& FaultManager::GetReadDtcInformationInterface() noexcept { return dtcFilter_; }

}  // namespace server
}  // namespace uds
}  // namespace isoft
