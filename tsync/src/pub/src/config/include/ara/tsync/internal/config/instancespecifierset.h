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
/// @file       instancespecifierset.h
/// @brief      configuration module instance descriptor set class
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

#ifndef ARA_TSYNC_INTERNAL_CONFIG_INSTANCESPECIFIERSET_H_
#define ARA_TSYNC_INTERNAL_CONFIG_INSTANCESPECIFIERSET_H_

#include "ara/core/string.h"
#include "ara/core/vector.h"
#include "ara/tsync/internal/common.h"
#include "ara/tsync/internal/config/common.h"
#include "ara/tsync/internal/config/instancespecifier.h"
#include "isoft/define.h"
#include "isoft/manifestreader/manifest_node.h"
#include "isoft/manifestreader/manifest_reader.h"
namespace ara {
namespace tsync {
namespace internal {
namespace config {

/// @brief instance descriptor set
class InstanceSpecifierSet final
{
public:
    /// @brief default constructor
    /// @exception on overflow
    InstanceSpecifierSet() = default;
    /// @brief default destructor
    ~InstanceSpecifierSet() = default;

    /// @brief move constructor
    /// @param ins -
    InstanceSpecifierSet(InstanceSpecifierSet &&ins) = default;

    /// @brief copy constructor is prohibited
    /// @param ins -
    InstanceSpecifierSet(InstanceSpecifierSet const &ins) = delete;

    /// @brief copy is prohibited
    /// @param other - other object
    /// @return reference to class object
    InstanceSpecifierSet &operator=(InstanceSpecifierSet const &other) = delete;

    /// @brief '=' operator overload
    /// @param other an existing rvalue reference.
    /// @return reference to class object
    InstanceSpecifierSet &operator=(InstanceSpecifierSet &&other) &noexcept = default;

    /// @brief load configuration
    /// @param manifestPath - configuration file path
    /// @return 0 - success
    /// @return <0 - failure
    std::int32_t Load(ara::core::String const &manifestPath) noexcept
    {
        /////////////// Key ////////////////
        /// @name kProcessTimeBaseMap
        static constexpr ara::core::StringView kProcessTimeBaseMap{"ProcessTimeBaseMap"};
        ara::core::StringView const path{manifestPath.c_str(), manifestPath.length()};
        ara::core::Result< std::unique_ptr< isoft::manifestreader::Manifest > > manifestRes{
            isoft::manifestreader::OpenManifest(path)};
        if (!manifestRes.HasValue()) {
            return kRET_E1;
        }
        std::unique_ptr< isoft::manifestreader::Manifest > const manifest{std::move(manifestRes).Value()};
        return manifest->Load(kProcessTimeBaseMap, instanceSpecifierSet_);
    }

    /// @brief print debug information
    void Debug() const noexcept
    {
#if ARA_TSYNC_DEBUG
        for (auto const &tbm : instanceSpecifierSet_) {
            tbm.Debug();
        }
#endif
    }

    /// @brief get mapped time base name based on instance descriptor
    /// @param instanceSpecifier - instance descriptor
    /// @return time base name
    ara::core::StringView GetTimeBaseNameByInstanceSpecifier(ara::core::String const &instanceSpecifier) const noexcept
    {
        ara::core::StringView sv;
        for (ara::core::Vector< InstanceSpecifier >::const_iterator itm{instanceSpecifierSet_.cbegin()};
             itm != instanceSpecifierSet_.cend(); ++itm) {
            if (instanceSpecifier == itm->GetInstanceSpecifier()) {
                ara::core::String const &tbName{itm->GetTimeBaseName()};
                sv = ara::core::StringView{tbName.data(), tbName.size()};
                break;
            }
        }
        return sv;
    }

    /// @brief get mapped instance descriptor based on time base name
    /// @param tbName - time base name
    /// @return instance descriptor
    ara::core::StringView GetInstanceSpecifierByTimeBase(ara::core::String const &tbName) const noexcept
    {
        ara::core::StringView sv;
        for (ara::core::Vector< InstanceSpecifier >::const_iterator itm{instanceSpecifierSet_.cbegin()};
             itm != instanceSpecifierSet_.cend(); ++itm) {
            ara::core::String const &ourTbName{itm->GetTimeBaseName()};
            if (BaseName(tbName) == BaseName(ourTbName)) {
                ara::core::String const &insName{itm->GetInstanceSpecifier()};
                sv = ara::core::StringView{insName.data(), insName.size()};
                break;
            }
        }
        return sv;
    }

private:
    /// @name instanceSpecifierSet_ - instance descriptor set
    ara::core::Vector< InstanceSpecifier > instanceSpecifierSet_;

};  /// class InstanceSpecifierSet

}  // namespace config
}  // namespace internal
}  // namespace tsync
}  // namespace ara

#endif  /// ARA_TSYNC_INTERNAL_CONFIG_INSTANCESPECIFIERSET_H_
