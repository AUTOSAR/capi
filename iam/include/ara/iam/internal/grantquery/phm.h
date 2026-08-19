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
/// @file       phm.h
/// @brief      phm interface.
/// @details
/// @date       2022-09-13
/// @author     jzy
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author       <th>Description
/// <tr><td> <td> <td> <td>Created initial version
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/IAM/IAM-PHM
/// @interface_level=software
/// @trace_id_sr=SR_IAM_00302
/// @unit_name=IAM_PHM
/// @unit_description=Verification interface provided by IAM for the PHM module
/// @endcode
///
/// ================================================================
///
/// rights reserved.
///
/// ================================================================

#ifndef ARA_IAM_PHM_GRANT_QUERY_INTERFACE_H_
#define ARA_IAM_PHM_GRANT_QUERY_INTERFACE_H_

#include "ara/iam/internal/grant/serialization_error_domain.h"
#include "ara/iam/internal/phm/iam_grant_phm.h"

namespace ara {
namespace iam {
namespace internal {
namespace grant {

/// @brief Interface for the IAMGrantQueryClient
/// This class is for phm grant check impl.
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = ad
/// @endcode
using IAMGrantPhmQuery = phm::PIamGrant_Phm;

}  // namespace grant
}  // namespace internal
}  // namespace iam
}  // namespace ara

#endif
