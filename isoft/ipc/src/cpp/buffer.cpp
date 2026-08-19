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
/// @file       buffer.cpp
/// @brief
/// @details
/// @date       2022-09-20
/// @author     yangtao
/// @version    1.2.0
///
/// ================================================================

#include "isoft/ipccpp/buffer.h"

#include "isoft/ipc/packet.h"

namespace isoft {
namespace ipc {

uint8_t* IPCPacketBufferReference::GetPtr() { return ipc_buffer_get_ptr(GetParent()); }
size_t IPCPacketBufferReference::GetLen() { return ipc_buffer_get_len(GetParent()); }

int IPCPacketBufferReference::SetLen(const size_t& len) { return ipc_buffer_set_len(GetParent(), len); }

size_t IPCPacketBufferReference::GetSize() { return ipc_buffer_get_size(GetParent()); }

}  // namespace ipc
}  // namespace isoft
