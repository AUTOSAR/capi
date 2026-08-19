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
/// @file       ids_proto_common.cpp
/// @brief      Byte order conversion related functions
/// @details
/// @date       2023-01-13
/// @author     niuliming
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/idsm/IDS protocol
/// @interface_level=unit
/// @trace_id_sr=SR_IDSM_0011
/// @unit_description=
/// @endcode
///
/// ================================================================

#include "ids_proto_common.h"

#include <algorithm>

namespace ara {
namespace idsm {
/// @brief Used to test whether the host is big-endian byte order
/// @return Returns true for big-endian byte order, otherwise returns false
/// @throws Stack overflow exception
bool IsBigEndian() noexcept
{
    union
    {
        uint32_t number{0U};
        uint8_t c;
    } test;
    uint32_t const testData{0x12345678U};
    test.number = testData;  // NOLINT
    uint8_t const exceptData{0x12U};
    return test.c == exceptData;  // NOLINT
}
/// @brief Convert 64-bit data from host byte order to network byte order
/// @param hostll Host byte order data
/// @return Network byte order data
/// @throws Stack overflow exception
uint64_t Htonll(uint64_t const hostll)
{
    if (IsBigEndian()) {
        return hostll;
    }
    return ByteOrderSwith(hostll);
}
/// @brief Convert 64-bit data from network byte order to host byte order
/// @param netll Network byte order data
/// @return Host byte order data
/// @throws Stack overflow exception
uint64_t Ntohll(uint64_t const netll)
{
    if (IsBigEndian()) {
        return netll;
    }
    return ByteOrderSwith(netll);
}
/// @brief Reverse byte order of 64-bit data
/// @param hostll Data to be reversed
/// @return 64-bit data after reversal
/// @throws Stack overflow exception
uint64_t ByteOrderSwith(uint64_t hostll)
{
    uint8_t *const ptr{static_cast< uint8_t * >(static_cast< void * >(&hostll))};
    std::reverse(ptr, ptr + sizeof(hostll));
    return hostll;
}
}  // namespace idsm
}  // namespace ara