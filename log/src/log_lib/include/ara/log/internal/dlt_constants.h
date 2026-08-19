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
/// @file       dlt_constants.h
/// @brief      DLT protocol constants for internal encoding module
/// @details
/// @date       2025-09-03
/// @author     yangjinbiao
/// @version    1.2.0
///
/// ================================================================

#ifndef __LOG_INTERNAL_DLT_CONSTANTS__
#define __LOG_INTERNAL_DLT_CONSTANTS__

#include <cstdint>

namespace ara {
namespace log {
namespace internal {

// ==================== Buffer Size ====================
static std::uint32_t const kLogBufferSize{2048U};

// ==================== SCOD (String Coding) ====================
static std::uint32_t const kDlt_Scod_Hex{0x00010000U};
static std::uint32_t const kDlt_Scod_Bin{0x00018000U};

// ==================== HTYP (Header Type) ====================
static std::uint8_t const kDlt_Htyp_Ueh{0x01U};   // use extended header
static std::uint8_t const kDlt_Htyp_Weid{0x04U};  // with ECU ID
static std::uint8_t const kDlt_Htyp_Wsid{0x08U};  // with Session ID
static std::uint8_t const kDlt_Htyp_Wtms{0x10U};  // with timestamp
static std::uint8_t const kDlt_Htyp_Msbf{0x02U};  // big endian
static std::uint8_t const kDlt_Htyp_Protocol_Version_N1{1U << 5U};

// ==================== MSIN (Message Info) ====================
static std::uint8_t const kDlt_Msin_Mstp{0x0eU};
static std::uint8_t const kDlt_Msin_Mtin{0xf0U};
static std::uint8_t const kDlt_Msin_Mstp_Shift{1U};
static std::uint8_t const kDlt_Msin_Mtin_Shift{4U};

// ==================== Type Constants ====================
static std::uint8_t const kDlt_Type_Log{0x00U};
static std::uint8_t const kDlt_Type_App_Trace{0x01U};
static std::uint8_t const kDlt_Trace_VFB{0x05U};

// ==================== Size Constants ====================
static std::uint8_t const kDlt_Id_Size{4U};
static std::uint8_t const kDlt_Size_Weid{4U};
static std::uint8_t const kDlt_Size_Wsid{sizeof(std::uint32_t)};
static std::uint8_t const kDlt_Size_Wtms{sizeof(std::uint32_t)};
static std::uint32_t const kDlt_Serial_Header_Size{4U};

// ==================== Type Info ====================
static std::uint32_t const kDlt_Type_Info_Sint{0x00000020U};
static std::uint32_t const kDlt_Type_Info_Uint{0x00000040U};
static std::uint32_t const kDlt_Type_Info_Floa{0x00000080U};
static std::uint32_t const kDlt_Tyle_8_Bit{0x00000001U};
static std::uint32_t const kDlt_Tyle_16_Bit{0x00000002U};
static std::uint32_t const kDlt_Tyle_32_Bit{0x00000003U};
static std::uint32_t const kDlt_Tyle_64_Bit{0x00000004U};

}  // namespace internal
}  // namespace log
}  // namespace ara

#endif  // __LOG_INTERNAL_DLT_CONSTANTS__
