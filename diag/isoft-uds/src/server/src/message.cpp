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
/// @file       message.cpp
/// @brief      This file implements message functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/isoft/uds
/// @trace_id_sr=ff80e971-507e-4752-89b5-2fe03e9f256d
/// @unit_name=message
/// @unit_description=This file provides message functionality
/// @interface_level=module
/// @endcode
///
/// ================================================================

#include "isoft/uds/message.h"

#include "common/serialize.h"
namespace isoft {
namespace uds {
namespace server {
Message::Message(Address sa, Address ta) : sa_{sa}, ta_{ta}, taType_{TargetAddressType::kPhysical}, body_{} {}

Address Message::GetSA() const noexcept { return sa_; }

Address Message::GetTA() const noexcept { return ta_; }

void Message::SetTaType(TargetAddressType const taType) noexcept { taType_ = taType; }

TargetAddressType Message::GetTaType() noexcept { return taType_; }

std::vector< std::uint8_t >& Message::GetBody() noexcept { return body_; }

std::vector< std::uint8_t > Message::Dump() noexcept
{
    std::vector< std::uint8_t > rawData;
    Serialize serialize{rawData};
    serialize.Write(sa_);
    serialize.Write(ta_);
    serialize.Write(body_);
    return rawData;
}
}  // namespace server
}  // namespace uds
}  // namespace isoft