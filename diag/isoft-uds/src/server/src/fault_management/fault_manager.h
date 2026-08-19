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
/// @file       fault_manager.h
/// @brief      This file provides fault_manager functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @module_path=fault_management
/// @interface_level=
/// @trace_id_sr=e5bc6e7e-4e1a-4b00-a4a2-be7e734a19b8
/// @unit_name=FaultManager
/// @unit_description=This file provides fault_manager functionality
///
/// ================================================================

#ifndef __ISOFT_UDS_FAULT_FAULT_MANAGER_H_
#define __ISOFT_UDS_FAULT_FAULT_MANAGER_H_

#include <memory>
#include <string>

#include "data_management/diagnostic_data_manager.h"
#include "isoft/uds/fault_management/fault_management.h"

namespace isoft {
namespace uds {
namespace server {

class FaultManagementRuntimeContext;

class DtcManager;

class DtcFilter;

/// @brief fault management external interface
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UDS_01291
/// @trace_id_dd=DD_UDS_02527
/// @needwork = dd
/// @endcode
class FaultManager final
{
public:
    /// @brief Copy constructor
    /// @param[in] object Reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01746
    /// @needwork = dd
    /// @endcode
    FaultManager(FaultManager const& object) = default;

    /// @brief Copy assignment function
    /// @param[in] object Reference
    /// @return Reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01747
    /// @needwork = dd
    /// @endcode
    FaultManager& operator=(FaultManager const& object) = default;

    /// @brief Move constructor
    /// @param[in, out] object Rvalue reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01748
    /// @needwork = dd
    /// @endcode
    FaultManager(FaultManager&& object) = default;

    /// @brief Move assignment function
    /// @param[in, out] object Rvalue reference
    /// @return Reference
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01749
    /// @needwork = dd
    /// @endcode
    FaultManager& operator=(FaultManager&& object) = default;

    /// @brief Constructor
    /// @param[in] dataProvider Data provider
    /// @param[in] dataIdentifierManager Data identifier manager
    /// @code{.isoft}
    /// @throw
    /// @needwork = dd
    /// @endcode
    FaultManager();

    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01750
    /// @needwork = dd
    /// @endcode
    ~FaultManager() noexcept;

    /// @brief Initialize event manager
    /// @param[in] dmc Diagnostic management configuration
    /// @param[in] storage Used to store diagnostic fault data
    /// @return Returns 0 on success, non-zero on failure
    /// @code{.isoft}
    /// @throw
    /// @needwork = dd
    /// @endcode
    std::int32_t Initialize(FaultManagement& config,
                            std::shared_ptr< PersistenceInterface >& per,
                            std::shared_ptr< DiagnosticDataManager >& dataManager);

    /// @brief Start event manager
    /// @return Returns 0 on success, non-zero on failure
    /// @code{.isoft}
    /// @throw
    /// @needwork = dd
    /// @endcode
    std::int32_t Start();

    /// @brief Stop event manager
    /// @code{.isoft}
    /// @throw
    /// @needwork = dd
    /// @endcode
    void Stop();

    /// @brief The subfunction of UDS service ClearDiagnosticInformation will be called here
    /// @param[in] groupOfDtc dtc group or single dtc passed in by external tester
    /// @param[in] isUserDefined Whether to clear user defined destination specified by external tester
    /// @param[in] memIdUserDefined When parameter isUserDefined is true, this value is valid, indicating the id of the destination to be cleared
    /// @return result
    /// @code{.isoft}
    /// @throw
    /// @needwork = dd
    /// @endcode
    int32_t ClearDTC(uint32_t const groupOfDtc, bool const isUserDefined, uint8_t const memIdUserDefined);

    /// @brief The subfunction of UDS service ControlDTCSetting will be called here
    /// @param on Parameter passed in by external tester, true or false, indicating the switch of control dtc setting
    /// @return result
    /// @code{.isoft}
    /// @throw
    /// @needwork = dd
    /// @endcode
    int32_t ControlDTCSetting(bool const on);

    /// @brief Get the implementation interface of UDS service ReadDtcInformation, the subfunctions of ReadDtcInformation are more complex and encapsulated with an independent interface
    /// @return DemReadDtcInformationInterface interface
    /// @code{.isoft}
    /// @throw
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< DtcFilter >& GetReadDtcInformationInterface() noexcept;

private:
    /// @brief Runtime context
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01751
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< FaultManagementRuntimeContext > runtimeContext_;

    /// @brief DTC manager
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01752
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< DtcManager > pDtcMgr_;

    /// @brief DTC filter
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01753
    /// @needwork = dd
    /// @endcode
    std::shared_ptr< DtcFilter > dtcFilter_;

    /// @brief Initialization flag
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UDS_00888
    /// @trace_id_dd=DD_UDS_01754
    /// @needwork = dd
    /// @endcode
    bool initOk_{false};
};
}  // namespace server
}  // namespace uds
}  // namespace isoft

#endif  // ARA_DIAG_DEM_EVENT_MANAGER_H_