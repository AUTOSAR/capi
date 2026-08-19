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
/// @file       ara_common.h
/// @brief      log
/// @details
/// @date       2024-05-13
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
/// @module_path=/IAM-COMMON
/// @interface_level=software
/// @trace_id_sr=SR_IAM_00501
/// @unit_name=IAM_Common
/// @unit_description=Provided by IAM
/// @endcode
///
/// ================================================================
///
/// rights reserved.
///
/// ================================================================

#ifndef ARA_IAM_COMMON_ARA_COMMON_H_
#define ARA_IAM_COMMON_ARA_COMMON_H_

#ifdef ARA_WITH_EXEC
    #include <ara/exec/internal/find_process_client.h>
#endif

#include <isoft/ara_fsh/filesystem_hierarchy.h>
#include <isoft/manifestreader/tps_enumeration.h>

#include "common/ara_log.h"
#include "common/structDefine.h"
namespace ara {
namespace iam {
namespace internal {
namespace common {
//********************************/
/// @brief Wrapper code related to interactions with other dependent Ara modules
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_IAM_00338
/// @trace_id_dd=DD_IAM_00415
/// @needwork = ad
/// @endcode
class PAraCommon final
{
private:
    /// @brief fsh get.

    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IAM_00000
    /// @trace_id_dd=DD_IAM_00416
    /// @needwork = dda
    /// @endcode
    isoft::ara_fsh::Platform cmPlatform_{};
    /// @brief Remote  event identify check.
#ifdef ARA_WITH_EXEC
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IAM_00000
    /// @trace_id_dd=DD_IAM_00417
    /// @needwork = dda
    /// @endcode
    ara::exec::internal::FindProcessClient fpc_{};
#endif

public:
    /// @brief  PAraCommon Constructor.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IAM_00000
    /// @trace_id_dd=DD_IAM_00418
    /// @needwork = dda
    /// @endcode
    PAraCommon() = default;

    /// @brief PAraCommon Deconstructor.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IAM_00000
    /// @trace_id_dd=DD_IAM_00419
    /// @needwork = dda
    /// @endcode
    ~PAraCommon() = default;

    /// @brief copy constructor  - default.
    /// @param other the object to be copy.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IAM_00000
    /// @trace_id_dd=DD_IAM_00420
    /// @needwork = dda
    /// @endcode
    explicit PAraCommon(PAraCommon const &other) = delete;

    /// @brief move constructor  - default.
    /// @param other the object to be move.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IAM_00000
    /// @trace_id_dd=DD_IAM_00421
    /// @needwork = dda
    /// @endcode
    explicit PAraCommon(PAraCommon &&other) = delete;

    /// @brief Copy assignment operator of the PAraCommon - not allowed.
    /// @param other the object to be copy.
    /// @return PAraCommon object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IAM_00000
    /// @trace_id_dd=DD_IAM_00422
    /// @needwork = dda
    /// @endcode
    PAraCommon &operator=(PAraCommon const &other) = delete;

    /// @brief Move assignment operator of the PAraCommon - not allowed.
    /// @param other the object to be copy.
    /// @return PAraCommon object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IAM_00000
    /// @trace_id_dd=DD_IAM_00423
    /// @needwork = dda
    /// @endcode
    PAraCommon &operator=(PAraCommon &&other) = delete;

public:
#ifdef ARA_WITH_EXEC
    /// @brief pid to fqn
    /// @param pid  pid
    /// @param proName  fqn value.
    /// @return true/false
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IAM_00000
    /// @trace_id_dd=DD_IAM_00424
    /// @needwork = dda
    /// @endcode
    ara::core::String PidTranslate(
        uint32_t const &pid,
        isoft::manifestreader::tps::TrustedPlatformExecutableLaunchBehaviorEnum const eTrustPlatform) const noexcept;
#endif
    /// @brief get all   iam configuation  dir.
    /// @param fileDir file dirctory
    /// @param type  file type
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_IAM_00000
    /// @trace_id_dd=DD_IAM_00425
    /// @needwork = dda
    /// @endcode
    ara::core::String GetIamConfigDir(EFileDirectoryType const &type) const noexcept;
};
//********************************/
}  // namespace common
}  // namespace internal
}  // namespace iam
}  // namespace ara
#endif