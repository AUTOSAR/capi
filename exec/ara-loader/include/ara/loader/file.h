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
/// @brief      Utility file methods
/// @details
/// @date       2023-03-06
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#ifndef ISOFT_UTILS_FILE_H_
#define ISOFT_UTILS_FILE_H_

#include <cstdint>
#include <string>
#include <vector>

namespace isoft {
namespace utils {
namespace file {

/// @brief Search strategy
enum class SearchPolicy : uint8_t
{
    kMaxValue = 1,
    kMinValue = 2
};

/// @brief Search for files
/// @param dirPath Directory path
/// @param regStr Regular expression string
/// @param policy Search policy
/// @return File path
std::string Search(std::string const &dirPath, std::string const &regStr, SearchPolicy const &policy) noexcept;

}  // namespace file
}  // namespace utils
}  // namespace isoft

#endif  ///< ISOFT_UTILS_FILE_H_
