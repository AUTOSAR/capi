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
/// @file       datalinkfilter.cpp
/// @brief      Data link layer filter processing class
/// @details    Data link layer filter processing class
/// @date       2024-12-31
/// @author     jzy
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/fw/Filter Mode
/// @interface_level=module
/// @trace_id_sr=SR_FW_0002
/// @unit_name=Filter_Datalink
/// @unit_description=Firewall data link layer filter.
/// @endcode
///
/// ================================================================
///
///
/// ================================================================

#include "datalinkfilter.h"

#include <string>

#include "ara/fw/internal/log_api.h"

namespace ara {
namespace fw {
namespace internal {

/// @brief Constructor
/// @param vecDataLinkRules
/// @param action
/// @param inOutFlag
DataLinkFilter::DataLinkFilter(ara::core::Vector< PRuleParse > vecDataLinkRules,
                               int32_t const &action,
                               int32_t const &inOutFlag,
                               int32_t pRate,
                               int32_t qLength) noexcept
    : dataLinkRules_(std::move(vecDataLinkRules))
    , unpDataLinkEngine_(std::make_unique< DataLinkEngine >(action, inOutFlag, pRate, qLength))
{
}

/// @brief Rule filtering
/// @throws
void DataLinkFilter::RulesFilter() noexcept
{
    LogInfo() << "DataLinkFilter::RulesFilter() in!";
    // set srcAddrMac  and DestAddrMac
    unpDataLinkEngine_->SetMacAddressFilter(this->srcMacAddress_, this->srcAddressMask_, this->destMacAddress_,
                                            this->destMacAddressMask_);

    // set ether type.
    unpDataLinkEngine_->SetEtherTypeFilter(this->etherType_);
}

/// @brief
void DataLinkFilter::GetRules() noexcept
{
    LogInfo() << "DataLinkFilter::GetRules() in!";
    if (dataLinkRules_.empty()) {
        LogError() << "DataLinkFilter::GetRules():dataLinkRules_ is null!, please "
                      "check manifest!";
        return;
    }
    for (auto const &it : dataLinkRules_) {
        if (it.ruleName == "destinationMacAddress") {
            this->destMacAddress_ = it.ruleValue;
        }
        if (it.ruleName == "destinationMacAddressMask") {
            this->destMacAddressMask_ = it.ruleValue;
        }
        if (it.ruleName == "etherType") {
            this->etherType_ = std::stoi(it.ruleValue.c_str());
        }
        if (it.ruleName == "sourceMacAddress") {
            this->srcMacAddress_ = it.ruleValue;
        }
        if (it.ruleName == "sourceMacAddressMask") {
            this->srcAddressMask_ = it.ruleValue;
        }
        if (it.ruleName == "vlanId") {
            this->vlanId_ = std::stoi(it.ruleValue.c_str());
        }
        if (it.ruleName == "vlanPriority") {
            this->vlanPriority_ = std::stoi(it.ruleValue.c_str());
        }
    }
}

}  // namespace internal
}  // namespace fw
}  // namespace ara