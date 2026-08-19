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
/// @file       md5.h
/// @brief
/// @details
/// @date       2024-09-11
/// @author     mazelin
/// @version    1.2.0
///
/// ================================================================

#pragma once

#include <cstdint>

#define MD5_NUM_LEN 16
#define MD5_STR_LEN 32

extern int32_t MD5Num2Str(uint8_t const* md5num, char* md5str) noexcept;
extern int32_t MD5Str2Num(uint8_t* md5num, char const* md5str) noexcept;
extern int32_t File2MD5Num(char const* file, uint8_t* md5num) noexcept;
extern int32_t Str2MD5Num(char const* str, uint8_t* md5num) noexcept;
