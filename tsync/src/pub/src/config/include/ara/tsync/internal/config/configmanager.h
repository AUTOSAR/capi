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
/// @file       configmanager.h
/// @brief      configuration module management class
/// @details
/// @date       2023-01-11
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// export_level=/TimeSync/Config
/// module_path=/TimeSync/Config
/// @endcode
///
/// ================================================================

#ifndef ARA_TSYNC_INTERNAL_CONFIG_MANAGER_H_
#define ARA_TSYNC_INTERNAL_CONFIG_MANAGER_H_

#include <ara/core/set.h>
#include <ara/core/string_view.h>

#include <cstdint>

#include "ara/tsync/internal/config/timebaseset.h"
#include "ara/tsync/internal/config/timedomainset.h"

namespace ara {
namespace tsync {
namespace internal {
namespace config {

/// @brief configuration manager, aggregates all configuration related to time synchronization
class ConfigManager final
{
public:
    /// @brief default constructor
    ConfigManager() = default;

    /// @brief copy constructor
    /// @param other - other object
    ConfigManager(ConfigManager const& other) = delete;

    /// @brief move constructor is prohibited
    /// @param other - other object
    ConfigManager(ConfigManager&& other) = default;

    /// @brief copy assignment
    /// @param other
    /// @return reference to self
    ConfigManager& operator=(ConfigManager const& other) = delete;

    /// @brief move assignment is prohibited
    /// @param other
    /// @return reference to self
    ConfigManager& operator=(ConfigManager&& other) = delete;

    /// @brief default destructor
    ~ConfigManager() noexcept
    {
        timeBaseSet_   = nullptr;
        timeDomainSet_ = nullptr;
    }

    /// @brief CreateManager - construct configuration manager
    /// @return configuration manager pointer; return nullptr on failure.
    static std::shared_ptr< ConfigManager > CreateManager() noexcept;

    /// @brief get time base configuration set
    /// @return time base set
    std::shared_ptr< TimeBaseSet const > GetTimeBaseSet() const noexcept
    {
        return std::shared_ptr< TimeBaseSet const >{timeBaseSet_};
    }

    /// @brief get time domain configuration set
    /// @return time domain set
    std::shared_ptr< TimeDomainSet const > GetTimeDomainSet() const noexcept
    {
        return std::shared_ptr< TimeDomainSet const >{timeDomainSet_};
    }

private:
    /// @brief load configuration
    /// @return 0 - success
    /// @return <0 - failure
    std::int32_t _load() noexcept;

    /// @brief parse configuration data, analyze data that needs to be coordinated and shared within the manager
    /// @return 0 - success
    /// @return <0 - failure
    std::int32_t _parse() noexcept;

private:
    /// @name timeBaseSet_ - time base set
    std::shared_ptr< TimeBaseSet > timeBaseSet_{nullptr};

    /// @name timeDomainSet_ - time domain set handle
    std::shared_ptr< TimeDomainSet > timeDomainSet_{nullptr};
};

}  // namespace config
}  // namespace internal
}  // namespace tsync
}  // namespace ara

#endif  //_ARA_TSYNC_INTERNAL_CONFIG_CENTER_H_
