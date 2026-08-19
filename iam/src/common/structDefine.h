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
/// @file       structDefine.h
/// @brief      struct_define
/// @details
/// @date       2022-08-18
/// @author     jzy
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author       <th>Description
/// <tr><td>2022-08-18 <td> <td>jzy <td>Created initial version
/// <tr><td>2025-04-18 <td> <td>Han Yuxin <td>Refactored logic
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/IAM-COM
/// @interface_level=software
/// @trace_id_sr=SR_IAM_00102
/// @unit_name=IAM_COM
/// @unit_description=Provided by IAM
/// @endcode
///
/// ================================================================
///
/// rights reserved.
///
/// ================================================================

#ifndef ARA_IAM_COMMON_STRUCT_DEFINE_H_
#define ARA_IAM_COMMON_STRUCT_DEFINE_H_
#include <ara/core/string.h>

#include <cstdint>

namespace ara {
namespace iam {
namespace internal {
namespace common {

/// @brief Access Control Status class
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_sr=SR_IAM_00101
/// @needwork = ad
/// @endcode
enum class AccessControlEnable : std::uint8_t
{
    kIamMode_AccessControl_Null  = 1,
    kIamMode_AccessControl_False = 2,
    kIamMode_AccessControl_True  = 3
};

/// @brief the enum of  fire type.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_sr=SR_IAM_00501
/// @needwork = ad
/// @endcode
enum class EFileDirectoryType : std::uint8_t
{
    kComFileDirectory     = 0,
    kCryptoFileDirectory  = 1,
    kPhmFileDirectory     = 2,
    kIdsmFileDirectory    = 3,
    kRawFileDirectory     = 4,
    kMachineFileDirectory = 5
};
}  // namespace common
}  // namespace internal
}  // namespace iam
}  // namespace ara

#endif