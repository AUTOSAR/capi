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
/// @file       kv_update_file.cpp
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

#include "ara/per/internal/isoftkv/kv_update_file.h"

#include <string>

#include "ara/per/internal/common/isoft_select_mofn.h"

namespace ara {
namespace per {
namespace isoftkv {
/// @brief Convert update strategy character to enum
/// @param chAction
/// @return
EUpdateAction GetUpdateAction(char8_t const chAction) noexcept
{
    EUpdateAction eUpdate;
    switch (chAction) {
        case 'E': {
            eUpdate = EUpdateAction::kKeepExisting;
        } break;
        case 'W': {
            eUpdate = EUpdateAction::kOverWrite;
        } break;
        case 'D': {
            eUpdate = EUpdateAction::kDelete;
        } break;
        case 'I': {  // NOLINT
            eUpdate = EUpdateAction::kIgnore;
        } break;
        default: {
            eUpdate = EUpdateAction::kIgnore;
        } break;
    }
    return eUpdate;
}
//********************************/
/// @brief
/// @param fileOpt
/// @param nPageLen
POptUpdateLoadLine::POptUpdateLoadLine(PFileOpt &fileOpt, uint32_t const nPageLen) noexcept
    : fileOpt_{fileOpt}, readBuff_{nPageLen * kInt_2U}, nScanPos_{0}, nLineIndex_{0}, nReadTotal_{0}, nDealTotal_{0}
{
}
//***************/
/// @brief Set the file operation cursor
/// @param eSeekType
/// @param nOffset
/// @return
bool POptUpdateLoadLine::SeekFilePos(EFileSeekType const eSeekType, int64_t const nOffset) const noexcept
{
    return fileOpt_.SeekPos(eSeekType, nOffset);
}
/// @brief Check if the end of the file has been reached
/// @return
bool POptUpdateLoadLine::IsFileEnd() const noexcept
{
    if (false == fileOpt_.IsOpen()) {
        return true;
    }
    return nReadTotal_ >= fileOpt_.GetFileSize();
}
/// @brief
/// @param nStartPos
/// @param nEndPos
/// @param nSeparation
/// @return
int32_t POptUpdateLoadLine::ReadLine(int32_t &nStartPos, int32_t &nEndPos, uint8_t const nSeparation) noexcept
{
    std::ignore = nStartPos;
    std::ignore = nEndPos;
    if (false == fileOpt_.IsOpen()) {
        return -1;
    }
    int32_t const nFileLen{static_cast< int32_t >(fileOpt_.GetFileSize())};
    int32_t nFindPos{-1};
    if (readBuff_.size() > 0U) {
        nFindPos = _FindLinePos(nScanPos_, nSeparation);
    }
    // Re-read at most one page
    if (nFindPos < 0) {
        if (nReadTotal_ >= nFileLen) {
            return -1;
        }
        // First copy the remaining data out
        std::ignore = _MoveValidData();
        // Load new data
        int32_t const nReadLen{_ReadData()};
        if (nReadLen < 0) {
            return -1;
        }
        nReadTotal_ += nReadLen;
        nFindPos = _FindLinePos(nScanPos_, nSeparation);
    }
    int32_t const nWorkLineID{nLineIndex_};
    // Process the corresponding line
    if (nFindPos >= 0) {
        // Also distinguish between finding a single line or finding a line that spans pages
        nStartPos = nScanPos_;
        nEndPos   = nFindPos;
        nLineIndex_ += 1;
        nScanPos_ = nFindPos + 1;  // +1 to skip '\n'
    } else                         // Line terminator not found
    {
        nStartPos = nScanPos_;
        if ((readBuff_.size() >= readBuff_.GetBuffLen()) || (nReadTotal_ >= nFileLen)) {
            nEndPos   = static_cast< int32_t >(readBuff_.GetBuffLen());
            nScanPos_ = static_cast< int32_t >(readBuff_.GetBuffLen());
        }
        if (nReadTotal_ >= nFileLen)  // End of file reached
        {
            nLineIndex_ += 1;
        }
    }
    return nWorkLineID;
}
/// @brief Get the current line data
/// @param nPos
/// @return
uint8_t const *POptUpdateLoadLine::GetLineData(int32_t const nPos) const noexcept
{
    return readBuff_.data(static_cast< uint32_t >(nPos));
}
/// @brief
/// @return
int32_t POptUpdateLoadLine::GetLineValidLen() const noexcept { return static_cast< int32_t >(readBuff_.size()); }
/// @brief Get the file cursor at a position offset by nReadOffset relative to PReadBuff
/// @param nReadOffset
/// @return
int64_t POptUpdateLoadLine::GetReadFilePos(int32_t const nReadOffset) const noexcept
{
    return nDealTotal_ + nReadOffset;
}
/// @brief Set a new file cursor and clear the buffer
/// @param nNewFilePos
/// @return
bool POptUpdateLoadLine::SetReadFilePos(int64_t const nNewFilePos) noexcept
{
    if (false == fileOpt_.IsOpen()) {
        return false;
    }
    nReadTotal_ = nNewFilePos;
    nDealTotal_ = nNewFilePos;
    std::ignore = fileOpt_.SeekPos(EFileSeekType::kSeekSet, nReadTotal_);
    readBuff_.SetDataLen(0U);
    nScanPos_ = 0;
    nLineIndex_ -= 1;  // Note: line number must also be maintained
    return true;
}
/// @brief Clear the buffer and move the file cursor
/// @param nPos
/// @return
bool POptUpdateLoadLine::ResetReadLine(int32_t const nPos) noexcept { return SetReadFilePos(nDealTotal_ + nPos); }
//***************/
/// @brief
/// @return
int32_t POptUpdateLoadLine::_ReadData() noexcept
{
    PER_Assert(fileOpt_.IsOpen());
    if (readBuff_.size() >= readBuff_.GetBuffLen()) {
        return -1;
    }
    int32_t const nPlanLen{static_cast< int32_t >(readBuff_.GetBuffLen()) - static_cast< int32_t >(readBuff_.size())};
    int32_t const nReadLen{fileOpt_.ReadData(readBuff_.data(readBuff_.size()), static_cast< uint32_t >(nPlanLen))};
    if (nReadLen > 0) {
        readBuff_.SetDataLen(readBuff_.size() + static_cast< uint32_t >(nReadLen));
    }
    return nReadLen;
}
/// @brief Move the remaining data block to the buffer head
/// @return
int32_t POptUpdateLoadLine::_MoveValidData() noexcept
{
    if (nScanPos_ <= 0) {
        return -1;
    }
    int32_t const nCopyLen{static_cast< int32_t >(readBuff_.GetBuffLen()) - nScanPos_};
    if (nCopyLen > 0) {
        std::ignore = T_Memcpy(readBuff_.data(), readBuff_.data(static_cast< uint32_t >(nScanPos_)),
                               static_cast< uint32_t >(nCopyLen));
    }
    nDealTotal_ += nScanPos_;
    nScanPos_ = 0;
    readBuff_.SetDataLen(static_cast< uint32_t >(nCopyLen));
    return static_cast< int32_t >(readBuff_.size());
}
/// @brief Find a delimiter
/// @param nPosStart
/// @param nSeparation
/// @return
int32_t POptUpdateLoadLine::_FindLinePos(int32_t const nPosStart, uint8_t const nSeparation) const noexcept
{
    for (int32_t i{nPosStart}; i < static_cast< int32_t >(readBuff_.size()); i++) {
        if (i >= static_cast< int32_t >(readBuff_.GetBuffLen())) {
            break;
        }
        if (readBuff_.at(static_cast< uint32_t >(i)) == nSeparation) {
            return i;
        }
    }
    return -1;
}
//********************************/
/// @brief Class for processing/operating line data in an UpdateFile: find the corresponding Key
class POptUpdateLine_Find final
{
private:
    /// @brief Whether to allow operation of "ignore items"
    bool bEnableScanIgnore_;
    /// @brief Update strategy
    EUpdateAction eUpdateAction_;
    /// @brief KeyData
    ara::core::String stKey_;
    /// @brief Value type
    int32_t nValueType_;
    /// @brief List of page locations where data is stored within the database
    ara::core::Vector< uint8_t > vecValue_;
    /// @brief CRC checksum read from the file
    uint32_t nReadCrc32_;

private:
    /// @brief Reset all attributes
    void _reset() noexcept
    {
        eUpdateAction_ = EUpdateAction::kIgnore;
        stKey_.clear();
        nValueType_ = 0;
        vecValue_.clear();
        nReadCrc32_ = 0U;
    }

public:
    /// @brief Delete default constructor
    explicit POptUpdateLine_Find(bool bEnableScanIgnore) noexcept
        : bEnableScanIgnore_{bEnableScanIgnore}, eUpdateAction_{EUpdateAction::kIgnore}, nValueType_(0), nReadCrc32_{0U}
    {
    }
    /// @brief Get the currently operating key
    /// @return
    inline ara::core::String const &GetKey() const noexcept { return stKey_; }
    /// @brief
    /// @return
    inline ara::core::Vector< uint8_t > const &GetValue() const noexcept { return vecValue_; }
    /// @brief
    /// @return
    inline int32_t const &GetValueType() const noexcept { return nValueType_; }
    /// @brief Process the line-read data (will not be interrupted)
    /// @param nLineID
    /// @param pOptKvUpdateRead
    /// @return -1 indicates found
    int32_t DealReadData(int32_t const nLineID,
                         PKvUpdateRead_Base const *const pOptKvUpdateRead,
                         ara::core::StringView const &stKeyFind) noexcept
    {
        std::ignore = nLineID;
        PER_Assert(nullptr != pOptKvUpdateRead);
        // eUpdateWord is used to distinguish the type of pOptKvUpdateRead object
        EUpdateKeyWord const eUpdateWord{pOptKvUpdateRead->GetUpdateWord()};
        if (false == bEnableScanIgnore_) {
            if ((EUpdateAction::kIgnore == eUpdateAction_) && (EUpdateKeyWord::kAction != eUpdateWord)) {
                return 0;
            }
        }
        int32_t const nDataType{pOptKvUpdateRead->GetReadDataType()};
        int32_t const nTotalLen{static_cast< int32_t >(pOptKvUpdateRead->GetReadTotalLen())};
        uint8_t const *const pBRead{pOptKvUpdateRead->GetReadData()};
        int32_t const nReadLen{static_cast< int32_t >(pOptKvUpdateRead->GetReadLen())};
        switch (eUpdateWord) {
            case EUpdateKeyWord::kAction: {
                std::ignore = _DealReadAction(nDataType, nTotalLen, pBRead, nReadLen);
            } break;
            case EUpdateKeyWord::kKey: {
                std::ignore = _DealReadKey(nDataType, nTotalLen, pBRead, nReadLen);
            } break;
            case EUpdateKeyWord::kValue: {
                // After receiving the CRC and successfully verifying it, proceed with the second traversal of the Value data
                if (stKey_ == stKeyFind) {
                    std::ignore = _DealReadValue(nDataType, nTotalLen, pBRead, nReadLen);
                }
            } break;
            case EUpdateKeyWord::kCrc: {
                if (stKey_ == stKeyFind) {
                    std::ignore = _DealReadCrc(nDataType, nTotalLen, pBRead, nReadLen);
                    return -1;
                }
            } break;
        }
        return nReadLen;
    }

protected:
    /// @brief Process update action data
    /// @param nDataType
    /// @param nTotalLen
    /// @param pBRead
    /// @param nReadLen
    /// @return
    int32_t _DealReadAction(int32_t const nDataType,
                            int32_t const nTotalLen,
                            uint8_t const *const pBRead,
                            int32_t const nReadLen) noexcept
    {
        std::ignore = nDataType;
        std::ignore = nTotalLen;
        if ((nullptr == pBRead) || (nReadLen <= 0)) {
            return 0;
        }
        _reset();
        ara::core::StringView const stAction{std::move(T_StringView(pBRead, static_cast< uint32_t >(nReadLen)))};
        eUpdateAction_ = GetUpdateAction(stAction.at(0U));
        return nReadLen;
    }
    /// @brief Process Key
    /// @param nDataType
    /// @param nTotalLen
    /// @param pBRead
    /// @param nReadLen
    /// @return
    int32_t _DealReadKey(int32_t const nDataType,
                         int32_t const nTotalLen,
                         uint8_t const *const pBRead,
                         int32_t const nReadLen) noexcept
    {
        std::ignore = nDataType;
        std::ignore = nTotalLen;
        if ((nullptr == pBRead) || (nReadLen <= 0)) {
            return 0;
        }
        stKey_ = T_String(pBRead, static_cast< uint32_t >(nReadLen));
        return nReadLen;
    }
    /// @brief Process Value
    /// @param nDataType
    /// @param nTotalLen
    /// @param pBRead
    /// @param nReadLen
    /// @return
    int32_t _DealReadValue(int32_t const nDataType,
                           int32_t const nTotalLen,
                           uint8_t const *const pBRead,
                           int32_t const nReadLen) noexcept
    {
        if ((nullptr == pBRead) || (nReadLen <= 0)) {
            return 0;
        }
        if (vecValue_.empty()) {
            vecValue_.reserve(nTotalLen);
        }
        nValueType_ = nDataType;
        for (int32_t i = 0; i < nReadLen; i++) {
            vecValue_.push_back(pBRead[i]);
        }
        return nReadLen;
    }
    /// @brief Process Crc
    /// @param nDataType
    /// @param nTotalLen
    /// @param pBRead
    /// @param nReadLen
    /// @return
    int32_t _DealReadCrc(int32_t const nDataType,
                         int32_t const nTotalLen,
                         uint8_t const *const pBRead,
                         int32_t const nReadLen) noexcept
    {
        std::ignore = nDataType;
        std::ignore = nTotalLen;
        if ((nullptr == pBRead) || (nReadLen <= 0)) {
            return 0;
        }
        ara::core::StringView const stCrc{std::move(T_StringView(pBRead, static_cast< uint32_t >(nReadLen)))};
        nReadCrc32_ = isoftkv::T_stoi< uint32_t >(stCrc.data(), static_cast< uint32_t >(stCrc.size()), kInt_10);
        return nReadLen;
    }
};
//***************/
/// @brief
/// @param stLogKvName
/// @param stFileName
/// @param bReadOnly
PKvUpdateFile::PKvUpdateFile(ara::core::StringView const &stLogKvName,
                             ara::core::StringView const &stFileName,
                             bool const bReadOnly) noexcept
    : fileOpt_{PFileOpt::ReadWrite()}, stLogKvName_{stLogKvName}
{
    if (bReadOnly) {
        fileOpt_.SetFileFlags(static_cast< uint32_t >(NAI_O_RDONLY));
    }
    if (false == fileOpt_.DoPrepareWork(stFileName, 0U)) {
        if (false == bReadOnly) {
            PER_OnOptFailed(EErrorPHKV::kFileUpdateOpen);
        }
    }
}
/// @brief
PKvUpdateFile::~PKvUpdateFile() noexcept { std::ignore = fileOpt_.CloseFile(); }
/// @brief
/// @return
bool PKvUpdateFile::IsAccessReady() const noexcept { return fileOpt_.IsOpen(); }
/// @brief
/// @return
inline ara::core::StringView PKvUpdateFile::GetLogKvName() const noexcept { return stLogKvName_; }
//***************/
/// @brief Clear the file
/// @return
bool PKvUpdateFile::ClearFile() const noexcept
{
    if (false == fileOpt_.IsOpen()) {
        return false;
    }
    std::ignore = fileOpt_.ResizeFile(0);
    return true;
}
/// @brief Prepare for reading, reset cursor to zero
/// @return
bool PKvUpdateFile::PrepareRead() const noexcept
{
    if (false == fileOpt_.IsOpen()) {
        return false;
    }
    std::ignore = fileOpt_.SeekPos(EFileSeekType::kSeekSet, 0);
    return true;
}
/// @brief Save in self-update format
/// @param nLineIndex
/// @param eUpdateAction
/// @param stData
/// @return
bool PKvUpdateFile::SaveKey(int32_t const nLineIndex,
                            EUpdateAction const eUpdateAction,
                            ara::core::String const &stData) const noexcept
{
    if (false == IsAccessReady()) {
        PER_OnOptFailed(EErrorPHKV::kFileUpdateOpen);
        return false;
    }
    PKvUpdateSave_Action optSaveAction{fileOpt_, nLineIndex, nullptr};
    ara::core::String stAction;
    stAction += static_cast< char8_t >(eUpdateAction);
    optSaveAction.SaveString(EUpdateKeyWord::kAction, stAction, false);
    // Save type: defined as raw data on 2021-09-16
    PKvUpdateSave_String optSave{fileOpt_, nLineIndex, nullptr};
    optSave.SaveString(EUpdateKeyWord::kKey, stData, false);
    return true;
}
/// @brief Save a KV pair
/// @param nLineIndex
/// @param eUpdateAction
/// @param stKey
/// @param nDataType
/// @param pVoidData
/// @param nDataLen
/// @return
bool PKvUpdateFile::SaveElement(int32_t const nLineIndex,
                                EUpdateAction const eUpdateAction,
                                ara::core::String const &stKey,
                                int32_t const nDataType,
                                const void *const pVoidData,
                                uint32_t const nDataLen) const noexcept
{
    if (false == IsAccessReady()) {
        PER_OnOptFailed(EErrorPHKV::kFileUpdateOpen);
        return false;
    }
    uint32_t nCrc{0U};
    // Save update action identifier
    PKvUpdateSave_Action optSaveAction{fileOpt_, nLineIndex, nullptr};
    ara::core::String stAction;
    stAction += static_cast< char8_t >(eUpdateAction);
    optSaveAction.SaveString(EUpdateKeyWord::kAction, stAction, false);
    // Save KeyData
    PKvUpdateSave_String optSaveKey{fileOpt_, nLineIndex, nullptr};
    optSaveKey.SaveString(EUpdateKeyWord::kKey, stKey, false);
    nCrc = CalculateCrc32(stKey.data(), static_cast< uint32_t >(stKey.size()), nCrc);
    // Save ValueData
    PWordEncode wordEncode{nCrc};
    PKvUpdateSave_Value optSaveValue{fileOpt_, nLineIndex, &wordEncode};
    std::ignore = optSaveValue.SaveBegin(EUpdateKeyWord::kValue, nDataType, nDataLen);
    std::ignore = optSaveValue.SaveData(pVoidData, nDataLen);
    std::ignore = optSaveValue.SaveEnd(false);
    nCrc        = CalculateCrc32(pVoidData, nDataLen, nCrc);
    // Save CRC
    if (nCrc > 0U) {
        ara::core::String const stCrc{std::to_string(nCrc)};
        PKvUpdateSave_String optSaveCrc{fileOpt_, 0};
        optSaveCrc.SaveString(EUpdateKeyWord::kCrc, stCrc, true);
    }
    return true;
}
/// @brief Iterate over each line of data
/// @param bEnableIgnore Whether to skip items with Action [I] in the file: mainly AppVersion, PerVersion
/// @param nPageLen
/// @param pFunLine
/// @return
int32_t PKvUpdateFile::ScanAllLine(bool const bEnableIgnore,
                                   uint32_t const nPageLen,
                                   CB_ReadUpdateData const &pFunLine) noexcept
{
    PER_Assert(nPageLen >= kInt_512U);
    if (false == PrepareRead()) {
        PER_OnOptFailed(EErrorPHKV::kFileUpdateRead);
        return 0;
    }
    POptUpdateLoadLine optLoadLine{fileOpt_, nPageLen};
    int32_t nLineID{-1};
    int32_t nPosValueDataStart{0};
    uint32_t nWorkCrc{0U};           // CRC32 code calculated for each line of data
    uint32_t nReadCrc{0U};           // CRC32 code read
    ara::core::String stReadCrcSrc;  // Original CRC content from the file
    PWordEncode wordEncode{0U};
    while (true) {  // do {} while(nLineID >= 0)
        int32_t nStartPos{-1};
        int32_t nEndPos{-1};
        int32_t const nWorkLineID{optLoadLine.ReadLine(nStartPos, nEndPos, static_cast< uint8_t >(kChar_Enter))};
        if (nWorkLineID < 0) {
            if (false == optLoadLine.IsFileEnd()) {
                PER_OnOptFailed(EErrorPHKV::kFileUpdateRead);
            }
            break;
        }
        // End not found: indicates it's not a complete line, it's the first packet of a line that spans pages
        if (nEndPos < 0) {
            nEndPos = optLoadLine.GetLineValidLen();
        }
        if ((nEndPos - nStartPos) <= 0) {
            continue;
        }
        PKvUpdateRead_Action optReadAction{nWorkLineID, EUpdateKeyWord::kAction, nullptr};
        PKvUpdateRead_String optReadKey{nWorkLineID, EUpdateKeyWord::kKey, nullptr};
        optReadKey.ClearQac();
        PKvUpdateRead_Value optReadValue{nWorkLineID, EUpdateKeyWord::kValue, &wordEncode};
        optReadValue.ClearReadLen();
        PKvUpdateRead_String optReadCrc{nWorkLineID, EUpdateKeyWord::kCrc, nullptr};
        optReadCrc.ClearQac();
        uint8_t const *const pBLineData{optLoadLine.GetLineData(nStartPos)};
        if (nWorkLineID > nLineID)  // Newline
        {
            nWorkCrc = 0U;
            nReadCrc = 0U;
            stReadCrcSrc.clear();
            wordEncode.SetCode(0U);
            // At this point, the full Key text and Value length can be found; if the end is not found, it indicates it's not a complete line, it's the first packet of a line that spans pages
            nLineID = nWorkLineID;
            int32_t nScanIndex{0};
            // Read Action
            nScanIndex = optReadAction.ParseReadData(pBLineData, nEndPos - nStartPos, nScanIndex);
            PER_Assert(nScanIndex > 0);
            if (bEnableIgnore) {
                if (optReadAction.GetUpdateAction() == static_cast< char8_t >(EUpdateAction::kIgnore)) {
                    continue;
                }
            }
            // Read Key
            nScanIndex = optReadKey.ParseReadData(pBLineData, nEndPos - nStartPos, nScanIndex);
            PER_Assert(nScanIndex > 0);
            nWorkCrc                    = CalculateCrc32(optReadKey.GetReadData(), optReadKey.GetReadLen(), nWorkCrc);
            ara::core::String stKeyTemp = optReadKey.GetReadString();
            // Read Value
            wordEncode.SetCode(nWorkCrc);
            nScanIndex = optReadValue.ParseReadData(pBLineData, nEndPos - nStartPos, nScanIndex);
            ara::core::String stValueTemp{
                T_String(static_cast< void const * >(optReadValue.GetReadData()), optReadValue.GetReadLen())};
            PER_Assert(nScanIndex > 0);
            nWorkCrc = CalculateCrc32(optReadValue.GetReadData(), optReadValue.GetReadLen(), nWorkCrc);
            // Save the cursor of the Value's data area
            nPosValueDataStart
                = static_cast< int32_t >(optLoadLine.GetReadFilePos(nStartPos + optReadValue.GetDataStartPos()));
            // Read CRC
            nScanIndex = optReadCrc.ParseReadData(pBLineData, nEndPos - nStartPos, nScanIndex);
            if (nScanIndex > 0)  // Indicates that the entire KV Update information was read within one page
            {
                nReadCrc = T_stoi< uint32_t >(optReadCrc.GetReadData(), optReadCrc.GetReadLen(), kInt_10);
                if ((nReadCrc > 0U) && (nReadCrc == nWorkCrc)) {
                    if (pFunLine(nLineID, &optReadAction) < 0) {
                        break;
                    }
                    if (pFunLine(nLineID, &optReadKey) < 0) {
                        break;
                    }
                    if (pFunLine(nLineID, &optReadValue) < 0) {
                        break;
                    }
                    if (pFunLine(nLineID, &optReadCrc) < 0) {
                        break;
                    }
                }
            }
        } else {  // This is a line spanning pages, containing Value + CRC
            int32_t nScanIndex{0};
            // Need to handle the possibly existing CRC value, and re-traverse the data after CRC verification
            if (nReadCrc > 0U) {
                nScanIndex = optReadValue.ParseReadDataAdd(pBLineData, nEndPos - nStartPos, nScanIndex);
                PER_Assert(nScanIndex > 0);
                if (pFunLine(nLineID, &optReadValue) < 0) {
                    break;
                }
                // No longer handle second read of CRC
            } else {
                // Value not fully read, prioritize reading Value
                if (optReadValue.GetReadLen() < optReadValue.GetReadTotalLen()) {
                    // CRC must appear after Value
                    nScanIndex = optReadValue.ParseReadDataAdd(pBLineData, nEndPos - nStartPos, nScanIndex);
                    if (nReadCrc <= 0U) {  // Indicates Value
                        nWorkCrc = CalculateCrc32(optReadValue.GetReadData(), optReadValue.GetReadLen(), nWorkCrc);
                    }
                    if (nScanIndex > 0) {
                        int32_t const nTempLen{nEndPos - (nStartPos + nScanIndex)};
                        stReadCrcSrc = T_String(pBLineData + nScanIndex, static_cast< uint32_t >(nTempLen));
                    }
                } else {
                    stReadCrcSrc += T_String(pBLineData, static_cast< uint32_t >(nEndPos));
                }
                // CRC may be fragmented, so additionally save the raw CRC content "C={2123883332},"
                if (false == stReadCrcSrc.empty()) {
                    int32_t const nPosEndCrc{optReadCrc.ParseReadData(T_TransBytes(stReadCrcSrc.data()),
                                                                      static_cast< int32_t >(stReadCrcSrc.size()), 0)};
                    if (nPosEndCrc > 0) {
                        nReadCrc = T_stoi< uint32_t >(optReadCrc.GetReadData(), optReadCrc.GetReadLen(), kInt_10);
                        if ((nReadCrc > 0U) && (nReadCrc == nWorkCrc)) {
                            // Send the saved Action, Key, CRC
                            if (pFunLine(nLineID, &optReadAction) < 0) {
                                break;
                            }
                            if (pFunLine(nLineID, &optReadKey) < 0) {
                                break;
                            }
                            if (pFunLine(nLineID, &optReadCrc) < 0) {
                                break;
                            }
                            // Reset the File operation cursor to the Value data area "V=3,4,{bGcHaAaA}"
                            std::ignore = optLoadLine.SetReadFilePos(static_cast< int64_t >(nPosValueDataStart));
                            optReadValue.ClearReadLen();
                        } else {
                            LogInfo() << "KvStorage[" << stLogKvName_.data() << "] Find CRC Error When InitUpdate.Key["
                                      << optReadKey.GetReadString().c_str() << "], Ignore This Record.";
                        }
                    }
                }
            }
        }
    }

    return nLineID + 1;  // Return value is the number of lines: nLineID starts counting from 0
}
/// @brief Find a version number
/// @param stVersinName Key name corresponding to the version number
/// @param nPageLen Single page length
/// @return Retrieved version number in string format
ara::core::String PKvUpdateFile::FindVersion(ara::core::StringView const &stVersinName,
                                             uint32_t const nPageLen) noexcept
{
    ara::per::isoftkv::POptUpdateLine_Find optUpdateFind{true};
    std::ignore
        = ScanAllLine(false, static_cast< uint32_t >(nPageLen),
                      [stVersinName, &optUpdateFind](
                          int32_t const nLineID, PKvUpdateRead_Base const *const pKvUpdateRead) noexcept -> int32_t {
                          // Will return -1 upon finding, thus interrupting traversal
                          return optUpdateFind.DealReadData(nLineID, pKvUpdateRead, stVersinName);
                      });
    ara::core::String stReturn;
    if (optUpdateFind.GetKey() != stVersinName) {
        return stReturn;
    }
    ara::core::Vector< uint8_t > const &vecValue{optUpdateFind.GetValue()};
    if (static_cast< int32_t >(ara::per::isoftkv::EDataType::kString) == optUpdateFind.GetValueType()) {
        stReturn = ara::core::String{ara::per::isoftkv::T_TransChar(vecValue.data()), vecValue.size()};
    } else {
        ara::per::isoftkv::PPerVersion const *pPerVersion{
            ara::per::isoftkv::T_TransPtr< ara::per::isoftkv::PPerVersion >(vecValue.data())};
        stReturn = Version_PerToString(*pPerVersion);
    }
    return stReturn;
}
/// @brief Find the Line data corresponding to stKey
/// @param stKey Key to be found
/// @param nPageLen Single page length of the file
/// @param [out] eDataType Located data type
/// @return
ara::core::Vector< uint8_t > PKvUpdateFile::FindLineData(ara::core::StringView const &stKey,
                                                         uint32_t const nPageLen,
                                                         EDataType &eDataType) noexcept
{
    ara::per::isoftkv::POptUpdateLine_Find optUpdateFind{true};
    std::ignore
        = ScanAllLine(false, static_cast< uint32_t >(nPageLen),
                      [stKey, &optUpdateFind](int32_t const nLineID,
                                              PKvUpdateRead_Base const *const pKvUpdateRead) noexcept -> int32_t {
                          // Will return -1 upon finding, thus interrupting traversal
                          return optUpdateFind.DealReadData(nLineID, pKvUpdateRead, stKey);
                      });

    eDataType = EDataType::kDefault;
    if (optUpdateFind.GetKey() != stKey) {
        return ara::core::Vector< uint8_t >{};
    }
    eDataType = TransToDataType(optUpdateFind.GetValueType());
    return optUpdateFind.GetValue();
}
//********************************/
}  // namespace isoftkv
}  // namespace per
}  // namespace ara
