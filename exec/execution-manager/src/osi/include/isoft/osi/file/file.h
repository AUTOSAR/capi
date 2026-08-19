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
/// @file       file.h
/// @brief      OS interface layer file series operations
/// @details
/// @date       2023-03-08
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#ifndef ISOFT_OSI_FILE_H_
#define ISOFT_OSI_FILE_H_

#include <unistd.h>

#include <cstdint>

namespace isoft {
namespace osi {
namespace file {

/// @brief Check file permission
/// @param pathName File path
/// @param mode Permission to check
/// @return 0 success; <0 failure
inline int32_t Access(char const *pathName, int32_t const mode) { return access(pathName, mode); }

}  // namespace file
}  // namespace osi
}  // namespace isoft

#endif  ///< ISOFT_OSI_FILE_H_