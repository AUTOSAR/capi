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
/// @file       ipv6filter.cpp
/// @brief      ipv6 filter processing class
/// @details    ipv6 filter processing class
/// @date       2024-12-31
/// @author     jzy
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/fw/filter
/// @interface_level=module
/// @trace_id_sr=SR_FW_0002
/// @unit_name=Filter_Ipv6
/// @unit_description=Firewall ipv6 filter
/// @endcode
///
/// ================================================================
///
///
/// ================================================================

#include "ipv6filter.h"

#include "ara/fw/filterengine/engine_common.h"
#include "ara/fw/internal/log_api.h"
namespace ara {
namespace fw {
namespace internal {

/// @brief ipv6 constructor
/// @param vecIpv6Rules datalink rule set
/// @param action Processing method
/// @param inOutFlag input or output hook point
Ipv6Filter::Ipv6Filter(ara::core::Vector< PRuleParse > vecIpv6Rules,
                       int const &action,
                       int32_t const &inOutFlag,
                       int32_t pRate,
                       int32_t qLength) noexcept
    : vecIpv6Rules_(std::move(vecIpv6Rules))
    , unpIpv6Engine_(std::make_unique< Ipv6Engine >(action, inOutFlag, pRate, qLength))
    , unpIcmpEngine_(std::make_unique< IcmpEngine >(action, inOutFlag, 1, pRate, qLength))
{
}

/// @brief Rule filtering
void Ipv6Filter::RulesFilter() noexcept
{
    LogInfo() << "Ipv6Filter::RulesFilter() in!";

    if (nullptr == unpIpv6Engine_.get()) {
        LogError() << "Ipv6Filter::RulesFilter() get unpIpv6Engine_ ptr is null!";
        return;
    }

    // src srcMask dest destMask
    unpIpv6Engine_->SetIp6AddressFilter(this->srcIpAddress_, this->srcNetworkMask_, this->destMacAddress_,
                                        this->destMacAddressMask_);

    // flowlabel
    unpIpv6Engine_->SetFlowLabelFilter(this->flowLabel_);

    // hoplimit
    unpIpv6Engine_->SetHopLimitFilter(this->hopLimit_);

    // nextheader
    unpIpv6Engine_->SetNextHeaderFilter(this->nextHeader_);

    // trafficClass (upper 6 bits are dscp value, lower 2 bits are ecn value.)
    // This field value is 8bit.
    _setTraffiClassValue(this->trafficClass_);

    // icmpv6 code.
    unpIcmpEngine_->SetCodeFilter(this->icmpCode_);

    // icmpv6  type.
    unpIcmpEngine_->SetTypeFilter(this->icmpType_);
}

/// @brief Get filter rules
void Ipv6Filter::GetRules() noexcept
{
    LogInfo() << "Ipv6Filter::GetRules() in!";
    if (vecIpv6Rules_.empty()) {
        LogError() << "Ipv6Filter::GetRules():vecIpv6Rules_ is null!, please check "
                      "manifest!";
        return;
    }
    for (auto const &it : vecIpv6Rules_) {
        if (it.ruleName == "destinationIpAddress") {
            this->destMacAddress_ = it.ruleValue;
        }
        if (it.ruleName == "destinationNetworkMask") {
            this->destMacAddressMask_ = it.ruleValue;
        }
        if (it.ruleName == "flowLabel") {
            this->flowLabel_ = std::stoi(it.ruleValue.c_str());
        }
        if (it.ruleName == "hopLimit") {
            this->hopLimit_ = std::stoi(it.ruleValue.c_str());
        }
        if (it.ruleName == "icmpCheckSum") {
            it.ruleValue == "true" ? this->icmpCheckSum_ = true : this->icmpCheckSum_ = false;
        }
        if (it.ruleName == "icmpCode") {
            this->icmpCode_ = std::stoi(it.ruleValue.c_str());
        }
        if (it.ruleName == "icmpType") {
            this->icmpType_ = std::stoi(it.ruleValue.c_str());
        }
        if (it.ruleName == "nextHeader") {
            this->nextHeader_ = std::stoi(it.ruleValue.c_str());
        }
        if (it.ruleName == "sourceIpAddress") {
            this->srcIpAddress_ = it.ruleValue;
        }
        if (it.ruleName == "sourceNetworkMask") {
            this->srcNetworkMask_ = it.ruleValue;
        }
        if (it.ruleName == "trafficClass") {
            this->trafficClass_ = std::stoi(it.ruleValue.c_str());
        }
    }
}

/// @brief TrafficClass value for nft processing.
/// @param traffClass  ipv6 trafficlass.
void Ipv6Filter::_setTraffiClassValue(int32_t traffClass) noexcept
{
    if (traffClass < 0) {
        LogDebug() << "Ipv6Filter::_setTraffiClassValue() no configuration for "
                      "ipv6 traffClass.";
        return;
    }
    uint8_t dscp = static_cast< uint8_t >(traffClass) >> kIp6DscpShift;
    uint8_t ecn  = static_cast< uint8_t >(traffClass) << kIp6EcnShift;
    // dscp
    unpIpv6Engine_->SetIp6DscpFilter(dscp);
    // ecn
    unpIpv6Engine_->SetIp6ECNFilter(ecn);
}

}  // namespace internal
}  // namespace fw
}  // namespace ara