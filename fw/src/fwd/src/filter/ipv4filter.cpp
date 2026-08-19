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
/// @file       ipv4filter.cpp
/// @brief      ipv4 filter processing class
/// @details    ipv4 filter processing class
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
/// @unit_name=Filter_Ipv4
/// @unit_description=Firewall ipv4 filter
/// @endcode
///
/// ================================================================
///
///
/// ================================================================

#include "ipv4filter.h"

#include "ara/fw/internal/log_api.h"
namespace ara {
namespace fw {
namespace internal {

/// @brief ipv4 constructor initialization
/// @param vecIpv4Rules datalink rule set
/// @param action Processing method
/// @param inOutFlag input or output hook point
Ipv4Filter::Ipv4Filter(ara::core::Vector< PRuleParse > vecIpv4Rules,
                       int32_t const &action,
                       int32_t const &inOutFlag,
                       int32_t pRate,
                       int32_t qLength) noexcept
    : vecIpv4Rules_(std::move(vecIpv4Rules))
    , unpIpv4Engine_(std::make_unique< Ipv4Engine >(action, inOutFlag, pRate, qLength))
    , unpIcmpEngine_(std::make_unique< IcmpEngine >(action, inOutFlag, 0, pRate, qLength))
{
}

/// @brief Ipv4 performs actual filtering.
void Ipv4Filter::RulesFilter() noexcept
{
    LogInfo() << "Ipv4Filter::RulesFilter() in!";
    if (nullptr == unpIpv4Engine_.get()) {
        LogError() << "Ipv4Filter::RulesFilter() get unpIpv4Engine_ ptr is null!";
        return;
    }

    // ip filter.
    unpIpv4Engine_->SetAddressFilter(this->srcIpAddress_, this->srcNetworkMask_, this->destMacAddress_,
                                     this->destMacAddressMask_);
    // protocol
    unpIpv4Engine_->SetProtocolFilter(this->protocol_);
    // ip  dscp.
    unpIpv4Engine_->SetDscpFilter(this->differentiatedServiceCodePoint_);

    // ip MF/DF fragmentation
    unpIpv4Engine_->SetDFOrMFFilter(this->doNotFragment_, this->moreFragments_);

    // ip ecn
    unpIpv4Engine_->SetECNFilter(this->explicitCongestionNotification_);

    // icmp code.
    unpIcmpEngine_->SetCodeFilter(this->icmpCode_);

    // icmp type.
    unpIcmpEngine_->SetTypeFilter(this->icmpType_);

    // ip interheaderLength.
    unpIpv4Engine_->SetHdrLengthFilter(this->internetHeaderLength_);

    // ttl.
    unpIpv4Engine_->SetTtlFilter(this->ttlMin_, this->ttlMax_);
}

/// @brief Get ipv4Rules
void Ipv4Filter::GetRules() noexcept
{
    LogInfo() << "Ipv4Filter::GetRules() in!";
    if (vecIpv4Rules_.empty()) {
        LogError() << "Ipv4Filter::GetRules():vecIpv4Rules_ is null!, please check "
                      "manifest!";
        return;
    }
    for (auto const &it : vecIpv4Rules_) {
        if (it.ruleName == "checksumVerification") {
            // Whether checksum is enabled cannot be decided by nft command; Linux
            // kernel does checksum by default.
            it.ruleValue == "true" ? this->checksumVerification_ = true : this->checksumVerification_ = false;
        }
        if (it.ruleName == "destinationIpAddress") {
            this->destMacAddress_ = it.ruleValue;
        }
        if (it.ruleName == "destinationNetworkMask") {
            this->destMacAddressMask_ = it.ruleValue;
        }
        if (it.ruleName == "differentiatedServiceCodePoint") {
            this->differentiatedServiceCodePoint_ = std::stoi(it.ruleValue.c_str());
        }
        if (it.ruleName == "doNotFragment") {
            it.ruleValue == "1" ? this->doNotFragment_ = true : this->doNotFragment_ = false;
        }
        if (it.ruleName == "explicitCongestionNotification") {
            this->explicitCongestionNotification_ = std::stoi(it.ruleValue.c_str());
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
        if (it.ruleName == "internetHeaderLength") {
            this->internetHeaderLength_ = std::stoi(it.ruleValue.c_str());
        }
        if (it.ruleName == "moreFragments") {
            it.ruleValue == "1" ? this->moreFragments_ = true : this->moreFragments_ = false;
        }
        if (it.ruleName == "protocol") {
            this->protocol_ = std::stoi(it.ruleValue.c_str());
        }
        if (it.ruleName == "sourceIpAddress") {
            this->srcIpAddress_ = it.ruleValue;
        }
        if (it.ruleName == "sourceNetworkMask") {
            this->srcNetworkMask_ = it.ruleValue;
        }
        if (it.ruleName == "ttlMin") {
            this->ttlMin_ = std::stoi(it.ruleValue.c_str());
        }
        if (it.ruleName == "ttlMax") {
            this->ttlMax_ = std::stoi(it.ruleValue.c_str());
        }
    }
}

}  // namespace internal
}  // namespace fw
}  // namespace ara