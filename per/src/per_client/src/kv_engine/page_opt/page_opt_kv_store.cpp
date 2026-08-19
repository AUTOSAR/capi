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
/// @file       page_opt_kv_store.cpp
/// @brief      AutoSar-AP Data Persistence Storage Module
/// @details    Key-Value storage page: Key hash management page
/// @date       2021-05-19
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/per/KV Engine/Logical Page Interpreter
/// @interface_level=module
/// @trace_id_sr=
/// @unit_name=PPageOptKvStore
/// @unit_description=Logical page for parsing data storage
/// @endcode
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// | 2021-05-19 | 1.0.0   | hanjingjing  | Initial version created                   |
///
/// ================================================================

#include "ara/per/internal/isoftkv/page_opt_kv_store.h"

#include "ara/core/map.h"
#include "ara/per/internal/common/isoft_common_api.h"
#include "ara/per/internal/common/isoft_select_mofn.h"
#include "ara/per/internal/isoftkv/kv_cache_page.h"
#include "ara/per/internal/isoftkv/kv_log_api.h"

namespace {
/// @brief Check if the Kv index is valid
/// @param kvBlockHead
/// @param nIndex
/// @return
bool G_CheckArrayIndex(ara::per::isoftkv::PBlockHeadKvStore const &kvBlockHead, int32_t const nIndex) noexcept
{
    return (nIndex >= 0) && (nIndex < static_cast< int32_t >(kvBlockHead.sElementCount));
}
/// @brief Get the real length of the KV header
/// @return
uint16_t G_GetKvHeadSize() noexcept { return static_cast< uint16_t >(sizeof(ara::per::isoftkv::PBlockHeadKvStore)); }
/// @brief Return data offset relative to the start address of KvBlockData
/// @param pKvHead
/// @param nOffset
/// @return
ara::per::isoftkv::PElementKvData *G_GetKvDataByOffset(ara::per::isoftkv::PBlockHeadKvStore *const pKvHead,
                                                       int32_t const nOffset) noexcept
{
    uint8_t *const pBase{ara::per::isoftkv::T_TransBytes(pKvHead)};
    return ara::per::isoftkv::T_TransPtr< ara::per::isoftkv::PElementKvData >(pBase + nOffset);
}
/// @brief Find the page management data area
/// @param pBlockHead
/// @param nElementIndex
/// @return
ara::per::isoftkv::PBlockElementKv *G_GetBlockElement(ara::per::isoftkv::PBlockHeadKvStore *const pBlockHead,
                                                      int32_t const nElementIndex) noexcept
{
    PER_Assert(nullptr != pBlockHead);
    ara::per::isoftkv::PBlockElementKv *const pArrayElement{
        ara::per::isoftkv::T_TransPtr< ara::per::isoftkv::PBlockElementKv >(
            ara::per::isoftkv::T_TransBytes(pBlockHead + 1))};
    return pArrayElement + nElementIndex;
}
/// @brief Iterate over all KV pairs
/// @param pBlockHead
/// @param bFromBack
/// @param pfun
/// @return
int32_t G_ForEachKv(ara::per::isoftkv::PBlockHeadKvStore *const pBlockHead,
                    bool const bFromBack,
                    ara::per::isoftkv::PPageOptKvStore::CB_EachKvInt const &pfun) noexcept
{
    PER_Assert(nullptr != pBlockHead);
    int32_t nReturn{0};
    nReturn = 0;
    for (int32_t i{0}; i < static_cast< int32_t >(pBlockHead->sElementCount); i++) {
        int32_t nIndex{i};
        if (bFromBack) {
            nIndex = static_cast< int32_t >(pBlockHead->sElementCount) - i - 1;
        }
        ara::per::isoftkv::PBlockElementKv *pKvElement{nullptr};
        pKvElement = G_GetBlockElement(pBlockHead, nIndex);
        ara::per::isoftkv::PElementKvData *pKvData{nullptr};
        pKvData = G_GetKvDataByOffset(pBlockHead, static_cast< int32_t >(pKvElement->sOffset));
        int32_t nFunCode{0};
        nFunCode = pfun(nIndex, pKvElement, pKvData);
        if (nFunCode < 0) {
            break;
        }
        if (nFunCode == 0) {
            continue;
        }
        nReturn += 1;
    }
    return nReturn;
}
/// @brief Initialize the KV storage Block header
/// @param pKvHead
/// @param nInitLen
/// @param nKvStoreID
/// @param nLastKvPageID
void G_InitKvHead(ara::per::isoftkv::PBlockHeadKvStore *const pKvHead,
                  uint16_t const nInitLen,
                  uint32_t const nKvStoreID,
                  uint32_t const nLastKvPageID) noexcept
{
    PER_Assert(nullptr != pKvHead);
    pKvHead->nKvStoreID    = nKvStoreID;
    pKvHead->nKvTotal      = 0U;
    pKvHead->nLastKvPageID = nLastKvPageID;
    pKvHead->sInitLen      = static_cast< uint16_t >(nInitLen);
    pKvHead->sFreeLen      = static_cast< uint16_t >(pKvHead->sInitLen - G_GetKvHeadSize());
    pKvHead->sElementCount = 0U;
    pKvHead->nPrevPageID   = 0U;
    pKvHead->nNextPageID   = 0U;
    std::ignore            = memset(static_cast< void * >(pKvHead->cReserve), 0, sizeof(pKvHead->cReserve));
    std::ignore            = memset(T_TransBytes(pKvHead + 1), 0, static_cast< std::size_t >(pKvHead->sFreeLen));
}
/// @brief Convert a 64-bit hash value to a 32-bit value
/// @param nSrcKeyHash
/// @return
inline uint32_t G_GetKeyHashID(uint64_t const nSrcKeyHash) noexcept
{
    return static_cast< uint32_t >(nSrcKeyHash % ara::per::kInt_0x100000000U);
}
/// @brief Verify whether the KV storage area can accommodate a new KV
/// @param kvBlockHead
/// @param nAddLen
/// @return
inline bool G_CheckKvStoreCapacity(ara::per::isoftkv::PBlockHeadKvStore const &kvBlockHead,
                                   uint16_t const nAddLen) noexcept
{
    return kvBlockHead.sFreeLen >= nAddLen;
}
/// @brief Calculate the real length of KV data
/// @param pKvHead
/// @param nKvDataLen
/// @return
int32_t G_ComputePlanAddLen(ara::per::isoftkv::PBlockHeadKvStore *const pKvHead, int32_t const nKvDataLen) noexcept
{
    std::ignore = pKvHead;
    return static_cast< int32_t >(sizeof(ara::per::isoftkv::PBlockElementKv)) + nKvDataLen;
}

}  // namespace
namespace ara {
namespace per {
namespace isoftkv {
//********************************/
/// @brief KV data operation class
class POptKvData
{
private:
    /// @brief
    PPageOptKvStore const *pOptPageKv_;
    /// @brief
    PBlockElementKv *pKvElement_;
    /// @brief
    PElementKvData *pKvData_;

protected:
    /// @brief
    /// @return
    inline PElementKvData *_GetKvData() const noexcept { return pKvData_; }

public:
    /// @brief
    /// @param pOptPageKv
    /// @param nKvIndex
    POptKvData(PPageOptKvStore const *const pOptPageKv, int32_t const nKvIndex) noexcept;
    /// @brief
    /// @param pOptPageKv
    /// @param pKvElement
    POptKvData(PPageOptKvStore const *const pOptPageKv, PBlockElementKv *const pKvElement) noexcept;
    /// @brief
    /// @param pOptPageKv
    /// @param pKvElement
    /// @param pKvData
    POptKvData(PPageOptKvStore const *const pOptPageKv,
               PBlockElementKv *const pKvElement,
               PElementKvData *const pKvData) noexcept;
    /// @brief
    POptKvData() = delete;
    /// @brief
    virtual ~POptKvData() = default;
    /// @brief
    /// @param a
    POptKvData(POptKvData const &a) = delete;
    /// @brief
    /// @param a
    /// @return
    POptKvData &operator=(POptKvData const &a) = delete;
    /// @brief
    /// @param a
    POptKvData(POptKvData &&a) = delete;
    /// @brief
    /// @param a
    /// @return
    POptKvData &operator=(POptKvData &&a) = delete;
    /// @brief
    /// @return
    bool IsAccessReady() const noexcept;
    /// @brief
    /// @return
    inline PBlockElementKv *GetKvElement() const noexcept { return pKvElement_; }
    /// @brief
    /// @return
    inline PElementKvData *GetKvData() const noexcept { return pKvData_; }
    /// @brief Calculate the CRC16 code of the KV-Element
    /// @return
    uint16_t CalKvCRC16() const noexcept;
    /// @brief Update CRC16
    /// @return
    bool UpdateCrc() const noexcept;
    /// @brief Verify CRC16
    /// @return
    bool CheckCrc() const noexcept;
    /// @brief Determine the data type of the Key
    /// @param eDataFormat
    /// @return
    bool IsKeyFormat(EDataFormat const eDataFormat) const noexcept;
    /// @brief Determine the data type of the Value
    /// @param eDataFormat
    /// @return
    bool IsValueFormat(EDataFormat const eDataFormat) const noexcept;
    /// @brief Determine the data type of the Redd
    /// @param eDataFormat
    /// @return
    bool IsReddFormat(EDataFormat const eDataFormat) const noexcept;
    /// @brief Replace the entire KvData
    /// @param pBData
    /// @param nDataLen
    /// @return
    bool ReplaceKvData(uint8_t const *const pBData, uint16_t const nDataLen) const noexcept;
    /// @brief Replace the local Value data + Redd check data
    /// @param eValueType
    /// @param pBData
    /// @param nDataLen
    /// @return
    bool ReplaceLocalValue(EDataType const eValueType,
                           uint8_t const *const pBData,
                           uint16_t const nDataLen,
                           EReddType const eReddType,
                           ara::core::Vector< uint8_t > const &vecReddData) const noexcept;
    /// @brief Calculate the total length of the KV external page data
    /// @param nKeyLen
    /// @param nValueLen
    /// @param nReddLen
    /// @return
    int32_t StatExtBlockLen(int32_t const nKeyLen, int32_t const nValueLen, int32_t const nReddLen) const noexcept;

protected:
    /// @brief
    /// @return
    uint16_t _CalKvCRC() const noexcept;
    /// @brief Get local data  // if ppLocalData/ppBlockData is null, move the data read pointer
    /// @param nIndex
    /// @param eDataFormat
    /// @param ppLocalData
    /// @param ppBlockData
    /// @return
    int32_t _ReadLogicData(int32_t nIndex,
                           EDataFormat const eDataFormat,
                           PDataLocal **const ppLocalData,
                           PBlockIndex **const ppBlockData) const noexcept;
};
//***************/
/// @brief
/// @param pOptPageKv
/// @param nKvIndex
POptKvData::POptKvData(PPageOptKvStore const *const pOptPageKv, int32_t const nKvIndex) noexcept
    : pOptPageKv_{pOptPageKv}, pKvElement_{nullptr}, pKvData_{nullptr}
{
    if (nullptr != pOptPageKv) {
        pKvElement_ = pOptPageKv->GetKvElement(nKvIndex);
        pKvData_    = pOptPageKv->GetKvData(pKvElement_);
    }
}
/// @brief
/// @param pOptPageKv
/// @param pKvElement
POptKvData::POptKvData(PPageOptKvStore const *const pOptPageKv, PBlockElementKv *const pKvElement) noexcept
    : pOptPageKv_{pOptPageKv}, pKvElement_{pKvElement}, pKvData_{nullptr}
{
    if (nullptr != pOptPageKv) {
        pKvData_ = pOptPageKv->GetKvData(pKvElement_);
    }
}
/// @brief
/// @param pOptPageKv
/// @param pKvElement
/// @param pKvData
POptKvData::POptKvData(PPageOptKvStore const *const pOptPageKv,
                       PBlockElementKv *const pKvElement,
                       PElementKvData *const pKvData) noexcept
    : pOptPageKv_{pOptPageKv}, pKvElement_{pKvElement}, pKvData_{pKvData}
{
}
/// @brief
/// @return
bool POptKvData::IsAccessReady() const noexcept { return (nullptr != pKvElement_) && (nullptr != pKvData_); }
/// @brief Calculate the CRC16 code of the KV-Element
/// @return
uint16_t POptKvData::CalKvCRC16() const noexcept
{
    if (false == IsAccessReady()) {
        return 0U;
    }
    return _CalKvCRC();
}
/// @brief Update CRC16
/// @return
bool POptKvData::UpdateCrc() const noexcept
{
    if (false == IsAccessReady()) {
        return false;
    }
    pKvData_->sCrc16 = _CalKvCRC();
    return true;
}
/// @brief Verify CRC16
/// @return
bool POptKvData::CheckCrc() const noexcept
{
    if (false == IsAccessReady()) {
        return false;
    }
    return pKvData_->sCrc16 == _CalKvCRC();
}
/// @brief Determine the data type of the Key
/// @param eDataFormat
/// @return
bool POptKvData::IsKeyFormat(EDataFormat const eDataFormat) const noexcept
{
    if (nullptr == pKvData_) {
        return false;
    }
    return pKvData_->cFormatKey == static_cast< uint8_t >(eDataFormat);
}
/// @brief Determine the data type of the Value
/// @param eDataFormat
/// @return
bool POptKvData::IsValueFormat(EDataFormat const eDataFormat) const noexcept
{
    if (nullptr == pKvData_) {
        return false;
    }
    return pKvData_->cFormatValue == static_cast< uint8_t >(eDataFormat);
}
/// @brief Determine the data type of the Redd
/// @param eDataFormat
/// @return
bool POptKvData::IsReddFormat(EDataFormat const eDataFormat) const noexcept
{
    if (nullptr == pKvData_) {
        return false;
    }
    return pKvData_->cFormatRedd == static_cast< uint8_t >(eDataFormat);
}
/// @brief Replace the entire KvData
/// @param pBData
/// @param nDataLen
/// @return
bool POptKvData::ReplaceKvData(uint8_t const *const pBData, uint16_t const nDataLen) const noexcept
{
    if (false == IsAccessReady()) {
        return false;
    }
    pOptPageKv_->CopyData(pKvData_, pBData, nDataLen);
    pKvData_->sCrc16 = _CalKvCRC();
    return true;
}
/// @brief Replace the local Value data
/// @param eValueType
/// @param pBData
/// @param nDataLen
/// @return
bool POptKvData::ReplaceLocalValue(EDataType const eValueType,
                                   uint8_t const *const pBData,
                                   uint16_t const nDataLen,
                                   EReddType const eReddType,
                                   ara::core::Vector< uint8_t > const &vecReddData) const noexcept
{
    if (false == IsAccessReady()) {
        return false;
    }
    // Local type data must have valid data present
    if (false == IsValueFormat(EDataFormat::kLocal)) {
        return false;
    }

    int32_t nIndex{static_cast< int32_t >(sizeof(PElementKvData))};
    nIndex = _ReadLogicData(nIndex, static_cast< EDataFormat >(pKvData_->cFormatKey), nullptr, nullptr);
    PDataLocal *const pLocalValue{T_TransPtr< PDataLocal >(T_TransBytes(pKvData_) + nIndex)};
    if (pLocalValue->cDataLen != nDataLen) {
        return false;
    }
    if (pKvElement_->cReddType != static_cast< uint8_t >(eReddType)) {
        return false;
    }
    if (EReddType::kNone != eReddType) {
        if (false == IsReddFormat(EDataFormat::kLocal)) {
            return false;
        }
        nIndex = _ReadLogicData(nIndex, static_cast< EDataFormat >(pKvData_->cFormatValue), nullptr, nullptr);
        PDataLocal *const pLocalRedd{T_TransPtr< PDataLocal >(T_TransBytes(pKvData_) + nIndex)};
        if (pLocalRedd->cDataLen != static_cast< uint8_t >(vecReddData.size())) {
            return false;
        }
        // Update Redd check data
        pLocalRedd->cDataType = static_cast< uint8_t >(EDataType::kBinary);
        pOptPageKv_->CopyData(pLocalRedd + 1, static_cast< const void * >(vecReddData.data()),
                              static_cast< uint16_t >(vecReddData.size()));
    }
    // Only applicable when the Value lengths are exactly the same
    pLocalValue->cDataType = static_cast< uint8_t >(eValueType);
    pOptPageKv_->CopyData(pLocalValue + 1, pBData, nDataLen);
    pKvData_->sCrc16 = _CalKvCRC();
    return true;
}
/// @brief Calculate the total length of the KV external page data
/// @param nKeyLen
/// @param nValueLen
/// @param nReddLen
/// @return
int32_t POptKvData::StatExtBlockLen(int32_t const nKeyLen,
                                    int32_t const nValueLen,
                                    int32_t const nReddLen) const noexcept
{
    if (nullptr == pKvData_) {
        return -1;
    }
    int32_t nExtDataTotal{0};
    // Fill external page data: Key
    if (pKvData_->cFormatKey == static_cast< uint8_t >(EDataFormat::kBlockIndex)) {
        nExtDataTotal += nKeyLen + static_cast< int32_t >(sizeof(PBlockHead));
    }
    // Fill external page data: Value
    if (pKvData_->cFormatValue == static_cast< uint8_t >(EDataFormat::kBlockIndex)) {
        nExtDataTotal += nValueLen + static_cast< int32_t >(sizeof(PBlockHead));
    }
    // Fill external page data: Redd
    if (pKvData_->cFormatRedd == static_cast< uint8_t >(EDataFormat::kBlockIndex)) {
        nExtDataTotal += nReddLen + static_cast< int32_t >(sizeof(PBlockHead));
    }
    return nExtDataTotal;
}
//***************/
/// @brief Get KV storage local data / BlockIndex data
/// @param nIndex
/// @param eDataFormat
/// @param ppLocalData
/// @param ppBlockData
/// @return
int32_t POptKvData::_ReadLogicData(int32_t nIndex,
                                   EDataFormat const eDataFormat,
                                   PDataLocal **const ppLocalData,
                                   PBlockIndex **const ppBlockData) const noexcept
{
    if (nullptr != ppLocalData) {
        *ppLocalData = nullptr;
    }
    if (nullptr != ppBlockData) {
        *ppBlockData = nullptr;
    }
    uint8_t *const pDataBase{T_TransBytes(pKvData_)};
    switch (eDataFormat) {
        case EDataFormat::kNoData: {
        } break;
        case EDataFormat::kLocal: {
            PDataLocal *const pLocalData{T_TransPtr< PDataLocal >(pDataBase + nIndex)};
            nIndex += static_cast< int32_t >(sizeof(PDataLocal));
            nIndex += static_cast< int32_t >(pLocalData->cDataLen);
            if (nullptr != ppLocalData) {
                *ppLocalData = pLocalData;
            }
        } break;
        case EDataFormat::kBlockIndex: {
            PBlockIndex *const pBlockData{T_TransPtr< PBlockIndex >(pDataBase + nIndex)};
            nIndex += static_cast< int32_t >(sizeof(PBlockIndex));
            if (nullptr != ppBlockData) {
                *ppBlockData = pBlockData;
            }
        } break;
        case EDataFormat::kHugeBlock: {
        } break;
    }
    return nIndex;
}
/// @brief Calculate the CRC16 code of the KV-Element
/// @return
uint16_t POptKvData::_CalKvCRC() const noexcept
{
    PER_Assert((nullptr != pKvElement_) && (nullptr != pKvData_));
    // Calculate the CRC16 value of the KV-Element; here, sCrc16 must be the first element of the PElementKvData structure
    uint16_t nCrc16{0U};
    nCrc16 = CalculateCrc16(pKvElement_, sizeof(PBlockElementKv), nCrc16);
    uint8_t const *const pBData{T_TransBytes(pKvData_)};
    uint32_t const nDataLen{pKvElement_->cLen};
    nCrc16 = CalculateCrc16(pBData + sizeof(pKvData_->sCrc16),
                            nDataLen - static_cast< uint32_t >(sizeof(pKvData_->sCrc16)), nCrc16);
    return nCrc16;
}
//********************************/
/// @brief Operation class for the KV data area
class POptKvData_Read final : public POptKvData
{
private:
    /// @brief This page's data
    PDataLocal *pLocalData_{nullptr};
    /// @brief Cross-page data
    PBlockIndex *pBlockData_{nullptr};

public:
    /// @brief
    /// @param pOptPageKv
    /// @param nKvIndex
    POptKvData_Read(PPageOptKvStore const *const pOptPageKv, int32_t const nKvIndex) noexcept
        : POptKvData{pOptPageKv, nKvIndex}
    {
    }
    /// @brief
    /// @param pOptPageKv
    /// @param pKvElement
    POptKvData_Read(PPageOptKvStore const *const pOptPageKv, PBlockElementKv const *const pKvElement) noexcept
        : POptKvData{pOptPageKv, const_cast< PBlockElementKv * >(pKvElement)}
    {
    }
    /// @brief
    /// @return
    inline PDataLocal *GetLocalData() const noexcept { return pLocalData_; }
    /// @brief
    /// @return
    inline PBlockIndex *GetBlockData() const noexcept { return pBlockData_; }
    /// @brief Get KV data: return value, -1 indicates cross-page, 1 indicates local, 0 indicates failure
    /// @param eLogicData
    /// @return
    int32_t ReadKvLogicData(EDataLogic const eLogicData) noexcept;
    /// @brief
    /// @return
    PDataLocal *ReadLocalCrc64() const noexcept;
};
/// @brief Get local data
/// @param eLogicData
/// @return
int32_t POptKvData_Read::ReadKvLogicData(EDataLogic const eLogicData) noexcept
{
    pLocalData_ = nullptr;  // This page's data
    pBlockData_ = nullptr;  // Cross-page data
    if (false == IsAccessReady()) {
        return 0;
    }
    int32_t nIndex{static_cast< int32_t >(sizeof(PElementKvData))};
    nIndex = _ReadLogicData(nIndex, static_cast< EDataFormat >(_GetKvData()->cFormatKey), &pLocalData_, &pBlockData_);
    if (EDataLogic::kLogicKey == eLogicData) {
        if (nullptr != pLocalData_) {
            return 1;
        }
        return -1;
    }
    nIndex = _ReadLogicData(nIndex, static_cast< EDataFormat >(_GetKvData()->cFormatValue), &pLocalData_, &pBlockData_);
    if (EDataLogic::kLogicValue == eLogicData) {
        if (nullptr != pLocalData_) {
            return 1;
        }
        return -1;
    }
    nIndex = _ReadLogicData(nIndex, static_cast< EDataFormat >(_GetKvData()->cFormatRedd), &pLocalData_, &pBlockData_);
    if (EDataLogic::kLogicRedd == eLogicData) {
        if (nullptr != pLocalData_) {
            return 1;
        }
        return -1;
    }

    if (EDataLogic::kLogicCrc64 == eLogicData) {
        pLocalData_ = T_TransPtr< PDataLocal >(T_TransBytes(_GetKvData()) + nIndex);
    }
    if (nullptr != pLocalData_) {
        return 1;
    }
    return -1;
}
/// @brief
/// @return
PDataLocal *POptKvData_Read::ReadLocalCrc64() const noexcept
{
    uint8_t *const pDataBase{T_TransBytes(_GetKvData())};
    int32_t nIndex{static_cast< int32_t >(sizeof(PElementKvData))};
    nIndex = _ReadLogicData(nIndex, static_cast< EDataFormat >(_GetKvData()->cFormatKey), nullptr, nullptr);
    nIndex = _ReadLogicData(nIndex, static_cast< EDataFormat >(_GetKvData()->cFormatValue), nullptr, nullptr);
    nIndex = _ReadLogicData(nIndex, static_cast< EDataFormat >(_GetKvData()->cFormatRedd), nullptr, nullptr);
    return T_TransPtr< PDataLocal >(pDataBase + nIndex);
}
//********************************/
/// @brief Operation class for the KV data area
class POptKvData_Assemble final
{
private:
    /// @brief
    uint8_t *pDataBase_;
    /// @brief Traversal cursor
    uint16_t nIndex_;
    /// @brief
    uint16_t nBuffMax_;

public:
    /// @brief
    /// @param pBuff
    /// @param nBuffMax
    explicit POptKvData_Assemble(uint8_t *const pBuff, uint16_t const nBuffMax) noexcept;
    /// @brief
    POptKvData_Assemble() = delete;
    /// @brief
    ~POptKvData_Assemble() = default;
    /// @brief
    /// @param a
    POptKvData_Assemble(POptKvData_Assemble const &a) = delete;
    /// @brief
    /// @param a
    /// @return
    POptKvData_Assemble &operator=(POptKvData_Assemble const &a) = delete;
    /// @brief
    /// @param a
    POptKvData_Assemble(POptKvData_Assemble &&a) = delete;
    /// @brief
    /// @param a
    /// @return
    POptKvData_Assemble &operator=(POptKvData_Assemble &&a) = delete;
    /// @brief Assemble the KV data structure into pDataBase_
    /// @param nReddType Redundancy type, stored in PBlockElementKv.cReddType
    /// @param nSrcKeyHash Calculated key hash value: 64-bit hash
    /// @param eKeyType Key storage type: EDataFormat
    /// @param pBKey Key data
    /// @param nKeyLen Key length
    /// @param eValueType Value storage type: EDataFormat
    /// @param pBValue Value data
    /// @param nValueLen Value length
    /// @param eReddDataType Redd storage type: EDataFormat
    /// @param pBRedd Redd data
    /// @param nReddLen Length of Redd data
    /// @return Length of the KV data structure
    int32_t AssembleKvData(EReddType const eReddType,
                           uint64_t const nSrcKeyHash,
                           EDataType const eKeyType,
                           uint8_t const *const pBKey,
                           uint16_t const nKeyLen,
                           EDataType const eValueType,
                           uint8_t const *const pBValue,
                           uint32_t const nValueLen,
                           EDataType const eReddDataType,
                           uint8_t const *const pBRedd,
                           uint16_t const nReddLen) noexcept;

protected:
    /// @brief
    /// @param eDataType
    /// @param pBData
    /// @param nDataLen
    /// @return
    uint8_t _AssembleLocalData(EDataType const eDataType,
                               uint8_t const *const pBData,
                               uint16_t const nDataLen) noexcept;
    /// @brief
    /// @return
    uint8_t _AssembleBlockData() noexcept;
    /// @brief
    /// @param nReddType
    /// @return
    uint8_t _AssembleBlockData(EReddType const nReddType) noexcept;  // Assemble Redd
};
/// @brief
/// @param pBuff
/// @param nBuffMax
POptKvData_Assemble::POptKvData_Assemble(uint8_t *const pBuff, uint16_t const nBuffMax) noexcept
    : pDataBase_{pBuff}, nIndex_{0U}, nBuffMax_{nBuffMax}
{
}
/// @brief Assemble the KV data structure into pDataBase_
/// @param eReddType Redundancy type, stored in PBlockElementKv.cReddType
/// @param nSrcKeyHash Calculated key hash value: 64-bit hash
/// @param eKeyType Key storage type: EDataFormat
/// @param pBKey Key data
/// @param nKeyLen Key length
/// @param eValueType Value storage type: EDataFormat
/// @param pBValue Value data
/// @param nValueLen Value length
/// @param eReddDataType Redd storage type: EDataFormat
/// @param pBRedd Redd data
/// @param nReddLen Length of Redd data
/// @return Length of the KV data structure
int32_t POptKvData_Assemble::AssembleKvData(EReddType const eReddType,
                                            uint64_t const nSrcKeyHash,
                                            EDataType const eKeyType,
                                            uint8_t const *const pBKey,
                                            uint16_t const nKeyLen,
                                            EDataType const eValueType,
                                            uint8_t const *const pBValue,
                                            uint32_t const nValueLen,
                                            EDataType const eReddDataType,
                                            uint8_t const *const pBRedd,
                                            uint16_t const nReddLen) noexcept
{
    nIndex_ = 0U;
    PElementKvData tempKvData;  // Provide a storage space when pBBuff == nullptr
    tempKvData.sCrc16       = 0U;
    tempKvData.cKeyWhole    = 0U;
    tempKvData.cFormatKey   = 0U;
    tempKvData.cFormatValue = 0U;
    tempKvData.cFormatRedd  = 0U;
    PElementKvData *pKvData{T_TransPtr< PElementKvData >(pDataBase_)};
    if (nullptr == pKvData) {
        pKvData = &tempKvData;
    }
    nIndex_ = sizeof(PElementKvData);
    // Process Key first
    if ((nIndex_ + sizeof(PDataLocal) + nKeyLen) > nBuffMax_) {
        // If the Key value cannot fit, only save the Key's hash code
        pKvData->cKeyWhole = 0U;
        // Key in external page: PBlockIndex
        pKvData->cFormatKey = _AssembleBlockData();
    } else {
        // Can store the entire Key text: PDataLocal * 1 + PBlockIndex * X (Value and Redd to be determined)
        pKvData->cKeyWhole  = 1U;
        pKvData->cFormatKey = _AssembleLocalData(eKeyType, pBKey, nKeyLen);
    }
    // Process Value
    if ((nIndex_ + static_cast< uint_t >(sizeof(PDataLocal) + nValueLen)) > nBuffMax_) {
        // Value in external page: PBlockIndex
        pKvData->cFormatValue = _AssembleBlockData();
    } else {
        // Can save the entire Value text: PDataLocal
        pKvData->cFormatValue = _AssembleLocalData(eValueType, pBValue, static_cast< uint16_t >(nValueLen));
    }
    // Process Redd
    if (eReddType == EReddType::kNone) {
        pKvData->cFormatRedd = static_cast< uint8_t >(EDataFormat::kNoData);
    } else {
        if ((nIndex_ + sizeof(PDataLocal) + nReddLen) > nBuffMax_) {
            pKvData->cFormatRedd = _AssembleBlockData(eReddType);  // Redd in external page
        } else {
            pKvData->cFormatRedd = _AssembleLocalData(eReddDataType, pBRedd, nReddLen);
        }
    }
    // If the Key is incomplete, keep a hash code of the Key
    if (0 == pKvData->cKeyWhole) {
        std::ignore = _AssembleLocalData(EDataType::kUint64, T_TransBytes(&nSrcKeyHash), sizeof(nSrcKeyHash));
    }
    // The final total length of the generated KV storage structure is around 64, not a maximum of 64. The maximum should be 76.
    return static_cast< int32_t >(nIndex_);
}
//***************/
/// @brief
/// @param eDataType
/// @param pBData
/// @param nDataLen
/// @return
uint8_t POptKvData_Assemble::_AssembleLocalData(EDataType const eDataType,
                                                uint8_t const *const pBData,
                                                uint16_t const nDataLen) noexcept
{
    if (nullptr != pDataBase_) {
        PDataLocal *const pDataLocal{T_TransPtr< PDataLocal >(pDataBase_ + nIndex_)};
        pDataLocal->cDataType = static_cast< uint8_t >(eDataType);
        pDataLocal->cDataLen  = static_cast< uint8_t >(nDataLen);
        std::ignore           = PPageOptBase::MoveData(pDataLocal + 1, pBData, nDataLen);
    }
    nIndex_ += static_cast< uint16_t >(sizeof(PDataLocal)) + nDataLen;
    return static_cast< uint8_t >(EDataFormat::kLocal);
}
/// @brief
/// @return
uint8_t POptKvData_Assemble::_AssembleBlockData() noexcept
{
    if (nullptr != pDataBase_) {
        PBlockIndex *const pBlockIndex{T_TransPtr< PBlockIndex >(pDataBase_ + nIndex_)};
        pBlockIndex->nPageID  = kInvalidPageID;
        pBlockIndex->sBlockID = 0U;
    }
    nIndex_ += sizeof(PBlockIndex);
    return static_cast< uint8_t >(EDataFormat::kBlockIndex);
}
/// @brief Assemble Redd
/// @param nReddType
/// @return
uint8_t POptKvData_Assemble::_AssembleBlockData(EReddType const nReddType) noexcept
{
    if (nReddType > EReddType::kNone) {
        return _AssembleBlockData();  // Redd in external page
    }
    return static_cast< uint8_t >(EDataFormat::kNoData);
}
//********************************/
/// @brief
/// @param pCachePagePtr
PPageOptKvStore::PPageOptKvStore(PCachePagePtr const &pCachePagePtr) noexcept
    : PPageOptSolo{pCachePagePtr, EBlockType::kKvStore, 0U}
{
}

/// @brief Get the minimum length of the current page type: when there is no data
/// @return
uint16_t PPageOptKvStore::GetMinValidLen() const noexcept
{
    uint32_t nMinValidLen{0U};
    nMinValidLen += sizeof(PPageHeadData);
    nMinValidLen += sizeof(PBlockHead);
    nMinValidLen += sizeof(PBlockHeadKvStore);
    return static_cast< uint16_t >(nMinValidLen);
}
//***************/
/// @brief
/// @param nKvStoreID
/// @param nLastKvPageID
/// @param bResetData
/// @return
bool PPageOptKvStore::InitKvHead(uint32_t const nKvStoreID,
                                 uint32_t const nLastKvPageID,
                                 bool const bResetData) noexcept
{
    if (false == InitPageHead(PageClassType(), GetMemoryPage()->GetMainPageID(), bResetData)) {
        PER_OnOptFailed(EErrorPHKV::kPageInit);
        return false;
    }
    // 2023-08-24 hanjingjing: KvStorage pages use single-page mode Blocks because their multi-page mode is supported within PBlockHeadKvStore
    EStoreRange const eRangeType{EStoreRange::kSingle};
    uint16_t const nMinBlockLen{
        static_cast< uint16_t >(sizeof(PBlockHeadKvStore) + sizeof(PBlockElementKv) + 3U * sizeof(PBlockIndex))};
    if (false == _CheckBlockSpace(eRangeType, nMinBlockLen)) {
        PER_OnOptFailed(EErrorPHKV::kPageLackSpace);
        return false;
    }
    // Occupy all remaining space
    uint16_t const nRealBlockLen{static_cast< uint16_t >(GetPageHead()->sFreeLen - S_GetBlockHeadLen(eRangeType))};
    if (false == InitSoloBlock(eRangeType, nRealBlockLen)) {
        return false;
    }
    if (false == IsAccessReady()) {
        PER_OnOptFailed(EErrorPHKV::kNotReadyPageKv);
        return false;
    }
    G_InitKvHead(_GetKvHead(), nRealBlockLen, nKvStoreID, nLastKvPageID);
    _LogDebug("[KV_Opt].InitKvHead", ":", "PageID", static_cast< int32_t >(GetPageID()), "nKvStoreID",
              static_cast< int32_t >(nKvStoreID), "nLastKvPageID", static_cast< int32_t >(nLastKvPageID));
    _OnOptSuccess(true);
    return true;
}
/// @brief Reset KV-Store: retain cross-page information
/// @param bKeepNext
/// @return
bool PPageOptKvStore::ResetKvStore(bool const bKeepNext) const noexcept
{
    if (false == IsAccessReady()) {
        PER_OnOptFailed(EErrorPHKV::kNotReadyPageKv);
        return false;
    }
    PBlockHeadKvStore *const pKvHead{_GetKvHead()};
    PER_Assert(nullptr != pKvHead);
    PBlockHeadKvStore const kvHead{*pKvHead};
    G_InitKvHead(pKvHead, kvHead.sInitLen, kvHead.nKvStoreID, kvHead.nLastKvPageID);
    if (bKeepNext) {
        pKvHead->nNextPageID = kvHead.nNextPageID;
        pKvHead->nPrevPageID = kvHead.nPrevPageID;
    }
    _LogDebug("[KV_Opt].ResetKvStore", ":", "KvStoreID", static_cast< int32_t >(kvHead.nKvStoreID), "PageID",
              static_cast< int32_t >(GetPageID()), "NextPageID", static_cast< int32_t >(kvHead.nNextPageID));
    _OnOptSuccess(true);
    return true;
}
/// @brief Add a Kv data item
/// @param nSrcKeyHash
/// @param nReddType
/// @param pBData
/// @param nDataLen
/// @return
PBlockElementKv *PPageOptKvStore::AddKvElement(uint64_t const nSrcKeyHash,
                                               uint8_t const nReddType,
                                               uint8_t const *const pBData,
                                               uint16_t const nDataLen) const noexcept
{
    if (false == IsAccessReady()) {
        PER_OnOptFailed(EErrorPHKV::kNotReadyPageKv);
        return nullptr;
    }
    PBlockHeadKvStore *const pKvHead{_GetKvHead()};
    PER_Assert(nullptr != pKvHead);
    // Add a new KV
    uint16_t const nNeedKvLen{static_cast< uint16_t >(G_ComputePlanAddLen(pKvHead, static_cast< int32_t >(nDataLen)))};
    if (false == G_CheckKvStoreCapacity(*pKvHead, nNeedKvLen)) {
        _OnOptFailed(EErrorPHKV::kPageLackSpace);
        return nullptr;
    }
    uint32_t const nKeyHashID{G_GetKeyHashID(nSrcKeyHash)};
    PBlockElementKv *const pKvElement{G_GetBlockElement(pKvHead, static_cast< int32_t >(pKvHead->sElementCount))};
    _SaveKvElementData(pKvHead, pKvElement, pBData, static_cast< uint8_t >(nDataLen), nKeyHashID, nReddType);
    pKvHead->sElementCount += 1U;
    pKvHead->nKvTotal += 1U;
    pKvHead->sFreeLen -= nNeedKvLen;
    _OnOptSuccess(true);
    return pKvElement;
}
/// @brief Add a Kv data item
/// @param pKvElement
/// @param pKvData
/// @return
PBlockElementKv *PPageOptKvStore::AddKvElement(PBlockElementKv const *const pKvElement,
                                               PElementKvData const *const pKvData) const noexcept
{
    return AddKvElement(static_cast< uint64_t >(pKvElement->nKeyHashID), pKvElement->cReddType, T_TransBytes(pKvData),
                        static_cast< uint16_t >(pKvElement->cLen));
}
/// @brief Delete a KV-Element
/// @param nKvIndex
/// @return
bool PPageOptKvStore::DelKvElement(int32_t const nKvIndex) noexcept
{
    if (false == IsAccessReady()) {
        PER_OnOptFailed(EErrorPHKV::kNotReadyPageKv);
        return false;
    }
    PBlockHeadKvStore *const pKvHead{_GetKvHead()};
    PER_Assert(nullptr != pKvHead);
    if (false == G_CheckArrayIndex(*pKvHead, nKvIndex)) {
        PER_OnOptFailed(EErrorPHKV::kIndexKvArray);
        return false;
    }
    // Delete an old KV
    PBlockElementKv *pFindElement{nullptr};
    pFindElement = G_GetBlockElement(pKvHead, nKvIndex);
    int32_t const nAmendHeadLen{static_cast< int32_t >(sizeof(PBlockElementKv))};
    int32_t const nAmendBodyLen{static_cast< int32_t >(pFindElement->cLen)};
    std::ignore       = _MoveListElementBody(pKvHead, nKvIndex, nAmendBodyLen);
    std::ignore       = _MoveListElementHead(pKvHead, nKvIndex, -nAmendHeadLen);
    pKvHead->sFreeLen = T_AddInt(pKvHead->sFreeLen, nAmendBodyLen + nAmendHeadLen);
    pKvHead->sElementCount -= 1U;
    pKvHead->nKvTotal -= 1U;
    _OnOptSuccess(true);
    return true;
}
/// @brief Replace the entire KV storage structure and data
/// @param nKvIndex
/// @param pBData
/// @param nDataLen
/// @return
bool PPageOptKvStore::ReplaceKvElement(int32_t const nKvIndex,
                                       uint8_t const *const pBData,
                                       uint16_t const nDataLen) noexcept
{
    if (false == IsAccessReady()) {
        PER_OnOptFailed(EErrorPHKV::kNotReadyPageKv);
        return false;
    }
    PBlockHeadKvStore *const pKvHead{_GetKvHead()};
    PER_Assert(nullptr != pKvHead);
    if (false == G_CheckArrayIndex(*pKvHead, nKvIndex)) {
        PER_OnOptFailed(EErrorPHKV::kIndexKvArray);
        return false;
    }
    // Operate on old data
    PBlockElementKv *pFindElement{nullptr};
    pFindElement = G_GetBlockElement(pKvHead, nKvIndex);
    int32_t nAmendLen{0};
    nAmendLen = static_cast< int32_t >(nDataLen) - static_cast< int32_t >(pFindElement->cLen);

    if (0 == nAmendLen) {  // Exactly
        _KvElementReplace(pKvHead, pFindElement, pBData, nDataLen);
    } else {
        if (nAmendLen < 0) {  // Shrink table
            // Save itself first
            _KvElementReplace(pKvHead, pFindElement, pBData, nDataLen);
            // Move subsequent nodes to new positions in forward order
            std::ignore = _MoveListElementBody(pKvHead, nKvIndex, -nAmendLen);
        } else {  // Expand table
            if (false == _CheckPageCapacity(static_cast< uint16_t >(nAmendLen))) {
                PER_OnOptFailed(EErrorPHKV::kPageLackSpace);
                return false;
            }
            // Move subsequent nodes in reverse order first
            std::ignore = _MoveListElementBody(pKvHead, nKvIndex, -nAmendLen);
            // Then save itself
            _KvElementReplace(pKvHead, pFindElement, pBData, nDataLen);
        }
    }
    // Maintain free space
    pKvHead->sFreeLen = T_AddInt(pKvHead->sFreeLen, nAmendLen);
    _OnOptSuccess(true);
    return true;
}
/// @brief Replace the locally stored Value in KV
/// @param nKvIndex
/// @param eValueType
/// @param pBData
/// @param nDataLen
/// @return
bool PPageOptKvStore::ReplaceLocalValue(int32_t const nKvIndex,
                                        EDataType const eValueType,
                                        uint8_t const *const pBData,
                                        uint16_t const nDataLen,
                                        EReddType eReddType,
                                        ara::core::Vector< uint8_t > const &vecReddData) const noexcept
{
    if (false == IsAccessReady()) {
        PER_OnOptFailed(EErrorPHKV::kNotReadyPageKv);
        return false;
    }
    POptKvData const optKvDataRead{this, nKvIndex};
    if (false == optKvDataRead.IsAccessReady()) {
        PER_OnOptFailed(EErrorPHKV::kIndexKvArray);
        return false;
    }
    if (false == optKvDataRead.IsValueFormat(EDataFormat::kLocal)) {
        return false;
    }
    if (false == optKvDataRead.ReplaceLocalValue(eValueType, pBData, nDataLen, eReddType, vecReddData)) {
        return false;
    }
    _OnOptSuccess(true);
    return true;
}
/// @brief Get the size of the locally stored Value data
/// @param nKvIndex
/// @return
uint32_t PPageOptKvStore::GetLocalValueLen(int32_t const nKvIndex) const noexcept
{
    if (false == IsAccessReady()) {
        PER_OnOptFailed(EErrorPHKV::kNotReadyPageKv);
        return 0U;
    }
    POptKvData_Read optKvDataRead{this, nKvIndex};
    if (false == optKvDataRead.IsAccessReady()) {
        PER_OnOptFailed(EErrorPHKV::kIndexKvArray);
        return 0U;
    }
    if (false == optKvDataRead.IsValueFormat(EDataFormat::kLocal)) {
        return 0U;
    }
    std::ignore = optKvDataRead.ReadKvLogicData(EDataLogic::kLogicValue);
    PDataLocal const *const pLocalData{optKvDataRead.GetLocalData()};
    if (nullptr == pLocalData) {
        return 0U;
    }
    _OnOptSuccess(false);
    return static_cast< uint32_t >(pLocalData->cDataLen);
}
/// @brief Get the size of the locally stored Redd check data
/// @param nKvIndex
/// @return
uint32_t PPageOptKvStore::GetLocalReddLen(int32_t const nKvIndex) const noexcept
{
    if (false == IsAccessReady()) {
        PER_OnOptFailed(EErrorPHKV::kNotReadyPageKv);
        return 0U;
    }
    POptKvData_Read optKvDataRead{this, nKvIndex};
    if (false == optKvDataRead.IsAccessReady()) {
        PER_OnOptFailed(EErrorPHKV::kIndexKvArray);
        return 0U;
    }
    if (false == optKvDataRead.IsReddFormat(EDataFormat::kLocal)) {
        return 0U;
    }
    std::ignore = optKvDataRead.ReadKvLogicData(EDataLogic::kLogicRedd);
    PDataLocal const *const pLocalData{optKvDataRead.GetLocalData()};
    if (nullptr == pLocalData) {
        return 0U;
    }
    _OnOptSuccess(false);
    return static_cast< uint32_t >(pLocalData->cDataLen);
}
/// @brief Get the corresponding KV-Element
/// @param nKvIndex
/// @return
PBlockElementKv *PPageOptKvStore::GetKvElement(int32_t const nKvIndex) const noexcept
{
    if (false == IsAccessReady()) {
        PER_OnOptFailed(EErrorPHKV::kNotReadyPageKv);
        return nullptr;
    }
    PBlockElementKv *pFindElement{nullptr};
    pFindElement = _GetKvElement(nKvIndex);
    if (nullptr == pFindElement) {
        PER_OnOptFailed(EErrorPHKV::kIndexKvArray);
        return nullptr;
    }
    _OnOptSuccess(false);
    return pFindElement;
}
/// @brief Get the corresponding KV-Data
/// @param pKvElement
/// @return
PElementKvData *PPageOptKvStore::GetKvData(PBlockElementKv const *const pKvElement) const noexcept
{
    if (false == IsAccessReady()) {
        PER_OnOptFailed(EErrorPHKV::kNotReadyPageKv);
        return nullptr;
    }
    _OnOptSuccess(false);
    return _GetKvData(pKvElement);
}
/// @brief Find the BlockIndex data of the KV
/// @param nKvIndex
/// @param eDataLogic
/// @return
PBlockIndex *PPageOptKvStore::FindBlockIndex(int32_t const nKvIndex, EDataLogic const eDataLogic) const noexcept
{
    if (false == IsAccessReady()) {
        PER_OnOptFailed(EErrorPHKV::kNotReadyPageKv);
        return nullptr;
    }
    PBlockIndex *pBlockIndex{nullptr};
    PDataLocal *pLocalKey{nullptr};
    if (false == GetKvLocalData(eDataLogic, nKvIndex, pBlockIndex, pLocalKey)) {
        PER_OnOptFailed(GetLastError());
        return nullptr;
    }
    _OnOptSuccess(false);
    return pBlockIndex;
}
/// @brief Traverse KV-Elements from the end, find the first KV whose length is less than or equal to nMaxLen
/// @param nMaxLen
/// @return
int32_t PPageOptKvStore::FindKvElementByLen(uint16_t const nMaxLen) const noexcept
{
    if (false == IsAccessReady()) {
        PER_OnOptFailed(EErrorPHKV::kNotReadyPageKv);
        return -1;
    }
    int32_t nFindIndex{-1};
    CB_EachKvIntConst pfunWork;
    pfunWork = [nMaxLen, &nFindIndex](int32_t const nIndex, PBlockElementKv const *const pKvElement,
                                      PElementKvData const *) noexcept -> int32_t {
        if (nullptr == pKvElement) {
            return 0;
        }
        uint16_t nKvLen{static_cast< uint16_t >(sizeof(PBlockElementKv))};
        nKvLen += static_cast< uint16_t >(pKvElement->cLen);
        if (nKvLen <= nMaxLen) {
            nFindIndex = nIndex;
            return -1;
        }
        return 1;
    };
    PBlockHeadKvStore *const pKvHead{_GetKvHead()};
    PER_Assert(nullptr != pKvHead);
    std::ignore = G_ForEachKv(pKvHead, true, pfunWork);
    _OnOptSuccess(false);
    return nFindIndex;
}
/// @brief Traverse KV-Elements
/// @param bFromBack
/// @param pfun
/// @return
int32_t PPageOptKvStore::ForEachKv(bool const bFromBack, CB_EachKvIntConst const &pfun) const noexcept
{
    if (false == IsAccessReady()) {
        PER_OnOptFailed(EErrorPHKV::kNotReadyPageKv);
        return 0;
    }
    return _ForEachKv(bFromBack, pfun);
}
/// @brief Delete all KVs with the same hash index
/// @param pfun
/// @return
int32_t PPageOptKvStore::DelKvElement(CB_EachKvBool const &pfun) const noexcept
{
    if (false == IsAccessReady()) {
        PER_OnOptFailed(EErrorPHKV::kNotReadyPageKv);
        return 0;
    }
    PBlockHeadKvStore *const pKvHead{_GetKvHead()};
    PER_Assert(nullptr != pKvHead);
    uint16_t nAmendHeadLen{0U};
    uint16_t nAmendBodyLen{0U};
    uint16_t nDelCount{0U};
    for (uint16_t i{0U}; i < pKvHead->sElementCount; i++) {
        int32_t const nKvIndex{static_cast< int32_t >(i)};
        PBlockElementKv *const pKvElement{G_GetBlockElement(pKvHead, nKvIndex)};
        if (nullptr == pKvElement) {
            continue;
        }
        PElementKvData *const pKvData{G_GetKvDataByOffset(pKvHead, static_cast< int32_t >(pKvElement->sOffset))};
        if (pfun(nKvIndex, pKvElement, pKvData)) {
            nAmendHeadLen += sizeof(PBlockElementKv);
            nAmendBodyLen += pKvElement->cLen;
            nDelCount += 1U;
        } else {
            if ((nAmendHeadLen > 0U) || (nAmendHeadLen > 0U)) {
                _MoveElement(pKvHead, pKvElement, -1 * static_cast< int32_t >(nAmendHeadLen),
                             static_cast< int32_t >(nAmendBodyLen));
            }
        }
    }
    if (nDelCount > 0U) {
        pKvHead->sFreeLen += nAmendBodyLen + nAmendHeadLen;
        pKvHead->sElementCount -= nDelCount;
        pKvHead->nKvTotal -= nDelCount;
    }
    _OnOptSuccess(true);
    return static_cast< int32_t >(nDelCount);
}
/// @brief Split KV-Elements into two parts according to rules
/// @param pPageLeft
/// @param pPageRight
/// @param pfun
/// @return
bool PPageOptKvStore::DivKvElement(PCachePagePtr const &pPageLeft,
                                   PCachePagePtr const &pPageRight,
                                   CB_EachKvBool const &pfun) const noexcept
{
    PER_Assert(nullptr != pPageLeft);
    PER_Assert(nullptr != pPageRight);
    if (false == IsAccessReady()) {
        PER_OnOptFailed(EErrorPHKV::kNotReadyPageKv);
        return false;
    }
    (*pPageLeft)->ResetCacheData();
    (*pPageLeft)->SetMainPageID(static_cast< uint32_t >(-1));
    PPageOptKvStore optKvLeft{pPageLeft};
    if (false == optKvLeft.InitKvHead(0U, 0U, true)) {
        PER_OnOptFailed(EErrorPHKV::kNotReadyPageKv);
        return false;
    }
    (*pPageRight)->ResetCacheData();
    (*pPageRight)->SetMainPageID(static_cast< uint32_t >(-1));
    PPageOptKvStore optKvRight{pPageRight};
    if (false == optKvRight.InitKvHead(0U, 0U, true)) {
        PER_OnOptFailed(EErrorPHKV::kNotReadyPageKv);
        return false;
    }
    PBlockHeadKvStore *const pKvHead{_GetKvHead()};
    PER_Assert(nullptr != pKvHead);
    for (int32_t i{0}; i < static_cast< int32_t >(pKvHead->sElementCount); i++) {
        PBlockElementKv *const pKvElement{G_GetBlockElement(pKvHead, i)};
        PElementKvData *const pKvData{G_GetKvDataByOffset(pKvHead, static_cast< int32_t >(pKvElement->sOffset))};
        if (pfun(i, pKvElement, pKvData)) {
            std::ignore = optKvLeft.AddKvElement(pKvElement, pKvData);
        } else {
            std::ignore = optKvRight.AddKvElement(pKvElement, pKvData);
        }
    }
    // Clear old data
    std::ignore = ResetKvStore(true);
    _LogInfo("[KV_Opt].DivKvElement", ": ", "WorkPageID", static_cast< int32_t >(GetPageID()), "LeftCount",
             optKvLeft.GetKvElementCount(), "RightCount", optKvRight.GetKvElementCount());
    _OnOptSuccess(true);
    return true;
}
/// @brief Update the CRC16 checksum corresponding to the KV-Element
/// @param nKvIndex
/// @return
bool PPageOptKvStore::UpdateKvCRC16(int32_t const nKvIndex) const noexcept
{
    POptKvData const optKvData{this, nKvIndex};
    if (false == optKvData.UpdateCrc()) {
        PER_OnOptFailed(EErrorPHKV::kIndexKvArray);
        return false;
    }
    _OnOptSuccess(true);
    return true;
}
//***************/
/// @brief Assemble a new KV storage structure
/// @param pBBuff Output BUFF
/// @param nMaxBuffLen BUFF length
/// @param nSrcKeyHash Calculated Hash value of the Key
/// @param stKey Key string
/// @param eValueType Value type
/// @param pBValue Value content
/// @param nValueLen Value length
/// @param eReddType Redundancy type
/// @param pBRedd Redundancy data
/// @param nReddLen Redundancy length
/// @return Actual length of the KV storage structure
int32_t PPageOptKvStore::AssembleKvData(uint8_t *const pBBuff,
                                        uint16_t const nMaxBuffLen,
                                        uint64_t const nSrcKeyHash,
                                        ara::core::StringView const &stKey,
                                        EDataType const eValueType,
                                        uint8_t const *const pBValue,
                                        uint32_t const nValueLen,
                                        EReddType const eReddType,
                                        uint8_t const *const pBRedd,
                                        uint32_t const nReddLen) noexcept
{
    POptKvData_Assemble optKvDataWrite{pBBuff, nMaxBuffLen};
    int32_t nKvDataLen{0};
    nKvDataLen = optKvDataWrite.AssembleKvData(eReddType, nSrcKeyHash, EDataType::kString, T_TransBytes(stKey.data()),
                                               static_cast< uint16_t >(stKey.size()), eValueType, pBValue, nValueLen,
                                               EDataType::kBinary, pBRedd, static_cast< uint16_t >(nReddLen));
    return nKvDataLen;
}
/// @brief Get the total length of the external KV page data
/// @param pKvElement
/// @param nKeyLen
/// @param nValueLen
/// @param nReddLen
/// @return
int32_t PPageOptKvStore::ComputeKvExtBlockLen(PBlockElementKv *const pKvElement,
                                              int32_t const nKeyLen,
                                              int32_t const nValueLen,
                                              int32_t const nReddLen) const noexcept
{
    POptKvData const optKvData{this, pKvElement};
    return optKvData.StatExtBlockLen(nKeyLen, nValueLen, nReddLen);
}
/// @brief Get the corresponding KV.Value
/// @param eDataLogic
/// @param nKvIndex
/// @param pBlockIndex
/// @param pLocalData
/// @return
bool PPageOptKvStore::GetKvLocalData(EDataLogic const eDataLogic,
                                     int32_t const nKvIndex,
                                     PBlockIndex *&pBlockIndex,
                                     PDataLocal *&pLocalData) const noexcept
{
    std::ignore = pBlockIndex;
    std::ignore = pLocalData;
    POptKvData_Read optKvDataRead{this, nKvIndex};
    if (false == optKvDataRead.IsAccessReady()) {
        PER_OnOptFailed(EErrorPHKV::kIndexKvArray);
        return false;
    }
    if (false == optKvDataRead.CheckCrc()) {
        PER_OnOptFailed(EErrorPHKV::kErrorKvCRC16);
        return false;
    }
    if (optKvDataRead.ReadKvLogicData(eDataLogic) < 0) {
        // PER_OnOptFailed(EErrorPHKV::kKvDataError);
        // return false;
    }
    pLocalData  = optKvDataRead.GetLocalData();
    pBlockIndex = optKvDataRead.GetBlockData();
    return true;
}
/// @brief Get the number of the next page of the cross-page KV page
/// @return
uint32_t PPageOptKvStore::GetNextPageID() const noexcept
{
    PBlockHeadKvStore *const pKvHead{_GetKvHead()};
    PER_Assert(nullptr != pKvHead);
    return pKvHead->nNextPageID;
}
/// @brief
/// @param nPageID
void PPageOptKvStore::SetNextPageID(uint32_t const nPageID) const noexcept
{
    PBlockHeadKvStore *const pKvHead{_GetKvHead()};
    PER_Assert(nullptr != pKvHead);
    pKvHead->nNextPageID = nPageID;
    _OnOptSuccess(true);
}
/// @brief Get the number of the previous page of the cross-page KV page
/// @return
uint32_t PPageOptKvStore::GetPrevPageID() const noexcept
{
    PBlockHeadKvStore *const pKvHead{_GetKvHead()};
    PER_Assert(nullptr != pKvHead);
    return pKvHead->nPrevPageID;
}
/// @brief
/// @param nPageID
void PPageOptKvStore::SetPrevPageID(uint32_t const nPageID) const noexcept
{
    PBlockHeadKvStore *const pKvHead{_GetKvHead()};
    PER_Assert(nullptr != pKvHead);
    pKvHead->nPrevPageID = nPageID;
    _OnOptSuccess(true);
}
/// @brief Determine whether the current page is a single-page KV
/// @return
bool PPageOptKvStore::IsSimplePage() const noexcept
{
    PBlockHeadKvStore *const pKvHead{_GetKvHead()};
    PER_Assert(nullptr != pKvHead);
    if (IsValidPageID(pKvHead->nNextPageID)) {
        return false;
    }
    if (IsValidPageID(pKvHead->nPrevPageID)) {
        return false;
    }
    return true;
}
/// @brief Get the number of KV-Elements
/// @return
int32_t PPageOptKvStore::GetKvElementCount() const noexcept
{
    PBlockHeadKvStore *const pKvHead{_GetKvHead()};
    PER_Assert(nullptr != pKvHead);
    return static_cast< int32_t >(pKvHead->sElementCount);
}
/// @brief Get the space occupied by KV
/// @return
uint16_t PPageOptKvStore::GetKvSpaceUse() const noexcept
{
    PBlockHeadKvStore *const pKvHead{_GetKvHead()};
    PER_Assert(nullptr != pKvHead);
    return static_cast< uint16_t >(pKvHead->sInitLen - pKvHead->sFreeLen);
}
/// @brief Get the remaining space of KV
/// @return
uint16_t PPageOptKvStore::GetKvSpaceFree() const noexcept
{
    PBlockHeadKvStore *const pKvHead{_GetKvHead()};
    PER_Assert(nullptr != pKvHead);
    return pKvHead->sFreeLen;
}
/// @brief Get the space occupied by KV
/// @return
uint32_t PPageOptKvStore::GetKvStoreID() const noexcept
{
    PBlockHeadKvStore *const pKvHead{_GetKvHead()};
    PER_Assert(nullptr != pKvHead);
    return pKvHead->nKvStoreID;
}
/// @brief Set the KV-StoreID
/// @param nPageID
void PPageOptKvStore::SetKvStoreID(uint32_t const nPageID) const noexcept
{
    PBlockHeadKvStore *const pKvHead{_GetKvHead()};
    PER_Assert(nullptr != pKvHead);
    pKvHead->nKvStoreID = nPageID;
    _OnOptSuccess(true);
}
/// @brief Get the last page data of the cross-page Kv chain
/// @return
uint32_t PPageOptKvStore::GetLastKvPageID() const noexcept
{
    PBlockHeadKvStore *const pKvHead{_GetKvHead()};
    PER_Assert(nullptr != pKvHead);
    return pKvHead->nLastKvPageID;
}
/// @brief Set the KV-StoreID
/// @param nPageID
void PPageOptKvStore::SetLastKvPageID(uint32_t const nPageID) const noexcept
{
    PBlockHeadKvStore *const pKvHead{_GetKvHead()};
    PER_Assert(nullptr != pKvHead);
    pKvHead->nLastKvPageID = nPageID;
    _OnOptSuccess(true);
}
/// @brief Check if the index is valid
/// @param nKvIndex
/// @return
bool PPageOptKvStore::CheckElementIndex(int32_t const nKvIndex) const noexcept
{
    PBlockHeadKvStore *const pKvHead{_GetKvHead()};
    PER_Assert(nullptr != pKvHead);
    return G_CheckArrayIndex(*pKvHead, nKvIndex);
}
/// @brief Check if the hash value matches
/// @param pKvElement
/// @param nSrcKeyHash
/// @return
bool PPageOptKvStore::IsMatchHashKey(PBlockElementKv const *const pKvElement, uint64_t const nSrcKeyHash) const noexcept
{
    uint32_t const nKeyHashID32{G_GetKeyHashID(nSrcKeyHash)};
    if (pKvElement->nKeyHashID != nKeyHashID32) {
        return false;
    }
    uint64_t const nKeyHashID{_GetLocalHashKey64(pKvElement)};
    return (0U == nKeyHashID) || (nKeyHashID == nSrcKeyHash);
}
/// @brief Find the KV index via a KvElement pointer
/// @param pFindKvElement
/// @return
int32_t PPageOptKvStore::FindKvIndex(PBlockElementKv const *const pFindKvElement) const noexcept
{
    int32_t nFindIndex{-1};
    CB_EachKvIntConst pfunWork;
    pfunWork = [&nFindIndex, pFindKvElement](int32_t const nKvIndex, PBlockElementKv const *const pKvElement,
                                             PElementKvData const *) noexcept -> int32_t {
        if (nullptr == pKvElement) {
            return 0;
        }
        if (pFindKvElement == pKvElement) {
            nFindIndex = nKvIndex;
            return -1;
        }
        return 1;
    };
    std::ignore = _ForEachKv(false, pfunWork);
    return nFindIndex;
}
/// @brief Count the hash value index with the highest frequency after grouping by nHashCount
/// @param nExcepHashIndex
/// @param nHashCount
/// @return
int32_t PPageOptKvStore::StatMaxHashCountIndex(int32_t const nExcepHashIndex, uint32_t const nHashCount) const noexcept
{
    PBlockHeadKvStore *const pKvHead{_GetKvHead()};
    PER_Assert(nullptr != pKvHead);
    ara::core::Map< int32_t, int32_t > mapMaxHash;
    int32_t nMaxCount{0};
    int32_t nFindHashKey{-1};
    for (int32_t i{0}; i < static_cast< int32_t >(pKvHead->sElementCount); i++) {
        PBlockElementKv *pKvElement{nullptr};
        pKvElement = G_GetBlockElement(pKvHead, i);
        if (nullptr == pKvElement) {
            break;
        }
        int32_t const nHashIndex{TransHashIndex(pKvElement->nKeyHashID, nHashCount)};
        if (nHashIndex == nExcepHashIndex) {
            continue;
        }
        int32_t &nStatCount{mapMaxHash[nHashIndex]};
        nStatCount += 1;
        if (nStatCount > nMaxCount) {
            nMaxCount    = nStatCount;
            nFindHashKey = nHashIndex;
        }
    }
    return nFindHashKey;
}
/// @brief Count the number of Kv pairs whose HashIndex matches the input value
/// @param nStatHashIndex
/// @param nHashCount
/// @return
int32_t PPageOptKvStore::StatKvCountByHashIndex(int32_t const nStatHashIndex, uint32_t const nHashCount) const noexcept
{
    CB_EachKvIntConst pfunWork;
    pfunWork = [nStatHashIndex, nHashCount](int32_t const nIndex, PBlockElementKv const *const pKvElement,
                                            PElementKvData const *const pKvData) noexcept -> int32_t {
        std::ignore = nIndex;
        std::ignore = pKvElement;
        std::ignore = pKvData;

        if (nullptr == pKvElement) {
            return 0;
        }
        int32_t const nHashIndex{TransHashIndex(pKvElement->nKeyHashID, nHashCount)};
        if (nHashIndex != nStatHashIndex) {
            return 0;
        }
        return 1;
    };
    return _ForEachKv(false, pfunWork);
}
/// @brief Get PBlockHeadKvStore
/// @return
PBlockHeadKvStore *PPageOptKvStore::GetHeadKvStorage() const noexcept
{
    uint8_t *const pBlockHead{_GetMineBlockData()};
    if (nullptr == pBlockHead) {
        PER_OnOptFailed(EErrorPHKV::kPageNotFindBlock);
        return nullptr;
    }
    _OnOptSuccess(false);
    return T_TransPtr< PBlockHeadKvStore >(pBlockHead);
}
/// @brief Check KV storage
/// @return
bool PPageOptKvStore::CheckKvList(bool bPrint) noexcept
{
    PBlockHeadKvStore *const pKvHead{_GetKvHead()};
    PER_Assert(nullptr != pKvHead);
    uint8_t const *pBCheckBase{T_TransBytes(G_GetKvDataByOffset(pKvHead, static_cast< int32_t >(pKvHead->sInitLen)))};

    // Verify for duplicates
    PElementKvData const *pCheckKvData{nullptr};
    CB_EachKvIntConst pfunWork;
    pfunWork
        = [this, &pBCheckBase, &pCheckKvData, bPrint](int32_t const nKvIndex, PBlockElementKv const *const pKvElement,
                                                      PElementKvData const *const pKvData) noexcept -> int32_t {
        uint8_t const *const pDataBase{T_TransBytes(pKvData) + pKvElement->cLen};
        if (bPrint) {
            PConsoleLog consoleLog{"CheckKvList.", nKvIndex};
            consoleLog.PrintLog(": KVData Base = ", static_cast< void const * >(pKvData));
            consoleLog.PrintLog(", Len = ", pKvElement->cLen);
            consoleLog.PrintLog(", Offset = ", pKvElement->sOffset);
            consoleLog.PrintLog(", EndPos = ", static_cast< void const * >(pDataBase));
            consoleLog.PrintLog();
        }
        PER_Assert(pKvElement->cLen > 0U);
        PER_Assert(pCheckKvData != pKvData);
        PER_Assert((pDataBase - pBCheckBase) == 0);
        POptKvData const optKvData{this, nKvIndex};
        PER_Assert(optKvData.CheckCrc());
        pBCheckBase  = T_TransBytes(pKvData);
        pCheckKvData = pKvData;
        return 1;
    };
    int32_t nCount{0};
    nCount = _ForEachKv(false, pfunWork);
    return nCount >= static_cast< int32_t >(pKvHead->sElementCount);
}
//********************************/
/// @brief
/// @param bFromBack
/// @param pfun
/// @return
int32_t PPageOptKvStore::_ForEachKv(bool const bFromBack, CB_EachKvIntConst const &pfun) const noexcept
{
    PBlockHeadKvStore *const pBlockHead{_GetKvHead()};
    PER_Assert(nullptr != pBlockHead);
    int32_t nReturn{0};
    nReturn = 0;
    for (int32_t i{0}; i < static_cast< int32_t >(pBlockHead->sElementCount); i++) {
        int32_t nIndex{i};
        if (bFromBack) {
            nIndex = static_cast< int32_t >(pBlockHead->sElementCount) - i - 1;
        }
        PBlockElementKv *const pKvElement{G_GetBlockElement(pBlockHead, nIndex)};
        PElementKvData *const pKvData{G_GetKvDataByOffset(pBlockHead, static_cast< int32_t >(pKvElement->sOffset))};
        int32_t const nFunCode{pfun(nIndex, pKvElement, pKvData)};
        if (nFunCode < 0) {
            break;
        }
        if (nFunCode == 0) {
            continue;
        }
        nReturn += 1;
    }
    return nReturn;
}
/// @brief Calculate the effective length of the KV header: logical length
/// @return
int32_t PPageOptKvStore::_ComputeKvHeadLen() const noexcept
{
    PBlockHeadKvStore *const pKvHead{_GetKvHead()};
    PER_Assert(nullptr != pKvHead);
    int32_t nLenTotal{static_cast< int32_t >(G_GetKvHeadSize())};
    nLenTotal += static_cast< int32_t >(sizeof(PBlockElementKv)) * static_cast< int32_t >(pKvHead->sElementCount);
    return nLenTotal;
}
/// @brief Calculate the effective length of the KV tail
/// @return
int32_t PPageOptKvStore::_ComputeKvBodyLen() const noexcept
{
    int32_t nLenTotal{0};
    CB_EachKvIntConst pfunWork;
    pfunWork = [&nLenTotal](int32_t const nKvIndex, PBlockElementKv const *const pKvElement,
                            PElementKvData const *) -> int32_t {
        std::ignore = nKvIndex;
        nLenTotal += static_cast< int32_t >(pKvElement->cLen);
        return 1;
    };
    std::ignore = _ForEachKv(false, pfunWork);
    return nLenTotal;
}
//***************/
/// @brief Find the page management data area
/// @return
PBlockHeadKvStore *PPageOptKvStore::_GetKvHead() const noexcept
{
    return T_TransPtr< PBlockHeadKvStore >(_GetMineBlockData());
}
/// @brief Get the corresponding KV-Element
/// @param nKvIndex
/// @return
PBlockElementKv *PPageOptKvStore::_GetKvElement(int32_t const nKvIndex) const noexcept
{
    PBlockHeadKvStore *const pKvHead{_GetKvHead()};
    PER_Assert(nullptr != pKvHead);
    if (false == G_CheckArrayIndex(*pKvHead, nKvIndex)) {
        return nullptr;
    }
    return G_GetBlockElement(pKvHead, nKvIndex);
}
/// @brief Get the corresponding KV-Data
/// @param pKvElement
/// @return
PElementKvData *PPageOptKvStore::_GetKvData(PBlockElementKv const *const pKvElement) const noexcept
{
    if (nullptr == pKvElement) {
        return nullptr;
    }
    PBlockHeadKvStore *const pKvHead{_GetKvHead()};
    PER_Assert(nullptr != pKvHead);
    return G_GetKvDataByOffset(pKvHead, static_cast< int32_t >(pKvElement->sOffset));
}
/// @brief Move a Block: move the header up, move the tail down
/// @param pKvHead
/// @param pFindElement
/// @param nMoveHeadLen
/// @param nMoveBodyLen
void PPageOptKvStore::_MoveElement(PBlockHeadKvStore *const pKvHead,
                                   PBlockElementKv *const pFindElement,
                                   int32_t const nMoveHeadLen,
                                   int32_t const nMoveBodyLen) const noexcept
{
    PER_Assert(nullptr != pKvHead);
    PBlockElementKv *pNewKvElement{pFindElement};
    // The caller ensures that copying will not overwrite other information
    if (nMoveBodyLen != 0)  // Data area is saved in reverse order from the page tail
    {
        uint8_t *const pWorkData{
            T_TransBytes(G_GetKvDataByOffset(pKvHead, static_cast< int32_t >(pFindElement->sOffset)))};
        CopyData(pWorkData + nMoveBodyLen, pWorkData, static_cast< uint16_t >(pFindElement->cLen));
        pFindElement->sOffset = T_AddInt(pFindElement->sOffset, nMoveBodyLen);
    }

    if (nMoveHeadLen != 0)  // Head information is saved sequentially from the page head
    {
        uint16_t const nElementLenWork{static_cast< uint16_t >(sizeof(PBlockElementKv))};
        uint8_t *const pOldData{T_TransBytes(pFindElement)};
        CopyData(pOldData + nMoveHeadLen, pOldData, nElementLenWork);
        pNewKvElement = T_TransPtr< PBlockElementKv >(pOldData + nMoveHeadLen);
    }
    // If the KV data location has moved, the corresponding CRC16 needs to be updated;
    if (nMoveBodyLen
        != 0)  // Actually, only need to calculate after the Body moves; the CRC remains unchanged after the Head moves
    {
        POptKvData const optKvData{this, pNewKvElement};
        std::ignore = optKvData.UpdateCrc();
    }
}
/// @brief Move the data area of all Kv-Elements after a specific node
/// @param pKvHead
/// @param nFindKvIndex
/// @param nMoveLen
/// @return
int32_t PPageOptKvStore::_MoveListElementHead(PBlockHeadKvStore *const pKvHead,
                                              int32_t const nFindKvIndex,
                                              int32_t const nMoveLen) noexcept
{
    if (0 == nMoveLen) {
        return 0;
    }
    PER_Assert(nullptr != pKvHead);
    // Control area: moving towards larger addresses means addition
    bool bInvertedOrder{false};
    if (nMoveLen > 0) {
        bInvertedOrder = true;
    }
    CB_EachKvInt pfunWork;
    pfunWork = [this, pKvHead, nFindKvIndex, nMoveLen](int32_t const nKvIndex, PBlockElementKv *const pKvElement,
                                                       PElementKvData *) noexcept -> int32_t {
        if (nKvIndex > nFindKvIndex) {
            _MoveElement(pKvHead, pKvElement, nMoveLen, 0);
            return 1;
        }
        return 0;
    };
    int32_t nAmendCount{0};
    nAmendCount = G_ForEachKv(pKvHead, bInvertedOrder, pfunWork);
    return nAmendCount;
}
/// @brief Move the KV data area of all nodes after a specific node
/// @param pKvHead
/// @param nFindKvIndex
/// @param nMoveLen
/// @return
int32_t PPageOptKvStore::_MoveListElementBody(PBlockHeadKvStore *const pKvHead,
                                              int32_t const nFindKvIndex,
                                              int32_t const nMoveLen) noexcept
{
    if (0 == nMoveLen) {
        return 0;
    }
    PER_Assert(nullptr != pKvHead);
    // Control area: moving towards larger addresses means deletion
    bool bFromBackToFront{true};
    if (nMoveLen > 0) {
        bFromBackToFront = false;
    }
    CB_EachKvInt pfunWork;
    pfunWork = [this, pKvHead, nFindKvIndex, nMoveLen](int32_t const nKvIndex, PBlockElementKv *const pKvElement,
                                                       PElementKvData *const) noexcept -> int32_t {
        if (nKvIndex > nFindKvIndex) {
            _MoveElement(pKvHead, pKvElement, 0, nMoveLen);
            return 1;
        }
        return 0;
    };
    return G_ForEachKv(pKvHead, bFromBackToFront, pfunWork);
}
/// @brief Initialize KV-Element data
/// @param pKvHead
/// @param pKvElement
/// @param pBData
/// @param nDataLen
/// @param nKeyHashID
/// @param nReddType
void PPageOptKvStore::_SaveKvElementData(PBlockHeadKvStore const *const pKvHead,
                                         PBlockElementKv *const pKvElement,
                                         uint8_t const *const pBData,
                                         uint8_t const nDataLen,
                                         uint32_t const nKeyHashID,
                                         uint8_t const nReddType) const noexcept
{
    PER_Assert(nullptr != pKvHead);
    PER_Assert(nullptr != pKvElement);
    int32_t const nDataBaseOffset{static_cast< int32_t >(G_GetKvHeadSize())
                                  + static_cast< int32_t >(pKvHead->sElementCount)
                                        * static_cast< int32_t >(sizeof(PBlockElementKv))};
    int32_t const nOffset{nDataBaseOffset + static_cast< int32_t >(pKvHead->sFreeLen)
                          - static_cast< int32_t >(nDataLen)};
    // Fill KvElement header
    pKvElement->nKeyHashID = nKeyHashID;
    pKvElement->sOffset    = static_cast< uint16_t >(nOffset);
    pKvElement->cLen       = nDataLen;
    pKvElement->cReddType  = nReddType;
    // Fill Kv data
    POptKvData const optKvData{this, pKvElement};
    std::ignore = optKvData.ReplaceKvData(pBData, static_cast< uint16_t >(nDataLen));
}
/// @brief
/// @param pKvHead
/// @param pKvElement
/// @param pBData
/// @param nDataLen
void PPageOptKvStore::_KvElementReplace(PBlockHeadKvStore *const pKvHead,
                                        PBlockElementKv *const pKvElement,
                                        uint8_t const *const pBData,
                                        uint16_t const nDataLen) const noexcept
{
    PER_Assert(nullptr != pKvHead);
    PER_Assert(nullptr != pKvElement);
    int32_t const nAmendLen{static_cast< int32_t >(pKvElement->cLen) - static_cast< int32_t >(nDataLen)};
    // New position algorithm: add for shrinking table (move backward), reduce for expanding table (move forward)
    uint16_t const nNewOffset{T_AddInt< uint16_t >(pKvElement->sOffset, -1 * nAmendLen)};
    PElementKvData *const pKvData{G_GetKvDataByOffset(pKvHead, static_cast< int32_t >(nNewOffset))};

    POptKvData const optKvData{this, pKvElement, pKvData};
    std::ignore         = optKvData.ReplaceKvData(pBData, nDataLen);
    pKvElement->cLen    = static_cast< uint8_t >(nDataLen);
    pKvElement->sOffset = nNewOffset;
}
/// @brief Get the possibly existing 64-bit KeyHash
/// @param pKvElement
/// @return
uint64_t PPageOptKvStore::_GetLocalHashKey64(PBlockElementKv const *const pKvElement) const noexcept
{
    PER_Assert(nullptr != pKvElement);
    PElementKvData const *pKvData{nullptr};
    pKvData = G_GetKvDataByOffset(_GetKvHead(), static_cast< int32_t >(pKvElement->sOffset));
    uint64_t nKeyHashID{0U};
    if ((static_cast< int32_t >(pKvData->cKeyWhole) <= 0)
        && (pKvData->cFormatKey == static_cast< uint8_t >(EDataFormat::kBlockIndex))) {
        POptKvData_Read optKvDataRead{this, pKvElement};
        if (false == optKvDataRead.IsAccessReady()) {
            return nKeyHashID;
        }
        // It's best not to have a Key spanning multiple pages
        if ((false == optKvDataRead.IsKeyFormat(EDataFormat::kBlockIndex))
            && (false == optKvDataRead.IsKeyFormat(EDataFormat::kHugeBlock))) {
            return nKeyHashID;
        }
        PDataLocal *const pLocalValue{optKvDataRead.ReadLocalCrc64()};
        CopyData(&nKeyHashID, pLocalValue + 1, sizeof(nKeyHashID));
    }
    return nKeyHashID;
}
//********************************/
}  // namespace isoftkv
}  // namespace per
}  // namespace ara
