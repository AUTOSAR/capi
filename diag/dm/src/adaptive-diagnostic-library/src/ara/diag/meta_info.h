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
/// @file       meta_info.h
/// @brief      This file provides the definitions of MetaInfo and related types.
/// @details
/// @date       2021-11-23
/// @author     jiawei
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_DIAG_META_INFO_H_
#define ARA_DIAG_META_INFO_H_

#include <ara/core/map.h>
#include <ara/core/optional.h>
#include <ara/core/string.h>
#include <ara/core/string_view.h>

#include <cstdint>
#include <utility>

namespace ara {
namespace diag {
/// @brief Declare MetaInfo
class MetaInfo;

namespace api {
/// @brief Construct MetaInfo object
/// @param metaInfo
/// @return
/// @throws on overflow
MetaInfo CreateMetaInfo(ara::core::Map< ara::core::String, ara::core::String > const& metaInfo);
}  // namespace api

/// @brief Metainfo interface
///
/// @code{.isoft}
/// export_level=/Diagnostics
/// @endcode
///
/// @traceid{SWS_DM_00971}@tracestatus{draft}
class MetaInfo final
{
public:
    /// @brief Constructor of MetaInfo cannot be used
    ///
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    ///
    /// @traceid{SWS_DM_00972}@tracestatus{draft}
    MetaInfo() noexcept = delete;

    /// @brief Copy Constructor of MetaInfo cannot be used
    ///
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    ///
    /// @traceid{SWS_DM_00973}@tracestatus{draft}
    MetaInfo(MetaInfo const&) = delete;

    /// @brief Move Constructor of MetaInfo
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] obj object to be moved
    ///
    ///
    /// @traceid{SWS_DM_00974}@tracestatus{draft}
    MetaInfo(MetaInfo&& obj) noexcept : data_{std::move(obj.data_)} {}

    /// @brief Copy Assignment Operator of MetaInfo cannot be used
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @return MetaInfo&
    ///
    ///
    /// @traceid{SWS_DM_00975}@tracestatus{draft}
    MetaInfo& operator=(MetaInfo const&) = delete;

    /// @brief Move Assignment Operator of MetaInfo
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] other MetaInfo instance
    /// @return Reference to the current object
    ///
    ///
    /// @traceid{SWS_DM_00976}@tracestatus{draft}
    MetaInfo& operator=(MetaInfo&& other) & noexcept
    {
        if (this != &other) {
            data_ = std::move(other.data_);
        }
        return *this;
    }

    /// @brief Definition of possible call context
    ///
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    ///
    /// @traceid{SWS_DM_00977}@tracestatus{draft}
    enum class Context : std::uint32_t
    {
        kDiagnosticCommunication,  ///< service request in DCM context
        kFaultMemory,              ///< for DIDs in Snapshots
        kDoIP                      ///< for reading VIN
    };

    /// @brief Get the metainfo value for a given key
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @param[in] key identification of value to be returned
    /// @return Returns value for the given key.
    ///
    ///
    /// @traceid{SWS_DM_00978}@tracestatus{draft}
    ara::core::Optional< ara::core::StringView > GetValue(ara::core::StringView key) const noexcept;

    /// @brief Get the context of the invocation
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    /// @return Returns the context.
    ///
    ///
    /// @traceid{SWS_DM_00979}@tracestatus{draft}
    Context GetContext() const noexcept;

    /// @brief Default destructor
    ///
    /// @code{.isoft}
    /// export_level=/Diagnostics
    /// @endcode
    ///
    /// @traceid{SWS_DM_00980}@tracestatus{draft}
    ~MetaInfo() noexcept = default;

private:
    ara::core::Map< ara::core::String, ara::core::String >
        /// @name data_
        data_;
    /// @brief
    /// @param mapping
    /// @return
    /// @throws on overflow
    friend MetaInfo api::CreateMetaInfo(ara::core::Map< ara::core::String, ara::core::String > const& mapping);
    /// @brief
    /// @param data
    /// @throws on overflow
    explicit MetaInfo(ara::core::Map< ara::core::String, ara::core::String > data) : data_{std::move(data)} {}
};
}  // namespace diag
}  // namespace ara

#endif  // ARA_DIAG_META_INFO_H_