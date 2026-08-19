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
/// @file       environmental_condition_manager.cpp
/// @brief      This file implements environmental_condition_manager functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=data_management
/// @trace_id_sr=5df200f4-87da-45cb-b1e5-eb04d0158d42
/// @unit_name=EnvironmentalConditionManager
/// @unit_description=This file provides environmental_condition_manager functionality
/// @interface_level=unit
/// @endcode
///
/// ================================================================

#include "environmental_condition_manager.h"

#include "common/log.h"

namespace isoft {
namespace uds {
namespace server {
namespace environmental_condition_manager {
char const kEnvironmentalCondition[] = "environmentalCondition";
char const kId[]                     = "id";
}  // namespace environmental_condition_manager
EnvironmentalConditionManager::EnvironmentalConditionManager() noexcept : mapEnviromentCondition_{} {}

std::int32_t EnvironmentalConditionManager::Initialize(
    std::vector< EnvironmentalConditionModelPtr > const& modelTable,
    std::shared_ptr< DiagnosticDataManager >& diagnosticDataManagerPtr) noexcept
{
    std::int32_t result{0};
    for (auto&& model : modelTable) {
        if (model.get() == nullptr) {
            LogWarn() << "EnvironmentalConditionManager::Initialize| EnvironmentalConditionModelPtr is nullptr!";
            return -1;
        }
        std::shared_ptr< EnvironmentalCondition > envConditionPtr{};
        envConditionPtr = std::make_shared< EnvironmentalCondition >();
        result          = envConditionPtr->Initialize(*model, diagnosticDataManagerPtr);
        if (result != 0) {
            LogWarn()
                << "EnvironmentalConditionManager::Initialize|Please check EnvironmentalConditionModel config! id:"
                << model->id;
            return result;
        }
        std::ignore = mapEnviromentCondition_.insert({model->id, envConditionPtr});
    }
    return result;
}

}  // namespace server
}  // namespace uds
}  // namespace isoft