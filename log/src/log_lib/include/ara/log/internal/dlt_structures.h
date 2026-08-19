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
/// @file       dlt_structures.h
/// @brief      DLT protocol structures for internal encoding module
/// @details
/// @date       2025-09-03
/// @author     yangjinbiao
/// @version    1.2.0
///
/// ================================================================

#ifndef __LOG_INTERNAL_DLT_STRUCTURES__
#define __LOG_INTERNAL_DLT_STRUCTURES__

#include <cstdint>
#include <cstring>

#include "dlt_constants.h"

namespace ara {
namespace log {
namespace internal {

// ==================== DltStorageHeader ====================
/// @brief The structure of the DLT file storage header
struct DltStorageHeader final
{
    std::uint8_t pattern[kDlt_Id_Size]{'D', 'L', 'T', 1U};
    std::uint32_t seconds{0};
    std::uint32_t microseconds{0};
    std::uint8_t ecu[kDlt_Id_Size]{'\0'};
};

// ==================== DltStandardHeader ====================
/// @brief Standard header structure
struct DltStandardHeader
{
    std::uint8_t htyp;
    std::uint8_t mcnt;
    std::uint16_t len;
};

// ==================== DltStandardHeaderExtra ====================
/// @brief Extra header parameters (ECU ID, Session ID, Timestamp)
struct DltStandardHeaderExtra
{
    std::uint8_t ecu[kDlt_Id_Size]{0U};
    std::uint32_t seid{0U};
    std::uint32_t tmsp{0U};
};

// ==================== DltExtendedHeader ====================
/// @brief Extended header structure
struct DltExtendedHeader
{
    std::uint8_t msin{};
    std::uint8_t noar{};
    std::uint8_t apid[kDlt_Id_Size]{0};
    std::uint8_t ctid[kDlt_Id_Size]{0};
};

// ==================== DltMessage ====================
/// @brief Complete DLT message structure
struct DltMessage
{
    std::int8_t foundSerialheader{0};
    std::uint32_t resyncOffset{0U};
    std::uint32_t headersize{0U};
    std::uint32_t datasize{0U};
    std::uint8_t headerbuffer[sizeof(DltStandardHeader) + sizeof(DltStandardHeaderExtra) + sizeof(DltExtendedHeader)]{
        0U};
    std::uint8_t* databuffer{nullptr};
    std::uint32_t databuffersize{0U};
    DltStandardHeader* standardheader{nullptr};
    DltStandardHeaderExtra headerExtra{};
    DltExtendedHeader* extendedheader{nullptr};
};

}  // namespace internal
}  // namespace log
}  // namespace ara

#endif  // __LOG_INTERNAL_DLT_STRUCTURES__
