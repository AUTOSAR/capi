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
/// @file       isoft_check_redd.h
/// @brief      AutoSar-AP data persistence storage module
/// @details    Detect M/N redundancy errors
/// @date       2023-07-10
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr><td>2023-07-10  <td>1.0.0    <td>hanjingjing      <td>Create initial version
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/per/Common/Reusable Functions
/// @interface_level=module
/// @trace_id_sr=
/// @unit_name=
/// @unit_description=Detect M/N redundancy errors
/// @endcode
///
/// ================================================================

#ifndef ARA_PER_COMMON_PH_CHECK_REDD_H_
#define ARA_PER_COMMON_PH_CHECK_REDD_H_

#include "ara/core/vector.h"

namespace ara {
namespace per {
namespace isoftkv {
//********************************/
/// @brief Structure for checking redundancy errors in KV library
/// @code{.isoft}
/// @unit_name=PReddCheckData_Kv
/// @endcode
struct PReddCheckData_Kv final
{
    /// @brief Error page number: 0 is invalid value
    uint32_t nPageID{0U};
    /// @brief Selected correct redundancy index, nSelectIndex=-1 indicates all redundancy is broken
    uint8_t nSelectIndex{0U};
    /// @brief Redundancy index needing recovery
    ara::core::Vector< uint8_t > vecNeedRecover;
};
/// @brief Redundancy error detection
/// @code{.isoft}
/// @unit_name=PReddCheckData_File
/// @endcode
struct PReddCheckData_File final
{
    /// @brief Error file cursor
    int64_t nPosStart{0U};
    /// @brief Error data length
    uint32_t nDataLen{0U};
    /// @brief Selected correct redundancy index, nSelectIndex=-1 indicates all redundancy is broken
    uint8_t nSelectIndex{0U};
    /// @brief Redundancy index needing recovery
    ara::core::Vector< uint8_t > vecNeedRecover;
};
//********************************/
/// @brief Operation class for checking redundancy errors
/// @code{.isoft}
/// @unit_name=PCheckRedd
/// @endcode
template < typename T_CheckData >
class PCheckRedd
{
private:
    /// @brief List of PageIDs with detected redundancy errors, cleared only after recovery attempts
    ara::core::Vector< T_CheckData > vecReddCheck_;

public:
    ara::core::Vector< T_CheckData > &GetVecReddCheck() noexcept { return vecReddCheck_; }
    /// @brief Add a detected redundancy recovery request data
    /// @param reddCheckData
    void AddReddCheck(T_CheckData const &reddCheckData) noexcept { vecReddCheck_.push_back(reddCheckData); }
    /// @brief Check if there is a redundancy recovery request
    /// @return Redundancy data needing recovery
    T_CheckData FindRecoverData() const noexcept
    {
        T_CheckData reddCheckData;
        if (false == vecReddCheck_.empty()) {
            reddCheckData = vecReddCheck_.front();
        }
        return reddCheckData;
    }
};
//********************************/
/// @brief Check if KV system has redundancy errors
/// @code{.isoft}
/// @unit_name=PCheckRedd_Kv
/// @endcode
class PCheckRedd_Kv final : public PCheckRedd< PReddCheckData_Kv >
{
public:
    /// @brief Delete a pending recovery redundancy request
    /// @param nPageID
    void DelReddCheck(uint32_t const nPageID) noexcept
    {
        ara::core::Vector< PReddCheckData_Kv > &vecReddCheck{GetVecReddCheck()};
        // Delete only the foremost redundancy recovery request
        for (ara::core::Vector< PReddCheckData_Kv >::iterator it = vecReddCheck.begin(); it != vecReddCheck.end();
             it++) {
            if (it->nPageID == nPageID) {
                vecReddCheck.erase(it);
                break;
            }
        }
    }
};
/// @brief Check if File system has redundancy errors
/// @code{.isoft}
/// @unit_name=PCheckRedd_File
/// @endcode
class PCheckRedd_File final : public PCheckRedd< PReddCheckData_File >
{
public:
    /// @brief Delete a redundancy recovery request
    /// @param nPosStart
    /// @param nDataLen
    void DelReddCheck(int64_t const nPosStart, uint32_t const nDataLen) noexcept
    {
        ara::core::Vector< PReddCheckData_File > &vecReddCheck{GetVecReddCheck()};
        // Delete only the foremost redundancy recovery request
        for (ara::core::Vector< PReddCheckData_File >::iterator it = vecReddCheck.begin(); it != vecReddCheck.end();
             it++) {
            if ((it->nPosStart == nPosStart) && (it->nDataLen <= nDataLen)) {
                vecReddCheck.erase(it);
                break;
            }
        }
    }
};
//********************************/
}  // namespace isoftkv
}  // namespace per
}  // namespace ara
#endif
