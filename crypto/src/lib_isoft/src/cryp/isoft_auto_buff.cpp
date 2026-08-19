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
/// @brief      AutoSar-Crypto Encryption and Decryption Module
/// @details
/// @date       2022-02-17
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/CRYPTO/Reusable Functions/Reusable Function Module
/// @interface_level=unit
/// @trace_id_sr=SR_CRYPTO_06005
/// @unit_name=PAutoBuff
/// @unit_description=Automatically managed heap memory Buff encapsulation
/// @endcode
///
/// ================================================================

#include "ara/crypto/cryp/isoft_auto_buff.h"

#include <cstring>
#include <functional>

#include "ara/crypto/common/isoft_common_api.h"

namespace {
/// @brief Allocate memory
/// @name  NewBuff
/// @param nBuffSize Capacity of the BUFF
/// @returns Starting address of the allocated memory
uint8_t* NewBuff(uint32_t const nBuffSize) noexcept
{
    if (nBuffSize <= 0U) {
        return nullptr;
    }
    uint8_t* const pBuffData{new uint8_t[static_cast< size_t >(nBuffSize)]};
    std::ignore = memset(pBuffData, 0, static_cast< std::size_t >(nBuffSize));
    return pBuffData;
}
}  // namespace

namespace ara {
namespace crypto {
namespace internal {

//********************************/
/// @brief Constructor: initialize only the buffer length
/// @param nBuffSize Capacity of the BUFF
/// @returns
PAutoBuff::PAutoBuff(uint32_t const nBuffSize) noexcept : PAutoBuff{}
{
    pBuffData_ = NewBuff(nBuffSize);
    nBuffSize_ = nBuffSize;
}
/// @brief Override move assignment function
/// @param other Another object instance of this class
/// @returns  *this
PAutoBuff& PAutoBuff::operator=(PAutoBuff&& other) & noexcept
{
    _DelBuff();
    pBuffData_       = other.pBuffData_;
    nBuffSize_       = other.nBuffSize_;
    nDataLen_        = other.nDataLen_;
    other.pBuffData_ = nullptr;
    other.nBuffSize_ = 0U;
    other.nDataLen_  = 0U;
    std::ignore      = std::move(other);
    return *this;
}
/// @brief Destructor
PAutoBuff::~PAutoBuff() noexcept { _DelBuff(); }
//***************/
/// @brief Return the data area
/// @name  Data
/// @param nIndex Index
/// @returns Starting address of the data area
uint8_t* PAutoBuff::Data(uint32_t const nIndex) const noexcept
{
    if (nIndex >= nBuffSize_) {
        return nullptr;
    }
    return pBuffData_ + nIndex;
}
/// @brief Get data at a specific position
/// @name  at
/// @param nIndex Index
/// @returns Data at the specific position
uint8_t PAutoBuff::at(uint32_t const nIndex) const noexcept { return *(pBuffData_ + nIndex); }
/// @brief Reset the buffer: may allocate new memory
/// @name  ResetBuff
/// @param nBuffSize Capacity of the BUFF
void PAutoBuff::ResetBuff(uint32_t nBuffSize) noexcept
{
    _DelBuff();
    nBuffSize  = (nBuffSize + kInt_7U) & (~kInt_7U);
    pBuffData_ = NewBuff(nBuffSize);
    nBuffSize_ = nBuffSize;
    nDataLen_  = 0U;
}
/// @brief Reset the data area
/// @name  ResetData
void PAutoBuff::ResetData() noexcept
{
    std::ignore = memset(pBuffData_, 0, static_cast< std::size_t >(nBuffSize_));
    nDataLen_   = 0U;
}
/// @brief Set the data area starting from the head of the data area
/// @name  SetData
/// @param pData Data: starting address of memory
/// @param nLen Data length: in bytes
/// @param bForceNew Whether to force new creation
/// @returns  true if set data sucess false otherwise
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
        std::ignore = memcpy(pBuffData_, pData,
                             static_cast< std::size_t >(nLen));  // Not responsible for solving overlap issues
    }
    nDataLen_ = nLen;
    return true;
}
/// @brief Append data starting from the tail of the data area
/// @name  AddData
/// @param pData Data: starting address of memory
/// @param nLen Data length: in bytes
/// @returns  ture if add data sucess false otherwise
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
/// @param pData Data: starting address of memory
/// @param nDataLen Data length
/// @returns  ture if add data sucess false otherwise
bool PAutoBuff::AddData(uint8_t const pData, uint32_t const nDataLen) noexcept
{
    if ((nDataLen_ + nDataLen) > nBuffSize_) {
        return false;
    }
    for (uint32_t i{0U}; i < nDataLen; ++i) {
        *(pBuffData_ + nDataLen_ + i) = pData;
    }
    nDataLen_ += nDataLen;
    return true;
}
/// @brief Increase data length
/// @param nDataLen Data length
void PAutoBuff::AddDataLen(int32_t const nDataLen) noexcept { nDataLen_ = T_AddInt(nDataLen_, nDataLen); }
//***************/
/// @brief Free memory
/// @name  _DelBuff
void PAutoBuff::_DelBuff() noexcept
{
    if (pBuffData_ != nullptr) {
        delete[] pBuffData_;
    }
    pBuffData_ = nullptr;
}
//********************************/
}  // namespace internal
}  // namespace crypto
}  // namespace ara
