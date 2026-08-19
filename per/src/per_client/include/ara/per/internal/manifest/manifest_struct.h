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
/// @file       manifest_struct.h
/// @brief      AutoSar-AP data persistence storage module
/// @details    Parsed data structure of Manifest.Json file used by Puhua persistence module
/// @date       2021-10-09
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr><td>2021-10-09  <td>1.0.0    <td>hanjingjing      <td>Create initial version
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/per/Common/Configuration Management
/// @interface_level=module
/// @trace_id_sr=
/// @unit_name=
/// @unit_description=
/// @endcode
///
/// ================================================================

#ifndef ARA_PER_MANIFEST_MANIFEST_STRUCT_H_
#define ARA_PER_MANIFEST_MANIFEST_STRUCT_H_
#include "ara/core/string.h"
#include "ara/core/string_view.h"
#include "ara/core/vector.h"

namespace ara {
namespace per {
namespace manifest {
//********************************/
/// @brief CRC redundancy
/// @code{.isoft}
/// @unit_name=MConfigData_ReddCrc
/// @interface_level=unit
/// @endcode
struct MConfigData_ReddCrc final
{
public:
    /// @brief CRC algorithm
    ara::core::String algorithmFamily;
    /// @brief CRC length
    std::uint32_t length;
};
/// @brief M/N redundancy
/// @code{.isoft}
/// @unit_name=MConfigData_ReddMofN
/// @interface_level=unit
/// @endcode
struct MConfigData_ReddMofN final
{
public:
    /// @brief
    std::uint32_t m;
    /// @brief N should be bigger than M
    std::uint32_t n;
};
/// @brief Hash redundancy
/// @code{.isoft}
/// @unit_name=MConfigData_ReddHash
/// @interface_level=unit
/// @endcode
struct MConfigData_ReddHash final
{
public:
    /// @brief Hash algorithm
    ara::core::String algorithmFamily;
    /// @brief Hash length
    std::uint32_t length;
    /// @brief Hash initialization vector length
    std::uint32_t nInitVectorLen;
};
/// @brief Redundancy configuration
/// @code{.isoft}
/// @unit_name=MConfigData_Redundancy
/// @endcode
struct MConfigData_Redundancy final
{
public:
    /// uint32_t    nScope { 0 };
    // Scope of redundancy configuration deployment: 0 corresponds to ScopeElement, 1 corresponds to ScopeStorage
    // Seems duplicated with MConfigData_StorageBase.stReddStrategy as of 2021-11-03
    /// @brief M-N redundancy
    ara::core::Vector< MConfigData_ReddMofN > vecMofN;
    /// @brief CRC redundancy
    ara::core::Vector< MConfigData_ReddCrc > vecCrc;
    /// @brief Hash redundancy
    ara::core::Vector< MConfigData_ReddHash > vecHash;
};
//********************************/
/// @brief Persistence common data
/// @code{.isoft}
/// @unit_name=MConfigData_StorageBase
/// @interface_level=unit
/// @endcode
struct MConfigData_StorageBase final
{
public:
    /// @brief Storage name
    ara::core::String stName;
    /// @brief Version number of persistent data
    ara::core::String stVersionPer;
    /// @brief Storage directory
    ara::core::String stPath;
    /// @brief Minimum occupied space
    uint64_t nSizeMin{0U};
    /// @brief Maximum occupied space
    uint64_t nSizeMax{0U};
    /// @brief Update strategy
    ara::core::String stUpdateStrategy;
    /// @brief Redundancy strategy
    ara::core::String stReddStrategy;
    /// @brief Redundancy settings
    MConfigData_Redundancy reddConfig;
    /// @brief
};
/// @brief KvStorage KV library configuration (contains "unique parameters" for initializing different KV libraries: initial page size, etc.)
/// @code{.isoft}
/// @unit_name=MConfig_StorageBase
/// @endcode
class MConfig_StorageBase
{
public:
    /// @brief Identifier: Port
    ara::core::String stPortIns;
    /// @brief Identifier: Storage
    ara::core::String stStorageIns;
    /// @brief
    MConfigData_StorageBase dataStorage;

public:
    /// @brief
    /// @throws
    MConfig_StorageBase() = default;
    /// @brief
    virtual ~MConfig_StorageBase() = default;
    /// @brief
    /// @param a
    /// @throws
    MConfig_StorageBase(MConfig_StorageBase const &a) = default;
    /// @brief
    /// @param a
    /// @return
    /// @throws
    MConfig_StorageBase &operator=(MConfig_StorageBase const &a) = default;
    /// @brief
    /// @param a
    MConfig_StorageBase(MConfig_StorageBase &&a) = delete;
    /// @brief
    /// @param a
    /// @return
    MConfig_StorageBase &operator=(MConfig_StorageBase &&a) = delete;
};
//***************/
/// @brief KvStorage KV library configuration (contains "unique parameters" for initializing different KV libraries: initial page size, etc.)
/// @code{.isoft}
/// @unit_name=MConfigData_StorageKv
/// @interface_level=unit
/// @endcode
struct MConfigData_StorageKv final
{
public:
    /// @brief
    ara::core::String stInitFile;
    /// @brief Initial Cache page count
    uint32_t nMaxCacheCount{-1U};
    /// @brief Initial Page count
    uint32_t nInitPageCount{-1U};
    /// @brief Single page length
    uint32_t nPageInitLen{-1U};
    /// @brief Free list classification count
    uint32_t nFreeListCapacity{-1U};
    /// @brief Group size, maximum number of pages per group
    uint32_t nMaxPageInGroup{-1U};
    /// @brief Initial Hash bucket count
    uint32_t nHashInitCount{-1U};
    /// @brief Maximum Hash bucket count
    uint32_t nHashMaxCapacity{-1U};
    /// @brief Whether to automatically save Walog library
    uint32_t nAutoSaveWalog{-1U};
    /// @brief Maximum allowed KV data length: Default is 64 bytes, maximum is half of nPageInitLen
    uint32_t nMaxKvDataLen{-1U};
};
/// @brief KvStorage KV library configuration (contains "unique parameters" for initializing different KV libraries: initial page size, etc.)
/// @code{.isoft}
/// @unit_name=MConfig_StorageKv
/// @endcode
class MConfig_StorageKv final : public MConfig_StorageBase
{
public:
    /// @brief KvStorage library configuration
    MConfigData_StorageKv dataKv;

public:
    /// @brief
    /// @throws
    MConfig_StorageKv() = default;
    /// @brief
    ~MConfig_StorageKv() final = default;
    /// @brief
    /// @param a
    /// @throws
    MConfig_StorageKv(MConfig_StorageKv const &a) = default;
    /// @brief
    /// @param a
    /// @return
    /// @throws
    MConfig_StorageKv &operator=(MConfig_StorageKv const &a) = default;
    /// @brief
    /// @param a
    MConfig_StorageKv(MConfig_StorageKv &&a) = delete;
    /// @brief
    /// @param a
    /// @return
    MConfig_StorageKv &operator=(MConfig_StorageKv &&a) = delete;
};
//***************/
/// @brief FileStorage initialization file information
/// @code{.isoft}
/// @unit_name=MConfigData_InitFile
/// @endcode
struct MConfigData_InitFile final
{
public:
    /// @brief
    ara::core::String stUpdateType;
    /// @brief
    ara::core::String stFileUri;
    /// @brief
    ara::core::String stFileName;
    /// @brief Redundancy settings
    MConfigData_Redundancy reddConfig;
};
/// @brief FileStorage File library configuration
/// @code{.isoft}
/// @unit_name=MConfigData_StorageFile
/// @interface_level=unit
/// @endcode
struct MConfigData_StorageFile final
{
public:
    /// @brief Maximum file handle count
    uint32_t nMaxFileCount;
    /// @brief Initialization file list
    ara::core::Vector< MConfigData_InitFile > vecInitFile;
};
/// @brief FileStorage File library configuration
/// @code{.isoft}
/// @unit_name=MConfig_StorageFile
/// @endcode
class MConfig_StorageFile final : public MConfig_StorageBase
{
public:
    /// @brief FileStorage library configuration
    MConfigData_StorageFile dataFile;

public:
    /// @brief
    MConfig_StorageFile() = default;
    /// @brief
    ~MConfig_StorageFile() final = default;
    /// @brief
    /// @param a
    /// @throws
    MConfig_StorageFile(MConfig_StorageFile const &a) = default;
    /// @brief
    /// @param a
    /// @return
    /// @throws
    MConfig_StorageFile &operator=(MConfig_StorageFile const &a) = default;
    /// @brief
    /// @param a
    MConfig_StorageFile(MConfig_StorageFile &&a) = delete;
    /// @brief
    /// @param a
    /// @return
    MConfig_StorageFile &operator=(MConfig_StorageFile &&a) = delete;
};
//********************************/
/// @brief Port information
/// @code{.isoft}
/// @unit_name=MConfigData_PerPort
/// @interface_level=unit
/// @endcode
struct MConfigData_PerPort final
{
public:
    /// @brief Port identifier
    ara::core::String stPotIns;
    /// @brief Port type
    ara::core::String stPortType;
    /// @brief Storage identifier
    ara::core::String stStorageIns;
};
//********************************/
/// @brief Encryption key slot configuration used by persistence: Library body
/// @code{.isoft}
/// @unit_name=MConfigData_KeySlotStorage
/// @interface_level=unit
/// @endcode
struct MConfigData_KeySlotStorage final
{
public:
    /// @brief Key slot identifier
    ara::core::String stKeySlotIns;
    /// @brief Encryption algorithm description string in 2211
    ara::core::String stCryptoAlgorithm;
    /// @brief Key slot usage scope
    ara::core::String stKeySlotUsage;
    /// @brief Verification hash value
    ara::core::String stVerificationHash;
    /// @brief Persistence library or persistence entry identifier: persistencyDeployment
    ara::core::String stStorageName;
};
/// @brief Encryption key slot configuration used by persistence: Library member object
/// @code{.isoft}
/// @unit_name=MConfigData_KeySlotElement
/// @interface_level=unit
/// @endcode
struct MConfigData_KeySlotElement final
{
public:
    /// @brief Key slot identifier
    ara::core::String stKeySlotIns;
    /// @brief Encryption algorithm description string in 2211
    ara::core::String stCryptoAlgorithm;
    /// @brief Key slot usage scope
    ara::core::String stKeySlotUsage;
    /// @brief Verification hash value
    ara::core::String stVerificationHash;
    /// @brief Persistence library or persistence entry identifier: persistencyDeployment
    ara::core::String stStorageName;
    /// @brief Persistence library or persistence entry identifier: persistencyDeploymentElement
    ara::core::String stElementName;
};
//********************************/
}  // namespace manifest
}  // namespace per
}  // namespace ara

#endif
