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
/// @file       process.cpp
/// @brief      Process information
/// @details
/// @date       2024-04-20
/// @author     xueliang.bao
/// @version    1.2.0
///
/// ================================================================

#include <isoft/utils/process.h>

#include <array>
#include <iostream>
#include <sstream>
#include <string>
#include <tuple>

namespace isoft {
namespace utils {
namespace process {

/// @brief Redefine char
using Char8_t = char;

/// @brief Whether the process specified by pid is being traced
/// @param pid Process ID
/// @return true process is being traced; false process is not being traced
bool IsProcessTraced(int32_t const pid) noexcept
{
    std::string statFilePath;
    std::ignore = statFilePath.append("/proc/").append(std::to_string(pid)).append("/stat");
    FILE *const fp{fopen(statFilePath.c_str(), "re")};
    if (nullptr == fp) {
        perror("Read Proc trace state failed");
        return false;
    }

    constexpr uint16_t const kStatInfoLen{64U};
    std::array< Char8_t, kStatInfoLen > statInfo{};
    std::ignore = fgets(statInfo.data(), static_cast< int32_t >(kStatInfoLen), fp);
    std::ignore = fclose(fp);

    std::string statStr{statInfo.data()};
 std::size_t const pos{statStr.find_last_of(")")}; // Format: pid (exe_name) t ....
    if (pos == std::string::npos) {
        return false;
    }

    constexpr uint16_t const kForward2Bytes{2U};
    constexpr Char8_t const kTraceFlag{'t'};
    if (statStr[pos + kForward2Bytes] == kTraceFlag) {
        return true;
    }

    return false;
}

}  // namespace process
}  // namespace utils
}  // namespace isoft
