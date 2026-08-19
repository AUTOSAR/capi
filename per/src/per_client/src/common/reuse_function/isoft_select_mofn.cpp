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
/// @file       isoft_select_mofn.cpp
/// @brief      AutoSar-AP data persistence storage module
/// @details    Puhua M/N election algorithm
/// @date       2021-11-12
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr><td>2021-11-12  <td>1.0.0    <td>hanjingjing      <td>Create initial version
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

#include "ara/per/internal/common/isoft_select_mofn.h"

#include "ara/core/map.h"
#include "ara/per/internal/isoftkv/kv_cache_page.h"
#include "isoft/crc/crc.h"

namespace ara {
namespace per {
namespace isoftkv {
//********************************/
/// @brief Calculate 16-bit CRC checksum
/// @param pVoidData
/// @param nDataLen
/// @param nLastCrc
/// @return
uint16_t CalculateCrc16(const void* const pVoidData, uint32_t const nDataLen, uint16_t const nLastCrc) noexcept
{
    isoft::crc::BufferView const buffView{T_TransBytes< void >(pVoidData), static_cast< std::size_t >(nDataLen)};
    return isoft::crc::CRC::CalculateCRC16(buffView, nLastCrc == 0U, nLastCrc);
}
/// @brief Calculate 32-bit CRC checksum
/// @param pVoidData
/// @param nDataLen
/// @param nLastCrc
/// @return
uint32_t CalculateCrc32(const void* const pVoidData, uint32_t const nDataLen, uint32_t const nLastCrc) noexcept
{
    isoft::crc::BufferView const buffView{T_TransBytes< void >(pVoidData), static_cast< std::size_t >(nDataLen)};
    return isoft::crc::CRC::CalculateCRC32(buffView, nLastCrc == 0U, nLastCrc);
}
//********************************/
/// @brief Calculate full-text Hash value of redundant file
/// @param stFileName
/// @return
uint32_t PSelectMofN::CalFileHashID(ara::core::StringView const& stFileName) noexcept
{
    PFileOpt fileOpt{isoftkv::PFileOpt::ReadOnly()};
    if (false == fileOpt.DoPrepareWork(stFileName, 0U)) {
        return 0U;
    }
    return CalFileHashID(&fileOpt);
}
/// @brief Calculate full-text Hash value of redundant file
/// @param pFileOpt
/// @return
uint32_t PSelectMofN::CalFileHashID(PFileOpt const* const pFileOpt) noexcept
{
    if (nullptr == pFileOpt) {
        return 0U;
    }
    int64_t const nFileSize{pFileOpt->GetFileSize()};
    PMemoryPage const pageRead{0U, PFileOpt::GetNaiSectorSize()};
    uint32_t nHashCrcID{0U};
    int64_t nReadTotal{0};
    while (true) {
        if (nReadTotal >= nFileSize) {
            break;
        }
        int32_t const nReadLen{pFileOpt->ReadData(pageRead.GetCache(), pageRead.GetCacheLen())};
        if (nReadLen < 0) {
            break;
        }
        nReadTotal += nReadLen;
        nHashCrcID = CalculateCrc32(pageRead.GetCache(), static_cast< uint32_t >(nReadLen), nHashCrcID);
    }
    return nHashCrcID;
}
/// @brief Calculate full-text Hash value of redundant file
/// @param pFileOpt
/// @param nPageLen
/// @return
uint32_t PSelectMofN::CalFileHashID(PFileOpt_Page const* const pFileOpt, uint32_t const nPageLen) noexcept
{
    if (nullptr == pFileOpt) {
        return 0U;
    }
    uint32_t nFileHashID{0U};  // Calculate Hash of entire file
    PMemoryPage const pageRedd{0U, nPageLen};
    uint64_t const nPageTotal{static_cast< uint64_t >(pFileOpt->GetFileSize()) / static_cast< uint64_t >(nPageLen)};
    uint32_t nPageIndex{0U};
    while (nPageIndex < nPageTotal) {
        uint32_t const nPageID{nPageIndex + 1U};
        int32_t const nReadLen{pFileOpt->ReadPage(nPageID, pageRedd.GetCache(), pageRedd.GetCacheLen())};
        if (nReadLen < 0) {
            break;
        }
        nPageIndex += 1U;
        nFileHashID = CalculateCrc32(pageRedd.GetCache(), static_cast< uint32_t >(nReadLen), nFileHashID);
    }
    return nFileHashID;
}
//********************************/
/// @brief M/N redundancy constructor
/// @param m
/// @param n
PSelectMofN::PSelectMofN(uint32_t const m, uint32_t const n) noexcept : nDataM_{m}, nDataN_{n} {}
/// @brief Generate original candidate data using callback function, return selected majority representative index
/// @param nDirtyData
/// @param pfun
/// @return
int32_t PSelectMofN::SelectGoal(uint64_t const nDirtyData, CB_DealInt32 const& pfun) const noexcept
{
    ara::core::Vector< uint64_t > vecHashID;
    for (int32_t i{0}; i < static_cast< int32_t >(nDataN_); i++) {
        vecHashID.push_back(pfun(i));
    }
    return _SelectGoal(nDirtyData, vecHashID);
}
//***************/
/// @brief M/N redundancy election algorithm
/// @param nDirtyData
/// @param vecHashID
/// @return
int32_t PSelectMofN::_SelectGoal(uint64_t const nDirtyData, ara::core::Vector< uint64_t >& vecHashID) const noexcept
{
    // Count votes
    ara::core::Map< uint64_t, uint32_t > mapStatSame;
    for (uint64_t const& nHashID : vecHashID) {
        if (nHashID != nDirtyData) {
            mapStatSame[nHashID] += 1U;
        }
    }
    // Find majority
    uint32_t nMaxSameCount{0U};
    uint64_t nFindHashID{static_cast< uint64_t >(-1)};
    for (auto const it : mapStatSame) {
        if (it.second > nMaxSameCount) {
            nMaxSameCount = it.second;
            nFindHashID   = it.first;
        }
    }
    // Check if majority exceeds standard value
    if (static_cast< uint32_t >(nMaxSameCount) < nDataM_) {
        return -1;
    }
    // Find representative of majority
    int32_t nFindIndex{-1};
    for (std::size_t i{0U}; i < vecHashID.size(); i++) {
        if (vecHashID[i] == nFindHashID) {
            nFindIndex = static_cast< int32_t >(i);
            break;
        }
    }
    return nFindIndex;
}
//********************************/
}  // namespace isoftkv
}  // namespace per
}  // namespace ara
