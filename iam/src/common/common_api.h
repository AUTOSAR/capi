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
/// @file       common_api.h
/// @brief      AutoSar-AP
/// @details    Some common data
/// @date       2025-04-15
/// @author     Han Yuxin
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author          <th>Description
/// <tr><td>2025-04-15 <td>0.1 <td>CheJinzhao <td>Created initial version
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/IAM/IAM-COMMON
/// @interface_level=Unit
/// @trace_id_sr=SR_IAM_
/// @unit_name=PAraCommon
/// @endcode
///
/// ================================================================
///
/// rights reserved.
///
/// ================================================================

#ifndef ARA_IAM_COMMON_PUHUA_COMMON_API_H_
#define ARA_IAM_COMMON_PUHUA_COMMON_API_H_

#include <ara/core/string.h>

#include <cstddef>
#include <cstdint>
#include <memory>

namespace ara {
namespace iam {
namespace internal {
namespace common {
//********************************/
/// @brief Generate singleton (non-thread-safe)
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = dd
/// @endcode
template < class T_Class >
std::shared_ptr< T_Class > MakeSinglePtr()
{
    static std::shared_ptr< T_Class > sS_ObjectPtr{nullptr};
    if (sS_ObjectPtr) {
        return sS_ObjectPtr;
    };
    sS_ObjectPtr = std::make_shared< T_Class >();
    return sS_ObjectPtr;
}
/// @brief Convert PID to ProcessName
/// @param pid Process ID
/// @return Process name FQN
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = dd
/// @endcode
ara::core::String Pid2Name(uint32_t const &pid) noexcept;
//********************************/
}  // namespace common
}  // namespace internal
}  // namespace iam
}  // namespace ara
#endif
