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
/// @file       engine_ipv6.cpp
/// @brief      ipv6 Rule
/// @details    ipv6 Rule
/// @date       2025-06-04
/// @author     jzy
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/fw/Firewall Engine Abstraction Layer
/// @interface_level=module
/// @trace_id_sr=SR_FW_0010
/// @unit_name=Engine_Ipv6
/// @unit_description=Firewall engine abstraction layer ipv6 data abstraction
/// processing class
/// @endcode
///
/// ================================================================
///
///
/// ================================================================

#include "ara/fw/filterengine/engine_ipv6.h"

#include <string>

#include "ara/fw/filterengine/engine_common.h"
#include "ara/fw/internal/cmd_str.h"
#include "ara/fw/internal/log_api.h"

namespace ara {
namespace fw {
namespace internal {

Ipv6Engine::Ipv6Engine(int32_t action, int32_t inOutFlag, int32_t pRate, int32_t qLength) noexcept
    : action_(action), inOutFlag_(inOutFlag), packetRate_(pRate), queueLength_(qLength)
{
}
/// @brief Set flow label rule filtering
/// @param dscp flowlabel filter value
void Ipv6Engine::SetFlowLabelFilter(int32_t flowLabel) const noexcept
{
    ara::core::String cmd{};
    std::string strflowLabel{std::to_string(flowLabel)};
    // User did not configure.
    if (flowLabel < 0) {
        LogDebug() << "SetFlowLabelFilter:  flowlabel  is not configuration.";
        return;
    }

    cmd = GetkAddRuleForNftCmdTable() + (this->inOutFlag_ == 0 ? GetInputChain() : GetOutPutChain())
          + GetkAddRuleForFlowLabel() + strflowLabel;

    // Set rate and log command
    SetLogAndRata(cmd, this->packetRate_, this->queueLength_, this->action_, this->logPrefix_);

    LogDebug() << "[SetFlowLabelFilter] CallSystem cmd is :" << cmd;

    if (!CallSystem(cmd)) {
        LogError() << "SetFlowLabelFilter: flowlabel filter set failed!";
        return;
    }
}

/// @brief Set hopLimit rule filtering
/// @param hopLimit User set hopLimit value
void Ipv6Engine::SetHopLimitFilter(int32_t hopLimit) const noexcept
{
    ara::core::String cmd{};
    std::string strHopLimit{std::to_string(hopLimit)};

    // User did not configure.
    if (hopLimit < 0) {
        LogDebug() << "SetHopLimitFilter:  hopLimit  is not configuration.";
        return;
    }

    cmd = GetkAddRuleForNftCmdTable() + (this->inOutFlag_ == 0 ? GetInputChain() : GetOutPutChain())
          + GetkAddRuleForHopLimit() + strHopLimit;

    // Set rate and log command
    SetLogAndRata(cmd, this->packetRate_, this->queueLength_, this->action_, this->logPrefix_);
    LogDebug() << "[SetHopLimitFilter] CallSystem cmd is :" << cmd;

    if (!CallSystem(cmd)) {
        LogError() << "SetHopLimitFilter: hopLimit  filter set failed!";
        return;
    }
}

/// @brief Set next header
/// @param hdrLength User set ipv6 nextheader field value
void Ipv6Engine::SetNextHeaderFilter(int32_t nexthdr) const noexcept
{
    ara::core::String cmd{};
    std::string strNexthdr{std::to_string(nexthdr)};

    // User did not configure.
    if (nexthdr < 0) {
        LogDebug() << "SetNextHeaderFilter:  nexthdr  is not configuration.";
        return;
    }

    cmd = GetkAddRuleForNftCmdTable() + (this->inOutFlag_ == 0 ? GetInputChain() : GetOutPutChain())
          + GetkAddRuleForNexthdr() + strNexthdr;

    // Set rate and log command
    SetLogAndRata(cmd, this->packetRate_, this->queueLength_, this->action_, this->logPrefix_);

    LogDebug() << "[SetECNFilter] CallSystem cmd is :" << cmd;

    if (!CallSystem(cmd)) {
        LogError() << "SetInputECNFilter: ecn filter set failed!";
        return;
    }
}

/// @brief Set DSCP rule filtering (ipv6)
/// @param dscp DSCP filter value
void Ipv6Engine::SetIp6DscpFilter(uint8_t dscp) const noexcept
{
    ara::core::String cmd{};
    std::string strDscp{std::to_string(static_cast< uint32_t >(dscp))};
    cmd = GetkAddRuleForNftCmdTable() + (this->inOutFlag_ == 0 ? GetInputChain() : GetOutPutChain())
          + GetkAddRuleForIp6Dscp() + strDscp;

    // Set rate and log command
    SetLogAndRata(cmd, this->packetRate_, this->queueLength_, this->action_, this->logPrefix_);

    LogDebug() << "[SetIp6DscpFilter] CallSystem cmd is :" << cmd;

    if (!CallSystem(cmd)) {
        LogError() << "SetIp6DscpFilter: dscp filter set failed!";
        return;
    }
}

/// @brief Set ECN rule filtering
/// @param ecn User set ECN value
void Ipv6Engine::SetIp6ECNFilter(uint8_t ecn) const noexcept
{
    ara::core::String cmd{};
    std::string strEcn{std::to_string(static_cast< uint32_t >(ecn))};

    cmd = GetkAddRuleForNftCmdTable() + (this->inOutFlag_ == 0 ? GetInputChain() : GetOutPutChain())
          + GetkAddRuleForIp6Ecn() + strEcn;

    // Set rate and log command
    SetLogAndRata(cmd, this->packetRate_, this->queueLength_, this->action_, this->logPrefix_);

    LogDebug() << "[SetIp6ECNFilter] CallSystem cmd is :" << cmd;

    if (!CallSystem(cmd)) {
        LogError() << "SetIp6ECNFilter: ecn filter set failed!";
        return;
    }
}

/// @brief Set source address and destination address rule filtering
/// @param srcAddress Source address
/// @param srcAddrMask Source address
/// @param srcAddress Source address
/// @param destAddress Destination address
void Ipv6Engine::SetIp6AddressFilter(ara::core::String const &srcAddr,
                                     ara::core::String const &srcAddrMask,
                                     ara::core::String const &destAddr,
                                     ara::core::String const &destAddrMask) noexcept
{
    ara::core::String cmd;
    // User did not configure source address and destination address verification.
    if (srcAddr.empty() && destAddr.empty()) {
        LogDebug() << "SetIp6AddressFilter:  srcAddr  and  destAddr is empty!";
        return;
    }
    // If both source and destination addresses are configured for filtering, the
    // destination address is configured as the local machine's IP, and the source
    // address is the external address IP. If the subnet mask of srcAddr and
    // destAddr is not empty, the IP range filtering needs to be specified.
    if (!srcAddr.empty() && !destAddr.empty() && !srcAddrMask.empty() && !destAddrMask.empty()) {
        // Calculate src ip range
        ara::core::String strSrcIpRange{_calIp6Range(srcAddr, srcAddrMask)};
        // Calculate dest ip range
        ara::core::String strDestIpRange{_calIp6Range(destAddr, destAddrMask)};

        if (strSrcIpRange.empty() || strDestIpRange.empty()) {
            LogError() << "SetIp6AddressFilter: get saddr/daddr range failed! ";
            return;
        }

        // action 0:block | 1:allow
        cmd = GetkAddRuleForNftCmdTable() + (this->inOutFlag_ == 0 ? GetInputChain() : GetOutPutChain())
              + GetkAddRuleForIp6SrcAddr() + strSrcIpRange + GetkAddRuleForIp6DestAddr() + strDestIpRange;
        // Set rate and log command
        SetLogAndRata(cmd, this->packetRate_, this->queueLength_, this->action_, this->logPrefix_);

        LogDebug() << "[SetIp6AddressFilter] CallSystem cmd is :" << cmd;
        if (!CallSystem(cmd)) {
            LogError() << "SetIp6AddressFilter: saddr and daddr filter set failed!";
            return;
        }
    }
    // If both source and destination addresses are configured for filtering, the
    // destination address is configured as the local machine's IP, and the source
    // address is the external address IP.
    if (!srcAddr.empty() && !destAddr.empty() && srcAddrMask.empty() && destAddrMask.empty()) {
        // action 0:block | 1:allow
        cmd = GetkAddRuleForNftCmdTable() + (this->inOutFlag_ == 0 ? GetInputChain() : GetOutPutChain())
              + GetkAddRuleForIp6SrcAddr() + srcAddr + GetkAddRuleForIp6DestAddr() + destAddr;

        // Set rate and log command
        SetLogAndRata(cmd, this->packetRate_, this->queueLength_, this->action_, this->logPrefix_);

        LogDebug() << "[SetIp6AddressFilter] CallSystem cmd is :" << cmd;
        if (!CallSystem(cmd)) {
            LogError() << "SetIp6AddressFilter: saddr and daddr filter set failed!";
            return;
        }
    }

    // Only configure srcAddr, not srcMask.
    if (!srcAddr.empty() && destAddr.empty() && srcAddrMask.empty()) {
        // action 0:block | 1:allow
        cmd = GetkAddRuleForNftCmdTable() + (this->inOutFlag_ == 0 ? GetInputChain() : GetOutPutChain())
              + GetkAddRuleForIp6SrcAddr() + srcAddr;

        // Set rate and log command
        SetLogAndRata(cmd, this->packetRate_, this->queueLength_, this->action_, this->logPrefix_);

        LogDebug() << "[SetIp6AddressFilter] CallSystem cmd is :" << cmd;
        if (!CallSystem(cmd)) {
            LogError() << "SetIp6AddressFilter: saddr  filter set failed!";
            return;
        }
    }
    // Only configure destAddr, not destMask.
    if (srcAddr.empty() && !destAddr.empty() && destAddrMask.empty()) {
        // action 0:block | 1:allow
        cmd = GetkAddRuleForNftCmdTable() + (this->inOutFlag_ == 0 ? GetInputChain() : GetOutPutChain())
              + GetkAddRuleForIp6DestAddr() + destAddr;

        // Set rate and log command
        SetLogAndRata(cmd, this->packetRate_, this->queueLength_, this->action_, this->logPrefix_);

        LogDebug() << "[SetIp6AddressFilter] CallSystem cmd is :" << cmd;
        if (!CallSystem(cmd)) {
            LogError() << "SetIp6AddressFilter: daddr  filter set failed!";
            return;
        }
    }
    // Configure src and srcmask. dest not configured.
    if (!srcAddr.empty() && !srcAddrMask.empty() && destAddr.empty()) {
        ara::core::String strSrcIpRange{_calIp6Range(srcAddr, srcAddrMask)};
        if (strSrcIpRange.empty()) {
            LogError() << "SetIp6AddressFilter: get saddr range failed! ";
            return;
        }

        // action 0:block | 1:allow
        cmd = GetkAddRuleForNftCmdTable() + (this->inOutFlag_ == 0 ? GetInputChain() : GetOutPutChain())
              + GetkAddRuleForIp6SrcAddr() + strSrcIpRange;

        // Set rate and log command
        SetLogAndRata(cmd, this->packetRate_, this->queueLength_, this->action_, this->logPrefix_);

        LogDebug() << "[SetIp6AddressFilter] CallSystem cmd is :" << cmd;
        if (!CallSystem(cmd)) {
            LogError() << "SetIp6AddressFilter: saddr  filter set failed!";
            return;
        }
    }
    // Configure dest and destmask. src not configured
    if (!destAddr.empty() && !destAddrMask.empty() && srcAddr.empty()) {
        ara::core::String strDestIpRange{_calIp6Range(destAddr, destAddrMask)};
        if (strDestIpRange.empty()) {
            LogError() << "SetIp6AddressFilter: get daddr range failed! ";
            return;
        }

        // action 0:block | 1:allow
        cmd = GetkAddRuleForNftCmdTable() + (this->inOutFlag_ == 0 ? GetInputChain() : GetOutPutChain())
              + GetkAddRuleForIp6DestAddr() + strDestIpRange;

        // Set rate and log command
        SetLogAndRata(cmd, this->packetRate_, this->queueLength_, this->action_, this->logPrefix_);

        LogDebug() << "[SetIp6AddressFilter] CallSystem cmd is :" << cmd;
        if (!CallSystem(cmd)) {
            LogError() << "SetIp6AddressFilter: daddr  filter set failed!";
            return;
        }
    }
    return;
}

/// @brief parse ipv6 subnet mask configuration.
/// @param ipv6Str
/// @return
IPv6 Ipv6Engine::_parseIPv6(ara::core::String const &ipv6Str) noexcept
{
    // Handle empty string
    if (ipv6Str.empty()) {
        LogError() << "_parseIPv6:ipv6Str is  null!";
        return IPv6(0, 0);
    }

    ara::core::String str{ipv6Str};
    // Remove possible square brackets (e.g., [::1])
    if (str.front() == '[' && str.back() == ']') {
        str = str.substr(1, str.size() - 2);
    }

    // Check double colon position
    size_t dcolonPos = str.find("::");
    bool hasDcolon   = (dcolonPos != std::string::npos);

    // Split string
    std::vector< std::string > parts;
    std::string part;

    // Handle double colon
    if (hasDcolon) {
        // Parts before and after double colon
        ara::core::String left{str.substr(0, dcolonPos)};
        ara::core::String right{str.substr(dcolonPos + 2)};

        // Parse left part
        if (!left.empty()) {
            std::stringstream leftSs(left.c_str());
            while (std::getline(leftSs, part, ':')) {
                if (!part.empty()) {
                    parts.emplace_back(part);
                }
            }
        }

        // Add empty part to indicate double colon
        parts.emplace_back("");

        // Parse right part
        if (!right.empty()) {
            std::stringstream rightSs(right.c_str());
            while (std::getline(rightSs, part, ':')) {
                if (!part.empty()) {
                    parts.emplace_back(part);
                }
            }
        }
    } else {
        // No double colon, split directly
        std::stringstream ss(str.c_str());
        while (std::getline(ss, part, ':')) {
            parts.emplace_back(part);
        }
    }

    // Handle double colon expansion (up to 8 segments)
    std::vector< uint16_t > segments;
    int32_t expectedSegments{kIp6Segment};
    int32_t dcolonIndex{-1};
    int32_t partCount = static_cast< int32_t >(parts.size());

    // Find double colon position
    for (int32_t i = 0; i < partCount; i++) {
        if (parts[i].empty()) {
            if (dcolonIndex != -1) {
                LogError() << "_parseIPv6:Multiple '::' in IPv6 address";
                return IPv6(0, 0);
            }
            dcolonIndex = i;
        }
    }

    // Handle case with no double colon
    if (dcolonIndex == -1) {
        if (partCount != expectedSegments) {
            LogError() << "_parseIPv6:IPv6 address must have exactly 8 segments";
            return IPv6(0, 0);
        }
        for (const auto &p : parts) {
            if (p.empty()) {
                LogError() << "_parseIPv6:Empty segment in IPv6 address";
                return IPv6(0, 0);
            }
            segments.emplace_back(static_cast< uint16_t >(std::stoul(p, nullptr, kIp6Value16)));
        }
    }
    // Handle case with double colon
    else {
        int32_t leftCount  = dcolonIndex;
        int32_t rightCount = partCount - dcolonIndex - 1;
        int32_t zeroCount  = expectedSegments - (leftCount + rightCount);

        if (zeroCount < 1) {
            LogError() << "_parseIPv6:Too many segments in IPv6 address";
            return IPv6(0, 0);
        }

        // Add left segments
        for (int32_t i = 0; i < leftCount; i++) {
            segments.emplace_back(static_cast< uint16_t >(std::stoul(parts[i], nullptr, kIp6Value16)));
        }

        // Add zero segments
        for (int32_t i = 0; i < zeroCount; i++) {
            segments.emplace_back(0);
        }

        // Add right segments
        for (int32_t i = dcolonIndex + 1; i < partCount; i++) {
            segments.emplace_back(static_cast< uint16_t >(std::stoul(parts[i], nullptr, kIp6Value16)));
        }
    }

    // Verify segment count
    if (segments.size() != kIp6Segment) {
        LogError() << "_parseIPv6:IPv6 address must have exactly 8 segments";
        return IPv6(0, 0);
    }

    // Combine into 128-bit address
    uint64_t high = (static_cast< uint64_t >(segments[0]) << kIp6Value48)
                    | (static_cast< uint64_t >(segments[1]) << kIp6Value32)
                    | (static_cast< uint64_t >(segments[2]) << kIp6Value16) | static_cast< uint64_t >(segments[3]);

    uint64_t low = (static_cast< uint64_t >(segments[4]) << kIp6Value48)
                   | (static_cast< uint64_t >(segments[kIp6Segment5]) << kIp6Value32)
                   | (static_cast< uint64_t >(segments[kIp6Segment6]) << kIp6Value16)
                   | static_cast< uint64_t >(segments[kIp6Segment7]);

    return IPv6(high, low);
}

/// @brief Format IPv6 address to string
/// @param addr
/// @return
std::string Ipv6Engine::_formatIPv6(const IPv6 &addr) noexcept
{
    // Extract 8 16-bit segments
    std::vector< uint16_t > segments;
    segments.push_back(static_cast< uint16_t >((addr.high >> kIp6Value48) & kIp6Uint16Max));
    segments.push_back(static_cast< uint16_t >((addr.high >> kIp6Value32) & kIp6Uint16Max));
    segments.push_back(static_cast< uint16_t >((addr.high >> kIp6Value16) & kIp6Uint16Max));
    segments.push_back(static_cast< uint16_t >(addr.high & kIp6Uint16Max));
    segments.push_back(static_cast< uint16_t >((addr.low >> kIp6Value48) & kIp6Uint16Max));
    segments.push_back(static_cast< uint16_t >((addr.low >> kIp6Value32) & kIp6Uint16Max));
    segments.push_back(static_cast< uint16_t >((addr.low >> kIp6Value16) & kIp6Uint16Max));
    segments.push_back(static_cast< uint16_t >(addr.low & kIp6Uint16Max));

    // Find longest continuous 0 segment
    int maxZeroLen{0};
    int maxZeroStart{-1};
    int currentZeroLen{0};
    int currentZeroStart{-1};

    for (int32_t i = 0; i < kIp6Segment; i++) {
        if (segments[i] == 0) {
            if (currentZeroLen == 0) {
                currentZeroStart = i;
            }
            currentZeroLen++;
        } else {
            if (currentZeroLen > maxZeroLen) {
                maxZeroLen   = currentZeroLen;
                maxZeroStart = currentZeroStart;
            }
            currentZeroLen = 0;
        }
    }

    // Check continuous 0 at the end
    if (currentZeroLen > maxZeroLen) {
        maxZeroLen   = currentZeroLen;
        maxZeroStart = currentZeroStart;
    }

    // Generate string
    std::ostringstream oss;
    bool compressed = false;
    bool first      = true;

    for (int32_t i = 0; i < kIp6Segment; i++) {
        // Skip compressed 0 segments
        if (maxZeroLen > 1 && i >= maxZeroStart && i < maxZeroStart + maxZeroLen) {
            if (!compressed) {
                if (first) {
                    oss << "::";
                } else {
                    oss << ":";
                }
                compressed = true;
            }
            continue;
        }

        // Non-0 segment or single 0
        if (!first) {
            oss << ":";
        }

        first = false;

        if (segments[i] != 0) {
            oss << std::hex << segments[i];
        } else {
            // Single 0 segment needs to be retained
            oss << "0";
        }
    }

    // Handle all-0 address
    if (compressed && maxZeroStart == 0 && maxZeroLen == kIp6Segment) {
        return "::";
    }

    return oss.str();
}

ara::core::String Ipv6Engine::_calIp6Range(ara::core::String const &ipv6Str,
                                           ara::core::String const &ipv6StrMask) noexcept
{
    IPv6 strIpv6{_parseIPv6(ipv6Str)};
    IPv6 strMaskIpv6{_parseIPv6(ipv6StrMask)};
    ara::core::String strRange{};
    if ((!strIpv6.CheckVaild()) || (!strMaskIpv6.CheckVaild())) {
        LogError() << "_calIp6Range: ipv6 Invaild please check configuration.";
        return strRange;
    }
    // Calculate network address and broadcast address
    IPv6 network   = strIpv6 & strMaskIpv6;
    IPv6 broadcast = strIpv6 | ~strMaskIpv6;
    // Calculate host range
    IPv6 minHost = network + 1;
    IPv6 maxHost = broadcast - 1;

    // Verify range validity. Within max and min range. Return corresponding
    // ipRange. Otherwise report error.
    if (minHost <= maxHost) {
        std::string range{};
        range = _formatIPv6(minHost) + "-" + _formatIPv6(maxHost);
        strRange.append(range.c_str());
        LogDebug() << "_calIp6Range: ipv6 strAddr range:" << strRange;
        return strRange;
    }
    LogError() << "_calIp6Range: no ipv6 iprange can be  used.";
    return strRange;
}

}  // namespace internal
}  // namespace fw
}  // namespace ara