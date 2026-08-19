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
/// @file       msg_process.cpp
/// @brief      fw message processing
/// @details    fw message processing
/// @date       2025-03-19
/// @author     jzy
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/FW/serialization
/// @interface_level=module
/// @trace_id_sr=SR_FW_0011
/// @unit_name=Msg_Process
/// @unit_description=fw client and server communication serialization
/// processing class.
/// @endcode
///
/// ================================================================
///
///
/// ================================================================

#include "ara/fw/internal/msg_process.h"

#include <ara/core/string.h>
#include <rapidjson/document.h>
#include <rapidjson/rapidjson.h>

namespace ara {
namespace fw {
namespace internal {

/// @brief
/// @param instanceId
/// @param fwStatus
/// @return
/// @throw
Chunk ClientEventParse::Serialize(ara::core::StringView const &instanceId, uint32_t const fwStatus)
{
    ClientEventMsg const msg{instanceId, fwStatus};
    return msg.Dump();
}

/// @brief
/// @param clientEvMsg
/// @return
/// @throw
Chunk ClientEventParse::Serialize(ClientEventMsg const &clientEvMsg) { return clientEvMsg.Dump(); }
/// @brief
/// @param data
/// @param dataSize
/// @param msg
/// @throw
void ClientEventParse::Deserialize(uint8_t *const data, uint32_t const dataSize, ClientEventMsg *const msg)
{
    std::ignore = msg->Load(Chunk(data, data + dataSize));
    return;
}

}  // namespace internal
}  // namespace fw
}  // namespace ara