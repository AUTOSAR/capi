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
/// @file       rjson.cpp
/// @brief      rapid json simple wrapper implementation
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
/// @unit_name=RJson
/// @unit_description=rapid json simple wrapper implementation
/// @endcode
///
/// ================================================================

#include "rjson.h"

namespace ara {
namespace ucm {
namespace pkgmgr {
namespace rjson {

/// @brief OpenAndCheck
/// @param rootDom
/// @param file
/// @param ls
/// @returns bool and doc
/// @throws no
bool OpenAndCheck(Doc& rootDom, AraStringView const& file, ara::log::LogStream&& ls)
{
    std::ifstream ifStream{file.data(), std::ifstream::in};
    bool const isGood{ifStream.good()};
    if ((!ifStream.is_open()) || (!isGood)) {
        ls << file << " can't be opened properly for reading";
        return false;
    }
    if (ifStream.peek() == std::fstream::traits_type::eof()) {
        ls << file << " is empty";
        ifStream.close();
        return false;
    }

    rapidjson::IStreamWrapper jsonIsw{ifStream};
    std::ignore = rootDom.ParseStream(jsonIsw);
    ifStream.close();
    if (rootDom.HasParseError()) {
        ls << file << " the schema is not a valid JSON";
        return false;
    }

    return true;
}

}  // namespace rjson
}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara
