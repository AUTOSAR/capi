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
/// @file       hash.h
/// @brief      This file provides the Diagnostic Management class
/// @details
/// @date       2025-12-24
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================

#ifndef _ARA_DIAG_HASH_H_
#define _ARA_DIAG_HASH_H_
#include <cmath>
#include <string>

#include "../log/log.h"
namespace ara {
namespace diag {
inline std::uint16_t Hash16(std::string const& value) noexcept
{
    std::hash< std::string > hasher;
    std::uint32_t hashValue = static_cast< std::uint16_t >(hasher(value) & 0xFFFFU);
    ara::diag::common::LogDebug() << "Hash16| input:" << value << " output:" << hashValue;
    return hashValue;
}
inline std::uint32_t Hash32(std::string const& value) noexcept
{
    std::hash< std::string > hasher;
    std::uint32_t hashValue = static_cast< std::uint32_t >(hasher(value) & 0xFFFFFFFF);
    ara::diag::common::LogDebug() << "Hash32| input:" << value << " output:" << hashValue;
    return hashValue;
}

}  // namespace diag
}  // namespace ara

#endif  // _ARA_DIAG_NSOMEIP_CONFIG_H_