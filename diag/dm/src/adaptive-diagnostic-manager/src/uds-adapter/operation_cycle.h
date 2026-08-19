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
/// @file       operation_cycle.h
/// @brief      This file provides the definition of operation and related types.
/// @details
/// @date       2024-12-09
/// @author     zhanglipeng
/// @version    1.2.0
///
/// ================================================================

#ifndef __ARA_DIAG_FAULT_OPERATION_CYCLE_H__
#define __ARA_DIAG_FAULT_OPERATION_CYCLE_H__
#include <isoft/uds/fault_management/operation_cycle_interface.h>

#include "gen_code/operationCycle/serviceAgent/operationCycle_agent.h"

namespace ara {
namespace diag {
namespace dmd {

/// @brief monitor interface
class OperationCycle : public isoft::uds::server::OperationCycleInterface
{
public:
    OperationCycle(OperationCycle const&) = delete;
    OperationCycle(OperationCycle&&)      = delete;
    OperationCycle& operator=(OperationCycle const&) = delete;
    OperationCycle& operator=(OperationCycle&&) = delete;
    explicit OperationCycle(uint32_t const& serviceInstanceId) noexcept;
    ~OperationCycle() noexcept override = default;

public:
    /// @brief Notify AA side of operation cycle status changes
    /// @param operation Operation cycle status
    /// @return Returns 0 for successful call
    std::int32_t NotifyOperationCycle(isoft::uds::server::OperationCycleType const& operation) override;

    /// @brief Register callback for AA side to get the status of the operation cycle
    /// @param callBack Callback function
    /// @return Returns 0 for successful registration
    std::int32_t RegisterGetOperationCycle(isoft::uds::server::GetOperationCycleCallBack const& callBack) override;

    /// @brief Register callback for AA side to set the status of the operation cycle
    /// @param callBack Callback function
    /// @return Returns 0 for successful registration
    std::int32_t RegisterSetOperationCycle(isoft::uds::server::SetOperationCycleCallBack const& callBack) override;

private:
    std::unique_ptr< isoft::dm::dis::OperationCycleAgent > operationCycleAgent_;

    isoft::uds::server::GetOperationCycleCallBack getOpCb_{nullptr};
    isoft::uds::server::SetOperationCycleCallBack setOpCb_{nullptr};
};

}  // namespace dmd
}  // namespace diag
}  // namespace ara

#endif  // DM_UDS_GENERIC_UDS_SERVICE_H_