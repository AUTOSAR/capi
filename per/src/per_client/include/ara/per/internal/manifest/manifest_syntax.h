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
/// @file       manifest_syntax.h
/// @brief      AutoSar-AP data persistence storage module
/// @details    Manifest syntax format involved in ara/per
/// @date       2021-09-13
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr><td>2021-09-13  <td>1.0.0    <td>      <td>Create initial version
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

#ifndef ARA_PER_MANIFEST_MANIFEST_SYNTAX_H_
#define ARA_PER_MANIFEST_MANIFEST_SYNTAX_H_

#include <ara/core/string_view.h>

#include "ara/per/internal/common/isoft_data_type.h"
namespace ara {
namespace per {
namespace manifest {
//********************************/
/// @brief APP version number
constexpr char8_t kVersionApp[]{"versionApp"};
/// @brief Configuration file name
/// @return
constexpr ara::core::StringView kManifestFileName{"./etc/storage_manifest.json"};
// JSon file, File/KV mapping sequence
/// @brief
/// @return
constexpr ara::core::StringView kKeyValueStorageSet{"KeyValueStorageSet"};
/// @brief
/// @return
constexpr ara::core::StringView kFileStorageSet{"FileStorageSet"};
/// @brief
/// @return
constexpr ara::core::StringView kMapPortToKvStorage{"PersistencyPortPrototypeToKeyValueStorageMapping"};
/// @brief
/// @return
constexpr ara::core::StringView kMapPortToFileStorage{"PersistencyPortPrototypeToFileStorageMapping"};

/// @brief
/// @return
constexpr ara::core::StringView kMapKeySlotToStorage{"PersistencyDeploymentToCryptoKeySlotMapping"};

/// @brief
/// @return
constexpr ara::core::StringView kMapKeySlotToStorageElement{"PersistencyDeploymentElementToCryptoKeySlotMapping"};
// Port attributes
constexpr char8_t kPortIns[]{"portIns"};
constexpr char8_t kPortType[]{"portType"};
constexpr char8_t kStorageIns[]{"storageIns"};
constexpr char8_t kPersistencyPortReadOnly[]{"RPort"};
// Storage common attributes
constexpr char8_t kStorageProperty[]{"storage"};
constexpr char8_t kStorageName[]{"name"};
constexpr char8_t kVersionPer[]{"versionPer"};
constexpr char8_t kStoragePath[]{"uri"};
constexpr char8_t kStorageMinSize[]{"minimumSustainedSize"};
constexpr char8_t kStorageMaxSize[]{"maximumAllowedSize"};
constexpr char8_t kStorageUpdateStrategy[]{"updateStrategy"};
constexpr char8_t kStorageReddStrategy[]{"redundancy"};
constexpr char8_t kStorageRedundancy[]{"redundancyHandling"};
// FileStorage attributes
constexpr char8_t kFileStorage[]{"fileStorage"};
constexpr char8_t kFileStorageMaxFiles[]{"maxNumberOfFiles"};
constexpr char8_t kFileStorageFileList[]{"initFileList"};
constexpr char8_t kInitFileUpdateType[]{"updateStrategy"};
constexpr char8_t kInitFileUri[]{"contentUri"};
constexpr char8_t kInitFileName[]{"fileName"};
// KvStorage attributes
constexpr char8_t kKvStorage[]{"kvStorage"};
constexpr char8_t kKvStorageInitKvList[]{"initKvPairUri"};
// KvSystem attributes
constexpr char8_t kDebugSerialization[]{"debug"};
constexpr char8_t kMaxCacheCount[]{"nMaxCacheCount"};        // Initial Cache page count
constexpr char8_t kInitPageCount[]{"nInitPageCount"};        // Initial Page count
constexpr char8_t kPageInitLen[]{"nPageInitLen"};            // Single page length
constexpr char8_t kFreeListCapacity[]{"nFreeListCapacity"};  // Free list classification count
constexpr char8_t kMaxPageInGroup[]{"nMaxPageInGroup"};      // Group size, maximum number of pages per group
constexpr char8_t kHashInitCount[]{"nHashInitCount"};        // Initial Hash bucket count
constexpr char8_t kHashMaxCapacity[]{"nHashMaxCapacity"};    // Maximum Hash bucket count
constexpr char8_t kAutoSaveWalog[]{"nAutoSaveWalog"};        // Whether to automatically save Walog library
constexpr char8_t kMaxKvDataLen[]{"nMaxKvDataLen"};          // Whether to automatically save Walog library
// Update strategy values  //DELETE、KEEP-EXISTING
constexpr ara::core::StringView kUpdateDataKeepExist{"KEEP-EXISTING"};
constexpr ara::core::StringView kUpdateDataOverWrite{"OVERWRITE"};
constexpr ara::core::StringView kUpdateDataDelete{"DELETE"};
// Redundancy strategy values  //NONE、REDUNDANT、REDUNDANT-PER-ELEMENT
constexpr ara::core::StringView kRedundancyDataNone{"none"};
constexpr ara::core::StringView kRedundancyDataRedundant{"redundant"};
constexpr ara::core::StringView kRedundancyDataPerElement{"redundantPerElement"};
// Redundancy configuration M-N redundancy
constexpr char8_t kRedundancyTag_MofN[]{"PersistencyRedundancyMOutOfN"};
constexpr char8_t kRedundancyTag_M[]{"m"};
constexpr char8_t kRedundancyTag_N[]{"n"};
// Redundancy configuration CRC check
constexpr char8_t kRedundancyTag_Crc[]{"PersistencyRedundancyCrc"};
constexpr char8_t kRedundancyTag_AlgorithmFamily[]{"algorithmFamily"};
constexpr char8_t kRedundancyTag_AlgorithmLength[]{"length"};
// Redundancy configuration Hash check
constexpr char8_t kRedundancyTag_Hash[]{"PersistencyRedundancyHash"};
constexpr char8_t kRedundancyTag_HashInitVecLen[]{"initVectorLength"};
// Persistence key slot configuration
constexpr char8_t kKeySlotIns[]{"keySlotIns"};
constexpr char8_t kCryptoAlgorithm[]{"cryptoAlgorithmString"};
constexpr char8_t kKeySlotUsage[]{"keySlotUsage"};
constexpr char8_t kKeySlotUsage_encryption[]{"encryption"};
constexpr char8_t kKeySlotUsage_verification[]{"verification"};
constexpr char8_t kVerificationHash[]{"verificationHash"};
constexpr char8_t kPerDeployment[]{"storageIns"};
constexpr char8_t kPerDeploymentElement[]{"elementIns"};
//********************************/
}  // namespace manifest
}  // namespace per
}  // namespace ara

#endif  // ARA_PER_MANIFEST_MANIFEST_SYNTAX_H_
