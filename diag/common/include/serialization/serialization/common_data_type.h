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
/// @file       common_data_type.h
/// @brief      Autosar-AP communication module constant data type definitions
/// @details    Communication module constant data type definitions
/// @date       2025-04-27
/// @author     Han Yuxin
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/com/com
/// @interface_level=software
/// @trace_id_sr=SR_COM_
/// @endcode
///
/// ================================================================

#ifndef COMMON_DATA_TYPE_H_
#define COMMON_DATA_TYPE_H_

#include <cstdint>

namespace isoft {
namespace serialize {
using char8_t = char;  // NOLINT
//********************************/
constexpr uint8_t kInt8_16U{16U};
constexpr uint8_t kInt8_0x0BU{0x0BU};
constexpr uint8_t kInt8_0x3FU{0x3FU};
constexpr uint8_t kInt8_0xEFU{0xEFU};
constexpr uint8_t kInt8_0xBBU{0xBBU};
constexpr uint8_t kInt8_0xBFU{0xBFU};
constexpr uint8_t kInt8_0xC0U{0xC0U};
constexpr uint8_t kInt8_0x22U{0x22U};
constexpr uint8_t kInt8_0x2EU{0x2EU};
//********************************/
constexpr uint16_t kInt16_8U{8U};
constexpr uint16_t kInt16_16U{8U};
constexpr uint16_t kInt16_0x80U{0x80U};
constexpr uint16_t kInt16_0x81U{0x81U};
constexpr uint16_t kInt16_0xFFU{0xFFU};
constexpr uint16_t kInt16_0x0FFFU{0x0FFFU};
constexpr uint16_t kInt16_0xFEFFU{0xFEFFU};
constexpr uint16_t kInt16_0xFFFFU{0xFFFFU};
constexpr uint16_t kInt16_0x0E00U{0x0E00U};
constexpr uint16_t kInt16_0x0E7FU{0x0E7FU};
constexpr uint16_t kInt16_0x0E80U{0x0E80U};
constexpr uint16_t kInt16_0x0EFFU{0x0EFFU};
constexpr uint16_t kInt16_0x0F00U{0x0F00U};
constexpr uint16_t kInt16_0x0F7FU{0x0F7FU};
//********************************/
constexpr int32_t kInt32_5{5};
constexpr int32_t kInt32_7{7};
constexpr int32_t kInt32_8{8};
constexpr int32_t kInt32_10{10};
constexpr int32_t kInt32_12{12};
constexpr int32_t kInt32_13{13};
constexpr int32_t kInt32_16{16};
constexpr int32_t kInt32_32{32};
constexpr int32_t kInt32_48{48};
constexpr int32_t kInt32_512{512};
constexpr int32_t kInt32_1000{1000};
constexpr int32_t kInt32_0x10000{0x10000};
constexpr int32_t kInt32_0x20000{0x20000};
//********************************/
constexpr int32_t kInt32_0x10{0x10};
//********************************/
constexpr uint32_t kInt32_8U{8U};
constexpr uint32_t kInt32_10U{10U};
constexpr uint32_t kInt32_12U{12U};
constexpr uint32_t kInt32_13U{13U};
constexpr uint32_t kInt32_14U{14U};
constexpr uint32_t kInt32_15U{15U};
constexpr uint32_t kInt32_16U{16U};
constexpr uint32_t kInt32_24U{24U};
constexpr uint32_t kInt32_29U{29U};
constexpr uint32_t kInt32_32U{32U};
constexpr uint32_t kInt32_56U{56U};
constexpr uint32_t kInt32_64U{64U};
constexpr uint32_t kInt32_255U{255U};
constexpr uint32_t kInt32_256U{256U};
constexpr uint32_t kInt32_0755U{0755U};
constexpr uint32_t kInt32_0x3FU{0x3FU};
constexpr uint32_t kInt32_0x80U{0x80U};
constexpr uint32_t kInt32_500U{500U};
constexpr uint32_t kInt32_1000U{1000U};
constexpr uint32_t kInt32_1024U{1024U};
constexpr uint32_t kInt32_5000U{5000U};
constexpr uint32_t kInt32_65535U{65535U};
constexpr uint32_t kInt32_65536U{65536U};
//****************/
constexpr uint32_t kInt32_0x0007U{0x0007U};
constexpr uint32_t kInt32_0x8000U{0x8000U};
constexpr uint32_t kInt32_0xFFU{0xFFU};
constexpr uint32_t kInt32_0xFFFFU{0xFFFFU};
constexpr uint32_t kInt32_0xFFFFFFFU{0xFFFFFFFU};
constexpr uint32_t kInt32_0x00000100U{0x00000100U};
constexpr uint32_t kInt32_0xFFFF7FFFU{0xFFFF7FFFU};
constexpr uint32_t kInt32_0xFFFFFFFFU{0xFFFFFFFFU};
//********************************/
constexpr int64_t kInt64_50{50};
//********************************/
constexpr double kDouble_0P01{0.01};
constexpr double kDouble_3P0{3.0};
constexpr double kDouble_5P0{5.0};
constexpr double kDouble_10P0{10.0};
//********************************/
}  // namespace serialize
}  // namespace isoft
#endif
