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
/// @file       secoc_type.h
/// @brief      secoc type header file
/// @details
/// @date       2023-09-04
/// @author     shigang.zan
/// @version    1.2.0
///
/// ================================================================

#ifndef __SECOC_TYPE_H__
#define __SECOC_TYPE_H__

#include "ara/core/vector.h"

#define SECOC_UT_MOCK_CMAC_BY_OPENSSL
#undef SECOC_UT_MOCK_CMAC_BY_OPENSSL

#define SECOC_UT_DUMP_BIT
#undef SECOC_UT_DUMP_BIT

#ifdef SECOC_UT_MOCK_CMAC_BY_OPENSSL
    #include <openssl/cmac.h>
#endif

#ifdef SECOC_UT_DUMP_BIT
    #include <string>
#endif

namespace ara {
namespace com {
namespace secoc {
//********************************/
uint8_t const kCuc_0{0x00};
uint8_t const kCuc_1{0x01};
uint8_t const kCuc_2{0x02};
uint8_t const kCuc_3{0x03};
uint8_t const kCuc_7{0x07};
uint8_t const kCuc_8{0x08};
uint8_t const kCuc_F{0xFF};
//********************************/
// uint16_t const kCus_0{0x0000};
// uint16_t const kCus_1{0x0001};
// uint16_t const kCus_2{0x0002};
// uint16_t const kCus_3{0x0003};
// uint16_t const kCus_4{0x0004};
// uint16_t const kCus_8{0x0008};
// uint16_t const kCus_16{0x0010};
// uint16_t const kCus_32{0x0020};
// uint16_t const kCus_64{0x0040};
// uint16_t const kCus_128{0x0080};
}  // namespace secoc
}  // namespace com
}  // namespace ara
namespace ara {
namespace com {
namespace secoc {
namespace internal {
//********************************/
uint8_t const kCuc_0{0x00};
uint8_t const kCuc_1{0x01};
uint8_t const kCuc_2{0x02};
uint8_t const kCuc_3{0x03};
uint8_t const kCuc_7{0x07};
uint8_t const kCuc_8{0x08};
uint8_t const kCuc_F{0xFF};
//********************************/
uint16_t const kCus_0{0x0000};
uint16_t const kCus_1{0x0001};
uint16_t const kCus_2{0x0002};
uint16_t const kCus_3{0x0003};
uint16_t const kCus_4{0x0004};
uint16_t const kCus_8{0x0008};
uint16_t const kCus_16{0x0010};
uint16_t const kCus_32{0x0020};
uint16_t const kCus_64{0x0040};
uint16_t const kCus_128{0x0080};
//********************************/
uint64_t const kCul_F{0xFFFFFFFFFFFFFFFF};
//********************************/
uint16_t const kDefaultValue_0{0x0000};
uint16_t const kDefaultValue_1{0x0001};
uint16_t const kDefaultValue_3{0x0003};
uint16_t const kDefaultValue_4{0x0004};
uint16_t const kDefaultValue_8{0x0008};
uint16_t const kDefaultValue_24{0x0018};
uint16_t const kDefaultValue_28{0x001C};
uint16_t const kDefaultValue_64{0x0040};
uint16_t const kDefaultValue_128{0x0080};
//********************************/
}  // namespace internal
}  // namespace secoc
}  // namespace com
}  // namespace ara

#endif