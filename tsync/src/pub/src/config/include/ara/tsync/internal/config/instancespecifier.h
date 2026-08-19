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
/// @file       instancespecifier.h
/// @brief      configuration module instance descriptor class
/// @details
/// @date       2023-01-09
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

#ifndef ARA_TSYNC_INTERNAL_CONFIG_INSTANCESPECIFIER_H_
#define ARA_TSYNC_INTERNAL_CONFIG_INSTANCESPECIFIER_H_

#include "ara/core/string.h"
#include "isoft/define.h"
#include "isoft/manifestreader/manifest_node.h"
namespace ara {
namespace tsync {
namespace internal {
namespace config {

/// @brief instance descriptor
class InstanceSpecifier final
{
public:
    /// @brief default constructor
    InstanceSpecifier() = default;

    /// @brief default destructor
    ~InstanceSpecifier() = default;

    /// @brief default move constructor
    /// @param other - object reference
    InstanceSpecifier(InstanceSpecifier&& other) = default;

    /// @brief default copy constructor
    /// @param other - object reference
    InstanceSpecifier(InstanceSpecifier const& other) = default;

    /// @brief copy assignment
    /// @param other - object reference
    /// @return reference to self
    InstanceSpecifier& operator=(InstanceSpecifier const& other) & = default;

    /// @brief move assignment
    /// @param other - object reference
    /// @return reference to self
    InstanceSpecifier& operator=(InstanceSpecifier&& other) = default;

    /// @brief get instance descriptor
    /// @return instance descriptor
    ara::core::String const& GetInstanceSpecifier() const noexcept { return instanceSpecifier_; }

    /// @brief get time base name
    /// @return time base name
    ara::core::String const& GetTimeBaseName() const noexcept { return timeBaseName_; }

    /// @brief load function required by manifestReader interface
    /// @param node - manifest node
    /// @return kSucess/ErrorCode
    std::int32_t ManifestLoader(isoft::manifestreader::ManifestNode const& node) noexcept
    {
        /// @name kTimeBase
        static constexpr ara::core::StringView kTimeBase{"timeBase"};
        /// @name kInstanceSpecifier
        static constexpr ara::core::StringView kInstanceSpecifier{"instanceSpecifier"};
        this->instanceSpecifier_ = node.GetValue(kInstanceSpecifier, ara::core::String());
        this->timeBaseName_      = node.GetValue(kTimeBase, ara::core::String());
        return isoft::kSuccess;
    }

    /// @brief print debug information
    void Debug() const noexcept
    {
#if ARA_TSYNC_DEBUG
        std::cout << "instanceSpecifier: " << instanceSpecifier_ << std::endl;
        std::cout << "timeBase: " << timeBaseName_ << std::endl;
#endif
    }

private:
    /// @name instanceSpecifier_ - instance descriptor
    ara::core::String instanceSpecifier_;
    /// @name timeBaseName_ - time base name
    ara::core::String timeBaseName_;
};

}  // namespace config
}  // namespace internal
}  // namespace tsync
}  // namespace ara

#endif  /// ARA_TSYNC_INTERNAL_CONFIG_INSTANCESPECIFIER_H_
