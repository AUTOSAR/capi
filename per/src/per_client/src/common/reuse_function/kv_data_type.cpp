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
/// @file       kv_data_type.cpp
/// @brief      AutoSar-AP data persistence storage module
/// @details    Centralized definition of KV storage related data structures
/// @date       2021-05-19
/// @author     hanjingjing
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author      <th>Description
/// <tr><td>2021-05-19  <td>1.0.0    <td>hanjingjing      <td>Create initial version
/// <tr><td>2021-07-13  <td>1.0.0    <td>hanjingjing      <td>Modify valid PageID starting from 1, 0 invalid
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

#include "ara/per/internal/isoftkv/kv_data_type.h"

namespace ara {
namespace per {
namespace isoftkv {
//********************************/

/// @brief
/// @tparam T
template < typename T >
class T_DataType final
{
public:
    /// @brief
    /// @return
    static EDataType GetDataType() noexcept { return EDataType::kDefault; }
};
/// @brief
template <>
class T_DataType< int32_t > final
{
public:
    /// @brief
    /// @return
    static EDataType GetDataType() noexcept { return EDataType::kInt32; }
};
/// @brief
template <>
class T_DataType< uint32_t > final
{
public:
    /// @brief
    /// @return
    static EDataType GetDataType() noexcept { return EDataType::kUint32; }
};
//********************************/
}  // namespace isoftkv
}  // namespace per
}  // namespace ara