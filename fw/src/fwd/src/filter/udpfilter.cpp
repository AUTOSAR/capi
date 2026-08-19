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
/// @file       udpfilter.cpp
/// @brief      udpfilter filter
/// @details    udpfilter filter
/// @date       2025-10-15
/// @author     jzy
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/fw/Filter Mode
/// @interface_level=module
/// @trace_id_sr=SR_FW_0002
/// @unit_name=Filter_Udp
/// @unit_description=Firewall Udp filter
/// @endcode
///
/// ================================================================
///
///
/// ================================================================

#include "udpfilter.h"

#include "ara/fw/internal/log_api.h"
namespace ara {
namespace fw {
namespace internal {

/// @brief udpfilter constructor
/// @param vecUdpRules Rule set
/// @param action Action
/// @param inOutFlag IN/OUT flag
/// @param pRate Rate
/// @param qLength Queue length
UdpFilter::UdpFilter(ara::core::Vector< PRuleParse > vecUdpRules,
                     int const &action,
                     int32_t const &inOutFlag,
                     int32_t pRate,
                     int32_t qLength) noexcept
    : vecUdpRules_(std::move(vecUdpRules))
    , unpUdpEngine_(std::make_unique< UdpEngine >(action, inOutFlag, pRate, qLength))
{
}

/// @brief Rule filtering
void UdpFilter::RulesFilter() noexcept
{
    LogInfo() << "UdpFilter::RulesFilter() in!";

    if (nullptr == unpUdpEngine_.get()) {
        LogInfo() << "UdpFilter::RulesFilter() get unpUdpEngine_ ptr is null!";
        return;
    }
    // Port engine filtering.
    unpUdpEngine_->SetPortRangeFilter(this->minSrcPortNum_, this->maxSrcPortNum_, this->minDestPortNum_,
                                      this->maxDestPortNum_);
}

/// @brief Get rules
void UdpFilter::GetRules() noexcept
{
    LogInfo() << "UdpFilter::GetRules() in!";
    if (vecUdpRules_.empty()) {
        LogError() << "UdpFilter::GetRules():vecUdpRules_ is null!, please check "
                      "manifest!";
        return;
    }
    for (auto const &it : vecUdpRules_) {
        if (it.ruleName == "checksumVerification") {
            it.ruleValue == "true" ? this->checksumVerification_ = true : this->checksumVerification_ = false;
        }
        if (it.ruleName == "maxDestinationPortNumber") {
            this->maxDestPortNum_ = std::stoi(it.ruleValue.c_str());
        }
        if (it.ruleName == "minDestinationPortNumber") {
            this->minDestPortNum_ = std::stoi(it.ruleValue.c_str());
        }
        if (it.ruleName == "maxSourcePortNumber") {
            this->maxSrcPortNum_ = std::stoi(it.ruleValue.c_str());
        }
        if (it.ruleName == "minSourcePortNumber") {
            this->minSrcPortNum_ = std::stoi(it.ruleValue.c_str());
        }
    }
}

}  // namespace internal
}  // namespace fw
}  // namespace ara