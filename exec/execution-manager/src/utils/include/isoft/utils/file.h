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
/// @brief      Tool set file methods
/// @details
/// @date       2023-03-06
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#ifndef ISOFT_UTILS_FILE_H_
#define ISOFT_UTILS_FILE_H_

#include <string>

namespace isoft {
namespace utils {
namespace file {

/// @brief Copy file to another location
/// @param srcFileName Source file
/// @param dstFileName Destination file
/// @return 0 success; <0 failure
int32_t CopyFile(std::string const &srcFileName, std::string const &dstFileName) noexcept;  // PRQA S 2024

/// @brief Create nested directories
/// @param dirName Nested directory name
/// @return 0 success; <0 failure
int32_t MakeCascadeDir(std::string const &dirName) noexcept;  // PRQA S 2024

/// @brief Remove directory (including files in the directory)
/// @param dirName Directory name
/// @return 0 success; <0 failure
int32_t RemoveDir(std::string const &dirName) noexcept;  // PRQA S 2024

}  // namespace file
}  // namespace utils
}  // namespace isoft

#endif  ///< ISOFT_UTILS_FILE_H_
