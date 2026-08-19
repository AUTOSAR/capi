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
/// @file       engine_datalink.cpp
/// @brief      datalink Rule
/// @details    datalink Rule input
/// @date       2025-06-25
/// @author     jzy
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/fw/Firewall Engine Abstraction Layer
/// @interface_level=module
/// @trace_id_sr=SR_FW_0010
/// @unit_name=Engine_Datalink
/// @unit_description=Firewall engine abstraction layer common part
/// @endcode
///
/// ================================================================
///
///
/// ================================================================

#include "ara/fw/filterengine/engine_datalink.h"

#include <arpa/inet.h>
#include <netinet/in.h>

#include <cctype>
#include <string>

#include "ara/fw/common/common.h"
#include "ara/fw/filterengine/engine_common.h"
#include "ara/fw/internal/cmd_str.h"
#include "ara/fw/internal/log_api.h"

namespace ara {
namespace fw {
namespace internal {

DataLinkEngine::DataLinkEngine(int32_t action, int32_t inOutFlag, int32_t pRate, int32_t qLength) noexcept
    : action_(action), inOutFlag_(inOutFlag), packetRate_(pRate), queueLength_(qLength)
{
}

/// @brief Set source MAC address and destination MAC address rule filtering
/// @param srcMacAddr Source MAC address
/// @param srcMacAddrMask Source address MASK
/// @param destMacAddr Destination MAC address
/// @param destMacAddrMask Destination address MASK
void DataLinkEngine::SetMacAddressFilter(ara::core::String const &srcMacAddr,
                                         ara::core::String const &srcMacAddrMask,
                                         ara::core::String const &destMacAddr,
                                         ara::core::String const &destMacAddrMask) noexcept
{
    ara::core::String cmd;
    // User did not configure source address and destination address verification.
    if (srcMacAddr.empty() && destMacAddr.empty()) {
        LogDebug() << "SetMacAddressFilter:  srcMacAddr  and  destMacAddr is empty!";
        return;
    }
    // If both source and destination addresses are configured for filtering, the
    // destination address is configured as the local machine's MAC, and the
    // source address is the external address MAC. If the subnet mask of srcAddr
    // and destAddr is not empty, the MAC range filtering needs to be specified.
    if (!srcMacAddr.empty() && !destMacAddr.empty() && !srcMacAddrMask.empty() && !destMacAddrMask.empty()) {
        ara::core::String strSrcMacRange{_getMacRange(srcMacAddr, srcMacAddrMask)};
        ara::core::String strDestMacRange{_getMacRange(destMacAddr, destMacAddrMask)};
        if (strSrcMacRange.empty() || strDestMacRange.empty()) {
            LogError() << "SetMacAddressFilter: get saddr/daddr range failed! ";
            return;
        }

        // action 0:block | 1:allow
        cmd = GetkAddRuleForNftCmdTable() + (this->inOutFlag_ == 0 ? GetInputChain() : GetOutPutChain())
              + GetkAddRuleForDataLinkSaddrMac() + strSrcMacRange + GetkAddRuleForDataLinkDaddrMac() + strDestMacRange;

        // Set rate and log command
        SetLogAndRata(cmd, this->packetRate_, this->queueLength_, this->action_, this->logPrefix_);

        LogDebug() << "[SetMacAddressFilter] CallSystem cmd is :" << cmd;
        if (!CallSystem(cmd)) {
            LogError() << "SetDatalinkAddressFilter: saddr and daddr filter set failed!";
            return;
        }
    }
    // If both source and destination addresses are configured for filtering, the
    // destination address is configured as the local machine's MAC, and the
    // source address is the external address MAC.
    if (!srcMacAddr.empty() && !destMacAddr.empty() && srcMacAddrMask.empty() && destMacAddrMask.empty()) {
        // action 0:block | 1:allow
        cmd = GetkAddRuleForNftCmdTable() + (this->inOutFlag_ == 0 ? GetInputChain() : GetOutPutChain())
              + GetkAddRuleForDataLinkSaddrMac() + srcMacAddr + GetkAddRuleForDataLinkDaddrMac() + destMacAddr;

        // Set rate and log command
        SetLogAndRata(cmd, this->packetRate_, this->queueLength_, this->action_, this->logPrefix_);
        LogDebug() << "[SetMacAddressFilter] CallSystem cmd is :" << cmd;
        if (!CallSystem(cmd)) {
            LogError() << "SetDatalinkAddressFilter: saddr and daddr filter set failed!";
            return;
        }
    }

    // Only configure srcAddr, not srcMask.
    if (!srcMacAddr.empty() && destMacAddr.empty() && srcMacAddrMask.empty()) {
        // action 0:block | 1:allow
        cmd = GetkAddRuleForNftCmdTable() + (this->inOutFlag_ == 0 ? GetInputChain() : GetOutPutChain())
              + GetkAddRuleForDataLinkSaddrMac() + srcMacAddr;

        // Set rate and log command
        SetLogAndRata(cmd, this->packetRate_, this->queueLength_, this->action_, this->logPrefix_);

        LogDebug() << "[SetMacAddressFilter] CallSystem cmd is :" << cmd;
        if (!CallSystem(cmd)) {
            LogError() << "SetInputAddressFilter: saddr  filter set failed!";
            return;
        }
    }
    // Only configure destAddr, not destMask.
    if (srcMacAddr.empty() && !destMacAddr.empty() && destMacAddrMask.empty()) {
        // action 0:block | 1:allow
        cmd = GetkAddRuleForNftCmdTable() + (this->inOutFlag_ == 0 ? GetInputChain() : GetOutPutChain())
              + GetkAddRuleForDataLinkDaddrMac() + destMacAddr;

        // Set rate and log command
        SetLogAndRata(cmd, this->packetRate_, this->queueLength_, this->action_, this->logPrefix_);

        LogDebug() << "[SetMacAddressFilter] CallSystem cmd is :" << cmd;
        if (!CallSystem(cmd)) {
            LogError() << "SetInputAddressFilter: daddr  filter set failed!";
            return;
        }
    }
    // Configure src and srcmask. dest not configured.
    if (!srcMacAddr.empty() && !srcMacAddrMask.empty() && destMacAddr.empty()) {
        ara::core::String strSrcMacRange{_getMacRange(srcMacAddr, srcMacAddrMask)};
        if (strSrcMacRange.empty()) {
            LogError() << "SetMacAddressFilter: get saddr range failed! ";
            return;
        }

        // action 0:block | 1:allow
        cmd = GetkAddRuleForNftCmdTable() + (this->inOutFlag_ == 0 ? GetInputChain() : GetOutPutChain())
              + GetkAddRuleForDataLinkSaddrMac() + strSrcMacRange;

        // Set rate and log command
        SetLogAndRata(cmd, this->packetRate_, this->queueLength_, this->action_, this->logPrefix_);

        LogDebug() << "[SetMacAddressFilter] CallSystem cmd is :" << cmd;
        if (!CallSystem(cmd)) {
            LogError() << "SetMacAddressFilter: saddr  filter set failed!";
            return;
        }
    }
    // Configure dest and destmask. src not configured
    if (!destMacAddr.empty() && !destMacAddrMask.empty() && srcMacAddr.empty()) {
        ara::core::String strDestMacRange{_getMacRange(destMacAddr, destMacAddrMask)};
        if (strDestMacRange.empty()) {
            LogError() << "SetMacAddressFilter: get daddr range failed! ";
            return;
        }

        // action 0:block | 1:allow
        cmd = GetkAddRuleForNftCmdTable() + (this->inOutFlag_ == 0 ? GetInputChain() : GetOutPutChain())
              + GetkAddRuleForDataLinkDaddrMac() + strDestMacRange;

        // Set rate and log command
        SetLogAndRata(cmd, this->packetRate_, this->queueLength_, this->action_, this->logPrefix_);

        LogDebug() << "[SetMacAddressFilter] CallSystem cmd is :" << cmd;
        if (!CallSystem(cmd)) {
            LogError() << "SetInputAddressFilter: daddr  filter set failed!";
            return;
        }
    }
    return;
}

/// @brief Set ether type filtering
/// @param etherType  ether type.
void DataLinkEngine::SetEtherTypeFilter(int32_t etherType) const noexcept
{
    ara::core::String cmd{};
    ara::core::String strEtherType{std::to_string(etherType)};

    // User did not configure.
    if (etherType < 0) {
        LogDebug() << "SetEtherTypeFilter:  etherType  is not configuration.";
        return;
    }

    cmd = GetkAddRuleForNftCmdTable() + (this->inOutFlag_ == 0 ? GetInputChain() : GetOutPutChain())
          + GetkAddRuleForDataLinkEtherType() + strEtherType;

    // Set rate and log command
    SetLogAndRata(cmd, this->packetRate_, this->queueLength_, this->action_, this->logPrefix_);

    LogDebug() << "[SetEtherTypeFilter] CallSystem cmd is :" << cmd;

    if (!CallSystem(cmd)) {
        LogError() << "SetEtherTypeFilter: ether type filter set failed!";
        return;
    }
}

/// @brief Parse MAC address string
/// @param macStr
/// @param macBytes
/// @return
bool DataLinkEngine::_parseMAC(const ara::core::String &macStr, std::vector< uint8_t > &macBytes) noexcept
{
    ara::core::String cleanStr;
    // for (auto const c : macStr) {
    //     if (std::isxdigit(c) != 0) {
    //         cleanStr += static_cast< char8_t >(std::toupper(c));
    //     }
    // }

    for (char c : macStr) {
        if (c != ':') {
            cleanStr.push_back(c);
        }
    }

    if (cleanStr.length() != kDataLinkMacLength) {
        return false;
    }

    macBytes.clear();
    for (size_t i = 0; i < kDataLinkMacLength; i += 2) {
        ara::core::String byteStr = cleanStr.substr(i, 2);
        std::string byteBuff{byteStr.c_str()};
        uint8_t byte = static_cast< uint8_t >(std::stoul(byteBuff, nullptr, kIp6Value16));
        macBytes.push_back(byte);
    }
    return true;
}

/// @brief Determine whether the character is a hexadecimal data character
/// @param c Character
/// @return Yes/No
bool DataLinkEngine::_isHexChar(char8_t c) noexcept
{
    return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f');
}

/// @brief Assemble into MAC address string
/// @param macBytes Store each byte address of mac
/// @return mac string address
ara::core::String DataLinkEngine::_formatMAC(const std::vector< uint8_t > &macBytes) noexcept
{
    std::ostringstream oss;
    for (size_t i = 0; i < macBytes.size(); ++i) {
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast< int >(macBytes[i]);
        if (i < macBytes.size() - 1) {
            oss << ":";
        }
    }
    ara::core::String strFormatMac{oss.str()};
    return strFormatMac;
}

/// @brief Get the corresponding MAC range of the firewall through MAC and
/// subnet mask
/// @param mac MAC address
/// @param macMask MAC address subnet mask
/// @return Get the corresponding MAC range.
ara::core::String DataLinkEngine::_getMacRange(ara::core::String const &mac, ara::core::String const &macMask) noexcept
{
    std::vector< uint8_t > macBytes{};
    std::vector< uint8_t > maskBytes{};
    ara::core::String macRange{};
    ara::core::String macRangeStart{};
    ara::core::String macRangeEnd{};
    // Parse MAC address
    if (!_parseMAC(mac, macBytes) || !_parseMAC(macMask, maskBytes)) {
        LogError() << "_getMacRange: parse MAC or MACMask failed "
                   << " Mac: " << mac.c_str() << " MacMask: " << macMask.c_str();
        return macRange;
    }
    // Calculate start address (network address): MAC & Mask
    std::vector< uint8_t > startAddr;
    for (size_t i = 0; i < kDataLinkMacSegment; ++i) {
        startAddr.push_back(macBytes[i] & maskBytes[i]);
    }

    // Calculate end address: start address | ~Mask
    std::vector< uint8_t > endAddr;
    for (size_t i = 0; i < kDataLinkMacSegment; ++i) {
        endAddr.push_back(startAddr[i] | ~maskBytes[i]);
    }

    macRangeStart = _formatMAC(startAddr);
    macRangeEnd   = _formatMAC(endAddr);
    // If user uses mask as ff:ff:ff:ff:ff:ff, the start and end range are the
    // same, so fw needs to be compatible.
    if (macRangeStart == macRangeEnd) {
        macRange = macRangeStart;
    } else {
        macRange = macRangeStart + "-" + macRangeEnd;
        ;
    }
    LogInfo() << "_getMacRange: return macRange " << macRange.c_str();
    return macRange;
}

}  // namespace internal
}  // namespace fw
}  // namespace ara