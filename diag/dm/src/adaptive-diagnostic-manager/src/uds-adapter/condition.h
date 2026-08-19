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
/// @file       condition.h
/// @brief      This file provides the definition of Condition and related types.
/// @details
/// @date       2024-12-09
/// @author     zhanglipeng
/// @version    1.2.0
///
/// ================================================================

#ifndef __ARA_DIAG_FAULT_CONDITION_H__
#define __ARA_DIAG_FAULT_CONDITION_H__
#include <isoft/uds/fault_management/condition_interface.h>

#include "gen_code/condition/serviceAgent/condition_agent.h"

namespace ara {
namespace diag {
namespace dmd {

/// @brief monitor interface
class Condition : public isoft::uds::server::ConditionInterface
{
public:
    Condition(Condition const&) = delete;
    Condition(Condition&&)      = delete;
    Condition& operator=(Condition const&) = delete;
    Condition& operator=(Condition&&) = delete;
    explicit Condition(uint32_t const& serviceInstanceId) noexcept;
    ~Condition() noexcept override = default;

public:
    /// @brief Register callback for AA side to set condition status
    /// @param callBack Callback function
    /// @return Returns 0 for successful registration
    std::int32_t RegisterSetCondition(isoft::uds::server::SetConditionCallBack const& callBack) override;

    /// @brief Register callback for AA side to get condition status
    /// @param callBack Callback function
    /// @return Returns 0 for successful registration
    std::int32_t RegisterGetCondition(isoft::uds::server::GetConditionCallBack const& callBack) override;

private:
    std::unique_ptr< isoft::dm::dis::ConditionAgent > conditionAgent_;

    isoft::uds::server::SetConditionCallBack setCb_{nullptr};
    isoft::uds::server::GetConditionCallBack getCb_{nullptr};
};

}  // namespace dmd
}  // namespace diag
}  // namespace ara

#endif  // DM_UDS_GENERIC_UDS_SERVICE_H_