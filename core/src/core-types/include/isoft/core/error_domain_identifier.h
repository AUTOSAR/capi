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
/// @file       error_domain_identifier.h
/// @brief
/// @details
/// @date       2022-06-24
/// @author     hanzhibo
/// @version    1.2.0
///
/// ================================================================

#ifndef ISOFT_CORE_ERROR_DOMAIN_IDENTIFIER_H
#define ISOFT_CORE_ERROR_DOMAIN_IDENTIFIER_H

#include <cstdint>

namespace isoft {
namespace core {
///
/// @brief Get ISoft error domain ID prefix
///
/// [SWS_CORE_00016]fDRAFTg Vendor-defined error domain range dError domain
/// identifiers where the top 32 bits (i.e. bit #63..#32) are equal to 0xc000’0000 are reserved for vendor-specific
/// error domains. Bits #31..#16 hold the vendor’s numerical identifier, and bits #15..#0 can be used by each vendor for
/// error domain identifiers.c (RS_AP_00130)
///
constexpr std::uint64_t GetErrDomIDPrefix()
{
    constexpr std::uint16_t kNUM_16{16};
    constexpr std::uint16_t kNUM_32{32};
    constexpr std::uint64_t kVendorReservedID = 0XC0000000;
    constexpr std::uint64_t kISoftReservedID  = 0X0000;
    return (kVendorReservedID << kNUM_32) | (kISoftReservedID << kNUM_16);
}
} /* namespace core */
} /* namespace isoft */

#endif /* ISOFT_CORE_ERROR_DOMAIN_IDENTIFIER_H */
