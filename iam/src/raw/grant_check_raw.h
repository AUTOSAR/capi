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
/// @file       grant_check_raw.h
/// @brief      IAM-com module logic processing function
/// @details
/// @date       2025-04-18
/// @author     Han Yuxin
/// @version    1.2.0
///
/// ================================================================
///
/// @par Modification Log:
/// <table>
/// <tr><th>Date        <th>Version  <th>Author     <th>Description
/// <tr><td>2025-04-18 <td>1.0.0 <td>Han Yuxin <td>Refactored based on
/// </table>
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/IAM/IAM-COM
/// @interface_level=module
/// @trace_id_sr=SR_IAM_00201
/// @unit_name=IAM_COM
/// @unit_description=Check verification provided by IAM for the Com module: Raw
/// @endcode
///
/// ================================================================
///
/// rights reserved.
///
/// ================================================================

#ifndef ARA_IAM_RAW_GRANT_CHECK_RAW_H_
#define ARA_IAM_RAW_GRANT_CHECK_RAW_H_
#include <ara/core/result.h>
#include <ara/core/string.h>

#include "config_iam_raw.h"
namespace ara {
namespace iam {
namespace internal {
namespace com {
//********************************/
/// @brief IAM-RAW checker
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = ad
/// @endcode
class PGrantCheck_Raw final
{
public:
    /// @brief Shared pointer
    using Uptr = std::shared_ptr< PGrantCheck_Raw >;

public:
    /// @brief PGrantCheck_Raw constructor.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = ad
    /// @endcode
    PGrantCheck_Raw() = default;

    /// @brief PGrantCheck_Raw Destruction.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = ad
    /// @endcode
    ~PGrantCheck_Raw() = default;

    /// @brief move constructor of PGrantCheck_Raw.
    /// @param other reference of object.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = ad
    /// @endcode
    explicit PGrantCheck_Raw(PGrantCheck_Raw &&other) = delete;

    /// @brief copy constructor of PGrantCheck_Raw.
    /// @param other reference of object.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = ad
    /// @endcode
    explicit PGrantCheck_Raw(PGrantCheck_Raw const &other) = delete;

    /// @brief assignment operator of PGrantCheck_Raw.
    /// @param other reference of object.
    /// @returns reference of this object.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = ad
    /// @endcode
    PGrantCheck_Raw &operator=(PGrantCheck_Raw const &other) = delete;

    /// @brief move assignment operator of PGrantCheck_Raw.
    /// @param other reference of object.
    /// @returns reference of this object.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = ad
    /// @endcode
    PGrantCheck_Raw &operator=(PGrantCheck_Raw &&other) = delete;

    /// @brief Load Raw configuration data
    /// @brief com init  load  data .

    /// @return load success/falied.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = ad
    /// @endcode
    bool Initialize() noexcept;
    /// @brief Clear  All GrantInfo
    /// @returns true
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = ad
    /// @endcode
    bool Deinitialize() noexcept;
    /// @brief Whether initialization is successful
    /// @return true if has init manifest sucess false otherwise
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    bool IsReady() const noexcept;
    /// @brief Return error code
    /// @return Error code
    /// @interface_level=unit
    /// @needwork = dda
    /// @endcode
    ara::iam::internal::grant::GrantSerializationErrc GetErrorCode() const noexcept;

public:
    /// @brief grant Event check.
    /// @param grantObject event struct
    /// @return  event check success or failed.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_IAM_00103
    /// @needwork = ad
    /// @endcode
    bool GrantCheck_RawDataStream(PIamConfigData_RawDataStreamGrant const &grantObject) const noexcept;

private:
    PConfigIam_Raw iamConfigRaw_{};
};
//********************************/
}  // namespace com
}  // namespace internal
}  // namespace iam
}  // namespace ara
#endif  // ARA_IAM_RAW_GRANT_CHECK_RAW_H_