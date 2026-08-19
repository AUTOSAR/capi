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
/// @file       isoft_kv_debug.cpp
/// @brief      AutoSar-AP Data Persistence Storage Module
/// @details    Puhua KV System: Collection of test functions
/// @date       2023-07-18
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/per/Common/Reusable Features
/// @interface_level=module
/// @trace_id_sr=
/// @unit_name=
/// @unit_description=Convenient for debugging, print relevant information
/// @endcode
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// | 2023-07-18 | 1.0.0   | hanjingjing  | Initial version created                   |
///
/// ================================================================

#include "ara/per/internal/isoftkv/isoft_kv_debug.h"

#include "ara/per/internal/common/isoft_assert.h"
#include "ara/per/internal/common/isoft_common_api.h"
#include "ara/per/internal/common/isoft_select_mofn.h"
#include "ara/per/internal/isoftkv/kv_update_file.h"

namespace {
bool G_IsHaveBlockType(uint8_t nPageType, ara::per::isoftkv::EBlockType eBlockType)
{
    uint8_t nBlockType = static_cast< uint8_t >(eBlockType);
    return (nPageType & nBlockType) == nBlockType;
}
}  // namespace
namespace ara {
namespace per {
namespace isoftkv {
//********************************/
/// @brief Convenient for debugging, print relevant information: PPageOptBase
/// @param consoleLog Print output operation class
/// @param optPage Page operation class
/// @return
bool Debug_PrintPage(PConsoleLog &consoleLog, PPageOptBase const &optPage) noexcept
{
    PPageHeadData const *const pPageHead{optPage.GetPageHead()};
    PER_Assert(nullptr != pPageHead);
    consoleLog.PrintLog();
    consoleLog.PrintLog("<<<< BlockData Begin");
    consoleLog.PrintLog(": PageID = ", pPageHead->nPageID);
    if (false == optPage.CheckBlock()) {
        consoleLog.PrintLog(" [Find Block Data Overlay].");
        return false;
    }
    ara::core::String const stMagic{static_cast< char8_t const * >(pPageHead->cMagic), sizeof(pPageHead->cMagic)};
    consoleLog.PrintLog(", Magic = [", stMagic.data());
    consoleLog.PrintLog("], PageType = ", pPageHead->cPageType);
    consoleLog.PrintLog(", PageLen = ", pPageHead->sPageLen);
    consoleLog.PrintLog(", FreeLen = ", pPageHead->sFreeLen);
    consoleLog.PrintLog(", CountBlock = ", pPageHead->sCountBlock);
    consoleLog.PrintLog();
    consoleLog.PrintLog("  -- BlockData      : |BlockType|BlockID|Offset| Len|");
    consoleLog.PrintLog();

    int32_t nIndex = 0;
    int32_t nScanBlockCount{0};
    nScanBlockCount
        = optPage.ForEachBlock(false, [&consoleLog, &nIndex](PBlockHead const *const pBlockData) noexcept -> int32_t {
              if (nullptr == pBlockData) {
                  return 0;
              }
              consoleLog.PrintLogEx("  --  %4d          : |", nIndex);
              consoleLog.PrintLogEx("     0x%02X|", static_cast< int32_t >(pBlockData->cBlockType));
              consoleLog.PrintLogEx("%7d|", static_cast< int32_t >(pBlockData->sBlockID));
              consoleLog.PrintLogEx("%6d|", static_cast< int32_t >(pBlockData->sOffset));
              consoleLog.PrintLogEx("%4d|", static_cast< int32_t >(pBlockData->sLen));
              consoleLog.PrintLog();
              nIndex += 1;
              return 1;
          });
    consoleLog.PrintLog(">>>> BlockData End  :");
    consoleLog.PrintLogEx(" ScanBlock = %d.", nScanBlockCount);
    return true;
}
/// @brief Convenient for debugging, print relevant information: PPageOptValue
/// @param consoleLog Print output operation class
/// @param optPage Page operation class
/// @return
bool Debug_PrintPage(PConsoleLog &consoleLog, PPageOptValue const &optPage) noexcept
{
    PER_Assert(nullptr != optPage.GetPageHead());
    consoleLog.PrintLog();
    consoleLog.PrintLog("  <<<< Value Begin");
    consoleLog.PrintLog();
    consoleLog.PrintLog("    -- Value Data : |SpreadID|TotalLen|ZoneTotal|ZoneIndex|prevPage.Block|nextPage.Block|");
    consoleLog.PrintLog();
    int32_t nScanCount{0};
    nScanCount
        = optPage.ForEachBlock(false, [&optPage, &consoleLog](PBlockHead const *const pBlockData) noexcept -> int32_t {
              if (nullptr == pBlockData) {
                  return 0;
              }
              PBlockSpread *const pBlockSpread{optPage.GetBlockSpread(*pBlockData)};
              if (static_cast< uint8_t >(optPage.PageClassType()) != pBlockData->cBlockType) {
                  return 0;
              }
              if (nullptr != pBlockSpread) {
                  consoleLog.PrintLog("    --            : |");
                  consoleLog.PrintLogEx("%8u|", pBlockSpread->nSpreadID);
                  consoleLog.PrintLogEx("%8u|", pBlockSpread->nTotalLen);
                  consoleLog.PrintLogEx("%9u|", pBlockSpread->nZoneTotal);
                  consoleLog.PrintLogEx("%9u|", pBlockSpread->nZoneIndex);
                  consoleLog.PrintLogEx("%8u|", pBlockSpread->prevBlock.nPageID);
                  consoleLog.PrintLogEx(".%5u|", pBlockSpread->prevBlock.sBlockID);
                  consoleLog.PrintLogEx("%8u|", pBlockSpread->nextBlock.nPageID);
                  consoleLog.PrintLogEx(".%5u|", pBlockSpread->nextBlock.sBlockID);
              } else {
                  consoleLog.PrintLog("    --            : |Local Data|");
              }
              consoleLog.PrintLog();
              return 1;
          });
    consoleLog.PrintLog("  >>>> Value End   :");
    consoleLog.PrintLogEx(" ScanCount = %d.", nScanCount);
    return true;
}
/// @brief Print Hash table
/// @param consoleLog Print output operation class
/// @param optPage Page operation class
/// @return Whether successful
bool Debug_PrintHash(PConsoleLog &consoleLog, PPageOptKeyHash const &optPage) noexcept
{
    PBlockHeadKeyHash const *const pHashHead{optPage.GetHeadHash()};
    if (nullptr == pHashHead) {
        return false;
    }
    consoleLog.PrintLog("[PH_Debug].PrintHash : ");
    consoleLog.PrintLog("ElementCount", static_cast< int32_t >(pHashHead->sElementCount));
    consoleLog.PrintLog("KeyPageCount", static_cast< int32_t >(pHashHead->nKvPageCount));
    consoleLog.PrintLog();
    ara::core::String stPrintHash;
    int32_t nScanCount{0};
    nScanCount = optPage.ForEachHash(false, [&stPrintHash](int32_t const nIndex, uint32_t const nPageID) -> int32_t {
        std::ignore = nPageID;
        uint8_t chBuff[kInt_64]{0U};
        std::ignore = sprintf(T_TransChar(static_cast< uint8_t * >(chBuff)), "|%03d", nIndex);
        if (chBuff[0] != 0U) {  // This output parameter value is not subsequently checked.
            chBuff[kInt_64 - 1] = 0U;
            stPrintHash += ara::core::String{T_TransChar(static_cast< uint8_t * >(chBuff))};
        }
        return 1;
    });
    consoleLog.PrintLog("[PH_Debug].Hash : ", stPrintHash.c_str());
    consoleLog.PrintLog("| Count", nScanCount);
    consoleLog.PrintLog();
    ara::core::String stPrintData;
    nScanCount = optPage.ForEachHash(false, [&stPrintData](int32_t const nIndex, uint32_t const nPageID) -> int32_t {
        std::ignore = nIndex;
        uint8_t chBuff[kInt_64]{0U};
        std::ignore = sprintf(T_TransChar(static_cast< uint8_t * >(chBuff)), "|%03d", nPageID);
        if (chBuff[0] != 0U) {  // This output parameter value is not subsequently checked.
            chBuff[kInt_64 - 1] = 0U;
            stPrintData += ara::core::String{T_TransChar(static_cast< uint8_t * >(chBuff))};
        }
        return 1;
    });
    consoleLog.PrintLog("[PH_Debug].Data : ", stPrintData.c_str());
    consoleLog.PrintLog("| Count", nScanCount);
    consoleLog.PrintLog();
    return true;
}
/// @brief Convenient for debugging, print relevant information: PPageOptKeyHash
/// @param consoleLog Print output operation class
/// @param optPage Page operation class
/// @return Whether successful
bool Debug_PrintPage(PConsoleLog &consoleLog, PPageOptKeyHash const &optPage) noexcept
{
    PER_Assert(nullptr != optPage.GetPageHead());
    consoleLog.PrintLog();
    PBlockHeadKeyHash const *const pHeadWork{optPage.GetHeadHash()};
    consoleLog.PrintLog("  <<<< KeyHash Begin");
    consoleLog.PrintLog(": SoloBlockID = ", optPage.GetSoloBlockID());
    consoleLog.PrintLog(", KvPageCount = ", pHeadWork->nKvPageCount);
    consoleLog.PrintLog(", HashMaxCapacity = ", pHeadWork->sHashMaxCapacity);
    consoleLog.PrintLog(", HashType = ", pHeadWork->cHashType);
    consoleLog.PrintLog(", CompressType = ", pHeadWork->cCompressType);
    consoleLog.PrintLog(", HashInitCount = ", pHeadWork->cHashInitCount);
    consoleLog.PrintLog(", ElementCount = ", pHeadWork->sElementCount);
    consoleLog.PrintLog();
    ara::core::String stPrintHash;
    int32_t nScanCount{0};
    nScanCount = optPage.ForEachHash(false, [&stPrintHash](int32_t const nIndex, uint32_t const nPageID) -> int32_t {
        std::ignore = nPageID;
        uint8_t chBuff[kInt_64]{0U};
        std::ignore = sprintf(T_TransChar(static_cast< uint8_t * >(chBuff)), "|%4d", nIndex);
        if (chBuff[0] != 0U) {  // This output parameter value is not subsequently checked.
            chBuff[kInt_64 - 1] = 0U;
            stPrintHash += ara::core::String{T_TransChar(static_cast< uint8_t * >(chBuff))};
        }
        return 1;
    });
    consoleLog.PrintLog("    -- KeyHash Index : ", stPrintHash.c_str());
    consoleLog.PrintLogEx("|Count = %d|", nScanCount);
    consoleLog.PrintLog();
    ara::core::String stPrintData{};
    nScanCount = optPage.ForEachHash(false, [&stPrintData](int32_t const nIndex, uint32_t const nPageID) -> int32_t {
        std::ignore = nIndex;
        uint8_t chBuff[kInt_64]{0U};
        std::ignore = sprintf(T_TransChar(static_cast< uint8_t * >(chBuff)), "|%4d", nPageID);
        if (chBuff[0] != 0U) {  // This output parameter value is not subsequently checked.
            chBuff[kInt_64 - 1] = 0U;
            stPrintData += ara::core::String{T_TransChar(static_cast< uint8_t * >(chBuff))};
        }
        return 1;
    });
    consoleLog.PrintLog("    -- KeyHash Data  : ", stPrintData.c_str());
    consoleLog.PrintLogEx("|Count = %d|", nScanCount);
    consoleLog.PrintLog();
    consoleLog.PrintLog("  >>>> KeyHash End   .");
    return true;
}
/// @brief Convenient for debugging, print relevant information: PPageOptPageGroup
/// @param consoleLog Print output operation class
/// @param optPage Page operation class
/// @return
bool Debug_PrintPage(PConsoleLog &consoleLog, PPageOptPageGroup const &optPage) noexcept
{
    PER_Assert(nullptr != optPage.GetPageHead());
    consoleLog.PrintLog();
    PBlockHeadPageGroup *const pHeadWork{optPage.GetHeadGroup()};
    consoleLog.PrintLog("  <<<< PageGroup Begin");
    consoleLog.PrintLog(": SoloBlockID = ", optPage.GetSoloBlockID());
    consoleLog.PrintLog(", versionSystem = ", FormatVersion(pHeadWork->versionSystem).c_str());
    consoleLog.PrintLog(", versionPer = ", FormatVersion(pHeadWork->versionPer).c_str());
    consoleLog.PrintLog(", SpaceAmountMin = ", pHeadWork->nSpaceAmountMin);
    consoleLog.PrintLog(", SpaceAmountMax = ", pHeadWork->nSpaceAmountMax);
    consoleLog.PrintLog(", HashPageID = ", pHeadWork->nHashPageID);
    consoleLog.PrintLog(", GroupCapacity = ", pHeadWork->sGroupCapacity);
    consoleLog.PrintLog(", PageInitLen = ", pHeadWork->sPageInitLen);
    consoleLog.PrintLog(", PageTotal = ", pHeadWork->nPageTotal);
    consoleLog.PrintLog(", ElementCount = ", pHeadWork->sElementCount);
    consoleLog.PrintLog();
    consoleLog.PrintLog("    -- PageGroup Data : |BossPageID|FreePageCount|MaxFreeIndex|");
    consoleLog.PrintLog();
    for (int32_t i{0}; i < static_cast< int32_t >(pHeadWork->sElementCount); i++) {
        PBlockElementGroup &elementData{
            pHeadWork->arrayElement[i]};  // NOLINT [cppcoreguidelines-pro-bounds-constant-array-index]
        consoleLog.PrintLog("    --                : |");
        consoleLog.PrintLogEx("%10d|", static_cast< int32_t >(elementData.sBossPageIndex));
        consoleLog.PrintLogEx("%13d|", static_cast< int32_t >(elementData.sFreePageCount));
        consoleLog.PrintLogEx("%12d|", static_cast< int32_t >(elementData.sMaxFreeIndex));
        consoleLog.PrintLog();
    }
    consoleLog.PrintLog("  >>>> PageGroup End  .");
    return true;
}
/// @brief Convenient for debugging, print relevant information: PPageOptKvStore
/// @param consoleLog Print output operation class
/// @param optPage Page operation class
/// @return
bool Debug_PrintPage(PConsoleLog &consoleLog, PPageOptKvStore const &optPage) noexcept
{
    PER_Assert(nullptr != optPage.GetBlockHead());
    consoleLog.PrintLog();
    PBlockHeadKvStore const *const pHeadWork{optPage.GetHeadKvStorage()};
    consoleLog.PrintLog("  <<<< KvStore Begin");
    consoleLog.PrintLog(": SoloBlockID = ", optPage.GetSoloBlockID());
    consoleLog.PrintLog(", KvStoreID = ", pHeadWork->nKvStoreID);
    consoleLog.PrintLog(", KvTotal = ", pHeadWork->nKvTotal);
    consoleLog.PrintLog(", LastKvPageID = ", pHeadWork->nLastKvPageID);
    consoleLog.PrintLog(", InitLen = ", pHeadWork->sInitLen);
    consoleLog.PrintLog(", FreeLen = ", pHeadWork->sFreeLen);
    consoleLog.PrintLog(", PrevPageID = ", pHeadWork->nPrevPageID);
    consoleLog.PrintLog(", NextPageID = ", pHeadWork->nNextPageID);
    consoleLog.PrintLog(", ElementCount = ", pHeadWork->sElementCount);
    consoleLog.PrintLog();
    consoleLog.PrintLog(
        "    -- KvStore Data : |KvDataBase    |Offset| "
        "Len|KeyType|KeyLen|KeyBlockPage|KeyBlockID|ValueType|ValueLen|ValueBlockPage|ValueBlockID|ReddType|ReddLen|"
        "ReddBlockPage|ReddBlockID|");
    consoleLog.PrintLog();

    PPageOptKvStore::CB_EachKvIntConst pfunWork;
    pfunWork = [&consoleLog, &optPage](int32_t const nKvIndex, PBlockElementKv const *const pKvElement,
                                       PElementKvData const *const pKvData) noexcept -> int32_t {
        consoleLog.PrintLogEx("    --  %4d        : |", nKvIndex);
        consoleLog.PrintLogEx("%p|", static_cast< void const * >(pKvData));
        consoleLog.PrintLogEx("%6d|", pKvElement->sOffset);
        consoleLog.PrintLogEx("%4d|", pKvElement->cLen);
        // Key
        {
            PBlockIndex *pBlockIndex{nullptr};
            PDataLocal *pLocalValue{nullptr};
            if (optPage.GetKvLocalData(EDataLogic::kLogicKey, nKvIndex, pBlockIndex, pLocalValue)) {
                if (nullptr != pLocalValue) {
                    consoleLog.PrintLogEx("%7d|", pLocalValue->cDataType);
                    consoleLog.PrintLogEx("%6d|", pLocalValue->cDataLen);
                    consoleLog.PrintLog("------------|");
                    consoleLog.PrintLog("----------|");
                }
                if (nullptr != pBlockIndex) {
                    consoleLog.PrintLog("-------|");
                    consoleLog.PrintLog("------|");
                    consoleLog.PrintLogEx("%12d|", pBlockIndex->nPageID);
                    consoleLog.PrintLogEx("%10d|", pBlockIndex->sBlockID);
                }
            }
        }
        // Value
        {
            PBlockIndex *pBlockIndex{nullptr};
            PDataLocal *pLocalValue{nullptr};
            if (optPage.GetKvLocalData(EDataLogic::kLogicValue, nKvIndex, pBlockIndex, pLocalValue)) {
                if (nullptr != pLocalValue) {
                    consoleLog.PrintLogEx("%9d|", pLocalValue->cDataType);
                    consoleLog.PrintLogEx("%8d|", pLocalValue->cDataLen);
                    consoleLog.PrintLog("--------------|");
                    consoleLog.PrintLog("------------|");
                }
                if (nullptr != pBlockIndex) {
                    consoleLog.PrintLog("---------|");
                    consoleLog.PrintLog("--------|");
                    consoleLog.PrintLogEx("%14d|", pBlockIndex->nPageID);
                    consoleLog.PrintLogEx("%12d|", pBlockIndex->sBlockID);
                }
            }
        }
        // Redd
        {
            PBlockIndex *pBlockIndex{nullptr};
            PDataLocal *pLocalValue{nullptr};
            if (optPage.GetKvLocalData(EDataLogic::kLogicRedd, nKvIndex, pBlockIndex, pLocalValue)) {
                if (nullptr != pLocalValue) {
                    consoleLog.PrintLogEx("%8d|", pLocalValue->cDataType);
                    consoleLog.PrintLogEx("%7d|", pLocalValue->cDataLen);
                    consoleLog.PrintLog("-------------|");
                    consoleLog.PrintLog("-----------|");
                }
                if (nullptr != pBlockIndex) {
                    consoleLog.PrintLog("--------|");
                    consoleLog.PrintLog("-------|");
                    consoleLog.PrintLogEx("%13d|", pBlockIndex->nPageID);
                    consoleLog.PrintLogEx("%11d|", pBlockIndex->sBlockID);
                }
            }
        }
        consoleLog.PrintLog();
        return 1;
    };
    int32_t nCount{0};
    nCount = optPage.ForEachKv(false, pfunWork);
    consoleLog.PrintLog("  >>>> KvStore End  :");
    consoleLog.PrintLogEx(" ScanCount = %d.", nCount);
    return true;
}
/// @brief Convenient for debugging, print relevant information: PPageOptPageBoss
/// @param consoleLog Print output operation class
/// @param optPage Page operation class
/// @return
bool Debug_PrintPage(PConsoleLog &consoleLog, PPageOptPageBoss const &optPage) noexcept
{
    PER_Assert(nullptr != optPage.GetPageHead());
    consoleLog.PrintLog();
    PBlockHeadPageBoss *const pHeadWork{optPage.GetHeadBoss()};
    consoleLog.PrintLog("  <<<< PageBoss Begin");
    consoleLog.PrintLog(": SoloBlockID = ", optPage.GetSoloBlockID());
    consoleLog.PrintLog(", GroupID = ", pHeadWork->sGroupID);
    consoleLog.PrintLog(", PageCountMax = ", pHeadWork->sPageCountMax);
    consoleLog.PrintLog(", FreeListCount = ", pHeadWork->sFreeListCount);
    consoleLog.PrintLog(", ElementCount = ", pHeadWork->sElementCount);
    consoleLog.PrintLog();
    consoleLog.PrintLog("    -- FreeList Data : |");
    for (uint16_t const nPageID : pHeadWork->sFreeList) {
        consoleLog.PrintLogEx("%d|", static_cast< int32_t >(nPageID));
    }
    consoleLog.PrintLog();
    consoleLog.PrintLog("    -- PageBoss Data : |PrevPageID|NextPageID|");
    consoleLog.PrintLog();
    for (int32_t i{0}; i < static_cast< int32_t >(pHeadWork->sElementCount); i++) {
        PBlockElementFreePage const &elementData{
            pHeadWork->arrayElement[i]};  // NOLINT [cppcoreguidelines-pro-bounds-constant-array-index]
        consoleLog.PrintLog("    --               : |");
        consoleLog.PrintLogEx("%10d|", elementData.sPrevPageID);
        consoleLog.PrintLogEx("%10d|", elementData.sNextPageID);
        consoleLog.PrintLog();
    }
    consoleLog.PrintLog("  >>>> PageBoss End   .");
    return true;
}
//********************************/
/// @brief Traverse all pages and print relevant information
void Debug_PrintPage(PKvSystem const &kvSystem, uint8_t nPrintType) noexcept
{
    uint32_t const nPageTotal{kvSystem.GetPageTotal()};
    for (uint32_t i{0U}; i < nPageTotal; i++) {
        uint32_t const nPageID{i + 1U};
        Debug_PrintPage(kvSystem, nPageID, nPrintType);
    }
}
/// @brief Traverse all pages and print BLOCK information
/// @param kvSystem
void Debug_PrintPage(PKvSystem const &kvSystem, uint32_t nPageID, uint8_t nPrintType) noexcept
{
    PConsoleLog consoleLog{"Debug_PrintPage :"};
    consoleLog.PrintLog("nPageID = ", nPageID);
    PCachePagePtr const pMemPage{kvSystem.LoadPageData(nPageID)};
    PPageOptBase const pageOptBase{pMemPage};
    if (false == pageOptBase.IsAccessReady()) {
        consoleLog.PrintLogEx("Find Error When Load Page [%u]", nPageID);
        return;
    }
    std::ignore = Debug_PrintPage(consoleLog, pageOptBase);
    if (G_IsHaveBlockType(nPrintType, EBlockType::kPageGroup)) {
        if (pageOptBase.IsHavePageType(static_cast< uint8_t >(EBlockType::kPageGroup))) {
            PPageOptPageGroup const pageOptGroup{pMemPage};
            std::ignore = Debug_PrintPage(consoleLog, pageOptGroup);
        }
    }
    if (G_IsHaveBlockType(nPrintType, EBlockType::kPageBoss)) {
        if (pageOptBase.IsHavePageType(static_cast< uint8_t >(EBlockType::kPageBoss))) {
            PPageOptPageBoss const pageOptBoss{pMemPage};
            std::ignore = Debug_PrintPage(consoleLog, pageOptBoss);
        }
    }
    if (G_IsHaveBlockType(nPrintType, EBlockType::kKeyHash)) {
        if (pageOptBase.IsHavePageType(static_cast< uint8_t >(EBlockType::kKeyHash))) {
            PPageOptKeyHash const pageKeyHash{pMemPage};
            std::ignore = Debug_PrintPage(consoleLog, pageKeyHash);
        }
    }
    if (G_IsHaveBlockType(nPrintType, EBlockType::kKvStore)) {
        if (pageOptBase.IsHavePageType(static_cast< uint8_t >(EBlockType::kKvStore))) {
            PPageOptKvStore const pageOptKv{pMemPage};
            std::ignore = Debug_PrintPage(consoleLog, pageOptKv);
        }
    }
    if (G_IsHaveBlockType(nPrintType, EBlockType::kValue)) {
        if (pageOptBase.IsHavePageType(static_cast< uint8_t >(EBlockType::kValue))) {
            PPageOptValue const pageOptValue{pMemPage};
            std::ignore = Debug_PrintPage(consoleLog, pageOptValue);
        }
    }
    return;
}
/// @brief Print all Keys
/// @param kvSystem
void Debug_PrintAllKey(PKvSystem &kvSystem) noexcept
{
    PConsoleLog consoleLog{"Debug_PrintAllKey :"};
    ara::core::Vector< ara::core::String > vecKeyAll;
    if (false == kvSystem.EnumAllKey(vecKeyAll)) {
        return;
    }
    consoleLog.PrintLog("Key.Count = ", static_cast< int32_t >(vecKeyAll.size()));
    for (auto &key : vecKeyAll) {
        consoleLog.PrintLog("Key.Data = ", key.c_str());
    }
}
void PrintKvSystem(ara::core::String const &stWorkPath,
                   ara::core::String const &stKvName,
                   std::function< bool(PKvSystem &kvSystem, ara::core::String const &stKey) > const &func)
{
    PConfigMuster_Kv configKv;
    configKv.SetWorkPath(stWorkPath);
    configKv.SetStorageName(stKvName);
    configKv.SetSpaceAmountMin(kInt64_4096U);
    configKv.SetSpaceAmountMax(kInt64_10240000U);
    configKv.SetConfigValid(true);
    PKvSystem kvSystem{configKv};
    return PrintKvSystem(kvSystem, func);
}
void PrintKvSystem(PKvSystem &kvSystem,
                   std::function< bool(PKvSystem &kvSystem, ara::core::String const &stKey) > const &func)
{
    ara::core::String stKvStorageName = kvSystem.GetSystemName();
    printf("--------Operate KV library[%s].PrintKvSystem--------\n", stKvStorageName.c_str());
    if (false == kvSystem.OpenSystem()) {
        printf("Failed: Operate KV library[%s].OpenSystem\n", stKvStorageName.c_str());
        return;
    }
    ara::core::String stVersionApp = kvSystem.GetVersion_StorageApp();
    printf("-------- : stVersionApp = %s\n", stVersionApp.c_str());
    ara::core::String stVersionPer = kvSystem.GetVersion_StoragePer();
    printf("-------- : stVersionPer = %s\n", stVersionPer.c_str());
    ara::core::String stVersionSys = kvSystem.GetVersion_StorageSys();
    printf("-------- : stVersionSys = %s\n", stVersionSys.c_str());
    ara::core::Vector< ara::core::String > vecKeyName;
    if (false == kvSystem.EnumAllKey(vecKeyName)) {
        printf("Failed: Operate KV library[%s].EnumAllKey\n", stKvStorageName.c_str());
        return;
    }
    printf("-------- : EnumAllKey.Count = %lu\n", vecKeyName.size());
    Debug_PrintPage(kvSystem, static_cast< uint8_t >(EBlockType::kKvStore));
    for (auto &stKey : vecKeyName) {
        // auto nReadLen = kvSystem.GetValueLen(stKey);
        // printf("Prompt: Operate KV library[%s].ReadValue, stKey = %-65s, nReadLen = %02d\n", stKvStorageName.c_str(),
        // stKey.c_str(),
        //        nReadLen);
        if (func) {
            if (false == func(kvSystem, stKey)) {
                break;
            }
        }
    }
}
/// @brief Print the content of KvSystem
/// @param kvSystem
void PrintKvSystem(PKvSystem &kvSystemWork)
{
    int nIndex = 0;
    PrintKvSystem(kvSystemWork, [&nIndex](PKvSystem &kvSystem, ara::core::String const &stKey) -> bool {
        auto nReadLen = kvSystem.GetValueLen(stKey);
        if (kvSystem.IsHaveError()) {
            printf("-------- : [%s].ReadValue[%02d], stKey = %-65s, ErrorID = %2d\n", kvSystem.GetSystemName().c_str(),
                   nIndex, stKey.c_str(), static_cast< int32_t >(kvSystem.GetLastError()));
        } else {
            printf("-------- : [%s].ReadValue[%02d], stKey = %-65s, nReadLen = %02d\n",
                   kvSystem.GetSystemName().c_str(), nIndex, stKey.c_str(), nReadLen);
        }
        nIndex += 1;
        return true;
    });
}
/// @brief  Print the UpdateFile of the KV library, output version information and Key values but not Value values, and perform CRC check
/// @param stFileName
/// @return
bool Debug_PrintUpdateFile(ara::core::StringView const &stFileName)
{
    // PFileOpt fileOpt{PFileOpt::ReadOnly()};
    // if( false == fileOpt.DoPrepareWork(T_StringView(stFileName)) )
    // {
    //     printf( "Cannot open file[%s]", stFileName.c_str() );
    //     return false;
    // }
    PKvUpdateFile updateFile{T_StringView("DebugKv"), stFileName, true};
    if (false == updateFile.IsAccessReady()) {
        printf("[Debug].PrintUpdateFile: Not Find UpdateFile [%s]", stFileName.data());
        return false;
    }
    constexpr uint32_t kPageLen = 4096;
    uint32_t nCompareCrc        = 0;
    int32_t nSuccess            = 0;
    int32_t nLoadKvTotal        = updateFile.ScanAllLine(
        false, kPageLen,
        [&nCompareCrc, &nSuccess](int32_t const nLineID,
                                  PKvUpdateRead_Base const *const pReadUpdate) noexcept -> int32_t {
            EUpdateKeyWord const eUpdateWord{pReadUpdate->GetUpdateWord()};
            uint8_t const *const pBRead{pReadUpdate->GetReadData()};
            uint32_t const nReadLen{static_cast< uint32_t >(pReadUpdate->GetReadLen())};
            switch (eUpdateWord) {
                case EUpdateKeyWord::kAction: {
                    printf("  --  LineID=[%d], Action=[%s]", nLineID, T_String(pBRead, nReadLen).c_str());
                    nCompareCrc = 0;
                } break;
                case EUpdateKeyWord::kKey: {
                    printf(", Key=[%65s]", T_String(pBRead, nReadLen).c_str());
                    nCompareCrc = CalculateCrc32(pBRead, nReadLen, nCompareCrc);
                } break;
                case EUpdateKeyWord::kValue: {
                    if (nLineID <= 2) {
                        printf(", Value=[%s]", T_String(pBRead, nReadLen).c_str());
                    } else {
                        printf(", ValueLen=[%u]", nReadLen);
                    }
                    nCompareCrc = CalculateCrc32(pBRead, nReadLen, nCompareCrc);
                } break;
                case EUpdateKeyWord::kCrc: {
                    uint32_t nReadCrc32 = isoftkv::T_stoi< uint32_t >(pBRead, nReadLen, kInt_10);
                    if (nReadCrc32 != nCompareCrc) {
                        printf(", ReadCrc[%u] != ComputeCrc[%u]", nReadCrc32, nCompareCrc);
                    } else {
                        nSuccess += 1;
                    }
                    printf("\n");
                } break;
            }
            return static_cast< int32_t >(nReadLen);
        });
    // Save version number
    printf("[Debug].PrintUpdateFile : LoadTotal = %d, Success = %d\n", nLoadKvTotal, nSuccess);
    return true;
}
//********************************/
}  // namespace isoftkv
}  // namespace per
}  // namespace ara
