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
/// @file       iam_grant_raw.h
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

#ifndef ARA_IAM_RAW_IAM_GRANT_RAW_H_
#define ARA_IAM_RAW_IAM_GRANT_RAW_H_

#include <ara/core/result.h>
#include <ara/core/string.h>

#include <cstdint>
namespace ara {
namespace iam {
namespace internal {
namespace com {
//********************************/
/// @brief Interface for the IAMGrantQueryClient. This class establishes grant
/// init and check process.
/// @code{.isoft}
/// @interface_level=unit
/// @needwork = ad
/// @endcode
class PIamGrant_Raw final
{
public:
    /// @brief Construct a new PIamGrant_Raw object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = ad
    /// @endcode
    PIamGrant_Raw() = default;

    /// @brief Destroy the PIamGrant_Raw object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = ad
    /// @endcode
    ~PIamGrant_Raw() = default;

    /// @brief copy constructor of PIamGrant_Raw.
    /// @param other reference of object.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = ad
    /// @endcode
    explicit PIamGrant_Raw(PIamGrant_Raw &other) = delete;

    /// @brief move constructor of PIamGrant_Raw
    /// @param other reference of object.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = ad
    /// @endcode
    explicit PIamGrant_Raw(PIamGrant_Raw &&other) = delete;

    /// @brief assignment operator of PIamGrant_Raw.
    /// @param other reference of object.
    /// @returns reference of this object.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = ad
    /// @endcode
    PIamGrant_Raw &operator=(PIamGrant_Raw const &other) = delete;

    /// @brief move assignment operator of PIamGrant_Raw.
    /// @param other reference of object.
    /// @returns reference of this object.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = ad
    /// @endcode
    PIamGrant_Raw &operator=(PIamGrant_Raw &&other) = delete;

    /// @brief grant init store
    /// @returns bool
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_sr=SR_IAM_00101
    /// @needwork = ad
    /// @endcode
    static ara::core::Result< void > Initialize() noexcept;

    /// @brief Clear  All GrantInfo
    /// @returns Deinitialization bool result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = ad
    /// @endcode
    static bool Deinitialize() noexcept;

public:
    /// @brief Checks if the given grant exists
    /// @param pid raw client id
    /// @param stIp ip
    /// @param nTcpPort tcp port
    /// @param nUdpPort udp port
    /// @param nMulCastUdpPort mulcast port
    /// @return  true/false
    /// @code{.isoft}
    /// @interface_level=unit
    /// @needwork = ad
    /// @endcode
    static bool HasRawGrant(uint32_t const &pid,
                            ara::core::String const &stIp,
                            uint32_t const &nTcpPort,
                            uint32_t const &nUdpPort,
                            uint32_t const &nMulCastUdpPort) noexcept;
};
//********************************/
}  // namespace com
}  // namespace internal
}  // namespace iam
}  // namespace ara

#endif  // ARA_IAM_RAW_IAM_GRANT_RAW_H_
