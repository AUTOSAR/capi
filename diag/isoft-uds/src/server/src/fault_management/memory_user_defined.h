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
/// @file       memory_user_defined.h
/// @brief      This file provides memory_user_defined functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/isoft/uds/fault_management
/// @interface_level=software
/// @trace_id_sr=3b25c33a-6c78-49cb-980d-f668fbdf9374
/// @unit_name=MemoryUserDefined
/// @unit_description=This file provides memory_user_defined functionality
/// @endcode
///
/// ================================================================

#ifndef __ISOFT_UDS_FAULT_MEMORY_USER_DEFINED_H__
#define __ISOFT_UDS_FAULT_MEMORY_USER_DEFINED_H__

#include <memory>

#include "fault_management/memory_destination.h"
namespace isoft {
namespace uds {
namespace server {

/// @brief user defined memory class
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02473
/// @needwork = dd
/// @endcode
class MemoryUserDefined : public MemoryDestination
{
public:
    /// @brief MemoryDestination copy constructor
    /// @param[in] other MemoryDestination reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00789
    /// @needwork = dda
    /// @endcode
    MemoryUserDefined(MemoryUserDefined const& other) = delete;
    /// @brief MemoryDestination move constructor
    /// @param[in] other MemoryDestination rvalue reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00790
    /// @needwork = dda
    /// @endcode
    MemoryUserDefined(MemoryUserDefined&& other) = delete;
    /// @brief MemoryDestination copy assignment operator
    /// @param[in] right MemoryDestination reference
    /// @return MemoryDestination reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00791
    /// @needwork = dda
    /// @endcode
    MemoryUserDefined& operator=(MemoryUserDefined const& right) = delete;
    /// @brief MemoryDestination move assignment operator
    /// @param[in] right MemoryDestination rvalue reference
    /// @return MemoryDestination reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00000
    /// @trace_id_dd=DD_UDS_00792
    /// @needwork = dda
    /// @endcode
    MemoryUserDefined& operator=(MemoryUserDefined&& right) = delete;

public:
    /// @brief Default constructor
    MemoryUserDefined() = delete;

    /// @brief Constructor
    /// @param context
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01593
    /// @needwork = dd
    /// @endcode
    MemoryUserDefined(FaultManagementRuntimeContext& context,
                      MemoryDestinationConfig& config,
                      std::string& clusterName,
                      std::shared_ptr< PersistenceInterface > per) noexcept
        : MemoryDestination{context, config, clusterName, per}
    {
    }

    /// @brief MemoryDestination destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01594
    /// @needwork = dd
    /// @endcode
    ~MemoryUserDefined() override = default;
};

}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif  // __ISOFT_UDS_FAULT_MEMORY_USER_DEFINED_H__