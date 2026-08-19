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
/// @file       isoft_select_mofn.h
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

#ifndef ARA_PER_COMMON_PH_SELECT_MOFN_H_
#define ARA_PER_COMMON_PH_SELECT_MOFN_H_

#include <functional>

#include "ara/core/string_view.h"
#include "ara/core/vector.h"
#include "ara/per/internal/common/isoft_file_opt_page.h"

namespace ara {
namespace per {
namespace isoftkv {
//********************************/
/// @brief Calculate 16-bit CRC checksum
/// @code{.isoft}
/// @unit_name=CalculateCrc16
/// @endcode
/// @param pVoidData
/// @param nDataLen
/// @param nLastCrc
/// @return
uint16_t CalculateCrc16(const void *const pVoidData, uint32_t const nDataLen, uint16_t const nLastCrc) noexcept;
/// @brief Calculate 32-bit CRC checksum
/// @code{.isoft}
/// @unit_name=CalculateCrc32
/// @endcode
/// @param pVoidData
/// @param nDataLen
/// @param nLastCrc
/// @return
uint32_t CalculateCrc32(const void *const pVoidData, uint32_t const nDataLen, uint32_t const nLastCrc) noexcept;
//********************************/
/// @brief M/N election algorithm
/// @code{.isoft}
/// @unit_name=PSelectMofN
/// @endcode
class PSelectMofN final
{
private:
    /// @brief Number of valid copies
    uint32_t nDataM_;
    /// @brief Number of invalid copies
    uint32_t nDataN_;

public:
    /// @brief Calculate file Hash code
    /// @param pFileOpt
    /// @return
    static uint32_t CalFileHashID(PFileOpt const *const pFileOpt) noexcept;
    /// @brief Calculate file Hash code
    /// @param stFileName
    /// @return
    static uint32_t CalFileHashID(ara::core::StringView const &stFileName) noexcept;
    /// @brief Calculate file Hash code
    /// @param pFileOpt
    /// @param nPageLen
    /// @return
    static uint32_t CalFileHashID(PFileOpt_Page const *const pFileOpt, uint32_t const nPageLen) noexcept;

public:
    /// @brief Parameterized constructor
    /// @param m
    /// @param n
    PSelectMofN(uint32_t const m, uint32_t const n) noexcept;
    /// @brief Destructor
    ~PSelectMofN() = default;
    /// @brief
    /// @param  a
    PSelectMofN(PSelectMofN const &a) = delete;
    /// @brief
    /// @param  a
    /// @return
    PSelectMofN &operator=(PSelectMofN const &a) = delete;
    /// @brief
    /// @param  a
    PSelectMofN(PSelectMofN &&a) = delete;
    /// @brief
    /// @param  a
    /// @return
    PSelectMofN &operator=(PSelectMofN &&a) = delete;
    /// @brief Generate original candidate data using callback function, return selected majority representative index
    /// @param nDirtyData
    /// @param pfun
    /// @return
    int32_t SelectGoal(uint64_t const nDirtyData, CB_DealInt32 const &pfun) const noexcept;

protected:
    /// @brief Select majority representative from vecHashID candidate list
    /// @param nDirtyData
    /// @param vecHashID
    /// @return
    int32_t _SelectGoal(uint64_t const nDirtyData, ara::core::Vector< uint64_t > &vecHashID) const noexcept;
};
//********************************/
/// @brief Encapsulation of election algorithm
/// @code{.isoft}
/// @unit_name=SelectMofN
/// @endcode
/// @tparam T_Data Result to calculate
/// @param m MofN.m
/// @param n MofN.n
/// @param pfunWork Specific calculation operation function
/// @return Election result
template < typename T_Data >
bool SelectMofN(T_Data &data,
                uint32_t const m,
                uint32_t const n,
                std::function< T_Data(int32_t const nIndex, uint32_t &nHashData) > pfunWork) noexcept
{
    ara::core::Vector< T_Data > vecFileHashList;
    isoftkv::CB_DealInt32 pfunInt32;
    pfunInt32 = [&vecFileHashList, &pfunWork](int32_t const nReddIndex) -> uint64_t {
        uint32_t nHashData{0};
        vecFileHashList.push_back(pfunWork(nReddIndex, nHashData));
        return static_cast< uint64_t >(nHashData);
    };
    PSelectMofN const selectMofN{m, n};
    int32_t nFindReddIndex{0};
    nFindReddIndex = selectMofN.SelectGoal(0U, pfunInt32);
    if (nFindReddIndex == -1) {
        return false;
    }
    if (nFindReddIndex >= static_cast< int32_t >(vecFileHashList.size())) {
        return false;
    }
    data = std::move(vecFileHashList[nFindReddIndex]);
    return true;
}
//********************************/
}  // namespace isoftkv
}  // namespace per
}  // namespace ara

#endif
