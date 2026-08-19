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
/// @file       string_ext.cpp
/// @brief      string extension utility implementation
/// @details
/// @date       2024-08-15
/// @author     cuiyinli
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/UCM/Utils
/// @interface_level=unit
/// @trace_id_sr=SR_UCM_00001
/// @unit_name=StringExt
/// @unit_description=string extension utility implementation
/// @endcode
///
/// ================================================================

#include "string_ext.h"

#include "ara/ucm/internal/extraction/tinyfs.h"

namespace ara {
namespace ucm {
namespace pkgmgr {
namespace strutil {

/// @brief convert string with format of bigcase
/// @param data source string
/// @returns string in format of bigcase
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10404
/// @trace_id_dd=DD_UCM_10788
/// @needwork = dd
/// @endcode
AraString Upper(AraString const& data)
{
    AraString ret{data};

    for (char8_t& c : ret) {
        c = static_cast< char8_t >(std::toupper(static_cast< std::int32_t >(static_cast< uchar8_t >(c))));
    }

    return ret;
}

/// @brief join vector as string
/// @param items a vector
/// @returns string
/// @throws no
AraString JoinVector(AraVectorString const& items)
{
    AraString ret;

    auto it = items.begin();
    if (it == items.end()) {
        return ret;
    }
    ret = *it;
    ++it;
    for (; it != items.end(); ++it) {
        ret += ", " + *it;
    }
    return ret;
}

/// @brief join map as string
/// @param items a map
/// @returns string
/// @throws no
AraString JoinMap(AraMap< AraString, AraString > const& items)
{
    AraString ret{"{"};

    AraMap< AraString, AraString >::const_iterator it;
    for (it = items.begin(); it != items.end(); ++it) {
        if (it != items.begin()) {
            ret += ", ";
        }
        ret += it->first + ":" + it->second;
    }
    ret += "}";

    return ret;
}

/// @brief compare between first and second, ignore bigcase and lowcase
/// @param first
/// @param second
/// @returns bool
/// @throws no
bool CaseCompare(AraString const& first, AraString const& second) noexcept
{
    if (first.length() != second.length()) {
        return false;
    }

    return 0 == strncasecmp(first.c_str(), second.c_str(), first.length());
}

/// @brief hex string convert to byte vector
/// @param hexStr source hex string
/// @returns byte vector
/// @throws no
AraVector< AraByte > HexStrToBytes(AraString const& hexStr)
{
    AraVector< AraByte > ret;
    size_t const step{2U};

    for (size_t i{0U}; i < hexStr.length(); i += step) {
        AraStringView const sub{hexStr.substr(i, step)};
        AraByte const byt{static_cast< uchar8_t >(strtol(sub.data(), nullptr, 16))};
        ret.push_back(byt);
    }

    return ret;
}

/// @brief replace the first 'from' to 'to'
/// @param str source string
/// @param from
/// @param to
/// @returns bool
/// @throws no
bool ReplaceFirst(AraString& str, AraString const& from, AraString const& to) noexcept
{  // PRQA S 1503
    size_t const startPos{str.find(from)};
    if (startPos == std::string::npos) {
        return false;
    }

    std::ignore = str.replace(startPos, from.length(), to);  // PRQA S 3803
    return true;
}

/// @brief replace the last 'from' to 'to'
/// @param str source string
/// @param from
/// @param to
/// @returns bool
/// @throws no
bool ReplaceLast(AraString& str, AraString const& from, AraString const& to) noexcept
{  // PRQA S 1503
    size_t const startPos{str.rfind(from)};
    if (startPos == std::string::npos) {
        return false;
    }

    std::ignore = str.replace(startPos, from.length(), to);  // PRQA S 3803
    return true;
}

/// @brief StrGetOption
/// @param args
/// @param optionName
/// @return option value
/// @throws no
AraString GetOption(std::list< AraStringView > const& args, AraStringView const& optionName)
{
    AraString optionValue;

    std::list< AraStringView >::const_iterator it;
    for (it = args.begin(); it != args.end(); ++it) {
        if (*it == optionName) {
            auto const nextIt = std::next(it);
            if (nextIt != args.end()) {
                optionValue = *nextIt;
                break;
            }
        }
    }

    return optionValue;
}

AraString GetLastComponent(AraString const& fqn)
{
    size_t const lastLeftBracketPos{fqn.rfind('[')};
    size_t const lastRightBracketPos{fqn.rfind(']')};
    if (lastLeftBracketPos == std::string::npos || lastRightBracketPos == std::string::npos
        || lastLeftBracketPos >= lastRightBracketPos) {
        return tinyfs::Basename(fqn);
    }

    return fqn.substr(lastLeftBracketPos + 1, lastRightBracketPos - lastLeftBracketPos - 1);
}

}  // namespace strutil
}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara
