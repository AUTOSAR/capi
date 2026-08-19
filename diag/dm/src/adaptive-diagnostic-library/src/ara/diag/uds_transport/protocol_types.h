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
/// @file       protocol_types.h
/// @brief      This file provides definitions of uds_transport basic types.
/// @details
/// @date       2021-11-23
/// @author     jiawei
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_DIAG_UDS_TRANSPORT_PROTOCOL_TYPES_H_
#define ARA_DIAG_UDS_TRANSPORT_PROTOCOL_TYPES_H_

#include <ara/core/span.h>
#include <ara/core/string.h>

#include <cstdint>

namespace ara {
namespace diag {
namespace uds_transport {

/// @brief UdsTransportProtocolHandler are flexible "plugins", which need an identification.
///
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
///
/// @traceid{SWS_DM_00336}@tracestatus{draft}
using UdsTransportProtocolHandlerID = uint8_t;

/// @brief Channel identifier
///
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
///
/// @traceid{SWS_DM_00337}@tracestatus{draft}
using ChannelID = uint32_t;

/// @brief Priority
///
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
///
/// @traceid{SWS_DM_00451}@tracestatus{draft}
using Priority = uint8_t;

/// @brief Protocol type
///
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
///
/// @traceid{SWS_DM_00452}@tracestatus{draft}
using ProtocolKind = ara::core::String;

/// @brief This is the type of ByteVector.
///
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
///
/// @traceid{SWS_DM_00338}@tracestatus{draft}
using ByteVector = ara::core::Span< uint8_t >;

}  // namespace uds_transport
}  // namespace diag
}  // namespace ara

#endif  // ARA_DIAG_UDS_TRANSPORT_PROTOCOL_TYPES_H_