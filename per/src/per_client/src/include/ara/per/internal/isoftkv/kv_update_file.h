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
/// @file       kv_update_file.h
/// @brief      AutoSar-AP Data Persistence Storage Module
/// @details    KV storage self-update operation class
/// @date       2021-05-18
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/per/KeyValueStorage/Data Version Management
/// @interface_level=module
/// @trace_id_sr=
/// @unit_name=
/// @unit_description=KV storage self-update operation
/// @endcode
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// | 2021-08-18 | 1.0.0   | hanjingjing  | Initial version created                   |
///
/// ================================================================

#ifndef ARA_PER_PHKV_KV_UPDATE_FILE_H_
#define ARA_PER_PHKV_KV_UPDATE_FILE_H_

#include <ara/core/vector.h>

#include "ara/per/internal/common/isoft_auto_buff.h"
#include "ara/per/internal/common/isoft_file_opt.h"
#include "ara/per/internal/isoftkv/kv_object.h"
#include "ara/per/internal/isoftkv/kv_update_opt.h"

namespace ara {
namespace per {
namespace isoftkv {
//********************************/
/// @brief Update strategy for each record
/// @code{.isoft}
/// @unit_name=EUpdateAction
/// @endcode
enum class EUpdateAction : char8_t
{
    kKeepExisting = 'E',  // Existing data remains unchanged
    kOverWrite    = 'W',  // Overwrite existing data
    kDelete       = 'D',  // Delete existing data
    kIgnore       = 'I',  // Ignore item, do not enter KVS
};
/// @brief Convert update strategy character to enum
/// @code{.isoft}
/// @unit_name=GetUpdateAction
/// @endcode
/// @param chAction
/// @returns
EUpdateAction GetUpdateAction(char8_t const chAction) noexcept;
//********************************/
/// @brief Load a line of data from the Update file
/// @code{.isoft}
/// @unit_name=POptUpdateLoadLine
/// @interface_level=unit
/// @endcode
class POptUpdateLoadLine final
{
private:
    /// @brief
    PFileOpt &fileOpt_;
    /// @brief
    PAutoBuff readBuff_;
    /// @brief Traversal index during packet assembly
    int32_t nScanPos_;
    /// @brief Line number
    int32_t nLineIndex_;
    /// @brief Cumulative length read from the file
    int64_t nReadTotal_;
    /// @brief Cumulative processed length (also the total length discarded from the cache)
    int64_t nDealTotal_;

public:
    /// @brief
    POptUpdateLoadLine() = delete;
    /// @brief
    /// @param fileOpt
    /// @param nPageLen
    POptUpdateLoadLine(PFileOpt &fileOpt, uint32_t const nPageLen) noexcept;
    /// @brief
    ~POptUpdateLoadLine() = default;
    /// @brief
    /// @param a
    POptUpdateLoadLine(POptUpdateLoadLine const &a) = delete;
    /// @brief
    /// @param a
    /// @return
    POptUpdateLoadLine &operator=(POptUpdateLoadLine const &a) = delete;
    /// @brief
    /// @param a
    POptUpdateLoadLine(POptUpdateLoadLine &&a) = delete;
    /// @brief
    /// @param a
    /// @return
    POptUpdateLoadLine &operator=(POptUpdateLoadLine &&a) = delete;

public:
    /// @brief Set the file operation cursor
    /// @param eSeekType
    /// @param nOffset
    /// @return
    bool SeekFilePos(EFileSeekType const eSeekType, int64_t const nOffset) const noexcept;
    /// @brief Check if the end of the file has been reached
    /// @return
    bool IsFileEnd() const noexcept;
    /// @brief
    /// @param nStartPos
    /// @param nEndPos
    /// @param nSeparation
    /// @return
    int32_t ReadLine(int32_t &nStartPos, int32_t &nEndPos, uint8_t const nSeparation) noexcept;
    /// @brief Get the current line data
    /// @param nPos
    /// @return
    uint8_t const *GetLineData(int32_t const nPos) const noexcept;
    /// @brief
    /// @return
    int32_t GetLineValidLen() const noexcept;
    /// @brief Get the file cursor at a position offset by nReadOffset relative to PReadBuff
    /// @param nReadOffset
    /// @return
    int64_t GetReadFilePos(int32_t const nReadOffset) const noexcept;
    /// @brief Set a new file cursor and clear the buffer
    /// @param nNewFilePos
    /// @return
    bool SetReadFilePos(int64_t const nNewFilePos) noexcept;
    /// @brief Clear the buffer and move the file cursor
    /// @param nPos
    /// @return
    bool ResetReadLine(int32_t const nPos) noexcept;

protected:
    /// @brief Read data, return the actual length read
    /// @return
    int32_t _ReadData() noexcept;
    /// @brief Move the remaining data block to the buffer head
    /// @return
    int32_t _MoveValidData() noexcept;
    /// @brief Find a delimiter
    /// @param nPosStart
    /// @param nSeparation
    /// @return
    int32_t _FindLinePos(int32_t const nPosStart, uint8_t const nSeparation) const noexcept;
};
//********************************/
/// @brief UpdateFile operation class
/// @code{.isoft}
/// @unit_name=PKvUpdateFile
/// @endcode
class PKvUpdateFile final : public PKvObject
{
private:
    /// @brief
    PFileOpt fileOpt_;
    /// @brief
    ara::core::StringView stLogKvName_;

public:
    /// @brief
    using CB_ReadUpdateData
        = std::function< int32_t(int32_t const nLineIndex, PKvUpdateRead_Base const *const pOptKvUpdateRead) >;
    /// @brief
    /// @param stLogKvName
    /// @param stFileName
    /// @param bReadOnly
    /// @return
    PKvUpdateFile(ara::core::StringView const &stLogKvName,
                  ara::core::StringView const &stFileName,
                  bool const bReadOnly) noexcept;
    /// @brief
    ~PKvUpdateFile() noexcept final;
    /// @brief
    /// @param a
    PKvUpdateFile(PKvUpdateFile const &a) = delete;
    /// @brief
    /// @param a
    /// @return
    PKvUpdateFile &operator=(PKvUpdateFile const &a) = delete;
    /// @brief
    /// @param a
    PKvUpdateFile(PKvUpdateFile &&a) = delete;
    /// @brief
    /// @param a
    /// @return
    PKvUpdateFile &operator=(PKvUpdateFile &&a) = delete;
    /// @brief
    /// @return
    bool IsAccessReady() const noexcept final;
    /// @brief
    /// @return
    ara::core::StringView GetLogKvName() const noexcept final;

public:
    /// @brief
    /// @return
    inline PFileOpt const &GetFileOpt() const noexcept { return fileOpt_; }
    /// @brief Clear the file
    /// @return
    bool ClearFile() const noexcept;
    /// @brief Prepare for reading, reset cursor to zero
    /// @return
    bool PrepareRead() const noexcept;
    /// @brief Save in self-update format
    /// @param nLineIndex
    /// @param eUpdateAction
    /// @param stData
    /// @return
    bool SaveKey(int32_t const nLineIndex,
                 EUpdateAction const eUpdateAction,
                 ara::core::String const &stData) const noexcept;
    /// @brief Save a KV pair
    /// @param nLineIndex
    /// @param eUpdateAction
    /// @param stKey
    /// @param nDataType
    /// @param pVoidData
    /// @param nDataLen
    /// @return
    bool SaveElement(int32_t const nLineIndex,
                     EUpdateAction const eUpdateAction,
                     ara::core::String const &stKey,
                     int32_t const nDataType,
                     const void *const pVoidData,
                     uint32_t const nDataLen) const noexcept;
    /// @brief Iterate over each line of data
    /// @param bEnableIgnore Whether to skip items with Action [I] in the file: mainly AppVersion, PerVersion
    /// @param nPageLen
    /// @param pFunLine
    /// @return Return value is the number of lines: nLineID starts counting from 0
    int32_t ScanAllLine(bool const bEnableIgnore, uint32_t const nPageLen, CB_ReadUpdateData const &pFunLine) noexcept;
    /// @brief Find a version number
    /// @param stVersinName Key name corresponding to the version number
    /// @param nPageLen Single page length
    /// @return Retrieved version number in string format
    ara::core::String FindVersion(ara::core::StringView const &stVersinName, uint32_t const nPageLen) noexcept;
    /// @brief Find the Line data corresponding to stKey
    /// @param stKey Key to be found
    /// @param nPageLen Single page length of the file
    /// @param [out] eDataType Located data type
    /// @return
    ara::core::Vector< uint8_t > FindLineData(ara::core::StringView const &stKey,
                                              uint32_t const nPageLen,
                                              EDataType &eDataType) noexcept;

protected:
};
//********************************/
}  // namespace isoftkv
}  // namespace per
}  // namespace ara
#endif
