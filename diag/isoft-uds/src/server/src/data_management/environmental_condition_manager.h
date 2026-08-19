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
/// @file       environmental_condition_manager.h
/// @brief      This file provides environmental_condition_manager functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/isoft/uds/data_management
/// @interface_level=software
/// @trace_id_sr=5df200f4-87da-45cb-b1e5-eb04d0158d42
/// @unit_name=EnvironmentalConditionManager
/// @unit_description=This file provides environmental_condition_manager functionality
/// @endcode
///
/// ================================================================

#ifndef ISOFT_UDS_SERVER_ENVIRONMENTAL_CONDITION_MANGER_H_
#define ISOFT_UDS_SERVER_ENVIRONMENTAL_CONDITION_MANGER_H_

#include <cstdint>
#include <memory>

#include "environmental_condition.h"

namespace isoft {
namespace uds {
namespace server {

/// @brief Environmental condition manager
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = dd
/// @endcode
class EnvironmentalConditionManager
{
    /// @brief Environmental condition mapping table
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    std::map< EnvironmentalConditionId, std::shared_ptr< EnvironmentalCondition > > mapEnviromentCondition_;

public:
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00743
    /// @needwork = dda
    /// @endcode
    EnvironmentalConditionManager(EnvironmentalConditionManager const&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00744
    /// @needwork = dda
    /// @endcode
    EnvironmentalConditionManager& operator=(EnvironmentalConditionManager const&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00745
    /// @needwork = dda
    /// @endcode
    EnvironmentalConditionManager(EnvironmentalConditionManager&&) = delete;
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00746
    /// @needwork = dda
    /// @endcode
    EnvironmentalConditionManager& operator=(EnvironmentalConditionManager&&) = delete;



    /// @brief Constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    explicit EnvironmentalConditionManager() noexcept;

    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    ~EnvironmentalConditionManager() noexcept = default;

    /// @brief Initialize environmental condition manager
    /// @param[in] modelTable Environmental condition model table
    /// @param[in] diagnosticDataManagerPtr Diagnostic data manager pointer
    /// @return 0: Success
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    std::int32_t Initialize(std::vector< EnvironmentalConditionModelPtr > const& modelTable,
                            std::shared_ptr< DiagnosticDataManager >& diagnosticDataManagerPtr) noexcept;

    /// @brief Get environmental condition
    /// @param[in] id
    /// @return
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dd
    /// @endcode
    inline std::shared_ptr< EnvironmentalCondition > Get(EnvironmentalConditionId const id) noexcept
    {
        std::map< EnvironmentalConditionId, std::shared_ptr< EnvironmentalCondition > >::const_iterator const result{
            mapEnviromentCondition_.find(id)};
        if (result != mapEnviromentCondition_.end()) {
            return result->second;
        }
        return {nullptr};
    }
};

}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif  // ISOFT_UDS_SERVER_ENVIRONMENTAL_CONDITION_MANGER_H_