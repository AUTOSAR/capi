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
/// @file       hc_message.cpp
/// @brief
/// @details
/// @date       2024-06-20
/// @author     wangyanlong
/// @version    1.2.0
///
/// ================================================================

#include "ara/phm/internal/com/hc_message.h"

#include "ara/phm/internal/phm_log.h"

namespace ara {
namespace phm {
namespace internal {
namespace hccom {

/// @brief serialization.
/// @return data after serialization.
Chunk MessageSerializer::Serialize() const noexcept
{
    Chunk payload;
    isoft::serialize::Buffer< Chunk > buffer{payload};
    if (isoft::serialize::Serialize(buffer, msg_.instance, msg_.healthStatus, msg_.processId, msg_.timestamp) < 0) {
        std::ignore = buffer;
        LOG_ERROR << "serialize error.";
        payload.clear();
    }
    return payload;
}

/// @brief deserialization.
/// @param chunk msg after deserialization.
/// @return 0 success;other fail.
int32_t MessageSerializer::Deserialize(Chunk& chunk) noexcept
{
    isoft::serialize::Buffer< Chunk > const buffer{chunk};
    if (isoft::serialize::Deserialize(buffer, msg_.instance, msg_.healthStatus, msg_.processId, msg_.timestamp) < 0) {
        LOG_ERROR << "deserialize error, data len " << chunk.size();
        return -1;
    }
    return 0;
}

/// @brief set msg.
/// @param msg msg.
void MessageSerializer::SetMsg(Message const& msg) noexcept { msg_ = msg; }

/// @brief Returns the msg.
/// @return msg.
Message MessageSerializer::GetMsg() const noexcept { return msg_; }

}  // namespace hccom
}  // namespace internal
}  // namespace phm
}  // namespace ara