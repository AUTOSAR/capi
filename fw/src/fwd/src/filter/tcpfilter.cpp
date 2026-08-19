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
/// @file       tcpfilter.cpp
/// @brief      tcpfilter filter
/// @details    tcpfilter filter
/// @date       2025-10-15
/// @author     jzy
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/fw/Filter Mode
/// @interface_level=module
/// @trace_id_sr=SR_FW_0004
/// @unit_name=Filter_Tcp
/// @unit_description=Firewall Tcp filter
/// @endcode
///
/// ================================================================
///
///
/// ================================================================

#include "tcpfilter.h"

#include "ara/fw/internal/log_api.h"
namespace ara {
namespace fw {
namespace internal {

/// @brief TcpFilter filter
/// @param vecTcpRules Rule set
/// @param action Action
/// @param inOutFlag IN/OUT flag
/// @param pRate Rate
/// @param qLength Queue length
TcpFilter::TcpFilter(ara::core::Vector< PRuleParse > vecTcpRules,
                     int const &action,
                     int32_t const &inOutFlag,
                     int32_t pRate,
                     int32_t qLength) noexcept
    : vecTcpRules_(std::move(vecTcpRules))
    , unpTcpEngine_(std::make_unique< TcpEngine >(action, inOutFlag, pRate, qLength))
{
}

/// @brief Rule filtering
void TcpFilter::RulesFilter() noexcept
{
    LogInfo() << "TcpFilter::RulesFilter() in!";
    // tcp port range validation
    unpTcpEngine_->SetTcpPortRangeFilter(this->minSrcPortNum_, this->maxSrcPortNum_, this->minDestPortNum_,
                                         this->maxDestPortNum_);
    // tcp max session count limit filtering.
    unpTcpEngine_->SetTcpMaxSeesionFilter(this->numberOfParallelTcpSessions_);

    // tcp max session timeout filtering, currently not implemented.
    unpTcpEngine_->SetTcpTimeOutFilter(this->timeoutCheck_);

    // tcp set SM flags to verify that the network packet attempts to perform
    // allowed TCP state transitions.
    unpTcpEngine_->SetTcpSMFlagsFilter(this->stateManagementBasedOnTcpFlags_);
}

/// @brief Get tcp rules
void TcpFilter::GetRules() noexcept
{
    LogInfo() << "TcpFilter::GetRules() in!";
    if (vecTcpRules_.empty()) {
        LogError() << "TcpFilter::GetRules():vecTcpRules_ is null!, please check "
                      "manifest!";
        return;
    }
    for (auto const &it : vecTcpRules_) {
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
        if (it.ruleName == "numberOfParallelTcpSessions") {
            this->numberOfParallelTcpSessions_ = std::stoi(it.ruleValue.c_str());
        }
        if (it.ruleName == "stateManagementBasedOnTcpFlags") {
            it.ruleValue == "1" ? this->stateManagementBasedOnTcpFlags_ = true
                                : this->stateManagementBasedOnTcpFlags_ = false;
        }
        if (it.ruleName == "timeoutCheck") {
            this->timeoutCheck_ = std::stoi(it.ruleValue.c_str());
        }
    }
}

}  // namespace internal
}  // namespace fw
}  // namespace ara