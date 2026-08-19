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
/// @file       isoft_auto_buff.cpp
/// @brief      AutoSar-AP data persistence storage module
/// @details    Automatic buffer
/// @date       2021-05-18
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr><td>2021-05-18  <td>1.0.0    <td>hanjingjing      <td>Create initial version
/// <tr><td>2023-01-03  <td>1.0.0    <td>hanjingjing      <td>Refactor file directory and crypto support
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/per/Common/Reusable Functions
/// @interface_level=module
/// @trace_id_sr=
/// @unit_name=PAutoBuff
/// @unit_description=Automatic buffer
/// @endcode
///
/// ================================================================

#include "ara/per/internal/common/isoft_auto_buff.h"

#include "ara/per/internal/common/isoft_assert.h"
#include "ara/per/internal/common/isoft_common_api.h"

namespace {
/// @brief Allocate a new memory block
/// @param nBuffSize
/// @return
uint8_t* G_NewBuff(uint32_t const nBuffSize) noexcept
{
    PER_Assert(nBuffSize > 0U);
    uint8_t* const pNewBuff{new uint8_t[static_cast< std::size_t >(nBuffSize)]};
    if (nullptr != pNewBuff) {
        std::ignore = memset(pNewBuff, 0, static_cast< std::size_t >(nBuffSize));
    }
    return pNewBuff;
}
}  // namespace

namespace ara {
namespace per {
namespace isoftkv {
//********************************/
/// @brief Constructor: Default
PAutoBuff::PAutoBuff() noexcept : PAutoBuff{nullptr, 0U, 0U} {}
/// @brief Constructor: Initialize only buffer length
/// @param nBuffSize
PAutoBuff::PAutoBuff(uint32_t const nBuffSize) noexcept : PAutoBuff{nullptr, T_AlignNumberUp(nBuffSize, kInt_64U), 0U}
{
}
/// @brief Constructor: No data initialization, but with data length
/// @param nBuffSize
/// @param nDataLen
PAutoBuff::PAutoBuff(uint32_t const nBuffSize, uint32_t const nDataLen) noexcept
    : PAutoBuff{nullptr, T_AlignNumberUp(nBuffSize, kInt_64U), nDataLen}
{
}
/// @brief Constructor: With data initialization
/// @param pBuffData
/// @param nBuffSize
/// @param nDataLen
PAutoBuff::PAutoBuff(uint8_t* const pBuffData, uint32_t const nBuffSize, uint32_t const nDataLen) noexcept
    : pBuffData_{pBuffData}, nBuffSize_{nBuffSize}, nDataLen_{nDataLen}
{
    if ((nullptr == pBuffData_) && (nBuffSize_ > 0U)) {
        pBuffData_ = G_NewBuff(nBuffSize_);
    }
}
/// @brief Destructor
/// @name  operator=
/// @param  other
/// @brief Override move assignment function
/// @throws
PAutoBuff& PAutoBuff::operator=(PAutoBuff&& other) & noexcept
{
    _DelBuff();
    pBuffData_       = other.pBuffData_;
    nBuffSize_       = other.nBuffSize_;
    nDataLen_        = other.nDataLen_;
    other.pBuffData_ = nullptr;
    other.nBuffSize_ = 0U;
    other.nDataLen_  = 0U;
    return *this;
}
/// @brief Return data area
PAutoBuff::~PAutoBuff() noexcept { _DelBuff(); }
//***************/
/// @brief Get data at specific position
/// @name  data
/// @param  nIndex
/// @returns  uint8_t*
uint8_t* PAutoBuff::data(uint32_t const nIndex) const noexcept
{
    if (nIndex >= nBuffSize_) {
        return nullptr;
    }
    return pBuffData_ + nIndex;
}
/// @brief Reset buffer: May allocate new memory
/// @param nIndex
/// @return
uint8_t PAutoBuff::at(uint32_t const nIndex) const noexcept { return *(pBuffData_ + nIndex); }
/// @brief Reset data area
/// @param nBuffSize
void PAutoBuff::ResetBuff(uint32_t nBuffSize) noexcept
{
    nBuffSize = T_AlignNumberUp(nBuffSize, kInt_64U);
    if (nBuffSize <= 0) {
        return;
    }
    if ((nBuffSize_ > 0U) && (nBuffSize_ == nBuffSize)) {
        return;
    }
    _DelBuff();
    _NewBuff(nBuffSize);
}
/// @brief Set data area starting from head of data area
/// @name  ResetData
/// @returns  void
void PAutoBuff::ResetData() noexcept
{
    std::ignore = memset(pBuffData_, 0, static_cast< std::size_t >(nBuffSize_));
    nDataLen_   = 0U;
}
/// @brief Append data starting from tail of data area
/// @name  SetData
/// @param  pData
/// @param  nLen
/// @param  bForceNew
/// @returns  bool
bool PAutoBuff::SetData(uint8_t const* const pData, uint32_t const nLen, bool bForceNew) noexcept
{
    if (nLen > nBuffSize_) {
        bForceNew = true;
    }
    if (bForceNew) {
        uint32_t const nBuffSize{(nLen + 7U) & (~7U)};
        ResetBuff(nBuffSize);
    }
    if (pData != nullptr) {
        std::ignore = memcpy(pBuffData_, pData, static_cast< std::size_t >(nLen));  // Does not handle overwrite issues
    }
    nDataLen_ = nLen;
    return true;
}
/// @brief Add multiple identical data
/// @name  AddData
/// @param  pData
/// @param  nLen
/// @returns  bool
bool PAutoBuff::AddData(uint8_t const* const pData, uint32_t const nLen) noexcept
{
    if ((nDataLen_ + nLen) > nBuffSize_) {
        return false;
    }
    std::ignore = memcpy(pBuffData_ + nDataLen_, pData, static_cast< std::size_t >(nLen));
    nDataLen_ += nLen;
    return true;
}
/// @brief Add multiple identical data
/// @name  AddData
/// @param  pData
/// @param  nCount
/// @returns  bool
bool PAutoBuff::AddData(uint8_t const pData, uint32_t const nCount) noexcept  // Add multiple identical data
{
    if ((nDataLen_ + nCount) > nBuffSize_) {
        return false;
    }
    for (uint32_t i{0U}; i < nCount; ++i) {
        pBuffData_[nDataLen_ + i] = pData;
    }
    nDataLen_ += nCount;
    return true;
}
/// @brief Increase data length
/// @param nDataLen
/// @return
void PAutoBuff::AddDataLen(int32_t const nDataLen) noexcept { nDataLen_ = T_AddInt(nDataLen_, nDataLen); }
//********************************/
/// @brief Allocate memory
/// @param  nBuffSize
void PAutoBuff::_NewBuff(uint32_t const nBuffSize) noexcept
{
    PER_Assert(nBuffSize > 0U);
    pBuffData_ = G_NewBuff(nBuffSize);
    nBuffSize_ = nBuffSize;
    nDataLen_  = 0U;
}
/// @brief Free memory
void PAutoBuff::_DelBuff() noexcept
{
    if (nullptr != pBuffData_) {
        delete[] pBuffData_;
    }
    pBuffData_ = nullptr;
    nBuffSize_ = 0U;
    nDataLen_  = 0U;
}
//********************************/
}  // namespace isoftkv
}  // namespace per
}  // namespace ara
