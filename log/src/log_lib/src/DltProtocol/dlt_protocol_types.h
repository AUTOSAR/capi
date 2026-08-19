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
/// @file       dlt_protocol_types.h
/// @brief      Externally visible basic type definitions for DltProtocol
/// @details
/// @date       2025-09-03
/// @author     yangjinbiao
/// @version    1.2.0
///
/// ================================================================

#ifndef DLT_PROTOCOL_TYPES_H
#define DLT_PROTOCOL_TYPES_H

#include <cstdint>

namespace ara {
namespace log {
namespace internal {

/// @brief Definitions of DLT Format
enum class DltFormatType : std::uint32_t
{
    kDlt_Format_Default = 0x00,
    kDlt_Format_Hex8    = 0x01,
    kDlt_Format_Hex16   = 0x02,
    kDlt_Format_Hex32   = 0x03,
    kDlt_Format_Hex64   = 0x04,
    kDlt_Format_Bin8    = 0x05,
    kDlt_Format_Bin16   = 0x06,
    kDlt_Format_Max     = 0xff
};

}  // namespace internal
}  // namespace log
}  // namespace ara

#endif  // DLT_PROTOCOL_TYPES_H
