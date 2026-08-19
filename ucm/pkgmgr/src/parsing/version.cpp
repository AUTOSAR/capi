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
/// @file       version.cpp
/// @brief      Implementation of the version class
/// @details
/// @date       2024-08-15
/// @author     cuiyinli
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/UCM/SoftwarePackageManager
/// @interface_level=unit
/// @trace_id_sr=SR_UCM_00001
/// @unit_name=Version
/// @unit_description=Implementation of the version class
/// @endcode
///
/// ================================================================

#include "version.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief kDot
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10255
/// @trace_id_dd=DD_UCM_10399
/// @needwork = dda
/// @endcode
constexpr char8_t kDot{'.'};

/// @brief Initializes this version with vmajor.vminor.vpatch
///
/// @param vmajor The major version number
/// @param vminor The minor version number
/// @param vpatch The patch version number
/// @throws no
Version::Version(uint32_t const vmajor, uint32_t const vminor, uint32_t const vpatch)
    : vmajor_{vmajor}
    , vminor_{vminor}
    , vpatch_{vpatch}
    , verStr_{std::to_string(vmajor) + kDot + std::to_string(vminor) + kDot + std::to_string(vpatch)}
{
}

/// @brief Initializes this version by parsing a version from a string.
///
/// Only the major, minor and patch version numbers are considered from the version string.
///
/// @param verStr A version string
///
/// @throws no
Version::Version(AraString const& verStr) : verStr_{verStr}
{
    if (verStr.empty()) {
        valid_ = false;
        return;
    }

    std::int32_t const expect{3};
    std::int32_t const nassign{std::sscanf(verStr.c_str(),  // NOLINT : Recommend using C++ strtoul to replace sscanf
                                           "%u.%u.%u", &vmajor_, &vminor_, &vpatch_)};
    if (nassign != expect) {
        valid_ = false;
    }
}

/// @brief Represents the version as a string in the format
/// @returns The provided stream
/// @throws no
AraString Version::ToString() const { return verStr_; }

/// @brief The version is valid or not
/// @returns bool
/// @throws no
bool Version::IsValid() const noexcept { return valid_; }

/// @brief Prints the version to the stream in the serialized format
/// @param stream The stream this version will be serialized to
/// @param ver The Version to serialize
/// @returns The provided stream
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10255
/// @trace_id_dd=DD_UCM_10404
/// @needwork = no
/// @endcode
std::ostream& operator<<(std::ostream& stream, Version const& ver) { return stream << ver.ToString(); }

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara
