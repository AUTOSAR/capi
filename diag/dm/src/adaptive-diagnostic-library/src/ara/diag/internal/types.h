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
/// @file       types.h
/// @brief      This file provides definitions of basic types.
/// @details
/// @date       2022-08-14
/// @author     gaohuiming
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_DIAG_INTERNAL_TYPES_H_
#define ARA_DIAG_INTERNAL_TYPES_H_

#include <ara/core/map.h>
#include <ara/core/string.h>
#include <ara/core/vector.h>

#include <cstdint>

namespace ara {
namespace diag {
namespace internal {
/// @brief Define Chunk
using Chunk = ara::core::Vector< std::uint8_t >;
/// @brief Define MetaInfoMap
using MetaInfoMap = ara::core::Map< ara::core::String, ara::core::String >;

}  // namespace internal
}  // namespace diag
}  // namespace ara

#endif  // ARA_DIAG_INTERNAL_TYPES_H_