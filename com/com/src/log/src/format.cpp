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
/// @file       format.cpp
/// @brief      Format implementation file
/// @details
/// @date       2022-06-17
/// @author     mazelin
/// @version    1.2.0
///
/// ================================================================
///
/// @ref https://misc.flogisoft.com/bash/tip_colors_and_formatting
///
/// ================================================================

#include "ara/com/internal/format/format.h"

namespace ara {
namespace com {
namespace internal {
namespace format {
bool g_EnableColor{false};
/// @brief EnableColor
/// @param[in] enable
/// @return bool
bool EnableColor(bool enable) noexcept { return g_EnableColor = enable; }
/// @brief KeyPrefix
/// @return char const*
char const* KeyPrefix() noexcept
{
    if (!g_EnableColor) {
        return "\"";
    }
    return "\033[0m\"\033[38;5;222m";
}
/// @brief KeySuffix
/// @return char const*
char const* KeySuffix() noexcept
{
    if (!g_EnableColor) {
        return "\"";
    }
    return "\033[0m\"";
}
/// @brief ValPrefix
/// @return char const*
char const* ValPrefix() noexcept
{
    if (!g_EnableColor) {
        return "{";
    }
    return "\033[0m{\033[38;5;111m";
}
/// @brief ValSuffix
/// @return char const*
char const* ValSuffix() noexcept
{
    if (!g_EnableColor) {
        return "};";
    }
    return "\033[0m};";
}
/// @brief PairInfix
/// @return char const*
char const* PairInfix() noexcept
{
    if (!g_EnableColor) {
        return ": ";
    }
    return "\033[0m: ";
}
/// @brief ArgsInfix
/// @return char const*
char const* ArgsInfix() noexcept
{
    if (!g_EnableColor) {
        return " ";
    }
    return "\033[0m ";
}
/// @brief ModePrefix
/// @param[in] mode
/// @return char const*
char const* ModePrefix(FormatMode mode) noexcept
{
    if (!g_EnableColor) {
        return "";
    }
    char const* r{};
    switch (mode) {
        case FormatMode::kBlack: {
            r = "\033[0m\033[1;30m";
        } break;
        case FormatMode::kRed: {
            r = "\033[0m\033[1;31m";
        } break;
        case FormatMode::kGreen: {
            r = "\033[0m\033[1;32m";
        } break;
        case FormatMode::kYellow: {
            r = "\033[0m\033[1;33m";
        } break;
        case FormatMode::kBlue: {
            r = "\033[0m\033[1;34m";
        } break;
        case FormatMode::kMagenta: {
            r = "\033[0m\033[1;35m";
        } break;
        case FormatMode::kCyan: {
            r = "\033[0m\033[1;36m";
        } break;
        case FormatMode::kWhite: {
            r = "\033[0m\033[1;37m";
        } break;
        default: {
            r = "";
        } break;
    }
    return r;
}
/// @brief ModeSuffix
/// @return char const*
char const* ModeSuffix() noexcept
{
    if (!g_EnableColor) {
        return "";
    }
    return "\033[0m";
}
}  // namespace format
}  // namespace internal
}  // namespace com
}  // namespace ara