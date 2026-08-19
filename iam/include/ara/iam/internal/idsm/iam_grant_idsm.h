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
/// @file       iam_grant_idsm.h
/// @brief      com  interface.
/// @details
/// @date       2022-08-19
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
/// @module_path=/IAM/IAM-IDSM
/// @interface_level=software
/// @trace_id_sr=SR_IAM_00401
/// @unit_name=IAM_IDSM
/// @unit_description=Verification interface provided by IAM for the IDSM module
/// @endcode
///
/// ================================================================
///
/// rights reserved.
///
/// ================================================================

#ifndef ARA_IAM_IDSM_IAM_GRANT_IDSM_H_
#define ARA_IAM_IDSM_IAM_GRANT_IDSM_H_
#include <ara/core/string.h>

#include <cstdint>
namespace ara {
namespace iam {
namespace internal {
namespace idsm {

/// @brief Interface for the IAMGrantQueryClient. This class establishes grant
/// init and check process.
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = ad
/// @endcode
class PIamGrant_Idsm final
{
public:
    /// @brief Construct a new PIamGrant_Idsm object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = ad
    /// @endcode
    PIamGrant_Idsm() = default;

    /// @brief Destroy the PIamGrant_Idsm object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = ad
    /// @endcode
    ~PIamGrant_Idsm() = default;

    /// @brief copy constructor of PIamGrant_Idsm.
    /// @param other reference of object.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = ad
    /// @endcode
    explicit PIamGrant_Idsm(PIamGrant_Idsm &other) = delete;

    /// @brief move constructor of PIamGrant_Idsm
    /// @param other reference of object.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = ad
    /// @endcode
    explicit PIamGrant_Idsm(PIamGrant_Idsm &&other) = delete;

    /// @brief assignment operator of PIamGrant_Idsm.
    /// @param other reference of object.
    /// @returns reference of this object.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = ad
    /// @endcode
    PIamGrant_Idsm &operator=(PIamGrant_Idsm const &other) = delete;

    /// @brief move assignment operator of PIamGrant_Idsm.
    /// @param other reference of object.
    /// @returns reference of this object.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = ad
    /// @endcode
    PIamGrant_Idsm &operator=(PIamGrant_Idsm &&other) = delete;

    /// @brief grant init store
    /// @returns bool
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = ad
    /// @endcode
    static bool Initialize() noexcept;

    /// @brief Clear  All GrantInfo
    /// @returns Deinitialization bool result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = ad
    /// @endcode
    static bool Deinitialize() noexcept;

    /// @brief Checks if the given grant exists
    /// @param stProcess idsm client pid
    /// @param nID Security event ID
    /// @returns bool.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = ad
    /// @endcode
    static bool HasIdsmGrant(ara::core::String const &stProcess, uint32_t const &nID) noexcept;
    /// @brief Checks if the given grant exists
    /// @param  nPid idsm client pid
    /// @param nID Security event ID
    /// @returns bool.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = ad
    /// @endcode
    static bool HasIdsmGrant(uint32_t const &nPid, uint32_t const &nID) noexcept;
};

}  // namespace idsm
}  // namespace internal
}  // namespace iam
}  // namespace ara

#endif  // ARA_IAM_IDSM_IAM_GRANT_IDSM_H_
