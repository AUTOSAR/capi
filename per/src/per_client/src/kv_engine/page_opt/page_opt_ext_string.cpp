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
/// @file       page_opt_ext_string.cpp
/// @brief      AutoSar-AP Data Persistence Storage Module
/// @details    Extended data: initially designed only to save extended version number information as of 2023-08
/// @date       2023-07-12
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/per/KV Engine/Logical Page Interpreter
/// @interface_level=module
/// @trace_id_sr=
/// @unit_name=PPageOpt_ExtString
/// @unit_description=Logical page for parsing data extensions
/// @endcode
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// | 2023-07-12 | 1.0.0   | hanjingjing  | Initial version created                   |
///
/// ================================================================

#include "ara/per/internal/isoftkv/page_opt_ext_string.h"

#include "ara/per/internal/isoftkv/kv_cache_page.h"
#include "ara/per/internal/isoftkv/kv_log_api.h"

namespace {
/// @brief Check if the capacity boundary is exceeded after adding/modifying a string
/// @param eStringType
/// @param stVersion
/// @param pBlockHead
/// @return
bool G_CheckStringSpace(ara::per::isoftkv::EVersionStringType const eStringType,
                        ara::core::StringView const &stVersion,
                        ara::per::isoftkv::PBlockHead_Ext const *const pBlockHead) noexcept
{
    uint32_t nNewTotalLen{0U};
    nNewTotalLen += sizeof(ara::per::isoftkv::PBlockHead_Ext);
    for (int32_t i{0}; i < ara::per::isoftkv::GetExtBlockStringCount(); i++) {
        nNewTotalLen += static_cast< uint32_t >(
            pBlockHead->nStringLen[i]);  // NOLINT [cppcoreguidelines-pro-bounds-constant-array-index]
    }
    nNewTotalLen += static_cast< uint32_t >(stVersion.size());
    int32_t const nStringIndex{static_cast< int32_t >(eStringType)};
    nNewTotalLen -= pBlockHead->nStringLen[nStringIndex];  // NOLINT [cppcoreguidelines-pro-bounds-constant-array-index]
    // Check total capacity
    if (nNewTotalLen > static_cast< uint32_t >(ara::per::isoftkv::EDefaultValue::kMaxExtVersionLenLimit)) {
        return false;
    }
    return true;
}
/// @brief Assemble new Ext data area
/// @param eStringType String type
/// @param stVersion Version number string
/// @param pBlockHeadOld Old ExtBlockHead
/// @param pDataNew Starting address of new cache
/// @return
uint16_t G_AssembleExtString(ara::per::isoftkv::EVersionStringType const eStringType,
                             ara::core::StringView const &stVersion,
                             ara::per::isoftkv::PBlockHead_Ext const *const pBlockHeadOld,
                             uint8_t *const pDataNew,
                             uint32_t nMaxBuffLen) noexcept
{
    ara::per::isoftkv::PBlockHead_Ext *const pBlockHeadNew{
        ara::per::isoftkv::T_TransPtr< ara::per::isoftkv::PBlockHead_Ext >(pDataNew)};
    uint8_t const *const pDataOld{ara::per::isoftkv::T_TransBytes(pBlockHeadOld)};
    uint16_t nIndexOld{static_cast< uint16_t >(sizeof(ara::per::isoftkv::PBlockHead_Ext))};
    uint16_t nIndexNew{static_cast< uint16_t >(sizeof(ara::per::isoftkv::PBlockHead_Ext))};
    for (int32_t i{0}; i < ara::per::isoftkv::GetExtBlockStringCount(); i++) {
        uint8_t const *pDataString{pDataOld + nIndexOld};
        uint16_t nDataLen{static_cast< uint16_t >(
            pBlockHeadOld->nStringLen[i])};  // NOLINT [cppcoreguidelines-pro-bounds-constant-array-index]
        if (i == static_cast< int32_t >(eStringType)) {
            pDataString = ara::per::isoftkv::T_TransBytes(stVersion.data());
            nDataLen    = static_cast< uint32_t >(stVersion.size());
        }
        if ((nIndexNew + nDataLen) >= nMaxBuffLen) {
            return 0U;
        }
        pBlockHeadNew->nStringLen[i]  // NOLINT [cppcoreguidelines-pro-bounds-constant-array-index]
            = static_cast< uint32_t >(nDataLen);
        std::ignore = ara::per::isoftkv::PPageOptBase::MoveData(pDataNew + nIndexNew, pDataString, nDataLen);

        nIndexOld += static_cast< int32_t >(
            pBlockHeadOld->nStringLen[i]);  // NOLINT [cppcoreguidelines-pro-bounds-constant-array-index]
        nIndexNew += static_cast< int32_t >(
            pBlockHeadNew->nStringLen[i]);  // NOLINT [cppcoreguidelines-pro-bounds-constant-array-index]
    }
    return nIndexNew;
}
}  // namespace
namespace ara {
namespace per {
namespace isoftkv {
//********************************/
/// @brief
/// @param pCachePagePtr
PPageOpt_ExtString::PPageOpt_ExtString(PCachePagePtr const &pCachePagePtr, uint16_t const nDefBlockID) noexcept
    : PPageOptSolo{pCachePagePtr, EBlockType::kExt, nDefBlockID}
{
}
/// @brief Get the minimum length of the current page type: when there is no data
/// @return
uint16_t PPageOpt_ExtString::GetMinValidLen() const noexcept
{
    uint32_t nMinValidLen{0U};
    nMinValidLen += sizeof(PPageHeadData);
    nMinValidLen += sizeof(PBlockHead);
    nMinValidLen += sizeof(PBlockHead_Ext);
    return static_cast< uint16_t >(nMinValidLen);
}
//***************/
/// @brief Initialize the data control header of the extension item
/// @return
bool PPageOpt_ExtString::InitExtHead(uint16_t const nCapacityLen) noexcept
{
    bool const bReturn{InitPageHead(PageClassType(), GetMemoryPage()->GetMainPageID())};
    if (false == bReturn) {
        return false;
    }
    uint16_t nMinBlockLen{static_cast< uint16_t >(sizeof(PBlockHead_Ext))};
    nMinBlockLen += nCapacityLen;
    if (false == InitSoloBlock(EStoreRange::kSingle, nMinBlockLen)) {
        return false;
    }
    if (false == IsAccessReady()) {
        PER_OnOptFailed(EErrorPHKV::kNotReadyPageExt);
        return false;
    }
    PBlockHead_Ext *pBlockHeadExt{_GetBlockHeadExt(true)};
    for (int32_t i{0}; i < ara::per::isoftkv::GetExtBlockStringCount(); i++) {
        pBlockHeadExt->nStringLen[i] = 0;  // NOLINT [cppcoreguidelines-pro-bounds-constant-array-index]
    }
    _LogDebug("[KV_Opt].InitValueHead", ":", "PageID", static_cast< int32_t >(GetPageID()));
    _OnOptSuccess(true);
    return bReturn;
}
/// @brief Get the String value of VersionApp within Ext
/// @return
ara::core::String PPageOpt_ExtString::GetVersionApp() const noexcept
{
    return _GetVersionString(EVersionStringType::kVersionString_App);
}
/// @brief Get the String value of VersionPer within Ext
/// @return
ara::core::String PPageOpt_ExtString::GetVersionPer() const noexcept
{
    return _GetVersionString(EVersionStringType::kVersionString_Per);
}
/// @brief Get the String value of VersionSystem within Ext
/// @return
ara::core::String PPageOpt_ExtString::GetVersionSystem() const noexcept
{
    return _GetVersionString(EVersionStringType::kVersionString_System);
}
/// @brief Set the String value of VersionApp within Ext
/// @param stVersion String version number
/// @return
bool PPageOpt_ExtString::SetVersionApp(ara::core::StringView const &stVersion) noexcept
{
    return _SetVersionString(EVersionStringType::kVersionString_App, stVersion);
}
/// @brief Set the String value of VersionPer within Ext
/// @param stVersion String version number
/// @return
bool PPageOpt_ExtString::SetVersionPer(ara::core::StringView const &stVersion) noexcept
{
    return _SetVersionString(EVersionStringType::kVersionString_Per, stVersion);
}
/// @brief Set the String value of VersionSystem within Ext
/// @param stVersion String version number
/// @return
bool PPageOpt_ExtString::SetVersionSystem(ara::core::StringView const &stVersion) noexcept
{
    return _SetVersionString(EVersionStringType::kVersionString_System, stVersion);
}
//********************************/
/// @brief Get BlockHead
/// @param bCheck Whether to execute PER_Assert
/// @return
PBlockHead_Ext *PPageOpt_ExtString::_GetBlockHeadExt(bool const bCheck) const noexcept
{
    PBlockHead const *pBlockHead{_FindBlock(PageClassType(), 0)};
    if (bCheck) {
        PER_Assert(nullptr != pBlockHead);
    }
    uint8_t *const pBlockDatHead{_GetBlockDataHead(pBlockHead)};
    return T_TransPtr< PBlockHead_Ext >(pBlockDatHead);
}
/// @brief Get the String value within Ext
/// @return
ara::core::String PPageOpt_ExtString::_GetVersionString(EVersionStringType const eStringType) const noexcept
{
    PBlockHead_Ext *pBlockHeadExt{_GetBlockHeadExt(true)};
    if (eStringType >= EVersionStringType::kVersionString_Count) {
        return ara::core::String{""};
    }
    uint8_t nStringLen{pBlockHeadExt->nStringLen[static_cast< uint32_t >(eStringType)]};  // NOLINT
    if (nStringLen <= 0) {
        return ara::core::String{""};
    }
    char8_t *pBlockData{T_TransChar< PBlockHead_Ext >(pBlockHeadExt + 1)};
    uint32_t nIndex{0};
    for (uint32_t i{0U}; i < static_cast< uint32_t >(eStringType); i++) {
        nIndex += static_cast< uint32_t >(
            pBlockHeadExt->nStringLen[i]);  // NOLINT [cppcoreguidelines-pro-bounds-constant-array-index]
    }
    return ara::core::String{pBlockData + nIndex, static_cast< std::size_t >(nStringLen)};
}
/// @brief Set the String value within Ext
/// @param eStringType String type
/// @param stData Version string
/// @return Success or failure
bool PPageOpt_ExtString::_SetVersionString(EVersionStringType const eStringType,
                                           ara::core::StringView const &stVersion) noexcept
{
    if (false == IsAccessReady()) {
        PER_OnOptFailed(EErrorPHKV::kNotReadyPageExt);
        return false;
    }
    if (stVersion.empty()) {
        return true;
    }
    PBlockHead const *pBlockHead{_FindBlock(PageClassType(), 0)};
    if (nullptr == pBlockHead) {
        return false;
    }
    uint16_t const nBlockID{pBlockHead->sBlockID};
    // Read all data first, then modify
    PBlockHead_Ext *pBlockHeadExtOld{T_TransPtr< PBlockHead_Ext >(_GetBlockDataHead(pBlockHead))};
    if (false == G_CheckStringSpace(eStringType, stVersion, pBlockHeadExtOld)) {
        PER_OnOptFailed(EErrorPHKV::kPageExtLenLimit);
        return false;
    }
    constexpr uint32_t kBuffMaxLen{static_cast< uint32_t >(sizeof(PBlockHead_Ext))
                                   + static_cast< uint32_t >(EDefaultValue::kMaxExtVersionLenLimit)};
    PER_Assert(kBuffMaxLen == pBlockHead->sLen);
    uint8_t newExtBuff[kBuffMaxLen]{0U};
    uint16_t const nNewLen{
        G_AssembleExtString(eStringType, stVersion, pBlockHeadExtOld, newExtBuff, sizeof(newExtBuff))};
    if (0U == nNewLen) {
        PER_OnOptFailed(EErrorPHKV::kPageExtLenLimit);
        return false;
    }
    // Finally, save again
    if (false == AmendBlock(nBlockID, newExtBuff, pBlockHead->sLen)) {
        PER_OnOptFailed(EErrorPHKV::kPageAddBlock);
        return false;
    }

    _OnOptSuccess(true);
    return true;
}
//********************************/
}  // namespace isoftkv
}  // namespace per
}  // namespace ara
