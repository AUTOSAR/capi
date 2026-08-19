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
/// @file       manager_value.h
/// @brief      AutoSar-AP Data Persistence Storage Module
/// @details    Manage individual Value pages
/// @date       2021-06-30
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/per/KV Engine/Value Page Management
/// @interface_level=module
/// @trace_id_sr=
/// @unit_name=PManagerValue
/// @unit_description=Manage individual Value pages
/// @endcode
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// | 2021-06-30 | 1.0.0   | hanjingjing  | Initial version created                   |
///
/// ================================================================

#ifndef ARA_PER_PHKV_MANAGER_VALUE_H_
#define ARA_PER_PHKV_MANAGER_VALUE_H_

#include <functional>

#include "ara/per/internal/isoftkv/kv_object.h"
#include "ara/per/internal/isoftkv/manager_cache.h"
#include "ara/per/internal/isoftkv/manager_page.h"
#include "ara/per/internal/isoftkv/page_opt_value.h"

namespace ara {
namespace per {
namespace isoftkv {
//********************************/
/// @brief Execution class for cache operations
class PManagerValue : public PKvObject
{
private:
    /// @brief
    PManagerCache &managerCache_;
    /// @brief
    PManagerPage &managerPage_;

public:
    /// @brief
    /// @param a
    PManagerValue(PManagerValue const &a) = delete;
    /// @brief
    /// @param a
    /// @return
    PManagerValue &operator=(PManagerValue const &a) = delete;
    /// @brief
    /// @param a
    PManagerValue(PManagerValue &&a) = delete;
    /// @brief
    /// @param a
    /// @return
    PManagerValue &operator=(PManagerValue &&a) = delete;
    /// @brief
    /// @param managerCache
    /// @param managerPage
    explicit PManagerValue(PManagerCache &managerCache, PManagerPage &managerPage) noexcept;
    /// @brief
    ~PManagerValue() noexcept override = default;
    /// @brief Check if PManagerValue is ready for read/write
    /// @return
    bool IsAccessReady() const noexcept override;
    /// @brief Get the KV library name displayed in the log system
    /// @return
    ara::core::StringView GetLogKvName() const noexcept override;

public:
    /// @brief Save Local data
    /// @param pBData
    /// @param nDataLen
    /// @param eDataType
    /// @param pSavePageValue
    /// @return
    PBlockIndex SetBlockValue(uint8_t const *const pBData,
                              int32_t const nDataLen,
                              EDataType const eDataType,
                              PCachePagePtr const &pSavePageValue) noexcept;
    /// @brief Save external page data
    /// @param pBData
    /// @param nDataLen
    /// @param eDataType
    /// @param nCurPageID
    /// @return
    PBlockIndex SetBlockValue(uint8_t const *const pBData,
                              int32_t const nDataLen,
                              EDataType const eDataType,
                              uint32_t const nCurPageID) noexcept;
    /// @brief Delete the Value value
    /// @param blockFirst
    /// @return
    bool DelBlockValue(PBlockIndex const &blockFirst) noexcept;
    /// @brief Read data: return value is the total data length
    /// @param blockFirst
    /// @param pBuff
    /// @param nMaxBuffLen
    /// @return
    int32_t GetBlockValue(PBlockIndex const &blockFirst,
                          uint8_t *const pBuff,
                          int32_t const nMaxBuffLen) const noexcept;
    /// @brief Read data: return value is the total data length
    /// @param blockFirst
    /// @param pfun
    /// @return
    int32_t GetBlockValue(PBlockIndex const &blockFirst, PPageOptBase::CB_ReadData const &pfun) const noexcept;
    /// @brief Get the total length of the Value corresponding to BlockIndex
    /// @param blockFirst
    /// @param pReturnDataType
    /// @return
    int32_t GetValueTotalLen(PBlockIndex const &blockFirst, int32_t *const pReturnDataType) noexcept;

protected:
    /// @brief Iterate over the list of external Value data
    /// @param blockFirst
    /// @param pfun
    /// @return
    bool _ForEachSpreadData(
        PBlockIndex const &blockFirst,
        std::function< PBlockIndex(PPageOptValue &pageOptValue, uint16_t const nBlockID) > const &pfun) const noexcept;

protected:
private:
};
//********************************/
}  // namespace isoftkv
}  // namespace per
}  // namespace ara

#endif
