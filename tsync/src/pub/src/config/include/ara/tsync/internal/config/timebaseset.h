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
/// @file       timebaseset.h
/// @brief      configuration module time base set class
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

#ifndef ARA_TSYNC_INTERNAL_CONFIG_TIMEBASESET_H_
#define ARA_TSYNC_INTERNAL_CONFIG_TIMEBASESET_H_

#include <ara/core/string.h>
#include <ara/core/string_view.h>
#include <ara/core/vector.h>
#include <isoft/manifestreader/manifest_node.h>
#include <isoft/manifestreader/manifest_reader.h>

#include <cstdint>

#include "ara/tsync/internal/TimeDomainId.h"
#include "ara/tsync/internal/config/common.h"
#include "ara/tsync/internal/config/timebase.h"
#include "ara/tsync/internal/config/timedomainset.h"

namespace ara {
namespace tsync {
namespace internal {
namespace config {

/// @brief time base configuration set
class TimeBaseSet final
{
public:
    /// @brief default constructor
    /// @exception exception specification
    TimeBaseSet() = default;

    /// @brief move constructor is prohibited
    /// @param tb time base object
    TimeBaseSet(TimeBaseSet &&tb) = delete;

    /// @brief copy constructor is prohibited
    /// @param tb time base object
    TimeBaseSet(TimeBaseSet const &tb) = delete;

    /// @brief default destructor
    ~TimeBaseSet() = default;

    /// @brief '=' operator overload
    /// @param other an existing rvalue reference.
    /// @return reference to class object
    TimeBaseSet &operator=(TimeBaseSet &&other) &noexcept = default;

    /// @brief copy is prohibited
    /// @param other - other object
    /// @return reference to class object
    TimeBaseSet &operator=(TimeBaseSet &other) = delete;

    /// @brief load configuration
    /// @param manifestPath - configuration file path
    /// @return 0 - success
    /// @return <0 - failure
    std::int32_t Load(ara::core::String const &manifestPath) noexcept;

    /// @brief print debug information
    void Debug() const noexcept;

    /// @brief get all time base configurations
    /// @return time base configuration set
    ara::core::Vector< TimeBase > const &GetAll() const noexcept { return timeBaseSet_; }

    /// @brief get time base pointer based on time base name
    /// @param name - time base name
    /// @return time base configuration pointer
    TimeBase const *GetTimeBase(ara::core::String const &name) const noexcept;

    /// @brief get the names of time domains bound to all time base providers
    /// @param domainNameList - list of time domain names
    void GetTimeDomainNameListWithProvider(ara::core::Vector< ara::core::String > &domainNameList) const noexcept;

    /// @brief set time domain configuration for time base
    /// @param pTimeDomainSet - time domain set
    /// @return time base configuration pointer
    void SetTimeDomainId(std::shared_ptr< TimeDomainSet > const &pTimeDomainSet) noexcept;

private:
private:
    /// @name timeBaseSet_ - time base configuration set
    ara::core::Vector< TimeBase > timeBaseSet_;
};  /// class TimeBaseSet

}  // namespace config
}  // namespace internal
}  // namespace tsync
}  // namespace ara

#endif  /// ARA_TSYNC_INTERNAL_CONFIG_TIMEBASESET_H_
