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
/// @file       manifest_instance.h
/// @brief      AutoSar-AP Data Persistence Storage Module
/// @details    Manifest operations related to ara/per
/// @date       2021-09-13
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/per/Common/Configuration Management
/// @interface_level=module
/// @trace_id_sr=
/// @unit_name=PManifestInstance
/// @unit_description=Manifest operations related to ara/per
/// @endcode
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// | 2021-09-13 | 1.0.0   |        | Initial version created                   |
///
/// ================================================================

#ifndef ARA_PER_MANIFEST_MANIFEST_INSTANCE_H_
#define ARA_PER_MANIFEST_MANIFEST_INSTANCE_H_

#include <memory>

#include "ara/per/internal/manifest/manifest_reader_per.h"

namespace ara {
namespace per {
namespace manifest {
//********************************/

/// @brief
class PManifestInstance final
{
public:
    /// @brief Storage type
    enum class EPerType : int32_t
    {
        kStorage_Any  = 0,
        kStorage_Kv   = 1,  // KV
        kStorage_File = 2,  // File
    };
    /// @brief
    /// @return
    static PManifestReader_Per* get() noexcept;  // NOLINT
    /// @brief
    /// @param stJsonConfig
    static void Initialize(ara::core::StringView const& stJsonConfig) noexcept;
    /// @brief
    static void DeInitialize() noexcept;
    /// @brief Find Kv library: Prioritize searching by Port identifier; if not found, convert to StorageIns and search again
    /// @param portIns Port identifier / KvStorage library identifier
    /// @return StorageKv configuration data
    static MConfig_StorageKv const* FindStorage_Kv(ara::core::InstanceSpecifier const& portIns) noexcept;
    /// @brief Find File library: Prioritize searching by Port identifier; if not found, convert to StorageIns and search again
    /// @param portIns Port identifier / KvStorage library identifier
    /// @return StorageFile configuration data
    static MConfig_StorageFile const* FindStorage_File(ara::core::InstanceSpecifier const& portIns) noexcept;
    /// @brief Find Kv/File library: Prioritize searching by Port identifier; if not found, convert to StorageIns and search again
    /// @param portIns Port identifier / KvStorage library identifier
    /// @param ePerType
    /// @return StorageFile configuration data
    static MConfig_StorageBase const* FindStorage(ara::core::InstanceSpecifier const& portIns,
                                                  EPerType const ePerType) noexcept;
    /// @brief Convert PortIns to StorageIns string
    /// @param portIns Port identifier / KvStorage library identifier
    /// @param ePerType
    /// @return
    static ara::core::String TransPortToStorageIns(ara::core::InstanceSpecifier const& portIns,
                                                   EPerType const ePerType) noexcept;
};
//********************************/
}  // namespace manifest
}  // namespace per
}  // namespace ara

#endif  // ARA_PER_MANIFEST_MANIFEST_INSTANCE_H_
