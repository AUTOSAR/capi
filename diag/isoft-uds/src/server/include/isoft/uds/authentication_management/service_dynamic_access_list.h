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
/// @file       service_dynamic_access_list.h
/// @brief      This file provides service_dynamic_access_list functionality
/// @details
/// @date       2026-03-31
/// @author     kai.ju
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/isoft/uds/authentication_management
/// @interface_level= module
/// @trace_id_sr=be95a5ba-d146-4256-8487-7990350ead3f
/// @unit_name=ServiceDynamicAccessList
/// @unit_description=This file provides service_dynamic_access_list functionality
/// @endcode
///
/// ================================================================

#ifndef ISOFT_UDS_SERVER_SERVICE_DYNAMIC_ACCESS_LIST_H_
#define ISOFT_UDS_SERVER_SERVICE_DYNAMIC_ACCESS_LIST_H_
#include <cstdint>
#include <vector>
namespace isoft {
namespace uds {
namespace server {

/// @brief Enumeration of rule item types
///
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = ad
///
/// @endcode
enum class RuleType
{
    kWildcard,  // Wildcard
    kRange,     // Range match
    kValue      // Exact value match
};

/// @brief Define matching rule for a single byte
///
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = ad
///
/// @endcode
struct RuleItem
{
    /// @brief Type of rule item
    RuleType type{};
    /// @brief Minimum value of rule item
    /// @note Only valid for RANGE type
    /// @default 0
    std::uint8_t minVal{0};  // Valid for RANGE type
    /// @brief Maximum value of rule item
    /// @note Only valid for RANGE and VALUE types
    /// @default UINT8_MAX
    std::uint8_t maxVal{UINT8_MAX};  // Valid for RANGE and VALUE types (min_val equals max_val for VALUE)

    /// @brief Default constructor
    /// @param maxVal Parameter
    /// @return Return value
    /// @code{.isoft}
    /// @note Only valid for wildcard type
    /// @default RuleType::kWildcard
    /// @interface_level=unit
    /// @needwork = ad
    /// @endcode
    RuleItem() : type(RuleType::kWildcard), maxVal{UINT8_MAX} {}
    /// @brief RuleItem function
    /// @param val Parameter
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = ad
    /// @endcode
    explicit RuleItem(std::uint8_t val) : type(RuleType::kValue), minVal(val), maxVal(val) {}

    /// @brief RuleItem function
    /// @param min Parameter
    /// @param max Parameter
    /// @return Return value
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = ad
    /// @endcode
    RuleItem(std::uint8_t min, std::uint8_t max) : type(RuleType::kRange), minVal(min), maxVal(max) {}
};

/// @brief Define service dynamic access list structure
///
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = ad
///
/// @endcode
struct ServiceDynamicAccess
{
    /// @brief List of rule items for service dynamic access list
    /// @note Used to match rules for service dynamic access
    /// @default empty
    std::vector< RuleItem > ruleList;
};
/// @brief Type of service dynamic access list
using ServiceDynamicAccessList = std::vector< ServiceDynamicAccess >;
}  // namespace server
}  // namespace uds
}  // namespace isoft
#endif  // ISOFT_UDS_SERVER_SERVICE_DYNAMIC_ACCESS_LIST_H_