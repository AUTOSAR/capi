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
/// @file       iam_grant_phm.h
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
/// @module_path=/IAM/IAM-PHM
/// @interface_level=software
/// @trace_id_sr=SR_IAM_00301
/// @unit_name=IAM_PHM
/// @unit_description=Verification interface provided by IAM for the PHM module
/// @endcode
///
/// ================================================================
///
/// rights reserved.
///
/// ================================================================

#ifndef ARA_IAM_PHM_IAM_GRANT_PHM_H_
#define ARA_IAM_PHM_IAM_GRANT_PHM_H_
#include <ara/core/string.h>

#include <cstdint>
namespace ara {
namespace iam {
namespace internal {
namespace phm {

/// @brief Interface for the IAMGrantQueryClient. This class establishes grant
/// init and check process.
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = ad
/// @endcode
class PIamGrant_Phm final
{
public:
    /// @brief Construct a new PIamGrant_Phm object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = ad
    /// @endcode
    PIamGrant_Phm() = default;

    /// @brief Destroy the PIamGrant_Phm object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = ad
    /// @endcode
    ~PIamGrant_Phm() = default;

    /// @brief copy constructor of PIamGrant_Phm.
    /// @param other reference of object.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = ad
    /// @endcode
    explicit PIamGrant_Phm(PIamGrant_Phm &other) = delete;

    /// @brief move constructor of PIamGrant_Phm
    /// @param other reference of object.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = ad
    /// @endcode
    explicit PIamGrant_Phm(PIamGrant_Phm &&other) = delete;

    /// @brief assignment operator of PIamGrant_Phm.
    /// @param other reference of object.
    /// @returns reference of this object.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = ad
    /// @endcode
    PIamGrant_Phm &operator=(PIamGrant_Phm const &other) = delete;

    /// @brief move assignment operator of PIamGrant_Phm.
    /// @param other reference of object.
    /// @returns reference of this object.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = ad
    /// @endcode
    PIamGrant_Phm &operator=(PIamGrant_Phm &&other) = delete;

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

    /// @brief phm checkpoint grant check.
    /// @param stProcess phm client processname
    /// @param checkPointId The ID of the checkpoint.
    /// @return true/false
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = ad
    /// @endcode
    static bool HasCheckPointGrant(ara::core::String const &stProcess, uint32_t const &checkPointId) noexcept;
    /// @brief phm checkpoint grant check.
    /// @param nPid phm client pid
    /// @param checkPointId The ID of the checkpoint.
    /// @return true/false
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = ad
    /// @endcode
    static bool HasCheckPointGrant(uint32_t const &nPid, uint32_t const &checkPointId) noexcept;

    /// @brief phm channel grant check.
    /// @param stProcess phm client processname
    /// @param channelId channel id
    /// @return true/false
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = ad
    /// @endcode
    static bool HasChannelGrant(ara::core::String const &stProcess, uint32_t const &channelId) noexcept;
    /// @brief phm channel grant check.
    /// @param nPid phm client pid
    /// @param channelId channel id
    /// @return true/false
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = ad
    /// @endcode
    static bool HasChannelGrant(uint32_t const &nPid, uint32_t const &channelId) noexcept;
};

}  // namespace phm
}  // namespace internal
}  // namespace iam
}  // namespace ara

#endif  // ARA_IAM_PHM_IAM_GRANT_PHM_H_
