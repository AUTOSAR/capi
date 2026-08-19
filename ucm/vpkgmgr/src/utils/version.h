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
/// @file       version.h
/// @brief      Definition of the version class
/// @details
/// @date       2024-07-20
/// @author     hanzhibo
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// export_level=/UCM Master/Utils
/// @module_path=/UCM Master/Utils
/// @interface_level=module
/// @trace_id_sr=SR_UCM_00035
/// @unit_name=Version
/// @unit_description=Definition of the version class
/// @endcode
///
/// ================================================================

#ifndef ARA_UCM_PKGMGR_PARSING_VERSION_H_
#define ARA_UCM_PKGMGR_PARSING_VERSION_H_

#include <cstdint>
#include <cstdio>
#include <iostream>
#include <stdexcept>

#include "ara/core/string.h"

namespace ara {
namespace ucm {
namespace vpkgmgr {

/// @brief This class implements the version of an adaptive application
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00419
/// @trace_id_dd=DD_UCM_Master_00828
/// @needwork = ad
/// @endcode
class Version
{
private:
    /// @brief The major version number (first number in the version string)
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00829
    /// @needwork = dda
    /// @endcode
    uint32_t vmajor_{0U};
    /// @brief The minor version number (second number in the version string)
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00830
    /// @needwork = dda
    /// @endcode
    uint32_t vminor_{0U};
    /// @brief The patch version number (third number in the version string)
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00831
    /// @needwork = dda
    /// @endcode
    uint32_t vpatch_{0U};

    /// @brief The version string.
    /// The string must respect the format
    /// (0|[1-9]\d*)\.(0|[1-9]\d*)\.(0|[1-9]\d*)(-((0|[1-9]\d*|\d*[a-zA-Z-][0-9a-zA-Z-]*)(\.(0|[1-9]\d*|\d*[a-zA-Z-][0-9a-zA-Z-]*))*))?(\+([0-9a-zA-Z-]+(\.[0-9a-zA-Z-]+)*))?
    ///
    /// The value cannot be empty.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00832
    /// @needwork = dda
    /// @endcode
    ara::core::String verStr_{""};

    /// @brief The version is valid or not
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00833
    /// @needwork = dda
    /// @endcode
    bool valid_{false};

public:
    /// @brief Initializes this version with vmajor.vminor.vpatch
    ///
    /// @param vmajor The major version number
    /// @param vminor The minor version number
    /// @param vpatch The patch version number
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00834
    /// @needwork = dda
    /// @endcode
    Version(uint32_t const vmajor, uint32_t const vminor, uint32_t const vpatch);

    /// @brief Initializes this version by parsing a version from a string.
    ///
    /// Only the major, minor and patch version numbers are considered from the version string.
    ///
    /// @param verStr A version string
    ///
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00835
    /// @needwork = dda
    /// @endcode
    explicit Version(ara::core::String const& verStr);

    /// @brief constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00836
    /// @needwork = dda
    /// @endcode
    Version() = default;

    /// @brief Represents the version as a string in the format
    /// (0|[1-9]\d*)\.(0|[1-9]\d*)\.(0|[1-9]\d*)(-((0|[1-9]\d*|\d*[a-zA-Z-][0-9a-zA-Z-]*)(\.(0|[1-9]\d*|\d*[a-zA-Z-][0-9a-zA-Z-]*))*))?(\+([0-9a-zA-Z-]+(\.[0-9a-zA-Z-]+)*))?
    ///
    /// @returns The provided stream
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00837
    /// @needwork = dda
    /// @endcode
    ara::core::String ToString() const;

    /// @brief The version is valid or not
    /// @returns bool
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00838
    /// @needwork = dda
    /// @endcode
    bool IsValid() const noexcept;

    /// @brief Version A < B if
    /// (A.major < B.major).
    /// else if (A.major == B.major)&&(A.minor < B.minor)
    /// else if  (A.major == B.major)&&(A.minor == B.minor) && (A.patch < B.patch))
    ///
    /// @param lver The left  Version to compare agains this version
    /// @param rver The right Version to compare agains this version
    ///
    /// @returns true if *this < other, else false
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00839
    /// @needwork = dda
    /// @endcode
    friend constexpr bool operator<(Version const& lver, Version const& rver) noexcept
    {
        if (lver.vmajor_ != rver.vmajor_) {
            return lver.vmajor_ < rver.vmajor_;
        }
        if (lver.vminor_ != rver.vminor_) {
            return lver.vminor_ < rver.vminor_;
        }
        return lver.vpatch_ < rver.vpatch_;
    }

    /// @brief Compares two Version objects.
    ///
    /// Two Version objects are equal if major, minor and patch version numbers are equal
    ///
    /// @param lver The left  Version to compare agains this version
    /// @param rver The right Version to compare agains this version
    ///
    /// @returns true if both Version objects are equal
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00840
    /// @needwork = dda
    /// @endcode
    friend constexpr bool operator==(Version const& lver, Version const& rver) noexcept
    {
        return ((lver.vmajor_ == rver.vmajor_) && (lver.vminor_ == rver.vminor_) && (lver.vpatch_ == rver.vpatch_));
    }

    /// @brief Version A > B if !(A<=B)
    ///
    /// @param lver The left  Version to compare agains this version
    /// @param rver The right Version to compare agains this version
    ///
    /// @returns true if this > other
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00841
    /// @needwork = dda
    /// @endcode
    friend constexpr bool operator>(Version const& lver, Version const& rver) noexcept
    {
        return !(lver < rver) && !(lver == rver);
    }

    /// @brief Version A <= B if !(A>B)
    ///
    /// @param lver The left  Version to compare agains this version
    /// @param rver The right Version to compare agains this version
    ///
    /// @returns true if this <= other
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00842
    /// @needwork = dda
    /// @endcode
    friend constexpr bool operator<=(Version const& lver, Version const& rver) noexcept { return !(lver > rver); }

    /// @brief Version A >= B if !(A < B)
    ///
    /// @param lver The left  Version to compare agains this version
    /// @param rver The right Version to compare agains this version
    ///
    /// @returns true if this >= other
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_Master_00000
    /// @trace_id_dd=DD_UCM_Master_00843
    /// @needwork = dda
    /// @endcode
    friend constexpr bool operator>=(Version const& lver, Version const& rver) noexcept { return !(lver < rver); }
};

// overload Stream extraction operator
/// @brief Prints the version to the stream in the serialized format
///
/// @param stream The stream this version will be serialized to
/// @param ver The Version to serialize
///
/// @returns The provided stream
/// @throws no
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_Master_00419
/// @trace_id_dd=DD_UCM_Master_00844
/// @needwork = dd
/// @endcode
std::ostream& operator<<(std::ostream& stream, Version const& ver);

}  // namespace vpkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // ARA_UCM_PKGMGR_PARSING_VERSION_H_
