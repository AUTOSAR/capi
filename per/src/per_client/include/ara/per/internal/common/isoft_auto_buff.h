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
/// @file       isoft_auto_buff.h
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

#ifndef ARA_PER_COMMON_PH_AUTO_BUFF_H_
#define ARA_PER_COMMON_PH_AUTO_BUFF_H_

#include <cstdint>

#include "ara/per/internal/common/isoft_data_type.h"

namespace ara {
namespace per {
namespace isoftkv {
//********************************/

/// @brief Persistence-owned memory block management
class PAutoBuff final
{
private:
    /// @brief Starting address of memory space
    uint8_t *pBuffData_;
    /// @brief Size of memory space
    uint32_t nBuffSize_;
    /// @brief Data length in memory space
    uint32_t nDataLen_;

protected:
public:
    /// @brief Constructor: Default
    PAutoBuff() noexcept;
    /// @brief Constructor: Initialize only buffer length
    /// @param nBuffSize
    explicit PAutoBuff(uint32_t const nBuffSize) noexcept;
    /// @brief No data initialization, but with data length
    /// @param nBuffSize
    /// @param nDataLen
    PAutoBuff(uint32_t const nBuffSize, uint32_t const nDataLen) noexcept;
    /// @brief Constructor: Initialization with data
    /// @param pBuffData
    /// @param nBuffSize
    /// @param nDataLen
    PAutoBuff(uint8_t *const pBuffData, uint32_t const nBuffSize, uint32_t const nDataLen) noexcept;
    /// @brief Destructor
    ~PAutoBuff() noexcept;
    /// @brief Copy constructor: Deleted
    /// @param a
    PAutoBuff(PAutoBuff const &a) = delete;
    /// @brief Move assignment function: Deleted
    /// @param a
    PAutoBuff(PAutoBuff &&a) noexcept = delete;
    /// @brief Copy assignment function: Deleted
    /// @param a
    /// @return
    PAutoBuff &operator=(PAutoBuff const &a) = delete;
    /// @brief Override move assignment function
    /// @name
    /// @param  other
    /// @returns
    /// @throws
    PAutoBuff &operator=(PAutoBuff &&other) &noexcept;

public:
    /// @brief Return data area
    /// @name  data
    /// @param  nIndex
    /// @returns  uint8_t*
    uint8_t *data(uint32_t const nIndex = 0U) const noexcept;  // NOLINT
    /// @brief Return data length
    /// @name  size
    /// @returns  uint32_t
    inline uint32_t size() const noexcept  // NOLINT
    {
        return nDataLen_;
    }
    /// @brief Get data at specific position
    /// @param nIndex
    /// @return
    uint8_t at(uint32_t const nIndex) const noexcept;  // NOLINT
    /// @brief Get buffer length
    /// @name  GetBuffLen
    /// @returns  uint32_t
    inline uint32_t GetBuffLen() const noexcept { return nBuffSize_; }
    /// @brief Reset buffer: May allocate new memory
    /// @param nBuffSize
    void ResetBuff(uint32_t nBuffSize) noexcept;
    /// @brief Reset data area
    /// @name   ResetData
    /// @returns
    void ResetData() noexcept;
    /// @brief Set data area starting from head of data area
    /// @name   SetData
    /// @param  pData
    /// @param  nLen
    /// @param  bForceNew
    /// @returns
    bool SetData(uint8_t const *const pData, uint32_t const nLen, bool bForceNew = true) noexcept;
    /// @brief Append data starting from tail of data area
    /// @name   AddData
    /// @param  pData
    /// @param  nLen
    /// @returns
    bool AddData(uint8_t const *const pData, uint32_t const nLen) noexcept;
    /// @brief Add multiple identical data
    /// @name  AddData
    /// @param  pData
    /// @param  nCount
    /// @returns
    bool AddData(uint8_t const pData, uint32_t const nCount) noexcept;
    /// @brief Set data length
    /// @name  SetDataSize
    /// @param  nSize
    /// @returns  void
    inline void SetDataSize(uint32_t const nSize) noexcept { nDataLen_ = nSize; }
    /// @brief Set data length
    /// @param nDataLen
    /// @return
    inline void SetDataLen(uint32_t const nDataLen) noexcept { nDataLen_ = nDataLen; }
    /// @brief Increase data length
    /// @param nDataLen
    /// @return
    void AddDataLen(int32_t const nDataLen) noexcept;

protected:
    /// @brief Allocate memory
    /// @param  nBuffSize
    void _NewBuff(uint32_t const nBuffSize) noexcept;
    /// @brief Free memory
    void _DelBuff() noexcept;
};
//********************************/
}  // namespace isoftkv
}  // namespace per
}  // namespace ara

#endif
