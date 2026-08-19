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
/// @file       isoft_per_common_api.cpp
/// @brief      AutoSar-AP
/// @details    Some common methods
/// @date       2021-07-14
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr><td>2021-07-14  <td>1.0.0    <td>hanjingjing      <td>Create initial version
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/per/Common/Reusable Functions
/// @interface_level=module
/// @trace_id_sr=
/// @unit_name=
/// @unit_description=
/// @endcode
///
/// ================================================================

#include "ara/per/internal/common/isoft_per_common_api.h"

namespace ara {
namespace per {
namespace isoftkv {
//********************************/
/// @brief Convert error code to PerErrc
/// @param eCode
/// @return
PerErrc TransErrorCode(ara::per::isoftkv::EErrorPHKV const eCode) noexcept
{
    PerErrc eReturn;
    switch (eCode) {
        case isoftkv::EErrorPHKV::kNullParam:  // Parameter passing error
        {
            eReturn = PerErrc::kNotInitialized;
        } break;
        case isoftkv::EErrorPHKV::kFileKvRead:      // File read error
        case isoftkv::EErrorPHKV::kFileKvOpt:       // File operation class error
        case isoftkv::EErrorPHKV::kFileKvWrite:     // File write error
        case isoftkv::EErrorPHKV::kFileScanAllOpt:  // Error occurred while traversing all FileOpt
        {
            eReturn = PerErrc::kPhOptKvFile;
        } break;
        case isoftkv::EErrorPHKV::kFileSize:  // File size error
        {
            eReturn = PerErrc::kInvalidSize;
        } break;
        case isoftkv::EErrorPHKV::kFileSpaceMax:  // File exceeds maximum limit
        {
            eReturn = PerErrc::kOutOfStorageSpace;
        } break;
        case isoftkv::EErrorPHKV::kFileUpdateOpen:  // Error opening Update file
        case isoftkv::EErrorPHKV::kFileUpdateRead:  // Error operating on update file
        {
            eReturn = PerErrc::kPhOptUpdateFile;
        } break;
        case isoftkv::EErrorPHKV::kFileVersion:  // File version number error
        {
            eReturn = PerErrc::kPhFileVersion;
        } break;
        case isoftkv::EErrorPHKV::kFileRecoverRedd:  // Error recovering redundant file
        {
            eReturn = PerErrc::kValidationFailed;
        } break;
        case isoftkv::EErrorPHKV::kFileCryptoConfig:  // File Crypto configuration Hash check error
        {
            eReturn = PerErrc::kAuthenticationFailed;
        } break;
        case isoftkv::EErrorPHKV::kNotReadySystem:  // Not successful: InitEmptySystem
        case isoftkv::EErrorPHKV::kNotReadyCache:   // Not ready: CacheManager
        case isoftkv::EErrorPHKV::
            kNotReadyPage:  // Not ready: Page not ready for reading/writing (header not initialized, etc.)
        case isoftkv::EErrorPHKV::kNotReadyPageGroup:  // Not ready: PageGroup
        case isoftkv::EErrorPHKV::kNotReadyPageBoss:   // Not ready: PageBoss
        case isoftkv::EErrorPHKV::kNotReadyPageHash:   // Not ready: PageHash
        case isoftkv::EErrorPHKV::kNotReadyPageKv:     // Not ready: PageKv
        case isoftkv::EErrorPHKV::kNotReadyPageValue:  // Not ready: PageValue
        case isoftkv::EErrorPHKV::kNotReadyPageWalog:  // Not ready: PageWalog
        case isoftkv::EErrorPHKV::kNotReadyPageExt:    // Not ready: PageExt
        case isoftkv::EErrorPHKV::kPageInit:           // Page initialization error
        {
            eReturn = PerErrc::kPhKvLogicInit;
        } break;
        case isoftkv::EErrorPHKV::kPageFindFree:       // Error finding free page
        case isoftkv::EErrorPHKV::kPageCacheCapacity:  // Cache page capacity overflow
        case isoftkv::EErrorPHKV::kPageCacheLoad:      // Error loading page
        case isoftkv::EErrorPHKV::kPageCacheSave:      // Error saving cache page back to Walog library
        {
            eReturn = PerErrc::kPhKvLogicCache;
        } break;
        case isoftkv::EErrorPHKV::kPageLackSpace:     // Insufficient remaining page space
        case isoftkv::EErrorPHKV::kPageNotFindPage:   // Page not found
        case isoftkv::EErrorPHKV::kPageNotFindBlock:  // Block corresponding to BlockID not found
        case isoftkv::EErrorPHKV::kPageSplitHash:     // Error splitting Hash page
        case isoftkv::EErrorPHKV::kPageAddBlock:      // Error adding BlockData
        case isoftkv::EErrorPHKV::kPageAmendBlock:    // Error modifying BlockData
        case isoftkv::EErrorPHKV::kPageSoloBoss:      // Error during SoloBossPage
        {
            eReturn = PerErrc::kIntegrityCorrupted;
        } break;
        case isoftkv::EErrorPHKV::kPageNewPage:      // Error creating empty Page
        case isoftkv::EErrorPHKV::kPageMoveToWalog:  // Error moving Main page to Walog
        {
            eReturn = PerErrc::kPhysicalStorageFailure;
        } break;
        case isoftkv::EErrorPHKV::kPageReadRedd:  // Error reading redundant page from disk
        case isoftkv::EErrorPHKV::kErrorKvCRC16:  // KV-Element CRC16 check error
        {
            eReturn = PerErrc::kValidationFailed;
        } break;
        case isoftkv::EErrorPHKV::kPageWalogTailData:  // Tail identification information when Walog is saved completely
        {
            eReturn = PerErrc::kPhErrorCode;  // This value is subject to discussion
        } break;
        case isoftkv::EErrorPHKV::kPageExtLenLimit:  // Ext page maximum length exceeded // NOLINT
        {
            eReturn = PerErrc::kIntegrityCorrupted;  // Unable to establish structural integrity of storage.
        } break;
        case isoftkv::EErrorPHKV::kPageGroupVersion:  // Error setting version number in Group page
        case isoftkv::EErrorPHKV::kErrorPageCRC32:    // Page self CRC32 check error
        case isoftkv::EErrorPHKV::kErrorBlockID:      // BlockID error: Main ID
        case isoftkv::EErrorPHKV::kErrorPageID:       // Invalid page ID
        case isoftkv::EErrorPHKV::kErrorWalogID:      // Invalid WalogID
        case isoftkv::EErrorPHKV::kErrorBossID:       // Invalid BossID
        case isoftkv::EErrorPHKV::kIndexGroup:        // Index error: Group array
        case isoftkv::EErrorPHKV::kIndexBoss:         // Index error: PageFree array
        case isoftkv::EErrorPHKV::kIndexHash:         // Index error: Hash array
        case isoftkv::EErrorPHKV::kIndexWalog:        // Index error: Walog array
        case isoftkv::EErrorPHKV::kIndexKvArray:      // Index error: Kv array
        {
            eReturn = PerErrc::kIntegrityCorrupted;
        } break;
        case isoftkv::EErrorPHKV::kKvElementWrite:  // Error writing KV-Element
        case isoftkv::EErrorPHKV::kKvElementRead:   // Error reading KV-Element
        case isoftkv::EErrorPHKV::kKvElementValue:  // Invalid BlockIndex data
        {
            eReturn = PerErrc::kIntegrityCorrupted;
        } break;
        case isoftkv::EErrorPHKV::kKvElementNotFind:  // Corresponding KV-Element not found
        {
            eReturn = PerErrc::kKeyNotFound;
        } break;
        case isoftkv::EErrorPHKV::kKvBuffCapacity:  // Provided Buff capacity insufficient
        {
            eReturn = PerErrc::kPhKvReadBuffCapacity;
        } break;
        case isoftkv::EErrorPHKV::kKvDataType:  // KV read data type error
        {
            eReturn = PerErrc::kDataTypeMismatch;
        } break;
        case isoftkv::EErrorPHKV::kKvElementCrypot:  // Kv data encrypt/decrypt error
        {
            eReturn = PerErrc::kEncryptionFailed;
        } break;
        case isoftkv::EErrorPHKV::kKvReddCheck:  // KV data redundancy check failed // NOLINT
        {
            eReturn = PerErrc::kIntegrityCorrupted;
        } break;
        case isoftkv::EErrorPHKV::kReddCrypto:  // Redundancy encryption algorithm calculation error
        {
            eReturn = PerErrc::kIntegrityCorrupted;
        } break;
        default: {
            eReturn = PerErrc::kPhErrorCode;  // Arbitrarily written value on 2023-02-28, subject to discussion
        } break;
    }
    return eReturn;
}
/// @brief Compare Hash values in Vector format and String format
/// @param vecHash
/// @param stHash
/// @return Whether equal
bool CompareHashData(ara::core::Vector< uint8_t > const& vecHash, ara::core::String const& stHash) noexcept
{
    // Compare file Hash
    int32_t const nHashLen{static_cast< int32_t >(vecHash.size())};
    ara::core::String stFileHash;
    stFileHash.reserve(static_cast< std::size_t >(nHashLen) * kInt_2U);
    for (int32_t i{0}; i < nHashLen; i++) {
        int32_t const nData{static_cast< int32_t >(vecHash[static_cast< std::size_t >(i)])};
        stFileHash += T_itochar(nData / kInt_0x10);
        stFileHash += T_itochar(nData % kInt_0x10);
    }
    if (stFileHash != stHash) {
        return false;
    }
    return true;
}
//********************************/
}  // namespace isoftkv
}  // namespace per
}  // namespace ara
