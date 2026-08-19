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
/// @file       kv_update_opt.cpp
/// @brief      AutoSar-AP Data Persistence Storage Module
/// @details    KV storage self-update operation class
/// @date       2021-08-18
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

#include "ara/per/internal/common/isoft_common_api.h"
#include "ara/per/internal/isoftkv/kv_update_file.h"

namespace ara {
namespace per {
namespace isoftkv {
//********************************/
/// @brief
/// @param nXorCode
PWordEncode::PWordEncode(uint32_t const nXorCode) noexcept
    : nXorCode_{static_cast< uint8_t >(T_Mod(nXorCode, kInt_0x100))}
{
}
/// @brief
/// @param chInput
/// @param nRandom
/// @return
ara::core::String PWordEncode::Encode(uint8_t chInput, int32_t const nRandom) const noexcept
{
    chInput ^= nXorCode_;
    ara::core::String stResult;
    uint32_t const nKey2{static_cast< uint32_t >(T_Mod(nRandom, kInt_0x10))};
    uint32_t const nKey1{kInt_0x10U - nKey2};
    stResult += T_itochar((static_cast< uint32_t >(chInput) / kInt_0x10U) ^ nKey1);
    stResult += T_itochar((static_cast< uint32_t >(chInput) % kInt_0x10U) ^ nKey2);
    return stResult;
}
/// @brief
/// @param chA
/// @param chB
/// @param nRandom
/// @return
uint8_t PWordEncode::Decode(uint8_t const chA, uint8_t const chB, int32_t const nRandom) const noexcept
{
    uint32_t const nKey2{static_cast< uint32_t >(T_Mod(nRandom, 0x10))};
    uint32_t const nKey1{kInt_0x10U - nKey2};
    uint32_t nData{0U};

    nData = (T_chartoi< uint32_t >(chA) ^ nKey1) * kInt_0x10U;
    nData += T_chartoi< uint32_t >(chB) ^ nKey2;

    nData ^= static_cast< uint32_t >(nXorCode_);
    return static_cast< uint8_t >(nData % kInt_0x100U);
}
//********************************/

/// @brief
/// @param fileOpt
/// @param nLineIndex
/// @param pWordEncode
PKvUpdateSave_Base::PKvUpdateSave_Base(PFileOpt const &fileOpt,
                                       int32_t const nLineIndex,
                                       PWordEncode *const pWordEncode) noexcept
    : fileOpt_{fileOpt}, nLineIndex_{nLineIndex}, pWordEncode_{pWordEncode}, bHaveBegin_{false}
{
}
/// @brief
/// @param eKeyWord
/// @param stData
/// @param bEndLine
void PKvUpdateSave_Base::SaveString(EUpdateKeyWord const eKeyWord,
                                    ara::core::String const &stData,
                                    bool const bEndLine) noexcept
{
    int32_t const nDataLen{static_cast< int32_t >(stData.size())};
    std::ignore = SaveBegin(eKeyWord, static_cast< int32_t >(EDataType::kString), static_cast< uint32_t >(nDataLen));
    std::ignore = SaveData(stData.data(), static_cast< uint32_t >(nDataLen));
    std::ignore = SaveEnd(bEndLine);
    return;
}
/// @brief
/// @param pVoidData
/// @param nDataLen
/// @return
int32_t PKvUpdateSave_Base::SaveData(const void *const pVoidData, uint32_t const nDataLen) noexcept
{
    if (false == fileOpt_.IsOpen()) {
        return 0;
    }
    ara::core::String stSaveData;
    stSaveData.reserve((static_cast< std::size_t >(nDataLen) + 1U) * kInt_2U);
    uint8_t const *const pBData{T_TransBytes(pVoidData)};
    if (nullptr == pWordEncode_)  // Choose encryption scheme
    {
        std::ignore = stSaveData.assign(T_TransPtr< char8_t const >(pBData), static_cast< std::size_t >(nDataLen));
    } else {
        for (int32_t i{0}; i < static_cast< int32_t >(nDataLen); i++) {
            stSaveData += pWordEncode_->Encode(*(pBData + i), (nLineIndex_ + 1) * kInt_0x10 + (i + 1));
        }
    }
    return fileOpt_.WriteString(T_StringView(stSaveData));
}
/// @brief
/// @param eKeyWord
/// @param nDataType
/// @param nDataLen
/// @return
int32_t PKvUpdateSave_Base::SaveBegin(EUpdateKeyWord const eKeyWord,
                                      int32_t const nDataType,
                                      uint32_t const nDataLen) noexcept
{
    std::ignore = eKeyWord;
    std::ignore = nDataType;
    std::ignore = nDataLen;
    if (false == _SaveStart()) {
        return 0;
    }
    return static_cast< int32_t >(nDataLen);
}
/// @brief
/// @param bEndLine
/// @return
int32_t PKvUpdateSave_Base::SaveEnd(bool const bEndLine) noexcept
{
    return _SaveEnd(bEndLine, ara::core::StringView{""});
}
/// @brief
/// @return
bool PKvUpdateSave_Base::_SaveStart() noexcept
{
    if (false == fileOpt_.IsOpen()) {
        return false;
    }
    bHaveBegin_ = true;
    return true;
}
/// @brief
/// @param bEndLine
/// @param stWord
/// @return
int32_t PKvUpdateSave_Base::_SaveEnd(bool const bEndLine, ara::core::StringView const &stWord) noexcept
{
    if (false == bHaveBegin_) {
        return 0;
    }
    if (false == fileOpt_.IsOpen()) {
        return 0;
    }

    int32_t nWriteLen{0};
    if (bEndLine)  // The code is tedious, but execution efficiency will be slightly higher
    {
        ara::core::String stEnd;
        stEnd += stWord.data();
        stEnd += "\n";
        nWriteLen += fileOpt_.WriteString(T_StringView(stEnd));
    } else {
        nWriteLen += fileOpt_.WriteString(stWord);
    }
    bHaveBegin_ = false;
    return nWriteLen;
}
//***************/

/// @brief
/// @param fileOpt
/// @param nLineIndex
/// @param pWordEncode
PKvUpdateSave_Value::PKvUpdateSave_Value(PFileOpt const &fileOpt,
                                         int32_t const nLineIndex,
                                         PWordEncode *const pWordEncode) noexcept
    : PKvUpdateSave_Base{fileOpt, nLineIndex, pWordEncode}
{
}
/// @brief
/// @param eKeyWord
/// @param nDataType
/// @param nDataLen
/// @return
int32_t PKvUpdateSave_Value::SaveBegin(EUpdateKeyWord const eKeyWord,
                                       int32_t const nDataType,
                                       uint32_t const nDataLen) noexcept
{
    if (false == _SaveStart()) {
        return 0;
    }
    uint8_t chBuff[kInt_0x100]{0U};
    std::ignore = snprintf(T_TransChar(static_cast< uint8_t * >(chBuff)), sizeof(chBuff), "%c=%x,%d,{",
                           static_cast< char8_t >(eKeyWord), nDataType, nDataLen);
    if (0U == chBuff[0]) {  // This output parameter value is not subsequently checked.
        chBuff[0] = 0U;
    }
    return _GetFileOpt().WriteString(ara::core::StringView{T_TransChar(static_cast< uint8_t * >(chBuff))});
}
/// @brief
/// @param bEndLine
/// @return
int32_t PKvUpdateSave_Value::SaveEnd(bool const bEndLine) noexcept
{
    return _SaveEnd(bEndLine, ara::core::StringView{"},"});
}
//***************/

/// @brief
/// @param fileOpt
/// @param nLineIndex
/// @param pWordEncode
PKvUpdateSave_String::PKvUpdateSave_String(PFileOpt const &fileOpt,
                                           int32_t const nLineIndex,
                                           PWordEncode *const pWordEncode) noexcept
    : PKvUpdateSave_Base{fileOpt, nLineIndex, pWordEncode}
{
}
/// @brief
/// @param eKeyWord
/// @param nDataType
/// @param nDataLen
/// @return
int32_t PKvUpdateSave_String::SaveBegin(EUpdateKeyWord const eKeyWord,
                                        int32_t const nDataType,
                                        uint32_t const nDataLen) noexcept
{
    std::ignore = eKeyWord;
    std::ignore = nDataType;
    std::ignore = nDataLen;
    if (false == _SaveStart()) {
        return 0;
    }
    ara::core::String stSaveData;
    stSaveData += static_cast< char8_t >(eKeyWord);
    stSaveData += "={";
    return _GetFileOpt().WriteString(T_StringView(stSaveData));
}
/// @brief
/// @param bEndLine
/// @return
int32_t PKvUpdateSave_String::SaveEnd(bool const bEndLine) noexcept
{
    return _SaveEnd(bEndLine, ara::core::StringView{"},"});
}
//***************/

/// @brief
/// @param fileOpt
/// @param nLineIndex
/// @param pWordEncode
PKvUpdateSave_Action::PKvUpdateSave_Action(PFileOpt const &fileOpt,
                                           int32_t const nLineIndex,
                                           PWordEncode *const pWordEncode) noexcept
    : PKvUpdateSave_Base{fileOpt, nLineIndex, pWordEncode}
{
}
/// @brief
/// @param eKeyWord
/// @param nDataType
/// @param nDataLen
/// @return
int32_t PKvUpdateSave_Action::SaveBegin(EUpdateKeyWord const eKeyWord,
                                        int32_t const nDataType,
                                        uint32_t const nDataLen) noexcept
{
    std::ignore = eKeyWord;
    std::ignore = nDataType;
    std::ignore = nDataLen;
    if (false == _SaveStart()) {
        return 0;
    }
    return _GetFileOpt().WriteString(ara::core::StringView{"["});
}
/// @brief
/// @param bEndLine
/// @return
int32_t PKvUpdateSave_Action::SaveEnd(bool const bEndLine) noexcept
{
    return _SaveEnd(bEndLine, ara::core::StringView{"]:"});
}
//********************************/
/// @brief
/// @param nLineIndex
/// @param eUpdateWord
/// @param pWordEncode
PKvUpdateRead_Base::PKvUpdateRead_Base(int32_t const nLineIndex,
                                       EUpdateKeyWord const eUpdateWord,
                                       PWordEncode *const pWordEncode) noexcept
    : nLineIndex_{nLineIndex}, eUpdateWord_{eUpdateWord}, pWordEncode_{pWordEncode}, nExtChar_{0U}, nDataStartPos_{0}
{
}
/// @brief
/// @return
uint8_t const *PKvUpdateRead_Base::GetReadData() const noexcept { return readBuff_.data(0U); }
/// @brief
/// @return
uint32_t PKvUpdateRead_Base::GetReadLen() const noexcept { return readBuff_.size(); }
/// @brief
/// @return
int32_t PKvUpdateRead_Base::GetReadDataType() const noexcept { return static_cast< int32_t >(EDataType::kString); }
/// @brief
/// @return
uint32_t PKvUpdateRead_Base::GetReadTotalLen() const noexcept { return readBuff_.size(); }
/// @brief Interpret the read data
/// @param pDataBase
/// @param nDataTotal
/// @param nOffset
/// @return
int32_t PKvUpdateRead_Base::ParseReadData(uint8_t const *pDataBase, int32_t nDataTotal, int32_t const nOffset) noexcept
{
    int32_t const nMaxDataLen{nDataTotal - nOffset};
    if (nMaxDataLen <= 0) {
        return -1;
    }

    V_ResetRead();
    PER_Assert((nullptr != pDataBase) && (nMaxDataLen > 0));
    uint8_t const *const pReadData{pDataBase + nOffset};
    ara::core::StringView::size_type nDataStart{0U};
    ara::core::StringView::size_type nDataEnd{0U};
    int32_t const nFindEnd{V_ParseReadData(
        ara::core::StringView{T_TransChar(pReadData), static_cast< ara::core::StringView::size_type >(nMaxDataLen)},
        nDataStart, nDataEnd)};
    if (nFindEnd < 0) {
        return nFindEnd;
    }
    int32_t const nDataLen{static_cast< int32_t >(nDataEnd) - static_cast< int32_t >(nDataStart)};
    if (nDataLen > 0) {
        readBuff_.ResetBuff(static_cast< uint32_t >(nDataLen));
        std::ignore = _DecodeData(pReadData + nDataStart, nDataLen);
        // Record the absolute offset of the data area
        nDataStartPos_ = nOffset + static_cast< int32_t >(nDataStart);
    }
    return nFindEnd + nOffset;
}
/// @brief
/// @param pDataBase
/// @param nDataTotal
/// @param nOffset
/// @return
int32_t PKvUpdateRead_Base::ParseReadDataAdd(uint8_t const *pDataBase,
                                             int32_t nDataTotal,
                                             int32_t const nOffset) noexcept
{
    readBuff_.SetDataLen(0U);  // Only need to reset the read length here
    int32_t const nMaxDataLen{nDataTotal - nOffset};
    PER_Assert((nullptr != pDataBase) && (nMaxDataLen > 0));
    // Find the ending delimiter "},"
    uint8_t const *const pReadData{pDataBase + nOffset};
    ara::core::StringView const stRead{T_TransChar(pReadData),
                                       static_cast< ara::core::StringView::size_type >(nMaxDataLen)};

    ara::core::StringView::size_type nPos4{stRead.find("},", 0U)};
    int32_t nReturn{0};
    if (nPos4 == ara::core::StringView::npos) {
        nPos4   = static_cast< ara::core::StringView::size_type >(nMaxDataLen);
        nReturn = nMaxDataLen;
    } else {
        nReturn = static_cast< int32_t >(nPos4) + kInt_2;
    }

    int32_t const nDataLen{static_cast< int32_t >(nPos4)};
    readBuff_.ResetBuff(static_cast< uint32_t >(nDataLen));
    std::ignore = _DecodeData(pReadData, nDataLen);
    return nReturn + nOffset;
}
//***************/

/// @brief Interpret the read data
/// @param pBReadData
/// @param nReadLen
/// @return
int32_t PKvUpdateRead_Base::_DecodeData(uint8_t const *const pBReadData, int32_t const nReadLen) noexcept
{
    if (nullptr == pWordEncode_) {
        PER_Assert((readBuff_.size() + static_cast< uint32_t >(nReadLen)) <= readBuff_.GetBuffLen());
        std::ignore = T_Memcpy(readBuff_.data(readBuff_.size()), pBReadData, static_cast< uint32_t >(nReadLen));
        readBuff_.AddDataLen(nReadLen);
    } else {
        PER_Assert((readBuff_.size() + static_cast< uint32_t >(nReadLen) / kInt_2U) <= readBuff_.GetBuffLen());
        uint8_t *const pDataBuffStart{readBuff_.data(0U)};
        int32_t i{0};
        while (i < (nReadLen - 1)) {
            uint8_t const nIndexData{*(pBReadData + i)};
            if (nIndexData == static_cast< uint8_t >(kChar_BraceEnd)) {
                break;
            }
            if (nIndexData == static_cast< uint8_t >(kChar_Comma)) {
                break;
            }
            if (nIndexData == static_cast< uint8_t >(kChar_Enter)) {
                break;
            }
            uint8_t dataCharA{0U};
            uint8_t dataCharB{0U};
            if (nExtChar_ > 0U) {
                dataCharA = nExtChar_;
                dataCharB = nIndexData;
                nExtChar_ = 0U;
                i += 1;
            } else {
                dataCharA = nIndexData;
                dataCharB = *(pBReadData + i + 1);
                i += kInt_2;
            }
            PER_Assert(readBuff_.size() < readBuff_.GetBuffLen());
            uint8_t const nDecodeData{pWordEncode_->Decode(
                dataCharA, dataCharB, (nLineIndex_ + 1) * kInt_0x10 + (static_cast< int32_t >(readBuff_.size()) + 1))};
            *(pDataBuffStart + readBuff_.size()) = nDecodeData;
            readBuff_.AddDataLen(1);
        }
        if (i < nReadLen) {
            nExtChar_ = *(pBReadData + i);
        }
    }
    return nReadLen;
}
//***************/
/// @brief Reset Read result data
void PKvUpdateRead_Base::V_ResetRead() noexcept
{
    readBuff_.SetDataLen(0U);
    nExtChar_      = 0U;
    nDataStartPos_ = 0;
}
//***************/
/// @brief
/// @param nLineIndex
/// @param eUpdateWord
/// @param pWordEncode
PKvUpdateRead_Value::PKvUpdateRead_Value(int32_t const nLineIndex,
                                         EUpdateKeyWord const eUpdateWord,
                                         PWordEncode *const pWordEncode) noexcept
    : PKvUpdateRead_Base{nLineIndex, eUpdateWord, pWordEncode}
{
}
/// @brief
/// @return
inline int32_t PKvUpdateRead_Value::GetReadDataType() const noexcept { return nDataType_; }
/// @brief
/// @return
inline uint32_t PKvUpdateRead_Value::GetReadTotalLen() const noexcept { return nTotalLen_; }
/// @brief
void PKvUpdateRead_Value::ClearReadLen() noexcept { V_ResetRead(); }
//***************/
/// @brief Reset Read result data
void PKvUpdateRead_Value::V_ResetRead() noexcept
{
    PKvUpdateRead_Base::V_ResetRead();
    nDataType_ = 0;
    nTotalLen_ = 0U;
}
/// @brief Interpret the read data
/// @param stReadData
/// @param nDataStart
/// @param nDataEnd
/// @return
int32_t PKvUpdateRead_Value::V_ParseReadData(ara::core::StringView const &stReadData,
                                             ara::core::StringView::size_type &nDataStart,
                                             ara::core::StringView::size_type &nDataEnd) noexcept
{
    std::ignore = nDataStart;
    std::ignore = nDataEnd;
    PER_Assert(false == stReadData.empty());
    ara::core::String stKeyWord;
    stKeyWord += static_cast< char8_t >(GetUpdateWord());
    stKeyWord += "=";
    ara::core::StringView::size_type const nPos1{stReadData.find(std::move(T_StringView(stKeyWord)), 0U)};
    if (nPos1 == ara::core::StringView::npos) {
        return -1;
    }
    ara::core::StringView::size_type const nPos2{stReadData.find(",", nPos1 + kInt_2U)};
    if (nPos2 == ara::core::StringView::npos) {
        return -1;
    }
    ara::core::StringView::size_type const nPos3{stReadData.find(",{", nPos2 + 1U)};
    if (nPos3 == ara::core::StringView::npos) {
        return -1;
    }

    nDataType_ = T_stoi< int32_t >(stReadData.data() + nPos1 + kInt_2U,
                                   static_cast< uint32_t >(nPos2 - nPos1 - kInt_2U), kInt_0x10);
    nTotalLen_
        = T_stoi< uint32_t >(stReadData.data() + nPos2 + 1U, static_cast< uint32_t >(nPos3 - nPos2 - 1U), kInt_10);
    nDataStart = nPos3 + static_cast< std::size_t >(kInt_2);
    nDataEnd   = stReadData.find("},", nDataStart);
    std::size_t nReturn{T_AddInt(nDataEnd, kInt_2)};
    if (nDataEnd == ara::core::StringView::npos) {
        nReturn  = stReadData.size();
        nDataEnd = stReadData.size();
    }
    return static_cast< int32_t >(nReturn);
}
//********************************/
/// @brief
/// @param nLineIndex
/// @param eUpdateWord
/// @param pWordEncode
PKvUpdateRead_String::PKvUpdateRead_String(int32_t const nLineIndex,
                                           EUpdateKeyWord const eUpdateWord,
                                           PWordEncode *const pWordEncode) noexcept
    : PKvUpdateRead_Base{nLineIndex, eUpdateWord, pWordEncode}
{
}
/// @brief Return the read string
/// @return
ara::core::String PKvUpdateRead_String::GetReadString() const
{
    return T_String(GetReadData(), static_cast< uint32_t >(GetReadLen()));
}
/// @brief
/// @param stReadData
/// @param nDataStart
/// @param nDataEnd
/// @return
int32_t PKvUpdateRead_String::V_ParseReadData(ara::core::StringView const &stReadData,
                                              ara::core::StringView::size_type &nDataStart,
                                              ara::core::StringView::size_type &nDataEnd) noexcept
{
    std::ignore = nDataStart;
    std::ignore = nDataEnd;
    PER_Assert(false == stReadData.empty());
    ara::core::String stKeyWord;
    stKeyWord += static_cast< char8_t >(GetUpdateWord());
    stKeyWord += "={";
    ara::core::StringView::size_type nPos1{0U};
    nPos1 = stReadData.find(std::move(T_StringView(stKeyWord)), 0U);
    if (nPos1 == ara::core::StringView::npos) {
        return -1;
    }

    nDataStart = nPos1 + stKeyWord.size();
    nDataEnd   = stReadData.find("},", nDataStart);
    std::size_t nReturn{T_AddInt(nDataEnd, kInt_2)};
    if (nDataEnd == ara::core::StringView::npos) {
        nReturn  = stReadData.size();
        nDataEnd = stReadData.size();
    }
    return static_cast< int32_t >(nReturn);
}
//***************/
/// @brief
/// @param nLineIndex
/// @param eUpdateWord
/// @param pWordEncode
PKvUpdateRead_Action::PKvUpdateRead_Action(int32_t const nLineIndex,
                                           EUpdateKeyWord const eUpdateWord,
                                           PWordEncode *const pWordEncode) noexcept
    : PKvUpdateRead_Base{nLineIndex, eUpdateWord, pWordEncode}
{
}
/// @brief Interpret the read data
/// @param stReadData
/// @param nDataStart
/// @param nDataEnd
/// @return
int32_t PKvUpdateRead_Action::V_ParseReadData(ara::core::StringView const &stReadData,
                                              ara::core::StringView::size_type &nDataStart,
                                              ara::core::StringView::size_type &nDataEnd) noexcept
{
    std::ignore = nDataStart;
    std::ignore = nDataEnd;
    PER_Assert(false == stReadData.empty());
    ara::core::StringView::size_type const nPos1{stReadData.find("[", 0U)};
    if (nPos1 == ara::core::StringView::npos) {
        return -1;
    }

    nDataStart = nPos1 + 1U;
    nDataEnd   = stReadData.find("]:", nDataStart);
    uint32_t nReturn{static_cast< uint32_t >(T_AddInt(nDataEnd, kInt_2))};
    if (nDataEnd == ara::core::StringView::npos) {
        nReturn  = static_cast< uint32_t >(stReadData.size());
        nDataEnd = stReadData.size();
    }
    chUpdateAction_ = stReadData[nDataStart];
    return static_cast< int32_t >(nReturn);
}
//********************************/
}  // namespace isoftkv
}  // namespace per
}  // namespace ara
