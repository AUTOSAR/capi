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
/// @file       base.h
/// @brief      Communication basic header file
/// @details
/// @date       2021-12-31
/// @author     mazelin
/// @version    1.2.0
///
/// ================================================================

#pragma once

#include <cstdint>

#include "ara/com/internal/time.h"
#include "ara/core/string.h"

namespace ara {
namespace com {
namespace internal {
/// @brief Service identifier type
/// @code{.isoft}
/// export_level=/COM/SoC
/// @endcode
/// @ref [SWS_CM_01010] -- Service Identifier Class
struct ServiceIdentifier
{
    /// @brief Constructor
    /// @param[in] value Service identifier value
    explicit ServiceIdentifier(ara::core::StringView value) noexcept;
    /// @brief Destructor
    ~ServiceIdentifier() noexcept;
    /// @brief Copy constructor
    /// @param other
    ServiceIdentifier(ServiceIdentifier const& other) = default;
    /// @brief Move constructor
    /// @param other
    ServiceIdentifier(ServiceIdentifier&& other) noexcept = default;
    /// @brief Copy assignment operator
    /// @param other
    /// @return ServiceIdentifier
    ServiceIdentifier& operator=(ServiceIdentifier const& other) noexcept = default;
    /// @brief Move assignment operator
    /// @param other
    /// @return ServiceIdentifier
    ServiceIdentifier& operator=(ServiceIdentifier&& other) noexcept = default;
    /// @brief Operator -- equality comparison
    /// @param[in] other
    /// @return bool
    bool operator==(ServiceIdentifier const& other) const noexcept;
    /// @brief Operator -- less than comparison
    /// @param[in] other
    /// @return bool
    bool operator<(ServiceIdentifier const& other) const noexcept;
    /// @brief Convert to string
    /// @return Service identifier string
    ara::core::StringView ToString() const noexcept;
    /// @brief Service identifier value
    ara::core::String value;
};

/// @brief Service version type
/// @code{.isoft}
/// export_level=/COM/SoC
/// @endcode
/// @ref [SWS_CM_01010] -- Service Version Class
struct ServiceVersion
{
    /// @brief Constructor
    /// @param[in] value Service version value
    explicit ServiceVersion(ara::core::StringView value) noexcept;
    /// @brief Destructor
    ~ServiceVersion() noexcept;
    /// @brief Copy constructor
    /// @param other
    ServiceVersion(ServiceVersion const& other) = default;
    /// @brief Move constructor
    /// @param other
    ServiceVersion(ServiceVersion&& other) noexcept = default;
    /// @brief Copy assignment operator
    /// @param other
    /// @return ServiceVersion
    ServiceVersion& operator=(ServiceVersion const& other) noexcept = default;
    /// @brief Move assignment operator
    /// @param other
    /// @return ServiceVersion
    ServiceVersion& operator=(ServiceVersion&& other) noexcept = default;
    /// @brief Operator -- equality comparison
    /// @param[in] other
    /// @return bool
    bool operator==(ServiceVersion const& other) const noexcept;
    /// @brief Operator -- less than comparison
    /// @param[in] other
    /// @return bool
    bool operator<(ServiceVersion const& other) const noexcept;
    /// @brief Convert to string
    /// @return Service version string
    ara::core::StringView ToString() const noexcept;
    /// @brief Service version value
    ara::core::String value;
};

/// @brief Service contract version type
/// @code{.isoft}
/// export_level=/COM/SoC
/// @endcode
/// @ref [SWS_CM_01010] -- Service Contract Version
using ServiceContractVersion = uint32_t;
}  // namespace internal
}  // namespace com
}  // namespace ara