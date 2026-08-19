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
/// @file       engine_ipv4.cpp
/// @brief      ipv4 Rule
/// @details    ipv4 Rule
/// @date       2025-05-23
/// @author     jzy
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/fw/Firewall Engine Abstraction Layer
/// @interface_level=module
/// @trace_id_sr=SR_FW_0010
/// @unit_name=Engine_Ipv4
/// @unit_description=Firewall engine abstraction layer ipv4 data abstraction
/// processing class
/// @endcode
///
/// ================================================================
///
///
/// ================================================================

#include "ara/fw/filterengine/engine_ipv4.h"

#include <arpa/inet.h>
#include <netinet/in.h>

#include <cctype>

#include "ara/fw/filterengine/engine_common.h"
#include "ara/fw/internal/cmd_str.h"
#include "ara/fw/internal/log_api.h"

namespace ara {
namespace fw {
namespace internal {

Ipv4Engine::Ipv4Engine(int32_t action, int32_t inOutFlag, int32_t pRate, int32_t qLength) noexcept
    : action_(action), inOutFlag_(inOutFlag), packetRate_(pRate), queueLength_(qLength)
{
}

/// @brief Set address filtering command
/// @param iProtocol Protocol type
void Ipv4Engine::SetProtocolFilter(int32_t iProtocol) const noexcept
{
    if (iProtocol < 0) {
        LogDebug() << "SetProtocolFilter: no configuration for ipv4 protocol.";
        return;
    }
    ara::core::String strProtocol;
    ara::core::String cmd;
    switch (static_cast< Ipv4Protocol >(iProtocol)) {
        case Ipv4Protocol::kIcmp: {
            strProtocol = "icmp";
            break;
        }

        case Ipv4Protocol::kTcp: {
            strProtocol = "tcp";
            break;
        }

        case Ipv4Protocol::kUdp: {
            strProtocol = "udp";
            break;
        }
        default:
            break;
    }

    // action 0:block | 1:allow
    // inoutFlag 0:in | 1:out
    cmd = GetkAddRuleForNftCmdTable() + (this->inOutFlag_ == 0 ? GetInputChain() : GetOutPutChain())
          + GetkAddRuleForIpProtocol() + strProtocol;

    // Set rate and log command
    SetLogAndRata(cmd, this->packetRate_, this->queueLength_, this->action_, this->logPrefix_);

    LogDebug() << "[SetProtocolFilter] CallSystem cmd is :" << cmd;
    if (!CallSystem(cmd)) {
        LogError() << "SetProtocolFilter: protocol filter set failed!";
    }
}

/// @brief Set source address and destination address rule filtering
/// @param srcAddress Source address
/// @param srcAddrMask Source address MASK
/// @param destAddr Destination address
/// @param destAddrMask Destination address MASK
void Ipv4Engine::SetAddressFilter(ara::core::String const &srcAddr,
                                  ara::core::String const &srcAddrMask,
                                  ara::core::String const &destAddr,
                                  ara::core::String const &destAddrMask) const noexcept
{
    ara::core::String cmd{};
    // User did not configure source address and destination address verification.
    if (srcAddr.empty() && destAddr.empty()) {
        LogInfo() << "SetAddressFilter:  srcAddr  and  destAddr is empty!";
        return;
    }
    // If both source and destination addresses are configured for filtering, the
    // destination address is configured as the local machine's IP, and the source
    // address is the external address IP. If the subnet mask of srcAddr and
    // destAddr is not empty, the IP range filtering needs to be specified.
    if (!srcAddr.empty() && !destAddr.empty() && !srcAddrMask.empty() && !destAddrMask.empty()) {
        ara::core::String strSrcIpRange{_getIpRange(srcAddr, srcAddrMask)};
        ara::core::String strDestIpRange{_getIpRange(destAddr, destAddrMask)};
        if (strSrcIpRange.empty() || strDestIpRange.empty()) {
            LogError() << "SetAddressFilter: get saddr/daddr range failed! ";
            return;
        }

        // action 0:block | 1:allow
        cmd = GetkAddRuleForNftCmdTable() + (this->inOutFlag_ == 0 ? GetInputChain() : GetOutPutChain())
              + GetkAddRuleForSrcAddr() + strSrcIpRange + GetkAddRuleForDestAddr() + strDestIpRange;

        // Set rate and log command
        SetLogAndRata(cmd, this->packetRate_, this->queueLength_, this->action_, this->logPrefix_);
        LogDebug() << "[SetAddressFilter] CallSystem cmd is :" << cmd;
        if (!CallSystem(cmd)) {
            LogError() << "SetInputAddressFilter: saddr and daddr filter set failed!";
            return;
        }
    }
    // If both source and destination addresses are configured for filtering, the
    // destination address is configured as the local machine's IP, and the source
    // address is the external address IP.
    if (!srcAddr.empty() && !destAddr.empty() && srcAddrMask.empty() && destAddrMask.empty()) {
        // action 0:block | 1:allow
        cmd = GetkAddRuleForNftCmdTable() + (this->inOutFlag_ == 0 ? GetInputChain() : GetOutPutChain())
              + GetkAddRuleForSrcAddr() + srcAddr + GetkAddRuleForDestAddr() + destAddr;

        // Set rate and log command
        SetLogAndRata(cmd, this->packetRate_, this->queueLength_, this->action_, this->logPrefix_);

        LogDebug() << "[SetAddressFilter] CallSystem cmd is :" << cmd;
        if (!CallSystem(cmd)) {
            LogError() << "SetInputAddressFilter: saddr and daddr filter set failed!";
            return;
        }
    }

    // Only configure srcAddr, not srcMask.
    if (!srcAddr.empty() && destAddr.empty() && srcAddrMask.empty()) {
        // action 0:block | 1:allow
        cmd = GetkAddRuleForNftCmdTable() + (this->inOutFlag_ == 0 ? GetInputChain() : GetOutPutChain())
              + GetkAddRuleForSrcAddr() + srcAddr;

        // Set rate and log command
        SetLogAndRata(cmd, this->packetRate_, this->queueLength_, this->action_, this->logPrefix_);
        LogDebug() << "[SetAddressFilter] CallSystem cmd is :" << cmd;
        if (!CallSystem(cmd)) {
            LogError() << "SetInputAddressFilter: saddr  filter set failed!";
            return;
        }
    }
    // Only configure destAddr, not destMask.
    if (srcAddr.empty() && !destAddr.empty() && destAddrMask.empty()) {
        // action 0:block | 1:allow
        cmd = GetkAddRuleForNftCmdTable() + (this->inOutFlag_ == 0 ? GetInputChain() : GetOutPutChain())
              + GetkAddRuleForDestAddr() + destAddr;

        // Set rate and log command
        SetLogAndRata(cmd, this->packetRate_, this->queueLength_, this->action_, this->logPrefix_);

        LogDebug() << "[SetAddressFilter] CallSystem cmd is :" << cmd;
        if (!CallSystem(cmd)) {
            LogError() << "SetInputAddressFilter: daddr  filter set failed!";
            return;
        }
    }
    // Configure src and srcmask. dest not configured.
    if (!srcAddr.empty() && !srcAddrMask.empty() && destAddr.empty()) {
        ara::core::String strSrcIpRange{_getIpRange(srcAddr, srcAddrMask)};
        if (strSrcIpRange.empty()) {
            LogError() << "SetAddressFilter: get saddr range failed! ";
            return;
        }

        // action 0:block | 1:allow
        cmd = GetkAddRuleForNftCmdTable() + (this->inOutFlag_ == 0 ? GetInputChain() : GetOutPutChain())
              + GetkAddRuleForSrcAddr() + strSrcIpRange;

        // Set rate and log command
        SetLogAndRata(cmd, this->packetRate_, this->queueLength_, this->action_, this->logPrefix_);

        LogDebug() << "[SetAddressFilter] CallSystem cmd is :" << cmd;
        if (!CallSystem(cmd)) {
            LogError() << "SetAddressFilter: saddr  filter set failed!";
            return;
        }
    }
    // Configure dest and destmask. src not configured
    if (!destAddr.empty() && !destAddrMask.empty() && srcAddr.empty()) {
        ara::core::String strDestIpRange{_getIpRange(destAddr, destAddrMask)};
        if (strDestIpRange.empty()) {
            LogError() << "SetAddressFilter: get daddr range failed! ";
            return;
        }

        // action 0:block | 1:allow
        cmd = GetkAddRuleForNftCmdTable() + (this->inOutFlag_ == 0 ? GetInputChain() : GetOutPutChain())
              + GetkAddRuleForDestAddr() + strDestIpRange;

        // Set rate and log command
        SetLogAndRata(cmd, this->packetRate_, this->queueLength_, this->action_, this->logPrefix_);

        LogDebug() << "[SetAddressFilter] CallSystem cmd is :" << cmd;
        if (!CallSystem(cmd)) {
            LogError() << "SetInputAddressFilter: daddr  filter set failed!";
            return;
        }
    }
    return;
}

/// @brief Set TTL rule filtering
/// @param ttlMin ttl minimum time
/// @param ttlMax ttl maximum time
void Ipv4Engine::SetTtlFilter(int32_t ttlMin, int32_t ttlMax) const noexcept
{
    ara::core::String cmd{};
    ara::core::String strTtlMin{std::to_string(ttlMin)};
    ara::core::String strTtlMax{std::to_string(ttlMax)};
    // User did not configure TTL, return directly.
    if (ttlMin == -1 && ttlMax == -1) {
        LogDebug() << "SetInputTtlFilter:  ttlMax ttlMin is not configuration.";
        return;
    }
    // User configured ttlMin value but did not configure ttlmax.
    if (ttlMin != -1 && ttlMax == -1) {
        // action 0:block | 1:allow
        cmd = GetkAddRuleForNftCmdTable() + (this->inOutFlag_ == 0 ? GetInputChain() : GetOutPutChain())
              + " ip ttl \\> " + strTtlMin;

        // Set rate and log command
        SetLogAndRata(cmd, this->packetRate_, this->queueLength_, this->action_, this->logPrefix_);

        LogDebug() << "[SetTtlFilter] CallSystem cmd is :" << cmd;
        if (!CallSystem(cmd)) {
            LogError() << "SetInputTtlFilter: ttlMin  filter set failed!";
            return;
        }
    }
    // User configured ttlMax value but did not configure ttlMin.
    if (ttlMin == -1 && ttlMax != -1) {
        // action 0:block | 1:allow
        cmd = GetkAddRuleForNftCmdTable() + (this->inOutFlag_ == 0 ? GetInputChain() : GetOutPutChain())
              + " ip ttl \\< " + strTtlMax;

        // Set rate and log command
        SetLogAndRata(cmd, this->packetRate_, this->queueLength_, this->action_, this->logPrefix_);

        LogDebug() << "[SetTtlFilter] CallSystem cmd is :" << cmd;
        if (!CallSystem(cmd)) {
            LogError() << "SetInputTtlFilter: ttlMax  filter set failed!";
            return;
        }
    }
    // User configured both ttlMax and ttlMin.
    if (ttlMin != -1 && ttlMax != -1) {
        // action 0:block | 1:allow
        cmd = GetkAddRuleForNftCmdTable() + (this->inOutFlag_ == 0 ? GetInputChain() : GetOutPutChain()) + " ip ttl "
              + strTtlMin + "-" + strTtlMax;

        // Set rate and log command
        SetLogAndRata(cmd, this->packetRate_, this->queueLength_, this->action_, this->logPrefix_);

        LogDebug() << "[SetTtlFilter] CallSystem cmd is :" << cmd;

        if (!CallSystem(cmd)) {
            LogError() << "SetInputTtlFilter:  ttlMin - ttlMax filter set failed!";
            return;
        }
    }
}

/// @brief Set DSCP rule filtering
/// @param dscp DSCP filter value
void Ipv4Engine::SetDscpFilter(int32_t dscp) const noexcept
{
    ara::core::String cmd{};
    ara::core::String strDscp{std::to_string(dscp)};
    // User did not configure.
    if (dscp < 0) {
        LogDebug() << "SetInputDscpFilter:  dscp  is not configuration.";
        return;
    }

    cmd = GetkAddRuleForNftCmdTable() + (this->inOutFlag_ == 0 ? GetInputChain() : GetOutPutChain())
          + GetkAddRuleForDscp() + strDscp;

    // Set rate and log command
    SetLogAndRata(cmd, this->packetRate_, this->queueLength_, this->action_, this->logPrefix_);

    LogDebug() << "[SetDscpFilter] CallSystem cmd is :" << cmd;

    if (!CallSystem(cmd)) {
        LogError() << "SetInputDscpFilter: dscp filter set failed!";
        return;
    }
}

/// @brief Set rule filtering
/// @param dfFlag Fragmentation flag
/// @param mfFlag More fragments flag
void Ipv4Engine::SetDFOrMFFilter(bool dfFlag, bool mfFlag) const noexcept
{
    ara::core::String cmd{};
    // User did not configure.
    if (!dfFlag && !mfFlag) {
        LogInfo() << "SetInputDFOrMFFilter:  doNotFragments/MoreFragments  is not "
                     "configuration.";
        return;
    }
    if (dfFlag && mfFlag) {
        LogError() << "SetInputDFOrMFFilter:  doNotFragments/MoreFragments all "
                      "true.please check your config.";
        return;
    }
    // DFFlag and MFFlag cannot be true at the same time.
    if (dfFlag) {
        cmd = GetkAddRuleForNftCmdTable() + (this->inOutFlag_ == 0 ? GetInputChain() : GetOutPutChain())
              + GetkAddRuleForDF();
        // Set rate and log command
        SetLogAndRata(cmd, this->packetRate_, this->queueLength_, this->action_, this->logPrefix_);
    }

    if (mfFlag) {
        cmd = GetkAddRuleForNftCmdTable() + (this->inOutFlag_ == 0 ? GetInputChain() : GetOutPutChain())
              + GetkAddRuleForMF();

        // Set rate and log command
        SetLogAndRata(cmd, this->packetRate_, this->queueLength_, this->action_, this->logPrefix_);
    }

    LogDebug() << "[SetDFOrMFFilter] CallSystem cmd is :" << cmd;

    if (!CallSystem(cmd)) {
        LogError() << "SetInputDFOrMFFilter: doNotFragment/MoreFragments filter "
                      "set failed!";
        return;
    }
}

/// @brief Set ECN rule filtering
/// @param ecn User set ECN value
void Ipv4Engine::SetECNFilter(int32_t ecn) const noexcept
{
    ara::core::String cmd{};
    ara::core::String strEcn{std::to_string(ecn)};

    // User did not configure.
    if (ecn < 0) {
        LogDebug() << "SetInputECNFilter:  ecn  is not configuration.";
        return;
    }

    cmd = GetkAddRuleForNftCmdTable() + (this->inOutFlag_ == 0 ? GetInputChain() : GetOutPutChain())
          + GetkAddRuleForEcn() + strEcn;

    // Set rate and log command
    SetLogAndRata(cmd, this->packetRate_, this->queueLength_, this->action_, this->logPrefix_);

    LogDebug() << "[SetECNFilter] CallSystem cmd is :" << cmd;

    if (!CallSystem(cmd)) {
        LogError() << "SetInputECNFilter: ecn filter set failed!";
        return;
    }
}

/// @brief Set minimum IP header filtering
/// @param hdrLength User set ipv4 header minimum value.
void Ipv4Engine::SetHdrLengthFilter(int32_t hdrLength) const noexcept
{
    ara::core::String cmd{};
    ara::core::String strLength{std::to_string(hdrLength)};
    // User did not configure.
    if (hdrLength < 0) {
        LogDebug() << "SetInputHdrLengthFilter:  ecn  is not configuration.";
        return;
    }

    cmd = GetkAddRuleForNftCmdTable() + (this->inOutFlag_ == 0 ? GetInputChain() : GetOutPutChain())
          + GetkAddRuleForHdrLength() + strLength;

    // Set rate and log command
    SetLogAndRata(cmd, this->packetRate_, this->queueLength_, this->action_, this->logPrefix_);

    LogDebug() << "[SetHdrLengthFilter] CallSystem cmd is :" << cmd;

    if (!CallSystem(cmd)) {
        LogError() << "SetInputHdrLengthFilter: hdrlength filter set failed!";
        return;
    }
}

/// @brief Get the corresponding IP range of the firewall through IP and subnet
/// mask. (ipv4)
/// @param ip
/// @param ipMask
/// @return Get the corresponding ip range.
ara::core::String Ipv4Engine::_getIpRange(ara::core::String const &ip, ara::core::String const &ipMask) const noexcept
{
    struct in_addr ipAddr
    {
    };
    struct in_addr maskAddr
    {
    };
    struct in_addr addrTranslate
    {
    };
    uint32_t ipBin{0};
    uint32_t maskBin{0};

    char networkStr[INET_ADDRSTRLEN]   = {0};
    char broadcastStr[INET_ADDRSTRLEN] = {0};

    // return value.
    ara::core::String strIpRange{};
    ara::core::String strNetworkAddress{};
    ara::core::String strBroadcastAddress{};

    in_addr firstAddr{};
    in_addr lastAddr{};
    char firstStr[INET_ADDRSTRLEN] = {0};
    char lastStr[INET_ADDRSTRLEN]  = {0};

    // Initialize structure
    std::memset(&ipAddr, 0, sizeof(ipAddr));
    std::memset(&maskAddr, 0, sizeof(maskAddr));
    std::memset(&firstAddr, 0, sizeof(firstAddr));
    std::memset(&lastAddr, 0, sizeof(lastAddr));

    // Convert IP address
    if (inet_pton(AF_INET, ip.c_str(), &ipAddr) <= 0) {
        LogError() << "_getIpRange:inet_pton ip   failed!";
        return strIpRange;
    }

    // Convert subnet mask
    if (inet_pton(AF_INET, ipMask.c_str(), &maskAddr) <= 0) {
        LogError() << "_getIpRange:inet_pton ipMask   failed!";
        return strIpRange;
    }

    // Convert to host byte order 32-bit integer
    ipBin   = ntohl(ipAddr.s_addr);
    maskBin = ntohl(maskAddr.s_addr);

    // Verify subnet mask validity
    if (!Is_Valid_Mask(maskBin)) {
        LogError() << "_getIpRange:Is_Valid_Mask  failed!";
        return strIpRange;
    }

    // Calculate network address and broadcast address
    uint32_t networkAddr   = ipBin & maskBin;
    uint32_t broadcastAddr = networkAddr | (~maskBin);

    // Convert network address
    addrTranslate.s_addr = htonl(networkAddr);
    inet_ntop(AF_INET, &addrTranslate, networkStr, INET_ADDRSTRLEN);

    // Convert broadcast address
    addrTranslate.s_addr = htonl(broadcastAddr);
    inet_ntop(AF_INET, &addrTranslate, broadcastStr, INET_ADDRSTRLEN);

    LogInfo() << "_getIpRange: network Address: " << networkStr;
    LogInfo() << "_getIpRange: broadcast Address: " << broadcastStr;

    // Handle special subnets
    // /32 subnet
    if (maskBin == kIpMask32Value) {
        LogError() << "_getIpRange:get ipmask is 255.255.255.255 no host used!!";
        return strIpRange;
    }
    // /31 subnet
    if (maskBin == kIpMask31Value) {
        strIpRange.append(networkStr);
        strIpRange.append("-");
        strIpRange.append(broadcastStr);
        LogInfo() << "_getIpRange:set strIpRange is " << strIpRange;
        return strIpRange;
    }

    // Calculate usable range
    firstAddr.s_addr = htonl(networkAddr + 1);
    lastAddr.s_addr  = htonl(broadcastAddr - 1);

    inet_ntop(AF_INET, &firstAddr, firstStr, INET_ADDRSTRLEN);
    inet_ntop(AF_INET, &lastAddr, lastStr, INET_ADDRSTRLEN);

    strIpRange.append(firstStr);
    strIpRange.append("-");
    strIpRange.append(lastStr);
    LogInfo() << "_getIpRange:set strIpRange is " << strIpRange;
    return strIpRange;
}

}  // namespace internal
}  // namespace fw
}  // namespace ara