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
/// @file       engine_icmp.cpp
/// @brief      icmp rule filtering
/// @details    icmp rule filtering
/// @date       2025-05-28
/// @author     jzy
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/fw/Firewall Engine Abstraction Layer
/// @interface_level=module
/// @trace_id_sr=SR_FW_0010
/// @unit_name=Engine_Icmp
/// @unit_description=Firewall engine abstraction layer icmp data abstraction
/// processing class
/// @endcode
///
/// ================================================================
///
///
/// ================================================================

#include "ara/fw/filterengine/engine_icmp.h"

#include "ara/fw/filterengine/engine_common.h"
#include "ara/fw/internal/cmd_str.h"
#include "ara/fw/internal/log_api.h"

namespace ara {
namespace fw {
namespace internal {

IcmpEngine::IcmpEngine(int32_t action, int32_t inOutFlag, int32_t ipv4Or6Flag, int32_t pRate, int32_t qLength) noexcept
    : action_(action), inOutFlag_(inOutFlag), ipv4Or6Flag_(ipv4Or6Flag), packetRate_(pRate), queueLength_(qLength)
{
}

/// @brief Set minimum IP header filtering (for input hook point)
/// @param type User set ipv4 header minimum value.
void IcmpEngine::SetTypeFilter(int32_t type) const noexcept
{
    ara::core::String cmd{};
    std::string strType{std::to_string(type)};
    // User did not configure.
    if (type < 0) {
        LogInfo() << "SetTypeFilter: table[apfilter] chain[apinput/outputchain]  "
                     "icmp type  is not configuration.";
        return;
    }

    cmd = GetkAddRuleForNftCmdTable() + (this->inOutFlag_ == 0 ? GetInputChain() : GetOutPutChain())
          + (this->ipv4Or6Flag_ == 0 ? GetkAddRuleForIcmpType() : GetkAddRuleForIcmpIP6Type()) + strType;

    // Set rate and log command
    SetLogAndRata(cmd, this->packetRate_, this->queueLength_, this->action_, this->logPrefix_);

    LogDebug() << "Icmp:[SetTypeFilter] CallSystem cmd is :" << cmd;

    if (!CallSystem(cmd)) {
        LogError() << "SetTypeFilter: table[apfilter]   chain[in/out] icmp type "
                      "filter set failed!";
        return;
    }
}

/// @brief Set Icmp Code filtering rule
/// @param code icmp Code value
void IcmpEngine::SetCodeFilter(int32_t code) const noexcept
{
    ara::core::String cmd{};
    std::string strCode{std::to_string(code)};
    // User did not configure.
    if (code < 0) {
        LogInfo() << "SetTypeFilter: table[apfilter] chain[apinputchain]  ecn  is "
                     "not configuration.";
        return;
    }

    cmd = GetkAddRuleForNftCmdTable() + (this->inOutFlag_ == 0 ? GetInputChain() : GetOutPutChain())
          + (this->ipv4Or6Flag_ == 0 ? GetkAddRuleForIcmpCode() : GetkAddRuleForIcmpIp6Code()) + strCode;

    // Set rate and log command
    SetLogAndRata(cmd, this->packetRate_, this->queueLength_, this->action_, this->logPrefix_);

    LogDebug() << "Icmp:[SetCodeFilter] CallSystem cmd is :" << cmd;

    if (!CallSystem(cmd)) {
        LogError() << "SetCodeFilter: table[apfilter]  chain[in/out] icmp code "
                      "filter set failed!";
        return;
    }
}

/// @brief Set whether to perform Icmp checksum rule filtering (currently the
/// Linux kernel performs checksum by default, this function does nothing)
/// @param checkSumFlag Whether to perform checksum flag.
void IcmpEngine::SetCheckSumFilter(bool checkSumFlag) noexcept
{
    if (checkSumFlag) {
        return;
    }
}

}  // namespace internal
}  // namespace fw
}  // namespace ara