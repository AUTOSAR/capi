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
/// @file       payloadfilter.cpp
/// @brief      payloadfilter filter processing class
/// @details    payloadfilter filter processing class
/// @date       2024-12-31
/// @author     jzy
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/fw/Filter Mode
/// @interface_level=module
/// @trace_id_sr=SR_FW_0005
/// @unit_name=Filter_Payload
/// @unit_description=Firewall payload filter
/// @endcode
///
/// ================================================================
///
///
/// ================================================================

#include "payloadfilter.h"

#include "ara/fw/internal/log_api.h"
namespace ara {
namespace fw {
namespace internal {

/// @brief PayloadFilter constructor
/// @param vecPayloadRules Rule set
/// @param action Action
/// @param inOutFlag IN/OUT flag
/// @param pRate Rate
/// @param qLength Queue length
PayloadFilter::PayloadFilter(ara::core::Vector< PRuleParse > vecPayloadRules,
                             int const &action,
                             int32_t const &inOutFlag,
                             int32_t pRate,
                             int32_t qLength) noexcept
    : vecPayloadRules_(std::move(vecPayloadRules))
    , unpPayloadEngine_(std::make_unique< PayloadEngine >(action, inOutFlag, pRate, qLength))
{
}

/// @brief Rule filtering
void PayloadFilter::RulesFilter() noexcept
{
    LogInfo() << "PayloadFilter::RulesFilter() in!";
    // tcp port range validation
    unpPayloadEngine_->SetOffsetAndValueFilter(this->offset_, this->value_);
}

/// @brief Get tcp rules
void PayloadFilter::GetRules() noexcept
{
    LogInfo() << "PayloadFilter::GetRules() in!";
    if (vecPayloadRules_.empty()) {
        LogError() << "PayloadFilter::GetRules():vecPayloadRules_ is null!, please "
                      "check manifest!";
        return;
    }
    for (auto const &it : vecPayloadRules_) {
        if (it.ruleName == "offset") {
            this->offset_ = std::stoi(it.ruleValue.c_str());
        }
        if (it.ruleName == "value") {
            this->value_ = std::stoi(it.ruleValue.c_str());
        }
    }
}
}  // namespace internal
}  // namespace fw
}  // namespace ara