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
/// @file       engine_tcp.cpp
/// @brief      tcp rule filtering
/// @details    tcp rule filtering
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
/// @unit_name=Engine_Tcp
/// @unit_description=Firewall engine abstraction layer tcp data abstraction
/// processing class
/// @endcode
///
/// ================================================================
///
///
/// ================================================================

#include "ara/fw/filterengine/engine_tcp.h"

#include "ara/fw/filterengine/engine_common.h"
#include "ara/fw/internal/cmd_str.h"
#include "ara/fw/internal/log_api.h"

namespace ara {
namespace fw {
namespace internal {

TcpEngine::TcpEngine(int32_t action, int32_t inOutFlag, int32_t pRate, int32_t qLength) noexcept
    : action_(action), inOutFlag_(inOutFlag), packetRate_(pRate), queueLength_(qLength)
{
}

/// @brief Set TCP port range filtering
/// @param sPortMin Source address port minimum value
/// @param sPortMax Source address port maximum value
/// @param pPortMin Destination address port minimum value
/// @param pPortMax Destination address port maximum value
void TcpEngine::SetTcpPortRangeFilter(int32_t sPortMin,
                                      int32_t sPortMax,
                                      int32_t pPortMin,
                                      int32_t pPortMax) const noexcept
{
    ara::core::String cmd{};
    ara::core::String strSrcPortMin{std::to_string(sPortMin)};
    ara::core::String strSrcPortMax{std::to_string(sPortMax)};
    ara::core::String strDestPortMin{std::to_string(pPortMin)};
    ara::core::String strDestPortMax{std::to_string(pPortMax)};

    // User did not configure TCP port range validation.
    if (sPortMin == -1 && sPortMax == -1 && pPortMin == -1 && pPortMax == -1) {
        LogDebug() << "SetTcpPortRangeFilter:  tcp port  is not configuration.";
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
              + GetkTcpForSPortMin() + strSrcPortMin;
        // Set rate and log command
        SetLogAndRata(cmd, this->packetRate_, this->queueLength_, this->action_, this->logPrefix_);
    }
    // User only configured S port maximum value
    if (sPortMin == -1 && sPortMax != -1 && pPortMin == -1 && pPortMax == -1) {
        cmd = GetkAddRuleForNftCmdTable() + (this->inOutFlag_ == 0 ? GetInputChain() : GetOutPutChain())
              + GetkTcpForSPortMax() + strSrcPortMax;
        // Set rate and log command
        SetLogAndRata(cmd, this->packetRate_, this->queueLength_, this->action_, this->logPrefix_);
    }
    // User only configured D port minimum value
    if (sPortMin == -1 && sPortMax == -1 && pPortMin != -1 && pPortMax == -1) {
        cmd = GetkAddRuleForNftCmdTable() + (this->inOutFlag_ == 0 ? GetInputChain() : GetOutPutChain())
              + GetkTcpForDportMin() + strDestPortMin;
        // Set rate and log command
        SetLogAndRata(cmd, this->packetRate_, this->queueLength_, this->action_, this->logPrefix_);
    }
    // User only configured D port maximum value
    if (sPortMin == -1 && sPortMax == -1 && pPortMin == -1 && pPortMax != -1) {
        cmd = GetkAddRuleForNftCmdTable() + (this->inOutFlag_ == 0 ? GetInputChain() : GetOutPutChain())
              + GetkTcpForDportMax() + strDestPortMax;
        // Set rate and log command
        SetLogAndRata(cmd, this->packetRate_, this->queueLength_, this->action_, this->logPrefix_);
    }

    LogDebug() << "[Tcp SetTcpPortRangeFilter] CallSystem cmd is :" << cmd;

    if (!CallSystem(cmd)) {
        LogError() << "Tcp SetTcpPortRangeFilter: hdrlength filter set failed!";
        return;
    }
}

/// @brief TCP max session connection count filtering. (Generally used for Input
/// hook point. If output, it means limiting the number of TCP connections
/// initiated locally.)
/// @param maxSessionConn TCP set maximum connection count.
void TcpEngine::SetTcpMaxSeesionFilter(int32_t maxSessionConn) const noexcept
{
    ara::core::String cmd{};
    ara::core::String strMaxSessionConn{std::to_string(maxSessionConn)};
    // User did not configure.
    if (maxSessionConn < 0) {
        LogDebug() << "SetTcpMaxSeesionFilter:  maxSessionConn  is not configuration.";
        return;
    }

    cmd = GetkAddRuleForNftCmdTable() + (this->inOutFlag_ == 0 ? GetInputChain() : GetOutPutChain())
          + GetkTcpForMaxTcpSession() + strMaxSessionConn;
    // Set rate and log command
    SetLogAndRata(cmd, this->packetRate_, this->queueLength_, this->action_, this->logPrefix_);

    LogDebug() << "[SetTcpMaxSeesionFilter] CallSystem cmd is :" << cmd;

    if (!CallSystem(cmd)) {
        LogError() << "SetTcpMaxSeesionFilter: maxSessionConn filter set failed!";
        return;
    }
}

/// @brief TCP set state transition Flags filtering. If this flag is set, TCP
/// state validation needs to be performed, and the action is not concerned
/// here. And the behavior of this flag being true is different for input hook
/// point and output hook point.
/// @param tcpSmFlags tcp state transition flags
void TcpEngine::SetTcpSMFlagsFilter(bool tcpSmFlags) const noexcept
{
    ara::core::String cmd{};
    ara::core::String accept{" accept "};

    // User did not configure.
    if (!tcpSmFlags) {
        LogDebug() << "SetTcpSMFlagsFilter:  tcpSmFlags  is not configuration.";
        return;
    }

    // Allow TCP to perform state transitions.
    if (tcpSmFlags) {
        // input hook
        if (this->inOutFlag_ == 0) {
            // establish,related accept.
            cmd = GetkAddRuleForNftCmdTable() + GetInputChain() + GetkTcpForSetTcpState();

            LogDebug() << "[SetTcpSMFlagsFilter] CallSystem cmd is :" << cmd;

            if (!CallSystem(cmd)) {
                LogError() << "SetTcpSMFlagsFilter: GetkTcpForSetTcpState filter set failed!";
                return;
            }
            // ct state new . only syn.
            cmd = GetkAddRuleForNftCmdTable() + GetInputChain() + GetkTcpForSetTcpNewState();
            LogDebug() << "[SetTcpSMFlagsFilter] CallSystem cmd is :" << cmd;

            if (!CallSystem(cmd)) {
                LogError() << "SetTcpSMFlagsFilter: GetkTcpForSetTcpNewState filter "
                              "set failed!";
                return;
            }
            // tcp  invaild state drop
            cmd = GetkAddRuleForNftCmdTable() + GetInputChain() + GetkTcpForDropTcpInvaildState();
            LogDebug() << "[SetTcpSMFlagsFilter] CallSystem cmd is :" << cmd;

            if (!CallSystem(cmd)) {
                LogError() << "SetTcpSMFlagsFilter: GetkTcpForDropTcpInvaildState "
                              "filter set failed!";
                return;
            }
        }
        // output
        else {
            // establish,related accept.
            cmd = GetkAddRuleForNftCmdTable() + GetOutPutChain() + GetkTcpForSetTcpState();

            LogDebug() << "[SetTcpSMFlagsFilter] CallSystem cmd is :" << cmd;

            if (!CallSystem(cmd)) {
                LogError() << "SetTcpSMFlagsFilter: GetkTcpForSetTcpState filter set failed!";
                return;
            }
            // tcp  invaild state drop
            cmd = GetkAddRuleForNftCmdTable() + GetOutPutChain() + GetkTcpForDropTcpInvaildState();
            LogDebug() << "[SetTcpSMFlagsFilter] CallSystem cmd is :" << cmd;

            if (!CallSystem(cmd)) {
                LogError() << "SetTcpSMFlagsFilter: GetkTcpForDropTcpInvaildState "
                              "filter set failed!";
                return;
            }
        }
    }
}

/// @brief TCP set TCP session timeout filtering.
/// @param timeout tcp session timeout time. (Currently no usable nft command
/// found. Not implemented yet.)
void TcpEngine::SetTcpTimeOutFilter(int32_t timeout) const noexcept
{
    ara::core::String strTimeOut{std::to_string(timeout)};
    return;
}

}  // namespace internal
}  // namespace fw
}  // namespace ara