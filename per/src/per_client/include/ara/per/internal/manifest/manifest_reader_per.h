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
/// @file       manifest_reader_per.h
/// @brief      AutoSar-Crypto configuration
/// @details    Configuration file reader used by Puhua persistence module.
/// @date       2022-12-13
/// @author     Han Jingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr><td>2022-12-13  <td>1.0.0    <td>Han Jingjing      <td>Refactor configuration file reading function of persistence module
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/per/Common/Configuration Management
/// @interface_level=module
/// @trace_id_sr=
/// @unit_name=
/// @unit_description=Configuration file reader used by Puhua persistence module.
/// @endcode
///
/// ================================================================

#ifndef ARA_PER_MANIFEST_MANIFEST_READER_H_
#define ARA_PER_MANIFEST_MANIFEST_READER_H_

#include <ara/core/instance_specifier.h>
#include <ara/core/map.h>
#include <ara/core/string.h>
#include <ara/core/string_view.h>
#include <isoft/manifestreader/manifest_node.h>
#include <isoft/manifestreader/manifest_reader.h>
#include <isoft/manifestreader/tps_enumeration.h>

#include <functional>

#include "ara/per/internal/manifest/manifest_struct.h"
#include "ara/per/internal/manifest/manifest_syntax.h"

namespace ara {
namespace per {
namespace manifest {
/// 2025-11-07 niuliming: Read String or array type fields in configuration file
/// @brief Read multi-type fields for persistence
/// @code{.isoft}
/// @interface_level=unit
/// @unit_name=MR_STRING_OR_VECTOR_FIELD
/// @endcode
#define MR_STRING_OR_VECTOR_FIELD(KEY, FIELDNAME)                                                                      \
    {                                                                                                                  \
        if (!v.HasMember(KEY)) {                                                                                       \
            return static_cast< int32_t >(ManifestReaderErrc::kKeyNotFound);                                           \
        }                                                                                                              \
        if (v[KEY].IsString()) {                                                                                       \
            ara::core::ErrorDomain::CodeType const err{                                                                \
                Helper< decltype(result.FIELDNAME) >::Load(result.FIELDNAME, v[KEY])};                                 \
            if (err != isoft::kSuccess) {                                                                              \
                return err;                                                                                            \
            }                                                                                                          \
        } else {                                                                                                       \
            ara::core::Vector< ara::core::String > vec;                                                                \
            ara::core::ErrorDomain::CodeType const err{Helper< decltype(vec) >::Load(vec, v[KEY])};                    \
            if (err != isoft::kSuccess) {                                                                              \
                return err;                                                                                            \
            }                                                                                                          \
            result.FIELDNAME = vec.front();                                                                            \
        }                                                                                                              \
    }
/// @brief Read persistence configuration file
/// @code{.isoft}
/// @unit_name=PManifestReader_Per
/// @endcode
class PManifestReader_Per final
{
public:
    /// @brief
    using CB_ScanStorage_Kv = std::function< bool(MConfig_StorageKv const& kvConfig) >;
    /// @brief
    using CB_ScanStorage_File = std::function< bool(MConfig_StorageFile const& fileConfig) >;
    /// @brief
    using CB_ScanInitFile = std::function< bool(MConfigData_InitFile const& initFile) >;
    /// @brief
    using CB_ScanElement = std::function< bool(MConfigData_KeySlotElement const& keySlotElement) >;

public:
    /// @brief Port to Storage mapping
    using MAP_PortToStorage = ara::core::Map< ara::core::String, MConfigData_PerPort >;
    /// @brief KvStorage configuration table
    using MAP_StorageKv = ara::core::Map< ara::core::String, MConfig_StorageKv >;
    /// @brief FileStorage configuration table
    using MAP_StorageFile = ara::core::Map< ara::core::String, MConfig_StorageFile >;
    /// @brief Mapping of key slots used by persistence to Storage
    using MAP_KeySlotStorage = ara::core::Map< ara::core::String, MConfigData_KeySlotStorage >;
    /// @brief Mapping of key slots used by persistence to Elements of Storage
    using MAP_KeySlotElement = ara::core::Map< ara::core::String, MConfigData_KeySlotElement >;

private:
    /// @brief ARA-VAR directory
    ara::core::String stAraVarPath_;
    /// @brief Directory where this configuration file resides
    ara::core::String stSelfEtcPath_;

private:
    /// @brief APP version number
    ara::core::String stVersionApp_;
    /// @brief Port to KvStorage mapping
    MAP_PortToStorage mapPortToStorageKv_;
    /// @brief Port to FileStorage mapping
    MAP_PortToStorage mapPortToStorageFile_;
    /// @brief KvStorage configuration table
    /// @name mapStorageKv_
    MAP_StorageKv mapStorageKv_;
    /// @brief FileStorage configuration
    /// @name mapStorageFile_
    MAP_StorageFile mapStorageFile_;
    /// @brief Mapping of key slots used by persistence to Storage
    MAP_KeySlotStorage mapKeySlotStorage_;
    /// @brief Mapping of key slots used by persistence to Elements of Storage
    MAP_KeySlotElement mapKeySlotElement_;

public:
    /// @brief Delete default constructor
    PManifestReader_Per() = delete;
    /// @brief Constructor
    /// @param stVarPath
    explicit PManifestReader_Per(ara::core::StringView const& stVarPath) noexcept;
    /// @brief Destructor
    ~PManifestReader_Per() noexcept = default;
    /// @brief
    /// @param a
    PManifestReader_Per(PManifestReader_Per const& a) = delete;
    /// @brief
    /// @param a
    /// @return
    PManifestReader_Per& operator=(PManifestReader_Per const& a) = delete;
    /// @brief
    /// @param a
    PManifestReader_Per(PManifestReader_Per&& a) = delete;
    /// @brief
    /// @param a
    /// @return
    PManifestReader_Per& operator=(PManifestReader_Per&& a) = delete;

public:
    /// @brief Initialize configuration file
    /// @param stConfigFile
    /// @return
    bool InitManifest(ara::core::StringView const& stConfigFile) noexcept;
    /// @brief Traverse each KvStorage configuration and execute callback function
    /// @param pfun Callback function
    /// @return Number of callbacks executed
    uint32_t ForEachStorage_Kv(CB_ScanStorage_Kv const& pfun) noexcept;
    /// @brief Traverse each FileStorage configuration and execute callback function
    /// @param pfun Callback function
    /// @return Number of callbacks executed
    uint32_t ForEachStorage_File(CB_ScanStorage_File const& pfun) noexcept;
    /// @brief Traverse each initialization file in specific FileStorage and execute callback function
    /// @param port FileStorage PortIns
    /// @param pfun Callback function
    /// @return Number of callbacks executed
    int32_t ForEachFileStorageInitFile(ara::core::String const& stPortIns, CB_ScanInitFile const& pfun) const noexcept;
    /// @brief Get name of KvStorage corresponding to port
    /// @param port
    /// @return
    ara::core::String GetStorageName_Kv(ara::core::String const& stPortIns) const noexcept;
    /// @brief Get name of FileStorage corresponding to port
    /// @param port
    /// @return
    ara::core::String GetStorageName_File(ara::core::String const& stPortIns) const noexcept;
    /// @brief Get initialization Uri of KvStorage
    /// @param port
    /// @return
    ara::core::String GetKvStorageUpdateFile(ara::core::String const& stPortIns) const noexcept;
    /// @brief Find KvStorage via PortIns
    /// @param portIns Port identifier
    /// @return Returns nullptr if not found
    MConfig_StorageKv const* FindStorageByPort_Kv(ara::core::String const& stPortIns) const noexcept;
    /// @brief Find FileStorage via PortIns
    /// @param portIns Port identifier
    /// @return Returns nullptr if not found
    MConfig_StorageFile const* FindStorageByPort_File(ara::core::String const& stPortIns) const noexcept;
    /// @brief Find KvStorage via PerIns
    /// @param stPerIns
    /// @return Returns nullptr if not found
    MConfig_StorageKv const* FindStorageByPer_Kv(ara::core::String const& stPerIns) const noexcept;
    /// @brief Find FileStorage via PerIns
    /// @param stPerIns
    /// @return Returns nullptr if not found
    MConfig_StorageFile const* FindStorageByPer_File(ara::core::String const& stPerIns) const noexcept;
    /// @brief Check if there is library encryption configuration via persistence library name
    /// @param stStorageName Library name
    /// @return
    MConfigData_KeySlotStorage const* FindKeySlot_Storage(ara::core::String const& stStorageName) const noexcept;
    /// @brief Check if there is library entry encryption configuration via persistence library name
    /// @param stElementName
    /// @return
    MConfigData_KeySlotElement const* FindKeySlot_Element(ara::core::String const& stElementName) const noexcept;
    /// @brief Traverse each KeySlotElement configuration and execute callback function
    /// @param stStorage
    /// @param pfun Callback function
    /// @return Number of callbacks executed
    uint32_t ForEachKeySlotElement(ara::core::String const& stStorage, CB_ScanElement const& pfun) const noexcept;
    /// @brief Find initialization information of file identified by stFileName within FileStorage corresponding to stPerIns
    /// @param stPerIns
    /// @param stFileName
    /// @return
    MConfigData_InitFile const* FindInitData_File(ara::core::String const& stPerIns,
                                                  ara::core::StringView const& stFileName) const noexcept;
    /// @brief Find initialization file name within KeyValueStorage corresponding to stPerIns
    /// @param stPerIns
    /// @return
    ara::core::String FindInitData_Kv(ara::core::String const& stPerIns) const noexcept;
    /// @brief Return APP version number
    /// @return
    ara::core::String const& GetVersionApp() const noexcept { return stVersionApp_; }
    /// @brief Delete KvStorage
    /// @param stStorageName Library name
    /// @return Success or failure
    bool DelStorage_Kv(ara::core::String const& stStorageName) noexcept;
    /// @brief Delete FileStorage
    /// @param stStorageName Library name
    /// @return Success or failure
    bool DelStorage_File(ara::core::String const& stStorageName) noexcept;

protected:
    /// @brief Find PortIns via StorageIns
    /// @param mapPortToStorage
    /// @param stStorageIns
    /// @return
    ara::core::String _FindPortInsByStorageIns(MAP_PortToStorage const& mapPortToStorage,
                                               ara::core::String const& stStorageIns);
    /// @brief Read KvPortMap configuration
    /// @param pManifest Configuration file interpreter
    /// @return
    uint32_t _ReadPortMap_Kv(isoft::manifestreader::Manifest const* const pManifest) noexcept;
    /// @brief Read FilePortMap configuration
    /// @param pManifest Configuration file interpreter
    /// @return
    uint32_t _ReadPortMap_File(isoft::manifestreader::Manifest const* const pManifest) noexcept;
    /// @brief Read KV library configuration
    /// @param pManifest Configuration file interpreter
    /// @return
    uint32_t _ReadStorageData_Kv(isoft::manifestreader::Manifest const* const pManifest) noexcept;
    /// @brief Read File library configuration
    /// @param pManifest Configuration file interpreter
    /// @return
    uint32_t _ReadStorageData_File(isoft::manifestreader::Manifest const* const pManifest) noexcept;
    /// @brief Read KeySlotStorage configuration
    /// @param pManifest Configuration file interpreter
    /// @return
    uint32_t _ReadKeySlot_Storage(isoft::manifestreader::Manifest const* const pManifest) noexcept;
    /// @brief Read KeySlotStorageElement configuration
    /// @param pManifest Configuration file interpreter
    /// @return
    uint32_t _ReadKeySlot_Element(isoft::manifestreader::Manifest const* const pManifest) noexcept;

public:
};

}  // namespace manifest
}  // namespace per
}  // namespace ara

#endif
