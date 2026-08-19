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
/// @file       engine_common.cpp
/// @brief      fw engine common processing part
/// @details    fw engine common processing part
/// @date       2025-05-06
/// @author     jzy
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/fw/Firewall Engine Abstraction Layer
/// @interface_level=module
/// @trace_id_sr=SR_FW_0010
/// @unit_name=Engine_Common
/// @unit_description=Firewall engine abstraction layer common part
/// @endcode
///
/// ================================================================
///
///
/// ================================================================

#include "ara/fw/filterengine/engine_common.h"

#include "ara/fw/internal/cmd_str.h"
#include "ara/fw/internal/log_api.h"

namespace ara {
namespace fw {
namespace internal {

/// @brief
/// @param strCmd
/// @return
bool CallSystem(ara::core::String const &strCmd) noexcept
{
    int32_t ret{system(strCmd.c_str())};  // NOLINT : [system] function is not thread safe
    if (ret != 0) {
        LogError() << "CallSystem: set system cmd  failed! return value:" << ret;
        return false;
    }
    return true;
}

/// @brief Verify whether the subnet mask is a valid value.
/// @param mask
/// @return true/false
bool Is_Valid_Mask(uint32_t mask) noexcept
{
    // Valid mask: continuous 1s followed by continuous 0s
    uint32_t inverted = ~mask;
    return ((inverted & (inverted + 1)) == 0);
}

/// @brief Check whether the string contains only hexadecimal characters
/// @param str Hexadecimal string
/// @return yes/not
bool Is_Hex_String(const std::string &str) noexcept
{
    for (char c : str) {
        if (!static_cast< bool >(std::isxdigit(c))) {
            return false;
        }
    }
    return !str.empty();
}

/// @brief Get Action Block
/// @return drop
ara::core::String GetActionBlock() noexcept
{
    ara::core::String actionBlock{" drop "};
    return actionBlock;
}

/// @brief Get Action Allow
/// @return accept
ara::core::String GetActionAllow() noexcept
{
    ara::core::String actionAllow{" accept "};
    return actionAllow;
}
/// @brief Get InputChain name
/// @return apinputchain
ara::core::String GetInputChain() noexcept
{
    ara::core::String inPutChain{" apinputchain "};
    return inPutChain;
}

/// @brief Get OutputChain name
/// @return apoutputchain
ara::core::String GetOutPutChain() noexcept
{
    ara::core::String outPutChain{" apoutputchain "};
    return outPutChain;
}

/// @brief Get netdev InputChain name ingress
/// @return apinputchain
ara::core::String GetNetDevInputChain() noexcept
{
    ara::core::String inNetDevPutChain{" ingresschain "};
    return inNetDevPutChain;
}

/// @brief Get netdev InputChain name ingress
/// @return apinputchain
ara::core::String GetNetDevOutputChain() noexcept
{
    ara::core::String inNetDevPutChain{" gresschain "};
    return inNetDevPutChain;
}

/// @brief Set packet flow rate (both ingress and egress can be set) command
/// @param cmdFirHalf First half of nft command
/// @param packetRate User flow rate setting
/// @return cmd command return value
void SetPacketRate(ara::core::String &nftCmd, int32_t packetRate) noexcept
{
    ara::core::String strLimitRate{std::to_string(packetRate)};

    nftCmd.append(GetkLimitRate() + strLimitRate + GetkLimitRateSecond());
    LogDebug() << "SetPacketRateFilter: limit cmd is " << nftCmd.c_str();
}

/// @brief Set packet flow rate and queue length command
/// @param nftFlowLenCmd First half of nft command
/// @param packetRate Flow rate
/// @param queueLength Queue length
/// @return cmd command return value
void SetPacketFlowLength(ara::core::String &nftFlowLenCmd, int32_t packetRate, int32_t queueLength) noexcept
{
    ara::core::String burst{" burst "};
    ara::core::String packets{" packets "};
    ara::core::String strLimitRate{std::to_string(packetRate)};
    ara::core::String strQueueLength{std::to_string(queueLength)};
    nftFlowLenCmd.append(GetkLimitRate() + strLimitRate + GetkLimitRateSecond() + burst + strQueueLength + packets);
    LogDebug() << "SetPacketFlowLength: limit cmd is " << nftFlowLenCmd.c_str();
}

/// @brief Check whether the user has set the rate
/// @param rate
/// @return true: rate field set false: not set
bool CheckRate(int32_t rate) noexcept { return (rate == -1 ? false : true); }

/// @brief Set log and action
/// @param cmdFirHalf First half of nft command
/// @param logPrefix nft log identifier
/// @param action Execution result
/// @return Add log command string
ara::core::String SetFirewallLogAndAction(ara::core::String &cmdFirHalf,
                                          int32_t action,
                                          ara::core::String const &logPrefix) noexcept
{
    ara::core::String cmd{};
    // Firewall logs only when action is drop.
    if (action == 0) {
        ara::core::String strLogPre{" log prefix "};
        ara::core::String strLogFlags{" log flags all "};
        cmdFirHalf.append(strLogPre + logPrefix + strLogFlags + GetkAddRuleForCounter() + GetActionBlock());
    } else {
        cmdFirHalf.append(GetkAddRuleForCounter() + (action == 0 ? GetActionBlock() : GetActionAllow()));
    }
    LogInfo() << "SetFirewallLogAndAction: log cmd is " << cmdFirHalf.c_str();
    return cmd;
}

/// @brief Check whether the user has set the rate
/// @param rate
/// @return true: rate field set false: not set
int32_t CheckRateAndQueueLen(int32_t rate, int32_t queueLen) noexcept
{
    // User did not configure rate and queue length
    if (rate == -1 && queueLen == -1) {
        return kNoRateAndQueueLen;
    }
    // User only configured rate
    if (rate != -1 && queueLen == -1) {
        return kRateOnly;
    }
    // User configured rate and queue length (if queue is configured, rate must be
    // configured)
    if (rate != -1 && queueLen != -1) {
        return kRateAndQueueLen;
    }
    return kNoRateAndQueueLen;
}

/// @brief Set rate and log command string
/// @param cmd First half of nft command
/// @param rate Rate
/// @param queueLen Queue length
/// @param action Action
/// @param logPrefix nft log identifier
void SetLogAndRata(
    ara::core::String &cmd, int32_t rate, int32_t queueLen, int32_t action, ara::core::String const &logPrefix) noexcept
{
    if (cmd.empty()) {
        LogWarn() << " not get nft cmd please check  your input.";
        return;
    }
    // User did not configure rate and queue length
    if (CheckRateAndQueueLen(rate, queueLen) == kNoRateAndQueueLen) {
        // action 0:block | 1:allow
        SetFirewallLogAndAction(cmd, action, logPrefix);
    }

    // User only configured rate
    if (CheckRateAndQueueLen(rate, queueLen) == kRateOnly) {
        // action 0:block | 1:allow
        SetPacketRate(cmd, rate);
        SetFirewallLogAndAction(cmd, action, logPrefix);
    }
    // User configured rate+length (queue length must be set together with rate if
    // set)
    if (CheckRateAndQueueLen(rate, queueLen) == kRateAndQueueLen) {
        // action 0:block | 1:allow
        SetPacketFlowLength(cmd, rate, queueLen);
        SetFirewallLogAndAction(cmd, action, logPrefix);
    }
}

}  // namespace internal
}  // namespace fw
}  // namespace ara
