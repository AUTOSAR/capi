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
/// @file       string.h
/// @brief      Tool set string operation interface
/// @details
/// @date       2023-03-08
/// @author     xueliang.bao
/// @version    1.2.0
///
/// ================================================================

#ifndef ISOFT_UTILS_STRING_H_
#define ISOFT_UTILS_STRING_H_

#include <regex>
#include <string>
#include <vector>

namespace isoft {
namespace utils {

/// @brief Trim spaces from both ends of a string
/// @param str String to trim spaces from
/// @return String with spaces trimmed from both ends
std::string TrimSpace(std::string &str);

/// @brief Split a string by spaces based on the specified delimiter
/// @param str String to split
/// @param splitStr List of split strings
void SplitString(std::string const &str, std::vector< std::string > &splitStr) noexcept;

/// @brief Get Short Name from FQN
/// @param fqn Full name
/// @return short name
std::string GetShortName(std::string const &fqn);

}  // namespace utils
}  // namespace isoft

#endif
