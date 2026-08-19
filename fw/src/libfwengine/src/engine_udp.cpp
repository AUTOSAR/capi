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
/// @file       engine_udp.cpp
/// @brief      udp rule filtering
/// @details    udp rule filtering
/// @date       2025-06-09
/// @author     jzy
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/fw/Firewall Engine Abstraction Layer
/// @interface_level=module
/// @trace_id_sr=SR_FW_0010
/// @unit_name=Engine_Udp
/// @unit_description=Firewall engine abstraction layer udp data abstraction
/// processing class
/// @endcode
///
/// ================================================================
///
///
/// ================================================================

#include "ara/fw/filterengine/engine_udp.h"

#include "ara/fw/filterengine/engine_common.h"
#include "ara/fw/internal/cmd_str.h"
#include "ara/fw/internal/log_api.h"

namespace ara {
namespace fw {
namespace internal {

UdpEngine::UdpEngine(int32_t action, int32_t inOutFlag, int32_t pRate, int32_t qLength) noexcept
    : action_(action), inOutFlag_(inOutFlag), packetRate_(pRate), queueLength_(qLength)
{
}
/// @brief Set UDP port range filtering
/// @param sPortMin Source address port minimum value
/// @param sPortMax Source address port maximum value
/// @param pPortMin Destination address port minimum value
/// @param pPortMax Destination address port maximum value
void UdpEngine::SetPortRangeFilter(int32_t sPortMin,
                                   int32_t sPortMax,
                                   int32_t pPortMin,
                                   int32_t pPortMax) const noexcept
{
    ara::core::String cmd{};
    ara::core::String strSrcPortMin{std::to_string(sPortMin)};
    ara::core::String strSrcPortMax{std::to_string(sPortMax)};
    ara::core::String strDestPortMin{std::to_string(pPortMin)};
    ara::core::String strDestPortMax{std::to_string(pPortMax)};

    // User did not configure UDP port range validation.
    if (sPortMin == -1 && sPortMax == -1 && pPortMin == -1 && pPortMax == -1) {
        LogDebug() << "SetPortRangeFilter:  udp port  is not configuration.";
        return;
    }

    // User configured S port range and D port range.
    if (sPortMin != -1 && sPortMax != -1 && pPortMin != -1 && pPortMax != -1) {
        // If min and max are the same value, no need to specify range. Src and Dest
        // min and max are the same values.
        if ((sPortMin == sPortMax) && (pPortMin == pPortMax)) {
            cmd = GetkAddRuleForNftCmdTable() + (this->inOutFlag_ == 0 ? GetInputChain() : GetOutPutChain())
                  + GetkTcpForSport() + strSrcPortMin + GetkTcpForDport() + strDestPortMin;
        } else if ((sPortMin == sPortMax) && (pPortMin != pPortMax)) {
            cmd = GetkAddRuleForNftCmdTable() + (this->inOutFlag_ == 0 ? GetInputChain() : GetOutPutChain())
                  + GetkTcpForSport() + strSrcPortMin + GetkTcpForDport() + strDestPortMin + "-" + strDestPortMax;
        } else if ((sPortMin != sPortMax) && (pPortMin == pPortMax)) {
            cmd = GetkAddRuleForNftCmdTable() + (this->inOutFlag_ == 0 ? GetInputChain() : GetOutPutChain())
                  + GetkTcpForSport() + strSrcPortMin + "-" + strSrcPortMax + GetkTcpForDport() + strDestPortMin;
        } else {
            cmd = GetkAddRuleForNftCmdTable() + (this->inOutFlag_ == 0 ? GetInputChain() : GetOutPutChain())
                  + GetkTcpForSport() + strSrcPortMin + "-" + strSrcPortMax + GetkTcpForDport() + strDestPortMin + "-"
                  + strDestPortMax;
        }
        // Set rate and log command
        SetLogAndRata(cmd, this->packetRate_, this->queueLength_, this->action_, this->logPrefix_);
    }

    // User configured S port range, not D port range.
    if (sPortMin != -1 && sPortMax != -1 && pPortMin == -1 && pPortMax == -1) {
        if (sPortMin == sPortMax) {
            cmd = GetkAddRuleForNftCmdTable() + (this->inOutFlag_ == 0 ? GetInputChain() : GetOutPutChain())
                  + GetkTcpForSport() + strSrcPortMin;
        } else {
            cmd = GetkAddRuleForNftCmdTable() + (this->inOutFlag_ == 0 ? GetInputChain() : GetOutPutChain())
                  + GetkTcpForSport() + strSrcPortMin + "-" + strSrcPortMax;
        }

        // Set rate and log command
        SetLogAndRata(cmd, this->packetRate_, this->queueLength_, this->action_, this->logPrefix_);
    }

    // User configured D port range, not S port range.
    if (sPortMin == -1 && sPortMax == -1 && pPortMin != -1 && pPortMax != -1) {
        if (pPortMin == pPortMax) {
            cmd = GetkAddRuleForNftCmdTable() + (this->inOutFlag_ == 0 ? GetInputChain() : GetOutPutChain())
                  + GetkTcpForDport() + strDestPortMin;
        } else {
            cmd = GetkAddRuleForNftCmdTable() + (this->inOutFlag_ == 0 ? GetInputChain() : GetOutPutChain())
                  + GetkTcpForDport() + strDestPortMin + "-" + strDestPortMax;
        }

        // Set rate and log command
        SetLogAndRata(cmd, this->packetRate_, this->queueLength_, this->action_, this->logPrefix_);
    }

    // User only configured S port minimum value
    if (sPortMin != -1 && sPortMax == -1 && pPortMin == -1 && pPortMax == -1) {
        cmd = GetkAddRuleForNftCmdTable() + (this->inOutFlag_ == 0 ? GetInputChain() : GetOutPutChain())
              + GetkUdpForSPortMin() + strSrcPortMin;
        // Set rate and log command
        SetLogAndRata(cmd, this->packetRate_, this->queueLength_, this->action_, this->logPrefix_);
    }
    // User only configured S port maximum value
    if (sPortMin == -1 && sPortMax != -1 && pPortMin == -1 && pPortMax == -1) {
        cmd = GetkAddRuleForNftCmdTable() + (this->inOutFlag_ == 0 ? GetInputChain() : GetOutPutChain())
              + GetkUdpForSPortMax() + strSrcPortMax;
        // Set rate and log command
        SetLogAndRata(cmd, this->packetRate_, this->queueLength_, this->action_, this->logPrefix_);
    }
    // User only configured D port minimum value
    if (sPortMin == -1 && sPortMax == -1 && pPortMin != -1 && pPortMax == -1) {
        cmd = GetkAddRuleForNftCmdTable() + (this->inOutFlag_ == 0 ? GetInputChain() : GetOutPutChain())
              + GetkUdpForDportMin() + strDestPortMin;
        // Set rate and log command
        SetLogAndRata(cmd, this->packetRate_, this->queueLength_, this->action_, this->logPrefix_);
    }
    // User only configured D port maximum value
    if (sPortMin == -1 && sPortMax == -1 && pPortMin == -1 && pPortMax != -1) {
        cmd = GetkAddRuleForNftCmdTable() + (this->inOutFlag_ == 0 ? GetInputChain() : GetOutPutChain())
              + GetkUdpForDportMax() + strDestPortMax;
        // Set rate and log command
        SetLogAndRata(cmd, this->packetRate_, this->queueLength_, this->action_, this->logPrefix_);
    }

    LogDebug() << "[Udp SetPortRangeFilter] CallSystem cmd is :" << cmd;

    if (!CallSystem(cmd)) {
        LogError() << "Udp SetPortRangeFilter: hdrlength filter set failed!";
        return;
    }
}

/// @brief Set whether to perform udp checksum rule filtering (currently the
/// Linux kernel performs checksum by default, this function does nothing)
/// @param checkSumFlag Whether to perform checksum flag.
void UdpEngine::SetCheckSumFilter(bool checkSumFlag) const noexcept
{
    if (checkSumFlag) {
        return;
    }
}

}  // namespace internal
}  // namespace fw
}  // namespace ara