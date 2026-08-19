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
/// @file       argument.h
/// @brief      Template class for encapsulating log parameters
/// @details
/// @date       2024-06-28
/// @author     yangjinbiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/LOG
/// @interface_level = module
/// @trace_id_sr=LOG_SR_00001,LOG_SR_00002,LOG_SR_00003,LOG_SR_00004,LOG_SR_00006,LOG_SR_00010,LOG_SR_00011,LOG_SR_00012,LOG_SR_00013,LOG_SR_00014
/// @unit_name = argument
/// @unit_description=Lib interface side of Dlt, providing application interfaces externally for users
/// @endcode
///
/// ================================================================

#ifndef ARGUMENT_H
#define ARGUMENT_H

#include <sstream>
#include <string>
#include <type_traits>

namespace ara {
namespace log {
/// @brief A payload (quantity) with an unit wrapper
/// @code{.isoft}
/// @tparam T Payload type
/// @interface_level=unit
/// @trace_id_ad=AD_LOG_00063
/// @trace_id_dd=DD_LOG_00073
/// @needwork = ad
/// @endcode
template < typename T >
class Argument
{
public:
    /// @brief Copy constructor
    /// @param[in] object Argument&
    Argument(Argument const& object) = default;
    /// @brief Copy the assignment function
    /// @param[in] object Argument&
    /// @return Argument&
    Argument& operator=(Argument const& object) = default;
    /// @briefMove constructor
    /// @param[in] object Argument&& object
    Argument(Argument&& object) noexcept = default;
    /// @brief Move operation
    /// @param[in] object Argument&& object
    /// @return Argument&
    Argument& operator=(Argument&& object) noexcept = default;

public:
    /// @brief Alias for char
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00314
    /// @trace_id_dd=DD_LOG_01710
    /// @needwork = ad
    /// @endcode
    using Char8_T = char;

    /// @brief Data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00063
    /// @trace_id_dd=DD_LOG_00074
    /// @needwork = dda
    /// @endcode
    T arg;

    /// @brief Identifier
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00063
    /// @trace_id_dd=DD_LOG_00075
    /// @needwork = dda
    /// @endcode
    Char8_T const* name;

    /// @brief Unit
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00063
    /// @trace_id_dd=DD_LOG_00076
    /// @needwork = dda
    /// @endcode
    Char8_T const* unit;

public:
    /// @brief Argument
    /// @param[in]  payload  Payload
    /// @param[in]  identifier  Identifier
    /// @param[in]  outunit External unit
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00064
    /// @trace_id_dd=DD_LOG_00077
    /// @needwork = ad
    /// @endcode
    Argument(T&& payload, Char8_T const* identifier, Char8_T const* outunit) noexcept
        : arg(payload), name(identifier == nullptr ? "" : identifier), unit(outunit == nullptr ? "" : outunit)
    {
    }

    /// @brief Destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00065
    /// @trace_id_dd=DD_LOG_00078
    /// @needwork = ad
    /// @endcode
    ~Argument() noexcept = default;

    /// @brief Convert payload to standard std::string
    /// @returns Standard string
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_LOG_00066
    /// @trace_id_dd=DD_LOG_00079
    /// @needwork = ad
    /// @endcode
    std::string ToString() const noexcept
    {
        std::stringstream ss;
        ss.width(0);
        ss.flags(std::ios::left);
        ss << name << " " << arg << " " << unit;
        return ss.str();
    }
};

}  // namespace log
}  // namespace ara

#endif