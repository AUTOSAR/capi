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
/// @file       engine_payload.cpp
/// @brief      payload rule filtering
/// @details    payload rule filtering
/// @date       2025-10-15
/// @author     jzy
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/fw/Firewall Engine Abstraction Layer
/// @interface_level=module
/// @trace_id_sr=SR_FW_0010
/// @unit_name=Engine_Payload
/// @unit_description=Firewall engine abstraction layer payload data abstraction
/// processing class
/// @endcode
///
/// ================================================================
///
///
/// ================================================================

#include "ara/fw/filterengine/engine_payload.h"

#include "ara/fw/common/common.h"
#include "ara/fw/filterengine/engine_common.h"
#include "ara/fw/internal/cmd_str.h"
#include "ara/fw/internal/log_api.h"

namespace ara {
namespace fw {
namespace internal {

PayloadEngine::PayloadEngine(int32_t action, int32_t inOutFlag, int32_t pRate, int32_t qLength) noexcept
    : action_(action), inOutFlag_(inOutFlag), packetRate_(pRate), queueLength_(qLength)
{
}
/// @brief Set UDP port range filtering
/// @param offset Offset position
/// @param value Value
void PayloadEngine::SetOffsetAndValueFilter(int32_t offset, int32_t value) const noexcept
{
    // This command is the offset in bits, so need * 8
    int32_t offsetBuf{offset * kPayloadOffset};
    ara::core::String cmd{};
    ara::core::String strOffset{std::to_string(offsetBuf)};
    ara::core::String strValue{std::to_string(value)};
    ara::core::String ruleBuf1{" @ll,"};
    ara::core::String ruleBuf2{",8 == "};
    // User did not configure.
    if (offset <= 0) {
        LogDebug() << "SetOffsetAndValueFilter: offset  is not configuration.";
        return;
    }

    cmd = GetkAddRuleForNftCmdTable() + (this->inOutFlag_ == 0 ? GetInputChain() : GetOutPutChain()) + ruleBuf1
          + strOffset + ruleBuf2 + strValue;

    // Set rate and log command
    SetLogAndRata(cmd, this->packetRate_, this->queueLength_, this->action_, this->logPrefix_);

    LogDebug() << "[SetOffsetAndValueFilter] CallSystem cmd is :" << cmd;

    if (!CallSystem(cmd)) {
        LogError() << "SetOffsetAndValueFilter: offset and value set failed!";
        return;
    }
}

}  // namespace internal
}  // namespace fw
}  // namespace ara