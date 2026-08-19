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
/// @file       delete.cpp
/// @brief      FW engine table and chain destruction
/// @details    FW engine table and chain destruction
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
/// @unit_name=DeleteTable
/// @unit_description=Firewall engine abstraction layer deletes nft table.
/// @endcode
///
/// ================================================================
///
///
/// ================================================================

#include "ara/fw/filterengine/delete.h"

#include <ara/core/string.h>

#include <cstdlib>
#include <cstring>

#include "ara/fw/filterengine/engine_common.h"
#include "ara/fw/internal/cmd_str.h"
#include "ara/fw/internal/log_api.h"

namespace ara {
namespace fw {
namespace internal {

/// @brief Delete table.
/// @param InOutFlag  In:0 Out:1
/// @return Success/Failure
bool FilterTableDelete() noexcept
{
    ara::core::String cmd{GetkDelTableForNftCmd()};
#if 0
    ara::core::String netDevCmd{GetkDelTableForNetDevCmd()};
#endif
    if (!CallSystem(cmd)) {
        LogError() << "FilterTableDelete: delete  nftables  table[apfilter] failed!";
        return false;
    }
#if 0
    if (!CallSystem(netDevCmd)) {
        LogError() << "FilterTableDelete: delete  nftables  table[ap_netdev_filter] failed!";
        return false;
    }
#endif
    return true;
}

/// @brief Clear chain rules.
/// @param InOutFlag  In:0 Out:1
/// @return
bool FilterRulesDelete(int32_t const &inOutFlag) noexcept
{
    ara::core::String cmd{};

    cmd = GetkDelChainRulesForNftCmd() + (inOutFlag == 0 ? GetInputChain() : GetOutPutChain());

    if (!CallSystem(cmd)) {
        LogError() << "FilterRulesDelete: delete  nftables  table[apfilter] "
                      "chain[apinput/outputchain]  rules failed!";
        return false;
    }
#if 0
    ara::core::String cmdNetDev{};

    cmdNetDev = GetkDelChainRulesForNetDevCmd() + (inOutFlag == 0 ? GetNetDevInputChain() : GetNetDevOutputChain());

    if (!CallSystem(cmdNetDev)) {
        LogError() << "FilterRulesDelete: delete  nftables  table[netdev] chain[ingress/gress]  rules failed!";
        return false;
    }
#endif
    return true;
}

/// @brief Delete corresponding chain.
/// @return Process success/failure
bool FilterChainDelete(int32_t const &inOutFlag) noexcept
{
    ara::core::String cmd{};
    // in  0  out 1
    cmd = GetkDelChainForNftCmd() + (inOutFlag == 0 ? GetInputChain() : GetOutPutChain());

    if (!CallSystem(cmd)) {
        LogError() << "FilterChainDelete: delete  nftables  table[apfilter] "
                      "chain[apinput/outputchain]  failed!";
        return false;
    }
    return true;
}

}  // namespace internal
}  // namespace fw
}  // namespace ara
