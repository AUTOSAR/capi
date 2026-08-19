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
/// @file       create.cpp
/// @brief      FW engine table and chain creation
/// @details    FW engine table and chain creation
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
/// @unit_name=CreateTable
/// @unit_description=Firewall engine abstraction layer creates nft table.
/// @endcode
///
/// ================================================================
///
///
/// ================================================================

#include "ara/fw/filterengine/create.h"

#include <sys/wait.h>
#include <unistd.h>

#include <cstdio>
#include <cstdlib>

#include "ara/fw/filterengine/engine_common.h"
#include "ara/fw/internal/cmd_str.h"
#include "ara/fw/internal/log_api.h"
namespace ara {
namespace fw {
namespace internal {

/// @brief Create firewall filter table (family = inet)
/// @return success/failed
bool FilterTableCreate() noexcept
{
    ara::core::String cmd{GetkAddTableForNftCmd()};
    if (!CallSystem(cmd)) {
        LogError() << "FilterTableCreate: create nftables   table[apfilter]  failed!";
        return false;
    }
#if 0
 // Create netdev table
    cmd = GetkAddTableForNetDevCmd();
    if (!CallSystem(cmd)) {
        LogError() << "FilterTableCreate: create nftables   table[ap_netdev_filter]  failed!";
        return false;
    }
#endif
    LogInfo() << "FilterTableCreate: create nftables   table[apfilter] successfully!";
    return true;
}

/// @brief Create ingress firewall filter chain (INPUT)
/// @param defaultAction Default action
/// @param inOutFlag input output flag.
/// @return success/failed
bool InputFilterChainCreate(int32_t const &defaultAction, int32_t const &inOutFlag) noexcept
{
    ara::core::String cmd{};
#if 0
    ara::core::String cmdNetDev{};

 // Network interface name is empty, do not create netdev table.
    if (netCard.empty()) {
        LogWarn() << "netCard is empty,so we don't create netdev Table.";
    }
#endif
    // 0: block |1:accept
    if (0 == defaultAction) {
        // 0:input |1:output
        cmd = GetkAddChainForNftCmdTable() + (inOutFlag == 0 ? GetInputChain() : GetOutPutChain())
              + (inOutFlag == 0 ? GetkAddInputChainForDrop() : GetkAddOutputChainForDrop());
#if 0
        if (!netCard.empty()) {
            cmdNetDev
                // = GetkAddChainForNetDevCmdTable() + (inOutFlag == 0 ? GetNetDevInputChain() : GetNetDevOutputChain())
                = GetkAddChainForNetDevCmdTable()
                  + GetNetDevInputChain()
                  //+ (inOutFlag == 0 ? GetkAddInputNetDdvChainForInitFirst() : GetkAddOutputNetDdvChainForInitFirst())
                  + GetkAddInputNetDdvChainForInitFirst() + netCard + GetkAddInputNetDdvChainForInitSecondDrop();
            LogInfo() << "InputFilterChainCreate:Create netdev_filter table cmd: " << cmdNetDev.c_str();
            if (!CallSystem(cmdNetDev)) {
                LogError() << "InputFilterChainCreate: create nftables  table[netdev_filter] chain[ingress]  "
                              "chain failed!";
                return false;
            }
        }
#endif

    } else {
        // 0:input |1:output
        cmd = GetkAddChainForNftCmdTable() + (inOutFlag == 0 ? GetInputChain() : GetOutPutChain())
              + (inOutFlag == 0 ? GetkAddInputChainForAccept() : GetkAddOutputChainForAccept());
#if 0
        if (!netCard.empty()) {
            cmdNetDev
                //= GetkAddChainForNetDevCmdTable() + (inOutFlag == 0 ? GetNetDevInputChain() : GetNetDevOutputChain())
                = GetkAddChainForNetDevCmdTable()
                  + GetNetDevInputChain()
                  //+ (inOutFlag == 0 ? GetkAddInputNetDdvChainForInitFirst() : GetkAddOutputNetDdvChainForInitFirst())
                  + GetkAddInputNetDdvChainForInitFirst() + netCard + GetkAddInputNetDdvChainForInitSecondAccept();
            LogInfo() << "InputFilterChainCreate:Create netdev_filter table cmd: " << cmdNetDev.c_str();
            if (!CallSystem(cmdNetDev)) {
                LogError() << "InputFilterChainCreate: create nftables  table[netdev_filter] chain[ingress]  "
                              "chain failed!";
                return false;
            }
        }
#endif
    }
    LogInfo() << "InputFilterChainCreate:Create filter table cmd: " << cmd.c_str();
    if (!CallSystem(cmd)) {
        LogError() << "InputFilterChainCreate: create nftables  table[apfilter] "
                      "chain[apinputchain/apoutputchain] "
                      "chain failed!";
        return false;
    }

    // netdev

    return true;
}
}  // namespace internal
}  // namespace fw
}  // namespace ara