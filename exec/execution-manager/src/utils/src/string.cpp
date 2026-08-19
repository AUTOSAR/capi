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
/// @file       string.cpp
/// @brief      ara configuration series execution manifest class
/// @details
/// @date       2025-08-04
/// @author     xueliang.bao
/// @version    1.2.0
///
/// ================================================================

#include "isoft/utils/string.h"

#include <cstdint>

namespace isoft {
namespace utils {

namespace {

/// @brief Get Short Name from FQN separated by '/'
/// @param fqn Full name
/// @return short name
std::string GetOldShortName(std::string const &fqn)
{
    std::size_t const pos{fqn.find_last_of("/")};
    if (std::string::npos == pos) {
        return fqn;
    }

    return fqn.substr(pos + 1U);
}

/// @brief Get Short Name from FQN separated by "[]"
/// @param fqn Full name
/// @return short name
std::string GetNewShortName(std::string const &fqn)
{
 /// If the string does not contain [], treat fqn as short name
    std::size_t const fqnLen{fqn.length()};
    if (fqn[fqnLen - 1U] != ']') {
 /// If the last character of the string is not ']', but contains any character from "[.]", the format is considered invalid
        if (fqn.find_last_of("[.]") != std::string::npos) {
            return "";
        }

        return fqn;
    }

 /// If the last character is ']', the minimum length that meets the requirement should be "[a]"
 /// Therefore, if the string length is less than 3, even if the format is correct, it is an invalid fqn
    constexpr uint32_t kMinFqnLength{3U};
    if (fqnLen < kMinFqnLength) {
        return "";
    }

    std::size_t const beginPos{fqn.find_last_of("[")};
    std::size_t const endPos{fqn.find_last_of("]", fqnLen - 2U)};
 /// If the string ends with something like [aaaa]bbbb], it is also considered an error
    if ((beginPos == std::string::npos) && (endPos != std::string::npos)) {
        if (endPos > beginPos) {
            return "";
        }
    }

    return fqn.substr(beginPos + 1, fqnLen - beginPos - 2);
}

}  // namespace

/// @brief Trim spaces from both ends of a string
/// @param str String to trim spaces from
/// @return String with spaces trimmed from both ends
std::string TrimSpace(std::string &str)
{
    if (str.empty()) {
        return "";
    }

    std::ignore = str.erase(0U, str.find_first_not_of(" "));
    std::ignore = str.erase(str.find_last_not_of(" ") + 1U);

    return str;
}

/// @brief Split a string by spaces based on the specified delimiter
/// @param str String to split
/// @param splitStr List of split strings
void SplitString(std::string const &str, std::vector< std::string > &splitStr) noexcept
{
    std::regex const reg("\\s+");
    std::sregex_token_iterator pos{str.begin(), str.end(), reg, -1};
    std::sregex_token_iterator const end;
    for (; pos != end; ++pos) {
        splitStr.emplace_back(pos->str());
    }
}

/// @brief Get Short Name from FQN
/// @param fqn Full name
/// @return short name
std::string GetShortName(std::string const &fqn)
{
    std::string tmpFqn{fqn};
    TrimSpace(tmpFqn);

    if (tmpFqn.empty()) {
        return "";
    }

    std::size_t const pos{tmpFqn.find_last_of("/")};
    if (pos != std::string::npos) {
        return GetOldShortName(tmpFqn);
    }

    return GetNewShortName(tmpFqn);
}

}  // namespace utils
}  // namespace isoft