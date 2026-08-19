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
/// @file       kv_error_code.h
/// @brief      AutoSar-AP data persistence storage module
/// @details    Error codes
/// @date       2021-06-09
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr><td>2021-06-09  <td>1.0.0    <td>hanjingjing      <td>Create initial version
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/per/Common/Error Handling
/// @interface_level=module
/// @trace_id_sr=
/// @unit_name=
/// @unit_description=
/// @endcode
///
/// ================================================================

#ifndef ARA_PER_PHKV_KV_ERROR_CODE_H_
#define ARA_PER_PHKV_KV_ERROR_CODE_H_

#include <ara/log/common.h>

#include "ara/per/internal/common/isoft_assert.h"
#include "ara/per/internal/isoftkv/kv_data_type.h"

//********************************/
#ifdef ARA_PER_DEBUG
    #define PH_DEBUG(exp)                                                                                              \
        {                                                                                                              \
            bool const bDebug{exp};                                                                                    \
            if (true == bDebug) {                                                                                      \
                std::ignore = printf("<PH_DEBUG> : %s:%d\n", __FILE__, __LINE__);                                      \
                std::ignore = T_Void(1);                                                                               \
            }                                                                                                          \
        }
    #define PER_OnOptFailed(eErrorCode) _OnOptFailed(__FILE__, __LINE__, eErrorCode, ara::log::LogLevel::kError)
    #define PH_Warning(eErrorCode)      PKvErrorCode::OnOptFailed(__FILE__, __LINE__, eErrorCode, ara::log::LogLevel::kWarn);
    #define PH_Error(eErrorCode)        PKvErrorCode::OnOptFailed(__FILE__, __LINE__, eErrorCode, ara::log::LogLevel::kError)
    #define PH_CheckParam(pBData, nDataLen, nReturn)                                                                   \
        if (false == PKvErrorCode::CheckParam(pBData, nDataLen)) {                                                     \
            PKvErrorCode::OnOptFailed(__FILE__, __LINE__, EErrorPHKV::kNullParam);                                     \
            return nReturn;                                                                                            \
        }                                                                                                              \
        std::ignore = T_Void(0)
    #define PH_CheckDataLen(nDataLen, nReturn)                                                                         \
        if (false == PKvErrorCode::CheckParam(nDataLen)) {                                                             \
            PKvErrorCode::OnOptFailed(__FILE__, __LINE__, EErrorPHKV::kNullParam);                                     \
            return nReturn;                                                                                            \
        }                                                                                                              \
        std::ignore = T_Void(0)
#else
    #define PH_DEBUG(exp)
    #define PER_OnOptFailed(eErrorCode) _OnOptFailed(eErrorCode)
    #define PH_Warning(eErrorCode)      _OnOptFailed(eErrorCode);
    #define PH_Error(eErrorCode)        _OnOptFailed(eErrorCode)
    #define PH_CheckParam(pBData, nDataLen, nReturn)                                                                   \
        if (false == PKvErrorCode::CheckParam(pBData, nDataLen)) {                                                     \
            return nReturn;                                                                                            \
        }                                                                                                              \
        std::ignore = T_Void(0)
    #define PH_CheckDataLen(nDataLen, nReturn)                                                                         \
        if (false == PKvErrorCode::CheckParam(nDataLen)) {                                                             \
            return nReturn;                                                                                            \
        }                                                                                                              \
        std::ignore = T_Void(0)
#endif
//********************************/
namespace ara {
namespace per {
namespace isoftkv {
//********************************/
/// @brief Error codes
/// @code{.isoft}
/// @unit_name=EErrorPHKV
/// @endcode
enum class EErrorPHKV : int32_t
{
    kSuccess          = 0,  // Success
    kNullParam        = 1,  // Parameter passing error
    kFileError        = 10,
    kFileKvRead       = kFileError + 1,      // File read error
    kFileKvOpt        = kFileError + 2,      // File operation class error
    kFileKvWrite      = kFileError + 3,      // File write error
    kFileSize         = kFileError + 4,      // File size error
    kFileScanAllOpt   = kFileError + 5,      // Error occurred while traversing all FileOpt
    kFileSpaceMax     = kFileError + 6,      // File exceeds maximum limit
    kFileUpdateOpen   = kFileError + 7,      // Error opening Update file
    kFileUpdateRead   = kFileError + 8,      // Error operating on update file
    kFileVersion      = kFileError + 9,      // File version number error
    kFileRecoverRedd  = kFileError + 10,     // Error recovering redundant file
    kFileCryptoConfig = kFileError + 11,     // File Crypto configuration Hash check error
    kNotReadyError    = 100,                 // Not successful:
    kNotReadySystem   = kNotReadyError + 1,  // Not successful: InitEmptySystem
    kNotReadyCache    = kNotReadyError + 2,  // Not ready: CacheManager
    kNotReadyPage = kNotReadyError + 3,  // Not ready: Page not ready for reading/writing (header not initialized, etc.)
    kNotReadyPageGroup = kNotReadyError + 4,   // Not ready: PageGroup
    kNotReadyPageBoss  = kNotReadyError + 5,   // Not ready: PageBoss
    kNotReadyPageHash  = kNotReadyError + 6,   // Not ready: PageHash
    kNotReadyPageKv    = kNotReadyError + 7,   // Not ready: PageKv
    kNotReadyPageValue = kNotReadyError + 8,   // Not ready: PageValue
    kNotReadyPageWalog = kNotReadyError + 9,   // Not ready: PageWalog
    kNotReadyPageExt   = kNotReadyError + 10,  // Not ready: PageExt
    kPageError         = 200,                  // Page error
    kPageInit          = kPageError + 1,       // Page initialization error
    kPageFindFree      = kPageError + 2,       // Error finding free page
    kPageCacheCapacity = kPageError + 3,       // Cache page capacity overflow
    kPageCacheLoad     = kPageError + 4,       // Error loading page
    kPageCacheSave     = kPageError + 5,       // Error saving cache page back to Walog library
    kPageLackSpace     = kPageError + 6,       // Insufficient remaining page space
    kPageNotFindPage   = kPageError + 7,       // Page not found
    kPageNotFindBlock  = kPageError + 8,       // BlockID corresponding Block not found
    kPageSplitHash     = kPageError + 9,       // Error splitting Hash page
    kPageAddBlock      = kPageError + 10,      // Error adding BlockData
    kPageAmendBlock    = kPageError + 11,      // Error modifying BlockData
    kPageSoloBoss      = kPageError + 12,      // Error during SoloBossPage
    kPageNewPage       = kPageError + 13,      // Error creating empty Page
    kPageMoveToWalog   = kPageError + 14,      // Error moving Main page to Walog
    kPageReadRedd      = kPageError + 15,      // Error reading redundant page from disk
    kPageWalogTailData = kPageError + 16,      // Tail identification information when Walog is saved completely
    kPageExtLenLimit   = kPageError + 17,      // Ext page maximum length exceeded
    kPageGroupVersion  = kPageError + 18,      // Error setting version number in Group page
    kKvDataError       = 400,                  // Error inside KV page
    kErrorKvCRC16      = kKvDataError + 1,     // KV-Element CRC16 check error
    kErrorPageCRC32    = kKvDataError + 2,     // Page self CRC32 check error
    kErrorBlockID      = kKvDataError + 3,     // BlockID error: Main ID
    kErrorPageID       = kKvDataError + 4,     // Invalid page ID
    kErrorWalogID      = kKvDataError + 5,     // Invalid WalogID
    kErrorBossID       = kKvDataError + 6,     // Invalid BossID
    kIndexGroup        = kKvDataError + 7,     // Index error: Group array
    kIndexBoss         = kKvDataError + 8,     // Index error: PageFree array
    kIndexHash         = kKvDataError + 9,     // Index error: Hash array
    kIndexWalog        = kKvDataError + 10,    // Index error: Walog array
    kIndexKvArray      = kKvDataError + 11,    // Index error: Kv array
    kKvElementWrite    = kKvDataError + 12,    // Error writing KV-Element
    kKvElementRead     = kKvDataError + 13,    // Error reading KV-Element
    kKvElementValue    = kKvDataError + 14,    // Invalid BlockIndex data
    kKvElementNotFind  = kKvDataError + 15,    // Corresponding KV-Element not found
    kKvBuffCapacity    = kKvDataError + 16,    // Provided Buff capacity insufficient
    kKvDataType        = kKvDataError + 17,    // KV read data type error
    kKvElementCrypot   = kKvDataError + 18,    // Kv data encrypt/decrypt error
    kKvReddCheck       = kKvDataError + 19,    // KV data redundancy check failed
    kReddCrypto        = kKvDataError + 20,    // Redundancy encryption algorithm calculation error
};
//********************************/
/// @brief
/// @code{.isoft}
/// @unit_name=PKvErrorCode
/// @endcode
class PKvErrorCode
{
private:
    /// @brief
    mutable EErrorPHKV eErrorCode_{EErrorPHKV::kSuccess};

public:
    /// @brief
    PKvErrorCode() = default;
    /// @brief
    virtual ~PKvErrorCode() = default;
    /// @brief
    /// @param a
    PKvErrorCode(PKvErrorCode const &a) = delete;
    /// @brief
    /// @param a
    /// @return
    PKvErrorCode &operator=(PKvErrorCode const &a) = delete;
    /// @brief
    /// @param a
    PKvErrorCode(PKvErrorCode &&a) noexcept = delete;
    /// @brief
    /// @param a
    /// @return
    PKvErrorCode &operator=(PKvErrorCode &&a) noexcept = delete;

public:
    /// @brief
    /// @param pchFile
    /// @param nLineNum
    /// @param eErrorCode
    /// @param nLogLevel
    static void OnOptFailed(char8_t const *const pchFile,
                            int32_t const nLineNum,
                            EErrorPHKV const eErrorCode,
                            ara::log::LogLevel const nLogLevel = ara::log::LogLevel::kError) noexcept;
    /// @brief Check parameter validity
    /// @param pBData
    /// @param nDataLen
    /// @return
    static bool CheckParam(uint8_t const *const pBData, int32_t const nDataLen) noexcept;
    /// @brief Check parameter validity
    /// @param nDataLen
    /// @return
    static bool CheckParam(int32_t const nDataLen) noexcept;
    /// @brief
    /// @return
    inline EErrorPHKV GetLastError() const noexcept { return eErrorCode_; }
    /// @brief
    /// @return
    inline bool IsHaveError() const noexcept { return EErrorPHKV::kSuccess != eErrorCode_; }
    /// @brief
    inline void ClearError() const noexcept { eErrorCode_ = EErrorPHKV::kSuccess; }

protected:
    /// @brief
    /// @param pchFile
    /// @param nLineNum
    /// @param eErrorCode
    /// @param nLogLevel
    inline void _OnOptFailed(char8_t const *const pchFile,
                             int32_t const nLineNum,
                             EErrorPHKV const eErrorCode,
                             ara::log::LogLevel const nLogLevel = ara::log::LogLevel::kError) const noexcept
    {
        if (EErrorPHKV::kKvElementNotFind != eErrorCode) {
            OnOptFailed(pchFile, nLineNum, eErrorCode, nLogLevel);
        }
        _OnOptFailed(eErrorCode);
    }
    /// @brief
    /// @param eErrorCode
    inline void _OnOptFailed(EErrorPHKV const eErrorCode) const noexcept { eErrorCode_ = eErrorCode; }
    /// @brief
    inline void _OnSuccess() const noexcept { eErrorCode_ = EErrorPHKV::kSuccess; }
};
//********************************/
}  // namespace isoftkv
}  // namespace per
}  // namespace ara

#endif
